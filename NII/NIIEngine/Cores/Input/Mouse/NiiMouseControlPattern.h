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
#include "NiiEventArgs.h"
#include "NiiSerialItem.h"
#include "NiiCommandObj.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace NII_MEDIA
{
    enum MouseButton
    {
        MB_Left = 0,
        MB_Right = 1,
        MB_Middle = 2,
        MB_Button3 = 3,
        MB_Button4 = 4,
        MB_Button5 = 5,
        MB_Button6 = 6,
        MB_Button7 = 7,
        MB_Unknow = 8
    };

    /** 鼠标事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseArgs : public EventArgs
    {
    public:
        MouseArgs();
        MouseArgs(const MouseArgs * o);
        MouseArgs(const MouseControlPattern * src);
        MouseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll, MouseButton b, bool state);
        MouseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll);
        MouseArgs(const MouseControlPattern * src, MouseButton b, bool state);

        const MouseControlPattern * mSrc;   ///< 参数来源
        Ni32 mX;                            ///< x坐标移动数量
        Ni32 mY;                            ///< y坐标移动数量
        Ni32 mScroll;                       ///< wheel滚动数量
        MouseButton mButton;                ///< 按键编码
        bool mState;
    };

    typedef void(MouseControlItem::*MouseMethod)(const MouseArgs *);
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MousePressSerialItemArgs : public SerialItemArgs
    {
    public:
        MousePressSerialItemArgs(MouseButton b) :
            mButton(b) {}

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const 
        {
            MouseButton src = static_cast<const MousePressArgs *>(o)->mButton;
            if (mButton == src)
                return true;
            return false;
        }

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const 
        {
            MouseButton src = static_cast<const MousePressArgs *>(o)->mButton;
            if (mButton == src)
                return false;
            return true;
        }

        MouseButton mButton;            ///<
    };

    /** 鼠标控制单元
    @remark 游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多
        的处理性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlItem : public ControlItem
    {
        friend class MouseControlPattern;
    public:
        MouseControlItem();

        virtual ~MouseControlItem();

        /** 设置鼠标单元的增量因子
        @remark 使用这个函数可以加快鼠标移动
        @par 因子级别,倍数关系
        @version NIIEngine 3.0.0
        */
        inline void setGain(NIIf factor)        { mFactor = factor; }

        /** 获取鼠标单元的增量因子
        @remark 使用这个函数可以加快鼠标移动
        @par 因子级别,倍数关系
        @version NIIEngine 3.0.0
        */
        inline NIIf getGain() const             { return mFactor; }

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    public:
        /** 当鼠标左键被按下时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onLeft(const MouseArgs * arg);

        /** 当鼠标右键被按下时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onRight(const MouseArgs * arg);

        /** 当鼠标中键被按下时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onMiddle(const MouseArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onButton3(const MouseArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onButton4(const MouseArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onButton5(const MouseArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onButton6(const MouseArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onButton7(const MouseArgs * arg);

        /** 当鼠标移动的时候触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onMove(const MouseArgs * arg);
    protected:
        /** 初始化映射表
        @remark 至少调用一次
        */
        static void init();

        /** 当鼠标按键按下时触发
        @param[in] arg
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onButton(const MouseArgs * arg);
    protected:
        NIIf mFactor;                        ///< 加速因子
    };

    /** 虚拟鼠标
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DummyMouseControlItem : public MouseControlItem, public ControlAlloc
    {
    protected:
        /// @copydetails MouseControlItem::onButton
        void onButton(const MouseArgs *) {/*do nothing*/ }
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerMouseControlItem : public MouseControlItem, public ControlAlloc
    {
    public:
        ListenerMouseControlItem();
        virtual ~ListenerMouseControlItem();

        /// 获取监听到的键盘字符的ascii码
        inline bool isButtonPress() const { return mButtonPress; }

        /// 获取监听到的键盘字符的unicode码
        inline MouseButton getButton() const { return mButton; }

        inline const String & getVerbose() const { return mDescrip; }
    public:
        /// @copydetails MouseControlItem::onLeft
        virtual void onLeft(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onRight
        virtual void onRight(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onMiddle
        virtual void onMiddle(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton3
        virtual void onButton3(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton4
        virtual void onButton4(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton5
        virtual void onButton5(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton6
        virtual void onButton6(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton7
        virtual void onButton7(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MouseArgs * arg);
    protected:
        MouseButton mButton;
        Ni32 mX;
        Ni32 mY;
        Ni32 mScroll;
        String mDescrip;
        bool mButtonPress;
    };

    /** 事件级鼠标控制单元
    @remark 游戏对象可以灵活的转换控制单元从而不同的效果,用于描述操作逻辑复杂的事件级对象,
        用于顶层设计中也可以方便事件连接.
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
        static const EventID LeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID RightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MiddleEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Button3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Button4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Button5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Button6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Button7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEvent;
    public:
        /// @copydetails MouseControlItem::onLeft
        virtual void onLeft(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onRight
        virtual void onRight(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onMiddle
        virtual void onMiddle(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton3
        virtual void onButton3(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton4
        virtual void onButton4(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton5
        virtual void onButton5(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton6
        virtual void onButton6(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onButton7
        virtual void onButton7(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MouseArgs * arg);
    };

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