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

#ifndef _UILAYOUTBUILD_CHECKS_H_
#define _UILAYOUTBUILD_CHECKS_H_

#include "Config.h"

// Check if current VS version is supported
// NOTE:  The non-existence of _MSC_VER is a clear indication Visual Studio is not being 
// used. Therefore, we can safely compare the version directly, since for other compilers
// this will be ignored
#if defined(_MSC_VER) && _MSC_VER < VISUAL_STUDIO_7_1 
    #if _MSC_VER == VISUAL_STUDIO_7_0
        COMPILE_WARN("VC++7.0 (VS2002) contains some serious bugs. It is highly recommended users upgrade to VS2003 or VS2005!")
    #else
        COMPILE_WARN("VC++6 (or lower) support has been discontinued. You might run into unexpected problems. An upgrade to VS2003 or VS2005 is highly recommended!")
    #endif // _MSC_VER == VISUAL_STUDIO_7_0
#endif // _MSC_VER < VISUAL_STUDIO_7_1 

//////////////////////////////////////////////////////////////////////////
// WXWIDGETS BUILD CHECKS
//////////////////////////////////////////////////////////////////////////

// Verify wxWidgets version
#if !wxCHECK_VERSION(2,8,0)
    COMPILE_WARN("LayoutEditor expects wxWidgets 2.8.* or higher. Lower versions of the library might conduct to errors!")
#endif // !wxCHECK_VERSION(2,8,0)

// Make sure OpenGL canvas is being used
#if !defined(wxUSE_GLCANVAS) || wxUSE_GLCANVAS != 1
    #error "LayoutEditor requires wxWidgets OpenGL support (wxUSE_GLCANVAS == 1). Check readme.txt for more info."
#endif // !defined(wxUSE_GLCANVAS) || wxUSE_GLCANVAS != 1

// Make sure exceptions are being used
#if !defined(wxUSE_EXCEPTIONS) || wxUSE_EXCEPTIONS != 1
    #error "LayoutEditor requires wxWidgets exception support (wxUSE_EXCEPTIONS == 1). Check readme.txt for more info."
#endif // !defined(wxUSE_EXCEPTIONS) || wxUSE_EXCEPTIONS != 1

// Make sure the about box is being used
#if !defined(wxUSE_ABOUTDLG) || wxUSE_ABOUTDLG != 1
#error "LayoutEditor requires wxWidgets about box support (wxUSE_ABOUTDLG == 1). Check readme.txt for more info."
#endif // !defined(wxUSE_ABOUTDLG) || wxUSE_ABOUTDLG != 1

#endif // _BUILD_CHECKS_H_
