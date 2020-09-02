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

#ifndef _NII_VERTEXBUFFER_H_
#define _NII_VERTEXBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiBuffer.h"
#include "NiiPixelFormat.h"
#include "NiiPixelBlock.h"
#include "NiiGpuProgram.h"

namespace NII
{
    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI GpuBuffer : public Buffer
    {
    public:
        enum LockType
        {
            LT_Unlock,
            UT_Lock
        };

        enum UnlockType
        {
            LT_UnlockAll,
            UT_KeepMap
        };
    public:
        GpuBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, Nmark mode);

        using Buffer::unlock;
        /**
        @version NIIEngine 5.0.0
        */
        void unlock(UnlockType type, NCount oft, NCount size);

        /**
        @version NIIEngine 5.0.0
        */
        void bindProgram(Nui16 slot, GpuProgram::ShaderType type);
    };

    /** GPU AGP / SYS 三角形顶点索引缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IndexBuffer : public GpuBuffer
    {
    public:
        enum IndexRenderType
        {
            IRT_Normal = 1,         ///< 普通索引
            IRT_LeftView = 2,       ///< 模型左视图(相对概念/切线空间)
            IRT_RightView = 3,      ///< 模型右视图(相对概念/切线空间)
            IRT_TopView = 4,        ///< 模型俯视图(相对概念/切线空间)
            IRT_Plane = 5           ///< 单一平行面
        };
    public:
        IndexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, Nmark mode);
        ~IndexBuffer();

        /// @copydetails Buffer::clone()
        Buffer * clone(Nmark m = M_WRITE | M_WHOLE | M_HOST) const;
    };

    /** AGP/GPU/Sys 三角形顶点缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexBuffer : public GpuBuffer
    {
    public:
        VertexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, Nmark mode);

        ~VertexBuffer();

        /** 同一渲染通路中多次应用绘制
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */
        void setInstancing(bool s);

        /** 同一渲染通路中多次应用绘制
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */
        bool isInstancing() const;
        
        /** 同一渲染通路中多次应用绘制数据偏移
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */ 
        inline void setInstancingOffset(NCount val){ mInstancingOffset = val; }
        
        /** 同一渲染通路中多次应用绘制数据偏移
        @remark 可以优化APG缓存应用,并非所有的渲染器都支持,d3d9+,openl3.0+才支持
        @version NIIEngine 3.0.0
        */
        inline NCount getInstancingOffset() const{ return mInstancingOffset; }
        
        /// @copydetails Buffer::clone
        Buffer * clone(Nmark m = M_WRITE | M_WHOLE | M_HOST) const;
    protected:
        NCount mInstancingOffset;
        
    };
    
    /** 着色程序中的计数缓存
    @note openl4.2才支持
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CounterBuffer : public Buffer
    {
    public:
        CounterBuffer(BufferManager * mag, NCount size, Nmark mode);
        ~CounterBuffer();
        
        /// @copydetails Buffer::clone()
        Buffer * clone(Nmark m = M_WRITE | M_WHOLE | M_HOST) const;
    };
    
    /** 片块缓存
    @version NIIEngine 4.0.0
    */
    class _EngineAPI UniformBuffer : public Buffer
    {
    public:
        UniformBuffer(BufferManager * mag, NCount size, Nmark mode);
        ~UniformBuffer();

        /// @copydetails Buffer::clone
        Buffer * clone(Nmark m = M_WRITE | M_WHOLE | M_HOST) const;
    };
    
    /** 帧缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameBuffer : public Buffer
    {
        friend class TextureFrame;
    public:
        FrameBuffer(BufferManager * mag, NCount w, NCount h, NCount d, PixelFormat pf, Nmark mode);
        ~FrameBuffer();

        using Buffer::lock;

        /** 锁定的缓寸
        @param[in] area 锁定缓存的区域
        @param[in] m 锁定选项
        @version NIIEngine 3.0.0
        */
        virtual const PixelBlock & lock(const Box & area, Nmark mm);

        /** 获取当前锁定的像素块
        @version NIIEngine 3.0.0
        */
        const PixelBlock & getMutexData();

        /** 从另外帧缓存复制
        @version NIIEngine 3.0.0
        */
        void write(const FrameBuffer * src);

        /** 从另外帧缓存中复制某个区域到这个帧缓存的某个区域
        @note 自动完成区域缩放
        @version NIIEngine 3.0.0
         */
        virtual void write(const FrameBuffer * target, const Box & src, const Box & dst);

        /** 写入像素缓存块
        @param[in] target 目标区域
        @version NIIEngine 3.0.0
        */
        virtual void write(const PixelBlock & src, const Box & target) = 0;

        /** 读取像素缓存块
        @param[in] target 包含在内存的目的像素和格式
        @version NIIEngine 3.0.0
        */
        virtual void read(const Box & src, PixelBlock & target) = 0;

        /** 获取可供使用的纹理帧
        @version NIIEngine 3.0.0
        */
        virtual TextureFrame * getBuffer(Nidx index = 0);

        /** 获取这个缓存的宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const { return mWidth; }

        /** 获取这个缓存的高度
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const { return mHeight; }

        /** 获取这个缓存的深度
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const { return mDepth; }

        /** 获取这个缓存的原像素格式
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const { return mFormat; }

        /// @copydetails Buffer::lock
        virtual void * lock(Nmark m, NCount oft, NCount size);

        /// @ copydetails Buffer::read
        virtual void read(void * dest, NCount oft, NCount size);

        /// @ copydetails Buffer::write
        virtual void write(const void * src, NCount oft, NCount size);
    protected:
        /** 锁定缓存
        @version NIIEngine 3.0.0
        */
        virtual PixelBlock lockImpl(Nmark m, const Box & src) = 0;

        /** 清理缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroyImpl(TextureFrame * obj, Nidx index);

        /// @copydetails Buffer::lockImpl
        virtual void * lockImpl(Nmark m, NCount oft, NCount size);
    protected:
        NCount mWidth;
        NCount mHeight;
        NCount mDepth;
        NCount mRowPitch;
        NCount mSlicePitch;
        PixelFormat mFormat;
        Box mMutexArea;
        PixelBlock mMutexData;
    };

    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI ConstGpuBuffer : public GpuBuffer
    {
    public:
        ConstGpuBuffer(BufferManager * mag, NCount size, Nmark mode);
    };

    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI IndirectGpuBuffer : public GpuBuffer
    {
    public:
        IndirectGpuBuffer(BufferManager * mag, NCount size, Nmark mode);
    };

    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI TextureGpuBuffer : public GpuBuffer
    {
    public:
        TextureGpuBuffer(BufferManager * mag, NCount size, Nmark mode);
    };

    /**
    @version NIIEngine 5.0.0
    */
    class _EngineAPI UavGpuBuffer : public GpuBuffer
    {
    public:
        UavGpuBuffer(BufferManager * mag, NCount size, Nmark mode);
    };
}
#endif