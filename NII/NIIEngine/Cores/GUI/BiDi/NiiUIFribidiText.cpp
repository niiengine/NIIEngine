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

#ifdef CEGUI_USE_FRIBIDI

#include "NiiUIFribidiText.h"
#include <fribidi.h>

#ifdef _MSC_VER
    #pragma comment(lib,"fribidi_dll")
#endif

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    BidiType FribidiText::getBidiType(Nui32 chr) const
    {
        switch(fribidi_get_type((FriBidiChar)chr))
        {
        case FRIBIDI_TYPE_RTL:
            return BT_RTL;
            break;

        case FRIBIDI_TYPE_LTR:
            return BT_LTR;
            break;

        case FRIBIDI_TYPE_ON:
        default:
            return BT_Normal;
            break;
        }
    }
    //------------------------------------------------------------------------
    bool FribidiText::reorder(const String & in, String & visual) const
    {
        visual = in;

        if(in.length() <= 1)
            return true;

        FriBidiCharType input_base_direction = FRIBIDI_TYPE_L;
        mLTV.resize(in.length());
        mVTL.resize(in.length());
        String logicalNotConst(in);

        fribidi_boolean res = fribidi_log2vis(static_cast<FriBidiChar *>(logicalNotConst.ptr()),
            static_cast<FriBidiStrIndex>(in.length()),
            &input_base_direction,
            /* output */
            static_cast<FriBidiChar *>(visual.ptr()),
            &mLTV[0], &mVTL[0], 0);

        // success?
        if(res)
            return true;

        // log failure and continue anyway :-p
        N_Only(Log).log(_I18n("FribidiText::reorder: fribidi_log2vis ")
            _I18n("call failed on logical string: ") + in);

        return false;
    }
    //------------------------------------------------------------------------
}
}
#endif