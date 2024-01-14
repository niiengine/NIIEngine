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
#ifndef __D3D11HARDWAREBUFFER_H__
#define __D3D11HARDWAREBUFFER_H__

#include "OgreD3D11Prerequisites.h"
#include "OgreHardwareBuffer.h"
#include "OgreHardwareIndexBuffer.h"
#include "OgreDescriptorSetTexture.h"
#include "OgreD3D11Driver.h"
#include "Vao/OgreUavBufferPacked.h"
#include "OgreDescriptorSetUav.h"

struct ID3D11Resource;

namespace NII 
{
    /** D3D11 doesn't support mapping the same buffer twice even if the regions
        don't overlap. For performance we keep many buffers as one big buffer,
        but for compatibility reasons (with GL3/DX10 HW) we treat them as
        separate buffers.
    @par
        This class takes care of mapping buffers once while allowing
        BufferOp to map subregions of it as if they were separate
        buffers.
    @remarks
        This is a very thin/lightweight wrapper around D3D11's Map, thus:
            Caller is responsible for unmapping.
            Caller is responsible for proper synchronization.
            No check is performed to see if two map calls overlap.
    */
    class _OgreD3D11Export D3D11Buffer
    {
    public:
        D3D11Buffer( ID3D11Buffer * vboName, NCount size, D3D11Device & device );
        ~D3D11Buffer();

        ID3D11Buffer * getVboName() const               { return mVboName.Get(); }

        /// Assumes mVboName is already bound to GL_COPY_WRITE_BUFFER!!!
        void * map(NCount start, NCount count, NCount & outTicket);

        /// Unmaps given ticket (got from @see map).
        /// Assumes mVboName is already bound to GL_COPY_WRITE_BUFFER!!!
        /// The ticket becomes invalid after this.
        void unmap(NCount ticket);
    protected:
        NCount addMappedRange(NCount start, NCount count);
        
        struct MappedRange
        {
            NCount start;
            NCount count;

            MappedRange(NCount _start, NCount _count) : start( _start ), count( _count ) {}
        };

        typedef vector<MappedRange>::type MappedRangeVec;

        ComPtr<ID3D11Buffer> mVboName;
        NCount mVboSize;
        void * mMappedPtr;

        D3D11Device & mDevice;

        MappedRangeVec mMappedRanges;
        vector<NCount>::type mFreeRanges;
    };
    
    /** For D3D11, most (if not all) buffers, can be treated with the same code.
        Hence most equivalent functionality is encapsulated here.
    */
    class _OgreD3D11Export D3D11BufferOpBase : public BufferOp
    {
    public:
        D3D11BufferOpBase(size_t vboPoolIdx, ID3D11Buffer * d3dBuffer);
        ~D3D11BufferOpBase();

        inline size_t getVboPoolIndex(void)                     { return mGroupIndex; }

        inline ID3D11Buffer * getVboName(void) const            { return mVboName.Get(); }

        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size);

        virtual bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true);

        virtual void waitImpl(void);
    protected:
        void splicedCopy( size_t dstOffsetBytes, size_t srcOffsetBytes, size_t sizeBytes, size_t alignment, 
            ID3D11Buffer *dstBuffer, ID3D11Buffer *srcBuffer, ID3D11DeviceContextN *context );
    protected:
        size_t mGroupIndex;
        ComPtr<ID3D11Buffer> mVboName;
        void * mMappedPtr;
    };
    
    /** For D3D11, most (if not all) buffers, can be treated with the same code.
        Hence most equivalent functionality is encapsulated here.
    */
    class _OgreD3D11Export D3D11BufferOp : public D3D11BufferOpBase
    {
    public:
        D3D11BufferOp( size_t vboPoolIdx, ID3D11Buffer * d3dBuffer, D3D11Buffer * dynamicBuffer );
        ~D3D11BufferOp();

        inline D3D11Buffer * getDynamicBuffer(void) const       { return mDynamicBuffer; }

        inline void _setVboPoolIndex( size_t newVboPool )       { mGroupIndex = newVboPool; }

        /// Only use this function for the first upload
        void _init( void * data );

        inline const void * _getInitialData(void) const         { return mInitialData; }
        void _deleteInitialData(void);
        void _setVboName( size_t vboPoolIdx, ID3D11Buffer * vboName, size_t internalBufferStartBytes );

        virtual void * lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx = 0);
        virtual void unlockImpl(UnlockType type, NCount oft, NCount size);
        bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true);
        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size);
    protected:
        size_t mUnmapTicket;
        D3D11Buffer * mDynamicBuffer;

        /// Used to store initial data of M_DEVREAD buffers and delay their
        /// creation as much as possible (so that we can batch them together)
        void *mInitialData;

        size_t advanceFrame( bool bAdvanceFrame );
    };
    
    /** In D3D11, const buffers can't be bound by offset. Their size at
        creation defines the size to be bound to the shader, and the max
        limit is 64kb.
        This is unless we're using D3D11.1 on Windows 8.1; which for simplicity
        we treat it the same as D3D11.
    @par
        This limitation prevents us from allocating 3x size for triple buffering.
    @par
        Therefore we need the traditional scheme of mapping with NO_OVERWRITE
        and then use DISCARD when we've filled the buffer.
    @par
        In D3D11.0, only index and vertex buffers can be mapped with NO_OVERWRITE.
        That leaves only DISCARD to be used on those systems.
        NO_OVERWRITE is used if used D3D 11.1 though.
    @par
        This buffer interface is for compatibility with these systems.
    */
    class _OgreD3D11Export D3D11CompatBufferInterface : public D3D11BufferOpBase
    {
    public:
        D3D11CompatBufferInterface( size_t vboPoolIdx, ID3D11Buffer * d3dBuffer, D3D11Device &device );
        ~D3D11CompatBufferInterface();

        virtual void * lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx = 0);
        virtual void unlockImpl(UnlockType type, NCount oft, NCount size);
        bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true);
        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size);
        virtual void advanceFrame(void);
        virtual void regressFrame(void);
    protected:
        D3D11Device & mDevice;
    };
    
    /** Base implementation of a D3D11 buffer, dealing with all the common
    aspects.
    */
    class _OgreD3D11Export D3D11HardwareBuffer : public Buffer
    {
    public:
        enum BufferType
        {
            VERTEX_BUFFER,
            INDEX_BUFFER,
            CONSTANT_BUFFER
        };
    public:
        D3D11HardwareBuffer(BufferType btype, size_t sizeBytes, Buffer::Usage usage, 
            D3D11Device & device, bool useSystemMem, bool useShadowBuffer, bool streamOut);
            
        ~D3D11HardwareBuffer();
        
        /** See Buffer. */
        void readData(size_t offset, size_t length, void* pDest);
        
        /** See Buffer. */
        void writeData(size_t offset, size_t length, const void* pSource, bool discardWholeBuffer = false);
        
        /** See Buffer. We perform a hardware copy here. */
        void copyData(Buffer& srcBuffer, size_t srcOffset,  size_t dstOffset, size_t length, bool discardWholeBuffer = false);

		void copyDataImpl(Buffer& srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer = false);

		/// Updates the real buffer from the shadow buffer, if required
		virtual void _updateFromShadow(void);

        /// Get the D3D-specific buffer
        ID3D11Buffer* getD3DBuffer(void) { return mlpD3DBuffer.Get(); }
    protected:
        /** See Buffer. */
        void* lockImpl(size_t offset, size_t length, LockOptions options);
        /** See Buffer. */
        void unlockImpl(void);
        
        ComPtr<ID3D11Buffer> mlpD3DBuffer;
        bool mUseTempStagingBuffer;
        D3D11HardwareBuffer* mpTempStagingBuffer;
        bool mStagingUploadNeeded;
        BufferType mBufferType;
        D3D11Device & mDevice;
        D3D11_BUFFER_DESC mDesc;
    };
    
    class _OgreD3D11Export D3D11IndexBuffer : public IndexBuffer
    {
    public:
        D3D11IndexBuffer(GBufferManager * mgr, IndexType idxType, size_t numIndexes, 
            Buffer::Usage usage, D3D11Device & device, bool useSystemMem, bool useShadowBuffer);
        ~D3D11IndexBuffer();

        // override all data-gathering methods
        void * lock(size_t offset, size_t length, LockOptions options);

        void unlock(void);

        void readData(size_t offset, size_t length, void * pDest);

        void writeData(size_t offset, size_t length, const void * pSource, bool discardWholeBuffer = false);

        void copyData(Buffer & srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer = false);
        bool isLocked(void) const;

        /// Get the D3D-specific index buffer
        ID3D11Buffer * getD3DIndexBuffer(void) const;
    protected:
        // have to implement these, but do nothing as overridden lock/unlock
        void * lockImpl(size_t offset, size_t length, LockOptions options)       { return 0; }
        
        void unlockImpl(void) {}
        
        D3D11HardwareBuffer* mBufferImpl;
    };
    
    /// Specialisation of VertexBuffer for D3D11
    class _OgreD3D11Export D3D11VertexBuffer : public VertexBuffer
    {
    public:
        D3D11VertexBuffer(GBufferManager * mgr, size_t vertexSize, size_t numVertices, 
            Buffer::Usage usage, D3D11Device & device, bool useSystemMem, bool useShadowBuffer, bool streamOut);
        ~D3D11VertexBuffer();

        // override all data-gathering methods
        void * lock(size_t offset, size_t length, LockOptions options);
        
        void unlock(void);
        
        void readData(size_t offset, size_t length, void * pDest);
        
        void writeData(size_t offset, size_t length, const void * pSource, bool discardWholeBuffer = false);

        void copyData(Buffer & srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer = false);
        
        bool isLocked(void) const;

        /// Get the D3D-specific vertex buffer
        ID3D11Buffer * getD3DVertexBuffer(void) const;
    protected:
        // have to implement these, but do nothing as overridden lock/unlock
        void* lockImpl(size_t offset, size_t length, LockOptions options)       {return 0;}
        
        void unlockImpl(void) {}
        
        D3D11HardwareBuffer * mBufferImpl;
    };
    
    class _OgreD3D11Export D3D11FrameBuffer : public FrameBuffer
    {
    public:
        D3D11FrameBuffer(D3D11Texture * parentTexture, D3D11Device & device, UINT mipLevel,
            size_t width, size_t height, size_t depth, UINT face, PixelFormat format, Buffer::Usage usage);

        ~D3D11FrameBuffer();

        /// @copydoc FrameBuffer::blit
        void blit(const HardwarePixelBufferSharedPtr &src, const Box &srcBox, const Box &dstBox);

        /// @copydoc FrameBuffer::blitFromMemory
        void blitFromMemory(const PixelBox &src, const Box &dstBox);

        /// @copydoc FrameBuffer::blitToMemory
        void blitToMemory(const Box &srcBox, const PixelBox &dst);

        /// Internal function to update mipmaps on update of level 0
        void _genMipmaps();

        /// Get rendertarget for z slice
        RenderTexture *getRenderTarget(size_t zoffset);

        /// Notify TextureBuffer of destruction of render target
        virtual void destroyImpl(size_t zoffset)
        {
            if (mSliceTRT.size() > zoffset)
            {
                mSliceTRT[zoffset] = 0;
            }
        }

        D3D11Texture * getParentTexture() const;

        UINT getFace() const;
        UINT getSubresourceIndex(size_t box_front) const;
        D3D11_BOX getSubresourceBox(const Box &box) const;
    protected:
        /// Lock a box
        PixelBox lockImpl(const Box &lockBox, LockOptions options);

        /// Unlock a box
        void unlockImpl(void);
        
        void createStagingBuffer();
        bool mUsingStagingBuffer;
        
        void _map(ID3D11Resource *res, D3D11_MAP flags, PixelBox & box);
        void _mapstagingbuffer(D3D11_MAP flags, PixelBox &box);

        void _unmap(ID3D11Resource *res);
        void _unmapstagingbuffer(bool copyback = true);
        void _unmapstaticbuffer();

        /// D3DDevice pointer
        D3D11Device & mDevice;

        D3D11Texture * mParentTexture;
        const UINT mFace;
        const UINT mMipLevel;

        // if the usage is static - alloc at lock then use device UpdateSubresource when unlock and free memory
        std::vector<int8> mDataForStaticUsageLock; 

        LockOptions mCurrentLockOptions;
        ComPtr<ID3D11Resource> mStagingBuffer;
        /// Render targets
        typedef std::vector<RenderTexture*> SliceTRT;
        SliceTRT mSliceTRT;
    };
    
    class _OgreD3D11Export D3D11StructBuffer : public StructBuffer
    {
    public:
        D3D11StructBuffer( size_t numElements, uint32 bytesPerElement, uint32 numElementsPadding,
                                BufferModeMark bufferType, void * initialData, bool keepAsShadow,
                                    GBufferManager * vaoManager, BufferOp * bufferInterface, D3D11Device & device );
        virtual ~D3D11StructBuffer();

        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft = 0, NCount size = 0);
    protected:
        D3D11Device & mDevice;
    };
    
    class _OgreD3D11Export D3D11TextureBuffer : public TextureBuffer
    {
    public:
        D3D11TextureBuffer( size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
                              uint32 numElementsPadding, BufferModeMark bufferType,
                              void *initialData, bool keepAsShadow,
                              GBufferManager *vaoManager, BufferOp *bufferInterface,
                              PixelFormat pf, bool bIsStructured, D3D11Device &device );
        virtual ~D3D11TextureBuffer();

        ComPtr<ID3D11ShaderResourceView> createSrv( const ShaderChTexture::BufferSlot &bufferSlot ) const;

        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft = 0, NCount size = 0);
    protected:
        bool isD3D11Structured(void) const;

        ID3D11ShaderResourceView* createResourceView( int cacheIdx, uint32 offset, uint32 sizeBytes );
        ID3D11ShaderResourceView* bindBufferCommon( size_t offset, size_t sizeBytes );
        
        DXGI_FORMAT mInternalFormat;
        D3D11Device &mDevice;

        struct CachedResourceView
        {
            ComPtr<ID3D11ShaderResourceView> mResourceView;
            uint32 mOffset;
            uint32 mSize;
        };

        CachedResourceView mCachedResourceViews[16];
        uint8 mCurrentCacheCursor;
    };
    
    class _OgreD3D11Export D3D11StorageBuffer : public StorageBuffer
    {
    public:
        D3D11StorageBuffer( size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
                              uint32 bindFlags, void *initialData, bool keepAsShadow,
                              GBufferManager *vaoManager, BufferOp *bufferInterface,
                              D3D11Device &device );
        virtual ~D3D11StorageBuffer();

        ID3D11UnorderedAccessView * _bindBufferCommon( size_t offset, size_t sizeBytes );

        ComPtr<ID3D11UnorderedAccessView> createUav( const DescriptorSetUav::BufferSlot & bufferSlot ) const;

        void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft = 0, NCount size = 0);
    protected:
        virtual TextureBuffer * createViewImpl(PixelFormat pf);

        ID3D11UnorderedAccessView * createResourceView(int cacheIdx, uint32 offset, uint32 sizeBytes);

        D3D11Device & mDevice;

        struct CachedResourceView
        {
            ComPtr<ID3D11UnorderedAccessView> mResourceView;
            uint32 mOffset;
            uint32 mSize;
        };

        CachedResourceView mCachedResourceViews[16];
        uint8 mCurrentCacheCursor;
    };
    
    /** NOTE FOR D3D12 PORTING: The GL3Plus implementation should be used as reference, since it
        is much lower level than this implementation for D3D11.

        A staging buffer is a buffer that resides on the GPU and be written to/from both CPU & GPU
        However the access in both cases is limited. GPUs can only copy (i.e. memcpy) to another
        real buffer (can't be used directly as i.e. texture or vertex buffer) and CPUs can only
        map it.
        In other words, a staging buffer is an intermediate buffer to transfer data between
        CPU & GPU
    */
    class _OgreD3D11Export D3D11MappedBuffer : public MappedBuffer
    {
    public:
        D3D11MappedBuffer(size_t sizeBytes, GBufferManager * vaoManager, BufferModeMark mode, ID3D11Buffer *stagingBuffer, D3D11Device &device );
        virtual ~D3D11MappedBuffer();

        virtual BlockType request( size_t sizeBytes, bool upload = true);

        virtual size_t async( OperationList & oplist, bool upload);

        ID3D11Buffer* getBufferName(void) const     { return mVboName.Get(); }
    protected:
        //------------------------------------
        // End used for uploads
        //------------------------------------

        /// Checks all the fences and compares them against current state (i.e. mMappingStart &
        /// mMappingCount), and stalls if needed (synchronize); also book-keeps mFences and
        /// mUnfencedHazards.
        /// May modify mMappingStart.
        void waitIfNeeded(void);

        virtual void* writeMapImpl( size_t sizeBytes );
        virtual void unmapImpl();

        virtual const void* readMapImpl( size_t offset, size_t sizeBytes );
        
        /// mVboName is not deleted by us (the GBufferManager does) as we may have
        /// only been assigned a chunk of the buffer, not the whole thing.
        ComPtr<ID3D11Buffer> mVboName;
        void * mMappedPtr;

        D3D11Device & mDevice;

        //------------------------------------
        // Begin used for uploads
        //------------------------------------
        /*typedef vector<GLFence>::type GLFenceVec;
        GLFenceVec mFences;

        /// Regions of memory that were unmapped but haven't
        /// been fenced due to not passing the threshold yet.
        GLFenceVec mUnfencedHazards;*/
    };
    
    class _OgreD3D11Export D3D11MappedTexture : public MappedTexture
    {
    public:
        D3D11MappedTexture( GBufferManager *vaoManager, PixelFormat formatFamily,
                             uint32 width, uint32 height, uint32 depthOrSlices, D3D11Device &device );
        virtual ~D3D11MappedTexture();

        virtual bool request(const PixelVolume & vol ) const;

        virtual void unmap(void);

        virtual void read(Texture * dst, NCount dstmipmap, const PixelBlock * dstBox, const PixelBlock & box, bool host=false );
        virtual void write(Texture * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host=false );
    protected:
        uint32 findRealSlice( void *data ) const;

        virtual bool belongsToUs( const PixelBlock & box );

        void shrinkRecords( size_t slice, StagingBoxVec::iterator record, PixelBlock consumedBox );
        void shrinkMultisliceRecords( size_t slice, StagingBoxVec::iterator record, const PixelBlock &consumedBox );
        PixelBlock mapMultipleSlices( const PixelVolume & vol );
        virtual PixelBlock mapImpl( const PixelVolume & vol );
    protected:
        typedef vector<D3D11_MAPPED_SUBRESOURCE>::type D3D11_MAPPED_SUBRESOURCEVec;

        /// mStagingTexture is either a 3D texture (if depthOrSlices > 1 and
        /// resolution is below 2048x2048) or else it's a 2D array texture.
        /// Fortunately D3D11 allows us to copy slices of 3D textures into
        /// 2D textures; so we use 3D Staging Textures whenever possible
        /// because they can be mapped like in other APIs.
        ComPtr<ID3D11Resource> mStagingTexture;
        D3D11_MAPPED_SUBRESOURCEVec mSubresourceData;
        D3D11_MAPPED_SUBRESOURCEVec mLastSubresourceData;
        uint32 mWidth;
        uint32 mHeight;
        uint32 mDepthOrSlices;
        bool mIsArray2DTexture;
        D3D11Device &mDevice;

        struct StagingBox
        {
            uint32 x;
            uint32 y;
            uint32 width;
            uint32 height;
            StagingBox() {}
            StagingBox( uint32 _x, uint32 _y, uint32 _width, uint32 _height ) :
                x( _x ), y( _y ), width( _width ), height( _height ) {}

            bool contains( uint32 _x, uint32 _y, uint32 _width, uint32 _height ) const
            {
                return !( _x >= this->x + this->width ||
                          _y >= this->y + this->height ||
                          this->x >= _x + _width ||
                          this->y >= _y + _height );
            }
        };
        typedef vector<StagingBox>::type StagingBoxVec;
        typedef vector<StagingBoxVec>::type StagingBoxVecVec;

        StagingBoxVecVec mFreeBoxes;
    };
    
    struct _OgreD3D11Export D3D11VertexArrayObjectShared
    {
        D3D11VertexArrayObjectShared( const VertexBufferList &vertexBuffers, IndexBufferPacked *indexBuffer,
                                      OperationType opType, VertexBufferPacked *drawId );
                                      
        ComPtr<ID3D11Buffer> mVertexBuffers[16];
        UINT mStrides[16];
        UINT mOffsets[16];
        ComPtr<ID3D11Buffer> mIndexBuffer;
        DXGI_FORMAT mIndexFormat;
    };

    struct _OgreD3D11Export D3D11VertexArrayObject : public GeometryRaw
    {
        D3D11VertexArrayObject( uint32 vaoName, uint32 renderQueueId, uint16 inputLayoutId,
                                const VertexBufferList &vertexBuffers,
                                IndexBufferPacked *indexBuffer,
                                OperationType opType,
                                D3D11VertexArrayObjectShared *sharedData ) :
            GeometryRaw( vaoName, renderQueueId, inputLayoutId, vertexBuffers, indexBuffer, opType ),
            mSharedData( sharedData )
        {
        }

        void _updateImmutableResource( uint32 vaoName, uint32 renderQueueId, D3D11VertexArrayObjectShared *sharedData );
        
        D3D11VertexArrayObjectShared * mSharedData;
    };
}
#endif