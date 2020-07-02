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

#ifndef _NII_CONTROL_DEV_TYPE_H_
#define _NII_CONTROL_DEV_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ¿ØÖÆÆ÷ÀàÐÍ
    @version NIIEngine 3.0.0
    */
    enum ControlDevType
    {
        CDT_Keyboard = 1,
        CDT_Mouse = 2,
        CDT_JoyStick = 3,
        CDT_JoyPad = 4,
        CDT_JoyHandle = 5,
        CDT_JoyWheel = 6,
        CDT_Touch = 7
    };
}
#endif