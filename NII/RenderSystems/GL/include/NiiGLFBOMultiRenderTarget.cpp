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

#include "NiiGLFBOMultiRenderTarget.h"
#include "NiiGLPixelFormat.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiEngine.h"
#include "NiiGLHardwarePixelBuffer.h"

namespace NII
{
    //-----------------------------------------------------------------------------
    GLFBOMultiRenderTarget::GLFBOMultiRenderTarget(GLFBOManager *manager, const String &name):
        MultiTextureFrame(name),
        fbo(manager, 0 /* TODO: multisampling on MRTs? */)
    {
    }
    //-----------------------------------------------------------------------------
    GLFBOMultiRenderTarget::~GLFBOMultiRenderTarget()
    {
    }
    //-----------------------------------------------------------------------------
    void GLFBOMultiRenderTarget::attachImpl(Nidx idx, TextureFrame *obj)
    {
        /// Check if the render obj is in the rendertarget->FBO map
        auto fbobj = dynamic_cast<GLRenderTarget*>(obj)->getFBO();
        N_assert(fbobj);
        fbo.bindSurface(idx, fbobj->getSurface(0));

        // Set width and height
        mWidth = fbo.getWidth();
        mHeight = fbo.getHeight();
    }
    //-----------------------------------------------------------------------------
    void GLFBOMultiRenderTarget::detachImpl(Nidx idx)
    {
        fbo.unbindSurface(idx);

        // Set width and height
        mWidth = fbo.getWidth();
        mHeight = fbo.getHeight();
    }
    //-----------------------------------------------------------------------------
    bool GLFBOMultiRenderTarget::getPlatformData(const String& name, void *pData ) const
    {
        if( name == GLTextureFrame::CustomAttributeString_FBO )
        {
            *static_cast<GLFrameBufferObject **>(pData) = &fbo;
            return true;
        }
        else if(name == GLTextureFrame::CustomAttributeString_GLCONTEXT)
        {
            *static_cast<GLContext**>(pData) = fbo.getContext();
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool GLFBOMultiRenderTarget::attachExt(ExtFrameBuffer * ebuf)
    {
        bool result;
        if( (result = MultiTextureFrame::attachExt(ebuf)) )
            fbo.attachExt(ebuf);

        return result;
    }
    //-----------------------------------------------------------------------------
    void GLFBOMultiRenderTarget::detachExt()
    {
        fbo.detachExt();
        MultiTextureFrame::detachExt();
    }
    //-----------------------------------------------------------------------------
}
