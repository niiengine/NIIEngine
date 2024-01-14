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

#include "OgreMetalMappings.h"
#include "OgrePixelFormatGpuUtils.h"
#include "OgreHlmsDatablock.h"
#include "OgreMetalDevice.h"

namespace Ogre
{
    //-----------------------------------------------------------------------------------
    void MetalMappings::getDepthStencilFormat( MetalDevice *device, PixelFormat pf,
                                               MTLPixelFormat &outDepth, MTLPixelFormat &outStencil )
    {
        MTLPixelFormat depthFormat = MTLPixelFormatInvalid;
        MTLPixelFormat stencilFormat = MTLPixelFormatInvalid;

        switch( pf )
        {
        case PF_Depth24UN_Stencil8ui:
        case PF_Depth24UN:
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
//            depthFormat = MTLPixelFormatDepth32Float;
//            stencilFormat = MTLPixelFormatStencil8;
            depthFormat     = MTLPixelFormatDepth32Float_Stencil8;
            stencilFormat   = MTLPixelFormatDepth32Float_Stencil8;
#else
            if( device->mDevice.depth24Stencil8PixelFormatSupported )
            {
                depthFormat = MTLPixelFormatDepth24Unorm_Stencil8;
                stencilFormat = MTLPixelFormatDepth24Unorm_Stencil8;
            }
            else
            {
                depthFormat = MTLPixelFormatDepth32Float_Stencil8;
                stencilFormat = MTLPixelFormatDepth32Float_Stencil8;
            }
#endif
            break;
        case PF_Depth16UN:
            depthFormat = MTLPixelFormatDepth32Float;
            break;
        case PF_Depth32:
            depthFormat = MTLPixelFormatDepth32Float;
            break;
        case PF_Depth32f_Stencil8ui_64:
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
//            depthFormat = MTLPixelFormatDepth32Float;
//            stencilFormat = MTLPixelFormatStencil8;
            depthFormat = MTLPixelFormatDepth32Float_Stencil8;
            stencilFormat = MTLPixelFormatDepth32Float_Stencil8;
#else
            depthFormat = MTLPixelFormatDepth32Float_Stencil8;
            stencilFormat = MTLPixelFormatDepth32Float_Stencil8;
#endif
            break;
//        case PFG_X32_X24_S8_UINT:
//            stencilFormat = MTLPixelFormatStencil8;
//            break;
        default:
            N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                         "PixelFormat '" + String( PixelUtil::toString( pf ) ) +
                         "' is not a valid depth buffer format",
                         "MetalRenderSystem::_createDepthBufferFor" );
        }

        outDepth = depthFormat;
        outStencil = stencilFormat;
    }
    //-----------------------------------------------------------------------------------
    MTLBlendFactor MetalMappings::get( ColourFactor op )
    {
        switch( op )
        {
        case SBF_ONE:                   return MTLBlendFactorOne;
        case SBF_ZERO:                  return MTLBlendFactorZero;
        case SBF_DEST_COLOUR:           return MTLBlendFactorDestinationColor;
        case SBF_SOURCE_COLOUR:         return MTLBlendFactorSourceColor;
        case SBF_ONE_MINUS_DEST_COLOUR: return MTLBlendFactorOneMinusDestinationColor;
        case SBF_ONE_MINUS_SOURCE_COLOUR:return MTLBlendFactorOneMinusSourceColor;
        case SBF_DEST_ALPHA:            return MTLBlendFactorDestinationAlpha;
        case SBF_SOURCE_ALPHA:          return MTLBlendFactorSourceAlpha;
        case SBF_ONE_MINUS_DEST_ALPHA:  return MTLBlendFactorOneMinusDestinationAlpha;
        case SBF_ONE_MINUS_SOURCE_ALPHA:return MTLBlendFactorOneMinusSourceAlpha;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLBlendOperation MetalMappings::get( ColourBlendMode op )
    {
        switch( op )
        {
        case SBO_ADD:                   return MTLBlendOperationAdd;
        case SBO_SUBTRACT:              return MTLBlendOperationSubtract;
        case SBO_REVERSE_SUBTRACT:      return MTLBlendOperationReverseSubtract;
        case SBO_MIN:                   return MTLBlendOperationMin;
        case SBO_MAX:                   return MTLBlendOperationMax;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLColorWriteMask MetalMappings::get( uint8 mask )
    {
        return ((mask & ShaderChBlend::BlendChannelRed)   << (3u - 0u)) |
               ((mask & ShaderChBlend::BlendChannelGreen) << (2u - 1u)) |
               ((mask & ShaderChBlend::BlendChannelBlue)  >> (2u - 1u)) |
               ((mask & ShaderChBlend::BlendChannelAlpha) >> (3u - 0u));
    }
    //-----------------------------------------------------------------------------------
    MTLStencilOperation MetalMappings::get( StencilOpType op )
    {
        switch( op )
        {
        case SOP_KEEP:              return MTLStencilOperationKeep;
        case SOP_ZERO:              return MTLStencilOperationZero;
        case SOP_REPLACE:           return MTLStencilOperationReplace;
        case SOP_INCREMENT:         return MTLStencilOperationIncrementClamp;
        case SOP_DECREMENT:         return MTLStencilOperationDecrementClamp;
        case SOP_INCREMENT_WRAP:    return MTLStencilOperationIncrementWrap;
        case SOP_DECREMENT_WRAP:    return MTLStencilOperationDecrementWrap;
        case SOP_INVERT:            return MTLStencilOperationInvert;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLCompareFunction MetalMappings::get( CmpMode cmp )
    {
        switch( cmp )
        {
        case CMPF_ALWAYS_FAIL:          return MTLCompareFunctionNever;
        case CMPF_ALWAYS_PASS:          return MTLCompareFunctionAlways;
        case CMPF_LESS:                 return MTLCompareFunctionLess;
        case CMPF_LESS_EQUAL:           return MTLCompareFunctionLessEqual;
        case CMPF_EQUAL:                return MTLCompareFunctionEqual;
        case CMPF_NOT_EQUAL:            return MTLCompareFunctionNotEqual;
        case CMPF_GREATER_EQUAL:        return MTLCompareFunctionGreaterEqual;
        case CMPF_GREATER:              return MTLCompareFunctionGreater;
        case NUM_COMPARE_FUNCTIONS:
            N_assert( false ); //Should never hit.
            return MTLCompareFunctionAlways;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLVertexFormat MetalMappings::get( DataType vertexElemType )
    {
        switch( vertexElemType )
        {
        case DT_Float1x:                return MTLVertexFormatFloat;
        case VET_FLOAT2:                return MTLVertexFormatFloat2;
        case VET_FLOAT3:                return MTLVertexFormatFloat3;
        case VET_FLOAT4:                return MTLVertexFormatFloat4;
        case DT_Short2x:                return MTLVertexFormatShort2;
        case VET_SHORT4:                return MTLVertexFormatShort4;
        case DT_UByte4x:                return MTLVertexFormatUChar4;
        case DT_UShort2x:               return MTLVertexFormatUShort2;
        case VET_USHORT4:               return MTLVertexFormatUShort4;
        case DT_Int1x:                  return MTLVertexFormatInt;
        case VET_INT2:                  return MTLVertexFormatInt2;
        case VET_INT3:                  return MTLVertexFormatInt3;
        case VET_INT4:                  return MTLVertexFormatInt4;
        case DT_UInt1x:                 return MTLVertexFormatUInt;
        case VET_UINT2:                 return MTLVertexFormatUInt2;
        case VET_UINT3:                 return MTLVertexFormatUInt3;
        case VET_UINT4:                 return MTLVertexFormatUInt4;
        case DT_Byte4x:                 return MTLVertexFormatChar4;
        case VET_BYTE4_SNORM:           return MTLVertexFormatChar4Normalized;
        case VET_UBYTE4_NORM:           return MTLVertexFormatUChar4Normalized;
        case VET_SHORT2_SNORM:          return MTLVertexFormatShort2Normalized;
        case VET_SHORT4_SNORM:          return MTLVertexFormatShort4Normalized;
        case VET_USHORT2_NORM:          return MTLVertexFormatUShort2Normalized;
        case VET_USHORT4_NORM:          return MTLVertexFormatUShort4Normalized;
        case VET_HALF2:                 return MTLVertexFormatHalf2;
        case VET_HALF4:                 return MTLVertexFormatHalf4;

        case DT_Colour:
        case DT_Colour_ARGB:
        case DT_Colour_ABGR:
            return MTLVertexFormatUChar4Normalized;

        case DT_Double1x:
        case VET_DOUBLE2:
        case VET_DOUBLE3:
        case VET_DOUBLE4:
        case VET_USHORT1_DEPRECATED:
        case VET_USHORT3_DEPRECATED:
        default:
            return MTLVertexFormatInvalid;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLSamplerMinMagFilter MetalMappings::get( TextureFilterOP filter )
    {
        switch( filter )
        {
        case FO_NONE:                   return MTLSamplerMinMagFilterNearest;
        case FO_POINT:                  return MTLSamplerMinMagFilterNearest;
        case FO_LINEAR:                 return MTLSamplerMinMagFilterLinear;
        case FO_ANISOTROPIC:            return MTLSamplerMinMagFilterLinear;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLSamplerMipFilter MetalMappings::getMipFilter( TextureFilterOP filter )
    {
        switch( filter )
        {
        case FO_NONE:                   return MTLSamplerMipFilterNotMipmapped;
        case FO_POINT:                  return MTLSamplerMipFilterNearest;
        case FO_LINEAR:                 return MTLSamplerMipFilterLinear;
        case FO_ANISOTROPIC:            return MTLSamplerMipFilterLinear;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLSamplerAddressMode MetalMappings::get( TextureAddressingMode mode )
    {
        switch( mode )
        {
        case TAM_WRAP:                  return MTLSamplerAddressModeRepeat;
        case TAM_MIRROR:                return MTLSamplerAddressModeMirrorRepeat;
        case TAM_CLAMP:                 return MTLSamplerAddressModeClampToEdge;
        //Not supported. Get the best next thing.
        case TAM_BORDER:                return MTLSamplerAddressModeClampToEdge;
        default:                        return MTLSamplerAddressModeClampToEdge;
        }
    }
    //-----------------------------------------------------------------------------------
    MTLVertexFormat MetalMappings::dataTypeToVertexFormat( MTLDataType dataType )
    {
        switch( dataType )
        {
        case MTLDataTypeNone:           return MTLVertexFormatInvalid;
        case MTLDataTypeStruct:         return MTLVertexFormatInvalid;
        case MTLDataTypeArray:          return MTLVertexFormatInvalid;

        case MTLDataTypeFloat:          return MTLVertexFormatFloat;
        case MTLDataTypeFloat2:         return MTLVertexFormatFloat2;
        case MTLDataTypeFloat3:         return MTLVertexFormatFloat3;
        case MTLDataTypeFloat4:         return MTLVertexFormatFloat4;

        case MTLDataTypeFloat2x2:       return MTLVertexFormatInvalid;
        case MTLDataTypeFloat2x3:       return MTLVertexFormatInvalid;
        case MTLDataTypeFloat2x4:       return MTLVertexFormatInvalid;

        case MTLDataTypeFloat3x2:       return MTLVertexFormatInvalid;
        case MTLDataTypeFloat3x3:       return MTLVertexFormatInvalid;
        case MTLDataTypeFloat3x4:       return MTLVertexFormatInvalid;

        case MTLDataTypeFloat4x2:       return MTLVertexFormatInvalid;
        case MTLDataTypeFloat4x3:       return MTLVertexFormatInvalid;
        case MTLDataTypeFloat4x4:       return MTLVertexFormatInvalid;

        case MTLDataTypeHalf:           return MTLVertexFormatHalf2;
        case MTLDataTypeHalf2:          return MTLVertexFormatHalf2;
        case MTLDataTypeHalf3:          return MTLVertexFormatHalf3;
        case MTLDataTypeHalf4:          return MTLVertexFormatHalf4;

        case MTLDataTypeHalf2x2:        return MTLVertexFormatInvalid;
        case MTLDataTypeHalf2x3:        return MTLVertexFormatInvalid;
        case MTLDataTypeHalf2x4:        return MTLVertexFormatInvalid;

        case MTLDataTypeHalf3x2:        return MTLVertexFormatInvalid;
        case MTLDataTypeHalf3x3:        return MTLVertexFormatInvalid;
        case MTLDataTypeHalf3x4:        return MTLVertexFormatInvalid;

        case MTLDataTypeHalf4x2:        return MTLVertexFormatInvalid;
        case MTLDataTypeHalf4x3:        return MTLVertexFormatInvalid;
        case MTLDataTypeHalf4x4:        return MTLVertexFormatInvalid;

        case MTLDataTypeInt:            return MTLVertexFormatInt;
        case MTLDataTypeInt2:           return MTLVertexFormatInt2;
        case MTLDataTypeInt3:           return MTLVertexFormatInt3;
        case MTLDataTypeInt4:           return MTLVertexFormatInt4;

        case MTLDataTypeUInt:           return MTLVertexFormatUInt;
        case MTLDataTypeUInt2:          return MTLVertexFormatUInt2;
        case MTLDataTypeUInt3:          return MTLVertexFormatUInt3;
        case MTLDataTypeUInt4:          return MTLVertexFormatUInt4;

        case MTLDataTypeShort:          return MTLVertexFormatShort2;
        case MTLDataTypeShort2:         return MTLVertexFormatShort2;
        case MTLDataTypeShort3:         return MTLVertexFormatShort3;
        case MTLDataTypeShort4:         return MTLVertexFormatShort4;

        case MTLDataTypeUShort:         return MTLVertexFormatUShort2;
        case MTLDataTypeUShort2:        return MTLVertexFormatUShort2;
        case MTLDataTypeUShort3:        return MTLVertexFormatUShort3;
        case MTLDataTypeUShort4:        return MTLVertexFormatUShort4;

        case MTLDataTypeChar:           return MTLVertexFormatChar2;
        case MTLDataTypeChar2:          return MTLVertexFormatChar2;
        case MTLDataTypeChar3:          return MTLVertexFormatChar3;
        case MTLDataTypeChar4:          return MTLVertexFormatChar4;

        case MTLDataTypeUChar:          return MTLVertexFormatUChar2;
        case MTLDataTypeUChar2:         return MTLVertexFormatUChar2;
        case MTLDataTypeUChar3:         return MTLVertexFormatUChar3;
        case MTLDataTypeUChar4:         return MTLVertexFormatUChar4;

        case MTLDataTypeBool:           return MTLVertexFormatFloat;
        case MTLDataTypeBool2:          return MTLVertexFormatFloat2;
        case MTLDataTypeBool3:          return MTLVertexFormatFloat3;
        case MTLDataTypeBool4:          return MTLVertexFormatFloat4;
        }

        return MTLVertexFormatInvalid;
    }
    //-----------------------------------------------------------------------------------
    MTLPixelFormat MetalMappings::get( PixelFormat pf )
    {
        switch( pf )
        {
        case PF_UNKNOWN:                   return MTLPixelFormatInvalid;
        case PFG_NULL:                      return MTLPixelFormatInvalid;
        case PF_R32G32B32A32:		        return MTLPixelFormatRGBA32Float;
        case PF_R32G32B32A32ui:		        return MTLPixelFormatRGBA32Uint;
        case PF_R32G32B32A32i:		        return MTLPixelFormatRGBA32Sint;
        case PF_R32G32B32:		            return MTLPixelFormatInvalid; //Not supported
        case PF_R32G32B32ui:		        return MTLPixelFormatInvalid; //Not supported
        case PF_R32G32B32i:		            return MTLPixelFormatInvalid; //Not supported
        case PF_R16G16B16A16:		        return MTLPixelFormatRGBA16Float;
        case PF_R16G16B16A16UN:		        return MTLPixelFormatRGBA16Unorm;
        case PF_R16G16B16A16ui:		        return MTLPixelFormatRGBA16Uint;
        case PF_R16G16B16A16N:		        return MTLPixelFormatRGBA16Snorm;
        case PF_R16G16B16A16i:		        return MTLPixelFormatRGBA16Sint;
        case PF_R32G32:		                return MTLPixelFormatRG32Float;
        case PF_R32G32ui:                   return MTLPixelFormatRG32Uint;
        case PF_R32G32i:                    return MTLPixelFormatRG32Sint;
        case PF_Depth32f_Stencil8ui_64:	    return MTLPixelFormatDepth32Float_Stencil8;
        case PF_R10G10B10A2UN:              return MTLPixelFormatRGB10A2Unorm;
        case PF_R10G10B10A2ui:              return MTLPixelFormatRGB10A2Uint;
        case PF_R11G11B10:		            return MTLPixelFormatRG11B10Float;
        case PF_R8G8B8A8UN:		            return MTLPixelFormatRGBA8Unorm;
        case PF_R8G8B8A8UN_SRGB:            return MTLPixelFormatRGBA8Unorm_sRGB;
        case PF_R8G8B8A8ui:		            return MTLPixelFormatRGBA8Uint;
        case PF_R8G8B8A8N:		            return MTLPixelFormatRGBA8Snorm;
        case PF_R8G8B8A8i:		            return MTLPixelFormatRGBA8Sint;
        case PF_R16G16:		                return MTLPixelFormatRG16Float;
        case PF_R16G16UN:                   return MTLPixelFormatRG16Unorm;
        case PF_R16G16ui:		            return MTLPixelFormatRG16Uint;
        case PF_R16G16N:                    return MTLPixelFormatRG16Snorm;
        case PF_R16G16i:		            return MTLPixelFormatRG16Sint;
        case PF_Depth32:		            return MTLPixelFormatDepth32Float;
        case PF_R32:		                return MTLPixelFormatR32Float;
        case PF_R32ui:		                return MTLPixelFormatR32Uint;
        case PF_R32i:		                return MTLPixelFormatR32Sint;
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        case PF_Depth24UN:		            return MTLPixelFormatDepth24Unorm_Stencil8;
        case PF_Depth24UN_Stencil8ui:       return MTLPixelFormatDepth24Unorm_Stencil8;
#else
        case PF_Depth24UN:		            return MTLPixelFormatDepth32Float;
        case PF_Depth24UN_Stencil8ui:       return MTLPixelFormatDepth32Float_Stencil8;
#endif
        case PF_R8G8UN:		                return MTLPixelFormatRG8Unorm;
        case PF_R8G8ui:		                return MTLPixelFormatRG8Uint;
        case PF_R8G8N:		                return MTLPixelFormatRG8Snorm;
        case PF_R8G8i:		                return MTLPixelFormatRG8Sint;
        case PF_R16:		                return MTLPixelFormatR16Float;
        case PF_Depth16UN:		            return MTLPixelFormatDepth32Float; //Unsupported
        case PF_R16UN:		                return MTLPixelFormatR16Unorm;
        case PF_R16ui:		                return MTLPixelFormatR16Uint;
        case PF_R16N:		                return MTLPixelFormatR16Snorm;
        case PF_R16i:		                return MTLPixelFormatR16Sint;
        case PF_R8UN:		                return MTLPixelFormatR8Unorm;
        case PF_R8ui:		                return MTLPixelFormatR8Uint;
        case PF_R8N:		                return MTLPixelFormatR8Snorm;
        case PF_R8i:		                return MTLPixelFormatR8Sint;
        case PF_A8UN:		                return MTLPixelFormatA8Unorm;
        case PF_R1UN:                       return MTLPixelFormatInvalid;   //Not supported
        case PF_R9G9B9E5:		            return MTLPixelFormatRGB9E5Float;
        case PF_UYVY_R8G8B8G8UN:            return MTLPixelFormatGBGR422;
        case PF_YUY2_G8R8G8B8UN:            return MTLPixelFormatBGRG422;
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        case PF_BC1UN:                     return MTLPixelFormatBC1_RGBA;
        case PF_BC1UN_SRGB:                return MTLPixelFormatBC1_RGBA_sRGB;
        case PF_BC2UN:                     return MTLPixelFormatBC2_RGBA;
        case PF_BC2UN_SRGB:                return MTLPixelFormatBC2_RGBA_sRGB;
        case PF_BC3UN:                     return MTLPixelFormatBC3_RGBA;
        case PF_BC3UN_SRGB:                return MTLPixelFormatBC3_RGBA_sRGB;
        case PF_BC4UN:                     return MTLPixelFormatBC4_RUnorm;
        case PF_BC4N:                      return MTLPixelFormatBC4_RSnorm;
        case PF_BC5UN:                     return MTLPixelFormatBC5_RGUnorm;
        case PF_BC5N:                      return MTLPixelFormatBC5_RGSnorm;
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        case PF_B5G6R5UN:                  return MTLPixelFormatB5G6R5Unorm;
        case PF_B5G5R5A1UN:                return MTLPixelFormatBGR5A1Unorm;
#endif
        case PF_B8G8R8A8UN:                 return MTLPixelFormatBGRA8Unorm;
        case PF_B8G8R8X8UN:                return MTLPixelFormatBGRA8Unorm;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        case PF_R10G10B10A2UN_XR_BIAS:      return MTLPixelFormatBGRA10_XR;
#endif
        case PF_B8G8R8A8UN_SRGB:            return MTLPixelFormatBGRA8Unorm_sRGB;
        case PF_B8G8R8X8UN_SRGB:           return MTLPixelFormatBGRA8Unorm_sRGB;
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        case PF_BC6H_u16:                   return MTLPixelFormatBC6H_RGBUfloat;
        case PF_BC6H_16:                    return MTLPixelFormatBC6H_RGBFloat;
        case PF_BC7UN:                      return MTLPixelFormatBC7_RGBAUnorm;
        case PF_BC7UN_SRGB:                 return MTLPixelFormatBC7_RGBAUnorm_sRGB;
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        case PF_B4G4R4A4UN:                 return MTLPixelFormatABGR4Unorm;
        case PF_PVRTC_RGB_2BPP:		        return MTLPixelFormatPVRTC_RGB_2BPP;
        case PF_PVRTC_RGB_2BPP_SRGB:        return MTLPixelFormatPVRTC_RGB_2BPP_sRGB;
        case PF_PVRTC_RGBA_2BPP:		    return MTLPixelFormatPVRTC_RGBA_2BPP;
        case PF_PVRTC_RGBA_2BPP_SRGB:       return MTLPixelFormatPVRTC_RGBA_2BPP_sRGB;
        case PF_PVRTC_RGB_4BPP:		        return MTLPixelFormatPVRTC_RGB_4BPP;
        case PF_PVRTC_RGB_4BPP_SRGB:        return MTLPixelFormatPVRTC_RGB_4BPP_sRGB;
        case PF_PVRTC_RGBA_4BPP:		    return MTLPixelFormatPVRTC_RGBA_4BPP;
        case PF_PVRTC_RGBA_4BPP_SRGB:       return MTLPixelFormatPVRTC_RGBA_4BPP_sRGB;
        case PF_PVRTC2_2BPP:		        return MTLPixelFormatInvalid;   //Not supported
        case PF_PVRTC2_2BPP_SRGB:           return MTLPixelFormatInvalid;   //Not supported
        case PF_PVRTC2_4BPP:		        return MTLPixelFormatInvalid;   //Not supported
        case PF_PVRTC2_4BPP_SRGB:           return MTLPixelFormatInvalid;   //Not supported
        case PF_ETC1_R8G8B8UN:              return MTLPixelFormatETC2_RGB8; //Backwards compatible
        case PF_ETC2_R8G8B8UN:              return MTLPixelFormatETC2_RGB8;
        case PF_ETC2_R8G8B8UN_SRGB:		    return MTLPixelFormatETC2_RGB8_sRGB;
        case PF_ETC2_R8G8B8A8UN:            return MTLPixelFormatEAC_RGBA8;
        case PF_ETC2_R8G8B8A8UN_SRGB:	    return MTLPixelFormatEAC_RGBA8_sRGB;
        case PF_ETC2_RGB8A1UN:              return MTLPixelFormatETC2_RGB8A1;
        case PF_ETC2_R8G8B8A1UN_SRGB:       return MTLPixelFormatETC2_RGB8A1_sRGB;
        case PF_EAC_R11UN:                  return MTLPixelFormatEAC_R11Unorm;
        case PF_EAC_R11N:                   return MTLPixelFormatEAC_R11Snorm;
        case PF_EAC_R11G11UN:               return MTLPixelFormatEAC_RG11Unorm;
        case PF_EAC_R11G11N:                return MTLPixelFormatEAC_RG11Snorm;
        case PF_ATC_RGB:                    return MTLPixelFormatInvalid;   //Not supported
        case PF_ATC_RGBA_EXPLICIT_A:        return MTLPixelFormatInvalid;   //Not supported
        case PF_ATC_RGBA_INTERPOLATED_A:    return MTLPixelFormatInvalid;   //Not supported

        case PF_ASTC_RGBAUN_4X4_LDR:       return MTLPixelFormatASTC_4x4_LDR;
        case PF_ASTC_RGBAUN_5X4_LDR:       return MTLPixelFormatASTC_5x4_LDR;
        case PF_ASTC_RGBAUN_5X5_LDR:       return MTLPixelFormatASTC_5x5_LDR;
        case PF_ASTC_RGBAUN_6X5_LDR:       return MTLPixelFormatASTC_6x5_LDR;
        case PF_ASTC_RGBAUN_6X6_LDR:       return MTLPixelFormatASTC_6x6_LDR;
        case PF_ASTC_RGBAUN_8X5_LDR:       return MTLPixelFormatASTC_8x5_LDR;
        case PF_ASTC_RGBAUN_8X6_LDR:       return MTLPixelFormatASTC_8x6_LDR;
        case PF_ASTC_RGBAUN_8X8_LDR:       return MTLPixelFormatASTC_8x8_LDR;
        case PF_ASTC_RGBAUN_10X5_LDR:      return MTLPixelFormatASTC_10x5_LDR;
        case PF_ASTC_RGBAUN_10X6_LDR:      return MTLPixelFormatASTC_10x6_LDR;
        case PF_ASTC_RGBAUN_10X8_LDR:      return MTLPixelFormatASTC_10x8_LDR;
        case PF_ASTC_RGBAUN_10X10_LDR:     return MTLPixelFormatASTC_10x10_LDR;
        case PF_ASTC_RGBAUN_12X10_LDR:     return MTLPixelFormatASTC_12x10_LDR;
        case PF_ASTC_RGBAUN_12X12_LDR:     return MTLPixelFormatASTC_12x12_LDR;
        case PF_ASTC_RGBAUN_4X4_sRGB:      return MTLPixelFormatASTC_4x4_sRGB;
        case PF_ASTC_RGBAUN_5X4_sRGB:      return MTLPixelFormatASTC_5x4_sRGB;
        case PF_ASTC_RGBAUN_5X5_sRGB:      return MTLPixelFormatASTC_5x5_sRGB;
        case PF_ASTC_RGBAUN_6X5_sRGB:      return MTLPixelFormatASTC_6x5_sRGB;
        case PF_ASTC_RGBAUN_6X6_sRGB:      return MTLPixelFormatASTC_6x6_sRGB;
        case PF_ASTC_RGBAUN_8X5_sRGB:      return MTLPixelFormatASTC_8x5_sRGB;
        case PF_ASTC_RGBAUN_8X6_sRGB:      return MTLPixelFormatASTC_8x6_sRGB;
        case PF_ASTC_RGBAUN_8X8_sRGB:      return MTLPixelFormatASTC_8x8_sRGB;
        case PF_ASTC_RGBAUN_10X5_sRGB:     return MTLPixelFormatASTC_10x5_sRGB;
        case PF_ASTC_RGBAUN_10X6_sRGB:     return MTLPixelFormatASTC_10x6_sRGB;
        case PF_ASTC_RGBAUN_10X8_sRGB:     return MTLPixelFormatASTC_10x8_sRGB;
        case PF_ASTC_RGBAUN_10X10_sRGB:    return MTLPixelFormatASTC_10x10_sRGB;
        case PF_ASTC_RGBAUN_12X10_sRGB:    return MTLPixelFormatASTC_12x10_sRGB;
        case PF_ASTC_RGBAUN_12X12_sRGB:    return MTLPixelFormatASTC_12x12_sRGB;
#endif
        case PF_R8G8B8UN:
        case PF_RGB8UN_SRGB:
        case PF_B8G8R8UN:
        case PF_B8G8R8UN_SRGB:
        case PF_R16G16B16UN:
        case PF_AYUV:
        case PF_Y410:
        case PF_Y416:
        case PF_NV12:
        case PF_P010:
        case PF_P016:
        case PF_420_OPAQUE:
        case PF_YUY2:
        case PF_Y210:
        case PF_Y216:
        case PF_NV11:
        case PF_AI44:
        case PF_IA44:
        case PF_P8:
        case PF_A8P8:
        case PF_P208:
        case PF_V208:
        case PF_V408:
        case PF_Count:
        default:
            return MTLPixelFormatInvalid;
        }

        return MTLPixelFormatInvalid;
    }
    //-----------------------------------------------------------------------------------
    GpuConstantType MetalMappings::get( MTLDataType dataType )
    {
        switch( dataType )
        {
        case MTLDataTypeNone:           return GCT_UNKNOWN;
        case MTLDataTypeStruct:         return GCT_UNKNOWN;
        case MTLDataTypeArray:          return GCT_UNKNOWN;

        case MTLDataTypeFloat:          return GDT_Float;
        case MTLDataTypeFloat2:         return GDT_Float2X;
        case MTLDataTypeFloat3:         return GDT_Float3X;
        case MTLDataTypeFloat4:         return GDT_Float4X;

        case MTLDataTypeFloat2x2:       return GDT_Matrix2X2;
        case MTLDataTypeFloat2x3:       return GDT_Matrix2X3;
        case MTLDataTypeFloat2x4:       return GDT_Matrix2X4;

        case MTLDataTypeFloat3x2:       return GDT_Matrix3X4;
        case MTLDataTypeFloat3x3:       return GDT_Matrix3X3;
        case MTLDataTypeFloat3x4:       return GDT_Matrix3X4;

        case MTLDataTypeFloat4x2:       return GDT_Matrix4X2;
        case MTLDataTypeFloat4x3:       return GDT_Matrix4X3;
        case MTLDataTypeFloat4x4:       return GDT_Matrix4X4;

        case MTLDataTypeHalf:           return GCT_UNKNOWN;
        case MTLDataTypeHalf2:          return GCT_UNKNOWN;
        case MTLDataTypeHalf3:          return GCT_UNKNOWN;
        case MTLDataTypeHalf4:          return GCT_UNKNOWN;

        case MTLDataTypeHalf2x2:        return GCT_UNKNOWN;
        case MTLDataTypeHalf2x3:        return GCT_UNKNOWN;
        case MTLDataTypeHalf2x4:        return GCT_UNKNOWN;

        case MTLDataTypeHalf3x2:        return GCT_UNKNOWN;
        case MTLDataTypeHalf3x3:        return GCT_UNKNOWN;
        case MTLDataTypeHalf3x4:        return GCT_UNKNOWN;

        case MTLDataTypeHalf4x2:        return GCT_UNKNOWN;
        case MTLDataTypeHalf4x3:        return GCT_UNKNOWN;
        case MTLDataTypeHalf4x4:        return GCT_UNKNOWN;

        case MTLDataTypeInt:            return GDT_Int;
        case MTLDataTypeInt2:           return GDT_Int2X;
        case MTLDataTypeInt3:           return GDT_Int3X;
        case MTLDataTypeInt4:           return GDT_Int4X;

        case MTLDataTypeUInt:           return GDT_UInt;
        case MTLDataTypeUInt2:          return GDT_UInt2X;
        case MTLDataTypeUInt3:          return GDT_UInt3X;
        case MTLDataTypeUInt4:          return GDT_UInt4X;

        case MTLDataTypeShort:          return GCT_UNKNOWN;
        case MTLDataTypeShort2:         return GCT_UNKNOWN;
        case MTLDataTypeShort3:         return GCT_UNKNOWN;
        case MTLDataTypeShort4:         return GCT_UNKNOWN;

        case MTLDataTypeUShort:         return GCT_UNKNOWN;
        case MTLDataTypeUShort2:        return GCT_UNKNOWN;
        case MTLDataTypeUShort3:        return GCT_UNKNOWN;
        case MTLDataTypeUShort4:        return GCT_UNKNOWN;

        case MTLDataTypeChar:           return GCT_UNKNOWN;
        case MTLDataTypeChar2:          return GCT_UNKNOWN;
        case MTLDataTypeChar3:          return GCT_UNKNOWN;
        case MTLDataTypeChar4:          return GCT_UNKNOWN;

        case MTLDataTypeUChar:          return GCT_UNKNOWN;
        case MTLDataTypeUChar2:         return GCT_UNKNOWN;
        case MTLDataTypeUChar3:         return GCT_UNKNOWN;
        case MTLDataTypeUChar4:         return GCT_UNKNOWN;

        case MTLDataTypeBool:           return GDT_Bool;
        case MTLDataTypeBool2:          return GDT_Bool2X;
        case MTLDataTypeBool3:          return GDT_Bool3X;
        case MTLDataTypeBool4:          return GDT_Bool4X;
        }

        return GCT_UNKNOWN;
    }
}
