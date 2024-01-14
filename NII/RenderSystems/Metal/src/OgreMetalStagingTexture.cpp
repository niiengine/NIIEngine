/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2017 Torus Knot Software Ltd

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

#include "OgreMetalStagingTexture.h"
#include "OgreMetalTextureGpu.h"

#include "OgreMetalDevice.h"
#include "OgreStringConverter.h"

#import "Metal/MTLBlitCommandEncoder.h"

namespace Ogre
{
    //-----------------------------------------------------------------------------------
    MetalStagingTexture::MetalStagingTexture( GBufferManager *vaoManager, PixelFormat format, size_t sizeBytes, MetalDevice *device ) :
        MappedTexture( vaoManager, format, sizeBytes, 0, 0 ),
        mMappedPtr( 0 ),
        mDevice( device )
    {
        MTLResourceOptions resourceOptions = MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared;
        mVboName = [mDevice->mDevice newBufferWithLength:sizeBytes options:resourceOptions];
        if( !mVboName )
        {
            N_EXCEPT( RenderingAPI,
                         "Out of GPU memory or driver refused.\n"
                         "Requested: " + StrConv::conv( sizeBytes ) + " bytes.",
                         "MetalStagingTexture::MetalStagingTexture" );
        }
        mMappedPtr = [mVboName contents];
    }
    //-----------------------------------------------------------------------------------
    MetalStagingTexture::~MetalStagingTexture()
    {
        mMappedPtr = 0;
        mVboName = 0;
    }
    //-----------------------------------------------------------------------------------
    bool MetalStagingTexture::belongsToUs( const PixelBlock &box )
    {
        return box.data >= mMappedPtr && box.data <= static_cast<uint8 *>(mMappedPtr) + mCurrentOffset;
    }
    //-----------------------------------------------------------------------------------
    PixelBlock MetalStagingTexture::mapImpl(const PixelVolume & vol)
    {
        N_assert( !mMapping && "startMapRegion already called!" );
        mMapping = true;

#if OGRE_DEBUG_MODE
        //We only warn if isBlock wasn't called. Because if you didn't wait
        //getDynamicBufferMultiplier frames and never called isBlock to check,
        //you're risking certain machines (with slower GPUs) to stall even if yours doesn't.
        if( mMag->getFrameCount() - mLastFrame < mMag->getDynamicBufferMultiplier() /*&&
            !mUserQueriedIfUploadWillStall*/ )
        {
            LogManager::getSingleton().logMessage(
                "PERFORMANCE WARNING: Calling MappedTexture::startMapRegion too soon, "
                "it will very likely stall depending on HW and OS setup. You need to wait"
                " GBufferManager::getDynamicBufferMultiplier frames after having called "
                "MappedTexture::upload before calling startMapRegion again.",
                LML_CRITICAL );
        }
#endif

        mMag->wait( mLastFrame );
        
        #define rowAlignment 4u
        const size_t sizeBytes = PixelUtil::getSize( vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray, vol.mFormat, rowAlignment );

        if( PixelUtil::isCompress( vol.mFormat ) )
        {
            //Ensure the offset is always aligned to the block size for compressed formats
            const size_t blockSizeBytes = PixelUtil::getUnitSize( pf );
            mCurrentOffset = multiCeil( mCurrentOffset, blockSizeBytes );
            mCurrentOffset = std::min( mCurrentOffset, mSize );
        }

        PixelBlock re;

        const size_t availableSize = mSize - mCurrentOffset;

        if( sizeBytes <= availableSize )
        {
            re.mWidth = vol.mWidth;
            re.mHeight = vol.mHeight;
            re.mDepth = vol.mDepth;
            re.mArray = vol.mArray;
            re.mPixelSize = PixelUtil::getUnitSize( vol.mFormat );
            re.mRowSize = PixelUtil::getSize( vol.mWidth, 1, 1, 1, vol.mFormat, rowAlignment );
            re.mSliceSize = PixelUtil::getSize( vol.mWidth, vol.mHeight, 1, 1, vol.mFormat, rowAlignment );
            re.mData = mMappedPtr + mCurrentOffset;;
            mCurrentOffset += sizeBytes;
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    void MetalStagingTexture::unmap(void)
    {
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//        NSRange range = NSMakeRange( 0, mCurrentOffset );
//        [mVboName didModifyRange:range];
#endif
        MappedTexture::unmap();
        mCurrentOffset = 0;
    }
    //-----------------------------------------------------------------------------------
    void MetalStagingTexture::read( TextureGpu *dst, NCount mipLevel, const PixelBlock * dstBox, 
        const PixelBlock & srcBox,  bool host )
    {
        if(host)
        {
            MappedTexture::read( dst, mipLevel, dstBox, srcBox, true );
            return ;
        }

        __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();

        size_t mRowSize      = srcBox.mRowSize;
        size_t mSliceSize    = srcBox.mSliceSize;

        // PVR textures must have 0 on these.
        if( mFormat == PFG_PVRTC2_2BPP || mFormat == PFG_PVRTC2_2BPP_SRGB ||
            mFormat == PFG_PVRTC2_4BPP || mFormat == PFG_PVRTC2_4BPP_SRGB ||
            mFormat == PFG_PVRTC_RGB2  || mFormat == PFG_PVRTC_RGB2_SRGB  ||
            mFormat == PFG_PVRTC_RGB4  || mFormat == PFG_PVRTC_RGB4_SRGB  ||
            mFormat == PFG_PVRTC_RGBA2 || mFormat == PFG_PVRTC_RGBA2_SRGB ||
            mFormat == PFG_PVRTC_RGBA4 || mFormat == PFG_PVRTC_RGBA4_SRGB )
        {
            mRowSize = 0;
            mSliceSize = 0;
        }

        // Recommended by documentation to set this value to 0 for non-3D textures.
        if( dst->getTextureType() != Texture::T_3D )
            mSliceSize = 0;

        N_assert( dynamic_cast<MetalTextureGpu *>( dst ) );
        MetalTextureGpu * dstTextureMetal = static_cast<MetalTextureGpu *>( dst );

        const size_t srcOffset = reinterpret_cast<uint8*>( srcBox.mData ) - reinterpret_cast<uint8*>( mMappedPtr );

        const NSUInteger destinationSlice = (dstBox ? dstBox->mArrayIdx : 0) + dst->getPoolArray();
        NSUInteger xPos = static_cast<NSUInteger>( dstBox ? dstBox->mLeft : 0 );
        NSUInteger yPos = static_cast<NSUInteger>( dstBox ? dstBox->mTop : 0 );
        NSUInteger zPos = static_cast<NSUInteger>( dstBox ? dstBox->mFront : 0 );

        for( NSUInteger i = 0; i < srcBox.mArray; ++i )
        {
            [blitEncoder copyFromBuffer:mVboName
                           sourceOffset:srcOffset + srcBox.mSliceSize * i
                      sourceBytesPerRow:mRowSize
                    sourceBytesPerImage:mSliceSize
                             sourceSize:MTLSizeMake( srcBox.mWidth, srcBox.mHeight, srcBox.mDepth )
                              toTexture:dstTextureMetal->getFinalTextureName()
                       destinationSlice:destinationSlice + i
                       destinationLevel:mipLevel
                      destinationOrigin:MTLOriginMake( xPos, yPos, zPos )];
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalStagingTexture::write(TextureGpu * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host)
    {
        
    }
    //-----------------------------------------------------------------------------------
}
