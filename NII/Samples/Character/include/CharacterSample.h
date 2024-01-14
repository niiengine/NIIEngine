#ifndef __Character_H__
#define __Character_H__

#include "SdkSample.h"
#include "SinbadCharacterController.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_Character : public SdkSample
{
public:
	Sample_Character()
	{
		mInfo["Title"] = "Character";
		mInfo["Description"] = "A demo showing 3rd-person character control and use of TagBones.";
		mInfo["Thumbnail"] = "thumb_char.png";
		mInfo["Category"] = "Animation";
		mInfo["Help"] = "Use the WASD keys to move Sinbad, and the space bar to jump. "
			"Use mouse to look around and mouse wheel to zoom. Press Q to take out or put back "
			"Sinbad's swords. With the swords equipped, you can left click to slice vertically or "
			"right click to slice horizontally. When the swords are not equipped, press E to "
			"start/stop a silly dance routine.";
	}

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		// let character update animations and camera
		mChara->addTime(evt->mCurrent);
		return SdkSample::onRenderCore(evt);
	}
	
	bool keyPressed(const OIS::KeyEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectKeyDown(evt);
		return SdkSample::keyPressed(evt);
	}
	
	bool keyReleased(const OIS::KeyEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectKeyUp(evt);
		return SdkSample::keyReleased(evt);
	}

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
	bool touchPressed(const OIS::MultiTouchEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt);
		return SdkSample::touchPressed(evt);
	}

	bool touchMoved(const OIS::MultiTouchEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
		return SdkSample::touchMoved(evt);
	}
#else
	bool mouseMoved(const OIS::MouseEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
		return SdkSample::mouseMoved(evt);
	}

	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt, id);
		return SdkSample::mousePressed(evt, id);
	}
#endif
protected:
	void setupContent()
	{   
		// set background and some fog
		mViewport->setBgColour(Colour(1.0f, 1.0f, 0.8f));
        
		mSceneMgr->setFog(ShaderChFog(Ogre::FM_LINEAR, Colour(1.0f, 1.0f, 0.8f), 0, 15, 100));

		// set shadow properties
		mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
		mSceneMgr->setShadowColour(Colour(0.5, 0.5, 0.5));
		mSceneMgr->getRenderPattern()->setTextureDim(1024);
		mSceneMgr->getRenderPattern()->setTextureCount(1);

		// disable default camera control so the character can do its own
		mCameraMan->setStyle(CS_MANUAL);

		// use a small amount of ambient lighting
		mSceneMgr->setAmbient(Colour(0.3, 0.3, 0.3));

		// add a bright light above the scene
		Light * light = mSceneMgr->createLight(0);
		light->setType(LT_POINT);
		light->setPos(-10, 40, 20);
		light->setSpecular(Colour::White);

		// create a floor mesh resource
		MeshManager::getSingleton().createPlane("floor", GroupDefault,
			Plane(Vector3::Y, 0), 100, 100, 10, 10, true, 1, 10, 10, Vector3::Z);

		// create a floor entity, give it a material, and place it at the origin
        GeometrySpace* floor = mSceneMgr->createGeo("Floor", "floor");
        floor->setMaterialName("Examples/Rockwall");
		floor->setCastShadow(false);
        mSceneMgr->getOriginNode()->attachObject(floor);

//		LogManager::getSingleton().logMessage("creating sinbad");
		// create our character controller
		mChara = new SinbadCharacterController(mCamera);

//		LogManager::getSingleton().logMessage("toggling stats");
		mTrayMgr->toggleAdvancedFrameStats();

//		LogManager::getSingleton().logMessage("creating panel");
		StringList items;
		items.push_back("Help");
		ParamsPanel* help = mTrayMgr->createParamsPanel(TL_TOPLEFT, "HelpMessage", 100, items);
		help->setParamValue("Help", "H / F1");
		
//		LogManager::getSingleton().logMessage("all done");
	}

	void cleanupContent()
	{
		// clean up character controller and the floor mesh
		if (mChara)
        {
            delete mChara;
            mChara = 0;
        }
		MeshManager::getSingleton().remove("floor");
	}

	SinbadCharacterController* mChara;
};
#endif