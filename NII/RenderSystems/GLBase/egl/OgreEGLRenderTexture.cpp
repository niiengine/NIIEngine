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

#if 0
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreRoot.h"
#include "OgreStringConverter.h"

#include "OgreEGLRenderTexture.h"
#include "OgreEGLContext.h"
#include "OgreEGLSupport.h"

#include <iostream>
#include <climits>

namespace Ogre {
    EGLPBuffer::EGLPBuffer(EGLSupport* glsupport, PixelComponentType format,
                           size_t width, size_t height)
        : GLESPBuffer(format, width, height)
    {

    }

    //Changed the constructor to a member function so that the
    //native constructor would be called first. This member
    //function is then called from the native constructor.
    void EGLPBuffer::initEGLPBuffer()
    {

//  These are now initialized in the native constructors.
//  mGLSupport = glsupport;
//        mGlDisplay = mGLSupport->getGLDisplay();
        mEglDrawable = 0;
        ::EGLConfig glConfig = 0;

        bool isFloat = false;
        int bits = 0;

        switch (mFormat)
        {
            case PCT_BYTE:
                bits = 8;
                break;

            case PCT_SHORT:
            case PCT_FLOAT16:
                bits = 16;
                break;

            case PCT_FLOAT32:
                bits = 32;
                break;

            default:
                break;
        }

        if (mFormat == PCT_FLOAT16 || mFormat == PCT_FLOAT32)
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
                        "No support for Floating point PBuffers",
                        "EGLRenderTexture::initEGLPBuffer");
        }

        int minAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };

        int maxAttribs[] = {
            EGL_RED_SIZE, bits,
            EGL_GREEN_SIZE, bits,
            EGL_BLUE_SIZE, bits,
            EGL_ALPHA_SIZE, bits,
            EGL_STENCIL_SIZE, INT_MAX,
            EGL_NONE
        };

        int pBufferAttribs[] = {
            // First we specify the width of the surface...
            EGL_WIDTH, mWidth,
            // ...then the height of the surface...
            EGL_HEIGHT, mHeight,
            /* ... then we specify the target for the texture
            that will be created when the pbuffer is created...*/
            EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
            /*..then the format of the texture that will be created
            when the pBuffer is bound to a texture...*/
            EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB,
            // The final thing is EGL_NONE which signifies the end.
            EGL_NONE
        };


        glConfig = mGLSupport->selectGLConfig(minAttribs, maxAttribs);

        EGL_CHECK_ERROR;
        mEglDrawable = eglCreatePbufferSurface(mGlDisplay, glConfig, pBufferAttribs);
        EGL_CHECK_ERROR;

        if (!glConfig || !mEglDrawable)
        {
            N_EXCEPT(RenderingAPI,
                        "Unable to create Pbuffer",
                        "EGLPBuffer::EGLPBuffer");
        }
        GLint glConfigID;
        GLint iWidth, iHeight;

        eglGetConfigAttrib(mGlDisplay, glConfig, EGL_CONFIG_ID, &glConfigID);
        EGL_CHECK_ERROR;
        eglQuerySurface(mGlDisplay, mEglDrawable, EGL_WIDTH, &iWidth);
        EGL_CHECK_ERROR;
        eglQuerySurface(mGlDisplay, mEglDrawable, EGL_HEIGHT, &iHeight);
        EGL_CHECK_ERROR;

        mWidth = iWidth;
        mHeight = iHeight;
        LogManager::getSingleton().logMessage(LML_NORMAL, "EGLPBuffer::create used final dimensions " + StrConv::conv(mWidth) + " x " + StrConv::conv(mHeight));
        LogManager::getSingleton().logMessage("EGLPBuffer::create used FBConfigID " + StrConv::conv(glConfigID));

    }

    EGLPBuffer::~EGLPBuffer()
    {
        eglDestroySurface(mGlDisplay, mEglDrawable);
        LogManager::getSingleton().logMessage(LML_NORMAL, "EGLPBuffer::PBuffer destroyed");
    }


}
#endif
