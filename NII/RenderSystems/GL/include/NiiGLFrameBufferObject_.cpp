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

#include "NiiGLFrameBufferObject.h"
#include "NiiGLPixelFormat.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiEngine.h"
#include "NiiGLHardwarePixelBuffer.h"
#include "NiiGLFBORenderTexture.h"
#include "NiiGLDepthBuffer.h"
#include "NiiGLRenderSystemBase.h"

namespace NII 
{
//-----------------------------------------------------------------------------
    GLFrameBufferObject::GLFrameBufferObject(GLFBOManager * mag, Nui32 fsaa):
        GLFrameBufferObjectBase(fsaa), 
        mManager(mag)
    {
        // Generate framebuffer object
        GLCheck(glGenFramebuffers(1, &mFB));
        
        mManager->getStateManager()->bindGLFrameBuffer( GL_FRAMEBUFFER, mFB );
        
        // check multisampling
        if (GLEW_EXT_framebuffer_blit && GLEW_EXT_framebuffer_multisample)
        {
            // check samples supported
            GLint maxSamples;
            GLCheck(glGetIntegerv(GL_MAX_SAMPLES, &maxSamples));
            mNumSamples = N_MIN(Nui32, mNumSamples, (Nui32)maxSamples);
        }
        else
        {
            mNumSamples = 0;
        }
        // will we need a second FBO to do multisampling?
        if (mNumSamples)
        {
            glGenFramebuffers(1, &mMultisampleFB);
        }
        else
        {
            mMultisampleFB = 0;
        }
    }
    //-----------------------------------------------------------------------------
    GLFrameBufferObject::~GLFrameBufferObject()
    {
        mManager->releaseRenderBuffer(mDepth);
        mManager->releaseRenderBuffer(mStencil);
        mManager->releaseRenderBuffer(mMultisampleColourBuffer);
        // Delete framebuffer object
        if(mContext && mFB)
        {
            GLRenderSystemBase* rs = static_cast<GLRenderSystemBase*>(N_Engine().getRender());
            rs->_destroyFbo(mContext, mFB);
            
            if (mMultisampleFB)
                rs->_destroyFbo(mContext, mMultisampleFB);
        }
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferObject::initialise()
    {
        N_assert(mContext == (static_cast<GLRenderSystemCommon*>(N_Engine().getRender()))->_getCurrentContext());
        
        // Release depth and stencil, if they were bound
        mManager->releaseRenderBuffer(mDepth);
        mManager->releaseRenderBuffer(mStencil);
        mManager->releaseRenderBuffer(mMultisampleColourBuffer);
        // First buffer must be bound
        if(!mColour[0].buffer)
        {
            N_EXCEPT(Internal, _T("Attachment 0 must have surface attached"));
        }

        // If we're doing multisampling, then we need another FBO which contains a
        // renderbuffer which is set up to multisample, and we'll blit it to the final 
        // FBO afterwards to perform the multisample resolve. In that case, the 
        // mMultisampleFB is bound during rendering and is the one with a depth/stencil

        // Store basic stats
        Nui32 width = mColour[0].buffer->getWidth();
        Nui32 height = mColour[0].buffer->getHeight();
        GLuint format = mColour[0].buffer->getGLFormat();

        auto rsc = N_Engine().getRender()->getFeature();
        Nui16 maxSupportedMRTs = rsc->getMaxMultiFrameObj();

        // Bind simple buffer to add colour attachments
        mManager->getStateManager()->bindGLFrameBuffer( GL_FRAMEBUFFER, mFB );

        bool isDepth = PixelUtil::isDepth(getFormat());

        // Bind all attachment points to frame buffer
        for(unsigned int x=0; x<maxSupportedMRTs; ++x)
        {
            if(mColour[x].buffer)
            {
                if(mColour[x].buffer->getWidth() != width || mColour[x].buffer->getHeight() != height)
                {
                    StringStream ss;
                    ss << "Attachment " << x << " has incompatible size ";
                    ss << mColour[x].buffer->getWidth() << "x" << mColour[x].buffer->getHeight();
                    ss << ". It must be of the same as the size of surface 0, ";
                    ss << width << "x" << height;
                    ss << ".";
                    N_EXCEPT(InvalidParam, ss.str());
                }

                mColour[x].buffer->bindToFramebuffer(
                    isDepth ? GL_DEPTH_ATTACHMENT : (GL_COLOR_ATTACHMENT0 + x), mColour[x].zoffset);
            }
            else
            {
                // Detach
                GLCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+x, GL_RENDERBUFFER, 0));
            }
        }

        // Now deal with depth / stencil
        if (mMultisampleFB)
        {
            // Bind multisample buffer
            mManager->getStateManager()->bindGLFrameBuffer( GL_FRAMEBUFFER, mMultisampleFB );

            // Create AA render buffer (colour)
            // note, this can be shared too because we blit it to the final FBO
            // right after the render is finished
            mMultisampleColourBuffer = mManager->requestRenderBuffer(format, width, height, mNumSamples);

            // Attach it, because we won't be attaching below and non-multisample has
            // actually been attached to other FBO
            mMultisampleColourBuffer.buffer->bindToFramebuffer(GL_COLOR_ATTACHMENT0, mMultisampleColourBuffer.zoffset);
            // depth & stencil will be dealt with below

        }

        // Depth buffer is not handled here anymore.
        // See GLFrameBufferObject::attachDepthBuffer() & RenderSystem::setDepthBufferFor()

        // Do glDrawBuffer calls
        GLenum bufs[NII_MAX_RenderTarget];
        GLsizei n=0;
        for(unsigned int x = 0; x < NII_MAX_RenderTarget; ++x)
        {
            // Fill attached colour buffers
            if(mColour[x].buffer)
            {
                bufs[x] = isDepth ? GL_DEPTH_ATTACHMENT : (GL_COLOR_ATTACHMENT0 + x);
                // Keep highest used buffer + 1
                n = x+1;
            }
            else
            {
                bufs[x] = GL_NONE;
            }
        }

        if(!isDepth)
            GLCheck(glDrawBuffers(n, bufs));

        if (mMultisampleFB)
        {
            // we need a read buffer because we'll be blitting to mFB
            GLCheck(glReadBuffer(bufs[0]));
        }
        else
        {
            // No read buffer, by default, if we want to read anyway we must not forget to set this.
            GLCheck(glReadBuffer(GL_NONE));
        }
        
        // Check status
        GLuint status;
        GLCheck(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
        
        // Bind main buffer
        mManager->getStateManager()->bindGLFrameBuffer( GL_FRAMEBUFFER, 0 );
        
        switch(status)
        {
        case GL_FRAMEBUFFER_COMPLETE:
            // All is good
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            N_EXCEPT(InvalidParam, _T("All framebuffer formats with this texture internal format unsupported"));
        default:
            N_EXCEPT(InvalidParam, _T("Framebuffer incomplete or other FBO status error"));
        }
    }
    //-----------------------------------------------------------------------------
    bool GLFrameBufferObject::bind(bool recreateIfNeeded)
    {
        GLRenderSystemBase* rs = static_cast<GLRenderSystemBase*>(N_Engine().getRender());
        GLContext * currentContext = rs->_getCurrentContext();
        if(mContext && mContext != currentContext) // FBO is unusable with current context, destroy it
        {
            if(mFB != 0)
                rs->_destroyFbo(mContext, mFB);
            if(mMultisampleFB != 0)
                rs->_destroyFbo(mContext, mMultisampleFB);
            
            mContext = 0;
            mFB = 0;
            mMultisampleFB = 0;
        }

        if(!mContext && recreateIfNeeded) // create FBO lazy or recreate after destruction
        {
            mContext = currentContext;
            
            // Generate framebuffer object
            GLCheck(glGenFramebuffers(1, &mFB));
            
            // Check samples supported
            mManager->getStateManager()->bindGLFrameBuffer( GL_FRAMEBUFFER, mFB );
            
            GLint maxSamples;
            GLCheck(glGetIntegerv(GL_MAX_SAMPLES, &maxSamples));
            mNumSamples = std::min(mNumSamples, (GLsizei)maxSamples);
            
            // Will we need a second FBO to do multisampling?
            if (mNumSamples)
            {
                GLCheck(glGenFramebuffers(1, &mMultisampleFB));
            }
            else
            {
                mMultisampleFB = 0;
            }
            
            // Re-initialise
            if(mColour[0].buffer)
                initialise();
        }

        if(mContext)
	        mManager->getStateManager()->bindGLFrameBuffer(GL_FRAMEBUFFER, mMultisampleFB ? mMultisampleFB : mFB);

        return mContext != 0;
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferObject::swap()
    {
        if (mMultisampleFB)
        {
            GLint oldfb = 0;
            GLCheck(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldfb));

            // Blit from multisample buffer to final buffer, triggers resolve
            uint32 width = mColour[0].buffer->getWidth();
            uint32 height = mColour[0].buffer->getHeight();
            mManager->getStateManager()->bindGLFrameBuffer( GL_READ_FRAMEBUFFER, mMultisampleFB );
            mManager->getStateManager()->bindGLFrameBuffer( GL_DRAW_FRAMEBUFFER, mFB );

            GLCheck(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
            // Unbind
            mManager->getStateManager()->bindGLFrameBuffer( GL_FRAMEBUFFER, oldfb );
        }
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferObject::attachExt(ExtFrameBuffer * extbuf)
    {
        bind(true); // recreate FBO if unusable with current context, bind it

        GLDepthBuffer *glDepthBuffer = static_cast<GLDepthBuffer*>(extbuf);
        if( glDepthBuffer )
        {
            auto *depthBuf   = glDepthBuffer->getDepthBuffer();
            auto *stencilBuf = glDepthBuffer->getStencilBuffer();

            // Attach depth buffer, if it has one.
            if( depthBuf )
                depthBuf->bindToFramebuffer( GL_DEPTH_ATTACHMENT, 0 );
            // Attach stencil buffer, if it has one.
            if( stencilBuf )
                stencilBuf->bindToFramebuffer( GL_STENCIL_ATTACHMENT, 0 );
        }
        else
        {
            GLCheck(glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
            GLCheck(glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0));
        }
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferObject::detachExt()
    {
        if(bind(false)) // bind or destroy FBO if it is unusable with current context
        {
            GLCheck(glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0 ));
            GLCheck(glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0 ));
        }
    }
    //-----------------------------------------------------------------------------
}
