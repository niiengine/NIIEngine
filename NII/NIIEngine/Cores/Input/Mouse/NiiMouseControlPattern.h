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

    /** ����¼�����
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

        const MouseControlPattern * mSrc;   ///< ������Դ
        Ni32 mX;                            ///< x�����ƶ�����
        Ni32 mY;                            ///< y�����ƶ�����
        Ni32 mScroll;                       ///< wheel��������
        MouseButton mButton;                ///< ��������
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
        inline void setGain(NIIf factor)        { mFactor = factor; }

        /** ��ȡ��굥Ԫ����������
        @remark ʹ������������Լӿ�����ƶ�
        @par ���Ӽ���,������ϵ
        @version NIIEngine 3.0.0
        */
        inline NIIf getGain() const             { return mFactor; }

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    public:
        /** ��������������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onLeft(const MouseArgs * arg);

        /** ������Ҽ�������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onRight(const MouseArgs * arg);

        /** ������м�������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onMiddle(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onButton3(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onButton4(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onButton5(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onButton6(const MouseArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onButton7(const MouseArgs * arg);

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
        virtual void onButton(const MouseArgs * arg);
    protected:
        NIIf mFactor;                        ///< ��������
    };

    /** �������
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

        /// ��ȡ�������ļ����ַ���ascii��
        inline bool isButtonPress() const { return mButtonPress; }

        /// ��ȡ�������ļ����ַ���unicode��
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