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
#include "Ogre3D_DX9_BackbufferGrabber.h"

void Ogre3D_DX9_BackbufferGrabber::InitBackbufferGrabber(Ogre::ViewWindow* renderWindow, int _width, int _height)
{
	LPDIRECT3DDEVICE9 DX9Device;
	renderWindow->getPlatformData("D3DDEVICE", &DX9Device);
	DX9_BackbufferGrabber::InitBackbufferGrabber(DX9Device, _width, _height);
}
