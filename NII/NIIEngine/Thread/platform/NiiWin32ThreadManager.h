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

#ifndef _NII_WIN32_THREAD_MANAGER_H_
#define _NII_WIN32_THREAD_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiThreadManager.h"

namespace NII
{
    /** 一个通用用途的 请求/回馈 式后台工作队列.
    @remark 这个个默认的实现,一个工作队列开始一个线程池和为处理请求提供队列.
    */
    class _EngineAPI Win32ThreadMananger : public ThreadManager
    {
    public:
        Win32ThreadMananger(const String & name = StrUtil::BLANK);

        virtual ~Win32ThreadMananger();

        /// @copydetails ThreadManager::shutdown
        virtual void shutdown();

        /// @copydetails ThreadManager::startup
        virtual void startup(bool force = true);

        /// @copydetails ThreadManager::main
        virtual void main();
    protected:
        /// 被单独线程调用,如果队列里条目将立即返回,或者当新条目加入到另一端时挂起线程.
        virtual void waitForNextRequest();

        /// 通知一个线程已通过render system被自身注册
        virtual void notifyThreadRegistered();

        /// 通知唤起工作线程
        virtual void notifyFire();
    protected:
        N_mutable_mutex(mInitMutex)                       ///< 初始化的通告互为作用事件 (在等initCondition前必须上锁)
        NII_THREAD_SYNCHRONISER(mInitSync)          ///< 同步所初始化系统的条件锁
        NII_THREAD_SYNCHRONISER(mRequestCondition)  ///< 同步"请求"条件锁
        NCount mNumThreadsRegisteredWithRS;         ///< 已经注册的渲染系统线程数量
#if NII_THREAD_SUPPORT
        typedef vector<NII_THREAD_TYPE *>::type WorkerThreadList;
        WorkerThreadList mWorkers;
#endif
    };
}
#endif