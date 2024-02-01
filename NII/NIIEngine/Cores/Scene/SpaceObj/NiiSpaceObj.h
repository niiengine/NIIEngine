﻿/*
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

#ifndef _NII_SPACE_OBJ_H_
#define _NII_SPACE_OBJ_H_

#include "NiiPreInclude.h"
#include "NiiRenderQueue.h"
#include "NiiFactoryObj.h"
#include "NiiAABox.h"
#include "NiiSphere.h"
#include "NiiExtData.h"

namespace NII
{
    /** 空间对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceObjFactory : public FactoryObj
    {
    public:
        SpaceObjFactory() : mTypeMark(0xFFFFFFFF) {}
        virtual ~SpaceObjFactory() {}

        /** 创建实例
        @param[in] sid
        @param[in] mag
        @param[in] params
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0) = 0;

        /** 设置工厂类的类型掩码
        @version NIIEngine 3.0.0
        */
        inline void setTypeMark(Nmark mark)         { mTypeMark = mark; }

        /** 获取工厂类的类型掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getTypeMark() const            { return mTypeMark; }
    protected:
        Nmark mTypeMark;
    };

    /** 事件监听
    @remark 可用于物理引擎/阴影渲染器控制
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI SpaceObjListener
    {
    public:
        SpaceObjListener();
        virtual ~SpaceObjListener();

        /** 创建后时
        @version NIIEngine 3.0.0
        */
        virtual void create(SpaceObj * obj);

        /** 删除时
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpaceObj * obj);

        /** 位置发生改变
        @version NIIEngine 3.0.0
        */
        virtual void notify(SpaceObj * obj);

        /** 附加到节点时
        @note 仅在参数2非0时触发
        @version NIIEngine 3.0.0
        */
        virtual void attach(SpaceObj * obj, const PosNode * np);

        /** 从节点中分离时
        @note 仅在参数2非0时触发
        @version NIIEngine 3.0.0
        */
        virtual void detach(SpaceObj * obj, const PosNode * op);

        /** 使用指定摄像机绘制空间
        @return 是否有实际绘制效果,没有效果则过滤
        @version NIIEngine 3.0.0
        */
        virtual bool render(SpaceObj * obj, const Camera * view);

        /** 查询实际影响空间对象的灯光
        @version NIIEngine 3.0.0
        */
        virtual void query(const SpaceObj * obj, LightList & out);
    };

    /** 有场景位置概念的对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceObj : public virtual ExtData, public virtual SpaceAlloc
    {
        friend class SpaceObjListener;
    public:
        enum ComType
        {
            CT_NormalsMap       = 0x01,
            CT_Halo             = 0x02,
            CT_TextureShadow    = 0x04,
            CT_StencilShadow    = 0x08
        };
        /** 拣选模式
        @version NIIEngine 3.0.0
        */
        enum CullType
        {
            CT_Auto,    ///< 自动渲染
            CT_Always,  ///< 总是渲染
            CT_Event    ///< 让事件驱动渲染
        };
    public:
        SpaceObj();
        SpaceObj(SpaceID sid, SpaceManager * mag = 0);
        virtual ~SpaceObj();

        /** 获取空间ID
        @version NIIEngine 3.0.0
        */
        inline SpaceID getID() const                    { return mID; }

        /** 获取类型ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getFactoryID() const;

        /** 设置几何管理器
        @verison NIIEngine 3.0.0 高级api
        */
        inline void setManager(SpaceManager * mag)      { mManager = mag; }

        /** 获取几何管理器
        @version NIIEngine 3.0.0
        */
        inline SpaceManager * getManager() const        { return mManager;  }

        /** 设置拣选模式
        @param[in] t 拣选模式
        @version NIIEngine 3.0.0
        */
        virtual void setCullType(CullType t);

        /** 设置拣选模式
        @version NIIEngine 3.0.0
        */
        virtual CullType getCullType() const;

        /** 是否是活动的空间对象
        @version NIIEngine 3.0.0
        */
        virtual bool isActive() const;

        /** 获取是否接收阴影
        @version NIIEngine 3.0.0
        */
        virtual bool isReceiveShadow();

        /** 设置是否投射阴影
        @remark
        @version NIIEngine 3.0.0
        */
        inline void setCastShadow(bool b)               { mCastShadows = b; }

        /** 获取是否投射阴影
        @version NIIEngine 3.0.0
        */
        inline bool isCastShadow() const                { return mCastShadows; }

        /** 与上一帧比较是否移动
        @note 非线程安全
        @version NIIEngine 3.0.0
        */
        inline bool isMove() const                      { return mMove; }

        /** 空间位置改变时调用
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _notify();

        /** 渲染摄像机改变时调用
        @version NIIEngine 3.0.0
        */
        virtual void _notify(Camera * cam);

        /** 附加到位置节点上
        @param[in] node 节点对象
        @param[in] tag 是否标签节点
        @version NIIEngine 3.0.0 高级api
        */
        virtual void attach(PosNode * node, bool tag = false);

        /** 是否附加到位置节点
        @version NIIEngine 3.0.0
        */
        virtual bool isAttach() const;

        /** 从位置节点中分离
        @version NIIEngine 3.0.0 高级api
        */
        virtual void detach();

        /** 获取空间节点
        @version NIIEngine 3.0.0
        */
        virtual PosNode * getSpaceNode() const;

        /** 获取场景节点
        @version NIIEngine 3.0.0
        */
        virtual SpaceNode * getSceneNode() const;

        /** 设置相关对象
        @version NIIEngine 3.0.0
        */
        inline void setRel(SpaceObj * o)                { mRelation = o; }

        /** 获取相关对象
        @version NIIEngine 3.0.0
        */
        inline SpaceObj * getRel() const                { return mRelation; }

        /** 获取对称盒(局部)
        @version NIIEngine 3.0.0
        */
        virtual const AABox & getAABB() const = 0;

        /** 获取中心半径
        @version NIIEngine 3.0.0
        */
        virtual NIIf getCenterRange() const = 0;

        /** 获取空间位置
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getSpacePos() const;

        /** 获取相对的空间位置
        @remark 和 setRel/getRel 相关
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getRelSpacePos() const;

        /** 获取空间朝向
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getSpaceDirection() const;

        /** 获取空间矩阵
        @version NIIEngine 3.0.0
        */
        virtual const Matrix4f & getSpaceMatrix() const;

        /** 或空间对称盒
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        virtual const AABox & getSpaceAABB(bool update = false) const;

        /** 获取空间边界球
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        virtual const Sphere & getSpaceBS(bool update = false) const;

        /** 设置是否可见
        @version NIIEngine 3.0.0
        */
        virtual void setVisible(bool b);

        /** 获取是否可见
        @version NIIEngine 3.0.0
        */
        virtual bool getVisible() const;

        /** 设置渲染像素限制
        @version NIIEngine 3.0.0
        */
        inline void setVisibleDistanceLimit(NIIf f)     { mDisLimit = f; }

        /** 获取渲染像素限制
        @version NIIEngine 3.0.0
        */
        inline NIIf getVisibleDistanceLimit() const     { return mDisLimit; }

        /** 设置渲染像素限制
        @version NIIEngine 3.0.0
        */
        inline void setVisiblePixelLimit(NIIf f)        { mPixelLimit = f; }

        /** 获取渲染像素限制
        @version NIIEngine 3.0.0
        */
        inline NIIf getVisiblePixelLimit() const        { return mPixelLimit; }

        /** 获取可见结果
        @remark 加上环境条件是否可见
        @version NIIEngine 3.0.0
        */
        virtual bool isVisible() const;

        /** 设置是否调试模式
        @version NIIEngine 3.0.0
        */
        inline void setDebugMode(bool b)                { mDebugMode = b; }

        /** 获取是否调试模式
        @version NIIEngine 3.0.0
        */
        inline bool isDebugMode() const                 { return mDebugMode; }

        /** 设置渲染队列
        @param[in] rqid
        @version NIIEngine 3.0.0
        */
        virtual void setRenderGroup(Nui16 rqid, Nui16 level = 0);

        /** 获取渲染队列
        @version NIIEngine 3.0.0
        */
        virtual Nui16 getRenderGroup() const;

        /** 设置这个对象的监听
        @version NIIEngine 3.0.0
        */
        void setListener(SpaceObjListener * obj);

        /** 获取这个对象当前的监听
        @version NIIEngine 3.0.0
        */
        inline SpaceObjListener * getListener() const   { return mListener; }

        /** 设置可见掩码
        @version NIIEngine 3.0.0
        */
        inline void setVisibleMark(Nmark m)             { mVisibleMark = m; }

        /** 返回可见掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getVisibleMark() const             { return mVisibleMark; }

        /** 设置灯光掩码
        @version NIIEngine 3.0.0
        */
        inline void setLightMark(Nmark mark)            { mLightMask = mark; mLightDirtyMark = 0;}

        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getLightMark()const                { return mLightMask; }

        /** 设置场景物相交可见掩码
        @version NIIEngine 3.0.0
        */
        inline void setIntersectMark(Nmark m)           { mIntersectMark = m; }

        /** 返回场景物相交可见掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getIntersectMark() const           { return mIntersectMark; }

        /** 类型掩码
        @version NIIEngine 3.0.0
        */
        virtual Nmark getTypeMark() const;

        /** 成分掩码
        @version NIIEngine 3.0.0
        */
        virtual Nmark getComMark() const;

        /** 加入到渲染队列中
        @version NIIEngine 3.0.0
        */
        void queue(RenderQueue * rq, Camera * cam, bool onlycast);

        /** 几何列队
        @version NIIEngine 3.0.0 高级api
        */
        virtual void queue(RenderQueue * queue) = 0;

        /** 获取实际影响这个空间对象的灯光
        @version NIIEngine 3.0.0 高级api
        */
        virtual void query(LightList & ll) const;

        /** 几何查询
        @version NIIEngine 3.0.0 高级api
        */
        virtual void query(GeometryQuery * query) = 0;

        /** 设置工厂类
        @version NIIEngine 3.0.0
        */
        inline void setFactory(SpaceObjFactory * f)     { mFactory = f; }

        /** 获取工厂类
        @version NIIEngine 3.0.0
        */
        inline SpaceObjFactory * getFactory() const     { return mFactory; }
    protected:
        N_static_mutex(mValidIDMutex)
        static SpaceID mValidID;
    protected:
        SpaceID mID;
        SpaceObjFactory * mFactory;
        SpaceObjListener * mListener;
        SpaceManager * mManager;
        PosNode * mSpaceNode;
        SpaceObj * mRelation;
        LightList mLightList;
        mutable Sphere mSpaceBS;
        mutable AABox mSpaceAABB;
        CullType mCullType;
        NIIf mDisLimit;
        NIIf mPixelLimit;
        Nui16 mRenderGroup;
        Nui16 mRenderLevel;
        Nmark mIntersectMark;
        Nmark mVisibleMark;
        Nmark mLightMask;
        Nmark mLightDirtyMark;
        Nmark mComMark;

        bool mCastShadows;
        bool mListenerCtl;
        bool mTagNode;
        bool mDebugMode;
        bool mVisible;
        bool mInvisible;
        bool mMove;
    };
}
#endif