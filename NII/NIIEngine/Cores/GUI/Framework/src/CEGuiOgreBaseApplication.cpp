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

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__HAIKU__)
    #include <unistd.h>
#endif

// this controls conditional compile of file for Apple
#include "CEGUISamplesConfig.h"
#ifdef CEGUI_SAMPLES_USE_OGRE

#include <OgreWindowEventUtilities.h>
#include "CEGuiOgreBaseApplication.h"
#include "SamplesFrameworkBase.h"

namespace NII
{
    //------------------------------------------------------------------------
    CEGuiOgreBaseApplication::CEGuiOgreBaseApplication() :
        d_ogreRoot(0),
        d_initialised(false),
        d_frameListener(0),
        d_windowEventListener(0)
    {
        using namespace Ogre;

    #ifdef DEBUG
        Ogre::String pluginsFileName = "plugins_d.cfg";
    #else
        Ogre::String pluginsFileName = "plugins.cfg";
    #endif

        d_ogreRoot = N_new Engine(pluginsFileName);

        if(d_ogreRoot->config())
        {
            // initialise system according to user options.
            d_window = d_ogreRoot->initialise(true);

            // Create the scene manager
            SpaceManager * sm = d_ogreRoot->createSpaceManager(SpaceTypeVerbose::ST_Normal, "SampleSceneMgr");
            // Create and initialise the camera
            d_camera = sm->createCamera("SampleCam");
            d_camera->setPosition(Vector3(0,0,500));
            d_camera->lookAt(Vector3f(0, 0, -300));
            d_camera->setNearClipDistance(5);

            // Create a viewport covering whole window
            Viewport * vp = d_window->createViewport(d_camera);
            vp->setBgColour(Colour(0.0f, 0.0f, 0.0f, 0.0f));
            // Update the camera aspect ratio to that of the viewport
            d_camera->setAspectRatio(NIIf(vp->getPixelWidth()) / NIIf(vp->getPixelHeight()));

            // create frame listener
            d_frameListener = N_new CEGuiDemoFrameListener(this, d_sampleApp, d_window, d_camera);
            d_ogreRoot->add(d_frameListener);

            // add a listener for OS framework window events (for resizing)
            d_windowEventListener = N_new WndEvtListener(d_frameListener->getOISMouse());
            WindowUtil::addWindowEventListener(d_window, d_windowEventListener);

            d_ogreRoot->add(this);
            N_OnlyPtr(UI)->setEnable(false);

            d_initialised = true;
        }
        else
        {
            // aborted.  Clean up and set root to 0 so when app attempts to run it
            // knows what happened here.
            N_delete d_ogreRoot;
            d_ogreRoot = 0;
        }
    }
    //------------------------------------------------------------------------
    CEGuiOgreBaseApplication::~CEGuiOgreBaseApplication()
    {
        N_delete d_frameListener;
        N_delete d_ogreRoot;
        N_delete d_windowEventListener;
    }
    //------------------------------------------------------------------------
    void CEGuiOgreBaseApplication::run()
    {
        // if base initialisation failed or app was cancelled by user, bail out now.
        if(!d_ogreRoot || !d_initialised)
            return;

        N_Only(ResourceScheme).setupAll();
        d_sampleApp->initialise();

        // start rendering via Ogre3D engine.
        CEGUI_TRY
        {
            d_ogreRoot->beginRender();
        }
        CEGUI_CATCH(...)
        {

        }
    }
    //------------------------------------------------------------------------
    void CEGuiOgreBaseApplication::destroyWindow()
    {

    }
    //------------------------------------------------------------------------
    void CEGuiOgreBaseApplication::beginRendering(const float elapsed)
    {
        // this is nover called under Ogre, since we're not in control of the
        // rendering process.
    }
    //------------------------------------------------------------------------
    void CEGuiOgreBaseApplication::endRendering()
    {
        // this is nover called under Ogre, since we're not in control of the
        // rendering process.
    }
    //------------------------------------------------------------------------
    void CEGuiOgreBaseApplication::initialiseResourceGroupDirectories()
    {
        using namespace Ogre;
        ResourceSchemeManager & rgm = ResourceSchemeManager::getOnly();

        // add resource groups that we use
        rgm.create("animations");
        rgm.create("imagesets");
        rgm.create("fonts");
        rgm.create("layouts");
        rgm.create("schemes");
        rgm.create("looknfeels");
        rgm.create("lua_scripts");
        rgm.create("schemas");
        rgm.create("samples");

        // add CEGUI sample framework datafile dirs as resource locations
        ResourceSchemeManager::getOnly().add("./", "FileSystem");

        const char * dataPathPrefix = getDataPathPrefix();
        char resourcePath[PATH_MAX];

        // for each resource type, set a resource group directory
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "schemes/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "schemes");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "samples/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "samples");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "imagesets/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "imagesets");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "fonts/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "fonts");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "layouts/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "layouts");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "looknfeel/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "looknfeels");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "lua_scripts/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "lua_scripts");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "animations/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "animations");
        sprintf(resourcePath, "%s/%s", dataPathPrefix, "xml_schemas/");
        ResourceSchemeManager::getOnly().add(resourcePath, "FileSystem", "schemas");
    }
    //------------------------------------------------------------------------
    void CEGuiOgreBaseApplication::doFrameUpdate(const float elapsed)
    {
    }
    //------------------------------------------------------------------------
    bool CEGuiOgreBaseApplication::onRenderCore(const Ogre::RenderFrameArgs * args)
    {
        renderSingleFrame(static_cast<float>(args->mCurrent));

        return true;
    }
    //------------------------------------------------------------------------
    bool CEGuiOgreBaseApplication::isInitialised()
    {
        return d_initialised;
    }
    //------------------------------------------------------------------------
    /*************************************************************************
        Start of CEGuiDemoFrameListener mehods
    *************************************************************************/
    //------------------------------------------------------------------------
    CEGuiDemoFrameListener::CEGuiDemoFrameListener(CEGuiOgreBaseApplication * baseApp,
        SamplesFrameworkBase *& sampleApp, ViewWindow * window, Camera * camera,
            bool useBufferedInputKeys, bool useBufferedInputMouse):
                d_baseApp(baseApp),
                d_sampleApp(sampleApp)
    {
        // OIS setup
        OIS::ParamList paramList;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        // get window handle
        window->getPlatformData("WINDOW", &windowHnd);

        // fill param list
        windowHndStr << (unsigned int)windowHnd;
        paramList.insert(Npair(std::string("WINDOW"), windowHndStr.str()));

    #if OGRE_PLATFORM == OGRE_PLATFORM_LINUX && defined (DEBUG)
        paramList.insert(Npair(std::string("x11_keyboard_grab"), "false"));
        paramList.insert(Npair(std::string("x11_mouse_grab"), "false"));
        paramList.insert(Npair(std::string("x11_mouse_hide"), "false"));
    #endif

        // create input system
        d_inputManager = OIS::InputManager::createInputSystem(paramList);

        // create buffered keyboard
    #ifdef CEGUI_OLD_OIS_API
        if(d_inputManager->numKeyboards() > 0)
    #else
        if(d_inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
    #endif
        {
            d_keyboard = static_cast<OIS::Keyboard *>(d_inputManager->createInputObject(OIS::OISKeyboard, true));
            d_keyboard->setEventCallback(this);
        }

        // create buffered mouse
    #ifdef CEGUI_OLD_OIS_API
        if(d_inputManager->numMice() > 0)
    #else
        if(d_inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
    #endif
        {
            d_mouse = static_cast<OIS::Mouse *>(d_inputManager->createInputObject(OIS::OISMouse, true));
            d_mouse->setEventCallback(this);

            unsigned int width, height, depth;
            int left, top;

            window->getArea(left, top, width, height, depth);
            const OIS::MouseState & mouseState = d_mouse->getMouseState();
            mouseState.width = width;
            mouseState.height = height;
        }

        // store inputs we want to make use of
        d_camera = camera;
        d_window = window;
    }
    //------------------------------------------------------------------------
    CEGuiDemoFrameListener::~CEGuiDemoFrameListener()
    {
        if (d_inputManager)
        {
            d_inputManager->destroyInputObject(d_mouse);
            d_inputManager->destroyInputObject(d_keyboard);
            OIS::InputManager::destroyInputSystem(d_inputManager);
        }
    }
    //------------------------------------------------------------------------
    bool CEGuiDemoFrameListener::onPreRender(const Ogre::RenderFrameArgs * evt)
    {
        if(d_window->isClosed() || d_sampleApp->isQuitting())
            return false;

        static_cast<CEGuiOgreBaseApplication *>(d_baseApp)->
            doFrameUpdate(static_cast<float>(evt->mCurrent));

        // update input system
        if (d_mouse)
            d_mouse->capture();
        if (d_keyboard)
            d_keyboard->capture();

        return true;
    }
    //------------------------------------------------------------------------
    bool CEGuiDemoFrameListener::onEndRender(const Ogre::RenderFrameArgs * evt)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool CEGuiDemoFrameListener::mouseMoved(const OIS::MouseEvent & e)
    {
        d_sampleApp->onCursorMove(e.state.X.abs, e.state.Y.abs);
        d_sampleApp->onCursorDrag(e.state.Z.rel / 120.0f);

        return true;
    }
    //------------------------------------------------------------------------
    bool CEGuiDemoFrameListener::keyPressed(NII::KeyEvent * e)
    {
        d_sampleApp->onKeyDown(static_cast<KeyValue>(e->key));
        d_sampleApp->onChar(e->text);

        return true;
    }
    //------------------------------------------------------------------------
    bool CEGuiDemoFrameListener::keyReleased(NII::KeyEvent * e)
    {
        d_sampleApp->onKeyUp(static_cast<KeyValue>(e->key));

        return true;
    }
    //------------------------------------------------------------------------
    bool CEGuiDemoFrameListener::mousePressed(const OIS::MouseEvent & e,
        OIS::MouseButtonID id)
    {
        d_sampleApp->onButtonDown(convertOISButtonToCegui(id));

        return true;
    }
    //------------------------------------------------------------------------
    bool CEGuiDemoFrameListener::mouseReleased(const OIS::MouseEvent & e,
        OIS::MouseButtonID id)
    {
       d_sampleApp->onButtonUp(convertOISButtonToCegui(id));

        return true;
    }
    //------------------------------------------------------------------------
    MouseButton CEGuiDemoFrameListener::convertOISButtonToCegui(int buttonID)
    {
        using namespace OIS;

        switch (buttonID)
        {
        case OIS::MB_Left:
            return LeftButton;
        case OIS::MB_Right:
            return RightButton;
        case OIS::MB_Middle:
            return MiddleButton;
        default:
            return LeftButton;
        }
    }
    //------------------------------------------------------------------------
    OIS::Mouse * CEGuiDemoFrameListener::getOISMouse()
    {
        return d_mouse;
    }
    //------------------------------------------------------------------------
    /*************************************************************************
        Start of WndEvtListener member functions
    *************************************************************************/
    //------------------------------------------------------------------------
    WndEvtListener::WndEvtListener(OIS::Mouse * mouse) :
        d_mouse(mouse)
    {
    }
    //------------------------------------------------------------------------
    void WndEvtListener::windowResized(Ogre::ViewWindow * rw)
    {
        UIManager * const sys = UIManager::getOnlyPtr();
        if(sys)
            sys->setViewSize(PlaneSizef(static_cast<float>(rw->getWidth()),
                static_cast<float>(rw->getHeight())));

        const OIS::MouseState & mouseState = d_mouse->getMouseState();
        mouseState.width = rw->getWidth();
        mouseState.height = rw->getHeight();
    }
    //------------------------------------------------------------------------
}
#endif