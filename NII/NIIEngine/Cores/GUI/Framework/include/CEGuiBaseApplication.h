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
#ifndef _CEGuiBaseApplication_h_
#define _CEGuiBaseApplication_h_

// If this looks wanky, it's becase it is!  Behold that this is not as fullblown
// as it could be though.
#ifndef PATH_MAX
    #include <stdlib.h>
    #ifndef PATH_MAX
        #include <limits.h>
    #endif
    #ifndef PATH_MAX
        #ifdef _MAX_PATH
            #define PATH_MAX _MAX_PATH
        #else
            #define PATH_MAX 260
        #endif
    #endif
#endif

/*************************************************************************
    Forward refs
*************************************************************************/
class SamplesFrameworkBase;
namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    class GeometryPixel;
    class EventArgs;
}
}
}

namespace NII
{
    /** Base application abstract base class.

        The "BaseApplication" family of classes are used to start up and run a
        host application for CeGui samples in a consistent manner.
    */
    class CEGuiBaseApplication
    {
    public:
        //! Constructor.
        CEGuiBaseApplication();

        //! Destructor
        virtual ~CEGuiBaseApplication();

        /** Start the base application
            This will fully initialise the application, finish initialisation of the
            demo via calls to 'sampleApp', and finally control execution of the
            sample.  This calls calls the virtual run function.
        @param sampleApp
            Pointer to the CEGuiSample object that the CEGuiBaseApplication is being
            invoked for.
        @return
            - true if the application initialised and ran okay (cleanup function
              will be called).
            - false if the application failed(cleanup function will
              not be called).
        */
        bool execute(SamplesFrameworkBase * sampleApp);

        /** Performs any required cleanup of the base application system.
        */
        void cleanup();

        /** Render a single display frame.  This should be called by subclasses to
            perform rendering.

            This function handles all per-frame updates, calls beginRendering, then
            renders the CEGUI output, and finally calls endRendering.
        @param elapsed
            Number of seconds elapsed since last frame.
        */
        void renderSingleFrame(float elapsed);

        /** Returns if the renderer (DX or OpenGL) was selected and the
            initialisation was subsequently finished
        */
        bool isInitialised();

        /** Return the path prefix to use for datafiles.  The value returned
            is obtained via a environment variable named 'CEGUI_SAMPLE_DATAPATH'
            if the variable is not set, a default will be used depending on the
            build system in use.
         */
        const char * getDataPathPrefix() const;
    protected:
        //! name of env var that holds the path prefix to the data files.
        static const char DATAPATH_VAR_NAME[];

        //! The abstract function for initialising and running the application.
        virtual void run() = 0;
        //! The abstract function for destroying the renderer and the window.
        virtual void destroyWindow() = 0;
        //! Implementation function to perform required pre-render operations.
        virtual void beginRendering(const float elapsed) = 0;
        //! Implementation function to perform required post-render operations.
        virtual void endRendering() = 0;

        //! initialise the standard default resource groups used by the samples.
        virtual void initialiseDefaultResourceGroups();

        //! function that updates the FPS rendering as needed.
        void updateFPS(float elapsed);
        //! function that updates the logo rotation as needed.
        void updateLogo(float elapsed);
        //! function that positions the logo in the correct place.
        void positionLogo();
        //! event handler function that draws the logo and FPS overlay elements.
        bool overlayHandler(const CEGUI::EventArgs & args);
        //! event handler function called when main view is resized
        bool resizeHandler(const CEGUI::EventArgs & args);
    protected:
        //! true when the base app should cleanup and exit.
        bool d_quitting;
        //! GeometryPixel used for drawing the spinning CEGUI logo
        NII::GeometryPixel * d_logoGeometry;
        //! GeometryPixel used for drawing the FPS value.
        NII::GeometryPixel * d_FPSGeometry;
        //! Fraction of second elapsed (used for counting frames per second).
        float d_FPSElapsed;
        //! Number of frames drawn so far.
        int d_FPSFrames;
        //! Last changed FPS value.
        int d_FPSValue;
        //! whether to spin the logo
        bool d_spinLogo;

        //! SampleFramework base used in the application
        static SamplesFrameworkBase* d_sampleApp;
        //! The window width the application should get created with at start
        static const int s_defaultWindowWidth = 1280;
        //! The window height the application should get created with at start
        static const int s_defaultWindowHeight = 720;
    };
}
#endif