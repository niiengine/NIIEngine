/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-11-7

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

#ifndef _NII_RenderFrameListener_H_
#define _NII_RenderFrameListener_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
    /** 帧参数
    @version NIIEngine 3.0.0
    */
    class RenderFrameArgs : public EventArgs
    {
    public:
        RenderFrameArgs();
        virtual ~RenderFrameArgs();

        /** 帧标准
        @note 一般是 14 毫秒
        @version NIIEngine 3.0.0
        */
        TimeDurMS mStandard;

        /** 一帧需要的时间
        @version NIIEngine 3.0.0
        */
        TimeDurMS mActual;

        /** 当前和上次需要的时间
        @note 应该不大于 mActual
        @version NIIEngine 3.0.0
        */
        TimeDurMS mCurrent;
    };

    /** 定义一个可以接收帧事件通知的监听.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderFrameListener
    {
        friend class Engine;
    public:
		RenderFrameListener();
        virtual ~RenderFrameListener();

        /** 当一帧开始渲染时调用.
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onPreRender(const RenderFrameArgs * e);

        /** 渲染主模型区域时
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onRenderCore(const RenderFrameArgs * e);

        /** 渲染天空时
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onRenderSky(const RenderFrameArgs * e);

		/** 渲染地形时
		@return 返回false 将会终止当前渲染循环机制
		@version NIIEngine 3.0.0
		*/
		virtual bool onRenderTerrain(const RenderFrameArgs * e);

        /** 渲染图形界面时
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onRenderUI(const RenderFrameArgs * e);

        /** 当一帧已经渲染时调用.
        @remark
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onEndRender(const RenderFrameArgs * e);
    protected:
        bool mEnable;
    };
}
#endif