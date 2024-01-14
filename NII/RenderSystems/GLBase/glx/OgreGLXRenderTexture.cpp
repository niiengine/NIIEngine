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
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreRoot.h"
#include "OgreStringConverter.h"

#include "OgreGLXRenderTexture.h"
#include "OgreGLXContext.h"
#include "OgreGLXGLSupport.h"

#include <climits>

#define GLX_RGBA_FLOAT_ATI_BIT 0x00000100
#define GLX_RGBA_FLOAT_BIT     0x00000004

namespace Ogre
{
    //-------------------------------------------------------------------------------------------------//
    GLXPBuffer::GLXPBuffer(GLXGLSupport* glsupport, PixelComponentType format, size_t width, size_t height):
        GLPBuffer(format, width, height), mContext(0), mGLSupport(glsupport)
    {
        Display *glDisplay = mGLSupport->getGLDisplay();
        ::GLXDrawable glxDrawable = 0;
        ::GLXFBConfig fbConfig = 0;
        
        int bits = 0;
        
        switch (mFormat)
        {
        case PCT_BYTE:
            bits = 8; 
            break;
            
        case PCT_SHORT:
            bits = 16; 
            break;
            
        case PCT_FLOAT16:
            bits = 16; 
            break;
            
        case PCT_FLOAT32:
            bits = 32; 
            break;
            
        default: 
            break;
        }
        
        int renderAttrib = GLX_RENDER_TYPE;
        int renderValue  = GLX_RGBA_BIT;
        
        if (mFormat == PCT_FLOAT16 || mFormat == PCT_FLOAT32)
        {
            if (glsupport->checkGpuApi("GLX_NV_float_buffer"))
            {
                renderAttrib = GLX_FLOAT_COMPONENTS_NV;
                renderValue  = GL_TRUE;
            }
            
            if (glsupport->checkGpuApi("GLX_ATI_pixel_format_float"))
            {
                renderAttrib = GLX_RENDER_TYPE;
                renderValue  = GLX_RGBA_FLOAT_ATI_BIT;
            }
            
            if (glsupport->checkGpuApi("GLX_ARB_fbconfig_float"))
            {
                renderAttrib = GLX_RENDER_TYPE;
                renderValue  = GLX_RGBA_FLOAT_BIT;
            }
            
            if (renderAttrib == GLX_RENDER_TYPE && renderValue == GLX_RGBA_BIT)
            {
                N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "No support for Floating point PBuffers",  "GLTextureFrame::createPBuffer");
            }
        }
        
        int minAttribs[] = {
            GLX_DRAWABLE_TYPE, GLX_PBUFFER,
            renderAttrib,     renderValue,
            GLX_DOUBLEBUFFER,  0,
            None
        };
        
        int maxAttribs[] = {
            GLX_RED_SIZE,     bits,
            GLX_GREEN_SIZE, bits,
            GLX_BLUE_SIZE,   bits,
            GLX_ALPHA_SIZE, bits,
            GLX_STENCIL_SIZE,  INT_MAX,
            None
        };
        
        int pBufferAttribs[] = {
            GLX_PBUFFER_WIDTH,    (int)mWidth,
            GLX_PBUFFER_HEIGHT,  (int)mHeight,
            GLX_PRESERVED_CONTENTS, GL_TRUE,
            None
        };
        
        fbConfig = mGLSupport->selectFBConfig(minAttribs, maxAttribs);
        
        glxDrawable = glXCreatePbuffer(glDisplay, fbConfig, pBufferAttribs);
        
        if (! fbConfig || ! glxDrawable) 
        {
            N_EXCEPT(RenderingAPI, "Unable to create Pbuffer", "GLXPBuffer::GLXPBuffer");
        }
        
        GLint fbConfigID;
        GLuint iWidth, iHeight;
        
        glXGetFBConfigAttrib(glDisplay, fbConfig, GLX_FBCONFIG_ID, &fbConfigID);
        glXQueryDrawable(glDisplay, glxDrawable, GLX_WIDTH, &iWidth);
        glXQueryDrawable(glDisplay, glxDrawable, GLX_HEIGHT, &iHeight);
        
        mWidth = iWidth;  
        mHeight = iHeight;
        LogManager::getSingleton().logMessage(LML_NORMAL, "GLXPBuffer::create used final dimensions " + StrConv::conv(mWidth) + " x " + StrConv::conv(mHeight));
        LogManager::getSingleton().logMessage("GLXPBuffer::create used FBConfigID " + StrConv::conv(fbConfigID));
        
        mContext = new GLXContext(mGLSupport, fbConfig, glxDrawable);
    }
    
    //-------------------------------------------------------------------------------------------------//
    GLXPBuffer::~GLXPBuffer()
    {
        glXDestroyPbuffer(mGLSupport->getGLDisplay(), mContext->mDrawable);
        
        delete mContext;
        
        LogManager::getSingleton().logMessage(LML_NORMAL, "GLXPBuffer::PBuffer destroyed");
    }
    
    //-------------------------------------------------------------------------------------------------//
    GLContext *GLXPBuffer::getContext() const
    {
        return mContext;
    }
}
