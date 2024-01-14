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
#include "OgreShaderExNormalMapLighting.h"
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
String NormalMapLighting::Type						= "SGX_NormalMapLighting";

Light NormalMapLighting::msBlankLight;

//-----------------------------------------------------------------------
NormalMapLighting::NormalMapLighting()
{
	mTrackVertexColourType			= CM_T_NONE;
	mNormalMapSamplerIndex			= 0;
	mVSTexCoordSetIndex				= 0;
	mSpecularEnable					= false;
	mNormalMapSpace					= NMS_TANGENT;
	mNormalMapMinFilter				= TFOP_LINEAR;
	mNormalMapMagFilter				= TFOP_LINEAR;
	mNormalMapMipFilter				= TFOP_POINT;
	mNormalMapAnisotropy			= 1;
	mNormalMapMipBias				= -1.0;

	msBlankLight.setDiffuse(Colour::Black);
	msBlankLight.setSpecular(Colour::Black);
    msBlankLight.setRange(0);
    msBlankLight.setAttenuationConstant(1);
    msBlankLight.setAttenuationLinear(0);
    msBlankLight.setAttenuationQuadric(0);
}
//-----------------------------------------------------------------------
const String& NormalMapLighting::getType() const
{
	return Type;
}
//-----------------------------------------------------------------------
int	NormalMapLighting::getExecutionOrder() const
{
	return FFP_LIGHTING;
}
//-----------------------------------------------------------------------
void NormalMapLighting::updateGpuProgramsParams(GeometryObj* rend, ShaderCh* pass,
    const SysSyncParam * source,  const LightList * pLightList)
{
	if (mLightParamsList.size() == 0)
		return;

	LightType curLightType = LT_DIRECTIONAL;
	unsigned int curSearchLightIndex = 0;
	const Matrix4& matWorld = source->getWorldMatrix();
	Matrix3 matWorldInvRotation;
	Vector3 vRow0(matWorld[0][0], matWorld[0][1], matWorld[0][2]);
	Vector3 vRow1(matWorld[1][0], matWorld[1][1], matWorld[1][2]);
	Vector3 vRow2(matWorld[2][0], matWorld[2][1], matWorld[2][2]);

	vRow0.normalise();
	vRow1.normalise();
	vRow2.normalise();

	matWorldInvRotation.SetColumn(0, vRow0);
	matWorldInvRotation.SetColumn(1, vRow1);
	matWorldInvRotation.SetColumn(2, vRow2);

	// Update inverse rotation parameter.
	if (mWorldInvRotMatrix.get() != NULL)
		mWorldInvRotMatrix->setGpuParameter(matWorldInvRotation);

	// Update per light parameters.
	for (unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		const LightParams& curParams = mLightParamsList[i];

		if (curLightType != curParams.mType)
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
			{
				Vector3 vec3;

				// Update light direction. (Object space).
				vec3 = matWorldInvRotation * srcLight->getSpaceDirection();
				vec3.normalise();

				vParameter.x = -vec3.x;
				vParameter.y = -vec3.y;
				vParameter.z = -vec3.z;
				vParameter.w = 0.0;
				curParams.mDirection->setGpuParameter(vParameter);
			}
			break;
		case LT_POINT:
			// Update light position. (World space).
			vParameter = srcLight->getRelSpace4();
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

				// Update light position. (World space).
				vParameter = srcLight->getRelSpace4();
				curParams.mPosition->setGpuParameter(vParameter);


				// Update light direction. (Object space).
				vec3 = matWorldInvRotation * srcLight->getSpaceDirection();
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
			if ((mTrackVertexColourType & CM_T_SPECULAR) == 0)
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
bool NormalMapLighting::resolveParameters(ProgramSet* programSet)
{
	if (false == resolveGlobalParameters(programSet))
		return false;

	if (false == resolvePerLightParameters(programSet))
		return false;

	return true;
}

//-----------------------------------------------------------------------
bool NormalMapLighting::resolveGlobalParameters(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();

	// Resolve normal map texture sampler parameter.
	mNormalMapSampler = psProgram->resolveParameter(GDT_Sampler2D, mNormalMapSamplerIndex, GPT_Space, "gNormalMapSampler");
	if (mNormalMapSampler.get() == NULL)
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
	if ((mTrackVertexColourType & CM_T_EMISSIVE) == 0)
	{
		mSurfaceEmissiveColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Emissive, 0);
		if (mSurfaceEmissiveColour.get() == NULL)
			return false;
	}

	// Get derived scene colour.
	mDerivedSceneColour = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Emissive_Evn, 0);
	if (mDerivedSceneColour.get() == NULL)
		return false;

	// Get surface shininess.
	mSurfaceShininess = psProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Ch_Shininess, 0);
	if (mSurfaceShininess.get() == NULL)
		return false;

	// Resolve input vertex shader normal.
	mVSInNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
	if (mVSInNormal.get() == NULL)
		return false;

	// Resolve input vertex shader tangent.
	if (mNormalMapSpace == NMS_TANGENT)
	{
		mVSInTangent = vsMain->resolveInputParameter(Parameter::SPS_TANGENT, 0, Parameter::SPC_TANGENT_OBJECT_SPACE, GDT_Float3X);
		if (mVSInTangent.get() == NULL)
			return false;

		// Resolve local vertex shader TNB matrix.
		mVSTBNMatrix = vsMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lMatTBN", GDT_Matrix3X3);
		if (mVSTBNMatrix.get() == NULL)
			return false;
	}

	// Resolve input vertex shader texture coordinates.
	mVSInTexcoord = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, mVSTexCoordSetIndex,
		Parameter::Content(Parameter::SPC_TEXTURE_COORDINATE0 + mVSTexCoordSetIndex),
		GDT_Float2X);
	if (mVSInTexcoord.get() == NULL)
		return false;

	// Resolve output vertex shader texture coordinates.
	mVSOutTexcoord = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
		Parameter::Content(Parameter::SPC_TEXTURE_COORDINATE0 + mVSTexCoordSetIndex),
		GDT_Float2X);
	if (mVSOutTexcoord.get() == NULL)
		return false;


	// Resolve pixel input texture coordinates normal.
	mPSInTexcoord = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
		mVSOutTexcoord->getIndex(),
		mVSOutTexcoord->getContent(),
		mVSOutTexcoord->getType());
	if (mPSInTexcoord.get() == NULL)
		return false;

	// Resolve pixel shader normal.
	if (mNormalMapSpace == NMS_OBJECT)
	{
		mPSNormal = psMain->resolveLocalParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
		if (mPSNormal.get() == NULL)
			return false;
	}
	else if (mNormalMapSpace == NMS_TANGENT)
	{
		mPSNormal = psMain->resolveLocalParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_TANGENT_SPACE, GDT_Float3X);
		if (mPSNormal.get() == NULL)
			return false;
	}


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

	mPSTempDiffuseColour = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lNormalMapDiffuse", GDT_Float4X);
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

		mPSTempSpecularColour = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lNormalMapSpecular", GDT_Float4X);
		if (mPSTempSpecularColour.get() == NULL)
			return false;


		mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
		if (mVSInPosition.get() == NULL)
			return false;

		if (mNormalMapSpace == NMS_TANGENT)
		{
			mVSOutView = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
				Parameter::SPC_POSTOCAMERA_TANGENT_SPACE, GDT_Float3X);
			if (mVSOutView.get() == NULL)
				return false;
		}
		else if (mNormalMapSpace == NMS_OBJECT)
		{
			mVSOutView = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
				Parameter::SPC_POSTOCAMERA_OBJECT_SPACE, GDT_Float3X);
			if (mVSOutView.get() == NULL)
				return false;
		}


		mPSInView = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
			mVSOutView->getIndex(),
			mVSOutView->getContent(),
			mVSOutView->getType());
		if (mPSInView.get() == NULL)
			return false;

		// Resolve camera position world space.
		mCamPosWorldSpace = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Camera_Pos, 0);
		if (mCamPosWorldSpace.get() == NULL)
			return false;

		mVSLocalDir = vsMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lNormalMapTempDir", GDT_Float3X);
		if (mVSLocalDir.get() == NULL)
			return false;

		mVSWorldPosition = vsMain->resolveLocalParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_WORLD_SPACE, GDT_Float3X);
		if (mVSWorldPosition.get() == NULL)
			return false;

		// Resolve world matrix.
		mWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space, 0);
		if (mWorldMatrix.get() == NULL)
			return false;

		// Resolve inverse world rotation matrix.
		mWorldInvRotMatrix = vsProgram->resolveParameter(GDT_Matrix4X4, -1, GPT_Space, "inv_world_rotation_matrix");
		if (mWorldInvRotMatrix.get() == NULL)
			return false;

	}

	return true;
}

//-----------------------------------------------------------------------
bool NormalMapLighting::resolvePerLightParameters(ProgramSet* programSet)
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
			mLightParamsList[i].mDirection = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light|GPT_Space, "light_direction_obj_space");
			if (mLightParamsList[i].mDirection.get() == NULL)
				return false;

			if (mNormalMapSpace == NMS_TANGENT)
			{
				mLightParamsList[i].mVSOutDirection = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_LIGHTDIRECTION_TANGENT_SPACE0 + i),
					GDT_Float3X);
			}
			else if (mNormalMapSpace == NMS_OBJECT)
			{
				mLightParamsList[i].mVSOutDirection = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_LIGHTDIRECTION_OBJECT_SPACE0 + i),
					GDT_Float3X);
			}
			if (mLightParamsList[i].mVSOutDirection.get() == NULL)
				return false;

			mLightParamsList[i].mPSInDirection = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
				mLightParamsList[i].mVSOutDirection->getIndex(),
				mLightParamsList[i].mVSOutDirection->getContent(),
				mLightParamsList[i].mVSOutDirection->getType());
			if (mLightParamsList[i].mPSInDirection.get() == NULL)
				return false;

			break;

		case LT_POINT:
			mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInPosition.get() == NULL)
				return false;

			mLightParamsList[i].mPosition = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light|GPT_Space, "light_position_world_space");
			if (mLightParamsList[i].mPosition.get() == NULL)
				return false;

			if (mNormalMapSpace == NMS_TANGENT)
			{
				mLightParamsList[i].mVSOutToLightDir = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_POSTOLIGHT_TANGENT_SPACE0 + i),
					GDT_Float3X);
			}
			else if (mNormalMapSpace == NMS_OBJECT)
			{
				mLightParamsList[i].mVSOutToLightDir = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_POSTOLIGHT_OBJECT_SPACE0 + i),
					GDT_Float3X);
			}

			if (mLightParamsList[i].mVSOutToLightDir.get() == NULL)
				return false;

			mLightParamsList[i].mPSInToLightDir = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
				mLightParamsList[i].mVSOutToLightDir->getIndex(),
				mLightParamsList[i].mVSOutToLightDir->getContent(),
				mLightParamsList[i].mVSOutToLightDir->getType());
			if (mLightParamsList[i].mPSInToLightDir.get() == NULL)
				return false;

			mLightParamsList[i].mAttenuatParams = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_attenuation");
			if (mLightParamsList[i].mAttenuatParams.get() == NULL)
				return false;

			// Resolve local dir.
			if (mVSLocalDir.get() == NULL)
			{
				mVSLocalDir = vsMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lNormalMapTempDir", GDT_Float3X);
				if(mVSLocalDir.get() == NULL)
					return false;
			}

			// Resolve world position.
			if(mVSWorldPosition.get() == NULL)
			{
				mVSWorldPosition = vsMain->resolveLocalParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_WORLD_SPACE, GDT_Float3X);
				if (mVSWorldPosition.get() == NULL)
					return false;
			}

			// Resolve world matrix.
			if(mWorldMatrix.get() == NULL)
			{
				mWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space, 0);
				if (mWorldMatrix.get() == NULL)
					return false;
			}

			// Resolve inverse world rotation matrix.
			if(mWorldInvRotMatrix.get() == NULL)
			{
				mWorldInvRotMatrix = vsProgram->resolveParameter(GDT_Matrix4X4, -1, GPT_Space, "inv_world_rotation_matrix");
				if (mWorldInvRotMatrix.get() == NULL)
					return false;
			}
			break;

		case LT_SPOT:
			mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
			if (mVSInPosition.get() == NULL)
				return false;

			mLightParamsList[i].mPosition = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light|GPT_Space, "light_position_world_space");
			if (mLightParamsList[i].mPosition.get() == NULL)
				return false;

			if (mNormalMapSpace == NMS_TANGENT)
			{
				mLightParamsList[i].mVSOutToLightDir = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_POSTOLIGHT_TANGENT_SPACE0 + i),
					GDT_Float3X);
			}
			else if (mNormalMapSpace == NMS_OBJECT)
			{
				mLightParamsList[i].mVSOutToLightDir = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_POSTOLIGHT_OBJECT_SPACE0 + i),
					GDT_Float3X);
			}
			if (mLightParamsList[i].mVSOutToLightDir.get() == NULL)
				return false;

			mLightParamsList[i].mPSInToLightDir = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
				mLightParamsList[i].mVSOutToLightDir->getIndex(),
				mLightParamsList[i].mVSOutToLightDir->getContent(),
				mLightParamsList[i].mVSOutToLightDir->getType());
			if (mLightParamsList[i].mPSInToLightDir.get() == NULL)
				return false;

			mLightParamsList[i].mDirection = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Light|GPT_Space, "light_direction_obj_space");
			if (mLightParamsList[i].mDirection.get() == NULL)
				return false;

			if (mNormalMapSpace == NMS_TANGENT)
			{
				mLightParamsList[i].mVSOutDirection = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_LIGHTDIRECTION_TANGENT_SPACE0 + i),
					GDT_Float3X);
			}
			else if (mNormalMapSpace == NMS_OBJECT)
			{
				mLightParamsList[i].mVSOutDirection = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1,
					Parameter::Content(Parameter::SPC_LIGHTDIRECTION_OBJECT_SPACE0 + i),
					GDT_Float3X);
			}
			if (mLightParamsList[i].mVSOutDirection.get() == NULL)
				return false;


			mLightParamsList[i].mPSInDirection = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
				mLightParamsList[i].mVSOutDirection->getIndex(),
				mLightParamsList[i].mVSOutDirection->getContent(),
				mLightParamsList[i].mVSOutDirection->getType());
			if (mLightParamsList[i].mPSInDirection.get() == NULL)
				return false;

			mLightParamsList[i].mAttenuatParams = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "light_attenuation");
			if (mLightParamsList[i].mAttenuatParams.get() == NULL)
				return false;

			mLightParamsList[i].mSpotParams = psProgram->resolveParameter(GDT_Float3X, -1, GPT_Light, "spotlight_params");
			if (mLightParamsList[i].mSpotParams.get() == NULL)
				return false;

			// Resolve local dir.
			if (mVSLocalDir.get() == NULL)
			{
				mVSLocalDir = vsMain->resolveLocalParameter(Parameter::SPS_UNKNOWN, 0, "lNormalMapTempDir", GDT_Float3X);
				if (mVSLocalDir.get() == NULL)
					return false;
			}

			// Resolve world position.
			if (mVSWorldPosition.get() == NULL)
			{
				mVSWorldPosition = vsMain->resolveLocalParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_WORLD_SPACE, GDT_Float3X);
				if (mVSWorldPosition.get() == NULL)
					return false;
			}

			// Resolve world matrix.
			if (mWorldMatrix.get() == NULL)
			{
				mWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space, 0);
				if (mWorldMatrix.get() == NULL)
					return false;
			}

			// Resolve inverse world rotation matrix.
			if (mWorldInvRotMatrix.get() == NULL)
			{
				mWorldInvRotMatrix = vsProgram->resolveParameter(GDT_Matrix4X4, -1, GPT_Space, "inv_world_rotation_matrix");
				if (mWorldInvRotMatrix.get() == NULL)
					return false;
			}
			break;
		}

		// Resolve diffuse colour.
		if ((mTrackVertexColourType & CM_T_DIFFUSE) == 0)
		{
			mLightParamsList[i].mDiffuseColour = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "derived_light_diffuse");
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
				mLightParamsList[i].mSpecularColour = psProgram->resolveParameter(GDT_Float4X, -1, GPT_Light, "derived_light_specular");
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
bool NormalMapLighting::resolveDependencies(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(SGX_LIB_NORMALMAPLIGHTING);

	psProgram->addDependency(FFP_LIB_COMMON);
	psProgram->addDependency(SGX_LIB_NORMALMAPLIGHTING);

	return true;
}

//-----------------------------------------------------------------------
bool NormalMapLighting::addFunctionInvocations(ProgramSet* programSet)
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


	// Add the normal fetch function invocation.
	if (false == addPSNormalFetchInvocation(psMain, FFP_PS_COLOUR_BEGIN + 1, internalCounter))
		return false;


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
bool NormalMapLighting::addVSInvocation(Function* vsMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	// Construct TNB matrix.
	if (mNormalMapSpace == NMS_TANGENT)
	{
		curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_CONSTRUCT_TBNMATRIX, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mVSInNormal, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSInTangent, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSTBNMatrix, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}


	// Output texture coordinates.
	curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mVSInTexcoord, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mVSOutTexcoord, Operand::OPS_OUT);
	vsMain->addAtomInstance(curFuncInvocation);

	// Compute world space position.
	if (mVSWorldPosition.get() != NULL)
	{
		curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMPOSITION, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mWorldMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSWorldPosition, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);
	}


	// Compute view vector.
	if (mVSInPosition.get() != NULL &&
		mVSOutView.get() != NULL)
	{
		// View vector in world space.
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_SUBTRACT, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mCamPosWorldSpace, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(mVSWorldPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		// Transform to object space.
		curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMNORMAL, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mWorldInvRotMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		// Transform to tangent space.
		if (mNormalMapSpace == NMS_TANGENT)
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMNORMAL, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mVSTBNMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutView, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}

		// Output object space.
		else if (mNormalMapSpace == NMS_OBJECT)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSOutView, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}
	}

	// Add per light functions.
	for (unsigned int i=0; i < mLightParamsList.size(); ++i)
	{
		if (false == addVSIlluminationInvocation(&mLightParamsList[i], vsMain, groupOrder, internalCounter))
			return false;
	}


	return true;
}

//-----------------------------------------------------------------------
bool NormalMapLighting::addVSIlluminationInvocation(LightParams* curLightParams, Function* vsMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	// Compute light direction in texture space.
	if (curLightParams->mDirection.get() != NULL &&
		curLightParams->mVSOutDirection.get() != NULL)
	{
		// Transform to texture space.
		if (mNormalMapSpace == NMS_TANGENT)
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMNORMAL, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mVSTBNMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mVSOutDirection, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}
		// Output object space.
		else if (mNormalMapSpace == NMS_OBJECT)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(curLightParams->mDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mVSOutDirection, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}
	}

	// Transform light vector to target space..
	if (curLightParams->mPosition.get() != NULL &&
		curLightParams->mVSOutToLightDir.get() != NULL)
	{
		// Compute light vector.
		curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_SUBTRACT, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(curLightParams->mPosition, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
		curFuncInvocation->pushOperand(mVSWorldPosition, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		// Transform to object space.
		curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMNORMAL, groupOrder, internalCounter++);
		curFuncInvocation->pushOperand(mWorldInvRotMatrix, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_IN);
		curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_OUT);
		vsMain->addAtomInstance(curFuncInvocation);

		// Transform to tangent space.
		if (mNormalMapSpace == NMS_TANGENT)
		{
			curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_TRANSFORMNORMAL, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mVSTBNMatrix, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mVSOutToLightDir, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}

		// Output object space.
		else if (mNormalMapSpace == NMS_OBJECT)
		{
			curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
			curFuncInvocation->pushOperand(mVSLocalDir, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mVSOutToLightDir, Operand::OPS_OUT);
			vsMain->addAtomInstance(curFuncInvocation);
		}
	}


	return true;
}

//-----------------------------------------------------------------------
bool NormalMapLighting::addPSNormalFetchInvocation(Function* psMain, const int groupOrder, int& internalCounter)
{
	FunctionInvocation* curFuncInvocation = NULL;

	curFuncInvocation = N_new FunctionInvocation(SGX_FUNC_FETCHNORMAL, groupOrder, internalCounter++);
	curFuncInvocation->pushOperand(mNormalMapSampler, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSInTexcoord, Operand::OPS_IN);
	curFuncInvocation->pushOperand(mPSNormal, Operand::OPS_OUT);
	psMain->addAtomInstance(curFuncInvocation);

	return true;
}

//-----------------------------------------------------------------------
bool NormalMapLighting::addPSGlobalIlluminationInvocation(Function* psMain,
    const int groupOrder, int & internalCounter)
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
bool NormalMapLighting::addPSIlluminationInvocation(LightParams* curLightParams, Function* psMain, const int groupOrder, int& internalCounter)
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
			curFuncInvocation->pushOperand(mPSNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInView, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPSInDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
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
			curFuncInvocation->pushOperand(mPSNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPSInDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
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
			curFuncInvocation->pushOperand(mPSNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInView, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPSInToLightDir, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
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
			curFuncInvocation->pushOperand(mPSNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPSInToLightDir, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
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
			curFuncInvocation->pushOperand(mPSNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(mPSInView, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPSInToLightDir, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mPSInDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
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
			curFuncInvocation->pushOperand(mPSNormal, Operand::OPS_IN);
			curFuncInvocation->pushOperand(curLightParams->mPSInToLightDir, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
			curFuncInvocation->pushOperand(curLightParams->mPSInDirection, Operand::OPS_IN, (Operand::OPM_X |  Operand::OPM_Y | Operand::OPM_Z));
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
bool NormalMapLighting::addPSFinalAssignmentInvocation( Function* psMain, const int groupOrder, int& internalCounter )
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
void NormalMapLighting::write(const SubRenderState& rhs)
{
	const NormalMapLighting& rhsLighting = static_cast<const NormalMapLighting&>(rhs);

	int lightCount[3];

	rhsLighting.getLightCount(lightCount);
	setLightCount(lightCount);

	mTrackVertexColourType = rhsLighting.mTrackVertexColourType;
	mSpecularEnable = rhsLighting.mSpecularEnable;
	mNormalMapSpace = rhsLighting.mNormalMapSpace;
	mNormalMapTextureName = rhsLighting.mNormalMapTextureName;
	mNormalMapMinFilter = rhsLighting.mNormalMapMinFilter;
	mNormalMapMagFilter = rhsLighting.mNormalMapMagFilter;
	mNormalMapMipFilter = rhsLighting.mNormalMapMipFilter;
	mNormalMapMipBias = rhsLighting.mNormalMapMipBias;
}
//-----------------------------------------------------------------------
bool NormalMapLighting::preAddToRenderState(const RenderState * renderState,
    ShaderCh * srcPass, ShaderCh * dstPass)
{
	if(srcPass->isLight() == false)
		return false;

	int lightCount[3];

	renderState->getLightCount(lightCount);

	ShaderChTextureUnit * normalMapTexture = dstPass->getTexture().create();

	normalMapTexture->setSrc(mNormalMapTextureName, Texture::T_2D);
	normalMapTexture->setMinFilter(mNormalMapMinFilter);
    normalMapTexture->setMagFilter(mNormalMapMagFilter);
    normalMapTexture->setMipFilter(mNormalMapMipFilter);
	normalMapTexture->setAnisotropy(mNormalMapAnisotropy);
	normalMapTexture->setMipmapBias(mNormalMapMipBias);
	mNormalMapSamplerIndex = dstPass->getTexture().getCount() - 1;

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
	if (srcPass->isEachLight())
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
void NormalMapLighting::setLightCount(const int lightCount[3])
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
void NormalMapLighting::getLightCount(int lightCount[3]) const
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
const String& NormalMapLightingFactory::getType() const
{
	return NormalMapLighting::Type;
}
//-----------------------------------------------------------------------
SubRenderState*	NormalMapLightingFactory::createInstance(ScriptCompiler * compiler,
    PropertyScriptNode * prop, ShaderCh * pass, SGScriptAnalyze * translator)
{
	if (prop->name == "lighting_stage")
	{
		if(prop->values.size() >= 2)
		{
			String strValue;
			ScriptNodeList::const_iterator it = prop->values.begin();

			// Read light model type.
			if(false == SGScriptAnalyze::get(*it, &strValue))
			{
				compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
				return NULL;
			}

			// Case light model type is normal map
			if (strValue == "normal_map")
			{
				++it;
				if (false == SGScriptAnalyze::get(*it, &strValue))
				{
					compiler->error(prop, ScriptCompiler::CE_STRINGEXPECTED);
					return NULL;
				}


				SubRenderState* subRenderState = createOrRetrieveInstance(translator);
				NormalMapLighting* normalMapSubRenderState = static_cast<NormalMapLighting*>(subRenderState);

				normalMapSubRenderState->setNormalMapTextureName(strValue);


				// Read normal map space type.
				if (prop->values.size() >= 3)
				{
					++it;
					if (false == SGScriptAnalyze::get(*it, &strValue))
					{
						compiler->error(prop, ScriptCompiler::CE_STRINGEXPECTED);
						return NULL;
					}

					// Normal map defines normals in tangent space.
					if (strValue == "tangent_space")
					{
						normalMapSubRenderState->setNormalMapSpace(NormalMapLighting::NMS_TANGENT);
					}

					// Normal map defines normals in object space.
					if (strValue == "object_space")
					{
						normalMapSubRenderState->setNormalMapSpace(NormalMapLighting::NMS_OBJECT);
					}
				}

				// Read texture coordinate index.
				if (prop->values.size() >= 4)
				{
					unsigned int textureCoordinateIndex = 0;

					++it;
					if (SGScriptAnalyze::get(*it, &textureCoordinateIndex))
					{
						normalMapSubRenderState->setTexCoordIndex(textureCoordinateIndex);
					}
				}

				// Read texture filtering format.
				if (prop->values.size() >= 5)
				{
					++it;
					if (false == SGScriptAnalyze::get(*it, &strValue))
					{
						compiler->error(prop, ScriptCompiler::CE_STRINGEXPECTED);
						return NULL;
					}

					if (strValue == "none")
					{
						normalMapSubRenderState->setNormalMapFiltering(TFOP_POINT, TFOP_POINT, TFOP_NONE);
					}

					else if (strValue == "bilinear")
					{
						normalMapSubRenderState->setNormalMapFiltering(TFOP_LINEAR, TFOP_LINEAR, TFOP_POINT);
					}

					else if (strValue == "trilinear")
					{
						normalMapSubRenderState->setNormalMapFiltering(TFOP_LINEAR, TFOP_LINEAR, TFOP_LINEAR);
					}

					else if (strValue == "anisotropic")
					{
						normalMapSubRenderState->setNormalMapFiltering(TFOP_ANISOTROPIC, TFOP_ANISOTROPIC, TFOP_LINEAR);
					}
				}

				// Read max anisotropy value.
				if(prop->values.size() >= 6)
				{
					unsigned int maxAnisotropy = 0;

					++it;
					if(SGScriptAnalyze::get(*it, &maxAnisotropy))
					{
						normalMapSubRenderState->setNormalMapAnisotropy(maxAnisotropy);
					}
				}

				// Read mip bias value.
				if(prop->values.size() >= 7)
				{
					NIIf mipBias = 0;

					++it;
					if (SGScriptAnalyze::getReal(*it, &mipBias))
					{
						normalMapSubRenderState->setNormalMapMipBias(mipBias);
					}
				}

				return subRenderState;
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------
void NormalMapLightingFactory::writeInstance(MaterialSerializer * ser,
    SubRenderState * subRenderState, ShaderCh * srcPass, ShaderCh * dstPass)
{
	NormalMapLighting * normalMapSubRenderState = static_cast<NormalMapLighting*>(subRenderState);

	ser->writeAttribute(4, "lighting_stage");
	ser->writeValue("normal_map");
	ser->writeValue(normalMapSubRenderState->getNormalMapTextureName());

	if (normalMapSubRenderState->getNormalMapSpace() == NormalMapLighting::NMS_TANGENT)
	{
		ser->writeValue("tangent_space");
	}
	else if (normalMapSubRenderState->getNormalMapSpace() == NormalMapLighting::NMS_OBJECT)
	{
		ser->writeValue("object_space");
	}

	ser->writeValue(StrConv::conv(normalMapSubRenderState->getTexCoordIndex()));
}

//-----------------------------------------------------------------------
SubRenderState*	NormalMapLightingFactory::createInstanceImpl()
{
	return N_new NormalMapLighting;
}

}
}

#endif
