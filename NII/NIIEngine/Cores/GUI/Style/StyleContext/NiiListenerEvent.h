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
    /** 监听级事件
    @remark Listener都是可以广播的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerEvent : public EventAlloc
    {
    public:
        ListenerEvent(EventID eid);

        /** 设置监听事件ID
        @param[in] eid
        @version NIIEngine 3.0.0
        */
        inline void setID(EventID eid){ mEventID = eid; }

        /** 获取监听事件ID
        @version NIIEngine 3.0.0
        */
        inline EventID getID() const{ return mEventID; }

        /** 添加监听事件
        @param[in] wid 所监听的UI单元
        @param[in] eid 所监听的事件
        @version NIIEngine 3.0.0
        */
        void add(WidgetID wid, EventID eid);

        /** 移去监听事件
        @param[in] wid 所监听的UI单元
        @param[in] eid 所监听的事件
        @version NIIEngine 3.0.0
        */
        void remove(WidgetID wid, EventID eid);

        /** 移去所有监听事件
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 把监听事件附加到UI单元中
        @param[in] dst UI单元
        @version NIIEngine 3.0.0
        */
        void attach(Widget * dst) const;

        /** 从UI单元中移去监听事件
        @param[in] dst UI单元
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