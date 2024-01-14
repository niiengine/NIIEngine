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

#include "NiiGLGBufferManager.h"
#include "NiiGLHardwareVertexBuffer.h"
#include "NiiGLRenderToVertexBuffer.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLGBufferManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    extern const GLuint64 kOneSecondInNanoSeconds;
    const GLuint64 kOneSecondInNanoSeconds = 1000000000;
    //-----------------------------------------------------------------------
    static const char * c_vboTypes[] =
    {
        "CPU_INACCESSIBLE",
        "CPU_ACCESSIBLE_DEFAULT",
        "CPU_ACCESSIBLE_PERSISTENT",
        "CPU_ACCESSIBLE_PERSISTENT_COHERENT",
    };
    //-----------------------------------------------------------------------
    // Scratch pool management (32 bit structure)
    struct GLScratchBufferAlloc
    {
        Nui32 size: 31;	///< 当前大小单位:字节
        Nui32 free: 1;	///< 是否空闲
    };
    #define SCRATCH_POOL_SIZE 1 * 1024 * 1024
    #define SCRATCH_ALIGNMENT 32
    //---------------------------------------------------------------------
    GLGBufferManager::GLGBufferManager(bool emulateTexBuffers, const PropertyData * params) : 
        mEmulateTexBuffers( emulateTexBuffers ),
        mDrawId( 0 )
    {
        mRenderSys = static_cast<GLRenderSystem *>(N_Engine().getRender());
        // Init scratch pool
        // TODO make it a configurable size?
        // 32-bit aligned buffer
        mScratchBufferPool = static_cast<Nui8 *>(N_alloc_align(SCRATCH_POOL_SIZE, SCRATCH_ALIGNMENT));
        GLScratchBufferAlloc * ptrAlloc = (GLScratchBufferAlloc *)mScratchBufferPool;
        ptrAlloc->size = SCRATCH_POOL_SIZE - sizeof(GLScratchBufferAlloc);
        ptrAlloc->free = 1;
        
        //Keep pools of 64MB each for static meshes
        mDefaultPoolSize[CPU_INACCESSIBLE]  = 64 * 1024 * 1024;

        //Keep pools of 4MB each for most dynamic buffers, 16 for the most common one.
        for( size_t i = CPU_ACCESSIBLE_DEFAULT; i < VF_MAX; ++i )
            mDefaultPoolSize[i] = 4 * 1024 * 1024;
        mDefaultPoolSize[CPU_ACCESSIBLE_PERSISTENT] = 16 * 1024 * 1024;

        if( params )
        {
            for( size_t i = 0; i < VF_MAX; ++i )
            {
                PropertyData::const_iterator itor = params->find( String( "GBufferManager::" ) + c_vboTypes[i] );
                if( itor != params->end() )
                {
                    StrConv::conv( itor->second, mDefaultPoolSize[i], mDefaultPoolSize[i] );
                }
            }
        }

        mFrameSyncVec.resize( mDynamicBufferMultiplier, 0 );

        GLCheck( glGetIntegerv( GLenum(GL_MAX_VERTEX_ATTRIBS), &mMaxVertexAttribs ) );

        if( mMaxVertexAttribs < 16 )
        {
            N_EXCEPT(Internal, "GL_MAX_VERTEX_ATTRIBS = " + StrConv::conv( mMaxVertexAttribs ) +
                " this value must be >= 16 for Ogre to function properly. Try updating your video card drivers." );
        }

        //The minimum alignment for these buffers is 16 because some
        //places of Ogre assume such alignment for SIMD reasons.
        GLint alignment = 1; //initial value according to specs
        GLCheck( glGetIntegerv( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment ) );
        mAlignment[GBT_Struct] = std::max<Nui32>( alignment, 16u );
        mAlignment[GBT_Texture] = 16;
        if( !emulateTexBuffers )
        {
            alignment = 1; //initial value according to specs
            GLCheck( glGetIntegerv( GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &alignment ) );
            mAlignment[GBT_Texture] = std::max<Nui32>( alignment, 16u );
        }
        if( mRenderSys->isSupport(CF_Program_StorgeBuffer) )
        {
            alignment = 1; //initial value according to specs
            GLCheck( glGetIntegerv( GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment ) );
            mAlignment[GBT_Storage] = std::max<Nui32>( alignment, 16u );
        }

        GLint maxBufferSize = 16384; //minimum value according to specs
        GLCheck( glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &maxBufferSize ) );
        mMaxSize[GBT_Struct] = static_cast<size_t>( maxBufferSize );
        maxBufferSize = 65536; //minimum value according to specs
        GLCheck( glGetIntegerv( GL_MAX_TEXTURE_BUFFER_SIZE, &maxBufferSize ) );
        mMaxSize[GBT_Texture] = static_cast<size_t>( maxBufferSize );
        if( mRenderSys->isSupport(CF_Program_StorgeBuffer) )
        {
            GLCheck( glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBufferSize ) );
            mMaxSize[GBT_Storage] = static_cast<size_t>( maxBufferSize );
        }

        //4096u is a sensible default because most Hlms implementations need 16 bytes per
        //instance in a const buffer. HlmsBufferManager::mapNextConstBuffer purposedly clamps
        //its const buffers to 64kb, so that 64kb / 16 = 4096 and thus it can never exceed
        //4096 instances.
        //However due to instanced stereo, we need twice that
        const Nui32 maxNumInstances = 4096u * 2u;
        VertexUnitList unitList;
        unitList.push_back( VertexUnit( DT_UInt1x, VES_COUNT ) );
        Nui32 * drawIdPtr = static_cast<Nui32 *>( OGRE_MALLOC_SIMD( maxNumInstances * sizeof(Nui32), MEMCATEGORY_GEOMETRY ) );
        for( Nui32 i=0; i<maxNumInstances; ++i )
            drawIdPtr[i] = i;
        mDrawId = createVertexBuffer( unitList, maxNumInstances, M_DEVREAD, drawIdPtr, true );
    }
    //-----------------------------------------------------------------------
    GLGBufferManager::~GLGBufferManager()
    {
        N_free_align(mScratchBufferPool, SCRATCH_ALIGNMENT);

        vector<GLuint>::type deletelist;

        deletelist.reserve(mRefMappedList.size() + mFreeMappedList.size());

        //Collect the buffer names from all staging buffers to use one API call
        MappedBufferList::const_iterator itor = mRefMappedList.begin();
        MappedBufferList::const_iterator end  = mRefMappedList.end();

        while( itor != end )
        {
            deletelist.push_back( static_cast<GLMappedBuffer *>(*itor)->getBufferName() );
            ++itor;
        }

        itor = mFreeMappedList.begin();
        end  = mFreeMappedList.end();

        while( itor != end )
        {
            deletelist.push_back( static_cast<GLMappedBuffer *>(*itor)->getBufferName() );
            ++itor;
        }

        for( size_t i = 0; i < VF_MAX; ++i )
        {
            //Free pointers and collect the buffer names from all VBOs to use one API call
            VBOList::iterator itor = mVBOList[i].begin();
            VBOList::iterator end  = mVBOList[i].end();

            while( itor != end )
            {
                deletelist.push_back( itor->mVBOId );
                if(itor->mBuffer)
                {
                    delete itor->mBuffer;
                    itor->mBuffer = 0;
                }
                ++itor;
            }
        }

        if( !deletelist.empty() )
        {
            GLCheck( glDeleteBuffers( deletelist.size(), &deletelist[0] ) );
            deletelist.clear();
        }

        GLSyncVec::const_iterator itor = mFrameSyncVec.begin();
        GLSyncVec::const_iterator end  = mFrameSyncVec.end();

        while( itor != end )
        {
            GLCheck( glDeleteSync( *itor ) );
            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    void GLGBufferManager::create(GeometryRaw *& out)
    {
        out = N_new GLGeometryRaw();
    }
    //-----------------------------------------------------------------------
    void GLGBufferManager::setupVAO(GeometryRaw * _obj)
    {
        obj = static_cast<GLGeometryRaw *>(_obj);
        VertexData * vb = obj->getVertex();
        const VertexData::BindingList & blist = vb->getAttachs();

        Vao vao;
        vao.mRefCount = 0;
        vao.mOpType = obj->mType;
        vao.mBindingList.reserve(blist.size());

        {
            VertexData::BindingList::const_iterator itor = blist.begin();
            VertexData::BindingList::const_iterator end  = blist.end();

            while( itor != end )
            {
                Vao::VertexBinding binding;
                vb->getUnits(itor->first, binding.mUnitList);
                binding.mVBOId = static_cast<GLBufferOp *>((*itor)->getOperation() )->getVboName();
                binding.mStride = VertexData::getUnitSize( binding.mUnitList );
                vao.mBindingList.push_back( binding );

                ++itor;
            }
        }
        
        IndexData * id = obj->getIndex()->getAttach();

        if( id )
        {
            vao.mIndexBuffer = static_cast<GLBufferOp *>(id->getOperation() )->getVboName();
            vao.mIndexSize = id->getUnitSize();
        }
        else
        {
            vao.mIndexBuffer = 0;
            vao.mIndexSize = 2;
        }

        InnerVAOList::iterator itor = mInnerVAOList.begin();
        InnerVAOList::iterator end  = mInnerVAOList.end();

        while( itor != end)
        {
            if( itor->mOpType == vao.mOpType && itor->mIndexBuffer == vao.mIndexBuffer &&
                itor->mIndexSize == vao.mIndexSize && itor->mBindingList == vao.mBindingList )
            {
                break;
            }
            else
            {
                ++itor;
            }
        }

        if(itor == end)
        {
            switch( vao.mOpType )
            {
            case OT_POINT_LIST:
                obj->mPrimType[0] = GL_POINTS;
                obj->mPrimType[1] = GL_POINTS;
                break;
            case OT_LINE_LIST:
                obj->mPrimType[0] = GL_LINES;
                obj->mPrimType[1] = GL_LINES_ADJACENCY;
                break;
            case OT_LINE_STRIP:
                obj->mPrimType[0] = GL_LINE_STRIP;
                obj->mPrimType[1] = GL_LINE_STRIP_ADJACENCY;
                break;
            default:
            case OT_TRIANGLE_LIST:
                obj->mPrimType[0] = GL_TRIANGLES;
                obj->mPrimType[1] = GL_TRIANGLES_ADJACENCY;
                break;
            case OT_TRIANGLE_STRIP:
                obj->mPrimType[0] = GL_TRIANGLE_STRIP;
                obj->mPrimType[1] = GL_TRIANGLE_STRIP_ADJACENCY;
                break;
            case OT_TRIANGLE_FAN:
                obj->mPrimType[0] = GL_TRIANGLE_FAN;
                obj->mPrimType[1] = GL_TRIANGLE_FAN;
                break;
            }
            
            vao.mVAOId = createVao( vao );
            obj->mVaoId = vao.mVAOId;
            mInnerVAOList.push_back( vao );
            itor = mInnerVAOList.begin() + mInnerVAOList.size() - 1;
        }

        //Mix mValidVAO with the GL Vao for better sorting purposes:
        //  If we only use the GL's vao, the RQ will sort Meshes with
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
        const int bitsVaoGl = 5;
        const Nui32 maskVaoGl = N_Mask( bitsVaoGl );
        const Nui32 maskVao = N_Mask( RqBits::MeshBits - bitsVaoGl );

        const Nui32 shiftVaoGl = RqBits::MeshBits - bitsVaoGl;

        obj->mRenderID = ( (itor->mVAOId & maskVaoGl) << shiftVaoGl ) | (mValidVAO & maskVao);

        ++itor->mRefCount;
        
        mVAOList.insert( obj );
        ++mValidVAO;
    }
    //-----------------------------------------------------------------------
    void GLGBufferManager::unsetupVAO(GeometryRaw * obj)
    {
        InnerVAOList::iterator itor = mInnerVAOList.begin();
        InnerVAOList::iterator end  = mInnerVAOList.end();

        GLuint vaoid = obj->mVaoId;
                
        while( itor != end && itor->mVAOId != vaoid )
            ++itor;

        if( itor != end )
        {
            --itor->mRefCount;

            if( !itor->mRefCount )
            {
                GLCheck( glDeleteVertexArrays( 1, &vaoid ) );

                remove( mInnerVAOList, itor );
            }
        }
    }
    //-----------------------------------------------------------------------
    void GLGBufferManager::create(VertexBuffer *& out, NCount usize, NCount count, Nmark mode, NCount multi, void * initData, GpuGroupID pool)
    {
        NCount vboIdx;
        NCount poft;

        Vbo vbo;
        bool re = allocateVbo(vbo, count * usize, usize, multi, mode, pool, vboIdx, poft );

        if(re)
        {
            GLBufferOp * bufop = new GLBufferOp( vboIdx, vbo.mVBOId, vbo.mBuffer );
            
            out = N_new GLVertexBuffer(this, usize, count, multi, mode, initData, pool, poft);
            out->setOperation(bufop);

            if( initData )
                bufop->initImpl(initData, 0, count);

            {
                N_lock_mutex(mVertexBuffersMutex)
                mVertexBuffers.insert(out);
            }
        }
        else
            out = 0;
    }
    //-----------------------------------------------------------------------
    void GLGBufferManager::destroy(VertexBuffer * obj)
    {
        GLBufferOp * bufop = static_cast<GLBufferOp *>(obj->getOperation());
        deallocateVbo( bufop->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(), obj->getAllocSize(), obj->getMultiCount(), obj->mMark );
        
        GBufferManager::destroy(obj);
    }
    //-----------------------------------------------------------------------
    void GLGBufferManager::create(IndexBuffer *& out, NCount usize, NCount count, Nmark mode, NCount multi, void * initData, GpuGroupID pool)
    {
        NCount vboIdx;
        NCount poft;

        Vbo vbo;
        bool re = allocateVbo(vbo, count * usize, usize, multi, mode, pool, vboIdx, poft );
        if(re)
        {
            GLBufferOp * bufop = new GLBufferOp( vboIdx, vbo.mVBOId, vbo.mBuffer);
            out = N_new GLIndexBuffer(this, usize, count, mode, multi, initData, pool, poft);
            out->setOperation(bufop);

            if( initData )
                bufop->initImpl( initData, 0, count );
            
            {
                N_lock_mutex(mIndexBuffersMutex)
                mIndexBuffers.insert(out);
            }
        }
        else
            out = 0;
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::destroy(IndexBuffer * obj)
    {
        GLBufferOp * bufop = static_cast<GLBufferOp*>(obj->getOperation() );

        deallocateVbo( bufop->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(), obj->getAllocSize(), obj->getMultiCount(), obj->mMark );

        GBufferManager::destroy(obj);
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::create(StructBuffer *& out, NCount size, Nmark mode, NCount multi, void * initData, GpuGroupID pool)
    {
        NCount vboIdx;
        NCount oft;

        GLint alignment = mAlignment[GBT_Struct];
        NCount reqsize = size;

        if(N_MarkTrue(mode, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //mOffset after each map to be aligned; and for that, we
            //size to be multiple of alignment.
            size = multiCeil( size, alignment );
        }

        Vbo vbo;
        bool re = allocateVbo(vbo, size, alignment, multi, mode, pool, vboIdx, oft );
        if(re)
        {
            GLBufferOp * bufop = new GLBufferOp( vboIdx, vbo.mVBOId, vbo.mBuffer );
            out = N_new GLStructBuffer(this, reqsize, multi, mode, initData, pool, oft, (size - reqsize) / 1);
            out->setOperation(bufop);
            if( initData )
                bufop->initImpl( initData, 0, reqsize );

            {
                N_lock_mutex(mStructBuffersMutex)
                mStructBufferList.insert(out);
            }
        }
        else
            out = 0;
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::destroy(StructBuffer * obj)
    {
        GLBufferOp *bufop = static_cast<GLBufferOp*>(obj->getOperation());

        deallocateVbo( bufop->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(), obj->getAllocSize(), obj->getMultiCount(), obj->mMark );
        
        GBufferManager::destroy(obj);
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::create(IndirectBuffer *& out, IndirectBuffer::Type type, NCount size, Nmark mode, NCount multi, void * initData, GpuGroupID pool)
    {
        const size_t alignment = 4;
        NCount oft = 0;
        size_t reqsize = size;

        if(N_MarkTrue(mode, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //mOffset after each map to be aligned; and for that, we
            //size to be multiple of alignment.
            size = multiCeil( size, alignment );
        }

        GLBufferOp * bufop = 0;
        if( mRenderSys->isSupport(CF_IndirectBuffer) )
        {
            NCount vboIdx;

            Vbo vbo;
            bool re = allocateVbo(vbo, size, alignment, multi, mode, pool, vboIdx, oft );
            if(re)
                bufop = new GLBufferOp( vboIdx, vbo.mVBOId, vbo.mBuffer );
        }

        out = N_new IndirectBuffer(this, type, reqsize, multi, mode, initData, pool, oft, (size - reqsize) / 1);
        out->setOperation(bufop);
        
        if( initData )
        {
            if( mRenderSys->isSupport(CF_IndirectBuffer) )
            {
                bufop->initImpl( initData, 0, reqsize );
            }
            else
            {
                memcpy( out->getHostData(), initData, reqsize );
            }
        }
        
        {
            N_lock_mutex(mIndirectBuffersMutex)
            mIndirectBufferList.insert( out );
        }
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::destroy(IndirectBuffer * obj)
    {
        if( mRenderSys->isSupport(CF_IndirectBuffer) )
        {
            GLBufferOp * bufop = static_cast<GLBufferOp *>(obj->getOperation() );

            deallocateVbo( bufop->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(), obj->getAllocSize(), obj->getMultiCount(), obj->mMark );
        }
        
        GBufferManager::destroy(obj);
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::create(CounterBuffer *& out, NCount size, Nmark mode, NCount multi, void * initData, GpuGroupID pool)
    {
        NCount vboIdx;
        NCount poft;

        GLint alignment = mAlignment[GBT_Counter];
        
        Vbo vbo;
        bool re = allocateVbo(vbo, size, alignment, multi, mode, pool, vboIdx, poft);
        if(re)
        {
            GLBufferOp * bufop = new GLBufferOp( vboIdx, vbo.mVBOId, vbo.mBuffer);
            out = N_new GLCounterBuffer(this, size, count, mode, multi, initData, pool, poft);
            out->setOperation(bufop);

            if( initData )
                bufop->initImpl( initData, 0, count );
            
            {
                N_lock_mutex(mCounterBuffersMutex)
                mCounterBuffers.insert(out);
            }
        }
        else
            out = 0;
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::destroy(CounterBuffer * obj)
    {
        GLBufferOp * bufop = static_cast<GLBufferOp*>(obj->getOperation());
        deallocateVbo( bufop->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(), obj->getAllocSize(), obj->getMultiCount(), obj->mMark );
        
        GBufferManager::destroy(obj);
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::create(DisplayBuffer *& out)
    {
        out = N_new GLRenderToVertexBuffer;
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::create(TextureBuffer *& out, PixelFormat pf, NCount size, Nmark mode, NCount multi, void * initData, GpuGroupID pool)
    {
        NCount vboIdx;
        NCount oft;

        GLint alignment = mAlignment[GBT_Texture];
        size_t reqsize = size;

        if( mEmulateTexBuffers )
        {
            // Align to the texture size since we must copy the PBO to a texture.
            ushort maxTexSizeBytes = 2048 * PixelUtil::getUnitSize( pf );
            // We need another line of maxTexSizeBytes for uploading
            //to create a rectangle when calling glTexSubImage2D().
            size = multiCeil( size, maxTexSizeBytes );
        }

        if(N_MarkTrue(mode, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //mOffset after each map to be aligned; and for that, we
            //size to be multiple of alignment.
            size = multiCeil( size, alignment );
        }

        Vbo vbo;
        bool re = allocateVbo(vbo, size, alignment, multi, mode, pool, vboIdx, oft );
        
        if(re)
        {
            GLBufferOp * bufop = new GLBufferOp( vboIdx, vbo.mVBOId, vbo.mBuffer );
            
            TextureBuffer * out;

            if( mEmulateTexBuffers )
            {
                out = N_new GLTextureBufferEmu(this, reqsize, pf, multi, mode, initData, pool, oft, (size - reqsize) / 1);
            }
            else
            {
                out = N_new GLTextureBuffer(this, reqsize, pf, multi, mode, initData, pool, oft, (size - reqsize) / 1);
            }

            out->setOperation(bufop);
            
            if( initData )
                bufop->initImpl( initData, 0, reqsize );
            
            {
                N_lock_mutex(mTextureBuffersMutex)
                mTextureBufferList.insert( out );
            }
        }
        else
            out = 0;
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::destroy(TextureBuffer * obj)
    {
        GLBufferOp * bufop = static_cast<GLBufferOp*>(obj->getOperation() );

        deallocateVbo( bufop->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(), obj->getAllocSize(), obj->getMultiCount(), obj->mMark );
                       
        GBufferManager::destroy(obj);
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::create(StorageBuffer *& out, NCount usize, NCount count, NCount multi, void * initData, GpuGroupID pool)
    {
        NCount vboIdx;
        NCount oft;

        size_t alignment = Math::lcm( mAlignment[GBT_Storage], usize );

        Vbo vbo;
        bool re = allocateVbo(vbo, count * usize, alignment, multi, M_DEVNORMAL, pool, vboIdx, oft);

        if(re)
        {
            GLBufferOp * bufop = new GLBufferOp(vboIdx, vbo.mVBOId, vbo.mBuffer);
            
            StorageBuffer * out = N_new GLStorageBuffer(oft, count, usize, multi, initData, keepAsShadow, this, bufop);

            if( initData )
                bufop->initImpl( initData, 0, count );

            {
                N_lock_mutex(mShaderBuffersMutex)
                mShaderBufferList.insert( out );
            }
        }
        else
            out = 0;
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::destroy(StorageBuffer * obj)
    {
        GLBufferOp *bufop = static_cast<GLBufferOp*>(obj->getOperation() );

        deallocateVbo( bufop->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(), obj->getAllocSize(), obj->getMultiCount(), obj->mMark );
                       
        GBufferManager::destroy(obj);
    }
    //---------------------------------------------------------------------
    GLenum GLGBufferManager::getGLUsage(Nmark usage)
    {
        switch(usage)
        {
        case Buffer::M_DEV:
        case (Buffer::M_DEV | Buffer::M_WRITE):
            return GL_STATIC_DRAW_ARB;
        case (Buffer::M_READ | Buffer::M_WRITE):
        case Buffer::M_WRITE:
            return GL_DYNAMIC_DRAW_ARB;
        case (Buffer::M_WRITE | Buffer::M_WHOLE):
            return GL_STREAM_DRAW_ARB;
        default:
            return GL_DYNAMIC_DRAW_ARB;
        };
        // GL_STATIC_DRAW_ARB
        // GL_STATIC_READ_ARB
        // GL_STATIC_COPY_ARB
        // GL_DYNAMIC_DRAW_ARB
        // GL_DYNAMIC_READ_ARB
        // GL_DYNAMIC_COPY_ARB
        // GL_STREAM_DRAW_ARB
        // GL_STREAM_READ_ARB
        // GL_STREAM_COPY_ARB
    }
    //---------------------------------------------------------------------
    GLenum GLGBufferManager::getGLType(Nmark type)
    {
        switch(type)
        {
        case DT_Float1x:
        case DT_Float2x:
        case DT_Float3x:
        case DT_Float4x:
			return GL_FLOAT;
        case DT_Double1x:
        case DT_Double2x:
        case DT_Double3x:
        case DT_Double4x:
            return GL_DOUBLE;
        case DT_Int1x:
        case DT_Int2x:
        case DT_Int3x:
        case DT_Int4x:
            return GL_INT;
        case DT_UInt1x:
        case DT_UInt2x:
        case DT_UInt3x:
        case DT_UInt4x:
            return GL_UNSIGNED_INT;
        case DT_Short2x:
        case DT_Short4x:
        case DT_ShortUnit2x:
        case DT_ShortUnit4x:
			return GL_SHORT;
        case DT_UShort2x:
        case DT_UShort4x:
        case DT_UShortUnit2x:
        case DT_UShortUnit4x:
            return GL_UNSIGNED_SHORT;
        case DT_Colour:
        case DT_Colour_ABGR:
        case DT_Colour_ARGB:
        case DT_UByte4x:
        case DT_UByteUnit4x:
			return GL_UNSIGNED_BYTE;
        case DT_Byte4x:
        case DT_ByteUnit4x:
            return GL_BYTE;
        case DT_HalfFloat2x:
        case DT_HalfFloat4x:
            return GL_HALF_FLOAT;
        default:
			return 0;
        };
    }
    //---------------------------------------------------------------------
    /*void * GLGBufferManager::allocTempMapBuffer(Nui32 size)
    {
        // simple forward link search based on alloc sizes
        // not that fast but the list should never get that long since not many
        // locks at once (hopefully)
        N_lock_mutex(mScratchMutex)

        // Alignment - round up the size to 32 bits
        // control blocks are 32 bits too so this packs nicely
        if (size % 4 != 0)
        {
            size += 4 - (size % 4);
        }

        Nui32 bufferPos = 0;
        while (bufferPos < SCRATCH_POOL_SIZE)
        {
            GLScratchBufferAlloc * pNext = (GLScratchBufferAlloc*)(mScratchBufferPool + bufferPos);
            
            if (pNext->free && pNext->size >= size)
            {
                // split? And enough space for control block
                if(pNext->size > size + sizeof(GLScratchBufferAlloc))
                {
                    Nui32 offset = sizeof(GLScratchBufferAlloc) + size;
                    
                    GLScratchBufferAlloc* pSplitAlloc = (GLScratchBufferAlloc*)(mScratchBufferPool + bufferPos + offset);
                    pSplitAlloc->free = 1;
                    // 上个模块存储了总池剩余的大小
                    pSplitAlloc->size = pNext->size - size - sizeof(GLScratchBufferAlloc);
                    // New size of current
                    pNext->size = size;
                }
                // allocate and return
                pNext->free = 0;
                // return pointer just after this control block (++ will do that for us)
                return ++pNext;
            }
            bufferPos += sizeof(GLScratchBufferAlloc) + pNext->size;
        }
        // no available alloc
        return 0;
    }*/
    //---------------------------------------------------------------------
    /*void GLGBufferManager::freeTempMapBuffer(void* ptr)
    {
        N_lock_mutex(mScratchMutex)

        // Simple linear search dealloc
        Nui32 bufferPos = 0;
        GLScratchBufferAlloc * pLast = 0;
        while (bufferPos < SCRATCH_POOL_SIZE)
        {
            GLScratchBufferAlloc * pCurrent = (GLScratchBufferAlloc*)(mScratchBufferPool + bufferPos);

            // Pointers match?
            if ((mScratchBufferPool + bufferPos + sizeof(GLScratchBufferAlloc)) == ptr)
            {
                // dealloc
                pCurrent->free = 1;

                // merge with previous
                if (pLast && pLast->free)
                {
                    // adjust buffer pos
                    bufferPos -= (pLast->size + sizeof(GLScratchBufferAlloc));
                    // merge free space
                    pLast->size += pCurrent->size + sizeof(GLScratchBufferAlloc);
                    pCurrent = pLast;
                }

                // merge with next
                Nui32 offset = bufferPos + pCurrent->size + sizeof(GLScratchBufferAlloc);
                if (offset < SCRATCH_POOL_SIZE)
                {
                    GLScratchBufferAlloc * pNext = (GLScratchBufferAlloc*)(mScratchBufferPool + offset);
                    if (pNext->free)
                    {
                        pCurrent->size += pNext->size + sizeof(GLScratchBufferAlloc);
                    }
                }

                // done
                return;
            }
            bufferPos += sizeof(GLScratchBufferAlloc) + pCurrent->size;
            pLast = pCurrent;
        }
        N_assert (false && "Memory deallocation error");
    }*/
	//---------------------------------------------------------------------
	void GLGBufferManager::createImpl(VertexData *& out)
	{
		out = N_new VertexData();
	}
	//---------------------------------------------------------------------
	void GLGBufferManager::destroyImpl(VertexData * obj)
	{
		N_delete obj;
	}
    //---------------------------------------------------------------------
    bool GLGBufferManager::createPoolImpl(GpuGroupID gid, BufferModeMark mode, StrategyType stype, NCount size)
    {
        Vbo temp;
        
        VbO * re;

        VboFlag flag = modeToFlag( mode );
        
        if(mValidVBOIndexList[flag].size())
        {
            re = &mVBOList[flag][mValidVBOIndexList[flag].back()];
        }
        else
        {
            re = &temp;
        }
            
        size_t psize = std::max( mDefaultPoolSize[flag], size );

        //No luck, allocate a new buffer.
        GLCheck( glGenBuffers( 1, &re->mVBOId ) );
        GLCheck( glBindBuffer( GL_ARRAY_BUFFER, re->mVBOId ) );

        GLenum error = 0;
        int trustCounter = 1000;
        //Reset the error code. Trust counter prevents an infinite loop
        //just in case we encounter a moronic GL implementation.
        while( glGetError() && trustCounter-- );

        if( mRenderSys->isSupport(CF_PersistentMapping) )
        {
            GLbitfield flags = 0;

            if( flag >= CPU_ACCESSIBLE_DEFAULT )
            {
                flags |= GL_MAP_WRITE_BIT;

                if( flag >= CPU_ACCESSIBLE_PERSISTENT )
                {
                    flags |= GL_MAP_PERSISTENT_BIT;

                    if( flag >= CPU_ACCESSIBLE_PERSISTENT_COHERENT )
                        flags |= GL_MAP_COHERENT_BIT;
                }
            }

            glBufferStorage(GL_ARRAY_BUFFER, psize, 0, flags);

            error = glGetError();
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, psize, 0, flag == CPU_INACCESSIBLE ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

            error = glGetError();
        }

        //OpenGL can't continue after any GL_OUT_OF_MEMORY has been raised,
        //thus ignore the trustCounter in that case.
        if((error != 0 && trustCounter != 0) || error == GL_OUT_OF_MEMORY)
        {
            N_EXCEPT(Internal, "Out of GPU memory or driver refused.\n glGetError code: " + Nconv( error ) +  ".\n Requested: " + Nconv( psize ) + " bytes." );
        }

        GLCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));

        re->mMark = mode;
        re->mStrategyType = stype;
        re->mGid = gid;
        re->mSize = psize;
        re->mFreeList.push_back(Block(0, psize));

        if( flag != CPU_INACCESSIBLE )
        {
            re->mBuffer = new GLBuffer(re->mVBOId, re->mSize, this, mode);
        }
        else
        {
            re->mBuffer = 0;
        }

        if(mValidVBOIndexList[flag].size())
        {
            mValidVBOIndexList[flag].pop_back();
        }
        else
        {
            mVBOList[flag].push_back( temp );
        }
    }
    //---------------------------------------------------------------------
    void GLGBufferManager::destroyPoolImpl(GpuGroupID gid)
    {
        for(NCount c = 0; c < VF_MAX; ++c)
        {
            VBOList & vbl = mVBOList[c];
            VBOList::iterator i, iend = vbl.end();
            for(i = vbl.begin(); i != iend; ++i)
            {
                Vbo & vbo = *i;
                if(vbo.mGid == gid)
                {
                    GLCheck(glDeleteBuffers(1, &vbo.mVBOId));
                    if(itor->mBuffer)
                    {
                        delete itor->mBuffer;
                        itor->mBuffer = 0;
                    }
                    vbl.erase(i);
                    return;
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::getMemoryStats(const Block & block, size_t vboIdx, size_t poolCapacity, LwString & text, MemoryStatsList & outStats, Log * log) const
    {
        if( log )
        {
            text.clear();
            text.a( c_vboTypes[vboIdx], ";", (uint64)block.mOffset, ";", (uint64)block.size, ";", (uint64)poolCapacity );
            log->logMessage( text.c_str(), LML_CRITICAL );
        }

        MemoryStatsEntry entry( (Nui32)vboIdx, block.mOffset, block.size, poolCapacity );
        outStats.push_back( entry );
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::getMemoryStats( MemoryStatsList & outStats, size_t & outCapacityBytes, size_t & outFreeBytes, Log *log ) const
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
            VBOList::const_iterator itor = mVBOList[vboIdx].begin();
            VBOList::const_iterator end  = mVBOList[vboIdx].end();

            while( itor != end )
            {
                const Vbo &vbo = *itor;
                capacityBytes += vbo.mSize;

                Block usedBlock( 0, 0 );

                BlockList freeBlocks = vbo.mFreeList;
                while( !freeBlocks.empty() )
                {
                    //Find the free block that comes next
                    BlockList::iterator nextBlock;
                    {
                        BlockList::iterator itBlock = freeBlocks.begin();
                        BlockList::iterator enBlock = freeBlocks.end();

                        nextBlock = itBlock;

                        while( itBlock != enBlock )
                        {
                            if( nextBlock->mOffset < itBlock->mOffset )
                                nextBlock = itBlock;
                            ++itBlock;
                        }
                    }

                    freeBytes += nextBlock->size;
                    usedBlock.size = nextBlock->mOffset;

                    //usedBlock.size could be 0 if:
                    //  1. All of memory is free
                    //  2. There's two contiguous free blocks, which should not happen
                    //     due to mergeBlocks
                    if( usedBlock.size > 0u )
                        getMemoryStats( usedBlock, vboIdx, vbo.mSize, text, statsVec, log );

                    usedBlock.mOffset += usedBlock.size;
                    usedBlock.size = 0;
                    remove( freeBlocks, nextBlock );
                }

                if( usedBlock.size > 0u || (usedBlock.mOffset == 0 && usedBlock.size == 0) )
                    getMemoryStats( usedBlock, vboIdx, vbo.mSize, text, statsVec, log );

                ++itor;
            }
        }

        outCapacityBytes = capacityBytes;
        outFreeBytes = freeBytes;
        statsVec.swap( outStats );
    }
    //-----------------------------------------------------------------------------------
    /*void GLGBufferManager::switchVboPoolIndexImpl( size_t oldPoolIdx, size_t newPoolIdx, GpuBuffer *buffer )
    {
        if( mRenderSys->isSupport(CF_IndirectBuffer) || buffer->getBufferPackedType() != BP_TYPE_INDIRECT )
        {
            GLBufferOp * bufop = static_cast<GLBufferOp *>(buffer->getOperation() );
            if( bufop->getVboPoolIndex() == oldPoolIdx )
                bufop->_setVboPoolIndex( newPoolIdx );
        }
    }*/
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::recoverPool(void)
    {
        vectorL<GLuint> deletelist;
        for(int vboIdx = 0; vboIdx < VF_MAX; ++vboIdx )
        {
            VBOList::iterator itor = mVBOList[vboIdx].begin();
            VBOList::iterator end  = mVBOList[vboIdx].end();

            while( itor != end )
            {
                Vbo & vbo = *itor;
                if( vbo.mFreeList.size() == 1u && vbo.mSize == vbo.mFreeList.back().size )
                {
#if OGRE_DEBUG_MODE >= OGRE_DEBUG_LOW
                    VAOList::const_iterator itVao = mInnerVAOList.begin();
                    VAOList::const_iterator enVao = mInnerVAOList.end();

                    while( itVao != enVao )
                    {
                        Vao::BindingList::const_iterator itBuf = itVao->mBindingList.begin();
                        Vao::BindingList::const_iterator enBuf = itVao->mBindingList.end();

                        while( itBuf != enBuf )
                        {
                            N_assert( itBuf->mVBOId != vbo.mVBOId && "A GeometryRaw still references a deleted vertex buffer!" );
                            ++itBuf;
                        }

                        N_assert( itVao->mIndexBuffer != vbo.mVBOId && "A GeometryRaw still references a deleted index buffer!" );

                        ++itVao;
                    }
#endif

                    deletelist.push_back( vbo.mVBOId );
                    if(vbo.mBuffer)
                    {
                        delete vbo.mBuffer;
                        vbo.mBuffer = 0;
                    }
                    vbo.mFreeList.clear();
                    vbo.mPaddingList.clear();
                    vbo.mVBOId = 0;

                    //There's (unrelated) live buffers whose vboIdx will now point out of bounds.
                    //We need to update them so they don't crash deallocateVbo later.
                    mValidVBOIndexList[vboIdx].push_back(itor - mVBOList[vboIdx].begin());
                    //itor = remove( mVBOList[vboIdx], itor );
                    end  = mVBOList[vboIdx].end();
                }
                else
                {
                    ++itor;
                }
            }
        }

        if( !deletelist.empty() )
        {
            GLCheck( glDeleteBuffers( deletelist.size(), &deletelist[0] ) );
            deletelist.clear();
        }
    }
    //-----------------------------------------------------------------------------------
    bool GLGBufferManager::allocateVbo(Vbo & revbo, NCount size, NCount alignment, NCount multi, BufferModeMark mode, GpuGroupID gid, NCount & vboIdx, NCount & vboOft )
    {
        N_assert( alignment > 0 );

        VboFlag flag = modeToFlag( mode );

        if(N_MarkTrue(mode, M_NORMAL))
            size *= multi;

        //Find a suitable VBO that can hold the requested size. We prefer those free
        //blocks that have a matching mStride (the current offset is a multiple of
        //usize) in order to minimize the amount of memory padding.
        vboIdx = ~0;
        size_t bestBlockIdx = ~0;
        bool matchStride = false;
        BufferModeMark vboMark = mode & M_USAGEMARK;
        StrategyType vboStrategyType = ST_Unknow;
        NCount vboSize = 0;
        VBOList::const_iterator itor, end  = mVBOList[flag].end();
        for(itor = mVBOList[flag].begin(); itor != end; ++itor)
        {
            Vbo & vbo = *itor;
            if(vbo.mGid == gid || (vbo.mStrategyType == vboStrategyType && vbo.mMark == vboMark))
            {
                BlockList::const_iterator blockIt = vbo.mFreeList.begin();
                BlockList::const_iterator blockEn = vbo.mFreeList.end();

                while(blockIt != blockEn && !matchStride)
                {
                    const Block & block = *blockIt;

                    //Round to next multiple of alignment
                    size_t newOffset = ((block.mOffset + alignment - 1) / alignment) * alignment;
                    size_t padding = newOffset - block.mOffset;

                    if( size + padding <= block.size )
                    {
                        vboIdx = itor - mVBOList[flag].begin();
                        bestBlockIdx = blockIt - gidmFreeList.begin();

                        if( newOffset == block.mOffset )
                            matchStride = true;
                        
                        if(vbo.mGid == gid)
                            goto ifoundit;
                    }

                    ++blockIt;
                }
                if(vbo.mGid == gid)
                {
                    vboMark = vbo.mMark;
                    vboStrategyType = vbo.mStrategyType;
                    vboSize = vbo.mSize;
                }
            }
        }
        
        if( bestBlockIdx == (size_t)~0 )
        {
            if(vboSize == 0)
                return false;

            addPool(++mValidVBOID, vboMark, vboStrategyType, vboSize);
            vboIdx = mVBOList[flag].size() - 1;
            bestBlockIdx = 0;
        }
    ifoundit:
        revbo = mVBOList[flag][vboIdx];
        Block & remain = revbo.mFreeList[bestBlockIdx];

        vboOft = ( (remain.mOffset + alignment - 1) / alignment ) * alignment;
        size_t padding = vboOft - remain.mOffset;
        //Shrink our records about available data.
        remain.size -= size + padding;
        remain.mOffset = vboOft + size;

        if( !matchStride )
        {
            //This is a mStride changer, record as such.
            PaddingList::iterator itStride = std::lower_bound( revbo.mPaddingList.begin(), revbo.mPaddingList.end(), vboOft, Padding() );
            revbo.mPaddingList.insert( itStride, Padding( vboOft, padding ) );
        }

        if( remain.size == 0 )
            revbo.mFreeList.erase( revbo.mFreeList.begin() + bestBlockIdx );
        
        return true;
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::deallocateVbo( NCount vboIdx, NCount vboOft, NCount size, NCount multi, BufferModeMark mode)
    {
        if(N_MarkTrue(mode, M_NORMAL))
            size *= multi;

        VboFlag flag = modeToFlag(mode);
        Vbo & vbo = mVBOList[flag][vboIdx];
        PaddingList::iterator itStride = std::lower_bound(vbo.mPaddingList.begin(), vbo.mPaddingList.end(), vboOft, Padding());

        if(itStride != vbo.mPaddingList.end() && itStride->mPaddingOffset == vboOft)
        {
            vboOft -= itStride->mPaddingSize;
            size += itStride->mPaddingSize;

            vbo.mPaddingList.erase( itStride );
        }

        //See if we're contiguous to a free block and make that block grow.
        vbo.mFreeList.push_back( Block( vboOft, size ) );
        mergeBlocks( vbo.mFreeList.end() - 1, vbo.mFreeList );
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::mergeBlocks(BlockList::iterator blockToMerge, BlockList & blocks)
    {
        BlockList::iterator itor = blocks.begin();
        BlockList::iterator end  = blocks.end();

        while( itor != end )
        {
            if( itor->mOffset + itor->size == blockToMerge->mOffset )
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
            else if( blockToMerge->mOffset + blockToMerge->size == itor->mOffset )
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
    GLuint GLGBufferManager::createVao( const Vao & vaoRef )
    {
        GLuint vaoid;
        GLCheck( glGenVertexArrays( 1, &vaoid ) );
        GLCheck( glBindVertexArray( vaoid ) );

        GLuint uvCount = 0;

        for( size_t i = 0; i < vaoRef.mBindingList.size(); ++i )
        {
            const Vao::VertexBinding & binding = vaoRef.mBindingList[i];

            glBindBuffer( GL_ARRAY_BUFFER, binding.mVBOId );

            VertexUnitList::const_iterator it = binding.mUnitList.begin();
            VertexUnitList::const_iterator en = binding.mUnitList.end();

            while( it != en )
            {
                GLint typeCount;
                GLboolean normalised;

                switch( it->mType )
                {
                case DT_Colour:
                case DT_Colour_ABGR:
                case DT_Colour_ARGB:
                    // Because GL takes these as a sequence of single unsigned bytes, count needs to be 4
                    // VertexElement::getTypeCount treats them as 1 (RGBA)
                    // Also need to normalise the fixed-point data
                    typeCount = 4;
                    normalised = GL_TRUE;
                    break;
                default:
                    typeCount= DataTypeUtil::getCount( it->mType );
                    normalised = DataTypeUtil::isUnit( it->mType ) ? GL_TRUE : GL_FALSE
                    break;
                };

                GLuint attributeIndex = GLSLProgramBase::DefaultAttributeLayout[it->mVType];

                if( it->mVType == VT_Tex_Coord )
                {
                    N_assert( uvCount < 8 && "Up to 8 UVs are supported." );
                    attributeIndex += uvCount;
                    ++uvCount;
                }

                N_assert( ( uvCount < 6 || (it->mVType != VT_Custom7 && it->mVType != VT_Custom8) ) &&
                    "Available UVs get reduced from 8 to 6 when VT_Custom7/INDICES2 is present" );

                if( it->mVType == VT_Binormal )
                {
                    N_log("WARNING: VT_Binormal will not render properly in "
                            "many GPUs where GL_MAX_VERTEX_ATTRIBS = 16. Consider"
                                " changing for VT_Tangent with 4 components or use QTangents");
                }
                NCount ccc;
                switch(DataTypeUtil::getBaseType( it->mType, ccc) )
                {
                default:
                case DT_Float1x:
                    GLCheck( glVertexAttribPointer( attributeIndex, typeCount, getGLType( it->mType ),
                        normalised, binding.mStride, (void*)(it->mOffset) ) );
                    break;
                case DT_Byte4x:
                case DT_UByte4x:
                case DT_Short2x:
                case DT_UShort2x:
                case DT_UInt1x:
                case DT_Int1x:
                    GLCheck( glVertexAttribIPointer( attributeIndex, typeCount, getGLType( it->mType ),
                        binding.mStride, (void*)(it->mOffset) ) );
                    break;
                case DT_Double1x:
                    GLCheck( glVertexAttribLPointer( attributeIndex, typeCount, getGLType( it->mType ),
                        binding.mStride, (void*)(it->mOffset) ) );
                    break;
                }

                GLCheck( glVertexAttribDivisor( attributeIndex, it->mDivisor ) );
                GLCheck( glEnableVertexAttribArray( attributeIndex ) );
                ++it;
            }

            GLCheck( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
        }

        {
            //Now bind the Draw ID.
            bindDrawId();
        }

        if( vaoRef.mIndexBuffer )
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vaoRef.mIndexBuffer );

        GLCheck( glBindVertexArray( 0 ) );

        return vaoid;
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::bindDrawId(void)
    {
        GLBufferOp * drawIdBufferInterface = static_cast<GLBufferOp *>( mDrawId->getOperation() );

        const GLuint drawIdIdx = 15;
        GLCheck( glBindBuffer( GL_ARRAY_BUFFER, drawIdBufferInterface->getVboName() ) );
        GLCheck( glVertexAttribIPointer( drawIdIdx, 1, GL_UNSIGNED_INT, sizeof(Nui32), (void*)(mDrawId->getCurrentOffset() * mDrawId->getUnitSize()) ) );
        GLCheck( glVertexAttribDivisor( drawIdIdx, 1 ) );
        GLCheck( glEnableVertexAttribArray( drawIdIdx ) );
        GLCheck( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::createImpl(MappedBuffer *& buf, NCount size, Nmark mode )
    {
        size = std::max<size_t>( size, 4 * 1024 * 1024 );

        GLuint bufferName;
        bool forUpload = mode & M_DEVWRITE;
        GLenum target = forUpload ? GL_COPY_READ_BUFFER : GL_COPY_WRITE_BUFFER;
        GLCheck( glGenBuffers( 1, &bufferName ) );
        GLCheck( glBindBuffer( target, bufferName ) );

        if( mRenderSys->isSupport(CF_PersistentMapping) )
        {
            GLCheck( glBufferStorage(target, size, 0, forUpload ? GL_MAP_WRITE_BIT : GL_MAP_READ_BIT));
        }
        else
        {
            GLCheck( glBufferData(target, size, 0, forUpload ? GL_STREAM_DRAW : GL_STREAM_READ));
        }

        buf = N_new GLMappedBuffer(this, 0, size, mode, bufferName);
        
        mRefMappedList.push_back( buf );

        if( mNextCheck == (unsigned long)( ~0 ) )
            mNextCheck = mTimer->getMilliseconds() + mDefaultStagingBufferLifetime;

        return buf;
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::create(GpuBufferAsync *& out, GpuBuffer *creator, NCount oft, NCount size, MappedBuffer * buf)
    {
        if(buf)
            out = N_new GLBufferAsync( creator, buf, oft, size ) ;
        else
            out = N_new GLBufferAsync( creator, oft, size ) ;
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::create(GLMappedTexture *& out, PixelFormat pf, NCount size )
    {
        size_t vboIdx = 0;
        size_t oft = 0;

        GLenum error = 0;
        int trustCounter = 1000;
        //Reset the error code. Trust counter prevents an infinite loop
        //just in case we encounter a moronic GL implementation.
        while( glGetError() && trustCounter-- );

        GLuint bufferName;

        if( mRenderSys->isSupport(CF_PersistentMapping) )
        {
            GLCheck( glGenBuffers( 1, &bufferName ) );
            GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, bufferName ) );

            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
            glBufferStorage( GL_COPY_READ_BUFFER, size, 0, flags );
        }
        else
        {
            //Non-persistent buffers cannot share the buffer with other data, because the worker
            //threads may keep the buffer mapped while the main thread wants to do GL operations
            //on it, which will probably work but is illegal as per the OpenGL spec.
            GLCheck( glGenBuffers( 1, &bufferName ) );
            GLCheck( glBindBuffer( GL_COPY_READ_BUFFER, bufferName ) );
            glBufferData( GL_COPY_READ_BUFFER, size, 0, GL_STREAM_DRAW );
        }

        error = glGetError();

        //OpenGL can't continue after any GL_OUT_OF_MEMORY has been raised,
        //thus ignore the trustCounter in that case.
        if( (error != 0 && trustCounter != 0) || error == GL_OUT_OF_MEMORY )
        {
            N_EXCEPT(Internal, "Out of GPU memory or driver refused.\n glGetError code: " + StrConv::conv( error ) + ".\n"
                "Requested: " + StrConv::conv( size ) + " bytes.");
        }

        GLBuffer * dynamicBuffer = new GLBuffer( bufferName, size, this, mRenderSys->isSupport(CF_PersistentMapping) ? M_NORMAL | M_PersistentMap : M_NORMAL );

        out = N_new GLMappedTexture( this, pf, size, oft, vboIdx, dynamicBuffer );
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::destroy(GLMappedTexture * buf)
    {
        buf->_unmapBuffer();
        GLBuffer * buffer = buf->_getDynamicBuffer();
        GLuint bufferName = buffer->getVboName();
        GLCheck( glDeleteBuffers( 1u, &bufferName ) );
        delete buffer;
        buf->_resetDynamicBuffer();
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::_update(void)
    {
        unsigned long currentTimeMs = mTimer->getMilliseconds();

        vectorL<GLuint> deletelist;

        if( currentTimeMs >= mNextCheck )
        {
            mNextCheck = (unsigned long)(~0);

            MappedBufferList::iterator itor = mFreeMappedList.begin();
            MappedBufferList::iterator end  = mFreeMappedList.end();

            while( itor != end )
            {
                MappedBuffer * mapbuf = *itor;

                mNextCheck = std::min(
                    mNextCheck, mapbuf->getLastUsedTimestamp() + mapbuf->getLifetimeThreshold() );

                if( mapbuf->getLastUsedTimestamp() + mapbuf->getUnfencedTimeThreshold() < currentTimeMs )
                {
                    static_cast<GLMappedBuffer*>( mapbuf )->cleanUnfencedHazards();
                }

                if( mapbuf->getLastUsedTimestamp() + mapbuf->getLifetimeThreshold() < currentTimeMs )
                {
                    //Time to delete this buffer.
                    deletelist.push_back( static_cast<GLMappedBuffer*>(mapbuf)->getBufferName());

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

        if( !deletelist.empty() )
        {
            GLCheck( glDeleteBuffers( deletelist.size(), &deletelist[0] ) );
            deletelist.clear();
        }

        if( !mRecoverList.empty() && mRecoverList.front().mFrameMark == mFrameMark )
        {
            wait();
            destroyRecover( mFrameMark );
        }

        GBufferManager::_update();

        wait();

        if( mFrameSyncVec[mFrameMark] )
        {
            GLCheck( glDeleteSync( mFrameSyncVec[mFrameMark] ) );
        }
        GLCheck( mFrameSyncVec[mFrameMark] = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
        mFrameMark = (mFrameMark + 1) % mDynamicBufferMultiplier;
    }
    //-----------------------------------------------------------------------------------
    Nui8 GLGBufferManager::wait()
    {
        if(mFrameSyncVec[mFrameMark])
        {
            waitFor(mFrameSyncVec[mFrameMark]);
            mFrameSyncVec[mFrameMark] = 0;
        }

        return mFrameMark;
    }
    //-----------------------------------------------------------------------------------
    void GLGBufferManager::wait(NCount frameCount)
    {
        if(frameCount == mFrameCount)
        {
            //Full stall
            glFinish();

            //All of the other per-frame fences are not needed anymore.
            GLSyncVec::iterator itor = mFrameSyncVec.begin();
            GLSyncVec::iterator end  = mFrameSyncVec.end();

            while( itor != end )
            {
                if( *itor )
                {
                    GLCheck( glDeleteSync( *itor ) );
                    *itor = 0;
                }
                ++itor;
            }

            destroyAllRecover();

            mFrameCount += mDynamicBufferMultiplier;
        }
        else if( mFrameCount - frameCount <= mDynamicBufferMultiplier )
        {
            //Let's wait on one of our existing fences...
            //frameDiff has to be in range [1; mDynamicBufferMultiplier]
            size_t frameDiff = mFrameCount - frameCount;
            const size_t idx = (mFrameMark + mDynamicBufferMultiplier - frameDiff) % mDynamicBufferMultiplier;
            if( mFrameSyncVec[idx] )
            {
                mFrameSyncVec[idx] = waitFor( mFrameSyncVec[idx] );

                //Delete all the fences until this frame we've just waited.
                size_t nextIdx = mFrameMark;
                while( nextIdx != idx )
                {
                    if( mFrameSyncVec[nextIdx] )
                    {
                        GLCheck( glDeleteSync( mFrameSyncVec[nextIdx] ) );
                        mFrameSyncVec[nextIdx] = 0;
                    }
                    nextIdx = (nextIdx + 1u) % mDynamicBufferMultiplier;
                }
            }
        }
        else
        {
            //No stall
        }
    }
    //-----------------------------------------------------------------------------------
    bool GLGBufferManager::isFrameFinished( NCount frameCount )
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
            const size_t idx = (mFrameMark + mDynamicBufferMultiplier - frameDiff) % mDynamicBufferMultiplier;

            if( mFrameSyncVec[idx] )
            {
                //Ask GL API to return immediately and tells us about the fence
                GLenum waitRet = glClientWaitSync( mFrameSyncVec[idx], 0, 0 );
                if( waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED )
                {
                    //Delete all the fences until this frame we've just waited.
                    size_t nextIdx = mFrameMark;
                    while( nextIdx != idx )
                    {
                        if( mFrameSyncVec[nextIdx] )
                        {
                            GLCheck( glDeleteSync( mFrameSyncVec[nextIdx] ) );
                            mFrameSyncVec[nextIdx] = 0;
                        }
                        nextIdx = (nextIdx + 1u) % mDynamicBufferMultiplier;
                    }

                    retVal = true;
                }
            }
            else
            {
                retVal = true;
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
    GLsync GLGBufferManager::waitFor( GLsync fenceName )
    {
        GLbitfield waitFlags    = 0;
        GLuint64 waitDuration   = 0;
        while( true )
        {
            GLenum waitRet = glClientWaitSync( fenceName, waitFlags, waitDuration );
            if( waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED )
            {
                GLCheck( glDeleteSync( fenceName ) );
                return 0;
            }

            if( waitRet == GL_WAIT_FAILED )
            {
                N_EXCEPT(Internal, "Failure while waiting for a GL Fence. Could be out of GPU memory. "
                    "Update your video card drivers. If that doesn't help, contact the developers." );

                return fenceName;
            }

            // After the first time, need to start flushing, and wait for a looong time.
            waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
            waitDuration = kOneSecondInNanoSeconds;
        }

        return fenceName;
    }
    //-----------------------------------------------------------------------------------
    GLuint GLGBufferManager::getAttributeIndexFor(VertexType type)
    {
        return GLSLProgramBase::DefaultAttributeLayout[type];
    }
    //-----------------------------------------------------------------------------------
    GLGBufferManager::VboFlag GLGBufferManager::modeToFlag(BufferModeMark mode)
    {
        //M_HOSTREAD
        //M_DEVWRITE

        if((mode & M_HOSTWRITE) && (mode & M_DEVREAD) && (mode & M_PersistentMap) && (mode & M_CoherentMap))
            return CPU_ACCESSIBLE_PERSISTENT_COHERENT;
        else if((mode & M_HOSTWRITE) && (mode & M_DEVREAD) && (mode & M_PersistentMap))
            return CPU_ACCESSIBLE_PERSISTENT;
        else if((mode & M_HOSTWRITE) && (mode & M_DEVREAD))
            return CPU_ACCESSIBLE_DEFAULT;

        return CPU_INACCESSIBLE;
    }
    //-----------------------------------------------------------------------------------
}