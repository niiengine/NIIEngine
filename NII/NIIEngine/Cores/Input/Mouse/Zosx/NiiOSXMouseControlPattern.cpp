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
#include "NiiOSXMouseControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    OSStatus MouseMoveCallBack(EventHandlerCallRef handler, EventRef event, void * src)
    {
        ((OSXMouseControlPattern *)src)->onMouseMove(event);

        return CallNextEventHandler(handler, event);
    }
    //------------------------------------------------------------------------
    OSStatus MousePressCallBack(EventHandlerCallRef handler, EventRef event, void * src)
    {
        ((OSXMouseControlPattern *)src)->onMousePress(event);

        return CallNextEventHandler(handler, event);
    }
    //------------------------------------------------------------------------
    OSStatus MouseReleaseCallBack(EventHandlerCallRef handler, EventRef event, void * src)
    {
        ((OSXMouseControlPattern *)src)->onMouseRelease(event);

        return CallNextEventHandler(handler, event);
    }
    //------------------------------------------------------------------------
    OSStatus MouseWheelCallBack(EventHandlerCallRef handler, EventRef event, void * src)
    {
        ((OSXMouseControlPattern *)src)->onMouseWheel(event);

        return CallNextEventHandler(handler, event);
    }
    //------------------------------------------------------------------------
    OSStatus WindowFocusCallBack(EventHandlerCallRef handler, EventRef event, void * src)
    {
        OSXMouseControlPattern * temp = ((OSXMouseControlPattern *)src);

        temp->mFocusChange = true;
        CGAssociateMouseAndMouseCursorPosition(TRUE);

        return CallNextEventHandler(handler, event);
    }
    //------------------------------------------------------------------------
    const EventTypeSpec MacMouseMove[] =
    {
        {kEventClassMouse, kEventMouseMoved},
        {kEventClassMouse, kEventMouseDragged}
    };

    const EventTypeSpec MacMousePress[] =
    {
        {kEventClassMouse, kEventMouseDown}
    };

    const EventTypeSpec MacMouseRelease[] =
    {
        {kEventClassMouse, kEventMouseUp}
    };

    const EventTypeSpec MacMouseWheel[] =
    {
        {kEventClassMouse, kEventMouseWheelMoved}
    };

    const EventTypeSpec MacWinFocus[] =
    {
        {kEventClassApplication, kEventAppDeactivated}
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // OSXMouseControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    OSXMouseControlPattern::OSXMouseControlPattern(){}
    //------------------------------------------------------------------------
    OSXMouseControlPattern::OSXMouseControlPattern(Nid id) :
        MouseControlPattern(id),
        mFocusChange(false),
        mMouseMoveRef(NULL),
        mMousePressRef(NULL),
        mMouseReleaseRef(NULL),
        mMouseWheelRef(NULL),
        mWindowFocusEventRef(NULL).
        WindowRef(NULL)
    {
        // Get a "Univeral procedure pointer" for our callback
        mMouseMove = NewEventHandlerUPP(MouseMoveCallBack);
        mMousePress = NewEventHandlerUPP(MousePressCallBack);
        mMouseRelease = NewEventHandlerUPP(MouseReleaseCallBack);
        mMouseWheel = NewEventHandlerUPP(MouseWheelCallBack);
        mWindowFocus = NewEventHandlerUPP(WindowFocusCallBack);

        setVendor("CallBack");
    }
    //------------------------------------------------------------------------
    OSXMouseControlPattern::~OSXMouseControlPattern()
    {
        if(mMouseMoveRef != NULL)
            RemoveEventHandler(mMouseMoveRef);
        if(mMousePressRef != NULL)
            RemoveEventHandler(mMousePressRef);
        if(mMouseReleaseRef != NULL)
            RemoveEventHandler(mMouseReleaseRef);
        if(mMouseWheelRef != NULL)
            RemoveEventHandler(mMouseWheelRef);
        if(mWindowFocusEventRef != NULL)
            RemoveEventHandler(mWindowFocusEventRef);

        DisposeEventHandlerUPP(mMouseMove);
        DisposeEventHandlerUPP(mMousePress);
        DisposeEventHandlerUPP(mMouseRelease);
        DisposeEventHandlerUPP(mMouseWheel);
        DisposeEventHandlerUPP(mWindowFocus);

        // Restore Mouse
        CGAssociateMouseAndMouseCursorPosition(TRUE);
        CGDisplayShowCursor(kCGDirectMainDisplay);
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::onMouseMove(EventRef event)
    {
        if(!mActive || mFocusChange)
            return;

        if(!mRelationChange)
        {
            HIPoint delta = {0.0f, 0.0f};

            GetEventParameter(event, kEventParamMouseDelta, typeHIPoint,
                NULL, sizeof(HIPoint), NULL, &delta);

            mTempX += delta.x;
            mTempY += delta.y;
        }

        mRelationChange = false;
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::onMousePress(EventRef event)
    {
        if(!mActive || mFocusChange)
            return;

        EventMouseButton button = 0;
        UInt32 modifiers = 0;

        // Capture parameters
        GetEventParameter(event, kEventParamMouseButton, typeMouseButton,
            NULL, sizeof(EventMouseButton), NULL, &button);

        GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL,
            sizeof(UInt32), NULL, &modifiers);

        if((button == kEventMouseButtonTertiary) || ((button == kEventMouseButtonPrimary) && (modifiers & optionKey)))
        {
            press(MB_Middle);
        }
        else if((button == kEventMouseButtonSecondary) || ((button == kEventMouseButtonPrimary) && (modifiers & controlKey)))
        {
            press(MB_Right);
        }
        else if(button == kEventMouseButtonPrimary)
        {
            press(MB_Left);
        }
        else
        {
            press(MB_Button3);
        }
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::onMouseRelease(EventRef event)
    {
        if(!mActive)
            return;

        EventMouseButton button = 0;
        UInt32 modifiers = 0;

        if(mFocusChange)
        {
            mFocusChange = false;
            CGAssociateMouseAndMouseCursorPosition(false);

            if(mWin)
            {
                Rect clip = {0.0f, 0.0f, 0.0f, 0.0f};
                GetWindowBounds(mWin, kWindowContentRgn, &clip);

                CGPoint warpPoint;
                warpPoint.x = ((clip.right - clip.left) / 2) + clip.left;
                warpPoint.y = ((clip.bottom - clip.top) / 2) + clip.top;
                CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint); //Place at display origin

                CGDisplayHideCursor(kCGDirectMainDisplay);

                mRelationChange = true;
            }

            //Once we regain focus, we do not really know what state all the buttons are in - for now, set to not pressed. todo, check current status
            //compare against old status, and send off any needed events
            mBuffer = 0;

            break;
        }

        // Capture parameters
        GetEventParameter(event, kEventParamMouseButton, typeMouseButton, NULL,
            sizeof(EventMouseButton), NULL, &button);

        GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL,
            sizeof(UInt32), NULL, &modifiers);

        if ((button == kEventMouseButtonTertiary) || ((button == kEventMouseButtonPrimary) && (modifiers & optionKey)))
        {
            release(MB_Middle);
        }
        else if ((button == kEventMouseButtonSecondary) || ((button == kEventMouseButtonPrimary) && (modifiers & controlKey)))
        {
            release(MB_Right);
        }
        else if (button == kEventMouseButtonPrimary)
        {
            release(MB_Left);
        }
        else
        {
            release(MB_Button3);
        }
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::onMouseWheel(EventRef event)
    {
        if(!mActive)
            return;

        SInt32 wheelDelta = 0;
        EventMouseWheelAxis	wheelAxis = 0;

        // Capture parameters
        GetEventParameter(event, kEventParamMouseWheelAxis, typeMouseWheelAxis,
            NULL, sizeof(EventMouseWheelAxis), NULL, &wheelAxis);

        GetEventParameter(event, kEventParamMouseWheelDelta, typeSInt32, NULL,
            sizeof(SInt32), NULL, &wheelDelta);

        // If the Y axis of the wheel changed, then update the Z
        // Does OIS care about the X wheel axis?
        if(wheelAxis == kEventMouseWheelAxisY)
            mTempZ += (wheelDelta * 60);
    }
    //------------------------------------------------------------------------
    NIIb OSXMouseControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb OSXMouseControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::_init()
    {
        OSStatus status;
        mX = 0;
        mY = 0;
        mTempX = 0;
        mTempY = 0;
        mTempZ = 0;

        mRelationChange = false;

        // Hide OS Mouse
        CGDisplayHideCursor(kCGDirectMainDisplay);

        //Now that mouse is warped, start listening for events
        EventTargetRef event = GetApplicationEventTarget();
        if(event == NULL)
            N_EXCEPT(EEC_I_Create_Inner, _T("Window event target error !"));

        if(mMouseMoveRef != NULL)
            RemoveEventHandler(mMouseMoveRef);

        if(mMousePressRef != NULL)
            RemoveEventHandler(mMousePressRef);

        if(mMouseReleaseRef != NULL)
            RemoveEventHandler(mMouseReleaseRef);

        if(mMouseWheelRef != NULL)
            RemoveEventHandler(mMouseWheelRef);

        if(mWindowFocusEventRef != NULL)
            RemoveEventHandler(mWindowFocusEventRef);

        mMouseMoveRef = mMousePressRef = mMouseReleaseRef = mMouseWheelRef = mWindowFocusEventRef = NULL;

        status = InstallEventHandler(event, mMouseMove, GetEventTypeCount(MacMouseMove), MacMouseMove, this, &mMouseMoveRef);
        if(status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading Mouse event handler error !"));

        status = InstallEventHandler(event, mMousePress, GetEventTypeCount(MacMousePress), MacMousePress, this, &mMousePressRef);
        if(status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading Mouse event handler error !"));

        status = InstallEventHandler(event, mMouseRelease, GetEventTypeCount(MacMouseRelease), MacMouseRelease, this, &mMouseReleaseRef);
        if(status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading Mouse event handler error !"));

        status = InstallEventHandler(event, mMouseWheel, GetEventTypeCount(MacMouseWheel), MacMouseWheel, this, &mMouseWheelRef);
        if(status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading Mouse event handler error !"));

        status = InstallEventHandler(event, mWindowFocus, GetEventTypeCount(MacWinFocus), MacWinFocus, this, &mWindowFocusEventRef);
        if(status != noErr)
            N_EXCEPT(CreateDevice, _T("Loading window event handler error !"));

        //Lock OS Mouse movement
        mFocusChange = false;
        CGAssociateMouseAndMouseCursorPosition(FALSE);
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::_update()
    {
        if(mTempZ || mTempX || mTempY)
        {
            move(mTempX, mTempY, mTempZ);
            mTempX = 0;
            mTempY = 0;
            mTempZ = 0;
        }
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::_reset()
    {
        OSStatus status;
        mX = 0;
        mY = 0;
        mTempX = 0;
        mTempY = 0;
        mTempZ = 0;

        mRelationChange = false;

        // Hide OS Mouse
        CGDisplayHideCursor(kCGDirectMainDisplay);

        mOwn->getPlatformData("WINDOW", &mWin);

        if(mWin)
        {
            Rect clip = {0.0f, 0.0f, 0.0f, 0.0f};
            GetWindowBounds(mWin, kWindowContentRgn, &clip);

            CGPoint warpPoint;
            warpPoint.x = ((clip.right - clip.left) / 2) + clip.left;
            warpPoint.y = ((clip.bottom - clip.top) / 2) + clip.top;
            CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint); //Place at display origin

            mRelationChange = true;
        }

        mFocusChange = false;
        CGAssociateMouseAndMouseCursorPosition(FALSE);
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void OSXMouseControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID OSXMouseControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}
}