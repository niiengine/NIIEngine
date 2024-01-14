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

#ifndef _NII_TBBThread_H_
#define _NII_TBBThread_H_

#include <tbb/recursive_mutex.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/queuing_rw_mutex.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/tbb_thread.h>

// 定义一个临时互斥控制
#define N_mutex1                                mutable tbb::recursive_mutex N_mutex1_name;
// 自动锁定/解除一个临时互斥控制
#define N_mutex1_lock                           tbb::recursive_mutex::scoped_lock niiAutoMutexLock(N_mutex1_name);
// 定义一个常量命名互斥控制
#define N_mutex(name)                           tbb::recursive_mutex name;
// 定义一个命名互斥控制
#define N_mutable_mutex(name)                   mutable tbb::recursive_mutex name;
// 定义一个静态命名互斥控制
#define N_static_mutex(name)                    static tbb::recursive_mutex name;
// 在具体范围内锁定一个互斥控制
#define N_lock_mutex(name)                      tbb::recursive_mutex::scoped_lock n_mutex1(name);
// 在具体范围内锁定一个互斥控制
#define NII_LOCK_MUTEX_NAMED(mutexName, lockName)   tbb::recursive_mutex::scoped_lock lockName(mutexName);
//和 N_mutex1 一样,但使用指针加解锁.
#define NII_AUTO_SHARED_MUTEX                       mutable tbb::recursive_mutex *N_mutex1_name;
// 和 N_mutex1_lock 一样,但使用指针加解锁
#define NII_LOCK_AUTO_SHARED_MUTEX                  N_assert(N_mutex1_name, "error"); tbb::recursive_mutex::scoped_lock niiAutoMutexLock(*N_mutex1_name);
// 动态创建一个互斥控制
#define NII_NEW_AUTO_SHARED_MUTEX                   N_assert(!N_mutex1_name, "error"); N_mutex1_name = new tbb::recursive_mutex();
// 动态删除一个互斥控制
#define NII_DELETE_AUTO_SHARED_MUTEX                N_assert(N_mutex1_name, "error"); delete N_mutex1_name;
// 从另一个动态互斥控制中复制
#define NII_COPY_AUTO_SHARED_MUTEX(from)            N_assert(!N_mutex1_name, "error"); N_mutex1_name = from;
// 设置互斥控制为无效
#define NII_SET_AUTO_SHARED_MUTEX_NULL              N_mutex1_name = 0;
// 是不是有效的互斥控制
#define NII_MUTEX_CONDITIONAL(mutex)                if (mutex)
// Read-Write互为作用事件(加解锁)
#define N_rw_mutex(name)                            mutable tbb::queuing_rw_mutex name
// 读锁(加解锁)
#define NII_LOCK_RW_MUTEX_READ(name)                tbb::queuing_rw_mutex::scoped_lock n_mutex1(name, false)
// 写锁(加解锁)
#define NII_LOCK_RW_MUTEX_WRITE(name)               tbb::queuing_rw_mutex::scoped_lock n_mutex1(name, true)
// 线程局部指针
#define N_Thread_Obj(T, var)                        tbb::enumerable_thread_specific<SharedPtrST<T> > var
// 初始化线程指针
#define NII_THREAD_POINTER_INIT(var)                var()
// 线程指针值
#define NII_THREAD_POINTER_VAR(T, var)              tbb::enumerable_thread_specific<SharedPtrST<T> > var
// 线程指针所指向的实体
#define N_Thread_Get_Obj(var)                       var.local().get()
// 设置线程指针所指向的实体
#define N_Thread_Set_Obj(var, expr)                 { var.local().setNull(); var.local().bind(expr); }
// 删除线程指针
#define NII_THREAD_POINTER_DELETE(var)              var.local().setNull()
// 实用函数
#define NII_THREAD_HARDWARE_CONCURRENCY             tbb::task_scheduler_init::default_num_threads()
// 获取当前线程
#define NII_THREAD_CURRENT_ID                       tbb::this_tbb_thread::get_id() 

// 实用函数
#define NII_THREAD_SLEEP(ms)                        tbb::this_tbb_thread::sleep(tbb::tick_count::interval_t(NIId(ms)/1000))

#endif