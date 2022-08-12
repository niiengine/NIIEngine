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

    /** ����¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseArgs : public EventArgs
    {
    public:
        MouseArgs();
        MouseArgs(const MouseArgs * o);
        MouseArgs(const MouseControlPattern * src);
        MouseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll, MouseButton b);
        MouseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll);
        MouseArgs(const MouseControlPattern * src, MouseButton b);

        const MouseControlPattern * mSrc;   ///< ������Դ
        Ni32 mX;                            ///< x�����ƶ�����
        Ni32 mY;                            ///< y�����ƶ�����
        Ni32 mScroll;                       ///< wheel��������
        MouseButton mButton;                ///< ��������
    };

    typedef void(MouseControlItem::*MouseMethod)(const MouseArgs *);
    /** �����Ƶ�Ԫ
    @remark ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���
        �Ĵ�����������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlItem : public ControlItem
    {
        friend class MouseControlPattern;
    public:
        MouseControlItem();

        virtual ~MouseControlItem();

        /** ������굥Ԫ����������
        @remark ʹ������������Լӿ�����ƶ�
        @par ���Ӽ���,������ϵ
        @version NIIEngine 3.0.0
        */
        void setGain(NIIf factor);

        /** ��ȡ��굥Ԫ����������
        @remark ʹ������������Լӿ�����ƶ�
        @par ���Ӽ���,������ϵ
        @version NIIEngine 3.0.0
        */
        NIIf getGain() const;

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    public:
        /** ��������������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressLeft(const MouseArgs * arg);

        /** �������������º��ͷ�ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLeft(const MouseArgs * arg);

        /** ������Ҽ�������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressRight(const MouseArgs * arg);

        /** ������Ҽ������º��ͷ�ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRight(const MouseArgs * arg);

        /** ������м�������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressMiddle(const MouseArgs * arg);

        /** ������м������º��ͷ�ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMiddle(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton3(const MouseArgs * arg);

        /** ������м������º��ͷ�ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton3(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton4(const MouseArgs * arg);

        /** ������м������º��ͷ�ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton4(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton5(const MouseArgs * arg);

        /** ������м������º��ͷ�ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton5(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton6(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton6(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton7(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton7(const MouseArgs * arg);

        /** ������ƶ���ʱ�򴥷�
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onMove(const MouseArgs * arg);
    protected:
        /** ��ʼ��ӳ���
        @remark ���ٵ���һ��
        */
        static void init();

        /** ����갴������ʱ����
        @param[in] arg
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onPress(const MouseArgs * arg);

        /** ����갴���ͷ�ʱ����
        @param[in] arg
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onRelease(const MouseArgs * arg);
    protected:
        NIIf mFactor;                        ///< ��������
    };

    /** �������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DummyMouseControlItem : public MouseControlItem, public ControlAlloc
    {
    protected:
        /// @copydetails MouseControlItem::onPress
        void onPress(const MouseArgs *) {/*do nothing*/ }

        /// @copydetails MouseControlItem::onRelease
        void onRelease(const MouseArgs *) {/*do nothing*/ }
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerMouseControlItem : public MouseControlItem, public ControlAlloc
    {
    public:
        ListenerMouseControlItem();
        virtual ~ListenerMouseControlItem();

        /// ��ȡ�������ļ����ַ���ascii��
        inline bool isButtonPress() const { return mButtonPress; }

        /// ��ȡ�������ļ����ַ���unicode��
        inline MouseButton getButton() const { return mButton; }

        inline const String & getVerbose() const { return mDescrip; }
    public:
        /// @copydetails MouseControlItem::onPressLeft
        virtual void onPressLeft(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseLeft
        virtual void onReleaseLeft(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressRight
        virtual void onPressRight(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseRight
        virtual void onReleaseRight(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressMiddle
        virtual void onPressMiddle(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseMiddle
        virtual void onReleaseMiddle(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton3
        virtual void onPressButton3(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton3
        virtual void onReleaseButton3(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton4
        virtual void onPressButton4(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton4
        virtual void onReleaseButton4(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton5
        virtual void onPressButton5(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton5
        virtual void onReleaseButton5(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton6
        virtual void onPressButton6(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton6
        virtual void onReleaseButton6(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton7
        virtual void onPressButton7(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton7
        virtual void onReleaseButton7(const MouseArgs * arg);

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

    /** �¼��������Ƶ�Ԫ
    @remark ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�������������߼����ӵ��¼�������,
        ���ڶ��������Ҳ���Է����¼�����.
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
        static const EventID PressLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMiddleEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMiddleEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEvent;
    public:
        /// @copydetails MouseControlItem::onPressLeft
        virtual void onPressLeft(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseLeft
        virtual void onReleaseLeft(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressRight
        virtual void onPressRight(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseRight
        virtual void onReleaseRight(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressMiddle
        virtual void onPressMiddle(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseMiddle
        virtual void onReleaseMiddle(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton3
        virtual void onPressButton3(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton3
        virtual void onReleaseButton3(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton4
        virtual void onPressButton4(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton4
        virtual void onReleaseButton4(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton5
        virtual void onPressButton5(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton5
        virtual void onReleaseButton5(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton6
        virtual void onPressButton6(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton6
        virtual void onReleaseButton6(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onPressButton7
        virtual void onPressButton7(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton7
        virtual void onReleaseButton7(const MouseArgs * arg);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MouseArgs * arg);
    };

    /** ��������
    @remark ʹ���й��������׼���
    @note 
        ���ڴ��ϵͳ�ں˶�ȡ����¼���Զ��л����,����Ϊ�˸��õ���Ʊ��ģ�鲻Ӧ��
        ��ʹ�ö��л���ģʽ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlPattern : public ControlPattern
    {
    public:
        /** ָ����갴���Ƿ���
        @remark ʵʱ״̬
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        inline bool isPress(MouseButton button) const { return (mBuffer & button) != 0; }

        /** ָ����갴���Ƿ��ͷ�
        @remark ʵʱ״̬
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        inline bool isRelease(MouseButton button) const { return (mBuffer & button) == 0;  }

        /** ��ȡ��ǰ���X����
        @remark �������Ⱦ����(�豸������)
        @version NIIEngine 3.0.0
        */
        inline Nui32 getX() const { return mX; }

        /** ��ȡ��ǰ���Y����
        @remark �������Ⱦ����(�豸������)
        @version NIIEngine 3.0.0
        */
        inline Nui32 getY() const { return mY; }
    protected:
        /** Ĭ�Ϲ��캯��
        @version NIIEngine 3.0.0 �߼�api
        */
        MouseControlPattern();

        /** Ĭ�Ϲ��캯��
        @param[in] id ��������Ψһ��ʶ,���п�������Ψһ(����ͬ����,�Ͳ�ͬ���͵ĸ���)
        @param[in] own ������������ĸ������
        @version NIIEngine 3.0.0 �߼�api
        */
        MouseControlPattern(Nid id, const ViewWindow * own);

        virtual ~MouseControlPattern();

        /** ������갴��
        @remark �������������갴�������¼�
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void press(MouseButton button);

        /** �ɿ���갴��
        @remark �������������갴���ͷ��¼�
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void release(MouseButton button);

        /** �ƶ����
        @remark ���������������ƶ��¼�
        @param[in] x x�����ƶ���
        @param[in] y y�����ƶ���
        @param[in] z (����)�ƶ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void move(Ni32 x, Ni32 y, Ni32 z);

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        Nui32 mWindowSizeX;         ///< �����ΧX��С
        Nui32 mWindowSizeY;         ///< �����ΧY��С
        Nui32 mX;                   ///< ��ǰ���X����(�������Ⱦ����)
        Nui32 mY;                   ///< ��ǰ���Y����(�������Ⱦ����)
        Ni32 mBuffer;               ///< ��������
        MouseControlItem * mTarget; ///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ
    };

    /** ����
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