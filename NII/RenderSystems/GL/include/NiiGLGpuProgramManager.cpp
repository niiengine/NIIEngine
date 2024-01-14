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

#include "NiiGLGpuProgramManager.h"
#include "NiiGLGpuProgram.h"
#include "NiiLogManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    GLGpuProgramManager::GLGpuProgramManager()
    {
        // Superclass sets up members

        // Register with resource group manager
        N_Only(ResourceScheme).add(mType, this);
    }
    //-----------------------------------------------------------------------
    GLGpuProgramManager::~GLGpuProgramManager()
    {
        // Unregister with resource group manager
        N_Only(ResourceScheme).remove(mType);
    }
    //-----------------------------------------------------------------------
    bool GLGpuProgramManager::addCreateFunc(ShaderLanguage lang, CreateGpuProgramCallback cb)
    {
        return mProgramMap.insert(ProgramMap::value_type(lang, cb)).second;
    }
    //-----------------------------------------------------------------------
    bool GLGpuProgramManager::removeCreateFunc(ShaderLanguage lang)
    {
        return mProgramMap.erase(lang) != 0;
    }
    //-----------------------------------------------------------------------
    Resource * GLGpuProgramManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, GpuProgram::ShaderType type, ShaderLanguage lang)
    {
        ProgramMap::const_iterator iter = mProgramMap.find(lang);
        if(iter == mProgramMap.end())
        {
            // No factory, this is an unsupported syntax code, probably for another rendersystem
            // Create a basic one, it doesn't matter what it is since it won't be used
            return GpuProgramManager::createImpl(rid, gid, ls, rs, type, lang);
        }
        return (iter->second)(rid, gid, ls, rs, type, lang);
    }
    //-----------------------------------------------------------------------
    const char* GLGpuProgramManager::getToken( PixelFormat pixelFormat ) const
    {
        switch( pixelFormat )
        {
        //UNORM formats
        case PF_R8UN:
        case PF_A8UN:
            return "r8";
        case PF_R16UN:
            return "r16";
        case PF_R8G8UN:
            return "rg8";
        case PF_R16G16UN:
            return "rg16";
        case PF_R8G8B8A8UN:
        case PF_R8G8B8A8UN_SRGB:
        case PF_B8G8R8A8UN:
        case PF_B8G8R8A8UN_SRGB:
        case PF_B8G8R8X8UN:
        case PF_B8G8R8X8UN_SRGB:
            return "rgba8";
        case PF_R10G10B10A2UN:
            return "rgb10_a2";
        case PF_R16G16B16A16UN:
            return "rgba16";

        //SNORM formats
        case PF_R8N:
            return "r8_snorm";
        case PF_R16N:
            return "r16_snorm";
        case PF_R8G8N:
            return "rg8_snorm";
        case PF_R16G16N:
            return "rg16_snorm";
        case PF_R8G8B8A8N:
            return "rgba8_snorm";
        case PF_R16G16B16A16N:
            return "rgba16_snorm";

        //SINT formats
        case PF_R8i:
            return "r8i";
        case PF_R16i:
            return "r16i";
        case PF_R32i:
            return "r32i";
        case PF_R8G8i:
            return "rg8i";
        case PF_R16G16i:
            return "rg16i";
        case PF_R32G32i:
            return "rg32i";
        case PF_R8G8B8A8i:
            return "rgba8i";
        case PF_R16G16B16A16i:
            return "rgba16i";
        case PF_R32G32B32i:
        case PF_R32G32B32A32i:
            return "rgba32i";

        //UINT formats
        case PF_R8ui:
            return "r8ui";
        case PF_R16ui:
            return "r16ui";
        case PF_R32ui:
            return "r32ui";
        case PF_R8G8ui:
            return "rg8ui";
        case PF_R16G16ui:
            return "rg16ui";
        case PF_R32G32ui:
            return "rg32ui";
        case PF_R8G8B8A8ui:
            return "rgba8ui";
        case PF_R10G10B10A2ui:
            return "rgb10_a2ui";
        case PF_R16G16B16A16ui:
            return "rgba16ui";
        case PF_R32G32B32ui:
        case PF_R32G32B32A32ui:
            return "rgba32ui";

        //FLOAT formats
        case PF_R16:
            return "r16f";
        case PF_R32:
            return "r32f";
        case PF_R16G16:
            return "rg16f";
        case PF_R32G32:
            return "rg32f";
        case PF_R11G11B10:
            return "r11f_g11f_b10f";
        case PF_R16G16B16A16:
            return "rgba16f";
        case PF_R32G32B32:
        case PF_R32G32B32A32:
            return "rgba32f";
        default:
            return 0;
        }

        return 0;
    }
    //-------------------------------------------------------------------------
    const char* GLGpuProgramManager::getToken(PixelFormat pixelFormat, uint32 _textureType, bool isMsaa,
        Buffer::OpType access) const
    {
        const bool bIsInteger = PixelUtil::isInteger( pixelFormat );
        Texture::Type textureType = static_cast<Texture::Texture::Type>( _textureType );

        if( !bIsInteger )
        {
            switch( textureType )
            {
            case Texture::Type1D:
                return "image1D";
            case Texture::Type1DArray:
                return "image1DArray";
            case Texture::Type2D:
            case Texture::Unknown:
                return isMsaa ? "image2DMS" : "image2D";
            case Texture::Type2DArray:
                return isMsaa ? "image2DMSArray" : "image2DArray";
            case Texture::T_CUBE:
                return "imageCube";
            case Texture::T_CUBEArray:
                return "imageCubeArray";
            case Texture::Type3D:
                return "image3D";
            }
        }
        else
        {
            const bool bIsSigned = PixelUtil::isSigned( pixelFormat );
            if( !bIsSigned )
            {
                switch( textureType )
                {
                case Texture::Type1D:
                    return "uimage1D";
                case Texture::Type1DArray:
                    return "uimage1DArray";
                case Texture::Type2D:
                case Texture::Unknown:
                    return isMsaa ? "uimage2DMS" : "uimage2D";
                case Texture::Type2DArray:
                    return isMsaa ? "uimage2DMSArray" : "uimage2DArray";
                case Texture::T_CUBE:
                    return "uimageCube";
                case Texture::T_CUBEArray:
                    return "uimageCubeArray";
                case Texture::Type3D:
                    return "uimage3D";
                }
            }
            else
            {
                switch( textureType )
                {
                case Texture::Type1D:
                    return "iimage1D";
                case Texture::Type1DArray:
                    return "iimage1DArray";
                case Texture::Type2D:
                case Texture::Unknown:
                    return isMsaa ? "iimage2DMS" : "iimage2D";
                case Texture::Type2DArray:
                    return isMsaa ? "iimage2DMSArray" : "iimage2DArray";
                case Texture::T_CUBE:
                    return "iimageCube";
                case Texture::T_CUBEArray:
                    return "iimageCubeArray";
                case Texture::Type3D:
                    return "iimage3D";
                }
            }
        }

        return 0;
    }
    //-------------------------------------------------------------------------
}