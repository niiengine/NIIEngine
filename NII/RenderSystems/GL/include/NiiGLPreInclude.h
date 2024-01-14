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

#ifndef _NII_GLPreInclude_H_
#define _NII_GLPreInclude_H_

#include "NiiPreInclude.h"

namespace NII 
{
    class GLSupport;
    class GLPBuffer;
    class GLRenderSystem;
    class GLRenderSystemBase;
    class GLTexture;
    class GLTextureManager;
	class GLGpuProgramManager;
    class GLGpuProgram;
    class GLContext;
    class GLRTTManager;
    class GLFBOManager;
    class GLFrameBuffer;
    class GLFrameBufferBase;
    class GLSLProgramManager;
    class GLStateCacheManager;
    class GLRenderBuffer;
    class GLDepthBuffer;
    
    typedef GLContext GL3PlusContext;
    typedef GLRTTManager GL3PlusRTTManager;
    
    typedef shared_ptr<GLSLShader> GLSLShaderPtr;
    typedef shared_ptr<GLTexture> GL3PlusTexturePtr;
}

#if N_PLAT == N_PLAT_WIN32
    #if !defined( __MINGW32__ )
        #define WIN32_LEAN_AND_MEAN
        #ifndef NOMINMAX
            #define NOMINMAX // required to stop windows.h messing up std::min
        #endif
    #endif
	#include <windows.h>
	#include <wingdi.h>
	#include <GL/glew.h>
	#include <GL/wglew.h>
    //#include <GL/gl3w.h>
#elif N_PLAT == NII_PLATFORM_LINUX
	#include <GL/glew.h>
    //#include <GL/gl3w.h>
	#define GL_GLEXT_PROTOTYPES
#elif N_PLAT == NII_PLATFORM_APPLE
	#include <GL/glew.h>
    //#include <GL/gl3w.h>
#endif

namespace NII {
    inline const Ntchar * glErrorToString(GLenum glErr) {
        switch (glErr)
        {
        case GL_INVALID_ENUM:
            return _T("GL_INVALID_ENUM");
        case GL_INVALID_VALUE:
            return _T("GL_INVALID_VALUE");
        case GL_INVALID_OPERATION:
            return _T("GL_INVALID_OPERATION");
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return _T("GL_INVALID_FRAMEBUFFER_OPERATION");
        case GL_OUT_OF_MEMORY:
            return _T("GL_OUT_OF_MEMORY");
        default:
            return _T("");
        }
    }
    
    extern void GlObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label);
    extern void GlObjectLabel(GLenum identifier, GLuint name, const String & label);
}

/// Lots of generated code in here which triggers the new VC CRT security warnings
#if !defined( _CRT_SECURE_NO_DEPRECATE )
#define _CRT_SECURE_NO_DEPRECATE
#endif

#if (N_PLAT == N_PLAT_WIN32) && !defined(__MINGW32__) && !defined(NIIEngine_Static)
    #ifdef NIIEngineOGL_DLL
        #define _EngineGLAPI __declspec(dllexport)
    #else
        #if defined(__MINGW32__)
            #define _EngineGLAPI
        #else
            #define _EngineGLAPI __declspec(dllimport)
        #endif
    #endif
#elif defined (NII_GCC_VISIBILITY)
    #define _EngineGLAPI  __attribute__ ((visibility("default")))
#else
    #define _EngineGLAPI
#endif

// Convenience macro from ARB_vertex_buffer_object spec
#define GL_BUFFER_OFFSET(i) ((char *)(i))

#define ENABLE_GL_CHECK 0
#define ENABLE_GL_DEBUG_OUTPUT 0

#if ENABLE_GL_CHECK
#include "NiiString.h"
#define GLCheck(glFunc) \
{ \
    glFunc; \
    int e = glGetError(); \
    if (e != 0) \
    { \
        const char * errorString = ""; \
        switch(e) \
        { \
            case GL_INVALID_ENUM:       errorString = "GL_INVALID_ENUM";        break; \
            case GL_INVALID_VALUE:      errorString = "GL_INVALID_VALUE";       break; \
            case GL_INVALID_OPERATION:  errorString = "GL_INVALID_OPERATION";   break; \
            case GL_INVALID_FRAMEBUFFER_OPERATION:  errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";   break; \
            case GL_OUT_OF_MEMORY:      errorString = "GL_OUT_OF_MEMORY";       break; \
            default:                                                            break; \
        } \
        Nchar msgBuf[4096]; \
        StringList tokens;\
        StrUtil::split(#glFunc, tokens, "("); \
        Nsnprintf(msgBuf, "OpenGL error 0x%04X %s in %s at line %i for %s\n", e, errorString, _N_FUNCTION__, __LINE__, tokens[0].c_str()); \
        N_log(msgBuf, LL_Fatal); \
    } \
}
#else
#   define GLCheck(glFunc) { glFunc; }
#endif

#define GLCheck GLCheck

#endif