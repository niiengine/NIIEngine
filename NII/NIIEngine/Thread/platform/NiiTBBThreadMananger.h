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

#ifndef _NII_TBB_THREADMANAGER_H_
#define _NII_TBB_THREADMANAGER_H_

#include "NiiWorkQueue.h"

namespace NII
{
    /** Implementation of a general purpose request / response style background work queue.
    @remark
        This implementation utilises tbb's task system for the ThreadManager implementation.
    */
    class _EngineAPI TBBThreadMananger : public ThreadManager
    {
    public:
        TBBThreadMananger(const String & name = StrUtil::BLANK);

        virtual ~TBBThreadMananger();

        /// @copydetails ThreadManager::startup
        virtual void startup(bool force = true);

        /// @copydetails ThreadManager::shutdown
        virtual void shutdown();

        /// @copydetails ThreadManager::main
        virtual void main();

        /// Register the current thread with the rendersystem
        void _registerThreadWithRenderSystem();
    protected:
        /// 通知工作线程
        virtual void notifyFire();
    private:
        tbb::task_scheduler_init mTaskScheduler;
        tbb::task_group mTaskGroup;
        /// Synchronise registering threads with the RenderSys
        N_mutable_mutex(mRegisterRSMutex);
        set<tbb::tbb_thread::id> mRegisteredThreads;
    };
}
#endif