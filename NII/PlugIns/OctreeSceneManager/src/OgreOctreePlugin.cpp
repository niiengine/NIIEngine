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

#include "OgreOctreePlugin.h"
#include "OgreRoot.h"

namespace Ogre 
{
	const String sPluginName = "Octree Scene Manager";
	//---------------------------------------------------------------------
	OctreePlugin::OctreePlugin()
		:mOctreeSMFactory(0)
	{

	}
	//---------------------------------------------------------------------
	const String& OctreePlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void OctreePlugin::install()
	{
		// Create objects
		mOctreeSMFactory = N_new OctreeSceneManagerFactory();

	}
	//---------------------------------------------------------------------
	void OctreePlugin::init()
	{
		// Register
		Root::getSingleton().addSceneManagerFactory(mOctreeSMFactory);
	}
	//---------------------------------------------------------------------
	void OctreePlugin::shutdown()
	{
		// Unregister
		Root::getSingleton().removeSceneManagerFactory(mOctreeSMFactory);
	}
	//---------------------------------------------------------------------
	void OctreePlugin::uninstall()
	{
		// destroy 
		N_delete mOctreeSMFactory;
		mOctreeSMFactory = 0;


	}


}
