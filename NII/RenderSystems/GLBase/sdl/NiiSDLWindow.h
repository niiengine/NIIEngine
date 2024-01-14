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

#ifndef _NII_SDLWindow_H_
#define _NII_SDLWindow_H_

#include "NiiSDLPrerequisites.h"
#include "NiiViewWindow.h"

namespace NII
{
    class _NIIPrivate SDLWindow : public ViewWindow
    {
    private:
        SDL_Surface * mScreen;
        bool mActive;
        bool mClosed;

        // Process pending events
        void processEvents(void);

        NIIi (*glXGetVideoSyncSGI) (NIIui *);
        NIIi (*glXWaitVideoSyncSGI) (NIIi, NIIi, NIIui *);

    public:
        SDLWindow();
        ~SDLWindow();

		void setup(const String & name, NCount width, NCount height,
            bool fullScreen, const PropertyData * params);
        /** Overridden - see ViewWindow */
        void destroy(void);
        /** Overridden - see ViewWindow */
        bool isActive(void) const;
        /** Overridden - see ViewWindow */
        bool isClosed(void) const;
        /** Overridden - see ViewWindow */
        void pos(NIIi left, NIIi top);
        /** Overridden - see ViewWindow */
        void resize(NIIui width, NIIui height);
        
        /// @copydetails Frame::swap
        void swap(bool vsync);

        /// @copydetails Frame::fill
        void fill(FrameObj::FaceType buffer, const Box & src, PixelBlock & dst);

        /** Overridden - see FrameObj.
        */
        bool getPlatformData(const String & name, void * pData)
        {
            // NOOP
        }

        bool isFullMode() const
        {
            return ( mScreen->flags & SDL_FULLSCREEN ) == SDL_FULLSCREEN;
        }
    };
}

#endif