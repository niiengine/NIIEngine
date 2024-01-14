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

#include "OgreMetalHardwareBufferManager.h"
#include "OgreMetalBuffer.h"
#include "OgreMetalDiscardBufferManager.h"
#include "OgreMetalDevice.h"

#include "Vao/OgreMetalGBufferManager.h"
#include "Vao/OgreMetalBufferInterface.h"

#include "OgreMetalHardwareBufferCommon.h"

#include "OgreStringConverter.h"

#import <Metal/MTLBlitCommandEncoder.h>
#import <Metal/MTLCommandBuffer.h>

#import <Metal/MTLComputeCommandEncoder.h>
#import <Metal/MTLRenderCommandEncoder.h>

#include "Vao/OgreMetalStagingBuffer.h"

#import <Metal/MTLDevice.h>

namespace Ogre 
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalHardwareBufferCommon
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalHardwareBufferCommon::MetalHardwareBufferCommon( size_t sizeBytes, Buffer::Usage usage, uint16 alignment,
                                                          MetalDiscardBufferManager * discardBufferMgr, MetalDevice * device ) :
        mBuffer( 0 ),
        mSizeBytes( sizeBytes ),
        mDevice( device ),
        mDiscardBuffer( 0 ),
        mMag( discardBufferMgr->getGBufferManager() ),
        mStagingBuffer( 0 ),
        //mLastFrame( 0 ),
        mLastFrameGpuWrote( 0 )
    {
        mLastFrame = mMag->getFrameCount() - mMag->getMutilCount();
        mLastFrameGpuWrote = mLastFrame;

        MTLResourceOptions resourceOptions = 0;

        if( usage & Buffer::HBU_WRITE_ONLY )
        {
            resourceOptions |= MTLResourceStorageModePrivate;
            resourceOptions |= MTLResourceCPUCacheModeWriteCombined;
        }
        else
        {
            resourceOptions |= MTLResourceStorageModeShared;
        }

        if( !(usage & Buffer::HBU_DISCARDABLE) )
        {
            const size_t sizeBytesAligned = multiCeil( sizeBytes, 4u );
            mBuffer = [mDevice->mDevice newBufferWithLength:sizeBytesAligned
                                                    options:resourceOptions];
            if( !mBuffer )
            {
                N_EXCEPT( RenderingAPI,
                             "Out of GPU memory or driver refused.\n"
                             "Requested: " + StrConv::conv( sizeBytesAligned ) + " bytes.",
                             "MetalHardwareBufferCommon::MetalHardwareBufferCommon" );
            }
        }
        else
        {
            mDiscardBuffer = discardBufferMgr->createDiscardBuffer( sizeBytes, alignment );
        }
    }
    //-----------------------------------------------------------------------------------
    MetalHardwareBufferCommon::~MetalHardwareBufferCommon()
    {
        mBuffer = 0;
        if( mDiscardBuffer )
        {
            MetalDiscardBufferManager *discardBufferManager = mDiscardBuffer->getOwner();
            discardBufferManager->destroyDiscardBuffer( mDiscardBuffer );
            mDiscardBuffer = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalHardwareBufferCommon::_notifyDeviceStalled(void)
    {
        mLastFrame      = mMag->getFrameCount() - mMag->getMutilCount();
        mLastFrameGpuWrote  = mLastFrame;
    }
    //-----------------------------------------------------------------------------------
    id<MTLBuffer> MetalHardwareBufferCommon::getBufferName( size_t &outOffset )
    {
        mLastFrame = mMag->getFrameCount();
        outOffset = 0;
        return !mDiscardBuffer ? mBuffer : mDiscardBuffer->getBufferName( outOffset );
    }
    //-----------------------------------------------------------------------------------
    id<MTLBuffer> MetalHardwareBufferCommon::getBufferNameForGpuWrite(void)
    {
        N_assert( !mDiscardBuffer && "Discardable buffers can't be written from GPU!" );
        mLastFrame      = mMag->getFrameCount();
        mLastFrameGpuWrote  = mLastFrame;
        return mBuffer;
    }
    //-----------------------------------------------------------------------------------
    void* MetalHardwareBufferCommon::lockImpl( size_t offset, size_t length,
                                               Buffer::LockOptions options,
                                               bool isLocked )
    {
        if( isLocked )
        {
            N_EXCEPT( Exception::ERR_INVALID_STATE,
                         "Invalid attempt to lock a buffer that has already been locked",
                         "MetalHardwareBufferCommon::lock" );
        }

        void *retPtr = 0;

        const uint32 currentFrame       = mMag->getFrameCount();
        const uint32 bufferMultiplier   = mMag->getMutilCount();

        if( mDiscardBuffer )
        {
            //If we're here, it was created with HBU_DISCARDABLE bit
            if( options == Buffer::HBL_READ_ONLY )
            {
                LogManager::getSingleton().logMessage(
                            "PERFORMANCE WARNING: reads from discardable "
                            "buffers are uncached. May be slow." );

                //We can't write from GPU to discardable memory. No need to check
            }
            else if( options == Buffer::HBL_NORMAL || options == Buffer::HBL_WRITE_ONLY )
            {
                if( currentFrame - mLastFrame < bufferMultiplier )
                {
                    LogManager::getSingleton().logMessage(
                                "PERFORMANCE WARNING: locking with HBL_NORMAL or HBL_WRITE_ONLY for a "
                                "buffer created with HBU_DISCARDABLE bit is slow/stalling. Consider "
                                "locking w/ another locking option, or change the buffer's usage flag" );
                    mDevice->stall();
                }
            }

            retPtr = mDiscardBuffer->map( options != Buffer::HBL_DISCARD );
            retPtr = static_cast<void*>( static_cast<uint8*>( retPtr ) + offset );
        }
        else
        {
            if( mBuffer.storageMode != MTLStorageModePrivate )
            {
                if( options == Buffer::HBL_READ_ONLY )
                {
                    if( currentFrame - mLastFrameGpuWrote < bufferMultiplier )
                        mDevice->stall();
                }
                else if( options != Buffer::HBL_NO_OVERWRITE )
                {
                    if( currentFrame - mLastFrame < bufferMultiplier )
                    {
                        LogManager::getSingleton().logMessage(
                                    "PERFORMANCE WARNING: locking to a non-HBU_WRITE_ONLY or "
                                    "non-HBU_DISCARDABLE for other than reading is slow/stalling." );

                        mDevice->stall();
                    }
                }

                retPtr = [mBuffer contents];
                retPtr = static_cast<void*>( static_cast<uint8*>( retPtr ) + offset );
            }
            else
            {
                //If we're here, the buffer was created with HBU_WRITE_ONLY, but not discardable.
                //Write to a staging buffer to avoid blocking. We don't have to care about
                //reading access.
                N_assert( (options != Buffer::HBL_NORMAL &&
                        options != Buffer::HBL_READ_ONLY) &&
                        "Reading from a write-only buffer! Create "
                        "the buffer without HBL_WRITE_ONLY bit (or use readData)" );

                N_assert( !mStagingBuffer && "Invalid state, and mStagingBuffer will leak" );

                mStagingBuffer = mMag->getStagingBuffer( length, true );
                retPtr = mStagingBuffer->map( length );
            }
        }

        return retPtr;
    }
    //-----------------------------------------------------------------------------------
    void MetalHardwareBufferCommon::unlockImpl( size_t lockStart, size_t lockSize )
    {
        if( mDiscardBuffer )
            mDiscardBuffer->unmap();

        if( mStagingBuffer )
        {
            static_cast<MetalMappedBuffer*>( mStagingBuffer )->_unmapToV1( this, lockStart, lockSize );
            mStagingBuffer->removeReferenceCount();
            mStagingBuffer = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalHardwareBufferCommon::readData( size_t offset, size_t length, void* pDest )
    {
        N_assert( (offset + length) <= mSizeBytes );

        void const *srcData = 0;
        StagingBuffer *stagingBuffer = 0;

        const uint32 currentFrame       = mMag->getFrameCount();
        const uint32 bufferMultiplier   = mMag->getMutilCount();

        if( mDiscardBuffer )
           {
            //We can't write from GPU to discardable memory. No need to check.
            srcData = mDiscardBuffer->map( true );
        }
        else
        {
            if( mBuffer.storageMode != MTLStorageModePrivate )
            {
                if( currentFrame - mLastFrameGpuWrote < bufferMultiplier )
                    mDevice->stall();
                srcData = [mBuffer contents];
            }
            else
            {
                //Reading from HBL_WRITE_ONLY.
                stagingBuffer = mMag->getStagingBuffer( length, false );
                size_t stagingBufferOffset = static_cast<MetalMappedBuffer*>(
                            stagingBuffer )->_asyncDownloadV1( this, offset, length );
                mDevice->stall();
                srcData = stagingBuffer->_mapForRead( stagingBufferOffset, length );
                offset = 0;
            }
        }

        srcData = static_cast<const void*>( static_cast<const uint8*>( srcData ) + offset );

        memcpy( pDest, srcData, length );

        if( stagingBuffer )
        {
            stagingBuffer->unmap( 0, 0 );
            stagingBuffer->removeReferenceCount();
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalHardwareBufferCommon::writeData( size_t offset, size_t length,
                                               const void* pSource,
                                               bool discardWholeBuffer )
    {
        if( (discardWholeBuffer && mDiscardBuffer) || mBuffer.storageMode == MTLStorageModePrivate )
        {
            //Fast path is through locking (it either discards or already uses a StagingBuffer).
            void *dstData = this->lockImpl( offset, length, Buffer::HBL_DISCARD, false );
            memcpy( dstData, pSource, length );
            this->unlockImpl( offset, length );
        }
        else
        {
            //Use a StagingBuffer to avoid blocking
            StagingBuffer *stagingBuffer = mMag->getStagingBuffer( length, true );
            void *dstData = stagingBuffer->map( length );
            memcpy( dstData, pSource, length );
            static_cast<MetalMappedBuffer*>( stagingBuffer )->_unmapToV1( this, offset, length );
            stagingBuffer->removeReferenceCount();
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalHardwareBufferCommon::copyData( MetalHardwareBufferCommon *srcBuffer, size_t srcOffset,
                                              size_t dstOffset, size_t length, bool discardWholeBuffer )
    {
        if( !this->mDiscardBuffer || srcBuffer->mBuffer.storageMode == MTLStorageModePrivate )
        {
            size_t srcOffsetStart = 0;
            __unsafe_unretained id<MTLBuffer> srcBuf = srcBuffer->getBufferName( srcOffsetStart );
            __unsafe_unretained id<MTLBuffer> dstBuf = this->getBufferNameForGpuWrite();

            __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();
            [blitEncoder copyFromBuffer:srcBuf
                           sourceOffset:srcOffset + srcOffsetStart
                               toBuffer:dstBuf
                      destinationOffset:dstOffset
                                   size:length];

            if( this->mDiscardBuffer )
                mDevice->stall();
        }
        else
        {
            Buffer::LockOptions dstOption;
            if( discardWholeBuffer )
                dstOption = Buffer::HBL_DISCARD;
            else
                dstOption = Buffer::HBL_WRITE_ONLY;

            const void *srcData = srcBuffer->lockImpl( srcOffset, length,
                                                       Buffer::HBL_READ_ONLY, false );
            void *dstData = this->lockImpl( dstOffset, length, dstOption, false );

            memcpy( dstData, srcData, length );

            this->unlockImpl( dstOffset, length );
            srcBuffer->unlockImpl( srcOffset, length );
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalBuffer::MetalBuffer( id<MTLBuffer> vboName, size_t vboSize ) :
        mVboName( vboName ),
        mVboSize( vboSize ),
        mMappedPtr( 0 )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalBuffer::~MetalBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    size_t MetalBuffer::addMappedRange( NCount start, NCount count )
    {
        size_t ticket;

        if( !mFreeRanges.empty() )
        {
            ticket = mFreeRanges.back();
            mMappedRanges[ticket] = MappedRange( start, count );
            mFreeRanges.pop_back();
        }
        else
        {
            ticket = mMappedRanges.size();
            mMappedRanges.push_back( MappedRange( start, count ) );
        }

        return ticket;
    }
    //-----------------------------------------------------------------------------------
    void * MetalBuffer::map( NCount start, NCount count, size_t &outTicket )
    {
        N_assert( start <= mVboSize && start + count <= mVboSize );

        if( mMappedRanges.size() == mFreeRanges.size() )
            mMappedPtr = [mVboName contents];

        outTicket = addMappedRange( start, count );

        return static_cast<uint8*>(mMappedPtr) + start;
    }
    //-----------------------------------------------------------------------------------
    void MetalBuffer::flush( size_t ticket, NCount start, NCount count )
    {
        N_assert( start <= mMappedRanges[ticket].count && start + count <= mMappedRanges[ticket].count );
//#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//        NSRange range = NSMakeRange( mMappedRanges[ticket].start + start, count );
//        [mVboName didModifyRange:range];
//#endif
    }
    //-----------------------------------------------------------------------------------
    void MetalBuffer::unmap( size_t ticket )
    {
        N_assert( ticket < mMappedRanges.size() && "Invalid unmap ticket!" );
        N_assert( mMappedRanges.size() != mFreeRanges.size() &&
                "Unmapping an already unmapped buffer! Did you call unmap with the same ticket twice?" );

        mFreeRanges.push_back( ticket );

        if( mMappedRanges.size() == mFreeRanges.size() )
        {
            mMappedPtr = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalBufferOp
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalBufferOp::MetalBufferOp( size_t vboPoolIdx, id<MTLBuffer> vboName,
                                                MetalBuffer *dynamicBuffer ) :
        mGroupIndex( vboPoolIdx ),
        mVboName( vboName ),
        mMappedPtr( 0 ),
        mUnmapTicket( (size_t)~0 ),
        mDynamicBuffer( dynamicBuffer )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalBufferOp::~MetalBufferOp()
    {
    }
    //-----------------------------------------------------------------------------------
    void MetalBufferOp::_init( const void *data, size_t oft, size_t count )
    {
        //In OpenGL; immutable buffers are a charade. They're mostly there to satisfy D3D11's needs.
        //However we emulate the behavior and trying to upload to an immutable buffer will result
        //in an exception or an N_assert, thus we temporarily change the type.
        BufferModeMark otype = mBuffer->mMark;
        if( N_MarkOnlyTrue(mBuffer->mMark, M_USAGEMARK, M_DEVREAD)）
            mBuffer->mMark = M_DEVREAD | M_DEVWRITE;

        upload( data, oft, count );

        mBuffer->mBufferType = otype;
    }
    //-----------------------------------------------------------------------------------
    void * MetalBufferOp::lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx)
    {
        //size_t bytesPerElement = mBuffer->mBytesPerElement;

        MetalGBufferManager * vaoManager = static_cast<MetalGBufferManager*>( mBuffer->mMag );

        //size_t dynamicCurrentFrame = advanceFrame( bAdvanceFrame );

        if( !mBuffer->mMapping )
        {
            //Non-persistent buffers just map the small region they'll need.
            size_t offset;
            size_t length;

            if( N_MarkTrue(mBuffer->mMark, M_PersistentMap) && N_MarkTrue(m, MM_Persistent) )
            {
                //Persistent buffers map the *whole* assigned buffer,
                //we later care for the offsets and lengths
                offset = mBuffer->mOffset;
                length = mBuffer->getReserveSize() * mBuffer->getMutilCount();
            }
            else
            {
                offset = mBuffer->mOffset + oft + mBuffer->getReserveSize() * multiIdx;
                length = size;
            }

            mMappedPtr = mDynamicBuffer->map( offset, length, mUnmapTicket );
        }

        //For regular maps, mMuteOft is 0. So that we can later flush correctly.
        mBuffer->mMuteSize = size;

        char * retVal = (char*)mMappedPtr;

        if( N_MarkTrue(mBuffer->mMark, M_PersistentMap) && N_MarkTrue(m, MM_Persistent) )
        {
            //For persistent maps, we've mapped the whole 3x size of the buffer. mMuteOft
            //points to the right offset so that we can later flush correctly.
            mBuffer->mMuteOft = oft + mBuffer->getReserveSize() * multiIdx;
            retVal += mBuffer->mMuteOft;
        }
        else
        {
            mBuffer->mMuteOft = 0;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalBufferOp::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
        N_assert( oft <= mBuffer->mMuteSize && "Flush starts after the end of the mapped region!" );
        N_assert( oft + size <= mBuffer->mMuteSize && "Flush region out of bounds!" );

        if( N_MarkFalse(mBuffer->mMark, M_CoherentMap) || type == UT_UnlockMap )
        {
            if( !size )
                size = mBuffer->mMuteSize - oft;

            mDynamicBuffer->flush( mUnmapTicket, mBuffer->mMuteOft + oft, size );

            if( type == UT_UnlockMap || N_MarkOnlyTrue(mBuffer->mMark, M_USAGEMARK, M_NORMAL) )
            {
                mDynamicBuffer->unmap( mUnmapTicket );
                mMappedPtr = 0;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    /*size_t MetalBufferOp::advanceFrame( bool bAdvanceFrame )
    {
        MetalGBufferManager *vaoManager = static_cast<MetalGBufferManager*>( mBuffer->mMag );
        size_t dynamicCurrentFrame = mBuffer->mFinalBufferStart - mBuffer->mOffset;
        dynamicCurrentFrame /= mBuffer->_getInternalNumElements();

        dynamicCurrentFrame = (dynamicCurrentFrame + 1) % vaoManager->getMutilCount();

        if( bAdvanceFrame )
        {
            mBuffer->mFinalBufferStart = mBuffer->mOffset +
                                            dynamicCurrentFrame * mBuffer->_getInternalNumElements();
        }

        return dynamicCurrentFrame;
    }
    //-----------------------------------------------------------------------------------
    void MetalBufferOp::regressFrame(void)
    {
        MetalGBufferManager *vaoManager = static_cast<MetalGBufferManager*>( mBuffer->mMag );
        size_t dynamicCurrentFrame = mBuffer->mFinalBufferStart - mBuffer->mOffset;
        dynamicCurrentFrame /= mBuffer->_getInternalNumElements();

        dynamicCurrentFrame = (dynamicCurrentFrame + vaoManager->getMutilCount() - 1) %
                                vaoManager->getMutilCount();

        mBuffer->mFinalBufferStart = mBuffer->mOffset +
                                        dynamicCurrentFrame * mBuffer->_getInternalNumElements();
    }*/
    //-----------------------------------------------------------------------------------
    void MetalBufferOp::memcpyImpl( BufferInterface * src, size_t srcOft, size_t oft,
                                       size_t srcOft, size_t size )
    {
        MetalGBufferManager *vaoManager = static_cast<MetalGBufferManager*>( mBuffer->mMag );
        MetalDevice *device = vaoManager->getDevice();

        N_assert( dynamic_cast<MetalBufferOp*>( src->mOp ) );
        MetalBufferOp *srcbuf = static_cast<MetalBufferOp*>( src->mOp );

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        if( oft % 4u || size % 4u || srcOft % 4u )
        {
            //macOS Mojave and earlier
            vaoManager->unalignedCopy( this->mVboName, oft, srcbuf->getVboName(), srcOft, size );
        }
        else
#endif
        {
            __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = device->getBlitEncoder();
            [blitEncoder copyFromBuffer:srcbuf->getVboName()
                           sourceOffset:srcOft
                               toBuffer:mVboName
                      destinationOffset:oft
                                   size:size];
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalBufferOp::waitImpl()
    {
        
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalVertexBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalVertexBuffer::MetalVertexBuffer( MetalBufferManagerBase *mgr, size_t vertexSize,
            size_t numVertices, Buffer::Usage usage, bool useShadowBuffer ) :
        VertexBuffer( mgr, vertexSize, numVertices, usage, false, useShadowBuffer ),
        mMetalHardwareBufferCommon( mSizeInBytes, usage, 16, mgr->_getDiscardBufferManager(),
                                    mgr->_getDiscardBufferManager()->getDevice() )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalVertexBuffer::~MetalVertexBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void* MetalVertexBuffer::lockImpl( size_t offset, size_t length, LockOptions options )
    {
        return mMetalHardwareBufferCommon.lockImpl( offset, length, options, mIsLocked );
    }
    //-----------------------------------------------------------------------------------
    void MetalVertexBuffer::unlockImpl(void)
    {
        mMetalHardwareBufferCommon.unlockImpl( mLockStart, mLockSize );
    }
    //-----------------------------------------------------------------------------------
    void MetalVertexBuffer::_notifyDeviceStalled(void)
    {
        mMetalHardwareBufferCommon._notifyDeviceStalled();
    }
    //-----------------------------------------------------------------------------------
    id<MTLBuffer> MetalVertexBuffer::getBufferName( size_t &outOffset )
    {
        return mMetalHardwareBufferCommon.getBufferName( outOffset );
    }
    //-----------------------------------------------------------------------------------
    id<MTLBuffer> MetalVertexBuffer::getBufferNameForGpuWrite(void)
    {
        return mMetalHardwareBufferCommon.getBufferNameForGpuWrite();
    }
    //-----------------------------------------------------------------------------------
    void MetalVertexBuffer::readData( size_t offset, size_t length, void* pDest )
    {
        if( mUseShadowBuffer )
        {
            void* srcData = mShadowBuffer->lock(offset, length, HBL_READ_ONLY);
            memcpy(pDest, srcData, length);
            mShadowBuffer->unlock();
        }
        else
        {
            mMetalHardwareBufferCommon.readData( offset, length, pDest );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalVertexBuffer::writeData( size_t offset, size_t length, const void* pSource, bool discardWholeBuffer )
    {
        // Update the shadow buffer
        if( mUseShadowBuffer )
        {
            void* destData = mShadowBuffer->lock( offset, length, discardWholeBuffer ? HBL_DISCARD : HBL_NORMAL );
            memcpy( destData, pSource, length );
            mShadowBuffer->unlock();
        }

        mMetalHardwareBufferCommon.writeData( offset, length, pSource,
                                              discardWholeBuffer ||
                                              (offset == 0 && length == mSizeInBytes) );
    }
    //-----------------------------------------------------------------------------------
    void MetalVertexBuffer::copyData( Buffer& srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer )
    {
        if( srcBuffer.isSystemMemory() )
        {
            Buffer::copyData( srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer );
        }
        else
        {
            MetalHardwareBufferCommon *metalBuffer = reinterpret_cast<MetalHardwareBufferCommon*>(
                        srcBuffer.getRenderSystemData() );
            mMetalHardwareBufferCommon.copyData( metalBuffer, srcOffset,
                                                 dstOffset, length,
                                                 discardWholeBuffer ||
                                                 (dstOffset == 0 && length == mSizeInBytes) );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalVertexBuffer::_updateFromShadow(void)
    {
        if( mUseShadowBuffer && mShadowUpdated && !mSuppressHardwareUpdate )
        {
            const void *srcData = mShadowBuffer->lock( mLockStart, mLockSize, HBL_READ_ONLY );

            const bool discardBuffer = mLockStart == 0 && mLockSize == mSizeInBytes;
            mMetalHardwareBufferCommon.writeData( mLockStart, mLockSize, srcData, discardBuffer );

            mShadowBuffer->unlock();
            mShadowUpdated = false;
        }
    }
    //-----------------------------------------------------------------------------------
    void * MetalVertexBuffer::getRenderSystemData(void)
    {
        return &mMetalHardwareBufferCommon;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalIndexBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalIndexBuffer::MetalIndexBuffer( MetalBufferManagerBase *mgr,
                                                        IndexType idxType,
                                                        size_t numIndexes,
                                                        Buffer::Usage usage,
                                                        bool useShadowBuffer ) :
        IndexBuffer( mgr, idxType, numIndexes, usage, false, useShadowBuffer ),
        mMetalHardwareBufferCommon( mSizeInBytes, usage, 4, mgr->_getDiscardBufferManager(),
                                    mgr->_getDiscardBufferManager()->getDevice() )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalIndexBuffer::~MetalIndexBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void* MetalIndexBuffer::lockImpl( size_t offset, size_t length, LockOptions options )
    {
        return mMetalHardwareBufferCommon.lockImpl( offset, length, options, mIsLocked );
    }
    //-----------------------------------------------------------------------------------
    void MetalIndexBuffer::unlockImpl(void)
    {
        mMetalHardwareBufferCommon.unlockImpl( mLockStart, mLockSize );
    }
    //-----------------------------------------------------------------------------------
    void MetalIndexBuffer::_notifyDeviceStalled(void)
    {
        mMetalHardwareBufferCommon._notifyDeviceStalled();
    }
    //-----------------------------------------------------------------------------------
    id<MTLBuffer> MetalIndexBuffer::getBufferName( size_t &outOffset )
    {
        return mMetalHardwareBufferCommon.getBufferName( outOffset );
    }
    //-----------------------------------------------------------------------------------
    id<MTLBuffer> MetalIndexBuffer::getBufferNameForGpuWrite(void)
    {
        return mMetalHardwareBufferCommon.getBufferNameForGpuWrite();
    }
    //-----------------------------------------------------------------------------------
    void MetalIndexBuffer::readData( size_t offset, size_t length, void* pDest )
    {
        if( mUseShadowBuffer )
        {
            void* srcData = mShadowBuffer->lock(offset, length, HBL_READ_ONLY);
            memcpy(pDest, srcData, length);
            mShadowBuffer->unlock();
        }
        else
        {
            mMetalHardwareBufferCommon.readData( offset, length, pDest );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalIndexBuffer::writeData( size_t offset, size_t length,
                                               const void* pSource, bool discardWholeBuffer )
    {
        // Update the shadow buffer
        if( mUseShadowBuffer )
        {
            void* destData = mShadowBuffer->lock( offset, length,
                                                  discardWholeBuffer ? HBL_DISCARD : HBL_NORMAL );
            memcpy( destData, pSource, length );
            mShadowBuffer->unlock();
        }

        mMetalHardwareBufferCommon.writeData( offset, length, pSource,
                                              discardWholeBuffer ||
                                              (offset == 0 && length == mSizeInBytes) );
    }
    //-----------------------------------------------------------------------------------
    void MetalIndexBuffer::copyData( Buffer& srcBuffer, size_t srcOffset,
                                              size_t dstOffset, size_t length, bool discardWholeBuffer )
    {
        if( srcBuffer.isSystemMemory() )
        {
            Buffer::copyData( srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer );
        }
        else
        {
            MetalHardwareBufferCommon *metalBuffer = reinterpret_cast<MetalHardwareBufferCommon*>(
                        srcBuffer.getRenderSystemData() );
            mMetalHardwareBufferCommon.copyData( metalBuffer, srcOffset,
                                                 dstOffset, length,
                                                 discardWholeBuffer ||
                                                 (dstOffset == 0 && length == mSizeInBytes) );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalIndexBuffer::_updateFromShadow(void)
    {
        if( mUseShadowBuffer && mShadowUpdated && !mSuppressHardwareUpdate )
        {
            const void *srcData = mShadowBuffer->lock( mLockStart, mLockSize, HBL_READ_ONLY );

            const bool discardBuffer = mLockStart == 0 && mLockSize == mSizeInBytes;
            mMetalHardwareBufferCommon.writeData( mLockStart, mLockSize, srcData, discardBuffer );

            mShadowBuffer->unlock();
            mShadowUpdated = false;
        }
    }
    //-----------------------------------------------------------------------------------
    void* MetalIndexBuffer::getRenderSystemData(void)
    {
        return &mMetalHardwareBufferCommon;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalTextureBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalTextureBuffer::MetalTextureBuffer(
                size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
                uint32 numElementsPadding, BufferType bufferType, void * initialData, bool keepAsShadow,
                GBufferManager * vaoManager, MetalBufferOp * op, PixelFormat pf, MetalDevice * device ) :
        TextureBuffer( internalBufStartBytes, numElements, bytesPerElement, numElementsPadding,
                         bufferType, initialData, keepAsShadow, vaoManager, op, pf ),
        mDevice( device )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalTextureBuffer::~MetalTextureBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureBuffer::bindBufferVS( uint16 slot, size_t offset, size_t sizeBytes )
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );
        N_assert( dynamic_cast<MetalBufferOp *>( mOp ) );
        MetalBufferOp * op = static_cast<MetalBufferOp *>( mOp );

        [mDevice->mRenderEncoder setVertexBuffer:op->getVboName()
                                          offset:mFinalBufferStart * mBytesPerElement + offset
                                         atIndex:slot + OGRE_METAL_TEX_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureBuffer::bindBufferPS( uint16 slot, size_t offset, size_t sizeBytes )
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );
        N_assert( dynamic_cast<MetalBufferOp *>( mOp ) );
        MetalBufferOp * op = static_cast<MetalBufferOp*>( mOp );

        [mDevice->mRenderEncoder setFragmentBuffer:op->getVboName()
                                            offset:mFinalBufferStart * mBytesPerElement + offset
                                           atIndex:slot + OGRE_METAL_TEX_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureBuffer::bindBufferCS( uint16 slot, size_t offset, size_t sizeBytes )
    {
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder =
                mDevice->getComputeEncoder();
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        [computeEncoder setBuffer:op->getVboName()
                           offset:mFinalBufferStart * mBytesPerElement + offset
                          atIndex:slot + OGRE_METAL_CS_TEX_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureBuffer::bindBufferForDescriptor( __unsafe_unretained id <MTLBuffer> *buffers,
                                                        NSUInteger *offsets, size_t offset )
    {
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        *buffers = op->getVboName();
        *offsets = mFinalBufferStart * mBytesPerElement + offset;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalStorageBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalStorageBuffer::MetalStorageBuffer(
                size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
                uint32 bindFlags, void * initialData, bool keepAsShadow,
                GBufferManager *vaoManager, MetalBufferOp *op,
                MetalDevice *device ) :
        StorageBuffer( internalBufStartBytes, numElements, bytesPerElement, bindFlags,
            initialData, keepAsShadow, vaoManager, op ),
        mDevice( device )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalStorageBuffer::~MetalStorageBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    TextureBuffer* MetalStorageBuffer::createViewImpl( PixelFormat pf )
    {
        N_assert( dynamic_cast<MetalBufferOp *>( mOp ) );

        MetalBufferOp * op = static_cast<MetalBufferOp*>( mOp );


        TextureBuffer * re = N_new MetalTextureBuffer(mInternalBufferStart * mBytesPerElement,
                                                        mNumElements, mBytesPerElement, 0,
                                                        mMark, (void*)0, false,
                                                        (GBufferManager*)0, op, pf,
                                                        mDevice );
        //We were overriden by the BufferPacked we just created. Restore this back!
        re->setOperation( op );

        mViewList.push_back( re );

        return re;
    }
    //-----------------------------------------------------------------------------------
    void MetalStorageBuffer::bindBufferAllRenderStages( uint16 slot, size_t offset )
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        [mDevice->mRenderEncoder setVertexBuffer:op->getVboName()
                                          offset:mFinalBufferStart * mBytesPerElement + offset
                                         atIndex:slot + OGRE_METAL_UAV_SLOT_START];
        [mDevice->mRenderEncoder setFragmentBuffer:op->getVboName()
                                            offset:mFinalBufferStart * mBytesPerElement + offset
                                           atIndex:slot + OGRE_METAL_UAV_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalStorageBuffer::bindBufferVS( uint16 slot, size_t offset, size_t sizeBytes )
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        [mDevice->mRenderEncoder setVertexBuffer:op->getVboName()
                                          offset:mFinalBufferStart * mBytesPerElement + offset
                                         atIndex:slot + OGRE_METAL_UAV_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalStorageBuffer::bindBufferPS( uint16 slot, size_t offset, size_t sizeBytes )
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        [mDevice->mRenderEncoder setFragmentBuffer:op->getVboName()
                                            offset:mFinalBufferStart * mBytesPerElement + offset
                                           atIndex:slot + OGRE_METAL_UAV_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalStorageBuffer::bindBufferCS( uint16 slot, size_t offset, size_t sizeBytes )
    {
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mDevice->getComputeEncoder();
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        [computeEncoder setBuffer:op->getVboName()
                           offset:mFinalBufferStart * mBytesPerElement + offset
                          atIndex:slot + OGRE_METAL_CS_UAV_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalStorageBuffer::bindBufferForDescriptor( __unsafe_unretained id <MTLBuffer> *buffers,
                                                        NSUInteger *offsets, size_t offset )
    {
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        *buffers = op->getVboName();
        *offsets = mFinalBufferStart * mBytesPerElement + offset;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalStructBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalStructBuffer::MetalStructBuffer(
                size_t internalBufferStartBytes, size_t numElements, uint32 bytesPerElement,
                uint32 numElementsPadding, BufferType bufferType, void *initialData, bool keepAsShadow,
                GBufferManager *vaoManager, BufferInterface *op,
                MetalDevice *device ) :
        StructBuffer( internalBufferStartBytes, numElements, bytesPerElement, numElementsPadding,
                           bufferType, initialData, keepAsShadow, vaoManager,
                           op ),
        mDevice( device )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalStructBuffer::~MetalStructBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void MetalStructBuffer::bindBufferVS( uint16 slot, uint32 offsetBytes )
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );
        MetalBufferOp *op = static_cast<MetalBufferOp*>( mOp );

        [mDevice->mRenderEncoder setVertexBuffer:op->getVboName()
                                          offset:mFinalBufferStart * mBytesPerElement + offsetBytes
                                         atIndex:slot + OGRE_METAL_CONST_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalStructBuffer::bindBufferPS( uint16 slot, uint32 offsetBytes )
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );
        MetalBufferOp * op = static_cast<MetalBufferOp*>( mOp );

        [mDevice->mRenderEncoder setFragmentBuffer:op->getVboName()
                                            offset:mFinalBufferStart * mBytesPerElement + offsetBytes
                                           atIndex:slot + OGRE_METAL_CONST_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalStructBuffer::bindBufferCS( uint16 slot, uint32 offsetBytes )
    {
        N_assert( dynamic_cast<MetalBufferOp*>( mOp ) );

        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mDevice->getComputeEncoder();
        MetalBufferOp * op = static_cast<MetalBufferOp*>( mOp );

        [computeEncoder setBuffer:op->getVboName()
                           offset:mFinalBufferStart * mBytesPerElement + offsetBytes
                          atIndex:slot + OGRE_METAL_CS_CONST_SLOT_START];
    }
    //-----------------------------------------------------------------------------------
    void MetalStructBuffer::bindBufferVS( uint16 slot )
    {
        bindBufferVS( slot, 0 );
    }
    //-----------------------------------------------------------------------------------
    void MetalStructBuffer::bindBufferPS( uint16 slot )
    {
        bindBufferPS( slot, 0 );
    }
    //-----------------------------------------------------------------------------------
    void MetalStructBuffer::bindBufferCS( uint16 slot )
    {
        bindBufferCS( slot, 0 );
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalMappedBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalMappedBuffer::MetalMappedBuffer( size_t internalBufferStart, size_t sizeBytes,
                                            GBufferManager *vaoManager, bool uploadOnly,
                                            id<MTLBuffer> vboName, MetalDevice *device ) :
        StagingBuffer( internalBufferStart, sizeBytes, vaoManager, uploadOnly ),
        mVboName( vboName ),
        mMappedPtr( 0 ),
        mDevice( device ),
        mFenceThreshold( sizeBytes / 4 ),
        mUnfencedBytes( 0 )
    {
    }
    //-----------------------------------------------------------------------------------
    MetalMappedBuffer::~MetalMappedBuffer()
    {
        if( !mFences.empty() )
            wait( mFences.back().fenceName );

        deleteFences( mFences.begin(), mFences.end() );
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::addFence( size_t from, size_t to, bool forceFence )
    {
        N_assert( to <= mSizeBytes );

        MetalFence unfencedHazard( from, to );

        mUnfencedHazards.push_back( unfencedHazard );

        N_assert( from <= to );

        mUnfencedBytes += to - from;

        if( mUnfencedBytes >= mFenceThreshold || forceFence )
        {
            MetalFenceVec::const_iterator itor = mUnfencedHazards.begin();
            MetalFenceVec::const_iterator end  = mUnfencedHazards.end();

            size_t startRange = itor->start;
            size_t endRange   = itor->end;

            while( itor != end )
            {
                if( endRange <= itor->end )
                {
                    //Keep growing (merging) the fences into one fence
                    endRange = itor->end;
                }
                else
                {
                    //We wrapped back to 0. Can't keep merging. Make a fence.
                    MetalFence fence( startRange, endRange );
                    fence.fenceName = dispatch_semaphore_create( 0 );

                    __block dispatch_semaphore_t blockSemaphore = fence.fenceName;
                    [mDevice->mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
                    {
                        dispatch_semaphore_signal( blockSemaphore );
                    }];
                    mFences.push_back( fence );

                    startRange  = itor->start;
                    endRange    = itor->end;
                }

                ++itor;
            }

            //Make the last fence.
            MetalFence fence( startRange, endRange );
            fence.fenceName = dispatch_semaphore_create( 0 );
            __block dispatch_semaphore_t blockSemaphore = fence.fenceName;
            [mDevice->mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
            {
                dispatch_semaphore_signal( blockSemaphore );
            }];

            //Flush the device for accuracy in the fences.
            mDevice->commitAndNextCommandBuffer();
            mFences.push_back( fence );

            mUnfencedHazards.clear();
            mUnfencedBytes = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::wait( dispatch_semaphore_t syncObj )
    {
        long result = dispatch_semaphore_wait( syncObj, DISPATCH_TIME_FOREVER );

        if( result != 0 )
        {
            N_EXCEPT( RenderingAPI, "Failure while waiting for a MetalFence. Could be out of GPU memory. "
                         "Update your video card drivers. If that doesn't help, "
                         "contact the developers. Error code: " + StrConv::conv( result ) );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::deleteFences( MetalFenceVec::iterator itor, MetalFenceVec::iterator end )
    {
        while( itor != end )
        {
            //ARC will destroy it
            itor->fenceName = 0;
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::waitIfNeeded(void)
    {
        //N_assert( mLastOp );

        size_t mappingStart = mMappingStart;
        size_t sizeBytes    = mMappingCount;

        if( mappingStart + sizeBytes > mSizeBytes )
        {
            if( !mUnfencedHazards.empty() )
            {
                //mUnfencedHazards will be cleared in addFence
                addFence( mUnfencedHazards.front().start, mSizeBytes - 1, true );
            }

            //Wraps around the ring buffer. Sadly we can't do advanced virtual memory
            //manipulation to keep the virtual space contiguous, so we'll have to reset to 0
            mappingStart = 0;
        }

        MetalFence regionToMap( mappingStart, mappingStart + sizeBytes );

        MetalFenceVec::iterator itor = mFences.begin();
        MetalFenceVec::iterator end  = mFences.end();

        MetalFenceVec::iterator lastWaitableFence = end;

        while( itor != end )
        {
            if( regionToMap.overlaps( *itor ) )
                lastWaitableFence = itor;

            ++itor;
        }

        if( lastWaitableFence != end )
        {
            wait( lastWaitableFence->fenceName );
            deleteFences( mFences.begin(), lastWaitableFence + 1 );
            mFences.erase( mFences.begin(), lastWaitableFence + 1 );
        }

        mMappingStart = mappingStart;
    }
    //-----------------------------------------------------------------------------------
    void* MetalMappedBuffer::writeMapImpl( size_t sizeBytes )
    {
        //N_assert( mLastOp );

        mMappingCount = multiCeil( sizeBytes, 4u );

        waitIfNeeded(); //Will fill mMappingStart

        mMappedPtr = reinterpret_cast<uint8*>( [mVboName contents] ) + mInternalBufferStart + mMappingStart;

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::unmapImpl()
    {
//        if( mLastOp )
//        {
//#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//            NSRange range = NSMakeRange( mInternalBufferStart + mMappingStart, mMappingCount );
//            [mVboName didModifyRange:range];
//#endif
//        }

        mMappedPtr = 0;
        
        if(mLastOp == BOT_Read)
        {
            recover(mMappingStart, mMappingCount );
        }
    }
    //-----------------------------------------------------------------------------------
    BlockType MetalMappedBuffer::request( size_t sizeBytes, bool upload)
    {
        if(upload)
        {
            //N_assert( mLastOp );

            size_t mappingStart = mMappingStart;

            BlockType retVal = STALL_NONE;

            if( mappingStart + sizeBytes > mSizeBytes )
            {
                if( !mUnfencedHazards.empty() )
                {
                    MetalFence regionToMap( 0, sizeBytes );
                    MetalFence hazardousRegion( mUnfencedHazards.front().start, mSizeBytes - 1 );

                    if( hazardousRegion.overlaps( regionToMap ) )
                    {
                        retVal = STALL_FULL;
                        return retVal;
                    }
                }

                mappingStart = 0;
            }

            MetalFence regionToMap( mappingStart, mappingStart + sizeBytes );

            MetalFenceVec::iterator itor = mFences.begin();
            MetalFenceVec::iterator end  = mFences.end();

            MetalFenceVec::iterator lastWaitableFence = end;

            while( itor != end )
            {
                if( regionToMap.overlaps( *itor ) )
                    lastWaitableFence = itor;

                ++itor;
            }

            if( lastWaitableFence != end )
            {
                //Ask Metal to return immediately and tells us about the fence
                long waitRet = dispatch_semaphore_wait( lastWaitableFence->fenceName, DISPATCH_TIME_NOW );

                if( waitRet != 0 )
                {
                    retVal = STALL_PARTIAL;
                }
                else
                {
                    deleteFences( mFences.begin(), lastWaitableFence + 1 );
                    mFences.erase( mFences.begin(), lastWaitableFence + 1 );
                }
            }

            return retVal;
        }
        else
        {
            return MappedBuffer::request(multiCeil( length, 4u ) , false);
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::cleanUnfencedHazards(void)
    {
        if( !mUnfencedHazards.empty() )
            addFence( mUnfencedHazards.front().start, mUnfencedHazards.back().end, true );
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::_notifyDeviceStalled(void)
    {
        deleteFences( mFences.begin(), mFences.end() );
        mFences.clear();
        mUnfencedHazards.clear();
        mUnfencedBytes = 0;
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::_unmapToV1( v1::MetalHardwareBufferCommon *hwBuffer,
                                         size_t lockStart, size_t lockSize )
    {
        //N_assert( mLastOp );

        if( mMappingState != true )
        {
            //This stuff would normally be done in StagingBuffer::unmap
            N_EXCEPT( Exception::ERR_INVALID_STATE, "Unmapping an unmapped buffer!" );
        }

//        if( mLastOp )
//        {
//#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//            NSRange range = NSMakeRange( mInternalBufferStart + mMappingStart, mMappingCount );
//            [mVboName didModifyRange:range];
//#endif
//        }

        mMappedPtr = 0;

        __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();
        [blitEncoder copyFromBuffer:mVboName
                                    sourceOffset:mInternalBufferStart + mMappingStart
                                    toBuffer:hwBuffer->getBufferNameForGpuWrite()
                                    destinationOffset:lockStart
                                    size:multiCeil( lockSize, 4u )];

        if( mLastOp == BOT_Write )
        {
            //Add fence to this region (or at least, track the hazard).
            addFence( mMappingStart, mMappingStart + mMappingCount - 1, false );
        }

        //This stuff would normally be done in StagingBuffer::unmap
        mMappingState = 0;
        mMappingStart += mMappingCount;

        if( mMappingStart >= mSizeBytes )
            mMappingStart = 0;
    }
    //-----------------------------------------------------------------------------------
    size_t MetalMappedBuffer::async( OperationList & oplist, bool upload)
    {
        if(upload)
        {
            OperationList::const_iterator i, iend = oplist.end();
            for( i = oplist.begin(); i != iend; ++i)
            {
                const Operation & dst = *i;
                const GpuBuffer * dest = dst.mDest;
                const size_t dstsize = multiCeil( dst.mSize, 4u );
                MetalBufferOp * op = static_cast<MetalBufferOp*>(dest->getBufferInterface() );

                N_assert( dest->mMark == M_DEVNORMAL );

                const size_t dstOffset = dst.mDestOffset + dest->_getInternalBufferStart() *
                                                                dest->getBytesPerElement();

                __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();
                [blitEncoder copyFromBuffer:mVboName
                                            sourceOffset:mInternalBufferStart + mMappingStart + dst.mOffset
                                            toBuffer:op->getVboName()
                                            destinationOffset:dstOffset
                                            size:dstsize];
                
                addFence( mLastUploadOffset + dst.mOffset, mLastUploadOffset + dstsize - 1, false );
            }
            mLastUploadOffset = mMappingStart;
            //Add fence to this region (or at least, track the hazard).
            //addFence( mMappingStart, mMappingStart + mMappingCount - 1, false );
        }
        else
        {
            OperationList::iterator i, iend = oplist.end();
            for( i = oplist.begin(); i != iend; ++i)
            {
                Operation & src = *i;
                GpuBuffer * source = dst.mDest;
                
                //Metal has alignment restrictions of 4 bytes for offset and size in copyFromBuffer
                size_t freeRegionOffset = getFreeDownloadRegion( multiCeil( src.mSize, 4u ) );

                if( freeRegionOffset == (size_t)(-1) )
                {
                    N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Cannot download the request amount of " +
                                 StrConv::conv( src.mSize ) + " bytes to this staging buffer. "
                                 "Try another one (we're full of requests that haven't been read by CPU yet)" );
                }

                //N_assert( !mLastOp );
                N_assert( dynamic_cast<MetalBufferOp*>( source->getBufferInterface() ) );
                N_assert( (src.mDestOffset + src.mSize) <= source->getTotalSizeBytes() );

                size_t extraOffset = src.mDestOffset & 0x03;
                if( extraOffset )
                {
                    //Not multiple of 4. Backtrack to make it multiple of 4, then add this value
                    //to the return value so it gets correctly mapped in _mapForRead.
                    //extraOffset = src.mDestOffset & 0x03;
                    src.mDestOffset -= extraOffset;
                }

                MetalBufferOp *op = static_cast<MetalBufferOp*>(
                                                                    source->getBufferInterface() );

                __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();
                [blitEncoder copyFromBuffer:op->getVboName()
                                            sourceOffset:source->_getFinalBufferStart() *
                                                source->getBytesPerElement() + src.mDestOffset
                                            toBuffer:mVboName
                                            destinationOffset:mInternalBufferStart + freeRegionOffset
                                            size:multiCeil( src.mSize, 4u )];
        #if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
                N_assert( mVboName.storageMode != MTLStorageModeManaged );
                //[blitEncoder synchronizeResource:mVboName];
        #endif

                src.mOffset = freeRegionOffset + extraOffset;
            }
        }
        
        return oplist.size();
    }
    //-----------------------------------------------------------------------------------
    void MetalMappedBuffer::recover( size_t offset, size_t sizeBytes )
    {
        //If offset isn't multiple of 4, we were making it go forward in
        //_asyncDownload. We need to backgrack it so regions stay contiguous.
        StagingBuffer::recover( offset & ~size_t(0x03), sizeBytes );
    }
    //-----------------------------------------------------------------------------------
    const void* MetalMappedBuffer::readMapImpl( size_t offset, size_t sizeBytes )
    {
        //N_assert( !mLastOp );

        mMappingStart = offset;
        mMappingCount = multiCeil( sizeBytes, 4u );

        mMappedPtr = reinterpret_cast<uint8*>( [mVboName contents] ) +
                mInternalBufferStart + mMappingStart;

        //Put the mapped region back to our records as "available" for subsequent _asyncDownload
        //recover( offset, sizeBytes );

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    size_t MetalMappedBuffer::_asyncDownloadV1( v1::MetalHardwareBufferCommon *source,
                                                 size_t srcOffset, size_t srcLength )
    {
        //Metal has alignment restrictions of 4 bytes for offset and size in copyFromBuffer
        size_t freeRegionOffset = getFreeDownloadRegion( multiCeil( srcLength, 4u ) );

        if( freeRegionOffset == (size_t)(-1) )
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Cannot download the request amount of " +
                         StrConv::conv( srcLength ) + " bytes to this staging buffer. "
                         "Try another one (we're full of requests that haven't been read by CPU yet)" );
        }

        //N_assert( !mLastOp );
        N_assert( (srcOffset + srcLength) <= source->getSizeBytes() );

        size_t extraOffset = srcOffset & 0x03;
        if( extraOffset )
        {
            //Not multiple of 4. Backtrack to make it multiple of 4, then add this value
            //to the return value so it gets correctly mapped in _mapForRead.
            //extraOffset = srcOffset & 0x03;
            srcOffset -= extraOffset;
        }

        size_t srcOffsetStart = 0;
        __unsafe_unretained id<MTLBuffer> srcBuffer = source->getBufferName( srcOffsetStart );

        __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();
        [blitEncoder copyFromBuffer:srcBuffer
                                    sourceOffset:srcOffset + srcOffsetStart
                                    toBuffer:mVboName
                                    destinationOffset:mInternalBufferStart + freeRegionOffset
                                    size:multiCeil( srcLength, 4u )];
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        N_assert( mVboName.storageMode != MTLStorageModeManaged );
        //[blitEncoder synchronizeResource:mVboName];
#endif

        return freeRegionOffset + extraOffset;
    }
    //-----------------------------------------------------------------------------------
}
