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

#include "OgreShaderGLSLESProgramProcessor.h"
#include "OgreShaderProgramSet.h"
#include "OgreShaderProgram.h"
#include "OgreHighLevelGpuProgramManager.h"

namespace Ogre 
{
namespace RTShader 
{

    String GLSLESProgramProcessor::TargetLanguage = "glsles";
    //-----------------------------------------------------------------------------
    GLSLESProgramProcessor::GLSLESProgramProcessor()
    {
    }
    //-----------------------------------------------------------------------------
    GLSLESProgramProcessor::~GLSLESProgramProcessor()
    {
        StringList::iterator it = mLibraryPrograms.begin();
        StringList::iterator itEnd = mLibraryPrograms.end();
        
        for(; it != itEnd; ++it)
        {
            HighLevelGpuProgramManager::getSingleton().remove(*it);
        }
        mLibraryPrograms.clear();
    }
    //-----------------------------------------------------------------------------
    bool GLSLESProgramProcessor::preCreateGpuPrograms(ProgramSet* programSet)
    {
        Program * vsProgram = programSet->getCpuVertexProgram();
        Program * fsProgram = programSet->getCpuFragmentProgram();
        Function * vsMain   = vsProgram->getEntryPointFunction();
        Function * fsMain   = fsProgram->getEntryPointFunction();	
        bool success;

        // Compact vertex shader outputs.
        success = ProgramProcessor::compactVsOutputs(vsMain, fsMain);
        if(success == false)	
            return false;	

        return true;
    }
    //-----------------------------------------------------------------------------
    bool GLSLESProgramProcessor::postCreateGpuPrograms(ProgramSet* programSet)
    {
        Program * vsCpuProgram = programSet->getCpuVertexProgram();
        Program * fsCpuProgram = programSet->getCpuFragmentProgram();
        GpuProgram * vsGpuProgram = programSet->getGpuVertexProgram();
        GpuProgram * fsGpuProgram = programSet->getGpuFragmentProgram();

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
    void GLSLESProgramProcessor::bindTextureSamplers(Program * pCpuProgram, GpuProgram * pGpuProgram)
    {
        GpuProgramParam * pGpuParams = pGpuProgram->createDefaultParam();
        const UniformParameterList & progParams = pCpuProgram->getParameters();
        UniformParameterConstIterator itParams;

        // Bind the samplers.
        for(itParams = progParams.begin(); itParams != progParams.end(); ++itParams)
        {
            const UniformParameterPtr pCurParam = *itParams;
            
            if(pCurParam->isSampler())
            {
                // The optimizer may remove some unnecessary parameters, so we should ignore them
                pGpuParams->setAllowMissing(true);

                pGpuParams->set(pCurParam->getName(), pCurParam->getIndex());
            }
        }
    }
    //-----------------------------------------------------------------------------
}
}