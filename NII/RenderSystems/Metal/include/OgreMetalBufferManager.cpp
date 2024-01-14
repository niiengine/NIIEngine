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
#include "OgreMetalHardwareIndexBuffer.h"
#include "OgreMetalHardwareVertexBuffer.h"
#include "OgreMetalDiscardBufferManager.h"
#include "OgreMetalRenderSystem.h"

#include "Vao/OgreGBufferManager.h"

#import <Metal/MTLDevice.h>
#import <Metal/MTLBuffer.h>
#import <Metal/MTLBlitCommandEncoder.h>

#include "Vao/OgreMetalStagingBuffer.h"
#include "Vao/OgreVertexArrayObject.h"
#include "Vao/OgreMetalBufferInterface.h"
#include "Vao/OgreMetalConstBufferPacked.h"
#include "Vao/OgreMetalTexBufferPacked.h"
#include "Vao/OgreMetalUavBufferPacked.h"
#include "Vao/OgreMetalMultiSourceVertexBufferPool.h"
#include "Vao/OgreMetalDynamicBuffer.h"
#include "Vao/OgreMetalAsyncTicket.h"

#include "Vao/OgreIndirectBufferPacked.h"

#include "OgreMetalDevice.h"
#include "OgreRenderQueue.h"
#include "OgreRoot.h"
#include "OgreHlmsManager.h"

#include "OgreTimer.h"
#include "OgreStringConverter.h"
#include "OgreLwString.h"

#import <Metal/MTLRenderCommandEncoder.h>
#import <Metal/MTLComputeCommandEncoder.h>
#import <Metal/MTLComputePipeline.h>

namespace NII 
{
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // MetalDiscardBufferManager
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    MetalDiscardBufferManager::MetalDiscardBufferManager( MetalDevice *device, GBufferManager *vaoManager ) :
        mBuffer( 0 ),
        mDevice( device ),
        mMag( vaoManager )
    {
        const size_t defaultCapacity = 4 * 1024 * 1024;

        MTLResourceOptions resourceOptions = MTLResourceCPUCacheModeWriteCombined |
                                             MTLResourceStorageModeShared;

        mBuffer = [mDevice->mDevice newBufferWithLength:defaultCapacity options:resourceOptions];
        if( !mBuffer )
        {
            N_EXCEPT( RenderingAPI, "Out of GPU memory or driver refused.\nRequested: " + StrConv::conv( defaultCapacity ) + " bytes.");
        }
        mBuffer.label = @"Discardable Buffer";
        mFreeBlocks.push_back( MetalGBufferManager::Block( 0, defaultCapacity ) );
    }
    //-------------------------------------------------------------------------
    MetalDiscardBufferManager::~MetalDiscardBufferManager()
    {
        MetalDiscardBufferVec::const_iterator itor = mDiscardBuffers.begin();
        MetalDiscardBufferVec::const_iterator end  = mDiscardBuffers.end();

        while( itor != end )
            N_delete *itor++;
        mDiscardBuffers.clear();

        mBuffer = 0;
    }
    //-------------------------------------------------------------------------
    void MetalDiscardBufferManager::growToFit( size_t extraBytes, MetalDiscardBuffer *forDiscardBuffer )
    {
        N_assert( !(extraBytes & 0x04) && "extraBytes must be multiple of 4!" );

        const size_t oldCapacity = [mBuffer length];
        const size_t newCapacity = std::max( oldCapacity + extraBytes,
                                             oldCapacity + (oldCapacity >> 1u) + 1u );

        MTLResourceOptions resourceOptions = MTLResourceCPUCacheModeWriteCombined |
                                             MTLResourceStorageModeShared;
        id<MTLBuffer> oldBuffer = mBuffer;
        mBuffer = [mDevice->mDevice newBufferWithLength:newCapacity options:resourceOptions];
        if( !mBuffer )
        {
            N_EXCEPT( RenderingAPI, "Out of GPU memory or driver refused.\nRequested: " + StrConv::conv( newCapacity ) + " bytes.");
        }
        mBuffer.label = @"Discardable Buffer";

        //TODO DEBUG: We need to check if we need to retain oldBuffer or Metal does that.
        NSLog( @"DEBUG Pre: Retain count is %ld", CFGetRetainCount( (__bridge CFTypeRef)oldBuffer ) );

        {
            //Update our buffers so they point to the new buffer, copy their blocks in use from old
            //MTLBuffer to new one, and tag all of them as in use by GPU (due to the copyFromBuffer);
            //except 'forDiscardBuffer' which we were told this data won't be used.
            __unsafe_unretained id<MTLBlitCommandEncoder> blitEncoder = mDevice->getBlitEncoder();

            const uint32 currentFrame = mMag->getFrameCount();
            MetalDiscardBufferVec::iterator itor = mDiscardBuffers.begin();
            MetalDiscardBufferVec::iterator end  = mDiscardBuffers.end();

            while( itor != end )
            {
                if( *itor != forDiscardBuffer )
                {
                    (*itor)->mBuffer = mBuffer;
                    [blitEncoder copyFromBuffer:oldBuffer
                                   sourceOffset:(*itor)->getBlockStart()
                                       toBuffer:mBuffer
                              destinationOffset:(*itor)->getBlockStart()
                                           size:(*itor)->getBlockSize()];
                    (*itor)->mLastFrame = currentFrame;
                }
                else
                {
                    (*itor)->mLastFrame = currentFrame - mMag->getMutilCount();
                }

                ++itor;
            }
        }

        NSLog( @"DEBUG Pos: Retain count is %ld", CFGetRetainCount( (__bridge CFTypeRef)oldBuffer ) );

        LogManager::getSingleton().logMessage(
                    "PERFORMANCE WARNING: MetalDiscardBufferManager::growToFit must stall."
                    "Consider increasing the default discard capacity to at least " +
                    StrConv::conv( newCapacity ) + " bytes" );

        //According to Metal docs, it's undefined behavior if both CPU & GPU
        //write to the same resource even if the regions don't overlap.
        mDevice->stall();

        NSLog( @"DEBUG Pos2: Retain count is %ld", CFGetRetainCount( (__bridge CFTypeRef)oldBuffer ) );

        mFreeBlocks.push_back( MetalGBufferManager::Block( oldCapacity, newCapacity - oldCapacity ) );

        {
            //All "unsafe" blocks are no longer unsafe, since we're using a new buffer.
            UnsafeBlockVec::const_iterator itor = mUnsafeBlocks.begin();
            UnsafeBlockVec::const_iterator end  = mUnsafeBlocks.end();

            while( itor != end )
            {
                mFreeBlocks.push_back( *itor );
                MetalGBufferManager::mergeContiguousBlocks( mFreeBlocks.end() - 1, mFreeBlocks );
                ++itor;
            }

            mUnsafeBlocks.clear();
        }
    }
    //-------------------------------------------------------------------------
    void MetalDiscardBufferManager::updateUnsafeBlocks(void)
    {
        const uint32 currentFrame = mMag->getFrameCount();
        const uint32 bufferMultiplier = mMag->getMutilCount();

        UnsafeBlockVec::iterator itor = mUnsafeBlocks.begin();
        UnsafeBlockVec::iterator end  = mUnsafeBlocks.end();

        while( itor != end && (currentFrame - itor->lastFrameUsed) >= bufferMultiplier )
        {
            //This block is safe now to put back into free blocks.
            mFreeBlocks.push_back( *itor );
            MetalGBufferManager::mergeContiguousBlocks( mFreeBlocks.end() - 1, mFreeBlocks );
            ++itor;
        }

        mUnsafeBlocks.erase( mUnsafeBlocks.begin(), itor );
    }
    //-------------------------------------------------------------------------
    void MetalDiscardBufferManager::_notifyDeviceStalled(void)
    {
        {
            UnsafeBlockVec::iterator itor = mUnsafeBlocks.begin();
            UnsafeBlockVec::iterator end  = mUnsafeBlocks.end();

            while( itor != end )
            {
                //This block is safe now to put back into free blocks.
                mFreeBlocks.push_back( *itor );
                MetalGBufferManager::mergeContiguousBlocks( mFreeBlocks.end() - 1, mFreeBlocks );
                ++itor;
            }

            mUnsafeBlocks.clear();
        }

        {
            const uint32 currentFrame       = mMag->getFrameCount();
            const uint32 bufferMultiplier   = mMag->getMutilCount();

            MetalDiscardBufferVec::const_iterator itor = mDiscardBuffers.begin();
            MetalDiscardBufferVec::const_iterator end  = mDiscardBuffers.end();

            while( itor != end )
            {
                (*itor)->mLastFrame = currentFrame - bufferMultiplier;
                ++itor;
            }
        }
    }
    //-------------------------------------------------------------------------
    void MetalDiscardBufferManager::_getBlock( MetalDiscardBuffer *discardBuffer )
    {
        const size_t alignment = discardBuffer->getAlignment();
        const size_t sizeBytes = discardBuffer->getSizeBytes();

        if( discardBuffer->mBuffer )
        {
            if( mMag->getFrameCount() - discardBuffer->mLastFrame >=
                mMag->getMutilCount() )
            {
                return; //Current block the buffer owns is safe to reuse.
            }
            else
            {
                //Release the block back into the pool (sorted by mLastFrame)
                UnsafeBlock unsafeBlock( discardBuffer->getBlockStart(),
                                         discardBuffer->getBlockSize(),
                                         discardBuffer->mLastFrame );
                UnsafeBlockVec::iterator it = std::lower_bound( mUnsafeBlocks.begin(),
                                                                mUnsafeBlocks.end(),
                                                                unsafeBlock );
                mUnsafeBlocks.insert( it, unsafeBlock );
            }
        }

        updateUnsafeBlocks();

        //Find smallest block.
        MetalGBufferManager::BlockVec::iterator itor = mFreeBlocks.begin();
        MetalGBufferManager::BlockVec::iterator end  = mFreeBlocks.end();

        MetalGBufferManager::BlockVec::iterator smallestBlock = end;

        while( itor != end )
        {
            const size_t alignedOffset = std::min( itor->offset + itor->size,
                                                   multiCeil( itor->offset, alignment ) );

            if( sizeBytes <= itor->size - (alignedOffset - itor->offset) )
            {
                //We can use 'itor' block. Now check if it's smaller.
                if( smallestBlock == end || itor->size < smallestBlock->size )
                {
                    smallestBlock = itor;
                }
            }

            ++itor;
        }

        if( smallestBlock == end )
        {
            //No block can satisfy us. Resize (slow!)
            growToFit( sizeBytes, discardBuffer );

            //Recursive call: Try again. This time it will work since we have the space.
            discardBuffer->mBuffer = 0;
            _getBlock( discardBuffer );
            discardBuffer->mBuffer = mBuffer;
        }
        else
        {
            //Assign this block and shrink our records.
            discardBuffer->mBufferOffset        = multiCeil( smallestBlock->offset, alignment );
            discardBuffer->mBlockPrePadding     = discardBuffer->mBufferOffset - smallestBlock->offset;

            const size_t shrunkBytes = discardBuffer->getBlockSize();

            smallestBlock->offset   = discardBuffer->mBufferOffset + discardBuffer->mBufferSize;
            smallestBlock->size     -= shrunkBytes;

            if( smallestBlock->size == 0 )
            {
                mFreeBlocks.erase( smallestBlock );
            }
        }
    }
    //-------------------------------------------------------------------------
    MetalDiscardBuffer* MetalDiscardBufferManager::createDiscardBuffer( size_t bufferSize, uint16 alignment )
    {
        alignment = std::max<uint16>( 4u, alignment ); //Prevent alignments lower than 4 bytes.
        MetalDiscardBuffer *retVal = N_new MetalDiscardBuffer( bufferSize, alignment,
                                                                  mMag, this );
        mDiscardBuffers.push_back( retVal );
        _getBlock( retVal );
        retVal->mBuffer = mBuffer;

        return retVal;
    }
    //-------------------------------------------------------------------------
    void MetalDiscardBufferManager::destroyDiscardBuffer( MetalDiscardBuffer *discardBuffer )
    {
        MetalDiscardBufferVec::iterator itor = std::find( mDiscardBuffers.begin(), mDiscardBuffers.end(), discardBuffer );

        if( itor != mDiscardBuffers.end() )
        {
            N_assert( discardBuffer->mOwner == this &&
                    "Manager says it owns the discard buffer, but discard buffer says it doesn't" );

            //Release the block back into the pool (sorted by mLastFrame)
            UnsafeBlock unsafeBlock( discardBuffer->getBlockStart(),
                                     discardBuffer->getBlockSize(),
                                     discardBuffer->mLastFrame );
            UnsafeBlockVec::iterator it = std::lower_bound( mUnsafeBlocks.begin(),
                                                            mUnsafeBlocks.end(),
                                                            unsafeBlock );
            mUnsafeBlocks.insert( it, unsafeBlock );

            remove( mDiscardBuffers, itor );
            N_delete discardBuffer;
        }
        else
        {
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "discardBuffer doesn't belong to this MetalDiscardBufferManager or was already freed" );
        }
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // MetalDiscardBuffer
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    MetalDiscardBuffer::MetalDiscardBuffer( size_t bufferSize, uint16 alignment,
                                            GBufferManager *vaoManager,
                                            MetalDiscardBufferManager *owner ) :
        mBuffer( 0 ),
        mBlockPrePadding( 0 ),
        mBufferOffset( 0 ),
        mBufferSize( bufferSize ),
        mAlignment( alignment ),
        mLastFrame( vaoManager->getFrameCount() - vaoManager->getMutilCount() ),
        mMag( vaoManager ),
        mOwner( owner )
    {
        N_assert( alignment >= 4u );
    }
    //-------------------------------------------------------------------------
    void* MetalDiscardBuffer::map( bool noOverwrite )
    {
        if( !noOverwrite )
            mOwner->_getBlock( this );
        return reinterpret_cast<uint8*>( [mBuffer contents] ) + mBufferOffset;
    }
    //-------------------------------------------------------------------------
    void MetalDiscardBuffer::unmap(void)
    {
//#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
//        NSRange range = NSMakeRange( mBufferOffset, mBufferSize );
//        [mBuffer didModifyRange:range];
//#endif
    }
    //-------------------------------------------------------------------------
    id<MTLBuffer> MetalDiscardBuffer::getBufferName( size_t &outOffset )
    {
        mLastFrame = mMag->getFrameCount();
        outOffset = mBufferOffset;
        return mBuffer;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // MetalBufferManagerBase
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalBufferManagerBase::MetalBufferManagerBase( MetalDevice *device, GBufferManager *vaoManager ) :
        mDiscardBufferManager( 0 )
    {
        mDiscardBufferManager = N_new MetalDiscardBufferManager( device, vaoManager );
    }
    //-----------------------------------------------------------------------------------
    MetalBufferManagerBase::~MetalBufferManagerBase()
    {
        destroyAllDeclarations();
        destroyAllBindings();

        N_delete mDiscardBufferManager;
        mDiscardBufferManager = 0;
    }
    //-----------------------------------------------------------------------------------
    void MetalBufferManagerBase::_notifyDeviceStalled(void)
    {
        {
            OGRE_LOCK_MUTEX(mVertexBuffersMutex);
            VertexBufferList::const_iterator itor = mVertexBuffers.begin();
            VertexBufferList::const_iterator end  = mVertexBuffers.end();

            while( itor != end )
            {
                MetalVertexBuffer *hwBuffer = static_cast<MetalVertexBuffer*>( *itor );
                hwBuffer->_notifyDeviceStalled();
                ++itor;
            }
        }
        {
            OGRE_LOCK_MUTEX(mIndexBuffersMutex);
            IndexBufferList::const_iterator itor = mIndexBuffers.begin();
            IndexBufferList::const_iterator end  = mIndexBuffers.end();

            while( itor != end )
            {
                MetalIndexBuffer *hwBuffer = static_cast<MetalIndexBuffer*>( *itor );
                hwBuffer->_notifyDeviceStalled();
                ++itor;
            }
        }

        mDiscardBufferManager->_notifyDeviceStalled();
    }
    //-----------------------------------------------------------------------------------
    HardwareVertexBufferSharedPtr MetalBufferManagerBase::createVertexBuffer( size_t vertexSize, size_t numVerts, Buffer::Usage usage, bool useShadowBuffer )
    {
        MetalVertexBuffer * buf = N_new MetalVertexBuffer( this, vertexSize, numVerts, usage, useShadowBuffer );
        {
            OGRE_LOCK_MUTEX(mVertexBuffersMutex);
            mVertexBuffers.insert(buf);
        }
        return HardwareVertexBufferSharedPtr(buf);
    }
    //-----------------------------------------------------------------------------------
    HardwareIndexBufferSharedPtr MetalBufferManagerBase::createIndexBuffer( IndexBuffer::IndexType itype, size_t numIndexes, Buffer::Usage usage, bool useShadowBuffer )
    {
        MetalIndexBuffer * buf = N_new MetalIndexBuffer( this, itype, numIndexes, usage, useShadowBuffer );
        {
            OGRE_LOCK_MUTEX(mIndexBuffersMutex);
            mIndexBuffers.insert(buf);
        }
        return HardwareIndexBufferSharedPtr(buf);
    }
    //-----------------------------------------------------------------------------------
    StructBuffer * MetalBufferManagerBase::createUniformBuffer( size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name )
    {
        N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Use v2 interfaces." );
        return StructBuffer *();
    }
    //-----------------------------------------------------------------------------------
    CounterBuffer * MetalBufferManagerBase::createCounterBuffer( size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name )
    {
        N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Use v2 interfaces." );
    }
    //-----------------------------------------------------------------------------------
    const uint32 MetalGBufferManager::VERTEX_ATTRIBUTE_INDEX[VES_COUNT] =
    {
        0,  // VT_Pos - 1
        3,  // VT_Matrix_Weight - 1
        4,  // VT_Matrix_Index - 1
        1,  // VT_Normals - 1
        5,  // VT_Diffuse - 1
        6,  // VT_Specular - 1
        7,  // VT_Tex_Coord - 1
        //There are up to 8 VT_Tex_Coord. Occupy range [7; 15)
        //Range [13; 15) overlaps with VT_Custom7 & VT_Custom8
        //Index 15 is reserved for draw ID.

        //VT_Binormal would use slot 16. Since Binormal is rarely used, we don't support it.
        //(slot 16 is where const buffers start)
        ~0u,// VT_Binormal - 1
        2,  // VT_Tangent - 1
        13, // VT_Custom7 - 1
        14, // VT_Custom8 - 1
    };
    //-----------------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    //On iOS 16 byte alignment makes it good to go for everything; but we need to satisfy
    //both original alignment for baseVertex reasons. So find LCM between both.
    const uint32 c_minimumAlignment = 16u;
#else
    //On OSX we have several alignments (just like GL & D3D11), but it can never be lower than 4.
    const uint32 c_minimumAlignment = 4u;
#endif
    const uint32 c_indexBufferAlignment = 4u;

    static const char *c_vboTypes[] =
    {
        "CPU_INACCESSIBLE",
        "CPU_ACCESSIBLE_DEFAULT",
        "CPU_ACCESSIBLE_PERSISTENT",
        "CPU_ACCESSIBLE_PERSISTENT_COHERENT",
    };

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
    static const char c_gpuMemcpyComputeShader[] =
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "\n"
            "struct Params\n"
            "{\n"
            "	uint32_t dstOffset;\n"
            "	uint32_t srcOffset;\n"
            "	uint32_t sizeBytes;\n"
            "};\n"
            "\n"
            "kernel void ogre_gpu_memcpy\n"
            "(\n"
            "	device uint8_t* dst			[[ buffer(0) ]],\n"
            "	device uint8_t* src			[[ buffer(1) ]],\n"
            "	constant Params &p			[[ buffer(2) ]],\n"
            "	uint3 gl_GlobalInvocationID	[[thread_position_in_grid]]\n"
            ")\n"
            "{\n"
            "//	for( uint32_t i=0u; i<p.sizeBytes; ++i )\n"
            "//		dst[i + p.dstOffset] = src[i + p.srcOffset];\n"
            "	uint32_t srcOffsetStart	= p.srcOffset + gl_GlobalInvocationID.x * 64u;\n"
            "	uint32_t dstOffsetStart	= p.dstOffset + gl_GlobalInvocationID.x * 64u;\n"
            "	uint32_t numBytesToCopy	= min( 64u, p.sizeBytes - gl_GlobalInvocationID.x * 64u );\n"
            "	for( uint32_t i=0u; i<numBytesToCopy; ++i )\n"
            "		dst[i + dstOffsetStart] = src[i + srcOffsetStart];\n"
            "}";
#endif

    MetalGBufferManager::MetalGBufferManager( uint8 dynamicBufferMultiplier, MetalDevice *device, const PropertyData *params ) :
        GBufferManager( params ),
        mVaoNames( 1 ),
        mSemaphoreFlushed( true ),
        mDevice( device ),
        mDrawId( 0 )
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        //On iOS alignment must match "the maximum accessed object" type. e.g.
        //if it's all float, then alignment = 4. if it's a float2, then alignment = 8.
        //The max. object is float4, so alignment = 16
        mAlignment[GBT_Struct]   = 16;
        mAlignment[GBT_Texture]     = 16;

        //Keep pools of 16MB for static buffers
        mDefaultPoolSize[CPU_INACCESSIBLE]  = 16 * 1024 * 1024;

        //Keep pools of 4MB each for dynamic buffers
        for( size_t i=CPU_ACCESSIBLE_DEFAULT; i<=CPU_ACCESSIBLE_PERSISTENT_COHERENT; ++i )
            mDefaultPoolSize[i] = 4 * 1024 * 1024;

#else
        //OS X restrictions.
        mAlignment[GBT_Struct]   = 256;
        mAlignment[GBT_Texture]     = 256;

        //Keep pools of 32MB for static buffers
        mDefaultPoolSize[CPU_INACCESSIBLE]  = 32 * 1024 * 1024;

        //Keep pools of 4MB each for dynamic buffers
        for( size_t i=CPU_ACCESSIBLE_DEFAULT; i<=CPU_ACCESSIBLE_PERSISTENT_COHERENT; ++i )
            mDefaultPoolSize[i] = 4 * 1024 * 1024;
#endif
        if( params )
        {
            for( size_t i=0; i<VF_MAX; ++i )
            {
                PropertyData::const_iterator itor =
                        params->find( String( "GBufferManager::" ) + c_vboTypes[i] );
                if( itor != params->end() )
                {
                    mDefaultPoolSize[i] = StringConverter::parseUnsignedInt( itor->second,
                                                                             mDefaultPoolSize[i] );
                }
            }
        }

        mAlreadyWaitedForSemaphore.resize( mDynamicBufferMultiplier, true );
        mFrameSyncVec.resize( mDynamicBufferMultiplier, 0 );
        for( size_t i=0; i<mDynamicBufferMultiplier; ++i )
            mFrameSyncVec[i] = dispatch_semaphore_create( 0 );

        mMaxSize[GBT_Struct] = 64 * 1024;        //64kb
        mMaxSize[GBT_Texture]   = 128 * 1024 * 1024;//128MB

        mDynamicBufferMultiplier = dynamicBufferMultiplier;

        const uint32 maxNumInstances = 4096u * 2u;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        uint32 *drawIdPtr = static_cast<uint32*>( OGRE_MALLOC_SIMD( maxNumInstances * sizeof(uint32),
                                                                    MEMCATEGORY_GEOMETRY ) );
        for( uint32 i=0; i<maxNumInstances; ++i )
            drawIdPtr[i] = i;
        mDrawId = createConstBuffer( maxNumInstances * sizeof(uint32), M_DEVREAD, drawIdPtr, false );
        OGRE_FREE_SIMD( drawIdPtr, MEMCATEGORY_GEOMETRY );
        drawIdPtr = 0;
#else
        VertexUnitList units;
        units.push_back( VertexUnit( DT_UInt1x, VES_COUNT ) );
        uint32 *drawIdPtr = static_cast<uint32*>( OGRE_MALLOC_SIMD( maxNumInstances * sizeof(uint32),
                                                                    MEMCATEGORY_GEOMETRY ) );
        for( uint32 i=0; i<maxNumInstances; ++i )
            drawIdPtr[i] = i;
        mDrawId = createVertexBuffer( units, maxNumInstances, M_DEVREAD, drawIdPtr, true );

        createUnalignedCopyShader();
#endif
    }
    //-----------------------------------------------------------------------------------
    MetalGBufferManager::~MetalGBufferManager()
    {
        for( size_t i=0; i<VF_MAX; ++i )
        {
            VboVec::iterator itor = mVbos[i].begin();
            VboVec::iterator end  = mVbos[i].end();

            while( itor != end )
            {
                itor->vboName = 0;
                delete itor->mBuffer;
                itor->mBuffer = 0;
                ++itor;
            }
        }
    }
    //-----------------------------------------------------------------------------------
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
    void MetalGBufferManager::createUnalignedCopyShader(void)
    {
        NSError *error;
        id <MTLLibrary> library =
                [mDevice->mDevice newLibraryWithSource:
                [NSString stringWithUTF8String:c_gpuMemcpyComputeShader]
                                               options:nil
                                                 error:&error];

        if( !library )
        {
            String errorDesc;
            if( error )
                errorDesc = [error localizedDescription].UTF8String;

            N_EXCEPT( RenderingAPI,
                         "Metal SL Compiler Error while compiling internal c_gpuMemcpyComputeShader:\n" + errorDesc );
        }
        else
        {
            if( error )
            {
                String errorDesc;
                if( error )
                    errorDesc = [error localizedDescription].UTF8String;
                LogManager::getSingleton().logMessage(
                            "Metal SL Compiler Warnings in c_gpuMemcpyComputeShader:\n" + errorDesc );
            }
        }
        library.label = @"c_gpuMemcpyComputeShader";
        id<MTLFunction> unalignedCopyFunc = [library newFunctionWithName:@"ogre_gpu_memcpy"];
        if( !unalignedCopyFunc )
        {
            N_EXCEPT( RenderingAPI,
                         "Error retriving entry point from internal 'ogre_gpu_memcpy'" );
        }

        MTLComputePipelineDescriptor *psd = [[MTLComputePipelineDescriptor alloc] init];
        psd.computeFunction = unalignedCopyFunc;
        mUnalignedCopyPso =
                [mDevice->mDevice newComputePipelineStateWithDescriptor:psd
                                                                options:MTLPipelineOptionNone
                                                             reflection:nil
                                                                  error:&error];
        if( !mUnalignedCopyPso || error )
        {
            String errorDesc;
            if( error )
                errorDesc = [error localizedDescription].UTF8String;

            N_EXCEPT( RenderingAPI,
                         "Failed to create pipeline state for compute for mUnalignedCopyPso, error " + errorDesc );
        }
    }
#endif
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::getMemoryStats( const Block &block, size_t vboIdx, size_t poolCapacity,
                                          LwString &text, MemoryStatsList &outStats,
                                          Log *log ) const
    {
        if( log )
        {
            text.clear();
            text.a( c_vboTypes[vboIdx], ";",
                    (uint64)block.offset, ";",
                    (uint64)block.size, ";",
                    (uint64)poolCapacity );
            log->logMessage( text.c_str(), LML_CRITICAL );
        }

        MemoryStatsEntry entry( (uint32)vboIdx, block.offset, block.size, poolCapacity );
        outStats.push_back( entry );
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::getMemoryStats( MemoryStatsList &outStats, size_t &outCapacityBytes,
                                          size_t &outFreeBytes, Log *log ) const
    {
        size_t capacityBytes = 0;
        size_t freeBytes = 0;
        MemoryStatsList statsVec;
        statsVec.swap( outStats );

        vector<char>::type tmpBuffer;
        tmpBuffer.resize( 512 * 1024 ); //512kb per line should be way more than enough
        LwString text( LwString::FromEmptyPointer( &tmpBuffer[0], tmpBuffer.size() ) );

        if( log )
            log->logMessage( "Pool Type;Offset;Size Bytes;Pool Capacity", LML_CRITICAL );

        for( int vboIdx=0; vboIdx<VF_MAX; ++vboIdx )
        {
            VboVec::const_iterator itor = mVbos[vboIdx].begin();
            VboVec::const_iterator end  = mVbos[vboIdx].end();

            while( itor != end )
            {
                const Vbo &vbo = *itor;
                capacityBytes += vbo.sizeBytes;

                Block usedBlock( 0, 0 );

                BlockVec freeBlocks = vbo.mFreeList;
                while( !freeBlocks.empty() )
                {
                    //Find the free block that comes next
                    BlockVec::iterator nextBlock;
                    {
                        BlockVec::iterator itBlock = freeBlocks.begin();
                        BlockVec::iterator enBlock = freeBlocks.end();

                        nextBlock = itBlock;

                        while( itBlock != enBlock )
                        {
                            if( nextBlock->offset < itBlock->offset )
                                nextBlock = itBlock;
                            ++itBlock;
                        }
                    }

                    freeBytes += nextBlock->size;
                    usedBlock.size = nextBlock->offset;

                    //usedBlock.size could be 0 if:
                    //  1. All of memory is free
                    //  2. There's two contiguous free blocks, which should not happen
                    //     due to mergeContiguousBlocks
                    if( usedBlock.size > 0u )
                        getMemoryStats( usedBlock, vboIdx, vbo.sizeBytes, text, statsVec, log );

                    usedBlock.offset += usedBlock.size;
                    usedBlock.size = 0;
                    remove( freeBlocks, nextBlock );
                }

                if( usedBlock.size > 0u || (usedBlock.offset == 0 && usedBlock.size == 0) )
                    getMemoryStats( usedBlock, vboIdx, vbo.sizeBytes, text, statsVec, log );

                ++itor;
            }
        }

        outCapacityBytes = capacityBytes;
        outFreeBytes = freeBytes;
        statsVec.swap( outStats );
    }
    //-----------------------------------------------------------------------------------
    /*void MetalGBufferManager::switchVboPoolIndexImpl( size_t oldPoolIdx, size_t newPoolIdx, BufferPacked *buffer )
    {
        if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) || buffer->getBufferPackedType() != BP_TYPE_INDIRECT )
        {
            MetalBufferOp * op = static_cast<MetalBufferOp*>(buffer->getBufferInterface() );
            if( op->getVboPoolIndex() == oldPoolIdx )
                op->_setVboPoolIndex( newPoolIdx );
        }
    }*/
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::recoverPool(void)
    {
        for( int vboIdx=0; vboIdx<VF_MAX; ++vboIdx )
        {
            VboVec::iterator itor = mVbos[vboIdx].begin();
            VboVec::iterator end  = mVbos[vboIdx].end();

            while( itor != end )
            {
                Vbo &vbo = *itor;
                if( vbo.mFreeList.size() == 1u &&
                    vbo.sizeBytes == vbo.mFreeList.back().size )
                {
                    VaoVec::iterator itVao = mVaos.begin();
                    VaoVec::iterator enVao = mVaos.end();

                    while( itVao != enVao )
                    {
                        bool usesBuffer = false;
                        Vao::VertexBindingVec::const_iterator itBuf = itVao->vertexBuffers.begin();
                        Vao::VertexBindingVec::const_iterator enBuf = itVao->vertexBuffers.end();

                        while( itBuf != enBuf && !usesBuffer )
                        {
                            N_assert( itBuf->vertexBufferVbo != vbo.vboName &&
                                             "A GeometryRaw still references "
                                             "a deleted vertex buffer!" );
                            ++itBuf;
                        }

                        N_assert( itVao->indexBufferVbo != vbo.vboName &&
                                         "A GeometryRaw still references "
                                         "a deleted index buffer!" );
                        ++itVao;
                    }

                    vbo.vboName = 0;
                    delete vbo.mBuffer;
                    vbo.mBuffer = 0;

                    //There's (unrelated) live buffers whose vboIdx will now point out of bounds.
                    //We need to update them so they don't crash deallocateVbo later.
                    switchVboPoolIndex( (size_t)(mVbos[vboIdx].size() - 1u),
                                        (size_t)(itor - mVbos[vboIdx].begin()) );

                    itor = remove( mVbos[vboIdx], itor );
                    end  = mVbos[vboIdx].end();
                }
                else
                {
                    ++itor;
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::allocateVbo( size_t sizeBytes, size_t alignment, BufferType bufferType,
                                       size_t &outVboIdx, size_t &outBufferOffset )
    {
        N_assert( alignment > 0 );

        alignment = Math::lcm( alignment, c_minimumAlignment );

        VboFlag vboFlag = modeToFlag( bufferType );

        if( N_MarkTrue(bufferType, M_NORMAL))
            sizeBytes   *= mDynamicBufferMultiplier;

        VboVec::const_iterator itor = mVbos[vboFlag].begin();
        VboVec::const_iterator end  = mVbos[vboFlag].end();

        //Find a suitable VBO that can hold the requested size. We prefer those free
        //blocks that have a matching stride (the current offset is a multiple of
        //bytesPerElement) in order to minimize the amount of memory padding.
        size_t bestVboIdx   = ~0;
        size_t bestBlockIdx = ~0;
        bool foundMatchingStride = false;

        while( itor != end && !foundMatchingStride )
        {
            BlockVec::const_iterator blockIt = itor->mFreeList.begin();
            BlockVec::const_iterator blockEn = itor->mFreeList.end();

            while( blockIt != blockEn && !foundMatchingStride )
            {
                const Block &block = *blockIt;

                //Round to next multiple of alignment
                size_t newOffset = ( (block.offset + alignment - 1) / alignment ) * alignment;
                size_t padding = newOffset - block.offset;

                if( sizeBytes + padding <= block.size )
                {
                    bestVboIdx      = itor - mVbos[vboFlag].begin();
                    bestBlockIdx    = blockIt - itor->mFreeList.begin();

                    if( newOffset == block.offset )
                        foundMatchingStride = true;
                }

                ++blockIt;
            }

            ++itor;
        }

        if( bestBlockIdx == (size_t)~0 )
        {
            bestVboIdx      = mVbos[vboFlag].size();
            bestBlockIdx    = 0;
            foundMatchingStride = true;

            Vbo newVbo;

            //Ensure pool size is multiple of 4 otherwise some StagingBuffer copies can fail.
            //(when allocations happen very close to the end of the pool)
            size_t poolSize = std::max( mDefaultPoolSize[vboFlag], sizeBytes );
            poolSize = multiCeil( poolSize, 4u );

            //No luck, allocate a new buffer.
            MTLResourceOptions resourceOptions = 0;

            if( vboFlag == CPU_INACCESSIBLE )
                resourceOptions = MTLResourceStorageModePrivate;
            else
            {
                resourceOptions = MTLResourceCPUCacheModeWriteCombined;
                if( vboFlag == CPU_ACCESSIBLE_DEFAULT )
                    resourceOptions |= 0;
                else if( vboFlag == CPU_ACCESSIBLE_PERSISTENT )
                    resourceOptions |= MTLResourceStorageModeShared;
                else if( vboFlag == CPU_ACCESSIBLE_PERSISTENT_COHERENT )
                    resourceOptions |= MTLResourceCPUCacheModeDefaultCache;
            }

            newVbo.vboName = [mDevice->mDevice newBufferWithLength:poolSize options:resourceOptions];

            if( !newVbo.vboName )
            {
                N_EXCEPT( RenderingAPI,
                             "Out of GPU memory or driver refused.\n"
                             "Requested: " + StrConv::conv( poolSize ) + " bytes.");
            }

            newVbo.sizeBytes = poolSize;
            newVbo.mFreeList.push_back( Block( 0, poolSize ) );
            newVbo.mBuffer = 0;

            if( vboFlag != CPU_INACCESSIBLE )
            {
                newVbo.mBuffer = new MetalBuffer( newVbo.vboName, newVbo.sizeBytes  );
            }

            mVbos[vboFlag].push_back( newVbo );
        }

        Vbo &bestVbo        = mVbos[vboFlag][bestVboIdx];
        Block &bestBlock    = bestVbo.mFreeList[bestBlockIdx];

        size_t newOffset = ( (bestBlock.offset + alignment - 1) / alignment ) * alignment;
        size_t padding = newOffset - bestBlock.offset;
        //Shrink our records about available data.
        bestBlock.size   -= sizeBytes + padding;
        bestBlock.offset = newOffset + sizeBytes;

        if( !foundMatchingStride )
        {
            //This is a stride changer, record as such.
            StrideChangerVec::iterator itStride = std::lower_bound( bestVbo.strideChangers.begin(),
                                                                    bestVbo.strideChangers.end(),
                                                                    newOffset, StrideChanger() );
            bestVbo.strideChangers.insert( itStride, StrideChanger( newOffset, padding ) );
        }

        if( bestBlock.size == 0 )
            bestVbo.mFreeList.erase( bestVbo.mFreeList.begin() + bestBlockIdx );

        outVboIdx       = bestVboIdx;
        outBufferOffset = newOffset;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::deallocateVbo( size_t vboIdx, size_t bufferOffset, size_t sizeBytes, BufferType bufferType )
    {
        VboFlag vboFlag = modeToFlag( bufferType );

        if( N_MarkTrue(bufferType, M_NORMAL))
            sizeBytes *= mDynamicBufferMultiplier;

        Vbo &vbo = mVbos[vboFlag][vboIdx];
        StrideChangerVec::iterator itStride = std::lower_bound( vbo.strideChangers.begin(),
                                                                vbo.strideChangers.end(),
                                                                bufferOffset, StrideChanger() );

        if( itStride != vbo.strideChangers.end() && itStride->offsetAfterPadding == bufferOffset )
        {
            bufferOffset    -= itStride->paddedBytes;
            sizeBytes       += itStride->paddedBytes;

            vbo.strideChangers.erase( itStride );
        }

        //See if we're contiguous to a free block and make that block grow.
        vbo.mFreeList.push_back( Block( bufferOffset, sizeBytes ) );
        mergeContiguousBlocks( vbo.mFreeList.end() - 1, vbo.mFreeList );
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::mergeContiguousBlocks( BlockVec::iterator blockToMerge,
                                                 BlockVec &blocks )
    {
        BlockVec::iterator itor = blocks.begin();
        BlockVec::iterator end  = blocks.end();

        while( itor != end )
        {
            if( itor->offset + itor->size == blockToMerge->offset )
            {
                itor->size += blockToMerge->size;
                size_t idx = itor - blocks.begin();

                //When blockToMerge is the last one, its index won't be the same
                //after removing the other iterator, they will swap.
                if( idx == blocks.size() - 1 )
                    idx = blockToMerge - blocks.begin();

                remove( blocks, blockToMerge );

                blockToMerge = blocks.begin() + idx;
                itor = blocks.begin();
                end  = blocks.end();
            }
            else if( blockToMerge->offset + blockToMerge->size == itor->offset )
            {
                blockToMerge->size += itor->size;
                size_t idx = blockToMerge - blocks.begin();

                //When blockToMerge is the last one, its index won't be the same
                //after removing the other iterator, they will swap.
                if( idx == blocks.size() - 1 )
                    idx = itor - blocks.begin();

                remove( blocks, itor );

                blockToMerge = blocks.begin() + idx;
                itor = blocks.begin();
                end  = blocks.end();
            }
            else
            {
                ++itor;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    VertexBufferPacked* MetalGBufferManager::createVertexBufferImpl( size_t numElements,
                                                                   uint32 bytesPerElement,
                                                                   BufferType bufferType,
                                                                   void *initialData, bool keepAsShadow,
                                                                   const VertexUnitList &vElements )
    {
        size_t vboIdx;
        size_t bufferOffset;

        const size_t requestedSize = numElements * bytesPerElement;
        size_t sizeBytes = requestedSize;

        if( N_MarkTrue(bufferType, M_NORMAL) )
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //offset after each map to be aligned; and for that, we
            //sizeBytes to be multiple of alignment.
            const uint32 alignment = Math::lcm( bytesPerElement, c_minimumAlignment );
            sizeBytes = multiCeil( sizeBytes, alignment );
        }

        allocateVbo( sizeBytes, bytesPerElement, bufferType, vboIdx, bufferOffset );

        VboFlag vboFlag = modeToFlag( bufferType );
        Vbo &vbo = mVbos[vboFlag][vboIdx];
        MetalBufferOp *op = new MetalBufferOp( vboIdx, vbo.vboName,
                                                                          vbo.mBuffer );

        VertexBufferPacked *retVal = N_new VertexBufferPacked(
                                                        bufferOffset, numElements, bytesPerElement,
                                                        (sizeBytes - requestedSize) / bytesPerElement,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, op, vElements, 0, 0, 0 );

        if( initialData )
            op->_init( initialData, 0, numElements );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::destroyVertexBufferImpl( VertexBufferPacked *vertexBuffer )
    {
        MetalBufferOp *op = static_cast<MetalBufferOp*>(
                                                        vertexBuffer->getBufferInterface() );


        deallocateVbo( op->getVboPoolIndex(),
                       vertexBuffer->_getInternalBufferStart() * vertexBuffer->getBytesPerElement(),
                       vertexBuffer->_getInternalTotalSizeBytes(),
                       vertexBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    MultiSourceVertexBufferPool* MetalGBufferManager::createMultiSourceVertexBufferPoolImpl(
                                                const VertexUnitListList & vertexElementsBySource,
                                                size_t maxNumVertices, size_t totalBytesPerVertex,
                                                BufferType bufferType )
    {
        size_t vboIdx;
        size_t bufferOffset;

        allocateVbo( maxNumVertices * totalBytesPerVertex, totalBytesPerVertex,
                     bufferType, vboIdx, bufferOffset );

        VboFlag vboFlag = modeToFlag( bufferType );

        const Vbo &vbo = mVbos[vboFlag][vboIdx];

        return N_new MetalMultiSourceVertexBufferPool( vboIdx, vbo.vboName, vertexElementsBySource,
                                                          maxNumVertices, bufferType,
                                                          bufferOffset, this );
    }
    //-----------------------------------------------------------------------------------
    IndexBufferPacked* MetalGBufferManager::createIndexBufferImpl( size_t numElements,
                                                                 uint32 bytesPerElement,
                                                                 BufferType bufferType,
                                                                 void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;

        const size_t requestedSize = numElements * bytesPerElement;
        size_t sizeBytes = requestedSize;

        if(N_MarkTrue(bufferType, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //offset after each map to be aligned; and for that, we
            //sizeBytes to be multiple of alignment.
            const uint32 alignment = Math::lcm( bytesPerElement, c_indexBufferAlignment );
            sizeBytes = multiCeil( sizeBytes, alignment );
        }

        allocateVbo( sizeBytes, bytesPerElement, bufferType, vboIdx, bufferOffset );

        VboFlag vboFlag = modeToFlag( bufferType );

        Vbo &vbo = mVbos[vboFlag][vboIdx];
        MetalBufferOp *op = new MetalBufferOp( vboIdx, vbo.vboName,
                                                                          vbo.mBuffer );
        IndexBufferPacked *retVal = N_new IndexBufferPacked(
                                                        bufferOffset, numElements, bytesPerElement,
                                                        (sizeBytes - requestedSize) / bytesPerElement,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, op );

        if( initialData )
            op->_init( initialData, 0, numElements );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::destroyIndexBufferImpl( IndexBufferPacked *indexBuffer )
    {
        MetalBufferOp *op = static_cast<MetalBufferOp*>(
                                                        indexBuffer->getBufferInterface() );


        deallocateVbo( op->getVboPoolIndex(),
                       indexBuffer->_getInternalBufferStart() * indexBuffer->getBytesPerElement(),
                       indexBuffer->_getInternalTotalSizeBytes(),
                       indexBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    StructBuffer* MetalGBufferManager::createConstBufferImpl( size_t sizeBytes, BufferType bufferType,
                                                                 void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;

        size_t alignment = mAlignment[GBT_Struct];

        VboFlag vboFlag = modeToFlag( bufferType );

        const size_t requestedSize = sizeBytes;

        if(N_MarkTrue(bufferType, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //offset after each map to be aligned; and for that, we
            //sizeBytes to be multiple of alignment.
            sizeBytes = multiCeil( sizeBytes, alignment );
        }

        allocateVbo( sizeBytes, alignment, bufferType, vboIdx, bufferOffset );

        Vbo &vbo = mVbos[vboFlag][vboIdx];
        MetalBufferOp *op = new MetalBufferOp( vboIdx, vbo.vboName,
                                                                              vbo.mBuffer );
        StructBuffer *retVal = N_new MetalStructBuffer(
                                                        bufferOffset, requestedSize, 1,
                                                        (sizeBytes - requestedSize) / 1,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, op,
                                                        mDevice );

        if( initialData )
            op->_init( initialData, 0, requestedSize );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::destroyConstBufferImpl( StructBuffer *constBuffer )
    {
        MetalBufferOp *op = static_cast<MetalBufferOp*>(
                                                        constBuffer->getBufferInterface() );


        deallocateVbo( op->getVboPoolIndex(),
                       constBuffer->_getInternalBufferStart() * constBuffer->getBytesPerElement(),
                       constBuffer->_getInternalTotalSizeBytes(),
                       constBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    TextureBuffer* MetalGBufferManager::createTexBufferImpl( PixelFormat pf, size_t sizeBytes,
                                                           BufferType bufferType,
                                                           void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;

        size_t alignment = mAlignment[GBT_Texture];

        VboFlag vboFlag = modeToFlag( bufferType );

        const size_t requestedSize = sizeBytes;

        if( N_MarkTrue(bufferType, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //offset after each map to be aligned; and for that, we
            //sizeBytes to be multiple of alignment.
            const uint32 alignment = Math::lcm( 1, c_minimumAlignment );
            sizeBytes = multiCeil( sizeBytes, alignment );
        }

        allocateVbo( sizeBytes, alignment, bufferType, vboIdx, bufferOffset );

        Vbo &vbo = mVbos[vboFlag][vboIdx];
        MetalBufferOp *op = new MetalBufferOp( vboIdx, vbo.vboName,
                                                                          vbo.mBuffer );
        TextureBuffer *retVal = N_new MetalTextureBuffer(
                                                        bufferOffset, requestedSize, 1,
                                                        (sizeBytes - requestedSize) / 1,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, op, pf,
                                                        mDevice );

        if( initialData )
            op->_init( initialData, 0, requestedSize );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::destroyTexBufferImpl( TextureBuffer *texBuffer )
    {
        MetalBufferOp *op = static_cast<MetalBufferOp*>(
                                                        texBuffer->getBufferInterface() );


        deallocateVbo( op->getVboPoolIndex(),
                       texBuffer->_getInternalBufferStart() * texBuffer->getBytesPerElement(),
                       texBuffer->_getInternalTotalSizeBytes(),
                       texBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    StorageBuffer* MetalGBufferManager::createUavBufferImpl( size_t numElements, uint32 bytesPerElement,
                                                           uint32 bindFlags,
                                                           void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;

        size_t alignment = Math::lcm( mAlignment[GBT_Storage], bytesPerElement );

        //UAV Buffers can't be dynamic.
        const BufferType bufferType = M_DEVNORMAL;
        VboFlag vboFlag = modeToFlag( bufferType );

        allocateVbo( numElements * bytesPerElement, alignment, bufferType, vboIdx, bufferOffset );

        Vbo &vbo = mVbos[vboFlag][vboIdx];
        MetalBufferOp *op = new MetalBufferOp( vboIdx, vbo.vboName,
                                                                          vbo.mBuffer );
        StorageBuffer *retVal = N_new MetalStorageBuffer(
                                                        bufferOffset, numElements, bytesPerElement,
                                                        bindFlags, initialData, keepAsShadow,
                                                        this, op, mDevice );

        if( initialData )
            op->_init( initialData, 0, numElements );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::destroyUavBufferImpl( StorageBuffer *uavBuffer )
    {
        MetalBufferOp *op = static_cast<MetalBufferOp*>(
                                                        uavBuffer->getBufferInterface() );


        deallocateVbo( op->getVboPoolIndex(),
                       uavBuffer->_getInternalBufferStart() * uavBuffer->getBytesPerElement(),
                       uavBuffer->_getInternalTotalSizeBytes(),
                       uavBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    IndirectBuffer* MetalGBufferManager::createIndirectBufferImpl( size_t sizeBytes,
                                                                     BufferType bufferType,
                                                                     void *initialData,
                                                                     bool keepAsShadow )
    {
        const size_t alignment = 4;
        size_t bufferOffset = 0;

        const size_t requestedSize = sizeBytes;

        if(N_MarkTrue(bufferType, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //offset after each map to be aligned; and for that, we
            //sizeBytes to be multiple of alignment.
            sizeBytes = multiCeil( sizeBytes, alignment );
        }

        MetalBufferOp *op = 0;
        if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
        {
            size_t vboIdx;
            VboFlag vboFlag = modeToFlag( bufferType );

            allocateVbo( sizeBytes, alignment, bufferType, vboIdx, bufferOffset );

            Vbo &vbo = mVbos[vboFlag][vboIdx];
            op = new MetalBufferOp( vboIdx, vbo.vboName, vbo.mBuffer );
        }

        IndirectBuffer *retVal = N_new IndirectBuffer(
                                                        bufferOffset, requestedSize, 1,
                                                        (sizeBytes - requestedSize) / 1,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, op );

        if( initialData )
        {
            if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
            {
                op->_init( initialData, 0, requestedSize );
            }
            else
            {
                memcpy( retVal->getHostData(), initialData, requestedSize );
            }
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::destroyIndirectBufferImpl( IndirectBuffer *indirectBuffer )
    {
        if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
        {
            MetalBufferOp *op = static_cast<MetalBufferOp*>(
                        indirectBuffer->getBufferInterface() );


            deallocateVbo( op->getVboPoolIndex(),
                           indirectBuffer->_getInternalBufferStart() *
                                indirectBuffer->getBytesPerElement(),
                           indirectBuffer->_getInternalTotalSizeBytes(),
                           indirectBuffer->mMark );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::bindDrawId(void)
    {
        N_assert( mDevice->mRenderEncoder || mDevice->mFrameAborted );

        MetalBufferOp *op = static_cast<MetalBufferOp*>(
                    mDrawId->getBufferInterface() );

        [mDevice->mRenderEncoder setVertexBuffer:op->getVboName() offset:0 atIndex:15];
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        [mDevice->mRenderEncoder setFragmentBuffer:op->getVboName() offset:0 atIndex:15];
#endif
    }
    //-----------------------------------------------------------------------------------
    GeometryRaw* MetalGBufferManager::createVertexArrayObjectImpl(
                                                            const VertexBufferList &vertexBuffers,
                                                            IndexBufferPacked *indexBuffer,
                                                            OperationType opType )
    {
        RenderSys *hlmsManager = Root::getSingleton().getRender();
        VertexUnitListList units = GeometryRaw::getVertexDeclaration( vertexBuffers );
        const uint16 inputLayout = hlmsManager->getVaoType( units, opType );

        VaoVec::iterator itor = findVao( vertexBuffers, indexBuffer, opType );

        const uint32 renderQueueId = generateRenderQueueId( itor->vaoName, mValidVAO );

        GeometryRaw *retVal = N_new GeometryRaw( itor->vaoName,
                                                                renderQueueId,
                                                                inputLayout,
                                                                vertexBuffers,
                                                                indexBuffer,
                                                                opType );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::destroyVertexArrayObjectImpl( GeometryRaw *vao )
    {
        VaoVec::iterator itor = mVaos.begin();
        VaoVec::iterator end  = mVaos.end();

        while( itor != end && itor->vaoName != vao->mVaoId )
            ++itor;

        if( itor != end )
        {
            --itor->refCount;

            if( !itor->refCount )
                remove( mVaos, itor );
        }

        //We delete it here because this class has no virtual destructor on purpose
        N_delete vao;
    }
    //-----------------------------------------------------------------------------------
    MetalGBufferManager::VaoVec::iterator MetalGBufferManager::findVao(
                                                        const VertexBufferList &vertexBuffers,
                                                        IndexBufferPacked *indexBuffer,
                                                        OperationType opType )
    {
        Vao vao;

        vao.mType = opType;
        vao.vertexBuffers.reserve( vertexBuffers.size() );

        {
            VertexBufferList::const_iterator itor = vertexBuffers.begin();
            VertexBufferList::const_iterator end  = vertexBuffers.end();

            while( itor != end )
            {
                Vao::VertexBinding vertexBinding;
                vertexBinding.vertexBufferVbo   = static_cast<MetalBufferOp*>(
                                                        (*itor)->getBufferInterface() )->getVboName();
                vertexBinding.mUnits    = (*itor)->getUnits();
                vertexBinding.instancingDivisor = 0;

                /*const MultiSourceVertexBufferPool *multiSourcePool = (*itor)->getMultiSourcePool();
                if( multiSourcePool )
                {
                    vertexBinding.offset = multiSourcePool->getBytesOffsetToSource(
                                                            (*itor)->_getSourceIndex() );
                }*/

                vao.vertexBuffers.push_back( vertexBinding );

                ++itor;
            }
        }

        vao.refCount = 0;

        if( indexBuffer )
        {
            vao.indexBufferVbo  = static_cast<MetalBufferOp*>(
                                    indexBuffer->getBufferInterface() )->getVboName();
            vao.indexType       = indexBuffer->getIndexType();
        }
        else
        {
            vao.indexBufferVbo  = 0;
            vao.indexType       = IndexBufferPacked::IT_16BIT;
        }

        bool bFound = false;
        VaoVec::iterator itor = mVaos.begin();
        VaoVec::iterator end  = mVaos.end();

        while( itor != end && !bFound )
        {
            if( itor->mType == vao.mType &&
                itor->indexBufferVbo == vao.indexBufferVbo &&
                itor->indexType == vao.indexType &&
                itor->vertexBuffers == vao.vertexBuffers )
            {
                bFound = true;
            }
            else
            {
                ++itor;
            }
        }

        if( !bFound )
        {
            vao.vaoName = createVao( vao );
            mVaos.push_back( vao );
            itor = mVaos.begin() + mVaos.size() - 1;
        }

        ++itor->refCount;

        return itor;
    }
    //-----------------------------------------------------------------------------------
    uint32 MetalGBufferManager::createVao( const Vao &vaoRef )
    {
        return mVaoNames++;
    }
    //-----------------------------------------------------------------------------------
    uint32 MetalGBufferManager::generateRenderQueueId( uint32 vaoName, uint32 uniqueVaoId )
    {
        //Mix mValidVAO with the D3D11 Vao for better sorting purposes:
        //  If we only use the D3D11's vao, the RQ will sort Meshes with
        //  multiple submeshes mixed with other meshes.
        //  For cache locality, and assuming all of them have the same GL vao,
        //  we prefer the RQ to sort:
        //      1. Mesh A - SubMesh 0
        //      2. Mesh A - SubMesh 1
        //      3. Mesh B - SubMesh 0
        //      4. Mesh B - SubMesh 1
        //      5. Mesh D - SubMesh 0
        //  If we don't mix mValidVAO in it; the following could be possible:
        //      1. Mesh B - SubMesh 1
        //      2. Mesh D - SubMesh 0
        //      3. Mesh A - SubMesh 1
        //      4. Mesh B - SubMesh 0
        //      5. Mesh A - SubMesh 0
        //  Thus thrashing the cache unnecessarily.
        const int bitsVaoGl  = 5;
        const uint32 maskVaoGl  = N_Mask( bitsVaoGl );
        const uint32 maskVao    = N_Mask( RqBits::MeshBits - bitsVaoGl );

        const uint32 shiftVaoGl     = RqBits::MeshBits - bitsVaoGl;

        uint32 renderQueueId =
                ( (vaoName & maskVaoGl) << shiftVaoGl ) |
                (uniqueVaoId & maskVao);

        return renderQueueId;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::createImpl(StagingBuffer*& buf, NCount sizeBytes, Nmark modemark )
    {
        sizeBytes = std::max<size_t>( sizeBytes, 4 * 1024 * 1024 );
        sizeBytes = multiCeil( sizeBytes, 4u );

        MTLResourceOptions resourceOptions = 0;

        resourceOptions |= MTLResourceStorageModeShared;

        if( forUpload )
            resourceOptions |= MTLResourceCPUCacheModeWriteCombined;
        else
            resourceOptions |= MTLResourceCPUCacheModeDefaultCache;

        id<MTLBuffer> bufferName = [mDevice->mDevice newBufferWithLength:sizeBytes
                                                                 options:resourceOptions];
        if( !bufferName )
        {
            N_EXCEPT( RenderingAPI, "Out of GPU memory or driver refused.\n Requested: " + StrConv::conv( sizeBytes ) + " bytes." );
        }

        MetalMappedBuffer *stagingBuffer = N_new MetalMappedBuffer( 0, sizeBytes, this, forUpload, bufferName, mDevice );
        
        mRefMappedList.push_back( stagingBuffer );

        return stagingBuffer;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::create(GpuBufferAsync *& out, BufferPacked *creator, StagingBuffer * buf, NCount oft, NCount size )
    {
        out = N_new MetalAsyncTicket( creator, buf, oft, size, mDevice ) ;
    }
    //-----------------------------------------------------------------------------------
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
    void MetalGBufferManager::unalignedCopy( id<MTLBuffer> dstBuffer, size_t dstOffsetBytes,
                                         id<MTLBuffer> srcBuffer, size_t srcOffsetBytes,
                                         size_t sizeBytes )
    {
        __unsafe_unretained id<MTLComputeCommandEncoder> computeEncoder = mDevice->getComputeEncoder();

        [computeEncoder setBuffer:dstBuffer offset:0 atIndex:0];
        [computeEncoder setBuffer:srcBuffer offset:0 atIndex:1];
        uint32_t copyInfo[3] =
        {
            static_cast<uint32_t>( dstOffsetBytes ),
            static_cast<uint32_t>( srcOffsetBytes ),
            static_cast<uint32_t>( sizeBytes )
        };
        [computeEncoder setBytes:copyInfo length:sizeof(copyInfo) atIndex:2];

        MTLSize threadsPerThreadgroup   = MTLSizeMake( 1024u, 1u, 1u );
        MTLSize threadgroupsPerGrid     = MTLSizeMake( 1u, 1u, 1u );

        const size_t threadsRequired = multiCeil( sizeBytes, 64u ) / 64u;
        threadsPerThreadgroup.width =
                std::min<NSUInteger>( static_cast<NSUInteger>( threadsRequired ), 1024u );
        threadgroupsPerGrid.width =
                static_cast<NSUInteger>( multiCeil( threadsRequired,
                                                              threadsPerThreadgroup.width ) /
                                         threadsPerThreadgroup.width );

        [computeEncoder setComputePipelineState:mUnalignedCopyPso];

        [computeEncoder dispatchThreadgroups:threadgroupsPerGrid
                       threadsPerThreadgroup:threadsPerThreadgroup];
    }
#endif
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::_update(void)
    {
        unsigned long currentTimeMs = mTimer->getMilliseconds();

        if( currentTimeMs >= mNextCheck )
        {
            mNextCheck = (unsigned long)(~0);

            MappedBufferList::iterator itor = mFreeMappedList.begin();
            MappedBufferList::iterator end  = mFreeMappedList.end();

            while( itor != end )
            {
                StagingBuffer *stagingBuffer = *itor;

                mNextCheck = std::min(
                                                    mNextCheck,
                                                    stagingBuffer->getLastUsedTimestamp() +
                                                    currentTimeMs );

                /*if( stagingBuffer->getLastUsedTimestamp() - currentTimeMs >
                    stagingBuffer->getUnfencedTimeThreshold() )
                {
                    static_cast<MetalMappedBuffer*>( stagingBuffer )->cleanUnfencedHazards();
                }*/

                if( stagingBuffer->getLastUsedTimestamp() - currentTimeMs >
                    stagingBuffer->getLifetimeThreshold() )
                {
                    //Time to delete this buffer.
                    delete *itor;

                    itor = remove( mFreeMappedList, itor );
                    end  = mFreeMappedList.end();
                }
                else
                {
                    ++itor;
                }
            }
        }

        if( !mSemaphoreFlushed )
        {
            //We could only reach here if _update() was called
            //twice in a row without completing a full frame.
            //Without this, wait will deadlock.
            mDevice->commitAndNextCommandBuffer();
        }

        if( !mRecoverList.empty() && mRecoverList.front().mFrameMark == mFrameMark )
        {
            wait();
            destroyRecover( mFrameMark );
        }

        //We must call this to raise the semaphore count in case we haven't already
        wait();

        GBufferManager::_update();

        mSemaphoreFlushed = false;
        mAlreadyWaitedForSemaphore[mFrameMark] = false;
        __block dispatch_semaphore_t blockSemaphore = mFrameSyncVec[mFrameMark];
        [mDevice->mCurrentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
        {
            dispatch_semaphore_signal( blockSemaphore );
        }];

        mFrameMark = (mFrameMark + 1) % mDynamicBufferMultiplier;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::_notifyNewCommandBuffer(void)
    {
        mSemaphoreFlushed = true;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::_notifyDeviceStalled(void)
    {
        mSemaphoreFlushed = true;

        MappedBufferList::const_iterator itor = mRefMappedList.begin();
        MappedBufferList::const_iterator end  = mRefMappedList.end();

        while( itor != end )
        {
            MetalMappedBuffer *stagingBuffer = static_cast<MetalMappedBuffer*>( *itor );
            stagingBuffer->_notifyDeviceStalled();
            ++itor;
        }

        itor = mFreeMappedList.begin();
        end  = mFreeMappedList.end();

        while( itor != end )
        {
            MetalMappedBuffer *stagingBuffer = static_cast<MetalMappedBuffer*>( *itor );
            stagingBuffer->_notifyDeviceStalled();
            ++itor;
        }

        destroyAllRecover();

        mFrameCount += mDynamicBufferMultiplier;
    }
    //-----------------------------------------------------------------------------------
    uint8 MetalGBufferManager::wait(void)
    {
        //MetalRenderSystem::_beginFrameOnce does a global waiting for us, but if we're outside
        //the render loop (i.e. user is manually uploading data) we may have to call this earlier.
        if( !mAlreadyWaitedForSemaphore[mFrameMark] )
        {
            waitFor( mFrameSyncVec[mFrameMark], mDevice );
            //Semaphore was just grabbed, so ensure we don't grab it twice.
            mAlreadyWaitedForSemaphore[mFrameMark] = true;
        }
        //mDevice->mRenderSys->_waitForTailFrameToFinish();
        return mFrameMark;
    }
    //-----------------------------------------------------------------------------------
    void MetalGBufferManager::wait( uint32 frameCount )
    {
        if( frameCount == mFrameCount )
        {
            //Full stall
            mDevice->stall();
            //"mFrameCount += mDynamicBufferMultiplier" is already handled in _notifyDeviceStalled;
        }
        if( mFrameCount - frameCount <= mDynamicBufferMultiplier )
        {
            //Let's wait on one of our existing fences...
            //frameDiff has to be in range [1; mDynamicBufferMultiplier]
            size_t frameDiff = mFrameCount - frameCount;

            const size_t idx = (mFrameMark +
                                mDynamicBufferMultiplier - frameDiff) % mDynamicBufferMultiplier;

            if( !mAlreadyWaitedForSemaphore[idx] )
            {
                waitFor( mFrameSyncVec[idx], mDevice );
                //Semaphore was just grabbed, so ensure we don't grab it twice.
                mAlreadyWaitedForSemaphore[idx] = true;
            }
        }
        else
        {
            //No stall
        }
    }
    //-----------------------------------------------------------------------------------
    bool MetalGBufferManager::isFrameFinished( uint32 frameCount )
    {
        bool retVal = false;
        if( frameCount == mFrameCount )
        {
            //Full stall
            //retVal = false;
        }
        else if( mFrameCount - frameCount <= mDynamicBufferMultiplier )
        {
            //frameDiff has to be in range [1; mDynamicBufferMultiplier]
            size_t frameDiff = mFrameCount - frameCount;
            const size_t idx = (mFrameMark +
                                mDynamicBufferMultiplier - frameDiff) % mDynamicBufferMultiplier;

            if( !mAlreadyWaitedForSemaphore[idx] )
            {
                const long result = dispatch_semaphore_wait( mFrameSyncVec[idx], DISPATCH_TIME_NOW );
                if( result == 0 )
                {
                    retVal = true;
                    //Semaphore was just grabbed, so ensure we don't grab it twice.
                    mAlreadyWaitedForSemaphore[idx] = true;
                }
            }
        }
        else
        {
            //No stall
            retVal = true;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    dispatch_semaphore_t MetalGBufferManager::waitFor( dispatch_semaphore_t fenceName, MetalDevice *device )
    {
        dispatch_time_t timeout = DISPATCH_TIME_NOW;
        while( true )
        {
            long result = dispatch_semaphore_wait( fenceName, timeout );

            if( result == 0 )
                return 0; //Success waiting.

            if( timeout == DISPATCH_TIME_NOW )
            {
                // After the first time, need to start flushing, and wait for a looong time.
                timeout = DISPATCH_TIME_FOREVER;
                device->commitAndNextCommandBuffer();
            }
            else if( result != 0 )
            {
                N_EXCEPT( RenderingAPI,
                             "Failure while waiting for a MetalFence. Could be out of GPU memory. "
                             "Update your video card drivers. If that doesn't help, "
                             "contact the developers. Error code: " + StrConv::conv( result ) );

                return fenceName;
            }
        }

        return 0;
    }
    //-----------------------------------------------------------------------------------
    uint32 MetalGBufferManager::getAttributeIndexFor( VertexType vtype )
    {
        return VERTEX_ATTRIBUTE_INDEX[vtype];
    }
    //-----------------------------------------------------------------------------------
    MetalGBufferManager::VboFlag MetalGBufferManager::modeToFlag( BufferType bufferType )
    {
        return static_cast<VboFlag>( std::max( 0, (bufferType - M_NORMAL) + CPU_ACCESSIBLE_DEFAULT ) );
    }
}
