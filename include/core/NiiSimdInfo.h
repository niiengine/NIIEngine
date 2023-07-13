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

#ifndef _NII_SIMD_INFO_H_
#define _NII_SIMD_INFO_H_

#include "NiiPreInclude.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineInner SimdInfo : public CPUAlloc
    {
    public:
        /** cpu并行算术特征
        @version NIIEngine 3.0.0
        */
        enum CpuSimd
        {
#if N_CPU == NII_CPU_X86
            CS_SSE         = 1 << 0,
            CS_SSE2        = 1 << 1,
            CS_SSE3        = 1 << 2,
            CS_SSSE3       = 1 << 3,
            CS_SSE4        = 1 << 4,
            CS_SSE42       = 1 << 5,
            CS_MMX         = 1 << 6,
            CS_MMX2        = 1 << 7,
            CS_3DNOW       = 1 << 8,
            CS_3DNOWEXT    = 1 << 9,
            CS_CMOV        = 1 << 10,
            CS_TSC         = 1 << 11,
            CS_FPU         = 1 << 12,
            CS_PRO         = 1 << 13,
            CS_HTT         = 1 << 14,
            CS_AVX         = 1 << 15,
            CS_AVX2        = 1 << 16,
#elif N_CPU == NII_CPU_PPC
            CS_ALTIVEC     = 1 << 17,
#elif N_CPU == NII_CPU_ARM
            CS_VFP         = 1 << 18,
            CS_VFP3        = 1 << 19
            CS_NEON        = 1 << 20,
#endif
            CS_NONE        = 0
        };
    public:
        SimdInfo();

        /** 获取CPU的标识
        @remark CPU的型号,性能描述
        */
        inline const String & getIdentifier() const { return mCpuName; }

        /** 检测CPU是否支持指定并行算术
        @return 如果支持则返回true.
        */
        bool isSupport(CpuSimd type);
    protected:
        String mCpuName;
        Nui32 mCpuMark;
    };

    /** 加速处理器信息
    @remark 核显一般和CPU使用同一个内存控制器
    @version NIIEngine 4.0.0
    */
    class _EngineInner ApuInfo : public CPUAlloc
    {
        friend class SimdManager;
    public:
        ApuInfo();

        inline const String & getIdentifier() const { return mPlatformName; }
    protected:
        String mPlatformName;
        String mPlatformVendor;
        String mPlatformVersion;
        Nui32 mDeviceCount;
    };

    typedef vector<ApuInfo *>::type ApuInfoList;
}
#endif