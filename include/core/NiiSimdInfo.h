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
        /** cpu������������
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
#elif N_CPU == NII_CPU_PPC
            CS_ALTIVEC     = 1 << 15,
#elif N_CPU == NII_CPU_ARM
            CS_VFP         = 1 << 16,
            CS_VFP3        = 1 << 17
            CS_NEON        = 1 << 18,
#endif
            CS_NONE        = 0
        };
        
        SimdInfo();
        virtual ~SimdInfo();

        /** ��ȡCPU�ı�ʶ
        @remark CPU���ͺ�,��������
        */
        const String & getIdentifier(void);

        /** ���CPU�Ƿ�֧��ָ����������
        @return ���֧���򷵻�true.
        */
        bool isSupport(CpuSimd type);
    protected:
        String mCpuName;    ///< cpu����
        Nui32 mCpuMark;     ///< �ڲ�֧�ֵ�����
    };
}
#endif