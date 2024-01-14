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
#include "OgreShaderFFPLighting.h"
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreGpuProgram.h"
#include "OgrePass.h"
#include "OgreShaderGenerator.h"
#include "OgreSceneManager.h"
#include "OgreViewport.h"


namespace Ogre 
{
namespace RTShader 
{
/************************************************************************/
/*                                                                      */
/************************************************************************/
String FFPLighting::Type = "FFP_Lighting";
Light FFPLighting::msBlankLight;

//-----------------------------------------------------------------------
FFPLighting::FFPLighting()
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
const String & FFPLighting::getType() const
{
	return Type;
}
//-----------------------------------------------------------------------
int	FFPLighting::getExecutionOrder() const
{
	return FFP_LIGHTING;
}
//-----------------------------------------------------------------------
void FFPLighting::updateGpuProgramsParams(GeometryObj * rend, ShaderCh * pass,
    const SysSyncParam * source,  const LightList * pLightList)
{
	if(mLightParamsList.size() == 0)
		return;

	const Matrix4 & matView = source->getViewMatrix();
	LightType curLightType = LT_DIRECTIONAL;
	unsigned int curSearchLightIndex = 0;

	// Update per light parameters.
	for(unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		const LightParams& curParams = mLightParamsList[i];

		if(curLightType != curParams.mType)
		{
			curLightType = curParams.mType;
			curSearchLightIndex = 0;
		}

		Light*		srcLight = NULL;
		Vector4		vParameter;
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
			Matrix3 matViewIT(source->getInverseTransposeViewMatrix());

			// Update light position.
			vParameter = matView.affine(srcLight->getRelSpace4());
			curParams.mPosition->setGpuParameter(vParameter);


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
		if (mSpecularEnable)
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
bool FFPLighting::resolveParameters(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();

	// Resolve world view IT matrix.
	mWorldViewITMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space_InverseTranspose, 0);
	if (mWorldViewITMatrix.get() == NULL)
		return false;

	// Get surface ambient colour if need to.
	if ((mTrackVertexColourType & CM_T_AMBIENT) == 0)
	{
		mDerivedAmbientLightColour = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Ambient_X_Env, 0);
		if (mDerivedAmbientLightColour.get() == NULL)
			return false;
	}
	else
	{
		mLightAmbientColour = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ambient, 0);
		if (mLightAmbientColour.get() == NULL)
			return false;

		mSurfaceAmbientColour = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Ambient, 0);
		if (mSurfaceAmbientColour.get() == NULL)
			return false;

	}

	// Get surface diffuse colour if need to.
	if ((mTrackVertexColourType & CM_T_DIFFUSE) == 0)
	{
		mSurfaceDiffuseColour = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Diffuse, 0);
		if (mSurfaceDiffuseColour.get() == NULL)
			return false;
	}

	// Get surface specular colour if need to.
	if ((mTrackVertexColourType & CM_T_SPECULAR) == 0)
	{
		mSurfaceSpecularColour = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Specular, 0);
		if (mSurfaceSpecularColour.get() == NULL)
			return false;
	}


	// Get surface emissive colour if need to.
	if ((mTrackVertexColourType & CM_T_EMISSIVE) == 0)
	{
		mSurfaceEmissiveColour = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Emissive, 0);
		if (mSurfaceEmissiveColour.get() == NULL)
			return false;
	}

	// Get derived scene colour.
	mDerivedSceneColour = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Emissive_Evn, 0);
	if (mDerivedSceneColour.get() == NULL)
		return false;

	// Get surface shininess.
	mSurfaceShininess = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Shininess, 0);
	if (mSurfaceShininess.get() == NULL)
		return false;

	// Resolve input vertex shader normal.
	mVSInNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
	if (mVSInNormal.get() == NULL)
		return false;

	if (mTrackVertexColourType != 0)
	{
		mVSDiffuse = vsMain->resolveInputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
		if (mVSDiffuse.get() == NULL)
			return false;
	}


	// Resolve output vertex shader diffuse colour.
	mVSOutDiffuse = vsMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
	if (mVSOutDiffuse.get() == NULL)
		return false;




	// Resolve per light parameters.
	for (unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		switch (mLightParamsList[i].mType)
		{
		case LT_DIRECTIONAL:
			mLightParamsList[i].mDirection = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_position_view_space");
			if (mLightParamsList[i].mDirection.get() == NULL)
				return false;
			break;

		case LT_POINT:
			mWorldViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space, 0);
			if (mWorldViewMatrix.get() == NULL)
				return false;

			mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInPosition.get() == NULL)
				return false;

			mLightParamsList[i].mPosition = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_position_view_space");
			if (mLightParamsList[i].mPosition.get() == NULL)
				return false;

			mLightParamsList[i].mAttenuatParams = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_attenuation");
			if (mLightParamsList[i].mAttenuatParams.get() == NULL)
				return false;
			break;

		case LT_SPOT:
			mWorldViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space, 0);
			if (mWorldViewMatrix.get() == NULL)
				return false;

			mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInPosition.get() == NULL)
				return false;

			mLightParamsList[i].mPosition = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_position_view_space");
			if (mLightParamsList[i].mPosition.get() == NULL)
				return false;

			mLightParamsList[i].mDirection = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_direction_view_space");
			if (mLightParamsList[i].mDirection.get() == NULL)
				return false;

			mLightParamsList[i].mAttenuatParams = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_attenuation");
			if (mLightParamsList[i].mAttenuatParams.get() == NULL)
				return false;

			mLightParamsList[i].mSpotParams = vsProgram->resolveParameter(GDT_Float3X, -1, GPT_Light, "spotlight_params");
			if (mLightParamsList[i].mSpotParams.get() == NULL)
				return false;
			break;
		}

		// Resolve diffuse colour.
		if ((mTrackVertexColourType & CM_T_DIFFUSE) == 0)
		{
			mLightParamsList[i].mDiffuseColour = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Render | GPT_Light, "derived_light_diffuse");
			if (mLightParamsList[i].mDiffuseColour.get() == NULL)
				return false;
		}
		else
		{
			mLightParamsList[i].mDiffuseColour = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_diffuse");
			if (mLightParamsList[i].mDiffuseColour.get() == NULL)
				return false;
		}

		if (mSpecularEnable)
		{
			// Resolve specular colour.
			if ((mTrackVertexColourType & CM_T_SPECULAR) == 0)
			{
				mLightParamsList[i].mSpecularColour = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Render | GPT_Light, "derived_light_specular");
				if (mLightParamsList[i].mSpecularColour.get() == NULL)
					return false;
			}
			else
			{
				mLightParamsList[i].mSpecularColour = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_specular");
				if (mLightParamsList[i].mSpecularColour.get() == NULL)
					return false;
			}

			if (mVSOutSpecular.get() == NULL)
			{
				mVSOutSpecular = vsMain->resolveOutputParameter(Parameter::SPS_COLOR, 1, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
				if (mVSOutSpecular.get() == NULL)
					return false;
			}

			if (mVSInPosition.get() == NULL)
			{
				mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
				if (mVSInPosition.get() == NULL)
					return false;
			}

			if (mWorldViewMatrix.get() == NULL)
			{
				mWorldViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space, 0);
				if (mWorldViewMatrix.get() == NULL)
					return false;
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------
bool FFPLighting::resolveDependencies(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(FFP_LIB_LIGHTING);

	return true;
}

//-----------------------------------------------------------------------
bool FFPLighting::addFunctionInvocations(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();

	int internalCounter = 0;

	// Add the global illumination functions.
	if (false == addGlobalIlluminationInvocation(vsMain, FFP_VS_LIGHTING, internalCounter))
		return false;


	// Add per light functions.
	for (unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		if (false == addIlluminationInvocation(&mLightParamsList[i], vsMain, FFP_VS_LIGHTING, internalCounter))
			return false;
	}



	return true;
}

//-----------------------------------------------------------------------
bool FFPLighting::addGlobalIlluminationInvocation(Function* vsMain,
    const int groupOrder, int & internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	if ((mTrackVertexColourType & CM_T_AMBIENT) == 0 &&
		(mTrackVertexColourType & CM_T_EMISSIVE) == 0)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mDerivedSceneColour, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}
	else
	{
		if (mTrackVertexColourType & CM_T_AMBIENT)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mLightAmbientColour, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSDiffuse, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mDerivedAmbientLightColour, Operand::OPS_IN, (Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT, (Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
			vsMain->addAtomInstance(curFuncInvocation);
		}

		if (mTrackVertexColourType & CM_T_EMISSIVE)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mVSDiffuse, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mSurfaceEmissiveColour, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}
	}

	return true;
}

//-----------------------------------------------------------------------
bool FFPLighting::addIlluminationInvocation(LightParams* curLightParams, Function* vsMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	// Merge diffuse colour with vertex colour if need to.
	if (mTrackVertexColourType & CM_T_DIFFUSE)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mVSDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		vsMain->addAtomInstance(curFuncInvocation);
	}

	// Merge specular colour with vertex colour if need to.
	if (mSpecularEnable && mTrackVertexColourType & CM_T_SPECULAR)
	{
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_MODULATE, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mVSDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		vsMain->addAtomInstance(curFuncInvocation);
	}

	switch (curLightParams->mType)
	{

	case LT_DIRECTIONAL:
		if (mSpecularEnable)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LIGHT_DIRECTIONAL_DIFFUSESPECULAR, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mWorldViewMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mWorldViewITMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mSurfaceShininess, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutSpecular, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutSpecular, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			vsMain->addAtomInstance(curFuncInvocation);
		}

		else
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LIGHT_DIRECTIONAL_DIFFUSE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mWorldViewITMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			vsMain->addAtomInstance(curFuncInvocation);
		}
		break;

	case LT_POINT:
		if (mSpecularEnable)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LIGHT_POINT_DIFFUSESPECULAR, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mWorldViewMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mWorldViewITMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mSurfaceShininess, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutSpecular, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutSpecular, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			vsMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LIGHT_POINT_DIFFUSE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mWorldViewMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mWorldViewITMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			vsMain->addAtomInstance(curFuncInvocation);
		}

		break;

	case LT_SPOT:
		if (mSpecularEnable)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LIGHT_SPOT_DIFFUSESPECULAR, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mWorldViewMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mWorldViewITMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mSpotParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mSpecularColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mSurfaceShininess, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutSpecular, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutSpecular, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			vsMain->addAtomInstance(curFuncInvocation);
		}
		else
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_LIGHT_SPOT_DIFFUSE, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mWorldViewMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mWorldViewITMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mAttenuatParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mSpotParams, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDiffuseColour, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(mVSOutDiffuse, Operand::OPS_OUT, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			vsMain->addAtomInstance(curFuncInvocation);
		}
		break;
	}

	return true;
}


//-----------------------------------------------------------------------
void FFPLighting::write(const SubRenderState& rhs)
{
	const FFPLighting& rhsLighting = static_cast<const FFPLighting&>(rhs);

	int lightCount[3];

	rhsLighting.getLightCount(lightCount);
	setLightCount(lightCount);
}

//-----------------------------------------------------------------------
bool FFPLighting::preAddToRenderState(const RenderState * renderState,
    ShaderCh * srcPass, ShaderCh * dstPass)
{
	if(srcPass->isLight() == false)
		return false;

	int lightCount[3];

	renderState->getLightCount(lightCount);

	setTrackVertexColourType(srcPass->getColour(ShaderCh::SOT_Get)->getColourTracking());

	if(srcPass->getColour(ShaderCh::SOT_Get)->getShininess() > 0.0 &&
        srcPass->getColour(ShaderCh::SOT_Get)->getSpecular() != Colour::Black)
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
			if(srcPass->getLightTypeLimit() == LT_POINT)
			{
				lightCount[0] = srcPass->getLightCountLimit();
				lightCount[1] = 0;
				lightCount[2] = 0;
			}
			else if(srcPass->getLightTypeLimit() == LT_DIRECTIONAL)
			{
				lightCount[0] = 0;
				lightCount[1] = srcPass->getLightCountLimit();
				lightCount[2] = 0;
			}
			else if(srcPass->getLightTypeLimit() == LT_SPOT)
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
				"Using iterative lighting method with RT Shader System requires specifieng explicit light type.");
		}
	}
	setLightCount(lightCount);
	return true;
}
//-----------------------------------------------------------------------
void FFPLighting::setLightCount(const int lightCount[3])
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
void FFPLighting::getLightCount(int lightCount[3]) const
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
const String & FFPLightingFactory::getType() const
{
	return FFPLighting::Type;
}

//-----------------------------------------------------------------------
SubRenderState * FFPLightingFactory::createInstance(ScriptCompiler * compiler,
    PropertyScriptNode * prop, ShaderCh * pass, SGScriptAnalyze * translator)
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

			if (modelType == "ffp")
			{
				return createOrRetrieveInstance(translator);
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------
void FFPLightingFactory::writeInstance(MaterialSerializer* ser, SubRenderState* subRenderState,
    ShaderCh * srcPass, ShaderCh * dstPass)
{
	ser->writeAttribute(4, "lighting_stage");
	ser->writeValue("ffp");
}

//-----------------------------------------------------------------------
SubRenderState*	FFPLightingFactory::createInstanceImpl()
{
	return N_new FFPLighting;
}

}
}

#endif
