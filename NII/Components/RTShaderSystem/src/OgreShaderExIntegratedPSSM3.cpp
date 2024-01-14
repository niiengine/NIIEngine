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
#include "OgreShaderExIntegratedPSSM3.h"
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreGpuProgram.h"
#include "OgrePass.h"
#include "OgreShaderGenerator.h"
#include "OgreViewport.h"
#include "OgreTextureUnitState.h"
#include "OgreTexture.h"
#include "OgreTextureFrame.h"
#include "NiiFrameBuffer.h"
#include "NiiFrameObj.h"
#include "OgreCamera.h"

namespace Ogre 
{
namespace RTShader 
{

/************************************************************************/
/*                                                                      */
/************************************************************************/
String IntegratedPSSM3::Type = "SGX_IntegratedPSSM3";
//-----------------------------------------------------------------------
IntegratedPSSM3::IntegratedPSSM3()
{

}
//-----------------------------------------------------------------------
const String& IntegratedPSSM3::getType() const
{
	return Type;
}
//-----------------------------------------------------------------------
int	IntegratedPSSM3::getExecutionOrder() const
{
	return FFP_TEXTURING + 1;
}
//-----------------------------------------------------------------------
void IntegratedPSSM3::updateGpuProgramsParams(GeometryObj * rend, ShaderCh * pass,
    const SysSyncParam * source, const LightList * pLightList)
{
	ShadowTextureParamsIterator it = mShadowTextureParamsList.begin();
	size_t shadowIndex = 0;

	while(it != mShadowTextureParamsList.end())
	{
		it->mWorldViewProjMatrix->setGpuParameter(source->getTextureWorldViewProjMatrix(shadowIndex));
		it->mInvTextureSize->setGpuParameter(source->getInverseTextureSize(it->mTextureSamplerIndex));

		++it;
		++shadowIndex;
	}

	Vector4 vSplitPoints;

	vSplitPoints.x = mShadowTextureParamsList[0].mMaxRange;
	vSplitPoints.y = mShadowTextureParamsList[1].mMaxRange;
	vSplitPoints.z = 0.0;
	vSplitPoints.w = 0.0;

	mPSSplitPoints->setGpuParameter(vSplitPoints);

}

//-----------------------------------------------------------------------
void IntegratedPSSM3::write(const SubRenderState & rhs)
{
	const IntegratedPSSM3 & rhsPssm= static_cast<const IntegratedPSSM3 &>(rhs);

	mShadowTextureParamsList.resize(rhsPssm.mShadowTextureParamsList.size());

	ShadowTextureParamsConstIterator itSrc = rhsPssm.mShadowTextureParamsList.begin();
	ShadowTextureParamsIterator itDst = mShadowTextureParamsList.begin();

	while(itDst != mShadowTextureParamsList.end())
	{
		itDst->mMaxRange = itSrc->mMaxRange;
		++itSrc;
		++itDst;
	}
}
//-----------------------------------------------------------------------
bool IntegratedPSSM3::preAddToRenderState(const RenderState * renderState,
    ShaderCh * srcPass, ShaderCh * dstPass)
{
	if(srcPass->isLight() == false ||
		srcPass->getParent()->getParent()->isReceiveShadow() == false)
		return false;

	ShadowTextureParamsIterator it = mShadowTextureParamsList.begin();

	while(it != mShadowTextureParamsList.end())
	{
		ShaderChTextureUnit * curShadowTexture = dstPass->getTexture().create();

		curShadowTexture->setContentType(ShaderChTexture::T_SHADOW);
        curShadowTexture->setUAddressing(TAM_BORDER);
        curShadowTexture->setVAddressing(TAM_BORDER);
        curShadowTexture->setWAddressing(TAM_BORDER);
		curShadowTexture->setBorderColour(Colour::White);
		it->mTextureSamplerIndex = dstPass->getTexture().getCount() - 1;
		++it;
	}
	return true;
}
//-----------------------------------------------------------------------
void IntegratedPSSM3::setSplitPoints(const SplitPointList& newSplitPoints)
{
	if (newSplitPoints.size() != 4)
	{
		N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
			"IntegratedPSSM3 sub render state supports only 4 split points",
			"IntegratedPSSM3::setSplitPoints");
	}

	mShadowTextureParamsList.resize(newSplitPoints.size() - 1);

	for (unsigned int i=1; i < newSplitPoints.size(); ++i)
	{
		ShadowTextureParams& curParams = mShadowTextureParamsList[i-1];

		curParams.mMaxRange				= newSplitPoints[i];
	}
}

//-----------------------------------------------------------------------
bool IntegratedPSSM3::resolveParameters(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();

	// Get input position parameter.
	mVSInPos = vsMain->getParameterBySemantic(vsMain->getInputParameters(), Parameter::SPS_POSITION, 0);
	if (mVSInPos.get() == NULL)
		return false;

	// Get output position parameter.
	mVSOutPos = vsMain->getParameterBySemantic(vsMain->getOutputParameters(), Parameter::SPS_POSITION, 0);
	if (mVSOutPos.get() == NULL)
		return false;

	// Resolve vertex shader output depth.
	mVSOutDepth = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
		Parameter::SPC_DEPTH_VIEW_SPACE,
		GDT_Float);
	if (mVSOutDepth.get() == NULL)
		return false;

	// Resolve input depth parameter.
	mPSInDepth = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, mVSOutDepth->getIndex(),
		mVSOutDepth->getContent(),
		GDT_Float);
	if (mPSInDepth.get() == NULL)
		return false;

	// Get in/local diffuse parameter.
	mPSDiffuse = psMain->getParameterBySemantic(psMain->getInputParameters(), Parameter::SPS_COLOR, 0);
	if (mPSDiffuse.get() == NULL)
	{
		mPSDiffuse = psMain->getParameterBySemantic(psMain->getLocalParameters(), Parameter::SPS_COLOR, 0);
		if (mPSDiffuse.get() == NULL)
			return false;
	}

	// Resolve output diffuse parameter.
	mPSOutDiffuse = psMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
	if (mPSOutDiffuse.get() == NULL)
		return false;

	// Get in/local specular parameter.
	mPSSpecualr = psMain->getParameterBySemantic(psMain->getInputParameters(), Parameter::SPS_COLOR, 1);
	if (mPSSpecualr.get() == NULL)
	{
		mPSSpecualr = psMain->getParameterBySemantic(psMain->getLocalParameters(), Parameter::SPS_COLOR, 1);
		if (mPSSpecualr.get() == NULL)
			return false;
	}

	// Resolve computed local shadow colour parameter.
	mPSLocalShadowFactor = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lShadowFactor", GDT_Float);
	if (mPSLocalShadowFactor.get() == NULL)
		return false;

	// Resolve computed local shadow colour parameter.
	mPSSplitPoints = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Render, "pssm_split_points");
	if (mPSSplitPoints.get() == NULL)
		return false;

	// Get derived scene colour.
	mPSDerivedSceneColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Emissive_Evn, 0);
	if (mPSDerivedSceneColour.get() == NULL)
		return false;

	ShadowTextureParamsIterator it = mShadowTextureParamsList.begin();
	int lightIndex = 0;

	while(it != mShadowTextureParamsList.end())
	{
		it->mWorldViewProjMatrix = vsProgram->resolveParameter(GDT_Matrix4X4, -1, (uint16)GPT_Space, "world_texture_view_proj");
		if (it->mWorldViewProjMatrix.get() == NULL)
			return false;

		it->mVSOutLightPosition = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
			Parameter::Content(Parameter::SPC_POSITION_LIGHT_SPACE0 + lightIndex),
			GDT_Float4X);
		if (it->mVSOutLightPosition.get() == NULL)
			return false;

		it->mPSInLightPosition = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
			it->mVSOutLightPosition->getIndex(),
			it->mVSOutLightPosition->getContent(),
			GDT_Float4X);

		if (it->mPSInLightPosition.get() == NULL)
			return false;

		it->mTextureSampler = psProgram->resolveParameter(GDT_Sampler2D, it->mTextureSamplerIndex, GPT_Render, "shadow_map");
		if (it->mTextureSampler.get() == NULL)
			return false;

		it->mInvTextureSize = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Render, "inv_shadow_texture_size");
		if (it->mInvTextureSize.get() == NULL)
			return false;

		++lightIndex;
		++it;
	}

	return true;
}
//-----------------------------------------------------------------------
bool IntegratedPSSM3::resolveDependencies(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);

	psProgram->addDependency(FFP_LIB_COMMON);
	psProgram->addDependency(SGX_LIB_INTEGRATEDPSSM);

	return true;
}
//-----------------------------------------------------------------------
bool IntegratedPSSM3::addFunctionInvocations(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Program* psProgram = programSet->getCpuFragmentProgram();
	int internalCounter;

	// Add vertex shader invocations.
	internalCounter = 0;
	if (false == addVSInvocation(vsMain, FFP_VS_TEXTURING + 1, internalCounter))
		return false;

	// Add pixel shader invocations.
	internalCounter = 0;
	if (false == addPSInvocation(psProgram, FFP_PS_COLOUR_BEGIN + 2, internalCounter))
		return false;

	return true;
}
//-----------------------------------------------------------------------
bool IntegratedPSSM3::addVSInvocation(Function* vsMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation;

	// Output the vertex depth in camera space.
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mVSOutPos, Operand::OPS_IN, Operand::OPM_Z);
	curFuncInvocation->pushOperand(mVSOutDepth, Operand::OPS_OUT);
	vsMain->addAtomInstance(curFuncInvocation);


	// Compute world space position.
	ShadowTextureParamsIterator it = mShadowTextureParamsList.begin();

	while(it != mShadowTextureParamsList.end())
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_TRANSFORM, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(it->mWorldViewProjMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSInPos, Operand::OPS_IN);
		curFuncInvocation->pushOperand(it->mVSOutLightPosition, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		++it;
	}
	return true;
}
//-----------------------------------------------------------------------
bool IntegratedPSSM3::addPSInvocation(Program * psProgram, const int groupOrder, int& internalCounter)
{
	Function* psMain = psProgram->getEntryPointFunction();
	FunctionInvocation* curFuncInvocation;

	ShadowTextureParams& splitParams0 = mShadowTextureParamsList[0];
	ShadowTextureParams& splitParams1 = mShadowTextureParamsList[1];
	ShadowTextureParams& splitParams2 = mShadowTextureParamsList[2];

	// Compute shadow factor.
	curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_COMPUTE_SHADOW_COLOUR3, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mPSInDepth, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSSplitPoints, Operand::OPS_IN);
	curFuncInvocation->pushOperand(splitParams0.mPSInLightPosition, Operand::OPS_IN);
	curFuncInvocation->pushOperand(splitParams1.mPSInLightPosition, Operand::OPS_IN);
	curFuncInvocation->pushOperand(splitParams2.mPSInLightPosition, Operand::OPS_IN);
	curFuncInvocation->pushOperand(splitParams0.mTextureSampler, Operand::OPS_IN);
	curFuncInvocation->pushOperand(splitParams1.mTextureSampler, Operand::OPS_IN);
	curFuncInvocation->pushOperand(splitParams2.mTextureSampler, Operand::OPS_IN);
 	curFuncInvocation->pushOperand(splitParams0.mInvTextureSize, Operand::OPS_IN);
 	curFuncInvocation->pushOperand(splitParams1.mInvTextureSize, Operand::OPS_IN);
 	curFuncInvocation->pushOperand(splitParams2.mInvTextureSize, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSLocalShadowFactor, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);

	// Apply shadow factor on diffuse colour.
	curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_APPLYSHADOWFACTOR_DIFFUSE, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mPSDerivedSceneColour, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSLocalShadowFactor, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);

	// Apply shadow factor on specular colour.
	curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_MODULATE_SCALAR, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mPSLocalShadowFactor, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSSpecualr, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSSpecualr, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);

	// Assign the local diffuse to output diffuse.
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);

	return true;
}

//-----------------------------------------------------------------------
const String & IntegratedPSSM3Factory::getType() const
{
	return IntegratedPSSM3::Type;
}
//-----------------------------------------------------------------------
SubRenderState *	IntegratedPSSM3Factory::createInstance(ScriptCompiler * compiler,
    PropertyScriptNode * prop, ShaderCh * pass, SGScriptAnalyze * translator)
{
	if (prop->name == "integrated_pssm4")
	{
		if (prop->values.size() != 4)
		{
			compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
		}
		else
		{
			IntegratedPSSM3::SplitPointList splitPointList;

			ScriptNodeList::const_iterator it = prop->values.begin();
			ScriptNodeList::const_iterator itEnd = prop->values.end();

			while(it != itEnd)
			{
				NIIf curSplitValue;

				if (false == SGScriptAnalyze::getReal(*it, &curSplitValue))
				{
					compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
					break;
				}

				splitPointList.push_back(curSplitValue);

				++it;
			}

			if (splitPointList.size() == 4)
			{
				SubRenderState* subRenderState = createOrRetrieveInstance(translator);
				IntegratedPSSM3* pssmSubRenderState = static_cast<IntegratedPSSM3*>(subRenderState);

				pssmSubRenderState->setSplitPoints(splitPointList);

				return pssmSubRenderState;
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------
SubRenderState* IntegratedPSSM3Factory::createInstanceImpl()
{
	return N_new IntegratedPSSM3;
}

}
}

#endif
