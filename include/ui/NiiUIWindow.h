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

#ifndef _NII_UI_WINDOW_H_
#define _NII_UI_WINDOW_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUIInstance.h"

namespace NII
{
namespace UI
{
    /** 窗体级UI单元
    @remark 只有窗体级UI单元才能被Sheet绘制
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Window : public Container
    {
        friend class UISheet;
    public:
        Window(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_Window, LangID lid = N_PrimaryLang);
        virtual ~Window();

        /** 设置旋转
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & q);

        /** 获取旋转
        @version NIIEngine 3.0.0
        */
        const Quaternion & getRotation() const;

        /** 设置是否需要等待处理
        @remark
            等待处理指的是从UI窗体显示开始到UI窗体显示结束的这断时间
        @version NIIEngine 3.0.0
        */
        void setModal(bool b);

        /** 返回是否需要等待处理
        @remark
            等待处理指的是从UI窗体显示开始到UI窗体显示结束的这断时间
        @version NIIEngine 3.0.0
        */
        bool isModal() const;

        /** 设置鼠标事件是否穿过这个对象而触发其背后对象
        @param b 是或否
        */
        inline void setOvergo(bool b) { mOvergo = b;}

        /** 返回鼠标事件是否穿过这个对象而触发其背后对象
        @return 是或否
        @version NIIEngine 3.0.0
        */
        inline bool isOvergo() const { return mOvergo;}

        /** 设置自动生成渲染体
        @version NIIEngine 3.0.0
        */
        void setAutoRender(bool b);

        /** 是否自动生成渲染体
        @version NIIEngine 3.0.0 高级api
        */
        bool isAutoRender() const;

        /** 设置独立渲染体
        @version NIIEngine 3.0.0 高级api
        */
        void setRender(UIInstance * o);

        /** 获取独立渲染体
        @version NIIEngine 3.0.0 高级api
        */
        inline UIInstance * getRender() const { return mRender;}

        /** 设置UI画盘
        @version NIIEngine 3.0.0 高级api
        */
        void setSheet(UISheet * obj);

        /** 获取UI画盘
        @version NIIEngine 3.0.0 高级api
        */
        inline UISheet * getSheet() const { return mSheet;}

        /** 获取一个投影后的位置在机制中的实际位置
        @param[in] pos 投影后(可见结果)的位置
        @return 内部机制实际的位置
        @version NIIEngine 3.0.0
        */
        Vector2f affine(const Vector2f & pos) const;

        /// @copydetails Widget::isPos
        bool isPos(const Vector2f & pos, bool disable) const;

        /// @copydetails Widget::activate
        void activate();
    protected:
        /** 旋转时触发
        @version NIIEngine 3.0.0
        */
        virtual void onRotate(const WidgetEventArgs * arg);

        /// @copydetails Widget::onSize
        virtual void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onRemove
        virtual void onRemove(const WidgetEventArgs * arg);

        /// @copydetails Container::onChildMove
        virtual void onChildMove(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::getClipAreaImpl
        virtual Rectf getClipAreaImpl() const;

        /// @copydetails Widget::getClipInnerAreaImpl
        virtual Rectf getClipInnerAreaImpl() const;

        /// @copydetails Widget::updateImpl
        virtual void updateImpl(TimeDurMS t);

        /// @copydetails Widget::updateImpl
        virtual void updateImpl(SheetContext * rc);

        /// @copydetails Widget::getContextImpl
        virtual void getContextImpl(SheetContext & ctx) const;

        /// @copydetails Widget::refreshFaceImpl
        virtual void refreshFaceImpl();

        /// @copydetails Widget::queuePixelImpl
        virtual void queuePixelImpl();

        /** 分配渲染面
        @version NIIEngine 3.0.0
        */
        void allocRenderCtx();

        /** 删除渲染面
        @version NIIEngine 3.0.0
        */
        void freeRenderCtx();
    protected:
        UIInstance * mRender;
        bool mAutoCtx;
        bool mOvergo;           ///< 光标类事件穿透这个窗体
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DefaultWindow : public Window
    {
    public:
        DefaultWindow(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~DefaultWindow();
    protected:
        /// @copydetails Window::moveFrontImpl
        bool moveFrontImpl(bool click);
    protected:
        /// @copydetails Widget::onCursorMove
        void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorTouch
        void onCursorTouch(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMultiTouch
        void onCursorMultiTouch(const CursorEventArgs * arg);
    };
}
}
#endif