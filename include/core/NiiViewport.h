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

#ifndef _NII_VIEWPORT_H_
#define _NII_VIEWPORT_H_

#include "NiiPreInclude.h"
#include "NiiFrustum.h"
#include "NiiColour.h"

#define Viewport_CalcDim        0x01
#define Viewport_CalcClipDim    0x02
#define Viewport_SyncRender     0x04
#define Viewport_SyncAspect     0x08
#define Viewport_AutoClear      0x10
#define Viewport_AutoUpdate     0x20
#define Viewport_OverlayEnable  0x40
#define Viewport_SkyEnable      0x80
#define Viewport_ShadowEnable   0x100
#define Viewport_FullBuffer     0x200
#define Viewport_NonClip        0x400

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

    /** 视口
    @remark 视口里可以秒速多个摄像机
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Viewport : public FrameAlloc
    {
        friend class RenderSys;
    public:
        Viewport(ViewportID id, Texture * buf, const Vector4f & area, const Vector4f & clip, NIIi z, NCount mipLevel);
        virtual ~Viewport();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline ViewportID getID()           { return mID; }

        /** 更新内容
        @version NIIEngine 3.0.0
        */
        void update(Camera * target, const RenderQueueFusion * rqfusion);

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
        void clear(Nmark32 mark = FBT_COLOUR | FBT_DEPTH, const Colour & colour = Colour::Black, NIIf depth = 1.0f, Nui16 stencil = 0);

        /** 设置是否自动清刷
        @version NIIEngine 3.0.0
        */
        void setAutoClear(bool enable, Nmark32 bmark = FBT_COLOUR | FBT_DEPTH);

        /** 获取是否自动清刷
        @version NIIEngine 3.0.0
        */
        inline bool isAutoClear() const                 { return N_MarkTrue(mMark, Viewport_AutoClear); }

        /** 是否已经同步
        @version NIIEngine 3.0.0
        */
        inline bool isSyncRender() const                { return N_MarkTrue(mMark, Viewport_SyncRender); }

        /** 是否同步宽高比
        @version NIIEngine 3.0.0
        */
        inline bool isSyncAspect() const                { return N_MarkTrue(mMark, Viewport_SyncAspect); }

        /** 设置是否自动更新
        @version NIIEngine 3.0.0
        */
        inline void setAutoUpdate(bool b)               { N_MarkBool(mMark, Viewport_AutoUpdate, b); }

        /** 获取是否自动更新
        @version NIIEngine 3.0.0
        */
        inline bool isAutoUpdate() const                { return N_MarkTrue(mMark, Viewport_AutoUpdate); }

        /** 设置背景颜色
        @version NIIEngine 3.0.0
        */
        inline void setBgColour(const Colour & c)       { mBgColour = c; }

        /** 获取背景颜色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getBgColour() const       { return mBgColour; }
        
        /** 设置清除的深度
        @version NIIEngine 3.0.0
        */
        inline void setDepthClear(NIIf d)               { mDepthClearValue = d; }

        /** 获取清除的深度
        @version NIIEngine 3.0.0
        */
        inline NIIf getDepthClear() const               { return mDepthClearValue; }

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
        @param[in] left [0 - 1.0]
        @param[in] top [0 - 1.0]
        @param[in] width [0 - 1.0]
        @param[in] height [0 - 1.0]
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

        /** 获取缓冲区
        @version NIIEngine 3.0.0
        */
        inline Texture * getBuffer() const              { return mTarget; }

        /** 完整的缓存
        @version NIIEngine 3.0.0
        */
        inline bool isFullBufer() const                 { return N_MarkTrue(mMark, Viewport_FullBuffer); }

        /** 获取是否非裁剪模式
        @version NIIEngine 3.0.0
        */
        inline bool isNonClip() const                   { return N_MarkTrue(mMark, Viewport_NonClip); }

        /** 获取像素 x 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getLeft() const                     { return mLeft; }

        /** 获取像素 y 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getTop() const                      { return mTop; }

        /** 获取像素宽度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getWidth() const                    { return mWidth; }

        /** 获取像素高度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIf getHeight() const                   { return mHeight; }
        
        /** 获取裁剪像素 x 位置
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipLeft() const                 { return mClipLeft; }
        
        /** 获取裁剪像素 y 位置
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipTop() const                  { return mClipTop; }
        
        /** 获取裁剪像素宽度
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipWidth() const                { return mClipWidth; }
        
        /** 获取裁剪像素高度
        @version NIIEngine 3.0.0
        */
        inline NIIf getClipHeight() const               { return mCliplHeight; }
        
        /** 获取像素 x 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIi getPixelLeft() const                { return mLeft * mTargetWidth; }

        /** 获取像素 y 位置
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIi getPixelTop() const                 { return mTop * mTargetHeight; }

        /** 获取像素宽度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIi getPixelWidth() const               { return mWidth * mTargetWidth; }

        /** 获取像素高度
        @remark 相对于视窗
        @version NIIEngine 3.0.0
        */
        inline NIIi getPixelHeight() const              { return mHeight * mTargetHeight; }
        
        /** 获取裁剪像素 x 位置
        @version NIIEngine 3.0.0
        */
        inline NIIi getClipPixelLeft() const            { return mClipLeft * mTargetWidth; }
        
        /** 获取裁剪像素 y 位置
        @version NIIEngine 3.0.0
        */
        inline NIIi getClipPixelTop() const             { return mClipTop * mTargetHeight; }
        
        /** 获取裁剪像素宽度
        @version NIIEngine 3.0.0
        */
        inline NIIi getClipPixelWidth() const           { return mClipWidth * mTargetWidth; }
        
        /** 获取裁剪像素高度
        @version NIIEngine 3.0.0
        */
        inline NIIi getClipPixelHeight() const          { return mCliplHeight * mTargetHeight; }

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
        inline void setVisableMark(Nmark32 m)           { mVisableMark = m; }

        /** 获取可见掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark32 getVisableMark() const           { return mVisableMark; }
        
        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        inline void setLigthMark(Nmark32 m)             { mLightMark = m; }
        
        /** 获取灯光掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark32 getLigthMark() const             { return mLightMark; }

        /** 获取清除缓存类型码
        @return FrameBufferType 组合
        @version NIIEngine 3.0.0
        */
        inline Nmark32 getClearMark() const             { return mClearMark; }

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
        inline void setOverlayEnable(bool b)            { N_MarkBool(mMark, Viewport_OverlayEnable, b); }

        /** 获取是否渲染UI
        @version NIIEngine 3.0.0
        */
        inline bool isOverlayEnable() const             { return N_MarkTrue(mMark, Viewport_OverlayEnable); }

        /** 设置是否渲染天空成分
        @version NIIEngine 3.0.0
        */
        inline void setSkyEnable(bool b)                { N_MarkBool(mMark, Viewport_SkyEnable, b); }

        /** 获取是否渲染天空成分
        @version NIIEngine 3.0.0
        */
        inline bool isSkyEnable() const                 { return N_MarkTrue(mMark, Viewport_SkyEnable); }

        /** 设置是否渲染阴影
        @version NIIEngine 3.0.0
        */
        inline void setShadowEnable(bool b)             { N_MarkBool(mMark, Viewport_ShadowEnable, b);  }

        /** 获取是否渲染阴影
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const              { return N_MarkTrue(mMark, Viewport_ShadowEnable); }

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
        inline void getBufferType(ViewportType type)    { mBufferType = type; }

        /** 获取缓存类型
        @version NIIEngine 5.0.0
        */
        inline ViewportType getBufferType() const       { return mBufferType; }

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
        Texture * mTarget;
        RenderQueueFusion * mQueueFusion;
        ViewportListenerList mListeners;
        ViewportType mBufferType;
        SchemeID mMaterial;
        Colour mBgColour;
        NIIf mDirection;
        NIIi mZOrder;
        NIIi mMipMap;

        NIIf mLeft;
        NIIf mTop;
        NIIf mWidth;
        NIIf mHeight;
        NIIf mClipLeft;
        NIIf mClipTop;
        NIIf mClipWidth; 
        NIIf mCliplHeight;

        mutable NIIi mViewWidth;
        mutable NIIi mViewHeight;
        NIIi mTargetWidth;
        NIIi mTargetHeight;
        NIIf mDepthClearValue;
        Nmark32 mClearMark;
        Nmark32 mVisableMark;
        Nmark32 mLightMark;
        Nmark32 mMark;

        static NIIf mDefaultDirection;
    };
    typedef vector<Viewport *>::type ViewportList;
}
#endif