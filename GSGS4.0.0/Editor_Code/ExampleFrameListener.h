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
/*
-----------------------------------------------------------------------------
Filename:    ExampleFrameListener.h
Description: Defines an example frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             F:        Toggle frame rate stats on/off
			 R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
             P:        Toggle on/off display of camera position / orientation
			 F2:	   Set the main viewport material scheme to default material manager scheme.
			 F3:	   Set the main viewport material scheme to shader generator default scheme.
			 F4:	   Toggle default shader generator lighting model from per vertex to per pixel.
-----------------------------------------------------------------------------
*/

#ifndef __ExampleFrameListener_H__
#define __ExampleFrameListener_H__

#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"


//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

using namespace Ogre;

#ifdef USE_RTSHADER_SYSTEM
#include "OgreRTShaderSystem.h"
#endif

class ExampleFrameListener: public RenderFrameListener, public WindowEventListener
{
protected:
	virtual void updateStats()
	{
		static String currFps = "Current FPS: ";
		static String avgFps = "Average FPS: ";
		static String bestFps = "Best FPS: ";
		static String worstFps = "Worst FPS: ";
		static String tris = "Triangle Count: ";
		static String batches = "Batch Count: ";

		// update stats when necessary
		try
        {
			OverlayGeo * guiAvg = OverlayManager::getSingleton().getGeo("Core/AverageFps");
			OverlayGeo * guiCurr = OverlayManager::getSingleton().getGeo("Core/CurrFps");
			OverlayGeo * guiBest = OverlayManager::getSingleton().getGeo("Core/BestFps");
			OverlayGeo * guiWorst = OverlayManager::getSingleton().getGeo("Core/WorstFps");

			const FramesPerSecond & stats = mWindow->getFPS();
			guiAvg->setCaption(avgFps + StringConverter::toString(stats.mAvg));
			guiCurr->setCaption(currFps + StringConverter::toString(stats.mLast));
			guiBest->setCaption(bestFps + StringConverter::toString(stats.mBest)
				+" "+StringConverter::toString(stats.mBestFrame)+" ms");
			guiWorst->setCaption(worstFps + StringConverter::toString(stats.mWorst)
				+" "+StringConverter::toString(stats.mWorstFrame)+" ms");

			OverlayGeo* guiTris = OverlayManager::getSingleton().getGeo("Core/NumTris");
			guiTris->setCaption(tris + StringConverter::toString(mWindow->mTriCount));

			OverlayGeo* guiBatches = OverlayManager::getSingleton().getGeo("Core/NumBatches");
			guiBatches->setCaption(batches + StringConverter::toString(mWindow->mBatchCount));

			OverlayGeo* guiDbg = OverlayManager::getSingleton().getGeo("Core/DebugText");
			guiDbg->setCaption(mDebugText);
		}
		catch(...) { /* ignore */ }
	}

public:
	// Constructor takes a ViewWindow because it uses that to determine input context
	ExampleFrameListener(ViewWindow* win, Camera* cam, bool bufferedKeys = false,
        bool bufferedMouse = false, bool bufferedJoy = false ) :
            mCamera(cam),
            mTranslateVector(Vector3::ZERO),
            mCurrentSpeed(0),
            mWindow(win),
            mStatsOn(true),
            mNumScreenShots(0),
            mMoveScale(0.0f),
            mRotScale(0.0f),
            mTimeUntilNextToggle(0),
            mFiltering(TFM_BILINEAR),
            mAniso(1),
            mSceneDetailIndex(0),
            mMoveSpeed(100),
            mRotateSpeed(36),
            mDebugOverlay(0),
            mInputManager(0),
            mMouse(0),
            mKeyboard(0),
            mJoy(0)
	{

		mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

		LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
		OIS::ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		win->getPlatformData("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(Npair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = OIS::InputManager::createInputSystem( pl );

		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, bufferedKeys ));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, bufferedMouse ));
		try {
			mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject( OIS::OISJoyStick, bufferedJoy ));
		}
		catch(...) {
			mJoy = 0;
		}

		//Set initial mouse clipping size
		windowResized(mWindow);

		showDebugOverlay(true);

		//Register as a Window listener
		WindowEventUtilities::addWindowEventListener(mWindow, this);
	}

#ifdef USE_RTSHADER_SYSTEM
	virtual void processShaderGeneratorInput()
	{
		// Switch to default scheme.
		if (mKeyboard->isKeyDown(OIS::KC_F2))
		{
			mCamera->getViewport()->setMaterial(0);
			mDebugText = "Active Viewport Scheme: ";
			mDebugText += 0;
		}

		// Switch to shader generator scheme.
		if (mKeyboard->isKeyDown(OIS::KC_F3))
		{
			mCamera->getViewport()->setMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
			mDebugText = "Active Viewport Scheme: ";
			mDebugText += RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME;
		}

		// Toggles per pixel per light model.
		if (mKeyboard->isKeyDown(OIS::KC_F4) && mTimeUntilNextToggle <= 0)
		{
			mTimeUntilNextToggle = 1.0;

			static bool userPerPixelLightModel = true;
			RTShader::ShaderGenerator* shaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();
			RTShader::RenderState* renderState = shaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

			// Remove all global sub render states.
			renderState->reset();

			// Add per pixel lighting sub render state to the global scheme render state.
			// It will override the default FFP lighting sub render state.
			if (userPerPixelLightModel)
			{
				RTShader::SubRenderState* perPixelLightModel = shaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
				renderState->addTemplateSubRenderState(perPixelLightModel);

				mDebugText = "Per pixel lighting model applied to shader generator default scheme";
			}
			else
			{
				mDebugText = "Per vertex lighting model applied to shader generator default scheme";
			}

			// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
			shaderGenerator->invalidateScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

			userPerPixelLightModel = !userPerPixelLightModel;
		}
	}
#endif

	//Adjust mouse clipping area
	virtual void windowResized(ViewWindow * rw)
	{
		unsigned int width, height, depth;
		int left, top;
		rw->getArea(left, top, width, height, depth);

		const OIS::MouseState & ms = mMouse->getMouseState();
		ms.width = width;
		ms.height = height;
	}

	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(ViewWindow * rw)
	{
		//Only close for window that created OIS (the main window in these demos)
		if( rw == mWindow )
		{
			if( mInputManager )
			{
				mInputManager->destroyInputObject( mMouse );
				mInputManager->destroyInputObject( mKeyboard );
				mInputManager->destroyInputObject( mJoy );

				OIS::InputManager::destroyInputSystem(mInputManager);
				mInputManager = 0;
			}
		}
	}

	virtual ~ExampleFrameListener()
	{
		//Remove ourself as a Window listener
		WindowEventUtilities::removeWindowEventListener(mWindow, this);
		windowClosed(mWindow);
	}

	virtual bool processUnbufferedKeyInput(const RenderFrameArgs& evt)
	{
		NIIf moveScale = mMoveScale;
		if(mKeyboard->isKeyDown(OIS::KC_LSHIFT))
			moveScale *= 10;

		if(mKeyboard->isKeyDown(OIS::KC_A))
			mTranslateVector.x = -moveScale;	// Move camera left

		if(mKeyboard->isKeyDown(OIS::KC_D))
			mTranslateVector.x = moveScale;	// Move camera RIGHT

		if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
			mTranslateVector.z = -moveScale;	// Move camera forward

		if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
			mTranslateVector.z = moveScale;	// Move camera backward

		if(mKeyboard->isKeyDown(OIS::KC_PGUP))
			mTranslateVector.y = moveScale;	// Move camera up

		if(mKeyboard->isKeyDown(OIS::KC_PGDOWN))
			mTranslateVector.y = -moveScale;	// Move camera down

		if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
			mCamera->yaw(-mRotScale);

		if(mKeyboard->isKeyDown(OIS::KC_LEFT))
			mCamera->yaw(mRotScale);

		if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
			return false;

       	if( mKeyboard->isKeyDown(OIS::KC_F) && mTimeUntilNextToggle <= 0 )
		{
			mStatsOn = !mStatsOn;
			showDebugOverlay(mStatsOn);
			mTimeUntilNextToggle = 1;
		}

		if( mKeyboard->isKeyDown(OIS::KC_T) && mTimeUntilNextToggle <= 0 )
		{
			switch(mFiltering)
			{
			case TFM_BILINEAR:
				mFiltering = TFM_TRILINEAR;
				mAniso = 1;
				break;
			case TFM_TRILINEAR:
				mFiltering = TFM_ANISOTROPIC;
				mAniso = 8;
				break;
			case TFM_ANISOTROPIC:
				mFiltering = TFM_BILINEAR;
				mAniso = 1;
				break;
			default: break;
			}
			N_Only(Material).setTextureFilter(mFiltering);
			N_Only(Material).setTextureAniso(mAniso);

			showDebugOverlay(mStatsOn);
			mTimeUntilNextToggle = 1;
		}

		if(mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0)
		{
			std::ostringstream ss;
			ss << "screenshot_" << ++mNumScreenShots << ".png";
			mWindow->screenshots(ss.str());
			mTimeUntilNextToggle = 0.5;
			mDebugText = "Saved: " + ss.str();
		}

		if(mKeyboard->isKeyDown(OIS::KC_R) && mTimeUntilNextToggle <=0)
		{
			mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
			switch(mSceneDetailIndex) 
            {
			case 0 : 
                mCamera->setPolygonMode(PM_SOLID); 
                break;
			case 1 : 
                mCamera->setPolygonMode(PM_WIREFRAME); 
                break;
			case 2 : 
                mCamera->setPolygonMode(PM_POINTS); 
                break;
			}
			mTimeUntilNextToggle = 0.5;
		}

		static bool displayCameraDetails = false;
		if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
		{
			displayCameraDetails = !displayCameraDetails;
			mTimeUntilNextToggle = 0.5;
			if(!displayCameraDetails)
				mDebugText = "";
		}

		// Print camera details
		if(displayCameraDetails)
			mDebugText = "P: " + StringConverter::toString(mCamera->getSpacePos()) +
                " " + "O: " + StringConverter::toString(mCamera->getSpaceOri());

		// Return true to continue rendering
		return true;
	}

	virtual bool processUnbufferedMouseInput(const RenderFrameArgs& evt)
	{

		// Rotation factors, may not be used if the second mouse button is pressed
		// 2nd mouse button - slide, otherwise rotate
		const OIS::MouseState &ms = mMouse->getMouseState();
		if( ms.buttonDown( OIS::MB_Right ) )
		{
			mTranslateVector.x += ms.X.rel * 0.13;
			mTranslateVector.y -= ms.Y.rel * 0.13;
		}
		else
		{
			mRotX = Angle(-ms.X.rel * 0.13);
			mRotY = Angle(-ms.Y.rel * 0.13);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            // Adjust the input depending upon viewport orientation
            Radian origRotY, origRotX;
            switch(mCamera->getViewport()->getOri())
            {
                case 270:
                    origRotY = mRotY;
                    origRotX = mRotX;
                    mRotX = origRotY;
                    mRotY = -origRotX;
                    break;
                case 90:
                    origRotY = mRotY;
                    origRotX = mRotX;
                    mRotX = -origRotY;
                    mRotY = origRotX;
                    break;
                // Portrait doesn't need any change
                case 0:
                default:
                    break;
            }
#endif
		}

		return true;
	}

	virtual void moveCamera()
	{
		// Make all the changes to the camera
		// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
		//(e.g. airplane)
		mCamera->yaw(mRotX);
		mCamera->pitch(mRotY);
		mCamera->moveRel(mTranslateVector);
	}

	virtual void showDebugOverlay(bool show)
	{
		if (mDebugOverlay)
		{
			if (show)
				mDebugOverlay->show();
			else
				mDebugOverlay->hide();
		}
	}

	// Override onRenderCore event to process that (don't care about onEndRender)
	bool onRenderCore(const RenderFrameArgs * evt)
	{
		if(mWindow->isClosed())
            return false;

		mSpeedLimit = mMoveScale * evt->mCurrent;

		//Need to capture/update each device
		mKeyboard->capture();
		mMouse->capture();
		if( mJoy ) mJoy->capture();

		bool buffJ = (mJoy) ? mJoy->buffered() : true;

    	Ogre::Vector3 lastMotion = mTranslateVector;

		//Check if one of the devices is not buffered
		if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
		{
			// one of the input modes is immediate, so setup what is needed for immediate movement
			if (mTimeUntilNextToggle >= 0)
				mTimeUntilNextToggle -= evt->mCurrent;

			// Move about 100 units per second
			mMoveScale = mMoveSpeed * evt->mCurrent;
			// Take about 10 seconds for full rotation
			mRotScale = mRotateSpeed * evt->mCurrent;

			mRotX = 0;
			mRotY = 0;
			mTranslateVector = Ogre::Vector3::ZERO;

		}

		//Check to see which device is not buffered, and handle it
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
		if( !mKeyboard->buffered() )
			if( processUnbufferedKeyInput(evt) == false )
				return false;

#ifdef USE_RTSHADER_SYSTEM
		processShaderGeneratorInput();
#endif

#endif
		if( !mMouse->buffered() )
			if( processUnbufferedMouseInput(evt) == false )
				return false;

		// ramp up / ramp down speed
    	if (mTranslateVector == Ogre::Vector3::ZERO)
		{
			// decay (one third speed)
			mCurrentSpeed -= evt->mCurrent * 0.3;
			mTranslateVector = lastMotion;
		}
		else
		{
			// ramp up
			mCurrentSpeed += evt->mCurrent;

		}
		// Limit motion speed
		if (mCurrentSpeed > 1.0)
			mCurrentSpeed = 1.0;
		if (mCurrentSpeed < 0.0)
			mCurrentSpeed = 0.0;

		mTranslateVector *= mCurrentSpeed;


		if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
			moveCamera();

		return true;
	}

	bool onEndRender(const RenderFrameArgs * evt)
	{
		updateStats();
		return true;
	}
protected:
	Camera * mCamera;

	Vector3 mTranslateVector;
	NIIf mCurrentSpeed;
	ViewWindow * mWindow;
	bool mStatsOn;

	String mDebugText;

	unsigned int mNumScreenShots;
	float mMoveScale;
	float mSpeedLimit;
	Angle mRotScale;
	// just to stop toggles flipping too fast
	NIIf mTimeUntilNextToggle ;
	Radian mRotX, mRotY;
	TextureFilterMode mFiltering;
	int mAniso;

	int mSceneDetailIndex ;
	NIIf mMoveSpeed;
	Angle mRotateSpeed;
	Overlay * mDebugOverlay;

	//OIS Input devices
	OIS::InputManager * mInputManager;
	OIS::Mouse * mMouse;
	OIS::JoyStick * mJoy;    
	OIS::Keyboard * mKeyboard;
};

#endif
