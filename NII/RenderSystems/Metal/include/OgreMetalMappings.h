/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2016 Torus Knot Software Ltd

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

#ifndef _OgreMetalMappings_H_
#define _OgreMetalMappings_H_

#include "OgreMetalPrerequisites.h"

#include "OgrePixelFormatGpu.h"
#include "OgreBlendMode.h"
#include "Vao/OgreVertexElements.h"
#include "OgreHlmsSamplerblock.h"
#include "OgreGpuProgramParams.h"

#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLPixelFormat.h>
#import <Metal/MTLRenderPipeline.h>
#import <Metal/MTLSampler.h>
#import <Metal/MTLVertexDescriptor.h>

namespace Ogre
{
    class _OgreMetalExport MetalMappings
    {
    public:
        /// Can throw if PixelFormat is not depth.
        static void getDepthStencilFormat( MetalDevice *device, PixelFormat pf,
                                           MTLPixelFormat &outDepth, MTLPixelFormat &outStencil );
        static MTLBlendFactor get( ColourFactor op );
        static MTLBlendOperation get( ColourBlendMode op );
        /// @see ShaderChBlend::BlendChannelMasks
        static MTLColorWriteMask get( uint8 mask );

        static MTLStencilOperation get( StencilOpType op );

        static MTLCompareFunction get( CmpMode cmp );

        static MTLVertexFormat get( DataType vertexElemType );

        static MTLSamplerMinMagFilter get( TextureFilterOP filter );
        static MTLSamplerMipFilter getMipFilter( TextureFilterOP filter );
        static MTLSamplerAddressMode get( TextureAddressingMode mode );

        static MTLVertexFormat dataTypeToVertexFormat( MTLDataType dataType );

        static MTLPixelFormat get( PixelFormat pf );

        static GpuConstantType get( MTLDataType dataType );
    };
}

#endif
