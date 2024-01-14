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

#include "OgreGLES2FBOMultiRenderTarget.h"
#include "OgreLogManager.h"

namespace Ogre {
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GLES2FBOMultiRenderTarget
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLES2FBOMultiRenderTarget::GLES2FBOMultiRenderTarget(GLES2FBOManager *manager, const String &name):
        MultiRenderTarget(name),
        fbo(manager, 0 /* TODO: multisampling on MRTs? */)
    {
    }
    //-----------------------------------------------------------------------------
    GLES2FBOMultiRenderTarget::~GLES2FBOMultiRenderTarget()
    {
    }
    //-----------------------------------------------------------------------------
    void GLES2FBOMultiRenderTarget::bindSurfaceImpl(size_t attachment, RenderTexture *target)
    {
        /// Check if the render target is in the rendertarget->FBO map
        auto *fbobj = dynamic_cast<GLRenderTarget*>(target)->getFBO();
        N_assert(fbobj);
        fbo.bindSurface(attachment, fbobj->getSurface(0));

        // Set width and height
        mWidth = fbo.getWidth();
        mHeight = fbo.getHeight();
    }
    //-----------------------------------------------------------------------------
    void GLES2FBOMultiRenderTarget::unbindSurfaceImpl(size_t attachment)
    {
        fbo.unbindSurface(attachment);

        // Set width and height
        mWidth = fbo.getWidth();
        mHeight = fbo.getHeight();
    }
    //-----------------------------------------------------------------------------
    void GLES2FBOMultiRenderTarget::getPlatformData( const String& name, void *pData )
    {
        if(name == GLTextureFrame::CustomAttributeString_FBO)
        {
            *static_cast<GLES2FrameBufferObject **>(pData) = &fbo;
        }
        else if(name == GLTextureFrame::CustomAttributeString_GLCONTEXT)
        {
            *static_cast<GLContext**>(pData) = fbo.getContext();
        }
    }
    //-----------------------------------------------------------------------------
    bool GLES2FBOMultiRenderTarget::attachDepthBuffer( DepthBuffer *depthBuffer )
    {
        bool result;
        if( (result = MultiRenderTarget::attachDepthBuffer( depthBuffer )) )
            fbo.attachDepthBuffer( depthBuffer );

        return result;
    }
    //-----------------------------------------------------------------------------
    void GLES2FBOMultiRenderTarget::detachDepthBuffer()
    {
        fbo.detachDepthBuffer();
        MultiRenderTarget::detachDepthBuffer();
    }
    //-----------------------------------------------------------------------------
    void GLES2FBOMultiRenderTarget::_detachDepthBuffer()
    {
        fbo.detachDepthBuffer();
        MultiRenderTarget::_detachDepthBuffer();
    }
}
