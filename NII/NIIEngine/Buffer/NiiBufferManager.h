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

#ifndef _NII_BUFFER_MANAGER_H_
#define _NII_BUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiBuffer.h"

namespace NII
{
    class TempBufferCtlPrc;
    
    /** 临时缓存
    @remark 一般用于非CPU存储外的内存,非CPU存储在创建/释放占用的系统时间多
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI TempBufferCtl
    {
    public:
        /** 操作频繁类型
        @version NIIEngine 3.0.0 高级api
        */
        enum OpType
        {
            T_ReadLess,     ///< 不经常读取
            T_ReadMore,     ///< 经常读取
            T_WriteLess,    ///< 不经常写入
            T_WriteMore     ///< 经常写入
        };
    public:
        TempBufferCtl(bool autoDsy = true);
        virtual ~TempBufferCtl();

        /** 设置是否自动删除
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b)             { mAutoDestroy = b; }

        /** 获取是否自动删除
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const              { return mAutoDestroy; }

        /** 任务的频繁类型
        @note 用于调整缓存存储结构
        @param[in] mark OpType的多个或单个枚举
        */
        virtual void task(Nmark mark);

        /** 执行过期
        @version NIIEngine 3.0.0
        */
        virtual void expire(Buffer * src);
    protected:
        bool mAutoDestroy;
    };
    
    /** 缓存管理器基类
    @note 帧级别的临时缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BufferManager : public BufferAlloc
    {
    public:
        BufferManager();
        virtual ~BufferManager();

        /** 分配临时缓存
        @param[in] src 源缓存指针
        @param[in] ctl 控制器
        @return 临时缓存
        @version NIIEngine 3.0.0
        */
        virtual Buffer * allocTemp(const Buffer * src, TempBufferCtl * ctl);

        /** 分离临时缓存,但不删除临时缓存
        @param[in] temp 临时缓存指针
        @version NIIEngine 3.0.0
        */
        virtual void freeTemp(Buffer * temp);

        /** 分离临时缓存,并且删除临时缓存
        @param[in] src 源缓存指针
        @version NIIEngine 3.0.0
        */
        virtual void recoverTemp(const Buffer * src);

        /** 临时缓存续期
        @note 在特定帧数过去后,如果不续期将自动释放临时缓存,但不删除临时缓存
        @param[in] temp 由源数据分配出来的临时缓存
        @version NIIEngine 3.0.0
        */
        virtual void refillTemp(Buffer * temp);

        /** 释放临时缓存
        @param[in] cost 上次调用和着次调用时差
        @param[in] recover 是否执行回收机制
        */
        virtual void recoverTemp(TimeDurMS cost, bool recover = false);
        
        /** 回收缓存
        @note 即将要删除的缓存
        @version NIIEngine 3.0.0 高级api
        */
        virtual void recover(Buffer * obj);
        
        /** 回收机制
        @remark 删除未使用临时缓存,删除所有回收缓存
        @version NIIEngine 3.0.0 高级api
        */
        virtual void recover();
    private:
        N_mutex(mBufferRecoverMutex)
        N_mutex(mTempBuffersMutex)
    private:
        typedef multimap<const Buffer *, Buffer *>::type CopyBuffers;
        typedef map<Buffer *, TempBufferCtlPrc *>::type TempBuffers;
    private:
        BufferList mRecoverList;
        TempBuffers mTempBuffers;
        CopyBuffers mCopyBuffers;
        NCount mFrameMark;
    };
}

#endif