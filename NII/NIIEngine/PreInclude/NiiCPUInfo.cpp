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
#include "NiiCPUInfo.h"

#include <stdio.h>
#include <string>
#include <string.h>

#if (N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_OSX)
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #ifdef __ppc__
        #include <mach-o/arch.h>
    #endif
#endif

#if N_PLAT == N_PLAT_FREEBSD
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <sys/resource.h>
#endif

#if (N_PLAT == N_PLAT_LINUX) && defined(__ARM_NEON__) && (N_PLAT != N_PLAT_ANDROID)
    #include <fcntl.h>
    #include <unistd.h>
    #include <elf.h>
    #include <linux/auxvec.h>
    #include <asm/hwcap.h>
#endif

#if N_PLAT == N_PLAT_ANDROID
    #include <cpu-features.h>
#endif

#if N_COMPILER == N_CPP_MSVC
    #include <excpt.h>      // For SEH values
    #include <windows.h>
    #if _MSC_VER >= 1400
        #include <intrin.h>
    #endif
#elif (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_GCCE)
    #include <signal.h>
    #include <setjmp.h>

    #if N_CPU == NII_CPU_ARM && N_PLAT != N_PLAT_ANDROID
        #include <sys/sysctl.h>
        #if _MACH_
            #include <mach/machine.h>
        #endif
    #endif
#endif

/*
user 从系统启动开始累计到当前时刻,用户态的CPU时间(单位:jiffies),不包含nice值为负进程.1jiffies=0.01秒
nice 从系统启动开始累计到当前时刻,nice值为负的进程所占用的CPU时间(单位:jiffies)
system 从系统启动开始累计到当前时刻,核心时间(单位:jiffies)
idle 从系统启动开始累计到当前时刻,除硬盘IO等待时间以外其它等待时间(单位:jiffies)
iowait 从系统启动开始累计到当前时刻,硬盘IO等待时间(单位:jiffies)
irq 从系统启动开始累计到当前时刻,硬中断时间(单位:jiffies)
softirq 从系统启动开始累计到当前时刻,软中断时间(单位:jiffies)
CPU时间=user+system+nice+idle+iowait+irq+softirq
*/
namespace NII
{
#if N_CPU == NII_CPU_X86
    //---------------------------------------------------------------------
    // Struct for store CPUID instruction result, compiler-independent
    //---------------------------------------------------------------------
    struct CpuidResult
    {
        Nui32 _eax;
        Nui32 _ebx;
        Nui32 _edx;
        Nui32 _ecx;
    };
    //---------------------------------------------------------------------
    // Compiler-dependent routines
    //---------------------------------------------------------------------
    #if N_COMPILER == N_CPP_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4035)  // no return value
    #endif
    //---------------------------------------------------------------------
    // Detect whether CPU supports CPUID instruction, returns non-zero if supported.
    static int _isSupportCpuid(void)
    {
    #if N_COMPILER == N_CPP_MSVC
        // Visual Studio 2005 & 64-bit compilers always supports __cpuid intrinsic
        // note that even though this is a build rather than runtime setting, all
        // 64-bit CPUs support this so since binary is 64-bit only we're ok
        #if _MSC_VER >= 1400 && defined(_M_X64)
        return true;
        #else
        // If we can modify flag register bit 21, the cpu is supports CPUID instruction
        __asm
        {
            // Read EFLAG
            pushfd
            pop     eax
            mov     ecx, eax

            // Modify bit 21
            xor     eax, 0x200000
            push    eax
            popfd

            // Read back EFLAG
            pushfd
            pop     eax

            // Restore EFLAG
            push    ecx
            popfd

            // Check bit 21 modifiable
            xor     eax, ecx
            neg     eax
            sbb     eax, eax

            // Return values in eax, no return statement requirement here for VC.
        }
        #endif
    #elif (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_GCCE)
        #if N_ARCH == N_ARCH_64
        return true;
        #else

        unsigned oldFlags, newFlags;
        __asm__
        (
            "pushfl         \n\t"
            "pop    %0      \n\t"
            "mov    %0, %1  \n\t"
            "xor    %2, %0  \n\t"
            "push   %0      \n\t"
            "popfl          \n\t"
            "pushfl         \n\t"
            "pop    %0      \n\t"
            "push   %1      \n\t"
            "popfl          \n\t"
            : "=r" (oldFlags), "=r" (newFlags)
            : "n" (0x200000)
        );
        return oldFlags != newFlags;
        #endif // 64
    #else
        // TODO: Supports other compiler
        return false;
    #endif
    }

    //---------------------------------------------------------------------
    // Performs CPUID instruction with 'query', fill the results, and return value of eax.
    static Nui32 _performCpuid(int query, CpuidResult & result)
    {
    #if N_COMPILER == N_CPP_MSVC
        #if _MSC_VER >= 1400
        int CPUInfo[4];
        __cpuid(CPUInfo, query);
        result._eax = CPUInfo[0];
        result._ebx = CPUInfo[1];
        result._ecx = CPUInfo[2];
        result._edx = CPUInfo[3];
        return result._eax;
        #else
        __asm
        {
            mov     edi, result
            mov     eax, query
            cpuid
            mov     [edi]._eax, eax
            mov     [edi]._ebx, ebx
            mov     [edi]._edx, edx
            mov     [edi]._ecx, ecx
            // Return values in eax, no return statement requirement here for VC.
        }
        #endif
    #elif (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_GCCE)
        #if N_ARCH == N_ARCH_64
        __asm__
        (
            "cpuid": "=a" (result._eax), "=b" (result._ebx), "=c" (result._ecx), "=d" (result._edx) : "a" (query)
        );
        #else
        __asm__
        (
            "pushl  %%ebx           \n\t"
            "cpuid                  \n\t"
            "movl   %%ebx, %%edi    \n\t"
            "popl   %%ebx           \n\t"
            : "=a" (result._eax), "=D" (result._ebx), "=c" (result._ecx), "=d" (result._edx)
            : "a" (query)
        );
       #endif // N_ARCH_64
        return result._eax;

    #else
        // TODO: Supports other compiler
        return 0;
    #endif
    }

    #if N_COMPILER == N_CPP_MSVC
    #pragma warning(pop)
    #endif
    //---------------------------------------------------------------------
    // Detect whether or not os support Streaming SIMD Extension.
    #if (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_GCCE)
    static jmp_buf sIllegalJmpBuf;
    static void _illegalHandler(int x)
    {
        (void)(x); // Unused
        longjmp(sIllegalJmpBuf, 1);
    }
    #endif
    static bool _checkOSHaveSSE(void)
    {
    #if N_COMPILER == N_CPP_MSVC
        /*
            The FP part of SSE introduces a new architectural state and therefore
            requires support from the operating system. So even if CPUID indicates
            support for SSE FP, the application might not be able to use it. If
            CPUID indicates support for SSE FP, check here whether it is also
            supported by the OS, and turn off the SSE FP feature bit if there
            is no OS support for SSE FP.

            Operating systems that do not support SSE FP return an illegal
            instruction exception if execution of an SSE FP instruction is performed.
            Here, a sample SSE FP instruction is executed, and is checked for an
            exception using the (non-standard) __try/__except mechanism
            of Microsoft Visual C/C++.
        */
        // Visual Studio 2005, Both AMD and Intel x64 support SSE
        // note that even though this is a build rather than runtime setting, all
        // 64-bit CPUs support this so since binary is 64-bit only we're ok
    #if _MSC_VER >= 1400 && defined(_M_X64)
            return true;
    #else
        __try
        {
            __asm orps  xmm0, xmm0
            return true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    #endif
    #elif (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_GCCE)
        #if N_ARCH == N_ARCH_64
        return true;
        #else
        // Does gcc have __try/__except similar mechanism?
        // Use signal, setjmp/longjmp instead.
        void (*oldHandler)(int);
        oldHandler = signal(SIGILL, _illegalHandler);

        if (setjmp(sIllegalJmpBuf))
        {
            signal(SIGILL, oldHandler);
            return false;
        }
        else
        {
            __asm__ __volatile__ ("orps %xmm0, %xmm0");
            signal(SIGILL, oldHandler);
            return true;
        }
       #endif
    #else
        // TODO: Supports other compiler, assumed is supported by default
        return true;
    #endif
    }

    //---------------------------------------------------------------------
    // Compiler-independent routines
    //---------------------------------------------------------------------

    static Nui32 queryCpuFeatures(void)
    {
    #define CPUID_STD_FPU               (1<<0)
    #define CPUID_STD_TSC               (1<<4)
    #define CPUID_STD_CMOV              (1<<15)
    #define CPUID_STD_MMX               (1<<23)
    #define CPUID_STD_SSE               (1<<25)
    #define CPUID_STD_SSE2              (1<<26)
    #define CPUID_STD_HTT               (1<<28)     // EDX[28] - Bit 28 set indicates  Hyper-Threading Technology is supported in hardware.

    #define CPUID_STD_SSE3              (1<<0)      // ECX[0] - Bit 0 of standard function 1 indicate SSE3 supported
    #define CPUID_STD_SSSE3             (1<<9)
    #define CPUID_STD_SSE4              (1<<19)
    #define CPUID_STD_SSE42             (1<<20)

    #define CPUID_FAMILY_ID_MASK        0x0F00      // EAX[11:8] - Bit 11 thru 8 contains family  processor id
    #define CPUID_EXT_FAMILY_ID_MASK    0x0F00000   // EAX[23:20] - Bit 23 thru 20 contains extended family processor id
    #define CPUID_PENTIUM4_ID           0x0F00      // Pentium 4 family processor id

    #define CPUID_EXT_3DNOW             (1<<31)
    #define CPUID_EXT_AMD_3DNOWEXT      (1<<30)
    #define CPUID_EXT_AMD_MMX2          (1<<22)

        Nui32 features = 0;

        // Supports CPUID instruction ?
        if (_isSupportCpuid())
        {
            CpuidResult result;

            // Has standard feature ?
            if (_performCpuid(0, result))
            {
                // Check vendor strings
                if (memcmp(&result._ebx, "GenuineIntel", 12) == 0)
                {
                    if (result._eax > 2)
                        features |= SimdInfo::CS_PRO;

                    // Check standard feature
                    _performCpuid(1, result);

                    if(result._edx & CPUID_STD_FPU)
                        features |= SimdInfo::CS_FPU;
                    if(result._edx & CPUID_STD_TSC)
                        features |= SimdInfo::CS_TSC;
                    if(result._edx & CPUID_STD_CMOV)
                        features |= SimdInfo::CS_CMOV;
                    if(result._edx & CPUID_STD_MMX)
                        features |= SimdInfo::CS_MMX;
                    if(result._edx & CPUID_STD_SSE)
                        features |= SimdInfo::CS_MMX2 | SimdInfo::CS_SSE;
                    if(result._edx & CPUID_STD_SSE2)
                        features |= SimdInfo::CS_SSE2;

                    if(result._ecx & CPUID_STD_SSE3)
                        features |= SimdInfo::CS_SSE3;
                    if(result._ecx & CPUID_STD_SSSE3)
                        features |= SimdInfo::CS_SSSE3;
                    if(result._ecx & CPUID_STD_SSE4)
                        features |= SimdInfo::CS_SSE4;
                    if(result._ecx & CPUID_STD_SSE42)
                        features |= SimdInfo::CS_SSE42;

                    // Check to see if this is a Pentium 4 or later processor
                    if ((result._eax & CPUID_EXT_FAMILY_ID_MASK) ||
                        (result._eax & CPUID_FAMILY_ID_MASK) == CPUID_PENTIUM4_ID)
                    {
                        // Check hyper-threading technology
                        if (result._edx & CPUID_STD_HTT)
                            features |= SimdInfo::CS_HTT;
                    }
                }
                else if (memcmp(&result._ebx, "AuthenticAMD", 12) == 0)
                {
                    features |= SimdInfo::CS_PRO;

                    // Check standard feature
                    _performCpuid(1, result);

                    if(result._edx & CPUID_STD_FPU)
                        features |= SimdInfo::CS_FPU;
                    if(result._edx & CPUID_STD_TSC)
                        features |= SimdInfo::CS_TSC;
                    if(result._edx & CPUID_STD_CMOV)
                        features |= SimdInfo::CS_CMOV;
                    if(result._edx & CPUID_STD_MMX)
                        features |= SimdInfo::CS_MMX;
                    if(result._edx & CPUID_STD_SSE)
                        features |= SimdInfo::CS_SSE;
                    if(result._edx & CPUID_STD_SSE2)
                        features |= SimdInfo::CS_SSE2;

                    if(result._ecx & CPUID_STD_SSE3)
                        features |= SimdInfo::CS_SSE3;
                    if(result._ecx & CPUID_STD_SSSE3)
                        features |= SimdInfo::CS_SSSE3;
                    if(result._ecx & CPUID_STD_SSE4)
                        features |= SimdInfo::CS_SSE4;
                    if(result._ecx & CPUID_STD_SSE42)
                        features |= SimdInfo::CS_SSE42;

                    // Has extended feature ?
                    if (_performCpuid(0x80000000, result) > 0x80000000)
                    {
                        // Check extended feature
                        _performCpuid(0x80000001, result);

                        if (result._edx & CPUID_EXT_3DNOW)
                            features |= SimdInfo::CS_3DNOW;
                        if (result._edx & CPUID_EXT_AMD_3DNOWEXT)
                            features |= SimdInfo::CS_3DNOWEXT;
                        if (result._edx & CPUID_EXT_AMD_MMX2)
                            features |= SimdInfo::CS_MMX2;
                    }
                }
            }
        }

        return features;
    }
    //---------------------------------------------------------------------
    static Nui32 _detectCpuFeatures(void)//x86
    {
        Nui32 features = queryCpuFeatures();

        const Nui32 sse_features = SimdInfo::CS_SSE |
            SimdInfo::CS_SSE2 | SimdInfo::CS_SSE3 |
                SimdInfo::CS_SSSE3 | SimdInfo::CS_SSE4 |
                    SimdInfo::CS_SSE42;
        if ((features & sse_features) && !_checkOSHaveSSE())
        {
            features &= ~sse_features;
        }

        return features;
    }
    //---------------------------------------------------------------------
    static String _detectCpuIdentifier(void)//x86
    {
        // Supports CPUID instruction ?
        if (_isSupportCpuid())
        {
            CpuidResult result;
            Nui32 nExIds;
            char CPUString[0x20];
            char CPUBrandString[0x40];

            StringStream detailedIdentStr;


            // Has standard feature ?
            if (_performCpuid(0, result))
            {
                memset(CPUString, 0, sizeof(CPUString));
                memset(CPUBrandString, 0, sizeof(CPUBrandString));

                *((int*)CPUString) = result._ebx;
                *((int*)(CPUString+4)) = result._edx;
                *((int*)(CPUString+8)) = result._ecx;

                detailedIdentStr << CPUString;

                // Calling _performCpuid with 0x80000000 as the query argument
                // gets the number of valid extended IDList.
                nExIds = _performCpuid(0x80000000, result);

                for (Nui32 i = 0x80000000; i <= nExIds; ++i)
                {
                    _performCpuid(i, result);

                    // Interpret CPU brand string and cache information.
                    if  (i == 0x80000002)
                    {
                        memcpy(CPUBrandString + 0, &result._eax, sizeof(result._eax));
                        memcpy(CPUBrandString + 4, &result._ebx, sizeof(result._ebx));
                        memcpy(CPUBrandString + 8, &result._ecx, sizeof(result._ecx));
                        memcpy(CPUBrandString + 12, &result._edx, sizeof(result._edx));
                    }
                    else if  (i == 0x80000003)
                    {
                        memcpy(CPUBrandString + 16 + 0, &result._eax, sizeof(result._eax));
                        memcpy(CPUBrandString + 16 + 4, &result._ebx, sizeof(result._ebx));
                        memcpy(CPUBrandString + 16 + 8, &result._ecx, sizeof(result._ecx));
                        memcpy(CPUBrandString + 16 + 12, &result._edx, sizeof(result._edx));
                    }
                    else if  (i == 0x80000004)
                    {
                        memcpy(CPUBrandString + 32 + 0, &result._eax, sizeof(result._eax));
                        memcpy(CPUBrandString + 32 + 4, &result._ebx, sizeof(result._ebx));
                        memcpy(CPUBrandString + 32 + 8, &result._ecx, sizeof(result._ecx));
                        memcpy(CPUBrandString + 32 + 12, &result._edx, sizeof(result._edx));
                    }
                }
                String brand;
                StrConv::conv(CPUBrandString, brand);
                StrUtil::trim(brand);
                if (!brand.empty())
                    detailedIdentStr << ": " << brand;

                return detailedIdentStr.str();
            }
        }

        return _T("X86");
    }

#elif N_CPU == NII_CPU_ARM

    //---------------------------------------------------------------------
    static Nui32 _detectCpuFeatures(void)//arm
    {
        // Use preprocessor definitions to determine architecture and CPU features
        Nui32 features = 0;
    #if defined(__ARM_ARCH_7A__) && defined(__ARM_NEON__)
        features |= SimdInfo::CS_NEON;
    #elif defined(__ARM_ARCH_6K__) && defined(__VFP_FP__)
        features |= SimdInfo::CS_VFP;
    #elif (N_PLAT == N_PLAT_ANDROID)
        if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM)
        {
            if(android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON)
                features |= SimdInfo::CS_NEON;
            if(android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_VFPv3)
                features |= CS_VFP3;
            if(android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_VFPv2)
                features |= CS_VFP;
        }
    #elif (N_PLAT == N_PLAT_LINUX) && defined(__ARM_NEON__)
        int fd = open("/proc/self/auxv", O_RDONLY);
        if (fd >= 0)
        {
            Elf32_auxv_t auxv;
            while (read(fd, &auxv, sizeof(Elf32_auxv_t)) == sizeof(Elf32_auxv_t))
            {
                if (auxv.a_type == AT_HWCAP)
                {
                    if(auxv.a_un.a_val & HWCAP_NEON)
                        features |= SimdInfo::CS_NEON;
                    if(auxv.a_un.a_val & HWCAP_VFP)
                        features |= SimdInfo::CS_VFP;
                    if(auxv.a_un.a_val & HWCAP_VFPv3)
                        features |= SimdInfo::CS_VFP3;
                    break;
                }
            }
            close(fd);
        }
    #endif
        return features;
    }
    //---------------------------------------------------------------------
    static String _detectCpuIdentifier(void)//arm
    {
        String cpuID = "";
    #if N_PLAT == N_PLAT_IOS
        // Get the ARM CPU subtype
        int cpusubtype = 0;
        NCount len = sizeof(cpusubtype);
        sysctlbyname("hw.cpusubtype", &cpusubtype, &len, NULL, 0);

        switch(cpusubtype)
        {
            case CPU_SUBTYPE_ARM_V4T:
                cpuID = "ARMv4T";
                break;
            case CPU_SUBTYPE_ARM_V6:
                cpuID = "ARMv6";
                break;
            case CPU_SUBTYPE_ARM_V5TEJ:
                cpuID = "ARMv5TEJ";
                break;
            case CPU_SUBTYPE_ARM_XSCALE:
                cpuID = "ARM XScale";
                break;
            case CPU_SUBTYPE_ARM_V7:
                cpuID = "ARMv7";
                break;
            case CPU_SUBTYPE_ARM_V7F:
                cpuID = "ARMV7F";
                break;
            case CPU_SUBTYPE_ARM_V7K:
                cpuID = "ARMV7K";
                break;
            default:
                cpuID = "Unknown ARM";
                break;
        }
    #elif N_PLAT == N_PLAT_LINUX
        char buffer[512];
        FILE * cpuinfo = NULL;
        cpuinfo = fopen("/proc/cpuinfo", "r");
        if(cpuinfo)
        {
            while (fgets(buffer, sizeof(buffer), cpuinfo))
            {
                if (strncmp(buffer, "Processor", strlen("Processor"))==0)
                {
                    char * needle = strstr(buffer, ":");
                    if (needle && strlen(needle)>3)
                    {
                        needle += 2;
                        cpuID += needle;
                        StrUtil::trim(cpuID);
                        cpuID += " ";
                    }
                }
            }
        }
        if(cpuinfo != NULL)
            fclose(cpuinfo);
    #else
        cpuID = "Unknown ARM";
    #endif
        return cpuID;
    }
#elif N_CPU == NII_CPU_PPC
    //---------------------------------------------------------------------
    static Nui32 _detectCpuFeatures(void)//PPC
    {
        Nui32 features = 0;
    #if (N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_OSX) && defined(__ppc__)
        features |= CS_ALTIVEC;
    #elif defined(__powerpc__) || defined(__ppc__)
        features |= CS_ALTIVEC;
    #endif
        return features;
    }
    //---------------------------------------------------------------------
    static String _detectCpuIdentifier(void)//PPC
    {
        return "PPC";
    }
#else // N_CPU == NII_CPU_UNKNOWN
    //---------------------------------------------------------------------
    static Nui32 _detectCpuFeatures(void)//unknow
    {
        return 0;
    }
    //---------------------------------------------------------------------
    static String _detectCpuIdentifier(void)//unknow
    {
        return "Unknown";
    }
#endif  // N_CPU
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CPUDev
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CPUDev : public CPUAlloc
    {
    public:
        CPUDev() :
            mID(0),
            mPercent(.0f),
            mUser(0),
            mNice(0),
            mSystem(0),
            mIdle(0),
            mIO(0){}
    public:
        Nid mID;
        NIId mPercent;
        Nui64 mUser;
        Nui64 mNice;
        Nui64 mSystem;
        Nui64 mIdle;
        Nui64 mIO;
        String mVendor;
        String mModel;
        String mBogoMips;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CPUInfo
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CPUInfo::CPUInfo()
    {
        String cpuModel = _T("N/A");
        String cpuVendor = _T("N/A");
        String cpuBogoMips = _T("N/A");
#if (N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_OSX)
        NCount len = 4;
        // The number of cores.
        if (sysctlbyname("hw.activecpu", &mCount, &len, NULL, 0) == -1)
            mCount = 1;
        // The model.
    #ifdef __ppc__
        const NXArchInfo * info = NXGetLocalArchInfo();
        if (info != NULL)
            cpuModel = info->description;
    #else
        // NXGetLocalArchInfo is ugly for intel so keep using this method
        Ni8 buffer[512];
        len = 512;
        if (sysctlbyname("machdep.cpu.brand_string", &buffer, &len, NULL, 0) == 0)
            cpuModel = buffer;

        // The CPU vendor
        len = 512;
        if (sysctlbyname("machdep.cpu.vendor", &buffer, &len, NULL, 0) == 0)
            cpuVendor = buffer;
    #endif
        // Go through each core.
        for (NIIi i = 0; i < mCount; ++i)
        {
            CPUDev * core = N_new CPUDev();
            core->mID = i;
            core->mVendor = cpuVendor;
            core->mModel = cpuModel;
            core->mBogoMips = cpuBogoMips;
            mCPUDevs.push_back(core);
        }
#elif N_PLAT == N_PLAT_WIN32
        Ntchar rgValue[128];
        HKEY hKey;
        DWORD dwSize = 128 * sizeof(Ntchar);
        LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey);
        ret = RegQueryValueEx(hKey, _T("ProcessorNameString"), NULL, NULL, (LPBYTE)rgValue, &dwSize);

        if(ret == 0)
        {
            cpuModel = rgValue;
            StrUtil::trim(cpuModel);
        }
        RegCloseKey(hKey);

        SYSTEM_INFO siSysInfo;
        GetSystemInfo(&siSysInfo);
        mCount = siSysInfo.dwNumberOfProcessors;

        CPUDev * core = N_new CPUDev();
        core->mID = 0;
        core->mVendor = cpuVendor;
        core->mModel = cpuModel;
        core->mBogoMips = cpuBogoMips;
        mCPUDevs.push_back(core);
#elif N_PLAT == N_PLAT_FREEBSD
        NCount len;
        Ni8 cpumodel[512];

        len = 4;
        if (sysctlbyname("hw.ncpu", &mCount, &len, NULL, 0) != 0)
            mCount = 1;

        len = sizeof(cpumodel);
        if (sysctlbyname("hw.model", &cpumodel, &len, NULL, 0) != 0)
            strncpy(cpumodel, "Unknown", 8);
        cpuModel = cpumodel;

        for (NCount i = 0; i < mCount; ++i)
        {
            CPUDev * core = N_new CPUDev();
            core->mID = i;
            core->mVendor = cpuVendor;
            core->mModel = cpuModel;
            core->mBogoMips = cpuBogoMips;
            mCPUDevs.push_back(core);
        }
#else
        FILE * cpuInfo = fopen("/proc/cpuinfo", "r");
        mCount = 0;
        CPUDev * core;
        if (cpuInfo != NULL)
        {
            Ni8 buffer[512];
            Nui32 lid;
            bool valid;
            while (fgets(buffer, sizeof(buffer), cpuInfo))
            {
                if (strncmp(buffer, "processor", strlen("processor"))==0)
                {
                    if(valid)
                    {
                        core = N_new CPUDev();
                        core->mID = mCount;
                        core->mVendor = cpuVendor;
                        core->mModel = cpuModel;
                        core->mBogoMips = cpuBogoMips;

                        mCPUDevs.push_back(core);
                        ++mCount;
                    }
                }
                else if(strncmp(buffer, "vendor_id", strlen("vendor_id"))==0)
                {
                    Ni8 * needle = strstr(buffer, ":");
                    if (needle && strlen(needle) > 3)
                    {
                        needle += 2;
                        core->mVendor = needle;
                        StrUtil::trim(core->mVendor);
                    }
                }
                else if(strncmp(buffer, "model name", strlen("model name"))==0)
                {
                    Ni8 * needle = strstr(buffer, ":");
                    if (needle && strlen(needle) > 3)
                    {
                        needle += 2;
                        core->mModel = needle;
                        StrUtil::trim(core->mModel);
                    }
                }
                else if(strncmp(buffer, "physical id", strlen("physical id"))==0)
                {
                    Ni8 * needle = strstr(buffer, ":");
                    Nui32 id;
                    if(needle && strlen(needle) > 3)
                    {
                        needle += 2;
                        StrConv::conv(StrUtil::trim(needle), id, lid);
                        if(id != lid)
                            valid = true;
                    }
                }
                else if(strncmp(buffer, "bogomips", strlen("bogomips"))==0)
                {
                    Ni8 * needle = strstr(buffer, ":");
                    if (needle && strlen(needle) > 3)
                    {
                        needle += 2;
                        core->mBogoMips = needle;
                        StrUtil::trim(core->mBogoMips);
                    }
                }
            }
            fclose(cpuInfo);
        }
        else
        {
            mCount = 1;
            core = N_new CPUDev();
            core->mID = 0;
            core->mVendor = cpuVendor;
            core->mModel = cpuModel;
            core->mBogoMips = cpuBogoMips;
            mCPUDevs.push_back(core);
        }
    #endif
        readStat(mUserTime, mNiceTime, mSysTime, mIdleTime, mIOTime);
        
        mCpuMark = _detectCpuFeatures();
        mCpuName = _detectCpuIdentifier();
    }
    //------------------------------------------------------------------------
    CPUInfo::~CPUInfo()
    {
        CPUDevs::iterator i, iend = mCPUDevs.end();
        for(i = mCPUDevs.begin(); i != iend; ++i)
            N_delete *i;

        mCPUDevs.clear();
    }
    //------------------------------------------------------------------------
    NIIi CPUInfo::getUsedPercent()
    {
        Nui64 user,nice,system,idle,io;

        NIIi percent = 0;
        if (!readStat(user, nice, system, idle, io))
            return percent;

        user -= mUserTime;
        nice -= mNiceTime;
        system -= mSysTime;
        idle -= mIdleTime;
        io -= mIOTime;

#if N_PLAT == N_PLAT_WIN32
        if(user + system == 0)
            return percent;
        NIIi result = (NIIi)((user + system - idle) * 100 / (user + system));
#else
        if(user + nice + system + idle + io == 0)
            return percent;
        NIIi result = (NIIi)((user + nice + system) * 100 / (user + nice + system + idle + io));
#endif
        mUserTime += user;
        mNiceTime += nice;
        mSysTime += system;
        mIdleTime += idle;
        mIOTime += io;

        return result;
    }
    //------------------------------------------------------------------------
    NIIf CPUInfo::getFrequency(Nidx index)
    {
        // Get CPU frequency, scaled to MHz.
#if (N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_OSX)
        long long hz = 0;
        NCount len = sizeof(hz);
        if (sysctlbyname("hw.cpufrequency", &hz, &len, NULL, 0) == -1)
            return 0.f;
        return hz / 1000000.0;
#elif N_PLAT == N_PLAT_WIN32
        HKEY hKey;
        DWORD dwMHz=0;
        DWORD dwSize = sizeof(dwMHz);
        LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,(LPCWSTR)"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",0, KEY_READ, &hKey);
        ret = RegQueryValueEx(hKey, (LPCWSTR)"~MHz", NULL, NULL, (LPBYTE)&dwMHz, &dwSize);
        RegCloseKey(hKey);
        if(ret == 0)
            return NIIf(dwMHz);
        else
            return 0.f;
#elif N_PLAT == N_PLAT_FREEBSD
        NIIi hz = 0;
        NCount len = sizeof(hz);
        if (sysctlbyname("dev.cpu.0.freq", &hz, &len, NULL, 0) != 0)
        hz = 0;
        return (NIIf)hz;
#else
        NIIf mhz = 0.f;
        Ni8 buf[256];

        FILE * cpuInfo = fopen("/proc/cpuinfo", "r");

        if (!cpuInfo)
            return mhz;

        while (fgets(buf, 256, cpuInfo) != NULL)
        {
            if (strncmp(buf, "cpu MHz", 7) == 0)
            {
                Ni8 * needle = strchr(buf, ':');
                Nsscanf(++needle, "%f", &mhz);
                break;
            }
        }
        fclose(cpuInfo);

        return mhz;
#endif
    }
    //------------------------------------------------------------------------
    NIId CPUInfo::getTemperature(TemperatureType type)
    {
        NIIi value = 0, ret = 0;
        Ni8 scale = 0;
        NIId tF = 0;
/*         FILE * p = NULL;
        CStdString cmd = g_advancedSettings.m_cpuTempCmd;

        if (cmd.IsEmpty() && mTemperature == NULL)
            return false;

        if (!cmd.IsEmpty())
        {
            p = popen (cmd.c_str(), "r");
            if (p)
            {
                ret = fscanf(p, "%d %c", &value, &scale);
                pclose(p);
            }
        }
        else
        {
            // procfs is deprecated in the linux kernel, we should move away from
            // using it for temperature data.  It doesn't seem that sysfs has a
            // general enough interface to bother implementing ATM.

            rewind(mTemperature);
            fflush(mTemperature);
            ret = fscanf(mTemperature, "temperature: %d %c", &value, &scale);

            // read from the temperature file of the new kernels
            if (!ret)
            {
                ret = fscanf(mTemperature, "%d", &value);
                value = value / 1000;
                scale = 'c';
                ret++;
            }
        } */

#if N_PLAT == N_PLAT_WIN32
        return tF;
#elif N_PLAT = N_PLAT_LINUX || N_PLAT == N_PLAT_LINUX_X
        FILE * F = fopen("/proc/acpi/thermal_zone/THM0/temperature", "r");
        if (F == NULL)
            F = fopen("/proc/acpi/thermal_zone/THRM/temperature", "r");
        if (F == NULL)
            F = fopen("/proc/acpi/thermal_zone/THR0/temperature", "r");
        if (F == NULL)
            F = fopen("/proc/acpi/thermal_zone/TZ0/temperature", "r");
        if (F == NULL)
            F = fopen("/sys/class/hwmon/hwmon0/temp1_input", "r");

        if(F == NULL)
            return tF;

        ret = fscanf(F, "temperature: %d %c", &value, &scale);

        // read from the temperature file of the new kernels
        if (!ret)
        {
            ret = fscanf(F, "%d", &value);
            value = value / 1000;
            scale = 'c';
            ret++;
        }

        fclose(F);

        if (ret != 2)
            return tF;
        // TT_F = TT_C * 1.8f + 32.0f
        switch(type)
        {
        case : TT_C
            if (scale == 'C' || scale == 'c')
                tF = value;
            else if (scale == 'F' || scale == 'f')
                tF = (value - 32.0f) / 1.8f;
            break;
        case : TT_F
            if (scale == 'C' || scale == 'c')
                tF = value * 1.8f + 32.0f;
            else if (scale == 'F' || scale == 'f')
                tF = value;
            break;
        default:
        }
#endif
        return tF;
    }
    //------------------------------------------------------------------------
    bool CPUInfo::readStat(Nui64 & user, Nui64& nice, Nui64 & sys, Nui64 & idle, Nui64 & io)
    {
#if N_PLAT == N_PLAT_WIN32
        FILETIME idleTime;
        FILETIME kernelTime;
        FILETIME userTime;
        ULARGE_INTEGER ulTime;
        Nui64 coreUser, coreSystem, coreIdle;
        GetSystemTimes(&idleTime, &kernelTime, &userTime);
        ulTime.HighPart = userTime.dwHighDateTime;
        ulTime.LowPart = userTime.dwLowDateTime;
        user = coreUser = ulTime.QuadPart;

        ulTime.HighPart = kernelTime.dwHighDateTime;
        ulTime.LowPart = kernelTime.dwLowDateTime;
        sys = coreSystem = ulTime.QuadPart;

        ulTime.HighPart = idleTime.dwHighDateTime;
        ulTime.LowPart = idleTime.dwLowDateTime;
        idle = coreIdle = ulTime.QuadPart;

        nice = 0;

        coreUser -= mCPUDevs[0]->mUser;
        coreSystem -= mCPUDevs[0]->mSystem;
        coreIdle -= mCPUDevs[0]->mIdle;
        mCPUDevs[0]->mPercent = ((NIId)(coreUser + coreSystem - coreIdle) * 100.0) / (NIId)(coreUser + coreSystem);
        mCPUDevs[0]->mUser += coreUser;
        mCPUDevs[0]->mSystem += coreSystem;
        mCPUDevs[0]->mIdle += coreIdle;

        io = 0;
#elif N_PLAT == N_PLAT_FREEBSD
        long * cptimes;
        NCount len;
        NIIi i;

        len = sizeof(long) * 32 * CPUSTATES;
        if (sysctlbyname("kern.cp_times", NULL, &len, NULL, 0) != 0)
            return false;
        cptimes = (long*)malloc(len);
        if (cptimes == NULL)
            return false;
        if (sysctlbyname("kern.cp_times", cptimes, &len, NULL, 0) != 0)
        {
            free(cptimes);
            return false;
        }
        user = 0;
        nice = 0;
        sys = 0;
        idle = 0;
        io = 0;
        for (i = 0; i < mCount; ++i)
        {
            NIIl coreUser, coreNice, coreSystem, coreIdle, coreIO;
            NIId total;

            coreUser = cptimes[i * CPUSTATES + CP_USER];
            coreNice = cptimes[i * CPUSTATES + CP_NICE];
            coreSystem = cptimes[i * CPUSTATES + CP_SYS];
            coreIO = cptimes[i * CPUSTATES + CP_INTR];
            coreIdle = cptimes[i * CPUSTATES + CP_IDLE];

            coreUser -= mCPUDevs[i].mUser;
            coreNice -= mCPUDevs[i].mNice;
            coreSystem -= mCPUDevs[i].mSystem;
            coreIdle -= mCPUDevs[i].mIdle;
            coreIO -= mCPUDevs[i].mIO;

            total = (NIId)(coreUser + coreNice + coreSystem + coreIdle + coreIO);
            if(total != 0.0f)
                mCPUDevs[i].mPercent = ((NIId)(coreUser + coreNice + coreSystem) * 100.0) / total;

            mCPUDevs[i].mUser += coreUser;
            mCPUDevs[i].mNice += coreNice;
            mCPUDevs[i].mSystem += coreSystem;
            mCPUDevs[i].mIdle += coreIdle;
            mCPUDevs[i].mIO += coreIO;

            user += coreUser;
            nice += coreNice;
            sys += coreSystem;
            idle += coreIdle;
            io += coreIO;
        }
        free(cptimes);
#else
        FILE * cpuStat = fopen("/proc/stat", "r");
        if (cpuStat == NULL)
            return false;

        Ni8 buf[256];
        if (!fgets(buf, sizeof(buf), cpuStat))
            return false;

        NIIi num = Nsscanf(buf, "cpu %llu %llu %llu %llu %llu %*s\n", &user, &nice, &sys, &idle, &io);
        if (num < 5)
            io = 0;

        while (fgets(buf, sizeof(buf), cpuStat) && num >= 4)
        {
            Nui64 coreUser, coreNice, coreSystem, coreIdle, coreIO;
            NIIi nCpu = 0;
            num = Nsscanf(buf, "cpu%d %llu %llu %llu %llu %llu %*s\n", &nCpu, &coreUser, &coreNice, &coreSystem, &coreIdle, &coreIO);
            if (num < 6)
                coreIO = 0;

            if (num > 4)
            {
                coreUser -= mCPUDevs[nCpu].mUser;
                coreNice -= mCPUDevs[nCpu].mNice;
                coreSystem -= mCPUDevs[nCpu].mSystem;
                coreIdle -= mCPUDevs[nCpu].mIdle;
                coreIO -= mCPUDevs[nCpu].mIO;

                NIId total = (NIId)(coreUser + coreNice + coreSystem + coreIdle + coreIO);
                if(total == 0.0f)
                    mCPUDevs[nCpu].mPercent = 0.0f;
                else
                    mCPUDevs[nCpu].mPercent = ((NIId)(coreUser + coreNice + coreSystem) * 100.0) / total;

                mCPUDevs[nCpu].mUser += coreUser;
                mCPUDevs[nCpu].mNice += coreNice;
                mCPUDevs[nCpu].mSystem += coreSystem;
                mCPUDevs[nCpu].mIdle += coreIdle;
                mCPUDevs[nCpu].mIO += coreIO;
            }
        }
        fclose(cpuStat);
    #endif
        return true;
    }
    //------------------------------------------------------------------------
    String CPUInfo::getCPUUsagePercent() const
    {
        String ret = _T("");
        CPUDevs::const_iterator i, iend = mCPUDevs.end();
        for(i = mCPUDevs.begin(); i != iend ; ++i)
        {
            const CPUDev * temp =  *i;
#if N_PLAT == N_PLAT_WIN32
            ret += _T("CPU core(s) :") + N_conv(mCount) + _T(" average : ") + N_conv((NIIf)temp->mPercent);
#else
            ret += "CPU :" + StrConv::conv(temp->mID) + " average : " +
                StrConv::conv((NIIf)temp->mPercent, 4);
#endif
        }
        return ret;
    }
    //------------------------------------------------------------------------
    const String & CPUInfo::getVendor(Nidx index = 0) 
    { 
        return index < mCount ? mCPUDevs[index]->mVendor : mCPUDevs[0]->mVendor;
    }
    //------------------------------------------------------------------------
    const String & CPUInfo::getModel(Nidx index = 0) 
    { 
        return index < mCount ? mCPUDevs[index]->mModel : mCPUDevs[0]->mModel; 
    }
    //------------------------------------------------------------------------
    const String & CPUInfo::getBogoMips(Nidx index = 0)
    { 
        return index < mCount ? mCPUDevs[index]->mBogoMips : mCPUDevs[0]->mBogoMips; 
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ApuInfo
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ApuInfo::ApuInfo()
    {

    }
    //------------------------------------------------------------------------
}