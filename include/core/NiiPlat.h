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

#ifndef _NII_PLATFORM_H_
#define _NII_PLATFORM_H_

#include "NiiConfig.h"

namespace NII
{
    #define N_PLAT_UNKNOW       0
    #define N_PLAT_WIN32        1
    #define N_PLAT_WINRT        2       //win8编程框架
    #define N_PLAT_WIN8_PHONE   3       //
    #define N_PLAT_LINUX        4       //linux内核
    #define N_PLAT_LINUX_X      5       //x窗体级别的linux系统
    #define N_PLAT_GTK          6
    #define N_PLAT_WXWIDGET     7
    #define N_PLAT_OSX          8       //苹果mac os x(darwin)
    #define N_PLAT_IOS          10      //苹果iphone(darwin)
    #define N_PLAT_ANDROID      11      // 安桌系统(linux内核级)
    #define N_PLAT_EMSCRIPTEN   12      // web编程框架
    #define N_PLAT_FREEBSD      13
    #define N_PLAT_PS4          14
    #define N_PLAT_PS5          15
    #define N_PLAT_XBOXONE      16

    #define N_HOST_X86          1
    #define N_HOST_X64          2
    #define N_HOST_ARM          3
    #define N_HOST_PPC          4

    #define N_ARCH_32           1
    #define N_ARCH_64           2
    #define N_ARCH_128          3
    #define N_ARCH_256          4
    #define N_ARCH_512          5
    #define N_ARCH_1024         6

    #define N_CPP_MSVC          1
    #define N_CPP_GNUC          2
    #define N_CPP_BORLAND       3
    #define N_CPP_WINSCW        4
    #define N_CPP_GCCE          5
    #define N_CPP_CLANG         6       //一般就苹果编程使用

    #define N_ENDIAN_L          1
    #define N_ENDIAN_B          2
    #define N_ENDIAN_N          3

    #define N_QUOTE_INPLACE(x) # x
    #define N_QUOTE(x) N_QUOTE_INPLACE(x)
    #define N_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

    #if defined(__GCCE__)
        #define N_COMPILER N_CPP_GCCE
        #define N_COMPILER_VER _MSC_VER
    #elif defined(__WINSCW__)
        #define N_COMPILER N_CPP_WINSCW
        #define N_COMPILER_VER _MSC_VER
    #elif defined(_MSC_VER)
        #define N_COMPILER N_CPP_MSVC
        #define N_COMPILER_VER _MSC_VER
    #elif defined(__clang__)
        #define N_COMPILER N_CPP_CLANG
        #define N_COMPILER_VER (((__clang_major__)*100) + (__clang_minor__*10) + __clang_patchlevel__)
    #elif defined(__GNUC__)
        #define N_COMPILER N_CPP_GNUC
        #define N_COMPILER_VER (((__GNUC__)*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__)
    #elif defined(__BORLANDC__)
        #define N_COMPILER N_CPP_BORLAND
        #define N_COMPILER_VER __BCPLUSPLUS__
        #define __FUNCTION__ __FUNC__
    #else
        #pragma error "unknown compiler"
    #endif

    #define N_COMPILER_CHECK(comp, ver) (N_COMPILER == (comp) && N_COMPILER_VER >= (ver))

    #if defined(_MSC_VER) && _MSC_VER > 0
        #define N_64BIT_MOD "I64"
    #else
        #define N_64BIT_MOD "ll"
    #endif

    #if defined(__WIN32__) || defined(_WIN32)
        #define N_PLAT N_PLAT_WIN32
        #ifndef _CRT_SECURE_NO_WARNINGS
            #define _CRT_SECURE_NO_WARNINGS
        #endif
        #ifndef _SCL_SECURE_NO_WARNINGS
            #define _SCL_SECURE_NO_WARNINGS
        #endif
    #elif defined(__APPLE_CC__)
        #include "Availability.h"
        #if __IPHONE_OS_VERSION_MIN_REQUIRED
            #define N_PLAT N_PLAT_IOS
        #else
            #define N_PLAT N_PLAT_OSX
        #endif
    #elif defined(__ANDROID__)
        #define N_PLAT N_PLAT_ANDROID
    #elif defined(__EMSCRIPTEN__)
    #   define N_PLAT N_PLAT_EMSCRIPTEN
    #else
        #define N_PLAT N_PLAT_LINUX
    #endif

    #if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || \
        defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
        #define N_ARCH N_ARCH_64
        #define IOType Nui64
    #else
        #define N_ARCH N_ARCH_32
        #define IOType Nui32
    #endif

    #if N_PLAT == N_PLAT_WIN32
        #define WIN32_LEAN_AND_MEAN
        #if !defined(NOMINMAX) && defined(_MSC_VER)
        #define NOMINMAX
        #endif
        #if defined(NIIEngine_Static)
            #define _EngineAPI
        #else
            #if defined(NIIEngine_DLL)
                #define _EngineAPI __declspec(dllexport)
            #else
                #define _EngineAPI __declspec(dllimport)
            #endif
        #endif
        #if defined(NIIEnginePlugin_DLL)
            #define _PluginAPI __declspec(dllexport)
        #else
            #define _PluginAPI __declspec(dllimport)
        #endif

        #define _EngineInner
        #if defined(_DEBUG) || defined(DEBUG)
            #define N_DEBUG 1
        #else
            #define N_DEBUG 0
        #endif

        #if defined(__MINGW32__)
            #include <unistd.h>
        #endif
    #define N_UNICODE_SUPPORT 1
    #elif N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_ANDROID
        #if defined(NII_GCC_VISIBILITY)
            #define _EngineAPI  __attribute__ ((visibility("default")))
            #define _EngineInner __attribute__ ((visibility("hidden")))
        #else
            #define _EngineAPI
            #define _EngineInner
        #endif

        #define stricmp strcasecmp

        #ifdef DEBUG
            #define N_DEBUG 1
        #else
            #define N_DEBUG 0
        #endif

        #if N_PLAT == N_PLAT_OSX
            #define NII_PLATFORM_LIB "NiiPlatform.bundle"
        #elif N_PLAT == N_PLAT_IOS
            #define NII_PLATFORM_LIB "NiiPlatform.a"
        #else
            #define NII_PLATFORM_LIB "libNiiPlatform.so"
        #endif

        #define N_UNICODE_SUPPORT 1
    #elif N_PLAT == N_PLAT_ANDROID
        #define _EngineInner
        #define _EngineAPI
        #define N_UNICODE_SUPPORT 1
        #define CLOCKS_PER_SEC  1000
        #pragma warn_unusedarg off
        #pragma warn_emptydecl off
        #pragma warn_possunwant off
        #define stricmp strcasecmp
        #ifndef CLOCKS_PER_SEC
            #define CLOCKS_PER_SEC  1000
        #endif
        #ifdef DEBUG
            #define N_DEBUG 1
        #else
            #define N_DEBUG 0
        #endif
    #endif

    #if _UNICODE
        #define __STR2WSTR(str) L##str
        #define _STR2WSTR(str) __STR2WSTR(str)
        #define _N_FUNCTION__ _STR2WSTR(__FUNCTION__)
        #define _N_FILE__ _STR2WSTR(__FILE__)
        typedef wchar_t     Ntchar;
    #else
        #define _N_FUNCTION__ __FUNCTION__
        #define _N_FILE__ __FILE__
        typedef char        Ntchar;
    #endif
    
    #define _N_LINE_ __LINE__

    #ifndef N_FLF
    #define N_FLF _N_FILE__, __LINE__, _N_FUNCTION__
    #endif

    #if N_DEBUG
        #if N_DEBUG_MEMTRACK
            #define N_MEMTRACK 1
            #define N_MEMTRACK_Decl                     , const Ntchar * file, int line, const Ntchar * func
            #define N_MEMTRACK_INVO                     , _N_FILE__, __LINE__, _N_FUNCTION__
            #define N_MEMTRACK_NewINVO                  (_N_FILE__, __LINE__, _N_FUNCTION__)
            #define N_MEMTRACK_Param                    , file, line, func
            #define N_MemTrack_alloc(p, size)           MemoryTracker::getOnly().allocT(p, s, 0 N_MEMTRACK_Param);
            #define N_MemTrack_realloc(p, np, nsize)    MemoryTracker::getOnly().reallocT(p, np, ns, 0 N_MEMTRACK_Param);
            #define N_MemTrack_free(p)                  MemoryTracker::getOnly().freeT(p);
        #else
            #define N_MEMTRACK 0
            #define N_MEMTRACK_Decl 
            #define N_MEMTRACK_INVO
            #define N_MEMTRACK_NewINVO
            #define N_MEMTRACK_Param
            #define N_MemTrack_alloc(p, size)           p
            #define N_MemTrack_realloc(p, np, nsize)    np
            #define N_MemTrack_free(p)
        #endif
    #else
        #if N_RELEASE_MEMTRACK
            #define N_MEMTRACK 1
            #define N_MEMTRACK_Decl                     , const Ntchar * file, int line, const Ntchar * func
            #define N_MEMTRACK_INVO                     , _N_FILE__, __LINE__, _N_FUNCTION__
            #define N_MEMTRACK_NewINVO                  (_N_FILE__, __LINE__, _N_FUNCTION__)
            #define N_MEMTRACK_Param                    , file, line, func
            #define N_MemTrack_alloc(p, size)           MemoryTracker::getOnly().allocT(p, s, 0 N_MEMTRACK_Param);
            #define N_MemTrack_realloc(p, np, nsize)    MemoryTracker::getOnly().reallocT(p, np, ns, 0 N_MEMTRACK_Param);
            #define N_MemTrack_free(p)                  MemoryTracker::getOnly().freeT(p);
        #else
            #define N_MEMTRACK 0
            #define N_MEMTRACK_Decl 
            #define N_MEMTRACK_INVO
            #define N_MEMTRACK_NewINVO
            #define N_MEMTRACK_Param
            #define N_MemTrack_alloc(p, size)           p
            #define N_MemTrack_realloc(p, np, nsize)    np
            #define N_MemTrack_free(p)
        #endif
    #endif
    #ifdef _DEBUG
        #define N_printf(msg) printf(msg)
        #define N_assert(a, b) assert((a)&&(b))
        #define N_assert1(a) assert(a)
    #else
        #define N_printf(msg)
        #define N_assert(a, b)
        #define N_assert1(a)
    #endif
    #ifdef NII_BIG_ENDIAN
        #define N_ENDIAN N_ENDIAN_B
    #else
        #define N_ENDIAN N_ENDIAN_L
    #endif

    #define NII_CPU_UNKNOWN 0
    #define NII_CPU_X86     1
    #define NII_CPU_PPC     2
    #define NII_CPU_ARM     3

    #if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
        (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
        #define N_CPU NII_CPU_X86
    #elif N_PLAT == N_PLAT_OSX && defined(__BIG_ENDIAN__)
        #define N_CPU NII_CPU_PPC
    #elif N_PLAT == N_PLAT_OSX
        #define N_CPU NII_CPU_X86
    #elif N_PLAT == N_PLAT_IOS && (defined(__i386__) || defined(__x86_64__))
        #define N_CPU NII_CPU_X86
    #elif defined(__arm__)
        #define N_CPU NII_CPU_ARM
    #else
        #define N_CPU NII_CPU_UNKNOWN
    #endif

    #if N_COMPILER_CHECK(N_CPP_MSVC, 1200)
        #define N_FORCEINLINE __forceinline
    #elif N_COMPILER_CHECK(N_CPP_GNUC, 340)
        #define N_FORCEINLINE inline __attribute__((always_inline))
    #else
        #define N_FORCEINLINE __inline
    #endif

    #if N_COMPILER_CHECK(N_CPP_GNUC, 700)
        #define N_FALLTHROUGH __attribute__((fallthrough))
    #else
        #define N_FALLTHROUGH
    #endif

    #if N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_CLANG
        #define N_NODISCARD __attribute__((__warn_unused_result__))
    #else
        #define N_NODISCARD
    #endif

    #if N_COMPILER == N_CPP_MSVC
        #define N_NORETURN __declspec(noreturn)
    #elif N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_CLANG
        #define N_NORETURN __attribute__((noreturn))
    #else
        #define N_NORETURN
    #endif

    #if N_COMPILER == N_CPP_MSVC
        #define N_DECLMALLOC __declspec(restrict) __declspec(noalias)
    #else
        #define N_DECLMALLOC __attribute__ ((malloc))
    #endif

    #if N_PLAT == N_PLAT_ANDROID
        #define N_DEFAULT_LOCALE ""
    #else
        #define N_DEFAULT_LOCALE "C"
    #endif

    #if N_COMPILER == N_CPP_MSVC
        #define N_ALIGNED_DECL(type, var, alignment)    __declspec(align(alignment)) type var
        #define N_ALIGNED_STRUCT(alignment) 
    #elif (N_COMPILER == N_CPP_GNUC) || (N_COMPILER == N_CPP_GCCE)
        #define N_ALIGNED_DECL(type, var, alignment)    type var __attribute__((__aligned__(alignment)))
        #define N_ALIGNED_STRUCT(alignment)             __attribute__((__aligned__(alignment)))
    #else
        #define N_ALIGNED_DECL(type, var, alignment)    type var
        #define N_ALIGNED_STRUCT(alignment)
    #endif

    #define N_SIMD_ALIGNMENT 16

    #define N_SIMD_ALIGNED_DECL(type, var)   N_ALIGNED_DECL(type, var, N_SIMD_ALIGNMENT)

    #if N_CPU == NII_CPU_X86 && N_COMPILER == N_CPP_MSVC
        #define N_HAVE_SSE  1
    #elif N_CPU == NII_CPU_X86 && (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_CLANG) && N_PLAT != N_PLAT_IOS
        #define N_HAVE_SSE  1
    #endif

    #if N_CPU == NII_CPU_ARM && (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_CLANG) && defined(__ARM_NEON__)
        #define N_HAVE_NEON  1
    #endif

    #ifndef N_HAVE_SSE
        #define N_HAVE_SSE 0
    #endif

    #ifndef N_HAVE_NEON
        #define N_HAVE_NEON 0
    #endif
}
#endif