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
#ifndef __D3D11TEXTURE_H__
#define __D3D11TEXTURE_H__

#include "OgreD3D11Prerequisites.h"
#include "OgreD3D11Device.h"
#include "OgreD3D11DeviceResource.h"
#include "OgreD3D11RenderTarget.h"
#include "OgreTexture.h"
#include "OgreRenderTexture.h"
#include "OgreSharedPtr.h"

namespace Ogre 
{
	/** Specialisation of Texture for D3D11 */
    class _OgreD3D11Export D3D11Texture
        : public Texture
        , protected D3D11DeviceResource
    {
	public:
		/// constructor 
		D3D11Texture(ResourceManager* creator, const String& name, ResourceHandle handle,
			const String& group, bool isManual, ManualResourceLoader* loader,
			D3D11Device & device);
		/// destructor
		virtual ~D3D11Texture();
        
        D3D11Texture( RecoverType rtype,
                         GBufferManager *vaoManager, IdString name, uint32 textureFlags,
                         Texture::Type::Texture::Type initialType,
                         TextureManager *textureManager );

		/// overridden from Texture
		void copyToTexture(TexturePtr& target);

		ID3D11Resource *getTextureResource() { N_assert(mpTex); return mpTex.Get(); }
		/// retrieves a pointer to the actual texture
		ID3D11ShaderResourceView *getSrvView() { N_assert(mpShaderResourceView); return mpShaderResourceView.Get(); }
		D3D11_SHADER_RESOURCE_VIEW_DESC getShaderResourceViewDesc() const { return mSRVDesc; }

		ID3D11Texture1D * GetTex1D() { return mp1DTex.Get(); };
		ID3D11Texture2D * GetTex2D() { return mp2DTex.Get(); };
		ID3D11Texture3D	* GetTex3D() { return mp3DTex.Get(); };

		bool HasAutoMipMapGenerationEnabled() const { return mAutoMipMapGeneration; }
        
    public:

        virtual void notifyGpuData(void);
        virtual bool isValidImpl(void) const;

        virtual void setTextureType( Texture::Type::Texture::Type textureType );

        virtual void read( TextureGpu *dst, const PixelBlock &dstBox, uint8 dstMipLevel,
                             const PixelBlock &srcBox, uint8 srcMipLevel,
                             bool keepResolvedTexSynced = true );

        virtual void _setToDisplayDummyTexture(void);
        virtual void poolImpl( const TexturePool *newPool, uint16 slice );

        virtual void mipmap(void);

        //The returned pointer has its ref. count incremented! Caller must decrease it!
        ComPtr<ID3D11ShaderResourceView> createSrv( const ShaderChTexture::TextureSlot &texSlot ) const;
        ComPtr<ID3D11ShaderResourceView> createSrv(void) const;
        ID3D11ShaderResourceView* getDefaultDisplaySrv(void) const  { return mDefaultDisplaySrv.Get(); }

        ComPtr<ID3D11UnorderedAccessView> createUav( const DescriptorSetUav::TextureSlot &texSlot ) const;

        virtual bool isSupport( CoordType::CoordType pattern );
        virtual void getMultiSampleCoord(Vector2List & locations );

        virtual void getPlatformData( IdString name, void *pData );

        ID3D11Resource* getDisplayTextureName(void) const   { return mDisplayTextureName; }
        ID3D11Resource* getFinalTextureName(void) const     { return mFinalTextureName.Get(); }
        ID3D11Resource* getMsaaFramebufferName(void) const  { return mMsaaFramebufferName.Get(); }

	protected:
		TextureUsage _getTextureUsage() { return static_cast<TextureUsage>(mUsage); }

    protected:
        template<typename fromtype, typename totype>
        void _queryInterface(const ComPtr<fromtype>& from, ComPtr<totype> *to)
        {
            HRESULT hr = from.As(to);

            if(FAILED(hr) || mDevice.isError())
            {
                this->freeInternalResources();
				String errorDescription = mDevice.getErrorString(hr);
				N_EXCEPT_EX(RenderingAPI, hr, "Can't get base texture\nError Description:" + errorDescription, 
                    "D3D11Texture::_queryInterface" );
            }
        }
        void _create1DResourceView();
        void _create2DResourceView();
        void _create3DResourceView();

        /// internal method, create a blank normal 1D Dtexture
        void _create1DTex();
        /// internal method, create a blank normal 2D texture
        void _create2DTex();
        /// internal method, create a blank cube texture
        void _create3DTex();

        /// @copydoc Texture::createInternalResources
        void createInternalResources(void);
        /// @copydoc Texture::createInternalImpl
        void createInternalImpl(void);
        /// @copydoc Texture::freeInternalResources
        void freeInternalResources(void);
        /// free internal resources
        void freeInternalImpl(void);
        /// internal method, set Texture class source image protected attributes
        void _setSrcAttributes(unsigned long width, unsigned long height, unsigned long depth, PixelFormat format);
        /// internal method, set Texture class final texture protected attributes
        void _setFinalAttributes(unsigned long width, unsigned long height, unsigned long depth, PixelFormat format, UINT miscflags);

        /// internal method, create D3D11HardwarePixelBuffers for every face and
        /// mipmap level. This method must be called after the D3D texture object was created
        void _createSurfaceList(void);

        void notifyDeviceLost(D3D11Device* device);
        void notifyDeviceRestored(D3D11Device* device);

        /// overridden from Resource
        void loadImpl();

    protected:
        D3D11Device&	mDevice;

        DXGI_FORMAT mD3DFormat;         // Effective pixel format, already gamma corrected if requested
        DXGI_SAMPLE_DESC mFSAAType;     // Effective FSAA mode, limited by hardware capabilities

        // device depended resources
        ComPtr<ID3D11Resource> mpTex;   // actual texture
        ComPtr<ID3D11ShaderResourceView> mpShaderResourceView;
        ComPtr<ID3D11Texture1D> mp1DTex;
        ComPtr<ID3D11Texture2D> mp2DTex;
        ComPtr<ID3D11Texture3D> mp3DTex;

        D3D11_SHADER_RESOURCE_VIEW_DESC mSRVDesc;
        bool mAutoMipMapGeneration;
        
        /// The general case is that the whole D3D11 texture will be accessed through the SRV.
        /// That means: createSrv( this->getFormat(), false );
        /// To avoid creating multiple unnecessary copies of the SRV, we keep a cache of that
        /// default SRV with us; and calling createSrv with default params will return
        /// this cache instead.
        ComPtr<ID3D11ShaderResourceView> mDefaultDisplaySrv;

        /// This will not be owned by us if isPoolSlice is true.
        /// It will also not be owned by us if we're not in StorageType::ST_Gpu
        /// This will always point to:
        ///     * A D3D11 SRV owned by us.
        ///     * A 4x4 dummy texture (now owned by us).
        ///     * A 64x64 mipmapped texture of us (but now owned by us).
        ///     * A D3D11 SRV not owned by us, but contains the final information.
        ID3D11Resource *mDisplayTextureName;

        /// When we're transitioning to StorageType::ST_Gpu but we're not there yet,
        /// we will be either displaying a 4x4 dummy texture or a 64x64 one. However
        /// we reserve a spot to a final place will already be there for us once the
        /// texture data is fully uploaded. This variable contains that texture.
        /// Async upload operations should stack to this variable.
        /// May contain:
        ///     1. 0, if not resident or resident but not yet reached main thread.
        ///     2. The texture
        ///     3. An msaa texture (isManualMSAA == true)
        ///     4. The msaa resolved texture (isManualMSAA==false)
        ComPtr<ID3D11Resource> mFinalTextureName;


        /// Only used when isManualMSAA() == false.
        ComPtr<ID3D11Resource> mMsaaFramebufferName;

        void create1DTexture(void);
        void create2DTexture( bool msaaTextureOnly = false );
        void create3DTexture(void);

        virtual void createInternalImpl(void);
        virtual void freeInternalImpl(void);
    };

    /// RenderTexture implementation for D3D11
    class _OgreD3D11Export D3D11RenderTexture
        : public RenderTexture, public D3D11RenderTarget
        , protected D3D11DeviceResource
    {
        D3D11Device & mDevice;
        ComPtr<ID3D11RenderTargetView> mRenderTargetView;
    public:
        D3D11RenderTexture(const String &name, D3D11FrameBuffer *buffer, uint32 zoffset, D3D11Device & device);
        virtual ~D3D11RenderTexture();

        void rebind(D3D11FrameBuffer *buffer);

        virtual uint getNumberOfViews() const;
        virtual ID3D11Texture2D* getSurface(uint index = 0) const;
        virtual ID3D11RenderTargetView* getRenderTargetView(uint index = 0) const;

        bool isFlipping() const { return false; }

    protected:
        void notifyDeviceLost(D3D11Device* device);
        void notifyDeviceRestored(D3D11Device* device);
    };
    
    class _OgreD3D11Export D3D11TextureGpuRenderTarget : public D3D11Texture
    {
    protected:
        uint16          mDSPool;
        bool            mDSTexture;
        PixelFormat  mDSFormat;

    public:
        D3D11TextureGpuRenderTarget( RecoverType rtype,
                                     GBufferManager *vaoManager, IdString name, uint32 textureFlags,
                                     Texture::Type::Texture::Type initialType,
                                     TextureManager *textureManager );

        virtual void _setDepthBufferDefaults( uint16 depthBufferPoolId, bool preferDepthTexture,
                                              PixelFormat desiredDepthBufferFormat );
        virtual uint16 getDepthBufferPoolId(void) const;
        virtual bool getPreferDepthTexture(void) const;
        virtual PixelFormat getDesiredDepthBufferFormat(void) const;
    };

}

#endif
