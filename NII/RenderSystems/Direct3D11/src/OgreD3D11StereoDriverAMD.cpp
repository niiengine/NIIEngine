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

#include "OgreD3D11StereoDriverAMD.h"

#if OGRE_NO_QUAD_BUFFER_STEREO == 0

#include "OgreD3D11RenderWindow.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	D3D11StereoDriverAMD::D3D11StereoDriverAMD()
	{
	}
	//---------------------------------------------------------------------
	D3D11StereoDriverAMD::~D3D11StereoDriverAMD()
	{
	}
	//---------------------------------------------------------------------
	bool D3D11StereoDriverAMD::addRenderWindow(D3D11RenderWindowBase* renderWindow)
	{
		return false;
	}
	//---------------------------------------------------------------------
	bool D3D11StereoDriverAMD::removeRenderWindow(const String& renderWindowName)
	{
		return false;
	}
	//---------------------------------------------------------------------
	bool D3D11StereoDriverAMD::isStereoEnabled(const String& renderWindowName)
	{
		return false;
	}
	//---------------------------------------------------------------------
	bool D3D11StereoDriverAMD::getBufferType(ViewportType colourBuffer)
	{
		return false;
	}
	//---------------------------------------------------------------------
}
#endif
