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
#include "IntervalTimer.h"

void IntervalTimer::SetPeriod(TimeDurMS period) {basePeriod=period; remaining=0;}
bool IntervalTimer::UpdateInterval(TimeDurMS elapsed)
{
	if (elapsed >= remaining)
	{
		TimeDurMS difference = elapsed-remaining;
		if (difference >= basePeriod)
		{
			remaining=basePeriod;
		}
		else
		{
			remaining=basePeriod-difference;
		}

		return true;
	}

	remaining-=elapsed;
	return false;
}