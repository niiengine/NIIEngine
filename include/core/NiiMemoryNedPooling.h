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

#ifndef _NII_MemoryNedPooling_H_
#define _NII_MemoryNedPooling_H_

#include "NiiPlat.h"
#if NII_MEMORY_ALLOCATOR == NII_MEMORY_ALLOCATOR_NEDPOOLING

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
        static N_DECLMALLOC void * alloc(size_t s N_MEMTRACK_Decl);

        /** 重新分配内存空间
        @param[in] p 重新分配内存空间的地址
        @param[in] os 上次分配的大小(单位:字节)
        @param[in] ns 重新分配的大小(单位:字节)
        */
        static void * realloc(void * p, size_t os, size_t ns N_MEMTRACK_Decl);

        /** 释放内存空间
        @param[in] p 内存空间的地址
        */
        static void dealloc(void * p);

        /** 分配对齐字节的内存空间
        @param[in] align 对齐字节数(单位:字节)
        @param[in] s 空间大小(单位:字节)
        */
        static N_DECLMALLOC void * allocAligned(size_t align, size_t s N_MEMTRACK_Decl);

        /** 重新分配对齐字节的内存空间
        @param[in] align 重新分配的对齐字节数(单位:字节)
        @param[in] os 上次分配的大小(单位:字节)
        @param[in] ns 重新分配的大小(单位:字节)
        */
        static void * reallocAligned(size_t align, void * p, size_t os, size_t ns N_MEMTRACK_Decl);

        /** 释放对齐字节的内存空间
        @param[in] align 对齐数
        @param[in] p 需要释放的指针
        */
        static void deallocAligned(size_t align, void * p);
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
    #define N_Alloc NedPoolAlloc
    #define N_AlignAlloc(align) NedPoolAlignAlloc<align>
}
#endif
#endif