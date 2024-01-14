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

#include "NiiPreProcess.h"
#include "NiiPixelFormat.h"
#include "NiiBitwise.h"
#include "NiiColour.h"
#include "NiiException.h"
#include "NiiImageResampler.h"
#include "NiiImageDownsampler.h"
#include "NiiDefaultStream.h"

namespace
{
#include "NiiPixelConversions.h"
};

namespace NII
{
    //-----------------------------------------------------------------------
    // A record that describes a pixel mFormat in detail.
    struct PixelFormatDescription
    {
        const Ntchar * mName;
        // Component count
        Nui8 mDataCount;
        // Number of bytes one element (colour value) takes.
        Nui16 mSize;
        Nui8 mLayout;
        // Pixel mFormat Feature, see enum PixelFormatFeature for the bit field definitions
        Nui16 mFeature;
        // Component type
        //Nui16 mDataType;
        // Number of bits for red(or luminance), green, blue, alpha
        Nui8 rbits,gbits,bbits,abits; /*, ibits, dbits, ... */

        // Masks and shifts as used by packers/unpackers
        Nui32 rmask, gmask, bmask, amask;
        Nui8 rshift, gshift, bshift, ashift;
    };
    #define PF_COMPRESS PixelUtil::PFF_Compress | PixelUtil::PFF_Int | PixelUtil::PFF_Unit
    //-----------------------------------------------------------------------
    PixelUtil::PixelFormatDescription PixelUtil::msPixelFormatDesc[PF_COUNT + 1u] =
    {   // components    mUnitSize   layout  mFeature
    //-----------------------------------------------------------------------
        {_T("PF_UNKNOWN"),         1u, 0,                      0,                  PFF_None ,                          0,0,0,0,    0,0,0,0,                        0,0,0,0},
        //{_T("PFG_NULL"),            1u, 0,                      0,                  PFF_None ,                          0,0,0,0,    0,0,0,0,        0,0,0,0},
        
        {_T("PF_L8ui"),             1,  1,                      PLT_R8,             PFF_Grayscale | PFF_NonEndian,      8,0,0,0,    0xFF,0,0,0,                     0,0,0,0 },
        {_T("PF_L16ui"),            1,  2,                      PLT_R16,        PFF_Grayscale|PFF_NonEndian|PFF_Int16,  16,0,0,0,   0xFFFF,0,0,0,                   0,0,0,0 },
        {_T("PF_A4L4i"),            2,  1,                      PLT_Compress,   PFF_Alpha|PFF_Grayscale|PFF_NonEndian,  4,0,0,4,    0x0F,0,0,0xF0,                  0,0,0,4 },
        {_T("PF_L8A8i"),            2,  2,                      PLT_R8G8,           PFF_Alpha | PFF_Grayscale,          8,0,0,8,    0,0,0,0,                        0,0,0,0 },
        {_T("PF_R5G6B5"),           3,  2,                      PLT_Compress,       PFF_NonEndian,                      5,6,5,0,    0xF800,0x07E0,0x001F,0,         11,5,0,0 },
        {_T("PF_B5G6R5"),           3,  2,                      PLT_Compress,       PFF_NonEndian,                      5,6,5,0,    0x001F,0x07E0,0xF800,0,         0,5,11,0 },
        {_T("PF_R3G3B2"),           3,  1,                      PLT_Compress,       PFF_NonEndian,                      3,3,2,0,    0xE0,0x1C,0x03,0,               5,2,0,0 },
        {_T("PF_A4R4G4B4"),         4,  2,                      PLT_Compress,       PFF_Alpha | PFF_NonEndian,          4,4,4,4,    0x0F00,0x00F0,0x000F,0xF000,    8,4,0,12 },
        {_T("PF_A1R5G5B5"),         4,  2,                      PLT_Compress,       PFF_Alpha | PFF_NonEndian,          5,5,5,1,    0x7C00,0x03E0,0x001F,0x8000,    10,5,0,15,},
        
        {_T("PF_A8R8G8B8"),         4,  4,                      PLT_R8G8B8A8,       PFF_Alpha | PFF_NonEndian,          8,8,8,8, 0xFF0000,0xFF00,0xFF,0xFF000000,   16, 8, 0, 24},
        {_T("PF_A8B8G8R8"),         4,  4,                      PLT_B8G8R8A8,       PFF_Alpha | PFF_NonEndian,          8,8,8,8, 0xFF,0xFF00,0xFF0000,0xFF000000,   0, 8, 16, 24, },
        {_T("PF_R8G8B8A8UN"),	    4u, 4u * sizeof(Nui8),		PLT_R8G8B8A8,	    PFF_Unit  | PFF_Alpha,              0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R8G8B8A8UN_SRGB"),	4u, 4u * sizeof(Nui8),		PLT_R8G8B8A8,	    PFF_Unit|PFF_SRGB | PFF_Alpha,      0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R8G8B8A8ui"),	    4u, 4u * sizeof(Nui8),		PLT_R8G8B8A8,	    PFF_Int|PFF_Alpha|PFF_NonEndian,    8,8,8,8, 0xFF000000,0xFF0000,0xFF00,0xFF,   24,16,8,0},
        {_T("PF_R8G8B8A8N"),	    4u, 4u * sizeof(Nui8),		PLT_R8G8B8A8,	    PFF_Unit |PFF_Signed | PFF_Alpha,   0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R8G8B8A8i"),	    4u, 4u * sizeof(Nui8),		PLT_R8G8B8A8,	    PFF_Int | PFF_Signed | PFF_Alpha,   0,0,0,0,        0,0,0,0, 0,0,0,0},
        
        {_T("PF_R16G16B16A16"),		4u, 4u * sizeof(uint16),	PLT_R16G16B16A16,   PFF_Float16 | PFF_Alpha,            16,16,16,16,    0,0,0,0, 0,0,0,0},
        {_T("PF_R16G16B16A16UN"),   4u, 4u * sizeof(uint16),	PLT_R16G16B16A16,   PFF_Unit | PFF_Alpha,               0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R16G16B16A16ui"),   4u, 4u * sizeof(uint16),	PLT_R16G16B16A16,   PFF_Int | PFF_Alpha | PFF_Int16,    16,16,16,16,    0,0,0,0, 0,0,0,0},
        {_T("PF_R16G16B16A16N"),    4u, 4u * sizeof(uint16),    PLT_R16G16B16A16,   PFF_Unit|PFF_Signed | PFF_Alpha,    0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R16G16B16A16i"),    4u, 4u * sizeof(uint16),	PLT_R16G16B16A16,   PFF_Int|PFF_Signed | PFF_Alpha,     0,0,0,0,        0,0,0,0, 0,0,0,0},
        
        {_T("PF_R32G32B32A32"),		4u, 4u * sizeof(uint32),	PLT_R32G32B32A32,	PFF_Float | PFF_Alpha,              32,32,32,32,    0,0,0,0, 0,0,0,0 },
        {_T("PF_R32G32B32A32ui"),	4u, 4u * sizeof(uint32),	PLT_R32G32B32A32,	PFF_Int | PFF_Alpha ,               0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R32G32B32A32i"),    4u, 4u * sizeof(uint32),	PLT_R32G32B32A32,	PFF_Int|PFF_Signed | PFF_Alpha,     0,0,0,0,        0,0,0,0, 0,0,0,0 },
        
        {_T("PF_R32G32B32"),	    3u, 3u * sizeof(uint32),	PLT_R32G32B32,	    PFF_Float,                          32,32,32,0,     0,0,0,0, 0,0,0,0 },
        {_T("PF_R32G32B32ui"),	    3u, 3u * sizeof(uint32),	PLT_R32G32B32,	    PFF_Int ,                           0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R32G32B32i"),	    3u, 3u * sizeof(uint32),	PLT_R32G32B32,	    PFF_Int|PFF_Signed ,                0,0,0,0,        0,0,0,0, 0,0,0,0 },
        
        {_T("PF_R8G8UN"),			2u, 2u * sizeof(Nui8),		PLT_R8G8,	        PFF_Unit,                           0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R8G8ui"),			2u, 2u * sizeof(Nui8),		PLT_R8G8,	        PFF_Int | PFF_NonEndian,            8,8,0,0,        0xFF0000,0x00FF00,0,0,  8,0,0,0},
        {_T("PF_R8G8N"),			2u, 2u * sizeof(Nui8),		PLT_R8G8,	        PFF_Unit|PFF_Signed,                0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R8G8i"),			2u, 2u * sizeof(Nui8),		PLT_R8G8,	        PFF_Int|PFF_Signed,                 0,0,0,0,        0,0,0,0, 0,0,0,0 },
        
        {_T("PF_G16R16"),           2,  4,                      PLT_R16G16,         PFF_Float16,                        16,16,0,0,      0,0,0,0, 0,0,0,0},
        {_T("PF_G16R16ui"),         2,  4,                      PLT_R16G16,         PFF_NonEndian|PFF_Int16,            16,16,0,0,      0xFFFF,0xFFFF0000,0,0,  0,16,0,0},
        {_T("PF_R16G16"),			2u, 2u * sizeof(uint16),	PLT_R16G16,	        PFF_Float16 ,                       0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R16G16UN"),			2u, 2u * sizeof(uint16),	PLT_R16G16,	        PFF_Unit,                           0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R16G16ui"),			2u, 2u * sizeof(uint16),	PLT_R16G16,	        PFF_Int,                            0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R16G16N"),			2u, 2u * sizeof(uint16),	PLT_R16G16,	        PFF_Unit|PFF_Signed,                0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R16G16i"),			2u, 2u * sizeof(uint16),	PLT_R16G16,	        PFF_Int|PFF_Signed,                 0,0,0,0,        0,0,0,0, 0,0,0,0 },
        
        {_T("PF_G32R32"),           2,  8,                      PLT_R32G32,         PFF_Float, PFF_Float,               32,32,0,0,      0,0,0,0, 0,0,0,0},
        {_T("PF_R32G32"),			2u, 2u * sizeof(uint32),	PLT_R32G32,	        PFF_Float ,                         0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R32G32ui"),			2u, 2u * sizeof(uint32),	PLT_R32G32,	        PFF_Int ,                           0,0,0,0,        0,0,0,0, 0,0,0,0},
        {_T("PF_R32G32i"),			2u, 2u * sizeof(uint32),	PLT_R32G32,	        PFF_Int | PFF_Signed ,              0,0,0,0,        0,0,0,0, 0,0,0,0},
        
        {_T("PF_R8UN",			    1u, 1u * sizeof(Nui8),		PLT_R8,		        PFF_Unit,                           0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R8ui",				1u, 1u * sizeof(Nui8),		PLT_R8,		        PFF_Int | PFF_NonEndian,            8,0,0,0,        0xFF0000,0,0,0, 0,0,0,0 },
        {_T("PF_R8N",			    1u, 1u * sizeof(Nui8),		PLT_R8,		        PFF_Unit|PFF_Signed,                0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R8i",				1u, 1u * sizeof(Nui8),		PLT_R8,		        PFF_Int|PFF_Signed,                 0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_A8UN"),			    1u, 1u * sizeof(Nui8),		PLT_R8,		        PFF_Unit | PFF_Alpha |PFF_NonEndian,0,0,0,8,        0,0,0,0xFF, 0,0,0,0},
        {_T("PF_R1UN",			    1u, 0,						0,                  0,                                  0,0,0,0,        0,0,0,0, 0,0,0,0}, // ???
        
        {_T("PF_R16"),			    1u, 1u * sizeof(uint16),	PLT_R16,	        PFF_Float16,                        16,0,0,0,       0,0,0,0, 0,0,0,0 },
        {_T("PF_R16UN"),			1u, 1u * sizeof(uint16),	PLT_R16,	        PFF_Unit,                           0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R16ui"),			1u, 1u * sizeof(uint16),	PLT_R16,	        PFF_Int,                            0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R16N"),			    1u, 1u * sizeof(uint16),	PLT_R16,	        PFF_Unit|PFF_Signed,                0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R16i"),			    1u, 1u * sizeof(uint16),	PLT_R16,	        PFF_Int|PFF_Signed,                 0,0,0,0,        0,0,0,0, 0,0,0,0 },
        
        {_T("PF_R32"),			    1u, 1u * sizeof(uint32),	PLT_R32,	        PFF_Float,                          32,0,0,0,       0,0,0,0, 0,0,0,0 },
        {_T("PF_R32ui"),			1u, 1u * sizeof(uint32),	PLT_R32,	        PFF_Int,                            0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_R32i"),			    1u, 1u * sizeof(uint32),	PLT_R32,	        PFF_Int|PFF_Signed,                 0,0,0,0,        0,0,0,0, 0,0,0,0 },
        
        {_T("PF_Depth16UN"),	    1u, 1u * sizeof(uint16),	PLT_R16,	        PFF_Unit | PFF_Depth,               0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_Depth24UN"),			1u, 1u * sizeof(uint32),PLT_Compress,	    PFF_Unit | PFF_Depth,               0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_Depth24UN_Stencil8ui"),	1u, 1u * sizeof(uint32),PLT_Compress,	    PFF_Unit | PFF_Depth | PFF_Stencil, 0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_Depth32f_Stencil8ui_64"),2u, 2u*sizeof(uint32), PLT_Compress,       PFF_Float | PFF_Depth | PFF_Stencil,0,0,0,0,        0,0,0,0, 0,0,0,0 },
        {_T("PF_Depth32"),			1u, 1u * sizeof(uint32),	PLT_R32,	        PFF_Float | PFF_Depth,              0,0,0,0,        0,0,0,0, 0,0,0,0 },
        
        {_T("PF_A2R10G10B10"),      4,  4,                      PLT_Compress,       PFF_Alpha | PFF_NonEndian,          10,10,10,2, 0x3FF00000, 0xFFC00, 0x3FF, 0xC0000000, 20,10,0,30 },
        {_T("PF_A2B10G10R10"),      4,  4,                      PLT_Compress,       PFF_Alpha | PFF_NonEndian,          10,10,10,2, 0x3FF, 0xFFC00, 0x3FF00000, 0xC0000000, 0,10,20,30 },
        {_T("PF_R10G10B10A2UN"),	4u, 1u * sizeof(uint32),	PLT_Compress,	    PFF_Unit | PFF_Alpha,               0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_R10G10B10A2ui"),	4u, 1u * sizeof(uint32),	PLT_Compress,	    PFF_Int | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_R11G11B10"),		3u, 1u * sizeof(uint32),	PLT_Compress,	    PFF_FloatE ,                        0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_R9G9B9E5",	        1u, 1u * sizeof(uint32),	PLT_Compress,	    PFF_FloatE ,                        0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_UYVY_R8G8B8G8UN",  4u, 4u * sizeof(Nui8),		PLT_Compress,	    PFF_Unit ,                          0,0,0,0, 0,0,0,0, 0,0,0,0},// NO ALPHA
        {_T("PF_YUY2_G8R8G8B8UN",  4u, 4u * sizeof(Nui8),		PLT_Compress,	    PFF_Unit|PFF_Signed,                0,0,0,0, 0,0,0,0, 0,0,0,0 }, // NO ALPHA
        
        {_T("PF_BC1UN",			    4u, 8,						PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_BC1UN_SRGB",		4u, 8,						PLT_Compress,	PF_COMPRESS | PFF_SRGB  | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_BC2UN",			    4u, 16,					    PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_BC2UN_SRGB",		4u, 16,						PLT_Compress,	PF_COMPRESS | PFF_SRGB  | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_BC3UN",			    4u, 16,						PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_BC3UN_SRGB",		4u, 16,						PLT_Compress,	PF_COMPRESS | PFF_SRGB | PFF_Alpha,     0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_BC4UN",			    1u, 8,						PLT_Compress,	PF_COMPRESS ,                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_BC4N",			    1u, 8,						PLT_Compress,	PF_COMPRESS |PFF_Signed ,               0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_BC5UN",			    2u, 16,						PLT_Compress,	PF_COMPRESS,                            0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_BC5N",			    2u, 16,						PLT_Compress,	PF_COMPRESS |PFF_Signed,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_B5G6R5UN",		    3u, 1u * sizeof(uint16),	PLT_Compress,	PFF_Unit,                               0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_B5G5R5A1UN",		3u, 1u * sizeof(uint16),	PLT_Compress,	PFF_Unit,                               0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_X8R8G8B8"),         3,  4,                      PLT_R8G8B8X8,       PFF_NonEndian,                      8,8,8,0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 16, 8, 0, 24 },
        {_T("PF_X8B8G8R8"),         3,  4,                      PLT_B8G8R8X8,       PFF_NonEndian,                      8,8,8,0, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 0, 8, 16, 24 },
        {_T("PF_B8G8R8A8UN",	    4u, 4u * sizeof(Nui8),		PLT_B8G8R8A8,	PFF_Unit | PFF_Alpha | PFF_NonEndian,   8,8,8,8, 0xFF00,0xFF0000,0xFF000000,0x000000FF, 8,16,24,0},
        {_T("PF_B8G8R8X8UN",	    3u, 4u * sizeof(Nui8),		PLT_B8G8R8X8,	PFF_Unit,                               0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_R10G10B10A2UN_XR_BIAS",4u, 1u * sizeof(uint32), PLT_Compress,   PFF_FloatE,                             0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_B8G8R8A8UN_SRGB",	4u, 4u * sizeof(Nui8),		PLT_B8G8R8A8,	PFF_Unit|PFF_SRGB  | PFF_Alpha,         0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_B8G8R8X8UN_SRGB",	3u, 4u * sizeof(Nui8),		PLT_B8G8R8X8,	PFF_Unit|PFF_SRGB,                      0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_BC6H_u16",			3u, 16,						PLT_Compress,	PFF_Compress | PFF_FloatE ,             0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_BC6H_16",			3u, 16,						PLT_Compress,	PFF_Compress | PFF_FloatE | PFF_Signed, 0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_BC7_UN",			4u, 16,						PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_BC7_UN_SRGB",		4u, 16,						PLT_Compress,	PF_COMPRESS | PFF_SRGB | PFF_Alpha,     0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_AYUV",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_Y410",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_Y416",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_NV12",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_P010",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_P016",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_420_OPAQUE",	    3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_YUY2",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_Y210",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_Y216",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_NV11",				3u, 0,						0, PFF_None ,                                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_AI44",				3u, 0,						0,              PFF_Alpha,                              0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_IA44",				3u, 0,						0,              PFF_Alpha,                              0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_P8",			    1u, 1u * sizeof(Nui8),		PLT_Compress,	PFF_Pallete,                            0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_A8P8",				2u, 2u * sizeof(Nui8),		PLT_Compress,	PFF_Pallete | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_B4G4R4A4UN",		4u, 1u * sizeof(uint16),	PLT_Compress,	PFF_Unit | PFF_Alpha,                   0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_P208",				3u, 0,						0, 0,                                                   0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_V208",				3u, 0,						0, 0,                                                   0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_V408",				3u, 0,						0, 0,                                                   0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_R8G8B8UN",			3u, 3u * sizeof(Nui8),		PLT_R8G8B8,	    PFF_Unit | PFF_NonEndian,               8,8,8,0, 0xFF0000,0xFF00,0x0000FF,0, 16,8,0,0 },
        {_T("PF_R8G8B8UN_SRGB",     3u, 3u * sizeof(Nui8),     PLT_R8G8B8,	    PFF_Unit|PFF_SRGB,                      0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_B8G8R8UN",			3u, 3u * sizeof(Nui8),		PLT_B8G8R8,	    PFF_Unit | PFF_NonEndian,               8,8,8,0, 0x0000FF,0xFF00,0xFF0000,0, 0,8,16,0 },
        {_T("PF_B8G8R8UN_SRGB",     3u, 3u * sizeof(Nui8),     PLT_B8G8R8,	    PFF_Unit|PFF_SRGB,                      0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_R16G16B16ui"),      3,  6,                      PLT_R16G16B16,  PFF_Int16,                              16,16,16,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_R16G16B16UN",	    3u, 3u * sizeof(uint16),    PLT_R16G16B16,	PFF_Unit,                               16,16,16,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_PVRTC_RGB_2BPP",        3u, 32,				    PLT_Compress,	PF_COMPRESS,                            0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_PVRTC_RGB_2BPP_SRGB",   3u, 32,			        PLT_Compress,	PF_COMPRESS| PFF_SRGB ,                 0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_PVRTC_RGBA_2BPP",	    4u, 32,			        PLT_Compress,	PF_COMPRESS| PFF_Alpha,                 0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC_RGBA_2BPP_SRGB",	4u, 32,				    PLT_Compress,	PF_COMPRESS| PFF_SRGB | PFF_Alpha,      0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC_RGB_4BPP",	    3u, 32,			        PLT_Compress,	PF_COMPRESS,                            0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_PVRTC_RGB_4BPP_SRGB",   3u, 32,			        PLT_Compress,	PF_COMPRESS|PFF_SRGB,                   0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC_RGBA_4BPP",	    4u, 32,			        PLT_Compress,	PF_COMPRESS| PFF_Alpha,                 0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC_RGBA_4BPP_SRGB",	4u, 32,				    PLT_Compress,	PF_COMPRESS|PFF_SRGB| PFF_Alpha,        0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC2_2BPP",			3u, 32,				    PLT_Compress,	PF_COMPRESS,                            0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC2_2BPP_SRGB",	    3u, 32,				    PLT_Compress,	PF_COMPRESS|PFF_SRGB,                   0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC2_4BPP",			3u, 32,				    PLT_Compress,	PF_COMPRESS,                            0,0,0,0, 0,0,0,0, 0,0,0,0 },
        {_T("PF_PVRTC2_4BPP_SRGB",      3u, 32,				    PLT_Compress,	PF_COMPRESS|PFF_SRGB,                   0,0,0,0, 0,0,0,0, 0,0,0,0 },

        {_T("PF_ETC1_R8G8B8UN",		    3u, 8,				    PLT_Compress,	PF_COMPRESS ,                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ETC2_R8G8B8UN",		    3u, 8,				    PLT_Compress,	PF_COMPRESS ,                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ETC2_R8G8B8UN_SRGB",    3u, 8,				    PLT_Compress,	PF_COMPRESS|PFF_SRGB ,                  0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ETC2_R8G8B8A8UN",	    4u, 16,				    PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ETC2_R8G8B8A8UN_SRGB",  4u, 16,				    PLT_Compress,	PF_COMPRESS|PFF_SRGB | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ETC2_R8G8B8A1UN",	    4u, 8,				    PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ETC2_R8G8B8A1UN_SRGB",  4u, 8,					PLT_Compress,	PF_COMPRESS|PFF_SRGB | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        
        {_T("PF_EAC_R11UN",		        1u, 8,				    PLT_Compress,	PF_COMPRESS ,                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_EAC_R11N",		        1u, 8,				    PLT_Compress,	PF_COMPRESS|PFF_Signed ,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_EAC_R11G11UN",	        2u, 16,				    PLT_Compress,	PF_COMPRESS ,                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_EAC_R11G11N",	        2u, 16,				    PLT_Compress,	PF_COMPRESS|PFF_Signed,                 0,0,0,0, 0,0,0,0, 0,0,0,0 },

        {_T("PF_ATC_RGB",				    3u, 8,			    PLT_Compress,	PF_COMPRESS ,                           0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ATC_RGBA_EXPLICIT_A",	    4u, 16,			    PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ATC_RGBA_INTERPOLATED_A",   4u, 16,			    PLT_Compress,	PF_COMPRESS | PFF_Alpha,                0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_ASTC_RGBAUN_4X4_LDR",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_5X4_LDR",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_5X5_LDR",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_6X5_LDR",       4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_6X6_LDR",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_8X5_LDR",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_8X6_LDR",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_8X8_LDR",       4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X5_LDR",		4u, 16,		        PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X6_LDR",		4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X8_LDR",		4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X10_LDR",     4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_12X10_LDR",     4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_12X12_LDR",     4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit | PFF_Alpha,       0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_ASTC_RGBAUN_4X4_SRGB",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_5X4_SRGB",	    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_5X5_SRGB",		4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_6X5_SRGB",      4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_6X6_SRGB",		4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_8X5_SRGB",		4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_8X6_SRGB",		4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_8X8_SRGB",      4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X5_SRGB",     4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X6_SRGB",     4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X8_SRGB",     4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_10X10_SRGB",    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_12X10_SRGB",    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},
        {_T("PF_ASTC_RGBAUN_12X12_SRGB",    4u, 16,			    PLT_Compress,	PF_COMPRESS|PFF_Unit|PFF_SRGB | PFF_Alpha,    0,0,0,0, 0,0,0,0, 0,0,0,0},

        {_T("PF_COUNT",                     1u,                 0,              0,                                      0,0,0,0, 0,0,0,0, 0,0,0,0 },
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PixelVolume
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PixelVolume::PixelVolume():
        mWidth(0), 
        mHeight(0), 
        mDepth(0), 
        mArray(0)
    {
    }
    //-----------------------------------------------------------------------
    PixelVolume::PixelVolume(NCount width, NCount height, NCount depth, PixelFormat pf):
        mWidth(width), 
        mHeight(height), 
        mDepth(depth), 
        mArray(1),
        mFormat(pf)
    {
    }
    //-----------------------------------------------------------------------
    PixelVolume::PixelVolume(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf):
        mWidth(width), 
        mHeight(height), 
        mDepth(depth), 
        mArray(array),
        mFormat(pf)
    {
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PixelBlock
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PixelBlock::PixelBlock():
        PixelVolume(),
        mUnitSize(0), mRowSize(0), mSliceSize(0), mData(0)
    {
    }
    //-----------------------------------------------------------------------
    PixelBlock::PixelBlock(const Box & ext, PixelFormat pf, Nui8 * data):
        PixelVolume(ext.getWidth(), ext.getHeight(), ext.getDepth(), pf),
        mLeft(ext.mLeft),
        mTop(ext.mTop), 
        mFront(ext.mFront),
        mArrayIdx(0),
        mFormat(pf),
        mData(data)
    {
        mUnitSize = PixelUtil::getUnitSize(mFormat);        // prepixel
        mRowSize = mWidth * mUnitSize;                      // perRow
        mSliceSize = mRowSize * mHeight;                    // perImage
    }
    //-----------------------------------------------------------------------
    PixelBlock::PixelBlock(NCount width, NCount height, NCount rowsize, PixelFormat pf, Nui8 * data):
        PixelVolume(width, height, 1, 1, pf),
        mLeft(0), mTop(0), mFront(0), mArrayIdx(0),
        mUnitSize(PixelUtil::getUnitSize(pf)),
        mRowSize(rowSize),
        mSliceSize(rowsize * height),
        mData(data)
    {
    }
    //-----------------------------------------------------------------------
    PixelBlock::PixelBlock(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf, Nui8 * data):
        PixelVolume(width, height, depth, array, pf),
        mLeft(0), mTop(0), mFront(0), mArrayIdx(0),
        mData(data),
    {
        mUnitSize = PixelUtil::getUnitSize(mFormat);        //prepixel
        mRowSize = mWidth * mUnitSize;                      // perRow getSize(mWidth, 1u, 1u, 1u, mFormat);
        mSliceSize = mRowSize * mHeight;                    // perImage
    }
    //-----------------------------------------------------------------------
    PixelBlock(NCount width, NCount height, NCount depth, NCount array, NCount unitSize, NCount rowSize,
        NCount sliceSize, PixelFormat pf, Nui8 * data) :
        PixelVolume(width, height, depth, array, pf),
        mLeft(0), mTop(0), mFront(0), mArrayIdx(0),
        mUnitSize(unitSize), mRowSize(rowSize), mSliceSize(sliceSize),
        mData(data)
    {
    }
    //-----------------------------------------------------------------------
    PixelBlock::~PixelBlock()
    {
    }
    //-----------------------------------------------------------------------
    Nui8 * PixelBlock::getOriginData(NCount x, NCount y, NCount z) const         
    {
        if( isCompress() )
        {
            uint32 blockWidth = PixelUtil::getBlockWidth(mFormat, false);
            uint32 blockHeight= PixelUtil::getBlockHeight(mFormat, false);
            const size_t yBlock = y / blockHeight;
            const size_t xBlock = x / blockWidth;
            return mData + z * mSliceSize + yBlock * mRowSize + xBlock * PixelUtil::getUnitSize(mFormat);
        }

        return mData + x * mUnitSize + y * mRowSize + z * mSliceSize;
    }
    //-----------------------------------------------------------------------
    bool PixelBlock::isWholeSlice() const 
    {
        if( mLeft == 0u && mTop == 0u )
            return true;

        if( isCompress() )
        {
            uint32 blockWidth = PixelUtil::getBlockWidth(mFormat, false);
            uint32 blockHeight = PixelUtil::getBlockHeight(mFormat, false);

            return ( mRowSize == PixelUtil::getUnitSize(mFormat) * ( mWidth + blockWidth - 1u ) / blockWidth ) &&
                mSliceSize == mRowSize * ( mHeight + blockHeight - 1u / blockHeight );
        }

        return ( mRowSize == mUnitSize * mWidth && mSliceSize == mRowSize * mHeight );
    }
    //-----------------------------------------------------------------------
    PixelBlock PixelBlock::getSub(const Box & def) const
    {
        if (!contains(def))
            N_EXCEPT(InvalidParam, _T("Bounds out of range"));

        // Calculate new pixelbox and optionally reset origin.
        PixelBlock re(def, mFormat, mData);
        re.mRowSize = mRowSize;
        re.mSliceSize = mSliceSize;

        re.mData = re.getOriginData(re.mLeft, re.mTop, re.getVolumeIdex());
        re.mFront = re.mTop = re.mLeft = 0;

        return re;
    }
    //-----------------------------------------------------------------------
    void PixelBlock::write(const PixelBlock & src)
    {
        assert(mWidth == src.mWidth && mHeight == src.mHeight && getVolume() >= src.getVolume());

        const NCount volume = src.getVolume();
        const NCount srcZorSlice = src.getVolumeIdex();
        const NCount dstZorSlice = getVolumeIdex();

        if(mRowSize == src.mRowSize && mSliceSize == src.mSliceSize && isWholeSlice() && src.isWholeSlice())
        {
            //Raw copy
            const void * srcData = src.getOriginData(0, 0, srcZorSlice);
            void * data = getOriginData(0, 0, dstZorSlice);
            memcpy( data, srcData, mSliceSize * volume );
        }
        else
        {
            if( isCompress() )
            {
                //Copy row of blocks by row of blocks, compressed.
                //PixelFormat pf = getFormat();
                NCount blockHeight = PixelUtil::getBlockHeight( mFormat, false );
                //NCount finalHeight = mHeight;
                NCount rowsize = std::min( mRowSize, src.mRowSize );
                for( NCount z = 0; z < volume; ++z )
                {
                    for( NCount y = 0; y < mHeight; y += blockHeight )
                    {
                        const Nui8 * srcData = src.getOriginData( src.mLeft,   y + src.mTop,   z + srcZorSlice );
                        Nui8 * data = getOriginData( mLeft, y + mTop, z + dstZorSlice );
                        memcpy( data, srcData, rowsize );
                    }
                }
            }
            else
            {
                //Copy row by row, uncompressed.
                //NCount finalHeight = mHeight;
                NCount rowsize = std::min( mRowSize, src.mRowSize );
                for( NCount z = 0; z < volume; ++z )
                {
                    for( NCount y = 0; y < mHeight; ++y )
                    {
                        const Nui8 * srcData = src.getOriginData( src.mLeft,   y + src.mTop,   z + srcZorSlice );
                        Nui8 * data = getOriginData( mLeft, y + mTop, z + dstZorSlice );
                        memcpy( data, srcData, rowsize );
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void PixelBlock::getColour(Colour & out, NCount x, NCount y, NCount z)
    {
        const void * src = getData( x, y, z );
        PixelUtil::unpackColour(&out, mFormat, src );
    }
    //-----------------------------------------------------------------------
    void PixelBlock::setColour(const Colour & c, NCount x, NCount y, NCount z)
    {
        void * dst = getData( x, y, z );
        PixelUtil::packColour( c, mFormat, dst );
    }
    //-----------------------------------------------------------------------
    void PixelBlock::verticalFlip()
    {
        // Check for compressed formats, we don't support decompression, compression or recoding
        if (PixelUtil::isCompress(mFormat))
        {
            N_EXCEPT(UnImpl, _T("This method can not be used for compressed formats"));
        }

        const size_t copySize = mWidth * mUnitSize;

        Nui8 * basesrcptr = getOriginData(mLeft, mTop, mFront);
        Nui8 * basedstptr = basesrcptr + (mHeight - 1) * mRowSize;
        Nui8 * tmpptr = (Nui8 *)N_alloc_align(copySize, 16);

        // swap rows
        const size_t halfRowCount = mHeight >> 1;
        for (NCount z = mFront; z < mFront + mDepth; ++z)
        {
            Nui8* srcptr = basesrcptr;
            Nui8* dstptr = basedstptr;
            for (NCount y = 0; y < halfRowCount; ++y)
            {
                // swap rows
                memcpy(tmpptr, dstptr, copySize);
                memcpy(dstptr, srcptr, copySize);
                memcpy(srcptr, tmpptr, copySize);
                srcptr += mRowSize;
                dstptr -= mRowSize;
            }
            basesrcptr += mSliceSize;
            basedstptr += mSliceSize;
        }

        N_free_align(tmpptr, 16);
    }
    //------------------------------------------------------------------------
    void PixelBlock::scale(const PixelBlock & src1, PixelBlock & scaled1, Filter filter)
    {
        N_assert(PixelUtil::isDirect(src.mFormat), "error");
        N_assert(PixelUtil::isDirect(scaled.mFormat), "error");
        
        PixelBlock src = src1, scaled = scaled1;
        for(NCount sliceIdx = 0; sliceIdx < src.mArray; ++sliceIdx, ++src.mArrayIdx, scaled.mArrayIdx)
        {
            MemStream * buf = 0;
            
            PixelBlock temp;
            switch (filter)
            {
            default:
            //三次线性
            case F_Nearest:
            {
                if (src.mFormat == scaled.mFormat)
                {
                    temp = scaled;
                }
                else
                {
                    // 分配资源目的大小的临时缓存
                    temp = PixelBlock(scaled.getWidth(), scaled.getHeight(), scaled.mDepth, src.mFormat);
                    buf = N_new MemStream(temp.getWholeSliceSize());
                    temp.mData = buf->getData();
                }
                // 超级优化:无需转换
                switch (PixelUtil::getUnitSize(src.mFormat))
                {
                case 1: NearestResampler<1>::scale(src, temp); break;
                case 2: NearestResampler<2>::scale(src, temp); break;
                case 3: NearestResampler<3>::scale(src, temp); break;
                case 4: NearestResampler<4>::scale(src, temp); break;
                case 6: NearestResampler<6>::scale(src, temp); break;
                case 8: NearestResampler<8>::scale(src, temp); break;
                case 12: NearestResampler<12>::scale(src, temp); break;
                case 16: NearestResampler<16>::scale(src, temp); break;
                default:
                    N_assert(false, "error");
                }
                if (temp.mData != scaled.mData)
                {
                    PixelUtil::conv(temp, scaled);
                }
                if (buf != 0)
                {
                    N_delete buf;
                    buf = 0;
                }
            }
            break;
            case F_Linear:
            case F_Bilinear:
            {
                switch (src.mFormat)
                {
                case PF_R8G8B8A8UN:
                case PF_B8G8R8X8UN:
                case PF_R8G8UN: case PF_R8G8ui:
                case PF_R8UN: case PF_R8ui:
                case PF_L8ui: case PF_A8UN: case PF_L8A8i:
                case PF_R8G8B8UN: case PF_B8G8R8UN:
                case PF_R8G8B8A8ui: case PF_B8G8R8A8UN:
                case PF_A8B8G8R8: case PF_A8R8G8B8:
                case PF_X8B8G8R8: case PF_X8R8G8B8:
                    if (src.mFormat == scaled.mFormat)
                    {
                        // 没有中间缓存需要
                        temp = scaled;
                    }
                    else
                    {
                        // 分配资源目的大小的临时缓存
                        temp = PixelBlock(scaled.getWidth(), scaled.getHeight(), scaled.mDepth, src.mFormat);
                        buf = N_new MemStream(temp.getWholeSliceSize());
                        temp.mData = buf->getData();
                    }
                    // 超级优化:面向字节的数学,无需转换
                    switch (PixelUtil::getUnitSize(src.mFormat))
                    {
                    case 1: LinearResampler_Byte<1>::scale(src, temp); break;
                    case 2: LinearResampler_Byte<2>::scale(src, temp); break;
                    case 3: LinearResampler_Byte<3>::scale(src, temp); break;
                    case 4: LinearResampler_Byte<4>::scale(src, temp); break;
                    default:
                        N_assert(false, "error");
                    }
                    if (temp.mData != scaled.mData)
                    {
                        // 位块传送temp缓存
                        PixelUtil::conv(temp, scaled);
                    }
                    if (buf != 0)
                    {
                        N_delete buf;
                        buf = 0;
                    }
                    break;
                case PF_R32G32B32:
                case PF_R32G32B32A32:
                case PF_R32G32:
                case PF_R32:
                    if (scaled.mFormat == PF_R32G32B32 || scaled.mFormat == PF_R32G32B32A32 ||
                        scaled.mFormat == PF_R32G32 || scaled.mFormat == PF_R32)
                    {
                        // float32到float32,避免 解包/装包 开销
                        LinearResampler_Float32::scale(src, scaled);
                        break;
                    }
                default:
                    // 非优化:浮点运算,执行转换但总是能工作的
                    LinearResampler::scale(src, scaled);
                }
            }
            break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void PixelBlock::applyGamma(Nui8 * buffer, NIIf gamma, NCount size, NCount bpp)
    {
        if(gamma == 1.0f)
            return;

        //注意仅 24/32-位支持
        if(bpp != 24 && bpp != 32)
            return;

        Nui32 stride = bpp >> 3;

        for(NCount i = 0, j = size / stride; i < j; ++i, buffer += stride)
        {
            NIIf r, g, b;

            const NIIf rangeMult = 255.0f;
            const NIIf rangeMultInv = 1.0f / rangeMult;
            const NIIf gammaValue = 1.0f / gamma;

            r = rangeMultInv * buffer[0];
            g = rangeMultInv * buffer[1];
            b = rangeMultInv * buffer[2];

            Math::Pow(r, gammaValue);
            Math::Pow(g, gammaValue);
            Math::Pow(b, gammaValue);

            buffer[0] = (Nui8)(r * rangeMult);
            buffer[1] = (Nui8)(g * rangeMult);
            buffer[2] = (Nui8)(b * rangeMult);
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PixelUtil
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    /**
    * Directly get the description record for provided pixel mFormat. For debug builds,
    * this checks the bounds of fmt with an assertion.
    */
    static inline const PixelFormatDescription & getVerbose(PixelFormat fmt)
    {
        const NIIi ord = (NIIi)fmt;
        N_assert(ord >= 0 && ord < PF_COUNT, "error");

        return msPixelFormatDesc[ord];
    }
    //-----------------------------------------------------------------------
    NCount PixelUtil::getUnitSize(PixelFormat pf)
    {
        return getVerbose(pf).mSize;
    }
    //-----------------------------------------------------------------------
    Nui32 PixelUtil::getExentMark(PixelFormat pf)
    {
        return getVerbose(pf).mFeature;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isAlpha(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Alpha) == PFF_Alpha;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isFloat(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Float) == PFF_Float;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isCompress(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Compress) == PFF_Compress;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isDepth(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Depth) == PFF_Depth;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isNativeEndian(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_NonEndian) == PFF_NonEndian;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isGrayscale(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Grayscale) == PFF_Grayscale;
    }
    //-----------------------------------------------------------------------
    void PixelUtil::getBitDepth(PixelFormat pf, NIIi rgba[4])
    {
        const PixelFormatDescription & des = getVerbose(pf);
        rgba[0] = des.rbits;
        rgba[1] = des.gbits;
        rgba[2] = des.bbits;
        rgba[3] = des.abits;
    }
    //-----------------------------------------------------------------------
    void PixelUtil::getBitMask(PixelFormat pf, Nui32 rgba[4])
    {
        const PixelFormatDescription & des = getVerbose(pf);
        rgba[0] = des.rmask;
        rgba[1] = des.gmask;
        rgba[2] = des.bmask;
        rgba[3] = des.amask;
    }
    //---------------------------------------------------------------------
    void PixelUtil::getBitShift(PixelFormat pf, Nui8 rgba[4])
    {
        const PixelFormatDescription & des = getVerbose(pf);
        rgba[0] = des.rshift;
        rgba[1] = des.gshift;
        rgba[2] = des.bshift;
        rgba[3] = des.ashift;
    }
    //-----------------------------------------------------------------------
    StringView PixelUtil::getMarkName(PixelFormat srcpf)
    {
        return getVerbose(srcpf).mName;
    }
    //-----------------------------------------------------------------------
    NCount PixelUtil::getDataCount(PixelFormat pf)
    {
        const PixelFormatDescription & dep = getVerbose(pf);
        return dep.mDataCount;
    }
    //-----------------------------------------------------------------------
    PixelFormat PixelUtil::getFormat(const String & name, Nui32 exclusionfeature, bool direct)
    {
        for (NIIi i = 0; i < PF_COUNT; ++i)
        {
            PixelFormat pf = (PixelFormat)i;
            const PixelFormatDescription & desc = getVerbose( pf );
            if ((!direct || isDirect(pf) && (desc.mFeature & exclusionFlags) == 0)
            {
                if (strcmp( name, desc.name ) == 0)
                    return pf;
            }
        }
        return PF_UNKNOWN;
    }
    //-----------------------------------------------------------------------
    PixelFormat PixelUtil::getFormat(PixelFormat pf, Nui16 integerBits, Nui16 floatBits)
    {
        switch (integerBits)
        {
        case 16:
            switch (pf)
            {
            case PF_R8G8B8UN:
            case PF_X8R8G8B8:
                return PF_R5G6B5;

            case PF_B8G8R8UN:
            case PF_X8B8G8R8:
                return PF_B5G6R5;

            case PF_A8R8G8B8:
            case PF_R8G8B8A8ui:
            case PF_A8B8G8R8:
            case PF_B8G8R8A8UN:
                return PF_A4R4G4B4;

            case PF_A2R10G10B10:
            case PF_A2B10G10R10:
                return PF_A1R5G5B5;

            default:
                // use original image mFormat
                break;
            }
            break;

        case 32:
            switch (pf)
            {
            case PF_R5G6B5:
                return PF_X8R8G8B8;

            case PF_B5G6R5:
                return PF_X8B8G8R8;

            case PF_A4R4G4B4:
                return PF_A8R8G8B8;

            case PF_A1R5G5B5:
                return PF_A2R10G10B10;

            default:
                // use original image mFormat
                break;
            }
            break;

        default:
            // use original image mFormat
            break;
        }

        switch (floatBits)
        {
        case 16:
            switch (pf)
            {
            case PF_R32:
                return PF_R16;

            case PF_R32G32B32:
                return PF_R16G16B16UN;

            case PF_R32G32B32A32:
                return PF_R16G16B16A16;

            default:
                // use original image mFormat
                break;
            }
            break;

        case 32:
            switch (pf)
            {
            case PF_R16:
                return PF_R32;

            case PF_R16G16B16UN:
                return PF_R32G32B32;

            case PF_R16G16B16A16:
                return PF_R32G32B32A32;

            default:
                // use original image mFormat
                break;
            }
            break;
        default:
            break;
        }
        return pf;
    }
    
    //-----------------------------------------------------------------------------------
    float toSRGB(float x)
    {
        if( x <= 0.0031308f )
            return 12.92f * x;
        else
            return 1.055f * powf( x, (1.0f / 2.4f) ) - 0.055f;
    }
    //-----------------------------------------------------------------------------------
    float fromSRGB(float x)
    {
        if( x <= 0.040449907f )
            return x / 12.92f;
        else
            return powf( (x + 0.055f) / 1.055f, 2.4f );
    }
    //-----------------------------------------------------------------------------------
    template <typename T> void convertFromFloat4(NIIf r, NIIf g, NIIf b, NIIf a, void * dstPtr, uint32 flags)
    {
        if(flags & PFF_Float)
        {
            NIIf * temp = (NIIf *)dstPtr;
            temp[0] = r;
            temp[1] = g;
            temp[2] = b;
            temp[3] = a;
        }
        else if(flags & PFF_Float16)
        {
            Nui16 * temp = (Nui16 *)dstPtr;
            temp[0] = Bitwise::floatToHalf(r);
            temp[1] = Bitwise::floatToHalf(g);
            temp[2] = Bitwise::floatToHalf(b);
            temp[3] = Bitwise::floatToHalf(a);
        }
        else if(flags & PFF_Unit)
        {
            T * temp = (T *)dstPtr;
            if(flags & PFF_Signed)
            {
                temp[0] = static_cast<T>(roundf(std::clamp(r, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                temp[1] = static_cast<T>(roundf(std::clamp(g, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                temp[2] = static_cast<T>(roundf(std::clamp(b, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                temp[3] = static_cast<T>(roundf(std::clamp(a, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    temp[0] = static_cast<T>(roundf(toSRGB(std::clamp(r, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                    temp[1] = static_cast<T>(roundf(toSRGB(std::clamp(g, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                    temp[2] = static_cast<T>(roundf(toSRGB(std::clamp(b, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                    temp[3] = static_cast<T>(roundf(std::clamp(a, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                }
                else
                {
                    temp[0] = static_cast<T>(roundf(std::clamp(r, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                    temp[1] = static_cast<T>(roundf(std::clamp(g, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                    temp[2] = static_cast<T>(roundf(std::clamp(b, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                    temp[3] = static_cast<T>(roundf(std::clamp(a, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                }
            }
        }
        else
        {
            T * temp = (T *)dstPtr;
            temp[0] = static_cast<T>(roundf(r));
            temp[1] = static_cast<T>(roundf(g));
            temp[2] = static_cast<T>(roundf(b));
            temp[3] = static_cast<T>(roundf(a));
        }
    }
    //-----------------------------------------------------------------------------------
    template <typename T> void convertFromFloat3(NIIf r, NIIf g, NIIf b, NIIf, void * dstPtr, uint32 flags )
    {
        if(flags & PFF_Float)
        {
            NIIf * temp = (NIIf *)dstPtr;
            temp[0] = r;
            temp[1] = g;
            temp[2] = b;
        }
        else if(flags & PFF_Float16)
        {
            Nui16 * temp = (Nui16 *)dstPtr;
            temp[0] = Bitwise::floatToHalf(r);
            temp[1] = Bitwise::floatToHalf(g);
            temp[2] = Bitwise::floatToHalf(b);
        }
        else if(flags & PFF_Unit)
        {
            T * temp = (T *)dstPtr;
            if(flags & PFF_Signed)
            {
                temp[0] = static_cast<T>(roundf(std::clamp(r, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                temp[1] = static_cast<T>(roundf(std::clamp(g, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                temp[2] = static_cast<T>(roundf(std::clamp(b, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    temp[0] = static_cast<T>(roundf(toSRGB(std::clamp(r, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                    temp[1] = static_cast<T>(roundf(toSRGB(std::clamp(g, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                    temp[2] = static_cast<T>(roundf(toSRGB(std::clamp(b, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                }
                else
                {
                    temp[0] = static_cast<T>(roundf(std::clamp(r, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                    temp[1] = static_cast<T>(roundf(std::clamp(g, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                    temp[2] = static_cast<T>(roundf(std::clamp(b, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                }
            }
        }
        else
        {
            T * temp = (T *)dstPtr;
            temp[0] = static_cast<T>(roundf(r));
            temp[1] = static_cast<T>(roundf(g));
            temp[2] = static_cast<T>(roundf(b));
        }
    }
    //-----------------------------------------------------------------------------------
    template <typename T> void convertFromFloat2(NIIf r, NIIf g, NIIf, NIIf, void * dstPtr,  uint32 flags)
    {
        if(flags & PFF_Float)
        {
            NIIf * temp = (NIIf *)dstPtr;
            temp[0] = r;
            temp[1] = g;
        }
        else if(flags & PFF_Float16)
        {
            Nui16 * temp = (Nui16 *)dstPtr;
            temp[0] = Bitwise::floatToHalf(r);
            temp[1] = Bitwise::floatToHalf(g);
        }
        else if(flags & PFF_Unit)
        {
            T * temp = (T *)dstPtr;
            if(flags & PFF_Signed)
            {
                temp[0] = static_cast<T>(roundf(std::clamp(r, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                temp[1] = static_cast<T>(roundf(std::clamp(g, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    temp[0] = static_cast<T>(roundf(toSRGB(std::clamp(r, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                    temp[1] = static_cast<T>(roundf(toSRGB(std::clamp(g, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                }
                else
                {
                    temp[0] = static_cast<T>(roundf(std::clamp(r, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                    temp[1] = static_cast<T>(roundf(std::clamp(g, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                }
            }
        }
        else
        {
            T * temp = (T *)dstPtr;
            temp[0] = static_cast<T>(roundf(r));
            temp[1] = static_cast<T>(roundf(g));
        }
    }
    //-----------------------------------------------------------------------------------
    template <typename T> void convertFromFloat1(NIIf r, NIIf, NIIf, NIIf, void * dstPtr, uint32 flags)
    {
        if(flags & PFF_Float)
        {
            NIIf * temp = (NIIf *)dstPtr;
            temp[0] = r;
        }
        else if(flags & PFF_Float16)
        {
            Nui16 * temp = (Nui16 *)dstPtr;
            temp[0] = Bitwise::floatToHalf(r);
        }
        else if(flags & PFF_Unit)
        {
            T * temp = (T *)dstPtr;
            if(flags & PFF_Signed)
            {
                temp[0] = static_cast<T>(roundf(std::clamp(r, -1.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    temp[0] = static_cast<T>(roundf(toSRGB(std::clamp(r, 0.0f, 1.0f)) * (float)std::numeric_limits<T>::max()));
                }
                else
                {
                    temp[0] = static_cast<T>(roundf(std::clamp(r, 0.0f, 1.0f) * (float)std::numeric_limits<T>::max()));
                }
            }
        }
        else
        {
            T * temp = (T *)dstPtr;
            temp[0] = static_cast<T>(roundf(r));
        }
    }
    //-----------------------------------------------------------------------------------
    template <typename T> void convertToFloat4(NIIf & r, NIIf & g, NIIf & b, NIIf & a, const void * srcPtr, uint32 flags)
    {
        if(flags & PFF_Float)
        {
            const NIIf * temp = (const NIIf *)srcPtr;
            r = temp[0];
            g = temp[1];
            b = temp[2];
            a = temp[3];
        }
        else if(flags & PFF_Float16)
        {
            const Nui16 * temp = (const Nui16 *)srcPtr;
            r = Bitwise::halfToFloat(temp[0]);
            g = Bitwise::halfToFloat(temp[1]);
            b = Bitwise::halfToFloat(temp[2]);
            a = Bitwise::halfToFloat(temp[3]);
        }
        else if(flags & PFF_Unit)
        {
            const T * temp = (const T *)srcPtr;
            if(flags & PFF_Signed)
            {
                // -128 & -127 and -32768 & -32767 both map to -1 according to D3D10 rules.
                r = std::max( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max(), -1.0f );
                g = std::max( static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max(), -1.0f );
                b = std::max( static_cast<float>( temp[2] ) / (float)std::numeric_limits<T>::max(), -1.0f );
                a = std::max( static_cast<float>( temp[3] ) / (float)std::numeric_limits<T>::max(), -1.0f );
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    r = fromSRGB( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max() );
                    g = fromSRGB( static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max() );
                    b = fromSRGB( static_cast<float>( temp[2] ) / (float)std::numeric_limits<T>::max() );
                    a = static_cast<float>( temp[3] ) / (float)std::numeric_limits<T>::max();
                }
                else
                {
                    r = static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max();
                    g = static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max();
                    b = static_cast<float>( temp[2] ) / (float)std::numeric_limits<T>::max();
                    a = static_cast<float>( temp[3] ) / (float)std::numeric_limits<T>::max();
                }
            }
        }
        else
        {
            const T * temp = (const T *)srcPtr;
            r = static_cast<float>( temp[0] );
            g = static_cast<float>( temp[1] );
            b = static_cast<float>( temp[2] );
            a = static_cast<float>( temp[3] );
        }
    }
    
    //-----------------------------------------------------------------------------------
    template <typename T> void convertToFloat3(NIIf & r, NIIf & g, NIIf & b, NIIf & a, const void * srcPtr, uint32 flags)
    {
        if(flags & PFF_Float)
        {
            const NIIf * temp = (const NIIf *)srcPtr;
            r = temp[0];
            g = temp[1];
            b = temp[2];
        }
        else if(flags & PFF_Float16)
        {
            const Nui16 * temp = (const Nui16 *)srcPtr;
            r = Bitwise::halfToFloat(temp[0]);
            g = Bitwise::halfToFloat(temp[1]);
            b = Bitwise::halfToFloat(temp[2]);
        }
        else if(flags & PFF_Unit)
        {
            const T * temp = (const T *)srcPtr;
            if(flags & PFF_Signed)
            {
                // -128 & -127 and -32768 & -32767 both map to -1 according to D3D10 rules.
                r = std::max( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max(), -1.0f );
                g = std::max( static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max(), -1.0f );
                b = std::max( static_cast<float>( temp[2] ) / (float)std::numeric_limits<T>::max(), -1.0f );
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    r = fromSRGB( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max() );
                    g = fromSRGB( static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max() );
                    b = fromSRGB( static_cast<float>( temp[2] ) / (float)std::numeric_limits<T>::max() );
                }
                else
                {
                    r = static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max();
                    g = static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max();
                    b = static_cast<float>( temp[2] ) / (float)std::numeric_limits<T>::max();
                }
            }
        }
        else
        {
            const T * temp = (const T *)srcPtr;
            r = static_cast<float>( temp[0] );
            g = static_cast<float>( temp[1] );
            b = static_cast<float>( temp[2] );
        }
        a = 1.0f;
    }
    
    //-----------------------------------------------------------------------------------
    template <typename T> void convertToFloat2(NIIf & r, NIIf & g, NIIf & b, NIIf & a, const void * srcPtr, uint32 flags)
    {
        if(flags & PFF_Float)
        {
            const NIIf * temp = (const NIIf *)srcPtr;
            r = temp[0];
            g = temp[1];
        }
        else if(flags & PFF_Float16)
        {
            const Nui16 * temp = (const Nui16 *)srcPtr;
            r = Bitwise::halfToFloat(temp[0]);
            g = Bitwise::halfToFloat(temp[1]);
        }
        else if(flags & PFF_Unit)
        {
            const T * temp = (const T *)srcPtr;
            if(flags & PFF_Signed)
            {
                // -128 & -127 and -32768 & -32767 both map to -1 according to D3D10 rules.
                r = std::max( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max(), -1.0f );
                g = std::max( static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max(), -1.0f );
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    r = fromSRGB( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max() );
                    g = fromSRGB( static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max() );
                }
                else
                {
                    r = static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max();
                    g = static_cast<float>( temp[1] ) / (float)std::numeric_limits<T>::max();
                }
            }
        }
        else
        {
            const T * temp = (const T *)srcPtr;
            r = static_cast<float>( temp[0] );
            g = static_cast<float>( temp[1] );
        }

        b = 0.0f;
        a = 1.0f;
    }
    
    //-----------------------------------------------------------------------------------
    template <typename T> void convertToFloat1(NIIf & r, NIIf & g, NIIf & b, NIIf & a, const void * srcPtr, uint32 flags)
    {
        if(flags & PFF_Float)
        {
            const NIIf * temp = (const NIIf *)srcPtr;
            r = temp[0];
        }
        else if(flags & PFF_Float16)
        {
            const Nui16 * temp = (const Nui16 *)srcPtr;
            r = Bitwise::halfToFloat(temp[0]);
        }
        else if(flags & PFF_Unit)
        {
            const T * temp = (const T *)srcPtr;
            if(flags & PFF_Signed)
            {
                // -128 & -127 and -32768 & -32767 both map to -1 according to D3D10 rules.
                r = std::max( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max(), -1.0f );
            }
            else
            {
                if(flags & PFF_SRGB)
                {
                    r = fromSRGB( static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max() );
                }
                else
                {
                    r = static_cast<float>( temp[0] ) / (float)std::numeric_limits<T>::max();
                }
            }
        }
        else
        {
            const T * temp = (const T *)srcPtr;
            r = static_cast<float>( temp[0] );
        }

        g = 0.0f;
        b = 0.0f;
        a = 1.0f;
    }
    //-----------------------------------------------------------------------------------
    void PixelUtil::packColour(NIIf r, NIIf g, NIIf b, NIIf a, PixelFormat pf, Nui8 * dst)
    {
        // Catch-it-all here
        /////////////////////////////////////////
        const PixelFormatDescription & des = getVerbose(pf);
        if(des.mFeature & PFF_NonEndian)
        {
            const Nui32 value = ((Bitwise::floatToFixed(r, des.rbits)<<des.rshift) & des.rmask) |
                ((Bitwise::floatToFixed(g, des.gbits)<<des.gshift) & des.gmask) |
                ((Bitwise::floatToFixed(b, des.bbits)<<des.bshift) & des.bmask) |
                ((Bitwise::floatToFixed(a, des.abits)<<des.ashift) & des.amask);
            // And write to memory
            Bitwise::intWrite(dst, des.mSize, value);
        }
        else
        {
        const uint32 flags = getExentMark(pf);
        switch( pf )
        {
        case PF_G32R32:
            ((NIIf*)dst)[0] = g;
            ((NIIf*)dst)[1] = r;
            break;
        case PF_G16R16:
            ((Nui16*)dst)[0] = Bitwise::floatToHalf(g);
            ((Nui16*)dst)[1] = Bitwise::floatToHalf(r);
            break;
        case PF_R16G16B16ui:
            ((Nui16*)dst)[0] = (Nui16)Bitwise::floatToFixed(r, 16);
            ((Nui16*)dst)[1] = (Nui16)Bitwise::floatToFixed(g, 16);
            ((Nui16*)dst)[2] = (Nui16)Bitwise::floatToFixed(b, 16);
            break;
        case PF_L8A8i:
            dst[0] = (Nui8)Bitwise::floatToFixed(r, 8);
            dst[1] = (Nui8)Bitwise::floatToFixed(a, 8);
            break;
        case PF_R32G32B32A32:
            ((NIIf*)dst)[0] = r;
            ((NIIf*)dst)[1] = g;
            ((NIIf*)dst)[2] = b;
            ((NIIf*)dst)[3] = a;
            //convertFromFloat4<float>(r, g, b, a, dst, flags);
            break;
        case PF_R32G32B32A32ui:
            convertFromFloat4<uint32>(r, g, b, a, dst, flags);
            break;
        case PF_R32G32B32A32i:
            convertFromFloat4<int32>(r, g, b, a, dst, flags);
            break;
        case PF_R32G32B32:
            ((NIIf*)dst)[0] = r;
            ((NIIf*)dst)[1] = g;
            ((NIIf*)dst)[2] = b;
            //convertFromFloat3<float>(r, g, b, a, dst, flags);
            break;
        case PF_R32G32B32ui:
            convertFromFloat3<uint32>(r, g, b, a, dst, flags);
            break;
        case PF_R32G32B32i:
            convertFromFloat3<int32>(r, g, b, a, dst, flags);
            break;
        case PF_R16G16B16A16:
            ((Nui16*)dst)[0] = Bitwise::floatToHalf(r);
            ((Nui16*)dst)[1] = Bitwise::floatToHalf(g);
            ((Nui16*)dst)[2] = Bitwise::floatToHalf(b);
            ((Nui16*)dst)[3] = Bitwise::floatToHalf(a);
            //convertFromFloat4<uint16>(r, g, b, a, dst, flags);
            break;
        case PF_R16G16B16A16UN:
            convertFromFloat4<uint16>(r, g, b, a, dst, flags);
            break;
        case PF_R16G16B16A16ui:
            ((Nui16*)dst)[0] = (Nui16)Bitwise::floatToFixed(r, 16);
            ((Nui16*)dst)[1] = (Nui16)Bitwise::floatToFixed(g, 16);
            ((Nui16*)dst)[2] = (Nui16)Bitwise::floatToFixed(b, 16);
            ((Nui16*)dst)[3] = (Nui16)Bitwise::floatToFixed(a, 16);
            //convertFromFloat4<uint16>(r, g, b, a, dst, flags);
            break;
        case PF_R16G16B16A16N:
            convertFromFloat4<int16>( r, g, b, a, dst, flags );
            break;
        case PF_R16G16B16A16i:
            convertFromFloat4<int16>( r, g, b, a, dst, flags );
            break;
        case PF_R32G32:
            convertFromFloat2<float>( r, g, b, a, dst, flags );
            break;
        case PF_R32G32ui:
            convertFromFloat2<uint32>( r, g, b, a, dst, flags );
            break;
        case PF_R32G32i:
            convertFromFloat2<int32>( r, g, b, a, dst, flags );
            break;
        case PF_Depth32f_Stencil8ui_64:
            ((float*)dst)[0] = r;
            ((uint32*)dst)[1] = static_cast<uint32>( g ) << 24u;
            break;
        case PF_R10G10B10A2UN:
        {
            const uint16 ir = static_cast<uint16>( std::clamp(r, 0.0f, 1.0f) * 1023.0f + 0.5f );
            const uint16 ig = static_cast<uint16>( std::clamp(g, 0.0f, 1.0f) * 1023.0f + 0.5f );
            const uint16 ib = static_cast<uint16>( std::clamp(b, 0.0f, 1.0f) * 1023.0f + 0.5f );
            const uint16 ia = static_cast<uint16>( std::clamp(a, 0.0f, 1.0f) * 3.0f + 0.5f );

            ((uint32*)dst)[0] = (ia << 30u) | (ib << 20u) | (ig << 10u) | (ir);
            break;
        }
        case PF_R10G10B10A2ui:
        {
            const uint16 ir = static_cast<uint16>( std::clamp( r, 0.0f, 1023.0f ) );
            const uint16 ig = static_cast<uint16>( std::clamp( g, 0.0f, 1023.0f ) );
            const uint16 ib = static_cast<uint16>( std::clamp( b, 0.0f, 1023.0f ) );
            const uint16 ia = static_cast<uint16>( std::clamp( a, 0.0f, 3.0f ) );

            ((uint32*)dst)[0] = (ia << 30u) | (ib << 20u) | (ig << 10u) | (ir);
            break;
        }
        case PF_R11G11B10:
            N_EXCEPT(InvalidParam, "PF_R11G11B10");
            break;
        case PF_R8G8B8A8UN:
            convertFromFloat4<Nui8>( r, g, b, a, dst, flags );
            break;
        case PF_R8G8B8A8UN_SRGB:
            convertFromFloat4<Nui8>( r, g, b, a, dst, flags );
            break;
        case PF_R8G8B8A8ui:
            convertFromFloat4<Nui8>( r, g, b, a, dst, flags );
            break;
        case PF_R8G8B8A8N:
            convertFromFloat4<int8>( r, g, b, a, dst, flags );
            break;
        case PF_R8G8B8A8i:
            convertFromFloat4<int8>( r, g, b, a, dst, flags );
            break;
        case PF_R16G16:
            convertFromFloat2<uint16>( r, g, b, a, dst, flags );
            break;
        case PF_R16G16UN:
            convertFromFloat2<uint16>( r, g, b, a, dst, flags );
            break;
        case PF_R16G16ui:
            convertFromFloat2<uint16>( r, g, b, a, dst, flags );
            break;
        case PF_R16G16N:
            convertFromFloat2<int16>( r, g, b, a, dst, flags );
            break;
        case PF_R16G16i:
            convertFromFloat2<int16>( r, g, b, a, dst, flags );
            break;
        case PF_Depth32:
            convertFromFloat1<float>( r, g, b, a, dst, flags );
            break;
        case PF_R32:
            ((NIIf*)dst)[0] = r;
        //    convertFromFloat1<float>(r, g, b, a, dst, flags);
            break;
        case PF_R32ui:
            convertFromFloat1<uint32>(r, g, b, a, dst, flags);
            break;
        case PF_R32i:
            convertFromFloat1<int32>(r, g, b, a, dst, flags);
            break;
        case PF_Depth24UN:
            ((uint32*)dst)[0] = static_cast<uint32>( roundf( r * 16777215.0f ) );
            break;
        case PF_Depth24UN_Stencil8ui:
            ((uint32*)dst)[0] = (static_cast<uint32>( g ) << 24u) | static_cast<uint32>( roundf( r * 16777215.0f ) );
            break;
        case PF_R8G8UN:
            convertFromFloat2<Nui8>(r, g, b, a, dst, flags);
            break;
        case PF_R8G8ui:
            convertFromFloat2<Nui8>(r, g, b, a, dst, flags);
            break;
        case PF_R8G8N:
            convertFromFloat2<int8>(r, g, b, a, dst, flags);
            break;
        case PF_R8G8i:
            convertFromFloat2<int8>(r, g, b, a, dst, flags);
            break;
        case PF_R16:
            ((Nui16*)dst)[0] = Bitwise::floatToHalf(r);
            //convertFromFloat1<uint16>( r, g, b, a, dst, 1u, flags );
            break;
        case PF_Depth16UN:
            convertFromFloat1<uint16>( r, g, b, a, dst, flags );
            break;
        case PF_R16UN:
            convertFromFloat1<uint16>( r, g, b, a, dst, flags );
            break;
        case PF_R16ui:
            convertFromFloat1<uint16>( r, g, b, a, dst, flags );
            break;
        case PF_R16N:
            convertFromFloat1<int16>( r, g, b, a, dst, flags );
            break;
        case PF_R16i:
            convertFromFloat1<int16>( r, g, b, a, dst, flags );
            break;
        case PF_R8UN:
            convertFromFloat1<Nui8>( r, g, b, a, dst, flags );
            break;
        case PF_R8ui:
            convertFromFloat1<Nui8>( r, g, b, a, dst, flags );
            break;
        case PF_R8N:
            convertFromFloat1<int8>( r, g, b, a, dst, flags );
            break;
        case PF_R8i:
            convertFromFloat1<int8>( r, g, b, a, dst, flags );
            break;
        case PF_A8UN:
            convertFromFloat1<Nui8>( r, g, b, a, dst, flags );
            break;
        case PF_R1UN:
        case PF_R9G9B9E5:
        case PF_UYVY_R8G8B8G8UN:
        case PF_YUY2_G8R8G8B8UN:
            N_EXCEPT(InvalidParam, "PF_R9G9B9E5");
            break;
        case PF_B5G6R5UN:
        {
            const Nui8 ir = static_cast<Nui8>( std::clamp( r, 0.0f, 1.0f) * 31.0f + 0.5f );
            const Nui8 ig = static_cast<Nui8>( std::clamp( g, 0.0f, 1.0f) * 63.0f + 0.5f );
            const Nui8 ib = static_cast<Nui8>( std::clamp( b, 0.0f, 1.0f) * 31.0f + 0.5f );

            ((uint16*)dst)[0] = (ir << 11u) | (ig << 5u) | (ib);
            break;
        }
        case PF_B5G5R5A1UN:
        {
            const Nui8 ir = static_cast<Nui8>( std::clamp( r, 0.0f, 1.0f  ) * 31.0f + 0.5f );
            const Nui8 ig = static_cast<Nui8>( std::clamp( g, 0.0f, 1.0f  ) * 31.0f + 0.5f );
            const Nui8 ib = static_cast<Nui8>( std::clamp( b, 0.0f, 1.0f  ) * 31.0f + 0.5f );
            const Nui8 ia = a == 0.0f ? 0u : 1u;

            ((uint16*)dst)[0] = (ia << 15u) | (ir << 10u) | (ig << 5u) | (ib);
            break;
        }
        case PF_B8G8R8A8UN:
            ((Nui8*)dst)[0] = static_cast<Nui8>( std::clamp( b, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[1] = static_cast<Nui8>( std::clamp( g, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[2] = static_cast<Nui8>( std::clamp( r, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[3] = static_cast<Nui8>( std::clamp( a, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            break;
        case PF_B8G8R8X8UN:
            ((Nui8*)dst)[0] = static_cast<Nui8>( std::clamp( b, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[1] = static_cast<Nui8>( std::clamp( g, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[2] = static_cast<Nui8>( std::clamp( r, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[3] = 255u;
            break;
        case PF_R10G10B10A2UN_XR_BIAS:
            N_EXCEPT(InvalidParam, "PF_R10G10B10A2UN_XR_BIAS");
            break;
        case PF_B8G8R8A8UN_SRGB:
            ((Nui8*)dst)[0] = static_cast<Nui8>( std::clamp( toSRGB( b ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[1] = static_cast<Nui8>( std::clamp( toSRGB( g ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[2] = static_cast<Nui8>( std::clamp( toSRGB( r ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[3] = static_cast<Nui8>( std::clamp( a, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            break;
        case PF_B8G8R8X8UN_SRGB:
            ((Nui8*)dst)[0] = static_cast<Nui8>( std::clamp( toSRGB( b ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[1] = static_cast<Nui8>( std::clamp( toSRGB( g ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[2] = static_cast<Nui8>( std::clamp( toSRGB( r ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[3] = 255u;
            break;
        case PF_B4G4R4A4UN:
        {
            const Nui8 ir = static_cast<Nui8>( std::clamp( r, 0.0f, 1.0f  ) * 15.0f + 0.5f );
            const Nui8 ig = static_cast<Nui8>( std::clamp( g, 0.0f, 1.0f  ) * 15.0f + 0.5f );
            const Nui8 ib = static_cast<Nui8>( std::clamp( b, 0.0f, 1.0f  ) * 15.0f + 0.5f );
            const Nui8 ia = static_cast<Nui8>( std::clamp( a, 0.0f, 1.0f  ) * 15.0f + 0.5f );

            ((uint16*)dst)[0] = (ia << 12u) | (ir << 8u) | (ig << 4u) | (ib);
            break;
        }

        case PF_R8G8B8UN:
            convertFromFloat3<Nui8>(r, g, b, a, dst, flags);
            break;
        case PF_R8G8B8UN_SRGB:
            convertFromFloat3<Nui8>(r, g, b, a, dst, flags);
            break;
        case PF_B8G8R8UN:
            ((Nui8*)dst)[0] = static_cast<Nui8>( std::clamp( b, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[1] = static_cast<Nui8>( std::clamp( g, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[2] = static_cast<Nui8>( std::clamp( r, 0.0f, 1.0f  ) * 255.0f + 0.5f );
            break;
        case PF_B8G8R8UN_SRGB:
            ((Nui8*)dst)[0] = static_cast<Nui8>( std::clamp( toSRGB( b ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[1] = static_cast<Nui8>( std::clamp( toSRGB( g ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            ((Nui8*)dst)[2] = static_cast<Nui8>( std::clamp( toSRGB( r ), 0.0f, 1.0f  ) * 255.0f + 0.5f );
            break;

        case PF_R16G16B16UN:
            ((Nui16*)dst)[0] = Bitwise::floatToHalf(r);
            ((Nui16*)dst)[1] = Bitwise::floatToHalf(g);
            ((Nui16*)dst)[2] = Bitwise::floatToHalf(b);
            //convertFromFloat3<uint16>(r, g, b, a, dst, flags);
            break;

        case PF_AYUV: case PF_Y410: case PF_Y416:
        case PF_NV12: case PF_P010: case PF_P016:
        case PF_420_OPAQUE:
        case PF_YUY2: case PF_Y210: case PF_Y216:
        case PF_NV11: case PF_AI44: case PF_IA44:
        case PF_P8:   case PF_A8P8:
        case PF_P208: case PF_V208: case PF_V408:
        case PF_UNKNOWN: /*case PFG_NULL:*/ case PF_COUNT:
            N_EXCEPT(InvalidParam, "");
            break;

        case PF_BC1UN: case PF_BC1UN_SRGB:
        case PF_BC2UN: case PF_BC2UN_SRGB:
        case PF_BC3UN: case PF_BC3UN_SRGB:
        case PF_BC4UN: case PF_BC4N:
        case PF_BC5UN: case PF_BC5N:
        case PF_BC6H_u16: case PF_BC6H_16:
        case PF_BC7_UN: case PF_BC7_UN_SRGB:
        case PF_PVRTC_RGB_2BPP:    case PF_PVRTC_RGB_2BPP_SRGB:
        case PF_PVRTC_RGBA_2BPP:   case PF_PVRTC_RGBA_2BPP_SRGB:
        case PF_PVRTC_RGB_4BPP:    case PF_PVRTC_RGB_4BPP_SRGB:
        case PF_PVRTC_RGBA_4BPP:   case PF_PVRTC_RGBA_4BPP_SRGB:
        case PF_PVRTC2_2BPP:   case PF_PVRTC2_2BPP_SRGB:
        case PF_PVRTC2_4BPP:   case PF_PVRTC2_4BPP_SRGB:
        case PF_ETC1_R8G8B8UN:
        case PF_ETC2_R8G8B8UN: case PF_ETC2_R8G8B8UN_SRGB:
        case PF_ETC2_R8G8B8A8UN: case PF_ETC2_R8G8B8A8UN_SRGB:
        case PF_ETC2_R8G8B8A1UN: case PF_ETC2_R8G8B8A1UN_SRGB:
        case PF_EAC_R11UN: case PF_EAC_R11N:
        case PF_EAC_R11G11UN: case PF_EAC_R11G11N:
        case PF_ATC_RGB:
        case PF_ATC_RGBA_EXPLICIT_A:
        case PF_ATC_RGBA_INTERPOLATED_A:
        case PF_ASTC_RGBAUN_4X4_LDR:   case PF_ASTC_RGBAUN_4X4_SRGB:
        case PF_ASTC_RGBAUN_5X4_LDR:   case PF_ASTC_RGBAUN_5X4_SRGB:
        case PF_ASTC_RGBAUN_5X5_LDR:   case PF_ASTC_RGBAUN_5X5_SRGB:
        case PF_ASTC_RGBAUN_6X5_LDR:   case PF_ASTC_RGBAUN_6X5_SRGB:
        case PF_ASTC_RGBAUN_6X6_LDR:   case PF_ASTC_RGBAUN_6X6_SRGB:
        case PF_ASTC_RGBAUN_8X5_LDR:   case PF_ASTC_RGBAUN_8X5_SRGB:
        case PF_ASTC_RGBAUN_8X6_LDR:   case PF_ASTC_RGBAUN_8X6_SRGB:
        case PF_ASTC_RGBAUN_8X8_LDR:   case PF_ASTC_RGBAUN_8X8_SRGB:
        case PF_ASTC_RGBAUN_10X5_LDR:  case PF_ASTC_RGBAUN_10X5_SRGB:
        case PF_ASTC_RGBAUN_10X6_LDR:  case PF_ASTC_RGBAUN_10X6_SRGB:
        case PF_ASTC_RGBAUN_10X8_LDR:  case PF_ASTC_RGBAUN_10X8_SRGB:
        case PF_ASTC_RGBAUN_10X10_LDR: case PF_ASTC_RGBAUN_10X10_SRGB:
        case PF_ASTC_RGBAUN_12X10_LDR: case PF_ASTC_RGBAUN_12X10_SRGB:
        case PF_ASTC_RGBAUN_12X12_LDR: case PF_ASTC_RGBAUN_12X12_SRGB:
            N_EXCEPT(InvalidParam, "Compressed formats not supported!");
            break;
        }
    }
    //-----------------------------------------------------------------------------------
    void PixelUtil::unpackColour(NIIf & r, NIIf & g, NIIf & b, NIIf & a, PixelFormat pf, const Nui8 * src)
    {
        const PixelFormatDescription & des = getVerbose(pf);
        if(des.mFeature & PFF_NonEndian)
        {
            // Shortcut for integer formats unpacking
            const Nui32 value = Bitwise::intRead(src, des.mSize);
            if(des.mFeature & PFF_Grayscale)
            {
                // Luminance mFormat -- only rbits used
                r = g = b = Bitwise::fixedToFloat((value & des.rmask)>>des.rshift, des.rbits);
            }
            else
            {
                r = Bitwise::fixedToFloat((value & des.rmask)>>des.rshift, des.rbits);
                g = Bitwise::fixedToFloat((value & des.gmask)>>des.gshift, des.gbits);
                b = Bitwise::fixedToFloat((value & des.bmask)>>des.bshift, des.bbits);
            }
            if(des.mFeature & PFF_Alpha)
            {
                a = Bitwise::fixedToFloat((value & des.amask)>>des.ashift, des.abits);
            }
            else
            {
                a = 1.0f; // No alpha, default a component to full
            }
        }
        else
        {
        const uint32 flags = getExentMark( pf );
        switch( pf )
        {
        case PF_G32R32:
            g = ((NIIf*)src)[0];
            r = b = ((NIIf*)src)[1];
            a = 1.0f;
            break;
        case PF_G16R16:
            g = Bitwise::halfToFloat(((Nui16*)src)[0]);
            r = b = Bitwise::halfToFloat(((Nui16*)src)[1]);
            a = 1.0f;
            break;
        case PF_R16G16B16ui:
            r = Bitwise::fixedToFloat(((Nui16*)src)[0], 16);
            g = Bitwise::fixedToFloat(((Nui16*)src)[1], 16);
            b = Bitwise::fixedToFloat(((Nui16*)src)[2], 16);
            a = 1.0f;
            break;
        case PF_L8A8i:
            r = g = b = Bitwise::fixedToFloat(src[0], 8);
            a = Bitwise::fixedToFloat(src[1], 8);
            break;
        case PF_R32G32B32A32:
            r = ((NIIf*)src)[0];
            g = ((NIIf*)src)[1];
            b = ((NIIf*)src)[2];
            a = ((NIIf*)src)[3];
            //convertToFloat4<float>( r, g, b, a, src, flags );
            break;
        case PF_R32G32B32A32ui:
            convertToFloat4<uint32>( r, g, b, a, src, flags );
            break;
        case PF_R32G32B32A32i:
            convertToFloat4<int32>( r, g, b, a, src, flags );
            break;
        case PF_R32G32B32:
            r = ((NIIf*)src)[0];
            g = ((NIIf*)src)[1];
            b = ((NIIf*)src)[2];
            a = 1.0f;
            //convertToFloat3<float>( r, g, b, a, src, flags );
            break;
        case PF_R32G32B32ui:
            convertToFloat3<uint32>( r, g, b, a, src, flags );
            break;
        case PF_R32G32B32i:
            convertToFloat3<int32>( r, g, b, a, src, flags );
            break;
        case PF_R16G16B16A16:
            r = Bitwise::halfToFloat(((Nui16*)src)[0]);
            g = Bitwise::halfToFloat(((Nui16*)src)[1]);
            b = Bitwise::halfToFloat(((Nui16*)src)[2]);
            a = Bitwise::halfToFloat(((Nui16*)src)[3]);
            //convertToFloat4<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16B16A16UN:
            convertToFloat4<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16B16A16ui:
            r = Bitwise::fixedToFloat(((Nui16*)src)[0], 16);
            g = Bitwise::fixedToFloat(((Nui16*)src)[1], 16);
            b = Bitwise::fixedToFloat(((Nui16*)src)[2], 16);
            a = Bitwise::fixedToFloat(((Nui16*)src)[3], 16);
            //convertToFloat4<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16B16A16N:
            convertToFloat4<int16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16B16A16i:
            convertToFloat4<int16>( r, g, b, a, src, flags );
            break;
        case PF_R32G32:
            convertToFloat2<float>( r, g, b, a, src, flags );
            break;
        case PF_R32G32ui:
            convertToFloat2<uint32>( r, g, b, a, src, flags );
            break;
        case PF_R32G32i:
            convertToFloat2<int32>( r, g, b, a, src, flags );
            break;
        case PF_Depth32f_Stencil8ui_64:
            r = ((const float*)src)[0];
            g = static_cast<float>( ((const uint32*)src)[1] >> 24u );
            b = 0.0f;
            a = 1.0f;
            break;
        case PF_R10G10B10A2UN:
        {
            const uint32 val = ((const uint32*)src)[0];
            r = static_cast<float>( val & 0x3FF ) / 1023.0f;
            g = static_cast<float>( (val >> 10u) & 0x3FF ) / 1023.0f;
            b = static_cast<float>( (val >> 20u) & 0x3FF ) / 1023.0f;
            a = static_cast<float>( val >> 30u ) / 3.0f;
            break;
        }
        case PF_R10G10B10A2ui:
        {
            const uint32 val = ((const uint32*)src)[0];
            r = static_cast<float>( val & 0x3FF );
            g = static_cast<float>( (val >> 10u) & 0x3FF );
            b = static_cast<float>( (val >> 20u) & 0x3FF );
            a = static_cast<float>( val >> 30u );
            break;
        }
        case PF_R11G11B10:
            N_EXCEPT(InvalidParam, "PF_R11G11B10");
            break;
        case PF_R8G8B8A8UN:
            convertToFloat4<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_R8G8B8A8UN_SRGB:
            convertToFloat4<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_R8G8B8A8ui:
            convertToFloat4<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_R8G8B8A8N:
            convertToFloat4<int8>( r, g, b, a, src, flags );
            break;
        case PF_R8G8B8A8i:
            convertToFloat4<int8>( r, g, b, a, src, flags );
            break;
        case PF_R16G16:
            convertToFloat2<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16UN:
            convertToFloat2<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16ui:
            convertToFloat2<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16N:
            convertToFloat2<int16>( r, g, b, a, src, flags );
            break;
        case PF_R16G16i:
            convertToFloat2<int16>( r, g, b, a, src, flags );
            break;
        case PF_Depth32:
            convertToFloat1<float>( r, g, b, a, src, flags );
            break;
        case PF_R32:
            r = g = b = ((NIIf*)src)[0];
            a = 1.0f;
            //convertToFloat1<float>( r, g, b, a, src, flags );
            break;
        case PF_R32ui:
            convertToFloat1<uint32>( r, g, b, a, src, flags );
            break;
        case PF_R32i:
            convertToFloat1<int32>( r, g, b, a, src, flags );
            break;
        case PF_Depth24UN:
            r = static_cast<float>( ((const uint32*)src)[0] ) / 16777215.0f;
            g = 0.0f;
            b = 0.0f;
            a = 1.0f;
            break;
        case PF_Depth24UN_Stencil8ui:
            r = static_cast<float>( ((const uint32*)src)[0] & 0x00FFFFFF ) / 16777215.0f;
            g = static_cast<float>( ((const uint32*)src)[0] >> 24u );
            b = 0.0f;
            a = 1.0f;
            break;
        case PF_R8G8UN:
            convertToFloat2<Nui8>( r, g, b, a, src,  flags );
            break;
        case PF_R8G8ui:
            convertToFloat2<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_R8G8N:
            convertToFloat2<int8>( r, g, b, a, src, flags );
            break;
        case PF_R8G8i:
            convertToFloat2<int8>( r, g, b, a, src, flags );
            break;
        case PF_R16:
            r = g = b = Bitwise::halfToFloat(((Nui16*)src)[0]);
            a = 1.0f;
            //convertToFloat1<uint16>( r, g, b, a, src, flags );
            break;
        case PF_Depth16UN:
            convertToFloat1<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16UN:
            convertToFloat1<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16ui:
            convertToFloat1<uint16>( r, g, b, a, src, flags );
            break;
        case PF_R16N:
            convertToFloat1<int16>( r, g, b, a, src, flags );
            break;
        case PF_R16i:
            convertToFloat1<int16>( r, g, b, a, src, flags );
            break;
        case PF_R8UN:
            convertToFloat1<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_R8ui:
            convertToFloat1<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_R8N:
            convertToFloat1<int8>( r, g, b, a, src, flags );
            break;
        case PF_R8i:
            convertToFloat1<int8>( r, g, b, a, src, flags );
            break;
        case PF_A8UN:
            r = 0;
            g = 0;
            b = 0;
            a = static_cast<float>( ((const uint32*)src)[0] );
            break;
        case PF_R1UN:
        case PF_R9G9B9E5:
        case PF_UYVY_R8G8B8G8UN:
        case PF_YUY2_G8R8G8B8UN:
            N_EXCEPT(InvalidParam, "PF_R9G9B9E5");
            break;
        case PF_B5G6R5UN:
        {
            const uint16 val = ((const uint16*)src)[0];
            r = static_cast<float>( (val >> 11u) & 0x1F ) / 31.0f;
            g = static_cast<float>( (val >>  5u) & 0x3F ) / 63.0f;
            b = static_cast<float>( val & 0x1F ) / 31.0f;
            a = 1.0f;
            break;
        }
        case PF_B5G5R5A1UN:
        {
            const uint16 val = ((const uint16*)src)[0];
            r = static_cast<float>( (val >> 10u) & 0x1F ) / 31.0f;
            g = static_cast<float>( (val >>  5u) & 0x1F ) / 31.0f;
            b = static_cast<float>( val & 0x1F ) / 31.0f;
            a = (val >> 15u) == 0 ? 0.0f : 1.0f;
            break;
        }
        case PF_B8G8R8A8UN:
            convertToFloat4<Nui8>( r, g, b, a, src, flags );
            std::swap( r, b );
            break;
        case PF_B8G8R8X8UN:
            convertToFloat4<Nui8>( r, g, b, a, src, flags );
            std::swap( r, b );
            break;
        case PF_R10G10B10A2UN_XR_BIAS:
            N_EXCEPT(InvalidParam, "PF_R10G10B10A2UN_XR_BIAS");
            break;
        case PF_B8G8R8A8UN_SRGB:
            convertToFloat4<Nui8>( r, g, b, a, src, flags );
            std::swap( r, b );
            break;
        case PF_B8G8R8X8UN_SRGB:
            convertToFloat3<Nui8>( r, g, b, a, src, flags );
            std::swap( r, b );
            break;
        case PF_B4G4R4A4UN:
        {
            const uint16 val = ((const uint16*)src)[0];
            r = static_cast<float>( (val >>  8u) & 0xF ) / 15.0f;
            g = static_cast<float>( (val >>  4u) & 0xF ) / 15.0f;
            b = static_cast<float>( val & 0xF ) / 15.0f;
            a = static_cast<float>( (val >> 12u) & 0xF ) / 15.0f;
            break;
        }

        case PF_R8G8B8UN:
            convertToFloat3<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_R8G8B8UN_SRGB:
            convertToFloat3<Nui8>( r, g, b, a, src, flags );
            break;
        case PF_B8G8R8UN:
            convertToFloat3<Nui8>( r, g, b, a, src, flags );
            std::swap( r, b );
            break;
        case PF_B8G8R8UN_SRGB:
            convertToFloat3<Nui8>( r, g, b, a, src, flags );
            std::swap( r, b );
            break;

        case PF_R16G16B16UN:
            r = Bitwise::halfToFloat(((Nui16*)src)[0]);
            g = Bitwise::halfToFloat(((Nui16*)src)[1]);
            b = Bitwise::halfToFloat(((Nui16*)src)[2]);
            a = 1.0f;
            //convertToFloat<uint16>( r, g, b, a, src, 3u, flags );
            break;

        case PF_AYUV: case PF_Y410: case PF_Y416:
        case PF_NV12: case PF_P010: case PF_P016:
        case PF_420_OPAQUE:
        case PF_YUY2: case PF_Y210: case PF_Y216:
        case PF_NV11: case PF_AI44: case PF_IA44:
        case PF_P8:   case PF_A8P8:
        case PF_P208: case PF_V208: case PF_V408:
        case PF_UNKNOWN: /*case PFG_NULL:*/ case PF_COUNT:
            N_EXCEPT(InvalidParam, "");
            break;

        case PF_BC1UN: case PF_BC1UN_SRGB:
        case PF_BC2UN: case PF_BC2UN_SRGB:
        case PF_BC3UN: case PF_BC3UN_SRGB:
        case PF_BC4UN: case PF_BC4N:
        case PF_BC5UN: case PF_BC5N:
        case PF_BC6H_u16: case PF_BC6H_16:
        case PF_BC7_UN: case PF_BC7_UN_SRGB:
        case PF_PVRTC_RGB_2BPP:    case PF_PVRTC_RGB_2BPP_SRGB:
        case PF_PVRTC_RGBA_2BPP:   case PF_PVRTC_RGBA_2BPP_SRGB:
        case PF_PVRTC_RGB_4BPP:    case PF_PVRTC_RGB_4BPP_SRGB:
        case PF_PVRTC_RGBA_4BPP:   case PF_PVRTC_RGBA_4BPP_SRGB:
        case PF_PVRTC2_2BPP:   case PF_PVRTC2_2BPP_SRGB:
        case PF_PVRTC2_4BPP:   case PF_PVRTC2_4BPP_SRGB:
        case PF_ETC1_R8G8B8UN:
        case PF_ETC2_R8G8B8UN: case PF_ETC2_R8G8B8UN_SRGB:
        case PF_ETC2_R8G8B8A8UN: case PF_ETC2_R8G8B8A8UN_SRGB:
        case PF_ETC2_R8G8B8A1UN: case PF_ETC2_R8G8B8A1UN_SRGB:
        case PF_EAC_R11UN: case PF_EAC_R11N:
        case PF_EAC_R11G11UN: case PF_EAC_R11G11N:
        case PF_ATC_RGB:
        case PF_ATC_RGBA_EXPLICIT_A:
        case PF_ATC_RGBA_INTERPOLATED_A:
        case PF_ASTC_RGBAUN_4X4_LDR:   case PF_ASTC_RGBAUN_4X4_SRGB:
        case PF_ASTC_RGBAUN_5X4_LDR:   case PF_ASTC_RGBAUN_5X4_SRGB:
        case PF_ASTC_RGBAUN_5X5_LDR:   case PF_ASTC_RGBAUN_5X5_SRGB:
        case PF_ASTC_RGBAUN_6X5_LDR:   case PF_ASTC_RGBAUN_6X5_SRGB:
        case PF_ASTC_RGBAUN_6X6_LDR:   case PF_ASTC_RGBAUN_6X6_SRGB:
        case PF_ASTC_RGBAUN_8X5_LDR:   case PF_ASTC_RGBAUN_8X5_SRGB:
        case PF_ASTC_RGBAUN_8X6_LDR:   case PF_ASTC_RGBAUN_8X6_SRGB:
        case PF_ASTC_RGBAUN_8X8_LDR:   case PF_ASTC_RGBAUN_8X8_SRGB:
        case PF_ASTC_RGBAUN_10X5_LDR:  case PF_ASTC_RGBAUN_10X5_SRGB:
        case PF_ASTC_RGBAUN_10X6_LDR:  case PF_ASTC_RGBAUN_10X6_SRGB:
        case PF_ASTC_RGBAUN_10X8_LDR:  case PF_ASTC_RGBAUN_10X8_SRGB:
        case PF_ASTC_RGBAUN_10X10_LDR: case PF_ASTC_RGBAUN_10X10_SRGB:
        case PF_ASTC_RGBAUN_12X10_LDR: case PF_ASTC_RGBAUN_12X10_SRGB:
        case PF_ASTC_RGBAUN_12X12_LDR: case PF_ASTC_RGBAUN_12X12_SRGB:
            N_EXCEPT(InvalidParam, "Compressed formats not supported!");
            break;
        }
        }
    }
    //-----------------------------------------------------------------------
    void PixelUtil::packColour(const Colour & c, PixelFormat pf, Nui8 * dest)
    {
        packColour(c.r, c.g, c.b, c.a, pf, dest);
    }
    //-----------------------------------------------------------------------
    void PixelUtil::packColour(Nui8 r, Nui8 g, Nui8 b, Nui8 a, PixelFormat pf, Nui8 * dest)
    {
        const PixelFormatDescription &des = getVerbose(pf);
        if(des.mFeature & PFF_NonEndian)
        {
            // Shortcut for integer formats packing
            Nui32 value = ((Bitwise::fixedToFixed(r, 8, des.rbits)<<des.rshift) & des.rmask) |
                ((Bitwise::fixedToFixed(g, 8, des.gbits)<<des.gshift) & des.gmask) |
                ((Bitwise::fixedToFixed(b, 8, des.bbits)<<des.bshift) & des.bmask) |
                ((Bitwise::fixedToFixed(a, 8, des.abits)<<des.ashift) & des.amask);
            // And write to memory
            Bitwise::intWrite(dest, des.mSize, value);
        }
        else
        {
            // Convert to NIIf
            packColour((NIIf)r/255.0f,(NIIf)g/255.0f,(NIIf)b/255.0f,(NIIf)a/255.0f, pf, dest);
        }
    }
    //-----------------------------------------------------------------------
    void PixelUtil::unpackColour(Colour * c, PixelFormat pf, const Nui8 * src)
    {
        unpackColour(c->r, c->g, c->b, c->a, pf, src);
    }
    //-----------------------------------------------------------------------
    void PixelUtil::unpackColour(Nui8 & r, Nui8 & g, Nui8 & b, Nui8 & a, PixelFormat pf, const Nui8 * src)
    {
        const PixelFormatDescription & des = getVerbose(pf);
        if(des.mFeature & PFF_NonEndian)
        {
            // Shortcut for integer formats unpacking
            const Nui32 value = Bitwise::intRead(src, des.mSize);
            if(des.mFeature & PFF_Grayscale)
            {
                // Luminance mFormat -- only rbits used
                r = g = b = (Nui8)Bitwise::fixedToFixed(
                    (value & des.rmask)>>des.rshift, des.rbits, 8);
            }
            else
            {
                r = (Nui8)Bitwise::fixedToFixed((value & des.rmask)>>des.rshift, des.rbits, 8);
                g = (Nui8)Bitwise::fixedToFixed((value & des.gmask)>>des.gshift, des.gbits, 8);
                b = (Nui8)Bitwise::fixedToFixed((value & des.bmask)>>des.bshift, des.bbits, 8);
            }
            // 处理透明通道
            if(des.mFeature & PFF_Alpha)
            {
                a = (Nui8)Bitwise::fixedToFixed((value & des.amask)>>des.ashift, des.abits, 8);
            }
            else
            {
                a = 255; // No alpha, default a component to full
            }
        }
        else
        {
            // Do the operation with the more generic floating point
            NIIf rr = 0, gg = 0, bb = 0, aa = 0;
            unpackColour(rr, gg, bb, aa, pf, src);
            r = (Nui8)Bitwise::floatToFixed(rr, 8);
            g = (Nui8)Bitwise::floatToFixed(gg, 8);
            b = (Nui8)Bitwise::floatToFixed(bb, 8);
            a = (Nui8)Bitwise::floatToFixed(aa, 8);
        }
    }
    //-----------------------------------------------------------------------------------
    namespace
    {
        typedef void ( *row_conversion_func_t )( Nui8 *src, Nui8 *dst, size_t width );

        void convCopy16Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 16 * width ); }
        void convCopy12Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 12 * width ); }
        void convCopy8Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 8 * width ); }
        void convCopy6Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 6 * width ); }
        void convCopy4Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 4 * width ); }
        void convCopy3Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 3 * width ); }
        void convCopy2Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 2 * width ); }
        void convCopy1Bpx( Nui8 *src, Nui8 *dst, size_t width ) { memcpy( dst, src, 1 * width ); }

        // clang-pf off
        void convRGBA32toRGB32(Nui8* _src, Nui8* _dst, size_t width) {
            uint32* src = (uint32*)_src; uint32* dst = (uint32*)_dst;
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; src += 4; dst += 3; }
        }
        void convRGB32toRG32(Nui8* _src, Nui8* _dst, size_t width) {
            uint32* src = (uint32*)_src; uint32* dst = (uint32*)_dst;
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 3; dst += 2; }
        }
        void convRG32toRGB32(Nui8* _src, Nui8* _dst, size_t width) {
            uint32* src = (uint32*)_src; uint32* dst = (uint32*)_dst;
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = 0u; src += 2; dst += 3; }
        }
        void convRG32toR32(Nui8* _src, Nui8* _dst, size_t width) {
            uint32* src = (uint32*)_src; uint32* dst = (uint32*)_dst;
            while (width--) { dst[0] = src[0]; src += 2; dst += 1; }
        }

        void convRGBA16toRGB16(Nui8* _src, Nui8* _dst, size_t width) {
            uint16* src = (uint16*)_src; uint16* dst = (uint16*)_dst;
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; src += 4; dst += 3; }
        }
        void convRGB16toRGBA16(Nui8* _src, Nui8* _dst, size_t width) {
            uint16* src = (uint16*)_src; uint16* dst = (uint16*)_dst;
            while (width--)
            { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 0xFFFF; src += 3; dst += 4; }
        }
        void convRGB16toRG16(Nui8* _src, Nui8* _dst, size_t width) {
            uint16* src = (uint16*)_src; uint16* dst = (uint16*)_dst;
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 3; dst += 2; }
        }
        void convRG16toRGB16(Nui8* _src, Nui8* _dst, size_t width) {
            uint16* src = (uint16*)_src; uint16* dst = (uint16*)_dst;
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[0] = 0u; src += 2; dst += 3; }
        }
        void convRG16toR16(Nui8* _src, Nui8* _dst, size_t width) {
            uint16* src = (uint16*)_src; uint16* dst = (uint16*)_dst;
            while (width--) { dst[0] = src[0]; src += 2; dst += 1; }
        }

        void convRGBAtoBGRA(Nui8* src, Nui8* dst, size_t width) {
            while (width--)
            { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; dst[3] = src[3]; src += 4; dst += 4; }
        }
        void convRGBAtoRGB(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; src += 4; dst += 3; }
        }
        void convRGBAtoBGR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; src += 4; dst += 3; }
        }
        void convRGBAtoRG(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 4; dst += 2; }
        }
        void convRGBAtoR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[0]; src += 4; dst += 1; }
        }

        void convBGRAtoRG(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[2]; dst[1] = src[1]; src += 4; dst += 2; }
        }
        void convBGRAtoR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[2]; src += 4; dst += 1; }
        }

        void convBGRXtoRGBA(Nui8* src, Nui8* dst, size_t width) {
            while (width--)
            { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; dst[3] = 0xFF; src += 4; dst += 4; }
        }
        void convBGRXtoBGRA(Nui8* src, Nui8* dst, size_t width) {
            while (width--)
            { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 0xFF; src += 4; dst += 4; }
        }

        void convRGBtoRGBA(Nui8* src, Nui8* dst, size_t width) {
            while (width--)
            { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 0xFF; src += 3; dst += 4; }
        }
        void convRGBtoBGRA(Nui8* src, Nui8* dst, size_t width) {
            while (width--)
            { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; dst[3] = 0xFF; src += 3; dst += 4; }
        }
        void convRGBtoBGR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; src += 3; dst += 3; }
        }
        void convRGBtoRG(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 3; dst += 2; }
        }
        void convRGBtoR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[0]; src += 3; dst += 1; }
        }

        void convBGRtoRG(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[2]; dst[1] = src[1]; src += 3; dst += 2; }
        }
        void convBGRtoR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[2]; src += 3; dst += 1; }
        }

        void convRGtoRGB(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = 0u; src += 2; dst += 3; }
        }
        void convRGtoBGR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = 0u; dst[1] = src[1]; dst[2] = src[0]; src += 2; dst += 3; }
        }
        void convRGtoR(Nui8* src, Nui8* dst, size_t width) {
            while (width--) { dst[0] = src[0]; src += 2; dst += 1; }
        }
        // clang-pf on
    }  // namespace
    //-----------------------------------------------------------------------
    void PixelUtil::conv(Nui8 * srcp, PixelFormat srcpf, Nui8 * destp, PixelFormat dstpf, NCount count)
    {
        PixelBlock src(count, 1, 1, srcpf, srcp);
        PixelBlock dst(count, 1, 1, dstpf, destp);

        conv(src, dst);
    }
    //-----------------------------------------------------------------------
    void PixelUtil::conv(const PixelBlock & src, PixelBlock & dst, bool verticalFlip)
    {
        if( src.mFormat == dst.mFormat && !verticalFlip )
        {
            dst.write( src );
            return;
        }

        if(isCompress(src.mFormat) || isCompress(dst.mFormat))
        {
            N_EXCEPT(InvalidParam, "This method can not be used to compress or decompress images");
        }

        assert( src.equalVolume( dst ) );
        assert( getUnitSize(src.mFormat) == src.mUnitSize );
        assert( getUnitSize(dst.mFormat) == dst.mUnitSize );

        const size_t srcBytesPerPixel = src.mUnitSize;
        const size_t dstBytesPerPixel = dst.mUnitSize;

        Nui8 * srcData = src.getOriginData(src.mLeft, src.mTop, src.getVolumeIdex());
        Nui8 * dstData = dst.getOriginData(dst.mLeft, dst.mTop, dst.getVolumeIdex());

        const NCount width = src.mWidth;
        const NCount height = src.mHeight;
        const NCount volume = src.getVolume();

        // Is there a optimized row conversion?
        row_conversion_func_t rowConversionFunc = 0;
        if( src.mFormat == dst.mFormat )
        {
            switch( srcBytesPerPixel )
            {
                // clang-pf off
            case 1: rowConversionFunc = convCopy1Bpx; break;
            case 2: rowConversionFunc = convCopy2Bpx; break;
            case 3: rowConversionFunc = convCopy3Bpx; break;
            case 4: rowConversionFunc = convCopy4Bpx; break;
            case 6: rowConversionFunc = convCopy6Bpx; break;
            case 8: rowConversionFunc = convCopy8Bpx; break;
            case 12: rowConversionFunc = convCopy12Bpx; break;
            case 16: rowConversionFunc = convCopy16Bpx; break;
                // clang-pf on
            }
        }
        else if( getExentMark( src.mFormat ) == getExentMark( dst.mFormat ) )  // semantic match, copy as typeless
        {
            assert(PLT_Count <= 16); // adjust PFL_PAIR definition if assertion failed
#define PFL_PAIR( a, b ) ( ( a << 4 ) | b )
            PixelLayoutType srcLayout = getDataLayout( src.mFormat );
            PixelLayoutType dstLayout = getDataLayout( dst.mFormat );
            switch( PFL_PAIR( srcLayout, dstLayout ) )
            {
                // clang-pf off
            case PFL_PAIR( PLT_R32G32B32A32, PLT_R32G32B32 ): rowConversionFunc = convRGBA32toRGB32; break;
            case PFL_PAIR( PLT_R32G32B32, PLT_R32G32 ): rowConversionFunc = convRGB32toRG32; break;
            case PFL_PAIR( PLT_R32G32, PLT_R32G32B32 ): rowConversionFunc = convRG32toRGB32; break;
            case PFL_PAIR( PLT_R32G32, PLT_R32 ): rowConversionFunc = convRG32toR32; break;

            case PFL_PAIR( PLT_R16G16B16A16, PLT_R16G16B16 ): rowConversionFunc = convRGBA16toRGB16; break;
            case PFL_PAIR( PLT_R16G16B16, PLT_R16G16B16A16 ): rowConversionFunc = convRGB16toRGBA16; break;
            case PFL_PAIR( PLT_R16G16B16, PLT_R16G16 ): rowConversionFunc = convRGB16toRG16; break;
            case PFL_PAIR( PLT_R16G16, PLT_R16G16B16 ): rowConversionFunc = convRG16toRGB16; break;
            case PFL_PAIR( PLT_R16G16, PLT_R16 ): rowConversionFunc = convRG16toR16; break;

            case PFL_PAIR( PLT_R8G8B8A8, PLT_B8G8R8A8 ): rowConversionFunc = convRGBAtoBGRA; break;
            case PFL_PAIR( PLT_R8G8B8A8, PLT_B8G8R8X8 ): rowConversionFunc = convRGBAtoBGRA; break;
            case PFL_PAIR( PLT_R8G8B8A8, PLT_R8G8B8 ): rowConversionFunc = convRGBAtoRGB; break;
            case PFL_PAIR( PLT_R8G8B8A8, PLT_B8G8R8 ): rowConversionFunc = convRGBAtoBGR; break;
            case PFL_PAIR( PLT_R8G8B8A8, PLT_R8G8 ): rowConversionFunc = convRGBAtoRG; break;
            case PFL_PAIR( PLT_R8G8B8A8, PLT_R8 ): rowConversionFunc = convRGBAtoR; break;

            case PFL_PAIR( PLT_B8G8R8A8, PLT_R8G8B8A8 ): rowConversionFunc = convRGBAtoBGRA; break;
            case PFL_PAIR( PLT_B8G8R8A8, PLT_B8G8R8X8 ): rowConversionFunc = convCopy4Bpx; break;
            case PFL_PAIR( PLT_B8G8R8A8, PLT_R8G8B8 ): rowConversionFunc = convRGBAtoBGR; break;
            case PFL_PAIR( PLT_B8G8R8A8, PLT_B8G8R8 ): rowConversionFunc = convRGBAtoRGB; break;
            case PFL_PAIR( PLT_B8G8R8A8, PLT_R8G8 ): rowConversionFunc = convBGRAtoRG; break;
            case PFL_PAIR( PLT_B8G8R8A8, PLT_R8 ): rowConversionFunc = convBGRAtoR; break;

            case PFL_PAIR( PLT_B8G8R8X8, PLT_R8G8B8A8 ): rowConversionFunc = convBGRXtoRGBA; break;
            case PFL_PAIR( PLT_B8G8R8X8, PLT_B8G8R8A8 ): rowConversionFunc = convBGRXtoBGRA; break;
            case PFL_PAIR( PLT_B8G8R8X8, PLT_R8G8B8 ): rowConversionFunc = convRGBAtoBGR; break;
            case PFL_PAIR( PLT_B8G8R8X8, PLT_B8G8R8 ): rowConversionFunc = convRGBAtoRGB; break;
            case PFL_PAIR( PLT_B8G8R8X8, PLT_R8G8 ): rowConversionFunc = convBGRAtoRG; break;
            case PFL_PAIR( PLT_B8G8R8X8, PLT_R8 ): rowConversionFunc = convBGRAtoR; break;

            case PFL_PAIR( PLT_R8G8B8, PLT_R8G8B8A8 ): rowConversionFunc = convRGBtoRGBA; break;
            case PFL_PAIR( PLT_R8G8B8, PLT_B8G8R8A8 ): rowConversionFunc = convRGBtoBGRA; break;
            case PFL_PAIR( PLT_R8G8B8, PLT_B8G8R8X8 ): rowConversionFunc = convRGBtoBGRA; break;
            case PFL_PAIR( PLT_R8G8B8, PLT_B8G8R8 ): rowConversionFunc = convRGBtoBGR; break;
            case PFL_PAIR( PLT_R8G8B8, PLT_R8G8 ): rowConversionFunc = convRGBtoRG; break;
            case PFL_PAIR( PLT_R8G8B8, PLT_R8 ): rowConversionFunc = convRGBtoR; break;

            case PFL_PAIR( PLT_B8G8R8, PLT_R8G8B8A8 ): rowConversionFunc = convRGBtoBGRA; break;
            case PFL_PAIR( PLT_B8G8R8, PLT_B8G8R8A8 ): rowConversionFunc = convRGBtoRGBA; break;
            case PFL_PAIR( PLT_B8G8R8, PLT_B8G8R8X8 ): rowConversionFunc = convRGBtoRGBA; break;
            case PFL_PAIR( PLT_B8G8R8, PLT_R8G8B8 ): rowConversionFunc = convRGBAtoBGR; break;
            case PFL_PAIR( PLT_B8G8R8, PLT_R8G8 ): rowConversionFunc = convBGRtoRG; break;
            case PFL_PAIR( PLT_B8G8R8, PLT_R8 ): rowConversionFunc = convBGRtoR; break;

            case PFL_PAIR( PLT_R8G8, PLT_R8G8B8 ): rowConversionFunc = convRGtoRGB; break;
            case PFL_PAIR( PLT_R8G8, PLT_B8G8R8 ): rowConversionFunc = convRGtoBGR; break;
            case PFL_PAIR( PLT_R8G8, PLT_R8 ): rowConversionFunc = convRGtoR; break;
                // clang-pf on
            }
#undef PFL_PAIR
        }

        if (rowConversionFunc)
        {
            for( NCount z = 0; z < volume; ++z )
            {
                for( NCount y = 0; y < height; ++y )
                {
                    size_t dest_y = verticalFlip ? height - 1 - y : y;
                    Nui8 * srcPtr = srcData + src.mSliceSize * z + src.mRowSize * y;
                    Nui8 * dstPtr = dstData + dst.mSliceSize * z + dst.mRowSize * dest_y;
                    rowConversionFunc( srcPtr, dstPtr, width );
                }
            }
            return;
        }
        else if(dstFormat == PF_R8G8N || dstFormat == PF_R8G8UN)
        {
            float multPart = 2.0f;
            float addPart  = 1.0f;

            if( dstFormat == PF_R8G8UN )
            {
                multPart = 1.0f;
                addPart  = 0.0f;
            }

            NIIf r = 0, g = 0, b = 0, a = 0;
            for( NCount z = 0; z < src.getVolume(); ++z )
            {
                for( NCount y = 0; y < src.mHeight; ++y )
                {
                    Nui8 const * srcPtr = src.getData( 0, y, z ) );
                    Nui8 * dstPtr = dst.getData( 0, y, z );

                    for( NCount x = 0; x < src.mWidth; ++x )
                    {
                        unpackColour( r, g, b, a, srcFormat, srcPtr );

                        // rgba * 2.0 - 1.0
                        //r = r * multPart - addPart;
                        //g = g * multPart - addPart;

                        *dstPtr++ = Bitwise::floatToSnorm8( r * multPart - addPart );
                        *dstPtr++ = Bitwise::floatToSnorm8( g * multPart - addPart );

                        srcPtr += src.mUnitSize;
                    }
                }
            }
        }
        else
        {

            // The brute force fallback
            float rangeM = 1.0f;
            float rangeA = 0.0f;

            const bool bSrcSigned = isSigned(src.mFormat);
            if(bSrcSigned != isSigned(dst.mFormat) && isUnit(src.mFormat))
            {
                if(bSrcSigned)
                {
                    // snormToUnorm
                    rangeM = 0.5f;
                    rangeA = 0.5f;
                }
                else
                {
                    // unormToSnorm
                    rangeM = 2.0f;
                    rangeA = -1.0f;
                }
            }

            NIIf r = 0, g = 0, b = 0, a = 0;
            for(NCount z = 0; z < volume; ++z)
            {
                for(NCount y = 0; y < height; ++y)
                {
                    size_t dest_y = verticalFlip ? height - 1 - y : y;
                    const Nui8 * srcPtr = srcData + src.mSliceSize * z + src.mRowSize * y;
                    Nui8 * dstPtr = dstData + dst.mSliceSize * z + dst.mRowSize * dest_y;

                    for(NCount x = 0; x < width; ++x)
                    {
                        unpackColour(r, g, b, a, src.mFormat, srcPtr);
                        //r = r * rangeM + rangeA;
                        //g = g * rangeM + rangeA;
                        //b = b * rangeM + rangeA;
                        //a = a * rangeM + rangeA;
                        packColour(r * rangeM + rangeA, g * rangeM + rangeA, b * rangeM + rangeA, a * rangeM + rangeA, dst.mFormat, dstPtr);
                        srcPtr += srcBytesPerPixel;
                        dstPtr += dstBytesPerPixel;
                    }
                }
            }
        }
        /*N_assert(src.mWidth == dst.mWidth && src.mHeight == dst.mHeight && src.mDepth == dst.mDepth, "error");

        // Check for compressed formats, we don't support decompression, compression or recoding
        if(PixelUtil::isCompress(src.mFormat) || PixelUtil::isCompress(dst.mFormat))
        {
            if(src.mFormat == dst.mFormat && src.isWholeSlice() && dst.isWholeSlice())
            {
                // we can copy with slice granularity, useful for Tex2DArray handling
                size_t bytesPerSlice = getSize(src.getWidth(), src.getHeight(), 1, 1, src.mFormat);
                memcpy(dst.mData + bytesPerSlice * dst.mFront, src.mData + bytesPerSlice * src.mFront,
                    bytesPerSlice * src.mDepth);
                return;
            }
            else
            {
                N_EXCEPT(UnImpl, _I18n("This method can not be used to compress or decompress images"));
            }
        }

        // The easy case
        if(src.mFormat == dst.mFormat)
        {
            // Everything consecutive?
            if(src.isWholeSlice() && dst.isWholeSlice())
            {
                memcpy(dst.getData(), src.getData(), src.getWholeSliceSize());
                return;
            }

            const NCount srcPixelSize = PixelUtil::getUnitSize(src.mFormat);
            const NCount dstPixelSize = PixelUtil::getUnitSize(dst.mFormat);
            Nui8 * srcptr = src.mData * src.mUnitSize + src.mLeft + src.mTop * src.mRowSize + src.mFront * src.mSliceSize;
            Nui8 * dstptr = dst.mData * dst.mUnitSize + dst.mLeft + dst.mTop * dst.mRowSize + dst.mFront * dst.mSliceSize;

            // Calculate pitches+skips in bytes
            const NCount srcRowPitchBytes = src.mRowSize;
            const NCount srcSliceSkipBytes = src.getSliceOft();

            const NCount dstRowPitchBytes = dst.mRowSize;
            const NCount dstSliceSkipBytes = dst.getSliceOft();

            // Otherwise, copy per row
            const NCount rowSize = src.mWidth * srcPixelSize;
            for(NCount z = src.mFront; z < src.mFront + src.mDepth; ++z)
            {
                for(NCount y = src.mTop; y < src.mTop + src.mHeight; ++y)
                {
                    memcpy(dstptr, srcptr, rowSize);
                    srcptr += srcRowPitchBytes;
                    dstptr += dstRowPitchBytes;
                }
                srcptr += srcSliceSkipBytes;
                dstptr += dstSliceSkipBytes;
            }
            return;
        }
        // Converting to PF_X8R8G8B8 is exactly the same as converting to
        // PF_A8R8G8B8. (same with PF_X8B8G8R8 and PF_A8B8G8R8)
        if(dst.mFormat == PF_X8R8G8B8 || dst.mFormat == PF_X8B8G8R8)
        {
            // Do the same conversion, with PF_A8R8G8B8, which has a lot of
            // optimized conversions
            PixelBlock tempdst = dst;
            tempdst.mFormat = dst.mFormat==PF_X8R8G8B8?PF_A8R8G8B8:PF_A8B8G8R8;
            conv(src, tempdst);
            return;
        }
        // Converting from PF_X8R8G8B8 is exactly the same as converting from
        // PF_A8R8G8B8, given that the destination mFormat does not have alpha.
        if((src.mFormat == PF_X8R8G8B8||src.mFormat == PF_X8B8G8R8) && !isAlpha(dst.mFormat))
        {
            // Do the same conversion, with PF_A8R8G8B8, which has a lot of
            // optimized conversions
            PixelBlock tempsrc = src;
            tempsrc.mFormat = src.mFormat == PF_X8R8G8B8 ? PF_A8R8G8B8:PF_A8B8G8R8;
            conv(tempsrc, dst);
            return;
        }

// NB VC6 can't handle the templates required for optimised conversion, tough
#if N_COMPILER != N_CPP_MSVC || N_COMPILER_VER >= 1300
        // Is there a specialized, inlined, conversion?
        if(doOptimizedConversion(src, dst))
        {
            // If so, good
            return;
        }
#endif
        const NCount srcPixelSize = PixelUtil::getUnitSize(src.mFormat);
        const NCount dstPixelSize = PixelUtil::getUnitSize(dst.mFormat);
        Nui8 * srcptr = src.mData + src.mLeft * src.mUnitSize + src.mTop * src.mRowSize + src.mFront * src.mSliceSize;

        Nui8 * dstptr = dst.mData + dst.mLeft * dst.mUnitSize + dst.mTop * dst.mRowSize + dst.mFront * dst.mSliceSize;

        // Old way, not taking into account box dimensions
        //Nui8 *srcptr = static_cast<Nui8*>(src.mData), *dstptr = static_cast<Nui8*>(dst.mData);

        // Calculate pitches+skips in bytes
        const NCount srcRowSkipBytes = src.getRowOft();
        const NCount srcSliceSkipBytes = src.getSliceOft();
        const NCount dstRowSkipBytes = dst.getRowOft();
        const NCount dstSliceSkipBytes = dst.getSliceOft();

        // The brute force fallback
        NIIf r = 0,g = 0,b = 0,a = 1;
        for(NCount z = src.mFront; z < src.mFront + src.mDepth; ++z)
        {
            for(NCount y = src.mTop; y < src.mTop + src.mHeight; ++y)
            {
                for(NCount x = src.mLeft; x < src.mLeft + src.mWidth; ++x)
                {
                    unpackColour(r, g, b, a, src.mFormat, srcptr);
                    packColour(r, g, b, a, dst.mFormat, dstptr);
                    srcptr += srcPixelSize;
                    dstptr += dstPixelSize;
                }
                srcptr += srcRowSkipBytes;
                dstptr += dstRowSkipBytes;
            }
            srcptr += srcSliceSkipBytes;
            dstptr += dstSliceSkipBytes;
        }*/
    }
    //------------------------------------------------------------------------------
#if N_COMPILER == N_CPP_MSVC && N_COMPILER_VER < 1800
    inline float roundf(float x)
    {
        return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
    }
#endif
    //-----------------------------------------------------------------------------------
    PixelLayoutType PixelUtil::getDataLayout(PixelFormat pf)
    {
        return (PixelLayoutType)getVerbose(pf).mLayout;
    }
    //-----------------------------------------------------------------------------------
    NCount PixelUtilgetSize(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf, NCount rowAlign)
    {
        if(isCompress( pf ) )
        {
            switch( pf )
            {
            // BCn formats work by dividing the image into 4x4 blocks, then
            // encoding each 4x4 block with a certain number of bytes.
            case PF_BC1UN:
            case PF_BC1UN_SRGB:
            case PF_EAC_R11UN:
            case PF_EAC_R11N:
            case PF_ETC1_R8G8B8UN:
            case PF_ETC2_R8G8B8UN:
            case PF_ETC2_R8G8B8UN_SRGB:
            case PF_ETC2_R8G8B8A1UN:
            case PF_ETC2_R8G8B8A1UN_SRGB:
            case PF_ATC_RGB:
                return ( (width + 3u) / 4u ) * ( (height + 3u) / 4u ) * 8u * depth * array;
            case PF_BC2UN:
            case PF_BC2UN_SRGB:
            case PF_BC3UN:
            case PF_BC3UN_SRGB:
            case PF_BC4UN:
            case PF_BC4N:
            case PF_BC5N:
            case PF_BC5UN:
            case PF_BC6H_16:
            case PF_BC6H_u16:
            case PF_BC7_UN:
            case PF_BC7_UN_SRGB:
            case PF_ETC2_R8G8B8A8UN:
            case PF_ETC2_R8G8B8A8UN_SRGB:
            case PF_EAC_R11G11UN:
            case PF_EAC_R11G11N:
            case PF_ATC_RGBA_EXPLICIT_A:
            case PF_ATC_RGBA_INTERPOLATED_A:
                return ( (width + 3u) / 4u) * ( (height + 3u) / 4u ) * 16u * depth * array;
            // Size calculations from the PVRTC OpenGL extension spec
            // http://www.khronos.org/registry/gles/extensions/IMG/IMG_texture_compression_pvrtc.txt
            // Basically, 32 bytes is the minimum texture size.  Smaller textures are padded up to 32 bytes
            case PF_PVRTC_RGB_2BPP:     case PF_PVRTC_RGB_2BPP_SRGB:
            case PF_PVRTC_RGBA_2BPP:    case PF_PVRTC_RGBA_2BPP_SRGB:
            case PF_PVRTC2_2BPP:        case PF_PVRTC2_2BPP_SRGB:
                return (std::max<uint32>( width, 16u ) * std::max<uint32>( height, 8u ) * 2u + 7u) / 8u * depth * array;
            case PF_PVRTC_RGB_4BPP:     case PF_PVRTC_RGB_4BPP_SRGB:
            case PF_PVRTC_RGBA_4BPP:    case PF_PVRTC_RGBA_4BPP_SRGB:
            case PF_PVRTC2_4BPP:        case PF_PVRTC2_4BPP_SRGB:
                return (std::max<uint32>( width, 8u ) * std::max<uint32>( height, 8u ) * 4u + 7u) / 8u * depth * array;
            case PF_ASTC_RGBAUN_4X4_LDR:   case PF_ASTC_RGBAUN_4X4_SRGB:
            case PF_ASTC_RGBAUN_5X4_LDR:   case PF_ASTC_RGBAUN_5X4_SRGB:
            case PF_ASTC_RGBAUN_5X5_LDR:   case PF_ASTC_RGBAUN_5X5_SRGB:
            case PF_ASTC_RGBAUN_6X5_LDR:   case PF_ASTC_RGBAUN_6X5_SRGB:
            case PF_ASTC_RGBAUN_6X6_LDR:   case PF_ASTC_RGBAUN_6X6_SRGB:
            case PF_ASTC_RGBAUN_8X5_LDR:   case PF_ASTC_RGBAUN_8X5_SRGB:
            case PF_ASTC_RGBAUN_8X6_LDR:   case PF_ASTC_RGBAUN_8X6_SRGB:
            case PF_ASTC_RGBAUN_8X8_LDR:   case PF_ASTC_RGBAUN_8X8_SRGB:
            case PF_ASTC_RGBAUN_10X5_LDR:  case PF_ASTC_RGBAUN_10X5_SRGB:
            case PF_ASTC_RGBAUN_10X6_LDR:  case PF_ASTC_RGBAUN_10X6_SRGB:
            case PF_ASTC_RGBAUN_10X8_LDR:  case PF_ASTC_RGBAUN_10X8_SRGB:
            case PF_ASTC_RGBAUN_10X10_LDR: case PF_ASTC_RGBAUN_10X10_SRGB:
            case PF_ASTC_RGBAUN_12X10_LDR: case PF_ASTC_RGBAUN_12X10_SRGB:
            case PF_ASTC_RGBAUN_12X12_LDR: case PF_ASTC_RGBAUN_12X12_SRGB:
            {
                uint32 blockWidth = PixelUtil::getBlockWidth( pf );
                uint32 blockHeight = PixelUtil::getBlockHeight( pf );
                return  (multiCeil( width, blockWidth ) / blockWidth) *
                        (multiCeil( height, blockHeight ) / blockHeight) * depth * 16u;
            }
            default:
                N_EXCEPT(InvalidParam, "Invalid compressed pixel pf");
            }
        }
        else
        {
            return multiCeil(width * getUnitSize(pf), rowAlign) * height * depth * array;
        }
    }
    //-----------------------------------------------------------------------------------
    NCount PixelUtil::getSize(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf,
        NCount mipLevel, NCount rowAlign)
    {
        NCount re = 0;
        if(mipLevel == 1)
        {
            re = PixelUtilgetSize(width, height, depth, array, pf, rowAlign);
        }
        else
        {
            while((width > 1u || height > 1u || depth > 1u) && mipLevel > 0)
            {
                re += PixelUtilgetSize( width, height, depth, array, pf, rowAlign );
                width   = std::max(1u, width  >> 1u);
                height  = std::max(1u, height >> 1u);
                depth   = std::max(1u, depth  >> 1u);
                --mipLevel;
            }

            if( width == 1u && height == 1u && depth == 1u && mipLevel > 0 )
            {
                //Add 1x1x1 mip.
                re += PixelUtilgetSize( width, height, depth, array, pf, 1, rowAlign );
                --mipLevel;
            }
        }

        return re;
    }
    //-----------------------------------------------------------------------
    NCount PixelUtil::getMaxMipmap(NCount maxResolution)
    {
        if( !maxResolution ) //log( 0 ) is undefined.
            return 0;

        NCount numMipmaps;
#if (ANDROID || (OGRE_COMPILER == OGRE_COMPILER_MSVC && OGRE_COMP_VER < 1800))
        numMipmaps = static_cast<NCount>( floorf( logf( static_cast<float>(maxResolution) ) / logf( 2.0f ) ) );
#else
        numMipmaps = static_cast<NCount>( floorf( log2f( static_cast<float>(maxResolution) ) ) );
#endif
        return numMipmaps + 1u;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isHwMipmap(PixelFormat pf)
    {
        switch(pf)
        {
        case PF_R8G8B8A8UN:
        case PF_R8G8B8A8UN_SRGB:
        case PF_B5G6R5UN:
        case PF_B8G8R8A8UN:
        case PF_B8G8R8A8UN_SRGB:
        case PF_B8G8R8X8UN:
        case PF_B8G8R8X8UN_SRGB:
        case PF_R16G16B16A16:
        case PF_R16G16B16A16UN:
        case PF_R16G16:
        case PF_R16G16UN:
        case PF_R32:
        case PF_R32G32B32A32:
        case PF_B4G4R4A4UN:
        //case PF_R32G32B32: (optional). This is a weird pf. Fallback to SW
        case PF_R16G16B16A16N:
        case PF_R32G32:
        case PF_R10G10B10A2UN:
        case PF_R11G11B10:
        case PF_R8G8B8A8N:
        case PF_R16G16N:
        case PF_R8G8UN:
        case PF_R8G8N:
        case PF_R16:
        case PF_R16UN:
        case PF_R16N:
        case PF_R8UN:
        case PF_R8N:
        case PF_A8UN:
        //case PF_B5G5R5A1UN: (optional)
            return true;
        default:
            return false;
        }

        return false;
    }
    //-----------------------------------------------------------------------
    uint32 PixelUtil::getBlockWidth(PixelFormat pf, bool apiStrict)
    {
        switch( pf )
        {
        // These formats work by dividing the image into 4x4 blocks, then encoding each
        // 4x4 block with a certain number of bytes.
        case PF_BC1UN: case PF_BC1UN_SRGB:
        case PF_BC2UN: case PF_BC2UN_SRGB:
        case PF_BC3UN: case PF_BC3UN_SRGB:
        case PF_BC4UN: case PF_BC4N:
        case PF_BC5UN: case PF_BC5N:
        case PF_BC6H_u16: case PF_BC6H_16:
        case PF_BC7_UN: case PF_BC7_UN_SRGB:
        case PF_ETC2_R8G8B8UN: case PF_ETC2_R8G8B8UN_SRGB:
        case PF_ETC2_R8G8B8A8UN: case PF_ETC2_R8G8B8A8UN_SRGB:
        case PF_ETC2_R8G8B8A1UN: case PF_ETC2_R8G8B8A1UN_SRGB:
        case PF_EAC_R11UN: case PF_EAC_R11N:
        case PF_EAC_R11G11UN: case PF_EAC_R11G11N:
        case PF_ATC_RGB:
        case PF_ATC_RGBA_EXPLICIT_A:
        case PF_ATC_RGBA_INTERPOLATED_A:
            return 4u;

        case PF_ETC1_R8G8B8UN:
            return apiStrict ? 0u : 4u;

            // Size calculations from the PVRTC OpenGL extension spec
            // http://www.khronos.org/registry/gles/extensions/IMG/IMG_texture_compression_pvrtc.txt
            //  "Sub-images are not supportable because the PVRTC
            //  algorithm uses significant adjacency information, so there is
            //  no discrete block of texels that can be decoded as a standalone
            //  sub-unit, and so it follows that no stand alone sub-unit of
            //  data can be loaded without changing the decoding of surrounding
            //  texels."
            // In other words, if the user wants atlas, they can't be automatic
        case PF_PVRTC_RGB_2BPP:    case PF_PVRTC_RGB_2BPP_SRGB:
        case PF_PVRTC_RGBA_2BPP:   case PF_PVRTC_RGBA_2BPP_SRGB:
        case PF_PVRTC_RGB_4BPP:    case PF_PVRTC_RGB_4BPP_SRGB:
        case PF_PVRTC_RGBA_4BPP:   case PF_PVRTC_RGBA_4BPP_SRGB:
        case PF_PVRTC2_2BPP:   case PF_PVRTC2_2BPP_SRGB:
        case PF_PVRTC2_4BPP:   case PF_PVRTC2_4BPP_SRGB:
            return 0u;

        case PF_ASTC_RGBAUN_4X4_LDR:
        case PF_ASTC_RGBAUN_4X4_SRGB:
            return 4u;
        case PF_ASTC_RGBAUN_5X4_LDR:
        case PF_ASTC_RGBAUN_5X4_SRGB:
        case PF_ASTC_RGBAUN_5X5_LDR:
        case PF_ASTC_RGBAUN_5X5_SRGB:
            return 5u;
        case PF_ASTC_RGBAUN_6X5_LDR:
        case PF_ASTC_RGBAUN_6X5_SRGB:
        case PF_ASTC_RGBAUN_6X6_LDR:
        case PF_ASTC_RGBAUN_6X6_SRGB:
            return 6u;
        case PF_ASTC_RGBAUN_8X5_LDR:
        case PF_ASTC_RGBAUN_8X5_SRGB:
        case PF_ASTC_RGBAUN_8X6_LDR:
        case PF_ASTC_RGBAUN_8X6_SRGB:
        case PF_ASTC_RGBAUN_8X8_LDR:
        case PF_ASTC_RGBAUN_8X8_SRGB:
            return 8u;
        case PF_ASTC_RGBAUN_10X5_LDR:
        case PF_ASTC_RGBAUN_10X5_SRGB:
        case PF_ASTC_RGBAUN_10X6_LDR:
        case PF_ASTC_RGBAUN_10X6_SRGB:
        case PF_ASTC_RGBAUN_10X8_LDR:
        case PF_ASTC_RGBAUN_10X8_SRGB:
        case PF_ASTC_RGBAUN_10X10_LDR:
        case PF_ASTC_RGBAUN_10X10_SRGB:
            return 10u;
        case PF_ASTC_RGBAUN_12X10_LDR:
        case PF_ASTC_RGBAUN_12X10_SRGB:
        case PF_ASTC_RGBAUN_12X12_LDR:
        case PF_ASTC_RGBAUN_12X12_SRGB:
            return 12u;
        default:
            return 1u;
        }
    }
    //-----------------------------------------------------------------------
    uint32 PixelUtil::getBlockHeight( PixelFormat pf, bool apiStrict )
    {
        switch( pf )
        {
        case PF_ASTC_RGBAUN_4X4_LDR:
        case PF_ASTC_RGBAUN_4X4_SRGB:
        case PF_ASTC_RGBAUN_5X4_LDR:
        case PF_ASTC_RGBAUN_5X4_SRGB:
            return 4u;
        case PF_ASTC_RGBAUN_5X5_LDR:
        case PF_ASTC_RGBAUN_5X5_SRGB:
        case PF_ASTC_RGBAUN_6X5_LDR:
        case PF_ASTC_RGBAUN_6X5_SRGB:
        case PF_ASTC_RGBAUN_8X5_LDR:
        case PF_ASTC_RGBAUN_8X5_SRGB:
        case PF_ASTC_RGBAUN_10X5_LDR:
        case PF_ASTC_RGBAUN_10X5_SRGB:
            return 5u;
        case PF_ASTC_RGBAUN_6X6_LDR:
        case PF_ASTC_RGBAUN_6X6_SRGB:
        case PF_ASTC_RGBAUN_8X6_LDR:
        case PF_ASTC_RGBAUN_8X6_SRGB:
        case PF_ASTC_RGBAUN_10X6_LDR:
        case PF_ASTC_RGBAUN_10X6_SRGB:
            return 6u;
        case PF_ASTC_RGBAUN_8X8_LDR:
        case PF_ASTC_RGBAUN_8X8_SRGB:
        case PF_ASTC_RGBAUN_10X8_LDR:
        case PF_ASTC_RGBAUN_10X8_SRGB:
            return 8u;
        case PF_ASTC_RGBAUN_10X10_LDR:
        case PF_ASTC_RGBAUN_10X10_SRGB:
        case PF_ASTC_RGBAUN_12X10_LDR:
        case PF_ASTC_RGBAUN_12X10_SRGB:
            return 10u;
        case PF_ASTC_RGBAUN_12X12_LDR:
        case PF_ASTC_RGBAUN_12X12_SRGB:
            return 12u;

        default:
            return getBlockWidth( pf, apiStrict );
        }

        return getBlockWidth( pf, apiStrict );
    }
    //-----------------------------------------------------------------------------------
    Nui8 * PixelUtil::getData(Nui8 * base, NCount width, NCount height, NCount depth, NCount array, NCount mipLevel,
        PixelFormat pf)
    {
        Nui8 * re = base;

        for( NCount i = 0; i < mipLevel; ++i )
        {
            re += PixelUtil::getSize( width, height, depth, array, pf);

            width   = std::max( 1u, width  >> 1u );
            height  = std::max( 1u, height >> 1u );
            depth   = std::max( 1u, depth  >> 1u );
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isFloat16(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Float16) == PFF_Float16;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isFloatRare(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_FloatE) == PFF_FloatE;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isInteger(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & (PFF_Int | PFF_Int16)) != 0;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isUnit(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Unit) == PFF_Unit;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isSigned(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Signed) == PFF_Signed;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isStencil(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Stencil) == PFF_Stencil;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isSRGB(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_SRGB) == PFF_SRGB;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isPalleteYUV(PixelFormat pf)
    {
        return (getVerbose(pf).mFeature & PFF_Pallete) == PFF_Pallete;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isDirect(PixelFormat pf)
    {
        if( pf == PF_UNKNOWN )
            return false;
        return (getVerbose(pf).mFeature & (PFF_Compress | PFF_Depth | PFF_Stencil)) == 0;
    }
    //-----------------------------------------------------------------------------------
    bool PixelUtil::isSwMipmaps(PixelFormat format, NCount depth, NCount face, Filter filter)
    {
        ImageDownsampler2D * sampler2DFunc = 0;
        ImageDownsamplerCube * samplerCubeFunc = 0;
        ImageBlur2D * separableBlur2DFunc = 0;

        bool srgb = PixelUtil::isSRGB(format);

        bool re = getDownsampler(format, &sampler2DFunc, &samplerCubeFunc, &separableBlur2DFunc,
            srgb, depth, array, face, filter);

        return re;
    }
    //-----------------------------------------------------------------------------------
    /*bool PixelUtil::isSRGBSupport( PixelFormat pf )
    {
        return getSRGB( pf ) != getLinear( pf );
    }*/
    //-----------------------------------------------------------------------------------
    PixelFormat PixelUtil::getSRGB(PixelFormat pf)
    {
        switch(pf)
        {
        case PF_R8G8B8A8UN:     return PF_R8G8B8A8UN_SRGB;
        case PF_BC1UN:          return PF_BC1UN_SRGB;
        case PF_BC2UN:          return PF_BC2UN_SRGB;
        case PF_BC3UN:          return PF_BC3UN_SRGB;
        case PF_B8G8R8A8UN:     return PF_B8G8R8A8UN_SRGB;
        case PF_B8G8R8X8UN:    return PF_B8G8R8X8UN_SRGB;
        case PF_BC7_UN:         return PF_BC7_UN_SRGB;
        case PF_R8G8B8UN:        return PF_R8G8B8UN_SRGB;
        case PF_B8G8R8UN:        return PF_B8G8R8UN_SRGB;
        case PF_ETC2_R8G8B8UN:   return PF_ETC2_R8G8B8UN_SRGB;
        case PF_ETC2_R8G8B8A8UN:  return PF_ETC2_R8G8B8A8UN_SRGB;
        case PF_ETC2_R8G8B8A1UN: return PF_ETC2_R8G8B8A1UN_SRGB;
        case PF_ASTC_RGBAUN_4X4_LDR:   return PF_ASTC_RGBAUN_4X4_SRGB;
        case PF_ASTC_RGBAUN_5X4_LDR:   return PF_ASTC_RGBAUN_5X4_SRGB;
        case PF_ASTC_RGBAUN_5X5_LDR:   return PF_ASTC_RGBAUN_5X5_SRGB;
        case PF_ASTC_RGBAUN_6X5_LDR:   return PF_ASTC_RGBAUN_6X5_SRGB;
        case PF_ASTC_RGBAUN_6X6_LDR:   return PF_ASTC_RGBAUN_6X6_SRGB;
        case PF_ASTC_RGBAUN_8X5_LDR:   return PF_ASTC_RGBAUN_8X5_SRGB;
        case PF_ASTC_RGBAUN_8X6_LDR:   return PF_ASTC_RGBAUN_8X6_SRGB;
        case PF_ASTC_RGBAUN_8X8_LDR:   return PF_ASTC_RGBAUN_8X8_SRGB;
        case PF_ASTC_RGBAUN_10X5_LDR:  return PF_ASTC_RGBAUN_10X5_SRGB;
        case PF_ASTC_RGBAUN_10X6_LDR:  return PF_ASTC_RGBAUN_10X6_SRGB;
        case PF_ASTC_RGBAUN_10X8_LDR:  return PF_ASTC_RGBAUN_10X8_SRGB;
        case PF_ASTC_RGBAUN_10X10_LDR: return PF_ASTC_RGBAUN_10X10_SRGB;
        case PF_ASTC_RGBAUN_12X10_LDR: return PF_ASTC_RGBAUN_12X10_SRGB;
        case PF_ASTC_RGBAUN_12X12_LDR: return PF_ASTC_RGBAUN_12X12_SRGB;
        default:
            return pf;
        }

        return pf;
    }
    //-----------------------------------------------------------------------------------
    PixelFormat PixelUtil::getLinear(PixelFormat sRgbFormat)
    {
        switch(sRgbFormat)
        {
        case PF_R8G8B8A8UN_SRGB:        return PF_R8G8B8A8UN;
        case PF_BC1UN_SRGB:             return PF_BC1UN;
        case PF_BC2UN_SRGB:             return PF_BC2UN;
        case PF_BC3UN_SRGB:             return PF_BC3UN;
        case PF_B8G8R8A8UN_SRGB:        return PF_B8G8R8A8UN;
        case PF_B8G8R8X8UN_SRGB:        return PF_B8G8R8X8UN;
        case PF_BC7_UN_SRGB:            return PF_BC7_UN;
        case PF_R8G8B8UN_SRGB:          return PF_R8G8B8UN;
        case PF_B8G8R8UN_SRGB:          return PF_B8G8R8UN;
        case PF_ETC2_R8G8B8UN_SRGB:     return PF_ETC2_R8G8B8UN;
        case PF_ETC2_R8G8B8A8UN_SRGB:   return PF_ETC2_R8G8B8A8UN;
        case PF_ETC2_R8G8B8A1UN_SRGB:   return PF_ETC2_R8G8B8A1UN;
        case PF_ASTC_RGBAUN_4X4_SRGB:  return PF_ASTC_RGBAUN_4X4_LDR;
        case PF_ASTC_RGBAUN_5X4_SRGB:  return PF_ASTC_RGBAUN_5X4_LDR;
        case PF_ASTC_RGBAUN_5X5_SRGB:  return PF_ASTC_RGBAUN_5X5_LDR;
        case PF_ASTC_RGBAUN_6X5_SRGB:  return PF_ASTC_RGBAUN_6X5_LDR;
        case PF_ASTC_RGBAUN_6X6_SRGB:  return PF_ASTC_RGBAUN_6X6_LDR;
        case PF_ASTC_RGBAUN_8X5_SRGB:  return PF_ASTC_RGBAUN_8X5_LDR;
        case PF_ASTC_RGBAUN_8X6_SRGB:  return PF_ASTC_RGBAUN_8X6_LDR;
        case PF_ASTC_RGBAUN_8X8_SRGB:  return PF_ASTC_RGBAUN_8X8_LDR;
        case PF_ASTC_RGBAUN_10X5_SRGB: return PF_ASTC_RGBAUN_10X5_LDR;
        case PF_ASTC_RGBAUN_10X6_SRGB: return PF_ASTC_RGBAUN_10X6_LDR;
        case PF_ASTC_RGBAUN_10X8_SRGB: return PF_ASTC_RGBAUN_10X8_LDR;
        case PF_ASTC_RGBAUN_10X10_SRGB:return PF_ASTC_RGBAUN_10X10_LDR;
        case PF_ASTC_RGBAUN_12X10_SRGB:return PF_ASTC_RGBAUN_12X10_LDR;
        case PF_ASTC_RGBAUN_12X12_SRGB:return PF_ASTC_RGBAUN_12X12_LDR;
        default:
            return sRgbFormat;
        }

        return sRgbFormat;
    }
    //-----------------------------------------------------------------------------------
    PixelFormat PixelUtil::getFamily(PixelFormat pf)
    {
        switch(pf)
        {
        case PF_R32G32B32A32:
        case PF_R32G32B32A32ui:
        case PF_R32G32B32A32i:
            return PF_R32G32B32A32ui;

        case PF_R32G32B32:
        case PF_R32G32B32ui:
        case PF_R32G32B32i:
            return PF_R32G32B32ui;

        case PF_R16G16B16A16:
        case PF_R16G16B16A16UN:
        case PF_R16G16B16A16ui:
        case PF_R16G16B16A16N:
        case PF_R16G16B16A16i:
            return PF_R16G16B16A16ui;

        case PF_R32G32:
        case PF_R32G32ui:
        case PF_R32G32i:
            return PF_R32G32ui;

        case PF_R10G10B10A2UN:
        case PF_R10G10B10A2ui:
            return PF_R10G10B10A2ui;

        case PF_R11G11B10:
            return PF_R11G11B10;

        case PF_R8G8B8A8UN:
        case PF_R8G8B8A8UN_SRGB:
        case PF_R8G8B8A8ui:
        case PF_R8G8B8A8N:
        case PF_R8G8B8A8i:
            return PF_R8G8B8A8UN;

        case PF_R16G16:
        case PF_R16G16UN:
        case PF_R16G16ui:
        case PF_R16G16N:
        case PF_R16G16i:
            return PF_R16G16ui;

        case PF_Depth32:
        case PF_R32:
        case PF_R32ui:
        case PF_R32i:
            return PF_R32ui;

        case PF_Depth24UN:
        case PF_Depth24UN_Stencil8ui:
            return PF_Depth24UN_Stencil8ui;

        case PF_R8G8UN:
        case PF_R8G8ui:
        case PF_R8G8N:
        case PF_R8G8i:
            return PF_R8G8ui;

        case PF_R16:
        case PF_Depth16UN:
        case PF_R16UN:
        case PF_R16ui:
        case PF_R16N:
        case PF_R16i:
            return PF_R16ui;

        case PF_R8UN:
        case PF_R8ui:
        case PF_R8N:
        case PF_R8i:
            return PF_R8ui;

        case PF_BC1UN:
        case PF_BC1UN_SRGB:
            return PF_BC1UN;
        case PF_BC2UN:
        case PF_BC2UN_SRGB:
            return PF_BC2UN;
        case PF_BC3UN:
        case PF_BC3UN_SRGB:
            return PF_BC3UN;
        case PF_BC4UN:
        case PF_BC4N:
            return PF_BC4UN;
        case PF_BC5UN:
        case PF_BC5N:
            return PF_BC5UN;

        case PF_B8G8R8A8UN:
        case PF_B8G8R8A8UN_SRGB:
            return PF_B8G8R8A8UN;

        case PF_B8G8R8X8UN:
        case PF_B8G8R8X8UN_SRGB:
            return PF_B8G8R8X8UN;

        case PF_BC6H_u16:
        case PF_BC6H_16:
            return PF_BC6H_u16;

        case PF_BC7_UN:
        case PF_BC7_UN_SRGB:
            return PF_BC7_UN;

        default:
            return pf;
        }

        return pf;
    }
    //-----------------------------------------------------------------------
}