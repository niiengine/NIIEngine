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
        bool readStat(Nui64 & user, Nui64 & nice, Nui64 & sys, Nui64 & idle, Nui64 & io);
    private:
        typedef vector<CPUDev *>::type CPUDevs;
    protected:
        CPUDevs mCPUDevs;   ///< cpu列表
        NCount mCount;      ///< cpu个数
        Nui64 mUserTime;    ///< 辅助计算负荷
        Nui64 mNiceTime;    ///< 辅助计算负荷
        Nui64 mSysTime;     ///< 辅助计算负荷
        Nui64 mIdleTime;    ///< 辅助计算负荷
        Nui64 mIOTime;      ///< 辅助计算负荷
    };
}
#endif