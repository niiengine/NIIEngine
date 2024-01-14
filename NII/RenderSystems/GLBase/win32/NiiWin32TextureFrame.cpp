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

#include "NiiWin32TextureFrame.h"
#include "NiiWin32GLSupport.h"
#include "NiiWin32Context.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "NiiImageCodec.h"
#include "NiiStrConv.h"

namespace NII
{
    //-----------------------------------------------------------------------
    Win32PBuffer::Win32PBuffer(PixelDataType format, NCount width, NCount height):
        GLPBuffer(format, width, height)
    {
        createPBuffer();
        mContext = N_new Win32Context(mHDC, mGlrc);
#if 0
        if(mUseBind)
        {
            // Bind texture
            glBindTextureEXT(GL_TEXTURE_2D, static_cast<GLTexture*>(mTexture.get())->getGLID());
            wglBindTexImageARB(mPBuffer, WGL_FRONT_LEFT_ARB);
        }
#endif
    }
    //-----------------------------------------------------------------------
    Win32PBuffer::~Win32PBuffer()
    {
#if 0
        if(mUseBind)
        {
            // Unbind texture
            glBindTextureEXT(GL_TEXTURE_2D, static_cast<GLTexture*>(mTexture.get())->getGLID());
            glBindTextureEXT(GL_TEXTURE_2D, static_cast<GLTexture*>(mTexture.get())->getGLID());
            wglReleaseTexImageARB(mPBuffer, WGL_FRONT_LEFT_ARB);
        }
#endif
		N_delete mContext;
        destroyPBuffer();
    }
    //-----------------------------------------------------------------------
    void Win32PBuffer::createPBuffer()
    {
        // Process format
        NIIi bits = 0;
        bool isFloat = false;
#if 0
        bool hasAlpha = true;
#endif
        switch(mFormat)
        {
        case PFF_Int:  bits=8; isFloat=false;break;
        case PFF_Int16: bits=16;isFloat=false;break;
        case PFF_Float16:bits=16; isFloat=true;break;
        case PFF_Float: bits=32; isFloat=true;break;
        default: break;
        };
        LogManager::getOnly().log(
            _I18n(" Win32PBuffer::Creating PBuffer of format bits=") +
            N_conv(bits) + _T(" float=") + N_conv(isFloat));

        HDC old_hdc = wglGetCurrentDC();
        HGLRC old_context = wglGetCurrentContext();

        // Bind to RGB or RGBA texture
        NIIi bttype = 0;
#if 0
        if(mUseBind)
        {
            // Only provide bind type when actually binding
            bttype = PixelUtil::hasAlpha(mInternalFormat) ?
                WGL_BIND_TO_TEXTURE_RGBA_ARB : WGL_BIND_TO_TEXTURE_RGB_ARB;
        }
        NIIi texformat = hasAlpha ? WGL_TEXTURE_RGBA_ARB : WGL_TEXTURE_RGB_ARB;
#endif
        // Make a NIIf buffer?
        NIIi pixeltype = isFloat ? WGL_TYPE_RGBA_FLOAT_ARB: WGL_TYPE_RGBA_ARB;

        NIIi attrib[] = {
            WGL_RED_BITS_ARB,bits,
            WGL_GREEN_BITS_ARB,bits,
            WGL_BLUE_BITS_ARB,bits,
            WGL_ALPHA_BITS_ARB,bits,
            WGL_STENCIL_BITS_ARB,1,
            WGL_DEPTH_BITS_ARB,15,
            WGL_DRAW_TO_PBUFFER_ARB,true,
            WGL_SUPPORT_OPENGL_ARB,true,
            WGL_PIXEL_TYPE_ARB,pixeltype,
            //WGL_DOUBLE_BUFFER_ARB,true,
            //WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB, // Make sure it is accelerated
            bttype,true, // must be last, as bttype can be zero
            0
        };
        NIIi pattrib_default[] = { 0 };
#if 0
        NIIi pattrib_bind[] = {
            WGL_TEXTURE_FORMAT_ARB, texformat,
            WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,
            WGL_PBUFFER_LARGEST_ARB, true,
            0
        };
#endif
        NIIi format;
        Nui32 count;

        // Choose suitable pixel format
        wglChoosePixelFormatARB(old_hdc, attrib, NULL, 1, &format, &count);
        if(count == 0)
            N_EXCEPT(RenderingAPI, _I18n("wglChoosePixelFormatARB() failed"));

        // Analyse pixel format
        const NIIi piAttributes[]=
        {
            WGL_RED_BITS_ARB,WGL_GREEN_BITS_ARB,WGL_BLUE_BITS_ARB,WGL_ALPHA_BITS_ARB,
            WGL_DEPTH_BITS_ARB,WGL_STENCIL_BITS_ARB
        };
        NIIi piValues[sizeof(piAttributes)/sizeof(const NIIi)];
        wglGetPixelFormatAttribivARB(old_hdc,format,0,sizeof(piAttributes)/sizeof(const NIIi),piAttributes,piValues);

        LogManager::getOnly().stream()
            << _I18n(" Win32PBuffer::PBuffer -- Chosen pixel format rgba=")
            << piValues[0] << "," << piValues[1] << "," << piValues[2] << "," << piValues[3]
            << _I18n(" depth=") << piValues[4] << _I18n(" stencil=") << piValues[5];

        mPBuffer = wglCreatePbufferARB(old_hdc, format, mWidth, mHeight, pattrib_default);
        if(!mPBuffer)
            N_EXCEPT(RenderingAPI, _I18n("wglCreatePbufferARB() failed"));

        mHDC = wglGetPbufferDCARB(mPBuffer);
        if(!mHDC)
        {
            wglDestroyPbufferARB(mPBuffer);
            N_EXCEPT(RenderingAPI, _I18n("wglGetPbufferDCARB() failed"));
        }

        mGlrc = wglCreateContext(mHDC);
        if(!mGlrc)
        {
            wglReleasePbufferDCARB(mPBuffer,mHDC);
            wglDestroyPbufferARB(mPBuffer);
            N_EXCEPT(RenderingAPI, _I18n("wglCreateContext() failed"));
        }

        if(!wglShareLists(old_context, mGlrc))
        {
            wglDeleteContext(mGlrc);
            wglReleasePbufferDCARB(mPBuffer,mHDC);
            wglDestroyPbufferARB(mPBuffer);
            N_EXCEPT(RenderingAPI, _I18n("wglShareLists() failed"));
        }

        // Query real width and height
        NIIi iWidth, iHeight;
        wglQueryPbufferARB(mPBuffer, WGL_PBUFFER_WIDTH_ARB, &iWidth);
        wglQueryPbufferARB(mPBuffer, WGL_PBUFFER_HEIGHT_ARB, &iHeight);
        mWidth = iWidth;
        mHeight = iHeight;
        LogManager::getOnly().stream()<< _I18n("Win32TextureFrame::PBuffer created -- x ")<< mWidth << "y " << mHeight;
    }
    //-----------------------------------------------------------------------
    void Win32PBuffer::destroyPBuffer()
    {
        wglDeleteContext(mGlrc);
        wglReleasePbufferDCARB(mPBuffer,mHDC);
        wglDestroyPbufferARB(mPBuffer);
    }
    //-----------------------------------------------------------------------
}