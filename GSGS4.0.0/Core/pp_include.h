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

#ifndef PP_INCLUDE_H
#define PP_INCLUDE_H

#include <vector>
#include <wx/string.h>

#ifndef _gsgsAPI

#ifdef WXMAKINGDLL_CL
#    define _gsgsAPI __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define _gsgsAPI __declspec(dllimport)
#else // not making nor using DLL
#    define _gsgsAPI
#endif

#endif

/**
 * @brief the main interface to the PP API.
 * @param filePath
 * @return 
 */
extern _gsgsAPI int PPScan      ( const wxString &filePath, bool forCC );
/**
 * @brief scan input string
 */
extern _gsgsAPI int PPScanString( const wxString &inputString );

#endif
