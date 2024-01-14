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
#ifndef __D3D11TEXTUREMANAGER_H__
#define __D3D11TEXTUREMANAGER_H__

#include "OgreD3D11Prerequisites.h"
#include "OgreTextureManager.h"

namespace Ogre 
{
    class _OgreD3D11Export D3D11TextureManager : public TextureManager
    {
    public:
        D3D11TextureManager( D3D11Device & device );
        ~D3D11TextureManager();

        /// @copydoc TextureManager::getNativeFormat
        PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage);
        
        D3D11TextureGpuManager( GBufferManager *vaoManager, RenderSystem *renderSystem,
                                D3D11Device &device );
        virtual ~D3D11TextureGpuManager();

        /** Creates a special D3D11TextureGpuWindow pointer, to be used by Ogre::Window.
            The pointer can be freed by a regular N_delete. We do not track this pointer.
            If caller doesnt' delete it, it will leak.
        */
        TextureGpu* createTextureGpuWindow( bool fromFlipModeSwapchain, Window *window );
        TextureGpu* createWindowDepthBuffer(void);

        ID3D11Resource* getBlankTextureD3dName( Texture::Type::Texture::Type textureType ) const;
        ID3D11ShaderResourceView* getBlankTextureSrv( Texture::Type::Texture::Type textureType ) const;

        D3D11Device& getDevice(void)        { return mDevice; }
        
    protected:
        D3D11Device & mDevice;
        /// @copydoc ResourceManager::createImpl
        Resource* createImpl(const String& name, ResourceHandle handle, 
            const String& group, bool isManual, ManualResourceLoader* loader, 
            const PropertyData* createParams);

        /// 4x4 texture for when we have nothing to display.
        ComPtr<ID3D11Resource>              mBlankTexture[Texture::T_3D + 1u];
        ComPtr<ID3D11ShaderResourceView>    mDefaultSrv[Texture::T_3D + 1u];

        D3D11Device &mDevice;

        virtual TextureGpu* createTextureImpl( RecoverType rtype,
                                               IdString name, uint32 textureFlags,
                                               Texture::Type::Texture::Type initialType );
        virtual MappedTexture* createMappedImpl(const PixelVolume & vol );
        virtual void destroyMappedImpl( MappedTexture *stagingTexture );

        virtual TextureAsync* createAsyncImpl (uint32 width, uint32 height,
                                                                  uint32 depthOrSlices,
                                                                  Texture::Type::Texture::Type textureType,
                                                                  PixelFormat pixelFormatFamily );

    };
}
#endif
