/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2016 Torus Knot Software Ltd

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

#ifndef _OgreMetalRenderSystem_H_
#define _OgreMetalRenderSystem_H_

#include "OgreMetalPrerequisites.h"
#include "OgreMetalPixelFormatToShaderType.h"
#include "OgreMetalRenderPassDescriptor.h"

#include "OgreRenderSystem.h"
#include "OgreMetalDevice.h"

#import <Metal/MTLRenderCommandEncoder.h>
#import <dispatch/dispatch.h>

namespace Ogre
{
    namespace v1
    {
        class GBufferManager;
    }

    /**
       Implementation of Metal as a rendering system.
    */
    class _OgreMetalExport MetalRenderSystem : public RenderSystem
    {
        struct CachedDepthStencilState
        {
            uint16                      refCount;
            bool                        depthWrite;
            CmpMode             depthFunc;
            ShaderChStencil               mStencil;

            id<MTLDepthStencilState>    depthStencilState;

            CachedDepthStencilState() : refCount( 0 ) {}

            bool operator < ( const CachedDepthStencilState &other ) const
            {
                if(   this->depthWrite < other.depthWrite  ) return true;
                if( !(this->depthWrite < other.depthWrite) ) return false;

                if(   this->depthFunc < other.depthFunc  ) return true;
                if( !(this->depthFunc < other.depthFunc) ) return false;

                if(   this->mStencil < other.mStencil  ) return true;
                //if( !(this->mStencil < other.mStencil) ) return false;

                return false;
            }

            bool operator != ( const CachedDepthStencilState &other ) const
            {
                return this->depthWrite != other.depthWrite ||
                       this->depthFunc != other.depthFunc ||
                       this->mStencil != other.mStencil;
            }
        };

        typedef vector<CachedDepthStencilState>::type CachedDepthStencilStateVec;

    private:
        String mDeviceName;    // it`s hint rather than hard requirement, could be ignored if empty or device removed
        bool mInitialized;
        v1::GBufferManager   *mHardwareBufferManager;
        MetalGpuProgramManager      *mGpuProgramManager;
        MetalProgramFactory         *mMetalProgramFactory;

        ConfigOptionMap mOptions;

        MetalPixelFormatToShaderType mPixelFormatToShaderType;

        __unsafe_unretained id<MTLBuffer>   mIndirectBuffer;
        unsigned char * mIndirectPtr;
        CachedDepthStencilStateVec mDepthStencilStates;
        MetalHlmsPso const * mPso;
        ComputeStateObject const * mComputePso;
        
        bool mStencilEnabled;
        uint32_t mStencilRefValue;

        //For v1 rendering.
        v1::IndexData * mCurrentIndexBuffer;
        v1::VertexData * mCurrentVertexBuffer;
        MTLPrimitiveType mCurrentPrimType;

        //TODO: AutoParamsBuffer probably belongs to MetalDevice (because it's per device?)
        typedef vector<StructBuffer*>::type StructBufferList;
        StructBufferList    mAutoParamsBuffer;
        size_t                  mAutoParamsBufferIdx;
        uint8                   *mCurrentAutoParamsBufferPtr;
        size_t                  mCurrentAutoParamsBufferSpaceLeft;
        size_t                  mHistoricalAutoParamsSize[60];

        uint8           mNumMRTs;
        MetalRenderTargetCommon     *mCurrentColourRTs[NII_MAX_RenderTarget];
        MetalDeviceList             mDeviceList;
        MetalDevice                 *mActiveDevice;
        __unsafe_unretained id<MTLRenderCommandEncoder> mActiveRenderEncoder;

        MetalDevice             mDevice;
        dispatch_semaphore_t    mMainGpuSyncSemaphore;
        bool                    mMainSemaphoreAlreadyWaited;
        bool                    mBeginFrameOnceStarted;

        MetalFrameBufferDescMap mFrameBufferDescMap;
        uint32                  mEntriesToFlush;
        bool                    mVpChanged;
        bool                    mInterruptedRenderCommandEncoder;

        MetalDeviceList* getDeviceList( bool refreshList = false );
        void refreshFSAAOptions(void);

        void setActiveDevice( MetalDevice *device );

        id<MTLDepthStencilState> getDepthStencilState( RenderStateObject *pso );
        void removeDepthStencilState( RenderStateObject *pso );

        void cleanAutoParamsBuffers(void);

    public:
        MetalRenderSystem();
        virtual ~MetalRenderSystem();

        virtual void shutdown(void);

        virtual const String& getName(void) const;
        virtual const String& getFriendlyName(void) const;
        void initConfigOptions();
        virtual ConfigOptionMap& getConfig(void) { return mOptions; }
        virtual void setConfig(const String &name, const String &value);

        virtual PixelCountQuery* create(void);

        virtual String checkConfig(void)  { return N_StrBlank; }

        virtual RenderFeature* setupFeature(void) const;

        virtual void reinit(void);

        virtual Window* init( bool autoCreateWindow, const String & windowTitle = "OGRE Render Window" );

        virtual Window* createWindow( const String & name, uint32 width, uint32 height, bool fullScreen, const PropertyData * miscParams = 0);

        virtual String getErrorString(long errorNumber) const;

        bool hasStoreAndMultisampleResolve(void) const;

        virtual void _setLight(const LightList& lights, unsigned short limit);
        virtual void _setModel(const Matrix4 &m);
        virtual void _setView(const Matrix4 &m);
        virtual void _setProj(const Matrix4 &m);

        virtual void setColour( const Colour &ambient,
                                const Colour &diffuse, const Colour &specular,
                                const Colour &emissive, Real shininess,
                                TrackVertexColourType tracking = TVC_NONE );
        virtual void setPoint(bool enabled);
        virtual void setPoint(Real size, bool attenuationEnabled,
            Real constant, Real linear, Real quadratic, Real minSize, Real maxSize);

        virtual void _bindTexture( size_t unit, TextureGpu *texPtr );
        virtual void _setTexture( uint32 slotStart, const DescriptorSetTexture *set,
                                   uint32 hazardousTexIdx );
        virtual void _setTexture( uint32 slotStart, const ShaderChTexture *set );
        virtual void _setSampler( uint32 slotStart, const DescriptorSetSampler *set );
        virtual void _setTextureCS( uint32 slotStart, const DescriptorSetTexture *set );
        virtual void _setTextureCS( uint32 slotStart, const ShaderChTexture *set );
        virtual void _setSamplerCS( uint32 slotStart, const DescriptorSetSampler *set );
        virtual void _setBufferCS( uint32 slotStart, const GpuParamBuffer *set );

        virtual void _setContext(TextureGpu * texture);
        virtual MetalFrameBufferDescMap & _getFrameBufferDescMap(void)   { return mFrameBufferDescMap; }
        virtual FrameBufferObject* createFBO(void);
        virtual void beginFBO(FrameBufferObject * fbo, const ViewportList & vlist);
        void executeFBO(bool officialCall);
        virtual void executeFBO(void);
        inline void endFBO(bool isInterruptingRender);
        virtual void endFBO(void);
    protected:
        virtual Texture * createDepthImpl( Texture  *colourTexture, bool texture, PixelFormat pf );
    public:
        virtual void _setTextureCoordPrc(size_t unit, TextureCoordPrc m,
                                                 const Frustum* frustum = 0);
        virtual void _setTextureBlend(size_t unit, const TextureBlend& bm);
        virtual void _setTextureMatrix(size_t unit, const Matrix4& xform);

        virtual void _setIndirect(IndirectBuffer * ib);

        virtual void createCSO(ComputeStateObject * newPso);
        virtual void destroyCSO(ComputeStateObject *pso);

        virtual void setStencil( uint32 refValue, const ShaderChStencil &stencil );

        /// See GBufferManager::wait
        virtual void _waitForTailFrameToFinish(void);

        virtual void _beginFrameOnce(void);
        virtual void _endFrameOnce(void);

        virtual void _beginFrame(void);
        virtual void _endFrame(void);

        virtual void createRSO( RenderStateObject *newPso );
        virtual void destroyRSO( RenderStateObject *pso );
        virtual void createTexSample( TextureSampleUnit *newBlock );
        virtual void destroyTexSample( TextureSampleUnit *block );
    protected:
        template <typename TDescriptorSetTexture,
                  typename TTexSlot,
                  typename TBufferPacked,
                  bool isbufstroge>
        void _descriptorSetTextureCreated( TDescriptorSetTexture *newSet,
                                           const vectorL<TTexSlot> &texContainer,
                                           uint16 *shaderTypeTexCount );
        void destroyMetalDescriptorSetTexture( MetalDescriptorSetTexture *metalSet );
    public:
        virtual void createTexture( ShaderChTexture *newSet );
        virtual void destroyTexture( ShaderChTexture *set );
        virtual void createBuffer( GpuParamBuffer *newSet );
        virtual void destroyBuffer( GpuParamBuffer *set );

        virtual void _setTextureSample( uint8 texUnit, const TextureSampleUnit *samplerblock );
        virtual void setRSO( const RenderStateObject *pso );
        virtual void setCSO( const ComputeStateObject *pso );

        virtual DataType getColourType(void) const;

        virtual void _execute( const ComputeStateObject &pso );

        virtual void setGeometry( const GeometryRaw *vao );

        virtual void _render( const CbDrawCallIndexed *cmd );
        virtual void _render( const CbDrawCallStrip *cmd );
        //virtual void _renderEmulated( const CbDrawCallIndexed *cmd );
        //virtual void _renderEmulated( const CbDrawCallStrip *cmd );

        //virtual void _setRenderOperation( const v1::GeometryRaw *cmd );
        //virtual void _render( const v1::CbDrawCallIndexed *cmd );
        //virtual void _render( const v1::CbDrawCallStrip *cmd );
        //virtual void _render( const v1::GeometryRaw &op );

        virtual void on(GpuProgramType gptype, GpuProgramParam * params, uint16 variabilityMask);
        virtual void bindGpuProgramPassIterationParameters(GpuProgramType gptype);

        virtual void clearBuffer(FrameBufferObject * fbobj, TextureGpu * anyTarget, uint8 mipLevel);

        virtual Real getTexelXOffset(void);
        virtual Real getTexelYOffset(void);
        virtual Real getMinDepth(void);
        virtual Real getMaxDepth(void);

        virtual void preExtraThreadsStarted();
        virtual void postExtraThreadsStarted();
        virtual void registerThread();
        virtual void unregisterThread();
        virtual unsigned int getDisplayMonitorCount() const     { return 1; }

        virtual SampleType checkSample(const SampleType & st, PixelFormat pf);
        
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
        virtual const char* getToken(PixelFormat pf) const = 0;

        /**
        @param pixelFormat
        @param textureType
            See Texture::Type::Texture::Type
        @param isMsaa
        @param access
            Texture access. Use OpType::OT_UNKNOW for requesting sampling mode
        @return
            String for the shader to use "as is". Returned pointer may be null.
            The validity of the pointer lasts as long as the RenderSystem remains
            loaded.
        */
        virtual const char* getToken(PixelFormat pf, uint32 textureType, bool isMsaa, Buffer::OpType access) const = 0;

        //virtual const PixelFormatToShaderType* getPixelFormatToShaderType(void) const;

        virtual void beginProfiler( const String &eventName );

        virtual void endProfiler( void );

        virtual void markProfiler( const String &event );

        virtual void setupGPUProfiler(void);
        virtual void shutdownGPUProfiler(void);
        virtual void beginGPUProfiler( const String &name, uint32 *hashCache );
        virtual void endGPUProfiler( const String &name );

        //virtual bool hasAnisotropicMipMapFilter() const         { return true; }

        virtual void setClipPlanesImpl(const PlaneList& clipPlanes);
        virtual void initImpl(RenderFeature* caps, Window *primary);
        virtual void updateCompositorManager(CompositorManager2 *compositorManager);

        virtual void _flush();

        MetalDevice* getActiveDevice()                          { return mActiveDevice; }
        MetalProgramFactory* getMetalProgramFactory()           { return mMetalProgramFactory; }

        void _notifyActiveEncoderEnded( bool callEndRenderPassDesc );
        void _notifyActiveComputeEnded(void);
        void _notifyNewCommandBuffer(void);
        void _notifyDeviceStalled(void);


    };
}

#endif
