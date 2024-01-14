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

#ifndef _NII_THREAD_INCLUDE_H_
#define _NII_THREAD_INCLUDE_H_

#include <atomic>
#include <future>
#include <thread>
#include "NiiDefaultThread.h"

#if N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
#include <mach/mach.h>
#endif

#define N_mutex1_name mMutex

#if N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_LINUX_X || N_PLAT == N_PLAT_OSX|| N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_ANDROID
    #include <sys/types.h>
    #include <unistd.h>
    namespace NII
    {
        constexpr int N_ThreadBasePriorityLow = 15;
        constexpr int N_ThreadBasePriorityMax = 2;
        constexpr int N_ThreadBasePriorityMin = -2;
        constexpr int N_ThreadBasePriorityIdle = -15;
        constexpr int N_ThreadPriorityHigh = N_ThreadBasePriorityMax;
        constexpr int N_ThreadPriorityLow = N_ThreadBasePriorityMin;
        constexpr int N_ThreadPriorityBelowNormal = N_ThreadPriorityLow + 1;
        constexpr int N_ThreadPriorityNormal = 0;
        constexpr int N_ThreadPriorityAboveNormal = N_ThreadPriorityHigh - 1;

        typedef pid_t ThreadId;
    }
#elif N_PLAT == N_PLAT_WIN32
    #if !defined(NOMINMAX) && defined(_MSC_VER)
    #define NOMINMAX
    #endif
    #include <windows.h>
    namespace NII
    {
        typedef HANDLE ThreadId;

        constexpr int N_ThreadBasePriorityLow = THREAD_BASE_PRIORITY_LOWRT;
        constexpr int N_ThreadBasePriorityMax = THREAD_BASE_PRIORITY_MAX;
        constexpr int N_ThreadBasePriorityMin = THREAD_BASE_PRIORITY_MIN;
        constexpr int N_ThreadBasePriorityIdle = THREAD_BASE_PRIORITY_IDLE;
        constexpr int N_ThreadPriorityHigh = THREAD_PRIORITY_HIGHEST;
        constexpr int N_ThreadPriorityLow = THREAD_PRIORITY_LOWEST;
        constexpr int N_ThreadPriorityBelowNormal = THREAD_PRIORITY_BELOW_NORMAL;
        constexpr int N_ThreadPriorityNormal = THREAD_PRIORITY_NORMAL;
        constexpr int N_ThreadPriorityAboveNormal = THREAD_PRIORITY_ABOVE_NORMAL;
    }
#endif

#endif