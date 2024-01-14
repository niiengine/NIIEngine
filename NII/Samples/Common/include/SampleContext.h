/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/

 Copyright (c) 2000-2012 Torus Knot Software Ltd

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */
#ifndef __SampleContext_H__
#define __SampleContext_H__

#include "OgreLogManager.h"
#include "OgrePlugin.h"
#include "FileSystemLayerImpl.h"

// Static plugins declaration section
// Note that every entry in here adds an extra header / library dependency
#ifdef OGRE_STATIC_LIB
#  ifdef OGRE_BUILD_RENDERSYSTEM_GL
#    define OGRE_STATIC_GL
#  endif
#  ifdef OGRE_BUILD_RENDERSYSTEM_GLES
#    define OGRE_STATIC_GLES
#    undef USE_RTSHADER_SYSTEM
#  endif
#  ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#undef OGRE_STATIC_GLES
#    define USE_RTSHADER_SYSTEM
#    define OGRE_STATIC_GLES2
#  endif
#  if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    ifdef OGRE_BUILD_RENDERSYSTEM_D3D9
#        define OGRE_STATIC_Direct3D9
#    endif
// dx11 will only work on vista, so be careful about statically linking
#    ifdef OGRE_BUILD_RENDERSYSTEM_D3D11
#      define OGRE_STATIC_Direct3D11
#    endif
#  endif

#  ifdef OGRE_BUILD_PLUGIN_BSP
#  define OGRE_STATIC_BSPSceneManager
#  endif
#  ifdef OGRE_BUILD_PLUGIN_PFX
#  define OGRE_STATIC_ParticleFX
#  endif
#  ifdef OGRE_BUILD_PLUGIN_CG
#  define OGRE_STATIC_CgProgramManager
#  endif

#  ifdef OGRE_USE_PCZ
#    ifdef OGRE_BUILD_PLUGIN_PCZ
#    define OGRE_STATIC_PCZSceneManager
#    define OGRE_STATIC_OctreeZone
#    endif
#  else
#    ifdef OGRE_BUILD_PLUGIN_OCTREE
#    define OGRE_STATIC_OctreeSceneManager
#  endif
#     endif
#  include "OgreStaticPluginLoader.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#include "macUtils.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#   ifdef __OBJC__
#       import <UIKit/UIKit.h>
#   endif
#endif
#endif

#include "Sample.h"

#include "OIS.h"

namespace OgreBites
{
    /*=============================================================================
    | Base class responsible for setting up a common context for samples.
    | May be subclassed for specific sample types (not specific samples).
    | Allows one sample to run at a time, while maintaining a sample queue.
    =============================================================================*/
    class SampleContext :
        public Ogre::RenderFrameListener,
        public Ogre::WindowEventListener,
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
        public OIS::KeyListener,
        public OIS::MouseListener
#else
        public OIS::MultiTouchListener
#endif
    {
    public:

        SampleContext()
        {
            mFSLayer = OGRE_NEW_T(FileSystemLayerImpl, Ogre::MEMCATEGORY_GENERAL)(OGRE_VERSION_NAME);
            mRoot = 0;
            mWindow = 0;
            mCurrentSample = 0;
            mSamplePaused = false;
            mFirstRun = true;
            mLastRun = false;
            mLastSample = 0;
            mInputMgr = 0;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mMouse = 0;
            mAccelerometer = 0;
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            mMouse = 0;
            mKeyboard = 0;
#else
            mKeyboard = 0;
            mMouse = 0;
#endif
        }

        virtual ~SampleContext()
        {
            N_delete_t(mFSLayer, FileSystemLayer, Ogre::MEMCATEGORY_GENERAL);
        }

        virtual Ogre::ViewWindow* getRenderWindow()
        {
            return mWindow;
        }

        virtual Sample* getCurrentSample()
        {
            return mCurrentSample;
        }

        /*-----------------------------------------------------------------------------
        | Quits the current sample and starts a new one.
        -----------------------------------------------------------------------------*/
        virtual void runSample(Sample * s)
        {
            if(mCurrentSample)
            {
                mCurrentSample->_shutdown();    // quit current sample
                mSamplePaused = false;          // don't pause the next sample
            }

            mWindow->destroyAllViewport();       // wipe viewports

            if (s)
            {
                // retrieve sample's required plugins and currently installed plugins
                Ogre::Root::PluginList ip = mRoot->getPluginList();
                Ogre::StringList rp = s->getRequiredPlugins();

                for (Ogre::StringList::iterator j = rp.begin(); j != rp.end(); j++)
                {
                    bool found = false;
                    // try to find the required plugin in the current installed plugins
                    for (Ogre::Root::PluginList::iterator k = ip.begin(); k != ip.end(); k++)
                    {
                        if ((*k)->getName() == *j)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)  // throw an exception if a plugin is not found
                    {
                        Ogre::String desc = "Sample requires plugin: " + *j;
                        Ogre::String src = "SampleContext::runSample";
                        N_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, desc, src);
                    }
                }

                // throw an exception if samples requires the use of another renderer
                Ogre::String rrs = s->getRequiredRenderSystem();
                if (!rrs.empty() && rrs != mRoot->getRender()->getName())
                {
                    Ogre::String desc = "Sample only runs with renderer: " + rrs;
                    Ogre::String src = "SampleContext::runSample";
                    N_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, desc, src);
                }

                // test system capabilities against sample requirements
                s->testCapabilities(mRoot->getRender()->getFeature());

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
                s->_setup(mWindow, mMouse, mFSLayer);   // start new sample
#else
                s->_setup(mWindow, mKeyboard, mMouse, mFSLayer);   // start new sample
#endif
            }

            mCurrentSample = s;
        }

        /*-----------------------------------------------------------------------------
        | This function initializes the render system and resources.
        -----------------------------------------------------------------------------*/
        virtual void initApp( Sample* initialSample = 0 )
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            createRoot();

            if(!oneTimeConfig()) 
                return;

            if(!mFirstRun) 
                mRoot->set(mRoot->getRenderSystemByName(mNextRenderer));

            setup();

            if (!mFirstRun) recoverLastSample();
            else if (initialSample) runSample(initialSample);

            mRoot->saveConfig();

            Ogre::Root::getSingleton().getRender()->_initAllFrame();

            // Clear event times
            Ogre::Root::getSingleton().resetFrameTime();
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            createRoot();

            setup();

            if (!mFirstRun) recoverLastSample();
            else if (initialSample) runSample(initialSample);

            //mRoot->saveConfig();

            Ogre::Root::getSingleton().getRender()->_initAllFrame();

            // Clear event times
            Ogre::Root::getSingleton().resetFrameTime();

#else
            createRoot();
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
            mNextRenderer = mRoot->getRenderList()[0]->getName();
#else
            if(!oneTimeConfig()) 
                return;
#endif

            // if the context was reconfigured, set requested renderer
            if(!mFirstRun) 
                mRoot->set(mRoot->getRenderSystemByName(mNextRenderer));

            setup();

            // restore the last sample if there was one or, if not, start initial sample
            if (!mFirstRun) recoverLastSample();
            else if (initialSample) runSample(initialSample);
#endif
        }


        /*-----------------------------------------------------------------------------
        | This function closes down the application - saves the configuration then
        | shutdowns.
        -----------------------------------------------------------------------------*/
        virtual void closeApp()
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            shutdown();
#else
            mRoot->saveConfig();
            shutdown();
            if (mRoot) N_delete mRoot;
#ifdef OGRE_STATIC_LIB
            mStaticPluginLoader.unload();
#endif
#endif

        }

        /*-----------------------------------------------------------------------------
        | This function encapsulates the entire lifetime of the context.
        -----------------------------------------------------------------------------*/
#if OGRE_PLATFORM != OGRE_PLATFORM_SYMBIAN && OGRE_PLATFORM != OGRE_PLATFORM_NACL
        virtual void go(Sample* initialSample = 0)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || ((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
            createRoot();

            if (!oneTimeConfig()) return;

            if (!mFirstRun) mRoot->setRenderSystem(mRoot->getRenderSystemByName(mNextRenderer));

            setup();

            if (!mFirstRun) recoverLastSample();
            else if (initialSample) runSample(initialSample);

            mRoot->saveConfig();
#else
            while (!mLastRun)
            {
                mLastRun = true;  // assume this is our last run

                initApp(initialSample);

                if (mRoot->getRender() != NULL)
                {
                    mRoot->beginRender();    // start the render loop
                }

                closeApp();

                mFirstRun = false;
            }
#endif
        }
#endif

        virtual bool isCurrentSamplePaused()
        {
            if (mCurrentSample) return mSamplePaused;
            return false;
        }

        virtual void pauseCurrentSample()
        {
            if (mCurrentSample && !mSamplePaused)
            {
                mSamplePaused = true;
                mCurrentSample->paused();
            }
        }

        virtual void unpauseCurrentSample()
        {
            if (mCurrentSample && mSamplePaused)
            {
                mSamplePaused = false;
                mCurrentSample->unpaused();
            }
        }

        /*-----------------------------------------------------------------------------
        | Processes frame started events.
        -----------------------------------------------------------------------------*/
        virtual bool onPreRender(const Ogre::RenderFrameArgs * evt)
        {
            captureInputDevices();      // capture input

            // manually call sample callback to ensure correct order
            return (mCurrentSample && !mSamplePaused) ? mCurrentSample->onPreRender(evt) : true;
        }

        /*-----------------------------------------------------------------------------
        | Processes rendering queued events.
        -----------------------------------------------------------------------------*/
        virtual bool onRenderCore(const Ogre::RenderFrameArgs * evt)
        {
            // manually call sample callback to ensure correct order
            return (mCurrentSample && !mSamplePaused) ? mCurrentSample->onRenderCore(evt) : true;
        }

        /*-----------------------------------------------------------------------------
        | Processes frame ended events.
        -----------------------------------------------------------------------------*/
        virtual bool onEndRender(const Ogre::RenderFrameArgs * evt)
        {
            // manually call sample callback to ensure correct order
            if(mCurrentSample && !mSamplePaused && !mCurrentSample->onEndRender(evt))
                return false;
            // quit if window was closed
            if(mWindow->isClosed()) 
                return false;
            // go into idle mode if current sample has ended
            if(mCurrentSample && mCurrentSample->isDone()) 
                runSample(0);

            return true;
        }

        /*-----------------------------------------------------------------------------
        | Processes window size change event. Adjusts mouse's region to match that
        | of the window. You could also override this method to prevent resizing.
        -----------------------------------------------------------------------------*/
        virtual void windowResized(Ogre::ViewWindow* rw)
        {
            // manually call sample callback to ensure correct order
            if (mCurrentSample && !mSamplePaused) mCurrentSample->windowResized(rw);

#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
            const OIS::MouseState& ms = mMouse->getMouseState();
            ms.width = rw->getWidth();
            ms.height = rw->getHeight();
#endif
        }

        // window event callbacks which manually call their respective sample callbacks to ensure correct order

        virtual void windowMoved(Ogre::ViewWindow* rw)
        {
            if (mCurrentSample && !mSamplePaused) mCurrentSample->windowMoved(rw);
        }

        virtual bool windowClosing(Ogre::ViewWindow* rw)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->windowClosing(rw);
            return true;
        }

        virtual void windowClosed(Ogre::ViewWindow* rw)
        {
            if (mCurrentSample && !mSamplePaused) mCurrentSample->windowClosed(rw);
        }

        virtual void windowFocusChange(Ogre::ViewWindow* rw)
        {
            if (mCurrentSample && !mSamplePaused) mCurrentSample->windowFocusChange(rw);
        }

        // keyboard and mouse callbacks which manually call their respective sample callbacks to ensure correct order

        virtual bool keyPressed(const OIS::KeyEvent& evt)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->keyPressed(evt);
            return true;
        }

        virtual bool keyReleased(const OIS::KeyEvent& evt)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->keyReleased(evt);
            return true;
        }

#if (NII_VIEWPORT_DIR_NONE == 0)
    #if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        void transformInputState(OIS::MultiTouchState &state)
    #else
        void transformInputState(OIS::MouseState &state)
    #endif
        {
            int w = mWindow->getViewport(0)->getPixelWidth();
            int h = mWindow->getViewport(0)->getPixelHeight();
            int absX = state.X.abs;
            int absY = state.Y.abs;
            int relX = state.X.rel;
            int relY = state.Y.rel;

            switch(mWindow->getViewport(0)->getDirection())
            {
            case 0:
                break;
            case 90:
                state.X.abs = w - absY;
                state.Y.abs = absX;
                state.X.rel = -relY;
                state.Y.rel = relX;
                break;
            case 180:
                state.X.abs = w - absX;
                state.Y.abs = h - absY;
                state.X.rel = -relX;
                state.Y.rel = -relY;
                break;
            case 270:
                state.X.abs = absY;
                state.Y.abs = h - absX;
                state.X.rel = relY;
                state.Y.rel = -relX;
                break;
            }
        }
#elif (NII_VIEWPORT_DIR_NONE == 1) && (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
    // Variation based upon device orientation for use with a view controller
    void transformInputState(OIS::MultiTouchState &state)
    {
        int w = mWindow->getViewport(0)->getPixelWidth();
        int h = mWindow->getViewport(0)->getPixelHeight();
        int absX = state.X.abs;
        int absY = state.Y.abs;
        int relX = state.X.rel;
        int relY = state.Y.rel;

        UIInterfaceOrientation interfaceOrientation = [UIApplication sharedApplication].statusBarOrientation;
        switch (interfaceOrientation)
        {
            case UIInterfaceOrientationPortrait:
                break;
            case UIInterfaceOrientationLandscapeLeft:
                state.X.abs = w - absY;
                state.Y.abs = absX;
                state.X.rel = -relY;
                state.Y.rel = relX;
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                state.X.abs = w - absX;
                state.Y.abs = h - absY;
                state.X.rel = -relX;
                state.Y.rel = -relY;
                break;
            case UIInterfaceOrientationLandscapeRight:
                state.X.abs = absY;
                state.Y.abs = h - absX;
                state.X.rel = relY;
                state.Y.rel = -relX;
                break;
        }
    }
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        virtual bool touchMoved(const OIS::MultiTouchEvent& evt)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->touchMoved(evt);
            return true;
        }
#else
        virtual bool mouseMoved(const OIS::MouseEvent& evt)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->mouseMoved(evt);
            return true;
        }
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        virtual bool touchPressed(const OIS::MultiTouchEvent& evt)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->touchPressed(evt);
            return true;
        }
#else
        virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->mousePressed(evt, id);
            return true;
        }
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        virtual bool touchReleased(const OIS::MultiTouchEvent& evt)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->touchReleased(evt);
            return true;
        }
#else
        virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
        {
            if (mCurrentSample && !mSamplePaused) return mCurrentSample->mouseReleased(evt, id);
            return true;
        }
#endif

    protected:

        /*-----------------------------------------------------------------------------
         | Sets up the context after configuration.
         -----------------------------------------------------------------------------*/
        virtual void setup()
        {
            mWindow = createWindow();
            setupInput();
            locateResources();
            loadResources();

            Ogre::TextureManager::getSingleton().setMipMapCount(5);

            // adds context as listener to process context-level (above the sample level) events
            mRoot->addFrameListener(this);
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
            Ogre::WindowUtil::addWindowEventListener(mWindow, this);
#endif
        }

        /*-----------------------------------------------------------------------------
        | Creates the OGRE root.
        -----------------------------------------------------------------------------*/
        virtual void createRoot()
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            mRoot = Ogre::Root::getSingletonPtr();
#else
            Ogre::String pluginsPath = Ogre::StringUtil::BLANK;
            #ifndef OGRE_STATIC_LIB
                pluginsPath = mFSLayer->getConfigFilePath("plugins.cfg");
            #endif
            mRoot = N_new Ogre::Root(pluginsPath, mFSLayer->getWritablePath("ogre.cfg"),
                mFSLayer->getWritablePath("ogre.log"));
#endif

#ifdef OGRE_STATIC_LIB
            mStaticPluginLoader.load();
#endif
        }

        /*-----------------------------------------------------------------------------
        | Configures the startup settings for OGRE. I use the config dialog here,
        | but you can also restore from a config file. Note that this only happens
        | when you start the context, and not when you reset it.
        -----------------------------------------------------------------------------*/
        virtual bool oneTimeConfig()
        {
            return mRoot->config();
            // return mRoot->restoreConfig();
        }

        /*-----------------------------------------------------------------------------
        | Creates the render window to be used for this context. I use an auto-created
        | window here, but you can also create an external window if you wish.
        | Just don't forget to init the root.
        -----------------------------------------------------------------------------*/
        virtual Ogre::ViewWindow* createWindow()
        {
            return mRoot->init(true);
        }

        /*-----------------------------------------------------------------------------
        | Sets up OIS input.
        -----------------------------------------------------------------------------*/
        virtual void setupInput(bool nograb = false)
        {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
            OIS::ParamList pl;
            size_t winHandle = 0;
            std::ostringstream winHandleStr;

            mWindow->getPlatformData("WINDOW", &winHandle);
            winHandleStr << winHandle;

            pl.insert(Npair("WINDOW", winHandleStr.str()));
            if(nograb)
            {
                pl.insert(Npair("x11_keyboard_grab", "false"));
                pl.insert(Npair("x11_mouse_grab", "false"));
                pl.insert(Npair("w32_mouse", "DISCL_FOREGROUND"));
                pl.insert(Npair("w32_mouse", "DISCL_NONEXCLUSIVE"));
                pl.insert(Npair("w32_keyboard", "DISCL_FOREGROUND"));
                pl.insert(Npair("w32_keyboard", "DISCL_NONEXCLUSIVE"));
            }

            mInputMgr = OIS::InputManager::createInputSystem(pl);

            createInputDevices();      // create the specific input devices

            windowResized(mWindow);    // do an initial adjustment of mouse area
#endif
        }

        /*-----------------------------------------------------------------------------
        | Creates the individual input devices. I only create a keyboard and mouse
        | here because they are the most common, but you can override this method
        | for other modes and devices.
        -----------------------------------------------------------------------------*/
        virtual void createInputDevices()
        {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mMouse = static_cast<OIS::MultiTouch*>(mInputMgr->createInputObject(OIS::OISMultiTouch, true));
            mAccelerometer = static_cast<OIS::JoyStick*>(mInputMgr->createInputObject(OIS::OISJoyStick, true));
#else
            OIS::Object* obj = mInputMgr->createInputObject(OIS::OISKeyboard, true);
            mKeyboard = static_cast<OIS::Keyboard*>(obj);
            mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));

            mKeyboard->setEventCallback(this);
#endif
            mMouse->setEventCallback(this);
#endif
        }

        /*-----------------------------------------------------------------------------
        | Finds context-wide resource groups. I load paths from a config file here,
        | but you can choose your resource locations however you want.
        -----------------------------------------------------------------------------*/
        virtual void locateResources()
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            // TODO: This is handled externally for now
#elif OGRE_PLATFORM == OGRE_PLATFORM_NACL
            Ogre::ResourceSchemeManager::getSingleton().add("Essential.zip", "EmbeddedZip", "Essential");
            Ogre::ResourceSchemeManager::getSingleton().add("Popular.zip", "EmbeddedZip", "Popular");
#else
            // load resource paths from config file
            Ogre::ConfigFile cf;
            cf.load(mFSLayer->getConfigFilePath("resources.cfg"));

            Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
            Ogre::String sec, type, arch;

            // go through all specified resource groups
            while (seci.isNextValid())
            {
                sec = seci.getCurrentKey();
                Ogre::ConfigFile::SettingsMultiMap* settings = seci.getAndNext();
                Ogre::ConfigFile::SettingsMultiMap::iterator i;

                // go through all resource paths
                for (i = settings->begin(); i != settings->end(); i++)
                {
                    type = i->first;
                    arch = i->second;

                    #if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                    // OS X does not set the working directory relative to the app,
                    // In order to make things portable on OS X we need to provide
                    // the loading with it's own bundle path location
                    if (!Ogre::StringUtil::beginsWith(arch, "/", false)) // only adjust relative dirs
                        arch = Ogre::String(Ogre::macBundlePath() + "/" + arch);
                    #endif
                    Ogre::N_Only(ResourceScheme).add(arch, type, sec);
                }
            }
#endif
        }

        /*-----------------------------------------------------------------------------
        | Loads context-wide resource groups. I chose here to simply init all
        | groups, but you can fully load specific ones if you wish.
        -----------------------------------------------------------------------------*/
        virtual void loadResources()
        {
            Ogre::ResourceSchemeManager::getSingleton().initialiseAllResourceGroups();
        }

        /*-----------------------------------------------------------------------------
        | Reconfigures the context. Attempts to preserve the current sample state.
        -----------------------------------------------------------------------------*/
        virtual void reconfigure(const Ogre::String& renderer, Ogre::PropertyData& options)
        {
            // save current sample state
            mLastSample = mCurrentSample;
            if (mCurrentSample) mCurrentSample->saveState(mLastSampleState);

            mNextRenderer = renderer;
            Ogre::RenderSystem* rs = mRoot->getRenderSystemByName(renderer);

            // set all given render system options
            for(Ogre::PropertyData::iterator it = options.begin(); it != options.end(); it++)
            {
                rs->setConfig(it->first, it->second);

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                // Change the viewport orientation on the fly if requested
                if(it->first == "Orientation")
                {
                    if(it->second == "Landscape Left")
                    {
                        mWindow->getViewport(0)->setDirection(270);
                        mWindow->getViewport(0)->setDefaultDirection(270);
                    }
                    else if(it->second == "Landscape Right")
                    {
                        mWindow->getViewport(0)->setDirection(90);
                        mWindow->getViewport(0)->setDefaultDirection(90);
                    }
                    else if(it->second == "Portrait")
                    {
                        mWindow->getViewport(0)->setDirection(0);
                        mWindow->getViewport(0)->setDefaultDirection(0);
                    }
                }
#endif
            }

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            // Need to save the config on iOS to make sure that changes are kept on disk
            mRoot->saveConfig();
#endif
            mLastRun = false;             // we want to go again with the new settings
            mRoot->endRender();   // break from render loop
        }

        /*-----------------------------------------------------------------------------
        | Recovers the last sample after a reset. You can override in the case that
        | the last sample is destroyed in the process of resetting, and you have to
        | recover it through another means.
        -----------------------------------------------------------------------------*/
        virtual void recoverLastSample()
        {
            runSample(mLastSample);
            mLastSample->restoreState(mLastSampleState);
            mLastSample = 0;
            mLastSampleState.clear();
        }

        /*-----------------------------------------------------------------------------
        | Cleans up and shuts down the context.
        -----------------------------------------------------------------------------*/
        virtual void shutdown()
        {
            if (mCurrentSample)
            {
                mCurrentSample->_shutdown();
                mCurrentSample = 0;
            }

            // remove window event listener before shutting down OIS
            Ogre::WindowUtil::removeWindowEventListener(mWindow, this);

            shutdownInput();
        }

        /*-----------------------------------------------------------------------------
        | Destroys OIS input devices and the input manager.
        -----------------------------------------------------------------------------*/
        virtual void shutdownInput()
        {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
            if (mInputMgr)
            {
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
                mInputMgr->destroyInputObject(mKeyboard);
#else
                mInputMgr->destroyInputObject(mAccelerometer);
#endif
                mInputMgr->destroyInputObject(mMouse);

                OIS::InputManager::destroyInputSystem(mInputMgr);
                mInputMgr = 0;
            }
#endif
        }

        /*-----------------------------------------------------------------------------
        | Captures input device states.
        -----------------------------------------------------------------------------*/
        virtual void captureInputDevices()
        {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
            mKeyboard->capture();
#else
            mAccelerometer->capture();
#endif
            mMouse->capture();
#endif
        }

        FileSystemLayer* mFSLayer;         // File system abstraction layer
        Ogre::Root* mRoot;              // OGRE root
        OIS::InputManager* mInputMgr;   // OIS input manager
#ifdef OGRE_STATIC_LIB
        Ogre::StaticPluginLoader mStaticPluginLoader;
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        OIS::MultiTouch* mMouse;        // multitouch device
        OIS::JoyStick* mAccelerometer;  // accelerometer device
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
        OIS::MultiTouch* mMouse;        // multitouch device
        OIS::Keyboard* mKeyboard;       // keyboard device
#else
        OIS::Keyboard* mKeyboard;       // keyboard device
        OIS::Mouse* mMouse;             // mouse device
#endif
        Sample* mCurrentSample;         // currently running sample
        bool mSamplePaused;             // whether current sample is paused
        bool mFirstRun;                 // whether or not this is the first run
        bool mLastRun;                  // whether or not this is the final run
        Ogre::String mNextRenderer;     // name of renderer used for next run
        Sample* mLastSample;            // last sample run before reconfiguration
        Ogre::PropertyData mLastSampleState;     // state of last sample
    public:
        Ogre::ViewWindow* mWindow;    // render window
    };
}

#endif
