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
#include "NiiWin32KeyboardControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    const KeyValue Key::Esc             = 0x01; ///< ESC°´¼ü(Ö÷¼üÅÌÉÏ)    //1
	const KeyValue Key::NO1             = 0x02; ///< Êý×Ö1¼ü(Ö÷¼üÅÌÉÏ)    //2
	const KeyValue Key::NO2             = 0x03; ///< Êý×Ö2¼ü(Ö÷¼üÅÌÉÏ)    //3
	const KeyValue Key::NO3             = 0x04; ///< Êý×Ö3¼ü(Ö÷¼üÅÌÉÏ)    //4
	const KeyValue Key::NO4             = 0x05; ///< Êý×Ö4¼ü(Ö÷¼üÅÌÉÏ)    //5
	const KeyValue Key::NO5             = 0x06; ///< Êý×Ö5¼ü(Ö÷¼üÅÌÉÏ)    //6
	const KeyValue Key::NO6             = 0x07; ///< Êý×Ö6¼ü(Ö÷¼üÅÌÉÏ)    //7
	const KeyValue Key::NO7             = 0x08; ///< Êý×Ö7¼ü(Ö÷¼üÅÌÉÏ)    //8
	const KeyValue Key::NO8             = 0x09; ///< Êý×Ö8¼ü(Ö÷¼üÅÌÉÏ)    //9
	const KeyValue Key::NO9             = 0x0A; ///< Êý×Ö9¼ü(Ö÷¼üÅÌÉÏ)    //10
	const KeyValue Key::NO0             = 0x0B; ///< Êý×Ö0¼ü(Ö÷¼üÅÌÉÏ)    //11
	const KeyValue Key::Minus           = 0x0C; ///< - ºÍ _ ¼ü(Ö÷¼üÅÌÉÏ)  //12
	const KeyValue Key::Equal           = 0x0D; ///< = ºÍ + ¼ü(Ö÷¼üÅÌÉÏ)  //13
	const KeyValue Key::BK              = 0x0E; ///< ¿Õ¸ñ¼ü(Ö÷¼üÅÌÉÏ)     //14
	const KeyValue Key::TAB             = 0x0F; ///< tab¼ü(Ö÷¼üÅÌÉÏ)      //15
	const KeyValue Key::Q               = 0x10; ///< q¼ü                 //16
	const KeyValue Key::W               = 0x11; ///< w¼ü                 //17
	const KeyValue Key::E               = 0x12; ///< e¼ü                 //18
	const KeyValue Key::R               = 0x13; ///< r¼ü                 //19
	const KeyValue Key::T               = 0x14; ///< t¼ü                 //20
	const KeyValue Key::Y               = 0x15; ///< y¼ü                 //21
	const KeyValue Key::U               = 0x16; ///< u¼ü                 //22
	const KeyValue Key::I               = 0x17; ///< i¼ü                 //23
	const KeyValue Key::O               = 0x18; ///< o¼ü                 //24
	const KeyValue Key::P               = 0x19; ///< p¼ü                 //25
	const KeyValue Key::LBracket        = 0x1A; ///< [ ºÍ { ¼ü(Ö÷¼üÅÌÉÏ)  //26
	const KeyValue Key::RBracket        = 0x1B; ///< ] ºÍ } ¼ü(Ö÷¼üÅÌÉÏ)  //27
	const KeyValue Key::Enter           = 0x1C; ///< Enter¼ü(Ö÷¼üÅÌÉÏ)    //28
	const KeyValue Key::LCtrl           = 0x1D; ///< ×óctrl(Ö÷¼üÅÌÉÏ)     //29
	const KeyValue Key::A               = 0x1E; ///< a¼ü                 //30
	const KeyValue Key::S               = 0x1F; ///< s¼ü                 //31
	const KeyValue Key::D               = 0x20; ///< d¼ü                 //32
	const KeyValue Key::F               = 0x21; ///< f¼ü                 //33
	const KeyValue Key::G               = 0x22; ///< g¼ü                 //34
	const KeyValue Key::H               = 0x23; ///< h¼ü                 //35
	const KeyValue Key::J               = 0x24; ///< j¼ü                 //36
	const KeyValue Key::K               = 0x25; ///< k¼ü                 //37
	const KeyValue Key::L               = 0x26; ///< l¼ü                 //38
	const KeyValue Key::Semicolon       = 0x27; ///< ; ºÍ : ¼ü(Ö÷¼üÅÌÉÏ)  //39
	const KeyValue Key::Apostrophe      = 0x28; ///< ' ºÍ " ¼ü(Ö÷¼üÅÌÉÏ)  //40
	const KeyValue Key::Grave           = 0x29; ///< ` ºÍ ~ ¼ü(Ö÷¼üÅÌÉÏ)  //41
	const KeyValue Key::LShift          = 0x2A; ///< ×óshift¼ü(Ö÷¼üÅÌÉÏ)  //42
	const KeyValue Key::BackSlash       = 0x2B; ///< \ºÍ|¼ü(102Ö÷¼üÅÌÉÏ)  //43
	const KeyValue Key::Z               = 0x2C; ///< z¼ü                 //44
	const KeyValue Key::X               = 0x2D; ///< x¼ü                 //45
	const KeyValue Key::C               = 0x2E; ///< c¼ü                 //46
	const KeyValue Key::V               = 0x2F; ///< v¼ü                 //47
	const KeyValue Key::B               = 0x30; ///< b¼ü                 //48
	const KeyValue Key::N               = 0x31; ///< n¼ü                 //49
	const KeyValue Key::M               = 0x32; ///< m¼ü                 //50
	const KeyValue Key::Comma           = 0x33; ///< , ºÍ < ¼ü(Ö÷¼üÅÌÉÏ)  //51
	const KeyValue Key::Period          = 0x34; ///< . ºÍ > ¼ü(Ö÷¼üÅÌÉÏ)  //52
	const KeyValue Key::Slash           = 0x35; ///< / ºÍ ? ¼ü(Ö÷¼üÅÌÉÏ)  //53
	const KeyValue Key::RShift          = 0x36; ///< ÓÒshift¼ü(Ö÷¼üÅÌÉÏ)  //54
	const KeyValue Key::N_Mul           = 0x37; ///< *¼ü(Êý×Ö¼üÅÌ)        //55
	const KeyValue Key::LAlt            = 0x38; ///< ×óalt¼ü(Ö÷¼üÅÌÉÏ)    //56
	const KeyValue Key::Space           = 0x39; ///< Space¼ü             //57
	const KeyValue Key::CapsLock        = 0x3A; ///< CapsLock¼ü          //58
	const KeyValue Key::F1              = 0x3B; ///< F1¼ü                //59
	const KeyValue Key::F2              = 0x3C; ///< F2¼ü                //60
	const KeyValue Key::F3              = 0x3D; ///< F3¼ü                //61
	const KeyValue Key::F4              = 0x3E; ///< F4¼ü                //62
	const KeyValue Key::F5              = 0x3F; ///< F5¼ü                //63
	const KeyValue Key::F6              = 0x40; ///< F6¼ü                //64
	const KeyValue Key::F7              = 0x41; ///< F7¼ü                //65
	const KeyValue Key::F8              = 0x42; ///< F8¼ü                //66
	const KeyValue Key::F9              = 0x43; ///< F9¼ü                //67
	const KeyValue Key::F10             = 0x44; ///< F10¼ü               //68
	const KeyValue Key::N_Lock          = 0x45; ///< NumLock¼ü           //69
	const KeyValue Key::M_ScrLock       = 0x46; ///< ScrollLock¼ü        //70
	const KeyValue Key::N_NO7           = 0x47; ///< Êý×Ö7¼ü(Êý×ÖÅÌÉÏ)    //71
	const KeyValue Key::N_NO8           = 0x48; ///< Êý×Ö8¼ü(Êý×ÖÅÌÉÏ)    //72
	const KeyValue Key::N_NO9           = 0x49; ///< Êý×Ö9¼ü(Êý×ÖÅÌÉÏ)    //73
	const KeyValue Key::N_Subtract      = 0x4A; ///< -¼ü(Êý×ÖÅÌÉÏ)        //74
	const KeyValue Key::N_NO4           = 0x4B; ///< Êý×Ö4¼ü(Êý×ÖÅÌÉÏ)    //75
	const KeyValue Key::N_NO5           = 0x4C; ///< Êý×Ö5¼ü(Êý×ÖÅÌÉÏ)    //76
	const KeyValue Key::N_NO6           = 0x4D; ///< Êý×Ö6¼ü(Êý×ÖÅÌÉÏ)    //77
	const KeyValue Key::N_Add           = 0x4E; ///< +¼ü(Êý×ÖÅÌÉÏ)        //78
	const KeyValue Key::N_NO1           = 0x4F; ///< Êý×Ö1¼ü(Êý×ÖÅÌÉÏ)    //79
	const KeyValue Key::N_NO2           = 0x50; ///< Êý×Ö2¼ü(Êý×ÖÅÌÉÏ)    //80
	const KeyValue Key::N_NO3           = 0x51; ///< Êý×Ö3¼ü(Êý×ÖÅÌÉÏ)    //81
	const KeyValue Key::N_NO0           = 0x52; ///< InsºÍÊý×Ö0¼ü(Êý×ÖÅÌÉÏ)//82
	const KeyValue Key::N_Dot           = 0x53; ///< . ºÍ Del ¼ü(Êý×ÖÅÌÉÏ)//83
	const KeyValue Key::F11             = 0x57; ///< F11¼ü(101+Ö÷¼üÅÌÉÏ)  //84
	const KeyValue Key::F12             = 0x58; ///< F12¼ü(101+Ö÷¼üÅÌÉÏ)  //85
	const KeyValue Key::N_Enter         = 0x9C; ///< Enter¼ü(Êý×ÖÅÌÉÏ)    //86
    const KeyValue Key::RCtrl           = 0x9D; ///< ÓÒctrl¼ü(104Ö÷¼üÅÌÉÏ)//87
	const KeyValue Key::N_Divide        = 0xB5; ///< /¼ü(Êý×ÖÅÌÉÏ)         //88
	const KeyValue Key::M_SysRQ         = 0xB7; ///< ¸´ÖÆÆÁÄ»Í¼Ïó(Êý×ÖÅÌÉÏ) //89
    const KeyValue Key::RAlt            = 0xB8; ///< ÓÒalt¼ü(104Ö÷¼üÅÌÉÏ)  //90
	const KeyValue Key::M_Pause         = 0xC5; ///< ÔÝÍ£¼þ(Êý×ÖÅÌÉÏ)      //91
	const KeyValue Key::M_Home          = 0xC7; ///< Home¼ü(Êý×ÖÅÌÉÏ)      //92
	const KeyValue Key::M_Up            = 0xC8; ///< UpArrow¼ü(Êý×ÖÅÌÉÏ)   //93
	const KeyValue Key::M_PgUp          = 0xC9; ///< PgUp¼ü(Êý×ÖÅÌÉÏ)      //94
	const KeyValue Key::M_Left          = 0xCB; ///< LeftArrow¼ü(Êý×ÖÅÌÉÏ) //95
	const KeyValue Key::M_Right         = 0xCD; ///< RightArrow¼ü(Êý×ÖÅÌÉÏ)//96
	const KeyValue Key::M_End           = 0xCF; ///< End¼ü(Êý×ÖÅÌÉÏ)       //97
	const KeyValue Key::M_Down          = 0xD0; ///< DownArrow¼ü(Êý×ÖÅÌÉÏ) //98
	const KeyValue Key::M_PgDown        = 0xD1; ///< PgDn¼ü(Êý×ÖÅÌÉÏ)      //99
	const KeyValue Key::M_Insert        = 0xD2; ///< Insert¼ü(Êý×ÖÅÌÉÏ)    //100
	const KeyValue Key::M_Delete        = 0xD3; ///< Delete¼ü(Êý×ÖÅÌÉÏ)    //101
	const KeyValue Key::LWin            = 0xDB; ///< ×óWin¼ü(Êý×ÖÅÌÉÏ)     //102
    const KeyValue Key::RWin            = 0xDC; ///< ÓÒWin¼ü(Êý×ÖÅÌÉÏ)     //103
	const KeyValue Key::Compose         = 0xDD; ///< AppMenu¼ü(Ö÷¼üÅÌÉÏ)   //104
    const KeyValue Key::Max             = 0xDE;
    //------------------------------------------------------------------------
    Win32KeyboardControlPattern::Win32KeyboardControlPattern(Nid id,
        IDirectInput8 * pDI, DWORD coop) :
            KeyboardControlPattern(id),
            mDirectInput(pDI),
            mKeyboard(0)
    {
        mlastKey = '\0';
    }
    //------------------------------------------------------------------------
    Win32KeyboardControlPattern::~Win32KeyboardControlPattern()
    {
        if(mKeyboard)
        {
            mKeyboard->Unacquire();
            mKeyboard->Release();
            mKeyboard = 0;
        }
    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::_init()
    {
        mModifiers = 0;
        mlastKey = '\0';
        if(FAILED(mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL)))
            N_EXCEPT(CreateDevice, _T("Could not init device!"));

        if(FAILED(mKeyboard->SetDataFormat(&c_dfDIKeyboard)))
            N_EXCEPT(DeviceInner, _T("Format error!"));

        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = KEYBOARD_DX_BUFFERSIZE;

        if (FAILED(mKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            N_EXCEPT(DeviceInner, _T("Buffer error!"));

        HRESULT hr = mKeyboard->Acquire();
        if(FAILED(hr) && hr != DIERR_OTHERAPPHASPRIO)
            N_EXCEPT(DeviceRun, _T("Aquire error!"));
        // Í¬²½ÄÚ²¿×´Ì¬
        syn(KT_L_NumLock);
        syn(KT_L_CapsLock);
        syn(KT_L_ScrLock);
    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::_update()
    {
        if(mActive)
        {
            static DIDEVICEOBJECTDATA diff[KEYBOARD_DX_BUFFERSIZE];
            DWORD count = KEYBOARD_DX_BUFFERSIZE;
            HRESULT hr;

            hr = mKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diff, &count, 0);
            if(hr != DI_OK)
            {
                hr = mKeyboard->Acquire();
                if (hr == E_ACCESSDENIED)
                {
                    //Í¬²½¶ªÊ§×´Ì¬
                    syn(KT_L_NumLock);
                    syn(KT_L_CapsLock);
                    syn(KT_L_ScrLock);
                }
                while(hr == DIERR_INPUTLOST)
                    hr = mKeyboard->Acquire();
                return;
            }

            if(FAILED(hr))
                return;

            //Update keyboard and modifier states
            for(NCount i = 0; i < count; ++i)
            {
                KeyValue kc = (KeyValue)diff[i].dwOfs;

                if(diff[i].dwData & 0x80)
                {
                    press(kc);
                }
                else
                {
                    release(kc);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::_reset()
    {
        HWND hwin;
		DWORD Coop;
        if(mOwn->getPlatformData("WINDOW", &hwin))
            if(mOwn->getPlatformData("KBD", &Coop))
                mKeyboard->SetCooperativeLevel(hwin, Coop);
    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    Nui8 Win32KeyboardControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    Nui8 Win32KeyboardControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    String Win32KeyboardControlPattern::getKeyCode(KeyValue key)
    {
        BYTE keyState[256];
        HKL layout = GetKeyboardLayout(0);
        if( GetKeyboardState(keyState) == 0 )
            return 0;

        unsigned int vk = MapVirtualKeyEx(key, 3, layout);
        if( vk == 0 )
            return 0;

        WCHAR buff[3] = {0};
        int ascii = ToUnicodeEx(vk, key, keyState, buff, 3, 0, layout);

        if(ascii == 1 && mlastKey != '\0' )
        {
            // A dead key is stored and we have just converted a character key
            // Combine the two into a single character
            WCHAR wcBuff[3] = {buff[0], mlastKey, '\0'};
            WCHAR out[3];

            mlastKey = '\0';
            if(FoldStringW(MAP_PRECOMPOSED, (LPWSTR)wcBuff, 3, (LPWSTR)out, 3))
                return out[0];
        }
        else if (ascii == 1)
        {	// We have a single character
            mlastKey = '\0';
            return buff[0];
        }
        else if(ascii == 2)
        {	// Convert a non-combining diacritical mark into a combining diacritical mark
            // Combining versions range from 0x300 to 0x36F; only 5 (for French) have been mapped below
            // http://www.fileformat.info/info/unicode/block/combining_diacritical_marks/images.htm
            switch(buff[0])	
            {
            case 0x5E: // Circumflex accent: ?
                mlastKey = 0x302; break;
            case 0x60: // Grave accent: ?
                mlastKey = 0x300; break;
            case 0xA8: // Diaeresis: ?
                mlastKey = 0x308; break;
            case 0xB4: // Acute accent: ?
                mlastKey = 0x301; break;
            case 0xB8: // Cedilla: ?
                mlastKey = 0x327; break;
            default:
                mlastKey = buff[0]; break;
            }
        }

        return 0;
    }
    //------------------------------------------------------------------------
    String Win32KeyboardControlPattern::getKeyText(KeyValue key)
    {
        String re;
        char temp[256];

        DIPROPSTRING prop;
        prop.diph.dwSize = sizeof(DIPROPSTRING);
        prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        prop.diph.dwObj = static_cast<DWORD>(key);
        prop.diph.dwHow = DIPH_BYOFFSET;

        if (SUCCEEDED(mKeyboard->GetProperty(DIPROP_KEYNAME, &prop.diph)))
        {
            // convert the WCHAR in "wsz" to multibyte
            if (WideCharToMultiByte(CP_ACP, 0, prop.wsz, -1, temp, sizeof(temp), NULL, NULL))
                return re.assign(temp);
        }

        std::stringstream ss;
        ss << "Key_" << (int)key;
        return re.assign(ss.str());
    }
    //------------------------------------------------------------------------
    PatternID Win32KeyboardControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::syn(KeyboardAlterLockType type) const
    {
        switch(type)
        {
        case KT_L_CapsLock:
            if(GetKeyState(VK_CAPITAL) & 0x01)
                on(KT_L_CapsLock);
            else
                off(KT_L_CapsLock);
            break;
        case KT_L_NumLock:
            if(GetKeyState(VK_NUMLOCK) & 0x01)
                on(KT_L_NumLock);
            else
                off(KT_L_NumLock);
            break;
        case KT_L_ScrLock:
            if(GetKeyState(VK_SCROLL) & 0x01)
                on(KT_L_ScrLock);
            else
                off(KT_L_ScrLock);
            break;
        }
    }
    //------------------------------------------------------------------------
}
}