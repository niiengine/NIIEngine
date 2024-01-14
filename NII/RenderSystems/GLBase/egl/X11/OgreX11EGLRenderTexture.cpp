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

#if 0
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreRoot.h"
#include "OgreStringConverter.h"

#include "OgreX11EGLRenderTexture.h"
#include "OgreX11EGLContext.h"
#include "OgreX11EGLSupport.h"

#include <iostream>
#include <climits>

namespace Ogre {

    X11EGLPBuffer::X11EGLPBuffer( X11EGLSupport* glsupport, PixelComponentType format, size_t width, size_t height )
        : EGLPBuffer(glsupport, format, width, height)
    {
        mGlDisplay = glsupport->getGLDisplay();
        mGLSupport = glsupport;
        initEGLPBuffer();
    }

    X11EGLPBuffer::~X11EGLPBuffer()
    {

    }

}
#endif

