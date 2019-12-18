/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_Win32Timer_H_
#define _NII_Win32Timer_H_

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX) && defined(_MSC_VER)
    #define NOMINMAX // required to stop windows.h messing up std::min
#endif
#include "windows.h"

namespace NII
{
    class _EngineAPI Timer : public TimerAlloc
    {
    public:
        Timer();
        ~Timer();

        bool setOption(const String & strKey, const void * value);

        /// 重置时间器
        void reset();

        /** 返回初始化或复位后度过的毫秒
        @remark 毫秒
        */
        Nul getMS();

        /** 返回初始化或复位后度过的微秒
        @remark 微秒
        */
        Nul getUS();

        /** 返回初始化或复位后度过的毫秒,CPU测量时间
        @remark 毫秒
        */
        Nul getCostMS();
    private:
        clock_t mZeroClock;

        DWORD mStartTick;
        LONGLONG mLastTime;
        LARGE_INTEGER mStartTime;
        LARGE_INTEGER mFrequency;

        DWORD_PTR mTimerMask;
    };
}
#endif
