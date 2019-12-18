/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-17

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

#ifndef _NII_Render_Listener_H_
#define _NII_Render_Listener_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 渲染监听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderListener
    {
    public:
        virtual ~RenderListener() {}

        /** 渲染前触发
        @version NIIEngine 3.0.0
        */
        virtual void renderBegin() {}

        /** 渲染队列前触发
        @param[in] render 渲染组
        @return 返回true代表渲染这个渲染组,否则不渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderBegin(GroupID gid)
        { 
            return true;
        }

        /** 渲染队列后触发
        @param[in] render 渲染组
        @return 返回true代表重复渲染这个渲染组,否则不重复渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderEnd(GroupID gid)
        { 
            return false;
        }
        
        /** 渲染后触发
        @version NIIEngine 3.0.0
        */
        virtual void renderEnd() {}
    };
}
#endif