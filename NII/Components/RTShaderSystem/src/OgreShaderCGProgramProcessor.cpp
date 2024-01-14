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

#include "OgreShaderCGProgramProcessor.h"
#include "OgreShaderProgramSet.h"
#include "OgreShaderProgram.h"

namespace Ogre {
namespace RTShader {

String CGProgramProcessor::TargetLanguage = "cg";

//-----------------------------------------------------------------------------
CGProgramProcessor::CGProgramProcessor()
{
	
}

//-----------------------------------------------------------------------------
CGProgramProcessor::~CGProgramProcessor()
{
	
}

//-----------------------------------------------------------------------------
bool CGProgramProcessor::preCreateGpuPrograms( ProgramSet* programSet )
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain   = vsProgram->getEntryPointFunction();
	Function* fsMain   = psProgram->getEntryPointFunction();	
	bool success;

	// Compact vertex shader outputs.
	success = ProgramProcessor::compactVsOutputs(vsMain, fsMain);
	if (success == false)	
		return false;	

	return true;
}

//-----------------------------------------------------------------------------
bool CGProgramProcessor::postCreateGpuPrograms( ProgramSet* programSet )
{
	// Bind vertex shader auto parameters.
	bindAutoParameters(programSet->getCpuVertexProgram(), programSet->getGpuVertexProgram());

	// Bind fragment shader auto parameters.
	bindAutoParameters(programSet->getCpuFragmentProgram(), programSet->getGpuFragmentProgram());

	return true;
}
}
}
