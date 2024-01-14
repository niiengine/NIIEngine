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

#include "OgreShaderProgramSet.h"
#include "OgreShaderProgramManager.h"
#include "OgreResourceLoadManager.h"

namespace Ogre {
namespace RTShader {

//-----------------------------------------------------------------------------
ProgramSet::ProgramSet()
{
	
}

//-----------------------------------------------------------------------------
ProgramSet::~ProgramSet()
{
	if (mVSCpuProgram != NULL)
	{
		ProgramManager::getSingleton().destroyCpuProgram(mVSCpuProgram);
		mVSCpuProgram = NULL;
	}

	if (mPSCpuProgram != NULL)
	{
		ProgramManager::getSingleton().destroyCpuProgram(mPSCpuProgram);
		mPSCpuProgram = NULL;
	}
				
	mVSGpuProgram.setNull();					
	mPSGpuProgram.setNull();	
}

//-----------------------------------------------------------------------------
void ProgramSet::setCpuVertexProgram(Program* vsCpuProgram)
{
	mVSCpuProgram = vsCpuProgram;
}

//-----------------------------------------------------------------------------
Program* ProgramSet::getCpuVertexProgram()
{
	return mVSCpuProgram;
}

//-----------------------------------------------------------------------------
void ProgramSet::setCpuFragmentProgram(Program* psCpuProgram)
{
	mPSCpuProgram = psCpuProgram;
}

//-----------------------------------------------------------------------------
Program* ProgramSet::getCpuFragmentProgram()
{
	return mPSCpuProgram;
}

//-----------------------------------------------------------------------------
void ProgramSet::setGpuVertexProgram(GpuProgram * vsGpuProgram)
{
	mVSGpuProgram = vsGpuProgram;
}

//-----------------------------------------------------------------------------
GpuProgram * ProgramSet::getGpuVertexProgram()
{
	return mVSGpuProgram;
}

//-----------------------------------------------------------------------------
void ProgramSet::setGpuFragmentProgram(GpuProgram * psGpuProgram)
{
	mPSGpuProgram = psGpuProgram;
}

//-----------------------------------------------------------------------------
GpuProgram * ProgramSet::getGpuFragmentProgram()
{
	return mPSGpuProgram;
}

}
}
