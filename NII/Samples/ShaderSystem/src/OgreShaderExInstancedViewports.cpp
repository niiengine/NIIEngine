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
#include "OgreShaderExInstancedViewports.h"
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreScriptCompiler.h"

namespace Ogre {
namespace RTShader {


/************************************************************************/
/*                                                                      */
/************************************************************************/
String ShaderExInstancedViewports::Type							= "SGX_InstancedViewports";

//-----------------------------------------------------------------------
#define SGX_LIB_INSTANCED_VIEWPORTS					        "SampleLib_InstancedViewports"
#define SGX_FUNC_INSTANCED_VIEWPORTS_TRANSFORM			    "SGX_InstancedViewportsTransform"
#define SGX_FUNC_INSTANCED_VIEWPORTS_DISCARD_OUT_OF_BOUNDS	"SGX_InstancedViewportsDiscardOutOfBounds"


//-----------------------------------------------------------------------
ShaderExInstancedViewports::ShaderExInstancedViewports()
{
	mMonitorsCount				= Vector2(1.0, 1.0);			
	mMonitorsCountChanged		= true;
}

//-----------------------------------------------------------------------
const String& ShaderExInstancedViewports::getType() const
{
	return Type;
}


//-----------------------------------------------------------------------
int	ShaderExInstancedViewports::getExecutionOrder() const
{
	// We place this effect after texturing stage and before fog stage.
	return FFP_POST_PROCESS+1;
}

//-----------------------------------------------------------------------
void ShaderExInstancedViewports::write(const SubRenderState& rhs)
{
	const ShaderExInstancedViewports& rhsInstancedViewports = static_cast<const ShaderExInstancedViewports&>(rhs);
	
	// Copy all settings that affect this sub render state output code.
	mMonitorsCount = rhsInstancedViewports.mMonitorsCount;
	mMonitorsCountChanged = rhsInstancedViewports.mMonitorsCountChanged;
}

//-----------------------------------------------------------------------
bool ShaderExInstancedViewports::preAddToRenderState( const RenderState* renderState, Pass* srcPass, Pass* dstPass )
{
	return srcPass->getParent()->getParent()->getName().find("SdkTrays") == Ogre::String::npos;
}
//-----------------------------------------------------------------------
bool ShaderExInstancedViewports::resolveParameters(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();


	// Resolve vertex shader output position in projective space.

	mVSInPosition = vsMain->resolveInputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_OBJECT_SPACE, GDT_Float4X);
	if (mVSInPosition.get() == NULL)
		return false;

	mVSOriginalOutPositionProjectiveSpace = vsMain->resolveOutputParameter(Parameter::SPS_POSITION, 0, Parameter::SPC_POSITION_PROJECTIVE_SPACE, GDT_Float4X);
	if (mVSOriginalOutPositionProjectiveSpace.get() == NULL)
		return false;

#define SPC_POSITION_PROJECTIVE_SPACE_AS_TEXCORD ((Parameter::Content)(Parameter::SPC_CUSTOM_CONTENT_BEGIN + 1))

	mVSOutPositionProjectiveSpace = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, SPC_POSITION_PROJECTIVE_SPACE_AS_TEXCORD, GDT_Float4X);
	if (mVSOutPositionProjectiveSpace.get() == NULL)
		return false;

	// Resolve ps input position in projective space.
	mPSInPositionProjectiveSpace = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 
		mVSOutPositionProjectiveSpace->getIndex(), 
		mVSOutPositionProjectiveSpace->getContent(),
		GDT_Float4X);
	if (mPSInPositionProjectiveSpace.get() == NULL)
		return false;

	// Resolve vertex shader uniform monitors count
	mVSInMonitorsCount = vsProgram->resolveParameter(GDT_Float2X, -1, GPT_Render, "monitorsCount");
	if (mVSInMonitorsCount.get() == NULL)
		return false;

	// Resolve pixel shader uniform monitors count
	mPSInMonitorsCount = psProgram->resolveParameter(GDT_Float2X, -1, GPT_Render, "monitorsCount");
	if (mPSInMonitorsCount.get() == NULL)
		return false;




	// Resolve the current world & view matrices concatenated	
	mWorldViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_View_X_Space, 0);
	if (mWorldViewMatrix.get() == NULL)
		return false;	

	// Resolve the current projection matrix
	mProjectionMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParam::GSP_Matrix_Proj, 0);
	if (mProjectionMatrix.get() == NULL)
		return false;	

#define SPC_MONITOR_INDEX Parameter::SPC_TEXTURE_COORDINATE3
	// Resolve vertex shader  monitor index
	mVSInMonitorIndex = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 3, SPC_MONITOR_INDEX, GDT_Float4X);
	if (mVSInMonitorIndex.get() == NULL)
		return false;

#define SPC_MATRIX_R0 Parameter::SPC_TEXTURE_COORDINATE4
#define SPC_MATRIX_R1 Parameter::SPC_TEXTURE_COORDINATE5
#define SPC_MATRIX_R2 Parameter::SPC_TEXTURE_COORDINATE6
#define SPC_MATRIX_R3 Parameter::SPC_TEXTURE_COORDINATE7

	// Resolve vertex shader viewport offset matrix
	mVSInViewportOffsetMatrixR0 = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 4, SPC_MATRIX_R0, GDT_Float4X);
	if (mVSInViewportOffsetMatrixR0.get() == NULL)
		return false;
	mVSInViewportOffsetMatrixR1 = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 5, SPC_MATRIX_R1, GDT_Float4X);
	if (mVSInViewportOffsetMatrixR1.get() == NULL)
		return false;
	mVSInViewportOffsetMatrixR2 = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 6, SPC_MATRIX_R2, GDT_Float4X);
	if (mVSInViewportOffsetMatrixR2.get() == NULL)
		return false;
	mVSInViewportOffsetMatrixR3 = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 7, SPC_MATRIX_R3, GDT_Float4X);
	if (mVSInViewportOffsetMatrixR3.get() == NULL)
		return false;


	
    // Resolve vertex shader output monitor index.	
	mVSOutMonitorIndex = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES, -1, 
			SPC_MONITOR_INDEX,
			GDT_Float4X);
	if (mVSOutMonitorIndex.get() == NULL)
		return false;

	// Resolve ps input monitor index.
	mPSInMonitorIndex = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES, 
		mVSOutMonitorIndex->getIndex(), 
		mVSOutMonitorIndex->getContent(),
		GDT_Float4X);
	if (mPSInMonitorIndex.get() == NULL)
		return false;

	return true;
}

//-----------------------------------------------------------------------
bool ShaderExInstancedViewports::resolveDependencies(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(SGX_LIB_INSTANCED_VIEWPORTS);
	
	psProgram->addDependency(FFP_LIB_COMMON);
	psProgram->addDependency(SGX_LIB_INSTANCED_VIEWPORTS);
	
	return true;
}


//-----------------------------------------------------------------------
bool ShaderExInstancedViewports::addFunctionInvocations(ProgramSet* programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();	
	Function* vsMain = vsProgram->getEntryPointFunction();	
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* psMain = psProgram->getEntryPointFunction();	
	

	// Add vertex shader invocations.
	if (false == addVSInvocations(vsMain, FFP_VS_TRANSFORM + 1))
		return false;


	// Add pixel shader invocations.
	if (false == addPSInvocations(psMain, FFP_PS_PRE_PROCESS + 1))
		return false;
	
	return true;
}

//-----------------------------------------------------------------------
bool ShaderExInstancedViewports::addVSInvocations( Function* vsMain, const int groupOrder )
{
	FunctionInvocation* funcInvocation = NULL;
	int internalCounter = 0;
	
    funcInvocation = N_new FunctionInvocation(SGX_FUNC_INSTANCED_VIEWPORTS_TRANSFORM, groupOrder, internalCounter++);
    funcInvocation->pushOperand(mVSInPosition, Operand::OPS_IN);
    funcInvocation->pushOperand(mWorldViewMatrix, Operand::OPS_IN);
    funcInvocation->pushOperand(mProjectionMatrix, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSInViewportOffsetMatrixR0, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSInViewportOffsetMatrixR1, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSInViewportOffsetMatrixR2, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSInViewportOffsetMatrixR3, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSInMonitorsCount, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSInMonitorIndex, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSOriginalOutPositionProjectiveSpace, Operand::OPS_OUT);
    vsMain->addAtomInstance(funcInvocation);

	// Output position in projective space.
    funcInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN,  groupOrder, internalCounter++); 
    funcInvocation->pushOperand(mVSOriginalOutPositionProjectiveSpace, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSOutPositionProjectiveSpace, Operand::OPS_OUT);
    vsMain->addAtomInstance(funcInvocation);

	// Output monitor index.
    funcInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN,  groupOrder, internalCounter++); 
    funcInvocation->pushOperand(mVSInMonitorIndex, Operand::OPS_IN);
    funcInvocation->pushOperand(mVSOutMonitorIndex, Operand::OPS_OUT);
    vsMain->addAtomInstance(funcInvocation);

	return true;
}

//-----------------------------------------------------------------------
bool ShaderExInstancedViewports::addPSInvocations( Function* psMain, const int groupOrder )
{
	FunctionInvocation* funcInvocation = NULL;
	int internalCounter = 0;

	funcInvocation = N_new FunctionInvocation(SGX_FUNC_INSTANCED_VIEWPORTS_DISCARD_OUT_OF_BOUNDS, groupOrder, internalCounter++);
	funcInvocation->pushOperand(mPSInMonitorsCount, Operand::OPS_IN);
	funcInvocation->pushOperand(mPSInMonitorIndex, Operand::OPS_IN);
	funcInvocation->pushOperand(mPSInPositionProjectiveSpace, Operand::OPS_IN);
	
	psMain->addAtomInstance(funcInvocation);

	return true;
}
//-----------------------------------------------------------------------
void ShaderExInstancedViewports::updateGpuProgramsParams(GeometryObj* rend,
    Pass* pass, const SysSyncParam* source, const LightList* pLightList)
{
	if (mMonitorsCountChanged)
	{
		mVSInMonitorsCount->setGpuParameter(mMonitorsCount + Vector2(0.0001, 0.0001));
		mPSInMonitorsCount->setGpuParameter(mMonitorsCount + Vector2(0.0001, 0.0001));

		mMonitorsCountChanged = false;
	}	
}
//-----------------------------------------------------------------------
void ShaderExInstancedViewports::setMonitorsCount( const Vector2 monitorsCount )
{
    mMonitorsCount = monitorsCount;
    mMonitorsCountChanged = true;
}
//-----------------------------------------------------------------------
SubRenderState*	ShaderExInstancedViewportsFactory::createInstance(ScriptCompiler* compiler, 
    PropertyScriptNode* prop, Pass* pass, SGScriptAnalyze* translator)
{
	SubRenderState* subRenderState = SubRenderStateFactory::createInstance();
	return subRenderState;								
}
//-----------------------------------------------------------------------
void ShaderExInstancedViewportsFactory::writeInstance(MaterialSerializer* ser, 
    SubRenderState* subRenderState, Pass* srcPass, Pass* dstPass)
{

}

//-----------------------------------------------------------------------
const String& ShaderExInstancedViewportsFactory::getType() const
{
	return ShaderExInstancedViewports::Type;
}


//-----------------------------------------------------------------------
SubRenderState*	ShaderExInstancedViewportsFactory::createInstanceImpl()
{
	return N_new ShaderExInstancedViewports;
}

//-----------------------------------------------------------------------

}
}

#endif
