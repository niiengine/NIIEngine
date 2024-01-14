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
#include "OgreD3D9GpuProgramManager.h"
#include "OgreD3D9GpuProgram.h"
#include "OgreException.h"

namespace Ogre {
    //-----------------------------------------------------------------------------
    D3D9GpuProgramManager::D3D9GpuProgramManager()
        :GpuProgramManager()
    {
        // Superclass sets up members 

        // Register with resource group manager
        ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);

    }
    //-----------------------------------------------------------------------------
    D3D9GpuProgramManager::~D3D9GpuProgramManager()
    {
        // Unregister with resource group manager
        ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);

    }
    //-----------------------------------------------------------------------------
    Resource* D3D9GpuProgramManager::createImpl(const String& name, ResourceHandle handle, 
        const String& group, bool isManual, ManualResourceLoader* loader,
        GpuProgramType gptype, const String& syntaxCode)
    {
        if (gptype == GPT_VERTEX_PROGRAM)
        {
            return N_new D3D9GpuVertexProgram(this, name, handle, group, 
                isManual, loader);
        }
        else
        {
            return N_new D3D9GpuFragmentProgram(this, name, handle, group, 
                isManual, loader);
        }
    }
}
