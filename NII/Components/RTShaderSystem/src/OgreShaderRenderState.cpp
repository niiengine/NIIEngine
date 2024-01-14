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
#include "OgreShaderPrerequisites.h"
#include "OgreShaderRenderState.h"
#include "OgreShaderGenerator.h"
#include "OgreLogManager.h"
#include "OgreShaderProgram.h"
#include "OgreShaderProgramSet.h"
#include "OgreStringConverter.h"
#include "OgreShaderProgramManager.h"
#include "OgreShaderFFPRenderState.h"


namespace Ogre {
namespace RTShader {


//-----------------------------------------------------------------------
RenderState::RenderState()
{
	mLightCountAutoUpdate	 = true;	
	mLightCount[0]			 = 0;
	mLightCount[1]			 = 0;
	mLightCount[2]			 = 0;	
}

//-----------------------------------------------------------------------
RenderState::~RenderState()
{
	reset();
}

//-----------------------------------------------------------------------
void RenderState::reset()
{
	for (SubRenderStateListIterator it=mSubRenderStateList.begin(); it != mSubRenderStateList.end(); ++it)
	{
		ShaderGenerator::getSingleton().destroySubRenderState(*it);
	}
	mSubRenderStateList.clear();
}

//-----------------------------------------------------------------------
void RenderState::setLightCount(const int lightCount[3])
{
	mLightCount[0] = lightCount[0];
	mLightCount[1] = lightCount[1];
	mLightCount[2] = lightCount[2];
}

//-----------------------------------------------------------------------
void RenderState::getLightCount(int lightCount[3]) const
{
	lightCount[0] = mLightCount[0];
	lightCount[1] = mLightCount[1];
	lightCount[2] = mLightCount[2];
}

//-----------------------------------------------------------------------
void RenderState::addTemplateSubRenderState(SubRenderState* subRenderState)
{
	bool addSubRenderState = true;

	// Go over the current sub render state.
	for (SubRenderStateListIterator it=mSubRenderStateList.begin(); it != mSubRenderStateList.end(); ++it)
	{
		// Case the same instance already exists -> do not add to list.
		if (*it == subRenderState)
		{
			addSubRenderState = false;
			break;
		}

		// Case it is different sub render state instance with the same type, use the new sub render state
		// instead of the previous sub render state. This scenario is usually caused by material inheritance, so we use the derived material sub render state
		// and destroy the base sub render state.
		else if ((*it)->getType() == subRenderState->getType())
		{
			removeTemplateSubRenderState(*it);
			break;
		}
	}

	// Add only if this sub render state instance is not on the list.
	if (addSubRenderState)
	{
		mSubRenderStateList.push_back(subRenderState);	
	}	
}

//-----------------------------------------------------------------------
void RenderState::removeTemplateSubRenderState(SubRenderState* subRenderState)
{
	for (SubRenderStateListIterator it=mSubRenderStateList.begin(); it != mSubRenderStateList.end(); ++it)
	{
		if ((*it) == subRenderState)
		{
			ShaderGenerator::getSingleton().destroySubRenderState(*it);
			mSubRenderStateList.erase(it);
			break;
		}		
	}
}
//-----------------------------------------------------------------------
TargetRenderState::TargetRenderState()
{	
	mProgramSet				 = NULL;
	mSubRenderStateSortValid = false;	
}
//-----------------------------------------------------------------------
TargetRenderState::~TargetRenderState()
{
	destroyProgramSet();
}
//-----------------------------------------------------------------------
void TargetRenderState::addSubRenderStateInstance(SubRenderState* subRenderState)
{
	mSubRenderStateList.push_back(subRenderState);
	mSubRenderStateSortValid = false;
}
//-----------------------------------------------------------------------
void TargetRenderState::removeSubRenderStateInstance(SubRenderState* subRenderState)
{
	for (SubRenderStateListIterator it=mSubRenderStateList.begin(); it != mSubRenderStateList.end(); ++it)
	{
		if ((*it) == subRenderState)
		{
			ShaderGenerator::getSingleton().destroySubRenderState(*it);
			mSubRenderStateList.erase(it);
			break;
		}		
	}
}
//-----------------------------------------------------------------------
bool TargetRenderState::createCpuPrograms()
{
	sortSubRenderStates();

	ProgramSet* programSet = createProgramSet();
	Program* vsProgram = ProgramManager::getSingleton().createCpuProgram(GPT_VERTEX_PROGRAM);
	Program* psProgram = ProgramManager::getSingleton().createCpuProgram(GPT_FRAGMENT_PROGRAM);
	RTShader::Function* vsMainFunc = NULL;
	RTShader::Function* psMainFunc = NULL;

	programSet->setCpuVertexProgram(vsProgram);
	programSet->setCpuFragmentProgram(psProgram);

	// Create entry point functions.
	vsMainFunc = vsProgram->createFunction("main", "Vertex Program Entry point", Function::FFT_VS_MAIN);
	vsProgram->setEntryPointFunction(vsMainFunc);

	psMainFunc = psProgram->createFunction("main", "Pixel Program Entry point", Function::FFT_PS_MAIN);
	psProgram->setEntryPointFunction(psMainFunc);

	for (SubRenderStateListIterator it=mSubRenderStateList.begin(); it != mSubRenderStateList.end(); ++it)
	{
		SubRenderState* srcSubRenderState = *it;

		if (false == srcSubRenderState->createCpuSubPrograms(programSet))
		{
			LogManager::getSingleton().stream()	<< "RTShader::TargetRenderState : Could not generate sub render program of type: " << srcSubRenderState->getType();
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------
ProgramSet*	TargetRenderState::createProgramSet()
{
	destroyProgramSet();

	mProgramSet = N_new ProgramSet;

	return mProgramSet;
}
//-----------------------------------------------------------------------
void TargetRenderState::destroyProgramSet()
{
	if (mProgramSet != NULL)
	{
		N_delete mProgramSet;
		mProgramSet = NULL;
	}
}
//-----------------------------------------------------------------------
void TargetRenderState::updateGpuProgramsParams(GeometryObj * rend, ShaderCh * pass,
    const SysSyncParam * source, const LightList * pLightList)
{
	for(SubRenderStateListIterator it=mSubRenderStateList.begin(); it != mSubRenderStateList.end(); ++it)
	{
		SubRenderState * curSubRenderState = *it;

		curSubRenderState->updateGpuProgramsParams(rend, pass, source, pLightList);		
	}
}
//-----------------------------------------------------------------------
void TargetRenderState::link(const RenderState& rhs, ShaderCh * srcPass, ShaderCh* dstPass)
{	
	SubRenderStateList customSubRenderStates;

	// Sort current render states.
	sortSubRenderStates();

	// Insert all custom sub render states. (I.E Not FFP sub render states).
	const SubRenderStateList& subRenderStateList = rhs.getTemplateSubRenderStateList();

	for (SubRenderStateListConstIterator itSrc=subRenderStateList.begin(); itSrc != subRenderStateList.end(); ++itSrc)
	{
		const SubRenderState* srcSubRenderState = *itSrc;
		bool isCustomSubRenderState = true;

		if (srcSubRenderState->getExecutionOrder() == FFP_TRANSFORM ||
			srcSubRenderState->getExecutionOrder() == FFP_COLOUR ||
			srcSubRenderState->getExecutionOrder() == FFP_LIGHTING ||
			srcSubRenderState->getExecutionOrder() == FFP_TEXTURING ||
			srcSubRenderState->getExecutionOrder() == FFP_FOG)
		{
			isCustomSubRenderState = false;
		}		


		// Case it is a custom sub render state.
		if (isCustomSubRenderState)
		{
			bool subStateTypeExists = false;

			// Check if this type of sub render state already exists.
			for (SubRenderStateListConstIterator itDst=mSubRenderStateList.begin(); itDst != mSubRenderStateList.end(); ++itDst)
			{
				if ((*itDst)->getType() == srcSubRenderState->getType())
				{
					subStateTypeExists = true;
					break;
				}				
			}

			// Case custom sub render state not exits -> add it to custom list.
			if (subStateTypeExists == false)
			{
				SubRenderState* newSubRenderState = NULL;

				newSubRenderState = ShaderGenerator::getSingleton().createSubRenderState(srcSubRenderState->getType());
				*newSubRenderState = *srcSubRenderState;
				customSubRenderStates.push_back(newSubRenderState);
			}						
		}						
	}	

	// Merge the local custom sub render states.
	for (SubRenderStateListIterator itSrc=customSubRenderStates.begin(); itSrc != customSubRenderStates.end(); ++itSrc)
	{
		SubRenderState* customSubRenderState = *itSrc;

		if (customSubRenderState->preAddToRenderState(this, srcPass, dstPass))
		{
			addSubRenderStateInstance(customSubRenderState);
		}
		else
		{
			ShaderGenerator::getSingleton().destroySubRenderState(customSubRenderState);
		}		
	}	
}

//-----------------------------------------------------------------------
void TargetRenderState::sortSubRenderStates()
{
	if (mSubRenderStateSortValid == false)
	{
		if (mSubRenderStateList.size() > 1)
			qsort(&mSubRenderStateList[0], mSubRenderStateList.size(), sizeof(SubRenderState*), sSubRenderStateCompare);		

		mSubRenderStateSortValid = true;
	}
}

//-----------------------------------------------------------------------
int	TargetRenderState::sSubRenderStateCompare(const void * p0, const void *p1)
{
	SubRenderState* pInstance0 = *((SubRenderState**)p0);
	SubRenderState* pInstance1 = *((SubRenderState**)p1);

	return pInstance0->getExecutionOrder() - pInstance1->getExecutionOrder();	
}

}
}

