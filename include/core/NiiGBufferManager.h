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

#ifndef _NII_GBUFFER_MANAGER_H_
#define _NII_GBUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiBufferManager.h"
#include "NiiDisplayBuffer.h"
#include "NiiVertexData.h"

namespace NII
{
    /** GPU硬件缓冲管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GBufferManager : public BufferManager, public Singleton<GBufferManager>
    {
        friend class Engine;
    public:
        /** 缓存类型
        @version NIIEngine 3.0.0
        */
        enum BufferType
        {
            BT_Vertex,
            BT_Index,
            BT_VertexData,
            BT_IndexData
        };
    public:
        GBufferManager();
        virtual ~GBufferManager();

        /** 创建顶点缓存
        @param[in] size 每顶点在缓存中的大小(单位:字节),一个顶点含概位置,法线,辅法线等
        @param[in] count 顶点数量.
        @param[in] mode 一般情况使用 Buffer::M_GPU | M_WRITE, 如果写入则应包含 M_WRITE 或 M_CPU
        @version NIIEngine 3.0.0
        */
        virtual void create(VertexBuffer *& out, NCount size, NCount count, Nmark mode) = 0;

        /** 删除顶点缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(VertexBuffer * obj);

        /** 创建索引缓存
        @param[in] size 索引大小,(16)16位/(32)32位
        @param[in] count 在缓存中索引的数量.
        @param[in] mode 一般情况使用 Buffer::M_GPU | M_WRITE, 如果写入则应包含 M_WRITE 或 M_CPU
        @version NIIEngine 3.0.0
        */
        virtual void create(IndexBuffer *& out, NCount size, NCount count, Nmark mode) = 0;
        
        /** 删除索引缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(IndexBuffer * obj);
        
        /** 创建计数缓存
        @param[in] size 大小
        @param[in] mode 一般情况使用 Buffer::M_GPU | M_WRITE, 如果写入则应包含 M_WRITE 或 M_CPU
        @version NIIEngine 3.0.0
        */
        virtual void create(CounterBuffer *& out, NCount size, Nmark mode) = 0;

        /** 删除计数缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(CounterBuffer * obj);

        /** 创建索引缓存
        @param[in] size 大小
        @param[in] mode 一般情况使用 Buffer::M_GPU | M_WRITE, 如果写入则应包含 M_WRITE 或 M_CPU
        @version NIIEngine 3.0.0
        */
        virtual void create(UniformBuffer *& out, NCount size, Nmark mode) = 0;

        /** 删除索引缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(UniformBuffer * obj);

        /** 创建显示列表
        @version NIIEngine 3.0.0
        */
        virtual void create(DisplayBuffer *& out) = 0;
        
        /** 删除显示列表
        @version NIIEngine 3.0.0
        */
        virtual void destroy(DisplayBuffer * obj);

        /** 创建顶点数据
        @param[out] out 创建出来的对象
        @version NIIEngine 3.0.0
        */
        virtual void create(VertexData *& out);

        /** 删除顶点数据
        @version NIIEngine 3.0.0
        */
        virtual void destroy(VertexData * obj);

        /** 创建索引数据
        @param[out] out 创建出来的对象
        @version NIIEngine 3.0.0
        */
        virtual void create(IndexData *& out);

        /** 删除索引数据
        @version NIIEngine 3.0.0
        */
        virtual void destroy(IndexData * obj);
        
        /** 创建几何像素
        @param[out] out 创建出来的对象
        @version NIIEngine 3.0.0
        */
        virtual void create(GeometryPixel *& out);

        /** 删除几何像素
        @param[in] obj 要删除的对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(GeometryPixel * obj);

        /** 删除所有缓存
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /// @copydetails Singleton::getOnly
        static GBufferManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static GBufferManager * getOnlyPtr();
    public:
        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        void onDestroy(VertexBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        void onDestroy(IndexBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        void onDestroy(UniformBuffer * obj);
        
        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        void onDestroy(CounterBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        void onDestroy(GeometryPixel * obj);
    protected:
        N_mutex(mVertexBuffersMutex)
        N_mutex(mIndexBuffersMutex)
        N_mutex(mCounterBuffersMutex)
        N_mutex(mUniformBuffersMutex)
        N_mutex(mGeoPixelListMutex)
        N_mutex(mVertexDataListMutex)
    protected:
        /** 创建顶点数据实现
        @version NIIEngine 高级api
        */
        virtual void createImpl(VertexData *& out) = 0;

        /** 删除顶点数据实现
        @version NIIEngine 高级api
        */
        virtual void destroyImpl(VertexData * obj) = 0;
    protected:
        typedef set<IndexBuffer *>::type IndexBufferList;
        typedef set<VertexBuffer *>::type VertexBufferList;
        typedef set<CounterBuffer *>::type CounterBufferList;
        typedef set<UniformBuffer *>::type UniformBufferList;
        typedef set<GeometryPixel *>::type GeometryPixelList;
        typedef set<VertexData *>::type VertexDataList;
    protected:
        IndexBufferList mIndexBuffers;
        VertexBufferList mVertexBuffers;
        VertexDataList mVertexDataList;
        CounterBufferList mCounterBufferList;
        UniformBufferList mUniformBufferList;
        GeometryPixelList mGeoPixelList;
    };
}
#endif