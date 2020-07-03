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

#ifndef _NII_STYLE_PROPERTY_H_
#define _NII_STYLE_PROPERTY_H_

#include "NiiUIPreInclude.h"
#include "NiiUIException.h"
#include "NiiUIWidget.h"
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
            mCustomDataID(pid + Widget::InnerPropertyLimit),
            PropertyCmd(pid + Widget::InnerPropertyLimit, _T(""), help)
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
}
}
#endif