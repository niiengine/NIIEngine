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

#include "OgreMetalTextureGpu.h"
#include "OgreMetalMappings.h"
#include "OgreMetalTextureGpuManager.h"

#include "OgreTextureGpuListener.h"
#include "OgreTextureBox.h"
#include "OgreVector2.h"

#include "Vao/OgreGBufferManager.h"

#include "OgreMetalDevice.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

#import "Metal/MTLBlitCommandEncoder.h"


namespace Ogre
{
    MetalTextureGpu::MetalTextureGpu( GpuPageOutStrategy::GpuPageOutStrategy rtype,
                                      GBufferManager *vaoManager, IdString name, uint32 textureFlags,
                                      Texture::Type::Texture::Type initialType,
                                      TextureManager *textureManager ) :
        TextureGpu( rtype, vaoManager, name, textureFlags, initialType, textureManager ),
        mDisplayTextureName( 0 ),
        mFinalTextureName( 0 ),
        mMsaaFramebufferName( 0 )
    {
        _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    MetalTextureGpu::~MetalTextureGpu()
    {
        freeInternalImpl(); // 2
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::createInternalImpl(void)
    {
        if( mPixelFormat == PFG_NULL )
            return; //Nothing to do

        MTLTextureDescriptor *desc = [MTLTextureDescriptor new];
        desc.mipmapLevelCount   = mNumMipmaps;
        desc.textureType        = getMetalTextureType();
        desc.width              = mWidth;
        desc.height             = mHeight;
        desc.depth              = getDepth();
        desc.arrayLength        = getArray();
        desc.pf        = MetalMappings::get( mPixelFormat );
        desc.sampleCount        = 1u;
        desc.storageMode        = MTLStorageModePrivate;

        if( mTextureType == Texture::T_CUBE || mTextureType == Texture::Type::T_CUBEArray )
            desc.arrayLength /= 6u;

        if( isMultiSample() && isManualMSAA() )
        {
            desc.textureType = MTLTextureType2DMultisample;
            desc.sampleCount = mSrcSample.getMultiSampling();
        }

        desc.usage = MTLTextureUsageShaderRead;
        if( isFrame() )
            desc.usage |= MTLTextureUsageRenderTarget;
        if( isBufferStroge() )
            desc.usage |= MTLTextureUsageShaderWrite;
        if( isAlterView() )
            desc.usage |= MTLTextureUsagePixelFormatView;

        String textureName = getName();

        MetalTextureGpuManager *textureManagerMetal =
                static_cast<MetalTextureGpuManager*>( mTextureManager );
        MetalDevice *device = textureManagerMetal->getDevice();

        mFinalTextureName = [device->mDevice newTextureWithDescriptor:desc];
        if( !mFinalTextureName )
        {
            size_t sizeBytes = PixelUtil::calculateSizeBytes( mWidth, mHeight, getDepth(),
                                                                        getArray(),
                                                                        mPixelFormat, mNumMipmaps, 4u );
            if( isMultiSample() && isManualMSAA() )
                sizeBytes *= mSrcSample.getMultiSampling();
            N_EXCEPT( RenderingAPI,
                         "Out of GPU memory or driver refused.\n"
                         "Requested: " + StrConv::conv( sizeBytes ) + " bytes.",
                         "MetalTextureGpu::createInternalImpl" );
        }
        mFinalTextureName.label = [NSString stringWithUTF8String:textureName.c_str()];

        if( isMultiSample() && !isManualMSAA() )
        {
            desc.textureType    = MTLTextureType2DMultisample;
            desc.depth          = 1u;
            desc.arrayLength    = 1u;
            desc.sampleCount    = mSrcSample.getMultiSampling();
            desc.usage          = MTLTextureUsageRenderTarget;
            mMsaaFramebufferName = [device->mDevice newTextureWithDescriptor:desc];
            if( !mMsaaFramebufferName )
            {
                size_t sizeBytes = PixelUtil::calculateSizeBytes( mWidth, mHeight, getDepth(),
                                                                            getArray(),
                                                                            mPixelFormat, mNumMipmaps, 4u );
                sizeBytes *= mSrcSample.getMultiSampling();
                N_EXCEPT( RenderingAPI, "Out of GPU memory or driver refused (MSAA surface).\n Requested: " + StrConv::conv( sizeBytes ) + " bytes.");
            }
            mMsaaFramebufferName.label = [NSString stringWithUTF8String:(textureName + "_MSAA").c_str()];
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::freeInternalImpl(void) // 2
    {
        if( mFinalTextureName )
            mFinalTextureName = 0;
        if( mMsaaFramebufferName )
            mMsaaFramebufferName = 0;

        if( isPoolSlice() )
        {
            if( mPool )
            {
                //This will end up calling poolImpl,
                //setting mPool & mPoolArray to 0
                mTextureManager->poolFree( this );
            }
        }

        _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    MTLTextureType MetalTextureGpu::getMetalTextureType(void) const
    {
        switch( mTextureType )
        {
            case Texture::T_Unknow:         return MTLTextureType2D;
            case Texture::T_1D:          return MTLTextureType1D;
            case Texture::T_1DArray:     return MTLTextureType1DArray;
            case Texture::T_2D:          return MTLTextureType2D;
            case Texture::T_2DArray:     return MTLTextureType2DArray;
            case Texture::T_CUBE:        return MTLTextureTypeCube;
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS || __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_11_0
            case Texture::Type::T_CUBEArray:   return MTLTextureTypeCubeArray;
#endif
            case Texture::T_3D:          return MTLTextureType3D;
            default:
                return (MTLTextureType)999;
        };
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::notifyGpuData(void)
    {
        N_assert( mCurrentStorage == StorageType::Resident );
        N_assert( mFinalTextureName || mPixelFormat == PFG_NULL );

        mDisplayTextureName = mFinalTextureName;

        process( Texture::Process::O_Valid );
    }
    //-----------------------------------------------------------------------------------
    bool MetalTextureGpu::isValidImpl(void) const
    {
        return mDisplayTextureName == mFinalTextureName;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::_setToDisplayDummyTexture(void)
    {
        if( !mTextureManager )
        {
            N_assert( isWindow() );
            return; //This can happen if we're a window and we're on shutdown
        }

        MetalTextureGpuManager *textureManagerMetal =
                static_cast<MetalTextureGpuManager*>( mTextureManager );
        if( isPoolSlice() )
        {
            mDisplayTextureName =
                    textureManagerMetal->getBlankTextureMetalName( Texture::T_2DArray );
        }
        else
        {
            mDisplayTextureName = textureManagerMetal->getBlankTextureMetalName( mTextureType );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::poolImpl( const TexturePool *newPool, uint16 slice )
    {
        TextureGpu::poolImpl( newPool, slice );

        _setToDisplayDummyTexture();

        if( mPool )
        {
            N_assert( dynamic_cast<MetalTextureGpu*>( mPool->masterTexture ) );
            MetalTextureGpu *masterTexture = static_cast<MetalTextureGpu*>(mPool->masterTexture);
            mFinalTextureName = masterTexture->mFinalTextureName;
        }

        process( Texture::Process::O_PoolAlter );
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::setTextureType( Texture::Type::Texture::Type textureType )
    {
        const Texture::Type::Texture::Type oldType = mTextureType;
        TextureGpu::setTextureType( textureType );

        if( oldType != mTextureType && mDisplayTextureName != mFinalTextureName )
            _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::read( TextureGpu *dst, const PixelBlock &dstBox, uint8 dstMipLevel,
                                  const PixelBlock &srcBox, uint8 srcMipLevel,
                                  bool keepResolvedTexSynced )
    {
        TextureGpu::read( dst, dstBox, dstMipLevel, srcBox, srcMipLevel );

        N_assert( dynamic_cast<MetalTextureGpu*>( dst ) );

        MetalTextureGpu *dstMetal = static_cast<MetalTextureGpu*>( dst );
        MetalTextureGpuManager *textureManagerMetal =
                static_cast<MetalTextureGpuManager*>( mTextureManager );
        MetalDevice *device = textureManagerMetal->getDevice();

        uint32 mArray = srcBox.mArray;

        id<MTLTexture> srcTextureName = this->mFinalTextureName;
        id<MTLTexture> dstTextureName = dstMetal->mFinalTextureName;

        if( this->isMultiSample() && !this->isManualMSAA() )
            srcTextureName = this->mMsaaFramebufferName;
        if( dstMetal->isMultiSample() && !dstMetal->isManualMSAA() )
            dstTextureName = dstMetal->mMsaaFramebufferName;

        __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = device->getBlitEncoder();
        for( uint32 slice=0; slice<mArray; ++slice )
        {
            [blitEncoder copyFromTexture:srcTextureName
                             sourceSlice:srcBox.mArrayIdx + this->getPoolArray() + slice
                             sourceLevel:srcMipLevel
                            sourceOrigin:MTLOriginMake( srcBox.x, srcBox.y, srcBox.z )
                              sourceSize:MTLSizeMake( srcBox.width, srcBox.height, srcBox.depth )
                               toTexture:dstTextureName
                        destinationSlice:dstBox.mArrayIdx + dstMetal->getPoolArray() + slice
                        destinationLevel:dstMipLevel
                       destinationOrigin:MTLOriginMake( dstBox.x, dstBox.y, dstBox.z )];
        }

        //Must keep the resolved texture up to date.
        if( dstMetal->isMultiSample() && !dstMetal->isManualMSAA() && keepResolvedTexSynced )
        {
            device->endAllEncoders();

            MTLRenderPassDescriptor *passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
            passDesc.colorAttachments[0].texture = dstMetal->mMsaaFramebufferName;
            passDesc.colorAttachments[0].resolveTexture = dstMetal->mFinalTextureName;
            passDesc.colorAttachments[0].loadAction = MTLLoadActionLoad;
            passDesc.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;

            for( uint32 slice=0; slice<mArray; ++slice )
            {
                passDesc.colorAttachments[0].slice = dstBox.mArrayIdx + dstMetal->getPoolArray() + slice;
                passDesc.colorAttachments[0].resolveSlice = dstBox.mArrayIdx + dstMetal->getPoolArray() + slice;
                passDesc.colorAttachments[0].level = dstMipLevel;
                passDesc.colorAttachments[0].resolveLevel = dstMipLevel;
            }

            device->mRenderEncoder =
                    [device->mCurrentCommandBuffer renderCommandEncoderWithDescriptor:passDesc];
            device->endRenderEncoder( false );
        }

        //Do not perform the sync if notifyGpuData hasn't been called yet (i.e. we're
        //still building the HW mipmaps, and the texture will never be ready)
        if( dst->isValidImpl() &&
            dst->getGpuPageOutStrategy() == GpuPageOutStrategy::AlwaysKeepSystemRamCopy )
        {
            dst->downloadImpl();
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::mipmap(void)
    {
        MetalTextureGpuManager *textureManagerMetal =
                static_cast<MetalTextureGpuManager*>( mTextureManager );
        MetalDevice *device = textureManagerMetal->getDevice();
        __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = device->getBlitEncoder();
        [blitEncoder generateMipmapsForTexture: mFinalTextureName];
    }
    //-----------------------------------------------------------------------------------
    id<MTLTexture> MetalTextureGpu::getView( PixelFormat pf, uint8 mipLevel,
                                             uint8 numMipmaps, uint16 arraySlice,
                                             bool cubemapsAs2DArrays, bool forUav )
    {
        if( pf == PF_UNKNOWN )
        {
            pf = mPixelFormat;
            if( forUav )
                pf = PixelUtil::getLinear( pf );
        }
        MTLTextureType texType = this->getMetalTextureType();

        if( isMultiSample() && isManualMSAA() )
            texType = MTLTextureType2DMultisample;

        if( ( cubemapsAs2DArrays || forUav ) &&          //
            ( mTextureType == Texture::T_CUBE ||  //
              mTextureType == Texture::Type::T_CUBEArray ) )
        {
            texType = MTLTextureType2DArray;
        }

        if( !numMipmaps )
            numMipmaps = mNumMipmaps - mipLevel;

        N_assert( numMipmaps <= mNumMipmaps - mipLevel &&
                         "Asking for more mipmaps than the texture has!" );

        NSRange mipLevels;
        mipLevels = NSMakeRange( mipLevel, numMipmaps );
        NSRange slices;
        slices = NSMakeRange( arraySlice, this->getArray() - arraySlice );

        return [mDisplayTextureName newTextureViewWithPixelFormat:MetalMappings::get( pf )
                                                      textureType:texType
                                                           levels:mipLevels
                                                           slices:slices];
    }
    //-----------------------------------------------------------------------------------
    id<MTLTexture> MetalTextureGpu::getView( ShaderChTexture::TextureSlot texSlot )
    {
        return getView( texSlot.pf, texSlot.mipmapLevel, texSlot.numMipmaps,
                        texSlot.textureArrayIndex, texSlot.cubemapsAs2DArrays, false );
    }
    //-----------------------------------------------------------------------------------
    id<MTLTexture> MetalTextureGpu::getView( DescriptorSetUav::TextureSlot texSlot )
    {
        return getView( texSlot.pf, texSlot.mipmapLevel, 1u,
                        texSlot.textureArrayIndex, false, true );
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpu::getMultiSampleCoord(Vector2List & locations )
    {
        locations.reserve( mSrcSample.getMultiSampling() );
        if( mSrcSample.getMultiSampling() <= 1u )
        {
            locations.push_back( Vector2( 0.0f, 0.0f ) );
        }
        else
        {
            N_assert( mSrcSample.getType() != CoordType::CT_Unknow );

            //TODO
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalTextureGpuRenderTarget::MetalTextureGpuRenderTarget(
            GpuPageOutStrategy::GpuPageOutStrategy rtype,
            GBufferManager *vaoManager, IdString name, uint32 textureFlags,
            Texture::Type::Texture::Type initialType,
            TextureManager *textureManager ) :
        MetalTextureGpu( rtype, vaoManager, name,
                         textureFlags, initialType, textureManager ),
        mDSPool( 1u ),
        mDSTexture( false ),
        mDSFormat( PF_UNKNOWN )
    {
        if( mPixelFormat == PFG_NULL )
            mDSPool = 0;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuRenderTarget::_setDepthBufferDefaults(
            uint16 depthBufferPoolId, bool preferDepthTexture, PixelFormat desiredDepthBufferFormat )
    {
        N_assert( isFrame() );
        mDSPool          = depthBufferPoolId;
        mDSTexture         = preferDepthTexture;
        mDSFormat   = desiredDepthBufferFormat;
    }
    //-----------------------------------------------------------------------------------
    uint16 MetalTextureGpuRenderTarget::getDepthBufferPoolId(void) const
    {
        return mDSPool;
    }
    //-----------------------------------------------------------------------------------
    bool MetalTextureGpuRenderTarget::getPreferDepthTexture(void) const
    {
        return mDSTexture;
    }
    //-----------------------------------------------------------------------------------
    PixelFormat MetalTextureGpuRenderTarget::getDesiredDepthBufferFormat(void) const
    {
        return mDSFormat;
    }
}
