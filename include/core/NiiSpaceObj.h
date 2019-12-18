/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-28

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_SPACE_OBJ_H_
#define _NII_SPACE_OBJ_H_

#include "NiiPreInclude.h"
#include "NiiRenderQueue.h"
#include "NiiSpaceObjFactory.h"
#include "NiiAABox.h"
#include "NiiSphere.h"
#include "NiiHash.h"
#include "NiiExtData.h"

namespace NII
{
    typedef HashedVector<Light *> LightList;
    /** 有场景位置概念的对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceObj : public virtual ExtData, public virtual SpaceAlloc
    {
        friend class SpaceObjListener;
    public:
        enum ComType
        {
            CT_NormalMap        = 0x01,
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
        SpaceID getID() const;

        /** 获取类型ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getFactoryID() const;

        /** 设置几何管理器
        @verison NIIEngine 3.0.0 高级api
        */
        void setManager(SpaceManager * mag);

        /** 获取几何管理器
        @version NIIEngine 3.0.0
        */
        SpaceManager * getManager() const;

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
        void setCastShadow(bool b);

        /** 获取是否投射阴影
        @version NIIEngine 3.0.0
        */
        bool isCastShadow() const;

        /** 与上一帧比较是否移动
        @note 非线程安全
        @version NIIEngine 3.0.0
        */
        bool isMove() const;

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
        void setRel(SpaceObj * o);

        /** 获取相关对象
        @version NIIEngine 3.0.0
        */
        SpaceObj * getRel() const;

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
        void setVisibleDistanceLimit(NIIf f);

        /** 获取渲染像素限制
        @version NIIEngine 3.0.0
        */
        NIIf getVisibleDistanceLimit() const;

        /** 设置渲染像素限制
        @version NIIEngine 3.0.0
        */
        void setVisiblePixelLimit(NIIf f);

        /** 获取渲染像素限制
        @version NIIEngine 3.0.0
        */
        NIIf getVisiblePixelLimit() const;

        /** 获取可见结果
        @remark 加上环境条件是否可见
        @version NIIEngine 3.0.0
        */
        virtual bool isVisible() const;

        /** 设置是否调试模式
        @version NIIEngine 3.0.0
        */
        void setDebugMode(bool b);

        /** 获取是否调试模式
        @version NIIEngine 3.0.0
        */
        bool isDebugMode() const;

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
        SpaceObjListener * getListener() const;

        /** 设置可见掩码
        @version NIIEngine 3.0.0
        */
        void setVisibleMark(Nmark m);

        /** 返回可见掩码
        @version NIIEngine 3.0.0
        */
        Nmark getVisibleMark() const;

        /** 设置灯光掩码
        @version NIIEngine 3.0.0
        */
        void setLightMark(Nmark mark);

        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        Nmark getLightMark()const;

        /** 设置场景物相交可见掩码
        @version NIIEngine 3.0.0
        */
        void setIntersectMark(Nmark m);

        /** 返回场景物相交可见掩码
        @version NIIEngine 3.0.0
        */
        Nmark getIntersectMark() const;

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
        void setFactory(SpaceObjFactory * f);

        /** 获取工厂类
        @version NIIEngine 3.0.0
        */
        SpaceObjFactory *  getFactory() const;
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