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

#ifndef _NII_BoostThread_H_
#define _NII_BoostThread_H_

#include <boost/thread/tss.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

namespace NII
{
    /** 给定指针,实施调用NII_DELETE的方法
    @remark 把自定义删除政策输入到外部库(如 boost 中线程)
    */
    template<typename T>
    void deletePtr(T * ptr)
    {
        N_delete ptr;
    }
}

// 定义一个临时互斥控制
#define N_mutex1                                mutable boost::recursive_mutex N_mutex1_name;
// 自动锁定/解除一个临时互斥控制
#define N_mutex1_lock                           boost::recursive_mutex::scoped_lock niiAutoMutexLock(N_mutex1_name);
// 定义一个常量命名互斥控制
#define N_mutex(name)                           boost::recursive_mutex name;
// 定义一个命名互斥控制
#define N_mutable_mutex(name)                   mutable boost::recursive_mutex name;
// 定义一个静态命名互斥控制
#define N_static_mutex(name)                    static boost::recursive_mutex name;
// 在具体范围内锁定一个互斥控制
#define N_lock_mutex(name)                      boost::recursive_mutex::scoped_lock n_mutex1(name);
// 在具体范围内锁定一个互斥控制
#define NII_LOCK_MUTEX_NAMED(mutexName, lockName)   boost::recursive_mutex::scoped_lock lockName(mutexName);
// 和 N_mutex1 一样,但使用指针加解锁.
#define NII_AUTO_SHARED_MUTEX                       mutable boost::recursive_mutex *N_mutex1_name;
// 和 N_mutex1_lock 一样,但使用指针加解锁
#define NII_LOCK_AUTO_SHARED_MUTEX                  N_assert(N_mutex1_name, "error"); boost::recursive_mutex::scoped_lock niiAutoMutexLock(*N_mutex1_name);
// 动态创建一个互斥控制
#define NII_NEW_AUTO_SHARED_MUTEX                   N_assert(!N_mutex1_name, "error"); N_mutex1_name = new boost::recursive_mutex();
// 动态删除一个互斥控制
#define NII_DELETE_AUTO_SHARED_MUTEX                N_assert(N_mutex1_name, "error"); delete N_mutex1_name;
// 从另一个动态互斥控制中复制
#define NII_COPY_AUTO_SHARED_MUTEX(from)            N_assert(!N_mutex1_name, "error"); N_mutex1_name = from;
// 设置互斥控制为无效
#define NII_SET_AUTO_SHARED_MUTEX_NULL              N_mutex1_name = 0;
// 是不是有效的互斥控制
#define NII_MUTEX_CONDITIONAL(mutex)                if (mutex)
// 同步结构
#define NII_THREAD_SYNCHRONISER(sync)               boost::condition sync;
// 配置
#define NII_THREAD_WAIT(sync, mutex, lock)          sync.wait(lock);
// 唤醒一个等待该condition发生的线程
#define NII_THREAD_NOTIFY_ONE(sync)                 sync.notify_one();
// 唤醒所有等待该condition发生的线程
#define NII_THREAD_NOTIFY_ALL(sync)                 sync.notify_all();
// Read-Write互为作用事件(加解锁)
#define N_rw_mutex(name)                            mutable boost::shared_mutex name
// 读锁(加解锁)
#define NII_LOCK_RW_MUTEX_READ(name)                boost::shared_lock<boost::shared_mutex> n_mutex1(name)
// 写锁(加解锁)
#define NII_LOCK_RW_MUTEX_WRITE(name)               boost::unique_lock<boost::shared_mutex> n_mutex1(name)
// 线程局部指针
#define N_Thread_Obj(T, var)                        boost::thread_specific_ptr<T> var
// 初始化线程指针
#define NII_THREAD_POINTER_INIT(var)                var(&deletePtr)
// 线程指针值
#define NII_THREAD_POINTER_VAR(T, var)              boost::thread_specific_ptr<T> var (&deletePtr<T>)
// 线程指针所指向的实体
#define N_Thread_Get_Obj(var)                       var.get()
// 设置线程指针所指向的实体
#define N_Thread_Set_Obj(var, expr)                 var.reset(expr)
// 删除线程指针
#define NII_THREAD_POINTER_DELETE(var)              var.reset(0)
// 线程对象和创建消除函数
#define NII_THREAD_TYPE                             boost::thread
// 创建一个工作线程
#define NII_THREAD_CREATE(name, worker)             boost::thread * name = N_new_t(boost::thread)(worker);
// 删除一个工作线程
#define NII_THREAD_DESTROY(name)                    N_delete_t(name, thread)
// 并发数量
#define NII_THREAD_HARDWARE_CONCURRENCY             boost::thread::hardware_concurrency()
// 获取当前线程
#define NII_THREAD_CURRENT_ID                       boost::this_thread::get_id()

// 实用函数
#define NII_THREAD_SLEEP(ms)                        boost::this_thread::sleep(boost::posix_time::millisec(ms));

#endif