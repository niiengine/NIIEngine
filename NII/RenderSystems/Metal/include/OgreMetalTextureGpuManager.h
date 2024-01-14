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

#ifndef _OgreMetalTextureGpuManager_H_
#define _OgreMetalTextureGpuManager_H_

#include "OgreMetalPrerequisites.h"
#include "OgreTextureGpuManager.h"

#include "OgreTextureGpu.h"

#include "OgreHeaderPrefix.h"

namespace Ogre
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Resources
    *  @{
    */
    class _OgreMetalExport MetalTextureGpuManager : public TextureManager
    {
    protected:
        /// 4x4 texture for when we have nothing to display.
        id<MTLTexture>  mBlankTexture[Texture::T_3D + 1u];

        MetalDevice *mDevice;

        virtual TextureGpu* createTextureImpl( RecoverType rtype,
                                               IdString name, uint32 textureFlags,
                                               Texture::Type::Texture::Type initialType );
        virtual MappedTexture* createMappedImpl(const PixelVolume & vol );
        virtual void destroyMappedImpl( MappedTexture *stagingTexture );

        virtual TextureAsync* createAsyncImpl (uint32 width, uint32 height,
                                                                  uint32 depthOrSlices,
                                                                  Texture::Type::Texture::Type textureType,
                                                                  PixelFormat pixelFormatFamily );

    public:
        MetalTextureGpuManager( GBufferManager *vaoManager, RenderSystem *renderSystem,
                                MetalDevice *device );
        virtual ~MetalTextureGpuManager();

        /** Creates a special MetalTextureGpuWindow pointer, to be used by Ogre::Window.
            The pointer can be freed by a regular N_delete. We do not track this pointer.
            If caller doesnt' delete it, it will leak.
        */
        TextureGpu* createTextureGpuWindow( MetalWindow *window );
        TextureGpu* createWindowDepthBuffer(void);

        id<MTLTexture> getBlankTextureMetalName( Texture::Type::Texture::Type textureType ) const;

        MetalDevice* getDevice(void) const          { return mDevice; }
    };

    /** @} */
    /** @} */
}

#include "OgreHeaderSuffix.h"

#endif
