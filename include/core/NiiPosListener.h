/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-2

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

#ifndef _NII_PosListener_H_
#define _NII_PosListener_H_

#include "NiiPreProcess.h"

namespace NII
{
    /** 位置节监听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosListener
    {
    public:
        PosListener();
        virtual ~PosListener();

        /** 创建时
        @version NIIEngine 3.0.0
        */
        virtual void create(PosNode * o);

        /** 删除时
        @version NIIEngine 3.0.0
        */
        virtual void destroy(PosNode * o);

        /** 添加子对象时
        @version NIIEngine 3.0.0
        */
        virtual void add(PosNode * obj, PosNode * child);

        /** 移去子对象时
        @version NIIEngine 3.0.0
        */
        virtual void remove(PosNode * o, PosNode * child);

        /** 附加到其他对象时
        @version NIIEngine 3.0.0
        */
        virtual void attach(PosNode * o);

        /** 从其他对象解除时
        @version NIIEngine 3.0.0
        */
        virtual void detach(PosNode * o);

        /** 更新时
        @version NIIEngine 3.0.0
        */
        virtual void update(PosNode * o);
    };
}
#endif