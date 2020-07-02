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

#ifndef _NII_JOYDEV_SERIAL_ITEM_ARGS_H_
#define _NII_JOYDEV_SERIAL_ITEM_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiSerialItem.h"
#include "NiiJoyDevControlArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevSerialItemArgs : public SerialItemArgs
    {
    public:
        JoyDevSerialItemArgs(JoyDevControlType t) :
            mType(t) {}
        virtual ~JoyDevSerialItemArgs() {}
        
        JoyDevControlType mType;   ///< 类型
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadSerialButtonItemArgs : public JoyDevSerialItemArgs
    {
    public:
        JoyPadSerialButtonItemArgs(Nui8 b);

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const;

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const;

        Nui8 mButton;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickSerialPovItemArgs : public JoyDevSerialItemArgs
    {
    public:
        JoyStickSerialPovItemArgs(Nui8 pov, PovType dir);

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * o) const;

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * o) const;

        Nui8 mPov;                      ///< 
        PovType mDirection;             ///< pov方向
    };
}
}
#endif