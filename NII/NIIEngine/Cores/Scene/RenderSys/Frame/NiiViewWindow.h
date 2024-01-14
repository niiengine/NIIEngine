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

#ifndef _NII_VIEW_WINDOW_H_
#define _NII_VIEW_WINDOW_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"

namespace NII
{
    /** 渲染视窗
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ViewWindow : public RenderSysData
    {
    public:
        ViewWindow(Nui32 wpixel, Nui32 hpixel, bool full, const String & name = N_StrBlank);
        virtual ~ViewWindow();
        
        /** 
        @version NIIEngine 5.0.0
        */
        virtual void _init() = 0;
        
        /** 设置名字
        @version NIIEngine 3.0.0
        */
        virtual void setName(const String & title);
        
        /** 获取名字
        @version NIIEngine 3.0.0
        */
        const String & getName(void) const              { return mName; }

        /** 建立
        @param[in] name 窗体名称
        @param[in] w 窗体宽度(单位:像素)
        @param[in] h 窗体高度(单位:像素)
        @param[in] full 全屏模式
        @param[in] params 额外属性集
        @version NIIEngine 3.0.0
        */
        virtual void setup(const String & name, NCount wpixel, NCount hpixel, bool full, const PropertyData * params) = 0;

        /** 获取区域信息
        @version NIIEngine 3.0.0
        */
        void getArea(NIIi & left, NIIi & top, Nui32 & width, Nui32 & height) const;

        /** 删除视窗
        @version NIIEngine 3.0.0
        */
        virtual void destroy() = 0;

        /** 重新定位视窗
        @version NIIEngine 3.0.0
        */
        virtual void pos(NIIi left, NIIi top) = 0;

        /** 改变视窗大小
        @version NIIEngine 3.0.0
        */
        virtual void resize(NCount width, NCount height);

        /** 是否启用全屏模式
        @param[in] enable 全屏模式
        @param[in] w 宽度(单位:像素)
        @param[in] h 高度(单位:像素)
        @version NIIEngine 3.0.0
        */
        virtual void setFullMode(bool full, Nui32 viewWidth, Nui32 viewHeight, Nui32 frequency, Nui32 denominator, bool borderless, Nui32 monitorIdx);

        /** 返回真,如果窗体在全屏模式下
        @version NIIEngine 3.0.0
        */
        virtual bool isFullMode() const;

        /** 设置可见状态
        @version NIIEngine 3.0.0
        */
        virtual void setVisible(bool b);

        /** 获取可见状态
        @version NIIEngine 3.0.0
        */
        virtual bool isVisible() const;

        /** 设置是否隐藏
        @version NIIEngine 3.0.0
        */
        virtual void setHidden(bool b);

        /** 获取是否隐藏
        @version NIIEngine 3.0.0
        */
        virtual bool isHidden() const;
        
        /** 用于设置渲染目标活动状态
        @version NIIEngine 3.0.0
        */
        virtual void setActive(bool set);
        
        /** 用于检索或设置渲染目标活动状态
        @version NIIEngine 3.0.0
        */
        virtual bool isActive() const;

        /** 启用或禁用垂直同步
        @version NIIEngine 3.0.0
        */
        virtual void setVSync(bool b);

        /** 标识是否垂直同步
        @version NIIEngine 3.0.0
        */
        virtual bool isVSync()const;

        /** 设置交换缓存间隔
        @version NIIEngine 3.0.0
        */
        virtual void setSwapInterval(NCount count);

        /** 获取交换缓存间隔.
        @version NIIEngine 3.0.0
        */
        virtual NCount getSwapInterval() const;

        /** 设置是否对焦自动渲染
        @param[in] b
        @version NIIEngine 3.0.0
        */
        inline void setAutoDeactive(bool set)           { mAutoDeactive = set;  }

        /** 返回是否对焦自动渲染
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDeactive() const              { return mAutoDeactive; }

        /** 设置是否第一窗体
        @version NIIEngine 高级 api
        */
        inline void setFirstWindow(bool set)            { mFirstDev = set; }
        
        /** 第一个渲染窗体
        @remark 第一个渲染窗体可能包含了一个主的设备器,主设备器共享渲染系统资源
        @version NIIEngine 3.0.0
        */
        virtual bool isFirstWindow() const;

        /** 获取颜色成分
        @version NIIEngine 3.0.0
        */
        virtual PixelFormat getFormat() const           { return mBuffer->getFormat(); }
        
        /** 获取颜色类型
        @version NIIEngine 4.0.0
        */
        SampleType getOriginSample() const              { return mBuffer->getOriginSample(); }

        /** 是否关闭了
        @version NIIEngine 3.0.0
        */
        virtual bool isClosed() const = 0;

        /** 设置是否无边框
        @version NIIEngine 4.0.0
        */
        virtual void setBorderless(bool set);
        
        /** 获取是否无边框
        @version NIIEngine 4.0.0
        */
        inline bool isBorderless() const                       { return mBorderless; }
        
        /** 获取渲染对象的宽度
        @version NIIEngine 3.0.0
        */
        void setSize(Nui32 width, Nui32 height);

        /** 获取渲染对象的宽度
        @version NIIEngine 3.0.0
        */
        inline Nui32 getWidth() const                          { return mBuffer->getWidth(); }
        
        /** 获取渲染对象的高度
        @version NIIEngine 3.0.0
        */
        inline Nui32 getHeight() const                         { return mBuffer->getHeight(); }
        
        /** 获取视图宽度
        @version NIIEngine 4.0.0
        */
        inline Nui32 getViewWidth() const                      { return mViewWidth; }
        
        /** 获取视图高度
        @version NIIEngine 4.0.0
        */
        inline Nui32 getViewHeight() const                     { return mViewHeight; }
        
        /** 获取缓存/视图比率
        @version NIIEngine 4.0.0
        */
        virtual float getViewRatio() const;

        /** 设置多重采样设置
        @version NIIEngine 4.0.0
        */
        virtual void setFsaa(const String & fsaa);
        
        /** 获取多重采样设置
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getFSAA() const;
        
        /** 获取多重采样设置
        @version NIIEngine 3.0.0
        */
        inline const String & getFSAAExt() const        { return mFSAAExt; }
        
        /** 是否多重采样
        @version NIIEngine 5.0.0
        */
        inline bool isMultiSample() const               { return mBuffer->isMultiSample(); }

        /** 设置显示频率
        @version NIIEngine 5.0.0
        */
        inline Nui32 getDisplayFrequency() const        { return mDisplayFrequency; }
        
        /** 设置显示频率除数(分数时使用)
        @version NIIEngine 5.0.0
        */
        inline Nui32 getDisplayDenominator() const      { return mDisplayDenominator; }
        
        /** 截屏
        @version NIIEngine 3.0.0
        */
        void screenshots(const String & filename);

        /** 截屏
        @return 文件使用的名字.
        */
        String screenshots(const String & name, const String & ext);

        /**
        @version NIIEngine 5.0.0
        */
        inline Texture * getBuffer() const              { return mBuffer; }
        
        /**
        @version NIIEngine 5.0.0
        */
        inline Texture * getDepth() const               { return mDepth; }
        
        /**
        @version NIIEngine 5.0.0
        */
        inline Texture * getStencil() const             { return mStencil; }
        
        /** 通知形态变化
        @version NIIEngine 3.0.0
        */
        virtual void notify();

        /**
        @version NIIEngine 3.0.0
        */
        virtual void swap() = 0;
    public:
        /** 通知对象已经被创建
        @version NIIEngine 4.0.0 高级api
        */
        virtual void _notifyCreate(RenderSys * sys);

        /** 通知对象已经被初始化
        @version NIIEngine 4.0.0 高级api
        */
        virtual void _notifyInit(RenderSys * sys);
    protected:
        String mName;
        NIIi mLeft;             ///< 窗体左边横坐标
        NIIi mTop;              ///< 窗体顶边纵坐标
        SampleType mSample;
        SampleType mSrcSample;

        Texture * mBuffer;
        Texture * mDepth;
        Texture * mStencil;     ///< 可能于 mDepth 值相同
        Nui32 mDisplayFrequency;
        Nui32 mDisplayDenominator;

        Nui32 mFSAA;
        Nui32 mViewWidth;
        Nui32 mViewHeight;
        Nui32 mColourDepth;
        NCount mVSyncInterval;
        bool mFirstDev;         ///< 是否是首要窗体
        bool mFullMode;         ///< 是否是全屏
        bool mNextFullMode;
        bool mAutoDeactive;     ///< 当掉焦变化时是否停止活动
        bool mBorderless;
        bool mActive;
        bool mVSync;
    };
}
#endif