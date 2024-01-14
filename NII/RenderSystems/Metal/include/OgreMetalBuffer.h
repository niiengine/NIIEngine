/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

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
#ifndef _OgreMetalHardwareVertexBuffer_H_
#define _OgreMetalHardwareVertexBuffer_H_

#include "OgreMetalHardwareBufferCommon.h"
#include "OgreHardwareVertexBuffer.h"
#import <dispatch/dispatch.h>

namespace NII 
{
    /// Common buffer operations for most v1 buffer interfaces used in Metal
    /// This implementation treats:
    ///		Ignores STATIC and DYNAMIC bit in buffers
    ///		Lack of WRITE_ONLY and DISCARDABLE buffer puts it in slowest path.
    ///		Puts WRITE_ONLY in device memory and uses staging buffers to avoid blocking.
    ///			Use WRITE_ONLY when possible.
    ///		When DISCARDABLE bit is set, it uses MetalDiscardBuffer.
    class _OgreMetalExport MetalHardwareBufferCommon
    {
    public:
        MetalHardwareBufferCommon( size_t sizeBytes, Buffer::Usage usage, uint16 alignment,
                                   MetalDiscardBufferManager *discardBufferManager,
                                   MetalDevice *device );
        virtual ~MetalHardwareBufferCommon();

        void _notifyDeviceStalled(void);

        /** Returns the actual API buffer, but first sets mLastFrame as we
            assume you're calling this function to use the buffer in the GPU.
        @param outOffset
            Out. Guaranteed to be written. Used by HBU_DISCARDABLE buffers which
            need an offset to the internal ring buffer we've allocated.
        @return
            The MTLBuffer in question.
        */
        id<MTLBuffer> getBufferName( size_t &outOffset );
        id<MTLBuffer> getBufferNameForGpuWrite(void);

        /// @see Buffer.
        void* lockImpl( size_t offset, size_t length,
                        Buffer::LockOptions options, bool isLocked );
        /// @see Buffer.
        void unlockImpl( size_t lockStart, size_t lockSize );

        /// @see Buffer.
        void readData( size_t offset, size_t length, void* pDest );

        /// @see Buffer.
        void writeData( size_t offset, size_t length,
                        const void* pSource, bool discardWholeBuffer = false );
        /// @see Buffer.
        void copyData( MetalHardwareBufferCommon *srcBuffer, size_t srcOffset,
                       size_t dstOffset, size_t length, bool discardWholeBuffer = false );

        size_t getSizeBytes(void) const         { return mSizeBytes; }
    private:
        id<MTLBuffer>       mBuffer;
        size_t              mSizeBytes;
        MetalDevice         *mDevice;
        MetalDiscardBuffer  *mDiscardBuffer;
        GBufferManager          *mMag;
        MappedBuffer       *mStagingBuffer;
        uint32              mLastFrame;
        uint32              mLastFrameGpuWrote;
    };
    
    /** Metal doesn't "map". You can directly access the unsynchronized contents
        For performance we keep many buffers as one big buffer, but for compatibility
        reasons (with GL3/DX10 HW) we treat them as separate buffers.
    @par
        This class treats mapping in a GL-style. The most usefulness is that in OS X
        we need to inform what regions were modified (flush). On iOS it serves
        no real purpose and is just a pass through with some asserts for consistency
        checking.
    @remarks
        This is a very thin/lightweight wrapper around MTLBuffer, thus:
            Caller is responsible for flushing regions before unmapping.
            Caller is responsible for proper synchronization.
            No check is performed to see if two map calls overlap.
    */
    class _OgreMetalExport MetalBuffer
    {
    public:
        MetalBuffer( id<MTLBuffer> vboName, size_t vboSize );
        ~MetalBuffer();

        id<MTLBuffer> getVboName(void) const        { return mVboName; }

        void * map(NCount start, NCount count, size_t & outTicket);

        /// Flushes the region of the given ticket. start is 0-based.
        void flush( size_t ticket, NCount start, NCount count );

        /// Unmaps given ticket (got from @see map).
        /// The ticket becomes invalid after this.
        void unmap( size_t ticket );
    protected:
        size_t addMappedRange( NCount start, NCount count );
        
        struct MappedRange
        {
            NCount start;
            NCount count;

            MappedRange( NCount _start, NCount _count ) : start( _start ), count( _count ) {}
        };

        typedef vector<MappedRange>::type MappedRangeVec;

        id<MTLBuffer> mVboName;
        size_t mVboSize;
        void * mMappedPtr;

        MappedRangeVec mMappedRanges;
        vector<size_t>::type mFreeRanges;
    };
    
    /** For Metal, all buffers can be treated with the same code.
        Hence most equivalent functionality is encapsulated here.
    */
    class _OgreMetalExport MetalBufferOp : public BufferOp
    {
    public:
        MetalBufferOp(size_t vboPoolIdx, id<MTLBuffer> vboName, MetalBuffer * dynamicBuffer );
        virtual ~MetalBufferOp();

        size_t getVboPoolIndex(void)                                { return mGroupIndex; }

        /// Use __unsafe_unretained when possible to avoid unnecessary ARC overhead.
        id<MTLBuffer> getVboName(void) const                        { return mVboName; }

        void _setVboPoolIndex( size_t newVboPool )                  { mGroupIndex = newVboPool; }

        /// Only use this function for the first upload
        void _init( const void *data, NCount elementStart, NCount elementCount );

        virtual void * lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx = 0);
        
        virtual void unlockImpl(UnlockType type, NCount oft, NCount size);
        
        bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true);
        
        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size);
        virtual void waitImpl();
        virtual void read( BufferOp *dstBuffer, size_t dstOffsetBytes, size_t srcOffsetBytes, size_t sizeBytes );
    protected:
        size_t mGroupIndex;
        id<MTLBuffer> mVboName;
        void * mMappedPtr;

        size_t mUnmapTicket;
        MetalBuffer * mDynamicBuffer;

        size_t advanceFrame( bool bAdvanceFrame );
    };
    
    /// Specialisation of VertexBuffer for Metal
    class _OgreMetalExport MetalVertexBuffer : public VertexBuffer
    {
    public:
        MetalVertexBuffer( MetalBufferManagerBase * mgr, size_t vertexSize, size_t numVertices, Buffer::Usage usage, bool useShadowBuffer );
        
        virtual ~MetalVertexBuffer();

        void _notifyDeviceStalled(void);

        /// @copydoc MetalHardwareBufferCommon::getBufferName
        id<MTLBuffer> getBufferName( size_t &outOffset );
        
        /// @copydoc MetalHardwareBufferCommon::getBufferNameForGpuWrite
        id<MTLBuffer> getBufferNameForGpuWrite(void);

        virtual void readData( size_t offset, size_t length, void* pDest );

        virtual void writeData( size_t offset, size_t length, const void* pSource, bool discardWholeBuffer = false );

        virtual void copyData( Buffer& srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer = false );

        virtual void _updateFromShadow(void);

        virtual void* getRenderSystemData(void);
    protected:
        virtual void* lockImpl( size_t offset, size_t length, LockOptions options );
        virtual void unlockImpl(void);
    private:
        MetalHardwareBufferCommon mMetalHardwareBufferCommon;
    };
    
    /// Specialisation of IndexBuffer for Metal
    class _OgreMetalExport MetalIndexBuffer : public IndexBuffer
    {
    public:
        MetalIndexBuffer( MetalBufferManagerBase * mgr, IndexType idxType, size_t numIndexes, Buffer::Usage usage, bool useShadowBuffer );
        virtual ~MetalIndexBuffer();

        void _notifyDeviceStalled(void);

        /// @copydoc MetalHardwareBufferCommon::getBufferName
        id<MTLBuffer> getBufferName( size_t &outOffset );

        /// @copydoc MetalHardwareBufferCommon::getBufferNameForGpuWrite
        id<MTLBuffer> getBufferNameForGpuWrite(void);

        virtual void readData(size_t offset, size_t length, void * pDest);

        virtual void writeData(size_t offset, size_t length, const void * pSource, bool discardWholeBuffer = false);

        virtual void copyData( Buffer& srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer = false );

        virtual void _updateFromShadow(void);

        virtual void* getRenderSystemData(void);
    protected:
        virtual void* lockImpl( size_t offset, size_t length, LockOptions options );
        virtual void unlockImpl(void);
    private:
        MetalHardwareBufferCommon mMetalHardwareBufferCommon;
    };
    
    class _OgreMetalExport MetalTextureBuffer : public TextureBuffer
    {
    public:
        MetalTextureBuffer( size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
                              uint32 numElementsPadding, BufferModeMark bufferType, void *initialData, bool keepAsShadow,
                              GBufferManager *vaoManager, MetalBufferOp *bufferInterface, PixelFormat pf, MetalDevice *device );
        virtual ~MetalTextureBuffer();

        virtual void bindBufferVS( uint16 slot, size_t offset=0, size_t sizeBytes=0 );

        virtual void bindBufferPS( uint16 slot, size_t offset=0, size_t sizeBytes=0 );

        virtual void bindBufferGS( uint16 slot, size_t offset=0, size_t sizeBytes=0 ) {}

        virtual void bindBufferDS( uint16 slot, size_t offset=0, size_t sizeBytes=0 ) {}

        virtual void bindBufferHS( uint16 slot, size_t offset=0, size_t sizeBytes=0 ) {}

        virtual void bindBufferCS( uint16 slot, size_t offset=0, size_t sizeBytes=0 );

        void bindBufferForDescriptor( __unsafe_unretained id <MTLBuffer> *buffers, NSUInteger *offsets, size_t offset );
    protected:
        MetalDevice * mDevice;
    };
    
    class _OgreMetalExport MetalStorageBuffer : public StorageBuffer
    {
    public:
        MetalStorageBuffer( size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
                              uint32 bindFlags, void *initialData, bool keepAsShadow, GBufferManager *vaoManager, MetalBufferOp *bufferInterface, MetalDevice *device );
        ~MetalStorageBuffer();

        void bindBufferAllRenderStages( uint16 slot, size_t offset=0 );
        virtual void bindBufferVS( uint16 slot, size_t offset=0, size_t sizeBytes=0 );
        virtual void bindBufferPS( uint16 slot, size_t offset=0, size_t sizeBytes=0 );
//        virtual void bindBufferGS( uint16 slot, size_t offset=0, size_t sizeBytes=0 ) {}
//        virtual void bindBufferDS( uint16 slot, size_t offset=0, size_t sizeBytes=0 ) {}
//        virtual void bindBufferHS( uint16 slot, size_t offset=0, size_t sizeBytes=0 ) {}
        virtual void bindBufferCS( uint16 slot, size_t offset=0, size_t sizeBytes=0 );

        void bindBufferForDescriptor( __unsafe_unretained id <MTLBuffer> * buffers, NSUInteger * offsets, size_t offset );
    protected:
        virtual TextureBuffer * createViewImpl( PixelFormat pf );
    protected:
        MetalDevice * mDevice;
    };
    
    class _OgreMetalExport MetalStructBuffer : public StructBuffer
    {
    public:
        MetalStructBuffer( size_t internalBufferStartBytes, size_t numElements, uint32 bytesPerElement, uint32 numElementsPadding,
                                BufferModeMark bufferType, void *initialData, bool keepAsShadow,
                                GBufferManager *vaoManager, BufferOp *bufferInterface, MetalDevice *device );
        virtual ~MetalStructBuffer();

        virtual void bindBufferVS( uint16 slot );
        virtual void bindBufferPS( uint16 slot );
        virtual void bindBufferGS( uint16 slot ) {}
        virtual void bindBufferHS( uint16 slot ) {}
        virtual void bindBufferDS( uint16 slot ) {}
        virtual void bindBufferCS( uint16 slot );

        void bindBufferVS( uint16 slot, uint32 offsetBytes );
        void bindBufferPS( uint16 slot, uint32 offsetBytes );
        void bindBufferCS( uint16 slot, uint32 offsetBytes );
    protected:
        MetalDevice * mDevice;
    };
    
    /** A staging buffer is a buffer that resides on the GPU and be written to/from both CPU & GPU
        However the access in both cases is limited. GPUs can only copy (i.e. memcpy) to another
        real buffer (can't be used directly as i.e. texture or vertex buffer) and CPUs can only
        map it.
        In other words, a staging buffer is an intermediate buffer to transfer data between
        CPU & GPU
    */
    class _OgreMetalExport MetalMappedBuffer : public MappedBuffer
    {
    public:
        MetalMappedBuffer( size_t internalBufferStart, size_t sizeBytes, GBufferManager *vaoManager, BufferModeMark mode,
            id<MTLBuffer> vboName, MetalDevice *device );
        virtual ~MetalMappedBuffer();

        virtual BlockType request( size_t sizeBytes, bool upload = true);

        void cleanUnfencedHazards(void);
        void _notifyDeviceStalled(void);

        void _unmapToV1( v1::MetalHardwareBufferCommon *hwBuffer, size_t lockStart, size_t lockSize );

        virtual NCount async( OperationList & oplist, bool upload);
        
        virtual void recover( NCount offset, NCount sizeBytes );
        
        virtual size_t _asyncDownloadV1( v1::MetalHardwareBufferCommon *source, size_t srcOffset, size_t srcLength );

        id<MTLBuffer> getBufferName(void) const     { return mVboName; }
    protected:
        //------------------------------------
        // End used for uploads
        //------------------------------------

        /** Adds a fence to the region [from; to]. The region may actually be kept in
            mUnfencedHazards until the threshold is achieved or we're forced to fence
            it (i.e. when the internal buffer wraps around)
        @param forceFence
            When true, the region is merged with all the other unfenced hazards, and
            put behind a fence
        */
        void addFence( size_t from, size_t to, bool forceFence );

        void deleteFences( MetalFenceVec::iterator itor, MetalFenceVec::iterator end );

        /// Waits undefinitely on the given GLSync object. Can throw if failed to wait.
        void wait( dispatch_semaphore_t syncObj );

        /// Checks all the fences and compares them against current state (i.e. mMappingStart &
        /// mMappingCount), and stalls if needed (synchronize); also book-keeps mFences and
        /// mUnfencedHazards.
        /// May modify mMappingStart.
        void waitIfNeeded(void);

        virtual void* writeMapImpl( size_t sizeBytes );
        virtual void unmapImpl( const Operation *destinations, size_t numDestinations );

        virtual const void* readMapImpl( size_t offset, size_t sizeBytes );

        id<MTLBuffer> mVboName;
        void * mMappedPtr;
        MetalDevice * mDevice;

        /** How many bytes between the last fence and our current offset do we need to let
            through before we place another fence?
        @remarks
            When the ring buffer wraps around, a fence is always put.
            A threshold of zero means to put a fence after every unmap operation.
        */
        size_t mFenceThreshold;

        struct MetalFence : Block
        {
            dispatch_semaphore_t    fenceName;

            MetalFence( size_t _start, size_t _end ) :
                Block( _start , _end ), fenceName( 0 )
            {
            }
        };

        //------------------------------------
        // Begin used for uploads
        //------------------------------------
        typedef vector<MetalFence>::type MetalFenceVec;
        MetalFenceVec mFences;

        /// Regions of memory that were unmapped but haven't
        /// been fenced due to not passing the threshold yet.
        MetalFenceVec mUnfencedHazards;
        size_t mUnfencedBytes;
    };
}
#endif