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

#include "NiiGLSLExtSupport.h"
#include "NiiGLSLProgram.h"
#include "NiiGLSLGpuProgram.h"
#include "NiiGLSLShader.h"
#include "NiiGLSLLinkProgramManager.h"
#include "NiiGpuProgramManager.h"
#include "NiiEngine.h"
#include "NiiException.h"
#include "NiiStrConv.h"
#include "NiiDefaultStream.h"


namespace NII 
{
namespace GLSL 
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLSLProgram
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLSLProgram::GLSLProgram(GLSLShader* vs, GLSLShader* ts, GLSLShader* ds, GLSLShader* gs, GLSLShader* fs, GLSLShader* cs)
        : GLSLProgramBase(vs)
        , mHullShader(ts)
        , mDomainShader(ds)
        , mGeometryShader(gs)
        , mFragmentShader(fs)
        , mComputeShader(cs)
    {
        // Initialise uniform cache
        mUniformCache = new GLUniformCache();
        if(mComputeShader)
            mVertexShader = mHullShader = mDomainShader = mGeometryShader = mFragmentShader = NULL;
    }
    //-----------------------------------------------------------------------
    GLSLProgram::~GLSLProgram(void)
    {
        glDeleteObject(mGLProgramHandle);
    }
    //-----------------------------------------------------------------------
    Nui32 GLSLShader::getCombinedHash() const
    {
        uint32 hash = 0;
        GpuProgram * progs[] = {mVertexShader, mFragmentShader, mGeometryShader, mHullShader, mDomainShader, mComputeShader};
        for (auto p : progs)
        {
            if(p) 
            {
                hash = p->getHash(hash);
            }
        }
        return hash;
    }
    //-----------------------------------------------------------------------
    void GLSLShader::setTransformFeedbackVaryings(const std::vector<String> & nameStrings)
    {
        // Get program object ID.
        GLuint programId;
        if (N_Engine().getRender()->getFeature()->isSupport(GF_GLSL_SeparateShaderObject))
        {
            //TODO include tessellation stages
            GLSLShader* glslGpuProgram = getGeometryShader();
            if (!glslGpuProgram)
                glslGpuProgram = getVertexShader();

            programId = glslGpuProgram->getGLProgramHandle();

            // force re-link
            N_Only(GpuProgram).destroyCache(glslGpuProgram->getHash());
            glslGpuProgram->setLinked(false);
        }
        else
        {
            programId = getGLProgramHandle();

            // force re-link
            GpuProgramManager::getSingleton().destroyCache(getCombinedHash());
        }
        mLinked = false;

        // Convert to const char * for GL
        std::vector<const char*> names;
        for (uint e = 0; e < nameStrings.size(); e++)
        {
            names.push_back(nameStrings[e].c_str());
        }

        GLCheck(glTransformFeedbackVaryings(programId, nameStrings.size(), &names[0], GL_INTERLEAVED_ATTRIBS));

#if _DEBUG
        activate();
        // Check if varyings were successfully set.
        GLchar Name[64];
        GLsizei Length(0);
        GLsizei Size(0);
        GLenum Type(0);
        // bool Validated = false;
        for (size_t i = 0; i < nameStrings.size(); i++)
        {
            GLCheck(glGetTransformFeedbackVarying(programId, i, 64, &Length, &Size, &Type, Name));
            LogManager::getSingleton().stream() << "Varying " << i << ": " << Name << " " << Length
                                                << " " << Size << " " << Type;
            // Validated = (Size == 1) && (Type == GL_FLOAT_VEC3);
            // std::cout << Validated << " " << GL_FLOAT_VEC3 << std::endl;
        }
#endif
    }
    //-----------------------------------------------------------------------
    String GLSLProgram::getCombinedName() const
    {
        String name;
        if (mVertexShader)
        {
            name += "Vertex Shader: ";
            name += mVertexShader->getName();
            name += "\n";
        }
        if (mHullShader)
        {
            name += "Tessellation Control Shader: ";
            name += mHullShader->getName();
            name += "\n";
        }
        if (mDomainShader)
        {
            name += "Tessellation Evaluation Shader: ";
            name += mDomainShader->getName();
            name += "\n";
        }
        if (mGeometryShader)
        {
            name += "Geometry Shader: ";
            name += mGeometryShader->getName();
            name += "\n";
        }
        if (mFragmentShader)
        {
            name += "Fragment Shader: ";
            name += mFragmentShader->getName();
            name += "\n";
        }
        if (mComputeShader)
        {
            name += "Compute Shader: ";
            name += mComputeShader->getName();
            name += "\n";
        }

        return name;
    }
    //-----------------------------------------------------------------------
    String GLSLProgram::getCombinedSource() const
    {
        String ret;
        size_t retsize = 0;

        if( mVertexShader )
            retsize += mVertexShader->getProgramCode().size();
        if( mHullShader )
            retsize += mHullShader->getProgramCode().size();
        if( mDomainShader )
            retsize += mDomainShader->getProgramCode().size();
        if( mGeometryShader )
            retsize += mGeometryShader->getProgramCode().size();
        if( mFragmentShader )
            retsize += mFragmentShader->getProgramCode().size();
        if( mComputeShader )
            retsize += mComputeShader->getProgramCode().size();

        ret.reserve( retsize );

        if( mVertexShader )
            ret += mVertexShader->getProgramCode();
        if( mHullShader )
            ret += mHullShader->getProgramCode();
        if( mDomainShader )
            ret += mDomainShader->getProgramCode();
        if( mGeometryShader )
            ret += mGeometryShader->getProgramCode();
        if( mFragmentShader )
            ret += mFragmentShader->getProgramCode();
        if( mComputeShader )
            ret += mComputeShader->getProgramCode();

        return ret;
    }
    //-----------------------------------------------------------------------
}
}

