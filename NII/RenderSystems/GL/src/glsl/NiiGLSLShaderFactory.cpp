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

#include "NiiGLSLShaderFactory.h"
#include "NiiGLSLLinkProgramManager.h"
#include "NiiGLSLShader.h"

namespace NII
{
    //-----------------------------------------------------------------------
	GLSLShaderFactory::GLSLShaderFactory() :
		HighLevelGpuProgramFactory(0)
    {
		mLinkProgramManager = N_new GLSL::GLSLProgramManager();
    }
    //-----------------------------------------------------------------------
    GLSLShaderFactory::~GLSLShaderFactory()
    {
		if (mLinkProgramManager)
		{
			N_delete mLinkProgramManager;
		}
    }
    //-----------------------------------------------------------------------
    FactoryID GLSLShaderFactory::getID() const
    {
        return N_FACTORY_GLSLProgram;
    }
    //-----------------------------------------------------------------------
    ShaderLanguage GLSLShaderFactory::getLanguage() const
    {
        return SL_GLSL;
    }
    //-----------------------------------------------------------------------
    HighLevelGpuProgram * GLSLShaderFactory::create(ResourceID rid, GroupID gid)
    {
        return N_new GLSL::GLSLShader(rid, gid, 0, 0);
    }
    //-----------------------------------------------------------------------
	void GLSLShaderFactory::destroy(void * prog)
    {
        GLSL::GLSLShader * temp = static_cast<GLSL::GLSLShader *>(prog);
        N_delete temp;
    }
    //-----------------------------------------------------------------------
}