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
#include "OgreParticleFXPrerequisites.h"
#include "OgreRoot.h"
#include "OgreParticleFXPlugin.h"

#ifndef OGRE_STATIC_LIB

namespace Ogre {

	ParticleFXPlugin* plugin;
    //-----------------------------------------------------------------------
    extern "C" void _OgreParticleFXExport dllStartPlugin(void) throw()
    {
		plugin = N_new ParticleFXPlugin();
		Root::getSingleton().installPlugin(plugin);
    }

    //-----------------------------------------------------------------------
    extern "C" void _OgreParticleFXExport dllStopPlugin(void)
    {
		Root::getSingleton().uninstallPlugin(plugin);
		N_delete plugin;

    }


}

#endif
