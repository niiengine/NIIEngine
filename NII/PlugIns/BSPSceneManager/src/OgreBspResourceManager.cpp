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
#include "OgreBspResourceManager.h"
#include "OgreBspLevel.h"
#include "OgreQuake3ShaderManager.h"


namespace Ogre {

    //-----------------------------------------------------------------------
    template<> BspResourceManager* Singleton<BspResourceManager>::msSingleton = 0;
    BspResourceManager* BspResourceManager::getSingletonPtr(void)
    {
        return msSingleton;
    }
    BspResourceManager& BspResourceManager::getSingleton(void)
    {  
        assert( msSingleton );  return ( *msSingleton );  
    }
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    BspResourceManager::BspResourceManager()
    {
        mResourceType = "BspLevel";
        // Also create related shader manager (singleton managed)
        mShaderMgr = N_new Quake3ShaderManager();

        ResourceSchemeManager::getSingleton()._registerResourceManager(mResourceType, this);
    }
    //-----------------------------------------------------------------------
    BspResourceManager::~BspResourceManager()
    {
        N_delete mShaderMgr;
        ResourceSchemeManager::getSingleton()._unregisterResourceManager(mResourceType);
    }
    //-----------------------------------------------------------------------
    ResourcePtr BspResourceManager::load(const String & name, const String & group,
        bool isManual, ManualResourceLoader * loader, const PropertyData * loadParams)
    {
        // Only 1 BSP level allowed loaded at once
        removeAll();

        return ResourceManager::load(name, group, isManual, loader, loadParams);
    }
    //-----------------------------------------------------------------------
    ResourcePtr BspResourceManager::load(DataStream * stream, const String & group)
    {
        // Only 1 BSP level allowed loaded at once
        removeAll();

		ResourcePtr ret = create("bsplevel", group, true, 0);
		BspLevelPtr bspLevel = ret;
		bspLevel->load(stream);
		
        return ret;
    }
    //-----------------------------------------------------------------------
    Resource* BspResourceManager::createImpl(const String& name, ResourceHandle handle, 
        const String& group, bool isManual, ManualResourceLoader* loader, 
        const PropertyData* createParams)
    {
        return N_new BspLevel(this, name, handle, group, isManual, loader);
    }
}
