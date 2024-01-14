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

#ifndef __NII_NACL_TIMER_H__
#define __NII_NACL_TIMER_H__

#include "NiiPreInclude.h"

#if N_PLAT == N_PLAT_NACL
    #include <sys/time.h>
#endif

namespace NII
{
	/** Timer class */
	class _OgreExport Timer : public TimerAlloc
	{
	public:
		Timer();
		~Timer();

		/** Method for setting a specific option of the Timer. These options are usually
            specific for a certain implementation of the Timer class, and may (and probably
            will) not exist across different implementations.  reset() must be called after
			all setOption() calls.
            @param
                strKey The name of the option to set
            @param
                pValue A pointer to the value - the size should be calculated by the timer
                based on the key
            @return
                On success, true is returned.
            @par
                On failure, false is returned.
        */
        bool setOption( const String& strKey, const void* pValue )
        { (void)strKey; (void)pValue; return false; }

		/** Resets timer */
		void reset();

		/** Returns milliseconds since initialisation or last reset */
		Nul getMS();

		/** Returns microseconds since initialisation or last reset */
		Nul getUS();

		/** Returns milliseconds since initialisation or last reset, only CPU time measured */	
		Nul getCostMS();
	private:
		struct timeval start;
		clock_t zeroClock;        
	};
}
#endif
