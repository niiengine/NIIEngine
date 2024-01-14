#ifndef __BSP_H__
#define __BSP_H__

#include "SdkSample.h"
#include "FileSystemLayer.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#include "macUtils.h"
#endif

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_BSP : public SdkSample
{
public:
	Sample_BSP()
	{
		mInfo["Title"] = "BSP";
		mInfo["Description"] = "A demo of the indoor, or BSP (Binary Space Partition) scene manager. "
			"Also demonstrates how to load BSP maps from Quake 3.";
		mInfo["Thumbnail"] = "thumb_bsp.png";
		mInfo["Category"] = "Geometry";
	}

	StringList getRequiredPlugins()
	{
		StringList names;
		names.push_back("BSP Scene Manager");
		return names;
	}
protected:
	void locateResources()
	{
		// load the Quake archive location and map name from a config file
		ConfigFile cf;
		cf.load(mFSLayer->getConfigFilePath("quakemap.cfg"));
		mArchive = cf.getSetting("Archive");
		mMap = cf.getSetting("Map");

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        // OS X does not set the working directory relative to the app,
        // In order to make things portable on OS X we need to provide
        // the loading with it's own bundle path location
        if (!Ogre::StrUtil::beginsWith(mArchive, "/", false)) // only adjust relative dirs
            mArchive = Ogre::String(Ogre::macBundlePath() + "/" + mArchive);
#endif
		// add the Quake archive to the world resource group
		ResourceSchemeManager::getSingleton().add(mArchive, "Zip",
			N_Only(ResourceScheme).getEngineGroup(), true);
	}

	void createSceneManager()
	{
		mSceneMgr = mRoot->createSpaceManager("BspSceneManager");   // the BSP scene manager is required for this sample
	}

	void loadResources()
	{
		/* NOTE: The browser initialises everything at the beginning already, so we use a 0 init proportion.
		If you're not compiling this sample for use with the browser, then leave the init proportion at 0.7. */
		mTrayMgr->showLoadingBar(1, 1, 0);

		// associate the world geometry with the world resource group, and then load the group
		ResourceSchemeManager & rgm = ResourceSchemeManager::getSingleton();
		mSceneMgr->setSceneGeo(mMap);
        rgm.setup(rgm.getEngineGroup());
		rgm.loadResourceGroup(rgm.getEngineGroup(), false);

		mTrayMgr->hideLoadingBar();
	}

	void unloadResources()
	{
		// unload the map so we don't interfere with subsequent samples
		ResourceSchemeManager & rgm = ResourceSchemeManager::getSingleton();
		rgm.unloadResourceGroup(rgm.getEngineGroup());
		rgm.remove(mArchive, ResourceSchemeManager::getSingleton().getEngineGroup());
	}

	void setupView()
	{
		SdkSample::setupView();

		// modify camera for close work
		mCamera->setNearClipDistance(4);
		mCamera->setFarClipDistance(4000);

		// set a random player starting point
		ViewPoint vp = static_cast<BspSceneManager *>(mSceneMgr)->getSuggestedViewpoint(true);

		// Quake uses the Z axis as the up axis, so make necessary adjustments
		mCamera->setCustomYaw(true, Vector3::Z);
		mCamera->pitch(Angle(90));

		mCamera->setPos(vp.position);
		mCamera->rotate(vp.orientation);

		mCameraMan->setTopSpeed(350);   // make the camera move a bit faster
	}

	String mArchive;
	String mMap;
};

#endif