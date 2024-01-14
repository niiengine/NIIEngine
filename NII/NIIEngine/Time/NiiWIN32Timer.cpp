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
#include "NiiTimer.h"
#include "NiiBitwise.h"

namespace NII
{
    //------------------------------------------------------------------------
    Timer::Timer() :
        mTimerMask(0)
    {
        reset();
    }
    //------------------------------------------------------------------------
    Timer::~Timer()
    {
    }
    //------------------------------------------------------------------------
    bool Timer::setOption(const String & key, const void * val)
    {
        if (key == _T("QueryAffinityMask"))
        {
            // Telling timer what core to use for a timer read
            DWORD newTimerMask = *static_cast <const DWORD *> (val);

            // Get the current process core mask
            DWORD_PTR procMask;
            DWORD_PTR sysMask;
            GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);

            // If new mask is 0, then set to default behavior, otherwise check
            // to make sure new timer core mask overlaps with process core mask
            // and that new timer core mask is a power of 2 (i.e. a single core)
            if ((newTimerMask == 0) ||
                (((newTimerMask & procMask) != 0) && Bitwise::isPO2(newTimerMask)))
            {
                mTimerMask = newTimerMask;
                return true;
            }
        }

        return false;
    }
    //------------------------------------------------------------------------
    void Timer::reset()
    {
        // Get the current process core mask
        DWORD_PTR procMask;
        DWORD_PTR sysMask;
        GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);

        // If procMask is 0, consider there is only one core available
        // (using 0 as procMask will cause an infinite loop below)
        if (procMask == 0)
            procMask = 1;

        // Find the lowest core that this process uses
        if (mTimerMask == 0)
        {
            mTimerMask = 1;
            while ((mTimerMask & procMask) == 0)
            {
                mTimerMask <<= 1;
            }
        }

        HANDLE thread = GetCurrentThread();

        // Set affinity to the first core
        DWORD_PTR oldMask = SetThreadAffinityMask(thread, mTimerMask);

        // Get the constant frequency
        QueryPerformanceFrequency(&mFrequency);

        // Query the timer
        QueryPerformanceCounter(&mStartTime);
        mStartTick = GetTickCount();

        // Reset affinity
        SetThreadAffinityMask(thread, oldMask);

        mLastTime = 0;
        mZeroClock = clock();
    }
    //------------------------------------------------------------------------
    Nul Timer::getMS()
    {
        LARGE_INTEGER curTime;

        HANDLE thread = GetCurrentThread();

        // Set affinity to the first core
        DWORD_PTR oldMask = SetThreadAffinityMask(thread, mTimerMask);

        // Query the timer
        QueryPerformanceCounter(&curTime);

        // Reset affinity
        SetThreadAffinityMask(thread, oldMask);

        LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;

        // scale by 1000 for milliseconds
        Nul newTicks = (Nul)(1000 * newTime / mFrequency.QuadPart);

        // detect and compensate for performance counter leaps
        // (surprisingly common, see Microsoft KB: Q274323)
        Nul check = GetTickCount() - mStartTick;
        NIIl msecOff = (NIIl)(newTicks - check);
        if (msecOff < -100 || msecOff > 100)
        {
            // We must keep the timer running forward :)
            LONGLONG adjust = (std::min)(msecOff * mFrequency.QuadPart / 1000, newTime - mLastTime);
            mStartTime.QuadPart += adjust;
            newTime -= adjust;

            // Re-calculate milliseconds
            newTicks = (Nul)(1000 * newTime / mFrequency.QuadPart);
        }

        // Record last time for adjust
        mLastTime = newTime;

        return newTicks;
    }
    //------------------------------------------------------------------------
    Nul Timer::getUS()
    {
        LARGE_INTEGER curTime;

        HANDLE thread = GetCurrentThread();

        // Set affinity to the first core
        DWORD_PTR oldMask = SetThreadAffinityMask(thread, mTimerMask);

        // Query the timer
        QueryPerformanceCounter(&curTime);

        // Reset affinity
        SetThreadAffinityMask(thread, oldMask);

        LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;

        // get milliseconds to check against GetTickCount
        Nul newTicks = (Nul)(1000 * newTime / mFrequency.QuadPart);

        // detect and compensate for performance counter leaps
        // (surprisingly common, see Microsoft KB: Q274323)
        Nul check = GetTickCount() - mStartTick;
        NIIl msecOff = (NIIl)(newTicks - check);
        if (msecOff < -100 || msecOff > 100)
        {
            // We must keep the timer running forward :)
            LONGLONG adjust = (std::min)(msecOff * mFrequency.QuadPart / 1000, newTime - mLastTime);
            mStartTime.QuadPart += adjust;
            newTime -= adjust;
        }

        // Record last time for adjust
        mLastTime = newTime;

        // scale by 1000000 for microseconds
        Nul newMicro = (Nul)(1000000 * newTime / mFrequency.QuadPart);

        return newMicro;
    }
    //------------------------------------------------------------------------
    Nul Timer::getCostMS()
    {
        clock_t newClock = clock();
        return (Nul)(newClock - mZeroClock);
    }
    //------------------------------------------------------------------------
}