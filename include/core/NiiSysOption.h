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

#ifndef _NII_SYS_OPTION_H_
#define _NII_SYS_OPTION_H_

#include "NiiPreInclude.h"
#include "NiiString.h"
#include "NiiPropertyObj.h"

#if N_PLAT == N_PLAT_WIN32
    #include <windows.h>
#elif N_PLAT == N_PLAT_OSX && !defined(__LP64__)
    #include <Carbon/Carbon.h>
#endif

namespace NII 
{
    /** 系统选项
    @version NIIEngine 3.0.0
    */
    typedef struct _SysOption
    {
        PropertyID mID;
        String mName;
        String mValue;
        StringList mReserve;
        String mDescription;
        bool mReserveEnable;
    } SysOption;

    typedef map<PropertyID, SysOption>::type SysOptionList;

    /** 窗体基本定义
    @version NIIEngine 3.0.0
    */
    struct WindowDefine
    {
        String mName;
        NCount mWidth;
        NCount mHeight;
        PropertyData mCustom;
        bool mFull;
    };

    typedef vector<WindowDefine>::type WindowDefineList;

    /** 窗体监听类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WindowListener
    {
    public:
        virtual ~WindowListener() {};

        /** 窗口移动位置
        @version NIIEngine 3.0.0
        */
        virtual void onMove(ViewWindow *) {}

        /** 窗口大小调整
        @version NIIEngine 3.0.0
        */
        virtual void onResize(ViewWindow *) {}

        /** 窗口正在关闭
        @return True 将关闭窗体(默认)
        @version NIIEngine 3.0.0
        */
        virtual bool onClosing(ViewWindow *) { return true; }

        /** 窗口已关闭
        @param[in] rw 创建这个事件的RenderWindow
        @version NIIEngine 3.0.0
        */
        virtual void onClose(ViewWindow *) {}

        /** 窗口已失去/获得 焦点
        @version NIIEngine 3.0.0
        */
        virtual void onFocusChange(ViewWindow *) {}
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WindowUtil
    {
    public:
        /** 处理消息
        @note 一般每帧处理一次
        @version NIIEngine 3.0.0
        */
        static void messageProcess();

        /** 添加渲染窗体
        @version NIIEngine 3.0.0
        */
        static void add(ViewWindow * win);

        /** 移去渲染窗体
        @version NIIEngine 3.0.0
        */
        static void remove(ViewWindow * win);

        /** 添加监听器
        @version NIIEngine 3.0.0
        */
        static void addListener(ViewWindow * win, WindowListener * listener);

        /** 移去监听器
        @version NIIEngine 3.0.0
        */
        static void removeListener(ViewWindow * win, WindowListener * listener);

#if N_PLAT == N_PLAT_WIN32
        static LRESULT CALLBACK proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif N_PLAT == N_PLAT_OSX && !defined __OBJC__ && !defined(__LP64__)
        static OSStatus proc(EventHandlerCallRef nextHandler, EventRef event, void* wnd);
#endif
    public:
        typedef multimap<ViewWindow *, WindowListener *>::type ListenerList;
        typedef vector<ViewWindow *>::type Windows;

        static ListenerList mListenerList;
        static Windows mWindowList;
    };
}
#endif