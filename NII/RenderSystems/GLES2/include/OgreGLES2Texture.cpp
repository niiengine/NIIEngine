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

#include "OgreGLES2Texture.h"
#include "OgreGLES2PixelFormat.h"
#include "OgreGLES2HardwarePixelBuffer.h"
#include "OgreGLES2StateCacheManager.h"
#include "OgreRoot.h"
#include "OgreBitwise.h"
#include "OgreTextureManager.h"

namespace Ogre {
    
    static inline void doImageIO(const String &name, const String &group,
                                 const String &ext,
                                 vector<Image>::type &images,
                                 Texture *tex)
    {
        size_t imgIdx = images.size();
        images.push_back(Image());

        DataStreamPtr dstream =
            ResourceGroupManager::getSingleton().openResource(
                name, group, true, tex);

        images[imgIdx].load(dstream, ext);

        if(!Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES) ||
           (Root::getSingleton().getRenderSystem()->getCapabilities()->getNonPOW2TexturesLimited() && tex->getMipmapCount() > 0))
        {
            size_t w = 0, h = 0;
            
            // Scale to nearest power of 2
            w = GLES2PixelUtil::optionalPO2(images[imgIdx].getWidth());
            h = GLES2PixelUtil::optionalPO2(images[imgIdx].getHeight());
            if((images[imgIdx].getWidth() != w) || (images[imgIdx].getHeight() != h))
                images[imgIdx].resize(w, h);
        }
    }

    GLES2Texture::GLES2Texture(ResourceManager* creator, const String& name,
                             ResourceHandle handle, const String& group, bool isManual,
                             ManualResourceLoader* loader, GLES2Support& support)
        : Texture(creator, name, handle, group, isManual, loader),
          mTextureID(0),
          mGLSupport(support)
    {
    }

    GLES2Texture::~GLES2Texture()
    {
        // have to call this here rather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        if (isLoaded())
        {
            unload();
        }
        else
        {
            freeInternalResources();
        }
    }

    GLenum GLES2Texture::getGLES2TextureTarget(void) const
    {
        switch(mTextureType)
        {
            case TEX_TYPE_1D:
            case TEX_TYPE_2D:
                return GL_TEXTURE_2D;
            case TEX_TYPE_CUBE_MAP:
                return GL_TEXTURE_CUBE_MAP;
#if OGRE_NO_GLES3_SUPPORT == 0
            case TEX_TYPE_3D:
                return GL_TEXTURE_3D;
            case TEX_TYPE_2D_ARRAY:
                return GL_TEXTURE_2D_ARRAY;
#endif
            default:
                return 0;
        };
    }

    void GLES2Texture::_createGLTexResource()
    {
        GLES2RenderSystem* rs = getGLES2RenderSystem();
        if(!rs->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES) ||
           (rs->getCapabilities()->getNonPOW2TexturesLimited() && mNumRequestedMipmaps > 0))
        {
            // Convert to nearest power-of-two size if required
            mWidth = GLES2PixelUtil::optionalPO2(mWidth);
            mHeight = GLES2PixelUtil::optionalPO2(mHeight);
            mDepth = GLES2PixelUtil::optionalPO2(mDepth);
        }

        // Adjust format if required
        mFormat = TextureManager::getSingleton().getNativeFormat(mTextureType, mFormat, mUsage);
        GLenum texTarget = getGLES2TextureTarget();

        // Check requested number of mipmaps
        size_t maxMips = GLES2PixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth, mFormat);
        
        if(PixelUtil::isCompress(mFormat) && (mNumMipmaps == 0))
            mNumRequestedMipmaps = 0;
        
        mNumMipmaps = mNumRequestedMipmaps;
        if (mNumMipmaps > maxMips)
            mNumMipmaps = maxMips;

        // Generate texture name
        GLCheck(glGenTextures(1, &mTextureID));
           
        // Set texture type
        GLCheck(glBindTexture(texTarget, mTextureID));
        
        // If we can do automip generation and the user desires this, do so
        mMipmapsHardwareGenerated =
            Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_AUTOMIPMAP) && !PixelUtil::isCompress(mFormat);

        if(!Bitwise::isPO2(mWidth) || !Bitwise::isPO2(mHeight))
            mMipmapsHardwareGenerated = false;

        // glGenerateMipmap require all mip levels to be prepared. So override how many this texture has.
        if((mUsage & TU_AUTOMIPMAP) && mMipmapsHardwareGenerated && mNumRequestedMipmaps)
            mNumMipmaps = maxMips;

        if(rs->hasMinGLVersion(3, 0) || rs->checkGpuApi("GL_APPLE_texture_max_level"))
            GLCheck(glTexParameteri(texTarget, GL_TEXTURE_MAX_LEVEL_APPLE, mNumRequestedMipmaps ? mNumMipmaps + 1 : 0));

        // Set some misc default parameters, these can of course be changed later
        GLCheck(glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, ((mUsage & TU_AUTOMIPMAP) && mNumRequestedMipmaps) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST));
        GLCheck(glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCheck(glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCheck(glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        // Set up texture swizzling
#if OGRE_NO_GLES3_SUPPORT == 0
        if(rs->hasMinGLVersion(3, 0))
        {
            GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_R, GL_RED));
            GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_G, GL_GREEN));
            GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_B, GL_BLUE));
            GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_A, GL_ALPHA));

            if(mFormat == PF_L8ui || mFormat == PF_L16ui)
            {
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_R, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_G, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_B, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_A, GL_RED));
            }
        }
#endif

        // Allocate internal buffer so that glTexSubImageXD can be used
        // Internal format
        GLenum format = GLES2PixelUtil::getGLOriginFormat(mFormat, mHwGamma);
        GLenum internalformat = GLES2PixelUtil::getClosestGLInternalFormat(mFormat, mHwGamma);
        uint32 width = mWidth;
        uint32 height = mHeight;
        uint32 depth = mDepth;
        
        if (PixelUtil::isCompress(mFormat))
        {
            // Compressed formats
            GLsizei size = static_cast<GLsizei>(PixelUtil::getSize(mWidth, mHeight, mDepth, mFormat));
            
            // Provide temporary buffer filled with zeroes as glCompressedTexImageXD does not
            // accept a 0 pointer like normal glTexImageXD
            // Run through this process for every mipmap to pregenerate mipmap pyramid
            
            uint8* tmpdata = new uint8[size];
            memset(tmpdata, 0, size);
            for (GLint mip = 0; mip <= mNumMipmaps; mip++)
            {
#if OGRE_DEBUG_MODE
                LogManager::getSingleton().logMessage("GLES2Texture::create - Mip: " + StrConv::conv(mip) +
                                                      " Width: " + StrConv::conv(width) +
                                                      " Height: " + StrConv::conv(height) +
                                                      " Internal Format: " + StrConv::conv(internalformat, 0, ' ', std::ios::hex) +
                                                      " Format: " + StrConv::conv(format, 0, ' ', std::ios::hex)
                                                      );
#endif
                size = static_cast<GLsizei>(PixelUtil::getSize(width, height, depth, mFormat));
                
                switch(mTextureType)
                {
                    case TEX_TYPE_1D:
                    case TEX_TYPE_2D:
                        GLCheck(glCompressedTexImage2D(GL_TEXTURE_2D,
                                               mip,
                                               internalformat,
                                               width, height,
                                               0,
                                               size,
                                               tmpdata));
                        break;
                    case TEX_TYPE_CUBE_MAP:
                        for(int face = 0; face < 6; face++) {
                            GLCheck(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, internalformat,
                                width, height, 0, 
                                size, tmpdata));
                        }
                        break;
#if OGRE_NO_GLES3_SUPPORT == 0
                    case TEX_TYPE_2D_ARRAY:
                    case TEX_TYPE_3D:
                        glCompressedTexImage3D(getGLES2TextureTarget(), mip, format,
                            width, height, depth, 0, 
                            size, tmpdata);
                        break;
#endif
                    default:
                        break;
                };
                
                if(width > 1)
                {
                    width = width / 2;
                }
                if(height > 1)
                {
                    height = height / 2;
                }
                if(depth > 1 && mTextureType != TEX_TYPE_2D_ARRAY)
                {
                    depth = depth / 2;
                }
            }
            delete [] tmpdata;
        }
        else
        {
#if OGRE_NO_GLES3_SUPPORT == 0
#if OGRE_DEBUG_MODE
            LogManager::getSingleton().logMessage("GLES2Texture::create - Name: " + mName +
                                                      " ID: " + StrConv::conv(mTextureID) +
                                                      " Width: " + StrConv::conv(width) +
                                                      " Height: " + StrConv::conv(height) +
                                                      " Internal Format: " + StrConv::conv(internalformat, 0, ' ', std::ios::hex));
#endif
            switch(mTextureType)
            {
                case TEX_TYPE_1D:
                case TEX_TYPE_2D:
                case TEX_TYPE_2D_RECT:
                    GLCheck(glTexStorage2D(GL_TEXTURE_2D, GLsizei(mNumMipmaps+1), internalformat, GLsizei(width), GLsizei(height)));
                    break;
                case TEX_TYPE_CUBE_MAP:
                    GLCheck(glTexStorage2D(GL_TEXTURE_CUBE_MAP, GLsizei(mNumMipmaps+1), internalformat, GLsizei(width), GLsizei(height)));
                    break;
                case TEX_TYPE_2D_ARRAY:
                    GLCheck(glTexStorage3D(GL_TEXTURE_2D_ARRAY, GLsizei(mNumMipmaps+1), internalformat, GLsizei(width), GLsizei(height), GLsizei(depth)));
                    break;
                case TEX_TYPE_3D:
                    GLCheck(glTexStorage3D(GL_TEXTURE_3D, GLsizei(mNumMipmaps+1), internalformat, GLsizei(width), GLsizei(height), GLsizei(depth)));
                    break;
            }
#else
            GLenum datatype = GLES2PixelUtil::getGLOriginDataType(mFormat);

            // Run through this process to pregenerate mipmap pyramid
            for(GLint mip = 0; mip <= mNumMipmaps; mip++)
            {
#if OGRE_DEBUG_MODE
                LogManager::getSingleton().logMessage("GLES2Texture::create - Mip: " + StrConv::conv(mip) +
                                                      " Name: " + mName +
                                                      " ID: " + StrConv::conv(mTextureID) +
                                                      " Width: " + StrConv::conv(width) +
                                                      " Height: " + StrConv::conv(height) +
                                                      " Internal Format: " + StrConv::conv(internalformat, 0, ' ', std::ios::hex) +
                                                      " Format: " + StrConv::conv(format, 0, ' ', std::ios::hex) +
                                                      " Datatype: " + StrConv::conv(datatype, 0, ' ', std::ios::hex)
                                                      );
#endif
                // Normal formats
                switch(mTextureType)
                {
                    case TEX_TYPE_1D:
                    case TEX_TYPE_2D:
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
                        if(internalformat != format)
                        {
                            LogManager::getSingleton().logMessage("glTexImage2D: format != internalFormat, "
                                "format=" + StrConv::conv(format) + 
                                ", internalFormat=" + StrConv::conv(internalformat));
                        }
#endif
                        GLCheck(glTexImage2D(GL_TEXTURE_2D,
                                     mip,
                                     internalformat,
                                     width, height,
                                     0,
                                     format,
                                     datatype, 0));
                        break;
                    case TEX_TYPE_CUBE_MAP:
                        for(int face = 0; face < 6; face++) {
                            GLCheck(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, internalformat,
                                width, height, 0, 
                                format, datatype, 0));
                        }
                        break;
                    default:
                        break;
                };
                
                if (width > 1)
                {
                    width = Bitwise::firstPO2From(width / 2);
                }
                if (height > 1)
                {
                    height = Bitwise::firstPO2From(height / 2);
                }
            }
#endif
        }
    }
    
    // Creation / loading methods
    void GLES2Texture::createInternalImpl(void)
    {
        _createGLTexResource();
        
        _createSurfaceList();

        // Get final internal format
        mFormat = getBuffer(0,0)->getFormat();
    }

    void GLES2Texture::createRenderTexture(void)
    {
        // Create the GL texture
        // This already does everything necessary
        createInternalResources();
    }

    void GLES2Texture::prepareImpl()
    {
        if (mUsage & TU_RENDERTARGET) return;

        String baseName, ext;
        size_t pos = mName.find_last_of(".");
        baseName = mName.substr(0, pos);

        if (pos != String::npos)
        {
            ext = mName.substr(pos+1);
        }

        LoadedImages loadedImages = LoadedImages(new vector<Image>::type());

        if (mTextureType == TEX_TYPE_1D || mTextureType == TEX_TYPE_2D ||
            mTextureType == TEX_TYPE_2D_ARRAY || mTextureType == TEX_TYPE_3D)

        {
            doImageIO(mName, mGroup, ext, *loadedImages, this);

            // If this is a volumetric texture set the texture type flag accordingly.
            // If this is a cube map, set the texture type flag accordingly.
            if ((*loadedImages)[0].hasFlag(IF_CUBEMAP))
                mTextureType = TEX_TYPE_CUBE_MAP;

            // If this is a volumetric texture set the texture type flag accordingly.
            if((*loadedImages)[0].getDepth() > 1 && mTextureType != TEX_TYPE_2D_ARRAY)
                mTextureType = TEX_TYPE_3D;
                        
            // If PVRTC and 0 custom mipmap disable auto mip generation and disable software mipmap creation
            if (PixelUtil::isCompress((*loadedImages)[0].getFormat()))
            {
                size_t imageMips = (*loadedImages)[0].getMipmapCount();
                if (imageMips == 0)
                {
                    mNumMipmaps = mNumRequestedMipmaps = imageMips;
                    // Disable flag for auto mip generation
                    mUsage &= ~TU_AUTOMIPMAP;
                }
            }
        }
        else if (mTextureType == TEX_TYPE_CUBE_MAP)
        {
            if(getSourceFileType() == "dds" || getSourceFileType() == "oitd")
            {
                // XX HACK there should be a better way to specify whether 
                // all faces are in the same file or not
                doImageIO(mName, mGroup, ext, *loadedImages, this);
            }
            else
            {
                vector<Image>::type images(6);
                ConstImagePtrList imagePtrs;
                static const String suffixes[6] = {"_rt", "_lf", "_up", "_dn", "_fr", "_bk"};

                for(size_t i = 0; i < 6; i++)
                {
                    String fullName = baseName + suffixes[i];
                    if (!ext.empty())
                        fullName = fullName + "." + ext;
                    // find & load resource data intro stream to allow resource
                    // group changes if required
                    doImageIO(fullName, mGroup, ext, *loadedImages, this);
                }
            }
        }
        else
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "**** Unknown texture type ****");
        }

        mLoadedImages = loadedImages;
    }

    void GLES2Texture::unprepareImpl()
    {
        mLoadedImages.setNull();
    }

    void GLES2Texture::loadImpl()
    {
        if (mUsage & TU_RENDERTARGET)
        {
            createRenderTexture();
            return;
        }

        // Now the only copy is on the stack and will be cleaned in case of
        // exceptions being thrown from _loadImages
        LoadedImages loadedImages = mLoadedImages;
        mLoadedImages.setNull();

        // Call internal _loadImages, not loadImage since that's external and 
        // will determine load status etc again
        ConstImagePtrList imagePtrs;

        for (size_t i = 0; i < loadedImages->size(); ++i)
        {
            imagePtrs.push_back(&(*loadedImages)[i]);
        }

        _loadImages(imagePtrs);

        if((mUsage & TU_AUTOMIPMAP) &&
           mNumRequestedMipmaps && mMipmapsHardwareGenerated)
        {
            GLCheck(glGenerateMipmap(getGLES2TextureTarget()));
        }
    }

    void GLES2Texture::freeInternalImpl()
    {
        mSurfaceList.clear();
        GLCheck(glDeleteTextures(1, &mTextureID));
        mTextureID = 0;
    }
    
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    void GLES2Texture::notifyOnContextLost()
    {
        if (!mIsManual) 
        {
            freeInternalResources();
        }
        else
        {
            GLCheck(glDeleteTextures(1, &mTextureID));
            mTextureID = 0;
        }
    }
    
    void GLES2Texture::notifyOnContextReset()
    {
        if (!mIsManual) 
        {
            reload();
        }
        else
        {
            preLoadImpl();
            
            _createGLTexResource();
            
            for(size_t i = 0; i < mSurfaceList.size(); i++)
            {
                static_cast<GLES2TextureBuffer*>(mSurfaceList[i].getPointer())->updateTextureId(mTextureID);
            }
            
            if (mLoader)
            {
                mLoader->loadResource(this);
            }
            
            postLoadImpl();
        }
    }
#endif

    void GLES2Texture::_createSurfaceList()
    {
        mSurfaceList.clear();

        // For all faces and mipmaps, store surfaces as HardwarePixelBufferSharedPtr
        bool wantGeneratedMips = (mUsage & TU_AUTOMIPMAP)!=0;

        // Do mipmapping in software? (uses GLU) For some cards, this is still needed. Of course,
        // only when mipmap generation is desired.
        bool doSoftware = wantGeneratedMips && !mMipmapsHardwareGenerated && getMipmapCount();

        for (size_t face = 0; face < getNumFaces(); face++)
        {
            uint32 width = mWidth;
            uint32 height = mHeight;
            uint32 depth = mDepth;

            for (uint8 mip = 0; mip <= getMipmapCount(); mip++)
            {
                v1::GLES2HardwarePixelBuffer *buf = N_new v1::GLES2TextureBuffer(mName,
                                                                            getGLES2TextureTarget(),
                                                                            mTextureID,
                                                                            width, height, depth,
                                                                            GLES2PixelUtil::getClosestGLInternalFormat(mFormat, mHwGamma),
                                                                            GLES2PixelUtil::getGLOriginDataType(mFormat),
                                                                            static_cast<GLint>(face),
                                                                            mip,
                                                                            static_cast<v1::Buffer::Usage>(mUsage),
                                                                            doSoftware && mip == 0, mHwGamma, mFSAA);

                mSurfaceList.push_back(v1::HardwarePixelBufferSharedPtr(buf));

                // Check for error
                if (buf->getWidth() == 0 ||
                    buf->getHeight() == 0 ||
                    buf->getDepth() == 0)
                {
                    N_EXCEPT(
                        RenderingAPI,
                        "Zero sized texture surface on texture " + getName() +
                            " face " + StrConv::conv(face) +
                            " mipmap " + StrConv::conv(mip) +
                            ". The GL driver probably refused to create the texture.");
                }
            }
        }
    }

    void GLES2Texture::mipmap(void)
    {
        const GLenum texTarget = getGLES2TextureTarget();
        GLCheck( glBindTexture( texTarget, mTextureID ) );
        GLCheck( glGenerateMipmap( texTarget ) );

        mSurfaceList[0]->getRenderTarget()->_setMipmapsUpdated();
    }

    v1::HardwarePixelBufferSharedPtr GLES2Texture::getBuffer(size_t face, size_t mipmap)
    {
        if (face >= getNumFaces())
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Face index out of range");
        }

        if (mipmap > mNumMipmaps)
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Mipmap index out of range");
        }

        unsigned long idx = face * (mNumMipmaps + 1) + mipmap;
        N_assert(idx < mSurfaceList.size());
        return mSurfaceList[idx];
    }
    
    void GLES2Texture::getPlatformData(const String& name, void* pData)
	{
		if (name == "GLID")
			*static_cast<GLuint*>(pData) = mTextureID;
	}
    
    GLES2Texture::GLES2Texture(ResourceManager* creator, const String& name,
                             ResourceHandle handle, const String& group, bool isManual,
                             ManualResourceLoader* loader, GLES2RenderSystem* renderSystem)
        : GLTextureCommon(creator, name, handle, group, isManual, loader),
          mRenderSys(renderSystem)
    {
    }

    GLES2Texture::~GLES2Texture()
    {
        // have to call this here rather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        if (isLoaded())
        {
            unload();
        }
        else
        {
            freeInternalResources();
        }
    }

    GLenum GLES2Texture::getGLES2TextureTarget(void) const
    {
        switch(mTextureType)
        {
            case TEX_TYPE_1D:
            case TEX_TYPE_2D:
                return GL_TEXTURE_2D;
            case TEX_TYPE_CUBE_MAP:
                return GL_TEXTURE_CUBE_MAP;
            case TEX_TYPE_3D:
                return GL_TEXTURE_3D_OES;
            case TEX_TYPE_2D_ARRAY:
                return GL_TEXTURE_2D_ARRAY;
            case TEX_TYPE_EXTERNAL_OES:
                return GL_TEXTURE_EXTERNAL_OES;
            default:
                return 0;
        };
    }

    void GLES2Texture::_createGLTexResource()
    {
        const RenderFeature *renderCaps =
                Root::getSingleton().getRenderSystem()->getCapabilities();

        const bool nonPowerOfTwoSupported = renderCaps->hasCapability(RSC_NON_POWER_OF_2_TEXTURES) ||
                                            ( renderCaps->getNonPOW2TexturesLimited() &&
                                              mNumRequestedMipmaps == 0 );

        if( !nonPowerOfTwoSupported )
        {
            // Convert to nearest power-of-two size if required
            mWidth = Bitwise::firstPO2From(mWidth);
            mHeight = Bitwise::firstPO2From(mHeight);
            mDepth = Bitwise::firstPO2From(mDepth);
        }

        // set Buffer::Usage for TU_RENDERTARGET if nothing else specified
        if((mUsage & TU_RENDERTARGET) && (mUsage & ~TU_RENDERTARGET) == 0)
            mUsage |= Buffer::HBU_DYNAMIC;

        // Adjust format if required
        mFormat = TextureManager::getSingleton().getNativeFormat(mTextureType, mFormat, mUsage);
        GLenum texTarget = getGLES2TextureTarget();

        // Check requested number of mipmaps
        uint32 maxMips = getMaxMipmaps();
        
        if(PixelUtil::isCompress(mFormat) && (mNumMipmaps == 0))
            mNumRequestedMipmaps = 0;
        
        mNumMipmaps = mNumRequestedMipmaps;
        if (mNumMipmaps > maxMips)
            mNumMipmaps = maxMips;

        // Generate texture name
        GLCheck(glGenTextures(1, &mTextureID));
           
        // Set texture type
        mRenderSys->getStateManager()->bindGLTexture(texTarget, mTextureID);
        
        // If we can do automip generation and the user desires this, do so
        mMipmapsHardwareGenerated = !PixelUtil::isCompress(mFormat);

        // glGenerateMipmap require all mip levels to be prepared. So override how many this texture has.
        if((mUsage & TU_AUTOMIPMAP) && mMipmapsHardwareGenerated && mNumRequestedMipmaps)
            mNumMipmaps = maxMips;

        if(mRenderSys->hasMinGLVersion(3, 0) || mRenderSys->checkGpuApi("GL_APPLE_texture_max_level"))
            mRenderSys->getStateManager()->setTexParameteri(texTarget, GL_TEXTURE_MAX_LEVEL_APPLE, mNumRequestedMipmaps ? mNumMipmaps + 1 : 0);

        // Set some misc default parameters, these can of course be changed later
        if(mTextureType == TEX_TYPE_EXTERNAL_OES && mNumRequestedMipmaps > 0) {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Mipmaps are not available for TEX_TYPE_EXTERNAL_OES", "GLES2Texture::_createGLTexResource");
        }

        mRenderSys->getStateManager()->setTexParameteri(texTarget, 
                                                            GL_TEXTURE_MIN_FILTER, ((mUsage & TU_AUTOMIPMAP) && mNumRequestedMipmaps) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
        mRenderSys->getStateManager()->setTexParameteri(texTarget,
                                                            GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        mRenderSys->getStateManager()->setTexParameteri(texTarget,
                                                            GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        mRenderSys->getStateManager()->setTexParameteri(texTarget,
                                                            GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        bool hasGLES30 = mRenderSys->hasMinGLVersion(3, 0);

        // Set up texture swizzling (not available in WebGL2)
        if (hasGLES30 && PixelUtil::isLuminance(mFormat) && (OGRE_PLATFORM != OGRE_PLATFORM_EMSCRIPTEN))
        {
            if (PixelUtil::getComponentCount(mFormat) == 2)
            {
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_R, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_G, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_B, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_A, GL_GREEN));
            }
            else
            {
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_R, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_G, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_B, GL_RED));
                GLCheck(glTexParameteri(texTarget, GL_TEXTURE_SWIZZLE_A, GL_ONE));
            }
        }

        // Allocate internal buffer so that glTexSubImageXD can be used
        // Internal format
        GLenum format = GLES2PixelUtil::getGLOriginFormat(mFormat);
        GLenum internalformat = GLES2PixelUtil::getGLInternalFormat(mFormat, mHwGamma);
        uint32 width = mWidth;
        uint32 height = mHeight;
        uint32 depth = mDepth;
        
        if (PixelUtil::isCompress(mFormat))
        {
            // Compressed formats
            GLsizei size = static_cast<GLsizei>(PixelUtil::getSize(mWidth, mHeight, mDepth, mFormat));
            
            // Provide temporary buffer filled with zeroes as glCompressedTexImageXD does not
            // accept a 0 pointer like normal glTexImageXD
            // Run through this process for every mipmap to pregenerate mipmap pyramid
            
            std::vector<uint8> tmpdata(size);
            for (uint32 mip = 0; mip <= mNumMipmaps; mip++)
            {
#if OGRE_DEBUG_MODE
                LogManager::getSingleton().logMessage("GLES2Texture::create - Mip: " + StrConv::conv(mip) +
                                                      " Width: " + StrConv::conv(width) +
                                                      " Height: " + StrConv::conv(height) +
                                                      " Internal Format: " + StrConv::conv(internalformat, 0, ' ', std::ios::hex) +
                                                      " Format: " + StrConv::conv(format, 0, ' ', std::ios::hex)
                                                      );
#endif
                size = static_cast<GLsizei>(PixelUtil::getSize(width, height, depth, mFormat));
                
                switch(mTextureType)
                {
                    case TEX_TYPE_1D:
                    case TEX_TYPE_2D:
                    case TEX_TYPE_2D_RECT:
                        GLCheck(glCompressedTexImage2D(GL_TEXTURE_2D,
                                               mip,
                                               internalformat,
                                               width, height,
                                               0,
                                               size,
                                               &tmpdata[0]));
                        break;
                    case TEX_TYPE_CUBE_MAP:
                        for(int face = 0; face < 6; face++) {
                            GLCheck(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, internalformat,
                                width, height, 0, 
                                size, &tmpdata[0]));
                        }
                        break;
                    case TEX_TYPE_2D_ARRAY:
                        if(!hasGLES30)
                            break;
                        OGRE_FALLTHROUGH;
                    case TEX_TYPE_3D:
                        glCompressedTexImage3DOES(getGLES2TextureTarget(), mip, format,
                            width, height, depth, 0, 
                            size, &tmpdata[0]);
                        break;
                    case TEX_TYPE_EXTERNAL_OES:
                        N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Attempt to create mipmap for TEX_TYPE_EXTERNAL_OES, should never happen", "GLES2Texture::_createGLTexResource");
                };
                
                if(width > 1)
                {
                    width = width / 2;
                }
                if(height > 1)
                {
                    height = height / 2;
                }
                if(depth > 1 && mTextureType != TEX_TYPE_2D_ARRAY)
                {
                    depth = depth / 2;
                }
            }
            return;
        }

        if(hasGLES30)
        {
#if OGRE_DEBUG_MODE
            LogManager::getSingleton().logMessage("GLES2Texture::create - Name: " + mName +
                                                      " ID: " + StrConv::conv(mTextureID) +
                                                      " Width: " + StrConv::conv(width) +
                                                      " Height: " + StrConv::conv(height) +
                                                      " Internal Format: " + StrConv::conv(internalformat, 0, ' ', std::ios::hex));
#endif
            switch(mTextureType)
            {
                case TEX_TYPE_1D:
                case TEX_TYPE_2D:
                case TEX_TYPE_2D_RECT:
                    GLCheck(glTexStorage2D(GL_TEXTURE_2D, GLsizei(mNumMipmaps+1), internalformat, GLsizei(width), GLsizei(height)));
                    break;
                case TEX_TYPE_CUBE_MAP:
                    GLCheck(glTexStorage2D(GL_TEXTURE_CUBE_MAP, GLsizei(mNumMipmaps+1), internalformat, GLsizei(width), GLsizei(height)));
                    break;
                case TEX_TYPE_2D_ARRAY:
                case TEX_TYPE_3D:
                    GLCheck(glTexStorage3D(getGLES2TextureTarget(), GLsizei(mNumMipmaps+1), internalformat, GLsizei(width), GLsizei(height), GLsizei(depth)));
                    break;
                case TEX_TYPE_EXTERNAL_OES:
                    // Not available for TEX_TYPE_EXTERNAL_OES
                    break;
            }
            return;
        }
        GLenum datatype = GLES2PixelUtil::getGLOriginDataType(mFormat);

        // Run through this process to pregenerate mipmap pyramid
        for(uint32 mip = 0; mip <= mNumMipmaps; mip++)
        {
#if OGRE_DEBUG_MODE
            LogManager::getSingleton().logMessage("GLES2Texture::create - Mip: " + StrConv::conv(mip) +
                                                  " Name: " + mName +
                                                  " ID: " + StrConv::conv(mTextureID) +
                                                  " Width: " + StrConv::conv(width) +
                                                  " Height: " + StrConv::conv(height) +
                                                  " Internal Format: " + StrConv::conv(internalformat, 0, ' ', std::ios::hex) +
                                                  " Format: " + StrConv::conv(format, 0, ' ', std::ios::hex) +
                                                  " Datatype: " + StrConv::conv(datatype, 0, ' ', std::ios::hex)
                                                  );
#endif
            // Normal formats
            switch(mTextureType)
            {
                case TEX_TYPE_1D:
                case TEX_TYPE_2D:
                    GLCheck(glTexImage2D(GL_TEXTURE_2D,
                                 mip,
                                 internalformat,
                                 width, height,
                                 0,
                                 format,
                                 datatype, 0));
                    break;
                case TEX_TYPE_2D_ARRAY:
                    if(!hasGLES30)
                        break;
                    OGRE_FALLTHROUGH;
                case TEX_TYPE_3D:
                    GLCheck(glTexImage3DOES(getGLES2TextureTarget(),
                                 mip,
                                 internalformat,
                                 width, height, depth,
                                 0,
                                 format,
                                 datatype, 0));
                    break;
                case TEX_TYPE_CUBE_MAP:
                    for(int face = 0; face < 6; face++) {
                        GLCheck(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, internalformat,
                            width, height, 0,
                            format, datatype, 0));
                    }
                    break;
                default:
                    break;
            };

            if (width > 1)
            {
                width = Bitwise::firstPO2From(width / 2);
            }
            if (height > 1)
            {
                height = Bitwise::firstPO2From(height / 2);
            }
        }
    }
    
    // Creation / loading methods
    void GLES2Texture::createInternalImpl(void)
    {
        _createGLTexResource();
        
        _createSurfaceList();

        // Get final internal format
        mFormat = getBuffer(0,0)->getFormat();
    }

    void GLES2Texture::freeInternalImpl()
    {
        if (GLES2StateCacheManager* stateCacheManager = mRenderSys->getStateManager())
        {
            GLCheck(glDeleteTextures(1, &mTextureID));
            stateCacheManager->invalidateStateForTexture(mTextureID);
        }
        mTextureID = 0;
    }
    
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    void GLES2Texture::notifyOnContextLost()
    {
        if (!mIsManual) 
        {
            freeInternalResources();
        }
        else
        {
            GLCheck(glDeleteTextures(1, &mTextureID));
            mTextureID = 0;
        }
    }
    
    void GLES2Texture::notifyOnContextReset()
    {
        if (!mIsManual) 
        {
            reload();
        }
        else
        {
            preLoadImpl();
            
            _createGLTexResource();
            
            for(size_t i = 0; i < mSurfaceList.size(); i++)
            {
                static_cast<GLES2TextureBuffer*>(mSurfaceList[i].get())->updateTextureId(mTextureID);
            }
            
            if (mLoader)
            {
                mLoader->loadResource(this);
            }
            
            postLoadImpl();
        }
    }
#endif

    void GLES2Texture::_createSurfaceList()
    {
        mSurfaceList.clear();

        uint32 depth = mDepth;

        // For all faces and mipmaps, store surfaces as HardwarePixelBufferSharedPtr
        for (size_t face = 0; face < getNumFaces(); face++)
        {
            uint32 width = mWidth;
            uint32 height = mHeight;

            for (uint32 mip = 0; mip <= getMipmapCount(); mip++)
            {
                GLES2HardwarePixelBuffer* buf = N_new GLES2TextureBuffer(
                    this, static_cast<GLint>(face), mip, width, height, depth);

                mSurfaceList.push_back(HardwarePixelBufferSharedPtr(buf));

                if (width > 1)
                    width = width / 2;
                if (height > 1)
                    height = height / 2;
                if (depth > 1 && mTextureType != TEX_TYPE_2D_ARRAY)
                    depth = depth / 2;
            }
        }
    }
}
