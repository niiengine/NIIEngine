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
#include "NiiGpuProgram.h"

namespace NII
{
    /** 缓存类型
    @version NIIEngine 3.0.0
    */
    enum GpuBufferType
    {
        GBT_Vertex          = 1,
        GBT_Index           = 2,
        GBT_Counter         = 3,
        GBT_Indirect        = 4,
        GBT_Struct          = 5,
        GBT_Texture         = 6,
        GBT_Storage         = 7,
        GBT_VertexData      = 8,
        GBT_IndexData       = 9,
        GBT_IndirectData    = 10,
        GBT_Count           = 10
    };

    /** 着色程序缓存
    @version NIIEngine 5.0.0
    */
    class _EngineAPI GpuBuffer : public Buffer
    {
    public:
        GpuBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);

        inline void setIdx(Nidx idx)   { mIdx = idx; }
        
        inline Nidx getIdx() const     { return mIdx; }
        
        inline bool operator () (const GpuBuffer & o1, Nidx o2) const
        {
            return o1.mIdx < o2;
        }
        inline bool operator () (Nidx o1, const GpuBuffer & o2) const
        {
            return o1 < o2.mIdx;
        }
        inline bool operator () (const GpuBuffer & o1, const GpuBuffer & o2) const
        {
            return o1.mIdx < o2.mIdx;
        }
        
        inline bool operator () (const GpuBuffer * o1, const GpuBuffer * o2) const
        {
            return o1->mIdx < o2->mIdx;
        }
        
        /** 绑定到着色程序
        @remark slot 就是着色程序中的binding口
        @version NIIEngine 5.0.0
        */
        virtual void bindProgram(GpuProgram::ShaderType type, Nui16 slot, NCount oft, NCount size);
    protected:
        Nidx mIdx;
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
        IndexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        ~IndexBuffer();

        /// @copydetails Buffer::clone
        Buffer * clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const;
        
        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    };

    /** AGP/GPU/Sys 三角形顶点缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexBuffer : public GpuBuffer
    {
    public:
        VertexBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        ~VertexBuffer();
        
        /// @copydetails Buffer::clone
        Buffer * clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const;
        
        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    };
    
    /** 着色程序中的计数缓存
    @remark 着色程序里的可以执行加一减一操作的缓存
    @note openl4.2才支持
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CounterBuffer : public GpuBuffer
    {
    public:
        CounterBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        ~CounterBuffer();
        
        /// @copydetails Buffer::clone()
        Buffer * clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const;

        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    };
    
    /** 结构体缓存
    @remark 着色程序里的静态数据结构体
    @version NIIEngine 4.0.0
    */
    class _EngineAPI StructBuffer : public GpuBuffer
    {
    public:
        StructBuffer(BufferManager * mag, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0, NCount padSize = 0);
        ~StructBuffer();

        /// @copydetails Buffer::clone
        Buffer * clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const;

        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    };
    
    /** 帧缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameBuffer : public Buffer
    {
        friend class TextureFrame;
    public:
        FrameBuffer(BufferManager * mag, NCount w, NCount h, NCount d, PixelFormat pf, NCount multi, BufferModeMark mode, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        ~FrameBuffer();

        using Buffer::lock;

        /** 锁定的缓寸
        @param[in] area 锁定缓存的区域
        @param[in] m 锁定选项
        @version NIIEngine 3.0.0
        */
        virtual const PixelBlock & lock(const Box & area, MuteMark mm);

        /** 获取当前锁定的像素块
        @version NIIEngine 3.0.0
        */
        const PixelBlock & getMutexData() const;

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
        virtual void * lock(MuteMark m, NCount oft, NCount size);

        /// @ copydetails Buffer::read
        virtual void read(void * dest, NCount oft, NCount size);

        /// @ copydetails Buffer::write
        virtual void write(const void * src, NCount oft, NCount size);
        
        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    protected:
        /** 锁定缓存
        @version NIIEngine 3.0.0
        */
        virtual PixelBlock lockImpl(MuteMark m, const Box & src) = 0;

        /** 清理缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroyImpl(TextureFrame * obj, Nidx index);

        /// @copydetails Buffer::lockImpl
        virtual void * lockImpl(MuteMark m, NCount oft, NCount size);
    protected:
        NCount mWidth;
        NCount mHeight;
        NCount mDepth;
        NCount mRowSize;
        NCount mSliceSize;
        PixelFormat mFormat;
        Box mMutexArea;
        PixelBlock mMutexData;
    };

    /** 绘制命令缓存(一般用于批次渲染)
    @version NIIEngine 5.0.0
    */
    class _EngineAPI IndirectBuffer : public GpuBuffer
    {
    public:
        enum Type
        {
            T_Vertex = 16,
            T_Index = 20
        };
    public:
        IndirectBuffer(BufferManager * mag, Type type, NCount size, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        
        inline Type getType() const             { return mType; }
        
        /// @copydetails Buffer::clone
        Buffer * clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const;

        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    protected:
        Type mType;
    };

    /** 纹理缓存
    @remark 一般纹理对象里绑定着纹理缓存,也可以用在顶点着色程序中
    @version NIIEngine 5.0.0
    */
    class _EngineAPI TextureBuffer : public GpuBuffer
    {
    public:
        TextureBuffer(BufferManager * mag, NCount size, PixelFormat pf, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        
        /** 获取纹理格式
        @version NIIEngine 5.0.0
        */
        inline PixelFormat getFormat() const    { return mPixelFormat; }
        
        /// @copydetails Buffer::clone
        Buffer * clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const;
        
        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    protected:
        PixelFormat mPixelFormat;
    };

    /** 着色程序存储缓存(无序/无[数据模型]视图缓存)
    @remark opengl中的ShaderStorage,direct中的uav
    @version NIIEngine 5.0.0
    */
    class _EngineAPI StorageBuffer : public GpuBuffer
    {
    public:
        enum DataViewType
        {
            DVT_Vertex       = 1u << GBT_Vertex,
            DVT_Index        = 1u << GBT_Index,
            DVT_Indirect     = 1u << GBT_Indirect,
            DVT_Struct       = 1u << GBT_Struct,
            DVT_Texture      = 1u << GBT_Texture,
            DVT_Storage      = 1u << GBT_Storage
        };
        typedef BindMark
    public:
        StorageBuffer(BufferManager * mag, NCount unitsize, NCount unitcount, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);
        virtual ~StorageBuffer();
        
        /** 创建索引视图(访问数据模型)
        @param[in] uszie 只有2种,值为2则是16位,值为4则是32位
        @version NIIEngine 5.0.0
        */
        IndexBuffer* createIndexView(NCount usize, NCount oft, NCount count);

        /** 删除索引视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        void destroyIndexView(NCount usize, NCount oft, NCount count);
        
        /** 删除所有索引视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        void destroyAllIndexView();
        
        /** 创建索引视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        IndirectBuffer* createIndirectView(IndirectBuffer::Type type, NCount oft, NCount count);

        /** 删除索引视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        void destroyIndirectView(IndirectBuffer::Type type, NCount oft, NCount count);
        
        /** 删除所有索引视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        void destroyAllIndirectView();

        /** 创建纹理视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        TextureBuffer* createTextureView( PixelFormat pf, NCount oft, NCount count);

        /** 删除纹理视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        void destroyTextureView( PixelFormat pf, NCount oft, NCount count);
        
        /** 删除所有纹理视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        void destroyAllTextureView();

        /** 删除所有视图(访问数据模型)
        @version NIIEngine 5.0.0
        */
        void destroyAllView();
        
        /// @copydetails Buffer::clone
        Buffer * clone(BufferModeMark m = M_WRITE | M_WHOLE | M_HOST) const;
        
        /// @copydetails Buffer::getHostData
        virtual Nui8 * getHostData() const;
    protected:
        virtual TextureBuffer* createViewImpl(PixelFormat pf) = 0;
        virtual TextureBuffer* createIndexImpl(NCount usize) = 0;
        virtual TextureBuffer* createIndirectImpl(IndirectBuffer::Type type) = 0;
    protected:
        TextureBufferList mTextureList;
        IndexBufferList mIndexBuffer;
        IndirectBufferList mIndirectList;
        Nmark mBindFlags;
    };

    /** 映射缓存
    @remark 一般情况读取可以同时进行,写入则会限制读取直到写入完成.(行列锁/读写锁概念)
    @version NIIEngine 5.0.0
    */
    class _EngineAPI MappedBuffer : public BufferAlloc
    {
        friend class GBufferManager;
    public:
        enum BlockType
        {
            BT_None = 0,
            BT_Part = 1,
            BT_All = 2,
            BT_Count = 3
        };
        
        enum BlockOpType
        {
            BOT_Read,
            BOT_Write,
            BOT_Free,
            BOT_Count
        };
        
        struct Operation
        {
            Operation(GpuBuffer * dst, NCount dstoffset, NCount offset, NCount size) :
                mDest(dst),
                mDestOffset(dstoffset),
                mOffset(offset),
                mSize(size)
            {
            }

            GpuBuffer * mDest;
            NCount mDestOffset;
            NCount mOffset;
            NCount mSize;
        };

        typedef vector<Operation>::type OperationList;

        struct Block
        {
            Block(NCount start, NCount end, BlockOpType type) :
                mStart(start),
                mEnd(end),
                mOpType(type),
                mCond(0)
            {
                N_assert(mStart <= mEnd, "error");
            }

            bool overlaps(const Block & block) const
            {
                return !(block.mEnd <= this->mStart || block.mStart >= this->mEnd);
            }

            NCount size(void) const { return mEnd - mStart; }
            
            BlockOpType mOpType;
            ThreadCondition * mCond;        //bilateral constraint
            NCount mStart;
            NCount mEnd;
        };

        typedef vector<Block>::type BlockList;
    public:
        MappedBuffer(GBufferManager * mag, BufferModeMark mode, NCount offset, NCount size);
        virtual ~MappedBuffer();

        /**
        @version NIIEngine 5.0.0
        */
        inline bool getLockState() const    { return mMapping; }

        /**
        @version NIIEngine 5.0.0
        */
        virtual BlockType request(NCount size, bool upload = true);

        /**
        @version NIIEngine 5.0.0
        */
        virtual NCount async(OperationList & oplist, bool upload) = 0;
        
        /**
        @version NIIEngine 5.0.0
        */
        inline void asyncRead(GpuBuffer * dst, NCount dstoffset, NCount offset, NCount size)
        {
            OperationList oplist;
            oplist.emplace_back(dst, dstoffset, offset, size);
            async(oplist, true);
        }
        
        /**
        @version NIIEngine 5.0.0
        */
        inline NCount asyncWrite(GpuBuffer * src, NCount srcoffset, NCount size)
        {
            OperationList oplist;
            oplist.emplace_back(src, srcoffset, 0, size);
            async(oplist, false);
            return oplist.begin()->mOffset;
        }
        
        /**
        @version NIIEngine 5.0.0
        */
        const void * readMap(NCount offset, NCount size);

        /**
        @version NIIEngine 5.0.0
        */
        void * writeMap(NCount size);

        /**
        @version NIIEngine 5.0.0
        */
        void unmap();

        /**
        @version NIIEngine 5.0.0
        */
        inline NCount getMaxSize() const    { return mSizeBytes; }

        /**
        @version NIIEngine 5.0.0
        */
        void ref();

        /**
        @version NIIEngine 5.0.0
        */
        void unref();

        /**
        @version NIIEngine 5.0.0
        */
        inline NCount getRefCount() const   { return mRefCount; }

        /**
        @version NIIEngine 5.0.0
        */
        inline NIIl getLastTime() const     { return mLastTime; }
    protected:
        virtual const void * readMapImpl(NCount offset, NCount size) = 0;

        virtual void * writeMapImpl(NCount size) = 0;

        virtual void unmapImpl() = 0;

        virtual NCount requestImpl(NCount length, bool upload = true);

        virtual void recover(NCount offset, NCount size);
    protected:
        GBufferManager * mMag;
        BufferModeMark mMark;
        BlockList mFreeBlockList;
        BlockOpType mLastOp;
        NCount mOffset;
        NCount mSizeBytes;
        
        NCount mMappingStart;
        NCount mLastUploadOffset;
        NCount mMappingCount;

        NCount mRefCount;
        Nui mLastTime;

        bool mMapping;
    };
    
    /** 
    @version NIIEngine 
    */
    class _EngineAPI MappedTexture : public BufferAlloc
    {
    public:
        MappedTexture(GBufferManager * mag, PixelFormat pf, NCount size, NCount groupOft = 0, NCount groupIdx = 0);
        virtual ~MappedTexture();
        
        /** 群组偏移
        @version NIIEngine 5.0.0
        */
        inline NCount getGroupOffset() const    { return mGroupOffset; }
        
        /** 群组索引
        @version NIIEngine 5.0.0
        */
        inline NCount getGroupIndex() const     { return mGroupIndex; }

        /**
        @version NIIEngine 5.0.0
        */
        virtual bool request(const PixelVolume & vol) const;

        /**
        @version NIIEngine 5.0.0
        */
        inline NCount getSize() const           { return mSize;}

        /**
        @version NIIEngine 5.0.0
        */
        inline PixelFormat getFormat() const    { return mFormat; }

        /**
        @version NIIEngine 5.0.0
        */
        bool isBlock() const;

        /**
        @version NIIEngine 5.0.0
        */
        PixelBlock map(const PixelVolume & vol);

        /**
        @version NIIEngine 5.0.0
        */
        virtual void unmap();

        /** 
        @param[in] host cpu缓存
        @version NIIEngine 5.0.0
        */
        virtual void read(Texture * dst, NCount dstmipmap, const PixelBlock * dstBox, const PixelBlock & box, bool host = false);
        
        /**
        @param[in] host cpu缓存
        @version NIIEngine 5.0.0
        */
        virtual void write(Texture * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host = false);

        /** 
        @version NIIEngine 5.0.0
        */
        inline NCount getLastFrame() const      { return mLastFrame; }
    protected:
        /**
        @version NIIEngine 5.0.0
        */
        virtual PixelBlock mapImpl(const PixelVolume & vol) = 0;
    protected:
        GBufferManager * mMag;
        Nui8 * mMappedPtr;
        PixelFormat mFormat;
        NCount mLastFrame;
        NCount mOffset;
        NCount mSize;
        NCount mCurrentOffset;
        NCount mGroupIndex;
        bool mMapping;
    };
    typedef vector<MappedTexture*>::type MappedTextureList;
}
#endif