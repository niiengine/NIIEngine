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
#include "NiiUISheet.h"

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
        const Quaternion & getRotation() const          { return mRot; }

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
        inline void setOvergo(bool b)                   { mOvergo = b;}

        /** 返回鼠标事件是否穿过这个对象而触发其背后对象
        @return 是或否
        @version NIIEngine 3.0.0
        */
        inline bool isOvergo() const                    { return mOvergo;}

        /** 设置自动生成渲染体
        @version NIIEngine 3.0.0
        */
        void setAutoRender(bool b);

        /** 是否自动生成渲染体
        @version NIIEngine 3.0.0 高级api
        */
        bool isAutoRender() const                       { return mAutoCtx; }

        /** 设置独立渲染体
        @version NIIEngine 3.0.0 高级api
        */
        void setRender(UISubSheet * o);

        /** 获取独立渲染体
        @version NIIEngine 3.0.0 高级api
        */
        inline UISubSheet * getRender() const           { return mRender;}

        /** 设置UI画盘
        @version NIIEngine 3.0.0 高级api
        */
        void setSheet(UISheet * obj);

        /** 获取UI画盘
        @version NIIEngine 3.0.0 高级api
        */
        inline UISheet * getSheet() const               { return mSheet;}

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
        UISubSheet * mRender;
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

    /** 边框窗体
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameWindow : public Window
    {
    public:
        /** 调整大小类型
        @version NIIEngine 3.0.0
        */
        enum ResizeType
        {
            RT_Top,
            RT_TopLeft,
            RT_Left,
            RT_TopRight,
            RT_Bottom,
            RT_BottomLeft,
            RT_Right,
            RT_BottomRight,
            RT_None
        };
    public:
        FrameWindow(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~FrameWindow();

        /** 切换最小化
        @version NIIEngine 3.0.0
        */
        void rollup();

        /** 最小化
        @version NIIEngine 3.0.0
        */
        void setRollUp(bool b);

        /** 最小化
        @version NIIEngine 3.0.0
        */
        bool isRollUp() const                   { return mRollUp; }

        /** 设置启用标题栏
        @version NIIEngine 3.0.0
        */
        void setTitleBarEnable(bool b);

        /** 是否启用标题栏
        @version NIIEngine 3.0.0
        */
        bool isTitleBarEnable() const;

        /** 设置标题栏文本
        @version NIIEngine 3.0.0
        */
        void setTitleBarText(const String & str);
        
        /** 获取标题栏文本
        @versioin NIIEngine 3.0.0
        */
        const String & getTitleBarText() const  { return mText; }
        
        /** 设置拖放边框调整大小
        @version NIIEngine 3.0.0
        */
        void setDragResize(bool b)              { mResizeEnable = b; }

        /** 是否拖放边框调整大小
        @version NIIEngine 3.0.0
        */
        bool isDragResize() const;

        /** 设置启动边框
        @version NIIEngine 3.0.0
        */
        void setFrameEnable(bool b);

        /** 是否启动边框
        @version NIIEngine 3.0.0
        */
        bool isFrameEnable() const              { return mFrameEnable; }

        /** 设置启动关闭按纽
        @version NIIEngine 3.0.0
        */
        void setCloseButtonEnable(bool b);

        /** 是否启动关闭按纽
        @version NIIEngine 3.0.0
        */
        bool isCloseButtonEnable() const;

        /** 设置拖拽标题栏可移动
        @version NIIEngine 3.0.0
        */
        void setDragMove(bool b);

        /** 是否拖拽标题栏可移动
        @version NIIEngine 3.0.0
        */
        bool isDragMove() const         { return mDragTitleBarMove; }

        /** 设置启动最小化
        @version NIIEngine 3.0.0
        */
        void setRollUpEnable(bool b);

        /** 是否启动最小化
        @version NIIEngine 3.0.0
        */
        bool isRollUpEnable() const     { return mRollUpEnable; }

        /** 设置调整边框的大小
        @version NIIEngine 3.0.0
        */
        void setBorderSize(NIIf size)   { mBorderSize = size; }

        /** 获取调整边框的大小
        @version NIIEngine 3.0.0
        */
        NIIf getBorderSize() const      { return mBorderSize; }

        /** 设置拖放图标
        @version NIIEngine 3.0.0
        */
        void setCursor(ResizeType type, PixelBuffer * pb);

        /** 获取拖放图标
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getCursor(ResizeType type) const;

        /** 设置标题栏
        @version NIIEngine 3.0.0
        */
        void setTitlebarView(StyleViewID id)    { mTitlebarView = id; }

        /** 获取标题栏
        @version NIIEngine 3.0.0
        */
        StyleViewID getTitlebarView() const     { return mTitlebarView; }

        /** 获取关闭按纽
        @version NIIEngine 3.0.0
        */
        void setCloseButtonView(StyleViewID id) { mCloseButtonView = id; }

        /** 设置关闭按纽
        @version NIIEngine 3.0.0
        */
        StyleViewID getCloseButtonView() const  { return mCloseButtonView; }

        /** 获取标题栏
        @version NIIEngine 3.0.0
        */
        Titlebar * getTitlebar() const          { return mTitlebar; }

        /** 获取关闭按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getCloseButton() const     { return mPushButton; }

        /// @copydetails Widget::intersects
        bool intersects(const Vector2f & pos, bool disable) const;
    public:
        /** 最小化切换事件
        @version NIIEngine 3.0.0
        */
        static const EventID RollUpEvent;

        /** 拖放大小开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID DragResizeBeginEvent;

        /** 拖放大小结束事件
        @version NIIEngine 3.0.0
        */
        static const EventID DragResizeEndEvent;

        /** 关闭按纽事件
        @version NIIEngine 3.0.0
        */
        static const EventID CloseButtonEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 转变游标
        @version NIIEngine 3.0.0 api
        */
        void alterCursor(const Vector2f & cursorpos) const;

        /** 获取调整大小类型
        @version NIIEngine 3.0.0 api
        */
        ResizeType getBorderResizeType(const Vector2f & pt) const;

        /** 从左侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeLeft(NIIf delta, RelRectf & out_area);

        /** 从右侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeRight(NIIf delta, RelRectf & out_area);

        /** 从上侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeTop(NIIf delta, RelRectf & out_area);

        /** 从下侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeBottom(NIIf delta, RelRectf & out_area);
    protected:
        void CloseButtonMFunc(const EventArgs * arg);

        /** 当最小化切换时触发
        @version NIIEngine 3.0.0
        */
        virtual void onRollUp(const WidgetEventArgs * arg);

        /** 关闭按纽按下时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCloseButton(const WidgetEventArgs * arg);

        /** 拖放大小开始时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragResizeBegin(const WidgetEventArgs * arg);

        /** 拖放大小结束时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragResizeEnd(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);

        /// @copydetails Widget::onActivate
        virtual void onActivate(const ActivationEventArgs * e);

        /// @copydetails Widget::onInactivate
        virtual void onInactivate(const ActivationEventArgs * e);
    protected:
        String mText;
        Titlebar * mTitlebar;
        PushButton * mPushButton;
        PixelBuffer * mResizeCursor[4];
        Vector2f mDragPos;
        NIIf mBorderSize;
        StyleViewID mTitlebarView;
        StyleViewID mCloseButtonView;
        bool mRollUp;
        bool mSizing;
        bool mDragTitleBarMove;
        bool mFrameEnable;
        bool mRollUpEnable;
        bool mResizeEnable;
    };

    /**
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI FrameWindowView : public WidgetView
    {
    public:
        FrameWindowView(WidgetViewlID type);

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::getArea
        Rectf getArea() const;
    public:
        static const StateID ActiveTitleFrameState;
        static const StateID InactiveTitleFrameState;
        static const StateID DisableTitleFrameState;
        static const StateID ActiveTitleState;
        static const StateID InactiveTitleState;
        static const StateID DisableTitleState;
        static const StateID ActiveFrameState;
        static const StateID InactiveFrameState;
        static const StateID DisableFrameState;
        static const StateID ActiveState;
        static const StateID InactiveState;
        static const StateID StateCount;
    };
}
}
#endif