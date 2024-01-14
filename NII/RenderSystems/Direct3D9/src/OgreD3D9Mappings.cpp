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
#include "OgreD3D9Mappings.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreLogManager.h"
#include "OgreException.h"

namespace Ogre 
{
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(ShadeOptions so)
    {
        switch( so )
        {
        case SO_FLAT:
            return D3DSHADE_FLAT;
        case SO_GOURAUD:
            return D3DSHADE_GOURAUD;
        case SO_PHONG:
            return D3DSHADE_PHONG;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    D3DLIGHTTYPE D3D9Mappings::get(Ogre::Light::LightTypes lightType)
    {
        switch( lightType )
        {
        case Light::LT_POINT:
            return D3DLIGHT_POINT;
        case Light::LT_DIRECTIONAL:
            return D3DLIGHT_DIRECTIONAL;
        case Light::LT_SPOTLIGHT:
            return D3DLIGHT_SPOT;
        }
        return D3DLIGHT_FORCE_DWORD;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(TextureCoordPrc m, const D3DCAPS9& caps)
    {
        switch( m )
        {
        case TCP_NONE:
            return D3DTSS_TCI_PASSTHRU;
        case TCP_ENV_REFLECTION_INV:
            return D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
        case TCP_ENV_REFLECTION:
            if (caps.VertexProcessingCaps & D3DVTXPCAPS_TEXGEN_SPHEREMAP)
            {
                // Use sphere map if available
                return D3DTSS_TCI_SPHEREMAP;
            }
            else
            {
                // If not, fall back on camera space reflection vector which isn't as good
                return D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
            }
        case TCP_ENV_NORMAL:
            return D3DTSS_TCI_CAMERASPACENORMAL;
        case TCP_ENV_SPHERE:
            if (caps.VertexProcessingCaps & D3DVTXPCAPS_TEXGEN_SPHEREMAP)
            {
                // Use sphere map if available
                return D3DTSS_TCI_SPHEREMAP;
            }
            else
            {
                // If not, fall back on camera space normal which isn't as good
                return D3DTSS_TCI_CAMERASPACENORMAL;
            }
        case TCP_ENV_PROJECTIVE:
            return D3DTSS_TCI_CAMERASPACEPOSITION;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    D3DTEXTUREADDRESS D3D9Mappings::get(TextureAddressingMode tam, const D3DCAPS9& devCaps)
    {
        switch( tam )
        {
        case ShaderChTextureUnit::TAM_WRAP:
            return D3DTADDRESS_WRAP;
        case ShaderChTextureUnit::TAM_MIRROR:
            return D3DTADDRESS_MIRROR;
        case ShaderChTextureUnit::TAM_CLAMP:
            return D3DTADDRESS_CLAMP;
        case ShaderChTextureUnit::TAM_BORDER:
            if (devCaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
                return D3DTADDRESS_BORDER;
            else
                return D3DTADDRESS_CLAMP;
        }
        return D3DTADDRESS_FORCE_DWORD;
    }
    //---------------------------------------------------------------------
    D3DTEXTURESTAGESTATETYPE D3D9Mappings::get(LayerBlendType lbt)
    {
        switch( lbt )
        {
        case LBT_COLOUR:
            return D3DTSS_COLOROP;
        case LBT_ALPHA:
            return D3DTSS_ALPHAOP;
        }
        return  D3DTSS_FORCE_DWORD;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(LayerBlendSource lbs, bool perStageConstants)
    {
        switch( lbs )
        {
        case LBS_CURRENT:
            return D3DTA_CURRENT;
        case LBS_TEXTURE:
            return D3DTA_TEXTURE;
        case LBS_DIFFUSE:
            return D3DTA_DIFFUSE;
        case LBS_SPECULAR:
            return D3DTA_SPECULAR;
        case LBS_MANUAL:
            return perStageConstants ? D3DTA_CONSTANT : D3DTA_TFACTOR;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(LayerBlendOperationEx lbo, const D3DCAPS9& devCaps)
    {
        switch( lbo )
        {
        case LBX_SOURCE1:
            return D3DTOP_SELECTARG1;
        case LBX_SOURCE2:
            return D3DTOP_SELECTARG2;
        case LBX_MODULATE:
            return D3DTOP_MODULATE;
        case LBX_MODULATE_X2:
            return D3DTOP_MODULATE2X;
        case LBX_MODULATE_X4:
            return D3DTOP_MODULATE4X;
        case LBX_ADD:
            return D3DTOP_ADD;
        case LBX_ADD_SIGNED:
            return D3DTOP_ADDSIGNED;
        case LBX_ADD_SMOOTH:
            return D3DTOP_ADDSMOOTH;
        case LBX_SUBTRACT:
            return D3DTOP_SUBTRACT;
        case LBX_BLEND_DIFFUSE_ALPHA:
            return D3DTOP_BLENDDIFFUSEALPHA;
        case LBX_BLEND_TEXTURE_ALPHA:
            return D3DTOP_BLENDTEXTUREALPHA;
        case LBX_BLEND_CURRENT_ALPHA:
            return D3DTOP_BLENDCURRENTALPHA;
        case LBX_BLEND_MANUAL:
            return D3DTOP_BLENDFACTORALPHA;
        case LBX_DOTPRODUCT:
            if (devCaps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3)
                return D3DTOP_DOTPRODUCT3;
            else
                return D3DTOP_MODULATE;
        case LBX_BLEND_DIFFUSE_COLOUR:
            if (devCaps.TextureOpCaps & D3DTEXOPCAPS_LERP)
                return D3DTOP_LERP;
            else
                return D3DTOP_MODULATE;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    D3DBLEND D3D9Mappings::get(ColourFactor sbf)
    {
        switch( sbf )
        {
        case SBF_ONE:
            return D3DBLEND_ONE;
        case SBF_ZERO:
            return D3DBLEND_ZERO;
        case SBF_DEST_COLOUR:
            return D3DBLEND_DESTCOLOR;
        case SBF_SOURCE_COLOUR:
            return D3DBLEND_SRCCOLOR;
        case SBF_ONE_MINUS_DEST_COLOUR:
            return D3DBLEND_INVDESTCOLOR;
        case SBF_ONE_MINUS_SOURCE_COLOUR:
            return D3DBLEND_INVSRCCOLOR;
        case SBF_DEST_ALPHA:
            return D3DBLEND_DESTALPHA;
        case SBF_SOURCE_ALPHA:
            return D3DBLEND_SRCALPHA;
        case SBF_ONE_MINUS_DEST_ALPHA:
            return D3DBLEND_INVDESTALPHA;
        case SBF_ONE_MINUS_SOURCE_ALPHA:
            return D3DBLEND_INVSRCALPHA;
        }
        return D3DBLEND_FORCE_DWORD;
    }
    //---------------------------------------------------------------------
    D3DBLENDOP D3D9Mappings::get(Ogre::ColourBlendMode sbo)
    {
        switch(sbo)
        {
        case SBO_ADD:
            return D3DBLENDOP_ADD;
        case SBO_SUBTRACT:
            return D3DBLENDOP_SUBTRACT;
        case SBO_REVERSE_SUBTRACT:
            return D3DBLENDOP_REVSUBTRACT;
        case SBO_MIN:
            return D3DBLENDOP_MIN;
        case SBO_MAX:
            return D3DBLENDOP_MAX;
        }

        return D3DBLENDOP_FORCE_DWORD;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(CmpMode cf)
    {
        switch( cf )
        {
        case CMPF_ALWAYS_FAIL:
            return D3DCMP_NEVER;
        case CMPF_ALWAYS_PASS:
            return D3DCMP_ALWAYS;
        case CMPF_LESS:
            return D3DCMP_LESS;
        case CMPF_LESS_EQUAL:
            return D3DCMP_LESSEQUAL;
        case CMPF_EQUAL:
            return D3DCMP_EQUAL;
        case CMPF_NOT_EQUAL:
            return D3DCMP_NOTEQUAL;
        case CMPF_GREATER_EQUAL:
            return D3DCMP_GREATEREQUAL;
        case CMPF_GREATER:
            return D3DCMP_GREATER;
        };
        return 0;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(CullingMode cm, bool flip)
    {
        switch( cm )
        {
        case CM_None:
            return D3DCULL_NONE;
        case CM_Clockwise:
            if( flip )
                return D3DCULL_CCW;
            else
                return D3DCULL_CW;
        case CM_Anticlockwise:
            if( flip )
                return D3DCULL_CW;
            else
                return D3DCULL_CCW;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    D3DFOGMODE D3D9Mappings::get(FogMode fm)
    {
        switch( fm )
        {
        case FOG_EXP:
            return D3DFOG_EXP;
        case FOG_EXP2:
            return D3DFOG_EXP2;
        case FOG_LINEAR:
            return D3DFOG_LINEAR;
        }
        return D3DFOG_FORCE_DWORD;
    }
    //---------------------------------------------------------------------
    D3DFILLMODE D3D9Mappings::get(PolygonMode level)
    {
        switch(level)
        {
        case PM_POINTS:
            return D3DFILL_POINT;
        case PM_WIREFRAME:
            return D3DFILL_WIREFRAME;
        case PM_SOLID:
            return D3DFILL_SOLID;
        }
        return D3DFILL_FORCE_DWORD;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(StencilOpType op, bool invert)
    {
        switch(op)
        {
        case SOP_KEEP:
            return D3DSTENCILOP_KEEP;
        case SOP_ZERO:
            return D3DSTENCILOP_ZERO;
        case SOP_REPLACE:
            return D3DSTENCILOP_REPLACE;
        case SOP_INCREMENT:
            return invert? D3DSTENCILOP_DECRSAT : D3DSTENCILOP_INCRSAT;
        case SOP_DECREMENT:
            return invert? D3DSTENCILOP_INCRSAT : D3DSTENCILOP_DECRSAT;
        case SOP_INCREMENT_WRAP:
            return invert? D3DSTENCILOP_DECR : D3DSTENCILOP_INCR;
        case SOP_DECREMENT_WRAP:
            return invert? D3DSTENCILOP_INCR : D3DSTENCILOP_DECR;
        case SOP_INVERT:
            return D3DSTENCILOP_INVERT;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    D3DSAMPLERSTATETYPE D3D9Mappings::get(FilterType ft)
    {
        switch (ft)
        {
        case FT_MIN:
            return D3DSAMP_MINFILTER;
            break;
        case FT_MAG:
            return D3DSAMP_MAGFILTER;
            break;
        case FT_MIP:
            return D3DSAMP_MIPFILTER;
            break;
        }

        // to keep compiler happy
        return D3DSAMP_MINFILTER;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(FilterType ft, TextureFilterOP fo, const D3DCAPS9& devCaps, 
        eD3DTexType texType)
    {
        // Assume normal
        DWORD capsType = devCaps.TextureFilterCaps;

        switch( texType )
        {
        case D3D_TEX_TYPE_NORMAL:
            capsType = devCaps.TextureFilterCaps;
            break;
        case D3D_TEX_TYPE_CUBE:
            capsType = devCaps.CubeTextureFilterCaps;
            break;
        case D3D_TEX_TYPE_VOLUME:
            capsType = devCaps.VolumeTextureFilterCaps;
            break;
        }

        switch (ft)
        {
        case FT_MIN:
            switch( fo )
            {
                // NOTE: Fall through if device doesn't support requested type
            case FO_ANISOTROPIC:
                if( capsType & D3DPTFILTERCAPS_MINFANISOTROPIC )
                {
                    return D3DTEXF_ANISOTROPIC;
                    break;
                }
            case FO_LINEAR:
                if( capsType & D3DPTFILTERCAPS_MINFLINEAR )
                {
                    return D3DTEXF_LINEAR;
                    break;
                }
            case FO_POINT:
            case FO_NONE:
                return D3DTEXF_POINT;
                break;
            }
            break;
        case FT_MAG:
            switch( fo )
            {
            // NOTE: Fall through if device doesn't support requested type
            case FO_ANISOTROPIC:
                if( capsType & D3DPTFILTERCAPS_MAGFANISOTROPIC )
                {
                    return D3DTEXF_ANISOTROPIC;
                    break;
                }
            case FO_LINEAR:
                if( capsType & D3DPTFILTERCAPS_MAGFLINEAR )
                {
                    return D3DTEXF_LINEAR;
                    break;
                }
            case FO_POINT:
            case FO_NONE:
                return D3DTEXF_POINT;
                break;
            }
            break;
        case FT_MIP:
            switch( fo )
            {
            case FO_ANISOTROPIC:
            case FO_LINEAR:
                if( capsType & D3DPTFILTERCAPS_MIPFLINEAR )
                {
                    return D3DTEXF_LINEAR;
                    break;
                }
            case FO_POINT:
                if( capsType & D3DPTFILTERCAPS_MIPFPOINT )
                {
                    return D3DTEXF_POINT;
                    break;
                }
            case FO_NONE:
                return D3DTEXF_NONE;
                break;
            }
            break;
        }

        // should never get here
        return 0;

    }
    //---------------------------------------------------------------------
    D3D9Mappings::eD3DTexType D3D9Mappings::get(TextureType ogreTexType)
    {
        switch( ogreTexType )
        {
        case TEX_TYPE_1D :
        case TEX_TYPE_2D :
            return D3D9Mappings::D3D_TEX_TYPE_NORMAL;
        case TEX_TYPE_CUBE_MAP :
            return D3D9Mappings::D3D_TEX_TYPE_CUBE;
        case TEX_TYPE_3D :
            return D3D9Mappings::D3D_TEX_TYPE_VOLUME;
        }
        return D3D9Mappings::D3D_TEX_TYPE_NONE;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(Buffer::Usage usage)
    {
        DWORD ret = 0;
        if (usage & Buffer::HBU_DYNAMIC)
        {
#if OGRE_D3D_MANAGE_BUFFERS
            // Only add the dynamic flag for default pool, and
            // we use default pool when buffer is discardable
            if (usage & Buffer::HBU_DISCARDABLE)
                ret |= D3DUSAGE_DYNAMIC;
#else
            ret |= D3DUSAGE_DYNAMIC;
#endif
        }
        if (usage & Buffer::HBU_WRITE_ONLY)
        {
            ret |= D3DUSAGE_WRITEONLY;
        }
        return ret;
    }
    //---------------------------------------------------------------------
    DWORD D3D9Mappings::get(Buffer::LockOptions options, Buffer::Usage usage)
    {
        DWORD ret = 0;
        if (options == Buffer::HBL_DISCARD)
        {
#if OGRE_D3D_MANAGE_BUFFERS
            // Only add the discard flag for dynamic usgae and default pool
            if ((usage & Buffer::HBU_DYNAMIC) &&
                (usage & Buffer::HBU_DISCARDABLE))
                ret |= D3DLOCK_DISCARD;
#else
            // D3D doesn't like discard or no_overwrite on non-dynamic buffers
            if (usage & Buffer::HBU_DYNAMIC)
                ret |= D3DLOCK_DISCARD;
#endif
        }
        if (options == Buffer::HBL_READ_ONLY)
        {
            // D3D debug runtime doesn't like you locking managed buffers readonly
            // when they were created with write-only (even though you CAN read
            // from the software backed version)
            if (!(usage & Buffer::HBU_WRITE_ONLY))
                ret |= D3DLOCK_READONLY;

        }
        if (options == Buffer::HBL_NO_OVERWRITE)
        {
#if OGRE_D3D_MANAGE_BUFFERS
            // Only add the nooverwrite flag for dynamic usgae and default pool
            if ((usage & Buffer::HBU_DYNAMIC) &&
                (usage & Buffer::HBU_DISCARDABLE))
                ret |= D3DLOCK_NOOVERWRITE;
#else
            // D3D doesn't like discard or no_overwrite on non-dynamic buffers
            if (usage & Buffer::HBU_DYNAMIC)
                ret |= D3DLOCK_NOOVERWRITE;
#endif 
        }

        return ret;
    }
    //---------------------------------------------------------------------
    D3DFORMAT D3D9Mappings::get(IndexBuffer::IndexType itype)
    {
        if (itype == IndexBuffer::IT_32BIT)
        {
            return D3DFMT_INDEX32;
        }
        else
        {
            return D3DFMT_INDEX16;
        }
    }
    //---------------------------------------------------------------------
    D3DDECLTYPE D3D9Mappings::get(DataType vType)
    {
        switch (vType)
        {
        case DT_Colour:
        case DT_Colour_ABGR:
        case DT_Colour_ARGB:
            return D3DDECLTYPE_D3DCOLOR;
        case DT_Float1x:
            return D3DDECLTYPE_FLOAT1;
        case VET_FLOAT2:
            return D3DDECLTYPE_FLOAT2;
        case VET_FLOAT3:
            return D3DDECLTYPE_FLOAT3;
        case VET_FLOAT4:
            return D3DDECLTYPE_FLOAT4;
        case DT_Short2x:
            return D3DDECLTYPE_SHORT2;
        case VET_SHORT4:
            return D3DDECLTYPE_SHORT4;
        case DT_UByte4x:
            return D3DDECLTYPE_UBYTE4;
        case VET_UBYTE4_NORM:
            // valid only with vertex shaders >= 2.0
            return D3DDECLTYPE_UBYTE4N;
        case VET_SHORT2_NORM:
            // valid only with vertex shaders >= 2.0
            return D3DDECLTYPE_SHORT2N;
        case VET_SHORT4_NORM:
            // valid only with vertex shaders >= 2.0
            return D3DDECLTYPE_SHORT4N;
        case VET_USHORT2_NORM:
            // valid only with vertex shaders >= 2.0
            return D3DDECLTYPE_USHORT2N;
        case VET_USHORT4_NORM:
            // valid only with vertex shaders >= 2.0
            return D3DDECLTYPE_USHORT4N;
        }
        // to keep compiler happy
        return D3DDECLTYPE_FLOAT3;
    }
    //---------------------------------------------------------------------
    D3DDECLUSAGE D3D9Mappings::get(VertexType sem)
    {
        switch (sem)
        {
        case VT_Matrix_Index:
            return D3DDECLUSAGE_BLENDINDICES;
            break;
        case VT_Matrix_Weight:
            return D3DDECLUSAGE_BLENDWEIGHT;
            break;
        case VT_Diffuse:
            return D3DDECLUSAGE_COLOR; // NB index will differentiate
            break;
        case VT_Specular:
            return D3DDECLUSAGE_COLOR; // NB index will differentiate
            break;
        case VT_Normals:
            return D3DDECLUSAGE_NORMAL;
            break;
        case VT_Pos:
            return D3DDECLUSAGE_POSITION;
            break;
        case VT_Tex_Coord:
            return D3DDECLUSAGE_TEXCOORD;
            break;
        case VT_Binormal:
            return D3DDECLUSAGE_BINORMAL;
            break;
        case VT_Tangent:
            return D3DDECLUSAGE_TANGENT;
            break;
        }
        // to keep compiler happy
        return D3DDECLUSAGE_POSITION;
    }
    //---------------------------------------------------------------------
    D3DXMATRIX D3D9Mappings::makeD3DXMatrix( const Matrix4& mat )
    {
        // Transpose matrix
        // D3D9 uses row vectors i.e. V*M
        // Ogre, OpenGL and everything else uses column vectors i.e. M*V
        return D3DXMATRIX(
            mat[0][0], mat[1][0], mat[2][0], mat[3][0],
            mat[0][1], mat[1][1], mat[2][1], mat[3][1],
            mat[0][2], mat[1][2], mat[2][2], mat[3][2],
            mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
    }
    //---------------------------------------------------------------------
    Matrix4 D3D9Mappings::convertD3DXMatrix( const D3DXMATRIX& mat )
    {
        return Matrix4(
            mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
            mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
            mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
            mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
    }
    /****************************************************************************************/
    PixelFormat D3D9Mappings::_getPF(D3DFORMAT d3dPF)
    {
        switch(d3dPF)
        {
        case D3DFMT_A8:
            return PF_A8UN;
        case D3DFMT_L8:
            return PF_L8ui;
        case D3DFMT_L16:
            return PF_L16ui;
        case D3DFMT_A8L8:
            return PF_BYTE_LA;  // Assume little endian here
        case D3DFMT_R3G3B2:
            return PF_R3G3B2;
        case D3DFMT_A1R5G5B5:
            return PF_A1R5G5B5;
        case D3DFMT_A4R4G4B4:
            return PF_A4R4G4B4;
        case D3DFMT_R5G6B5:
            return PF_R5G6B5;
        case D3DFMT_R8G8B8:
            return PF_R8G8B8;
        case D3DFMT_X8R8G8B8:
            return PF_X8R8G8B8;
        case D3DFMT_A8R8G8B8:
            return PF_A8R8G8B8;
        case D3DFMT_X8B8G8R8:
            return PF_X8B8G8R8;
        case D3DFMT_A8B8G8R8:
            return PF_A8B8G8R8;
        case D3DFMT_A2R10G10B10:
            return PF_A2R10G10B10;
        case D3DFMT_A2B10G10R10:
           return PF_A2B10G10R10;
        case D3DFMT_R16F:
            return PF_R16;
        case D3DFMT_A16B16G16R16F:
            return PF_R16G16B16A16;
        case D3DFMT_R32F:
            return PF_R32;
        case D3DFMT_G32R32F:
            return PF_G32R32;
        case D3DFMT_A32B32G32R32F:
            return PF_R32G32B32A32;
        case D3DFMT_G16R16F:
            return PF_G16R16;
        case D3DFMT_A16B16G16R16:
            return PF_R16G16B16A16ui;
        case D3DFMT_G16R16:
            return PF_G16R16ui;
        case D3DFMT_DXT1:
            return PF_BC1UN;
        //case D3DFMT_DXT2:
        //    return PF_BC2UN;
        case D3DFMT_DXT3:
            return PF_BC2UN;
        //case D3DFMT_DXT4:
        //    return PF_DXT4;
        case D3DFMT_DXT5:
            return PF_BC3UN;
        default:
            return PF_UNKNOWN;
        }
    }
    /****************************************************************************************/
    D3DFORMAT D3D9Mappings::_getPF(PixelFormat ogrePF)
    {
        switch(ogrePF)
        {
        case PF_L8ui:
            return D3DFMT_L8;
        case PF_L16ui:
            return D3DFMT_L16;
        case PF_A8UN:
            return D3DFMT_A8;
        case PF_BYTE_LA:
            return D3DFMT_A8L8; // Assume little endian here
        case PF_R3G3B2:
            return D3DFMT_R3G3B2;
        case PF_A1R5G5B5:
            return D3DFMT_A1R5G5B5;
        case PF_R5G6B5:
            return D3DFMT_R5G6B5;
        case PF_A4R4G4B4:
            return D3DFMT_A4R4G4B4;
        case PF_R8G8B8:
            return D3DFMT_R8G8B8;
        case PF_A8R8G8B8:
            return D3DFMT_A8R8G8B8;
        case PF_A8B8G8R8:
            return D3DFMT_A8B8G8R8;
        case PF_X8R8G8B8:
            return D3DFMT_X8R8G8B8;
        case PF_X8B8G8R8:
            return D3DFMT_X8B8G8R8;
        case PF_A2B10G10R10:
            return D3DFMT_A2B10G10R10;
        case PF_A2R10G10B10:
            return D3DFMT_A2R10G10B10;
        case PF_R16:
            return D3DFMT_R16F;
        case PF_G16R16:
            return D3DFMT_G16R16F;
        case PF_R16G16B16A16:
            return D3DFMT_A16B16G16R16F;
        case PF_R32:
            return D3DFMT_R32F;
        case PF_G32R32:
            return D3DFMT_G32R32F;
        case PF_R32G32B32A32:
            return D3DFMT_A32B32G32R32F;
        case PF_R16G16B16A16ui:
            return D3DFMT_A16B16G16R16;
        case PF_G16R16ui:
            return D3DFMT_G16R16;
        case PF_BC1UN:
            return D3DFMT_DXT1;
        //case PF_BC2UN:
        //    return D3DFMT_DXT2;
        case PF_BC2UN:
            return D3DFMT_DXT3;
        //case PF_DXT4:
        //    return D3DFMT_DXT4;
        case PF_BC3UN:
            return D3DFMT_DXT5;
        case PF_UNKNOWN:
        default:
            return D3DFMT_UNKNOWN;
        }
    }
    /****************************************************************************************/
    PixelFormat D3D9Mappings::_getClosestSupportedPF(PixelFormat ogrePF)
    {
        if (_getPF(ogrePF) != D3DFMT_UNKNOWN)
        {
            return ogrePF;
        }
        switch(ogrePF)
        {
        case PF_B5G6R5:
            return PF_R5G6B5;
        case PF_B8G8R8:
            return PF_R8G8B8;
        case PF_B8G8R8A8:
            return PF_A8R8G8B8;
        case PF_R16G16B16ui:
            return PF_R16G16B16A16ui;
        case PF_R16G16B16UN:
            return PF_R16G16B16A16;
        case PF_R32G32B32:
            return PF_R32G32B32A32;
        case PF_UNKNOWN:
        default:
            return PF_A8R8G8B8;
        }
    }

}
