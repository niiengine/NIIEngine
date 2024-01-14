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

#include "OgreShaderFunction.h"
#include "OgreShaderProgramManager.h"
#include "OgreStringConverter.h"

namespace Ogre {
namespace RTShader {
//-----------------------------------------------------------------------------
Function::Function(const String& name, const String& desc, const FunctionType functionType)
{
	mName			= name;
	mDescription	= desc;
	mFunctionType	= functionType;
}

//-----------------------------------------------------------------------------
Function::~Function()
{
	for (FunctionAtomInstanceIterator it=mAtomInstances.begin(); it != mAtomInstances.end(); ++it)		
		N_delete (*it);
	mAtomInstances.clear();

	for (ShaderParameterIterator it = mInputParameters.begin(); it != mInputParameters.end(); ++it)
		(*it).setNull();
	mInputParameters.clear();

	for (ShaderParameterIterator it = mOutputParameters.begin(); it != mOutputParameters.end(); ++it)
		(*it).setNull();
	mOutputParameters.clear();

	for (ShaderParameterIterator it = mLocalParameters.begin(); it != mLocalParameters.end(); ++it)
		(*it).setNull();
	mLocalParameters.clear();

}

//-----------------------------------------------------------------------------
ParameterPtr Function::resolveInputParameter(Parameter::Semantic semantic,
										int index,
										const Parameter::Content content,
										GpuDataType type)
{
	ParameterPtr param;

	// Check if desired parameter already defined.
	param = getParameterByContent(mInputParameters, content, type);
	if (param.get() != NULL)
		return param;

	// Case we have to create new parameter.
	if (index == -1)
	{
		index = 0;

		// Find the next available index of the target semantic.
		ShaderParameterIterator it;

		for (it = mInputParameters.begin(); it != mInputParameters.end(); ++it)
		{
			if ((*it)->getSemantic() == semantic)
			{
				index++;
			}
		}
	}
	else
	{
		// Check if desired parameter already defined.
		param = getParameterBySemantic(mInputParameters, semantic, index);
		if (param.get() != NULL && param->getContent() == content)
		{
			if (param->getType() == type)
			{
				return param;
			}
			else 
			{
				N_EXCEPT(Exception::ERR_INVALIDPARAMS, 
					"Can not resolve parameter - semantic: " + StrConv::conv(semantic) + " - index: " + StrConv::conv(index) + " due to type mismatch. Function <" + getName() + ">", 			
					"Function::resolveInputParameter" );
			}
		}
	}

	

	// No parameter found -> create new one.
	switch (semantic)
	{
	case Parameter::SPS_POSITION:	
		assert(type == GDT_Float4X);
		param = ParameterFactory::createInPosition(index);
		break;
			
	case Parameter::SPS_BLEND_WEIGHTS:			
		assert(type == GDT_Float4X);
		param = ParameterFactory::createInWeights(index);
		break;
			
	case Parameter::SPS_BLEND_INDICES:
		assert(type == GDT_Float4X);
		param = ParameterFactory::createInIndices(index);
		break;
			
	case Parameter::SPS_NORMAL:
		assert(type == GDT_Float3X);
		param = ParameterFactory::createInNormal(index);
		break;
			
	case Parameter::SPS_COLOR:
		assert(type == GDT_Float4X);
		param = ParameterFactory::createInColor(index);
		break;
						
	case Parameter::SPS_TEXTURE_COORDINATES:		
		param = ParameterFactory::createInTexcoord(type, index, content);				
		break;
			
	case Parameter::SPS_BINORMAL:
		assert(type == GDT_Float3X);
		param = ParameterFactory::createInBiNormal(index);
		break;
			
	case Parameter::SPS_TANGENT:
		assert(type == GDT_Float3X);
		param = ParameterFactory::createInTangent(index);
		break;
	case Parameter::SPS_UNKNOWN:
        break;
	}

	if (param.get() != NULL)
		addInputParameter(param);

	return param;
}

//-----------------------------------------------------------------------------
ParameterPtr Function::resolveOutputParameter(Parameter::Semantic semantic,
											int index,
											Parameter::Content content,											
											GpuDataType type)
{
	ParameterPtr param;

	// Check if desired parameter already defined.
	param = getParameterByContent(mOutputParameters, content, type);
	if (param.get() != NULL)
		return param;

	// Case we have to create new parameter.
	if (index == -1)
	{
		index = 0;

		// Find the next available index of the target semantic.
		ShaderParameterIterator it;

		for (it = mOutputParameters.begin(); it != mOutputParameters.end(); ++it)
		{
			if ((*it)->getSemantic() == semantic)
			{
				index++;
			}
		}
	}
	else
	{
		// Check if desired parameter already defined.
		param = getParameterBySemantic(mOutputParameters, semantic, index);
		if (param.get() != NULL && param->getContent() == content)
		{
			if (param->getType() == type)
			{
				return param;
			}
			else 
			{
				N_EXCEPT(Exception::ERR_INVALIDPARAMS, 
					"Can not resolve parameter - semantic: " + StrConv::conv(semantic) + " - index: " + StrConv::conv(index) + " due to type mismatch. Function <" + getName() + ">", 			
					"Function::resolveOutputParameter" );
			}
		}
	}
	

	// No parameter found -> create new one.
	switch (semantic)
	{
	case Parameter::SPS_POSITION:	
		assert(type == GDT_Float4X);
		param = ParameterFactory::createOutPosition(index);
		break;

	case Parameter::SPS_BLEND_WEIGHTS:		
	case Parameter::SPS_BLEND_INDICES:
		N_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"Can not resolve parameter - semantic: " + StrConv::conv(semantic) + " - index: " + StrConv::conv(index) + " since support in it is not implemented yet. Function <" + getName() + ">", 			
			"Function::resolveOutputParameter" );
		break;

	case Parameter::SPS_NORMAL:
		assert(type == GDT_Float3X);
		param = ParameterFactory::createOutNormal(index);
		break;

	case Parameter::SPS_COLOR:
		assert(type == GDT_Float4X);
		param = ParameterFactory::createOutColor(index);
		break;

	case Parameter::SPS_TEXTURE_COORDINATES:		
		param = ParameterFactory::createOutTexcoord(type, index, content);				
		break;

	case Parameter::SPS_BINORMAL:
		assert(type == GDT_Float3X);
		param = ParameterFactory::createOutBiNormal(index);
		break;

	case Parameter::SPS_TANGENT:
		assert(type == GDT_Float3X);
		param = ParameterFactory::createOutTangent(index);
		break;
	case Parameter::SPS_UNKNOWN:
        break;
	}

	if (param.get() != NULL)
		addOutputParameter(param);

	return param;
}

//-----------------------------------------------------------------------------
ParameterPtr Function::resolveLocalParameter(Parameter::Semantic semantic, int index,
										   const String& name,
										   GpuDataType type)
{
	ParameterPtr param;

	param = getParameterByName(mLocalParameters, name);
	if (param.get() != NULL)
	{
		if (param->getType() == type &&
			param->getSemantic() == semantic &&
			param->getIndex() == index)
		{
			return param;
		}
		else 
		{
			N_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Can not resolve local parameter due to type mismatch. Function <" + getName() + ">", 			
				"Function::resolveLocalParameter" );
		}		
	}
		
	param = ParameterPtr(N_new Parameter(type, name, semantic, index, Parameter::SPC_UNKNOWN));
	addParameter(mLocalParameters, param);
			
	return param;
}

//-----------------------------------------------------------------------------
ParameterPtr Function::resolveLocalParameter(Parameter::Semantic semantic, int index,
										   const Parameter::Content content,
										   GpuDataType type)
{
	ParameterPtr param;

	param = getParameterByContent(mLocalParameters, content, type);
	if (param.get() != NULL)	
		return param;

	param = ParameterPtr(N_new Parameter(type, "lLocalParam_" + StrConv::conv(mLocalParameters.size()), semantic, index, content));
	addParameter(mLocalParameters, param);

	return param;
}

//-----------------------------------------------------------------------------
void Function::addInputParameter(ParameterPtr parameter)
{

	// Check that parameter with the same semantic and index in input parameters list.
	if (getParameterBySemantic(mInputParameters, parameter->getSemantic(), parameter->getIndex()).get() != NULL)
	{
		N_EXCEPT( Exception::ERR_INVALIDPARAMS, 
			"Parameter <" + parameter->getName() + "> has equal sematic parameter in function <" + getName() + ">", 			
			"Function::addInputParameter" );
	}

	addParameter(mInputParameters, parameter);
}

//-----------------------------------------------------------------------------
void Function::addOutputParameter(ParameterPtr parameter)
{
	// Check that parameter with the same semantic and index in output parameters list.
	if (getParameterBySemantic(mOutputParameters, parameter->getSemantic(), parameter->getIndex()).get() != NULL)
	{
		N_EXCEPT( Exception::ERR_INVALIDPARAMS, 
			"Parameter <" + parameter->getName() + "> has equal sematic parameter in function <" + getName() + ">", 			
			"Function::addOutputParameter" );
	}

	addParameter(mOutputParameters, parameter);
}

//-----------------------------------------------------------------------------
void Function::deleteInputParameter(ParameterPtr parameter)
{
	deleteParameter(mInputParameters, parameter);
}

//-----------------------------------------------------------------------------
void Function::deleteOutputParameter(ParameterPtr parameter)
{
	deleteParameter(mOutputParameters, parameter);
}

//-----------------------------------------------------------------------------
void Function::deleteAllInputParameters()
{
	mInputParameters.clear();
}

//-----------------------------------------------------------------------------
void Function::deleteAllOutputParameters()
{
	mOutputParameters.clear();
}
//-----------------------------------------------------------------------------
void Function::addParameter(ShaderParameterList& parameterList, ParameterPtr parameter)
										
{
	// Check that parameter with the same name doest exist in input parameters list.
	if (getParameterByName(mInputParameters, parameter->getName()).get() != NULL)
	{
		N_EXCEPT( Exception::ERR_INVALIDPARAMS, 
			"Parameter <" + parameter->getName() + "> already declared in function <" + getName() + ">", 			
			"Function::addParameter" );
	}

	// Check that parameter with the same name doest exist in output parameters list.
	if (getParameterByName(mOutputParameters, parameter->getName()).get() != NULL)
	{
		N_EXCEPT( Exception::ERR_INVALIDPARAMS, 
			"Parameter <" + parameter->getName() + "> already declared in function <" + getName() + ">", 			
			"Function::addParameter" );
	}


	// Add to given parameters list.
	parameterList.push_back(parameter);
}

//-----------------------------------------------------------------------------
void Function::deleteParameter(ShaderParameterList& parameterList, ParameterPtr parameter)
{
	ShaderParameterIterator it;

	for (it = parameterList.begin(); it != parameterList.end(); ++it)
	{
		if (*it == parameter)
		{
			(*it).setNull();
			parameterList.erase(it);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
ParameterPtr Function::getParameterByName( const ShaderParameterList& parameterList, const String& name )
{
	ShaderParameterConstIterator it;

	for (it = parameterList.begin(); it != parameterList.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			return *it;
		}
	}

	return ParameterPtr();
}

//-----------------------------------------------------------------------------
ParameterPtr Function::getParameterBySemantic(const ShaderParameterList& parameterList, 
												const Parameter::Semantic semantic, 
												int index)
{
	ShaderParameterConstIterator it;

	for (it = parameterList.begin(); it != parameterList.end(); ++it)
	{
		if ((*it)->getSemantic() == semantic &&
			(*it)->getIndex() == index)
		{
			return *it;
		}
	}

	return ParameterPtr();
}

//-----------------------------------------------------------------------------
ParameterPtr Function::getParameterByContent(const ShaderParameterList& parameterList,
    const Parameter::Content content, GpuDataType type)
{
	ShaderParameterConstIterator it;

	// Search only for known content.
	if (content != Parameter::SPC_UNKNOWN)	
	{
		for (it = parameterList.begin(); it != parameterList.end(); ++it)
		{
			if ((*it)->getContent() == content &&
				(*it)->getType() == type)
			{
				return *it;
			}
		}
	}
	
	return ParameterPtr();
}


//-----------------------------------------------------------------------------
void Function::addAtomInstance(FunctionAtom* atomInstance)
{
	mAtomInstances.push_back(atomInstance);
}

//-----------------------------------------------------------------------------
bool Function::deleteAtomInstance(FunctionAtom* atomInstance)
{
	FunctionAtomInstanceIterator it;

	for (it=mAtomInstances.begin(); it != mAtomInstances.end(); ++it)
	{
		if (*it == atomInstance)
		{
			N_delete atomInstance;
			mAtomInstances.erase(it);
			return true;
		}		
	}

	return false;
	
}

//-----------------------------------------------------------------------------
void Function::sortAtomInstances()
{
	if (mAtomInstances.size() > 1)	
		qsort(&mAtomInstances[0], mAtomInstances.size(), sizeof(FunctionAtom*), sAtomInstanceCompare);		
}

//-----------------------------------------------------------------------------
int Function::sAtomInstanceCompare(const void* p0, const void* p1)
{
	FunctionAtom* pInstance0 = *((FunctionAtom**)p0);
	FunctionAtom* pInstance1 = *((FunctionAtom**)p1);

	if (pInstance0->getGroupExecutionOrder() != pInstance1->getGroupExecutionOrder())
	{
		return pInstance0->getGroupExecutionOrder() - pInstance1->getGroupExecutionOrder();
	}
	 
	return pInstance0->getInternalExecutionOrder() - pInstance1->getInternalExecutionOrder();	
}

//-----------------------------------------------------------------------------
Ogre::RTShader::Function::FunctionType Function::getFunctionType() const
{
	return mFunctionType;
}

}
}
