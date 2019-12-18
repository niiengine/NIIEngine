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
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_CPU_INFO_H_
#define _NII_CPU_INFO_H_

#include "NiiPreInclude.h"

namespace NII
{
    class CPUDev;

    /** CPU 信息
    @remark 其实很信息并不好获取,因为被系统内核限制了
    @version NIIEngine 3.0.0
    */
    class _EngineInner CPUInfo : public CPUAlloc
    {
    public:
        enum TemperatureType
        {
            TT_C,       ///< 摄氏温度
            TT_F        ///< 华氏温度
        };
    public:
        CPUInfo();
        virtual ~CPUInfo();

        /** cpu数量
        @remark 这里一般是指多少个核心,是指多核cpu,不是指多个cpu
        @version NIIEngine 3.0.0
        */
        NCount getCount();

        /** 获取cpu出厂工作频率
        @remark 出厂工作频率
        @version NIIEngine 3.0.0
        */
        NIIf getFrequency(Nidx index = 0);

        /** 获取CPU的供应商
        @param[in] index 需要获取的cpu的下标
        @version NIIEngine 3.0.0
        */
        const String & getVendor(Nidx index = 0);

        /** 获取CPU的型号
        @param[in] index 需要获取的cpu的下标
        @version NIIEngine 3.0.0
        */
        const String & getModel(Nidx index = 0);

        /** 获取CPU的运行速度(概念)
        @param[in] index 需要获取的cpu的下标
        @version NIIEngine 3.0.0
        */
        const String & getBogoMips(Nidx index = 0);

        /** 获取总CPU符合负荷值
        @remark 百分值,辅助多线程机制
        @version NIIEngine 3.0.0
        */
        NIIi getUsedPercent();
        
        /** 获取CPU负荷状态描述
        @return 描述字符串
        @version NIIEngine 3.0.0
        */
        String getCPUUsagePercent() const;

        /** 获取cpu温度
        @param[in] type 温度记数类型
        @version NIIEngine 3.0.0
        */
        NIId getTemperature(TemperatureType type);
    private:
        bool readStat(Null & user, Null & nice, Null & sys, Null & idle, Null & io);
    private:
        typedef vector<CPUDev *>::type CPUDevs;
    protected:
        CPUDevs mCPUDevs;   ///< cpu列表
        NCount mCount;      ///< cpu个数
        Null mUserTime;     ///< 辅助计算负荷
        Null mNiceTime;     ///< 辅助计算负荷
        Null mSysTime;      ///< 辅助计算负荷
        Null mIdleTime;     ///< 辅助计算负荷
        Null mIOTime;       ///< 辅助计算负荷
    };
}
#endif