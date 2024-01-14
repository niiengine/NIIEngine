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

#include "OgreRoot.h"
#include "OgreException.h"
#include "OgreStringConverter.h"

#include "OgreGLRenderSystemCommon.h"

#include "OgreEGLSupport.h"
#include "OgreEGLWindow.h"
#include "OgreEGLContext.h"

#include <iostream>
#include <algorithm>
#include <climits>

namespace Ogre {
    EGLWindow::EGLWindow(EGLSupport *glsupport)
        : mGLSupport(glsupport),
          mContext(0),
          mWindow(0),
          mNativeDisplay(0),
          mEglDisplay(EGL_NO_DISPLAY),
          mEglConfig(0),
          mEglSurface(0)
    {
        mIsTopLevel = false;
        mIsFullScreen = false;
        mIsExternal = false;
        mIsExternalGLControl = false;
        mClosed = false;
        mActive = true;//todo
        mIsExternalGLControl = false;
        mVisible = false;
        mVSync = false;
        mVSyncInterval = 1;
    }

    EGLWindow::~EGLWindow()
    {
        destroy();

        if (mContext)
        {
            delete mContext;
        }

        mContext = 0;
    }

    void EGLWindow::destroy(void)
    {
        if (mClosed)
        {
            return;
        }

        mClosed = true;
        mActive = false;

        if (mIsFullScreen)
        {
            mGLSupport->switchMode();
            switchFullScreen(false);
        }
    }

    void EGLWindow::setFullscreen(bool fullscreen, uint width, uint height)
    {
        short frequency = 0;

        if (mClosed || !mIsTopLevel)
        {
            return;
        }

        if (fullscreen == mIsFullScreen && width == mWidth && height == mHeight)
        {
            return;
        }
        if (fullscreen)
        {
            mGLSupport->switchMode(width, height, frequency);
        }
        else
        {
            mGLSupport->switchMode();
        }

        if (mIsFullScreen != fullscreen)
        {
            switchFullScreen(fullscreen);
        }

        if (!mIsFullScreen)
        {
            resize(width, height);
            pos(mLeft, mTop);
        }
    }

    bool EGLWindow::isClosed() const
    {
        return mClosed;
    }

    bool EGLWindow::isVisible() const
    {
        return mVisible;
    }

    void EGLWindow::setVisible(bool visible)
    {
        mVisible = visible;
    }

  

    void EGLWindow::swap()
    {
        if (mClosed || mIsExternalGLControl)
        {
            return;
        }

        if (eglSwapBuffers(mEglDisplay, mEglSurface) == EGL_FALSE)
        {
            EGL_CHECK_ERROR
            N_EXCEPT(RenderingAPI, "Fail to SwapBuffers");
        }
    }

    void EGLWindow::getPlatformData( const String& name, void* pData )
    {
        if (name == "DISPLAYNAME")
        {
            *static_cast<String*>(pData) = mGLSupport->getDisplayName();
            return;
        }
        else if (name == "DISPLAY")
        {
            *static_cast<EGLDisplay*>(pData) = mEglDisplay;
            return;
        }
        else if (name == "GLCONTEXT")
        {
            *static_cast<EGLContext**>(pData) = mContext;
            return;
        } 
        else if (name == "WINDOW")
        {
            *static_cast<NativeWindowType*>(pData) = mWindow;
            return;
        } 
    }

    PixelFormat EGLWindow::suggestPixelFormat() const
    {
        return mGLSupport->getContextProfile() == GLNativeSupport::CONTEXT_ES ? PF_BYTE_RGBA : PF_BYTE_RGB;
    }

    void EGLWindow::copyContentsToMemory(const Box& src, const PixelBox &dst, FrameBuffer buffer)
    {
        if(src.right > mWidth || src.bottom > mHeight || src.front != 0 || src.back != 1
        || dst.getWidth() != src.getWidth() || dst.getHeight() != src.getHeight() || dst.getDepth() != 1)
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid box.", "EGLWindow::copyContentsToMemory");
        }

        if (buffer == FB_AUTO)
        {
            buffer = mIsFullScreen? FB_FRONT : FB_BACK;
        }

        static_cast<GLRenderSystemCommon*>(Root::getSingleton().getRenderSystem())
                ->_copyContentsToMemory(getViewport(0), src, dst, buffer);
    }


    ::EGLSurface EGLWindow::createSurfaceFromWindow(::EGLDisplay display,
                                                    NativeWindowType win)
    {
        ::EGLSurface surface;

        surface = eglCreateWindowSurface(display, mEglConfig, (EGLNativeWindowType)win, NULL);
        EGL_CHECK_ERROR

        if (surface == EGL_NO_SURFACE)
        {
            N_EXCEPT(RenderingAPI,
                        "Fail to create EGLSurface based on NativeWindowType");
        }
        return surface;
    }

    bool EGLWindow::isFlipping() const
    {
        return false;
    }

    void EGLWindow::setVSyncEnabled(bool vsync) {
        mVSync = vsync;
        // we need to make our context current to set vsync
        // store previous context to restore when finished.
        ::EGLSurface oldRead = eglGetCurrentSurface(EGL_READ);
        EGL_CHECK_ERROR
        ::EGLSurface oldDraw = eglGetCurrentSurface(EGL_DRAW);
        EGL_CHECK_ERROR
        ::EGLContext  oldContext  = eglGetCurrentContext();
        EGL_CHECK_ERROR
        ::EGLDisplay dpy = mGLSupport->getGLDisplay();

        mContext->setCurrent();

        if (! mIsExternalGLControl )
        {
            eglSwapInterval(dpy, vsync ? mVSyncInterval : 0);
            EGL_CHECK_ERROR
        }

        mContext->endCurrent();

        eglMakeCurrent (dpy, oldDraw, oldRead, oldContext);
        EGL_CHECK_ERROR
    }

    void EGLWindow::setVSyncInterval(unsigned int interval) {
        mVSyncInterval = interval;
        if (mVSync)
            setVSyncEnabled(true);
    }

}
