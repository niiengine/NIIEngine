/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
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
        inline bool isPress(MouseButton button) const { return (mBuffer & button) != 0; }

        /** 指定鼠标按键是否释放
        @remark 实时状态
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        inline bool isRelease(MouseButton button) const { return (mBuffer & button) == 0;  }

        /** 获取当前鼠标X坐标
        @remark 相对于渲染窗口(设备器窗口)
        @version NIIEngine 3.0.0
        */
        inline Nui32 getX() const { return mX; }

        /** 获取当前鼠标Y坐标
        @remark 相对于渲染窗口(设备器窗口)
        @version NIIEngine 3.0.0
        */
        inline Nui32 getY() const { return mY; }
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
        Nui32 mX;                   ///< 当前鼠标X坐标(相对于渲染窗口)
        Nui32 mY;                   ///< 当前鼠标Y坐标(相对于渲染窗口)
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