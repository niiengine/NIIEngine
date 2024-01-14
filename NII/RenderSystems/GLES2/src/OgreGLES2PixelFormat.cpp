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

#include "OgreGLES2PixelFormat.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreBitwise.h"

namespace Ogre {
    struct GLPixelFormatDescription {
        GLenum format;
        GLenum type;
        GLenum internalFormat;
    };
    
    void GLES2PixelUtil::useSizedFormats()
    {
        memcpy(_pixelFormats, _pixelFormatsSized, sizeof(_pixelFormatsSized));
#if OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
        // disable formats that require swizzling
        _pixelFormats[PF_L8ui].internalFormat = GL_NONE;
        _pixelFormats[PF_L16ui].internalFormat = GL_NONE;
        _pixelFormats[PF_BYTE_LA].internalFormat = GL_NONE;
#endif
    }

    static GLPixelFormatDescription _pixelFormatsSized[] = {
            {GL_NONE},                                           // PF_UNKNOWN
            {GL_RED, GL_UNSIGNED_BYTE, GL_R8},                   // PF_L8ui
            {GL_RED, GL_UNSIGNED_SHORT, GL_R16UI},               // PF_L16ui
            {GL_ALPHA, GL_UNSIGNED_BYTE, GL_R8},                 // PF_A8UN
            {GL_RG, GL_UNSIGNED_BYTE, GL_RG8},                   // PF_BYTE_LA
            {GL_RGB, GL_UNSIGNED_SHORT_5_6_5, GL_RGB565},        // PF_R5G6B5
            {GL_NONE},                                           // PF_B5G6R5
            {GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, GL_RGBA4},      // PF_A4R4G4B4, todo: reversed?
            {GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGB5_A1},    // PF_A1R5G5B5, todo: reversed?
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
            {GL_RGB, GL_UNSIGNED_BYTE, GL_RGB8},                 // PF_R8G8B8
            {GL_NONE},                                           // PF_B8G8R8
            {GL_NONE},                                           // PF_A8R8G8B8
            {GL_NONE},                                           // PF_A8B8G8R8
            {GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_BGRA8_EXT},       // PF_B8G8R8A8
#else
            {GL_NONE},                                           // PF_R8G8B8
            {GL_RGB, GL_UNSIGNED_BYTE, GL_RGB8},                 // PF_B8G8R8
            {GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_BGRA8_EXT},       // PF_A8R8G8B8
            {GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8},               // PF_A8B8G8R8
            {GL_NONE},                                           // PF_B8G8R8A8
#endif
            {GL_NONE},                                                       // PF_A2R10G10B10
            {GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGB10_A2UI},// PF_A2B10G10R10
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},// PF_BC1UN
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},// PF_BC2UN
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},// PF_BC3UN
            {GL_RGB, GL_HALF_FLOAT, GL_RGB16F},                  // PF_R16G16B16UN
            {GL_RGBA, GL_HALF_FLOAT, GL_RGBA16F},                // PF_R16G16B16A16
            {GL_RGB, GL_FLOAT, GL_RGB32F},                       // PF_R32G32B32
            {GL_RGBA, GL_FLOAT, GL_RGBA32F},                     // PF_R32G32B32A32
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
            {GL_NONE},                                           // PF_X8R8G8B8
            {GL_NONE},                                           // PF_X8B8G8R8
            {GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8},               // PF_R8G8B8A8ui
#else
            {GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_BGRA8_EXT},       // PF_X8R8G8B8
            {GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8},               // PF_X8B8G8R8
            {GL_NONE},                                           // PF_R8G8B8A8ui
#endif
            {GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, GL_DEPTH_COMPONENT16}, // PF_DEPTH16
            {GL_RGBA, GL_UNSIGNED_SHORT, GL_RGBA16UI},           // PF_R16G16B16A16ui
            {GL_NONE},                                           // PF_R3G3B2
            {GL_RED, GL_HALF_FLOAT, GL_R16F},                    // PF_R16
            {GL_RED, GL_FLOAT, GL_R32F},                         // PF_R32
            {GL_RG, GL_UNSIGNED_SHORT, GL_RG16UI},               // PF_G16R16ui
            {GL_RG, GL_HALF_FLOAT, GL_RG16F},                    // PF_G16R16
            {GL_RG, GL_FLOAT, GL_RG32F},                         // PF_G32R32
            {GL_RGB, GL_UNSIGNED_SHORT, GL_RGB16UI},             // PF_R16G16B16ui
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG}, // PF_PVRTC_RGB_2BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG},// PF_PVRTC_RGBA_2BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG}, // PF_PVRTC_RGB_4BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG},// PF_PVRTC_RGBA_4BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG},// PF_PVRTC2_2BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG},// PF_PVRTC2_4BPP
            {GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_R11F_G11F_B10F}, // PF_R11G11B10_FLOAT, todo: reversed?
            {GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_R8UI},         // PF_R8_UINT
            {GL_RG_INTEGER, GL_UNSIGNED_BYTE, GL_RG8UI},         // PF_R8G8_UINT
            {GL_RGB_INTEGER, GL_UNSIGNED_BYTE, GL_RGB8UI},       // PF_R8G8B8_UINT
            {GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, GL_RGBA8UI},     // PF_R8G8B8A8_UINT
            {GL_RED_INTEGER, GL_UNSIGNED_SHORT, GL_R16UI},       // PF_R16_UINT
            {GL_RG_INTEGER, GL_UNSIGNED_SHORT, GL_RG16UI},       // PF_R16G16_UINT
            {GL_RGB_INTEGER, GL_UNSIGNED_SHORT, GL_RGB16UI},     // PF_R16G16B16_UINT
            {GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, GL_RGBA16UI},   // PF_R16G16B16A16_UINT
            {GL_RED_INTEGER, GL_UNSIGNED_INT, GL_R32UI},         // PF_R32_UINT
            {GL_RG_INTEGER, GL_UNSIGNED_INT, GL_RG32UI},         // PF_R32G32_UINT
            {GL_RGB_INTEGER, GL_UNSIGNED_INT, GL_RGB32UI},       // PF_R32G32B32_UINT
            {GL_RGBA_INTEGER, GL_UNSIGNED_INT, GL_RGBA32UI},     // PF_R32G32B32A32_UINT
            {GL_RED_INTEGER, GL_BYTE, GL_R8I},                   // PF_R8_SINT
            {GL_RG_INTEGER, GL_BYTE, GL_RG8I},                   // PF_R8G8_SINT
            {GL_RGB_INTEGER, GL_BYTE, GL_RGB8I},                 // PF_R8G8B8_SINT
            {GL_RGBA_INTEGER, GL_BYTE, GL_RGBA8I},               // PF_R8G8B8A8_SINT
            {GL_RED_INTEGER, GL_SHORT, GL_R16I},                 // PF_R16_SINT
            {GL_RG_INTEGER, GL_SHORT, GL_RG16I},                 // PF_R16G16_SINT
            {GL_RGB_INTEGER, GL_SHORT, GL_RGB16I},               // PF_R16G16B16_SINT
            {GL_RGBA_INTEGER, GL_SHORT, GL_RGBA16I},             // PF_R16G16B16A16_SINT
            {GL_RED_INTEGER, GL_INT, GL_R32I},                   // PF_R32_SINT
            {GL_RG_INTEGER, GL_INT, GL_RG32I},                   // PF_R32G32_SINT
            {GL_RGB_INTEGER, GL_INT, GL_RGB32I},                 // PF_R32G32B32_SINT
            {GL_RGBA_INTEGER, GL_INT, GL_RGBA32I},               // PF_R32G32B32A32_SINT
            {GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, GL_RGB9_E5},   // PF_R9G9B9E5_SHAREDEXP
            {GL_NONE},                                           // PF_BC4_UNORM
            {GL_NONE},                                           // PF_BC4_SNORM
            {GL_NONE},                                           // PF_BC5_UNORM
            {GL_NONE},                                           // PF_BC5_SNORM
            {GL_NONE},                                           // PF_BC6H_UF16
            {GL_NONE},                                           // PF_BC6H_SF16
            {GL_NONE},                                           // PF_BC7_UNORM
            {GL_RED_EXT, GL_UNSIGNED_BYTE, GL_R8_EXT},           // PF_R8ui
            {GL_RG_EXT, GL_UNSIGNED_BYTE, GL_RG8_EXT},           // PF_R8G8ui
            {GL_RED, GL_UNSIGNED_BYTE, GL_R8_SNORM},             // PF_R8_SNORM
            {GL_RG, GL_UNSIGNED_BYTE, GL_RG8_SNORM},             // PF_RG8_SNORM
            {GL_RGB, GL_UNSIGNED_BYTE, GL_RGB8_SNORM},           // PF_RGB8_SNORM
            {GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8_SNORM},         // PF_RGBA8_SNORM
            {GL_RED, GL_UNSIGNED_SHORT, GL_R16_SNORM_EXT},       // PF_R16_SNORM
            {GL_RG, GL_UNSIGNED_SHORT, GL_RG16_SNORM_EXT},       // PF_RG16_SNORM
            {GL_RGB, GL_UNSIGNED_SHORT, GL_RGB16_SNORM_EXT},     // PF_RGB16_SNORM
            {GL_RGBA, GL_UNSIGNED_SHORT, GL_RGBA16_SNORM_EXT},   // PF_RGBA16_SNORM
            // the rest are compressed formats that are same
    };

    static GLPixelFormatDescription _pixelFormats[] = {
            {GL_NONE},                                           // PF_UNKNOWN
            {GL_LUMINANCE, GL_UNSIGNED_BYTE, GL_LUMINANCE},      // PF_L8ui
            {GL_NONE},                                           // PF_L16ui
            {GL_ALPHA, GL_UNSIGNED_BYTE, GL_ALPHA},              // PF_A8UN
            {GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA},// PF_BYTE_LA
            {GL_RGB, GL_UNSIGNED_SHORT_5_6_5, GL_RGB},           // PF_R5G6B5
            {GL_NONE},                                           // PF_B5G6R5
            {GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, GL_RGBA},       // PF_A4R4G4B4, todo: reversed?
            {GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGBA},       // PF_A1R5G5B5, todo: reversed?
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
            {GL_RGB, GL_UNSIGNED_BYTE, GL_RGB},                  // PF_R8G8B8
            {GL_NONE},                                           // PF_B8G8R8
            {GL_NONE},                                           // PF_A8R8G8B8
            {GL_NONE},                                           // PF_A8B8G8R8
            {GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_BGRA_EXT},        // PF_B8G8R8A8
#else
            {GL_NONE},                                           // PF_R8G8B8
            {GL_RGB, GL_UNSIGNED_BYTE, GL_RGB},                  // PF_B8G8R8
            {GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_BGRA_EXT},        // PF_A8R8G8B8
            {GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA},                // PF_A8B8G8R8
            {GL_NONE},                                           // PF_B8G8R8A8
#endif
            {GL_NONE},                                                         // PF_A2R10G10B10
            {GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGB10_A2UI},  // PF_A2B10G10R10
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},// PF_BC1UN
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},// PF_BC2UN
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},// PF_BC3UN
            {GL_RGB, GL_HALF_FLOAT_OES, GL_RGB},                 // PF_R16G16B16UN
            {GL_RGBA, GL_HALF_FLOAT_OES, GL_RGBA},               // PF_R16G16B16A16
            {GL_RGB, GL_FLOAT, GL_RGB},                          // PF_R32G32B32
            {GL_RGBA, GL_FLOAT, GL_RGBA},                        // PF_R32G32B32A32
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
            {GL_NONE},                                           // PF_X8R8G8B8
            {GL_NONE},                                           // PF_X8B8G8R8
            {GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA},                // PF_R8G8B8A8ui
#else
            {GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_BGRA_EXT},        // PF_X8R8G8B8
            {GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA},                // PF_X8B8G8R8
            {GL_NONE},                                           // PF_R8G8B8A8ui
#endif
            {GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, GL_DEPTH_COMPONENT16}, // PF_DEPTH16
            {GL_NONE},                                           // PF_R16G16B16A16ui
            {GL_NONE},                                           // PF_R3G3B2
            {GL_RED, GL_HALF_FLOAT_OES, GL_R16F},                // PF_R16
            {GL_RED, GL_FLOAT, GL_R32F},                         // PF_R32
            {GL_NONE},                                           // PF_G16R16ui
            {GL_RG, GL_HALF_FLOAT_OES, GL_RG16F},                // PF_G16R16
            {GL_RG, GL_FLOAT, GL_RG32F},                         // PF_G32R32
            {GL_NONE},                                           // PF_R16G16B16ui
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG}, // PF_PVRTC_RGB_2BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG},// PF_PVRTC_RGBA_2BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG}, // PF_PVRTC_RGB_4BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG},// PF_PVRTC_RGBA_4BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG},// PF_PVRTC2_2BPP
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG},// PF_PVRTC2_4BPP
            {GL_NONE},                                           // PF_R11G11B10_FLOAT
            {GL_NONE},                                           // PF_R8_UINT
            {GL_NONE},                                           // PF_R8G8_UINT
            {GL_NONE},                                           // PF_R8G8B8_UINT
            {GL_NONE},                                           // PF_R8G8B8A8_UINT
            {GL_NONE},                                           // PF_R16_UINT
            {GL_NONE},                                           // PF_R16G16_UINT
            {GL_NONE},                                           // PF_R16G16B16_UINT
            {GL_NONE},                                           // PF_R16G16B16A16_UINT
            {GL_NONE},                                           // PF_R32_UINT
            {GL_NONE},                                           // PF_R32G32_UINT
            {GL_NONE},                                           // PF_R32G32B32_UINT
            {GL_NONE},                                           // PF_R32G32B32A32_UINT
            {GL_NONE},                                           // PF_R8_SINT
            {GL_NONE},                                           // PF_R8G8_SINT
            {GL_NONE},                                           // PF_R8G8B8_SINT
            {GL_NONE},                                           // PF_R8G8B8A8_SINT
            {GL_NONE},                                           // PF_R16_SINT
            {GL_NONE},                                           // PF_R16G16_SINT
            {GL_NONE},                                           // PF_R16G16B16_SINT
            {GL_NONE},                                           // PF_R16G16B16A16_SINT
            {GL_NONE},                                           // PF_R32_SINT
            {GL_NONE},                                           // PF_R32G32_SINT
            {GL_NONE},                                           // PF_R32G32B32_SINT
            {GL_NONE},                                           // PF_R32G32B32A32_SINT
            {GL_NONE},                                           // PF_R9G9B9E5_SHAREDEXP
            {GL_NONE},                                           // PF_BC4_UNORM
            {GL_NONE},                                           // PF_BC4_SNORM
            {GL_NONE},                                           // PF_BC5_UNORM
            {GL_NONE},                                           // PF_BC5_SNORM
            {GL_NONE},                                           // PF_BC6H_UF16
            {GL_NONE},                                           // PF_BC6H_SF16
            {GL_NONE},                                           // PF_BC7_UNORM
            {GL_RED_EXT, GL_UNSIGNED_BYTE, GL_RED_EXT},          // PF_R8ui
            {GL_RG_EXT, GL_UNSIGNED_BYTE, GL_RG_EXT},            // PF_R8G8ui
            {GL_NONE},                                           // PF_R8_SNORM
            {GL_NONE},                                           // PF_RG8_SNORM
            {GL_NONE},                                           // PF_RGB8_SNORM
            {GL_NONE},                                           // PF_RGBA8_SNORM
            {GL_NONE},                                           // PF_R16_SNORM
            {GL_NONE},                                           // PF_RG16_SNORM
            {GL_NONE},                                           // PF_RGB16_SNORM
            {GL_NONE},                                           // PF_RGBA16_SNORM
            {GL_NONE, GL_NONE, GL_ETC1_RGB8_OES},                // PF_ETC1_RGB8
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGB8_ETC2},         // PF_ETC2_RGB8
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA8_ETC2_EAC},    // PF_ETC2_RGBA8
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2},// PF_ETC2_RGB8A1
            {GL_NONE, GL_NONE, GL_ATC_RGB_AMD},                             // PF_ATC_RGB
            {GL_NONE, GL_NONE, GL_ATC_RGBA_EXPLICIT_ALPHA_AMD},             // PF_ATC_RGBA_EXPLICIT_ALPHA
            {GL_NONE, GL_NONE, GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD},         // PF_ATC_RGBA_INTERPOLATED_ALPHA
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_4x4_KHR}, // PF_ASTC_RGBA_4X4_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_5x4_KHR}, // PF_ASTC_RGBA_5X4_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_5x5_KHR}, // PF_ASTC_RGBA_5X5_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_6x5_KHR}, // PF_ASTC_RGBA_6X5_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_6x6_KHR}, // PF_ASTC_RGBA_6X6_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_8x5_KHR}, // PF_ASTC_RGBA_8X5_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_8x6_KHR}, // PF_ASTC_RGBA_8X6_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_8x8_KHR}, // PF_ASTC_RGBA_8X8_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_10x5_KHR},// PF_ASTC_RGBA_10X5_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_10x6_KHR},// PF_ASTC_RGBA_10X6_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_10x8_KHR},// PF_ASTC_RGBA_10X8_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_10x10_KHR},// PF_ASTC_RGBA_10X10_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_12x10_KHR},// PF_ASTC_RGBA_12X10_LDR
            {GL_NONE, GL_NONE, GL_COMPRESSED_RGBA_ASTC_12x12_KHR},// PF_ASTC_RGBA_12X12_LDR
            {GL_NONE, GL_NONE, GL_NONE}, // PF_DEPTH32
    };
    //-----------------------------------------------------------------------------
    GLenum GLES2PixelUtil::getGLOriginFormat(PixelFormat format, bool hwGamma)
    {
#if OGRE_NO_GLES3_SUPPORT != 0
		const RenderFeature *caps =
            Root::getSingleton().getRenderSystem()->getCapabilities();
#endif
        switch (format)
        {
            case PF_DEPTH_DEPRECATED:
            case PF_D24_UNORM:
            case PF_D16_UNORM:
            case PF_D32_FLOAT:
                return GL_DEPTH_COMPONENT;
            case PF_D24_UNORM_S8_UINT:
            case PF_D24_UNORM_X8:
            case PF_X24_S8_UINT:
            case PF_D32_FLOAT_X24_S8_UINT:
            case PF_D32_FLOAT_X24_X8:
            case PF_X32_X24_S8_UINT:
                return GL_DEPTH_STENCIL;

            case PF_A8UN:
                return GL_ALPHA;
                
#if GL_OES_texture_half_float || GL_EXT_color_buffer_half_float || (OGRE_NO_GLES3_SUPPORT == 0) && OGRE_PLATFORM != OGRE_PLATFORM_NACL
            case PF_R16G16B16UN:
            case PF_R32G32B32:
                return GL_RGB;
            case PF_R16G16B16A16:
            case PF_R32G32B32A32:
                return GL_RGBA;
#endif

#if (OGRE_NO_GLES3_SUPPORT == 0)
            case PF_R16:
            case PF_R32:
            case PF_R8ui:
                return GL_RED_EXT;
            case PF_G16R16:
            case PF_G32R32:
            case PF_R8G8ui:
                return GL_RG_EXT;
#else
            case PF_BYTE_LA:
            case PF_G16R16ui:
                return GL_LUMINANCE_ALPHA;
#endif

#if (GL_EXT_texture_rg && OGRE_PLATFORM != OGRE_PLATFORM_NACL)
            case PF_R16:
            case PF_R32:
            case PF_R8ui:
                return GL_RED_EXT;

            case PF_G16R16:
            case PF_G32R32:
            case PF_R8G8ui:
                return GL_RG_EXT;
#endif

            // PVRTC compressed formats
#if GL_IMG_texture_compression_pvrtc && OGRE_PLATFORM != OGRE_PLATFORM_NACL
            case PF_PVRTC_RGB_2BPP:
                return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
            case PF_PVRTC_RGB_4BPP:
                return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case PF_PVRTC_RGBA_2BPP:
                return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            case PF_PVRTC_RGBA_4BPP:
                return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#endif                
#if GL_IMG_texture_compression_pvrtc2 && OGRE_PLATFORM != OGRE_PLATFORM_NACL
            case PF_PVRTC2_2BPP:
                return GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
            case PF_PVRTC2_4BPP:
                return GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
#endif

#if OGRE_NO_ETC_CODEC == 0 
#   ifdef GL_OES_compressed_ETC1_RGB8_texture
            case PF_ETC1_RGB8:
                return GL_ETC1_RGB8_OES;
#   endif
#   ifdef GL_AMD_compressed_ATC_texture
            case PF_ATC_RGB:
                return ATC_RGB_AMD;
            case PF_ATC_RGBA_EXPLICIT_ALPHA:
                return ATC_RGBA_EXPLICIT_ALPHA_AMD;
            case PF_ATC_RGBA_INTERPOLATED_ALPHA:
                return ATC_RGBA_INTERPOLATED_ALPHA_AMD;
#   endif
#endif

#if OGRE_NO_GLES3_SUPPORT == 0
            case PF_ETC2_RGB8:
                return GL_COMPRESSED_RGB8_ETC2;
            case PF_ETC2_RGBA8:
                return GL_COMPRESSED_RGBA8_ETC2_EAC;
            case PF_ETC2_RGB8A1:
                return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
#endif

            case PF_R5G6B5:
            case PF_B5G6R5:
            case PF_R8G8B8:
            case PF_B8G8R8:
            case PF_X8B8G8R8:
            case PF_R16G16B16ui:
#if OGRE_NO_GLES3_SUPPORT == 0
                return GL_RGB;
#else
                if( hwGamma && caps->hasCapability( RSC_HW_GAMMA ) )
                    return GL_SRGB_EXT;
                else
                    return GL_RGB;
#endif

            case PF_X8R8G8B8:
            case PF_A8R8G8B8:
            case PF_A4R4G4B4:
            case PF_A1R5G5B5:
            case PF_B8G8R8A8:
#if OGRE_NO_GLES3_SUPPORT == 0
    #if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                    return GL_BGRA_EXT;
    #else
                    return GL_RGBA;
    #endif
#else
    #if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                    return GL_BGRA_EXT;
    #else
                    if( hwGamma && caps->hasCapability( RSC_HW_GAMMA ) )
                        return GL_SRGB_ALPHA_EXT;
                    else
                        return GL_RGBA;
    #endif
#endif

            case PF_A8B8G8R8:
            case PF_R8G8B8A8ui:
            case PF_A2R10G10B10:
            case PF_A2B10G10R10:
            case PF_R16G16B16A16ui:
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS && OGRE_NO_GLES3_SUPPORT == 1
                return GL_BGRA_EXT;
#else
    #if OGRE_NO_GLES3_SUPPORT == 0
                return GL_RGBA;
    #else
                if( hwGamma && caps->hasCapability( RSC_HW_GAMMA ) )
                    return GL_SRGB_ALPHA_EXT;
                else
                    return GL_RGBA;
    #endif
#endif

            case PF_BC1UN:
#if GL_EXT_texture_compression_dxt1
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
#endif
            case PF_BC2UN:
#if GL_EXT_texture_compression_s3tc
                return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
#endif
            case PF_BC3UN:
#if GL_EXT_texture_compression_s3tc
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif
#if OGRE_NO_GLES3_SUPPORT == 0
            case PF_L8ui:
                return GL_RED;
            case PF_R8_UINT:
            case PF_R16_UINT:
            case PF_R32_UINT:
            case PF_R8_SINT:
            case PF_R16_SINT:
            case PF_R32_SINT:
                return GL_RED_INTEGER;
            case PF_R8G8_UINT:
            case PF_R16G16_UINT:
            case PF_R32G32_UINT:
            case PF_R8G8_SINT:
            case PF_R16G16_SINT:
            case PF_R32G32_SINT:
                return GL_RG_INTEGER;
            case PF_R8G8B8_UINT:
            case PF_R16G16B16_UINT:
            case PF_R32G32B32_UINT:
            case PF_R8G8B8_SINT:
            case PF_R16G16B16_SINT:
            case PF_R32G32B32_SINT:
                return GL_RGB_INTEGER;
            case PF_R8G8B8A8_UINT:
            case PF_R16G16B16A16_UINT:
            case PF_R32G32B32A32_UINT:
            case PF_R8G8B8A8_SINT:
            case PF_R16G16B16A16_SINT:
            case PF_R32G32B32A32_SINT:
                return GL_RGBA_INTEGER;
            case PF_R11G11B10_FLOAT:
            case PF_R9G9B9E5_SHAREDEXP:
                return GL_RGB;
            case PF_R8_SNORM:
                return GL_RED;
            case PF_R8G8_SNORM:
                return GL_RG;
            case PF_R8G8B8_SNORM:
                return GL_RGB;
            case PF_R8G8B8A8_SNORM:
                return GL_RGBA;
#else
            case PF_L8ui:
            case PF_L16ui:
                return GL_LUMINANCE;
#endif
            default:
                return 0;
        }
    }

    GLenum GLES2PixelUtil::getGLOriginDataType(PixelFormat format)
    {
        switch (format)
        {
            case PF_A8UN:
            case PF_L8ui:
            case PF_L16ui:
            case PF_R8G8B8:
            case PF_B8G8R8:
            case PF_BYTE_LA:
                return GL_UNSIGNED_BYTE;
            case PF_R5G6B5:
            case PF_B5G6R5:
                return GL_UNSIGNED_SHORT_5_6_5;
            case PF_A4R4G4B4:
            case PF_R16G16B16ui:
            case PF_R16G16B16A16ui:
                return GL_UNSIGNED_SHORT_4_4_4_4;
            case PF_A1R5G5B5:
                return GL_UNSIGNED_SHORT_5_5_5_1;

#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
            case PF_X8B8G8R8:
            case PF_X8R8G8B8:
            case PF_A8B8G8R8:
            case PF_A8R8G8B8:
                return GL_UNSIGNED_INT_8_8_8_8_REV;
            case PF_B8G8R8A8:
            case PF_R8G8B8A8ui:
                return GL_UNSIGNED_BYTE;
#else
            case PF_X8B8G8R8:
            case PF_A8B8G8R8:
            case PF_X8R8G8B8:
            case PF_A8R8G8B8:
            case PF_B8G8R8A8:
            case PF_R8G8B8A8ui:
                return GL_UNSIGNED_BYTE;
#endif

            case PF_R16:
            case PF_G16R16:
            case PF_R16G16B16UN:
            case PF_R16G16B16A16:
#if (GL_OES_texture_half_float && OGRE_PLATFORM != OGRE_PLATFORM_NACL) || (OGRE_NO_GLES3_SUPPORT == 0)
                return GL_HALF_FLOAT_OES;
#else
                return 0;
#endif
#if (GL_EXT_texture_rg && OGRE_PLATFORM != OGRE_PLATFORM_NACL) || (OGRE_NO_GLES3_SUPPORT == 0)
            case PF_R8ui:
            case PF_R8G8ui:
                return GL_UNSIGNED_BYTE;
#endif
            case PF_R32:
            case PF_G32R32:
            case PF_R32G32B32:
            case PF_R32G32B32A32:
            case PF_D32_FLOAT:
#if (GL_OES_texture_float && OGRE_PLATFORM != OGRE_PLATFORM_NACL) || (OGRE_NO_GLES3_SUPPORT == 0)
                return GL_FLOAT;
#endif
#if OGRE_NO_GLES3_SUPPORT == 0
            case PF_A2R10G10B10:
                return GL_UNSIGNED_INT_2_10_10_10_REV;
            case PF_A2B10G10R10:
                return GL_UNSIGNED_INT_2_10_10_10_REV;
            case PF_R8_SNORM:
            case PF_R8G8_SNORM:
            case PF_R8G8B8_SNORM:
            case PF_R8G8B8A8_SNORM:
            case PF_R16_SNORM:
            case PF_R16G16_SNORM:
            case PF_R16G16B16_SNORM:
            case PF_R16G16B16A16_SNORM:
            case PF_R8_SINT:
            case PF_R8G8_SINT:
            case PF_R8G8B8_SINT:
            case PF_R8G8B8A8_SINT:
                return GL_BYTE;
            case PF_R8_UINT:
            case PF_R8G8_UINT:
            case PF_R8G8B8_UINT:
            case PF_R8G8B8A8_UINT:
                return GL_UNSIGNED_BYTE;
            case PF_R32_UINT:
            case PF_R32G32_UINT:
            case PF_R32G32B32_UINT:
            case PF_R32G32B32A32_UINT:
            case PF_DEPTH_DEPRECATED:
            case PF_D24_UNORM:
            case PF_D16_UNORM:
                return GL_UNSIGNED_INT;
            case PF_D24_UNORM_S8_UINT:
            case PF_D24_UNORM_X8:
            case PF_X24_S8_UINT:
                return GL_UNSIGNED_INT_24_8;
            case PF_R16_UINT:
            case PF_R16G16_UINT:
            case PF_R16G16B16_UINT:
            case PF_R16G16B16A16_UINT:
                return GL_UNSIGNED_SHORT;
            case PF_R16_SINT:
            case PF_R16G16_SINT:
            case PF_R16G16B16_SINT:
            case PF_R16G16B16A16_SINT:
                return GL_SHORT;
            case PF_R32G32B32_SINT:
            case PF_R32_SINT:
            case PF_R32G32_SINT:
            case PF_R32G32B32A32_SINT:
                return GL_INT;
            case PF_D32_FLOAT_X24_S8_UINT:
            case PF_X32_X24_S8_UINT:
            case PF_D32_FLOAT_X24_X8:
                return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

            case PF_R9G9B9E5_SHAREDEXP:
                return GL_UNSIGNED_INT_5_9_9_9_REV;
            case PF_R11G11B10_FLOAT:
                return GL_UNSIGNED_INT_10F_11F_11F_REV;
#endif
            default:
                return 0;
        }
    }

    //-----------------------------------------------------------------------------
    GLenum GLES2PixelUtil::getGLInternalFormat(PixelFormat fmt, bool hwGamma)
    {
#if OGRE_NO_GLES3_SUPPORT != 0
		const RenderFeature *caps =
            Root::getSingleton().getRenderSystem()->getCapabilities();
#endif

        switch (fmt)
        {
            case PF_DEPTH_DEPRECATED:
                return GL_DEPTH_COMPONENT;
            case PF_D24_UNORM_S8_UINT:
            case PF_D24_UNORM_X8:
            case PF_X24_S8_UINT:
                return GL_DEPTH24_STENCIL8_OES;
            case PF_D24_UNORM:
                return GL_DEPTH_COMPONENT24_OES;
            case PF_D16_UNORM:
                return GL_DEPTH_COMPONENT16;
            case PF_D32_FLOAT:
                return GL_DEPTH_COMPONENT32F;
            case PF_D32_FLOAT_X24_S8_UINT:
            case PF_D32_FLOAT_X24_X8:
            case PF_X32_X24_S8_UINT:
                return GL_DEPTH32F_STENCIL8;

#if GL_IMG_texture_compression_pvrtc && OGRE_PLATFORM != OGRE_PLATFORM_NACL
            case PF_PVRTC_RGB_2BPP:
                return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
            case PF_PVRTC_RGB_4BPP:
                return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case PF_PVRTC_RGBA_2BPP:
                return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            case PF_PVRTC_RGBA_4BPP:
                return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#endif
#if GL_IMG_texture_compression_pvrtc2 && OGRE_PLATFORM != OGRE_PLATFORM_NACL
            case PF_PVRTC2_2BPP:
                return GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
            case PF_PVRTC2_4BPP:
                return GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
#endif

#if OGRE_NO_ETC_CODEC == 0 
#   ifdef GL_OES_compressed_ETC1_RGB8_texture
            case PF_ETC1_RGB8:
                return GL_ETC1_RGB8_OES;
#   endif
#   ifdef GL_AMD_compressed_ATC_texture
            case PF_ATC_RGB:
                return ATC_RGB_AMD;
            case PF_ATC_RGBA_EXPLICIT_ALPHA:
                return ATC_RGBA_EXPLICIT_ALPHA_AMD;
            case PF_ATC_RGBA_INTERPOLATED_ALPHA:
                return ATC_RGBA_INTERPOLATED_ALPHA_AMD;
#   endif
#endif

#if OGRE_NO_GLES3_SUPPORT == 0
            case PF_ETC2_RGB8:
                return GL_COMPRESSED_RGB8_ETC2;
            case PF_ETC2_RGBA8:
                return GL_COMPRESSED_RGBA8_ETC2_EAC;
            case PF_ETC2_RGB8A1:
                return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
#endif

#if OGRE_NO_GLES3_SUPPORT == 0
            case PF_A1R5G5B5:
                return GL_RGB5_A1;
            case PF_R5G6B5:
            case PF_B5G6R5:
                return GL_RGB565;
            case PF_A4R4G4B4:
                return GL_RGBA4;
            case PF_R8G8B8:
            case PF_B8G8R8:
                if (hwGamma)
                    return GL_SRGB8;
                else
                    return GL_RGB8;
            case PF_A8R8G8B8:
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                return GL_BGRA8_EXT;
#endif
            case PF_B8G8R8A8:
            case PF_A8B8G8R8:
            case PF_R8G8B8A8ui:
            case PF_X8B8G8R8:
            case PF_X8R8G8B8:
                if (hwGamma)
                    return GL_SRGB8_ALPHA8;
                else
                    return GL_RGBA8;
            case PF_A2R10G10B10:
            case PF_A2B10G10R10:
                return GL_RGB10_A2UI;
            case PF_R32G32B32:
                return GL_RGB32F;
            case PF_R32G32B32A32:
                return GL_RGBA32F;
            case PF_R16G16B16A16:
                return GL_RGBA16F;
            case PF_R16G16B16UN:
                return GL_RGB16F;
            case PF_R11G11B10_FLOAT:
                return GL_R11F_G11F_B10F;
            case PF_R8_UINT:
                return GL_R8UI;
            case PF_R8G8_UINT:
                return GL_RG8UI;
            case PF_R8G8B8_UINT:
                return GL_RGB8UI;
            case PF_R8G8B8A8_UINT:
                return GL_RGBA8UI;
            case PF_R16_UINT:
                return GL_R16UI;
            case PF_R16G16_UINT:
                return GL_RG16UI;
            case PF_R16G16B16_UINT:
                return GL_RGB16UI;
            case PF_R16G16B16A16_UINT:
                return GL_RGBA16UI;
            case PF_R32_UINT:
                return GL_R32UI;
            case PF_R32G32_UINT:
                return GL_RG32UI;
            case PF_R32G32B32_UINT:
                return GL_RGB32UI;
            case PF_R32G32B32A32_UINT:
                return GL_RGBA32UI;
            case PF_R8_SINT:
                return GL_R8I;
            case PF_R8G8_SINT:
                return GL_RG8I;
            case PF_R8G8B8_SINT:
                return GL_RG8I;
            case PF_R8G8B8A8_SINT:
                return GL_RGB8I;
            case PF_R16_SINT:
                return GL_R16I;
            case PF_R16G16_SINT:
                return GL_RG16I;
            case PF_R16G16B16_SINT:
                return GL_RGB16I;
            case PF_R16G16B16A16_SINT:
                return GL_RGBA16I;
            case PF_R32_SINT:
                return GL_R32I;
            case PF_R32G32_SINT:
                return GL_RG32I;
            case PF_R32G32B32_SINT:
                return GL_RGB32I;
            case PF_R32G32B32A32_SINT:
                return GL_RGBA32I;
            case PF_R9G9B9E5_SHAREDEXP:
                return GL_RGB9_E5;
            case PF_R8_SNORM:
                return GL_R8_SNORM;
            case PF_R8G8_SNORM:
                return GL_RG8_SNORM;
            case PF_R8G8B8_SNORM:
                return GL_RGB8_SNORM;
            case PF_R8G8B8A8_SNORM:
                return GL_RGBA8_SNORM;
            case PF_L8ui:
            case PF_A8UN:
                return GL_R8;
            case PF_L16ui:
                return GL_R16F_EXT;
#else
            case PF_L8ui:
            case PF_L16ui:
                return GL_LUMINANCE;

            case PF_A8UN:
                return GL_ALPHA;

            case PF_BYTE_LA:
                return GL_LUMINANCE_ALPHA;
            case PF_A8R8G8B8:
            case PF_A8B8G8R8:
            case PF_B8G8R8A8:
            case PF_A1R5G5B5:
            case PF_A4R4G4B4:
            case PF_X8B8G8R8:
            case PF_X8R8G8B8:
            case PF_R16G16B16A16ui:
                if( hwGamma && caps->hasCapability( RSC_HW_GAMMA ) )
                    return GL_SRGB_ALPHA_EXT;
                else
                    return GL_RGBA;
            case PF_R16G16B16UN:
            case PF_R32G32B32:
            case PF_R5G6B5:
            case PF_B5G6R5:
            case PF_R8G8B8:
            case PF_B8G8R8:
            case PF_R16G16B16ui:
                if( hwGamma && caps->hasCapability( RSC_HW_GAMMA ) )
                    return GL_SRGB_EXT;
                else
                    return GL_RGB;
            case PF_R32G32B32A32:
            case PF_A2R10G10B10:
            case PF_A2B10G10R10:
#endif

            case PF_A4L4i:
            case PF_R3G3B2:
            case PF_G16R16ui:
            case PF_BC1UN:
#if GL_EXT_texture_compression_dxt1
                if (!hwGamma)
                    return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
#endif
            case PF_BC2UN:
#if GL_EXT_texture_compression_s3tc
                if (!hwGamma)
                    return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
#endif
            case PF_BC3UN:
#if GL_EXT_texture_compression_s3tc
                if (!hwGamma)
                    return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
#endif
                
#if (GL_EXT_texture_rg && OGRE_PLATFORM != OGRE_PLATFORM_NACL) || (OGRE_NO_GLES3_SUPPORT == 0)
            case PF_R16:
                return GL_R16F_EXT;
            case PF_R32:
                return GL_R32F_EXT;
            case PF_R8ui:
#if OGRE_NO_GLES3_SUPPORT == 0
                return GL_R8;
#else
                return GL_RED_EXT;
#endif
            case PF_G16R16:
                return GL_RG16F_EXT;
            case PF_G32R32:
                return GL_RG32F_EXT;
            case PF_R8G8ui:
#if OGRE_NO_GLES3_SUPPORT == 0
                return GL_RG8;
#else
                return GL_RG_EXT;
#endif
#endif
            default:
                return GL_NONE;
        }
    }

    GLenum GLES2PixelUtil::getClosestGLInternalFormat(PixelFormat format, bool hwGamma)
    {
        GLenum GLformat = getGLInternalFormat(format, hwGamma);
        if (GLformat == GL_NONE)
        {
#if OGRE_NO_GLES3_SUPPORT == 0
            if (hwGamma)
                return GL_SRGB8;
            else
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                return GL_BGRA8_EXT;
#else
                return GL_RGBA8;
#endif
#else
            const RenderFeature *caps =
                Root::getSingleton().getRenderSystem()->getCapabilities();

            if (hwGamma)
            {
                if( caps->hasCapability( RSC_HW_GAMMA ) )
                    return GL_SRGB_EXT;
                else // TODO not supported
                    return 0;
            }
            else
                return GL_RGBA8_OES;
#endif
        }
        else
            return GLformat;
    }
    //-----------------------------------------------------------------------------
    GLenum GLES2PixelUtil::getGLImageInternalFormat(PixelFormat format)
    {
        switch(format)
        {
        case PF_L8ui:
        case PF_A8UN:
            return GL_R8;
//        case PF_L16ui:
//            return GL_R16;
        case PF_BYTE_LA:
            return GL_RG8;
        case PF_A8R8G8B8:
        case PF_B8G8R8A8:
        case PF_A8B8G8R8:
        case PF_R8G8B8A8ui:
            return GL_RGBA8;
        case PF_A2R10G10B10:
        case PF_A2B10G10R10:
            //return GL_RGB10_A2UI;
            return GL_RGB10_A2;
        case PF_R16:
            return GL_R16F;
        case PF_G16R16:
            return GL_RG16F;
        case PF_R16G16B16A16:
            return GL_RGBA16F;
        case PF_R32:
            return GL_R32F;
        case PF_G32R32:
            return GL_RG32F;
        case PF_R32G32B32A32:
            return GL_RGBA32F;
//        case PF_R16G16B16A16ui:
//            return GL_RGBA16;
//        case PF_G16R16ui:
//            return GL_RG16;
        case PF_R11G11B10_FLOAT:
            return GL_R11F_G11F_B10F;
        case PF_R8_UINT:
            return GL_R8UI;
        case PF_R8G8_UINT:
            return GL_RG8UI;
        case PF_R8G8B8A8_UINT:
            return GL_RGBA8UI;
        case PF_R16_UINT:
            return GL_R16UI;
        case PF_R16G16_UINT:
            return GL_RG16UI;
        case PF_R16G16B16A16_UINT:
            return GL_RGBA16UI;
        case PF_R32_UINT:
            return GL_R32UI;
        case PF_R32G32_UINT:
            return GL_RG32UI;
        case PF_R32G32B32A32_UINT:
            return GL_RGBA32UI;
        case PF_R8_SINT:
            return GL_R8I;
        case PF_R8G8_SINT:
            return GL_RG8I;
        case PF_R8G8B8_SINT:
            return GL_RG8I;
        case PF_R16_SINT:
            return GL_R16I;
        case PF_R16G16_SINT:
            return GL_RG16I;
        case PF_R16G16B16A16_SINT:
            return GL_RGBA16I;
        case PF_R32_SINT:
            return GL_R32I;
        case PF_R32G32_SINT:
            return GL_RG32I;
        case PF_R32G32B32A32_SINT:
            return GL_RGBA32I;
        case PF_R8G8_SNORM:
            return GL_RG8_SNORM;
//        case PF_R16_SNORM:
//            return GL_R16_SNORM;
//        case PF_R16G16_SNORM:
//            return GL_RG16_SNORM;
//        case PF_R16G16B16A16_SNORM:
//            return GL_RGBA16_SNORM;
        case PF_R8G8B8A8_SNORM:
            return GL_RGBA8_SNORM;

        default:
            return GL_NONE;
        }
    }
    //-----------------------------------------------------------------------------
    GLenum GLES2PixelUtil::getClosestGLImageInternalFormat(PixelFormat format)
    {
        GLenum GLformat = getGLImageInternalFormat(format);
        return (format == GL_NONE ? GL_RGBA8 : GLformat);
    }
    //-----------------------------------------------------------------------------
    PixelFormat GLES2PixelUtil::getClosestOGREFormat(GLenum fmt, GLenum dataType)
    {
        switch (fmt)
        {
            case GL_DEPTH_COMPONENT16:
                return PF_D16_UNORM;
            case GL_DEPTH_COMPONENT24_OES:
                return PF_D24_UNORM;
            case GL_DEPTH_COMPONENT32F:
                return PF_D32_FLOAT;
            case GL_DEPTH_COMPONENT:
            case GL_DEPTH24_STENCIL8:
                return PF_D24_UNORM_S8_UINT;
            case GL_DEPTH32F_STENCIL8:
                return PF_D32_FLOAT_X24_S8_UINT;

#if GL_IMG_texture_compression_pvrtc
            case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
                return PF_PVRTC_RGB_2BPP;
            case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
                return PF_PVRTC_RGBA_2BPP;
            case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
                return PF_PVRTC_RGB_4BPP;
            case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
                return PF_PVRTC_RGBA_4BPP;
#endif

#if GL_IMG_texture_compression_pvrtc2
            case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG:
                return PF_PVRTC2_2BPP;
            case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG:
                return PF_PVRTC2_4BPP;
#endif

#if OGRE_NO_ETC_CODEC == 0 
#   ifdef GL_OES_compressed_ETC1_RGB8_texture
            case GL_ETC1_RGB8_OES:
                return PF_ETC1_RGB8;
#   endif
#   ifdef GL_AMD_compressed_ATC_texture
            case ATC_RGB_AMD:
                return PF_ATC_RGB;
            case ATC_RGBA_EXPLICIT_ALPHA_AMD:
                return PF_ATC_RGBA_EXPLICIT_ALPHA;
            case ATC_RGBA_INTERPOLATED_ALPHA_AMD:
                return PF_ATC_RGBA_INTERPOLATED_ALPHA;
#   endif
#endif

#if OGRE_NO_GLES3_SUPPORT == 0
            case GL_COMPRESSED_RGB8_ETC2:
                return PF_ETC2_RGB8;
            case GL_COMPRESSED_RGBA8_ETC2_EAC:
                return PF_ETC2_RGBA8;
            case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
                return PF_ETC2_RGB8A1;
#endif

            case GL_LUMINANCE:
                return PF_L8ui;
            case GL_ALPHA:
                return PF_A8UN;
            case GL_LUMINANCE_ALPHA:
                return PF_BYTE_LA;
                
            case GL_RGB:
            case GL_RGB8_OES:
                switch(dataType)
                {
                    case GL_UNSIGNED_SHORT_5_6_5:
                        return PF_B5G6R5;
                    case GL_HALF_FLOAT_OES:
                        return PF_R16G16B16UN;
                    case GL_FLOAT:
                        return PF_R32G32B32;
                    default:
                        return PF_R8G8B8;
                }
            case GL_RGBA:
            case GL_RGBA8_OES:
                switch(dataType)
                {
                    case GL_UNSIGNED_SHORT_5_5_5_1:
                        return PF_A1R5G5B5;
                    case GL_UNSIGNED_SHORT_4_4_4_4:
                        return PF_A4R4G4B4;
                    case GL_HALF_FLOAT_OES:
                        return PF_R16G16B16A16;
                    case GL_FLOAT:
                        return PF_R32G32B32A32;
                    default:
                        return PF_A8B8G8R8;
                }
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            case GL_BGRA8_EXT:
            case GL_BGRA_EXT:
                return PF_A8R8G8B8;
#endif

#if GL_EXT_texture_compression_dxt1
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
                return PF_BC1UN;
#endif
#if GL_EXT_texture_compression_s3tc
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
                return PF_BC2UN;
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
                return PF_BC3UN;
#endif
                
#if (GL_EXT_texture_rg && OGRE_PLATFORM != OGRE_PLATFORM_NACL) || (OGRE_NO_GLES3_SUPPORT == 0)
            case GL_R8_EXT:
                return PF_R8ui;
            case GL_RG_EXT:
            case GL_RG8_EXT:
                return PF_R8G8ui;
#endif
#if OGRE_NO_GLES3_SUPPORT != 0
        case GL_SRGB_EXT:
            return PF_X8R8G8B8;
        case GL_SRGB_ALPHA_EXT:
            return PF_A8R8G8B8;
#endif
#if OGRE_NO_GLES3_SUPPORT == 0
            case GL_RGB10_A2:
            case GL_RGB10_A2UI:
                return PF_A2R10G10B10;
            case GL_R11F_G11F_B10F:
                return PF_R11G11B10_FLOAT;
            case GL_R8UI:
                return PF_R8_UINT;
            case GL_RG8UI:
                return PF_R8G8_UINT;
            case GL_RGB8UI:
                return PF_R8G8B8_UINT;
            case GL_RGBA8UI:
                return PF_R8G8B8A8_UINT;
            case GL_R16UI:
                return PF_R16_UINT;
            case GL_RG16UI:
                return PF_R16G16_UINT;
            case GL_RGB16UI:
                return PF_R16G16B16_UINT;
            case GL_RGBA16UI:
                return PF_R16G16B16A16_UINT;
            case GL_R32UI:
                return PF_R32_UINT;
            case GL_RG32UI:
                return PF_R32G32_UINT;
            case GL_RGB32UI:
                return PF_R32G32B32_UINT;
            case GL_RGBA32UI:
                return PF_R32G32B32A32_UINT;
            case GL_R8I:
                return PF_R8_SINT;
            case GL_RG8I:
                return PF_R8G8_SINT;
            case GL_RGB8I:
                return PF_R8G8B8_SINT;
            case GL_RGBA8I:
                return PF_R8G8B8A8_SINT;
            case GL_R16I:
                return PF_R16_SINT;
            case GL_RG16I:
                return PF_R16G16_SINT;
            case GL_RGB16I:
                return PF_R16G16B16_SINT;
            case GL_RGBA16I:
                return PF_R16G16B16A16_SINT;
            case GL_R32I:
                return PF_R32_SINT;
            case GL_RG32I:
                return PF_R32G32_SINT;
            case GL_RGB32I:
                return PF_R32G32B32_SINT;
            case GL_RGBA32I:
                return PF_R32G32B32A32_SINT;
            case GL_RGB9_E5:
                return PF_R9G9B9E5_SHAREDEXP;
            case GL_R8_SNORM:
                return PF_R8_SNORM;
            case GL_RG8_SNORM:
                return PF_R8G8_SNORM;
            case GL_RGB8_SNORM:
                return PF_R8G8B8_SNORM;
            case GL_RGBA8_SNORM:
                return PF_R8G8B8A8_SNORM;
            case GL_SRGB8:
                return PF_X8R8G8B8;
            case GL_SRGB8_ALPHA8:
                return PF_A8R8G8B8;
#endif

#if GL_EXT_color_buffer_half_float || (OGRE_NO_GLES3_SUPPORT == 0)
            case GL_RGBA16F_EXT:
                return PF_R16G16B16A16;
            case GL_RGB16F_EXT:
                return PF_R16G16B16UN;
            case GL_RG16F_EXT:
                return PF_G16R16;
            case GL_R16F_EXT:
                return PF_R16;
#endif

#if GL_EXT_color_buffer_float || (OGRE_NO_GLES3_SUPPORT == 0)
            case GL_RGBA32F_EXT:
                return PF_R32G32B32A32;
            case GL_RGB32F_EXT:
                return PF_R32G32B32;
            case GL_RG32F_EXT:
                return PF_G32R32;
            case GL_R32F_EXT:
                return PF_R32;
#endif

            default:
                LogManager::getSingleton().logMessage("Unhandled Pixel format: " + StringConverter::toString(fmt));
                return PF_A8B8G8R8;
        };
    }
    //-----------------------------------------------------------------------------
    size_t GLES2PixelUtil::getMaxMipmaps(uint32 width, uint32 height, uint32 depth, PixelFormat format)
    {
        size_t count = 0;
        if((width > 0) && (height > 0) && (depth > 0))
        {
            do {
                if(width > 1)
                    width = width / 2;
                if(height > 1)
                    height = height / 2;
                if(depth > 1)
                    depth = depth / 2;
                /*
                 NOT needed, compressed formats will have mipmaps up to 1x1
                 if(PixelUtil::isValidExtent(width, height, depth, format))
                 count ++;
                 else
                 break;
                 */
                
                count++;
            } while(!(width == 1 && height == 1 && depth == 1));
        }       
        return count;
    }
    //-----------------------------------------------------------------------------
    // TODO: Remove
    uint32 GLES2PixelUtil::optionalPO2(uint32 value)
    {
        const RenderFeature * caps = Root::getSingleton().getRenderSystem()->getCapabilities();

        if (caps->hasCapability(RSC_NON_POWER_OF_2_TEXTURES))
        {
            return value;
        }
        else
        {
            return Bitwise::firstPO2From(value);
        }
    }
    //-----------------------------------------------------------------------------
}
