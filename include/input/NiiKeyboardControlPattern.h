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
        KeyboardArgs(KeyValue k);
    public:
        KeyValue mKey;                          ///< �����ı���
    };

    typedef void(KeyboardControlItem::*KeyboardMethod)(const KeyboardArgs *);
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
        virtual void onPressEsc(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ESC����(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseEsc(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����1��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����1��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����2��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����2��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����3��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����3��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����4��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����4��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����6��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����6��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����7��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����7��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����8��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����8��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����9��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����9��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����0��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPress0(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����0��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onRelease0(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark - �� _ ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMinus(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark - �� _ ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMinus(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark = �� + ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressEqual(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark = �� + ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseEqual(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark �ո��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressBK(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark �ո��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseBK(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark tab��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressTAB(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark tab��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseTAB(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark q��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressQ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark q��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseQ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark w��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressW(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark w��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseW(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark e��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressE(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark e��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseE(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark r��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressR(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark r��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseR(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark t��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressT(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark t��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseT(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark y��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressY(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark y��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseY(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark u��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressU(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark u��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseU(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark i��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressI(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark i��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseI(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark o��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressO(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark o��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseO(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark p��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressP(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark p��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseP(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark [ �� { ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressLBracket(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark [ �� { ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLBracket(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ] �� } ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressRBracket(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ] �� } ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRBracket(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Enter��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressEnter(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Enter��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseEnter(const KeyboardArgs * arg);

        /** ��ctrl(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressLCtrl(const KeyboardArgs * arg);

        /** ��ctrl(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLCtrl(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark a��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressA(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark a��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseA(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark s��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressS(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark s��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseS(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark d��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressD(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark d��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseD(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark f��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark f��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark g��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressG(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark g��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseG(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark h��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressH(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark h��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseH(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark j��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressJ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark j��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseJ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark k��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressK(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark k��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseK(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark l��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressL(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark l��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseL(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ; �� : ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressSemicolon(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ; �� : ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseSemicolon(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ' �� " ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressApostrophe(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ' �� " ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseApostrophe(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ` �� ~ ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressGrave(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ` �� ~ ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseGrave(const KeyboardArgs * arg);

        /** ��shift��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressLShift(const KeyboardArgs * arg);

        /** ��shift��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLShift(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark \ �� | ��(102��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressBackSlash(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark \ �� | ��(102��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseBackSlash(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark z��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressZ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark z��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseZ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark x��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressX(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark x��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseX(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark c��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressC(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark c��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseC(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark v��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressV(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark v��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseV(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark b��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressB(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark b��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseB(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark n��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressN(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark n��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseN(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark m��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressM(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark m��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseM(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark , �� < ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressComma(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark , �� < ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseComma(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark . �� > ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressPeriod(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark . �� > ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleasePeriod(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark / �� ? ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressSlash(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark / �� ? ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseSlash(const KeyboardArgs * arg);

        /** ��shift��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressRShift(const KeyboardArgs * arg);

        /** ��shift��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRShift(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark *��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumMul(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark *��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumMul(const KeyboardArgs * arg);

        /** ��alt��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressLAlt(const KeyboardArgs * arg);

        /** ��alt��(��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLAlt(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Space��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressSpace(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Space��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseSpace(const KeyboardArgs * arg);

        /** CapsLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressCapsLock(const KeyboardArgs * arg);

        /** CapsLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseCapsLock(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F1��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F1��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F2��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F2��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F3��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F3��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F4��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F4��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F5��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F5��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F6��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F6��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F7��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F7��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F8��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F8��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F9��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F9��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F10��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF10(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F10��
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF10(const KeyboardArgs * arg);

        /** NumLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumLock(const KeyboardArgs * arg);

        /** NumLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumLock(const KeyboardArgs * arg);

        /** ScrollLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidScrLock(const KeyboardArgs * arg);

        /** ScrollLock��
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidScrLock(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����7��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����7��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum7(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����8��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����8��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum8(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����9��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����9��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum9(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark -��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumSubtract(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark -��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumSubtract(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����4��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����4��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0@version NIIEngine 3.0.0
        */
        virtual void onReleaseNum4(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum5(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����5��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum6(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark +��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumAdd(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark +��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumAdd(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����1��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����1��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum1(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����2��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����2��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum2(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����3��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ����3��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum3(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Ins������0��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum0(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Ins������0��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum0(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark . �� Del ��(��������
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumDot(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark . �� Del ��(��������
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumDot(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F11��(101+��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF11(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F11��(101+��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF11(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F12��(101+��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressF12(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark F12��(101+��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF12(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Enter��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumEnter(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Enter��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumEnter(const KeyboardArgs * arg);

        /** ��ctrl��(104��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressRCtrl(const KeyboardArgs * arg);

        /** ��ctrl��(104��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRCtrl(const KeyboardArgs * arg);

        /** ��alt��(104��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressRAlt(const KeyboardArgs * arg);

        /** ��alt��(104��������)
        @remark �����������̳�
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRAlt(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Win��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressLWin(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Win��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLWin(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Win��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressRWin(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Win��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRWin(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Alt��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressCompose(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��Alt��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseCompose(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark /��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumDivide(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark /��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumDivide(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ������Ļͼ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidSysRQ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ������Ļͼ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidSysRQ(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��ͣ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidPause(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark ��ͣ��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidPause(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Home��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidHome(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Home��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidHome(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark UpArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidUp(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark UpArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidUp(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark PgUp��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidPgUp(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark PgUp��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidPgUp(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark LeftArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidLeft(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark LeftArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidLeft(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark RightArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidRight(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark RightArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidRight(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark End��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidEnd(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark End��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidEnd(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark DownArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidDown(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark DownArrow��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidDown(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark PgDn��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidPgDown(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark PgDn��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidPgDown(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Insert��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidInsert(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Insert��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidInsert(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Delete��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidDelete(const KeyboardArgs * arg);

        /** ��Ҫʱ��д
        @remark Delete��(��������)
        @par 104���̱�׼
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidDelete(const KeyboardArgs * arg);
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
        /// @copydetails ListenerKeyboardControlItem::onPressEsc
        virtual void onPressEsc(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress1
        virtual void onPress1(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress2
        virtual void onPress2(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress3
        virtual void onPress3(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress4
        virtual void onPress4(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress5
        virtual void onPress5(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress6
        virtual void onPress6(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress7
        virtual void onPress7(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress8
        virtual void onPress8(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress9
        virtual void onPress9(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPress0
        virtual void onPress0(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMinus
        virtual void onPressMinus(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressEqual
        virtual void onPressEqual(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressBK
        virtual void onPressBK(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressTAB
        virtual void onPressTAB(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressQ
        virtual void onPressQ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressW
        virtual void onPressW(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressE
        virtual void onPressE(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressR
        virtual void onPressR(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressT
        virtual void onPressT(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressY
        virtual void onPressY(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressU
        virtual void onPressU(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressI
        virtual void onPressI(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressO
        virtual void onPressO(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressP
        virtual void onPressP(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressLBracket
        virtual void onPressLBracket(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressRBracket
        virtual void onPressRBracket(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressEnter
        virtual void onPressEnter(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressLCtrl
        virtual void onPressLCtrl(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressA
        virtual void onPressA(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressS
        virtual void onPressS(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressD
        virtual void onPressD(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF
        virtual void onPressF(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressG
        virtual void onPressG(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressH
        virtual void onPressH(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressJ
        virtual void onPressJ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressK
        virtual void onPressK(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressL
        virtual void onPressL(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressSemicolon
        virtual void onPressSemicolon(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressApostrophe
        virtual void onPressApostrophe(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressGrave
        virtual void onPressGrave(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressLShift
        virtual void onPressLShift(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressBackSlash
        virtual void onPressBackSlash(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressZ
        virtual void onPressZ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressX
        virtual void onPressX(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressC
        virtual void onPressC(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressV
        virtual void onPressV(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressB
        virtual void onPressB(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressN
        virtual void onPressN(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressM
        virtual void onPressM(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressComma
        virtual void onPressComma(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressPeriod
        virtual void onPressPeriod(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressSlash
        virtual void onPressSlash(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressRShift
        virtual void onPressRShift(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNumMul
        virtual void onPressNumMul(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressLAlt
        virtual void onPressLAlt(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressSpace
        virtual void onPressSpace(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressCapsLock
        virtual void onPressCapsLock(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF1
        virtual void onPressF1(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF2
        virtual void onPressF2(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF3
        virtual void onPressF3(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF4
        virtual void onPressF4(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF5
        virtual void onPressF5(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF6
        virtual void onPressF6(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF7
        virtual void onPressF7(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF8
        virtual void onPressF8(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF9
        virtual void onPressF9(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF10
        virtual void onPressF10(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNumLock
        virtual void onPressNumLock(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidScrLock
        virtual void onPressMidScrLock(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum7
        virtual void onPressNum7(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum8
        virtual void onPressNum8(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum9
        virtual void onPressNum9(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNumSubtract
        virtual void onPressNumSubtract(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum4
        virtual void onPressNum4(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum5
        virtual void onPressNum5(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum6
        virtual void onPressNum6(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNumAdd
        virtual void onPressNumAdd(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum1
        virtual void onPressNum1(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum2
        virtual void onPressNum2(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum3
        virtual void onPressNum3(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNum0
        virtual void onPressNum0(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNumDot
        virtual void onPressNumDot(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF11
        virtual void onPressF11(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressF12
        virtual void onPressF12(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNumEnter
        virtual void onPressNumEnter(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressRCtrl
        virtual void onPressRCtrl(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressRAlt
        virtual void onPressRAlt(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressLWin
        virtual void onPressLWin(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressRWin
        virtual void onPressRWin(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressCompose
        virtual void onPressCompose(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressNumDivide
        virtual void onPressNumDivide(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidSysRQ
        virtual void onPressMidSysRQ(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidPause
        virtual void onPressMidPause(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidHome
        virtual void onPressMidHome(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidUp
        virtual void onPressMidUp(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidPgUp
        virtual void onPressMidPgUp(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidLeft
        virtual void onPressMidLeft(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidRight
        virtual void onPressMidRight(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidEnd
        virtual void onPressMidEnd(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidDown
        virtual void onPressMidDown(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidPgDown
        virtual void onPressMidPgDown(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidInsert
        virtual void onPressMidInsert(const KeyboardArgs * arg);

        /// @copydetails ListenerKeyboardControlItem::onPressMidDelete
        virtual void onPressMidDelete(const KeyboardArgs * arg);
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
        static const EventID PressEscEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseEscEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Press0Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID Release0Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMinusEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMinusEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressEqualEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseEqualEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressBKEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseBKEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressTABEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseTABEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressQEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseQEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressWEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseWEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressEEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseEEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressREvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseREvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressTEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseTEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressYEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseYEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressUEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseUEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressIEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseIEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressOEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseOEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressPEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleasePEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLBracketEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLBracketEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRBracketEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRBracketEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressEnterEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseEnterEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLCtrlEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLCtrlEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressAEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseAEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressSEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseSEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressDEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseDEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressFEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseFEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressGEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseGEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressHEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseHEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressJEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseJEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressKEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseKEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressSemicolonEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseSemicolonEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressApostropheEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseApostropheEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressGraveEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseGraveEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLShiftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLShiftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressBackSlashEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseBackSlashEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressZEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseZEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressXEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseXEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressCEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseCEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressVEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseVEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressBEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseBEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressCommaEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseCommaEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressPeriodEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleasePeriodEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressSlashEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseSlashEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRShiftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRShiftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNumMulEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNumMulEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLAltEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLAltEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressSpaceEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseSpaceEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressCapsLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseCapsLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF10Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF10Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNumLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNumLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidScrLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidScrLockEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum8Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum9Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNumSubtractEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNumSubtractEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNumAddEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNumAddEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum1Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum2Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNum0Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNum0Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNumDotEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNumDotEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF11Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF11Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressF12Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseF12Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNumEnterEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNumEnterEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRCtrlEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRCtrlEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRAltEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRAltEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLWinEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLWinEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRWinEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRWinEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressComposeEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseComposeEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressNumDivideEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseNumDivideEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidSysRQEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidSysRQEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidPauseEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidPauseEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidHomeEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidHomeEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidUpEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidUpEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidPgUpEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidPgUpEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidEndEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidEndEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidDownEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidDownEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidPgDownEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidPgDownEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidInsertEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidInsertEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMidDeleteEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMidDeleteEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /// @copydetail KeyboardControlItem::onPressEsc
        virtual void onPressEsc(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseEsc
        virtual void onReleaseEsc(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress1
        virtual void onPress1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease1
        virtual void onRelease1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress2
        virtual void onPress2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease2
        virtual void onRelease2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress3
        virtual void onPress3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease3
        virtual void onRelease3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress4
        virtual void onPress4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease4
        virtual void onRelease4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress5
        virtual void onPress5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease5
        virtual void onRelease5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress6
        virtual void onPress6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease6
        virtual void onRelease6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress7
        virtual void onPress7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease7
        virtual void onRelease7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress8
        virtual void onPress8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease8
        virtual void onRelease8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress9
        virtual void onPress9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease9
        virtual void onRelease9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPress0
        virtual void onPress0(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onRelease0
        virtual void onRelease0(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMinus
        virtual void onPressMinus(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMinus
        virtual void onReleaseMinus(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressEqual
        virtual void onPressEqual(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseEqual
        virtual void onReleaseEqual(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressBK
        virtual void onPressBK(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseBK
        virtual void onReleaseBK(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressTAB
        virtual void onPressTAB(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseTAB
        virtual void onReleaseTAB(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressQ
        virtual void onPressQ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseQ
        virtual void onReleaseQ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressW
        virtual void onPressW(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseW
        virtual void onReleaseW(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressE
        virtual void onPressE(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseE
        virtual void onReleaseE(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressR
        virtual void onPressR(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseR
        virtual void onReleaseR(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressT
        virtual void onPressT(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseT
        virtual void onReleaseT(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressY
        virtual void onPressY(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseY
        virtual void onReleaseY(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressU
        virtual void onPressU(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseU
        virtual void onReleaseU(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressI
        virtual void onPressI(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseI
        virtual void onReleaseI(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressO
        virtual void onPressO(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseO
        virtual void onReleaseO(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressP
        virtual void onPressP(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseP
        virtual void onReleaseP(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressLBracket
        virtual void onPressLBracket(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseLBracket
        virtual void onReleaseLBracket(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressRBracket
        virtual void onPressRBracket(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseRBracket
        virtual void onReleaseRBracket(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressEnter
        virtual void onPressEnter(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseEnter
        virtual void onReleaseEnter(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressLCtrl
        virtual void onPressLCtrl(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseLCtrl
        virtual void onReleaseLCtrl(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressA
        virtual void onPressA(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseA
        virtual void onReleaseA(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressS
        virtual void onPressS(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseS
        virtual void onReleaseS(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressD
        virtual void onPressD(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseD
        virtual void onReleaseD(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF
        virtual void onPressF(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF
        virtual void onReleaseF(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressG
        virtual void onPressG(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseG
        virtual void onReleaseG(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressH
        virtual void onPressH(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseH
        virtual void onReleaseH(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressJ
        virtual void onPressJ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseJ
        virtual void onReleaseJ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressK
        virtual void onPressK(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseK
        virtual void onReleaseK(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressL
        virtual void onPressL(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseL
        virtual void onReleaseL(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressSemicolon
        virtual void onPressSemicolon(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseSemicolon
        virtual void onReleaseSemicolon(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressApostrophe
        virtual void onPressApostrophe(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseApostrophe
        virtual void onReleaseApostrophe(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressGrave
        virtual void onPressGrave(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseGrave
        virtual void onReleaseGrave(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressLShift
        virtual void onPressLShift(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseLShift
        virtual void onReleaseLShift(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressBackSlash
        virtual void onPressBackSlash(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseBackSlash
        virtual void onReleaseBackSlash(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressZ
        virtual void onPressZ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseZ
        virtual void onReleaseZ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressX
        virtual void onPressX(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseX
        virtual void onReleaseX(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressC
        virtual void onPressC(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseC
        virtual void onReleaseC(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressV
        virtual void onPressV(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseV
        virtual void onReleaseV(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressB
        virtual void onPressB(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseB
        virtual void onReleaseB(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressN
        virtual void onPressN(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseN
        virtual void onReleaseN(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressM
        virtual void onPressM(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseM
        virtual void onReleaseM(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressComma
        virtual void onPressComma(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseComma
        virtual void onReleaseComma(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressPeriod
        virtual void onPressPeriod(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleasePeriod
        virtual void onReleasePeriod(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressSlash
        virtual void onPressSlash(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseSlash
        virtual void onReleaseSlash(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressRShift
        virtual void onPressRShift(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseRShift
        virtual void onReleaseRShift(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNumMul
        virtual void onPressNumMul(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumMul
        virtual void onReleaseNumMul(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressLAlt
        virtual void onPressLAlt(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseLAlt
        virtual void onReleaseLAlt(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressSpace
        virtual void onPressSpace(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseSpace
        virtual void onReleaseSpace(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressCapsLock
        virtual void onPressCapsLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseCapsLock
        virtual void onReleaseCapsLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF1
        virtual void onPressF1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF1
        virtual void onReleaseF1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF2
        virtual void onPressF2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF2
        virtual void onReleaseF2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF3
        virtual void onPressF3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF3
        virtual void onReleaseF3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF4
        virtual void onPressF4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF4
        virtual void onReleaseF4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF5
        virtual void onPressF5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF5
        virtual void onReleaseF5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF6
        virtual void onPressF6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF6
        virtual void onReleaseF6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF7
        virtual void onPressF7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF7
        virtual void onReleaseF7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF8
        virtual void onPressF8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF8
        virtual void onReleaseF8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF9
        virtual void onPressF9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF9
        virtual void onReleaseF9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF10
        virtual void onPressF10(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF10
        virtual void onReleaseF10(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNumLock
        virtual void onPressNumLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumLock
        virtual void onReleaseNumLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidScrLock
        virtual void onPressMidScrLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidScrLock
        virtual void onReleaseMidScrLock(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum7
        virtual void onPressNum7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum7
        virtual void onReleaseNum7(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum8
        virtual void onPressNum8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum8
        virtual void onReleaseNum8(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum9
        virtual void onPressNum9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum9
        virtual void onReleaseNum9(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNumSubtract
        virtual void onPressNumSubtract(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumSubtract
        virtual void onReleaseNumSubtract(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum4
        virtual void onPressNum4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum4
        virtual void onReleaseNum4(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum5
        virtual void onPressNum5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum5
        virtual void onReleaseNum5(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum6
        virtual void onPressNum6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumNO6
        virtual void onReleaseNum6(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNumAdd
        virtual void onPressNumAdd(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumAdd
        virtual void onReleaseNumAdd(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum1
        virtual void onPressNum1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum1
        virtual void onReleaseNum1(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum2
        virtual void onPressNum2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum2
        virtual void onReleaseNum2(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum3
        virtual void onPressNum3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum3
        virtual void onReleaseNum3(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNum0
        virtual void onPressNum0(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNum0
        virtual void onReleaseNum0(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNumDot
        virtual void onPressNumDot(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumDot
        virtual void onReleaseNumDot(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF11
        virtual void onPressF11(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF11
        virtual void onReleaseF11(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressF12
        virtual void onPressF12(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseF12
        virtual void onReleaseF12(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNumEnter
        virtual void onPressNumEnter(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumEnter
        virtual void onReleaseNumEnter(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressRCtrl
        virtual void onPressRCtrl(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseRCtrl
        virtual void onReleaseRCtrl(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressRAlt
        virtual void onPressRAlt(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseRAlt
        virtual void onReleaseRAlt(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressLWin
        virtual void onPressLWin(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseLWin
        virtual void onReleaseLWin(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressRWin
        virtual void onPressRWin(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseRWin
        virtual void onReleaseRWin(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressCompose
        virtual void onPressCompose(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseCompose
        virtual void onReleaseCompose(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressNumDivide
        virtual void onPressNumDivide(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseNumDivide
        virtual void onReleaseNumDivide(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidSysRQ
        virtual void onPressMidSysRQ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidSysRQ
        virtual void onReleaseMidSysRQ(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidPause
        virtual void onPressMidPause(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidPause
        virtual void onReleaseMidPause(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidHome
        virtual void onPressMidHome(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidHome
        virtual void onReleaseMidHome(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidUp
        virtual void onPressMidUp(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidUp
        virtual void onReleaseMidUp(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidPgUp
        virtual void onPressMidPgUp(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidPgUp
        virtual void onReleaseMidPgUp(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidLeft
        virtual void onPressMidLeft(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidLeft
        virtual void onReleaseMidLeft(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidRight
        virtual void onPressMidRight(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidRight
        virtual void onReleaseMidRight(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidEnd
        virtual void onPressMidEnd(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidEnd
        virtual void onReleaseMidEnd(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidDown
        virtual void onPressMidDown(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidDown
        virtual void onReleaseMidDown(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidPgDown
        virtual void onPressMidPgDown(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidPgDown
        virtual void onReleaseMidPgDown(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidInsert
        virtual void onPressMidInsert(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidInsert
        virtual void onReleaseMidInsert(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onPressMidDelete
        virtual void onPressMidDelete(const KeyboardArgs * arg);

        /// @copydetail KeyboardControlItem::onReleaseMidDelete
        virtual void onReleaseMidDelete(const KeyboardArgs * arg);
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