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
#include "OgreD3D9Resource.h"
#include "OgreD3D9ResourceManager.h"
#include "OgreD3D9RenderSystem.h"

namespace Ogre
{
    OGRE_STATIC_MUTEX_INSTANCE(D3D9Resource::msDeviceAccessMutex);

    D3D9Resource::D3D9Resource()
    {               
        D3D9RenderSystem::getResourceManager()->_notifyResourceCreated(static_cast<D3D9Resource*>(this));       
    }

    D3D9Resource::~D3D9Resource()
    {       
        D3D9RenderSystem::getResourceManager()->_notifyResourceDestroyed(static_cast<D3D9Resource*>(this));
    }
    
    void D3D9Resource::lockDeviceAccess()
    {       
        D3D9_DEVICE_ACCESS_LOCK;                                
    }
    
    void D3D9Resource::unlockDeviceAccess()
    {       
        D3D9_DEVICE_ACCESS_UNLOCK;              
    }
}
