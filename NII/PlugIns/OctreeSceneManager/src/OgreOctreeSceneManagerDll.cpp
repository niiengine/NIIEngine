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

#include <OgreRoot.h>
#include <OgreOctreePlugin.h>

#ifndef OGRE_STATIC_LIB

namespace Ogre
{
OctreePlugin* octreePlugin;

extern "C" void _OgreOctreePluginExport dllStartPlugin( void )
{
    // Create new scene manager
    octreePlugin = N_new OctreePlugin();

    // Register
    Root::getSingleton().installPlugin(octreePlugin);

}
extern "C" void _OgreOctreePluginExport dllStopPlugin( void )
{
	Root::getSingleton().uninstallPlugin(octreePlugin);
	N_delete octreePlugin;
}
}

#endif
