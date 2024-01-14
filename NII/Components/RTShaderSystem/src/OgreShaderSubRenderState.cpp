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

#include "OgreShaderSubRenderState.h"
#include "OgreException.h"
#include "OgreShaderGenerator.h"

namespace Ogre {
namespace RTShader {



//-----------------------------------------------------------------------
SubRenderState::SubRenderState()
{
	
}

//-----------------------------------------------------------------------
SubRenderState::~SubRenderState()
{
	if (mOtherAccessor.isNull() == false)
	{
		mOtherAccessor->removeSubRenderStateInstance(this);
	}
}

//-----------------------------------------------------------------------
SubRenderStateFactory::~SubRenderStateFactory()
{
	assert(mSubRenderStateList.size() == 0 &&
		"SubRenderStateFactory::~SubRenderStateFactory -> Sub render states still exists !!!");
}

//-----------------------------------------------------------------------
SubRenderState*	SubRenderStateFactory::createInstance()
{
	SubRenderState*	subRenderState = createInstanceImpl();

	mSubRenderStateList.insert(subRenderState);

	return subRenderState;
}

//-----------------------------------------------------------------------
SubRenderState*	SubRenderStateFactory::createOrRetrieveInstance(SGScriptAnalyze* translator)
{
	//check if we already create a SRS 
	SubRenderState*	subRenderState = translator->getGeneratedSubRenderState(getType());
	if (subRenderState == NULL)
	{
		//create a new sub render state
		subRenderState = SubRenderStateFactory::createInstance();
	}
	return subRenderState;
}
//-----------------------------------------------------------------------
void SubRenderStateFactory::destroyInstance(SubRenderState * subRenderState)
{
	SubRenderStateSetIterator it = mSubRenderStateList.find(subRenderState);

	if(it != mSubRenderStateList.end())
	{
		N_delete *it;
		mSubRenderStateList.erase(it);
	}	
}
//-----------------------------------------------------------------------
void SubRenderStateFactory::destroyAllInstances()
{
	SubRenderStateSetIterator it;

	for (it = mSubRenderStateList.begin(); it != mSubRenderStateList.end(); ++it)
	{		
		N_delete *it;			
	}
	mSubRenderStateList.clear();
}

//-----------------------------------------------------------------------
SubRenderState & SubRenderState::operator=(const SubRenderState & rhs)
{
	if(getType() != rhs.getType())
	{
		N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can not copy sub render states of different types !!");
	}

	write(rhs);

	SubRenderStateAccessorPtr rhsAccessor = rhs.getAccessor();

	rhsAccessor->addSubRenderStateInstance(this);
	mOtherAccessor = rhsAccessor;

	return *this;
}

//-----------------------------------------------------------------------
bool SubRenderState::createCpuSubPrograms(ProgramSet* programSet)
{
	bool result;

	// Resolve parameters.
	result = resolveParameters(programSet);
	if (false == result)
		return false;

	// Resolve dependencies.
	result = resolveDependencies(programSet);
	if (false == result)
		return false;

	// Add function invocations.
	result = addFunctionInvocations(programSet);
	if (false == result)
		return false;

	return true;
}

//-----------------------------------------------------------------------
bool SubRenderState::resolveParameters(ProgramSet* programSet)
{
	return true;
}

//-----------------------------------------------------------------------
bool SubRenderState::resolveDependencies(ProgramSet* programSet)
{
	return true;
}

//-----------------------------------------------------------------------
bool SubRenderState::addFunctionInvocations( ProgramSet* programSet )
{
	return true;
}

//-----------------------------------------------------------------------
SubRenderStateAccessorPtr SubRenderState::getAccessor()
{
	if (mThisAccessor.isNull())
	{
		SubRenderStateAccessor* accessor = OGRE_NEW_T(SubRenderStateAccessor, MEMCATEGORY_GENERAL)(this);
		
		mThisAccessor.bind(accessor, SPFM_DELETE_T);
	}

	return mThisAccessor;
}

//-----------------------------------------------------------------------
SubRenderStateAccessorPtr SubRenderState::getAccessor() const
{
	if (mThisAccessor.isNull())
	{
		SubRenderStateAccessor* accessor = OGRE_NEW_T(SubRenderStateAccessor, MEMCATEGORY_GENERAL)(this);

		mThisAccessor.bind(accessor, SPFM_DELETE_T);
	}

	return mThisAccessor;
}



}
}

