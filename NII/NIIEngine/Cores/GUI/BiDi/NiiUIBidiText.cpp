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

#include "NiiPreProcess.h"
#include "NiiUIBidiText.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    BidiText::~BidiText()
    {
    }
    //------------------------------------------------------------------------
    bool BidiText::setSrc(const String & in)
    {
        return reorder(in, mDst);
    }
    //------------------------------------------------------------------------
    const String & BidiText::getDst() const
    {
        return mDst;
    }
    //------------------------------------------------------------------------
    const BidiText::IndexList & BidiText::getLogicalToVisual() const
    {
        return mLTV;
    }
    //------------------------------------------------------------------------
    const BidiText::IndexList & BidiText::getVisualToLogical() const
    {
        return mVTL;
    }
    //------------------------------------------------------------------------
}
}