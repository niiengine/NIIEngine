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

#ifndef _GCC_WIN_STRINGS
#define _GCC_WIN_STRINGS

#if N_PLAT == N_PLAT_NACL
	#ifndef _stricmp
		int _stricmp(const char * s1, const char * s2);
	#endif
	int _strnicmp(const char * s1, const char * s2, size_t n);
	char *_strlwr(char * str);
	#define _vsnprintf vsnprintf
#else
	#if (defined(__GNUC__)  || defined(__GCCXML__) || defined(__S3E__) ) && N_PLAT != N_PLAT_WIN32
		#ifndef _stricmp
			int _stricmp(const char* s1, const char* s2);
		#endif
		int _strnicmp(const char* s1, const char* s2, size_t n);
        #define _vsnprintf vsnprintf
		#if (N_PLAT != N_PLAT_OSX) && (N_PLAT != N_PLAT_IOS)
			char *_strlwr(char * str); //this won't compile on OSX for some reason
		#endif
	#endif
#endif
#endif