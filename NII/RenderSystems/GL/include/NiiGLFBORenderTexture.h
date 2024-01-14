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
#ifndef _NII_GLFBORTT_H__
#define _NII_GLFBORTT_H__

#include "NiiGLRenderTexture.h"
#include "NiiGLContext.h"
#include "NiiGLFrameBufferObject.h"
#include "NiiGLPBuffer.h"

/// Extra GL constants
#define GL_DEPTH24_STENCIL8_EXT         0x88F0

namespace NII 
{
    class GLFBOManager;

    /** RenderTexture for GL FBO
    */
    class _EngineGLAPI GLFBORenderTexture: public GLTextureFrame
    {
    public:
        GLFBORenderTexture(GLFBOManager * manager, const String & name, const GLSurfaceDesc &target, bool writeGamma, Nui32 fsaa);

        /// @copydetails TextureFrame::getPlatformData
        virtual bool getPlatformData(const String& name, void* pData) const;

        /// @copydetails TextureFrame::attachExt
        virtual bool attachExt(ExtFrameBuffer * ebuf);

        /// @copydetails TextureFrame::detachExt
        virtual void detachExt();

        /// @copydetails FrameObj::swap
        virtual void swap();

        ///@copydetails GLRenderTarget::getContext
        GLContext * getContext() const          { return mFB.getContext(); }

        ///@copydetails GLRenderTarget::getFBO
        GLFrameBufferObjectBase* getFBO() const { return &mFB; }
    protected:
        mutable GLFrameBufferObject mFB;
    };
    
    /** Factory for GL Frame Buffer Objects, and related things.
    */
    class _EngineGLAPI GLFBOManager: public GLRTTManager
    {
    public:
        GLFBOManager(GLRenderSystem* sys);
        ~GLFBOManager();
        
        ///@copydetails GLRTTManager::bind
        void bind(FrameObj * obj);
        
        ///@copydetails GLRTTManager::unbind
        void unbind(FrameObj * obj) {};
        
        ///@copydetails GLRTTManager::getBestDepthStencil
        void getBestDepthStencil(PixelFormat interpf, Nui32 * depthpf, Nui32 * stencilpf);
        
        ///@copydetails GLRTTManager::createRenderTexture
        virtual TextureFrame * createRenderTexture(const String &name, const GLSurfaceDesc & target, bool writeGamma, Nui32 fsaa);
        
        /** Request a render buffer. If format is GL_NONE, return a zero buffer.
        */
        GLSurfaceDesc requestRenderBuffer(GLenum format, Nui32 width, Nui32 height, Nui32 fsaa);
        
        GLStateCacheManager * getStateManager()     { return mRenderSys->getStateManager(); }
    private:
        /** Detect allowed FBO formats */
        void detectFBOFormats();
        GLuint _tryFormat(GLenum depthFormat, GLenum stencilFormat);
        bool _tryPackedFormat(GLenum packedFormat);
        void _createTempFramebuffer(GLuint internalfmt, GLuint fmt, GLenum type, GLuint &fb, GLuint &tid);

        GLRenderSystem* mRenderSys;
    };
    
    /** RenderTexture for simple copying from frame buffer
    */
    class GLCopyingRTTManager;
    class _EngineGLAPI GLCopyingRenderTexture: public GLTextureFrame
    {
    public:
        GLCopyingRenderTexture(GLCopyingRTTManager * mag, const String &name, const GLSurfaceDesc &target, 
            bool writeGamma, Nui32 fsaa);
        
        /// @copydetails TextureFrame::getPlatformData
        bool getPlatformData(const String & name, void* pData) const;

        /// @copydetails GLRenderTarget::getContext
        GLContext* getContext() const { return NULL; }
    };
    
    /** Simple, copying manager/factory for RenderTextures. This is only used as the last fallback if
        both PBuffers and FBOs aren't supported.
    */
    class _EngineGLAPI GLCopyingRTTManager: public GLRTTManager
    {
    public:
        /// @copydetails GLRTTManager
        TextureFrame *createRenderTexture(const String &name, const GLSurfaceDesc &target, bool writeGamma, Nui32 fsaa)
        {
            return new GLCopyingRenderTexture(this, name, target, writeGamma, fsaa);
        }

        /// @copydetails GLRTTManager::checkFormat
        bool checkFormat(PixelFormat format) { return true; }
        
        /// @copydetails GLRTTManager::bind
        void bind(FrameObj * obj) {}

        /// @copydetails GLRTTManager::unbind
        void unbind(FrameObj * obj);
    };
    
    /** RenderTexture that uses a PBuffer (offscreen rendering context) for rendering.
    */
    class GLPBRTTManager;
    class _EngineGLAPI GLPBRenderTexture: public GLTextureFrame
    {
    public:
        GLPBRenderTexture(GLPBRTTManager * mag, const String &name, const GLSurfaceDesc &target, bool writeGamma, Nui32 fsaa);
        virtual ~GLPBRenderTexture();
        
        /// @copydetails TextureFrame::getPlatformData
        bool getPlatformData(const String& name, void* pData) const;

        /// @copydetails GLRenderTarget::getContext
        GLContext* getContext() const;
    protected:
        GLPBRTTManager * mManager;
        PixelDataType mPBFormat;
    };
        
    /** Manager for rendertextures and PBuffers (offscreen rendering contexts)
    */
    class _EngineGLAPI GLPBRTTManager: public GLRTTManager
    {
    public:
        GLPBRTTManager(GLSupport *support, FrameObj *mainwindow);
        virtual ~GLPBRTTManager();
        
        /// @copydetails GLRTTManager::createRenderTexture
        virtual TextureFrame * createRenderTexture(const String &name, const GLSurfaceDesc &target,
            bool writeGamma, Nui32 fsaa);
        
        /// @copydetails GLRTTManager::checkFormat
        virtual bool checkFormat(PixelFormat format);
        
        /// @copydetails GLRTTManager::bind
        virtual void bind(FrameObj *target);
        
        /// @copydetails GLRTTManager::unbind
        virtual void unbind(FrameObj *target);
        
        /** Create PBuffer for a certain pixel format and size
        */
        void requestPBuffer(PixelDataType ctype, Nui32 width, Nui32 height);
        
        /** Release PBuffer for a certain pixel format
        */
        void releasePBuffer(PixelDataType ctype);
        
        /** Get GL rendering context for a certain component type and size.
        */
        GLContext * getContextFor(PixelDataType ctype, Nui32 width, Nui32 height);
    protected:
        /** GLSupport reference, used to create PBuffers */
        GLSupport * mSupport;
        /** Primary window reference */
        FrameObj * mMainWindow;
        /** Primary window context */
        GLContext *mMainContext;
        /** Reference to a PBuffer, with refcount */
        struct PBRef
        {
            PBRef(): pb(0),refcount(0) {}
            GLPBuffer * pb;
            size_t refcount;
        };
        /** Type to map each component type to a PBuffer */
        PBRef mPBuffers[16];
    };
}
#endif