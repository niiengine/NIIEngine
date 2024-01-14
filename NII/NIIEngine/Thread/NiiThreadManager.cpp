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
#include "NiiThreadManager.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiTimer.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TaskGroup
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
	ThreadManager::TaskGroup::TaskGroup() :
        mValidJobID(0),
        mUpdateTimeOut(0),
        mRunThreadCount(0),
        mThreadCount(0),
        mPause(false),
        mRun(false)
    {
    }
    //------------------------------------------------------------------------
	ThreadManager::TaskGroup::~TaskGroup()
    {
        shutdown();
        removeAllInput();
        removeAllOutput();
    }
    //------------------------------------------------------------------------
    bool ThreadManager::TaskGroup::startup(NCount threadCnt, ThreadMain * task, void * arg)
    {
        mQuitMutex.lock();
        if(!mRun)
        {
            mRun = true;
            mQuitMutex.unlock();

            mExistMutex.lock();           
            mRunThreadCount = 0;
            mExistMutex.unlock();
            
            for(NCount i = 0; i < threadCnt; ++i)
            {
                Thread * tmp = N_new Thread(_T(""), task, arg, false);
                tmp->setPriority(0);
                tmp->create(true);
                mThreadList.push_back(tmp);
            }

            mQuitMutex.lock();
            while(mThreadCount < threadCnt)
            {
                mQuitCondition.wait(mQuitMutex);
            }
            mQuitMutex.unlock();
            
            return true;
        }
        mQuitMutex.unlock();
        return false;
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::update()
    {
        Nul start = N_Engine().getTimer()->getMS();
        Nul current = 0;
        
        while(1)
        {
            JobResult * jrs = 0;
            JobPrc * prc = 0;
            mOutMutex.lock();
            if(mResponseList.empty())
            {
                mOutMutex.unlock();
                break;
            }
            else
            {
                jrs = mResponseList.front().first;
                prc = mResponseList.front().second;
                mResponseList.pop_front();
                mOutMutex.unlock();
            }
            
            if(jrs)
            {
                prc->handle(jrs);
                N_delete jrs;
            }
            
            if(mUpdateTimeOut)
            {
                current = N_Engine().getTimer()->getMS();
                if(current - start > mUpdateTimeOut)
                    break;
            }
        }
    }
    //------------------------------------------------------------------------
    bool ThreadManager::TaskGroup::shutdown()
    {
        ScopeLock temp(mQuitMutex);
        if (mRun)
        {
            mRun = false;
            mQuitCondition.notify();
            while(mThreadCount > 0)
            {
                mQuitCondition.wait(mQuitMutex);
            }
        }
		return true;
    }
    //------------------------------------------------------------------------
    bool ThreadManager::TaskGroup::pause()
    {
        mQuitMutex.lock();
        if (mRun)
        {
            mQuitMutex.unlock();
        }
        else
        {
            mQuitMutex.unlock();
            return false;
        }
        
        mExistMutex.lock();
        mPause = true;
        while (mRunThreadCount > 0)
        {
            mExistCondition.wait(mExistMutex);
        }
        mExistMutex.unlock();
        return true;
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::resume()
    {
        ScopeLock temp(mExistMutex);
        mPause = false;
        mPauseCondition.notifyAll();
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::run(void *)
    {
        Job * jrq = 0;
        JobPrc * jp = 0;
        mQuitMutex.lock();
        ++mThreadCount;
        mQuitCondition.notifyAll();
        mQuitMutex.unlock();

        while(1)
        {
            mQuitMutex.lock();
            if(mRun)
            {
                mInMutex.lock();
                if (mRequestList.empty())
                {
                    mInMutex.unlock();
                    mQuitCondition.wait(mQuitMutex);
                    mQuitMutex.unlock();
                    continue;
                }
                else
                {
                    jrq = mRequestList.front().first;
                    jp = mRequestList.front().second;
                    jrq->mStage = Job::PS_Process;
                    mRequestList.pop_front();
                    mInMutex.unlock();
                    mQuitMutex.unlock();
                }
            }
            else
            {
                mQuitMutex.unlock();
                break;
            }
            
            mExistMutex.lock();
            mExistCondition.notifyAll();
            while(mPause)
            {
                mPauseCondition.wait(mExistMutex);
            }
            ++mRunThreadCount;
            mExistMutex.unlock();

            process(jrq, jp, false);
            
            mExistMutex.lock();
            --mRunThreadCount;
            mExistCondition.notifyAll();
            mExistMutex.unlock();
        }
        mQuitMutex.lock();
        --mThreadCount;
        mQuitCondition.notifyAll();
        mQuitMutex.unlock();
    }
    //------------------------------------------------------------------------
    RequestID ThreadManager::TaskGroup::add(Job * job, JobPrc * prc, bool sync)
    {
        mQuitMutex.lock();

        if(job->mID == 0)
            job->mID = ++mValidJobID;

        if(!sync)
        {
            if(!mRun)
            {
                mQuitMutex.unlock();
                return 0;
            }            
            mInMutex.lock();
            mRequestList.push_back(Npair(job, prc));
            mInMutex.unlock();
            job->mStage = Job::PS_Request;

            mQuitCondition.notify();
            mQuitMutex.unlock();
            return job->mID;
        }
        mQuitMutex.unlock();
        
        process(job, prc, true);
        return job->mID;
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::abort(RequestID id)
    {
        mInMutex.lock();
        PrcMap::iterator rq, rqend = mRequestList.end();
        for(rq = mRequestList.begin(); rq != rqend; ++rq)
        {
            if(rq->first->getID() == id)
            {
                rq->first->stop();
                break;
            }
        }
        mInMutex.unlock();
        
        mOutMutex.lock();
        ResponseMap::iterator rsq, rsqend = mResponseList.end();
        for(rsq = mResponseList.begin(); rsq != rsqend; ++rsq)
        {
            if(rsq->first->getJob()->getID() == id)
            {
                rsq->first->abort();
                break;
            }
        }
        mOutMutex.unlock();
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::abort(JobPrc * prc)
    {
        mInMutex.lock();
        PrcMap::iterator rq, rqend = mRequestList.end();
        for(rq = mRequestList.begin(); rq != rqend; ++rq)
        {
            if(rq->second == prc)
            {
                rq->first->stop();
            }
        }
        mInMutex.unlock();
        
        mOutMutex.lock();
        ResponseMap::iterator rsq, rsqend = mResponseList.end();
        for(rsq = mResponseList.begin(); rsq != rsqend; ++rsq)
        {
            if(rsq->second == prc)
            {
                rsq->first->abort();
            }
        }
        mOutMutex.unlock();
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::abort()
    {
        ScopeLock temp(mQuitMutex);
        if (mRun)
        {
            mInMutex.lock();
            PrcMap::iterator rq, rqend = mRequestList.end();
            for(rq = mRequestList.begin(); rq != rqend; ++rq)
            {
                rq->first->stop();
            }
            mInMutex.unlock();

            mOutMutex.lock();
            ResponseMap::iterator rsq, rsqend = mResponseList.end();
            for(rsq = mResponseList.begin(); rsq != rsqend; ++rsq)
            {
                rsq->first->abort();
            }
            mOutMutex.unlock();
        }
        else
        {
            PrcMap::iterator rq, rqend = mRequestList.end();
            for(rq = mRequestList.begin(); rq != rqend; ++rq)
            {
                rq->first->stop();
            }
            ResponseMap::iterator rsq, rsqend = mResponseList.end();
            for(rsq = mResponseList.begin(); rsq != rsqend; ++rsq)
            {
                rsq->first->abort();
            }
        }
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::remove(const Job::Identifier & id)
    {
        mInMutex.lock();
        PrcMap::iterator i, iend = mRequestList.begin();
        for(i = mRequestList.begin(); i != iend; ++i)
        {
            if(i->first->equal(id))
            {
                i->first->stop();
                N_delete i->first;
                if(i->second->isAutoDestroy())
                {
                    N_delete i->second;
                }

                if(id.isOnly())
                {
                    mRequestList.erase(i);
                    break;
                }
                else
                {
                    PrcMap::iterator temp = i++;
                    mRequestList.erase(temp);
                }
            }
            else
            {
                ++i;
            }
        }
        mInMutex.unlock();
    }
    //------------------------------------------------------------------------
    NCount ThreadManager::TaskGroup::getOutputSize() const
    { 
        ScopeLock temp(mOutMutex); 
        return mResponseList.size();
    }
    //------------------------------------------------------------------------
    NCount ThreadManager::TaskGroup::getInputSize() const
    { 
        ScopeLock temp(mInMutex); 
        return mRequestList.size(); 
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::removeAllInput()
    {
        mInMutex.lock();
        PrcMap::iterator rq, rqend = mRequestList.end();
        for(rq = mRequestList.begin(); rq != rqend; ++rq)
        {
            rq->first->stop();
            N_delete rq->first;
            if(rq->second->isAutoDestroy())
            {
                N_delete rq->second;
            }
            mRequestList.erase(rq);
        }
        mInMutex.unlock();
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::removeAllOutput()
    { 
        mOutMutex.lock();
        ResponseMap::iterator rsq, rsqend = mResponseList.end();
        for(rsq = mResponseList.begin(); rsq != rsqend; ++rsq)
        {
            rsq->first->abort();
            N_delete rsq->first;
            if(rsq->second->isAutoDestroy())
            {
                N_delete rsq->second;
            }
            mResponseList.erase(rsq);
        }
        mOutMutex.unlock();
    }
    //------------------------------------------------------------------------
    NCount ThreadManager::TaskGroup::getWorkingCount() const
    { 
        ScopeLock temp(mExistMutex); 
        return mRunThreadCount; 
    }
    //------------------------------------------------------------------------
    bool ThreadManager::TaskGroup::isRun() const
    { 
        ScopeLock temp(mQuitMutex); 
        return mRun; 
    }
    //------------------------------------------------------------------------
    bool ThreadManager::TaskGroup::isWorking() const
    {
        {
            ScopeLock temp(mOutMutex);
            if (mResponseList.size())
                return true;
        }
        {
            ScopeLock temp(mInMutex);
            if (mRequestList.size())
                return true;
        }
        {
            ScopeLock temp(mExistMutex);
            if(mRunThreadCount != 0)
                return true;
        }
        
        return false;
    }
    //------------------------------------------------------------------------
    void ThreadManager::TaskGroup::process(Job * jrq, JobPrc * jp, bool syn)
    {
        jp->onPrcBegin(jrq);
        JobResult * jrs = jp->handle(jrq);
        jp->onPrcEnd(jrq);
        if(jrs)
        {
            if(!jrs->isComplete())
            {
                jrs->retry(jp);
                N_delete jrs;
                return;
            }

            if(syn)
            {
                jp->handle(jrs);
                N_delete jrs;
            }
            else
            {
                if(jrs->getJob()->isStop())
                {
                    jrs->abort();
                }
                mOutMutex.lock();
                jrq->mStage = Job::PS_Response;
                mResponseList.push_back(Npair(jrs, jp));
                mOutMutex.unlock();
            }
        }
        else
        {
            N_delete jrq;
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ThreadManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    template<> ThreadManager * Singleton<ThreadManager>::mOnly = 0;
    //------------------------------------------------------------------------
    /*ThreadManager * ThreadManager::getOnlyPtr()
    {
        return mOnly;
    }
    //------------------------------------------------------------------------
    ThreadManager & ThreadManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //------------------------------------------------------------------------
    ThreadManager::ThreadManager()
    {
    }
    //------------------------------------------------------------------------
    ThreadManager::~ThreadManager()
    {
    }
    //------------------------------------------------------------------------
    void ThreadManager::update(Nui32 type)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i, iend = mGroupList.end();
        for(i = mGroupList.begin(); i != iend; ++i)
        {
            i->second->update();
        }
    }
    //------------------------------------------------------------------------
    bool ThreadManager::startup(Nui32 type, NCount threadCnt, ThreadMain * task, void * arg)
    {
        ScopeRWLock tlock(mGroupMutex, true);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            TaskGroup * group = N_new TaskGroup();
            mGroupList.insert(Npair(type, group));
            return group->startup(threadCnt, task, arg);
        }
        else
        {
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ThreadManager::shutdown(Nui32 type)
    {
        ScopeRWLock tlock(mGroupMutex, true);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->shutdown();
            N_delete i->second;
            mGroupList.erase(i);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ThreadManager::pause(Nui32 type)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            return i->second->pause();
        }
        return false;
    }
    //------------------------------------------------------------------------
    void ThreadManager::resume(Nui32 type)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->resume();
        }
    }
    //------------------------------------------------------------------------
    void ThreadManager::setUpdateTimeOut(Nui32 type, TimeDurMS ms)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->setUpdateTimeOut(ms);
        }
    }
    //------------------------------------------------------------------------
    TimeDurMS ThreadManager::getUpdateTimeOut(Nui32 type) const
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::const_iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            return i->second->getUpdateTimeOut();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    RequestID ThreadManager::add(Nui32 type, Job * job, JobPrc * prc, bool sync)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
			job->mTypeID = type;
            return i->second->add(job, prc, sync);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ThreadManager::abort(Nui32 type, RequestID id)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->abort(id);
        }
    }
    //------------------------------------------------------------------------
    void ThreadManager::abort(Nui32 type, JobPrc * prc)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->abort(prc);
        }
    }
    //------------------------------------------------------------------------
    void ThreadManager::abort(Nui32 type)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->abort();
        }
    }
    //------------------------------------------------------------------------
    void ThreadManager::remove(Nui32 type, const Job::Identifier & id)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->remove(id);
        }
    }
    //------------------------------------------------------------------------
    NCount ThreadManager::getOutputSize(Nui32 type) const 
    { 
        ScopeRWLock tlock(mGroupMutex);
        GroupList::const_iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            return i->second->getOutputSize();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    NCount ThreadManager::getInputSize(Nui32 type) const 
    { 
        ScopeRWLock tlock(mGroupMutex);
        GroupList::const_iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            return i->second->getInputSize();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ThreadManager::removeAllInput(Nui32 type)
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->removeAllInput();
        }
    }
    //------------------------------------------------------------------------
    void ThreadManager::removeAllOutput(Nui32 type)
    { 
        ScopeRWLock tlock(mGroupMutex);
        GroupList::iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            i->second->removeAllOutput();
        }
    }
    //------------------------------------------------------------------------
    NCount ThreadManager::getWorkingCount(Nui32 type) const 
    { 
        ScopeRWLock tlock(mGroupMutex);
        GroupList::const_iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            return i->second->getWorkingCount();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool ThreadManager::isRun(Nui32 type) const
    { 
        ScopeRWLock tlock(mGroupMutex);
        GroupList::const_iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            return i->second->isRun();
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ThreadManager::isWorking(Nui32 type) const
    {
        ScopeRWLock tlock(mGroupMutex);
        GroupList::const_iterator i = mGroupList.find(type);
        if(i != mGroupList.end())
        {
            return i->second->isWorking();
        }
        return false;
    }
    //------------------------------------------------------------------------
}