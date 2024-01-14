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

#include "NiiPreProcess.h"
#include "NiiPlatInfo.h"
#include "NiiCpuInfo.h"
#include "NiiSysInfo.h"
#include "NiiStrConv.h"

namespace NII
{
    CPUInfo * PlatInfo::mCPU = NULL;
    SystemInfo * PlatInfo::mSys = NULL;
    //------------------------------------------------------------------------
    void PlatInfo::startup()
    {
        mCPU = N_new CPUInfo();
        mSys = N_new SystemInfo();
    }
    //------------------------------------------------------------------------
    void PlatInfo::shutdown()
    {
        if(mCPU != NULL)
        {
            N_delete mCPU;
            mCPU = NULL;
        }

        if(mSys != NULL)
        {
            N_delete mSys;
            mSys = NULL;
        }
    }
    //------------------------------------------------------------------------
    const String & PlatInfo::getCPUVendor()
    {
        return mCPU->getVendor();
    }
    //------------------------------------------------------------------------
    const String & PlatInfo::getCPUModel()
    {
        return mCPU->getModel();
    }
    //------------------------------------------------------------------------
    const String & PlatInfo::getCPUBogoMips()
    {
        return mCPU->getBogoMips();
    }
    //------------------------------------------------------------------------
    NIIf PlatInfo::getCPUFrequency()
    {
        return mCPU->getFrequency();
    }
    //------------------------------------------------------------------------
    const String & PlatInfo::getIdentifier()
    {
        return mCPU->getIdentifier();
    }
    //------------------------------------------------------------------------
    bool PlatInfo::isSupport(CPUInfo::CpuSimd type)
    {
        return mCPU->isSupport(type);
    }
    //------------------------------------------------------------------------
    void PlatInfo::log(Log * log)
    {
        log->log(_T("*************************"));
        log->log(_I18n("��������ʶ/����"));
        log->log(_T("*************************"));
        log->log(_I18n(" * ��������ʶ: ") + getIdentifier());
#if N_CPU == NII_CPU_X86
        log->log(_I18n(" * ֧��SSE: ") + N_conv(isSupport(CPUInfo::CS_SSE)));
        log->log(_I18n(" * ֧��SSE2: ") + N_conv(isSupport(CPUInfo::CS_SSE2)));
        log->log(_I18n(" * ֧��SSE3: ") + N_conv(isSupport(CPUInfo::CS_SSE3)));
        log->log(_I18n(" * ֧��SSSE3: ") + N_conv(isSupport(CPUInfo::CS_SSSE3)));
        log->log(_I18n(" * ֧��SSE4: ") + N_conv(isSupport(CPUInfo::CS_SSE4)));
        log->log(_I18n(" * ֧��SSE42: ") + N_conv(isSupport(CPUInfo::CS_SSE42)));
        log->log(_I18n(" * ֧��MMX: ") + N_conv(isSupport(CPUInfo::CS_MMX)));
        log->log(_I18n(" * ֧��MMXEXT: ") + N_conv(isSupport(CPUInfo::CS_MMX2)));
        log->log(_I18n(" * ֧��3DNOW: ") + N_conv(isSupport(CPUInfo::CS_3DNOW)));
        log->log(_I18n(" * ֧��3DNOWEXT: ") + N_conv(isSupport(CPUInfo::CS_3DNOWEXT)));
        log->log(_I18n(" * ֧��CMOV: ") + N_conv(isSupport(CPUInfo::CS_CMOV)));
        log->log(_I18n(" * ֧��TSC: ") + N_conv(isSupport(CPUInfo::CS_TSC)));
        log->log(_I18n(" * ֧��FPU: ") + N_conv(isSupport(CPUInfo::CS_FPU)));
        log->log(_I18n(" * ֧��PRO: ") + N_conv(isSupport(CPUInfo::CS_PRO)));
        log->log(_I18n(" * ֧��HT: ") + N_conv(isSupport(CPUInfo::CS_HTT)));
#elif N_CPU == NII_CPU_ARM
        log->log(_I18n(" * ֧��VFP: ") + N_conv(isSupport(CPUInfo::CS_VFP)));
        log->log(_I18n(" * ֧��VFP3: ") + N_conv(isSupport(CPUInfo::CS_VFP3)));
        log->log(_I18n(" * ֧��NEON: ") + N_conv(isSupport(CPUInfo::CS_NEON)));
#elif N_CPU == NII_CPU_PPC
        log->log(_I18n(" * ֧��ALTIVEC: ") + N_conv(isSupport(CPUInfo::CS_ALTIVEC)));
#endif
        log->log(_T("-------------------------"));

    }
    //------------------------------------------------------------------------
}