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
#include "NiiGLPlugin.h"

#ifndef NIIEngine_Static

namespace NII 
{
    //-----------------------------------------------------------------------
	static GLPlugin * nii_glrendersystem_plugin;
    //-----------------------------------------------------------------------
    N_DMBFuncSign(_PluginAPI)
    {
		nii_glrendersystem_plugin = N_new GLPlugin();
		Engine::getOnly().install(nii_glrendersystem_plugin);
    }
    //-----------------------------------------------------------------------
    N_DMEFuncSign(_PluginAPI)
    {
		Engine::getOnly().uninstall(nii_glrendersystem_plugin);
		N_delete nii_glrendersystem_plugin;
    }
    //-----------------------------------------------------------------------    
}
#endif