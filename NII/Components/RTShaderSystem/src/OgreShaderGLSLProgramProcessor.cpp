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

#include "OgreShaderGLSLProgramProcessor.h"
#include "OgreShaderProgramSet.h"
#include "OgreShaderProgram.h"
#include "OgreLogManager.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreHighLevelGpuProgram.h"

namespace Ogre {
namespace RTShader {

String GLSLProgramProcessor::TargetLanguage = "glsl";

//-----------------------------------------------------------------------------
GLSLProgramProcessor::GLSLProgramProcessor()
{

}

//-----------------------------------------------------------------------------
GLSLProgramProcessor::~GLSLProgramProcessor()
{
	StringList::iterator it = mLibraryPrograms.begin();
	StringList::iterator itEnd = mLibraryPrograms.end();
	
	for (; it != itEnd; ++it)
	{
		HighLevelGpuProgramManager::getSingleton().remove(*it);
	}
	mLibraryPrograms.clear();
}

//-----------------------------------------------------------------------------
bool GLSLProgramProcessor::preCreateGpuPrograms(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* fsProgram = programSet->getCpuFragmentProgram();
	Function* vsMain   = vsProgram->getEntryPointFunction();
	Function* fsMain   = fsProgram->getEntryPointFunction();	
	bool success;

	// Compact vertex shader outputs.
	success = ProgramProcessor::compactVsOutputs(vsMain, fsMain);
	if (success == false)	
		return false;	

	return true;
}

//-----------------------------------------------------------------------------
bool GLSLProgramProcessor::postCreateGpuPrograms(ProgramSet* programSet)
{
	Program* vsCpuProgram = programSet->getCpuVertexProgram();
	Program* fsCpuProgram = programSet->getCpuFragmentProgram();
	GpuProgram * vsGpuProgram = programSet->getGpuVertexProgram();
	GpuProgram * fsGpuProgram = programSet->getGpuFragmentProgram();
	
	// Bind sub shaders for the vertex shader.
	bindSubShaders(vsCpuProgram, vsGpuProgram);
	
	// Bind sub shaders for the fragment shader.
	bindSubShaders(fsCpuProgram, fsGpuProgram);

	// Bind vertex shader auto parameters.
	bindAutoParameters(programSet->getCpuVertexProgram(), programSet->getGpuVertexProgram());

	// Bind fragment shader auto parameters.
	bindAutoParameters(programSet->getCpuFragmentProgram(), programSet->getGpuFragmentProgram());

	// Bind texture samplers for the vertex shader.
	bindTextureSamplers(vsCpuProgram, vsGpuProgram);

	// Bind texture samplers for the fragment shader.
	bindTextureSamplers(fsCpuProgram, fsGpuProgram);


	return true;
}

//-----------------------------------------------------------------------------
void GLSLProgramProcessor::bindSubShaders(Program* program, GpuProgram * pGpuProgram)
{
	if (program->getDependencyCount() > 0)
	{
		// Get all attached shaders so we do not attach shaders twice.
		// maybe GLSLShader should take care of that ( prevent add duplicate shaders )
		String attachedShaders;
        pGpuProgram->get("attach", attachedShaders);
		String subShaderDef = "";

		for (unsigned int i=0; i < program->getDependencyCount(); ++i)
		{
			// Here we append _VS and _FS to the library shaders (so max each lib shader
			// is compiled twice once as vertex and once as fragment shader)
			String subShaderName = program->getDependency(i);
			if (program->getType() == GPT_VERTEX_PROGRAM)
			{
				subShaderName += "_VS";
			}
			else
			{
				subShaderName += "_FS";
			}					

			// Check if the library shader already compiled
			if(!HighLevelGpuProgramManager::getSingleton().isExist(subShaderName))
			{
				// Create the library shader
				HighLevelGpuProgramPtr pSubGpuProgram = HighLevelGpuProgramManager::getSingleton().createProgram(subShaderName,
					GroupDefault, TargetLanguage, program->getType());

				// Set the source name
				String sourceName = program->getDependency(i) + "." + TargetLanguage;
				pSubGpuProgram->setProgramSrc(sourceName);

				// If we have compile errors than stop processing
				if(pSubGpuProgram->getErrorCode() != 0)
				{
					N_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
						"Could not compile shader library from the source file: " + sourceName);	
				}

				mLibraryPrograms.push_back(subShaderName);
			}

			// Check if the lib shader already attached to this shader
			if (attachedShaders.find(subShaderName) == String::npos)
			{
				// Append the shader name to subShaders
				subShaderDef += subShaderName + " ";
			}
		}

		// Check if we have something to attach
		if (subShaderDef.length() > 0)
		{
			pGpuProgram->set("attach", subShaderDef);
		}
	}
}
//-----------------------------------------------------------------------------
void GLSLProgramProcessor::bindTextureSamplers(Program * pCpuProgram, GpuProgram * pGpuProgram)
{
	GpuProgramParam * pGpuParams = pGpuProgram->createDefaultParam();
	const UniformParameterList & progParams = pCpuProgram->getParameters();
	UniformParameterConstIterator itParams;

	// Bind the samplers.
	for(itParams=progParams.begin(); itParams != progParams.end(); ++itParams)
	{
		const UniformParameterPtr pCurParam = *itParams;
		
		if(pCurParam->isSampler())
		{		
			pGpuParams->set(pCurParam->getName(), pCurParam->getIndex());						
		}		
	}
}

}
}
