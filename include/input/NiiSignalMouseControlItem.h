/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-24

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

#ifndef _NII_SignalMOUSE_CONTROLITEM_H_
#define _NII_SignalMOUSE_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiMouseControlItem.h"
#include "NiiCommandObj.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace NII_MEDIA
{
    /** 鼠标控制单元
    @remark 游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多
        的处理性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalMouseControlItem : public MouseControlItem, public CommandObj
    {
    public:
        SignalMouseControlItem();

        virtual ~SignalMouseControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMiddleEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMiddleEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEvent;
    public:
        /// @copydetails MouseControlItem::onPressLeft
        virtual void onPressLeft(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseLeft
        virtual void onReleaseLeft(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressRight
        virtual void onPressRight(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseRight
        virtual void onReleaseRight(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressMiddle
        virtual void onPressMiddle(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseMiddle
        virtual void onReleaseMiddle(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton3
        virtual void onPressButton3(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton3
        virtual void onReleaseButton3(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton4
        virtual void onPressButton4(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton4
        virtual void onReleaseButton4(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton5
        virtual void onPressButton5(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton5
        virtual void onReleaseButton5(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton6
        virtual void onPressButton6(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton6
        virtual void onReleaseButton6(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton7
        virtual void onPressButton7(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton7
        virtual void onReleaseButton7(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MouseMoveArgs * arg);
    };
}
}
#endif