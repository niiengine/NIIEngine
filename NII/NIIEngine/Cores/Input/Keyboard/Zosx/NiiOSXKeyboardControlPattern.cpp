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
#include "NiiOSXKeyboardControlPattern.h"

namespace NII
{
    //http://www.opensource.apple.com/source/ApplePS2Keyboard/ApplePS2Keyboard-9/ApplePS2ToADBMap.h
    //------------------------------------------------------------------------
    const KeyValue Key::Esc             = 0x35; ///< ESC按键(主键盘上)    //1
	const KeyValue Key::NO1             = 0x12; ///< 数字1键(主键盘上)    //2
	const KeyValue Key::NO2             = 0x13; ///< 数字2键(主键盘上)    //3
	const KeyValue Key::NO3             = 0x14; ///< 数字3键(主键盘上)    //4
	const KeyValue Key::NO4             = 0x15; ///< 数字4键(主键盘上)    //5
	const KeyValue Key::NO5             = 0x17; ///< 数字5键(主键盘上)    //6
	const KeyValue Key::NO6             = 0x16; ///< 数字6键(主键盘上)    //7
	const KeyValue Key::NO7             = 0x1a; ///< 数字7键(主键盘上)    //8
	const KeyValue Key::NO8             = 0x1c; ///< 数字8键(主键盘上)    //9
	const KeyValue Key::NO9             = 0x19; ///< 数字9键(主键盘上)    //10
	const KeyValue Key::NO0             = 0x1d; ///< 数字0键(主键盘上)    //11
	const KeyValue Key::Minus           = 0x1b; ///< - 和 _ 键(主键盘上)  //12
	const KeyValue Key::Equal           = 0x18; ///< = 和 + 键(主键盘上)  //13
	const KeyValue Key::BK              = 0x33; ///< 空格键(主键盘上)     //14
	const KeyValue Key::TAB             = 0x30; ///< tab键(主键盘上)      //15
	const KeyValue Key::Q               = 0x0c; ///< q键                 //16
	const KeyValue Key::W               = 0x0d; ///< w键                 //17
	const KeyValue Key::E               = 0x0e; ///< e键                 //18
	const KeyValue Key::R               = 0x0f; ///< r键                 //19
	const KeyValue Key::T               = 0x11; ///< t键                 //20
	const KeyValue Key::Y               = 0x10; ///< y键                 //21
	const KeyValue Key::U               = 0x20; ///< u键                 //22
	const KeyValue Key::I               = 0x22; ///< i键                 //23
	const KeyValue Key::O               = 0x1f; ///< o键                 //24
	const KeyValue Key::P               = 0x23; ///< p键                 //25
	const KeyValue Key::LBracket        = 0x21; ///< [ 和 { 键(主键盘上)  //26
	const KeyValue Key::RBracket        = 0x1e; ///< ] 和 } 键(主键盘上)  //27
	const KeyValue Key::Enter           = 0x24; ///< Enter键(主键盘上)    //28
	const KeyValue Key::LCtrl           = 0x3b; ///< 左ctrl(主键盘上)     //29
	const KeyValue Key::A               = 0x00; ///< a键                 //30
	const KeyValue Key::S               = 0x01; ///< s键                 //31
	const KeyValue Key::D               = 0x02; ///< d键                 //32
	const KeyValue Key::F               = 0x03; ///< f键                 //33
	const KeyValue Key::G               = 0x05; ///< g键                 //34
	const KeyValue Key::H               = 0x04; ///< h键                 //35
	const KeyValue Key::J               = 0x26; ///< j键                 //36
	const KeyValue Key::K               = 0x28; ///< k键                 //37
	const KeyValue Key::L               = 0x25; ///< l键                 //38
	const KeyValue Key::Semicolon       = 0x29; ///< ; 和 : 键(主键盘上)  //39
	const KeyValue Key::Apostrophe      = 0x27; ///< ' 和 " 键(主键盘上)  //40
	const KeyValue Key::Grave           = 0x32; ///< ` 和 ~ 键(主键盘上)  //41
	const KeyValue Key::LShift          = 0x38; ///< 左shift键(主键盘上)  //42
	const KeyValue Key::BackSlash       = 0x2a; ///< \和|键(102主键盘上)  //43
	const KeyValue Key::Z               = 0x06; ///< z键                 //44
	const KeyValue Key::X               = 0x07; ///< x键                 //45
	const KeyValue Key::C               = 0x08; ///< c键                 //46
	const KeyValue Key::V               = 0x09; ///< v键                 //47
	const KeyValue Key::B               = 0x0b; ///< b键                 //48
	const KeyValue Key::N               = 0x2d; ///< n键                 //49
	const KeyValue Key::M               = 0x2e; ///< m键                 //50
	const KeyValue Key::Comma           = 0x2b; ///< , 和 < 键(主键盘上)  //51
	const KeyValue Key::Period          = 0x2f; ///< . 和 > 键(主键盘上)  //52
	const KeyValue Key::Slash           = 0x2c; ///< / 和 ? 键(主键盘上)  //53
	const KeyValue Key::RShift          = 0x3c; ///< 右shift键(主键盘上)  //54
	const KeyValue Key::N_Mul           = 0x43; ///< *键(数字键盘)        //55
	const KeyValue Key::LAlt            = 0x3a; ///< 左alt键(主键盘上)    //56
	const KeyValue Key::Space           = 0x31; ///< Space键             //57
	const KeyValue Key::CapsLock        = 0x39; ///< CapsLock键          //58
	const KeyValue Key::F1              = 0x7a; ///< F1键                //59
	const KeyValue Key::F2              = 0x78; ///< F2键                //60
	const KeyValue Key::F3              = 0x63; ///< F3键                //61
	const KeyValue Key::F4              = 0x76; ///< F4键                //62
	const KeyValue Key::F5              = 0x60; ///< F5键                //63
	const KeyValue Key::F6              = 0x61; ///< F6键                //64
	const KeyValue Key::F7              = 0x62; ///< F7键                //65
	const KeyValue Key::F8              = 0x64; ///< F8键                //66
	const KeyValue Key::F9              = 0x65; ///< F9键                //67
	const KeyValue Key::F10             = 0x6d; ///< F10键               //68
	const KeyValue Key::N_Lock          = 0x47; ///< NumLock键           //69
	const KeyValue Key::M_ScrLock       = 0x6b; ///< ScrollLock键        //70
	const KeyValue Key::N_NO7           = 0x59; ///< 数字7键(数字盘上)    //71
	const KeyValue Key::N_NO8           = 0x5b; ///< 数字8键(数字盘上)    //72
	const KeyValue Key::N_NO9           = 0x5c; ///< 数字9键(数字盘上)    //73
	const KeyValue Key::N_Subtract      = 0x4e; ///< -键(数字盘上)        //74
	const KeyValue Key::N_NO4           = 0x56; ///< 数字4键(数字盘上)    //75
	const KeyValue Key::N_NO5           = 0x57; ///< 数字5键(数字盘上)    //76
	const KeyValue Key::N_NO6           = 0x58; ///< 数字6键(数字盘上)    //77
	const KeyValue Key::N_Add           = 0x45; ///< +键(数字盘上)        //78
	const KeyValue Key::N_NO1           = 0x53; ///< 数字1键(数字盘上)    //79
	const KeyValue Key::N_NO2           = 0x54; ///< 数字2键(数字盘上)    //80
	const KeyValue Key::N_NO3           = 0x55; ///< 数字3键(数字盘上)    //81
	const KeyValue Key::N_NO0           = 0x52; ///< Ins和数字0键(数字盘上)//82
	const KeyValue Key::N_Dot           = 0x41; ///< . 和 Del 键(数字盘上)//83
	const KeyValue Key::F11             = 0x67; ///< F11键(101+主键盘上)  //84
	const KeyValue Key::F12             = 0x6f; ///< F12键(101+主键盘上)  //85
	const KeyValue Key::N_Enter         = 0x4c; ///< Enter键(数字盘上)    //86
    const KeyValue Key::RCtrl           = 0x3e; ///< 右ctrl键(104主键盘上)//87
	const KeyValue Key::N_Divide        = 0x4b; ///< /键(数字盘上)         //88
	const KeyValue Key::M_SysRQ         = 0x69; ///< 复制屏幕图象(数字盘上) //89
    const KeyValue Key::RAlt            = 0x3d; ///< 右alt键(104主键盘上)  //90
	const KeyValue Key::M_Pause         = 0x71; ///< 暂停键(数字盘上)      //91
	const KeyValue Key::M_Home          = 0x73; ///< Home键(数字盘上)      //92
	const KeyValue Key::M_Up            = 0x7e; ///< UpArrow键(数字盘上)   //93
	const KeyValue Key::M_PgUp          = 0x74; ///< PgUp键(数字盘上)      //94
	const KeyValue Key::M_Left          = 0x7b; ///< LeftArrow键(数字盘上) //95
	const KeyValue Key::M_Right         = 0x7c; ///< RightArrow键(数字盘上)//96
	const KeyValue Key::M_End           = 0x77; ///< End键(数字盘上)       //97
	const KeyValue Key::M_Down          = 0x7d; ///< DownArrow键(数字盘上) //98
	const KeyValue Key::M_PgDown        = 0x79; ///< PgDn键(数字盘上)      //99
	const KeyValue Key::M_Insert        = 0x72; ///< Insert键(数字盘上)    //100
	const KeyValue Key::M_Delete        = 0x75; ///< Delete键(数字盘上)    //101
	const KeyValue Key::LWin            = 0x37; ///< 左Win键(数字盘上)     //102
    const KeyValue Key::RWin            = 0x36; ///< 右Win键(数字盘上)     //103
	const KeyValue Key::Compose         = 0x6e; ///< AppMenu键(主键盘上)   //104
    const KeyValue Key::Max             = 0x7f;
    //------------------------------------------------------------------------
    OSStatus MacKeyDownCallBack(EventHandlerCallRef handle, EventRef event, void * src)
    {
        ((OSXKeyboardControlPattern*)src)->onKeyDown(event);
        // propagate the event down the chain
        return CallNextEventHandler(handle, event);
    }
    //------------------------------------------------------------------------
    OSStatus MacKeyUpCallBack(EventHandlerCallRef handle, EventRef event, void * src)
    {
        ((OSXKeyboardControlPattern*)src)->onKeyUp(event);
        // propagate the event down the chain
        return CallNextEventHandler(handle, event);
    }
    //------------------------------------------------------------------------
    OSStatus MacKeyModCallBack(EventHandlerCallRef handle, EventRef event, void * src)
    {
        ((OSXKeyboardControlPattern*)src)->onKeyMod(event);
        // propagate the event down the chain
        return CallNextEventHandler(handle, event);
    }
    //------------------------------------------------------------------------
    const EventTypeSpec MacKeyDown[] =
        {{kEventClassKeyboard, kEventRawKeyDown},	//non - repeats
        {kEventClassKeyboard, kEventRawKeyRepeat}}; //repeats

    const EventTypeSpec MacKeyUp =
        {kEventClassKeyboard, kEventRawKeyUp};

    const EventTypeSpec MacKeyMod =
        {kEventClassKeyboard, kEventRawKeyModifiersChanged};

    const EventTypeSpec MacKeyAll[] =
        {{kEventClassKeyboard, kEventRawKeyDown},
        {kEventClassKeyboard, kEventRawKeyRepeat},
        {kEventClassKeyboard, kEventRawKeyUp},
        {kEventClassKeyboard, kEventRawKeyModifiersChanged}};
    //------------------------------------------------------------------------
    OSXKeyboardControlPattern::OSXKeyboardControlPattern(Nid id) :
        KeyboardControlPattern(id),
        mMain(NULL),
        mCaps((IOHIDElementCookie)0),
        mNum((IOHIDElementCookie)0),
        mScr((IOHIDElementCookie)0)
    {
        mKeyDownEventRef = NULL;
        mKeyUpEventRef = NULL;
        mKeyModEventRef = NULL;

        // Get a so-called "Univeral procedure pointer" for our callback
        mKeyDown = NewEventHandlerUPP(MacKeyDownCallBack);
        mKeyUp = NewEventHandlerUPP(MacKeyUpCallBack);
        mKeyMod = NewEventHandlerUPP(MacKeyModCallBack);

        setVendor("CallBack");
    }
    //------------------------------------------------------------------------
    OSXKeyboardControlPattern::~OSXKeyboardControlPattern()
    {
        // Remove our handlers so that this instance doesn't get called
        // after it is deleted
        if (mKeyDownEventRef != NULL)
            RemoveEventHandler(mKeyDownEventRef);

        if (mKeyUpEventRef != NULL)
            RemoveEventHandler(mKeyUpEventRef);

        if (mKeyModEventRef != NULL)
            RemoveEventHandler(mKeyModEventRef);

        // dispose of our UPPs
        DisposeEventHandlerUPP(mKeyDown);
        DisposeEventHandlerUPP(mKeyUp);
        DisposeEventHandlerUPP(mKeyMod);

        if(*mMain != NULL)
            (*mMain)->Release(mMain);
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::onKeyDown(EventRef event)
    {
        if(mActive)
        {
            UInt32 key;
            OSStatus status;

            status = GetEventParameter(event, 'kcod', typeUInt32, NULL,
                sizeof(UInt32), NULL, &key);

            if(status == noErr)
            {
                KeyValue kc = (KeyValue)key;
                mEvents.push_back(OSXKeyStackEvent(kc, MKET_KEYDOWN));
            }
        }
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::onKeyUp(EventRef event)
    {
        if(mActive)
        {
            UInt32 key;
            OSStatus status;

            status = GetEventParameter(event, kEventParamKeyCode, typeUInt32,
                NULL, sizeof(UInt32), NULL, &key);

            if(status == noErr)
            {
                KeyValue kc = (KeyValue)key;
                mEvents.push_back(OSXKeyStackEvent(kc, MKET_KEYUP));
            }
        }
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::onKeyMod(EventRef event)
    {
        if(mActive)
        {
            UInt32 mods;
            OSStatus status;
            status = GetEventParameter(event, kEventParamKeyModifiers,
                typeUInt32, NULL, sizeof(UInt32), NULL, &mods);

            if(status != noErr)
                return;
            // find the changed bit
            UInt32 change = mPrevModMask ^ mods;
            OSXKeyboardEventType newstate = ((change & mPrevModMask) > 0) ?
                MKET_KEYUP : MKET_KEYDOWN;

            // TODO test modifiers on a full keyboard to check if different mask for left/right
            switch (change)
            {
                case (shiftKey): // shift
                    mEvents.push_back(OSXKeyStackEvent(Key::LShift, newstate));
                    break;

                case (rightShiftKey):
                    mEvents.push_back(OSXKeyStackEvent(Key::RShift, newstate));
                    break;

                case (optionKey): // option (alt)
                    mEvents.push_back(OSXKeyStackEvent(Key::LAlt, newstate));
                    break;

                case (rightOptionKey):
                    mEvents.push_back(OSXKeyStackEvent(Key::RAlt, newstate));
                    break;

                case (controlKey): // Ctrl
                    mEvents.push_back(OSXKeyStackEvent(Key::LCtrl, newstate));
                    break;

                case (rightControlKey):
                    mEvents.push_back(OSXKeyStackEvent(Key::RCtrl, newstate));
                    break;

                case (cmdKey): // apple
                    mEvents.push_back(OSXKeyStackEvent(Key::LWin, newstate));
                    break;

                case (kEventKeyModifierRightCmdKeyMask):
                    mEvents.push_back(OSXKeyStackEvent(Key::RWin, newstate));
                    break;

                case (kEventKeyModifierFnMask): // fn key
                    mEvents.push_back(OSXKeyStackEvent(Key::Compose, newstate));
                    break;

                case (kEventKeyModifierNumLockMask): // numlock
                    mEvents.push_back(OSXKeyStackEvent(Key::N_Lock, newstate));
                    break;

                case (alphaLock): // caps lock
                    mEvents.push_back(OSXKeyStackEvent(Key::CapsLock, newstate));
                    break;

                default:
                    // not scroll lock
            }

            mPrevModMask = mods;
        }
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::_init()
    {
        OSStatus status;
        EventTargetRef event = GetApplicationEventTarget();
        if(event == NULL)
            N_EXCEPT(EEC_I_Create_Inner, _T("Window event target error !"));

        mModifiers = 0;
        mPrevModMask = 0;

        // just in case this gets called after the first time.. better safe
        if (mKeyUpEventRef != NULL)
        {
            RemoveEventHandler(mKeyUpEventRef);
            mKeyUpEventRef = NULL;
        }

        if (mKeyModEventRef != NULL)
        {
            RemoveEventHandler(mKeyModEventRef);
            mKeyModEventRef = NULL;
        }

        if (status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading Key down event handler error !"));

            status = InstallEventHandler(event, mKeyUp, 1, &MacKeyUp, this, &mKeyUpEventRef);
        if (status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading Key up event handler error !"));

            status = InstallEventHandler(event, mKeyMod, 1, &MacKeyMod, this, &mKeyModEventRef);
        if (status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading Key mod event handler error !"));
        // 建立Hid
        setupHid();
        // 同步内部状态
        syn(KT_L_NumLock);
        syn(KT_L_CapsLock);
        syn(KT_L_ScrLock);
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::_update()
    {
        if(mActive)
        {
            Events::iterator it, itend = mEvents.end();
            for(it = mEvents.begin(); it != itend; ++it)
            {
                switch((*it).mType)
                {
                case MKET_KEYDOWN :
                    press((*it).mKey);
                    break;
                case MKET_KEYUP :
                    release((*it).mKey);
                    break;
                case MKET_KEYREPEAT:
                default:
                    break;
                }
            }
            mEvents.clear();
        }
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::_reset()
    {
        bool repeat;

        if (mKeyDownEventRef != NULL)
            RemoveEventHandler(mKeyDownEventRef);

        EventTargetRef event = GetApplicationEventTarget();

        if(event != NULL)
        {
/*             if(mOwn->getPlatformData("KBRepeat", &repeat))
            {
                if (repeat)
                    InstallEventHandler(event, mKeyDown, 2, MacKeyDown, this, &mKeyDownEventRef);
            }
            else
            {
                InstallEventHandler(event, mKeyDown, 1, MacKeyDown, this, &mKeyDownEventRef);
            } */
            InstallEventHandler(event, mKeyDown, 1, MacKeyDown, this, &mKeyDownEventRef);
        }
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    Nui8 OSXKeyboardControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    Nui8 OSXKeyboardControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    PatternID OSXKeyboardControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::syn(KeyboardAlterLockType type) const
    {
        IOReturn ioresult
        IOHIDEventStruct destresult;
        ioresult = (*mMain)->open(mMain, 0);
        if(ioresult != kIOReturnSuccess)
            N_EXCEPT(EEC_I_Create_Run, _T("Can't open keyboard device interface !"));

        switch(type)
        {
        case KT_L_CapsLock:
            if(mCaps != 0)
            {
                ioresult = (*mMain)->getElementValue(mMain, mCaps, &destresult);
                if(destresult.value)
                    on(KT_L_CapsLock);
                else
                    off(KT_L_CapsLock);
            }
            break;
        case KT_L_NumLock:
            if(mNum != 0)
            {
                ioresult = (*mMain)->getElementValue(mMain, mNum, &destresult);
                if(destresult.value)
                    on(KT_L_NumLock);
                else
                    off(KT_L_NumLock);
            }
            break;
        case KT_L_ScrLock:
            if(mScr != 0)
            {
                ioresult = (*mMain)->getElementValue(mMain, mScr, &destresult);
                if(destresult.value)
                    on(KT_L_ScrLock);
                else
                    off(KT_L_ScrLock);
            }
            break;
        }
        (*mMain)->close(mMain);
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::setupHid()
    {
        CFMutableDictionaryRef matching = IOServiceMatching(kIOHIDDeviceKey);
        if(!matching)
            N_EXCEPT(CreateDevice, _T("Can't find keyboard service !"));

        UInt32 page = kHIDPage_GenericDesktop;
        UInt32 usage = kHIDUsage_GD_Keyboard;

        CFNumberRef pageRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
        CFNumberRef usageRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);

        CFDictionarySetValue(matching, CFSTR(kIOHIDPrimaryUsagePageKey), pageRef);
        CFDictionarySetValue(matching, CFSTR(kIOHIDPrimaryUsageKey), usageRef);

        io_object_t hid = (io_object_t)IOServiceGetMatchingService(kIOMasterPortDefault, matching);

        CFRelease(usageRef);
        CFRelease(pageRef);

        if(!hid)
            N_EXCEPT(CreateDevice, _T("Can't found matching keyboard device object !"));

        IOCFPlugInInterface ** interface = NULL;
        SInt32 score = 0;
        HRESULT plugresult;
        IOReturn ioresult;

        ioresult = IOCreatePlugInInterfaceForService(hid, kIOHIDDeviceUserClientTypeID,
            kIOCFPlugInInterfaceID, &interface, &score);
        if(ioresult == kIOReturnSuccess)
        {
            plugresult = (*interface)->QueryInterface(interface,
                CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID), (LPVOID)&mMain);

            IODestroyPlugInInterface(interface);

            if(plugresult != S_OK)
                N_EXCEPT(CreateDevice, _T("Can't query a device interface !"));
        }

        findCookie();

        ioresult = IOObjectRelease(hid);
        if(ioresult != kIOReturnSuccess)
            N_EXCEPT(CreateDevice, _T("Can't release matching keyboard device object !"));
    }
    //------------------------------------------------------------------------
    void OSXKeyboardControlPattern::findCookie()
    {
        IOHIDDeviceInterface122 ** temp = (IOHIDDeviceInterface122 **)mMain;
        IOHIDElementCookie cookie;
        CFTypeRef object;
        long tempresult;
        long usage;
        long page;
        CFArrayRef elements;
        CFDictionaryRef element;

        IOReturn result = (*temp)->copyMatchingElements(temp, NULL, &elements);
        if (result != kIOReturnSuccess)
            N_EXCEPT(EEC_I_Create_Inner, _T("Can't copy cookies !"));

        CFIndex i, iend = CFArrayGetCount(elements);
        NCount ok = 0;
        for (i = 0; i < iend; ++i)
        {
            element = CFArrayGetValueAtIndex(elements, i);

            object = (CFDictionaryGetValue(element, CFSTR(kIOHIDElementCookieKey)));
            if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
            {
                continue;
            }
            if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, &tempresult))
            {
                continue;
            }
            cookie = (IOHIDElementCookie)tempresult;

            object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementUsageKey));
            if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
            {
                continue;
            }
            if (!CFNumberGetValue((CFNumberRef)object, kCFNumberLongType, &tempresult))
            {
                continue;
            }
            usage = tempresult;

            object = CFDictionaryGetValue(element,CFSTR(kIOHIDElementUsagePageKey));
            if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
            {
                continue;
            }
            if (!CFNumberGetValue((CFNumberRef)object, kCFNumberLongType, &tempresult))
            {
                continue;
            }
            page = tempresult;

            if (page == kHIDPage_LEDs)
            {
                switch (usage)
                {
                case kHIDUsage_LED_NumLock:
                    mNum = cookie;
                    break;

                case kHIDUsage_LED_CapsLock:
                    mCaps = cookie;
                    break;

                case kHIDUsage_LED_ScrollLock:
                    mScr = cookie;
                    break;
                default:
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
}