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

#ifndef _OgreMetalTextureGpu_H_
#define _OgreMetalTextureGpu_H_

#include "OgreMetalPrerequisites.h"
#include "OgreTextureGpu.h"
#include "OgreDescriptorSetTexture.h"
#include "OgreDescriptorSetUav.h"

#import <Metal/MTLTexture.h>

#include "OgreHeaderPrefix.h"

namespace Ogre
{
    class _OgreMetalExport MetalTextureGpu : public TextureGpu
    {
    protected:
        /// This will not be owned by us if isPoolSlice is true.
        /// It will also not be owned by us if we're not in StorageType::ST_Gpu
        /// This will always point to:
        ///     * A GL texture owned by us.
        ///     * A 4x4 dummy texture (now owned by us).
        ///     * A 64x64 mipmapped texture of us (but now owned by us).
        ///     * A GL texture not owned by us, but contains the final information.
        id<MTLTexture>  mDisplayTextureName;

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
        id<MTLTexture>  mFinalTextureName;
        /// Only used when isManualMSAA() == false.
        id<MTLTexture>  mMsaaFramebufferName;

        virtual void createInternalImpl(void);
        virtual void freeInternalImpl(void);

        MTLTextureType getMetalTextureType(void) const;

    public:
        MetalTextureGpu( RecoverType rtype,
                         GBufferManager *vaoManager, IdString name, uint32 textureFlags,
                         Texture::Type::Texture::Type initialType,
                         TextureManager *textureManager );
        virtual ~MetalTextureGpu();

        virtual void setTextureType( Texture::Type::Texture::Type textureType );

        virtual void read( TextureGpu *dst, const PixelBlock &dstBox, uint8 dstMipLevel,
                             const PixelBlock &srcBox, uint8 srcMipLevel,
                             bool keepResolvedTexSynced = true );

        virtual void mipmap(void);

        virtual void getMultiSampleCoord(Vector2List & locations );

        virtual void notifyGpuData(void);
        virtual bool isValidImpl(void) const;

        virtual void _setToDisplayDummyTexture(void);
        virtual void poolImpl( const TexturePool * newPool, Nui16 array );

        id<MTLTexture> getView( PixelFormat pf, uint8 mipLevel, uint8 numMipmaps,
                                uint16 arraySlice, bool cubemapsAs2DArrays, bool forUav );
        id<MTLTexture> getView( ShaderChTexture::TextureSlot texSlot );
        id<MTLTexture> getView( DescriptorSetUav::TextureSlot uavSlot );

        id<MTLTexture> getDisplayTextureName(void) const    { return mDisplayTextureName; }
        id<MTLTexture> getFinalTextureName(void) const      { return mFinalTextureName; }
        id<MTLTexture> getMsaaFramebufferName(void) const   { return mMsaaFramebufferName; }
    };

    class _OgreMetalExport MetalTextureGpuRenderTarget : public MetalTextureGpu
    {
    protected:
        uint16          mDSPool;
        bool            mDSTexture;
        PixelFormat  mDSFormat;

    public:
        MetalTextureGpuRenderTarget( RecoverType rtype,
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

#include "OgreHeaderSuffix.h"

#endif
