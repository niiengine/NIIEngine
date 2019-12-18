/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-6

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

#ifndef _NII_StdHeaders_H_
#define _NII_StdHeaders_H_

#ifdef __BORLANDC__
    #define __STD_ALGORITHM
#endif

#if defined (NII_GCC_VISIBILITY)
    #pragma GCC visibility push(default)
#endif

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <cassert>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <queue>
#include <bitset>

#if (N_COMPILER == N_CPP_GNUC) && !defined(STLPORT)
    #if N_COMPILER_VER >= 430
        #include <tr1/unordered_map>
        #include <tr1/unordered_set>
    #else
        #include <ext/hash_map>
        #include <ext/hash_set>
    #endif
#elif (N_COMPILER == N_CPP_GCCE)
    #if defined(_LIBCPP_VERSION)
        #include <unordered_map>
        #include <unordered_set>
    #else
        #include <tr1/unordered_map>
        #include <tr1/unordered_set>
    #endif
#else
    #if (N_COMPILER == N_CPP_MSVC) && !defined(STLPORT) && N_COMPILER_VER >= 1600 // VC++ 10.0
        #include <unordered_map>
        #include <unordered_set>
    #else
        #include <hash_set>
        #include <hash_map>
    #endif
#endif

#include <algorithm>
#include <functional>
#include <limits>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef __BORLANDC__
namespace NII
{
    using namespace std;
}
#endif

extern "C"
{
    #include <sys/types.h>
    #include <sys/stat.h>
}

#if N_PLAT == N_PLAT_WIN32
    #undef min
    #undef max
    #if defined(__MINGW32__)
        #include <unistd.h>
    #endif
#endif

#if N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_ANDROID
extern "C"
{
    #include <unistd.h>
    #include <dlfcn.h>
}
#endif

#if N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
extern "C"
{
    #include <unistd.h>
    #include <sys/param.h>
    #include <CoreFoundation/CoreFoundation.h>
}
#endif

#if NII_THREAD_SUPPORT
    #if !defined(NOMINMAX) && defined(_MSC_VER)
        #define NOMINMAX
    #endif
    #include "NiiThreadInclude.h"
#endif

#if defined (NII_GCC_VISIBILITY)
    #pragma GCC visibility pop
#endif
#endif
