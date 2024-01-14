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

#include "OgreMetalRenderSystem.h"
#include "OgreMetalWindow.h"
#include "Vao/OgreMetalGBufferManager.h"
#include "Vao/OgreMetalBufferInterface.h"
#include "OgreMetalHlmsPso.h"
#include "OgreMetalRenderTargetCommon.h"
#include "OgreMetalDevice.h"
#include "OgreMetalGpuProgramManager.h"
#include "OgreMetalProgram.h"
#include "OgreMetalProgramFactory.h"
#include "OgreMetalTextureGpu.h"
#include "OgreMetalRenderPassDescriptor.h"
#include "OgreMetalDescriptorSetTexture.h"
#include "Vao/OgreMetalTexBufferPacked.h"

#include "OgreMetalHardwareBufferManager.h"
#include "OgreMetalHardwareIndexBuffer.h"
#include "OgreMetalHardwareVertexBuffer.h"

#include "OgreMetalTextureGpuManager.h"

#include "Vao/OgreMetalConstBufferPacked.h"
#include "Vao/OgreMetalUavBufferPacked.h"
#include "Vao/OgreIndirectBufferPacked.h"
#include "Vao/OgreVertexArrayObject.h"
#include "RenderCommandGroup/OgreCbDrawCall.h"

#include "OgreDepthBuffer.h"

#include "OgreFrustum.h"
#include "OgreViewport.h"
#include "Compositor/OgreCompositorManager2.h"

#include "OgreMetalMappings.h"

#import <Metal/Metal.h>
#import <Foundation/NSEnumerator.h>

#include <sstream>

namespace Ogre
{
    //-------------------------------------------------------------------------
    MetalRenderSystem::MetalRenderSystem() :
        RenderSystem(),
        mInitialized( false ),
        mHardwareBufferManager( 0 ),
        mGpuProgramManager( 0 ),
        mMetalProgramFactory( 0 ),
        mIndirectBuffer( 0 ),
        mIndirectPtr( 0 ),
        mPso( 0 ),
        mComputePso( 0 ),
        mCurrentIndexBuffer( 0 ),
        mCurrentVertexBuffer( 0 ),
        mCurrentPrimType( MTLPrimitiveTypePoint ),
        mAutoParamsBufferIdx( 0 ),
        mCurrentAutoParamsBufferPtr( 0 ),
        mCurrentAutoParamsBufferSpaceLeft( 0 ),
        mNumMRTs( 0 ),
        mActiveDevice( 0 ),
        mActiveRenderEncoder( 0 ),
        mDevice( this ),
        mMainGpuSyncSemaphore( 0 ),
        mMainSemaphoreAlreadyWaited( false ),
        mBeginFrameOnceStarted( false ),
        mEntriesToFlush( 0 ),
        mVpChanged( false ),
        mInterruptedRenderCommandEncoder( false )
    {
        memset( mHistoricalAutoParamsSize, 0, sizeof(mHistoricalAutoParamsSize) );
        for( size_t i=0; i<NII_MAX_RenderTarget; ++i )
            mCurrentColourRTs[i] = 0;
        
        // set config options defaults
        initConfigOptions();
    }
    //-------------------------------------------------------------------------
    MetalRenderSystem::~MetalRenderSystem()
    {
        shutdown();
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::shutdown(void)
    {
        if( mActiveDevice )
            mActiveDevice->endAllEncoders();
        
        for( size_t i=0; i<mAutoParamsBuffer.size(); ++i )
        {
            if( mAutoParamsBuffer[i]->getMappingState() != 0 )
                mAutoParamsBuffer[i]->unmap( UT_UnlockMap );
            mBufferManager->destroyConstBuffer( mAutoParamsBuffer[i] );
        }
        mAutoParamsBuffer.clear();
        mAutoParamsBufferIdx = 0;
        mCurrentAutoParamsBufferPtr = 0;
        mCurrentAutoParamsBufferSpaceLeft = 0;

        RenderSystem::shutdown();

        N_delete mHardwareBufferManager;
        mHardwareBufferManager = 0;

        if( mMetalProgramFactory )
        {
            // Remove from manager safely
            if( HighLevelGpuProgramManager::getSingletonPtr() )
                HighLevelGpuProgramManager::getSingleton().removeFactory( mMetalProgramFactory );
            N_delete mMetalProgramFactory;
            mMetalProgramFactory = 0;
        }

        N_delete mGpuProgramManager;
        mGpuProgramManager = 0;
    }
    //-------------------------------------------------------------------------
    const String& MetalRenderSystem::getName(void) const
    {
        static String strName("Metal Rendering Subsystem");
        return strName;
    }
    //-------------------------------------------------------------------------
    const String& MetalRenderSystem::getFriendlyName(void) const
    {
        static String strName("Metal_RS");
        return strName;
    }
    //-------------------------------------------------------------------------
    MetalDeviceList* MetalRenderSystem::getDeviceList( bool refreshList )
    {
        if( refreshList || mDeviceList.count() == 0 )
            mDeviceList.refresh();
        
        return &mDeviceList;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::initConfigOptions()
    {
        ConfigOption optDevice;
        ConfigOption optFSAA;
        ConfigOption optSRGB;

        optDevice.name = "Rendering Device";
        optDevice.currentValue = "(default)";
        optDevice.possibleValues.push_back("(default)");
        MetalDeviceList* deviceList = getDeviceList();
        for( unsigned j = 0; j < deviceList->count(); j++ )
            optDevice.possibleValues.push_back( deviceList->item(j)->getDescription() );
        optDevice.immutable = false;

        optFSAA.name = "FSAA";
        optFSAA.immutable = false;
        optFSAA.possibleValues.push_back( "None" );
        optFSAA.currentValue = "None";

        // SRGB on auto window
        optSRGB.name = "sRGB Gamma Conversion";
        optSRGB.possibleValues.push_back("Yes");
        optSRGB.possibleValues.push_back("No");
        optSRGB.currentValue = "Yes";
        optSRGB.immutable = false;

        mOptions[optDevice.name] = optDevice;
        mOptions[optFSAA.name] = optFSAA;
        mOptions[optSRGB.name] = optSRGB;

        refreshFSAAOptions();
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setConfig(const String &name, const String &value)
    {
        // Find option
        ConfigOptionMap::iterator it = mOptions.find( name );

        // Update
        if( it != mOptions.end() )
            it->second.currentValue = value;
        else
        {
            StringStream str;
            str << "Option named '" << name << "' does not exist.";
            //N_EXCEPT( Exception::ERR_INVALIDPARAMS, str.str(), "MetalRenderSystem::setConfig" );
        }
        
        // Refresh dependent options
        if( name == "Rendering Device" )
            refreshFSAAOptions();
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::refreshFSAAOptions(void)
    {
        ConfigOptionMap::iterator it = mOptions.find( "FSAA" );
        ConfigOption* optFSAA = &it->second;
        optFSAA->possibleValues.clear();

        it = mOptions.find("Rendering Device");
        if( @available( iOS 9.0, * ) )
        {
            const MetalDeviceItem *deviceItem = getDeviceList()->item( it->second.currentValue );
            id<MTLDevice> device = deviceItem ? deviceItem->getMTLDevice() : MTLCreateSystemDefaultDevice();
            for( unsigned samples = 1; samples <= 32; ++samples )
                if( [device supportsTextureSampleCount:samples] )
                    optFSAA->possibleValues.push_back( StrConv::conv(samples) + "x" );
        }
        
        if( optFSAA->possibleValues.empty() )
        {
            optFSAA->possibleValues.push_back( "1x" );
            optFSAA->possibleValues.push_back( "4x" );
        }
        
        // Reset FSAA to none if previous doesn't avail in new possible values
        if( std::find( optFSAA->possibleValues.begin(), optFSAA->possibleValues.end(),
                       optFSAA->currentValue ) == optFSAA->possibleValues.end() )
        {
            optFSAA->currentValue = optFSAA->possibleValues[0];
        }
    }
    //-------------------------------------------------------------------------
    SampleType MetalRenderSystem::checkSample( const SampleType &sampleDesc, PixelFormat format )
    {
        uint8 samples = sampleDesc.getMaxSampling();
        if( @available( iOS 9.0, * ) )
        {
            if( mActiveDevice )
            {
                while( samples > 1 && ![mActiveDevice->mDevice supportsTextureSampleCount:samples] )
                    --samples;
            }
        }
        return SampleType( samples, sampleDesc.getType() );
    }
    //-------------------------------------------------------------------------
    PixelCountQuery* MetalRenderSystem::create(void)
    {
        return 0; //TODO
    }
    //-------------------------------------------------------------------------
    RenderFeature* MetalRenderSystem::setupFeature(void) const
    {
        RenderFeature* rsc = new RenderFeature();
        rsc->setRenderSystemName(getName());

        rsc->setCapability(RSC_HWSTENCIL);
        rsc->setStencilBufferBitDepth(8);
        rsc->setMaxTexUnit(16);
        rsc->setNumVertexTextureUnits(16);
        rsc->setCapability(RSC_ANISOTROPY);
        rsc->setCapability(RSC_AUTOMIPMAP);
        rsc->setCapability(RSC_BLENDING);
        rsc->setCapability(RSC_DOT3);
        rsc->setCapability(RSC_CUBEMAPPING);
        rsc->setCapability(RSC_TEXTURE_COMPRESSION);
#if TARGET_OS_TV
        rsc->setCapability(RSC_TEXTURE_COMPRESSION_ASTC);
#endif
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);
        rsc->setCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5);
        //rsc->setCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7);
#else
        //Actually the limit is not the count but rather how many bytes are in the
        //GPU's internal TBDR cache (16 bytes for Family 1, 32 bytes for the rest)
        //Consult Metal's manual for more info.
        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v1] )
        {
            rsc->setCapability(RSC_TEXTURE_COMPRESSION_ASTC);
        }

        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily4_v1] )
            rsc->setCapability(CF_Tex_CubeArray);
#endif
        rsc->setCapability(RSC_VBO);
        rsc->setCapability(RSC_32BIT_INDEX);
        rsc->setCapability(RSC_TWO_SIDED_STENCIL);
        rsc->setCapability(RSC_STENCIL_WRAP);
        rsc->setCapability(RSC_USER_CLIP_PLANES);
        rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);
        rsc->setCapability(RSC_INFINITE_FAR_PLANE);
        rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
        rsc->setNonPOW2TexturesLimited(false);
        rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
        rsc->setCapability(RSC_TEXTURE_FLOAT);
        rsc->setCapability(RSC_POINT_SPRITES);
        rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
        rsc->setCapability(RSC_TEXTURE_1D);
        rsc->setCapability(RSC_TEXTURE_3D);
        rsc->setCapability(RSC_TEXTURE_SIGNED_INT);
        rsc->setCapability(RSC_VERTEX_PROGRAM);
        rsc->setCapability(RSC_FRAGMENT_PROGRAM);
        rsc->setCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA);
        rsc->setCapability(RSC_MIPMAP_LOD_BIAS);
        rsc->setCapability(RSC_ALPHA_TO_COVERAGE);
        rsc->setMaxPointSize(256);

        rsc->setCapability(RSC_COMPUTE_PROGRAM);
        rsc->setCapability(RSC_HW_GAMMA);
        rsc->setCapability(GF_Tex_Gather);
        rsc->setCapability(RSC_TEXTURE_2D_ARRAY);
        rsc->setCapability(RSC_SEPARATE_SAMPLERS_FROM_TEXTURES);
        rsc->setCapability(CF_Program_ConstBuffer);
        rsc->setCapability(RSC_EXPLICIT_FSAA_RESOLVE);

        //These don't make sense on Metal, so just use flexible defaults.
        rsc->setVertexProgramConstantFloatCount( 16384 );
        rsc->setVertexProgramConstantBoolCount( 16384 );
        rsc->setVertexProgramConstantIntCount( 16384 );
        rsc->setFragmentProgramConstantFloatCount( 16384 );
        rsc->setFragmentProgramConstantBoolCount( 16384 );
        rsc->setFragmentProgramConstantIntCount( 16384 );
        rsc->setComputeProgramConstantFloatCount( 16384 );
        rsc->setComputeProgramConstantBoolCount( 16384 );
        rsc->setComputeProgramConstantIntCount( 16384 );

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        uint8 mrtCount = 8u;
#else
        //Actually the limit is not the count but rather how many bytes are in the
        //GPU's internal TBDR cache (16 bytes for Family 1, 32 bytes for the rest)
        //Consult Metal's manual for more info.
        uint8 mrtCount = 4u;
        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v1] ||
            [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v2] ||
            [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v1] )
        {
            mrtCount = 8u;
        }
#endif
        rsc->setNumMultiRenderTargets( std::min<int>(mrtCount, NII_MAX_RenderTarget) );
        rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        uint16 max2DResolution = 16384;
#else
        uint16 max2DResolution = 4096;
        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v2] ||
            [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v2] )
        {
            max2DResolution = 8192;
        }
        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v1] )
        {
            max2DResolution = 16384;
        }
#endif
        rsc->setMaxTexSize( max2DResolution, 2048, max2DResolution );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v2] )
            rsc->setCapability(RSC_STORE_AND_MULTISAMPLE_RESOLVE);
#else
        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_OSX_GPUFamily1_v2] )
            rsc->setCapability(RSC_STORE_AND_MULTISAMPLE_RESOLVE);
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        rsc->setCapability( RSC_IS_TILER );
        rsc->setCapability( RSC_TILER_CAN_CLEAR_STENCIL_REGION );
#endif

        rsc->setCapability( CF_Program_StorgeBuffer );
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        rsc->setCapability(CF_Tex_CubeArray);
#endif
        rsc->setCapability( CF_Program_StorgeBuffer );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        if( [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily5_v1] )
            rsc->setCapability(CF_Program_ViwportArray);
#else
        rsc->setCapability(CF_Program_ViwportArray);
#endif

        //rsc->setCapability(RSC_ATOMIC_COUNTERS);

        rsc->addShaderProfile( "metal" );

        DriverVersion driverVersion;

        struct FeatureSets
        {
            MTLFeatureSet featureSet;
            const char* name;
            int major;
            int minor;
        };

        FeatureSets featureSets[] =
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            { MTLFeatureSet_iOS_GPUFamily1_v1, "iOS_GPUFamily1_v1", 1, 1 },
            { MTLFeatureSet_iOS_GPUFamily2_v1, "iOS_GPUFamily2_v1", 2, 1 },

            { MTLFeatureSet_iOS_GPUFamily1_v2, "iOS_GPUFamily1_v2", 1, 2 },
            { MTLFeatureSet_iOS_GPUFamily2_v2, "iOS_GPUFamily2_v2", 2, 2 },
            { MTLFeatureSet_iOS_GPUFamily3_v1, "iOS_GPUFamily3_v2", 3, 2 },

            { MTLFeatureSet_iOS_GPUFamily4_v1, "iOS_GPUFamily4_v1", 4, 1 },

    #ifdef __IPHONE_12_0
            { MTLFeatureSet_iOS_GPUFamily4_v2, "iOS_GPUFamily4_v2", 4, 2 },
            { MTLFeatureSet_iOS_GPUFamily5_v1, "iOS_GPUFamily5_v1", 5, 1 },
    #endif
#else
            { MTLFeatureSet_OSX_GPUFamily1_v1, "OSX_GPUFamily1_v1", 1, 1 },
#endif
        };

        for( size_t i=0; i<sizeof(featureSets) / sizeof(featureSets[0]); ++i )
        {
            if( [mActiveDevice->mDevice supportsFeatureSet:featureSets[i].featureSet] )
            {
                LogManager::getSingleton().logMessage( "Supports: " + String(featureSets[i].name) );
                driverVersion.major = featureSets[i].major;
                driverVersion.minor = featureSets[i].minor;
            }
        }

        rsc->setDriverVersion( driverVersion );

        uint32 threadgroupLimits[3] = { 1024u, 1024u, 64u };
        uint32 maxThreadsPerThreadgroup = 1024u;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        if( driverVersion.major < 4 )
        {
            threadgroupLimits[0] = 512u;
            threadgroupLimits[1] = 512u;
            maxThreadsPerThreadgroup = 512u;
        }
#endif
        rsc->setMaxThreadsPerThreadgroupAxis( threadgroupLimits );
        rsc->setMaxThreadsPerThreadgroup( maxThreadsPerThreadgroup );

        return rsc;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::reinit(void)
    {
        this->shutdown();
        this->init(true);
    }
    //-------------------------------------------------------------------------
    Window* MetalRenderSystem::init( bool autoCreateWindow, const String& windowTitle )
    {
        ConfigOptionMap::iterator opt = mOptions.find( "Rendering Device" );
        if( opt == mOptions.end() )
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Can`t find requested Metal device name!");
        mDeviceName = opt->second.currentValue;

        Window *autoWindow = 0;
        if( autoCreateWindow )
            autoWindow = createWindow( windowTitle, 1, 1, false );
        RenderSystem::init( autoCreateWindow, windowTitle );

        return autoWindow;
    }
    //-------------------------------------------------------------------------
    Window *MetalRenderSystem::createWindow( const String &name, uint32 width, uint32 height,
                                                    bool fullScreen,
                                                    const PropertyData *miscParams )
    {
        if( !mInitialized )
        {
            const MetalDeviceItem* deviceItem = getDeviceList(true)->item( mDeviceName );
            mDevice.init( deviceItem );
            setActiveDevice(&mDevice);
            String selectedDeviceName = deviceItem ? deviceItem->getDescription() :
                MetalDeviceItem(mDevice.mDevice, 0).getDescription() + " (system default)";
            LogManager::getSingleton().stream() << "Metal: Requested \"" << mDeviceName <<
                "\", selected \"" << selectedDeviceName << "\"";

            if( miscParams )
            {
                PropertyData::const_iterator itOption = miscParams->find( "reverse_depth" );
                if( itOption != miscParams->end() )
                    mReverseDepth = StringConverter::parseBool( itOption->second, true );
            }

            const long c_inFlightCommandBuffers = 3;
            mMainGpuSyncSemaphore = dispatch_semaphore_create(c_inFlightCommandBuffers);
            mMainSemaphoreAlreadyWaited = false;
            mBeginFrameOnceStarted = false;
            mVendorFeature = setupFeature();
            mVersion = mVendorFeature->getDriverVersion();

            if (mSysFeatureValid)
                mFeature = mVendorFeature;

            fireEvent("RenderSystemCapabilitiesCreated");

            initImpl( mFeature, 0 );

            mBufferManager = N_new MetalGBufferManager( c_inFlightCommandBuffers, &mDevice, miscParams );
            mHardwareBufferManager = new v1::MetalBufferManager( &mDevice, mBufferManager );
            mTextureManager = N_new MetalTextureGpuManager( mBufferManager, this, &mDevice );

            mInitialized = true;
        }

        Window *win = N_new MetalWindow( name, width, height, fullScreen, miscParams, &mDevice );
        mWindows.insert( win );

        win->_init(/* mTextureManager */);
        return win;
    }
    //-------------------------------------------------------------------------
    String MetalRenderSystem::getErrorString(long errorNumber) const
    {
        return N_StrBlank;
    }
    //-------------------------------------------------------------------------
    bool MetalRenderSystem::hasStoreAndMultisampleResolve(void) const
    {
        return mFeature->hasCapability( RSC_STORE_AND_MULTISAMPLE_RESOLVE );
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setLight(const LightList& lights, unsigned short limit)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setModel(const Matrix4 &m)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setView(const Matrix4 &m)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setProj(const Matrix4 &m)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setColour( const Colour &ambient,
                            const Colour &diffuse, const Colour &specular,
                            const Colour &emissive, Real shininess,
                            TrackVertexColourType tracking )
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setPoint(bool enabled)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setPoint(Real size, bool attenuationEnabled,
        Real constant, Real linear, Real quadratic, Real minSize, Real maxSize)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_bindTexture( size_t unit, Texture *texPtr )
    {
        if( texPtr )
        {
            const MetalTextureGpu *metalTex = static_cast<const MetalTextureGpu*>( texPtr );
            __unsafe_unretained id<MTLTexture> metalTexture = metalTex->getDisplayTextureName();

            [mActiveRenderEncoder setVertexTexture:metalTexture atIndex:unit];
            [mActiveRenderEncoder setFragmentTexture:metalTexture atIndex:unit];
        }
        else
        {
            [mActiveRenderEncoder setVertexTexture:0 atIndex:unit];
            [mActiveRenderEncoder setFragmentTexture:0 atIndex:unit];
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTexture( uint32 slotStart, const DescriptorSetTexture *set,
                                          uint32 hazardousTexIdx )
    {
        uint32 texUnit = slotStart;
        vectorL<const Texture*>::const_iterator itor = set->mTextures.begin();

        //for( size_t i=0u; i<ST_PCnt; ++i )
        for( size_t i=0u; i<ST_FS + 1u; ++i )
        {
            const size_t numTexturesUsed = set->mShaderTypeTexCount[i];
            for( size_t j=0u; j<numTexturesUsed; ++j )
            {
                const MetalTextureGpu *metalTex = static_cast<const MetalTextureGpu*>( *itor );
                __unsafe_unretained id<MTLTexture> metalTexture = 0;

                if( metalTex )
                {
                    metalTexture = metalTex->getDisplayTextureName();

                    if( (texUnit - slotStart) == hazardousTexIdx &&
                        mActiveFrame2->isTarget( set->mTextures[hazardousTexIdx] ) )
                    {
                        metalTexture = nil;
                    }
                }

                switch( i )
                {
                case ST_VS:
                    [mActiveRenderEncoder setVertexTexture:metalTexture atIndex:texUnit];
                    break;
                case ST_FS:
                    [mActiveRenderEncoder setFragmentTexture:metalTexture atIndex:texUnit];
                    break;
                case ST_GS:
                case ST_TS:
                case ST_DS:
                    break;
                }

                ++texUnit;
                ++itor;
            }
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTexture( uint32 slotStart, const ShaderChTexture *set )
    {
        MetalDescriptorSetTexture *metalSet = reinterpret_cast<MetalDescriptorSetTexture*>( set->mRsData );

        //Bind textures
        {
            vectorL<MetalTexRegion>::const_iterator itor = metalSet->textures.begin();
            vectorL<MetalTexRegion>::const_iterator end  = metalSet->textures.end();

            while( itor != end )
            {
                NSRange range = itor->range;
                range.location += slotStart;

                switch( itor->shaderType )
                {
                case ST_VS:
                    [mActiveRenderEncoder setVertexTextures:itor->textures withRange:range];
                    break;
                case ST_FS:
                    [mActiveRenderEncoder setFragmentTextures:itor->textures withRange:range];
                    break;
                case ST_GS:
                case ST_TS:
                case ST_DS:
                case ST_PCnt:
                    break;
                }

                ++itor;
            }
        }

        //Bind buffers
        {
            vectorL<MetalBufferRegion>::const_iterator itor = metalSet->buffers.begin();
            vectorL<MetalBufferRegion>::const_iterator end  = metalSet->buffers.end();

            while( itor != end )
            {
                NSRange range = itor->range;
                range.location += slotStart + OGRE_METAL_TEX_SLOT_START;

                switch( itor->shaderType )
                {
                case ST_VS:
                    [mActiveRenderEncoder setVertexBuffers:itor->buffers
                                                   offsets:itor->offsets
                                                 withRange:range];
                    break;
                case ST_FS:
                    [mActiveRenderEncoder setFragmentBuffers:itor->buffers
                                                     offsets:itor->offsets
                                                   withRange:range];
                    break;
                case ST_GS:
                case ST_TS:
                case ST_DS:
                case ST_PCnt:
                    break;
                }

                ++itor;
            }
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setSampler( uint32 slotStart, const DescriptorSetSampler *set )
    {
        __unsafe_unretained id <MTLSamplerState> samplers[16];

        vectorL<const TextureSampleUnit*>::const_iterator itor = set->mSamplers.begin();

        NSRange texUnitRange;
        texUnitRange.location = slotStart;
        //for( size_t i=0u; i<ST_PCnt; ++i )
        for( size_t i=0u; i<ST_FS + 1u; ++i )
        {
            const NSUInteger numSamplersUsed = set->mShaderTypeSamplerCount[i];

            if( !numSamplersUsed )
                continue;

            for( size_t j=0; j<numSamplersUsed; ++j )
            {
                if( *itor )
                    samplers[j] = (__bridge id<MTLSamplerState>)(*itor)->mRsData;
                else
                    samplers[j] = 0;
                ++itor;
            }

            texUnitRange.length = numSamplersUsed;

            switch( i )
            {
            case ST_VS:
                [mActiveRenderEncoder setVertexSamplerStates:samplers withRange:texUnitRange];
                break;
            case ST_FS:
                [mActiveRenderEncoder setFragmentSamplerStates:samplers withRange:texUnitRange];
                break;
            case ST_GS:
            case ST_TS:
            case ST_DS:
                break;
            }

            texUnitRange.location += numSamplersUsed;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTextureCS( uint32 slotStart, const DescriptorSetTexture *set )
    {
        vectorL<const Texture*>::const_iterator itor = set->mTextures.begin();

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mActiveDevice->getComputeEncoder();

        size_t texIdx = 0;

        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const size_t numTexturesUsed = set->mShaderTypeTexCount[i];
            for( size_t j=0u; j<numTexturesUsed; ++j )
            {
                const MetalTextureGpu *metalTex = static_cast<const MetalTextureGpu*>( *itor );
                __unsafe_unretained id<MTLTexture> metalTexture = 0;

                if( metalTex )
                    metalTexture = metalTex->getDisplayTextureName();

                [computeEncoder setTexture:metalTexture atIndex:slotStart + texIdx];
                texIdx += numTexturesUsed;

                ++texIdx;
                ++itor;
            }
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTextureCS( uint32 slotStart, const ShaderChTexture *set )
    {
        MetalDescriptorSetTexture * metalSet = reinterpret_cast<MetalDescriptorSetTexture*>( set->mRsData );

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mActiveDevice->getComputeEncoder();

        //Bind textures
        {
            vectorL<MetalTexRegion>::const_iterator itor = metalSet->textures.begin();
            vectorL<MetalTexRegion>::const_iterator end  = metalSet->textures.end();

            while( itor != end )
            {
                NSRange range = itor->range;
                range.location += slotStart;
                [computeEncoder setTextures:itor->textures withRange:range];
                ++itor;
            }
        }

        //Bind buffers
        {
            vectorL<MetalBufferRegion>::const_iterator itor = metalSet->buffers.begin();
            vectorL<MetalBufferRegion>::const_iterator end  = metalSet->buffers.end();

            while( itor != end )
            {
                NSRange range = itor->range;
                range.location += slotStart + OGRE_METAL_CS_TEX_SLOT_START;
                [computeEncoder setBuffers:itor->buffers offsets:itor->offsets withRange:range];
                ++itor;
            }
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setSamplerCS( uint32 slotStart, const DescriptorSetSampler *set )
    {
        __unsafe_unretained id <MTLSamplerState> samplers[16];

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mActiveDevice->getComputeEncoder();

        vectorL<const TextureSampleUnit*>::const_iterator itor = set->mSamplers.begin();

        NSRange texUnitRange;
        texUnitRange.location = slotStart;
        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const NSUInteger numSamplersUsed = set->mShaderTypeSamplerCount[i];

            if( !numSamplersUsed )
                continue;

            for( size_t j=0; j<numSamplersUsed; ++j )
            {
                if( *itor )
                    samplers[j] = (__bridge id<MTLSamplerState>)(*itor)->mRsData;
                else
                    samplers[j] = 0;
                ++itor;
            }

            texUnitRange.length = numSamplersUsed;

            [computeEncoder setSamplerStates:samplers withRange:texUnitRange];
            texUnitRange.location += numSamplersUsed;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setBufferCS( uint32 slotStart, const GpuParamBuffer *set )
    {
        MetalDescriptorSetTexture *metalSet = reinterpret_cast<MetalDescriptorSetTexture*>( set->mRsData );

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mActiveDevice->getComputeEncoder();

        //Bind textures
        {
            vectorL<MetalTexRegion>::const_iterator itor = metalSet->textures.begin();
            vectorL<MetalTexRegion>::const_iterator end  = metalSet->textures.end();

            while( itor != end )
            {
                NSRange range = itor->range;
                range.location += slotStart + OGRE_METAL_CS_UAV_SLOT_START;
                [computeEncoder setTextures:itor->textures withRange:range];
                ++itor;
            }
        }

        //Bind buffers
        {
            vectorL<MetalBufferRegion>::const_iterator itor = metalSet->buffers.begin();
            vectorL<MetalBufferRegion>::const_iterator end  = metalSet->buffers.end();

            while( itor != end )
            {
                NSRange range = itor->range;
                range.location += slotStart + OGRE_METAL_CS_UAV_SLOT_START;
                [computeEncoder setBuffers:itor->buffers offsets:itor->offsets withRange:range];
                ++itor;
            }
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setContext( Texture *texture )
    {
        //mDevice
    }
    //-------------------------------------------------------------------------
    FrameBufferObject* MetalRenderSystem::createFBO(void)
    {
        FrameBufferObject *retVal = N_new MetalRenderPassDescriptor( mActiveDevice, this );
        mFBOList.insert( retVal );
        return retVal;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::beginFBO(FrameBufferObject * desc, const ViewportList & vlist)
    {
        bool warnIfRtvWasFlushed = false;
        if( desc->mStepping && desc->isTargetEqual( mActiveFrame2 ) )
            return;

        const int oldWidth = mActiveViewport[0].getPixelWidth();
        const int oldHeight = mActiveViewport[0].getPixelHeight();
        const int oldX = mActiveViewport[0].getPixelLeft();
        const int oldY = mActiveViewport[0].getPixelTop();

        MetalRenderPassDescriptor * currPassDesc = static_cast<MetalRenderPassDescriptor*>( mActiveFrame2 );

        RenderSystem::beginFBO(desc, vlist);


        // Calculate the new "lower-left" corner of the viewport to compare with the old one
        const int w = mActiveViewport[0].getPixelWidth();
        const int h = mActiveViewport[0].getPixelHeight();
        const int x = mActiveViewport[0].getPixelLeft();
        const int y = mActiveViewport[0].getPixelTop();

        const bool vpChanged = oldX != x || oldY != y || oldWidth != w || oldHeight != h ||
                               vlist.size() > 1u;

        MetalRenderPassDescriptor *newPassDesc =
                static_cast<MetalRenderPassDescriptor*>( desc );

        //Determine whether:
        //  1. We need to store current active FrameBufferObject
        //  2. We need to perform clears when loading the new FrameBufferObject
        uint32 entriesToFlush = 0;
        if( currPassDesc )
        {
            entriesToFlush = currPassDesc->willSwitchTo( newPassDesc, warnIfRtvWasFlushed );

            if( entriesToFlush != 0 )
                currPassDesc->performStoreActions( entriesToFlush, false );

            //If rendering was interrupted but we're still rendering to the same
            //RTT, willSwitchTo will have returned 0 and thus we won't perform
            //the necessary load actions.
            //If RTTs were different, we need to have performStoreActions
            //called by now (i.e. to emulate StoreAndResolve)
            if( mInterruptedRenderCommandEncoder )
            {
                entriesToFlush = FrameBufferObject::TT_All;
                if( warnIfRtvWasFlushed )
                    newPassDesc->checkWarnIfRtvWasFlushed( entriesToFlush );
            }
        }
        else
        {
            entriesToFlush = FrameBufferObject::TT_All;
        }

        mEntriesToFlush = entriesToFlush;
        mVpChanged      = vpChanged;
        mInterruptedRenderCommandEncoder = false;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::executeFBO( bool officialCall )
    {
        if( officialCall )
            mInterruptedRenderCommandEncoder = false;

        if( mEntriesToFlush )
        {
            mActiveDevice->endAllEncoders( false );
            mActiveRenderEncoder = 0;

            MetalRenderPassDescriptor * newPassDesc =
                    static_cast<MetalRenderPassDescriptor *>( mActiveFrame2 );

            MTLRenderPassDescriptor * passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
            newPassDesc->performLoadActions( passDesc, mInterruptedRenderCommandEncoder );

            mActiveDevice->mRenderEncoder =
                    [mActiveDevice->mCurrentCommandBuffer renderCommandEncoderWithDescriptor:passDesc];
            mActiveRenderEncoder = mActiveDevice->mRenderEncoder;

            static_cast<MetalGBufferManager*>( mBufferManager )->bindDrawId();
            [mActiveRenderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];

            if (mStencilEnabled)
                [mActiveRenderEncoder setStencilReferenceValue:mStencilRefValue];

            mInterruptedRenderCommandEncoder = false;
        }

        if( !mGpuBufValid && mActiveRenderEncoder )
        {
            if(mGpuParamBuffer)
            {
                MetalDescriptorSetTexture * metalSet = reinterpret_cast<MetalDescriptorSetTexture *>(mGpuParamBuffer->mRsData);

                //Bind textures
                {
                    vectorL<MetalTexRegion>::const_iterator itor = metalSet->textures.begin();
                    vectorL<MetalTexRegion>::const_iterator end  = metalSet->textures.end();

                    while( itor != end )
                    {
                        NSRange range = itor->range;
                        range.location += /*mUavStartingSlot +*/ OGRE_METAL_UAV_SLOT_START;
                        [mActiveRenderEncoder setVertexTextures:itor->textures withRange:range];
                        [mActiveRenderEncoder setFragmentTextures:itor->textures withRange:range];
                        ++itor;
                    }
                }

                //Bind buffers
                {
                    vectorL<MetalBufferRegion>::const_iterator itor = metalSet->buffers.begin();
                    vectorL<MetalBufferRegion>::const_iterator end  = metalSet->buffers.end();

                    while( itor != end )
                    {
                        NSRange range = itor->range;
                        range.location += /*mUavStartingSlot +*/ OGRE_METAL_UAV_SLOT_START;

                        [mActiveRenderEncoder setVertexBuffers:itor->buffers
                                                       offsets:itor->offsets
                                                     withRange:range];
                        [mActiveRenderEncoder setFragmentBuffers:itor->buffers
                                                         offsets:itor->offsets
                                                       withRange:range];
                        ++itor;
                    }
                }
            }
            mGpuBufValid = true;
        }

        const uint32 numViewports = mViewportCount;

        //If we flushed, viewport and scissor settings got reset.
        if( !mActiveViewport[0].isFullBufer() || (mVpChanged && !mEntriesToFlush) ||
            numViewports > 1u  )
        {
#if defined( __IPHONE_12_0 ) || \
    (defined( MAC_OS_X_VERSION_10_13 ) && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
            bool hasMultiViewports = false;
            if( @available( iOS 12.0, macOS 10.13, tvOS 12.0, * ) )
                hasMultiViewports = true;
            if( numViewports <= 1u || !hasMultiViewports )
#endif
            {
                MTLViewport mtlVp;
                mtlVp.originX   = mActiveViewport[0].getPixelLeft();
                mtlVp.originY   = mActiveViewport[0].getPixelTop();
                mtlVp.width     = mActiveViewport[0].getPixelWidth();
                mtlVp.height    = mActiveViewport[0].getPixelHeight();
                mtlVp.znear     = 0;
                mtlVp.zfar      = 1;
                [mActiveRenderEncoder setViewport:mtlVp];
            }
#if defined( __IPHONE_12_0 ) || \
    (defined( MAC_OS_X_VERSION_10_13 ) && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
            else
            {
                MTLViewport mtlVp[16];
                for( size_t i=0; i<numViewports; ++i )
                {
                    mtlVp[i].originX    = mActiveViewport[i].getPixelLeft();
                    mtlVp[i].originY    = mActiveViewport[i].getPixelTop();
                    mtlVp[i].width      = mActiveViewport[i].getPixelWidth();
                    mtlVp[i].height     = mActiveViewport[i].getPixelHeight();
                    mtlVp[i].znear      = 0;
                    mtlVp[i].zfar       = 1;
                }
                [mActiveRenderEncoder setViewports:mtlVp count:numViewports];
            }
#endif
        }

        if( (!mActiveViewport[0].isFullBufer() ||
             !mActiveViewport[0].isNonClip()) || !mEntriesToFlush ||
            numViewports > 1u )
        {
#if defined( __IPHONE_12_0 ) || \
    (defined( MAC_OS_X_VERSION_10_13 ) && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
            bool hasMultiViewports = false;
            if( @available( iOS 12.0, macOS 10.13, tvOS 12.0, * ) )
                hasMultiViewports = true;
            if( numViewports <= 1u || !hasMultiViewports )
#endif
            {
                MTLScissorRect scissorRect;
                scissorRect.x       = mActiveViewport[0].getClipPixelLeft();
                scissorRect.y       = mActiveViewport[0].getClipPixelTop();
                scissorRect.width   = mActiveViewport[0].getClipPixelWidth();
                scissorRect.height  = mActiveViewport[0].getClipPixelHeight();
                [mActiveRenderEncoder setScissorRect:scissorRect];
            }
#if defined( __IPHONE_12_0 ) || \
    (defined( MAC_OS_X_VERSION_10_13 ) && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
            else
            {
                MTLScissorRect scissorRect[16];
                for( size_t i=0; i<numViewports; ++i )
                {
                    scissorRect[i].x        = mActiveViewport[i].getClipPixelLeft();
                    scissorRect[i].y        = mActiveViewport[i].getClipPixelTop();
                    scissorRect[i].width    = mActiveViewport[i].getClipPixelWidth();
                    scissorRect[i].height   = mActiveViewport[i].getClipPixelHeight();
                }
                [mActiveRenderEncoder setScissorRects:scissorRect count:numViewports];
            }
#endif
        }

        mEntriesToFlush = 0;
        mVpChanged = false;
        mInterruptedRenderCommandEncoder = false;

        if( mActiveDevice->mFrameAborted )
        {
            mActiveDevice->endAllEncoders();
            mActiveRenderEncoder = 0;
            return;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::executeFBO(void)
    {
        executeFBO( true );
    }
    //-------------------------------------------------------------------------
    inline void MetalRenderSystem::endFBO( bool isInterruptingRender )
    {
        if( mActiveFrame2 )
        {
            MetalRenderPassDescriptor *passDesc = static_cast<MetalRenderPassDescriptor*>( mActiveFrame2 );
            passDesc->performStoreActions( FrameBufferObject::TT_All, isInterruptingRender );

            mEntriesToFlush = 0;
            mVpChanged = false;

            mInterruptedRenderCommandEncoder = isInterruptingRender;

            if( !isInterruptingRender )
                RenderSystem::endFBO();
            else
                mEntriesToFlush = FrameBufferObject::TT_All;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::endFBO(void)
    {
        endFBO( false );
    }
    //-----------------------------------------------------------------------------------
    Texture* MetalRenderSystem::createDepthImpl( Texture *colourTexture,
                                                         bool preferDepthTexture,
                                                         PixelFormat depthBufferFormat )
    {
        if( depthBufferFormat == PF_UNKNOWN )
            depthBufferFormat = Texture::DepthFormat;

        return RenderSystem::createDepthImpl( colourTexture, preferDepthTexture,
                                                   depthBufferFormat );
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTextureCoordPrc( size_t unit, TextureCoordPrc m,
                                                        const Frustum* frustum )
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTextureBlend(size_t unit, const TextureBlend& bm)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTextureMatrix(size_t unit, const Matrix4& xform)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setIndirect( IndirectBuffer *indirectBuffer )
    {
        if( mFeature->isSupport(CF_IndirectBuffer) )
        {
            if( indirectBuffer )
            {
                MetalBufferOp *bufferInterface = static_cast<MetalBufferOp*>(
                                                            indirectBuffer->getBufferInterface() );
                mIndirectBuffer = bufferInterface->getVboName();
            }
            else
            {
                mIndirectBuffer = 0;
            }
        }
        else
        {
            if( indirectBuffer )
                mIndirectPtr = indirectBuffer->getHostData();
            else
                mIndirectPtr = 0;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_waitForTailFrameToFinish(void)
    {
        if( !mMainSemaphoreAlreadyWaited )
        {
            dispatch_semaphore_wait( mMainGpuSyncSemaphore, DISPATCH_TIME_FOREVER );
            //Semaphore was just grabbed, so ensure we don't grab it twice.
            mMainSemaphoreAlreadyWaited = true;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_beginFrameOnce(void)
    {
        N_assert( !mBeginFrameOnceStarted &&
                "Calling MetalRenderSystem::_beginFrameOnce more than once "
                "without matching call to _endFrameOnce!!!" );

        //Allow the renderer to preflight 3 frames on the CPU (using a semapore as a guard) and
        //commit them to the GPU. This semaphore will get signaled once the GPU completes a
        //frame's work via addCompletedHandler callback below, signifying the CPU can go ahead
        //and prepare another frame.
        _waitForTailFrameToFinish();

        mBeginFrameOnceStarted = true;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_endFrameOnce(void)
    {
        RenderSystem::_endFrameOnce();

        //TODO: We shouldn't tidy up JUST the active device. But all of them.

        cleanAutoParamsBuffers();

        __block dispatch_semaphore_t blockSemaphore = mMainGpuSyncSemaphore;
        [mActiveDevice->mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
        {
            // GPU has completed rendering the frame and is done using the contents of any buffers
            // previously encoded on the CPU for that frame. Signal the semaphore and allow the CPU
            // to proceed and construct the next frame.
            dispatch_semaphore_signal( blockSemaphore );
        }];

        mActiveDevice->commitAndNextCommandBuffer();

        mActiveDevice->mFrameAborted = false;
        mMainSemaphoreAlreadyWaited = false;
        mBeginFrameOnceStarted = false;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::cleanAutoParamsBuffers(void)
    {
        const size_t numUsedBuffers = mAutoParamsBufferIdx;
        size_t usedBytes = 0;
        for( size_t i=0; i<numUsedBuffers; ++i )
        {
            mAutoParamsBuffer[i]->unmap( (i == 0u && numUsedBuffers == 1u) ? UT_PersistentMap :
                                                                             UT_UnlockMap );
            usedBytes += mAutoParamsBuffer[i]->getTotalSizeBytes();
        }

        //Get the max amount of bytes consumed in the last N frames.
        const int numHistoricSamples = sizeof(mHistoricalAutoParamsSize) /
                                       sizeof(mHistoricalAutoParamsSize[0]);
        mHistoricalAutoParamsSize[numHistoricSamples-1] = usedBytes;
        for( int i=0; i<numHistoricSamples - 1; ++i )
        {
            usedBytes = std::max( usedBytes, mHistoricalAutoParamsSize[i + 1] );
            mHistoricalAutoParamsSize[i] = mHistoricalAutoParamsSize[i + 1];
        }

        if( //Merge all buffers into one for the next frame.
            numUsedBuffers > 1u ||
            //Historic data shows we've been using less memory. Shrink this record.
            (!mAutoParamsBuffer.empty() && mAutoParamsBuffer[0]->getTotalSizeBytes() > usedBytes) )
        {
            if( mAutoParamsBuffer[0]->getMappingState() != 0 )
                mAutoParamsBuffer[0]->unmap( UT_UnlockMap );

            for( size_t i=0; i<mAutoParamsBuffer.size(); ++i )
                mBufferManager->destroyConstBuffer( mAutoParamsBuffer[i] );
            mAutoParamsBuffer.clear();

            if( usedBytes > 0 )
            {
                StructBuffer *constBuffer = mBufferManager->createConstBuffer( usedBytes,
                                                                                 M_Normal | M_PersistentMap,
                                                                                 0, false );
                mAutoParamsBuffer.push_back( constBuffer );
            }
        }

        mCurrentAutoParamsBufferPtr = 0;
        mCurrentAutoParamsBufferSpaceLeft = 0;
        mAutoParamsBufferIdx = 0;

    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::createCSO(ComputeStateObject * newPso)
    {
        MetalProgram * mCS = static_cast<MetalProgram *>(newPso->mCS->getMain());

        //Btw. ComputePattern guarantees mGroupCount won't have zeroes.
        N_assert( newPso->mGroupCount[0] != 0 &&
                newPso->mGroupCount[1] != 0 &&
                newPso->mGroupCount[2] != 0 &&
                "Invalid parameters. Will also cause div. by zero!" );

        NSError* error = 0;
        MTLComputePipelineDescriptor *psd = [[MTLComputePipelineDescriptor alloc] init];
        psd.computeFunction = mCS->getMetalFunction();
        psd.threadGroupSizeIsMultipleOfThreadExecutionWidth =
                (newPso->mCeilCount[0] % newPso->mGroupCount[0] == 0) &&
                (newPso->mCeilCount[1] % newPso->mGroupCount[1] == 0) &&
                (newPso->mCeilCount[2] % newPso->mGroupCount[2] == 0);

        id<MTLComputePipelineState> pso =
                [mActiveDevice->mDevice newComputePipelineStateWithDescriptor:psd
                                                                      options:MTLPipelineOptionNone
                                                                   reflection:nil
                                                                        error:&error];
        if( !pso || error )
        {
            String errorDesc;
            if( error )
                errorDesc = [error localizedDescription].UTF8String;

            N_EXCEPT( RenderingAPI, "Failed to create pipeline state for compute, error " + errorDesc );
        }

        newPso->mExtData = const_cast<void*>( CFBridgingRetain( pso ) );
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::destroyCSO( ComputeStateObject *pso )
    {
        id<MTLComputePipelineState> metalPso = reinterpret_cast< id<MTLComputePipelineState> >(
                    CFBridgingRelease( pso->mExtData ) );
        pso->mExtData = 0;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_beginFrame(void)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_endFrame(void)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setActiveDevice( MetalDevice *device )
    {
        if( mActiveDevice != device )
        {
            mActiveDevice = device;
            mActiveRenderEncoder = device ? device->mRenderEncoder : 0;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_notifyActiveEncoderEnded( bool callEndRenderPassDesc )
    {
        // MetalDevice::endRenderEncoder gets called either because
        //  * Another encoder was required. Thus we interrupted and callEndRenderPassDesc = true
        //  * endFBO called us. Thus callEndRenderPassDesc = false
        //  * executeFBO called us. Thus callEndRenderPassDesc = false
        // In all cases, when callEndRenderPassDesc = true, it also implies rendering was interrupted.
        if( callEndRenderPassDesc )
            endFBO( true );

        mGpuBufValid = false;
        mActiveRenderEncoder = 0;
        mPso = 0;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_notifyActiveComputeEnded(void)
    {
        mComputePso = 0;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_notifyNewCommandBuffer(void)
    {
        MetalGBufferManager * vaoManager = static_cast<MetalGBufferManager *>(mBufferManager);
        vaoManager->_notifyNewCommandBuffer();
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_notifyDeviceStalled(void)
    {
        v1::MetalBufferManager * hwBufferMgr = static_cast<v1::MetalBufferManager *>(mHardwareBufferManager);
        MetalGBufferManager * vaoManager = static_cast<MetalGBufferManager *>(mBufferManager);

        hwBufferMgr->_notifyDeviceStalled();
        vaoManager->_notifyDeviceStalled();
    }
    //-------------------------------------------------------------------------
    id <MTLDepthStencilState> MetalRenderSystem::getDepthStencilState( RenderStateObject *pso )
    {
        CachedDepthStencilState depthState;
        if( pso->mStencil->mDepthCheck )
        {
            depthState.depthFunc    = pso->mStencil->mDepthFunc;
            if( mReverseDepth )
                depthState.depthFunc = reverse( depthState.depthFunc );
            depthState.depthWrite   = pso->mStencil->mDepthWrite;
        }
        else
        {
            depthState.depthFunc    = CMPF_ALWAYS_PASS;
            depthState.depthWrite   = false;
        }

        depthState.mStencil = pso->mSampleObject.mStencil;

        CachedDepthStencilStateVec::iterator itor = std::lower_bound( mDepthStencilStates.begin(),
                                                                      mDepthStencilStates.end(),
                                                                      depthState );

        if( itor == mDepthStencilStates.end() || depthState != *itor )
        {
            //Not cached. Add the entry
            MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
            depthStateDesc.depthCompareFunction = MetalMappings::get( depthState.depthFunc );
            depthStateDesc.depthWriteEnabled    = depthState.depthWrite;

            if( pso->mSampleObject.mStencil.enabled )
            {
                if( pso->mSampleObject.mStencil.stencilFront != StencilStateOp() )
                {
                    const StencilStateOp &stencilOp = pso->mSampleObject.mStencil.stencilFront;

                    MTLStencilDescriptor *stencilDesc = [MTLStencilDescriptor alloc];
                    stencilDesc.stencilCompareFunction = MetalMappings::get( stencilOp.compareOp );
                    stencilDesc.stencilFailureOperation = MetalMappings::get( stencilOp.stencilFailOp );
                    stencilDesc.depthFailureOperation =
                            MetalMappings::get( stencilOp.stencilDepthFailOp );
                    stencilDesc.depthStencilPassOperation =
                            MetalMappings::get( stencilOp.stencilPassOp );

                    stencilDesc.readMask = pso->mSampleObject.mStencil.readMask;
                    stencilDesc.writeMask = pso->mSampleObject.mStencil.writeMask;

                    depthStateDesc.frontFaceStencil = stencilDesc;
                }

                if( pso->mSampleObject.mStencil.stencilBack != StencilStateOp() )
                {
                    const StencilStateOp &stencilOp = pso->mSampleObject.mStencil.stencilBack;

                    MTLStencilDescriptor *stencilDesc = [MTLStencilDescriptor alloc];
                    stencilDesc.stencilCompareFunction = MetalMappings::get( stencilOp.compareOp );
                    stencilDesc.stencilFailureOperation = MetalMappings::get( stencilOp.stencilFailOp );
                    stencilDesc.depthFailureOperation =
                            MetalMappings::get( stencilOp.stencilDepthFailOp );
                    stencilDesc.depthStencilPassOperation =
                            MetalMappings::get( stencilOp.stencilPassOp );

                    stencilDesc.readMask = pso->mSampleObject.mStencil.readMask;
                    stencilDesc.writeMask = pso->mSampleObject.mStencil.writeMask;

                    depthStateDesc.backFaceStencil = stencilDesc;
                }
            }

            depthState.depthStencilState =
                    [mActiveDevice->mDevice newDepthStencilStateWithDescriptor:depthStateDesc];

            itor = mDepthStencilStates.insert( itor, depthState );
        }

        ++itor->refCount;
        return itor->depthStencilState;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::removeDepthStencilState( RenderStateObject *pso )
    {
        CachedDepthStencilState depthState;
        if( pso->mStencil->mDepthCheck )
        {
            depthState.depthFunc    = pso->mStencil->mDepthFunc;
            if( mReverseDepth )
                depthState.depthFunc = reverse( depthState.depthFunc );
            depthState.depthWrite   = pso->mStencil->mDepthWrite;
        }
        else
        {
            depthState.depthFunc    = CMPF_ALWAYS_PASS;
            depthState.depthWrite   = false;
        }

        depthState.mStencil = pso->mSampleObject.mStencil;

        CachedDepthStencilStateVec::iterator itor = std::lower_bound( mDepthStencilStates.begin(),
                                                                      mDepthStencilStates.end(),
                                                                      depthState );

        if( itor == mDepthStencilStates.end() || !(depthState != *itor) )
        {
            --itor->refCount;
            if( !itor->refCount )
            {
                mDepthStencilStates.erase( itor );
            }
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::createRSO( RenderStateObject *newPso )
    {
        MTLRenderPipelineDescriptor *psd = [[MTLRenderPipelineDescriptor alloc] init];
        [psd setSampleCount: newPso->pass.mSampleType.getMultiSampling()]; // aka .rasterSampleCount

        MetalProgram *mVS = 0;
        MetalProgram *mFS = 0;

        if( !newPso->mVS.isNull() )
        {
            mVS = static_cast<MetalProgram*>( newPso->mVS->getMain() );
            [psd setVertexFunction:mVS->getMetalFunction()];
        }

        if( !newPso->mFS.isNull() )
        {
            mFS = static_cast<MetalProgram*>( newPso->mFS->getMain() );
            [psd setFragmentFunction:mFS->getMetalFunction()];
        }
        //Ducttape shaders
//        id<MTLLibrary> library = [mActiveDevice->mDevice newDefaultLibrary];
//        [psd setVertexFunction:[library newFunctionWithName:@"vertex_vs"]];
//        [psd setFragmentFunction:[library newFunctionWithName:@"pixel_ps"]];

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        // On iOS we can skip the Vertex Desc.
        // On OS X we always need to set it for the baseInstance / draw id
        if( !newPso->mUnitsList.empty() )
#endif
        {
            size_t numUVs = 0;
            MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];

            VertexUnitListList::const_iterator itor = newPso->mUnitsList.begin();
            VertexUnitListList::const_iterator end  = newPso->mUnitsList.end();

            while( itor != end )
            {
                size_t accumOffset = 0;
                const size_t bufferIdx = itor - newPso->mUnitsList.begin();
                VertexUnitList::const_iterator it = itor->begin();
                VertexUnitList::const_iterator en = itor->end();

                while( it != en )
                {
                    size_t elementIdx = MetalGBufferManager::getAttributeIndexFor( it->mVType );
                    if( it->mVType == VT_Tex_Coord )
                    {
                        elementIdx += numUVs;
                        ++numUVs;
                    }
                    vertexDescriptor.attributes[elementIdx].format = MetalMappings::get( it->mType );
                    vertexDescriptor.attributes[elementIdx].bufferIndex = bufferIdx;
                    vertexDescriptor.attributes[elementIdx].offset = accumOffset;

                    accumOffset += v1::VertexElement::getTypeSize( it->mType );
                    ++it;
                }

                vertexDescriptor.layouts[bufferIdx].stride = accumOffset;
                vertexDescriptor.layouts[bufferIdx].stepFunction = MTLVertexStepFunctionPerVertex;

                ++itor;
            }

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
            vertexDescriptor.attributes[15].format      = MTLVertexFormatUInt;
            vertexDescriptor.attributes[15].bufferIndex = 15;
            vertexDescriptor.attributes[15].offset      = 0;

            vertexDescriptor.layouts[15].stride = 4;
            vertexDescriptor.layouts[15].stepFunction = MTLVertexStepFunctionPerInstance;
#endif
            [psd setVertexDescriptor:vertexDescriptor];
        }

        uint8 mrtCount = 0;
        for( int i=0; i<NII_MAX_RenderTarget; ++i )
        {
            if( newPso->pass.colourFormat[i] != PFG_NULL )
                mrtCount = i + 1u;
        }

        for( int i=0; i<mrtCount; ++i )
        {
            ShaderChBlend const *blend = newPso->mBlend;
            psd.colorAttachments[i].pf = MetalMappings::get( newPso->pass.colourFormat[i] );

            if( psd.colorAttachments[i].pf == MTLPixelFormatInvalid ||
                (blend->mBlendOperation == SBO_ADD &&
                 blend->mSourceBlendFactor == SBF_ONE &&
                 blend->mDestBlendFactor == SBF_ZERO &&
                (!blend->mSeparateBlend ||
                 (blend->mBlendOperation == blend->mBlendOperationAlpha &&
                 blend->mSourceBlendFactor == blend->mSourceBlendFactorAlpha &&
                 blend->mDestBlendFactor == blend->mDestBlendFactorAlpha))) )
            {
                //Note: Can NOT use blend->mIsTransparent. What Ogre understand
                //as transparent differs from what Metal understands.
                psd.colorAttachments[i].blendingEnabled = NO;
            }
            else
            {
                psd.colorAttachments[i].blendingEnabled = YES;
            }
            psd.colorAttachments[i].rgbBlendOperation           = MetalMappings::get( blend->mBlendOperation );
            psd.colorAttachments[i].alphaBlendOperation         = MetalMappings::get( blend->mBlendOperationAlpha );
            psd.colorAttachments[i].sourceRGBBlendFactor        = MetalMappings::get( blend->mSourceBlendFactor );
            psd.colorAttachments[i].destinationRGBBlendFactor   = MetalMappings::get( blend->mDestBlendFactor );
            psd.colorAttachments[i].sourceAlphaBlendFactor      = MetalMappings::get( blend->mSourceBlendFactorAlpha );
            psd.colorAttachments[i].destinationAlphaBlendFactor = MetalMappings::get( blend->mDestBlendFactorAlpha );

            psd.colorAttachments[i].writeMask = MetalMappings::get( blend->mBlendChannelMask );
        }

        if( newPso->pass.depthFormat != PFG_NULL )
        {
            MTLPixelFormat depthFormat = MTLPixelFormatInvalid;
            MTLPixelFormat stencilFormat = MTLPixelFormatInvalid;
            MetalMappings::getDepthStencilFormat( mActiveDevice, newPso->pass.depthFormat,
                                                  depthFormat, stencilFormat );
            psd.depthAttachmentPixelFormat = depthFormat;
            psd.stencilAttachmentPixelFormat = stencilFormat;
        }

        NSError* error = NULL;
        id <MTLRenderPipelineState> pso =
                [mActiveDevice->mDevice newRenderPipelineStateWithDescriptor:psd error:&error];

        if( !pso || error )
        {
            String errorDesc;
            if( error )
                errorDesc = [error localizedDescription].UTF8String;

            N_EXCEPT( RenderingAPI, "Failed to created pipeline state for rendering, error " + errorDesc );
        }

        id <MTLDepthStencilState> depthStencilState = getDepthStencilState( newPso );

        MetalHlmsPso *metalPso = new MetalHlmsPso();
        metalPso->pso = pso;
        metalPso->depthStencilState = depthStencilState;
        metalPso->mVS      = mVS;
        metalPso->mFS       = mFS;

        switch( newPso->mStencil->mCullMode )
        {
        case CM_None:             metalPso->cullMode = MTLCullModeNone; break;
        case CM_Clockwise:        metalPso->cullMode = MTLCullModeBack; break;
        case CM_Anticlockwise:    metalPso->cullMode = MTLCullModeFront; break;
        }

        newPso->mExtData = metalPso;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::destroyRSO( RenderStateObject *pso )
    {
        N_assert( pso->mExtData );

        removeDepthStencilState( pso );

        MetalHlmsPso *metalPso = reinterpret_cast<MetalHlmsPso*>(pso->mExtData);
        delete metalPso;
        pso->mExtData = 0;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::createTexSample( TextureSampleUnit *newBlock )
    {
        MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
        samplerDescriptor.minFilter = MetalMappings::get( newBlock->mMinFilter );
        samplerDescriptor.magFilter = MetalMappings::get( newBlock->mMagFilter );
        samplerDescriptor.mipFilter = MetalMappings::getMipFilter( newBlock->mMipFilter );
        samplerDescriptor.maxAnisotropy = newBlock->mMaxAnisotropy;
        samplerDescriptor.sAddressMode  = MetalMappings::get( newBlock->mU );
        samplerDescriptor.tAddressMode  = MetalMappings::get( newBlock->mV );
        samplerDescriptor.rAddressMode  = MetalMappings::get( newBlock->mW );
        samplerDescriptor.normalizedCoordinates = YES;
        samplerDescriptor.lodMinClamp   = newBlock->mMinLod;
        samplerDescriptor.lodMaxClamp   = newBlock->mMaxLod;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        const bool supportsCompareFunction =
                [mActiveDevice->mDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v1];
#else
        const bool supportsCompareFunction = true;
#endif

        if( supportsCompareFunction )
        {
            if( newBlock->mCompareFunction != NUM_COMPARE_FUNCTIONS )
                samplerDescriptor.compareFunction = MetalMappings::get( newBlock->mCompareFunction );
        }

        id <MTLSamplerState> sampler =
                [mActiveDevice->mDevice newSamplerStateWithDescriptor:samplerDescriptor];

        newBlock->mRsData = const_cast<void*>( CFBridgingRetain( sampler ) );
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::destroyTexSample( TextureSampleUnit *block )
    {
        id <MTLSamplerState> sampler = reinterpret_cast< id <MTLSamplerState> >(
                    CFBridgingRelease( block->mRsData ) );
    }
    //-------------------------------------------------------------------------
    template <typename TDescriptorSetTexture,
              typename TTexSlot,
              typename TBufferPacked,
              bool isbufstroge>
    void MetalRenderSystem::_descriptorSetTextureCreated( TDescriptorSetTexture *newSet,
                                                          const vectorL<TTexSlot> &texContainer,
                                                          uint16 *shaderTypeTexCount )
    {
        MetalDescriptorSetTexture *metalSet = new MetalDescriptorSetTexture();

        metalSet->numTextureViews = 0;

        size_t numBuffers = 0;
        size_t numTextures = 0;

        size_t numBufferRanges = 0;
        size_t numTextureRanges = 0;

        bool needsNewTexRange = true;
        bool needsNewBufferRange = true;

        ShaderType shaderType = ST_VS;
        size_t numProcessedSlots = 0;

        typename vectorL<TTexSlot>::const_iterator itor = texContainer.begin();
        typename vectorL<TTexSlot>::const_iterator end  = texContainer.end();

        while( itor != end )
        {
            if( shaderTypeTexCount )
            {
                //We need to break the ranges if we crossed stages
                while( shaderType <= ST_FS &&
                       numProcessedSlots >= shaderTypeTexCount[shaderType] )
                {
                    numProcessedSlots = 0;
                    shaderType = static_cast<ShaderType>( shaderType + 1u );
                    needsNewTexRange = true;
                    needsNewBufferRange = true;
                }
            }

            if( itor->isTexture() )
            {
                needsNewBufferRange = true;
                if( needsNewTexRange )
                {
                    ++numTextureRanges;
                    needsNewTexRange = false;
                }

                const typename TDescriptorSetTexture::TextureSlot &texSlot = itor->getTexture();
                const Texture::Type::Texture::Type texType = texSlot.texture->getTextureType();
                if( !texSlot.isValidView() ||
                    ( isbufstroge &&
                      ( texType == Texture::T_CUBE || texType == Texture::Type::T_CUBEArray ) ) )
                {
                    ++metalSet->numTextureViews;
                }
                ++numTextures;
            }
            else
            {
                needsNewTexRange = true;
                if( needsNewBufferRange )
                {
                    ++numBufferRanges;
                    needsNewBufferRange = false;
                }

                ++numBuffers;
            }
            ++numProcessedSlots;
            ++itor;
        }

        //Create two contiguous arrays of texture and buffers, but we'll split
        //it into regions as a buffer could be in the middle of two textures.
        __unsafe_unretained id <MTLTexture> *textures = 0;
        __unsafe_unretained id <MTLBuffer> *buffers = 0;
        NSUInteger *offsets = 0;

        if( metalSet->numTextureViews > 0 )
        {
            //Create a third array to hold the strong reference
            //to the reinterpreted versions of textures.
            //Must be init to 0 before ARC sees it.
            void *textureViews = OGRE_MALLOC_SIMD( sizeof(id<MTLTexture>*) * metalSet->numTextureViews,
                                                   MEMCATEGORY_RENDERSYS );
            memset( textureViews, 0, sizeof(id<MTLTexture>*) * metalSet->numTextureViews );
            metalSet->textureViews = (__strong id <MTLTexture>*)textureViews;
        }
        if( numTextures > 0 )
        {
            textures = (__unsafe_unretained id <MTLTexture>*)
                       OGRE_MALLOC_SIMD( sizeof(id<MTLTexture>*) * numTextures, MEMCATEGORY_RENDERSYS );
        }
        if( numBuffers > 0 )
        {
            buffers = (__unsafe_unretained id <MTLBuffer>*)
                       OGRE_MALLOC_SIMD( sizeof(id<MTLBuffer>*) * numBuffers, MEMCATEGORY_RENDERSYS );
            offsets = (NSUInteger*)OGRE_MALLOC_SIMD( sizeof(NSUInteger) * numBuffers,
                                                     MEMCATEGORY_RENDERSYS );
        }

        metalSet->textures.reserve( numTextureRanges );
        metalSet->buffers.reserve( numBufferRanges );

        needsNewTexRange = true;
        needsNewBufferRange = true;

        shaderType = ST_VS;
        numProcessedSlots = 0;

        size_t texViewIndex = 0;

        itor = texContainer.begin();
        end  = texContainer.end();

        while( itor != end )
        {
            if( shaderTypeTexCount )
            {
                //We need to break the ranges if we crossed stages
                while( shaderType <= ST_FS &&
                       numProcessedSlots >= shaderTypeTexCount[shaderType] )
                {
                    numProcessedSlots = 0;
                    shaderType = static_cast<ShaderType>( shaderType + 1u );
                    needsNewTexRange = true;
                    needsNewBufferRange = true;
                }
            }

            if( itor->isTexture() )
            {
                needsNewBufferRange = true;
                if( needsNewTexRange )
                {
                    metalSet->textures.push_back( MetalTexRegion() );
                    MetalTexRegion &texRegion = metalSet->textures.back();
                    texRegion.textures = textures;
                    texRegion.shaderType = shaderType;
                    texRegion.range.location    = itor - texContainer.begin();
                    texRegion.range.length      = 0;
                    needsNewTexRange = false;
                }

                const typename TDescriptorSetTexture::TextureSlot &texSlot = itor->getTexture();

                N_assert( dynamic_cast<MetalTextureGpu*>( texSlot.texture ) );
                MetalTextureGpu *metalTex = static_cast<MetalTextureGpu*>( texSlot.texture );
                __unsafe_unretained id<MTLTexture> textureHandle = metalTex->getDisplayTextureName();

                const Texture::Type::Texture::Type texType = texSlot.texture->getTextureType();
                if( !texSlot.isValidView() || ( isbufstroge && ( texType == Texture::T_CUBE || texType == Texture::Type::T_CUBEArray ) ) )
                {
                    metalSet->textureViews[texViewIndex] = metalTex->getView( texSlot );
                    textureHandle = metalSet->textureViews[texViewIndex];
                    ++texViewIndex;
                }

                MetalTexRegion &texRegion = metalSet->textures.back();
                *textures = textureHandle;
                ++texRegion.range.length;

                ++textures;
            }
            else
            {
                needsNewTexRange = true;
                if( needsNewBufferRange )
                {
                    metalSet->buffers.push_back( MetalBufferRegion() );
                    MetalBufferRegion &bufferRegion = metalSet->buffers.back();
                    bufferRegion.buffers = buffers;
                    bufferRegion.offsets = offsets;
                    bufferRegion.shaderType = shaderType;
                    bufferRegion.range.location = itor - texContainer.begin();
                    bufferRegion.range.length   = 0;
                    needsNewBufferRange = false;
                }

                const typename TDescriptorSetTexture::BufferSlot &bufferSlot = itor->getBuffer();

                N_assert( dynamic_cast<TBufferPacked*>( bufferSlot.buffer ) );
                TBufferPacked *metalBuf = static_cast<TBufferPacked*>( bufferSlot.buffer );

                MetalBufferRegion &bufferRegion = metalSet->buffers.back();
                metalBuf->bindBufferForDescriptor( buffers, offsets, bufferSlot.offset );
                ++bufferRegion.range.length;

                ++buffers;
                ++offsets;
            }
            ++numProcessedSlots;
            ++itor;
        }

        newSet->mRsData = metalSet;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::destroyMetalDescriptorSetTexture( MetalDescriptorSetTexture *metalSet )
    {
        const size_t numTextureViews = metalSet->numTextureViews;
        for( size_t i=0; i<numTextureViews; ++i )
            metalSet->textureViews[i] = 0; //Let ARC free these pointers

        if( numTextureViews )
        {
            OGRE_FREE_SIMD( metalSet->textureViews, MEMCATEGORY_RENDERSYS );
            metalSet->textureViews = 0;
        }

        if( !metalSet->textures.empty() )
        {
            MetalTexRegion &texRegion = metalSet->textures.front();
            OGRE_FREE_SIMD( texRegion.textures, MEMCATEGORY_RENDERSYS );
        }

        if( !metalSet->buffers.empty() )
        {
            MetalBufferRegion &bufferRegion = metalSet->buffers.front();
            OGRE_FREE_SIMD( bufferRegion.buffers, MEMCATEGORY_RENDERSYS );
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::createTexture( ShaderChTexture *newSet )
    {
        _descriptorSetTextureCreated<
                ShaderChTexture,
                ShaderChTexture::Slot,
                MetalTextureBuffer,
                false>( newSet, newSet->mTextures, newSet->mShaderTypeTexCount );
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::destroyTexture( ShaderChTexture *set )
    {
        N_assert( set->mRsData );

        MetalDescriptorSetTexture *metalSet =
                reinterpret_cast<MetalDescriptorSetTexture*>( set->mRsData );

        destroyMetalDescriptorSetTexture( metalSet );
        delete metalSet;

        set->mRsData = 0;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::createBuffer( GpuParamBuffer *newSet )
    {
        _descriptorSetTextureCreated<
                GpuParamBuffer,
                GpuParamBuffer::Slot,
                MetalStorageBuffer,
                true>( newSet, newSet->mUavs, 0 );
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::destroyBuffer( GpuParamBuffer *set )
    {
        N_assert( set->mRsData );

        MetalDescriptorSetTexture *metalSet =
                reinterpret_cast<MetalDescriptorSetTexture*>( set->mRsData );

        destroyMetalDescriptorSetTexture( metalSet );
        delete metalSet;

        set->mRsData = 0;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setTextureSample( uint8 texUnit, const TextureSampleUnit *samplerblock )
    {
        N_assert( (!samplerblock || samplerblock->mRsData) &&
                "The block must have been created via RenderSys::getSamplerblock!" );

        if( !samplerblock )
        {
            [mActiveRenderEncoder setFragmentSamplerState:0 atIndex: texUnit];
        }
        else
        {
            __unsafe_unretained id <MTLSamplerState> sampler =
                    (__bridge id<MTLSamplerState>)samplerblock->mRsData;
            [mActiveRenderEncoder setVertexSamplerState:sampler atIndex: texUnit];
            [mActiveRenderEncoder setFragmentSamplerState:sampler atIndex: texUnit];
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setRSO( const RenderStateObject *pso )
    {
        MetalHlmsPso *metalPso = reinterpret_cast<MetalHlmsPso*>(pso->mExtData);

        if( pso && !mActiveRenderEncoder )
        {
            N_assert( mInterruptedRenderCommandEncoder &&
                    "mActiveRenderEncoder can only be null at this stage if rendering was interrupted."
                    " Did you call executeFBO?");
            executeFBO( false );
        }

        if( !mPso || mPso->depthStencilState != metalPso->depthStencilState )
            [mActiveRenderEncoder setDepthStencilState:metalPso->depthStencilState];

        const float biasSign = mReverseDepth ? 1.0f : -1.0f;
        [mActiveRenderEncoder setDepthBias:pso->mStencil->mDepthBiasConstant * biasSign
                                     slopeScale:pso->mStencil->mDepthBiasSlopeScale * biasSign
                                     clamp:0.0f];
        [mActiveRenderEncoder setCullMode:metalPso->cullMode];

        if( mPso != metalPso )
        {
            [mActiveRenderEncoder setRenderPipelineState:metalPso->pso];
            mPso = metalPso;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setCSO( const ComputeStateObject *pso )
    {
        __unsafe_unretained id<MTLComputePipelineState> metalPso =
                (__bridge id<MTLComputePipelineState>)pso->mExtData;

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder =
                mActiveDevice->getComputeEncoder();

        if( mComputePso != pso )
        {
            [computeEncoder setComputePipelineState:metalPso];
            mComputePso = pso;
        }
    }
    //-------------------------------------------------------------------------
    DataType MetalRenderSystem::getColourType(void) const
    {
        return DT_Colour_ARGB;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_execute( const ComputeStateObject &pso )
    {
        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mActiveDevice->getComputeEncoder();

        MTLSize numThreadGroups = MTLSizeMake( pso.mGroupCount[0],
                                               pso.mGroupCount[1],
                                               pso.mGroupCount[2] );
        MTLSize threadsPerGroup = MTLSizeMake( pso.mCeilCount[0],
                                               pso.mCeilCount[1],
                                               pso.mCeilCount[2] );

        [computeEncoder dispatchThreadgroups:numThreadGroups
                       threadsPerThreadgroup:threadsPerGroup];
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setGeometry( const GeometryRaw *vao )
    {
        __unsafe_unretained id<MTLBuffer> metalVertexBuffers[15];
        NSUInteger offsets[15];
        memset( offsets, 0, sizeof(offsets) );

        const VertexBufferList &vertexBuffers = vao->getVertexBuffers();

        size_t numVertexBuffers = 0;
        VertexBufferList::const_iterator itor = vertexBuffers.begin();
        VertexBufferList::const_iterator end  = vertexBuffers.end();

        while( itor != end )
        {
            MetalBufferOp *bufferInterface = static_cast<MetalBufferOp*>((*itor)->getBufferInterface() );
            metalVertexBuffers[numVertexBuffers++] = bufferInterface->getVboName();
            ++itor;
        }

#if OGRE_DEBUG_MODE
        N_assert( numVertexBuffers < 15u );
#endif

        [mActiveRenderEncoder setVertexBuffers:metalVertexBuffers offsets:offsets
                                               withRange:NSMakeRange( 0, numVertexBuffers )];
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_render( const CbDrawCallIndexed *cmd )
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_render( const CbDrawCallStrip *cmd )
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_renderEmulated( const CbDrawCallIndexed *cmd )
    {
        const GeometryRaw * vao = cmd->vao;
        IndexIndirect * drawCmd = reinterpret_cast<IndexIndirect*>(mIndirectPtr + (size_t)cmd->indirectBufferOffset );

        const MTLIndexType indexType = static_cast<MTLIndexType>( vao->mIndexBuffer->getIndexType() );
        const MTLPrimitiveType primType =  std::min(MTLPrimitiveTypeTriangleStrip, static_cast<MTLPrimitiveType>( vao->getType() - 1u ) );

        //Calculate bytesPerVertexBuffer & numVertexBuffers which is the same for all draws in this cmd
        uint32 bytesPerVertexBuffer[15];
        size_t numVertexBuffers = 0;
        const VertexBufferList &vertexBuffers = vao->getVertexBuffers();
        VertexBufferList::const_iterator itor = vertexBuffers.begin();
        VertexBufferList::const_iterator end  = vertexBuffers.end();

        while( itor != end )
        {
            bytesPerVertexBuffer[numVertexBuffers] = (*itor)->getBytesPerElement();
            ++numVertexBuffers;
            ++itor;
        }

        //Get index buffer stuff which is the same for all draws in this cmd
        const size_t bytesPerIndexElement = vao->mIndexBuffer->getBytesPerElement();

        MetalBufferOp *indexBufferInterface = static_cast<MetalBufferOp*>(vao->mIndexBuffer->getBufferInterface() );
        __unsafe_unretained id<MTLBuffer> indexBuffer = indexBufferInterface->getVboName();

        for( uint32 i=cmd->numDraws; i--; )
        {
#if OGRE_DEBUG_MODE
            N_assert( ((drawCmd->firstVertexIndex * bytesPerIndexElement) & 0x03) == 0
                    && "Index Buffer must be aligned to 4 bytes. If you're messing with "
                    "GeometryRaw::setVertexRange, you've entered an invalid "
                    "primStart; not supported by the Metal API." );
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            for( size_t j=0; j<numVertexBuffers; ++j )
            {
                //Manually set vertex buffer offsets since in iOS baseVertex is not supported
                //until iOS GPU Family 3 v1 & OS X (we just use indirect rendering there).
                [mActiveRenderEncoder setVertexBufferOffset:drawCmd->baseVertex * bytesPerVertexBuffer[j]
                                                            atIndex:j];
            }

            //Setup baseInstance.
            [mActiveRenderEncoder setVertexBufferOffset:drawCmd->baseInstance * sizeof(uint32)
                                                atIndex:15];

            [mActiveRenderEncoder drawIndexedPrimitives:primType
                       indexCount:drawCmd->primCount
                        indexType:indexType
                      indexBuffer:indexBuffer
                indexBufferOffset:drawCmd->firstVertexIndex * bytesPerIndexElement
                    instanceCount:drawCmd->instanceCount];
#else
            [mActiveRenderEncoder drawIndexedPrimitives:primType
                       indexCount:drawCmd->primCount
                        indexType:indexType
                      indexBuffer:indexBuffer
                indexBufferOffset:drawCmd->firstVertexIndex * bytesPerIndexElement
                    instanceCount:drawCmd->instanceCount
                       baseVertex:drawCmd->baseVertex
                     baseInstance:drawCmd->baseInstance];
#endif
            ++drawCmd;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_renderEmulated( const CbDrawCallStrip *cmd )
    {
        const GeometryRaw *vao = cmd->vao;
        VertexIndirect *drawCmd = reinterpret_cast<VertexIndirect*>(
                                    mIndirectPtr + (size_t)cmd->indirectBufferOffset );

        const MTLPrimitiveType primType =  std::min(
                    MTLPrimitiveTypeTriangleStrip,
                    static_cast<MTLPrimitiveType>( vao->getType() - 1u ) );

//        const size_t numVertexBuffers = vertexBuffers.size();
//        for( size_t j=0; j<numVertexBuffers; ++j )
//        {
//            //baseVertex is not needed as vertexStart does the same job.
//            [mActiveRenderEncoder setVertexBufferOffset:0 atIndex:j];
//        }

        for( uint32 i=cmd->numDraws; i--; )
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            //Setup baseInstance.
            [mActiveRenderEncoder setVertexBufferOffset:drawCmd->baseInstance * sizeof(uint32)
                                                atIndex:15];
            [mActiveRenderEncoder drawPrimitives:primType
                      vertexStart:drawCmd->firstVertexIndex
                      vertexCount:drawCmd->primCount
                    instanceCount:drawCmd->instanceCount];
#else
            [mActiveRenderEncoder drawPrimitives:primType
                      vertexStart:drawCmd->firstVertexIndex
                      vertexCount:drawCmd->primCount
                    instanceCount:drawCmd->instanceCount
                     baseInstance:drawCmd->baseInstance];
#endif
            ++drawCmd;
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_setRenderOperation(const v1::GeometryRaw * cmd)
    {
        __unsafe_unretained id<MTLBuffer> metalVertexBuffers[15];
        NSUInteger offsets[15];
        memset( offsets, 0, sizeof(offsets) );

        size_t maxUsedSlot = 0;
        const v1::VertexBufferBinding::VertexBufferBindingMap & binds = cmd->vertexData->vertexBufferBinding->getBindings();
        v1::VertexBufferBinding::VertexBufferBindingMap::const_iterator itor = binds.begin();
        v1::VertexBufferBinding::VertexBufferBindingMap::const_iterator end  = binds.end();

        while( itor != end )
        {
            v1::MetalVertexBuffer * metalBuffer = static_cast<v1::MetalVertexBuffer *>(itor->second.get());

            const size_t slot = itor->first;
#if OGRE_DEBUG_MODE
            N_assert( slot < 15u );
#endif
            size_t offsetStart;
            metalVertexBuffers[slot] = metalBuffer->getBufferName( offsetStart );
            offsets[slot] = offsetStart;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            offsets[slot] += cmd->vertexData->vertexStart * metalBuffer->getVertexSize();
#endif
            ++itor;
            maxUsedSlot = std::max(maxUsedSlot, slot + 1u);
        }

        [mActiveRenderEncoder setVertexBuffers:metalVertexBuffers offsets:offsets
                                               withRange:NSMakeRange(0, maxUsedSlot)];

        mCurrentIndexBuffer = cmd->indexData;
        mCurrentVertexBuffer= cmd->vertexData;
        mCurrentPrimType    = std::min(MTLPrimitiveTypeTriangleStrip, static_cast<MTLPrimitiveType>(cmd->mType - 1u));
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_render(const v1::CbDrawCallIndexed * cmd)
    {
        const MTLIndexType indexType = static_cast<MTLIndexType>(mCurrentIndexBuffer->indexBuffer->getType() );

        //Get index buffer stuff which is the same for all draws in this cmd
        const size_t bytesPerIndexElement = mCurrentIndexBuffer->indexBuffer->getIndexSize();

        size_t offsetStart;
        v1::MetalIndexBuffer *metalBuffer =
            static_cast<v1::MetalIndexBuffer*>( mCurrentIndexBuffer->indexBuffer.get() );
        __unsafe_unretained id<MTLBuffer> indexBuffer = metalBuffer->getBufferName( offsetStart );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    #if OGRE_DEBUG_MODE
            N_assert( ((cmd->firstVertexIndex * bytesPerIndexElement) & 0x03) == 0
                    && "Index Buffer must be aligned to 4 bytes. If you're messing with "
                    "IndexBuffer::indexStart, you've entered an invalid "
                    "indexStart; not supported by the Metal API." );
    #endif

        //Setup baseInstance.
        [mActiveRenderEncoder setVertexBufferOffset:cmd->baseInstance * sizeof(uint32)
                                            atIndex:15];

        [mActiveRenderEncoder drawIndexedPrimitives:mCurrentPrimType
                   indexCount:cmd->primCount
                    indexType:indexType
                  indexBuffer:indexBuffer
            indexBufferOffset:cmd->firstVertexIndex * bytesPerIndexElement + offsetStart
            instanceCount:cmd->instanceCount];
#else
        [mActiveRenderEncoder drawIndexedPrimitives:mCurrentPrimType
                   indexCount:cmd->primCount
                    indexType:indexType
                  indexBuffer:indexBuffer
            indexBufferOffset:cmd->firstVertexIndex * bytesPerIndexElement + offsetStart
                instanceCount:cmd->instanceCount
                   baseVertex:mCurrentVertexBuffer->vertexStart
                 baseInstance:cmd->baseInstance];
#endif
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_render( const v1::CbDrawCallStrip *cmd )
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        //Setup baseInstance.
        [mActiveRenderEncoder setVertexBufferOffset:cmd->baseInstance * sizeof(uint32)
                                            atIndex:15];
        [mActiveRenderEncoder drawPrimitives:mCurrentPrimType
                  vertexStart:0 /*cmd->firstVertexIndex already handled in _setRenderOperation*/
                  vertexCount:cmd->primCount
                instanceCount:cmd->instanceCount];
#else
        [mActiveRenderEncoder drawPrimitives:mCurrentPrimType
                  vertexStart:cmd->firstVertexIndex
                  vertexCount:cmd->primCount
                instanceCount:cmd->instanceCount
                 baseInstance:cmd->baseInstance];
#endif
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_render( const v1::GeometryRaw &op )
    {
        // Call super class.
        RenderSystem::_render(op);

        const size_t numberOfInstances = op.mInstanceCount;
        const bool hasInstanceData = mCurrentVertexBuffer->vertexBufferBinding->getHasInstanceData();

        // Render to screen!
        if( op.useIndexes )
        {
            do
            {
                // Update derived depth bias.
                if (mAutoDepthBias && mCurrentRender > 0)
                {
                    const float biasSign = mReverseDepth ? 1.0f : -1.0f;
                    [mActiveRenderEncoder setDepthBias:(mDepthBiasBase +
                                                        mDepthBiasFactor *
                                                        mCurrentRender) * biasSign
                                            slopeScale:mDepthBiasSlopeScale * biasSign
                                                 clamp:0.0f];
                }

                const MTLIndexType indexType = static_cast<MTLIndexType>(
                            mCurrentIndexBuffer->indexBuffer->getType() );

                //Get index buffer stuff which is the same for all draws in this cmd
                const size_t bytesPerIndexElement = mCurrentIndexBuffer->indexBuffer->getIndexSize();

                size_t offsetStart;
                v1::MetalIndexBuffer *metalBuffer =
                    static_cast<v1::MetalIndexBuffer*>( mCurrentIndexBuffer->indexBuffer.get() );
                __unsafe_unretained id<MTLBuffer> indexBuffer = metalBuffer->getBufferName( offsetStart );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    #if OGRE_DEBUG_MODE
                    N_assert( ((mCurrentIndexBuffer->indexStart * bytesPerIndexElement) & 0x03) == 0
                            && "Index Buffer must be aligned to 4 bytes. If you're messing with "
                            "IndexBuffer::indexStart, you've entered an invalid "
                            "indexStart; not supported by the Metal API." );
    #endif

                [mActiveRenderEncoder drawIndexedPrimitives:mCurrentPrimType
                           indexCount:mCurrentIndexBuffer->indexCount
                            indexType:indexType
                          indexBuffer:indexBuffer
                    indexBufferOffset:mCurrentIndexBuffer->indexStart * bytesPerIndexElement + offsetStart
                        instanceCount:numberOfInstances];
#else
                [mActiveRenderEncoder drawIndexedPrimitives:mCurrentPrimType
                           indexCount:mCurrentIndexBuffer->indexCount
                            indexType:indexType
                          indexBuffer:indexBuffer
                    indexBufferOffset:mCurrentIndexBuffer->indexStart * bytesPerIndexElement + offsetStart
                        instanceCount:numberOfInstances
                           baseVertex:mCurrentVertexBuffer->vertexStart
                         baseInstance:0];
#endif
            } while (updateParam());
        }
        else
        {
            do
            {
                // Update derived depth bias.
                if (mAutoDepthBias && mCurrentRender > 0)
                {
                    const float biasSign = mReverseDepth ? 1.0f : -1.0f;
                    [mActiveRenderEncoder setDepthBias:(mDepthBiasBase +
                                                        mDepthBiasFactor *
                                                        mCurrentRender) * biasSign
                                            slopeScale:mDepthBiasSlopeScale * biasSign
                                                 clamp:0.0f];
                }

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                const uint32 vertexStart = 0;
#else
                const uint32 vertexStart = static_cast<uint32>( mCurrentVertexBuffer->vertexStart );
#endif

                if (hasInstanceData)
                {
                    [mActiveRenderEncoder drawPrimitives:mCurrentPrimType
                                             vertexStart:vertexStart
                                             vertexCount:mCurrentVertexBuffer->vertexCount
                                           instanceCount:numberOfInstances];
                }
                else
                {
                    [mActiveRenderEncoder drawPrimitives:mCurrentPrimType
                                             vertexStart:vertexStart
                                             vertexCount:mCurrentVertexBuffer->vertexCount];
                }
            } while (updateParam());
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::on( GpuProgramType gptype, GpuProgramParam * params, uint16 variabilityMask )
    {
        MetalProgram *shader = 0;
        switch (gptype)
        {
        case GPT_VERTEX_PROGRAM:
            mActiveVGPP = params;
            shader = mPso->mVS;
            break;
        case GPT_FRAGMENT_PROGRAM:
            mActiveFGPP = params;
            shader = mPso->mFS;
            break;
        case GPT_GEOMETRY_PROGRAM:
            mActiveGGPP = params;
            N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Geometry Shaders are not supported in Metal." );
            break;
        case GPT_HULL_PROGRAM:
            mActiveTGPP = params;
            N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Tesselation is different in Metal." );
            break;
        case GPT_DOMAIN_PROGRAM:
            mActiveDGPP = params;
            N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Tesselation is different in Metal." );
            break;
        case GPT_COMPUTE_PROGRAM:
            mActiveCGPP = params;
            shader = static_cast<MetalProgram*>( mComputePso->mCS->getMain() );
            break;
        default:
            break;
        }

        size_t bytesToWrite = shader->getBufferRequiredSize();
        if( shader && bytesToWrite > 0 )
        {
            if( mCurrentAutoParamsBufferSpaceLeft < bytesToWrite )
            {
                if( mAutoParamsBufferIdx >= mAutoParamsBuffer.size() )
                {
                    StructBuffer *constBuffer =  mBufferManager->createConstBuffer(std::max<size_t>( 512u * 1024u, bytesToWrite ), M_Normal | M_PersistentMap, 0, false );
                    mAutoParamsBuffer.push_back( constBuffer );
                }

                StructBuffer *constBuffer = mAutoParamsBuffer[mAutoParamsBufferIdx];
                mCurrentAutoParamsBufferPtr = reinterpret_cast<uint8*>(constBuffer->map( 0, constBuffer->getNumElements() ) );
                mCurrentAutoParamsBufferSpaceLeft = constBuffer->getTotalSizeBytes();

                ++mAutoParamsBufferIdx;
            }

            shader->updateBuffers( params, mCurrentAutoParamsBufferPtr );

            N_assert( dynamic_cast<MetalStructBuffer *>(mAutoParamsBuffer[mAutoParamsBufferIdx-1u] ) );

            MetalStructBuffer * constBuffer = static_cast<MetalStructBuffer*>(mAutoParamsBuffer[mAutoParamsBufferIdx-1u] );
            const size_t bindOffset = constBuffer->getTotalSizeBytes() - mCurrentAutoParamsBufferSpaceLeft;
            switch (gptype)
            {
            case GPT_VERTEX_PROGRAM:
                constBuffer->bindBufferVS( OGRE_METAL_PARAMETER_SLOT - OGRE_METAL_CONST_SLOT_START, bindOffset );
                break;
            case GPT_FRAGMENT_PROGRAM:
                constBuffer->bindBufferPS( OGRE_METAL_PARAMETER_SLOT - OGRE_METAL_CONST_SLOT_START, bindOffset );
                break;
            case GPT_COMPUTE_PROGRAM:
                constBuffer->bindBufferCS( OGRE_METAL_CS_PARAMETER_SLOT - OGRE_METAL_CS_CONST_SLOT_START, bindOffset );
                break;
            case GPT_GEOMETRY_PROGRAM:
            case GPT_HULL_PROGRAM:
            case GPT_DOMAIN_PROGRAM:
                break;
            }

            mCurrentAutoParamsBufferPtr += bytesToWrite;

            const uint8 *oldBufferPos = mCurrentAutoParamsBufferPtr;
            mCurrentAutoParamsBufferPtr = reinterpret_cast<uint8*>(multiCeil( reinterpret_cast<uintptr_t>( mCurrentAutoParamsBufferPtr ), 
                mBufferManager->getAlignment(GBT_Struct) ) );
            bytesToWrite += mCurrentAutoParamsBufferPtr - oldBufferPos;

            //We know that bytesToWrite <= mCurrentAutoParamsBufferSpaceLeft, but that was
            //before padding. After padding this may no longer hold true.
            mCurrentAutoParamsBufferSpaceLeft -= std::min( mCurrentAutoParamsBufferSpaceLeft, bytesToWrite );
        }
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::bindGpuProgramPassIterationParameters(GpuProgramType gptype)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::clearBuffer( FrameBufferObject *fbobj,
                                              Texture *anyTarget, uint8 mipLevel )
    {
        Vector4 fullVp( 0, 0, 1, 1 );
        ViewportList vlist;
        createViewport(vlist, 1, anyTarget, &fullVp, &fullVp, mipLevel, false);
        beginFBO(fbobj, vlist);
        executeFBO();
    }
    //-------------------------------------------------------------------------
    Real MetalRenderSystem::getTexelXOffset(void)
    {
        return 0.0f;
    }
    //-------------------------------------------------------------------------
    Real MetalRenderSystem::getTexelYOffset(void)
    {
        return 0.0f;
    }
    //-------------------------------------------------------------------------
    Real MetalRenderSystem::getMinDepth(void)
    {
        return 0.0f;
    }
    //-------------------------------------------------------------------------
    Real MetalRenderSystem::getMaxDepth(void)
    {
        return 1.0f;
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::preExtraThreadsStarted()
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::postExtraThreadsStarted()
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::registerThread()
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::unregisterThread()
    {
    }
    //-------------------------------------------------------------------------
    /*const PixelFormatToShaderType* MetalRenderSystem::getPixelFormatToShaderType(void) const
    {
        return &mPixelFormatToShaderType;
    }
    */
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setupGPUProfiler(void)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
//        _rmt_BindMetal( mActiveDevice->mCurrentCommandBuffer );
#endif
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::shutdownGPUProfiler(void)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_UnbindMetal();
#endif
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::beginGPUProfiler( const String &name, uint32 *hashCache )
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_BeginMetalSample( name.c_str(), hashCache );
#endif
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::endGPUProfiler( const String &name )
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_EndMetalSample();
#endif
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::beginProfiler( const String &eventName )
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::endProfiler( void )
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::markProfiler( const String &event )
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setClipPlanesImpl(const PlaneList& clipPlanes)
    {
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::initImpl(RenderFeature* caps, Window *primary)
    {
        Texture::DepthFormat = PF_Depth32f_Stencil8ui_64;
        mGpuProgramManager = N_new MetalGpuProgramManager( &mDevice );
        mMetalProgramFactory = new MetalProgramFactory( &mDevice );
        HighLevelGpuProgramManager::getSingleton().addFactory( mMetalProgramFactory );
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::updateCompositorManager( CompositorManager2 *compositorManager )
    {
        // Metal requires that a frame's worth of rendering be invoked inside an autorelease pool.
        // This is true for both iOS and macOS.
        /*@autoreleasepool
        {
            compositorManager->_updateFusion();
        }*/
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::_flush(void)
    {
        endFBO( false );
        mActiveDevice->commitAndNextCommandBuffer();
    }
    //-------------------------------------------------------------------------
    void MetalRenderSystem::setStencil( uint32 refValue, const ShaderChStencil &stencil )
    {
        RenderSystem::setStencil( refValue, stencil );
        
        // There are two main cases:
        // 1. The active render encoder is valid and will be subsequently used for drawing.
        //      We need to set the stencil reference value on this encoder. We do this below.
        // 2. The active render is invalid or is about to go away.
        //      In this case, we need to set the stencil reference value on the new encoder when it is created
        //      (see createRenderEncoder). (In this case, the setStencilReferenceValue below in this wasted, but it is inexpensive).

        // Save this info so we can transfer it into a new encoder if necessary
        mStencilEnabled = stencil.enabled;
        if (mStencilEnabled)
        {
            mStencilRefValue = refValue;

            if( mActiveRenderEncoder )
                [mActiveRenderEncoder setStencilReferenceValue:refValue];
        }
    }
    //-------------------------------------------------------------------------
 }
