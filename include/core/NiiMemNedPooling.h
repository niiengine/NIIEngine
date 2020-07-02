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

#ifndef _NII_MemoryNedPooling_H_
#define _NII_MemoryNedPooling_H_

#include "NiiPlat.h"
#include "NiiMemoryTracker.h"
#if NII_MemAllocType == NII_NedPoolAllocType
#include "nedmalloc.h"
namespace NII
{
    /** nedpool实现
	@version NIIEngine Inner 3.0.0
    */
    class _EngineAPI NedPoolingImpl
    {
    public:
        /** 分配内存空间
        @param[in] s 需要分配的大小(单位:字节)
        @version NIIEngine Inner 3.0.0
        */
        static N_FORCEINLINE N_DECLMALLOC void * alloc(size_t s N_MEMTRACK_Decl)
        {
            return N_MemTrack_alloc(internalAlloc(s), s);
        }

        /** 重新分配内存空间
        @param[in] p 重新分配内存空间的地址
        @param[in] os 上次分配的大小(单位:字节)
        @param[in] ns 重新分配的大小(单位:字节)
        */
        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return N_MemTrack_realloc(p, internalRealloc(p, os, ns), ns);
        }

        /** 释放内存空间
        @param[in] p 内存空间的地址
        */
        static N_FORCEINLINE void dealloc(void * p)
        {
            N_assert(p, "error");
            internalFree(p);
            N_MemTrack_free(p);
        }

        /** 分配对齐字节的内存空间
        @param[in] align 对齐字节数(单位:字节)
        @param[in] s 空间大小(单位:字节)
        */
        static N_FORCEINLINE N_DECLMALLOC void * allocAligned(size_t align, size_t s N_MEMTRACK_Decl)
        {
            return N_MemTrack_alloc(internalAllocAlign(align, s), s);
        }

        /** 重新分配对齐字节的内存空间
        @param[in] align 重新分配的对齐字节数(单位:字节)
        @param[in] os 上次分配的大小(单位:字节)
        @param[in] ns 重新分配的大小(单位:字节)
        */
        static N_FORCEINLINE void * reallocAligned(size_t align, void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return N_MemTrack_realloc(p, internalReallocAlign(p, align, os, ns), ns);
        }

        /** 释放对齐字节的内存空间
        @param[in] align 对齐数
        @param[in] p 需要释放的指针
        */
        static N_FORCEINLINE void deallocAligned(size_t align, void * p)
        {
            N_assert(p, "error");
            internalFree(p);
            N_MemTrack_free(p);
        }
    private:
        static void * internalAlloc(size_t s);
        static void * internalRealloc(void * p, size_t os, size_t ns);
        static void internalFree(void * p);
        static void * internalAllocAlign(size_t align, size_t s);
        static void * internalReallocAlign(void * p, size_t align, size_t os, size_t ns);
    };

    /** nedpool分配
	@version NIIEngine 3.0.0
    */
    class _EngineAPI NedPoolAlloc
    {
    public:
        /// @copydetails NedPoolingImpl::alloc
        static N_FORCEINLINE void * alloc(size_t s N_MEMTRACK_Decl)
        {
            return NedPoolingImpl::alloc(s N_MEMTRACK_Param);
        }

        /// @copydetails NedPoolingImpl::realloc
        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return NedPoolingImpl::realloc(p, os, ns N_MEMTRACK_Param);
        }

        /// @copydetails NedPoolingImpl::dealloc
        static N_FORCEINLINE void dealloc(void * p)
        {
            NedPoolingImpl::dealloc(p);
        }

        /// 获取单一分配的最大大小
        static N_FORCEINLINE NCount getMaxSize()
        {
            return std::numeric_limits<NCount>::max();
        }
    private:
        NedPoolAlloc(){}
    };

    /** nedpool字节对齐分配(2次幂的)
	@version NIIEngine 3.0.0
    */
    template <NCount align = 0> class NedPoolAlignAlloc
    {
    public:
        typedef int IsValidAlignment[align <= 128 && ((align & (align-1)) == 0) ? +1 : -1];
        
        /// @copydetails NedPoolingImpl::allocAligned
        static N_FORCEINLINE void * alloc(size_t s N_MEMTRACK_Decl)
        {
            return NedPoolingImpl::allocAligned(align, s N_MEMTRACK_Param);
        }

        /// @copydetails NedPoolingImpl::reallocAligned
        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return NedPoolingImpl::reallocAligned(align, p, os, ns N_MEMTRACK_Param);
        }

        /// @copydetails NedPoolingImpl::deallocAligned
        static N_FORCEINLINE void dealloc(void * p)
        {
            NedPoolingImpl::deallocAligned(align, p);
        }

        /// 获取单一分配最大大小
        static N_FORCEINLINE size_t getMaxSize()
        {
            return std::numeric_limits<NCount>::max();
        }
    private:
        NedPoolAlignAlloc(){}
    };
    #define N_Alloc                 NedPoolAlloc
    #define N_AlignAlloc(align)     NedPoolAlignAlloc<align>
}
#endif
#endif