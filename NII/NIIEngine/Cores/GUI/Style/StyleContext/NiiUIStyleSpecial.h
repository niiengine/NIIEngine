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

#ifndef _NII_UI_StyleSpecial_H_
#define _NII_UI_StyleSpecial_H_

#include "NiiUIPreInclude.h"
#include "NiiUIException.h"
#include "NiiSizeDefine.h"
#include "NiiXMLSerializer.h"
#include "NiiLogManager.h"

namespace NII
{
namespace UI
{
    
    class _EngineAPI StylePropertyCmd : public PropertyCmd
    {
    public:
        StylePropertyCmd(PropertyID pid, const String & help, bool refresh, bool layout, EventID eid) :
            mRefresh(refresh),
            mLayout(layout),
            mSetEvent(eid),
            mCustomDataID(pid + WidgetInnerProperty),
            PropertyCmd(pid + WidgetInnerProperty, _T(""), help)
        {
        }

        ~StylePropertyCmd()
        {
        }

        /** 设置是否刷新渲染
        @version NIIEngine 3.0.0
        */
        inline void setRefreshRender(bool b)           { mRefresh = b; }

        /** 获取是否刷新渲染
        @version NIIEngine 3.0.0
        */
        inline bool isRefreshRender() const            { return mRefresh; }

        /** 设置是否风格排版
        @version NIIEngine 3.0.0
        */
        inline void setLayoutStyle(bool b)             { mLayout = b; }

        /** 获取是否风格排版
        @version NIIEngine 3.0.0
        */
        inline bool isLayoutStyle() const              { return mLayout; }

        /** 设置触发事件
        @version NIIEngine 3.0.0
        */
        inline void setEvent(EventID eid)              { mSetEvent = eid; }

        /** 获取触发事件
        @version NIIEngine 3.0.0
        */
        inline EventID getEvent() const                { return mSetEvent; }

        /// @copydetails PropertyCmd::isSerialize
        bool isSerialize() const;

        /// @copydetails PropertyCmd::init
        void init(PropertyCmdObj * obj) const;

        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const;

        PropertyCmd * clone() const;

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * obj, String & out) const;

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * obj, const String & value);
        
        virtual void write(XmlSerializer * dst) const;
    protected:
        EventID mSetEvent;
        Nid mCustomDataID;
        bool mRefresh;
        bool mLayout;
    };
    
    /** 描述UI单元或图象成分
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleArea : public UIAlloc
    {
    public:
        StyleArea();

        /** 获取区域的像素区域
        @param w
        @version NIIEngine 3.0.0
        */
        Rectf getPixelRect(const Widget & w) const;

        /** 获取区域的像素区域
        @param w
        @param container
        @version NIIEngine 3.0.0
        */
        Rectf getPixelRect(const Widget & w, const Rectf & container) const;

        /**设置
        @version NIIEngine 3.0.0
        */
        void setSrcFetch(StyleID sid, Nid style_special);

        /**
        @version NIIEngine 3.0.0
        */
        void setPropertyFetch(PropertyID pid);

        /**
        @version NIIEngine 3.0.0
        */
        inline PropertyID getPropertyFetch() const      { return mPropertyID; }

        /** 通过属性获取
        @version NIIEngine 3.0.0
        */
        inline bool isPropertyFetch() const             { return mPropertyID != 0 && mStyleID == 0; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSpecialFetch() const              { return mStyleID != 0 && mPropertyID != 0; }

        /**
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * widget, const Font * font) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    public:
        SizeDefine mLeft;
        SizeDefine mTop;
        SizeDefine mXDim;
        SizeDefine mYDim;
    private:
        PropertyID mPropertyID;
        StyleID mStyleID;
    };

    /** 风格特定
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSpecial : public UIAlloc
    {
    public:
        StyleSpecial();
        StyleSpecial(Nid id);

        /** 设置ID
        @version NIIEngine 3.0.0
        */
        inline void setID(Nid id)                  {  mID = id; }

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const                   { return mID; }

        /** 设置区域
        @version NIIEngine 3.0.0
        */
        inline void setArea(const StyleArea & area){ mArea = area; }

        /** 获取区域
        @version NIIEngine 3.0.0
        */
        inline const StyleArea & getArea() const   { return mArea; }

        /** 写入到XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;

        /** 字体改变时触发
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * obj, const Font * font) const;
    private:
        Nid mID;
        StyleArea mArea;
    };
}
}
#endif