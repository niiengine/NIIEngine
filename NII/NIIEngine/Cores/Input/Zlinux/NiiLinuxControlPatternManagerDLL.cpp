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
#include "NiiLinuxControlPatternManagerPlugin.h"

#ifndef NII_STATIC_LIB
namespace NII 
{
    //-----------------------------------------------------------------------
    static LinuxControlPatternManagerPlugin * nii_linux_controlpattern_plugin;
    //-----------------------------------------------------------------------
    N_DMBFuncSign(_EngineAPI)
    {
        nii_linux_controlpattern_plugin = new LinuxControlPatternManagerPlugin();
        N_Engine().install(nii_linux_controlpattern_plugin);
    }
    //-----------------------------------------------------------------------
    N_DMEFuncSign(_EngineAPI)
    {
        N_Engine().uninstall(nii_linux_controlpattern_plugin);
        delete nii_linux_controlpattern_plugin;
    }
    //-----------------------------------------------------------------------     
}
#endif