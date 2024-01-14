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
#include "OgreD3D9VideoMode.h"

namespace Ogre 
{
    String D3D9VideoMode::getDescription() const
    {
        unsigned int colourDepth = 16;
        if( mDisplayMode.Format == D3DFMT_X8R8G8B8 ||
            mDisplayMode.Format == D3DFMT_A8R8G8B8 ||
            mDisplayMode.Format == D3DFMT_R8G8B8 )
            colourDepth = 32;

        return StringUtil::format("%d x %d @ %d-bit colour", mDisplayMode.Width, mDisplayMode.Height, colourDepth );
    }

    unsigned int D3D9VideoMode::getColourDepth() const
    {
        unsigned int colourDepth = 16;
        if( mDisplayMode.Format == D3DFMT_X8R8G8B8 ||
            mDisplayMode.Format == D3DFMT_A8R8G8B8 ||
            mDisplayMode.Format == D3DFMT_R8G8B8 )
            colourDepth = 32;

        return colourDepth;
    }
}
