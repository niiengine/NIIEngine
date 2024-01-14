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

#include "EpochTimeToString.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
// localtime
#include <time.h>
#include "LinuxStrings.h"

char * EpochTimeToString(long long time)
{
	static int textIndex=0;
	static char text[4][64];

	if (++textIndex==4)
		textIndex=0;

	struct tm * timeinfo;
	time_t t = time;
	timeinfo = localtime ( &t );
	strftime (text[textIndex], 64, "%c.", timeinfo);

	/*
	time_t
	// Copied from the docs
	struct tm *newtime;
	newtime = _localtime64(& time);
	asctime_s( text[textIndex], sizeof(text[textIndex]), newtime );

	while (text[textIndex][0] && (text[textIndex][strlen(text[textIndex])-1]=='\n' || text[textIndex][strlen(text[textIndex])-1]=='\r'))
		text[textIndex][strlen(text[textIndex])-1]=0;
		*/

	return text[textIndex];
}
