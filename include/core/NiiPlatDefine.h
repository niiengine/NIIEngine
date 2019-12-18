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

#ifndef _NII_PLAT_DEFINE_H_
#define _NII_PLAT_DEFINE_H_

namespace NII
{
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
        #define N_ALIGNED_DECL(type, var, alignment)  __declspec(align(alignment)) type var
    #elif (N_COMPILER == N_CPP_GNUC) || (N_COMPILER == N_CPP_GCCE)
        #define N_ALIGNED_DECL(type, var, alignment)  type var __attribute__((__aligned__(alignment)))
    #else
        #define N_ALIGNED_DECL(type, var, alignment)  type var
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