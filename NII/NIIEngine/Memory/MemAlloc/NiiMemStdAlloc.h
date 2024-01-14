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

#ifndef _NII_MemoryStdAlloc_H_
#define _NII_MemoryStdAlloc_H_

#include <memory>
#include <limits>

#include "NiiPlat.h"
#include "NiiMemoryTracker.h"

#if NII_MemAllocType == NII_StdAllocType
namespace NII
{
	/** std实现
	@version NIIEngine Inner 3.0.0
	*/
    class _EngineAPI StdAlloc
    {
    public:
        static N_FORCEINLINE void * alloc(size_t s N_MEMTRACK_Decl)
        {
            return N_MemTrack_alloc(::malloc(s), s);
        }

        static N_FORCEINLINE void * realloc(void * p, size_t, size_t ns N_MEMTRACK_Decl)
        {
            return N_MemTrack_realloc(p, ::realloc(p, ns), ns);
        }

        static N_FORCEINLINE void dealloc(void * p)
        {
            N_MemTrack_free(p);
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

    /** 对齐内存分配的SIMD功能
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI AlignedMemory
    {
    public:
        /** 给定对齐,分配内存
        @param[in] s 需要分配的内存大小
        @param[in] align 字节对齐数,必须为2次方数,且范围[1, 64]内
        @version NIIEngine 3.0.0 高级api
        */
        static N_DECLMALLOC void * alloc(size_t s, size_t align);

        /** 对齐分配内存
        @param[in] s 分配的内存大小
        @version NIIEngine 3.0.0 高级api
        */
        static N_DECLMALLOC void * alloc(size_t s);

        /** 重新分配内存空间,包含对齐
        @param[in] p 需要重新分配的指针
        @param[in] os 上次已经分配的大小
        @param[in] ns 重新分配的大小
        @param[in] nalign 新字节对齐数
        @version NIIEngine 3.0.0 高级api
        */
        static void * realloc(void * p, size_t os, size_t ns, size_t nalign);

        /** 从新分配内存控件
        @param[in] p 需要重新分配的地址
        @param[in] os 上次已经分配的大小
        @param[in] ns 重新分配的大小
        @version NIIEngine 3.0.0 高级api
        */
        static void * realloc(void * p, size_t os, size_t ns);

        /** 释放分配的内存
        @param[in] p 释放的地址
        @version NIIEngine 3.0.0 高级api
        */
        static void dealloc(void * p);
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
            return N_MemTrack_alloc(align ? AlignedMemory::alloc(s, align) : AlignedMemory::alloc(s), s);
        }

        static N_FORCEINLINE void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl)
        {
            return N_MemTrack_realloc(p, align ? AlignedMemory::realloc(p, os, ns, align) : AlignedMemory::realloc(p, os, ns), ns);
        }

        static N_FORCEINLINE void dealloc(void * p)
        {
            N_MemTrack_free(p);
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
    #define N_Alloc                 StdAlloc
    #define N_AlignAlloc(align)     StdAlignAlloc<align>
}
#endif
#endif