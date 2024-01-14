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
#ifndef __D3D11RENDERSYSTEM_H__
#define __D3D11RENDERSYSTEM_H__

#include "OgreD3D11Prerequisites.h"
#include "OgreRenderSystem.h"
#include "OgreD3D11Device.h"
#include "OgreD3D11DeviceResource.h"
#include "OgreD3D11Driver.h"
#include "OgreD3D11Mappings.h"

namespace Ogre 
{
    /** \addtogroup RenderSystems RenderSystems
    *  @{
    */
    /** \defgroup Direct3D11 Direct3D11
    * Implementation of DirectX11 as a rendering system.
    *  @{
    */
	/// Enable recognizing SM2.0 HLSL shaders.
	/// (the same shader code could be used by many RenderSystems, directly or via Cg)
	#define SUPPORT_SM2_0_HLSL_SHADERS  1

    class D3D11DriverList;
    class D3D11Driver;
	class D3D11StereoDriverBridge;
	
    /**
    Implementation of DirectX11 as a rendering system.
    */
    class _OgreD3D11Export D3D11RenderSystem
        : public RenderSystem
        , protected D3D11DeviceResourceManager
    {
    private:
        Ogre::String mDriverName;    // it`s hint rather than hard requirement, could be ignored if empty or device removed
        D3D_DRIVER_TYPE mDriverType; // should be XXX_HARDWARE, XXX_SOFTWARE or XXX_WARP, never XXX_UNKNOWN or XXX_NULL
        D3D_FEATURE_LEVEL mFeatureLevel;
        D3D_FEATURE_LEVEL mMinRequestedFeatureLevel;
        D3D_FEATURE_LEVEL mMaxRequestedFeatureLevel;

        /// Direct3D rendering device
        D3D11Device     mDevice;

        /// List of D3D drivers installed (video cards)
        D3D11DriverList* mDriverList;
        /// Currently active driver
        D3D11Driver mActiveD3DDriver;
        /// NVPerfHUD allowed?
        bool mUseNVPerfHUD;
		int mSwitchingFullscreenCounter;	// Are we switching from windowed to fullscreen 

        static ID3D11DeviceN* createD3D11Device(D3D11Driver* d3dDriver, D3D_DRIVER_TYPE driverType,
                         D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL, D3D_FEATURE_LEVEL* pFeatureLevel);

        D3D11DriverList* getDirect3DDrivers(bool refreshList = false);
        void refreshD3DSettings(void);
        void refreshFSAAOptions(void);

        void freeDevice(void);
        void createDevice();

        /// return anisotropy level
        DWORD _getCurrentAnisotropy(size_t unit);
        
        D3D11HardwareBufferManager* mHardwareBufferManager;
        GpuProgramManager* mGpuProgramManager;
        D3D11HLSLProgramFactory* mHLSLProgramFactory;

        size_t mLastVertexSourceCount;

        /// Internal method for populating the capabilities structure
        RenderFeature* setupFeature() const;
        /** See RenderSystem definition */
        void initImpl(RenderFeature* caps, RenderTarget* primary);

        void convertVertexShaderCaps(RenderFeature* rsc) const;
        void convertPixelShaderCaps(RenderFeature* rsc) const;
        void convertGeometryShaderCaps(RenderFeature* rsc) const;
        void convertHullShaderCaps(RenderFeature* rsc) const;
        void convertDomainShaderCaps(RenderFeature* rsc) const;
        void convertComputeShaderCaps(RenderFeature* rsc) const;

        bool checkVertexTextureFormats(void);
        void detachRenderTargetImpl(const String& name);

        CmpMode mSceneAlphaRejectFunc; // should be merged with - mBlendDesc
        unsigned char mSceneAlphaRejectValue; // should be merged with - mBlendDesc
        bool mSceneAlphaToCoverage;

        D3D11_BLEND_DESC    mBlendDesc;
        bool                mBlendDescChanged;

        D3D11_RASTERIZER_DESC   mRasterizerDesc;
        bool                    mRasterizerDescChanged;

        UINT mStencilRef;
        D3D11_DEPTH_STENCIL_DESC    mDepthStencilDesc; 
        bool                        mDepthStencilDescChanged;

        PolygonMode mPolygonMode;

        TextureFilterOP FilterMinification[NII_MAX_TextureLayer];
        TextureFilterOP FilterMagnification[NII_MAX_TextureLayer];
        TextureFilterOP FilterMips[NII_MAX_TextureLayer];
        bool          CompareEnabled;

        D3D11_RECT mScissorRect;
        
        bool mReadBackAsTexture;

        D3D11HLSLProgram* mBoundVertexProgram;
        D3D11HLSLProgram* mBoundFragmentProgram;
        D3D11HLSLProgram* mBoundGeometryProgram;
        D3D11HLSLProgram* mBoundTessellationHullProgram;
        D3D11HLSLProgram* mBoundTessellationDomainProgram;
        D3D11HLSLProgram* mBoundComputeProgram;

        ComPtr<ID3D11ShaderResourceView> mDSTResView;
        ComPtr<ID3D11BlendState> mBoundBlendState;
        ComPtr<ID3D11RasterizerState> mBoundRasterizer;
        ComPtr<ID3D11DepthStencilState> mBoundDepthStencilState;
        ComPtr<ID3D11SamplerState> mBoundSamplerStates[NII_MAX_TextureLayer];
        size_t mBoundSamplerStatesCount;

        ID3D11ShaderResourceView * mBoundTextures[NII_MAX_TextureLayer];
        size_t mBoundTexturesCount;

        // List of class instances per shader stage
        ID3D11ClassInstance* mClassInstances[6][8];

        // Number of class instances per shader stage
        UINT mNumClassInstances[6];
        
        // Store created shader subroutines, to prevent creation and destruction every frame
        typedef std::map<String, ID3D11ClassInstance*> ClassInstanceMap;
        typedef std::map<String, ID3D11ClassInstance*>::iterator ClassInstanceIterator;
        ClassInstanceMap mInstanceMap;

        /// structure holding texture unit settings for every stage
        struct sD3DTextureStageDesc
        {
            /// the type of the texture
            TextureType type;

            /// texture 
            ID3D11ShaderResourceView  *pTex;
            D3D11_SAMPLER_DESC  samplerDesc;
            bool used;
        } mTexStageDesc[NII_MAX_TextureLayer];

        size_t     mLastTextureUnitState;
		bool       mSamplerStatesChanged;



        /// Primary window, the one used to create the device
        D3D11RenderWindowBase* mPrimaryWindow;

        typedef std::vector<D3D11RenderWindowBase*> SecondaryWindowList;
        // List of additional windows after the first (swap chains)
        SecondaryWindowList mSecondaryWindows;

        bool mRenderSystemWasInited;

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
		D3D11StereoDriverBridge* mStereoDriver;
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
		Windows::Foundation::EventRegistrationToken suspendingToken, surfaceContentLostToken;
#endif

    protected:
        /**
         * With DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL flag render target views are unbound
         * from us each Present(), and we need the way to reestablish connection.
         */
        void _setRenderTargetViews();

    public:
        // constructor
        D3D11RenderSystem( );

        // destructor
        ~D3D11RenderSystem();
		
		
		int getSwitchingFullscreenCounter() const		        { return mSwitchingFullscreenCounter; }
		void addToSwitchingFullscreenCounter()			        { mSwitchingFullscreenCounter++; }
		
        void initRenderSystem();

        virtual void initConfigOptions(void);

        // Overridden RenderSystem functions
        String checkConfig(void);
        void init() override;
        /// @copydoc RenderSystem::createWindow
        RenderWindow* createWindow(const String &name, unsigned int width, unsigned int height, 
            bool fullScreen, const PropertyData *miscParams = 0);
        /// @copydoc RenderSystem::_updateAllRenderTargets
        virtual void _updateAllRenderTargets(bool _swap = true);
        /// @copydoc RenderSystem::_swapAllRenderTargetBuffers
        virtual void _swapAllRenderTargetBuffers();

        void fireDeviceEvent( D3D11Device* device, const String & name, D3D11RenderWindowBase* sendingWindow = NULL);

        /// @copydoc RenderSystem::createMultiRenderTarget
        virtual MultiRenderTarget * createMultiRenderTarget(const String & name);

        virtual DepthBuffer* _createDepthBufferFor( RenderTarget *renderTarget );

        /**
         * This function is meant to add Depth Buffers to the pool that aren't released when the DepthBuffer
         * is deleted. This is specially useful to put the Depth Buffer created along with the window's
         * back buffer into the pool. All depth buffers introduced with this method go to GroupInner
         */
        DepthBuffer* _addManualDepthBuffer( ID3D11DepthStencilView *depthSurface,
                                            uint32 width, uint32 height, uint32 fsaa, uint32 fsaaQuality );

        /// Reverts _addManualDepthBuffer actions
        void _removeManualDepthBuffer(DepthBuffer *depthBuffer);
        /// @copydoc RenderSystem::detachRenderTarget
        virtual RenderTarget * detachRenderTarget(const String &name);

        const String& getName(void) const;

        void getPlatformData(const String& name, void* pData);
        // Low-level overridden members
        bool setConfig( const String &name, const String &value );
        void reinit();
        void shutdown();
        void checkDevice(bool select = false);
        void handleDeviceLost();
        void destroyRenderTarget(const String& name);
        DataType getColourType(void) const;
        void setStencilCheckEnabled(bool enabled);
        void setStencil(CmpMode func = CMPF_ALWAYS_PASS, 
            uint32 refValue = 0, uint32 compareMask = 0xFFFFFFFF, uint32 writeMask = 0xFFFFFFFF,
            StencilOpType stencilFailOp = SOP_KEEP, 
            StencilOpType depthFailOp = SOP_KEEP,
            StencilOpType passOp = SOP_KEEP, 
            bool twoSidedOperation = false,
            bool readBackAsTexture = false);

        // Low-level overridden members, mainly for internal use
        D3D11HLSLProgram* _getBoundVertexProgram() const;
        D3D11HLSLProgram* _getBoundFragmentProgram() const;
        D3D11HLSLProgram* _getBoundGeometryProgram() const;
        D3D11HLSLProgram* _getBoundTessellationHullProgram() const;
        D3D11HLSLProgram* _getBoundTessellationDomainProgram() const;
        D3D11HLSLProgram* _getBoundComputeProgram() const;
        void _bindTexture(size_t unit, bool enabled, const TexturePtr &texPtr);
        void _setSampler(size_t unit, TextureSampleUnit& sampler);
        void _setTextureAddressingMode(size_t stage, const TextureSampleUnit::UVWAddressingMode& uvw);
        void _setSeparateSceneBlending(ColourFactor sourceFactor, ColourFactor destFactor, ColourFactor sourceFactorAlpha, 
            ColourFactor destFactorAlpha, ColourBlendMode op = SBO_ADD, ColourBlendMode alphaOp = SBO_ADD);
        void _setAlphaRejectSettings( CmpMode func, unsigned char value, bool alphaToCoverage );
        void _setViewport( Viewport *vp );
        void _beginFrame(void);
        void _endFrame(void);
        void _setCullingMode( CullingMode mode );
        void _setDepthBufferParams( bool depthTest = true, bool depthWrite = true, CmpMode depthFunction = CMPF_LESS_EQUAL );
        void _setDepthBufferCheckEnabled( bool enabled = true );
        bool _getDepthBufferCheckEnabled( void );
        void _setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);
        void _setDepthBufferWriteEnabled(bool enabled = true);
        void _setDepthBufferFunction( CmpMode func = CMPF_LESS_EQUAL );
        void _setDepthBias(float constantBias, float slopeScaleBias);
		void getSysProj(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram = false);
        void _setPolygonMode(PolygonMode level);
        void _setTextureUnitFiltering(size_t unit, FilterType ftype, TextureFilterOP filter);
        void setVertexDeclaration(VertexDeclaration* decl);
        void setVertexDeclaration(VertexDeclaration* decl, VertexBufferBinding* binding);
        void setVertexBufferBinding(VertexBufferBinding* binding);
        /** render using the feature of reading back the inactive depth-stencil buffers as texture*/
        void _renderUsingReadBackAsTexture(unsigned int passNr, Ogre::String variableName,unsigned int StartSlot);
        void _dispatchCompute(const Vector3i& workgroupDim);
        void _render(const GeometryRaw& op);

        void bindGpuProgram(GpuProgram* prg);

        void unbindGpuProgram(GpuProgramType gptype);

        void on(GpuProgramType gptype, const GpuProgramParam *& params, uint16 mask);

        void bindGpuProgramPassIterationParameters(GpuProgramType gptype);

        void setScissorTest(bool enabled, size_t left = 0, size_t top = 0, size_t right = 800, size_t bottom = 600);
        void clearBuffer(unsigned int buffers, 
            const Colour& colour = Colour::Black, 
            Real depth = 1.0f, unsigned short stencil = 0);
        PixelCountQuery* create(void);
        Real getTexelXOffset(void);
        Real getTexelYOffset(void);
        Real getMinDepth(void);
        Real getMaxDepth(void);
        void registerThread();
        void unregisterThread();
        void preExtraThreadsStarted();
        void postExtraThreadsStarted();

        /**
         * Set current render target to target, enabling its GL context if needed
         */
        void _setRenderTarget(RenderTarget *target);

        void determineFSAASettings(uint fsaa, const String& fsaaHint, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outFSAASettings);

        /// @copydoc RenderSystem::getDisplayMonitorCount
        unsigned int getDisplayMonitorCount() const {return 1;} //todo

        D3D11Device &_getDevice() { return mDevice; }
        
        
        D3D_FEATURE_LEVEL _getFeatureLevel() const { return mFeatureLevel; }

        void setSubroutine(GpuProgramType gptype, unsigned int slotIndex, const String& subroutineName);
        
        void setSubroutine(GpuProgramType gptype, const String& slotName, const String& subroutineName);

        /// @copydoc RenderSystem::beginProfiler
        virtual void beginProfiler( const String &eventName );

        /// @copydoc RenderSystem::endProfiler
        virtual void endProfiler( void );

        /// @copydoc RenderSystem::markProfiler
        virtual void markProfiler( const String &eventName );
		
		/// @copydoc RenderSystem::getBufferType
		virtual bool getBufferType(ViewportType colourBuffer);
        
    ////////////////////////////////////////////////////////////////////////////////
        Ogre::String mDriverName;    // it`s hint rather than hard requirement, could be ignored if empty or device removed
        D3D_DRIVER_TYPE mDriverType; // should be XXX_HARDWARE, XXX_SOFTWARE or XXX_WARP, never XXX_UNKNOWN or XXX_NULL
        D3D_FEATURE_LEVEL mFeatureLevel;
        D3D_FEATURE_LEVEL mMinRequestedFeatureLevel;
        D3D_FEATURE_LEVEL mMaxRequestedFeatureLevel;

        /// Direct3D rendering device
        D3D11Device     mDevice;

        D3D11VendorExtension *mVendorExtension;
        
        // Stored options
        ConfigOptionMap mOptions;

        /// List of D3D drivers installed (video cards)
        D3D11DriverList* mDriverList;
        /// Currently active driver
        D3D11Driver mActiveD3DDriver;
        /// NVPerfHUD allowed?
        bool mUseNVPerfHUD;
		int mSwitchingFullscreenCounter;	// Are we switching from windowed to fullscreen 

        static void createD3D11Device( D3D11VendorExtension *vendorExtension,
                                       const String &appName,
                                       D3D11Driver* d3dDriver, D3D_DRIVER_TYPE driverType,
                                       D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL,
                                       D3D_FEATURE_LEVEL* pFeatureLevel,
                                       ID3D11Device **outDevice );

        D3D11DriverList* getDirect3DDrivers(bool refreshList = false);
        void refreshD3DSettings(void);
        void refreshFSAAOptions(void);

        void freeDevice(void);
        void createDevice( const String &windowTitle );

        v1::D3D11HardwareBufferManager* mHardwareBufferManager;
        D3D11GpuProgramManager* mGpuProgramManager;
        D3D11HLSLProgramFactory* mHLSLProgramFactory;

        /// Internal method for populating the capabilities structure
        RenderFeature* setupFeature() const;
        /** See RenderSystem definition */
        void initImpl(RenderFeature* caps, Window* primary);

        void convertVertexShaderCaps(RenderFeature* rsc) const;
        void convertPixelShaderCaps(RenderFeature* rsc) const;
        void convertGeometryShaderCaps(RenderFeature* rsc) const;
        void convertHullShaderCaps(RenderFeature* rsc) const;
        void convertDomainShaderCaps(RenderFeature* rsc) const;
        void convertComputeShaderCaps(RenderFeature* rsc) const;

        bool checkVertexTextureFormats(void);

        ID3D11Buffer    *mBoundIndirectBuffer;
        unsigned char   *mIndirectPtr;
        D3D11HlmsPso    *mPso;
        D3D11HLSLProgram* mBoundComputeProgram;
        uint32          mMaxBoundUavCS;

        uint            mNumberOfViews;
        ID3D11RenderTargetView *mRenderTargetViews[NII_MAX_RenderTarget];
        ID3D11DepthStencilView *mDepthStencilView;

        /// For rendering legacy objects.
        v1::VertexData  *mCurrentVertexBuffer;
        v1::IndexData   *mCurrentIndexBuffer;

        //ShaderChTextureUnit::BindingType mBindingType;

        UINT                        mStencilRef;

        // List of class instances per shader stage
        ID3D11ClassInstance* mClassInstances[6][8];

        // Number of class instances per shader stage
        UINT mNumClassInstances[6];
        
        // Store created shader subroutines, to prevent creation and destruction every frame
        typedef std::map<String, ID3D11ClassInstance*> ClassInstanceMap;
        typedef std::map<String, ID3D11ClassInstance*>::iterator ClassInstanceIterator;
        ClassInstanceMap mInstanceMap;

        D3D11FrameBufferDescMap mFrameBufferDescMap;


        /// Primary window, the one used to create the device
        D3D11Window* mPrimaryWindow;

        typedef vector<D3D11Window*>::type SecondaryWindowList;
        // List of additional windows after the first (swap chains)
        SecondaryWindowList mSecondaryWindows;

        bool mRenderSystemWasInited;

        D3D11PixelFormatToShaderType mD3D11PixelFormatToShaderType;

        ID3D11ShaderResourceView *mNullViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
        uint32 mMaxSrvCount[ST_PCnt];
        uint32 mMaxComputeShaderSrvCount;

        String mLastWindowTitlePassedToExtensions;
		
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
		D3D11StereoDriverBridge* mStereoDriver;
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
		Windows::Foundation::EventRegistrationToken suspendingToken, surfaceContentLostToken;
#endif

    protected:

        void setClipPlanesImpl(const PlaneList& clipPlanes);

    public:
        // constructor
        D3D11RenderSystem( );

        // destructor
        ~D3D11RenderSystem();
		
		
		int getSwitchingFullscreenCounter() const					{ return mSwitchingFullscreenCounter; }
        void addToSwitchingFullscreenCounter()                      { ++mSwitchingFullscreenCounter; }
		
        void initRenderSystem();

        void initConfigOptions(void);

        // Overridden RenderSystem functions
        ConfigOptionMap& getConfig(void);
        String checkConfig(void);
        Window* init( bool autoCreateWindow, const String& windowTitle = "OGRE Render Window"  );
        /// @copydoc RenderSystem::createWindow
        Window* createWindow( const String &name, uint32 width, uint32 height,
                                     bool fullScreen, const PropertyData *miscParams = 0 );
        void _notifyWindowDestroyed( Window *window );

        /// @copydoc RenderSystem::fireDeviceEvent
        void fireDeviceEvent( D3D11Device* device, const String & name, D3D11Window *sendingWindow );
#if !TODO_OGRE_2_2
        void fireDeviceEvent( D3D11Device* device, const String & name, D3D11RenderWindowBase *sendingWindow = NULL ) {}
#endif

        virtual void _setContext( Texture *texture ) {}

        virtual D3D11FrameBufferDescMap& _getFrameBufferDescMap(void)   { return mFrameBufferDescMap; }
        virtual FrameBufferObject* createFBO(void);
        virtual void beginFBO(FrameBufferObject * fbo, const ViewportList & vlist);
        virtual void endFBO(void);

        Texture* createDepthImpl( Texture *colourTexture, bool preferDepthTexture,
                                          PixelFormat depthBufferFormat );

        const String& getName(void) const;
		
		const String& getFriendlyName(void) const;
		
        void getPlatformData(const String& name, void* pData);
        // Low-level overridden members
        void setConfig( const String &name, const String &value );
        void reinit();
        void shutdown();
        bool checkDevice(bool forceDeviceElection = false);
        void handleDeviceLost();
        void setShadingType( ShadeOptions so );
        void setLightingEnabled( bool enabled );
        DataType getColourType(void) const;
        virtual void setStencil( uint32 refValue, const ShaderChStencil &stencil );
        void setNormaliseNormals(bool normalise);

        virtual String getErrorString(long errorNumber) const;

        // Low-level overridden members, mainly for internal use
        D3D11HLSLProgram* _getBoundComputeProgram() const;
        void _setLight(const LightList& lights, unsigned short limit);
        void _setModel( const Matrix4 &m );
        void _setView( const Matrix4 &m );
        void _setProj( const Matrix4 &m );
        void setColour( const Colour &ambient, const Colour &diffuse, const Colour &specular, const Colour &emissive, Real shininess, TrackVertexColourType tracking );
        void setPoint(bool enabled);
        void setPoint(Real size, bool attenuationEnabled, 
            Real constant, Real linear, Real quadratic, Real minSize, Real maxSize);
        virtual void _bindTexture( size_t unit, Texture *texPtr );
        virtual void _setTexture( uint32 slotStart, const DescriptorSetTexture *set,
                                   uint32 hazardousTexIdx );
        virtual void _setTexture( uint32 slotStart, const ShaderChTexture *set );
        virtual void _setSampler( uint32 slotStart, const TextureSample *set );
        virtual void _setTextureCS( uint32 slotStart, const DescriptorSetTexture *set );
        virtual void _setTextureCS( uint32 slotStart, const ShaderChTexture *set );
        virtual void _setSamplerCS( uint32 slotStart, const TextureSample *set );
        virtual void _setBufferCS( uint32 slotStart, const GpuParamBuffer *set );
        //void _setBindingType(ShaderChTextureUnit::BindingType bindingType);
        void _bindVertexTexture(size_t unit, Texture *tex);
        void _bindGeometryTexture(size_t unit, Texture *tex);
        void _bindTessControlTexture(size_t unit, Texture *tex);
        void _bindDomainTexture(size_t unit, Texture *tex);
        void _setTextureCoordPrc(size_t unit, TextureCoordPrc m, const Frustum* frustum = 0);
        void _setTextureBlend( size_t unit, const TextureBlend& bm );
        void _setTextureMatrix( size_t unit, const Matrix4 &xform );

        virtual void createRSO( RenderStateObject *newPso );
        virtual void destroyRSO( RenderStateObject *pso );
        virtual void createStencil( ShaderChStencil *newBlock );
        virtual void destroyStencil( ShaderChStencil *block );
        virtual void createBlend( ShaderChBlend *newBlock );
        virtual void destroyBlend( ShaderChBlend *block );
        virtual void createTexSample( TextureSampleUnit *newBlock );
        virtual void destroyTexSample( TextureSampleUnit *block );
        virtual void _descriptorSetTextureCreated( DescriptorSetTexture *newSet );
        virtual void _descriptorSetTextureDestroyed( DescriptorSetTexture *set );
        virtual void createTexture( ShaderChTexture *newSet );
        virtual void destroyTexture( ShaderChTexture *set );
        virtual void createBuffer( GpuParamBuffer *newSet );
        virtual void destroyBuffer( GpuParamBuffer *set );
        void _setHlmsDepthblock( const ShaderChStencil * stencil );
        void _setHlmsBlendblock( const ShaderChBlend * blend );
        virtual void _setTextureSample( uint8 texUnit, const TextureSampleUnit *samplerblock );
        virtual void setRSO( const RenderStateObject *pso );

        virtual void _setIndirect( IndirectBuffer * ib );

        virtual void createCSO( ComputeStateObject * cso );
        virtual void destroyCSO( ComputeStateObject * cso );
        virtual void setCSO( const ComputeStateObject *pso );

        void _beginFrame(void);
        void _endFrame(void);
        void _setFog( FogMode mode = FOG_NONE, const Colour& colour = Colour::White, Real expDensity = 1.0, Real linearStart = 0.0, Real linearEnd = 1.0 );
        void _render(const v1::GeometryRaw& op);

        virtual void _execute(const ComputeStateObject & pso);

        virtual void setGeometry(const GeometryRaw * vao);
        virtual void _render(const CbDrawCallIndexed * cmd);
        virtual void _render(const CbDrawCallStrip * cmd);
        virtual void _renderEmulated(const CbDrawCallIndexed * cmd);
        virtual void _renderEmulated(const CbDrawCallStrip * cmd);

        virtual void _setRenderOperation(const v1::GeometryRaw * cmd);
        virtual void _render(const v1::CbDrawCallIndexed * cmd);
        virtual void _render(const v1::CbDrawCallStrip * cmd);
        /** See
          RenderSystem
         */
        void on(GpuProgramType gptype, GpuProgramParam * params, uint16 mask);
        /** See
          RenderSystem
         */
        void bindGpuProgramPassIterationParameters(GpuProgramType gptype);

        virtual void clearBuffer( FrameBufferObject *fbobj,
                                       Texture *anyTarget, uint8 mipLevel );
        void setClipPlane (ushort index, Real A, Real B, Real C, Real D);
        void enableClipPlane (ushort index, bool enable);
        PixelCountQuery* create(void);
        Real getTexelXOffset(void);
        Real getTexelYOffset(void);
        Real getMinDepth(void);
        Real getMaxDepth(void);
        void registerThread();
        void unregisterThread();
        void preExtraThreadsStarted();
        void postExtraThreadsStarted();

        virtual SampleType checkSample( const SampleType &sampleDesc, PixelFormat format );

        /// @copydoc RenderSystem::getDisplayMonitorCount
        unsigned int getDisplayMonitorCount() const;

        /// @copydoc RenderSystem::hasAnisotropicMipMapFilter
        //virtual bool hasAnisotropicMipMapFilter() const { return true; }  

        D3D11Device &_getDevice() { return mDevice; }
        
        
        D3D_FEATURE_LEVEL _getFeatureLevel() const { return mFeatureLevel; }

        /// @copydoc RenderSystem::setSubroutine
        void setSubroutine(GpuProgramType gptype, unsigned int slotIndex, const String& subroutineName);
        
        /// @copydoc RenderSystem::setSubroutineName
        void setSubroutine(GpuProgramType gptype, const String& slotName, const String& subroutineName);

        /// @copydoc RenderSystem::beginProfiler
        virtual void beginProfiler( const String &eventName );

        /// @copydoc RenderSystem::endProfiler
        virtual void endProfiler( void );

        /// @copydoc RenderSystem::markProfiler
        virtual void markProfiler( const String &eventName );

        virtual void setupGPUProfiler(void);
        virtual void shutdownGPUProfiler(void);
        virtual void beginGPUProfiler( const String &name, uint32 *hashCache );
        virtual void endGPUProfiler( const String &name );
		
		/// @copydoc RenderSystem::getBufferType
		virtual bool getBufferType(ViewportType colourBuffer);
        
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

        virtual void _clear(void);
        virtual void _flush(void);
    };
    /** @} */
    /** @} */
}
#endif
