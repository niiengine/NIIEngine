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

#ifndef _NII_TIMER_API_H_
#define _NII_TIMER_API_H_

#include "NiiPreInclude.h"
#include "NiiPlat.h"

namespace NII
{
    ///
    unsigned int SystemTick();
    
    /// a > b?
    inline bool greater(TimeDurMS a, TimeDurMS b)
    {
        const TimeDurMS half = (TimeDurMS) (((TimeDurMS)(TimeDurMS)-1)/(TimeDurMS)2);
        return b != a && b - a > half;
    }
        
    /// a < b?
    inline bool less(TimeDurMS a, TimeDurMS b)
    {
        const TimeDurMS half = ((TimeDurMS)(TimeDurMS)-1)/(TimeDurMS)2;
        return b != a && b - a < half;
    }
}

#if (N_PLAT == N_PLAT_WIN32 || N_PLAT_WINRT == N_PLAT_WINRT || N_PLAT_WINRT == N_PLAT_WIN8_PHONE)
#include "NiiWin32TimerImp.h"
#elif(N_PLAT == N_PLAT_ANDROID)
#include "NiiAndroidTimerImp.h"
#elif(N_PLAT == N_PLAT_LINUX_X)
#include "NiiGLXTimerImp.h"
#elif(N_PLAT == N_PLAT_OSX)
#include "NiiOSXTimerImp.h"
#elif(N_PLAT == N_PLAT_IOS)
#include "NiiIOSTimerImp.h"
#endif

#endif