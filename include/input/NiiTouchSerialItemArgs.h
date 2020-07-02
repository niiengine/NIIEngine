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
#include "NiiSerialItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIIEngine 
    */
    class _EngineAPI TouchSerialItemArgs : public SerialItemArgs
    {
    public:
        TouchSerialItemArgs() {}
        ~TouchSerialItemArgs() {}

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const 
        {
            const TouchSerialItemArgs * temp = static_cast<const TouchSerialItemArgs *>(o);
            if (temp->dest == dest)
                return true;
            return false;
        }

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const 
        {
            const TouchSerialItemArgs * temp = static_cast<const TouchSerialItemArgs *>(o);
            if (temp->dest != dest)
                return true;
            return false;
        }

        Vector3<NIIi> dest;  ///< Ä¿±êÎ»ÖÃ
    };
}
}