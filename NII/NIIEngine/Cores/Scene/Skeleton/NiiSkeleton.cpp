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
#include "NiiSkeleton.h"

#include "NiiNodeFusion.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiSkeletonManager.h"
#include "NiiSkeletonSerializer.h"
#include "NiiStrConv.h"
#include "NiiTagBone.h"
#include "NiiKeyFrame.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Bone
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Bone::Bone(SpaceID id, Skeleton * own) :
        PosNode(id),
        mCreator(own),
        mSrcPos(Vector3f::ZERO),
        mSrcOri(Quaternion::IDENTITY),
        mSrcScale(Vector3f::UNIT),
        mName(N_StrBlank),
        mManual(false)
    {
    }
    //------------------------------------------------------------------------
    Bone * Bone::create(SpaceID id, const Vector3f & pos, const Quaternion & ori)
    {
        Bone * re = mCreator->create(id);
        re->translate(pos);
        re->rotate(ori);
        add(re);
        return re;
    }
    //------------------------------------------------------------------------
    Bone::~Bone()
    {
    }
    //------------------------------------------------------------------------
    void Bone::origin()
    {
        mSrcPos = mPos;
        mSrcOri = mOri;
        mSrcScale = mScale;

        // Save inverse derived position/scale/orientation, used for calculate offset transform later
        mSrcPosOrg = -getSpacePos();
        mSrcScaleOrg = Vector3f::UNIT / getSpaceScale();
        mSrcOriOrg = getSpaceOri().inverse();
    }
    //------------------------------------------------------------------------
    void Bone::reset()
    {
        mPos = mSrcPos;
        mOri = mSrcOri;
        mScale = mSrcScale;

        notify();
    }
    //------------------------------------------------------------------------
    const Vector3f & Bone::getOriginPos() const
    {
        return mSrcPos;
    }
    //-----------------------------------------------------------------------
    const Quaternion & Bone::getOriginOri() const
    {
        return mSrcOri;
    }
    //-----------------------------------------------------------------------
    const Vector3f & Bone::getOriginScale() const
    {
        return mSrcScale;
    }
    //------------------------------------------------------------------------
    void Bone::setManual(bool b)
    {
        mManual = b;
        mCreator->_notifyManualChange(this);
    }
    //------------------------------------------------------------------------
    void Bone::getFusion(Matrix4f * m) const
    {
        Vector3f scale = getSpaceScale() * mSrcScaleOrg;
        Quaternion rotate = getSpaceOri() * mSrcOriOrg;
        Vector3f translate = getSpacePos() + rotate * (scale * mSrcPosOrg);

        m->set(translate, scale, rotate);
    }
    //------------------------------------------------------------------------
    void Bone::notify(bool force)
    {
        PosNode::notify(force);

        if (mManual)
        {
            mCreator->_notifyManualUpdate(this);
        }
    }
    //------------------------------------------------------------------------
    Bone * Bone::cloneTo(Skeleton * dst) const
    {
        Bone * re = dst->create(getID());
        re->setOri(getOri());
        re->setPos(getPos());
        re->setScale(getScale());

        PosNode::ChildList::const_iterator i, iend = mChildList.end();
        for (i = mChildList.begin(); i != iend; ++i)
        {
            re->add(static_cast<Bone *>(i->second)->cloneTo(dst));
        }
        return re;
    }
    //------------------------------------------------------------------------
    PosNode * Bone::createImpl(SpaceID id)
    {
        return mCreator->create(id);
    }
    //------------------------------------------------------------------------
    void Bone::destroyImpl(SpaceID id)
    {
        return mCreator->destroy(id);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Skeleton
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    struct DeltaTransform
    {
        Vector3f translate;
        Quaternion rotate;
        Vector3f scale;
        bool isIdentity;
    };
    //------------------------------------------------------------------------
    Skeleton::Skeleton(LangID lid) :
        Resource(N_CmdObj_Skeleton, lid),
        mBlendMode(M_AVG),
        mValidBone(true)
    {
    }
    //------------------------------------------------------------------------
    Skeleton::Skeleton(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, LangID lid) :
            Resource(N_CmdObj_Skeleton, rid, gid, ls, rs, lid),
            mBlendMode(M_AVG)
        // set animation blending to weighted, not cumulative
    {
        if(setupCmd(N_CmdObj_Skeleton))
        {
            // no custom params
        }
        mCreator = N_OnlyPtr(Skeleton);
    }
    //------------------------------------------------------------------------
    Skeleton::~Skeleton()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();
    }
    //------------------------------------------------------------------------
    void Skeleton::loadImpl()
    {

        N_Only(Log).stream() << _I18n("Skeleton: Loading ") << mOriginID;

        DataStream * stream = N_Only(ResourceScheme).open(mSrc, mGroup, true, this);
        SkeletonSerializer serializer(stream);
        serializer.read(stream, this);
        N_delete stream;

        ExtList::iterator i, iend = mExtList.end();
        for(i = mExtList.begin(); i != iend; ++i)
        {
            i->mSkeleton = static_cast<Skeleton *>(N_Only(Skeleton).load(i->mSkeletonID, mGroup));
            i->mSkeleton->ref();
        }
    }
    //------------------------------------------------------------------------
    void Skeleton::unloadImpl()
    {
        BoneList::iterator i, iend = mBoneList.end();
        for(i = mBoneList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mBoneList.clear();
        mRootBones.clear();
        mManualBones.clear();
        mValidBone = true;

        removeAllAnimations();
        ExtList::iterator j, jend = mExtList.end();
        for(j = mExtList.begin(); j != jend; ++j)
        {
            if(j->mSkeleton)
            {
                j->mSkeleton->unref();
                j->mSkeleton = 0;
            }
        }
        mExtList.clear();
    }
    //------------------------------------------------------------------------
    NCount Skeleton::calcSize() const
    {
        return 0; //todo
    }
    //------------------------------------------------------------------------
    Bone * Skeleton::create(Nidx id)
    {
        if(id >= NII_MAX_NUM_BONES)
        {
            N_EXCEPT(InvalidParam, _I18n("超出骨骼中的骨头限制!"));
        }
        if(id < mBoneList.size() && mBoneList[id] != NULL)
        {
            N_EXCEPT(UniqueRepeat, _I18n("骨头已经存在,ID: ") + N_conv(id));
        }

        Bone * re = N_new Bone(id, this);
        if(mBoneList.size() <= id)
        {
            mBoneList.resize(id + 1, 0);
        }
        mBoneList[id] = re;
        return re;
    }
    //------------------------------------------------------------------------
    void Skeleton::destroy(Nidx id)
    {
        if(id < mBoneList.size() && mBoneList[id] != NULL)
        {
            N_delete mBoneList[id];
            mBoneList[id] = 0;
        }
    }
    //------------------------------------------------------------------------
    Bone * Skeleton::get(Nidx index) const
    {
        N_assert(index < mBoneList.size(), "Index out of bounds");
        return mBoneList[index];
    }
    //------------------------------------------------------------------------
    Bone * Skeleton::getRoot() const
    {
        if(mRootBones.empty() && (mBoneList.size() != 0))
        {
            return getRootBoneImpl();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    const BoneList & Skeleton::getRootList() const
    {
        if (mRootBones.empty() && (mBoneList.size() != 0))
        {
            getRootBoneImpl();
        }
        return mRootBones;
    }
    //------------------------------------------------------------------------
    void Skeleton::_refresh(AnimationFusionObj * obj)
    {
        /* 算法:
        1. 重至所有骨头位置
        2. 混合所有起用的NodeFusion
        */

        // 重至所有骨头
        reset();

        NIIf factor = 1.0f;
        NIIf total = 0.0f;
        if(mBlendMode == M_AVG)
        {
            // Derive total weights so we can rebalance if > 1.0f
            AnimationFusionObj::FusionList::const_iterator i, iend = obj->getValidFusions().end();
            for(i = obj->getValidFusions().begin(); i != iend; ++i)
            {
                AnimationFusion * nf = *i;
                if(nf->getAnimationObj() == this)
                {
                    total += nf->getWeight();
                }
            }
            if(total > 1.0f)
            {
                factor = 1.0f / total;
            }
        }

        // Per enabled animation state
        AnimationFusionObj::FusionList::const_iterator i, iend = obj->getValidFusions().end();
        for(i = obj->getValidFusions().begin(); i != iend; ++i)
        {
            if ((*i)->getAnimationObj() == this)
            {
                NodeFusion * nf = static_cast<NodeFusion *>(*i);
                nf->apply(factor);
            }
        }
    }
    //------------------------------------------------------------------------
    void Skeleton::build()
    {
        _update();
        BoneList::iterator i, iend = mBoneList.end();
        for(i = mBoneList.begin(); i != iend; ++i)
        {
            (*i)->origin();
        }
    }
    //------------------------------------------------------------------------
    void Skeleton::reset(bool all)
    {
        BoneList::iterator i, iend = mBoneList.end();
        for(i = mBoneList.begin(); i != iend; ++i)
        {
            if(!(*i)->isManual() || all)
                (*i)->reset();
        }
    }
    //------------------------------------------------------------------------
    Animation * Skeleton::createAnimation(AnimationID id, TimeDurMS dur)
    {
        Animation * re = AnimationObj::createAnimation(id, dur);
        re->setObj(this);
        return re;
    }
    //------------------------------------------------------------------------
    Animation * Skeleton::getAnimation(AnimationID id, const Skeleton::Ext *& ext) const
    {
        Animation * re = AnimationObj::getAnimation(id);

        if(re)
        {
            ext = NULL;
        }
        else
        {
            ExtList::const_iterator i, iend = mExtList.end();
            for(i = mExtList.begin(); i != iend; ++i)
            {
                if(i->mSkeleton != 0)
                {
                    re = i->mSkeleton->getAnimation(id);
                    if(re)
                    {
                        ext = &(*i);
                        break;
                    }
                }
            }
        }
        return re;
    }
    //------------------------------------------------------------------------
    Animation * Skeleton::getAnimation(AnimationID id) const
    {
        const Ext *ext;
        return getAnimation(id, ext);
    }
    //------------------------------------------------------------------------
    void Skeleton::_init(AnimationFusionObj * obj)
    {
        AnimationList::iterator i, iend = mAnimations.end();
        for(i = mAnimations.begin(); i != iend; ++i)
        {
            obj->addFusion(i->second->createFusion(this));
        }

        ExtList::iterator li, liend = mExtList.end();
        for(li = mExtList.begin(); li != liend; ++li)
        {
            if(li->mSkeleton != 0)
            {
                AnimationList::iterator j, jend = li->mSkeleton->mAnimations.end();
                for(j = li->mSkeleton->mAnimations.begin(); j != jend; ++j)
                {
                    NodeFusion * fusion = static_cast<NodeFusion *>(j->second->createFusion(li->mSkeleton));
                    fusion->setScale(li->mScale);
                    obj->addFusion(fusion);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Skeleton::_update(AnimationFusionObj * obj)
    {
        AnimationList::iterator i, iend = mAnimations.end();
        for(i = mAnimations.begin(); i != iend; ++i)
        {
            Animation * ani = i->second;
            NodeFusion * nfusion = static_cast<NodeFusion *>(obj->getFusion(ani->getID()));

            if(nfusion)
            {
                // Update length incase changed
                nfusion->setDur(ani->getDur());
                nfusion->seek(nfusion->tell());
            }
            else
            {
                obj->addFusion(N_new NodeFusion(this, ani, 0, ani->getDur()));
            }
        }

        ExtList::iterator li, liend = mExtList.end();
        for(li = mExtList.begin(); li != liend; ++li)
        {
            if(li->mSkeleton != 0)
            {
                AnimationList::iterator j, jend = li->mSkeleton->mAnimations.end();
                for(j = li->mSkeleton->mAnimations.begin(); j != jend; ++j)
                {
                    Animation * ani = j->second;
                    NodeFusion * nfusion = static_cast<NodeFusion *>(obj->getFusion(ani->getID()));
                    if(nfusion)
                    {
                        // Update length incase changed
                        nfusion->setDur(ani->getDur());
                        nfusion->seek(nfusion->tell());
                    }
                    else
                    {
                        nfusion = static_cast<NodeFusion *>(ani->createFusion(li->mSkeleton));
                        nfusion->setScale(li->mScale);
                        obj->addFusion(nfusion);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Skeleton::_append(AnimationFusionObj * obj)
    {

    }
    //------------------------------------------------------------------------
    void Skeleton::_notifyManualUpdate(Bone * obj)
    {
        mValidBone = false;
    }
    //------------------------------------------------------------------------
    void Skeleton::_notifyManualChange(Bone * obj)
    {
        if(obj->isManual())
            mManualBones.insert(obj);
        else
            mManualBones.erase(obj);
    }
    //------------------------------------------------------------------------
    void Skeleton::getFusion(Matrix4f * out) const
    {
        const_cast<Skeleton *>(this)->_update();

        BoneList::const_iterator i, iend = mBoneList.end();
        for(i = mBoneList.begin();i != iend; ++i)
        {
            Bone * b = *i;
            b->getFusion(out);
            out++;
        }
    }
    //------------------------------------------------------------------------
    Bone * Skeleton::getRootBoneImpl() const
    {
        mRootBones.clear();
        BoneList::const_iterator i = mBoneList.begin(), iend = mBoneList.end();
        if(i == iend)
            return 0;

        for(; i != iend; ++i)
        {
            Bone * b = *i;
            if(b->getParent() == 0)
            {
                mRootBones.push_back(b);
            }
        }
        return mRootBones[0];
    }
    //------------------------------------------------------------------------
    void Skeleton::_update()
    {
        BoneList::iterator i, iend = mRootBones.end();
        for(i = mRootBones.begin(); i != iend; ++i)
        {
            (*i)->update(false, true);
        }
        mValidBone = true;
    }
    //------------------------------------------------------------------------
    void Skeleton::optimise()
    {
        AnimationList::iterator i, iend = mAnimations.end();
        for(i = mAnimations.begin(); i != iend; ++i)
        {
            i->second->optimise();
        }
    }
    //------------------------------------------------------------------------
    void Skeleton::addExt(ResourceID rid, NIIf scale)
    {
        if(rid == getOriginID())
            return;

        ExtList::iterator i, iend = mExtList.end();
        for(i = mExtList.begin();i != iend; ++i)
        {
            if(rid == i->mSkeletonID)
                return;
        }

        if(mState == LS_LOADED)
        {
            Skeleton * obj = static_cast<Skeleton *>(N_Only(Skeleton).load(rid, mGroup));
            mExtList.push_back(Skeleton::Ext(rid, scale, obj));
            obj->ref();
        }
        else
        {
            // Load later
            mExtList.push_back(Skeleton::Ext(rid, scale));
        }
    }
    //------------------------------------------------------------------------
    void Skeleton::removeAllExt()
    {
        ExtList::iterator i, iend = mExtList.end();
        for(i = mExtList.begin(); i != iend; ++i)
        {
            if(i->mSkeleton)
            {
                i->mSkeleton->unref();
                i->mSkeleton = 0;
            }
        }
        mExtList.clear();
    }
    //------------------------------------------------------------------------
    NCount Skeleton::getWorkFactor() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
/*     void Skeleton::_mergeSkeletonAnimations(const Skeleton * src,
        const IDList & bidmap, const IDList & animap)
    {
        NCount handle;

        NCount numSrcBones = src->mBoneList.size();
        NCount numDstBones = mBoneList.size();

        if(bidmap.size() != numSrcBones)
        {
            N_EXCEPT(InvalidParam, _I18n("Number of bones in the bone handle map must equal to number of bones in the source skeleton."));
        }

        bool existsMissingBone = false;

        // Check source skeleton structures compatible with ourself (that means
        // identically bones with identical handles, and with same hierarchy, but
        // not necessary to have same number of bones and bone names).
        for(handle = 0; handle < numSrcBones; ++handle)
        {
            const Bone * srcBone = src->get(handle);
            Nidx dstHandle = bidmap[handle];

            // Does it exists in target skeleton?
            if(dstHandle < numDstBones)
            {
                Bone * destBone = this->get(dstHandle);

                // Check both bones have identical parent, or both are root bone.
                const Bone * srcParent = static_cast<Bone*>(srcBone->getParent());
                Bone * destParent = static_cast<Bone*>(destBone->getParent());
                if((srcParent || destParent) && (!srcParent || !destParent ||
                     bidmap[srcParent->getID()] != destParent->getID()))
                {
                    N_EXCEPT(InvalidParam,
                    _I18n("Source skeleton incompatible with this skeleton:") +
                        _I18n("difference hierarchy between bone '") + srcBone->getName() +
                            _I18n("' and '") + destBone->getName() + "'.");
                }
            }
            else
            {
                existsMissingBone = true;
            }
        }

        // 需要复制骨头
        if(existsMissingBone)
        {
            // Create missing bones
            for(handle = 0; handle < numSrcBones; ++handle)
            {
                const Bone * srcBone = src->get(handle);
                Nid dstHandle = bidmap[handle];

                // The bone is missing in target skeleton?
                if(dstHandle >= numDstBones)
                {
                    Bone * dstBone = create(dstHandle);
                    // Sets initial transform
                    dstBone->setPos(srcBone->getOriginPos());
                    dstBone->setOri(srcBone->getOriginOri());
                    dstBone->setScale(srcBone->getOriginScale());
                }
            }

            // Link new bones to parent
            for(handle = 0; handle < numSrcBones; ++handle)
            {
                const Bone * srcBone = src->get(handle);
                Nidx dstHandle = bidmap[handle];

                // Is new bone?
                if(dstHandle >= numDstBones)
                {
                    const Bone * srcParent = static_cast<Bone *>(srcBone->getParent());
                    if(srcParent)
                    {
                        Bone * destParent = get(bidmap[srcParent->getID()]);
                        Bone * dstBone = get(dstHandle);
                        destParent->add(dstBone);
                    }
                }
            }

            // Derive root bones in case it was changed
            this->getRootBoneImpl();

            // Reset binding pose for new bones
            reset(true);
            build();
        }

        //
        // We need to adapt animations from source to target skeleton, but since source
        // and target skeleton bones bind transform might difference, so we need to alter
        // keyframes in source to suit to target skeleton.
        //
        // For any given animation time, formula:
        //
        //      LocalTransform = BindTransform * KeyFrame;
        //      DerivedTransform = ParentDerivedTransform * LocalTransform
        //
        // And all derived transforms should be keep identically after adapt to
        // target skeleton, Then:
        //
        //      DestDerivedTransform == SrcDerivedTransform
        //      DestParentDerivedTransform == SrcParentDerivedTransform
        // ==>
        //      DestLocalTransform = SrcLocalTransform
        // ==>
        //      DestBindTransform * DestKeyFrame = SrcBindTransform * SrcKeyFrame
        // ==>
        //      DestKeyFrame = inverse(DestBindTransform) * SrcBindTransform * SrcKeyFrame
        //
        // We define (inverse(DestBindTransform) * SrcBindTransform) as 'delta-transform' here.
        //

        // Calculate delta-transforms for all source bones.
        vector<DeltaTransform>::type deltaTransforms(numSrcBones);
        for(handle = 0; handle < numSrcBones; ++handle)
        {
            const Bone * srcBone = src->get(handle);
            DeltaTransform & deltaTransform = deltaTransforms[handle];
            Nidx dstHandle = bidmap[handle];

            if(dstHandle < numDstBones)
            {
                // Common bone, calculate delta-transform

                Bone * dstBone = this->get(dstHandle);

                deltaTransform.translate = srcBone->getOriginPos() - dstBone->getOriginPos();
                deltaTransform.rotate = dstBone->getOriginOri().inverse() * srcBone->getOriginOri();
                deltaTransform.scale = srcBone->getOriginScale() / dstBone->getOriginScale();

                // Check whether or not delta-transform is identity
                const NIIf tolerance = 1e-3f;
                Vector3f axis;
                Radian angle;
                deltaTransform.rotate.to(angle, axis);
                deltaTransform.isIdentity =
                    deltaTransform.translate.equal(Vector3f::ZERO, tolerance) &&
                        deltaTransform.scale.equal(Vector3f::UNIT, tolerance) &&
                            Math::isEqual(angle.getValue(), 0.0f, tolerance);
            }
            else  //why not use srcBone data
            {
                // New bone, the delta-transform is identity

                deltaTransform.translate = Vector3f::ZERO;
                deltaTransform.rotate = Quaternion::IDENTITY;
                deltaTransform.scale = Vector3f::UNIT;
                deltaTransform.isIdentity = true;
            }
        }

        // Now copy animations

        NCount numAnimations;
        if(animap.empty())
            numAnimations = src->getAnimationCount();
        else
            numAnimations = animap.size();

        for(Nidx i = 0; i < numAnimations; ++i)
        {
            const Animation * srcAnimation;
            if (animap.empty())
            {
                // Get animation of source skeleton by the given index
                srcAnimation = src->getAnimation(i);
            }
            else
            {
                // Get animation of source skeleton by the given name
                const Skeleton::Ext * linker = 0;
                srcAnimation = src->getAnimation(animap[i], &linker);
                if(!srcAnimation || linker)
                {
                    N_EXCEPT(NotExist, _I18n("No animation entry found named ") + animap[i]);
                }
            }

            // Create target animation
            Animation * dstAnimation = this->createAnimation(srcAnimation->getID(),
                srcAnimation->getDur());

            // Copy interpolation modes
            dstAnimation->setMode(srcAnimation->getMode());
            dstAnimation->setRotationMode(srcAnimation->getRotationMode());

            // Copy track for each bone
            for(handle = 0; handle < numSrcBones; ++handle)
            {
                const DeltaTransform & deltaTransform = deltaTransforms[handle];
                Nid dstHandle = bidmap[handle];

                if (srcAnimation->isExist(handle))
                {
                    // Clone track from source animation

                    const NodeKeyFrameTrack * srcTrack = srcAnimation->get(handle);
                    NodeKeyFrameTrack * dstTrack = N_new NodeKeyFrameTrack(dstAnimation, dstHandle);
                    dstTrack->setTarget(this->get(dstHandle));
                    dstTrack->setShortPath(srcTrack->getShortPath());

                    NCount numKeyFrames = srcTrack->getCount();
                    for(Nidx k = 0; k < numKeyFrames; ++k)
                    {
                        const NodeKeyFrame * srcKeyFrame = static_cast<NodeKeyFrame *>(srcTrack->get(k));
                        NodeKeyFrame * dstKeyFrame = static_cast<NodeKeyFrame *>(dstTrack->create(srcKeyFrame->getTime()));

                        // Adjust keyframes to match target binding pose
                        if (deltaTransform.isIdentity)
                        {
                            dstKeyFrame->setTranslate(srcKeyFrame->getTranslate());
                            dstKeyFrame->setRotation(srcKeyFrame->getRotation());
                            dstKeyFrame->setScale(srcKeyFrame->getScale());
                        }
                        else
                        {
                            dstKeyFrame->setTranslate(deltaTransform.translate + srcKeyFrame->getTranslate());
                            dstKeyFrame->setRotation(deltaTransform.rotate * srcKeyFrame->getRotation());
                            dstKeyFrame->setScale(deltaTransform.scale * srcKeyFrame->getScale());
                        }
                        dstKeyFrame->buildSpline();
                    }
                }
                else if(!deltaTransform.isIdentity)
                {
                    // Create 'static' track for this bone
                    NodeKeyFrameTrack * dstTrack = N_new NodeKeyFrameTrack(dstAnimation, dstHandle);
                    dstTrack->setTarget(this->get(dstHandle));
                    NodeKeyFrame * dstKeyFrame;

                    dstKeyFrame = static_cast<NodeKeyFrame *>(dstTrack->create(0));
                    dstKeyFrame->setTranslate(deltaTransform.translate);
                    dstKeyFrame->setRotation(deltaTransform.rotate);
                    dstKeyFrame->setScale(deltaTransform.scale);

                    dstKeyFrame = static_cast<NodeKeyFrame *>(dstTrack->create(dstAnimation->getDur()));
                    dstKeyFrame->setTranslate(deltaTransform.translate);
                    dstKeyFrame->setRotation(deltaTransform.rotate);
                    dstKeyFrame->setScale(deltaTransform.scale);
                    dstKeyFrame->buildSpline();
                }
            }
        }
    } */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SkeletonFusion
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SkeletonFusion::SkeletonFusion(Skeleton * obj, LangID lid) :
        Skeleton(lid),
        mSkeleton(obj),
        mFreeTagIndex(0)
    {
        mBoneSpace = static_cast<Matrix4f *>(
            N_alloc_simd(sizeof(Matrix4f) * mSkeleton->getCount()));
    }
    //-------------------------------------------------------------------------
    SkeletonFusion::~SkeletonFusion()
    {
        // have to call this here rather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        // ...and calling it in Skeleton destructor does not unload
        // SkeletonFusion since it has seized to be by then.
        unload();
        N_free_simd(mBoneSpace);
        mBoneSpace = 0;
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::update()
    {
        mSkeleton->getFusion(mBoneSpace);
    }
    //-------------------------------------------------------------------------
    NCount SkeletonFusion::getAnimationCount() const
    {
        return mSkeleton->getAnimationCount();
    }
    //-------------------------------------------------------------------------
    Animation * SkeletonFusion::getAnimation(Nidx index) const
    {
        return mSkeleton->getAnimation(index);
    }
    //-------------------------------------------------------------------------
    Animation * SkeletonFusion::getAnimation(AnimationID id, const Skeleton::Ext *& ext) const
    {
        return mSkeleton->getAnimation(id, ext);
    }
    //-------------------------------------------------------------------------
    Animation * SkeletonFusion::createAnimation(AnimationID id, TimeDurMS dur)
    {
        return mSkeleton->createAnimation(id, dur);
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::removeAnimation(AnimationID id)
    {
        mSkeleton->removeAnimation(id);
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::addExt(ResourceID rid, NIIf scale)
    {
        mSkeleton->addExt(rid, scale);
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::removeAllExt()
    {
        mSkeleton->removeAllExt();
    }
    //-------------------------------------------------------------------------
    const Skeleton::ExtList & SkeletonFusion::getExtList() const
    {
        return mSkeleton->getExtList();
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::_init(AnimationFusionObj * obj)
    {
        mSkeleton->_init(obj);
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::_update(AnimationFusionObj * obj)
    {
        mSkeleton->_update(obj);
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::loadImpl()
    {
        mBlendMode = mSkeleton->getBlendMode();
        mFreeTagIndex = 0;

        const BoneList & blist = mSkeleton->getRootList();
        BoneList::const_iterator i, iend = blist.end();
        for(i = blist.begin(); i != iend; ++i)
        {
            Bone * b = *i;
            mRootBones.push_back(b->cloneTo(this));
            b->update(false, true);
        }
        build();
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::unloadImpl()
    {
        Skeleton::unloadImpl();

        TagBoneList::const_iterator it, itend = mTagList.end();
        for(it = mTagList.begin(); it != itend; ++it)
        {
            N_delete *it;
        }
        mTagList.clear();

        TagBoneList::const_iterator it2, it2end = mFreeTagList.end();
        for(it2 = mFreeTagList.begin(); it2 != it2end; ++it2)
        {
            N_delete *it2;
        }
        mFreeTagList.clear();
    }
    //-------------------------------------------------------------------------
    TagBone * SkeletonFusion::attach(Bone * bone, const Vector3f & pos, const Quaternion & ori)
    {
        TagBone * re;
        if(mFreeTagList.empty())
        {
            re = N_new TagBone(mFreeTagIndex++, this);
            mTagList.push_back(re);
        }
        else
        {
            re = mFreeTagList.front();
            mTagList.splice(mTagList.end(), mFreeTagList, mFreeTagList.begin());
            // Initial some members ensure identically behavior, avoiding potential bug.
            re->setOwner(0);
            re->setAttach(0);
            re->setEnvOri(true);
            re->setEnvScale(true);
            re->setUseOwnerSpaceOri(true);
            re->setUseOwnerSpaceScale(true);
        }

        re->setPos(pos);
        re->setOri(ori);
        re->setScale(Vector3f::UNIT);
        re->origin();
        bone->add(re);

        return re;
    }
    //-------------------------------------------------------------------------
    void SkeletonFusion::detach(TagBone * node)
    {
        TagBoneList::iterator it = std::find(mTagList.begin(), mTagList.end(), node);
        N_assert(it != mTagList.end(), "error");
        if(it != mTagList.end())
        {
            if(node->getParent())
                node->getParent()->remove(node);

            mFreeTagList.splice(mFreeTagList.end(), mTagList, it);
        }
    }
    //-------------------------------------------------------------------------
    ResourceID SkeletonFusion::getOriginID() const
    {
        return mSkeleton->getOriginID();
    }
    //-------------------------------------------------------------------------
    PrcID SkeletonFusion::getPrc() const
    {
        return mSkeleton->getPrc();
    }
    //-------------------------------------------------------------------------
    GroupID SkeletonFusion::getGroup()
    {
        return mSkeleton->getGroup();
    }
    //-------------------------------------------------------------------------
}