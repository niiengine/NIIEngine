/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org

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

#include "OgreMetalPixelFormatToShaderType.h"
#include "OgreMetalPixelFormatToShaderType.inl"

#include "OgrePixelFormatGpuUtils.h"
#include "OgreTextureGpu.h"

namespace Ogre
{
    static const char c_pixelFormatTypes[PixelFormatDataTypes::NumPixelFormatDataTypes][8] =
    {
        { "float" },
        { "half" },

        { "int" },
        { "unit" },

        { "short" },
        { "ushort" },

        { "char" },
        { "uchar" }
    };

    //-------------------------------------------------------------------------
    PixelFormatDataTypes::PixelFormatDataTypes MetalPixelFormatToShaderType::getPixelFormatDataType(
            PixelFormat pf )
    {
        switch( pf )
        {
        case PF_R32G32B32A32:
        case PF_R32G32B32:
        case PF_R16G16B16A16UN:
        case PF_R32G32:
        case PF_Depth32f_Stencil8ui_64:
        case PF_R10G10B10A2UN:
        case PF_R11G11B10:
        case PF_R8G8B8A8UN:
        case PF_R8G8B8A8UN_SRGB:
        case PF_R16G16UN:
        case PF_Depth32:
        case PF_R32:
        case PF_Depth24UN:
        case PF_Depth24UN_Stencil8ui:
        case PF_R8G8UN:
        case PF_Depth16UN:
        case PF_R16UN:
        case PF_R8UN:
        case PF_A8UN:
        case PF_R1UN:
        case PF_R9G9B9E5:
        case PF_UYVY_R8G8B8G8UN:
        case PF_YUY2_G8R8G8B8UN:
        case PF_BC1UN:
        case PF_BC1UN_SRGB:
        case PF_BC2UN:
        case PF_BC2UN_SRGB:
        case PF_BC3UN:
        case PF_BC3UN_SRGB:
        case PF_BC4UN:
        case PF_BC4N:
        case PF_BC5UN:
        case PF_BC5N:
        case PF_B5G6R5UN:
        case PF_B5G5R5A1UN:
        case PF_B8G8R8A8UN:
        case PF_B8G8R8X8UN:
        case PF_R10G10B10A2UN_XR_BIAS:
        case PF_B8G8R8A8UN_SRGB:
        case PF_B8G8R8X8UN_SRGB:
        case PF_BC6H_u16:
        case PF_BC6H_16:
        case PF_BC7UN:
        case PF_BC7UN_SRGB:
        case PF_B4G4R4A4UN:
        case PF_R16G16B16A16N:
        case PF_R8G8B8A8N:
        case PF_R16G16N:
        case PF_R8G8N:
        case PF_R16N:
        case PF_R8N:
        case PF_PVRTC_RGB_2BPP:
        case PF_PVRTC_RGB_2BPP_SRGB:
        case PF_PVRTC_RGBA_2BPP:
        case PF_PVRTC_RGBA_2BPP_SRGB:
        case PF_PVRTC_RGB_4BPP:
        case PF_PVRTC_RGB_4BPP_SRGB:
        case PF_PVRTC_RGBA_4BPP:
        case PF_PVRTC_RGBA_4BPP_SRGB:
        case PF_PVRTC2_2BPP:
        case PF_PVRTC2_2BPP_SRGB:
        case PF_PVRTC2_4BPP:
        case PF_PVRTC2_4BPP_SRGB:
        case PF_ETC1_R8G8B8UN:
        case PF_ETC2_R8G8B8UN:
        case PF_ETC2_R8G8B8UN_SRGB:
        case PF_ETC2_R8G8B8A8UN:
        case PF_ETC2_R8G8B8A8UN_SRGB:
        case PF_ETC2_RGB8A1UN:
        case PF_ETC2_R8G8B8A1UN_SRGB:
        case PF_EAC_R11UN:
        case PF_EAC_R11N:
        case PF_EAC_R11G11UN:
        case PF_EAC_R11G11N:
        case PF_ATC_RGB:
        case PF_ATC_RGBA_EXPLICIT_A:
        case PF_ATC_RGBA_INTERPOLATED_A:
        case PF_ASTC_RGBAUN_4X4_LDR:   case PF_ASTC_RGBAUN_4X4_sRGB:
        case PF_ASTC_RGBAUN_5X4_LDR:   case PF_ASTC_RGBAUN_5X4_sRGB:
        case PF_ASTC_RGBAUN_5X5_LDR:   case PF_ASTC_RGBAUN_5X5_sRGB:
        case PF_ASTC_RGBAUN_6X5_LDR:   case PF_ASTC_RGBAUN_6X5_sRGB:
        case PF_ASTC_RGBAUN_6X6_LDR:   case PF_ASTC_RGBAUN_6X6_sRGB:
        case PF_ASTC_RGBAUN_8X5_LDR:   case PF_ASTC_RGBAUN_8X5_sRGB:
        case PF_ASTC_RGBAUN_8X6_LDR:   case PF_ASTC_RGBAUN_8X6_sRGB:
        case PF_ASTC_RGBAUN_8X8_LDR:   case PF_ASTC_RGBAUN_8X8_sRGB:
        case PF_ASTC_RGBAUN_10X5_LDR:  case PF_ASTC_RGBAUN_10X5_sRGB:
        case PF_ASTC_RGBAUN_10X6_LDR:  case PF_ASTC_RGBAUN_10X6_sRGB:
        case PF_ASTC_RGBAUN_10X8_LDR:  case PF_ASTC_RGBAUN_10X8_sRGB:
        case PF_ASTC_RGBAUN_10X10_LDR: case PF_ASTC_RGBAUN_10X10_sRGB:
        case PF_ASTC_RGBAUN_12X10_LDR: case PF_ASTC_RGBAUN_12X10_sRGB:
        case PF_ASTC_RGBAUN_12X12_LDR: case PF_ASTC_RGBAUN_12X12_sRGB:
            return PixelFormatDataTypes::Float;

        case PF_R16G16B16A16:
        case PF_R16G16:
        case PF_R16:
            return PixelFormatDataTypes::Half;

        case PF_R32G32B32A32ui:
        case PF_R32G32B32ui:
        case PF_R32G32ui:
        case PF_R32ui:
            return PixelFormatDataTypes::Uint;

        case PF_R32G32B32A32i:
        case PF_R32G32B32i:
        case PF_R32G32i:
        case PF_R32i:
            return PixelFormatDataTypes::Int;

        case PF_R10G10B10A2ui:
        case PF_R16G16ui:
        case PF_R16ui:
            return PixelFormatDataTypes::Ushort;

        case PF_R16G16B16A16i:
        case PF_R16G16i:
        case PF_R16i:
            return PixelFormatDataTypes::Short;

        case PF_R8G8B8A8ui:
        case PF_R8G8ui:
        case PF_R8ui:
            return PixelFormatDataTypes::Uchar;

        case PF_R8G8B8A8i:
        case PF_R8G8i:
        case PF_R8i:
            return PixelFormatDataTypes::Char;

        default:
            return PixelFormatDataTypes::NumPixelFormatDataTypes;
        }

        return PixelFormatDataTypes::NumPixelFormatDataTypes;
    }
    //-------------------------------------------------------------------------
    const char* MetalPixelFormatToShaderType::getPixelFormatType( PixelFormat pf ) const
    {
        PixelFormatDataTypes::PixelFormatDataTypes pfDataType = getPixelFormatDataType( pf );

        if( pfDataType == PixelFormatDataTypes::NumPixelFormatDataTypes )
            return 0;

        return c_pixelFormatTypes[pfDataType];
    }
    //-------------------------------------------------------------------------
    const char* MetalPixelFormatToShaderType::getDataType( PixelFormat pf,
                                                           uint32 textureType,
                                                           bool isMsaa,
                                                           Buffer::OpType access ) const
    {
        if( textureType == Texture::T_Unknow )
            textureType = Texture::T_2D;

        PixelFormatDataTypes::PixelFormatDataTypes pfDataType = getPixelFormatDataType( pf );
        if( pfDataType == PixelFormatDataTypes::NumPixelFormatDataTypes )
            return 0;

        size_t accessIdx = static_cast<size_t>( access );

        if( !PixelUtil::isDepth( pf ) )
        {
            if( isMsaa )
            {
                if( textureType == Texture::T_2D )
                    textureType = 7u;
                else if( textureType == Texture::T_2DArray )
                    textureType = 8u;
            }
            else
            {
                --textureType;
            }

            size_t dataTypeIdx = textureType * PixelFormatDataTypes::NumPixelFormatDataTypes * 4u +
                                 pfDataType * 4u + accessIdx;
            return c_dataTypes[dataTypeIdx];
        }
        else
        {
            if( isMsaa )
            {
                if( textureType == Texture::T_2D )
                    textureType = 11u;
                else if( textureType == Texture::T_2DArray )
                    textureType = 12u;
            }
            else
            {
                if( textureType == Texture::T_2D )
                    textureType = 9u;
                else
                    textureType = 10u;
            }

            size_t dataTypeIdx = 9u * PixelFormatDataTypes::NumPixelFormatDataTypes * 4u +
                                 (textureType - 9u) * 4u + accessIdx;
            return c_dataTypes[dataTypeIdx];
        }
    }
}
