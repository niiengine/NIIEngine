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

#include "OgreTimer.h"
#include <sys/time.h>

namespace NII 
{

    //------------------------------------------------------------------------
    Timer::Timer()
    {
        reset();
    }

    //------------------------------------------------------------------------
    Timer::~Timer()
    {
    }

    //------------------------------------------------------------------------
    void Timer::reset()
    {
        zeroClock = clock();
        gettimeofday(&start, NULL);
    }

    //------------------------------------------------------------------------
    Nul Timer::getMS()
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        return (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec-start.tv_usec) / 1000;
    }

    //------------------------------------------------------------------------
    Nul Timer::getUS()
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        return (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);
    }

    //-- Common Across All Timers --------------------------------------------
    Nul Timer::getCostMS()
    {
        clock_t newClock = clock();
        return (Nul)(newClock-zeroClock);
    }
    //------------------------------------------------------------------------
}
