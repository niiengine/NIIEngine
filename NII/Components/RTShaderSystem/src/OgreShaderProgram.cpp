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

#include "OgreShaderProgram.h"
#include "OgreStringConverter.h"

namespace Ogre 
{
namespace RTShader 
{

//-----------------------------------------------------------------------------
Program::Program(GpuProgramType type)
{
	mType				= type;
	mEntryPointFunction = NULL;
	mSkeletalAnimation	= false;
}

//-----------------------------------------------------------------------------
Program::~Program()
{
	destroyParameters();

	destroyFunctions();
}

//-----------------------------------------------------------------------------
void Program::destroyParameters()
{
	mParameters.clear();
}

//-----------------------------------------------------------------------------
void Program::destroyFunctions()
{
	ShaderFunctionIterator it;

	for (it = mFunctions.begin(); it != mFunctions.end(); ++it)
	{
		if (*it != NULL)
		{
			N_delete *it;
		}	
	}
	mFunctions.clear();
}

//-----------------------------------------------------------------------------
GpuProgramType Program::getType() const
{
	return mType;
}

//-----------------------------------------------------------------------------
void Program::addParameter(UniformParameterPtr parameter)
{
	if (getParameterByName(parameter->getName()).get() != NULL)
	{
		N_EXCEPT( Exception::ERR_INVALIDPARAMS, 
			"Parameter <" + parameter->getName() + "> already declared in program.", 
			"Program::addParameter" );
	}

	mParameters.push_back(parameter);
}

//-----------------------------------------------------------------------------
void Program::removeParameter(UniformParameterPtr parameter)
{
	UniformParameterIterator it;

	for (it = mParameters.begin(); it != mParameters.end(); ++it)
	{
		if ((*it) == parameter)
		{
			(*it).setNull();
			mParameters.erase(it);
		}
	}
}

//-----------------------------------------------------------------------------
UniformParameterPtr Program::resolveAutoParameterReal(
    GpuSyncParam autoType, NIIf data, size_t size)
{
	UniformParameterPtr param;

	// Check if parameter already exists.
	param = getParameterByAutoType(autoType);
	if (param.get() != NULL)
	{
		if (param->isAutoConstantRealParameter() &&
			param->getAutoConstantRealData() == data)
		{
			param->setSize(std::max(size, param->getSize()));
			return param;
		}
	}
	
	// Create new parameter.
	param = UniformParameterPtr(N_new UniformParameter(autoType, data, size));
	addParameter(param);

	return param;
}

//-----------------------------------------------------------------------------
UniformParameterPtr Program::resolveAutoParameterReal(
    GpuSyncParam autoType, GpuDataType type, NIIf data, size_t size)
{
	UniformParameterPtr param;

	// Check if parameter already exists.
	param = getParameterByAutoType(autoType);
	if (param.get() != NULL)
	{
		if (param->isAutoConstantRealParameter() &&
			param->getAutoConstantRealData() == data)
		{
			param->setSize(std::max(size, param->getSize()));
			return param;
		}
	}
	
	// Create new parameter.
	param = UniformParameterPtr(N_new UniformParameter(autoType, data, size, type));
	addParameter(param);

	return param;
}

//-----------------------------------------------------------------------------
UniformParameterPtr Program::resolveAutoParameterInt(
    GpuSyncParam autoType, size_t data, size_t size)
{
	UniformParameterPtr param;

	// Check if parameter already exists.
	param = getParameterByAutoType(autoType);
	if (param.get() != NULL)
	{
		if (param->isAutoConstantIntParameter() &&
			param->getAutoConstantIntData() == data)
		{
			param->setSize(std::max(size, param->getSize()));
			return param;
		}
	}

	// Create new parameter.
	param = UniformParameterPtr(N_new UniformParameter(autoType, data, size));
	addParameter(param);

	return param;
}

//-----------------------------------------------------------------------------
UniformParameterPtr Program::resolveAutoParameterInt(
    GpuSyncParam autoType, GpuDataType type, size_t data, size_t size)
{
	UniformParameterPtr param;

	// Check if parameter already exists.
	param = getParameterByAutoType(autoType);
	if (param.get() != NULL)
	{
		if (param->isAutoConstantIntParameter() &&
			param->getAutoConstantIntData() == data)
		{
			param->setSize(std::max(size, param->getSize()));
			return param;
		}
	}

	// Create new parameter.
	param = UniformParameterPtr(N_new UniformParameter(autoType, data, size, type));
	addParameter(param);

	return param;
}

//-----------------------------------------------------------------------------
UniformParameterPtr Program::resolveParameter(GpuDataType type, 
    int index, Nmark typemark, const String & suggestedName, size_t size)
{
	UniformParameterPtr param;

	if (index == -1)
	{
		index = 0;

		// Find the next available index of the target type.
		UniformParameterIterator it;

		for (it = mParameters.begin(); it != mParameters.end(); ++it)
		{
			if ((*it)->getType() == type &&
				(*it)->isAutoConstantParameter() == false)
			{
				index++;
			}
		}
	}
	else
	{
		// Check if parameter already exists.
		param = getParameterByType(type, index);
		if (param.get() != NULL)
		{		
			return param;		
		}
	}
	
	// Create new parameter.
	param = ParameterFactory::createUniform(type, index, typemark, suggestedName, size);
	addParameter(param);

	return param;
}



//-----------------------------------------------------------------------------
UniformParameterPtr Program::getParameterByName(const String& name)
{
	UniformParameterIterator it;

	for (it = mParameters.begin(); it != mParameters.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			return *it;
		}
	}

	return UniformParameterPtr();
}

//-----------------------------------------------------------------------------
UniformParameterPtr Program::getParameterByType(GpuDataType type, int index)
{
	UniformParameterIterator it;

	for (it = mParameters.begin(); it != mParameters.end(); ++it)
	{
		if ((*it)->getType() == type &&
			(*it)->getIndex() == index)
		{
			return *it;
		}
	}

	return UniformParameterPtr();
}

//-----------------------------------------------------------------------------
UniformParameterPtr Program::getParameterByAutoType(GpuSyncParam autoType)
{
	UniformParameterIterator it;

	for (it = mParameters.begin(); it != mParameters.end(); ++it)
	{
		if ((*it)->isAutoConstantParameter() && (*it)->getAutoConstantType() == autoType)
		{
			return *it;
		}
	}

	return UniformParameterPtr();
}

//-----------------------------------------------------------------------------
Function* Program::createFunction(const String & name, const String & desc,
    const Function::FunctionType functionType)
{
	Function* shaderFunction;

	shaderFunction = getFunctionByName(name);
	if (shaderFunction != NULL)
	{
		N_EXCEPT( Exception::ERR_INVALIDPARAMS, 
			"Function " + name + " already declared in program.");
	}

	shaderFunction = N_new Function(name, desc, functionType);
	mFunctions.push_back(shaderFunction);

	return shaderFunction;
}

//-----------------------------------------------------------------------------
Function* Program::getFunctionByName(const String& name)
{
	ShaderFunctionIterator it;

	for (it = mFunctions.begin(); it != mFunctions.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			return *it;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
void Program::addDependency(const String& libFileName)
{
	for (unsigned int i=0; i < mDependencies.size(); ++i)
	{
		if (mDependencies[i] == libFileName)
		{
			return;
		}
	}
	mDependencies.push_back(libFileName);
}

//-----------------------------------------------------------------------------
size_t Program::getDependencyCount() const
{
	return mDependencies.size();
}

//-----------------------------------------------------------------------------
const String& Program::getDependency(unsigned int index) const
{
	return mDependencies[index];
}

}
}
