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
#ifndef _NII_UI_FRIBIDITEXT_H_
#define _NII_UI_FRIBIDITEXT_H_

#include "NiiUIPreInclude.h"
#include "NiiUIBidiText.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class FribidiText : public BidiText
    {
    public:
        BidiType getBidiType(Nui32 chr) const;

        bool reorder(const String & in, String & out) const;
    };
}
}
#endif
