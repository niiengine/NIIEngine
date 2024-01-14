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
#include "NiiCommandObj.h"
#include "NiiBitSet.h"
#include "NiiEngine.h"
#include "NiiScriptModule.h"

namespace NII
{
namespace NII_COMMAND
{
    //------------------------------------------------------------------------
    N_EVENT(NII, CommandObj, NotifyEvent,       0);
    N_EVENT(NII, CommandObj, ParentNotifyEvent, 1);
    N_EVENT(NII, CommandObj, ChildNotifyEvent,  2);
    N_EVENT(NII, CommandObj, MapEvent,          3);
    N_EVENT(NII, CommandObj, UnMapEvent,        4);
    N_EVENT(NII, CommandObj, EventCount,        5);
    //------------------------------------------------------------------------
    CommandObj::CommandObj() :
        mEnable(true)
    {
        mEventMarks = N_new BitSet(CommandObj::EventCount);
    }
    //------------------------------------------------------------------------
    CommandObj::~CommandObj()
    {
        Events::iterator it, itend = mEvents.end();
        for(it = mEvents.begin(); it != itend; ++it)
            N_delete *it;
        mEvents.clear();
        N_delete mEventMarks;
    }
    //------------------------------------------------------------------------
    Event * CommandObj::bind(EventID id)
    {
        NCount iter = id >> IOMark;
        NCount itermark = id % IOValid;

        if(mEventMarks->isTrue(iter, itermark))
            return mEvents[mEventMarks->getCount(iter, itermark)];

        Event * e = N_new Event(id);
        mEvents.insert(mEvents.begin() + mEventMarks->getCount(iter, itermark), e);
        mEventMarks->setTrue(iter, itermark);
        return e;
    }
    //------------------------------------------------------------------------
    SignalPtr CommandObj::bind(EventID id, EventFunctor * slot)
    {
        NCount iter = id >> IOMark;
        NCount itermark = id % IOValid;

        if(mEventMarks->isTrue(iter, itermark))
        {
            return mEvents[mEventMarks->getCount(iter, itermark)]->add(slot);
        }
        else
        {
            Event * e = N_new Event(id);
            mEvents.insert(mEvents.begin() + mEventMarks->getCount(iter, itermark), e);
            mEventMarks->setTrue(iter, itermark);
            return e->add(slot);
        }
        // no reach
        return SignalPtr();
    }
    //------------------------------------------------------------------------
    void CommandObj::unbind(EventID id)
    {
        NCount iter = id >> IOMark;
        NCount itermark = id % IOValid;

        if(mEventMarks->isTrue(iter, itermark))
        {
            Events::iterator it = mEvents.begin() + mEventMarks->getCount(iter, itermark);
            N_delete *it;
            mEvents.erase(it);
            mEventMarks->setFalse(iter, itermark);
        }
    }
    //------------------------------------------------------------------------
    void CommandObj::unbind(EventID id, EventFunctor * fs)
    {
        NCount iter = id >> IOMark;
        NCount itermark = id % IOValid;

        if(mEventMarks->isTrue(iter, itermark))
        {
            Events::iterator it = mEvents.begin() + mEventMarks->getCount(iter, itermark);
            Event * e = *it;
            e->remove(fs);
            if(e->isInvalid())
            {
                N_delete e;
                mEvents.erase(it);
                mEventMarks->setFalse(iter, itermark);
            }
        }
    }
    //------------------------------------------------------------------------
    SignalPtr CommandObj::addScript(ScriptModuleID smid, EventID id, const String & fc)
    {
        return N_Engine().getScriptModule(smid)->add(this, id, fc);
    }
    //------------------------------------------------------------------------
    Event * CommandObj::getEvent(EventID id, bool _add)
    {
        NCount iter = id >> IOMark;
        NCount itermark = id % IOValid;

        Event * temp;
        if(mEventMarks->isTrue(iter, itermark))
        {
            temp = mEvents[mEventMarks->getCount(iter, itermark)];
        }
        else
        {
            if(_add)
            {
                temp = bind(id);
            }
            else
            {
                temp = 0;
            }
        }

        return temp;
    }
    //------------------------------------------------------------------------
    void CommandObj::setSignalEnable(bool s, bool r)
    {
        mEnable = s;
        if(!s && r)
        {
            Events::iterator it, itend = mEvents.end();
            for(it = mEvents.begin(); it != itend; ++it)
                N_delete *it;
            mEvents.clear();
            N_delete mEventMarks;
        }
    }
    //------------------------------------------------------------------------
    bool CommandObj::isSignalEnable() const
    {
        return mEnable;
    }
    //------------------------------------------------------------------------
    bool CommandObj::isInvalid() const
    {
        return mEvents.begin() == mEvents.end();
    }
    //------------------------------------------------------------------------
    void CommandObj::signal(EventID id, const EventArgs * arg)
    {
        if(mEnable)
        {
            NCount iter = id >> IOMark;
            NCount itermark = id % IOValid;
            if(mEventMarks->isTrue(iter, itermark))
                (*mEvents[mEventMarks->getCount(iter, itermark)])(arg);
        }
    }
    //------------------------------------------------------------------------
    void CommandObj::onNotify(const EventArgs * arg)
    {
        signal(NotifyEvent, arg);
    }
    //------------------------------------------------------------------------
    void CommandObj::onParentNotify(const EventArgs * arg)
    {
        signal(ParentNotifyEvent, arg);
    }
    //------------------------------------------------------------------------
    void CommandObj::onChildNotify(const EventArgs * arg)
    {
        signal(ChildNotifyEvent, arg);
    }
    //------------------------------------------------------------------------
    void CommandObj::onMap(const EventArgs * arg)
    {
        signal(MapEvent, arg);
    }
    //------------------------------------------------------------------------
    void CommandObj::onUnMap(const EventArgs * arg)
    {
        signal(UnMapEvent, arg);
    }
    //------------------------------------------------------------------------
    void CommandObj::notifySignal(NCount count)
    {
        mEventMarks->resize(count);
    }
    //------------------------------------------------------------------------
}
}