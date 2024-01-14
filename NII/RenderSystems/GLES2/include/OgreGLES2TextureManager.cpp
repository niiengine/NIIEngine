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

#include "OgreGLES2TextureManager.h"
#include "OgreGLRenderTexture.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreGLES2StateCacheManager.h"
#include "OgreGLES2PixelFormat.h"

namespace Ogre {
    GLES2TextureManager::GLES2TextureManager(GLES2RenderSystem* renderSystem)
        : TextureManager(), mRenderSys(renderSystem)
    {
        // Register with group manager
        ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }

    GLES2TextureManager::~GLES2TextureManager()
    {
        // Unregister with group manager
        ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
    }

    Resource* GLES2TextureManager::createImpl(const String& name, ResourceHandle handle, 
                                           const String& group, bool isManual,
                                           ManualResourceLoader* loader,
                                           const PropertyData* createParams)
    {
        return N_new GLES2Texture(this, name, handle, group, isManual, loader, mRenderSys);
    }

    PixelFormat GLES2TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
    {
        // Adjust requested parameters to capabilities
        const RenderFeature *caps = Root::getSingleton().getRenderSystem()->getCapabilities();

        // Check compressed texture support
        // if a compressed format not supported, revert to PF_A8R8G8B8
        if (PixelUtil::isCompress(format) &&
            !caps->hasCapability(RSC_TEXTURE_COMPRESSION))
        {
            return PF_BYTE_RGBA;
        }
        // if floating point textures not supported, revert to PF_A8R8G8B8
        if (PixelUtil::isFloatingPoint(format) &&
            !caps->hasCapability(RSC_TEXTURE_FLOAT))
        {
            return PF_BYTE_RGBA;
        }

        // format not supported by GLES2: e.g. BGR
        if(GLES2PixelUtil::getGLInternalFormat(format) == GL_NONE)
        {
            return PF_BYTE_RGBA;
        }

        // Check if this is a valid rendertarget format
        if (usage & TU_RENDERTARGET)
        {
            /// Get closest supported alternative
            /// If mFormat is supported it's returned
            return GLRTTManager::getSingleton().getSupportedAlternative(format);
        }

        // Supported
        return format;
    }

    bool GLES2TextureManager::isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
            bool preciseFormatOnly)
    {
        // precise format check
        if (!TextureManager::isHardwareFilteringSupported(ttype, format, usage, preciseFormatOnly))
            return false;

        // Assume non-floating point is supported always
        if (!PixelUtil::isFloatingPoint(getNativeFormat(ttype, format, usage)))
            return true;
        
        // check for floating point extension
        return mRenderSys->checkGpuApi("GL_OES_texture_float_linear");
    }
    GLES2TextureManager::GLES2TextureManager(GLES2Support& support)
        : TextureManager(), mGLSupport(support), mWarningTextureID(0)
    {
        // Register with group manager
        ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }

    GLES2TextureManager::~GLES2TextureManager()
    {
        // Unregister with group manager
        ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);

        // Delete warning texture
        GLCheck(glDeleteTextures(1, &mWarningTextureID));
    }

    Resource* GLES2TextureManager::createImpl(const String& name, ResourceHandle handle, 
                                           const String& group, bool isManual,
                                           ManualResourceLoader* loader,
                                           const PropertyData* createParams)
    {
        if( createParams )
        {
            if( createParams->find( "DepthTexture" ) != createParams->end() )
            {
                const bool shareableDepthBuffer = createParams->find( "shareableDepthBuffer" ) !=
                                                                                createParams->end();
                return N_new GLES2DepthTexture( shareableDepthBuffer, this, name, handle, group,
                                                isManual, loader, mGLSupport );
            }

            PropertyData::const_iterator it = createParams->find( "SpecialFormat" );
            if( it != createParams->end() && it->second == "PF_NULL" )
            {
                return N_new GLES2NullTexture( this, name, handle, group,
                                               isManual, loader, mGLSupport );
            }
        }

        return N_new GLES2Texture(this, name, handle, group, isManual, loader, mGLSupport);
    }

    //-----------------------------------------------------------------------------
    void GLES2TextureManager::createWarningTexture()
    {
        // Generate warning texture
        uint32 width = 8;
        uint32 height = 8;

        uint16* data = new uint16[width * height];

        // Yellow/black stripes
        for(size_t y = 0; y < height; ++y)
        {
            for(size_t x = 0; x < width; ++x)
            {
                data[y * width + x] = (((x + y) % 8) < 4) ? 0x0000 : 0xFFF0;
            }
        }

        // Create GL resource
        GLCheck(glGenTextures(1, &mWarningTextureID));
        GLCheck(glBindTexture(GL_TEXTURE_2D, mWarningTextureID));

        GLCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                                         GL_UNSIGNED_SHORT_5_6_5, (void*)data));
        // Free memory
        delete [] data;
    }

    PixelFormat GLES2TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
    {
        // Adjust requested parameters to capabilities
        const RenderFeature *caps = Root::getSingleton().getRenderSystem()->getCapabilities();

        // Check compressed texture support
        // if a compressed format not supported, revert to PF_A8R8G8B8
        if (PixelUtil::isCompress(format) &&
            !caps->hasCapability(RSC_TEXTURE_COMPRESSION))
        {
            return PF_A8R8G8B8;
        }
        // if floating point textures not supported, revert to PF_A8R8G8B8
        if (PixelUtil::isFloatingPoint(format) &&
            !caps->hasCapability(RSC_TEXTURE_FLOAT))
        {
            return PF_A8R8G8B8;
        }

        // Check if this is a valid rendertarget format
        if (usage & TU_RENDERTARGET)
        {
            /// Get closest supported alternative
            /// If mFormat is supported it's returned
            return GLES2RTTManager::getSingleton().getSupportedAlternative(format);
        }

        // Supported
        return format;
    }

    bool GLES2TextureManager::isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
            bool preciseFormatOnly)
    {
        if (format == PF_UNKNOWN)
        {
            return false;
        }

        // Check native format
        PixelFormat nativeFormat = getNativeFormat(ttype, format, usage);
        if (preciseFormatOnly && format != nativeFormat)
        {
            return false;
        }

        // Assume non-floating point is supported always
        if (!PixelUtil::isFloatingPoint(nativeFormat))
        {
            return true;
        }

        return false;
    }
}
