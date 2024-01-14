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

#include "NiiThreadTimer.h"
#include "SystemClock.h"
#include <algorithm>

namespace NII
{
    //-----------------------------------------------------------------------
    ThreadTimer::ThreadTimer(ThreadMain * _main) :
        Thread("Timer", _main),
        mTimerOut(0),
        mLastTime(0),
        mLoop(false)
    { 
    }
    //-----------------------------------------------------------------------
    ThreadTimer::~ThreadTimer()
    {
        Stop(true);
    }
    //-----------------------------------------------------------------------
    bool ThreadTimer::start(TimeDurMS timeout, bool loop)
    {
        if(mFunc == NULL || timeout == 0 || isRun())
            return false;

        mTimerOut = timeout;
        mLoop = loop;

        Create();
        return true;
    }
    //-----------------------------------------------------------------------
    bool ThreadTimer::stop(bool wait)
    {
        if(!isRun())
            return false;

        m_bStop = true;
        mTimeEvent.Set();
        stop(wait);

        return true;
    }
    //-----------------------------------------------------------------------
    bool ThreadTimer::restart(TimeDurMS timeout)
    {
        if(timeout)
        {
            if (!isRun())
                return false;

            Stop(true);
            return Start(mTimerOut, mLoop);
        }
        else
        {
            mTimerOut = timeout;
            mLastTime = XbmcThreads::SystemClockMillis() + timeout;
            mTimeEvent.Set();
        }
        return true;
    }
    //-----------------------------------------------------------------------
    float ThreadTimer::geCostMS() const
    {
        if (!isRun())
            return 0.0f;

        return (float)(XbmcThreads::SystemClockMillis() - (mLastTime - mTimerOut));
    }
    //-----------------------------------------------------------------------
    void ThreadTimer::process()
    {
        while (!m_bStop)
        {
            uint32_t currentTime = XbmcThreads::SystemClockMillis();
            mLastTime = currentTime + mTimerOut;

            // wait the necessary time
            if (!mTimeEvent.wait(mTimerOut))
            {
                currentTime = XbmcThreads::SystemClockMillis();
                if (mLastTime <= currentTime)
                {
                    if (mMain)
                        mMain->Run();

                    // continue if this is an interval timer, or if it was restarted during callback
                    if (!mLoop && mLastTime <= currentTime)
                        break;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
}