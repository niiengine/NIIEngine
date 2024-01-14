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
#include "NiiSysOption.h"
#include "NiiViewWindow.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"
#include "NiiInputManager.h"

#if(N_PLAT == N_PLAT_LINUX)
    #include <X11/Xlib.h>
    void GLXProc(NII::ViewWindow * win, const XEvent & event);
#endif

namespace NII
{
    //------------------------------------------------------------------------
    WindowUtil::ListenerList WindowUtil::mListenerList;
    WindowUtil::Windows WindowUtil::mWindowList;
    //------------------------------------------------------------------------
    void WindowUtil::messageProcess()
    {
    #if N_PLAT == N_PLAT_WIN32
        // Windows Message Loop (NULL means check all HWNDs belonging to this context)
        MSG msg;
        while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    #elif N_PLAT == N_PLAT_LINUX
        //GLX Message Pump
        Windows::iterator win, end = mWindowList.end();

        Display * xDisplay = 0; // same for all windows

        for (win = mWindowList.begin(); win != end; ++win)
        {
            XID xid;
            XEvent event;

            if (!xDisplay)
            (*win)->getPlatformData("XDISPLAY", &xDisplay);

            (*win)->getPlatformData("WINDOW", &xid);

            while (XCheckWindowEvent (xDisplay, xid, StructureNotifyMask | VisibilityChangeMask | FocusChangeMask, &event))
            {
                GLXProc(*win, event);
            }

            // The ClientMessage event does not appear under any Event Mask
            while (XCheckTypedWindowEvent (xDisplay, xid, ClientMessage, &event))
            {
                GLXProc(*win, event);
            }
        }
    #elif N_PLAT == N_PLAT_OSX && !defined __OBJC__ && !defined __LP64__
        // OSX Message Pump
        EventRef event = NULL;
        EventTargetRef targetWindow;
        targetWindow = GetEventDispatcherTarget();

        // If we are unable to get the target then we no longer care about events.
        if(!targetWindow)
            return;

        // Grab the next event, process it if it is a window event
        if(ReceiveNextEvent(0, NULL, kEventDurationNoWait, true, &event) == noErr)
        {
            // Dispatch the event
            SendEventToEventTarget(event, targetWindow);
            ReleaseEvent(event);
        }
    #endif
    }
    //------------------------------------------------------------------------
    void WindowUtil::addListener(ViewWindow * win, WindowListener * listener)
    {
        mListenerList.insert(Npair(win, listener));
    }
    //------------------------------------------------------------------------
    void WindowUtil::removeListener(ViewWindow * win, WindowListener * listener)
    {
        ListenerList::iterator i, e = mListenerList.end();

        for(i = mListenerList.begin() ; i != e; ++i)
        {
            if(i->first == win && i->second == listener)
            {
                mListenerList.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void WindowUtil::add(ViewWindow * win)
    {
        mWindowList.push_back(win);
    }
    //------------------------------------------------------------------------
    void WindowUtil::remove(ViewWindow * win)
    {
        Windows::iterator i = std::find(mWindowList.begin(), mWindowList.end(), win);
        if (i != mWindowList.end())
        {
            mWindowList.erase(i);
        }
    }
}
#if N_PLAT == N_PLAT_WIN32
namespace NII
{
    //------------------------------------------------------------------------
    LRESULT CALLBACK WindowUtil::proc
        (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
        {    // Store pointer to Win32Window in user data area
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));
            return 0;
        }

        // look up window instance
        // note: it is possible to get a WM_SIZE before WM_CREATE
        ViewWindow * win = (ViewWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (!win)
            return DefWindowProc(hWnd, uMsg, wParam, lParam);

        //LogManager* log = LogManager::getOnlyPtr();
        //Iterator of all listeners registered to this ViewWindow
        ListenerList::iterator index,
            start = mListenerList.lower_bound(win),
            end = mListenerList.upper_bound(win);

        switch(uMsg)
        {
        case WM_ACTIVATE:
        {
            bool active = (LOWORD(wParam) != WA_INACTIVE);
            if(active)
            {
                win->setActive(true);
                N_Only(Input).active(win);
            }
            else
            {
                if(win->isAutoDeactive())
                {
                    win->setActive(false);
                    N_Only(Input).suspend(win);
                }
            }

            for (; start != end; ++start)
            {
                (start->second)->onFocusChange(win);
            }
            break;
        }
        case WM_SYSKEYDOWN:
            switch(wParam)
            {
            case VK_CONTROL:
            case VK_SHIFT:
            case VK_MENU: //ALT
                //return zero to bypass defProc and signal we processed the message
                return 0;
            }
            break;
        case WM_SYSKEYUP:
            switch(wParam)
            {
            case VK_CONTROL:
            case VK_SHIFT:
            case VK_MENU: //ALT
            case VK_F10:
                //return zero to bypass defProc and signal we processed the message
                return 0;
            }
            break;
        case WM_SYSCHAR:
            // return zero to bypass defProc and signal we processed the message, unless it's an ALT-space
            if (wParam != VK_SPACE)
                return 0;
            break;
        case WM_ENTERSIZEMOVE:
            //log->log("WM_ENTERSIZEMOVE");
            break;
        case WM_EXITSIZEMOVE:
            //log->log("WM_EXITSIZEMOVE");
            break;
        case WM_MOVE:
            //log->log("WM_MOVE");
            win->notify();
            for(index = start; index != end; ++index)
                (index->second)->onMove(win);
            break;
        case WM_DISPLAYCHANGE:
            win->notify();
            for(index = start; index != end; ++index)
                (index->second)->onResize(win);
            break;
        case WM_SIZE:
            //log->log("WM_SIZE");
            win->notify();
            for(index = start; index != end; ++index)
                (index->second)->onResize(win);
            break;
        case WM_GETMINMAXINFO:
            // Prevent the window from going smaller than some minimu size
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;
        case WM_CLOSE:
        {
            //log->log("WM_CLOSE");
            bool close = true;
            for(index = start; index != end; ++index)
            {
                if (!(index->second)->onClosing(win))
                    close = false;
            }
            if (!close) return 0;

            for(index = mListenerList.lower_bound(win); index != end; ++index)
                (index->second)->onClose(win);
            win->destroy();
            return 0;
        }
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}
#elif N_PLAT == N_PLAT_LINUX
    //------------------------------------------------------------------------
void GLXProc(NII::ViewWindow * win, const XEvent & event)
{
    //An iterator for the window listeners
    NII::WindowUtil::ListenerList::iterator index,
        start = NII::WindowUtil::mListenerList.lower_bound(win),
            end   = NII::WindowUtil::mListenerList.upper_bound(win);

    switch(event.type)
    {
    case ClientMessage:
    {
        ::Atom atom;
        win->getPlatformData("ATOM", &atom);
        if(event.xclient.format == 32 && event.xclient.data.l[0] == (NIIl)atom)
        {    //Window closed by window manager
            //Send message first, to allow app chance to unregister things that need done before
            //window is shutdown
            bool close = true;
            for(index = start ; index != end; ++index)
            {
                if (!(index->second)->onClosing(win))
                    close = false;
            }
            if (!close) return;

            for(index = start ; index != end; ++index)
                (index->second)->onClose(win);
            win->destroy();
        }
        break;
    }
    case DestroyNotify:
    {
        if(!win->isClosed())
        {
            // Window closed without window manager warning.
            for(index = start ; index != end; ++index)
                (index->second)->onClose(win);
            win->destroy();
        }
        break;
    }
    case ConfigureNotify:
    {
        // This could be slightly more efficient if notify took arguments:
        Nui32 oldWidth, oldHeight, oldDepth;
        NIIi oldLeft, oldTop;
        win->getArea(oldLeft, oldTop, oldWidth, oldHeight, oldDepth);
        win->notify();

        Nui32 newWidth, newHeight, newDepth;
        NIIi newLeft, newTop;
        win->getArea(newLeft, newTop, newWidth, newHeight, newDepth);

        if(newLeft != oldLeft || newTop != oldTop)
        {
            for(index = start ; index != end; ++index)
                (index->second)->onMove(win);
        }

        if(newWidth != oldWidth || newHeight != oldHeight)
        {
            for(index = start ; index != end; ++index)
                (index->second)->onResize(win);
        }
        break;
    }
    case FocusIn:     // Gained keyboard focus
    case FocusOut:    // Lost keyboard focus
        for(index = start ; index != end; ++index)
            (index->second)->onFocusChange(win);
        break;
    case MapNotify:   //Restored
        win->setActive(true);
        N_Only(ControlPattern).active(win);
        for(index = start ; index != end; ++index)
            (index->second)->onFocusChange(win);
        break;
    case UnmapNotify: //Minimised
        win->setActive(false);
        N_Only(ControlPattern).suspend(win);
        win->setVisible(false);
        for(index = start ; index != end; ++index)
            (index->second)->onFocusChange(win);
        break;
    case VisibilityNotify:
        switch(event.xvisibility.state)
        {
        case VisibilityUnobscured:
            win->setActive(true);
            N_Only(ControlPattern).active(win);
            win->setVisible(true);
            break;
        case VisibilityPartiallyObscured:
            win->setActive(true);
            N_Only(ControlPattern).active(win);
            win->setVisible(true);
            break;
        case VisibilityFullyObscured:
            win->setActive(false);
            N_Only(ControlPattern).suspend(win);
            win->setVisible(false);
            break;
        }
        for(index = start ; index != end; ++index)
            (index->second)->onFocusChange(win);
        break;
    default:
        break;
    } //End switch event.type
}
#elif N_PLAT == N_PLAT_OSX && !defined __OBJC__ && !defined __LP64__
namespace NII
{
    //------------------------------------------------------------------------
    OSStatus WindowUtil::_CarbonWindowHandler(EventHandlerCallRef nextHandler,
        EventRef event, void * wnd)
    {
        OSStatus status = noErr;

        // Only events from our window should make it here
        // This ensures that our user data is our WindowRef
        ViewWindow * curWindow = (ViewWindow*)wnd;
        if(!curWindow) return eventNotHandledErr;

        //Iterator of all listeners registered to this ViewWindow
        ListenerList::iterator index,
            start = mListenerList.lower_bound(curWindow),
            end = mListenerList.upper_bound(curWindow);

        // We only get called if a window event happens
        UInt32 eventKind = GetEventKind(event);

        switch(eventKind)
        {
            case kEventWindowActivated:
                curWindow->setActive(true);
                N_Only(ControlPattern).active(win);
                for(; start != end; ++start)
                    (start->second)->onFocusChange(curWindow);
                break;
            case kEventWindowDeactivated:
                if(curWindow->isAutoDeactive())
                {
                    curWindow->setActive(false);
                    N_Only(ControlPattern).suspend(win);
                }

                for(; start != end; ++start)
                    (start->second)->onFocusChange(curWindow);
                break;
            case kEventWindowShown:
            case kEventWindowExpanded:
                curWindow->setActive(true);
                N_Only(ControlPattern).active(win);
                curWindow->setVisible(true);
                for(; start != end; ++start)
                    (start->second)->onFocusChange(curWindow);
                break;
            case kEventWindowHidden:
            case kEventWindowCollapsed:
                curWindow->setActive(false);
                N_Only(ControlPattern).suspend(win);
                curWindow->setVisible(false);
                for( ; start != end; ++start)
                    (start->second)->onFocusChange(curWindow);
                break;
            case kEventWindowDragCompleted:
                curWindow->notify();
                for( ; start != end; ++start)
                    (start->second)->onMove(curWindow);
                break;
            case kEventWindowBoundsChanged:
                curWindow->notify();
                for( ; start != end; ++start)
                    (start->second)->onResize(curWindow);
                break;
            case kEventWindowClose:
            {
                bool close = true;
                for( ; start != end; ++start)
                {
                    if (!(start->second)->onClosing(curWindow))
                        close = false;
                }
                if (close)
                    // This will cause event handling to continue on to the standard handler, which calls
                    // DisposeWindow(), which leads to the 'kEventWindowClosed' event
                    status = eventNotHandledErr;
                break;
            }
            case kEventWindowClosed:
                curWindow->destroy();
                for( ; start != end; ++start)
                    (start->second)->onClose(curWindow);
                break;
            default:
                status = eventNotHandledErr;
                break;
        }
        return status;
    }
}
    //------------------------------------------------------------------------
#endif
