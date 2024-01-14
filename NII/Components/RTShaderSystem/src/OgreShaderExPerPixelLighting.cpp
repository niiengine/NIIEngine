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
#include "OgreShaderExPerPixelLighting.h"
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreGpuProgram.h"
#include "OgrePass.h"
#include "OgreShaderGenerator.h"
#include "OgreSceneManager.h"
#include "OgreViewport.h"

namespace Ogre {
namespace RTShader {

/************************************************************************/
/*                                                                      */
/************************************************************************/
String PerPixelLighting::Type = "SGX_PerPixelLighting";
Light PerPixelLighting::msBlankLight;

//-----------------------------------------------------------------------
PerPixelLighting::PerPixelLighting()
{
	mTrackVertexColourType = CM_T_NONE;
	mSpecularEnable = false;

	msBlankLight.setDiffuse(Colour::Black);
	msBlankLight.setSpecular(Colour::Black);
    msBlankLight.setRange(0);
    msBlankLight.setAttenuationConstant(1);
    msBlankLight.setAttenuationLinear(0);
    msBlankLight.setAttenuationQuadric(0);
}
//-----------------------------------------------------------------------
const String & PerPixelLighting::getType() const
{
	return Type;
}
//-----------------------------------------------------------------------
int	PerPixelLighting::getExecutionOrder() const
{
	return FFP_LIGHTING;
}
//-----------------------------------------------------------------------
void PerPixelLighting::updateGpuProgramsParams(GeometryObj* rend, ShaderCh * pass,
    const SysSyncParam * source, const LightList * pLightList)
{
	if (mLightParamsList.size() == 0)
		return;

	const Matrix4 & matView = source->getViewMatrix();
	LightType curLightType = LT_DIRECTIONAL;
	unsigned int curSearchLightIndex = 0;

	// Update per light parameters.
	for(unsigned int i = 0; i < mLightParamsList.size(); ++i)
	{
		const LightParams & curParams = mLightParamsList[i];

		if(curLightType != curParams.mType)
		{
			curLightType = curParams.mType;
			curSearchLightIndex = 0;
		}

		Light * srcLight = NULL;
		Vector4 vParameter;
		Colour colour;

		// Search a matching light from the current sorted lights of the given renderable.
		for (unsigned int j = curSearchLightIndex; j < pLightList->size(); ++j)
		{
			if (pLightList->at(j)->getType() == curLightType)
			{
				srcLight = pLightList->at(j);
				curSearchLightIndex = j + 1;
				break;
			}
		}

		// No matching light found -> use a blank dummy light for parameter update.
		if (srcLight == NULL)
		{
			srcLight = &msBlankLight;
		}

		switch (curParams.mType)
		{
		case LT_DIRECTIONAL:

			// Update light direction.
			vParameter = matView.affine(srcLight->getRelSpace4());
			curParams.mDirection->setGpuParameter(vParameter);
			break;

		case LT_POINT:

			// Update light position.
			vParameter = matView.affine(srcLight->getRelSpace4());
			curParams.mPosition->setGpuParameter(vParameter);

			// Update light attenuation parameters.
			vParameter.x = srcLight->getRange();
			vParameter.y = srcLight->getAttenuationConstant();
			vParameter.z = srcLight->getAttenuationLinear();
			vParameter.w = srcLight->getAttenuationQuadric();
			curParams.mAttenuatParams->setGpuParameter(vParameter);
			break;

		case LT_SPOT:
			{
				Vector3 vec3;
				Matrix3 matViewIT;

				// Update light position.
				vParameter = matView.affine(srcLight->getRelSpace4());
				curParams.mPosition->setGpuParameter(vParameter);

				// Update light direction.
				matViewIT = source->getInverseTransposeViewMatrix();
				vec3 = matViewIT * srcLight->getSpaceDirection();
				vec3.normalise();

				vParameter.x = -vec3.x;
				vParameter.y = -vec3.y;
				vParameter.z = -vec3.z;
				vParameter.w = 0.0;
				curParams.mDirection->setGpuParameter(vParameter);

				// Update light attenuation parameters.
				vParameter.x = srcLight->getRange();
				vParameter.y = srcLight->getAttenuationConstant();
				vParameter.z = srcLight->getAttenuationLinear();
				vParameter.w = srcLight->getAttenuationQuadric();
				curParams.mAttenuatParams->setGpuParameter(vParameter);

				// Update spotlight parameters.
				NIIf phi   = Math::Cos(srcLight->getSpotlightOuter().getValue() * 0.5f);
				NIIf theta = Math::Cos(srcLight->getSpotlightInner().getValue() * 0.5f);

				vec3.x = theta;
				vec3.y = phi;
				vec3.z = srcLight->getSpotlightFalloff();

				curParams.mSpotParams->setGpuParameter(vec3);
			}
			break;
		}

		// Update diffuse colour.
		if ((mTrackVertexColourType & CM_T_DIFFUSE) == 0)
		{
			colour = srcLight->getDiffuse() * pass->getColour(ShaderCh::SOT_Get)->getDiffuse();
			curParams.mDiffuseColour->setGpuParameter(colour);
		}
		else
		{
			colour = srcLight->getDiffuse();
			curParams.mDiffuseColour->setGpuParameter(colour);
		}

		// Update specular colour if need to.
		if(mSpecularEnable)
		{
			// Update diffuse colour.
			if((mTrackVertexColourType & CM_T_SPECULAR) == 0)
			{
				colour = srcLight->getSpecular() * pass->getColour(ShaderCh::SOT_Get)->getSpecular();
				curParams.mSpecularColour->setGpuParameter(colour);
			}
			else
			{
				colour = srcLight->getSpecular();
				curParams.mSpecularColour->setGpuParameter(colour);
			}
		}
	}
}

//-----------------------------------------------------------------------
bool PerPixelLighting::resolveParameters(ProgramSet* programSet)
{
	if (false == resolveGlobalParameters(programSet))
		return false;

	if (false == resolvePerLightParameters(programSet))
		return false;

	return true;
}

//-----------------------------------------------------------------------
bool PerPixelLighting::resolveGlobalParameters(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();


	// Resolve world view IT matrix.
	mWorldViewITMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space_InverseTranspose, 0);
	if (mWorldViewITMatrix.get() == NULL)
		return false;

	// Get surface ambient colour if need to.
	if ((mTrackVertexColourType & CM_T_AMBIENT) == 0)
	{
		mDerivedAmbientLightColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Ambient_X_Env, 0);
		if (mDerivedAmbientLightColour.get() == NULL)
			return false;
	}
	else
	{
		mLightAmbientColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ambient, 0);
		if (mLightAmbientColour.get() == NULL)
			return false;

		mSurfaceAmbientColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Ambient, 0);
		if (mSurfaceAmbientColour.get() == NULL)
			return false;

	}

	// Get surface diffuse colour if need to.
	if ((mTrackVertexColourType & CM_T_DIFFUSE) == 0)
	{
		mSurfaceDiffuseColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Diffuse, 0);
		if (mSurfaceDiffuseColour.get() == NULL)
			return false;
	}

	// Get surface specular colour if need to.
	if ((mTrackVertexColourType & CM_T_SPECULAR) == 0)
	{
		mSurfaceSpecularColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Specular, 0);
		if (mSurfaceSpecularColour.get() == NULL)
			return false;
	}

	// Get surface emissive colour if need to.
	if((mTrackVertexColourType & CM_T_EMISSIVE) == 0)
	{
		mSurfaceEmissiveColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Emissive, 0);
		if (mSurfaceEmissiveColour.get() == NULL)
			return false;
	}

	// Get derived scene colour.
	mDerivedSceneColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Emissive_Evn, 0);
	if(mDerivedSceneColour.get() == NULL)
		return false;

	// Get surface shininess.
	mSurfaceShininess = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Shininess, 0);
	if (mSurfaceShininess.get() == NULL)
		return false;

	// Resolve input vertex shader normal.
	mVSInNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
	if (mVSInNormal.get() == NULL)
		return false;

	// Resolve output vertex shader normal.
	mVSOutNormal = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, Parameter::SPC_NORMAL_VIEW_SPACE, GDT_Float3X);
	if (mVSOutNormal.get() == NULL)
		return false;

	// Resolve input pixel shader normal.
	mPSInNormal = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
		mVSOutNormal->getIndex(),
		mVSOutNormal->getContent(),
		GDT_Float3X);
	if (mPSInNormal.get() == NULL)
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

	mPSOutDiffuse = psMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
	if (mPSOutDiffuse.get() == NULL)
		return false;

	mPSTempDiffuseColour = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lPerPixelDiffuse", GDT_Float4X);
	if (mPSTempDiffuseColour.get() == NULL)
		return false;

	if (mSpecularEnable)
	{
		mPSSpecular = psMain->getParameterByContent(inputParams, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
		if (mPSSpecular.get() == NULL)
		{
			mPSSpecular = psMain->getParameterByContent(localParams, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
			if (mPSSpecular.get() == NULL)
				return false;
		}

		mPSTempSpecularColour = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lPerPixelSpecular", GDT_Float4X);
		if (mPSTempSpecularColour.get() == NULL)
			return false;


		mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
		if (mVSInPosition.get() == NULL)
			return false;

		mVSOutViewPos = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, Parameter::SPC_POSITION_VIEW_SPACE, GDT_Float3X);
		if (mVSOutViewPos.get() == NULL)
			return false;

		mPSInViewPos = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
			mVSOutViewPos->getIndex(),
			mVSOutViewPos->getContent(),
			GDT_Float3X);
		if (mPSInViewPos.get() == NULL)
			return false;

		mWorldViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space, 0);
		if (mWorldViewMatrix.get() == NULL)
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------
bool PerPixelLighting::resolvePerLightParameters(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();


	// Resolve per light parameters.
	for (unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		switch (mLightParamsList[i].mType)
		{
		case LT_DIRECTIONAL:
			mLightParamsList[i].mDirection = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_direction_view_space");
			if (mLightParamsList[i].mDirection.get() == NULL)
				return false;
			break;

		case LT_POINT:
			mWorldViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space, 0);
			if (mWorldViewMatrix.get() == NULL)
				return false;

			mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0,  Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInPosition.get() == NULL)
				return false;

			mLightParamsList[i].mPosition = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_position_view_space");
			if (mLightParamsList[i].mPosition.get() == NULL)
				return false;

			mLightParamsList[i].mAttenuatParams = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_attenuation");
			if (mLightParamsList[i].mAttenuatParams.get() == NULL)
				return false;

			if (mVSOutViewPos.get() == NULL)
			{
				mVSOutViewPos = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, Parameter::SPC_POSITION_VIEW_SPACE, GDT_Float3X);
				if (mVSOutViewPos.get() == NULL)
					return false;

				mPSInViewPos = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
					mVSOutViewPos->getIndex(),
					mVSOutViewPos->getContent(),
					GDT_Float3X);
				if (mPSInViewPos.get() == NULL)
					return false;
			}
			break;

		case LT_SPOT:
			mWorldViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space, 0);
			if (mWorldViewMatrix.get() == NULL)
				return false;

			mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInPosition.get() == NULL)
				return false;

			mLightParamsList[i].mPosition = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_position_view_space");
			if (mLightParamsList[i].mPosition.get() == NULL)
				return false;

			mLightParamsList[i].mDirection = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_direction_view_space");
			if (mLightParamsList[i].mDirection.get() == NULL)
				return false;

			mLightParamsList[i].mAttenuatParams = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_attenuation");
			if (mLightParamsList[i].mAttenuatParams.get() == NULL)
				return false;

			mLightParamsList[i].mSpotParams = psProgram->resolveParameter(GDT_Float3X, -1, GPT_Light, "spotlight_params");
			if (mLightParamsList[i].mSpotParams.get() == NULL)
				return false;

			if (mVSOutViewPos.get() == NULL)
			{
				mVSOutViewPos = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, Parameter::SPC_POSITION_VIEW_SPACE, GDT_Float3X);
				if (mVSOutViewPos.get() == NULL)
					return false;

				mPSInViewPos = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
					mVSOutViewPos->getIndex(),
					mVSOutViewPos->getContent(),
					GDT_Float3X);
				if (mPSInViewPos.get() == NULL)
					return false;
			}
			break;
		}

		// Resolve diffuse colour.
		if ((mTrackVertexColourType & CM_T_DIFFUSE) == 0)
		{
			mLightParamsList[i].mDiffuseColour = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light | GPT_Render, "derived_light_diffuse");
			if (mLightParamsList[i].mDiffuseColour.get() == NULL)
				return false;
		}
		else
		{
			mLightParamsList[i].mDiffuseColour = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_diffuse");
			if (mLightParamsList[i].mDiffuseColour.get() == NULL)
				return false;
		}

		if (mSpecularEnable)
		{
			// Resolve specular colour.
			if ((mTrackVertexColourType & CM_T_SPECULAR) == 0)
			{
				mLightParamsList[i].mSpecularColour = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light | GPT_Render, "derived_light_specular");
				if (mLightParamsList[i].mSpecularColour.get() == NULL)
					return false;
			}
			else
			{
				mLightParamsList[i].mSpecularColour = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_specular");
				if (mLightParamsList[i].mSpecularColour.get() == NULL)
					return false;
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------
bool PerPixelLighting::resolveDependencies(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(SGX_LIB_PERPIXELLIGHTING);

	psProgram->addDependency(FFP_LIB_COMMON);
	psProgram->addDependency(SGX_LIB_PERPIXELLIGHTING);

	return true;
}

//-----------------------------------------------------------------------
bool PerPixelLighting::addFunctionInvocations(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* psMain = psProgram->getEntryPointFunction();

	int internalCounter = 0;


	// Add the global illumination functions.
	if (false == addVSInvocation(vsMain, FFP_VS_LIGHTING, internalCounter))
		return false;


	internalCounter = 0;

	// Add the global illumination functions.
	if (false == addPSGlobalIlluminationInvocation(psMain, FFP_PS_COLOUR_BEGIN + 1, internalCounter))
		return false;


	// Add per light functions.
	for (unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		if (false == addPSIlluminationInvocation(&mLightParamsList[i], psMain, FFP_PS_COLOUR_BEGIN + 1, internalCounter))
			return false;
	}

	// Assign back temporary variables to the ps diffuse and specular components.
	if (false == addPSFinalAssignmentInvocation(psMain, FFP_PS_COLOUR_BEGIN + 1, internalCounter))
		return false;


	return true;
}

//-----------------------------------------------------------------------
bool PerPixelLighting::addVSInvocation(Function* vsMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	// Transform normal in view space.
	curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMNORMAL, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mWorldViewITMatrix, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mVSOutNormal, Operand::OPS_OUT);
	vsMain->addAtomInstance(curFuncInvocation);

	// Transform view space position if need to.
	if (mVSOutViewPos.get() != NULL)
	{
		curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMPOSITION, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mWorldViewMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSOutViewPos, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}


	return true;
}


//-----------------------------------------------------------------------
bool PerPixelLighting::addPSGlobalIlluminationInvocation(Function* psMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	if ((mTrackVertexColourType & CM_T_AMBIENT) == 0 &&
		(mTrackVertexColourType & CM_T_EMISSIVE) == 0)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mDerivedSceneColour, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
	}
	else
	{
		if (mTrackVertexColourType & CM_T_AMBIENT)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mLightAmbientColour, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT);
			psMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mDerivedAmbientLightColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			psMain->addAtomInstance(curFuncInvocation);
		}

		if (mTrackVertexColourType & CM_T_EMISSIVE)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT);
			psMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mSurfaceEmissiveColour, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT);
			psMain->addAtomInstance(curFuncInvocation);
		}
	}

	if (mSpecularEnable)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mPSSpecular, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
	}

	return true;
}

//-----------------------------------------------------------------------
bool PerPixelLighting::addPSIlluminationInvocation(LightParams* curLightParams, Function* psMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	// Merge diffuse colour with vertex colour if need to.
	if (mTrackVertexColourType & CM_T_DIFFUSE)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		psMain->addAtomInstance(curFuncInvocation);
	}

	// Merge specular colour with vertex colour if need to.
	if (mSpecularEnable && mTrackVertexColourType & CM_T_SPECULAR)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		psMain->addAtomInstance(curFuncInvocation);
	}

	switch (curLightParams->mType)
	{

	case LT_DIRECTIONAL:
		if (mSpecularEnable)
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_LIGHT_DIRECTIONAL_DIFFUSESPECULAR, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mPSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInViewPos, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mSurfaceShininess, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			psMain->addAtomInstance(curFuncInvocation);
		}

		else
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_LIGHT_DIRECTIONAL_DIFFUSE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mPSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			psMain->addAtomInstance(curFuncInvocation);
		}
		break;

	case LT_POINT:
		if (mSpecularEnable)
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_LIGHT_POINT_DIFFUSESPECULAR, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mPSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInViewPos, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mSurfaceShininess, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			psMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_LIGHT_POINT_DIFFUSE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mPSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInViewPos, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			psMain->addAtomInstance(curFuncInvocation);
		}

		break;

	case LT_SPOT:
		if (mSpecularEnable)
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_LIGHT_SPOT_DIFFUSESPECULAR, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mPSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInViewPos, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mSpotParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mSurfaceShininess, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			psMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_LIGHT_SPOT_DIFFUSE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mPSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInViewPos, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mSpotParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			psMain->addAtomInstance(curFuncInvocation);
		}
		break;
	}

	return true;
}

//-----------------------------------------------------------------------
bool PerPixelLighting::addPSFinalAssignmentInvocation( Function* psMain, const int groupOrder, int& internalCounter )
{
	FunctionInvocation* curFuncInvocation;

	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_PS_COLOUR_BEGIN + 1, internalCounter++);
	curFuncInvocation->pushOperand(mPSTempDiffuseColour, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);

	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_PS_COLOUR_BEGIN + 1, internalCounter++);
	curFuncInvocation->pushOperand(mPSDiffuse, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);

	if (mSpecularEnable)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_PS_COLOUR_BEGIN + 1, internalCounter++);
		curFuncInvocation->pushOperand(mPSTempSpecularColour, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mPSSpecular, Operand::OPS_OUT);
		psMain->addAtomInstance(curFuncInvocation);
	}

	return true;
}


//-----------------------------------------------------------------------
void PerPixelLighting::write(const SubRenderState& rhs)
{
	const PerPixelLighting& rhsLighting = static_cast<const PerPixelLighting&>(rhs);

	int lightCount[3];

	rhsLighting.getLightCount(lightCount);
	setLightCount(lightCount);
}

//-----------------------------------------------------------------------
bool PerPixelLighting::preAddToRenderState(const RenderState * renderState,
    ShaderCh * srcPass, ShaderCh * dstPass)
{
	if(srcPass->isLight() == false)
		return false;

	int lightCount[3];

	renderState->getLightCount(lightCount);

	setTrackVertexColourType(srcPass->getColour(ShaderCh::SOT_Get)->getColourTracking());

	if(srcPass->getColour(ShaderCh::SOT_Get)->getShininess() > 0.0 && srcPass->getColour(ShaderCh::SOT_Get)->getSpecular() != Colour::Black)
	{
		setSpecularEnable(true);
	}
	else
	{
		setSpecularEnable(false);
	}

	// Case this pass should run once per light(s) -> override the light policy.
	if(srcPass->isEachLight())
	{
		// This is the preferred case -> only one type of light is handled.
		if(srcPass->getLightTypeLimit() != LT_NONE)
		{
			if (srcPass->getLightTypeLimit() == LT_POINT)
			{
				lightCount[0] = srcPass->getLightCountLimit();
				lightCount[1] = 0;
				lightCount[2] = 0;
			}
			else if (srcPass->getLightTypeLimit() == LT_DIRECTIONAL)
			{
				lightCount[0] = 0;
				lightCount[1] = srcPass->getLightCountLimit();
				lightCount[2] = 0;
			}
			else if (srcPass->getLightTypeLimit() == LT_SPOT)
			{
				lightCount[0] = 0;
				lightCount[1] = 0;
				lightCount[2] = srcPass->getLightCountLimit();
			}
		}

		// This is worse case -> all light types expected to be handled.
		// Can not handle this request in efficient way - throw an exception.
		else
		{
			N_EXCEPT(Exception::ERR_INVALIDPARAMS,
				"Using iterative lighting method with RT Shader System requires specifieng explicit light type.",
				"PerPixelLighting::preAddToRenderState");
		}
	}

	setLightCount(lightCount);

	return true;
}

//-----------------------------------------------------------------------
void PerPixelLighting::setLightCount(const int lightCount[3])
{
	for (int type=0; type < 3; ++type)
	{
		for (int i=0; i < lightCount[type]; ++i)
		{
			LightParams curParams;

			if (type == 0)
				curParams.mType = LT_POINT;
			else if (type == 1)
				curParams.mType = LT_DIRECTIONAL;
			else if (type == 2)
				curParams.mType = LT_SPOT;

			mLightParamsList.push_back(curParams);
		}
	}
}

//-----------------------------------------------------------------------
void PerPixelLighting::getLightCount(int lightCount[3]) const
{
	lightCount[0] = 0;
	lightCount[1] = 0;
	lightCount[2] = 0;

	for (unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		const LightParams curParams = mLightParamsList[i];

		if (curParams.mType == LT_POINT)
			lightCount[0]++;
		else if (curParams.mType == LT_DIRECTIONAL)
			lightCount[1]++;
		else if (curParams.mType == LT_SPOT)
			lightCount[2]++;
	}
}
//-----------------------------------------------------------------------
const String & PerPixelLightingFactory::getType() const
{
	return PerPixelLighting::Type;
}
//-----------------------------------------------------------------------
SubRenderState*	PerPixelLightingFactory::createInstance(ScriptCompiler* compiler,
 PropertyScriptNode* prop, ShaderCh* pass, SGScriptAnalyze* translator)
{
	if (prop->name == "lighting_stage")
	{
		if(prop->values.size() == 1)
		{
			String modelType;

			if(false == SGScriptAnalyze::get(prop->values.front(), &modelType))
			{
				compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
				return NULL;
			}

			if (modelType == "per_pixel")
			{
				return createOrRetrieveInstance(translator);
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------
void PerPixelLightingFactory::writeInstance(MaterialSerializer * ser,
    SubRenderState * subRenderState, ShaderCh * srcPass, ShaderCh * dstPass)
{
	ser->writeAttribute(4, "lighting_stage");
	ser->writeValue("per_pixel");
}

//-----------------------------------------------------------------------
SubRenderState*	PerPixelLightingFactory::createInstanceImpl()
{
	return N_new PerPixelLighting;
}

}
}

#endif
