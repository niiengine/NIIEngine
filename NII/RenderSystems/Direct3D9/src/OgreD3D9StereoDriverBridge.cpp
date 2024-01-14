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

#include "OgreD3D9StereoDriverBridge.h"

#if OGRE_NO_QUAD_BUFFER_STEREO == 0

#include "OgreD3D9StereoDriverAMD.h"
#include "OgreD3D9StereoDriverNVIDIA.h"

namespace Ogre
{
	//-----------------------------------------------------------------------------
	D3D9StereoDriverBridge::D3D9StereoDriverBridge(StereoModeType stereoMode)
	{
		mPimpl = NULL;
		mIsNvapiInitialized = false;
		mStereoMode = stereoMode;

		if (SMT_FRAME_SEQUENTIAL != mStereoMode)
			return;

		// Both initialize and set driver mode need to be called to determine whether NVIDIA stereo is available
		NvAPI_Status nvStatus = NvAPI_Initialize();
		if (NVAPI_OK == nvStatus)
			nvStatus = NvAPI_Stereo_SetDriverMode(NVAPI_STEREO_DRIVER_MODE_DIRECT);
			
		if (NVAPI_OK == nvStatus)
		{
			NvAPI_ShortString nvapiStatusMessage;
			NvAPI_GetErrorMessage(nvStatus, nvapiStatusMessage);
			mIsNvapiInitialized = true;
			mPimpl = new D3D9StereoDriverNVIDIA();
		}
		else
		{
			// Assume the AMD implementation, since the device must be created before verifying AMD QBS
			mPimpl = new D3D9StereoDriverAMD();
		}
	}
	//-----------------------------------------------------------------------------
	D3D9StereoDriverBridge::~D3D9StereoDriverBridge()
	{
		if (NULL != mPimpl)
		{
			delete mPimpl;
			mPimpl = NULL;
		}

		if (mIsNvapiInitialized)
		{
			NvAPI_Status nvStatus = NvAPI_Unload();
			NvAPI_ShortString nvapiStatusMessage;
			NvAPI_GetErrorMessage(nvStatus, nvapiStatusMessage);
		}
	}
	//-----------------------------------------------------------------------------
	template<> D3D9StereoDriverBridge* Ogre::Singleton<D3D9StereoDriverBridge>::msSingleton = 0;
	D3D9StereoDriverBridge& D3D9StereoDriverBridge::getSingleton(void)
	{
		N_assert(msSingleton);
		return (*msSingleton);
	}
	//-----------------------------------------------------------------------------
	D3D9StereoDriverBridge* D3D9StereoDriverBridge::getSingletonPtr(void)
	{
		return msSingleton;
	}
	//-----------------------------------------------------------------------------
	StereoModeType D3D9StereoDriverBridge::getStereoMode() const
	{
		return mStereoMode;
	}
	//-----------------------------------------------------------------------------
	bool D3D9StereoDriverBridge::addRenderWindow(D3D9RenderWindow* renderWindow) const
	{
		if (NULL != mPimpl)
			return mPimpl->addRenderWindow(renderWindow);

		return false;
	}
	//-----------------------------------------------------------------------------
	bool D3D9StereoDriverBridge::removeRenderWindow(const String& name) const
	{
		if (NULL != mPimpl)
			return mPimpl->removeRenderWindow(name);

		return false;
	}
	//-----------------------------------------------------------------------------
	bool D3D9StereoDriverBridge::isStereoEnabled(const String& renderWindowName) const
	{
		if (NULL != mPimpl)
			return mPimpl->isStereoEnabled(renderWindowName);

		return false;
	}
	//-----------------------------------------------------------------------------
	bool D3D9StereoDriverBridge::getBufferType(ViewportType colourBuffer) const
	{
		if (NULL != mPimpl)
			return mPimpl->getBufferType(colourBuffer);

		return false;
	}
	//-----------------------------------------------------------------------------
}
#endif

