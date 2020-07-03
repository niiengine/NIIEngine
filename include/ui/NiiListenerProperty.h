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

#ifndef _NII_LISTENER_PROPERTY_H_
#define _NII_LISTENER_PROPERTY_H_

#include "NiiUIPreInclude.h"
#include "NiiPropertyCmd.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** ����/ͬ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerProperty : public PropertyCmd
    {
    public:
        ListenerProperty(PropertyID pid, WidgetID widget, PropertyID wpid,
            bool refresh, bool layout, EventID eid) :
                mRefresh(refresh),
                mLayout(layout),
                mSetEvent(eid),
                PropertyCmd(pid, _T("Listener"), _T("ListenerProperty"))
        {
            if((widget != 0) || (wpid != 0))
                add(widget, wpid);
        }

        ListenerProperty(const ListenerProperty & o) :
            PropertyCmd(o.mID, o.mName, o.mVerbose),
            mRefresh(o.mRefresh),
            mLayout(o.mLayout),
            mSetEvent(o.mSetEvent)
        {
            mListeners = o.mListeners;
        }

        ~ListenerProperty()
        {
        }

        /** ����ӵ�Ԫ����ͬ��
        @version NIIEngine 3.0.0
        */
        void add(WidgetID widget, PropertyID pid);

        /** ��ȥ�ӵ�Ԫ����ͬ��
        @version NIIEngine 3.0.0
        */
        bool remove(WidgetID wid, PropertyID pid);

        /** �Ƿ�����ӵ�Ԫ����ͬ��
        @version NIIEngine 3.0.0
        */
        bool isExist(WidgetID wid, PropertyID pid) const;

        /** ��ȥ�����ӵ�Ԫ����ͬ��
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** �����Ƿ�ˢ����Ⱦ
        @version NIIEngine 3.0.0
        */
        inline void setRefreshRender(bool b)           { mRefresh = b; }

        /** ��ȡ�Ƿ�ˢ����Ⱦ
        @version NIIEngine 3.0.0
        */
        inline bool isRefreshRender() const            { return mRefresh; }

        /** �����Ƿ����Ű�
        @version NIIEngine 3.0.0
        */
        inline void setLayoutStyle(bool b)              { mLayout = b; }

        /** ��ȡ�Ƿ����Ű�
        @version NIIEngine 3.0.0
        */
        inline bool isLayoutStyle() const               { return mLayout; }

        /** ���ô����¼�
        @version NIIEngine 3.0.0
        */
        inline void setEvent(EventID eid)               { mSetEvent = eid; }

        /** ��ȡ�����¼�
        @version NIIEngine 3.0.0
        */
        inline EventID getEvent() const                 { return mSetEvent; }

        /** ��ȡ�ӵ�Ԫ
        @version NIIEngine 3.0.0
        */
        Widget * getDest(const PropertyCmdObj * obj, WidgetID wid) const;

        /// @copydetails PropertyCmd::write
        virtual void write(XmlSerializer * out) const;

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const;

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & value);

        /// @copydetails PropertyCmd::init
        void init(PropertyCmdObj * own) const;

        /// @copydetails PropertyCmd::isSerialize
        bool isSerialize() const;

        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const;

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const;
    protected:
        typedef vector<std::pair<WidgetID, PropertyID> >::type Listeners;

        Listeners mListeners;
        EventID mSetEvent;
        bool mRefresh;
        bool mLayout;

    };
}
}
#endif