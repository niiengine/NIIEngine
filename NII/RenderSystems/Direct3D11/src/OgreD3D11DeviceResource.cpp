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
#include "OgreD3D11DeviceResource.h"


namespace Ogre
{
    D3D11DeviceResource::D3D11DeviceResource()
    {
        D3D11DeviceResourceManager::get()->notifyResourceCreated(this);
    }

    D3D11DeviceResource::~D3D11DeviceResource()
    {
        D3D11DeviceResourceManager::get()->notifyResourceDestroyed(this);
    }

    // ------------------------------------------------------------------------
    D3D11DeviceResourceManager* gs_D3D11DeviceResourceManager = NULL;

    D3D11DeviceResourceManager* D3D11DeviceResourceManager::get()
    {
        return gs_D3D11DeviceResourceManager;
    }

    D3D11DeviceResourceManager::D3D11DeviceResourceManager()
    {
        N_assert(gs_D3D11DeviceResourceManager == NULL);
        gs_D3D11DeviceResourceManager = this;
    }

    D3D11DeviceResourceManager::~D3D11DeviceResourceManager()
    {
        N_assert(mResources.empty());
        N_assert(gs_D3D11DeviceResourceManager == this);
        gs_D3D11DeviceResourceManager = NULL;
    }

    void D3D11DeviceResourceManager::notifyResourceCreated(D3D11DeviceResource* deviceResource)
    {
        N_assert(std::find(mResources.begin(), mResources.end(), deviceResource) == mResources.end());
        mResources.push_back(deviceResource);
    }

    void D3D11DeviceResourceManager::notifyResourceDestroyed(D3D11DeviceResource* deviceResource)
    {
        std::vector<D3D11DeviceResource*>::iterator it = std::find(mResources.begin(), mResources.end(), deviceResource);
        N_assert(it != mResources.end());
        mResources.erase(it);

        std::vector<D3D11DeviceResource*>::iterator itCopy = std::find(mResourcesCopy.begin(), mResourcesCopy.end(), deviceResource);
        if(itCopy != mResourcesCopy.end())
            *itCopy = NULL;
    }

    void D3D11DeviceResourceManager::notifyDeviceLost(D3D11Device* device)
    {
        N_assert(mResourcesCopy.empty()); // reentrancy is not expected nor supported
        mResourcesCopy = mResources;
        for(std::vector<D3D11DeviceResource*>::iterator it = mResourcesCopy.begin(), it_end = mResourcesCopy.end(); it != it_end; ++it)
            if(D3D11DeviceResource* deviceResource = *it)
                deviceResource->notifyDeviceLost(device);
        mResourcesCopy.clear();
    }

    void D3D11DeviceResourceManager::notifyDeviceRestored(D3D11Device* device)
    {
        N_assert(mResourcesCopy.empty()); // reentrancy is not expected nor supported
        mResourcesCopy = mResources;
        for(std::vector<D3D11DeviceResource*>::iterator it = mResourcesCopy.begin(), it_end = mResourcesCopy.end(); it != it_end; ++it)
            if(D3D11DeviceResource* deviceResource = *it)
                deviceResource->notifyDeviceRestored(device);
        mResourcesCopy.clear();
    }

}
