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
#include "OgreShaderFFPFog.h"
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreGpuProgram.h"
#include "OgrePass.h"
#include "OgreShaderGenerator.h"

namespace Ogre
{
namespace RTShader
{

/************************************************************************/
/*                                                                      */
/************************************************************************/
String FFPFog::Type = "FFP_Fog";

//-----------------------------------------------------------------------
FFPFog::FFPFog()
{
	mFogMode				= FM_NONE;
	mCalcMode				= CM_PER_VERTEX;
	mPassOverrideParams		= false;
}
//-----------------------------------------------------------------------
const String& FFPFog::getType() const
{
	return Type;
}
//-----------------------------------------------------------------------
int	FFPFog::getExecutionOrder() const
{
	return FFP_FOG;
}
//-----------------------------------------------------------------------
void FFPFog::updateGpuProgramsParams(GeometryObj * rend, ShaderCh * pass,
    const SysSyncParam * source, const LightList * pLightList)
{
	if (mFogMode == FM_NONE)
		return;

	FogMode fogMode;
	Colour newFogColour;
	NIIf newFogStart, newFogEnd, newFogDensity;

	if (mPassOverrideParams)
	{
		fogMode			= pass->mMode;
		newFogColour	= pass->mColour;
		newFogStart		= pass->mBegin;
		newFogEnd		= pass->mEnd;
		newFogDensity	= pass->mDensity;
	}
	else
	{
		SpaceManager * sceneMgr = ShaderGenerator::getSingleton().getActiveSceneManager();
        const ShaderChFog * fog = sceneMgr->getFog(ShaderCh::SOT_Get);
		fogMode	= fog->mMode;
		newFogColour = fog->mColour;
		newFogStart	= fog->mBegin;
		newFogEnd = fog->mEnd;
		newFogDensity = fog->mDensity;
	}

	// Set fog properties.
	setFogProperties(fogMode, newFogColour, newFogStart, newFogEnd, newFogDensity);

	// Per pixel fog.
	if (mCalcMode == CM_PER_PIXEL)
	{
		mFogParams->setGpuParameter(mFogParamsValue);
	}

	// Per vertex fog.
	else
	{
		mFogParams->setGpuParameter(mFogParamsValue);
	}

	mFogColour->setGpuParameter(mFogColourValue);

}

//-----------------------------------------------------------------------
bool FFPFog::resolveParameters(ProgramSet * programSet)
{
	if(mFogMode == FM_NONE)
		return true;

	Program * vsProgram = programSet->getCpuVertexProgram();
	Program * psProgram = programSet->getCpuFragmentProgram();
	Function * vsMain = vsProgram->getEntryPointFunction();
	Function * psMain = psProgram->getEntryPointFunction();

	// Resolve world view matrix.
	mWorldViewProjMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space, 0);
	if (mWorldViewProjMatrix.get() == NULL)
		return false;

	// Resolve vertex shader input position.
	mVSInPos = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
	if (mVSInPos.get() == NULL)
		return false;


	// Resolve fog colour.
	mFogColour = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Render, "gFogColor");
	if (mFogColour.get() == NULL)
		return false;


	// Resolve pixel shader output diffuse color.
	mPSOutDiffuse = psMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
	if (mPSOutDiffuse.get() == NULL)
		return false;

	// Per pixel fog.
	if (mCalcMode == CM_PER_PIXEL)
	{
		// Resolve fog params.
		mFogParams = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Render, "gFogParams");
		if (mFogParams.get() == NULL)
			return false;


		// Resolve vertex shader output depth.
		mVSOutDepth = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
			Parameter::SPC_DEPTH_VIEW_SPACE,
			GDT_Float);
		if (mVSOutDepth.get() == NULL)
			return false;


		// Resolve pixel shader input depth.
		mPSInDepth = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, mVSOutDepth->getIndex(),
			mVSOutDepth->getContent(),
			GDT_Float);
		if (mPSInDepth.get() == NULL)
			return false;

	}

	// Per vertex fog.
	else
	{
		// Resolve fog params.
		mFogParams = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Render, "gFogParams");
		if (mFogParams.get() == NULL)
			return false;


		// Resolve vertex shader output fog factor.
		mVSOutFogFactor = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
			Parameter::SPC_UNKNOWN, GDT_Float);
		if (mVSOutFogFactor.get() == NULL)
			return false;


		// Resolve pixel shader input fog factor.
		mPSInFogFactor = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, mVSOutFogFactor->getIndex(),
			mVSOutFogFactor->getContent(), GDT_Float);
		if (mPSInFogFactor.get() == NULL)
			return false;
	}


	return true;
}

//-----------------------------------------------------------------------
bool FFPFog::resolveDependencies(ProgramSet* programSet)
{
	if (mFogMode == FM_NONE)
		return true;

	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_FOG);
	psProgram->addDependency(FFP_LIB_COMMON);

	// Per pixel fog.
	if (mCalcMode == CM_PER_PIXEL)
	{
		psProgram->addDependency(FFP_LIB_FOG);

	}

	return true;
}

//-----------------------------------------------------------------------
bool FFPFog::addFunctionInvocations(ProgramSet* programSet)
{
	if (mFogMode == FM_NONE)
		return true;

	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();
	FunctionInvocation* curFuncInvocation = NULL;
	int internalCounter;


	// Per pixel fog.
	if (mCalcMode == CM_PER_PIXEL)
	{
		internalCounter = 0;
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_PIXELFOG_DEPTH, FFP_VS_FOG, internalCounter++);
		curFuncInvocation->pushOperand(mWorldViewProjMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSInPos, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSOutDepth, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		internalCounter = 0;
		switch (mFogMode)
		{
		case FM_LINEAR:
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_PIXELFOG_LINEAR, FFP_PS_FOG, internalCounter++);
			break;
		case FM_EXP:
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_PIXELFOG_EXP, FFP_PS_FOG, internalCounter++);
			break;
		case FM_EXP2:
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_PIXELFOG_EXP2, FFP_PS_FOG, internalCounter++);
			break;
        case FM_NONE:
        default:
            break;
		}

		curFuncInvocation->pushOperand(mPSInDepth, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mFogParams, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mFogColour, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);

	}

	// Per vertex fog.
	else
	{
		internalCounter = 0;

		switch (mFogMode)
		{
		case FM_LINEAR:
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_VERTEXFOG_LINEAR, FFP_VS_FOG, internalCounter++);
			break;
		case FM_EXP:
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_VERTEXFOG_EXP, FFP_VS_FOG, internalCounter++);
			break;
		case FM_EXP2:
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_VERTEXFOG_EXP2, FFP_VS_FOG, internalCounter++);
			break;
        case FM_NONE:
        default:
            break;
		}

		curFuncInvocation->pushOperand(mWorldViewProjMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSInPos, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mFogParams, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSOutFogFactor, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);


		internalCounter = 0;

		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LERP, FFP_PS_FOG, internalCounter++);
		curFuncInvocation->pushOperand(mFogColour, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSInFogFactor, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
	}
	return true;
}
//-----------------------------------------------------------------------
void FFPFog::write(const SubRenderState & rhs)
{
	const FFPFog & rhsFog = static_cast<const FFPFog &>(rhs);

	mFogMode = rhsFog.mFogMode;
	mFogColourValue = rhsFog.mFogColourValue;
	mFogParamsValue = rhsFog.mFogParamsValue;

	setCalcMode(rhsFog.mCalcMode);
}
//-----------------------------------------------------------------------
bool FFPFog::preAddToRenderState(const RenderState * renderState, ShaderCh * srcPass,
    ShaderCh * dstPass)
{
	FogMode fogMode;
    ShaderChFog dfog;
    const ShaderChFog * fog1 = srcPass->getFog(ShaderCh::SOT_Get);
	if(fog != 0)
	{
		dfog.mMode	= fog1->mMode;
		dfog.mColour = fog1->mColour;
		dfog.mBegin	= fog1->mBegin;
		dfog.mEnd = fog1->mEnd;
		dfog.mDensity = fog1->mDensity;
		mPassOverrideParams = true;
	}
	else
	{
		SpaceManager * sceneMgr = ShaderGenerator::getSingleton().getActiveSceneManager();
        const ShaderChFog * fog = sceneMgr->getFog(ShaderCh::SOT_Get);
		if(sceneMgr == NULL)
		{
			dfog.mMode = FM_NONE;
			dfog.mColour = Colour::White;
			dfog.mBegin = 0.0;
			dfog.mEnd = 0.0;
			dfog.mDensity = 0.0;
		}
		else
		{
			dfog.mMode = fog->mMode;
			dfog.mColour = fog->mColour;
			dfog.mBegin = fog->mBegin;
			dfog.mEnd = fog->mEnd;
			dfog.mDensity = fog->mDensity;
		}
		mPassOverrideParams = false;
	}

	// Set fog properties.
	setFogProperties(dfog.mMode, dfog.mColour, dfog.mBegin, dfog.mEnd, dfog.mDensity);
	// Override scene fog since it will happen in shader.
    dfog.mMode = FM_NONE;
	dstPass->setFog(dfog);
	return true;
}

//-----------------------------------------------------------------------
void FFPFog::setFogProperties(FogMode fogMode, const Colour & fogColour,
    float fogStart, float fogEnd, float fogDensity)
{
	mFogMode			= fogMode;
	mFogColourValue		= fogColour;
	mFogParamsValue.x 	= fogDensity;
	mFogParamsValue.y 	= fogStart;
	mFogParamsValue.z 	= fogEnd;
	mFogParamsValue.w 	= fogEnd != fogStart ? 1 / (fogEnd - fogStart) : 0;
}
//-----------------------------------------------------------------------
const String & FFPFogFactory::getType() const
{
	return FFPFog::Type;
}
//-----------------------------------------------------------------------
SubRenderState * FFPFogFactory::createInstance(ScriptCompiler * compiler,
    PropertyScriptNode * prop, ShaderCh * pass, SGScriptAnalyze * translator)
{
	if (prop->name == "fog_stage")
	{
		if(prop->values.size() >= 1)
		{
			String strValue;

			if(false == SGScriptAnalyze::get(prop->values.front(), &strValue))
			{
				compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
				return NULL;
			}

			if (strValue == "ffp")
			{
				SubRenderState* subRenderState = createOrRetrieveInstance(translator);
				FFPFog* fogSubRenderState = static_cast<FFPFog*>(subRenderState);
				ScriptNodeList::const_iterator it = prop->values.begin();

				if(prop->values.size() >= 2)
				{
					++it;
					if (false == SGScriptAnalyze::get(*it, &strValue))
					{
						compiler->error(prop, ScriptCompiler::CE_STRINGEXPECTED);
						return NULL;
					}

					if (strValue == "per_vertex")
					{
						fogSubRenderState->setCalcMode(FFPFog::CM_PER_VERTEX);
					}
					else if (strValue == "per_pixel")
					{
						fogSubRenderState->setCalcMode(FFPFog::CM_PER_PIXEL);
					}
				}

				return subRenderState;
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------
void FFPFogFactory::writeInstance(MaterialSerializer * ser,
    SubRenderState * subRenderState, ShaderCh * srcPass, ShaderCh * dstPass)
{
	ser->writeAttribute(4, "fog_stage");
	ser->writeValue("ffp");

	FFPFog* fogSubRenderState = static_cast<FFPFog*>(subRenderState);


	if (fogSubRenderState->getCalcMode() == FFPFog::CM_PER_VERTEX)
	{
		ser->writeValue("per_vertex");
	}
	else if (fogSubRenderState->getCalcMode() == FFPFog::CM_PER_PIXEL)
	{
		ser->writeValue("per_pixel");
	}
}

//-----------------------------------------------------------------------
SubRenderState*	FFPFogFactory::createInstanceImpl()
{
	return N_new FFPFog;
}

}
}

#endif
