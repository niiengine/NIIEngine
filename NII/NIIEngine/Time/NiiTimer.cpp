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

#if N_PLAT == N_PLAT_OSX
    #include <mach/mach_time.h>
    #include <CoreVideo/CVHostTime.h>
#elif N_PLAT == N_PLAT_WIN32
	#include <Mmsystem.h>
    #include <windows.h>
#else
    #include <time.h>
#endif

namespace NII
{
    unsigned int SystemTick()
    {
        uint64_t now_time;
        static uint64_t start_time = 0;
        static bool start_time_set = false;
    #if N_PLAT == N_PLAT_OSX
        now_time = CVGetCurrentHostTime() *  1000 / CVGetHostClockFrequency();
    #elif N_PLAT == N_PLAT_WIN32
        now_time = (uint64_t)timeGetTime();
    #else
        struct timespec ts = {};
      #ifdef CLOCK_MONOTONIC_RAW
        clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
      #else
        clock_gettime(CLOCK_MONOTONIC, &ts);
      #endif // CLOCK_MONOTONIC_RAW
        now_time = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    #endif
        if (!start_time_set)
        {
          start_time = now_time;
          start_time_set = true;
        }
        return (unsigned int)(now_time - start_time);
    }
}