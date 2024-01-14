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

#include "NiiEngine.h"
#include "NiiWin32ControlPatternPlugin.h"

#ifndef NII_STATIC_LIB
namespace NII 
{
    //-----------------------------------------------------------------------
    static Win32ControlPatternManagerPlugin * nii_win32_controlpattern_plugin;
    //-----------------------------------------------------------------------
    N_DMBFuncSign(_EngineAPI)
    {
        nii_win32_controlpattern_plugin = new Win32ControlPatternManagerPlugin();
        N_Engine().install(nii_win32_controlpattern_plugin);
    }
    //-----------------------------------------------------------------------
    N_DMEFuncSign(_EngineAPI)
    {
        N_Engine().uninstall(nii_win32_controlpattern_plugin);
        delete nii_win32_controlpattern_plugin;
    }
    //-----------------------------------------------------------------------     
}
#endif