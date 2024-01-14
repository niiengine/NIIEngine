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
#define NOMINMAX
#include "OgreGLES2RenderSystem.h"
#include "OgreGLES2TextureManager.h"
#include "OgreGLES2DepthBuffer.h"
#include "OgreGLES2HardwarePixelBuffer.h"
#include "OgreGLES2HardwareBufferManager.h"
#include "OgreGLES2HardwareIndexBuffer.h"
#include "OgreGLES2HardwareVertexBuffer.h"
#include "OgreGpuProgramManager.h"
#include "OgreGLUtil.h"
#include "OgreGLES2FBORenderTexture.h"
#include "OgreGLES2HardwareOcclusionQuery.h"
#include "OgreGLVertexArrayObject.h"
#include "OgreGLSLESProgramFactory.h"
#include "OgreRoot.h"
#include "OgreViewport.h"
#include "OgreFrustum.h"
#if !OGRE_NO_GLES2_CG_SUPPORT
#include "OgreGLSLESCgProgramFactory.h"
#endif
#include "OgreGLSLESLinkProgram.h"
#include "OgreGLSLESProgramManager.h"
#include "OgreGLSLESProgramPipeline.h"
#include "OgreGLES2StateCacheManager.h"
#include "OgreRenderWindow.h"
#include "OgreGLES2PixelFormat.h"
#include "OgreGLES2FBOMultiRenderTarget.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#include "OgreEAGLES2Context.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
#   include "OgreGLES2ManagedResourceManager.h"
Ogre::GLES2ManagedResourceManager* Ogre::GLES2RenderSystem::mResourceManager = NULL;
#endif

// Convenience macro from ARB_vertex_buffer_object spec
#define VBO_BUFFER_OFFSET(i) ((char *)(i))

#ifndef GL_PACK_ROW_LENGTH_NV
#define GL_PACK_ROW_LENGTH_NV             0x0D02
#endif

using namespace std;

static void gl2ext_to_gl3core() {
    glUnmapBufferOES = glUnmapBuffer;
    glRenderbufferStorageMultisampleAPPLE = glRenderbufferStorageMultisample;

    glGenQueriesEXT = glGenQueries;
    glDeleteQueriesEXT = glDeleteQueries;
    glBeginQueryEXT = glBeginQuery;
    glEndQueryEXT = glEndQuery;
    glGetQueryObjectuivEXT = glGetQueryObjectuiv;

    glMapBufferRangeEXT = glMapBufferRange;
    glFlushMappedBufferRangeEXT = glFlushMappedBufferRange;

    glTexImage3DOES = (PFNGLTEXIMAGE3DOESPROC)glTexImage3D;
    glCompressedTexImage3DOES = glCompressedTexImage3D;
    glTexSubImage3DOES = glTexSubImage3D;
    glCompressedTexSubImage3DOES = glCompressedTexSubImage3D;

    glFenceSyncAPPLE = glFenceSync;
    glClientWaitSyncAPPLE = glClientWaitSync;
    glDeleteSyncAPPLE = glDeleteSync;

    glProgramBinaryOES = glProgramBinary;
    glGetProgramBinaryOES = glGetProgramBinary;

    glDrawElementsInstancedEXT = glDrawElementsInstanced;
    glDrawArraysInstancedEXT = glDrawArraysInstanced;
    glVertexAttribDivisorEXT = glVertexAttribDivisor;
    glBindVertexArrayOES = glBindVertexArray;
    glGenVertexArraysOES = glGenVertexArrays;
    glDeleteVertexArraysOES = glDeleteVertexArrays;
}

namespace Ogre {

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS && OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
    static GLNativeSupport* glsupport;
    static GLESWglProc get_proc(const char* proc) {
        return (GLESWglProc)glsupport->getProcAddress(proc);
    }
#endif

    static GLint getCombinedMinMipFilter(TextureFilterOP min, TextureFilterOP mip)
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

    GLES2RenderSystem::GLES2RenderSystem()
        : mState(0),
          mGpuProgramManager(0),
          mGLSLESProgramFactory(0),
#if !OGRE_NO_GLES2_CG_SUPPORT
          mGLSLESCgProgramFactory(0),
#endif
          mHardwareBufferManager(0),
          mCurTexMipCount(0)
    {
        size_t i;

        LogManager::getSingleton().logMessage(getName() + " created.");

        mRenderAttribsBound.reserve(100);
        mRenderInstanceAttribsBound.reserve(100);

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
        mResourceManager = N_new GLES2ManagedResourceManager();
#endif
        
        mGLSupport = getGLSupport(GLNativeSupport::CONTEXT_ES);
        
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS && OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_WIN32
        glsupport = mGLSupport;
#endif

        initConfigOptions();

        for (i = 0; i < NII_MAX_TextureLayer; i++)
        {
            // Dummy value
            mTextureTypes[i] = 0;
        }

        mActiveRenderTarget = 0;
        mCurrentContext = 0;
        mMainContext = 0;
        mGLInitialised = false;
        mMinFilter = FO_LINEAR;
        mMipFilter = FO_POINT;
        mPolygonMode = GL_FILL;
        mVS = 0;
        mFS = 0;
        mRTTManager = NULL;
    }

    GLES2RenderSystem::~GLES2RenderSystem()
    {
        shutdown();

        N_delete mGLSupport;
        mGLSupport = 0;

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
        if (mResourceManager != NULL)
        {
            N_delete mResourceManager;
            mResourceManager = NULL;
        }
#endif
    }

    const String& GLES2RenderSystem::getName(void) const
    {
        static String strName("OpenGL ES 2.x Rendering Subsystem");
        return strName;
    }

    void GLES2RenderSystem::init()
    {
        RenderSystem::init();

        mGLSupport->start();
        // Create the texture manager
        mTextureManager = N_new GLES2TextureManager(this);
    }

    RenderFeature* GLES2RenderSystem::setupFeature() const
    {
        RenderFeature* rsc = N_new RenderFeature();

        rsc->setCategoryRelevant(CAPS_CATEGORY_GL, true);
        rsc->setDriverVersion(mVersion);

        const char* deviceName = (const char*)glGetString(GL_RENDERER);
        if (deviceName)
        {
            rsc->setDeviceName(deviceName);
        }

        rsc->setRenderSystemName(getName());
        rsc->parseVendorFromString(mVendor);

        // Multitexturing support and set number of texture units
        GLint units;
        GLCheck(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &units));
        rsc->setMaxTexUnit(std::min(NII_MAX_TextureLayer, units));

        glGetIntegerv( GL_MAX_VERTEX_ATTRIBS , &units);
        rsc->setNumVertexAttributes(units);

        // Check for hardware stencil support and set bit depth
        GLint stencil;

        GLCheck(glGetIntegerv(GL_STENCIL_BITS, &stencil));

        if(stencil)
        {
            rsc->setCapability(RSC_HWSTENCIL);
            rsc->setCapability(RSC_TWO_SIDED_STENCIL);
            rsc->setStencilBufferBitDepth(stencil);
        }

        if(hasMinGLVersion(3, 0) ||
                (checkGpuApi("GL_EXT_sRGB") && checkGpuApi("GL_NV_sRGB_formats")))
            rsc->setCapability(RSC_HW_GAMMA);

        // Scissor test is standard
        rsc->setCapability(RSC_SCISSOR_TEST);

        // Vertex Buffer Objects are always supported by OpenGL ES
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_element_index_uint"))
            rsc->setCapability(RSC_32BIT_INDEX);

        // Check for hardware occlusion support
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_occlusion_query_boolean"))
        {
            rsc->setCapability(RSC_HWOCCLUSION);
        }

        // OpenGL ES - Check for these extensions too
        // For 2.0, http://www.khronos.org/registry/gles/api/2.0/gl2ext.h

        if (checkGpuApi("GL_IMG_texture_compression_pvrtc") ||
            checkGpuApi("GL_EXT_texture_compression_dxt1") ||
            checkGpuApi("GL_EXT_texture_compression_s3tc") ||
            checkGpuApi("GL_OES_compressed_ETC1_RGB8_texture") ||
            checkGpuApi("GL_AMD_compressed_ATC_texture") ||
            checkGpuApi("WEBGL_compressed_texture_s3tc") ||
            checkGpuApi("WEBGL_compressed_texture_atc") ||
            checkGpuApi("WEBGL_compressed_texture_pvrtc") ||
            checkGpuApi("WEBGL_compressed_texture_etc1") ||
            checkGpuApi("WEBGL_compressed_texture_astc") ||
            checkGpuApi("GL_KHR_texture_compression_astc_ldr"))

        {
            rsc->setCapability(RSC_TEXTURE_COMPRESSION);

            if(checkGpuApi("GL_IMG_texture_compression_pvrtc") ||
               checkGpuApi("GL_IMG_texture_compression_pvrtc2") ||
               checkGpuApi("WEBGL_compressed_texture_pvrtc"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_PVRTC);
                
            if((checkGpuApi("GL_EXT_texture_compression_dxt1") &&
               checkGpuApi("GL_EXT_texture_compression_s3tc")) ||
               checkGpuApi("WEBGL_compressed_texture_s3tc"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);

            if(checkGpuApi("GL_OES_compressed_ETC1_RGB8_texture") ||
               checkGpuApi("WEBGL_compressed_texture_etc1"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_ETC1);

            if(hasMinGLVersion(3, 0))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_ETC2);

            if(checkGpuApi("GL_AMD_compressed_ATC_texture") ||
               checkGpuApi("WEBGL_compressed_texture_atc"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_ATC);

            if(checkGpuApi("WEBGL_compressed_texture_astc") ||
               checkGpuApi("GL_KHR_texture_compression_astc_ldr"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_ASTC);
        }

        // Check for Anisotropy support
        if (checkGpuApi("GL_EXT_texture_filter_anisotropic"))
        {
            GLfloat maxAnisotropy = 0;
            GLCheck(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy));
            rsc->setMaxSupportedAnisotropy(maxAnisotropy);
            rsc->setCapability(RSC_ANISOTROPY);
        }

        rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
        if (hasMinGLVersion(3, 0))
        {
            // Probe number of draw buffers
            // Only makes sense with FBO support, so probe here
            GLint buffers;
            glGetIntegerv(GL_MAX_DRAW_BUFFERS, &buffers);
            rsc->setNumMultiRenderTargets(
                std::min<int>(buffers, (GLint)NII_MAX_RenderTarget));
            rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);
        }
        else
        {
            rsc->setNumMultiRenderTargets(1);
        }

        // Stencil wrapping
        rsc->setCapability(RSC_STENCIL_WRAP);

        // GL always shares vertex and fragment texture units (for now?)
        rsc->setVertexTextureUnitsShared(true);

        // Blending support
        rsc->setCapability(RSC_ADVANCED_BLEND_OPERATIONS);

        // DOT3 support is standard
        rsc->setCapability(RSC_DOT3);
        
        // Point size
        GLfloat psRange[2] = {0.0, 0.0};
        GLCheck(glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, psRange));
        rsc->setMaxPointSize(psRange[1]);

        // Point sprites
        rsc->setCapability(RSC_POINT_SPRITES);
        rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
        
        // GLSL ES is always supported in GL ES 2
        rsc->addShaderProfile("glsles");
        if (getShaderVersion() >= 320)
            rsc->addShaderProfile("glsl320es");
        if (getShaderVersion() >= 310)
            rsc->addShaderProfile("glsl310es");
        if (getShaderVersion() >= 300)
            rsc->addShaderProfile("glsl300es");

#if !OGRE_NO_GLES2_CG_SUPPORT
        rsc->addShaderProfile("cg");
        rsc->addShaderProfile("ps_2_0");
        rsc->addShaderProfile("vs_2_0");
#endif

        // UBYTE4 always supported
        rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

        // Infinite far plane always supported
        rsc->setCapability(RSC_INFINITE_FAR_PLANE);

        // Vertex/Fragment Programs
        rsc->setCapability(RSC_VERTEX_PROGRAM);
        rsc->setCapability(RSC_FRAGMENT_PROGRAM);

        // Separate shader objects
        OGRE_IF_IOS_VERSION_IS_GREATER_THAN(5.0)
        if(checkGpuApi("GL_EXT_separate_shader_objects"))
        {
            // this relaxes shader matching rules and requires slightly different GLSL declaration
            // however our usage pattern does not benefit from this and driver support is quite poor
            // so disable it for now (see below)
            /*rsc->setCapability(GF_GLSL_SeparateShaderObject);
            rsc->setCapability(RSC_GLSL_SSO_REDECLARE);*/
        }

        // Mesa 11.2 does not behave according to spec and throws a "gl_Position redefined"
        if(rsc->getDeviceName().find("Mesa") != String::npos) {
            rsc->unsetCapability(RSC_GLSL_SSO_REDECLARE);
        }

        GLfloat floatConstantCount = 0;
#if OGRE_NO_GLES3_SUPPORT == 0
        glGetFloatv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &floatConstantCount);
#else
        glGetFloatv(GL_MAX_VERTEX_UNIFORM_VECTORS, &floatConstantCount);
#endif
        rsc->setVertexProgramConstantFloatCount((Ogre::ushort)floatConstantCount);
        rsc->setVertexProgramConstantBoolCount((Ogre::ushort)floatConstantCount);
        rsc->setVertexProgramConstantIntCount((Ogre::ushort)floatConstantCount);

        // Fragment Program Properties
        floatConstantCount = 0;
#if OGRE_NO_GLES3_SUPPORT == 0
        glGetFloatv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &floatConstantCount);
#else
        glGetFloatv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &floatConstantCount);
#endif
        rsc->setFragmentProgramConstantFloatCount((Ogre::ushort)floatConstantCount);
        rsc->setFragmentProgramConstantBoolCount((Ogre::ushort)floatConstantCount);
        rsc->setFragmentProgramConstantIntCount((Ogre::ushort)floatConstantCount);

        // Check for Float textures
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_texture_float") || checkGpuApi("GL_OES_texture_half_float"))
            rsc->setCapability(RSC_TEXTURE_FLOAT);

        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_texture_3D"))
            rsc->setCapability(RSC_TEXTURE_3D);

        // ES 3 always supports NPOT textures
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_texture_npot") || checkGpuApi("GL_ARB_texture_non_power_of_two"))
        {
            rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
            rsc->setNonPOW2TexturesLimited(false);
        }
        else if(checkGpuApi("GL_APPLE_texture_2D_limited_npot"))
        {
            rsc->setNonPOW2TexturesLimited(true);
        }

        // Alpha to coverage always 'supported' when MSAA is available
        // although card may ignore it if it doesn't specifically support A2C
        rsc->setCapability(RSC_ALPHA_TO_COVERAGE);
        
        // No point sprites, so no size
        rsc->setMaxPointSize(0.f);
        
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_vertex_array_object"))
            rsc->setCapability(RSC_VAO);

        if (hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_get_program_binary"))
        {
            // http://www.khronos.org/registry/gles/extensions/OES/OES_get_program_binary.txt
            GLint formats;
            GLCheck(glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS_OES, &formats));

            if(formats > 0)
                rsc->setCapability(RSC_CAN_GET_COMPILED_SHADER_BUFFER);
        }

        if (hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_instanced_arrays"))
        {
            rsc->setCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA);
        }
        else if(checkGpuApi("GL_ANGLE_instanced_arrays"))
        {
            rsc->setCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA);
            glDrawElementsInstancedEXT = glDrawElementsInstancedANGLE;
            glDrawArraysInstancedEXT = glDrawArraysInstancedANGLE;
            glVertexAttribDivisorEXT = glVertexAttribDivisorANGLE;
        }

        if (checkGpuApi("GL_EXT_debug_marker") &&
            checkGpuApi("GL_EXT_debug_label"))
        {
            OGRE_IF_IOS_VERSION_IS_GREATER_THAN(5.0)
            rsc->setCapability(RSC_DEBUG);
        }

        if ((hasMinGLVersion(3, 0) && OGRE_PLATFORM != OGRE_PLATFORM_EMSCRIPTEN) ||
            checkGpuApi("GL_EXT_map_buffer_range"))
        {
            rsc->setCapability(RSC_MAPBUFFER);
        }

        if(hasMinGLVersion(3, 0))
        {
            // Check if render to vertex buffer (transform feedback in OpenGL)
            rsc->setCapability(RSC_HWRENDER_TO_VERTEX_BUFFER);

            rsc->setCapability(RSC_PRIMITIVE_RESTART);
        }

        GLfloat lineWidth[2] = {1, 1};
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidth);
        if(lineWidth[1] != 1 && lineWidth[1] != lineWidth[0])
            rsc->setCapability(RSC_WIDE_LINES);

        return rsc;
    }

    void GLES2RenderSystem::initImpl(RenderFeature* caps, RenderTarget* primary)
    {
        if(caps->getRenderSystemName() != getName())
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Trying to initialize GLES2RenderSystem from RenderFeature that do not support OpenGL ES");
        }

        if(caps->getNumVertexAttributes() < 16)
            GLSLProgramBase::setTightAttLayout();

        mGpuProgramManager = new GpuProgramManager();
        ResourceGroupManager::getSingleton()._registerResourceManager(mGpuProgramManager->getResourceType(),
                                                                      mGpuProgramManager);

        mGLSLESProgramFactory = N_new GLSLESProgramFactory();
        HighLevelGpuProgramManager::getSingleton().addFactory(mGLSLESProgramFactory);

#if !OGRE_NO_GLES2_CG_SUPPORT
        mGLSLESCgProgramFactory = N_new GLSLESCgProgramFactory();
        HighLevelGpuProgramManager::getSingleton().addFactory(mGLSLESCgProgramFactory);
#endif

        // Use VBO's by default
        mHardwareBufferManager = N_new GLES2HardwareBufferManager();

        // Create FBO manager
        LogManager::getSingleton().logMessage("GL ES 2: Using FBOs for rendering to textures");
        mRTTManager = new GLES2FBOManager();

        Log* defaultLog = LogManager::getSingleton().getDefaultLog();
        if (defaultLog)
        {
            caps->log(defaultLog);
        }

        mGLInitialised = true;
    }

    void GLES2RenderSystem::shutdown(void)
    {

        // Deleting the GLSL program factory
        if (mGLSLESProgramFactory)
        {
            // Remove from manager safely
            if (HighLevelGpuProgramManager::getSingletonPtr())
                HighLevelGpuProgramManager::getSingleton().removeFactory(mGLSLESProgramFactory);
            N_delete mGLSLESProgramFactory;
            mGLSLESProgramFactory = 0;
        }

#if !OGRE_NO_GLES2_CG_SUPPORT
        // Deleting the GLSL program factory
        if (mGLSLESCgProgramFactory)
        {
            // Remove from manager safely
            if (HighLevelGpuProgramManager::getSingletonPtr())
                HighLevelGpuProgramManager::getSingleton().removeFactory(mGLSLESCgProgramFactory);
            N_delete mGLSLESCgProgramFactory;
            mGLSLESCgProgramFactory = 0;
        }
#endif
        // Deleting the GPU program manager and hardware buffer manager.  Has to be done before the mGLSupport->stop().
        if(mGpuProgramManager)
        {
            ResourceGroupManager::getSingleton()._unregisterResourceManager(mGpuProgramManager->getResourceType());
            N_delete mGpuProgramManager;
            mGpuProgramManager = 0;
        }

        N_delete mHardwareBufferManager;
        mHardwareBufferManager = 0;

        delete mRTTManager;
        mRTTManager = 0;

        N_delete mTextureManager;
        mTextureManager = 0;

        // Delete extra threads contexts
        for (GLContextList::iterator i = mBackgroundContextList.begin();
             i != mBackgroundContextList.end(); ++i)
        {
            GLContext* pCurContext = *i;

            pCurContext->releaseContext();

            delete pCurContext;
        }

        mBackgroundContextList.clear();

        RenderSystem::shutdown();

        mGLSupport->stop();

        mGLInitialised = 0;
    }

    RenderWindow* GLES2RenderSystem::createWindow(const String &name, unsigned int width, unsigned int height,
                                                        bool fullScreen, const PropertyData *miscParams)
    {
        if (mRenderTargets.find(name) != mRenderTargets.end())
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "NativeWindowType with name '" + name + "' already exists");
        }

        // Log a message
        StringStream ss;
        ss << "GLES2RenderSystem::createWindow \"" << name << "\", " <<
            width << "x" << height << " ";
        if (fullScreen)
            ss << "fullscreen ";
        else
            ss << "windowed ";

        if (miscParams)
        {
            ss << " miscParams: ";
            PropertyData::const_iterator it;
            for (it = miscParams->begin(); it != miscParams->end(); ++it)
            {
                ss << it->first << "=" << it->second << " ";
            }

            LogManager::getSingleton().logMessage(ss.str());
        }

        // Create the window
        RenderWindow* win = mGLSupport->newWindow(name, width, height, fullScreen, miscParams);
        attachRenderTarget((Ogre::RenderTarget&) *win);

        if (!mGLInitialised)
        {
            initialiseContext(win);

            // Get the shader language version
            const char* shadingLangVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
            LogManager::getSingleton().logMessage("Shading language version: " + String(shadingLangVersion));
            StringList tokens = StringUtil::split(shadingLangVersion, ". ");
            size_t i = 0;

            // iOS reports the GLSL version with a whole bunch of non-digit characters so we have to find where the version starts.
            for(; i < tokens.size(); i++)
            {
                if (isdigit(*tokens[i].c_str()))
                    break;
            }
            mShaderVersion = (StringConverter::parseUnsignedInt(tokens[i]) * 100) + StringConverter::parseUnsignedInt(tokens[i+1]);
            if (mShaderVersion < 100) // Emscripten + MS IE/Edge reports an experimental WebGL version (e.g. 0.96) which causes a compile error
                mShaderVersion = 100;

            // Initialise GL after the first window has been created
            // TODO: fire this from emulation options, and don't duplicate Real and Current capabilities
            mVendorFeature = setupFeature();

            // use real capabilities if custom capabilities are not available
            if (mSysFeatureValid)
                mFeature = mVendorFeature;

            fireEvent("RenderSystemCapabilitiesCreated");

            initImpl(mFeature, (RenderTarget *) win);

            // Initialise the main context
            _oneTimeContextInitialization();
            if (mCurrentContext)
                mCurrentContext->setInitialized();
        }

        if( win->getDepthBufferPool() != DepthBuffer::GroupUnknow )
        {
            // Unlike D3D9, OGL doesn't allow sharing the main depth buffer, so keep them separate.
            // Only Copy does, but Copy means only one depth buffer...
            GLContext *windowContext = dynamic_cast<GLRenderTarget*>(win)->getContext();
            GLES2DepthBuffer *depthBuffer = N_new GLES2DepthBuffer( DepthBuffer::GroupInner, this,
                                                            windowContext, 0, 0,
                                                            win->getWidth(), win->getHeight(),
                                                            win->getFSAA(), true );

            mDepthBufferPool[depthBuffer->getPoolId()].push_back( depthBuffer );

            win->attachDepthBuffer( depthBuffer );
        }

        return win;
    }

    //---------------------------------------------------------------------
    DepthBuffer* GLES2RenderSystem::_createDepthBufferFor( RenderTarget *renderTarget )
    {
        GLES2DepthBuffer *retVal = 0;

        if( auto fbo = dynamic_cast<GLRenderTarget*>(renderTarget)->getFBO() )
        {
            // Presence of an FBO means the manager is an FBO Manager, that's why it's safe to downcast
            // Find best depth & stencil format suited for the RT's format
            GLuint depthFormat, stencilFormat;
            static_cast<GLES2FBOManager*>(mRTTManager)->getBestDepthStencil( fbo->getFormat(),
                                                                        &depthFormat, &stencilFormat );

            GLES2RenderBuffer *depthBuffer = N_new GLES2RenderBuffer( depthFormat, fbo->getWidth(),
                                                                fbo->getHeight(), fbo->getFSAA() );

            GLES2RenderBuffer *stencilBuffer = NULL;
            if (depthFormat == GL_DEPTH32F_STENCIL8 || depthFormat == GL_DEPTH24_STENCIL8_OES)
            {
                // If we have a packed format, the stencilBuffer is the same as the depthBuffer
                stencilBuffer = depthBuffer;
            }
            else if(stencilFormat)
            {
                stencilBuffer = new GLES2RenderBuffer( stencilFormat, fbo->getWidth(),
                                                       fbo->getHeight(), fbo->getFSAA() );
            }

            // No "custom-quality" multisample for now in GL
            retVal = N_new GLES2DepthBuffer( 0, this, mCurrentContext, depthBuffer, stencilBuffer,
                                        fbo->getWidth(), fbo->getHeight(), fbo->getFSAA(), false );
        }

        return retVal;
    }

    MultiRenderTarget* GLES2RenderSystem::createMultiRenderTarget(const String & name)
    {
        MultiRenderTarget* retval =
            new GLES2FBOMultiRenderTarget(static_cast<GLES2FBOManager*>(mRTTManager), name);
        attachRenderTarget(*retval);
        return retval;
    }

    void GLES2RenderSystem::destroyWindow(const String& name)
    {
        // Find it to remove from list.
        RenderTarget* pWin = detachRenderTarget(name);
        OgreAssert(pWin, "unknown RenderWindow name");

        _destroyDepthBuffer(pWin);
        N_delete pWin;
    }

    void GLES2RenderSystem::_destroyDepthBuffer(RenderTarget* pWin)
    {
        GLContext *windowContext = dynamic_cast<GLRenderTarget*>(pWin)->getContext();
        
        // 1 Window <-> 1 Context, should be always true
        N_assert( windowContext );
        
        bool bFound = false;
        // Find the depth buffer from this window and remove it.
        DepthBufferList::iterator itMap = mDepthBufferPool.begin();
        DepthBufferList::iterator enMap = mDepthBufferPool.end();
        
        while( itMap != enMap && !bFound )
        {
            DepthBufferVec::iterator itor = itMap->second.begin();
            DepthBufferVec::iterator end  = itMap->second.end();
            
            while( itor != end )
            {
                // A DepthBuffer with no depth & stencil pointers is a dummy one,
                // look for the one that matches the same GL context
                GLES2DepthBuffer *depthBuffer = static_cast<GLES2DepthBuffer*>(*itor);
                GLContext *glContext = depthBuffer->getGLContext();
                
                if( glContext == windowContext &&
                   (depthBuffer->getDepthBuffer() || depthBuffer->getStencilBuffer()) )
                {
                    bFound = true;
                    
                    delete *itor;
                    itMap->second.erase( itor );
                    break;
                }
                ++itor;
            }
            
            ++itMap;
        }
    }

    void GLES2RenderSystem::_bindTexture(size_t stage, bool enabled, const TexturePtr &texPtr)
    {
        if (!mState->activateGLTextureUnit(stage))
            return;

        if (enabled)
        {
            GLES2TexturePtr tex = static_pointer_cast<GLES2Texture>(texPtr);

            mCurTexMipCount = 0;

            // Note used
            tex->ref();
            mTextureTypes[stage] = tex->getGLES2TextureTarget();
            mCurTexMipCount = tex->getMipmapCount();

            mState->bindGLTexture(mTextureTypes[stage], tex->getGLID());
        }
        else
        {
            // Bind zero texture
            mState->bindGLTexture(GL_TEXTURE_2D, 0);
        }
    }

    void GLES2RenderSystem::_setSampler(size_t unit, TextureSampleUnit& sampler)
    {
        if (!mState->activateGLTextureUnit(unit))
            return;

        GLenum target = mTextureTypes[unit];

        const TextureSampleUnit::UVWAddressingMode& uvw = sampler.getAddressingMode();
        mState->setTexParameteri(target, GL_TEXTURE_WRAP_S, getTextureAddressingMode(uvw.u));
        mState->setTexParameteri(target, GL_TEXTURE_WRAP_T, getTextureAddressingMode(uvw.v));
        if(getCapabilities()->hasCapability(RSC_TEXTURE_3D))
            mState->setTexParameteri(target, GL_TEXTURE_WRAP_R, getTextureAddressingMode(uvw.w));

        if ((uvw.u == TAM_BORDER || uvw.v == TAM_BORDER || uvw.w == TAM_BORDER) && checkGpuApi("GL_EXT_texture_border_clamp"))
            GLCheck(glTexParameterfv( target, GL_TEXTURE_BORDER_COLOR_EXT, sampler.getBorderColour().ptr()));

        // only via shader..
        // GLCheck(glTexParameterf(target, GL_TEXTURE_LOD_BIAS, sampler.getTextureMipmapBias()));

        if (mFeature->hasCapability(RSC_ANISOTROPY))
            mState->setTexParameteri(
                target, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                std::min<uint>(getCapabilities()->getMaxSupportedAnisotropy(), sampler.getAnisotropy()));

        if(hasMinGLVersion(3, 0))
        {
            mState->setTexParameteri(target, GL_TEXTURE_COMPARE_MODE,
                                                 sampler.getCompareEnabled() ? GL_COMPARE_REF_TO_TEXTURE
                                                                                    : GL_NONE);
            if(sampler.getCompareEnabled())
                mState->setTexParameteri(target, GL_TEXTURE_COMPARE_FUNC,
                                                 convertCompareFunction(sampler.getCompareFunction()));
        }

        // Combine with existing mip filter
        mState->setTexParameteri(
            target, GL_TEXTURE_MIN_FILTER,
            getCombinedMinMipFilter(sampler.getFiltering(FT_MIN), sampler.getFiltering(FT_MIP)));

        switch (sampler.getFiltering(FT_MAG))
        {
        case FO_ANISOTROPIC: // GL treats linear and aniso the same
        case FO_LINEAR:
            mState->setTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case FO_POINT:
        case FO_NONE:
            mState->setTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        }
    }

    GLint GLES2RenderSystem::getTextureAddressingMode(TextureAddressingMode tam) const
    {
        switch (tam)
        {
            case ShaderChTextureUnit::TAM_CLAMP:
            case ShaderChTextureUnit::TAM_BORDER:
                return GL_CLAMP_TO_EDGE;
            case ShaderChTextureUnit::TAM_MIRROR:
                return GL_MIRRORED_REPEAT;
            case ShaderChTextureUnit::TAM_WRAP:
            default:
                return GL_REPEAT;
        }
    }

    void GLES2RenderSystem::_setTextureAddressingMode(size_t stage, const TextureSampleUnit::UVWAddressingMode& uvw)
    {
        if (!mState->activateGLTextureUnit(stage))
            return;

        mState->setTexParameteri(mTextureTypes[stage], GL_TEXTURE_WRAP_S, getTextureAddressingMode(uvw.u));
        mState->setTexParameteri(mTextureTypes[stage], GL_TEXTURE_WRAP_T, getTextureAddressingMode(uvw.v));

        if(getCapabilities()->hasCapability(RSC_TEXTURE_3D))
            mState->setTexParameteri(mTextureTypes[stage], GL_TEXTURE_WRAP_R_OES, getTextureAddressingMode(uvw.w));
    }

    void GLES2RenderSystem::_setLineWidth(float width)
    {
        GLCheck(glLineWidth(width));
    }

    GLenum GLES2RenderSystem::getBlendMode(ColourFactor ogreBlend) const
    {
        switch (ogreBlend)
        {
            case SBF_ONE:
                return GL_ONE;
            case SBF_ZERO:
                return GL_ZERO;
            case SBF_DEST_COLOUR:
                return GL_DST_COLOR;
            case SBF_SOURCE_COLOUR:
                return GL_SRC_COLOR;
            case SBF_ONE_MINUS_DEST_COLOUR:
                return GL_ONE_MINUS_DST_COLOR;
            case SBF_ONE_MINUS_SOURCE_COLOUR:
                return GL_ONE_MINUS_SRC_COLOR;
            case SBF_DEST_ALPHA:
                return GL_DST_ALPHA;
            case SBF_SOURCE_ALPHA:
                return GL_SRC_ALPHA;
            case SBF_ONE_MINUS_DEST_ALPHA:
                return GL_ONE_MINUS_DST_ALPHA;
            case SBF_ONE_MINUS_SOURCE_ALPHA:
                return GL_ONE_MINUS_SRC_ALPHA;
        };

        // To keep compiler happy
        return GL_ONE;
    }

    void GLES2RenderSystem::_setSeparateSceneBlending(
        ColourFactor sourceFactor, ColourFactor destFactor,
        ColourFactor sourceFactorAlpha, ColourFactor destFactorAlpha,
        ColourBlendMode op, ColourBlendMode alphaOp )
    {
        GLenum sourceBlend = getBlendMode(sourceFactor);
        GLenum destBlend = getBlendMode(destFactor);
        GLenum sourceBlendAlpha = getBlendMode(sourceFactorAlpha);
        GLenum destBlendAlpha = getBlendMode(destFactorAlpha);
        
        if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO && 
           sourceFactorAlpha == SBF_ONE && destFactorAlpha == SBF_ZERO)
        {
            mState->setDisabled(GL_BLEND);
        }
        else
        {
            mState->setEnabled(GL_BLEND);
            mState->setBlendFunc(sourceBlend, destBlend, sourceBlendAlpha, destBlendAlpha);
        }
        
        GLint func = GL_FUNC_ADD, alphaFunc = GL_FUNC_ADD;
        
        switch(op)
        {
            case SBO_ADD:
                func = GL_FUNC_ADD;
                break;
            case SBO_SUBTRACT:
                func = GL_FUNC_SUBTRACT;
                break;
            case SBO_REVERSE_SUBTRACT:
                func = GL_FUNC_REVERSE_SUBTRACT;
                break;
            case SBO_MIN:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    func = GL_MIN_EXT;
                break;
            case SBO_MAX:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    func = GL_MAX_EXT;
                break;
        }
        
        switch(alphaOp)
        {
            case SBO_ADD:
                alphaFunc = GL_FUNC_ADD;
                break;
            case SBO_SUBTRACT:
                alphaFunc = GL_FUNC_SUBTRACT;
                break;
            case SBO_REVERSE_SUBTRACT:
                alphaFunc = GL_FUNC_REVERSE_SUBTRACT;
                break;
            case SBO_MIN:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    alphaFunc = GL_MIN_EXT;
                break;
            case SBO_MAX:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    alphaFunc = GL_MAX_EXT;
                break;
        }
        
        mState->setBlendEquation(func, alphaFunc);
    }

    void GLES2RenderSystem::_setAlphaRejectSettings(CmpMode func, unsigned char value, bool alphaToCoverage)
    {
        if (getCapabilities()->hasCapability(RSC_ALPHA_TO_COVERAGE))
        {
            if ((func != CMPF_ALWAYS_PASS) && alphaToCoverage)
                mState->setEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
            else
                mState->setDisabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
        }
    }

    void GLES2RenderSystem::_setViewport(Viewport *vp)
    {
        // Check if viewport is different
        if (!vp)
        {
            mActiveViewport = NULL;
            _setRenderTarget(NULL);
        }
        else if (vp != mActiveViewport || !vp->isSyncRender())
        {
            RenderTarget* target;
            
            target = vp->getTarget();
            _setRenderTarget(target);
            mActiveViewport = vp;
            
            GLsizei x, y, w, h;
            
            // Calculate the "lower-left" corner of the viewport
            w = vp->getPixelWidth();
            h = vp->getPixelHeight();
            x = vp->getPixelLeft();
            y = vp->getPixelTop();
            
            if (!target->isFlipping())
            {
                // Convert "upper-left" corner to "lower-left"
                y = target->getHeight() - h - y;
            }
            
#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
            ConfigOptionMap::const_iterator opt;
            ConfigOptionMap::const_iterator end = mGLSupport->getConfig().end();
            
            if ((opt = mGLSupport->getConfig().find("Orientation")) != end)
            {
                String val = opt->second.currentValue;
                String::size_type pos = val.find("Landscape");
                
                if (pos != String::npos)
                {
                    GLsizei temp = h;
                    h = w;
                    w = temp;
                }
            }
#endif
            
            mState->setViewport(x, y, w, h);

            // Configure the viewport clipping
            GLCheck(glScissor(x, y, w, h));
            mScissorBox[0] = x;
            mScissorBox[1] = y;
            mScissorBox[2] = w;
            mScissorBox[3] = h;

            vp->syncRenderImpl();
        }
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        else
        {
            // On iOS RenderWindow is FBO based, renders to multisampled FBO and then resolves
            // to non-multisampled FBO, therefore we need to restore FBO binding even when
            // rendering to the same viewport.
            RenderTarget* target = vp->getTarget();
            mRTTManager->bind(target);
        }
#endif
    }

    void GLES2RenderSystem::_beginFrame(void)
    {
        if (!mActiveViewport)
            N_EXCEPT(Exception::ERR_INVALID_STATE,
                        "Cannot begin frame - no viewport selected.",
                        "GLES2RenderSystem::_beginFrame");

        mState->setEnabled(GL_SCISSOR_TEST);
    }

    void GLES2RenderSystem::_endFrame(void)
    {
        // Deactivate the viewport clipping.
        mState->setDisabled(GL_SCISSOR_TEST);

        // unbind GPU programs at end of frame
        // this is mostly to avoid holding bound programs that might get deleted
        // outside via the resource manager
        unbindGpuProgram(GPT_VERTEX_PROGRAM);
		unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
    }

    void GLES2RenderSystem::_setCullingMode(CullingMode mode)
    {
        mCullingMode = mode;
        // NB: Because two-sided stencil API dependence of the front face, we must
        // use the same 'winding' for the front face everywhere. As the OGRE default
        // culling mode is clockwise, we also treat anticlockwise winding as front
        // face for consistently. On the assumption that, we can't change the front
        // face by glFrontFace anywhere.

        GLenum cullMode;

        switch( mode )
        {
            case CM_None:
                mState->setDisabled(GL_CULL_FACE);
                return;

            default:
            case CM_Clockwise:
                if (mActiveRenderTarget &&
                    ((mActiveRenderTarget->isFlipping() && !mInvertVertexWinding) ||
                     (!mActiveRenderTarget->isFlipping() && mInvertVertexWinding)))
                {
                    cullMode = GL_FRONT;
                }
                else
                {
                    cullMode = GL_BACK;
                }
                break;
            case CM_Anticlockwise:
                if (mActiveRenderTarget && 
                    ((mActiveRenderTarget->isFlipping() && !mInvertVertexWinding) ||
                    (!mActiveRenderTarget->isFlipping() && mInvertVertexWinding)))
                {
                    cullMode = GL_BACK;
                }
                else
                {
                    cullMode = GL_FRONT;
                }
                break;
        }

        mState->setEnabled(GL_CULL_FACE);
        mState->setCullFace(cullMode);
    }

    void GLES2RenderSystem::_setDepthBufferParams(bool depthTest, bool depthWrite, CmpMode depthFunction)
    {
        _setDepthBufferCheckEnabled(depthTest);
        _setDepthBufferWriteEnabled(depthWrite);
        _setDepthBufferFunction(depthFunction);
    }

    void GLES2RenderSystem::_setDepthBufferCheckEnabled(bool enabled)
    {
        if (enabled)
        {
            mState->setClearDepth(1.0f);
            mState->setEnabled(GL_DEPTH_TEST);
        }
        else
        {
            mState->setDisabled(GL_DEPTH_TEST);
        }
    }

    void GLES2RenderSystem::_setDepthBufferWriteEnabled(bool enabled)
    {
        // Store for reference in _beginFrame
        mState->setDepthMask(enabled ? GL_TRUE : GL_FALSE);
    }

    void GLES2RenderSystem::_setDepthBufferFunction(CmpMode func)
    {
        mState->setDepthFunc(convertCompareFunction(func));
    }

    void GLES2RenderSystem::_setDepthBias(float constantBias, float slopeScaleBias)
    {
        if (constantBias != 0 || slopeScaleBias != 0)
        {
            mState->setEnabled(GL_POLYGON_OFFSET_FILL);
            GLCheck(glPolygonOffset(-slopeScaleBias, -constantBias));
        }
        else
        {
            mState->setDisabled(GL_POLYGON_OFFSET_FILL);
        }
    }

    void GLES2RenderSystem::_setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
    {
        mState->setColourMask(red, green, blue, alpha);
    }

    //---------------------------------------------------------------------
    PixelCountQuery* GLES2RenderSystem::create(void)
    {
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_occlusion_query_boolean"))
        {
            GLES2HardwareOcclusionQuery* ret = new GLES2HardwareOcclusionQuery(); 
            mPixelQueryList.push_back(ret);
            return ret;
        }
        else
        {
            return NULL;
        }
    }

    void GLES2RenderSystem::_setPolygonMode(PolygonMode level)
    {
        switch(level)
        {
        case PM_POINTS:
            mPolygonMode = GL_POINTS;
            break;
        case PM_WIREFRAME:
            mPolygonMode = GL_LINE_STRIP;
            break;
        default:
        case PM_SOLID:
            mPolygonMode = GL_FILL;
            break;
        }
    }

    void GLES2RenderSystem::setStencilCheckEnabled(bool enabled)
    {
        if (enabled)
        {
            mState->setEnabled(GL_STENCIL_TEST);
        }
        else
        {
            mState->setDisabled(GL_STENCIL_TEST);
        }
    }

    void GLES2RenderSystem::setStencil(CmpMode func,
                                                uint32 refValue, uint32 compareMask, uint32 writeMask,
                                                StencilOpType stencilFailOp,
                                                StencilOpType depthFailOp,
                                                StencilOpType passOp,
                                                bool twoSidedOperation,
                                                bool readBackAsTexture)
    {
        bool flip = false;

        if (twoSidedOperation)
        {
            if (!mFeature->hasCapability(RSC_TWO_SIDED_STENCIL))
                N_EXCEPT(Exception::ERR_INVALIDPARAMS, "2-sided stencils are not supported",
                            "GLES2RenderSystem::setStencil");
            
            // NB: We should always treat CCW as front face for consistent with default
            // culling mode. Therefore, we must take care with two-sided stencil settings.
            flip = (mInvertVertexWinding && !mActiveRenderTarget->isFlipping()) ||
            (!mInvertVertexWinding && mActiveRenderTarget->isFlipping());
            // Back
            GLCheck(glStencilMaskSeparate(GL_BACK, writeMask));
            GLCheck(glStencilFuncSeparate(GL_BACK, convertCompareFunction(func), refValue, compareMask));
            GLCheck(glStencilOpSeparate(GL_BACK,
                                                    convertStencilOp(stencilFailOp, !flip),
                                                    convertStencilOp(depthFailOp, !flip), 
                                                    convertStencilOp(passOp, !flip)));

            // Front
            GLCheck(glStencilMaskSeparate(GL_FRONT, writeMask));
            GLCheck(glStencilFuncSeparate(GL_FRONT, convertCompareFunction(func), refValue, compareMask));
            GLCheck(glStencilOpSeparate(GL_FRONT,
                                                    convertStencilOp(stencilFailOp, flip),
                                                    convertStencilOp(depthFailOp, flip), 
                                                    convertStencilOp(passOp, flip)));
        }
        else
        {
            flip = false;
            mState->setStencilMask(writeMask);
            GLCheck(glStencilFunc(convertCompareFunction(func), refValue, compareMask));
            GLCheck(glStencilOp(
                                            convertStencilOp(stencilFailOp, flip),
                                            convertStencilOp(depthFailOp, flip), 
                                            convertStencilOp(passOp, flip)));
        }
    }

    void GLES2RenderSystem::_setTextureUnitFiltering(size_t unit, TextureFilterOP minFilter,
                TextureFilterOP magFilter, TextureFilterOP mipFilter)
    {       
        mMipFilter = mipFilter;
        if(mCurTexMipCount == 0 && mMipFilter != FO_NONE)
        {
            mMipFilter = FO_NONE;           
        }
        _setTextureUnitFiltering(unit, FT_MAG, magFilter);
        _setTextureUnitFiltering(unit, FT_MIN, minFilter);
    }
                
    void GLES2RenderSystem::_setTextureUnitFiltering(size_t unit, FilterType ftype, TextureFilterOP fo)
    {
        if (!mState->activateGLTextureUnit(unit))
            return;

        switch (ftype)
        {
            case FT_MIN:
                mMinFilter = fo;
                // Combine with existing mip filter
                mState->setTexParameteri(mTextureTypes[unit],
                                GL_TEXTURE_MIN_FILTER,
                                getCombinedMinMipFilter(mMinFilter, mMipFilter));
                break;
            case FT_MAG:
                switch (fo)
                {
                    case FO_ANISOTROPIC: // GL treats linear and aniso the same
                    case FO_LINEAR:
                        mState->setTexParameteri(mTextureTypes[unit],
                                        GL_TEXTURE_MAG_FILTER,
                                        GL_LINEAR);
                        break;
                    case FO_POINT:
                    case FO_NONE:
                        mState->setTexParameteri(mTextureTypes[unit],
                                        GL_TEXTURE_MAG_FILTER,
                                        GL_NEAREST);
                        break;
                }
                break;
            case FT_MIP:
                mMipFilter = fo;

                // Combine with existing min filter
                mState->setTexParameteri(mTextureTypes[unit],
                                                     GL_TEXTURE_MIN_FILTER,
                                                     getCombinedMinMipFilter(mMinFilter, mMipFilter));
                
                break;
        }
    }

    void GLES2RenderSystem::_render(const GeometryRaw& op)
    {
        // Call super class
        RenderSystem::_render(op);

        HardwareVertexBufferSharedPtr globalInstanceVertexBuffer;
        VertexDeclaration * globalVertexDeclaration = 0;
        bool hasInstanceData = false;
        size_t numberOfInstances = 0;
        if(getCapabilities()->hasCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA))
        {
            globalInstanceVertexBuffer = getInstanceBuffer();
            globalVertexDeclaration = getInstanceData();
            hasInstanceData = (op.useGlobalInstancingVertexBufferIsAvailable &&
                                globalInstanceVertexBuffer && (globalVertexDeclaration != NULL))
                                || op.vertexData->vertexBufferBinding->hasInstanceData();

            numberOfInstances = op.mInstanceCount;

            if (op.useGlobalInstancingVertexBufferIsAvailable)
            {
                numberOfInstances *= getInstanceCount();
            }
        }

        void * pBufferData = 0;

        GLVertexArrayObject* vao = static_cast<GLVertexArrayObject*>(op);

        //bool updateVAO = true;
        if(isSupport(RSC_VAO))
        {
            vao->bind(this);
            //updateVAO = vao->needsUpdate(op.mVertex->mBindingList, op.mVertex->mOffset);
        }

        //if (updateVAO)
        //    vao->bindToGpu(this, op.mVertex->mBindingList, op.mVertex->mOffset);

        // We treat index buffer binding inside VAO as volatile, always updating and never relying onto it,
        // as one shared vertex buffer could be rendered with several index buffers, from submeshes and/or LODs
        if (op.useIndexes)
            mState->bindGLBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLES2IndexBuffer*>(op.indexData->indexBuffer.get())->getGLBufferId());


        if (getCapabilities()->hasCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA)
            && globalInstanceVertexBuffer && globalVertexDeclaration)
        {
            VertexDeclaration::VertexElementList::const_iterator elemIter, elemEnd;
            elemEnd = globalVertexDeclaration->getElements().end();
            for (elemIter = globalVertexDeclaration->getElements().begin(); elemIter != elemEnd;
                 ++elemIter)
            {
                const VertexElement& elem = *elemIter;
                bindVertexElementToGpu(elem, globalInstanceVertexBuffer, 0);
            }
        }

        // Find the correct type to render
        GLint primType;
        switch (op.mType)
        {
            case GeometryRaw::OT_POINT_LIST:
                primType = GL_POINTS;
                break;
            case GeometryRaw::OT_LINE_LIST:
                primType = GL_LINES;
                break;
            case GeometryRaw::OT_LINE_STRIP:
                primType = GL_LINE_STRIP;
                break;
            default:
            case GeometryRaw::OT_TRIANGLE_LIST:
                primType = GL_TRIANGLES;
                break;
            case GeometryRaw::OT_TRIANGLE_STRIP:
                primType = GL_TRIANGLE_STRIP;
                break;
            case GeometryRaw::OT_TRIANGLE_FAN:
                primType = GL_TRIANGLE_FAN;
                break;
        }

        GLenum polyMode = mPolygonMode;
        if (op.useIndexes)
        {
            pBufferData = VBO_BUFFER_OFFSET(op.indexData->indexStart * op.indexData->indexBuffer->getIndexSize());

            GLenum indexType = (op.indexData->indexBuffer->getType() == IndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

            do
            {
                // Update derived depth bias
                if (mAutoDepthBias && mCurrentRender > 0)
                {
                    _setDepthBias(mDepthBiasBase + mDepthBiasFactor * mCurrentRender, mDepthBiasSlopeScale);
                }

                if(hasInstanceData && getCapabilities()->hasCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA))
                {
                    GLCheck(glDrawElementsInstancedEXT((polyMode == GL_FILL) ? primType : polyMode, static_cast<GLsizei>(op.indexData->indexCount), indexType, pBufferData, static_cast<GLsizei>(numberOfInstances)));
                }
                else
                {
                    GLCheck(glDrawElements((polyMode == GL_FILL) ? primType : polyMode, static_cast<GLsizei>(op.indexData->indexCount), indexType, pBufferData));
                }

            } while (updateParam());
        }
        else
        {
            do
            {
                // Update derived depth bias
                if (mAutoDepthBias && mCurrentRender > 0)
                {
                    _setDepthBias(mDepthBiasBase + mDepthBiasFactor * mCurrentRender, mDepthBiasSlopeScale);
                }

                if(getCapabilities()->hasCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA) && hasInstanceData)
                {
                    GLCheck(glDrawArraysInstancedEXT((polyMode == GL_FILL) ? primType : polyMode, 0, static_cast<GLsizei>(op.vertexData->vertexCount), static_cast<GLsizei>(numberOfInstances)));
                }
                else
                {
                    GLCheck(glDrawArrays((polyMode == GL_FILL) ? primType : polyMode, 0, static_cast<GLsizei>(op.vertexData->vertexCount)));
                }
            } while (updateParam());
        }

        if(getCapabilities()->hasCapability(RSC_VAO))
        {
            // Do not unbind the vertex array object if VAOs are supported
            // VAOs > 0 are selected each time before usage
            // VAO #0 WOULD BE explicitly selected by Ogre before usage
        }
        else // VAOs are not supported, we should clear scratch 'VAO #0'
        {
            // Unbind all attributes
            for(std::vector<GLuint>::iterator ai = mRenderAttribsBound.begin(); ai != mRenderAttribsBound.end(); ++ai)
                GLCheck(glDisableVertexAttribArray(*ai));

            // Unbind any instance attributes
            for (std::vector<GLuint>::iterator ai = mRenderInstanceAttribsBound.begin(); ai != mRenderInstanceAttribsBound.end(); ++ai)
                GLCheck(glVertexAttribDivisorEXT(*ai, 0));
        }
        mRenderAttribsBound.clear();
        mRenderInstanceAttribsBound.clear();
    }

    void GLES2RenderSystem::setScissorTest(bool enabled, size_t left,
                                        size_t top, size_t right,
                                        size_t bottom)
    {
        // If request texture flipping, use "upper-left", otherwise use "lower-left"
        bool flipping = mActiveRenderTarget->isFlipping();
        //  GL measures from the bottom, not the top
        size_t targetHeight = mActiveRenderTarget->getHeight();
        // Calculate the "lower-left" corner of the viewport
        size_t w, h, x, y;

        if (enabled)
        {
            mState->setEnabled(GL_SCISSOR_TEST);
            // NB GL uses width / height rather than right / bottom
            x = left;
            if (flipping)
                y = top;
            else
                y = targetHeight - bottom;
            w = right - left;
            h = bottom - top;
            GLCheck(glScissor(static_cast<GLsizei>(x),
                                          static_cast<GLsizei>(y),
                                          static_cast<GLsizei>(w),
                                          static_cast<GLsizei>(h)));
        }
        else
        {
            mState->setDisabled(GL_SCISSOR_TEST);
            // GL requires you to reset the scissor when disabling
            w = mActiveViewport->getPixelWidth();
            h = mActiveViewport->getPixelHeight();
            x = mActiveViewport->getPixelLeft();
            if (flipping)
                y = mActiveViewport->getPixelTop();
            else
                y = targetHeight - mActiveViewport->getPixelTop() - h;
            GLCheck(glScissor(static_cast<GLsizei>(x),
                                          static_cast<GLsizei>(y),
                                          static_cast<GLsizei>(w),
                                          static_cast<GLsizei>(h)));
        }
    }

    void GLES2RenderSystem::clearBuffer(unsigned int buffers,
                                            const Colour& colour,
                                            Real depth, unsigned short stencil)
    {
        uchar* colourWrite = mState->getColourMask();
        bool colourMask = !colourWrite[0] || !colourWrite[1] ||
                          !colourWrite[2] || !colourWrite[3];
        GLuint stencilMask = mState->getStencilMask();
        GLbitfield flags = 0;

        if (buffers & FBT_COLOUR)
        {
            flags |= GL_COLOR_BUFFER_BIT;
            // Enable buffer for writing if it isn't
            if (colourMask)
            {
                mState->setColourMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }
            mState->setClearColour(colour.r, colour.g, colour.b, colour.a);
        }
        if (buffers & FBT_DEPTH)
        {
            flags |= GL_DEPTH_BUFFER_BIT;
            // Enable buffer for writing if it isn't
            mState->setDepthMask(GL_TRUE);
            mState->setClearDepth(depth);
        }
        if (buffers & FBT_STENCIL)
        {
            flags |= GL_STENCIL_BUFFER_BIT;
            // Enable buffer for writing if it isn't
            mState->setStencilMask(0xFFFFFFFF);
            GLCheck(glClearStencil(stencil));
        }

        // Should be enable scissor test due the clear region is
        // relied on scissor box bounds.
         mState->setEnabled(GL_SCISSOR_TEST);

        // Sets the scissor box as same as viewport
        GLint viewport[4];
        mState->getViewport(viewport);
        bool scissorBoxDifference =
            viewport[0] != mScissorBox[0] || viewport[1] != mScissorBox[1] ||
            viewport[2] != mScissorBox[2] || viewport[3] != mScissorBox[3];
        if (scissorBoxDifference)
        {
            GLCheck(glScissor(viewport[0], viewport[1], viewport[2], viewport[3]));
        }

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        static_cast<EAGLES2Context*>(mCurrentContext)->mDiscardBuffers = buffers;
#endif

        // Clear buffers
        GLCheck(glClear(flags));

        // Restore scissor box
        if (scissorBoxDifference)
        {
            GLCheck(glScissor(mScissorBox[0], mScissorBox[1], mScissorBox[2], mScissorBox[3]));
        }

        // Restore scissor test
        mState->setDisabled(GL_SCISSOR_TEST);

        // Reset buffer write state
        if (!mState->getDepthMask() && (buffers & FBT_DEPTH))
        {
            mState->setDepthMask(GL_FALSE);
        }

        if (colourMask && (buffers & FBT_COLOUR))
        {
            mState->setColourMask(colourWrite[0], colourWrite[1], colourWrite[2], colourWrite[3]);
        }

        if (buffers & FBT_STENCIL)
        {
            mState->setStencilMask(stencilMask);
        }
    }

    void GLES2RenderSystem::_switchContext(GLContext *context)
    {
        // Unbind GPU programs and rebind to new context later, because
        // scene manager treat render system as ONE 'context' ONLY, and it
        // cached the GPU programs using state.
        if (mVS)
            mVS->unbindProgram();
        if (mFS)
            mFS->unbindProgram();
        
        // Disable textures
        _disableTextureUnitsFrom(0);

        // It's ready for switching
        if (mCurrentContext!=context)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            // EAGLContext::setCurrentContext does not flush automatically. everybody else does.
            // see https://developer.apple.com/library/content/qa/qa1612/_index.html
            glFlush();
#endif
            mCurrentContext->endCurrent();
            mCurrentContext = context;
        }
        mCurrentContext->setCurrent();

        mState = mCurrentContext->getState<GLES2StateCacheManager>();
        _completeDeferredVaoFboDestruction();

        // Check if the context has already done one-time initialisation
        if (!mCurrentContext->getInitialized())
        {
            _oneTimeContextInitialization();
            mCurrentContext->setInitialized();
        }

        // Rebind GPU programs to new context
        if (mVS)
            mVS->bindProgram();
        if (mFS)
            mFS->bindProgram();
        
        // Must reset depth/colour write mask to according with user desired, otherwise,
        // clearBuffer would be wrong because the value we are recorded may be
        // difference with the really state stored in GL context.
        uchar* colourWrite = mState->getColourMask();
        GLuint stencilMask = mState->getStencilMask();
        GLboolean depthMask = mState->getDepthMask();
        mState->setStencilMask(stencilMask);
        mState->setColourMask(colourWrite[0], colourWrite[1], colourWrite[2], colourWrite[3]);
        mState->setDepthMask(depthMask);
    }

    void GLES2RenderSystem::_unregisterContext(GLContext *context)
    {
        if(GBufferManager::getSingletonPtr())
            static_cast<GLES2HardwareBufferManager*>(GBufferManager::getSingletonPtr())->notifyContextDestroyed(context);
        
        for(RenderTargetMap::iterator it = mRenderTargets.begin(); it!=mRenderTargets.end(); ++it)
        {
            if(auto target = dynamic_cast<GLRenderTarget*>(it->second))
            {
                if(auto fbo = target->getFBO())
                    fbo->notifyContextDestroyed(context);
            }
        }
        
        if (mCurrentContext == context)
        {
            // Change the context to something else so that a valid context
            // remains active. When this is the main context being unregistered,
            // we set the main context to 0.
            if (mCurrentContext != mMainContext)
            {
                _switchContext(mMainContext);
            }
            else
            {
                // No contexts remain
                mCurrentContext->endCurrent();
                mCurrentContext = 0;
                mMainContext = 0;
                mState = 0;
            }
        }
    }

    uint32 GLES2RenderSystem::_createVao()
    {
        uint32 vao = 0;
        if(getCapabilities()->hasCapability(RSC_VAO))
            GLCheck(glGenVertexArraysOES(1, &vao));
        return vao;
    }

    void GLES2RenderSystem::_destroyVao(GLContext* context, uint32 vao)
    {
        if(context != mCurrentContext)
            context->_getVaoDeferredForDestruction().push_back(vao);
        else
            GLCheck(glDeleteVertexArraysOES(1, &vao));
    }
    
    void GLES2RenderSystem::_destroyFbo(GLContext* context, uint32 fbo)
    {
        if(context != mCurrentContext)
            context->_getFboDeferredForDestruction().push_back(fbo);
        else
            GLCheck(glDeleteFramebuffers(1, &fbo));
    }

    void GLES2RenderSystem::_bindVao(GLContext* context, uint32 vao)
    {
        OgreAssert(context == mCurrentContext, "VAO used in wrong OpenGL context");
        getStateManager()->bindGLVertexArray(vao);
    }

    void GLES2RenderSystem::_oneTimeContextInitialization()
    {
        mState->setDisabled(GL_DITHER);

        if(!getCapabilities()->hasCapability(RSC_PRIMITIVE_RESTART))
            return;

        // Enable primitive restarting with fixed indices depending upon the data type
        // https://www.khronos.org/registry/webgl/specs/latest/2.0/#NO_PRIMITIVE_RESTART_FIXED_INDEX
#if OGRE_PLATFORM != OGRE_PLATFORM_EMSCRIPTEN
        GLCheck(glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX));
#endif
    }

    void GLES2RenderSystem::initialiseContext(RenderWindow* primary)
    {
        // Set main and current context
        mMainContext = dynamic_cast<GLRenderTarget*>(primary)->getContext();
        mCurrentContext = mMainContext;

        // Set primary context as active
        if (mCurrentContext)
            mCurrentContext->setCurrent();

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        // ios: EAGL2Support redirects to glesw for get_proc. Overwriting it there would create an infinite loop
        // android: eglGetProcAddress fails in some cases (e.g. Virtual Device), whereas dlsym always works.
        if (glGetError == NULL && gleswInit())
#else
        if (gleswInit2(get_proc))
#endif
        {
            N_EXCEPT(RenderingAPI,
                        "Could not initialize glesw",
                        "GLES2RenderSystem::initialiseContext");
        }

        // Setup GLSupport
        initialiseExtensions();

        mState = mCurrentContext->getState<GLES2StateCacheManager>();

        if(hasMinGLVersion(3, 0)) {
            gl2ext_to_gl3core();
            GLES2PixelUtil::useSizedFormats();
        }

        LogManager::getSingleton().logMessage("**************************************");
        LogManager::getSingleton().logMessage("*** OpenGL ES 2.x Renderer Started ***");
        LogManager::getSingleton().logMessage("**************************************");
    }

    void GLES2RenderSystem::_setRenderTarget(RenderTarget *target)
    {
        mActiveRenderTarget = target;
        if (target && mRTTManager)
        {
            // Switch context if different from current one
            GLContext *newContext = dynamic_cast<GLRenderTarget*>(target)->getContext();
            if (newContext && mCurrentContext != newContext)
            {
                _switchContext(newContext);
            }

            // Check the FBO's depth buffer status
            GLES2DepthBuffer *depthBuffer = static_cast<GLES2DepthBuffer*>(target->getDepthBuffer());

            if( target->getDepthBufferPool() != DepthBuffer::GroupUnknow &&
                (!depthBuffer || depthBuffer->getGLContext() != mCurrentContext ) )
            {
                // Depth is automatically managed and there is no depth buffer attached to this RT
                // or the Current context doesn't match the one this Depth buffer was created with
                setDepthBufferFor( target );
            }

            // Bind frame buffer object
            mRTTManager->bind(target);
        }
    }

    GLint GLES2RenderSystem::convertCompareFunction(CmpMode func) const
    {
        switch(func)
        {
            case CMPF_ALWAYS_FAIL:
                return GL_NEVER;
            case CMPF_ALWAYS_PASS:
                return GL_ALWAYS;
            case CMPF_LESS:
                return GL_LESS;
            case CMPF_LESS_EQUAL:
                return GL_LEQUAL;
            case CMPF_EQUAL:
                return GL_EQUAL;
            case CMPF_NOT_EQUAL:
                return GL_NOTEQUAL;
            case CMPF_GREATER_EQUAL:
                return GL_GEQUAL;
            case CMPF_GREATER:
                return GL_GREATER;
        };
        // To keep compiler happy
        return GL_ALWAYS;
    }

    GLint GLES2RenderSystem::convertStencilOp(StencilOpType op, bool invert) const
    {
        switch(op)
        {
        case SOP_KEEP:
            return GL_KEEP;
        case SOP_ZERO:
            return GL_ZERO;
        case SOP_REPLACE:
            return GL_REPLACE;
        case SOP_INCREMENT:
            return invert ? GL_DECR : GL_INCR;
        case SOP_DECREMENT:
            return invert ? GL_INCR : GL_DECR;
        case SOP_INCREMENT_WRAP:
            return invert ? GL_DECR_WRAP : GL_INCR_WRAP;
        case SOP_DECREMENT_WRAP:
            return invert ? GL_INCR_WRAP : GL_DECR_WRAP;
        case SOP_INVERT:
            return GL_INVERT;
        };
        // to keep compiler happy
        return SOP_KEEP;
    }

    //---------------------------------------------------------------------
    void GLES2RenderSystem::bindGpuProgram(GpuProgram* prg)
    {
        if (!prg)
        {
            N_EXCEPT(RenderingAPI, 
                        "Null program bound.",
                        "GLES2RenderSystem::bindGpuProgram");
        }
        
        GLSLESProgram* glprg = static_cast<GLSLESProgram*>(prg);
        
        // Unbind previous gpu program first.
        //
        // Note:
        //  1. Even if both previous and current are the same object, we can't
        //     bypass re-bind completely since the object itself may be modified.
        //     But we can bypass unbind based on the assumption that object
        //     internally GL program type shouldn't be changed after it has
        //     been created. The behavior of bind to a GL program type twice
        //     should be same as unbind and rebind that GL program type, even
        //     for different objects.
        //  2. We also assumed that the program's type (vertex or fragment) should
        //     not be changed during it's in using. If not, the following switch
        //     statement will confuse GL state completely, and we can't fix it
        //     here. To fix this case, we must coding the program implementation
        //     itself, if type is changing (during load/unload, etc), and it's in use,
        //     unbind and notify render system to correct for its state.
        //
        switch (glprg->getType())
        {
            case GPT_VERTEX_PROGRAM:
                if (mVS != glprg)
                {
                    if (mVS)
                        mVS->unbindProgram();
                    mVS = glprg;
                }
                break;
                
            case GPT_FRAGMENT_PROGRAM:
                if (mFS != glprg)
                {
                    if (mFS)
                        mFS->unbindProgram();
                    mFS = glprg;
                }
                break;
            default:
                break;
        }
        
        // Bind the program
        glprg->bindProgram();

        RenderSystem::bindGpuProgram(prg);
    }

    void GLES2RenderSystem::unbindGpuProgram(GpuProgramType gptype)
    {
        if (gptype == GPT_VERTEX_PROGRAM && mVS)
        {
            mActiveVGPP.reset();
            mVS->unbindProgram();
            mVS = 0;
        }
        else if (gptype == GPT_FRAGMENT_PROGRAM && mFS)
        {
            mActiveFGPP.reset();
            mFS->unbindProgram();
            mFS = 0;
        }
        RenderSystem::unbindGpuProgram(gptype);
    }

    void GLES2RenderSystem::on(GpuProgramType gptype, const GpuProgramParam *& params, uint16 mask)
    {
        // Just copy
        params->syncValue();
        switch (gptype)
        {
            case GPT_VERTEX_PROGRAM:
                mActiveVGPP = params;
                mVS->bindProgramSharedParameters(params, mask);
                break;
            case GPT_FRAGMENT_PROGRAM:
                mActiveFGPP = params;
                mFS->bindProgramSharedParameters(params, mask);
                break;
            default:
                break;
        }

        switch (gptype)
        {
            case GPT_VERTEX_PROGRAM:
                mActiveVGPP = params;
                mVS->bindProgramParameters(params, mask);
                break;
            case GPT_FRAGMENT_PROGRAM:
                mActiveFGPP = params;
                mFS->bindProgramParameters(params, mask);
                break;
            default:
                break;
        }
    }

    void GLES2RenderSystem::bindGpuProgramPassIterationParameters(GpuProgramType gptype)
    {
        switch (gptype)
        {
            case GPT_VERTEX_PROGRAM:
                mVS->bindProgramPassIterationParameters(mActiveVGPP);
                break;
            case GPT_FRAGMENT_PROGRAM:
                mFS->bindProgramPassIterationParameters(mActiveFGPP);
                break;
            default:
                break;
        }
    }

    void GLES2RenderSystem::registerThread()
    {
        OGRE_LOCK_MUTEX(mThreadInitMutex);
        // This is only valid once we've created the main context
        if (!mMainContext)
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Cannot register a background thread before the main context "
                        "has been created.",
                        "GLES2RenderSystem::registerThread");
        }

        // Create a new context for this thread. Cloning from the main context
        // will ensure that resources are shared with the main context
        // We want a separate context so that we can safely create GL
        // objects in parallel with the main thread
        GLContext* newContext = mMainContext->clone();
        mBackgroundContextList.push_back(newContext);

        // Bind this new context to this thread.
        newContext->setCurrent();

        _oneTimeContextInitialization();
        newContext->setInitialized();
    }

    void GLES2RenderSystem::unregisterThread()
    {
        // nothing to do here?
        // Don't need to worry about active context, just make sure we delete
        // on shutdown.
    }

    void GLES2RenderSystem::preExtraThreadsStarted()
    {
        OGRE_LOCK_MUTEX(mThreadInitMutex);
        // free context, we'll need this to share lists
        if(mCurrentContext)
            mCurrentContext->endCurrent();
    }

    void GLES2RenderSystem::postExtraThreadsStarted()
    {
        OGRE_LOCK_MUTEX(mThreadInitMutex);
        // reacquire context
        if(mCurrentContext)
            mCurrentContext->setCurrent();
    }

    unsigned int GLES2RenderSystem::getDisplayMonitorCount() const
    {
        return 1;
    }

    //---------------------------------------------------------------------
    void GLES2RenderSystem::beginProfiler( const String &eventName )
    {
        if(getCapabilities()->hasCapability(RSC_DEBUG))
            glPushGroupMarkerEXT(0, eventName.c_str());
    }
    //---------------------------------------------------------------------
    void GLES2RenderSystem::endProfiler( void )
    {
        if(getCapabilities()->hasCapability(RSC_DEBUG))
            glPopGroupMarkerEXT();
    }
    //---------------------------------------------------------------------
    void GLES2RenderSystem::markProfiler( const String &eventName )
    {
        if( eventName.empty() )
            return;

        if(getCapabilities()->hasCapability(RSC_DEBUG))
           glInsertEventMarkerEXT(0, eventName.c_str());
    }
    //---------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    void GLES2RenderSystem::notifyOnContextLost() {
        static_cast<GLES2HardwareBufferManager*>(GBufferManager::getSingletonPtr())->notifyContextDestroyed(mCurrentContext);
        GLES2RenderSystem::mResourceManager->notifyOnContextLost();
    }

    void GLES2RenderSystem::resetRenderer(RenderWindow* win)
    {
        LogManager::getSingleton().logMessage("********************************************");
        LogManager::getSingleton().logMessage("*** OpenGL ES 2.x Reset Renderer Started ***");
        LogManager::getSingleton().logMessage("********************************************");
                
        initialiseContext(win);
        
        static_cast<GLES2FBOManager*>(mRTTManager)->_reload();
        
        _destroyDepthBuffer(win);
        
        GLES2DepthBuffer *depthBuffer = N_new GLES2DepthBuffer( DepthBuffer::GroupInner, this,
                                                                  mMainContext, 0, 0,
                                                                  win->getWidth(), win->getHeight(),
                                                                  win->getFSAA(), true );
        
        mDepthBufferPool[depthBuffer->getPoolId()].push_back( depthBuffer );
        win->attachDepthBuffer( depthBuffer );
        
        GLES2RenderSystem::mResourceManager->notifyOnContextReset();
        
        mState->clearCache();
        _setViewport(NULL);
        _setRenderTarget(win);
    }
    
    GLES2ManagedResourceManager* GLES2RenderSystem::getResourceManager()
    {
        return GLES2RenderSystem::mResourceManager;
    }
#endif

    void GLES2RenderSystem::bindVertexElementToGpu(
        const VertexElement& elem, const HardwareVertexBufferSharedPtr& vertexBuffer,
        const size_t vertexStart)
    {
        VertexType sem = elem.getSemantic();
        unsigned short elemIndex = elem.getIndex();

        const GLES2VertexBuffer* hwGlBuffer = static_cast<const GLES2VertexBuffer*>(vertexBuffer.get());

        mState->bindGLBuffer(GL_ARRAY_BUFFER, hwGlBuffer->getGLBufferId());
        void* pBufferData = VBO_BUFFER_OFFSET(elem.getOffset() + vertexStart * vertexBuffer->getVertexSize());

        unsigned short typeCount = VertexElement::getTypeCount(elem.getType());
        GLboolean normalised = GL_FALSE;
        GLuint attrib = 0;

        attrib = (GLuint)GLSLProgramBase::getFixedAttributeIndex(sem, elemIndex);

        if(getCapabilities()->hasCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA))
        {
            if (mVS)
            {
                if (hwGlBuffer->isInstanceData())
                {
                    GLCheck(glVertexAttribDivisorEXT(attrib, static_cast<GLuint>(hwGlBuffer->getInstanceDataStepRate())));
                    mRenderInstanceAttribsBound.push_back(attrib);
                }
            }
        }

        switch(elem.getType())
        {
            case DT_Colour:
            case DT_Colour_ABGR:
            case DT_Colour_ARGB:
                // Because GL takes these as a sequence of single unsigned bytes, count needs to be 4
                // VertexElement::getTypeCount treats them as 1 (RGBA)
                // Also need to normalise the fixed-point data
                typeCount = 4;
                normalised = GL_TRUE;
                break;
            case VET_UBYTE4_NORM:
            case VET_SHORT2_NORM:
            case VET_USHORT2_NORM:
            case VET_SHORT4_NORM:
            case VET_USHORT4_NORM:
                normalised = GL_TRUE;
                break;
            default:
                break;
        };

        GLCheck(glVertexAttribPointer(attrib,
                                                  typeCount,
                                                  GLES2HardwareBufferManager::getGLType(elem.getType()),
                                                  normalised,
                                                  static_cast<GLsizei>(vertexBuffer->getVertexSize()),
                                                  pBufferData));

        GLCheck(glEnableVertexAttribArray(attrib));
        mRenderAttribsBound.push_back(attrib);
    }

    void GLES2RenderSystem::_copyContentsToMemory(Viewport* vp, const Box& src, const PixelBox& dst,
                                                  RenderWindow::FrameBuffer buffer) {
        GLenum format = GLES2PixelUtil::getGLOriginFormat(dst.format);
        GLenum type = GLES2PixelUtil::getGLOriginDataType(dst.format);

        if (dst.format != PF_BYTE_RGBA)
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                "Unsupported format.",
                "GLES2RenderSystem::_copyContentsToMemory" );
        }

        bool hasPackImage = hasMinGLVersion(3, 0) || checkGpuApi("GL_NV_pack_subimage");
        OgreAssert(dst.getWidth() == dst.rowPitch || hasPackImage, "GL_PACK_ROW_LENGTH not supported");

        // Switch context if different from current one
        _setViewport(vp);

        GLCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        if(dst.getWidth() != dst.rowPitch && hasPackImage)
            glPixelStorei(GL_PACK_ROW_LENGTH_NV, dst.rowPitch);

        // Must change the packing to ensure no overruns!
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        if(hasMinGLVersion(3, 0)) {
            glReadBuffer((buffer == RenderWindow::FB_FRONT) ? GL_FRONT : GL_BACK);
        }

        uint32_t height = vp->getTarget()->getHeight();

        glReadPixels((GLint)src.left, (GLint)(height - src.bottom),
                     (GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(),
                     format, type, dst.getData());

        // restore default alignment
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glPixelStorei(GL_PACK_ROW_LENGTH_NV, 0);

        PixelUtil::bulkPixelVerticalFlip(dst);
    }

    void GLES2RenderSystem::initialiseExtensions(void)
    {
        String tmpStr;
#if 1
        // Set version string
        const GLubyte* pcVer = glGetString(GL_VERSION);
        N_assert(pcVer && "Problems getting GL version string using glGetString");
        tmpStr = (const char*)pcVer;

        // format explained here:
        // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetString.xml
        size_t offset = sizeof("OpenGL ES ") - 1;
        if(tmpStr.length() > offset) {
            mVersion.fromString(tmpStr.substr(offset, tmpStr.find(' ', offset)));
        }
#else
        // GLES3 way, but should work with ES2 as well, so disabled for now
        glGetIntegerv(GL_MAJOR_VERSION, &mVersion.major);
        glGetIntegerv(GL_MINOR_VERSION, &mVersion.minor);
#endif

        LogManager::getSingleton().logMessage("GL_VERSION = " + mVersion.toString());


        // Get vendor
        const GLubyte* pcVendor = glGetString(GL_VENDOR);
        tmpStr = (const char*)pcVendor;
        LogManager::getSingleton().logMessage("GL_VENDOR = " + tmpStr);
        mVendor = tmpStr.substr(0, tmpStr.find(' '));

        // Get renderer
        const GLubyte* pcRenderer = glGetString(GL_RENDERER);
        tmpStr = (const char*)pcRenderer;
        LogManager::getSingleton().logMessage("GL_RENDERER = " + tmpStr);

        // Set extension list
        StringStream ext;
        String str;

        const GLubyte* pcExt = glGetString(GL_EXTENSIONS);
        OgreAssert(pcExt, "Problems getting GL extension string using glGetString");
        ext << pcExt;

        Log::Stream log = LogManager::getSingleton().stream();
        log << "GL_EXTENSIONS = ";
        while (ext >> str)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
            // emscripten gives us both prefixed (GL_) and unprefixed (EXT_) forms
            // use prefixed form (GL_EXT) unless its a WebGL extension (WEBGL_)
            if ((str.substr(0, 3) != "GL_" && str.substr(0, 6) != "WEBGL_") || str.substr(0, 9) == "GL_WEBGL_")
                continue;
#endif
            log << str << " ";
            mExtensionList.insert(str);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    GLES2RenderSystem::GLES2RenderSystem()
        : mLargestSupportedAnisotropy(0),
          mBlendChannelMask( ShaderChBlend::BlendChannelAll ),
          mDepthWrite(true),
          mScissorsEnabled(false),
          mGlobalVao(0),
          mCurrentVertexBuffer(0),
          mCurrentIndexBuffer(0),
          mCurrentPolygonMode( GL_TRIANGLES ),
          mGpuProgramManager(0),
          mGLSLESShaderFactory(0),
          mHardwareBufferManager(0),
          mRTTManager(0),
          mNullColourFramebuffer( 0 ),
          mHasDiscardFramebuffer(false),
          mCurTexMipCount(0)
    {
        LogManager::getSingleton().logMessage(getName() + " created.");

        mRenderAttribsBound.reserve(100);
#if OGRE_NO_GLES3_SUPPORT == 0
        mRenderInstanceAttribsBound.reserve(100);
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
        mResourceManager = OGRE_NEW GLES2ManagedResourceManager();
#endif
        
        mGLSupport = getGLSupport();
        
        mWorldMatrix = Matrix4::IDENTITY;
        mViewMatrix = Matrix4::IDENTITY;

        mGLSupport->addConfig();

        for( size_t i = 0; i < NII_MAX_TextureLayer; i++ )
        {
            // Dummy value
            mTextureTypes[i] = 0;
        }

        mActiveRenderTarget = 0;
        mCurrentContext = 0;
        mMainContext = 0;
        mGLInitialised = false;
        mUseAdjacency = false;
        mMinFilter = FO_LINEAR;
        mMipFilter = FO_POINT;
        mIndirectPtr = 0;
        mPso = 0;
    }

    GLES2RenderSystem::~GLES2RenderSystem()
    {
        shutdown();

        // Destroy render windows
        RenderTargetMap::iterator i;
        for (i = mRenderTargets.begin(); i != mRenderTargets.end(); ++i)
        {
            OGRE_DELETE i->second;
        }

        mRenderTargets.clear();
        OGRE_DELETE mGLSupport;
        mGLSupport = 0;

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
        if (mResourceManager != NULL)
        {
            OGRE_DELETE mResourceManager;
            mResourceManager = NULL;
        }
#endif
    }

    const String& GLES2RenderSystem::getName(void) const
    {
        static String strName("OpenGL ES 2.x Rendering Subsystem");
        return strName;
    }

    const String& GLES2RenderSystem::getFriendlyName(void) const
    {
        static String strName("OpenGL ES 2.x");
        return strName;
    }

    ConfigOptionMap& GLES2RenderSystem::getConfig(void)
    {
        return mGLSupport->getConfig();
    }

    void GLES2RenderSystem::setConfig(const String &name, const String &value)
    {
        mGLSupport->setConfig(name, value);
    }

    String GLES2RenderSystem::checkConfig(void)
    {
        // XXX Return an error string if something is invalid
        return mGLSupport->validateConfig();
    }

    bool GLES2RenderSystem::hasMinGLVersion(int major, int minor) const
    {
        return mGLSupport->hasMinGLVersion(major, minor);
    }

    bool GLES2RenderSystem::checkGpuApi(const String& ext) const
    {
        return mGLSupport->checkGpuApi(ext);
    }

    RenderWindow* GLES2RenderSystem::init(bool autoCreateWindow,
                                                 const String& windowTitle)
    {
        mGLSupport->start();

        // Create the texture manager
        mTextureManager = OGRE_NEW GLES2TextureManager(*mGLSupport); 

        RenderWindow *autoWindow = mGLSupport->createWindow(autoCreateWindow,
                                                            this, windowTitle);
        RenderSystem::init(autoCreateWindow, windowTitle);
        return autoWindow;
    }

    RenderFeature* GLES2RenderSystem::setupFeature() const
    {
        RenderFeature* rsc = OGRE_NEW RenderFeature();

        rsc->setCategoryRelevant(CAPS_CATEGORY_GL, true);
        rsc->setDriverVersion(mVersion);

        const char* deviceName = (const char*)glGetString(GL_RENDERER);
        if (deviceName)
        {
            rsc->setDeviceName(deviceName);
        }

        rsc->setRenderSystemName(getName());
        rsc->parseVendorFromString(mGLSupport->getGLVendor());

        // Check for hardware mipmapping support.
        bool disableAutoMip = false;
#if OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
        if (rsc->getVendor() == Ogre::GPU_MOZILLA)
            disableAutoMip = true;
#endif
        if (!disableAutoMip)
            rsc->setCapability(RSC_AUTOMIPMAP);

        // Check for blending support
        rsc->setCapability(RSC_BLENDING);

        // Multitexturing support and set number of texture units
        GLint units;
        OGRE_CHECK_GL_ERROR(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &units));
        rsc->setMaxTexUnit(std::min<ushort>(16, units));

        // Check for Anisotropy support
        if (checkGpuApi("GL_EXT_texture_filter_anisotropic"))
        {
            // Max anisotropy
            GLfloat maxAnisotropy = 0;
            OGRE_CHECK_GL_ERROR(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy));
            rsc->setMaxSupportedAnisotropy(maxAnisotropy);
            rsc->setCapability(RSC_ANISOTROPY);
        }

        // DOT3 support is standard
        rsc->setCapability(RSC_DOT3);
        
        // Cube map
        rsc->setCapability(RSC_CUBEMAPPING);

        // Point sprites
        rsc->setCapability(RSC_POINT_SPRITES);
        rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
        
        // Check for hardware stencil support and set bit depth
        GLint stencil;
        OGRE_CHECK_GL_ERROR(glGetIntegerv(GL_STENCIL_BITS, &stencil));
        if(stencil)
        {
            rsc->setCapability(RSC_HWSTENCIL);
            rsc->setCapability(RSC_TWO_SIDED_STENCIL);
            rsc->setStencilBufferBitDepth(stencil);
        }

#if OGRE_NO_GLES3_SUPPORT == 0
        //Extension support is disabled as it is a bit buggy and hard to support
        if (hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_sRGB"))
        {
            rsc->setCapability( RSC_HW_GAMMA );
        }
#endif

#if OGRE_NO_GLES3_SUPPORT == 0
        rsc->setCapability( RSC_TEXTURE_SIGNED_INT );
#endif
        
        // Vertex Buffer Objects are always supported by OpenGL ES
        rsc->setCapability(RSC_VBO);
        if(checkGpuApi("GL_OES_element_index_uint"))
            rsc->setCapability(RSC_32BIT_INDEX);

#if OGRE_NO_GLES2_VAO_SUPPORT == 0
#   if OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_vertex_array_object") || emscripten_get_compiler_setting("LEGACY_GL_EMULATION"))
#   else
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_vertex_array_object"))
#   endif
            rsc->setCapability(RSC_VAO);
#endif

        // Check for texture compression
        // OpenGL ES - Check for these extensions too
        // For 2.0, http://www.khronos.org/registry/gles/api/2.0/gl2ext.h
        if (checkGpuApi("GL_IMG_texture_compression_pvrtc") ||
            checkGpuApi("GL_EXT_texture_compression_dxt1") ||
            checkGpuApi("GL_EXT_texture_compression_s3tc") ||
            checkGpuApi("GL_OES_compressed_ETC1_RGB8_texture") ||
            checkGpuApi("GL_AMD_compressed_ATC_texture") ||
            checkGpuApi("WEBGL_compressed_texture_s3tc") ||
            checkGpuApi("WEBGL_compressed_texture_atc") ||
            checkGpuApi("WEBGL_compressed_texture_pvrtc") ||
            checkGpuApi("WEBGL_compressed_texture_etc1"))

        {
            rsc->setCapability(RSC_TEXTURE_COMPRESSION);

            if(checkGpuApi("GL_IMG_texture_compression_pvrtc") ||
               checkGpuApi("GL_IMG_texture_compression_pvrtc2") ||
               checkGpuApi("WEBGL_compressed_texture_pvrtc"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_PVRTC);
                
            if((checkGpuApi("GL_EXT_texture_compression_dxt1") &&
               checkGpuApi("GL_EXT_texture_compression_s3tc")) ||
               checkGpuApi("WEBGL_compressed_texture_s3tc"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);

            if(checkGpuApi("GL_OES_compressed_ETC1_RGB8_texture") ||
               checkGpuApi("WEBGL_compressed_texture_etc1"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_ETC1);

            if(hasMinGLVersion(3, 0))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_ETC2);

            if(checkGpuApi("GL_AMD_compressed_ATC_texture") ||
               checkGpuApi("WEBGL_compressed_texture_atc"))
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_ATC);
        }

        rsc->setCapability(RSC_FBO);
        rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
#if OGRE_NO_GLES3_SUPPORT == 0
        // Probe number of draw buffers
        // Only makes sense with FBO support, so probe here
        GLint buffers;
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &buffers);
        rsc->setNumMultiRenderTargets(std::min<int>(buffers, (GLint)NII_MAX_RenderTarget));
        rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);
#else
        rsc->setNumMultiRenderTargets(1);
#endif

        // Stencil wrapping
        rsc->setCapability(RSC_STENCIL_WRAP);

        // GL always shares vertex and fragment texture units (for now?)
        rsc->setVertexTextureUnitsShared(true);

        // Check for non-power-of-2 texture support
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_EMSCRIPTEN
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_texture_npot") || checkGpuApi("GL_ARB_texture_non_power_of_two"))
        {
            rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
            rsc->setNonPOW2TexturesLimited(false);
        }
        else
#endif
        {
            rsc->setNonPOW2TexturesLimited(true);
        }

        rsc->setCapability(RSC_TEXTURE_1D);
#if OGRE_NO_GLES3_SUPPORT == 0
        rsc->setCapability(RSC_TEXTURE_3D);
        rsc->setCapability(RSC_TEXTURE_2D_ARRAY);
#endif

        // UBYTE4 always supported
        rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

        // Infinite far plane always supported
        rsc->setCapability(RSC_INFINITE_FAR_PLANE);

        // Check for hardware occlusion support
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_occlusion_query_boolean"))
        {
            rsc->setCapability(RSC_HWOCCLUSION);
        }

        GLint maxRes2d, maxRes3d, maxResCube;
        OGRE_CHECK_GL_ERROR( glGetIntegerv( GL_MAX_TEXTURE_SIZE,            &maxRes2d ) );
#if OGRE_NO_GLES3_SUPPORT == 0
        OGRE_CHECK_GL_ERROR( glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE,         &maxRes3d ) );
#else
        maxRes3d = 0;
#endif
        OGRE_CHECK_GL_ERROR( glGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE,   &maxResCube ) );

        rsc->setMaxTexSize( static_cast<ushort>(maxRes2d), static_cast<ushort>(maxRes3d),
                                    static_cast<ushort>(maxResCube) );

        // Point size
        GLfloat psRange[2] = {0.0, 0.0};
        OGRE_CHECK_GL_ERROR(glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, psRange));
        rsc->setMaxPointSize(psRange[1]);
        // No point sprites, so no size
        rsc->setMaxPointSize(0.f);

        // GLSL ES is always supported in GL ES 2
        rsc->addShaderProfile("glsles");
        LogManager::getSingleton().logMessage("GLSL ES support detected");

#if !OGRE_NO_GLES2_CG_SUPPORT
        rsc->addShaderProfile("cg");
        rsc->addShaderProfile("ps_2_0");
        rsc->addShaderProfile("vs_2_0");
#endif

        // Vertex/Fragment Programs
        rsc->setCapability(RSC_VERTEX_PROGRAM);
        rsc->setCapability(RSC_FRAGMENT_PROGRAM);

        // Separate shader objects
#if OGRE_PLATFORM != OGRE_PLATFORM_NACL
        OGRE_IF_IOS_VERSION_IS_GREATER_THAN(5.0)
        if(0 && checkGpuApi("GL_EXT_separate_shader_objects"))
            rsc->setCapability(CF_Program_SeparateObject);
#endif

        // Separate shader objects don't work properly on Tegra
        if (rsc->getVendor() == GPU_NVIDIA)
            rsc->unsetCapability(CF_Program_SeparateObject);

        GLfloat floatConstantCount = 0;
#if OGRE_NO_GLES3_SUPPORT == 0
        glGetFloatv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &floatConstantCount);
#else
        glGetFloatv(GL_MAX_VERTEX_UNIFORM_VECTORS, &floatConstantCount);
#endif
        rsc->setVertexProgramConstantFloatCount((Ogre::ushort)floatConstantCount);
        rsc->setVertexProgramConstantBoolCount((Ogre::ushort)floatConstantCount);
        rsc->setVertexProgramConstantIntCount((Ogre::ushort)floatConstantCount);

        // Fragment Program Properties
        floatConstantCount = 0;
#if OGRE_NO_GLES3_SUPPORT == 0
        glGetFloatv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &floatConstantCount);
#else
        glGetFloatv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &floatConstantCount);
#endif
        rsc->setFragmentProgramConstantFloatCount((Ogre::ushort)floatConstantCount);
        rsc->setFragmentProgramConstantBoolCount((Ogre::ushort)floatConstantCount);
        rsc->setFragmentProgramConstantIntCount((Ogre::ushort)floatConstantCount);

#if OGRE_NO_GLES3_SUPPORT == 0
        if (hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_get_program_binary"))
        {
            GLint formats;
            OGRE_CHECK_GL_ERROR(glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats));

            if(formats > 0)
                rsc->setCapability(RSC_CAN_GET_COMPILED_SHADER_BUFFER);
        }
#endif

        if (hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_instanced_arrays"))
        {
            rsc->setCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA);
        }

        // Check for Float textures
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_texture_float") || checkGpuApi("GL_OES_texture_half_float"))
            rsc->setCapability(RSC_TEXTURE_FLOAT);

        // Alpha to coverage always 'supported' when MSAA is available
        // although card may ignore it if it doesn't specifically support A2C
        rsc->setCapability(RSC_ALPHA_TO_COVERAGE);
        
#if OGRE_NO_GLES3_SUPPORT == 0
        // Check if render to vertex buffer (transform feedback in OpenGL)
        rsc->setCapability(RSC_HWRENDER_TO_VERTEX_BUFFER);
#endif
        return rsc;
    }

    void GLES2RenderSystem::initImpl(RenderFeature* caps, RenderTarget* primary)
    {
        if(caps->getRenderSystemName() != getName())
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Trying to initialize GLES2RenderSystem from RenderFeature that do not support OpenGL ES",
                        "GLES2RenderSystem::initImpl");
        }

        mGpuProgramManager = OGRE_NEW GLES2GpuProgramManager();

        mGLSLESShaderFactory = OGRE_NEW GLSLESShaderFactory();
        HighLevelGpuProgramManager::getSingleton().addFactory(mGLSLESShaderFactory);

#if !OGRE_NO_GLES2_CG_SUPPORT
        mGLSLESCgProgramFactory = OGRE_NEW GLSLESCgProgramFactory();
        HighLevelGpuProgramManager::getSingleton().addFactory(mGLSLESCgProgramFactory);
#endif

        // Set texture the number of texture units
        mFixedFunctionTextureUnits = caps->getMaxTexUnit();

        // Use VBO's by default
        mHardwareBufferManager = OGRE_NEW v1::GLES2HardwareBufferManager();

        // Create FBO manager
        LogManager::getSingleton().logMessage("GL ES 2: Using FBOs for rendering to textures");
        mRTTManager = new GLES2FBOManager();
        caps->setCapability(RSC_RTT_SEPARATE_DEPTHBUFFER);

        Log* defaultLog = LogManager::getSingleton().getDefaultLog();
        if (defaultLog)
        {
            caps->log(defaultLog);
        }

        mGLInitialised = true;
    }

    void GLES2RenderSystem::reinit(void)
    {
        this->shutdown();
        this->init(true);
    }

    void GLES2RenderSystem::shutdown(void)
    {
        // Deleting the GLSL shader factory
        if (mGLSLESShaderFactory)
        {
            // Remove from manager safely
            if (HighLevelGpuProgramManager::getSingletonPtr())
                HighLevelGpuProgramManager::getSingleton().removeFactory(mGLSLESShaderFactory);
            OGRE_DELETE mGLSLESShaderFactory;
            mGLSLESShaderFactory = 0;
        }

#if !OGRE_NO_GLES2_CG_SUPPORT
        // Deleting the GLSL program factory
        if (mGLSLESCgProgramFactory)
        {
            // Remove from manager safely
            if (HighLevelGpuProgramManager::getSingletonPtr())
                HighLevelGpuProgramManager::getSingleton().removeFactory(mGLSLESCgProgramFactory);
            OGRE_DELETE mGLSLESCgProgramFactory;
            mGLSLESCgProgramFactory = 0;
        }
#endif
        // Deleting the GPU program manager and hardware buffer manager.  Has to be done before the mGLSupport->stop().
        OGRE_DELETE mGpuProgramManager;
        mGpuProgramManager = 0;

        OGRE_DELETE mHardwareBufferManager;
        mHardwareBufferManager = 0;

        delete mRTTManager;
        mRTTManager = 0;

        OGRE_DELETE mTextureManager;
        mTextureManager = 0;

        // Delete extra threads contexts
        for (GLES2ContextList::iterator i = mBackgroundContextList.begin();
             i != mBackgroundContextList.end(); ++i)
        {
            GLES2Context* pCurContext = *i;

            pCurContext->releaseContext();

            delete pCurContext;
        }

        mBackgroundContextList.clear();

        if( mNullColourFramebuffer )
        {
            GLCheck( glDeleteFramebuffers( 1, &mNullColourFramebuffer ) );
            mNullColourFramebuffer = 0;
        }

        RenderSystem::shutdown();

        mGLSupport->stop();

        if( mGlobalVao )
        {
            GLCheck( glBindVertexArray( 0 ) );
            GLCheck( glDeleteVertexArrays( 1, &mGlobalVao ) );
            mGlobalVao = 0;
        }

        mGLInitialised = 0;
    }

    RenderWindow* GLES2RenderSystem::createWindow(const String &name, unsigned int width, unsigned int height,
                                                        bool fullScreen, const PropertyData *miscParams)
    {
        if (mRenderTargets.find(name) != mRenderTargets.end())
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Window with name '" + name + "' already exists",
                        "GLES2RenderSystem::createWindow");
        }

        // Log a message
        StringStream ss;
        ss << "GLES2RenderSystem::createWindow \"" << name << "\", " <<
            width << "x" << height << " ";
        if (fullScreen)
            ss << "fullscreen ";
        else
            ss << "windowed ";

        if (miscParams)
        {
            ss << " miscParams: ";
            PropertyData::const_iterator it;
            for (it = miscParams->begin(); it != miscParams->end(); ++it)
            {
                ss << it->first << "=" << it->second << " ";
            }

            LogManager::getSingleton().logMessage(ss.str());
        }

        // Create the window
        RenderWindow* win = mGLSupport->newWindow(name, width, height, fullScreen, miscParams);
        attachRenderTarget((Ogre::RenderTarget&) *win);

        if (!mGLInitialised)
        {
            initialiseContext(win);
            
            mVersion = mGLSupport->getGLVersion();

            if( !mVersion.isOlder( 3, 0 ) )
            {
                N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "OpenGL ES 3.0 or greater required. Try updating your drivers.");
            }

            assert( !mBufferManager );
            mBufferManager = OGRE_NEW GLES2VaoManager();

            //Bind the Draw ID
            GLCheck( glGenVertexArrays( 1, &mGlobalVao ) );
            GLCheck( glBindVertexArray( mGlobalVao ) );
            static_cast<GLES2VaoManager*>( mBufferManager )->bindDrawId();
            GLCheck( glBindVertexArray( 0 ) );

            // Get the shader language version
            const char* shadingLangVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
            StringList tokens = StringUtil::split(shadingLangVersion, ". ");
            size_t i = 0;

            // iOS reports the GLSL version with a whole bunch of non-digit characters so we have to find where the version starts.
            for(; i < tokens.size(); i++)
            {
                if (isdigit(*tokens[i].c_str()))
                    break;
            }
            mShaderVersion = (StringConverter::parseUnsignedInt(tokens[i]) * 100) + StringConverter::parseUnsignedInt(tokens[i+1]);

            // Initialise GL after the first window has been created
            // TODO: fire this from emulation options, and don't duplicate Real and Current capabilities
            mVendorFeature = setupFeature();

            mHasDiscardFramebuffer = hasMinGLVersion(3, 0) || checkGpuApi( "GL_EXT_discard_framebuffer" );

            // use real capabilities if custom capabilities are not available
            if (mSysFeatureValid)
                mFeature = mVendorFeature;

            fireEvent("RenderSystemCapabilitiesCreated");

            initImpl(mFeature, (RenderTarget *) win);

            // Initialise the main context
            _oneTimeContextInitialization();
            if (mCurrentContext)
                mCurrentContext->setInitialized();
        }

        if( win->getDepthBufferPool() != DepthBuffer::GroupUnknow )
        {
            // Unlike D3D9, OGL doesn't allow sharing the main depth buffer, so keep them separate.
            // Only Copy does, but Copy means only one depth buffer...
            GLES2Context *windowContext = 0;
            win->getPlatformData( "GLCONTEXT", &windowContext );
            GLES2DepthBuffer *depthBuffer = OGRE_NEW GLES2DepthBuffer( DepthBuffer::GroupInner, this,
                                                            windowContext, GL_NONE, GL_NONE,
                                                            win->getWidth(), win->getHeight(),
                                                            win->getFSAA(), 0, PF_UNKNOWN,
                                                            false, true );

            mDepthBufferPool[depthBuffer->getPoolId()].push_back( depthBuffer );

            win->attachDepthBuffer( depthBuffer, false );
        }

        return win;
    }

    //---------------------------------------------------------------------
    DepthBuffer* GLES2RenderSystem::_createDepthBufferFor( RenderTarget *renderTarget, bool exactMatchFormat )
    {
        GLES2DepthBuffer *retVal = 0;

        // Only FBOs support different depth buffers, so everything
        // else creates dummy (empty) containers
        // retVal = mRTTManager->_createDepthBufferFor( renderTarget );
        GLES2FrameBufferObject *fbo = 0;
        renderTarget->getPlatformData("FBO", &fbo);

        if( fbo || renderTarget->getForceDisableColourWrites() )
        {
            PixelFormat desiredDepthBufferFormat = renderTarget->getDesiredDepthBufferFormat();

            if( !exactMatchFormat )
            {
                if( desiredDepthBufferFormat == PF_D24_UNORM_X8 && renderTarget->prefersDepthTexture() )
                    desiredDepthBufferFormat = PF_D24_UNORM;
                else
                    desiredDepthBufferFormat = PF_D24_UNORM_S8_UINT;
            }

            PixelFormat renderTargetFormat;

            if( fbo )
                renderTargetFormat = fbo->getFormat();
            else
            {
                //Deal with depth textures
                renderTargetFormat = desiredDepthBufferFormat;
            }

            // Presence of an FBO means the manager is an FBO Manager, that's why it's safe to downcast
            // Find best depth & stencil format suited for the RT's format
            GLuint depthFormat, stencilFormat;
            static_cast<GLES2FBOManager*>(mRTTManager)->getBestDepthStencil( desiredDepthBufferFormat,
                                                                             renderTargetFormat,
                                                                             &depthFormat,
                                                                             &stencilFormat );

            // OpenGL specs explicitly disallow depth textures with separate stencil.
            if( stencilFormat == GL_NONE || !renderTarget->prefersDepthTexture() )
            {
                // No "custom-quality" multisample for now in GL
                retVal = OGRE_NEW GLES2DepthBuffer( 0, this, mCurrentContext, depthFormat, stencilFormat,
                                                    renderTarget->getWidth(), renderTarget->getHeight(),
                                                    renderTarget->getFSAA(), 0,
                                                    desiredDepthBufferFormat,
                                                    renderTarget->prefersDepthTexture(), false );
            }
        }

        return retVal;
    }
    //---------------------------------------------------------------------
    void GLES2RenderSystem::_getDepthStencilFormatFor( GLenum internalColourFormat, GLenum *depthFormat,
                                                      GLenum *stencilFormat )
    {
        mRTTManager->getBestDepthStencil( internalColourFormat, depthFormat, stencilFormat );
    }

    MultiRenderTarget* GLES2RenderSystem::createMultiRenderTarget(const String & name)
    {
        MultiRenderTarget *retval = mRTTManager->createMultiRenderTarget(name);
        attachRenderTarget(*retval);
        return retval;
    }

    void GLES2RenderSystem::destroyWindow(RenderWindow* pWin)
    {
        // Find it to remove from list.
        RenderTargetMap::iterator i = mRenderTargets.begin();

        while (i != mRenderTargets.end())
        {
            if (i->second == pWin)
            {
                _destroyDepthBuffer(pWin);
                mRenderTargets.erase(i);
                OGRE_DELETE pWin;
                break;
            }
        }
    }

    void GLES2RenderSystem::_destroyDepthBuffer(RenderWindow* pWin)
    {
        GLES2Context *windowContext = 0;
        pWin->getPlatformData("GLCONTEXT", &windowContext);
        
        // 1 Window <-> 1 Context, should be always true
        assert( windowContext );
        
        bool bFound = false;
        // Find the depth buffer from this window and remove it.
        DepthBufferList::iterator itMap = mDepthBufferPool.begin();
        DepthBufferList::iterator enMap = mDepthBufferPool.end();
        
        while( itMap != enMap && !bFound )
        {
            DepthBufferVec::iterator itor = itMap->second.begin();
            DepthBufferVec::iterator end  = itMap->second.end();
            
            while( itor != end )
            {
                // A DepthBuffer with no depth & stencil pointers is a dummy one,
                // look for the one that matches the same GL context.
                GLES2DepthBuffer *depthBuffer = static_cast<GLES2DepthBuffer*>(*itor);
                GLES2Context *glContext = depthBuffer->getGLContext();
                
                if( glContext == windowContext &&
                   (depthBuffer->getDepthBuffer() || depthBuffer->getStencilBuffer()) )
                {
                    bFound = true;
                    
                    delete *itor;
                    itMap->second.erase( itor );
                    break;
                }
                ++itor;
            }
            
            ++itMap;
        }
    }
    
    String GLES2RenderSystem::getErrorString(long errorNumber) const
    {
        // TODO find a way to get error string
//        const GLubyte *errString = gluErrorString (errCode);
//        return (errString != 0) ? String((const char*) errString) : N_StrBlank;

        return N_StrBlank;
    }

    DataType GLES2RenderSystem::getColourType(void) const
    {
        return VET_COLOUR_ABGR;
    }

    void GLES2RenderSystem::_setModel(const Matrix4 &m)
    {
        mWorldMatrix = m;
    }

    void GLES2RenderSystem::_setView(const Matrix4 &m)
    {
        mViewMatrix = m;

        // Also mark clip planes dirty.
        if (!mClipPlanes.empty())
        {
            mClipValid = true;
        }
    }

    void GLES2RenderSystem::_setProj(const Matrix4 &m)
    {
        // Nothing to do but mark clip planes dirty.
        if (!mClipPlanes.empty())
            mClipValid = true;
    }

    void GLES2RenderSystem::_bindTexture(size_t stage, bool enabled, Texture *texPtr)
    {
        GLES2Texture *tex = static_cast<GLES2Texture*>(texPtr);

        if (!activateGLTextureUnit(stage))
            return;

        if (enabled)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
            mCurTexMipCount = 0;
#endif
            GLuint texID =  0;
            if (tex)
            {
                // Note used
                tex->ref();
                mTextureTypes[stage] = tex->getGLES2TextureTarget();
                texID = tex->getGLID();
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
                mCurTexMipCount = tex->getNumMipmaps();
#endif
            }
            else
            {
                // Assume 2D.
                mTextureTypes[stage] = GL_TEXTURE_2D;
                texID = static_cast<GLES2TextureManager*>(mTextureManager)->getWarningTextureID();
            }

            OGRE_CHECK_GL_ERROR(glBindTexture(mTextureTypes[stage], texID));
        }
        else
        {
            // Bind zero texture
            OGRE_CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
        }

        activateGLTextureUnit(0);
    }

    void GLES2RenderSystem::_setTextureCoordSet(size_t stage, size_t index)
    {
        mTextureCoordIndex[stage] = index;
    }

    GLint GLES2RenderSystem::getTextureAddressingMode(TextureAddressingMode tam) const
    {
    }

    void GLES2RenderSystem::queueBindUAV( uint32 slot, UavBufferPacked *buffer,
                                          Buffer::OpType access,
                                          size_t offset, size_t sizeBytes )
    {
    }

    void GLES2RenderSystem::clearUAVs(void)
    {
    }

    void GLES2RenderSystem::_bindTextureUavCS( uint32 slot, Texture *texture,
                                               Buffer::OpType _access,
                                               int32 mipmapLevel, int32 textureArrayIndex,
                                               PixelFormat pixelFormat )
    {
    }

    void GLES2RenderSystem::_setTextureCS( uint32 slot, bool enabled, Texture *texPtr )
    {
        this->_bindTexture( slot, enabled, texPtr );
    }

    void GLES2RenderSystem::_setHlmsSamplerblockCS( uint8 texUnit, const TextureSampleUnit *samplerblock )
    {
        this->_setTextureSample( texUnit, samplerblock );
    }

    GLint GLES2RenderSystem::getTextureAddressingMode(TextureAddressingMode tam) const
    {
        switch (tam)
        {
            case TAM_CLAMP:
            case TAM_BORDER:
                return GL_CLAMP_TO_EDGE;
            case TAM_MIRROR:
                return GL_MIRRORED_REPEAT;
            case TAM_WRAP:
            default:
                return GL_REPEAT;
        }
    }

    GLenum GLES2RenderSystem::getBlendMode(ColourFactor ogreBlend) const
    {
        switch (ogreBlend)
        {
            case SBF_ONE:
                return GL_ONE;
            case SBF_ZERO:
                return GL_ZERO;
            case SBF_DEST_COLOUR:
                return GL_DST_COLOR;
            case SBF_SOURCE_COLOUR:
                return GL_SRC_COLOR;
            case SBF_ONE_MINUS_DEST_COLOUR:
                return GL_ONE_MINUS_DST_COLOR;
            case SBF_ONE_MINUS_SOURCE_COLOUR:
                return GL_ONE_MINUS_SRC_COLOR;
            case SBF_DEST_ALPHA:
                return GL_DST_ALPHA;
            case SBF_SOURCE_ALPHA:
                return GL_SRC_ALPHA;
            case SBF_ONE_MINUS_DEST_ALPHA:
                return GL_ONE_MINUS_DST_ALPHA;
            case SBF_ONE_MINUS_SOURCE_ALPHA:
                return GL_ONE_MINUS_SRC_ALPHA;
        };

        // To keep compiler happy.
        return GL_ONE;
    }

    GLenum GLES2RenderSystem::getBlendOperation(ColourBlendMode op) const
    {
        switch( op )
        {
        case SBO_ADD:
            return GL_FUNC_ADD;
        case SBO_SUBTRACT:
            return GL_FUNC_SUBTRACT;
        case SBO_REVERSE_SUBTRACT:
            return GL_FUNC_REVERSE_SUBTRACT;
        case SBO_MIN:
            if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                return GL_MIN_EXT;
            break;
        case SBO_MAX:
            if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                return GL_MAX_EXT;
            break;
        }
        // To keep compiler happy.
        return GL_FUNC_ADD;
    }

    void GLES2RenderSystem::_setSceneBlending(ColourFactor sourceFactor, ColourFactor destFactor, ColourBlendMode op)
    {
        GLenum sourceBlend = getBlendMode(sourceFactor);
        GLenum destBlend = getBlendMode(destFactor);
        if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
        {
            OGRE_CHECK_GL_ERROR(glDisable(GL_BLEND));
        }
        else
        {
            OGRE_CHECK_GL_ERROR(glEnable(GL_BLEND));
            OGRE_CHECK_GL_ERROR(glBlendFunc(sourceBlend, destBlend));
        }
        
        GLint func = GL_FUNC_ADD;
        switch(op)
        {
        case SBO_ADD:
            func = GL_FUNC_ADD;
            break;
        case SBO_SUBTRACT:
            func = GL_FUNC_SUBTRACT;
            break;
        case SBO_REVERSE_SUBTRACT:
            func = GL_FUNC_REVERSE_SUBTRACT;
            break;
        case SBO_MIN:
            if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                func = GL_MIN_EXT;
            break;
        case SBO_MAX:
            if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                func = GL_MAX_EXT;
            break;
        }

        OGRE_CHECK_GL_ERROR(glBlendEquation(func));
    }

    void GLES2RenderSystem::_setSeparateSceneBlending(
        ColourFactor sourceFactor, ColourFactor destFactor,
        ColourFactor sourceFactorAlpha, ColourFactor destFactorAlpha,
        ColourBlendMode op, ColourBlendMode alphaOp )
    {
        GLenum sourceBlend = getBlendMode(sourceFactor);
        GLenum destBlend = getBlendMode(destFactor);
        GLenum sourceBlendAlpha = getBlendMode(sourceFactorAlpha);
        GLenum destBlendAlpha = getBlendMode(destFactorAlpha);
        
        if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO && 
           sourceFactorAlpha == SBF_ONE && destFactorAlpha == SBF_ZERO)
        {
            OGRE_CHECK_GL_ERROR(glDisable(GL_BLEND));
        }
        else
        {
            OGRE_CHECK_GL_ERROR(glEnable(GL_BLEND));
            OGRE_CHECK_GL_ERROR(glBlendFuncSeparate(sourceBlend, destBlend, sourceBlendAlpha, destBlendAlpha));
        }
        
        GLint func = GL_FUNC_ADD, alphaFunc = GL_FUNC_ADD;
        
        switch(op)
        {
            case SBO_ADD:
                func = GL_FUNC_ADD;
                break;
            case SBO_SUBTRACT:
                func = GL_FUNC_SUBTRACT;
                break;
            case SBO_REVERSE_SUBTRACT:
                func = GL_FUNC_REVERSE_SUBTRACT;
                break;
            case SBO_MIN:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    func = GL_MIN_EXT;
                break;
            case SBO_MAX:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    func = GL_MAX_EXT;
                break;
        }
        
        switch(alphaOp)
        {
            case SBO_ADD:
                alphaFunc = GL_FUNC_ADD;
                break;
            case SBO_SUBTRACT:
                alphaFunc = GL_FUNC_SUBTRACT;
                break;
            case SBO_REVERSE_SUBTRACT:
                alphaFunc = GL_FUNC_REVERSE_SUBTRACT;
                break;
            case SBO_MIN:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    alphaFunc = GL_MIN_EXT;
                break;
            case SBO_MAX:
                if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_blend_minmax"))
                    alphaFunc = GL_MAX_EXT;
                break;
        }
        
        OGRE_CHECK_GL_ERROR(glBlendEquationSeparate(func, alphaFunc));
    }

    void GLES2RenderSystem::correctViewport( GLint x, GLint &y, GLint &w, GLint &h, RenderTarget *renderTarget )
    {
        if( !renderTarget->isFlipping() )
        {
            // Convert "upper-left" corner to "lower-left"
            y = renderTarget->getHeight() - h - y;
        }

#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
        ConfigOptionMap::const_iterator opt;
        ConfigOptionMap::const_iterator end = mGLSupport->getConfig().end();

        if ((opt = mGLSupport->getConfig().find("Orientation")) != end)
        {
            String val = opt->second.currentValue;
            String::size_type pos = val.find("Landscape");

            if (pos != String::npos)
            {
                GLsizei temp = h;
                h = w;
                w = temp;
            }
        }
#endif
    }

    void GLES2RenderSystem::_setViewport(Viewport *vp)
    {
        // Check if viewport is different
        if (!vp)
        {
            mActiveViewport = NULL;
            _setRenderTarget(NULL, VP_RTT_COLOUR_WRITE);
        }
        else if (vp != mActiveViewport || !vp->isSyncRender())
        {
            RenderTarget* target;
            
            target = vp->getTarget();
            _setRenderTarget(target, vp->getViewportRenderTargetFlags());
            mActiveViewport = vp;
            
            GLsizei x, y, w, h;
            
            // Calculate the "lower-left" corner of the viewport
            w = vp->getPixelWidth();
            h = vp->getPixelHeight();
            x = vp->getPixelLeft();
            y = vp->getPixelTop();
            
            correctViewport( x, y, w, h, target );

            OGRE_CHECK_GL_ERROR(glViewport(x, y, w, h));

            w = vp->getClipPixelWidth();
            h = vp->getClipPixelHeight();
            x = vp->getClipPixelLeft();
            y = vp->getClipPixelTop();

            correctViewport( x, y, w, h, target );

            OGRE_CHECK_GL_ERROR(glScissor(x, y, w, h));
            
            vp->syncRenderImpl();
        }
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        else
        {
            // On iOS RenderWindow is FBO based, renders to multisampled FBO and then resolves
            // to non-multisampled FBO, therefore we need to restore FBO binding even when
            // rendering to the same viewport.
            RenderTarget* target = vp->getTarget();
            mRTTManager->bind(target);
        }
#endif
    }

    void GLES2RenderSystem::createRSO( RenderStateObject *newBlock )
    {
        GLES2HlmsPso *pso = new GLES2HlmsPso();
        memset( pso, 0, sizeof(GLES2HlmsPso) );
        
        //
        // Macroblock stuff
        //

        pso->depthWrite   = newBlock->mStencil->mDepthWrite ? GL_TRUE : GL_FALSE;
        pso->depthFunc    = convertCompareFunction( newBlock->mStencil->mDepthFunc );

        switch( newBlock->mStencil->mCullMode )
        {
        case CM_None:
            pso->cullMode = 0;
            break;
        default:
        case CM_Clockwise:
            pso->cullMode = GL_BACK;
            break;
        case CM_Anticlockwise:
            pso->cullMode = GL_FRONT;
            break;
        }

        switch( newBlock->mStencil->mPolygonMode )
        {
        case PM_POINTS:
            //pso->polygonMode = GL_POINTS;
            pso->polygonMode = GL_POINT;
            break;
        case PM_WIREFRAME:
            //pso->polygonMode = GL_LINE_STRIP;
            pso->polygonMode = GL_LINE;
            break;
        default:
        case PM_SOLID:
            pso->polygonMode = GL_FILL;
            break;
        }

        //
        // Blendblock stuff
        //
        pso->enableAlphaBlend = newBlock->mBlend->mSourceBlendFactor != SBF_ONE ||
                                newBlock->mBlend->mDestBlendFactor != SBF_ZERO;
        if( newBlock->mBlend->mSeparateBlend )
        {
            pso->enableAlphaBlend |= newBlock->mBlend->mSourceBlendFactorAlpha != SBF_ONE ||
                                     newBlock->mBlend->mDestBlendFactorAlpha != SBF_ZERO;
        }
        pso->sourceBlend        = getBlendMode( newBlock->mBlend->mSourceBlendFactor );
        pso->destBlend          = getBlendMode( newBlock->mBlend->mDestBlendFactor );
        pso->sourceBlendAlpha   = getBlendMode( newBlock->mBlend->mSourceBlendFactorAlpha );
        pso->destBlendAlpha     = getBlendMode( newBlock->mBlend->mDestBlendFactorAlpha );
        pso->blendFunc          = getBlendOperation( newBlock->mBlend->mBlendOperation );
        pso->blendFuncAlpha     = getBlendOperation( newBlock->mBlend->mBlendOperationAlpha );


        //
        // Shader stuff
        //

        if( !newBlock->mVS.isNull() )
        {
            pso->mVS = static_cast<GLSLESShader*>( newBlock->mVS->
                                                          getMain() );
        }
        if( !newBlock->mFS.isNull() )
        {
            pso->mFS = static_cast<GLSLESShader*>( newBlock->mFS->
                                                         getMain() );
        }
        
        newBlock->mExtData = pso;
    }

    void GLES2RenderSystem::destroyRSO( RenderStateObject *pso )
    {
        GLES2HlmsPso *glPso = reinterpret_cast<GLES2HlmsPso*>(pso->mExtData);
        delete glPso;
        pso->mExtData = 0;
    }

    void GLES2RenderSystem::createStencil( ShaderChStencil *newBlock )
    {
        //Set it to non-zero to get the assert in _setHlmsBlendblock
        //to work correctly (which is a very useful assert)
        newBlock->mRsData = reinterpret_cast<void*>( 1 );
    }

    void GLES2RenderSystem::destroyStencil( ShaderChStencil *block )
    {
        block->mRsData = 0;
    }

    void GLES2RenderSystem::createBlend( ShaderChBlend *newBlock )
    {
        //Set it to non-zero to get the assert in _setHlmsBlendblock
        //to work correctly (which is a very useful assert)
        newBlock->mRsData = reinterpret_cast<void*>( 1 );
    }

    void GLES2RenderSystem::destroyBlend( ShaderChBlend *block )
    {
        block->mRsData = 0;
    }

    void GLES2RenderSystem::createTexSample( TextureSampleUnit *newBlock )
    {
#if OGRE_NO_GLES3_SUPPORT == 0
        GLuint samplerName;
        glGenSamplers( 1, &samplerName );
#endif

        GLint minFilter, magFilter;
        switch( newBlock->mMinFilter )
        {
        case FO_ANISOTROPIC:
        case FO_LINEAR:
            switch( newBlock->mMipFilter )
            {
            case FO_ANISOTROPIC:
            case FO_LINEAR:
                // linear min, linear mip
                minFilter = GL_LINEAR_MIPMAP_LINEAR;
                break;
            case FO_POINT:
                // linear min, point mip
                minFilter = GL_LINEAR_MIPMAP_NEAREST;
                break;
            case FO_NONE:
                // linear min, no mip
                minFilter = GL_LINEAR;
                break;
            }
            break;
        case FO_POINT:
        case FO_NONE:
            switch( newBlock->mMipFilter )
            {
            case FO_ANISOTROPIC:
            case FO_LINEAR:
                // nearest min, linear mip
                minFilter = GL_NEAREST_MIPMAP_LINEAR;
                break;
            case FO_POINT:
                // nearest min, point mip
                minFilter = GL_NEAREST_MIPMAP_NEAREST;
                break;
            case FO_NONE:
                // nearest min, no mip
                minFilter = GL_NEAREST;
                break;
            }
            break;
        }

        magFilter = newBlock->mMagFilter <= FO_POINT ? GL_NEAREST : GL_LINEAR;

#if OGRE_NO_GLES3_SUPPORT == 0
        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_MIN_FILTER, minFilter ) );
        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_MAG_FILTER, magFilter ) );

        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_WRAP_S,
                                   getTextureAddressingMode( newBlock->mU ) ) );
        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_WRAP_T,
                                   getTextureAddressingMode( newBlock->mV ) ) );
        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_WRAP_R,
                                   getTextureAddressingMode( newBlock->mW ) ) );

        /*GLCheck( glSamplerParameterfv( samplerName, GL_TEXTURE_BORDER_COLOR,
                                    newBlock->mBorderColour.ptr() ) );
        GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_LOD_BIAS, newBlock->mMipLodBias ) );*/
        GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_MIN_LOD, newBlock->mMinLod ) );
        GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_MAX_LOD, newBlock->mMaxLod ) );

        if( newBlock->mCompareFunction != NUM_COMPARE_FUNCTIONS )
        {
            GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_COMPARE_MODE,
                                       GL_COMPARE_REF_TO_TEXTURE ) );
            GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_COMPARE_FUNC,
                                       convertCompareFunction( newBlock->mCompareFunction ) ) );
        }

        if( mFeature->hasCapability(RSC_ANISOTROPY) )
        {
            GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                                       newBlock->mMaxAnisotropy ) );
        }

        newBlock->mRsData = (void*)(intptr_t)samplerName;
#else
        GLES2HlmsSamplerblock *glSamplerblock = new GLES2HlmsSamplerblock();
        glSamplerblock->mInternalId= ++mSamplerblocksInternalIdCount;
        glSamplerblock->mMinFilter = minFilter;
        glSamplerblock->mMagFilter = magFilter;
        glSamplerblock->mU  = getTextureAddressingMode( newBlock->mU );
        glSamplerblock->mV  = getTextureAddressingMode( newBlock->mV );
        glSamplerblock->mW  = getTextureAddressingMode( newBlock->mW );

        glSamplerblock->mAnisotropy = std::min( newBlock->mMaxAnisotropy, mLargestSupportedAnisotropy );

        newBlock->mRsData = glSamplerblock;
#endif
    }

    void GLES2RenderSystem::destroyTexSample( TextureSampleUnit *block )
    {
#if OGRE_NO_GLES3_SUPPORT == 0
        GLuint samplerName = static_cast<GLuint>( reinterpret_cast<intptr_t>( block->mRsData ) );
        glDeleteSamplers( 1, &samplerName );
#else
        GLES2HlmsSamplerblock *glSamplerblock = reinterpret_cast<GLES2HlmsSamplerblock*>(block->mRsData);
        delete glSamplerblock;
        block->mRsData = 0;
#endif
    }

    void GLES2RenderSystem::_setHlmsDepthblock( const ShaderChStencil *s, const GLES2HlmsPso *pso )
    {
        if( s->mDepthCheck )
        {
            GLCheck( glEnable( GL_DEPTH_TEST ) );
        }
        else
        {
            GLCheck( glDisable( GL_DEPTH_TEST ) );
        }
        GLCheck( glDepthMask( pso->depthWrite ) );
        GLCheck( glDepthFunc( pso->depthFunc ) );

        _setDepthBias( s->mDepthBiasConstant, s->mDepthBiasSlopeScale );

        //Cull mode
        if( pso->cullMode == 0 )
        {
            GLCheck( glDisable( GL_CULL_FACE ) );
        }
        else
        {
            GLCheck( glEnable( GL_CULL_FACE ) );
            GLCheck( glCullFace( pso->cullMode ) );
        }

        //Polygon mode
        //GLCheck( glPolygonMode( GL_FRONT_AND_BACK, pso->polygonMode ) );

        if( s->mScissorTestEnabled )
        {
            GLCheck( glEnable(GL_SCISSOR_TEST) );
        }
        else
        {
            GLCheck( glDisable(GL_SCISSOR_TEST) );
        }

        mDepthWrite      = s->mDepthWrite;
        mScissorsEnabled = s->mScissorTestEnabled;
    }

    void GLES2RenderSystem::_setHlmsBlendblock( const ShaderChBlend *blend, const GLES2HlmsPso *pso )
    {
        if( pso->enableAlphaBlend )
        {
            GLCheck( glEnable(GL_BLEND) );
            if( blend->mSeparateBlend )
            {
                GLCheck( glBlendFuncSeparate( pso->sourceBlend, pso->destBlend,
                                           pso->sourceBlendAlpha, pso->destBlendAlpha ) );
                GLCheck( glBlendEquationSeparate( pso->blendFunc, pso->blendFuncAlpha ) );
            }
            else
            {
                GLCheck( glBlendFunc( pso->sourceBlend, pso->destBlend ) );
                GLCheck( glBlendEquation( pso->blendFunc ) );
            }
        }
        else
        {
            GLCheck( glDisable(GL_BLEND) );
        }

        if( blend->mAlphaToCoverageEnabled )
        {
            GLCheck( glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE) );
        }
        else
        {
            GLCheck( glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE) );
        }


        if( mBlendChannelMask != blend->mBlendChannelMask )
        {
            GLboolean r = (blend->mBlendChannelMask & ShaderChBlend::BlendChannelRed) != 0;
            GLboolean g = (blend->mBlendChannelMask & ShaderChBlend::BlendChannelGreen) != 0;
            GLboolean b = (blend->mBlendChannelMask & ShaderChBlend::BlendChannelBlue) != 0;
            GLboolean a = (blend->mBlendChannelMask & ShaderChBlend::BlendChannelAlpha) != 0;
            GLCheck( glColorMask( r, g, b, a ) );

            mBlendChannelMask = blend->mBlendChannelMask;
        }
    }

    void GLES2RenderSystem::_setTextureSample( uint8 texUnit, const TextureSampleUnit *samplerblock )
    {
        assert( (!samplerblock || samplerblock->mRsData) &&
                "The block must have been created via RenderSys::getSamplerblock!" );

#if OGRE_NO_GLES3_SUPPORT == 0
        if( !samplerblock )
            glBindSampler( texUnit, 0 );
        else
            glBindSampler( texUnit, static_cast<GLuint>(
                                    reinterpret_cast<intptr_t>( samplerblock->mRsData ) ) );
#else
        if (!activateGLTextureUnit(texUnit))
            return;

        GLES2HlmsSamplerblock *glSamplerblock = reinterpret_cast<GLES2HlmsSamplerblock*>( samplerblock->mRsData );
        GLenum target = mTextureTypes[texUnit]

        OGRE_CHECK_GL_ERROR(glTexParameteri( target, GL_TEXTURE_MIN_FILTER, glSamplerblock->mMinFilter ));
        OGRE_CHECK_GL_ERROR(glTexParameteri( target, GL_TEXTURE_MAG_FILTER, glSamplerblock->mMagFilter ));

        OGRE_CHECK_GL_ERROR(glTexParameteri( target, GL_TEXTURE_WRAP_S, glSamplerblock->mU ));
        OGRE_CHECK_GL_ERROR(glTexParameteri( target, GL_TEXTURE_WRAP_T, glSamplerblock->mV ));
        //OGRE_CHECK_GL_ERROR(glTexParameteri( target, GL_TEXTURE_WRAP_R_OES, glSamplerblock->mW ));

        if( glSamplerblock->mAnisotropy != 0 )
        {
            OGRE_CHECK_GL_ERROR(glTexParameterf( target, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                                                  glSamplerblock->mAnisotropy ));
        }

        /*OGRE_CHECK_GL_ERROR(glTexParameterf( target, GL_TEXTURE_LOD_BIAS, samplerblock->mMipLodBias ));

        OGRE_CHECK_GL_ERROR(glTexParameterfv( target, GL_TEXTURE_BORDER_COLOR,
                                reinterpret_cast<GLfloat*>( &samplerblock->mBorderColour ) ) );

        OGRE_CHECK_GL_ERROR(glTexParameterf( target, GL_TEXTURE_MIN_LOD, samplerblock->mMinLod ));
        OGRE_CHECK_GL_ERROR(glTexParameterf( target, GL_TEXTURE_MAX_LOD, samplerblock->mMaxLod ));*/


        activateGLTextureUnit(0);
#endif
    }

    void GLES2RenderSystem::setRSO( const RenderStateObject *pso )
    {
        // Disable previous state
        GLSLESShader::unbindAll();

        RenderSystem::setRSO( pso );

        mUseAdjacency   = false;
        mPso            = 0;

        if( !pso )
            return;
        
        //Set new state
        mPso = reinterpret_cast<GLES2HlmsPso*>( pso->mExtData );

        _setHlmsDepthblock( pso->mStencil, mPso );
        _setHlmsBlendblock( pso->mBlend, mPso );

        if( mPso->mVS )
        {
            mPso->mVS->bind();
            mActiveVGPP = mPso->mVS->getDefaultParam();
            mVPMode = true;
        }
        if( mPso->mFS )
        {
            mPso->mFS->bind();
            mActiveFGPP = mPso->mFS->getDefaultParam();
            mFPMode = true;
        }
        
        if(Root::getSingleton().getRenderSystem()->getFeature()->hasCapability(CF_Program_SeparateObject))
            GLSLESProgramPipelineManager::getSingleton().getActiveProgramPipeline();
        else
            GLSLESLinkProgramManager::getSingleton().getActiveLinkProgram();
    }

    void GLES2RenderSystem::setCSO( const ComputeStateObject *pso )
    {
    }

    void GLES2RenderSystem::_setIndirect( IndirectBuffer *indirectBuffer )
    {
//        if( mBufferManager->supportsIndirectBuffers() )
//        {
//            if( indirectBuffer )
//            {
//                GLES2BufferInterface *bufferInterface = static_cast<GLES2BufferInterface*>( indirectBuffer->getGpuData() );
//                GLCheck( glBindBuffer( GL_DRAW_INDIRECT_BUFFER, bufferInterface->getVboName() ) );
//            }
//            else
//            {
//                GLCheck( glBindBuffer( GL_DRAW_INDIRECT_BUFFER, 0 ) );
//            }
//        }
//        else
        {
            if( indirectBuffer )
                mIndirectPtr = indirectBuffer->getSwBufferPtr();
            else
                mIndirectPtr = 0;
        }
    }

    void GLES2RenderSystem::_beginFrame(void)
    {
    }

    void GLES2RenderSystem::_endFrame(void)
    {
        // unbind PSO programs at end of frame
        // this is mostly to avoid holding bound programs that might get deleted
        // outside via the resource manager
        setRSO( 0 );

#if OGRE_PLATFORM != OGRE_PLATFORM_NACL
        glBindProgramPipelineEXT(0);
#endif
    }

    void GLES2RenderSystem::_setDepthBias(float constantBias, float slopeScaleBias)
    {
        if (constantBias != 0 || slopeScaleBias != 0)
        {
            OGRE_CHECK_GL_ERROR(glEnable(GL_POLYGON_OFFSET_FILL));
            OGRE_CHECK_GL_ERROR(glPolygonOffset(-slopeScaleBias, -constantBias));
        }
        else
        {
            OGRE_CHECK_GL_ERROR(glDisable(GL_POLYGON_OFFSET_FILL));
        }
    }

    void GLES2RenderSystem::_setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
    {
        mState->setColourMask(red, green, blue, alpha);
    }

    void GLES2RenderSystem::getSysProj(const Matrix4& matrix, Matrix4& dest)
    {
        if( !mReverseDepth )
        {
            // no any conversion request for OpenGL
            dest = matrix;
        }
        else
        {
            RenderSystem::getSysProj( matrix, dest );
        }
    }

    //---------------------------------------------------------------------
    PixelCountQuery* GLES2RenderSystem::create(void)
    {
        if(mGLSupport->checkGpuApi("GL_EXT_occlusion_query_boolean") || gleswIsSupported(3, 0))
        {
            GLES2HardwareOcclusionQuery* ret = new GLES2HardwareOcclusionQuery(); 
            mPixelQueryList.push_back(ret);
            return ret;
        }
        else
        {
            return NULL;
        }
    }

    void GLES2RenderSystem::_applyObliqueDepthProjection(Matrix4& matrix,
                                                      const Plane& plane,
                                                      bool forGpuProgram)
    {
        // Thanks to Eric Lenyel for posting this calculation at www.terathon.com
        
        // Calculate the clip-space corner point opposite the clipping plane
        // as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
        // transform it into camera space by multiplying it
        // by the inverse of the projection matrix

        Vector4 q;
        q.x = (Math::Sign(plane.normal.x) + matrix[0][2]) / matrix[0][0];
        q.y = (Math::Sign(plane.normal.y) + matrix[1][2]) / matrix[1][1];
        q.z = -1.0F;
        q.w = (1.0F + matrix[2][2]) / matrix[2][3];

        // Calculate the scaled plane vector
        Vector4 clipPlane4d(plane.normal.x, plane.normal.y, plane.normal.z, plane.d);
        Vector4 c = clipPlane4d * (2.0F / (clipPlane4d.dotProduct(q)));

        // Replace the third row of the projection matrix
        matrix[2][0] = c.x;
        matrix[2][1] = c.y;
        matrix[2][2] = c.z + 1.0F;
        matrix[2][3] = c.w; 
    }

    void GLES2RenderSystem::setStencilCheckEnabled(bool enabled)
    {
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_occlusion_query_boolean"))
        {
            GLES2HardwareOcclusionQuery* ret = new GLES2HardwareOcclusionQuery();
            mPixelQueryList.push_back(ret);
            return ret;
        }
        else
        {
            return NULL;
        }
    }

    void GLES2RenderSystem::setStencil(uint32 refValue, const ShaderChStencil & stencil)
    {
        RenderSystem::setStencil(refValue, stencil);
        
        if( stencil.enabled )
        {
            GLCheck(glEnable(GL_STENCIL_TEST));

            GLCheck(glStencilMask(mStencilList[0].writeMask));

            //GLCheck( glStencilMaskSeparate( GL_BACK, mStencilList[0].writeMask ) );
            //GLCheck( glStencilMaskSeparate( GL_FRONT, mStencilList[0].writeMask ) );

            if(stencil.stencilFront != stencil.stencilBack)
            {
                if (!mFeature->hasCapability(RSC_TWO_SIDED_STENCIL))
                    N_EXCEPT(Exception::ERR_INVALIDPARAMS, "2-sided stencils are not supported",
                                "GLES2RenderSystem::setStencil");
                
                GLCheck( glStencilFuncSeparate( GL_BACK,
                                             convertCompareFunction(stencil.stencilBack.compareOp),
                                             refValue, stencil.readMask ) );
                GLCheck( glStencilOpSeparate( GL_BACK,
                                           convertStencilOp(stencil.stencilBack.stencilFailOp),
                                           convertStencilOp(stencil.stencilBack.stencilDepthFailOp),
                                           convertStencilOp(stencil.stencilBack.stencilPassOp)));

                GLCheck( glStencilFuncSeparate( GL_FRONT,
                                             convertCompareFunction(stencil.stencilFront.compareOp),
                                             refValue, stencil.readMask ) );
                GLCheck( glStencilOpSeparate( GL_FRONT,
                                           convertStencilOp(stencil.stencilFront.stencilFailOp),
                                           convertStencilOp(stencil.stencilFront.stencilDepthFailOp),
                                           convertStencilOp(stencil.stencilFront.stencilPassOp)));
            }
            else
            {
                GLCheck( glStencilFunc( convertCompareFunction(stencil.stencilFront.compareOp),
                                     refValue, stencil.readMask ) );
                GLCheck( glStencilOp( convertStencilOp(stencil.stencilFront.stencilFailOp),
                                   convertStencilOp(stencil.stencilFront.stencilDepthFailOp),
                                   convertStencilOp(stencil.stencilFront.stencilPassOp)));
            }
        }
        else
        {
            GLCheck( glDisable(GL_STENCIL_TEST) );
        }
    }

    /*GLint GLES2RenderSystem::getCombinedMinMipFilter(void) const
    {
        switch(mMinFilter)
        {
            case FO_ANISOTROPIC:
            case FO_LINEAR:
                switch (mMipFilter)
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
                switch (mMipFilter)
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
    }*/

    void GLES2RenderSystem::_render(const v1::GeometryRaw& op)
    {
        // Call super class.
        RenderSystem::_render(op);

        // Create variables related to instancing.
        v1::HardwareVertexBufferSharedPtr globalInstanceVertexBuffer;
        v1::VertexDeclaration* globalVertexDeclaration = 0;
        bool hasInstanceData = false;
        size_t numberOfInstances = 0;
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_instanced_arrays"))
        {
            globalInstanceVertexBuffer = getInstanceBuffer();
            globalVertexDeclaration = getInstanceData();
            hasInstanceData = (op.useGlobalInstancingVertexBufferIsAvailable &&
                                !globalInstanceVertexBuffer.isNull() && (globalVertexDeclaration != NULL))
                                || op.vertexData->vertexBufferBinding->getHasInstanceData();

            numberOfInstances = op.mInstanceCount;

            if (op.useGlobalInstancingVertexBufferIsAvailable)
            {
                numberOfInstances *= getInstanceCount();
            }
        }

        const v1::VertexDeclaration::VertexElementList& decl =
            op.vertexData->vertexDeclaration->getElements();
        v1::VertexDeclaration::VertexElementList::const_iterator elemIter, elemEnd;
        elemEnd = decl.end();

        // Bind VAO (set of per-vertex attributes: position, normal, etc.).
        bool updateVAO = true;
        // Till VAO is NYI we use single shared, and update it each time

        // Bind the appropriate VBOs to the active attributes of the VAO.
        for (elemIter = decl.begin(); elemIter != elemEnd; ++elemIter)
        {
            const v1::VertexElement & elem = *elemIter;
            size_t source = elem.getSource();

            if (!op.vertexData->vertexBufferBinding->isAttach(source))
                continue; // Skip unbound elements.
 
            v1::HardwareVertexBufferSharedPtr vertexBuffer =
                op.vertexData->vertexBufferBinding->getBuffer(source);
            bindVertexElementToGpu(elem, vertexBuffer, op.vertexData->vertexStart,
                                   mRenderAttribsBound, mRenderInstanceAttribsBound, updateVAO);
        }

        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_instanced_arrays"))
        {
            if( !globalInstanceVertexBuffer.isNull() && globalVertexDeclaration != NULL )
            {
                elemEnd = globalVertexDeclaration->getElements().end();
                for (elemIter = globalVertexDeclaration->getElements().begin(); elemIter != elemEnd; ++elemIter)
                {
                    const v1::VertexElement & elem = *elemIter;
                    bindVertexElementToGpu(elem, globalInstanceVertexBuffer, 0,
                                           mRenderAttribsBound, mRenderInstanceAttribsBound, updateVAO);
                }
            }
        }

        // Determine the correct primitive type to render.
        GLint primType;
        switch (op.mType)
        {
            case OT_POINT_LIST:
                primType = GL_POINTS;
                break;
            case OT_LINE_LIST:
                primType = GL_LINES;
                break;
            case OT_LINE_STRIP:
                primType = GL_LINE_STRIP;
                break;
            default:
            case OT_TRIANGLE_LIST:
                primType = GL_TRIANGLES;
                break;
            case OT_TRIANGLE_STRIP:
                primType = GL_TRIANGLE_STRIP;
                break;
            case OT_TRIANGLE_FAN:
                primType = GL_TRIANGLE_FAN;
                break;
        }

        if (op.useIndexes)
        {
            OGRE_CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<v1::GLES2HardwareIndexBuffer*>(op.indexData->indexBuffer.get())->getGLBufferId()));

            void* pBufferData = GL_BUFFER_OFFSET(op.indexData->indexStart *
                                            op.indexData->indexBuffer->getIndexSize());

            GLenum indexType = (op.indexData->indexBuffer->getType() == v1::HardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

            do
            {
                // Update derived depth bias.
                if (mAutoDepthBias && mCurrentRender > 0)
                {
                    _setDepthBias(mDepthBiasBase +
                                  mDepthBiasFactor * mCurrentRender,
                                  mDepthBiasSlopeScale);
                }

                if(hasInstanceData && (hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_instanced_arrays")))
                {
                    OGRE_CHECK_GL_ERROR(glDrawElementsInstancedEXT(primType, static_cast<GLsizei>(op.indexData->indexCount), indexType, pBufferData, static_cast<GLsizei>(numberOfInstances)));
                }
                else
                {
                    OGRE_CHECK_GL_ERROR(glDrawElements(primType, static_cast<GLsizei>(op.indexData->indexCount), indexType, pBufferData));
                }

            } while (updateParam());
        }
        else
        {
            do
            {
                // Update derived depth bias.
                if (mAutoDepthBias && mCurrentRender > 0)
                {
                    _setDepthBias(mDepthBiasBase +
                                  mDepthBiasFactor * mCurrentRender,
                                  mDepthBiasSlopeScale);
                }

                if(hasInstanceData && (hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_instanced_arrays")))
                {
                    OGRE_CHECK_GL_ERROR(glDrawArraysInstancedEXT(primType, 0, static_cast<GLsizei>(op.vertexData->vertexCount), static_cast<GLsizei>(numberOfInstances)));
                }
                else
                {
                    OGRE_CHECK_GL_ERROR(glDrawArrays(primType, 0, static_cast<GLsizei>(op.vertexData->vertexCount)));
                }
            } while (updateParam());
        }

#if OGRE_NO_GLES2_VAO_SUPPORT == 0
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_vertex_array_object"))
            // Unbind the vertex array object.  Marks the end of what state will be included.
            OGRE_CHECK_GL_ERROR(glBindVertexArrayOES(0));
#endif

        // Unbind all attributes
        for (vector<GLuint>::type::iterator ai = mRenderAttribsBound.begin(); ai != mRenderAttribsBound.end(); ++ai)
        {
            OGRE_CHECK_GL_ERROR(glDisableVertexAttribArray(*ai));
        }

        // Unbind any instance attributes
        for (vector<GLuint>::type::iterator ai = mRenderInstanceAttribsBound.begin(); ai != mRenderInstanceAttribsBound.end(); ++ai)
        {
            glVertexAttribDivisorEXT(*ai, 0);
        }

        mRenderAttribsBound.clear();
        mRenderInstanceAttribsBound.clear();
    }

    void GLES2RenderSystem::_execute( const ComputeStateObject &pso )
    {
    }

    void GLES2RenderSystem::setGeometry( const GeometryRaw *_vao )
    {
#if OGRE_NO_GLES2_VAO_SUPPORT == 0
        if(hasMinGLVersion(3, 0) || checkGpuApi("GL_OES_vertex_array_object"))
        {
            if( _vao )
            {
                const GLES2VertexArrayObject *vao = static_cast<const GLES2VertexArrayObject*>( _vao );
                OGRE_CHECK_GL_ERROR( glBindVertexArrayOES( vao->mVaoId ) );
            }
            else
            {
                OGRE_CHECK_GL_ERROR( glBindVertexArrayOES( 0 ) );
            }
        }
#endif
    }

    void GLES2RenderSystem::_render( const CbDrawCallIndexed *cmd )
    {
//        const GLES2VertexArrayObject *vao = static_cast<const GLES2VertexArrayObject*>( cmd->vao );
//        GLenum mode = mPso->mDS ? GL_PATCHES : vao->mPrimType[mUseAdjacency];
//
//        GLenum indexType = vao->mIndexBuffer->getIndexType() == IndexBufferPacked::IT_16BIT ?
//                                                            GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
//
//        GLCheck( glMultiDrawElementsIndirect( mode, indexType, cmd->indirectBufferOffset,
//                                           cmd->numDraws, sizeof(IndexIndirect) ) );
    }

    void GLES2RenderSystem::_render( const CbDrawCallStrip *cmd )
    {
//        const GLES2VertexArrayObject *vao = static_cast<const GLES2VertexArrayObject*>( cmd->vao );
//        GLenum mode = mPso->mDS ? GL_PATCHES : vao->mPrimType[mUseAdjacency];
//
//        GLCheck( glMultiDrawArraysIndirect( mode, cmd->indirectBufferOffset,
//                                         cmd->numDraws, sizeof(VertexIndirect) ) );
    }

    void GLES2RenderSystem::_renderEmulated( const CbDrawCallIndexed *cmd )
    {
//        const GLES2VertexArrayObject *vao = static_cast<const GLES2VertexArrayObject*>( cmd->vao );
//        GLenum mode = mPso->mDS ? GL_PATCHES : vao->mPrimType[mUseAdjacency];
//
//        GLenum indexType = vao->mIndexBuffer->getIndexType() == IndexBufferPacked::IT_16BIT ?
//                                                            GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
//
//        IndexIndirect *drawCmd = reinterpret_cast<IndexIndirect*>(
//                                    mIndirectPtr + (size_t)cmd->indirectBufferOffset );
//
//        const size_t bytesPerIndexElement = vao->mIndexBuffer->getUnitSize();
//
//        for( uint32 i=cmd->numDraws; i--; )
//        {
//            GLCheck( glDrawElementsInstancedBaseVertexBaseInstance(
//                      mode,
//                      drawCmd->primCount,
//                      indexType,
//                      reinterpret_cast<void*>( drawCmd->firstVertexIndex * bytesPerIndexElement ),
//                      drawCmd->instanceCount,
//                      drawCmd->baseVertex,
//                      drawCmd->baseInstance ) );
//            ++drawCmd;
//        }
    }

    void GLES2RenderSystem::_renderEmulated( const CbDrawCallStrip *cmd )
    {
//        const GLES2VertexArrayObject *vao = static_cast<const GLES2VertexArrayObject*>( cmd->vao );
//        GLenum mode = mPso->mDS ? GL_PATCHES : vao->mPrimType[mUseAdjacency];
//
//        VertexIndirect *drawCmd = reinterpret_cast<VertexIndirect*>(
//                                    mIndirectPtr + (size_t)cmd->indirectBufferOffset );
//
//        for( uint32 i=cmd->numDraws; i--; )
//        {
//            GLCheck( glDrawArraysInstancedBaseInstance(
//                      mode,
//                      drawCmd->firstVertexIndex,
//                      drawCmd->primCount,
//                      drawCmd->instanceCount,
//                      drawCmd->baseInstance ) );
//            ++drawCmd;
//        }
    }

    void GLES2RenderSystem::_renderEmulatedNoBaseInstance( const CbDrawCallIndexed *cmd )
    {
        const GLES2VertexArrayObject *vao = static_cast<const GLES2VertexArrayObject*>( cmd->vao );
        GLenum mode = /* mPso->mDS ? GL_PATCHES : */ vao->mPrimType[mUseAdjacency];

        GLenum indexType = vao->mIndexBuffer->getIndexType() == IndexBufferPacked::IT_16BIT ?
                                                            GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

        IndexIndirect *drawCmd = reinterpret_cast<IndexIndirect*>(
                                    mIndirectPtr + (size_t)cmd->indirectBufferOffset );

        const size_t bytesPerIndexElement = vao->mIndexBuffer->getUnitSize();

        GLSLESProgramCommon* activeProgram = NULL;
        if(Root::getSingleton().getRenderSystem()->getFeature()->hasCapability(CF_Program_SeparateObject))
            activeProgram = GLSLESProgramPipelineManager::getSingleton().getActiveProgramPipeline();
        else
            activeProgram = GLSLESLinkProgramManager::getSingleton().getActiveLinkProgram();

        for( uint32 i=cmd->numDraws; i--; )
        {
            if(activeProgram->getBaseInstanceLocation() != (GLint)GL_INVALID_INDEX)
                GLCheck( glUniform1ui( activeProgram->getBaseInstanceLocation(),
                                    static_cast<GLuint>( drawCmd->baseInstance ) ) );

//          if(hasMinGLVersion(3, 2))
//          {
//            GLCheck( glDrawElementsInstancedBaseVertex(
//                    mode,
//                    drawCmd->primCount,
//                    indexType,
//                    reinterpret_cast<void*>( drawCmd->firstVertexIndex * bytesPerIndexElement ),
//                    drawCmd->instanceCount,
//                    drawCmd->baseVertex ) );
//          }
//          else
            {
                assert(drawCmd->baseVertex == 0);
                GLCheck( glDrawElementsInstanced(
                        mode,
                        drawCmd->primCount,
                        indexType,
                        reinterpret_cast<void*>( drawCmd->firstVertexIndex * bytesPerIndexElement ),
                        drawCmd->instanceCount ) );
            }
            ++drawCmd;
        }
    }

    void GLES2RenderSystem::_renderEmulatedNoBaseInstance( const CbDrawCallStrip *cmd )
    {
        const GLES2VertexArrayObject *vao = static_cast<const GLES2VertexArrayObject*>( cmd->vao );
        GLenum mode = /* mPso->mDS ? GL_PATCHES : */ vao->mPrimType[mUseAdjacency];

        VertexIndirect * drawCmd = reinterpret_cast<VertexIndirect*>(
                                    mIndirectPtr + (size_t)cmd->indirectBufferOffset );

        GLSLESProgramCommon* activeProgram = NULL;
        if(Root::getSingleton().getRenderSystem()->getFeature()->hasCapability(CF_Program_SeparateObject))
            activeProgram = GLSLESProgramPipelineManager::getSingleton().getActiveProgramPipeline();
        else
            activeProgram = GLSLESLinkProgramManager::getSingleton().getActiveLinkProgram();

        for( uint32 i=cmd->numDraws; i--; )
        {
            if(activeProgram->getBaseInstanceLocation() != (GLint)GL_INVALID_INDEX)
                GLCheck( glUniform1ui( activeProgram->getBaseInstanceLocation(),
                                    static_cast<GLuint>( drawCmd->baseInstance ) ) );

            GLCheck( glDrawArraysInstanced(
                    mode,
                    drawCmd->firstVertexIndex,
                    drawCmd->primCount,
                    drawCmd->instanceCount ) );
            ++drawCmd;
        }
    }

    void GLES2RenderSystem::_startLegacyV1Rendering(void)
    {
        glBindVertexArray(mGlobalVao);
    }

    void GLES2RenderSystem::_setRenderOperation(const v1::GeometryRaw * cmd)
    {
        mCurrentVertexBuffer = cmd->vertexData;
        mCurrentIndexBuffer = cmd->indexData;

        glBindVertexArray(mGlobalVao);

        v1::VertexBufferBinding * vertexBufferBinding = cmd->vertexData->vertexBufferBinding;
        v1::VertexDeclaration * vertexDeclaration     = cmd->vertexData->vertexDeclaration;

        const v1::VertexDeclaration::VertexElementList& elements = vertexDeclaration->getElements();
        v1::VertexDeclaration::VertexElementList::const_iterator itor;
        v1::VertexDeclaration::VertexElementList::const_iterator end;

        itor = elements.begin();
        end  = elements.end();

        while( itor != end )
        {
            const v1::VertexElement &elem = *itor;

            unsigned short source = elem.getSource();

            VertexType semantic = elem.getSemantic();
            GLuint attributeIndex = GLES2VaoManager::getAttributeIndexFor( semantic ) +
                                    elem.getIndex();

            if( !vertexBufferBinding->isAttach( source ) )
            {
                GLCheck( glDisableVertexAttribArray( attributeIndex ) );
                ++itor;
                continue; // Skip unbound elements.
            }

            v1::HardwareVertexBufferSharedPtr vertexBuffer = vertexBufferBinding->getBuffer( source );
            const v1::GLES2HardwareVertexBuffer* hwGlBuffer =
                            static_cast<v1::GLES2HardwareVertexBuffer*>( vertexBuffer.get() );

            GLCheck( glBindBuffer( GL_ARRAY_BUFFER, hwGlBuffer->getGLBufferId() ) );
            void *bindOffset = GL_BUFFER_OFFSET( elem.getOffset() );

            DataType vertexElementType = elem.getType();

            GLint typeCount = v1::VertexElement::getTypeCount( vertexElementType );
            GLboolean normalised = v1::VertexElement::isTypeNormalized( vertexElementType ) ? GL_TRUE :
                                                                                              GL_FALSE;
            switch( vertexElementType )
            {
            case VET_COLOUR:
            case VET_COLOUR_ABGR:
            case VET_COLOUR_ARGB:
                // Because GL takes these as a sequence of single unsigned bytes, count needs to be 4
                // VertexElement::getTypeCount treats them as 1 (RGBA)
                // Also need to normalise the fixed-point data
                typeCount = 4;
                normalised = GL_TRUE;
                break;
            default:
                break;
            };

            assert( (semantic != VES_TEXTURE_COORDINATES || elem.getIndex() < 8) &&
                    "Up to 8 UVs are supported." );

            if( semantic == VES_BINORMAL )
            {
                LogManager::getSingleton().logMessage(
                            "WARNING: VES_BINORMAL will not render properly in "
                            "many GPUs where GL_MAX_VERTEX_ATTRIBS = 16. Consider"
                            " changing for VES_TANGENT with 4 components or use"
                            " QTangents", LML_CRITICAL );
            }

            GLenum type = v1::GLES2HardwareBufferManager::getGLType( vertexElementType );

            switch( v1::VertexElement::getBaseType( vertexElementType ) )
            {
            default:
            case VET_FLOAT1:
                GLCheck( glVertexAttribPointer( attributeIndex, typeCount,
                                             type,
                                             normalised,
                                             static_cast<GLsizei>(vertexBuffer->getVertexSize()),
                                             bindOffset ) );
                break;
            case VET_BYTE4:
            case VET_UBYTE4:
            case VET_SHORT2:
            case VET_USHORT2:
            case VET_UINT1:
            case VET_INT1:
                GLCheck( glVertexAttribIPointer( attributeIndex, typeCount,
                                              type,
                                              static_cast<GLsizei>(vertexBuffer->getVertexSize()),
                                              bindOffset ) );
                break;
//            case VET_DOUBLE1:
//                GLCheck( glVertexAttribLPointer( attributeIndex, typeCount,
//                                              type,
//                                              static_cast<GLsizei>(vertexBuffer->getVertexSize()),
//                                              bindOffset ) );
                break;
            }

            GLCheck( glVertexAttribDivisor( attributeIndex, hwGlBuffer->getInstanceDataStepRate() *
                                         hwGlBuffer->getIsInstanceData() ) );
            GLCheck( glEnableVertexAttribArray( attributeIndex ) );

            ++itor;
        }

        if( cmd->indexData )
        {
            v1::GLES2HardwareIndexBuffer * indexBuffer = static_cast<v1::GLES2HardwareIndexBuffer*>(
                                                                    cmd->indexData->indexBuffer.get() );
            GLCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getGLBufferId()));
        }

        mCurrentPolygonMode = GL_TRIANGLES;
        switch(cmd->mType)
        {
        case OT_POINT_LIST:
            mCurrentPolygonMode = GL_POINTS;
            break;
        case OT_LINE_LIST:
            mCurrentPolygonMode = /*mUseAdjacency ? GL_LINES_ADJACENCY :*/ GL_LINES;
            break;
        case OT_LINE_STRIP:
            mCurrentPolygonMode = /*mUseAdjacency ? GL_LINE_STRIP_ADJACENCY :*/ GL_LINE_STRIP;
            break;
        default:
        case OT_TRIANGLE_LIST:
            mCurrentPolygonMode = /*mUseAdjacency ? GL_TRIANGLES_ADJACENCY :*/ GL_TRIANGLES;
            break;
        case OT_TRIANGLE_STRIP:
            mCurrentPolygonMode = /*mUseAdjacency ? GL_TRIANGLE_STRIP_ADJACENCY :*/ GL_TRIANGLE_STRIP;
            break;
        case OT_TRIANGLE_FAN:
            mCurrentPolygonMode = GL_TRIANGLE_FAN;
            break;
        }
    }

    void GLES2RenderSystem::_render(const v1::CbDrawCallIndexed * cmd)
    {
        assert(false);
//        GLenum indexType = mCurrentIndexBuffer->indexBuffer->getType() ==
//                            v1::HardwareIndexBuffer::IT_16BIT ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
//
//        const size_t bytesPerIndexElement = mCurrentIndexBuffer->indexBuffer->getIndexSize();
//
//        GLCheck( glDrawElementsInstancedBaseVertexBaseInstance(
//                    mCurrentPolygonMode,
//                    cmd->primCount,
//                    indexType,
//                    reinterpret_cast<void*>( cmd->firstVertexIndex * bytesPerIndexElement ),
//                    cmd->instanceCount,
//                    mCurrentVertexBuffer->vertexStart,
//                    cmd->baseInstance ) );
    }

    void GLES2RenderSystem::_render(const v1::CbDrawCallStrip * cmd)
    {
        assert(false);
//        GLCheck( glDrawArraysInstancedBaseInstance(
//                    mCurrentPolygonMode,
//                    cmd->firstVertexIndex,
//                    cmd->primCount,
//                    cmd->instanceCount,
//                    cmd->baseInstance ) );
    }

    void GLES2RenderSystem::_renderNoBaseInstance( const v1::CbDrawCallIndexed *cmd )
    {
        GLenum indexType = mCurrentIndexBuffer->indexBuffer->getType() ==
                            v1::HardwareIndexBuffer::IT_16BIT ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

        const size_t bytesPerIndexElement = mCurrentIndexBuffer->indexBuffer->getIndexSize();

        GLSLESProgramCommon* activeProgram = NULL;
        if(Root::getSingleton().getRenderSystem()->getFeature()->hasCapability(CF_Program_SeparateObject))
            activeProgram = GLSLESProgramPipelineManager::getSingleton().getActiveProgramPipeline();
        else
            activeProgram = GLSLESLinkProgramManager::getSingleton().getActiveLinkProgram();

        GLCheck( glUniform1ui( activeProgram->getBaseInstanceLocation(),
                            static_cast<GLuint>( cmd->baseInstance ) ) );

//        if(hasMinGLVersion(3, 2))
//        {
//            GLCheck( glDrawElementsInstancedBaseVertex(
//                        mCurrentPolygonMode,
//                        cmd->primCount,
//                        indexType,
//                        reinterpret_cast<void*>(cmd->firstVertexIndex * bytesPerIndexElement),
//                        cmd->instanceCount,
//                        mCurrentVertexBuffer->vertexStart ) );
//        }
//        else
        {
            assert(mCurrentVertexBuffer->vertexStart == 0);
            GLCheck( glDrawElementsInstanced(
                        mCurrentPolygonMode,
                        cmd->primCount,
                        indexType,
                        reinterpret_cast<void*>(cmd->firstVertexIndex * bytesPerIndexElement),
                        cmd->instanceCount ) );
        }
    }

    void GLES2RenderSystem::_renderNoBaseInstance( const v1::CbDrawCallStrip *cmd )
    {
        GLSLESProgramCommon* activeProgram = NULL;
        if(Root::getSingleton().getRenderSystem()->getFeature()->hasCapability(CF_Program_SeparateObject))
            activeProgram = GLSLESProgramPipelineManager::getSingleton().getActiveProgramPipeline();
        else
            activeProgram = GLSLESLinkProgramManager::getSingleton().getActiveLinkProgram();

        GLCheck( glUniform1ui( activeProgram->getBaseInstanceLocation(),
                            static_cast<GLuint>( cmd->baseInstance ) ) );

        GLCheck( glDrawArraysInstanced(
                    mCurrentPolygonMode,
                    cmd->firstVertexIndex,
                    cmd->primCount,
                    cmd->instanceCount ) );
    }

    void GLES2RenderSystem::clearBuffer(unsigned int buffers,
                                            const Colour& colour,
                                            Real depth, unsigned short stencil)
    {
        bool colourMask = mBlendChannelMask != ShaderChBlend::BlendChannelAll;
        
        GLbitfield flags = 0;
        if (buffers & FBT_COLOUR)
        {
            flags |= GL_COLOR_BUFFER_BIT;
            // Enable buffer for writing if it isn't
            if (colourMask)
            {
                OGRE_CHECK_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
            }
            OGRE_CHECK_GL_ERROR(glClearColor(colour.r, colour.g, colour.b, colour.a));
        }
        if (buffers & FBT_DEPTH)
        {
            flags |= GL_DEPTH_BUFFER_BIT;
            // Enable buffer for writing if it isn't
            if(!mDepthWrite)
            {
                OGRE_CHECK_GL_ERROR(glDepthMask(GL_TRUE));
            }
            OGRE_CHECK_GL_ERROR(glClearDepthf(depth));
        }
        if (buffers & FBT_STENCIL)
        {
            flags |= GL_STENCIL_BUFFER_BIT;
            // Enable buffer for writing if it isn't
            OGRE_CHECK_GL_ERROR(glStencilMask(0xFFFFFFFF));
            OGRE_CHECK_GL_ERROR(glClearStencil(stencil));
        }

        RenderTarget* target = mActiveViewport->getTarget();
        bool scissorsNeeded = mActiveViewport->getPixelLeft() != 0 ||
                                mActiveViewport->getPixelTop() != 0 ||
                                mActiveViewport->getPixelWidth() != (int)target->getWidth() ||
                                mActiveViewport->getPixelHeight() != (int)target->getHeight();

        if( scissorsNeeded )
        {
            //We clear the viewport area. The Viewport may not
            //coincide with the current clipping region
            GLsizei x, y, w, h;
            w = mActiveViewport->getPixelWidth();
            h = mActiveViewport->getPixelHeight();
            x = mActiveViewport->getPixelLeft();
            y = mActiveViewport->getPixelTop();

            if( !target->isFlipping() )
            {
                // Convert "upper-left" corner to "lower-left"
                y = target->getHeight() - h - y;
            }

            OGRE_CHECK_GL_ERROR(glScissor(x, y, w, h));
        }

        if( scissorsNeeded && !mScissorsEnabled )
        {
            // Clear the buffers
            // Subregion clears need scissort tests enabled.
            OGRE_CHECK_GL_ERROR(glEnable(GL_SCISSOR_TEST));
            OGRE_CHECK_GL_ERROR(glClear(flags));
            OGRE_CHECK_GL_ERROR(glDisable(GL_SCISSOR_TEST));
        }
        else
        {
            // Clear the buffers
            // Either clearing the whole screen, or scissor test is already enabled.
            OGRE_CHECK_GL_ERROR(glClear(flags));
        }

        if( scissorsNeeded )
        {
            //Restore the clipping region
            GLsizei x, y, w, h;
            w = mActiveViewport->getClipPixelWidth();
            h = mActiveViewport->getClipPixelHeight();
            x = mActiveViewport->getClipPixelLeft();
            y = mActiveViewport->getClipPixelTop();

            if( !target->isFlipping() )
            {
                // Convert "upper-left" corner to "lower-left"
                y = target->getHeight() - h - y;
            }

            OGRE_CHECK_GL_ERROR(glScissor(x, y, w, h));
        }

        // Reset buffer write state
        if (!mDepthWrite && (buffers & FBT_DEPTH))
        {
            OGRE_CHECK_GL_ERROR(glDepthMask(GL_FALSE));
        }

        if (colourMask && (buffers & FBT_COLOUR))
        {
            GLboolean r = (mBlendChannelMask & ShaderChBlend::BlendChannelRed) != 0;
            GLboolean g = (mBlendChannelMask & ShaderChBlend::BlendChannelGreen) != 0;
            GLboolean b = (mBlendChannelMask & ShaderChBlend::BlendChannelBlue) != 0;
            GLboolean a = (mBlendChannelMask & ShaderChBlend::BlendChannelAlpha) != 0;
            GLCheck( glColorMask( r, g, b, a ) );
        }

        if (buffers & FBT_STENCIL)
        {
            OGRE_CHECK_GL_ERROR(glStencilMask(mStencilList[0].writeMask));
        }
    }

    void GLES2RenderSystem::discardFrameBuffer(unsigned int buffers)
    {
        //To GLES2 porting note:
        //GL_EXT_discard_framebuffer does not imply a clear.
        //GL_EXT_discard_framebuffer should be called after rendering
        //(Allows to omit writeback of unneeded data e.g. Z-buffers, Stencil)
        //On most renderers, not clearing (and invalidate is not clearing)
        //can put you in slow mode

        //GL_ARB_invalidate_subdata

        assert( mActiveRenderTarget );
        if( !mHasDiscardFramebuffer )
            return;

        GLsizei numAttachments = 0;
        GLenum attachments[NII_MAX_RenderTarget+2];

        GLES2FrameBufferObject *fbo = 0;
        mActiveRenderTarget->getPlatformData( "FBO", &fbo );

        if( fbo )
        {
            if( buffers & FBT_COLOUR )
            {
                for( size_t i=0; i<NII_MAX_RenderTarget; ++i )
                {
                    const GLES2SurfaceDesc &surfDesc = fbo->getSurface( i );
                    if( surfDesc.buffer )
                        attachments[numAttachments++] = static_cast<GLenum>( GL_COLOR_ATTACHMENT0 + i );
                }
            }

            GLES2DepthBuffer *depthBuffer = static_cast<GLES2DepthBuffer*>(
                                                mActiveRenderTarget->getDepthBuffer() );

            if( depthBuffer )
            {
                if( buffers & FBT_STENCIL && depthBuffer->getStencilBuffer() )
                    attachments[numAttachments++] = GL_STENCIL_ATTACHMENT;
                if( buffers & FBT_DEPTH )
                    attachments[numAttachments++] = GL_DEPTH_ATTACHMENT;
            }
        }
        else
        {
            if( buffers & FBT_COLOUR )
            {
                attachments[numAttachments++] = GL_COLOR;
                /*attachments[numAttachments++] = GL_BACK_LEFT;
                attachments[numAttachments++] = GL_BACK_RIGHT;*/
            }

            if( buffers & FBT_DEPTH )
                attachments[numAttachments++] = GL_DEPTH;
            if( buffers & FBT_STENCIL )
                attachments[numAttachments++] = GL_STENCIL;
        }

        assert( numAttachments && "Bad flags provided" );
        assert( (size_t)numAttachments <= sizeof(attachments) / sizeof(attachments[0]) );
#if OGRE_NO_GLES3_SUPPORT == 1
        OGRE_CHECK_GL_ERROR(glDiscardFramebufferEXT(GL_FRAMEBUFFER, numAttachments, attachments));
#else
        OGRE_CHECK_GL_ERROR(glInvalidateFramebuffer(GL_FRAMEBUFFER, numAttachments, attachments));
#endif
    }

    void GLES2RenderSystem::_switchContext(GLES2Context * context)
    {
        // Unbind GPU programs and rebind to new context later, because
        // scene manager treat render system as ONE 'context' ONLY, and it
        // cached the GPU programs using state.
        if (mVS)
            mVS->unbindProgram();
        if (mFS)
            mFS->unbindProgram();
        
        // Disable textures
        _disableTextureUnitsFrom(0);

        // It's ready for switching
        if(mCurrentContext != context)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            // EAGLContext::setCurrentContext does not flush automatically. everybody else does.
            // see https://developer.apple.com/library/content/qa/qa1612/_index.html
            glFlush();
#endif
            mCurrentContext->endCurrent();
            mCurrentContext = context;
        }
        mCurrentContext->setCurrent();

        // Check if the context has already done one-time initialisation
        if (!mCurrentContext->getInitialized())
        {
            _oneTimeContextInitialization();
            mCurrentContext->setInitialized();
        }

        // Rebind GPU programs to new context
        if( mPso )
        {
            if (mPso->mVS)
                mPso->mVS->bind();
            if (mPso->mFS)
                mPso->mFS->bind();
        }
        
        // Must reset depth/colour write mask to according with user desired, otherwise,
        // clearBuffer would be wrong because the value we are recorded may be
        // difference with the really state stored in GL context.
        OGRE_CHECK_GL_ERROR(glDepthMask(mDepthWrite));
        {
            GLboolean r = (mBlendChannelMask & ShaderChBlend::BlendChannelRed) != 0;
            GLboolean g = (mBlendChannelMask & ShaderChBlend::BlendChannelGreen) != 0;
            GLboolean b = (mBlendChannelMask & ShaderChBlend::BlendChannelBlue) != 0;
            GLboolean a = (mBlendChannelMask & ShaderChBlend::BlendChannelAlpha) != 0;
            GLCheck( glColorMask( r, g, b, a ) );
        }
        OGRE_CHECK_GL_ERROR(glStencilMask(mStencilList[0].writeMask));
    }

    void GLES2RenderSystem::_unregisterContext(GLES2Context *context)
    {
        if (mCurrentContext == context)
        {
            // Change the context to something else so that a valid context
            // remains active. When this is the main context being unregistered,
            // we set the main context to 0.
            if (mCurrentContext != mMainContext)
            {
                _switchContext(mMainContext);
            }
            else
            {
                // No contexts remain
                mCurrentContext->endCurrent();
                mCurrentContext = 0;
                mMainContext = 0;
            }
        }
    }

    void GLES2RenderSystem::_oneTimeContextInitialization()
    {
        OGRE_CHECK_GL_ERROR(glDisable(GL_DITHER));
        static_cast<GLES2TextureManager*>(mTextureManager)->createWarningTexture();

#if OGRE_NO_GLES3_SUPPORT == 0
        // Enable primitive restarting with fixed indices depending upon the data type
        OGRE_CHECK_GL_ERROR(glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX));
#endif

        if( checkGpuApi("GL_EXT_texture_filter_anisotropic") )
            GLCheck( glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mLargestSupportedAnisotropy) );
        
        GLCheck( glGenFramebuffers( 1, &mNullColourFramebuffer ) );
    }

    void GLES2RenderSystem::initialiseContext(RenderWindow* primary)
    {
        // Set main and current context
        mMainContext = 0;
        primary->getPlatformData("GLCONTEXT", &mMainContext);
        mCurrentContext = mMainContext;

        // Set primary context as active
        if (mCurrentContext)
            mCurrentContext->setCurrent();

        // Setup GLSupport
        mGLSupport->initialiseExtensions();

        LogManager::getSingleton().logMessage("**************************************");
        LogManager::getSingleton().logMessage("*** OpenGL ES 2.x Renderer Started ***");
        LogManager::getSingleton().logMessage("**************************************");
    }

    void GLES2RenderSystem::_setRenderTarget( RenderTarget *target, uint8 viewportRenderTargetFlags )
    {
        // Unbind frame buffer object
        if(mActiveRenderTarget && mRTTManager)
            mRTTManager->unbind(mActiveRenderTarget);

        mActiveRenderTarget = target;
        if (target && mRTTManager)
        {
            // Switch context if different from current one
            GLES2Context *newContext = 0;
            target->getPlatformData("GLCONTEXT", &newContext);
            if (newContext && mCurrentContext != newContext)
            {
                _switchContext(newContext);
            }

            // Check the FBO's depth buffer status
            GLES2DepthBuffer *depthBuffer = static_cast<GLES2DepthBuffer*>(target->getDepthBuffer());

            if( target->getDepthBufferPool() != DepthBuffer::GroupUnknow &&
                (!depthBuffer || depthBuffer->getGLContext() != mCurrentContext ) )
            {
                // Depth is automatically managed and there is no depth buffer attached to this RT
                // or the Current context doesn't match the one this Depth buffer was created with
                setDepthBufferFor( target, true );
            }

            depthBuffer = static_cast<GLES2DepthBuffer*>(target->getDepthBuffer());

            if( target->getForceDisableColourWrites() )
                viewportRenderTargetFlags &= ~VP_RTT_COLOUR_WRITE;

            // Bind frame buffer object
            mRTTManager->bind(target);

            if( !(viewportRenderTargetFlags & VP_RTT_COLOUR_WRITE) )
            {
                assert(!target->isRenderWindow());
                assert(depthBuffer);
                
                GLCheck( glBindFramebuffer( GL_FRAMEBUFFER, mNullColourFramebuffer ) );

                if( depthBuffer )
                {
                    //Attach the depth buffer to this no-colour framebuffer
                    depthBuffer->bindToFramebuffer();
                }
            }
            else
            {
                // ...
            }
            
        }
    }

    GLint GLES2RenderSystem::convertCompareFunction(CmpMode func) const
    {
        switch(func)
        {
            case CMPF_ALWAYS_FAIL:
                return GL_NEVER;
            case CMPF_ALWAYS_PASS:
                return GL_ALWAYS;
            case CMPF_LESS:
                return GL_LESS;
            case CMPF_LESS_EQUAL:
                return GL_LEQUAL;
            case CMPF_EQUAL:
                return GL_EQUAL;
            case CMPF_NOT_EQUAL:
                return GL_NOTEQUAL;
            case CMPF_GREATER_EQUAL:
                return GL_GEQUAL;
            case CMPF_GREATER:
                return GL_GREATER;
            case NUM_COMPARE_FUNCTIONS:
                return GL_ALWAYS; // To keep compiler happy
        };
        // To keep compiler happy
        return GL_ALWAYS;
    }

    GLint GLES2RenderSystem::convertStencilOp(StencilOpType op, bool invert) const
    {
        switch(op)
        {
        case SOP_KEEP:
            return GL_KEEP;
        case SOP_ZERO:
            return GL_ZERO;
        case SOP_REPLACE:
            return GL_REPLACE;
        case SOP_INCREMENT:
            return invert ? GL_DECR : GL_INCR;
        case SOP_DECREMENT:
            return invert ? GL_INCR : GL_DECR;
        case SOP_INCREMENT_WRAP:
            return invert ? GL_DECR_WRAP : GL_INCR_WRAP;
        case SOP_DECREMENT_WRAP:
            return invert ? GL_INCR_WRAP : GL_DECR_WRAP;
        case SOP_INVERT:
            return GL_INVERT;
        };
        // to keep compiler happy
        return SOP_KEEP;
    }

    void GLES2RenderSystem::on(GpuProgramType gptype, GpuProgramParam * params, uint16 mask)
    {
        // Just copy
        params->syncValue();
        switch (gptype)
        {
        case GPT_VERTEX_PROGRAM:
            mActiveVGPP = params;
            mPso->mVS->bindSharedParameters(params, mask);
            break;
        case GPT_FRAGMENT_PROGRAM:
            mActiveFGPP = params;
            mPso->mFS->bindSharedParameters(params, mask);
            break;
        default:
            break;
        }

        switch (gptype)
        {
            case GPT_VERTEX_PROGRAM:
                mActiveVGPP = params;
                mPso->mVS->bindParameters(params, mask);
                break;
            case GPT_FRAGMENT_PROGRAM:
                mActiveFGPP = params;
                mPso->mFS->bindParameters(params, mask);
                break;
            default:
                break;
        }
    }

    void GLES2RenderSystem::bindGpuProgramPassIterationParameters(GpuProgramType gptype)
    {
        switch (gptype)
        {
            case GPT_VERTEX_PROGRAM:
                mPso->mVS->bindPassIterationParameters(mActiveVGPP);
                break;
            case GPT_FRAGMENT_PROGRAM:
                mPso->mFS->bindPassIterationParameters(mActiveFGPP);
                break;
            default:
                break;
        }
    }

    void GLES2RenderSystem::registerThread()
    {
        OGRE_LOCK_MUTEX(mThreadInitMutex);
        // This is only valid once we've created the main context
        if (!mMainContext)
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Cannot register a background thread before the main context has been created.");
        }

        // Create a new context for this thread. Cloning from the main context
        // will ensure that resources are shared with the main context
        // We want a separate context so that we can safely create GL
        // objects in parallel with the main thread
        GLES2Context* newContext = mMainContext->clone();
        mBackgroundContextList.push_back(newContext);

        // Bind this new context to this thread.
        newContext->setCurrent();

        _oneTimeContextInitialization();
        newContext->setInitialized();
    }

    void GLES2RenderSystem::unregisterThread()
    {
        // nothing to do here?
        // Don't need to worry about active context, just make sure we delete
        // on shutdown.
    }

    void GLES2RenderSystem::preExtraThreadsStarted()
    {
        OGRE_LOCK_MUTEX(mThreadInitMutex);
        // free context, we'll need this to share lists
        if(mCurrentContext)
            mCurrentContext->endCurrent();
    }

    void GLES2RenderSystem::postExtraThreadsStarted()
    {
        OGRE_LOCK_MUTEX(mThreadInitMutex);
        // reacquire context
        if(mCurrentContext)
            mCurrentContext->setCurrent();
    }

    unsigned int GLES2RenderSystem::getDisplayMonitorCount() const
    {
        return 1;
    }

    //---------------------------------------------------------------------
    void GLES2RenderSystem::beginProfiler( const String &eventName )
    {
        if(checkGpuApi("GL_EXT_debug_marker"))
            glPushGroupMarkerEXT(0, eventName.c_str());
    }
    //---------------------------------------------------------------------
    void GLES2RenderSystem::endProfiler( void )
    {
        if(checkGpuApi("GL_EXT_debug_marker"))
            glPopGroupMarkerEXT();
    }
    //---------------------------------------------------------------------
    void GLES2RenderSystem::markProfiler( const String &eventName )
    {
        if( eventName.empty() )
            return;

        if(checkGpuApi("GL_EXT_debug_marker"))
           glInsertEventMarkerEXT(0, eventName.c_str());
    }
    //---------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    void GLES2RenderSystem::resetRenderer(RenderWindow* win)
    {
        LogManager::getSingleton().logMessage("********************************************");
        LogManager::getSingleton().logMessage("*** OpenGL ES 2.x Reset Renderer Started ***");
        LogManager::getSingleton().logMessage("********************************************");
                
        initialiseContext(win);
        
        static_cast<GLES2FBOManager*>(mRTTManager)->_reload();
        
        _destroyDepthBuffer(win);
        
        GLES2DepthBuffer *depthBuffer = OGRE_NEW GLES2DepthBuffer( DepthBuffer::GroupInner, this,
                                                                  mMainContext, GL_NONE, GL_NONE,
                                                                  win->getWidth(), win->getHeight(),
                                                                  win->getFSAA(), 0, PF_UNKNOWN,
                                                                  false, true );
        
        mDepthBufferPool[depthBuffer->getPoolId()].push_back( depthBuffer );
        win->attachDepthBuffer( depthBuffer, false );
        
        GLES2RenderSystem::mResourceManager->notifyOnContextReset();
        
        _setViewport(NULL);
        _setRenderTarget(win);
    }
    
    GLES2ManagedResourceManager* GLES2RenderSystem::getResourceManager()
    {
        return GLES2RenderSystem::mResourceManager;
    }
#endif

    void GLES2RenderSystem::setupGPUProfiler(void)
    {
    }

    void GLES2RenderSystem::shutdownGPUProfiler(void)
    {
    }

    void GLES2RenderSystem::beginGPUProfiler( const String &name, uint32 *hashCache )
    {
    }

    void GLES2RenderSystem::endGPUProfiler( const String &name )
    {
    }


    bool GLES2RenderSystem::activateGLTextureUnit(size_t unit)
    {
        // Always return true for the currently bound texture unit
        if (mActiveTextureUnit == unit)
            return true;
        
        if (unit < dynamic_cast<GLES2RenderSystem*>(Root::getSingleton().getRenderSystem())->getFeature()->getMaxTexUnit())
        {
            OGRE_CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + unit));
            
            mActiveTextureUnit = unit;
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void GLES2RenderSystem::bindVertexElementToGpu( const v1::VertexElement &elem,
                                                    v1::HardwareVertexBufferSharedPtr vertexBuffer,
                                                    const size_t vertexStart,
                                                    vector<GLuint>::type &attribsBound,
                                                    vector<GLuint>::type &instanceAttribsBound,
                                                    bool updateVAO)
    {
        const v1::GLES2HardwareVertexBuffer* hwGlBuffer = static_cast<const v1::GLES2HardwareVertexBuffer*>(vertexBuffer.get());

        // FIXME: Having this commented out fixes some rendering issues but leaves VAO's useless
        if (updateVAO)
        {
            OGRE_CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                                             hwGlBuffer->getGLBufferId()));
            void* pBufferData = GL_BUFFER_OFFSET(elem.getOffset());

            if (vertexStart)
            {
                pBufferData = static_cast<char*>(pBufferData) + vertexStart * vertexBuffer->getVertexSize();
            }

            VertexType sem = elem.getSemantic();
            unsigned short typeCount = v1::VertexElement::getTypeCount(elem.getType());
            GLboolean normalised = GL_FALSE;
            GLuint attrib = 0;
            unsigned short elemIndex = elem.getIndex();

            if(Root::getSingleton().getRenderSystem()->getFeature()->hasCapability(CF_Program_SeparateObject))
            {
                GLSLESProgramPipeline* programPipeline =
                GLSLESProgramPipelineManager::getSingleton().getActiveProgramPipeline();
                if (!programPipeline || !programPipeline->isAttributeValid(sem, elemIndex))
                {
                    return;
                }

                attrib = (GLuint)programPipeline->getAttributeIndex(sem, elemIndex);
            }
            else
            {
                GLSLESLinkProgram* linkProgram = GLSLESLinkProgramManager::getSingleton().getActiveLinkProgram();
                if (!linkProgram || !linkProgram->isAttributeValid(sem, elemIndex))
                {
                    return;
                }

                attrib = (GLuint)linkProgram->getAttributeIndex(sem, elemIndex);
            }

            if(hasMinGLVersion(3, 0) || checkGpuApi("GL_EXT_instanced_arrays"))
            {
                if (mPso->mVS)
                {
                    if (hwGlBuffer->getIsInstanceData())
                    {
                        OGRE_CHECK_GL_ERROR(glVertexAttribDivisorEXT(attrib, static_cast<GLuint>(hwGlBuffer->getInstanceDataStepRate())));
                        instanceAttribsBound.push_back(attrib);
                    }
                }
            }

            switch(elem.getType())
            {
                case VET_COLOUR:
                case VET_COLOUR_ABGR:
                case VET_COLOUR_ARGB:
                    // Because GL takes these as a sequence of single unsigned bytes, count needs to be 4
                    // VertexElement::getTypeCount treats them as 1 (RGBA)
                    // Also need to normalise the fixed-point data
                    typeCount = 4;
                    normalised = GL_TRUE;
                    break;
                default:
                    break;
            };

            OGRE_CHECK_GL_ERROR(glVertexAttribPointer(attrib,
                                                      typeCount,
                                                      v1::GLES2HardwareBufferManager::getGLType(elem.getType()),
                                                      normalised,
                                                      static_cast<GLsizei>(vertexBuffer->getVertexSize()),
                                                      pBufferData));

            // If this attribute hasn't been enabled, do so and keep a record of it.
            OGRE_CHECK_GL_ERROR(glEnableVertexAttribArray(attrib));
            
            attribsBound.push_back(attrib);
        }
    }

    /*const PixelFormatToShaderType* GLES2RenderSystem::getPixelFormatToShaderType(void) const
    {
        return &mPixelFormatToShaderType;
    }*/
}
