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

#include "NiiPreInclude.h"

#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>

//Max number of elements to collect from buffered DirectInput
#define KEYBOARD_DX_BUFFERSIZE 10
#define MOUSE_DX_BUFFERSIZE 20
#define JOYSTICK_DX_BUFFERSIZE 30

//MinGW defines
#if defined(__MINGW__)
    #undef FIELD_OFFSET
    #define FIELD_OFFSET offsetof
#endif

namespace NII
{
namespace NII_MEDIA
{
    //Local Forward declarations
    class Win32ControlPatternManager;
    class Win32KeyboardControlPattern;
    class Win32JoyStickControlPattern;
    class Win32MouseControlPattern;
    class Win32JoyStickForceFeedback;
}
}