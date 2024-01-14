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
#ifndef _SamplesFrameworkBase_h_
#define _SamplesFrameworkBase_h_

// forward declarations
class CEGuiBaseApplication;
class CEGuiRendererSelector;

#include "NiiUIPreInclude.h"
#include "CEGUI/InputEvent.h"

    /** This is a base class that is intended to be used for all sample applications.
        Here we take care of common things such the renderer selection and application
        startup.
    */
    class SamplesFrameworkBase
    {
    public:
        /** Constructor.
        */
        SamplesFrameworkBase();

        /** Destructor.
        */
        virtual ~SamplesFrameworkBase();

        /** Application entry point.
        @return code to be returned by the application.
        */
        int run();

        /** Sample specific initialisation goes here.  This method is called by the application base object created
            as part of the initialise call.
        @return false if something went wrong.
        */
        virtual bool initialise()  = 0;

        /** deinitialise the resources allocated in the initialise if needed.
        */
        virtual void deinitialise() = 0;

        /** Update function called before rendering
        */
        virtual void update(float passedTime) = 0;

        /** Update function for window size changes
        */
        virtual void handleNewWindowSize(float width, float height) = 0;

        /** Draw function to draw GUIContexts
        */
        virtual void renderGUIContexts() = 0;

        /** Function to inject key down to GUIContexts
        @return true if event was handled.
        */
        virtual bool onKeyDown(NII::KeyValue key) = 0;

        /** Function to inject key up to GUIContexts
        @return true if event was handled.
        */
        virtual bool onKeyUp(NII::KeyValue key) = 0;

        /** Function to inject characters to GUIContexts
        @return true if event was handled.
        */
        virtual bool onChar(Nui32 chr) = 0;

        /** Function to inject mouse button down to GUIContexts
        @return true if event was handled.
        */
        virtual bool onButtonDown(NII::MouseButton button) = 0;

        /** Function to inject mouse button up to GUIContexts
        @return true if event was handled.
        */
        virtual bool onButtonUp(NII::MouseButton button) = 0;

        /** Function to inject mouse wheel changes to GUIContexts
        @return true if event was handled.
        */
        virtual bool onCursorDrag(NIIi drag) = 0;

        /** Function to inject the mouse position to GUIContexts
        @return true if event was handled.
        */
        virtual bool onCursorMove(NIIi x, NIIi y) = 0;

        /** Function to set the bool defining if the application should quit as soon as possible
        */
        virtual void setQuitting(bool quit);

        /** Function returning if the application should quit as soon as possible
        @return true if should quit.
        */
        bool isQuitting();

        /** Function setting the application window's size
        */
        void setApplicationWindowSize(int width, int height);
    protected:
        /** Initialises the sample system, this includes asking the user for a render to use and
            the subsequent creation of the required systems to support that renderer.
        @return
            false if anything went wrong.
        */
        virtual bool runApplication();

        /** Cleans up all resources allocated by the initialise call.
        */
        virtual void cleanup();

        /** Output a message to the user in some OS independant way.
        */
        static void outputExceptionMessage(const char * message);
    protected:
        CEGuiRendererSelector * d_rendererSelector; //!< Points to the renderer selector object.
        CEGuiBaseApplication * d_baseApp;           //!< Pointer to the base application object.

        bool d_quitting;        //!< Bool defining if application should quit.
        int d_appWindowWidth;   //!< Int defining the application window's width.
        int d_appWindowHeight;  //!< Int defining the application window's height.
    };

#endif  // end of guard _SamplesFrameworkBase_h_