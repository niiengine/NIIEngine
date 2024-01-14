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
#include "OgreD3D11HardwareBufferManager.h"
#include "OgreD3D11HardwareVertexBuffer.h"
#include "OgreD3D11HardwareIndexBuffer.h"
#include "OgreD3D11VertexDeclaration.h"
#include "OgreD3D11RenderToVertexBuffer.h"
#include "OgreD3D11HardwareUniformBuffer.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreD3D11Device.h"

namespace NII 
{
	//-----------------------------------------------------------------------
	D3D11HardwareBufferManager::D3D11HardwareBufferManager(D3D11Device & device)
		: mlpD3DDevice(device)
	{
	}
	//-----------------------------------------------------------------------
	D3D11HardwareBufferManager::~D3D11HardwareBufferManager()
	{
		destroyAllDeclarations();
		destroyAllBindings();
	}
	//-----------------------------------------------------------------------
	HardwareVertexBufferSharedPtr D3D11HardwareBufferManager::createVertexBuffer(size_t vertexSize, size_t numVerts, Buffer::Usage usage, bool useShadowBuffer)
	{
		N_assert(numVerts > 0);
		D3D11VertexBuffer* vbuf = new D3D11VertexBuffer(this, vertexSize, numVerts, usage, mlpD3DDevice, false, useShadowBuffer, false);
		{
			OGRE_LOCK_MUTEX(mVertexBuffersMutex);
			mVertexBuffers.insert(vbuf);
		}
		return HardwareVertexBufferSharedPtr(vbuf);
	}
	//-----------------------------------------------------------------------
	HardwareVertexBufferSharedPtr D3D11HardwareBufferManager::createStreamOutputVertexBuffer(size_t vertexSize, size_t numVerts, Buffer::Usage usage, bool useShadowBuffer)
	{
		N_assert(numVerts > 0);
		D3D11VertexBuffer* vbuf = new D3D11VertexBuffer(this, vertexSize, numVerts, usage, mlpD3DDevice, false, useShadowBuffer, true);
		{
			OGRE_LOCK_MUTEX(mVertexBuffersMutex);
			mVertexBuffers.insert(vbuf);
		}
		return HardwareVertexBufferSharedPtr(vbuf);
	}
	//-----------------------------------------------------------------------
	HardwareIndexBufferSharedPtr D3D11HardwareBufferManager::createIndexBuffer(IndexBuffer::IndexType itype, size_t numIndexes, Buffer::Usage usage, bool useShadowBuffer)
	{
		N_assert(numIndexes > 0);
		D3D11IndexBuffer* idx = new D3D11IndexBuffer(this, itype, numIndexes, usage, mlpD3DDevice, false, useShadowBuffer);
		{
			OGRE_LOCK_MUTEX(mIndexBuffersMutex);
			mIndexBuffers.insert(idx);
		}
		return HardwareIndexBufferSharedPtr(idx);

	}
	//-----------------------------------------------------------------------
	RenderToVertexBufferSharedPtr D3D11HardwareBufferManager::createRenderToVertexBuffer()
	{
		return RenderToVertexBufferSharedPtr(new D3D11RenderToVertexBuffer(mlpD3DDevice, this));
	}
	//-----------------------------------------------------------------------
	StructBuffer *
		D3D11HardwareBufferManager::createUniformBuffer(size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name)
	{
		N_assert(sizeBytes > 0);
		D3D11HardwareUniformBuffer* uni = 0;
		/*
		if (name != "")
		{
			SharedUniformBufferMap::iterator it = mSharedUniformBuffers.find(name);
			if (it != mSharedUniformBuffers.end())
			{
				uni = static_cast<D3D11HardwareUniformBuffer*>(it->second);
				N_assert (uni->getSizeInBytes() == sizeBytes);
				N_assert (uni->getUsage() == usage);
			}
			else
			{
				uni = new D3D11HardwareUniformBuffer(this, sizeBytes, usage, useShadowBuffer, name, mlpD3DDevice);
				{
					OGRE_LOCK_MUTEX(mUniformBuffersMutex)
					mUniformBuffers.insert(uni);
					//mSharedUniformBuffers.insert(std::make_pair(name, uni));
				}
			}
		}
		else
		{*/
			uni = new D3D11HardwareUniformBuffer(this, sizeBytes, usage, useShadowBuffer, name, mlpD3DDevice);
			{
				OGRE_LOCK_MUTEX(mUniformBuffersMutex);
				mUniformBuffers.insert(uni);
			}
		//}

		return StructBuffer *(uni);
	}
	//-----------------------------------------------------------------------
	CounterBuffer * D3D11HardwareBufferManager::createCounterBuffer(size_t sizeBytes,
		Buffer::Usage usage, bool useShadowBuffer, const String& name)
	{
		N_EXCEPT(RenderingAPI, "*** not implemented ***");
	}
	//-----------------------------------------------------------------------
	VertexDeclaration* D3D11HardwareBufferManager::createVertexDeclarationImpl(void)
	{
		return new D3D11VertexDeclaration(mlpD3DDevice);
	}
	//-----------------------------------------------------------------------
	void D3D11HardwareBufferManager::destroyVertexDeclarationImpl(VertexDeclaration* decl)
	{
		delete decl;
	}
    //----------------------------------------------------------------------------------------------------------------------------
    
    static const char *c_vboTypes[3][3] =
    {
        {
            "VERTEX_IMMUTABLE",
            "VERTEX_DEFAULT",
            "VERTEX_DYNAMIC",
        },
        {
            "INDEX_IMMUTABLE",
            "INDEX_DEFAULT",
            "INDEX_DYNAMIC",
        },
        {
            "SHADER_IMMUTABLE",
            "SHADER_DEFAULT",
            "SHADER_DYNAMIC",
        }
    };

    D3D11VaoManager::D3D11VaoManager( bool _supportsIndirectBuffers, D3D11Device &device,
                                      D3D11RenderSystem *renderSystem, const PropertyData *params ) :
        GBufferManager( params ),
        mVaoNames( 1 ),
        mDevice( device ),
        mDrawId( 0 ),
        mD3D11RenderSystem( renderSystem )
    {
        mDefaultPoolSize[VERTEX_BUFFER][M_DEVREAD]   = 64 * 1024 * 1024;
        mDefaultPoolSize[INDEX_BUFFER][M_DEVREAD]    = 64 * 1024 * 1024;
        mDefaultPoolSize[SHADER_BUFFER][M_DEVREAD]   = 64 * 1024 * 1024;

        mDefaultPoolSize[VERTEX_BUFFER][M_DEVNORMAL]     = 32 * 1024 * 1024;
        mDefaultPoolSize[INDEX_BUFFER][M_DEVNORMAL]      = 16 * 1024 * 1024;
        mDefaultPoolSize[SHADER_BUFFER][M_DEVNORMAL]     = 16 * 1024 * 1024;

        mDefaultPoolSize[VERTEX_BUFFER][M_NORMAL] = 16 * 1024 * 1024;
        mDefaultPoolSize[INDEX_BUFFER][M_NORMAL]  = 16 * 1024 * 1024;
        mDefaultPoolSize[SHADER_BUFFER][M_NORMAL] = 16 * 1024 * 1024;

        if( params )
        {
            for( size_t i=0; i<NumInternalBufferTypes; ++i )
            {
                for( size_t j=M_DEVREAD; j<=M_NORMAL; ++j )
                {
                    PropertyData::const_iterator itor = params->find( String( "GBufferManager::" ) + c_vboTypes[i][j] );
                    if( itor != params->end() )
                    {
                        mDefaultPoolSize[i][j] = StringConverter::parseUnsignedLong( itor->second, mDefaultPoolSize[i][j] );
                    }
                }
            }
        }

        mFrameSyncVec.resize( mDynamicBufferMultiplier );

        //There's no way to query the API, grrr... but this
        //is the minimum supported by all known GPUs
        mAlignment[GBT_Struct]   = 256;
        mAlignment[GBT_Texture]     = 256;

        //64kb by spec. DX 11.1 on Windows 8.1 allows to query to see if there's more.
        //But it's a PITA to use a special path just for that.
        mMaxSize[GBT_Struct] = 4096 * 1024;
        //Umm... who knows? Just use GL3's minimum.
        mMaxSize[GBT_Texture] = 128 * 1024 * 1024;

        //4096u is a sensible default because most Hlms implementations need 16 bytes per
        //instance in a const buffer. HlmsBufferManager::mapNextConstBuffer purposedly clamps
        //its const buffers to 64kb, so that 64kb / 16 = 4096 and thus it can never exceed
        //4096 instances.
        //However due to instanced stereo, we need twice that
        const uint32 maxNumInstances = 4096u * 2u;
        VertexUnitList units;
        units.push_back( VertexUnit( DT_UInt1x, VES_COUNT ) );
        uint32 *drawIdPtr = static_cast<uint32*>( OGRE_MALLOC_SIMD( maxNumInstances * sizeof(uint32), MEMCATEGORY_GEOMETRY ) );
        
        for( uint32 i=0; i<maxNumInstances; ++i )
            drawIdPtr[i] = i;
        mDrawId = createVertexBuffer( units, maxNumInstances, M_DEVREAD, drawIdPtr, true );
        createDelayedImmutableBuffers(); //Ensure mDrawId gets allocated before we continue
    }
    //-----------------------------------------------------------------------------------
    D3D11VaoManager::~D3D11VaoManager()
    {
        {
            //Destroy all buffers which don't have a pool
            BufferPackedSet::const_iterator itor = mBuffers[BP_TYPE_CONST].begin();
            BufferPackedSet::const_iterator end  = mBuffers[BP_TYPE_CONST].end();
            while( itor != end )
                destroyConstBufferImpl( static_cast<StructBuffer*>( *itor++ ) );
        }

        mSplicingHelperBuffer.Reset();

        for( size_t i=0; i<NumInternalBufferTypes; ++i )
        {
            for( size_t j=0; j<M_NORMAL+1; ++j )
            {
                //Free pointers and collect the buffer names from all VBOs to use one API call
                VboVec::iterator itor = mVbos[i][j].begin();
                VboVec::iterator end  = mVbos[i][j].end();

                while( itor != end )
                {
                    itor->vboName.Reset();
                    delete itor->mBuffer;
                    itor->mBuffer = 0;
                    ++itor;
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::getMemoryStats( const Block &block, uint32 vboIdx0, uint32 vboIdx1,
        size_t poolCapacity, LwString &text, MemoryStatsList &outStats, Log * log ) const
    {
        if( log )
        {
            text.clear();
            text.a( c_vboTypes[vboIdx0][vboIdx1], ";",
                    (uint64)block.offset, ";",
                    (uint64)block.size, ";",
                    (uint64)poolCapacity );
            log->logMessage( text.c_str(), LML_CRITICAL );
        }

        const uint32 vboIdx = (vboIdx0 << 16u) | (vboIdx1 & 0xFFFF);
        MemoryStatsEntry entry( vboIdx, block.offset, block.size, poolCapacity );
        outStats.push_back( entry );
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::getMemoryStats( MemoryStatsList &outStats, size_t &outCapacityBytes,
                                          size_t & outFreeBytes, Log * log ) const
    {
        size_t capacityBytes = 0;
        size_t freeBytes = 0;
        MemoryStatsList statsVec;
        statsVec.swap( outStats );

        vector<char>::type tmpBuffer;
        tmpBuffer.resize( 512 * 1024 ); //512kb per line should be way more than enough
        LwString text( LwString::FromEmptyPointer( &tmpBuffer[0], tmpBuffer.size() ) );

        if( log )
            log->logMessage( "Pool Type;Offset;Bytes;Pool Capacity", LML_CRITICAL );

        for( uint32 idx0=0; idx0<NumInternalBufferTypes; ++idx0 )
        {
            for( uint32 idx1=0; idx1<M_NORMAL+1; ++idx1 )
            {
                VboVec::const_iterator itor = mVbos[idx0][idx1].begin();
                VboVec::const_iterator end  = mVbos[idx0][idx1].end();

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
                            getMemoryStats( usedBlock, idx0, idx1, vbo.sizeBytes, text, statsVec, log );

                        usedBlock.offset += usedBlock.size;
                        usedBlock.size = 0;
                        remove( freeBlocks, nextBlock );
                    }

                    if( usedBlock.size > 0u || (usedBlock.offset == 0 && usedBlock.size == 0) )
                        getMemoryStats( usedBlock, idx0, idx1, vbo.sizeBytes, text, statsVec, log );

                    ++itor;
                }
            }
        }

        outCapacityBytes = capacityBytes;
        outFreeBytes = freeBytes;
        statsVec.swap( outStats );
    }
    //-----------------------------------------------------------------------------------
    /*void D3D11VaoManager::switchVboPoolIndexImpl( size_t oldPoolIdx, size_t newPoolIdx, GpuBuffer *buffer )
    {
        const BufferPackedTypes bufferPackedType = buffer->getBufferPackedType();
        if( (mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) || bufferPackedType != BP_TYPE_INDIRECT) &&
            (mD3D11RenderSystem->_getFeatureLevel() > D3D_FEATURE_LEVEL_11_0 ||
             bufferPackedType != BP_TYPE_TEX) &&
            bufferPackedType != BP_TYPE_CONST &&
            bufferPackedType != BP_TYPE_UAV )
        {
            N_assert( dynamic_cast<D3D11BufferOp*>( buffer->getOperation() ) );
            D3D11BufferOp *bufferInterface = static_cast<D3D11BufferOp*>( buffer->getOperation() );
            if( bufferInterface->_getInitialData() == 0 )
            {
                if( bufferInterface->getVboPoolIndex() == oldPoolIdx )
                    bufferInterface->_setVboPoolIndex( newPoolIdx );
            }
        }
    }*/
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::recoverPool(void)
    {
        for( uint32 idx0=0; idx0<NumInternalBufferTypes; ++idx0 )
        {
            for( uint32 idx1=0; idx1<M_NORMAL+1; ++idx1 )
            {
                VboVec::iterator itor = mVbos[idx0][idx1].begin();
                VboVec::iterator end  = mVbos[idx0][idx1].end();

                while( itor != end )
                {
                    Vbo &vbo = *itor;
                    if( vbo.mFreeList.size() == 1u &&
                        vbo.sizeBytes == vbo.mFreeList.back().size )
                    {
                        vbo.vboName.Reset();
                        delete vbo.mBuffer;
                        vbo.mBuffer = 0;

                        //There's (unrelated) live buffers whose vboIdx will now point out of bounds.
                        //We need to update them so they don't crash deallocateVbo later.
                        switchVboPoolIndex( (size_t)(mVbos[idx0][idx1].size() - 1u),
                                            (size_t)(itor - mVbos[idx0][idx1].begin()) );

                        itor = remove( mVbos[idx0][idx1], itor );
                        end  = mVbos[idx0][idx1].end();
                    }
                    else
                    {
                        ++itor;
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::allocateVbo( size_t sizeBytes, size_t alignment, Buffer::ModeMark bufferType,
                                       InternalBufferType internalType, size_t &outVboIdx, size_t &outBufferOffset )
    {
        N_assert( alignment > 0 );

        //Immutable buffers are delayed as much as possible so we can merge all of them.
        //See createDelayedImmutableBuffers
        N_assert( bufferType != M_DEVREAD );

        if(N_MarkTrue(bufferType, M_NORMAL))
        {
            bufferType  = M_NORMAL; //Persitent mapping not supported in D3D11.
            sizeBytes   *= mDynamicBufferMultiplier;
        }

        VboVec::const_iterator itor = mVbos[internalType][bufferType].begin();
        VboVec::const_iterator end  = mVbos[internalType][bufferType].end();

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
                    bestVboIdx = itor - mVbos[internalType][bufferType].begin();
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
            bestVboIdx      = mVbos[internalType][bufferType].size();
            bestBlockIdx    = 0;
            foundMatchingStride = true;

            Vbo newVbo;

            size_t poolSize = std::max( mDefaultPoolSize[internalType][bufferType], sizeBytes );

            ID3D11DeviceN *d3dDevice = mDevice.get();

            D3D11_BUFFER_DESC desc;
            ZeroMemory( &desc, sizeof(D3D11_BUFFER_DESC) );
            desc.ByteWidth  = poolSize;
            desc.CPUAccessFlags = 0;
            if( bufferType == M_DEVREAD )
                desc.Usage = D3D11_USAGE_IMMUTABLE;
            else if( bufferType == M_DEVNORMAL )
                desc.Usage = D3D11_USAGE_DEFAULT;
            else if( N_MarkTrue(bufferType, M_NORMAL))
            {
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            }

            if( internalType == VERTEX_BUFFER )
                desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            else if( internalType == INDEX_BUFFER )
                desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            else
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            HRESULT hr = d3dDevice->CreateBuffer( &desc, 0, newVbo.vboName.GetAddressOf() );

            if( FAILED( hr ) )
            {
                String errorDescription = mDevice.getErrorString(hr);
                N_EXCEPT( RenderingAPI,
                             "Failed to create buffer. ID3D11Device::CreateBuffer.\n"
                             "Error code: " + StrConv::conv( hr ) + ".\n" +
                             errorDescription +
                             "Requested: " + StrConv::conv( poolSize ) + " bytes.");
            }

            newVbo.sizeBytes = poolSize;
            newVbo.mFreeList.push_back( Block( 0, poolSize ) );
            newVbo.mBuffer = 0;

            if(N_MarkTrue(bufferType, M_NORMAL))
            {
                newVbo.mBuffer = new D3D11Buffer( newVbo.vboName.Get(), newVbo.sizeBytes, mDevice );
            }

            mVbos[internalType][bufferType].push_back( newVbo );
        }

        Vbo &bestVbo        = mVbos[internalType][bufferType][bestVboIdx];
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
    void D3D11VaoManager::deallocateVbo( size_t vboIdx, size_t bufferOffset, size_t sizeBytes,
                                         Buffer::ModeMark bufferType, InternalBufferType internalType )
    {
        if (N_MarkTrue(bufferType, M_NORMAL))
        {
            bufferType = M_NORMAL; //Persitent mapping not supported in D3D11.
            sizeBytes *= mDynamicBufferMultiplier;
        }

        Vbo &vbo = mVbos[internalType][bufferType][vboIdx];
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

        if( vbo.mFreeList.back().size == vbo.sizeBytes && bufferType == M_DEVREAD )
        {
            //Immutable buffer is empty. It can't be filled again. Release the GPU memory.
            //The vbo is not removed from mVbos since that would alter the index of other
            //buffers (except if this is the last one).
            //We can call switchVboPoolIndex, but that has unknown run time
            vbo.vboName.Reset();

            while( !mVbos[internalType][bufferType].empty() &&
                   mVbos[internalType][bufferType].back().vboName.Get() == 0 )
            {
                mVbos[internalType][bufferType].pop_back();
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::createImmutableBuffer( InternalBufferType internalType, size_t sizeBytes, void *initialData, Vbo &inOutVbo )
    {
        const size_t poolSize = sizeBytes;

        ID3D11DeviceN *d3dDevice = mDevice.get();

        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof(D3D11_BUFFER_DESC) );
        desc.ByteWidth  = poolSize;
        desc.CPUAccessFlags = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;

        if( internalType == VERTEX_BUFFER )
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        else if( internalType == INDEX_BUFFER )
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        else
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA subResData;
        ZeroMemory( &subResData, sizeof(D3D11_SUBRESOURCE_DATA) );
        subResData.pSysMem = initialData;

        HRESULT hr = d3dDevice->CreateBuffer( &desc, &subResData, inOutVbo.vboName.GetAddressOf() );

        if( FAILED( hr ) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT( RenderingAPI,
                         "Failed to create buffer. ID3D11Device::CreateBuffer.\n"
                         "Error code: " + StrConv::conv( hr ) + ".\n" +
                         errorDescription +
                         "Requested: " + StrConv::conv( poolSize ) + " bytes." );
        }

        inOutVbo.sizeBytes = poolSize;
        inOutVbo.mFreeList.clear(); //No free blocks
        inOutVbo.mBuffer = 0;

        mVbos[internalType][M_DEVREAD].push_back( inOutVbo );
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::mergeContiguousBlocks(BlockVec::iterator blockToMerge, BlockVec & blocks)
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
    VertexBufferPacked* D3D11VaoManager::createVertexBufferImpl( size_t numElements,
                                                                 uint32 bytesPerElement,
                                                                 Buffer::ModeMark bufferType,
                                                                 void *initialData, bool keepAsShadow,
                                                                 const VertexUnitList &vElements )
    {
        size_t vboIdx;
        size_t bufferOffset;
        ID3D11Buffer *vboName;
        D3D11Buffer *dynamicBuffer;

        if( bufferType == M_DEVREAD )
        {
            vboIdx          = 0;
            bufferOffset    = 0;
            vboName         = 0;
            dynamicBuffer   = 0;
        }
        else
        {
            allocateVbo( numElements * bytesPerElement, bytesPerElement, bufferType, VERTEX_BUFFER, vboIdx, bufferOffset );

            Buffer::ModeMark vboFlag = bufferType;
            if(N_MarkTrue(vboFlag, M_NORMAL))
                vboFlag = M_NORMAL;
            Vbo &vbo = mVbos[VERTEX_BUFFER][vboFlag][vboIdx];
            vboName         = vbo.vboName.Get();
            dynamicBuffer   = vbo.mBuffer;
        }

        D3D11BufferOp *bufferInterface = new D3D11BufferOp( vboIdx, vboName, dynamicBuffer );
        VertexBufferPacked *retVal = N_new VertexBufferPacked(
                                                        bufferOffset, numElements, bytesPerElement, 0,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, bufferInterface, vElements, 0, 0, 0 );

        if( initialData )
            bufferInterface->_init( initialData );

        if( bufferType == M_DEVREAD )
            mDelayedBuffers[VERTEX_BUFFER].push_back( retVal );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::destroyVertexBufferImpl( VertexBufferPacked * vertexBuffer )
    {
        D3D11BufferOp * bufferInterface = static_cast<D3D11BufferOp*>(vertexBuffer->getOperation());

        if( bufferInterface->_getInitialData() != 0 )
        {
            //Immutable buffer that never made to the GPU.
            removeBufferFromDelayedQueue( mDelayedBuffers[VERTEX_BUFFER], vertexBuffer );
        }
        else
        {
            deallocateVbo( bufferInterface->getVboPoolIndex(),
                           vertexBuffer->getOffset() * vertexBuffer->getUnitSize(),
                           vertexBuffer->getAllocSize(),
                           vertexBuffer->mMark, VERTEX_BUFFER );
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::removeBufferFromDelayedQueue(BufferPackedVec & container, GpuBuffer * buffer)
    {
        N_assert( buffer->mMark == M_DEVREAD );

        BufferPackedVec::iterator itor = container.begin();
        BufferPackedVec::iterator end  = container.end();
        while( itor != end )
        {
            if( *itor == buffer )
            {
                itor = remove( container, itor );
                itor = container.end();
                end  = container.end();
            }
            else
            {
                ++itor;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::_forceCreateDelayedImmutableBuffers(void)
    {
        bool delayedBuffersPending = false;
        for( size_t i=0; i<NumInternalBufferTypes; ++i )
        {
            delayedBuffersPending |= !mDelayedBuffers[i].empty() && mDefaultPoolSize[i][M_DEVREAD] > 0u;
        }

        if( delayedBuffersPending )
        {
            LogManager::getSingleton().logMessage(
                        "PERFORMANCE WARNING D3D11: Calling createAsyncTicket when there are "
                        "pending immutable buffers to be created. This will force Ogre to "
                        "create them immediately; which diminish our ability to batch meshes "
                        "toghether & could affect performance during rendering.\n"
                        "You should call createAsyncTicket after all immutable meshes have "
                        "been loaded to ensure they get batched together. If you're already "
                        "doing this, you can ignore this warning.\n"
                        "If you're not going to render (i.e. this is a mesh export tool) "
                        "you can also ignore this warning." );
        }

        createDelayedImmutableBuffers();
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::createDelayedImmutableBuffers(void)
    {
        OgreProfileExhaustive( "D3D11VaoManager::createDelayedImmutableBuffers" );

        bool immutableBuffersCreated = false;

        for( size_t i = 0; i < NumInternalBufferTypes; ++i )
        {
            BufferPackedVec::const_iterator start = mDelayedBuffers[i].begin();
            while( start != mDelayedBuffers[i].end() )
            {
                //Each iteration means a new pool
                Vbo newVbo;

                //Calculate the size for this pool
                BufferPackedVec::const_iterator itor = start;
                BufferPackedVec::const_iterator end  = mDelayedBuffers[i].end();

                size_t totalBytes = (*itor)->getSize();
                ++itor;

                while( itor != end )
                {
                    if( totalBytes + (*itor)->getSize() > mDefaultPoolSize[i][M_DEVREAD] )
                    {
                        end = itor;
                    }
                    else
                    {
                        totalBytes = multiCeil( totalBytes, (*itor)->getUnitSize() );
                        totalBytes += (*itor)->getSize();

                        ++itor;
                    }
                }

                uint8 * mergedData = reinterpret_cast<uint8 *>(OGRE_MALLOC_SIMD( totalBytes, MEMCATEGORY_GEOMETRY ));
                size_t dstOffset = 0;

                //Merge the binary data as a contiguous array
                itor = start;
                while( itor != end )
                {
                    D3D11BufferOp * bufferInterface = static_cast<D3D11BufferOp *>((*itor)->getOperation() );

                    const size_t dstOffsetBeforeAlignment = dstOffset;
                    dstOffset = multiCeil(dstOffset, (*itor)->getUnitSize());

                    if( dstOffsetBeforeAlignment != dstOffset )
                    {
                        //This is a stride changer, record as such.
                        StrideChangerVec::iterator itStride = std::lower_bound(newVbo.strideChangers.begin(), newVbo.strideChangers.end(), dstOffset, StrideChanger() );
                        const size_t padding = dstOffset - dstOffsetBeforeAlignment;
                        newVbo.strideChangers.insert( itStride, StrideChanger( dstOffset, padding ) );
                    }

                    memcpy( mergedData + dstOffset, bufferInterface->_getInitialData(), (*itor)->getSize() );

                    bufferInterface->_deleteInitialData();

                    dstOffset += (*itor)->getSize();

                    ++itor;
                }

                //Create the actual D3D11 object loaded with the merged data
                createImmutableBuffer( static_cast<InternalBufferType>(i), totalBytes, mergedData, newVbo );

                const size_t vboIdx = mVbos[i][M_DEVREAD].size() - 1;
                ID3D11Buffer *vboName = newVbo.vboName.Get();
                dstOffset = 0;

                //Each buffer needs to be told about its new D3D11 object
                //(and pool index & where it starts).
                itor = start;
                while( itor != end )
                {
                    D3D11BufferOp * bufferInterface = static_cast<D3D11BufferOp*>((*itor)->getOperation() );
                    dstOffset = multiCeil( dstOffset, (*itor)->getUnitSize() );

                    bufferInterface->_setVboName( vboIdx, vboName, dstOffset );

                    dstOffset += (*itor)->getSize();
                    ++itor;
                }

                OGRE_FREE_SIMD( mergedData, MEMCATEGORY_GEOMETRY );
                mergedData = 0;

                start = end;
                immutableBuffersCreated = true;
            }
        }

        //We've populated the Vertex & Index buffers with their GPU/API pointers. Now we need
        //to update the Vaos' internal structures that cache these GPU/API pointers.
        if( immutableBuffersCreated )
            reorganizeImmutableVaos();

        for( size_t i=0; i<NumInternalBufferTypes; ++i )
            mDelayedBuffers[i].clear();
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::reorganizeImmutableVaos(void)
    {
        OgreProfileExhaustive( "D3D11VaoManager::reorganizeImmutableVaos" );

        VAOList::const_iterator itor = VAOList.begin();
        VAOList::const_iterator end  = VAOList.end();

        while( itor != end )
        {
            GeometryRaw *vertexArrayObject = *itor;

            bool needsUpdate = false;

            const VertexBufferList &vertexBuffers = vertexArrayObject->getVertexBuffers();
            VertexBufferList::const_iterator itVertexBuf = vertexBuffers.begin();
            VertexBufferList::const_iterator enVertexBuf = vertexBuffers.end();

            while( itVertexBuf != enVertexBuf && !needsUpdate )
            {
                if( (*itVertexBuf)->mMark == M_DEVREAD )
                {
                    BufferPackedVec::const_iterator it = std::find(mDelayedBuffers[VERTEX_BUFFER].begin(),
                        mDelayedBuffers[VERTEX_BUFFER].end(), *itVertexBuf );

                    if( it != mDelayedBuffers[VERTEX_BUFFER].end() )
                        needsUpdate = true;
                }

                ++itVertexBuf;
            }

            if( vertexArrayObject->getIndexBuffer() && vertexArrayObject->getIndexBuffer()->mMark == M_DEVREAD )
            {
                BufferPackedVec::const_iterator it = std::find( mDelayedBuffers[INDEX_BUFFER].begin(),
                                                                mDelayedBuffers[INDEX_BUFFER].end(),
                                                                vertexArrayObject->getIndexBuffer() );
                if( it != mDelayedBuffers[INDEX_BUFFER].end() )
                    needsUpdate = true;
            }

            if( needsUpdate )
            {
                if( vertexArrayObject->mVaoId )
                    releaseVao( vertexArrayObject );

                VaoVec::iterator itor = findVao( vertexArrayObject->getVertexBuffers(), vertexArrayObject->getIndexBuffer(), vertexArrayObject->getType() );

                D3D11VertexArrayObject *d3dVao = static_cast<D3D11VertexArrayObject*>(vertexArrayObject );

                uint32 uniqueVaoId = extractUniqueVaoIdFromRenderQueueId( d3dVao->getRenderID() );

                const uint32 renderQueueId = generateRenderQueueId( itor->vaoName, uniqueVaoId );

                d3dVao->_updateImmutableResource( itor->vaoName, renderQueueId, itor->sharedData );
            }

            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    MultiSourceVertexBufferPool* D3D11VaoManager::createMultiSourceVertexBufferPoolImpl(
                                                const VertexUnitListList & vertexElementsBySource,
                                                size_t maxNumVertices, size_t totalBytesPerVertex,
                                                Buffer::ModeMark bufferType )
    {
        N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Not implemented. Come back later." );
        /*size_t vboIdx;
        size_t bufferOffset;

        allocateVbo( maxNumVertices * totalBytesPerVertex, totalBytesPerVertex,
                     bufferType, VERTEX_BUFFER, vboIdx, bufferOffset );

        Buffer::ModeMark vboFlag = bufferType;
        if( N_MarkTrue(vboFlag, M_NORMAL))
            vboFlag = M_NORMAL;
        const Vbo &vbo = mVbos[VERTEX_BUFFER][vboFlag][vboIdx];

        return N_new D3D11MultiSourceVertexBufferPool( vboIdx, vbo.vboName, vertexElementsBySource,
                                                          maxNumVertices, bufferType,
                                                          bufferOffset, this );*/
    }
    //-----------------------------------------------------------------------------------
    IndexBufferPacked* D3D11VaoManager::createIndexBufferImpl( size_t numElements,
                                                                 uint32 bytesPerElement,
                                                                 Buffer::ModeMark bufferType,
                                                                 void *initialData, bool keepAsShadow )
    {
        size_t vboIdx;
        size_t bufferOffset;
        ID3D11Buffer *vboName;
        D3D11Buffer *dynamicBuffer;

        if( bufferType == M_DEVREAD )
        {
            vboIdx          = 0;
            bufferOffset    = 0;
            vboName         = 0;
            dynamicBuffer   = 0;
        }
        else
        {
            allocateVbo( numElements * bytesPerElement, bytesPerElement, bufferType, INDEX_BUFFER,
                         vboIdx, bufferOffset );

            Buffer::ModeMark vboFlag = bufferType;
            if(N_MarkTrue(vboFlag, M_NORMAL))
                vboFlag = M_NORMAL;
            Vbo &vbo = mVbos[INDEX_BUFFER][vboFlag][vboIdx];
            vboName         = vbo.vboName.Get();
            dynamicBuffer   = vbo.mBuffer;
        }

        D3D11BufferOp *bufferInterface = new D3D11BufferOp( vboIdx, vboName, dynamicBuffer );
        
        IndexBufferPacked *retVal = N_new IndexBufferPacked(bufferOffset, numElements, bytesPerElement, 0,
            bufferType, initialData, keepAsShadow, this, bufferInterface );

        if( initialData )
            bufferInterface->_init( initialData );

        if( bufferType == M_DEVREAD )
            mDelayedBuffers[INDEX_BUFFER].push_back( retVal );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::destroyIndexBufferImpl( IndexBufferPacked *indexBuffer )
    {
        D3D11BufferOp *bufferInterface = static_cast<D3D11BufferOp*>(indexBuffer->getOperation() );

        if( bufferInterface->_getInitialData() != 0 )
        {
            //Immutable buffer that never made to the GPU.
            removeBufferFromDelayedQueue( mDelayedBuffers[INDEX_BUFFER], indexBuffer );
        }
        else
        {
            deallocateVbo( bufferInterface->getVboPoolIndex(),
                           indexBuffer->getOffset() * indexBuffer->getUnitSize(),
                           indexBuffer->getAllocSize(),
                           indexBuffer->mMark, INDEX_BUFFER );
        }
    }
    //-----------------------------------------------------------------------------------
    D3D11CompatBufferInterface* D3D11VaoManager::createShaderBufferInterface(
            uint32 bindFlags, size_t sizeBytes, Buffer::ModeMark bufferType,
            void *initialData, uint32 structureByteStride )
    {
        ID3D11DeviceN *d3dDevice = mDevice.get();

        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof(D3D11_BUFFER_DESC) );
        if( bindFlags & BB_FLAG_CONST )
            desc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
        if( bindFlags & BB_FLAG_TEX )
            desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if( bindFlags & BB_FLAG_UAV )
        {
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
            desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED /*|
                    D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS*/;
        }
        desc.ByteWidth      = sizeBytes;
        desc.CPUAccessFlags = 0;
        if( bufferType == M_DEVREAD )
            desc.Usage = D3D11_USAGE_IMMUTABLE;
        else if( bufferType == M_DEVNORMAL )
            desc.Usage = D3D11_USAGE_DEFAULT;
        else if( N_MarkTrue(bufferType, M_NORMAL))
        {
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        }
        desc.StructureByteStride = structureByteStride;

        D3D11_SUBRESOURCE_DATA subResData;
        ZeroMemory( &subResData, sizeof(D3D11_SUBRESOURCE_DATA) );
        subResData.pSysMem = initialData;
        ComPtr<ID3D11Buffer> vboName;

        HRESULT hr = d3dDevice->CreateBuffer( &desc, initialData ? &subResData : 0, vboName.GetAddressOf() );

        if( FAILED( hr ) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT( RenderingAPI,
                         "Failed to create buffer. ID3D11Device::CreateBuffer.\n"
                         "Error code: " + StrConv::conv( hr ) + ".\n" +
                         errorDescription +
                         "Requested: " + StrConv::conv( sizeBytes ) + " bytes." );
        }

        return new D3D11CompatBufferInterface( 0, vboName.Get(), mDevice );
    }
    //-----------------------------------------------------------------------------------
    StructBuffer* D3D11VaoManager::createConstBufferImpl( size_t sizeBytes, Buffer::ModeMark bufferType,
                                                               void *initialData, bool keepAsShadow )
    {
        //Const buffers don't get batched together since D3D11 doesn't allow binding just a
        //region and has a 64kb limit. Only D3D11.1 on Windows 8.1 supports this feature.
        D3D11CompatBufferInterface *bufferInterface = createShaderBufferInterface( BB_FLAG_CONST,
                                                                                   sizeBytes,
                                                                                   bufferType,
                                                                                   initialData );
        StructBuffer *retVal = N_new D3D11ConstBufferPacked(
                                                        sizeBytes, 1, 0,
                                                        bufferType,
                                                        initialData, keepAsShadow,
                                                        this, bufferInterface,
                                                        mDevice );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::destroyConstBufferImpl( StructBuffer *constBuffer )
    {
        D3D11CompatBufferInterface *bufferInterface = static_cast<D3D11CompatBufferInterface*>(
                                                                constBuffer->getOperation() );

        //bufferInterface->getVboNamePtr().Reset();
    }
    //-----------------------------------------------------------------------------------
    TextureBuffer* D3D11VaoManager::createTexBufferImpl( PixelFormat pf, size_t sizeBytes,
        Buffer::ModeMark bufferType, void *initialData, bool keepAsShadow )
    {
        BufferOp *bufferInterface = 0;
        size_t bufferOffset = 0;

        uint32 alignment = mAlignment[GBT_Texture];
        size_t requestedSize = sizeBytes;

        const size_t bytesPerElement = 1;

        if(N_MarkTrue(bufferType, M_NORMAL) )
        {
            //For dynamic buffers, the size will be 3x times larger
            //(depending on mDynamicBufferMultiplier); we need the
            //offset after each map to be aligned; and for that, we
            //sizeBytes to be multiple of alignment.
            sizeBytes = multiCeil( sizeBytes, Math::lcm( alignment, bytesPerElement ) );
        }

        if( mD3D11RenderSystem->_getFeatureLevel() > D3D_FEATURE_LEVEL_11_0 )
        {
            //D3D11.1 supports NO_OVERWRITE on shader buffers, use the common pool
            size_t vboIdx = 0;

            Buffer::ModeMark vboFlag = bufferType;
            if(N_MarkTrue(vboFlag, M_NORMAL))
                vboFlag = M_NORMAL;

            if( bufferType == M_DEVREAD )
            {
				bufferInterface = new D3D11BufferOp( vboIdx, 0, 0 );
            }
            else
            {
				allocateVbo( sizeBytes, alignment, bufferType, SHADER_BUFFER, vboIdx, bufferOffset );

				Vbo &vbo = mVbos[SHADER_BUFFER][vboFlag][vboIdx];
				bufferInterface = new D3D11BufferOp( vboIdx, vbo.vboName.Get(), vbo.mBuffer );
            }
        }
        else
        {
            //D3D11.0 and below doesn't support NO_OVERWRITE on shader buffers. Use the basic interface.
            bufferInterface = createShaderBufferInterface( BB_FLAG_TEX, sizeBytes, bufferType, initialData );
        }

        const size_t numElements = requestedSize;

        D3D11TextureBuffer * retVal = N_new D3D11TextureBuffer(
                                                        bufferOffset, numElements, bytesPerElement,
                                                        (sizeBytes - requestedSize) / bytesPerElement,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, bufferInterface,
                                                        pf, false, mDevice );

        if( mD3D11RenderSystem->_getFeatureLevel() > D3D_FEATURE_LEVEL_11_0 )
        {
            if( initialData )
                static_cast<D3D11BufferOp*>( bufferInterface )->_init( initialData );

            if( bufferType == M_DEVREAD )
                mDelayedBuffers[SHADER_BUFFER].push_back( retVal );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::destroyTexBufferImpl( TextureBuffer *texBuffer )
    {
        if( mD3D11RenderSystem->_getFeatureLevel() > D3D_FEATURE_LEVEL_11_0 )
        {
            D3D11BufferOp * bufferInterface = static_cast<D3D11BufferOp*>(texBuffer->getOperation());

            if( bufferInterface->_getInitialData() != 0 )
            {
                //Immutable buffer that never made to the GPU.
                removeBufferFromDelayedQueue( mDelayedBuffers[SHADER_BUFFER], texBuffer );
            }
            else
            {
                deallocateVbo( bufferInterface->getVboPoolIndex(),
                               texBuffer->getOffset() * texBuffer->getUnitSize(),
                               texBuffer->getAllocSize(),
                               texBuffer->mMark, SHADER_BUFFER );
            }
        }
        else
        {
            D3D11CompatBufferInterface *bufferInterface = static_cast<D3D11CompatBufferInterface*>(texBuffer->getOperation() );

            //bufferInterface->getVboNamePtr().Reset();
        }
    }
    //-----------------------------------------------------------------------------------
    StorageBuffer* D3D11VaoManager::createUavBufferImpl( size_t numElements, uint32 bytesPerElement,
                                                           uint32 bindFlags, void *initialData,
                                                           bool keepAsShadow )
    {
        size_t bufferOffset = 0;

        const Buffer::ModeMark bufferType = M_DEVNORMAL;

        BufferOp *bufferInterface = createShaderBufferInterface( bindFlags|BB_FLAG_UAV,
                                                                        numElements * bytesPerElement,
                                                                        bufferType, initialData,
                                                                        bytesPerElement );

        D3D11StorageBuffer *retVal = N_new D3D11StorageBuffer(
                                                        bufferOffset, numElements, bytesPerElement,
                                                        bindFlags, initialData, keepAsShadow,
                                                        this, bufferInterface, mDevice );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::destroyUavBufferImpl(StorageBuffer * uavBuffer)
    {
        D3D11CompatBufferInterface *bufferInterface = static_cast<D3D11CompatBufferInterface*>(uavBuffer->getOperation() );

        //bufferInterface->getVboNamePtr().Reset();
    }
    //-----------------------------------------------------------------------------------
    IndirectBuffer* D3D11VaoManager::createIndirectBufferImpl( size_t sizeBytes,
                                                                       Buffer::ModeMark bufferType,
                                                                       void *initialData,
                                                                       bool keepAsShadow )
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

        D3D11BufferOp *bufferInterface = 0;
        if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
        {
            N_assert( bufferType != M_DEVREAD && "Immutable indirect buffers not implemented yet" );

            size_t vboIdx;
            Buffer::ModeMark vboFlag = bufferType;
            if( N_MarkTrue(vboFlag, M_NORMAL) )
                vboFlag = M_NORMAL;

            allocateVbo( sizeBytes, alignment, bufferType, SHADER_BUFFER, vboIdx, bufferOffset );

            Vbo &vbo = mVbos[SHADER_BUFFER][vboFlag][vboIdx];
            bufferInterface = new D3D11BufferOp( vboIdx, vbo.vboName.Get(), vbo.mBuffer );
        }

        IndirectBuffer *retVal = N_new IndirectBuffer(
                                                        bufferOffset, requestedSize, 1,
                                                        (sizeBytes - requestedSize) / 1,
                                                        bufferType, initialData, keepAsShadow,
                                                        this, bufferInterface );

        if( initialData )
        {
            if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
            {
                bufferInterface->_init( initialData );
            }
            else
            {
                memcpy( retVal->getHostData(), initialData, requestedSize );
            }
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::destroyIndirectBufferImpl( IndirectBuffer *indirectBuffer )
    {
        if( mRenderSys->getFeature()->isSupport(CF_IndirectBuffer) )
        {
            D3D11BufferOp *bufferInterface = static_cast<D3D11BufferOp*>(indirectBuffer->getOperation() );

            if( bufferInterface->_getInitialData() != 0 )
            {
                //Immutable buffer that never made to the GPU.
                removeBufferFromDelayedQueue( mDelayedBuffers[SHADER_BUFFER], indirectBuffer );
            }
            else
            {
                deallocateVbo( bufferInterface->getVboPoolIndex(),
                               indirectBuffer->getOffset() * indirectBuffer->getUnitSize(),
                               indirectBuffer->getAllocSize(),
                               indirectBuffer->mMark, SHADER_BUFFER );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    D3D11VaoManager::VaoVec::iterator D3D11VaoManager::findVao( const VertexBufferList &vertexBuffers, IndexBufferPacked *indexBuffer, OperationType opType )
    {
        OgreProfileExhaustive( "D3D11VaoManager::findVao" );

        Vao vao;

        vao.mType = opType;
        vao.vertexBuffers.reserve( vertexBuffers.size() );

        {
            VertexBufferList::const_iterator itor = vertexBuffers.begin();
            VertexBufferList::const_iterator end  = vertexBuffers.end();

            while( itor != end )
            {
                Vao::VertexBinding vertexBinding;
                vertexBinding.vertexBufferVbo   = static_cast<D3D11BufferOp*>((*itor)->getOperation() )->getVboName();
                vertexBinding.mUnits    = (*itor)->getUnits();
                vertexBinding.stride            = calculateVertexSize( vertexBinding.mUnits );
                vertexBinding.offset            = 0;
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
            vao.indexBufferVbo  = static_cast<D3D11BufferOp*>(indexBuffer->getOperation() )->getVboName();
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
            if( itor->mType == vao.mType && itor->indexBufferVbo == vao.indexBufferVbo &&
                itor->indexType == vao.indexType && itor->vertexBuffers == vao.vertexBuffers )
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

            //Bake all the D3D11 data that is shared by all VAOs.
            vao.sharedData = new D3D11VertexArrayObjectShared( vertexBuffers, indexBuffer, opType, mDrawId );

            mVaos.push_back( vao );
            itor = mVaos.begin() + mVaos.size() - 1;
        }

        ++itor->refCount;

        return itor;
    }
    //-----------------------------------------------------------------------------------
    uint32 D3D11VaoManager::createVao( const Vao &vaoRef )
    {
        return mVaoNames++;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::releaseVao( GeometryRaw *vao )
    {
        OgreProfileExhaustive( "D3D11VaoManager::releaseVao" );

        D3D11VertexArrayObject *glVao = static_cast<D3D11VertexArrayObject*>( vao );

        VaoVec::iterator itor = mVaos.begin();
        VaoVec::iterator end  = mVaos.end();

        while( itor != end && itor->vaoName != glVao->mVaoId )
            ++itor;

        if( itor != end )
        {
            --itor->refCount;

            if( !itor->refCount )
            {
                //TODO: Remove cached ID3D11InputLayout from all D3D11HLSLProgram
                //(the one generated in D3D11HLSLProgram::getLayoutForVao)
                delete glVao->mSharedData;
                glVao->mSharedData = 0;
                remove( mVaos, itor );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::bindDrawId( uint32 bindSlotId )
    {
        D3D11BufferOp *bufferInterface = static_cast<D3D11BufferOp*>(mDrawId->getOperation() );

        ID3D11Buffer *vertexBuffer = bufferInterface->getVboName();
        UINT stride = mDrawId->getUnitSize();
        UINT offset = 0;

        mDevice.GetImmediateContext()->IASetVertexBuffers(
                    bindSlotId,
                    1,
                    &vertexBuffer,
                    &stride,
                    &offset );
    }
    //-----------------------------------------------------------------------------------
    uint32 D3D11VaoManager::generateRenderQueueId( uint32 vaoName, uint32 uniqueVaoId )
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
    uint32 D3D11VaoManager::extractUniqueVaoIdFromRenderQueueId( uint32 rqId )
    {
        const int bitsVaoGl  = 5;
        const uint32 maskVao = N_Mask( RqBits::MeshBits - bitsVaoGl );
        return rqId & maskVao;
    }
    //-----------------------------------------------------------------------------------
    GeometryRaw* D3D11VaoManager::createVertexArrayObjectImpl(
                                                            const VertexBufferList &vertexBuffers,
                                                            IndexBufferPacked *indexBuffer,
                                                            OperationType opType )
    {
        RenderSys *hlmsManager = Root::getSingleton().getRender();
        VertexUnitListList units = GeometryRaw::getVertexDeclaration( vertexBuffers );
        uint16 inputLayout = hlmsManager->getVaoType( units, opType );

        {
            bool hasImmutableDelayedBuffer = false;
            VertexBufferList::const_iterator itor = vertexBuffers.begin();
            VertexBufferList::const_iterator end  = vertexBuffers.end();

            while( itor != end && !hasImmutableDelayedBuffer )
            {
                if( (*itor)->mMark == M_DEVREAD )
                {
                    D3D11BufferOp *bufferInterface = static_cast<D3D11BufferOp*>((*itor)->getOperation() );

                    if( bufferInterface->_getInitialData() != 0 )
                        hasImmutableDelayedBuffer = true;
                }
                ++itor;
            }

            if( indexBuffer && indexBuffer->mMark == M_DEVREAD )
            {
                D3D11BufferOp *bufferInterface = static_cast<D3D11BufferOp*>(indexBuffer->getOperation() );

                if( bufferInterface->_getInitialData() != 0 )
                    hasImmutableDelayedBuffer = true;
            }

            if( hasImmutableDelayedBuffer )
            {
                const uint32 renderQueueId = generateRenderQueueId( 0, mValidVAO );
                //If the Vao contains an immutable buffer that is delayed, we can't
                //create the actual Vao yet. We'll modify the pointer later.
                D3D11VertexArrayObject *retVal = N_new D3D11VertexArrayObject( 0,
                                                                                  renderQueueId,
                                                                                  inputLayout,
                                                                                  vertexBuffers,
                                                                                  indexBuffer,
                                                                                  opType,
                                                                                  0 );
                return retVal;
            }
        }

        VaoVec::iterator itor = findVao( vertexBuffers, indexBuffer, opType );

        const uint32 renderQueueId = generateRenderQueueId( itor->vaoName, mValidVAO );

        D3D11VertexArrayObject *retVal = N_new D3D11VertexArrayObject( itor->vaoName,
                                                                          renderQueueId,
                                                                          inputLayout,
                                                                          vertexBuffers,
                                                                          indexBuffer,
                                                                          opType,
                                                                          itor->sharedData );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::destroyVertexArrayObjectImpl( GeometryRaw *vao )
    {
        releaseVao( vao );

        D3D11VertexArrayObject *glVao = static_cast<D3D11VertexArrayObject*>( vao );
        //We delete it here because this class has no virtual destructor on purpose
        N_delete glVao;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::createImpl(MappedBuffer*& out, NCount sizeBytes, Nmark forUpload )
    {
        sizeBytes = std::max<size_t>( sizeBytes, 4 * 1024 * 1024 );

        ID3D11DeviceN *d3dDevice = mDevice.get();

        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof(D3D11_BUFFER_DESC) );
        desc.ByteWidth  = sizeBytes;

        if( forUpload )
        {
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.Usage          = D3D11_USAGE_DYNAMIC;
            desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER|D3D11_BIND_INDEX_BUFFER;
        }
        else
        {
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage          = D3D11_USAGE_STAGING;
        }

        ComPtr<ID3D11Buffer> bufferName;
        HRESULT hr = d3dDevice->CreateBuffer( &desc, 0, bufferName.GetAddressOf() );

        if( FAILED( hr ) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT( RenderingAPI,
                         "Failed to create buffer. ID3D11Device::CreateBuffer.\n"
                         "Error code: " + StrConv::conv( hr ) + ".\n" +
                         errorDescription +
                         "Requested: " + StrConv::conv( sizeBytes ) +
                         String(" bytes. For ") + (forUpload ? "Uploading" : "Downloading") );
        }

        D3D11MappedBuffer *stagingBuffer = N_new D3D11MappedBuffer( sizeBytes, this,
                                                                         forUpload, bufferName.Get(),
                                                                         mDevice );
        mRefMappedList.push_back( stagingBuffer );

        if( mNextCheck == (unsigned long)( ~0 ) )
            mNextCheck = mTimer->getMilliseconds() + mDefaultStagingBufferLifetime;

        return stagingBuffer;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::create(GpuBufferAsync *& out, GpuBuffer *creator, MappedBuffer *buf, NCount oft, NCount size )
    {
        if( creator->mMark == M_DEVREAD )
            _forceCreateDelayedImmutableBuffers();

        out = N_new D3D11AsyncTicket( creator, buf, oft, size, mDevice );
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::_beginFrame(void)
    {
        createDelayedImmutableBuffers();

        //TODO: If we have many tiny immutable buffers, get the data back to CPU,
        //destroy the buffers and create a unified immutable buffer.
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::_update(void)
    {
        unsigned long currentTimeMs = mTimer->getMilliseconds();

        if( currentTimeMs >= mNextCheck )
        {
            OgreProfileExhaustive( "D3D11VaoManager::_update zero-ref staging buffers" );

            mNextCheck = (unsigned long)(~0);


            MappedBufferList::iterator itor = mFreeMappedList.begin();
            MappedBufferList::iterator end  = mFreeMappedList.end();

            while( itor != end )
            {
                MappedBuffer *stagingBuffer = *itor;

                mNextCheck = std::min(mNextCheck,
                    stagingBuffer->getLastUsedTimestamp() + stagingBuffer->getLifetimeThreshold() );

                if( stagingBuffer->getLastUsedTimestamp() + stagingBuffer->getLifetimeThreshold() < currentTimeMs )
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

        if( !mRecoverList.empty() && mRecoverList.front().mFrameMark == mFrameMark )
        {
            wait();
            destroyRecover( mFrameMark );
            GBufferManager::_update();
        }
        else
        {
            GBufferManager::_update();

            wait();
        }

        mFrameSyncVec[mFrameMark].Reset();
        mFrameSyncVec[mFrameMark] = createFence();
        mFrameMark = (mFrameMark + 1) % mDynamicBufferMultiplier;
    }
    //-----------------------------------------------------------------------------------
    ID3D11Buffer* D3D11VaoManager::getSplicingHelperBuffer(void)
    {
        if( !mSplicingHelperBuffer )
        {
            D3D11_BUFFER_DESC desc;
            ZeroMemory( &desc, sizeof(desc) );

            desc.BindFlags	= 0;
            desc.ByteWidth	= 2048u;
            desc.CPUAccessFlags = 0;
            desc.Usage          = D3D11_USAGE_DEFAULT;

            HRESULT hr = mDevice.get()->CreateBuffer( &desc, 0, mSplicingHelperBuffer.GetAddressOf() );
            if( FAILED( hr ) )
            {
                N_EXCEPT_EX( RenderingAPI, hr, "Failed to create helper buffer!" );
            }
        }
        return mSplicingHelperBuffer.Get();
    }
    //-----------------------------------------------------------------------------------
    ComPtr<ID3D11Query> D3D11VaoManager::createFence( D3D11Device &device )
    {
        ComPtr<ID3D11Query> retVal;

        D3D11_QUERY_DESC queryDesc;
        queryDesc.Query     = D3D11_QUERY_EVENT;
        queryDesc.MiscFlags = 0;
        HRESULT hr = device.get()->CreateQuery( &queryDesc, retVal.GetAddressOf() );

        if( FAILED( hr ) )
        {
            String errorDescription = device.getErrorString(hr);
            N_EXCEPT( RenderingAPI,
                         "Failed to create frame fence.\nError code: " + StrConv::conv( hr ) + ".\n" + errorDescription );
        }

        // Insert the fence into D3D's commands
        device.GetImmediateContext()->End( retVal.Get() );

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    ComPtr<ID3D11Query> D3D11VaoManager::createFence(void)
    {
        return D3D11VaoManager::createFence( mDevice );
    }
    //-----------------------------------------------------------------------------------
    uint8 D3D11VaoManager::wait(void)
    {
        if( mFrameSyncVec[mFrameMark] )
        {
            waitFor( mFrameSyncVec[mFrameMark].Get() );
            mFrameSyncVec[mFrameMark].Reset();
        }

        return mFrameMark;
    }
    //-----------------------------------------------------------------------------------
    void D3D11VaoManager::wait( uint32 frameCount )
    {
        if( frameCount == mFrameCount )
        {
            //Full stall
            ComPtr<ID3D11Query> fence = createFence();
            waitFor( fence.Get() );
            fence.Reset();

            //All of the other per-frame fences are not needed anymore.
            D3D11SyncVec::iterator itor = mFrameSyncVec.begin();
            D3D11SyncVec::iterator end  = mFrameSyncVec.end();

            while( itor != end )
            {
                itor->Reset();
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
            const size_t idx = (mFrameMark +
                                mDynamicBufferMultiplier - frameDiff) % mDynamicBufferMultiplier;
            if( mFrameSyncVec[idx] )
            {
                waitFor( mFrameSyncVec[idx].Get() );
                mFrameSyncVec[idx].Reset();

                //Delete all the fences until this frame we've just waited.
                size_t nextIdx = mFrameMark;
                while( nextIdx != idx )
                {
                    mFrameSyncVec[nextIdx].Reset();
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
    bool D3D11VaoManager::isFrameFinished( uint32 frameCount )
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

            if( mFrameSyncVec[idx] )
            {
                HRESULT hr = mDevice.GetImmediateContext()->GetData( mFrameSyncVec[idx].Get(), NULL, 0, 0 );

                if( FAILED( hr ) )
                {
                    N_EXCEPT( RenderingAPI,
                                 "Failure while waiting for a D3D11 Fence. Could be out of GPU memory. "
                                 "Update your video card drivers. If that doesn't help, contact the developers." );
                }

                retVal = hr != S_FALSE;

                if( retVal )
                {
                    mFrameSyncVec[idx].Reset();

                    //Delete all the fences until this frame we've just waited.
                    size_t nextIdx = mFrameMark;
                    while( nextIdx != idx )
                    {
                        mFrameSyncVec[nextIdx].Reset();
                        nextIdx = (nextIdx + 1u) % mDynamicBufferMultiplier;
                    }
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
    ID3D11Query* D3D11VaoManager::waitFor( ID3D11Query *fenceName, ID3D11DeviceContextN *deviceContext )
    {
        HRESULT hr = S_FALSE;

        while( hr == S_FALSE )
        {
            hr = deviceContext->GetData( fenceName, NULL, 0, 0 );

            if( FAILED( hr ) )
            {
                N_EXCEPT( RenderingAPI,
                             "Failure while waiting for a D3D11 Fence. Could be out of GPU memory. "
                             "Update your video card drivers. If that doesn't help, contact the developers." );

                return fenceName;
            }

            if( hr == S_FALSE )
            {
                //Give HyperThreading threads a breath on this spinlock.
                YieldProcessor();
            }
        } // spin until event is finished

        return fenceName;
    }
    //-----------------------------------------------------------------------------------
    ID3D11Query* D3D11VaoManager::waitFor( ID3D11Query *fenceName )
    {
        return waitFor( fenceName, mDevice.GetImmediateContext() );
    }
    //-----------------------------------------------------------------------------------
    bool D3D11VaoManager::queryIsDone( ID3D11Query *fenceName, ID3D11DeviceContextN *deviceContext )
    {
        HRESULT hr = deviceContext->GetData( fenceName, NULL, 0, 0 );

        if( FAILED( hr ) )
        {
            N_EXCEPT( RenderingAPI,
                         "Failure while waiting for a D3D11 Fence. Could be out of GPU memory. "
                         "Update your video card drivers. If that doesn't help, contact the developers." );

            return true;
        }

        return hr != S_FALSE;
    }
    //-----------------------------------------------------------------------------------
    bool D3D11VaoManager::queryIsDone( ID3D11Query *fenceName )
    {
        return queryIsDone( fenceName, mDevice.GetImmediateContext() );
    }
    //-----------------------------------------------------------------------------------
}
