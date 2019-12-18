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
#ifndef _NII_TEMP_BUFFER_CTL_H_
#define _NII_TEMP_BUFFER_CTL_H_

#include "NiiPreInclude.h"

namespace NII
{
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
        void setAutoDestroy(bool b);

        /** 获取是否自动删除
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const;

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
}

#endif