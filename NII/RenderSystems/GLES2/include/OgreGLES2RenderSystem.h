/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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

#ifndef __GLES2RenderSystem_H__
#define __GLES2RenderSystem_H__

#include "OgreGLES2Prerequisites.h"

#include "OgreMaterialManager.h"
#include "OgreRenderSystem.h"
#include "OgreGLSLESProgram.h"
#include "OgreGLRenderSystemCommon.h"

namespace Ogre {
    /** \addtogroup RenderSystems RenderSystems
    *  @{
    */
    /** \defgroup GLES2 GLES2
    * Implementation of GL ES 2.x as a rendering system.
    *  @{
    */
    class GLES2FBOManager;
    class GpuProgramManager;
    class GLSLESProgramCommon;
    class GLSLESProgramFactory;
    class GLES2StateCacheManager;
#if !OGRE_NO_GLES2_CG_SUPPORT
    class GLSLESCgProgramFactory;
#endif
    class GBufferManager;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    class GLES2ManagedResourceManager;
#endif
    
    /**
      Implementation of GL ES 2.x as a rendering system.
     */
    class _OgreGLES2Export GLES2RenderSystem : public GLRenderSystemCommon
    {
        private:
            /// Last min & mip filtering options, so we can combine them
            TextureFilterOP mMinFilter;
            TextureFilterOP mMipFilter;

            /// Holds texture type settings for every stage
            GLenum mTextureTypes[NII_MAX_TextureLayer];

            /// State cache manager which responsible to reduce redundant state changes
            GLES2StateCacheManager* mState;

            typedef std::list<GLContext*> GLContextList;
            /// List of background thread contexts
            GLContextList mBackgroundContextList;

            GpuProgramManager *mGpuProgramManager;
            GLSLESProgramFactory* mGLSLESProgramFactory;
#if !OGRE_NO_GLES2_CG_SUPPORT
            GLSLESCgProgramFactory* mGLSLESCgProgramFactory;
#endif
            GBufferManager* mHardwareBufferManager;

            /// Check if the GL system has already been initialised
            bool mGLInitialised;

            // local data member of _render that were moved here to improve performance
            // (save allocations)
            std::vector<GLuint> mRenderAttribsBound;
            std::vector<GLuint> mRenderInstanceAttribsBound;

            GLenum mPolygonMode;

            GLSLESProgram* mVS;
            GLSLESProgram* mFS;

            GLint getTextureAddressingMode(TextureAddressingMode tam) const;
            GLenum getBlendMode(ColourFactor ogreBlend) const;
            void bindVertexElementToGpu(const VertexElement& elem,
                                        const HardwareVertexBufferSharedPtr& vertexBuffer,
                                        const size_t vertexStart);

            /** Initialises GL extensions, must be done AFTER the GL context has been
                established.
            */
            void initialiseExtensions();

            // Mipmap count of the actual bounded texture
            size_t mCurTexMipCount;
        
            // Store scissor box
            GLint mScissorBox[4];

        public:
            // Default constructor / destructor
            GLES2RenderSystem();
            virtual ~GLES2RenderSystem();
        
            friend class ShaderGeneratorTechniqueResolverListener;

            // ----------------------------------
            // Overridden RenderSystem functions
            // ----------------------------------

            const String& getName(void) const;

            void init() override;

            virtual RenderFeature* setupFeature() const;

            void initImpl(RenderFeature* caps, RenderTarget* primary);

            void shutdown(void);

            /// @copydoc RenderSystem::createWindow
            RenderWindow* createWindow(const String &name, unsigned int width, unsigned int height, 
                bool fullScreen, const PropertyData *miscParams = 0);

            /// @copydoc RenderSystem::_createDepthBufferFor
            DepthBuffer* _createDepthBufferFor( RenderTarget *renderTarget );

            /// @copydoc RenderSystem::createMultiRenderTarget
            virtual MultiRenderTarget * createMultiRenderTarget(const String & name);


            void destroyWindow(const String& name);

            // -----------------------------
            // Low-level overridden members
            // -----------------------------
            void _bindTexture(size_t unit, bool enabled, const TexturePtr &tex);

            void _setSampler(size_t unit, TextureSampleUnit& sampler);

            void _setTextureAddressingMode(size_t stage, const TextureSampleUnit::UVWAddressingMode& uvw);

            void _setLineWidth(float width);

            void _setViewport(Viewport *vp);

            void _beginFrame(void);

            void _endFrame(void);

            void _setCullingMode(CullingMode mode);

            void _setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CmpMode depthFunction = CMPF_LESS_EQUAL);

            void _setDepthBufferCheckEnabled(bool enabled = true);

            void _setDepthBufferWriteEnabled(bool enabled = true);

            void _setDepthBufferFunction(CmpMode func = CMPF_LESS_EQUAL);

            void _setDepthBias(float constantBias, float slopeScaleBias);

            void _setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

            void _setPolygonMode(PolygonMode level);

            void setStencilCheckEnabled(bool enabled);

            void setStencil(CmpMode func = CMPF_ALWAYS_PASS, 
                    uint32 refValue = 0, uint32 compareMask = 0xFFFFFFFF, uint32 writeMask = 0xFFFFFFFF,
                    StencilOpType stencilFailOp = SOP_KEEP,
                    StencilOpType depthFailOp = SOP_KEEP,
                    StencilOpType passOp = SOP_KEEP,
                    bool twoSidedOperation = false,
                    bool readBackAsTexture = false);

            virtual void _setTextureUnitFiltering(size_t unit, TextureFilterOP minFilter,
                TextureFilterOP magFilter, TextureFilterOP mipFilter);              

            void _setTextureUnitFiltering(size_t unit, FilterType ftype, TextureFilterOP filter);

            void _render(const GeometryRaw& op);

            void setScissorTest(bool enabled, size_t left = 0, size_t top = 0, size_t right = 800, size_t bottom = 600);

            void clearBuffer(unsigned int buffers,
                const Colour& colour = Colour::Black,
                Real depth = 1.0f, unsigned short stencil = 0);
            PixelCountQuery* create(void);
            OGRE_MUTEX(mThreadInitMutex);
            void registerThread();
            void unregisterThread();
            void preExtraThreadsStarted();
            void postExtraThreadsStarted();

            // ----------------------------------
            // GLES2RenderSystem specific members
            // ----------------------------------        
            GLES2StateCacheManager * getStateManager() { return mState; }
        
            /** Create VAO on current context */
            uint32 _createVao();
            /** Bind VAO, context should be equal to current context, as VAOs are not shared  */
            void _bindVao(GLContext* context, uint32 vao);
            /** Destroy VAO immediately or defer if it was created on other context */
            void _destroyVao(GLContext* context, uint32 vao);
            /** Destroy FBO immediately or defer if it was created on other context */
            void _destroyFbo(GLContext* context, uint32 fbo);

            /** Unregister a render target->context mapping. If the context of target 
             is the current context, change the context to the main context so it
             can be destroyed safely. 
             
             @note This is automatically called by the destructor of 
             GLContext.
             */
            void _unregisterContext(GLContext *context);
            /** Switch GL context, dealing with involved internal cached states too
             */
            void _switchContext(GLContext *context);
            /** One time initialization for the RenderState of a context. Things that
             only need to be set once, like the LightingModel can be defined here.
             */
            void _oneTimeContextInitialization();
            void initialiseContext(RenderWindow* primary);
            /**
             * Set current render target to target, enabling its GL context if needed
             */
            void _setRenderTarget(RenderTarget *target);

            GLint convertCompareFunction(CmpMode func) const;
            GLint convertStencilOp(StencilOpType op, bool invert = false) const;

            void bindGpuProgram(GpuProgram* prg);
            void unbindGpuProgram(GpuProgramType gptype);
            void on(GpuProgramType gptype, const GpuProgramParam *& params, uint16 mask);
            void bindGpuProgramPassIterationParameters(GpuProgramType gptype);

            /// @copydoc RenderSystem::_setSeparateSceneBlending
            void _setSeparateSceneBlending( ColourFactor sourceFactor, ColourFactor destFactor, ColourFactor sourceFactorAlpha, ColourFactor destFactorAlpha, ColourBlendMode op, ColourBlendMode alphaOp );
            /// @copydoc RenderSystem::_setAlphaRejectSettings
            void _setAlphaRejectSettings( CmpMode func, unsigned char value, bool alphaToCoverage );
            /// @copydoc RenderSystem::getDisplayMonitorCount
            unsigned int getDisplayMonitorCount() const;

            void _destroyDepthBuffer(RenderTarget* pRenderWnd);
        
            /// @copydoc RenderSystem::beginProfiler
            virtual void beginProfiler( const String &eventName );
            
            /// @copydoc RenderSystem::endProfiler
            virtual void endProfiler( void );
            
            /// @copydoc RenderSystem::markProfiler
            virtual void markProfiler( const String &eventName );

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
            void resetRenderer(RenderWindow* pRenderWnd);
        
            void notifyOnContextLost();

            static GLES2ManagedResourceManager* getResourceManager();
    private:
            static GLES2ManagedResourceManager* mResourceManager;
#endif
            void _copyContentsToMemory(Viewport* vp, const Box& src, const PixelBox& dst, RenderWindow::FrameBuffer buffer);
            
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        private:
            /// View matrix to set world against
            Matrix4 mViewMatrix;
            Matrix4 mWorldMatrix;
            Matrix4 mTextureMatrix;

            /// Last min & mip filtering options, so we can combine them
            TextureFilterOP mMinFilter;
            TextureFilterOP mMipFilter;

            /// Holds texture type settings for every stage
            GLenum mTextureTypes[NII_MAX_TextureLayer];

            GLfloat mLargestSupportedAnisotropy;

            /// Number of fixed-function texture units
            unsigned short mFixedFunctionTextureUnits;

            /// Store last colour write state
            uint8 mBlendChannelMask;

            /// Store last depth write state
            bool mDepthWrite;

            /// Store last scissor enable state
            bool mScissorsEnabled;

            GLfloat mAutoTextureMatrix[16];

            bool mUseAutoTextureMatrix;

            /// GL support class, used for creating windows etc.
            GLES2Support *mGLSupport;

            /* The main GL context - main thread only */
            GLES2Context *mMainContext;

            /* The current GL context  - main thread only */
            GLES2Context *mCurrentContext;

            typedef list<GLES2Context*>::type GLES2ContextList;
            /// List of background thread contexts
            GLES2ContextList mBackgroundContextList;

            /// For rendering legacy objects.
            GLuint  mGlobalVao;
            v1::VertexData  *mCurrentVertexBuffer;
            v1::IndexData   *mCurrentIndexBuffer;
            GLenum          mCurrentPolygonMode;

            GLES2GpuProgramManager *mGpuProgramManager;
            GLSLESShaderFactory* mGLSLESShaderFactory;
#if !OGRE_NO_GLES2_CG_SUPPORT
            GLSLESCgProgramFactory* mGLSLESCgProgramFactory;
#endif
            v1::HardwareBufferManager* mHardwareBufferManager;

            /** Manager object for creating render textures.
                Direct render to texture via GL_OES_framebuffer_object is preferable 
                to pbuffers, which depend on the GL support used and are generally 
                unwieldy and slow. However, FBO support for stencil buffers is poor.
              */
            GLES2RTTManager *mRTTManager;

            /** These variables are used for caching RenderSystem state.
                They are cached because OpenGL state changes can be quite expensive,
                which is especially important on mobile or embedded systems.
            */
            GLenum mActiveTextureUnit;
        
            /// Check if the GL system has already been initialised
            bool mGLInitialised;
            bool mUseAdjacency;

            bool mHasDiscardFramebuffer;

            // local data member of _render that were moved here to improve performance
            // (save allocations)
            vector<GLuint>::type mRenderAttribsBound;
            vector<GLuint>::type mRenderInstanceAttribsBound;

            //GLint getCombinedMinMipFilter(void) const;
            
            /** Converts a PixelFormat into its equivalent layout for image variables (GLSL)
                or its equivalent for D3D11/12 variables (HLSL). Used mostly with UAVs.
            @param pixelFormat
                Pixel format to convert.
            @return
                String for the shader to use "as is". If the Pixel Format doesn't have
                a shader equivalent (i.e. depth formats), a null pointer is returned.
                The validity of the pointer lasts as long as the RenderSystem remains
                loaded.
            */
            virtual const char* getToken( PixelFormat pf ) const = 0;

            /**
            @param pixelFormat
            @param textureType
                See Texture::Type::Texture::Type
            @param isMsaa
            @param access
                Texture access. Use ResourceAccess::Undefined for requesting sampling mode
            @return
                String for the shader to use "as is". Returned pointer may be null.
                The validity of the pointer lasts as long as the RenderSystem remains
                loaded.
            */
            virtual const char* getToken( PixelFormat pf, uint32 textureType, bool isMsaa, Buffer::OpType access ) const = 0;

            /// @copydoc RenderSystem::getPixelFormatToShaderType
            //virtual const PixelFormatToShaderType* getPixelFormatToShaderType(void) const;

            unsigned char *mIndirectPtr;

            GLES2HlmsPso const    *mPso;

            GLuint  mNullColourFramebuffer;

            GLES2PixelFormatToShaderType mPixelFormatToShaderType;

            GLint getTextureAddressingMode(TextureAddressingMode tam) const;
            GLenum getBlendMode(ColourFactor ogreBlend) const;
            GLenum getBlendOperation(ColourBlendMode op) const;

            bool activateGLTextureUnit(size_t unit);
            void bindVertexElementToGpu( const v1::VertexElement &elem,
                                         v1::HardwareVertexBufferSharedPtr vertexBuffer,
                                         const size_t vertexStart,
                                         vector<GLuint>::type &attribsBound,
                                         vector<GLuint>::type &instanceAttribsBound,
                                         bool updateVAO);

            void correctViewport( GLint x, GLint &y, GLint &w, GLint &h, RenderTarget *renderTarget );

            // Mipmap count of the actual bounded texture
            size_t mCurTexMipCount;

        public:
            // Default constructor / destructor
            GLES2RenderSystem();
            virtual ~GLES2RenderSystem();
        
            friend class ShaderGeneratorTechniqueResolverListener;

            // ----------------------------------
            // Overridden RenderSystem functions
            // ----------------------------------
            /** See
              RenderSystem
             */
            const String& getName(void) const;
            /** See
              RenderSystem
             */
            const String& getFriendlyName(void) const;
            /** See
              RenderSystem
             */
            ConfigOptionMap& getConfig(void);
            /** See
              RenderSystem
             */
            void setConfig(const String &name, const String &value);
            /** See
              RenderSystem
             */
            String checkConfig(void);
            /** See
              RenderSystem
             */
            RenderWindow* init(bool autoCreateWindow, const String& windowTitle = "OGRE Render Window");
            /** See
              RenderSystem
             */
            virtual RenderFeature* setupFeature() const;
            /** See
              RenderSystem
             */
            void initImpl(RenderFeature* caps, RenderTarget* primary);
            /** See
              RenderSystem
             */
            void reinit(void); // Used if settings changed mid-rendering
            /** See
              RenderSystem
             */
            void shutdown(void);

            /// @copydoc RenderSystem::createWindow
            RenderWindow* createWindow(const String &name, unsigned int width, unsigned int height, 
                bool fullScreen, const PropertyData *miscParams = 0);

            /// @copydoc RenderSystem::_createDepthBufferFor
            DepthBuffer* _createDepthBufferFor( RenderTarget *renderTarget, bool exactMatchFormat );

            /// Mimics D3D9RenderSystem::_getDepthStencilFormatFor, if no FBO RTT manager, outputs GL_NONE
            void _getDepthStencilFormatFor( GLenum internalColourFormat, GLenum *depthFormat,
                                            GLenum *stencilFormat );

            /// @copydoc RenderSystem::createMultiRenderTarget
            virtual MultiRenderTarget * createMultiRenderTarget(const String & name);

            /** See
              RenderSystem
             */
            void destroyWindow(RenderWindow* pWin);
            /** See
              RenderSystem
             */
            String getErrorString(long errorNumber) const;
            /** See
              RenderSystem
             */
            DataType getColourType(void) const;

            // -----------------------------
            // Low-level overridden members
            // -----------------------------
            /** See
             RenderSystem
             */
            void _setLight(const LightList& lights, unsigned short limit) { };   // Not supported
            /** See
             RenderSystem
             */
            bool areFixedFunctionLightsInViewSpace() const { return true; }
            /** See
             RenderSystem
             */
            void _setModel(const Matrix4 &m);
            /** See
             RenderSystem
             */
            void _setView(const Matrix4 &m);
            /** See
             RenderSystem
             */
            void _setProj(const Matrix4 &m);
            /** See
             RenderSystem
             */
            void setColour(const Colour &ambient,
                                   const Colour &diffuse, const Colour &specular,
                                   const Colour &emissive, Real shininess,
                                   TrackVertexColourType tracking) {}
            /** See
             RenderSystem
             */
            void setPoint(Real size, bool attenuationEnabled,
                                     Real constant, Real linear, Real quadratic, Real minSize, Real maxSize) {}
            /** See
             RenderSystem
             */
            void setPoint(bool enabled) {}
            /** See
             RenderSystem
             */
            void _bindTexture(size_t unit, bool enabled, Texture *tex);
            /** See
             RenderSystem
             */
            void _setTextureCoordPrc(size_t stage, TextureCoordPrc m,
                    const Frustum* frustum = 0) { };   // Not supported
            /** See
             RenderSystem
             */
            void _setTextureBlend(size_t stage, const TextureBlend& bm) { };   // Not supported
            /** See
             RenderSystem
             */
            void _setTextureMatrix(size_t stage, const Matrix4& xform) { };   // Not supported
        
            virtual void queueBindUAV( uint32 slot, TexturePtr texture,
                                       Buffer::OpType access = Buffer::OT_READWRITE,
                                       int32 mipmapLevel = 0, int32 textureArrayIndex = 0,
                                       PixelFormat pixelFormat = PF_UNKNOWN );
            virtual void queueBindUAV( uint32 slot, UavBufferPacked *buffer,
                                       Buffer::OpType access = Buffer::OT_READWRITE,
                                       size_t offset = 0, size_t sizeBytes = 0 );

            virtual void clearUAVs(void);
 
            virtual void _bindTextureUavCS( uint32 slot, Texture *texture,
                                            Buffer::OpType access,
                                            int32 mipmapLevel, int32 textureArrayIndex,
                                            PixelFormat pixelFormat );
            virtual void _setTextureCS( uint32 slot, bool enabled, Texture *texPtr );
            virtual void _setHlmsSamplerblockCS( uint8 texUnit, const TextureSampleUnit *samplerblock );

            /** See
             RenderSystem
             */
            void _setViewport(Viewport *vp);

            virtual void createRSO( RenderStateObject * rso );
            virtual void destroyRSO( RenderStateObject * rso );
            virtual void createStencil( ShaderChStencil *newBlock );
            virtual void destroyStencil( ShaderChStencil *block );
            virtual void createBlend( ShaderChBlend *newBlock );
            virtual void destroyBlend( ShaderChBlend *block );
            virtual void createTexSample( TextureSampleUnit *newBlock );
            virtual void destroyTexSample( TextureSampleUnit *block );
            void _setHlmsDepthblock( const ShaderChStencil * stencil, const GLES2HlmsPso *pso );
            void _setHlmsBlendblock( const ShaderChBlend * blend, const GLES2HlmsPso *pso );
            virtual void _setTextureSample( uint8 texUnit, const TextureSampleUnit *samplerblock );
            virtual void setRSO( const RenderStateObject *pso );

            virtual void _setIndirect( IndirectBuffer * ib );
            virtual void setCSO( const ComputeStateObject *pso );

            /** See
             RenderSystem
             */
            void _beginFrame(void);
            /** See
             RenderSystem
             */
            void _endFrame(void);
            /** See
             RenderSystem
             */
            void _setDepthBias(float constantBias, float slopeScaleBias);
            /** See
             RenderSystem
             */
            void _setDepthBufferCheckEnabled(bool enabled = true);
            /** See
             RenderSystem
             */
            void _setDepthBufferWriteEnabled(bool enabled = true);
            /** See
             RenderSystem
             */
            void _makeProjectionMatrix(const Radian& fovy, Real aspect, Real nearPlane, Real farPlane,
                    Matrix4& dest, bool forGpuProgram = false);
            /** See
             RenderSystem
             */
            void _makeProjectionMatrix(Real left, Real right, Real bottom, Real top, 
                    Real nearPlane, Real farPlane, Matrix4& dest, bool forGpuProgram = false);
            /** See
             RenderSystem
             */
            void _makeOrthoMatrix(const Radian& fovy, Real aspect, Real nearPlane, Real farPlane,
                    Matrix4& dest, bool forGpuProgram = false);
            /** See
             RenderSystem
             */
            void _applyObliqueDepthProjection(Matrix4& matrix, const Plane& plane, 
                    bool forGpuProgram);
            /** See
             RenderSystem
             */
            void setClipPlane (ushort index, Real A, Real B, Real C, Real D);
            /** See
             RenderSystem
             */
            void enableClipPlane (ushort index, bool enable);
            /** See
             RenderSystem
             */
            virtual void setStencil(uint32 refValue, const ShaderChStencil &stencil);
            /** See
             RenderSystem
             */
            //virtual bool hasAnisotropicMipMapFilter() const { return false; }   
            /** See
             RenderSystem
             */
            void _render(const v1::GeometryRaw& op);

            virtual void _execute( const ComputeStateObject &pso );

            virtual void setGeometry( const GeometryRaw *vao );
            virtual void _render( const CbDrawCallIndexed *cmd );
            virtual void _render( const CbDrawCallStrip *cmd );
            virtual void _renderEmulated( const CbDrawCallIndexed *cmd );
            virtual void _renderEmulated( const CbDrawCallStrip *cmd );
            virtual void _renderEmulatedNoBaseInstance( const CbDrawCallIndexed *cmd );
            virtual void _renderEmulatedNoBaseInstance( const CbDrawCallStrip *cmd );

            virtual void _startLegacyV1Rendering(void);
            virtual void _setRenderOperation( const v1::GeometryRaw *cmd );
            virtual void _render( const v1::CbDrawCallIndexed *cmd );
            virtual void _render( const v1::CbDrawCallStrip *cmd );
            virtual void _renderNoBaseInstance( const v1::CbDrawCallIndexed *cmd );
            virtual void _renderNoBaseInstance( const v1::CbDrawCallStrip *cmd );

            virtual void clearBuffer(unsigned int buffers,
                const Colour& colour = Colour::Black,
                Real depth = 1.0f, unsigned short stencil = 0);
            virtual void discardFrameBuffer( unsigned int buffers );
            PixelCountQuery* create(void);
            Real getTexelXOffset(void) { return 0.0; }               // No offset in GL
            Real getTexelYOffset(void) { return 0.0; }                 // No offset in GL
            Real getMinDepth(void) { return -1.0f; }            // Range [-1.0f, 1.0f]
            Real getMaxDepth(void) { return 1.0f; }             // Range [-1.0f, 1.0f]
            OGRE_MUTEX(mThreadInitMutex);
            void registerThread();
            void unregisterThread();
            void preExtraThreadsStarted();
            void postExtraThreadsStarted();
            void setClipPlanesImpl(const Ogre::PlaneList& planeList) {}

            // ----------------------------------
            // GLES2RenderSystem specific members
            // ----------------------------------
            bool hasMinGLVersion(int major, int minor) const;
            bool checkGpuApi(const String& ext) const;
        
            /** Returns the main context */
            GLES2Context* _getMainContext() { return mMainContext; }
            /** Unregister a render target->context mapping. If the context of target 
             is the current context, change the context to the main context so it
             can be destroyed safely. 
             
             @note This is automatically called by the destructor of 
             GLES2Context.
             */
            void _unregisterContext(GLES2Context *context);
            /** Switch GL context, dealing with involved internal cached states too
             */
            void _switchContext(GLES2Context *context);
            /** One time initialization for the RenderState of a context. Things that
             only need to be set once, like the LightingModel can be defined here.
             */
            void _oneTimeContextInitialization();
            void initialiseContext(RenderWindow* primary);
            /**
             * Set current render target to target, enabling its GL context if needed
             */
            void _setRenderTarget(RenderTarget *target, uint8 viewportRenderTargetFlags);

            GLES2Support* getGLES2Support() { return mGLSupport; }

            void on(GpuProgramType gptype, GpuProgramParam * params, uint16 mask);
            void bindGpuProgramPassIterationParameters(GpuProgramType gptype);

            /// @copydoc RenderSystem::_setSceneBlending
            void _setSceneBlending( ColourFactor sourceFactor, ColourFactor destFactor, ColourBlendMode op );
            /// @copydoc RenderSystem::_setSeparateSceneBlending
            void _setSeparateSceneBlending( ColourFactor sourceFactor, ColourFactor destFactor, ColourFactor sourceFactorAlpha, ColourFactor destFactorAlpha, ColourBlendMode op, ColourBlendMode alphaOp );
            /// @copydoc RenderSystem::getDisplayMonitorCount
            unsigned int getDisplayMonitorCount() const;

            //void _setSceneBlendingOperation(ColourBlendMode op);
            //void _setSeparateSceneBlendingOperation(ColourBlendMode op, ColourBlendMode alphaOp);

            void _destroyDepthBuffer(RenderWindow* pRenderWnd);
        
            /// @copydoc RenderSystem::beginProfiler
            virtual void beginProfiler( const String &eventName );
            
            /// @copydoc RenderSystem::endProfiler
            virtual void endProfiler( void );
            
            /// @copydoc RenderSystem::markProfiler
            virtual void markProfiler( const String &eventName );

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
            void resetRenderer(RenderWindow* pRenderWnd);
        
            static GLES2ManagedResourceManager* getResourceManager();
    private:
            static GLES2ManagedResourceManager* mResourceManager;
#endif

            virtual void setupGPUProfiler(void);
            virtual void shutdownGPUProfiler(void);
            virtual void beginGPUProfiler( const String &name, uint32 *hashCache );
            virtual void endGPUProfiler( const String &name );
    };
}

#endif
