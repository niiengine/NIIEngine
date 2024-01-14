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
    /** ��ť��UI��Ԫ����
    @remark ��ť������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Button : public Widget
    {
    public:
        Button(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
            ScriptTypeID stid = N_CmdObj_Button, LangID lid = N_PrimaryLang);

        virtual ~Button();

        /** �����Ƿ��ڰ���״̬
        @version NIIEngine 3.0.0
        */
        void press(bool b);

        /** �����Ƿ���ѡ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void select(bool b);

        /** ��ȡ�Ƿ���ѡ��״̬
        @version NIIEngine 3.0.0
        */
        bool isSelect() const               { return mSelect; }
        
        /** �α��Ƿ������UI��Ԫ�����Ϸ�
        @version NIIEngine 3.0.0
        */
        bool isHover() const                { return mOver; }

        /** ���UI��Ԫ�����Ƿ��ڱ�����״̬
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
        /** ѡ���¼�
        @version NIIEngine 3.0.0
        */
        static const EventID SelectChangeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        ///@copydetails Widget::int;
        bool initCmd(PropertyCmdSet * dest);

        /** ����UI��Ԫ����
        @param pos UI��ϵ�е�λ
        */
        void update(const Vector2f & cursorpos);
    protected:
        bool mPress;    ///< �Ƿ��ڱ�����״̬
        bool mOver;     ///< �α긲��״̬��
        bool mSelect;
    };

    /** Ĭ�ϰ�Ŧ
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
    
    /** �ɰ���ť
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PushButton : public Button
    {
    public:
        PushButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_PushButton, LangID lid = N_PrimaryLang);

        virtual ~PushButton();
    public:
        /** �����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PushEvent;
        
        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /** ����ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onPush(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);
    };
    
    /** ��¼��Ŧ
    @remark ��¼λ��״̬�İ�Ŧ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosButton : public PushButton
    {
    public:
        PosButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~PosButton();

        /** �����Ƿ�ͬ��λ���¼�
        @remark 
        @version NIIEngine 3.0.0
        */
        void setPosEvent(bool b)            { mSynEvent = b; }

        /** ��ȡ�Ƿ�ͬ��λ���¼�
        @version NIIEngine 3.0.0
        */
        bool isPosEvent() const             { return mSynEvent; }

        /** ����y�����ƶ�
        @version NIIEngine 3.0.0
        */
        void setYMove(bool b)               { mYMove = b; }

        /** ��ȡy�����ƶ�
        @version NIIEngine 3.0.0
        */
        bool isYMove() const                { return mYMove; }

        /** ����x�����ƶ�
        @version NIIEngine 3.0.0
        */
        void setXMove(bool b)               { mXMove = b; }

        /** ��ȡx�����ƶ�
        @version NIIEngine 3.0.0
        */
        bool isXMove() const                { return mXMove; }

        /** ����y����Χ
        @version NIIEngine 3.0.0
        */
        void setYRange(NIIf min, NIIf max);

        /** ��ȡy����Χ
        @version NIIEngine 3.0.0
        */
        void getYRange(NIIf & min, NIIf & max) const;

        /** ����x����Χ
        @version NIIEngine 3.0.0
        */
        void setXRange(NIIf min, NIIf max);

        /** ��ȡx����Χ
        @version NIIEngine 3.0.0
        */
        void getXRange(NIIf & min, NIIf & max) const;
    protected:
        /** ���UI����ʼ��קʱ����
        @remark
            �����ק����һ����ָ�α������UI�����λ��������,���Ұ��´�����ק�İ���
        @version NIIEngine 3.0.0
        */
        virtual void onMoveBegin(const WidgetEventArgs * arg);

        /** ���UI���������קʱ����
        @remark
            �����ק������,�ִ������ק�����������󴥷�,һ����ָ�ͷ����α�İ���
        @version NIIEngine 3.0.0
        */
        virtual void onMoveEnd(const WidgetEventArgs * arg);

        /** ���UI�����λ�÷����仯ʱ����
        @remark
            ������Ӧ���Ƿ�ͬ��λ�ñ仯�¼������,���ͬ����ʵʱ����(���ڶ��),����
            ������ÿ�ο�ʼ��ק�¼���������ק�¼��󴥷�һ��.
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
        /** �ƶ���ʼ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MoveBeginEvent;

        /** �ƶ������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEndEvent;

        /** λ�øı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PosChangeEvent;
        
        /** �¼�����
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
    
    /** ��ѡ��ť
    @remark ��������ͬһȺ���еĽ���һѡ�еİ�ťȺ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RadioButton : public Button
    {
    public:
        RadioButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
            LangID lid = N_PrimaryLang);

        /** ����������Ⱥ��
        @param[in] gid
        @version NIIEngine 3.0.0
        */
        void setGroup(GroupID gid);

        /** ��ȡ������Ⱥ��
        @version NIIEngine 3.0.0
        */
        GroupID getGroup() const            { return mGID;}

        /** ��ȡ������Ⱥ��������ѡ�еĵ�ѡ��ť
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
    
    /** ��ǩ�ؼ�ר�ð�Ŧ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabButton : public Button
    {
    public:
        TabButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~TabButton();

        /** �����Ƿ���ѡ��
        version NIIEngine 3.0.0
        */
        virtual void select(bool b);

        /** ����Ŀ���ǩ����
        @version NIIEngine 3.0.0
        */
        void setDest(Container * c);

        /** ��ȡĿ���ǩ����
        @version NIIEngine 3.0.0
        */
        Container * getDest()           { return mDest;}
    protected:
        /** ���ʱ����
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
        /** ����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ClickEvent;

        /** ��ק�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DragEvent;

        /** �����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ScrollEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Container * mDest;  ///< ��ǩ����
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

    /** �л���ť
    @remark ��������2��״̬(on or off)�İ�ť,������2�߼��л�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ToggleButton : public Button
    {
    public:
        ToggleButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        /** ����״̬
        @version NIIEngine 3.0.0
        */
        void on(bool b);

        /** �Ƿ���ON״̬
        @version NIIEngine 3.0.0
        */
        bool isOn() const               { return mOn;}
    public:
        /** ״̬�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID StateOnEvent;

        /** �¼�����
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