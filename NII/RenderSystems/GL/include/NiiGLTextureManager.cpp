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

#include "NiiGLTextureManager.h"
#include "NiiGLRenderTexture.h"
#include "NiiGLPixelFormat.h"
#include "NiiGLRenderSystem.h"
#include "NiiEngine.h"

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#endif

namespace NII
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GL3PlusSampler
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLint GL3PlusSampler::getCombinedMinMipFilter(TextureFilterOP min, TextureFilterOP mip)
    {
        switch(min)
        {
        case FO_ANISOTROPIC:
        case FO_LINEAR:
            switch (mip)
            {
            case FO_ANISOTROPIC:
            case FO_LINEAR:
                // linear min, linear mip
                return GL_LINEAR_MIPMAP_LINEAR;
            case FO_POINT:
                // linear min, point mip
                return GL_LINEAR_MIPMAP_NEAREST;
            case FO_NONE:
                // linear min, no mip
                return GL_LINEAR;
            }
            break;
        case FO_POINT:
        case FO_NONE:
            switch (mip)
            {
            case FO_ANISOTROPIC:
            case FO_LINEAR:
                // nearest min, linear mip
                return GL_NEAREST_MIPMAP_LINEAR;
            case FO_POINT:
                // nearest min, point mip
                return GL_NEAREST_MIPMAP_NEAREST;
            case FO_NONE:
                // nearest min, no mip
                return GL_NEAREST;
            }
            break;
        }

        // should never get here
        return 0;
    }
    //-----------------------------------------------------------------------------
    GLint GL3PlusSampler::getTextureAddressingMode(TextureAddressingMode tam)
    {
        switch (tam)
        {
        default:
        case ShaderChTextureUnit::TAM_WRAP:
            return GL_REPEAT;
        case ShaderChTextureUnit::TAM_MIRROR:
            return GL_MIRRORED_REPEAT;
        case ShaderChTextureUnit::TAM_CLAMP:
            return GL_CLAMP_TO_EDGE;
        case ShaderChTextureUnit::TAM_BORDER:
            return GL_CLAMP_TO_BORDER;
        }
    }
    //-----------------------------------------------------------------------------
    GL3PlusSampler::GL3PlusSampler(GL3PlusRenderSystem* rs) : mSamplerId(0)
    {
        glGenSamplers(1, &mSamplerId);
    }
    //-----------------------------------------------------------------------------
    GL3PlusSampler::~GL3PlusSampler()
    {
        glDeleteSamplers(1, &mSamplerId);
    }
    //-----------------------------------------------------------------------------
    void GL3PlusSampler::bind(uint32 unit)
    {
        GLCheck(glBindSampler(unit, mSamplerId));

        if(!mDirty)
            return;

        GLCheck(glSamplerParameteri(mSamplerId, GL_TEXTURE_WRAP_S, getTextureAddressingMode(mAddressMode.u)));
        GLCheck(glSamplerParameteri(mSamplerId, GL_TEXTURE_WRAP_T, getTextureAddressingMode(mAddressMode.v)));
        GLCheck(glSamplerParameteri(mSamplerId, GL_TEXTURE_WRAP_R, getTextureAddressingMode(mAddressMode.w)));

        if (mAddressMode.u == TAM_BORDER || mAddressMode.v == TAM_BORDER || mAddressMode.w == TAM_BORDER)
            GLCheck(glSamplerParameterfv( mSamplerId, GL_TEXTURE_BORDER_COLOR, mBorderColour.ptr()));
        GLCheck(glSamplerParameterf(mSamplerId, GL_TEXTURE_LOD_BIAS, mMipmapBias));

        auto caps = Root::getSingleton().getRenderSystem()->getCapabilities();
        if (caps->hasCapability(RSC_ANISOTROPY))
            GLCheck(glSamplerParameteri(mSamplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min<uint>(caps->getMaxSupportedAnisotropy(), mMaxAniso)));

        GLCheck(
            glSamplerParameteri(mSamplerId, GL_TEXTURE_COMPARE_MODE, mCompareEnabled ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE));
        GLCheck(
            glSamplerParameteri(mSamplerId, GL_TEXTURE_COMPARE_FUNC, GL3PlusRenderSystem::convertCompareFunction(mCompareFunc)));

        // Combine with existing mip filter
        GLCheck(glSamplerParameteri(mSamplerId, GL_TEXTURE_MIN_FILTER, getCombinedMinMipFilter(mMinFilter, mMipFilter)));

        switch (mMagFilter)
        {
        case FO_ANISOTROPIC: // GL treats linear and aniso the same
        case FO_LINEAR:
            GLCheck(glSamplerParameteri(mSamplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            break;
        case FO_POINT:
        case FO_NONE:
            GLCheck(glSamplerParameteri(mSamplerId, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            break;
        }

        mDirty = false;
    }
    //-----------------------------------------------------------------------------
    GLTextureManager::GLTextureManager(GBufferManager * mag, RenderSystem *rsys, const GL3PlusSupport &support) :
        TextureManager( mag, rsys ),
        mSupport( support )
    {
        memset( mBlankTexture, 0, sizeof(mBlankTexture) );
        memset( mTmpFbo, 0, sizeof(mTmpFbo) );

        GLCheck( glGenTextures( Texture::T_3D, &mBlankTexture[1u] ) );
        mBlankTexture[Texture::T_Unknow] = mBlankTexture[Texture::T_2D];

        const GLenum targets[] =
        {
            GL_NONE,
            GL_TEXTURE_1D,
            GL_TEXTURE_1D_ARRAY,
            GL_TEXTURE_2D,
            GL_TEXTURE_2D_ARRAY,
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_CUBE_MAP_ARRAY,
            GL_TEXTURE_3D
        };

        //Must be large enough to hold the biggest transfer we'll do.
        uint8 c_whiteData[4*4*6*4];
        uint8 c_blackData[4*4*6*4];
        memset( c_whiteData, 0xff, sizeof( c_whiteData ) );
        memset( c_blackData, 0x00, sizeof( c_blackData ) );

        for( int i=1; i<=Texture::T_3D; ++i )
        {
            GLCheck( glBindTexture( targets[i], mBlankTexture[i] ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_BASE_LEVEL, 0 ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_MAX_LEVEL, 0 ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE ) );
            GLCheck( glTexParameteri( targets[i], GL_TEXTURE_MAX_LEVEL, 0 ) );

            switch( i )
            {
            case Texture::T_Unknow:
                N_EXCEPT( Exception::ERR_INVALID_STATE, "Ogre should never hit this path");
                break;
            case Texture::T_1D:
                GLCheck( glTexStorage1D( targets[i], 1, GL_RGBA8, 4 ) );
                GLCheck( glTexSubImage1D( targets[i], 0, 0, 4, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, c_whiteData ) );
                break;
            case Texture::T_1DArray:
                GLCheck( glTexStorage2D( targets[i], 1, GL_RGBA8, 4, 1 ) );
                GLCheck( glTexSubImage2D( targets[i], 0, 0, 0, 4, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, c_whiteData ) );
                break;
            case Texture::T_2D:
                GLCheck( glTexStorage2D( targets[i], 1, GL_RGBA8, 4, 4 ) );
                GLCheck( glTexSubImage2D( targets[i], 0, 0, 0, 4, 4, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, c_whiteData ) );
                break;
            case Texture::T_CUBE:
                GLCheck( glTexStorage2D( targets[i], 1, GL_RGBA8, 4, 4 ) );
                for( int j=0; j<6; ++j )
                {
                    GLCheck( glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, 0, 0, 4, 4, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, c_blackData ) );
                }
                break;
            case Texture::T_CUBEArray:
                GLCheck( glTexStorage3D( targets[i], 1, GL_RGBA8, 4, 4, 6 ) );
                GLCheck( glTexSubImage3D( targets[i], 0, 0, 0, 0, 4, 4, 6, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, c_blackData ) );
                break;
            case Texture::T_2DArray:
            case Texture::T_3D:
                GLCheck( glTexStorage3D( targets[i], 1, GL_RGBA8, 4, 4, 4 ) );
                GLCheck( glTexSubImage3D( targets[i], 0, 0, 0, 0, 4, 4, 4, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, c_whiteData ) );
                break;
            }
        }

        GLCheck( glGenFramebuffers( 2u, mTmpFbo ) );
        
        N_Only(ResourceScheme).add(mType, this); 
    }
    //-----------------------------------------------------------------------------
    GLTextureManager::~GLTextureManager()
    {
        destroyAll();

        GLCheck( glDeleteFramebuffers( 2u, mTmpFbo ) );
        memset( mTmpFbo, 0, sizeof(mTmpFbo) );

        GLCheck( glDeleteTextures( Texture::T_3D - 1u, &mBlankTexture[1u] ) );
        memset( mBlankTexture, 0, sizeof(mBlankTexture) );
        
        // unregister with group manager
        N_Only(ResourceScheme).remove(mType);
    }
    //-----------------------------------------------------------------------------
    PixelFormat GL3PlusTextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
    {
        // Adjust requested parameters to capabilities
        const RenderFeature * caps = Root::getSingleton().getRenderSystem()->getCapabilities();

        // Check compressed texture support
        // if a compressed format not supported, revert to PF_A8R8G8B8
        if(PixelUtil::isCompress(format) && !caps->hasCapability( RSC_TEXTURE_COMPRESSION_DXT ))
        {
            return PF_BYTE_RGBA;
        }
        // if floating point textures not supported, revert to PF_A8R8G8B8
        if (PixelUtil::isFloatingPoint(format) && !caps->hasCapability(RSC_TEXTURE_FLOAT))
        {
            return PF_BYTE_RGBA;
        }

        if(GLPixelUtil::getGLInternalFormat(format) == GL_NONE)
        {
            return PF_BYTE_RGBA;
        }

        // Check if this is a valid rendertarget format
        if (usage & TU_RENDERTARGET)
        {
            /// Get closest supported alternative
            /// If mFormat is supported it's returned
            return GL3PlusRTTManager::getSingleton().getSupportedAlternative(format);
        }

        // Supported
        return format;
    }
    //-----------------------------------------------------------------------------
    Resource * GLTextureManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        return N_new GLTexture(rid, gid, ls, rs, mRenderSys);
    }
    //-----------------------------------------------------------------------------
    PixelFormat GLTextureManager::getFormat(Texture::Type type, PixelFormat format, Nmark usage)
    {
        // Adjust requested parameters to capabilities
        const RenderFeature * caps = Engine::getOnly().getRender()->getFeature();

        // Check compressed texture support
        // if a compressed format not supported, revert to PF_A8R8G8B8
        if(PixelUtil::isCompress(format) && !caps->isSupport(GF_Tex_Fromat_DXT))
        {
            return PF_BYTE_RGBA;
        }
        // if floating point textures not supported, revert to PF_A8R8G8B8
        if(PixelUtil::isFloat(format) && !caps->isSupport(GF_Tex_Float))
        {
            return PF_BYTE_RGBA;
        }

        if (GLPixelUtil::getGLInternalFormat(format) == GL_NONE)
        {
            return PF_BYTE_RGBA;
        }

        // Check if this is a valid rendertarget format
        if(usage & Texture::MM_FRAME_RGB)
        {
            // Get closest supported alternative
            // If mFormat is supported it's returned
            return N_Only(GLRTT).getSupportedAlternative(format);
        }

        // Supported
        return format;
    }
    //-----------------------------------------------------------------------------
    bool GLTextureManager::isSupport(Texture::Type type, PixelFormat format, Nmark usage, bool preciseFormatOnly)
    {
        if(format == PF_UNKNOWN)
            return false;

        // Check natively format
        PixelFormat nativeFormat = getFormat(type, format, usage);
        if(preciseFormatOnly && format != nativeFormat)
            return false;

        // Assume non-floating point is supported always
        if(!PixelUtil::isFloat(nativeFormat))
            return true;

        // check for floating point extension
        // note false positives on old harware: 
        // https://www.khronos.org/opengl/wiki/Floating_point_and_mipmapping_and_filtering
        return mRenderSys->checkGpuApi("GL_ARB_texture_float");
    }
    //-----------------------------------------------------------------------------------
    TextureGpu* GLTextureManager::createTextureGpuWindow( GL3PlusContext *context, Window *window )
    {
        //GL3+ needs to set MM_ManualMSAA so that "resolveTexture" in FrameBufferObject
        //can remain empty (because the system will be performing the resolve).
        return N_new GL3PlusTextureGpuWindow( Texture::RT_Disk, mBufferManager,
                                                 "RenderWindow",
                                                 Texture::MM_FRAME|
                                                 Texture::MM_ManualMSAA|
                                                 Texture::MM_Window,
                                                 Texture::T_2D, this, context, window );
    }
    //-----------------------------------------------------------------------------------
    GLuint GLTextureManager::getBlankTextureGlName(Texture::Type type ) const
    {
        return mBlankTexture[type];
    }
    //-----------------------------------------------------------------------------------
    TextureGpu* GLTextureManager::createTextureImpl(RecoverType rtype,
        IdString name, uint32 textureFlags, Texture::Type initialType )
    {
        GLTexture *retVal = 0;
        if( textureFlags & Texture::MM_FRAME )
        {
            retVal = N_new GL3PlusTextureGpuRenderTarget(rtype, mBufferManager, name,
                         textureFlags | Texture::MM_VertFlip,
                         initialType, this );
        }
        else
        {
            retVal = N_new GLTexture(rtype, mBufferManager, name,
                         textureFlags | Texture::MM_VertFlip,
                         initialType, this );
        }

        return retVal;
    }
    //-----------------------------------------------------------------------------------
    MappedTexture* GLTextureManager::createMappedImpl( uint32 width, uint32 height, uint32 depth, uint32 slices,
        PixelFormat pf )
    {
        const uint32 rowAlignment = 4u;
        const size_t sizeBytes = PixelUtil::getSize( width, height, depth, slices, pf, rowAlignment );

        GLGBufferManager * mag = static_cast<GLGBufferManager*>( mBufferManager );
        
        GLMappedTexture * re;
        mag->create(re, PixelUtil::getFamily( pf ), sizeBytes );
        return re;
    }
    //-----------------------------------------------------------------------------------
    void GLTextureManager::destroyMappedImpl( MappedTexture * tex )
    {
        N_assert( dynamic_cast<GLMappedTexture*>( tex ) );

        GLGBufferManager *mag = static_cast<GLGBufferManager*>( mBufferManager );
        mag->destroy( static_cast<GLMappedTexture*>( tex ) );
    }
    //-----------------------------------------------------------------------------------
    TextureAsync* GLTextureManager::createAsyncImpl(uint32 width, uint32 height, uint32 depthOrSlices,
        Texture::Type textureType, PixelFormat familypf )
    {
        GLGBufferManager *mag = static_cast<GLGBufferManager*>( mBufferManager );
        bool supportsGetTextureSubImage = mRenderSys.isVersionNewer( 4, 5 ) || mSupport.checkGpuApi( "GL_ARB_get_texture_sub_image" );
        return N_new GLTextureAsync( width, height, depthOrSlices, textureType, familypf, mag, supportsGetTextureSubImage );
    }
    //-----------------------------------------------------------------------
}