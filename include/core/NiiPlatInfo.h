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