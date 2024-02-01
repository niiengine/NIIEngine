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
#include "OgreD3D11Plugin.h"

#ifndef OGRE_STATIC_LIB
namespace Ogre 
{
    D3D11Plugin* plugin;

    extern "C" void _OgreD3D11Export dllStartPlugin(void) throw()
    {
        plugin = new D3D11Plugin();
        Root::getSingleton().installPlugin(plugin);
    }

    extern "C" void _OgreD3D11Export dllStopPlugin(void)
    {
        Root::getSingleton().uninstallPlugin(plugin);
        delete plugin;
    }
}
#endif