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
#include "NiiParticleCtrl.h"
#include "NiiSyncParticle.h"
#include "NiiMaterialManager.h"
#include "NiiGBufferManager.h"
#include "NiiRadixSort.h"
#include "NiiMath.h"
#include "NiiSphere.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"
#include "NiiLogManager.h"
#include "NiiDefine.h"
#include "NiiGeometryObj.h"
#include <algorithm>

namespace NII
{
    //-----------------------------------------------------------------------
    struct ParticleCtrlDirSortFunctor
    {
        Vector3f sortDir;

        ParticleCtrlDirSortFunctor(const Vector3f & dir):
            sortDir(dir)
        {
        }
        NIIf operator()(SyncParticle * bill) const
        {
            return sortDir.dotProduct(bill->getPos());
        }
    };
    //-----------------------------------------------------------------------
    struct ParticleCtrlDisSortFunctor
    {
        Vector3f sortPos;

        ParticleCtrlDisSortFunctor(const Vector3f & pos) :
            sortPos(pos)
        {
        }
        NIIf operator()(SyncParticle * bill) const
        {
            return - (sortPos - bill->getPos()).lengthSquared();
        }
    };

    static RadixSort<ParticleCtrl::ParticleList, SyncParticle *, NIIf> __ParticleCtrlRadixSorter;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ParticleCtrl
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ParticleCtrl::ParticleCtrl() :
        mBoundRange(0.0f),
        mOriginType(OT_Center),
        mRotationType(RM_TexCoord),
        mCtrlMode(CM_Parallel_View),
        mCtrlDir(Vector3f::Z),
        mCtrlUp(Vector3f::Y),
        mVertexData(0),
        mIndexData(0),
        mPoolSize(0),
        mGeoBuffer(0),
        mMaterial(0),
        mParticleChange(true),
        mSameSize(true),
        mAutoExtendPool(true),
        mSortEnable(false),
        mParallelView(false),
        mSameRotation(true),
        mCullItself(false),
        mPointSprites(false),
        mGeoBufCreate(false),
        mExternalData(false),
        mLocalSpace(true),
        mUpdatePerFrame(true)
    {
        setParticleWidth(100);
        setParticleHeight(100);
        setMaterial(MaterialManager::WhiteLight->getOriginID());
        mCastShadows = false;
        setTexCoordGrid(1, 1);
    }
    //-----------------------------------------------------------------------
    ParticleCtrl::ParticleCtrl(SpaceID sid, NCount poolSize, bool linkdata) :
        SpaceObj(sid),
        mBoundRange(0.0f),
        mOriginType(OT_Center),
        mRotationType(RM_TexCoord),
        mMaterial(0),
        mVertexData(0),
        mIndexData(0),
        mGeoBuffer(0),
        mCtrlMode(CM_Parallel_View),
        mCtrlDir(Vector3f::Z),
        mCtrlUp(Vector3f::Y),
        mExternalData(linkdata),
        mCullItself(false),
        mSameSize(true),
        mAutoExtendPool(true),
        mSortEnable(false),
        mParallelView(false),
        mSameRotation(true),
        mParticleChange(true),
        mPointSprites(false),
        mGeoBufCreate(false),
        mLocalSpace(true),
        mUpdatePerFrame(true)
    {
        setParticleWidth(100);
        setParticleHeight(100);
        setMaterial(MaterialManager::WhiteLight->getOriginID());
        setPoolSize(poolSize);
        mCastShadows = false;
        setTexCoordGrid(1, 1);
    }
    //-----------------------------------------------------------------------
    ParticleCtrl::~ParticleCtrl()
    {
        // Free pool items
        ParticlePool::iterator i, iend = mParticlePool.end();
        for(i = mParticlePool.begin(); i != iend; ++i)
        {
            N_delete *i;
        }

        // Delete shared buffers
        destroyGeoBuffer();
    }
    //-----------------------------------------------------------------------
    SyncParticle * ParticleCtrl::create(const Vector3f & position, const Colour & c)
    {
        if(mFreeList.empty())
        {
            if(mAutoExtendPool)
            {
                setPoolSize(getPoolSize() * 2);
            }
            else
            {
                return 0;
            }
        }

        // Get a new billboard
        SyncParticle * newBill = mFreeList.front();
        mActiveList.splice(mActiveList.end(), mFreeList, mFreeList.begin());

        newBill->setPos(position);
        newBill->setColour(c);
        newBill->mDirection = Vector3f::ZERO;
        newBill->setRotation(Radian(0));
        newBill->setCtrlTexCoord(0);
        newBill->inheritSize();
        newBill->mCtrl = this;

        // Merge into bounds
        NIIf adjust = std::max(mParticleWidth, mParticleHeight);
        Vector3f vecAdjust(adjust, adjust, adjust);
        Vector3f newMin = position - vecAdjust;
        Vector3f newMax = position + vecAdjust;

        mAABB.merge(newMin);
        mAABB.merge(newMax);

        mBoundRange = mAABB.calcRadius();

        return newBill;
    }
    //-----------------------------------------------------------------------
    NCount ParticleCtrl::getCount() const
    {
        return mActiveList.size();
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::clear()
    {
        // Move actives to free list
        mFreeList.splice(mFreeList.end(), mActiveList);
    }
    //-----------------------------------------------------------------------
    SyncParticle * ParticleCtrl::get(Nidx index) const
    {
        N_assert(index < mActiveList.size(), "SyncParticle index out of bounds." );

        /* We can't access it directly, so we check wether it's in the first
           or the second half, then we start either from the beginning or the
           end of the list
        */
        ParticleList::const_iterator it;
        if(index >= (mActiveList.size() >> 1))
        {
            index = static_cast<Nui32>(mActiveList.size()) - index;
            for(it = mActiveList.end(); index; --index, --it);
        }
        else
        {
            for(it = mActiveList.begin(); index; --index, ++it);
        }

        return *it;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::remove(Nidx index)
    {
        N_assert(index < mActiveList.size(), "SyncParticle index out of bounds." );

        /* We can't access it directly, so we check wether it's in the first
           or the second half, then we start either from the beginning or the
           end of the list.
           We then remove the billboard form the 'used' list and add it to
           the 'free' list.
        */
        ParticleList::iterator it;
        if(index >= (mActiveList.size() >> 1))
        {
            index = static_cast<Nui32>(mActiveList.size()) - index;
            for(it = mActiveList.end(); index; --index, --it);
        }
        else
        {
            for(it = mActiveList.begin(); index; --index, ++it);
        }

        mFreeList.splice(mFreeList.end(), mActiveList, it);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::remove(SyncParticle * obj)
    {
        ParticleList::iterator it = std::find(mActiveList.begin(),
            mActiveList.end(), obj);
        N_assert(it != mActiveList.end(), "SyncParticle isn't in the active list." );

        mFreeList.splice(mFreeList.end(), mActiveList, it);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setOriginType(ParticleCtrl::OriginType origin)
    {
        mOriginType = origin;

        switch(mOriginType)
        {
        case OT_Top_Left:
            mOriginLeft = 0.0f;
            mOriginRight = 1.0f;
            mOriginTop = 0.0f;
            mOriginBottom = -1.0f;
            break;
        case OT_Top_Center:
            mOriginLeft = -0.5f;
            mOriginRight = 0.5f;
            mOriginTop = 0.0f;
            mOriginBottom = -1.0f;
            break;
        case OT_Top_Right:
            mOriginLeft = -1.0f;
            mOriginRight = 0.0f;
            mOriginTop = 0.0f;
            mOriginBottom = -1.0f;
            break;
        case OT_Center_Left:
            mOriginLeft = 0.0f;
            mOriginRight = 1.0f;
            mOriginTop = 0.5f;
            mOriginBottom = -0.5f;
            break;
        case OT_Center:
            mOriginLeft = -0.5f;
            mOriginRight = 0.5f;
            mOriginTop = 0.5f;
            mOriginBottom = -0.5f;
            break;
        case OT_Center_Right:
            mOriginLeft = -1.0f;
            mOriginRight = 0.0f;
            mOriginTop = 0.5f;
            mOriginBottom = -0.5f;
            break;
        case OT_Bottom_Left:
            mOriginLeft = 0.0f;
            mOriginRight = 1.0f;
            mOriginTop = 1.0f;
            mOriginBottom = 0.0f;
            break;
        case OT_Bottom_Center:
            mOriginLeft = -0.5f;
            mOriginRight = 0.5f;
            mOriginTop = 1.0f;
            mOriginBottom = 0.0f;
            break;
        case OT_Bottom_Right:
            mOriginLeft = -1.0f;
            mOriginRight = 0.0f;
            mOriginTop = 1.0f;
            mOriginBottom = 0.0f;
            break;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setRotationMode(ParticleCtrl::RotationMode rm)
    {
        mRotationType = rm;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setParticleWidth(NIIf w)
    {
        mParticleWidth = w;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setParticleHeight(NIIf h)
    {
        mParticleHeight = h;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setMaterial(ResourceID rid, GroupID gid)
    {
        if(mMaterialID != rid)
        {
            if(mMaterial != 0)
                mMaterial->unref();
            mMaterialID = rid;
            mMaterial = static_cast<Material *>(N_Only(Material).get(rid, gid));

            if(mMaterial == 0)
                N_EXCEPT(NotExist, _I18n("Could not find material ") + rid);

            /* Ensure that the new material was loaded (will not load again if
               already loaded anyway)
            */
            mMaterial->load();
            mMaterial->ref(false);
        }
    }
    //-----------------------------------------------------------------------
    ParticleCtrl::SortMode ParticleCtrl::getSortMode() const
    {
        // Need to sort by distance if we're using accurate facing, or perpendicular billboard type.
        if(mParallelView || mCtrlMode == CM_Vertical_Self || mCtrlMode == CM_Vertical_Ctrl)
        {
            return SM_DISTANCE;
        }
        else
        {
            return SM_DIRECTION;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::_notify(Camera * cam)
    {
        SpaceObj::_notify(cam);

        mCamera = cam;

        // Calculate camera orientation and position
        mCameraLocalOri = mCamera->getSpaceOri();
        mCameraLocalPos = mCamera->getSpacePos();
        if(mLocalSpace)
        {
            // Default behaviour is that billboards are in local node space
            // so orientation of camera (in world space) must be reverse-transformed
            // into node space
            mCameraLocalOri = mSpaceNode->getSpaceOri().inverseUnit() * mCameraLocalOri;
            mCameraLocalPos = mSpaceNode->getSpaceOri().inverseUnit() *
                (mCameraLocalPos - mSpaceNode->getSpacePos()) / mSpaceNode->getSpaceScale();
        }

        // Camera direction points down -Z
        mCameraLocalDir = mCameraLocalOri * Vector3f::INV_Z;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::onBeginRender(NCount numBillboards)
    {
        /* Generate the vertices for all the billboards relative to the camera
           Also take the opportunity to update the vertex colours
           May as well do it here to save on loops elsewhere
        */

        /* NOTE: most engines generate world coordinates for the billboards
           directly, taking the world axes of the camera as offsets to the
           center points. I take a different approach, reverse-transforming
           the camera world axes into local billboard space.
           Why?
           Well, it's actually more efficient this way, because I only have to
           reverse-transform using the billboardset world matrix (inverse)
           once, from then on it's simple additions (assuming identically
           sized billboards). If I transformed every billboard center by it's
           world transform, that's a matrix multiplication per billboard
           instead.
           I leave the final transform to the render pipeline since that can
           use hardware TnL if it is available.
        */

        // create vertex and index buffers if they haven't already been
        if(!mGeoBufCreate)
            createGeoBuffer();

        // Only calculate vertex offets et al if we're not point rendering
        if(!mPointSprites)
        {
            // Generate axes etc up-front if not oriented per-billboard
            if(mCtrlMode != CM_Parallel_Self && mCtrlMode != CM_Vertical_Self &&
                !(mParallelView && mCtrlMode != CM_Vertical_Ctrl))
            {
                switch(mCtrlMode)
                {
                case CM_Parallel_View:
                    if(mParallelView)
                    {
                        // Point billboards will have 'up' based on but not equal to cameras
                        // Use mTempLocalY temporarily to avoid allocation
                        mTempLocalY = mCameraLocalOri * Vector3f::Y;
                        mTempLocalX = mCameraLocalDir.crossProduct(mTempLocalY);
                        mTempLocalX.normalise();
                        mTempLocalY = mTempLocalX.crossProduct(mCameraLocalDir); // both normalised already
                    }
                    else
                    {
                        // Get camera axes for X and Y (depth is irrelevant)
                        mTempLocalX = mCameraLocalOri * Vector3f::X;
                        mTempLocalY = mCameraLocalOri * Vector3f::Y;
                    }
                    break;
                case CM_Parallel_Ctrl:
                    // Y-axis is common direction
                    // X-axis is cross with camera direction
                    mTempLocalY = mCtrlDir;
                    mTempLocalX = mCameraLocalDir.crossProduct(mTempLocalY);
                    mTempLocalX.normalise();
                    break;
                case CM_Vertical_Ctrl:
                    // X-axis is up-vector cross common direction
                    // Y-axis is common direction cross X-axis
                    mTempLocalX = mCtrlUp.crossProduct(mCtrlDir);
                    mTempLocalY = mCtrlDir.crossProduct(mTempLocalX);
                    break;
                }

                /* If all billboards are the same size we can precalculate the
                   offsets and just use '+' instead of '*' for each billboard,
                   and it should be faster.
                */
                calcOffset(mParticleWidth, mParticleHeight, mVOffset);
            }
        }

        // Init num visible
        mVisableCount = 0;

        if(numBillboards) // optimal lock
        {
            // clamp to max
            numBillboards = std::min(mPoolSize, numBillboards);

            NCount billboardSize;
            if(mPointSprites)
            {
                // just one vertex per billboard (this also excludes texcoords)
                billboardSize = mGeoBuffer->getUnitSize();
            }
            else
            {
                // 4 corners
                billboardSize = mGeoBuffer->getUnitSize() * 4;
            }
            N_assert(numBillboards * billboardSize <= mGeoBuffer->getSize(), "error");

            mGeoBufferPtr = static_cast<NIIf *>(
                mGeoBuffer->lock(0, numBillboards * billboardSize,
                    mGeoBuffer->getModeMark() & (Buffer::M_NORMAL ) ?
                        Buffer::MM_WHOLE : (Buffer::MM_READ | Buffer::MM_WRITE)));
        }
        else
            mGeoBufferPtr = static_cast<NIIf *>(
                mGeoBuffer->lock(mGeoBuffer->getModeMark() & (Buffer::M_NORMAL ) ?
                    Buffer::MM_WHOLE : (Buffer::MM_READ | Buffer::MM_WRITE)));
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::onRender(const SyncParticle & bb)
    {
        if(mVisableCount == mPoolSize)
            return;

        if(isCulling(mCamera, bb))
            return;

        if(!mPointSprites && (mCtrlMode == CM_Parallel_Self || mCtrlMode == CM_Vertical_Self ||
            (mParallelView && mCtrlMode != CM_Vertical_Ctrl)))
        {
            // If we're using accurate facing, recalculate camera direction per BB
            if(mParallelView && (mCtrlMode == CM_Parallel_View ||
                mCtrlMode == CM_Parallel_Ctrl || mCtrlMode == CM_Parallel_Self))
            {
                // cam -> bb direction
                mCameraLocalDir = bb.mPosition - mCameraLocalPos;
                mCameraLocalDir.normalise();
            }

            switch(mCtrlMode)
            {
            case CM_Parallel_View:
                if(mParallelView)
                {
                    // Point billboards will have 'up' based on but not equal to cameras
                    // Use mTempLocalY temporarily to avoid allocation
                    mTempLocalY = mCameraLocalOri * Vector3f::Y;
                    mTempLocalX = mCameraLocalDir.crossProduct(mTempLocalY);
                    mTempLocalX.normalise();
                    mTempLocalY = mTempLocalX.crossProduct(mCameraLocalDir); // both normalised already
                }
                else
                {
                    // Get camera axes for X and Y (depth is irrelevant)
                    mTempLocalX = mCameraLocalOri * Vector3f::X;
                    mTempLocalY = mCameraLocalOri * Vector3f::Y;
                }
                break;
            case CM_Parallel_Ctrl:
                // Y-axis is common direction
                // X-axis is cross with camera direction
                mTempLocalY = mCtrlDir;
                mTempLocalX = mCameraLocalDir.crossProduct(mTempLocalY);
                mTempLocalX.normalise();
                break;
            case CM_Parallel_Self:
                // Y-axis is direction
                // X-axis is cross with camera direction
                // Scale direction first
                mTempLocalY = bb.mDirection;
                mTempLocalX = mCameraLocalDir.crossProduct(mTempLocalY);
                mTempLocalX.normalise();
                break;
            case CM_Vertical_Ctrl:
                // X-axis is up-vector cross common direction
                // Y-axis is common direction cross X-axis
                mTempLocalX = mCtrlUp.crossProduct(mCtrlDir);
                mTempLocalY = mCtrlDir.crossProduct(mTempLocalX);
                break;
            case CM_Vertical_Self:
                // X-axis is up-vector cross own direction
                // Y-axis is own direction cross X-axis
                mTempLocalX = mCtrlUp.crossProduct(bb.mDirection);
                mTempLocalX.normalise();
                mTempLocalY = bb.mDirection.crossProduct(mTempLocalX); // both should be normalised
                break;
            }
        }

        // If they're all the same size or we're point rendering
        if(mSameSize || mPointSprites)
        {
            /* No per-billboard checking, just blast through.
            Saves us an if clause every billboard which may
            make a difference.
            */

            if(!mPointSprites && (mCtrlMode == CM_Parallel_Self ||
                mCtrlMode == CM_Vertical_Self ||
                    (mParallelView && mCtrlMode != CM_Vertical_Ctrl)))
            {
                calcOffset(mParticleWidth, mParticleHeight, mVOffset);
            }
            addParticle(bb, mVOffset);
        }
        else
        {
            Vector3f vOwnOffset[4];
            // If it has own dimensions, or self-oriented, gen offsets
            if(mCtrlMode == CM_Parallel_Self || mCtrlMode == CM_Vertical_Self ||
                bb.mSizeValid || (mParallelView && mCtrlMode != CM_Vertical_Ctrl))
            {
                // Generate using own dimensions
                calcOffset(bb.mWidth, bb.mHeight, vOwnOffset);
                // Create vertex data
                addParticle(bb, vOwnOffset);
            }
            else // Use default dimension, already computed before the loop, for faster creation
            {
                addParticle(bb, mVOffset);
            }
        }
        // Increment visibles
        mVisableCount++;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::onEndRender()
    {
        mGeoBuffer->unlock();
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::updateBounds()
    {
        if(mActiveList.empty())
        {
            mAABB.setNull();
            mBoundRange = 0.0f;
        }
        else
        {
            NIIf maxSqLen = -1.0f;

            Vector3f min(Math::POS_INFINITY, Math::POS_INFINITY, Math::POS_INFINITY);
            Vector3f max(Math::NEG_INFINITY, Math::NEG_INFINITY, Math::NEG_INFINITY);
            ParticleList::iterator i, iend = mActiveList.end();
            Matrix4f invWorld;
            if(!mLocalSpace && getSceneNode())
                invWorld = getSceneNode()->getSpaceMatrix().inverse();

            for(i = mActiveList.begin(); i != iend; ++i)
            {
                Vector3f pos = (*i)->getPos();
                // transform from world space to local space
                if(!mLocalSpace && getSceneNode())
                    pos = invWorld * pos;
                min = min.floor(pos);
                max = max.ceil(pos);

                maxSqLen = std::max(maxSqLen, pos.lengthSquared());
            }
            // Adjust for billboard size
            NIIf adjust = std::max(mParticleWidth, mParticleHeight);
            Vector3f vecAdjust(adjust, adjust, adjust);
            min -= vecAdjust;
            max += vecAdjust;

            mAABB.setMin(min);
            mAABB.setMax(max);
            mBoundRange = Math::sqrt(maxSqLen);
        }

        if(mSpaceNode)
            mSpaceNode->notify();
    }
    //-----------------------------------------------------------------------
    const AABox& ParticleCtrl::getAABB(void) const
    {
        return mAABB;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::queue(RenderQueue * queue)
    {
        // If we're driving this from our own data, update geometry if need to.
        if(!mExternalData && (mUpdatePerFrame || mParticleChange || !mGeoBufCreate))
        {
            if(mSortEnable)
            {
                switch(getSortMode())
                {
                case SM_DIRECTION:
                    __ParticleCtrlRadixSorter.sort(mActiveList, ParticleCtrlDirSortFunctor(-mCameraLocalDir));
                    break;
                case SM_DISTANCE:
                    __ParticleCtrlRadixSorter.sort(mActiveList, ParticleCtrlDisSortFunctor(mCameraLocalPos));
                    break;
                }
            }

            onBeginRender(mActiveList.size());
            ParticleList::iterator it, itend = mActiveList.end();
            for(it = mActiveList.begin(); it != itend; ++it)
            {
                onRender(*(*it));
            }
            onEndRender();
            mParticleChange = false;
        }

        //only set the render queue group if it has been explicitly set.
        if(mRenderLevel != 0)
        {
            queue->add(this, mRenderGroup, mRenderLevel);
        }
        else if(mRenderGroup != RQG_Unknow)
        {
           queue->add(this, mRenderGroup);
        }
        else
        {
           queue->add(this);
        }
    }
    //-----------------------------------------------------------------------
    const Material * ParticleCtrl::getMaterial() const
    {
        return mMaterial;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setMaterial(Material * material)
    {
        if(mMaterial != material)
        {
            if(mMaterial != 0)
                mMaterial->unref();

            if(material != 0)
            {
                mMaterial = material;
                material->ref(false);
            }
            else
            {
                N_Only(Log).log(
                    _I18n("Can't assign materialto ParticleCtrl of ") + N_conv(mID) +
                        _I18n("because this Material does not exist.") +
                            _I18n("Have you forgotten to define it in a .material script?"));

                mMaterial = const_cast<Material *>(MaterialManager::WhiteLight);

                if(mMaterial == 0)
                {
                    N_EXCEPT(Internal,_I18n("Can't assign default material to ParticleCtrl ") + 
                        N_conv(mID) + _I18n(". Did you forget to call MaterialManager::init()?"));
                }
                mMaterial->ref(false);
            }

            mMaterialID = mMaterial->getOriginID();

            // Ensure new material loaded (will not load again if already loaded)
            mMaterial->load();
        }
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::getGeometry(GeometryRaw & op) const
    {
        op.mVertex = mVertexData;
        op.mVertex->mOffset = 0;

        if(mPointSprites)
        {
            op.mType = GeometryRaw::OT_POINT_LIST;

            op.remove(GRT_Index | GRT_Instancing);
            op.mIndex = 0;
            op.mVertex->mCount = mVisableCount;
        }
        else
        {
            op.mType = GeometryRaw::OT_TRIANGLE_LIST;
            op.add(GRT_Index);

            op.mVertex->mCount = mVisableCount * 4;

            op.mIndex = mIndexData;
            op.mIndex->mCount = mVisableCount * 6;
            op.mIndex->mOffset = 0;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::getMatrix(Matrix4f * out, NCount & count) const
    {
        if(mLocalSpace)
        {
            *out = getSpaceMatrix();
        }
        else
        {
            *out = Matrix4f::IDENTITY;
        }
        count = 1;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setPoolSize(NCount size)
    {
        if(!mExternalData)
        {
            // Never shrink below size()
            NCount current = mParticlePool.size();
            if(current >= size)
                return;

            NCount oldSize = mParticlePool.size();

            mParticlePool.reserve(size);
            mParticlePool.resize(size);

            for (NCount i = oldSize; i < size; ++i)
            {
                mParticlePool[i] = N_new SyncParticle();
            }
            for(NCount i = current; i < size; ++i)
            {
                mFreeList.push_back(mParticlePool[i]);
            }
        }

        mPoolSize = size;

        destroyGeoBuffer();
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::createGeoBuffer()
    {
        /* Allocate / reallocate vertex data
           Note that we allocate enough space for ALL the billboards in the pool, but only issue
           rendering operations for the sections relating to the active billboards
        */

        /* Alloc positions   ( 1 or 4 verts per billboard, 3 components )
                 colours     ( 1 x RGBA per vertex )
                 indices     ( 6 per billboard ( 2 tris ) if not point rendering )
                 tex. coords ( 2D coords, 1 or 4 per billboard )
        */

        // Warn if user requested an invalid setup
        // Do it here so it only appears once
        if(mPointSprites && mCtrlMode != CM_Parallel_View)
        {
            N_Only(Log).log(_I18n("Warning: ParticleCtrl ") + N_conv(mID) +
                _I18n(" has point rendering enabled but is using a type other than CM_Parallel_View,")
                    _I18n("this may not give you the results you expect."));
        }

        N_Only(GBuffer).create(mVertexData);
        if(mPointSprites)
            mVertexData->mCount = mPoolSize;
        else
            mVertexData->mCount = mPoolSize * 4;

        mVertexData->mOffset = 0;

        NCount offset = 0;

        mVertexData->add(DT_Float3x, VT_Pos, 0, offset);
        offset += DataTypeUtil::getSize(DT_Float3x);
        mVertexData->add(DT_Colour, VT_Diffuse, 0, offset);
        offset += DataTypeUtil::getSize(DT_Colour);

        // Texture coords irrelevant when enabled point rendering (generated
        // in point sprite mode, and unused in standard point mode)
        if(!mPointSprites)
        {
            mVertexData->add(DT_Float2x, VT_Tex_Coord, 0, offset, 0);
        }

        N_Only(GBuffer).create(mGeoBuffer, mVertexData->getUnitSize(0), mVertexData->mCount, mUpdatePerFrame ? Buffer::M_WHOLE | Buffer::M_NORMAL : Buffer::M_NORMAL );
        // bind position and diffuses
        mVertexData->attach(0, mGeoBuffer);

        if(!mPointSprites)
        {
            mIndexData  = N_new IndexData();
            mIndexData->mOffset = 0;
            mIndexData->mCount = mPoolSize * 6;

            N_Only(GBuffer).create(mIndexData->mBuffer, 16, mIndexData->mCount, Buffer::M_NORMAL );

            /* Create indexes (will be the same every frame)
               Using indexes because it means 1/3 less vertex transforms (4 instead of 6)

               SyncParticle layout relative to camera:

                0-----1
                |    /|
                |  /  |
                |/    |
                2-----3
            */

            Nui16 * pIdx = static_cast<Nui16*>(mIndexData->mBuffer->lock(0,
                mIndexData->mBuffer->getSize(), Buffer::MM_WHOLE));

            for(NCount idx, idxOff, bboard = 0; bboard < mPoolSize; ++bboard )
            {
                idx    = bboard * 6;
                idxOff = bboard * 4;

                pIdx[idx] = static_cast<Nui16>(idxOff); // + 0;, for clarity
                pIdx[idx+1] = static_cast<Nui16>(idxOff + 2);
                pIdx[idx+2] = static_cast<Nui16>(idxOff + 1);
                pIdx[idx+3] = static_cast<Nui16>(idxOff + 1);
                pIdx[idx+4] = static_cast<Nui16>(idxOff + 2);
                pIdx[idx+5] = static_cast<Nui16>(idxOff + 3);
            }
            mIndexData->mBuffer->unlock();
        }
        mGeoBufCreate = true;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::destroyGeoBuffer()
    {
        if(mVertexData)
        {
            N_delete mVertexData;
            mVertexData = 0;
        }
        if(mIndexData)
        {
            N_delete mIndexData;
            mIndexData = 0;
        }
        if(mGeoBuffer)
        {
            N_delete mGeoBuffer;
            mGeoBuffer = 0;
        }
        mGeoBufCreate = false;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::onParticleSize()
    {
        mSameSize = false;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::onParticleRotate()
    {
        mSameRotation = false;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::onParticleChange()
    {
        mParticleChange = true;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setCullItself(bool b)
    {
        mCullItself = b;
    }
    //-----------------------------------------------------------------------
    bool ParticleCtrl::isCulling(Camera * cam, const SyncParticle & bill)
    {
        if(!mCullItself)
            return false;

        // Cull based on sphere (have to transform less)
        Sphere sph;
        Matrix4f xworld;
        NCount count;
        getMatrix(&xworld, count);

        sph.setCenter(xworld.affine(bill.mPosition));

        if(bill.mSizeValid)
        {
            sph.setRadius(std::max(bill.mWidth, bill.mHeight));
        }
        else
        {
            sph.setRadius(std::max(mParticleWidth, mParticleHeight));
        }

        return !cam->isIntersect(sph);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setCtrlMode(ParticleCtrl::CtrlMode bt)
    {
        mCtrlMode = bt;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setParallelView(bool b)
    {
        mParallelView = b;
    }
    //-----------------------------------------------------------------------
    Nui32 ParticleCtrl::getTypeMark() const
    {
        return SpaceManager::ParticleSpaceMark;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::addParticle(const SyncParticle & bb, const Vector3f * offsets)
    {
        RGBA colour;
        RenderSys * rs = N_Engine().getRender();
        rs->convertType(bb.mColour, &colour);
        RGBA * pCol;

        // Texcoords
        N_assert(bb.mTexCoordValid || bb.mCtrlTexCoord < mTexCoordList.size(), "error");
        const Rectf & r = bb.mTexCoordValid ? bb.mTexCoord : mTexCoordList[bb.mCtrlTexCoord];

        if(mPointSprites)
        {
            // Single vertex per billboard, ignore offsets
            // position
            *mGeoBufferPtr++ = bb.mPosition.x;
            *mGeoBufferPtr++ = bb.mPosition.y;
            *mGeoBufferPtr++ = bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA *>(static_cast<void *>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void *>(pCol));
            // No texture coords in point rendering
        }
        else if (mSameRotation || bb.mRotation == Radian(0))
        {
            // Left-top
            // Positions
            *mGeoBufferPtr++ = offsets[0].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[0].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[0].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void *>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void *>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mLeft;
            *mGeoBufferPtr++ = r.mTop;

            // Right-top
            // Positions
            *mGeoBufferPtr++ = offsets[1].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[1].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[1].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void *>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void *>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mRight;
            *mGeoBufferPtr++ = r.mTop;

            // Left-bottom
            // Positions
            *mGeoBufferPtr++ = offsets[2].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[2].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[2].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void *>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void *>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mLeft;
            *mGeoBufferPtr++ = r.mBottom;

            // Right-bottom
            // Positions
            *mGeoBufferPtr++ = offsets[3].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[3].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[3].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void *>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void *>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mRight;
            *mGeoBufferPtr++ = r.mBottom;
        }
        else if (mRotationType == RM_VertexPos)
        {
            // TODO: Cache axis when billboard type is CM_Parallel_View or CM_Vertical_Ctrl
            Vector3f axis;
            (offsets[3] - offsets[0]).crossProduct(offsets[2] - offsets[1]).normalise(axis);

            Matrix3f rotation;
            rotation.from(axis, bb.mRotation);

            Vector3f pt;

            // Left-top
            // Positions
            pt = rotation * offsets[0];
            *mGeoBufferPtr++ = pt.x + bb.mPosition.x;
            *mGeoBufferPtr++ = pt.y + bb.mPosition.y;
            *mGeoBufferPtr++ = pt.z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mLeft;
            *mGeoBufferPtr++ = r.mTop;

            // Right-top
            // Positions
            pt = rotation * offsets[1];
            *mGeoBufferPtr++ = pt.x + bb.mPosition.x;
            *mGeoBufferPtr++ = pt.y + bb.mPosition.y;
            *mGeoBufferPtr++ = pt.z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mRight;
            *mGeoBufferPtr++ = r.mTop;

            // Left-bottom
            // Positions
            pt = rotation * offsets[2];
            *mGeoBufferPtr++ = pt.x + bb.mPosition.x;
            *mGeoBufferPtr++ = pt.y + bb.mPosition.y;
            *mGeoBufferPtr++ = pt.z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mLeft;
            *mGeoBufferPtr++ = r.mBottom;

            // Right-bottom
            // Positions
            pt = rotation * offsets[3];
            *mGeoBufferPtr++ = pt.x + bb.mPosition.x;
            *mGeoBufferPtr++ = pt.y + bb.mPosition.y;
            *mGeoBufferPtr++ = pt.z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = r.mRight;
            *mGeoBufferPtr++ = r.mBottom;
        }
        else
        {
            const NIIf cos_rot(Math::Cos(bb.mRotation));
            const NIIf sin_rot(Math::Sin(bb.mRotation));

            NIIf width = (r.mRight-r.mLeft)/2;
            NIIf height = (r.mBottom-r.mTop)/2;
            NIIf mid_u = r.mLeft+width;
            NIIf mid_v = r.mTop+height;

            NIIf cos_rot_w = cos_rot * width;
            NIIf cos_rot_h = cos_rot * height;
            NIIf sin_rot_w = sin_rot * width;
            NIIf sin_rot_h = sin_rot * height;

            // Left-top
            // Positions
            *mGeoBufferPtr++ = offsets[0].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[0].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[0].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = mid_u - cos_rot_w + sin_rot_h;
            *mGeoBufferPtr++ = mid_v - sin_rot_w - cos_rot_h;

            // Right-top
            // Positions
            *mGeoBufferPtr++ = offsets[1].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[1].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[1].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = mid_u + cos_rot_w + sin_rot_h;
            *mGeoBufferPtr++ = mid_v + sin_rot_w - cos_rot_h;

            // Left-bottom
            // Positions
            *mGeoBufferPtr++ = offsets[2].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[2].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[2].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = mid_u - cos_rot_w - sin_rot_h;
            *mGeoBufferPtr++ = mid_v - sin_rot_w + cos_rot_h;

            // Right-bottom
            // Positions
            *mGeoBufferPtr++ = offsets[3].x + bb.mPosition.x;
            *mGeoBufferPtr++ = offsets[3].y + bb.mPosition.y;
            *mGeoBufferPtr++ = offsets[3].z + bb.mPosition.z;
            // Colour
            // Convert NIIf* to RGBA*
            pCol = static_cast<RGBA*>(static_cast<void*>(mGeoBufferPtr));
            *pCol++ = colour;
            // Update lock pointer
            mGeoBufferPtr = static_cast<NIIf*>(static_cast<void*>(pCol));
            // Texture coords
            *mGeoBufferPtr++ = mid_u + cos_rot_w - sin_rot_h;
            *mGeoBufferPtr++ = mid_v + sin_rot_w + cos_rot_h;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::calcOffset(NIIf width, NIIf height, Vector3f * pDestVec)
    {
        Vector3f vLeftOff, vRightOff, vTopOff, vBottomOff;
        /* Calculate default offsets. Scale the axes by parametric offset and dimensions,
            ready to be added to positions.
        */

        vLeftOff   = mTempLocalX * (mOriginLeft * width);
        vRightOff  = mTempLocalX * (mOriginRight * width);
        vTopOff    = mTempLocalY * (mOriginTop * height);
        vBottomOff = mTempLocalY * (mOriginBottom * height);

        // Make final offsets to vertex positions
        pDestVec[0] = vLeftOff  + vTopOff;
        pDestVec[1] = vRightOff + vTopOff;
        pDestVec[2] = vLeftOff  + vBottomOff;
        pDestVec[3] = vRightOff + vBottomOff;
    }
    //-----------------------------------------------------------------------
    FactoryID ParticleCtrl::getFactoryID() const
    {
        return N_FACTORY_ParticleCtrl;
    }
    //-----------------------------------------------------------------------
    NIIf ParticleCtrl::distanceSquared(const Camera * const cam) const
    {
        N_assert(mSpaceNode, "error");
        return mSpaceNode->distanceSquared(cam);
    }
    //-----------------------------------------------------------------------
    NIIf ParticleCtrl::getCenterRange() const
    {
        return mBoundRange;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::getLight(LightList & ll) const
    {
        // It's actually quite unlikely that this will be called,
        // because most billboards are unlit, but here we go anyway
        query(ll);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::query(GeometryQuery * query)
    {
        query->query(this);
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setTextureCoord(Rectf * coords, NCount count)
    {
        if(!count || !coords)
        {
            setTexCoordGrid(1, 1);
            return;
        }
        //  clear out any previous allocation (as vectors may not shrink)
        TexCoordList().swap(mTexCoordList);
        //  make room
        mTexCoordList.resize(count);
        //  copy in data
        std::copy(coords, coords + count, &mTexCoordList.front());
    }
    //----------------------------------------------------------------------
    void ParticleCtrl::setTextureCoord(Nidx idx, const Rectf * coords)
    {
        if(idx < mTexCoordList.size())
        {
            mTexCoordList[idx] = *coords;
        }
    }
    //----------------------------------------------------------------------
    void ParticleCtrl::setTexCoordGrid(NCount stacks, NCount slices)
    {
        if(stacks == 0)
            stacks = 1;
        if(slices == 0)
            slices = 1;
        //  clear out any previous allocation (as vectors may not shrink)
        TexCoordList().swap(mTexCoordList);
        //  make room
        mTexCoordList.resize((NCount)stacks * slices);
        Nui32 coordIndex = 0;
        //  spread the U and V coordinates across the rects
        for(Nui32 v = 0; v < stacks; ++v)
        {
            //  (NIIf)X / X is guaranteed to be == 1.0f for X up to 8 million, so
            //  our range of 1..256 is quite enough to guarantee perfect coverage.
            NIIf top = (NIIf)v / (NIIf)stacks;
            NIIf bottom = ((NIIf)v + 1) / (NIIf)stacks;
            for(Nui32 u = 0; u < slices; ++u)
            {
                Rectf & r = mTexCoordList[coordIndex];
                r.mLeft = (NIIf)u / (NIIf)slices;
                r.mBottom = bottom;
                r.mRight = ((NIIf)u + 1) / (NIIf)slices;
                r.mTop = top;
                ++coordIndex;
            }
        }
        N_assert(coordIndex == (NCount)stacks * slices, "error");
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setPointSprites(bool b)
    {
        // Override point rendering if not supported
        RenderSys * rsys = N_Engine().getRender();
        if(b && !rsys->getFeature()->isSupport(GF_Point_Sprites))
        {
            b = false;
        }

        if(b != mPointSprites)
        {
            mPointSprites = b;
            // Different buffer structure (1 or 4 verts per billboard)
            destroyGeoBuffer();
        }
    }
    //-----------------------------------------------------------------------
    void ParticleCtrl::setUpdatePerFrame(bool b)
    {
        // Case auto update buffers changed we have to destroy the current buffers
        // since their usage will be different.
        if(b != mUpdatePerFrame)
        {
            mUpdatePerFrame = b;
            destroyGeoBuffer();
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ParticleCtrlFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ParticleCtrlFactory::ParticleCtrlFactory()
    {
    }
    //-----------------------------------------------------------------------
    ParticleCtrlFactory::~ParticleCtrlFactory()
    {
    }
    //-----------------------------------------------------------------------
    FactoryID ParticleCtrlFactory::getID() const
    {
        return N_FACTORY_ParticleCtrl;
    }
    //-----------------------------------------------------------------------
    SpaceObj * ParticleCtrlFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        bool externalData;
        Nui32 poolSize;
        ParticleCtrl * re;

        if(params != 0)
        {
            params->get(N_PropertyParticleCtrl_PoolSize, poolSize, 0);
            params->get(N_PropertyParticleCtrl_ExtData, externalData, false);
        }

        if(poolSize > 0)
        {
            re = N_new ParticleCtrl(sid, poolSize, externalData);
        }
        else
        {
            re = N_new ParticleCtrl(sid);
        }
        re->setFactory(this);
        re->setManager(mag);
        return re;
    }
    //-----------------------------------------------------------------------
    void ParticleCtrlFactory::destroy(void * obj)
    {
        ParticleCtrl * temp = static_cast<ParticleCtrl *>(obj);
        N_delete temp;
    }
    //------------------------------------------------------------------------
}