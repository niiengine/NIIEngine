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

#include "OgreShaderExLinearSkinning.h"
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreEntity.h"
#include "OgreSubEntity.h"
#include "OgreMaterial.h"
#include "OgreSubMesh.h"
#include "OgreShaderGenerator.h"

#define HS_DATA_BIND_NAME "HS_SRS_DATA"


namespace Ogre {

namespace RTShader {


/************************************************************************/
/*                                                                      */
/************************************************************************/
LinearSkinning::LinearSkinning() : HardwareSkinningTechnique()
{
}

//-----------------------------------------------------------------------
bool LinearSkinning::resolveParameters(ProgramSet* programSet)
{

	Program* vsProgram = programSet->getCpuVertexProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();

	//if needed mark this vertex program as hardware skinned
	if (mDoBoneCalculations == true)
	{
		vsProgram->setVertexMatrixFusion(true);
	}

	//
	// get the parameters we need whether we are doing bone calculations or not
	//
	// Note: in order to be consistent we will always output position, normal,
	// tangent and binormal in both object and world space. And output position
	// in projective space to cover the responsibility of the transform stage

	//input param
	mParamInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
	mParamInNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
	mParamInBiNormal = vsMain->resolveInputParameter(Parameter::SPS_BINORMAL, 0, Parameter::SPC_BINORMAL_OBJECT_SPACE, GDT_Float3X);
	mParamInTangent = vsMain->resolveInputParameter(Parameter::SPS_TANGENT, 0, Parameter::SPC_TANGENT_OBJECT_SPACE, GDT_Float3X);

	//local param
	mParamLocalPositionWorld = vsMain->resolveLocalParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_WORLD_SPACE, GDT_Float4X);
	mParamLocalNormalWorld = vsMain->resolveLocalParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_WORLD_SPACE, GDT_Float3X);
	mParamLocalTangentWorld = vsMain->resolveLocalParameter(Parameter::SPS_TANGENT, 0, Parameter::SPC_TANGENT_WORLD_SPACE, GDT_Float3X);
	mParamLocalBinormalWorld = vsMain->resolveLocalParameter(Parameter::SPS_BINORMAL, 0, Parameter::SPC_BINORMAL_WORLD_SPACE, GDT_Float3X);

	//output param
	mParamOutPositionProj = vsMain->resolveOutputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_PROJECTIVE_SPACE, GDT_Float4X);

	//check if parameter retrival went well
	bool isValid =
		(mParamInPosition.get() != NULL) &&
		(mParamInNormal.get() != NULL) &&
		(mParamInBiNormal.get() != NULL) &&
		(mParamInTangent.get() != NULL) &&
		(mParamLocalPositionWorld.get() != NULL) &&
		(mParamLocalNormalWorld.get() != NULL) &&
		(mParamLocalTangentWorld.get() != NULL) &&
		(mParamLocalBinormalWorld.get() != NULL) &&
		(mParamOutPositionProj.get() != NULL);


	if(mDoBoneCalculations == true)
	{
		GpuSyncParam worldMatrixType = GpuProgramParam::GSP_Matrix3x4_Space_Array;
		if(ShaderGenerator::getSingleton().getTargetLanguage() == "hlsl")
		{
			//given that hlsl shaders use column major matrices which are not compatible with the cg
			//and glsl method of row major matrices, we will use a full matrix instead.
			worldMatrixType = GpuProgramParam::GSP_Matrix4x4_Space_Array;
		}

		//input parameters
		mParamInNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
		mParamInBiNormal = vsMain->resolveInputParameter(Parameter::SPS_BINORMAL, 0, Parameter::SPC_BINORMAL_OBJECT_SPACE, GDT_Float3X);
		mParamInTangent = vsMain->resolveInputParameter(Parameter::SPS_TANGENT, 0, Parameter::SPC_TANGENT_OBJECT_SPACE, GDT_Float3X);
		mParamInIndices = vsMain->resolveInputParameter(Parameter::SPS_BLEND_INDICES, 0, Parameter::SPC_UNKNOWN, GDT_Float4X);
		mParamInWeights = vsMain->resolveInputParameter(Parameter::SPS_BLEND_WEIGHTS, 0, Parameter::SPC_UNKNOWN, GDT_Float4X);
		mParamInWorldMatrices = vsProgram->resolveAutoParameterInt(worldMatrixType, 0, mBoneCount);
		mParamInInvWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space_Inverse, 0);
		mParamInViewProjMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Proj_X_View, 0);

		mParamTempFloat4 = vsMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, -1, "TempVal4", GDT_Float4X);
		mParamTempFloat3 = vsMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, -1, "TempVal3", GDT_Float3X);

		//check if parameter retrival went well
		isValid &=
			(mParamInIndices.get() != NULL) &&
			(mParamInWeights.get() != NULL) &&
			(mParamInWorldMatrices.get() != NULL) &&
			(mParamInViewProjMatrix.get() != NULL) &&
			(mParamInInvWorldMatrix.get() != NULL) &&
			(mParamTempFloat4.get() != NULL) &&
			(mParamTempFloat3.get() != NULL);
	}
	else
	{
		mParamInWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space, 0);
		mParamInWorldViewProjMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space, 0);

		//check if parameter retrival went well
		isValid &= (mParamInWorldMatrix.get() != NULL) &&
            (mParamInWorldViewProjMatrix.get() != NULL);
	}
	return isValid;
}

//-----------------------------------------------------------------------
bool LinearSkinning::resolveDependencies(ProgramSet * programSet)
{
	Program * vsProgram = programSet->getCpuVertexProgram();
	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(FFP_LIB_TRANSFORM);

	return true;
}

//-----------------------------------------------------------------------
bool LinearSkinning::addFunctionInvocations(ProgramSet* programSet)
{

	Program* vsProgram = programSet->getCpuVertexProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	int internalCounter = 0;

	//add functions to calculate position data in world, object and projective space
	addPositionCalculations(vsMain, internalCounter);

	//add functions to calculate normal and normal related data in world and object space
	addNormalRelatedCalculations(vsMain, mParamInNormal, mParamLocalNormalWorld, internalCounter);
	addNormalRelatedCalculations(vsMain, mParamInTangent, mParamLocalTangentWorld, internalCounter);
	addNormalRelatedCalculations(vsMain, mParamInBiNormal, mParamLocalBinormalWorld, internalCounter);
	return true;
}

//-----------------------------------------------------------------------
void LinearSkinning::addPositionCalculations(Function* vsMain, int& funcCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	if (mDoBoneCalculations == true)
	{
		//set functions to calculate world position
		for(int i = 0 ; i < getWeightCount() ; ++i)
		{
			addIndexedPositionWeight(vsMain, i, funcCounter);
		}

		//update back the original position relative to the object
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamInInvWorldMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamLocalPositionWorld, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamInPosition, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		//update the projective position thereby filling the transform stage role
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamInViewProjMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamLocalPositionWorld, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamOutPositionProj, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
	else
	{
		//update from object to world space
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamInWorldMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamInPosition, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamLocalPositionWorld, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		//update from object to projective space
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamInWorldViewProjMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamInPosition, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamOutPositionProj, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
}

//-----------------------------------------------------------------------
void LinearSkinning::addNormalRelatedCalculations(Function* vsMain,
								ParameterPtr& pNormalRelatedParam,
								ParameterPtr& pNormalWorldRelatedParam,
								int& funcCounter)
{
	FunctionInvocation* curFuncInvocation;

	if (mDoBoneCalculations == true)
	{
		//set functions to calculate world normal
		for(int i = 0 ; i < getWeightCount() ; ++i)
		{
			addIndexedNormalRelatedWeight(vsMain, pNormalRelatedParam, pNormalWorldRelatedParam, i, funcCounter);
		}

		//update back the original position relative to the object
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamInInvWorldMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(pNormalWorldRelatedParam, Operand::OPS_IN);
		curFuncInvocation->pushOperand(pNormalRelatedParam, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
	else
	{
		//update from object to world space
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamInWorldMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(pNormalRelatedParam, Operand::OPS_IN);
		curFuncInvocation->pushOperand(pNormalWorldRelatedParam, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}

}

//-----------------------------------------------------------------------
void LinearSkinning::addIndexedPositionWeight(Function* vsMain,
								int index, int& funcCounter)
{
	Operand::OpMask indexMask = indexToMask(index);

	FunctionInvocation* curFuncInvocation;

	int outputMask = Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z;
	if (mParamInWorldMatrices->getType() == GDT_Matrix4X4)
	{
		outputMask = Operand::OPM_ALL;
	}

	//multiply position with world matrix and put into temporary param
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
	curFuncInvocation->pushOperand(mParamInWorldMatrices, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mParamInIndices, Operand::OPS_IN,  indexMask, 1);
	curFuncInvocation->pushOperand(mParamInPosition, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mParamTempFloat4, Operand::OPS_OUT, outputMask);
	vsMain->addAtomInstance(curFuncInvocation);

	//set w value of temporary param to 1
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_VS_TRANSFORM, funcCounter++);
	curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0f), Operand::OPS_IN);
	curFuncInvocation->pushOperand(mParamTempFloat4, Operand::OPS_OUT, Operand::OPM_W);
	vsMain->addAtomInstance(curFuncInvocation);

	//multiply temporary param with  weight
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, FFP_VS_TRANSFORM, funcCounter++);
	curFuncInvocation->pushOperand(mParamTempFloat4, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mParamInWeights, Operand::OPS_IN, indexMask);
	curFuncInvocation->pushOperand(mParamTempFloat4, Operand::OPS_OUT);
	vsMain->addAtomInstance(curFuncInvocation);

	//check if on first iteration
	if (index == 0)
	{
		//set the local param as the value of the world param
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamTempFloat4, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamLocalPositionWorld, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
	else
	{
		//add the local param as the value of the world param
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamTempFloat4, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamLocalPositionWorld, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mParamLocalPositionWorld, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
}


//-----------------------------------------------------------------------
void LinearSkinning::addIndexedNormalRelatedWeight(Function* vsMain,
								ParameterPtr& pNormalParam,
								ParameterPtr& pNormalWorldRelatedParam,
								int index, int& funcCounter)
{

	FunctionInvocation* curFuncInvocation;

	Operand::OpMask indexMask = indexToMask(index);

	//multiply position with world matrix and put into temporary param
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, FFP_VS_TRANSFORM, funcCounter++);
	curFuncInvocation->pushOperand(mParamInWorldMatrices, Operand::OPS_IN, Operand::OPM_ALL);
	curFuncInvocation->pushOperand(mParamInIndices, Operand::OPS_IN,  indexMask, 1);
	curFuncInvocation->pushOperand(pNormalParam, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mParamTempFloat3, Operand::OPS_OUT);
	vsMain->addAtomInstance(curFuncInvocation);

	//multiply temporary param with weight
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, FFP_VS_TRANSFORM, funcCounter++);
	curFuncInvocation->pushOperand(mParamTempFloat3, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mParamInWeights, Operand::OPS_IN, indexMask);
	curFuncInvocation->pushOperand(mParamTempFloat3, Operand::OPS_OUT);
	vsMain->addAtomInstance(curFuncInvocation);

	//check if on first iteration
	if (index == 0)
	{
		//set the local param as the value of the world normal
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamTempFloat3, Operand::OPS_IN);
		curFuncInvocation->pushOperand(pNormalWorldRelatedParam, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
	else
	{
		//add the local param as the value of the world normal
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, FFP_VS_TRANSFORM, funcCounter++);
		curFuncInvocation->pushOperand(mParamTempFloat3, Operand::OPS_IN);
		curFuncInvocation->pushOperand(pNormalWorldRelatedParam, Operand::OPS_IN);
		curFuncInvocation->pushOperand(pNormalWorldRelatedParam, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
}

}
}

#endif