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

#ifndef _NII_Win32MinGWSupport_H_
#define _NII_Win32MinGWSupport_H_

// This header provides a compatibility layer for some MSVC symbols
// for MinGW. They may be needed when using MSVC libraries, most
// notably the DirectX SDK.

#ifdef __MINGW32__
    #include <stdint.h>

    // define a number of symbols MSVC uses for annotation.
    #include <specstrings.h>
    #define __in_z
    #define __in_z_opt
    #define UINT8 uint8_t
    #define WINAPI_INLINE inline
    #define __uuidof(Object) IID_##Object

    #endif
#endif