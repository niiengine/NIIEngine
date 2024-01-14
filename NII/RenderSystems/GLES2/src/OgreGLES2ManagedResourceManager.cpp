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

namespace Ogre
{

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN

    //-----------------------------------------------------------------------
    GLES2ManagedResourceManager::GLES2ManagedResourceManager()
    {
    }
    //-----------------------------------------------------------------------
    GLES2ManagedResourceManager::~GLES2ManagedResourceManager()
    {
    
    }
    //-----------------------------------------------------------------------
    void GLES2ManagedResourceManager::notifyOnContextLost()
    {               
        ResourceContainerIterator it = mResources.begin();
        while (it != mResources.end())
        {
            (*it)->notifyOnContextLost();
            ++it;
        }               
    }
    //-----------------------------------------------------------------------
    void GLES2ManagedResourceManager::notifyOnContextReset()
    {
        ResourceContainerIterator it = mResources.begin();
        while (it != mResources.end())
        {
            (*it)->notifyOnContextReset();
            ++it;
        }   
    }
    //-----------------------------------------------------------------------
    void GLES2ManagedResourceManager::_notifyResourceCreated(GLES2ManagedResource* pResource)
    {           
        mResources.push_back(pResource);
    }
    //-----------------------------------------------------------------------
    void GLES2ManagedResourceManager::_notifyResourceDestroyed(GLES2ManagedResource* pResource)
    {       
        ResourceContainerIterator it = std::find( mResources.begin(), mResources.end(), pResource );
        if(it != mResources.end())
            mResources.erase(it);
    }

#endif

}
