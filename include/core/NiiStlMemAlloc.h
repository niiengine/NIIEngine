/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-17

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

#ifndef _NII_STL_MEM_ALLOC_H_
#define _NII_STL_MEM_ALLOC_H_

#include "NiiPreInclude.h"
#include "NiiHeaderPrefix.h"

namespace NII
{
    template<typename T, typename AllocClass> class StlAlloc
    {
    public:
        typedef T                    value_type;
        typedef T *                    pointer;
        typedef const T *            const_pointer;
        typedef T &                    reference;
        typedef const T &            const_reference;
        typedef std::size_t            size_type;
        typedef std::ptrdiff_t        difference_type;

        /// 标准的重新绑定机制
        template<typename U> struct rebind
        {
            typedef StlAlloc<U, AllocClass> other;
        };
    public:
        inline explicit StlAlloc(){}

        virtual ~StlAlloc(){}

        inline StlAlloc(StlAlloc const &){}

        template <typename U> inline StlAlloc(StlAlloc<U, AllocClass> const &){}

        template <typename U, typename P> inline StlAlloc( StlAlloc<U, P> const &){}

        /// 内存分配
        N_FORCEINLINE pointer allocate(size_type count, typename std::allocator<void>::const_pointer ptr = 0)
        {
            (void)ptr;
            register size_type sz = count * sizeof( T );
            pointer p  = static_cast<pointer>(AllocClass::alloc(sz N_MEMTRACK_INVO));
            return p;
        }

        /// 内存释放
        N_FORCEINLINE void deallocate(pointer ptr, size_type)
        {
            AllocClass::dealloc(ptr);
        }

        N_FORCEINLINE pointer address(reference x)
        {
            return &x;
        }

        N_FORCEINLINE const_pointer address(const_reference x) const
        {
            return &x;
        }

        N_FORCEINLINE size_type max_size() const throw()
        {
            return AllocClass::getMaxSize();
        }

        N_FORCEINLINE void construct(pointer p)
        {
            new(static_cast<void *>(p)) T();
        }

        N_FORCEINLINE void construct(pointer p, const T & val)
        {
            new(static_cast<void *>(p)) T(val);
        }

        N_FORCEINLINE void destroy(pointer p)
        {
            p->~T();
        }

    };

    /// (ISO C++)
    template<typename T, typename T2, typename P>
    N_FORCEINLINE bool operator ==(StlAlloc<T, P> const &, StlAlloc<T2, P> const &)
    {
        return true;
    }

    /// (ISO C++)
    template<typename T, typename P, typename OtherAllocator>
    N_FORCEINLINE bool operator ==(StlAlloc<T, P> const &, OtherAllocator const &)
    {
        return false;
    }
    ///  (ISO C++)
    template<typename T, typename T2, typename P>
    N_FORCEINLINE bool operator !=(StlAlloc<T, P> const &, StlAlloc<T2, P> const &)
    {
        return false;
    }

    /// (ISO C++)
    template<typename T, typename P, typename OtherAllocator>
    N_FORCEINLINE bool operator !=(StlAlloc<T, P> const &, OtherAllocator const &)
    {
        return true;
    }
}
#include "NiiHeaderSuffix.h"
#endif