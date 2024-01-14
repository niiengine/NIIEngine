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

#ifndef _NII_UI_LISTENEREVENT_H_
#define _NII_UI_LISTENEREVENT_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** �������¼�
    @remark Listener���ǿ��Թ㲥��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerEvent : public EventAlloc
    {
    public:
        ListenerEvent(EventID eid);

        /** ���ü����¼�ID
        @param[in] eid
        @version NIIEngine 3.0.0
        */
        inline void setID(EventID eid){ mEventID = eid; }

        /** ��ȡ�����¼�ID
        @version NIIEngine 3.0.0
        */
        inline EventID getID() const{ return mEventID; }

        /** ��Ӽ����¼�
        @param[in] wid ��������UI��Ԫ
        @param[in] eid ���������¼�
        @version NIIEngine 3.0.0
        */
        void add(WidgetID wid, EventID eid);

        /** ��ȥ�����¼�
        @param[in] wid ��������UI��Ԫ
        @param[in] eid ���������¼�
        @version NIIEngine 3.0.0
        */
        void remove(WidgetID wid, EventID eid);

        /** ��ȥ���м����¼�
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** �Ѽ����¼����ӵ�UI��Ԫ��
        @param[in] dst UI��Ԫ
        @version NIIEngine 3.0.0
        */
        void attach(Widget * dst) const;

        /** ��UI��Ԫ����ȥ�����¼�
        @param[in] dst UI��Ԫ
        @version NIIEngine 3.0.0
        */
        void detach(Widget * dst) const;
    protected:
        typedef vector<std::pair<WidgetID, EventID> >::type Listeners;

        EventID mEventID;       ///<
        EventID mDestEventID;   ///<
        Listeners mListeners;   ///<
    };
}
}
#endif