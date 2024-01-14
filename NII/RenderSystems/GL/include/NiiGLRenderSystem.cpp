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

#include "NiiGLRenderSystem.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiLight.h"
#include "NiiCamera.h"
#include "NiiGLTextureManager.h"
#include "NiiGLHardwareVertexBuffer.h"
#include "NiiGLHardwarePixelBuffer.h"
#include "NiiGLGBufferManager.h"
#include "NiiGLGpuProgramManager.h"
#include "NiiGLUtil.h"
#include "NiiGLGpuNvparseProgram.h"
#include "NiiGLSLProgramBase.h"
#include "ATI_FS_GLGpuProgram.h"
#include "NiiGLSLExtSupport.h"
#include "NiiGLPixelCountQuery.h"
#include "NiiGLPixelFormat.h"
#include "NiiGLDepthBuffer.h"
#include "NiiGLFBOMultiRenderTarget.h"
#include "NiiGLContext.h"
#include "NiiGLSLShaderFactory.h"
#include "NiiGLFBORenderTexture.h"
#include "NiiGLPBRenderTexture.h"
#include "NiiMultiTextureFrame.h"
#include "NiiGLTexture.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiViewWindow.h"

// Convenience macro from ARB_vertex_buffer_object spec
#define VBO_BUFFER_OFFSET(i) ((char *)(i))

#define INFINITE_FAR_PLANE_ADJUST 0.00001f

#include <sstream>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
extern "C" void glFlushRenderAPPLE();
#endif

#if N_DEBUG
static void APIENTRY GLDebugCallback(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar* message,
                                     const void* userParam)
{
    if( type == GL_DEBUG_TYPE_PUSH_GROUP || type == GL_DEBUG_TYPE_POP_GROUP )
        return; //Ignore these

    char debSource[32], debType[32], debSev[32];

    if (source == GL_DEBUG_SOURCE_API)
        strcpy(debSource, "OpenGL");
    else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
        strcpy(debSource, "Windows");
    else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER)
        strcpy(debSource, "Shader Compiler");
    else if (source == GL_DEBUG_SOURCE_THIRD_PARTY)
        strcpy(debSource, "Third Party");
    else if (source == GL_DEBUG_SOURCE_APPLICATION)
        strcpy(debSource, "Application");
    else if (source == GL_DEBUG_SOURCE_OTHER)
        strcpy(debSource, "Other");

    if (type == GL_DEBUG_TYPE_ERROR)
        strcpy(debType, "error");
    else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        strcpy(debType, "deprecated behavior");
    else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        strcpy(debType, "undefined behavior");
    else if (type == GL_DEBUG_TYPE_PORTABILITY)
        strcpy(debType, "portability");
    else if (type == GL_DEBUG_TYPE_PERFORMANCE)
        strcpy(debType, "performance");
    else if (type == GL_DEBUG_TYPE_OTHER)
        strcpy(debType, "message");
    else if (type == GL_DEBUG_TYPE_PUSH_GROUP)
        strcpy(debType, "push");
    else if (type == GL_DEBUG_TYPE_POP_GROUP)
        strcpy(debType, "pop");

    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        strcpy(debSev, "high");
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
        strcpy(debSev, "medium");
    else if (severity == GL_DEBUG_SEVERITY_LOW)
        strcpy(debSev, "low");
    else if( severity == GL_DEBUG_SEVERITY_NOTIFICATION )
    {
        strcpy(debSev, "notification");

#if OGRE_PROFILING == 0
        //Filter notification debug GL messages as they can
        //be quite noisy, annoying and useless on NVIDIA.
        return;
#endif
    }
    else
        strcpy(debSev, "unknown");

    N_Only(Log).stream() << debSource << ":" << debType << "(" << debSev << ") " << id << ": " << message;
}
#endif

#if N_PLAT == N_PLAT_OSX
extern "C" void glFlushRenderAPPLE();
#endif

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

namespace NII
{
	//---------------------------------------------------------------------
    static GLNativeSupport* glsupport;
    static GL3WglProc get_proc(const char* proc) {
        return (GL3WglProc)glsupport->getProcAddress(proc);
    }
    //---------------------------------------------------------------------
    static const GLint __CBM_GL_FUNC[CBM_COUNT] =
    {
        GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT,
        GL_MIN, GL_MAX
    };
    //-----------------------------------------------------------------------
    // Callback function used when registering GLGpuPrograms
    GpuProgram * createGLArbGpuProgram(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, GpuProgram::ShaderType type, ShaderLanguage lang)
    {
        GLArbGpuProgram * re = N_new GLArbGpuProgram(rid, gid, ls, rs);
        re->setType(type);
        re->setSyntaxType(lang);
        return re;
    }
    //-----------------------------------------------------------------------
    GpuProgram * createGLGpuNvparseProgram(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, GpuProgram::ShaderType type, ShaderLanguage lang)
    {
        GLGpuNvparseProgram * re = N_new GLGpuNvparseProgram(rid, gid, ls, rs);
        re->setType(type);
        re->setSyntaxType(lang);
        return re;
    }
    //-----------------------------------------------------------------------
    GpuProgram * createGL_ATI_FS_GpuProgram(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, GpuProgram::ShaderType type, ShaderLanguage lang)
    {
        ATI_FS_GLGpuProgram * re = N_new ATI_FS_GLGpuProgram(rid, gid, ls, rs);
        re->setType(type);
        re->setSyntaxType(lang);
        return re;
    }
    //---------------------------------------------------------------------
    GLuint getCombinedMinMipFilter(TextureFilterOP min, TextureFilterOP mip)
    {
        switch (min)
        {
        case TFOP_ANISOTROPIC:
        case TFOP_LINEAR:
            switch (mip)
            {
            case TFOP_ANISOTROPIC:
            case TFOP_LINEAR:
                // linear min, linear mip
                return GL_LINEAR_MIPMAP_LINEAR;
            case TFOP_POINT:
                // linear min, point mip
                return GL_LINEAR_MIPMAP_NEAREST;
            case TFOP_NONE:
                // linear min, no mip
                return GL_LINEAR;
            }
            break;
        case TFOP_POINT:
        case TFOP_NONE:
            switch (mip)
            {
            case TFOP_ANISOTROPIC:
            case TFOP_LINEAR:
                // nearest min, linear mip
                return GL_NEAREST_MIPMAP_LINEAR;
            case TFOP_POINT:
                // nearest min, point mip
                return GL_NEAREST_MIPMAP_NEAREST;
            case TFOP_NONE:
                // nearest min, no mip
                return GL_NEAREST;
            }
            break;
        }
        // should never get here
        return 0;
    }
    //-----------------------------------------------------------------------
    GLRenderStateObject::GLRenderStateObject()
    {
        memset(rso, 0, sizeof(GLRenderStateObject));
    }
    //-----------------------------------------------------------------------
    GLRenderSystem::GLRenderSystem() :
        mStopRendering(false),
        mFixedFunctionTextureUnits(0),
        mDepthWrite(true),
        mScissorsEnabled(false),
        mFrontStencilMask(0xFFFFFFFF),
        mBackStencilMask(0xFFFFFFFF),
        mUseAutoTextureMatrix(false),
        mHardwareBufferManager(0),
        mGpuProgramManager(0),
        mGLSLProgramFactory(0),
        mState(0),
        mActiveTextureUnit(0),
        mMaxBuiltInTextureAttribIndex(0),
        mColourWrite(CM_RGBA),
        mGlobalVao(0),
        mHasArbInvalidateSubdata(false),
        mNullColourFramebuffer(0)
    {
        LogManager::getOnly().log(getName() + _I18n(" created."));

        //mRenderAttribsBound.reserve(100);
        //mRenderInstanceAttribsBound.reserve(100);

        // Get our GLSupport
        mGLSupport = getGLSupport();
        glsupport = mGLSupport;
        
        mWorldMatrix = Matrix4<NIIf>::IDENTITY;
        mViewMatrix = Matrix4<NIIf>::IDENTITY;

        NCount i;
        for (i = 0; i < MAX_LIGHTS; ++i)
            mLights[i] = NULL;

        setupConfig();

        for (i = 0; i < NII_MAX_TextureLayer; ++i)
        {
            mTextureCoordIndex[i] = 99;
            mTextureTypes[i] = GL_TEXTURE_2D;
        }

        mActiveFrame = 0;
        mCurrentContext = 0;
        mMainContext = 0;

        mGLInitialised = false;
        mVertexTextureShared = true;
        mCurrentLights = 0;
        mMinFilter = TFOP_LINEAR;
        mMipFilter = TFOP_POINT;
        mCS = 0;
        mLargestSupportedAnisotropy = 1;
        mRTTManager = 0;

        N_log(getName() + " created.");

        mUseAdjacency = 0;
        mTextureMipmapCount = 0;
        mIndirectPtr = 0;
        mFirstUavBoundSlot = 255;
        mLastUavBoundPlusOne = 0;
        mClipDistances = 0;
        mRSO = 0;
    }
    //-----------------------------------------------------------------------
    GLRenderSystem::~GLRenderSystem()
    {
        shutdown();

        if(mGLSupport)
            N_delete mGLSupport;
    }
    //-----------------------------------------------------------------------
    const String & GLRenderSystem::getName(void) const
    {
        static String strName(_T("OpenGL_3-4 Rendering Subsystem"));
        return strName;
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::init()
    {
        RenderSys::init();

        mGLSupport->start();
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::setFFPParam(const GpuProgramParam & param, Nmark typemark)
    {
        bool updateLightPos = false;

        // Autoconstant index is not a physical index
        for (const auto& ac : param.getSyncParamList())
        {
            // Only update needed slots
            if (ac.mGBTMark & typemark)
            {
                const float* ptr = (NIIf *)param.getBufferData(ac.mMemIndex);
                switch (ac.mSyncParam)
                {
                case GSP_Matrix_Space:
                    _setModel(Matrix4f(ptr));
                    break;
                case GSP_Matrix_View:
                {
                    // force light update
                    updateLightPos = true;
                    typemark |= GPT_Light;
                    _setView(Matrix4f(ptr));
                }
                    break;
                case GSP_Matrix_Proj:
                {
                    glMatrixMode(GL_PROJECTION);
                    glLoadMatrixf(Matrix4f(ptr).T()[0]);
                    glMatrixMode(GL_MODELVIEW);

                    // also mark clip planes dirty
                    if (mClipPlanes.size())
                        mClipValid = false;
                }
                    break;
                case GSP_Ch_Ambient:
                {
                    NIIi extdata;
                    if (param.getExtData(12, extdata))
                    {
                        _setSurfaceTracking(extdata);
                        mState->setMaterialAmbient(ptr[0], ptr[1], ptr[2], ptr[3]);
                    }
                }
                    break;
                case GSP_Ch_Diffuse:
                    mState->setMaterialDiffuse(ptr[0], ptr[1], ptr[2], ptr[3]);
                    break;
                case GSP_Ch_Specular:
                    mState->setMaterialSpecular(ptr[0], ptr[1], ptr[2], ptr[3]);
                    break;
                case GSP_Ch_Emissive:
                    mState->setMaterialEmissive(ptr[0], ptr[1], ptr[2], ptr[3]);
                    break;
                case GSP_Ch_Shininess:
                    mState->setMaterialShininess(ptr[0]);
                    break;
                case GSP_Ponit_Params:
                    mState->setPointSize(ptr[0]);
                    mState->setPointParameters(ptr + 1);
                    break;
                case GSP_Fog_Params:
                {
                    NIIi extdata;
                    if (param.getExtData(18, extdata))
                    {
                        bool enable = _setFog((FogMode)extdata);
                        if (enable)
                        {
                            glFogf(GL_FOG_DENSITY, ptr[0]);
                            glFogf(GL_FOG_START, ptr[1]);
                            glFogf(GL_FOG_END, ptr[2]);
                            glFogfv(GL_FOG_COLOR, ptr + 4);
                        }
                    }
                }
                    break;
                //case GSP_Fog_Colour:
                //    glFogfv(GL_FOG_COLOR, ptr);
                //    break;
                case GSP_Ambient:
                    mState->setLightAmbient(ptr[0], ptr[1], ptr[2]);
                    break;
                case GSP_Light_Diffuse_ParamIndex:
                    glLightfv(GL_LIGHT0 + ac.mInputInt, GL_DIFFUSE, ptr);
                    break;
                case GSP_Light_Specular_ParamIndex:
                    glLightfv(GL_LIGHT0 + ac.mInputInt, GL_SPECULAR, ptr);
                    break;
                case GSP_Light_Attenuation_ParamIndex:
                    glLightf(GL_LIGHT0 + ac.mInputInt, GL_CONSTANT_ATTENUATION, ptr[1]);
                    glLightf(GL_LIGHT0 + ac.mInputInt, GL_LINEAR_ATTENUATION, ptr[2]);
                    glLightf(GL_LIGHT0 + ac.mInputInt, GL_QUADRATIC_ATTENUATION, ptr[3]);
                    break;
                case GSP_Light_Spot_Params_ParamIndex:
                {
                    float cutoff = ptr[3] ? Radian(std::acos(ptr[1])).getAngle() : 180;
                    glLightf(GL_LIGHT0 + ac.mInputInt, GL_SPOT_CUTOFF, cutoff);
                    glLightf(GL_LIGHT0 + ac.mInputInt, GL_SPOT_EXPONENT, ptr[2]);
                    break;
                }
                case GSP_Light_Pos_ParamIndex:
                case GSP_Light_Dir_ParamIndex:
                    updateLightPos = true;
                    break;
                default:
                    N_assert(false, "unknown autoconstant");
                    break;
                }
            }
        }

        if (!updateLightPos) 
            return;

        // GL lights use eye coordinates, which we only know now

        // Save previous modelview
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(mViewMatrix.T()[0]);

        for (const auto& ac : param.getSyncParamList())
        {
            // Only update needed slots
            if ((GPT_Render | GPT_Light) & typemark)
            {
                const float* ptr = (float *)param.getBufferData(ac.mMemIndex);
                switch (ac.mSyncParam)
                {
                case GSP_Light_Pos_ParamIndex:
                    glLightfv(GL_LIGHT0 + ac.mInputInt, GL_POSITION, ptr);
                    break;
                case GSP_Light_Dir_ParamIndex:
                    glLightfv(GL_LIGHT0 + ac.mInputInt, GL_SPOT_DIRECTION, ptr);
                    break;
                default:
                    break;
                }
            }
        }
        glPopMatrix();
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::setupFeature()
    {
        mVendorFeature = N_new RenderFeature();
        GPUInfo * info = const_cast<GPUInfo *>(mVendorFeature->getGpuInfo());
        mVendorFeature->add(GF_GL);
        mVendorFeature->add(GF_GL_3_1);
        info->setVersion(mVersion);
        const NIIb * deviceName = (const NIIb *)glGetString(GL_RENDERER);
        const NIIb * vendorName = (const NIIb *)glGetString(GL_VENDOR);
#if NII_STRING
        String tempstr;
        DataStream::s2ws(deviceName, tempstr);
        info->setName(tempstr);
#else
        info->setName(deviceName);
#endif
        mVendorFeature->setName(getName());
        info->setVendor(vendorName);

        mVendorFeature->add(GF_Fixed_Pipeline);
        mVendorFeature->add(GF_Tex_AutoMipmapCompress);

        // Also check GL_MAX_TEXTURE_IMAGE_UNITS_ARB since NV at least
        // only increased this on the FX/6x00 series
        GLint arbUnits;
        GLCheck(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &arbUnits));
        mVendorFeature->setMaxTexUnit(std::min(NII_MAX_TextureLayer, arbUnits));

        // Check for Anisotropy support
        if(GLEW_EXT_texture_filter_anisotropic)
        {
            GLfloat maxAnisotropy = 0;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            mVendorFeature->setMaxAnisotropy(maxAnisotropy);
            mVendorFeature->add(GF_Tex_Anisotropy);
        }

        mVendorFeature->add(GF_Tex_Dot3);

        // Point sprites
        mVendorFeature->add(GF_Point_Sprites);
        mVendorFeature->add(GF_Point_Param);

        // Check for hardware stencil support and set bit depth
        GLint stencil;
        glGetIntegerv(GL_STENCIL_BITS,&stencil);
        if(stencil)
        {
            mVendorFeature->add(GF_HW_Stencil);
            mVendorFeature->add(GF_Stencil_BothSide);
            mVendorFeature->setStencilBit(stencil);
        }

        mVendorFeature->add(GF_HW_Buffer);
        mVendorFeature->add(GF_HW_GAMMA);
        mVendorFeature->add(GF_Index_32);
        mVendorFeature->add(GF_GL_VArrayObj);
        
        bool hasGL40 = isVersionNewer(4, 0);
        bool hasGL41 = isVersionNewer(4, 1);
        bool hasGL42 = isVersionNewer(4, 2);

        // Check for etc compression
        if (mGLSupport->checkGpuApi("GL_ARB_ES3_compatibility") || mIs43)
        {
            mVendorFeature->add(GF_Tex_Fromat_ETC2);
        }
        
        if( mGLSupport->checkGpuApi( "GL_KHR_texture_compression_astc_ldr" ) )
            mVendorFeature->add(GF_Tex_Fromat_ASTC);

        // Vertex Program
        if(GLEW_ARB_vertex_program)
        {
            mVendorFeature->add(GF_Program_Vertex);
            mVendorFeature->addShader(SL_ARBVP1);
            
            GLint fc;
            GLCheck(glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &fc);
            mVendorFeature->setMaxVertexProgramParam(fc, 0, fc, fc);

            GLCheck(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &fc));
            mVendorFeature->setMaxVertexAttrib(fc);

            if(GLEW_NV_vertex_program2_option)
            {
                mVendorFeature->addShader(SL_VP_30);
            }

            if(GLEW_NV_vertex_program3)
            {
                mVendorFeature->addShader(SL_VP_40);
            }

            if(GLEW_NV_vertex_program4)
            {
                mVendorFeature->addShader(SL_GP4_VP | SL_GPU_VP);
            }
        }

        if (GLEW_NV_register_combiners2 && GLEW_NV_texture_shader)
        {
            mVendorFeature->add(GF_Program_Fragment);
            mVendorFeature->addShader(SL_FP_20);
        }

        // NFZ - check for ATI fragment shader support
        if (GLEW_ATI_fragment_shader)
        {
            mVendorFeature->add(GF_Program_Fragment);
            // no boolean params allowed
            // no integer params allowed
            // only 8 Vector4<NIIf> constant floats supported
            mVendorFeature->setMaxFragmentProgramParam(8, 0, 0, 0);

            mVendorFeature->addShader(SL_PS_1_4 | SL_PS_1_3 | SL_PS_1_2 | SL_PS_1_1);
        }

        // Fragment Program
        if(GLEW_ARB_fragment_program)
        {
            mVendorFeature->add(GF_Program_Fragment);
            mVendorFeature->addShader(SL_ARBFP1);
            
            GLint fc;
            GLCheck(glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &fc);
            mVendorFeature->setMaxFragmentProgramParam(fc, 0, fc, fc);

            if (GLEW_NV_fragment_program_option)
            {
                mVendorFeature->addShader(SL_FP_30);
            }

            if (GLEW_NV_fragment_program2)
            {
                mVendorFeature->addShader(SL_FP_40);
            }

            if (GLEW_NV_fragment_program4)
            {
                mVendorFeature->addShader(SL_GP4_FP | SL_GPU_FP);
            }
        }
        
        // Geometry shaders
        if(GLEW_EXT_geometry_shader4)
        {
            mVendorFeature->add(GF_Program_Geometry);
            mVendorFeature->addShader(SL_NV_GP4);

            //Also add the CG profiles
            mVendorFeature->addShader(SL_GPU_GP | SL_GP4_GP);

            GLint ccount = 0;
            glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &ccount);
            mVendorFeature->setMaxGeometryProgramParam(ccount, 0, ccount, ccount);

            glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &ccount);
            mVendorFeature->setMaxGeometryOutput(ccount);
        }
        
        // Tessellation Program
        if (hasGL40 || checkGpuApi("GL_ARB_tessellation_shader"))
        {
            mVendorFeature->add(GF_Program_Tess);
            mVendorFeature->add(GF_Program_Domain);

            GLint ccount = 0;
            GLCheck(glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS, &ccount));
            mVendorFeature->setMaxTessControlProgramParam(ccount, 0, ccount, ccount);

            GLCheck(glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, &ccount));
            mVendorFeature->setMaxDomainProgramParam(ccount, 0, ccount, ccount);
        }

        // Compute Program
        if (mIs43 || checkGpuApi("GL_ARB_compute_shader"))
        {
            mVendorFeature->add(GF_Program_Compute);

            //FIXME Is this correct?
            GLCheck(glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &ccount));
            mVendorFeature->setMaxComputeProgramParam(ccount, 0, ccount, ccount);

            //TODO we should also check max workgroup count & size
            // GLCheck(glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, &workgroupCount));
            // GLCheck(glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroupInvocations));
        }

        bool limitedOSXCoreProfile = N_PLAT == N_PLAT_OSX && isVersionNewer(3, 2);
        
        mVendorFeature->addShader(SL_GLSL);
        if (mShaderVersion >= 330 && !limitedOSXCoreProfile)
            mVendorFeature->addShader(SL_GLSL_3_3);
        if (mShaderVersion >= 150 && !limitedOSXCoreProfile)
            mVendorFeature->addShader(SL_GLSL_1_5);
        if (mShaderVersion >= 140 && !limitedOSXCoreProfile)
            mVendorFeature->addShader(SL_GLSL_1_4);
        if (mShaderVersion >= 130 && !limitedOSXCoreProfile)
            mVendorFeature->addShader(SL_GLSL_1_3);
        if (mShaderVersion >= 120 && !limitedOSXCoreProfile)
            mVendorFeature->addShader(SL_GLSL_1_2);
        if (mShaderVersion >= 110 && !limitedOSXCoreProfile)
            mVendorFeature->addShader(SL_GLSL_1_1);
        if (mShaderVersion >= 100 && !limitedOSXCoreProfile)
            mVendorFeature->addShader(SL_GLSL_1_0);
        
        if(GL_ARB_shading_language_420pack)
            mVendorFeature->addShader(SL_GLSL_4_2);
        if (mShaderVersion >= 430)
            mVendorFeature->addShader(SL_GLSL_4_3);
        if (mShaderVersion >= 440)
            mVendorFeature->addShader(SL_GLSL_4_4);

        if(hasGL41 || checkGpuApi("GL_ARB_get_program_binary"))
        {
            // states 3.0 here: http://developer.download.nvidia.com/opengl/specs/GL_ARB_get_program_binary.txt
            // but not here: http://www.opengl.org/sdk/docs/man4/xhtml/glGetProgramBinary.xml
            // and here states 4.1: http://www.geeks3d.com/20100727/opengl-4-1-allows-the-use-of-binary-shaders/
            GLint formats;
            glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);

            if (formats > 0)
                mVendorFeature->add(GF_Shader_Binary_Get);
        }

        if(isVersionNewer(3, 3) || GLEW_ARB_instanced_arrays)
        {
            // states 3.3 here: http://www.opengl.org/sdk/docs/man3/xhtml/glVertexAttribDivisor.xml
            mVendorFeature->add(GF_Vertex_Instance);
        }

        //Check if render to vertex buffer (transform feedback in OpenGL)
        if(GLEW_NV_transform_feedback)
        {
            mVendorFeature->add(GF_Render_To_Vertex);
        }

        mVendorFeature->add(GF_Tex_Compress);
        
        if (checkGpuApi("WEBGL_compressed_texture_astc") || checkGpuApi("GL_KHR_texture_compression_astc_ldr"))
            mVendorFeature->add(GF_Tex_Fromat_ASTC);

        // Check for dxt compression
        if(GLEW_EXT_texture_compression_s3tc)
        {
#if defined(__APPLE__) && defined(__PPC__)
            // Apple on ATI & PPC has errors in DXT
            if(mGLSupport->getGLVendor().find("ATI") == std::string::npos)
#endif
                mVendorFeature->add(GF_Tex_Fromat_DXT);
        }
        // Check for vtc compression
        if(GLEW_NV_texture_compression_vtc)
        {
            mVendorFeature->add(GF_Tex_Fromat_VTC);
        }

        // Check for etc compression
        if (mIs43 || GLEW_ARB_ES3_compatibility)
        {
            mVendorFeature->add(GF_Tex_Fromat_ETC2);
        }
        
        if (isVersionNewer(3, 0))
        {
            mVendorFeature->add(GF_Tex_Fromat_BC4_5);
        }
        
        if (hasGL42 || GLEW_ARB_texture_compression_bptc)
        {
            mVendorFeature->add(GF_Tex_Fromat_BC6H_7);
        }
        
        if (hasGL42 || checkGpuApi("GL_ARB_shader_atomic_counters"))
        {
            mVendorFeature->add(GF_Atomic_Counters);
        }
        
        if (hasGL41 || checkGpuApi("GL_ARB_separate_shader_objects")) {
            // this relaxes shader matching rules and requires slightly different GLSL declaration
            // however our usage pattern does not benefit from this and driver support is quite poor
            // so disable it for now (see below)
            /*mVendorFeature->add(GF_GLSL_SeparateShaderObject);*/
        }
        
        if (mIs43 || checkGpuApi("GL_KHR_debug"))
            mVendorFeature->add(GF_Debug);

        if(hasGL42 || mGLSupport->checkGpuApi("GL_ARB_base_instance"))
            mVendorFeature->add(CF_BaseInstance);
        if(isVersionNewer( 4, 6 ) || mGLSupport->checkGpuApi("GL_ARB_multi_draw_indirect"))
            mVendorFeature->add(CF_IndirectBuffer);
        if(isVersionNewer( 4, 4 ) || mGLSupport->checkGpuApi("GL_ARB_buffer_storage"))
            mVendorFeature->add(CF_PersistentMapping);

        mVendorFeature->add(GF_ClipRect);
        mVendorFeature->add(GF_ClipPlanes);
        mVendorFeature->add(GF_Infinite_FarPlane);

        mVendorFeature->add(GF_Stencil_Wrap);
        mVendorFeature->add(GF_HW_PixelQuery);
        
        mVendorFeature->add(GF_Vertex_UByte4);
        mVendorFeature->add(GF_Tex_NonPow2);
        mVendorFeature->add(GF_Tex_MipMapBias);

        mVendorFeature->add(GF_Colour_Blend);
        
        //Technically D3D10.1 hardware (GL3) supports gather and exposes this extension.
        //However we have bug reports that textureGather isn't working properly, and
        //sadly these cards no longer receive updates. So, assume modern cards and
        //modern drivers are needed.
        //https://bitbucket.org/sinbad/ogre/commits/c76e1bedfed65d0f9dc45353d432a26f278cc968#comment-1776416
        //if( mGLSupport->checkGpuApi("GL_ARB_texture_gather") || mHasGL40 )
        if( mIs43 )
            mVendorFeature->add(GF_Tex_Gather);

        // Check for Float textures
        if(GLEW_ATI_texture_float || GLEW_ARB_texture_float)
        {
            mVendorFeature->add(GF_Tex_Float);
        }

        // 3D textures should be supported by GL 1.2, which is our minimum version
        mVendorFeature->add(GF_Tex_1D);
        mVendorFeature->add(GF_Tex_3D);

        GLint buffers;
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &buffers);
        mVendorFeature->setMaxMultiFrameObj(N_MIN(NIIi, buffers, NII_MAX_RenderTarget));
        mVendorFeature->add(GF_MultiRenderTarget_EachDepthBit);
        
        GLint maxRes2d, maxRes3d, maxResCube;
        GLCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE,            &maxRes2d));
        GLCheck(glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,         &maxRes3d));
        GLCheck(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,   &maxResCube));

        mVendorFeature->setMaxTexSize(static_cast<Nui16>(maxRes2d), static_cast<Nui16>(maxRes3d),
                                    static_cast<Nui16>(maxResCube));

        // Check GLSupport for PBuffer support
        if(GLEW_ARB_pixel_buffer_object || GLEW_EXT_pixel_buffer_object)
        {
            mVendorFeature->add(GF_GL_PBuffer);
        }
        mVendorFeature->add(GF_Render_To_Texture);
        // Point size
        GLfloat psRange[2] = {0.0, 0.0};
        GLCheck(glGetFloatv(GL_POINT_SIZE_RANGE, psRange));
        mVendorFeature->setMaxPointSize(psRange[1]);
        
        GLfloat lineWidth[2] = { 1, 1 };
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidth);
        if (lineWidth[1] != 1 && lineWidth[1] != lineWidth[0])
            mVendorFeature->add(GF_LineWide);

        // Vertex texture fetching
        if (checkGpuApi("GL_ARB_vertex_shader"))
        {
            GLint vcnt;
            glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vcnt);
            mVendorFeature->setMaxVertexTexUnit((Nui16)vcnt);
            if (vcnt > 0)
            {
                mVendorFeature->add(GF_Program_VertexTexFetch);
            }
            mVertexTextureShared = true;
        }

        if (checkGpuApi("GL_ARB_multisample"))
        {
            // Alpha to coverage always 'supported' when MSAA is available
            // although card may ignore it if it doesn't specifically support A2C
            mVendorFeature->add(GF_Alpha_Coverage);
        }

        if( mIs43 || (mGLSupport->checkGpuApi("GL_ARB_shader_image_load_store") &&
            mGLSupport->checkGpuApi("GL_ARB_shader_storage_buffer_object")) )
        {
            mVendorFeature->add(CF_Program_StorgeBuffer);
        }

        if( mGLSupport->checkGpuApi( "GL_ARB_shader_viewport_layer_array" ) )
            mVendorFeature->add( CF_Program_ViwportArray );

        if( mVersion.major >= 4 || mGLSupport->checkGpuApi("GL_ARB_texture_cube_map_array") )
            mVendorFeature->add(CF_Tex_CubeArray);

        // FIXME: This isn't working right yet in some rarer cases
        /*if (mGLSupport->checkGpuApi("GL_ARB_separate_shader_objects") || hasGL41)
            mVendorFeature->add(CF_Program_SeparateObject);*/

        if(hasGL42 || mGLSupport->checkGpuApi("GL_ARB_shading_language_420pack"))
        {
            mVendorFeature->add(CF_Program_ConstBuffer);
        }
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::initImpl(ViewWindow * primary)
    {
        if(mFeature->getName() != getName())
        {
            N_EXCEPT(InvalidParam, _I18n("Trying to initialize GLRenderSystem from RenderFeature that do not support OpenGL"));
        }

        // set texture the number of texture units
        mFixedFunctionTextureUnits = mFeature->getMaxTexUnit();

        //In GL there can be less fixed function texture units than general
        //texture units. Get the minimum of the two.
        if(mFeature->isSupport(GF_Program_Fragment))
        {
            GLint maxTexCoords = 0;
            glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTexCoords);
            if(mFixedFunctionTextureUnits > maxTexCoords)
            {
                mFixedFunctionTextureUnits = maxTexCoords;
            }
        }

        mGpuProgramManager = N_new GLGpuProgramManager();

        mHardwareBufferManager = N_new GLGBufferManager;

        if(mFeature->isSupport(GF_Program_Vertex))
        {
            if(mFeature->isShaderSupport(SL_ARBVP1))
            {
                mGpuProgramManager->addCreateFunc(SL_ARBVP1, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_VP_30))
            {
                mGpuProgramManager->addCreateFunc(SL_VP_30, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_VP_40))
            {
                mGpuProgramManager->addCreateFunc(SL_VP_40, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_GP4_VP))
            {
                mGpuProgramManager->addCreateFunc(SL_GP4_VP, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_GPU_VP))
            {
                mGpuProgramManager->addCreateFunc(SL_GPU_VP, createGLArbGpuProgram);
            }
        }

        if(mFeature->isSupport(GF_Program_Geometry))
        {
            //TODO : Should these be createGLArbGpuProgram or createGLGpuNVparseProgram?
            if(mFeature->isShaderSupport(SL_NV_GP4))
            {
                mGpuProgramManager->addCreateFunc(SL_NV_GP4, createGLArbGpuProgram);
            }
            if(mFeature->isShaderSupport(SL_GP4_GP))
            {
                mGpuProgramManager->addCreateFunc(SL_GP4_GP, createGLArbGpuProgram);
            }
            if(mFeature->isShaderSupport(SL_GPU_GP))
            {
                mGpuProgramManager->addCreateFunc(SL_GPU_GP, createGLArbGpuProgram);
            }
        }

        if(mFeature->isSupport(GF_Program_Fragment))
        {
            if(mFeature->isShaderSupport(SL_FP_20))
            {
                mGpuProgramManager->addCreateFunc(SL_FP_20, createGLGpuNvparseProgram);
            }

            if(mFeature->isShaderSupport(SL_PS_1_4))
            {
                mGpuProgramManager->addCreateFunc(SL_PS_1_4, createGL_ATI_FS_GpuProgram);
            }

            if(mFeature->isShaderSupport(SL_PS_1_3))
            {
                mGpuProgramManager->addCreateFunc(SL_PS_1_3, createGL_ATI_FS_GpuProgram);
            }

            if(mFeature->isShaderSupport(SL_PS_1_2))
            {
                mGpuProgramManager->addCreateFunc(SL_PS_1_2, createGL_ATI_FS_GpuProgram);
            }

            if(mFeature->isShaderSupport(SL_PS_1_1))
            {
                mGpuProgramManager->addCreateFunc(SL_PS_1_1, createGL_ATI_FS_GpuProgram);
            }

            if(mFeature->isShaderSupport(SL_ARBFP1))
            {
                mGpuProgramManager->addCreateFunc(SL_ARBFP1, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_FP_40))
            {
                mGpuProgramManager->addCreateFunc(SL_FP_40, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_FP_30))
            {
                mGpuProgramManager->addCreateFunc(SL_FP_30, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_GP4_FP))
            {
                mGpuProgramManager->addCreateFunc(SL_GP4_FP, createGLArbGpuProgram);
            }

            if(mFeature->isShaderSupport(SL_GPU_FP))
            {
                mGpuProgramManager->addCreateFunc(SL_GPU_FP, createGLArbGpuProgram);
            }
        }

        if(mFeature->isShaderSupport(SL_GLSL))
        {
            // NFZ - check for GLSL vertex and fragment shader support successful
            mGLSLProgramFactory = N_new GLSLShaderFactory(*mGLSupport);
            N_Only(HighLevelGpuProgram).add(mGLSLProgramFactory);
            N_Only(Log).log(_I18n("GLSL support detected"));
        }

        // Do this after extension function pointers are initialised as the extension
        // is used to probe further capabilities.
        SysOptionList::const_iterator cfi = getConfig().find(N_PropertyWindow_TBOMode);
        // RTT Mode: 0 use whatever available, 1 use PBuffers, 2 force use copying
        NIIi rttMode = 0;
        if (cfi != getConfig().end())
        {
            if (cfi->second.mValue == _T("PBuffer"))
            {
                rttMode = 1;
            }
            else if (cfi->second.mValue == _T("Copy"))
            {
                rttMode = 2;
            }
        }

        // Check for framebuffer object extension
        if(mFeature->isSupport(GF_Render_To_Texture) && rttMode < 1)
        {
            // Before GL version 2.0, we need to get one of the extensions
            if (GLEW_ARB_draw_buffers)
                GLEW_GET_FUN(__glewDrawBuffers) = glDrawBuffersARB;
            else if (GLEW_ATI_draw_buffers)
                GLEW_GET_FUN(__glewDrawBuffers) = glDrawBuffersATI;

            // Create FBO manager
            N_Only(Log).log(_I18n("GL: Using GL_EXT_framebuffer_object for rendering to textures (best)"));
            mRTTManager = N_new GLFBOManager(this);
            mFeature->add(GF_RenderTarget_LessDepth);
        }
        else if(mFeature->isSupport(GF_GL_PBuffer) && rttMode < 2) // Check GLSupport for PBuffer support
        {
            if(mFeature->isSupport(GF_Render_To_Texture))
            {
                // Use PBuffers
                mRTTManager = N_new GLPBRTTManager(mGLSupport, primary);
                N_Only(Log).log(_I18n("GL: Using PBuffers for rendering to textures"));
                //TODO: Depth buffer sharing in pbuffer is left unsupported
            }
                
            // Downgrade number of simultaneous targets
            mFeature->setMaxMultiFrameObj(1);
        }
        else
        {
            // No pbuffer support either -- fallback to simplest copying from framebuffer
            mRTTManager = N_new GLCopyingRTTManager();
            N_Only(Log).log(_I18n("GL: Using framebuffer copy for rendering to textures (worst)"));
            N_Only(Log).log(_I18n("GL: Warning: TextureFrame size is restricted to size of framebuffer. If you are on Linux, consider using GLX instead of SDL."));

            //Copy method uses the main depth buffer but no other depth buffer
            mFeature->add(GF_RenderTarget_AttachMainDepth);
            mFeature->add(GF_RenderTarget_LessDepth);
            // Downgrade number of simultaneous targets
            mFeature->setMaxMultiFrameObj(1);
        }
		
		mTextureManager = N_new GLTextureManager(this);

        Log * defaultLog = LogManager::getOnly().get();
        if(defaultLog)
        {
            mFeature->log(defaultLog);
            defaultLog->log(_T(" * Using Reverse Z: ") + N_conv(mReverseDepth));
        }

        mGLInitialised = true;
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::shutdown(void)
    {
        RenderSys::shutdown();

        // 删除GLSL着色程序工厂类
        if (mGLSLProgramFactory)
        {
            // Remove from manager safely
            if (HighLevelGpuProgramManager::getOnlyPtr())
                HighLevelGpuProgramManager::getOnly().remove(mGLSLProgramFactory);
			N_delete mGLSLProgramFactory;
            mGLSLProgramFactory = 0;
        }

        // Deleting the GPU program manager and hardware buffer manager.  Has to be done before the mGLSupport->stop().
		N_delete mGpuProgramManager;
        mGpuProgramManager = 0;

		N_delete mHardwareBufferManager;
        mHardwareBufferManager = 0;

		N_delete mRTTManager;
        mRTTManager = 0;

        // Delete extra threads contexts
        GLContextList::iterator i, iend = mBackgroundContextList.end();
        for (i = mBackgroundContextList.begin();i != iend;++i)
        {
            GLContext * ctx = *i;

            ctx->releaseContext();

			N_delete ctx;
        }
        mBackgroundContextList.clear();
        
        if( mNullColourFramebuffer )
        {
            GLCheck( glDeleteFramebuffers( 1, &mNullColourFramebuffer ) );
            mNullColourFramebuffer = 0;
        }

        mGLSupport->stop();
        mStopRendering = true;

		N_delete mTextureManager;
        mTextureManager = 0;
        
        if( mGlobalVao )
        {
            GLCheck( glBindVertexArray( 0 ) );
            GLCheck( glDeleteVertexArrays( 1, &mGlobalVao ) );
            mGlobalVao = 0;
        }

        // There will be a new initial window and so forth, thus any call to test
        //  some params will access an invalid pointer, so it is best to reset
        //  the whole state.
        mGLInitialised = false;
    }
    //--------------------------------------------------------------------
    void GLRenderSystem::setAmbient(const Colour & c)
    {
        GLfloat lmodel_ambient[] = {c.r, c.g, c.b, c.a};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    }
    //--------------------------------------------------------------------
    void GLRenderSystem::setShadingType(ShadeMode so)
    {
        switch(so)
        {
        case SM_FLAT:
            mState->setShadeModel(GL_FLAT);
            break;
        default:
            mState->setShadeModel(GL_SMOOTH);
            break;
        }
    }
    //---------------------------------------------------------------------
    bool GLRenderSystem::createWindow(const WindowDefineList & defList, WindowList & out)
    {
        if(false == RenderSys::createWindow(defList, out))
            return false;

        // Simply call createWindow in a loop.
        for(NCount i = 0; i < defList.size(); ++i)
        {
            const WindowDefine & winDef = defList[i];

            ViewWindow * cWin = createWindow(winDef.mName, winDef.mWidth, winDef.mHeight,
                winDef.mFull, &winDef.mCustom);

            out.push_back(cWin);
        }

        return true;
    }
    //---------------------------------------------------------------------
    ViewWindow * GLRenderSystem::createWindow(const String & name, NCount width,
        NCount height, bool fullScreen, const PropertyData * extparam)
    {
        StringStream ss;
        ss << "GLRenderSystem::createWindow \"" << name << "\", " << width << "x" << height << " ";
        if(fullScreen)
            ss << _I18n("fullscreen ");
        else
            ss << _I18n("windowed ");

        if(extparam)
        {
            ss << _I18n(" ExtParam: ");
			PropertyData::PropertyList::const_iterator it, itend = extparam->getList().end();
            for(it = extparam->getList().begin(); it != itend; ++it)
            {
                ss << it->first << "=" << it->second << " ";
            }
            LogManager::getOnly().log(ss.str());
        }

        ViewWindow * win = mGLSupport->newWindow(name, width, height, fullScreen, extparam);

        mWindows.insert( win );
        win->_notifyCreate(this);

        if(!mGLInitialised)
        {
            mWinProperty = *extparam; 
            mWinProperty->get(N_PropertyWindow_ReverseDepth, mReverseDepth, true);
            
            initialiseContext(win);
			
			if (isVersionOlder(3, 3))
                N_EXCEPT(RenderingAPI, _I18n("Driver does not support at least OpenGL 3.0."));

            bool emulateTexBuffers = !(mIs43 || mGLSupport->checkGpuApi("GL_ARB_texture_buffer_range"));
            mBufferManager = N_new GL3PlusVaoManager(emulateTexBuffers);
                           
            //Bind the Draw ID
            GLCheck( glGenVertexArrays( 1, &mGlobalVao ) );
            GLCheck( glBindVertexArray( mGlobalVao ) );
            static_cast<GL3PlusVaoManager*>( mBufferManager )->bindDrawId();
            GLCheck( glBindVertexArray( 0 ) );
            
            const char* shadingLangVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
            VStringList tokens;
            StrUtil::split(shadingLangVersion, tokens, ". ");
            Nui32 temp1, temp2;
            StrConv::conv(tokens[0], temp1);
            StrConv::conv(tokens[1], temp2);
            mShaderVersion = (temp1 * 100) + temp2;

            mHasArbInvalidateSubdata = mIs43 || mGLSupport->checkGpuApi( "GL_ARB_invalidate_subdata" );
            
            setupFeature();
            setupFFPParam();

            // use real capabilities if custom capabilities are not available
            if(mSysFeatureValid)
                mFeature = mVendorFeature;

            //On AMD's GCN cards, there is no performance or memory difference between
            //PF_D24_UNORM_S8_UINT & PF_D32_FLOAT_X24_S8_UINT, so prefer the latter
            //on modern cards (GL >= 4.3) and that also claim to support this format.
            //NVIDIA's preference? Dunno, they don't tell. But at least the quality
            //will be consistent.
            if( isVersionNewer( 4, 0 ) )
                Texture::DepthFormat = PF_Depth32f_Stencil8ui_64;
            else
                Texture::DepthFormat = PF_Depth24UN_Stencil8ui;
            
            mTextureManager = OGRE_NEW GL3PlusTextureGpuManager( mBufferManager, this, *mGLSupport );

            fireEvent("RenderSystemCapabilitiesCreated");
            initImpl(win);

            // 初始化主设备器
            _oneTimeContextInitialization();
            if(mCurrentContext)
                mCurrentContext->setInitialized();
            
            mTextureManager->_update( true );
        }

        win->_init();
        win->_notifyInit(this);
        return win;
    }
    //-----------------------------------------------------------------------
    /*ExtFrameBuffer * GLRenderSystem::createExtFrameImpl(FrameObj * renderTarget)
    {
        GLDepthBuffer * retVal = 0;

        //Only FBO & pbuffer support different depth buffers, so everything
        //else creates dummy (empty) containers
        GLFrameBufferObjectBase * fbo = dynamic_cast<GLRenderTarget*>(renderTarget)->getFBO();

        if(fbo)
        {
            //Presence of an FBO means the manager is an FBO Manager, that's why it's safe to downcast
            //Find best depth & stencil format suited for the RT's format
            GLuint depthFormat, stencilFormat;
            static_cast<GLFBOManager*>(mRTTManager)->getBestDepthStencil(fbo->getFormat(), &depthFormat, &stencilFormat);

            GLRenderBuffer * depthbuf = N_new GLRenderBuffer(depthFormat, fbo->getWidth(), fbo->getHeight(), fbo->getFSAA());

            GLRenderBuffer * stencilbuf = 0;
            if (depthFormat == GL_DEPTH24_STENCIL8 || depthFormat == GL_DEPTH32F_STENCIL8 )
            {
                // If we have a packed format, the stencilBuffer is the same as the depthBuffer
                stencilbuf = depthbuf;
            }
            else if(stencilFormat)
            {
                stencilbuf = N_new GLRenderBuffer(stencilFormat, fbo->getWidth(), fbo->getHeight(), fbo->getFSAA());
            }

            //No "custom-quality" multisample for now in GL
            retVal = new GLDepthBuffer(0, this, mCurrentContext, depthbuf, stencilbuf,
                fbo->getWidth(), fbo->getHeight(), fbo->getFSAA(), false);
        }
        else
        {
            retVal = new GLDepthBuffer(0, this, mCurrentContext, NULL, NULL, renderTarget->getWidth(),
                renderTarget->getHeight(), renderTarget->getFSAA(), false);
        }
        return retVal;
    }*/
    //-----------------------------------------------------------------------
    void GLRenderSystem::initialiseContext(ViewWindow * primary)
    {
        // Set main and current context
        mMainContext = 0;
        primary->getPlatformData( "GLCONTEXT", &mMainContext );
        mCurrentContext = mMainContext;

        // Set primary context as active
        if (mCurrentContext)
            mCurrentContext->setCurrent();
		
		// Initialise GL3W
        /*if (gl3wInit2(get_proc)) { // gl3wInit() fails if GL3.0 is not supported
            N_EXCEPT(RenderingAPI, _I18n("OpenGL 3.0 is not supported"));
        }*/

		bool gl3wFailed = glewInit() != 0;
        if( gl3wFailed )
        {
            N_error("Failed to initialize GL3W");
        }
        else
        {
            initialiseExtensions();
        }

        // Make sure that OpenGL 3.3+ is supported in this context
        if( gl3wFailed || !isVersionNewer(3, 3))
        {
            N_EXCEPT(RenderingAPI, _I18n("OpenGL 3.3 is not supported"));
        }

        mIs43 = isVersionNewer(4, 3);

        mState = mCurrentContext->getState<GLStateCacheManager>();
		
        LogManager::getOnly().log(_T("***************************"));
        LogManager::getOnly().log(_T("*** GL4 Started ***********"));
        LogManager::getOnly().log(_T("***************************"));
    }
    //-----------------------------------------------------------------------
    /*MultiTextureFrame * GLRenderSystem::createMultiFrame(const String & name)
    {
        auto fboMgr = dynamic_cast<GLFBOManager*>(mRTTManager);
        if (!fboMgr)
            return 0;

        MultiTextureFrame *re = new GLFBOMultiRenderTarget(fboMgr, name);
        add(re);
        return re;
    }*/
    //-----------------------------------------------------------------------
    /*void GLRenderSystem::destroyFrame(const FrameObj * pWin)
    {
        // Find it to remove from list
        FrameList::iterator i = mFrameObjList.begin();
        while(i != mFrameObjList.end())
        {
            if(i->second == pWin)
            {
                const GLContext * windowContext = dynamic_cast<const GLRenderTarget*>(pWin)->getContext();

                //1 Window <-> 1 Context, should be always true
                N_assert(windowContext);

                bool bFound = false;
                //Find the depth buffer from this window and remove it.
                ExtFrameList::iterator itMap = mExtFrameList.begin();
                ExtFrameList::iterator enMap = mExtFrameList.end();

                while(itMap != enMap && !bFound)
                {
                    ExtFrameBufferList::iterator itor = itMap->second.begin();
                    ExtFrameBufferList::iterator end  = itMap->second.end();

                    while(itor != end)
                    {
                        //A ExtFrameBuffer with no depth & stencil pointers is a dummy one,
                        //look for the one that matches the same GL context
                        GLDepthBuffer * depthBuffer = static_cast<GLDepthBuffer*>(*itor);
                        GLContext * glContext = depthBuffer->getGLContext();

                        if(glContext == windowContext &&
                            (depthBuffer->getDepthBuffer() || depthBuffer->getStencilBuffer()))
                        {
                            bFound = true;

							N_delete *itor;
                            itMap->second.erase(itor);
                            break;
                        }
                        ++itor;
                    }

                    ++itMap;
                }
                mFrameObjList.erase(i);
				N_delete pWin;
                break;
            }
        }
    }*/
    //---------------------------------------------------------------------
    void GLRenderSystem::_setLight(NCount limit)
    {
        if (limit == mCurrentLights)
            return;

        unsigned short num = 0;
        for (; num < limit; ++num)
        {
            setGLLight(num, true);
        }
        // Disable extra lights
        for (; num < mCurrentLights; ++num)
        {
            setGLLight(num, false);
        }
        mCurrentLights = limit;
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::setGLLight(NCount index, bool set)
    {
        setFFPLightParams(index, set);

        GLenum gl_index = GL_LIGHT0 + index;

        if (set)
        {
            GLfloat f4vals[4] = { 0, 0, 0, 1 };
            // Disable ambient light for movables;
            glLightfv(gl_index, GL_AMBIENT, f4vals);

            mState->setEnabled(gl_index);
        }
        else
        {
            mState->setDisabled(gl_index);
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::makeGLMatrix(GLfloat gl_matrix[16], const Matrix4<NIIf> & m)
	{
        gl_matrix[0]  = m[0][0];gl_matrix[1]  = m[1][0];gl_matrix[2]  = m[2][0];gl_matrix[3]  = m[3][0];
        gl_matrix[4]  = m[0][1];gl_matrix[5]  = m[1][1];gl_matrix[6]  = m[2][1];gl_matrix[7]  = m[3][1];
        gl_matrix[8]  = m[0][2];gl_matrix[9]  = m[1][2];gl_matrix[10] = m[2][2];gl_matrix[11] = m[3][2];
        gl_matrix[12] = m[0][3];gl_matrix[13] = m[1][3];gl_matrix[14] = m[2][3];gl_matrix[15] = m[3][3];
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setModel(const Matrix4f & m)
    {
        GLfloat mat[16];
        mWorldMatrix = m;
        makeGLMatrix(mat, mViewMatrix * mWorldMatrix);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(mat);
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setView(const Matrix4f & m)
    {
        mViewMatrix = m;

        GLfloat mat[16];
        makeGLMatrix( mat, mViewMatrix * mWorldMatrix );
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(mat);

        if (mClipPlanes.size())
            mClipValid = false;
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setProj(const Matrix4f &m)
    {
        GLfloat mat[16];
        makeGLMatrix(mat, m);
        if (mActiveFrame->isFlipping())
        {
            // Invert transformed y
            mat[1]  = -mat[1];
            mat[5]  = -mat[5];
            mat[9]  = -mat[9];
            mat[13] = -mat[13];
        }
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(mat);
        glMatrixMode(GL_MODELVIEW);

        // also mark clip planes dirty
        if(mClipPlanes.size())
            mClipValid = false;
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setSurfaceTracking(Nmark tracking)
    {

        // Track vertex colour
        if (tracking != CTM_T_NONE)
        {
            GLenum gt = GL_DIFFUSE;
            // There are actually 15 different combinations for tracking, of which
            // GL only supports the most used 5. This means that we have to do some
            // magic to find the best match. NOTE:
            //  GL_AMBIENT_AND_DIFFUSE != GL_AMBIENT | GL__DIFFUSE
            if (tracking & CTM_T_AMBIENT)
            {
                if (tracking & CTM_T_DIFFUSE)
                {
                    gt = GL_AMBIENT_AND_DIFFUSE;
                }
                else
                {
                    gt = GL_AMBIENT;
                }
            }
            else if (tracking & CTM_T_DIFFUSE)
            {
                gt = GL_DIFFUSE;
            }
            else if (tracking & CTM_T_SPECULAR)
            {
                gt = GL_SPECULAR;
            }
            else if (tracking & CTM_T_EMISSIVE)
            {
                gt = GL_EMISSION;
            }
            glColorMaterial(GL_FRONT_AND_BACK, gt);

            mState->setEnabled(GL_COLOR_MATERIAL);
        }
        else
        {
            mState->setDisabled(GL_COLOR_MATERIAL);
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::setColour(const ShaderChColour & c, Nmark applymark)
    {
        Nmark mark = c.getColourTracking();

        // Track vertex colour
        if(mark != CTM_T_NONE)
        {
            GLenum gt = GL_DIFFUSE;
            // There are actually 15 different combinations for tracking, of which
            // GL only supports the most used 5. This means that we have to do some
            // magic to find the best match. NOTE:
            //  GL_AMBIENT_AND_DIFFUSE != GL_AMBIENT | GL_DIFFUSE
            if(mark & CTM_T_AMBIENT)
            {
                if(mark & CTM_T_DIFFUSE)
                {
                    gt = GL_AMBIENT_AND_DIFFUSE;
                }
                else
                {
                    gt = GL_AMBIENT;
                }
            }
            else if(mark & CTM_T_DIFFUSE)
            {
                gt = GL_DIFFUSE;
            }
            else if(mark & CTM_T_SPECULAR)
            {
                gt = GL_SPECULAR;
            }
            else if(mark & CTM_T_EMISSIVE)
            {
                gt = GL_EMISSION;
            }
            glColorMaterial(GL_FRONT_AND_BACK, gt);

            mState->setEnabled(GL_COLOR_MATERIAL);
        }
        else
        {
            mState->setDisabled(GL_COLOR_MATERIAL);
        }

        // XXX Cache previous values?
        // XXX Front or Front and Back?

        //GLfloat f4val[4];
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c.mDiffuse.ptr());
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c.mAmbient.ptr());
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c.mSpecular.ptr());
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c.mEmissive.ptr());
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, c.mShininess);
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::setPoint(const ShaderChPoint & p, Nmark applymark)
    {
        GLfloat val[4] = {1, 0, 0, 1};

        NIIf size = p.mSize;
        NIIf minSize = p.mMinSize;
        NIIf maxSize = p.mMaxSize;

        if(p.isAttenuateEnable())
        {
            // Point size is still calculated in pixels even when attenuation is
            // enabled, which is pretty awkward, since you typically want a viewport
            // independent size if you're looking for attenuation.
            // So, scale the point size up by viewport size (this is equivalent to
            // what D3D does as standard)
            size = size * mActiveViewports[0]->getPixelHeight();
            minSize = minSize * mActiveViewports[0]->getPixelHeight();
            if (maxSize == 0.0f)
                maxSize = mFeature->getMaxPointSize(); // pixels
            else
                maxSize = maxSize * mActiveViewports[0]->getPixelHeight();

            // XXX: why do I need this for results to be consistent with D3D?
            // Equations are supposedly the same once you factor in vp height
            NIIf correction = 0.005f;
            // scaling required
            val[0] = p.mConstant;
            val[1] = p.mLinear * correction;
            val[2] = p.mQuadratic * correction;
            val[3] = 1;

            if (mFeature->isSupport(GF_Program_Vertex))
                mState->setEnabled(GL_VERTEX_PROGRAM_POINT_SIZE);
        }
        else
        {
            if (maxSize == 0.0f)
                maxSize = mFeature->getMaxPointSize();
            if (mFeature->isSupport(GF_Program_Vertex))
                mState->setDisabled(GL_VERTEX_PROGRAM_POINT_SIZE);
        }

        mState->setPointSize(size);
        mState->setPointParameters(val, minSize, maxSize);

        if(mFeature->isSupport(GF_Point_Sprites))
        {
            if(p.isSpritesEnable())
            {
                mState->setEnabled(GL_POINT_SPRITE);
                // Set sprite texture coord generation
                // Don't offer this as an option since D3D links it to sprite enabled
                for(Nui16 i = 0; i < mFixedFunctionTextureUnits; ++i)
                {
                    mState->activateGLTextureUnit(i);
                    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
                }
            }
            else
            {
                mState->setDisabled(GL_POINT_SPRITE);
                // Set sprite texture coord generation
                // Don't offer this as an option since D3D links it to sprite enabled
                for(Nui16 i = 0; i < mFixedFunctionTextureUnits; ++i)
                {
                    mState->activateGLTextureUnit(i);
                    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);
                }
            }
        }

        if(mFeature->isSupport(GF_LineWide))
            glLineWidth(p.getLineWidth());
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_bindTexture(Nidx stage, const Texture * otex, bool enabled)
    {
        if (!mState->activateGLTextureUnit(stage))
            return;
		
        GLenum lastType = mTextureTypes[stage];

        if(enabled)
        {
            const GLTexture * tex = static_cast<const GLTexture *>(otex);
            if(tex)
            {
                const_cast<GLTexture *>(tex)->ref();
                mTextureTypes[stage] = tex->getGLTextureTarget();
            }

            if(stage < mFixedFunctionTextureUnits)
            {
                if(mTextureTypes[stage] != GL_TEXTURE_2D_ARRAY_EXT)
                    glEnable(mTextureTypes[stage]);
            }

            if(tex)
                mState->bindGLTexture(mTextureTypes[stage], tex->getDisplayTextureName());
			else
				mState->bindGLTexture(lastType, 0);
        }
        else
        {
            mState->bindGLTexture(lastType, 0);
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setTextureSample(Nidx unit, const TextureSampleUnit & sampler)
    {
        if (!mState->activateGLTextureUnit(unit))
            return;

        /*GLenum target = mTextureTypes[unit];

        const TextureAddressing & uvw = sampler.getMode();
        mState->setTexParameteri(target, GL_TEXTURE_WRAP_S, getTextureAddressingMode(uvw.mS_U));
        mState->setTexParameteri(target, GL_TEXTURE_WRAP_T, getTextureAddressingMode(uvw.mT_V));
        mState->setTexParameteri(target, GL_TEXTURE_WRAP_R, getTextureAddressingMode(uvw.mR_W));

        if (uvw.mS_U == TAM_BORDER || uvw.mT_V == TAM_BORDER || uvw.mR_W == TAM_BORDER)
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, sampler.getBorder().ptr());

        mState->glTexParameterf(target, GL_TEXTURE_LOD_BIAS sampler.getMipmapBias());

        if (mFeature->isSupport(GF_Tex_Anisotropy))
            mState->setTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min<Nui32>(mFeature->getMaxAnisotropy(), sampler.getAnisotropy()));

        mState->glTexParameterf(target, GL_TEXTURE_MIN_LOD, sampler.mMinLod );
        mState->glTexParameterf(target, GL_TEXTURE_MAX_LOD, sampler.mMaxLod );
                                             
        mState->setTexParameteri(target, GL_TEXTURE_COMPARE_MODE, sampler.getCompareFunc() ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
        if (!sampler.getCompareFunc())
            mState->setTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, convertCompareFunction(sampler.getCompareFunc()));

        // Combine with existing mip filter
        mState->setTexParameteri(target, GL_TEXTURE_MIN_FILTER, getCombinedMinMipFilter(sampler.getFiltering(TFT_MIN), sampler.getFiltering(TFT_MIP)));

        switch (sampler.getFiltering(TFT_MAG))
        {
        case TFOP_ANISOTROPIC: // GL treats linear and aniso the same
        case TFOP_LINEAR:
            mState->setTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case TFOP_POINT:
        case TFOP_NONE:
            mState->setTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        }*/
        
        assert( (!sampler || sampler->mRsData) && "The block must have been created via RenderSys::getSamplerblock!" );

        if( !sampler )
        {
            glBindSampler( unit, 0 );
        }
        else
        {
            glBindSampler( unit, static_cast<GLuint>(reinterpret_cast<intptr_t>( sampler->mRsData ) ) );
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setTextureCoordSet(Nidx stage, Nidx index)
    {
        mTextureCoordIndex[stage] = index;
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setTextureCoordPrc(Nidx stage, TextureCoordPrc m, const Frustum * frustum)
    {
        if(stage >= mFixedFunctionTextureUnits)
        {
            // Can't do this
            return;
        }

        GLfloat M[16];
        Matrix4f projectionBias;

        // Default to no extra auto texture matrix
        mUseAutoTextureMatrix = false;

        GLfloat eyePlaneS[] = {1.0, 0.0, 0.0, 0.0};
        GLfloat eyePlaneT[] = {0.0, 1.0, 0.0, 0.0};
        GLfloat eyePlaneR[] = {0.0, 0.0, 1.0, 0.0};
        GLfloat eyePlaneQ[] = {0.0, 0.0, 0.0, 1.0};

        if (!mState->activateGLTextureUnit(stage))
            return;

        switch(m)
        {
        case TCP_NONE:
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_S);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_T);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_R);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_Q);
            break;

        case TCP_ENV_SPHERE:
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

            mState->enableTextureCoordGen(GL_TEXTURE_GEN_S);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_T);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_R);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_Q);

            // Need to use a texture matrix to flip the spheremap
            mUseAutoTextureMatrix = true;
            memset(mAutoTextureMatrix, 0, sizeof(GLfloat) * 16);
            mAutoTextureMatrix[0] = mAutoTextureMatrix[10] = mAutoTextureMatrix[15] = 1.0f;
            mAutoTextureMatrix[5] = -1.0f;

            break;
        case TCP_ENV_NORMAL:
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);

            mState->enableTextureCoordGen(GL_TEXTURE_GEN_S);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_T);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_R);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_Q);
            break;
        case TCP_ENV_REFLECTION:
            // XXX This doesn't seem right?!
#ifdef GL_VERSION_1_3
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

            mState->enableTextureCoordGen(GL_TEXTURE_GEN_S);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_T);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_R);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_Q);
#else
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

            mState->enableTextureCoordGen(GL_TEXTURE_GEN_S);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_T);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_R);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_Q);
#endif
            break;
        case TCP_ENV_REFLECTION_INV:

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

            mState->enableTextureCoordGen(GL_TEXTURE_GEN_S);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_T);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_R);
            mState->disableTextureCoordGen(GL_TEXTURE_GEN_Q);

            // We need an extra texture matrix here
            // This sets the texture matrix to be the inverse of the view matrix
            mUseAutoTextureMatrix = true;
            makeGLMatrix(M, mViewMatrix);

            // Transpose 3x3 in order to invert matrix (rotation)
            // Note that we need to invert the Z _before_ the rotation
            // No idea why we have to invert the Z at all, but reflection is wrong without it
            mAutoTextureMatrix[0] = M[0]; mAutoTextureMatrix[1] = M[4]; mAutoTextureMatrix[2] = -M[8];
            mAutoTextureMatrix[4] = M[1]; mAutoTextureMatrix[5] = M[5]; mAutoTextureMatrix[6] = -M[9];
            mAutoTextureMatrix[8] = M[2]; mAutoTextureMatrix[9] = M[6]; mAutoTextureMatrix[10] = -M[10];
            mAutoTextureMatrix[3] = mAutoTextureMatrix[7] = mAutoTextureMatrix[11] = 0.0f;
            mAutoTextureMatrix[12] = mAutoTextureMatrix[13] = mAutoTextureMatrix[14] = 0.0f;
            mAutoTextureMatrix[15] = 1.0f;

            break;
        case TCP_ENV_PROJECTIVE:
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS);
            glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT);
            glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR);
            glTexGenfv(GL_Q, GL_EYE_PLANE, eyePlaneQ);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_S);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_T);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_R);
            mState->enableTextureCoordGen(GL_TEXTURE_GEN_Q);

            mUseAutoTextureMatrix = true;

            // Set scale and translation matrix for projective textures
            projectionBias = RenderSys::ProjClipSpace2DToImageSpace;

            projectionBias = projectionBias * frustum->getProjMatrix();
            if(mTexProjRelative)
            {   //使用相对纹理投影矩阵产生的视图矩阵
                Matrix4f tmp4 = Matrix4f::IDENTITY;
                tmp4.setTrans(mTextureProjRel);
                Matrix4f viewMatrix = frustum->getViewMatrix() * tmp4;

                projectionBias = projectionBias * viewMatrix;
            }
            else
            {   //使用普通的视图矩阵
                projectionBias = projectionBias * frustum->getViewMatrix();
            }
            // 获取最终形成的纹理矩阵
            projectionBias = projectionBias * mWorldMatrix;

            makeGLMatrix(mAutoTextureMatrix, projectionBias);
            break;
        default:
            break;
        }
    }
    //-----------------------------------------------------------------------------
    GLint GLRenderSystem::getTextureAddressingMode(TextureAddressingMode tam) const
    {
        switch(tam)
        {
        default:
        case TAM_REPEAT:
            return GL_REPEAT;
        case TAM_MIRROR:
            return GL_MIRRORED_REPEAT;
        case TAM_CLAMP:
            return GL_CLAMP_TO_EDGE;
        case TAM_BORDER:
            return GL_CLAMP_TO_BORDER;
        }
        return GL_REPEAT;
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setTextureMatrix(Nidx stage, const Matrix4f & xform)
    {
        if (stage >= mFixedFunctionTextureUnits)
        {
            // Can't do this
            return;
        }

        if (!mState->activateGLTextureUnit(stage))
            return;
        glMatrixMode(GL_TEXTURE);
        glLoadMatrixf(xform.T()[0]);

        if (mUseAutoTextureMatrix)
        {
            glMultMatrixf(mAutoTextureMatrix);
        }

        glMatrixMode(GL_MODELVIEW);
    }
    //-----------------------------------------------------------------------------
    GLint GLRenderSystem::getBlendMode(ColourFactor Blend) const
    {
        switch(Blend)
        {
        case CF_ONE:
            return GL_ONE;
        case CF_ZERO:
            return GL_ZERO;
        case CF_DST:
            return GL_DST_COLOR;
        case CF_SRC:
            return GL_SRC_COLOR;
        case CF_INV_DST:
            return GL_ONE_MINUS_DST_COLOR;
        case CF_INV_SRC:
            return GL_ONE_MINUS_SRC_COLOR;
        case CF_DST_ALPHA:
            return GL_DST_ALPHA;
        case CF_SRC_ALPHA:
            return GL_SRC_ALPHA;
        case CF_INV_DST_ALPHA:
            return GL_ONE_MINUS_DST_ALPHA;
        case CF_INV_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        };
        return GL_ONE;
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::setBlend(const ShaderChBlend & b, Nmark applymark)
    {
        if(b.isPartAlpha())
        {
            if(b.isPartReplace())
            {
                mState->setDisabled(GL_BLEND);
                return;
            }
            GLint func = GL_FUNC_ADD, afunc = GL_FUNC_ADD;
            GLint src = getBlendMode(b.mSrcFactor);
            GLint dst = getBlendMode(b.mDstFactor);
            GLint asrc = getBlendMode(b.mSrcAlphaFactor);
            GLint adst = getBlendMode(b.mDstAlphaFactor);
            mState->setEnabled(GL_BLEND);
            mState->setBlendFunc(src, dst, asrc, adst);

            if (b.getBlendMode() < CBM_COUNT)
            {
                func = __CBM_GL_FUNC[b.getBlendMode()];
            }
            if(b.isPartAlphaBlendMode())
            {
                if(b.getAlphaBlendMode() < CBM_COUNT)
                    afunc = __CBM_GL_FUNC[b.getAlphaBlendMode()];
            }
			else
			{
				afunc = func;
			}

            mState->setBlendEquation(func, afunc);
        }
        else
        {
            if(b.isPartAlphaBlendMode())
            {
                if(b.isPartReplace())
                {
                    mState->setDisabled(GL_BLEND);
                    return;
                }
                GLint func = GL_FUNC_ADD, afunc = GL_FUNC_ADD;
                GLint src = getBlendMode(b.mSrcFactor);
                GLint dst = getBlendMode(b.mDstFactor);
                mState->setEnabled(GL_BLEND);
                mState->setBlendFunc(src, dst, src, dst);

                if(b.getBlendMode() < CBM_COUNT)
                    func = __CBM_GL_FUNC[b.getBlendMode()];

                if(b.getAlphaBlendMode() < CBM_COUNT)
                    afunc = __CBM_GL_FUNC[b.getAlphaBlendMode()];

                mState->setBlendEquation(func, afunc);
            }
            else
            {
                if(b.isReplace())
                {
                    mState->setDisabled(GL_BLEND);
                    return;
                }
                GLint func = GL_FUNC_ADD;
                GLint src = getBlendMode(b.mSrcFactor);
                GLint dst = getBlendMode(b.mDstFactor);
                mState->setEnabled(GL_BLEND);
                mState->setBlendFunc(src, dst, src, dst);

                if(b.getBlendMode() < CBM_COUNT)
                    func = __CBM_GL_FUNC[b.getBlendMode()];

                mState->setBlendEquation(func, func);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::setColour(const ShaderChColour & a, Nmark applymark)
    {
        bool a2c = false;
        CmpMode func = a.getAlphaReject();

        if(func == CPM_ALWAYS_PASS)
        {
            mState->setDisabled(GL_ALPHA_TEST);
        }
        else
        {
            mState->setEnabled(GL_ALPHA_TEST);
            a2c = a.isAlphaCoverage();
            glAlphaFunc(convertCompareFunction(func), a.getValue());
        }

        if(getFeature()->isSupport(GF_Alpha_Coverage))
        {
            if (a2c)
                mState->setEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
            else
                mState->setDisabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setViewport(Viewport * vp)
    {
        if(!vp)
        {
            //mActiveViewports[0] = NULL;
            _setFrame(NULL);
        }
        else if(vp != mActiveViewports[0] || !vp->isSyncRender())
        {
            FrameObj * target = vp->getBuffer();
            _setFrame(target);
            mActiveViewports[0] = vp;

            GLsizei x, y, w, h;

            // Calculate the "lower-left" corner of the viewport
            w = vp->getPixelWidth();
            h = vp->getPixelHeight();
            x = vp->getPixelLeft();
            y = vp->getPixelTop();
            if(target && !target->isFlipping())
            {   // 转换 "左上角" 到 "左下角"
                y = target->getHeight() - h - y;
            }
            mState->setViewport(x, y, w, h);
            // 配置视口裁剪
            glScissor(x, y, w, h);
            mScissorBox[0] = x;
            mScissorBox[1] = y;
            mScissorBox[2] = w;
            mScissorBox[3] = h;

            vp->syncRenderImpl();
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_beginFrame()
    {
        if(!mActiveViewports[0])
            N_EXCEPT(InvalidState, _I18n("Cannot begin frame - no viewport selected."));

        // 启动视口裁剪
        mScissorsEnabled = true;
        mState->setEnabled(GL_SCISSOR_TEST);
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_endFrame()
    {
        // 禁用视口裁剪
        mScissorsEnabled = false;
        mState->setDisabled(GL_SCISSOR_TEST);
        mState->setDisabled(GL_DEPTH_CLAMP);

        // unbind PSO programs at end of frame
        // this is mostly to avoid holding bound programs that might get deleted
        // outside via the resource manager
        setRSO( 0 );

        mState->getActiveProgram()->inactivate();
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setCullingMode(CullingMode mode)
    {
        mCullingMode = mode;
        // NB: Because two-sided stencil API dependence of the front face, we must
        // use the same 'winding' for the front face everywhere. As the NII default
        // culling mode is clockwise, we also treat anticlockwise winding as front
        // face for consistently. On the assumption that, we can't change the front
        // face by glFrontFace anywhere.

        GLenum cullMode;

        switch(mode)
        {
        case CM_None:
            mState->setDisabled(GL_CULL_FACE);
            return;
        default:
        case CM_Clockwise:
            if(mActiveFrame &&
                ((mActiveFrame->isFlipping() && !mInvertVertexWinding) ||
                (!mActiveFrame->isFlipping() && mInvertVertexWinding)))
            {
                cullMode = GL_FRONT;
            }
            else
            {
                cullMode = GL_BACK;
            }
            break;
        case CM_Anticlockwise:
            if(mActiveFrame &&
                ((mActiveFrame->isFlipping() && !mInvertVertexWinding) ||
                (!mActiveFrame->isFlipping() && mInvertVertexWinding)))
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
    //-----------------------------------------------------------------------------
    void GLRenderSystem::setStencil(const ShaderChStencil & b, Nmark applymark)
    {
        if(b.isDepthEnable())
        {
            mState->setClearDepth(1.0f);
            mState->setEnabled(GL_DEPTH_TEST);
            
            mDepthWrite = b.isDepthWrite();

            mState->setDepthMask(mDepthWrite ? GL_TRUE : GL_FALSE);
            mState->setDepthFunc(convertCompareFunction(b.getDepthCompareFunc()));

            if(b.getBiasConstant() != 0.0f || b.getBiasSlopeScale() != 0.0f)
            {
                mState->setEnabled(GL_POLYGON_OFFSET_FILL);
                mState->setEnabled(GL_POLYGON_OFFSET_POINT);
                mState->setEnabled(GL_POLYGON_OFFSET_LINE);
                glPolygonOffset(mReverseDepth ? b.getBiasSlopeScale() : -b.getBiasSlopeScale(), mReverseDepth ? b.getBiasConstant() : -b.getBiasConstant());
            }
            else
            {
                mState->setDisabled(GL_POLYGON_OFFSET_FILL);
                mState->setDisabled(GL_POLYGON_OFFSET_POINT);
                mState->setDisabled(GL_POLYGON_OFFSET_LINE);
            }
        }
        else
        {
            mState->setDisabled(GL_DEPTH_TEST);
        }
		
        if(b.isStencilEnable())
        {
            mFrontStencilMask = s.mFrontTestMark;
            mBackStencilMask = s.mBackTestMark;
            if(s.mBothSide)
            {
                if (!mFeature->isSupport(GF_Stencil_BothSide))
                    N_EXCEPT(InvalidParam, _I18n("2-sided stencils are not supported"));

                // NB: We should always treat CCW as front face for consistent with default
                // culling mode. Therefore, we must take care with two-sided stencil settings.
                bool flip = (mInvertVertexWinding && !mActiveFrame->isFlipping()) ||
                    (!mInvertVertexWinding && mActiveFrame->isFlipping());
                if(GLEW_VERSION_2_0)
                {
                    GLCheck(glEnable(GL_STENCIL_TEST));
                    // 背面
                    GLCheck(glStencilMaskSeparate(GL_BACK, mBackStencilMask));
                    GLCheck(glStencilFuncSeparate(GL_BACK, convertCompareFunction(s.mBackFunc), s.mBackCmpValue, s.mBackCmpMark));
                    GLCheck(glStencilOpSeparate(GL_BACK, convertStencilOp(s.mBackStencilFailOp), 
                                                         convertStencilOp(s.mBackDepthFailOp),
                                                         convertStencilOp(s.mBackTestPassOp)));
                    // 前面
                    GLCheck(glStencilMaskSeparate(GL_FRONT, mFrontStencilMask);
                    GLCheck(glStencilFuncSeparate(GL_FRONT, convertCompareFunction(s.mFrontFunc), s.mFrontCmpValue, s.mFrontCmpMark));
                    GLCheck(glStencilOpSeparate(GL_FRONT, convertStencilOp(s.mFrontStencilFailOp),
                                                          convertStencilOp(s.mFrontDepthFailOp), 
                                                          convertStencilOp(s.mFrontTestPassOp)));
                }
                else
                {
                    GLCheck(glEnable(GL_STENCIL_TEST_TWO_SIDE));
                    // 背面
                    GLCheck(glActiveStencilFace(GL_BACK));
                    GLCheck(glStencilMask(mBackStencilMask));
                    GLCheck(glStencilFunc(convertCompareFunction(s.mBackFunc), s.mBackCmpValue, s.mBackCmpMark));
                    GLCheck(glStencilOp(convertStencilOp(s.mBackStencilFailOp), convertStencilOp(s.mBackDepthFailOp), convertStencilOp(s.mBackTestPassOp)));
                    // 前面
                    GLCheck(glActiveStencilFace(GL_FRONT);
                    GLCheck(glStencilMask(mFrontStencilMask);
                    GLCheck(glStencilFunc(convertCompareFunction(s.mFrontFunc), s.mFrontCmpValue, s.mFrontCmpMark);
                    GLCheck(glStencilOp(convertStencilOp(s.mFrontStencilFailOp), convertStencilOp(s.mFrontDepthFailOp), convertStencilOp(s.mFrontTestPassOp));
                }
            }
            else
            {
                GLCheck(glEnable(GL_STENCIL_TEST));
                mState->setStencilMask(mFrontStencilMask);
                GLCheck(glStencilFunc(convertCompareFunction(s.mFrontFunc), s.mFrontCmpValue, s.mFrontCmpMark));
                GLCheck(glStencilOp(convertStencilOp(s.mFrontStencilFailOp), convertStencilOp(s.mFrontDepthFailOp), convertStencilOp(s.mFrontTestPassOp)));
            }
        }
        else
        {
            GLCheck(glDisable(GL_STENCIL_TEST));
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setDepthCheck(bool b)
    {
        if(b)
        {
            glClearDepth(1.0f);
            mState->setEnabled(GL_DEPTH_TEST);
        }
        else
        {
            mState->setDisabled(GL_DEPTH_TEST);
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setDepthWrite(bool b)
    {
        mState->setDepthMask(b ? GL_TRUE : GL_FALSE);
        // Store for reference in _beginFrame
        mDepthWrite = b;
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setDepthOpFunc(CmpMode mode)
    {
        mState->setDepthFunc(convertCompareFunction(mode));
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setDepthBias(NIIf constantBias, NIIf slopeScaleBias)
    {
        if(constantBias != 0 || slopeScaleBias != 0)
        {
            mState->setEnabled(GL_POLYGON_OFFSET_FILL);
            mState->setEnabled(GL_POLYGON_OFFSET_POINT);
            mState->setEnabled(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(mReverseDepth? slopeScaleBias : -slopeScaleBias, mReverseDepth ? constantBias: -constantBias);
        }
        else
        {
            mState->setDisabled(GL_POLYGON_OFFSET_FILL);
            mState->setDisabled(GL_POLYGON_OFFSET_POINT);
            mState->setDisabled(GL_POLYGON_OFFSET_LINE);
        }
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::setColourWrite(Nmark mark)
    {
        mColourWrite = mark;
        mState->setColourMask(mark);
    }
    //-----------------------------------------------------------------------------
    String GLRenderSystem::getErrorString(NIIl errCode) const
    {
        const GLubyte *errString = gluErrorString (errCode);
        return (errString != 0) ? String((const Ntchar*) errString) : StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::setLight(bool enabled)
    {
        enable ? mState->setEnabled(GL_LIGHTING) : mState->setDisabled(GL_LIGHTING);
    }
    //-----------------------------------------------------------------------------
    bool GLRenderSystem::_setFog(FogMode mode)
    {
        GLint fogMode;
        switch (mode)
        {
        case FM_EXP:
            fogMode = GL_EXP;
            break;
        case FM_EXP2:
            fogMode = GL_EXP2;
            break;
        case FM_LINEAR:
            fogMode = GL_LINEAR;
            break;
        default:
            // Give up on it
            mState->setDisabled(GL_FOG);
            //mFFGPP->removeSyncParam(18);
            //mFFGPP->removeSyncParam(19);
            return false;
        }

        //mFFGPP->set(18, GSP_Fog_Params);
        //mFFGPP->set(19, GSP_Fog_Colour);
        mState->setEnabled(GL_FOG);
        glFogi(GL_FOG_MODE, fogMode);
        return true;
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::setFog(const ShaderChFog & f, Nmark applymark)
    {
        const Colour & colour = f.mColour;
        NIIf density = f.mDensity;
        NIIf start = f.mBegin;
        NIIf end = f.mEnd;

        GLint fogMode;
        switch(f.mMode)
        {
        case FM_EXP:
            fogMode = GL_EXP;
            break;
        case FM_EXP2:
            fogMode = GL_EXP2;
            break;
        case FM_LINEAR:
            fogMode = GL_LINEAR;
            break;
        default:
            // Give up on it
            mState->setDisabled(GL_FOG);
            return;
        }

        mState->setEnabled(GL_FOG);
        glFogi(GL_FOG_MODE, fogMode);
        GLfloat fogColor[4] = {colour.r, colour.g, colour.b, colour.a};
        glFogfv(GL_FOG_COLOR, fogColor);
        glFogf(GL_FOG_DENSITY, density);
        glFogf(GL_FOG_START, start);
        glFogf(GL_FOG_END, end);
        // XXX Hint here?
    }
    //-----------------------------------------------------------------------
    DataType GLRenderSystem::getColourType() const
    {
        return DT_Colour_ABGR;
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::getSysProj(const Matrix4f & in, Matrix4f & out, bool)
    {
        if(mReverseDepth)
        {
            RenderSys::getSysProj(in, out);
        }
        else
        {
            // no any conversion request for OpenGL
            out = in;
        }
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::getSysProj(const Radian & fovy, NIIf aspect, NIIf nearPlane, NIIf farPlane, Matrix4f & dest, bool gpuprog)
    {
        Radian thetaY (fovy / 2.0f);
        NIIf tanThetaY = Math::Tan(thetaY);
        //NIIf thetaX = thetaY * aspect;
        //NIIf tanThetaX = Math::Tan(thetaX);

        // Calc matrix elements
        NIIf w = (1.0f / tanThetaY) / aspect;
        NIIf h = 1.0f / tanThetaY;
        NIIf q, qn;
        if (farPlane == 0)
        {
            // Infinite far plane
            q = INFINITE_FAR_PLANE_ADJUST - 1;
            qn = nearPlane * (INFINITE_FAR_PLANE_ADJUST - 2);
        }
        else
        {
            q = -(farPlane + nearPlane) / (farPlane - nearPlane);
            qn = -2 * (farPlane * nearPlane) / (farPlane - nearPlane);
        }

        // NB This creates Z in range [-1,1]
        //
        // [ w   0   0   0  ]
        // [ 0   h   0   0  ]
        // [ 0   0   q   qn ]
        // [ 0   0   -1  0  ]

        dest = Matrix4<NIIf>::ZERO;
        dest[0][0] = w;
        dest[1][1] = h;
        dest[2][2] = q;
        dest[2][3] = qn;
        dest[3][2] = -1;
    }
    //------------------------------------------------------------------
    void GLRenderSystem::getSysProj(NIIf left, NIIf right, NIIf bottom, NIIf top, NIIf near, NIIf far, 
        Matrix4f & dest, bool gpuprog)
    {
        NIIf width = right - left;
        NIIf height = top - bottom;
        NIIf q, qn;
        if(far == 0)
        {
            // Infinite far plane
            q = INFINITE_FAR_PLANE_ADJUST - 1;
            qn = near * (INFINITE_FAR_PLANE_ADJUST - 2);
        }
        else
        {
            q = -(far + near) / (far - near);
            qn = -2 * (far * near) / (far - near);
        }
        dest = Matrix4<NIIf>::ZERO;
        dest[0][0] = 2 * near / width;
        dest[0][2] = (right+left) / width;
        dest[1][1] = 2 * near / height;
        dest[1][2] = (top+bottom) / height;
        dest[2][2] = q;
        dest[2][3] = qn;
        dest[3][2] = -1;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::getSysProj(const Matrix4 & in, Matrix4 & out, NIIf near, NIIf far, Frustum::Type type)
    {
        if(mReverseDepth)
        {
            RenderSys::getSysProj(in, out, near, far, type);
        }
        else
        {
            // no any conversion request for OpenGL
            out = in;
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::getOpenVrProj(const Matrix4 & in, Matrix4 & out)
    {
        if(mReverseDepth)
        {
            RenderSys::getSysProj(in, out);
        }
        else
        {
            out = in;

            // Convert depth range from [0,1] to [-1,1]
            out[2][0] = (out[2][0] + out[3][0]) * 2.0f;
            out[2][1] = (out[2][1] + out[3][1]) * 2.0f;
            out[2][2] = (out[2][2] + out[3][2]) * 2.0f;
            out[2][3] = (out[2][3] + out[3][3]) * 2.0f;
        }
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::getSysOrtho(const Radian & fovy, NIIf aspect,
        NIIf nearPlane, NIIf farPlane, Matrix4f & dest, bool forGpuProgram)
    {
        Radian thetaY (fovy / 2.0f);
        NIIf tanThetaY = Math::Tan(thetaY);

        //NIIf thetaX = thetaY * aspect;
        NIIf tanThetaX = tanThetaY * aspect; //Math::Tan(thetaX);
        NIIf half_w = tanThetaX * nearPlane;
        NIIf half_h = tanThetaY * nearPlane;
        NIIf iw = 1.0f / half_w;
        NIIf ih = 1.0f / half_h;
        NIIf q;
        if (farPlane == 0)
        {
            q = 0;
        }
        else
        {
            q = 2.0f / (farPlane - nearPlane);
        }

        dest = Matrix4<NIIf>::ZERO;
        dest[0][0] = iw;
        dest[1][1] = ih;
        dest[2][2] = -q;
        dest[2][3] = - (farPlane + nearPlane)/(farPlane - nearPlane);
        dest[3][3] = 1;
    }
    //-----------------------------------------------------------------------
    void GLRenderSystem::_setPolygonMode(PolygonMode level)
    {
        switch(level)
        {
        case PM_POINTS:
            mState->setPolygonMode(GL_POINT);
            break;
        case PM_LINE:
            mState->setPolygonMode(GL_LINE);
            break;
        default:
        case PM_SOLID:
            mState->setPolygonMode(GL_FILL);
            break;
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setStencilTest(bool b)
    {
        b ? mState->setEnabled(GL_STENCIL_TEST) : mState->setDisabled(GL_STENCIL_TEST);
    }
    //---------------------------------------------------------------------
    GLint GLRenderSystem::convertCompareFunction(CmpMode func) const
    {
        switch(func)
        {
        case CPM_ALWAYS_FAIL:
            return GL_NEVER;
        case CPM_ALWAYS_PASS:
            return GL_ALWAYS;
        case CPM_LESS:
            return GL_LESS;
        case CPM_LESS_EQUAL:
            return GL_LEQUAL;
        case CPM_EQUAL:
            return GL_EQUAL;
        case CPM_NOT_EQUAL:
            return GL_NOTEQUAL;
        case CPM_GREATER_EQUAL:
            return GL_GEQUAL;
        case CPM_GREATER:
            return GL_GREATER;
        default:
            return GL_ALWAYS;
        };
        // to keep compiler happy
        return GL_ALWAYS;
    }
    //---------------------------------------------------------------------
    GLint GLRenderSystem::convertStencilOp(StencilOpType op) const
    {
        switch(op)
        {
        case SOT_KEEP:
            return GL_KEEP;
        case SOT_ZERO:
            return GL_ZERO;
        case SOT_REPLACE:
            return GL_REPLACE;
        case SOT_INCR:
            return GL_INCR;
        case SOT_DECR:
            return GL_DECR;
        case SOT_INCR_WRAP:
            return GL_INCR_WRAP;
        case SOT_DECR_WRAP:
            return GL_DECR_WRAP;
        case SOT_INVERT:
            return GL_INVERT;
        };
        // to keep compiler happy
        return GL_KEEP;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setTextureFilter(Nidx unit, TextureFilterType ftype, TextureFilterOP fo)
    {
        if (mState->activateGLTextureUnit(unit))
		{
			switch (ftype)
			{
			case FT_MIN:
				mMinFilter = fo;

				// Combine with existing mip filter
				mState->setTexParameteri(mTextureTypes[unit], GL_TEXTURE_MIN_FILTER, getCombinedMinMipFilter(mMinFilter, mMipFilter));
				break;

			case FT_MAG:
				switch (fo)
				{
				case FO_ANISOTROPIC: // GL treats linear and aniso the same
				case FO_LINEAR:
					mState->setTexParameteri(mTextureTypes[unit], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;
				case FO_POINT:
				case FO_NONE:
					mState->setTexParameteri(mTextureTypes[unit], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					break;
				}
				break;
			case FT_MIP:
				mMipFilter = fo;

				// Combine with existing min filter
				mState->setTexParameteri(mTextureTypes[unit], GL_TEXTURE_MIN_FILTER, getCombinedMinMipFilter(mMinFilter, mMipFilter));
				break;
			}
		}
    }
    //-----------------------------------------------------------------------------
    void GLRenderSystem::_setTextureBlend(Nidx stage, const TextureBlend & bm)
    {
        if(stage >= mFixedFunctionTextureUnits)
        {
            return;
        }

        //颜色颜料源头1,源头2,混合操作类型
        GLenum src1op, src2op, cmd;
        GLfloat cv1[4], cv2[4];

        if(bm.mColourType == TCT_RGB)
        {
            cv1[0] = bm.mConstant1.r;
            cv1[1] = bm.mConstant1.g;
            cv1[2] = bm.mConstant1.b;
            cv1[3] = bm.mConstant1.a;
            mTextureBlend[stage][0] = bm.mConstant1;

            cv2[0] = bm.mConstant2.r;
            cv2[1] = bm.mConstant2.g;
            cv2[2] = bm.mConstant2.b;
            cv2[3] = bm.mConstant2.a;
            mTextureBlend[stage][1] = bm.mConstant2;
        }

        if(bm.mColourType == TCT_AAA)
        {
            cv1[0] = mTextureBlend[stage][0].r;
            cv1[1] = mTextureBlend[stage][0].g;
            cv1[2] = mTextureBlend[stage][0].b;
            cv1[3] = bm.mConstant1.a;

            cv2[0] = mTextureBlend[stage][1].r;
            cv2[1] = mTextureBlend[stage][1].g;
            cv2[2] = mTextureBlend[stage][1].b;
            cv2[3] = bm.mConstant2.a;
        }

        switch(bm.mColourSrc1)
        {
        case TBS_PRE_TEXTURE:
            src1op = GL_PREVIOUS;
            break;
        case TBS_TEXTURE:
            src1op = GL_TEXTURE;
            break;
        case TBS_CONSTANT:
            src1op = GL_CONSTANT;
            break;
        case TBS_DIFFUSE:
            src1op = GL_PRIMARY_COLOR;
            break;
            // XXX
        case TBS_SPECULAR:
            src1op = GL_PRIMARY_COLOR;
            break;
        default:
            src1op = 0;
        }

        switch (bm.mColourSrc2)
        {
        case TBS_PRE_TEXTURE:
            src2op = GL_PREVIOUS;
            break;
        case TBS_TEXTURE:
            src2op = GL_TEXTURE;
            break;
        case TBS_CONSTANT:
            src2op = GL_CONSTANT;
            break;
        case TBS_DIFFUSE:
            src2op = GL_PRIMARY_COLOR;
            break;
            // XXX
        case TBS_SPECULAR:
            src2op = GL_PRIMARY_COLOR;
            break;
        default:
            src2op = 0;
        }

        switch (bm.mOP)
        {
        case TBO_OPERAND1:
            cmd = GL_REPLACE;
            break;
        case TBO_OPERAND2:
            cmd = GL_REPLACE;
            break;
        case TBO_MODULATE:
            cmd = GL_MODULATE;
            break;
        case TBO_MODULATE_2X:
            cmd = GL_MODULATE;
            break;
        case TBO_MODULATE_4X:
            cmd = GL_MODULATE;
            break;
        case TBO_ADD:
            cmd = GL_ADD;
            break;
        case TBO_ADD_SIGNED:
            cmd = GL_ADD_SIGNED;
            break;
        case TBO_ADD_SMOOTH:
            cmd = GL_INTERPOLATE;
            break;
        case TBO_SUB:
            cmd = GL_SUBTRACT;
            break;
        case TBO_ADD_SPECULAR:
            cmd = GL_INTERPOLATE;
            break;
        case TBO_ADD_DIFFUSE:
            cmd = GL_INTERPOLATE;
            break;
        case TBO_ADD_TEXTURE:
            cmd = GL_INTERPOLATE;
            break;
        case TBO_ADD_PRE_TEXTURE:
            cmd = GL_INTERPOLATE;
            break;
        case TBO_CONSTANT_FACTOR:
            cmd = GL_INTERPOLATE;
            break;
        case TBO_DOT_PRODUCT:
            cmd = mFeature->isSupport(GF_Tex_Dot3) ? GL_DOT3_RGB : GL_MODULATE;
            break;
        default:
            cmd = 0;
        }

        if (!mState->activateGLTextureUnit(stage))
            return;

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

        if(bm.mColourType == TCT_RGB)
        {
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, cmd);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, src1op);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, src2op);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
        }
        else
        {
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, cmd);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, src1op);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, src2op);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_CONSTANT);
        }

        NIIf blendValue[4] = {0, 0, 0, bm.mConstantFactor};
        switch(bm.mOP)
        {
        case TBO_ADD_SPECULAR:
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PRIMARY_COLOR);
            break;
        case TBO_ADD_DIFFUSE:
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PRIMARY_COLOR);
            break;
        case TBO_ADD_TEXTURE:
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_TEXTURE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_TEXTURE);
            break;
        case TBO_ADD_PRE_TEXTURE:
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PREVIOUS);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PREVIOUS);
            break;
        case TBO_CONSTANT_FACTOR:
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blendValue);
            break;
        default:
            break;
        };

        switch(bm.mOP)
        {
        case TBO_MODULATE_2X:
            glTexEnvi(GL_TEXTURE_ENV, bm.mColourType == TCT_RGB ? GL_RGB_SCALE : GL_ALPHA_SCALE, 2);
            break;
        case TBO_MODULATE_4X:
            glTexEnvi(GL_TEXTURE_ENV, bm.mColourType == TCT_RGB ? GL_RGB_SCALE : GL_ALPHA_SCALE, 4);
            break;
        default:
            glTexEnvi(GL_TEXTURE_ENV, bm.mColourType == TCT_RGB ? GL_RGB_SCALE : GL_ALPHA_SCALE, 1);
            break;
        }

        if(bm.mColourType == TCT_RGB)
        {
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            if(bm.mOP == TBO_ADD_SPECULAR)
            {
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
            }
            else
            {
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
            }
        }

        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);
        if(bm.mColourSrc1 == TBS_CONSTANT)
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, cv1);
        if(bm.mColourSrc2 == TBS_CONSTANT)
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, cv2);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setGLLightPositionDirection(Light * lt, GLenum lightindex)
    {
        // Use general 4D vector which is the same as GL's approach
        Vector4f vec = lt->getRelSpace4();

        glLightfv(lightindex, GL_POSITION, &vec.x);

        if(lt->getType() == LT_SPOT)
        {
            vec = lt->getSpaceDirection();
            vec.w = 0.0;
            glLightfv(lightindex, GL_SPOT_DIRECTION, &vec.x);
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_render()
    {
        RenderSys::_render();

        mMaxBuiltInTextureAttribIndex = 0;

        GeoRawTypeMark geomark = mGeometry->getMark();

        VertexBuffer * gvb = getInstanceBuffer();
        VertexData * gvd = getInstanceData();
        bool hasInstanceData = (((geomark & GRT_Global) && (geomark & GRT_Instancing)) && gvb != 0 && gvd != 0);

        size_t numberOfInstances = mGeometry->mInstanceCount;

        if((geomark & GRT_Global) && (geomark & GRT_Instancing))
        {
            numberOfInstances *= getInstanceCount();
        }
		
        GLSLProgram * program = N_Only(GLSLProgram).getActiveProgram();

        if (!program)
        {
            N_log("Failed to create shader program.");
        }
		
        const GLVertexArrayObject & vao = static_cast<const GLVertexArrayObject &>(mGeometry);
        vao->bind(this);
        
        const IndexData * tidx = mGeometry->mIndex;
        const IndirectData * tidc = mGeometry->mIndirect;
        const VertexData * tvd = mGeometry->mVertex;
        
        if(geomark & GRT_Index)
        {
            mState->bindGLBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLIndexBuffer *>(tidx->mBuffer)->getGLBufferId());
        }
        // unconditionally modify VAO for global instance data (FIXME bad API)
        if(gvb != 0 && gvd != 0)
        {
            VertexUnitList::const_iterator elemIter, elemEnd = gvd->getUnits().end();
            for(elemIter = gvd->getUnits().begin(); elemIter != elemEnd; ++elemIter)
            {
                bindVertexElementToGpu(*elemIter, gvb, 0);
            }
        }
        
        // Bind atomic counter buffers.
        // if (isSupport(RSC_ATOMIC_COUNTERS))
        // {
        //     GLuint atomicsBuffer = 0;

        //     glGenBuffers(1, &atomicsBuffer);
        //     glBindBuffer(GL_ATOMIC_COUNTER_BUFFER,
        //                  static_cast<GL3PlusHardwareCounterBuffer*>(HardwareBufferManager::getSingleton().getCounterBuffer().getGLBufferId()));
        //                  //static_cast<GL3PlusHardwareCounterBuffer*>(mGeometry->getCounterBuffer().getGLBufferId()));
        //     // glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 3, NULL, GL_DYNAMIC_DRAW);
        //     // glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
        // }
        //TODO: Reset atomic counters somewhere

        //如果当前使用上了几何着色程序而且几何着色程序需要临近点信息

        const GLGeometryRaw * vao = static_cast<const GLGeometryRaw *>(mGeometry);
        GLenum primType = mRSO->mDS ? GL_PATCHES : vao->mPrimType[mUseAdjacency];
        // Render to screen!
        if (mRSO->mDS)
        {
            // Tessellation shader special case.
            // Note: Only evaluation (domain) shaders are required.

            // GLuint primCount = 0;
            // // Useful primitives for tessellation
            // switch( mGeometry->mType )
            // {
            // case GeometryRaw::OT_LINE_LIST:
            //     primCount = (GLuint)(geomark & GRT_Index ? tidx->indexCount : tvd->vertexCount) / 2;
            //     break;

            // case GeometryRaw::OT_LINE_STRIP:
            //     primCount = (GLuint)(geomark & GRT_Index ? tidx->indexCount : tvd->vertexCount) - 1;
            //     break;

            // case GeometryRaw::OT_TRIANGLE_LIST:
            //     primCount = (GLuint)(geomark & GRT_Index ? tidx->indexCount : tvd->vertexCount);
            //     //primCount = (GLuint)(geomark & GRT_Index ? tidx->indexCount : tvd->vertexCount) / 3;
            //     break;

            // case GeometryRaw::OT_TRIANGLE_STRIP:
            //     primCount = (GLuint)(geomark & GRT_Index ? tidx->indexCount : tvd->vertexCount) - 2;
            //     break;
            // default:
            //     break;
            // }

            // These are set via shader in DX11, SV_InsideTessFactor and SV_OutsideTessFactor
            // Hardcoding for the sample
            // float patchLevel(1.f);
            // GLCheck(glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, &patchLevel));
            // GLCheck(glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, &patchLevel));
            // GLCheck(glPatchParameteri(GL_PATCH_VERTICES, tvd->mCount));

            if(isSupport(CF_IndirectBuffer) && (geomark & GRT_Indirect))
            {
                GLCheck(glMultiDrawElementsIndirect(primType, indexType, tidc->mOffset * tidc->mType, tidc->mCount, sizeof(IndexIndirect)));
            }
            else if (geomark & GRT_Index)
            {
                void * oftData = VBO_BUFFER_OFFSET((tidx->mOffset + mGeometry->mOffset) * tidx->mBuffer->getUnitSize());
                GLenum indexType = (tidx->mBuffer->getUnitSize() == 2) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
                GLCheck(glDrawElements(GL_PATCHES, tidx->indexCount, indexType, oftData));
                //                GLCheck(glDrawArraysInstanced(GL_PATCHES, 0, primCount, 1));
            }
            else
            {
                GLCheck(glDrawArrays(GL_PATCHES, 0, tvd->mCount));
                //                GLCheck(glDrawArraysInstanced(GL_PATCHES, 0, primCount, 1));
            }
        }
        else if(geomark & GRT_Index)
        {
            void * oftData = VBO_BUFFER_OFFSET((tidx->mOffset + mGeometry->mOffset) * tidx->mBuffer->getUnitSize());

            GLenum indexType = (tidx->mBuffer->getUnitSize() == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

            do
            {
                // 更新推导出的深度偏量(多通道需要的设置)
                if(mAutoDepthBias && mCurrentRender > 0)
                {
                    _setDepthBias(mDepthBiasBase + mDepthBiasFactor * mCurrentRender, mDepthBiasSlopeScale);
                }

                if(isSupport(CF_IndirectBuffer) && (geomark & GRT_Indirect))
                {
                    GLCheck(glMultiDrawElementsIndirect(primType, indexType, tidc->mOffset * tidc->mType, tidc->mCount, sizeof(IndexIndirect)));
                }
                else if(isSupport(CF_BaseInstance) && (geomark & GRT_Indirect))
                {
                    IndexIndirect * ii = reinterpret_cast<IndexIndirect *>(mIndirectPtr + tidc->mOffset * tidc->mType);

                    const size_t unitsize = tidx->mBuffer->getUnitSize();
                    
                    for(uint32 i = tidc->mCount; i--;)
                    {
                        GLCheck( glDrawElementsInstancedBaseVertexBaseInstance(primType, ii->mCount, indexType,
                            reinterpret_cast<void*>(ii->mOft * unitsize), ii->mInstanceCount, ii->mVertexOft, ii->mInstanceOft));
                        ++ii;
                    }
                }
                else if(geomark & GRT_Indirect)
                {
                    IndexIndirect * ii = reinterpret_cast<IndexIndirect *>(mIndirectPtr + tidc->mOffset * tidc->mType);

                    const size_t unitsize = tidx->mBuffer->getUnitSize();
                        
                    GLSLProgram * program = N_Only(GLSLProgram).getActiveProgram();

                    for(uint32 i = tidc->mCount; i--;)
                    {
                        GLCheck(glUniform1ui(program->mBaseInstanceLocation, static_cast<GLuint>(ii->mInstanceOft)));

                        GLCheck(glDrawElementsInstancedBaseVertex(primType, ii->mCount, indexType,
                            reinterpret_cast<void*>( ii->mOft * unitsize ), ii->mInstanceCount, ii->mVertexOft));
                        ++ii;
                    }
                }
                else if(hasInstanceData)
                {
                    GLCheck(glDrawElementsInstancedBaseVertex(primType, mGeometry->mCount, indexType, oftData, numberOfInstances, tvd->mOffset));
                }
                else
                {
                    GLCheck(glDrawElementsBaseVertex(primType, mGeometry->mCount, indexType, oftData, tvd->mOffset));
                }
               
                
            } while (updateParam());
        }
        else
        {
            do
            {
                // 更新推导出的深度偏量
                if(mAutoDepthBias && mCurrentRender > 0)
                {
                    _setDepthBias(mDepthBiasBase + mDepthBiasFactor * mCurrentRender, mDepthBiasSlopeScale);
                }

                if(if(isSupport(CF_IndirectBuffer))&& (geomark & GRT_Indirect))
                {
                    GLCheck(glMultiDrawArraysIndirect(primType, tidc->mOffset * tidc->mType, tidc->mCount, sizeof(VertexIndirect)));
                }
                else if(isSupport(CF_BaseInstance) && (geomark & GRT_Indirect))
                {
                    if(geomark & GRT_Indirect)
                    {
                        VertexIndirect * vi = reinterpret_cast<VertexIndirect *>(mIndirectPtr + tidc->mOffset * tidc->mType);
                        
                        for(uint32 i = tidc->mCount; i--;)
                        {
                            GLCheck(glDrawArraysInstancedBaseInstance(primType, vi->mOft, vi->mCount, vi->mInstanceCount, vi->mInstanceOft));
                            ++vi;
                        }
                    }
                    else
                    {
                        GLCheck(glDrawArraysInstancedBaseInstance(primType, mGeometry->mOffset, vi->mCount, vi->mInstanceCount, vi->mInstanceOft));
                    }
                    
                }
                else if(geomark & GRT_Indirect)
                {
                    VertexIndirect * vi = reinterpret_cast<VertexIndirect *>(mIndirectPtr + tidc->mOffset * tidc->mType);

                    GLSLProgram * program = N_Only(GLSLProgram).getActiveProgram();
                    for(uint32 i = tidc->mCount; i--; )
                    {
                        GLCheck(glUniform1ui(program->mBaseInstanceLocation, static_cast<GLuint>(vi->mInstanceOft)));

                        GLCheck(glDrawArraysInstanced(primType, vi->mOft, vi->mCount, vi->mInstanceCount));
                        ++vi;
                    }
                }
                else if(numberOfInstances)
                {
                    GLCheck(glDrawArraysInstanced(primType, mGeometry->mOffset, mGeometry->mCount, numberOfInstances));
                }
                else
                {
                    GLCheck(glDrawArrays(primType, mGeometry->mOffset, mGeometry->mCount));
                }
            } while(updateParam());
        }
        
        /*mState->activateGLTextureUnit(0);

        /*if(multitexturing)
        {
            Nui16 enableTexCnt = N_MAX(Nui16, N_MIN(Nui16, mEnableTexCount, mFixedFunctionTextureUnits), (mMaxBuiltInTextureAttribIndex + 1));
            for(Nui16 i = 0; i < enableTexCnt; i++)
            {
                glClientActiveTexture(GL_TEXTURE0 + i);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
            glClientActiveTexture(GL_TEXTURE0);
        }
        else
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        // unbind any custom attributes
        for (vector<GLuint>::type::iterator ai = mRenderAttribsBound.begin(); ai != mRenderAttribsBound.end(); ++ai)
        {
            glDisableVertexAttribArray(*ai);
        }

        // unbind any instance attributes
        for (vector<GLuint>::type::iterator ai = mRenderInstanceAttribsBound.begin(); ai != mRenderInstanceAttribsBound.end(); ++ai)
        {
            glVertexAttribDivisor(*ai, 0);
        }*/

        //mRenderAttribsBound.clear();
        //mRenderInstanceAttribsBound.clear();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setUnitNormals(bool b)
    {
        b ? mState->setEnabled(GL_NORMALIZE) : mState->setDisabled(GL_NORMALIZE);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::on(GpuProgram * prg)
    {
        GLGpuProgram * glprg = static_cast<GLGpuProgram *>(prg);

        // Unbind previous gpu program first.
        //
        // Note:
        //  1. Even if both previous and current are the same object, we can't
        //     bypass re-bind completely since the object itself maybe modified.
        //     But we can bypass unbind based on the assumption that object
        //     internally GL program type shouldn't be changed after it has
        //     been created. The behavior of bind to a GL program type twice
        //     should be same as unbind and rebind that GL program type, even
        //     for difference objects.
        //  2. We also assumed that the program's type (vertex or fragment) should
        //     not be changed during it's in using. If not, the following switch
        //     statement will confuse GL state completely, and we can't fix it
        //     here. To fix this case, we must coding the program implementation
        //     itself, if type is changing (during load/unload, etc), and it's inuse,
        //     unbind and notify render system to correct for its state.
        //
        switch (glprg->getType())
        {
        case GpuProgram::ST_VS:
            if (mRSO->mVS != glprg)
            {
                if(mRSO->mVS)
                    mRSO->mVS->unbind();
                mRSO->mVS = glprg;
            }
            break;
        case GpuProgram::ST_TS:
            if (mRSO->mTS != glprg)
            {
                if(mRSO->mTS)
                    mRSO->mTS->unbind();
                mRSO->mTS = glprg;
            }
            break;
        case GpuProgram::ST_DS:
            if (mRSO->mDS != glprg)
            {
                if(mRSO->mDS)
                    mRSO->mDS->unbind();
                mRSO->mDS = glprg;
            }
            break;
        case GpuProgram::ST_FS:
            if (mRSO->mFS != glprg)
            {
                if(mRSO->mFS)
                    mRSO->mFS->unbind();
                mRSO->mFS = glprg;
            }
            break;
        case GpuProgram::ST_GS:
            if (mRSO->mGS != glprg)
            {
                if(mRSO->mGS)
                    mRSO->mGS->unbind();
                mRSO->mGS = glprg;
            }
            break;
        case GPT_COMPUTE_PROGRAM:
            if (mCS != glprg)
            {
                if(mCS)
                    mCS->unbind();
                mCS  = glprg;
            }
            break;
        default:
            break;
        }

        glprg->bind();
        RenderSys::on(prg);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::off(GpuProgram::ShaderType gptype)
    {
        switch(gptype)
        {
        case GpuProgram::ST_VS : 
            if(mRSO->mVS)
            {
                mActiveVGPP = 0;
                mRSO->mVS->unbind();
                mRSO->mVS = 0;
            }
            break;
        case GpuProgram::ST_GS : 
            if(mRSO->mGS)
            {
                mActiveGGPP = 0;
                mRSO->mGS->unbind();
                mRSO->mGS = 0;
            }
            break;
        case GpuProgram::ST_FS : 
            if(mRSO->mFS)
            {
                mActiveFGPP = 0;
                mRSO->mFS->unbind();
                mRSO->mFS = 0;
            }
            break;
        case GpuProgram::ST_TS :
            if(mRSO->mTS)
            {
                mActiveTGPP = 0;
                mRSO->mTS->unbind();
                mRSO->mTS = 0;
            }
            break;
        case GpuProgram::ST_DS :
            if(mRSO->mDS)
            {
                mActiveDGPP = 0;
                mRSO->mDS->unbind();
                mRSO->mDS = 0;
            }
            break;
        case GpuProgram::ST_CS :
            if(mCS)
            {
                mActiveCGPP = 0;
                mCS->unbind();
                mCS = 0;
            }
            break;
		}
        RenderSys::off(gptype);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::on(GpuProgram::ShaderType gptype, GpuProgramParam * params, Nmark mask)
    {
        //if(mask & GPT_Render)
        {
            // We could maybe use GL_EXT_bindable_uniform here to produce Dx10-style
            // shared constant buffers, but GPU support seems fairly weak?
            // for now, just copy
            params->syncValue();
			
            switch (gptype)
            {
            case GpuProgram::ST_VS:
                mActiveVGPP = params;
                mRSO->mVS->bindSharedParameters(params, mask);
                break;
            case GpuProgram::ST_FS:
                mActiveFGPP = params;
                mRSO->mFS->bindSharedParameters(params, mask);
                break;
            case GpuProgram::ST_GS:
                mActiveGGPP = params;
                mRSO->mGS->bindSharedParameters(params, mask);
                break;
            case GpuProgram::ST_TS:
                mActiveTGPP = params;
                mRSO->mTS->bindSharedParameters(params, mask);
                break;
            case GpuProgram::ST_DS:
                mActiveDGPP = params;
                mRSO->mDS->bindSharedParameters(params, mask);
                break;
            case GpuProgram::ST_CS:
                mActiveCGPP = params;
                mCS->bindSharedParameters(params, mask);
                break;
            default:
                break;
            }
        }

        switch(gptype)
        {
        case GpuProgram::ST_VS:
            mActiveVGPP = params;
            mRSO->mVS->bindParameters(params, mask);
            break;
        case GpuProgram::ST_GS:
            mActiveGGPP = params;
            mRSO->mGS->bindParameters(params, mask);
            break;
        case GpuProgram::ST_FS:
            mActiveFGPP = params;
            mRSO->mFS->bindParameters(params, mask);
            break;
        case GpuProgram::ST_TS:
            mActiveTGPP = params;
            mRSO->mTS->bindParameters(params, mask);
            break;
        case GpuProgram::ST_DS:
            mActiveDGPP = params;
            mRSO->mDS->bindParameters(params, mask);
            break;
        case GpuProgram::ST_CS:
            mActiveCGPP = params;
            mCS->bindParameters(params, mask);
            break;
        default:
            break;
        }
    }
    //---------------------------------------------------------------------
    bool GLRenderSystem::updateParam()
    {
        if(RenderSys::updateParam())
        {
            if(mActiveVGPP != 0)
            {
                mActiveVGPP->setRenderCount(*mActiveVGPP->getRenderCount() + 1);
                mRSO->mVS->bindPassIterationParameters(mActiveVGPP);
            }
            if(mActiveGGPP != 0)
            {
                mActiveGGPP->setRenderCount(*mActiveGGPP->getRenderCount() + 1);
                mRSO->mGS->bindPassIterationParameters(mActiveGGPP);
            }
            if(mActiveFGPP != 0)
            {
                mActiveFGPP->setRenderCount(*mActiveFGPP->getRenderCount() + 1);
                mRSO->mFS->bindPassIterationParameters(mActiveFGPP);
            }
			if(mActiveTGPP != 0)
			{
				mActiveTGPP->setRenderCount(*mActiveTGPP->getRenderCount() + 1);
				mRSO->mTS->bindPassIterationParameters(mActiveTGPP);
			}
			if(mActiveDGPP != 0)
			{
				mActiveDGPP->setRenderCount(*mActiveDGPP->getRenderCount() + 1);
				mRSO->mDS->bindPassIterationParameters(mActiveDGPP);
			}
			if(mActiveCGPP != 0)
			{
				mActiveCGPP->setRenderCount(*mActiveCGPP->getRenderCount() + 1);
				mCS->bindPassIterationParameters(mActiveCGPP);
			}
            
            return true;
        }

        return false;
    }
    //---------------------------------------------------------------------
    bool GLRenderSystem::getBufferType(ViewportType type)
    {
        bool result = true;

        switch (type)
        {
        case VT_Back:
            GLCheck(glDrawBuffer(GL_BACK));
            break;
        case VT_BackLeft:
            GLCheck(glDrawBuffer(GL_BACK_LEFT));
            break;
        case VT_BackRight:
            GLCheck(glDrawBuffer(GL_BACK_RIGHT));
            //break;
        default:
            result = false;
        }

        return result;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setClipPlanesImpl(const PlaneList & clipPlanes)
    {
        // A note on GL user clipping:
        // When an ARB vertex program is enabled in GL, user clipping is completely
        // disabled. There is no way around this, it's just turned off.
        // When using GLSL, user clipping can work but you have to include a
        // glClipVertex command in your vertex shader.
        // Thus the planes set here may not actually be respected.
        NCount i = 0;
        NCount numClipPlanes;
        GLdouble clipPlane[4];

        // 存储前一个视图矩阵
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        // 只加载视图矩阵 (统一世界)
        GLfloat mat[16];
        makeGLMatrix(mat, mViewMatrix);
        glLoadMatrixf(mat);

        numClipPlanes = clipPlanes.size();
        for(i = 0; i < numClipPlanes; ++i)
        {
            GLenum clipPlaneId = static_cast<GLenum>(GL_CLIP_PLANE0 + i);
            const Plane & plane = clipPlanes[i];

            if(i >= 6/*GL_MAX_CLIP_PLANES*/)
            {
                N_EXCEPT(RenderingAPI, _I18n("Unable to set clip plane"));
            }

            clipPlane[0] = plane.mNormal.x;
            clipPlane[1] = plane.mNormal.y;
            clipPlane[2] = plane.mNormal.z;
            clipPlane[3] = plane.mD;

            glClipPlane(clipPlaneId, clipPlane);
            mState->setEnabled(clipPlaneId);
        }
        // 禁止多出的裁剪面
        for( ; i < 6/*GL_MAX_CLIP_PLANES*/; ++i)
        {
            mState->setDisabled((GLenum)(GL_CLIP_PLANE0 + i));
        }
        // 还原矩阵
        glPopMatrix();
        
        //GLCheck(glEnable(GL_DEPTH_CLAMP));
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setScissorTest(bool enabled, NCount left, NCount top, NCount right, NCount bottom)
    {
        mScissorsEnabled = enabled;
        // If request texture flipping, use "upper-left", otherwise use "lower-left"
        bool flipping = mActiveFrame->isFlipping();
        //  GL measures from the bottom, not the top
        NCount targetHeight = mActiveFrame->getHeight();
        // Calculate the "lower-left" corner of the viewport
        GLsizei x = 0, y = 0, w = 0, h = 0;

        if(enabled)
        {
            mState->setEnabled(GL_SCISSOR_TEST);
            // 注意GL使用宽/高,而不是左/下
            x = left;
            if(flipping)
                y = top;
            else
                y = targetHeight - bottom;
            w = right - left;
            h = bottom - top;
            GLCheck(glScissor(x, y, w, h));
            mScissorBox[0] = x;
            mScissorBox[1] = y;
            mScissorBox[2] = w;
            mScissorBox[3] = h;
        }
        else
        {
			mState->setDisabled(GL_SCISSOR_TEST);
            // GL 需要重置裁剪矩形
            w = mActiveViewports[0]->getPixelWidth();
            h = mActiveViewports[0]->getPixelHeight();
            x = mActiveViewports[0]->getPixelLeft();
            if(flipping)
                y = mActiveViewports[0]->getPixelTop();
            else
                y = targetHeight - mActiveViewports[0]->getPixelTop() - h;
            GLCheck(glScissor(x, y, w, h));
            mScissorBox[0] = x;
            mScissorBox[1] = y;
            mScissorBox[2] = w;
            mScissorBox[3] = h;
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::clearBuffer(Nmark buffers, const Colour & colour, NIIf depth, Nui16 stencil)
    {
        bool colourMask = (CM_RGBA != mColourWrite);

        GLbitfield flags = 0;
        if(buffers & FBT_COLOUR)
        {
            flags |= GL_COLOR_BUFFER_BIT;
            if(colourMask)
            {
                mState->setColourMask(CM_RGBA);
            }
            mState->setClearColour(colour.r, colour.g, colour.b, colour.a);
        }

        if(buffers & FBT_DEPTH)
        {
            flags |= GL_DEPTH_BUFFER_BIT;
            // 如果没启用缓存写入则启用它
            if(!mDepthWrite)
            {
                mState->setDepthMask(GL_TRUE);
            }
            mState->setClearDepth(depth);
        }

        if(buffers & FBT_STENCIL)
        {
            flags |= GL_STENCIL_BUFFER_BIT;
            // Enable buffer for writing if it isn't
            mState->setStencilMask(0xFFFFFFFF);
            GLCheck(glClearStencil(stencil));
        }

        // 因为清空区域依赖裁减口边界,所以要启用裁减测试
        if(!mScissorsEnabled)
        {
            mState->setEnabled(GL_SCISSOR_TEST);
        }

        // 设置裁剪口和视口一样
        GLint viewport[4];
        mState->getViewport(viewport);
        bool scissorBoxDifference =
            viewport[0] != mScissorBox[0] || viewport[1] != mScissorBox[1] ||
            viewport[2] != mScissorBox[2] || viewport[3] != mScissorBox[3];
        if (scissorBoxDifference)
        {
            GLCheck(glScissor(viewport[0], viewport[1], viewport[2], viewport[3]));
        }

        // 清除缓存
        GLCheck(glClear(flags));

        // Restore scissor box
        if (scissorBoxDifference)
        {
            GLCheck(glScissor(mScissorBox[0], mScissorBox[1], mScissorBox[2], mScissorBox[3]));
        }

        // Restore scissor test
        if (!mScissorsEnabled)
        {
            mState->setDisabled(GL_SCISSOR_TEST);
        }

        // 重置缓存写入状态
        if(!mDepthWrite && (buffers & FBT_DEPTH))
        {
            mState->setDepthMask(GL_FALSE);
        }
        if(colourMask && (buffers & FBT_COLOUR))
        {
            mState->setColourMask(mColourWrite);
        }
        if(buffers & FBT_STENCIL)
        {
            mState->setStencilMask(mFrontStencilMask);
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::create(PixelCountQuery *& obj)
    {
        obj = N_new GLPixelCountQuery();
        mPixelQueryList.push_back(obj);
    }
    //---------------------------------------------------------------------
    NIIf GLRenderSystem::getTexelXOffset() const
    {
        // 在GL中没有
        return 0.0f;
    }
    //---------------------------------------------------------------------
    NIIf GLRenderSystem::getTexelYOffset() const
    {
        // 在GL中没有
        return 0.0f;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::getObliqueDepthProj(const Plane & plane, Matrix4f & matrix, bool forGpuProgram)
    {
        // Thanks to Eric Lenyel for posting this calculation at www.terathon.com

        // Calculate the clip-space corner point opposite the clipping plane
        // as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
        // transform it into camera space by multiplying it
        // by the inverse of the projection matrix

        Vector4<NIIf> q;
        q.x = (Math::Sign(plane.mNormal.x) + matrix[0][2]) / matrix[0][0];
        q.y = (Math::Sign(plane.mNormal.y) + matrix[1][2]) / matrix[1][1];
        q.z = -1.0F;
        q.w = (1.0F + matrix[2][2]) / matrix[2][3];

        // Calculate the scaled plane vector
        Vector4<NIIf> clipPlane4d(plane.mNormal.x, plane.mNormal.y, plane.mNormal.z, plane.mD);
        Vector4<NIIf> c = clipPlane4d * (2.0F / (clipPlane4d.dotProduct(q)));

        // 替换投影矩阵的第三行
        matrix[2][0] = c.x;
        matrix[2][1] = c.y;
        matrix[2][2] = c.z + 1.0F;
        matrix[2][3] = c.w;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_oneTimeContextInitialization()
    {
		// Set nicer lighting model -- d3d9 has this by default
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
		GLCheck(glEnable(GL_COLOR_SUM));
		GLCheck(glDisable(GL_DITHER));
        
        if( mReverseDepth && (isVersionNewer(4, 5) || mGLSupport->checkGpuApi( "GL_ARB_clip_control" )) )
        {
            GLCheck( glClipControl( GL_LOWER_LEFT, GL_ZERO_TO_ONE ) );
        }
        else
        {
            mReverseDepth = false;
        }

        // 检测 FSAA 如果GLSupport中启用了,则启用里面的扩展
        int fsaa_active = false;
        GLCheck(glGetIntegerv(GL_SAMPLE_BUFFERS, (GLint*)&fsaa_active));
        if (fsaa_active)
        {
            GLCheck(glEnable(GL_MULTISAMPLE));
            N_log("Using FSAA.");
        }
        
        if (mGLSupport->checkGpuApi("GL_EXT_texture_filter_anisotropic"))
        {
            GLCheck(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mLargestSupportedAnisotropy));
        }

        GLCheck( glGenFramebuffers( 1, &mNullColourFramebuffer ) );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        // Some Apple NVIDIA hardware can't handle seamless cubemaps
        if (mFeature->getVendor() != GPU_NVIDIA)
#endif

		GLCheck (glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
		GLCheck(glProvokingVertex(GL_FIRST_VERTEX_CONVENTION));
		
        if (getFeature()->isSupport(GF_Debug))
        {
#if ENABLE_GL_DEBUG_OUTPUT
            GLCheck(glEnable(GL_DEBUG_OUTPUT));
            GLCheck(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
            GLCheck(glDebugMessageCallback(&GLDebugCallback, NULL));
            GLCheck(glDebugMessageControl(GL_DEBUG_SOURCE_THIRD_PARTY, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 0, NULL, GL_TRUE));
#endif
            g_hasDebugObjectLabel = true;
        }
        else
        {
            g_hasDebugObjectLabel = false;
        }

        if(getFeature()->isSupport(RSC_PRIMITIVE_RESTART))
        {
            GLCheck(glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX));
        }

        glEnable(GL_PROGRAM_POINT_SIZE);

        if(getFeature()->getVendor() == GPU_NVIDIA)
        {
            // bug in NVIDIA driver, see e.g.
            // https://www.opengl.org/discussion_boards/showthread.php/168217-gl_PointCoord-and-OpenGL-3-1-GLSL-1-4
            glEnable(0x8861); // GL_POINT_SPRITE
            glGetError();     // clear the error that it generates nevertheless..
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_switchContext(GLContext * context)
    {
        // Unbind GPU programs and rebind to new context later, because
        // scene manager treat render system as ONE 'context' ONLY, and it
        // cached the GPU programs using state.
        if (mRSO->mVS)
            mRSO->mVS->unbind();
        if (mRSO->mGS)
            mRSO->mGS->unbind();
        if (mRSO->mFS)
            mRSO->mFS->unbind();
        if (mRSO->mTS)
			mRSO->mTS->unbind();
        if (mRSO->mDS)
			mRSO->mDS->unbind();
        if (mCS)
			mCS->unbind();

        //for (Nui16 i = 0; i < mCurrentLights; ++i)
        //{
        //    setGLLight(i, false);
        //}
        //mCurrentLights = 0;

        // 禁止所有纹理
        //_enableTexture(0);

        if (mCurrentContext != context)
        {
#if N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
            // NSGLContext::makeCurrentContext does not flush automatically. everybody else does.
            glFlushRenderAPPLE();
#endif
            mCurrentContext->endCurrent();
            mCurrentContext = context;
        }

        mCurrentContext->setCurrent();

        mState = mCurrentContext->getState<GLStateCacheManager>();
		_completeDeferredVaoFboDestruction();
		
        // Check if the context has already done one-time initialisation
        if(!mCurrentContext->getInitialized())
        {
            _oneTimeContextInitialization();
            mCurrentContext->setInitialized();
        }

        // Rebind GPU programs to new context
        if (mRSO->mVS)
            mRSO->mVS->bind();
        if (mRSO->mGS)
            mRSO->mGS->bind();
        if (mRSO->mFS)
            mRSO->mFS->bind();
        if (mRSO->mTS)
			mRSO->mTS->bind();
        if (mRSO->mDS)
			mRSO->mDS->bind();
        if (mCS)
			mCS->bind();

        // Must reset depth/colour write mask to according with user desired, otherwise,
        // clearBuffer would be wrong because the value we are recorded may be
        // difference with the really state stored in GL context.
        //mState->setDepthMask(mDepthWrite);
        //mState->setColourMask(mColourWrite);
        //mState->setStencilMask(mFrontStencilMask);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setFrame(FrameObj * target)
    {
        // Unbind frame buffer object
        if(mActiveFrame)
            mRTTManager->unbind(mActiveFrame);

        mActiveFrame = target;

		GLRenderTarget * gltarget = dynamic_cast<GLRenderTarget*>(target);
        if (gltarget)
        {
            // Switch context if different from current one
            GLContext * newContext = gltarget->getContext();
            if(newContext && mCurrentContext != newContext)
            {
                _switchContext(newContext);
            }
            //Check the FBO's depth buffer status
            GLDepthBuffer * depthBuffer = static_cast<GLDepthBuffer*>(target->getExtBuffer());

            if(target->getRasterGroup() != GroupUnknow && (!depthBuffer || depthBuffer->getGLContext() != mCurrentContext))
            {
                //Depth is automatically managed and there is no depth buffer attached to this RT
                //or the Current context doesn't match the one this Depth buffer was created with
                //createExtBuffer(target);///////////////////
            }

            // Bind frame buffer object
            mRTTManager->bind(target);

            if (target->isHardwareSRGB())
            {
                GLCheck(glEnable(GL_FRAMEBUFFER_SRGB));

                // Note: could test GL_FRAMEBUFFER_SRGB_CAPABLE here before
                // enabling, but GL spec says incapable surfaces ignore the setting
                // anyway. We test the capability to enable isHardwareGammaEnabled.
            }
			else
			{
				GLCheck(glDisable(GL_FRAMEBUFFER_SRGB));
			}
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_unregisterContext(GLContext * context)
    {
        if(mCurrentContext == context)
        {
            // Change the context to something else so that a valid context
            // remains active. When this is the main context being unregistered,
            // we set the main context to 0.
            if(mCurrentContext != mMainContext)
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
    //---------------------------------------------------------------------
    NIIf GLRenderSystem::getMinDepth() const
    {
        // Range [-1.0f, 1.0f] or range [0.0f; 1.0f]
        return mReverseDepth ? 0.0f : -1.0f;
    }
    //---------------------------------------------------------------------
    NIIf GLRenderSystem::getMaxDepth() const
    {
        // 范围 [-1.0f, 1.0f]
        return 1.0f;
    }
    //---------------------------------------------------------------------
    Real GLRenderSystem::getDepthRange() const
    {
        return mReverseDepth ? 1.0f : 2.0f;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::unregisterThread()
    {
        // nothing to do here?
        // Don't need to worry about active context, just make sure we delete
        // on shutdown.
    }
    //---------------------------------------------------------------------
    bool GLRenderSystem::activateGLTextureUnit(NCount unit)
    {
        if (mActiveTextureUnit != unit)
        {
            if (unit < getFeature()->getMaxTexUnit())
            {
                GLCheck(glActiveTexture(GL_TEXTURE0 + unit));
                mActiveTextureUnit = unit;
                return true;
            }
            else if (!unit)
            {
                // always ok to use the first unit
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    //---------------------------------------------------------------------
    NCount GLRenderSystem::getDisplayMonitorCount() const
    {
        return mGLSupport->getDisplayMonitorCount();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::bindVertexElementToGpu(const VertexUnit & vunit, const VertexBuffer * vb, NCount vertexOft)
    {
        const GLVertexBuffer * hwGlBuffer = static_cast<const GLVertexBuffer *>(vb);

        mState->bindGLBuffer(GL_ARRAY_BUFFER, hwGlBuffer->getGLBufferId());

        void * oftData = VBO_BUFFER_OFFSET(vunit.mOffset + vertexOft * vb->getUnitSize());

        VertexType sem = vunit.mVType;
        NIIi sdidx = vunit.mIndex;
        bool isCustomAttrib = false;
        if (mRSO->mVS)
        {
            isCustomAttrib = N_Only(GLSL::GLSLProgram)->isAttributeValid(sem, sdidx);

            if (vunit.mDivisor)
            {
                GLint attrib = N_Only(GLSL::GLSLProgram)->getAttributeIndex(sem, sdidx);
                glVertexAttribDivisor(attrib, vunit.mDivisor);
                //mRenderInstanceAttribsBound.push_back(attrib);
            }
        }
        // Custom attribute support
        // tangents, binormals, blendweights etc always via this route
        // builtins may be done this way too
        if (isCustomAttrib)
        {
            GLint attrib = N_Only(GLSL::GLSLProgram)->getAttributeIndex(sem, sdidx);
            unsigned short typeCount = DataTypeUtil::getCount(vunit.mType);
            GLboolean normalised = GL_FALSE;
            switch(vunit.mType)
            {
            case DT_Colour:
            case DT_Colour_ABGR:
            case DT_Colour_ARGB:
                // Because GL takes these as a sequence of single unsigned bytes, count needs to be 4
                // VertexUnit::getTypeCount treats them as 1 (RGBA)
                // Also need to normalise the fixed-point data
                typeCount = 4;
                normalised = GL_TRUE;
                break;
            case DT_UByteUnit4x:
            case DT_ShortUnit2x:
            case DT_UShortUnit2x:
            case DT_ShortUnit4x:
            case DT_UShortUnit4x:
                normalised = GL_TRUE;
                break;
            default:
                break;
            };

            DataType dt = DataTypeUtil::getBaseType(vunit.getType());
			if(dt == DT_Double1x)
				GLCheck(glVertexAttribLPointer(attrib, typeCount, GLGBufferManager::getGLType(vunit.mType),
					static_cast<GLsizei>(vb->getUnitSize()), oftData));
			else if(dt == DT_Float1x)
                GLCheck(glVertexAttribIPointer(attrib, typeCount, GLGBufferManager::getGLType(vunit.mType),
                    static_cast<GLsizei>(vb->getUnitSize()), oftData));
            else
				GLCheck(glVertexAttribPointer(attrib, typeCount, GLGBufferManager::getGLType(vunit.mType),
					normalised, static_cast<GLsizei>(vb->getUnitSize()), oftData));
            GLCheck(glEnableVertexAttribArray(attrib));

            //mRenderAttribsBound.push_back(attrib);
        }
        else
        {
            switch(sem)
            {
            case VT_Pos:
                glVertexPointer(DataTypeUtil::getCount(vunit.mType), GLGBufferManager::getGLType(vunit.mType),
                    static_cast<GLsizei>(vb->getUnitSize()), oftData);
                glEnableClientState(GL_VERTEX_ARRAY);
                break;
            case VT_Normals:
                glNormalPointer(GLGBufferManager::getGLType(vunit.mType), static_cast<GLsizei>(vb->getUnitSize()), oftData);
                glEnableClientState(GL_NORMAL_ARRAY);
                break;
            case VT_Diffuse:
                glColorPointer(4, GLGBufferManager::getGLType(vunit.mType), static_cast<GLsizei>(vb->getUnitSize()), oftData);
                glEnableClientState(GL_COLOR_ARRAY);
                break;
            case VT_Specular:
                if (GLEW_EXT_secondary_color)
                {
                    glSecondaryColorPointerEXT(4, GLGBufferManager::getGLType(vunit.mType), static_cast<GLsizei>(vb->getUnitSize()), oftData);
                    glEnableClientState( GL_SECONDARY_COLOR_ARRAY );
                }
                break;
            case VT_Tex_Coord:
                if (mRSO->mVS)
                {
                    // Programmable pipeline - direct UV assignment
                    glClientActiveTexture(GL_TEXTURE0 + sdidx);
                    glTexCoordPointer(DataTypeUtil::getCount(vunit.mType), GLGBufferManager::getGLType(vunit.mType),
                        static_cast<GLsizei>(vb->getUnitSize()), oftData);
                    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
                    if (sdidx > mMaxBuiltInTextureAttribIndex)
                        mMaxBuiltInTextureAttribIndex = sdidx;
                }
                else
                {
                    bool multitexturing = (getFeature()->getMaxTexUnit() > 1);
                    // fixed function matching to units based on tex_coord_set
                    for(NCount i = 0; i < mEnableTexCount; ++i)
                    {
                        // Only set this texture unit's texcoord pointer if it
                        // is supposed to be using this element's index
                        if(mTextureCoordIndex[i] == sdidx && i < mFixedFunctionTextureUnits)
                        {
                            if(multitexturing)
                                glClientActiveTexture(GL_TEXTURE0 + i);
                            glTexCoordPointer(DataTypeUtil::getCount(vunit.mType), GLGBufferManager::getGLType(vunit.mType),
                                static_cast<GLsizei>(vb->getUnitSize()), oftData);
                            glEnableClientState( GL_TEXTURE_COORD_ARRAY );
                        }
                    }
                }
                break;
            default:
                break;
            };
        } // isCustomAttrib
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_copyContentsToMemory(Viewport* vp, const Box& src, PixelBlock &dst, ViewWindow::FaceType buffer)
    {
        GLenum format = GLPixelUtil::getGLOriginFormat(dst.mFormat);
        GLenum type = GLPixelUtil::getGLOriginDataType(dst.mFormat);

        if ((format == GL_NONE) || (type == 0))
        {
            N_EXCEPT(InvalidParam, _I18n("Unsupported format."));
        }

        // Switch context if different from current one
        _setViewport(vp);
        NCount rowskip = dst.mRowSize / dst.mPixelSize;
        if (dst.getWidth() != rowskip)
            glPixelStorei(GL_PACK_ROW_LENGTH, rowskip);
        // Must change the packing to ensure no overruns!
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glReadBuffer((buffer == ViewWindow::FT_Front) ? GL_FRONT : GL_BACK);

        uint32_t height = vp->getBuffer()->getHeight();

        glReadPixels((GLint)src.mLeft, (GLint)(height - src.mBottom), (GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(),
            format, type, dst.getData());

        // restore default alignment
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);

        dst.verticalFlip();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::initialiseExtensions(void)
    {
        mGLSupport->initialiseExtensions();
        // get driver version.
        // this is the recommended way for GL3 see: https://www.opengl.org/wiki/Get_Context_Info
        glGetIntegerv(GL_MAJOR_VERSION, &mVersion.major);
        glGetIntegerv(GL_MINOR_VERSION, &mVersion.minor);

        N_log("GL version = " + mVersion.toString());
        
        const GLubyte* pcVersion = glGetString(GL_VERSION);
        String tmpStr = (const char*)pcVersion;
        N_log("GL_VERSION = " + tmpStr);

        // Get vendor
        const GLubyte * pcVendor = glGetString(GL_VENDOR);
        tmpStr = (const char*)pcVendor;
        N_log("GL_VENDOR = " + tmpStr);
        mVendor = tmpStr.substr(0, tmpStr.find(' '));

        // Get renderer
        const GLubyte* pcRenderer = glGetString(GL_RENDERER);
        tmpStr = (const char*)pcRenderer;
        N_log("GL_RENDERER = " + tmpStr);

        // Set extension list
        Log::Stream log = LogManager::getSingleton().stream();
        String str;

        GLint numExt;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

        log << "GL_EXTENSIONS = ";
        for(int i = 0; i < numExt; i++)
        {
            const GLubyte* pcExt = glGetStringi(GL_EXTENSIONS, i);
            N_assert(pcExt && "Problems getting GL extension string using glGetString");
            str = String((const char*)pcExt);
            log << str << " ";
            mExtensionList.insert(str);
        }
    }
    //--------------------------------------------------------------------------------
    void GLRenderSystem::setFFPLightParams(size_t index, bool enabled)
    {
        if (!mFFGPP)
            return;

        size_t light_offset = 21 + 6 * index;
        if (enabled)
        {
            mFFGPP->set(light_offset + 0, GSP_Light_Pos_ParamIndex, (NIIi)index);
            mFFGPP->set(light_offset + 1, GSP_Light_Dir_ParamIndex, (NIIi)index);
            mFFGPP->set(light_offset + 2, GSP_Light_Diffuse_ParamIndex, (NIIi)index);
            mFFGPP->set(light_offset + 3, GSP_Light_Specular_ParamIndex, (NIIi)index);
            mFFGPP->set(light_offset + 4, GSP_Light_Attenuation_ParamIndex, (NIIi)index);
            mFFGPP->set(light_offset + 5, GSP_Light_Spot_Params_ParamIndex, (NIIi)index);
        }
        else
        {
            mFFGPP->removeSyncParam(light_offset + 0);
            mFFGPP->removeSyncParam(light_offset + 1);
            mFFGPP->removeSyncParam(light_offset + 2);
            mFFGPP->removeSyncParam(light_offset + 3);
            mFFGPP->removeSyncParam(light_offset + 4);
            mFFGPP->removeSyncParam(light_offset + 5);
        }
    }
	//--------------------------------------------------------------------------------
    uint32 GLRenderSystem::_createVao()
    {
        uint32 vao = 0;
        GLCheck(glGenVertexArrays(1, &vao));
        return vao;
    }
	//--------------------------------------------------------------------------------
    void GLRenderSystem::_destroyVao(GLContext * context, uint32 vao)
    {
        if(context != mCurrentContext)
            context->_getVaoDeferredForDestruction().push_back(vao);
        else
            GLCheck(glDeleteVertexArrays(1, &vao));
    }
	//--------------------------------------------------------------------------------
    void GLRenderSystem::_destroyFbo(GLContext * context, uint32 fbo)
    {
        if(context != mCurrentContext)
            context->_getFboDeferredForDestruction().push_back(fbo);
        else
            getStateManager()->deleteGLFrameBuffer(GL_FRAMEBUFFER, fbo);
    }
	//--------------------------------------------------------------------------------
    void GLRenderSystem::_bindVao(GLContext * context, uint32 vao)
    {
        N_assert(context == mCurrentContext, "VAO used in wrong OpenGL context");
        getStateManager()->bindGLVertexArray(vao);
    }
    //--------------------------------------------------------------------------------
    void GLRenderSystem::preExtraThreadsStarted()
    {
        N_lock_mutex(mThreadInitMutex);
        // free context, we'll need this to share lists
        if (mCurrentContext)
            mCurrentContext->endCurrent();
    }
    //--------------------------------------------------------------------------------
    void GLRenderSystem::postExtraThreadsStarted()
    {
        N_lock_mutex(mThreadInitMutex);
        // reacquire context
        if (mCurrentContext)
            mCurrentContext->setCurrent();
    }
    //--------------------------------------------------------------------------------
    void GLRenderSystem::beginProfiler(const String & eventName)
    {
        if (getFeature()->isSupport(RSC_DEBUG))
            GLCheck(glPushDebugGroup(GL_DEBUG_SOURCE_THIRD_PARTY, 0, static_cast<GLint>(eventName.length()), eventName.c_str()));
    }
    //--------------------------------------------------------------------------------
    void GLRenderSystem::endProfiler()
    {
        if (getFeature()->isSupport(RSC_DEBUG))
            GLCheck(glPopDebugGroup());
    }
    //--------------------------------------------------------------------------------
    void GLRenderSystem::markProfiler(const String & eventName)
    {
        if ( eventName.empty() )
            return;

        if (getFeature()->isSupport(RSC_DEBUG))
            glDebugMessageInsert(GL_DEBUG_SOURCE_THIRD_PARTY, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_LOW,
				static_cast<GLint>(eventName.length()), eventName.c_str());
    }
    //-----------------------------------------------------------------------------
    static bool g_hasDebugObjectLabel = false;
    void GlObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label)
    {
        if( g_hasDebugObjectLabel )
        {
            GLCheck(glObjectLabel(identifier, name, length, label));
        }
    }
    void GlObjectLabel(GLenum identifier, GLuint name, const String &label)
    {
        GlObjectLabel(identifier, name, label.size(), label.c_str());
    }

    void GLRenderSystem::reinit(void)
    {
        this->shutdown();
        this->init(true);
    }
    //-----------------------------------------------------------------------------------
    extern const IdString CustomAttributeIdString_GLCONTEXT;
    const IdString CustomAttributeIdString_GLCONTEXT( "GLCONTEXT" );
    void GLRenderSystem::_setContext( Texture * texture )
    {
        GL3PlusContext *newContext = 0;
        texture->getPlatformData( CustomAttributeIdString_GLCONTEXT, &newContext );
        if( newContext && mCurrentContext != newContext )
            _switchContext( newContext );
    }
    //-----------------------------------------------------------------------------------
    FrameBufferObject* GLRenderSystem::createFBO(void)
    {
        FrameBufferObject *retVal = N_new GLFrameBufferObject( this );
        mFBOList.insert( retVal );
        return retVal;
    }
    //-----------------------------------------------------------------------------------
    void GLRenderSystem::beginFBO(FrameBufferObject *desc, const ViewportList & vlist)
    {
        bool warnIfRtvWasFlushed = false;
        
        if( desc->mStepping && desc->isTargetEqual( mActiveFrame ) )
            return;

        if( !mGpuBufValid )
        {
            //Unbind in range [mFirstUavBoundSlot; mUavStartingSlot)
            if( mFirstUavBoundSlot < mUavStartingSlot )
            {
                for(size_t i = mFirstUavBoundSlot; i < mUavStartingSlot; ++i)
                {
                    GLCheck( glBindImageTexture( i, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI ) );
                    GLCheck( glBindBufferRange( GL_SHADER_STORAGE_BUFFER, i, 0, 0, 0 ) );
                }

                mFirstUavBoundSlot = 255;
            }

            //Unbind in range [lastUavToBindPlusOne, mLastUavBoundPlusOne)
            if( !mGpuParamBuffer ||
                mLastUavBoundPlusOne > (mUavStartingSlot + mGpuParamBuffer->mUavs.size()) )
            {
                size_t lastUavToBindPlusOne = mUavStartingSlot;
                if( mGpuParamBuffer )
                    lastUavToBindPlusOne += mGpuParamBuffer->mUavs.size();

                const size_t lastUavBoundPlusOne = mLastUavBoundPlusOne;

                for(size_t i = lastUavToBindPlusOne; i < lastUavBoundPlusOne; ++i)
                {
                    GLCheck( glBindImageTexture( i, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI ) );
                    GLCheck( glBindBufferRange( GL_SHADER_STORAGE_BUFFER, i, 0, 0, 0 ) );
                }

                mLastUavBoundPlusOne = 0;
            }

            _setBufferCS( mUavStartingSlot, mGpuParamBuffer );
            mGpuBufValid = true;
        }
        Texture *anyTarget = vlist[0]->getBuffer();
        uint32 numViewports = vlist.size();
        GLFrameBufferObject * currPassDesc = static_cast<GLFrameBufferObject*>( mActiveFrame );

        RenderSys::beginFBO( desc, vlist);

        GLFrameBufferObject * newPassDesc = static_cast<GLFrameBufferObject*>( desc );

        //Determine whether:
        //  1. We need to store current active FrameBufferObject
        //  2. We need to perform clears when loading the new FrameBufferObject
        uint32 entriesToFlush = 0;
        if( currPassDesc )
        {
            entriesToFlush = currPassDesc->willSwitchTo( newPassDesc, warnIfRtvWasFlushed );

            if( entriesToFlush != 0 )
                currPassDesc->performStoreActions( mHasArbInvalidateSubdata, entriesToFlush );
        }
        else
        {
            entriesToFlush = FrameBufferObject::TT_All;
        }

        if( entriesToFlush )
        {
            //If we clear, we need the whole viewport
            const GLsizei w = static_cast<GLsizei>( anyTarget->getWidth() );
            const GLsizei h = static_cast<GLsizei>( anyTarget->getHeight() );
            GLCheck( glViewport( 0, 0, w, h ) );
            GLCheck( glScissor( 0, 0, w, h ) );
        }

        newPassDesc->performLoadActions( mBlendChannelMask, mDepthWrite, mStencilList[0].writeMask, entriesToFlush );

        {
            GLfloat xywhVp[16][4];
            GLint xywhSc[16][4];
            for( size_t i = 0; i < numViewports; ++i )
            {
                xywhVp[i][0] = mActiveViewports[i].getPixelLeft();
                xywhVp[i][1] = mActiveViewports[i].getPixelTop();
                xywhVp[i][2] = mActiveViewports[i].getPixelWidth();
                xywhVp[i][3] = mActiveViewports[i].getPixelHeight();

                xywhSc[i][0] = mActiveViewports[i].getClipPixelLeft();
                xywhSc[i][1] = mActiveViewports[i].getClipPixelTop();
                xywhSc[i][2] = mActiveViewports[i].getClipPixelWidth();
                xywhSc[i][3] = mActiveViewports[i].getClipPixelHeight();

                if( !desc->isFlipping() )
                {
                    // Convert "upper-left" corner to "lower-left"
                    xywhVp[i][1] = anyTarget->getHeight() - xywhVp[i][3] - xywhVp[i][1];
                    xywhSc[i][1] = anyTarget->getHeight() - xywhSc[i][3] - xywhSc[i][1];
                }
            }
            glViewportArrayv( 0u, numViewports, reinterpret_cast<GLfloat*>( xywhVp ) );
            glScissorArrayv( 0u, numViewports, reinterpret_cast<GLint*>( xywhVp ) );
        }
        /*else
        {
            GLsizei x, y, w, h;
            w = mActiveViewports[0].getPixelWidth();
            h = mActiveViewports[0].getPixelHeight();
            x = mActiveViewports[0].getPixelLeft();
            y = mActiveViewports[0].getPixelTop();

            if( !desc->isFlipping() )
            {
                // Convert "upper-left" corner to "lower-left"
                y = anyTarget->getHeight() - h - y;
            }
            GLCheck( glViewport( x, y, w, h ) );

            w = mActiveViewports[0].getClipPixelWidth();
            h = mActiveViewports[0].getClipPixelHeight();
            x = mActiveViewports[0].getClipPixelLeft();
            y = mActiveViewports[0].getClipPixelTop();

            if( !desc->isFlipping() )
            {
                // Convert "upper-left" corner to "lower-left"
                y = anyTarget->getHeight() - h - y;
            }

            // Configure the viewport clipping
            GLCheck( glScissor( x, y, w, h ) );
        }*/
    }
    //-----------------------------------------------------------------------------------
    void GLRenderSystem::endFBO()
    {
        if(mActiveFrame)
        {
            GLFrameBufferObject * passDesc = static_cast<GLFrameBufferObject*>( mActiveFrame );
            passDesc->performStoreActions( mHasArbInvalidateSubdata, FrameBufferObject::TT_All );
        }
        GLCheck( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

        RenderSys::endFBO();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::clearBuffer(FrameBufferObject * fbo, Texture * dst, NCount mipLevel )
    {
        Vector4 fullVp( 0, 0, 1, 1 );
        ViewportList vplist;
        createViewport(vplist, 1, dst, &fullVp, &fullVp, mipLevel, false);
        beginFBO(fbo, vplist);
    }
    //-----------------------------------------------------------------------------------
    Texture * GLRenderSystem::createDepthImpl(Texture * colour, bool texture, PixelFormat pf)
    {
        if(pf == PF_UNKNOWN)
        {
            //GeForce 8 & 9 series are faster using 24-bit depth buffers. Likely
            //other HW from that era has the same issue. Assume GL <4.0 is old
            //HW that prefers 24-bit.
            pf = Texture::DepthFormat;
        }

        return RenderSys::createDepthImpl( colour, texture, pf );
    }
    //---------------------------------------------------------------------
    /*void GLRenderSystem::_setTexture(uint32 slotStart, const DescriptorSetTexture * set, uint32 hazardousTexIdx)
    {
        //Ignore hazardousTexIdx, GL doesn't care. It will happily let us do hazards.
        uint32 texUnit = slotStart;

        vectorL<const Texture*>::const_iterator itor = set->mTextures.begin();
        vectorL<const Texture*>::const_iterator end  = set->mTextures.end();

        while( itor != end )
        {
            GLCheck( glActiveTexture( static_cast<uint32>( GL_TEXTURE0 + texUnit ) ) );

            if( *itor )
            {
                const GL3PlusTextureGpu * textureGpu = static_cast<const GL3PlusTextureGpu*>( *itor );
                const GLenum texTarget = textureGpu->getGlTextureTarget();
                const GLuint texName = textureGpu->getDisplayTextureName();
                GLCheck( glBindTexture( texTarget, texName ) );
                mTextureTypes[texUnit] = texTarget;
            }
            else
            {
                GLCheck( glBindTexture( mTextureTypes[texUnit], 0 ) );
            }

            ++texUnit;
            ++itor;
        }

        GLCheck( glActiveTexture( GL_TEXTURE0 ) );
    }*/
    //---------------------------------------------------------------------
    void GLRenderSystem::_setTexture( Nidx slotStart, const ShaderChTexture * set )
    {
        Nidx texUnit = slotStart;

        GL3PlusDescriptorSetTexture2 * srvList = reinterpret_cast<GL3PlusDescriptorSetTexture2 *>( set->mRsData );
        vectorL<ShaderChTexture::Slot>::const_iterator itor = set->mTextures.begin();
        vectorL<ShaderChTexture::Slot>::const_iterator end  = set->mTextures.end();

        while( itor != end )
        {
            GLCheck( glActiveTexture( static_cast<Nidx>( GL_TEXTURE0 + texUnit ) ) );

            if( itor->isBuffer() )
            {
                //Bind buffer
                const ShaderChTexture::StorageUnit & bufferSlot = itor->getBuffer();
                if( bufferSlot.buffer )
                    bufferSlot.buffer->_bind( bufferSlot.offset, bufferSlot.sizeBytes );
            }
            else
            {
                //Bind texture
                const ShaderChTexture::SamplerUnit & texSlot = itor->getTexture();
                if( texSlot.texture )
                {
                    const size_t idx = texUnit - slotStart;
                    mTextureTypes[texUnit] = srvList[idx].target;
                    GLCheck( glBindTexture( srvList[idx].target, srvList[idx].texName ) );
                }
                else
                {
                    GLCheck( glBindTexture( mTextureTypes[texUnit], 0 ) );
                }
            }

            ++texUnit;
            ++itor;
        }

        GLCheck( glActiveTexture( GL_TEXTURE0 ) );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setSampler(Nidx slotStart, const TextureSample * set)
    {
        Nidx texUnit = slotStart;

        vectorL<const TextureSampleUnit*>::const_iterator itor = set->getList().begin();
        vectorL<const TextureSampleUnit*>::const_iterator end  = set->getList().end();

        while( itor != end )
        {
            const TextureSampleUnit *tsu = *itor;

            if( !tsu )
            {
                glBindSampler( texUnit, 0 );
            }
            else
            {
                glBindSampler( texUnit, static_cast<GLTextureSample>(set)->mGLID);
            }

            ++texUnit;
            ++itor;
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setTextureCS(Nidx slotStart, const ShaderChTexture * set)
    {
        _setTexture( slotStart, set );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setSamplerCS(Nidx slotStart, const TextureSample * set)
    {
        _setSampler( slotStart, set );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setBufferUavCS(Nidx slot, const GpuParamBufferUnit::StorageUnit & bufferSlot)
    {
        if( bufferSlot.buffer )
        {
            bufferSlot.buffer->bindBufferCS(slot, bufferSlot.offset, bufferSlot.sizeBytes);
        }
        else
        {
            GLCheck( glBindImageTexture(slot, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI) );
            GLCheck( glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, 0, 0, 0) );
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setTextureUavCS( Nidx slot, const GpuParamBufferUnit::SamplerUnit & texSlot, GLuint srvView )
    {
        if( texSlot.texture )
        {
            GLenum access;
            switch( texSlot.access )
            {
            case Buffer::OT_Read:
                access = GL_READ_ONLY;
                break;
            case Buffer::OT_WRITE:
                access = GL_WRITE_ONLY;
                break;
            case Buffer::OT_READWRITE:
                access = GL_READ_WRITE;
                break;
            default:
                N_EXCEPT(RenderingAPI, _I18n("Invalid ResourceAccess parameter '") + N_conv( texSlot.access ) + _T("'") );
                break;
            }

            PixelFormat pixelFormat = texSlot.pixelFormat;
            if( pixelFormat == PF_UNKNOWN )
                pixelFormat = texSlot.texture->getFormat();

            GLboolean isArrayTexture;
            const Texture::Type textureType = texSlot.texture->getTextureType();
            if( textureType == Texture::Type1DArray ||
                textureType == Texture::Type2DArray ||
                textureType == Texture::Type3D ||
                textureType == Texture::TypeCube ||
                textureType == Texture::T_CUBEArray )
            {
                isArrayTexture = GL_TRUE;
            }
            else
            {
                isArrayTexture = GL_FALSE;
            }
            const GLenum format = GL3PlusMappings::get( pixelFormat );

            GLCheck( glBindImageTexture( slot, srvView, texSlot.mipmapLevel, isArrayTexture,
                                      texSlot.textureArrayIndex, access, format ) );
        }
        else
        {
            GLCheck( glBindImageTexture( slot, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI ) );
            GLCheck( glBindBufferRange( GL_SHADER_STORAGE_BUFFER, slot, 0, 0, 0 ) );
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setBufferCS( Nidx slotStart, const GpuParamBuffer * set )
    {
        if( !set )
            return;

        GLuint * srvList = reinterpret_cast<GLuint*>( set->mRsData );
        size_t elementIdx = 0;

        vectorL<GpuParamBuffer::Slot>::const_iterator itor = set->mUavs.begin();
        vectorL<GpuParamBuffer::Slot>::const_iterator end  = set->mUavs.end();

        while( itor != end )
        {
            if( itor->isBuffer() )
                setBufferUavCS( slotStart, itor->getBuffer() );
            else
                setTextureUavCS( slotStart, itor->getTexture(), srvList[elementIdx] );

            ++elementIdx;
            ++slotStart;
            ++itor;
        }

        mFirstUavBoundSlot   = std::min<uint8>( mFirstUavBoundSlot, slotStart );
        mLastUavBoundPlusOne = std::max<uint8>( mLastUavBoundPlusOne, slotStart + set->mUavs.size() );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_bindVertexTexture( Nidx unit, Texture * tex )
    {
        _bindTexture(unit, tex, true);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_bindGeometryTexture( Nidx unit, Texture * tex )
    {
        _bindTexture(unit, tex, true);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_bindTessControlTexture( Nidx unit, Texture * tex )
    {
        _bindTexture(unit, tex, true);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_bindDomainTexture( Nidx unit, Texture *tex )
    {
        _bindTexture(unit, tex, true);
    }
    //---------------------------------------------------------------------
    GLenum GLRenderSystem::getBlendOperation( ColourBlendMode op )
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
            return GL_MIN;
        case SBO_MAX:
            return GL_MAX;
        }

        // To keep compiler happy.
        return GL_FUNC_ADD;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createStorageTask(StorageTask *& st)
    {
        assert( sizeof(void*) >= sizeof(GLbitfield) );

        assert( (st->readBarrierBits || st->writeBarrierBits) &&
                "A zero-bit memory barrier is invalid!" );

        GLbitfield barriers = 0;

        //TODO:
        //GL_QUERY_BUFFER_BARRIER_BIT is nearly impossible to determine
        //specifically
        //Should be used in all barriers? Since we don't yet support them,
        //we don't include it in case it brings performance down.
        //Or should we use 'All' instead for these edge cases?

        if( st->readBarrierBits & ReadBarrier::CpuRead ||
            st->writeBarrierBits & WriteBarrier::CpuWrite )
        {
            barriers |= GL_PIXEL_BUFFER_BARRIER_BIT|GL_TEXTURE_UPDATE_BARRIER_BIT|
                        GL_BUFFER_UPDATE_BARRIER_BIT|GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT;
        }

        if( st->readBarrierBits & ReadBarrier::Indirect )
            barriers |= GL_COMMAND_BARRIER_BIT;

        if( st->readBarrierBits & ReadBarrier::VertexBuffer )
            barriers |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT|GL_TRANSFORM_FEEDBACK_BARRIER_BIT;

        if( st->readBarrierBits & ReadBarrier::IndexBuffer )
            barriers |= GL_ELEMENT_ARRAY_BARRIER_BIT;

        if( st->readBarrierBits & ReadBarrier::ConstBuffer )
            barriers |= GL_UNIFORM_BARRIER_BIT;

        if( st->readBarrierBits & ReadBarrier::Texture )
            barriers |= GL_TEXTURE_FETCH_BARRIER_BIT;

        if( st->readBarrierBits & ReadBarrier::Uav ||
            st->writeBarrierBits & WriteBarrier::Uav )
        {
            barriers |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT|GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT;
        }

        if( st->readBarrierBits & (ReadBarrier::RenderTarget|ReadBarrier::DepthStencil) ||
            st->writeBarrierBits & (WriteBarrier::RenderTarget|WriteBarrier::DepthStencil) )
        {
            barriers |= GL_FRAMEBUFFER_BARRIER_BIT;
        }

        if( st->readBarrierBits == ReadBarrier::All ||
            st->writeBarrierBits == WriteBarrier::All )
        {
            barriers = GL_ALL_BARRIER_BITS;
        }

        st->mRsData = reinterpret_cast<void*>( barriers );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::destroyStorageTask(StorageTask * st)
    {
        assert( st->mRsData ); //A zero-bit memory barrier is invalid
        st->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncStorageTask(StorageTask * st)
    {
        
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_execute(StorageTask * st)
    {
        if( !glMemoryBarrier )
            return;

        GLbitfield barriers = static_cast<GLbitfield>( reinterpret_cast<intptr_t>(st->mRsData ) );

        assert( barriers && "A zero-bit memory barrier is invalid" );
        glMemoryBarrier( barriers );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createRSO(RenderStateObject *& rso)
    {
        rso = N_new GLRenderStateObject();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncRSO(RenderStateObject * _rso)
    {
        GLRenderStateObject * rso = static_cast<GLRenderStateObject * >(_rso);
        //
        // Macroblock stuff
        //
        rso->mGLDepthWrite = rso->mStencil->mDepthWrite ? GL_TRUE : GL_FALSE;
        CmpMode depthFunc = rso->mStencil->mDepthFunc;
        if(mReverseDepth)
            depthFunc = reverse(depthFunc);
        rso->mGLDepthFunc = convertCompareFunction(depthFunc);

        switch(rso->mStencil->mCullMode)
        {
        case CM_None:
            rso->mGLCullMode = 0;
            break;
        default:
        case CM_Clockwise:
            rso->mGLCullMode = GL_BACK;
            break;
        case CM_Anticlockwise:
            rso->mGLCullMode = GL_FRONT;
            break;
        }

        switch(rso->mStencil->mPolygonMode)
        {
        case PM_POINTS:
            //rso->mGLPolygonMode = GL_POINTS;
            rso->mGLPolygonMode = GL_POINT;
            break;
        case PM_WIREFRAME:
            //rso->mGLPolygonMode = GL_LINE_STRIP;
            rso->mGLPolygonMode = GL_LINE;
            break;
        default:
        case PM_SOLID:
            rso->mGLPolygonMode = GL_FILL;
            break;
        }

        //
        // Blendblock stuff
        //
        rso->mAlphaBlend = rso->mBlend->mSourceBlendFactor != SBF_ONE || rso->mBlend->mDestBlendFactor != SBF_ZERO;
        if(rso->mBlend->mSeparateBlend)
        {
            rso->mAlphaBlend |= rso->mBlend->mSourceBlendFactorAlpha != SBF_ONE || rso->mBlend->mDestBlendFactorAlpha != SBF_ZERO;
        }
        rso->mGLSourceBlend     = getBlendMode( rso->mBlend->mSourceBlendFactor );
        rso->mGLDestBlend       = getBlendMode( rso->mBlend->mDestBlendFactor );
        rso->mGLSourceBlendAlpha= getBlendMode( rso->mBlend->mSourceBlendFactorAlpha );
        rso->mGLDestBlendAlpha  = getBlendMode( rso->mBlend->mDestBlendFactorAlpha );
        rso->mGLBlendFunc       = getBlendOperation( rso->mBlend->mBlendOperation );
        rso->mGLBlendFuncAlpha  = getBlendOperation( rso->mBlend->mBlendOperationAlpha );

        //
        // Shader stuff
        //

        if(rso->mVS)
        {
            rso->mVS = static_cast<GLSLShader *>(rso->mVS->getMain());
        }
        if(rso->mGS)
        {
            rso->mGS = static_cast<GLSLShader *>(rso->mGS->getMain());
        }
        if(rso->mTS)
        {
            rso->mTS = static_cast<GLSLShader *>(rso->mTS->getMain());
        }
        if(rso->mDS)
        {
            rso->mDS = static_cast<GLSLShader *>(rso->mDS->getMain());
        }
        if(rso->mFS)
        {
            rso->mFS = static_cast<GLSLShader *>(rso->mFS->getMain());
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createStencil(ShaderChStencil *& scs)
    {
        scs->mRsData = reinterpret_cast<void *>(1);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::destroyStencil(ShaderChStencil * scs)
    {
        block->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncStencil(ShaderChStencil * scds)
    {
        
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createBlend(ShaderChBlend *& scb)
    {
        scb->mRsData = reinterpret_cast<void*>( 1 );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::destroyBlend(ShaderChBlend * scb)
    {
        block->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncBlend(ShaderChBlend * scb)
    {
        
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createTexSample(TextureSampleUnit *& tsu)
    {
        GLuint samplerName = 0;
        GLCheck(glGenSamplers(1, &samplerName));

        GLint minFilter, magFilter;
        switch(tsu->mMinFilter)
        {
        case FO_ANISOTROPIC:
        case FO_LINEAR:
            switch(tsu->mMipFilter)
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
            switch(tsu->mMipFilter)
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

        magFilter = tsu->mMagFilter <= FO_POINT ? GL_NEAREST : GL_LINEAR;

        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_MIN_FILTER, minFilter ) );
        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_MAG_FILTER, magFilter ) );

        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_WRAP_S, getTextureAddressingMode(tsu->mU)));
        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_WRAP_T, getTextureAddressingMode(tsu->mV)));
        GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_WRAP_R, getTextureAddressingMode(tsu->mW)));

        float borderColour[4] = { (float)tsu->mBorderColour.r, (float)tsu->mBorderColour.g,
                                  (float)tsu->mBorderColour.b, (float)tsu->mBorderColour.a };
        GLCheck( glSamplerParameterfv( samplerName, GL_TEXTURE_BORDER_COLOR, borderColour ) );
        GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_LOD_BIAS, tsu->mMipLodBias ) );
        GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_MIN_LOD, tsu->mMinLod ) );
        GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_MAX_LOD, tsu->mMaxLod ) );

        if( tsu->mCompareFunction != NUM_COMPARE_FUNCTIONS )
        {
            GLCheck( glSamplerParameteri( samplerName, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE ) );
            GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_COMPARE_FUNC, convertCompareFunction( tsu->mCompareFunction ) ) );
        }

        if( isSupport(RSC_ANISOTROPY) )
        {
            GLCheck( glSamplerParameterf( samplerName, GL_TEXTURE_MAX_ANISOTROPY_EXT, tsu->mMaxAnisotropy ) );
        }

        tsu->mRsData = reinterpret_cast<void*>( samplerName );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::destroyTexSample(TextureSampleUnit * block)
    {
        GLuint samplerName = static_cast<GLuint>( reinterpret_cast<intptr_t>(block->mRsData));
        glDeleteSamplers(1, &samplerName);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncTexSample(TextureSampleUnit * tsu)
    {
        
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createTexture( ShaderChTexture *& newSet )
    {
        const size_t numElements = newSet->mTextures.size();
        GL3PlusDescriptorSetTexture2 * srvList = new GL3PlusDescriptorSetTexture2[numElements];
        newSet->mRsData = srvList;

        vectorL<ShaderChTexture::Slot>::const_iterator itor = newSet->mTextures.begin();

        for( size_t i=0u; i<numElements; ++i )
        {
            if( itor->isTexture() && itor->getTexture().texture )
            {
                const ShaderChTexture::SamplerUnit & texSlot = itor->getTexture();
                const GL3PlusTextureGpu * textureGpu = static_cast<const GL3PlusTextureGpu*>( itor->getTexture().texture );
                GLuint displayTex = textureGpu->getDisplayTextureName();

                if( !texSlot.isValidView() )
                {
                    glGenTextures( 1u, &srvList[i].texName );

                    PixelFormat pixelFormat = texSlot.pixelFormat;
                    if( pixelFormat == PF_UNKNOWN )
                        pixelFormat = textureGpu->getFormat();

                    //const GLenum format = GL3PlusMappings::get( pixelFormat );
                    srvList[i].target = GL3PlusMappings::get( textureGpu->getTextureType(), texSlot.mForce2DArray );

                    uint8 numMipmaps = texSlot.numMipmaps;
                    if( !texSlot.numMipmaps )
                        numMipmaps = textureGpu->getNumMipmaps() - texSlot.mipmapLevel;

                    OGRE_ASSERT_LOW( numMipmaps <= textureGpu->getNumMipmaps() - texSlot.mipmapLevel &&
                                     "Asking for more mipmaps than the texture has!" );

                    glTextureView( srvList[i].texName, srvList[i].target, displayTex, GL3PlusMappings::get( pixelFormat ),
                                   texSlot.mipmapLevel, numMipmaps,
                                   texSlot.textureArrayIndex,
                                   textureGpu->getArray() - texSlot.textureArrayIndex );
                }
                else
                {
                    srvList[i].texName = displayTex;
                    srvList[i].target = textureGpu->getGlTextureTarget();
                }
            }
            else
            {
                srvList[i].target = 0;
                srvList[i].texName = 0;
            }

            ++itor;
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::destroyTexture(ShaderChTexture * set)
    {
        assert( set->mRsData );
        GL3PlusDescriptorSetTexture2 * srvList = reinterpret_cast<GL3PlusDescriptorSetTexture2 *>( set->mRsData );

        const size_t numElements = set->mTextures.size();
        vectorL<ShaderChTexture::Slot>::const_iterator itor = set->mTextures.begin();

        for( size_t i=0u; i<numElements; ++i )
        {
            if( itor->isTexture() && itor->getTexture().texture )
            {
                const ShaderChTexture::SamplerUnit &texSlot = itor->getTexture();
                if( !texSlot.isValidView() )
                    glDeleteTextures( 1u, &srvList[i].texName );
            }
        }

        delete [] srvList;
        set->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncTexture(ShaderChTexture * sct)
    {
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createBuffer( GpuParamBuffer *& newSet )
    {
        const size_t numElements = newSet->mUavs.size();
        GLuint *srvList = new GLuint[numElements];
        newSet->mRsData = srvList;

        vectorL<GpuParamBuffer::Slot>::const_iterator itor = newSet->mUavs.begin();

        for( size_t i = 0u; i < numElements; ++i )
        {
            if( itor->isTexture() && itor->getTexture().texture )
            {
                const GpuParamBuffer::SamplerUnit & texSlot = itor->getTexture();

                const GL3PlusTextureGpu * textureGpu = static_cast<const GL3PlusTextureGpu *>( itor->getTexture().texture );

                srvList[i] = textureGpu->getDisplayTextureName();

                if( !texSlot.isValidView() && texSlot.pixelFormat != PF_UNKNOWN &&
                    PixelUtil::isSRGB( texSlot.texture->getFormat() ) )
                {
                    GLCheck( glGenTextures( 1u, &srvList[i] ) );

                    PixelFormat pixelFormat = texSlot.pixelFormat;

                    //const GLenum format = GL3PlusMappings::get( pixelFormat );

                    GLCheck( glTextureView( srvList[i], textureGpu->getGlTextureTarget(),
                                         textureGpu->getDisplayTextureName(), GL3PlusMappings::get( pixelFormat ),
                                         0, textureGpu->getNumMipmaps(),
                                         0, textureGpu->getArray() ) );
                }
            }
            else
            {
                srvList[i] = 0;
            }

            ++itor;
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::destroyBuffer(GpuParamBuffer * set)
    {
        OGRE_ASSERT_LOW( set->mRsData );
        GLuint *srvList = reinterpret_cast<GLuint*>(set->mRsData);

        const size_t numElements = set->mUavs.size();
        vectorL<GpuParamBuffer::Slot>::const_iterator itor = set->mUavs.begin();

        for( size_t i=0u; i<numElements; ++i )
        {
            if( itor->isTexture() && itor->getTexture().texture )
            {
                const GpuParamBuffer::SamplerUnit &texSlot = itor->getTexture();
                if( !texSlot.isValidView() &&
                    texSlot.pixelFormat != PF_UNKNOWN &&
                    PixelUtil::isSRGB( texSlot.texture->getFormat() ) )
                {
                    glDeleteTextures( 1u, &srvList[i] );
                }
            }
        }

        delete [] srvList;
        set->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncBuffer(GpuParamBuffer * gpb)
    {
        
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setHlmsDepthblock(const GLRenderStateObject * rso)
    {
        const ShaderChStencil * s = rso->mStencil;
        if(s->mDepthCheck)
        {
            GLCheck(glEnable(GL_DEPTH_TEST));
        }
        else
        {
            GLCheck(glDisable(GL_DEPTH_TEST));
        }
        GLCheck(glDepthMask(rso->mGLDepthWrite));
        GLCheck(glDepthFunc(rso->mGLDepthFunc));

        _setDepthBias(s->mDepthBiasConstant, s->mDepthBiasSlopeScale);


        //Cull mode
        if( rso->mGLCullMode == 0 )
        {
            GLCheck( glDisable( GL_CULL_FACE ) );
        }
        else
        {
            GLCheck( glEnable( GL_CULL_FACE ) );
            GLCheck( glCullFace( rso->mGLCullMode ) );
        }

        //Polygon mode
        GLCheck( glPolygonMode( GL_FRONT_AND_BACK, rso->mGLPolygonMode ) );

        if( s->mScissorTestEnabled )
        {
            GLCheck( glEnable(GL_SCISSOR_TEST) );
        }
        else
        {
            GLCheck( glDisable(GL_SCISSOR_TEST) );
        }

        mDepthWrite         = s->mDepthWrite;
        mScissorsEnabled    = s->mScissorTestEnabled;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setHlmsBlendblock(const GLRenderStateObject * rso)
    {
        const ShaderChBlend *blend = rso->mBlend;
        if( rso->mAlphaBlend )
        {
            GLCheck(glEnable(GL_BLEND));
            if(blend->mSeparateBlend)
            {
                GLCheck( glBlendFuncSeparate( rso->mGLSourceBlend, rso->mGLDestBlend, rso->mGLSourceBlendAlpha, rso->mGLDestBlendAlpha ) );
                GLCheck( glBlendEquationSeparate( rso->mGLBlendFunc, rso->mGLBlendFuncAlpha ) );
            }
            else
            {
                GLCheck( glBlendFunc( rso->mGLSourceBlend, rso->mGLDestBlend ) );
                GLCheck( glBlendEquation( rso->mGLBlendFunc ) );
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
    //---------------------------------------------------------------------
    void GLRenderSystem::setRSO(const RenderStateObject * rso)
    {
        //Disable previous state
        GLSLShader::unbindAll();

        RenderSys::setRSO(rso);
        setCSO(0);

        Nmark newClipDistances = 0;
        if(rso)
            newClipDistances = rso->mRenderMark;

        if(mClipDistances != newClipDistances)
        {
            for( size_t i=0; i < 8u; ++i )
            {
                const Nmark bitFlag = 1u << i;
                bool oldClipSet = (mClipDistances & bitFlag) != 0;
                bool newClipSet = (newClipDistances & bitFlag) != 0;
                if( oldClipSet != newClipSet )
                {
                    if( newClipSet )
                        glEnable( GL_CLIP_DISTANCE0 + i );
                    else
                        glDisable( GL_CLIP_DISTANCE0 + i );
                }
            }

            mClipDistances = newClipDistances;
        }

        mUseAdjacency   = 0;
        mRSO            = rso;

        if( !rso )
            return;

        _setHlmsDepthblock(mRSO);
        _setHlmsBlendblock(mRSO);

        if( mRSO->mVS )
        {
            mRSO->mVS->bind();
            mActiveVGPP = mRSO->mVS->getDefaultParam();
            mVPMode = true;
        }
        if( mRSO->mGS )
        {
            mRSO->mGS->bind();
            mActiveGGPP = mRSO->mGS->getDefaultParam();
            mGPMode = true;

            mUseAdjacency = mRSO->mGS->isAdjacencyPrimitive() ? 1 : 0;
        }
        if( mRSO->mTS )
        {
            mRSO->mTS->bind();
            mActiveTGPP = mRSO->mTS->getDefaultParam();
            mTPMode = true;
        }
        if( mRSO->mDS )
        {
            mRSO->mDS->bind();
            mActiveDGPP = mRSO->mDS->getDefaultParam();
            mDPMode = true;
        }
        if( mRSO->mFS )
        {
            mRSO->mFS->bind();
            mActiveFGPP = mRSO->mFS->getDefaultParam();
            mFPMode = true;
        }
        
        N_Only(GLSL::GLSLProgram).getActiveProgram();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setCSO(const ComputeStateObject * cso)
    {
        GLSLShader *ncs = 0;

        if(cso)
        {
            ncs = reinterpret_cast<GLSLShader *>(cso->mExtData);

            if(mCS == ncs)
                return;
        }

        //Disable previous state
        GLSLShader::unbindAll();

        RenderSys::setRSO( (RenderStateObject*)0 );

        mUseAdjacency   = 0;
        mRSO            = 0;
        mCS = 0;

        if( !cso )
            return;

        mCS = ncs;
        mCS->bind();
        mActiveCGPP = cso->mParam;
        mCPMode = true;

        N_Only(GLSL::GLSLProgram).getActiveProgram();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_setIndirect( IndirectBuffer * ib )
    {
        if( mBufferManager->supportsIndirectBuffers() )
        {
            if( ib )
            {
                GLCheck( glBindBuffer( GL_DRAW_INDIRECT_BUFFER, static_cast<GL3PlusBufferInterface*>( ib->getGpuData() )->getVboName() ) );
            }
            else
            {
                GLCheck( glBindBuffer( GL_DRAW_INDIRECT_BUFFER, 0 ) );
            }
        }
        else
        {
            if( ib )
                mIndirectPtr = ib->getSwBufferPtr();
            else
                mIndirectPtr = 0;
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::createCSO(ComputeStateObject *& cso)
    {
        cso = N_new GLComputeStateObject();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::destroyCSO(ComputeStateObject * cso)
    {
        static_cast<GLComputeStateObject*>(cso)->mCS = 0;
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::syncCSO(ComputeStateObject * cso)
    {
        static_cast<GLComputeStateObject*>(cso)->mCS = cso->mCS->getMain();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_execute(const ComputeStateObject & cso)
    {
        glDispatchCompute( cso.mGroupCount[0], cso.mGroupCount[1], cso.mGroupCount[2] );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setGeometry(const GeometryRaw * geo)
    {
        RenderSys::setGeometry(geo);
        if( geo )
        {
            GLCheck( glBindVertexArray( static_cast<const GLGeometryRaw *>(geo)->mVaoId ) );
        }
        else
        {
            GLCheck( glBindVertexArray( 0 ) );
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::registerThread()
    {
        N_lock_mutex(mThreadInitMutex)
        // This is only valid once we've created the main context
        if(!mMainContext)
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot register a background thread before the main context has been created."));
        }

        // Create a new context for this thread. Cloning from the main context
        // will ensure that resources are shared with the main context
        // We want a separate context so that we can safely create GL
        // objects in parallel with the main thread
        GLContext * nctx = mMainContext->clone();
        mBackgroundContextList.push_back(nctx);

        // Bind this new context to this thread.
        nctx->setCurrent();

        _oneTimeContextInitialization();
        nctx->setInitialized();
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::beginProfiler(const String & eventName)
    {
        markProfiler("Begin Event: " + eventName);
        if(mIs43 || mGLSupport->checkGpuApi("ARB_debug_group"))
        {
            GLCheck(glPushDebugGroup(GL_DEBUG_SOURCE_THIRD_PARTY, 0, static_cast<GLint>( eventName.length() ), eventName.c_str()));
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::endProfiler()
    {
        markProfiler("End Event");
        if(mIs43 || mGLSupport->checkGpuApi("ARB_debug_group"))
            GLCheck( glPopDebugGroup() );
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::markProfiler(const String & eventName)
    {
        if(eventName.empty())
            return;

        if(mIs43 || mGLSupport->checkGpuApi("GL_KHR_debug"))
        {
            glDebugMessageInsert( GL_DEBUG_SOURCE_THIRD_PARTY,
                                  GL_DEBUG_TYPE_PERFORMANCE,
                                  0,
                                  GL_DEBUG_SEVERITY_LOW,
                                  static_cast<GLint>( eventName.length() ),
                                  eventName.c_str() );
        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::setupGPUProfiler(void)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_BindOpenGL();
#endif
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::shutdownGPUProfiler(void)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_UnbindOpenGL();
#endif
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::beginGPUProfiler(const String & name, Nui32 * hashCache)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_BeginOpenGLSample( name.c_str(), hashCache );
#endif
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::endGPUProfiler(const String & name)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_EndOpenGLSample();
#endif
    }
    //---------------------------------------------------------------------
    bool GLRenderSystem::checkGpuApi(const VString & ext) const
    {
        return mGLSupport->checkGpuApi(ext);
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_clear()
    {
        //OgreProfileExhaustive("GLRenderSystem::_clear");
        GLCheck(glFlush());
    }
    //---------------------------------------------------------------------
    void GLRenderSystem::_flush()
    {
        //OgreProfileExhaustive("GLRenderSystem::_flush");
        GLCheck(glFlush());
    }
    //---------------------------------------------------------------------
}