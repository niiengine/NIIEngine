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

#ifndef _CEGUICommonDialogsModule_h_
#define _CEGUICommonDialogsModule_h_

#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(CEGUI_STATIC)
    #ifdef CEGUICOMMONDIALOGS_EXPORTS
        #define CEGUI_COMMONDIALOGS_API __declspec(dllexport)
    #else
        #define CEGUI_COMMONDIALOGS_API __declspec(dllimport)
    #endif
#else
    #define CEGUI_COMMONDIALOGS_API
#endif

extern "C"
/**
\brief
Initialise common dialogs library ready for use.

This should be called prior to creating any common dialog windows.
*/
CEGUI_COMMONDIALOGS_API void initialiseCEGUICommonDialogs();

#endif

