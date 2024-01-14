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
#include "OgreD3D11HardwareBuffer.h"
#include "OgreD3D11Mappings.h"
#include "OgreD3D11Device.h"
#include "OgreException.h"
#include "Vao/OgreD3D11GBufferManager.h"
#include "Vao/OgreD3D11StagingBuffer.h"
#include "Vao/OgreD3D11DynamicBuffer.h"

#include "OgreD3D11Device.h"
#include "OgreD3D11RenderSystem.h"
#include "OgrePixelFormatGpuUtils.h"
#include "OgreD3D11Texture.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreBitwise.h"

#include "OgreRoot.h"
#include "OgreRenderSystem.h"

#include <algorithm>

namespace Ogre 
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11Buffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11Buffer::D3D11Buffer(ID3D11Buffer * vboid, NCount size, D3D11Device & device) :
        mVboName(vboid),
        mVboSize(size),
        mMappedPtr(0),
        mDevice(device)
    {
    }
    //-----------------------------------------------------------------------------------
    D3D11Buffer::~D3D11Buffer()
    {
    }
    //-----------------------------------------------------------------------------------
    NCount D3D11Buffer::addMappedRange(NCount start, NCount count)
    {
        NCount ticket;

        if(mFreeRanges.empty() )
        {
            ticket = mMappedRanges.size();
            mMappedRanges.push_back(MappedRange(start, count));
        }
        else
        {
            ticket = mFreeRanges.back();
            mMappedRanges[ticket] = MappedRange(start, count);
            mFreeRanges.pop_back();
        }

        return ticket;
    }
    //-----------------------------------------------------------------------------------
    void * D3D11Buffer::map(NCount start, NCount count, NCount & outTicket)
    {
        N_assert(start <= mVboSize && start + count <= mVboSize);

        if(mMappedRanges.size() == mFreeRanges.size())
        {
            D3D11_MAPPED_SUBRESOURCE mappedSubres;
            mDevice.GetImmediateContext()->Map(mVboName.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedSubres);
            mMappedPtr = mappedSubres.pData;
        }

        outTicket = addMappedRange(start, count);

        return static_cast<uint8 *>(mMappedPtr) + start;
    }
    //-----------------------------------------------------------------------------------
    void D3D11Buffer::unmap(size_t ticket)
    {
        N_assert( ticket < mMappedRanges.size() && "Invalid unmap ticket!" );
        N_assert( mMappedRanges.size() != mFreeRanges.size() && "Unmapping an already unmapped buffer! Did you call unmap with the same ticket twice?" );

        mFreeRanges.push_back( ticket );

        if( mMappedRanges.size() == mFreeRanges.size() )
        {
            mDevice.GetImmediateContext()->Unmap( mVboName.Get(), 0 );
            mMappedPtr = 0;
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11BufferOpBase
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11BufferOpBase::D3D11BufferOpBase( size_t vboPoolIdx, ID3D11Buffer * d3dBuffer ) :
        mGroupIndex( vboPoolIdx ),
        mVboName( d3dBuffer ),
        mMappedPtr( 0 )
    {
    }
    //-----------------------------------------------------------------------------------
    D3D11BufferOpBase::~D3D11BufferOpBase()
    {
    }
    //-----------------------------------------------------------------------------------
    void D3D11BufferOpBase::splicedCopy( size_t dstOffsetBytes, size_t srcOffsetBytes,
        size_t sizeBytes, size_t alignment, ID3D11Buffer *dstBuffer, ID3D11Buffer * srcBuffer, ID3D11DeviceContextN *context )
    {
        OGRE_ASSERT_MEDIUM( (dstOffsetBytes % alignment) == 0u );

        D3D11GBufferManager * vaoManager = static_cast<D3D11GBufferManager *>( mBuffer->mMag );
        ID3D11Buffer * helperBuffer = vaoManager->getSplicingHelperBuffer();

        D3D11_BOX srcBox;
        srcBox.top      = 0u;
        srcBox.bottom   = 1u;
        srcBox.front    = 0u;
        srcBox.back     = 1u;

        //Only the tail end needs to be spliced
        size_t dstBlockStart= multiFloor( dstOffsetBytes + sizeBytes, alignment );
        size_t dstBlockEnd  = multiCeil( dstOffsetBytes + sizeBytes, alignment );

        //Copy DST's block current contents
        srcBox.left = static_cast<UINT>( dstBlockStart );
        srcBox.right = static_cast<UINT>( dstBlockEnd );
        context->CopySubresourceRegion( helperBuffer, 0, 0, 0, 0, dstBuffer, 0, &srcBox );

        //Merge in SRC's contents
        srcBox.left = static_cast<UINT>( srcOffsetBytes + multiFloor( sizeBytes, alignment ) );
        srcBox.right = srcBox.left + static_cast<UINT>( sizeBytes % alignment );
        context->CopySubresourceRegion( helperBuffer, 0, 0, 0, 0, srcBuffer, 0, &srcBox );

        //Copy spliced result into DST block
        srcBox.left = static_cast<UINT>( 0u );
        srcBox.right = static_cast<UINT>( alignment );
        context->CopySubresourceRegion( dstBuffer, 0, static_cast<UINT>( dstBlockStart ), 0, 0, helperBuffer, 0, &srcBox );
    }
    //-----------------------------------------------------------------------------------
    void D3D11BufferOpBase::memcpyImpl( Buffer * srcBuffer, size_t srcOft, size_t dstOft, size_t sizeBytes )
    {
        D3D11GBufferManager * vaoManager = static_cast<D3D11GBufferManager*>( mBuffer->mMag );
        ID3D11DeviceContextN * context = vaoManager->getDevice().GetImmediateContext();

        N_assert( dynamic_cast<D3D11BufferOpBase*>( srcBuffer ) );
        D3D11BufferOpBase * srcbuf = static_cast<D3D11BufferOpBase*>( srcBuffer );

        D3D11_BOX srcBox;
        srcBox.top      = 0u;
        srcBox.bottom   = 1u;
        srcBox.front    = 0u;
        srcBox.back     = 1u;

        if( (mData->getBufferPackedType() == BP_TYPE_UAV ||
            mData->getBufferPackedType() == BP_TYPE_TEX) && sizeBytes % mData->getUnitSize() )
        {
            //D3D11 wants the length of copies to/from structured buffers to be multiples of the stride.
            //We have to perform splicing with a temporary helper buffer to deal with the issue.
            const size_t alignment = mData->getUnitSize();

            if( dstOft % alignment )
            {
                N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Copying to a destination offset that is not multiple of dst->getUnitSize()" );
            }

            if( sizeBytes >= mData->getUnitSize() )
            {
                //Copy range [srcOft; srcOft + sizeBytes - alignment % sizeBytes)
                //which is allowed by D3D11
                srcBox.left = static_cast<UINT>( srcOft );
                srcBox.right = static_cast<UINT>( srcOft + multiFloor( sizeBytes, alignment ) );
                context->CopySubresourceRegion( mVboName.Get(), 0, static_cast<UINT>( dstOft ), 0, 0, srcbuf->mVboName.Get(), 0, &srcBox );
            }

            //Now deal with the last few bytes (which is up to 'alignment - 1u' bytes)
            splicedCopy( dstOft, srcOft, sizeBytes, alignment, mVboName.Get(), srcbuf->mVboName.Get(), context );
        }
        else
        {
            srcBox.left = static_cast<UINT>( srcOft );
            srcBox.right = static_cast<UINT>( srcOft + sizeBytes );
            context->CopySubresourceRegion( mVboName.Get(), 0, static_cast<UINT>( dstOft ), 0, 0, srcbuf->mVboName.Get(), 0, &srcBox );
        }
    }
    //-----------------------------------------------------------------------------------
    bool D3D11BufferOpBase::reserveImpl(NCount size, BufferModeMark newMode, bool oldData)
    {
        
    }
    //-----------------------------------------------------------------------------------
    void D3D11BufferOpBase::waitImpl(void)
    {
        if( !mVboName )
        {
            D3D11GBufferManager *mag = static_cast<D3D11GBufferManager*>( mBuffer->mMag );
            mag->_forceCreateDelayedImmutableBuffers();
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11BufferOp
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11BufferOp::D3D11BufferOp( size_t vboPoolIdx, ID3D11Buffer * d3dBuffer, D3D11Buffer * dynamicBuffer) :
        D3D11BufferOpBase( vboPoolIdx, d3dBuffer ),
        mUnmapTicket((size_t)~0),
        mDynamicBuffer(dynamicBuffer),
        mInitialData(0)
    {
    }
    //-----------------------------------------------------------------------------------
    D3D11BufferOp::~D3D11BufferOp()
    {
        _deleteInitialData();
    }
    //-----------------------------------------------------------------------------------
    void D3D11BufferOp::_init(void * data)
    {
        //In D3D11, we upload non-immutable buffers the traditional way.
        //For immutable buffers, we store the data into CPU memory until the last moment
        //when the buffer may be need, and only then gets the D3D11 buffer created and
        //batched together with many other buffers (and mInitialData may be freed then).

        if(N_MarkOnlyTrue(mBuffer->mMark, M_USAGEMARK, M_DEVREAD))
        {
            if( mBuffer->mShadow )
            {
                //Reference the shadow copy directly.
                mInitialData = mBuffer->mShadow;
            }
            else
            {
                //The initial data pointer may be lost. Copy it to a temporary location.
                mInitialData = OGRE_MALLOC_SIMD( mBuffer->getSize(), MEMCATEGORY_GEOMETRY );
                memcpy( mInitialData, data, mBuffer->getSize() );
            }
        }
        else
        {
            upload( data, 0, mBuffer->getUnitCount() );
        }
    }
    //-----------------------------------------------------------------------------------
    void * D3D11BufferOp::lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx)
    {
        //size_t unitSize = mBuffer->mUnitSize;

        D3D11GBufferManager * mag = static_cast<D3D11GBufferManager *>( mBuffer->mMag );

        mag->wait();

        //size_t dFrame = advanceFrame( bAdvanceFrame );

        {
            //Non-persistent buffers just map the small region they'll need.
            size_t offset = mBuffer->mOffset + oft + mBuffer->mSize * multiIdx;
            size_t cnt = size;

            mMappedPtr = mDynamicBuffer->map( offset, cnt, mUnmapTicket );
        }

        //For regular maps, mMuteOft is 0. So that we can later flush correctly.
        mBuffer->mMuteOft = 0;
        mBuffer->mMuteSize = size;
        mMapping = true;
        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    void D3D11BufferOp::_deleteInitialData(void)
    {
        if( mInitialData )
        {
            if( !mBuffer->mShadow )
                OGRE_FREE_SIMD( mInitialData, MEMCATEGORY_GEOMETRY );
            mInitialData = 0;
        }
    }
    //---------------------------------------------------------------------
    void D3D11BufferOp::_setVboName(size_t vboPoolIdx, ID3D11Buffer * vboName, size_t oft)
    {
        mGroupIndex = vboPoolIdx;
        mVboName = vboName;

        mBuffer->mOffset = oft;
        //mBuffer->mCurrentOffset = mBuffer->mOffset;
    }
    //---------------------------------------------------------------------
    void D3D11BufferOp::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
        //All arguments aren't really used by D3D11, these asserts are for the other APIs.
        N_assert( oft <= mBuffer->mMuteSize && "Flush starts after the end of the mapped region!" );
        N_assert( oft + size <= mBuffer->mMuteSize && "Flush region out of bounds!" );

        mDynamicBuffer->unmap( mUnmapTicket );
        mMappedPtr = 0;
        mMapping = false;
    }
    //---------------------------------------------------------------------
    /*size_t D3D11BufferOp::advanceFrame(bool bAdvanceFrame)
    {
        D3D11GBufferManager * mag = static_cast<D3D11GBufferManager *>(mBuffer->mMag);
        size_t dFrame = mBuffer->mCurrentOffset - mBuffer->mOffset;
        dFrame /= mBuffer->mUnitCount;

        dFrame = (dFrame + 1) % mag->getMutilCount();

        if( bAdvanceFrame )
        {
            mBuffer->mCurrentOffset = mBuffer->mOffset + dFrame * mBuffer->mUnitCount;
        }

        return dFrame;
    }
    //---------------------------------------------------------------------
    void D3D11BufferOp::regressFrame(void)
    {
        D3D11GBufferManager * mag = static_cast<D3D11GBufferManager *>(mBuffer->mMag);
        size_t dFrame = mBuffer->mCurrentOffset - mBuffer->mOffset;
        dFrame /= mBuffer->mUnitCount;

        dFrame = (dFrame + mag->getMutilCount() - 1) % mag->getMutilCount();

        mBuffer->mCurrentOffset = mBuffer->mOffset + dFrame * mBuffer->mUnitCount;
    }*/
    //---------------------------------------------------------------------
    D3D11CompatBufferInterface::D3D11CompatBufferInterface(size_t vboPoolIdx, ID3D11Buffer * d3dBuffer, D3D11Device & device) :
        D3D11BufferOpBase( vboPoolIdx, d3dBuffer ),
        mDevice( device )
    {
    }
    //-----------------------------------------------------------------------------------
    D3D11CompatBufferInterface::~D3D11CompatBufferInterface()
    {
    }
    //-----------------------------------------------------------------------------------
    void* D3D11CompatBufferInterface::lockImpl(MuteMark m, NCount oft, NCount size, NCount multiIdx)
    {
        N_assert( oft + size <= mBuffer->mSize );

        D3D11RenderSystem * renderSystem = static_cast<D3D11GBufferManager*>( mBuffer->mMag )-> getD3D11RenderSystem();

        D3D11_MAP mapFlag = D3D11_MAP_WRITE_NO_OVERWRITE;

        if( renderSystem->_getFeatureLevel() <= D3D_FEATURE_LEVEL_11_0 ||
            (renderSystem->_getFeatureLevel() > D3D_FEATURE_LEVEL_11_0 &&
            oft <= mBuffer->mMuteOft + mBuffer->mMuteSize) )
        {
            //D3D 11.0 and below doesn't support NO_OVERWRITE
            //On D3D 11.1, only use discard when wrapping back.
            mapFlag = D3D11_MAP_WRITE_DISCARD;
        }

        D3D11_MAPPED_SUBRESOURCE mappedSubres;
        mDevice.GetImmediateContext()->Map( mVboName.Get(), 0, mapFlag, 0, &mappedSubres );
        mMappedPtr = reinterpret_cast<uint8 *>( mappedSubres.pData ) + oft;

        //Store the last map start so we know when to discard on the next map.
        mBuffer->mMuteOft = oft + size;
        //Store the count for the N_assert in unmap
        mBuffer->mMuteSize = size;
        mMapping = true;
        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    void D3D11CompatBufferInterface::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
        //All arguments aren't really used by D3D11, these asserts are for the other APIs.
        N_assert( oft <= mBuffer->mMuteSize && "Flush starts after the end of the mapped region!" );
        N_assert( oft + size <= mBuffer->mMuteSize && "Flush region out of bounds!" );

        mDevice.GetImmediateContext()->Unmap( mVboName.Get(), 0 );
        mMappedPtr = 0;
        mMapping = false;
    }
    //-----------------------------------------------------------------------------------
    void D3D11CompatBufferInterface::advanceFrame(void)
    {
    }
    //-----------------------------------------------------------------------------------
    void D3D11CompatBufferInterface::regressFrame(void)
    {
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // D3D11HardwareBuffer
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    D3D11HardwareBuffer::D3D11HardwareBuffer(
        BufferType btype, size_t sizeBytes,
        Buffer::Usage usage, D3D11Device & device, 
        bool useSystemMemory, bool useShadowBuffer, bool streamOut)
        : Buffer(usage, useSystemMemory,  useShadowBuffer),
        mpTempStagingBuffer(0),
        mUseTempStagingBuffer(false),
        mBufferType(btype),
        mDevice(device)
    {
        mSizeInBytes = sizeBytes;
        mDesc.ByteWidth = static_cast<UINT>(sizeBytes);
        mDesc.CPUAccessFlags = D3D11Mappings::_getAccessFlags(mUsage); 
        mDesc.MiscFlags = 0;

        if (useSystemMemory)
        {
            mDesc.Usage = D3D11_USAGE_STAGING;
            //A D3D11_USAGE_STAGING Resource cannot be bound to any parts of the graphics pipeline, so therefore cannot have any BindFlags bits set.
            mDesc.BindFlags = 0;
            mDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ ;// A D3D11_USAGE_STAGING Resource must have at least one CPUAccessFlag bit set.

        }
        else
        {
            mDesc.Usage = D3D11Mappings::_getUsage(mUsage);
            mDesc.BindFlags = btype == VERTEX_BUFFER ? D3D11_BIND_VERTEX_BUFFER : 
                              btype == INDEX_BUFFER  ? D3D11_BIND_INDEX_BUFFER  :
                                                       D3D11_BIND_CONSTANT_BUFFER;
        }
        // Better check of stream out flag
        if (streamOut && btype != CONSTANT_BUFFER)
        {
            mDesc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
        }

        if (!useSystemMemory && (usage & Buffer::HBU_DYNAMIC))
        {
            // We want to be able to map this buffer
            mDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
            mDesc.Usage = D3D11_USAGE_DYNAMIC;
        }

        // Note that in D3D11 you can only directly lock (map) a dynamic resource
        // directly for writing. You can only map for read / write staging resources,
        // which themselves cannot be used for input / output to the GPU. Thus
        // for any locks except write locks on dynamic resources, we have to use
        // temporary staging resources instead and use async copies.
        // We use the 'useSystemMemory' option to indicate a staging resource


        // TODO: we can explicitly initialise the buffer contents here if we like
        // not doing this since OGRE doesn't support this model yet
        HRESULT hr = device->CreateBuffer( &mDesc, NULL, mlpD3DBuffer.ReleaseAndGetAddressOf() );
        if (FAILED(hr) || mDevice.isError())
        {
            String msg = device.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr, "Cannot create D3D11 buffer: " + msg);
        }

        // Create shadow buffer
        if (mUseShadowBuffer)
        {
            mShadowBuffer.reset(new D3D11HardwareBuffer(mBufferType, mSizeInBytes, mUsage, mDevice, true, false, false));
        }

    }
    //---------------------------------------------------------------------
    D3D11HardwareBuffer::~D3D11HardwareBuffer()
    {
        SAFE_DELETE(mpTempStagingBuffer); // should never be nonzero unless destroyed while locked
        mShadowBuffer.reset();
    }
    //---------------------------------------------------------------------
    void * D3D11HardwareBuffer::lockImpl(size_t offset, size_t length, LockOptions options)
    {
        if (length > mSizeInBytes)
        {
            // need to realloc
            mDesc.ByteWidth = static_cast<UINT>(mSizeInBytes);
            HRESULT hr = mDevice->CreateBuffer(&mDesc, 0, mlpD3DBuffer.ReleaseAndGetAddressOf());
            if (FAILED(hr) || mDevice.isError())
            {
                String msg = mDevice.getErrorString(hr);
				N_EXCEPT_EX(RenderingAPI, hr, "Cannot create D3D11 buffer: " + msg);
            }
        }


        if (mSystemMemory ||
            (mUsage & Buffer::HBU_DYNAMIC && 
            (options == Buffer::HBL_DISCARD || options == Buffer::HBL_NO_OVERWRITE)))
        {
            // Staging (system memory) buffers or dynamic, write-only buffers 
            // are the only case where we can lock directly
            // We have no 'lock for writing' but discard + locking the entire
            // buffer essentially means the same thing, especially since it's
            // not possible to map part of a buffer in Dx11

            // map directly
            D3D11_MAP mapType;
            switch(options)
            {
            case HBL_DISCARD:
                // To use D3D11_MAP_WRITE_DISCARD resource must have been created with write access and dynamic usage.
                mapType = mSystemMemory ? D3D11_MAP_WRITE : D3D11_MAP_WRITE_DISCARD;
                break;
            case HBL_NO_OVERWRITE:
                // To use D3D11_MAP_WRITE_NO_OVERWRITE resource must have been created with write access.
                // TODO: check (mSystemMemory aka D3D11_USAGE_STAGING => D3D11_MAP_WRITE_NO_OVERWRITE) combo - it`s not forbidden by MSDN
                mapType = mSystemMemory ? D3D11_MAP_WRITE : D3D11_MAP_WRITE_NO_OVERWRITE; 
                break;
            case HBL_NORMAL:
                mapType = (mDesc.CPUAccessFlags & D3D11_CPU_ACCESS_READ) ? D3D11_MAP_READ_WRITE : D3D11_MAP_WRITE;
                break;
            case HBL_READ_ONLY:
                mapType = D3D11_MAP_READ;
                break;
            case HBL_WRITE_ONLY:
                mapType = D3D11_MAP_WRITE;
                break;
            }

            void * pRet = NULL;
            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
            mappedSubResource.pData = NULL;
            HRESULT hr = mDevice.GetImmediateContext()->Map(mlpD3DBuffer.Get(), 0, mapType, 0, &mappedSubResource);
            if (FAILED(hr) || mDevice.isError())
            {
                String msg = mDevice.getErrorString(hr);
				N_EXCEPT_EX(RenderingAPI, hr, "Error calling Map: " + msg);
            }

            pRet = static_cast<void*>(static_cast<char*>(mappedSubResource.pData) + offset);

            return pRet;

        }
        else
        {
            mUseTempStagingBuffer = true;
            if (!mpTempStagingBuffer)
            {
                // create another buffer instance but use system memory
                mpTempStagingBuffer = new D3D11HardwareBuffer(mBufferType, mSizeInBytes, mUsage, mDevice, true, false, false);
            }

            // schedule a copy to the staging
            if (options != HBL_DISCARD)
                mpTempStagingBuffer->copyData(*this, 0, 0, mSizeInBytes, true);

            // register whether we'll need to upload on unlock
            mStagingUploadNeeded = (options != HBL_READ_ONLY);

            return mpTempStagingBuffer->lock(offset, length, options);
        }
    }
    //---------------------------------------------------------------------
    void D3D11HardwareBuffer::unlockImpl(void)
    {
        if (mUseTempStagingBuffer)
        {
            mUseTempStagingBuffer = false;

            // ok, we locked the staging buffer
            mpTempStagingBuffer->unlock();

            // copy data if needed
            // this is async but driver should keep reference
            if (mStagingUploadNeeded)
                copyData(*mpTempStagingBuffer, 0, 0, mSizeInBytes, true);

            // delete
            // not that efficient, but we should not be locking often
            SAFE_DELETE(mpTempStagingBuffer);
        }
        else
        {
            // unmap
            mDevice.GetImmediateContext()->Unmap(mlpD3DBuffer.Get(), 0);
        }
    }
    //---------------------------------------------------------------------
    void D3D11HardwareBuffer::copyData(Buffer& srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer)
    {
		if (mUseShadowBuffer)
		{
			static_cast<D3D11HardwareBuffer*>(mShadowBuffer.get())->copyDataImpl(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
		}
		copyDataImpl(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}
	//---------------------------------------------------------------------
	void D3D11HardwareBuffer::copyDataImpl(Buffer& srcBuffer, size_t srcOffset, size_t dstOffset, size_t length, bool discardWholeBuffer)
	{
        // If we're copying same-size buffers in their entirety...
        if (srcOffset == 0 && dstOffset == 0 &&
            length == mSizeInBytes && mSizeInBytes == srcBuffer.getSizeInBytes())
        {
            // schedule hardware buffer copy
            mDevice.GetImmediateContext()->CopyResource(mlpD3DBuffer.Get(), static_cast<D3D11HardwareBuffer&>(srcBuffer).getD3DBuffer());
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI,  "Cannot copy D3D11 resource\nError Description:" + errorDescription);
            }
        }
        else
        {
            // copy subregion
            D3D11_BOX srcBox;
            srcBox.left = (UINT)srcOffset;
            srcBox.right = (UINT)srcOffset + length;
            srcBox.top = 0;
            srcBox.bottom = 1;
            srcBox.front = 0;
            srcBox.back = 1;

            mDevice.GetImmediateContext()->CopySubresourceRegion(mlpD3DBuffer.Get(), 0, (UINT)dstOffset, 0, 0, 
                static_cast<D3D11HardwareBuffer&>(srcBuffer).getD3DBuffer(), 0, &srcBox);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, "Cannot copy D3D11 subresource region\nError Description:" + errorDescription);
            }
		}
	}
	//---------------------------------------------------------------------
	void D3D11HardwareBuffer::_updateFromShadow(void)
	{
		if(mUseShadowBuffer && mShadowUpdated && !mSuppressHardwareUpdate)
		{
			bool discardWholeBuffer = mLockStart == 0 && mLockSize == mSizeInBytes;
			copyDataImpl(*mShadowBuffer, mLockStart, mLockStart, mLockSize, discardWholeBuffer);
			mShadowUpdated = false;
        }
    }
    //---------------------------------------------------------------------
    void D3D11HardwareBuffer::readData(size_t offset, size_t length, void* pDest)
    {
        // There is no functional interface in D3D, just do via manual 
        // lock, copy & unlock
        HardwareBufferLockGuard thisLock(this, offset, length, Buffer::HBL_READ_ONLY);
        memcpy(pDest, thisLock.pData, length);
    }
    //---------------------------------------------------------------------
    void D3D11HardwareBuffer::writeData(size_t offset, size_t length, const void* pSource, bool discardWholeBuffer)
    {
        // There is no functional interface in D3D, just do via manual 
        // lock, copy & unlock
        HardwareBufferLockGuard thisLock(this, offset, length,
            discardWholeBuffer ? Buffer::HBL_DISCARD : Buffer::HBL_NORMAL);
        memcpy(thisLock.pData, pSource, length);

        //What if we try UpdateSubresource
        //mDevice.GetImmediateContext()->UpdateSubresource(mlpD3DBuffer.Get(), 0, NULL, pSource, offset, length);
    }
    //---------------------------------------------------------------------
    
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // D3D11IndexBuffer
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    D3D11IndexBuffer::D3D11IndexBuffer(GBufferManager* mgr, IndexBuffer::IndexType idxType, 
        size_t numIndexes, Buffer::Usage usage, D3D11Device & device, bool useSystemMemory, bool useShadowBuffer) : 
            IndexBuffer(mgr, idxType, numIndexes, usage, useSystemMemory, useShadowBuffer)
    {
        // everything is done via internal generalisation
        mBufferImpl = new D3D11HardwareBuffer(D3D11HardwareBuffer::INDEX_BUFFER, 
            mSizeInBytes, mUsage, device, useSystemMemory, useShadowBuffer, false);
    }
    //---------------------------------------------------------------------
    D3D11IndexBuffer::~D3D11IndexBuffer()
    {
        delete mBufferImpl;
    }
    //---------------------------------------------------------------------
    void* D3D11IndexBuffer::lock(size_t offset, size_t length, LockOptions options)
    {
        return mBufferImpl->lock(offset, length, options);
    }
    //---------------------------------------------------------------------
    void D3D11IndexBuffer::unlock(void)
    {
        mBufferImpl->unlock();
    }
    //---------------------------------------------------------------------
    void D3D11IndexBuffer::readData(size_t offset, size_t length, void* pDest)
    {
        mBufferImpl->readData(offset, length, pDest);
    }
    //---------------------------------------------------------------------
    void D3D11IndexBuffer::writeData(size_t offset, size_t length, const void* pSource,
        bool discardWholeBuffer)
    {
        mBufferImpl->writeData(offset, length, pSource, discardWholeBuffer);
    }
    //---------------------------------------------------------------------
    void D3D11IndexBuffer::copyData(Buffer& srcBuffer, size_t srcOffset, 
        size_t dstOffset, size_t length, bool discardWholeBuffer)
    {
        // check if the other buffer is also a D3D11IndexBuffer
        if (srcBuffer.isSystemMemory())
        {
            // src is not a D3D11IndexBuffer - use default copy
            Buffer::copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
        }
        else
        {
            // src is a D3D11IndexBuffer use d3d11 optimized copy
            D3D11IndexBuffer& d3dBuf = static_cast<D3D11IndexBuffer&>(srcBuffer);

            mBufferImpl->copyData(*(d3dBuf.mBufferImpl), srcOffset, dstOffset, length, discardWholeBuffer);
        }
    }
    //---------------------------------------------------------------------
    bool D3D11IndexBuffer::isLocked(void) const
    {
        return mBufferImpl->isLocked();
    }
    //---------------------------------------------------------------------
    ID3D11Buffer * D3D11IndexBuffer::getD3DIndexBuffer( void ) const
    {
        return mBufferImpl->getD3DBuffer();
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // D3D11VertexBuffer
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    D3D11VertexBuffer::D3D11VertexBuffer(GBufferManager* mgr, size_t vertexSize, 
        size_t numVertices, Buffer::Usage usage, D3D11Device & device, 
        bool useSystemMemory, bool useShadowBuffer, bool streamOut)
        : VertexBuffer(mgr, vertexSize, numVertices, usage, useSystemMemory, useShadowBuffer),
          mBufferImpl(0)
          
    {
        // everything is done via internal generalisation
        mBufferImpl = new D3D11HardwareBuffer(D3D11HardwareBuffer::VERTEX_BUFFER, 
            mSizeInBytes, mUsage, device, useSystemMemory, useShadowBuffer, streamOut);
    }
    //---------------------------------------------------------------------
    D3D11VertexBuffer::~D3D11VertexBuffer()
    {
        SAFE_DELETE(mBufferImpl);
    }
    //---------------------------------------------------------------------
    void * D3D11VertexBuffer::lock(size_t offset, size_t length, LockOptions options)
    {
        return mBufferImpl->lock(offset, length, options);
    }
    //---------------------------------------------------------------------
    void D3D11VertexBuffer::unlock(void)
    {
        mBufferImpl->unlock();
    }
    //---------------------------------------------------------------------
    void D3D11VertexBuffer::readData(size_t offset, size_t length, void* pDest)
    {
        mBufferImpl->readData(offset, length, pDest);
    }
    //---------------------------------------------------------------------
    void D3D11VertexBuffer::writeData(size_t offset, size_t length, const void* pSource,
        bool discardWholeBuffer)
    {
        mBufferImpl->writeData(offset, length, pSource, discardWholeBuffer);
    }
    //---------------------------------------------------------------------
    void D3D11VertexBuffer::copyData(Buffer& srcBuffer, size_t srcOffset, 
        size_t dstOffset, size_t length, bool discardWholeBuffer)
    {
        // check if the other buffer is also a D3D11VertexBuffer
        if (srcBuffer.isSystemMemory())
        {
            // src is not a D3D11VertexBuffer - use default copy
            Buffer::copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
        }
        else
        {
            // src is a D3D11VertexBuffer use d3d11 optimized copy
            D3D11VertexBuffer& d3dBuf = static_cast<D3D11VertexBuffer&>(srcBuffer);

			mBufferImpl->copyData(*(d3dBuf.mBufferImpl), srcOffset, dstOffset, length, discardWholeBuffer);
		}
	}
	//---------------------------------------------------------------------
	bool D3D11VertexBuffer::isLocked(void) const
	{
		return mBufferImpl->isLocked();
	}
	//---------------------------------------------------------------------
    ID3D11Buffer * D3D11VertexBuffer::getD3DVertexBuffer( void ) const
    {
        return mBufferImpl->getD3DBuffer();
    }
    //---------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // D3D11FrameBuffer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
	namespace
	{
		const char* toString(TextureType textureType)
		{
			switch(textureType)
			{
			case TEX_TYPE_1D:       return "1D texture";
			case TEX_TYPE_CUBE_MAP: return "cube map texture";
			case TEX_TYPE_2D:       return "2D texture";
			case TEX_TYPE_2D_ARRAY: return "2D texture array";
			case TEX_TYPE_3D:       return "3D texture";
			default:                return "texture";
			}
		}
	}
    //-----------------------------------------------------------------------------  
    //-----------------------------------------------------------------------------  
    // D3D11FrameBuffer
    //-----------------------------------------------------------------------------  
    //-----------------------------------------------------------------------------  
    D3D11FrameBuffer::D3D11FrameBuffer(D3D11Texture * parentTexture, D3D11Device & device, UINT mipLevel,
        size_t width, size_t height, size_t depth, UINT face, PixelFormat format, Buffer::Usage usage):
            FrameBuffer(width, height, depth, format, usage, false, false),
            mParentTexture(parentTexture),
            mDevice(device),
            mFace(face),
            mMipLevel(mipLevel)
    {
        if(mUsage & TU_RENDERTARGET)
        {
            // Create render target for each slice
            mSliceTRT.reserve(mDepth);
            for(size_t zoffset=0; zoffset<mDepth; ++zoffset)
            {
                String name;
                name = "rtt/"+StrConv::conv((size_t)this) + "/" + parentTexture->getName();

                RenderTexture *trt = new D3D11RenderTexture(name, this, zoffset, mDevice);
                mSliceTRT.push_back(trt);
                Root::getSingleton().getRenderSystem()->attachRenderTarget(*trt);
            }
        }
		
		mSizeInBytes = PixelUtil::getSize(mWidth, mHeight, mDepth, mFormat);
		
    }
    //----------------------------------------------------------------------------- 
    D3D11FrameBuffer::~D3D11FrameBuffer()
    {
        if(!mSliceTRT.empty())
        {   
            // Delete all render targets that are not yet deleted via destroyImpl
            for(size_t zoffset=0; zoffset<mDepth; ++zoffset)
            {
                if(mSliceTRT[zoffset])
                    Root::getSingleton().getRenderSystem()->destroyRenderTarget(mSliceTRT[zoffset]->getName());
            }
        }
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::_map(ID3D11Resource *res, D3D11_MAP flags, PixelBox & box)
    {
        N_assert(mLockedBox.getDepth() == 1 || mParentTexture->getTextureType() == TEX_TYPE_3D);

        D3D11_MAPPED_SUBRESOURCE pMappedResource = { 0 };
        UINT subresource = (res == mStagingBuffer.Get()) ? 0 : getSubresourceIndex(mLockedBox.front);

        HRESULT hr = mDevice.GetImmediateContext()->Map(res, subresource, flags, 0, &pMappedResource);
        if(FAILED(hr) || mDevice.isError())
        {
            String errorDescription; errorDescription
                .append("D3D11 device cannot map ").append(toString(mParentTexture->getTextureType()))
                .append("\nError Description:").append(mDevice.getErrorString(hr));
            N_EXCEPT_EX(RenderingAPI, hr, errorDescription, "D3D11FrameBuffer::_map");
        }

        D3D11Mappings::setPixelBoxMapping(box, pMappedResource);
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::_mapstagingbuffer(D3D11_MAP flags, PixelBox &box)
    {
        if(!mStagingBuffer)
            createStagingBuffer();

        if(flags == D3D11_MAP_READ_WRITE || flags == D3D11_MAP_READ || flags == D3D11_MAP_WRITE)  
        {
            D3D11_BOX boxDx11 = getSubresourceBox(mLockedBox); // both src and dest
            UINT subresource = getSubresourceIndex(mLockedBox.front);
            mDevice.GetImmediateContext()->CopySubresourceRegion(
                mStagingBuffer.Get(), 0, boxDx11.left, boxDx11.top, boxDx11.front,
                mParentTexture->getTextureResource(), subresource, &boxDx11);
        }
        else if(flags == D3D11_MAP_WRITE_DISCARD)
            flags = D3D11_MAP_WRITE; // stagingbuffer doesn't support discarding

        _map(mStagingBuffer.Get(), flags, box);
    }
    //-----------------------------------------------------------------------------  
    PixelBox D3D11FrameBuffer::lockImpl(const Box &lockBox, LockOptions options)
    {
        // Check for misuse
        if(mUsage & TU_RENDERTARGET)
            N_EXCEPT(RenderingAPI, "DirectX does not allow locking of or directly writing to RenderTargets. Use blitFromMemory if you need the contents.");  

        mLockedBox = lockBox;

        // Set extents and format
        // Note that we do not carry over the left/top/front here, since the returned
        // PixelBox will be re-based from the locking point onwards
        PixelBox rval(lockBox.getWidth(), lockBox.getHeight(), lockBox.getDepth(), mFormat);
        // Set locking flags according to options
        D3D11_MAP  flags = D3D11_MAP_WRITE_DISCARD ;
        switch(options)
        {
        case HBL_NO_OVERWRITE:
            flags = D3D11_MAP_WRITE_NO_OVERWRITE;
            break;
        case HBL_NORMAL:
            flags = D3D11_MAP_READ_WRITE;
            break;
        case HBL_DISCARD:
            flags = D3D11_MAP_WRITE_DISCARD;
            break;
        case HBL_READ_ONLY:
            flags = D3D11_MAP_READ;
            break;
        case HBL_WRITE_ONLY:
            flags = D3D11_MAP_WRITE;
            break;
        default: 
            break;
        };

        if(mUsage == HBU_STATIC || mUsage & HBU_DYNAMIC)
        {
            if(mUsage == HBU_STATIC || options == HBL_READ_ONLY || options == HBL_NORMAL || options == HBL_WRITE_ONLY)
                _mapstagingbuffer(flags, rval);
            else
                _map(mParentTexture->getTextureResource(), flags, rval);
        }
        else
        {
            mDataForStaticUsageLock.resize(rval.getWholeSliceSize());
            rval.data = (uchar*)mDataForStaticUsageLock.data();
        }
        // save without offset
        mCurrentLock = rval;
        mCurrentLockOptions = options;

        return rval;
    }
    //-----------------------------------------------------------------------------
    void D3D11FrameBuffer::_unmap(ID3D11Resource *res)
    {
        UINT subresource = (res == mStagingBuffer.Get()) ? 0 : getSubresourceIndex(mLockedBox.front);
        mDevice.GetImmediateContext()->Unmap(res, subresource);

        if (mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(RenderingAPI, "D3D11 device unmap resource\nError Description:" + errorDescription,);
        }
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::_unmapstaticbuffer()
    {
        D3D11_BOX dstBoxDx11 = getSubresourceBox(mLockedBox);
        UINT subresource = getSubresourceIndex(mLockedBox.front);
        UINT srcRowPitch = PixelUtil::getSize(mCurrentLock.getWidth(), 1, 1, mFormat);
        UINT srcDepthPitch = PixelUtil::getSize(mCurrentLock.getWidth(), mCurrentLock.getHeight(), 1, mFormat); // H * rowPitch is invalid for compressed formats

        mDevice.GetImmediateContext()->UpdateSubresource(
                    mParentTexture->getTextureResource(), subresource, &dstBoxDx11,
                    mDataForStaticUsageLock.data(), srcRowPitch, srcDepthPitch);
        if (mDevice.isError())
        {
            String errorDescription; errorDescription
                .append("D3D11 device cannot update staging ").append(toString(mParentTexture->getTextureType()))
                .append("\nError Description:").append(mDevice.getErrorString());
            N_EXCEPT(RenderingAPI, errorDescription);
        }

        mDataForStaticUsageLock.shrink_to_fit();
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::_unmapstagingbuffer(bool copyback)
    {
        _unmap(mStagingBuffer.Get());

        if(copyback)
        {
            D3D11_BOX boxDx11 = getSubresourceBox(mLockedBox); // both src and dest
            UINT subresource = getSubresourceIndex(mLockedBox.front);

            mDevice.GetImmediateContext()->CopySubresourceRegion(
                mParentTexture->getTextureResource(), subresource, boxDx11.left, boxDx11.top, boxDx11.front,
                mStagingBuffer.Get(), 0, &boxDx11);

            mStagingBuffer.Reset();
        }
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::unlockImpl(void)
    {
        if(mUsage == HBU_STATIC)
            _unmapstagingbuffer();
        else if(mUsage & HBU_DYNAMIC)
        {
            if(mCurrentLockOptions == HBL_READ_ONLY || mCurrentLockOptions == HBL_NORMAL || mCurrentLockOptions == HBL_WRITE_ONLY)
            {
                PixelBox box;
                box.format = mFormat;
                _map(mParentTexture->getTextureResource(), D3D11_MAP_WRITE_DISCARD, box);
                void *data = box.data; 
				memcpy(data, mCurrentLock.data, mSizeInBytes);
                // unmap the texture and the staging buffer
                _unmap(mParentTexture->getTextureResource());
                _unmapstagingbuffer(false);
            }
            else
                _unmap(mParentTexture->getTextureResource());
        }
        else
            _unmapstaticbuffer();

        _genMipmaps();
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::blit(const HardwarePixelBufferSharedPtr &src, const Box &srcBox, const Box &dstBox)
    {
        if (srcBox.getWidth() != dstBox.getWidth()
            || srcBox.getHeight() != dstBox.getHeight()
            || srcBox.getDepth() != dstBox.getDepth())
        {
            N_EXCEPT(RenderingAPI, "D3D11 device cannot copy a subresource - source and dest size are not the same and they have to be the same in DX11.");
        }

        D3D11FrameBuffer * srcDx11 = static_cast<D3D11FrameBuffer *>(src.get());

        // We should blit TEX_TYPE_2D_ARRAY with depth > 1 by iterating over subresources.
        if (srcBox.getDepth() > 1 &&
            (mParentTexture->getTextureType() == TEX_TYPE_2D_ARRAY || srcDx11->mParentTexture->getTextureType() == TEX_TYPE_2D_ARRAY))
        {
            Box srcSlice = srcBox, dstSlice = dstBox;
            srcSlice.back = srcSlice.front + 1;
            dstSlice.back = dstSlice.front + 1;
            for(uint32 slice = srcBox.front; slice < srcBox.back; ++slice)
            {
                blit(src, srcSlice, dstSlice); // recursive call
                ++srcSlice.front; ++srcSlice.back;
                ++dstSlice.front; ++dstSlice.back;
            }
            return;
        }

        // Do real work without extra checking - debug layer will catch erroneous parameters.
        D3D11_BOX srcBoxDx11 = srcDx11->getSubresourceBox(srcBox);
        UINT srcSubresource = srcDx11->getSubresourceIndex(srcBox.front);
        D3D11_BOX dstBoxDx11 = getSubresourceBox(dstBox);
        UINT dstSubresource = getSubresourceIndex(dstBox.front);

        mDevice.GetImmediateContext()->CopySubresourceRegion(
            mParentTexture->getTextureResource(), dstSubresource, dstBoxDx11.left, dstBoxDx11.top, dstBoxDx11.front,
            srcDx11->mParentTexture->getTextureResource(), srcSubresource, &srcBoxDx11);

        if(mDevice.isError())
        {
            String errorDescription; errorDescription
                .append("D3D11 device cannot copy to ").append(toString(mParentTexture->getTextureType()))
                .append(" subresource region from ").append(toString(srcDx11->mParentTexture->getTextureType()))
                .append("\nError Description:").append(mDevice.getErrorString());
            N_EXCEPT(RenderingAPI, errorDescription);
        }

        _genMipmaps();
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::blitFromMemory(const PixelBox &src, const Box &dst)
    {
        if (src.getWidth() != dst.getWidth()
            || src.getHeight() != dst.getHeight()
            || src.getDepth() != dst.getDepth())
        {
            N_EXCEPT(RenderingAPI, "D3D11 device cannot copy a subresource - source and dest size are not the same and they have to be the same in DX11.");
        }

        // convert to pixelbuffer's native format if necessary
        if(src.format != mFormat)
        {
            std::vector<uint8> buffer;
            buffer.resize(PixelUtil::getSize(src.getWidth(), src.getHeight(), src.getDepth(), mFormat));
            PixelBox converted = PixelBox(src.getWidth(), src.getHeight(), src.getDepth(), mFormat, buffer.data());
            PixelUtil::bulkPixelConversion(src, converted);
            blitFromMemory(converted, dst); // recursive call
            return;
        }

        // We should blit TEX_TYPE_2D_ARRAY with depth > 1 by iterating over subresources.
        if (src.getDepth() > 1 && mParentTexture->getTextureType() == TEX_TYPE_2D_ARRAY)
        {
            PixelBox srcSlice = src;
            Box dstSlice = dst;
            srcSlice.back = srcSlice.front + 1;
            dstSlice.back = dstSlice.front + 1;
            for(uint32 slice = src.front; slice < src.back; ++slice)
            {
                blitFromMemory(srcSlice, dstSlice); // recursive call
                ++srcSlice.front; ++srcSlice.back;
                ++dstSlice.front; ++dstSlice.back;
            }
            return;
        }

        // Do the real work
        if (mUsage & HBU_DYNAMIC) // i.e. UpdateSubresource can not be used
        {
            Ogre::PixelBox locked = lock(dst, HBL_DISCARD);
            PixelUtil::bulkPixelConversion(src, locked); // compressed formats are handled using per slice granularity, pitches are honoured
            unlock();
        }
        else
        {
            D3D11_BOX dstBox = getSubresourceBox(dst);
            UINT dstSubresource = getSubresourceIndex(dst.front);
            UINT srcRowPitch = PixelUtil::getSize(src.getWidth(), 1, 1, src.format);
            UINT srcDepthPitch = PixelUtil::getSize(src.getWidth(), src.getHeight(), 1, src.format); // H * rowPitch is invalid for compressed formats

            mDevice.GetImmediateContext()->UpdateSubresource(
                mParentTexture->getTextureResource(), dstSubresource, &dstBox,
                src.getData(), srcRowPitch, srcDepthPitch);
        }

        if(mDevice.isError())
        {
            String errorDescription; errorDescription
                .append("D3D11 device cannot update ").append(toString(mParentTexture->getTextureType()))
                .append("\nError Description:").append(mDevice.getErrorString());
            N_EXCEPT(RenderingAPI, errorDescription);
        }

        _genMipmaps();
    }
    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::blitToMemory(const Box &srcBox, const PixelBox &dst)
    {
        N_assert(srcBox.getDepth() == 1 && dst.getDepth() == 1);

        //This is a pointer to the texture we're trying to copy
        //Only implemented for 2D at the moment...
        ID3D11Texture2D *texture = mParentTexture->GetTex2D();
        HRESULT hr = texture ? S_OK : E_INVALIDARG;
        mDevice.throwIfFailed(hr, "blitToMemory is implemented only for 2D textures", "D3D11FrameBuffer::blitToMemory");

        // get the description of the texture
        D3D11_TEXTURE2D_DESC desc = {0};
        texture->GetDesc( &desc );
        UINT srcSubresource = getSubresourceIndex(srcBox.front); // one face of cubemap, one item of array
        D3D11_BOX srcBoxDx11 = getSubresourceBox(srcBox);

        // MSAA content must be resolved before being copied to a staging texture
        ComPtr<ID3D11Texture2D> textureNoMSAA;
        if(desc.SampleDesc.Count == 1)
        {
            textureNoMSAA = texture;
        }
        else
        {
            // Note - we create textureNoMSAA with all mip levels of parent texture (not good), but will resolve only required one.
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;

            hr = mDevice->CreateTexture2D(&desc, NULL, textureNoMSAA.ReleaseAndGetAddressOf());
            mDevice.throwIfFailed(hr, "Error creating texture without MSAA", "D3D11FrameBuffer::blitToMemory");

            mDevice.GetImmediateContext()->ResolveSubresource(textureNoMSAA.Get(), srcSubresource, texture, srcSubresource, desc.Format);
            mDevice.throwIfFailed("Error resolving MSAA subresource", "D3D11FrameBuffer::blitToMemory");
        }

        // Create the staging texture
        ComPtr<ID3D11Texture2D> stagingTexture;
        if(desc.Usage == D3D11_USAGE_STAGING && (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ))
        {
            stagingTexture.Swap(textureNoMSAA); // Handle case where the source is already a staging texture we can use directly
        }
        else
        {
            desc.Usage = D3D11_USAGE_STAGING;
            desc.BindFlags = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
            hr = mDevice->CreateTexture2D(&desc, NULL, stagingTexture.ReleaseAndGetAddressOf());
            mDevice.throwIfFailed(hr, "Error creating staging texture", "D3D11FrameBuffer::blitToMemory");

            // Copy our texture into the staging texture
            mDevice.GetImmediateContext()->CopySubresourceRegion(
                stagingTexture.Get(), srcSubresource, srcBoxDx11.left, srcBoxDx11.top, srcBoxDx11.front,
                textureNoMSAA.Get(), srcSubresource, &srcBoxDx11);
            mDevice.throwIfFailed("Error while copying to staging texture", "D3D11FrameBuffer::blitToMemory");
        }

        // Map the subresource of the staging texture
        D3D11_MAPPED_SUBRESOURCE mapped = {0};
        hr = mDevice.GetImmediateContext()->Map(stagingTexture.Get(), srcSubresource, D3D11_MAP_READ, 0, &mapped);
        mDevice.throwIfFailed(hr, "Error while mapping staging texture", "D3D11FrameBuffer::blitToMemory");
        
        // Read the data out of the texture.
        PixelBox locked = D3D11Mappings::getPixelBoxWithMapping(srcBoxDx11, desc.Format, mapped);
        PixelUtil::bulkPixelConversion(locked, dst);

        // Release the staging texture
        mDevice.GetImmediateContext()->Unmap(stagingTexture.Get(), srcSubresource);
    }

    //-----------------------------------------------------------------------------  
    void D3D11FrameBuffer::_genMipmaps()
    {
        if(mParentTexture->HasAutoMipMapGenerationEnabled())
        {
            ID3D11ShaderResourceView * pShaderResourceView = mParentTexture->getSrvView();
            ID3D11DeviceContextN * context =  mDevice.GetImmediateContext();
            context->GenerateMips(pShaderResourceView);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, "D3D11 device cannot generate mips\nError Description:" + errorDescription);
            }   
        }

    }
    //-----------------------------------------------------------------------------    
    RenderTexture *D3D11FrameBuffer::getRenderTarget(size_t zoffset)
    {
        N_assert(mUsage & TU_RENDERTARGET);
        N_assert(zoffset < mDepth);
        return mSliceTRT[zoffset];
    }
    //-----------------------------------------------------------------------------    
    D3D11Texture * D3D11FrameBuffer::getParentTexture() const
    {
        return mParentTexture;
    }
    //-----------------------------------------------------------------------------    
    UINT D3D11FrameBuffer::getSubresourceIndex(size_t box_front) const
    {
        switch(mParentTexture->getTextureType())
        {
        case TEX_TYPE_CUBE_MAP: return D3D11CalcSubresource(mMipLevel, mFace, mParentTexture->getMipmapCount() + 1);
        case TEX_TYPE_2D_ARRAY: return D3D11CalcSubresource(mMipLevel, box_front, mParentTexture->getMipmapCount() + 1);
        }
        return mMipLevel;
    }
    //-----------------------------------------------------------------------------  
    D3D11_BOX D3D11FrameBuffer::getSubresourceBox(const Box &inBox) const
    {
        // Ogre index Tex2DArray using Z component of the box, but Direct3D expect 
        // this index to be in subresource, and Z component should be sanitized
        bool is2DArray = (mParentTexture->getTextureType() == TEX_TYPE_2D_ARRAY);

        D3D11_BOX res;
        res.left    = static_cast<UINT>(inBox.left);
        res.top     = static_cast<UINT>(inBox.top);
        res.front   = is2DArray ? 0 : static_cast<UINT>(inBox.front);
        res.right   = static_cast<UINT>(inBox.right);
        res.bottom  = static_cast<UINT>(inBox.bottom);
        res.back    = is2DArray ? 1 : static_cast<UINT>(inBox.back);
        return res;
    }
    //-----------------------------------------------------------------------------    
    UINT D3D11FrameBuffer::getFace() const
    {
        return mFace;
    }
    //-----------------------------------------------------------------------------    
    void D3D11FrameBuffer::createStagingBuffer()
    {
        D3D11Texture *tex = static_cast<D3D11Texture*>(mParentTexture);

        switch (mParentTexture->getTextureType())
        {
        case TEX_TYPE_1D:
            {
                D3D11_TEXTURE1D_DESC desc;
                tex->GetTex1D()->GetDesc(&desc);

                desc.Width     = mWidth;
                desc.MipLevels = 0;
                desc.BindFlags = 0;
                desc.MiscFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                desc.Usage = D3D11_USAGE_STAGING;

                mDevice->CreateTexture1D(&desc, NULL, (ID3D11Texture1D**)mStagingBuffer.ReleaseAndGetAddressOf());
            }                   
            break;
        case TEX_TYPE_2D:
        case TEX_TYPE_CUBE_MAP:
        case TEX_TYPE_2D_ARRAY:
            {
                D3D11_TEXTURE2D_DESC desc;
                tex->GetTex2D()->GetDesc(&desc);

                desc.Width     = mWidth;
                desc.Height    = mHeight;
                desc.MipLevels = 0;
                desc.BindFlags = 0;
                desc.MiscFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                desc.Usage = D3D11_USAGE_STAGING;

                mDevice->CreateTexture2D(&desc, NULL, (ID3D11Texture2D**)mStagingBuffer.ReleaseAndGetAddressOf());
            }
            break;
        case TEX_TYPE_3D:
            {
                D3D11_TEXTURE3D_DESC desc;
                tex->GetTex3D()->GetDesc(&desc);

                desc.Width     = mWidth;
                desc.Height    = mHeight;
                desc.Depth     = mDepth;
                desc.MipLevels = 0;
                desc.BindFlags = 0;
                desc.MiscFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                desc.Usage = D3D11_USAGE_STAGING;

                mDevice->CreateTexture3D(&desc, NULL, (ID3D11Texture3D**)mStagingBuffer.ReleaseAndGetAddressOf());
            }
            break;
        }
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // D3D11StructBuffer
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    D3D11StructBuffer::D3D11StructBuffer( size_t numElements, uint32 bytesPerElement, uint32 numElementsPadding, 
        BufferModeMark bufferType, void * initialData, bool keepAsShadow,
            GBufferManager * mag, BufferOp * op, D3D11Device & device ) :
        StructBuffer( 0, numElements, bytesPerElement, numElementsPadding, bufferType, initialData, keepAsShadow, mag, op ),
        mDevice( device )
    {
    }
    //-----------------------------------------------------------------------------------
    D3D11StructBuffer::~D3D11StructBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void D3D11StructBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        switch(type)
        {
        case ST_VS :
            N_assert( dynamic_cast<D3D11CompatBufferInterface *>(mOp));

            D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>(mOp);

            ID3D11Buffer *d3dBuffer = op->getVboName();
            mDevice.GetImmediateContext()->VSSetConstantBuffers(slot, 1, &d3dBuffer);
            break;
        case ST_TS :
            N_assert( dynamic_cast<D3D11CompatBufferInterface*>( mOp ) );

            D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>(mOp);

            ID3D11Buffer *d3dBuffer = op->getVboName();
            mDevice.GetImmediateContext()->HSSetConstantBuffers( slot, 1, &d3dBuffer );
        case ST_DS :
            N_assert( dynamic_cast<D3D11CompatBufferInterface*>( mOp ) );

            D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>(mOp);

            ID3D11Buffer *d3dBuffer = op->getVboName();
            mDevice.GetImmediateContext()->DSSetConstantBuffers( slot, 1, &d3dBuffer );
        case ST_GS :
            N_assert( dynamic_cast<D3D11CompatBufferInterface *>(mOp));

            D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>(mOp);

            ID3D11Buffer *d3dBuffer = op->getVboName();
            mDevice.GetImmediateContext()->GSSetConstantBuffers(slot, 1, &d3dBuffer);
            break;
        case ST_FS :
            N_assert( dynamic_cast<D3D11CompatBufferInterface *>(mOp));

            D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>(mOp);

            ID3D11Buffer *d3dBuffer = op->getVboName();
            mDevice.GetImmediateContext()->PSSetConstantBuffers(slot, 1, &d3dBuffer);
            break;
        case ST_CS :
            N_assert( dynamic_cast<D3D11CompatBufferInterface*>( mOp ) );

            D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>(mOp);

            ID3D11Buffer *d3dBuffer = op->getVboName();
            mDevice.GetImmediateContext()->CSSetConstantBuffers( slot, 1, &d3dBuffer );
            break;
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11TextureBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11TextureBuffer::D3D11TextureBuffer(
            size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
            uint32 numElementsPadding, BufferModeMark bufferType, void * initialData, bool keepAsShadow,
            GBufferManager * vaoManager, BufferOp * op, PixelFormat pf, bool bIsStructured, D3D11Device &device ) :
        TextureBuffer( internalBufStartBytes, numElements, bytesPerElement, numElementsPadding,
                         bufferType, initialData, keepAsShadow, vaoManager, op, pf ),
        mInternalFormat( DXGI_FORMAT_UNKNOWN ),
        mDevice( device ),
        mCurrentCacheCursor( 0 )
    {
        memset( mCachedResourceViews, 0, sizeof( mCachedResourceViews ) );

        if( !bIsStructured )
            mInternalFormat = D3D11Mappings::get( pf );
    }
    //-----------------------------------------------------------------------------------
    D3D11TextureBuffer::~D3D11TextureBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        switch(type)
        {
        case ST_VS :
            ID3D11ShaderResourceView *resourceView = bindBufferCommon( oft, size );
            ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
            deviceContext->VSSetShaderResources( slot, 1, &resourceView );
            break;
        case ST_TS :
            ID3D11ShaderResourceView *resourceView = bindBufferCommon( oft, size );
            ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
            deviceContext->HSSetShaderResources( slot, 1, &resourceView );
        case ST_DS :
            ID3D11ShaderResourceView *resourceView = bindBufferCommon( oft, size );
            ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
            deviceContext->DSSetShaderResources( slot, 1, &resourceView );
            break;
        case ST_GS :
            ID3D11ShaderResourceView *resourceView = bindBufferCommon( oft, size );
            ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
            deviceContext->GSSetShaderResources( slot, 1, &resourceView );
            break;
        case ST_FS :
            ID3D11ShaderResourceView *resourceView = bindBufferCommon( oft, size );
            ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
            deviceContext->PSSetShaderResources( slot, 1, &resourceView );
            break;
        case ST_CS :
            ID3D11ShaderResourceView *resourceView = bindBufferCommon( oft, size );
            ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
            deviceContext->CSSetShaderResources( slot, 1, &resourceView );
            break;
        default:
            break;
        }
    }
    //-----------------------------------------------------------------------------------
    bool D3D11TextureBuffer::isD3D11Structured(void) const
    {
        return mInternalFormat == DXGI_FORMAT_UNKNOWN;
    }
    //-----------------------------------------------------------------------------------
    ID3D11ShaderResourceView * D3D11TextureBuffer::createResourceView(int cacheIdx, uint32 offset, uint32 size)
    {
        N_assert( cacheIdx < 16 );

        const size_t formatSize = isD3D11Structured() ? mUnitSize : PixelUtil::getUnitSize(mPixelFormat);

        mCachedResourceViews[cacheIdx].mResourceView.Reset();

        mCachedResourceViews[cacheIdx].mOffset = static_cast<uint32>(mCurrentMulti * mReserveSize + offset);
        mCachedResourceViews[cacheIdx].mSize = size;

        D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;

        srDesc.Format = mInternalFormat;
        srDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srDesc.Buffer.FirstElement = static_cast<UINT>((mCurrentMulti * mReserveSize + offset) / formatSize);
        srDesc.Buffer.NumElements = size / formatSize;

        ID3D11Buffer * vboName = 0;

        D3D11BufferOpBase * op = static_cast<D3D11BufferOpBase *>(mOp);
        vboName = op->getVboName();

        mDevice.get()->CreateShaderResourceView(vboName, &srDesc, mCachedResourceViews[cacheIdx].mResourceView.ReleaseAndGetAddressOf());

        mCurrentCacheCursor = (cacheIdx + 1) % 16;

        return mCachedResourceViews[cacheIdx].mResourceView.Get();
    }
    //-----------------------------------------------------------------------------------
    ID3D11ShaderResourceView * D3D11TextureBuffer::bindBufferCommon( size_t offset, size_t size )
    {
        N_assert( offset <= getSize() );
        N_assert( size <= getSize() );
        N_assert( (offset + size) <= getSize() );

        size = !size ? (mUnitCount * mUnitSize - offset) : size;

        ID3D11ShaderResourceView * resourceView = 0;
        for(int i = 0; i < 16; ++i)
        {
            //Reuse resource views. Reuse res. views that are bigger than what's requested too.
            if( mCurrentMulti * mReserveSize + offset == mCachedResourceViews[i].mOffset && size <= mCachedResourceViews[i].mSize )
            {
                resourceView = mCachedResourceViews[i].mResourceView.Get();
                break;
            }
            else if( !mCachedResourceViews[i].mResourceView )
            {
                //We create in-order. If we hit here, the next ones are also null pointers.
                resourceView = createResourceView( i, offset, size );
                break;
            }
        }

        if( !resourceView )
        {
            //If we hit here, the cache is full and couldn't find a match.
            resourceView = createResourceView( mCurrentCacheCursor, offset, size );
        }

        return resourceView;
    }
    //-----------------------------------------------------------------------------------
    ComPtr<ID3D11ShaderResourceView> D3D11TextureBuffer::createSrv(const ShaderChTexture::BufferSlot &bufferSlot ) const
    {
        N_assert( bufferSlot.offset <= getSize() );
        N_assert( bufferSlot.sizeBytes <= getSize() );
        N_assert( (bufferSlot.offset + bufferSlot.sizeBytes) <= getSize() );

        const size_t formatSize = isD3D11Structured() ? mUnitSize : PixelUtil::getUnitSize( mPixelFormat );

        const size_t size = !bufferSlot.sizeBytes ? (mUnitCount * mUnitSize - bufferSlot.offset) : bufferSlot.sizeBytes;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        memset( &srvDesc, 0, sizeof( srvDesc ) );

        srvDesc.Format               = mInternalFormat;
        srvDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement  = static_cast<UINT>((mCurrentMulti * mReserveSize + bufferSlot.offset) / formatSize);
        srvDesc.Buffer.NumElements   = static_cast<UINT>(size / formatSize);

        D3D11BufferOpBase *op = static_cast<D3D11BufferOpBase*>( mOp );
        ID3D11Buffer *vboName = op->getVboName();

        ComPtr<ID3D11ShaderResourceView> retVal;
        HRESULT hr = mDevice->CreateShaderResourceView( vboName, &srvDesc, retVal.GetAddressOf() );

        if( FAILED(hr) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT_EX( RenderingAPI, hr, "Failed to create SRV view on buffer. \nError Description: " + errorDescription );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11StorageBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11StorageBuffer::D3D11StorageBuffer(size_t internalBufStartBytes, size_t numElements, uint32 bytesPerElement,
            uint32 bindFlags, void *initialData, bool keepAsShadow, GBufferManager *vaoManager, BufferOp *op, D3D11Device &device ) :
        StorageBuffer( internalBufStartBytes, numElements, bytesPerElement, bindFlags,
                         initialData, keepAsShadow, vaoManager, op ),
        mDevice( device ),
        mCurrentCacheCursor( 0 )
    {
        memset( mCachedResourceViews, 0, sizeof( mCachedResourceViews ) );
    }
    //-----------------------------------------------------------------------------------
    D3D11StorageBuffer::~D3D11StorageBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void D3D11StorageBuffer::bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size)
    {
        switch(type)
        {
        case ST_CS :
            ID3D11UnorderedAccessView *resourceView = _bindBufferCommon( oft, size );
            ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
            deviceContext->CSSetUnorderedAccessViews( slot, 1, &resourceView, 0 );
            break;
        default:
            break;
        }
        
    //    void bindBufferVS( uint16 slot, size_t offset, size_t sizeBytes )
    //    {
    //        ID3D11UnorderedAccessView *resourceView = _bindBufferCommon( offset, sizeBytes );
    //        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
    //        deviceContext->VSSetShaderResources( slot, 1, &resourceView );
    //    }
    //    //-----------------------------------------------------------------------------------
    //    void bindBufferPS( uint16 slot, size_t offset, size_t sizeBytes )
    //    {
    //        ID3D11UnorderedAccessView *resourceView = _bindBufferCommon( offset, sizeBytes );
    //        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
    //        deviceContext->PSSetShaderResources( slot, 1, &resourceView );
    //    }
    //    //-----------------------------------------------------------------------------------
    //    void bindBufferGS( uint16 slot, size_t offset, size_t sizeBytes )
    //    {
    //        ID3D11UnorderedAccessView *resourceView = _bindBufferCommon( offset, sizeBytes );
    //        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
    //        deviceContext->GSSetShaderResources( slot, 1, &resourceView );
    //    }
    //    //-----------------------------------------------------------------------------------
    //    void bindBufferHS( uint16 slot, size_t offset, size_t sizeBytes )
    //    {
    //        ID3D11UnorderedAccessView *resourceView = _bindBufferCommon( offset, sizeBytes );
    //        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
    //        deviceContext->HSSetShaderResources( slot, 1, &resourceView );
    //    }
    //    //-----------------------------------------------------------------------------------
    //    void bindBufferDS( uint16 slot, size_t offset, size_t sizeBytes )
    //    {
    //        ID3D11UnorderedAccessView *resourceView = _bindBufferCommon( offset, sizeBytes );
    //        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();
    //        deviceContext->DSSetShaderResources( slot, 1, &resourceView );
    //    }
    }
    //-----------------------------------------------------------------------------------
    TextureBuffer* D3D11StorageBuffer::createViewImpl( PixelFormat pf )
    {
        N_assert( dynamic_cast<D3D11CompatBufferInterface*>( mOp ) );

        D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>( mOp );


        TextureBuffer * re = N_new D3D11TextureBuffer(mOffset * mUnitSize, mUnitCount, mUnitSize, 0,
            mMark, (void*)0, false, (GBufferManager*)0,  pf, true, mDevice );
        //We were overriden by the GpuBuffer we just created. Restore this back!
        re->setOperation( op );

        mViewList.push_back( re );

        return re;
    }
    //-----------------------------------------------------------------------------------
    ID3D11UnorderedAccessView * D3D11StorageBuffer::createResourceView( int cacheIdx, uint32 offset, uint32 size )
    {
        N_assert( cacheIdx < 16 );

        mCachedResourceViews[cacheIdx].mResourceView.Reset();

        mCachedResourceViews[cacheIdx].mOffset  = mCurrentMulti * mReserveSize + offset;
        mCachedResourceViews[cacheIdx].mSize    = size;

        D3D11_UNORDERED_ACCESS_VIEW_DESC srDesc;

        // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        srDesc.Format               = DXGI_FORMAT_UNKNOWN;
        srDesc.ViewDimension        = D3D11_UAV_DIMENSION_BUFFER;
        srDesc.Buffer.FirstElement  = (mCurrentMulti * mReserveSize + offset) / mUnitSize;
        srDesc.Buffer.NumElements   = size / mUnitSize;
        srDesc.Buffer.Flags         = 0;

        N_assert(dynamic_cast<D3D11CompatBufferInterface *>(mOp));
        D3D11CompatBufferInterface * op = static_cast<D3D11CompatBufferInterface*>(mOp);
        ID3D11Buffer * vboName = op->getVboName();

        mDevice.get()->CreateUnorderedAccessView( vboName, &srDesc, mCachedResourceViews[cacheIdx].mResourceView.ReleaseAndGetAddressOf() );
        mCurrentCacheCursor = (cacheIdx + 1) % 16;

        return mCachedResourceViews[cacheIdx].mResourceView.Get();
    }
    //-----------------------------------------------------------------------------------
    ID3D11UnorderedAccessView * D3D11StorageBuffer::_bindBufferCommon( size_t offset, size_t size )
    {
        N_assert( offset <= getSize() );
        N_assert( size <= getSize() );
        N_assert( (offset + size) <= getSize() );

        size = !size ? (mUnitCount * mUnitSize - offset) : size;

        ID3D11UnorderedAccessView *resourceView = 0;
        for( int i=0; i<16; ++i )
        {
            //Reuse resource views. Reuse res. views that are bigger than what's requested too.
            if( mCurrentMulti * mReserveSize + offset == mCachedResourceViews[i].mOffset && size <= mCachedResourceViews[i].mSize )
            {
                resourceView = mCachedResourceViews[i].mResourceView.Get();
                break;
            }
            else if( !mCachedResourceViews[i].mResourceView )
            {
                //We create in-order. If we hit here, the next ones are also null pointers.
                resourceView = createResourceView( i, offset, size );
                break;
            }
        }

        if( !resourceView )
        {
            //If we hit here, the cache is full and couldn't find a match.
            resourceView = createResourceView( mCurrentCacheCursor, offset, size );
        }

        return resourceView;
    }
    //-----------------------------------------------------------------------------------
    ComPtr<ID3D11UnorderedAccessView> D3D11StorageBuffer::createUav(const DescriptorSetUav::BufferSlot & bufferSlot) const
    {
        N_assert( bufferSlot.offset <= getSize() );
        N_assert( bufferSlot.sizeBytes <= getSize() );
        N_assert( (bufferSlot.offset + bufferSlot.sizeBytes) <= getSize() );

        const size_t size = !bufferSlot.sizeBytes ? (mUnitCount * mUnitSize - bufferSlot.offset) : bufferSlot.sizeBytes;

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        memset( &uavDesc, 0, sizeof( uavDesc ) );

        // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        uavDesc.Format               = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension        = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement  = (mCurrentMulti * mReserveSize + bufferSlot.offset) / mUnitSize;
        uavDesc.Buffer.NumElements   = size / mUnitSize;
        uavDesc.Buffer.Flags         = 0;

        N_assert( dynamic_cast<D3D11CompatBufferInterface*>( mOp ) );
        D3D11CompatBufferInterface *op = static_cast<D3D11CompatBufferInterface*>(
                    mOp );
        ID3D11Buffer *vboName = op->getVboName();

        ComPtr<ID3D11UnorderedAccessView> retVal;
        HRESULT hr = mDevice->CreateUnorderedAccessView( vboName, &uavDesc, retVal.GetAddressOf() );
        if( FAILED(hr) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT_EX( RenderingAPI, hr,
                            "Failed to create UAV view on buffer."
                            "\nError Description: " + errorDescription,
                            "D3D11StorageBuffer::createUav" );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11MappedBuffer
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11MappedBuffer::D3D11MappedBuffer( size_t sizeBytes, GBufferManager *vaoManager, bool uploadOnly,
            ID3D11Buffer * stagingBuffer, D3D11Device &device ) :
        MappedBuffer( 0 /*No internalBufferStart in D3D11 (should be implemented in D3D12)*/,
                       sizeBytes, vaoManager, uploadOnly ),
        mVboName( stagingBuffer ),
        mMappedPtr( 0 ),
        mDevice( device )
    {
    }
    //-----------------------------------------------------------------------------------
    D3D11MappedBuffer::~D3D11MappedBuffer()
    {
    }
    //-----------------------------------------------------------------------------------
    void* D3D11MappedBuffer::writeMapImpl( NCount sizeBytes )
    {
        //N_assert( mLastOp );

        D3D11_MAP mapFlag = D3D11_MAP_WRITE_NO_OVERWRITE;

        if( mMappingStart + sizeBytes > mSizeBytes )
            mMappingStart = 0;

        if( mMappingStart == 0 )
            mapFlag = D3D11_MAP_WRITE_DISCARD;

        mMappingCount = sizeBytes;

        D3D11_MAPPED_SUBRESOURCE mappedSubres;
        mDevice.GetImmediateContext()->Map(mVboName.Get(), 0, mapFlag, 0, &mappedSubres);
        mMappedPtr = reinterpret_cast<uint8 *>(mappedSubres.pData) + mMappingStart;

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    void D3D11MappedBuffer::unmapImpl()
    {
        ID3D11DeviceContextN * d3dContext = mDevice.GetImmediateContext();
        d3dContext->Unmap( mVboName.Get(), 0 );
        mMappedPtr = 0;
        
        if(mLastOp == BOT_Read)
        {
            recover(mMappingStart, mMappingCount );
        }

        //We must wrap exactly to zero so that next map uses DISCARD.
        if( mMappingStart + mMappingCount > mSizeBytes )
            mMappingStart = 0;

    }
    //-----------------------------------------------------------------------------------
    BlockType D3D11MappedBuffer::request( size_t sizeBytes, bool upload )
    {
        if(upload)
        {
            //N_assert( mLastOp );

            size_t mappingStart = mMappingStart;

            BlockType retVal = BT_None;

            //We don't really know, it's inaccurate. But don't trust the driver
            //will be able to run a stall-free discard every time; thus BT_Part
            if( mappingStart + sizeBytes > mSizeBytes )
            {
                retVal = BT_Part;
                mappingStart = 0;
            }

            return retVal;
        }
        else
        {
            return MappedBuffer::request(sizeBytes, upload);
        }
        return BT_All;
    }
    //-----------------------------------------------------------------------------------
    size_t D3D11MappedBuffer::async( OperationList & oplist, bool upload)
    {
        if(upload)
        {
            OperationList::iterator i, iend = oplist.end();
            for(i = oplist.begin(); i != iend; ++i)
            {
                const Operation & dst = *i;
                const GpuBuffer * dest = dst.mDest;

                D3D11BufferOpBase * op = static_cast<D3D11BufferOpBase*>(dest->getOperation() );

                N_assert( dest->mMark == M_DEVNORMAL );

                UINT dstOffset = static_cast<UINT>( dst.mDestOffset + dest->getOffset() * dest->getUnitSize() );

                D3D11_BOX srcBox;
                ZeroMemory( &srcBox, sizeof(D3D11_BOX) );
                srcBox.left     = mMappingStart + dst.mOffset;
                srcBox.right    = mMappingStart + dst.mOffset + dst.mSize;
                srcBox.back     = 1;
                srcBox.bottom   = 1;

                d3dContext->CopySubresourceRegion( op->getVboName(), 0,
                                                   dstOffset, 0, 0, mVboName.Get(), 0, &srcBox );
            }
            mLastUploadOffset = mMappingStart;
        }
        else
        {
            OperationList::iterator i, iend = oplist.end();
            for(i = oplist.begin(); i != iend; ++i)
            {
                Operation & src = *i;
                GpuBuffer * source = dst.mDest;
                size_t freeRegionOffset = requestImpl( src.mSize );

                if( freeRegionOffset == -1 )
                {
                    N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                                 "Cannot download the request amount of " +
                                 StrConv::conv( src.mSize ) + " bytes to this staging buffer. "
                                 "Try another one (we're full of requests that haven't been read by CPU yet)" );
                }

                //N_assert( !mLastOp );
                N_assert( dynamic_cast<D3D11BufferOpBase*>( source->getOperation() ) );
                N_assert( (src.mDestOffset + src.mSize) <= source->getSize() );

                D3D11BufferOpBase *op = static_cast<D3D11BufferOpBase*>(source->getOperation() );

                D3D11_BOX srcBox;
                ZeroMemory( &srcBox, sizeof(D3D11_BOX) );
                srcBox.left     = source->getCurrentOffset() * source->getUnitSize() + src.mDestOffset;
                srcBox.right    = srcBox.left + src.mSize;
                srcBox.back     = 1;
                srcBox.bottom   = 1;

                ID3D11DeviceContextN *d3dContext = mDevice.GetImmediateContext();
                d3dContext->CopySubresourceRegion( mVboName.Get(), 0, freeRegionOffset, 0, 0, op->getVboName(), 0, &srcBox );

                src.mOffset = freeRegionOffset;
            }

        }
        return 0;
    }
    //-----------------------------------------------------------------------------------
    const void* D3D11MappedBuffer::readMapImpl( NCount offset, NCount sizeBytes )
    {
        //N_assert( !mLastOp );

        mMappingStart = offset;
        mMappingCount = sizeBytes;

        D3D11_MAPPED_SUBRESOURCE mappedSubres;
        mDevice.GetImmediateContext()->Map( mVboName.Get(), 0, D3D11_MAP_READ, 0, &mappedSubres );
        mMappedPtr = reinterpret_cast<uint8*>( mappedSubres.pData ) + mMappingStart;

        //Put the mapped region back to our records as "available" for subsequent async
        //recover( offset, sizeBytes );

        return mMappedPtr;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11MappedTexture
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11MappedTexture::D3D11MappedTexture( GBufferManager *vaoManager, PixelFormat formatFamily,
                                              uint32 width, uint32 height, uint32 depthOrSlices, D3D11Device &device ) :
        MappedTexture( vaoManager, formatFamily ),
        mWidth( width ),
        mHeight( height ),
        mDepthOrSlices( depthOrSlices ),
        mIsArray2DTexture( false ),
        mDevice( device )
    {
        HRESULT hr;
        if( mDepthOrSlices > 1u &&
            mWidth <= D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
            mHeight <= D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION )
        {
            D3D11_TEXTURE3D_DESC desc;
            memset( &desc, 0, sizeof( desc ) );

            desc.Width      = static_cast<UINT>( mWidth );
            desc.Height     = static_cast<UINT>( mHeight );
            desc.Depth      = static_cast<UINT>( mDepthOrSlices );
            desc.MipLevels  = 1u;
            desc.Format     = D3D11Mappings::getFamily( mFormat );
            desc.Usage              = D3D11_USAGE_STAGING;
            desc.BindFlags          = 0;
            desc.CPUAccessFlags     = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags          = 0;

            if( PixelUtil::isCompress( mFormat ) )
            {
                const uint32 blockWidth = PixelUtil::getBlockWidth( mFormat, false );
                const uint32 blockHeight = PixelUtil::getBlockHeight( mFormat, false );
                desc.Width  = std::max( desc.Width, blockWidth );
                desc.Height = std::max( desc.Height, blockHeight );
            }

            ID3D11Texture3D *texture = 0;
            hr = mDevice->CreateTexture3D( &desc, 0, &texture );
            mStagingTexture.Attach( texture );
            mIsArray2DTexture = false;

            mSubresourceData.resize( 1u );
            mLastSubresourceData.resize( 1u );
            memset( &mSubresourceData[0], 0, sizeof( D3D11_MAPPED_SUBRESOURCE ) );
            memset( &mLastSubresourceData[0], 0, sizeof( D3D11_MAPPED_SUBRESOURCE ) );
        }
        else
        {
            D3D11_TEXTURE2D_DESC desc;
            memset( &desc, 0, sizeof( desc ) );

            desc.Width      = static_cast<UINT>( mWidth );
            desc.Height     = static_cast<UINT>( mHeight );
            desc.MipLevels  = 1u;
            desc.ArraySize  = static_cast<UINT>( mDepthOrSlices );
            desc.Format     = D3D11Mappings::getFamily( mFormat );
            desc.SampleDesc.Count   = 1u;
            desc.Usage              = D3D11_USAGE_STAGING;
            desc.BindFlags          = 0;
            desc.CPUAccessFlags     = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags          = 0;

            if( PixelUtil::isCompress( mFormat ) )
            {
                const uint32 blockWidth = PixelUtil::getBlockWidth( mFormat, false );
                const uint32 blockHeight = PixelUtil::getBlockHeight( mFormat, false );
                desc.Width  = std::max( desc.Width, blockWidth );
                desc.Height = std::max( desc.Height, blockHeight );
            }

            ID3D11Texture2D *texture = 0;
            hr = mDevice->CreateTexture2D( &desc, 0, &texture );
            mStagingTexture.Attach( texture );
            mIsArray2DTexture = true;

            mSubresourceData.resize( mDepthOrSlices );
            mLastSubresourceData.resize( mDepthOrSlices );
            for( size_t i=0; i<mDepthOrSlices; ++i )
            {
                memset( &mSubresourceData[i], 0, sizeof( D3D11_MAPPED_SUBRESOURCE ) );
                memset( &mLastSubresourceData[i], 0, sizeof( D3D11_MAPPED_SUBRESOURCE ) );
            }
        }

        mSize = PixelUtil::getSize(mWidth, mHeight, mDepthOrSlices, 1u, mFormat, 4u );
        
        if( FAILED(hr) || mDevice.isError() )
        {
            String errorDescription = mDevice.getErrorString( hr );
            N_EXCEPT_EX( RenderingAPI, hr, "Error creating MappedTexture\nError Description:" + errorDescription );
        }
    }
    //-----------------------------------------------------------------------------------
    D3D11MappedTexture::~D3D11MappedTexture()
    {
        if( mMapping )
        {
            LogManager::getSingleton().logMessage(
                        "WARNING: D3D11MappedTexture being deleted did not call unmap. Calling it for you.", LML_CRITICAL );
            unmap();
        }
    }
    //-----------------------------------------------------------------------------------
    bool D3D11MappedTexture::request( const PixelVolume & vol ) const
    {
        if( mFormat != PixelUtil::getFamily( vol.mFormat ) )
            return false;

        if( vol.mWidth > mWidth )
            return false;
        if( vol.mHeight > mHeight )
            return false;

        //Don't allow copying from TextureArray to 3D volume textures.
        //It may be possible but complicates D3D11MappedTexture::upload a lot
        //(also complicates mapImpl)
        if( vol.mDepth > 1u && mIsArray2DTexture )
            return false;

        if( vol.mDepth * vol.mArray > mDepthOrSlices )
            return false;

        return true;
    }
    //-----------------------------------------------------------------------------------
    uint32 D3D11MappedTexture::findRealSlice( void *data ) const
    {
        const size_t mArray = mLastSubresourceData.size();
        for( size_t slice=0; slice<mArray; ++slice )
        {
            if( data == mLastSubresourceData[slice].pData )
                return slice;
        }

        return -1;
    }
    //-----------------------------------------------------------------------------------
    bool D3D11MappedTexture::belongsToUs( const PixelBlock &box )
    {
        if( box.mArray > 1u && mIsArray2DTexture )
            return false;

        if( box.mLeft + box.mWidth > mWidth ||
            box.mTop + box.mHeight > mHeight ||
            box.getVolumeIdex() + box.getVolume() > mDepthOrSlices )
        {
            return false;
        }

        if( !mIsArray2DTexture )
            return box.data == mLastSubresourceData[0].pData;

        const uint32 realSlice = findRealSlice( box.data );

        return realSlice < mLastSubresourceData.size();
    }
    //-----------------------------------------------------------------------------------
    void D3D11MappedTexture::shrinkRecords( size_t slice, StagingBoxVec::iterator record, PixelBlock consumedBox )
    {
        const bool isCompress = PixelUtil::isCompress( mFormat );
        uint32 blockWidth = 1u;
        uint32 blockHeight = 1u;

        bool canShrink = true;

        if( isCompress )
        {
            //Always consume the whole block for compressed formats.
            blockWidth = PixelUtil::getBlockWidth( mFormat, false );
            blockHeight= PixelUtil::getBlockHeight( mFormat, false );
            consumedBox.width   = multiCeil( consumedBox.width, blockWidth );
            consumedBox.height  = multiCeil( consumedBox.height, blockHeight );

            // This MappedTexture is theoretically smaller than the minimum block size and we only
            // rounded it upwards to satisfy D3D11. We can't shrink this further.
            // There can and should only be one record.
            if( mWidth < blockWidth || mHeight < blockHeight )
            {
                N_assert( mFreeBoxes.size() == 1u );
                canShrink = false;
            }
        }

        if( ( record->width == consumedBox.width && record->height == consumedBox.height ) ||
            !canShrink )
        {
            //Whole record was consumed. Easy case.
            remove( mFreeBoxes[slice], record );
            return;
        }

        if( consumedBox.width <= record->width >> 1u &&
            consumedBox.height <= record->height >> 1u &&
            (!isCompress ||
             (isCompress &&
              (record->width >> 1u) % blockWidth == 0u &&
              (record->height >> 1u) % blockHeight == 0u) ) )
        {
            //If what was consumed is too small, partition the records in 4 fragments,
            //which should maximize our free space (assuming most textures are powers of 2).
            record->width >>= 1u;
            record->height>>= 1u;

            StagingBox newRecord[3];
            newRecord[0] = *record;
            newRecord[1] = *record;
            newRecord[2] = *record;

            newRecord[0].x += newRecord[0].width;
            newRecord[1].y += newRecord[1].height;
            newRecord[2].x += newRecord[2].width;
            newRecord[2].y += newRecord[2].height;

            const size_t idx = record - mFreeBoxes[slice].begin();
            mFreeBoxes[slice].push_back( newRecord[0] );
            mFreeBoxes[slice].push_back( newRecord[1] );
            mFreeBoxes[slice].push_back( newRecord[2] );
            record = mFreeBoxes[slice].begin() + idx;
        }

        //We need to split the record into 3 parts (2 parts are free, 1 is consumed).
        //To do that, we'll overwrite the existing record, and create one more.
        // If this is the record (origin is at upper left corner in this graph):
        //  -----------------
        //  |               |
        //  |               |
        //  |               |
        //  |               |
        //  |-              |
        //  |R|             |
        //  -----------------
        //Then we slice it like this:
        //  -----------------
        //  | |             |
        //  | |             |
        //  |0|      1      |
        //  | |             |
        //  |-|             |
        //  |R|             |
        //  -----------------

        // New slice #0
        StagingBox originalRecord = *record;
        record->width   = consumedBox.width;
        record->y       = consumedBox.y + consumedBox.height;
        record->height  = originalRecord.height - consumedBox.height;
        if( record->height == 0 )
            remove( mFreeBoxes[slice], record );

        // New slice #1
        StagingBox newRecord = originalRecord;
        newRecord.x     = consumedBox.x + consumedBox.width;
        newRecord.width = originalRecord.width - consumedBox.width;
        if( newRecord.width > 0 )
            mFreeBoxes[slice].push_back( newRecord );
    }
    //-----------------------------------------------------------------------------------
    void D3D11MappedTexture::shrinkMultisliceRecords( size_t slice, StagingBoxVec::iterator record,
                                                       const PixelBlock & consumedBox )
    {
        if( record->x == consumedBox.x && record->y == consumedBox.y )
        {
            //Trivial case. Consumed box is at the origin of the record.
            shrinkRecords( slice, record, consumedBox );
            return;
        }

        //The consumed box isn't at the origin of the record. It's somewhere in the middle.
        //Therefore we need to divide the record in 4, and leave a hole in the middle.
        //If this is the record (origin is at upper left corner in this graph):
        //  -----------------
        //  |               |
        //  |     -         |
        //  |    |R|        |
        //  |     -         |
        //  |               |
        //  |               |
        //  -----------------
        //Then we slice it like this:
        //  -----------------
        //  |       0       |
        //  |---------------|
        //  | 1  |R|   2    |
        //  |---------------|
        //  |       3       |
        //  |               |
        //  -----------------

        // New slice #0
        StagingBox originalRecord = *record;
        record->height = consumedBox.y;
        if( record->height == 0 )
            remove( mFreeBoxes[slice], record );

        StagingBox newRecord;

        // New slice #1
        newRecord = originalRecord;
        newRecord.width     = consumedBox.x;
        newRecord.y         = consumedBox.y;
        newRecord.height    = consumedBox.height;
        if( newRecord.width > 0 )
            mFreeBoxes[slice].push_back( newRecord );

        // New slice #2
        newRecord = originalRecord;
        newRecord.x         = consumedBox.x + consumedBox.width;
        newRecord.width     = originalRecord.width - newRecord.x;
        newRecord.y         = consumedBox.y;
        newRecord.height    = consumedBox.height;
        if( newRecord.width > 0 )
            mFreeBoxes[slice].push_back( newRecord );

        // New slice #3
        newRecord = originalRecord;
        newRecord.y         = consumedBox.y + consumedBox.height;
        newRecord.height    = originalRecord.height - newRecord.y;
        if( newRecord.height > 0 )
            mFreeBoxes[slice].push_back( newRecord );
    }
    //-----------------------------------------------------------------------------------
    PixelBlock D3D11MappedTexture::mapMultipleSlices( const PixelVolume & vol )
    {
        //By definition, we can't map multiple vol.mAarray if this is an array texture
        N_assert( !mIsArray2DTexture );

        const uint32 depthOrSlices = std::max( vol.mDepth, vol.mAarray );

        vector<StagingBoxVec::iterator>::type selectedRecords;
        selectedRecords.reserve( depthOrSlices ); //One record per slice

        //Walk backwards, to start with less fragmented vol.mAarray first.
        const size_t mArray = mFreeBoxes.size();
        for( size_t slice = mArray; slice--;)
        {
            StagingBoxVec &freeBoxesInSlice = mFreeBoxes[slice];

            StagingBoxVec::iterator itor = freeBoxesInSlice.begin();
            StagingBoxVec::iterator end  = freeBoxesInSlice.end();

            while( itor != end )
            {
                //There are too many possible candidates. Just grab any free region, and see if
                //this region is also available in the consecutive vol.mAarray (backwards or forwards).
                //This algorithm may not find any candidate even if it is still possible.
                //An exhaustive search would be too expensive.
                if( vol.mWidth <= itor->width && vol.mHeight <= itor->height )
                {
                    size_t numBackwardSlices = 0;
                    size_t numForwardSlices = 0;

                    size_t lastNumSlices = 1u;

                    //Now check how many vol.mAarray backwards & forwards can also
                    //contain this 3D region at the same location.
                    //Walk backwards
                    for( size_t extraSlice = slice;
                         extraSlice-- && lastNumSlices != numBackwardSlices &&
                         (numBackwardSlices + numForwardSlices) < depthOrSlices - 1u; )
                    {
                        lastNumSlices = numBackwardSlices;

                        StagingBoxVec & freeBoxesInExtraSlice = mFreeBoxes[extraSlice];
                        StagingBoxVec::iterator it = freeBoxesInExtraSlice.begin();
                        StagingBoxVec::iterator en = freeBoxesInExtraSlice.end();

                        while( it != en )
                        {
                            if( it->contains( itor->x, itor->y, vol.mWidth, vol.mHeight ) )
                            {
                                selectedRecords.push_back( it );
                                ++numBackwardSlices;
                                break;
                            }
                            ++it;
                        }
                    }

                    std::reverse( selectedRecords.begin(), selectedRecords.end() );
                    selectedRecords.push_back( itor );
                    lastNumSlices = 1u;

                    //Walk forward
                    for( size_t extraSlice = slice + 1u;
                         extraSlice<mArray && lastNumSlices != numForwardSlices &&
                         (numBackwardSlices + numForwardSlices) < depthOrSlices - 1u;
                         ++extraSlice )
                    {
                        lastNumSlices = numForwardSlices;

                        StagingBoxVec &freeBoxesInExtraSlice = mFreeBoxes[extraSlice];
                        StagingBoxVec::iterator it = freeBoxesInExtraSlice.begin();
                        StagingBoxVec::iterator en = freeBoxesInExtraSlice.end();

                        while( it != en )
                        {
                            if( it->contains( itor->x, itor->y, vol.mWidth, vol.mHeight ) )
                            {
                                selectedRecords.push_back( it );
                                ++numForwardSlices;
                                break;
                            }
                            ++it;
                        }
                    }

                    if( (numBackwardSlices + numForwardSlices) == depthOrSlices - 1u )
                    {
                        //Found it!
                        PixelBlock retVal( vol.mWidth, vol.mHeight, vol.mDepth, vol.mAarray,
                                           PixelUtil::getUnitSize( vol.mFormat ),
                                           mSubresourceData[0].RowPitch,
                                           mSubresourceData[0].DepthPitch );
                        retVal.data = reinterpret_cast<uint8*>( mSubresourceData[0].pData );
                        retVal.x = itor->x;
                        retVal.y = itor->y;
                        if( vol.mDepth > 1u )
                            retVal.z = slice - numBackwardSlices;
                        else
                            retVal.mArrayIdx = slice - numBackwardSlices;

                        for( size_t i = 0; i < depthOrSlices; ++i )
                        {
                            const size_t currentSliceIdx = i + slice - numBackwardSlices;
                            shrinkMultisliceRecords( currentSliceIdx, selectedRecords[i], retVal );
                        }

                        return retVal; //We're done!
                    }

                    //If we couldn't find it, then we need to be prepared for the next iteration.
                    selectedRecords.clear();
                }

                ++itor;
            }
        }

        return PixelBlock();
    }
    //-----------------------------------------------------------------------------------
    PixelBlock D3D11MappedTexture::mapImpl( const PixelVolume & vol )
    {
        N_assert( !mMapping && "startMapRegion already called!" );
        mMapping = true;

#if OGRE_DEBUG_MODE
        //We only warn if isBlock wasn't called. Because if you didn't wait
        //getDynamicBufferMultiplier frames and never called isBlock to check,
        //you're risking certain machines (with slower GPUs) to stall even if yours doesn't.
        if( mMag->getFrameCount() - mLastFrame < mMag->getDynamicBufferMultiplier() /*&&
            !mUserQueriedIfUploadWillStall*/ )
        {
            LogManager::getSingleton().logMessage(
                "PERFORMANCE WARNING: Calling MappedTexture::startMapRegion too soon, "
                "it will very likely stall depending on HW and OS setup. You need to wait"
                " GBufferManager::getDynamicBufferMultiplier frames after having called "
                "MappedTexture::upload before calling startMapRegion again.",
                LML_CRITICAL );
        }
#endif

        mMag->wait( mLastFrame );

        //mCurrentOffset = 0;

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        const uint32 mArray = mIsArray2DTexture ? mDepthOrSlices : 1u;
        for( uint32 i=0; i<mArray; ++i )
        {
            const UINT subresourceIdx = D3D11CalcSubresource( 0, i, 1u );
            HRESULT hr = context->Map( mStagingTexture.Get(), subresourceIdx, D3D11_MAP_WRITE, 0, &mSubresourceData[i] );
            mLastSubresourceData[i] = mSubresourceData[i];

            if( FAILED(hr) || mDevice.isError() )
            {
                String errorDescription = mDevice.getErrorString( hr );
                N_EXCEPT_EX( RenderingAPI, hr, "Error mapping MappedTexture\nError Description:" + errorDescription );
            }
        }

        mFreeBoxes.reserve( mDepthOrSlices );
        for( size_t i=0; i<mDepthOrSlices; ++i )
        {
            mFreeBoxes.push_back( StagingBoxVec() );
            StagingBoxVec &freeBoxesInSlice = mFreeBoxes.back();
            freeBoxesInSlice.push_back( StagingBox( 0, 0, mWidth, mHeight ) );
        }
        
        if( vol.mDepth > 1u || vol.mArray > 1u )
            return mapMultipleSlices( vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray, vol.mFormat );

        size_t bestMatchSlice = 0;
        StagingBoxVec::iterator bestMatch = mFreeBoxes[0].end();

        const size_t mArray = mFreeBoxes.size();
        for( size_t slice = 0; slice < mArray; ++slice )
        {
            StagingBoxVec & freeBoxesInSlice = mFreeBoxes[slice];

            StagingBoxVec::iterator itor = freeBoxesInSlice.begin();
            StagingBoxVec::iterator end  = freeBoxesInSlice.end();

            while( itor != end )
            {
                //Grab the smallest fragment that can hold the request,
                //to avoid hurtful fragmentation.
                if( vol.mWidth <= itor->width && vol.mHeight <= itor->height )
                {
                    if( bestMatch == mFreeBoxes[bestMatchSlice].end() ||
                        itor->width < bestMatch->width ||
                        itor->height < bestMatch->height )
                    {
                        bestMatchSlice = slice;
                        bestMatch = itor;
                    }
                }

                ++itor;
            }
        }

        const size_t sliceIdx = mIsArray2DTexture ? bestMatchSlice : 0;

        PixelBlock retVal( vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray,
                           PixelUtil::getUnitSize( vol.mFormat ),
                           mSubresourceData[sliceIdx].RowPitch,
                           mSubresourceData[sliceIdx].DepthPitch );
        if( !(bestMatchSlice == 0 && bestMatch == mFreeBoxes[0].end()) )
        {
            retVal.x = bestMatch->x;
            retVal.y = bestMatch->y;
            if( !mIsArray2DTexture )
                retVal.z = bestMatchSlice;
            retVal.data = reinterpret_cast<uint8*>( mSubresourceData[sliceIdx].pData );

            //Now shrink our records.
            shrinkRecords( bestMatchSlice, bestMatch, retVal );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11MappedTexture::unmap(void)
    {
        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        const uint32 mArray = mIsArray2DTexture ? mDepthOrSlices : 1u;
        for( uint32 i=0; i<mArray; ++i )
        {
            const UINT subresourceIdx = D3D11CalcSubresource( 0, i, 1u );
            context->Unmap( mStagingTexture.Get(), subresourceIdx );
            memset( &mSubresourceData[i], 0, sizeof( D3D11_MAPPED_SUBRESOURCE ) );
        }
        mFreeBoxes.clear();

        MappedTexture::unmap();
    }
    //-----------------------------------------------------------------------------------
    void D3D11MappedTexture::read(Texture *dstTexture, NCount mipLevel, const PixelBlock *dstBox, const PixelBlock & srcBox, bool shadow )
    {
        if(shadow)
        {
            MappedTexture::read(dstTexture, mipLevel, dstBox, srcBox, true);
            return;
        }

        D3D11_BOX srcBoxD3d;

        srcBoxD3d.left  = srcBox.x;
        srcBoxD3d.top   = srcBox.y;
        srcBoxD3d.right = srcBox.x + srcBox.width;
        srcBoxD3d.bottom= srcBox.y + srcBox.height;
        //These are the possibilities:
        //  Volume -> Array
        //  Volume -> Volume
        //  Array  -> Array
        //  Array  -> Volume    Not possible unless copying one slice due to the restrictions
        //                      in map. Also request prevents this.
        //If we're an Array, we need to copy 1 slice at a time, no matter what type src is.
        UINT srcSlicePos = 0;
        if( !mIsArray2DTexture )
        {
            srcSlicePos = 0;
            //If destination is Array, we'll be copying one slice at a time,
            //so add only srcBox.depth; which will be either 1u, or more
            //than 1u if destination is a 3D texture.
            srcBoxD3d.front = srcBox.getVolumeIdex();
            srcBoxD3d.back  = srcBox.getVolumeIdex() + srcBox.depth;
        }
        else
        {
            srcSlicePos = findRealSlice( srcBox.data );
            srcBoxD3d.front = 0;
            srcBoxD3d.back  = 1u;
        }

        if( PixelUtil::isCompress( mFormat ) )
        {
            uint32 blockWidth = PixelUtil::getBlockWidth( mFormat, false );
            uint32 blockHeight= PixelUtil::getBlockHeight( mFormat, false );
            srcBoxD3d.right = multiCeil( srcBoxD3d.right, blockWidth );
            srcBoxD3d.bottom = multiCeil( srcBoxD3d.bottom, blockHeight );
        }

        const UINT xPos = static_cast<UINT>( dstBox ? dstBox->x : 0 );
        const UINT yPos = static_cast<UINT>( dstBox ? dstBox->y : 0 );
        const UINT zPos = static_cast<UINT>( dstBox ? dstBox->z : 0 );
        UINT dstSlicePos = static_cast<UINT>( dstBox ? dstBox->mArrayIdx : 0 ) + dstTexture->getPoolArray();

        N_assert( dynamic_cast<D3D11TextureGpu*>( dstTexture ) );
        D3D11TextureGpu *dstTextureD3d = static_cast<D3D11TextureGpu*>( dstTexture );

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();

        for( UINT slice=0; slice<(UINT)srcBox.mArray; ++slice )
        {
            const UINT srcSubResourceIdx = D3D11CalcSubresource( 0, srcSlicePos, 1u );
            const UINT dstSubResourceIdx = D3D11CalcSubresource( mipLevel, dstSlicePos, dstTexture->getMipmapCount() );

            context->CopySubresourceRegion( dstTextureD3d->getFinalTextureName(), dstSubResourceIdx,
                                            xPos, yPos, zPos, mStagingTexture.Get(),
                                            srcSubResourceIdx, &srcBoxD3d );
            ++dstSlicePos;
            if( !mIsArray2DTexture )
            {
                ++srcBoxD3d.front;
                ++srcBoxD3d.back;
            }
            else
            {
                ++srcSlicePos;
            }
        }

        if( mDevice.isError() )
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT( RenderingAPI,
                         "Error after calling CopySubresourceRegion\n"
                         "Error Description:" + errorDescription,
                         "D3D11MappedTexture::upload" );
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11MappedTexture::write(Texture * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host)
    {
        
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11VertexArrayObjectShared
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11VertexArrayObjectShared::D3D11VertexArrayObjectShared(const VertexBufferList &vertexBuffers,
        IndexBufferPacked *indexBuffer, OperationType opType, VertexBufferPacked *drawId )
    {
        memset( mStrides, 0, sizeof( mStrides ) );
        memset( mOffsets, 0, sizeof( mOffsets ) );

        N_assert( vertexBuffers.size() + 1 < 16 );
        const size_t numVertexBuffers = vertexBuffers.size();
        for( size_t i=0; i<numVertexBuffers; ++i )
        {
            VertexBufferPacked *vertexBuffer = vertexBuffers[i];
            D3D11BufferOp *op = static_cast<D3D11BufferOp*>(vertexBuffer->getOperation() );
            mVertexBuffers[i] = op->getVboName();
            mStrides[i] = vertexBuffer->getUnitSize();
        }

        {
            VertexBufferPacked *vertexBuffer = drawId;
            D3D11BufferOp *op = static_cast<D3D11BufferOp*>( vertexBuffer->getOperation() );
            mVertexBuffers[numVertexBuffers]    = op->getVboName();
            mStrides[numVertexBuffers]          = vertexBuffer->getUnitSize();
        }

        if( indexBuffer )
        {
            D3D11BufferOp *op = static_cast<D3D11BufferOp*>( indexBuffer->getOperation() );

            mIndexBuffer = op->getVboName();
            mIndexFormat = indexBuffer->getIndexType() == IndexBufferPacked::IT_16BIT ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        }
        else
        {
            mIndexBuffer = 0;
            mIndexFormat = DXGI_FORMAT_UNKNOWN;
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11VertexArrayObject::_updateImmutableResource( uint32 vaoName, uint32 renderQueueId,
        D3D11VertexArrayObjectShared *sharedData )
    {
        mVaoId = vaoName;
        mRenderID = renderQueueId;
        mSharedData = sharedData;
    }
    //-----------------------------------------------------------------------------------
}