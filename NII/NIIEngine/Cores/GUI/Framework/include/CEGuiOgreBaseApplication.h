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
#ifndef _CEGuiOgreBaseApplication_h_
#define _CEGuiOgreBaseApplication_h_

#include "CEGuiBaseApplication.h"
#include "CEGUI/CEGUI.h"

#include <Ogre.h>
#include <OIS.h>

// Frame listener forward ref (see class below)
class CEGuiDemoFrameListener;
// Window event listener forward ref (see class below)
class WndEvtListener;


    class CEGuiOgreBaseApplication : public CEGuiBaseApplication, public Ogre::RenderFrameListener
    {
    public:
        //! Constructor.
        CEGuiOgreBaseApplication();

        //! Destructor.
        ~CEGuiOgreBaseApplication();

        //! called by the frame listener to perform requried per-frame updates.
        void doFrameUpdate(float elapsed);

        //! Ogre frame listener callback that will trigger our specific context rendering
        bool onRenderCore(const Ogre::RenderFrameArgs * evt);

        bool isInitialised();
    protected:
        // override from base class since we use a non-default resource provider.
        void initialiseResourceGroupDirectories();

        // Implementation of base class abstract methods.
        void run();
        void destroyWindow();
        void beginRendering(const float elapsed);
        void endRendering();

        /*************************************************************************
            Data Fields
        *************************************************************************/
        NII::Engine * d_ogreRoot;
        NII::Camera * d_camera;
        NII::ViewWindow * d_window;
        bool d_initialised;

        CEGuiDemoFrameListener* d_frameListener;
        WndEvtListener* d_windowEventListener;
    };


/*!
\brief
    Ogre RenderFrameListener class where we deal with input processing and the like.
*/
class CEGuiDemoFrameListener : public Ogre::RenderFrameListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
    // Construction and Destruction
    CEGuiDemoFrameListener(CEGuiOgreBaseApplication* baseApp, SamplesFrameworkBase*& sampleApp, Ogre::ViewWindow* win, Ogre::Camera* cam, bool useBufferedInputKeys = false, bool useBufferedInputMouse = false);
    ~CEGuiDemoFrameListener();

    // Processing to be done at start and end of each frame.
    bool onPreRender(const NII::RenderFrameArgs * evt);
    bool onEndRender(const Ogre::RenderFrameArgs * evt);

    // Raw input handlers that we care about
    bool mouseMoved(const OIS::MouseEvent & e);
    bool keyPressed(const OIS::KeyEvent & e);
    bool keyReleased(const OIS::KeyEvent & e);
    bool mousePressed(const OIS::MouseEvent & e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent & e, OIS::MouseButtonID id);

    OIS::Mouse* getOISMouse();

protected:
    // convert an OIS mouse button into a CEGUI mouse button
    CEGUI::MouseButton convertOISButtonToCegui(int buttonID);

    /*************************************************************************
        Data Fields
    *************************************************************************/
    OIS::InputManager* d_inputManager;
    OIS::Keyboard* d_keyboard;
    OIS::Mouse* d_mouse;
    Ogre::Camera * d_camera;
    Ogre::ViewWindow * d_window;

    CEGuiOgreBaseApplication *   d_baseApp;
    SamplesFrameworkBase *&      d_sampleApp;
};

//! window event listener class we use to hear abour window resizing
class WndEvtListener : public Ogre::WindowEventListener
{
public:
    WndEvtListener(OIS::Mouse * mouse);

    void windowResized(Ogre::ViewWindow * rw);
protected:
    OIS::Mouse* d_mouse;
};


#endif  // end of guard _CEGuiOgreBaseApplication_h_
