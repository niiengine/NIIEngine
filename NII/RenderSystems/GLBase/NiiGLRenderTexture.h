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

#ifndef _NII_GLRENDERTEXTURE_H_
#define _NII_GLRENDERTEXTURE_H_

#include "NiiGLPreInclude.h"
#include "NiiGLCommon.h"
#include "NiiTextureFrame.h"
#include "NiiSingleton.h"

namespace NII
{
    class GLFrameBufferBase;

    /** GL surface descriptor. Points to a 2D surface that can be rendered to.
     */
    struct _EngineInner GLSurfaceDesc
    {
    public:
        GLSurfaceDesc() : buffer(0), zoffset(0), numSamples(0) {}

        GLFrameBufferBase * buffer;
        Nui32 zoffset;
        Nui32 numSamples;
    };

    /// Frame Buffer Object abstraction
    class _EngineGLAPI GLFrameBufferObjectBase
    {
    public:
        GLFrameBufferObjectBase(Nui32 fsaa);
        virtual ~GLFrameBufferObjectBase() {}

        /// Get the GL id for the FBO
        inline Nui32 getGLFBOID() const             { return mFB; }

        /// Get the GL id for the multisample FBO
        inline Nui32 getGLMultisampleFBOID() const  { return mMultisampleFB; }

        inline GLContext * getContext() const       { return mContext; }

        virtual bool bind(bool recreateIfNeeded) = 0;

        /** Bind a surface to a certain attachment point.
            attachment: 0..NII_MAX_RenderTarget-1
        */
        void bindSurface(NCount attachment, const GLSurfaceDesc & target);

        void unbindSurface(NCount attachment);

        inline const GLSurfaceDesc & getSurface(NCount attachment) const { return mColour[attachment]; }

        /// Accessors
        inline Nui32 getFSAA() const                { return mNumSamples; }
        inline Nui32 getWidth() const               { N_assert(mColour[0].buffer); return mColour[0].buffer->getWidth(); }
        inline Nui32 getHeight() const              { N_assert(mColour[0].buffer); return mColour[0].buffer->getHeight(); }
        inline PixelFormat getFormat() const        { N_assert(mColour[0].buffer); return mColour[0].buffer->getFormat(); }

        inline void notifyContextDestroyed(GLContext * context) 
        { 
            if(mContext == context) 
            { 
                mContext = 0; mFB = 0; mMultisampleFB = 0; 
            } 
        }
    protected:
        /** Initialise object (find suitable depth and stencil format).
            Must be called every time the bindings change.
            It fails with an exception (ERR_INVALIDPARAMS) if:
            - Attachment point 0 has no binding
            - Not all bound surfaces have the same size
            - Not all bound surfaces have the same internal format
        */
        virtual void initialise() = 0;
    protected:
        /// Context that was used to create FBO. It could already be destroyed, so do not dereference this field blindly
        GLContext* mContext;
        GLSurfaceDesc mDepth;
        GLSurfaceDesc mStencil;
        // Arbitrary number of texture surfaces
        GLSurfaceDesc mColour[NII_MAX_RenderTarget];
        Nui32 mFB;
        Nui32 mMultisampleFB;
        Nui32 mNumSamples;
    };

    /** Base class for GL Render Textures
     */
    class _EngineGLAPI GLTextureFrame : public TextureFrame, public GLRenderTarget
    {
    public:
        GLTextureFrame(const String & name, const GLSurfaceDesc & target, bool writeGamma, Nui32 fsaa);

        /// @copydetails TextureFrame::isFlipping
        bool isFlipping() const { return true; }

        static const String CustomAttributeString_FBO;
        static const String CustomAttributeString_TARGET;
        static const String CustomAttributeString_GLCONTEXT;
    };

    /** Manager/factory for RenderTextures.
     */
    class _EngineGLAPI GLRTTManager : public Singleton<GLRTTManager> , public GPUAlloc
    {
    public:
        GLRTTManager();
        virtual ~GLRTTManager();

        /** Create a texture rendertarget object
         */
        virtual TextureFrame * createRenderTexture(const String &name, const GLSurfaceDesc &target,
            bool writeGamma, Nui32 fsaa) = 0;

        /** Release a render buffer. Ignore silently if surface.buffer is 0.
         */
        void releaseRenderBuffer(const GLSurfaceDesc &surface);

        /** Check if a certain format is usable as FBO rendertarget format
        */
        virtual bool checkFormat(PixelFormat format) { return mProps[format].valid; }

        /** Bind a certain render target.
            @note only needed for FBO RTTs
         */
        virtual void bind(FrameObj *target) {}

        /** Unbind a certain render target. This is called before binding another FrameObj, and
            before the context is switched. It can be used to do a copy, or just be a noop if direct
            binding is used.
            @note only needed for Copying or PBuffer RTTs
        */
        virtual void unbind(FrameObj * target) {}

        virtual void getBestDepthStencil(PixelFormat interpf, Nui32 *depthpf, Nui32 *stencilpf)
        {
            *depthpf = 0;
            *stencilpf = 0;
        }

        /** Get the closest supported alternative format. If format is supported, returns format.
         */
        PixelFormat getSupportedAlternative(PixelFormat format);
    protected:
        /** Frame Buffer Object properties for a certain texture format.
         */
        struct FormatProperties
        {
            bool valid; // This format can be used as RTT (FBO)

            /** Allowed modes/properties for this pixel format
             */
            struct Mode
            {
                Nui8 depth;     // Depth format (0=no depth)
                Nui8 stencil;   // Stencil format (0=no stencil)
            };

            vector<Mode>::type modes;
        };
        /** Properties for all internal formats defined by OGRE
         */
        FormatProperties mProps[PF_COUNT];

        /** Stencil and depth renderbuffers of the same format are re-used between surfaces of the
            same size and format. This can save a lot of memory when a large amount of rendertargets
            are used.
        */
        struct RBFormat
        {
            RBFormat(Nui32 inFormat, Nui32 inWidth, Nui32 inHeight, Nui32 fsaa)
                : format(inFormat), width(inWidth), height(inHeight), samples(fsaa)
            {
            }
            RBFormat() {}
            Nui32 format;
            Nui32 width;
            Nui32 height;
            Nui32 samples;
            // Overloaded comparison operator for usage in map
            bool operator < (const RBFormat &other) const
            {
                if(format < other.format)
                {
                    return true;
                }
                else if(format == other.format)
                {
                    if(width < other.width)
                    {
                        return true;
                    }
                    else if(width == other.width)
                    {
                        if(height < other.height)
                            return true;
                        else if (height == other.height)
                        {
                            if (samples < other.samples)
                                return true;
                        }
                    }
                }
                return false;
            }
        };
        struct RBRef
        {
            RBRef() {}
            RBRef(GLFrameBufferBase * inBuffer) : buffer(inBuffer), refcount(1) {}

            GLFrameBufferBase * buffer;
            NCount refcount;
        };
        typedef map<RBFormat, RBRef>::type RenderBufferMap;
        RenderBufferMap mRenderBufferMap;
    };

}

#endif
