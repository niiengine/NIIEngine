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
#include "OgreShaderFFPTransform.h"
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"

namespace Ogre {
namespace RTShader {

/************************************************************************/
/*                                                                      */
/************************************************************************/
String FFPTransform::Type = "FFP_Transform";

//-----------------------------------------------------------------------
const String& FFPTransform::getType() const
{
	return Type;
}


//-----------------------------------------------------------------------
int	FFPTransform::getExecutionOrder() const
{
	return FFP_TRANSFORM;
}

//-----------------------------------------------------------------------
bool FFPTransform::createCpuSubPrograms(ProgramSet * programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	
	// Resolve World View Projection Matrix.
	UniformParameterPtr wvpMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space, 0);
	if(wvpMatrix.get() == NULL)
		return false;
		
	Function* vsEntry = vsProgram->getEntryPointFunction();
	assert(vsEntry != NULL);

	// Resolve input position parameter.
	ParameterPtr positionIn = vsEntry->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);	
	if(positionIn.get() == NULL)
		return false;
	

	// Resolve output position parameter.
	ParameterPtr positionOut = vsEntry->resolveOutputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_PROJECTIVE_SPACE, GDT_Float4X);
	if(positionOut.get() == NULL)
		return false;
	

	// Add dependency.
	vsProgram->addDependency(FFP_LIB_TRANSFORM);

	FunctionInvocation* transformFunc = N_new FunctionInvocation(FFP_FUNC_TRANSFORM,  FFP_VS_TRANSFORM, 0); 

	transformFunc->pushOperand(wvpMatrix, Operand::OPS_IN);
	transformFunc->pushOperand(positionIn, Operand::OPS_IN);
	transformFunc->pushOperand(positionOut, Operand::OPS_OUT);

	vsEntry->addAtomInstance(transformFunc);

	return true;
}


//-----------------------------------------------------------------------
void FFPTransform::write(const SubRenderState& rhs)
{

}

//-----------------------------------------------------------------------
const String& FFPTransformFactory::getType() const
{
	return FFPTransform::Type;
}

//-----------------------------------------------------------------------
SubRenderState*	FFPTransformFactory::createInstance(ScriptCompiler* compiler, 
    PropertyScriptNode* prop, ShaderCh* pass, SGScriptAnalyze* translator)
{
	if (prop->name == "transform_stage")
	{
		if(prop->values.size() == 1)
		{
			String modelType;

			if(false == SGScriptAnalyze::get(prop->values.front(), &modelType))
			{
				compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
				return NULL;
			}

			if (modelType == "ffp")
			{
				return createOrRetrieveInstance(translator);
			}
		}		
	}

	return NULL;
}

//-----------------------------------------------------------------------
void FFPTransformFactory::writeInstance(MaterialSerializer* ser, SubRenderState* subRenderState, 
									   ShaderCh* srcPass, ShaderCh* dstPass)
{
	ser->writeAttribute(4, "transform_stage");
	ser->writeValue("ffp");
}

//-----------------------------------------------------------------------
SubRenderState*	FFPTransformFactory::createInstanceImpl()
{
	return N_new FFPTransform;
}


}
}

#endif
