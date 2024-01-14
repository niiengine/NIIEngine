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

#ifndef NII_GLRenderSystem_H
#define NII_GLRenderSystem_H

#include "NiiGLPreInclude.h"
#include "NiiGLRenderSystembase.h"
#include "NiiDataType.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiGLStateCacheManager.h"

#define MAX_LIGHTS 8

namespace NII
{
    class GLSLShaderFactory;
    class GLGpuProgramManager;
    class GLSLShader;
    class GBufferManager;
    
    class _EngineGLAPI GLGeometryRaw : public GeometryRaw, public GLVertexArrayObject
    {
        GLenum mPrimType[2];
    };
    
    struct GL3PlusDescriptorSetTexture2
    {
        GLenum target;
        GLuint texName;
    };
    
    class _EngineGLAPI GLComputeStateObject : public ComputeStateObject
    {
    public:
        GLSLShader * mCS;
    };
    
    class _EngineGLAPI GLTextureSample : public TextureSample
    {
    public:
        GLuint mGLID;
    };
    
    /// @See RenderStateObject. We cache some conversion values here
    /// to avoid doing it every time we set the Pso
    class _EngineGLAPI GLRenderStateObject : public RenderStateObject
    {
    public:
        GLRenderStateObject();
        //Macroblock data
        GLboolean mGLDepthWrite;
        GLenum mGLDepthFunc;
        GLenum mGLCullMode;
        GLenum mGLPolygonMode;

        //Blendblock data
        bool mAlphaBlend;
        GLenum mGLSourceBlend;
        GLenum mGLDestBlend;
        GLenum mGLSourceBlendAlpha;
        GLenum mGLDestBlendAlpha;
        GLenum mGLBlendFunc;
        GLenum mGLBlendFuncAlpha;

        //Shader
        GLSLShader * mVS;
        GLSLShader * mGS;
        GLSLShader * mTS;
        GLSLShader * mDS;
        GLSLShader * mFS;
    };

    /** GL渲染器的实现
    @version NIIEngine 3.0.0
     */
    class _EngineGLAPI GLRenderSystem : public GLRenderSystemBase
    {
    public:
        GLRenderSystem();
        ~GLRenderSystem();

        /// @copydetails RenderSys::getName
        const String & getName() const;

        /// @copydetails RenderSys::init
        void init();

        /// @copydetails RenderSys::setupFeature
        virtual void setupFeature();

        /// @copydetails RenderSys::setFFPParam
        void setFFPParam(const GpuProgramParam & param, Nmark typemark);

        /// @copydetails RenderSys::initImpl
        void initImpl(ViewWindow * primary);

        /// @copydetails RenderSys::shutdown
        void shutdown();

        /// @copydetails RenderSys::setAmbient
        void setAmbient(const Colour & c);

        /// @copydetails RenderSys::setShadingType
        void setShadingType(ShadeMode sm);

        /// @copydetails RenderSys::setLight
        void setLight(bool b);

		/// @copydoc RenderSys::createWindow
		ViewWindow * createWindow(const String & name, NCount width, NCount height, bool full, const PropertyData * custom = 0);

		/// @copydoc RenderSys::createWindow
		bool createWindow(const WindowDefineList & dlist, WindowList & out);

        /// @copydetails RenderSys::getErrorString
        String getErrorString(NIIl num) const;

        /// @copydetails RenderSys::getColourType
        DataType getColourType() const;

        /// @copydetails RenderSys::setUnitNormals
        void setUnitNormals(bool set);

        /// @copydetails RenderSys::_setProj
        void _setProj(const Matrix4f & m);

        /// @copydetails RenderSys::_setView
        void _setView(const Matrix4f & m);

        /// @copydetails RenderSys::_setModel
        void _setModel(const Matrix4f & m);

        /// @copydetails RenderSys::setBlend
        void setBlend(const ShaderChBlend & b, Nmark mark = 0xFFFFFFFF);

        /// @copydetails RenderSys::setColour
        void setColour(const ShaderChColour & c, Nmark mark = 0xFFFFFFFF);

        /// @copydetails RenderSys::setPoint
		void setPoint(const ShaderChPoint & p, Nmark mark = 0xFFFFFFFF);

        /// @copydetails RenderSys::setStencil
        void setStencil(const ShaderChStencil & b, Nmark applymark = 0xFFFFFFFF);

        /// @copydetails RenderSys::setFog
        void setFog(const ShaderChFog & f, Nmark mark = 0xFFFFFFFF);

        /// @copydetails RenderSys::_setLight
        void _setLight(NCount cnt);

        /// @copydetails RenderSys::_setCullingMode
        void _setCullingMode(CullingMode m);

        /// @copydetails RenderSys::_bindTexture
        void _bindTexture(Nidx index, const Texture * tex, bool enable);

        /// @copydetails RenderSys::_setTextureCoordSet
        void _setTextureCoordSet(Nidx index, Nidx setidx);

        /// @copydetails RenderSys::_setTextureCoordPrc
        void _setTextureCoordPrc(Nidx index, TextureCoordPrc m, const Frustum * rel = 0);

        /// @copydetails RenderSys::_setTextureBlend
        void _setTextureBlend(Nidx stage, const TextureBlend & tb);

        /// @copydetails RenderSys::_setTextureMatrix
        void _setTextureMatrix(Nidx index, const Matrix4f & xform);

        /// @copydetails RenderSys::_setViewport
        void _setViewport(Viewport * vp);

        /// @copydetails RenderSys::_beginFrame
        void _beginFrame();

        /// @copydetails RenderSys::_endFrame
        void _endFrame();

        /// @copydetails RenderSys::_setDepthCheck
        void _setDepthCheck(bool b = true);

        /// @copydetails RenderSys::_setDepthWrite
        void _setDepthWrite(bool b = true);

        /// @copydetails RenderSys::_setDepthOpFunc
        void _setDepthOpFunc(CmpMode func = CPM_LESS_EQUAL);

        /// @copydetails RenderSys::_setDepthBias
        void _setDepthBias(NIIf bias, NIIf slopeScale);

        /// @copydetails RenderSys::setColourWrite
        void setColourWrite(Nmark mark);

        /// @copydetails RenderSys::getSysProj
        void getSysProj(const Matrix4f & in, Matrix4f & out, bool gpuprog = false);

        /// @copydetails RenderSys::getSysProj
        void getSysProj(const Radian & fovy, NIIf aspect, NIIf near, NIIf far, Matrix4f & out, bool gpuprog = false);

        /// @copydetails RenderSys::getSysProj
        void getSysProj(NIIf left, NIIf right, NIIf bottom, NIIf top, NIIf near, NIIf far, Matrix4f & out, bool gpuprog = false);
        
        /// @copydetails RenderSys::getSysProj
        void getSysProj(const Matrix4f & in, Matrix4f & out, NIIf near, NIIf far, Frustum::Type type);
        
        /// @copydetails RenderSys::getSysProj
        void getOpenVrProj(const Matrix4f & in, Matrix4f & out);

        /// @copydetails RenderSys::getSysOrtho
		void getSysOrtho(const Radian & fovy, NIIf aspect, NIIf near, NIIf far, Matrix4f & out, bool gpuprog = false);

        /// @copydetails RenderSys::getObliqueDepthProj
        void getObliqueDepthProj(const Plane & plane, Matrix4f & io, bool gpuprog);

        /// @copydetails RenderSys::_setPolygonMode
        void _setPolygonMode(PolygonMode m);

        /// @copydetails RenderSys::setStencilTest
        void setStencilTest(bool set);

        /// @copydetails RenderSys::_setTextureFilter
        void _setTextureFilter(Nidx index, TextureFilterType ftype, TextureFilterOP filter);

        /// @copydetails RenderSys::_setTextureSample
        void _setTextureSample(Nidx indx, const TextureSampleUnit & ts);

        /// @copydetails RenderSys::setGeometry
        virtual void setGeometry(const GeometryRaw * geo);

        /// @copydetails RenderSys::_render
        void _render();

        /// @copydetails RenderSys::on
        void on(GpuProgram * prg);

        /// @copydetails RenderSys::off
        void off(GpuProgram::ShaderType type);

        /// @copydetails RenderSys::on
		void on(GpuProgram::ShaderType type, GpuProgramParam * params, Nmark range);

        /// @copydetails RenderSys::setScissorTest
        void setScissorTest(bool set, NCount left = 0, NCount top = 0, NCount right = 800, NCount bottom = 600) ;

        /// @copydetails RenderSys::clearBuffer
        void clearBuffer(Nmark buffers, const Colour & colour = Colour::Black, NIIf depth = 1.0f, Nui16 stencil = 0);

        /// @copydetails RenderSys::create
        void create(PixelCountQuery *& obj);

        /// @copydetails RenderSys::getTexelXOffset
        virtual NIIf getTexelXOffset() const;

        /// @copydetails RenderSys::getTexelYOffset
        virtual NIIf getTexelYOffset() const;

        /// @copydetails RenderSys::getMinDepth
        virtual NIIf getMinDepth() const;

        /// @copydetails RenderSys::getMaxDepth
        virtual NIIf getMaxDepth() const;
        
        /// @copydetails RenderSys::getDepthRange
        virtual NIIf getDepthRange() const;

		/// @copydetails RenderSys::_setFrame
		void _setFrame(FrameObj * fo);

		/// @copydoc RenderSys::getDisplayMonitorCount
		NCount getDisplayMonitorCount() const;

        /// @copydetails GLRenderSystemBase::_copyContentsToMemory
        void _copyContentsToMemory(Viewport * vp, const Box & src, PixelBlock & dst, ViewWindow::FaceType buffer);

        void _setSurfaceTracking(Nmark tracking);

        bool _setFog(FogMode mode);

		void registerThread();

		void unregisterThread();

        void _oneTimeContextInitialization();

        void _switchContext(GLContext * context);

        void _unregisterContext(GLContext * context);

        GLStateCacheManager * getStateManager()         { return mState; }
    protected:
		/// @copydetails RenderSys::setClipPlanesImpl
		void setClipPlanesImpl(const PlaneList & clip);

		bool activateGLTextureUnit(NCount unit);

        /// @copydetails GLRenderSystemBase::bindVertexElementToGpu
		void bindVertexElementToGpu(const VertexUnit & elem, const VertexBuffer * data, NCount vertexStart);
        
        void bindVertexElementToGpu(const v1::VertexElement & elem, v1::HardwareVertexBufferSharedPtr vertexBuffer,
            const size_t vertexStart, vector<GLuint>::type &attribsBound, vector<GLuint>::type &instanceAttribsBound, bool updateVAO);

        /// @copydetails RenderSys::updateParam
        virtual bool updateParam();

        /// @copydetails RenderSys::getBufferType
        bool getBufferType(ViewportType type);
        
        void preExtraThreadsStarted();
        
        void postExtraThreadsStarted();

        /** Create VAO on current context */
        uint32 _createVao();

        /** Bind VAO, context should be equal to current context, as VAOs are not shared  */
        void _bindVao(GLContext * context, uint32 vao);

        /** Destroy VAO immediately or defer if it was created on other context */
        void _destroyVao(GLContext * context, uint32 vao);

        /** Destroy FBO immediately or defer if it was created on other context */
        void _destroyFbo(GLContext * context, uint32 fbo);

        /// @copydoc RenderSys::beginProfiler
        virtual void beginProfiler(const String & eventName);

        /// @copydoc RenderSys::endProfiler
        virtual void endProfiler();

        /// @copydoc RenderSys::markProfiler
        virtual void markProfiler(const String & eventName);
	private:
		void setGLLight(NCount index, bool lt);

		void makeGLMatrix(GLfloat gl_matrix[16], const Matrix4<NIIf> & m);

		/// Internal method to set pos / direction of a light
		void setGLLightPositionDirection(Light * lt, GLenum lightindex);

        /** Initialises GL extensions, must be done AFTER the GL context has been
            established.
        */
        void initialiseExtensions();
        void setFFPLightParams(size_t index, bool enabled);

        /// @copydoc RenderSys::checkGpuApi
        virtual bool checkGpuApi(const VString & ext) const;

        virtual void _clear();

        virtual void _flush();
        
		static GLint getBlendMode(ColourFactor Blend) const;

		static GLint getTextureAddressingMode(TextureAddressingMode tam) const;

		static GLint convertCompareFunction(CmpMode func) const;
        
        static GLint convertStencilOp(StencilOpType op) const;

        static GLenum getBlendOperation( ColourBlendMode op );
    public:
        friend class ShaderGeneratorTechniqueResolverListener;

        /// @copydetails RenderSys::reinit
        void reinit(void); // Used if settings changed mid-rendering

        /// @copydetails RenderSys::_setContext
        virtual void _setContext(Texture * texture);

        virtual GL3PlusFrameBufferDescMap & _getFrameBufferDescMap() const { return mFrameBufferDescMap; }

        /// @copydetails RenderSys::createFBO
        virtual FrameBufferObject * createFBO();

        /// @copydetails RenderSys::beginFBO
        virtual void beginFBO(FrameBufferObject * fbo, const ViewportList & vlist);

        /// @copydetails RenderSys::endFBO
        virtual void endFBO();
        
        /// @copydetails RenderSys::clearBuffer
        virtual void clearBuffer(FrameBufferObject * fbo, Texture * dst, NCount mipLevel);

        TextureGpu * createDepthImpl(TextureGpu * colour, bool tex, PixelFormat pf);

        bool areFixedFunctionLightsInViewSpace() const                      { return true; }

        void _bindVertexTexture(Nidx unit, Texture *tex);

        void _bindGeometryTexture(Nidx unit, Texture *tex);

        void _bindTessControlTexture(Nidx unit, Texture *tex);

        void _bindDomainTexture(Nidx unit, Texture *tex);

        virtual void _setTexture(Nidx slotStart, const ShaderChTexture * sct);

        virtual void _setSampler(Nidx slotStart, const TextureSample * ts);

        virtual void _setTextureCS(Nidx slotStart, const ShaderChTexture * sct);

        virtual void _setSamplerCS(Nidx slotStart, const TextureSample * ts);
    protected:
        virtual void setBufferUavCS(Nidx slot, const GpuParamBuffer::StorageUnit & buf);
        
        virtual void setTextureUavCS(Nidx slot, const GpuParamBuffer::SamplerUnit & tex, GLuint srvView);
    public:
        virtual void _setBufferCS(Nidx slotStart, const GpuParamBuffer * set);

        virtual void createStorageTask(StorageTask *& rt);
        virtual void destroyStorageTask(StorageTask * rt);
        virtual void syncStorageTask(StorageTask * st);
        virtual void _execute(StorageTask * rt);

        virtual void createRSO(RenderStateObject *& rso); 
        virtual void syncRSO(RenderStateObject * rso);
        virtual void setRSO(const RenderStateObject * rso);

        virtual void createStencil(ShaderChStencil *& scs);
        virtual void destroyStencil(ShaderChStencil * scs);
        virtual void syncStencil(ShaderChStencil * scs);
        
        virtual void createBlend(ShaderChBlend *& scb);
        virtual void destroyBlend(ShaderChBlend * scb);
        virtual void syncBlend(ShaderChBlend * scb);
        
        virtual void createTexSample(TextureSampleUnit *& tsu);
        virtual void destroyTexSample(TextureSampleUnit * tsu);
        virtual void syncTexSample(TextureSampleUnit * tsu);
        
        virtual void createTexture(ShaderChTexture *& sct);
        virtual void destroyTexture(ShaderChTexture * sct);
        virtual void syncTexture(ShaderChTexture * sct);
        
        virtual void createBuffer(GpuParamBuffer *& gpb);
        virtual void destroyBuffer(GpuParamBuffer * gpb);
        virtual void syncBuffer(GpuParamBuffer * gpb);
        
        void _setHlmsDepthblock(const GLRenderStateObject * pso);
        void _setHlmsBlendblock(const GLRenderStateObject * pso);

        virtual void _setIndirect(IndirectBuffer * ib);

        virtual void createCSO(ComputeStateObject *& cso);
        virtual void destroyCSO(ComputeStateObject * cso);
        virtual void syncCSO(ComputeStateObject * cso);
        virtual void setCSO(const ComputeStateObject * cso);
        virtual void _execute(const ComputeStateObject & cso);

        void initialiseContext(ViewWindow * primary);

        virtual void setupGPUProfiler();
        
        virtual void shutdownGPUProfiler();
        
        virtual void beginGPUProfiler(const String & name, Nui32 *hashCache);
        
        virtual void endGPUProfiler(const String & name);
    private:
		typedef list<GLContext *>::type GLContextList;
        
        N_mutex(mThreadInitMutex);
        Light * mLights[MAX_LIGHTS];
        Matrix4f mViewMatrix;
        Matrix4f mWorldMatrix;
        Matrix4f mTextureMatrix;
        TextureFilterOP mMinFilter;
        TextureFilterOP mMipFilter;
        NCount mTextureCoordIndex[NII_MAX_TextureLayer];
        GLenum mTextureTypes[NII_MAX_TextureLayer];
        NIIi mScissorBox[4];

		/// 固定管线纹理单元数量
		Nui16 mFixedFunctionTextureUnits;
		Nui32 mFrontStencilMask;
        Nui32 mBackStencilMask;
		Nui32 mColourWrite;
        Nui8 mBlendChannelMask;
        Nui8 mUseAdjacency;
        bool mStopRendering;
        bool mDepthWrite;
        bool mScissorsEnabled;
        bool mUseAutoTextureMatrix;
        bool mGLInitialised;
        bool mIs43;
        bool mHasArbInvalidateSubdata;
        
        PropertyData mWinProperty;

        /// 当前最终的纹理矩阵
        GLfloat mAutoTextureMatrix[16];

        GBufferManager * mHardwareBufferManager;
        GLGpuProgramManager * mGpuProgramManager;
		GLSLShaderFactory * mGLSLProgramFactory;
        GLStateCacheManager* mState;

        Nui16 mCurrentLights;
		GLenum mActiveTextureUnit;                  ///< 当前活动的纹理单元
        Nui16 mMaxBuiltInTextureAttribIndex;

        GLSLShader * mCS;
        
		GLContextList mBackgroundContextList;       ///< 后台线程的设备器

        typedef unordered_map<GLenum, GLuint>::type BindBufferMap;

        bool mTextureCompareEnabled;

        /** Used to store the number of mipmaps in the currently bound texture.  This is then
            used to modify the texture unit filtering.
        */
        size_t mTextureMipmapCount;

        GLfloat mLargestSupportedAnisotropy;

        /// For rendering legacy objects.
        GLuint mGlobalVao;

        GLSLShaderFactory * mGLSLProgramFactory;

        BindBufferMap mActiveBufferMap;

        // local data members of _render that were moved here to improve performance
        // (save allocations)
        vector<GLuint>::type mRenderAttribsBound;
        vector<GLuint>::type mRenderInstanceAttribsBound;
        
        Nui8 * mIndirectPtr;

        Nui8 mFirstUavBoundSlot;
        Nui8 mLastUavBoundPlusOne;

        Nui8 mClipDistances;
        GLRenderStateObject const * mRSO;

        GLuint mNullColourFramebuffer;

        GL3PlusFrameBufferDescMap mFrameBufferDescMap;
    };
}
#endif