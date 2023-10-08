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
    /** 操作结果
    @version NIIEngine 5.0.0
    */
    enum OperationReason
    {
        OR_OK,          ///< 完成
        OR_ERROR,       ///< 错误
        OR_BLOCK        ///< 阻塞
    };

    /** 系统资源硬件缓存辅助操作类
    @version NIIEngine 5.0.0
    */
    class _EngineGLAPI BufferOp : public BufferAlloc
    {
        friend class Buffer;
        friend class BufferManager;
        friend class ABufferManager;
        friend class GBufferManager;
    public:
        BufferOp(Buffer * buf);
        ~BufferOp();
    protected:
        /** 初始数据实现
        @version NIIEngine 5.0.0
        */
        virtual void initImpl(void * data, NCount oft, NCount size) = 0；

        /** 映射缓存实现
        @version NIIEngine 5.0.0
        */
        virtual void * lockImpl(MuteMark m, NCount oft, NCount size) = 0;

        /** 解除映射缓存实现
        @version NIIEngine 5.0.0
        */
        virtual void unlockImpl(UnlockType type, NCount oft, NCount size)= 0;

        /** 预分配缓存实现
        @version NIIEngine 5.0.0
        */
        virtual bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true)= 0;

        /** 复制缓存实现
        @version NIIEngine 5.0.0
        */
        virtual void memcpyImpl(void * data, NCount oft, NCount size, bool write = true) = 0;

        /** 复制缓存实现
        @version NIIEngine 5.0.0
        */
        virtual void memcpyImpl(Buffer * target, NCount targetOft, NCount oft, NCount size)= 0;

        /** 等待缓存操作完成实现
        @version NIIEngine 5.0.0
        */
        virtual void waitImpl() = 0;
    protected:
        Buffer * mBuffer;
    };

    typedef Nmark BufferModeMark;

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
        friend class GBufferManager;
        friend class BufferOp;
    public:
        /// 缓存特征
        enum Mode
        {
            /** cpu读取快,cpu写入快,gpu读取中,gpu写入慢(客服端)
            @version NIIEngine 3.0.0
            */
            M_HOST          = 0x01,

            /** gpu读取快,gpu写入快,如果没有M_HOST标识,cpu不能直接读写操作,
                但可通过其他指令操作,如使用(着色程序，加速程序，或内部复制内存API)(服务端)
            @version NIIEngine 3.0.0
            */
            M_DEV           = 0x02,
            
            /** 拥有客服端读取功能
            @version NIIEngine 3.0.0
            */
            M_HOSTREAD      = M_HOST | 0x04,
            
            /** 拥有客服端写入功能
            @version NIIEngine 3.0.0
            */
            M_HOSTWRITE     = M_HOST | 0x08,
            
            /** 拥有设备读取功能(服务端)(默认)
            @version NIIEngine 3.0.0
            */
            M_DEVREAD       = M_DEV | 0x10,
            
            /** 拥有设备写入功能(服务端)
            @version NIIEngine 3.0.0
            */
            M_DEVWRITE      = M_DEV | 0x20,
            
            /** 拥有设备读取功能/写入功能(服务端)
            @version NIIEngine 3.0.0
            */
            M_DEVNORMAL     = M_DEVREAD | M_DEVWRITE,
            
            /** 只读,初始化数据后不可改变数据
            @version NIIEngine 3.0.0
            */
            M_OnlyREAD      = M_DEVREAD | M_HOSTREAD,
            
            /** 通常情况
            @version NIIEngine 3.0.0
            */
            M_Normal        = M_DEVREAD | M_HOSTWRITE,
            
            /** 允许服务端在缓冲被映射的状态下读写数据
            @version NIIEngine 3.0.0
            */
            M_PersistentMap = 0x40,
            
            /** 通常情况 和 允许服务端在缓冲被映射的状态下读写数据
            @version NIIEngine 3.0.0
            */
            M_NormalPersistent = M_Normal | M_PersistentMap,
            
            /** 客户端/服务端更新的数据会在服务端/客户端立即同步
            @version NIIEngine 3.0.0
            */
            M_CoherentMap   = 0x80,
            
            /** M_Normal M_PersistentMap M_CoherentMap 特性
            @version NIIEngine 3.0.0
            */
            M_NormalPersistentCoherent = M_Normal | M_PersistentMap | M_CoherentMap,

            /** 访问特征区域
            @version NIIEngine 3.0.0
            */
            M_USAGEMARK     = 0xFF,
            
            /** 每次操作都是整块(整体)的
            @version NIIEngine 3.0.0
            */
            M_WHOLE = 0x100,

            /** 操作时以线程形式
            @version NIIEngine 3.0.0
            */
            M_THREAD = 0x200,

            /** 同步CPU->GPU(APU)缓存
            @version NIIEngine 3.0.0
            */
            M_SYN = 0x400,
            
            /** 实例数据
            @version NIIEngine 3.0.0
            */
            M_INSTANCE  = 0x800,

            /** 每帧冲刷后数据被重写
            @version NIIEngine 3.0.0
            */
            M_FrameRecover      = 0x1000,

            /** 每次队列绘制完后数据被重写
            @version NIIEngine 3.0.0
            */
            M_QueueDrawRecover  = 0x2000,

            /** 每次like-drawcall函数调用后数据被重写
            @version NIIEngine 3.0.0
            */
            M_DrawRecover       = 0x4000,
            
            /** 绘制特征区域
            @version NIIEngine 3.0.0
            */
            M_DrawRecoverMARK   = 0x7000,

            /** 复合缓存,拥有1倍以上的大小,此选项默认2倍大小(double缓存概念).
                可以有更多用在帧缓存混算.
            @version NIIEngine 5.0.0
            */
            M_Multi             = 0x8000,
            
            /** cpu保留副本
            @version NIIEngine 3.0.0
            */
            M_HostCache         = 0x10000,

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
            M_EXT15 = 0x80000000,
            
            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT16 = 0x100000000,
            
            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT17 = 0x200000000,
            
            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT18 = 0x400000000,

            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT19 = 0x800000000,
            
            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT20 = 0x1000000000,
            
            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT20 = 0x2000000000,
            
            /** 扩展标记
            @version NIIEngine 3.0.0
            */
            M_EXT21 = 0x4000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT22 = 0x8000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT23 = 0x10000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT24 = 0x20000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT25 = 0x40000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT26 = 0x80000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT27 = 0x100000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT28 = 0x200000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT29 = 0x400000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT30 = 0x800000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT31 = 0x1000000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT32 = 0x2000000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT33 = 0x4000000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT34 = 0x8000000000000,
            
            /** 扩展标记
            @version NIIEngine 5.0.0
            */
            M_EXT35 = 0x10000000000000
        };
        
        enum OpType
        {
            OT_UNKNOW,
            OT_READ,
            OT_WRITE,
            OT_READWRITE
        };

        /** 锁定特征
        @remark 用于上锁操作
        @version NIIEngine 3.0.0
        */
        enum MuteMode
        {
            MM_OP       = 0x01,         ///< 内部机制(device)级别执行运算/赋值,不通过CPU(Host)级别
            MM_READ     = 0x02,         ///< CPU(Host)级别的操作
            MM_WRITE    = 0x04,         ///< CPU(Host)级别的操作
            MM_WHOLE    = 0x08,         ///< 整块操作,将会把旧数据无效化
            MM_EVENT    = 0x100,        ///< 非手动,而是内部线程机制上锁
            MM_DevREAD  = 0x200 | MM_OP,///< GPU(Dev)级别的操作
            MM_DevWRITE = 0x400 | MM_OP,///< GPU(Dev)级别的操作
            MM_Persistent = 0x800,
            MM_Coherent = 0x1000,
            MM_EXT1     = 0x2000,
            MM_EXT2     = 0x4000,
            MM_EXT3     = 0x8000,
            MM_EXT4     = 0x10000,
            MM_EXT5     = 0x20000,
            MM_EXT6     = 0x40000,
            MM_EXT7     = 0x80000,
            MM_EXT8     = 0x100000,
            MM_EXT9     = 0x200000,
            MM_EXT10    = 0x400000,
            MM_EXT11    = 0x800000,
            MM_EXT12    = 0x1000000,
            MM_EXT13    = 0x2000000,
            MM_EXT14    = 0x4000000,
            MM_EXT15    = 0x8000000,
        };
        typedef Nmark MuteMark;
        
        enum UnlockType
        {
            UT_UnlockMap,
            UT_PersistentMap
        };
    public:
        /** 构造函数
        @param[in] mode Buffer::Mode 选项
        @version NIIEngine 3.0.0
        */
        Buffer(BufferManager * mag, NCount unitsize, NCount unitcnt, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid = 0, NCount goft = 0, NCount padSize = 0);

        virtual ~Buffer();

        /** 获取群组
        @version NIIEngine 5.0.0
        */
        inline GroupID getGroupID() const           { return mGroupID; }

        /** 获取群组中的偏移
        @version NIIEngine 5.0.0
        */
        inline NCount getGroupOffset() const        { return mOffset; }

        /** 获取单元数量
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitCount() const          { return mUnitCount; }

        /** 获取单元大小(单位:字节)
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitSize() const           { return mUnitSize; }

        /** 获取总分配(预留)的单元数量
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitReserveCount() const   { return mReserveSize / mUnitSize; }

        /** 设置是否同步缓存
        @remark 如果缓存本身并没有副本,这个设置没有效果
        @version NIIEngine 3.0.0
        */
        void setSync(bool set);

        /** 设置是否计数自动删除
        @version NIIEngine 3.0.0
        */
        inline void setAutoDestroy(bool set)        { mAutoDestroy = set; }
        
        /** 设置是否计数自动删除
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDestroy() const           { return mAutoDestroy; }

        /** 是否支持特性
        @param[in] mode Buffer::Mode选项
        @version NIIEngine 3.0.0
        */
        inline bool isSupport(Mode m)               { return mMark & m; }

        /** 缓存是否处于锁定状态
        @version NIIEngine 3.0.0
        */
        inline bool isMapping() const               { return mMapping || (mShadow && mShadow->isMapping()); }

        /** 设置复合数量
        @version NIIEngine 5.0.0 高级api
        */
        virtual void setMultiCount(NCount cnt);

        /** 获取复合数量
        @version NIIEngine 5.0.0 高级api
        */
        inline NCount getMultiCount() const         { return mMultiCount; }
        
        /** 设置辅助缓存
        @version NIIEngine 5.0.0 高级api
        */
        inline void setSecondBuffer(Buffer * buff)  { mSecond = buff; }
        
        /** 获取辅助缓存
        @version NIIEngine 5.0.0 高级api
        */
        inline Buffer * getSecondBuffer() const     { return mSecond;}
        
        /** 设置操作类
        @version NIIEngine 5.0.0 高级api
        */
        void setOperation(BufferOp * op);
        
        /** 获取操作类
        @version NIIEngine 5.0.0 高级api
        */
        inline BufferOp * getOperation() const      { return mOp; }
        
        /** 获取CPU存储
        @version NIIEngine 5.0.0 高级api
        */
        virtual Nui8 * getHostData() const;

        /** 锁定整个缓冲区(可)读/写(CPU 操作)
        @remark 尽量使用read/write代替
        @param[in] mm MuteMode单一/MuteMode复合选项
        @param[in] multiIdx 复合缓存选项,默认只有1倍,索引为0.双缓存1则是DEV应用中的.更多的一般用于离线渲染
        @return 指向锁定/映射内存
        @version NIIEngine 3.0.0
        */
        inline void * lock(MuteMark mm, NCount multiIdx = 0){ return lock(mm | M_WHOLE, 0, mSize, multiIdx); }

        /** 锁定的缓冲区(可)读/写.(CPU 操作)
        @remark 尽量使用read/write代替 
        @param[in] mm MuteMode单一/MuteMode复合选项
        @param[in] oft 从锁定缓存开始的字节偏移量
        @param[in] size 锁定面积大小(单位:字节)
        @param[in] multiIdx 复合缓存选项,默认只有1倍,索引为0.双缓存1则是DEV应用中的.更多的一般用于离线渲染
        @return 指向锁定/映射内存
        @version NIIEngine 3.0.0
        */
        virtual void * lock(MuteMark mm, NCount oft, NCount size, NCount multiIdx = 0);

        /** 释放这个缓冲区的锁.(CPU 操作)
        @remark 尽量使用read/write代替 
        @note lock函数操作完后调用
        @version NIIEngine 3.0.0
        */
        virtual void unlock(UnlockType type);
        
        /** 释放这个缓冲区的锁(CPU 操作)
        @remark 尽量使用read/write代替 
        @note lock函数操作完后调用
        @version NIIEngine 5.0.0
        */
        virtual void unlock(UnlockType type, NCount oft, NCount size);

        /** 读取数据到缓冲中
        @param[in] out 内存区域
        @param[in] oft 从这个缓存开始到读取位置的字节偏移量(单位:字节)
        @param[in] size 读取的区域大小(单位:字节)
        @version NIIEngine 3.0.0
        */
        virtual void read(void * out, NCount oft, NCount size);
        
        /** 异步读取
        @version NIIEngine 5.0.0
        */
        virtual OperationReason readAsync(void * out, NCount oft, NCount size);

        /** 读取数据到缓冲中
        @param[in] out 内存区域
        @param[in] unitoft 从这个缓存开始到读取位置的单元偏移量(单位:单元)
        @param[in] unitCnt 读取的区域大小(单位:单元)
        @version NIIEngine 3.0.0
        */
        void readUnit(void * out, NCount unitOft, NCount unitCnt);

        /** 把系统内存写入到缓冲区
        @param[in] in 写入的数据资源
        @param[in] oft 从这个缓存开始到写入位置的偏移量(单位:字节)
        @param[in] size 写入数据的大小(单位:字节)
        @version NIIEngine 3.0.0
        */
        virtual void write(const void * in, NCount oft, NCount size);
        
        /** 异步写入
        @version NIIEngine 5.0.0
        */
        virtual void writeAsync(const void * in, NCount oft, NCount size);

        /** 把系统内存写入到缓冲区
        @param[in] in 写入的数据资源
        @param[in] unitoft 从这个缓存开始到写入位置的单元偏移量(单位:单元)
        @param[in] unitCnt 写入数据的大小(单位:单元)
        @version NIIEngine 3.0.0
        */
        void writeUnit(const void * in, NCount unitOft, NCount unitCnt);

        /** 从指定缓冲区复制数据
        @remark (this)要有 M_DEVWRITE 功能, 来源缓存(src)要有 M_DEVREAD 功能
        @param[in] src 需要读取的数据源缓存
        @param[in] srcoft 数据源到读取的位置的字节偏移量(单位:字节)
        @param[in] oft 从这个缓存开始到写入位置的字节偏移量(单位:字节)
        @param[in] size 复制的数据长度(单位:字节)
        @version NIIEngine 3.0.0
        */
        virtual void write(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /** 从指定缓冲区复制数据
        @remark (this)要有 M_DEVWRITE 功能, 来源缓存(src)要有 M_DEVREAD 功能
        @param[in] src 需要读取的数据源缓存
        @param[in] srcUnitOft 数据源到读取的位置的单元偏移量(单位:单元)
        @param[in] unitOft 从这个缓存开始到写入位置的单元偏移量(单位:单元)
        @param[in] unitCnt 复制的数据长度(单位:单元)
        @version NIIEngine 3.0.0
        */
        void writeUnit(Buffer * src, NCount srcUnitOft, NCount unitOft, NCount unitCnt);

        /** 从指定缓冲区复制所有数据
        @remark (this)缓存要有 M_DEVWRITE 功能, 来源缓存(src)要有 M_DEVREAD 功能
        @note 注意缓存大小
        @version NIIEngine 3.0.0
        */
        inline void write(Buffer * src)     { write(src, 0, 0, std::min(mSize, src->mSize)); }

        /**读取数据到指定缓冲区
        @remark (this)缓存要有 M_DEVREAD 功能, 来源缓存(src)要有 M_WRITE功能
        @note 注意缓存大小
        @version NIIEngine 3.0.0
        */
        void read(Buffer * dst, NCount dstOft, NCount oft, NCount size);

        /**读取数据到指定缓冲区区
        @remark (this)缓存要有 M_DEVREAD 功能, 来源缓存(src)要有 M_WRITE功能
        @note 注意缓存大小
        @version NIIEngine 3.0.0
        */
        void readUnit(Buffer * dst, NCount dstUnitOft, NCount unitOft, NCount unitCnt);

        /** 读取所有数据到指定缓冲区
        @remark (this)缓存要有 M_DEVREAD 功能, 来源缓存(src)要有 M_WRITE功能
        @note 注意缓存大小
        @version NIIEngine 3.0.0
        */
        inline void read(Buffer * dst)      { read(dst, 0, 0, std::min(mSize, dst->mSize)); }

        /** 分配缓冲区大小(预留)
        @param[in] newMode 如果保留原来的设置则值为 -1.
        @version NIIEngine 4.0.0
        */
        bool resize(NCount newSize, BufferModeMark newMode = -1, bool paddingUnit = true, bool keeyOldData = true);

        /** 分配缓冲区大小(预留)
        @param[in] newMode 如果保留原来的设置则值为 -1.
        @version NIIEngine 4.0.0
        */
        bool resizeUnit(NCount newUnitCnt, BufferModeMark newMode = -1, bool paddingUnit = true, bool keeyOldData = true);

        /** 副本对象
        @param[in] m Buffer::Mode 选项
        @version NIIEngine 3.0.0
        */
        virtual Buffer * clone(BufferModeMark m = M_Normal | M_WHOLE) const = 0;
        
        /** 等待所有内部操作完成
        @remark 有些命令在调用线程中立即返回,但内部并没有完成,这个函数阻塞等待直到命令完成
        @version NIIEngine 3.0.0
        */
        virtual void wait();

        /** 正式引用
        @remark 引用数量为0,如果是设置为自动删除,这个缓存将被销毁
        @version NIIEngine 3.0.0
        */
        NCount ref();

        /** 解除引用
        @remark 引用数量为0,如果是设置为自动删除,这个缓存将被销毁
        @version NIIEngine 3.0.0
        */
        NCount unref();

        /** 获取引用数量
        @version NIIEngine 3.0.0
        */
        inline NCount getRefCount()                 { return mRefCount;  }

        /** 返回缓冲区分配字节的大小
        @version NIIEngine 3.0.0
        */
        inline NCount getReserveSize() const        { return mReserveSize; }

        /** 返回缓冲区字节的大小(非预留)
        @version NIIEngine 3.0.0
        */
        inline NCount getSize() const               { return mSize;  }

        /** 返回缓冲创建时使用用法标志
        @version NIIEngine 3.0.0
        */
        inline BufferModeMark getModeMark() const         { return mMark;  }

        /** 管理器
        @version NIIEngine 3.0.0
        */
        inline BufferManager * getManager() const   { return mMag; }
    protected:
        Buffer(){}
        
        /** 同步硬件与系统缓存
        @version NIIEngine 3.0.0 高级api
        */
        virtual void syncImpl();

        /** 加锁实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void * lockImpl(MuteMark m, NCount oft, NCount size);

        /** 解锁实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void unlockImpl(UnlockType type, NCount oft, NCount size);

        /** 预留空间实现
        @remark 新数据块和旧数据块大概率同机制，最好使用机制API复制函数而不使用lock/unlock.
        @param[in] oldData 是否复制原来的数据
        @version NIIEngine 4.0.0 高级api
        */
        virtual bool reserveImpl(NCount size, BufferModeMark newMode = -1, bool oldData = true);

        /** 复制实现
        @remark 最好使用机制API复制函数而不使用lock/unlock.
        @note 如果这个缓存和src属于同个内存控制器产物，直接使用内部函数API,达到优化效果，则不需要有 M_HOSTWRITE M_HOSTREAD 功能约束，
            如果两个缓存非CPU缓存且跨度不同(stride != unitSize)，可以考虑使用着色程序或加速程序去复制
        @version NIIEngine 4.0.0 高级api
        */
        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size);

        /** 复制实现
        @remark 最好使用机制API复制函数而不使用lock/unlock.
        @note 如果这个缓存和src属于同个内存控制器产物，直接使用内部函数API,达到优化效果，则不需要有 M_HOSTWRITE M_HOSTREAD 功能约束，
            如果两个缓存非CPU缓存且跨度不同(stride != unitSize)，可以考虑使用着色程序或加速程序去复制
        @version NIIEngine 4.0.0 高级api
        */
        virtual void memcpyImpl(Buffer * src, NCount srcOft, NCount srcstride, NCount oft, NCount stride, NCount size) {}
        
        /** 等待缓存操作完成实现
        @version NIIEngine 5.0.0
        */
        virtual void waitImpl();
    protected:
        GpuGroupID mGroupID;
        BufferManager * mMag;
        Buffer * mShadow;
        Buffer * mSecond;
        BufferOp * mOp;
        NCount mUnitCount;
        NCount mUnitSize;
        NCount mOffset;         ///< Group 中的某一段
        NCount mSize;
        NCount mReserveSize;    ///< mSize + padSize
        NCount mMuteOft;
        NCount mMuteSize;
        NCount mMuteMulti;
        NCount mRefCount;
        NCount mCurrentMulti;
        NCount mMultiCount;     ///< totle size = mMultiCount * mReserveSize
        BufferModeMark mMark;
        bool mMapping;
        bool mShadowRefresh;
        bool mAutoDestroy;
    };
    typedef vector<Buffer *>::type BufferList;

    inline void Buffer::readUnit(void * out, NCount unitOft, NCount unitCnt)
    {
        N_assert(unitOft % getUnitSize() == 0, "error");
        read(out, unitOft * mUnitSize, unitCnt * mUnitSize);
    }

    inline void Buffer::writeUnit(const void * in, NCount unitOft, NCount unitCnt)
    {
        N_assert(unitOft % getUnitSize() == 0, "error");
        write(in, unitOft * mUnitSize, unitCnt * mUnitSize);
    }

    inline bool Buffer::resizeUnit(NCount newUnitSize, BufferModeMark newMode, bool oldData)
    { 
        return resize(newUnitSize * mUnitSize, newMode, oldData); 
    }

    inline void Buffer::writeUnit(Buffer * src, NCount srcUnitOft, NCount unitOft, NCount unitCnt)
    {
        N_assert(srcUnitOft % src->getUnitSize() == 0, "error");
        N_assert(unitOft % getUnitSize() == 0, "error");
        memcpyImpl(src, srcUnitOft * mUnitSize, unitOft * mUnitSize, unitCnt * mUnitSize);
        
        write(src, srcUnitOft * mUnitSize, unitOft * mUnitSize, unitCnt * mUnitSize)
    }

    inline void Buffer::read(Buffer * dst, NCount dstOft, NCount oft, NCount size)
    {
        dst->write(this, oft, dstOft, size);
    }

    inline void Buffer::readUnit(Buffer * dst, NCount dstUnitOft, NCount unitOft, NCount unitCnt)
    {
        N_assert(dstUnitOft % dst->getUnitSize() == 0, "error");
        N_assert(unitOft % getUnitSize() == 0, "error");
        dst->write(this, unitOft * mUnitSize, dstUnitOft * mUnitSize, unitCnt * mUnitSize);
    }
    
    struct ScopeBuffer
    {
        ScopeBuffer(void * ptr) : mPtr(ptr) {}

        ~ScopeBuffer()
        {
            if(mPtr)
                N_free(mPtr);
        }
    private:
        ScopeBuffer(const ScopeBuffer &);
        ScopeBuffer & operator=(const ScopeBuffer &);
        
        void * mPtr;
    };
}
#endif