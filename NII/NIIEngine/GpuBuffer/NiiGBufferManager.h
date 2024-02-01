﻿/*
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
#include "NiiVertexIndexIndirect.h"

namespace NII
{
    /** GPU硬件缓冲管理器
    @remark 当各个成分各属于同个组时，将自动启用渲染优化
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GBufferManager : public BufferManager, public Singleton<GBufferManager>
    {
        friend class Engine;
    public:
        /** 使用策略类型
        @version NIIEngine 5.0.0
        */
        enum StrategyType
        {
            ST_Unknow,
            ST_Manual,          ///<手动回收，否则不回收
            ST_DrawCall,        ///<每次调用绘制函数完成时回收(不是调用返回,而是命令已经finish)
            ST_QueueDrawCall,   ///<每次调用队列绘制完成时回收(不是调用返回,而是命令已经finish)
            ST_FrameCall,       ///<每次调用帧绘制完成时回收(不是调用返回,而是命令已经finish)
            ST_Event,           ///<每次事件触发时回收
            ST_Count
        };
    public:
        GBufferManager();
        virtual ~GBufferManager();
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void _update();
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual Nui8 wait() = 0;

        /** 
        @version NIIEngine 6.0.0
        */
        virtual void wait(NCount frameCount) = 0;
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void _beginFrame() {}
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void _endFrame() {}
        
        /**
        @version NIIEngine 5.0.0
        */
        virtual bool isStaticUpdate() const                     { return true; }

        /** 添加池
        @version NIIEngine 5.0.0
        */
        virtual bool addPool(GpuGroupID gid, BufferModeMark mode, StrategyType stype, NCount size);

        /** 移去池
        @version NIIEngine 5.0.0
        */
        virtual bool removePool(GpuGroupID gid);

        /** 移去所有池
        @version NIIEngine 5.0.0
        */
        virtual void removeAllPool();
        
        /** 回收不使用的池
        @version NIIEngine 5.0.0
        */
        virtual void recoverPool() = 0;

        /** 设置是否强制池组
        @note 当各个成分各属于同个组时，将自动启用渲染优化
        @version NIIEngine 5.0.0
        */
        void setForcePoolGroup(bool set)    { mForcePoolGroup = set; }

        /** 获取是否强制池组
        @note 当各个成分各属于同个组时，将自动启用渲染优化
        @version NIIEngine 5.0.0
        */
        bool isForcePoolGroup() const       { return mForcePoolGroup; }

        /** 设置池默认大小
        @note 默认128MByte(单位:字节)
        @version NIIEngine 5.0.0
        */
        void setPoolSize(NCount size)       { mPoolDefaultSize = size; }

        /** 设置池默认大小
        @note 默认128MByte(单位:字节)
        @version NIIEngine 5.0.0
        */
        NCount getPoolSize() const          { return mPoolDefaultSize; }

        /** 构建VAO(vertex array object)
        @note 如果调用完后mVaoId == -1 则说明渲染系统不支持
        @version NIIEngine 5.0.0
        */
        virtual void setupVAO(GeometryRaw * obj) = 0;

        /** 解除VAO(vertex array object)
        @note 调用完后mVaoId == -1
        @version NIIEngine 5.0.0
        */
        virtual void unsetupVAO(GeometryRaw * obj);
        
        /** 解除所有VAO(vertex array object)
        @version NIIEngine 5.0.0
        */
        void unsetupAllVAO();

        /** 请求映射
        @version NIIEngine 5.0.0
        */
        virtual void request(MappedBuffer *& out, NCount size, BufferModeMark modemark = M_HOSTWRITE);

        /** 回收映射
        @version NIIEngine 5.0.0
        */
        virtual void recover(MappedBuffer * obj);
        
        /** 创建绘制元
        @version NIIEngine 5.0.0
        */
        virtual void create(GeometryRaw *& out) = 0;
        
        /** 删除绘制元
        @version NIIEngine 5.0.0
        */
        virtual void destroy(GeometryRaw *& out);

        /** 创建异步读取
        @version NIIEngine 5.0.0
        */
        virtual void create(GpuBufferAsync *& out, GpuBuffer * buf, NCount oft, NCount size, MappedBuffer * buf = 0) = 0;

        /** 创建顶点缓存
        @param[in] usize 每顶点在缓存中的大小(单位:字节),一个顶点含概位置,法线,辅法线等
        @param[in] ucount 顶点数量.
        @param[in] mode 一般情况使用 Buffer::M_DEVWRITE, 如果读取则应包含 Buffer::M_HOSTREAD
		@param[in] multi 双缓存则为2，离线混合渲染则更高
        @param[in] pool 是否存在于数据池中的某一连续数据空间,0值则不属于任何一个池
        @version NIIEngine 3.0.0
        */
        virtual void create(VertexBuffer *& out, NCount usize, NCount ucount, BufferModeMark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0) = 0;

        /** 删除顶点缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(VertexBuffer * obj);

        /** 创建索引缓存
        @param[in] usize 索引大小,只有2种,值为2则是16位,值为4则是32位
        @param[in] ucount 在缓存中索引的数量.
        @param[in] mode 一般情况使用 Buffer::M_DEVWRITE, 如果读取则应包含 Buffer::M_HOSTREAD
		@param[in] multi 双缓存则为2，离线混合渲染则更高
        @param[in] pool 是否存在于数据池中的某一连续数据空间,0值则不属于任何一个池
        @version NIIEngine 3.0.0
        */
        virtual void create(IndexBuffer *& out, NCount usize, NCount ucount, BufferModeMark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0) = 0;
        
        /** 删除索引缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(IndexBuffer * obj);
        
        /** 创建着色程序计数缓存
        @param[in] size 大小
        @param[in] mode 一般情况使用 Buffer::M_DEVWRITE, 如果读取则应包含 Buffer::M_HOSTREAD
		@param[in] multi 双缓存则为2，离线混合渲染则更高
        @param[in] pool 是否存在于数据池中的某一连续数据空间,0值则不属于任何一个池
        @version NIIEngine 3.0.0
        */
        virtual void create(CounterBuffer *& out, NCount size, BufferModeMark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0) = 0;

        /** 删除着色程序计数缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(CounterBuffer * obj);
        
        /** 创建绘制命令缓存
        @param[in] size 大小
        @param[in] mode 一般情况使用 Buffer::M_DEVWRITE, 如果读取则应包含 Buffer::M_HOSTREAD
		@param[in] multi 双缓存则为2，离线混合渲染则更高
        @param[in] pool 是否存在于数据池中的某一连续数据空间,0值则不属于任何一个池
        @version NIIEngine 3.0.0
        */
        virtual void create(IndirectBuffer *& out, IndirectBuffer::Type type, NCount size, BufferModeMark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0) = 0;
        
        /** 删除绘制命令缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(IndirectBuffer * obj);

        /** 创建着色程序数据结构体缓存
        @param[in] size 大小(单位:字节)
        @param[in] mode 一般情况使用 Buffer::M_DEVWRITE, 如果读取则应包含 Buffer::M_HOSTREAD
		@param[in] multi 双缓存则为2，离线混合渲染则更高
        @param[in] pool 是否存在于数据池中的某一连续数据空间,0值则不属于任何一个池
        @version NIIEngine 3.0.0
        */
        virtual void create(StructBuffer *& out, NCount size, BufferModeMark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0) = 0;

        /** 删除着色程序数据结构体缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(StructBuffer * obj);

        /** 创建纹理缓存(可用于着色程序)
		@param[in] multi 双缓存则为2，离线混合渲染则更高
        @version NIIEngine 3.0.0
        */
        virtual void create(TextureBuffer *& out, PixelFormat pf, NCount size, BufferModeMark mode, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0) = 0;

        /** 删除纹理缓存(可用于着色程序)
        @version NIIEngine 3.0.0
        */
        virtual void destroy(TextureBuffer * obj);

        /** 创建着色程序缓存
        @param[in] usize 单元的大小(单位:字节)
        @param[in] ucount 单元数量.
		@param[in] multi 双缓存则为2，离线混合渲染则更高
        @version NIIEngine 3.0.0
        */
        virtual void create(StorageBuffer *& out, NCount usize, NCount ucount, NCount multi = 1, void * initData = 0, GpuGroupID pool = 0) = 0;

        /** 删除着色程序缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(StorageBuffer * obj);

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
        
        /** 创建索引数据
        @param[out] out 创建出来的对象
        @version NIIEngine 3.0.0
        */
        virtual void create(IndirectData *& out, IndirectBuffer::Type);

        /** 删除索引数据
        @version NIIEngine 3.0.0
        */
        virtual void destroy(IndirectData * obj);
        
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
        
        /** 获取最大顶点成分
        @version NIIEngine 
        */
        inline NCount getMaxVertexAttribs() const                       { return mMaxVertexAttribs; }

        /** 
        @version NIIEngine 5.0.0
        */
        inline NCount getAlignment(GpuBufferType type) const            { return mAlignment[type]; }
        
        /** 
        @version NIIEngine 5.0.0
        */
        inline NCount getMaxSize(GpuBufferType type) const              { return mMaxSize[type]; }

        /**
        @version NIIEngine 5.0.0
        */
        inline Timer * getTimer()                                       { return mTimer; }

        /**
        @version NIIEngine 5.0.0
        */
        inline uint32 getFrameCount() const                             { return mFrameCount; }

        /**
        @version NIIEngine 6.0.0
        */
        virtual bool isFrameFinished(NCount frameCount) = 0;
    protected:
        void destroyRecover(Nui32 frameMark);
        
        void destroyAllRecover();
    public:
        /** 回收
        @version NIIEngine 高级api
        */
        virtual void onRecover(StrategyType type, FrameID frame, QueueID qid);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(VertexBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(IndexBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(StructBuffer * obj);
        
        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(IndirectBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(TextureBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(StorageBuffer * obj);
        
        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(CounterBuffer * obj);

        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(GeometryPixel * obj);
        
        /**
        @version NIIEngine 5.0.0
        */
        void onUnRef(MappedBuffer * buf);
        
        /**
        @version NIIEngine 5.0.0
        */
        void onRef(MappedBuffer * buf);
    protected:
        N_mutex(mVertexBuffersMutex)
        N_mutex(mIndexBuffersMutex)
        N_mutex(mCounterBuffersMutex)
        N_mutex(mStructBuffersMutex)
        N_mutex(mIndirectBuffersMutex)
        N_mutex(mTextureBuffersMutex)
        N_mutex(mShaderBuffersMutex)
        N_mutex(mGeoPixelListMutex)
        N_mutex(mVertexDataListMutex)
        N_mutex(mVAOListMutex)
    protected:
        /** 创建顶点数据实现
        @version NIIEngine 高级api
        */
        virtual void createImpl(VertexData *& out) = 0;

        /** 删除顶点数据实现
        @version NIIEngine 高级api
        */
        virtual void destroyImpl(VertexData * obj) = 0;
        
        /** 创建映射缓存
        @version NIIEngine 高级api
        */
        virtual void createImpl(MappedBuffer *& buf, NCount size, BufferModeMark mode) = 0;

        /** 创建实现
        @version NIIEngine 高级api
        */
        virtual bool createPoolImpl(GpuGroupID gid, BufferModeMark mode, StrategyType stype, NCount size) = 0;

        /** 删除实现
        @version NIIEngine 高级api
        */
        virtual void destroyPoolImpl(GpuGroupID gid) = 0;
    protected:
        /**
        @remark 实际帧达到了理想帧说明任务已经完成了
        */
        struct Recover
        {
            GpuBuffer * mBuffer;
            Nui32 mFrame;       //理想帧
            Nui32 mFrameMark;   //实际帧

            Recover(GpuBuffer * buffer, Nui32 frameCount, Nui32 frameMark) :
                mBuffer(buffer), mFrame(frameCount), mFrameMark(frameMark) {}
        };

        typedef vector<Recover>::type RecoverList;
        typedef set<IndexBuffer *>::type IndexBufferList;
        typedef set<VertexBuffer *>::type VertexBufferList;
        typedef set<CounterBuffer *>::type CounterBufferList;
        typedef set<StructBuffer *>::type StructBufferList;
        typedef set<IndirectBuffer *>::type IndirectBufferList;
        typedef set<TextureBuffer *>::type TextureBufferList;
        typedef set<StorageBuffer *>::type StorageBufferList;
        typedef set<GeometryPixel *>::type GeometryPixelList;
        typedef set<VertexData *>::type VertexDataList;
        typedef set<GeometryRaw *>::type VAOList;
        typedef vector<GpuGroupID>::type GpuGroupList;
    protected:
        Timer * mTimer;
        VAOList mVAOList;
        VertexDataList mVertexDataList;
        IndexBufferList mIndexBuffers;
        VertexBufferList mVertexBuffers;
        CounterBufferList mCounterBufferList;
        StructBufferList mStructBufferList;
        IndirectBufferList mIndirectBufferList;
        TextureBufferList mTextureBufferList;
        StorageBufferList mStorageBufferList;
        GeometryPixelList mGeoPixelList;
        MappedBufferList mRefMappedList;
        MappedBufferList mFreeMappedList;
        RecoverList mRecoverList;
        GpuGroupList mGpuGroupList;
        NCount mPoolDefaultSize;
        NCount mAlignment[GBT_Count];
        NCount mMaxSize[GBT_Count];
        NCount mMaxVertexAttribs;
        Nui32 mFrameCount;
        Nui32 mValidVAO;
        bool mForcePoolGroup;
        Nui64 mNextCheck;
    };
}
#endif