/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

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

#ifndef __GLXWindow_H__
#define __GLXWindow_H__

#include "OgreRenderWindow.h"
#include "OgreGLRenderTarget.h"

#include <X11/X.h>

namespace Ogre 
{
    class GLXContext;
    class GLXGLSupport;

    class _OgrePrivate GLXWindow : public RenderWindow, public GLRenderTarget
    {
    public:
        GLXWindow(GLXGLSupport* glsupport);
        ~GLXWindow();
        
        void create(const String& name, unsigned int width, unsigned int height,
                    bool fullScreen, const PropertyData *miscParams);
        
        /** @copydoc see RenderWindow::setFullscreen */
        void setFullscreen (bool fullscreen, uint width, uint height);
        
        /** @copydoc see RenderWindow::destroy */
        void destroy(void);
        
        /** @copydoc see RenderWindow::isClosed */
        bool isClosed(void) const;
        
        /** @copydoc see RenderWindow::isVisible */
        bool isVisible(void) const;
        
        /** @copydoc see RenderWindow::setVisible */
        void setVisible(bool visible);

        /** @copydoc see RenderWindow::isHidden */
        bool isHidden(void) const { return mHidden; }

        /** @copydoc see RenderWindow::setHidden */
        void setHidden(bool hidden);

        /** @copydoc see RenderWindow::setVSyncEnabled */
        void setVSyncEnabled(bool vsync);

        /** @copydoc see RenderWindow::isVSyncEnabled */
        bool isVSyncEnabled() const;

        /** @copydoc see RenderWindow::setVSyncInterval */
        void setVSyncInterval(unsigned int interval);

        /** @copydoc see RenderWindow::getSwapInterval */
        unsigned int getSwapInterval() const;
        
        /** @copydoc see RenderWindow::pos */
        void pos(int left, int top);
        
        /** @copydoc see RenderWindow::resize */
        void resize(unsigned int width, unsigned int height);

        /** @copydoc see RenderWindow::notify */
        void notify();
        
        /** @copydoc see RenderWindow::swap */
        void swap();
        
        /** @copydoc see RenderTarget::copyContentsToMemory */
        void copyContentsToMemory(const Box& src, const PixelBox &dst, FrameBuffer buffer);
        
        /**
           @remarks
           * Get custom attribute; the following attributes are valid:
           * WINDOW      The X Window target for rendering.
           * GLCONTEXT    The Ogre GLContext used for rendering.
           * DISPLAY        The X Display connection behind that context.
           * DISPLAYNAME    The X Server name for the connected display.
           * ATOM          The X Atom used in client delete events.
           */
        void getPlatformData(const String& name, void* pData);
        
        bool isFlipping() const { return false; }

        PixelFormat suggestPixelFormat() const;

        GLContext* getContext() const;

    private:
        bool mClosed;
        bool mVisible;
        bool mHidden;
        bool mIsTopLevel;
        bool mIsExternal;
        bool mIsExternalGLControl;
        bool mVSync;
        int mVSyncInterval;
        
        GLXGLSupport* mGLSupport;
        ::Window      mWindow;
        GLXContext*   mContext;
        void switchFullScreen(bool fullscreen);
    };
}

#endif
