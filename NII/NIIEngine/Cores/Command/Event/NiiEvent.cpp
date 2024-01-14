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
#include "NiiEvent.h"

namespace NII
{
namespace NII_COMMAND
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // EventFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor() :
        mObj(0),
        mFunc(0)
    {
    }
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor(Event * obj):
        mObj(const_cast<Event *>(obj)),
        mFunc(0)
    {
    }
	//------------------------------------------------------------------------
	EventFunctor::EventFunctor(Functor * func) :
		mObj(0),
		mFunc(func)
	{
	}
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor(const EventFunctor & o) :
        mObj(o.mObj),
        mFunc(o.mFunc->clone())
    {
    }
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor(const Event * dst, const EventFunctor & o) :
        mObj(const_cast<Event *>(dst)),
        mFunc(o.mFunc->clone())
    {
    }
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor(Functor & func) :
        mObj(0),
        mFunc(N_new RefFunctor(func))
    {
    }
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor(const Functor & func) :
        mObj(0),
        mFunc(N_new CopyFunctor(func))
    {
    }
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor(Event * obj, EventMethod m) :
        mObj(0),
        mFunc(N_new MethodFunctor(obj, m))
    {
    }
    //------------------------------------------------------------------------
    EventFunctor::EventFunctor(CommandObj * obj, CommandObjFunctor mf) :
        mObj(0),
        mFunc(N_new CommandFunctor(obj, mf))
    {
    }
    //------------------------------------------------------------------------
    EventFunctor::~EventFunctor()
    {
        disconnect();
    }
    //------------------------------------------------------------------------
    bool EventFunctor::operator ==(const EventFunctor & o) const
    {
        return  mObj == o.mObj && mFunc == o.mFunc;
    }
    //------------------------------------------------------------------------
    bool EventFunctor::operator !=(const EventFunctor & o) const
    {
        return mObj != o.mObj || mFunc != o.mFunc;
    }
    //------------------------------------------------------------------------
    EventFunctor & EventFunctor::operator =(const EventFunctor & o)
    {
        mObj = o.mObj;
        mFunc = o.mFunc->clone();

        return *this;
    }
    //------------------------------------------------------------------------
    bool EventFunctor::connected() const
    {
        return mFunc != 0;
    }
    //------------------------------------------------------------------------
    void EventFunctor::disconnect()
    {
		if (mObj != 0)
		{
			mObj->remove(this);
			mObj = 0;
		}

        if(mFunc != 0)
        {
            N_delete mFunc;
            mFunc = 0;
        }
    }
    //------------------------------------------------------------------------
    void EventFunctor::cleanup()
    {
        if(mFunc != 0)
        {
            N_delete mFunc;
            mFunc = 0;
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Event
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Event::Event(EventID id):
        mID(id)
    {
    }
    //------------------------------------------------------------------------
    Event::~Event()
    {
        mSignalList.clear();
    }
    //------------------------------------------------------------------------
    SignalPtr Event::add(EventFunctor * ef)
    {
        SignalPtr slot(ef, SignalPtr::MU_Delete);
        ef->mObj = this;
        mSignalList.push_back(slot);
        return slot;
    }
    //------------------------------------------------------------------------
    void Event::remove(EventFunctor * ef)
    {
        SignalPtrs::iterator i, iend = mSignalList.end();
        for(i = mSignalList.begin(); i != iend; ++i)
        {
            if(**i == *ef)
            {
                mSignalList.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void Event::operator()(const EventArgs * args)
    {
        SignalPtrs::iterator i, iend = mSignalList.end();
        for(i = mSignalList.begin(); i != iend; ++iend)
        {
            (**i)(args);
        }
    }
    //------------------------------------------------------------------------
    bool Event::isInvalid() const
    {
        return mSignalList.empty();
    }
    //------------------------------------------------------------------------
    void Event::execute(const EventArgs * args)
    {
        SignalPtrs::iterator i, iend = mSignalList.end();
        for(i = mSignalList.begin(); i != iend; ++iend)
        {
            (**i)(args);
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // EventSignal
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    EventSignal::EventSignal()
    {

    }
    //------------------------------------------------------------------------
    EventSignal::~EventSignal()
    {
        disconnect();
    }
    //------------------------------------------------------------------------
    EventSignal::EventSignal(const SignalPtr & signal) :
        mSignal(signal)
    {
    }
    //------------------------------------------------------------------------
    EventSignal & EventSignal::operator=(const SignalPtr & signal)
    {
        mSignal = signal;
        return *this;
    }
    //------------------------------------------------------------------------
    bool EventSignal::connected() const
    {
        return mSignal.isValid() ? mSignal->connected() : false;
    }
    //------------------------------------------------------------------------
    void EventSignal::disconnect()
    {
        if (mSignal.isValid())
            mSignal->disconnect();
    }
    //------------------------------------------------------------------------
}
}