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

#ifndef _NII_POV_DIRECTION_H_
#define _NII_POV_DIRECTION_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_MEDIA
{
    enum PovType
    {
        PT_Center    = 0,
        PT_North     = 0x01,
        PT_South     = 0x02,
        PT_East      = 0x04,
        PT_NorthEast = 0x05,
        PT_SouthEast = 0x06,
        PT_West      = 0x08,
        PT_NorthWest = 0x09,
        PT_SouthWest = 0x0A
    };
}
}
#endif
