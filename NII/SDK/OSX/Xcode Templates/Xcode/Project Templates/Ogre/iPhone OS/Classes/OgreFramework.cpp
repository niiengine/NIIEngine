//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFramework.h"

#include <macUtils.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

template<> OgreFramework * Ogre::Singleton<OgreFramework>::msSingleton = 0;

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFramework::OgreFramework()
{
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;

	m_bShutDownOgre		= false;
	m_iNumScreenShots	= 0;

	m_pRoot				= 0;
	m_pSceneMgr			= 0;
	m_pRenderWnd		= 0;
	m_pCamera			= 0;
	m_pViewport			= 0;
	m_pLog				= 0;
	m_pTimer			= 0;

	m_pInputMgr			= 0;
	m_pKeyboard			= 0;
	m_pMouse			= 0;

	m_pDebugOverlay		= 0;
	m_pInfoOverlay		= 0;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    m_ResourcePath = macBundlePath() + "/Contents/Resources/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
    m_ResourcePath = macBundlePath() + "/";
#else
    m_ResourcePath = "";
#endif
}

//|||||||||||||||||||||||||||||||||||||||||||||||
#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MultiTouchListener *pMouseListener)
#else
bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
#endif
{
    new Ogre::LogManager();

	m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
	m_pLog->setDebugOutputEnabled(true);

    String pluginsPath;
    // only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
    pluginsPath = m_ResourcePath + "plugins.cfg";
#endif

    m_pRoot = new Ogre::Root(pluginsPath, Ogre::macBundlePath() + "/ogre.cfg");

#ifdef OGRE_STATIC_LIB
    m_StaticPluginLoader.load();
#endif

    if(!m_pRoot->config())
        return false;

	m_pRenderWnd = m_pRoot->init(true, wndTitle);

	m_pSceneMgr = m_pRoot->createSpaceManager(SpaceTypeVerbose::ST_Normal, "SpaceManager");
	m_pSceneMgr->setAmbient(NII::Colour(0.7, 0.7, 0.7));

	m_pCamera = m_pSceneMgr->createCamera("Camera");
	m_pCamera->setPos(Vector3f(0, 60, 60));
	m_pCamera->lookAt(Vector3f(0,0,0));
	m_pCamera->setNearClipDistance(1);

	m_pViewport = m_pRenderWnd->createViewport(m_pCamera);
	m_pViewport->setBgColour(Colour(0.8, 0.7, 0.6, 1.0));

	m_pCamera->setAspectRatio(NIIf(m_pViewport->getPixelWidth()) / NIIf(m_pViewport->getPixelHeight()));

	m_pViewport->setCamera(m_pCamera);

	unsigned long hWnd = 0;
    OIS::ParamList paramList;
    m_pRenderWnd->getPlatformData("WINDOW", &hWnd);

	paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StrConv::conv(hWnd)));

	m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
	m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

	m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
	m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();
#else
	m_pMouse = static_cast<OIS::MultiTouch*>(m_pInputMgr->createInputObject(OIS::OISMultiTouch, true));
#endif

#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
	if(pKeyListener == 0)
		m_pKeyboard->setEventCallback(this);
	else
		m_pKeyboard->setEventCallback(pKeyListener);
#endif

	if(pMouseListener == 0)
		m_pMouse->setEventCallback(this);
	else
		m_pMouse->setEventCallback(pMouseListener);

	Ogre::String secName, typeName, archName;
	Ogre::ConfigFile cf;
    cf.load(m_ResourcePath + "resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.isNextValid())
    {
        secName = seci.getCurrentKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getAndNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            if (!Ogre::StrUtil::beginsWith(archName, "/", false)) // only adjust relative dirs
                archName = Ogre::String(Ogre::macBundlePath() + "/" + archName);
#endif
            Ogre::ResourceSchemeManager::getSingleton().add(archName, typeName, secName);
        }
    }
	Ogre::TextureManager::getSingleton().setMipMapCount(5);
	Ogre::ResourceSchemeManager::getSingleton().initialiseAllResourceGroups();

	m_pTimer = N_new Ogre::Timer();
	m_pTimer->reset();

	m_pDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	m_pDebugOverlay->show();

	m_pRenderWnd->setActive(true);

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFramework::~OgreFramework()
{
	if(m_pInputMgr)
	{
		OIS::InputManager::destroyInputSystem(m_pInputMgr);
	}

#ifdef OGRE_STATIC_LIB
    m_StaticPluginLoader.unload();
#endif

	N_delete m_pRoot;
}
//|||||||||||||||||||||||||||||||||||||||||||||||
bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
	m_pLog->logMessage("OgreFramework::keyPressed");

	if(m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
        m_bShutDownOgre = true;
        return true;
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++m_iNumScreenShots << ".png";
		m_pRenderWnd->screenshots(ss.str());
		return true;
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_M))
	{
		static int mode = 0;

		if(mode == 2)
		{
			m_pCamera->setPolygonMode(PM_SOLID);
			mode = 0;
		}
		else if(mode == 0)
		{
            m_pCamera->setPolygonMode(PM_LINE);
            mode = 1;
		}
		else if(mode == 1)
		{
			m_pCamera->setPolygonMode(PM_POINTS);
			mode = 2;
		}
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_O))
	{
		if(m_pDebugOverlay)
		{
			if(!m_pDebugOverlay->isVisible())
				m_pDebugOverlay->show();
			else
				m_pDebugOverlay->hide();
		}
	}
#endif
	return true;
}
//|||||||||||||||||||||||||||||||||||||||||||||||
bool OgreFramework::keyReleased(const OIS::KeyEvent & keyEventRef)
{
	return true;
}
//|||||||||||||||||||||||||||||||||||||||||||||||

#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
bool OgreFramework::touchMoved(const OIS::MultiTouchEvent &evt)
{
    OIS::MultiTouchState state = evt.state;
    int origTransX = 0, origTransY = 0;
    switch(m_pCamera->getViewport()->getDirection())
    {
        case 270:
            origTransX = state.X.rel;
            origTransY = state.Y.rel;
            state.X.rel = -origTransY;
            state.Y.rel = origTransX;
            break;

        case 90:
            origTransX = state.X.rel;
            origTransY = state.Y.rel;
            state.X.rel = origTransY;
            state.Y.rel = origTransX;
            break;

            // Portrait doesn't need any change
        case 0:
        default:
            break;
    }
	m_pCamera->yaw(Angle(state.X.rel * -0.1));
	m_pCamera->pitch(Angle(state.Y.rel * -0.1));

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::touchPressed(const OIS:: MultiTouchEvent &evt)
{
#pragma unused(evt)
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::touchReleased(const OIS:: MultiTouchEvent &evt)
{
#pragma unused(evt)
	return true;
}

bool OgreFramework::touchCancelled(const OIS:: MultiTouchEvent &evt)
{
#pragma unused(evt)
	return true;
}
#else
bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
	m_pCamera->yaw(Angle(evt.state.X.rel * -0.1));
	m_pCamera->pitch(Angle(evt.state.Y.rel * -0.1));

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||
bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}
//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}
#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
void OgreFramework::updateOgre(double timeSinceLastFrame)
{
	m_MoveScale = m_MoveSpeed * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

	m_TranslateVector = Vector3::ZERO;

	getInput();
	moveCamera();
	updateStats();
}
//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFramework::updateStats()
{
	static String currFps = "Current FPS: ";
    static String avgFps = "Average FPS: ";
    static String bestFps = "Best FPS: ";
    static String worstFps = "Worst FPS: ";
    static String tris = "Triangle Count: ";
    static String batches = "Batch Count: ";

    OverlayGeo * guiAvg = OverlayManager::getSingleton().getGeo("Core/AverageFps");
    OverlayGeo * guiCurr = OverlayManager::getSingleton().getGeo("Core/CurrFps");
    OverlayGeo * guiBest = OverlayManager::getSingleton().getGeo("Core/BestFps");
    OverlayGeo * guiWorst = OverlayManager::getSingleton().getGeo("Core/WorstFps");

	const FramesPerSecond & stats = m_pRenderWnd->getFPS();
    guiAvg->setCaption(avgFps + StrConv::conv(stats.mAvg));
    guiCurr->setCaption(currFps + StrConv::conv(stats.mLast));
    guiBest->setCaption(bestFps + StrConv::conv(stats.mBest)
        +" "+StrConv::conv(stats.mBestFrame)+" ms");
    guiWorst->setCaption(worstFps + StrConv::conv(stats.mWorst)
        +" "+StrConv::conv(stats.mWorstFrame)+" ms");

    OverlayGeo * guiTris = OverlayManager::getSingleton().getGeo("Core/NumTris");
    guiTris->setCaption(tris + StrConv::conv(m_pRenderWnd->mTriCount));

	OverlayGeo * guiBatches = OverlayManager::getSingleton().getGeo("Core/NumBatches");
    guiBatches->setCaption(batches + StrConv::conv(m_pRenderWnd->mBatchCount));

	OverlayGeo * guiDbg = OverlayManager::getSingleton().getGeo("Core/DebugText");
	guiDbg->setCaption("");
}
//|||||||||||||||||||||||||||||||||||||||||||||||
void OgreFramework::moveCamera()
{
#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
	if(m_pKeyboard->isKeyDown(OIS::KC_LSHIFT)) 
        m_pCamera->moveRel(m_TranslateVector);
#endif
	m_pCamera->moveRel(m_TranslateVector / 10);
}
//|||||||||||||||||||||||||||||||||||||||||||||||
void OgreFramework::getInput()
{
#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
	if(m_pKeyboard->isKeyDown(OIS::KC_A))
	{
		m_TranslateVector.x = -m_MoveScale;
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_D))
	{
		m_TranslateVector.x = m_MoveScale;
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_W))
	{
		m_TranslateVector.z = -m_MoveScale;
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_S))
	{
		m_TranslateVector.z = m_MoveScale;
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_LEFT))
	{
		m_pCamera->yaw(m_RotScale);
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
	{
		m_pCamera->yaw(-m_RotScale);
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_UP))
	{
		m_pCamera->pitch(m_RotScale);
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_DOWN))
	{
		m_pCamera->pitch(-m_RotScale);
	}
#endif
}
//|||||||||||||||||||||||||||||||||||||||||||||||