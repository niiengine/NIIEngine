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

#include <stdint.h>

// Define some symbols referred to by MSVC libs. Not having these symbols
// may cause linking errors when linking against the DX SDK.

extern "C" {
    // MSVC libs use _chkstk for stack-probing. MinGW equivalent is _alloca.
    void _alloca();
    void _chkstk()
    {
        _alloca();
    }
    
    // MSVC uses security cookies to prevent some buffer overflow attacks.
    // provide dummy implementations.
    intptr_t __security_cookie;
    
    void _fastcall __security_check_cookie(intptr_t i)
    {
    }
}
