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

#include "OgreGLES2ManagedResource.h"
#include "OgreGLES2ManagedResourceManager.h"
#include "OgreGLES2RenderSystem.h"

namespace Ogre
{

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    
    GLES2ManagedResource::GLES2ManagedResource()
    {               
        GLES2RenderSystem::getResourceManager()->_notifyResourceCreated(static_cast<GLES2ManagedResource*>(this));
    }

    GLES2ManagedResource::~GLES2ManagedResource()
    {   
        GLES2RenderSystem::getResourceManager()->_notifyResourceDestroyed(static_cast<GLES2ManagedResource*>(this));
    }

 #endif

}
