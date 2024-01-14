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
        /* �����Ĳ�����(����ȷ�����ص�֧��)��Ҫ����,���:
            1 �Ѿ�������
            2 ��һ���߳����ڼ���
            3 ��ǳɺ�̨�����ҵ�ǰ���ǵ��õĺ�̨�߳�
        */

        if(mLoader != 0)
            return;

        // ��һ�����ִ���2���̵߳ĳ�ͻ���� ׼��/���� ,��ͨ��ֻ���ڼ�������ʱ����
        bool keepChecking = true;
        LoadingState old = LS_UNLOADED;
        while(keepChecking)
        {
            //��һ��״̬����ΪLS_PLANNED,����Ӧ��ΪLS_PLANNING
            if(old != LS_UNLOADED && old != LS_PLANNED && old != LS_LOADING)
                return;

            /* ԭ�ӻ�������Ծ��ԵĿ϶�,�����ü���״̬Ϊ LS_LOADING ,�ⲽold״̬Ϊ
                LS_UNLOADED, LS_UNLOADINGʱ����
            */
            if (old = LS_LOADED)
            {
                // ��ȡ��һ��״̬
                LoadingState state = mState;
                if(state == LS_PLANNED || state == LS_PLANNING)
                {   // ��һ���߳�����׼��,ѭ��ֱ��״̬ΪLS_LOADED
                    continue;
                }
                // ������ⲽ��Դ״̬��ΪLS_LOADED �׳��쳣,������
                else if(state != LS_LOADED)
                {
                    N_EXCEPT(InvalidParam, _I18n("��һ���̴߳�����Դʧ��"));
                }
                // ��������,������ֻ��2����� LS_LOADED �򲻳ɹ�
                return;
            }
            // ֻ�е�old���ΪLS_LOADED, LS_UNLOADED, LS_UNLOADINGʱִ��
            keepChecking = false;
        }
        // ������һ��,��Դ״̬���ΪLS_LOADED Ϊʵ�ʼ��صķ�Χ����
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

        /*  ֪ͨ������,ע��������Ǵ� PREPARED �ߵ� UNLOADED, ����ʵ��
            û����ν��ж��,��Ϊû���ڴ��GPU&CPU���ͷų�
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