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

#ifndef _NII_UI_STYLE_H_
#define _NII_UI_STYLE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIStyleWidget.h"
#include "NiiUIStyleLayout.h"
#include "NiiUIStyleSection.h"
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
        inline StyleID getID() const            { return mID; }

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
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;

        /**
        @version NIIEngine 3.0.0
        */
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