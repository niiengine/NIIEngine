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
#include "NiiRenderEffectManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    template<> RenderEffectManager * Singleton<RenderEffectManager>::mOnly = 0;
    //------------------------------------------------------------------------
    RenderEffectManager::RenderEffectManager()
    {
        N_Only(Log).log(_I18n("RenderEffectManager singleton setup"));
    }
    //------------------------------------------------------------------------
    RenderEffectManager::~RenderEffectManager()
    {
        // Destroy any RenderEffect objects we created that still exist.
        while (!mEffectList.empty())
            destroy(mEffectList.begin()->first);

        // Remove(destroy) all the RenderEffectFactory objects.
        while (!mEffectFactoryList.empty())
            removeEffect(mEffectFactoryList.begin()->first);

        N_Only(Log).log(_I18n("RenderEffectManager singleton destroyed "));
    }
    //------------------------------------------------------------------------
    void RenderEffectManager::removeEffect(EffectID name)
    {
        RenderEffectRegistry::iterator i(mEffectFactoryList.find(name));

        // exit if no factory exists for this type
        if (i == mEffectFactoryList.end())
            return;

        N_Only(Log).log(_I18n("Unregistered RenderEffect named '") + N_conv(name) + _T("'"));

        N_delete i->second;
        mEffectFactoryList.erase(name);
    }
    //------------------------------------------------------------------------
    bool RenderEffectManager::isEffectAvailable(EffectID name) const
    {
        return mEffectFactoryList.find(name) != mEffectFactoryList.end();
    }
    //------------------------------------------------------------------------
    RenderEffect * RenderEffectManager::create(EffectID name, UI::Window * win)
    {
        RenderEffectRegistry::iterator i(mEffectFactoryList.find(name));

        // throw if no factory exists for this type
        if(i == mEffectFactoryList.end())
            N_EXCEPT(NotExist, _I18n("No RenderEffect has been registered with the name '") + N_conv(name) + _T("'"));

        RenderEffect * effect = i->second->create(win);

        // here we keep track of the factory used to create the effect object.
        mEffectList[effect] = i->second;

        return effect;
    }
    //------------------------------------------------------------------------
    void RenderEffectManager::destroy(RenderEffect * effect)
    {
        EffectCreatorMap::iterator i(mEffectList.find(effect));

        // We will only destroy effects that we created (and throw otherwise)
        if (i == mEffectList.end())
            N_EXCEPT(InvalidRequest,
                _I18n("The given RenderEffect was not created by the ")
                _I18n("RenderEffectManager - perhaps you created it directly?"));

        // Get string of object address before we delete it.
        Ntchar addr_buff[32];
        Nsntprintf(addr_buff, sizeof(addr_buff), _T("%p"), static_cast<void *>(effect));

        // use the same factory to delete the RenderEffect as what created it
        i->second->destroy(effect);
        mEffectList.erase(i);

        N_Only(Log).log(_I18n("Destroyed RenderEffect object at ") + String(addr_buff));
    }
    //------------------------------------------------------------------------
    /*RenderEffectManager * RenderEffectManager::getOnlyPtr()
    {
        return mOnly;
    }
    //---------------------------------------------------------------------
    RenderEffectManager & RenderEffectManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //------------------------------------------------------------------------
}