/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-10

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

#ifndef _NII_TEMPPRCBUFFER_H_
#define _NII_TEMPPRCBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiTempBufferCtl.h"

namespace NII
{
    /** 临时顶点缓存
    @version NIIEngine 3.0.0
    */
    class _EngineInner AniTempBuffer : public TempBufferCtl, public BufferAlloc
    {
    public:
        AniTempBuffer(bool autoDsy = true);
        ~AniTempBuffer();

        /** 设置映射源
        @param[in] src 源缓存
        @version NIIEngine 3.0.0
        */
        void map(const VertexData * src);

        /** 分配副本
        @version NIIEngine 3.0.0
        */
        void alloc(bool pos = true, bool normal = true);

        /** 续定临时缓存
        @version NIIEngine 3.0.0
        */
        bool refill(bool pos = true, bool normal = true) const;

        /** 绑定副本到指定顶点数据中
        @param[in] dst 指定顶点数据
        @param[in] syn 是否同步前后缓存
        @version NIIEngine 3.0.0
        */
        void bind(VertexData * dst, bool syn);

        /// @copydetails TempBufferCtl::expire
        void expire(Buffer * buffer);
    private:
        VertexBuffer * mSrcPos;     ///< 源头指针(位置)
        VertexBuffer * mSrcNrls;    ///< 源头指针(法线)
        VertexBuffer * mDstPos;     ///< 副本指针(位置)
        VertexBuffer * mDstNrls;    ///< 副本指针(法线)
        Nui16 mPosIndex;
        Nui16 mNrlsIndex;
        Nui8 mMark;
    };
}

#endif