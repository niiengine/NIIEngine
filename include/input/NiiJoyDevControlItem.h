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

#ifndef _NII_JOYPAD_CONTROL_ITEM_H_
#define _NII_JOYPAD_CONTROL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlItem.h"
#include "NiiJoyDevControlArgs.h"
#include "NiiCommandObj.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace NII_MEDIA
{
    /** 手柄控制单元
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlItem : public ControlItem
    {
    public:
        JoyPadControlItem() {}
        virtual ~JoyPadControlItem() {}
        
        ///@copydetails ControlItem::getType
        virtual ControlDevType getType() const 
        {
            return CDT_JoyPad;
        };
    public:
        /** 当摇杆中的普通键按下时触发
        @param[in] args 摇杆按键事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton(const JoyPadControlButtonArgs * args) {}
        
        /** 当摇杆中的普通键按下后释放时触发
        @param[in] args
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton(const JoyPadControlButtonArgs * args) {}
    
        /** 当摇杆中的摇杆摇动时触发
        @param[in] args 摇杆杆事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onTwistAxis(const JoyPadControlAxisArgs * args) {}
    };

    class _EngineAPI DummyJoyPadControlItem : public JoyPadControlItem, public ControlAlloc
    {
    public:
        DummyJoyPadControlItem() {}
    };

    class _EngineAPI ListenerJoyPadControlItem : public JoyPadControlItem, public ControlAlloc
    {
    public:
        ListenerJoyPadControlItem() {}
        virtual ~ListenerJoyPadControlItem() {}
    };

    /** 事件级 JoyPadControlItem
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalJoyPadControlItem : public JoyPadControlItem, public CommandObj
    {
    public:
        SignalJoyPadControlItem();
        virtual ~SignalJoyPadControlItem();
    public:
        static const EventID PressButtonEvent;
        static const EventID ReleaseButtonEvent;
        static const EventID TwistAxisEvent;
        static const EventID EventCount;
    public:
        /// @copydetails JoyControlItem::onPressButton
        virtual void onPressButton(const JoyPadControlButtonArgs * args);

        /// @copydetails JoyControlItem::onReleaseButton
        virtual void onReleaseButton(const JoyPadControlButtonArgs * args);

        /// @copydetails JoyControlItem::onTwistAxis
        virtual void onTwistAxis(const JoyPadControlAxisArgs * args);
    };
    
    /** 摇杆控制单元
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlItem : public JoyPadControlItem
    {
    public:
        JoyStickControlItem() {}
        virtual ~JoyStickControlItem() {}

        ///@copydetails ControlItem::getType
        ControlDevType getType() const
        {
            return CDT_JoyStick;
        }
    public:
        /** 当摇杆中的滑块推动时触发
        @param[in] args 摇杆按键事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onSlipSlider(const JoyStickControlSliderArgs * args) {}

        /** 当摇杆中的方向键按下时触发
        @param[in] args 摇杆杆上按键事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPushPOV(const JoyStickControlPovArgs * args) {}
    };

    class _EngineAPI DummyJoyStickControlItem : public JoyStickControlItem, public ControlAlloc
    {
    public:
        DummyJoyStickControlItem() {}
    };

    class _EngineAPI ListenerJoyStickControlItem : public JoyStickControlItem, public ControlAlloc
    {
    public:
        ListenerJoyStickControlItem() {}
        virtual ~ListenerJoyStickControlItem() {}
    };

    /** 事件级 JoyStickControlItem
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalJoyStickControlItem : public JoyStickControlItem,
        public SignalJoyPadControlItem
    {
    public:
        SignalJoyStickControlItem();
        virtual ~SignalJoyStickControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SlipSliderEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PushPOVEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /// @copydetails SignalJoyPadControlItem::onSlipSlider
        virtual void onSlipSlider(const JoyStickControlSliderArgs * args);

        /// @copydetails SignalJoyPadControlItem::onPushPOV
        virtual void onPushPOV(const JoyStickControlPovArgs * args);
    };
    
    /** 游戏手把类型控制器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyHandleControlItem : public JoyStickControlItem
    {
    public:
        JoyHandleControlItem() {}
        virtual ~JoyHandleControlItem() {}
        
        ///@copydetails ControlItem::getType
        ControlDevType getType() const
        {
            return CDT_JoyHandle;
        }
    public:
        /** 当摇杆中的向量输出时触发
        @param[in] args 摇杆向量输出事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onHandleVector(const JoyHandleControlVectorArgs * args) {}
    };

    class _EngineAPI DummyJoyHandleControlItem : public JoyHandleControlItem, public ControlAlloc
    {
    public:
        DummyJoyHandleControlItem() {}
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerJoyHandleControlItem : public JoyHandleControlItem, public ControlAlloc
    {
    public:
        ListenerJoyHandleControlItem() {};
        virtual ~ListenerJoyHandleControlItem() {};
    };

    /** 游戏手把类型控制器
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalJoyHandleControlItem : public JoyHandleControlItem,
        public SignalJoyPadControlItem
    {
    public:
        SignalJoyHandleControlItem();
        virtual ~SignalJoyHandleControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID HandleVectorEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /** 当摇杆中的向量输出时触发
        @param[in] args 摇杆向量输出事件参数
        */
        virtual void onHandleVector(const JoyHandleControlVectorArgs * args);
    };
    
    /** 摇杆控制单元
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyWheelControlItem : public JoyPadControlItem
    {
    public:
        JoyWheelControlItem() {}
        virtual ~JoyWheelControlItem() {}

        ///@copydetails ControlItem::getType
        ControlDevType getType() const
        {
            return CDT_JoyWheel;
        }
    public:
        /** 当摇杆中的滑块推动时触发
        @param[in] args 摇杆按键事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onWheel(const JoyWheelControlArgs * args) {}
    };

    class _EngineAPI DummyJoyWheelControlItem : public JoyWheelControlItem, public ControlAlloc
    {
    public:
        DummyJoyWheelControlItem() {}
    };

    class _EngineAPI ListenerJoyWheelControlItem : public JoyWheelControlItem, public ControlAlloc
    {
    public:
        ListenerJoyWheelControlItem() {}
        virtual ~ListenerJoyWheelControlItem() {}
    };

    /** 事件级 JoyWheelControlItem
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalJoyWheelControlItem : public JoyWheelControlItem,
        public SignalJoyPadControlItem
    {
    public:
        SignalJoyWheelControlItem();
        virtual ~SignalJoyWheelControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID WheelEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /// @copydetails SignalJoyWheelControlItem::onSlipSlider
        virtual void onWheel(const JoyWheelControlArgs * args);
    };
}
}
#endif