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
#include "NiiThreadLock.h"
#include "NiiTimer.h"
#include <chrono>

namespace NII
{
	class EndTime
	{
		unsigned int startTime;
		unsigned int totalWaitTime;
	public:
		static const unsigned int InfiniteValue;
		inline EndTime() : startTime(0), totalWaitTime(0) {}
		inline EndTime(unsigned int millisecondsIntoTheFuture) : startTime(SystemTick()), totalWaitTime(millisecondsIntoTheFuture) {}

		inline void Set(unsigned int millisecondsIntoTheFuture) { startTime = SystemTick(); totalWaitTime = millisecondsIntoTheFuture; }

		inline bool IsTimePast() const { return totalWaitTime == InfiniteValue ? false : (totalWaitTime == 0 ? true : (SystemTick() - startTime) >= totalWaitTime); }

		inline unsigned int MillisLeft() const
		{
			if (totalWaitTime == InfiniteValue)
				return InfiniteValue;
			if (totalWaitTime == 0)
				return 0;
			unsigned int timeWaitedAlready = (SystemTick() - startTime);
			return (timeWaitedAlready >= totalWaitTime) ? 0 : (totalWaitTime - timeWaitedAlready);
		}

		inline void SetExpired() { totalWaitTime = 0; }
		inline void SetInfinite() { totalWaitTime = InfiniteValue; }
		inline bool IsInfinite(void) const { return (totalWaitTime == InfiniteValue); }
		inline unsigned int GetInitialTimeoutValue(void) const { return totalWaitTime; }
		inline unsigned int GetStartTime(void) const { return startTime; }
	};
	const unsigned int EndTime::InfiniteValue = std::numeric_limits<unsigned int>::max();
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ThreadMutex
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    NCount ThreadMutex::unlock(NCount leave)
    {
        NCount ret = 0;
        if(tryLock())
        {
            if(leave < (mCount - 1))
            {
                ret = mCount - 1 - leave;  // The -1 is because we don't want
                                    //  to count the tryLock increment.
                // We must NOT compare "count" in this loop since
                // as soon as the last unlock is called another thread
                // can modify it.
                for(NCount i = 0; i < ret; ++i)
                {
                    unlock();
                }
            }
            unlock(); // undo the tryLock before returning
        }

        return ret;
    }
    //------------------------------------------------------------------------
    void ThreadMutex::lock(NCount cnt)
    {
        for(NCount i = 0; i < cnt; ++i)
        {
            lock();
        }
    }
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// ThreadCondition
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ThreadCondition::wait(ThreadMutex & mutex)
	{
		NCount count = mutex.mCount;
		mutex.mCount = 0;
		mCondition.wait(mutex.getMutex());
		mutex.mCount = count;
	}
	//------------------------------------------------------------------------
	bool ThreadCondition::wait(ThreadMutex & mutex, Nui64 ms)
	{
		NCount count = mutex.mCount;
		mutex.mCount = 0;
		std::cv_status res = mCondition.wait_for(mutex.getMutex(), std::chrono::milliseconds(ms));
		mutex.mCount = count;
		return res == std::cv_status::no_timeout;
	}
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// ThreadConditionEvent
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ThreadConditionEvent::wait(ThreadMutex & mutex)
	{
		while (!mSignal)
			mCondition.wait(mutex);
	}
	//------------------------------------------------------------------------
	bool ThreadConditionEvent::wait(ThreadMutex & mutex, Nui64 ms)
	{
		bool ret = true;
		if (!mSignal)
		{
			if (!ms)
			{
				mCondition.wait(mutex, ms);
				return !(!mSignal);
			}
			else
			{
				EndTime endTime((NCount)ms);
				for (bool notdone = true; notdone && ret == true;
					ret = (notdone = (!mSignal)) ? ((ms = endTime.MillisLeft()) != 0) : true)
					mCondition.wait(mutex, ms);
			}
		}
		return ret;
	}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ScopeSpinLock
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ScopeSpinLock::ScopeSpinLock(std::atomic_flag & lock) : mLock(lock)
    {
        while (atomic_flag_test_and_set(&mLock));
    }
    //------------------------------------------------------------------------
    ScopeSpinLock::~ScopeSpinLock()
    {
        std::atomic_flag_clear(&mLock);
    }
    //------------------------------------------------------------------------
}