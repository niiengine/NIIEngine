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
#include "ShaderExReflectionMap.h"
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreScriptCompiler.h"



/************************************************************************/
/*                                                                      */
/************************************************************************/
String ShaderExReflectionMap::Type							= "SGX_ReflectionMap";

//-----------------------------------------------------------------------
#define SGX_LIB_REFLECTIONMAP					"SampleLib_ReflectionMap"
#define SGX_FUNC_APPLY_REFLECTION_MAP			"SGX_ApplyReflectionMap"

//-----------------------------------------------------------------------
ShaderExReflectionMap::ShaderExReflectionMap()
{
	mMaskMapSamplerIndex				= 0;			
	mReflectionMapSamplerIndex			= 0;
	mReflectionMapType					= TEX_TYPE_2D;
	mReflectionPowerChanged			    = true;
	mReflectionPowerValue				= 0.5;
}

//-----------------------------------------------------------------------
const String& ShaderExReflectionMap::getType() const
{
	return Type;
}


//-----------------------------------------------------------------------
int	ShaderExReflectionMap::getExecutionOrder() const
{
	// We place this effect after texturing stage and before fog stage.
	return FFP_TEXTURING + 1;
}

//-----------------------------------------------------------------------
void ShaderExReflectionMap::write(const SubRenderState& rhs)
{
	const ShaderExReflectionMap& rhsReflectionMap = static_cast<const ShaderExReflectionMap&>(rhs);
	
	// Copy all settings that affect this sub render state output code.
	mMaskMapSamplerIndex = rhsReflectionMap.mMaskMapSamplerIndex;
	mReflectionMapSamplerIndex = rhsReflectionMap.mReflectionMapSamplerIndex;
	mReflectionMapType = rhsReflectionMap.mReflectionMapType;
	mReflectionPowerChanged	= rhsReflectionMap.mReflectionPowerChanged;
	mReflectionPowerValue = rhsReflectionMap.mReflectionPowerValue;
	mReflectionMapTextureName = rhsReflectionMap.mReflectionMapTextureName;
	mMaskMapTextureName = rhsReflectionMap.mMaskMapTextureName;
}

//-----------------------------------------------------------------------
bool ShaderExReflectionMap::preAddToRenderState(const RenderState * renderState,
    Pass * srcPass, Pass * dstPass)
{
	ShaderChTextureUnit * textureUnit;
	
	// Create the mask texture unit.
	textureUnit = dstPass->getTexture().create();
	textureUnit->setSrc(mMaskMapTextureName, Texture::T_2D);		
	mMaskMapSamplerIndex = dstPass->getTexture().getCount() - 1;

	// Create the reflection texture unit.
	textureUnit = dstPass->getTexture().create();

	if(mReflectionMapType == TEX_TYPE_2D)
	{
		textureUnit->setSrc(mReflectionMapTextureName, Texture::T_2D);	
	}
	else
	{
		textureUnit->setSrc(mReflectionMapTextureName, Texture::T_CUBE);	
	}
		
	mReflectionMapSamplerIndex = dstPass->getTexture().getCount() - 1;

	return true;
}
//-----------------------------------------------------------------------
bool ShaderExReflectionMap::resolveParameters(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();

	// Resolve vs input mask texture coordinates.
	// NOTE: We use the first texture coordinate hard coded here
	// You may want to parametrize this as well - just remember to add it to hash and copy methods. 
	mVSInMaskTexcoord = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 0, Parameter::SPC_TEXTURE_COORDINATE0, GDT_Float2X);
	if(mVSInMaskTexcoord.get() == 0)
		return false;

	// Resolve vs output mask texture coordinates.
	mVSOutMaskTexcoord = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, mVSInMaskTexcoord->getContent(), GDT_Float2X);
	if (mVSOutMaskTexcoord.get() == 0)
		return false;

	// Resolve ps input mask texture coordinates.
	mPSInMaskTexcoord = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 
		mVSOutMaskTexcoord->getIndex(), 
		mVSOutMaskTexcoord->getContent(),
		GDT_Float2X);

	// Resolve vs output reflection texture coordinates.
	mVSOutReflectionTexcoord = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, 
		Parameter::SPC_UNKNOWN,
		mReflectionMapType == TEX_TYPE_2D ? GDT_Float2X : GDT_Float3X);
	if (mVSOutReflectionTexcoord.get() == 0)
		return false;

	// Resolve ps input reflection texture coordinates.
	mPSInReflectionTexcoord= psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 
		mVSOutReflectionTexcoord->getIndex(), 
		mVSOutReflectionTexcoord->getContent(),
		mVSOutReflectionTexcoord->getType());


	// Resolve world matrix.	
	mWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space, 0);
	if(mWorldMatrix.get() == NULL)
		return false;	

	// Resolve world inverse transpose matrix.	
	mWorldITMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Space_InverseTranspose, 0);
	if(mWorldITMatrix.get() == NULL)
		return false;	


	// Resolve view matrix.
	mViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View, 0);
	if (mViewMatrix.get() == NULL)
		return false;	

	// Resolve vertex position.
	mVSInputPos = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
	if (mVSInputPos.get() == NULL)
		return false;		

	// Resolve vertex normal.
	mVSInputNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL, 0, Parameter::SPC_NORMAL_OBJECT_SPACE, GDT_Float3X);
	if (mVSInputNormal.get() == NULL)
		return false;		

	// Resolve mask texture sampler parameter.		
	mMaskMapSampler = psProgram->resolveParameter(GDT_Sampler2D, mMaskMapSamplerIndex, GPT_Render, "mask_sampler");
	if (mMaskMapSampler.get() == NULL)
		return false;

	// Resolve reflection texture sampler parameter.		
	mReflectionMapSampler = psProgram->resolveParameter(mReflectionMapType == TEX_TYPE_2D ? GDT_Sampler2D : GDT_SamplerCube, 
		mReflectionMapSamplerIndex, GPT_Render, "reflection_texture");
	if (mReflectionMapSampler.get() == NULL)
		return false;

	// Resolve reflection power parameter.		
	mReflectionPower = psProgram->resolveParameter(GDT_Float, -1, GPT_Render, "reflection_power");
	if (mReflectionPower.get() == NULL)
		return false;

	// Resolve ps output diffuse colour.
	mPSOutDiffuse = psMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
	if (mPSOutDiffuse.get() == NULL)
		return false;

	return true;
}

//-----------------------------------------------------------------------
bool ShaderExReflectionMap::resolveDependencies(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(FFP_LIB_TEXTURING);
	
	psProgram->addDependency(FFP_LIB_COMMON);
	psProgram->addDependency(FFP_LIB_TEXTURING);
	psProgram->addDependency(SGX_LIB_REFLECTIONMAP);
	
	return true;
}


//-----------------------------------------------------------------------
bool ShaderExReflectionMap::addFunctionInvocations(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();	
	Function* vsMain = vsProgram->getEntryPointFunction();	
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* psMain = psProgram->getEntryPointFunction();	
	

	// Add vertex shader invocations.
	if(false == addVSInvocations(vsMain, FFP_VS_TEXTURING + 1))
		return false;


	// Add pixel shader invocations.
	if(false == addPSInvocations(psMain, FFP_PS_TEXTURING + 1))
		return false;
	
	return true;
}
//-----------------------------------------------------------------------
bool ShaderExReflectionMap::addVSInvocations( Function* vsMain, const int groupOrder )
{
	FunctionInvocation* funcInvocation = NULL;
	int internalCounter = 0;

	// Output mask texture coordinates.
	funcInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN,  groupOrder, internalCounter++); 
	funcInvocation->pushOperand(mVSInMaskTexcoord, Operand::OPS_IN);
	funcInvocation->pushOperand(mVSOutMaskTexcoord, Operand::OPS_OUT);
	vsMain->addAtomInstance(funcInvocation);

	// Output reflection texture coordinates.
	if (mReflectionMapType == TEX_TYPE_2D)
	{
		funcInvocation = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_SPHERE,  groupOrder, internalCounter++); 
		funcInvocation->pushOperand(mWorldITMatrix, Operand::OPS_IN);
		funcInvocation->pushOperand(mViewMatrix, Operand::OPS_IN);	
		funcInvocation->pushOperand(mVSInputNormal, Operand::OPS_IN);	
		funcInvocation->pushOperand(mVSOutReflectionTexcoord, Operand::OPS_OUT);
		vsMain->addAtomInstance(funcInvocation);
	}
	else
	{
		funcInvocation = N_new FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_REFLECT, groupOrder, internalCounter++); 
		funcInvocation->pushOperand(mWorldMatrix, Operand::OPS_IN);
		funcInvocation->pushOperand(mWorldITMatrix, Operand::OPS_IN);
		funcInvocation->pushOperand(mViewMatrix, Operand::OPS_IN);					
		funcInvocation->pushOperand(mVSInputNormal, Operand::OPS_IN);	
		funcInvocation->pushOperand(mVSInputPos, Operand::OPS_IN);				
		funcInvocation->pushOperand(mVSOutReflectionTexcoord, Operand::OPS_OUT);
		vsMain->addAtomInstance(funcInvocation);
	}
	return true;
}
//-----------------------------------------------------------------------
bool ShaderExReflectionMap::addPSInvocations( Function* psMain, const int groupOrder )
{
	FunctionInvocation* funcInvocation = NULL;
	int internalCounter = 0;

	funcInvocation = N_new FunctionInvocation(SGX_FUNC_APPLY_REFLECTION_MAP, groupOrder, internalCounter++);
	funcInvocation->pushOperand(mMaskMapSampler, Operand::OPS_IN);
	funcInvocation->pushOperand(mPSInMaskTexcoord, Operand::OPS_IN);
	funcInvocation->pushOperand(mReflectionMapSampler, Operand::OPS_IN);
	funcInvocation->pushOperand(mPSInReflectionTexcoord, Operand::OPS_IN);	
	funcInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_IN,(Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
	funcInvocation->pushOperand(mReflectionPower, Operand::OPS_IN);
	funcInvocation->pushOperand(mPSOutDiffuse, Operand::OPS_OUT,(Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
	
	psMain->addAtomInstance(funcInvocation);

	return true;
}

//-----------------------------------------------------------------------
void ShaderExReflectionMap::setReflectionMapType(Texture::Type type)
{
	if(type != TEX_TYPE_2D && type != TEX_TYPE_CUBE_MAP)
	{
		N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"Invalid texture type set - only 2D or Cube supported");
	}
	mReflectionMapType = type;
}
//-----------------------------------------------------------------------
void ShaderExReflectionMap::setReflectionPower(const NIIf reflectionPower)
{
	mReflectionPowerValue = reflectionPower;
	mReflectionPowerChanged = true;
}

//-----------------------------------------------------------------------
void ShaderExReflectionMap::updateGpuProgramsParams(GeometryObj* rend, Pass* pass,
    const SysSyncParam* source, const LightList* pLightList)
{
	if(mReflectionPowerChanged)
	{
		GpuProgramParam * fsParams;
        pass->getProgram().get(0, ST_FS, NULL, fsParams);

		mReflectionPower->setGpuParameter(mReflectionPowerValue);

		mReflectionPowerChanged = false;
	}	
}

//-----------------------------------------------------------------------
SubRenderState * ShaderExReflectionMapFactory::createInstance(ScriptCompiler * compiler, 
    PropertyScriptNode * prop, Pass * pass, SGScriptAnalyze * translator)
{
	if (prop->name == "rtss_ext_reflection_map")
	{
		if(prop->values.size() >= 2)
		{
			String strValue;
			ScriptNodeList::const_iterator it = prop->values.begin();

			// Read reflection map type.
			if(false == SGScriptAnalyze::get(*it, &strValue))
			{
				compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
				return NULL;
			}
			++it;

			SubRenderState* subRenderState = SubRenderStateFactory::createInstance();
			ShaderExReflectionMap* reflectionMapSubRenderState = static_cast<ShaderExReflectionMap*>(subRenderState);
			

			// Reflection map is cubic texture.
			if (strValue == "cube_map")
			{
				reflectionMapSubRenderState->setReflectionMapType(TEX_TYPE_CUBE_MAP);
			}

			// Reflection map is 2d texture.
			else if (strValue == "2d_map")
			{
				reflectionMapSubRenderState->setReflectionMapType(TEX_TYPE_2D);
			}
	
			// Read mask texture.
			if (false == SGScriptAnalyze::get(*it, &strValue))
			{
				compiler->error(prop, ScriptCompiler::CE_STRINGEXPECTED);
				return NULL;
			}
			reflectionMapSubRenderState->setMaskMapTextureName(strValue);
			++it;
			
		
			// Read reflection texture.
			if (false == SGScriptAnalyze::get(*it, &strValue))
			{
				compiler->error(prop, ScriptCompiler::CE_STRINGEXPECTED);
				return NULL;
			}			
			reflectionMapSubRenderState->setReflectionMapTextureName(strValue);
			++it;

			// Read reflection power value.
			NIIf reflectionPower = 0.5;
			if (false == SGScriptAnalyze::getReal(*it, &reflectionPower))
			{
				compiler->error(prop, ScriptCompiler::CE_STRINGEXPECTED);
				return NULL;
			}			
			reflectionMapSubRenderState->setReflectionPower(reflectionPower);
				
			return subRenderState;								
			
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------
void ShaderExReflectionMapFactory::writeInstance(MaterialSerializer* ser, 
    SubRenderState * subRenderState, Pass * srcPass, Pass * dstPass)
{
	ser->writeAttribute(4, "rtss_ext_reflection_map");

	ShaderExReflectionMap* reflectionMapSubRenderState = static_cast<ShaderExReflectionMap*>(subRenderState);

	if (reflectionMapSubRenderState->getReflectionMapType() == TEX_TYPE_CUBE_MAP)
	{
		ser->writeValue("cube_map");
	}
	else if (reflectionMapSubRenderState->getReflectionMapType() == TEX_TYPE_2D)
	{
		ser->writeValue("2d_map");
	}	

	ser->writeValue(reflectionMapSubRenderState->getMaskMapTextureName());
	ser->writeValue(reflectionMapSubRenderState->getReflectionMapTextureName());
	ser->writeValue(StrConv::conv(reflectionMapSubRenderState->getReflectionPower()));
}

//-----------------------------------------------------------------------
const String& ShaderExReflectionMapFactory::getType() const
{
	return ShaderExReflectionMap::Type;
}


//-----------------------------------------------------------------------
SubRenderState*	ShaderExReflectionMapFactory::createInstanceImpl()
{
	return N_new ShaderExReflectionMap;
}
