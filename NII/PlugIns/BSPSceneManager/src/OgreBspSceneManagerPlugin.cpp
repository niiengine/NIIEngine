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

#include "OgreBspSceneManagerPlugin.h"
#include "OgreRoot.h"

namespace Ogre 
{
	const String sPluginName = "BSP Scene Manager";
	//---------------------------------------------------------------------
	BspSceneManagerPlugin::BspSceneManagerPlugin()
		:mBspFactory(0), mBspResourceManager(0)
	{

	}
	//---------------------------------------------------------------------
	const String& BspSceneManagerPlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void BspSceneManagerPlugin::install()
	{
		// Create new scene manager
		mBspFactory = N_new BspSceneManagerFactory();

	}
	//---------------------------------------------------------------------
	void BspSceneManagerPlugin::init()
	{
		// Register (factory not dependent on rsys resources)
		Root::getSingleton().addSceneManagerFactory(mBspFactory);
		// Create resource manager (registers itself)
		mBspResourceManager = N_new BspResourceManager();
	}
	//---------------------------------------------------------------------
	void BspSceneManagerPlugin::shutdown()
	{
		// Unregister SM factory
		Root::getSingleton().removeSceneManagerFactory(mBspFactory);

		// delete resource manager, will unregister itself
		N_delete mBspResourceManager;
		mBspResourceManager = 0;

	}
	//---------------------------------------------------------------------
	void BspSceneManagerPlugin::uninstall()
	{

		N_delete mBspFactory;
		mBspFactory = 0;

	}


}
