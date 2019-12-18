/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-23

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

#ifndef _NII_RESOURCE_LISTENER_H_
#define _NII_RESOURCE_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 资源监听
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI ResourceListener
    {
    public:
        ResourceListener();
        virtual ~ResourceListener();

        /** 资源加载策略部署后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPlanEnd(Resource * r);

        /** 当加载完后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onLoadEnd(Resource * r);

        /** 资源转换策略完成后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onResultEnd(Resource * r);

        /** 当资源卸载完成后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onUnloadEnd(Resource * r);

        /** 当加载策略启动时触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onLoadScheme(Resource * r, LoadScheme * s, bool enable);

        /** 当结果策略启动时触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onResultScheme(Resource * r, ResultScheme * s, bool enable);
    };
}

#endif