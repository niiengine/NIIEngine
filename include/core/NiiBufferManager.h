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

#ifndef _NII_BUFFER_MANAGER_H_
#define _NII_BUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiTempBufferCtl.h"
#include "NiiBuffer.h"

namespace NII
{
    class TempBufferCtlPrc;
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
        @param[in] type 类型,对应的实际的 BufferType
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