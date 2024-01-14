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
#include "NiiOSXJoyPadControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    OSXJoyPadControlPattern::OSXJoyPadControlPattern(Nid id,
        IOHIDDeviceInterface ** interface) :
            JoyPadControlPattern(id),
            mInterface(interface)
    {
    }
    //------------------------------------------------------------------------
    OSXJoyPadControlPattern::~OSXJoyPadControlPattern()
    {
        //TODO: check if the queue has been started first?
        (*mQueue)->dispose(mQueue);
        (*mQueue)->Release(mQueue);


        //TODO: check if the interface has been opened first?
        (*mInterface)->close(mInterface);
        (*mInterface)->Release(mInterface);
    }
    //------------------------------------------------------------------------
    NIIb OSXJoyPadControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb OSXJoyPadControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void OSXJoyPadControlPattern::_init()
    {
        if ((*mInterface)->open(mInterface, 0) != KERN_SUCCESS)
            N_EXCEPT(DeviceInner, _T("Could not initialize joy device !"));

        enumCom();

        setButtonCount(mButtonCookies.size());
        setAxisCount(mAxisCookies.size());

        mQueue = create();
    }
    //------------------------------------------------------------------------
    void OSXJoyPadControlPattern::_update()
    {
        if(mActive)
        {
            AbsoluteTime zeroTime = {0,0};

            IOHIDEventStruct event;
            IOReturn result = (*mQueue)->getNextEvent(mQueue, &event, zeroTime, 0);
            while(result == kIOReturnSuccess)
            {
                switch(event.type)
                {
                case kIOHIDElementTypeInput_Button:
                    ButtonCookies::iterator bit =
                        std::find(mButtonCookies.begin(), mButtonCookies.end(), event.elementCookie);
                    int button = std::distance(mButtonCookies.begin(), bit);

                    if(event.value == 0)
                        release(button);
                    else if(event.value == 1)
                        press(button);

                    break;
                case kIOHIDElementTypeInput_Misc:
                    //TODO: It's an axis! - kind of - for gamepads - or should this be a pov?
                case kIOHIDElementTypeInput_Axis:
                    AxisCookies::iterator ait = mAxisCookies.find(event.elementCookie);
                    if(ait != mAxisCookies.end())
                    {
                        int axis = std::distance(mAxisCookies.begin(), ait);

                        //Copied from LinuxJoyStickEvents.cpp, line 149
                        const Info & info = ait->second;
                        twist(axis, event.value * info.mScale + info.mOffset);
                    }
                    break;
                }

                result = (*mQueue)->getNextEvent(mQueue, &event, zeroTime, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void OSXJoyPadControlPattern::_reset()
    {
        // do nothing
    }
    //------------------------------------------------------------------------
    void OSXJoyPadControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void OSXJoyPadControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void OSXJoyPadControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID OSXJoyPadControlPattern::getPID() const
    {

    }
    //------------------------------------------------------------------------
    void OSXJoyPadControlPattern::enumCom()
    {
        CFTypeRef object;
        long number;
        IOHIDElementCookie cookie;
        long usage;
        long usagePage;
        int min;
        int max;

        CFDictionaryRef element;

        // Copy all elements, since we're grabbing most of the elements
        // for this device anyway, and thus, it's faster to iterate them
        // ourselves. When grabbing only one or two elements, a matching
        // dictionary should be passed in here instead of NULL.
        CFArrayRef elements;
        IOReturn success = reinterpret_cast<IOHIDDeviceInterface122 *>(*mInterface)->copyMatchingElements(mInfo->interface, NULL, &elements);

        if (success == kIOReturnSuccess)
        {
            const CFIndex numOfElements = CFArrayGetCount(elements);
            for (CFIndex i = 0; i < numOfElements; ++i)
            {
                element = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(elements, i));

                //Get cookie
                object = (CFDictionaryGetValue(element, CFSTR(kIOHIDElementCookieKey)));

                if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                    continue;

                if(!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, &number))
                    continue;

                cookie = (IOHIDElementCookie) number;

                //Get usage
                object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementUsageKey));

                if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                    continue;

                if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, &number))
                    continue;

                usage = number;

                //Get min, kIOHIDElementMinKey or kIOHIDElementScaledMinKey?, no idea ...
                object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementMinKey));
                if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                    continue;

                if (!CFNumberGetValue((CFNumberRef) object, kCFNumberIntType, &number))
                    continue;

                min = number;

                //Get max, kIOHIDElementMaxKey or kIOHIDElementScaledMaxKey?, no idea ...
                object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementMaxKey));
                if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                    continue;

                if (!CFNumberGetValue((CFNumberRef) object, kCFNumberIntType, &number))
                    continue;

                max = number;

                //Get usage page
                object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementUsagePageKey));

                if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
                    continue;

                if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, &number))
                    continue;

                usagePage = number;
                switch(usagePage)
                {
                    case kHIDPage_GenericDesktop:
                        switch(usage)
                    {
                        case kHIDUsage_GD_Pointer:
                            break;
                        case kHIDUsage_GD_X:
                        case kHIDUsage_GD_Y:
                        case kHIDUsage_GD_Z:
                        case kHIDUsage_GD_Rx:
                        case kHIDUsage_GD_Ry:
                        case kHIDUsage_GD_Rz:
                            NCount t11 = max - min;
                            NCount t22 = MAX_AXIS - MIN_AXIS;
                            scale = t22 / t11;
                            t11 = max * scale;
                            if(MAX_AXIS > t11)
                                offset = MAX_AXIS - t11;
                            else
                                offset = t11 - MAX_AXIS;
                            mAxisCookies.insert(Npair(cookie, Info(scale, offset)));
                            break;
                        case kHIDUsage_GD_Slider:
                        case kHIDUsage_GD_Dial:
                        case kHIDUsage_GD_Wheel:
                            break;
                        case kHIDUsage_GD_Hatswitch:
                            break;
                    }
                        break;
                    case kHIDPage_Button:
                        mButtonCookies.push_back(cookie);
                        break;
                }
            }
        }
        else
        {
            N_EXCEPT(DeviceInner,
                _T("JoyStick elements could not be copied: copyMatchingElements failed !"));
        }
    }
    //------------------------------------------------------------------------
    IOHIDQueueInterface ** OSXJoyPadControlPattern::create(Nui32 depth)
    {
        IOHIDQueueInterface ** queue = (*mInterface)->allocQueue(mInterface);

        if (queue)
        {
            //create the queue
            IOReturn result = (*queue)->create(queue, 0, depth);

            if(result == kIOReturnSuccess)
            {
                //add elements to the queue
                AxisCookies::iterator ait, aitend = mAxisCookies.end();
                for(ait = mAxisCookies.begin(); ait != aitend; ++ait)
                {
                    result = (*queue)->addElement(queue, ait->first, 0);
                }

                ButtonCookies::iterator bit, bitend = mButtonCookies.end();
                for(bit = mButtonCookies.begin(); bit != bitend; ++bit)
                {
                    result = (*queue)->addElement(queue, (*bit), 0);
                }

                //start data delivery to queue
                result = (*queue)->start(queue);
                if(result == kIOReturnSuccess)
                {
                    return queue;
                }
                else
                {
                    N_EXCEPT(CreateDevice, _T("Queue could not be started !"));
                }
            }
            else
            {
                N_EXCEPT(CreateDevice, _T("Queue could not be created !"));
            }
        }
        else
        {
            N_EXCEPT(CreateDevice, _T("Queue allocation failed !"));
        }
    }
    //------------------------------------------------------------------------
}
}