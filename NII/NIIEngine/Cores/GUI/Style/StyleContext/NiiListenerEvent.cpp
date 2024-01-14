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

#include "NiiPreProcess.h"
#include "NiiListenerEvent.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIContainer.h"
#include "NiiEvent.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // _ListenerEvent
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner _ListenerEvent : public NII_COMMAND::Event
    {
    public:
        _ListenerEvent(EventID id) :
            Event(id)
        {
        }
        
        ~_ListenerEvent()
        {
            SignalPtrs::iterator i, iend = mSignalList.end();
            for (i = mSignalList.begin(); i != iend; ++i)
            {
                (*i)->disconnect();
            }
        }

        void add(Widget * dst)
        {
            SignalPtr sig(dst->bind(mID, this, (EventMethod)&_ListenerEvent::execute));
            mSignalList.push_back(sig);
        }
    };
    //------------------------------------------------------------------------
    ListenerEvent::ListenerEvent(EventID eid) :
        mEventID(eid)
    {
    }
    //------------------------------------------------------------------------
    void ListenerEvent::add(WidgetID wid, EventID eid)
    {
        mListeners.push_back(Npair(wid, eid));
    }
    //------------------------------------------------------------------------
    void ListenerEvent::remove(WidgetID wid, EventID eid)
    {
        Listeners::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            if (i->first == wid && i->second == eid)
            {
                mListeners.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void ListenerEvent::removeAll()
    {
        mListeners.clear();
    }
    //------------------------------------------------------------------------
    void ListenerEvent::attach(Widget * dst) const
    {
        _ListenerEvent * e = N_new _ListenerEvent(mEventID);
        dst->bind(mEventID, N_new EventFunctor(e));

        Listeners::const_iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            WidgetID wid = i->first;
            Widget * target;
            if(wid == 0)
            {
                target = static_cast<Container *>(dst)->get(i->second, true);
                N_assert(target, "error logic");
                e->add(target);
            }
            else if(wid == N_Parent_Obj)
            {
                target = dst->getParent()->get(i->second, true);
                N_assert(target, "error logic");
                e->add(target);
            }
            else
            {
                target = static_cast<Container *>(dst)->get(wid);
                if(target)
                    e->add(static_cast<Container *>(target)->get(i->second, true));
            }
        }
    }
    //------------------------------------------------------------------------
    void ListenerEvent::detach(Widget * dst) const
    {
        dst->unbind(mEventID);
    }
    //------------------------------------------------------------------------
}
}