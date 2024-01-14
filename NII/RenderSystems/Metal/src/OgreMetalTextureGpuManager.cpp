/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2017 Torus Knot Software Ltd

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

#include "OgreMetalTextureGpuManager.h"
#include "OgreMetalMappings.h"
#include "OgreMetalTextureGpu.h"
#include "OgreMetalStagingTexture.h"
#include "OgreMetalAsyncTextureTicket.h"
#include "OgreMetalTextureGpuWindow.h"

#include "Vao/OgreMetalGBufferManager.h"

#include "OgreMetalDevice.h"

#include "OgrePixelFormatGpuUtils.h"
#include "OgreVector2.h"

#include "OgreException.h"

namespace Ogre
{
    MetalTextureGpuManager::MetalTextureGpuManager( GBufferManager *vaoManager, RenderSystem *renderSystem,
                                                    MetalDevice *device ) :
        TextureManager( vaoManager, renderSystem ),
        mDevice( device )
    {
        MTLTextureDescriptor *desc = [MTLTextureDescriptor new];
        desc.mipmapLevelCount   = 1u;
        desc.width              = 4u;
        desc.height             = 4u;
        desc.depth              = 1u;
        desc.arrayLength        = 1u;
        desc.pf        = MTLPixelFormatRGBA8Unorm;
        desc.sampleCount        = 1u;
        desc.usage = MTLTextureUsageShaderRead;

        const MTLTextureType textureTypes[] =
        {
            MTLTextureType2D,
            MTLTextureType1D,
            MTLTextureType1DArray,
            MTLTextureType2D,
            MTLTextureType2DArray,
            MTLTextureTypeCube,
    #if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
            MTLTextureTypeCubeArray,
    #else
            MTLTextureTypeCube,
    #endif
            MTLTextureType3D
        };

        const char *dummyNames[] =
        {
            "Dummy Unknown (2D)",
            "Dummy 1D 4x1",
            "Dummy 1DArray 4x1",
            "Dummy 2D 4x4",
            "Dummy 2DArray 4x4x1",
            "Dummy Cube 4x4",
    #if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
            "Dummy CubeArray 4x4x1",
    #else
            "Dummy CubeArray 4x4x1 (Emulated w/ Cube)",
    #endif
            "Dummy 3D 4x4x4"
        };

        //Must be large enough to hold the biggest transfer we'll do.
        uint8 c_whiteData[4*4*6*4];
        uint8 c_blackData[4*4*6*4];
        memset( c_whiteData, 0xff, sizeof( c_whiteData ) );
        memset( c_blackData, 0x00, sizeof( c_blackData ) );

        for( int i=1; i<=Texture::T_3D; ++i )
        {
            if( i == Texture::T_3D )
                desc.depth = 4u;
            else
                desc.depth = 1u;
            if( i == Texture::T_1D || i == Texture::T_1DArray )
                desc.height = 1u;
            else
                desc.height = 4u;

            desc.textureType = textureTypes[i];
            mBlankTexture[i] = [device->mDevice newTextureWithDescriptor:desc];
            if( !mBlankTexture[i] )
            {
                N_EXCEPT( RenderingAPI,
                             "Out of GPU memory or driver refused.",
                             "MetalTextureGpuManager::MetalTextureGpuManager" );
            }
            mBlankTexture[i].label = [NSString stringWithUTF8String:dummyNames[i]];

            if( i == Texture::T_CUBE || i == Texture::Type::T_CUBEArray )
            {
                for( uint32 j=0; j<6u; ++j )
                {
                    [mBlankTexture[i] replaceRegion:MTLRegionMake2D( 0, 0, 4u, 4u )
                                        mipmapLevel:0
                                              slice:j
                                          withBytes:c_blackData
                                        mRowSize:4u*4u
                                      mSliceSize:4u*4u*4u];
                }
            }
            else
            {
                [mBlankTexture[i] replaceRegion:MTLRegionMake3D( 0, 0, 0, 4u, desc.height, desc.depth )
                                    mipmapLevel:0
                                          slice:0
                                      withBytes:c_whiteData
                                    mRowSize:4u*4u
                                  mSliceSize:4u*4u*4u];
            }
        }

        mBlankTexture[Texture::T_Unknow] = mBlankTexture[Texture::T_2D];
    }
    //-----------------------------------------------------------------------------------
    MetalTextureGpuManager::~MetalTextureGpuManager()
    {
        destroyAll();

        for( int i=0; i<=Texture::T_3D; ++i )
            mBlankTexture[i] = 0;
    }
    //-----------------------------------------------------------------------------------
    TextureGpu* MetalTextureGpuManager::createTextureGpuWindow( MetalWindow *window )
    {
        return N_new MetalTextureGpuWindow( GpuPageOutStrategy::Discard, mBufferManager,
                                               "RenderWindow",
                                               Texture::MM_FRAME|
                                               Texture::MM_Window,
                                               Texture::T_2D, this, window );
    }
    //-----------------------------------------------------------------------------------
    TextureGpu* MetalTextureGpuManager::createWindowDepthBuffer(void)
    {
        return N_new MetalTextureGpuRenderTarget( GpuPageOutStrategy::Discard, mBufferManager,
                                                     "RenderWindow DepthBuffer",
                                                     Texture::MM_FRAME|
                                                     Texture::MM_Window,
                                                     Texture::T_2D, this );
    }
    //-----------------------------------------------------------------------------------
    id<MTLTexture> MetalTextureGpuManager::getBlankTextureMetalName(
            Texture::Type::Texture::Type textureType ) const
    {
        return mBlankTexture[textureType];
    }
    //-----------------------------------------------------------------------------------
    TextureGpu* MetalTextureGpuManager::createTextureImpl(
            GpuPageOutStrategy::GpuPageOutStrategy rtype,
            IdString name, uint32 textureFlags, Texture::Type::Texture::Type initialType )
    {
        MetalTextureGpu *retVal = 0;
        if( textureFlags & Texture::MM_FRAME )
        {
            retVal = N_new MetalTextureGpuRenderTarget( rtype, mBufferManager, name,
                                                           textureFlags, initialType, this );
        }
        else
        {
            retVal = N_new MetalTextureGpu( rtype, mBufferManager, name,
                                               textureFlags, initialType, this );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    MappedTexture* MetalTextureGpuManager::createMappedImpl( uint32 width, uint32 height,
                                                                      uint32 depth,
                                                                      uint32 slices,
                                                                      PixelFormat pf )
    {
        const uint32 rowAlignment = 4u;
        const size_t sizeBytes = PixelUtil::getSize( width, height, depth, slices, pf, rowAlignment );
        MetalStagingTexture *retVal = N_new MetalStagingTexture( mBufferManager, PixelUtil::getFamily( pf ), sizeBytes, mDevice );
        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuManager::destroyMappedImpl( MappedTexture *stagingTexture )
    {
        //Do nothing, caller will delete stagingTexture.
    }
    //-----------------------------------------------------------------------------------
    TextureAsync* MetalTextureGpuManager::createAsyncImpl(
            uint32 width, uint32 height, uint32 depthOrSlices,
            Texture::Type::Texture::Type textureType, PixelFormat pixelFormatFamily )
    {
        MetalGBufferManager *vaoManager = static_cast<MetalGBufferManager*>( mBufferManager );
        return N_new MetalAsyncTextureTicket( width, height, depthOrSlices, textureType,
                                                 pixelFormatFamily, vaoManager, mDevice );
    }
}
