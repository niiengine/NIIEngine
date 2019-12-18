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

#ifndef NII_JOYSTICK_CONTROLPATTERN_H
#define NII_JOYSTICK_CONTROLPATTERN_H

#include "NiiPreInclude.h"
#include "NiiJoyPadControlPattern.h"
#include "NiiPovDirection.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 游戏摇杆类控制器
    @remark 包含飞机摇杆概念
    @note 
        由于大多系统内核读取摇杆记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI JoyStickControlPattern : public JoyPadControlPattern
    {
    public:
        typedef vector<PovType>::type Povs;
        typedef vector<Vector2<NIIi> >::type Sliders;
    protected:
        JoyStickControlPattern();

        JoyStickControlPattern(Nid id, const ViewWindow * own);

        virtual ~JoyStickControlPattern();

        /** 滑动划块
        @remark 这个方法引发划块事件
        @param[in] slider 划块索引
        @param[in] x 横轴方向数值
        @param[in] y 纵轴方向数值
        */
        void slip(Nui8 slider, NIIi x, NIIi y);

        /** 滑动划块
        @remark 这个方法引发划块事件,使用最近一次设置的数据缓存
        @param[in] slider 划块索引
        */
        void slip(Nui8 slider);

        /** 推动摇杆键
        @remark 这个方法引发摇杆键事件
        @param[in] pov 摇杆键索引
        @param[in] dir 方向类型
        */
        void push(Nui8 pov, PovType dir);

        /** 推动摇杆键
        @remark 这个方法引发摇杆键事件,使用最近一次设置的数据缓存
        @param[in] pov 摇杆键索引
        */
        void push(Nui32 pov);

        /// 设置按纽摇杆数量
        void setPovCount(NCount num);

        /// 获取按纽摇帽数量
        NCount getPovCount() const;

        /// 设置划块数量
        void setSliderCount(NCount num);

        /// 获取划块数量
        NCount getSliderCount() const;

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        Povs mPovBuffer;                ///< 摇杆键缓存
        Sliders mSliderBuffer;          ///< 划块缓存
        JoyStickControlItem * mTarget;  ///< 当前对焦的控制单元
    };

    /** 傀儡控制器
    @remark 用于对象完整性，或全局性
    @version NIIEngine 3.0.0
    */
    class DummyJoyStickControlPattern : public JoyStickControlPattern
    {
    public:
        DummyJoyStickControlPattern() {}
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