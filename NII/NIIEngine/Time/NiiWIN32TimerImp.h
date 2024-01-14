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

#ifndef _NII_Win32Timer_H_
#define _NII_Win32Timer_H_

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX) && defined(_MSC_VER)
    #define NOMINMAX
#endif
#include "windows.h"

namespace NII
{
    class _EngineAPI Timer : public TimerAlloc
    {
    public:
        Timer();
        ~Timer();

        bool setOption(const String & strKey, const void * value);

        /// 重置时间器
        void reset();

        /** 返回初始化或复位后度过的毫秒
        @remark 毫秒
        */
        Nul getMS();

        /** 返回初始化或复位后度过的微秒
        @remark 微秒
        */
        Nul getUS();

        /** 返回初始化或复位后度过的毫秒,CPU测量时间
        @remark 毫秒
        */
        Nul getCostMS();
    private:
        clock_t mZeroClock;

        DWORD mStartTick;
        LONGLONG mLastTime;
        LARGE_INTEGER mStartTime;
        LARGE_INTEGER mFrequency;

        DWORD_PTR mTimerMask;
    };
}
#endif
