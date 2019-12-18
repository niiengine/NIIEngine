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

#ifndef _NII_TOUCH_CONTROLPATTERN_H_
#define _NII_TOUCH_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiTouchControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 触屏控制器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TouchControlPattern : public ControlPattern
    {
    public:
        TouchControlPattern();

        TouchControlPattern(Nid id, const ViewWindow * main);

        virtual ~TouchControlPattern();

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        /// @copydetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        TouchControlItem * mTarget;        ///< 当前对焦的控制单元(本应该在父对象中出现的,但存在性能问题)
    };

    /** 触屏控制器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DummyTouchControlPattern : public TouchControlPattern
    {
    public:
        DummyTouchControlPattern() {}

        ///@copydetails ControlPattern::_init
        bool _init() { return true; }

        ///@copydetails ControlPattern::_end
        bool _end() { return true; }

        ///@copydetails ControlPattern::_update
        void _update() {}

        ///@copydetails KeyboardControlPattern::_reset
        void _reset() {}

        ///@copydetails KeyboardControlPattern::_start
        void _start() {}

        ///@copydetails KeyboardControlPattern::_pause
        void _pause() {}

        ///@copydetails KeyboardControlPattern::_stop
        void _stop() {}
    };
}
}
#endif