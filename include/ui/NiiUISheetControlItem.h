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

#include "NiiUIPreInclude.h"
#include "NiiKeyboardControlItem.h"
#include "NiiMouseControlItem.h"
#include "NiiJoyDevControlItem.h"
#include "NiiTouchControlItem.h"

namespace NII
{
namespace UI
{
    class _EngineAPI SheetKeyboardControlItem : public NII_MEDIA::KeyboardControlItem
    {
    public:
        SheetKeyboardControlItem();
    };

    class _EngineAPI SheetMouseControlItem : public NII_MEDIA::MouseControlItem
    {
    public:
        SheetMouseControlItem();
    };

    class _EngineAPI SheetJoyPadControlItem : public NII_MEDIA::JoyPadControlItem
    {
    public:
        SheetJoyPadControlItem();
    };
}
}