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

#ifndef _NII_DEFAULT_Thread_H_
#define _NII_DEFAULT_Thread_H_

// 定义一个临时互斥控制
#define N_mutex1
// 自动锁定/解除一个临时互斥控制
#define N_mutex1_lock
// 定义一个常量命名互斥控制
#define N_mutex(name)
// 定义一个命名互斥控制
#define N_mutable_mutex(name)
// 定义一个静态命名互斥控制
#define N_static_mutex(name)
// 在具体范围内锁定一个互斥控制
#define N_lock_mutex(name)
// 在具体范围内锁定一个互斥控制
#define NII_LOCK_MUTEX_NAMED(mutexName, lockName)
// 和 N_mutex1 一样,但使用指针加解锁.
#define NII_AUTO_SHARED_MUTEX
// 和 N_mutex1_lock 一样,但使用指针加解锁
#define NII_LOCK_AUTO_SHARED_MUTEX
// 动态创建一个互斥控制
#define NII_NEW_AUTO_SHARED_MUTEX
// 动态删除一个互斥控制
#define NII_DELETE_AUTO_SHARED_MUTEX
// 从另一个动态互斥控制中复制
#define NII_COPY_AUTO_SHARED_MUTEX(from)
// 设置互斥控制为无效
#define NII_SET_AUTO_SHARED_MUTEX_NULL
// 是不是有效的互斥控制
#define NII_MUTEX_CONDITIONAL(name)                 if(true)
// 同步结构
#define NII_THREAD_SYNCHRONISER(sync)
// 配置
#define NII_THREAD_WAIT(sync, lock)
// 唤醒一个等待该condition发生的线程
#define NII_THREAD_NOTIFY_ONE(sync)
// 唤醒所有等待该condition发生的线程
#define NII_THREAD_NOTIFY_ALL(sync)
// Read-Write互为作用事件(加解锁)
#define N_rw_mutex(name)
// 读锁(加解锁)
#define NII_LOCK_RW_MUTEX_READ(name)
// 写锁(加解锁)
#define NII_LOCK_RW_MUTEX_WRITE(name)
// 线程局部指针
#define N_Thread_Obj(T, var)                        T * var
// 初始化线程指针
#define NII_THREAD_POINTER_INIT(var)                var(0)
// 线程指针值
#define NII_THREAD_POINTER_VAR(T, var)              T * var = 0
// 设置线程指针所指向的实体
#define N_Thread_Set_Obj(var, expr)                 var = expr
// 线程指针所指向的实体
#define N_Thread_Get_Obj(var)                       var
// 删除线程指针
#define NII_THREAD_POINTER_DELETE(var)              { N_delete var; var = 0; }
// 工作类(继承用)
#define NII_THREAD_WORKER_INHERIT
// 实用函数
#define NII_THREAD_SLEEP(ms)

#endif
