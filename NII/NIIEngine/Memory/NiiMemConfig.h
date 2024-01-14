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

#ifndef _NII_MEMALLOCCONFIG_H_
#define _NII_MEMALLOCCONFIG_H_

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

#if N_COMPILER == N_CPP_MSVC
#pragma warning (push)
#pragma warning (disable : 4503)
#pragma warning (disable : 4251)
#pragma warning (disable : 4786)
#pragma warning (disable : 4503)
#pragma warning (disable : 4275)

#pragma warning( disable: 4996)
#pragma warning (disable : 4100)
#pragma warning( disable : 4290)
#pragma warning( disable: 4661)

#pragma warning (disable : 201)
#endif

namespace NII
{
    template<typename T, typename AllocClass> class StlAlloc
    {
    public:
        typedef T                   value_type;
        typedef T *                 pointer;
        typedef const T *           const_pointer;
        typedef T &                 reference;
        typedef const T &           const_reference;
        typedef std::size_t         size_type;
        typedef std::ptrdiff_t      difference_type;

        /// 标准的重新绑定机制
        template<typename U> struct rebind
        {
            typedef StlAlloc<U, AllocClass> other;
        };
    public:
        inline explicit StlAlloc() {}

        virtual ~StlAlloc() {}

        inline StlAlloc(StlAlloc const &) {}

        template <typename U> inline StlAlloc(StlAlloc<U, AllocClass> const &) {}

        template <typename U, typename P> inline StlAlloc(StlAlloc<U, P> const &) {}

        N_FORCEINLINE pointer allocate(size_type count, typename std::allocator<void>::const_pointer ptr = 0)
        {
            (void)ptr;
            register size_type sz = count * sizeof(T);
            pointer p = static_cast<pointer>(AllocClass::alloc(sz N_MEMTRACK_INVO));
            return p;
        }

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

#if N_COMPILER == N_CPP_MSVC
#pragma warning (pop)
#endif

#if NII_MemAllocType == NII_NedPoolAllocType
    #include "NiiMemNedPooling.h"
#elif NII_MemAllocType == NII_NedAllocType
    #include "NiiMemNedAlloc.h"
#elif NII_MemAllocType == NII_StdAllocType
    #include "NiiMemStdAlloc.h"
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
    typedef GeometryAllocObj        GeoAlloc;
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
    template<typename T> T * NewArray(T * p, NCount cnt)
    {
        for (NCount i = 0; i < cnt; ++i)
        {
            new ((void*)(p + i)) T();
        }
        return p;
    }

    template<typename T> T * NewArray(T * src, const T * dst, NCount cnt)
    {
        for (NCount i = 0; i < cnt; ++i)
        {
            new ((void*)(src + i)) T(dst[i]);
        }
        return src;
    }
    
    template<typename T> T * NewArray(T * src, const T & dst, NCount cnt)
    {
        for (NCount i = 0; i < cnt; ++i)
        {
            new ((void*)(src + i)) T(dst);
        }
        return src;
    }
    
    template<typename T> void DeleteArray(T * p, NCount cnt)
    {
        for (NCount i = 0; i < cnt; ++i) 
        { 
            (p)[b].~T();
        }
    }
    #define N_alloc(size) N_Alloc::alloc(size N_MEMTRACK_INVO)
    #define N_realloc(ptr, olds, news) N_Alloc::realloc(ptr, olds, news N_MEMTRACK_INVO)
    #define N_alloc_t(T, count) static_cast<T *>(N_Alloc::alloc(sizeof(T) * (count) N_MEMTRACK_INVO))
    #define N_free(ptr) N_Alloc::dealloc((void *)ptr)

    #define N_new_t(T) new (N_Alloc::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    #define N_new_array_t(T, count) NewArray(static_cast<T *>(N_Alloc::alloc(sizeof(T) * (count) N_MEMTRACK_INVO)), count)
    #define N_delete_t(ptr, T) if(ptr){(ptr)->~T(); N_Alloc::dealloc(ptr);}
    #define N_delete_array_t(ptr, T, count) if(ptr){for (NCount b = 0; b < count; ++b) { (ptr)[b].~T();} N_Alloc::dealloc(ptr);}

    #define N_alloc_align(bytes, align) N_AlignAlloc(align)::alloc(bytes N_MEMTRACK_INVO)
    #define N_realloc_align(ptr, oldsize, newsize, align) N_AlignAlloc(align)::realloc(ptr, oldsize, newsize N_MEMTRACK_INVO)
    #define N_alloc_t_align(T, count, align) static_cast<T *>(N_AlignAlloc(align)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO))
    #define N_free_align(ptr, align) N_AlignAlloc(align)::dealloc(ptr)

    #define N_new_t_align(T, align) new (N_AlignAlloc(align)::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    #define N_new_array_t_align(T, count, align) NewArray(static_cast<T*>(N_AlignAlloc(align)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO)), count)
    #define N_delete_t_align(ptr, T, align) if(ptr){(ptr)->~T(); N_AlignAlloc(align)::dealloc(ptr);}
    #define N_delete_array_t_align(ptr, T, count, align) if(ptr){DeleteArray(ptr, count); N_AlignAlloc(align)::dealloc(ptr);}

    #define N_alloc_simd(bytes) N_AlignAlloc(16)::alloc(bytes N_MEMTRACK_INVO)
    #define N_alloc_t_simd(T, count) static_cast<T *>(N_AlignAlloc(16)::alloc(sizeof(T) * (count) N_MEMTRACK_INVO))
    #define N_free_simd(ptr) N_AlignAlloc(16)::dealloc(ptr)
    
    /// 分配并行算术内存对齐对象
    #define N_new_t_simd(T) new (N_AlignAlloc(16)::alloc(sizeof(T) N_MEMTRACK_INVO)) T
    /// 分配多个并行算术内存对齐对象
    #define N_new_array_t_simd(T, count) NewArray(static_cast<T *>(N_AlignAlloc(16)::alloc(sizeof(T)*(count) N_MEMTRACK_INVO)), count)
    /// 释放并行算术内存对齐对象
    #define N_delete_t_simd(ptr, T) if(ptr){(ptr)->~T(); N_AlignAlloc(16)::dealloc(ptr);}
    /// 释放多个并行算术内存对齐对象
    #define N_delete_array_t_simd(ptr, T, count) if(ptr){DeleteArray(ptr, count); N_AlignAlloc(16)::dealloc(ptr);}
    #define N_new new N_MEMTRACK_NewINVO
    #define N_delete delete
}

#endif