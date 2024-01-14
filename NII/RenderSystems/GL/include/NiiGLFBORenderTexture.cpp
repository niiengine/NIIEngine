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

#include "NiiGLFBORenderTexture.h"
#include "NiiGLPixelFormat.h"
#include "NiiGLBuffer.h"
#include "NiiGLFBOMultiRenderTarget.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiEngine.h"
#include "NiiGLSupport.h"

namespace NII 
{
    //-----------------------------------------------------------------------------   
    //-----------------------------------------------------------------------------   
    // GLFBORenderTexture
    //-----------------------------------------------------------------------------   
    //-----------------------------------------------------------------------------    
    GLFBORenderTexture::GLFBORenderTexture(GLFBOManager *manager, const String &name,
        const GLSurfaceDesc &target, bool writeGamma, Nui32 fsaa):
        GLTextureFrame(name, target, writeGamma, fsaa),
        mFB(manager, fsaa)
    {
        // Bind target to surface 0 and initialise
        mFB.bindSurface(0, target);
        // Get attributes
        mWidth = mFB.getWidth();
        mHeight = mFB.getHeight();
    }
    //-----------------------------------------------------------------------------
    bool GLFBORenderTexture::getPlatformData(const String& name, void* pData) const
    {
        if( name == GLTextureFrame::CustomAttributeString_FBO )
        {
            *static_cast<GLFrameBufferObject **>(pData) = &mFB;
            return true;
        }
        else if(name == GLTextureFrame::CustomAttributeString_GLCONTEXT)
        {
            *static_cast<GLContext**>(pData) = getContext();
            return true;
        }
        else if (name == _T("GL_FBOID"))
        {
            *static_cast<GLuint*>(pData) = mFB.getGLFBOID();
            return true;
        }
        else if (name == _T("GL_MULTISAMPLEFBOID"))
        {
            *static_cast<GLuint*>(pData) = mFB.getGLMultisampleFBOID();
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    void GLFBORenderTexture::swap()
    {
        mFB.swap();
    }
    //-----------------------------------------------------------------------------
    bool GLFBORenderTexture::attachExt( ExtFrameBuffer *depthBuffer )
    {
        bool result;
        if( (result = GLTextureFrame::attachExt( depthBuffer )) )
            mFB.attachExt( depthBuffer );

        return result;
    }
    //-----------------------------------------------------------------------------
    void GLFBORenderTexture::detachExt()
    {
        mFB.detachExt();
        GLTextureFrame::detachExt();
    }
    //-----------------------------------------------------------------------------
/// Size of probe texture
#define PROBE_SIZE 16

/// Stencil and depth formats to be tried
static const GLenum stencilFormats[] =
{
    GL_NONE,                    // No stencil
    GL_STENCIL_INDEX1_EXT,
    GL_STENCIL_INDEX4_EXT,
    GL_STENCIL_INDEX8_EXT,
    GL_STENCIL_INDEX16_EXT
};
static const Nui8 stencilBits[] =
{
    0, 1, 4, 8, 16
};
#define STENCILFORMAT_COUNT (sizeof(stencilFormats)/sizeof(GLenum))

    static const GLenum depthFormats[] =
        {
            GL_NONE,
            GL_DEPTH_COMPONENT16,
            GL_DEPTH_COMPONENT24,   // Prefer 24 bit depth
            GL_DEPTH_COMPONENT32,
            GL_DEPTH_COMPONENT32F,
            GL_DEPTH24_STENCIL8,    // Packed depth / stencil
            GL_DEPTH32F_STENCIL8
        };
    static const uchar depthBits[] =
        {
            0,16,24,32,32,24,32
        };
#define DEPTHFORMAT_COUNT (sizeof(depthFormats)/sizeof(GLenum))
    //----------------------------------------------------------------------------- 
    //----------------------------------------------------------------------------- 
    // GLFBOManager
    //----------------------------------------------------------------------------- 
    //-----------------------------------------------------------------------------
    GLFBOManager::GLFBOManager(GLRenderSystem* sys):
        mRenderSys(sys)
    {
        detectFBOFormats();
    }
    //-----------------------------------------------------------------------------
    GLFBOManager::~GLFBOManager()
    {
        if(!mRenderBufferMap.empty())
        {
            N_Only(Log).log(_T("GLFBOManager destructor called, but not all renderbuffers were released."));
        }     
    }
    //-----------------------------------------------------------------------------
    void GLFBOManager::_createTempFramebuffer(GLuint internalFormat, GLuint fmt, GLenum dataType, GLuint &fb, GLuint &tid)
    {
        GLCheck(glGenFramebuffers(1, &fb));
        mRenderSys->getStateManager()->bindGLFrameBuffer( GL_DRAW_FRAMEBUFFER, fb );
        if (fmt != GL_NONE)
        {
            if (tid)
            {
                GLCheck(glDeleteTextures(1, &tid));
                mRenderSys->getStateManager()->invalidateStateForTexture( tid );
            }

            // Create and attach texture
            GLCheck(glGenTextures(1, &tid));
            mRenderSys->getStateManager()->bindGLTexture( GL_TEXTURE_2D, tid );

            // Set some default parameters
            GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
            GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
            GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

            GLCheck(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, PROBE_SIZE, PROBE_SIZE, 0, fmt, dataType, 0));

            GLCheck(glFramebufferTexture2D(
                GL_DRAW_FRAMEBUFFER, fmt == GL_DEPTH_COMPONENT ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tid, 0));
        }
        else
        {
            // Draw to nowhere -- stencil/depth only
            GLCheck(glDrawBuffer(GL_NONE));
            GLCheck(glReadBuffer(GL_NONE));
        }
    }
    //-----------------------------------------------------------------------------
    /** Try a certain FBO format, and return the status. Also sets mDepthRB and mStencilRB.
        @return true    if this combo is supported
                 false   if this combo is not supported
    */
    GLuint GLFBOManager::_tryFormat(GLenum depthFormat, GLenum stencilFormat)
    {
        GLuint status, depthRB = 0, stencilRB = 0;

        if (depthFormat != GL_NONE)
        {
            // Generate depth renderbuffer
            GLCheck(glGenRenderbuffers(1, &depthRB));

            // Bind it to FBO
            mRenderSys->getStateManager()->bindGLRenderBuffer( depthRB );

            // Allocate storage for depth buffer
            GLCheck(glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, PROBE_SIZE, PROBE_SIZE));

            // Attach depth
            GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB));
        }

        if (stencilFormat != GL_NONE)
        {
            // Generate stencil renderbuffer
            GLCheck(glGenRenderbuffers(1, &stencilRB));
            // Bind it to FBO
            mRenderSys->getStateManager()->bindGLRenderBuffer( stencilRB );

            // Allocate storage for stencil buffer
            GLCheck(glRenderbufferStorage(GL_RENDERBUFFER, stencilFormat, PROBE_SIZE, PROBE_SIZE));

            // Attach stencil
            GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilRB));
        }

        GLCheck(status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

        // If status is negative, clean up
        // Detach and destroy
        GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
        GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0));

        if (depthRB)
            mRenderSys->getStateManager()->deleteGLRenderBuffer(depthRB);

        if (stencilRB)
            mRenderSys->getStateManager()->deleteGLRenderBuffer(stencilRB);

        return status == GL_FRAMEBUFFER_COMPLETE;
    }
    //-----------------------------------------------------------------------------
    /** Try a certain packed depth/stencil format, and return the status.
        @return true    if this combo is supported
                 false   if this combo is not supported
    */
    bool GLFBOManager::_tryPackedFormat(GLenum packedFormat)
    {
        GLuint packedRB;

        // Generate renderbuffer
        GLCheck(glGenRenderbuffers(1, &packedRB));

        // Bind it to FBO
        mRenderSys->getStateManager()->bindGLRenderBuffer( packedRB );

        // Allocate storage for buffer
        GLCheck(glRenderbufferStorage(GL_RENDERBUFFER, packedFormat, PROBE_SIZE, PROBE_SIZE));

        // Attach depth
        GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, packedRB));

        // Attach stencil
        GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, packedRB));

        GLuint status;
        GLCheck(status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

        // Detach and destroy
        GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
        GLCheck(glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0));
        mRenderSys->getStateManager()->deleteGLRenderBuffer(packedRB);

        return status == GL_FRAMEBUFFER_COMPLETE;
    }
    //-----------------------------------------------------------------------------
    /** Detect which internal formats are allowed as RTT
        Also detect what combinations of stencil and depth are allowed with this internal
        format.
    */
    void GLFBOManager::detectFBOFormats()
    {
        // is glGetInternalformativ supported?
        // core since GL 4.2: see https://www.opengl.org/wiki/GLAPI/glGetInternalformat
        // NOTE! GL_FRAMEBUFFER_RENDERABLE is supported only if the GL version is 4.3 or higher
        bool hasInternalFormatQuery = mRenderSys->hasMinGLVersion(4, 3) || mRenderSys->checkGpuApi("GL_ARB_internalformat_query2");

        // Try all formats, and report which ones work as target
        GLuint fb = 0, tid = 0;

        bool formatSupported;
        GLint params;

        for(int x = 0; x < PF_COUNT; ++x)
        {
            mProps[x].valid = false;

            // Fetch GL format token
            GLenum internalFormat = GLPixelUtil::getGLInternalFormat((PixelFormat)x);
            GLenum originFormat = GLPixelUtil::getGLOriginFormat((PixelFormat)x);
            GLenum dataType = GLPixelUtil::getGLOriginDataType((PixelFormat)x);
            
            if(internalFormat == GL_NONE && x != 0)
                continue;

            // No test for compressed formats
            if(PixelUtil::isCompress((PixelFormat)x))
                continue;

            if (hasInternalFormatQuery) {
                GLCheck(glGetInternalformativ(GL_RENDERBUFFER, internalFormat, GL_FRAMEBUFFER_RENDERABLE, 1, &params));
                formatSupported = params == GL_FULL_SUPPORT;
            } else {
                // Create and attach framebuffer
                _createTempFramebuffer(internalFormat, originFormat, dataType, fb, tid);

                // Check status
                GLuint status;
                GLCheck(status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));
                formatSupported = status == GL_FRAMEBUFFER_COMPLETE;
            }

            // Ignore status in case of fmt==GL_NONE, because no implementation will accept
            // a buffer without *any* attachment. Buffers with only stencil and depth attachment
            // might still be supported, so we must continue probing.
            if(internalFormat == GL_NONE || formatSupported)
            {
                mProps[x].valid = true;
                StringStream str;
                str << "FBO " << PixelUtil::getFormatName((PixelFormat)x)
                    << " depth/stencil support: ";

                // For each depth/stencil formats
                for (uchar depth = 0; depth < DEPTHFORMAT_COUNT; ++depth)
                {
                    if ((depthFormats[depth] != GL_DEPTH24_STENCIL8) && (depthFormats[depth] != GL_DEPTH32F_STENCIL8))
                    {
                        // General depth/stencil combination

                        if(hasInternalFormatQuery) {
                            GLCheck(glGetInternalformativ(GL_RENDERBUFFER, depthFormats[depth], GL_FRAMEBUFFER_RENDERABLE, 1, &params));

                            // skip unsupported depth unless it is GL_NONE, as we still want D0Sxx formats
                            if(params != GL_FULL_SUPPORT && depthFormats[depth] != GL_NONE) {
                                continue;
                            }
                        }

                        for (uchar stencil = 0; stencil < STENCILFORMAT_COUNT; ++stencil)
                        {
                            if (hasInternalFormatQuery) {
                                GLCheck(glGetInternalformativ( GL_RENDERBUFFER, stencilFormats[stencil], GL_FRAMEBUFFER_RENDERABLE, 1, &params));

                                // skip unsupported stencil unless it is GL_NONE, as we still want DxxS0 formats
                                formatSupported = params == GL_FULL_SUPPORT || stencilFormats[stencil] == GL_NONE;
                            } else {
                                formatSupported = _tryFormat(depthFormats[depth], stencilFormats[stencil]) != 0;
                            }

                            if (formatSupported)
                            {
                                // Add mode to allowed modes
                                str << StringUtil::format("D%dS%d ", depthBits[depth], stencilBits[stencil]);
                                FormatProperties::Mode mode = {depth, stencil};
                                mProps[x].modes.push_back(mode);
                            }
                        }
                    }
                    else
                    {
                        // Packed depth/stencil format
                        if (hasInternalFormatQuery) 
                        {
                            GLCheck(glGetInternalformativ( GL_RENDERBUFFER, depthFormats[depth], GL_FRAMEBUFFER_RENDERABLE, 1, &params));

                            formatSupported = params == GL_FULL_SUPPORT;
                        } 
                        else 
                        {
                            formatSupported = _tryPackedFormat(depthFormats[depth]);
                        }

                        if (formatSupported)
                        {
                            // Add mode to allowed modes
                            str << "Packed-D" << int(depthBits[depth]) << "S8 ";
                            FormatProperties::Mode mode = {depth, 0}; // stencil unused
                            mProps[x].modes.push_back(mode);
                        }
                    }
                }
                N_log(str.str());
            }

            if (!hasInternalFormatQuery) {
                // Delete texture and framebuffer
                mRenderSys->getStateManager()->bindGLFrameBuffer( GL_DRAW_FRAMEBUFFER, 0 );
                mRenderSys->getStateManager()->deleteGLRenderBuffer(fb);

                if (internalFormat != GL_NONE) 
                {
                    GLCheck(glDeleteTextures(1, &tid));
                    mRenderSys->getStateManager()->invalidateStateForTexture( tid );
                    tid = 0;
                }
            }
        }

        String fmtstring = "";
        for(size_t x = 0; x < PF_COUNT; ++x)
        {
            if(mProps[x].valid)
                fmtstring += PixelUtil::getFormatName((PixelFormat)x)+" ";
        }
        N_log("[GL] : Valid FBO targets " + fmtstring);
    }
    //-----------------------------------------------------------------------------
    void GLFBOManager::getBestDepthStencil(PixelFormat internalFormat, Nui32 *depthFormat, Nui32 *stencilFormat)
    {
        const FormatProperties &props = mProps[internalFormat];
        /// Decide what stencil and depth formats to use
        /// [best supported for internal format]
        size_t bestmode=0;
        int bestscore=-1;
        bool requestDepthOnly = PixelUtil::isDepth(internalFormat);
        for(size_t mode=0; mode<props.modes.size(); mode++)
        {
            int desirability = 0;
            /// Find most desirable mode
            /// desirability == 0            if no depth, no stencil
            /// desirability == 1000...2000  if no depth, stencil
            /// desirability == 2000...3000  if depth, no stencil
            /// desirability == 3000+        if depth and stencil
            /// beyond this, the total numer of bits (stencil+depth) is maximised
            if(props.modes[mode].stencil && !requestDepthOnly)
                desirability += 1000;
            if(props.modes[mode].depth)
                desirability += 2000;
            if(depthBits[props.modes[mode].depth]==24) // Prefer 24 bit for now
                desirability += 500;
            if((depthFormats[props.modes[mode].depth]==GL_DEPTH24_STENCIL8 || depthFormats[props.modes[mode].depth]==GL_DEPTH32F_STENCIL8) && !requestDepthOnly) // Prefer 24/8 packed
                desirability += 5000;
            desirability += stencilBits[props.modes[mode].stencil] + depthBits[props.modes[mode].depth];
            
            if(desirability>bestscore)
            {
                bestscore = desirability;
                bestmode = mode;
            }
        }
        *depthFormat = depthFormats[props.modes[bestmode].depth];
        *stencilFormat = requestDepthOnly ? 0 : stencilFormats[props.modes[bestmode].stencil];
    }
    //-----------------------------------------------------------------------------
    TextureFrame *GLFBOManager::createRenderTexture(const String &name, const GLSurfaceDesc &target, bool writeGamma, Nui32 fsaa)
    {
        return new GLFBORenderTexture(this, name, target, writeGamma, fsaa);;
    }
    //---------------------------------------------------------------------
    void GLFBOManager::bind(FrameObj *obj)
    {
        /// Check if the render target is in the rendertarget->FBO map
        if(auto fbo = dynamic_cast<GLRenderTarget*>(obj)->getFBO())
            fbo->bind(true);
        else
            // Old style context (window/pbuffer) or copying render texture
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    //-----------------------------------------------------------------------------
    GLSurfaceDesc GLFBOManager::requestRenderBuffer(GLenum format, Nui32 width, Nui32 height, Nui32 fsaa)
    {
        GLSurfaceDesc retval;
        retval.buffer = 0; // Return 0 buffer if GL_NONE is requested
        if(format != GL_NONE)
        {
            RBFormat key(format, width, height, fsaa);
            RenderBufferMap::iterator it = mRenderBufferMap.find(key);
            if(it != mRenderBufferMap.end())
            {
                retval.buffer = it->second.buffer;
                retval.zoffset = 0;
                retval.numSamples = fsaa;
                // Increase refcount
                ++it->second.refcount;
            }
            else
            {
                // New one
                GLRenderBuffer *rb = new GLRenderBuffer(format, width, height, fsaa);
                mRenderBufferMap[key] = RBRef(rb);
                retval.buffer = rb;
                retval.zoffset = 0;
                retval.numSamples = fsaa;
            }
        }
        return retval;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GLCopyingRenderTexture
    //-----------------------------------------------------------------------------  
    //-----------------------------------------------------------------------------  
    GLCopyingRenderTexture::GLCopyingRenderTexture(GLCopyingRTTManager * manager, 
        const String &name, const GLSurfaceDesc &target, bool writeGamma, Nui32 fsaa):
        GLTextureFrame(name, target, writeGamma, fsaa)
    {
    }
    //-----------------------------------------------------------------------------  
    bool GLCopyingRenderTexture::getPlatformData(const String& name, void* pData) const
    {
        if( name == GLTextureFrame::CustomAttributeString_TARGET )
        {
            GLSurfaceDesc &target = *static_cast<GLSurfaceDesc*>(pData);
            target.buffer = static_cast<GLFrameBuffer*>(mBuffer);
            target.zoffset = mZOft;
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------  
    //-----------------------------------------------------------------------------  
    // GLCopyingRTTManager
    //-----------------------------------------------------------------------------  
    //-----------------------------------------------------------------------------  
    void GLCopyingRTTManager::unbind(FrameObj * target)
    {
        // Copy on unbind
        GLSurfaceDesc surface;
        surface.buffer = 0;
        target->getPlatformData(GLTextureFrame::CustomAttributeString_TARGET, &surface);
        if(surface.buffer)
            static_cast<GLTextureBuffer1*>(surface.buffer)->copyFromFramebuffer(surface.zoffset);
    }
    //-----------------------------------------------------------------------------  
    //-----------------------------------------------------------------------------  
    // GLPBRenderTexture
    //-----------------------------------------------------------------------------  
    //-----------------------------------------------------------------------------  
    GLPBRenderTexture::GLPBRenderTexture(GLPBRTTManager * manager, const String &name, 
        const GLSurfaceDesc & target, bool writeGamma, Nui32 fsaa):
        GLTextureFrame(name, target, writeGamma, fsaa),
        mManager(manager)
    {
        mPBFormat = PixelUtil::getExentMark(target.buffer->getFormat());
        
        mManager->requestPBuffer(mPBFormat, mWidth, mHeight);
    }
    //-----------------------------------------------------------------------------  
    GLPBRenderTexture::~GLPBRenderTexture()
    {
        // Release PBuffer
        mManager->releasePBuffer(mPBFormat);
    }
    //-----------------------------------------------------------------------------  
    bool GLPBRenderTexture::getPlatformData(const String& name, void* pData) const
    {
        if( name == GLTextureFrame::CustomAttributeString_TARGET )
        {
            GLSurfaceDesc &target = *static_cast<GLSurfaceDesc*>(pData);
            target.buffer = static_cast<GLFrameBuffer*>(mBuffer);
            target.zoffset = mZOft;
            return true;
        }
        else if (name == GLTextureFrame::CustomAttributeString_GLCONTEXT )
        {
            // Get PBuffer for our internal format
            *static_cast<GLContext**>(pData) = getContext();
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------  
    GLContext* GLPBRenderTexture::getContext() const
    {
        return mManager->getContextFor(mPBFormat, mWidth, mHeight);
    }
    //----------------------------------------------------------------------------- 
    //----------------------------------------------------------------------------- 
    // GLPBRTTManager
    //----------------------------------------------------------------------------- 
    //-----------------------------------------------------------------------------  
    GLPBRTTManager::GLPBRTTManager(GLSupport *support, FrameObj *mainwindow):
        mSupport(support),
        mMainWindow(mainwindow),
        mMainContext(0)
    {
        mMainContext = dynamic_cast<GLRenderTarget*>(mMainWindow)->getContext();
    }
    //-----------------------------------------------------------------------------  
    GLPBRTTManager::~GLPBRTTManager()
    {
        // Delete remaining PBuffers
        for(size_t x = 0; x < 16; ++x)
        {
            delete mPBuffers[x].pb;
        }
    }
    //-----------------------------------------------------------------------------
    TextureFrame * GLPBRTTManager::createRenderTexture(const String & name,
        const GLSurfaceDesc & target, bool writeGamma, Nui32 fsaa)
    {
        return new GLPBRenderTexture(this, name, target, writeGamma, fsaa);
    }
    //-----------------------------------------------------------------------------
    bool GLPBRTTManager::checkFormat(PixelFormat format) 
    { 
        return true; 
    }
    //-----------------------------------------------------------------------------
    void GLPBRTTManager::bind(FrameObj *target)
    {
        // Nothing to do here
        // Binding of context is done by GL subsystem, as contexts are also used for RenderWindows
    }
    //-----------------------------------------------------------------------------
    void GLPBRTTManager::unbind(FrameObj *target)
    { 
        // Copy on unbind
        GLSurfaceDesc surface;
        surface.buffer = 0;
        target->getPlatformData(GLTextureFrame::CustomAttributeString_TARGET, &surface);
        if(surface.buffer)
            static_cast<GLTextureBuffer1*>(surface.buffer)->copyFromFramebuffer(surface.zoffset);
    }
    //-----------------------------------------------------------------------------
    void GLPBRTTManager::requestPBuffer(PixelDataType ctype, Nui32 width, Nui32 height)
    {
        //Check size
        if(mPBuffers[ctype].pb)
        {
            if(mPBuffers[ctype].pb->getWidth()<width || mPBuffers[ctype].pb->getHeight()<height)
            {
                // If the current PBuffer is too small, destroy it and create a new one
                delete mPBuffers[ctype].pb;
                mPBuffers[ctype].pb = 0;
            }
        }
        if(!mPBuffers[ctype].pb)
        {
            // Create pbuffer via rendersystem
            mPBuffers[ctype].pb = mSupport->createPBuffer(ctype, width, height);
        }
        
        ++mPBuffers[ctype].refcount;
    }
    //-----------------------------------------------------------------------------
    void GLPBRTTManager::releasePBuffer(PixelDataType ctype)
    {
        --mPBuffers[ctype].refcount;
        if(mPBuffers[ctype].refcount == 0)
        {
            delete mPBuffers[ctype].pb;
            mPBuffers[ctype].pb = 0;
        }
    }
    //-----------------------------------------------------------------------------
    GLContext *GLPBRTTManager::getContextFor(PixelDataType ctype, Nui32 width, Nui32 height)
    {
        // Faster to return main context if the RTT is smaller than the window size
        // and ctype is PCT_BYTE. This must be checked every time because the window might have been resized
        if(ctype == PFF_Int)
        {
            if(width <= mMainWindow->getWidth() && height <= mMainWindow->getHeight())
                return mMainContext;
        }
        N_assert(mPBuffers[ctype].pb);
        return mPBuffers[ctype].pb->getContext();
    }
    //-----------------------------------------------------------------------------
}
