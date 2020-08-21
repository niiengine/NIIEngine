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
#include "NiiFrameObj.h"

namespace NII
{
    /** 渲染视窗
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ViewWindow : public FrameObj
    {
    public:
        ViewWindow(FrameObjID id, const String & name = N_StrBlank);

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
        void getArea(NIIi & left, NIIi & top, Nui32 & width, Nui32 & height, Nui32 & depth) const;

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
        virtual void resize(NCount width, NCount height) = 0;

        /** 是否启用全屏模式
        @param[in] enable 全屏模式
        @param[in] w 宽度(单位:像素)
        @param[in] h 高度(单位:像素)
        @version NIIEngine 3.0.0
        */
        virtual void setFullMode(bool enable, NCount wpixel, NCount hpixel);

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
        inline void setAutoFocusActive(bool set)        { mAutoFocusActive = set;  }

        /** 返回是否对焦自动渲染
        @version NIIEngine 3.0.0
        */
        inline bool isAutoFocusActive() const           { return mAutoFocusActive; }

        /** 设置是否第一窗体
        @version NIIEngine 高级 api
        */
        inline void setFirstWindow(bool b)              { mFirstDev = b; }

        /** 获取颜色成分
        @version NIIEngine 3.0.0
        */
        virtual PixelFormat getFormat() const;

        /** 截屏
        @version NIIEngine 3.0.0
        */
        void screenshots(const String & filename);

        /** 截屏
        @return 文件使用的名字.
        */
        String screenshots(const String & name, const String & ext);

        /** 通知形态变化
        @version NIIEngine 3.0.0
        */
        virtual void notify();

        /// @copydetails FrameObj::isActive
        virtual bool isActive() const;

        /// @copydetails FrameObj::isClosed
        virtual bool isClosed() const = 0;

        /// @copydetails FrameObj::isFirstWindow
        virtual bool isFirstWindow() const;
    public:
        /** 通知对象已经被创建
        @version NIIEngine 4.0.0 高级api
        */
        void _notifyCreate(RenderSys * sys);

        /** 通知对象已经被初始化
        @version NIIEngine 4.0.0 高级api
        */
        void _notifyInit(RenderSys * sys);
    protected:
        NIIi mLeft;             ///< 窗体左边横坐标
        NIIi mTop;              ///< 窗体顶边纵坐标
        bool mFirstDev;         ///< 是否是首要窗体
        bool mFullMode;         ///< 是否是全屏
        bool mAutoFocusActive;  ///< 当掉焦变化时是否停止活动
    };
}
#endif