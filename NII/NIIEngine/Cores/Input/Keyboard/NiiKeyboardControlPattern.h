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

#ifndef _NII_KEYBOARD_CONTROLPATTERN_H_
#define _NII_KEYBOARD_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiMouseControlPattern.h"
#include "NiiCommandObj.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace NII_MEDIA
{
    typedef Nui8 KeyValue;

    /** ����������,����������״̬�İ���
    @version NIIEngine 3.0.0
    */
    enum KeyboardAlterLockType
    {
        KT_A_Shift = 0x01,
        KT_A_Ctrl = 0x02,
        KT_A_Alt = 0x04,
        KT_A_Meta = 0x08,
        KT_L_NumLock = 0x10,
        KT_L_CapsLock = 0x20,
        KT_L_ScrLock = 0x40
    };

    /** �豸������չ
    @version NIIEngine 3.0.0
    */
    enum WinComKey
    {
        WCK_None = 0,
        WCK_MB_Left = (1 << NII_MEDIA::MB_Left),
        WCK_MB_Right = (1 << NII_MEDIA::MB_Right),
        WCK_MB_Middle = (1 << NII_MEDIA::MB_Middle),
        WCK_MB_Button3 = (1 << NII_MEDIA::MB_Button3),
        WCK_MB_Button4 = (1 << NII_MEDIA::MB_Button4),
        WCK_KT_A_Shift = 0x1000,
        WCK_KT_A_Ctrl = 0x2000,
        WCK_KT_A_Alt = 0x4000
    };

    /** ���̻�����104��׼
    @remark
        ���ϣ��ʹ�ø���İ���,����ʹ�� ListenerKeyboardControlItem ���ȡ����ı�̿ռ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Key
    {
    public:
        /** �޷���
        @version NIIEngine 3.0.0
        */
        static const KeyValue Unknow;

        /** ESC����(��������)
        @remark 1
        @version NIIEngine 3.0.0
        */
        static const KeyValue Esc;

        /** ����1��(��������)
        @remark 2
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO1;

        /** ����2��(��������)
        @remark 3
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO2;

        /** ����3��(��������)
        @remark 4
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO3;

        /** ����4��(��������)
        @remark 5
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO4;

        /** ����5��(��������)
        @remark 6
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO5;

        /** ����6��(��������)
        @remark 7
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO6;

        /** ����7��(��������)
        @remark 8
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO7;

        /** ����8��(��������)
        @remark 9
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO8;

        /** ����9��(��������)
        @remark 10
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO9;

        /** ����0��(��������)
        @remark 11
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO0;

        /** - �� _ ��(��������)
        @remark 12
        @version NIIEngine 3.0.0
        */
        static const KeyValue Minus;

        /** = �� + ��(��������)
        @remark 13
        @version NIIEngine 3.0.0
        */
        static const KeyValue Equals;

        /** �ո��(��������)
        @remark 14
        @version NIIEngine 3.0.0
        */
        static const KeyValue BK;

        /** tab��(��������)
        @remark 15
        @version NIIEngine 3.0.0
        */
        static const KeyValue TAB;

        /** q��
        @remark 16
        @version NIIEngine 3.0.0
        */
        static const KeyValue Q;

        /** w��
        @remark 17
        @version NIIEngine 3.0.0
        */
        static const KeyValue W;

        /** e��
        @remark 18
        @version NIIEngine 3.0.0
        */
        static const KeyValue E;

        /** r��
        @remark 19
        @version NIIEngine 3.0.0
        */
        static const KeyValue R;

        /** t��
        @remark 20
        @version NIIEngine 3.0.0
        */
        static const KeyValue T;

        /** y��
        @remark 21
        @version NIIEngine 3.0.0
        */
        static const KeyValue Y;

        /** u��
        @remark 22
        @version NIIEngine 3.0.0
        */
        static const KeyValue U;

        /** i��
        @remark 23
        @version NIIEngine 3.0.0
        */
        static const KeyValue I;

        /** o��
        @remark 24
        @version NIIEngine 3.0.0
        */
        static const KeyValue O;

        /** p��
        @remark 25
        @version NIIEngine 3.0.0
        */
        static const KeyValue P;

        /** [ �� { ��(��������)
        @remark 26
        @version NIIEngine 3.0.0
        */
        static const KeyValue LBracket;

        /** ] �� } ��(��������)
        @remark 27
        @version NIIEngine 3.0.0
        */
        static const KeyValue RBracket;

        /** Enter��(��������)
        @remark 28
        @version NIIEngine 3.0.0
        */
        static const KeyValue Enter;

        /** ��ctrl(��������)
        @remark 29
        @version NIIEngine 3.0.0
        */
        static const KeyValue LCtrl;

        /** a��
        @remark 30
        @version NIIEngine 3.0.0
        */
        static const KeyValue A;

        /** s��
        @remark 31
        @version NIIEngine 3.0.0
        */
        static const KeyValue S;

        /** d��
        @remark 32
        @version NIIEngine 3.0.0
        */
        static const KeyValue D;

        /** f��
        @remark 33
        @version NIIEngine 3.0.0
        */
        static const KeyValue F;

        /** g��
        @remark 34
        @version NIIEngine 3.0.0
        */
        static const KeyValue G;

        /** h��
        @remark 35
        @version NIIEngine 3.0.0
        */
        static const KeyValue H;

        /** j��
        @remark 36
        @version NIIEngine 3.0.0
        */
        static const KeyValue J;

        /** k��
        @remark 37
        @version NIIEngine 3.0.0
        */
        static const KeyValue K;

        /** l��
        @remark 38
        @version NIIEngine 3.0.0
        */
        static const KeyValue L;

        /** ; �� : ��(��������)
        @remark 39
        @version NIIEngine 3.0.0
        */
        static const KeyValue Semicolon;

        /** ' �� " ��(��������)
        @remark 40
        @version NIIEngine 3.0.0
        */
        static const KeyValue Apostrophe;

        /** ` �� ~ ��(��������)
        @remark 41
        @version NIIEngine 3.0.0
        */
        static const KeyValue Grave;

        /** ��shift��(��������)
        @remark 42
        @version NIIEngine 3.0.0
        */
        static const KeyValue LShift;

        /** \��|��(102��������)
        @remark 43
        @version NIIEngine 3.0.0
        */
        static const KeyValue BackSlash;

        /** z��
        @remark 44
        @version NIIEngine 3.0.0
        */
        static const KeyValue Z;

        /** x��
        @remark 45
        @version NIIEngine 3.0.0
        */
        static const KeyValue X;

        /** c��
        @remark 46
        @version NIIEngine 3.0.0
        */
        static const KeyValue C;

        /** v��
        @remark 47
        @version NIIEngine 3.0.0
        */
        static const KeyValue V;

        /** b��
        @remark 48
        @version NIIEngine 3.0.0
        */
        static const KeyValue B;

        /** n��
        @remark 49
        @version NIIEngine 3.0.0
        */
        static const KeyValue N;

        /** m��
        @remark 50
        @version NIIEngine 3.0.0
        */
        static const KeyValue M;

        /** , �� < ��(��������)
        @remark 51
        @version NIIEngine 3.0.0
        */
        static const KeyValue Comma;

        /** . �� > ��(��������)
        @remark 52
        @version NIIEngine 3.0.0
        */
        static const KeyValue Period;

        /** / �� ? ��(��������)
        @remark 53
        @version NIIEngine 3.0.0
        */
        static const KeyValue Slash;

        /** ��shift��(��������)
        @remark 54
        @version NIIEngine 3.0.0
        */
        static const KeyValue RShift;

        /** *��(���ּ���)
        @remark 55
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Mul;

        /** ��alt��(��������)
        @remark 56
        @version NIIEngine 3.0.0
        */
        static const KeyValue LAlt;

        /** Space��
        @remark 57
        @version NIIEngine 3.0.0
        */
        static const KeyValue Space;

        /** CapsLock��
        @remark 58
        @version NIIEngine 3.0.0
        */
        static const KeyValue CapsLock;

        /** F1��
        @remark 59
        @version NIIEngine 3.0.0
        */
        static const KeyValue F1;

        /** F2��
        @remark 60
        @version NIIEngine 3.0.0
        */
        static const KeyValue F2;

        /** F3��
        @remark 61
        @version NIIEngine 3.0.0
        */
        static const KeyValue F3;

        /** F4��
        @remark 62
        @version NIIEngine 3.0.0
        */
        static const KeyValue F4;

        /** F5��
        @remark 63
        @version NIIEngine 3.0.0
        */
        static const KeyValue F5;

        /** F6��
        @remark 64
        @version NIIEngine 3.0.0
        */
        static const KeyValue F6;

        /** F7��
        @remark 65
        @version NIIEngine 3.0.0
        */
        static const KeyValue F7;

        /** F8��
        @remark 66
        @version NIIEngine 3.0.0
        */
        static const KeyValue F8;

        /** F9��
        @remark 67
        @version NIIEngine 3.0.0
        */
        static const KeyValue F9;

        /** F10��
        @remark 68
        @version NIIEngine 3.0.0
        */
        static const KeyValue F10;

        /** NumLock��
        @remark 69
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Lock;

        /** ScrollLock��
        @remark 70
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_ScrLock;

        /** ����7��(��������)
        @remark 71
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO7;

        /** ����8��(��������)
        @remark 72
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO8;

        /** ����9��(��������)
        @remark 73
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO9;

        /** -��(��������)
        @remark 74
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Subtract;

        /** ����4��(��������)
        @remark 75
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO4;

        /** ����5��(��������)
        @remark 76
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO5;

        /** ����6��(��������)
        @remark 77
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO6;

        /** +��(��������)
        @remark 78
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Add;

        /** ����1��(��������)
        @remark 79
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO1;

        /** ����2��(��������)
        @remark 80
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO2;

        /** ����3��(��������)
        @remark 81
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO3;

        /** Ins������0��(��������)
        @remark 82
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO0;

        /** . �� Del ��(��������)
        @remark 83 �������е�����������Ϊֹ������ͬ��
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Dot;

        /** F11��(101+��������)
        @remark 84
        @version NIIEngine 3.0.0
        */
        static const KeyValue F11;

        /** F12��(101+��������)
        @remark 85
        @version NIIEngine 3.0.0
        */
        static const KeyValue F12;

        /** Enter��(��������)
        @remark 86
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Enter;

        /** ��ctrl��(104��������)
        @remark 87
        @version NIIEngine 3.0.0
        */
        static const KeyValue RCtrl;

        /** /��(��������)
        @remark 88
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Divide;

        /** ������Ļͼ��(��������)
        @remark 89
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_SysRQ;

        /** ��alt��(104��������)
        @remark 90
        @version NIIEngine 3.0.0
        */
        static const KeyValue RAlt;

        /** ��ͣ��(��������)
        @remark 91
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Pause;

        /** Home��(��������)
        @remark 92
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Home;

        /** UpArrow��(��������)
        @remark 93
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Up;

        /** PgUp��(��������)
        @remark 94
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_PgUp;

        /** LeftArrow��(��������)
        @remark 95
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Left;

        /** LeftArrow��(��������)
        @remark 96
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Right;

        /** End��(��������)
        @remark 97
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_End;

        /** DownArrow��(��������)
        @remark 98
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Down;

        /** PgDn��(��������)
        @remark 99
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_PgDown;

        /** Insert��(��������)
        @remark 100
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Insert;

        /** Delete��(��������)
        @remark 101
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Delete;

        /** ��Win��(��������)
        @remark 102
        @version NIIEngine 3.0.0
        */
        static const KeyValue LWin;

        /** ��Win��(��������)
        @remark 103
        @version NIIEngine 3.0.0
        */
        static const KeyValue RWin;

        /** AppMenu��(��������)
        @remark 104
        @version NIIEngine 3.0.0
        */
        static const KeyValue Compose;

        /** ������ʶ
        @remark ��ʶ
        */
        static const KeyValue Max;
    };

    /** ����¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyboardArgs : public EventArgs
    {
    public:
        KeyboardArgs(KeyValue k, bool state);
    public:
        KeyValue mKey;                          ///< �����ı���
        bool mState;                            ///< ture press /false release
    };

    typedef void(KeyboardControlItem::*KeyboardMethod)(const KeyboardArgs *);
    
    /** �������гɷ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyboardSerialItemArgs : public SerialItemArgs
    {
    public:
        KeyboardSerialItemArgs(KeyValue k);
        ~KeyboardSerialItemArgs();

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const;

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const;

        KeyValue mKey;                       ///< �����ı���
    };
    
    /** ���̿��Ƶ�Ԫ
    @remark
        ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���Ĵ���
        ��������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @par 104���̱�׼
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyboardControlItem : public ControlItem
    {
        friend class KeyboardControlPattern;
    public:
        KeyboardControlItem();

        virtual ~KeyboardControlItem();

        /** ָ������ģʽ�Ƿ�������״̬
        @remark �첽״̬
        @param[in] type ��Ҫ���İ���
        @version NIIEngine 3.0.0
        */
        bool isEnable(KeyboardAlterLockType type);

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    public:
        /** ��Ҫʱ��д
        @remark ESC����(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onEsc(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����1��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����2��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����3��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����4��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����6��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����7��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����8��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����9��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����0��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void on0(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark - �� _ ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMinus(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark = �� + ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onEqual(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark �ո��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onBK(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark tab��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onTAB(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark q��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onQ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark w��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onW(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark e��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onE(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark r��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onR(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark t��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onT(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark y��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onY(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark u��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onU(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark i��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onI(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark o��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onO(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark p��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onP(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark [ �� { ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onLBracket(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ] �� } ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRBracket(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Enter��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onEnter(const KeyboardArgs * arg);

        /** ��ctrl(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onLCtrl(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark a��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onA(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark s��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onS(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark d��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onD(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark f��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark g��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onG(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark h��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onH(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark j��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onJ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark k��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onK(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark l��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onL(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ; �� : ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onSemicolon(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ' �� " ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onApostrophe(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ` �� ~ ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onGrave(const KeyboardArgs * arg);

        /** ��shift��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onLShift(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark \ �� | ��(102��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onBackSlash(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark z��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onZ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark x��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onX(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark c��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onC(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark v��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onV(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark b��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onB(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark n��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onN(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark m��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onM(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark , �� < ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onComma(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark . �� > ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPeriod(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark / �� ? ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onSlash(const KeyboardArgs * arg);

        /** ��shift��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRShift(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark *��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNumMul(const KeyboardArgs * arg);

        /** ��alt��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onLAlt(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Space��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onSpace(const KeyboardArgs * arg);

        /** CapsLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onCapsLock(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F1��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F2��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F3��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F4��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F5��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F6��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F7��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F8��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F9��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F10��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF10(const KeyboardArgs * arg);

        /** NumLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNumLock(const KeyboardArgs * arg);

        /** ScrollLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidScrLock(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����7��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����8��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����9��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark -��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNumSubtract(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����4��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark +��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNumAdd(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����1��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����2��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����3��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Ins������0��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNum0(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark . �� Del ��(��������
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNumDot(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F11��(101+��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF11(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F12��(101+��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onF12(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Enter��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNumEnter(const KeyboardArgs * arg);

        /** ��ctrl��(104��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRCtrl(const KeyboardArgs * arg);

        /** ��alt��(104��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRAlt(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Win��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onLWin(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Win��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRWin(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Alt��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onCompose(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark /��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onNumDivide(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ������Ļͼ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidSysRQ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��ͣ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidPause(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Home��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidHome(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark UpArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidUp(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark PgUp��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidPgUp(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark LeftArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidLeft(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark RightArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidRight(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark End��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidEnd(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark DownArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidDown(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark PgDn��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidPgDown(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Insert��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidInsert(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Delete��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onMidDelete(const KeyboardArgs * arg);
    protected:
        /** ��ʼ��ӳ���
        @remark ���������(���ٵ���һ��)
        */
        static void init();

        /** ��һ���������µ�ʱ�򴥷�
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onPress(KeyValue key);

        /** ��һ�������ͷŵ�ʱ�򴥷�
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onRelease(KeyValue key);
    private:
        void OnDummyMethod(const KeyboardArgs * arg);
    protected:
        Nui8 mModifiers;                    ///< ���뻺��
    };

    /** ������̿��Ƶ�Ԫ
    @version NIIEngine 3.0.0
    */
    class DummyKeyboardControlItem : public KeyboardControlItem, public ControlAlloc
    {
    protected:
        ///@copydetails KeyboardControlItem::onPress
        void onPress(KeyValue) { /*do nothing*/ }

        ///@copydetails KeyboardControlItem::onRelease
        void onRelease(KeyValue) {/*do nothing*/ }
    };

    /** ���������̿��Ƶ�Ԫ
    @remark
        ���������Զ�����̰���ӳ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerKeyboardControlItem : public KeyboardControlItem
    {
    public:
        ListenerKeyboardControlItem();
        virtual ~ListenerKeyboardControlItem();

        /// ��ȡ�������ļ����ַ���ascii��
        inline NIIb getAsciiChar() const { return mChar; }

        /// ��ȡ�������ļ����ַ���unicode��
        inline Nwchar getUnicodeChar() const { return mWChar; }

        inline const String & getVerbose() const { return mDescrip; }
    public:
        /// @copydetails ListenerKeyboardControlItem::onEsc
        virtual void onEsc(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on1
        virtual void on1(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on2
        virtual void on2(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on3
        virtual void on3(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on4
        virtual void on4(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on5
        virtual void on5(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on6
        virtual void on6(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on7
        virtual void on7(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on8
        virtual void on8(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on9
        virtual void on9(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::on0
        virtual void on0(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMinus
        virtual void onMinus(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onEqual
        virtual void onEqual(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onBK
        virtual void onBK(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onTAB
        virtual void onTAB(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onQ
        virtual void onQ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onW
        virtual void onW(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onE
        virtual void onE(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onR
        virtual void onR(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onT
        virtual void onT(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onY
        virtual void onY(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onU
        virtual void onU(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onI
        virtual void onI(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onO
        virtual void onO(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onP
        virtual void onP(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onLBracket
        virtual void onLBracket(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onRBracket
        virtual void onRBracket(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onEnter
        virtual void onEnter(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onLCtrl
        virtual void onLCtrl(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onA
        virtual void onA(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onS
        virtual void onS(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onD
        virtual void onD(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF
        virtual void onF(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onG
        virtual void onG(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onH
        virtual void onH(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onJ
        virtual void onJ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onK
        virtual void onK(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onL
        virtual void onL(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onSemicolon
        virtual void onSemicolon(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onApostrophe
        virtual void onApostrophe(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onGrave
        virtual void onGrave(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onLShift
        virtual void onLShift(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onBackSlash
        virtual void onBackSlash(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onZ
        virtual void onZ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onX
        virtual void onX(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onC
        virtual void onC(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onV
        virtual void onV(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onB
        virtual void onB(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onN
        virtual void onN(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onM
        virtual void onM(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onComma
        virtual void onComma(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPeriod
        virtual void onPeriod(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onSlash
        virtual void onSlash(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onRShift
        virtual void onRShift(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNumMul
        virtual void onNumMul(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onLAlt
        virtual void onLAlt(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onSpace
        virtual void onSpace(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onCapsLock
        virtual void onCapsLock(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF1
        virtual void onF1(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF2
        virtual void onF2(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF3
        virtual void onF3(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF4
        virtual void onF4(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF5
        virtual void onF5(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF6
        virtual void onF6(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF7
        virtual void onF7(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF8
        virtual void onF8(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF9
        virtual void onF9(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF10
        virtual void onF10(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNumLock
        virtual void onNumLock(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidScrLock
        virtual void onMidScrLock(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum7
        virtual void onNum7(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum8
        virtual void onNum8(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum9
        virtual void onNum9(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNumSubtract
        virtual void onNumSubtract(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum4
        virtual void onNum4(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum5
        virtual void onNum5(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum6
        virtual void onNum6(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNumAdd
        virtual void onNumAdd(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum1
        virtual void onNum1(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum2
        virtual void onNum2(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum3
        virtual void onNum3(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNum0
        virtual void onNum0(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNumDot
        virtual void onNumDot(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF11
        virtual void onF11(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onF12
        virtual void onF12(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNumEnter
        virtual void onNumEnter(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onRCtrl
        virtual void onRCtrl(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onRAlt
        virtual void onRAlt(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onLWin
        virtual void onLWin(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onRWin
        virtual void onRWin(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onCompose
        virtual void onCompose(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onNumDivide
        virtual void onNumDivide(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidSysRQ
        virtual void onMidSysRQ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidPause
        virtual void onMidPause(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidHome
        virtual void onMidHome(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidUp
        virtual void onMidUp(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidPgUp
        virtual void onMidPgUp(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidLeft
        virtual void onMidLeft(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidRight
        virtual void onMidRight(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidEnd
        virtual void onMidEnd(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidDown
        virtual void onMidDown(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidPgDown
        virtual void onMidPgDown(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidInsert
        virtual void onMidInsert(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onMidDelete
        virtual void onMidDelete(const KeyboardArgs * arg);
    protected:
        NIIb mChar;
        Nwchar mWChar;
        String mDescrip;
    };

    class IOMap;

    /** �¼������̿��Ƶ�Ԫ
    @remark
        �����ͼ��̿��Ƶ�Ԫ,�������������߼����ӵ��¼�������,���ڶ��������Ҳ���Է���
        �¼�����.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalKeyboardControlItem : public KeyboardControlItem, public CommandObj
    {
    public:
        SignalKeyboardControlItem();

        virtual ~SignalKeyboardControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EscEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Key0Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMinusEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyEqualEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyBKEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyTABEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyQEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyWEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyEEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyREvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyTEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyYEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyUEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyIEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyOEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyPEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyLBracketEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyRBracketEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyEnterEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyLCtrlEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyAEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeySEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyDEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyFEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyGEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyHEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyJEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyKEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyLEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeySemicolonEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyApostropheEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyGraveEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyLShiftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyBackSlashEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyZEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyXEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyCEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyVEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyBEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyCommaEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyPeriodEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeySlashEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyRShiftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNumMulEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyLAltEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeySpaceEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyCapsLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF10Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNumLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidScrLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNumSubtractEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNumAddEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNum0Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNumDotEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF11Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyF12Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNumEnterEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyRCtrlEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyRAltEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyLWinEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyRWinEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyComposeEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyNumDivideEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidSysRQEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidPauseEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidHomeEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidUpEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidPgUpEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidEndEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidDownEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidPgDownEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidInsertEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID KeyMidDeleteEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /// @copydetail KeyboardControlItem::onEsc
        virtual void onEsc(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on1
        virtual void on1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on2
        virtual void on2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on3
        virtual void on3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on4
        virtual void on4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on5
        virtual void on5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on6
        virtual void on6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on7
        virtual void on7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on8
        virtual void on8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on9
        virtual void on9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::on0
        virtual void on0(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMinus
        virtual void onMinus(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onEqual
        virtual void onEqual(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onBK
        virtual void onBK(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onTAB
        virtual void onTAB(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onQ
        virtual void onQ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onW
        virtual void onW(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onE
        virtual void onE(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onR
        virtual void onR(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onT
        virtual void onT(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onY
        virtual void onY(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onU
        virtual void onU(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onI
        virtual void onI(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onO
        virtual void onO(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onP
        virtual void onP(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onLBracket
        virtual void onLBracket(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRBracket
        virtual void onRBracket(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onEnter
        virtual void onEnter(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onLCtrl
        virtual void onLCtrl(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onA
        virtual void onA(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onS
        virtual void onS(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onD
        virtual void onD(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF
        virtual void onF(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onG
        virtual void onG(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onH
        virtual void onH(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onJ
        virtual void onJ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onK
        virtual void onK(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onL
        virtual void onL(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onSemicolon
        virtual void onSemicolon(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onApostrophe
        virtual void onApostrophe(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onGrave
        virtual void onGrave(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onLShift
        virtual void onLShift(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onBackSlash
        virtual void onBackSlash(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onZ
        virtual void onZ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onX
        virtual void onX(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onC
        virtual void onC(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onV
        virtual void onV(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onB
        virtual void onB(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onN
        virtual void onN(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onM
        virtual void onM(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onComma
        virtual void onComma(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPeriod
        virtual void onPeriod(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onSlash
        virtual void onSlash(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRShift
        virtual void onRShift(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNumMul
        virtual void onNumMul(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onLAlt
        virtual void onLAlt(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onSpace
        virtual void onSpace(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onCapsLock
        virtual void onCapsLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF1
        virtual void onF1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF2
        virtual void onF2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF3
        virtual void onF3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF4
        virtual void onF4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF5
        virtual void onF5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF6
        virtual void onF6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF7
        virtual void onF7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF8
        virtual void onF8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF9
        virtual void onF9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF10
        virtual void onF10(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNumLock
        virtual void onNumLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidScrLock
        virtual void onMidScrLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum7
        virtual void onNum7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum8
        virtual void onNum8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum9
        virtual void onNum9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNumSubtract
        virtual void onNumSubtract(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum4
        virtual void onNum4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum5
        virtual void onNum5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum6
        virtual void onNum6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNumAdd
        virtual void onNumAdd(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum1
        virtual void onNum1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum2
        virtual void onNum2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum3
        virtual void onNum3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNum0
        virtual void onNum0(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNumDot
        virtual void onNumDot(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF11
        virtual void onF11(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onF12
        virtual void onF12(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNumEnter
        virtual void onNumEnter(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRCtrl
        virtual void onRCtrl(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRAlt
        virtual void onRAlt(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onLWin
        virtual void onLWin(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRWin
        virtual void onRWin(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onCompose
        virtual void onCompose(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onNumDivide
        virtual void onNumDivide(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidSysRQ
        virtual void onMidSysRQ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidPause
        virtual void onMidPause(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidHome
        virtual void onMidHome(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidUp
        virtual void onMidUp(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidPgUp
        virtual void onMidPgUp(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidLeft
        virtual void onMidLeft(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidRight
        virtual void onMidRight(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidEnd
        virtual void onMidEnd(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidDown
        virtual void onMidDown(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidPgDown
        virtual void onMidPgDown(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidInsert
        virtual void onMidInsert(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onMidDelete
        virtual void onMidDelete(const KeyboardArgs * arg);
    };

    /** �ⲿ�豸���ƴ�������(����)
    @remark ʹ�õ���104��׼�ļ������
    @note
        ���ڴ��ϵͳ�ں˶�ȡ���̼�¼���Զ��л����,����Ϊ�˸��õ���Ʊ��ģ�鲻Ӧ��
        ��ʹ�ö��л���ģʽ
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI KeyboardControlPattern : public ControlPattern
    {
    public:
        /** ��ָ��������
        @remark �����ڲ���������,��ȷ���û�ı����״̬
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        void on(KeyboardAlterLockType type);

        /** �ر�ָ��������
        @remark �����ڲ���������,��ȷ���û�ı����״̬
        @paramp[in] type ��������
        @version NIIEngine 3.0.0
        */
        void off(KeyboardAlterLockType type);

        /** ͬ��Ŀ�����״̬
        @remark ͬ�� capslock/scrolllock/numlock ��״̬
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void syn(KeyboardAlterLockType type);

        /** ָ�������Ƿ񱻰���
        @remark �첽״̬
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        bool isPress(KeyValue key);

        /** ָ�������Ƿ����ͷ�
        @remark �첽״̬
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        bool isRelease(KeyValue key);

        /** ��ȡʵ�ʰ����ַ��ı�
        @remark Unicode����
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getKeyCode(KeyValue key);

        /** ��ȡʵ�ʰ����ַ�����
        @remark
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        virtual String getKeyText(KeyValue key);

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        /** Ĭ�Ϲ��캯��
        */
        KeyboardControlPattern();

        /** Ĭ�Ϲ��캯��
        @param[in] id ��������Ψһ��ʶ,���п�������Ψһ(����ͬ����,�Ͳ�ͬ���͵ĸ���)
        @param[in] own ������������ĸ������
        */
        KeyboardControlPattern(Nid id, const ViewWindow * own);

        virtual ~KeyboardControlPattern();

        /** ����±���
        @remark ����������������¼�,
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void press(const KeyValue key);

        /** �ͷŰ��±���
        @remark ����������������¼�
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void release(const KeyValue key);

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();
    protected:
        KeyboardControlItem * mTarget;              ///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ
        Nui32 mBuffer[8];                           ///< ��������
        Nui8 mModifiers;                            ///< ����ģʽ
    };

    /** ����
    @version NIIEngine 3.0.0
    */
    class DummyKeyboardControlPattern : public KeyboardControlPattern
    {
    public:
        DummyKeyboardControlPattern() {}

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