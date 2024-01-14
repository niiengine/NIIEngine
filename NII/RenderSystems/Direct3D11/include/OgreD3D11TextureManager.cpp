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
#include "OgreD3D11TextureManager.h"
#include "OgreD3D11Texture.h"
#include "OgreRoot.h"
#include "OgreLogManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreD3D11RenderSystem.h"
#include "OgreD3D11Device.h"

namespace Ogre 
{
    D3D11TextureManager::D3D11TextureManager( GBufferManager *vaoManager, RenderSystem *renderSystem,
                                                    D3D11Device &device ) :
        TextureManager( vaoManager, renderSystem ),
        mDevice( device )
    {
        D3D11_TEXTURE1D_DESC desc1;
        D3D11_TEXTURE2D_DESC desc2;
        D3D11_TEXTURE3D_DESC desc3;

        memset( &desc1, 0, sizeof(desc1) );
        memset( &desc2, 0, sizeof(desc2) );
        memset( &desc3, 0, sizeof(desc3) );

        desc1.Width     = 4;
        desc1.MipLevels = 1u;
        desc1.ArraySize = 1u;
        desc1.Format    = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc1.Usage     = D3D11_USAGE_IMMUTABLE;
        desc1.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        desc2.Width     = 4u;
        desc2.Height    = 4u;
        desc2.MipLevels = 1u;
        desc2.ArraySize = 1u;
        desc2.Format    = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc2.SampleDesc.Count = 1u;
        desc2.Usage     = D3D11_USAGE_IMMUTABLE;
        desc2.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        desc3.Width     = 4u;
        desc3.Height    = 4u;
        desc3.Depth     = 4u;
        desc3.MipLevels = 1u;
        desc3.Format    = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc3.Usage     = D3D11_USAGE_IMMUTABLE;
        desc3.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        //Must be large enough to hold the biggest transfer we'll do.
        uint8 c_whiteData[4*4*6*4];
        uint8 c_blackData[4*4*6*4];
        memset( c_whiteData, 0xff, sizeof( c_whiteData ) );
        memset( c_blackData, 0x00, sizeof( c_blackData ) );

        D3D11_SUBRESOURCE_DATA dataWhite;
        D3D11_SUBRESOURCE_DATA dataBlack[6];

        dataWhite.pSysMem           = c_whiteData;
        dataWhite.SysMemPitch       = 4u * sizeof(uint32);
        dataWhite.SysMemSlicePitch  = dataWhite.SysMemPitch * 4u;

        for( size_t i=0; i<6u; ++i )
        {
            dataBlack[i].pSysMem            = c_blackData;
            dataBlack[i].SysMemPitch        = 4u * sizeof(uint32);
            dataBlack[i].SysMemSlicePitch   = dataBlack[i].SysMemPitch * 4u;
        }

        ID3D11Texture1D *tex1D;
        ID3D11Texture2D *tex2D;
        ID3D11Texture3D *tex3D;

        for( int i=1; i<=Texture::T_3D; ++i )
        {
            HRESULT hr = 0;
            switch( i )
            {
            case Texture::T_Unknow:
                N_EXCEPT( Exception::ERR_INVALID_STATE, "Ogre should never hit this path");
                break;
            case Texture::T_1D:
            case Texture::T_1DArray:
                hr = mDevice->CreateTexture1D( &desc1, &dataWhite, &tex1D );
                mBlankTexture[i].Attach( tex1D );
                break;
            case Texture::T_2D:
            case Texture::T_2DArray:
                hr = mDevice->CreateTexture2D( &desc2, &dataWhite, &tex2D );
                mBlankTexture[i].Attach( tex2D );
                break;
            case Texture::T_CUBE:
            case Texture::Type::T_CUBEArray:
                desc2.ArraySize = 6;
                desc2.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
                hr = mDevice->CreateTexture2D( &desc2, dataBlack, &tex2D );
                mBlankTexture[i].Attach( tex2D );
                desc2.MiscFlags = 0;
                desc2.ArraySize = 1;
                break;
            case Texture::T_3D:
                hr = mDevice->CreateTexture3D( &desc3, &dataWhite, &tex3D );
                mBlankTexture[i].Attach( tex3D );
                break;
            }

            if( FAILED(hr) || mDevice.isError() )
            {
                String errorDescription = mDevice.getErrorString( hr );
                N_EXCEPT_EX( RenderingAPI, hr,
                                "Error creating dummy textures\nError Description:" + errorDescription);
            }

            hr = device->CreateShaderResourceView( mBlankTexture[i].Get(), 0, mDefaultSrv[i].GetAddressOf() );
            if( FAILED(hr) || mDevice.isError() )
            {
                String errorDescription = mDevice.getErrorString( hr );
                N_EXCEPT_EX( RenderingAPI, hr,
                                "Error creating dummy SRVs\nError Description:" + errorDescription);
            }
        }

        mBlankTexture[Texture::T_Unknow]    = mBlankTexture[Texture::T_2D];
        mDefaultSrv[Texture::T_Unknow]      = mDefaultSrv[Texture::T_2D];
    }
    //-----------------------------------------------------------------------------------
    D3D11TextureManager::~D3D11TextureManager()
    {
        destroyAll();
    }
    //-----------------------------------------------------------------------------------
    TextureGpu* D3D11TextureManager::createTextureGpuWindow( bool fromFlipModeSwapchain,
                                                                Window *window )
    {
        return N_new D3D11TextureGpuWindow( Texture::RT_Disk, mBufferManager,
                                               "RenderWindow",
                                               Texture::MM_FRAME|
                                               Texture::MM_Window|
                                               (fromFlipModeSwapchain ? 0 : Texture::MM_ManualMSAA)|
                                               (fromFlipModeSwapchain ? Texture::MM_AlterView : 0),
                                               Texture::T_2D, this, window );
    }
    //-----------------------------------------------------------------------------------
    TextureGpu* D3D11TextureManager::createWindowDepthBuffer(void)
    {
        return N_new D3D11TextureGpuRenderTarget( Texture::RT_Disk, mBufferManager,
                                                     "RenderWindow DepthBuffer",
                                                     Texture::MM_FRAME|
                                                     Texture::MM_Window,
                                                     Texture::T_2D, this );
    }
    //-----------------------------------------------------------------------------------
    ID3D11Resource* D3D11TextureManager::getBlankTextureD3dName(Texture::Type::Texture::Type textureType ) const
    {
        return mBlankTexture[textureType].Get();
    }
    //-----------------------------------------------------------------------------------
    ID3D11ShaderResourceView * D3D11TextureManager::getBlankTextureSrv(Texture::Type::Texture::Type textureType ) const
    {
        return mDefaultSrv[textureType].Get();
    }
    //-----------------------------------------------------------------------------------
    TextureGpu * D3D11TextureManager::createTextureImpl( RecoverType rtype,
        IdString name, uint32 textureFlags, Texture::Type::Texture::Type initialType )
    {
        D3D11TextureGpu *retVal = 0;
        if( textureFlags & Texture::MM_FRAME )
        {
            retVal = N_new D3D11TextureGpuRenderTarget( rtype, mBufferManager, name,
                                                           textureFlags, initialType, this );
        }
        else
        {
            retVal = N_new D3D11TextureGpu( rtype, mBufferManager, name,
                                               textureFlags, initialType, this );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    MappedTexture* D3D11TextureManager::createMappedImpl( uint32 width, uint32 height,
                                                                      uint32 depth, uint32 slices,
                                                                      PixelFormat pf )
    {
        D3D11MappedTexture *retVal =
                N_new D3D11MappedTexture( mBufferManager,
                                              PixelUtil::getFamily( pf ),
                                              width, height, std::max( depth, slices ), mDevice );
        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureManager::destroyMappedImpl( MappedTexture *stagingTexture )
    {
        //Do nothing, caller will delete stagingTexture.
    }
    //-----------------------------------------------------------------------------------
    TextureAsync* D3D11TextureManager::createAsyncImpl(
            uint32 width, uint32 height, uint32 depthOrSlices,
            Texture::Type::Texture::Type textureType, PixelFormat pixelFormatFamily )
    {
        D3D11GBufferManager *vaoManager = static_cast<D3D11GBufferManager*>( mBufferManager );
        return N_new D3D11AsyncTextureTicket( width, height, depthOrSlices, textureType,
                                                 pixelFormatFamily, vaoManager );
    }
    //---------------------------------------------------------------------
    D3D11TextureManager::D3D11TextureManager( D3D11Device & device ) : TextureManager(), mDevice (device)
    {
        if( mDevice.isNull())
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Invalid Direct3DDevice passed");
        // register with group manager
        ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }
    //---------------------------------------------------------------------
    D3D11TextureManager::~D3D11TextureManager()
    {
        // unregister with group manager
        ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);

    }
    //---------------------------------------------------------------------
    Resource* D3D11TextureManager::createImpl(const String& name, 
        ResourceHandle handle, const String& group, bool isManual, 
        ManualResourceLoader* loader, const PropertyData* createParams)
    {
        return new D3D11Texture(this, name, handle, group, isManual, loader, mDevice); 
    }
    //---------------------------------------------------------------------
    PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
    {
        // Basic filtering
        DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(format));

        return D3D11Mappings::_getPF(d3dPF);
    }
    //---------------------------------------------------------------------
}
