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

#include "OgreGLES2HardwareBufferManager.h"
#include "OgreGLES2HardwareVertexBuffer.h"
#include "OgreGLES2HardwareIndexBuffer.h"
#include "OgreGLES2HardwareUniformBuffer.h"
#include "OgreGLES2RenderToVertexBuffer.h"
#include "OgreGLES2RenderSystem.h"
#include "OgreGLVertexArrayObject.h"
#include "OgreGLUtil.h"
#include "OgreRoot.h"

namespace NII 
{
    //-----------------------------------------------------------------------
    GLES2HardwareBufferManager::GLES2HardwareBufferManager()
    {
        mRenderSys = getGLES2RenderSystem();
    }
    //-----------------------------------------------------------------------
    GLES2HardwareBufferManager::~GLES2HardwareBufferManager()
    {
        destroyAllDeclarations();
        destroyAllBindings();
    }
    //-----------------------------------------------------------------------
    GLES2StateCacheManager * GLES2HardwareBufferManager::getStateManager()
    {
        return mRenderSys->getStateManager();
    }
    //-----------------------------------------------------------------------
    void GLES2HardwareBufferManager::notifyContextDestroyed(GLContext* context)
    {
        OGRE_LOCK_MUTEX(mVertexDeclarationsMutex);
        for(VertexDeclarationList::iterator it = mVertexDeclarations.begin(), it_end = mVertexDeclarations.end(); it != it_end; ++it)
            static_cast<GLVertexArrayObject*>(*it)->notifyContextDestroyed(context);
    }
    //-----------------------------------------------------------------------
    HardwareVertexBufferSharedPtr GLES2HardwareBufferManager::createVertexBuffer(size_t vertexSize, size_t numVerts, Buffer::Usage usage, bool useShadowBuffer)
    {
        if(!mRenderSys->getCapabilities()->hasCapability(RSC_MAPBUFFER))
            useShadowBuffer = true;

        GLES2VertexBuffer* buf = N_new GLES2VertexBuffer(this, vertexSize, numVerts, usage, useShadowBuffer);

        {
            OGRE_LOCK_MUTEX(mVertexBuffersMutex);
            mVertexBuffers.insert(buf);
        }
        return HardwareVertexBufferSharedPtr(buf);
    }
    //-----------------------------------------------------------------------
    HardwareIndexBufferSharedPtr GLES2HardwareBufferManager::createIndexBuffer(IndexBuffer::IndexType itype, size_t numIndexes, Buffer::Usage usage, bool useShadowBuffer)
    {
        if(!mRenderSys->getCapabilities()->hasCapability(RSC_MAPBUFFER))
            useShadowBuffer = true;

        GLES2IndexBuffer* buf = N_new GLES2IndexBuffer(this, itype, numIndexes, usage, useShadowBuffer);

        {
            OGRE_LOCK_MUTEX(mIndexBuffersMutex);
            mIndexBuffers.insert(buf);
        }
        return HardwareIndexBufferSharedPtr(buf);
    }
    //-----------------------------------------------------------------------
    RenderToVertexBufferSharedPtr GLES2HardwareBufferManager::createRenderToVertexBuffer()
    {
        if(mRenderSys->hasMinGLVersion(3, 0))
            return RenderToVertexBufferSharedPtr(new GLES2RenderToVertexBuffer());

        // not supported
        return RenderToVertexBufferSharedPtr();
    }
    //-----------------------------------------------------------------------
    VertexDeclaration* GLES2HardwareBufferManager::createVertexDeclarationImpl(void)
    {
        return N_new GLVertexArrayObject();
    }
    //-----------------------------------------------------------------------
    void GLES2HardwareBufferManager::destroyVertexDeclarationImpl(VertexDeclaration* decl)
    {
        if(decl)
            N_delete decl;
    }
    //-----------------------------------------------------------------------
    GLenum GLES2HardwareBufferManager::getGLType(DataType type)
    {
        switch(type)
        {
            case DT_Float1x:
            case VET_FLOAT2:
            case VET_FLOAT3:
            case VET_FLOAT4:
                return GL_FLOAT;
            case VET_SHORT1:
            case DT_Short2x:
            case VET_SHORT3:
            case VET_SHORT4:
            case VET_SHORT2_NORM:
            case VET_SHORT4_NORM:
                return GL_SHORT;
            case DT_Colour:
            case DT_Colour_ABGR:
            case DT_Colour_ARGB:
            case DT_UByte4x:
            case VET_UBYTE4_NORM:
                return GL_UNSIGNED_BYTE;
            case DT_Byte4x:
            case VET_BYTE4_NORM:
                return GL_BYTE;
            case DT_Int1x:
            case VET_INT2:
            case VET_INT3:
            case VET_INT4:
                return GL_INT;
            case DT_UInt1x:
            case VET_UINT2:
            case VET_UINT3:
            case VET_UINT4:
                return GL_UNSIGNED_INT;
            case VET_USHORT1:
            case DT_UShort2x:
            case VET_USHORT3:
            case VET_USHORT4:
            case VET_USHORT2_NORM:
            case VET_USHORT4_NORM:
                return GL_UNSIGNED_SHORT;
            case DT_Double1x:
            case VET_DOUBLE2:
            case VET_DOUBLE3:
            case VET_DOUBLE4:
                return 0;
        };

        OgreAssert(false, "unknown Vertex Element Type");
        return 0;
    }
    //---------------------------------------------------------------------
    Ogre::StructBuffer * GLES2HardwareBufferManager::createUniformBuffer( size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name )
    {
        if(!mRenderSys->hasMinGLVersion(3, 0))
        {
            N_EXCEPT(RenderingAPI, "GLES2 does not support uniform buffer objects");
        }

        GLES2HardwareUniformBuffer* buf = new GLES2HardwareUniformBuffer(this, sizeBytes, usage, useShadowBuffer, name);
        {
            OGRE_LOCK_MUTEX(mUniformBuffersMutex);
            mUniformBuffers.insert(buf);
        }
        return StructBuffer *(buf);
    }
    //---------------------------------------------------------------------
    Ogre::CounterBuffer * GLES2HardwareBufferManager::createCounterBuffer( size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name )
    {
        N_EXCEPT(RenderingAPI, "GLES2 does not support atomic counter buffers");
    }
    //-------------------------------------------------------------------------------------------------------------
    extern const GLuint64 kOneSecondInNanoSeconds;
    const GLuint64 kOneSecondInNanoSeconds = 1000000000;
    //-----------------------------------------------------------------------
    const GLuint GLES2VaoManager::VERTEX_ATTRIBUTE_INDEX[VES_COUNT] =
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

        //VT_Binormal uses slot 16. Lots of GPUs don't support more than 16 attributes
        //(even very modern ones like the GeForce 680). Since Binormal is rarely used, it
        //is technical (not artist controlled, unlike UVs) and can be replaced by a
        //4-component VT_Tangent, we leave this one for the end.
        16, // VT_Binormal - 1
        2,  // VT_Tangent - 1
        13, // VT_Custom7 - 1
        14, // VT_Custom8 - 1
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLES2VaoManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLES2VaoManager::GLES2VaoManager() :
        mArbBufferStorage( false ),
        mEmulateTexBuffers( true ),
        mMaxVertexAttribs( 30 ),
        mDrawId( 0 )
    {
        //Keep pools of 128MB each for static meshes
        mDefaultPoolSize[CPU_INACCESSIBLE]  = 0; // hasMinGLVersion(3, 2) ? 128 * 1024 * 1024 : 0;

        //Keep pools of 32MB each for dynamic vertex buffers
        for( size_t i=CPU_ACCESSIBLE_DEFAULT; i<=CPU_ACCESSIBLE_PERSISTENT_COHERENT; ++i )
            mDefaultPoolSize[i] = 0; // hasMinGLVersion(3, 2) ? 32 * 1024 * 1024 : 0;

        mFrameSyncVec.resize( mDynamicBufferMultiplier, 0 );

        GLCheck( glGetIntegerv( GLenum(GL_MAX_VERTEX_ATTRIBS), &mMaxVertexAttribs ) );

        if( mMaxVertexAttribs < 16 )
        {
            N_EXCEPT( RenderingAPI,
                         "GL_MAX_VERTEX_ATTRIBS = " + StrConv::conv( mMaxVertexAttribs ) +
                         " this value must be >= 16 for Ogre to function properly. Try updating your video card drivers." );
        }

        //The minimum alignment for these buffers is 16 because some
        //places of Ogre assume such alignment for SIMD reasons.
        GLint alignment = 1; //initial value according to specs
        GLCheck( glGetIntegerv( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment ) );
        mAlignment[GBT_Struct] = std::max<uint32>( alignment, 16u );
        mAlignment[GBT_Texture] = 16;
//        if( !emulateTexBuffers )
//        {
//            alignment = 1; //initial value according to specs
//            GLCheck( glGetIntegerv( GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &alignment ) );
//            mAlignment[GBT_Texture] = std::max<uint32>( alignment, 16u );
//        }
//        if( _supportsSsbo )
//        {
//            alignment = 1; //initial value according to specs
//            GLCheck( glGetIntegerv( GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment ) );
//            mAlignment[GBT_Storage] = std::max<uint32>( alignment, 16u );
//        }

        GLint maxBufferSize = 16384; //minimum value according to specs
        GLCheck( glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &maxBufferSize ) );
        mMaxSize[GBT_Struct] = static_cast<size_t>( maxBufferSize );
        maxBufferSize = 65536; //minimum value according to specs
        maxBufferSize = 16 * 1024 * 1024; // GLCheck( glGetIntegerv( GL_MAX_TEXTURE_BUFFER_SIZE, &maxBufferSize ) );
        mMaxSize[GBT_Texture] = static_cast<size_t>( maxBufferSize );
//        if( _supportsSsbo )
//        {
//            GLCheck( glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBufferSize ) );
//            mMaxSize[GBT_Storage] = static_cast<size_t>( maxBufferSize );
//        }

        VertexUnitList units;
        units.push_back( VertexUnit( DT_UInt1x, VES_COUNT ) );
        uint32 * drawIdPtr = static_cast<uint32 *>( OGRE_MALLOC_SIMD( 4096 * sizeof(uint32), MEMCATEGORY_GEOMETRY ) );
        for( uint32 i=0; i<4096; ++i )
            drawIdPtr[i] = i;
        mDrawId = createVertexBuffer( units, 4096, M_DEVREAD, drawIdPtr, true );
    }
    //-----------------------------------------------------------------------------------
    GLES2VaoManager::~GLES2VaoManager()
    {
        vector<GLuint>::type bufferNames;

        bufferNames.reserve( mRefMappedList.size() + mFreeMappedList.size() );

        //Collect the buffer names from all staging buffers to use one API call
        MappedBufferList::const_iterator itor = mRefMappedList.begin();
        MappedBufferList::const_iterator end  = mRefMappedList.end();

        while( itor != end )
        {
            bufferNames.push_back( static_cast<GLES2MappedBuffer*>(*itor)->getBufferName() );
            ++itor;
        }

        itor = mFreeMappedList.begin();
        end  = mFreeMappedList.end();

        while( itor != end )
        {
            bufferNames.push_back( static_cast<GLES2MappedBuffer*>(*itor)->getBufferName() );
            ++itor;
        }

        for( size_t i=0; i<VF_MAX; ++i )
        {
            //Free pointers and collect the buffer names from all VBOs to use one API call
            VboVec::iterator itor = mVbos[i].begin();
            VboVec::iterator end  = mVbos[i].end();

            while( itor != end )
            {
                bufferNames.push_back( itor->vboName );
                delete itor->mBuffer;
                itor->mBuffer = 0;
                ++itor;
            }
        }

        if( !bufferNames.empty() )
        {
            GLCheck( glDeleteBuffers( bufferNames.size(), &bufferNames[0] ) );
            bufferNames.clear();
        }

        GLSyncVec::const_iterator itor = mFrameSyncVec.begin();
        GLSyncVec::const_iterator end  = mFrameSyncVec.end();

        while( itor != end )
        {
            GLCheck( glDeleteSync( *itor ) );
            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::allocateVbo(size_t sizeBytes, size_t alignment, BufferModeMark bufferType,
        size_t & outVboIdx, size_t & outBufferOffset)
    {
        N_assert( alignment > 0 );

        VboFlag vboFlag = modeToFlag( bufferType );

        if(N_MarkTrue(bufferType, M_NORMAL) )
            sizeBytes *= mDynamicBufferMultiplier;

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
                const Block & block = *blockIt;

                //Round to next multiple of alignment
                size_t newOffset = ( (block.offset + alignment - 1) / alignment ) * alignment;
                size_t padding = newOffset - block.offset;

                if( sizeBytes + padding <= block.size )
                {
                    bestVboIdx = itor - mVbos[vboFlag].begin();
                    bestBlockIdx = blockIt - itor->mFreeList.begin();

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

            size_t poolSize = std::max( mDefaultPoolSize[vboFlag], sizeBytes );

            //No luck, allocate a new buffer.
            GLCheck( glGenBuffers( 1, &newVbo.vboName ) );
            GLCheck( glBindBuffer( GL_ARRAY_BUFFER, newVbo.vboName ) );

            GLenum error = 0;
            int trustCounter = 1000;
            //Reset the error code. Trust counter prevents an infinite loop
            //just in case we encounter a moronic GL implementation.
            while( glGetError() && trustCounter-- );

//            if( mArbBufferStorage )
//            {
//                GLbitfield flags = 0;
//
//                if( vboFlag >= CPU_ACCESSIBLE_DEFAULT )
//                {
//                    flags |= GL_MAP_WRITE_BIT;
//
//                    if( vboFlag >= CPU_ACCESSIBLE_PERSISTENT )
//                    {
//                        flags |= GL_MAP_PERSISTENT_BIT;
//
//                        if( vboFlag >= CPU_ACCESSIBLE_PERSISTENT_COHERENT )
//                            flags |= GL_MAP_COHERENT_BIT;
//                    }
//                }
//
//                glBufferStorage( GL_ARRAY_BUFFER, poolSize, 0, flags );
//
//                error = glGetError();
//            }
//            else
            {
                glBufferData( GL_ARRAY_BUFFER, poolSize, 0, vboFlag == CPU_INACCESSIBLE ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );

                error = glGetError();
            }

            //OpenGL can't continue after any GL_OUT_OF_MEMORY has been raised,
            //thus ignore the trustCounter in that case.
            if( (error != 0 && trustCounter != 0) || error == GL_OUT_OF_MEMORY )
            {
                N_EXCEPT( RenderingAPI,
                             "Out of GPU memory or driver refused.\n"
                             "glGetError code: " + StrConv::conv( error ) + ".\n"
                             "Requested: " + StrConv::conv( poolSize ) + " bytes." );
            }

            GLCheck( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );

            newVbo.sizeBytes = poolSize;
            newVbo.mFreeList.push_back( Block( 0, poolSize ) );
            newVbo.mBuffer = 0;

            if( vboFlag != CPU_INACCESSIBLE )
            {
                newVbo.mBuffer = new GLES2Buffer( newVbo.vboName, newVbo.sizeBytes, this, bufferType );
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
    void GLES2VaoManager::deallocateVbo(size_t vboIdx, size_t bufferOffset, size_t sizeBytes, BufferModeMark bufferType)
    {
        VboFlag vboFlag = modeToFlag(bufferType);

        if(N_MarkTrue(bufferType, M_NORMAL))
            sizeBytes *= mDynamicBufferMultiplier;

        Vbo & vbo = mVbos[vboFlag][vboIdx];
        StrideChangerVec::iterator itStride = std::lower_bound( vbo.strideChangers.begin(), vbo.strideChangers.end(),
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
    void GLES2VaoManager::mergeContiguousBlocks(BlockVec::iterator blockToMerge, BlockVec & blocks)
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
    VertexBufferPacked* GLES2VaoManager::createVertexBufferImpl( size_t numElements,
                                                                   uint32 bytesPerElement,
                                                                   BufferModeMark bufferType,
                                                                   void *initialData, bool keepAsShadow,
                                                                   const VertexUnitList &vElements )
    {
        size_t vboIdx;
        size_t bufferOffset;

        allocateVbo( numElements * bytesPerElement, bytesPerElement, bufferType, vboIdx, bufferOffset );

        VboFlag vboFlag = modeToFlag( bufferType );
        Vbo &vbo = mVbos[vboFlag][vboIdx];
        GLES2BufferOp *op = new GLES2BufferOp( vboIdx, vbo.vboName, vbo.mBuffer );

        VertexBufferPacked *retVal = N_new VertexBufferPacked( bufferOffset, numElements, bytesPerElement, 0,
            bufferType, initialData, keepAsShadow, this, op, vElements, 0, 0, 0 );

        if( initialData )
            op->_init( initialData, 0, numElements );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::destroyVertexBufferImpl(VertexBufferPacked * vertexBuffer)
    {
        GLES2BufferOp *op = static_cast<GLES2BufferOp*>(vertexBuffer->getOperation() );

        deallocateVbo( op->getVboPoolIndex(),
                       vertexBuffer->getOffset() * vertexBuffer->getUnitSize(),
                       vertexBuffer->getAllocSize(),
                       vertexBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    IndexBufferPacked* GLES2VaoManager::createIndexBufferImpl( size_t numElements,
        uint32 bytesPerElement, BufferModeMark bufferType, void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;

        allocateVbo( numElements * bytesPerElement, bytesPerElement, bufferType, vboIdx, bufferOffset );

        VboFlag vboFlag = modeToFlag( bufferType );

        Vbo & vbo = mVbos[vboFlag][vboIdx];
        GLES2BufferOp * op = new GLES2BufferOp( vboIdx, vbo.vboName, vbo.mBuffer );
        IndexBufferPacked * retVal = N_new IndexBufferPacked(bufferOffset, numElements, bytesPerElement, 0,
                                                        bufferType, initialData, keepAsShadow, this, op );

        if( initialData )
            op->_init( initialData, 0, numElements );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::destroyIndexBufferImpl( IndexBufferPacked *indexBuffer )
    {
        GLES2BufferOp * op = static_cast<GLES2BufferOp*>(indexBuffer->getOperation() );

        deallocateVbo( op->getVboPoolIndex(), indexBuffer->getOffset() * indexBuffer->getUnitSize(),
                       indexBuffer->getAllocSize(), indexBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    StructBuffer* GLES2VaoManager::createConstBufferImpl( size_t sizeBytes, BufferModeMark bufferType,
                                                                 void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;

        GLint alignment = mAlignment[GBT_Struct];
        size_t requestedSize = sizeBytes;

        VboFlag vboFlag = modeToFlag( bufferType );

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
        GLES2BufferOp *op = new GLES2BufferOp( vboIdx, vbo.vboName, vbo.mBuffer );
        StructBuffer *retVal = N_new GLES2StructBuffer(bufferOffset, requestedSize, 1,
                                                        (sizeBytes - requestedSize) / 1,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, op );

        if( initialData )
            op->_init( initialData, 0, requestedSize );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::destroyConstBufferImpl(StructBuffer * constBuffer)
    {
        GLES2BufferOp * op = static_cast<GLES2BufferOp *>(constBuffer->getOperation() );

        deallocateVbo( op->getVboPoolIndex(),
                       constBuffer->getOffset() * constBuffer->getUnitSize(),
                       constBuffer->getAllocSize(),
                       constBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    TextureBuffer* GLES2VaoManager::createTexBufferImpl( PixelFormat pf, size_t sizeBytes,
        BufferModeMark bufferType, void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;

        GLint alignment = mAlignment[GBT_Texture];
        size_t requestedSize = sizeBytes;

        VboFlag vboFlag = modeToFlag( bufferType );

        if( mEmulateTexBuffers )
        {
            // Align to the texture size since we must copy the PBO to a texture.
            ushort maxTexSizeBytes = 2048 * PixelUtil::getNumElemBytes( pf );
            // We need another line of maxTexSizeBytes for uploading
            //to create a rectangle when calling glTexSubImage2D().
            if(sizeBytes > maxTexSizeBytes)
                sizeBytes += maxTexSizeBytes;
        }
        
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
        GLES2BufferOp *op = new GLES2BufferOp( vboIdx, vbo.vboName, vbo.mBuffer );
        TextureBuffer *retVal;

//        if( !mEmulateTexBuffers )
//        {
//            retVal = N_new GLES2TexBufferPacked( bufferOffset, requestedSize, 1,
//                                                      (sizeBytes - requestedSize) / 1,
//                                                      bufferType, initialData, keepAsShadow,
//                                                      this, op, pf );
//        }
//        else
        {
            retVal = N_new GLES2TextureBufferEmu( bufferOffset, requestedSize, 1,
                                                              (sizeBytes - requestedSize) / 1,
                                                              bufferType, initialData, keepAsShadow,
                                                              this, op, pf );
        }

        if( initialData )
            op->_init( initialData, 0, requestedSize );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::destroyTexBufferImpl( TextureBuffer *texBuffer )
    {
        GLES2BufferOp *op = static_cast<GLES2BufferOp*>(texBuffer->getOperation() );
        
        deallocateVbo( op->getVboPoolIndex(), texBuffer->getOffset() * texBuffer->getUnitSize(),
                       texBuffer->getAllocSize(), texBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    StorageBuffer* GLES2VaoManager::createUavBufferImpl( size_t numElements, uint32 bytesPerElement,
        uint32 bindFlags, void *initialData, bool keepAsShadow )
    {
        return NULL;
//        size_t vboIdx;
//        size_t bufferOffset;
//
//        GLint alignment = mAlignment[GBT_Storage];
//
//        //UAV Buffers can't be dynamic.
//        const BufferModeMark bufferType = M_DEVNORMAL;
//        VboFlag vboFlag = modeToFlag( bufferType );
//
//        allocateVbo( numElements * bytesPerElement, alignment, bufferType, vboIdx, bufferOffset );
//
//        Vbo &vbo = mVbos[vboFlag][vboIdx];
//        GLES2BufferOp *op = new GLES2BufferOp( vboIdx, vbo.vboName,
//                                                                              vbo.mBuffer );
//        StorageBuffer *retVal = N_new GLES2UavBufferPacked(
//                                                        bufferOffset, numElements, bytesPerElement,
//                                                        bindFlags, initialData, keepAsShadow,
//                                                        this, op );
//
//        if( initialData )
//            op->_init( initialData, 0, numElements );
//
//        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::destroyUavBufferImpl( StorageBuffer *uavBuffer )
    {
//        GLES2BufferOp *op = static_cast<GLES2BufferOp*>(
//                                                        uavBuffer->getOperation() );
//
//
//        deallocateVbo( op->getVboPoolIndex(),
//                       uavBuffer->getOffset() * uavBuffer->getUnitSize(),
//                       uavBuffer->getAllocSize(),
//                       uavBuffer->mMark );
    }
    //-----------------------------------------------------------------------------------
    IndirectBuffer* GLES2VaoManager::createIndirectBufferImpl( size_t sizeBytes,
        BufferModeMark bufferType, void *initialData, bool keepAsShadow )
    {
        const size_t alignment = 4;
        size_t bufferOffset = 0;
        size_t requestedSize = sizeBytes;

        if(N_MarkTrue(bufferType, M_NORMAL))
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //offset after each map to be aligned; and for that, we
            //sizeBytes to be multiple of alignment.
            sizeBytes = multiCeil( sizeBytes, alignment );
        }

        GLES2BufferOp *op = 0;
        if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
        {
            size_t vboIdx;
            VboFlag vboFlag = modeToFlag( bufferType );

            allocateVbo( sizeBytes, alignment, bufferType, vboIdx, bufferOffset );

            Vbo &vbo = mVbos[vboFlag][vboIdx];
            op = new GLES2BufferOp( vboIdx, vbo.vboName, vbo.mBuffer );
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
    void GLES2VaoManager::destroyIndirectBufferImpl( IndirectBuffer * obj )
    {
        if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
        {
            GLES2BufferOp * op = static_cast<GLES2BufferOp *>(obj->getOperation() );

            deallocateVbo( op->getVboPoolIndex(), obj->getOffset() * obj->getUnitSize(),
                           obj->getAllocSize(), obj->mMark );
        }
    }
    //-----------------------------------------------------------------------------------
    GLuint GLES2VaoManager::createVao( const Vao &vaoRef )
    {
        GLuint vaoName;
        GLCheck( glGenVertexArrays( 1, &vaoName ) );
        GLCheck( glBindVertexArray( vaoName ) );

        GLuint uvCount = 0;

        for( size_t i=0; i<vaoRef.vertexBuffers.size(); ++i )
        {
            const Vao::VertexBinding &binding = vaoRef.vertexBuffers[i];

            glBindBuffer( GL_ARRAY_BUFFER, binding.vertexBufferVbo );

            VertexUnitList::const_iterator it = binding.mUnits.begin();
            VertexUnitList::const_iterator en = binding.mUnits.end();

            size_t bindAccumOffset = 0;

            while( it != en )
            {
                GLint typeCount = v1::VertexElement::getTypeCount( it->mType );
                GLboolean normalised = DataTypeUtil::isUnit( it->mType ) ? GL_TRUE :
                                                                                          GL_FALSE;
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
                    break;
                };

                GLuint attributeIndex = VERTEX_ATTRIBUTE_INDEX[it->mVType];

                if( it->mVType == VT_Tex_Coord )
                {
                    N_assert( uvCount < 8 && "Up to 8 UVs are supported." );
                    attributeIndex += uvCount;
                    ++uvCount;
                }

                N_assert( ( uvCount < 6 || (it->mVType != VT_Custom7 &&
                                          it->mVType != VT_Custom8) ) &&
                        "Available UVs get reduced from 8 to 6 when"
                        " VT_Custom7/INDICES2 is present" );

                if( it->mVType == VT_Binormal )
                {
                    LogManager::getSingleton().logMessage(
                                "WARNING: VT_Binormal will not render properly in "
                                "many GPUs where GL_MAX_VERTEX_ATTRIBS = 16. Consider"
                                " changing for VT_Tangent with 4 components or use"
                                " QTangents", LML_CRITICAL );
                }

                switch( v1::VertexElement::getBaseType( it->mType ) )
                {
                default:
                case DT_Float1x:
                    GLCheck( glVertexAttribPointer( attributeIndex, typeCount,
                                                 v1::GLES2HardwareBufferManager::getGLType( it->mType ),
                                                 normalised, binding.stride,
                                                 (void*)(binding.offset + bindAccumOffset) ) );
                    break;
                case DT_Byte4x:
                case DT_UByte4x:
                case DT_Short2x:
                case DT_UShort2x:
                case DT_UInt1x:
                case DT_Int1x:
                    GLCheck( glVertexAttribIPointer( attributeIndex, typeCount,
                                                  v1::GLES2HardwareBufferManager::getGLType( it->mType ),
                                                  binding.stride,
                                                  (void*)(binding.offset + bindAccumOffset) ) );
                    break;
//                case DT_Double1x:
//                    GLCheck( glVertexAttribLPointer( attributeIndex, typeCount,
//                                                  v1::GLES2HardwareBufferManager::getGLType( it->mType ),
//                                                  binding.stride,
//                                                  (void*)(binding.offset + bindAccumOffset) ) );
//                    break;
                }

                GLCheck( glVertexAttribDivisor( attributeIndex, binding.instancingDivisor ) );
                GLCheck( glEnableVertexAttribArray( attributeIndex ) );

                bindAccumOffset += v1::VertexElement::getTypeSize( it->mType );

                ++it;
            }

            GLCheck( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
        }

        {
            //Now bind the Draw ID.
            bindDrawId();
        }

        if( vaoRef.indexBufferVbo )
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vaoRef.indexBufferVbo );

        GLCheck( glBindVertexArray( 0 ) );

        return vaoName;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::bindDrawId(void)
    {
        GLES2BufferOp *drawIdBufferInterface = static_cast<GLES2BufferOp*>(mDrawId->getOperation() );
        const GLuint drawIdIdx = 15;
        GLCheck( glBindBuffer( GL_ARRAY_BUFFER, drawIdBufferInterface->getVboName() ) );
        GLCheck( glVertexAttribIPointer( drawIdIdx, 1, GL_UNSIGNED_INT, sizeof(uint32),
                                     (void*)(mDrawId->getCurrentOffset() * mDrawId->getUnitSize()) ) );
        GLCheck( glVertexAttribDivisor( drawIdIdx, 1 ) );
        GLCheck( glEnableVertexAttribArray( drawIdIdx ) );
        GLCheck( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
    }
    //-----------------------------------------------------------------------------------
    GeometryRaw* GLES2VaoManager::createVertexArrayObjectImpl(const VertexBufferList &vertexBuffers,
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
                vertexBinding.vertexBufferVbo   = static_cast<GLES2BufferOp*>((*itor)->getOperation() )->getVboName();
                vertexBinding.mUnits    = (*itor)->getUnits();
                vertexBinding.stride            = calculateVertexSize( vertexBinding.mUnits );
                vertexBinding.offset            = 0;
                vertexBinding.instancingDivisor = 0;

                const MultiSourceVertexBufferPool *multiSourcePool = (*itor)->getMultiSourcePool();
                if( multiSourcePool )
                {
                    vertexBinding.offset = multiSourcePool->getBytesOffsetToSource((*itor)->_getSourceIndex() );
                }

                vao.vertexBuffers.push_back( vertexBinding );

                ++itor;
            }
        }

        vao.refCount = 0;

        if( indexBuffer )
        {
            vao.indexBufferVbo  = static_cast<GLES2BufferOp*>(indexBuffer->getOperation() )->getVboName();
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
        const int bitsVaoGl  = 5;
        const uint32 maskVaoGl  = N_Mask( bitsVaoGl );
        const uint32 maskVao    = N_Mask( RqBits::MeshBits - bitsVaoGl );

        const uint32 shiftVaoGl = RqBits::MeshBits - bitsVaoGl;

        uint32 renderQueueId = ( (itor->vaoName & maskVaoGl) << shiftVaoGl ) | (mValidVAO & maskVao);

        GLES2VertexArrayObject *retVal = N_new GLES2VertexArrayObject( itor->vaoName,
                                                                              renderQueueId,
                                                                              vertexBuffers,
                                                                              indexBuffer,
                                                                              opType );

        ++itor->refCount;

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::destroyVertexArrayObjectImpl( GeometryRaw *vao )
    {
        GLES2VertexArrayObject *glVao = static_cast<GLES2VertexArrayObject*>( vao );

        VaoVec::iterator itor = mVaos.begin();
        VaoVec::iterator end  = mVaos.end();

        while( itor != end && itor->vaoName != glVao->mVaoId )
            ++itor;

        if( itor != end )
        {
            --itor->refCount;

            if( !itor->refCount )
            {
				GLuint vaoName = glVao->mVaoId;
                GLCheck( glDeleteVertexArrays( 1, &vaoName ) );

                remove( mVaos, itor );
            }
        }

        //We delete it here because this class has no virtual destructor on purpose
        N_delete glVao;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::createImpl(MappedBuffer*& buf, NCount sizeBytes, Nmark modemark )
    {
        sizeBytes = std::max<size_t>( sizeBytes, 4 * 1024 * 1024 );

        GLuint bufferName;
        GLenum target = forUpload ? GL_COPY_READ_BUFFER : GL_COPY_WRITE_BUFFER;
        GLCheck( glGenBuffers( 1, &bufferName ) );
        GLCheck( glBindBuffer( target, bufferName ) );

//        if( mArbBufferStorage )
//        {
//            GLCheck( glBufferStorage( target, sizeBytes, 0,
//                                    forUpload ? GL_MAP_WRITE_BIT : GL_MAP_READ_BIT ) );
//        }
//        else
        {
            GLCheck( glBufferData( target, sizeBytes, 0, forUpload ? GL_STREAM_DRAW : GL_STREAM_READ ) );
        }

        GLES2MappedBuffer *stagingBuffer = N_new GLES2MappedBuffer( 0, sizeBytes, this, forUpload, bufferName );
        mRefMappedList.push_back( stagingBuffer );

        if( mNextCheck == (unsigned long)( ~0 ) )
            mNextCheck = mTimer->getMilliseconds() + mDefaultStagingBufferLifetime;

        return stagingBuffer;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::create(GpuBufferAsync *& out, GpuBuffer *creator, MappedBuffer *buf, NCount oft, NCount size )
    {
        out = N_new GLES2AsyncTicket( creator, buf, oft, size ) ;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::_update(void)
    {
        unsigned long currentTimeMs = mTimer->getMilliseconds();

        vectorL<GLuint> bufferNames;

        if( currentTimeMs >= mNextCheck )
        {
            mNextCheck = (unsigned long)(~0);

            //for( size_t i=0; i<2; ++i )
            //{
                MappedBufferList::iterator itor = mFreeMappedList.begin();
                MappedBufferList::iterator end  = mFreeMappedList.end();

                while( itor != end )
                {
                    MappedBuffer *stagingBuffer = *itor;

                    mNextCheck = std::min(
                        mNextCheck, 
                        stagingBuffer->getLastUsedTimestamp() + stagingBuffer->getLifetimeThreshold() );

                    if( stagingBuffer->getLastUsedTimestamp() + stagingBuffer->getUnfencedTimeThreshold() < currentTimeMs )
                    {
                        static_cast<GLES2MappedBuffer*>( stagingBuffer )->cleanUnfencedHazards();
                    }

                    if( stagingBuffer->getLastUsedTimestamp() + stagingBuffer->getLifetimeThreshold() < currentTimeMs )
                    {
                        //Time to delete this buffer.
                        bufferNames.push_back( static_cast<GLES2MappedBuffer*>(stagingBuffer)->getBufferName() );

                        delete *itor;

                        itor = remove( mFreeMappedList, itor );
                        end  = mFreeMappedList.end();
                    }
                    else
                    {
                        ++itor;
                    }
                }
            //}
        }

        if( !bufferNames.empty() )
        {
            GLCheck( glDeleteBuffers( bufferNames.size(), &bufferNames[0] ) );
            bufferNames.clear();
        }

        if( !mRecoverList.empty() &&
            mRecoverList.front().mFrameMark == mFrameMark )
        {
            wait();
            destroyRecover( mFrameMark );
        }

        GBufferManager::_update();

        if( mFrameSyncVec[mFrameMark] )
        {
            GLCheck( glDeleteSync( mFrameSyncVec[mFrameMark] ) );
        }
        GLCheck( mFrameSyncVec[mFrameMark] = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
        mFrameMark = (mFrameMark + 1) % mDynamicBufferMultiplier;
    }
    //-----------------------------------------------------------------------------------
    uint8 GLES2VaoManager::wait(void)
    {
        if( mFrameSyncVec[mFrameMark] )
        {
            waitFor( mFrameSyncVec[mFrameMark] );
            mFrameSyncVec[mFrameMark] = 0;
        }

        return mFrameMark;
    }
    //-----------------------------------------------------------------------------------
    void GLES2VaoManager::wait( uint32 frameCount )
    {
        if( frameCount == mFrameCount )
        {
            //Full stall
            glFinish();

            //All of the other per-fences are not needed anymore.
            GLSyncVec::const_iterator itor = mFrameSyncVec.begin();
            GLSyncVec::const_iterator end  = mFrameSyncVec.end();

            while( itor != end )
            {
                if( *itor )
                    GLCheck( glDeleteSync( *itor ) );
                ++itor;
            }
        }
        else if( mFrameCount - frameCount <= mDynamicBufferMultiplier )
        {
            //Let's wait on one of our existing fences...
            //frameDiff has to be in range [1; mDynamicBufferMultiplier]
            size_t frameDiff = mFrameCount - frameCount;
            const size_t idx = (mFrameMark +
                                mDynamicBufferMultiplier - frameDiff) % mDynamicBufferMultiplier;
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
    bool GLES2VaoManager::isFrameFinished( uint32 frameCount )
    {
        bool retVal = true;
        if( frameCount == mFrameCount )
        {
            //Full stall
            //retVal = true;
        }
        else if( mFrameCount - frameCount <= mDynamicBufferMultiplier )
        {
            //frameDiff has to be in range [1; mDynamicBufferMultiplier]
            size_t frameDiff = mFrameCount - frameCount;
            const size_t idx = (mFrameMark +
                                mDynamicBufferMultiplier - frameDiff) % mDynamicBufferMultiplier;

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

                    retVal = false;
                }
            }
            else
            {
                retVal = false;
            }
        }
        else
        {
            //No stall
            retVal = false;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    GLsync GLES2VaoManager::waitFor( GLsync fenceName )
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
                N_EXCEPT( RenderingAPI,
                             "Failure while waiting for a GL Fence. Could be out of GPU memory. "
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
    GLuint GLES2VaoManager::getAttributeIndexFor( VertexType vtype )
    {
        return VERTEX_ATTRIBUTE_INDEX[vtype];
    }
    //-----------------------------------------------------------------------------------
    GLES2VaoManager::VboFlag GLES2VaoManager::modeToFlag(BufferModeMark bufferType )
    {
        return static_cast<VboFlag>( std::max( 0, (bufferType - M_NORMAL) + CPU_ACCESSIBLE_DEFAULT ) );
    }
    //-----------------------------------------------------------------------------------
}
