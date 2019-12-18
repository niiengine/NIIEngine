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

#ifndef _NII_UI_TAB_BUTTON_H_
#define _NII_UI_TAB_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIButton.h"

namespace NII
{
namespace UI
{
    /** 标签控件专用按纽
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabButton : public Button
    {
	public:
		TabButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);

        virtual ~TabButton();

        /** 设置是否处于选中
        version NIIEngine 3.0.0
        */
        virtual void select(bool b);

        /** 获取是否处于选中
        @version NIIEngine 3.0.0
        */
        bool isSelect() const;

        /** 设置目标标签容器
        @version NIIEngine 3.0.0
        */
        void setDest(Container * c);

        /** 获取目标标签容器
        @version NIIEngine 3.0.0
        */
        Container * getDest();
    protected:
        /** 点击时触发
        @version NIIEngine 3.0.0
        */
        virtual void onClick(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);
    public:
        /** 点击事件
        @version NIIEngine 3.0.0
        */
        static const EventID ClickEvent;

        /** 拖拽事件
        @version NIIEngine 3.0.0
        */
        static const EventID DragEvent;

        /** 滚动事件
        @version NIIEngine 3.0.0
        */
        static const EventID ScrollEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Container * mDest;  ///< 标签容器
        bool mSelect;       ///< 
        bool mDrag;         ///< 
    };
}
}
#endif