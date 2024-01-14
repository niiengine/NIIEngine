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
#ifndef OGRE3D_DX9_BACKBUFFER_GRABBER_H
#define OGRE3D_DX9_BACKBUFFER_GRABBER_H

#include "DX9_BackbufferGrabber.h"
#include "OgreRenderWindow.h"

class Ogre3D_DX9_BackbufferGrabber : public DX9_BackbufferGrabber
{
public:
	void InitBackbufferGrabber(Ogre::ViewWindow* renderWindow, int _width, int _height);
};

#endif
