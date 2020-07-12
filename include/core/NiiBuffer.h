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

#ifndef _NII_BUFFER_H_
#define _NII_BUFFER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 系统资源硬件缓存
    @remark
        硬件缓存(声卡/显卡/网卡/etc)和系统使用的缓存可能有所不同,属于另一个处理体系,
        当想要操作这类缓存的时候,可能并不能如愿的同步的操作,更可能的是把当前系统调用
        进程堵塞,所以在创建选项/操作选项的时候更要注意,但提高空间复杂度优化处理复杂
        度并不是明智的.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Buffer : public BufferAlloc
    {
        friend class BufferManager;
        friend class ABufferManager;
    public:
        /// 缓存特征
        enum Mode
        {
            /** cpu读取快,cpu写入快,gpu读取中,gpu写入慢
            @version NIIEngine 3.0.0
            */
            M_CPU = 0x01,

            /** gpu读取快,gpu写入快,如果没有M_CPU标识,cpu不能操作
            @version NIIEngine 3.0.0
            */
            M_GPU = 0x02,

            /** 拥有读取功能
            @version NIIEngine 3.0.0
            */
            M_READ = 0x04,

            /** 拥有写入功能
            @version NIIEngine 3.0.0
            */
            M_WRITE = 0x08,

            /** 拥有首次写入功能，之后不能再写入
            @version NIIEngine 3.0.0
            */
            M_CONST = 0x10,

            /** 整块(整体)操作
            @version NIIEngine 3.0.0
            */
            M_WHOLE = 0x20,

            /** 操作时以线程形式
            @version NIIEngine 3.0.0
            */
            M_THREAD = 0x40,

            /** 同步CPU->GPU缓存
            @version NIIEngine 3.0.0
            */
            M_SYN = 0x80,
            
            /** 被动标记,不使用
            @version NIIEngine 3.0.0
            */
            M_LOCK = 0x100,            

            /** 实例数据
            @version NIIEngine 3.0.0
            */
            M_INSTANCE = 0x200,

            /** 绑定计数自动删除
            @version NIIEngine 3.0.0
            */
            M_AttachRef_Destroy = 0x400,

            /** 被动标记,不使用
            @version NIIEngine 3.0.0
            */
            M_SHADOW_ASYN = 0x800,

            /** 被动标记,不使用
            @version NIIEngine 3.0.0
            */
            M_SHADOW_Refresh = 0x1000,

            M_Reserve1 =  0x2000,
            
            M_Reserve2 = 0x4000,
            
            M_Reserve3 = 0x8000,
            
            M_Reserve4 = 0x10000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT1 = 0x20000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT2 = 0x40000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT3 = 0x80000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT4 = 0x100000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT5 = 0x200000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT6 = 0x400000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT7 = 0x800000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT8 = 0x1000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT9 = 0x2000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT10 = 0x4000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT11 = 0x8000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT12 = 0x10000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT13 = 0x20000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT14 = 0x40000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT15 = 0x80000000
        };
        /** 锁定特征
        @remark 用于上锁操作
        @version NIIEngine 3.0.0
        */
        enum MuteMode
        {
            MM_COMPUTE  = 0x0,
            MM_READ     = 0x01,
            MM_WRITE    = 0x02,
            MM_BLOCK    = 0x04,
            MM_EVENT    = 0x08
        };
    public:
        /** 构造函数
        @param[in] mode Buffer::Mode 选项
        @version NIIEngine 3.0.0
        */
        Buffer(BufferManager * mag, Nmark mode);

        virtual ~Buffer();

        /** 设置是否同步缓存
        @remark 如果缓存本身并没有副本,这个设置没有效果
        @param[in] set 是否
        @version NIIEngine 3.0.0
        */
        void setSync(bool set);

        /** 设置是否绑定计数自动删除
        @param[in] set 是否
        @version NIIEngine 3.0.0
        */
        void setRefDestroy(bool set);

        /** 是否支持特性
        @param[in] mode Buffer::Mode选项
        @version NIIEngine 3.0.0
        */
        bool isSupport(Mode m);

        /** 缓存是否处于锁定状态
        @version NIIEngine 3.0.0
        */
        bool isMute() const;

        /** 锁定整个缓冲区(可)读/写
        @param[in] mm MuteMode/复合选项
        @return 指向锁定内存
        @version NIIEngine 3.0.0
        */
        void * lock(Nmark mm);

        /** 锁定的缓冲区(可)读/写.
        @param[in] mm MuteMode/复合选项
        @param[in] oft 从锁定缓存开始的字节偏移量
        @param[in] size 锁定面积大小(单位:字节)
        @return 指向锁定内存
        @version NIIEngine 3.0.0
        */
        virtual void * lock(Nmark mm, NCount oft, NCount size);

        /** 释放这个缓冲区的锁.
        @version NIIEngine 3.0.0
        */
        virtual void unlock();

        /** 从缓冲中读取数据
        @param[in] out 内存区域
        @param[in] oft 从缓存开始到开始读取的字节偏移量
        @param[in] size 读取的区域大小 单位字节
        @version NIIEngine 3.0.0
        */
        virtual void read(void * out, NCount oft, NCount size) = 0;

        /** 把系统内存写入到缓冲区
        @param[in] in 写入的数据资源
        @param[in] oft 从缓存开始到开始写入的字节偏移量
        @param[in] size 写入数据的大小,单位字节
        @version NIIEngine 3.0.0
        */
        virtual void write(const void * in, NCount oft, NCount size) = 0;

        /** 从指定缓冲区复制数据
        @remark 来源缓存要有 M_WRITE 功能, 目标缓存要有 M_READ 功能
        @param[in] src 需要读取的数据源缓存
        @param[in] srcoft 数据源读取的位置
        @param[in] oft 这个缓存写入的位置
        @param[in] size 复制的数据长度,单位:字节
        @version NIIEngine 3.0.0
        */
        virtual void write(Buffer * src, NCount srcoft, NCount oft, NCount size);

        /** 从另一个缓冲区复制所有数据
        @remark 来源缓存要有 M_WRITE 功能, 目标缓存要有 M_READ 功能
        @note 注意缓存大小
        @version NIIEngine 3.0.0
        */
        virtual void write(Buffer * src);

        /** 副本对象
        @param[in] m Buffer::Mode 选项
        @version NIIEngine 3.0.0
        */
        virtual Buffer * clone(Nmark m = M_WRITE | M_WHOLE | M_CPU) const = 0;

        /** 正式引用
        @remark 引用数量为0,这个缓存将被销毁
        @version NIIEngine 3.0.0
        */
        NCount touch();

        /** 解除引用
        @remark 引用数量为0,这个缓存将被销毁
        @version NIIEngine 3.0.0
        */
        NCount untouch();

        /** 获取引用数量
        @version NIIEngine 3.0.0
        */
        inline NCount getRefCount() { return mRefCount;  }

        /** 返回此缓冲区字节的大小
        @version NIIEngine 3.0.0
        */
        inline NCount getSize() const { return mSize;  }

        /** 返回缓冲创建时使用用法标志
        @version NIIEngine 3.0.0
        */
        inline Nmark getModeMark() const { return mMark;  }

        /** 管理器
        @version NIIEngine 3.0.0
        */
        inline BufferManager * getManager() const { return mMag; }
    protected:
        /** 同步硬件与系统缓存
        @version NIIEngine 3.0.0 高级api
        */
        virtual void syncImpl();

        /** 加锁实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void * lockImpl(Nmark m, NCount oft, NCount size) = 0;

        /** 解锁实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void unlockImpl() = 0;
    protected:
        BufferManager * mMag;
        Buffer * mShadow;
        NCount mSize;
        NCount mMuteOft;
        NCount mMuteSize;
        NCount mRefCount;
        Nmark mMark;
    };
    typedef vector<Buffer *>::type BufferList;
}
#endif