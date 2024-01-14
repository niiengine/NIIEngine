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

#include "OgreShaderProgramManager.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreConfigFile.h"
#include "OgreShaderRenderState.h"
#include "OgreShaderProgramSet.h"
#include "OgreShaderGenerator.h"
#include "OgrePass.h"
#include "OgreLogManager.h"
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
#include "OgreShaderCGProgramWriter.h"
#include "OgreShaderHLSLProgramWriter.h"
#include "OgreShaderGLSLProgramWriter.h"
#endif
#include "OgreShaderGLSLESProgramWriter.h"
#include "OgreShaderProgramProcessor.h"
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
#include "OgreShaderCGProgramProcessor.h"
#include "OgreShaderHLSLProgramProcessor.h"
#include "OgreShaderGLSLProgramProcessor.h"
#endif
#include "OgreShaderGLSLESProgramProcessor.h"
#include "OgreGpuProgramManager.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "OgreStringSerialiser.h"
#endif

namespace Ogre {

//-----------------------------------------------------------------------
template<> 
RTShader::ProgramManager* Singleton<RTShader::ProgramManager>::msSingleton = 0;

namespace RTShader {


//-----------------------------------------------------------------------
ProgramManager* ProgramManager::getSingletonPtr()
{
	assert( msSingleton );  
	return msSingleton;
}

//-----------------------------------------------------------------------
ProgramManager& ProgramManager::getSingleton()
{
	assert( msSingleton );  
	return ( *msSingleton );
}

//-----------------------------------------------------------------------------
ProgramManager::ProgramManager()
{
	createDefaultProgramProcessors();
	createDefaultProgramWriterFactories();
}

//-----------------------------------------------------------------------------
ProgramManager::~ProgramManager()
{
	flushGpuProgramsCache();
	destroyDefaultProgramWriterFactories();
	destroyDefaultProgramProcessors();	
	destroyProgramWriters();
}

//-----------------------------------------------------------------------------
void ProgramManager::acquirePrograms(ShaderCh * pass, TargetRenderState * renderState)
{
	// Create the CPU programs.
	if(false == renderState->createCpuPrograms())
	{
		N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Could not apply render state ");	
	}	

	ProgramSet * programSet = renderState->getProgramSet();

	// Create the GPU programs.
	if(false == createGpuPrograms(programSet))
	{
		N_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"Could not create gpu programs from render state ");
	}	

	// Bind the created GPU programs to the target pass.
    pass->getProgram().set(ST_VS, 0, programSet->getGpuVertexProgram()->getOriginID());
	pass->getProgram().set(ST_FS, 0, programSet->getGpuFragmentProgram()->getOriginID());

	// Bind uniform parameters to pass parameters.
    GpuProgramParam * temp;
    pass->getProgram().get(0, ST_VS, NULL, temp);
	bindUniformParameters(programSet->getCpuVertexProgram(), temp);
    pass->getProgram().get(0, ST_FS, NULL, temp);
	bindUniformParameters(programSet->getCpuFragmentProgram(), temp);
}
//-----------------------------------------------------------------------------
void ProgramManager::releasePrograms(ShaderCh * pass, TargetRenderState * renderState)
{
	ProgramSet * programSet = renderState->getProgramSet();

    pass->getProgram().set(ST_VS, 0, 0);
	pass->getProgram().set(ST_FS, 0, 0);

	GpuProgramsMapIterator itVsGpuProgram = mVertexShaderMap.find(programSet->getGpuVertexProgram()->getName());
	GpuProgramsMapIterator itFsGpuProgram = mFragmentShaderMap.find(programSet->getGpuFragmentProgram()->getName());

	renderState->destroyProgramSet();

	if (itVsGpuProgram != mVertexShaderMap.end())
	{
		if (itVsGpuProgram->second.useCount() == 3 + 1)
		{
			destroyGpuProgram(itVsGpuProgram->second);
			mVertexShaderMap.erase(itVsGpuProgram);
		}
	}

	if (itFsGpuProgram != mFragmentShaderMap.end())
	{
		if (itFsGpuProgram->second.useCount() == 3 + 1)
		{
			destroyGpuProgram(itFsGpuProgram->second);
			mFragmentShaderMap.erase(itFsGpuProgram);
		}
	}
}
//-----------------------------------------------------------------------------
void ProgramManager::flushGpuProgramsCache()
{
	flushGpuProgramsCache(mVertexShaderMap);
	flushGpuProgramsCache(mFragmentShaderMap);
}

//-----------------------------------------------------------------------------
void ProgramManager::flushGpuProgramsCache(GpuProgramsMap& gpuProgramsMap)
{
	while (gpuProgramsMap.size() > 0)
	{
		GpuProgramsMapIterator it = gpuProgramsMap.begin();

		destroyGpuProgram(it->second);
		gpuProgramsMap.erase(it);
	}
}

//-----------------------------------------------------------------------------
void ProgramManager::createDefaultProgramWriterFactories()
{
	// Add standard shader writer factories 
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
	mProgramWriterFactories.push_back(N_new ShaderProgramWriterCGFactory());
	mProgramWriterFactories.push_back(N_new ShaderProgramWriterGLSLFactory());
	mProgramWriterFactories.push_back(N_new ShaderProgramWriterHLSLFactory());
#endif
	mProgramWriterFactories.push_back(N_new ShaderProgramWriterGLSLESFactory());
	
	for (unsigned int i=0; i < mProgramWriterFactories.size(); ++i)
	{
		ProgramWriterManager::getSingletonPtr()->add(mProgramWriterFactories[i]);
	}
}

//-----------------------------------------------------------------------------
void ProgramManager::destroyDefaultProgramWriterFactories()
{ 
	for (unsigned int i=0; i<mProgramWriterFactories.size(); i++)
	{
		ProgramWriterManager::getSingletonPtr()->remove(mProgramWriterFactories[i]);
		N_delete mProgramWriterFactories[i];
	}
	mProgramWriterFactories.clear();
}

//-----------------------------------------------------------------------------
void ProgramManager::createDefaultProgramProcessors()
{
	// Add standard shader processors
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
	mDefaultProgramProcessors.push_back(N_new CGProgramProcessor);
	mDefaultProgramProcessors.push_back(N_new GLSLProgramProcessor);
	mDefaultProgramProcessors.push_back(N_new HLSLProgramProcessor);
#endif
	mDefaultProgramProcessors.push_back(N_new GLSLESProgramProcessor);

	for (unsigned int i=0; i < mDefaultProgramProcessors.size(); ++i)
	{
		addProgramProcessor(mDefaultProgramProcessors[i]);
	}
}

//-----------------------------------------------------------------------------
void ProgramManager::destroyDefaultProgramProcessors()
{
	for (unsigned int i=0; i < mDefaultProgramProcessors.size(); ++i)
	{
		removeProgramProcessor(mDefaultProgramProcessors[i]);
		N_delete mDefaultProgramProcessors[i];
	}
	mDefaultProgramProcessors.clear();
}

//-----------------------------------------------------------------------------
void ProgramManager::destroyProgramWriters()
{
	ProgramWriterIterator it    = mProgramWritersMap.begin();
	ProgramWriterIterator itEnd = mProgramWritersMap.end();

	for (; it != itEnd; ++it)
	{
		if (it->second != NULL)
		{
			N_delete it->second;
			it->second = NULL;
		}					
	}
	mProgramWritersMap.clear();
}

//-----------------------------------------------------------------------------
Program* ProgramManager::createCpuProgram(GpuProgramType type)
{
	Program* shaderProgram = N_new Program(type);

	mCpuProgramsList.insert(shaderProgram);

	return shaderProgram;
}


//-----------------------------------------------------------------------------
void ProgramManager::destroyCpuProgram(Program* shaderProgram)
{
	ProgramListIterator it = mCpuProgramsList.find(shaderProgram);
	
	if(it != mCpuProgramsList.end())
	{			
		N_delete *it;			
		mCpuProgramsList.erase(it);	
	}			
}

//-----------------------------------------------------------------------------
bool ProgramManager::createGpuPrograms(ProgramSet* programSet)
{
	// Before we start we need to make sure that the pixel shader input
	//  parameters are the same as the vertex output, this required by 
	//  shader models 4 and 5.
	// This change may incrase the number of register used in older shader
	//  models - this is why the check is present here.
	bool isVs4 = GpuProgramManager::getSingleton().isSyntaxSupported("vs_4_0");
	if (isVs4)
	{
		synchronizePixelnToBeVertexOut(programSet);
	}

	// Grab the matching writer.
	const String& language = ShaderGenerator::getSingleton().getTargetLanguage();
	ProgramWriterIterator itWriter = mProgramWritersMap.find(language);
	ProgramWriter* programWriter = NULL;

	// No writer found -> create new one.
	if (itWriter == mProgramWritersMap.end())
	{
		programWriter = ProgramWriterManager::getSingletonPtr()->createProgramWriter(language);
		mProgramWritersMap[language] = programWriter;
	}
	else
	{
		programWriter = itWriter->second;
	}

	ProgramProcessorIterator itProcessor = mProgramProcessorsMap.find(language);
	ProgramProcessor* programProcessor = NULL;

	if (itProcessor == mProgramProcessorsMap.end())
	{
		N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
			"Could not find processor for language '" + language);		
	}

	programProcessor = itProcessor->second;

	bool success;
	
	// Call the pre creation of GPU programs method.
	success = programProcessor->preCreateGpuPrograms(programSet);
	if (success == false)	
		return false;	
	
	// Create the vertex shader program.
	GpuProgram * vsGpuProgram;
	
	vsGpuProgram = createGpuProgram(programSet->getCpuVertexProgram(), 
		programWriter,
		language, 
		ShaderGenerator::getSingleton().getVertexShaderProfiles(),
		ShaderGenerator::getSingleton().getVertexShaderProfilesList(),
		ShaderGenerator::getSingleton().getShaderCachePath());

	if (vsGpuProgram.isNull())	
		return false;

	programSet->setGpuVertexProgram(vsGpuProgram);

	//update flags
	programSet->getGpuVertexProgram()->setVertexMatrixFusion(
		programSet->getCpuVertexProgram()->getSkeletalAnimationIncluded());
	// Create the fragment shader program.
	GpuProgram * psGpuProgram;

	psGpuProgram = createGpuProgram(programSet->getCpuFragmentProgram(), 
		programWriter,
		language, 
		ShaderGenerator::getSingleton().getFragmentShaderProfiles(),
		ShaderGenerator::getSingleton().getFragmentShaderProfilesList(),
		ShaderGenerator::getSingleton().getShaderCachePath());

	if (psGpuProgram.isNull())	
		return false;

	programSet->setGpuFragmentProgram(psGpuProgram);

	// Call the post creation of GPU programs method.
	success = programProcessor->postCreateGpuPrograms(programSet);
	if (success == false)	
		return false;	

	
	return true;
	
}


//-----------------------------------------------------------------------------
void ProgramManager::bindUniformParameters(Program* pCpuProgram, const GpuProgramParam * passParams)
{
	const UniformParameterList& progParams = pCpuProgram->getParameters();
	UniformParameterConstIterator itParams = progParams.begin();
	UniformParameterConstIterator itParamsEnd = progParams.end();

	// Bind each uniform parameter to its GPU parameter.
	for (; itParams != itParamsEnd; ++itParams)
	{			
		(*itParams)->bind(passParams);					
	}
}

//-----------------------------------------------------------------------------
GpuProgram * ProgramManager::createGpuProgram(Program* shaderProgram, 
											   ProgramWriter* programWriter,
											   const String& language,
											   const String& profiles,
											   const StringList& profilesList,
											   const String& cachePath)
{

	
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
	Ogre::StringSerialiser sourceCodeStringStream;
#else
	std::stringstream sourceCodeStringStream;
#endif
	_StringHash stringHash;
	uint32 programHashCode;
	String programName;

	// Generate source code.
	programWriter->writeSourceCode(sourceCodeStringStream, shaderProgram);

	// Generate program hash code.
	programHashCode = static_cast<uint32>(stringHash(sourceCodeStringStream.str()));

	// Generate program name.
	programName = StrConv::conv(programHashCode);
	
	if (shaderProgram->getType() == GPT_VERTEX_PROGRAM)
	{
		programName += "_VS";
	}
	else if (shaderProgram->getType() == GPT_FRAGMENT_PROGRAM)
	{
		programName += "_FS";
	}

	HighLevelGpuProgramPtr pGpuProgram;

	// Try to get program by name.
	pGpuProgram = HighLevelGpuProgramManager::getSingleton().getByName(programName);

	// Case the program doesn't exist yet.
	if(pGpuProgram.isNull())
	{
		// Create new GPU program.
		pGpuProgram = HighLevelGpuProgramManager::getSingleton().createProgram(programName,
			GroupDefault, language, shaderProgram->getType());

		// Case cache directory specified -> create program from file.
		if (cachePath.empty() == false)
		{
			const String  programFullName = programName + "." + language;
			const String  programFileName = cachePath + programFullName;	
			std::ifstream programFile;
			bool		  writeFile = true;


			// Check if program file already exist.
			programFile.open(programFileName.c_str());

			// Case no matching file found -> we have to write it.
			if (!programFile)
			{			
				writeFile = true;
			}
			else
			{
				writeFile = false;
				programFile.close();
			}

			// Case we have to write the program to a file.
			if (writeFile)
			{
				std::ofstream outFile(programFileName.c_str());

				if (!outFile)
					return 0;

				outFile << sourceCodeStringStream.str();
				outFile.close();
			}

			pGpuProgram->setProgramSrc(programFullName);
		}

		// No cache directory specified -> create program from system memory.
		else
		{
			pGpuProgram->setProgramCode(sourceCodeStringStream.str());
		}
		
		
		pGpuProgram->set("entry_point", shaderProgram->getEntryPointFunction()->getName());

		// HLSL program requires specific target profile settings - we have to split the profile string.
		if (language == "hlsl")
		{
			StringList::const_iterator it = profilesList.begin();
			StringList::const_iterator itEnd = profilesList.end();
			
			for (; it != itEnd; ++it)
			{
				if (GpuProgramManager::getSingleton().isSyntaxSupported(*it))
				{
					pGpuProgram->set("target", *it);
					break;
				}
			}
		}
		
		pGpuProgram->set("profiles", profiles);
		pGpuProgram->load();
	
		// Case an error occurred.
		if (pGpuProgram->getErrorCode() != 0)
		{
			pGpuProgram.setNull();
			return pGpuProgram;
		}

		// Add the created GPU program to local cache.
		if (pGpuProgram->getType() == GPT_VERTEX_PROGRAM)
		{
			mVertexShaderMap[programName] = pGpuProgram;			
		}
		else if (pGpuProgram->getType() == GPT_FRAGMENT_PROGRAM)
		{
			mFragmentShaderMap[programName] = pGpuProgram;	
		}				
	}
	
	return pGpuProgram;
}

//-----------------------------------------------------------------------------
void ProgramManager::addProgramProcessor(ProgramProcessor* processor)
{
	
	ProgramProcessorIterator itFind = mProgramProcessorsMap.find(processor->getTargetLanguage());

	if (itFind != mProgramProcessorsMap.end())
	{
		N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
			"A processor for language '" + processor->getTargetLanguage() + "' already exists.",
			"ProgramManager::addProgramProcessor");
	}		

	mProgramProcessorsMap[processor->getTargetLanguage()] = processor;
}

//-----------------------------------------------------------------------------
void ProgramManager::removeProgramProcessor(ProgramProcessor* processor)
{
	ProgramProcessorIterator itFind = mProgramProcessorsMap.find(processor->getTargetLanguage());

	if (itFind != mProgramProcessorsMap.end())
		mProgramProcessorsMap.erase(itFind);

}

//-----------------------------------------------------------------------------
void ProgramManager::destroyGpuProgram(GpuProgram * gpuProgram)
{		
	const String& programName = gpuProgram->getName();
	ResourcePtr res			  = HighLevelGpuProgramManager::getSingleton().getByName(programName);	

	if (res.isNull() == false)
	{		
		HighLevelGpuProgramManager::getSingleton().remove(programName);
	}
}

//-----------------------------------------------------------------------
void ProgramManager::synchronizePixelnToBeVertexOut( ProgramSet* programSet )
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	// first find the vertex shader
	ShaderFunctionConstIterator itFunction ;
	Function* vertexMain = NULL;
	Function* pixelMain = NULL;

	// find vertex shader main
	{
		const ShaderFunctionList& functionList = vsProgram->getFunctions();
		for (itFunction=functionList.begin(); itFunction != functionList.end(); ++itFunction)
		{
			Function* curFunction = *itFunction;
			if (curFunction->getFunctionType() == Function::FFT_VS_MAIN)
			{
				vertexMain = curFunction;
				break;
			}
		}
	}

	// find pixel shader main
	{
		const ShaderFunctionList & functionList = psProgram->getFunctions();
		for (itFunction=functionList.begin(); itFunction != functionList.end(); ++itFunction)
		{
			Function * curFunction = *itFunction;
			if (curFunction->getFunctionType() == Function::FFT_PS_MAIN)
			{
				pixelMain = curFunction;
				break;
			}
		}
	}

	// save the pixel program original input parameters
	const ShaderParameterList pixelOriginalInParams = pixelMain->getInputParameters();

	// set the pixel Input to be the same as the vertex prog output
	pixelMain->deleteAllInputParameters();

	// Loop the vertex shader output parameters and make sure that
	//   all of them exist in the pixel shader input.
	// If the parameter type exist in the original output - use it
	// If the parameter doesn't exist - use the parameter from the 
	//   vertex shader input.
	// The order will be based on the vertex shader parameters order 
	// Write output parameters.
	ShaderParameterConstIterator it;
	const ShaderParameterList & outParams = vertexMain->getOutputParameters();
	for (it=outParams.begin(); it != outParams.end(); ++it)
	{
		ParameterPtr curOutParemter = *it;
		ParameterPtr paramToAdd = Function::getParameterBySemantic(
			pixelOriginalInParams, 
			curOutParemter->getSemantic(), 
			curOutParemter->getIndex());

		if(paramToAdd.isNull())
		{
			// param not found - we will add the one from the vertex shader
			paramToAdd = curOutParemter; 
		}

		pixelMain->addInputParameter(paramToAdd);
	}
}

/** @} */
/** @} */
}
}
