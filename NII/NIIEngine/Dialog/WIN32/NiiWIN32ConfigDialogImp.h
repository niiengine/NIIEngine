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

#ifndef _NII_Win32ConfigDialog_H_
#define _NII_Win32ConfigDialog_H_

#include "NiiPreInclude.h"
#include "windows.h"

#if N_ARCH_64 == N_ARCH
#define PROCOUT INT_PTR
#else
#define PROCOUT BOOL
#endif

namespace NII
{
    /** 定义自动渲染配置的对话框
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ConfigDialog : public UtilityAlloc
    {
    public:
        ConfigDialog();
        ~ConfigDialog();

        bool display();
    protected:
        static PROCOUT CALLBACK proc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
    protected:
        RenderSys * mCurrentSys;
        HINSTANCE mHInstance;
    };
}
#endif
