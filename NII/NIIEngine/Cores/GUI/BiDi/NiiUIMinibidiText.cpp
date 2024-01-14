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

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "CEGUI/Config.h"

#ifdef CEGUI_USE_MINIBIDI

#include "NiiPreProcess.h"
#include "NiiUIMinibidiText.h"

// include minibidi code directly
#include "minibidi.cpp"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    BidiType MinibidiText::getBidiType(Nui32 chr) const
    {
        switch(getType(chr))
        {
        case R:
            return BT_RTL;
            break;
        case L:
            return BT_LTR;
            break;
        default:
            return BT_Normal;
            break;
        }
    }
    //------------------------------------------------------------------------
    bool MinibidiText::reorder(const String & in, String & visual) const
    {
        visual = in;

        if(in.length() <= 1)
            return true;

        mLTV.resize(in.length());
        mVTL.resize(in.length());

        doBidi(visual.ptr(), static_cast<int>(in.length()), true, false, &mVTL[0], &mLTV[0]);

        return true;
    }
    //------------------------------------------------------------------------
}
}
#endif