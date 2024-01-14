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
#include "NiiTouchControlPattern.h"
#include "NiiInputManager.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    TouchControlItem::TouchControlItem() : ControlItem(false) {}
    //------------------------------------------------------------------------
    void TouchControlItem::onPress(const TouchControlArgs * args)
    {
    }
    //------------------------------------------------------------------------
    void TouchControlItem::onRelease(const TouchControlArgs * args)
    {
    }
    //------------------------------------------------------------------------
    void TouchControlItem::onMove(const TouchControlArgs * args)
    {
    }
    //------------------------------------------------------------------------
    void TouchControlItem::onCancel(const TouchControlArgs * args)
    {
    }
    //------------------------------------------------------------------------
    ControlDevType TouchControlItem::getType() const
    {
        return CDT_Touch;
    }
    //------------------------------------------------------------------------
    void TouchControlItem::raise(const EventArgs * arg)
    {
        if (mSerial)
        {

        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TouchControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TouchControlPattern::TouchControlPattern(){}
    //------------------------------------------------------------------------
    TouchControlPattern::TouchControlPattern(Nid id, const ViewWindow * main) :
        ControlPattern(id, main){}
    //------------------------------------------------------------------------
    TouchControlPattern::~TouchControlPattern()
    {
        detach();
    }
    //------------------------------------------------------------------------
    void TouchControlPattern::apply(ControlItem * item)
    {
        item->setInfluence(this);
        mTarget = static_cast<TouchControlItem *>(item);
    }
    //------------------------------------------------------------------------
    void TouchControlPattern::detach()
    {
        mTarget->setInfluence(0);
        mTarget = N_Only(Input).getDummyTouch();
    }
    //------------------------------------------------------------------------
    ControlDevType TouchControlPattern::getType() const
    {
        return CDT_Touch;
    }
    //------------------------------------------------------------------------
}
}