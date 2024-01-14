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

#include "OgreGLES2Plugin.h"
#include "OgreRoot.h"
#include "OgreGLES2RenderSystem.h"

namespace Ogre {
    static const String sPluginName = "OpenGL ES 2.0 RenderSystem";

    GLES2Plugin::GLES2Plugin()
        : mRenderSys(0)
    {
    }

    const String& GLES2Plugin::getName() const
    {
        return sPluginName;
    }

    void GLES2Plugin::install()
    {
        mRenderSys = N_new GLES2RenderSystem();

        Root::getSingleton().addRenderSystem(mRenderSys);
    }

    void GLES2Plugin::initialise()
    {
        // nothing to do
    }

    void GLES2Plugin::shutdown()
    {
        // nothing to do
    }

    void GLES2Plugin::uninstall()
    {
        N_delete mRenderSys;
        mRenderSys = 0;
    }
}
