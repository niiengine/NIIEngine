/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-23

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_KEYBOARD_CONTROLITEM_H_
#define _NII_KEYBOARD_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlItem.h"
#include "NiiMouseControlItem.h"

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
        @remark 火星人使用的
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
        /** 键盘控制器事件
        @param[in] k
        */
        KeyboardArgs(KeyValue k);

        KeyValue mKey;                          ///< 按键的编码
    };

    typedef void(KeyboardControlItem::*KeyboardMethod)(const KeyboardArgs *);
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
        virtual void onPressEsc(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ESC按键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseEsc(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字1键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字1键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字2键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字2键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字3键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字3键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字4键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字4键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字6键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字6键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字7键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字7键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字8键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字8键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字9键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字9键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字0键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPress0(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字0键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onRelease0(const KeyboardArgs * arg);

        /** 需要时重写
        @remark - 和 _ 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMinus(const KeyboardArgs * arg);

        /** 需要时重写
        @remark - 和 _ 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMinus(const KeyboardArgs * arg);

        /** 需要时重写
        @remark = 和 + 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressEqual(const KeyboardArgs * arg);

        /** 需要时重写
        @remark = 和 + 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseEqual(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 空格键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressBK(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 空格键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseBK(const KeyboardArgs * arg);

        /** 需要时重写
        @remark tab键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressTAB(const KeyboardArgs * arg);

        /** 需要时重写
        @remark tab键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseTAB(const KeyboardArgs * arg);

        /** 需要时重写
        @remark q键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressQ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark q键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseQ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark w键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressW(const KeyboardArgs * arg);

        /** 需要时重写
        @remark w键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseW(const KeyboardArgs * arg);

        /** 需要时重写
        @remark e键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressE(const KeyboardArgs * arg);

        /** 需要时重写
        @remark e键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseE(const KeyboardArgs * arg);

        /** 需要时重写
        @remark r键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressR(const KeyboardArgs * arg);

        /** 需要时重写
        @remark r键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseR(const KeyboardArgs * arg);

        /** 需要时重写
        @remark t键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressT(const KeyboardArgs * arg);

        /** 需要时重写
        @remark t键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseT(const KeyboardArgs * arg);

        /** 需要时重写
        @remark y键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressY(const KeyboardArgs * arg);

        /** 需要时重写
        @remark y键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseY(const KeyboardArgs * arg);

        /** 需要时重写
        @remark u键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressU(const KeyboardArgs * arg);

        /** 需要时重写
        @remark u键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseU(const KeyboardArgs * arg);

        /** 需要时重写
        @remark i键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressI(const KeyboardArgs * arg);

        /** 需要时重写
        @remark i键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseI(const KeyboardArgs * arg);

        /** 需要时重写
        @remark o键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressO(const KeyboardArgs * arg);

        /** 需要时重写
        @remark o键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseO(const KeyboardArgs * arg);

        /** 需要时重写
        @remark p键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressP(const KeyboardArgs * arg);

        /** 需要时重写
        @remark p键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseP(const KeyboardArgs * arg);

        /** 需要时重写
        @remark [ 和 { 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressLBracket(const KeyboardArgs * arg);

        /** 需要时重写
        @remark [ 和 { 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLBracket(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ] 和 } 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressRBracket(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ] 和 } 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRBracket(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Enter键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressEnter(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Enter键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseEnter(const KeyboardArgs * arg);

        /** 左ctrl(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressLCtrl(const KeyboardArgs * arg);

        /** 左ctrl(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLCtrl(const KeyboardArgs * arg);

        /** 需要时重写
        @remark a键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressA(const KeyboardArgs * arg);

        /** 需要时重写
        @remark a键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseA(const KeyboardArgs * arg);

        /** 需要时重写
        @remark s键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressS(const KeyboardArgs * arg);

        /** 需要时重写
        @remark s键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseS(const KeyboardArgs * arg);

        /** 需要时重写
        @remark d键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressD(const KeyboardArgs * arg);

        /** 需要时重写
        @remark d键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseD(const KeyboardArgs * arg);

        /** 需要时重写
        @remark f键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF(const KeyboardArgs * arg);

        /** 需要时重写
        @remark f键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF(const KeyboardArgs * arg);

        /** 需要时重写
        @remark g键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressG(const KeyboardArgs * arg);

        /** 需要时重写
        @remark g键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseG(const KeyboardArgs * arg);

        /** 需要时重写
        @remark h键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressH(const KeyboardArgs * arg);

        /** 需要时重写
        @remark h键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseH(const KeyboardArgs * arg);

        /** 需要时重写
        @remark j键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressJ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark j键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseJ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark k键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressK(const KeyboardArgs * arg);

        /** 需要时重写
        @remark k键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseK(const KeyboardArgs * arg);

        /** 需要时重写
        @remark l键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressL(const KeyboardArgs * arg);

        /** 需要时重写
        @remark l键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseL(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ; 和 : 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressSemicolon(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ; 和 : 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseSemicolon(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ' 和 " 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressApostrophe(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ' 和 " 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseApostrophe(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ` 和 ~ 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressGrave(const KeyboardArgs * arg);

        /** 需要时重写
        @remark ` 和 ~ 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseGrave(const KeyboardArgs * arg);

        /** 左shift键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressLShift(const KeyboardArgs * arg);

        /** 左shift键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLShift(const KeyboardArgs * arg);

        /** 需要时重写
        @remark \ 和 | 键(102主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressBackSlash(const KeyboardArgs * arg);

        /** 需要时重写
        @remark \ 和 | 键(102主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseBackSlash(const KeyboardArgs * arg);

        /** 需要时重写
        @remark z键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressZ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark z键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseZ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark x键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressX(const KeyboardArgs * arg);

        /** 需要时重写
        @remark x键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseX(const KeyboardArgs * arg);

        /** 需要时重写
        @remark c键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressC(const KeyboardArgs * arg);

        /** 需要时重写
        @remark c键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseC(const KeyboardArgs * arg);

        /** 需要时重写
        @remark v键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressV(const KeyboardArgs * arg);

        /** 需要时重写
        @remark v键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseV(const KeyboardArgs * arg);

        /** 需要时重写
        @remark b键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressB(const KeyboardArgs * arg);

        /** 需要时重写
        @remark b键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseB(const KeyboardArgs * arg);

        /** 需要时重写
        @remark n键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressN(const KeyboardArgs * arg);

        /** 需要时重写
        @remark n键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseN(const KeyboardArgs * arg);

        /** 需要时重写
        @remark m键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressM(const KeyboardArgs * arg);

        /** 需要时重写
        @remark m键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseM(const KeyboardArgs * arg);

        /** 需要时重写
        @remark , 和 < 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressComma(const KeyboardArgs * arg);

        /** 需要时重写
        @remark , 和 < 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseComma(const KeyboardArgs * arg);

        /** 需要时重写
        @remark . 和 > 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressPeriod(const KeyboardArgs * arg);

        /** 需要时重写
        @remark . 和 > 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleasePeriod(const KeyboardArgs * arg);

        /** 需要时重写
        @remark / 和 ? 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressSlash(const KeyboardArgs * arg);

        /** 需要时重写
        @remark / 和 ? 键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseSlash(const KeyboardArgs * arg);

        /** 右shift键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressRShift(const KeyboardArgs * arg);

        /** 右shift键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRShift(const KeyboardArgs * arg);

        /** 需要时重写
        @remark *键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumMul(const KeyboardArgs * arg);

        /** 需要时重写
        @remark *键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumMul(const KeyboardArgs * arg);

        /** 左alt键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressLAlt(const KeyboardArgs * arg);

        /** 左alt键(主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLAlt(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Space键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressSpace(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Space键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseSpace(const KeyboardArgs * arg);

        /** CapsLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressCapsLock(const KeyboardArgs * arg);

        /** CapsLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseCapsLock(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F1键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F1键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F2键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F2键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F3键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F3键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F4键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F4键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F5键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F5键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F6键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F6键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F7键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F7键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F8键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F8键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F9键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F9键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F10键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF10(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F10键
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF10(const KeyboardArgs * arg);

        /** NumLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumLock(const KeyboardArgs * arg);

        /** NumLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumLock(const KeyboardArgs * arg);

        /** ScrollLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidScrLock(const KeyboardArgs * arg);

        /** ScrollLock键
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidScrLock(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字7键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字7键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum7(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字8键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字8键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum8(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字9键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字9键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum9(const KeyboardArgs * arg);

        /** 需要时重写
        @remark -键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumSubtract(const KeyboardArgs * arg);

        /** 需要时重写
        @remark -键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumSubtract(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字4键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字4键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0@version NIIEngine 3.0.0
        */
        virtual void onReleaseNum4(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum5(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字5键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum6(const KeyboardArgs * arg);

        /** 需要时重写
        @remark +键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumAdd(const KeyboardArgs * arg);

        /** 需要时重写
        @remark +键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumAdd(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字1键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字1键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum1(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字2键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字2键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum2(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字3键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 数字3键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum3(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Ins和数字0键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNum0(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Ins和数字0键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNum0(const KeyboardArgs * arg);

        /** 需要时重写
        @remark . 和 Del 键(数字盘上
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumDot(const KeyboardArgs * arg);

        /** 需要时重写
        @remark . 和 Del 键(数字盘上
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumDot(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F11键(101+主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF11(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F11键(101+主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF11(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F12键(101+主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressF12(const KeyboardArgs * arg);

        /** 需要时重写
        @remark F12键(101+主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseF12(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Enter键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumEnter(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Enter键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumEnter(const KeyboardArgs * arg);

        /** 右ctrl键(104主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressRCtrl(const KeyboardArgs * arg);

        /** 右ctrl键(104主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRCtrl(const KeyboardArgs * arg);

        /** 右alt键(104主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressRAlt(const KeyboardArgs * arg);

        /** 右alt键(104主键盘上)
        @remark 这个方法必须继承
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRAlt(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 左Win键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressLWin(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 左Win键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLWin(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 右Win键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressRWin(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 右Win键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRWin(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 右Alt键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressCompose(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 右Alt键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseCompose(const KeyboardArgs * arg);

        /** 需要时重写
        @remark /键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressNumDivide(const KeyboardArgs * arg);

        /** 需要时重写
        @remark /键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseNumDivide(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 复制屏幕图象(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidSysRQ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 复制屏幕图象(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidSysRQ(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 暂停件(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidPause(const KeyboardArgs * arg);

        /** 需要时重写
        @remark 暂停件(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidPause(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Home键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidHome(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Home键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidHome(const KeyboardArgs * arg);

        /** 需要时重写
        @remark UpArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidUp(const KeyboardArgs * arg);

        /** 需要时重写
        @remark UpArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidUp(const KeyboardArgs * arg);

        /** 需要时重写
        @remark PgUp键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidPgUp(const KeyboardArgs * arg);

        /** 需要时重写
        @remark PgUp键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidPgUp(const KeyboardArgs * arg);

        /** 需要时重写
        @remark LeftArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidLeft(const KeyboardArgs * arg);

        /** 需要时重写
        @remark LeftArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidLeft(const KeyboardArgs * arg);

        /** 需要时重写
        @remark RightArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidRight(const KeyboardArgs * arg);

        /** 需要时重写
        @remark RightArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidRight(const KeyboardArgs * arg);

        /** 需要时重写
        @remark End键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidEnd(const KeyboardArgs * arg);

        /** 需要时重写
        @remark End键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidEnd(const KeyboardArgs * arg);

        /** 需要时重写
        @remark DownArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidDown(const KeyboardArgs * arg);

        /** 需要时重写
        @remark DownArrow键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidDown(const KeyboardArgs * arg);

        /** 需要时重写
        @remark PgDn键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidPgDown(const KeyboardArgs * arg);

        /** 需要时重写
        @remark PgDn键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidPgDown(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Insert键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidInsert(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Insert键(数字盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidInsert(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Delete键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onPressMidDelete(const KeyboardArgs * arg);

        /** 需要时重写
        @remark Delete键(主键盘上)
        @par 104键盘标准
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMidDelete(const KeyboardArgs * arg);
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

    /**
    @version NIIEngine 3.0.0
    */
    class DummyKeyboardControlItem : public KeyboardControlItem, public ControlAlloc
    {
    public:
        DummyKeyboardControlItem() {}
    };
}
}
#endif