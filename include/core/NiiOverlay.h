/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_OVERLAY_H_
#define _NII_OVERLAY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 表层面
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Overlay : public OverlayAlloc
    {
        friend class OverlayManager;
    public:
        typedef list<OverlayView *>::type ViewList;
    public:
        Overlay(OverlayID oid, Viewport * vp);
        virtual ~Overlay();

        /** 获取
        @version NIIEngine 3.0.0
        */
        OverlayID getID() const;

        /** 获取视口
        @version NIIEngine 3.0.0
        */
        Viewport * getViewport() const;
        
        /** 设置数据来源
        @version NIIEngine 3.0.0
        */
        void setSrc(const String & data);

        /** 获取数据来源
        @version NIIEngine 3.0.0
        */
        const String & getSrc() const;

        /** 设置层
        @version NIIEngine 3.0.0
        */
        void setZOrder(Nidx level);

        /** 获取层
        @version NIIEngine 3.0.0
        */
        Nidx getZOrder() const;

        /** 实现
        @version NIIEngine 3.0.0
        */
        void show();

        /** 隐藏
        @version NIIEngine 3.0.0
        */
        void hide();

        /** 是否显示
        @version NIIEngine 3.0.0
        */
        bool isVisible() const;

        /** 是否已经初始化
        @version NIIEngine 3.0.0
        */
        bool isInit() const;

        /** 添加节点
        @version NIIEngine 3.0.0
        */
        void add(SpaceNode * node);

        /** 移去节点
        @version NIIEngine 3.0.0
        */
        void remove(SpaceNode * node);

        /** 添加图元视图
        @version NIIEngine 3.0.0
        */
        void add(OverlayView * obj);

        /** 移去图元视图
        @version NIIEngine 3.0.0
        */
        void remove(OverlayView * obj);

        /** 获取图元视图
        @version NIIEngine 3.0.0
        */
        OverlayView * get(OverlayGeoID id);

        /** 清除所有图元/节点
        @version NIIEngine 3.0.0
        */
        void clearAll();

        /** 设置平移因子
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void setOffset(NIIf x, NIIf y);

        /** 滚动
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void offset(NIIf xoft, NIIf yoft);

        /** 获取平移因子
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        NIIf getXOffset() const;

        /** 获取平移因子
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        NIIf getYOffset() const;

        /** 设置旋转
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void setRotate(const Radian & angle);

        /** 旋转偏移
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void rotate(const Radian & angleoft);

        /** 获取旋转
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        const Radian & getRotate() const;

        /** 设置缩放因子.
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void setScale(NIIf x, NIIf y);

        /** 获取x缩放值
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        NIIf getXScale() const;

        /** 获取y缩放值
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        NIIf getYScale() const;

        /** 获取空间矩阵
        @version NIIEngine 3.0.0 高级api
        */
        void getSpaceMatrix(Matrix4f * xform) const;

        /** 获取层元
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeo * getGeo(NIIf x, NIIf y);

        /** 层视图列表
        @version NIIEngine 3.0.0
        */
        const ViewList & getViewList() const;
    protected:
        /** 初始化
        @version NIIEngine 3.0.0
        */
        void init();

        /** 渲染队列
        @version NIIEngine 3.0.0
        */
        void renderImpl(Camera * cam, RenderQueue * queue);
    protected:
        OverlayID mID;
        String mOrigin;
        ViewList mViewList;
        Viewport * mViewport;
        SpaceNode * mRootNode;
        Nidx mZOrder;
        Radian mRotate;
        NIIf mScrollX;
        NIIf mScrollY;
        NIIf mScaleX;
        NIIf mScaleY;        
        Matrix4f mSpace;
        bool mSpaceValid;
        bool mVisible;
        bool mInit;
    };
}
#endif