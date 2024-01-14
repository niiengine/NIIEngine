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
#include "NiiOSXControlPatternManager.h"
#include "NiiOSXKeyboardControlPattern.h"
#include "NiiOSXMouseControlPattern.h"
#include "NiiOSXJoyPadControlPattern.h"
#include "NiiOSXJoyStickControlPattern.h"

namespace NII
{
	/**
		Simple wrapper class for CFString which will create a valid CFString and retain ownership until class instance is outof scope
		To Access the CFStringRef instance, simply cast to void*, pass into a function expecting a void* CFStringRef object, or access via cf_str() method
	*/
	class OSXStr
	{
	public:
		OSXStr()
        {
            m_StringRef = CFStringCreateWithCString(NULL, "", kCFStringEncodingUTF8);
        }

		OSXStr(const char * c_str)
        {
            m_StringRef = CFStringCreateWithCString(NULL, c_str, kCFStringEncodingUTF8);
        }

		OSXStr(const std::string & s_str)
        {
            m_StringRef = CFStringCreateWithCString(NULL, s_str.c_str(), kCFStringEncodingUTF8);
        }

		~OSXStr()
        {
            CFRelease(m_StringRef);
        }

		//Allow this class to be autoconverted to base class of StringRef (void*)
		operator void * ()
        {
            return (void*)m_StringRef;
        }

		CFStringRef cf_str()
        {
            return m_StringRef;
        }
	private:
		CFStringRef m_StringRef;
	};
    //------------------------------------------------------------------------
    template<typename T>
    T getDictionaryItemAsRef(CFDictionaryRef dict, const char * keyName)
    {
        return CFDictionaryGetValue(dict, OSXStr(keyName));
    }

    template<>
    CFArrayRef getDictionaryItemAsRef(CFDictionaryRef dict, const char * keyName)
    {
        CFTypeRef temp = CFDictionaryGetValue(dict, OSXStr(keyName));

        if(temp && CFGetTypeID(temp) == CFArrayGetTypeID())
            return (CFArrayRef)temp;
        else
            return 0;
    }

    template<>
    CFStringRef getDictionaryItemAsRef(CFDictionaryRef dict, const char * keyName)
    {
        CFTypeRef temp = CFDictionaryGetValue(dict, OSXStr(keyName));

        if(temp && CFGetTypeID(temp) == CFStringGetTypeID())
            return (CFStringRef)temp;
        else
            return 0;
    }

    template<>
    CFNumberRef getDictionaryItemAsRef(CFDictionaryRef dict, const char* keyName)
    {
        CFTypeRef temp = CFDictionaryGetValue(dict, OSXStr(keyName));

        if(temp && CFGetTypeID(temp) == CFNumberGetTypeID())
            return (CFNumberRef)temp;
        else
            return 0;
    }
    //------------------------------------------------------------------------
    template<typename T>
    T getArrayItemAsRef(CFArrayRef array, CFIndex idx)
    {
        return CFArrayGetValueAtIndex(array, idx);
    }

    template<>
    CFDictionaryRef getArrayItemAsRef(CFArrayRef array, CFIndex idx)
    {
        CFTypeRef temp = CFArrayGetValueAtIndex(array, idx);

        if(temp && CFGetTypeID(temp) == CFDictionaryGetTypeID())
            return (CFDictionaryRef)temp;
        else
            return 0;
    }
    //------------------------------------------------------------------------
    int getInt32(CFNumberRef ref)
    {
        int r = 0;
        if (r)
            CFNumberGetValue(ref, kCFNumberIntType, &r);
        return r;
    }
    //------------------------------------------------------------------------
    OSXControlPatternManager::OSXControlPatternManager() :
        mMark(0)
    {
    }
    //------------------------------------------------------------------------
    OSXControlPatternManager::~OSXControlPatternManager()
    {
        WindowControlList::iterator a, aend = mIdleDev.end();
        for(a = mIdleDev.begin(); a != aend; ++a)
        {
            (a->second).clear();
        }
        mIdleDev.clear();

        ControlList::iterator i, iend = mDev.end();
        for(i = mDev.begin(); i != iend; ++i)
        {
            N_Engine().remove(*i);
            N_delete *i;
        }
        mDev.clear();
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManager::init()
    {
        enumDev();
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManager::retrieve(const ViewWindow * main, ControlPattern * dest)
    {
        WindowControlList::iterator c = mIdleDev.find(main);
        if(c != mIdleDev.end())
        {
            ControlList::iterator it, itend = (c->second).end();
            for(it = (c->second).begin(); it != itend; ++it)
            {
                if((*it) == dest)
                {
                    (c->second).erase(it);
                    break;
                }
            }
            if((c->second).empty())
                mIdleDev.erase(c);
        }
    }
    //------------------------------------------------------------------------
    NCount OSXControlPatternManager::getCount(ControlDevType type) const
    {
        NCount count = 0;
        ControlList::iterator i, iend = mDev.end();
        for(i = mDev.begin(); i != iend; ++i)
        {
            if((*i)->getType() == type)
                ++ count;
        }
        return count;
    }
    //------------------------------------------------------------------------
    ControlPattern * OSXControlPatternManager::createImpl(const ViewWindow * main,
        ControlDevType type)
    {
        ControlPattern * re = NULL;
        WindowControlList::iterator c = mIdleDev.find(main);
        if(c != mIdleDev.end())
        {
            ControlList::iterator i, iend = (c->second).end();
            for(i = (c->second).begin(); i != iend; ++i)
            {
                if((*i)->getType() == type)
                {
                    (c->second).erase(i);
                    re = *i;
                    break;
                }
            }
        }
        else
        {
            ControlList temp;
            temp.insert(temp.begin(), mDev.begin(), mDev.end());
            ControlList::iterator i, iend = temp.end();
            for(i = temp.begin(); i != iend; ++i)
            {
                if((*i)->getType() == type)
                {
                    temp.erase(i);
                    re = *i;
                    break;
                }
            }
            mIdleDev.insert(WindowControlList::value_type(main, temp));
        }

        return re;
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManager::enumDev()
    {
        ControlPattern * re;
        io_iterator_t iterator;

        int page = kHIDPage_GenericDesktop;
        int usage = kHIDUsage_GD_Keyboard;
        iterator = lookUp(usage, page);
        if(iterator)
        {
            re = N_new OSXKeyboardControlPattern(mMark++);
            mDev.push_back(re);
            re->_pause();
            N_Engine().add(re);
        }

        usage = kHIDUsage_GD_Mouse;
        iterator = lookUp(usage, page);
        if(iterator)
        {
            re = N_new OSXMouseControlPattern(mMark++);
            mDev.push_back(re);
            re->_pause();
            N_Engine().add(re);
        }

        usage = kHIDUsage_GD_GamePad;
        iterator = lookUp(usage, page);
        if(iterator)
            enumDev(iterator);

        usage = kHIDUsage_GD_Joystick;
        iterator = lookUp(usage, page);
        if(iterator)
            enumDev(iterator);

        usage = kHIDUsage_GD_Keypad;
        iterator = lookUp(usage, page);

        usage = kHIDUsage_GD_MultiAxisController;
        iterator = lookUp(usage, page);
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManager::enumDev(int usage, io_iterator_t iterator)
    {
        ControlPattern * re;
        io_object_t dev = 0;
        while ((dev = IOIteratorNext(iterator)) !=0)
        {
            //Get the current registry items property map
            CFMutableDictionaryRef propertyMap = 0;
            if (IORegistryEntryCreateCFProperties(dev, &propertyMap, kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS && propertyMap)
            {
                //todo - we need to hold an open interface so we do not have to enumerate again later
                //should be able to watch for device removals also

                // Testing opening / closing interface
                IOCFPlugInInterface ** face = NULL;
                SInt32 score = 0;
                IOHIDDeviceInterface ** interface;
                if (IOCreatePlugInInterfaceForService(dev, kIOHIDDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &face, &score) == kIOReturnSuccess)
                {
                    HRESULT result = (*face)->QueryInterface(face, CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID), (void **)&(interface));

                    if(result != S_OK)
                        N_EXCEPT(CreateDevice, _T("Can't to create plugin interface !"));

                    IODestroyPlugInInterface(face);

                    switch(usage)
                    {
                    case kHIDUsage_GD_Keyboard:
                    case kHIDUsage_GD_Mouse:
                        // 使用回调模式
                        break;
                    case kHIDUsage_GD_GamePad:
                        re = N_new OSXJoyPadControlPattern(mMark++, interface);
                        break;
                    case kHIDUsage_GD_Joystick:
                        re = N_new OSXJoyStickControlPattern(mMark++, interface);
                        break;
                    case kHIDUsage_GD_Keypad:
                    case kHIDUsage_GD_MultiAxisController:
                        break;
                    }

                    //Go through device to find all needed info
                    enumDevProperties(re, propertyMap);
                    mDev.push_back(re);
                    re->_pause();
                    N_Engine().add(re);
                }
            }
        }

        IOObjectRelease(iterator);
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManager::enumDevProperties(ControlPattern * re,
        CFMutableDictionaryRef propertyMap)
    {
        String temp1;
        String temp2;
        CFStringRef str = getDictionaryItemAsRef<CFStringRef>(propertyMap, kIOHIDManufacturerKey);
        if (str)
            temp1 = CFStringGetCStringPtr(str, CFStringGetSystemEncoding());

        str = getDictionaryItemAsRef<CFStringRef>(propertyMap, kIOHIDProductKey);
        if (str)
            temp2 = CFStringGetCStringPtr(str, CFStringGetSystemEncoding());

        re->setVendor(temp1 + " " + temp2);

        //Go through all items in this device (i.e. buttons, hats, sticks, axes, etc)
        CFArrayRef array = getDictionaryItemAsRef<CFArrayRef>(propertyMap, kIOHIDElementKey);
        if (array)
            for (int i = 0; i < CFArrayGetCount(array); i++)
                analysisProperties(getArrayItemAsRef<CFDictionaryRef>(array, i));
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManager::analysisProperties(CFDictionaryRef properties)
    {
        if(!properties)
            return;

        CFArrayRef array = getDictionaryItemAsRef<CFArrayRef>(properties, kIOHIDElementKey);
        if (array)
        {
            for (int i = 0; i < CFArrayGetCount(array); ++i)
            {
                CFDictionaryRef element = getArrayItemAsRef<CFDictionaryRef>(array, i);
                if (element)
                {
                    if(getInt32(getDictionaryItemAsRef<CFNumberRef>(element, kIOHIDElementTypeKey)) == kIOHIDElementTypeCollection)
                    {	//Check if we need to recurse further intoi another collection
                        if(getInt32(getDictionaryItemAsRef<CFNumberRef>(element, kIOHIDElementUsagePageKey)) == kHIDPage_GenericDesktop)
                            analysisProperties(element);
                    }
                    else
                    {
                        switch(getInt32(getDictionaryItemAsRef<CFNumberRef>(element, kIOHIDElementUsagePageKey)))
                        {
                        case kHIDPage_GenericDesktop:
                            switch(getInt32(getDictionaryItemAsRef<CFNumberRef>(element, kIOHIDElementUsageKey)))
                            {
                            case kHIDUsage_GD_Pointer:
                                //cout << "\tkHIDUsage_GD_Pointer\n";
                                analysisPropertiesGroup(element);
                                break;
                            case kHIDUsage_GD_X:
                            case kHIDUsage_GD_Y:
                            case kHIDUsage_GD_Z:
                            case kHIDUsage_GD_Rx:
                            case kHIDUsage_GD_Ry:
                            case kHIDUsage_GD_Rz:
                                //cout << "\tAxis\n";
                                break;
                            case kHIDUsage_GD_Slider:
                            case kHIDUsage_GD_Dial:
                            case kHIDUsage_GD_Wheel:
                                //cout << "\tUnsupported kHIDUsage_GD_Wheel\n";
                                break;
                            case kHIDUsage_GD_Hatswitch:
                                //cout << "\tUnsupported - kHIDUsage_GD_Hatswitch\n";
                                break;
                            }
                            break;
                        case kHIDPage_Button:
                            //cout << "\tkHIDPage_Button\n";
                            break;
                        }
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManager::analysisPropertiesGroup(CFDictionaryRef properties)
    {
        if(!properties)
            return;

        CFArrayRef array = getDictionaryItemAsRef<CFArrayRef>(properties, kIOHIDElementKey);
        if(array)
        {
            for (int i = 0; i < CFArrayGetCount(array); ++i)
            {
                CFDictionaryRef element = getArrayItemAsRef<CFDictionaryRef>(array, i);
                if (element)
                {
                    switch(getInt32(getDictionaryItemAsRef<CFNumberRef>(element, kIOHIDElementUsagePageKey)))
                    {
                    case kHIDPage_GenericDesktop:
                        switch(getInt32(getDictionaryItemAsRef<CFNumberRef>(element, kIOHIDElementUsageKey)))
                        {
                        case kHIDUsage_GD_X:
                        case kHIDUsage_GD_Y:
                        case kHIDUsage_GD_Z:
                        case kHIDUsage_GD_Rx:
                        case kHIDUsage_GD_Ry:
                        case kHIDUsage_GD_Rz:
                            //cout << "\t\tAxis\n";
                            break;
                        case kHIDUsage_GD_Slider:
                        case kHIDUsage_GD_Dial:
                        case kHIDUsage_GD_Wheel:
                            //cout << "\tUnsupported - kHIDUsage_GD_Wheel\n";
                            break;
                        case kHIDUsage_GD_Hatswitch:
                            //cout << "\tUnsupported - kHIDUsage_GD_Hatswitch\n";
                            break;
                        }
                        break;
                    case kHIDPage_Button:
                        break;
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    io_iterator_t OSXControlPatternManager::lookUp(int usage, int page)
    {
        CFMutableDictionaryRef deviceLookupMap = IOServiceMatching(kIOHIDDeviceKey);
        if(!deviceLookupMap)
            OIS_EXCEPT(E_General, "Could not setup HID device search parameters");

        CFNumberRef _usage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
        CFNumberRef _page  = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);

        CFDictionarySetValue(deviceLookupMap, CFSTR(kIOHIDPrimaryUsageKey), _usage);
        CFDictionarySetValue(deviceLookupMap, CFSTR(kIOHIDPrimaryUsagePageKey), _page);

        //IOServiceGetMatchingServices consumes the map so we do not have to release it ourself
        io_iterator_t iterator = 0;
        IOReturn result = IOServiceGetMatchingServices(kIOMasterPortDefault, deviceLookupMap, &iterator);

        CFRelease(_usage);
        CFRelease(_page);

        if(result == kIOReturnSuccess)
        {
            return iterator;
        }
        //TODO: Throw exception instead?
        else
        {
            return 0;
        }
    }
    //------------------------------------------------------------------------
}