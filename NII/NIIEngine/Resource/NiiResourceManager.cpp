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
#include "NiiResourceManager.h"
#include "NiiException.h"
#include "NiiVFSManager.h"
#include "NiiStrConv.h"
#include "NiiResourceSchemeManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    ResourceManager::ResourceManager() :
        mValidRID(N_Max_Valid_ID),
        mMemUsage(0),
        mScriptLevel(0),
        mVerbose(VM_1)
    {
        // Init memory limit & usage
        mMemMax = N_TYPE_MAX(Nul);
    }
    //-----------------------------------------------------------------------
    ResourceManager::ResourceManager(ResourceType type) :
        mValidRID(N_Max_Valid_ID),
        mType(type),
        mMemUsage(0),
        mScriptLevel(0),
        mVerbose(VM_1)
    {
        // Init memory limit & usage
        mMemMax = N_TYPE_MAX(Nul);
    }
    //-----------------------------------------------------------------------
    ResourceManager::~ResourceManager()
    {
        destroyAll();
    }
    //-----------------------------------------------------------------------
    Resource * ResourceManager::get(ResourceID rid, GroupID gid)
    {
        Resource * re = NULL;

        Groups::iterator i;
        if(N_Only(ResourceScheme).isGlobal(gid))
        {
            i = mGroups.find(GroupGlobal);
        }
        else
        {
            i = mGroups.find(gid);
        }

        if(i != mGroups.end())
        {
            Resources::iterator j = i->second.find(rid);
            if(j != i->second.end())
            {
                re = j->second;
            }
        }
        return re;
    }
    //-----------------------------------------------------------------------
    Resource * ResourceManager::get(PrcID pid)
    {
        PrcIDMap::iterator it = mPrcIDs.find(pid);
        if(it == mPrcIDs.end())
        {
            return NULL;
        }
        else
        {
            return it->second;
        }
    }
    //-----------------------------------------------------------------------
    bool ResourceManager::isExist(ResourceID rid, GroupID gid)
    {
        return get(rid, gid) != NULL;
    }
    //-----------------------------------------------------------------------
    bool ResourceManager::isExist(PrcID pid)
    {
        return get(pid) != NULL;
    }
    //-----------------------------------------------------------------------
    Resource * ResourceManager::create(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        if(rid == 0)
            rid = genValidID();

        Resource * re = get(rid, gid);
        if(re != NULL)
            return re;

        re = createImpl(rid, gid, ls, rs, params);
        if(params)
            re->set(*params);

        addImpl(re);

        N_Only(ResourceScheme).onCreate(re);
        return re;
    }
    //-----------------------------------------------------------------------
    ResourceManager::TouchResult ResourceManager::ref(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        bool created = false;
        Resource * re = get(rid, gid);
        if(re == NULL)
        {
            created = true;
            re = create(rid, gid, ls, rs, params);
        }
        re->ref(false);
        return TouchResult(re, created);
    }
    //-----------------------------------------------------------------------
    void ResourceManager::unref(ResourceID rid, GroupID gid)
    {
        Resource * re = get(rid, gid);
        if(re != NULL)
        {
            re->unref();
        }
    }
    //-----------------------------------------------------------------------
    Resource * ResourceManager::plan(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, 
        const PropertyData * params)
    {
        Resource * re = get(rid, gid);
        if(re == NULL)
        {
            re = create(rid, gid, ls, rs, params);
        }
        re->plan();
        return re;
    }
    //-----------------------------------------------------------------------
    Resource * ResourceManager::load(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
        const PropertyData * params)
    {
        Resource * re = get(rid, gid);
        if(re == NULL)
        {
            re = create(rid, gid, ls, rs, params);
        }
        re->load();
        return re;
    }
    //-----------------------------------------------------------------------
    void ResourceManager::addImpl(Resource * res)
    {
        std::pair<Resources::iterator, bool> r;
        std::pair<PrcIDMap::iterator, bool> pid;
        Groups::iterator i;

        if(N_Only(ResourceScheme).isGlobal(res->getGroup()))
        {
            i = mGroups.find(GroupGlobal);
        }
        else
        {
            i = mGroups.find(res->getGroup());
        }

        if(i == mGroups.end())
        {
            i = mGroups.insert(Groups::value_type(res->getGroup(), Resources())).first;
        }

        r = (i->second).insert(Resources::value_type(res->getOriginID(), res));
        if(!r.second)
        {
            N_EXCEPT(UniqueRepeat, _I18n("资源ID: ") + N_conv(res->getOriginID()) + _I18n(" 已经存在."));
        }
        else
        {
            pid = mPrcIDs.insert(PrcIDMap::value_type(res->getPrc(), res));
            if(!pid.second)
            {
                N_EXCEPT(UniqueRepeat, _I18n("资源PID: ") + N_conv(res->getPrc()) + _I18n(" 已经存在"));
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::removeImpl(Resource * res)
    {
        Groups::iterator i;
        if(N_Only(ResourceScheme).isGlobal(res->getGroup()))
        {
            i = mGroups.find(GroupGlobal);
        }
        else
        {
            i = mGroups.find(res->getGroup());
        }

        if(i != mGroups.end())
        {
            Resources::iterator j = (i->second).find(res->getOriginID());
            if(j != (i->second).end())
            {
                (i->second).erase(j);
            }
            if((i->second).empty() && (i->first != GroupGlobal))
            {
                mGroups.erase(i);
            }
        }

        PrcIDMap::iterator z = mPrcIDs.find(res->getPrc());
        if(z != mPrcIDs.end())
        {
            mPrcIDs.erase(z);
        }

        N_Only(ResourceScheme).onRemove(res);
    }
    //-----------------------------------------------------------------------
    void ResourceManager::unload(ResourceID rid)
    {
        Resource * re = get(rid);
        if(re != NULL)
        {
            re->unload();
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::unload(PrcID pid)
    {
        Resource * re = get(pid);
        if(re != NULL)
        {
            re->unload();
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::unloadAll(bool resume)
    {
        Groups::iterator j, jend = mGroups.end();
        for(j = mGroups.begin(); j != jend; ++j)
        {
            Resources::iterator i, iend = (j->second).end();
            for(i = (j->second).begin(); i != iend; ++i)
            {
                if(!resume || i->second->isReloadable())
                {
                    i->second->unload();
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::reloadAll(bool resume)
    {
        Groups::iterator j, jend = mGroups.end();
        for(j = mGroups.begin(); j != jend; ++j)
        {
            Resources::iterator i, iend = (j->second).end();
            for(i = (j->second).begin(); i != iend; ++i)
            {
                if(!resume || i->second->isReloadable())
                {
                    i->second->reload();
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::unloadUnused(bool resume)
    {
        Groups::iterator j, jend = mGroups.end();
        for(j = mGroups.begin(); j != jend; ++j)
        {
            Resources::iterator i, iend = (j->second).end();
            for(i = (j->second).begin(); i != iend; ++i)
            {
                if(i->second->getRefCount() == 0)
                {
                    Resource * res = i->second;
                    if(!resume || res->isReloadable())
                    {
                        res->unload();
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::reloadUnused(bool resume)
    {
        Groups::iterator j, jend = mGroups.end();
        for(j = mGroups.begin(); j != jend; ++j)
        {
            Resources::iterator i, iend = (j->second).end();
            for(i = (j->second).begin(); i != iend; ++i)
            {
                Resource * res = i->second;
                if(res->getRefCount() == 0)
                {
                    if(!resume || res->isReloadable())
                    {
                        res->reload();
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::remove(ResourceID rid)
    {
        Resource * res = get(rid);
        if(res)
        {
            removeImpl(res);
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::remove(PrcID pid)
    {
        Resource * res = get(pid);
        if(res)
        {
            removeImpl(res);
        }
    }
    //-----------------------------------------------------------------------
    /*void ResourceManager::removeAll()
    {
        mGroups.clear();
        mPrcIDs.clear();
        N_Only(ResourceScheme).onAllRemove(this);
    }*/
    //-----------------------------------------------------------------------
    /*void ResourceManager::removeUnused()
    {
        Groups::iterator j, jend = mGroups.end();
        for(j = mGroups.begin(); j != jend; ++j)
        {
            Resources::iterator i, iend = (j->second).end();
            for(i = (j->second).begin(); i != iend; ++i)
            {
                if(i->second->getRefCount() == 0)
                {
                    remove(i->second->getPrc());
                }
            }
        }
    }*/
    //-----------------------------------------------------------------------
    void ResourceManager::destroy(ResourceID rid)
    {
        Resource * res = get(rid);
        if(res)
        {
            removeImpl(res);
            N_delete res;
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::destroy(PrcID pid)
    {
        Resource * obj = get(pid);
        if(obj)
        {
            removeImpl(obj);
            N_delete obj;
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::destroyUnused()
    {
        Groups::iterator j, jend = mGroups.end();
        for(j = mGroups.begin(); j != jend; ++j)
        {
            Resources::iterator i, iend = (j->second).end();
            for(i = (j->second).begin(); i != iend; ++i)
            {
                if(i->second->getRefCount() == 0 && i->second->isAutoDestroy())
                {
                    destroy(i->second->getPrc());
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::destroyAll()
    {
        N_Only(ResourceScheme).onAllRemove(this);

        Groups::iterator j, jend = mGroups.end();
        for(j = mGroups.begin(); j != jend; ++j)
        {
            Resources::iterator i, iend = (j->second).end();
            for(i = (j->second).begin(); i != iend; ++i)
            {
                N_delete i->second;
            }
        }
        mGroups.clear();
        mPrcIDs.clear();
    }
    //-----------------------------------------------------------------------
    ResourceID ResourceManager::genValidID()const
    {
        N_lock_mutex(mValidRIDMutex)
        ResourceID temp = ++mValidRID;
        return temp;
    }
    //-----------------------------------------------------------------------
    void ResourceManager::onTouch(Resource * res)
    {
        // TODO
    }
    //-----------------------------------------------------------------------
    void ResourceManager::onUnTouch(Resource * res)
    {
        if(res->getRefCount() == 0 && res->isAutoDestroy())
        {
            destroy(res->getOriginID());
        }
    }
    //-----------------------------------------------------------------------
    void ResourceManager::onLoad(Resource * res)
    {
        mMemUsage += res->getSize();
    }
    //-----------------------------------------------------------------------
    void ResourceManager::onUnload(Resource * res)
    {
        mMemUsage -= res->getSize();
    }
    //-----------------------------------------------------------------------
}