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

#include "NiiPreProcess.h"
#include "NiiDynamicModuleManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    template<> DynamicModuleManager * Singleton<DynamicModuleManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    /*DynamicModuleManager * DynamicModuleManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    DynamicModuleManager & DynamicModuleManager::getOnly()
    {  
        N_assert(mOnly, "error");
        return(*mOnly);  
    }*/
    //-----------------------------------------------------------------------
    DynamicModuleManager::DynamicModuleManager()
    {
    }
    //-----------------------------------------------------------------------
    DynamicModule * DynamicModuleManager::load(const String & lib)
    {
        DynList::iterator i = mLibList.find(lib);
        if(i != mLibList.end())
        {
            return i->second;
        }
        else
        {
            DynamicModule * pLib = N_new DynamicModule(lib);
            pLib->load();        
            mLibList[lib] = pLib;
            return pLib;
        }
    }
    //-----------------------------------------------------------------------
    void DynamicModuleManager::unload(DynamicModule * lib)
    {
        DynList::iterator i = mLibList.find(lib->getName());
        if(i != mLibList.end())
        {
            mLibList.erase(i);
        }
        lib->unload();
        N_delete lib;
    }
    //-----------------------------------------------------------------------
    DynamicModuleManager::~DynamicModuleManager()
    {
        for(DynList::iterator it = mLibList.begin(); it != mLibList.end(); ++it )
        {
            it->second->unload();
            N_delete it->second;
        }
        mLibList.clear();
    }
    //----------------------------------------------------------------------
}