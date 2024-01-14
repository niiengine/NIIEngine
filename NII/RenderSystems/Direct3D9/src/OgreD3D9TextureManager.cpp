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
#include "OgreD3D9TextureManager.h"
#include "OgreD3D9Texture.h"
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreD3D9Mappings.h"
#include "OgreRoot.h"
#include "OgreD3D9RenderSystem.h"

namespace Ogre 
{
    D3D9TextureManager::D3D9TextureManager() : TextureManager()
    {       
        // register with group manager
        ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }
    
    D3D9TextureManager::~D3D9TextureManager()
    {
        // unregister with group manager
        ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);

    }

    Resource* D3D9TextureManager::createImpl(const String& name, 
        ResourceHandle handle, const String& group, bool isManual, 
        ManualResourceLoader* loader, const PropertyData* createParams)
    {
        D3D9Texture* ret = N_new D3D9Texture(this, name, handle, group, isManual, loader);       
        return ret;
    }

    PixelFormat D3D9TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
    {
        // Basic filtering
        D3DFORMAT d3dPF = D3D9Mappings::_getPF(D3D9Mappings::_getClosestSupportedPF(format));

        // Calculate usage
        DWORD d3dusage = 0;
        D3DPOOL pool = D3DPOOL_MANAGED;
        if (D3D9RenderSystem::isDirectX9Ex())
        {
            pool = D3DPOOL_DEFAULT;
        }
        if (usage & TU_RENDERTARGET) 
        {
            d3dusage |= D3DUSAGE_RENDERTARGET;
            pool = D3DPOOL_DEFAULT;
        }
        if (usage & TU_DYNAMIC)
        {
            d3dusage |= D3DUSAGE_DYNAMIC;
            pool = D3DPOOL_DEFAULT;
        }

        IDirect3DDevice9* pCurDevice = D3D9RenderSystem::getActiveD3D9Device();

        // Use D3DX to adjust pixel format
        switch(ttype)
        {
        case TEX_TYPE_1D:
        case TEX_TYPE_2D:
            D3DXCheckTextureRequirements(pCurDevice, NULL, NULL, NULL, d3dusage, &d3dPF, pool);
            break;
        case TEX_TYPE_3D:
            D3DXCheckVolumeTextureRequirements(pCurDevice, NULL, NULL, NULL, NULL, d3dusage, &d3dPF, pool);
            break;
        case TEX_TYPE_CUBE_MAP:
            D3DXCheckCubeTextureRequirements(pCurDevice, NULL, NULL, d3dusage, &d3dPF, pool);
            break;
        };

        return D3D9Mappings::_getPF(d3dPF);
    }

    bool D3D9TextureManager::isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
        bool preciseFormatOnly)
    {
        if (!preciseFormatOnly)
            format = getNativeFormat(ttype, format, usage);

        D3D9RenderSystem* rs = static_cast<D3D9RenderSystem*>(
            Root::getSingleton().getRenderSystem());

        return rs->_checkTextureFilteringSupported(ttype, format, usage);
    }
}
