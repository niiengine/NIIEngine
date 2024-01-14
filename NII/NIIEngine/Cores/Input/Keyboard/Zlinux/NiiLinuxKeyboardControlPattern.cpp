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

#include "NiiPreProcess.h"
#include "NiiLinuxKeyboardControlPattern.h"

#define EV_MAKE   1  // when key pressed
#define EV_BREAK  0  // when key released
#define EV_REPEAT 2  // when key switches to repeating after short delay

namespace NII
{
    #define test_bit(array, bit)  (array[(bit)/8] & (1 << ((bit)%8)))
    //------------------------------------------------------------------------
    const KeyValue Key::Esc             = 0x01; ///< ESC按键(主键盘上)    //1
	const KeyValue Key::NO1             = 0x02; ///< 数字1键(主键盘上)    //2
	const KeyValue Key::NO2             = 0x03; ///< 数字2键(主键盘上)    //3
	const KeyValue Key::NO3             = 0x04; ///< 数字3键(主键盘上)    //4
	const KeyValue Key::NO4             = 0x05; ///< 数字4键(主键盘上)    //5
	const KeyValue Key::NO5             = 0x06; ///< 数字5键(主键盘上)    //6
	const KeyValue Key::NO6             = 0x07; ///< 数字6键(主键盘上)    //7
	const KeyValue Key::NO7             = 0x08; ///< 数字7键(主键盘上)    //8
	const KeyValue Key::NO8             = 0x09; ///< 数字8键(主键盘上)    //9
	const KeyValue Key::NO9             = 0x0A; ///< 数字9键(主键盘上)    //10
	const KeyValue Key::NO0             = 0x0B; ///< 数字0键(主键盘上)    //11
	const KeyValue Key::Minus           = 0x0C; ///< - 和 _ 键(主键盘上)  //12
	const KeyValue Key::Equal           = 0x0D; ///< = 和 + 键(主键盘上)  //13
	const KeyValue Key::BK              = 0x0E; ///< 空格键(主键盘上)     //14
	const KeyValue Key::TAB             = 0x0F; ///< tab键(主键盘上)      //15
	const KeyValue Key::Q               = 0x10; ///< q键                 //16
	const KeyValue Key::W               = 0x11; ///< w键                 //17
	const KeyValue Key::E               = 0x12; ///< e键                 //18
	const KeyValue Key::R               = 0x13; ///< r键                 //19
	const KeyValue Key::T               = 0x14; ///< t键                 //20
	const KeyValue Key::Y               = 0x15; ///< y键                 //21
	const KeyValue Key::U               = 0x16; ///< u键                 //22
	const KeyValue Key::I               = 0x17; ///< i键                 //23
	const KeyValue Key::O               = 0x18; ///< o键                 //24
	const KeyValue Key::P               = 0x19; ///< p键                 //25
	const KeyValue Key::LBracket        = 0x1A; ///< [ 和 { 键(主键盘上)  //26
	const KeyValue Key::RBracket        = 0x1B; ///< ] 和 } 键(主键盘上)  //27
	const KeyValue Key::Enter           = 0x1C; ///< Enter键(主键盘上)    //28
	const KeyValue Key::LCtrl           = 0x1D; ///< 左ctrl(主键盘上)     //29
	const KeyValue Key::A               = 0x1E; ///< a键                 //30
	const KeyValue Key::S               = 0x1F; ///< s键                 //31
	const KeyValue Key::D               = 0x20; ///< d键                 //32
	const KeyValue Key::F               = 0x21; ///< f键                 //33
	const KeyValue Key::G               = 0x22; ///< g键                 //34
	const KeyValue Key::H               = 0x23; ///< h键                 //35
	const KeyValue Key::J               = 0x24; ///< j键                 //36
	const KeyValue Key::K               = 0x25; ///< k键                 //37
	const KeyValue Key::L               = 0x26; ///< l键                 //38
	const KeyValue Key::Semicolon       = 0x27; ///< ; 和 : 键(主键盘上)  //39
	const KeyValue Key::Apostrophe      = 0x28; ///< ' 和 " 键(主键盘上)  //40
	const KeyValue Key::Grave           = 0x29; ///< ` 和 ~ 键(主键盘上)  //41
	const KeyValue Key::LShift          = 0x2A; ///< 左shift键(主键盘上)  //42
	const KeyValue Key::BackSlash       = 0x2B; ///< \和|键(102主键盘上)  //43
	const KeyValue Key::Z               = 0x2C; ///< z键                 //44
	const KeyValue Key::X               = 0x2D; ///< x键                 //45
	const KeyValue Key::C               = 0x2E; ///< c键                 //46
	const KeyValue Key::V               = 0x2F; ///< v键                 //47
	const KeyValue Key::B               = 0x30; ///< b键                 //48
	const KeyValue Key::N               = 0x31; ///< n键                 //49
	const KeyValue Key::M               = 0x32; ///< m键                 //50
	const KeyValue Key::Comma           = 0x33; ///< , 和 < 键(主键盘上)  //51
	const KeyValue Key::Period          = 0x34; ///< . 和 > 键(主键盘上)  //52
	const KeyValue Key::Slash           = 0x35; ///< / 和 ? 键(主键盘上)  //53
	const KeyValue Key::RShift          = 0x36; ///< 右shift键(主键盘上)  //54
	const KeyValue Key::N_Mul           = 0x37; ///< *键(数字键盘)        //55
	const KeyValue Key::LAlt            = 0x38; ///< 左alt键(主键盘上)    //56
	const KeyValue Key::Space           = 0x39; ///< Space键             //57
	const KeyValue Key::CapsLock        = 0x3A; ///< CapsLock键          //58
	const KeyValue Key::F1              = 0x3B; ///< F1键                //59
	const KeyValue Key::F2              = 0x3C; ///< F2键                //60
	const KeyValue Key::F3              = 0x3D; ///< F3键                //61
	const KeyValue Key::F4              = 0x3E; ///< F4键                //62
	const KeyValue Key::F5              = 0x3F; ///< F5键                //63
	const KeyValue Key::F6              = 0x40; ///< F6键                //64
	const KeyValue Key::F7              = 0x41; ///< F7键                //65
	const KeyValue Key::F8              = 0x42; ///< F8键                //66
	const KeyValue Key::F9              = 0x43; ///< F9键                //67
	const KeyValue Key::F10             = 0x44; ///< F10键               //68
	const KeyValue Key::N_Lock          = 0x45; ///< NumLock键           //69
	const KeyValue Key::M_ScrLock       = 0x46; ///< ScrollLock键        //70
	const KeyValue Key::N_NO7           = 0x47; ///< 数字7键(数字盘上)    //71
	const KeyValue Key::N_NO8           = 0x48; ///< 数字8键(数字盘上)    //72
	const KeyValue Key::N_NO9           = 0x49; ///< 数字9键(数字盘上)    //73
	const KeyValue Key::N_Subtract      = 0x4A; ///< -键(数字盘上)        //74
	const KeyValue Key::N_NO4           = 0x4B; ///< 数字4键(数字盘上)    //75
	const KeyValue Key::N_NO5           = 0x4C; ///< 数字5键(数字盘上)    //76
	const KeyValue Key::N_NO6           = 0x4D; ///< 数字6键(数字盘上)    //77
	const KeyValue Key::N_Add           = 0x4E; ///< +键(数字盘上)        //78
	const KeyValue Key::N_NO1           = 0x4F; ///< 数字1键(数字盘上)    //79
	const KeyValue Key::N_NO2           = 0x50; ///< 数字2键(数字盘上)    //80
	const KeyValue Key::N_NO3           = 0x51; ///< 数字3键(数字盘上)    //81
	const KeyValue Key::N_NO0           = 0x52; ///< Ins和数字0键(数字盘上)//82
	const KeyValue Key::N_Dot           = 0x53; ///< . 和 Del 键(数字盘上)//83
	const KeyValue Key::F11             = 0x57; ///< F11键(101+主键盘上)  //84
	const KeyValue Key::F12             = 0x58; ///< F12键(101+主键盘上)  //85
	const KeyValue Key::N_Enter         = 0x60; ///< Enter键(数字盘上)    //86
    const KeyValue Key::RCtrl           = 0x61; ///< 右ctrl键(104主键盘上)//87
	const KeyValue Key::N_Divide        = 0x62; ///< /键(数字盘上)         //88
	const KeyValue Key::M_SysRQ         = 0x63; ///< 复制屏幕图象(数字盘上) //89
    const KeyValue Key::RAlt            = 0x64; ///< 右alt键(104主键盘上)  //90
	const KeyValue Key::M_Pause         = 0x77; ///< 暂停件(数字盘上)      //91
	const KeyValue Key::M_Home          = 0x66; ///< Home键(数字盘上)      //92
	const KeyValue Key::M_Up            = 0x67; ///< UpArrow键(数字盘上)   //93
	const KeyValue Key::M_PgUp          = 0x68; ///< PgUp键(数字盘上)      //94
	const KeyValue Key::M_Left          = 0x69; ///< LeftArrow键(数字盘上) //95
	const KeyValue Key::M_Right         = 0x6A; ///< RightArrow键(数字盘上)//96
	const KeyValue Key::M_End           = 0x6B; ///< End键(数字盘上)       //97
	const KeyValue Key::M_Down          = 0x6C; ///< DownArrow键(数字盘上) //98
	const KeyValue Key::M_PgDown        = 0x6D; ///< PgDn键(数字盘上)      //99
	const KeyValue Key::M_Insert        = 0x6E; ///< Insert键(数字盘上)    //100
	const KeyValue Key::M_Delete        = 0x6F; ///< Delete键(数字盘上)    //101
	const KeyValue Key::LWin            = 0x7D; ///< 左Win键(主键盘上)     //102
    const KeyValue Key::RWin            = 0x7E; ///< 右Win键(主键盘上)     //103
	const KeyValue Key::Compose         = 0x7F; ///< AppMenu键(主键盘上)   //104
    const KeyValue Key::Max             = 0x80;
    //------------------------------------------------------------------------
    LinuxKeyboardControlPattern::LinuxKeyboardControlPattern(Nid id, NIIi main) :
        KeyboardControlPattern(id),
        mKeyboard(main),
        mVTKeyboard(-1)
    {

    }
    //------------------------------------------------------------------------
    LinuxKeyboardControlPattern::~LinuxKeyboardControlPattern()
    {
        if(mKeyboard >= 0)
            close(mKeyboard);

        if(mVTKeyboard >= 0)
            close(mVTKeyboard);
    }
    //------------------------------------------------------------------------
    void LinuxKeyboardControlPattern::_init()
    {
        if(mKeyboard == -1)
            N_EXCEPT(CreateDevice, _T("Could not init keyboard device!"));

        mVTKeyboard = open("/dev/tty0", O_RDWR | O_NOCTTY);

        if (mVTKeyboard == -1)
            N_EXCEPT(CreateDevice, _T("Could not init vt keyboard device!"));

        syn(KT_L_NumLock);
        syn(KT_L_CapsLock);
        syn(KT_L_ScrLock);
    }
    //------------------------------------------------------------------------
    void LinuxKeyboardControlPattern::_update()
    {
        if(mActive)
        {
            struct input_event raw[KEYBOARD_BUFFERSIZE];
            KeyValue kc;
            NCount i;
            ssize_t count = read(mKeyboard, raw, sizeof(raw)) / sizeof(struct input_event);
            for(i = 0; i < count; ++i)
            {
                switch(raw[i].type)
                {
                case EV_KEY :
                    kc = raw[i].code;
                    if(raw[i].value == EV_MAKE)
                    {
                        press(kc);
                    }
                    else if(raw[i].value == EV_BREAK)
                    {
                        release(kc);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void LinuxKeyboardControlPattern::_reset()
    {
        // donothing
    }
    //------------------------------------------------------------------------
    void LinuxKeyboardControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void LinuxKeyboardControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void LinuxKeyboardControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    Nui8 LinuxKeyboardControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    Nui8 LinuxKeyboardControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    Nui32 LinuxKeyboardControlPattern::getKeyCode(KeyValue key)
    {
        int code = key;
        Nui16 value;
        //DFBInputDeviceKeyIdentifier identifier;

        if(mVTKeyboard < 0)
            return false;

        // to support '+'  and '/' with Boxee's remote control we do something ugly like this for now
        if (KVAL(code) == 98)
        {
            code = K(KTYP(code), 53);
        }

        if((mModifiers & KT_A_Shift & ~KT_L_CapsLock) || (mModifiers & ~KT_A_Shift & KT_L_CapsLock))
            return getSymbol(K_SHIFTTAB, key));
        if(mModifiers & KT_A_Alt)
            return getSymbol(K_ALTTAB, key));
        if(mModifiers & KT_A_Meta)
            return getSymbol(K_ALTSHIFTTAB, key));

        // to support '+'  and '/' with Boxee's remote control we could do ugly something like this for now
        if (KVAL(code) == 78)
        {
            return K(KTYP(code), 43);
        }
        else
        {
            return getSymbol(K_NORMTAB, code));
        }
    }
    //------------------------------------------------------------------------
    PatternID LinuxKeyboardControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    Nui32 LinuxKeyboardControlPattern::getSymbol(Nui8 table, Nui8 index1)
    {
        struct kbentry entry;
        Nui16 value;

        entry.kb_table = table;
        entry.kb_index = index1;
        entry.kb_value = 0;

        if (ioctl(mVTKeyboard, KDGKBENT, &entry))
            value = 0;
        else
            value = entry.kb_value;
        Nui8 type = KTYP(value);
        Nui8 index2 = KVAL(value);

        switch (type)
        {
        case KT_FN:
            break;
        case KT_LETTER:
        case KT_LATIN:
            switch (index2)
            {
            case 0x7f:
                value = 0x08;
                break;
            default:
                value = index2;
                break;
            }
            break;
        }

        if(value < 127)
            return value;

        return 0;
    }
    //------------------------------------------------------------------------
    void LinuxKeyboardControlPattern::syn(KeyboardAlterLockType type) const
    {
        struct input_event levt;
        levt.type = EV_LED;
        bool set = mModifiers & type;
        switch(type)
        {
        case KT_L_NumLock :
            levt.code = LED_NUML;
            if(set)
            {
                levt.value = false;
                off(KT_L_NumLock);
            }
            else
            {
                levt.value = true;
                on(KT_L_NumLock);
            }
            break;
        case KT_L_CapsLock :
            levt.code = LED_CAPSL;
            if(set)
            {
                levt.value = false;
                off(KT_L_CapsLock);
            }
            else
            {
                levt.value = true;
                on(KT_L_CapsLock);
            }
        case KT_L_ScrLock :
            levt.code = LED_SCROLLL;
            if(set)
            {
                levt.value = false;
                off(KT_L_ScrLock);
            }
            else
            {
                levt.value = true;
                on(KT_L_ScrLock);
            }
        }
        write(mKeyboard, &levt, sizeof(levt));
    }
    //------------------------------------------------------------------------
}