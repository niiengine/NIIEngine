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

#include "NiiGLTexture.h"
#include "NiiGLPixelFormat.h"
#include "NiiGLBuffer.h"
#include "NiiGLFBORenderTexture.h"
#include "NiiGLRenderSystem.h"

#include "NiiTextureManager.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiImageCodec.h"
#include "NiiStrConv.h"
#include "NiiBitwise.h"

#if N_PLAT == N_PLAT_WIN32
    #define WIN32_LEAN_AND_MEAN
    #if !defined(NOMINMAX) && defined(_MSC_VER)
        #define NOMINMAX // required to stop windows.h messing up std::min
    #endif
    #include <windows.h>
    #include <wingdi.h>
#endif

#define TODO_use_StagingTexture_with_GPU_GPU_visibility 1
#define TODO_sw_fallback 1

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLTexture
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLTexture::GLTexture( RecoverType rtype, GBufferManager * mag, IdString name, uint32 textureFlags,
        Texture::Type type, TextureManager * texmag) :
        TextureGpu( rtype, mag, name, textureFlags, type, texmag ),
        mDisplayTextureName( 0 ),
        mGlTextureTarget( GL_NONE ),
        mFinalTextureName( 0 ),
        mMsaaFramebufferName( 0 )
    {
        _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------
    GLTexture::GLTexture(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, GLRenderSystem* rsys) :
        GLTextureBase(rid, gid, ls, rs),
        mRenderSys(rsys)
    {
        mMark |= MM_MIPMAP_HW;
    }
    //-----------------------------------------------------------------------
    GLTexture::~GLTexture()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        if(mState == LS_LOADED)
        {
            unload();
        }
        else
        {
            unloadImpl();
        }
    }
    //-----------------------------------------------------------------------
    GLenum GLTexture::getGLTextureTarget() const
    {
        switch(mTextureType)
        {
        case Texture::T_1D:
            return GL_TEXTURE_1D;
        case Texture::T_2D:
            return GL_TEXTURE_2D;
        case Texture::T_3D:
            return GL_TEXTURE_3D;
        case Texture::T_CUBE:
            return GL_TEXTURE_CUBE_MAP;
        case Texture::T_2DArray:
            return GL_TEXTURE_2D_ARRAY;
        case Texture::T_2D_RECT:
            return GL_TEXTURE_RECTANGLE;
        default:
            return 0;
        };
        return 0;
    }
    //-----------------------------------------------------------------------
    void GLTexture::createInternalImpl()
    {
        // Convert to nearest power-of-two size if required
        //mWidth = GLPixelUtil::optionalPO2(mWidth);
        //mHeight = GLPixelUtil::optionalPO2(mHeight);
        //mDepth = GLPixelUtil::optionalPO2(mDepth);
        
        // set Buffer::Usage for TU_RENDERTARGET if nothing else specified
        if((mMark & MM_FRAME_RGB) && (mMark & ~MM_FRAME_RGB) == 0)
            mMark |= Buffer::M_DEVREAD & Buffer::M_DEVWRITE;

        // Adjust format if required
        mFormat = TextureManager::getOnly().getFormat(mTextureType, mFormat, mMark);

        if (PixelUtil::isCompress(mFormat) && (mSrcMipmapCount == 0))
            mMipmapCount = 0;
        
        // Check requested number of mipmaps
        NCount maxMips = getMaxMipmaps();
        mSrcMipmapCount = mMipmapCount;
        if(mSrcMipmapCount > maxMips)
            mSrcMipmapCount = maxMips;

        // Generate texture name
        GLCheck(glGenTextures(1, &mTextureID));
        GLenum texTarget = getGLTextureTarget();
        
        // Calculate size for all mip levels of the texture.
        Nui32 width, height, depth;
        
        if ((mWidth * PixelUtil::getUnitSize(mFormat)) & 3) {
            // Standard alignment of 4 is not right for some formats.
            GLCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        }
        
        // Bind texture object to its type, making it the active texture object
        // for that type.
        mRenderSys->getStateManager()->bindGLTexture( texTarget, mTextureID );

        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_BASE_LEVEL, 0);
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_MAX_LEVEL, mSrcMipmapCount);

        // Set some misc default parameters, these can of course be changed later.
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Set up texture swizzling.
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_R, GL_RED);
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
        mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
        
        if (PixelUtil::isGrayscale(mFormat))
        {
            if (PixelUtil::getDataCount(mFormat) == 2)
            {
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_R, GL_RED);
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_G, GL_RED);
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_B, GL_RED);
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
            }
            else
            {
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_R, GL_RED);
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_G, GL_RED);
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_B, GL_RED);
                mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_A, GL_ONE);
            }
        }

        // Allocate internal buffer so that glTexSubImageXD can be used
        // Internal format
        GLenum format = GLPixelUtil::getGLInternalFormat(mFormat, mMark & MM_SRGB);
        GLenum datatype = GLPixelUtil::getGLOriginDataType(mFormat);
        width = mWidth;
        height = mHeight;
        depth = mDepth;

        if (PixelUtil::isCompress(mFormat))
        {
            for (NCount mip = 0; mip <= mSrcMipmapCount; ++mip)
            {
                GLsizei size = static_cast<GLsizei>(PixelUtil::getSize(width, height, depth, mFormat));
                switch(mTextureType)
                {
                case TEX_TYPE_1D:
                    GLCheck(glCompressedTexImage1D(GL_TEXTURE_1D, mip, format, width, 0, size, NULL));
                    break;
                case TEX_TYPE_2D:
                    GLCheck(glCompressedTexImage2D(GL_TEXTURE_2D, mip, format, width, height, 0, size, NULL));
                    break;
                case TEX_TYPE_2D_RECT:
                    GLCheck(glCompressedTexImage2D(GL_TEXTURE_RECTANGLE, mip, format, width, height, 0, size, NULL));
                    break;
                case TEX_TYPE_2D_ARRAY:
                case TEX_TYPE_3D:
                    GLCheck(glCompressedTexImage3D(texTarget, mip, format, width, height, depth, 0, size, NULL));
                    break;
                case TEX_TYPE_CUBE_MAP:
                    for(int face = 0; face < 6; face++) {
                        GLCheck(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, format, width, height, 0, size, NULL));
                    }
                    break;
                default:
                    break;
                };
                if (width > 1)
                    width = width / 2;
                if (height > 1)
                    height = height / 2;
                if (depth > 1 && mTextureType != T_2DArray)
                    depth = depth / 2;
            }
        }
        else
        {
            if (mRenderSys->hasMinGLVersion(4, 2) || mRenderSys->checkGpuApi("GL_ARB_texture_storage"))
            {
                switch(mTextureType)
                {
                case TEX_TYPE_1D:
                    GLCheck(glTexStorage1D(GL_TEXTURE_1D, GLsizei(mSrcMipmapCount+1), format, GLsizei(width)));
                    break;
                case TEX_TYPE_2D:
                case TEX_TYPE_2D_RECT:
                    GLCheck(glTexStorage2D(GL_TEXTURE_2D, GLsizei(mSrcMipmapCount+1), format, GLsizei(width), GLsizei(height)));
                    break;
                case TEX_TYPE_CUBE_MAP:
                    GLCheck(glTexStorage2D(GL_TEXTURE_CUBE_MAP, GLsizei(mSrcMipmapCount+1), format, GLsizei(width), GLsizei(height)));
                    break;
                case TEX_TYPE_2D_ARRAY:
                    GLCheck(glTexStorage3D(GL_TEXTURE_2D_ARRAY, GLsizei(mSrcMipmapCount+1), format, GLsizei(width), GLsizei(height), GLsizei(depth)));
                    break;
                case TEX_TYPE_3D:
                    GLCheck(glTexStorage3D(GL_TEXTURE_3D, GLsizei(mSrcMipmapCount+1), format, GLsizei(width), GLsizei(height), GLsizei(depth)));
                    break;
                case TEX_TYPE_EXTERNAL_OES:
                    N_EXCEPT(RenderingAPI, "Attempt to store texture for unsupported TEX_TYPE_EXTERNAL_OES, should never happen");
                    break;
                }
            }
            else
            {
                GLenum originFormat = GLPixelUtil::getGLOriginFormat(mFormat);

                // Run through this process to pregenerate mipmap pyramid
                for(uint32 mip = 0; mip <= mSrcMipmapCount; mip++)
                {
                    // Normal formats
                    switch(mTextureType)
                    {
                    case TEX_TYPE_1D:
                        GLCheck(glTexImage1D(GL_TEXTURE_1D, mip, format, width, 0, originFormat, datatype, NULL));
                        break;
                    case TEX_TYPE_2D:
                        GLCheck(glTexImage2D(GL_TEXTURE_2D, mip, format, width, height, 0, originFormat, datatype, NULL));
                        break;
                    case TEX_TYPE_2D_RECT:
                        GLCheck(glTexImage2D(GL_TEXTURE_RECTANGLE, mip, format, width, height, 0, originFormat, datatype, NULL));
                        break;
                    case TEX_TYPE_3D:
                    case TEX_TYPE_2D_ARRAY:
                        GLCheck(glTexImage3D(texTarget, mip, format, width, height, depth, 0, originFormat, datatype, NULL));
                        break;
                    case TEX_TYPE_CUBE_MAP:
                        for(int face = 0; face < 6; face++) {
                            GLCheck(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, format, width, height, 0, originFormat, datatype, NULL));
                        }
                        break;
                    case TEX_TYPE_EXTERNAL_OES:
                        N_EXCEPT(RenderingAPI, "Attempt to create mipmaps for unsupported TEX_TYPE_EXTERNAL_OES, should never happen");
                        break;
                    default:
                        break;
                    };

                    if (width > 1)
                    {
                        width = width / 2;
                    }
                    if (height > 1)
                    {
                        height = height / 2;
                    }
                    if (depth > 1 && mTextureType != TEX_TYPE_2D_ARRAY)
                    {
                        depth = depth / 2;
                    }
                }
            }
        }
        // Reset unpack alignment to defaults
        GLCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
        
        _createSurfaceList();
        
        // Generate mipmaps after all texture levels have been loaded
        // This is required for compressed formats such as DXT
        if (PixelUtil::isCompress(mFormat) && (mUsage & MM_MIPMAP_AUTO))
        {
            GLCheck(glGenerateMipmap(texTarget));
        }
        
        // Get final internal format
        mFormat = getBuffer(0)->getFormat();
        
        
        
        /// 
        GLenum format = GL3PlusMappings::get( mPixelFormat );

        if( !isMultiSample() || !isManualMSAA() )
        {
            GLCheck( glGenTextures( 1u, &mFinalTextureName ) );

            mGlTextureTarget = GL3PlusMappings::get( mTextureType, false );

            GLCheck( glBindTexture( mGlTextureTarget, mFinalTextureName ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_BASE_LEVEL, 0 ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_MAX_LEVEL, 0 ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE ) );
            GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_MAX_LEVEL, mNumMipmaps - 1u ) );

            if( mPixelFormat == PFG_A8_UNORM )
            {
                GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_R, GL_ZERO ) );
                GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_G, GL_ZERO ) );
                GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_B, GL_ZERO ) );
                GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_A, GL_RED ) );
            }

            switch( mTextureType )
            {
            case Texture::T_Unknow:
                N_EXCEPT( Exception::ERR_INVALID_STATE, "Texture '" + getName() + "': Ogre should never hit this path");
                break;
            case Texture::T_1D:
                GLCheck( glTexStorage1D( GL_TEXTURE_1D, GLsizei(mNumMipmaps), format, GLsizei(mWidth) ) );
                break;
            case Texture::T_1DArray:
                GLCheck( glTexStorage2D( GL_TEXTURE_1D_ARRAY, GLsizei(mNumMipmaps), format, GLsizei(mWidth), GLsizei(mDepthOrSlices) ) );
                break;
            case Texture::T_2D:
                GLCheck( glTexStorage2D( GL_TEXTURE_2D, GLsizei(mNumMipmaps), format, GLsizei(mWidth), GLsizei(mHeight) ) );
                break;
            case Texture::T_2DArray:
                GLCheck( glTexStorage3D( GL_TEXTURE_2D_ARRAY, GLsizei(mNumMipmaps), format, GLsizei(mWidth), GLsizei(mHeight), GLsizei(mDepthOrSlices) ) );
                break;
            case Texture::T_CUBE:
                GLCheck( glTexStorage2D( GL_TEXTURE_CUBE_MAP, GLsizei(mNumMipmaps), format, GLsizei(mWidth), GLsizei(mHeight) ) );
                break;
            case Texture::T_CUBEArray:
                GLCheck( glTexStorage3D( GL_TEXTURE_CUBE_MAP_ARRAY, GLsizei(mNumMipmaps), format, GLsizei(mWidth), GLsizei(mHeight), GLsizei(mDepthOrSlices) ) );
                break;
            case Texture::T_3D:
                GLCheck( glTexStorage3D( GL_TEXTURE_3D, GLsizei(mNumMipmaps), format, GLsizei(mWidth), GLsizei(mHeight), GLsizei(mDepthOrSlices) ) );
                break;
            }

            //Allocate internal buffers for automipmaps before we load anything into them
            if( isAutoMipmap() )
                GLCheck( glGenerateMipmap( mGlTextureTarget ) );

            //Set debug name for RenderDoc and similar tools
            GlObjectLabel( GL_TEXTURE, mFinalTextureName, getName() );
        }

        if( isMultiSample() )
        {
            //const GLboolean fixedsamplelocations = mMsaaPattern != CoordType::CT_Unknow;
            //RENDERBUFFERS have fixedsamplelocations implicitly set to true. Be consistent
            //with non-texture depth buffers.
            const GLboolean fixedsamplelocations = GL_TRUE;

            if( !isTexture() || (!isManualMSAA() && !PixelUtil::isDepth( mPixelFormat )) )
            {
                GLCheck( glGenRenderbuffers( 1, &mMsaaFramebufferName ) );
                GLCheck( glBindRenderbuffer( GL_RENDERBUFFER, mMsaaFramebufferName ) );
                GLCheck( glRenderbufferStorageMultisample( GL_RENDERBUFFER, mSrcSample.getMultiSampling(), format, GLsizei(mWidth), GLsizei(mHeight) ) );
                GLCheck( glBindRenderbuffer( GL_RENDERBUFFER, 0 ) );

                //Set debug name for RenderDoc and similar tools
                GlObjectLabel( GL_RENDERBUFFER, mMsaaFramebufferName, getName() + "/MsaaImplicit" );
            }
            else
            {
                GLCheck( glGenTextures( 1u, &mFinalTextureName ) );

                N_assert( mTextureType == Texture::T_2D || mTextureType == Texture::T_2DArray );
                mGlTextureTarget = mTextureType == Texture::T_2D ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

                GLCheck( glBindTexture( mGlTextureTarget, mFinalTextureName ) );
                GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_BASE_LEVEL, 0 ) );
                GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_MAX_LEVEL, 0 ) );
                GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_MAX_LEVEL, mNumMipmaps - 1u ) );

                if( mPixelFormat == PFG_A8_UNORM )
                {
                    GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_R, GL_ZERO ) );
                    GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_G, GL_ZERO ) );
                    GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_B, GL_ZERO ) );
                    GLCheck( glTexParameteri( mGlTextureTarget, GL_TEXTURE_SWIZZLE_A, GL_RED ) );
                }

                if( mTextureType == Texture::T_2D )
                {
                    glTexImage2DMultisample( mGlTextureTarget, mSrcSample.getMultiSampling(),
                        format, GLsizei( mWidth ), GLsizei( mHeight ), fixedsamplelocations );
                }
                else
                {
                    glTexImage3DMultisample( mGlTextureTarget, mSrcSample.getMultiSampling(),
                        format, GLsizei( mWidth ), GLsizei( mHeight ), GLsizei( mDepthOrSlices ), fixedsamplelocations );
                }

                //Set debug name for RenderDoc and similar tools
                GlObjectLabel( GL_TEXTURE, mFinalTextureName, getName() );
            }
        }
    }
    //-------------------------------------------------------------------------
    void GLTexture::freeInternalImpl()
    {
        if (GLStateCacheManager* stateCacheManager = mRenderSys->getStateManager())
        {
            GLCheck(glDeleteTextures(1, &mTextureID));
            stateCacheManager->invalidateStateForTexture(mTextureID);
        }
        
        if( !isPoolSlice() )
        {
            if( mFinalTextureName )
            {
                glDeleteTextures( 1, &mFinalTextureName );
                mFinalTextureName = 0;
            }
            if( mMsaaFramebufferName )
            {
                if( isMultiSample() && !isManualMSAA() )
                    glDeleteRenderbuffers( 1, &mMsaaFramebufferName );
                else
                    glDeleteTextures( 1, &mMsaaFramebufferName );
                mMsaaFramebufferName = 0;
            }
        }
        else
        {
            if( mPool )
            {
                mTextureManager->poolFree( this );
            }

            mFinalTextureName = 0;
            mMsaaFramebufferName = 0;
        }

        _setToDisplayDummyTexture();
    }
    //-------------------------------------------------------------------------
    TextureBuffer * GLTexture::createViewImpl(PixelFormat pf)
    {
        return 0;
    }
    //-------------------------------------------------------------------------
    void GLTexture::_createSurfaceList()
    {
        FrameBufferList::iterator j, jend = mSurfaceList.end();
        for (j = mSurfaceList.begin(); j != jend; ++j)
        {
            N_delete *j;
        }
        mSurfaceList.clear();

        Nui32 depth = mDepth;
        GLint facecnt = getUnitCount();
        NCount minmapcnt = getMipmapCount();
        // For all faces and mipmaps, store surfaces as HardwarePixelBufferSharedPtr
        for (GLint face = 0; face < facecnt; ++face)
        {
            Nui32 width = mWidth;
            Nui32 height = mHeight;

            for (NCount mip = 0; mip <= minmapcnt; ++mip)
            {
                GLFrameBuffer * buf = N_new GLTextureBuffer1(mRenderSys, this, face, mip, width, height, depth);
                mSurfaceList.push_back(buf);

                if (width > 1)
                    width = width / 2;
                if (height > 1)
                    height = height / 2;
                if (depth > 1 && mTextureType != T_2DArray)
                    depth = depth / 2;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::notifyGpuData(void)
    {
        N_assert( mCurrentStorage == StorageType::ST_Gpu );
        N_assert( mFinalTextureName || mPixelFormat == PFG_NULL );

        mDisplayTextureName = mFinalTextureName;

        process( Texture::Process::O_Valid );
    }
    //-----------------------------------------------------------------------------------
    bool GLTexture::isValidImpl(void) const
    {
        return mDisplayTextureName == mFinalTextureName;
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::_setToDisplayDummyTexture(void)
    {
        if( !mTextureManager )
        {
            N_assert( isWindow() );
            return; //This can happen if we're a window and we're on shutdown
        }

        GL3PlusTextureGpuManager * textureManagerGl = static_cast<GL3PlusTextureGpuManager*>( mTextureManager );
        if( isPoolSlice() )
        {
            mDisplayTextureName = textureManagerGl->getBlankTextureGlName( Texture::T_2DArray );
            mGlTextureTarget = GL_TEXTURE_2D_ARRAY;
        }
        else
        {
            mDisplayTextureName = textureManagerGl->getBlankTextureGlName( mTextureType );
            mGlTextureTarget = GL3PlusMappings::get( mTextureType, false );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::poolImpl( const TexturePool *newPool, uint16 slice )
    {
        TextureGpu::poolImpl( newPool, slice );

        _setToDisplayDummyTexture();

        mGlTextureTarget = GL_TEXTURE_2D_ARRAY;

        if( mPool )
        {
            N_assert( dynamic_cast<GLTexture*>( mPool->mPool ) );
            GLTexture *tex = static_cast<GLTexture*>(mPool->mPool);
            mFinalTextureName = tex->mFinalTextureName;
        }

        process( Texture::Process::O_PoolAlter );
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::setTextureType(Texture::Type type )
    {
        const Texture::Type oldType = mTextureType;
        TextureGpu::setTextureType( type );

        if( oldType != mTextureType && mDisplayTextureName != mFinalTextureName )
            _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    bool GLTexture::isRenderbuffer(void) const
    {
        const bool isDepth = PixelUtil::isDepth( mPixelFormat );

        return  ( isMultiSample() && ((!isManualMSAA() && !isDepth) || !isTexture()) ) ||
                ( isDepth && !isTexture() ) || isWindow();
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::bindTextureToFrameBuffer(GLenum target, uint8 mipLevel, uint32 depthOrSlice )
    {
        GLuint textureName = mFinalTextureName;
        bool bindMsaaColourRenderbuffer = isMultiSample() && ( !isManualMSAA() || !isTexture() );
        if( bindMsaaColourRenderbuffer )
            textureName = mMsaaFramebufferName;
        bindTextureToFrameBuffer( target, textureName, mipLevel, depthOrSlice, bindMsaaColourRenderbuffer );
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::bindTextureToFrameBuffer(GLenum target, GLuint textureName, uint8 mipLevel,
                                                      uint32 depthOrSlice, bool bindMsaaColourRenderbuffer )
    {
        N_assert( !isWindow() );

        const bool isDepth = PixelUtil::isDepth( mPixelFormat );

        if( isDepth )
        {
            if( isTexture() )
            {
                GLCheck( glFramebufferTexture( target, GL_DEPTH_ATTACHMENT, textureName, 0 ) );
                if( PixelUtil::isStencil( mPixelFormat ) )
                {
                    GLCheck( glFramebufferTexture( target, GL_STENCIL_ATTACHMENT, textureName, 0 ) );
                }
            }
            else
            {
                GLCheck( glFramebufferRenderbuffer( target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, textureName ) );
                if( PixelUtil::isStencil( mPixelFormat ) )
                {
                    GLCheck( glFramebufferRenderbuffer( target, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, textureName ) );
                }
            }
        }
        else
        {
            if( bindMsaaColourRenderbuffer )
            {
                GLCheck( glFramebufferRenderbuffer( target, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, textureName ) );
            }
            else
            {
                const bool hasLayers = mTextureType != Texture::T_1D && mTextureType != Texture::T_2D;
                if( !hasLayers )
                {
                    GLCheck( glFramebufferTexture( target, GL_COLOR_ATTACHMENT0, textureName, mipLevel ) );
                }
                else
                {
                    GLCheck( glFramebufferTextureLayer( target, GL_COLOR_ATTACHMENT0, textureName, mipLevel, depthOrSlice ) );
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::copyViaFramebuffer( TextureGpu *dst, const PixelBlock &dstBox,
                                                uint8 dstMipLevel, const PixelBlock &srcBox,
                                                uint8 srcMipLevel, bool keepResolvedTexSynced )
    {
        RenderSystem * renderSystem = mTextureManager->getRenderSystem();
        renderSystem->endFBO();

        GL3PlusTextureGpuManager *textureManagerGl = static_cast<GL3PlusTextureGpuManager*>( mTextureManager );

        N_assert( dynamic_cast<GLTexture *>( dst ) );
        GLTexture *dstGl = static_cast<GLTexture*>( dst );

        const bool srcIsFboAble = this->isRenderbuffer() || this->isFrame();
        const bool dstIsFboAble = dstGl->isRenderbuffer() || dst->isFrame();

        if( !this->isWindow() && srcIsFboAble )
        {
            GLCheck( glBindFramebuffer( GL_READ_FRAMEBUFFER, textureManagerGl->getTemporaryFbo(0) ) );
        }
        else
        {
            GLCheck( glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 ) );
        }

        if( !dst->isWindow() && dstIsFboAble )
        {
            GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, textureManagerGl->getTemporaryFbo(1) ) );
        }
        else
        {
            GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );
        }
        GLCheck( glViewport( 0, 0, dstBox.width, dstBox.height ) );

        size_t depthOrSlices = srcBox.getVolume();
        for( size_t i=0; i<depthOrSlices; ++i )
        {
            if( srcIsFboAble )
            {
                if( !this->isWindow() )
                {
                    this->bindTextureToFrameBuffer( GL_READ_FRAMEBUFFER, srcMipLevel, srcBox.getVolumeIdex() + i );
                    GLCheck( glReadBuffer( GL_COLOR_ATTACHMENT0 ) );
                }
                else
                {
                    GLCheck( glReadBuffer( GL_BACK ) );
                }
            }
            if( dstIsFboAble )
            {
                if( !dstGl->isWindow() )
                {
                    dstGl->bindTextureToFrameBuffer( GL_DRAW_FRAMEBUFFER, dstMipLevel, dstBox.getVolumeIdex() + i );
                    GLCheck( glDrawBuffer( GL_COLOR_ATTACHMENT0 ) );
                }
                else
                {
                    GLCheck( glDrawBuffer( GL_BACK ) );
                }
            }

            if( srcIsFboAble && !dstIsFboAble )
            {
                //We can use glCopyTexImageXD
                //https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCopyTexImage2D.xhtml
                GLenum texTarget = dstGl->mGlTextureTarget;
                if( dst->getTextureType() == Texture::T_CUBE )
                    texTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + dstBox.mArrayIdx;

                switch( dst->getTextureType() )
                {
                case Texture::T_1D:
                    GLCheck( glCopyTexSubImage1D( GL_TEXTURE_1D, dstMipLevel, dstBox.x, srcBox.x, srcBox.y, srcBox.width ) );
                    break;
                case Texture::T_1DArray:
                    GLCheck( glCopyTexSubImage2D( GL_TEXTURE_1D_ARRAY, dstMipLevel,
                                               dstBox.x, dstBox.mArrayIdx,
                                               srcBox.x, srcBox.mArrayIdx,
                                               srcBox.width, srcBox.height ) );
                    break;
                case Texture::T_Unknow:
                case Texture::T_2D:
                case Texture::T_CUBE:
                    GLCheck( glCopyTexSubImage2D( texTarget, dstMipLevel, dstBox.x, dstBox.y, srcBox.x, srcBox.y, srcBox.width, srcBox.height ) );
                    break;
                case Texture::T_2DArray:
                case Texture::T_CUBEArray:
                case Texture::T_3D:
                    GLCheck( glCopyTexSubImage3D( texTarget, dstMipLevel, dstBox.x, dstBox.y, dstBox.getVolumeIdex(),
                                               srcBox.x, srcBox.y, srcBox.width, srcBox.height ) );
                    break;
                }
            }
            else if( srcIsFboAble && dstIsFboAble )
            {
                GLenum readStatus, drawStatus;
                GLCheck( readStatus = glCheckFramebufferStatus( GL_READ_FRAMEBUFFER ) );
                GLCheck( drawStatus = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) );

                const bool supported = readStatus == GL_FRAMEBUFFER_COMPLETE && drawStatus == GL_FRAMEBUFFER_COMPLETE;

                if( supported )
                {
                    GLbitfield bufferBits = 0;
                    if( PixelUtil::isDepth( this->mPixelFormat ) )
                        bufferBits |= GL_DEPTH_BUFFER_BIT;
                    if( PixelUtil::isStencil( this->mPixelFormat ) )
                        bufferBits |= GL_STENCIL_BUFFER_BIT;
                    if( !bufferBits )
                        bufferBits = GL_COLOR_BUFFER_BIT;

                    GLint srcX0 = srcBox.x;
                    GLint srcX1 = srcBox.x + srcBox.width;;
                    GLint srcY0;
                    GLint srcY1;
                    if( this->isWindow() )
                    {
                        srcY0 = this->mHeight - srcBox.y;
                        srcY1 = this->mHeight - srcBox.y - srcBox.height;
                    }
                    else
                    {
                        srcY0 = srcBox.y;
                        srcY1 = srcBox.y + srcBox.height;
                    }
                    GLint dstX0 = dstBox.x;
                    GLint dstX1 = dstBox.x + dstBox.width;
                    GLint dstY0;
                    GLint dstY1;
                    if( dst->isWindow() )
                    {
                        dstY0 = dstGl->mHeight - dstBox.y;
                        dstY1 = dstGl->mHeight - dstBox.y - dstBox.height;
                    }
                    else
                    {
                        dstY0 = dstBox.y;
                        dstY1 = dstBox.y + dstBox.height;
                    }

                    GLCheck( glBlitFramebuffer( srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, bufferBits, GL_NEAREST ) );
                }
                else
                {
                    static bool alreadyWarned = false;
                    if( !alreadyWarned )
                    {
                        LogManager::getSingleton().logMessage(
                                    "Unsupported FBO in GLTexture::read. Falling back to "
                                    "software copy. This is slow. This message will only appear once.",
                                    LML_CRITICAL );
                        alreadyWarned = true;
                    }

                    N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Unsupported FBO in GLTexture::read. Fallback to SW copy not implemented" );
                    TODO_sw_fallback;
                }
            }
            else
            {
                N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "We should have never reached this path. Contact Ogre3D developers.");
            }
        }

        if( srcIsFboAble )
        {
            if( !this->isWindow() )
            {
                GLCheck( glFramebufferRenderbuffer( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0 ) );
                GLCheck( glFramebufferRenderbuffer( GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0 ) );
                GLCheck( glFramebufferRenderbuffer( GL_READ_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0 ) );
            }
            GLCheck( glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 ) );
        }
        if( dstIsFboAble )
        {
            if( !this->isWindow() )
            {
                GLCheck( glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0 ) );
                GLCheck( glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0 ) );
                GLCheck( glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0 ) );
            }
            GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );
        }

        if( dstGl->isMultiSample() && !dstGl->isManualMSAA() && keepResolvedTexSynced )
        {
            GLCheck( glBindFramebuffer( GL_READ_FRAMEBUFFER, textureManagerGl->getTemporaryFbo( 0 ) ) );
            GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, textureManagerGl->getTemporaryFbo( 1 ) ) );
            GLCheck( glViewport( 0, 0, srcBox.width, srcBox.height ) );

            GLCheck( glReadBuffer( GL_COLOR_ATTACHMENT0 ) );
            GLCheck( glDrawBuffer( GL_COLOR_ATTACHMENT0 ) );

            for( size_t i = 0; i < dstBox.mArray; ++i )
            {
                dstGl->bindTextureToFrameBuffer( GL_READ_FRAMEBUFFER, dstGl->mMsaaFramebufferName, 0, dstBox.getVolumeIdex() + i, true );
                dstGl->bindTextureToFrameBuffer( GL_DRAW_FRAMEBUFFER, dstGl->mFinalTextureName, dstMipLevel, dstBox.getVolumeIdex() + i, false );

                GLCheck( glBlitFramebuffer( 0, 0, srcBox.width, srcBox.height, 0, 0, srcBox.width, srcBox.height, GL_COLOR_BUFFER_BIT, GL_NEAREST ) );
            }

            GLCheck( glFramebufferRenderbuffer( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0 ) );
            GLCheck( glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0 ) );
            GLCheck( glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 ) );
            GLCheck( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );
        }
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::read( TextureGpu *dst, const PixelBlock &dstBox, uint8 dstMipLevel,
                                    const PixelBlock &srcBox, uint8 srcMipLevel,
                                    bool keepResolvedTexSynced )
    {
        TextureGpu::read( dst, dstBox, dstMipLevel, srcBox, srcMipLevel );

        N_assert( dynamic_cast<GLTexture*>( dst ) );

        GLTexture *dstGl = static_cast<GLTexture*>( dst );
        GL3PlusTextureGpuManager *textureManagerGl = static_cast<GL3PlusTextureGpuManager*>( mTextureManager );
        const GL3PlusSupport &support = textureManagerGl->getGlSupport();

        if( !this->isWindow() && !dst->isWindow() &&
            ( !this->isMultiSample() || !dst->isMultiSample() ||
              ( this->isManualMSAA() && dst->isManualMSAA() ) ) )
        {
            if( support.hasMinGLVersion( 4, 3 ) || support.checkGpuApi( "GL_ARB_copy_image" ) )
            {
                GLCheck( glCopyImageSubData( this->mFinalTextureName, this->mGlTextureTarget,
                                          srcMipLevel, srcBox.x, srcBox.y,
                                          srcBox.getVolumeIdex() + this->getPoolArray(),
                                          dstGl->mFinalTextureName, dstGl->mGlTextureTarget,
                                          dstMipLevel, dstBox.x, dstBox.y,
                                          dstBox.getVolumeIdex() + dstGl->getPoolArray(),
                                          srcBox.width, srcBox.height, srcBox.getVolume() ) );
            }
            /*TODO
            else if( support.checkGpuApi( "GL_NV_copy_image" ) )
            {
                GLCheck( glCopyImageSubDataNV( this->mFinalTextureName, this->mGlTextureTarget,
                                            srcMipLevel, srcBox.x, srcBox.y, srcBox.z,
                                            dstGl->mFinalTextureName, dstGl->mGlTextureTarget,
                                            dstMipLevel, dstBox.x, dstBox.y, dstBox.z,
                                            srcBox.width, srcBox.height, srcBox.getVolume() ) );
            }*/
            /*TODO: These are for OpenGL ES 3.0+
            else if( support.checkGpuApi( "GL_OES_copy_image" ) )
            {
                GLCheck( glCopyImageSubDataOES( this->mFinalTextureName, this->mGlTextureTarget,
                                             srcMipLevel, srcBox.x, srcBox.y, srcBox.z,
                                             dstGl->mFinalTextureName, dstGl->mGlTextureTarget,
                                             dstMipLevel, dstBox.x, dstBox.y, dstBox.z,
                                             srcBox.width, srcBox.height, srcBox.getVolume() ) );
            }
            else if( support.checkGpuApi( "GL_EXT_copy_image" ) )
            {
                GLCheck( glCopyImageSubDataEXT( this->mFinalTextureName, this->mGlTextureTarget,
                                             srcMipLevel, srcBox.x, srcBox.y, srcBox.z,
                                             dstGl->mFinalTextureName, dstGl->mGlTextureTarget,
                                             dstMipLevel, dstBox.x, dstBox.y, dstBox.z,
                                             srcBox.width, srcBox.height, srcBox.getVolume() ) );
            }*/
            else
            {
//                GLenum format, type;
//                GL3PlusMappings::getFormatAndType( mPixelFormat, format, type );
//                glGetTexImage( this->mFinalTextureName, srcMipLevel, format, type,  );
                //glGetCompressedTexImage
                TODO_use_StagingTexture_with_GPU_GPU_visibility;
                N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "");
            }
        }
        else
        {
            this->copyViaFramebuffer( dst, dstBox, dstMipLevel, srcBox, srcMipLevel, keepResolvedTexSynced );
        }

        //Do not perform the sync if notifyGpuData hasn't been called yet (i.e. we're
        //still building the HW mipmaps, and the texture will never be ready)
        if( dst->isValidImpl() &&
            dst->getRecoverType() == Texture::RT_CpuGpu )
        {
            dst->downloadImpl();
        }
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::mipmap(void)
    {
        if( !mFinalTextureName )
            return;

        const GLenum texTarget = getGlTextureTarget();
        GLCheck( glBindTexture( texTarget, mFinalTextureName ) );
        GLCheck( glGenerateMipmap( texTarget ) );
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::getMultiSampleCoord(Vector2List & locations )
    {
        locations.reserve( mSrcSample.getMultiSampling() );
        if( mSrcSample.getMultiSampling() <= 1u )
        {
            locations.push_back( Vector2( 0.0f, 0.0f ) );
        }
        else
        {
            N_assert( mSrcSample.getType() != CoordType::CT_Unknow );

            float vals[2];
            for( int i=0; i<mSrcSample.getMultiSampling(); ++i )
            {
                glGetMultisamplefv( GL_SAMPLE_POSITION, i, vals );
                locations.push_back( Vector2( vals[0], vals[1] ) * 2.0f - 1.0f );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void GLTexture::getPlatformData( IdString name, void *pData )
    {
        if( name == msFinalTextureBuffer )
            *static_cast<GLuint*>(pData) = mFinalTextureName;
        else if( name == msMsaaTextureBuffer )
            *static_cast<GLuint*>(pData) = mMsaaFramebufferName;
    }
    //-----------------------------------------------------------------------------------
    void GL3PlusTexture::createShaderAccessPoint(uint bindPoint, TextureAccess access, int mipmapLevel, int textureArrayIndex, 
        PixelFormat format)
    {
        GLenum GlAccess = 0;

        switch (access)
        {
        case TA_READ:
            GlAccess = GL_READ_ONLY;
            break;
        case TA_WRITE:
            GlAccess = GL_WRITE_ONLY;
            break;
        case TA_READ_WRITE:
            GlAccess = GL_READ_WRITE;
            break;
        }

        if (format == PF_UNKNOWN) format = mFormat;
        GLenum GlFormat = GLPixelUtil::getClosestGLImageInternalFormat(format);
        GLboolean isArrayTexture = mTextureType == TEX_TYPE_2D_ARRAY;

        // TODO
        // * add memory barrier
        // * material script access (can have multiple instances for a single texture_unit)
        //     shader_access <binding point> [<access>] [<mipmap level>] [<texture array layer>] [<format>]
        //     shader_access 2 read_write 0 0 PF_UINT32_R
        //   binding point - location to bind for shader access; for OpenGL this must be unique and is not related to texture binding point
        //   access - give the shader read, write, or read_write privileges [default read_write]
        //   mipmap level - texture mipmap level to use [default 0]
        //   texture array layer - layer of texture array to use: 'all', or layer number (if not layered, just use 0) [default 0]
        //   format - texture format to be read in shader; for OpenGL this may be different than bound texture format - not sure about DX11 [default same format as texture]
        //   Note that for OpenGL the shader access (image) binding point 
        //   must be specified, it is NOT the same as the texture binding point,
        //   and it must be unique among textures in this pass.
        // * enforce binding point uniqueness by checking against 
        //   image binding point allocation list in GL3PlusTextureManager
        // * generalize for other render systems by introducing vitual method in Texture 
        // for (image in mImages)
        // {
        // GLCheck(
        //     glBindImageTexture(
        //         mImageBind, mTextureID, 
        //         mMipmapLevel, 
        //         mLayered.find('all') != str::npos ? GL_TRUE : GL_FALSE, mLayer,
        //         mImageAccess (READ, WRITE, READ_WRITE), 
        //         toImageFormat(mFormatInShader))); //GL_RGBA8)); //GL_R32UI)); GL_READ_WRITE
        if (mRenderSys->hasMinGLVersion(4, 2) || mRenderSys->checkGpuApi("GL_ARB_shader_image_load_store"))
        {
            GLCheck(glBindImageTexture(bindPoint, mTextureID, mipmapLevel, isArrayTexture, textureArrayIndex, GlAccess, GlFormat));
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    GL3PlusTextureGpuRenderTarget::GL3PlusTextureGpuRenderTarget(
        RecoverType rtype,
        GBufferManager *vaoManager, IdString name, uint32 textureFlags,
        Texture::Texture::Type initialType,
        TextureManager *textureManager ) :
        GLTexture( rtype, vaoManager, name, textureFlags, initialType, textureManager ),
        mDSPool( 1u ),
        mDSTexture( false ),
        mDSFormat( PF_UNKNOWN )
    {
        if( mPixelFormat == PFG_NULL )
            mDSPool = 0;
    }
    //-----------------------------------------------------------------------------------
    void GL3PlusTextureGpuRenderTarget::createInternalImpl(void)
    {
        if( mPixelFormat == PFG_NULL )
            return; //Nothing to do

        if( isTexture() || !PixelUtil::isDepth( mPixelFormat ) )
        {
            GLTexture::createInternalImpl();
        }
        else
        {
            GLCheck( glGenRenderbuffers( 1, &mFinalTextureName ) );
            GLCheck( glBindRenderbuffer( GL_RENDERBUFFER, mFinalTextureName ) );

            GLenum format = GL3PlusMappings::get( mPixelFormat );

            if( !isMultiSample() )
            {
                GLCheck( glRenderbufferStorage( GL_RENDERBUFFER, format, GLsizei(mWidth), GLsizei(mHeight) ) );
            }
            else
            {
                GLCheck( glRenderbufferStorageMultisample(GL_RENDERBUFFER, GLsizei( mSrcSample.getMultiSampling() ), format,
                    GLsizei( mWidth ), GLsizei( mHeight ) ) );
            }

            //Set debug name for RenderDoc and similar tools
            GlObjectLabel( GL_RENDERBUFFER, mFinalTextureName, getName() );
        }
    }
    //-----------------------------------------------------------------------------------
    void GL3PlusTextureGpuRenderTarget::freeInternalImpl(void)
    {
        if( isTexture() || !PixelUtil::isDepth( mPixelFormat ) )
        {
            GLTexture::freeInternalImpl();
            return;
        }

        if( mFinalTextureName )
        {
            glDeleteRenderbuffers( 1, &mFinalTextureName );
            mFinalTextureName = 0;
        }
        _setToDisplayDummyTexture();
    }
    //-----------------------------------------------------------------------------------
    void GL3PlusTextureGpuRenderTarget::_setDepthBufferDefaults(
        uint16 depthBufferPoolId, bool preferDepthTexture, PixelFormat desiredDepthBufferFormat )
    {
        N_assert( isFrame() );
        mDSPool = depthBufferPoolId;
        mDSTexture = preferDepthTexture;
        mDSFormat = desiredDepthBufferFormat;
    }
    //-----------------------------------------------------------------------------------
    uint16 GL3PlusTextureGpuRenderTarget::getDepthBufferPoolId(void) const
    {
        return mDSPool;
    }
    //-----------------------------------------------------------------------------------
    bool GL3PlusTextureGpuRenderTarget::getPreferDepthTexture(void) const
    {
        return mDSTexture;
    }
    //-----------------------------------------------------------------------------------
    PixelFormat GL3PlusTextureGpuRenderTarget::getDesiredDepthBufferFormat(void) const
    {
        return mDSFormat;
    }
    //-----------------------------------------------------------------------------------
}