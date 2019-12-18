/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_UI_BUTTON_H_
#define _NII_UI_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** 按钮型UI单元对象
    @remark 按钮等事物
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Button : public Widget
    {
    public:
        Button(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
			ScriptTypeID stid = N_CMD_Button, LangID lid = N_PrimaryLang);

        virtual ~Button();

        /** 设置是否处于按下状态
        @version NIIEngine 3.0.0
        */
        void press(bool b);

		/** 设置是否处于选中状态
		@version NIIEngine 3.0.0
		*/
		void select(bool b);

		/** 获取是否处于选中状态
		@version NIIEngine 3.0.0
		*/
		bool isSelect() const;
        
        /** 游标是否在这个UI单元对象上方
        @version NIIEngine 3.0.0
        */
        bool isHover() const;

        /** 这个UI单元对象是否处于被按下状态
        @version NIIEngine 3.0.0
        */
        bool isPress() const;
    protected:
		/// @copydetails Button::onSelectChange
		virtual void onSelectChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorLeave
        virtual void onCursorLeave(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
	public:
		/** 选中属性
		@version NIIEngine 3.0.0
		*/
		static const PropertyID SelectProperty;

		/** 属性总数
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PropertyCount;

		/** 选中事件
		@version NIIEngine 3.0.0
		*/
		static const EventID SelectChangeEvent;

		/** 事件总数
		@version NIIEngine 3.0.0
		*/
		static const EventID EventCount;
    protected:
		///@copydetails Widget::int;
		bool init(PropertyCmdSet * dest);

        /** 更新UI单元对象
        @param pos UI体系中的位
        */
        void update(const Vector2f & pos);
    protected:
        bool mPress;    ///< 是否处于被按下状态
        bool mOver;     ///< 游标覆盖状态下
		bool mSelect;
    };
}
}
#endif