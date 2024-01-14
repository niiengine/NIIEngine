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

#include "NiiGLGpuProgram.h"
#include "NiiException.h"
#include "NiiStrConv.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "ps_1_4.h"
#include "nvparse.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLGpuProgram
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLGpuProgram::GLGpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs) :
        GpuProgram(rid, gid, ls, rs)
    {
        setupCmd(N_CmdObj_GLGpuProgram);
    }
    //-----------------------------------------------------------------------
    GLGpuProgram::~GLGpuProgram()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();
    }
    //-----------------------------------------------------------------------
    NCount GLGpuProgram::calcSize() const
    {
        NCount memSize = 0;

        // Delegate Names
        memSize += sizeof(GLuint);
        memSize += sizeof(GLenum);
        memSize += GpuProgram::calcSize();

        return memSize;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLArbGpuProgram
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLArbGpuProgram::GLArbGpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs) :
        GLGpuProgram(rid, gid, ls, rs)
    {
        glGenPrograms(1, &mProgramID);
    }
    //-----------------------------------------------------------------------
    GLArbGpuProgram::~GLArbGpuProgram()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();
    }
    //-----------------------------------------------------------------------
    void GLArbGpuProgram::bindProgram() const
    {
        glEnable(getProgramType());
        glBindProgram(getProgramType(), mProgramID);
    }
    //-----------------------------------------------------------------------
    void GLArbGpuProgram::unbindProgram() const
    {
        glBindProgram(getProgramType(), 0);
        glDisable(getProgramType());
    }
    //-----------------------------------------------------------------------
    void GLArbGpuProgram::bindProgramParameters(const GpuProgramParam * params, Nui16 mask) const
    {
        GLenum type = getProgramType();

        // only supports NIIf constants
        GpuParamBlockMap::const_iterator i, iend = params->getBlockList().end();
        for(i = params->getBlockList().begin(); i != iend; ++i)
        {
            if(i->second.mGBTMark & mask)
            {
                GLuint logicalIndex = i->first;
                const NIIf * pFloat = (NIIf *)params->getBufferData(i->second.mMemIndex);
                // Iterate over the params, set in 4-NIIf chunks (low-level)
                for(NCount j = 0; j < i->second.m32bSize; j+=4)
                {
                    glProgramLocalParameter4fv(type, logicalIndex, pFloat);
                    pFloat += 4;
                    ++logicalIndex;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void GLArbGpuProgram::bindProgramPassIterationParameters(const GpuProgramParam * params) const
    {
        const NIIf * pFloat = params->getRenderCount();
        if(pFloat != 0)
        {
            GLenum type = getProgramType();
            GLuint logicalIndex = params->getLocation(pFloat - &params->getBufferData()[0]);
            glProgramLocalParameter4fv(type, logicalIndex, pFloat);
        }
    }
    //-----------------------------------------------------------------------
    void GLArbGpuProgram::unloadImpl()
    {
        glDeletePrograms(1, &mProgramID);
    }
    //-----------------------------------------------------------------------
    void GLArbGpuProgram::loadCodeImpl(const VString & code)
    {
        if (GL_INVALID_OPERATION == glGetError())
            LogManager::getOnly().log(_I18n("Invalid Operation before loading program ") + mSrc);

        glBindProgram(getProgramType(), mProgramID);
        glProgramString(getProgramType(), GL_PROGRAM_FORMAT_ASCII, (GLsizei)mSource.length(), mSource.c_str());

        if (GL_INVALID_OPERATION == glGetError())
        {
            GLint errPos;
            glGetIntegerv(GL_PROGRAM_ERROR_POSITION, &errPos);
            NIIb* errStr = (NIIb*)glGetString(GL_PROGRAM_ERROR_STRING);
#if NII_STRING
            String temperrStr;
            DataStream::s2ws(errStr, temperrStr);
#else
            temperrStr = errStr;
#endif
            N_EXCEPT(Internal, _I18n("Cannot load GL vertex program ") + mSrc +
                _I18n(".  Line ") + N_conv(errPos) + _T(":\n") + temperrStr);
        }
        glBindProgram(getProgramType(), 0);
    }
    //-----------------------------------------------------------------------
    GLenum GLArbGpuProgram::getProgramType() const
    {
        switch (mType)
        {
        case ST_VS:
        default:
            return GL_VERTEX_PROGRAM;
        case ST_GS:
            return GL_GEOMETRY_PROGRAM_NV;
        case ST_FS:
            return GL_FRAGMENT_PROGRAM;
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLGpuNvparseProgram
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLGpuNvparseProgram::GLGpuNvparseProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs) :
        GLGpuProgram(rid, gid, ls, rs)
    {
        mProgramID = glGenLists(1);
    }
    //-----------------------------------------------------------------------
    GLGpuNvparseProgram::~GLGpuNvparseProgram()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();
    }
    //-----------------------------------------------------------------------
    void GLGpuNvparseProgram::bindProgram() const
    {
         glCallList(mProgramID);
         glEnable(GL_TEXTURE_SHADER_NV);
         glEnable(GL_REGISTER_COMBINERS_NV);
         glEnable(GL_PER_STAGE_CONSTANTS_NV);
    }
    //-----------------------------------------------------------------------
    void GLGpuNvparseProgram::unbindProgram() const
    {
        glDisable(GL_TEXTURE_SHADER_NV);
        glDisable(GL_REGISTER_COMBINERS_NV);
        glDisable(GL_PER_STAGE_CONSTANTS_NV);
    }
    //-----------------------------------------------------------------------
    void GLGpuNvparseProgram::bindProgramParameters(const GpuProgramParam * params, Nui16 mask) const
    {
        // NB, register combiners uses 2 constants per texture stage (0 and 1)
        // We have stored these as (stage * 2) + const_index in the physical buffer
        // There are no other parameters in a register combiners shader
        const GBufferArray & floatList = params->getBufferData();
        NCount index = 0;
        GBufferArray::const_iterator i, iend = floatList.end();
        for(i = floatList.begin(); i != iend; ++i, ++index)
        {
            GLenum combinerStage = GL_COMBINER0_NV + (Nui32)(index / 2);
            GLenum pname = GL_CONSTANT_COLOR0_NV + (index % 2);
            glCombinerStageParameterfvNV(combinerStage, pname, &(*i));
        }
    }
    //-----------------------------------------------------------------------
    void GLGpuNvparseProgram::unloadImpl(void)
    {
        glDeleteLists(mProgramID, 1);
    }
    //-----------------------------------------------------------------------
    void GLGpuNvparseProgram::loadCodeImpl(const VString & code)
    {
        glNewList(mProgramID, GL_COMPILE);

        VString::size_type pos = mSource.find("!!");

        while(pos != String::npos)
        {
            VString::size_type newPos = mSource.find("!!", pos + 1);

            VString script = mSource.substr(pos, newPos - pos);
            nvparse(script.c_str(), 0);

            for(NIIb * const * errors = nvparse_get_errors(); *errors; errors++)
            {
                LogManager::getOnly().log(_I18n("Warning: nvparse reported the following errors:"));
                String tempstr;
#if NII_STRING
                DataStream::s2ws(*errors, tempstr);
#else
                tempstr = *errors;
#endif
                LogManager::getOnly().log(_T("\t") + tempstr);
            }

            pos = newPos;
        }
        glEndList();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ATI_FS_GLGpuProgram
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	ATI_FS_GLGpuProgram::ATI_FS_GLGpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs) :
		GLGpuProgram(rid, gid, ls, rs)
	{
		mProgramType = GL_FRAGMENT_SHADER_ATI;
		mProgramID = glGenFragmentShadersATI(1);
	}
	//---------------------------------------------------------------------
	ATI_FS_GLGpuProgram::~ATI_FS_GLGpuProgram()
	{
		// have to call this here reather than in Resource destructor
		// since calling virtual methods in base destructors causes crash
		unload();
	}
	//---------------------------------------------------------------------
	void ATI_FS_GLGpuProgram::bindProgram() const
	{
		glEnable(mProgramType);
		glBindFragmentShaderATI(mProgramID);
	}
	//---------------------------------------------------------------------
	void ATI_FS_GLGpuProgram::unbindProgram() const
	{
		glDisable(mProgramType);
	}
	//---------------------------------------------------------------------
	void ATI_FS_GLGpuProgram::bindProgramParameters(const GpuProgramParam * params, Nui16 mask) const
	{
		// only supports float constants
		GpuParamBlockMap::const_iterator i, iend = params->getBlockList().end();
		for(i = params->getBlockList().begin(); i != iend; ++i)
		{
			if(i->second.mGBTMark & mask)
			{
				size_t logicalIndex = i->first;
				const float * pFloat = (NIIf *)params->getBufferData(i->second.mMemIndex);
				// Iterate over the params, set in 4-float chunks (low-level)
				for(size_t j = 0; j < i->second.m32bSize; j+=4)
				{
					glSetFragmentShaderConstantATI(GL_CON_0_ATI + logicalIndex, pFloat);
					pFloat += 4;
					++logicalIndex;
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void ATI_FS_GLGpuProgram::bindProgramPassIterationParameters(const GpuProgramParam * params) const
	{
        const float * pFloat = params->getRenderCount();
		if(pFloat != 0)
		{
            GLuint index = params->getLocation(pFloat - &params->getBufferData()[0]);
			glSetFragmentShaderConstantATI( GL_CON_0_ATI + index, pFloat);
		}
	}
	//---------------------------------------------------------------------
	void ATI_FS_GLGpuProgram::unloadImpl()
	{
		glDeleteFragmentShaderATI(mProgramID);
	}
	//---------------------------------------------------------------------
	void ATI_FS_GLGpuProgram::loadCodeImpl(const VString & code)
	{
		PS_1_4 PS1_4Assembler;
		// attempt to compile the source
	#ifdef _DEBUG
		PS1_4Assembler.test(); // run compiler tests in debug mode
	#endif
		bool Error = !PS1_4Assembler.compile(mSource.c_str());
		if(!Error) 
		{
			glBindFragmentShaderATI(mProgramID);
			glBeginFragmentShaderATI();
			// compile was successful so send the machine instructions thru GL to GPU
			Error = !PS1_4Assembler.bindAllMachineInstToFragmentShader();
			glEndFragmentShaderATI();

			// check GL for GPU machine instruction bind erros
			if (Error)
			{
				N_EXCEPT(Internal, _T("Cannot Bind ATI fragment shader :") + mFile);
			}
		}
		else
		{
			// an error occurred when compiling the ps_1_4 source code
			Ntchar buff[50];
			Nsntprintf(buff, sizeof(50), _T("error on line %d in pixel shader source\n"), PS1_4Assembler.mCurrentLine);

			LogManager::getOnly().log(_T("Warning: atifs compiler reported the following errors:"));
			LogManager::getOnly().log(buff + mFile);

			N_EXCEPT(Internal, _T("Cannot Compile ATI fragment shader : ") + mFile + _T("\n\n") + buff);// +
		}
	}
	//---------------------------------------------------------------------
}
