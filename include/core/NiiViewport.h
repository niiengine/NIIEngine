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

#ifndef _NII_VIEWPORT_H_
#define _NII_VIEWPORT_H_

#include "NiiPreInclude.h"
#include "NiiFrustum.h"
#include "NiiColour.h"

namespace NII
{
    /** 视口监听器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ViewportListener
    {
    public:
        virtual ~ViewportListener();

        /** 创建视口时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCreate(Viewport * obj);
        
        /** 删除视口时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(Viewport * obj);

        /** 所属摄像机改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCameraChange(Viewport * obj, Camera * dst);

        /** 视口区域改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onAreaChange(Viewport * obj);
    };
    typedef vector<ViewportListener *>::type ViewportListenerList;
    
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
        Viewport(ViewportID id, Camera * cam, FrameObj * fb, const Vector4f & area, const Vector4f & clip, NIIi z, NCount mipLevel);
        virtual ~Viewport();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline ViewportID getID()           { return mID; }

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
        void clear(Nmark mark = FBT_COLOUR | FBT_DEPTH, const Colour & colour = Colour::Black, NIIf depth = 1.0f, Nui16 stencil = 0);

        /** 设置是否自动清刷
        @version NIIEngine 3.0.0
        */
        void setAutoClear(bool enable, Nmark bmark = FBT_COLOUR | FBT_DEPTH);

        /** 获取是否自动清刷
        @version NIIEngine 3.0.0
        */
        inline bool isAutoClear() const                 { return mAutoClear; }

        /** 是否已经同步
        @version NIIEngine 3.0.0
        */
        inline bool isSyncRender() const                { return mSync; }

        /** 是否同步宽高比
        @version NIIEngine 3.0.0
        */
        inline bool isSyncAspect() const                { return mAspect; }

        /** 设置清除的深度
        @version NIIEngine 3.0.0
        */
        inline void setDepthClear(NIIf d)               { mDepthClearValue = d; }

        /** 获取清除的深度
        @version NIIEngine 3.0.0
        */
        inline NIIf getDepthClear() const               { return mDepthClearValue; }

        /** 设置是否自动更新
        @version NIIEngine 3.0.0
        */
        inline void setAutoUpdate(bool b)               { mAutoUpdate = b; }

        /** 获取是否自动更新
        @version NIIEngine 3.0.0
        */
        inline bool isAutoUpdate() const                { return mAutoUpdate; }

        /** 设置背景颜色
        @version NIIEngine 3.0.0
        */
        inline void setBgColour(const Colour & c)       { mBgColour = c; }

        /** 获取背景颜色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getBgColour() const       { return mBgColour; }

        /** 设置使用的材质
        @version NIIEngine 3.0.0
        */
        inline void setMaterial(SchemeID sid)           { mMaterial = sid; }

        /** 获取使用的材质
        @version NIIEngine 3.0.0
        */
        inline SchemeID getMaterial() const             { return mMaterial; }

        /** 设置区域位置/裁剪大小
        @param[in] area [0 - 1.0]
        @param[in] clip [0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void setArea(const Vector4f & area, const Vector4f & clip, NCount mipLevel);
        
        /** 设置区域位置
        @param[in] area [0 - 1.0]
        @param[in] clip [0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void setArea(NIIf left, NIIf top, NIIf width, NIIf height);
        
        /** 设置裁剪面 
        @param[in] left top width height [0 - 1.0]
        @version NIIEngine 5.0.0
        */
        void setClip(NIIf left, NIIf top, NIIf width, NIIf height);

        /** 获取像素位置/大小
        @version NIIEngine 3.0.0
        */
        void getPixelArea(NIIi & left, NIIi & top, NIIi & width, NIIi & height) const;
        
        /** 获取像素位置/大小
        @version NIIEngine 3.0.0
        */
        void getPixelClip(NIIi & left, NIIi & top, NIIi & width, NIIi & height) const;

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
        inline Camera * getCamera() const               { return mCamera; }

        /** 获取缓冲区
        @version NIIEngine 3.0.0
        */
        inline FrameObj * getBuffer() const             { return mTarget; }
        
        /** 完整的缓存
        @version NIIEngine 3.0.0
        */
        bool isFullBufer() const                        { return mFullBufer; }

        /** 获取像素 x 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getPixelLeft() const                { return mPixelLeft; }

        /** 获取像素 y 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getPixelTop() const                 { return mPixelTop; }

        /** 获取像素宽度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getPixelWidth() const               { return mPixelWidth; }

        /** 获取像素高度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getPixelHeight() const              { return mPixelHeight; }
        
        /** 获取裁剪像素 x 位置
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipPixelLeft() const            { return mPixelClipLeft; }
        
        /** 获取裁剪像素 y 位置
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipPixelTop() const             { return mPixelClipTop; }
        
        /** 获取裁剪像素宽度
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipPixelWidth() const           { return mPixelClipWidth; }
        
        /** 获取裁剪像素高度
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipPixelHeight() const          { return mPixelCliplHeight; }
        
        /** 获取是否非裁剪模式
        @version NIIEngine 3.0.0
        */
        bool isNonClip() const                          { return mNonClip; }

        /** 获取视图宽
        @version NIIEngine 3.0.0
        */
        inline NIIi getViewWidth() const                { return mViewWidth; }

        /** 获取视图高
        @version NIIEngine 3.0.0
        */
        inline NIIi getViewHeight() const               { return mViewHeight; }

        /** 获取深度次序
        @remark 值越小越越先被渲染
        @version NIIEngine 3.0.0
        */
        inline NIIi getZOrder() const                   { return mZOrder; }

        /** 设置定向
        @version NIIEngine 3.0.0
        */
        void setDirection(NIIf dir);

        /** 获取定向
        @version NIIEngine 3.0.0
        */
        inline NIIf getDirection() const                { return mDirection;}

        /** 设置可见掩码
        @version NIIEngine 3.0.0
        */
        inline void setVisableMark(Nmark m)             { mVisableMark = m; }

        /** 获取可见掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getVisableMark() const             { return mVisableMark; }
        
        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        inline void setLigthMark(Nmark m)               { mLightMark = m; }
        
        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getLigthMark() const               { return mLightMark; }

        /** 获取清除缓存类型码
        @return FrameBufferType 组合
        @version NIIEngine 3.0.0
        */
        inline Nmark getClearMark() const               { return mClearMark; }

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
        inline void setUIEnable(bool b)                 { mUIEnable = b; }

        /** 获取是否渲染UI
        @version NIIEngine 3.0.0
        */
        inline bool isUIEnable() const                  { return mUIEnable; }

        /** 设置是否渲染天空成分
        @version NIIEngine 3.0.0
        */
        inline void setSkyEnable(bool b)                { mSkyEnable = b; }

        /** 获取是否渲染天空成分
        @version NIIEngine 3.0.0
        */
        inline bool isSkyEnable() const                 { return mSkyEnable; }

        /** 设置是否渲染阴影
        @version NIIEngine 3.0.0
        */
        inline void setShadowEnable(bool b)             { mShadowEnable = b; }

        /** 获取是否渲染阴影
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const              { return mShadowEnable; }

        /** 设置渲染形式
        @version NIIEngine 3.0.0
        */
        void setCustomRender(Nid id);

        /** 获取渲染形式
        @version NIIEngine 3.0.0
        */
        inline RenderQueueFusion * getQueueFusion()     { return mQueueFusion; }
        
        /** 设置缓存类型
        @version NIIEngine 5.0.0
        */
        void getBufferType(ViewportType type)           { mBufferType = type; }

        /** 获取缓存类型
        @version NIIEngine 5.0.0
        */
        ViewportType getBufferType() const              { return mBufferType; }

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
        RenderQueueFusion * mQueueFusion;
        ViewportListenerList mListeners;
        ViewportType mBufferType;
        SchemeID mMaterial;
        Colour mBgColour;
        NIIf mDirection;
        NIIi mZOrder;
        NIIf mPixelLeft;
        NIIf mPixelTop;
        NIIf mPixelWidth;
        NIIf mPixelHeight;
        NIIf mPixelClipLeft, 
        NIIf mPixelClipTop, 
        NIIf mPixelClipWidth, 
        NIIf mPixelCliplHeight;
        NCount mMipMap;
        
        mutable NIIi mViewWidth;
        mutable NIIi mViewHeight;
        NIIf mDepthClearValue;
        Nmark mClearMark;
        Nmark mVisableMark;
        Nmark mLightMark;

        bool mSync;
        bool mAspect;
        bool mAutoClear;
        bool mAutoUpdate;
        bool mUIEnable;
        bool mSkyEnable;
        bool mShadowEnable;
        static NIIf mDefaultDirection;
    };
    typedef vector<Viewport *>::type ViewportList;
}
#endif