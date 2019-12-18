/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_VIEWPORT_H_
#define _NII_VIEWPORT_H_

#include "NiiPreInclude.h"
#include "NiiViewportListener.h"
#include "NiiFrustum.h"
#include "NiiColour.h"

namespace NII
{
    /** 帧缓存类型
    @version NIIEngine 3.0.0
    */
    enum FrameBufferType
    {
        FBT_COLOUR  = 0x1,
        FBT_DEPTH   = 0x2,
        FBT_STENCIL = 0x4
    };

    /** 摄像机中的一个视区/视口
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Viewport : public FrameAlloc
    {
    public:
        /** 常用的构造函数
        @param[in] cam 所属设想机
        @param[in] fb 使用的缓存
        @version NIIEngine 3.0.0
        */
        Viewport(ViewportID id, Camera * camera, FrameObj * fb, NIIf left, NIIf top,
            NIIf width, NIIf height, NIIi z);
        virtual ~Viewport();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        ViewportID getID();

        /** 更新内容
        @version NIIEngine 3.0.0
        */
        void update();

        /** 同步状态
        @note 一般是指大小/位置
        @version NIIEngine 3.0.0
        */
        void sync();

        /** 执行清刷缓存
        @param[in] mark FrameBufferType 的一个或多个组合
        @param[in] colour 需要 FBT_COLOUR
        @param[in] depth 需要 FBT_DEPTH
        @param[in] stencil 需要 FBT_STENCIL
        */
        void clear(Nmark mark = FBT_COLOUR | FBT_DEPTH,
            const Colour & colour = Colour::Black, NIIf depth = 1.0f, Nui16 stencil = 0);

        /** 设置是否自动清刷
        @version NIIEngine 3.0.0
        */
        void setAutoClear(bool enable, Nmark bmark = FBT_COLOUR | FBT_DEPTH);

        /** 获取是否自动清刷
        @version NIIEngine 3.0.0
        */
        bool isAutoClear() const;

        /** 是否已经同步
        @version NIIEngine 3.0.0
        */
        bool isSyncRender() const;

        /** 是否同步宽高比
        @version NIIEngine 3.0.0
        */
        bool isSyncAspect() const;

        /** 设置清除的深度
        @version NIIEngine 3.0.0
        */
        void setDepthClear(NIIf d);

        /** 获取清除的深度
        @version NIIEngine 3.0.0
        */
        NIIf getDepthClear() const;

        /** 设置是否自动更新
        @version NIIEngine 3.0.0
        */
        void setAutoUpdate(bool b);

        /** 获取是否自动更新
        @version NIIEngine 3.0.0
        */
        bool isAutoUpdate() const;

        /** 设置背景颜色
        @version NIIEngine 3.0.0
        */
        void setBgColour(const Colour & c);

        /** 获取背景颜色
        @version NIIEngine 3.0.0
        */
        const Colour & getBgColour() const;

        /** 设置使用的材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(SchemeID sid);

        /** 获取使用的材质
        @version NIIEngine 3.0.0
        */
        SchemeID getMaterial() const;

        /** 设置区域位置/大小
        @param[in] left top width height [0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void setArea(NIIf left, NIIf top, NIIf width, NIIf height);

        /** 获取像素位置/大小
        @version NIIEngine 3.0.0
        */
        void getPixelArea(NIIi & left, NIIi & top, NIIi & width, NIIi & height) const;

        /** 添加视口
        @version NIIEngine 3.0.0
        */
        void add(ViewportListener * l);

        /** 移去视口
        @version NIIEngine 3.0.0
        */
        void remove(ViewportListener * l);

        /** 设置所属的摄像机
        @version NIIEngine 3.0.0
        */
        void setCamera(Camera * c);

        /** 获取所属的摄像机
        @version NIIEngine 3.0.0
        */
        Camera * getCamera() const;

        /** 获取缓冲区
        @version NIIEngine 3.0.0
        */
        FrameObj * getBuffer() const;

        /** 获取 x 位置
        @remark 相对于设想机
        @version NIIEngine 3.0.0
        */
        NIIf getLeft() const;

        /** 获取 y 位置
        @remark 相对于设想机
        @version NIIEngine 3.0.0
        */
        NIIf getTop() const;

        /** 获取宽度
        @remark 相对于设想机
        @version NIIEngine 3.0.0
        */
        NIIf getWidth() const;

        /** 获取高度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        NIIf getHeight() const;

        /** 获取像素 x 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        NIIi getPixelLeft() const;

        /** 获取像素 y 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        NIIi getPixelTop() const;

        /** 获取像素宽度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        NIIi getPixelWidth() const;

        /** 获取像素高度
        @remark 相对于视窗
        @version NIIEngine 3.0.
        */
        NIIi getPixelHeight() const;

        /** 获取视图宽
        @version NIIEngine 3.0.0
        */
        NIIi getViewWidth() const;

        /** 获取视图高
        @version NIIEngine 3.0.0
        */
        NIIi getViewHeight() const;

        /** 获取深度次序
        @remark 值越小越越先被渲染
        @version NIIEngine 3.0.0
        */
        NIIi getZOrder() const;

        /** 设置定向
        @version NIIEngine 3.0.0
        */
        void setDirection(NIIf dir);

        /** 获取定向
        @version NIIEngine 3.0.0
        */
        NIIf getDirection() const;

        /** 设置可见掩码
        @version NIIEngine 3.0.0
        */
        void setVisableMark(Nmark m);

        /** 获取可见掩码
        @version NIIEngine 3.0.0
        */
        Nmark getVisableMark() const;

        /** 获取清除缓存类型码
        @return FrameBufferType 组合
        @version NIIEngine 3.0.0
        */
        Nmark getClearMark() const;

        /** 获取最后更新的渲染面数目
        @version NIIEngine 3.0.0
        */
        NCount getTriangleCount() const;

        /** 获取最后更新的渲染批次数目.
        @version NIIEngine 3.0.0
        */
        NCount getBatchCount() const;

        /** 设置是否渲染UI
        @version NIIEngine 3.0.0
        */
        void setUIEnable(bool b);

        /** 获取是否渲染UI
        @version NIIEngine 3.0.0
        */
        bool isUIEnable() const;

        /** 设置是否渲染天空成分
        @version NIIEngine 3.0.0
        */
        void setSkyEnable(bool b);

        /** 获取是否渲染天空成分
        @version NIIEngine 3.0.0
        */
        bool isSkyEnable() const;

        /** 设置是否渲染阴影
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b);

        /** 获取是否渲染阴影
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const;

        /** 设置渲染形式
        @version NIIEngine 3.0.0
        */
        void setCustomRender(Nid id);

        /** 获取渲染形式
        @version NIIEngine 3.0.0
        */
        CustomRender * getCustomRender();

        /** 设置默认定向
        @version NIIEngine 3.0.0
        */
        static void setDefaultDirection(NIIf dir);

        /** 获取默认定向
        @version NIIEngine 3.0.0
        */
        static NIIf getDefaultDirection();
    public:
        /** 同步实现
        @version NIIEngine 3.0.0
        */
        void syncRenderImpl();

        /** 同步实现
        @version NIIEngine 3.0.0
        */
        void syncAspectImpl();
    protected:
        ViewportID mID;
        Camera * mCamera;
        FrameObj * mTarget;
        CustomRender * mCustomRender;
        ViewportListenerList mListeners;
        SchemeID mMaterial;
        Colour mBgColour;
        NIIf mDirection;
        NIIf mLeft;
        NIIf mTop;
        NIIf mWidth;
        NIIf mHeight;
        NIIi mZOrder;
        NIIi mPixelLeft;
        NIIi mPixelTop;
        NIIi mPixelWidth;
        NIIi mPixelHeight;
        mutable NIIi mViewWidth;
        mutable NIIi mViewHeight;
        NIIf mDepthClearValue;
        Nmark mClearMark;
        Nmark mVisableMark;

        bool mSync;
        bool mAspect;
        bool mAutoClear;
        bool mAutoUpdate;
        bool mUIEnable;
        bool mSkyEnable;
        bool mShadowEnable;
        static NIIf mDefaultDirection;
    };
}
#endif