/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_AligneAlloc_H_
#define _NII_AligneAlloc_H_

namespace NII
{
    /** 对齐内存分配的SIMD功能
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI AlignedMemory
    {
    public:
        /** 给定对齐,分配内存
        @param[in] s 需要分配的内存大小
        @param[in] align 字节对齐数,必须为2次方数,且范围[1, 64]内
        @version NIIEngine 3.0.0 高级api
        */
        static N_DECLMALLOC void * alloc(size_t s, size_t align);

        /** 对齐分配内存
        @param[in] s 分配的内存大小
        @version NIIEngine 3.0.0 高级api
        */
        static N_DECLMALLOC void * alloc(size_t s);

        /** 重新分配内存空间,包含对齐
        @param[in] p 需要重新分配的指针 
        @param[in] os 上次已经分配的大小
        @param[in] ns 重新分配的大小
        @param[in] nalign 新字节对齐数
        @version NIIEngine 3.0.0 高级api
        */
        static void * realloc(void * p, size_t os, size_t ns, size_t nalign);

        /** 从新分配内存控件
        @param[in] p 需要重新分配的地址
        @param[in] os 上次已经分配的大小
        @param[in] ns 重新分配的大小
        @version NIIEngine 3.0.0 高级api
        */
        static void * realloc(void * p, size_t os, size_t ns);

        /** 释放分配的内存
        @param[in] p 释放的地址
        @version NIIEngine 3.0.0 高级api
        */
        static void dealloc(void * p);
    };
}
#endif
