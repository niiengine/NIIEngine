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

#include "OgreShaderExHardwareSkinningTechnique.h"

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderExDualQuaternionSkinning.h"
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
HardwareSkinningTechnique::HardwareSkinningTechnique() :
	mBoneCount(0),
	mWeightCount(0),
	mCorrectAntipodalityHandling(false),
	mScalingShearingSupport(false),
	mDoBoneCalculations(false)
{
}

//-----------------------------------------------------------------------
HardwareSkinningTechnique::~HardwareSkinningTechnique()
{
}

//-----------------------------------------------------------------------
void HardwareSkinningTechnique::setHardwareSkinningParam(ushort boneCount, ushort weightCount, bool correctAntipodalityHandling, bool scalingShearingSupport)
{
	mBoneCount = std::min<ushort>(boneCount, 256);
	mWeightCount = std::min<ushort>(weightCount, 4);
	mCorrectAntipodalityHandling = correctAntipodalityHandling;
	mScalingShearingSupport = scalingShearingSupport;
}

//-----------------------------------------------------------------------
void HardwareSkinningTechnique::setDoBoneCalculations(bool doBoneCalculations)
{
	mDoBoneCalculations = doBoneCalculations;
}

//-----------------------------------------------------------------------
ushort HardwareSkinningTechnique::getBoneCount()
{
	return mBoneCount;
}

//-----------------------------------------------------------------------
ushort HardwareSkinningTechnique::getWeightCount()
{
	return mWeightCount;
}

//-----------------------------------------------------------------------
bool HardwareSkinningTechnique::hasCorrectAntipodalityHandling()
{
	return mCorrectAntipodalityHandling;
}

//-----------------------------------------------------------------------
bool HardwareSkinningTechnique::hasScalingShearingSupport()
{
	return mScalingShearingSupport;
}

//-----------------------------------------------------------------------
Operand::OpMask HardwareSkinningTechnique::indexToMask(int index)
{
	switch(index)
	{
	case 0: return Operand::OPM_X;
	case 1: return Operand::OPM_Y;
	case 2: return Operand::OPM_Z;
	case 3: return Operand::OPM_W;
	default: N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Illegal value", "HardwareSkinningTechnique::indexToMask");
	}
}

//-----------------------------------------------------------------------
void HardwareSkinningTechnique::write(const HardwareSkinningTechnique* hardSkin)
{
	mWeightCount = hardSkin->mWeightCount;
	mBoneCount = hardSkin->mBoneCount;
	mDoBoneCalculations = hardSkin->mDoBoneCalculations;
	mCorrectAntipodalityHandling = hardSkin->mCorrectAntipodalityHandling;
	mScalingShearingSupport = hardSkin->mScalingShearingSupport;
}

}
}

#endif


