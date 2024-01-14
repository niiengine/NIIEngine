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

#ifndef _NII_UI_BUTTON_H_
#define _NII_UI_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** 按钮型UI单元对象
    @remark 按钮等事物
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Button : public Widget
    {
    public:
        Button(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
            ScriptTypeID stid = N_CmdObj_Button, LangID lid = N_PrimaryLang);

        virtual ~Button();

        /** 设置是否处于按下状态
        @version NIIEngine 3.0.0
        */
        void press(bool b);

        /** 设置是否处于选中状态
        @version NIIEngine 3.0.0
        */
        virtual void select(bool b);

        /** 获取是否处于选中状态
        @version NIIEngine 3.0.0
        */
        bool isSelect() const               { return mSelect; }
        
        /** 游标是否在这个UI单元对象上方
        @version NIIEngine 3.0.0
        */
        bool isHover() const                { return mOver; }

        /** 这个UI单元对象是否处于被按下状态
        @version NIIEngine 3.0.0
        */
        bool isPress() const                { return mPress; }
    protected:
        /// @copydetails Button::onSelect
        virtual void onSelect(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorLeave
        virtual void onCursorLeave(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    public:
        /** 选中事件
        @version NIIEngine 3.0.0
        */
        static const EventID SelectChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        ///@copydetails Widget::int;
        bool initCmd(PropertyCmdSet * dest);

        /** 更新UI单元对象
        @param pos UI体系中的位
        */
        void update(const Vector2f & cursorpos);
    protected:
        bool mPress;    ///< 是否处于被按下状态
        bool mOver;     ///< 游标覆盖状态下
        bool mSelect;
    };

    /** 默认按纽
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ButtonView : public WidgetView
    {
    public:
        ButtonView(WidgetViewlID wsid);

        /// @copydetails WidgetView::flush
        virtual void flush();

        virtual StateID alter(StateID pid) const;
    public:
        static const StateID PushState;
        static const StateID PushOffState;
        static const StateID StateCount;
    };
    
    /** 可按按钮
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PushButton : public Button
    {
    public:
        PushButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_PushButton, LangID lid = N_PrimaryLang);

        virtual ~PushButton();
    public:
        /** 按下事件
        @version NIIEngine 3.0.0
        */
        static const EventID PushEvent;
        
        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /** 按下时触发
        @version NIIEngine 3.0.0
        */
        virtual void onPush(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);
    };
    
    /** 记录按纽
    @remark 记录位置状态的按纽
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosButton : public PushButton
    {
    public:
        PosButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~PosButton();

        /** 设置是否同步位置事件
        @remark 
        @version NIIEngine 3.0.0
        */
        void setPosEvent(bool b)            { mSynEvent = b; }

        /** 获取是否同步位置事件
        @version NIIEngine 3.0.0
        */
        bool isPosEvent() const             { return mSynEvent; }

        /** 设置y方向移动
        @version NIIEngine 3.0.0
        */
        void setYMove(bool b)               { mYMove = b; }

        /** 获取y方向移动
        @version NIIEngine 3.0.0
        */
        bool isYMove() const                { return mYMove; }

        /** 设置x方向移动
        @version NIIEngine 3.0.0
        */
        void setXMove(bool b)               { mXMove = b; }

        /** 获取x方向移动
        @version NIIEngine 3.0.0
        */
        bool isXMove() const                { return mXMove; }

        /** 设置y方向范围
        @version NIIEngine 3.0.0
        */
        void setYRange(NIIf min, NIIf max);

        /** 获取y方向范围
        @version NIIEngine 3.0.0
        */
        void getYRange(NIIf & min, NIIf & max) const;

        /** 设置x方向范围
        @version NIIEngine 3.0.0
        */
        void setXRange(NIIf min, NIIf max);

        /** 获取x方向范围
        @version NIIEngine 3.0.0
        */
        void getXRange(NIIf & min, NIIf & max) const;
    protected:
        /** 这个UI对象开始拖拽时触发
        @remark
            达成拖拽条件一般是指游标在这个UI对象的位置区域里,而且按下触发拖拽的按键
        @version NIIEngine 3.0.0
        */
        virtual void onMoveBegin(const WidgetEventArgs * arg);

        /** 这个UI对象结束拖拽时触发
        @remark
            达成拖拽条件后,又达成了拖拽结束的条件后触发,一般是指释放了游标的按键
        @version NIIEngine 3.0.0
        */
        virtual void onMoveEnd(const WidgetEventArgs * arg);

        /** 这个UI对象的位置发生变化时触发
        @remark
            依赖相应的是否同步位置变化事件的设计,如果同步则实时触发(存在多次),否则
            是以在每次开始拖拽事件到结束拖拽事件后触发一次.
        @version NIIEngine 3.0.0
        */
        virtual void onPosChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
        /** 移动开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID MoveBeginEvent;

        /** 移动结束事件
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEndEvent;

        /** 位置改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID PosChangeEvent;
        
        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Vector2f mDragPos;
        NIIf mVMin;
        NIIf mVMax;
        NIIf mHMin;
        NIIf mHMax;
        bool mSynEvent;
        bool mDrag;
        bool mYMove;
        bool mXMove;
    };
    
    /** 单选按钮
    @remark 用于描述同一群组中的仅单一选中的按钮群
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RadioButton : public Button
    {
    public:
        RadioButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
            LangID lid = N_PrimaryLang);

        /** 设置所属的群组
        @param[in] gid
        @version NIIEngine 3.0.0
        */
        void setGroup(GroupID gid);

        /** 获取所属的群组
        @version NIIEngine 3.0.0
        */
        GroupID getGroup() const            { return mGID;}

        /** 获取所属的群组中正被选中的单选按钮
        @version NIIEngine 3.0.0
        */
        RadioButton * getGroupSelect() const;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Button::onSelect
        void onSelect(const WidgetEventArgs * arg);
    protected:
        void verify() const;
    protected:
        GroupID mGID;
    };

    /**
    */
    class _EngineAPI RadioButtonView : public ButtonView
    {
    public:
        RadioButtonView(WidgetViewlID wsid);

        static const StateID SelectEnableState;
        static const StateID SelectHoverState;
        static const StateID SelectPushState;
        static const StateID SelectPushOffState;
        static const StateID SelectDisableState;
        static const StateID StateCount;
    protected:
        StateID alter(StateID pid) const;
    };
    
    /** 标签控件专用按纽
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabButton : public Button
    {
    public:
        TabButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~TabButton();

        /** 设置是否处于选中
        version NIIEngine 3.0.0
        */
        virtual void select(bool b);

        /** 设置目标标签容器
        @version NIIEngine 3.0.0
        */
        void setDest(Container * c);

        /** 获取目标标签容器
        @version NIIEngine 3.0.0
        */
        Container * getDest()           { return mDest;}
    protected:
        /** 点击时触发
        @version NIIEngine 3.0.0
        */
        virtual void onClick(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);
    public:
        /** 点击事件
        @version NIIEngine 3.0.0
        */
        static const EventID ClickEvent;

        /** 拖拽事件
        @version NIIEngine 3.0.0
        */
        static const EventID DragEvent;

        /** 滚动事件
        @version NIIEngine 3.0.0
        */
        static const EventID ScrollEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Container * mDest;  ///< 标签容器
        bool mDrag;         ///< 
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabButtonView : public WidgetView
    {
    public:
        TabButtonView(WidgetViewlID wsid);

        /// @copydetails WidgetView::flush
        void flush();

        static const StateID BottomEnableState;
        static const StateID BottomDisableState;
        static const StateID BottomSelectState;
        static const StateID BottomPushState;
        static const StateID BottomHoverState;
        static const StateID TopEnableState;
        static const StateID TopDisableState;
        static const StateID TopSelectState;
        static const StateID TopPushState;
        static const StateID TopHoverState;
        static const StateID StateCount;
    };

    /** 切换按钮
    @remark 用于描述2种状态(on or off)的按钮,并在这2者间切换
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ToggleButton : public Button
    {
    public:
        ToggleButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        /** 设置状态
        @version NIIEngine 3.0.0
        */
        void on(bool b);

        /** 是否处于ON状态
        @version NIIEngine 3.0.0
        */
        bool isOn() const               { return mOn;}
    public:
        /** 状态改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID StateOnEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// event triggered internally when toggle button on state changes.
        virtual void onStateOn(const WidgetEventArgs * arg);
    protected:
        bool mOn;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ToggleButtonView : public ButtonView
    {
    public:
        ToggleButtonView(WidgetViewlID wsid);

        static const StateID SelectEnableState;
        static const StateID SelecteHoverState;
        static const StateID SelectePushState;
        static const StateID SelectePushOffState;
        static const StateID SelecteDisableState;
        static const StateID StateCount;
    protected:
        StateID alter(StateID pid) const;
    };
}
}
#endif