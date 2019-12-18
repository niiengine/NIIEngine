/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-26

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

#ifndef _NII_UI_STYLE_H_
#define _NII_UI_STYLE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIStyleWidget.h"
#include "NiiUIStyleLayout.h"
#include "NiiUIStyleSection.h"
#include "NiiAssignProperty.h"
#include "NiiStylePropertyCmd.h"
#include "NiiListenerProperty.h"
#include "NiiListenerEvent.h"
#include "NiiUIStyleSpecial.h"
#include "NiiAnimationObj.h"

namespace NII
{
namespace UI
{
    class StyleOuter;

    /** UI元素风格发生变化时的参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleEventArgs : public WidgetEventArgs
    {
    public:
        StyleEventArgs();
        StyleEventArgs(Widget * e, Style * obj);

        Style * mStyle;     ///< 风格
    };

    /** 风格
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Style : public AnimationObj, public AnimationFusionObj
    {
        friend class Widget;
        friend class StyleOuter;
    public:
        Style();
        Style(const Style & o);
        Style(StyleID sid, StyleID psid = 0);

        virtual ~Style();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        StyleID getID() const;

        /** 添加风格状态
        @par 这将产生副本数据
        @version NIIEngine 3.0.0
        */
        void add(const StyleLayout & obj);

        /** 添加风格区域
        @par 这将产生副本数据
        @version NIIEngine 3.0.0
        */
        void add(const StyleSection & obj);

        /** 添加风格组件
        @par 这将产生副本数据
        @version NIIEngine 3.0.0
        */
        void add(const StyleWidget & obj);

        /** 添加风格特例组件
        @par 这将产生副本数据
        @version NIIEngine 3.0.0
        */
        void add(const StyleSpecial & obj);

        /** 添加风格基础属性
        @par 这将产生副本数据
        @version NIIEngine 3.0.0
        */
        void add(const AssignProperty & obj);

        /** 添加监听事件
        @version NIIEngine 3.0.0
        */
        void add(const ListenerEvent & obj);

        /** 添加风格属性
        @param obj
        */
        void add(StylePropertyCmd * obj);

        /** 添加监听属性
        @param obj
        */
        void add(ListenerProperty * obj);

        /**
        @param[in] id
        @version NIIEngine 3.0.0
        */
        const StyleLayout & getLayout(Nid id) const;

        /**
        @version NIIEngine 3.0.0
        */
        const StyleSection & getDesign(Nid id) const;

        /** 获取风格
        @version NIIEngine 3.0.0
        */
        const StyleSpecial & getSpecial(Nid id) const;

        /**
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        const AssignProperty * getAssign(PropertyID pid) const;

        /** 获取风格单元
        @param[in] wid
        @version NIIEngine 3.0.0
        */
        const StyleWidget * getWidget(WidgetID wid) const;
        
        /**
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        const ListenerProperty * getListener(PropertyID pid) const;

        /**
        @param src
        @version NIIEngine 3.0.0
        */
        void layout(Widget * src) const;

        /** 附加到
        @param dst
        @version NIIEngine 3.0.0
        */
        void attach(Widget * dst) const;

        /**
        @param dst
        @version NIIEngine 3.0.0
        */
        void detach(Widget * dst) const;

        /**
        @param id
        @version NIIEngine 3.0.0
        */
        bool isStateExist(Nid id) const;

        /**
        @param id
        @version NIIEngine 3.0.0
        */
        bool isSpecialExist(Nid id) const;

        /**
        @param[out] out
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;

        bool notifyFont(Widget * w, const Font * font) const;
    protected:
		Style & operator=(const Style & o);
	protected:
        typedef map<Nid, StyleLayout>::type States;
        typedef map<Nid, StyleSection>::type Designs;
        typedef map<Nid, StyleSpecial>::type Specials;
        typedef vector<StyleWidget>::type Widgets;
        typedef vector<AssignProperty>::type AssignPropertyList;
        typedef vector<StylePropertyCmd *>::type StylePropertyList;
        typedef vector<ListenerProperty *>::type ListenerPropertyList;
        typedef vector<ListenerEvent>::type ListenerEvents;
    protected:
		StyleID mID;
		StyleID mInheritID;

        Designs mDesigns;
        Widgets mWidgets;
        States mModes;
        Specials mSpecials;
        AssignPropertyList mAssignPropertyList;
        StylePropertyList mStylePropertyList;
        ListenerPropertyList mListenerPropertyList;
        ListenerEvents mListenerEvents;
    };
}
}
#endif