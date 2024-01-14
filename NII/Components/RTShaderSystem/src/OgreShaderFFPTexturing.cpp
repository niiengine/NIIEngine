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
#include "OgreShaderFFPTexturing.h"
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreTextureUnitState.h"
#include "OgreFrustum.h"
#include "OgrePass.h"

namespace Ogre {
namespace RTShader {

/************************************************************************/
/*                                                                      */
/************************************************************************/
String FFPTexturing::Type = "FFP_Texturing";
#define _INT_VALUE(f) (*(int*)(&(f)))

const String c_ParamTexelEx("texel_");

//-----------------------------------------------------------------------
FFPTexturing::FFPTexturing()
{
}

//-----------------------------------------------------------------------
const String& FFPTexturing::getType() const
{
	return Type;
}

//-----------------------------------------------------------------------
int	FFPTexturing::getExecutionOrder() const
{
	return FFP_TEXTURING;
}

//-----------------------------------------------------------------------
bool FFPTexturing::resolveParameters(ProgramSet * programSet)
{
	for (unsigned int i=0; i < mTextureUnitParamsList.size(); ++i)
	{
		TextureUnitParams* curParams = &mTextureUnitParamsList[i];

		if (false == resolveUniformParams(curParams, programSet))
			return false;


		if (false == resolveFunctionsParams(curParams, programSet))
			return false;
	}


	return true;
}

//-----------------------------------------------------------------------
bool FFPTexturing::resolveUniformParams(TextureUnitParams* textureUnitParams, ProgramSet* programSet)
{
	Program * vsProgram = programSet->getCpuVertexProgram();
	Program * psProgram = programSet->getCpuFragmentProgram();


	// Resolve texture sampler parameter.
	textureUnitParams->mTextureSampler = psProgram->resolveParameter(textureUnitParams->mTextureSamplerType, textureUnitParams->mTextureSamplerIndex, GPT_Render, "gTextureSampler");
	if(textureUnitParams->mTextureSampler.get() == NULL)
		return false;


	// Resolve texture matrix parameter.
	if (needsTextureMatrix(textureUnitParams->mTextureUnitState))
	{
		textureUnitParams->mTextureMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Texture_Matrix_Transform_ParamIndex, textureUnitParams->mTextureSamplerIndex);
		if(textureUnitParams->mTextureMatrix.get() == NULL)
			return false;
	}

	switch(textureUnitParams->mTexCoordCalcMethod)
	{
	case TCP_NONE:
		break;

	// Resolve World + View matrices.
	case TCP_ENV_SPHERE:
	case TCP_ENV_REFLECTION:
	case TCP_ENV_NORMAL:

		mWorldITMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space_InverseTranspose, 0);
		if (mWorldITMatrix.get() == NULL)
			return false;

		mViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View, 0);
		if (mViewMatrix.get() == NULL)
			return false;

		break;

	case TCP_ENV_REFLECTION_INV:
		mWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space, 0);
		if (mWorldMatrix.get() == NULL)
			return false;

		mWorldITMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space_InverseTranspose, 0);
		if (mWorldITMatrix.get() == NULL)
			return false;

		mViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View, 0);
		if (mViewMatrix.get() == NULL)
			return false;

		break;
	case TCP_ENV_PROJECTIVE:

		mWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space, 0);
		if(mWorldMatrix.get() == NULL)
			return false;

		textureUnitParams->mTextureViewProjImageMatrix = vsProgram->resolveParameter(GDT_Matrix4X4, -1, GPT_Light, "gTexViewProjImageMatrix");
		if(textureUnitParams->mTextureViewProjImageMatrix.get() == NULL)
			return false;

        if(textureUnitParams->mTextureUnitState->isProjTexture())
        {
            textureUnitParams->mTextureProjector = textureUnitParams->mTextureUnitState->getProjFrustum();
        }

		if (textureUnitParams->mTextureProjector == NULL)
			return false;

		break;
	}
	return true;
}
//-----------------------------------------------------------------------
bool FFPTexturing::resolveFunctionsParams(TextureUnitParams* textureUnitParams, ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain   = vsProgram->getEntryPointFunction();
	Function* psMain   = psProgram->getEntryPointFunction();
	Parameter::Content texCoordContent = Parameter::SPC_UNKNOWN;

	switch (textureUnitParams->mTexCoordCalcMethod)
	{
		case TCP_NONE:
			// Resolve explicit vs input texture coordinates.

			if (textureUnitParams->mTextureMatrix.get() == NULL)
				texCoordContent = Parameter::Content(Parameter::SPC_TEXTURE_COORDINATE0 + textureUnitParams->mTextureUnitState->getCoordSet());

			textureUnitParams->mVSInputTexCoord = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
				textureUnitParams->mTextureUnitState->getCoordSet(),
				Parameter::Content(Parameter::SPC_TEXTURE_COORDINATE0 + textureUnitParams->mTextureUnitState->getCoordSet()),
				textureUnitParams->mVSInTextureCoordinateType);
			if (textureUnitParams->mVSInputTexCoord.get() == NULL)
				return false;
			break;

		case TCP_ENV_SPHERE:
		case TCP_ENV_REFLECTION:
		case TCP_ENV_NORMAL:
			// Resolve vertex normal.
			mVSInputNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
			if (mVSInputNormal.get() == NULL)
				return false;
			break;

		case TCP_ENV_REFLECTION_INV:

			// Resolve vertex normal.
			mVSInputNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
			if (mVSInputNormal.get() == NULL)
				return false;

			// Resolve vertex position.
			mVSInputPos = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInputPos.get() == NULL)
				return false;
			break;

		case TCP_ENV_PROJECTIVE:
			// Resolve vertex position.
			mVSInputPos = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInputPos.get() == NULL)
				return false;
			break;
	}

	// Resolve vs output texture coordinates.
	textureUnitParams->mVSOutputTexCoord = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES,
		-1, texCoordContent, textureUnitParams->mVSOutTextureCoordinateType);

	if (textureUnitParams->mVSOutputTexCoord.get() == NULL)
		return false;


	// Resolve ps input texture coordinates.
	textureUnitParams->mPSInputTexCoord = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
		textureUnitParams->mVSOutputTexCoord->getIndex(),
		textureUnitParams->mVSOutputTexCoord->getContent(),
		textureUnitParams->mVSOutTextureCoordinateType);

	if (textureUnitParams->mPSInputTexCoord.get() == NULL)
		return false;

	const ShaderParameterList& inputParams = psMain->getInputParameters();
	const ShaderParameterList& localParams = psMain->getLocalParameters();

	mPSDiffuse = psMain->getParameterByContent(inputParams, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
	if (mPSDiffuse.get() == NULL)
	{
		mPSDiffuse = psMain->getParameterByContent(localParams, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
		if (mPSDiffuse.get() == NULL)
			return false;
	}

	mPSSpecular = psMain->getParameterByContent(inputParams, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
	if (mPSSpecular.get() == NULL)
	{
		mPSSpecular = psMain->getParameterByContent(localParams, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
		if (mPSSpecular.get() == NULL)
			return false;
	}

	mPSOutDiffuse = psMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
	if (mPSOutDiffuse.get() == NULL)
		return false;


	return true;
}

//-----------------------------------------------------------------------
bool FFPTexturing::resolveDependencies(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(FFP_LIB_TEXTURING);
	psProgram->addDependency(FFP_LIB_COMMON);
	psProgram->addDependency(FFP_LIB_TEXTURING);

	return true;
}

//-----------------------------------------------------------------------
bool FFPTexturing::addFunctionInvocations(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain   = vsProgram->getEntryPointFunction();
	Function* psMain   = psProgram->getEntryPointFunction();
	int internalCounter = 0;

	for (unsigned int i=0; i < mTextureUnitParamsList.size(); ++i)
	{
		TextureUnitParams* curParams = &mTextureUnitParamsList[i];

		if (false == addVSFunctionInvocations(curParams, vsMain))
			return false;

		if (false == addPSFunctionInvocations(curParams, psMain, internalCounter))
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------
bool FFPTexturing::addVSFunctionInvocations(TextureUnitParams * textureUnitParams,
    Function * vsMain)
{
	FunctionInvocation* texCoordCalcFunc = NULL;


	switch (textureUnitParams->mTexCoordCalcMethod)
	{
	case TCP_NONE:
		if (textureUnitParams->mTextureMatrix.get() == NULL)
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_ASSIGN,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(textureUnitParams->mVSInputTexCoord, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		else
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_TRANSFORM_TEXCOORD,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(textureUnitParams->mTextureMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSInputTexCoord, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		break;

	case TCP_ENV_SPHERE:
	case TCP_ENV_REFLECTION:
		if (textureUnitParams->mTextureMatrix.get() == NULL)
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_SPHERE,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(mWorldITMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mViewMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputNormal, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		else
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_SPHERE,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(mWorldITMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mViewMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mTextureMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputNormal, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		break;


	case TCP_ENV_REFLECTION_INV:
		if (textureUnitParams->mTextureMatrix.get() == NULL)
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_REFLECT,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(mWorldMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mWorldITMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mViewMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputNormal, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputPos, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		else
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_REFLECT,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(mWorldMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mWorldITMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mViewMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mTextureMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputNormal, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputPos, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		break;

	case TCP_ENV_NORMAL:
		if (textureUnitParams->mTextureMatrix.get() == NULL)
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_NORMAL,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(mWorldITMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mViewMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputNormal, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		else
		{
			texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_NORMAL,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

			texCoordCalcFunc->pushOperand(mWorldITMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mViewMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mTextureMatrix, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(mVSInputNormal, Operand::OPS_IN);
			texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);
		}
		break;

	case TCP_ENV_PROJECTIVE:

		texCoordCalcFunc = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_PROJECTION,  FFP_VS_TEXTURING, textureUnitParams->mTextureSamplerIndex);

		texCoordCalcFunc->pushOperand(mWorldMatrix, Operand::OPS_IN);
		texCoordCalcFunc->pushOperand(textureUnitParams->mTextureViewProjImageMatrix, Operand::OPS_IN);
		texCoordCalcFunc->pushOperand(mVSInputPos, Operand::OPS_IN);
		texCoordCalcFunc->pushOperand(textureUnitParams->mVSOutputTexCoord, Operand::OPS_OUT);

		break;
	}

	if (texCoordCalcFunc != NULL)
		vsMain->addAtomInstance(texCoordCalcFunc);

	return true;
}
//-----------------------------------------------------------------------
bool FFPTexturing::addPSFunctionInvocations(TextureUnitParams * textureUnitParams,
    Function * psMain, int & internalCounter)
{
    TextureBlend * colourBlend = textureUnitParams->mTextureUnitState->getColourBlend();
    TextureBlend * alphaBlend  = textureUnitParams->mTextureUnitState->getAlphaBlend();
	ParameterPtr source1;
	ParameterPtr source2;
	int groupOrder = FFP_PS_TEXTURING;


	// Add texture sampling code.
	ParameterPtr texel = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, c_ParamTexelEx + StrConv::conv(textureUnitParams->mTextureSamplerIndex), GDT_Float4X);
	addPSSampleTexelInvocation(textureUnitParams, psMain, texel, FFP_PS_SAMPLING, internalCounter);

	// Build colour argument for source1.
	source1 = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "source1", GDT_Float4X);

	addPSArgumentInvocations(psMain, source1, texel,
		textureUnitParams->mTextureSamplerIndex,
		colourBlend->mColourSrc1, colourBlend->mConstant1,
		colourBlend->mConstant1.a, false, groupOrder, internalCounter);

	// Build colour argument for source2.
	source2 = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "source2", GDT_Float4X);

	addPSArgumentInvocations(psMain, source2, texel,
		textureUnitParams->mTextureSamplerIndex,
		colourBlend->mColourSrc2, colourBlend->mConstant2,
		colourBlend->mConstant2.a, false, groupOrder, internalCounter);

	bool needDifferentAlphaBlend = false;
	if (alphaBlend->mOP != colourBlend->mOP ||
		alphaBlend->mColourSrc1 != colourBlend->mColourSrc1 ||
		alphaBlend->mColourSrc2 != colourBlend->mColourSrc2 ||
		colourBlend->mColourSrc1 == TBS_CONSTANT ||
		colourBlend->mColourSrc2 == TBS_CONSTANT ||
		alphaBlend->mColourSrc1 == TBS_CONSTANT ||
		alphaBlend->mColourSrc2 == TBS_CONSTANT)
		needDifferentAlphaBlend = true;

	// Build colours blend
	addPSBlendInvocations(psMain, source1, source2, texel,
		textureUnitParams->mTextureSamplerIndex,
		colourBlend, groupOrder, internalCounter,
		needDifferentAlphaBlend ? (Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z )  : Operand::OPM_ALL);

	// Case we need different alpha channel code.
	if (needDifferentAlphaBlend)
	{
		// Build alpha argument for source1.
		addPSArgumentInvocations(psMain, source1, texel,
			textureUnitParams->mTextureSamplerIndex,
                alphaBlend->mColourSrc1, alphaBlend->mConstant1,
                    alphaBlend->mConstant1.a, true, groupOrder, internalCounter);

		// Build alpha argument for source2.
		addPSArgumentInvocations(psMain, source2, texel,
			textureUnitParams->mTextureSamplerIndex,
                alphaBlend->mColourSrc2, alphaBlend->mConstant2,
                    alphaBlend->mConstant2.a, true, groupOrder, internalCounter);

		// Build alpha blend
		addPSBlendInvocations(psMain, source1, source2, texel,
			textureUnitParams->mTextureSamplerIndex,
                alphaBlend, groupOrder, internalCounter, Operand::OPM_W);
	}
	return true;
}

//-----------------------------------------------------------------------
void FFPTexturing::addPSSampleTexelInvocation(TextureUnitParams* textureUnitParams, Function* psMain,
											  const ParameterPtr& texel, int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	if (textureUnitParams->mTexCoordCalcMethod == TCP_ENV_PROJECTIVE)
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_SAMPLE_TEXTURE_PROJ, groupOrder, internalCounter++);
	else
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_SAMPLE_TEXTURE, groupOrder, internalCounter++);

	curFuncInvocation->pushOperand(textureUnitParams->mTextureSampler, Operand::OPS_IN);
	curFuncInvocation->pushOperand(textureUnitParams->mPSInputTexCoord, Operand::OPS_IN);
	curFuncInvocation->pushOperand(texel, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);
}


//-----------------------------------------------------------------------
void FFPTexturing::addPSArgumentInvocations(Function * psMain, ParameterPtr arg,
    ParameterPtr texel, int samplerIndex, TextureBlendSource blendSrc,
        const Colour & colourValue, NIIf alphaValue, bool isAlphaArgument,
            const int groupOrder, int & internalCounter)
{
	FunctionInvocation * curFuncInvocation = NULL;

	switch(blendSrc)
	{
	case TBS_PRE_TEXTURE:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		if (samplerIndex == 0)
			curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
		else
			curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_IN);
		curFuncInvocation->pushOperand(arg, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBS_TEXTURE:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(texel, Operand::OPS_IN);
		curFuncInvocation->pushOperand(arg, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBS_DIFFUSE:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
		curFuncInvocation->pushOperand(arg, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBS_SPECULAR:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mPSSpecular, Operand::OPS_IN);
		curFuncInvocation->pushOperand(arg, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
		break;

	case TBS_CONSTANT:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_CONSTRUCT, groupOrder, internalCounter++);

		if (isAlphaArgument)
		{
			curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(alphaValue), Operand::OPS_IN);
		}
		else
		{
			curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(colourValue.r), Operand::OPS_IN);
			curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(colourValue.g), Operand::OPS_IN);
			curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(colourValue.b), Operand::OPS_IN);
			curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(colourValue.a), Operand::OPS_IN);
		}

		curFuncInvocation->pushOperand(arg, Operand::OPS_IN);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	}
}

//-----------------------------------------------------------------------
void FFPTexturing::addPSBlendInvocations(Function * psMain,
    ParameterPtr arg1, ParameterPtr arg2, ParameterPtr texel,
        int samplerIndex, const TextureBlend& blendMode,
            const int groupOrder, int& internalCounter, int targetChannels)
{
	FunctionInvocation* curFuncInvocation = NULL;

	switch(blendMode.mOP)
	{
	case TBO_OPERAND1:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_OPERAND2:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_MODULATE:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_MODULATE_2X:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATEX2, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_MODULATE_4X:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATEX4, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_ADD:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_ADD_SIGNED:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADDSIGNED, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_ADD_SMOOTH:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADDSMOOTH, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_SUB:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_SUBTRACT, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_ADD_DIFFUSE:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_SUBTRACT, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN, Operand::OPM_W);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_ADD_TEXTURE:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LERP, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(texel, Operand::OPS_IN, Operand::OPM_W);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_ADD_PRE_TEXTURE:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LERP, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);

		if (samplerIndex == 0)
			curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN, Operand::OPM_W);
		else
			curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_IN, Operand::OPM_W);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_CONSTANT_FACTOR:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LERP, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(blendMode.mConstantFactor), Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_DOT_PRODUCT:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_DOTPRODUCT, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	case TBO_ADD_SPECULAR:
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LERP, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(arg2, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(arg1, Operand::OPS_IN, targetChannels);
		curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT, targetChannels);
		psMain->addAtomInstance(curFuncInvocation);
		break;
	}
}

//-----------------------------------------------------------------------
TextureCoordPrc FFPTexturing::getTexCalcMethod(ShaderChTextureUnit * unit)
{
	TextureCoordPrc	texCoordCalcMethod = TCP_NONE;
    if(isEnvMap())
    {
        if(unit->getEnvMapType() == ShaderChTextureUnit::ENM_Sphere)
        {
            return TCP_ENV_SPHERE;
        }
        else if(unit->getEnvMapType() == ShaderChTextureUnit::ENM_Planar)
        {
            return TCP_ENV_REFLECTION;
        }
        else if(unit->getEnvMapType() == ShaderChTextureUnit::ENM_Reflection)
        {
            return TCP_ENV_REFLECTION_INV;
        }
        else if(unit->getEnvMapType() == ShaderChTextureUnit::ENM_Normal)
        {
            return TCP_ENV_NORMAL;
        }
    }

    if(isProjTexture())
    {
        return TCP_ENV_PROJECTIVE;
    }

	return texCoordCalcMethod;
}
//-----------------------------------------------------------------------
bool FFPTexturing::needsTextureMatrix(ShaderChTextureUnit * textureUnitState)
{
	const ShaderChTextureUnit::EffectMap & effectMap = textureUnitState->getEffects();
	ShaderChTextureUnit::EffectMap::const_iterator effi;
	for(effi = effectMap.begin(); effi != effectMap.end(); ++effi)
	{
		switch(effi->second.type)
		{
		case ShaderChTextureUnit::TTT_UV_OFT:
		case ShaderChTextureUnit::TTT_U_OFT:
		case ShaderChTextureUnit::TTT_V_OFT:
		case ShaderChTextureUnit::TTT_ROTATE:
		case ShaderChTextureUnit::TTT_FUNCTION:
			return true;
		}
	}

    if(isProjTexture())
        return true;
    if(isEnvMap())
        return true;

	const Matrix4 & matTexture = textureUnitState->getMatrix();
	// Resolve texture matrix parameter.
	if(matTexture != Matrix4::IDENTITY)
		return true;

	return false;
}
//-----------------------------------------------------------------------
void FFPTexturing::write(const SubRenderState & rhs)
{
	const FFPTexturing & rhsTexture = static_cast<const FFPTexturing &>(rhs);

	setTextureUnitCount(rhsTexture.getTextureUnitCount());

	for(unsigned int i=0; i < rhsTexture.getTextureUnitCount(); ++i)
	{
		setTextureUnit(i, rhsTexture.mTextureUnitParamsList[i].mTextureUnitState);
	}
}
//-----------------------------------------------------------------------
bool FFPTexturing::preAddToRenderState(const RenderState* renderState,
    ShaderCh * srcPass, ShaderCh * dstPass)
{
	//count the number of texture units we need to process
	size_t validTexUnits = 0;
	for(NCount i=0; i < srcPass->getTexture().getCount(); ++i)
	{
		if(isProcessingNeeded(srcPass->getTexture().get(i)))
		{
			++validTexUnits;
		}
	}

	setTextureUnitCount(validTexUnits);

	// Build texture stage sub states.
	for(NCount i=0; i < srcPass->getTexture().getCount(); ++i)
	{
		ShaderChTextureUnit* texUnitState = srcPass->getTexture().get(i);

		if(isProcessingNeeded(texUnitState))
		{
			setTextureUnit(i, texUnitState);
		}
	}
	return true;
}
//-----------------------------------------------------------------------
void FFPTexturing::updateGpuProgramsParams(GeometryObj * rend, ShaderCh * pass,
    const SysSyncParam * source, const LightList * pLightList)
{
	for(unsigned int i = 0; i < mTextureUnitParamsList.size(); ++i)
	{
		TextureUnitParams * curParams = &mTextureUnitParamsList[i];

		if(curParams->mTextureProjector != NULL && curParams->mTextureViewProjImageMatrix.get() != NULL)
		{
			Matrix4 matTexViewProjImage;
            Matrix4 temp;
            
            RenderSys * rsys;
            N_Engine().get(rsys);
            rsys->getSysProj(curParams->mTextureProjector->getProjMatrix(), temp, true);
        
			matTexViewProjImage = RenderSys::ProjClipSpace2DToImageSpace * temp *
				curParams->mTextureProjector->getViewMatrix();

			curParams->mTextureViewProjImageMatrix->setGpuParameter(matTexViewProjImage);
		}
	}
}
//-----------------------------------------------------------------------
void FFPTexturing::setTextureUnitCount(size_t count)
{
	mTextureUnitParamsList.resize(count);

	for (unsigned int i=0; i < count; ++i)
	{
		TextureUnitParams& curParams = mTextureUnitParamsList[i];

		curParams.mTextureUnitState				= NULL;
		curParams.mTextureProjector				= NULL;
		curParams.mTextureSamplerIndex			= 0;
		curParams.mTextureSamplerType			= GDT_Sampler2D;
		curParams.mVSInTextureCoordinateType	= GDT_Float2X;
		curParams.mVSOutTextureCoordinateType	= GDT_Float2X;
	}
}

//-----------------------------------------------------------------------
void FFPTexturing::setTextureUnit(unsigned short index, ShaderChTextureUnit* textureUnitState)
{
	if (index >= mTextureUnitParamsList.size())
	{
		N_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"FFPTexturing unit index out of bounds !!!");
	}

	if (textureUnitState->getFetchType() == VDF_VS)
	{
		N_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"FFP Texture unit does not support vertex texture fetch !!!");
	}


	TextureUnitParams& curParams = mTextureUnitParamsList[index];


	curParams.mTextureSamplerIndex = index;
	curParams.mTextureUnitState    = textureUnitState;

	switch (curParams.mTextureUnitState->getType())
	{
	case TEX_TYPE_1D:
		curParams.mTextureSamplerType = GDT_Sampler1D;
		curParams.mVSInTextureCoordinateType = GDT_Float;
		break;
	case TEX_TYPE_2D:
		curParams.mTextureSamplerType = GDT_Sampler2D;
		curParams.mVSInTextureCoordinateType = GDT_Float2X;
		break;
    case TEX_TYPE_2D_ARRAY:
        curParams.mTextureSamplerType = GDT_SamplerArray2D;
        curParams.mVSInTextureCoordinateType = GDT_Float3X;
        break;
	case TEX_TYPE_3D:
		curParams.mTextureSamplerType = GDT_Sampler3D;
		curParams.mVSInTextureCoordinateType = GDT_Float3X;
		break;
	case TEX_TYPE_CUBE_MAP:
		curParams.mTextureSamplerType = GDT_SamplerCube;
		curParams.mVSInTextureCoordinateType = GDT_Float3X;
		break;
	}

	 curParams.mVSOutTextureCoordinateType = curParams.mVSInTextureCoordinateType;
	 curParams.mTexCoordCalcMethod = getTexCalcMethod(curParams.mTextureUnitState);

	 if (curParams.mTexCoordCalcMethod == TCP_ENV_PROJECTIVE)
		 curParams.mVSOutTextureCoordinateType = GDT_Float3X;
}

//-----------------------------------------------------------------------
bool FFPTexturing::isProcessingNeeded(ShaderChTextureUnit* texUnitState)
{
	return texUnitState->getFetchType() == VDF_FS;
}


//-----------------------------------------------------------------------
const String & FFPTexturingFactory::getType() const
{
	return FFPTexturing::Type;
}

//-----------------------------------------------------------------------
SubRenderState *	FFPTexturingFactory::createInstance(ScriptCompiler * compiler,
    PropertyScriptNode * prop, ShaderCh* pass, SGScriptAnalyze * translator)
{
	if (prop->name == "texturing_stage")
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
void FFPTexturingFactory::writeInstance(MaterialSerializer* ser, SubRenderState* subRenderState,
    ShaderCh * srcPass, ShaderCh * dstPass)
{
	ser->writeAttribute(4, "texturing_stage");
	ser->writeValue("ffp");
}

//-----------------------------------------------------------------------
SubRenderState*	FFPTexturingFactory::createInstanceImpl()
{
	return N_new FFPTexturing;
}


}
}

#endif
