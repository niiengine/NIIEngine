/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-11-20

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

#ifndef _NII_INDEX_DATA_H_
#define _NII_INDEX_DATA_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 顶点索引
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IndexData : public BufferAlloc
    {
    public:
        IndexData();
        ~IndexData();

        /** 克隆
        @param[in] copy 副本
        @param[in] mag 管理器
        @remark 但完成时候,调用者期望 删除和返回 指针
        */
        IndexData * clone(bool copy = true, GBufferManager * mag = 0) const;

        /** 设置是否自动删除缓存
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b);

        /** 获取是否自动删除缓存
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const;

        /** 附加缓存
        @version NIIEngine 3.0.0
        */
        void attach(IndexBuffer * buf);
        
        /** 分离缓存
        @version NIIEngine 3.0.0
        */
        void detach();
        
        /** 是否已经附加缓存
        @version NIIEngine 3.0.0
        */
        bool isAttach() const;
        
        /** 获取附加的缓存
        @version NIIEngine 3.0.0
        */
        IndexBuffer * getAttach() const;
        
        /** 优化索引,使硬件调用更快(ati显卡专用)
        @remark
            仅用于索引数据(完整三角形链表),在三角形带或其他扇形则无意义
        @version NIIEngine 3.0.0
        */
        void optA();

        /** 优化索引,使硬件调用更快(nv显卡专用)
        @remark
            仅用于索引数据(完整三角形链表),在三角形带或其他扇形则无意义
        @version NIIEngine 3.0.0
        */
        void optN();

        /** 优化索引,使硬件调用更快
        @remark
            仅用于索引数据(完整三角形链表),在三角形带或其他扇形则无意义
        @version NIIEngine 3.0.0
        */
        void opt();
    protected:
        IndexData(const IndexData & o);
        IndexData & operator=(const IndexData & o);
    public:
        NCount mOffset;         ///< 在缓存中的偏移
        NCount mCount;          ///< 在缓存中的数量
        IndexBuffer * mBuffer;  ///< 缓存主体
    protected:
        mutable bool mAutoDestroy;
    };
}

#endif