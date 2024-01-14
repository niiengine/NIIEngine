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

#include "OgreMetalPrerequisites.h"

#include "Vao/OgreMetalAsyncTicket.h"
#include "Vao/OgreMetalGBufferManager.h"

#include "Vao/OgreStagingBuffer.h"

#include "OgreMetalDevice.h"

#import <Metal/MTLCommandBuffer.h>
#include "OgreMetalTextureGpu.h"
#include "OgreMetalMappings.h"

#include "OgreStringConverter.h"

#include "OgreTextureBox.h"
#include "OgreTextureGpuManager.h"
#include "OgrePixelFormatGpuUtils.h"

#import "Metal/MTLBlitCommandEncoder.h"

namespace Ogre
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalAsyncTicket
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalAsyncTicket::MetalAsyncTicket( BufferPacked *creator, MappedBuffer * buf,
        NCount oft, NCount size, MetalDevice * device) :
        AsyncTicket( creator, buf, oft, size ),
        //mFenceName( 0 ),
        mDevice( device )
    {
        mFenceName = dispatch_semaphore_create( 0 );

        __block dispatch_semaphore_t blockSemaphore = mFenceName;
        [mDevice->mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
        {
            dispatch_semaphore_signal( blockSemaphore );
        }];
        //Flush now for accuracy with downloads.
        mDevice->commitAndNextCommandBuffer();
    }
    //-----------------------------------------------------------------------------------
    MetalAsyncTicket::~MetalAsyncTicket()
    {
        mFenceName = 0;
    }
    //-----------------------------------------------------------------------------------
    const void* MetalAsyncTicket::mapImpl(void)
    {
        if( mFenceName )
            mFenceName = MetalGBufferManager::waitFor( mFenceName, mDevice );

        return mMapBuffer->_mapForRead( mMapOffset, mSize);
    }
    //-----------------------------------------------------------------------------------
    OperationReason MetalAsyncTicket::isDataValid() const
    {
        if( mFenceName )
        {
            //Ask to return immediately and tell us about the fence
            const long result = dispatch_semaphore_wait( mFenceName, DISPATCH_TIME_NOW );
            if( result == 0 )
            {
                mFenceName = 0;
                return OR_OK;
            }
            else
            {
                return OR_BLOCK;
            }
        }

        return OR_OK;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalAsyncTextureTicket
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalAsyncTextureTicket::MetalAsyncTextureTicket( NCount width, NCount height,
                                                      NCount depth, NCount array,
                                                      Texture::Type textureType,
                                                      PixelFormat pf,
                                                      MetalGBufferManager *vaoManager,
                                                      MetalDevice *device ) :
        TextureAsync( width, height, depth, textureType, pf ),
        mVboName( 0 ),
        mDownloadFrame( 0 ),
        mAccurateFence( 0 ),
        mMag( vaoManager ),
        mDevice( device )
    {
        const uint32 rowAlignment = 4u;
        const size_t sizeBytes = PixelUtil::getSize( width, height, depth, 1u, mFormat, rowAlignment );

        MTLResourceOptions resourceOptions = MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeShared;
        mVboName = [mDevice->mDevice newBufferWithLength:sizeBytes options:resourceOptions];
        if( !mVboName )
        {
            N_EXCEPT( RenderingAPI,
                         "Out of GPU memory or driver refused.\n"
                         "Requested: " + StrConv::conv( sizeBytes ) + " bytes." );
        }
    }
    //-----------------------------------------------------------------------------------
    MetalAsyncTextureTicket::~MetalAsyncTextureTicket()
    {
        if( mBlockType == BT_Mapping )
            unmap();

        mVboName = 0;
        mAccurateFence = 0;
    }
    //-----------------------------------------------------------------------------------
    void MetalAsyncTextureTicket::writeImpl( TextureGpu * src, NCount srcmipmap, PixelBlock *srcBox, NCount oft, bool notify )
    {
        TextureAsync::writeImpl( src, srcmipmap, srcBox, oft, notify);

        mDownloadFrame = mMag->getFrameCount();

        mAccurateFence = 0;

        PixelBlock srcTextureBox;
        PixelBlock fullSrcTextureBox( src->getEmptyBox( srcmipmap ) );

        if( !srcBox )
            srcTextureBox = fullSrcTextureBox;
        else
        {
            srcTextureBox = *srcBox;
            srcTextureBox.mRowSize   = fullSrcTextureBox.mRowSize;
            srcTextureBox.mPixelSize = fullSrcTextureBox.mPixelSize;
            srcTextureBox.mSliceSize = fullSrcTextureBox.mSliceSize;
        }

        if( src->isPoolSlice() )
        {
//            fullSrcTextureBox.mArrayIdx= src->getPoolArray();
//            fullSrcTextureBox.mArray = src->getPool()->masterTexture->getArray();

            srcTextureBox.mArrayIdx += src->getPoolArray();
        }

        N_assert( dynamic_cast<MetalTextureGpu*>( src ) );
        MetalTextureGpu *srcTextureMetal = static_cast<MetalTextureGpu*>( src );

        __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();

        size_t destBytesPerRow = getRowSize();
        size_t destBytesPerImage = getSliceSize();

        if( mFormat == PFG_PVRTC2_2BPP || mFormat == PFG_PVRTC2_2BPP_SRGB ||
            mFormat == PFG_PVRTC2_4BPP || mFormat == PFG_PVRTC2_4BPP_SRGB ||
            mFormat == PFG_PVRTC_RGB2  || mFormat == PFG_PVRTC_RGB2_SRGB  ||
            mFormat == PFG_PVRTC_RGB4  || mFormat == PFG_PVRTC_RGB4_SRGB  ||
            mFormat == PFG_PVRTC_RGBA2 || mFormat == PFG_PVRTC_RGBA2_SRGB ||
            mFormat == PFG_PVRTC_RGBA4 || mFormat == PFG_PVRTC_RGBA4_SRGB )
        {
            destBytesPerRow = 0;
            destBytesPerImage = 0;
        }

        MTLOrigin mtlOrigin = MTLOriginMake( srcTextureBox.x, srcTextureBox.y, srcTextureBox.z );
        MTLSize mtlSize     = MTLSizeMake( srcTextureBox.width, srcTextureBox.height, srcTextureBox.depth );

        for( NSUInteger i=0; i<srcTextureBox.mArray; ++i )
        {
            [blitEncoder copyFromTexture:srcTextureMetal->getFinalTextureName()
                             sourceSlice:srcTextureBox.mArrayIdx + i
                             sourceLevel:srcmipmap
                            sourceOrigin:mtlOrigin
                              sourceSize:mtlSize
                                toBuffer:mVboName
                       destinationOffset:destBytesPerImage * i
                  destinationBytesPerRow:destBytesPerRow
                destinationBytesPerImage:destBytesPerImage];
        }

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        N_assert( mVboName.storageMode != MTLStorageModeManaged );
        //[blitEncoder synchronizeResource:mVboName];
#endif

        if( notify )
        {
            mAccurateFence = dispatch_semaphore_create( 0 );
            __weak dispatch_semaphore_t blockSemaphore = mAccurateFence;
            [mDevice->mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
            {
                if( blockSemaphore )
                    dispatch_semaphore_signal( blockSemaphore );
            }];
            //Flush now for accuracy with downloads.
            mDevice->commitAndNextCommandBuffer();
        }
    }
    //-----------------------------------------------------------------------------------
    PixelBlock MetalAsyncTextureTicket::mapImpl(NCount depthIdx, NCount arrayIdx, NCount oft)
    {
        waitForDownloadToFinish();

        PixelBlock retVal;

        retVal = PixelBlock( mWidth, mHeight, getDepth(), getArray(), PixelUtil::getUnitSize( mFormat ), getRowSize(), getSliceSize() );

        //if( PixelUtil::isCompress( mFormat ) )
            retVal.setFormat( mFormat );

        retVal.data = [mVboName contents];
        retVal.data = retVal.at( 0, 0, depthIdx );
        retVal.mArray -= depthIdx;

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalAsyncTextureTicket::unmapImpl(void)
    {
    }
    //-----------------------------------------------------------------------------------
    void MetalAsyncTextureTicket::waitForDownloadToFinish(void)
    {
        if( mBlockType != BT_Writting )
            return; //We're done.

        if( mAccurateFence )
        {
            mAccurateFence = MetalGBufferManager::waitFor( mAccurateFence, mDevice );
        }
        else
        {
            mMag->wait( mDownloadFrame );
            mQueryCount = 0;
        }

        mBlockType = BT_None;
    }
    //-----------------------------------------------------------------------------------
    bool MetalAsyncTextureTicket::queryIsTransferDone(void)
    {
        if( !TextureAsync::queryIsTransferDone() )
        {
            //Early out. The texture is not even finished being ready.
            //We didn't even start the actual download.
            return false;
        }

        bool retVal = false;

        if( mBlockType != BT_Writting )
        {
            retVal = true;
        }
        else if( mAccurateFence )
        {
            //Ask GL API to return immediately and tells us about the fence
            long result = dispatch_semaphore_wait( mAccurateFence, DISPATCH_TIME_NOW );
            if( result == 0 )
            {
                mAccurateFence = 0;
                if( mBlockType != BT_Mapping )
                    mBlockType = BT_None;
                retVal = true;
            }
        }
        else
        {
            if( mDownloadFrame == mMag->getFrameCount() )
            {
                if( mQueryCount > 3 )
                {
                    //Use is not calling vaoManager->update(). Likely it's stuck in an
                    //infinite loop checking if we're done, but we'll always return false.
                    //If so, switch to accurate tracking.
                    mAccurateFence = dispatch_semaphore_create( 0 );
                    __weak dispatch_semaphore_t blockSemaphore = mAccurateFence;
                    [mDevice->mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
                    {
                        if( blockSemaphore )
                            dispatch_semaphore_signal( blockSemaphore );
                    }];
                    //Flush now for accuracy with downloads.
                    mDevice->commitAndNextCommandBuffer();

                    LogManager::getSingleton().logMessage(
                                "WARNING: Calling TextureAsync::queryIsTransferDone too "
                                "often with innacurate tracking in the same frame this transfer "
                                "was issued. Switching to accurate tracking. If this is an accident, "
                                "wait until you've rendered a few frames before checking if it's done. "
                                "If this is on purpose, consider calling TextureAsync::download()"
                                " with accurate tracking enabled.", LML_CRITICAL );
                }

                ++mQueryCount;
            }

            //We're downloading but have no fence. That means we don't have accurate tracking.
            retVal = mMag->isFrameFinished( mDownloadFrame );
            ++mQueryCount;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
}
