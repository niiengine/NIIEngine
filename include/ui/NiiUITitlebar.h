/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-1

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
#ifndef _NII_UI_TITLEBAR_H_
#define _NII_UI_TITLEBAR_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiUIWidgetModel.h"

namespace NII
{
namespace UI
{
    /** 标题栏
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Titlebar : public Widget
    {
    public:
        Titlebar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        virtual ~Titlebar();

        /** 设置是否可拖拽
        @version NIIEngine 3.0.0
        */
        void setDragEnable(bool b);

        /** 获取是否可拖拽
        @version NIIEngine 3.0.0
        */
        bool isDragEnable() const;

        /** 是否拖拽中
        @version NIIEngine 3.0.0
        */
        bool isDragging() const;

        /** 获取拖拽开始处
        @version NIIEngine 3.0.0
        */
        const Vector2f & getDragStartPos() const;
    protected:
        /// @copydetails Titlebar::init
        bool init(PropertyCmdSet * dest);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMultiTouch
        virtual void onCursorMultiTouch(const CursorEventArgs * arg);

        /// @copydetails Widget::onFontChange
        virtual void onFontChange(const FontEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    protected:
        Vector2f mDragStartPos;
        Rectf mDragArea;
        bool mDragEnable;		
        bool mDrag;	   
    };

	/**
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI DefaultTitlebarWidgetModel : public WidgetModel
	{
	public:
		DefaultTitlebarWidgetModel(WidgetModelID wsid);

		/// @copydetails WidgetModel::flush
		void flush();
	public:
		static const StateID ActiveState;
		static const StateID InactiveState;
		static const StateID StateCount;
	};
}
}
#endif