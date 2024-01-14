/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#include "NiiPreProcess.h"
#include "NiiInstanceGeo.h"
#include "NiiInstanceBatch.h"
#include "NiiSkeleton.h"
#include "NiiAnimationFusion.h"
#include "NiiSimdUtil.h"
#include "NiiSpaceNode.h"
#include "NiiStrConv.h"
#include "NiiCamera.h"
#include "NiiException.h"

namespace NII
{
    NameGenerator InstanceGeo::msNameGenerator(_T(""));
    //--------------------------------------------------------------------
    InstanceGeo::InstanceGeo(InstanceBatch * batchOwner, Nui32 instanceID,
        InstanceGeo * sharedTransformEntity) :
        SpaceObj(),
        mInstanceId(instanceID),
        mBatchOwner(batchOwner),
        mAnimationState(0),
        mSkeleton(0),
        mBoneWorldMatrices(0),
        mFrameAnimationLastUpdated(N_TYPE_MAX(Nul) - 1),
        mSharedTransformEntity(0),
        mTransformLookupNumber(instanceID),
        mPosition(Vector3f::ZERO),
        mDerivedLocalPosition(Vector3f::ZERO),
        mOrientation(Quaternion::IDENTITY),
        mScale(Vector3f::UNIT),
        mMaxScaleLocal(1),
        mNeedAnimTransformUpdate(true),
        mNeedTransformUpdate(true),
        mUseLocalTransform(false),
        mInUse(false)
    {
        //Use a static name generator to ensure this name stays unique (which may not happen
        //otherwise due to reparenting when defragmenting)
        //mName = batchOwner->getName() + "/InstanceGeo_" +
        //    StrConv::conv(mInstanceId) + "/"+ msNameGenerator.generate();

        if(sharedTransformEntity)
        {
            sharedTransformEntity->shareTransformWith(this);
        }
        else
        {
            createSkeletonInstance();
        }
        updateTransforms();
    }
    //---------------------------------------------------------------------
    InstanceGeo::~InstanceGeo()
    {
        unlinkTransform();
        destroySkeletonInstance();
    }
    //---------------------------------------------------------------------
    bool InstanceGeo::shareTransformWith(InstanceGeo * slave)
    {
        if(!(this->mBatchOwner->_getMeshRef()->isSkeletonFusion()) ||
            this->mBatchOwner->_getMeshRef()->getSkeleton() == 0 ||
                !this->mBatchOwner->_supportsSkeletalAnimation())
        {
            return false;
        }

        if(this->mSharedTransformEntity)
        {
            N_EXCEPT(InvalidState, _I18n("Attempted to share '") + N_conv(mID) +
                _I18n("' transforms with slave '") + N_conv(slave->mID) + _I18n("' but '") +
                N_conv(mID) + _I18n("' is already sharing. Hierarchical sharing not allowed."));
            return false;
        }

        if(this->mBatchOwner->_getMeshRef()->getSkeleton() !=
            slave->mBatchOwner->_getMeshRef()->getSkeleton())
        {
            N_EXCEPT(InvalidState, _I18n("Sharing transforms requires")
                _I18n("both instanced entities to have the same skeleton"));
            return false;
        }

        slave->unlinkTransform();
        slave->destroySkeletonInstance();

        slave->mSkeleton = this->mSkeleton;
        slave->mAnimationState = this->mAnimationState;
        if(mBatchOwner->useBoneWorldMatrices())
        {
            slave->mBoneWorldMatrices = this->mBoneWorldMatrices;
        }
        slave->mSharedTransformEntity = this;
        //The sharing partners are kept in the parent entity
        this->mSharingPartners.push_back(slave);

        slave->mBatchOwner->_markTransformSharingDirty();

        return true;
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::stopSharingTransform()
    {
        if(mSharedTransformEntity)
        {
            unlinkTransform();
            createSkeletonInstance();
        }
        else
        {
            //Tell the ones sharing skeleton with us to use their own
            InstanceGeoVec::const_iterator itor = mSharingPartners.begin();
            InstanceGeoVec::const_iterator end  = mSharingPartners.end();
            while(itor != end)
            {
                (*itor)->stopSharingTransform();
                ++itor;
            }
            mSharingPartners.clear();
        }
    }
    //-----------------------------------------------------------------------
    FactoryID InstanceGeo::getFactoryID() const
    {
        return N_FACTORY_InstanceGeo;
    }
    //-----------------------------------------------------------------------
    NCount InstanceGeo::getTransforms(Matrix4f * xform) const
    {
        NCount retVal = 1;

        //When not attached, returns zero matrix to avoid rendering this one, not identity
        if(isVisible() && isActive())
        {
            if(!mSkeleton)
            {
                *xform = mBatchOwner->useBoneWorldMatrices() ?
                    getSpaceMatrix() : Matrix4f::IDENTITY;
            }
            else
            {
                const Matrix4f * matrices = mBatchOwner->useBoneWorldMatrices() ?
                    mBoneWorldMatrices : mSkeleton->getMatrix();
                const BoneIndexList * indexMap = mBatchOwner->_getIndexToBoneMap();
                BoneIndexList::const_iterator itor = indexMap->begin();
                BoneIndexList::const_iterator end  = indexMap->end();

                while(itor != end)
                    *xform++ = matrices[*itor++];

                retVal = indexMap->size();
            }
        }
        else
        {
            if(mSkeleton)
                retVal = mBatchOwner->_getIndexToBoneMap()->size();

            std::fill_n(xform, retVal, Matrix4f::AFFINE);
        }

        return retVal;
    }
    //-----------------------------------------------------------------------
    NCount InstanceGeo::getTransforms3x4(NIIf * xform) const
    {
        NCount retVal;
        //When not attached, returns zero matrix to avoid rendering this one, not identity
        if(isVisible() && isActive())
        {
            if(!mSkeleton)
            {
                const Matrix4f & mat = mBatchOwner->useBoneWorldMatrices() ?
                    getSpaceMatrix() : Matrix4f::IDENTITY;
                for( NIIi i=0; i<3; ++i)
                {
                    NIIf const * row = mat[i];
                    for(NIIi j = 0; j < 4; ++j)
                        *xform++ = static_cast<NIIf>(*row++);
                }

                retVal = 12;
            }
            else
            {
                const Matrix4f * matrices = mBatchOwner->useBoneWorldMatrices() ?
                    mBoneWorldMatrices : mSkeleton->getMatrix();

                const BoneIndexList * indexMap = mBatchOwner->_getIndexToBoneMap();
                BoneIndexList::const_iterator itor = indexMap->begin();
                BoneIndexList::const_iterator end  = indexMap->end();

                while(itor != end)
                {
                    const Matrix4f &mat = matrices[*itor++];
                    for(NIIi i=0; i<3; ++i)
                    {
                        NIIf const *row = mat[i];
                        for(NIIi j=0; j<4; ++j)
                            *xform++ = static_cast<NIIf>(*row++);
                    }
                }

                retVal = indexMap->size() * 4 * 3;
            }
        }
        else
        {
            if(mSkeleton)
                retVal = mBatchOwner->_getIndexToBoneMap()->size() * 3 * 4;
            else
                retVal = 12;

            std::fill_n(xform, retVal, 0.0f);
        }

        return retVal;
    }
    //-----------------------------------------------------------------------
    bool InstanceGeo::findVisible(Camera * camera) const
    {
        //Object is active
        bool retVal = isActive();
        if(retVal)
        {
            //check object is explicitly visible
            retVal = isVisible();

            //Object's bounding box is viewed by the camera
            if(retVal && camera)
                retVal = camera->isIntersect(Sphere(setSpacePos(), getCenterRange()));
        }

        return retVal;
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::createSkeletonInstance()
    {
        //Is mesh skeletally animated?
        if(mBatchOwner->_getMeshRef()->isSkeletonFusion() &&
            mBatchOwner->_getMeshRef()->getSkeleton() &&
                mBatchOwner->_supportsSkeletalAnimation())
        {
            mSkeleton = N_new SkeletonFusion(mBatchOwner->_getMeshRef()->getSkeleton());
            mSkeleton->load();

            if(mBatchOwner->useBoneWorldMatrices())
            {
                mBoneWorldMatrices = static_cast<Matrix4f *>(N_alloc_simd(
                    sizeof(Matrix4f) * mSkeleton->getCount()));
            }

            mAnimationState = N_new AnimationFusionObj();
            mBatchOwner->_getMeshRef()->_init(mAnimationState);
        }
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::destroySkeletonInstance()
    {
        if(mSkeleton)
        {
            //Tell the ones sharing skeleton with us to use their own
            //sharing partners will remove themselves from notifyUnlink
            while(mSharingPartners.empty() == false)
            {
                mSharingPartners.front()->stopSharingTransform();
            }
            mSharingPartners.clear();

            N_delete mSkeleton;
            mSkeleton = 0;
            
            N_delete mAnimationState;
            mAnimationState = 0;
            
            N_free_simd(mBoneWorldMatrices);
            mBoneWorldMatrices = 0;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::unlinkTransform()
    {
        if(mSharedTransformEntity)
        {
            //Tell our master we're no longer his slave
            mSharedTransformEntity->notifyUnlink(this);
            mBatchOwner->_markTransformSharingDirty();

            mSkeleton = 0;
            mAnimationState = 0;
            mBoneWorldMatrices = 0;
            mSharedTransformEntity = 0;
        }
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::notifyUnlink(const InstanceGeo * slave)
    {
        //Find the slave and remove it
        InstanceGeoVec::iterator itor = mSharingPartners.begin();
        InstanceGeoVec::iterator end  = mSharingPartners.end();
        while(itor != end)
        {
            if(*itor == slave)
            {
                std::swap(*itor,mSharingPartners.back());
                mSharingPartners.pop_back();
                break;
            }
            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    const AABox & InstanceGeo::getAABB() const
    {
        //TODO: Add attached objects (TagBones) to the bbox
        return mBatchOwner->_getMeshReference()->getAABB();
    }
    //-----------------------------------------------------------------------
    NIIf InstanceGeo::getCenterRange() const
    {
        return mBatchOwner->_getMeshReference()->getCenterRange() * getMaxScaleCoef();
    }
    //-----------------------------------------------------------------------
    NIIf InstanceGeo::distanceSquared(const Camera * cam) const
    {
        return setSpacePos().distanceSquared(cam->getSpacePos());
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::_notify()
    {
        markTransformDirty();
        SpaceObj::_notify();
        updateTransforms();
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::attach(PosNode * node, bool tag)
    {
        markTransformDirty();
        SpaceObj::attach(node, tag);
        updateTransforms();
    }
    //-----------------------------------------------------------------------
    AnimationFusion * InstanceGeo::getFusion(AnimationID id) const
    {
        if(!mAnimationState)
        {
            N_EXCEPT(NotExist, _I18n("GeometrySpace is not animated"));
        }

        return mAnimationState->getFusion(id);
    }
    //-----------------------------------------------------------------------
    AnimationFusionObj * InstanceGeo::getAnimationCtrl() const
    {
        return mAnimationState;
    }
    //-----------------------------------------------------------------------
    bool InstanceGeo::_updateAnimation(void)
    {
        if(mSharedTransformEntity)
        {
            return mSharedTransformEntity->_updateAnimation();
        }
        else
        {
            bool animationDirty = (mFrameAnimationLastUpdated !=
                mAnimationState->getFrameMark()) || (!mSkeleton->isValidBone());

            if(animationDirty || (mNeedAnimTransformUpdate && mBatchOwner->useBoneWorldMatrices()))
            {
                mSkeleton->_refresh(mAnimationState);
                mSkeleton->update();

                // Cache last parent transform for next frame use too.
                if(mBatchOwner->useBoneWorldMatrices())
                {
                    SIMDUtil::getImplementation()->affine(mBoneWorldMatrices,
                        getSpaceMatrix(), mSkeleton->getMatrix(), mSkeleton->getCount());
                    mNeedAnimTransformUpdate = false;
                }

                mFrameAnimationLastUpdated = mAnimationState->getFrameMark();

                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void InstanceGeo::markTransformDirty()
    {
        mNeedTransformUpdate = true;
        mNeedAnimTransformUpdate = true;
        mBatchOwner->_boundsDirty();
    }
    //---------------------------------------------------------------------------
    void InstanceGeo::setPos(const Vector3f & pos, bool doUpdate)
    {
        mPosition = pos;
        mDerivedLocalPosition = pos;
        mUseLocalTransform = true;
        markTransformDirty();
        if(doUpdate)
            updateTransforms();
    }
    //---------------------------------------------------------------------------
    void InstanceGeo::setOri(const Quaternion & ori, bool doUpdate)
    {
        mOrientation = ori;
        mUseLocalTransform = true;
        markTransformDirty();
        if(doUpdate)
            updateTransforms();
    }
    //---------------------------------------------------------------------------
    void InstanceGeo::setScale(const Vector3f & scale, bool doUpdate)
    {
        mScale = scale;
        mMaxScaleLocal = N_MAX(NIIf, N_MAX(NIIf,
            Math::Abs(mScale.x), Math::Abs(mScale.y)), Math::Abs(mScale.z));
        mUseLocalTransform = true;
        markTransformDirty();
        if (doUpdate) updateTransforms();
    }
    //---------------------------------------------------------------------------
    NIIf InstanceGeo::getMaxScaleCoef() const
    {
        if(mSpaceNode)
        {
            const Vector3f & parentScale = mSpaceNode->getSpaceScale();
            return mMaxScaleLocal * N_MAX(NIIf, N_MAX(NIIf,
                Math::Abs(parentScale.x), Math::Abs(parentScale.y)), Math::Abs(parentScale.z));
        }
        return mMaxScaleLocal;
    }
    //---------------------------------------------------------------------------
    void InstanceGeo::updateTransforms()
    {
        if(mUseLocalTransform && mNeedTransformUpdate)
        {
            if(mSpaceNode)
            {
                const Vector3f & parentPosition = mSpaceNode->getSpacePos();
                const Quaternion & parentOrientation = mSpaceNode->getSpaceOri();
                const Vector3f & parentScale = mSpaceNode->getSpaceScale();

                Quaternion derivedOrientation = parentOrientation * mOrientation;
                Vector3f derivedScale = parentScale * mScale;
                mDerivedLocalPosition = parentOrientation * (parentScale * mPosition) + parentPosition;

                mFullLocalTransform.set(mDerivedLocalPosition, derivedScale, derivedOrientation);
            }
            else
            {
                mFullLocalTransform.set(mPosition, mScale, mOrientation);
            }
            mNeedTransformUpdate = false;
        }
    }
    //---------------------------------------------------------------------------
    void InstanceGeo::setInUse(bool used)
    {
        mInUse = used;
        //Remove the use of local transform if the object is deleted
        mUseLocalTransform &= used;
    }
    //---------------------------------------------------------------------------
}
