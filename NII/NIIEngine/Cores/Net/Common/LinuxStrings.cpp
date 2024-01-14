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

#if (defined(__GNUC__) || defined(__ARMCC_VERSION) || defined(__GCCXML__) || defined(__S3E__) ) && N_PLAT != N_PLAT_WIN32
#include <string.h>
#ifndef _stricmp
int _stricmp(const char * s1, const char * s2)
{
	return strcasecmp(s1,s2);
}
#endif
int _strnicmp(const char * s1, const char * s2, size_t n)
{
	return strncasecmp(s1,s2,n);
}

#ifndef _vsnprintf
#define _vsnprintf vsnprintf
#endif

#if (N_PLAT != N_PLAT_OSX) && (N_PLAT != N_PLAT_IOS)
char * _strlwr(char * str)
{
	if (str==0)
		return 0;
	for (int i=0; str[i]; i++)
	{
		if (str[i]>='A' && str[i]<='Z')
			str[i]+='a'-'A';
	}
	return str;
}
#endif

#endif