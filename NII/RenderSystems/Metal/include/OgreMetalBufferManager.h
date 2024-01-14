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

#ifndef _OgreMetalHardwareBufferManager_H_
#define _OgreMetalHardwareBufferManager_H_

#include "OgreMetalPrerequisites.h"
#include "OgreHardwareBufferManager.h"
#include "Vao/OgreGBufferManager.h"

#import <dispatch/dispatch.h>

@protocol MTLComputePipelineState;

namespace Ogre 
{
    class MetalDiscardBuffer;
    typedef vector<MetalDiscardBuffer *>::type MetalDiscardBufferVec;

    /// Metal doesn't support "DISCARD" like D3D9/D3D11 (and OpenGL but often it's broken)
    /// where we requested to map a write-only buffer and the API would discard the previous
    /// contents (thus allowing us to avoid a stall until the GPU is done with the region)
    ///
    /// We need Discard for the v1 interfaces. So we need to emulate it.
    /// This class does exactly this.
    class _OgreMetalExport MetalDiscardBufferManager : public BufferAlloc
    {
    public:
        struct UnsafeBlock : public MetalGBufferManager::Block
        {
            UnsafeBlock( size_t _offset, size_t _size, uint32 _lastFrameUsed ) :
                MetalGBufferManager::Block(_offset, _size), lastFrameUsed( _lastFrameUsed ) {}

            bool operator < (const UnsafeBlock & other) const
            {
                return this->lastFrameUsed < other.lastFrameUsed;
            }
            
            uint32 lastFrameUsed;
        };
        typedef vector<UnsafeBlock>::type UnsafeBlockVec;
    public:
        MetalDiscardBufferManager( MetalDevice * device, GBufferManager * vaoManager );
        ~MetalDiscardBufferManager();

        void _notifyDeviceStalled(void);

        /** For internal use. Retrieves a fresh new block. The old block will be returned
            to a pool until it's safe to reuse again. Used by the DiscardBuffer when
            map( DISCARD ) is called.
        @param discardBuffer
            Buffer to assign a new region.
        */
        void _getBlock( MetalDiscardBuffer * discardBuffer );

        /** Creates a buffer that supports discarding to hold the required size.
        @param bufferSize
            Requested size in bytes.
        @param alignment
            Alignment requirements.
        @return
            A new MetalDiscardBuffer
        */
        MetalDiscardBuffer * createDiscardBuffer( size_t bufferSize, uint16 alignment );

        /** Destroys an existing MetalDiscardBuffer, releasing its memory.
        @param discardBuffer
            MetalDiscardBuffer to destroy
        */
        void destroyDiscardBuffer( MetalDiscardBuffer * discardBuffer );

        MetalDevice * getDevice(void) const          { return mDevice; }
        GBufferManager * getGBufferManager(void) const       { return mMag; }
    private:
        id<MTLBuffer> mBuffer;
        MetalDevice * mDevice;
        GBufferManager * mMag;
        MetalGBufferManager::BlockVec mFreeBlocks;

        UnsafeBlockVec  mUnsafeBlocks;

        MetalDiscardBufferVec mDiscardBuffers;

        /** Moves our current mBuffer into a new one (bigger one). Used when we've ran
            out of usable space. This operation can be slow and will increase GPU memory
            consumption. Until the old buffer stops being used, memory usage will stay
            higher than normal.
        @param extraBytes
            Extra bytes are required. We may grow bigger than that, but no less.
        @param forDiscardBuffer
            Optional. The block owned by the input won't be copied from old buffer
            into the new one, so that it can be reused. Can be null.
            @see _getBlock's usage code for reference.
        */
        void growToFit( size_t extraBytes, MetalDiscardBuffer *forDiscardBuffer );

        /// Puts unsafe blocks that are now safe back to the free blocks pool.
        void updateUnsafeBlocks(void);
    };

    class _OgreMetalExport MetalDiscardBuffer : public BufferAlloc
    {
        friend class MetalDiscardBufferManager;
    public:
        MetalDiscardBuffer( size_t bufferSize, uint16 alignment, GBufferManager *vaoManager, MetalDiscardBufferManager *owner );

        /** Returns a pointer that maps to the beginning of this buffer to begin writing.
        @param noOverwrite
            When true, noOverwrite is slow
        @return
        */
        void * map( bool noOverwrite );
        void unmap(void);

        uint16 getAlignment(void) const         { return mAlignment; }
        /// Size of the buffer, may be bigger than requested due to 4-byte alignment required by Metal.
        size_t getSizeBytes(void) const         { return mBufferSize; }

        size_t getOffset(void) const            { return mBufferOffset; }
        /** Returns the actual API buffer, but first sets mLastFrame as we
            assume you're calling this function to use the buffer in the GPU.
        @param outOffset
            Out. Guaranteed to be written. Used to point to the start
            of our data in the internal ring buffer we've allocated.
        @return
            The MTLBuffer in question.
        */
        id<MTLBuffer> getBufferName( size_t &outOffset );

        /// For internal use by MetalDiscardBufferManager
        size_t getBlockStart(void) const        { return mBufferOffset - mBlockPrePadding; }
        size_t getBlockSize(void) const         { return mBufferSize + mBlockPrePadding; }

        MetalDiscardBufferManager * getOwner(void)   { return mOwner; }
    protected:
        __unsafe_unretained id<MTLBuffer> mBuffer;
        size_t mBlockPrePadding;
        size_t mBufferOffset;
        size_t mBufferSize;

        uint16 mAlignment;
        uint32 mLastFrame;

        GBufferManager * mMag;
        MetalDiscardBufferManager * mOwner;
    };
    
    /// Implementation of GBufferManager for Metal.
    class _OgreMetalExport MetalBufferManagerBase : public GBufferManager
    {
    public:
        MetalBufferManagerBase( MetalDevice *device, GBufferManager *vaoManager );
        virtual ~MetalBufferManagerBase();

        void _notifyDeviceStalled(void);

        MetalDiscardBufferManager * _getDiscardBufferManager(void)   { return mDiscardBufferManager; }

        /// Creates a vertex buffer
        virtual HardwareVertexBufferSharedPtr createVertexBuffer( size_t vertexSize, size_t numVerts, Buffer::Usage usage, bool useShadowBuffer = false );

        /// Create an index buffer
        virtual HardwareIndexBufferSharedPtr createIndexBuffer(IndexBuffer::IndexType itype, size_t numIndexes, Buffer::Usage usage, bool useShadowBuffer = false );

        /// Create a uniform buffer
        virtual StructBuffer * createUniformBuffer( size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name = "");
        
        /// Create a counter buffer
        virtual CounterBuffer * createCounterBuffer( size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name = "" );
    public:
        struct Block
        {
            NCount offset;
            NCount size;

            Block( NCount _offset, NCount _size ) : offset( _offset ), size( _size ) {}
        };
        struct StrideChanger
        {
            size_t offsetAfterPadding;
            size_t paddedBytes;

            StrideChanger() : offsetAfterPadding( 0 ), paddedBytes( 0 ) {}
            StrideChanger( size_t _offsetAfterPadding, size_t _paddedBytes ) :
                offsetAfterPadding( _offsetAfterPadding ), paddedBytes( _paddedBytes ) {}

            bool operator () ( const StrideChanger &left, size_t right ) const
            {
                return left.offsetAfterPadding < right;
            }
            bool operator () ( size_t left, const StrideChanger &right ) const
            {
                return left < right.offsetAfterPadding;
            }
            bool operator () ( const StrideChanger &left, const StrideChanger &right ) const
            {
                return left.offsetAfterPadding < right.offsetAfterPadding;
            }
        };

        typedef vector<Block>::type BlockVec;
        typedef vector<StrideChanger>::type StrideChangerVec;

    public:
        MetalGBufferManager( uint8 dynamicBufferMultiplier, MetalDevice * device, const PropertyData * params);
        virtual ~MetalGBufferManager();

        virtual void getMemoryStats(MemoryStatsList & outStats, size_t & outCapacityBytes, size_t & outFreeBytes, Log * log) const;

        virtual void recoverPool(void);

        /// Binds the Draw ID to the current RenderEncoder. (Assumed to be active!)
        void bindDrawId(void);

        /** Creates a new staging buffer and adds it to the pool. @see getStagingBuffer.
        @remarks
            The returned buffer starts with a reference count of 1. You should decrease
            it when you're done using it.
        */
        virtual void createImpl(MappedBuffer *& buf, NCount sizeBytes, Nmark mode );

        virtual void create(GpuBufferAsync *& out, GpuBuffer *creator, MappedBuffer * buf, NCount oft, NCount size );

        MetalDevice* getDevice(void)        { return mDevice; }

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        /// In macOS before Catalina (i.e. <= Mojave), MTLBlitCommandEncoder copyFromBuffer
        /// must be aligned to 4 bytes. When that's not possible, we have to workaround
        /// this limitation with a compute shader
        void unalignedCopy( id<MTLBuffer> dstBuffer, size_t dstOffsetBytes, id<MTLBuffer> srcBuffer, size_t srcOffsetBytes, size_t sizeBytes );
#endif

        virtual void _update(void);
        void _notifyNewCommandBuffer(void);
        void _notifyDeviceStalled(void);

        /// @see GBufferManager::wait
        virtual uint8 wait(void);

        /// See GBufferManager::wait
        virtual void wait( uint32 frameCount );

        /// See GBufferManager::isFrameFinished
        virtual bool isFrameFinished( uint32 frameCount );

        /** Will stall undefinitely until GPU finishes (signals the sync object).
        @param fenceName
            Sync object to wait for. Will be deleted on success. On failure,
            throws an exception and fenceName will not be deleted.
        @param device
            Device this fence is linked to.
        @returns
            Null ptr on success. Should throw on failure, but if this function for some
            strange reason doesn't throw, it is programmed to return 'fenceName'
        */
        static dispatch_semaphore_t waitFor( dispatch_semaphore_t fenceName, MetalDevice *device );

        static uint32 getAttributeIndexFor( VertexType vtype );
        
        /// @see MappedBuffer::mergeContiguousBlocks
        static void mergeContiguousBlocks( BlockVec::iterator blockToMerge, BlockVec &blocks );
    protected:
        enum VboFlag
        {
            CPU_INACCESSIBLE,
            CPU_ACCESSIBLE_DEFAULT,
            CPU_ACCESSIBLE_PERSISTENT,
            CPU_ACCESSIBLE_PERSISTENT_COHERENT,
            VF_MAX
        };

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        void createUnalignedCopyShader(void);
#endif

        /** Asks for allocating buffer space in a VBO (Vertex Buffer Object).
            If the VBO doesn't exist, all VBOs are full or can't fit this request,
            then a new VBO will be created.
        @remarks
            Can throw if out of video memory
        @param sizeBytes
            The requested size, in bytes.
        @param bytesPerElement
            The number of bytes per vertex or per index (i.e. 16-bit indices = 2).
            Cannot be 0.
        @param bufferType
            The type of buffer
        @param outVboIdx [out]
            The index to the mVbos.
        @param outBufferOffset [out]
            The offset in bytes at which the buffer data should be placed.
        */
        void allocateVbo( size_t sizeBytes, size_t alignment, BufferModeMark bufferType, size_t &outVboIdx, size_t &outBufferOffset );

        /** Deallocates a buffer allocated with @allocateVbo.
        @remarks
            All four parameters *must* match with the ones provided to or
            returned from allocateVbo, otherwise the behavior is undefined.
        @param vboIdx
            The index to the mVbos pool that was returned by allocateVbo
        @param bufferOffset
            The buffer offset that was returned by allocateVbo
        @param sizeBytes
            The sizeBytes parameter that was passed to allocateVbos.
        @param bufferType
            The type of buffer that was passed to allocateVbo.
        */
        void deallocateVbo( size_t vboIdx, size_t bufferOffset, size_t sizeBytes, BufferModeMark bufferType );
    protected:
        virtual VertexBufferPacked * createVertexBufferImpl( size_t numElements, uint32 bytesPerElement, BufferModeMark bufferType, void *initialData, bool keepAsShadow, const VertexUnitList &units );

        virtual void destroyVertexBufferImpl( VertexBufferPacked *vertexBuffer );

        virtual MultiSourceVertexBufferPool * createMultiSourceVertexBufferPoolImpl(const VertexUnitListList &vertexElementsBySource, size_t maxNumVertices, size_t totalBytesPerVertex, BufferModeMark bufferType );

        virtual IndexBufferPacked * createIndexBufferImpl( size_t numElements, uint32 bytesPerElement, BufferModeMark bufferType, void *initialData, bool keepAsShadow );

        virtual void destroyIndexBufferImpl( IndexBufferPacked * indexBuffer );

        virtual StructBuffer * createConstBufferImpl( size_t sizeBytes, BufferModeMark bufferType, void *initialData, bool keepAsShadow );
        virtual void destroyConstBufferImpl( StructBuffer * constBuffer );

        virtual TextureBuffer * createTexBufferImpl( PixelFormat pf, size_t sizeBytes, BufferModeMark bufferType, void * initialData, bool keepAsShadow );
        virtual void destroyTexBufferImpl( TextureBuffer * texBuffer );

        virtual StorageBuffer * createUavBufferImpl( size_t numElements, uint32 bytesPerElement, uint32 bindFlags, void * initialData, bool keepAsShadow );
        virtual void destroyUavBufferImpl( StorageBuffer * uavBuffer );

        virtual IndirectBuffer * createIndirectBufferImpl( size_t sizeBytes, BufferModeMark bufferType, void * initialData, bool keepAsShadow );
        virtual void destroyIndirectBufferImpl( IndirectBuffer *indirectBuffer );

        virtual GeometryRaw * createVertexArrayObjectImpl(const VertexBufferList &vertexBuffers, IndexBufferPacked * indexBuffer, OperationType opType );

        virtual void destroyVertexArrayObjectImpl( GeometryRaw * vao );

        /// Finds the Vao. Calls createVao automatically if not found.
        /// Increases refCount before returning the iterator.
        VaoVec::iterator findVao( const VertexBufferList & vertexBuffers, IndexBufferPacked * indexBuffer, OperationType opType );
        
        uint32 createVao( const Vao &vaoRef );

        static uint32 generateRenderQueueId( uint32 vaoName, uint32 uniqueVaoId );

        static VboFlag modeToFlag(BufferModeMark bufferType );

        inline void getMemoryStats( const Block &block, size_t vboIdx, size_t poolCapacity, LwString &text, MemoryStatsList &outStats, Log *log ) const;

        //virtual void switchVboPoolIndexImpl( size_t oldPoolIdx, size_t newPoolIdx, GpuBuffer *buffer );
    protected:
        struct Vbo
        {
            id<MTLBuffer> vboName;
            size_t sizeBytes;
            MetalBuffer * mBuffer; //Null for CPU_INACCESSIBLE BOs.

            BlockVec mFreeList;
            StrideChangerVec strideChangers;
        };

        struct Vao
        {
            uint32 vaoName;

            struct VertexBinding
            {
                __unsafe_unretained id<MTLBuffer> vertexBufferVbo;
                VertexUnitList mUnits;

                //OpenGL supports this parameter per attribute, but
                //we're a bit more conservative and do it per buffer
                uint32 instancingDivisor;

                bool operator == ( const VertexBinding &_r ) const
                {
                    return vertexBufferVbo == _r.vertexBufferVbo && mUnits == _r.mUnits &&
                        instancingDivisor == _r.instancingDivisor;
                }
            };

            typedef vector<VertexBinding>::type VertexBindingVec;

            /// Not used anymore, however it's useful for sorting
            /// purposes in the RenderQueue (using the Vao's ID).
            OperationType mType;
            VertexBindingVec vertexBuffers;
            __unsafe_unretained id<MTLBuffer> indexBufferVbo;
            IndexBufferPacked::IndexType indexType;
            uint32 refCount;
        };

        typedef vector<Vbo>::type VboVec;
        typedef vector<Vao>::type VaoVec;
        typedef vector<dispatch_semaphore_t>::type DispatchSemaphoreVec;
        typedef vector<uint8>::type DispatchSemaphoreAlreadyWaitedVec;

        VboVec  mVbos[VF_MAX];
        size_t  mDefaultPoolSize[VF_MAX];

        VaoVec  mVaos;
        uint32  mVaoNames;

        bool mSemaphoreFlushed;
        DispatchSemaphoreAlreadyWaitedVec mAlreadyWaitedForSemaphore;
        DispatchSemaphoreVec mFrameSyncVec;

        MetalDevice *mDevice;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        StructBuffer   *mDrawId;
#else
        VertexBufferPacked  *mDrawId;

        id<MTLComputePipelineState> mUnalignedCopyPso;
#endif

        static const uint32 VERTEX_ATTRIBUTE_INDEX[VES_COUNT];
    protected:
        MetalDiscardBufferManager *mDiscardBufferManager;
    };

    /// MetalBufferManagerBase as a Singleton
    class _OgreMetalExport MetalBufferManager : public BufferManager
    {
    public:
        MetalBufferManager( MetalDevice *device, GBufferManager *vaoManager ) :
            GBufferManager( N_new MetalBufferManagerBase( device, vaoManager ) )
        {
        }
        virtual ~MetalBufferManager()
        {
            N_delete mImpl;
        }

        void _notifyDeviceStalled(void) { static_cast<MetalBufferManagerBase*>(mImpl)->_notifyDeviceStalled(); }
    };
}

#endif
