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

#ifndef _NII_POS_NODE_H_
#define _NII_POS_NODE_H_

#include "NiiPreInclude.h"
#include "NiiMatrix4.h"
#include "NiiQuaternion.h"
#include "NiiPosListener.h"
#include "NiiExtData.h"

namespace NII
{
    class PosNodeDebugObj;
    /** 位置节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosNode : public ExtData, public SpaceAlloc
    {
        friend class Engine;
        friend class PosNodeDebugObj;
    public:
        /** 空间模式
        @version NIIEngine 3.0.0
        */
        enum SpaceMode
        {
            SM_Env,     ///< 相对于环境空间的变换
            SM_Local,   ///< 相对于本地空间的变换
            SM_Space,   ///< 相对于顶级空间的变换
            SM_NII      ///< 类似 SM_Space 但不使用(0, 0, 0)作为标准
        };

        typedef set<PosNode *>::type Childs;
        typedef map<SpaceID, PosNode *>::type ChildList;
    public:
        PosNode(SpaceID id);
        virtual ~PosNode();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        SpaceID getID() const;

        /** 获取父节点
        @version NIIEngine 3.0.0
        */
        virtual PosNode * getParent() const;

        /** 更新位置
        @param[in] own 更新自己
        @param[in] child 更新子对象
        @version NIIEngine 3.0.0
        */
        virtual void update(bool own, bool child);

        /** 通知位置变化
        @param[in] force 强制更新位置
        @version NIIEngine 3.0.0
        */
        virtual void notify(bool force = false);

        /** 创建子节点
        @param[in] pos 相对位置
        @param[in] rot 相对方向
        @version NIIEngine 3.0.0
        */
        virtual PosNode * create(const Vector3f & pos = Vector3f::ZERO,
            const Quaternion & rot = Quaternion::IDENTITY);

        /** 创建子节点
        @param[in] pos 相对位置
        @param[in] rot 相对方向
        @version NIIEngine 3.0.0
        */
        virtual PosNode * create(SpaceID id, const Vector3f & pos = Vector3f::ZERO,
            const Quaternion & rot = Quaternion::IDENTITY);

        /** 添加子对象
        @version NIIEngine 3.0.0
        */
        virtual void add(PosNode * obj);

        /** 获取命名子节点的指针.
        @param[in] id ID
        @version NIIEngine 3.0.0
        */
        virtual PosNode * get(SpaceID id) const;

        /** 获取子节点的指针
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        virtual PosNode * get(Nidx index) const;

        /** 移去子对象
        @note 仅移去,不删除
        @return 被移去的对象
        @version NIIEngine 3.0.0
        */
        virtual PosNode * remove(Nidx index);

        /** 移去子对象
        @note 仅移去,不删除
        @return 被移去的对象
        @version NIIEngine 3.0.0
        */
        virtual PosNode * remove(PosNode * child);

        /** 移去子对象
        @note 仅移去,不删除
        @return 被移去的对象
        @version NIIEngine 3.0.0
        */
        virtual PosNode * remove(SpaceID sid);

        /** 移去所有子对象
        @note 仅移去,不删除
        @version NIIEngine 3.0.0
        */
        virtual void removeAll();

        /** 删除子对象
        @param[in] r 是否遍历其下对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(Nidx index, bool r = true);

        /** 删除子对象
        @param[in] r 是否遍历其下对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpaceID sid, bool r = true);

        /** 删除所有子对象
        @param[in] r 是否遍历其下对象
        @version NIIEngine 3.0.0
        */
        virtual void destroyAll(bool r = true);

        /** 获取子对象数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getCount() const;

        /** 获取子对象列表
        @version NIIEngine 3.0.0
        */
        const ChildList & getChildList() const;

        /** 获取距离
        @note 是 距离*距离
        @version NIIEngine 3.0.0
        */
        virtual NIIf distanceSquared(const Camera * obj) const;

        /** 设置位置
        @param[in] pos 位置
        @param[in] scale 缩放
        @param[in] rot 方向
        @version NIIEngine 3.0.0
        */
        virtual void set(const Vector3f & pos, const Vector3f & scale, const Quaternion & rot);

        /** 设置位置
        @version NIIEngine 3.0.0
        */
        virtual void setPos(const Vector3f & pos);

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getPos() const;

        /** 设置方向
        @verison NIIEngine 3.0.0
        */
        virtual void setOri(const Quaternion & q);

        /** 获取方向
        @version NIIEngine 3.0.0
        */
        virtual const Quaternion & getOri() const;

        /** 设置缩放
        @version NIIEngine 3.0.0
        */
        virtual void setScale(const Vector3f & scale);

        /** 获取缩放
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getScale() const;

        /** 把空间坐标转换成在这个节点里的坐标
        @version NIIEngine 3.0.0
        */
        virtual Vector3f toLocal(const Vector3f & space);

        /** 把空间方向转换成在这个节点里的方向
        @version NIIEngine 3.0.0
        */
        virtual Quaternion toLocal(const Quaternion & space);

        /** 设置是否受环境位置影响
        @version NIIEngine 3.0.0
        */
        virtual void setEnvPos(bool b);

        /** 获取是否受环境位置影响
        @version NIIEngine 3.0.0
        */
        virtual bool isEnvPos() const;

        /** 设置是否受环境方向影响
        @version NIIEngine 3.0.0
        */
        virtual void setEnvOri(bool b);

        /** 获取是否受环境方向影响
        @version NIIEngine 3.0.0
        */
        virtual bool isEnvOri() const;

        /** 设置是否受环境缩放影响
        @version NIIEngine 3.0.0
        */
        virtual void setEnvScale(bool b);

        /** 获取是否受环境缩放影响
        @version NIIEngine 3.0.0
        */
        virtual bool isEnvScale() const;

        /** 设置空间位置
        @version NIIEngine 3.0.0
        */
        virtual void setSpacePos(const Vector3f & pos);

        /** 获取空间位置
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getSpacePos() const;

        /** 设置空间方向
        @version NIIEngine 3.0.0
        */
        virtual void setSpaceOri(const Quaternion & q);

        /** 获取空间方向
        @version NIIEngine 3.0.0
        */
        virtual const Quaternion & getSpaceOri() const;

        /** 获取空间缩放
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getSpaceScale() const;

        /** 获取空间变换矩阵
        @version NIIEngine 3.0.0
        */
        virtual const Matrix4f & getSpaceMatrix() const;

        /** 把在这个节点里的坐标转换成空间坐标
        @version NIIEngine 3.0.0
        */
        virtual Vector3f toSpace(const Vector3f & local);

        /** 把在这个节点里的方向转换成空间方向
        @version NIIEngine 3.0.0
        */
        virtual Quaternion toSpace(const Quaternion & local);

        /** 平移
        @version NIIEngine 3.0.0
        */
        virtual void translate(const Vector3f & v, SpaceMode mode = SM_Env);

        /** 缩放
        @version NIIEngine 3.0.0
        */
        virtual void scale(const Vector3f & v);

        /** 旋转
        @version NIIEngine 3.0.0
        */
        virtual void rotate(const Quaternion & q, SpaceMode mode = SM_Local);

        /** 绕X轴旋转
        @version NIIEngine 3.0.0
        */
        virtual void pitch(const Radian & angle, SpaceMode mode = SM_Local);

        /** 绕Y轴旋转
        @version NIIEngine 3.0.0
        */
        virtual void yaw(const Radian & angle, SpaceMode mode = SM_Local);

        /** 绕Z轴旋转
        @version NIIEngine 3.0.0
        */
        virtual void roll(const Radian & angle, SpaceMode mode = SM_Local);

        /** 绕任意轴旋转
        @version NIIEngine 3.0.0
        */
        void rotate(const Vector3f & axis, const Radian & angle, SpaceMode mode = SM_Local);

        /** 设置监听
        @version NIIEngine 3.0.0
        */
        void set(PosListener * obj);

        /** 获取监听
        @version NIIEngine 3.0.0
        */
        PosListener * getListener() const;

        /** 获取节点几何
        @remark 一般不显示,用于编辑调试
        @version NIIEngine 3.0.0
        */
        virtual GeometryObj * getGeometry(NIIf scale);
    protected:
        PosNode();

        /** 设置父节点
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setParent(PosNode * obj);

        /** 创建实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual PosNode * createImpl(SpaceID id = 0) = 0;

        /** 删除实现
        @version NIIEngine 3.0.0
        */
        virtual void destroyImpl(SpaceID id) = 0;

        /** 更新实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateImpl() const;

        /** 添加到更新队列
        @param[in] obj 子对象
        @param[in] force 强制更新
        @version NIIEngine 3.0.0 高级api
        */
        virtual void addUpdateImpl(PosNode * obj, bool force = false);

        /** 从更新队列中移去
        @param[in] obj 子对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual void removeUpdateImpl(PosNode * obj);
    protected:
        N_static_mutex(mValidIDMutex)
        static SpaceID mValidID;
        static ResourceID mDebugMeshRid;
        static ResourceID mDebugMatRid;
    protected:
        SpaceID mID;
        PosNode * mParent;
        PosListener * mListener;
        PosNodeDebugObj * mDebug;
        ChildList mChildList;
        Vector3f mPos;
        Quaternion mOri;
        Vector3f mScale;
        mutable Vector3f mSpacePos;
        mutable Vector3f mSpaceScale;
        mutable Quaternion mSpaceOri;
        mutable Matrix4f mSpaceMatrix;
        mutable Childs mUpdateList;
        mutable bool mSpaceMatrixValid;
        mutable bool mPosUnvalid;
        mutable bool mChildUnvalid;
        mutable bool mParentValid;
        mutable bool mRootUpdate;
        bool mEvnPos;
        bool mEvnScale;
        bool mEvnOri;
    };
    typedef vector<PosNode *>::type PosNodeList;
}
#endif