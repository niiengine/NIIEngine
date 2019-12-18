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

#ifndef _NII_MemoryStdAlloc_H_
#define _NII_MemoryStdAlloc_H_

#include <memory>
#include <limits>

#include "NiiPlat.h"
#include "NiiAlignAlloc.h"
#include "NiiMemoryTracker.h"

namespace NII
{
#if NII_MEMORY_ALLOCATOR == NII_MEMORY_ALLOCATOR_STD

	/** std实现
	@version NIIEngine Inner 3.0.0
	*/
    class _EngineAPI StdAlloc
    {
    public:
        static N_FORCEINLINE void * alloc(size_t s N_MEMTRACK_Decl)
        {
            void * p = malloc(s);
    #if N_MEMTRACK
            MemoryTracker::get()._recordAlloc(p, s, 0, file, line, func);
    #endif
            return p;
        }

        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            (void)os;
            void * np = ::realloc(p, ns);
    #if N_MEMTRACK
            MemoryTracker::get()._recordRealloc(np, ns, 0, file, line, func);
    #endif
            return np;
        }

        static N_FORCEINLINE void dealloc(void * p)
        {
    #if N_MEMTRACK
            MemoryTracker::get()._recordDealloc(p);
    #endif
            free(p);
        }

        /// 获取单一分配最大大小
        static N_FORCEINLINE size_t getMaxSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        StdAlloc(){}
    };

    /** std字节对齐分配(2次幂的)
	@version NIIEngine 3.0.0
    */
    template <size_t align = 0> class StdAlignAlloc
    {
    public:
        typedef int IsValidAlignment[align <= 128 && ((align & (align - 1)) == 0) ? +1 : -1];

        static N_FORCEINLINE void * alloc(size_t s N_MEMTRACK_Decl)
        {
            void * p = align ? AlignedMemory::alloc(s, align) : AlignedMemory::alloc(s);
    #if N_MEMTRACK
            MemoryTracker::get()._recordAlloc(p, s, 0, file, line, func);
    #endif
            return p;
        }

        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            void * np = align ? AlignedMemory::realloc(p, os, ns, align) : AlignedMemory::realloc(p, os, ns);
    #if N_MEMTRACK
            MemoryTracker::get()._recordRealloc(np, ns, 0, file, line, func);
    #endif
            return np;
        }

        static N_FORCEINLINE void dealloc(void * p)
        {
    #if N_MEMTRACK
            MemoryTracker::get()._recordDealloc(p);
    #endif
            AlignedMemory::dealloc(p);
        }

        /// 获取单一分配最大大小
        static N_FORCEINLINE size_t getMaxSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        StdAlignAlloc(){}
    };
    #define N_Alloc StdAlloc
    #define N_AlignAlloc(align) StdAlignAlloc<align>
#endif
}
#endif