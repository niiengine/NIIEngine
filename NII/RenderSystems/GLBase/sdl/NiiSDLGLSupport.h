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

#ifndef _NII_SDLGLSUPPORT_H_
#define _NII_SDLGLSUPPORT_H_

#include "NiiSDLPrerequisites.h"
#include "NiiGLSupport.h"

namespace NII
{
    
    class _NIIPrivate SDLGLSupport : public GLSupport
    {
    public:
        SDLGLSupport();
        ~SDLGLSupport();

        /**
        * Add any special config values to the system.
        * Must have a "Full Screen" value that is a bool and a "Video Mode" value
        * that is a string in the form of wxh
        */
        void addConfig();
        /**
        * Make sure all the extra options are valid
        */
        String checkConfig();

        virtual ViewWindow * createWindow(bool autoCreateWindow, GLRenderSystem* renderSystem, const String& windowTitle);

        /// @copydoc RenderSys::createWindow
        virtual ViewWindow * newWindow(const String &name, NCount width, NCount height, 
            bool fullScreen, const PropertyData * miscParams = 0);

        /**
        * Start anything special
        */
        void start();
        /**
        * Stop anything special
        */
        void stop();

        /**
        * Get the address of a function
        */
        void* getProcAddress(const String& procname);
    private:
        // Allowed video modes
        SDL_Rect** mVideoModes;


    }; // class SDLGLSupport

}; // namespace NII

#endif // NII_SDLGLSUPPORT_H