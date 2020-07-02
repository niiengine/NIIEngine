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

#ifndef _NII_JOYPAD_CONTROL_PATTERN_H_
#define _NII_JOYPAD_CONTROL_PATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiJoyDevEffect.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 游戏手柄类控制器
    @remark 这也仅仅是描述手柄,不包含飞机遥杆概念
    @note 
        由于大多系统内核读取手柄记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI JoyPadControlPattern : public ControlPattern
    {
    public:
        typedef vector<Nui32>::type Buttons;
        typedef vector<Ni32>::type Axes;
    public:
        /** 应用控制效果
        @remark 同步设置
        @param[in] eff 摇杆效果
        @version NIIEngine 3.0.0
        */
        virtual void apply(JoyStickEffect * eff);

        /** 指定按键是否被按下
        @remark 非固定按键,不像键盘按键在系统中有具体的意义和扫描码
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isPress(Nui8 button);

        /** 指定按键是否已释放
        @remark 非固定按键,不像键盘按键在系统中有具体的意义和扫描码
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isRelease(Nui8 button);
    protected:
        /** 默认构造函数
        @remark 使用protected类型,防止滥用
        */
        JoyPadControlPattern();

        /** 默认构造函数
        @remark 使用protected类型,防止滥用
        @param[in] id
            控制器的唯一标识,所有控制器中唯一(包含同类型,和不同类型的概念)
        @param[in] own 这个控制属于哪个窗体的
        */
        JoyPadControlPattern(Nid id, const ViewWindow * own);

        virtual ~JoyPadControlPattern();

        /** 按下手柄按键
        @remark 这个方法引发按键按下事件
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        void press(Nui8 button);

        /** 松开手柄按键
        @remark 这个方法引发按键释放事件
        @param[in] button 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        void release(Nui8 button);

        /** 推动手柄方向轴
        @remark 这个方法引发轴事件
        @param[in] axis 轴(使用标识)
        @param[in] data 轴推动的数据
        @version NIIEngine 3.0.0
        */
        void twist(Nui8 axis, Ni32 data);

        /** 推动手柄方向轴
        @remark 这个方法引发轴事件,使用最近一次设置的数据缓存
        @param[in] axis 轴(使用标识)
        @version NIIEngine 3.0.0
        */
        void twist(Nui8 axis);

        /** 设置按键数量
        @remark 这个方法内部使用
        @param[in] num 按键数量
        @version NIIEngine 3.0.0
        */
        void setButtonCount(NCount num);

        /** 获取按键数量
        @version NIIEngine 3.0.0
        */
        NCount getButtonCount() const;

        /** 设置方向摇杆数量
        @remark 这个方法内部使用
        @param[in] num 摇杆数量
        @version NIIEngine 3.0.0
        */
        void setAxisCount(NCount num);

        /** 获取方向摇杆数量
        @version NIIEngine 3.0.0
        */
        NCount getAxisCount() const;

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        virtual ControlDevType getType() const;
        
        static const NIIi MIN_AXIS;
        static const NIIi MAX_AXIS;
    protected:
        Buttons mButtonBuffer;            ///< 按件缓存
        NCount mButtonCount;            ///< 按件数量
        Axes mAxisBuffer;                ///< 摇杆缓存
        JoyPadControlItem * mTarget;    ///< 当前对焦的控制单元
    };

    /** 傀儡控制器
    @version NIIEngine 3.0.0
    */
    class DummyJoyPadControlPattern : public JoyPadControlPattern
    {
    public:
        DummyJoyPadControlPattern() {}

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