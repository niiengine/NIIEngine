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

    /** 键盘锁类型,包含会锁定状态的按键
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

    /** 设备输入扩展
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

    /** 键盘机制码104标准
    @remark
        如果希望使用更多的按键,可以使用 ListenerKeyboardControlItem 类获取更多的编程空间
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Key
    {
    public:
        /** 无分配
        @version NIIEngine 3.0.0
        */
        static const KeyValue Unknow;

        /** ESC按键(主键盘上)
        @remark 1
        @version NIIEngine 3.0.0
        */
        static const KeyValue Esc;

        /** 数字1键(主键盘上)
        @remark 2
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO1;

        /** 数字2键(主键盘上)
        @remark 3
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO2;

        /** 数字3键(主键盘上)
        @remark 4
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO3;

        /** 数字4键(主键盘上)
        @remark 5
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO4;

        /** 数字5键(主键盘上)
        @remark 6
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO5;

        /** 数字6键(主键盘上)
        @remark 7
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO6;

        /** 数字7键(主键盘上)
        @remark 8
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO7;

        /** 数字8键(主键盘上)
        @remark 9
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO8;

        /** 数字9键(主键盘上)
        @remark 10
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO9;

        /** 数字0键(主键盘上)
        @remark 11
        @version NIIEngine 3.0.0
        */
        static const KeyValue NO0;

        /** - 和 _ 键(主键盘上)
        @remark 12
        @version NIIEngine 3.0.0
        */
        static const KeyValue Minus;

        /** = 和 + 键(主键盘上)
        @remark 13
        @version NIIEngine 3.0.0
        */
        static const KeyValue Equals;

        /** 空格键(主键盘上)
        @remark 14
        @version NIIEngine 3.0.0
        */
        static const KeyValue BK;

        /** tab键(主键盘上)
        @remark 15
        @version NIIEngine 3.0.0
        */
        static const KeyValue TAB;

        /** q键
        @remark 16
        @version NIIEngine 3.0.0
        */
        static const KeyValue Q;

        /** w键
        @remark 17
        @version NIIEngine 3.0.0
        */
        static const KeyValue W;

        /** e键
        @remark 18
        @version NIIEngine 3.0.0
        */
        static const KeyValue E;

        /** r键
        @remark 19
        @version NIIEngine 3.0.0
        */
        static const KeyValue R;

        /** t键
        @remark 20
        @version NIIEngine 3.0.0
        */
        static const KeyValue T;

        /** y键
        @remark 21
        @version NIIEngine 3.0.0
        */
        static const KeyValue Y;

        /** u键
        @remark 22
        @version NIIEngine 3.0.0
        */
        static const KeyValue U;

        /** i键
        @remark 23
        @version NIIEngine 3.0.0
        */
        static const KeyValue I;

        /** o键
        @remark 24
        @version NIIEngine 3.0.0
        */
        static const KeyValue O;

        /** p键
        @remark 25
        @version NIIEngine 3.0.0
        */
        static const KeyValue P;

        /** [ 和 { 键(主键盘上)
        @remark 26
        @version NIIEngine 3.0.0
        */
        static const KeyValue LBracket;

        /** ] 和 } 键(主键盘上)
        @remark 27
        @version NIIEngine 3.0.0
        */
        static const KeyValue RBracket;

        /** Enter键(主键盘上)
        @remark 28
        @version NIIEngine 3.0.0
        */
        static const KeyValue Enter;

        /** 左ctrl(主键盘上)
        @remark 29
        @version NIIEngine 3.0.0
        */
        static const KeyValue LCtrl;

        /** a键
        @remark 30
        @version NIIEngine 3.0.0
        */
        static const KeyValue A;

        /** s键
        @remark 31
        @version NIIEngine 3.0.0
        */
        static const KeyValue S;

        /** d键
        @remark 32
        @version NIIEngine 3.0.0
        */
        static const KeyValue D;

        /** f键
        @remark 33
        @version NIIEngine 3.0.0
        */
        static const KeyValue F;

        /** g键
        @remark 34
        @version NIIEngine 3.0.0
        */
        static const KeyValue G;

        /** h键
        @remark 35
        @version NIIEngine 3.0.0
        */
        static const KeyValue H;

        /** j键
        @remark 36
        @version NIIEngine 3.0.0
        */
        static const KeyValue J;

        /** k键
        @remark 37
        @version NIIEngine 3.0.0
        */
        static const KeyValue K;

        /** l键
        @remark 38
        @version NIIEngine 3.0.0
        */
        static const KeyValue L;

        /** ; 和 : 键(主键盘上)
        @remark 39
        @version NIIEngine 3.0.0
        */
        static const KeyValue Semicolon;

        /** ' 和 " 键(主键盘上)
        @remark 40
        @version NIIEngine 3.0.0
        */
        static const KeyValue Apostrophe;

        /** ` 和 ~ 键(主键盘上)
        @remark 41
        @version NIIEngine 3.0.0
        */
        static const KeyValue Grave;

        /** 左shift键(主键盘上)
        @remark 42
        @version NIIEngine 3.0.0
        */
        static const KeyValue LShift;

        /** \和|键(102主键盘上)
        @remark 43
        @version NIIEngine 3.0.0
        */
        static const KeyValue BackSlash;

        /** z键
        @remark 44
        @version NIIEngine 3.0.0
        */
        static const KeyValue Z;

        /** x键
        @remark 45
        @version NIIEngine 3.0.0
        */
        static const KeyValue X;

        /** c键
        @remark 46
        @version NIIEngine 3.0.0
        */
        static const KeyValue C;

        /** v键
        @remark 47
        @version NIIEngine 3.0.0
        */
        static const KeyValue V;

        /** b键
        @remark 48
        @version NIIEngine 3.0.0
        */
        static const KeyValue B;

        /** n键
        @remark 49
        @version NIIEngine 3.0.0
        */
        static const KeyValue N;

        /** m键
        @remark 50
        @version NIIEngine 3.0.0
        */
        static const KeyValue M;

        /** , 和 < 键(主键盘上)
        @remark 51
        @version NIIEngine 3.0.0
        */
        static const KeyValue Comma;

        /** . 和 > 键(主键盘上)
        @remark 52
        @version NIIEngine 3.0.0
        */
        static const KeyValue Period;

        /** / 和 ? 键(主键盘上)
        @remark 53
        @version NIIEngine 3.0.0
        */
        static const KeyValue Slash;

        /** 右shift键(主键盘上)
        @remark 54
        @version NIIEngine 3.0.0
        */
        static const KeyValue RShift;

        /** *键(数字键盘)
        @remark 55
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Mul;

        /** 左alt键(主键盘上)
        @remark 56
        @version NIIEngine 3.0.0
        */
        static const KeyValue LAlt;

        /** Space键
        @remark 57
        @version NIIEngine 3.0.0
        */
        static const KeyValue Space;

        /** CapsLock键
        @remark 58
        @version NIIEngine 3.0.0
        */
        static const KeyValue CapsLock;

        /** F1键
        @remark 59
        @version NIIEngine 3.0.0
        */
        static const KeyValue F1;

        /** F2键
        @remark 60
        @version NIIEngine 3.0.0
        */
        static const KeyValue F2;

        /** F3键
        @remark 61
        @version NIIEngine 3.0.0
        */
        static const KeyValue F3;

        /** F4键
        @remark 62
        @version NIIEngine 3.0.0
        */
        static const KeyValue F4;

        /** F5键
        @remark 63
        @version NIIEngine 3.0.0
        */
        static const KeyValue F5;

        /** F6键
        @remark 64
        @version NIIEngine 3.0.0
        */
        static const KeyValue F6;

        /** F7键
        @remark 65
        @version NIIEngine 3.0.0
        */
        static const KeyValue F7;

        /** F8键
        @remark 66
        @version NIIEngine 3.0.0
        */
        static const KeyValue F8;

        /** F9键
        @remark 67
        @version NIIEngine 3.0.0
        */
        static const KeyValue F9;

        /** F10键
        @remark 68
        @version NIIEngine 3.0.0
        */
        static const KeyValue F10;

        /** NumLock键
        @remark 69
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Lock;

        /** ScrollLock键
        @remark 70
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_ScrLock;

        /** 数字7键(数字盘上)
        @remark 71
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO7;

        /** 数字8键(数字盘上)
        @remark 72
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO8;

        /** 数字9键(数字盘上)
        @remark 73
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO9;

        /** -键(数字盘上)
        @remark 74
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Subtract;

        /** 数字4键(数字盘上)
        @remark 75
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO4;

        /** 数字5键(数字盘上)
        @remark 76
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO5;

        /** 数字6键(数字盘上)
        @remark 77
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO6;

        /** +键(数字盘上)
        @remark 78
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Add;

        /** 数字1键(数字盘上)
        @remark 79
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO1;

        /** 数字2键(数字盘上)
        @remark 80
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO2;

        /** 数字3键(数字盘上)
        @remark 81
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO3;

        /** Ins和数字0键(数字盘上)
        @remark 82
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_NO0;

        /** . 和 Del 键(数字盘上)
        @remark 83 几乎所有的驱动到这里为止都是相同的
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Dot;

        /** F11键(101+主键盘上)
        @remark 84
        @version NIIEngine 3.0.0
        */
        static const KeyValue F11;

        /** F12键(101+主键盘上)
        @remark 85
        @version NIIEngine 3.0.0
        */
        static const KeyValue F12;

        /** Enter键(数字盘上)
        @remark 86
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Enter;

        /** 右ctrl键(104主键盘上)
        @remark 87
        @version NIIEngine 3.0.0
        */
        static const KeyValue RCtrl;

        /** /键(数字盘上)
        @remark 88
        @version NIIEngine 3.0.0
        */
        static const KeyValue N_Divide;

        /** 复制屏幕图象(数字盘上)
        @remark 89
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_SysRQ;

        /** 右alt键(104主键盘上)
        @remark 90
        @version NIIEngine 3.0.0
        */
        static const KeyValue RAlt;

        /** 暂停件(数字盘上)
        @remark 91
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Pause;

        /** Home键(数字盘上)
        @remark 92
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Home;

        /** UpArrow键(数字盘上)
        @remark 93
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Up;

        /** PgUp键(数字盘上)
        @remark 94
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_PgUp;

        /** LeftArrow键(数字盘上)
        @remark 95
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Left;

        /** LeftArrow键(数字盘上)
        @remark 96
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Right;

        /** End键(数字盘上)
        @remark 97
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_End;

        /** DownArrow键(数字盘上)
        @remark 98
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Down;

        /** PgDn键(数字盘上)
        @remark 99
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_PgDown;

        /** Insert键(数字盘上)
        @remark 100
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Insert;

        /** Delete键(数字盘上)
        @remark 101
        @version NIIEngine 3.0.0
        */
        static const KeyValue M_Delete;

        /** 左Win键(数字盘上)
        @remark 102
        @version NIIEngine 3.0.0
        */
        static const KeyValue LWin;

        /** 右Win键(数字盘上)
        @remark 103
        @version NIIEngine 3.0.0
        */
        static const KeyValue RWin;

        /** AppMenu键(主键盘上)
        @remark 104
        @version NIIEngine 3.0.0
        */
        static const KeyValue Compose;

        /** 结束标识
        @remark 标识
        */
        static const KeyValue Max;
    };

    /** 鼠标事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyboardArgs : public EventArgs
    {
    public:
        KeyboardArgs(KeyValue k, bool state);
    public:
        KeyValue mKey;                          ///< 按键的编码
        bool mState;                            ///< ture press /false release
    };

    typedef void(KeyboardControlItem::*KeyboardMethod)(const KeyboardArgs *);
    
    /** 键盘序列成分
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

        KeyValue mKey;                       ///< 按键的编码
    };
    
    /** 键盘控制单元
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @par 104键盘标准
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyboardControlItem : public ControlItem
    {
        friend class KeyboardControlPattern;
    public:
        KeyboardControlItem();

        virtual ~KeyboardControlItem();

        /** 指定复合模式是否处于启用状态
        @remark 异步状态
        @param[in] type 需要检测的按键
        @version NIIEngine 3.0.0
        */
        bool isEnable(KeyboardAlterLockType type);

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    public:
        /** 需要时重写
        @remark ESC按键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onEsc(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字1键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字2键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字3键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字4键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字6键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字7键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字8键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字9键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字0键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void on0(const KeyboardArgs * arg);

        /** 需要时重写
        @remark - 和 _ 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMinus(const KeyboardArgs * arg);

        /** 需要时重写
        @remark = 和 + 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onEqual(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 空格键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onBK(const KeyboardArgs * arg);

        /** 需要时重写
        @remark tab键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onTAB(const KeyboardArgs * arg);

        /** 需要时重写
        @remark q键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onQ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark w键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onW(const KeyboardArgs * arg);

        /** 需要时重写
        @remark e键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onE(const KeyboardArgs * arg);

        /** 需要时重写
        @remark r键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onR(const KeyboardArgs * arg);

        /** 需要时重写
        @remark t键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onT(const KeyboardArgs * arg);

        /** 需要时重写
        @remark y键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onY(const KeyboardArgs * arg);

        /** 需要时重写
        @remark u键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onU(const KeyboardArgs * arg);

        /** 需要时重写
        @remark i键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onI(const KeyboardArgs * arg);

        /** 需要时重写
        @remark o键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onO(const KeyboardArgs * arg);

        /** 需要时重写
        @remark p键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onP(const KeyboardArgs * arg);

        /** 需要时重写
        @remark [ 和 { 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onLBracket(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ] 和 } 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRBracket(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Enter键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onEnter(const KeyboardArgs * arg);

        /** 左ctrl(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onLCtrl(const KeyboardArgs * arg);

        /** 需要时重写
        @remark a键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onA(const KeyboardArgs * arg);

        /** 需要时重写
        @remark s键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onS(const KeyboardArgs * arg);

        /** 需要时重写
        @remark d键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onD(const KeyboardArgs * arg);

        /** 需要时重写
        @remark f键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF(const KeyboardArgs * arg);

        /** 需要时重写
        @remark g键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onG(const KeyboardArgs * arg);

        /** 需要时重写
        @remark h键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onH(const KeyboardArgs * arg);

        /** 需要时重写
        @remark j键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onJ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark k键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onK(const KeyboardArgs * arg);

        /** 需要时重写
        @remark l键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onL(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ; 和 : 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onSemicolon(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ' 和 " 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onApostrophe(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ` 和 ~ 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onGrave(const KeyboardArgs * arg);

        /** 左shift键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onLShift(const KeyboardArgs * arg);

        /** 需要时重写
        @remark \ 和 | 键(102主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onBackSlash(const KeyboardArgs * arg);

        /** 需要时重写
        @remark z键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onZ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark x键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onX(const KeyboardArgs * arg);

        /** 需要时重写
        @remark c键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onC(const KeyboardArgs * arg);

        /** 需要时重写
        @remark v键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onV(const KeyboardArgs * arg);

        /** 需要时重写
        @remark b键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onB(const KeyboardArgs * arg);

        /** 需要时重写
        @remark n键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onN(const KeyboardArgs * arg);

        /** 需要时重写
        @remark m键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onM(const KeyboardArgs * arg);

        /** 需要时重写
        @remark , 和 < 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onComma(const KeyboardArgs * arg);

        /** 需要时重写
        @remark . 和 > 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPeriod(const KeyboardArgs * arg);

        /** 需要时重写
        @remark / 和 ? 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onSlash(const KeyboardArgs * arg);

        /** 右shift键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRShift(const KeyboardArgs * arg);

        /** 需要时重写
        @remark *键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNumMul(const KeyboardArgs * arg);

        /** 左alt键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onLAlt(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Space键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onSpace(const KeyboardArgs * arg);

        /** CapsLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onCapsLock(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F1键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F2键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F3键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F4键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F5键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F6键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F7键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F8键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F9键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F10键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF10(const KeyboardArgs * arg);

        /** NumLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNumLock(const KeyboardArgs * arg);

        /** ScrollLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidScrLock(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字7键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字8键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字9键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark -键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNumSubtract(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字4键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark +键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNumAdd(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字1键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字2键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字3键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Ins和数字0键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNum0(const KeyboardArgs * arg);

        /** 需要时重写
        @remark . 和 Del 键(数字盘上
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNumDot(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F11键(101+主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF11(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F12键(101+主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onF12(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Enter键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNumEnter(const KeyboardArgs * arg);

        /** 右ctrl键(104主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRCtrl(const KeyboardArgs * arg);

        /** 右alt键(104主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRAlt(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 左Win键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onLWin(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 右Win键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRWin(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 右Alt键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onCompose(const KeyboardArgs * arg);

        /** 需要时重写
        @remark /键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onNumDivide(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 复制屏幕图象(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidSysRQ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 暂停件(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidPause(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Home键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidHome(const KeyboardArgs * arg);

        /** 需要时重写
        @remark UpArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidUp(const KeyboardArgs * arg);

        /** 需要时重写
        @remark PgUp键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidPgUp(const KeyboardArgs * arg);

        /** 需要时重写
        @remark LeftArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidLeft(const KeyboardArgs * arg);

        /** 需要时重写
        @remark RightArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidRight(const KeyboardArgs * arg);

        /** 需要时重写
        @remark End键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidEnd(const KeyboardArgs * arg);

        /** 需要时重写
        @remark DownArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidDown(const KeyboardArgs * arg);

        /** 需要时重写
        @remark PgDn键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidPgDown(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Insert键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidInsert(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Delete键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onMidDelete(const KeyboardArgs * arg);
    protected:
        /** 初始化映射表
        @remark 由引擎完成(至少调用一次)
        */
        static void init();

        /** 当一个按键按下的时候触发
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPress(KeyValue key);

        /** 当一个按键释放的时候触发
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onRelease(KeyValue key);
    private:
        void OnDummyMethod(const KeyboardArgs * arg);
    protected:
        Nui8 mModifiers;                    ///< 复码缓存
    };

    /** 虚拟键盘控制单元
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

    /** 监听级键盘控制单元
    @remark
        用于配置自定义键盘按键映射
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerKeyboardControlItem : public KeyboardControlItem
    {
    public:
        ListenerKeyboardControlItem();
        virtual ~ListenerKeyboardControlItem();

        /// 获取监听到的键盘字符的ascii码
        inline NIIb getAsciiChar() const { return mChar; }

        /// 获取监听到的键盘字符的unicode码
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

    /** 事件级键盘控制单元
    @remark
        复杂型键盘控制单元,用于描述操作逻辑复杂的事件级对象,用于顶层设计中也可以方便
        事件连接.
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

    /** 外部设备控制处理器类(驱动)
    @remark 使用的是104标准的键盘设计
    @note
        由于大多系统内核读取键盘记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI KeyboardControlPattern : public ControlPattern
    {
    public:
        /** 打开指定键盘锁
        @remark 属于内部方法级别,明确调用会改变键盘状态
        @param[in] type 启用类型
        @version NIIEngine 3.0.0
        */
        void on(KeyboardAlterLockType type);

        /** 关闭指定键盘锁
        @remark 属于内部方法级别,明确调用会改变键盘状态
        @paramp[in] type 启用类型
        @version NIIEngine 3.0.0
        */
        void off(KeyboardAlterLockType type);

        /** 同步目标对象状态
        @remark 同步 capslock/scrolllock/numlock 等状态
        @version NIIEngine 3.0.0 高级api
        */
        virtual void syn(KeyboardAlterLockType type);

        /** 指定按键是否被按下
        @remark 异步状态
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isPress(KeyValue key);

        /** 指定按键是否已释放
        @remark 异步状态
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isRelease(KeyValue key);

        /** 获取实际按键字符文本
        @remark Unicode编码
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getKeyCode(KeyValue key);

        /** 获取实际按键字符描述
        @remark
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        virtual String getKeyText(KeyValue key);

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        /** 默认构造函数
        */
        KeyboardControlPattern();

        /** 默认构造函数
        @param[in] id 控制器的唯一标识,所有控制器中唯一(包含同类型,和不同类型的概念)
        @param[in] own 这个控制属于哪个窗体的
        */
        KeyboardControlPattern(Nid id, const ViewWindow * own);

        virtual ~KeyboardControlPattern();

        /** 激活按下编码
        @remark 这个方法引发按键事件,
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        void press(const KeyValue key);

        /** 释放按下编码
        @remark 这个方法引发按键事件
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        void release(const KeyValue key);

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();
    protected:
        KeyboardControlItem * mTarget;              ///< 当前对焦的控制单元
        Nui32 mBuffer[8];                           ///< 按键缓存
        Nui8 mModifiers;                            ///< 复码模式
    };

    /** 傀儡
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