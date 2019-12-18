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

#ifndef _NII_PLATINFO_H_
#define _NII_PLATINFO_H_

#include "NiiPreInclude.h"
#include "NiiSimdInfo.h"
#include "NiiString.h"

namespace NII
{
    class CPUInfo;
    class SimdInfo;
    class SystemInfo;

    /** 平台特征
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlatInfo
    {
    public:
        /** 启动系统平台
        @version NIIEngine 3.0.0
        */
        static void startup();

        /** 关闭系统平台
        @version NIIEngine 3.0.0
        */
        static void shutdown();

        /** 获取CPU的供应商
        @version NIIEngine 3.0.0
        */
        static const String & getCPUVendor();

        /** 获取CPU的型号
        @remark 这个方法比较像 getIdentifier
        @version NIIEngine 3.0.0
        */
        static const String & getCPUModel();

        /** 获取CPU的运行速度(概念)
        @version NIIEngine 3.0.0
        */
        static const String & getCPUBogoMips();

        /** 获取cpu出厂工作频率
        @remark 出厂工作频率
        @version NIIEngine 3.0.0
        */
        static NIIf getCPUFrequency();

        /** 获取CPU的标识
        @remark CPU的型号,性能描述
        @version NIIEngine 3.0.0
        */
        static const String & getIdentifier();

        /** 检测CPU是否支持指定并行算术
        @remark 这东西直接和系统cpu有关,并不是想支持就支持
        @return 如果支持则返回true
        @version NIIEngine 3.0.0
        */
        static bool isSupport(SimdInfo::CpuSimd type);

        /** 记录平台信息
        @version NIIEngine 3.0.0
        */
        static void log(Log * log);
    private:
        static CPUInfo * mCPU;
        static SimdInfo * mCPUext;
        static SystemInfo * mSys;
    };
}
#endif