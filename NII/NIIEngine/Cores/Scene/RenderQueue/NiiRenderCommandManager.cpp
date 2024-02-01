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
#include "NiiDrawCallManager.h"
#include "NiiLogManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RenderCommandGroup
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    template<> RenderCommandManager * Singleton<RenderCommandManager>::mOnly = 0;
    //------------------------------------------------------------------------
    RenderCommandGroup::RenderCommandGroup():
        mRenderSys(0)
    {
    }
    //------------------------------------------------------------------------
    RenderCommandGroup::~RenderCommandGroup()
    {
    }
    //------------------------------------------------------------------------
    void RenderCommandGroup::run()
    {
        RenderCommandList::iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
            (*i)->run(this);
    }
    //------------------------------------------------------------------------
    RenderCommand * RenderCommandGroup::create(RenderCommand::Type type, bool fence)
    {
        RenderCommand * unit = N_Only(DrawCall).createUnit(type);
        mUnitList.push_back(unit);
        return unit;
    }
    //------------------------------------------------------------------------
    void RenderCommandGroup::remove(Nidx idx)
    {
        N_assert(idx < mUnitList.size(), "error");
        RenderCommandList::iterator i = mUnitList.begin();
        std::advance(i, idx);
        N_Only(DrawCall).destroyUnit(*i);
        mUnitList.erase(i);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RenderCommandManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RenderCommandManager::RenderCommandManager()
    {
    
    }
    //------------------------------------------------------------------------
    RenderCommandManager::~RenderCommandManager()
    {
        GroupList::iterator i, iend = mGroupList.end();
        for(i = mGroupList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }

        mGroupList.clear();
        
        FactoryList::iterator j, jend = mFactoryList.end();
        for(j = mFactoryList.begin(); j != jend; ++j)
        {
            N_delete j->second;
        }
        mFactoryList.clear();
    }
    //------------------------------------------------------------------------
    void RenderCommandManager::add(RenderCommandFactory * factory)
    {
        mFactoryList.insert(FactoryList::value_type(factory->getType(), factory));
        N_Only(Log).log(_I18n("DrawCallUnit工厂类型: ") + N_conv(factory->getType()) + _I18n(" 注册成功!"));
    }
    //------------------------------------------------------------------------
    void RenderCommandManager::remove(RenderCommandFactory * factory)
    {
        FactoryList::iterator i, iend = mFactoryList.end();
        for(i = mFactoryList.begin(); i != iend; ++i)
        {
            if(i->second == factory)
            {
                N_delete i->second;
                mFactoryList.erase(i);
                N_Only(Log).log(_I18n("DrawCallUnit工厂类型: ") + N_conv(factory->getType()) + _I18n(" 卸除成功!"));
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    RenderCommandGroup * RenderCommandManager::create(DrawCallGroupID id)
    {
        GroupList::iterator i = mGroupList.find(id);
        if(i == mGroupList.end())
        {
            mGroupList.insert(Npair(id, N_new RenderCommandGroup()));
        }
        return i->second;
    }
    //------------------------------------------------------------------------
    RenderCommandGroup * RenderCommandManager::get(DrawCallGroupID id) const
    {
        GroupList::const_iterator i = mGroupList.find(id);
        if(i != mGroupList.end())
        {
            return i->second;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void RenderCommandManager::destroy(DrawCallGroupID id)
    {
        GroupList::const_iterator i = mGroupList.find(id);
        if(i != mGroupList.end())
        {
            N_delete i->second;
            mGroupList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void RenderCommandManager::run(void * arg)
    {

    }
    //------------------------------------------------------------------------
    RenderCommand * RenderCommandManager::createUnit(RenderCommand::Type type) const
    {
        FactoryList::const_iterator i = mFactoryList.find(type);
        if(i == mFactoryList.end())
            return 0;
        return 0;
    }
    //------------------------------------------------------------------------
    void RenderCommandManager::destroyUnit(RenderCommand * unit) const
    {
        FactoryList::const_iterator i = mFactoryList.find(unit->getType());
        if (i != mFactoryList.end())
            i->second->destroy(unit);
    }
    //------------------------------------------------------------------------
}
