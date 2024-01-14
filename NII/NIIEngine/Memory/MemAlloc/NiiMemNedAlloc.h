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

#ifndef _NII_MemoryNedAlloc_H_
#define _NII_MemoryNedAlloc_H_

#include "NiiMemoryTracker.h"
#if NII_MemAllocType == NII_NedAllocType

#include "nedmalloc.h"
#include<cassert>
namespace NII
{
    /** ned实现
    @version NIIEngine Inner 3.0.0
    */
    class _EngineAPI NedAllocImpl
    {
    public:
        /** 分配内存空间
        @param[in] s 需要分配的大小(单位:字节)
        */
        static N_FORCEINLINE N_DECLMALLOC void * alloc(size_t s N_MEMTRACK_Decl)
        {
            return N_MemTrack_alloc(nedalloc::nedmalloc(s), s);
        }

        /** 重新分配内存空间
        @param[in] p 重新分配内存空间的地址
        @param[in] os 上次分配的大小
        @param[in] ns 重新分配的大小(单位:字节)
        */
        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return N_MemTrack_realloc(p, nedalloc::nedrealloc(p, ns), ns);
        }

        /** 释放内存空间
        @param[in] p 内存空间的地址
        */
        static N_FORCEINLINE void dealloc(void * p)
        {
            N_assert(p, "error");
            nedalloc::nedfree(p);
            N_MemTrack_free(p);
        }

        /** 分配对齐字节的内存空间
        @param[in] align 对齐字节数(单位:字节)
        @param[in] s 空间大小
        */
        static N_FORCEINLINE N_DECLMALLOC void * allocAligned(size_t align, size_t s N_MEMTRACK_Decl)
        {
            return N_MemTrack_alloc(nedalloc::nedmemalign(align, s), s);
        }

        /** 重新分配对齐字节的内存空间
        @param[in] align 重新分配的对齐字节数(单位:字节)
        @param[in] os 上次分配的大小(单位:字节)
        @param[in] ns 重新分配的大小(单位:字节)
        */
        static N_FORCEINLINE void * reallocAligned(size_t align, void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            void * np = nedalloc::nedmemalign(align, ns);
            memcpy(np, p, std::min<size_t>(os, ns));
            nedalloc::nedfree(p);
            return N_MemTrack_realloc(p, np, ns);
        }

        /** 释放对齐字节的内存空间
        @param[in] align 对齐数
        @param[in] p 需要释放的指针
        */
        static N_FORCEINLINE void deallocAligned(size_t align, void * p)
        {
            N_assert(p, "error");
            N_MemTrack_free(p);
            nedalloc::nedfree(p);
        }
    };

    /** ned分配
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NedAlloc
    {
    public:
        /// @copydetails NedAllocImpl::alloc
        static N_FORCEINLINE void * alloc(size_t s N_MEMTRACK_Decl)
        {
            return NedAllocImpl::alloc(s N_MEMTRACK_Param);
        }

        /// @copydetails NedAllocImpl::realloc
        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return NedAllocImpl::realloc(p, os, ns N_MEMTRACK_Param);
        }

        /// @copydetails NedAllocImpl::dealloc
        static N_FORCEINLINE void dealloc(void * p)
        {
            NedAllocImpl::dealloc(p);
        }

        /// 获取单一分配最大大小
        static N_FORCEINLINE size_t getMaxSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        NedAlloc(){}
    };

    /** ned字节对齐分配(2次幂的)
    @version NIIEngine 3.0.0
    */
    template <size_t align = 0> class NedAlignAlloc
    {
    public:
        typedef int IsValidAlignment[align <= 128 && ((align & (align - 1)) == 0) ? +1 : -1];

        /// @copydetails NedAllocImpl::allocAligned
        static N_FORCEINLINE void * alloc(size_t s N_MEMTRACK_Decl)
        {
            return NedAllocImpl::allocAligned(align, s N_MEMTRACK_Param);
        }

        /// @copydetails NedAllocImpl::reallocAligned
        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return NedAllocImpl::reallocAligned(align, p, os, ns N_MEMTRACK_Param);
        }

        /// @copydetails NedAllocImpl::deallocAligned
        static N_FORCEINLINE void dealloc(void * p)
        {
            NedAllocImpl::deallocAligned(align, p);
        }

        /// 获取单一分配的最大大小
        static N_FORCEINLINE size_t getMaxSize()
        {
            return (std::numeric_limits<size_t>::max)();
        }
    private:
        NedAlignAlloc(){}
    };
    #define N_Alloc                 NedAlloc
    #define N_AlignAlloc(align)     NedAlignAlloc<align>
}
#endif
#endif