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
#include "NiiSpaceNode.h"
#include "NiiSpaceObj.h"
#include "NiiException.h"
#include "NiiGeometrySpace.h"
#include "NiiCamera.h"
#include "NiiLight.h"
#include "NiiMath.h"
#include "NiiSpaceManager.h"
#include "NiiWireAABGeo.h"

namespace NII
{
    //-----------------------------------------------------------------------
    SpaceNode::SpaceNode(SpaceManager * creator, SpaceID id) :
        PosNode(id),
        mWorldAABBGeo(0),
        mCreator(creator),
        mAutoFocus(0),
        mEnable(false),
        mCustomYaw(false),
        mAABBView(false),
        mNodeView(false)
    {
        notify();
    }
    //-----------------------------------------------------------------------
    SpaceNode::~SpaceNode()
    {
        // Detach all objects, do this manually to avoid notify() call
        // which can fail because of deleted items
        SpaceObj * so;
        AttachList::iterator i, iend = mAttachList.end();
        for(i = mAttachList.begin(); i != iend; ++i)
        {
            so = i->second;
            so->attach((SpaceNode*)0);
        }
        mAttachList.clear();

        if(mWorldAABBGeo)
            N_delete mWorldAABBGeo;
    }
    //-----------------------------------------------------------------------
    void SpaceNode::update(bool own, bool child)
    {
        PosNode::update(own, child);
        updateSpaceAABB();
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setParent(PosNode * parent)
    {
        PosNode::setParent(parent);

        if(parent)
        {
            SpaceNode * p = static_cast<SpaceNode *>(parent);
            setEnable(p->isEnable());
        }
        else
        {
            setEnable(false);
        }
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setEnable(bool b)
    {
        if(b != mEnable)
        {
            mEnable = b;
            ChildList::iterator c, cend = mChildList.end();
            for(c = mChildList.begin(); c != cend; ++c)
            {
                static_cast<SpaceNode *>(c->second)->setEnable(b);
            }
        }
    }
    //-----------------------------------------------------------------------
    bool SpaceNode::isEnable() const
    {
        return mEnable;
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setNodeView(bool b)
    {
        if(mNodeView != b)
        {
            mNodeView = b;
            ChildList::iterator c, cend = mChildList.end();
            for(c = mChildList.begin(); c != cend; ++c)
            {
                static_cast<SpaceNode *>(c->second)->setNodeView(b);
            }
        }
    }
    //-----------------------------------------------------------------------
    bool SpaceNode::isNodeView() const
    {
        return mNodeView;
    }
    //-----------------------------------------------------------------------
    void SpaceNode::attach(SpaceObj * obj)
    {
        if(obj->isAttach())
        {
            N_EXCEPT(InvalidParam, _I18n("Object already attached to a SpaceNode or a Bone"));
        }

        obj->attach(this);

        // Also add to name index
        std::pair<AttachList::iterator, bool> insresult =
            mAttachList.insert(AttachList::value_type(obj->getID(), obj));
        N_assert(insresult.second, "Object was not attached because an object of the "
            "same name was already attached to this node.");

        // Make sure bounds get updated (must go right to the top)
        notify();
    }
    //-----------------------------------------------------------------------
    SpaceObj * SpaceNode::getAttach(Nidx index)
    {
        if(index < mAttachList.size())
        {
            AttachList::iterator i = mAttachList.begin();
            // Increment (must do this one at a time)
            while(index--)
                ++i;

            return i->second;
        }
        else
        {
            N_EXCEPT(InvalidParam, _I18n("Object index out of bounds."));
        }
    }
    //-----------------------------------------------------------------------
    SpaceObj * SpaceNode::getAttach(SpaceID sid)
    {
        // Look up
        AttachList::iterator i = mAttachList.find(sid);
        if(i == mAttachList.end())
        {
            N_EXCEPT(NotExist, _I18n("Attached object ") + N_conv(sid) + _I18n(" not found."));
        }
        return i->second;
    }
    //-----------------------------------------------------------------------
    SpaceObj * SpaceNode::detach(Nidx index)
    {
        SpaceObj * ret;
        if (index < mAttachList.size())
        {

            AttachList::iterator i = mAttachList.begin();
            std::advance(i, index);
            ret = i->second;
            mAttachList.erase(i);
            ret->attach((SpaceNode *)0);

            // Make sure bounds get updated (must go right to the top)
            notify();

            return ret;
        }
        else
        {
            N_EXCEPT(InvalidParam, _I18n("Object index out of bounds."));
        }
    }
    //-----------------------------------------------------------------------
    SpaceObj * SpaceNode::detach(SpaceID sid)
    {
        AttachList::iterator it = mAttachList.find(sid);
        if(it == mAttachList.end())
        {
            N_EXCEPT(NotExist, _I18n("Object ") + N_conv(sid) + _I18n(" is not attached to this node."));
        }

        SpaceObj * so = it->second;
        mAttachList.erase(it);
        so->attach((SpaceNode *)0);
        // Make sure bounds get updated (must go right to the top)
        notify();

        return so;
    }
    //-----------------------------------------------------------------------
    void SpaceNode::detach(SpaceObj * obj)
    {
        AttachList::iterator i, iend = mAttachList.end();
        for(i = mAttachList.begin(); i != iend; ++i)
        {
            if(i->second == obj)
            {
                mAttachList.erase(i);
                break;
            }
        }
        obj->attach((SpaceNode *)0);

        // Make sure bounds get updated (must go right to the top)
        notify();
    }
    //-----------------------------------------------------------------------
    void SpaceNode::detachAll()
    {
        SpaceObj * ret;
        AttachList::iterator itr, itrend = mAttachList.end();
        for(itr = mAttachList.begin(); itr != itrend; ++itr)
        {
            ret = itr->second;
            ret->attach((SpaceNode *)0);
        }
        mAttachList.clear();
        // Make sure bounds get updated (must go right to the top)
        notify();
    }
    //-----------------------------------------------------------------------
    void SpaceNode::updateSpaceAABB()
    {
        // Reset bounds first
        mWorldAABB.setNull();

        // Update bounds from own attached objects
        AttachList::iterator i, iend = mAttachList.end();
        for(i = mAttachList.begin(); i != iend; ++i)
        {
            // Merge world bounds of each object
            mWorldAABB.merge(i->second->getSpaceAABB(true));
        }

        // Merge with children
        ChildList::iterator child;
        for(child = mChildList.begin(); child != mChildList.end(); ++child)
        {
            SpaceNode * sceneChild = static_cast<SpaceNode *>(child->second);
            mWorldAABB.merge(sceneChild->mWorldAABB);
        }
    }
    //-----------------------------------------------------------------------
    const AABox & SpaceNode::getSpaceAABB() const
    {
        return mWorldAABB;
    }
    //-----------------------------------------------------------------------
    void SpaceNode::queue(RenderQueue * queue, Camera * cam, bool r, bool onlycast)
    {
        // 检查自我可见
        if(!cam->isIntersect(mWorldAABB))
            return;

        AttachList::iterator iobj, iobjend = mAttachList.end();
        for(iobj = mAttachList.begin(); iobj != iobjend; ++iobj)
        {
            SpaceObj * mo = iobj->second;

            mo->queue(queue, cam, onlycast);
        }

        if(r)
        {
            ChildList::iterator c, cend = mChildList.end();
            for(c = mChildList.begin(); c != cend; ++c)
            {
                SpaceNode * sceneChild = static_cast<SpaceNode *>(c->second);
                sceneChild->queue(queue, cam, r, onlycast);
            }
        }

        if(mNodeView)
        {
            queue->add(getGeometry());
        }

        // 检查是否显示边界盒,查看我们的标记设置或场景管理器标记设置
        if(mAABBView || (mCreator && mCreator->isAABBView()))
        {
            queueAABB(queue);
        }
    }
    //-----------------------------------------------------------------------
    GeometryObj * SpaceNode::getGeometry()
    {
        Vector3f hs = mWorldAABB.getSize() * 0.5;
        NIIf sz = std::min(hs.x, hs.y);
        sz = std::min(sz, hs.z);
        sz = std::max(sz, (NIIf)1.0);
        return PosNode::getGeometry(sz);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::queueAABB(RenderQueue * queue)
    {
        // Create a WireAABGeo if needed.
        if(mWorldAABBGeo == NULL)
            mWorldAABBGeo = N_new WireAABGeo();

        static_cast<WireAABGeo *>(mWorldAABBGeo)->setAABBGeo(mWorldAABB);
        queue->add(mWorldAABBGeo);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setAABBView(bool b)
    {
        mAABBView = b;
    }
    //-----------------------------------------------------------------------
    bool SpaceNode::isAABBView() const
    {
        return mAABBView;
    }
    //-----------------------------------------------------------------------
    void SpaceNode::updateImpl() const
    {
        PosNode::updateImpl();

        // Notify objects that it has been moved
        AttachList::const_iterator i, iend = mAttachList.end();
        for (i = mAttachList.begin(); i != iend; ++i)
        {
            SpaceObj * so = i->second;
            so->_notify();
        }
    }
    //-----------------------------------------------------------------------
    PosNode * SpaceNode::createImpl(SpaceID id)
    {
        N_assert(mCreator, "error");
        return mCreator->createSpaceNode(id);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::destroyImpl(SpaceID id)
    {
        N_assert(mCreator, "error");
        mCreator->destroySpaceNode(id);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::findLights(LightList & dest, NIIf radius, Nmark mask) const
    {
        // No any optimisation here, hope inherits more smart for that.
        //
        // If a scene node is static and lights have moved, light list won't change
        // can't use a simple global boolean flag since this is only called for
        // visible nodes, so temporarily visible nodes will not be updated
        // Since this is only called for visible nodes, skip the check for now
        //
        if(mCreator)
        {
            // Use SpaceManager to calculate
            mCreator->findLight(dest, getSpacePos(), radius, mask);
        }
        else
        {
            dest.clear();
        }
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setAutoFocus(bool enabled, const SpaceNode * target,
        const Vector3f & offset, const Vector3f & dir)
    {
        if(enabled)
        {
            mAutoFocus = target;
            mAutoFocusPos = offset;
            mAutoFocusDir = dir;
        }
        else
        {
            mAutoFocus = 0;
        }

        if(mCreator)
            mCreator->setAutoFocus(this, enabled);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setCustomYaw(bool enable, const Vector3f & o)
    {
        mCustomYaw = enable;
        mCustomYawAxis = o;
    }
    //-----------------------------------------------------------------------
    void SpaceNode::yaw(const Radian & angle, SpaceMode relativeTo)
    {
        if(mCustomYaw)
        {
            rotate(mCustomYawAxis, angle, relativeTo);
        }
        else
        {
            rotate(Vector3f::Y, angle, relativeTo);
        }
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setDirection(NIIf x, NIIf y, NIIf z, SpaceMode relativeTo,
        const Vector3f & localDirectionVector)
    {
        setDirection(Vector3f(x, y, z), relativeTo, localDirectionVector);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::lookAt(const Vector3f & point, SpaceMode mode, const Vector3f & dir)
    {
        // Calculate ourself origin relative to the given transform space
        Vector3f origin;
        switch(mode)
        {
        default:
        case SM_Space:
            origin = getSpacePos();
            break;
        case SM_Env:
            origin = mPos;
            break;
        case SM_Local:
            origin = Vector3f::ZERO;
            break;
        }
        setDirection(point - origin, mode, dir);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setDirection(const Vector3f & vec, SpaceMode relativeTo,
        const Vector3f & localDirectionVector)
    {
        // Do nothing if given a zero vector
        if(vec == Vector3f::ZERO)
            return;

        // The direction we want the local direction point to
        Vector3f targetDir;
        vec.normalise(targetDir);

        // Transform target direction to world space
        switch(relativeTo)
        {
        case SM_Env:
            if(mEvnOri)
            {
                if(mParent)
                {
                    targetDir = mParent->getSpaceOri() * targetDir;
                }
            }
            break;
        case SM_Local:
            targetDir = getSpaceOri() * targetDir;
            break;
        case SM_Space:
            // default orientation
            break;
        }

        // Calculate target orientation relative to world space
        Quaternion targetOrientation;
        if(mCustomYaw)
        {
            // Calculate the quaternion for rotate local Z to target direction
            Vector3f xVec = mCustomYawAxis.crossProduct(targetDir);
            xVec.normalise();
            Vector3f yVec = targetDir.crossProduct(xVec);
            yVec.normalise();
            Quaternion unitZToTarget;
            unitZToTarget.from(xVec, yVec, targetDir);

            if(localDirectionVector == Vector3f::INV_Z)
            {
                // Specail case for avoid calculate 180 degree turn
                targetOrientation = Quaternion(-unitZToTarget.y, -unitZToTarget.z,
                    unitZToTarget.w, unitZToTarget.x);
            }
            else
            {
                // Calculate the quaternion for rotate local direction to target direction
                Quaternion localToUnitZ = Quaternion::getRotation(localDirectionVector, Vector3f::Z);
                targetOrientation = unitZToTarget * localToUnitZ;
            }
        }
        else
        {
            const Quaternion & currentOrient = getSpaceOri();

            // Get current local direction relative to world space
            Vector3f currentDir = currentOrient * localDirectionVector;

            if((currentDir+targetDir).lengthSquared() < 0.00005f)
            {
                // Oops, a 180 degree turn (infinite possible rotation axes)
                // Default to yaw i.e. use current UP
                targetOrientation =
                    Quaternion(-currentOrient.y, -currentOrient.z, currentOrient.w, currentOrient.x);
            }
            else
            {
                // Derive shortest arc to new direction
                Quaternion rotQuat = Quaternion::getRotation(currentDir, targetDir);
                targetOrientation = rotQuat * currentOrient;
            }
        }

        // Set target orientation, transformed to parent space
        if(mParent && mEvnOri)
            setOri(mParent->getSpaceOri().inverseUnit() * targetOrientation);
        else
            setOri(targetOrientation);
    }
    //-----------------------------------------------------------------------
    void SpaceNode::updateFocus()
    {
        // NB assumes that all scene nodes have been updated
        if(mAutoFocus)
        {
            lookAt(mAutoFocus->getSpacePos() + mAutoFocusPos, SM_Space, mAutoFocusDir);
            // update self & children
            update(true, true);
        }
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setVisible(bool visible, bool r)
    {
        AttachList::iterator oi, oiend = mAttachList.end();
        for(oi = mAttachList.begin(); oi != oiend; ++oi)
        {
            oi->second->setVisible(visible);
        }

        if(r)
        {
            ChildList::iterator i, iend = mChildList.end();
            for(i = mChildList.begin(); i != iend; ++i)
            {
                static_cast<SpaceNode *>(i->second)->setVisible(visible, r);
            }
        }
    }
    //-----------------------------------------------------------------------
    void SpaceNode::setDebugMode(bool enabled, bool cascade)
    {
        AttachList::iterator oi, oiend = mAttachList.end();
        for(oi = mAttachList.begin(); oi != oiend; ++oi)
        {
            oi->second->setDebugMode(enabled);
        }

        if(cascade)
        {
            ChildList::iterator i, iend = mChildList.end();
            for(i = mChildList.begin(); i != iend; ++i)
            {
                static_cast<SpaceNode*>(i->second)->setDebugMode(enabled, cascade);
            }
        }
    }
    //-----------------------------------------------------------------------
}