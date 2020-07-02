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

#ifndef _NII_MOUSE_PRESS_SERIAL_ITEM_ARGS_H_
#define _NII_MOUSE_PRESS_SERIAL_ITEM_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiSerialItem.h"
#include "NiiMouseControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MousePressSerialItemArgs : public SerialItemArgs
    {
    public:
        MousePressSerialItemArgs(MouseButton b) :
            mButton(b) {}

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const 
        {
            MouseButton src = static_cast<const MousePressArgs *>(o)->mButton;
            if (mButton == src)
                return true;
            return false;
        }

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const 
        {
            MouseButton src = static_cast<const MousePressArgs *>(o)->mButton;
            if (mButton == src)
                return false;
            return true;
        }

        MouseButton mButton;            ///<
    };
}
}

#endif