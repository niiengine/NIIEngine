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
#include "OgreD3D11Texture.h"
#include "OgreD3D11HardwarePixelBuffer.h"
#include "OgreD3D11Mappings.h"
#include "OgreD3D11Device.h"
#include "OgreD3D11RenderSystem.h"
#include "OgreRoot.h"
#include "OgreLogManager.h"
#include "OgreException.h"

namespace Ogre 
{
    //---------------------------------------------------------------------
    D3D11Texture::D3D11Texture(ResourceManager* creator, const String& name, 
        ResourceHandle handle, const String& group, bool isManual, 
        ManualResourceLoader* loader, D3D11Device & device)
        :Texture(creator, name, handle, group, isManual, loader),
        mDevice(device), 
        mAutoMipMapGeneration(false)
    {
        mFSAAType.Count = 1;
        mFSAAType.Quality = 0;
    }
    //---------------------------------------------------------------------
    D3D11Texture::~D3D11Texture()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        if (isLoaded())
        {
            unload(); 
        }
        else
        {
            freeInternalResources();
        }
    }
    //---------------------------------------------------------------------
    void D3D11Texture::notifyDeviceLost(D3D11Device* device)
    {
        unloadImpl();
    }
    //---------------------------------------------------------------------
    void D3D11Texture::notifyDeviceRestored(D3D11Device* device)
    {
        if(mIsManual)
        {
            preLoadImpl();
            createInternalImpl();
            if (mLoader != NULL)
                mLoader->loadResource(this);
            postLoadImpl();
        }
        else
        {
            preLoadImpl();
            loadImpl();
            postLoadImpl();
        }
    }
    //---------------------------------------------------------------------
    void D3D11Texture::copyToTexture(TexturePtr& target)
    {
        // check if this & target are the same format and type
        // blitting from or to cube textures is not supported yet
        if (target->getUsage() != this->getUsage() ||
            target->getTextureType() != this->getTextureType())
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Src. and dest. textures must be of same type and must have the same usage !!!");
        }


        D3D11Texture *other;
        // get the target
        other = static_cast< D3D11Texture * >( target.get() );

        mDevice.GetImmediateContext()->CopyResource(other->getTextureResource(), mpTex.Get());
        if (mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(RenderingAPI, "D3D11 device cannot copy resource\nError Description:" + errorDescription);
        }

    }
    //---------------------------------------------------------------------
    void D3D11Texture::loadImpl()
    {
        Texture::loadImpl();

        if (mUsage & TU_RENDERTARGET)
        {
            return;
        }

        _setSrcAttributes(mWidth, mHeight, mDepth, mFormat);
    }
    //---------------------------------------------------------------------
    void D3D11Texture::freeInternalResources(void)
    {
        freeInternalImpl();
    }
    //---------------------------------------------------------------------
    void D3D11Texture::freeInternalImpl()
    {
        mpTex.Reset();
        mpShaderResourceView.Reset();
        mp1DTex.Reset();
        mp2DTex.Reset();
        mp3DTex.Reset();
    }
    //---------------------------------------------------------------------
    void D3D11Texture::createInternalResources(void)
    {
        createInternalImpl();
    }

    //---------------------------------------------------------------------
    void D3D11Texture::createInternalImpl(void)
    {
        // If mSrcWidth and mSrcHeight are zero, the requested extents have probably been set
        // through setWidth and setHeight, which set mWidth and mHeight. Take those values.
        if(mSrcWidth == 0 || mSrcHeight == 0) {
            mSrcWidth = mWidth;
            mSrcHeight = mHeight;
        }

        mFormat = D3D11Mappings::_getClosestSupportedPF(mFormat);

        // Choose closest supported D3D format
        mD3DFormat = D3D11Mappings::_getGammaFormat(D3D11Mappings::_getPF(mFormat), isHardwareGammaEnabled());

        mFSAAType.Count = 1;
        mFSAAType.Quality = 0;
        if((mUsage & TU_RENDERTARGET) != 0 && (mUsage & TU_DYNAMIC) == 0)
        {
            D3D11RenderSystem* rsys = static_cast<D3D11RenderSystem*>(Root::getSingleton().getRenderSystem());
            // http://msdn.microsoft.com/en-us/library/windows/desktop/ff476150%28v=vs.85%29.aspx#ID3D11Device_CreateTexture2D
            // 10Level9, When using D3D11_BIND_SHADER_RESOURCE, SampleDesc.Count must be 1.
            if(rsys->_getFeatureLevel() >= D3D_FEATURE_LEVEL_10_0 || (mUsage & TU_NOT_SRV))
                rsys->determineFSAASettings(mFSAA, mFSAAHint, mD3DFormat, &mFSAAType);
        }

        // load based on tex.type
        switch (this->getTextureType())
        {
        case TEX_TYPE_1D:
            {
                D3D11RenderSystem* rs = (D3D11RenderSystem*)Root::getSingleton().getRenderSystem();
                if(rs->_getFeatureLevel() >= D3D_FEATURE_LEVEL_10_0)
                {
                    this->_create1DTex();
                    break; // For Feature levels that do not support 1D textures, revert to creating a 2D texture.
                }
            }
        case TEX_TYPE_2D:
        case TEX_TYPE_CUBE_MAP:
        case TEX_TYPE_2D_ARRAY:
            this->_create2DTex();
            break;
        case TEX_TYPE_3D:
            this->_create3DTex();
            break;
        default:
            this->freeInternalResources();
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Unknown texture type");
        }
    }
    //---------------------------------------------------------------------
    void D3D11Texture::_create1DTex()
    {
        // we must have those defined here
        N_assert(mSrcWidth > 0 || mSrcHeight > 0);

        // determine total number of mipmaps including main one (d3d11 convention)
        UINT numMips = (mNumRequestedMipmaps == MIP_UNLIMITED || (1U << mNumRequestedMipmaps) > mSrcWidth) ? 0 : mNumRequestedMipmaps + 1;

        D3D11_TEXTURE1D_DESC desc;
        desc.Width          = static_cast<UINT>(mSrcWidth);
        desc.MipLevels      = numMips;
        desc.ArraySize      = 1;
        desc.Format         = mD3DFormat;
		desc.Usage			= D3D11Mappings::_getUsage(_getTextureUsage());
		desc.BindFlags		= D3D11Mappings::_getTextureBindFlags(mD3DFormat, _getTextureUsage());
		desc.CPUAccessFlags = D3D11Mappings::_getAccessFlags(_getTextureUsage());
        desc.MiscFlags      = D3D11Mappings::_getTextureMiscFlags(desc.BindFlags, getTextureType(), _getTextureUsage());

        // create the texture
        HRESULT hr = mDevice->CreateTexture1D(  
            &desc,
            NULL,
            mp1DTex.ReleaseAndGetAddressOf());                      // data pointer
        // check result and except if failed
        if (FAILED(hr) || mDevice.isError())
        {
            this->freeInternalResources();
			String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr, "Error creating texture\nError Description:" + errorDescription );
        }

        _queryInterface<ID3D11Texture1D, ID3D11Resource>(mp1DTex, &mpTex);
        _create1DResourceView();
    }
    //---------------------------------------------------------------------
    void D3D11Texture::_create1DResourceView()
    {
        // set final tex. attributes from tex. description
        // they may differ from the source image !!!
        HRESULT hr;
        D3D11_TEXTURE1D_DESC desc;

        // set final tex. attributes from tex. description
        // they may differ from the source image !!!
        mp1DTex->GetDesc(&desc);
        mNumMipmaps = desc.MipLevels - 1;

        ZeroMemory( &mSRVDesc, sizeof(mSRVDesc) );
        mSRVDesc.Format = desc.Format;
        mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
        mSRVDesc.Texture1D.MipLevels = desc.MipLevels;
        hr = (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) ? mDevice->CreateShaderResourceView(mp1DTex.Get(), &mSRVDesc, mpShaderResourceView.ReleaseAndGetAddressOf()) : S_FALSE;
        if (FAILED(hr) || mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
                "D3D11 device can't create shader resource view.\nError Description:" + errorDescription);
        }

        this->_setFinalAttributes(desc.Width, 1, 1, D3D11Mappings::_getPF(desc.Format), desc.MiscFlags);
    }
    //---------------------------------------------------------------------
    inline bool IsPowerOfTwo(unsigned int n)                { return ((n&(n-1))==0);                    }
    //---------------------------------------------------------------------
    void D3D11Texture::_create2DTex()
    {
        // we must have those defined here
        N_assert(mSrcWidth > 0 || mSrcHeight > 0);

        // determine total number of mipmaps including main one (d3d11 convention)
        UINT numMips = (mNumRequestedMipmaps == MIP_UNLIMITED || (1U << mNumRequestedMipmaps) > std::max(mSrcWidth, mSrcHeight)) ? 0 : mNumRequestedMipmaps + 1;
        if(D3D11Mappings::_isBinaryCompressedFormat(mD3DFormat) && numMips > 1)
            numMips = std::max(1U, numMips - 2);

        D3D11_TEXTURE2D_DESC desc;
        desc.Width          = static_cast<UINT>(mSrcWidth);
        desc.Height         = static_cast<UINT>(mSrcHeight);
        desc.MipLevels      = numMips;
        desc.ArraySize      = mDepth == 0 ? 1 : mDepth;
        desc.Format         = mD3DFormat;
        desc.SampleDesc     = mFSAAType;
        desc.Usage          = D3D11Mappings::_getUsage(_getTextureUsage());
        desc.BindFlags      = D3D11Mappings::_getTextureBindFlags(mD3DFormat, _getTextureUsage());
        desc.CPUAccessFlags = D3D11Mappings::_getAccessFlags(_getTextureUsage());
        desc.MiscFlags      = D3D11Mappings::_getTextureMiscFlags(desc.BindFlags, getTextureType(), _getTextureUsage());

        if (this->getTextureType() == TEX_TYPE_CUBE_MAP)
        {
                desc.ArraySize          = 6;
        }

        D3D11RenderSystem* rs = (D3D11RenderSystem*)Root::getSingleton().getRenderSystem();
        if(rs->_getFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
        {
            // http://msdn.microsoft.com/en-us/library/windows/desktop/ff476150%28v=vs.85%29.aspx#ID3D11Device_CreateTexture2D
            // ...If MipCount > 1, Dimensions must be integral power of two...
            if(!IsPowerOfTwo(desc.Width) || !IsPowerOfTwo(desc.Height))
            {
                desc.MipLevels = 1;
            }

#if 0
           // there seems to be a Microsoft bug that crash if you do GenerateMips in a level less then D3D_FEATURE_LEVEL_10_0
           // is this still true or addressed by the code above?
           desc.MipLevels = 1;
#endif
        }

        // create the texture
        HRESULT hr = mDevice->CreateTexture2D(  
            &desc,
            NULL,// data pointer
            mp2DTex.ReleaseAndGetAddressOf());
        // check result and except if failed
        if (FAILED(hr) || mDevice.isError())
        {
            this->freeInternalResources();
            String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
                "Error creating texture\nError Description:" + errorDescription, 
                "D3D11Texture::_create2DTex" );
        }

        //set the base texture we'll use in the render system
        _queryInterface<ID3D11Texture2D, ID3D11Resource>(mp2DTex, &mpTex);

        _create2DResourceView();
    }
    //----------------------------------------------------------------------------
    void D3D11Texture::_create2DResourceView()
    {
        // set final tex. attributes from tex. description
        // they may differ from the source image !!!
        HRESULT hr;
        D3D11_TEXTURE2D_DESC desc;
        mp2DTex->GetDesc(&desc);
        mNumMipmaps = desc.MipLevels - 1;
        
        ZeroMemory( &mSRVDesc, sizeof(mSRVDesc) );
        mSRVDesc.Format = desc.Format;
        
        switch(this->getTextureType())
        {
        case TEX_TYPE_CUBE_MAP:
            mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            mSRVDesc.TextureCube.MipLevels = desc.MipLevels;
            mSRVDesc.TextureCube.MostDetailedMip = 0;
            break;

        case TEX_TYPE_2D_ARRAY:
            if (mFSAAType.Count > 1)
            {
                mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                mSRVDesc.Texture2DMSArray.FirstArraySlice = 0;
                mSRVDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
            }
            else
            {
                mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                mSRVDesc.Texture2DArray.MostDetailedMip = 0;
                mSRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
                mSRVDesc.Texture2DArray.FirstArraySlice = 0;
                mSRVDesc.Texture2DArray.ArraySize = desc.ArraySize;
            }
            break;

        case TEX_TYPE_2D:
        case TEX_TYPE_1D:  // For Feature levels that do not support 1D textures, revert to creating a 2D texture.
            if (mFSAAType.Count > 1)
            {
                mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                mSRVDesc.Texture2D.MostDetailedMip = 0;
                mSRVDesc.Texture2D.MipLevels = desc.MipLevels;
            }
            break;
        }

        hr = (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) ? mDevice->CreateShaderResourceView(mp2DTex.Get(), &mSRVDesc,mpShaderResourceView.ReleaseAndGetAddressOf()) : S_FALSE;
        if (FAILED(hr) || mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
                "D3D11 device can't create shader resource view.\nError Description:" + errorDescription,
                "D3D11Texture::_create2DTex");
        }

        this->_setFinalAttributes(desc.Width, desc.Height, desc.ArraySize / getNumFaces(), D3D11Mappings::_getPF(desc.Format), desc.MiscFlags);
    }
    //---------------------------------------------------------------------
    void D3D11Texture::_create3DTex()
    {
        // we must have those defined here
        N_assert(mWidth > 0 && mHeight > 0 && mDepth>0);

        // determine total number of mipmaps including main one (d3d11 convention)
        UINT numMips = (mNumRequestedMipmaps == MIP_UNLIMITED || (1U << mNumRequestedMipmaps) > std::max(std::max(mSrcWidth, mSrcHeight), mDepth)) ? 0 : mNumRequestedMipmaps + 1;

        D3D11_TEXTURE3D_DESC desc;
        desc.Width          = static_cast<UINT>(mSrcWidth);
        desc.Height         = static_cast<UINT>(mSrcHeight);
        desc.Depth          = static_cast<UINT>(mDepth);
        desc.MipLevels      = numMips;
        desc.Format         = mD3DFormat;
		desc.Usage			= D3D11Mappings::_getUsage(_getTextureUsage());
        desc.BindFlags      = D3D11Mappings::_getTextureBindFlags(mD3DFormat, _getTextureUsage());

        D3D11RenderSystem* rsys = static_cast<D3D11RenderSystem*>(Root::getSingleton().getRenderSystem());
        if (rsys->_getFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
           desc.BindFlags      &= ~D3D11_BIND_RENDER_TARGET;

		desc.CPUAccessFlags = D3D11Mappings::_getAccessFlags(_getTextureUsage());
        desc.MiscFlags      = 0;

        // create the texture
        HRESULT hr = mDevice->CreateTexture3D(  
            &desc,
            NULL,
            mp3DTex.ReleaseAndGetAddressOf());                      // data pointer
        // check result and except if failed
        if (FAILED(hr) || mDevice.isError())
        {
            this->freeInternalResources();
            String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
                "Error creating texture\nError Description:" + errorDescription, 
                "D3D11Texture::_create3DTex" );
        }

        _queryInterface<ID3D11Texture3D, ID3D11Resource>(mp3DTex, &mpTex);
        _create3DResourceView();
    }
    //-------------------------------------------------------------------------------
    void D3D11Texture::_create3DResourceView()
    {
        // set final tex. attributes from tex. description
        // they may differ from the source image !!!
        HRESULT hr;
        D3D11_TEXTURE3D_DESC desc;
        mp3DTex->GetDesc(&desc);
        mNumMipmaps = desc.MipLevels - 1;

        ZeroMemory( &mSRVDesc, sizeof(mSRVDesc) );
        mSRVDesc.Format = desc.Format;
        mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        mSRVDesc.Texture3D.MostDetailedMip = 0;
        mSRVDesc.Texture3D.MipLevels = desc.MipLevels;
        hr = (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) ? mDevice->CreateShaderResourceView(mp3DTex.Get(), &mSRVDesc, mpShaderResourceView.ReleaseAndGetAddressOf()) : S_FALSE;
        if (FAILED(hr) || mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
                "D3D11 device can't create shader resource view.\nError Description:" + errorDescription,
                "D3D11Texture::_create3DTex");
        }

        this->_setFinalAttributes(desc.Width, desc.Height, desc.Depth, D3D11Mappings::_getPF(desc.Format), desc.MiscFlags);
    }
    //-------------------------------------------------------------------------------
    void D3D11Texture::_setFinalAttributes(unsigned long width, unsigned long height, 
        unsigned long depth, PixelFormat format, UINT miscflags)
    { 
        // set target texture attributes
        mHeight = height; 
        mWidth = width; 
        mDepth = depth;
        mFormat = format;
        mAutoMipMapGeneration = miscflags & D3D11_RESOURCE_MISC_GENERATE_MIPS;

        // Update size (the final size, including temp space because in consumed memory)
        // this is needed in Resource class
        mSize = calculateSize();

        // say to the world what we are doing
        if (mWidth != mSrcWidth ||
            mHeight != mSrcHeight)
        {
            LogManager::getSingleton().logMessage("D3D11: ***** Dimensions altered by the render system");
            LogManager::getSingleton().logMessage("D3D11: ***** Source image dimensions : " + StrConv::conv(mSrcWidth) + "x" + StrConv::conv(mSrcHeight));
            LogManager::getSingleton().logMessage("D3D11: ***** Texture dimensions : " + StrConv::conv(mWidth) + "x" + StrConv::conv(mHeight));
        }

        // Create list of subsurfaces for getBuffer()
        _createSurfaceList();
    }
    //---------------------------------------------------------------------
    void D3D11Texture::_setSrcAttributes(unsigned long width, unsigned long height, 
        unsigned long depth, PixelFormat format)
    { 
        // set source image attributes
        mSrcWidth = width; 
        mSrcHeight = height; 
        mSrcDepth = depth;
        mSrcFormat = format;
    }
    //---------------------------------------------------------------------
    void D3D11Texture::_createSurfaceList(void)
    {
        // Create new list of surfaces
        mSurfaceList.clear();
        size_t depth = mDepth;

        for(size_t face=0; face<getNumFaces(); ++face)
        {
            size_t width = mWidth;
            size_t height = mHeight;
            for(size_t mip=0; mip<=mNumMipmaps; ++mip)
            { 

                D3D11FrameBuffer *buffer;
                buffer = new D3D11FrameBuffer(
                    this, // parentTexture
                    mDevice, // device
                    mip, 
                    width, 
                    height, 
                    depth,
                    face,
                    mFormat,
                    (Buffer::Usage)mUsage
                    ); 

                mSurfaceList.push_back(HardwarePixelBufferSharedPtr(buffer));

                if(width > 1) width /= 2;
                if(height > 1) height /= 2;
                if(depth > 1 && getTextureType() != TEX_TYPE_2D_ARRAY) depth /= 2;
            }
        }
    }
    D3D11TextureGpu::D3D11TextureGpu( RecoverType rtype,
                                      GBufferManager *vaoManager, IdString name, uint32 textureFlags,
                                      Texture::Type::Texture::Type initialType,
                                      TextureManager *textureManager ) :
        TextureGpu( rtype, vaoManager, name, textureFlags, initialType, textureManager ),
        mDisplayTextureName( 0 )
    {
        _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    D3D11TextureGpu::~D3D11TextureGpu()
    {
        freeInternalImpl(); // 2
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::create1DTexture(void)
    {
        D3D11_TEXTURE1D_DESC desc;
        memset( &desc, 0, sizeof( desc ) );

        desc.Width      = static_cast<UINT>( mWidth );
        desc.MipLevels  = static_cast<UINT>( mNumMipmaps );
        desc.ArraySize  = static_cast<UINT>( mDepthOrSlices );
        if( isAlterView() || (isTexture() && PixelUtil::isDepth( mPixelFormat )) )
            desc.Format = D3D11Mappings::getFamily( mPixelFormat );
        else
            desc.Format = D3D11Mappings::get( mPixelFormat );
        desc.Usage                  = D3D11_USAGE_DEFAULT;
        desc.BindFlags              = 0;
        desc.CPUAccessFlags         = 0;
        desc.MiscFlags              = 0;

        if( isTexture() )
            desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if( isFrame() )
        {
            if( PixelUtil::isDepth( mPixelFormat ) )
                desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
            else
                desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        }
        if( isBufferStroge() )
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

        if( mTextureType == Texture::T_CUBE || mTextureType == Texture::Type::T_CUBEArray )
            desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        if( isAutoMipmap() )
            desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

        D3D11TextureGpuManager *textureManagerD3d =
                static_cast<D3D11TextureGpuManager*>( mTextureManager );
        D3D11Device &device = textureManagerD3d->getDevice();

        ID3D11Texture1D *texture = 0;
        HRESULT hr = device->CreateTexture1D( &desc, 0, &texture );
        mFinalTextureName.Attach( texture );

        if( FAILED(hr) || device.isError() )
        {
            this->freeInternalImpl(); // 2
            //TODO: Should we call this->load( StorageType::ST_Disk, 0 );
            //instead? But doing such thing is problematic because we're already
            //inside load
            String errorDescription = device.getErrorString( hr );
            N_EXCEPT_EX( RenderingAPI, hr,
                            "Texture '" + getName() + "': "
                            "Error creating texture\nError Description:" + errorDescription,
                            "D3D11TextureGpu::create1DTexture" );
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::create2DTexture( bool msaaTextureOnly /* = false */)
    {
        D3D11_TEXTURE2D_DESC desc;
        memset( &desc, 0, sizeof( desc ) );

        desc.Width      = static_cast<UINT>( mWidth );
        desc.Height     = static_cast<UINT>( mHeight );
        desc.MipLevels  = static_cast<UINT>( mNumMipmaps );
        desc.ArraySize  = static_cast<UINT>( mDepthOrSlices );
        if( isAlterView() || (isTexture() && PixelUtil::isDepth( mPixelFormat )) )
            desc.Format = D3D11Mappings::getFamily( mPixelFormat );
        else
            desc.Format = D3D11Mappings::get( mPixelFormat );
        if( isMultiSample() && isManualMSAA() )
        {
            desc.SampleDesc.Count   = mSrcSample.getMultiSampling();
            desc.SampleDesc.Quality = mSrcSample.getCoverageSampling()
                                          ? mSrcSample.getCoverageSampling()
                                          : D3D11Mappings::get( mSrcSample.getType() );
        }
        else
        {
            desc.SampleDesc.Count   = 1u;
            desc.SampleDesc.Quality = 0;
        }
        desc.Usage                  = D3D11_USAGE_DEFAULT;
        desc.BindFlags              = 0;
        desc.CPUAccessFlags         = 0;
        desc.MiscFlags              = 0;

        if( isTexture() )
            desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if( isFrame() )
        {
            if( PixelUtil::isDepth( mPixelFormat ) )
                desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
            else
                desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        }
        if( isBufferStroge() )
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

        if( mTextureType == Texture::T_CUBE || mTextureType == Texture::Type::T_CUBEArray )
            desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        if( isAutoMipmap() )
            desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

        D3D11TextureGpuManager *textureManagerD3d =
                static_cast<D3D11TextureGpuManager*>( mTextureManager );
        D3D11Device &device = textureManagerD3d->getDevice();

        ID3D11Texture2D *texture = 0;
        HRESULT hr = S_FALSE;
        if( !msaaTextureOnly )
        {
            hr = device->CreateTexture2D(&desc, 0, &texture);
            mFinalTextureName.Attach( texture );
        }

        if( FAILED(hr) || device.isError() )
        {
            this->freeInternalImpl(); //2
            //TODO: Should we call this->load( StorageType::ST_Disk, 0 );
            //instead? But doing such thing is problematic because we're already
            //inside load
            String errorDescription = device.getErrorString( hr );
            N_EXCEPT_EX( RenderingAPI, hr,
                            "Texture '" + getName() + "': "
                            "Error creating texture\nError Description:" + errorDescription,
                            "D3D11TextureGpu::create2DTexture" );
        }

        if( isMultiSample() && !isManualMSAA() )
        {
            //We just created the resolve texture. Must create the actual MSAA surface now.
            desc.SampleDesc.Count = mSrcSample.getMultiSampling();
            desc.SampleDesc.Quality = mSrcSample.getCoverageSampling()
                                          ? mSrcSample.getCoverageSampling()
                                          : D3D11Mappings::get( mSrcSample.getType() );

            //Reset bind flags. We won't bind it as SRV, allows more aggressive
            //optimizations on AMD cards (DCC - Delta Color Compression since GCN 1.2)
            if( PixelUtil::isDepth( mPixelFormat ) )
                desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            else
                desc.BindFlags = D3D11_BIND_RENDER_TARGET;
            desc.MiscFlags = 0;

            texture = 0;
            hr = device->CreateTexture2D( &desc, 0, &texture );
            mMsaaFramebufferName.Attach( texture );

            if( FAILED(hr) || device.isError() )
            {
                freeInternalImpl(); // 2
                //TODO: Should we call this->load( StorageType::ST_Disk, 0 );
                //instead? But doing such thing is problematic because we're already
                //inside load
                String errorDescription = device.getErrorString( hr );
                N_EXCEPT_EX( RenderingAPI, hr,
                                "Texture '" + getName() + "': "
                                "Error creating MSAA surface\nError Description:" + errorDescription,
                                "D3D11TextureGpu::create2DTexture" );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::create3DTexture(void)
    {
        D3D11_TEXTURE3D_DESC desc;
        memset( &desc, 0, sizeof( desc ) );

        desc.Width      = static_cast<UINT>( mWidth );
        desc.Height     = static_cast<UINT>( mHeight );
        desc.Depth      = static_cast<UINT>( mDepthOrSlices );
        desc.MipLevels  = static_cast<UINT>( mNumMipmaps );
        if( isAlterView() || (isTexture() && PixelUtil::isDepth( mPixelFormat )) )
            desc.Format = D3D11Mappings::getFamily( mPixelFormat );
        else
            desc.Format = D3D11Mappings::get( mPixelFormat );
        desc.Usage                  = D3D11_USAGE_DEFAULT;
        desc.BindFlags              = 0;
        desc.CPUAccessFlags         = 0;
        desc.MiscFlags              = 0;

        if( isTexture() )
            desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if( isFrame() )
        {
            if( PixelUtil::isDepth( mPixelFormat ) )
                desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
            else
                desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        }
        if( isBufferStroge() )
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

        if( mTextureType == Texture::T_CUBE || mTextureType == Texture::Type::T_CUBEArray )
            desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        if( isAutoMipmap() )
        {
            desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
            desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        }

        D3D11TextureGpuManager *textureManagerD3d =
                static_cast<D3D11TextureGpuManager*>( mTextureManager );
        D3D11Device &device = textureManagerD3d->getDevice();

        ID3D11Texture3D *texture = 0;
        HRESULT hr = device->CreateTexture3D( &desc, 0, &texture );
        mFinalTextureName.Attach( texture );

        if( FAILED(hr) || device.isError() )
        {
            freeInternalImpl(); // 2
            //TODO: Should we call this->load( StorageType::ST_Disk, 0 );
            //instead? But doing such thing is problematic because we're already
            //inside load
            String errorDescription = device.getErrorString( hr );
            N_EXCEPT_EX( RenderingAPI, hr,
                            "Texture '" + getName() + "': "
                            "Error creating texture\nError Description:" + errorDescription,
                            "D3D11TextureGpu::create2DTexture" );
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::createInternalImpl(void)
    {
        if( mPixelFormat == PFG_NULL )
            return; //Nothing to do

        switch( mTextureType )
        {
        case Texture::T_Unknow:
            N_EXCEPT( Exception::ERR_INVALID_STATE, "Texture '" + getName() + "': Ogre should never hit this path" );
            break;
        case Texture::T_1D:
        case Texture::T_1DArray:
            create1DTexture();
            break;
        case Texture::T_2D:
        case Texture::T_2DArray:
        case Texture::T_CUBE:
        case Texture::Type::T_CUBEArray:
            create2DTexture();
            break;
        case Texture::T_3D:
            create3DTexture();
            break;
        }

        //Set debug name for RenderDoc and similar tools
        const String texName = getName();
        mFinalTextureName->SetPrivateData( WKPDID_D3DDebugObjectName,
                                           static_cast<UINT>( texName.size() ), texName.c_str() );
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::freeInternalImpl(void) //2
    {
        mDefaultDisplaySrv.Reset();

        if( isPoolSlice() && mPool )
        {
            //This will end up calling poolImpl,
            //setting mPool & mPoolArray to 0
            mTextureManager->poolFree( this );
        }
        mFinalTextureName.Reset();
        mMsaaFramebufferName.Reset();

        _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::notifyGpuData(void)
    {
        N_assert( mCurrentStorage == StorageType::ST_Gpu );
        N_assert( mFinalTextureName || mPixelFormat == PFG_NULL );

        mDefaultDisplaySrv.Reset();

        mDisplayTextureName = mFinalTextureName.Get();
        if( isTexture() )
        {
            ShaderChTexture::TextureSlot texSlot( ShaderChTexture::TextureSlot::
                                                        zero() );
            mDefaultDisplaySrv = createSrv( texSlot );
        }

        process( Texture::Process::O_Valid );
    }
    //-----------------------------------------------------------------------------------
    bool D3D11TextureGpu::isValidImpl(void) const
    {
        return mDisplayTextureName == mFinalTextureName.Get();
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::_setToDisplayDummyTexture(void)
    {
        mDefaultDisplaySrv.Reset();

        if( !mTextureManager )
        {
            N_assert( isWindow() );
            return; //This can happen if we're a window and we're on shutdown
        }

        D3D11TextureGpuManager * textureManagerD3d = static_cast<D3D11TextureGpuManager *>( mTextureManager );
        if( isPoolSlice() )
        {
            mDisplayTextureName = textureManagerD3d->getBlankTextureD3dName( Texture::T_2DArray );
            if( isTexture() )
            {
                mDefaultDisplaySrv = textureManagerD3d->getBlankTextureSrv( Texture::T_2DArray );
            }
        }
        else
        {
            mDisplayTextureName = textureManagerD3d->getBlankTextureD3dName( mTextureType );
            if( isTexture() )
            {
                mDefaultDisplaySrv = textureManagerD3d->getBlankTextureSrv( mTextureType );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::poolImpl( const TexturePool * newPool, uint16 slice )
    {
        TextureGpu::poolImpl( newPool, slice );

        _setToDisplayDummyTexture();

        if( mPool )
        {
            N_assert( dynamic_cast<D3D11TextureGpu*>( mPool->mPool ) );
            D3D11TextureGpu *tex = static_cast<D3D11TextureGpu*>(mPool->mPool);
            mFinalTextureName = tex->mFinalTextureName;
        }

        process( Texture::Process::O_PoolAlter );
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::setTextureType( Texture::Type::Texture::Type textureType )
    {
        const Texture::Type::Texture::Type oldType = mTextureType;
        TextureGpu::setTextureType( textureType );

        if( oldType != mTextureType && mDisplayTextureName != mFinalTextureName.Get() )
            _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::read( TextureGpu *dst, const PixelBlock &dstBox, uint8 dstMipLevel,
                                  const PixelBlock &srcBox, uint8 srcMipLevel,
                                  bool keepResolvedTexSynced )
    {
        TextureGpu::read( dst, dstBox, dstMipLevel, srcBox, srcMipLevel );

        N_assert( dynamic_cast<D3D11TextureGpu*>( dst ) );
        D3D11TextureGpu *dstD3d = static_cast<D3D11TextureGpu*>( dst );

        D3D11_BOX d3dBox;
        d3dBox.front    = srcBox.z;
        d3dBox.back     = srcBox.depth;
        d3dBox.top      = srcBox.y;
        d3dBox.bottom   = srcBox.height;
        d3dBox.left     = srcBox.x;
        d3dBox.right    = srcBox.width;

        D3D11_BOX *d3dBoxPtr = &d3dBox;

        if( srcBox.equalSize( this->getEmptyBox( srcMipLevel ) ) )
            d3dBoxPtr = 0;

        ID3D11Resource *srcTextureName = this->mFinalTextureName.Get();
        ID3D11Resource *dstTextureName = dstD3d->mFinalTextureName.Get();

        if( this->isMultiSample() && !this->isManualMSAA() )
            srcTextureName = this->mMsaaFramebufferName.Get();
        if( dstD3d->isMultiSample() && !dstD3d->isManualMSAA() )
            dstTextureName = dstD3d->mMsaaFramebufferName.Get();

        D3D11TextureGpuManager *textureManagerD3d =
                static_cast<D3D11TextureGpuManager*>( mTextureManager );
        D3D11Device &device = textureManagerD3d->getDevice();
        ID3D11DeviceContextN *context = device.GetImmediateContext();

        DXGI_FORMAT format = D3D11Mappings::get( dstD3d->getFormat() );

        for( size_t i=0; i<srcBox.mArray; ++i )
        {
            const UINT srcResourceIndex = D3D11CalcSubresource( srcMipLevel, srcBox.mArrayIdx + i +
                                                                this->getPoolArray(),
                                                                this->mNumMipmaps );
            const UINT dstResourceIndex = D3D11CalcSubresource( dstMipLevel, dstBox.mArrayIdx + i +
                                                                dstD3d->getPoolArray(),
                                                                dstD3d->mNumMipmaps );
            context->CopySubresourceRegion( dstTextureName, dstResourceIndex,
                                            dstBox.x, dstBox.y, dstBox.z,
                                            srcTextureName, srcResourceIndex,
                                            d3dBoxPtr );

            if( dstD3d->isMultiSample() && !dstD3d->isManualMSAA() && keepResolvedTexSynced )
            {
                //Must keep the resolved texture up to date.
                context->ResolveSubresource( dstD3d->mFinalTextureName.Get(),
                                             dstResourceIndex,
                                             dstD3d->mMsaaFramebufferName.Get(),
                                             dstResourceIndex,
                                             format );
            }
        }

        if( device.isError() )
        {
            String errorDescription = device.getErrorString();
            N_EXCEPT( RenderingAPI, "Texture '" + getName() + "': "
                         "Error copying and/or resolving texture\nError Description:" + errorDescription);
        }

        //Do not perform the sync if notifyGpuData hasn't been called yet (i.e. we're
        //still building the HW mipmaps, and the texture will never be ready)
        if( dst->isValidImpl() &&
            dst->getRecoverType() == Texture::RT_CpuGpu )
        {
            dst->downloadImpl();
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::mipmap(void)
    {
        if( !mFinalTextureName || !isValid() )
            return;

        D3D11TextureGpuManager * textureManagerD3d = static_cast<D3D11TextureGpuManager *>(mTextureManager);
        D3D11Device & device = textureManagerD3d->getDevice();
        device.GetImmediateContext()->GenerateMips( mDefaultDisplaySrv.Get() );
    }
    //-----------------------------------------------------------------------------------
    ComPtr<ID3D11ShaderResourceView> D3D11TextureGpu::createSrv(
            const ShaderChTexture::TextureSlot &texSlot ) const
    {
        N_assert( isTexture() &&
                "This texture is marked as 'Texture::MM_FRAME', which "
                "means it can't be used for reading as a regular texture." );

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        D3D11_SHADER_RESOURCE_VIEW_DESC *srvDescPtr = 0;

        PixelFormat format = texSlot.pf;
        if( format == PF_UNKNOWN )
            format = mPixelFormat;

        if( format != mPixelFormat ||
            texSlot.cubemapsAs2DArrays ||
            texSlot.mipmapLevel > 0 ||
            texSlot.numMipmaps != 0 ||
            texSlot.textureArrayIndex > 0 ||
            (getTextureType() == Texture::T_2DArray && mDepthOrSlices == 1u) ||
            (mPool && mPool->mPool->getArray() == 1u) ||
            isAlterView() ||
            PixelUtil::isDepth( mPixelFormat ) )
        {
            memset( &srvDesc, 0, sizeof(srvDesc) );

            srvDesc.Format = D3D11Mappings::getForSrv( format );

            const Texture::Type::Texture::Type textureType = getActualType();

            const bool isMsaaSrv = isMultiSample() && isManualMSAA();
            srvDesc.ViewDimension = D3D11Mappings::get( textureType, texSlot.cubemapsAs2DArrays,
                                                        isMsaaSrv );

            if( isMsaaSrv )
            {
                srvDesc.Texture2DMSArray.FirstArraySlice    = texSlot.textureArrayIndex;
                srvDesc.Texture2DMSArray.ArraySize          = mDepthOrSlices - texSlot.textureArrayIndex;
            }
            else if( textureType == Texture::Type::T_CUBEArray && !texSlot.cubemapsAs2DArrays )
            {
                //Write to all elements due to C++ aliasing rules on the union.
                srvDesc.TextureCubeArray.MostDetailedMip    = texSlot.mipmapLevel;
                srvDesc.TextureCubeArray.MipLevels          = mNumMipmaps - texSlot.mipmapLevel;
                srvDesc.TextureCubeArray.First2DArrayFace   = texSlot.textureArrayIndex;
                srvDesc.TextureCubeArray.NumCubes           = (mDepthOrSlices -
                                                               texSlot.textureArrayIndex) / 6u;
            }
            else
            {
                //It's a union, so 2DArray == everyone else.
                uint8 numMipmaps = texSlot.numMipmaps;
                if( !texSlot.numMipmaps )
                    numMipmaps = mNumMipmaps - texSlot.mipmapLevel;

                N_assert( numMipmaps <= mNumMipmaps - texSlot.mipmapLevel &&
                                 "Asking for more mipmaps than the texture has!" );

                srvDesc.Texture2DArray.MostDetailedMip  = texSlot.mipmapLevel;
                srvDesc.Texture2DArray.MipLevels        = numMipmaps;

                if( textureType == Texture::T_1DArray ||
                    textureType == Texture::T_2DArray )
                {
                    srvDesc.Texture2DArray.FirstArraySlice  = texSlot.textureArrayIndex;
                    srvDesc.Texture2DArray.ArraySize        = mDepthOrSlices - texSlot.textureArrayIndex;
                }
            }

            srvDescPtr = &srvDesc;
        }

        D3D11TextureGpuManager *textureManagerD3d =
                static_cast<D3D11TextureGpuManager*>( mTextureManager );
        D3D11Device &device = textureManagerD3d->getDevice();
        ComPtr<ID3D11ShaderResourceView> retVal;
        HRESULT hr = device->CreateShaderResourceView( mDisplayTextureName, srvDescPtr, retVal.GetAddressOf() );
        if( FAILED(hr) || device.isError() )
        {
            String errorDescription = device.getErrorString( hr );
            N_EXCEPT_EX( RenderingAPI, hr,
                            "Texture '" + getName() + "': "
                            "Error creating ShaderResourceView\nError Description:" + errorDescription,
                            "D3D11TextureGpu::createSrv" );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    ComPtr<ID3D11ShaderResourceView> D3D11TextureGpu::createSrv(void) const
    {
        N_assert( isTexture() &&
                "This texture is marked as 'Texture::MM_FRAME', which "
                "means it can't be used for reading as a regular texture." );
        N_assert( mDefaultDisplaySrv.Get() &&
                "Either the texture wasn't properly loaded or _setToDisplayDummyTexture "
                "wasn't called when it should have been" );

        return mDefaultDisplaySrv;
    }
    //-----------------------------------------------------------------------------------
    ComPtr<ID3D11UnorderedAccessView> D3D11TextureGpu::createUav(const DescriptorSetUav::TextureSlot &texSlot ) const
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        memset( &uavDesc, 0, sizeof( uavDesc ) );

        PixelFormat finalFormat;
        if( texSlot.pf == PF_UNKNOWN )
            finalFormat = PixelUtil::getLinear( mPixelFormat );
        else
            finalFormat = texSlot.pf;

        uavDesc.Format = D3D11Mappings::get( finalFormat );

        switch( mTextureType )
        {
        case Texture::T_1D:
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
            uavDesc.Texture1D.MipSlice = static_cast<UINT>( texSlot.mipmapLevel );
            break;
        case Texture::T_1DArray:
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
            uavDesc.Texture1DArray.MipSlice         = static_cast<UINT>( texSlot.mipmapLevel );
            uavDesc.Texture1DArray.FirstArraySlice  = static_cast<UINT>( texSlot.textureArrayIndex );
            uavDesc.Texture1DArray.ArraySize        = static_cast<UINT>( getArray() - texSlot.textureArrayIndex );
            break;
        case Texture::T_2D:
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = static_cast<UINT>( texSlot.mipmapLevel );
            break;
        case Texture::T_2DArray:
        case Texture::T_CUBE:
        case Texture::Type::T_CUBEArray:
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
            uavDesc.Texture2DArray.MipSlice         = static_cast<UINT>( texSlot.mipmapLevel );
            uavDesc.Texture2DArray.FirstArraySlice  = texSlot.textureArrayIndex;
            uavDesc.Texture2DArray.ArraySize        = static_cast<UINT>( getArray() - texSlot.textureArrayIndex );
            break;
        case Texture::T_3D:
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
            uavDesc.Texture3D.MipSlice      = static_cast<UINT>( texSlot.mipmapLevel );
            uavDesc.Texture3D.FirstWSlice   = 0;
            uavDesc.Texture3D.WSize         = static_cast<UINT>( getDepth() >> texSlot.mipmapLevel );
            break;
        default:
            break;
        }

        D3D11TextureGpuManager *textureManagerD3d = static_cast<D3D11TextureGpuManager*>( mTextureManager );
        D3D11Device &device = textureManagerD3d->getDevice();

        ComPtr<ID3D11UnorderedAccessView> retVal;
        HRESULT hr = device->CreateUnorderedAccessView( mFinalTextureName.Get(), &uavDesc, retVal.GetAddressOf() );
        if( FAILED(hr) )
        {
            String errorDescription = device.getErrorString(hr);
            N_EXCEPT_EX( RenderingAPI, hr,
                            "Failed to create UAV view on texture '" + this->getName() +
                            "'\nError Description: " + errorDescription,
                            "D3D11TextureGpu::createUav" );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    bool D3D11TextureGpu::isSupport( CoordType::CoordType pattern )
    {
        return pattern != CoordType::CT_Standard;
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::getMultiSampleCoord(Vector2List & locations )
    {
        locations.reserve( mSrcSample.getMultiSampling() );
        if( mSrcSample.getMultiSampling() <= 1u )
        {
            locations.push_back( Vector2( 0.0f, 0.0f ) );
        }
        else
        {
            N_assert( mSrcSample.getType() != CoordType::CT_Unknow );

            if( mSrcSample.getType() == CoordType::CT_Default )
            {
                //As defined per D3D11_STANDARD_MULTISAMPLE_PATTERN docs.
                switch( mSrcSample.getMultiSampling() )
                {
                case 2:
                    locations.push_back( Vector2( Real(  4.0 / 8.0 ), Real(  4.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -4.0 / 8.0 ), Real( -4.0 / 8.0 ) ) );
                    break;
                case 4:
                    locations.push_back( Vector2( Real( -2.0 / 8.0 ), Real( -6.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  6.0 / 8.0 ), Real( -2.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -6.0 / 8.0 ), Real(  2.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  2.0 / 8.0 ), Real(  6.0 / 8.0 ) ) );
                    break;
                case 8:
                    locations.push_back( Vector2( Real(  1.0 / 8.0 ), Real( -3.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -1.0 / 8.0 ), Real(  3.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  5.0 / 8.0 ), Real(  1.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -3.0 / 8.0 ), Real( -5.0 / 8.0 ) ) );

                    locations.push_back( Vector2( Real( -5.0 / 8.0 ), Real(  5.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -7.0 / 8.0 ), Real( -1.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  3.0 / 8.0 ), Real(  7.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  7.0 / 8.0 ), Real( -7.0 / 8.0 ) ) );
                    break;
                case 16:
                    locations.push_back( Vector2( Real(  1.0 / 8.0 ), Real(  1.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -1.0 / 8.0 ), Real( -3.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -3.0 / 8.0 ), Real(  2.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  4.0 / 8.0 ), Real( -1.0 / 8.0 ) ) );

                    locations.push_back( Vector2( Real( -5.0 / 8.0 ), Real( -2.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  2.0 / 8.0 ), Real(  5.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  5.0 / 8.0 ), Real(  3.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  3.0 / 8.0 ), Real( -5.0 / 8.0 ) ) );

                    locations.push_back( Vector2( Real( -2.0 / 8.0 ), Real(  6.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  0.0 / 8.0 ), Real( -7.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -4.0 / 8.0 ), Real( -6.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -6.0 / 8.0 ), Real(  4.0 / 8.0 ) ) );

                    locations.push_back( Vector2( Real( -8.0 / 8.0 ), Real(  0.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  7.0 / 8.0 ), Real( -4.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real(  6.0 / 8.0 ), Real(  7.0 / 8.0 ) ) );
                    locations.push_back( Vector2( Real( -7.0 / 8.0 ), Real( -8.0 / 8.0 ) ) );
                    break;
                }
            }
            else
            {
                //Center
                for( uint8 i=0; i<mSrcSample.getMultiSampling(); ++i )
                    locations.push_back( Vector2( 0, 0 ) );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpu::getPlatformData( IdString name, void *pData )
    {
        if( name == msFinalTextureBuffer || name == "ID3D11Resource" )
        {
            ID3D11Resource **pTex = (ID3D11Resource**)pData;
            *pTex = mFinalTextureName.Get();
        }
        else if( name == msMsaaTextureBuffer )
        {
            ID3D11Resource **pTex = (ID3D11Resource**)pData;
            *pTex = mMsaaFramebufferName.Get();
        }
        else
        {
            TextureGpu::getPlatformData( name, pData );
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    D3D11TextureGpuRenderTarget::D3D11TextureGpuRenderTarget(
            RecoverType rtype,
            GBufferManager *vaoManager, IdString name, uint32 textureFlags,
            Texture::Type::Texture::Type initialType,
            TextureManager *textureManager ) :
        D3D11TextureGpu( rtype, vaoManager, name,
                         textureFlags, initialType, textureManager ),
        mDSPool( 1u ),
        mDSTexture( false ),
        mDSFormat( PF_UNKNOWN )
    {
        if( mPixelFormat == PFG_NULL )
            mDSPool = 0;
    }
    //-----------------------------------------------------------------------------------
    void D3D11TextureGpuRenderTarget::_setDepthBufferDefaults(
            uint16 depthBufferPoolId, bool preferDepthTexture, PixelFormat desiredDepthBufferFormat )
    {
        N_assert( isFrame() );
        mDSPool          = depthBufferPoolId;
        mDSTexture         = preferDepthTexture;
        mDSFormat   = desiredDepthBufferFormat;
    }
    //-----------------------------------------------------------------------------------
    uint16 D3D11TextureGpuRenderTarget::getDepthBufferPoolId(void) const
    {
        return mDSPool;
    }
    //-----------------------------------------------------------------------------------
    bool D3D11TextureGpuRenderTarget::getPreferDepthTexture(void) const
    {
        return mDSTexture;
    }
    //-----------------------------------------------------------------------------------
    PixelFormat D3D11TextureGpuRenderTarget::getDesiredDepthBufferFormat(void) const
    {
        return mDSFormat;
    }
    //---------------------------------------------------------------------
    // D3D11RenderTexture
    //---------------------------------------------------------------------
    void D3D11RenderTexture::rebind( D3D11FrameBuffer *buffer )
    {
        mBuffer = buffer;
        mWidth = (unsigned int) mBuffer->getWidth();
        mHeight = (unsigned int) mBuffer->getHeight();
        mColourDepth = (unsigned int) PixelUtil::getNumElemBits(mBuffer->getFormat());
        
        ID3D11Resource * pBackBuffer = buffer->getParentTexture()->getTextureResource();

        D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
        ZeroMemory( &RTVDesc, sizeof(RTVDesc) );

        RTVDesc.Format = buffer->getParentTexture()->getShaderResourceViewDesc().Format;
        switch(buffer->getParentTexture()->getShaderResourceViewDesc().ViewDimension)
        {
        case D3D11_SRV_DIMENSION_BUFFER:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE1D:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            break;
        case D3D11_SRV_DIMENSION_TEXTURECUBE:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            RTVDesc.Texture2DArray.FirstArraySlice = buffer->getFace();
            RTVDesc.Texture2DArray.ArraySize = 1;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2D:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            RTVDesc.Texture2DArray.FirstArraySlice = mZOffset;
            RTVDesc.Texture2DArray.ArraySize = 1;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMS:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
            RTVDesc.Texture2DArray.FirstArraySlice = mZOffset;
            RTVDesc.Texture2DArray.ArraySize = 1;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE3D:
            RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
            break;
        default:
            N_assert(false);
        }
        HRESULT hr = mDevice->CreateRenderTargetView(pBackBuffer, &RTVDesc, mRenderTargetView.ReleaseAndGetAddressOf());

        if (FAILED(hr) || mDevice.isError())
        {
			String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
				"Error creating Render Target View\nError Description:" + errorDescription,
                "D3D11RenderTexture::rebind" );
        }
    }

    uint D3D11RenderTexture::getNumberOfViews() const { return 1; }

    ID3D11Texture2D* D3D11RenderTexture::getSurface(uint index) const
    {
        return index == 0 ? static_cast<D3D11FrameBuffer*>(mBuffer)->getParentTexture()->GetTex2D()
                          : NULL;
    }

    ID3D11RenderTargetView* D3D11RenderTexture::getRenderTargetView(uint index) const
    {
        return index == 0 ? mRenderTargetView.Get() : NULL;
    }

    D3D11RenderTexture::D3D11RenderTexture( const String &name, D3D11FrameBuffer *buffer, uint32 zoffset, D3D11Device & device )
        : RenderTexture(buffer, zoffset)
        , mDevice(device)
    {
        mName = name;
        rebind(buffer);
    }
    //---------------------------------------------------------------------
    D3D11RenderTexture::~D3D11RenderTexture()
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderTexture::notifyDeviceLost(D3D11Device* device)
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderTexture::notifyDeviceRestored(D3D11Device* device)
    {
        rebind(static_cast<D3D11FrameBuffer*>(mBuffer));
    }
}
