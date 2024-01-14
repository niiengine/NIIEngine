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

#include "OgreRoot.h"
#include "OgreGLES2Prerequisites.h"
#include "OgreGLES2Plugin.h"

namespace Ogre {
    static GLES2Plugin* plugin;
    extern "C" void _OgreGLES2Export dllStartPlugin(void);
    extern "C" void _OgreGLES2Export dllStopPlugin(void);

    extern "C" void _OgreGLES2Export dllStartPlugin(void)
    {
        plugin = N_new GLES2Plugin();
        Root::getSingleton().installPlugin(plugin);
    }

    extern "C" void _OgreGLES2Export dllStopPlugin(void)
    {
        Root::getSingleton().uninstallPlugin(plugin);
        N_delete plugin;
    }
}
