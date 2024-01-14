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
#include "NiiResource.h"
#include "NiiResourceManager.h"
#include "NiiResourceLoadScheme.h"
#include "NiiLogManager.h"
#include "NiiException.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ResourceListener
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ResourceListener::ResourceListener()
    {
    }
    //------------------------------------------------------------------------
    ResourceListener::~ResourceListener()
    {
    }
    //------------------------------------------------------------------------
    void ResourceListener::onPlanEnd(Resource * r)
    {
    }
    //------------------------------------------------------------------------
    void ResourceListener::onLoadEnd(Resource * r)
    {
    }
    //------------------------------------------------------------------------
    void ResourceListener::onResultEnd(Resource * r)
    {
    }
    //------------------------------------------------------------------------
    void ResourceListener::onUnloadEnd(Resource * r)
    {
    }
    //------------------------------------------------------------------------
    void ResourceListener::onLoadScheme(Resource * r, LoadScheme * s, bool enable)
    {
    }
    //------------------------------------------------------------------------
    void ResourceListener::onResultScheme(Resource * r, ResultScheme * s, bool enable)
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Resource
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Resource::Resource(ScriptTypeID stid, LangID lid) :
        PropertyCmdObj(stid, lid),
        mOriginID(0),
        mCreator(0),
        mPrc(0),
        mSize(0),
        mLoader(0),
        mState(LS_UNLOADED),
        mNotifyCount(0),
        mAutoDestroy(true)
    {
    }
    //------------------------------------------------------------------------
    Resource::Resource(ScriptTypeID stid, ResourceID rid, GroupID gid,
        ResLoadScheme * rl, ResResultScheme * rs, LangID lid) :
            PropertyCmdObj(stid, lid),
            mOriginID(rid),
            mGroup(gid),
            mState(LS_UNLOADED),
            mSize(0),
            mLoader(rl),
            mResulter(rs),
            mNotifyCount(0),
            mAutoDestroy(true)
    {
        mPrc = N_Only(ResourceScheme).genValidPID();
    }
    //------------------------------------------------------------------------
    Resource::~Resource()
    {
    }
    //------------------------------------------------------------------------
    void Resource::setProperty(PropertyData *)
    {
        
    }
    //------------------------------------------------------------------------
    void Resource::plan()
    {
        LoadingState old = mState;
        if(old != LS_UNLOADED && old != LS_PLANNING)
            return;

        if(mState = LS_PLANNED)
        {
            LoadingState state = mState;
            if(state != LS_PLANNED && state != LS_LOADING && state != LS_LOADED)
            {
                N_EXCEPT(InvalidParam, _I18n("Another thread failed in resource operation"));
            }
            return;
        }

        try
        {
            N_mutex1_lock
            if(mLoader)
            {
                mLoader->plan(this);
            }
            else
            {
                if(mGroup == GroupUnknow)
                {
                    setGroup(N_Only(ResourceScheme).find(mSrc));
                }

                planImpl();
            }
        }
        catch(...)//error
        {
            mState = LS_UNLOADED;
            throw;
        }

        mState = LS_PLANNED;

        onPlanEnd();
    }
    //------------------------------------------------------------------------
    void Resource::load()
    {
        /* 做过的不锁定(减轻确保加载的支出)不要加载,如果:
            1 已经加载了
            2 另一个线程正在加载
            3 标记成后台加载且当前不是调用的后台线程
        */

        if(mLoader != 0)
            return;

        // 下一个部分处理2个线程的冲突可以 准备/加载 ,这通常只会在加载升级时发生
        bool keepChecking = true;
        LoadingState old = LS_UNLOADED;
        while(keepChecking)
        {
            //下一个状态期望为LS_PLANNED,而不应该为LS_PLANNING
            if(old != LS_UNLOADED && old != LS_PLANNED && old != LS_LOADING)
                return;

            /* 原子缓慢检测以绝对的肯定,和设置加载状态为 LS_LOADING ,这步old状态为
                LS_UNLOADED, LS_UNLOADING时跳过
            */
            if (old = LS_LOADED)
            {
                // 获取下一个状态
                LoadingState state = mState;
                if(state == LS_PLANNED || state == LS_PLANNING)
                {   // 另一个线程正在准备,循环直到状态为LS_LOADED
                    continue;
                }
                // 如果到这步资源状态不为LS_LOADED 抛出异常,和跳出
                else if(state != LS_LOADED)
                {
                    N_EXCEPT(InvalidParam, _I18n("另一个线程处理资源失败"));
                }
                // 跳出函数,到这里只有2个结果 LS_LOADED 或不成功
                return;
            }
            // 只有当old结果为LS_LOADED, LS_UNLOADED, LS_UNLOADING时执行
            keepChecking = false;
        }
        // 来到这一步,资源状态结果为LS_LOADED 为实际加载的范围锁定
        try
        {
            N_mutex1_lock

            if(mLoader)
            {
                mLoader->load(this);
                loadEndImpl();
            }
            else
            {
                if(old == LS_UNLOADED)
                    planImpl();

                if(mGroup == GroupUnknow)
                    setGroup(N_Only(ResourceScheme).find(mSrc));

                loadImpl();
                loadEndImpl();
            }
            mSize = calcSize();
        }
        catch(...)//error
        {
            mState = LS_UNLOADED;
            throw;
        }
        mState = LS_LOADED;

        ++mNotifyCount;

        if(mCreator)
            mCreator->onLoad(this);

        onLoadEnd();
    }
    //------------------------------------------------------------------------
    void Resource::setGroup(GroupID gid)
    {
        if(mGroup != gid)
        {
            GroupID ogid = mGroup;
            mGroup = gid;
            N_Only(ResourceScheme).onGroupChange(ogid, this);
        }
    }
    //------------------------------------------------------------------------
    GroupID Resource::getGroup() const
    {
        return mGroup;
    }
    //------------------------------------------------------------------------
    void Resource::notify()
    {
        ++mNotifyCount;
    }
    //------------------------------------------------------------------------
    void Resource::unload()
    {
        if(mState == LS_UNLOADING)
            return;

        {
            N_mutex1_lock
            if(mState == LS_PLANNED)
            {
                unplanImpl();
            }
            else
            {
                unloadImpl();
                unloadEndImpl();
            }
        }
        mState = LS_UNLOADED;

        /*  通知管理器,注意如果我们从 PREPARED 走到 UNLOADED, 我们实际
            没有所谓的卸载,因为没有内存从GPU&CPU中释放出
        */
        if(mState == LS_LOADED && mCreator)
            mCreator->onUnload(this);

        onUnloadEnd();
    }
    //------------------------------------------------------------------------
    void Resource::reload()
    {
        N_mutex1_lock
        if(mState == LS_LOADED)
        {
            unload();
            load();
        }
    }
    //------------------------------------------------------------------------
    bool Resource::isReloadable() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    NCount Resource::getSize() const
    {
        return mSize;
    }
    //------------------------------------------------------------------------
    NCount Resource::getOriginSize() const
    {
        return mOriginSize;
    }
    //------------------------------------------------------------------------
    bool Resource::isLoading() const
    {
        return mState == LS_LOADING;
    }
    //--------------------------------------------------------------------------
    void Resource::setAutoDestroy(bool s)
    {
        mAutoDestroy = s;
    }
    //--------------------------------------------------------------------------
    bool Resource::isAutoDestroy() const
    {
        return mAutoDestroy;
    }
    //-----------------------------------------------------------------------
    ResourceManager * Resource::getCreator()
    {
        return mCreator;
    }
    //-----------------------------------------------------------------------
    NCount Resource::getNotifyCount() const
    {
        return mNotifyCount;
    }
    //-----------------------------------------------------------------------
    void Resource::ref(bool doload)
    {
        if(doload)
            load();

        if(mCreator)
            mCreator->onTouch(this);

        ++mRefCount;
    }
    //-----------------------------------------------------------------------
    void Resource::unref()
    {
        --mRefCount;
        if(mCreator)
            mCreator->onUnTouch(this);
    }
    //-----------------------------------------------------------------------
    void Resource::add(ResourceListener * rl)
    {
        Listeners::iterator i, iend = mListenerList.end();
        for (i = mListenerList.begin(); i != iend; ++i)
        {
            if (*i == rl)
                return;
        }
        mListenerList.push_back(rl);
    }
    //-----------------------------------------------------------------------
    void Resource::remove(ResourceListener * rl)
    {
        Listeners::iterator i, iend = mListenerList.end();
        for (i = mListenerList.begin(); i != iend; ++i)
        {
            if (*i == rl)
            {
                mListenerList.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void Resource::setLoadScheme(ResLoadScheme * rls)
    {
        mLoader = rls;
    }
    //-----------------------------------------------------------------------
    ResLoadScheme * Resource::getLoadScheme() const
    {
        return mLoader;
    }
    //-----------------------------------------------------------------------
    void Resource::setResultScheme(ResResultScheme * rrs)
    {
        mResulter = rrs;
    }
    //-----------------------------------------------------------------------
    ResResultScheme * Resource::getResultScheme() const
    {
        return mResulter;
    }
    //-----------------------------------------------------------------------
    void Resource::onLoadEnd()
    {
        Listeners::iterator i, iend = mListenerList.end();
        for(i = mListenerList.begin(); i != iend; ++i)
        {
            (*i)->onLoadEnd(this);
        }
    }
    //-----------------------------------------------------------------------
    void Resource::onUnloadEnd()
    {
        Listeners::iterator i, iend = mListenerList.end();
        for (i = mListenerList.begin(); i != iend; ++i)
        {
            (*i)->onUnloadEnd(this);
        }
    }
    //-----------------------------------------------------------------------
    void Resource::onPlanEnd()
    {
        Listeners::iterator i, iend = mListenerList.end();
        for(i = mListenerList.begin(); i != iend; ++i)
        {
            (*i)->onPlanEnd(this);
        }
    }
    //-----------------------------------------------------------------------
    void Resource::read(Serializer * out) const
    {
        
    }
    //-----------------------------------------------------------------------
    void Resource::write(const Serializer * in)
    {
        
    }
    //-----------------------------------------------------------------------
    void Resource::requestImpl(LoadingState ls)
    {
    }
    //-----------------------------------------------------------------------
    void Resource::planImpl()
    {
    }
    //-----------------------------------------------------------------------
    void Resource::unplanImpl()
    {
    }
    //-----------------------------------------------------------------------
    void Resource::loadEndImpl()
    {
    }
    //-----------------------------------------------------------------------
    void Resource::unloadEndImpl()
    {
    }
    //-----------------------------------------------------------------------
}