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
#ifndef _NII_UI_PopupMenu_H_
#define _NII_UI_PopupMenu_H_

#include "NiiUIPreInclude.h"
#include "NiiUIMenubar.h"
#include "NiiUIListContainerWidgetModel.h"

namespace NII
{
namespace UI
{
    /** 弹出菜单栏
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PopupMenu : public Menubar
    {
	public:
        PopupMenu(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);

        virtual ~PopupMenu();

        /** 是否处于打开状态
        @version NIIEngine 3.0.0
        */
        bool isOpenState() const;

        /** 设置谈入时段
        @version NIIEngine 3.0.0
        */
        void setFadeInTime(TimeDurMS time);

        /** 获取谈入时段
        @version NIIEngine 3.0.0
        */
        TimeDurMS getFadeInTime() const;

        /** 设置谈出时段
        @version NIIEngine 3.0.0
        */
        void setFadeOutTime(TimeDurMS time);

        /** 获取谈出时段
        @version NIIEngine 3.0.0
        */
        TimeDurMS getFadeOutTime() const;

        /** 激发打开
        @version NIIEngine 3.0.0
        */
        void toggleOpen(bool parent = true);

        /** 激发关闭
        @version NIIEngine 3.0.0
        */
        void toggleClose(bool parent = true);
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);
    protected:
        /// @copydetails Widget::updateImpl
        virtual void updateImpl(TimeDurMS cost);

        /// @copydetails PopupMenu::layoutContent
        virtual void layoutContent();

        /// @copydetails PopupMenu::getContentSize
        virtual PlaneSizef getContentSize() const;

        /// @copydetails Container::onAlphaChange
        virtual void onAlphaChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onShow
        virtual void onShow(const WidgetEventArgs * arg);

        /// @copydetails Menubar::onHide
        virtual void onHide(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);
    protected:
        TimeDurMS mPopupFadeOut;
        TimeDurMS mPopupFadeIn;
        TimeDurMS mFadeCost;
        NIIf mOrgAlpha;
        bool mFading;
        bool mFadeOut;
        bool mOpenState;
    };

	/**
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI DefaultPopupMenuWidgetModel : public ListContainerWidgetModel
	{
	public:
		DefaultPopupMenuWidgetModel(WidgetModelID wmid);

		/// @copydetails ListContainerWidgetModel::flush
		void flush();

		Rectf getItemRenderArea() const;

		static const StateID ItemRenderAreaState;
		static const StateID StateCount;
	};
}
}
#endif