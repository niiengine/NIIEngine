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

#ifndef _NII_PocoThread_H_
#define _NII_PocoThread_H_

#if N_PLAT == N_PLAT_WIN32
    #define POCO_NO_UNWINDOWS
#endif

#include <Poco/ThreadLocal.h>
#include <Poco/Mutex.h>
#include <Poco/Condition.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/RWLock.h>

// 定义一个临时互斥控制
#define N_mutex1                                    mutable Poco::Mutex N_mutex1_name;
// 自动锁定/解除一个临时互斥控制
#define N_mutex1_lock                               Poco::Mutex::ScopedLock niiAutoMutexLock(N_mutex1_name);
// 定义一个常量命名互斥控制
#define N_mutex(name)                               Poco::Mutex name;
// 定义一个命名互斥控制
#define N_mutable_mutex(name)                       mutable Poco::Mutex name;
// 定义一个静态命名互斥控制
#define N_static_mutex(name)                        static Poco::Mutex name;
// 在具体范围内锁定一个互斥控制
#define N_lock_mutex(name)                          Poco::Mutex::ScopedLock n_mutex1(name);
// 在具体范围内锁定一个互斥控制
#define NII_LOCK_MUTEX_NAMED(mutexName, lockName)   Poco::Mutex::ScopedLock lockName(mutexName);
// 和 N_mutex1 一样,但使用指针加解锁.
#define NII_AUTO_SHARED_MUTEX                       mutable Poco::Mutex * N_mutex1_name;
// 和 N_mutex1_lock 一样,但使用指针加解锁
#define NII_LOCK_AUTO_SHARED_MUTEX                  N_assert(N_mutex1_name, "error"); Poco::Mutex::ScopedLock niiAutoMutexLock(*N_mutex1_name);
// 动态创建一个互斥控制
#define NII_NEW_AUTO_SHARED_MUTEX                   N_assert(!N_mutex1_name, "error"); N_mutex1_name = new Poco::Mutex();
// 动态删除一个互斥控制
#define NII_DELETE_AUTO_SHARED_MUTEX                N_assert(N_mutex1_name, "error"); delete N_mutex1_name;
// 从另一个动态互斥控制中复制
#define NII_COPY_AUTO_SHARED_MUTEX(from)            N_assert(!N_mutex1_name, "error"); N_mutex1_name = from;
// 设置互斥控制为无效
#define NII_SET_AUTO_SHARED_MUTEX_NULL              N_mutex1_name = 0;
// 是不是有效的互斥控制
#define NII_MUTEX_CONDITIONAL(mutex)                if(mutex)
// 同步结构
#define NII_THREAD_SYNCHRONISER(sync)               Poco::Condition sync;
// 配置
#define NII_THREAD_WAIT(sync, mutex, lock)          sync.wait(mutex);
// 唤醒一个等待该condition发生的线程
#define NII_THREAD_NOTIFY_ONE(sync)                 sync.signal();
// 唤醒所有等待该condition发生的线程
#define NII_THREAD_NOTIFY_ALL(sync)                 sync.broadcast();
// Read-Write互为作用事件(加解锁)
#define N_rw_mutex(name)                          mutable Poco::RWLock name
// 读锁(加解锁)
#define NII_LOCK_RW_MUTEX_READ(name)                Poco::RWLock::ScopedLock n_mutex1(name, false);
// 写锁(加解锁)
#define NII_LOCK_RW_MUTEX_WRITE(name)               Poco::RWLock::ScopedLock n_mutex1(name, true);
// 线程局部指针
#define N_Thread_Obj(T, var)                        Poco::ThreadLocal<SharedPtrST<T> > var
// 初始化线程指针
#define NII_THREAD_POINTER_INIT(var)                var()
// 线程指针值
#define NII_THREAD_POINTER_VAR(T, var)              Poco::ThreadLocal<SharedPtrST<T> > var
// 线程指针所指向的实体
#define N_Thread_Get_Obj(var)                       var.get().get()
// 设置线程指针所指向的实体
#define N_Thread_Set_Obj(var, expr)                 { var.get().setNull(); var.get().bind(expr); }
// 删除线程指针
#define NII_THREAD_POINTER_DELETE(var)              var.get().setNull()
// 线程对象和创建消除函数
#define NII_THREAD_TYPE Poco::Thread
// 创建一个工作线程
#define NII_THREAD_CREATE(name, worker)             Poco::Thread * name = N_new_t(Poco::Thread)(); name->start(worker);
// 删除一个工作线程
#define NII_THREAD_DESTROY(name)                    N_delete_t(name, Thread)
// 并发数量
#define NII_THREAD_HARDWARE_CONCURRENCY             2
// 获取当前线程
#define NII_THREAD_CURRENT_ID (NCount)              Poco::Thread::current()
// (hardware concurrency is not accessible via POCO atm)
// 实用函数
#define NII_THREAD_SLEEP(ms)                        Poco::Thread::sleep(ms);

#endif