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

#ifndef _NII_MEMALLOCCONFIG_H_
#define _NII_MEMALLOCCONFIG_H_

#include "NiiPreInclude.h"

/** 
    @code
        /// AllocObj子类
        N_Class * obj = N_new N_SubClass();
        N_delete obj;

        /// AllocObj子类数组
        N_Class * array = N_new N_SubClass[10];
        N_delete [] obj;

        /// AllocObj子类数组
        N_Class * obj = N_new_t(N_Class)(args);
        N_delete_t(obj, N_Class);

        /// AllocObj子类数组
        N_Class * obj = N_new_array_t(N_SubClass, 10);
        N_delete_array_t(obj, N_SubClass, 10);

        /// 基础数据数组
        Nui32 * i32 = N_alloc_t(Nui32, 10);
        N_free(i32);

        /// 基础数据参数
        Nui32 * i32 = N_new_t(Nui32)(0);
        N_free(i32);

        /// 内存数据
        void * pVoid = N_alloc(1024);
        N_free(pVoid);
    @endcode
*/

#ifdef new
    #undef new
#endif
#ifdef delete
    #undef delete
#endif

#include "NiiStlMemAlloc.h"

#if NII_MEMORY_ALLOCATOR == NII_MEMORY_ALLOCATOR_NEDPOOLING
    #include "NiiMemoryNedPooling.h"
#elif NII_MEMORY_ALLOCATOR == NII_MEMORY_ALLOCATOR_NED
    #include "NiiMemoryNedAlloc.h"
#elif NII_MEMORY_ALLOCATOR == NII_MEMORY_ALLOCATOR_STD
    #include "NiiMemoryStdAlloc.h"
#else
    #if !defined(N_Alloc) || !defined(N_AlignAlloc)
        #error "No Memory allocation"
    #endif
#endif


namespace NII
{
    template <class t> class _EngineAPI AllocObj
    {
    public:
        explicit AllocObj() {}

        ~AllocObj() {}

        N_FORCEINLINE void * operator new(NCount s, const Ntchar * file, int line, const Ntchar * func)
        {
            return t::alloc(s N_MEMTRACK_INVO);
        }

        N_FORCEINLINE void * operator new(NCount s)
        {
            return t::alloc(s N_MEMTRACK_INVO);
        }

        N_FORCEINLINE void * operator new(NCount s, void * ptr)
        {
            (void)s;
            return ptr;
        }

        N_FORCEINLINE void * operator new[](NCount s, const Ntchar * file, int line, const Ntchar * func)
        {
            return t::alloc(s N_MEMTRACK_Param);
        }

        N_FORCEINLINE void * operator new[](NCount s)
        {
            return t::alloc(s N_MEMTRACK_INVO);
        }

        N_FORCEINLINE void operator delete(void * ptr)
        {
            t::dealloc(ptr);
        }

        N_FORCEINLINE void operator delete(void * ptr, void *)
        {
            t::dealloc(ptr);
        }

        N_FORCEINLINE void operator delete(void * ptr, const Ntchar *, NIIi, const Ntchar *)
        {
            t::dealloc(ptr);
        }

        N_FORCEINLINE void operator delete[](void * ptr)
        {
            t::dealloc(ptr);
        }

        N_FORCEINLINE void operator delete[](void * ptr, const Ntchar *, NIIi, const Ntchar *)
        {
            t::dealloc(ptr);
        }
    };

    typedef AllocObj<N_Alloc>       NetSysAllocObj;
    typedef AllocObj<N_Alloc>       InputSysAllocObj;
    typedef AllocObj<N_Alloc>       PhysicalAllocObj;
    typedef AllocObj<N_Alloc>       SoundSysAllocObj;
    typedef AllocObj<N_Alloc>       RenderSysAllocObj;
    typedef AllocObj<N_Alloc>       ResourceAllocObj;
    typedef AllocObj<N_Alloc>       SceneObjAllocObj;
    typedef AllocObj<N_Alloc>       CommonAllocObj;
    typedef AllocObj<N_Alloc>       GeometryAllocObj;
    typedef AllocObj<N_Alloc>       EventAllocObj;

    #define CommonAlignAllocObj(align)      AllocObj<N_AlignAlloc(align)>
    #define GeometryAlignAllocObj(align)    AllocObj<N_AlignAlloc(align)>
    #define SceneObjAlignAllocObj(align)    AllocObj<N_AlignAlloc(align)>
    #define ResourceAlignAllocObj(align)    AllocObj<N_AlignAlloc(align)>
    #define ScriptSysAlignAllocObj(align)   AllocObj<N_AlignAlloc(align)>
    #define RenderSysAlignAllocObj(align)   AllocObj<N_AlignAlloc(align)>
    #define SoundSysAlignAllocObj(align)    AllocObj<N_AlignAlloc(align)>
    #define InputSysAlignAllocObj(align)    AllocObj<N_AlignAlloc(align)>

    typedef ResourceAllocObj        VFSAlloc;
    typedef ResourceAllocObj        XmlAlloc;
    typedef GeometryAllocObj        GeometryAlloc;
    typedef GeometryAllocObj        AnimableAlloc;
    typedef RenderSysAllocObj       BufferAlloc;
    typedef CommonAllocObj          CodecAlloc;
    typedef ResourceAllocObj        ConfigAlloc;
    typedef CommonAllocObj          ControlAlloc;
    typedef GeometryAllocObj        DebugGeoAlloc;
    typedef CommonAllocObj          DynamicModuleAlloc;
    typedef GeometryAllocObj        GeoEdgetAlloc;
    typedef ResourceAllocObj        FactoryAlloc;
    typedef SceneObjAllocObj        FXAlloc;
    typedef CommonAllocObj          ImageAlloc;
    typedef GeometryAllocObj        IndexDataAlloc;
    typedef CommonAllocObj          LogAlloc;
    typedef SceneObjAllocObj        SpaceAlloc;
    typedef SceneObjAllocObj        NodeAlloc;
    typedef SceneObjAllocObj        OverlayAlloc;
    typedef RenderSysAllocObj       GpuParamsAlloc;
    typedef ResourceAllocObj        PassAlloc;
    typedef GeometryAllocObj        PatchAlloc;
    typedef CommonAllocObj          PluginAlloc;
    typedef CommonAllocObj          ProfilerAlloc;
    typedef GeometryAllocObj        ProgMeshAlloc;
    typedef SceneObjAllocObj        RenderQueueAlloc;
    typedef SceneObjAllocObj        RenderAlloc;
    typedef RenderSysAllocObj       RenderSysAlloc;
    typedef CommonAllocObj          UserAlloc;
    typedef CommonAllocObj          EngineAlloc;
    typedef ResourceAllocObj        ResourceAlloc;
    typedef CommonAllocObj          SerializerAlloc;
    typedef SceneObjAllocObj        SceneAlloc;
    typedef ResourceAllocObj        ScriptAlloc;
    typedef SceneObjAllocObj        ShadowAlloc;
    typedef CommonAllocObj          StreamAlloc;
    typedef SceneObjAllocObj        GeometryObjAlloc;
    typedef GeometryAllocObj        BatchObjectAlloc;
    typedef ResourceAllocObj        SubMeshAlloc;
    typedef ResourceAllocObj        MaterialAlloc;
    typedef CommonAllocObj          ShaderAlloc;
    typedef CommonAllocObj          TimerAlloc;
    typedef GeometryAllocObj        ShaderChAlloc;
    typedef CommonAllocObj          UtilityAlloc;
    typedef GeometryAllocObj        VertexDataAlloc;
    typedef RenderSysAllocObj       TextureAlloc;
    typedef RenderSysAllocObj       ViewportAlloc;
    typedef SceneObjAllocObj        LodAlloc;
    typedef CommonAllocObj          SysAlloc;
    typedef SoundSysAllocObj        AudioAlloc;
    typedef SceneObjAllocObj        VideoAlloc;
    typedef CommonAllocObj          ManagerAlloc;
    typedef ResourceAllocObj        FactoryAlloc;
    typedef CommonAllocObj          PatternAlloc;
    typedef EventAllocObj           EventAlloc;
    typedef NetSysAllocObj          NetWorkAlloc;
    typedef CommonAllocObj          GameAlloc;
    typedef ResourceAllocObj        FilePatchAlloc;
    typedef ResourceAllocObj        FontAlloc;
    typedef CommonAllocObj          PhysicalAlloc;
    typedef ResourceAllocObj        JobAlloc;
    typedef CommonAllocObj          CPUAlloc;
    typedef CommonAllocObj          GPUAlloc;
    typedef CommonAllocObj          DataAlloc;
    typedef SoundSysAllocObj        SoundAlloc;
    typedef GeometryAllocObj        UIAlloc;
    typedef ResourceAllocObj        IDAlloc;
    typedef CommonAllocObj          FrameAlloc;
    typedef CommonAllocObj          ParticleAlloc;
}

namespace NII
{
    template<typename T> T * NewArray(T * p, NCount count)
    {
        for (NCount i = 0; i < count; ++i)
        {
            new ((void*)(p + i)) T();
        }
        return p;
    }
#if N_DEBUG
    /// 分配内存
    #define N_alloc(size) N_Alloc::alloc(size N_MEMTRACK_INVO)
    /// 分配内存
    #define N_realloc(ptr, olds, news) N_Alloc::realloc(ptr, olds, news N_MEMTRACK_INVO)
    /// 分配内存
    #define N_alloc_t(T, count) (T *)(N_Alloc::alloc(sizeof(T) * (count) N_MEMTRACK_INVO))
    /// 分配内存 N_alloc 和 N_alloc_t 还有 N_realloc
    #define N_free(ptr) N_Alloc::dealloc((void *)ptr)

    /// 分配对象内存
    #define N_new_t(T) new (N_Alloc::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    /// 分配对象数组内存
    #define N_new_array_t(T, count) NewArray(static_cast<T *>(N_Alloc::alloc(sizeof(T) * (count) N_MEMTRACK_INVO)), count)
    /// 释放对象内存
    #define N_delete_t(ptr, T) if(ptr){(ptr)->~T(); N_Alloc::dealloc((void *)ptr);}
    /// 释放对象数组内存
    #define N_delete_array_t(ptr, T, count) if(ptr){for (NCount b = 0; b < count; ++b) { (ptr)[b].~T();} N_Alloc::dealloc(ptr);}

    /// 分配字节对齐的内存
    #define N_alloc_align(bytes, align) N_AlignAlloc(align)::alloc(bytes N_MEMTRACK_INVO)
    /// 从新分配已经分配字节对齐的内存
    #define N_realloc_align(ptr, os, ns, align) N_AlignAlloc(align)::realloc(ptr, os, ns N_MEMTRACK_INVO)
    /// 分配字节对齐的内存
    #define N_alloc_t_align(T, count, align) static_cast<T *>(N_AlignAlloc(align)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO))
    /// 分配字节对齐的内存N_alloc_align和N_alloc_t_align,还有N_realloc_align
    #define N_free_align(ptr, align) N_AlignAlloc(align)::dealloc(ptr)
    /// 分配字节对齐对象
    #define N_new_t_align(T, align) new (N_AlignAlloc(align)::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    /// 分配字节对齐对象数组
    #define N_new_array_t_align(T, count, align) NewArray(static_cast<T*>(N_AlignAlloc(align)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO)), count)
    /// 释放字节对齐对象
    #define N_delete_t_align(ptr, T, align) if(ptr){(ptr)->~T(); N_AlignAlloc(align)::dealloc(ptr);}
    /// 释放字节对齐对象数组
    #define N_delete_array_t_align(ptr, T, count, align) if(ptr){for (NCount _b = 0; _b < count; ++_b) { (ptr)[_b].~T();} N_AlignAlloc(align)::dealloc(ptr);}

    /// 并行算术内存分配专用
    #define N_alloc_simd(bytes) N_AlignAlloc(16)::alloc(bytes N_MEMTRACK_INVO)
    /// 并行算术内存分配专用
    #define N_alloc_t_simd(T, count) static_cast<T *>(N_AlignAlloc(16)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO))
    /// 并行算术内存分配专用 N_alloc_simd 和or N_alloc_t_simd
    #define N_free_simd(ptr) N_AlignAlloc(16)::dealloc(ptr)
    /// 分配并行算术内存对齐对象
    #define N_new_t_simd(T) new (N_AlignAlloc(16)::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    /// 分配多个并行算术内存对齐对象
    #define N_new_array_t_simd(T, count) NewArray(static_cast<T *>(N_AlignAlloc(16)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO)), count)
    /// 释放并行算术内存对齐对象
    #define N_delete_t_simd(ptr, T) if(ptr){(ptr)->~T(); N_AlignAlloc(16)::dealloc(ptr);}
    /// 释放多个并行算术内存对齐对象
    #define N_delete_array_t_simd(ptr, T, count) if(ptr){for (NCount b = 0; b < count; ++b) { (ptr)[b].~T();} N_AlignAlloc(16)::dealloc(ptr);}
#else
    /// 分配内存
    #define N_alloc(size) N_Alloc::alloc(size N_MEMTRACK_INVO)
    /// 分配内存
    #define N_realloc(ptr, olds, news) N_Alloc::realloc(ptr, olds, news N_MEMTRACK_INVO)
    /// 分配内存
    #define N_alloc_t(T, count) static_cast<T *>(N_Alloc::alloc(sizeof(T) * (count) N_MEMTRACK_INVO))
    /// 分配内存 N_alloc 和 N_alloc_t 还有 N_realloc
    #define N_free(ptr) N_Alloc::dealloc((void *)ptr)

    /// 分配对象内存
    #define N_new_t(T) new (N_Alloc::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    /// 分配对象数组内存
    #define N_new_array_t(T, count) NewArray(static_cast<T *>(N_Alloc::alloc(sizeof(T) * (count) N_MEMTRACK_INVO)), count)
    /// 释放对象内存
    #define N_delete_t(ptr, T) if(ptr){(ptr)->~T(); N_Alloc::dealloc(ptr);}
    /// 释放对象数组内存
    #define N_delete_array_t(ptr, T, count) if(ptr){for (NCount b = 0; b < count; ++b) { (ptr)[b].~T();} N_Alloc::dealloc(ptr);}

    /// 分配字节对齐的内存
    #define N_alloc_align(bytes, align) N_AlignAlloc(align)::alloc(bytes N_MEMTRACK_INVO)
    /// 从新分配已经分配字节对齐的内存
    #define N_realloc_align(ptr, os, ns, align) N_AlignAlloc(align)::realloc(ptr, os, ns N_MEMTRACK_INVO)
    /// 分配字节对齐的内存
    #define N_alloc_t_align(T, count, align) static_cast<T *>(N_AlignAlloc(align)::alloc(sizeof(T) * (count N_MEMTRACK_INVO)))
    /// 分配字节对齐的内存N_alloc_align和N_alloc_t_align还有N_realloc_align
    #define N_free_align(ptr, align) N_AlignAlloc(align)::dealloc(ptr)
    /// 分配字节对齐对象
    #define N_new_t_align(T, align) new (N_AlignAlloc(align)::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    /// 分配字节对齐对象数组
    #define N_new_array_t_align(T, count, align) NewArray(static_cast<T *>(N_AlignAlloc(align)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO)), count)
    /// 释放字节对齐对象
    #define N_delete_t_align(ptr, T, align) if(ptr){(ptr)->~T(); N_AlignAlloc(align)::dealloc(ptr);}
    /// 释放字节对齐对象数组
    #define N_delete_array_t_align(ptr, T, count, align) if(ptr){for (NCount _b = 0; _b < count; ++_b) { (ptr)[_b].~T();} N_AlignAlloc(align)::dealloc(ptr);}

    /// 并行算术内存分配专用
    #define N_alloc_simd(bytes) N_AlignAlloc(16)::alloc(bytes N_MEMTRACK_INVO)
    /// 并行算术内存分配专用
    #define N_alloc_t_simd(T, count) static_cast<T *>(N_AlignAlloc(16)::alloc(sizeof(T) * (count) N_MEMTRACK_INVO))
    /// 并行算术内存分配专用 N_alloc_simd 和 N_alloc_t_simd
    #define N_free_simd(ptr) N_AlignAlloc(16)::dealloc(ptr)
    /// 分配并行算术内存对齐对象
    #define N_new_t_simd(T) new (N_AlignAlloc(16)::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    /// 分配并行算术内存对齐对象数组
    #define N_new_array_t_simd(T, count) NewArray(static_cast<T *>(N_AlignAlloc(16)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO)), count)
    /// 释放并行算术内存对齐对象
    #define N_delete_t_simd(ptr, T) if(ptr){(ptr)->~T(); N_AlignAlloc(16)::dealloc(ptr);}
    /// 释放并行算术内存对齐对象数组
    #define N_delete_array_t_simd(ptr, T, count) if(ptr){for (NCount b = 0; b < count; ++b) { (ptr)[b].~T();} N_AlignAlloc(16)::dealloc(ptr);}
#endif
#define N_new new N_MEMTRACK_NewINVO
#define N_delete delete
}

#endif