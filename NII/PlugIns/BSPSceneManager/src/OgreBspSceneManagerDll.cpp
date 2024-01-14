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
#include "OgreBspSceneManagerPlugin.h"

Ogre::BspSceneManagerPlugin* bspPlugin;
//-----------------------------------------------------------------------
#ifndef OGRE_STATIC_LIB
extern "C" void _OgreBspPluginExport dllStartPlugin(void)
{
    // Create new scene manager
    bspPlugin = N_new Ogre::BspSceneManagerPlugin();

    // Register
    Ogre::Root::getSingleton().installPlugin(bspPlugin);


}
extern "C" void _OgreBspPluginExport dllStopPlugin(void)
{
	Ogre::Root::getSingleton().uninstallPlugin(bspPlugin);
    N_delete bspPlugin;
}
#endif
