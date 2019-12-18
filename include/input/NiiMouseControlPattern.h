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

#ifndef _NII_MOUSE_CONTROLPATTERN_H_
#define _NII_MOUSE_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiMouseControlItem.h"
#include "NiiMouseControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 鼠标控制器
    @remark 使用有滚块的鼠标标准设计
    @note 
        由于大多系统内核读取鼠标记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlPattern : public ControlPattern
    {
    public:
        /** 指定鼠标按键是否按下
        @remark 实时状态
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isPress(MouseButton button) const;

        /** 指定鼠标按键是否释放
        @remark 实时状态
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isRelease(MouseButton button) const;

        /** 获取当前鼠标X坐标
        @remark 相对于渲染窗口(设备器窗口)
        @version NIIEngine 3.0.0
        */
        Ni32 getX() const;

        /** 获取当前鼠标Y坐标
        @remark 相对于渲染窗口(设备器窗口)
        @version NIIEngine 3.0.0
        */
        Ni32 getY() const;
    protected:
        /** 默认构造函数
        @version NIIEngine 3.0.0 高级api
        */
        MouseControlPattern();

        /** 默认构造函数
        @param[in] id 控制器的唯一标识,所有控制器中唯一(包含同类型,和不同类型的概念)
        @param[in] own 这个控制属于哪个窗体的
        @version NIIEngine 3.0.0 高级api
        */
        MouseControlPattern(Nid id, const ViewWindow * own);

        virtual ~MouseControlPattern();

        /** 按下鼠标按键
        @remark 这个方法引发鼠标按键按下事件
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        void press(MouseButton button);

        /** 松开鼠标按键
        @remark 这个方法引发鼠标按键释放事件
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        void release(MouseButton button);

        /** 移动鼠标
        @remark 这个方法引发鼠标移动事件
        @param[in] x x方向移动量
        @param[in] y y方向移动量
        @param[in] z (滚块)移动量
        @version NIIEngine 3.0.0 高级api
        */
        void move(Ni32 x, Ni32 y, Ni32 z);

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        Nui32 mWindowSizeX;         ///< 鼠标活动范围X大小
        Nui32 mWindowSizeY;         ///< 鼠标活动范围Y大小
        Ni32 mX;                    ///< 当前鼠标X坐标(相对于渲染窗口)
        Ni32 mY;                    ///< 当前鼠标Y坐标(相对于渲染窗口)
        Ni32 mBuffer;               ///< 按键缓存
        MouseControlItem * mTarget; ///< 当前对焦的控制单元
    };

    /** 傀儡
    @version NIIEngine 3.0.0
    */
    class DummyMouseControlPattern : public MouseControlPattern
    {
    public:
        DummyMouseControlPattern() {}
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