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

#ifndef __GLSLShaderFactory_H__
#define __GLSLShaderFactory_H__

#include "NiiGLSLExtSupport.h"
#include "NiiGpuProgramManager.h"

namespace NII
{
    /** Factory class for GLSL programs. */
    class _EngineGLAPI GLSLShaderFactory : public HighLevelGpuProgramFactory
    {
    public:
        GLSLShaderFactory(GLRenderSystem* renderSystem);
        ~GLSLShaderFactory();
        
        /// @copydetails FactoryObj::getID
        FactoryID getID() const;
		
        /// @copydetails HighLevelGpuProgramFactory::getLanguage
		ShaderLanguage getLanguage() const;
        
		/// @copydetails HighLevelGpuProgramFactory::create
        HighLevelGpuProgram * create(ResourceID rid, GroupID gid);
       
        /// @copydetails FactoryObj::destroy
		void destroy(void * obj);
	private:
        GLSL::GLSLProgramManager * mLinkProgramManager;
    };
}

#endif
