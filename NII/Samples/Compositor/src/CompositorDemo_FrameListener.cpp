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

#include <Ogre.h>
#include <OgreTimer.h>
#include <CEGUI/CEGUIPropertyHelper.h>

#include "CompositorDemo_FrameListener.h"
#include "Compositor.h"

inline Ogre::String operator +(const Ogre::String & l,const CEGUI::String & o)
{
	return l + o.c_str();
}
/*
inline CEGUI::String operator +(const CEGUI::String& l,const Ogre::String& o)
{
	return l+o.c_str();
}
*/

/*************************************************************************
	CompositorDemo_FrameListener methods that handle all input for this Compositor demo.
*************************************************************************/

    CompositorDemo_FrameListener::CompositorDemo_FrameListener(CompositorDemo* main)
        : mMain(main)
        , mTranslateVector(Ogre::Vector3::ZERO)
        , mStatsOn(true)
        , mNumScreenShots(0)
        , mWriteToFile(false)
        , mSkipCount(0)
        , mUpdateFreq(50)
        , mSceneDetailIndex(0)
        , mFiltering(Ogre::TFM_BILINEAR)
        , mAniso(1)
        , mQuit(false)
        , mMoveScale(0.0f)
        , mRotScale(0.0f)
        , mSpeed(MINSPEED)
        , mAvgFrameTime(0.1)
        , mMoveSpeed(100)
        , mRotateSpeed(0)
        , mLastMousePositionSet(false)
        , mTimeUntilNextToggle(0)
        , mRotX(0)
        , mRotY(0)
        , mProcessMovement(false)
        , mUpdateMovement(false)

        , mLMBDown(false)
        , mRMBDown(false)
        , mMoveFwd(false)
        , mMoveBck(false)
        , mMoveLeft(false)
        , mMoveRight(false)
		, mSpinny(0)
        , mCompositorSelectorViewManager(0)
		, mMouse(0)
		, mKeyboard(0)

    {
        Ogre::Root::getSingleton().addFrameListener(this);

        // using buffered input
		OIS::ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		mMain->getRenderWindow()->getPlatformData("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(Npair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = OIS::InputManager::createInputSystem( pl );

		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

		unsigned int width, height, depth;
		int left, top;
		mMain->getRenderWindow()->getArea(left, top, width, height, depth);

		//Set Mouse Region.. if window resizes, we should alter this to reflect as well
		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = width;
		ms.height = height;

		mMouse->setEventCallback(this);
		mKeyboard->setEventCallback(this);

        mGuiRenderer = CEGUI::System::getSingleton().getRenderer();

        mGuiAvg   = CEGUI::WindowManager::getSingleton().getWindow("OPAverageFPS");
        mGuiCurr  = CEGUI::WindowManager::getSingleton().getWindow("OPCurrentFPS");
        mGuiBest  = CEGUI::WindowManager::getSingleton().getWindow("OPBestFPS");
        mGuiWorst = CEGUI::WindowManager::getSingleton().getWindow("OPWorstFPS");
        mGuiTris  = CEGUI::WindowManager::getSingleton().getWindow("OPTriCount");
        mGuiDbg   = CEGUI::WindowManager::getSingleton().getWindow("OPDebugMsg");
        mRoot	  = CEGUI::WindowManager::getSingleton().getWindow("root");

        registerCompositors();
		initDebugRTTWindow();
        connectEventHandlers();
    }
//--------------------------------------------------------------------------
    CompositorDemo_FrameListener::~CompositorDemo_FrameListener()
    {
		if(mInputManager)
		{
			mInputManager->destroyInputObject(mKeyboard);
			mInputManager->destroyInputObject(mMouse);
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}

        delete mCompositorSelectorViewManager;
    }
//--------------------------------------------------------------------------
    void CompositorDemo_FrameListener::connectEventHandlers(void)
    {
        CEGUI::Window* wndw = CEGUI::WindowManager::getSingleton().getWindow("root");

        wndw->subscribeEvent(CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&CompositorDemo_FrameListener::handleMouseMove, this));

        wndw->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&CompositorDemo_FrameListener::handleMouseButtonUp, this));

        wndw->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&CompositorDemo_FrameListener::handleMouseButtonDown, this));

        wndw->subscribeEvent(CEGUI::Window::EventMouseWheel, CEGUI::Event::Subscriber(&CompositorDemo_FrameListener::handleMouseWheelEvent, this));
        wndw->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CompositorDemo_FrameListener::handleKeyDownEvent, this ));
        wndw->subscribeEvent(CEGUI::Window::EventKeyUp, CEGUI::Event::Subscriber(&CompositorDemo_FrameListener::handleKeyUpEvent, this ));
    }
//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::onRenderCore(const Ogre::RenderFrameArgs * evt)
    {
	mMouse->capture();
	mKeyboard->capture();

	if( mMain->getRenderWindow()->isClosed() )
		return false;

        if (mQuit)
            return false;
        else
        {
            mSkipCount++;
            if (mSkipCount >= mUpdateFreq)
            {
                mSkipCount = 0;
                updateStats();
            }
            // update movement process
            if(mProcessMovement || mUpdateMovement)
            {
                mTranslateVector.x += mMoveLeft ? mAvgFrameTime * -MOVESPEED : 0;
                mTranslateVector.x += mMoveRight ? mAvgFrameTime * MOVESPEED : 0;
                mTranslateVector.z += mMoveFwd ? mAvgFrameTime * -MOVESPEED : 0;
                mTranslateVector.z += mMoveBck ? mAvgFrameTime * MOVESPEED : 0;

                mMain->getCamera()->yaw(Ogre::Angle(mRotX).getRadian());
                mMain->getCamera()->pitch(Ogre::Angle(mRotY).getRadian());
                mMain->getCamera()->moveRel(mTranslateVector);

                mUpdateMovement = false;
                mRotX = 0;
                mRotY = 0;
                mTranslateVector = Ogre::Vector3::ZERO;
            }

            if(mWriteToFile)
            {
                mMain->getRenderWindow()->screenshots("frame_" +
                    Ogre::StrConv::conv(++mNumScreenShots) + ".png");
            }

			if(mSpinny)
				mSpinny->yaw(Ogre::Angle(10 * evt->mCurrent));

            return true;
        }
    }
//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::mouseMoved (const OIS::MouseEvent &e)
    {
        CEGUI::System::getSingleton().injectMouseMove( e.state.X.rel, e.state.Y.rel );
		CEGUI::System::getSingleton().injectMouseWheelChange(e.state.Z.rel);
		return true;
    }

//--------------------------------------------------------------------------
	bool CompositorDemo_FrameListener::keyPressed (const OIS::KeyEvent &e)
    {
        // give 'quitting' priority
		if (e.key == OIS::KC_ESCAPE)
        {
            mQuit = true;
            return false;
        }

		if (e.key == OIS::KC_V)
			mMain->getRenderWindow()->getViewport(0)->setBgColour(Ogre::Colour(0,1,0));

        if (e.key == OIS::KC_SYSRQ )
        {
			Ogre::StringStream ss;
            ss << "screenshot_" << ++mNumScreenShots << ".png";
            mMain->getRenderWindow()->screenshots(ss.str());
            mDebugText = "Saved: " + ss.str();
			//mTimeUntilNextToggle = 0.5;
        }

        // do event injection
        CEGUI::System& cegui = CEGUI::System::getSingleton();
        cegui.injectKeyDown(e.key);
		cegui.injectChar(e.text);
		return true;
    }

//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::keyReleased (const OIS::KeyEvent &e)
    {
        CEGUI::System::getSingleton().injectKeyUp(e.key);
		return true;
    }

//--------------------------------------------------------------------------
	bool CompositorDemo_FrameListener::mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id)
    {
        CEGUI::System::getSingleton().injectMouseButtonDown(convertOISButtonToCegui(id));
		return true;
	}

//--------------------------------------------------------------------------
	bool CompositorDemo_FrameListener::mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id)
    {
        CEGUI::System::getSingleton().injectMouseButtonUp(convertOISButtonToCegui(id));
		return true;
    }

//--------------------------------------------------------------------------
    CEGUI::MouseButton CompositorDemo_FrameListener::convertOISButtonToCegui(int ois_button_id)
    {
        switch (ois_button_id)
		{
		case 0: return CEGUI::LeftButton;
		case 1: return CEGUI::RightButton;
		case 2:	return CEGUI::MiddleButton;
		case 3: return CEGUI::X1Button;
		default: return CEGUI::LeftButton;
		}
    }
//--------------------------------------------------------------------------
    void CompositorDemo_FrameListener::updateStats(void)
    {
        static CEGUI::String currFps = "Current FPS: ";
        static CEGUI::String avgFps = "Average FPS: ";
        static CEGUI::String bestFps = "Best FPS: ";
        static CEGUI::String worstFps = "Worst FPS: ";
        static CEGUI::String tris = "Triangle Count: ";

        const NII::FramesPerSecond & stats = mMain->getRenderWindow()->getFPS();

        mGuiAvg->setText(avgFps + Ogre::StrConv::conv(stats.mAvg));
        mGuiCurr->setText(currFps + Ogre::StrConv::conv(stats.mLast));
        mGuiBest->setText(bestFps + Ogre::StrConv::conv(stats.mBest)
            + " " + Ogre::StrConv::conv(stats.mBestFrame)+" ms");
        mGuiWorst->setText(worstFps + Ogre::StrConv::conv(stats.mWorst)
            + " " + Ogre::StrConv::conv(stats.mWorstFrame)+" ms");

        mGuiTris->setText(tris + Ogre::StrConv::conv(mMain->getRenderWindow()->mTriCount));
        mGuiDbg->setText(mDebugText.c_str());
        mAvgFrameTime = 1.0f/(stats.mAvg + 1.0f);
        if(mAvgFrameTime > 0.1f)
            mAvgFrameTime = 0.1f;
    }
//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::handleMouseMove(const CEGUI::EventArgs& e)
    {
        using namespace CEGUI;

        if( mLMBDown && !mRMBDown)
        {
            // rotate camera
            mRotX += -((const MouseEventArgs&)e).moveDelta.d_x * mAvgFrameTime * 10.0;
            mRotY += -((const MouseEventArgs&)e).moveDelta.d_y * mAvgFrameTime * 10.0;
            MouseCursor::getSingleton().setPos(mLastMousePosition);
            mUpdateMovement = true;
        }
        else
        {
            if( mRMBDown && !mLMBDown)
            {
                // translate camera
                mTranslateVector.x += ((const MouseEventArgs&)e).moveDelta.d_x * mAvgFrameTime * MOVESPEED;
                mTranslateVector.y += -((const MouseEventArgs&)e).moveDelta.d_y * mAvgFrameTime * MOVESPEED;
                //mTranslateVector.z = 0;
                MouseCursor::getSingleton().setPos( mLastMousePosition );
                mUpdateMovement = true;
            }
            else
            {
                if( mRMBDown && mLMBDown)
                {
                    mTranslateVector.z += (((const MouseEventArgs&)e).moveDelta.d_x + ((const MouseEventArgs&)e).moveDelta.d_y) * mAvgFrameTime * MOVESPEED;
                    MouseCursor::getSingleton().setPos( mLastMousePosition );
                    mUpdateMovement = true;
                }

            }
        }

        return true;
    }

//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::handleMouseButtonUp(const CEGUI::EventArgs& e)
    {
        using namespace CEGUI;

        //Window* wndw = ((const WindowEventArgs&)e).window;
        if( ((const MouseEventArgs&)e).button == LeftButton )
        {
            mLMBDown = false;
        }

        if( ((const MouseEventArgs&)e).button == RightButton )
        {
            mRMBDown = false;
        }
        if( !mLMBDown && !mRMBDown )
        {
            MouseCursor::getSingleton().show();
            if(mLastMousePositionSet)
            {
                MouseCursor::getSingleton().setPos( mLastMousePosition );
                mLastMousePositionSet = false;
            }
            mRoot->releaseInput();
        }

        return true;
    }

//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::handleMouseButtonDown(const CEGUI::EventArgs& e)
    {
        using namespace CEGUI;

        //Window* wndw = ((const WindowEventArgs&)e).window;
        if( ((const MouseEventArgs&)e).button == LeftButton )
        {
            mLMBDown = true;
        }

        if( ((const MouseEventArgs&)e).button == RightButton )
        {
            mRMBDown = true;
        }

        if( mLMBDown || mRMBDown )
        {
            MouseCursor::getSingleton().hide();
            if (!mLastMousePositionSet)
            {
                mLastMousePosition = MouseCursor::getSingleton().getPos();
                mLastMousePositionSet = true;
            }
            mRoot->captureInput();
        }

        return true;
    }

//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::handleMouseWheelEvent(const CEGUI::EventArgs& e)
    {
        using namespace CEGUI;
        mTranslateVector.z += ((const MouseEventArgs&)e).wheelChange * -5.0;
        mUpdateMovement = true;

        return true;
    }

//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::handleKeyDownEvent(const CEGUI::EventArgs& e)
    {
        using namespace CEGUI;

        CheckMovementKeys( ((const KeyEventArgs&)e).scancode , true);

        return true;
    }

//--------------------------------------------------------------------------
    bool CompositorDemo_FrameListener::handleKeyUpEvent(const CEGUI::EventArgs& e)
    {
        using namespace CEGUI;
        CheckMovementKeys( ((const KeyEventArgs&)e).scancode, false );

        return true;
    }

//--------------------------------------------------------------------------
    void CompositorDemo_FrameListener::CheckMovementKeys( CEGUI::Key::Scan scancode, bool state )
    {
        using namespace CEGUI;

        switch ( scancode )
        {
            case Key::A:
                mMoveLeft = state;
                break;

            case Key::D:
                mMoveRight = state;
                break;

            case Key::S:
                mMoveBck = state;
                break;

            case Key::W:
                mMoveFwd = state;
                break;

            default:
                break;

        }

        mProcessMovement = mMoveLeft || mMoveRight || mMoveFwd || mMoveBck;

    }

//-----------------------------------------------------------------------------------
    void CompositorDemo_FrameListener::itemStateChanged(const size_t index, const bool state)
    {
        // get the item text and tell compositor manager to set enable state
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(mMain->getRenderWindow()->getViewport(0),
            mCompositorSelectorViewManager->getItemSelectorText(index), state);
		updateDebugRTTWindow();
    }
//-----------------------------------------------------------------------------------
    void CompositorDemo_FrameListener::registerCompositors(void)
    {
        Ogre::Viewport *vp = mMain->getRenderWindow()->getViewport(0);

        mCompositorSelectorViewManager = new ItemSelectorViewManager("CompositorSelectorWin");
        // tell view manager to notify us when an item changes selection state
        mCompositorSelectorViewManager->setItemSelectorController(this);
        //iterate through Compositor Managers resources and add name keys ast Item selectors to Compositor selector view manager
        NII::ResourceManager::PrcIDMap::const_iterator i, iend =
            NII::N_Only(Compositor).getPrcResList().end();
        // add all compositor resources to the view container
        for(i = NII::N_Only(Compositor).getPrcResList().begin(); i != iend; ++i)
        {
            Ogre::Resource * resource = i->second;
            const Ogre::String & compositorName = resource->getName();
            // Don't add base Ogre/Scene compositor to view
            if (compositorName == "Ogre/Scene")
                continue;
			// Don't add the deferred shading compositors, thats a different demo.
			if (Ogre::StringUtil::beginsWith(compositorName, "DeferredShading", false))
				continue;

            mCompositorSelectorViewManager->addItemSelector(compositorName);
			int addPosition = -1;
			if (compositorName == "HDR")
			{
				// HDR must be first in the chain
				addPosition = 0;
			}
			try
			{
				Ogre::CompositorInstance *instance = Ogre::CompositorManager::getSingleton().addCompositor(vp, compositorName, addPosition);
				Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, compositorName, false);
			} catch (...) {
			}

        }
    }
	//---------------------------------------------------------------------
	void CompositorDemo_FrameListener::initDebugRTTWindow(void)
	{
		mDebugRTTStaticImage = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"DebugRTTImage");
		mDebugRTTListbox = static_cast<CEGUI::Listbox*>(
			CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"DebugRTTListbox"));
		mDebugRTTListbox->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
			CEGUI::Event::Subscriber(&CompositorDemo_FrameListener::handleRttSelection, this));
	}
	//---------------------------------------------------------------------
	bool CompositorDemo_FrameListener::handleRttSelection(const CEGUI::EventArgs& e)
	{
		if (mDebugRTTListbox->getSelectCount() > 0)
		{
			// image set is in user data
			CEGUI::Imageset* imgSet = (CEGUI::Imageset*)mDebugRTTListbox->getFirstSelectedItem()->getUserData();

			mDebugRTTStaticImage->set("Image", &imgSet->getImage("RttImage")).getSrc());

		}
		else
		{
			mDebugRTTStaticImage->set("Image", "");

		}
		return true;
	}
	//---------------------------------------------------------------------
	void CompositorDemo_FrameListener::updateDebugRTTWindow(void)
	{
		// Clear listbox
		mDebugRTTListbox->removeAll();
		// Clear imagesets
		mDebugRTTStaticImage->set("Image", "");
		for (ImageSetList::iterator isIt = mDebugRTTImageSets.begin();
			isIt != mDebugRTTImageSets.end(); ++isIt)
		{
			CEGUI::ImagesetManager::getSingleton().destroyImageset(*isIt);
		}
		mDebugRTTImageSets.clear();
		Ogre::set<Ogre::String>::type uniqueTextureNames;
		// Add an entry for each render texture for all active compositors
		Ogre::Viewport * vp = mMain->getRenderWindow()->getViewport(0);
		Ogre::FusionInstance * chain = Ogre::CompositorManager::getSingleton().getCompositorChain(vp);
		Ogre::FusionInstance::InstanceIterator it = chain->getCompositors();
		while(it.isNextValid())
		{
			Ogre::CompositorInstance * inst = it.getAndNext();
			if(inst->getEnabled())
			{
				Ogre::CompositionTechnique::TextureDefinitionIterator texIt =
					inst->getShaderFusion()->getTextureDefinitionIterator();
				while (texIt.isNextValid())
				{
					Ogre::CompositionTechnique::FrameDefine* texDef = texIt.getAndNext();

					// Get instance name of texture (NB only index 0 if MRTs for now)
					const Ogre::String& instName = inst->getTextureInstanceName(texDef->name, 0);
					// Create CEGUI texture from name of OGRE texture
					CEGUI::Texture* tex = mMain->getGuiRenderer()->createTexture(instName.c_str());
					// Create imageset
					// Note that if we use shared textures in compositor, the same texture name
					// will occur more than once, so we have to cater for this
					if (uniqueTextureNames.find(instName) == uniqueTextureNames.end())
					{
						CEGUI::Imageset* imgSet =
							CEGUI::ImagesetManager::getSingleton().createImageset(
								instName.c_str(), tex);
						mDebugRTTImageSets.push_back(imgSet);
						imgSet->defineImage((CEGUI::utf8*)"RttImage",
							CEGUI::Point(0.0f, 0.0f),
							CEGUI::Size(tex->getWidth(), tex->getHeight()),
							CEGUI::Point(0.0f,0.0f));

						CEGUI::ListboxTextItem *item = new CEGUI::ListboxTextItem(texDef->name.c_str(), 0, imgSet);
						item->setSelectView("TaharezLook", "ListboxSelectionBrush");
						item->setSelectColour(CEGUI::colour(0,0,1));
						mDebugRTTListbox->addItem(item);

						uniqueTextureNames.insert(instName);
					}
				}
			}
		}
	}