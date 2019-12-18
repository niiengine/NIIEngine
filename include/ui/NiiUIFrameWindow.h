/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-3-8

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

#ifndef _NII_UI_FRAMEWINDOW_H_
#define _NII_UI_FRAMEWINDOW_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWindow.h"
#include "NiiUIWidgetModel.h"

namespace NII
{
namespace UI
{
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
        bool isRollUp() const;

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
        const String & getTitleBarText() const;
        
        /** 设置拖放边框调整大小
        @version NIIEngine 3.0.0
        */
        void setDragFrameResize(bool b);

        /** 是否拖放边框调整大小
        @version NIIEngine 3.0.0
        */
        bool isDragFrameResize() const;

        /** 设置启动边框
        @version NIIEngine 3.0.0
        */
        void setFrameEnable(bool b);

        /** 是否启动边框
        @version NIIEngine 3.0.0
        */
        bool isFrameEnable() const;

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
        void setDragTitleBarMoving(bool b);

        /** 是否拖拽标题栏可移动
        @version NIIEngine 3.0.0
        */
        bool isDragTitleBarMoving() const;

        /** 设置启动最小化
        @version NIIEngine 3.0.0
        */
        void setRollUpEnable(bool b);

        /** 是否启动最小化
        @version NIIEngine 3.0.0
        */
        bool isRollUpEnable() const;

        /** 设置调整边框的大小
        @version NIIEngine 3.0.0
        */
        void setBorderSize(NIIf size);

        /** 获取调整边框的大小
        @version NIIEngine 3.0.0
        */
        NIIf getBorderSize() const;

        /** 设置拖放图标
        @version NIIEngine 3.0.0
        */
        void setCursor(ResizeType type, PixelBuffer * pb);

        /** 获取拖放图标
        @version NIIEngine 3.0.0
        */
		PixelBuffer * getCursor(ResizeType type) const;

        /** 获取标题栏
        @version NIIEngine 3.0.0
        */
        Titlebar * getTitlebar() const;

        /** 获取关闭按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getCloseButton() const;

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
        static const EventID DragSizeBeginEvent;

        /** 拖放大小结束事件
        @version NIIEngine 3.0.0
        */
        static const EventID DragSizeEndEvent;

        /** 关闭按纽事件
        @version NIIEngine 3.0.0
        */
        static const EventID CloseButtonEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails Window::init
        virtual void init();

        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** 转变游标
        @version NIIEngine 3.0.0 api
        */
        void alterCursor(const Vector2f & cursor) const;

        /** 获取调整大小类型
        @version NIIEngine 3.0.0 api
        */
        ResizeType getBorderResizeType(const Vector2f & pt) const;

        /** 从左侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeLeft(NIIf delta, RelRect & out_area);

        /** 从右侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeRight(NIIf delta, RelRect & out_area);

        /** 从上侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeTop(NIIf delta, RelRect & out_area);

        /** 从下侧调整大小
        @version NIIEngine 3.0.0
        */
        bool resizeBottom(NIIf delta, RelRect & out_area);
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
        Vector2f mDragBegin;
        NIIf mBorderSize;
        
        bool mRollUp;
        bool mSizing;
        bool mDragTitleBarMove;
        bool mFrameEnable;
        bool mRollUpEnable;
        bool mResizeEnable;
    };

	/** FrameWindow class for the WidgetModel module.
		This class requires Style to be assigned.  The Style should provide the following:
	Named Areas:
		- ClientWithTitleWithFrame
		- ClientWithTitleNoFrame
		- ClientNoTitleWithFrame
		- ClientNoTitleNoFrame
	*/
	class _EngineAPI DefaultFrameWindowWidgetModel : public WidgetModel
	{
	public:
		DefaultFrameWindowWidgetModel(WidgetModelID type);

		/// @copydetails WidgetModel::flush
		void flush();

		/// @copydetails WidgetModel::getArea
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