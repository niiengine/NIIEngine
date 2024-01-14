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

#ifndef _NII_GLGBufferManager_H_
#define _NII_GLGBufferManager_H_

#include "NiiGLPreInclude.h"
#include "NiiGBufferManager.h"
#include "NiiGLStateCacheManager.h"
#include "NiiGLRenderSystem.h"

namespace NII
{
    /** OpenGL专用GBufferManager
	*/
    class _EngineGLAPI GLGBufferManager : public GBufferManager
    {
    public:
        GLGBufferManager(bool emulateTexBuffers, const PropertyData *params);
        ~GLGBufferManager();
        
        /// @copydetails GBufferManager::create
        void create(GeometryRaw *& out);
        
        /// @copydetails GBufferManager::setupVAO
        void setupVAO(GeometryRaw * obj)

        /// @copydetails GBufferManager::unsetupVAO
        void unsetupVAO(GeometryRaw * vao);
        
        /// @copydetails GBufferManager::create
        void create(VertexBuffer *& out, NCount usize, NCount count, Nmark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::destroy
        void destroy(VertexBuffer * obj);
        
        /// @copydetails GBufferManager::create
        void create(IndexBuffer *& out, NCount usize, NCount count, Nmark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::destroy
        void destroy(IndexBuffer * obj);

        /// @copydetails GBufferManager::create
        void create(StructBuffer *& out, NCount size, Nmark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::destroy
        void destroy(StructBuffer * obj);
        
        /// @copydetails GBufferManager::create
        void create(IndirectBuffer *& out, IndirectBuffer::Type type, NCount size, Nmark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::destroy
        void destroy(IndirectBuffer * obj);

        /// @copydetails GBufferManager::create
        void create(CounterBuffer *& out, NCount size, Nmark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::destroy
        void destroy(CounterBuffer * obj);

        /// @copydetails GBufferManager::create
        void create(TextureBuffer *& out, PixelFormat pf, NCount size, Nmark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::destroy
        void destroy(TextureBuffer *obj);

        /// @copydetails GBufferManager::create
        void create(StorageBuffer *& out, NCount usize, NCount count, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0);
        
        /// @copydetails GBufferManager::destroy
        void destroy(StorageBuffer *obj);

        /// @copydetails GBufferManager::create
        void create(DisplayBuffer *& out);

        /// 获取GL的缓存类型
        static GLenum getGLUsage(Nmark usage);

        /// 获取GL的数据类型
        static GLenum getGLType(Nmark type);

        inline GLStateCacheManager * getStateManager()          { return mRenderSys->getStateManager(); }
    public:
        virtual void getMemoryStats( MemoryStatsList & outStats, size_t & outCapacityBytes, size_t & outFreeBytes, Log * log) const;

        virtual void recoverPool(void);

        /// Binds the Draw ID to the currently bound vertex array object.
        void bindDrawId(void);

        /**
        @version NIIEngine 
        */
        virtual void create(GpuBufferAsync *& out, GpuBuffer * creator, MappedBuffer * buf, NCount oft, NCount size );

        /// See GL3PlusTextureGpuManager::createMappedImpl. TextureManager delegates
        /// to the GBufferManager because behind the scenes, in GL StagingTextures are just a
        /// buffer with CPU access. However we won't track them, so it's the TextureManager's
        /// job to call destroy before the GBufferManager gets deleted.
        /// This case is more of an exception because of D3D11.
        void create(GLMappedTexture *& out, PixelFormat pf, NCount size );

        /// Important: Does not delete the stagingTexture. The TextureManager should do that.
        /// Caller is also responsible for ensuring it is safe to destroy stagingTexture
        /// (i.e. no hazards).
        void destroy(GLMappedTexture * out);

        virtual void _update(void);

        /// See GBufferManager::wait
        virtual Nui8 wait();

        /// See GBufferManager::wait
        virtual void wait( NCount frameCount );

        /// See GBufferManager::isFrameFinished
        virtual bool isFrameFinished( uint32 frameCount );

        /** Will stall undefinitely until GPU finishes (signals the sync object).
        @param fenceName
            Sync object to wait for. Will be deleted on success. On failure,
            throws an exception and fenceName will not be deleted.
        @returns
            Null ptr on success. Should throw on failure, but if this function for some
            strange reason doesn't throw, it is programmed to return 'fenceName'
        */
        static GLsync waitFor( GLsync fenceName );

        static GLuint getAttributeIndexFor( VertexType semantic );
    public:
        struct Block
        {
            NCount mOffset;
            NCount size;

            Block( NCount _offset, NCount _size ) : mOffset( _offset ), size( _size ) {}
        };
        struct Padding
        {
            NCount mPaddingOffset;
            NCount mPaddingSize;

            Padding() : mPaddingOffset( 0 ), mPaddingSize( 0 ) {}
            Padding( NCount paddingOffset, NCount paddingSize ) :
                mPaddingOffset( paddingOffset ), mPaddingSize( paddingSize ) {}

            bool operator () ( const Padding & left, NCount right ) const
            {
                return left.mPaddingOffset < right;
            }
            bool operator () ( NCount left, const Padding & right ) const
            {
                return left < right.mPaddingOffset;
            }
            bool operator () ( const Padding & left, const Padding & right ) const
            {
                return left.mPaddingOffset < right.mPaddingOffset;
            }
        };

        typedef vector<Block>::type BlockList;
        typedef vector<Padding>::type PaddingList;
        
	protected:
		/// @copydetails GBufferManager::createImpl
		void createImpl(VertexData *& out);

		/// @copydetails GBufferManager::destroyImpl
		void destroyImpl(VertexData * obj);

        /// @copydetails GBufferManager::createPoolImpl
        virtual bool createPoolImpl(GpuGroupID gid, BufferModeMark mode, StrategyType stype, NCount size);

        /// @copydetails GBufferManager::destroyPoolImpl
        virtual void destroyPoolImpl(GpuGroupID gid);
    protected:
        /** Asks for allocating buffer space in a VBO (Vertex Buffer Object).
            If the VBO doesn't exist, all VBOs are full or can't fit this request,
            then a new VBO will be created.
        @remarks
            Can throw if out of video memory
        @param size
            The requested size, in bytes.
        @param bytesPerElement
            The number of bytes per vertex or per index (i.e. 16-bit indices = 2).
            Cannot be 0.
        @param bufferType
            The type of buffer
        @param vboId [out]
            The index to the mVBOList.
        @param vboOft [out]
            The offset in bytes at which the buffer data should be placed.
        */
        bool allocateVbo(Vbo & vbo, NCount size, NCount alignment, NCount multi, BufferModeMark mode, GpuGroupID gid, NCount & vboIdx, NCount & vboOft );

        /** Deallocates a buffer allocated with @allocateVbo.
        @remarks
            All four parameters *must* match with the ones provided to or
            returned from allocateVbo, otherwise the behavior is undefined.
        @param vboIdx
            The index to the mVBOList pool that was returned by allocateVbo
        @param vboOft
            The buffer offset that was returned by allocateVbo
        @param size
            The sizeBytes parameter that was passed to allocateVbos.
        @param bufferType
            The type of buffer that was passed to allocateVbo.
        */
        void deallocateVbo( NCount vboIdx, NCount vboOft, NCount size, NCount multi, BufferModeMark mode );
    public:
        /// @see MappedBuffer::mergeBlocks
        static void mergeBlocks( BlockList::iterator blockToMerge, BlockList &blocks );
    protected:
        enum VboFlag
        {
            CPU_INACCESSIBLE                    = 0,
            CPU_ACCESSIBLE_DEFAULT              = 1,
            CPU_ACCESSIBLE_PERSISTENT           = 2,
            CPU_ACCESSIBLE_PERSISTENT_COHERENT  = 3,
            VF_MAX                              = 4
        };
        
        /** Creates a new staging buffer and adds it to the pool. @see getStagingBuffer.
        @remarks
            The returned buffer starts with a reference count of 1. You should decrease
            it when you're done using it.
        */
        virtual void createImpl(MappedBuffer *& buf, NCount size, Nmark modemark);

        GLuint createVao(const Vao & vaoRef);

        static VboFlag modeToFlag(BufferModeMark mode);

        inline void getMemoryStats(const Block & block, size_t vboIdx, size_t poolCapacity, LwString & text, MemoryStatsList & outStats, Log * log ) const;
	protected:
        struct Vbo
        {
            BufferModeMark mMark;
            StrategyType mStrategyType;
            GpuGroupID mGid;
            GLuint mVBOId;
            NCount mSize;
            GLBuffer * mBuffer;
            BlockList mFreeList;
            PaddingList mPaddingList;
        };

        struct Vao
        {
            struct VertexBinding
            {
                GLuint mVBOId;
                GLsizei mStride;
                VertexUnitList mUnitList;

                bool operator == (const VertexBinding & o) const
                {
                    return mVBOId == o.mVBOId && mUnitList == o.mUnitList && mStride == o.mStride;
                }
            };

            typedef vector<VertexBinding>::type BindingList;

            GLuint mVAOId;
            //GLint mPrimType[2];
            /// Strictly speaking the opType is not part of a GL's "VAO", however
            /// we need to generate a different VAO to perform correct rendering
            /// by the RenderQueue (and also satisfy Direct3D 11)
            OperationType mOpType;
            BindingList mBindingList;
            GLuint mIndexBuffer;
            NCount mIndexSize;
            uint32 mRefCount;
        };

        typedef vector<Vbo>::type VBOList;
        typedef vector<Vao>::type InnerVAOList;
        typedef vector<GLsync>::type GLSyncVec;
        typedef vector<Nui32>::type VBOIndexList;

        VBOList mVBOList[VF_MAX];
        VBOList mStrategyList[ST_Count];
        size_t mDefaultPoolSize[VF_MAX];

        InnerVAOList mInnerVAOList;
        VBOIndexList mValidVBOIndexList[VF_MAX];

        GLSyncVec mFrameSyncVec;

        /// True if ARB_buffer_storage is supported (Persistent Mapping and immutable buffers)
        bool mEmulateTexBuffers;

        VertexBufferPacked * mDrawId;
        
		N_mutable_mutex(mScratchMutex)
		Nui8 * mScratchBufferPool;
        NCount mValidVBOID;
        GLRenderSystem* mRenderSys;
    };
}
#endif