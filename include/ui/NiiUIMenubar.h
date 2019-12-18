/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-2-8

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
#ifndef _NII_UI_Menubar_H_
#define _NII_UI_Menubar_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiUIListContainer.h"
#include "NiiUIListContainerWidgetModel.h"

namespace NII
{
namespace UI
{
    /** 菜单栏
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Menubar : public ListContainer
    {
    public:
        Menubar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			ScriptTypeID stid = N_CMD_Menubar, LangID lid = N_PrimaryLang);
        virtual ~Menubar();

        /** 设置弹出项
        @version NIIEngine 3.0.0
        */
        void setPopupItem(MenuItem * item);

        /** 获取弹出项
        @version NIIEngine 3.0.0
        */
        MenuItem * getPopupItem() const;

        /** 设置项距
        @version NIIEngine 3.0.0
        */
        void setItemSpace(NIIf space);

        /** 获取项距
        @version NIIEngine 3.0.0
        */
        NIIf getItemSpace() const;

        /** 设置是否多弹出模式
        @version NIIEngine 3.0.0
        */
        void setMultiPopup(bool b);

        /** 获取是否多弹出模式
        @version NIIEngine 3.0.0
        */
        bool isMultiPopup() const;

        /** 设置自动关闭弹出项
        @version NIIEngine 3.0.0
        */
        void setAutoClosePopup(bool b);

        /** 获取自动关闭弹出项
        @version NIIEngine 3.0.0
        */
        bool isAutoClosePopup() const;
    protected:
        bool init(PropertyCmdSet * dest);

        /// @copydetails ListContainer::layoutContent
        virtual void layoutContent();

        /// @copydetails ListContainer::getContentSize
        virtual PlaneSizef getContentSize() const;
    protected:
        /** 弹出项时触发
        @version NIIEngine 3.0.0
        */
        virtual void onOpenPopup(const WidgetEventArgs * arg);

        /** 关闭项时触发
        @version NIIEngine 3.0.0
        */
        virtual void onClosePopup(const WidgetEventArgs * arg);

        /// @copydetails Widget::onHide
        virtual void onHide(const WidgetEventArgs * arg);
        
        /// @copydetails Container::onRemove
        virtual void onRemove(const WidgetEventArgs * arg);
    public:
        /** 打开项时触发
        @version NIIEngine 3.0.0
        */
        static const EventID OpenPopupEvent;

        /** 关闭项时触发
        @version NIIEngine 3.0.0
        */
        static const EventID ClosePopupEvent;

        /// 事件总数
        static const EventID EventCount;
    protected:
        MenuItem * mPopupItem;
        NIIf mItemSpace;
        bool mMultiPopups;
        bool mAutoClosePopups;
    };

	/**
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI DefaultMenubarWidgetModel : public ListContainerWidgetModel
	{
	public:
		DefaultMenubarWidgetModel(WidgetModelID wsid);

		/// @copydetails WidgetModel::flush
		void flush();

		Rectf getItemRenderArea() const;

		static const StateID ItemRenderAreaState;
		static const StateID StateCount;
	};
}
}
#endif