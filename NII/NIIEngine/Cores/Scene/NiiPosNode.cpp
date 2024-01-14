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
#include "NiiPosNode.h"
#include "NiiMaterialManager.h"
#include "NiiMeshManager.h"
#include "NiiGeometryObj.h"
#include "NiiCustomSpaceObj.h"
#include "NiiEngine.h"

namespace NII
{
    /// 内部类,用于节点可渲染物调试显示
    class _EngineInner PosNodeDebugObj : public GeometryObj, public SpaceAlloc
    {
    public:
        PosNodeDebugObj(PosNode * parent) :
            mParent(parent)
        {
            mMat = static_cast<Material *>(N_Only(Material).get(PosNode::mDebugMatRid));
            if(mMat == 0)
            {
                mMat = static_cast<Material *>(N_Only(Material).create(PosNode::mDebugMatRid, GroupInner));
                ShaderCh * ch = mMat->get(0)->get(0);
                ch->setLight(false);
                ch->setAutoReduce(false);
                ch->setColour(ShaderCh::SOT_Set)->setColourTracking(CTM_T_AMBIENT);
                ch->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_INV_ALPHA);
                ch->setCullingMode(CM_None);
                ch->setStencil(ShaderCh::SOT_Set)->setDepthWrite(false);
                mMat->ref(false);
            }

            mMeshPtr = static_cast<Mesh *>(N_Only(Mesh).get(PosNode::mDebugMeshRid));
            if(mMeshPtr == 0)
            {
                CustomSpaceObj mo(0);
                mo.begin(GeometryRaw::OT_TRIANGLE_LIST, mMat->getOriginID());
                /* 3 axes, each made up of 2 of these (base plane = XY)
                 *   .------------|\
                 *   '------------|/
                 */
                mo.reserveVertexCount(7 * 2 * 3);
                mo.reserveIndexCount(3 * 2 * 3);
                Quaternion quat[6];
                Colour col[3];

                // x-axis
                quat[0] = Quaternion::IDENTITY;
                quat[1].from(Vector3f::X, Vector3f::INV_Z, Vector3f::Y);
                col[0] = Colour::Red;
                col[0].a = 0.8f;
                // y-axis
                quat[2].from(Vector3f::Y, Vector3f::INV_X, Vector3f::Z);
                quat[3].from(Vector3f::Y, Vector3f::Z, Vector3f::X);
                col[1] = Colour::Green;
                col[1].a = 0.8f;
                // z-axis
                quat[4].from(Vector3f::Z, Vector3f::Y, Vector3f::INV_X);
                quat[5].from(Vector3f::Z, Vector3f::X, Vector3f::Y);
                col[2] = Colour::Blue;
                col[2].a = 0.8f;

                Vector3f basepos[7] =
                {
                    // stalk
                    Vector3f(0, 0.05f, 0),
                    Vector3f(0, -0.05f, 0),
                    Vector3f(0.7f, -0.05f, 0),
                    Vector3f(0.7f, 0.05f, 0),
                    // head
                    Vector3f(0.7f, -0.15f, 0),
                    Vector3f(1, 0, 0),
                    Vector3f(0.7f, 0.15f, 0)
                };

                // vertices
                // 6 arrows
                for(NCount i = 0; i < 6; ++i)
                {
                    // 7 points
                    for (NCount p = 0; p < 7; ++p)
                    {
                        Vector3f pos = quat[i] * basepos[p];
                        mo.position(pos.x, pos.y, pos.z);
                        mo.colour(col[i / 2]);
                    }
                }

                // indices
                // 6 arrows
                for(NCount i = 0; i < 6; ++i)
                {
                    NCount base = i * 7;
                    mo.triangle(base + 0, base + 1, base + 2);
                    mo.triangle(base + 0, base + 2, base + 3);
                    mo.triangle(base + 4, base + 5, base + 6);
                }

                mo.end();

                mMeshPtr = mo.createMesh(PosNode::mDebugMeshRid, GroupInner);
                mMeshPtr->ref(false);
            }
        }
        ~PosNodeDebugObj()
        {
            if(mMeshPtr)
                mMeshPtr->unref();
            if(mMat)
                mMat->unref();
        }

        /// @copydetails GeometryObj::getMaterial
        const Material * getMaterial() const
        {
            return mMat;
        }

        /// @copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const
        {
            return mMeshPtr->getSub(0)->getGeometry(op);
        }

        /// @copydetails GeometryObj:getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const
        {
            // Assumes up to date
            *out = mParent->getSpaceMatrix();
            if(!Math::isEqual(mScaling, 1.0))
            {
                Matrix4f m = Matrix4f::IDENTITY;
                Vector3f s(mScaling, mScaling, mScaling);
                m.setScale(s);
                *out = (*out) * m;
            }
            count = 1;
        }

        /// @copydetails GeometryObj:distanceSquared
        NIIf distanceSquared(const Camera * obj) const
        {
            return mParent->distanceSquared(obj);
        }

        /// @copydetails GeometryObj:getLight
        void getLight(LightList & ll) const
        {
            ll.clear();
        }

        /// 设置缩放
        inline void setScaling(NIIf s)
        {
            mScaling = s;
        }
    protected:
        PosNode * mParent;      ///< 这个对象的依赖对象
        Mesh * mMeshPtr;        ///< 这个调试对象使用的网格
        Material * mMat;        ///< 这个调试对象使用的材质
        NIIf mScaling;          ///< 这个对象实际应用时的缩放因子
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PosListener
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PosListener::PosListener()
    {
    }
    //-----------------------------------------------------------------------
    PosListener::~PosListener() 
    {
    }
    //-----------------------------------------------------------------------
    void PosListener::create(PosNode *)
    {
    }
    //-----------------------------------------------------------------------
    void PosListener::add(PosNode *, PosNode *)
    {
    }
    //-----------------------------------------------------------------------
    void PosListener::remove(PosNode *, PosNode *)
    {
    }
    //-----------------------------------------------------------------------
    void PosListener::update(PosNode *) 
    {
    }
    //-----------------------------------------------------------------------
    void PosListener::destroy(PosNode *) 
    {
    }
    //-----------------------------------------------------------------------
    void PosListener::attach(PosNode *) 
    {
    }
    //-----------------------------------------------------------------------
    void PosListener::detach(PosNode *) 
    {
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpaceID PosNode::mValidID = N_Max_Valid_ID;
    ResourceID PosNode::mDebugMeshRid = N_Max_Valid_ID - 1;
    ResourceID PosNode::mDebugMatRid = N_Max_Valid_ID - 1;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PosNode
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PosNode::PosNode() :
        mParent(0),
        mPosUnvalid(false),
        mChildUnvalid(false),
        mParentValid(false),
        mRootUpdate(false),
        mOri(Quaternion::IDENTITY),
        mPos(Vector3f::ZERO),
        mScale(Vector3f::UNIT),
        mEvnPos(true),
        mEvnOri(true),
        mEvnScale(true),
        mSpaceOri(Quaternion::IDENTITY),
        mSpacePos(Vector3f::ZERO),
        mSpaceScale(Vector3f::UNIT),
        mSpaceMatrixValid(false),
        mListener(0),
        mDebug(0)
    {
        notify();
        N_lock_mutex(mValidIDMutex)
        mID = ++mValidID;
    }
    //-----------------------------------------------------------------------
    PosNode::PosNode(SpaceID id) :
        mParent(0),
        mPosUnvalid(false),
        mChildUnvalid(false),
        mParentValid(false),
        mRootUpdate(false),
        mOri(Quaternion::IDENTITY),
        mPos(Vector3f::ZERO),
        mScale(Vector3f::UNIT),
        mEvnPos(true),
        mEvnOri(true),
        mEvnScale(true),
        mSpaceOri(Quaternion::IDENTITY),
        mSpacePos(Vector3f::ZERO),
        mSpaceScale(Vector3f::UNIT),
        mSpaceMatrixValid(false),
        mListener(0),
        mDebug(0)
    {
        notify();
        
        if(id == 0)
        {
            N_lock_mutex(mValidIDMutex)
            mID = ++mValidID;
        }
        else
        {
            mID = id;
        }
    }
    //-----------------------------------------------------------------------
    PosNode::~PosNode()
    {
        if(mDebug)
        {
            N_delete mDebug;
            mDebug = 0;
        }
        // Call listener (note, only called if there's something to do)
        if(mListener)
        {
            mListener->destroy(this);
        }

        removeAll();
        if(mParent)
            mParent->remove(this);

        if(mRootUpdate)
        {
            N_Engine().removeUpdate(this);
        }
    }
    //-----------------------------------------------------------------------
    void PosNode::setParent(PosNode * parent)
    {
        bool different = (parent != mParent);

        mParent = parent;
        mParentValid = false ;
        notify();

        if(mListener && different)
        {
            if(mParent)
                mListener->attach(this);
            else
                mListener->detach(this);
        }
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::getParent() const
    {
        return mParent;
    }
    //-----------------------------------------------------------------------
    const Matrix4f & PosNode::getSpaceMatrix() const
    {
        if(!mSpaceMatrixValid)
        {
            // Use derived values
            mSpaceMatrix.set(getSpacePos(), getSpaceScale(), getSpaceOri());

            mSpaceMatrixValid = true;
        }
        return mSpaceMatrix;
    }
    //-----------------------------------------------------------------------
    void PosNode::update(bool own, bool child)
    {
        mParentValid = false;

        // See if we should process everyone
        if(mPosUnvalid || own)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if(mListener)
            {
                mListener->update(this);
            }
        }

        if(child)
        {
            if(mChildUnvalid || own)
            {
                ChildList::iterator it, itend = mChildList.end();
                for(it = mChildList.begin(); it != itend; ++it)
                {
                    PosNode * obj = it->second;
                    obj->update(true, true);
                }
            }
            else
            {
                // Just update selected children
                Childs::iterator it, itend = mUpdateList.end();
                for(it = mUpdateList.begin(); it != itend; ++it)
                {
                    PosNode * obj = *it;
                    obj->update(false, true);
                }
            }
            mUpdateList.clear();
            mChildUnvalid = false;
        }
    }
    //-----------------------------------------------------------------------
    void PosNode::updateImpl() const
    {
        if(mParent)
        {
            // 更新方向
            const Quaternion & parentOrientation = mParent->getSpaceOri();
            if(mEvnOri)
            {
                // Combine orientation with that of parent
                mSpaceOri = parentOrientation * mOri;
            }
            else
            {
                // No inheritence
                mSpaceOri = mOri;
            }

            // 更新缩放
            const Vector3f & parentScale = mParent->getSpaceScale();
            if(mEvnScale)
            {
                // Scale own position by parent scale, NB just combine
                // as equivalent axes, no shearing
                mSpaceScale = parentScale * mScale;
            }
            else
            {
                // No inheritence
                mSpaceScale = mScale;
            }

            // 更新位置因子(依赖于父节点方向和缩放)
            mSpacePos = parentOrientation * (parentScale * mPos);
            // 更新最终的位置
            mSpacePos += mParent->getSpacePos();
        }
        else
        {
            mSpaceOri = mOri;
            mSpacePos = mPos;
            mSpaceScale = mScale;
        }
        mSpaceMatrixValid = false;
        mPosUnvalid = false;
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::create(const Vector3f & oft, const Quaternion & rot)
    {
        PosNode * re = createImpl();
        re->translate(oft);
        re->rotate(rot);
        add(re);

        return re;
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::create(SpaceID id, const Vector3f & oft, const Quaternion & rot)
    {
        PosNode * re = createImpl(id);
        re->translate(oft);
        re->rotate(rot);
        add(re);

        return re;
    }
    //-----------------------------------------------------------------------
    void PosNode::add(PosNode * obj)
    {
        if(obj->mParent)
        {
            N_EXCEPT(InvalidParam, _I18n("PosNode :'") + N_conv(obj->getID()) +
                _I18n("' already was a child of '") + N_conv(obj->mParent->getID()) + _T("'."));
        }

        mChildList.insert(ChildList::value_type(obj->getID(), obj));
        obj->setParent(this);
        if(mListener)
            mListener->add(this, obj);
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::remove(Nidx index)
    {
        PosNode * re;
        if(index < mChildList.size())
        {
            ChildList::iterator i = mChildList.begin();
            std::advance(i, index);
            re = i->second;
            // cancel any pending update
            removeUpdateImpl(re);

            mChildList.erase(i);
            re->setParent(NULL);
            if(mListener)
                mListener->remove(this, re);
            return re;
        }
        else
        {
            N_EXCEPT(InvalidParam, _I18n("Child index out of bounds."));
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::remove(SpaceID id)
    {
        ChildList::iterator i = mChildList.find(id);
        if(i == mChildList.end())
        {
            N_EXCEPT(NotExist, _I18n("Child node named :") + N_conv(id) + _I18n(" does not exist."));
        }

        PosNode * re = i->second;
        // Cancel any pending update
        removeUpdateImpl(re);

        mChildList.erase(i);
        re->setParent(NULL);
        if(mListener)
            mListener->remove(this, re);
        return re;
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::remove(PosNode * child)
    {
        if(child)
        {
            ChildList::iterator i = mChildList.find(child->getID());
            if(i != mChildList.end() && i->second == child)
            {
                // cancel any pending update
                removeUpdateImpl(child);

                mChildList.erase(i);
                child->setParent(NULL);
                if(mListener)
                    mListener->remove(this, child);
            }
        }

        return child;
    }
    //-----------------------------------------------------------------------
    void PosNode::removeAll()
    {
        ChildList::iterator i, iend = mChildList.end();
        for (i = mChildList.begin(); i != iend; ++i)
        {
            i->second->setParent(0);
        }
        mChildList.clear();
        mUpdateList.clear();
    }
    //-----------------------------------------------------------------------
    void PosNode::destroy(Nidx index, bool r)
    {
        PosNode * c = get(index);
        if(r)
            c->destroyAll(r);
        else
        {
            ChildList::iterator i, iend = c->mChildList.end();
            for(i = c->mChildList.begin(); i != iend;)
            {
                i->second->mParent = 0;
            }
        }

        remove(index);
        destroyImpl(c->getID());
    }
    //-----------------------------------------------------------------------
    void PosNode::destroy(SpaceID sid, bool r)
    {
        PosNode * c = get(sid);
        if(r)
            c->destroyAll(r);
        else
        {
            ChildList::iterator i, iend = c->mChildList.end();
            for(i = c->mChildList.begin(); i != iend;)
            {
                i->second->mParent = 0;
            }
        }
        remove(sid);
        destroyImpl(sid);
    }
    //-----------------------------------------------------------------------
    void PosNode::destroyAll(bool r)
    {
        ChildList::iterator i, iend = mChildList.end();
        for(i = mChildList.begin(); i != iend;)
        {
            PosNode * sn = i->second;
            // increment iterator before destroying (iterator invalidated by
            // SpaceManager::destroySpaceNode because it causes removal from parent)
            ++i;
            if(r)
                sn->destroyAll(r);
            else
            {
                ChildList::iterator i, iend = sn->mChildList.end();
                for(i = sn->mChildList.begin(); i != iend;)
                {
                    i->second->mParent = 0;
                }
            }
            destroyImpl(sn->getID());
        }
        mChildList.clear();
        notify();
    }
    //-----------------------------------------------------------------------
    NCount PosNode::getCount() const
    {
        return mChildList.size();
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::get(Nidx index) const
    {
        if(index < mChildList.size())
        {
            ChildList::const_iterator i = mChildList.begin();

            while(index--)
                ++i;

            return i->second;
        }
        else
            return NULL;
    }
    //-----------------------------------------------------------------------
    PosNode * PosNode::get(SpaceID id) const
    {
        ChildList::const_iterator i = mChildList.find(id);
        if(i == mChildList.end())
        {
            N_EXCEPT(NotExist, _I18n("Child node named :") + N_conv(id) + _I18n(" does not exist."));
        }
        return i->second;
    }
    //-----------------------------------------------------------------------
    const Quaternion & PosNode::getOri() const
    {
        return mOri;
    }
    //-----------------------------------------------------------------------
    void PosNode::setOri(const Quaternion & q)
    {
        N_assert(!q.isNaN(), "Invalid orientation supplied as parameter");
        mOri = q;
        mOri.normalise();
        notify();
    }
    //-----------------------------------------------------------------------
    void PosNode::setPos(const Vector3f & pos)
    {
        mPos = pos;
        notify();
    }
    //-----------------------------------------------------------------------
    const Vector3f & PosNode::getPos() const
    {
        return mPos;
    }
    //-----------------------------------------------------------------------
    void PosNode::translate(const Vector3f & d, SpaceMode mode)
    {
        switch(mode)
        {
        case SM_Local:
            // position is relative to parent so transform downwards
            mPos += mOri * d;
            break;
        case SM_Space:
            // position is relative to parent so transform upwards
            if(mParent)
            {
                mPos += (mParent->getSpaceOri().inverse() * d) / mParent->getSpaceScale();
            }
            else
            {
                mPos += d;
            }
            break;
        case SM_Env:
            mPos += d;
            break;
        }
        notify();
    }
    //-----------------------------------------------------------------------
    void PosNode::roll(const Radian & angle, SpaceMode mode)
    {
        rotate(Vector3f::Z, angle, mode);
    }
    //-----------------------------------------------------------------------
    void PosNode::pitch(const Radian & angle, SpaceMode mode)
    {
        rotate(Vector3f::X, angle, mode);
    }
    //-----------------------------------------------------------------------
    void PosNode::yaw(const Radian & angle, SpaceMode mode)
    {
        rotate(Vector3f::Y, angle, mode);
    }
    //-----------------------------------------------------------------------
    void PosNode::rotate(const Vector3f & axis, const Radian & angle, SpaceMode mode)
    {
        Quaternion q;
        q.from(angle, axis);
        rotate(q, mode);
    }
    //-----------------------------------------------------------------------
    void PosNode::rotate(const Quaternion & q, SpaceMode mode)
    {
        // Normalise quaternion to avoid drift
        Quaternion qnorm = q;
        qnorm.normalise();

        switch(mode)
        {
        case SM_Env:
            // Rotations are normally relative to local axes, transform up
            mOri = qnorm * mOri;
            break;
        case SM_Space:
            // Rotations are normally relative to local axes, transform up
            mOri = mOri * getSpaceOri().inverse() * qnorm * getSpaceOri();
            break;
        case SM_Local:
            // Note the order of the mult, i.e. q comes after
            mOri = mOri * qnorm;
            break;
        }
        notify();
    }
    //-----------------------------------------------------------------------
    void PosNode::setSpacePos(const Vector3f & pos)
    {
        //find where the node would end up in parent's local space
        setPos(mParent->toLocal(pos));
    }
    //-----------------------------------------------------------------------
    void PosNode::setSpaceOri(const Quaternion & q)
    {
        //find where the node would end up in parent's local space
        setOri(mParent->toLocal(q));
    }
    //-----------------------------------------------------------------------
    const Quaternion & PosNode::getSpaceOri() const
    {
        if(mPosUnvalid)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if (mListener)
            {
                mListener->update(const_cast<PosNode *>(this));
            }
        }
        return mSpaceOri;
    }
    //-----------------------------------------------------------------------
    const Vector3f & PosNode::getSpacePos() const
    {
        if(mPosUnvalid)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if(mListener)
            {
                mListener->update(const_cast<PosNode *>(this));
            }
        }
        return mSpacePos;
    }
    //-----------------------------------------------------------------------
    const Vector3f & PosNode::getSpaceScale() const
    {
        if(mPosUnvalid)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if (mListener)
            {
                mListener->update(const_cast<PosNode *>(this));
            }
        }
        return mSpaceScale;
    }
    //-----------------------------------------------------------------------
    Vector3f PosNode::toLocal(const Vector3f & worldPos)
    {
        if(mPosUnvalid)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if(mListener)
            {
                mListener->update(this);
            }
        }
        return mSpaceOri.inverse() * (worldPos - mSpacePos) / mSpaceScale;
    }
    //-----------------------------------------------------------------------
    Vector3f PosNode::toSpace(const Vector3f & localPos)
    {
        if(mPosUnvalid)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if (mListener)
            {
                mListener->update(this);
            }
        }
        return (mSpaceOri * (localPos * mSpaceScale)) + mSpacePos;
    }
    //-----------------------------------------------------------------------
    Quaternion PosNode::toLocal(const Quaternion & worldOrientation)
    {
        if(mPosUnvalid)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if (mListener)
            {
                mListener->update(this);
            }
        }
        return mSpaceOri.inverse() * worldOrientation;
    }
    //-----------------------------------------------------------------------
    Quaternion PosNode::toSpace(const Quaternion & localOrientation)
    {
        if(mPosUnvalid)
        {
            updateImpl();

            // Call listener (note, this method only called if there's something to do)
            if (mListener)
            {
                mListener->update(this);
            }
        }
        return mSpaceOri * localOrientation;
    }

    //-----------------------------------------------------------------------
    void PosNode::setScale(const Vector3f & scale)
    {
        mScale = scale;
        notify();
    }
    //-----------------------------------------------------------------------
    const Vector3f & PosNode::getScale() const
    {
        return mScale;
    }
    //-----------------------------------------------------------------------
    void PosNode::set(const Vector3f & p, const Vector3f & s, const Quaternion & q)
    {
        mPos = p;
        mScale = s;
        mOri = q;
        mOri.normalise();
        notify();
    }
    //-----------------------------------------------------------------------
    void PosNode::setEnvPos(bool b)
    {
        mEvnPos = b;
    }
    //-----------------------------------------------------------------------
    bool PosNode::isEnvPos() const
    {
        return mEvnPos;
    }
    //-----------------------------------------------------------------------
    void PosNode::setEnvOri(bool b)
    {
        mEvnOri = b;
        notify();
    }
    //-----------------------------------------------------------------------
    bool PosNode::isEnvOri() const
    {
        return mEvnOri;
    }
    //-----------------------------------------------------------------------
    void PosNode::setEnvScale(bool b)
    {
        mEvnScale = b;
        notify();
    }
    //-----------------------------------------------------------------------
    bool PosNode::isEnvScale() const
    {
        return mEvnScale;
    }
    //-----------------------------------------------------------------------
    void PosNode::scale(const Vector3f & scale)
    {
        mScale *= scale;
        notify();
    }
    //-----------------------------------------------------------------------
    NIIf PosNode::distanceSquared(const Camera * cam) const
    {
        Vector3f dis = getSpacePos() - cam->getSpacePos();
        // NB use squared length rather than real depth to avoid square root
        return dis.lengthSquared();
    }
    //-----------------------------------------------------------------------
    void PosNode::addUpdateImpl(PosNode * child, bool force)
    {
        // If we're already going to update everything this doesn't matter
        if(mChildUnvalid)
        {
            return;
        }

        mUpdateList.insert(child);
        // Request selective update of me, if we didn't do it before
        if(mParent && (!mParentValid || force))
        {
            mParent->addUpdateImpl(this, force);
            mParentValid = true ;
        }
    }
    //-----------------------------------------------------------------------
    void PosNode::notify(bool force)
    {
        mPosUnvalid = true;
        mChildUnvalid = true;
        mSpaceMatrixValid = false;

        // Make sure we're not root and parent hasn't been notified before
        if(mParent && (!mParentValid || force))
        {
            mParent->addUpdateImpl(this, force);
            mParentValid = true ;
        }

        // all children will be update
        mUpdateList.clear();
    }
    //-----------------------------------------------------------------------
    void PosNode::removeUpdateImpl(PosNode * child)
    {
        mUpdateList.erase(child);

        // Propogate this up if we're done
        if(mUpdateList.empty() && mParent && !mChildUnvalid)
        {
            mParent->removeUpdateImpl(this);
            mParentValid = false ;
        }
    }
    //---------------------------------------------------------------------
    GeometryObj * PosNode::getGeometry(NIIf scale)
    {
        if(!mDebug)
        {
            mDebug = N_new PosNodeDebugObj(this);
        }
        mDebug->setScaling(scale);
        return mDebug;
    }
    //-----------------------------------------------------------------------
}