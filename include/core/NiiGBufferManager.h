/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-2

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

#ifndef _NII_GBUFFER_MANAGER_H_
#define _NII_GBUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiBufferManager.h"
#include "NiiDisplayBuffer.h"
#include "NiiVertexCom.h"

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

        /** 分配切线,辅助方法
        @version NIIEngine 3.0.0
        */
        static void allocTangents(VertexData * dst, VertexType type, Nui16 index, Nui16 texcoord);

        /// @copydetails Singleton::getOnly
        static GBufferManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static GBufferManager * getOnlyPtr();
    public:
        /** 对象已经被删除
        @note 被动方法
        @version NIIEngine 3.0.0
        */
        void onDestroy(VertexBuffer * obj);

        /** 对象已经被删除
        @note 被动方法
        @version NIIEngine 3.0.0
        */
        void onDestroy(IndexBuffer * obj);

        /** 对象已经被删除
        @note 被动方法
        @version NIIEngine 3.0.0
        */
        void onDestroy(UniformBuffer * obj);

        /** 对象已经被删除
        @note 被动方法
        @version NIIEngine 3.0.0
        */
        void onDestroy(GeometryPixel * obj);
    protected:
        N_mutex(mVertexBuffersMutex)
        N_mutex(mIndexBuffersMutex)
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
        typedef set<UniformBuffer *>::type UniformBufferList;
        typedef set<GeometryPixel *>::type GeometryPixelList;
        typedef set<VertexData *>::type VertexDataList;
    protected:
        IndexBufferList mIndexBuffers;
        VertexBufferList mVertexBuffers;
        VertexDataList mVertexDataList;
        UniformBufferList mUniformBufferList;
        GeometryPixelList mGeoPixelList;
    };
}
#endif