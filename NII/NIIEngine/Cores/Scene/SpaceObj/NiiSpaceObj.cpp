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
#include "NiiSpaceObj.h"
#include "NiiSpaceNode.h"
#include "NiiTagBone.h"
#include "NiiGeometrySpace.h"
#include "NiiEngine.h"
#include "NiiLodListener.h"
#include "NiiRenderQueue.h"
#include "NiiGeometryObj.h"
#include "NiiMaterial.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // SpaceObjListener
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpaceObjListener::SpaceObjListener()
    {
    }
    //-----------------------------------------------------------------------
    SpaceObjListener::~SpaceObjListener()
    {
    }
    //-----------------------------------------------------------------------
    void SpaceObjListener::create(SpaceObj *)
    {

    }
    //-----------------------------------------------------------------------
    void SpaceObjListener::destroy(SpaceObj *)
    {
    }
    //-----------------------------------------------------------------------
    void SpaceObjListener::notify(SpaceObj *)
    {
    }
    //-----------------------------------------------------------------------
    void SpaceObjListener::attach(SpaceObj *, const PosNode *)
    {
    }
    //-----------------------------------------------------------------------
    void SpaceObjListener::detach(SpaceObj *, const PosNode *)
    {
    }
    //-----------------------------------------------------------------------
    bool SpaceObjListener::render(SpaceObj *, const Camera *)
    {
        return true;
    }
    //-----------------------------------------------------------------------
    void SpaceObjListener::query(const SpaceObj *, LightList & out)
    {
        out.clear();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // SpaceObj
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpaceID SpaceObj::mValidID = N_Max_Valid_ID;
    //-----------------------------------------------------------------------
    SpaceObj::SpaceObj() :
        mFactory(0),
        mManager(0),
        mDisLimit(0),
        mSpaceNode(0),
        mRenderLevel(0),
        mRenderGroup(RQG_Unknow),
        mIntersectMark(0xFFFFFFFF),
        mVisibleMark(0xFFFFFFFF),
        mLightMask(0xFFFFFFFF),
        mComMark(0),
        mListener(0),
        mRelation(0),
        mLightDirtyMark(0),
        mCullType(CT_Auto),
        mVisible(true),
        mTagNode(false),
        mDebugMode(false),
        mInvisible(false),
        mCastShadows(true),
        mListenerCtl(false)
    {
        if(Engine::getOnlyPtr())
            mPixelLimit = N_Engine().getMinPixelSize();
        else
            mPixelLimit = 0;
            
        N_lock_mutex(mValidIDMutex)
        mID = ++mValidID;
    }
    //-----------------------------------------------------------------------
    SpaceObj::SpaceObj(SpaceID sid, SpaceManager * mag) :
        mFactory(0),
        mManager(mag),
        mSpaceNode(0),
        mListener(0),
        mRelation(0),
        mDisLimit(0),
        mCullType(CT_Auto),
        mLightDirtyMark(0),
        mRenderGroup(RQG_Unknow),
        mRenderLevel(0),
        mIntersectMark(0xFFFFFFFF),
        mVisibleMark(0xFFFFFFFF),
        mLightMask(0xFFFFFFFF),
        mTagNode(false),
        mVisible(true),
        mDebugMode(false),
        mInvisible(false),
        mCastShadows(true),
        mListenerCtl(false)
    {
        if(Engine::getOnlyPtr())
            mPixelLimit = N_Engine().getMinPixelSize();
        else
            mPixelLimit = 0;
            
        if(sid == 0)
        {
            N_lock_mutex(mValidIDMutex)
            mID = ++mValidID;
        }
        else
        {
            mID = sid;
        }
    }
    //-----------------------------------------------------------------------
    SpaceObj::~SpaceObj()
    {
        if(mListener)
        {
            mListener->destroy(this);
        }

        if(mSpaceNode)
        {
            // detach from parent
            if(mTagNode)
            {
                // May be we are a lod entity which not in the parent entity child object list,
                // call this method could safely ignore this case.
                static_cast<GeometrySpace *>(static_cast<TagBone *>(mSpaceNode)->getOwner())->detach(this);
            }
            else
            {
                // May be we are a lod entity which not in the parent node child object list,
                // call this method could safely ignore this case.
                static_cast<SpaceNode *>(mSpaceNode)->detach(this);
            }
        }
    }
    //-----------------------------------------------------------------------
    FactoryID SpaceObj::getFactoryID() const
    {
        return N_FACTORY_SpaceObj;
    }
    //-----------------------------------------------------------------------
    void SpaceObj::attach(PosNode * node, bool tag)
    {
        N_assert(!mSpaceNode || !node, "error");

        PosNode * old = mSpaceNode;
        mSpaceNode = node;
        mTagNode = tag;

        // 标记灯光列表发生改变，尽量减少开销所以简单减少记数
        --mLightDirtyMark;

        if(mListener && (old != mSpaceNode))
        {
            if(mSpaceNode)
            {
                if(old)
                    mListener->detach(this, old);
                mListener->attach(this, mSpaceNode);
            }
            else if(old)
            {
                mListener->detach(this, old);
            }
        }
    }
    //-----------------------------------------------------------------------
    PosNode * SpaceObj::getSpaceNode() const
    {
        return mSpaceNode;
    }
    //-----------------------------------------------------------------------
    SpaceNode * SpaceObj::getSceneNode() const
    {
        if(mTagNode)
        {
            TagBone * tp = static_cast<TagBone *>(mSpaceNode);
            return tp->getOwner()->getSceneNode();
        }
        else
        {
            return static_cast<SpaceNode *>(mSpaceNode);
        }
    }
    //-----------------------------------------------------------------------
    bool SpaceObj::isAttach() const
    {
        return (mSpaceNode != 0);
    }
    //-----------------------------------------------------------------------
    void SpaceObj::detach()
    {
        if(isAttach())
        {
            if(mTagNode)
            {
                TagBone * tp = static_cast<TagBone *>(mSpaceNode);
                static_cast<GeometrySpace *>(tp->getOwner())->detach(this);
            }
            else
            {
                SpaceNode * sn = static_cast<SpaceNode *>(mSpaceNode);
                sn->detach(this);
            }
        }
    }
    //-----------------------------------------------------------------------
    void SpaceObj::setCullType(CullType t)
    {
        mCullType = t;
    }
    //-----------------------------------------------------------------------
    SpaceObj::CullType SpaceObj::getCullType() const
    {
        return mCullType;
    }
    //-----------------------------------------------------------------------
    bool SpaceObj::isActive() const
    {
        if(mSpaceNode != 0)
        {
            if(mTagNode)
            {
                TagBone * tp = static_cast<TagBone *>(mSpaceNode);
                return static_cast<GeometrySpace *>(tp->getOwner())->isActive();
            }
            else
            {
                SpaceNode * sn = static_cast<SpaceNode *>(mSpaceNode);
                return sn->isEnable();
            }
        }
        else
        {
            return false;
        }
    }
    //-----------------------------------------------------------------------
    void SpaceObj::_notify()
    {
        // 标记灯光列表发生改变，尽量减少开销所以简单减少记数
        --mLightDirtyMark;

        if(mListener)
        {
            mListener->notify(this);
        }
        mMove = true;
    }
    //-----------------------------------------------------------------------
    void SpaceObj::setVisible(bool b)
    {
        mVisible = b;
    }
    //-----------------------------------------------------------------------
    bool SpaceObj::getVisible() const
    {
        return mVisible;
    }
    //-----------------------------------------------------------------------
    bool SpaceObj::isVisible() const
    {
        if(!mVisible || mInvisible || mListenerCtl)
            return false;

        SpaceManager * sm = N_Engine().getProcessSpace();
        if(sm && !(mVisibleMark & sm->getEnvVisibleMask()))
            return false;

        return true;
    }
    //-----------------------------------------------------------------------
    void SpaceObj::_notify(Camera * cam)
    {
        if(mSpaceNode)
        {
            mInvisible = false;
            if(cam->isVisibleDistanceLimit() && mDisLimit > 0)
            {
                NIIf rad = getCenterRange();
                NIIf squaredDepth = mSpaceNode->distanceSquared(cam->getExtLod());

                const Vector3f & scl = mSpaceNode->getSpaceScale();
                NIIf factor = std::max(std::max(scl.x, scl.y), scl.z);
                // 仍然渲染的最大距离
                NIIf maxDist = mDisLimit + rad * factor;
                if(squaredDepth > Math::Sqr(maxDist))
                {   // 它与摄象机的距离大于所设定的,所以不会被渲染
                    mInvisible = true;
                }
            }
            if(!mInvisible && cam->isVisiblePixelLimit() && mPixelLimit > 0)
            {
                NIIf pixelRatio = cam->getPixelRatio();

                //if ratio is relative to distance than the distance at which the object should be displayed
                //is the size of the radius divided by the ratio
                //get the size of the entity in the world
                Vector3f objBound = getAABB().getSize() * getSpaceNode()->getSpaceScale();

                //We object are projected from 3 dimensions to 2. The shortest displayed dimension of
                //as object will always be at most the second largest dimension of the 3 dimensional
                //bounding box.
                //The square calculation come both to get rid of minus sign and for improve speed
                //in the final calculation
                objBound.x = Math::Sqr(objBound.x);
                objBound.y = Math::Sqr(objBound.y);
                objBound.z = Math::Sqr(objBound.z);
                NIIf sqrObjMedianSize = std::max(std::max(std::min(objBound.x, objBound.y),
                    std::min(objBound.x, objBound.z)), std::min(objBound.y, objBound.z));

                //If we have a perspective camera calculations are done relative to distance
                NIIf sqrDistance = 1;
                if(cam->getProjType() == Frustum::T_Perspective)
                {
                    sqrDistance = mSpaceNode->distanceSquared(cam->getExtLod());
                }

                //Final Calculation to tell whether the object is to small
                mInvisible =  sqrObjMedianSize < sqrDistance * Math::Sqr(pixelRatio * mPixelLimit);
            }
            // Construct event object
            SpaceLodEvent evt;
            evt.mSpaceObj = this;
            evt.mCamera = cam;
            // 告诉LOD事件监听器
            cam->getManager()->onLodEvent(evt);
        }
        mListenerCtl = mListener && !mListener->render(this, cam);
    }
    //-----------------------------------------------------------------------
    void SpaceObj::setRenderGroup(Nui16 qid, Nui16 level)
    {
        N_assert(qid <= RQG_Count, "Render queue out of range!");
        mRenderGroup = qid;
        mRenderLevel = level;
    }
    //-----------------------------------------------------------------------
    Nui16 SpaceObj::getRenderGroup() const
    {
        return mRenderGroup;
    }
    //-----------------------------------------------------------------------
    void SpaceObj::setListener(SpaceObjListener * obj)
    {
        mListener = obj;
        if(mListener)
            mListener->create(this);
    }
    //-----------------------------------------------------------------------
    const Matrix4f & SpaceObj::getSpaceMatrix() const
    {
        if(mSpaceNode)
        {
            return mSpaceNode->getSpaceMatrix();
        }
        return Matrix4f::IDENTITY;
    }
    //-----------------------------------------------------------------------
    const Vector3f & SpaceObj::getSpacePos() const
    {
        return Vector3f::ZERO;
    }
    //-----------------------------------------------------------------------
    const Vector3f & SpaceObj::getRelSpacePos() const
    {
        return Vector3f::ZERO;
    }
    //-----------------------------------------------------------------------
    const Vector3f & SpaceObj::getSpaceDirection() const
    {
        return Vector3f::ZERO;
    }
    //-----------------------------------------------------------------------
    const AABox & SpaceObj::getSpaceAABB(bool update) const
    {
        if(update)
        {
            mSpaceAABB = this->getAABB();
            mSpaceAABB.affine(getSpaceMatrix());
        }
        return mSpaceAABB;
    }
    //-----------------------------------------------------------------------
    const Sphere & SpaceObj::getSpaceBS(bool update) const
    {
        if(update)
        {
            const Vector3f & scl = mSpaceNode->getSpaceScale();
            NIIf factor = std::max(std::max(scl.x, scl.y), scl.z);
            mSpaceBS.setRadius(getCenterRange() * factor);
            mSpaceBS.setCenter(mSpaceNode->getSpacePos());
        }
        return mSpaceBS;
    }
    //-----------------------------------------------------------------------
    void SpaceObj::query(LightList & ll) const
    {
        if(mListener)
        {
            mListener->query(this, ll);
            if(ll.size() != 0)
            {
                return;
            }
        }

        // 从父实体中获取
        if(mTagNode)
        {
            TagBone * tp = static_cast<TagBone *>(mSpaceNode);
            static_cast<GeometrySpace *>(tp->getOwner())->query(ll);
            return;
        }

        if(mSpaceNode)
        {
            SpaceNode * sn = static_cast<SpaceNode *>(mSpaceNode);

            Nmark frame = sn->getCreator()->getLightDirtyMark();
            if(mLightDirtyMark != frame)
            {
                const_cast<SpaceObj*>(this)->mLightDirtyMark = frame;

                const Vector3f & scl = mSpaceNode->getSpaceScale();
                NIIf factor = std::max(std::max(scl.x, scl.y), scl.z);

                sn->findLights(const_cast<SpaceObj*>(this)->mLightList, getCenterRange() * factor, mLightMask);
            }
        }
        else
        {
            const_cast<SpaceObj*>(this)->mLightList.clear();
        }
        ll = mLightList;
    }
    //-----------------------------------------------------------------------
    Nmark SpaceObj::getTypeMark() const
    {
        if(mFactory)
        {
            return mFactory->getTypeMark();
        }
        else
        {
            return 0xFFFFFFFF;
        }
    }
    //-----------------------------------------------------------------------
    Nmark SpaceObj::getComMark() const
    {
        return mComMark;
    }
    //---------------------------------------------------------------------
    void SpaceObj::queue(RenderQueue * rq, Camera * cam, bool onlycast)
    {
        _notify(cam);
        if(isVisible())
        {
            bool receiveShadows = 
                rq->getGroup(getRenderGroup())->isShadowEnable() && isReceiveShadow();

            if(!onlycast || isCastShadow())
            {
                queue(rq);

                cam->expandBound(getSpaceAABB(true), getSpaceBS(true), receiveShadows ? 
                    ShadowType::ST_Receive | ShadowType::ST_Normal : ShadowType::ST_Normal);
            }
            // not shadow caster, receiver only?
            else if(onlycast && !isCastShadow() && receiveShadows)
            {
                cam->expandBound(getSpaceAABB(true), getSpaceBS(true), ShadowType::ST_Cast);
            }
        }
    }
    //---------------------------------------------------------------------
    class SORecvShadVisitor : public GeometryQuery
    {
    public:
        SORecvShadVisitor() :
            mReceive(false){}

        void query(GeometryObj * obj)
        {
            ShaderFusion * tech = obj->getShaderFusion();
            bool techReceivesShadows = tech && tech->getParent()->isReceiveShadow();
            mReceive = mReceive || techReceivesShadows || !tech;
        }

        bool mReceive;
    };
    //---------------------------------------------------------------------
    bool SpaceObj::isReceiveShadow()
    {
        SORecvShadVisitor visitor;
        query(&visitor);
        return visitor.mReceive;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
}