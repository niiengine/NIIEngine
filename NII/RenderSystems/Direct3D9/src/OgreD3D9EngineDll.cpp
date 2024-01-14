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
#include "OgreD3D9Plugin.h"

#ifndef OGRE_STATIC_LIB

namespace Ogre 
{
    D3D9Plugin* plugin;

    extern "C" void _OgreD3D9Export dllStartPlugin(void) throw()
    {
        plugin = N_new D3D9Plugin();
        Root::getSingleton().installPlugin(plugin);
    }

    extern "C" void _OgreD3D9Export dllStopPlugin(void)
    {
        Root::getSingleton().uninstallPlugin(plugin);
        N_delete plugin;
    }
}
#endif