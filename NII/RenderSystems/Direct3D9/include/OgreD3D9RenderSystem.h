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
#ifndef __D3D9RENDERSYSTEM_H__
#define __D3D9RENDERSYSTEM_H__

#include "OgreD3D9Prerequisites.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreRenderSystem.h"
#include "OgreRenderSystemCapabilities.h"
#include "OgreD3D9Mappings.h"

namespace Ogre 
{
    /** \addtogroup RenderSystems RenderSystems
    *  @{
    */
    /** \defgroup Direct3D9 Direct3D9
    * Implementation of DirectX9 as a rendering system.
    *  @{
    */
#define MAX_LIGHTS 8

    class D3D9DriverList;
    class D3D9Driver;
    class D3D9Device;
    class D3D9DeviceManager;
    class D3D9ResourceManager;
	class D3D9StereoDriverBridge;
	
    /**
    Implementation of DirectX9 as a rendering system.
    */
    class _OgreD3D9Export D3D9RenderSystem : public RenderSystem
    {
    public:
        enum MultiheadUseType
        {
            mutAuto,
            mutYes,
            mutNo
        };
        
    private:
        /// Direct3D
        IDirect3D9*  mD3D;
        // TODO: remove following fields, use values directly from mOptions map as other render systems does
        size_t mFSAASamples;
        String mFSAAHint;
        bool mVSync; 
        unsigned int mVSyncInterval;
        unsigned int mBackBufferCount; // -1 means 2 for vsync and 1 for no vsync

        /// instance
        HINSTANCE mhInstance;

        /// List of D3D drivers installed (video cards)
        D3D9DriverList* mDriverList;
        /// Currently active driver
        D3D9Driver* mActiveD3DDriver;
        /// NVPerfHUD allowed?
        bool mUseNVPerfHUD;
        /// Per-stage constant support? (not in main caps since D3D specific & minor)
        bool mPerStageConstantSupport;
        /// Fast singleton access.
        static D3D9RenderSystem* msD3D9RenderSystem;
        /// Tells whether to attempt to initialize the system with DirectX 9Ex driver
        /// Read more in http://msdn.microsoft.com/en-us/library/windows/desktop/ee890072(v=vs.85).aspx
        bool mAllowDirectX9Ex;
        /// Tells whether the system is initialized with DirectX 9Ex driver
        /// Read more in http://msdn.microsoft.com/en-us/library/windows/desktop/ee890072(v=vs.85).aspx
        bool mIsDirectX9Ex;

        bool mEnableFixedPipeline;

        /// structure holding texture unit settings for every stage
        struct sD3DTextureStageDesc
        {
            /// the type of the texture
            D3D9Mappings::eD3DTexType texType;
            /// which texCoordIndex to use
            size_t coordIndex;
            /// type of auto tex. calc. used
            TextureCoordPrc autoTexCoordType;
            /// Frustum, used if the above is projection
            const Frustum *frustum;
            /// texture 
            IDirect3DBaseTexture9 *pTex;
            /// vertex texture 
            IDirect3DBaseTexture9 *pVertexTex;
        } mTexStageDesc[NII_MAX_TextureLayer];

        // Array of up to 8 lights, indexed as per API
        // Note that a null value indicates a free slot
        Light* mLights[MAX_LIGHTS];     
        D3D9DriverList* getDirect3DDrivers();
        void refreshD3DSettings();
        void refreshFSAAOptions();
        
        void setD3D9Light( size_t index, bool enabled);
        
        // state management methods, very primitive !!!
        HRESULT __SetRenderState(D3DRENDERSTATETYPE state, DWORD value);
        HRESULT __SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value);
        HRESULT __SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);

        HRESULT __SetFloatRenderState(D3DRENDERSTATETYPE state, Real value)
        {
#if OGRE_DOUBLE_PRECISION == 1
            float temp = static_cast<float>(value);
            return __SetRenderState(state, *((LPDWORD)(&temp)));
#else
            return __SetRenderState(state, *((LPDWORD)(&value)));
#endif
        }

        /// return anisotropy level
        DWORD _getCurrentAnisotropy(size_t unit);
        /// check if a FSAA is supported
        bool _checkMultiSampleQuality(D3DMULTISAMPLE_TYPE type, DWORD *outQuality, D3DFORMAT format, UINT adapterNum, D3DDEVTYPE deviceType, BOOL fullScreen);
        
        D3D9HardwareBufferManager* mHardwareBufferManager;
        D3D9GpuProgramManager* mGpuProgramManager;
        D3D9HLSLProgramFactory* mHLSLProgramFactory;
        D3D9ResourceManager* mResourceManager;
        D3D9DeviceManager* mDeviceManager;

        size_t mLastVertexSourceCount;


        /// Internal method for populating the capabilities structure
        virtual RenderFeature* setupFeature() const;
        RenderFeature* updateRenderSystemCapabilities(D3D9RenderWindow* renderWindow);

        /** See RenderSystem definition */
        virtual void initImpl(RenderFeature* caps, RenderTarget* primary);


        void convertVertexShaderCaps(RenderFeature* rsc) const;
        void convertPixelShaderCaps(RenderFeature* rsc) const;
        bool checkVertexTextureFormats(D3D9RenderWindow* renderWindow) const;
        void detachRenderTargetImpl(const String& name);
        
        std::unordered_map<IDirect3DDevice9*, unsigned short> mCurrentLights;
        /// Saved last view matrix
        Matrix4 mViewMatrix;

        D3DXMATRIX mDxViewMat, mDxProjMat, mDxWorldMat;
    
        typedef std::vector<D3D9RenderWindow*> D3D9RenderWindowList;
        // List of additional windows after the first (swap chains)
        D3D9RenderWindowList mRenderWindows;
        
        /** Mapping of texture format -> DepthStencil. Used as cache by _getDepthStencilFormatFor
        */
        typedef std::unordered_map<unsigned int, D3DFORMAT> DepthStencilHash;
        DepthStencilHash mDepthStencilHash;

        MultiheadUseType mMultiheadUse;

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        D3D9StereoDriverBridge* mStereoDriver;
#endif

    protected:
        void setClipPlanesImpl(const PlaneList& clipPlanes);
        void setWorldMatrix( const Matrix4 &m );
        void setViewMatrix( const Matrix4 &m );
        void setProjectionMatrix( const Matrix4 &m );
    public:
        D3D9RenderSystem( HINSTANCE hInstance );
        ~D3D9RenderSystem();

        virtual void initConfigOptions();

        const GpuProgramParam *& getFixedFunctionParams(TrackVertexColourType tracking, FogMode fog);
        void applyFixedFunctionParams(const GpuProgramParam *& params, uint16 variabilityMask);

        // Overridden RenderSystem functions
        String checkConfig();
        void init() override;
        
        /// @copydoc RenderSystem::createWindow
        RenderWindow* createWindow(const String &name, unsigned int width, unsigned int height, 
            bool fullScreen, const PropertyData *miscParams = 0);
        
        /// @copydoc RenderSystem::createWindow
        bool createWindow(const WindowDefineList& renderWindowDescriptions, 
            RenderWindowList& createdWindows);

        /// @copydoc RenderSystem::_createDepthBufferFor
        DepthBuffer* _createDepthBufferFor( RenderTarget *renderTarget );

        /**
         * This function is meant to add Depth Buffers to the pool that aren't released when the DepthBuffer
         * is deleted. This is specially useful to put the Depth Buffer created along with the window's
         * back buffer into the pool. All depth buffers introduced with this method go to GroupInner
         */
        DepthBuffer* _addManualDepthBuffer( IDirect3DDevice9* depthSurfaceDevice, IDirect3DSurface9 *surf );

        /**
         * This function does NOT override RenderSystem::_cleanupDepthBuffers(bool) functionality.
         * On multi monitor setups, when a device becomes "inactive" (it has no RenderWindows; like
         * when the window was moved from one monitor to another); the Device will be destroyed,
         * meaning all it's depth buffers (auto & manual) should be removed from the pool,
         * but only selectively removing those created by that D3D9Device.
         * @param:
         *      Creator device to compare against. Shouldn't be null
         */
        using RenderSystem::_cleanupDepthBuffers;
        void _cleanupDepthBuffers( IDirect3DDevice9 *creator );

        /**
         * This function does NOT override RenderSystem::_cleanupDepthBuffers(bool) functionality.
         * Manually created surfaces may be released arbitrarely without being pulled out from the pool
         * (specially RenderWindows) this function takes care of that.
         * @param manualSurface
         *      Depth buffer surface to compare against. Shouldn't be null
         */
        void _cleanupDepthBuffers( IDirect3DSurface9 *manualSurface );

        /**
         * Set current render target to target, enabling its GL context if needed
         */
        void _setRenderTarget(RenderTarget *target);
        
        /// @copydoc RenderSystem::createMultiRenderTarget
        virtual MultiRenderTarget * createMultiRenderTarget(const String & name);

        /// @copydoc RenderSystem::detachRenderTarget
        virtual RenderTarget * detachRenderTarget(const String &name);

        String getErrorString( long errorNumber ) const;
        const String& getName() const;

        // Low-level overridden members
        bool setConfig( const String &name, const String &value );
        void reinit();
        void shutdown();
        void setAmbientLight( float r, float g, float b );
        void setShadingType( ShadeOptions so );
        void setLightingEnabled( bool enabled );
        void destroyRenderTarget(const String& name);
        DataType getColourType() const;
        void setStencilCheckEnabled(bool enabled);
        void setStencil(CmpMode func = CMPF_ALWAYS_PASS, 
            uint32 refValue = 0, uint32 compareMask = 0xFFFFFFFF, uint32 writeMask = 0xFFFFFFFF,
            StencilOpType stencilFailOp = SOP_KEEP, 
            StencilOpType depthFailOp = SOP_KEEP,
            StencilOpType passOp = SOP_KEEP, 
            bool twoSidedOperation = false,
            bool readBackAsTexture = false);
        void setNormaliseNormals(bool normalise);

        // Low-level overridden members, mainly for internal use
        void _setLight(unsigned short limit);
        void _setSurfaceTracking( TrackVertexColourType tracking );
        void setPoint(bool enabled);
        void setPoint(bool attenuationEnabled, Real minSize, Real maxSize);
        void _bindTexture(size_t unit, bool enabled, const TexturePtr& texPtr);
        void _setSampler(size_t unit, TextureSampleUnit& sampler);
        void _bindVertexTexture(size_t unit, const TexturePtr& tex);
        void _disableTextureUnit(size_t texUnit);
        void _setTextureCoordSet( size_t unit, size_t index );
        void _setTextureCoordPrc(size_t unit, TextureCoordPrc m, 
            const Frustum* frustum = 0);
        void _setTextureBlend( size_t unit, const TextureBlend& bm );
        void _setTextureAddressingMode(size_t stage, const TextureSampleUnit::UVWAddressingMode& uvw);
        void _setTextureMatrix( size_t unit, const Matrix4 &xform );
        void _setSeparateSceneBlending( ColourFactor sourceFactor, ColourFactor destFactor, ColourFactor sourceFactorAlpha, ColourFactor destFactorAlpha, ColourBlendMode op, ColourBlendMode alphaOp );
        void _setAlphaRejectSettings( CmpMode func, unsigned char value, bool alphaToCoverage );
        void _setViewport( Viewport *vp );      
        void _beginFrame();
        virtual FrameObj* _pauseFrame(void);
        virtual void _resumeFrame(FrameObj* context);
        void _endFrame();       
        void _setCullingMode( CullingMode mode );
        void _setDepthBufferParams( bool depthTest = true, bool depthWrite = true, CmpMode depthFunction = CMPF_LESS_EQUAL );
        void _setDepthBufferCheckEnabled( bool enabled = true );
        void _setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);
        void _setDepthBufferWriteEnabled(bool enabled = true);
        void _setDepthBufferFunction( CmpMode func = CMPF_LESS_EQUAL );
        void _setDepthBias(float constantBias, float slopeScaleBias);
        void _setFog( FogMode mode);
        void getSysProj(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram = false);
        void _setPolygonMode(PolygonMode level);
        void _setTextureUnitFiltering(size_t unit, FilterType ftype, TextureFilterOP filter);
        void setVertexDeclaration(VertexDeclaration* decl);
        void setVertexDeclaration(VertexDeclaration* decl, bool useGlobalInstancingVertexBufferIsAvailable);
        void setVertexBufferBinding(VertexBufferBinding* binding);
        void setVertexBufferBinding(VertexBufferBinding* binding, size_t instanceCnt, bool useGlobalInstancingVertexBufferIsAvailable, bool indexesUsed);
        void _render(const GeometryRaw& op);

        void bindGpuProgram(GpuProgram* prg);

        void unbindGpuProgram(GpuProgramType gptype);

        void on(GpuProgramType gptype, const GpuProgramParam *& params, uint16 variabilityMask);
        void bindGpuProgramPassIterationParameters(GpuProgramType gptype);

        void setScissorTest(bool enabled, size_t left = 0, size_t top = 0, size_t right = 800, size_t bottom = 600);
        void clearBuffer(unsigned int buffers, 
            const Colour& colour = Colour::Black, 
            Real depth = 1.0f, unsigned short stencil = 0);
        void setClipPlane (ushort index, Real A, Real B, Real C, Real D);
        void enableClipPlane (ushort index, bool enable);
        PixelCountQuery* create();
        Real getTexelXOffset();
        Real getTexelYOffset();
        Real getMinDepth();
        Real getMaxDepth();
        void registerThread();
        void unregisterThread();
        void preExtraThreadsStarted();
        void postExtraThreadsStarted();     
                
        /*
        Returns whether under the current render system buffers marked as TU_STATIC can be locked for update
        */
        virtual bool isStaticUpdate() const { return !mIsDirectX9Ex; }

		bool IsActiveDeviceLost();

        /// Tells whether the system is initialized with DirectX 9Ex driver
        /// Read more in http://msdn.microsoft.com/en-us/library/windows/desktop/ee890072(v=vs.85).aspx
        static bool isDirectX9Ex()  { return msD3D9RenderSystem->mIsDirectX9Ex; }

        static D3D9ResourceManager* getResourceManager();
        static D3D9DeviceManager* getDeviceManager();
        static IDirect3D9* getDirect3D9();
        static UINT getResourceCreationDeviceCount();
        static IDirect3DDevice9* getResourceCreationDevice(UINT index);
        static IDirect3DDevice9* getActiveD3D9Device();
        static IDirect3DDevice9* getActiveD3D9DeviceIfExists();

        /** Check which depthStencil formats can be used with a certain pixel format,
            and return the best suited.
        */
        D3DFORMAT _getDepthStencilFormatFor(D3DFORMAT fmt);

        /** Check whether or not filtering is supported for the precise texture format requested
        with the given usage options.
        */
        bool _checkTextureFilteringSupported(TextureType ttype, PixelFormat format, int usage);

        /// Take in some requested FSAA settings and output supported D3D settings
        void determineFSAASettings(IDirect3DDevice9* d3d9Device, size_t fsaa, const String& fsaaHint, D3DFORMAT d3dPixelFormat, 
            bool fullScreen, D3DMULTISAMPLE_TYPE *outMultisampleType, DWORD *outMultisampleQuality);

        /// @copydoc RenderSystem::getDisplayMonitorCount
        unsigned int getDisplayMonitorCount() const;

        /// @copydoc RenderSystem::beginProfiler
        virtual void beginProfiler( const String &eventName );

        /// @copydoc RenderSystem::endProfiler
        virtual void endProfiler( void );

        /// @copydoc RenderSystem::markProfiler
        virtual void markProfiler( const String &eventName );
            
        /// Fires a device related event
        void fireDeviceEvent( D3D9Device* device, const String & name );

        /// Returns how multihead should be activated
        MultiheadUseType getMultiheadUse() const { return mMultiheadUse; }

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        /// @copydoc RenderSystem::getBufferType
        virtual bool getBufferType(ViewportType colourBuffer);

        /// Creates a bridge to the Direct3D stereo driver implementation
        void createStereoDriver(const PropertyData* miscParams);
#endif

    protected:  
        /// Returns the sampler id for a given unit texture number
        DWORD getSamplerId(size_t unit);

        /// Notify when a device has been lost.
        void notifyOnDeviceLost(D3D9Device* device);

        /// Notify when a device has been reset.
        void notifyOnDeviceReset(D3D9Device* device);

    private:
        friend class D3D9Device;
        friend class D3D9DeviceManager;     
    };
    /** @} */
    /** @} */
}
#endif
