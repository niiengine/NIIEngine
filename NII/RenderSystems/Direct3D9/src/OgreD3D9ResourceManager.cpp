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
#include "OgreD3D9HardwarePixelBuffer.h"

namespace Ogre
{
    
    //-----------------------------------------------------------------------
    D3D9ResourceManager::D3D9ResourceManager()
    {
        mResourceCreationPolicy = RCP_CREATE_ON_ALL_DEVICES;
        mDeviceAccessLockCount = 0;
        mAutoHardwareBufferManagement = false;
    }

    //-----------------------------------------------------------------------
    D3D9ResourceManager::~D3D9ResourceManager()
    {
    
    }
 
    //-----------------------------------------------------------------------
    void D3D9ResourceManager::setCreationPolicy(D3D9ResourceCreationPolicy creationPolicy)
    {
        mResourceCreationPolicy = creationPolicy;
    }

    //-----------------------------------------------------------------------
    D3D9ResourceCreationPolicy D3D9ResourceManager::getCreationPolicy() const
    {
        return mResourceCreationPolicy;
    }

     //-----------------------------------------------------------------------
    void D3D9ResourceManager::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
    {               
            OGRE_LOCK_MUTEX(mResourcesMutex);

        ResourceContainerIterator it = mResources.begin();
        while (it != mResources.end())
        {
            (*it)->notifyOnDeviceCreate(d3d9Device);
            ++it;
        }               
    }

     //-----------------------------------------------------------------------
    void D3D9ResourceManager::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
    {
            OGRE_LOCK_MUTEX(mResourcesMutex);

        ResourceContainerIterator it = mResources.begin();
        while (it != mResources.end())
        {
            (*it)->notifyOnDeviceDestroy(d3d9Device);
            ++it;
        }   
    }

     //-----------------------------------------------------------------------
    void D3D9ResourceManager::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
    {
            OGRE_LOCK_MUTEX(mResourcesMutex);

        ResourceContainerIterator it = mResources.begin();
        while (it != mResources.end())
        {
            (*it)->notifyOnDeviceLost(d3d9Device);
            ++it;
        }   
    }

     //-----------------------------------------------------------------------
    void D3D9ResourceManager::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
    {       
            OGRE_LOCK_MUTEX(mResourcesMutex);

        ResourceContainerIterator it = mResources.begin();
        while (it != mResources.end())
        {
            (*it)->notifyOnDeviceReset(d3d9Device);
            ++it;           
        }       
    }

    //-----------------------------------------------------------------------
    void D3D9ResourceManager::_notifyResourceCreated(D3D9Resource* pResource)
    {       
            OGRE_LOCK_MUTEX(mResourcesMutex);
        mResources.insert(pResource);
    }
    
    //-----------------------------------------------------------------------
    void D3D9ResourceManager::_notifyResourceDestroyed(D3D9Resource* pResource)
    {       
            OGRE_LOCK_MUTEX(mResourcesMutex);

        mResources.erase(pResource);
    }
    
    //-----------------------------------------------------------------------
    void D3D9ResourceManager::lockDeviceAccess()
    {   
        N_assert(mDeviceAccessLockCount >= 0);
        mDeviceAccessLockCount++;
        if (mDeviceAccessLockCount == 1)
        {                   
            OGRE_LOCK_RECURSIVE_MUTEX(mResourcesMutex);
            D3D9Resource::lockDeviceAccess();
            D3D9HardwarePixelBuffer::lockDeviceAccess();
        }
    }

    //-----------------------------------------------------------------------
    void D3D9ResourceManager::unlockDeviceAccess()
    {
        N_assert(mDeviceAccessLockCount > 0);     
        mDeviceAccessLockCount--;               
        if (mDeviceAccessLockCount == 0)
        {                       
            D3D9HardwarePixelBuffer::unlockDeviceAccess();
            D3D9Resource::unlockDeviceAccess();         
            OGRE_UNLOCK_RECURSIVE_MUTEX(mResourcesMutex);           
        }
    }
}
