/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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

#include "OgreD3D11Prerequisites.h"

#include "Vao/OgreD3D11AsyncTicket.h"
#include "Vao/OgreD3D11GBufferManager.h"

#include "Vao/OgreStagingBuffer.h"

#include "OgreD3D11Device.h"
#include "OgreD3D11TextureGpu.h"
#include "OgreD3D11Mappings.h"
#include "OgreD3D11Device.h"
#include "Vao/OgreD3D11GBufferManager.h"

#include "OgreTextureBox.h"
#include "OgreTextureGpuManager.h"
#include "OgrePixelFormatGpuUtils.h"

#include "OgreLogManager.h"

namespace Ogre
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11AsyncTicket
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11AsyncTicket::D3D11AsyncTicket( GpuBuffer * creator, MappedBuffer * buffer, NCount oft, NCount size, D3D11Device & device ) :
        GpuBufferAsync( creator, buffer, oft, size ),
        mDevice( device )
    {
        //Base constructor has already called async. We should now place a fence.
        mFenceName = D3D11GBufferManager::createFence( mDevice );
    }
    //-----------------------------------------------------------------------------------
    D3D11AsyncTicket::~D3D11AsyncTicket()
    {
    }
    //-----------------------------------------------------------------------------------
    const void * D3D11AsyncTicket::mapImpl(void)
    {
        if( mFenceName )
            *mFenceName.GetAddressOf() = D3D11GBufferManager::waitFor( mFenceName.Get(), mDevice.GetImmediateContext() );

        return mMapBuffer->readMap( mMapOffset, mSize );
    }
    //-----------------------------------------------------------------------------------
    OperationReason D3D11AsyncTicket::isDataValid() const
    {
        if( mFenceName )
        {
            HRESULT hr = mDevice.GetImmediateContext()->GetData( mFenceName.Get(), NULL, 0, 0 );

            if( FAILED( hr ) )
            {
                return OR_ERROR;
            }

            return hr != S_FALSE ?  OR_OK : OR_BLOCK;
        }

        return OR_OK;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // D3D11AsyncTextureTicket
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11AsyncTextureTicket::D3D11AsyncTextureTicket( NCount width, NCount height,
                                                      NCount depth, NCount array,
                                                      
                                                      Texture::Type textureType,
                                                      PixelFormat pf,
                                                      D3D11GBufferManager *vaoManager ) :
        TextureAsync( width, height, depth, array, textureType, pf ),
        mDownloadFrame( 0 ),
        mMag( vaoManager ),
        mMappedSlice( 0 ),
        mIsArray2DTexture( false )
    {
        D3D11Device &device = mMag->getDevice();

        HRESULT hr;
        if( mDepth > 1u &&
            mWidth <= D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
            mHeight <= D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION )
        {
            D3D11_TEXTURE3D_DESC desc;
            memset( &desc, 0, sizeof( desc ) );

            desc.Width      = static_cast<UINT>( mWidth );
            desc.Height     = static_cast<UINT>( mHeight );
            desc.Depth      = static_cast<UINT>( mDepth );
            desc.MipLevels  = 1u;
            desc.Format     = D3D11Mappings::getFamily( mFormat );
            desc.Usage              = D3D11_USAGE_STAGING;
            desc.BindFlags          = 0;
            desc.CPUAccessFlags     = D3D11_CPU_ACCESS_READ;
            desc.MiscFlags          = 0;

            if( PixelUtil::isCompress( mFormat ) )
            {
                desc.Width  = std::max( desc.Width, 4u );
                desc.Height = std::max( desc.Height, 4u );
            }

            ID3D11Texture3D *texture = 0;
            hr = device->CreateTexture3D( &desc, 0, &texture );
            mStagingTexture.Attach( texture );
        }
        else
        {
            D3D11_TEXTURE2D_DESC desc;
            memset( &desc, 0, sizeof( desc ) );

            desc.Width      = static_cast<UINT>( mWidth );
            desc.Height     = static_cast<UINT>( mHeight );
            desc.MipLevels  = 1u;
            desc.ArraySize  = static_cast<UINT>( mArray );
            desc.Format     = D3D11Mappings::getFamily( mFormat );
            desc.SampleDesc.Count   = 1u;
            desc.Usage              = D3D11_USAGE_STAGING;
            desc.BindFlags          = 0;
            desc.CPUAccessFlags     = D3D11_CPU_ACCESS_READ;
            desc.MiscFlags          = 0;

            if( PixelUtil::isCompress( mFormat ) )
            {
                desc.Width  = std::max( desc.Width, 4u );
                desc.Height = std::max( desc.Height, 4u );
            }

            ID3D11Texture2D *texture = 0;
            hr = device->CreateTexture2D( &desc, 0, &texture );
            mStagingTexture.Attach( texture );
            mIsArray2DTexture = true;
        }

        if( FAILED(hr) || device.isError() )
        {
            String errorDescription = device.getErrorString( hr );
            N_EXCEPT_EX( RenderingAPI, hr, "Error creating TextureAsync\nError Description:" + errorDescription );
        }
    }
    //-----------------------------------------------------------------------------------
    D3D11AsyncTextureTicket::~D3D11AsyncTextureTicket()
    {
        if( mBlockType == BT_Mapping )
            unmap();
    }
    //-----------------------------------------------------------------------------------
    void D3D11AsyncTextureTicket::writeImpl( Texture * textureSrc, NCount mipLevel, PixelBlock * srcBox, NCount oft, bool notify )
    {
        TextureAsync::writeImpl( textureSrc, mipLevel, srcBox, notify);

        mDownloadFrame = mMag->getFrameCount();

        mAccurateFence.Reset();

        PixelBlock srcTextureBox;
        PixelBlock fullSrcTextureBox( textureSrc->getEmptyBox( mipLevel ) );

        if( srcBox )
        {
            srcTextureBox = *srcBox;
            srcTextureBox.mRowSize   = fullSrcTextureBox.mRowSize;
            srcTextureBox.mPixelSize = fullSrcTextureBox.mPixelSize;
            srcTextureBox.mSliceSize = fullSrcTextureBox.mSliceSize;
        }
        else
        {
            srcTextureBox = fullSrcTextureBox;
        }

        if( textureSrc->isPoolSlice() )
        {
            fullSrcTextureBox.mArrayIdx= textureSrc->getPoolArray();
            fullSrcTextureBox.mArray = textureSrc->getPool()->mPool->getArray();

            srcTextureBox.mArrayIdx += textureSrc->getPoolArray();
        }

        const Texture::Type::Texture::Type textureType = textureSrc->getActualType();

        D3D11_BOX srcBoxD3d;

        srcBoxD3d.left  = srcTextureBox.x;
        srcBoxD3d.top   = srcTextureBox.y;
        srcBoxD3d.front = srcTextureBox.z;
        srcBoxD3d.right = srcTextureBox.x + srcTextureBox.width;
        srcBoxD3d.bottom= srcTextureBox.y + srcTextureBox.height;
        srcBoxD3d.back  = srcTextureBox.z + srcTextureBox.depth;

        if( PixelUtil::isCompress( mFormat ) )
        {
            uint32 blockWidth  = PixelUtil::getBlockWidth( mFormat, false );
            uint32 blockHeight = PixelUtil::getBlockHeight( mFormat, false );
            srcBoxD3d.right     = multiCeil( srcBoxD3d.right, blockWidth );
            srcBoxD3d.bottom    = multiCeil( srcBoxD3d.bottom, blockHeight );
        }

        UINT srcSlicePos = srcTextureBox.mArrayIdx;

        //These are the possibilities:
        //  Volume -> Array
        //  Array  -> Array
        //  Volume -> Volume
        //  Array  -> Volume
        //If we're an Array, we need to copy 1 slice at a time, no matter what type src is.
        UINT mArray = srcTextureBox.mArray;
        if( mIsArray2DTexture )
        {
            mArray = srcTextureBox.getVolume();
            srcBoxD3d.back = srcBoxD3d.front + 1u;
        }
        UINT zPos = 0;
        UINT dstSlicePos = 0;

        N_assert( dynamic_cast<D3D11TextureGpu*>( textureSrc ) );
        D3D11TextureGpu * srcTextureD3d = static_cast<D3D11TextureGpu*>( textureSrc );

        D3D11Device & device = mMag->getDevice();
        ID3D11DeviceContextN * context = device.GetImmediateContext();

        for( UINT slice=0; slice < mArray; ++slice )
        {
            const UINT srcSubResourceIdx = D3D11CalcSubresource( mipLevel, srcSlicePos, textureSrc->getMipmapCount() );
            const UINT dstSubResourceIdx = D3D11CalcSubresource( 0, dstSlicePos, 1u );

            context->CopySubresourceRegion( mStagingTexture.Get(), dstSubResourceIdx,
                                            0, 0, zPos, srcTextureD3d->getFinalTextureName(),
                                            srcSubResourceIdx, &srcBoxD3d );
            if( textureType == Texture::T_3D )
            {
                ++srcBoxD3d.front;
                ++srcBoxD3d.back;
            }
            else
            {
                ++srcSlicePos;
            }
            if( !mIsArray2DTexture )
                ++zPos;
            else
                ++dstSlicePos;
        }

        if( device.isError() )
        {
            String errorDescription = device.getErrorString();
            N_EXCEPT( RenderingAPI, "Error after calling CopySubresourceRegion\n"
                         "Error Description:" + errorDescription);
        }

        if( notify )
            mAccurateFence = mMag->createFence();
    }
    //-----------------------------------------------------------------------------------
    PixelBlock D3D11AsyncTextureTicket::mapImpl(NCount depthIdx, NCount arrayIdx, NCount oft )
    {
        waitForDownloadToFinish();

        D3D11Device &device = mMag->getDevice();
        ID3D11DeviceContextN *context = device.GetImmediateContext();

        mMappedSlice = depthIdx;
        const UINT subresourceIdx = D3D11CalcSubresource( 0, depthIdx, 1u );

        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        context->Map( mStagingTexture.Get(), subresourceIdx, D3D11_MAP_READ, 0, &mappedSubresource );

        PixelBlock retVal( mWidth, mHeight, getDepth(), getArray(),
                           PixelUtil::getUnitSize( mFormat ),
                           mappedSubresource.RowPitch, mappedSubresource.DepthPitch );
        retVal.data = mappedSubresource.pData;

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11AsyncTextureTicket::unmapImpl(void)
    {
        const UINT subresourceIdx = D3D11CalcSubresource( 0, mMappedSlice, 1u );

        D3D11Device &device = mMag->getDevice();
        ID3D11DeviceContextN *context = device.GetImmediateContext();
        context->Unmap( mStagingTexture.Get(), subresourceIdx );
    }
    //-----------------------------------------------------------------------------------
    bool D3D11AsyncTextureTicket::getArray(void) const
    {
        return !mIsArray2DTexture ?  : 1;
    }
    //-----------------------------------------------------------------------------------
    void D3D11AsyncTextureTicket::waitForDownloadToFinish(void)
    {
        if( mBlockType != BT_Writting )
            return; //We're done.

        if( mAccurateFence )
        {
            *mAccurateFence.GetAddressOf() = mMag->waitFor( mAccurateFence.Get() );
            mAccurateFence.Reset();
        }
        else
        {
            mMag->wait( mDownloadFrame );
            mQueryCount = 0;
        }

        mBlockType = BT_None;
    }
    //-----------------------------------------------------------------------------------
    bool D3D11AsyncTextureTicket::isDataValid(void)
    {
        if( !TextureAsync::isDataValid() )
        {
            //Early out. The texture is not even finished being ready.
            //We didn't even start the actual download.
            return false;
        }

        bool retVal = false;

        if( mBlockType != BT_Writting )
        {
            retVal = true;
        }
        else if( mAccurateFence )
        {
            //Ask D3D11 API to return immediately and tells us about the fence
            if( mMag->queryIsDone( mAccurateFence.Get() ) )
            {
                mAccurateFence.Reset();
                if( mBlockType != BT_Mapping )
                    mBlockType = BT_None;
            }
        }
        else
        {
            if( mDownloadFrame == mMag->getFrameCount() )
            {
                if( mQueryCount > 3 )
                {
                    //Use is not calling vaoManager->update(). Likely it's stuck in an
                    //infinite loop checking if we're done, but we'll always return false.
                    //If so, switch to accurate tracking.
                    mAccurateFence = mMag->createFence() ;

                    D3D11Device &device = mMag->getDevice();
                    ID3D11DeviceContextN *context = device.GetImmediateContext();
                    context->Flush();

                    LogManager::getSingleton().logMessage(
                                "WARNING: Calling TextureAsync::isDataValid too "
                                "often with innacurate tracking in the same frame this transfer "
                                "was issued. Switching to accurate tracking. If this is an accident, "
                                "wait until you've rendered a few frames before checking if it's done. "
                                "If this is on purpose, consider calling TextureAsync::download()"
                                " with accurate tracking enabled.", LML_CRITICAL );
                }

                ++mQueryCount;
            }

            //We're downloading but have no fence. That means we don't have accurate tracking.
            retVal = mMag->isFrameFinished( mDownloadFrame );
            ++mQueryCount;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
}
