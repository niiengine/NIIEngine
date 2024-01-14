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
#include "NiiEventArgs.h"
#include "NiiSerialItem.h"
#include "NiiJoyDevEffect.h"
#include "NiiCommandObj.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace NII_MEDIA
{
    enum PovType
    {
        PT_Center    = 0,
        PT_North     = 0x01,
        PT_South     = 0x02,
        PT_East      = 0x04,
        PT_NorthEast = 0x05,
        PT_SouthEast = 0x06,
        PT_West      = 0x08,
        PT_NorthWest = 0x09,
        PT_SouthWest = 0x0A
    };
    
    enum JoyDevControlType
    {
        JDCT_Button,
        JDCT_Slider,
        JDCT_Pov,
        JDCT_Vector,
        JDCT_Axis,
        JDCT_Wheel
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevControlArgs : public EventArgs
    {
    public:
        JoyDevControlArgs(JoyDevControlType t) : mType(t) {}
        virtual ~JoyDevControlArgs() {}
        
        JoyDevControlType mType;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyHandleControlVectorArgs : public JoyDevControlArgs
    {
    public:
        JoyHandleControlVectorArgs(NIIi v, NIIf x, NIIf y, NIIf z) :
            JoyDevControlArgs(JDCT_Vector),
            mVector(v),
            mX(x),
            mY(y),
            mZ(z) {}

        NIIi mVector;   ///< vector索引
        NIIf mX;        ///< x坐标
        NIIf mY;        ///< y坐标
        NIIf mZ;        ///< z坐标
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlAxisArgs : public JoyDevControlArgs
    {
    public:
        JoyPadControlAxisArgs(Nui8 a, NIIi rel) :
            JoyDevControlArgs(JDCT_Axis),
            mAxis(a),
            mRel(rel) {}

        Nui8 mAxis;                         ///< 摇杆的索引
        NIIi mRel;                          ///< 摇动的数量
    };

    /** 手柄控制参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlButtonArgs : public JoyDevControlArgs
    {
    public:
        JoyPadControlButtonArgs(Nui8 b, bool state) :
            JoyDevControlArgs(JDCT_Button),
            mButton(b),
            mState(state){}

        Nui8 mButton;                       ///< 按键的索引
        bool mState;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlPovArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlPovArgs(Nui8 pov, PovType dir) :
            JoyDevControlArgs(JDCT_Pov),
            mPov(pov),
            mDirection(dir) {}

        Nui8 mPov;                      ///< 索引
        PovType mDirection;             ///< pov方向
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlSliderArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlSliderArgs(Nui8 s, const Vector2<NIIi> & data) :
            JoyDevControlArgs(JDCT_Slider),
            mSlider(s),
            mData(data) {}

        const Vector2<NIIi> & mData;        ///< 横纵轴数据
        Nui8 mSlider;                       ///< slider索引
    };

    /** 摇轮
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyWheelControlArgs : public JoyDevControlArgs
    {
    public:
        JoyWheelControlArgs(NIIf w) :
            JoyDevControlArgs(JDCT_Wheel),
            mWheel(w) {}

        NIIf mWheel;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevSerialItemArgs : public SerialItemArgs
    {
    public:
        JoyDevSerialItemArgs(JoyDevControlType t) :
            mType(t) {}
        virtual ~JoyDevSerialItemArgs() {}
        
        JoyDevControlType mType;   ///< 类型
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadSerialButtonItemArgs : public JoyDevSerialItemArgs
    {
    public:
        JoyPadSerialButtonItemArgs(Nui8 b);

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const;

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const;

        Nui8 mButton;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickSerialPovItemArgs : public JoyDevSerialItemArgs
    {
    public:
        JoyStickSerialPovItemArgs(Nui8 pov, PovType dir);

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * o) const;

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * o) const;

        Nui8 mPov;                      ///< 
        PovType mDirection;             ///< pov方向
    };
    
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
        /** 当摇杆中的普通键触发
        @param[in] args 摇杆按键事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onButton(const JoyPadControlButtonArgs * args) {}

        /** 当摇杆中的摇杆摇动时触发
        @param[in] args 摇杆杆事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onTwistAxis(const JoyPadControlAxisArgs * args) {}

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
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButtonEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButtonEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID TwistAxisEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SlipSliderEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PushPOVEvent;

        static const EventID EventCount;
    public:
        /// @copydetails JoyControlItem::onPressButton
        virtual void onButton(const JoyPadControlButtonArgs * args);

        /// @copydetails JoyControlItem::onTwistAxis
        virtual void onTwistAxis(const JoyPadControlAxisArgs * args);

        /// @copydetails SignalJoyPadControlItem::onSlipSlider
        virtual void onSlipSlider(const JoyStickControlSliderArgs * args);

        /// @copydetails SignalJoyPadControlItem::onPushPOV
        virtual void onPushPOV(const JoyStickControlPovArgs * args);
    };

    /** 游戏手把类型控制器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyHandleControlItem : public JoyPadControlItem
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

    /** 游戏手柄类控制器
    @remark 这也仅仅是描述标准手柄,不包含飞机遥杆概念
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
        typedef vector<PovType>::type Povs;
        typedef vector<Vector2<NIIi> >::type Sliders;
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

        /** 设置按键数量
        @remark 这个方法内部使用
        @param[in] num 按键数量
        @version NIIEngine 3.0.0
        */
        void setButtonCount(NCount num);

        /** 获取按键数量
        @version NIIEngine 3.0.0
        */
        inline NCount getButtonCount() const            { return mButtonCount; }

        /** 设置方向摇杆数量
        @remark 这个方法内部使用
        @param[in] num 摇杆数量
        @version NIIEngine 3.0.0
        */
        void setAxisCount(NCount num);

        /** 获取方向摇杆数量
        @version NIIEngine 3.0.0
        */
        inline NCount getAxisCount() const              { return mAxisBuffer.size(); }

        /** 设置按纽摇杆数量
        @version NIIEngine 3.0.0
        */
        void setPovCount(NCount num);

        /** 获取按纽摇帽数量
        @version NIIEngine 3.0.0
        */
        inline NCount getPovCount() const               { return mPovBuffer.size(); }

        /** 设置划块数量
        @version NIIEngine 3.0.0
        */
        void setSliderCount(NCount num);

        /** 获取划块数量
        @version NIIEngine 3.0.0
        */
        inline NCount getSliderCount() const            { return mSliderBuffer.size(); }

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        virtual ControlDevType getType() const;
        
        static const NIIi MIN_AXIS;
        static const NIIi MAX_AXIS;
    protected:
        Buttons mButtonBuffer;          ///< 按件缓存
        NCount mButtonCount;            ///< 按件数量
        Axes mAxisBuffer;               ///< 摇杆缓存
        Povs mPovBuffer;                ///< 摇杆键缓存
        Sliders mSliderBuffer;          ///< 划块缓存
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

    /** 游戏手把类型控制器
    @remark 包含手势类游戏把概念
    @note
        由于大多系统内核读取手把记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI JoyHandleControlPattern : public JoyPadControlPattern
    {
    public:
        virtual ~JoyHandleControlPattern();

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    protected:
        JoyHandleControlPattern();

        JoyHandleControlPattern(Nid id, const ViewWindow * own);

        void handle(NIIi v, NIIf x, NIIf y, NIIf z);
    };

    /** 傀儡控制器
    @version NIIEngine 3.0.0
    */
    class DummyJoyHandleControlPattern : public JoyHandleControlPattern
    {
    public:
        DummyJoyHandleControlPattern() {}

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

    /** 游戏摇杆类控制器
    @remark 包含飞机摇杆概念
    @note
        由于大多系统内核读取摇杆记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI JoyWheelControlPattern : public JoyPadControlPattern
    {
    protected:
        JoyWheelControlPattern();

        JoyWheelControlPattern(Nid id, const ViewWindow * own);

        virtual ~JoyWheelControlPattern();

        /** 滚动摇盘
        @remark 这个方法引发划块事件
        @param[in] num
        */
        void wheel(NIIf num);

        /** 滚动摇盘
        @remark 这个方法引滚动摇盘事件,使用最近一次设置的数据缓存
        */
        void wheel();

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        NIIf mWheel;                    ///< 摇盘值
        JoyWheelControlItem * mTarget;    ///< 当前对焦的控制单元
    };

    /** 傀儡控制器
    @remark 用于对象完整性，或全局性
    @version NIIEngine 3.0.0
    */
    class DummyJoyWheelControlPattern : public JoyWheelControlPattern
    {
    public:
        DummyJoyWheelControlPattern() {}

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