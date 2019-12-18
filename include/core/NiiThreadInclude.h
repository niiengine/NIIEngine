/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-6

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_THREAD_INCLUDE_H_
#define _NII_THREAD_INCLUDE_H_

#include <atomic>
#include <future>
#include <thread>
#include "NiiDefaultThread.h"

#if N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
#include <mach/mach.h>
#endif

#define N_mutex1_name mMutex

#if N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_LINUX_X || N_PLAT == N_PLAT_OSX|| N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_ANDROID
    #include <sys/types.h>
    #include <unistd.h>
    namespace NII
    {
        constexpr int N_ThreadBasePriorityLow = 15;
        constexpr int N_ThreadBasePriorityMax = 2;
        constexpr int N_ThreadBasePriorityMin = -2;
        constexpr int N_ThreadBasePriorityIdle = -15;
        constexpr int N_ThreadPriorityHigh = N_ThreadBasePriorityMax;
        constexpr int N_ThreadPriorityLow = N_ThreadBasePriorityMin;
        constexpr int N_ThreadPriorityBelowNormal = N_ThreadPriorityLow + 1;
        constexpr int N_ThreadPriorityNormal = 0;
        constexpr int N_ThreadPriorityAboveNormal = N_ThreadPriorityHigh - 1;

        typedef pid_t ThreadId;
    }
#elif N_PLAT == N_PLAT_WIN32
    #include <windows.h>
    namespace NII
    {
        typedef HANDLE ThreadId;

        constexpr int N_ThreadBasePriorityLow = THREAD_BASE_PRIORITY_LOWRT;
        constexpr int N_ThreadBasePriorityMax = THREAD_BASE_PRIORITY_MAX;
        constexpr int N_ThreadBasePriorityMin = THREAD_BASE_PRIORITY_MIN;
        constexpr int N_ThreadBasePriorityIdle = THREAD_BASE_PRIORITY_IDLE;
        constexpr int N_ThreadPriorityHigh = THREAD_PRIORITY_HIGHEST;
        constexpr int N_ThreadPriorityLow = THREAD_PRIORITY_LOWEST;
        constexpr int N_ThreadPriorityBelowNormal = THREAD_PRIORITY_BELOW_NORMAL;
        constexpr int N_ThreadPriorityNormal = THREAD_PRIORITY_NORMAL;
        constexpr int N_ThreadPriorityAboveNormal = THREAD_PRIORITY_ABOVE_NORMAL;
    }
#endif

#endif