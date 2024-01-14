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
#include "NiiUIWidgetControlItem.h"
#include "NiiUIWindow.h"

namespace NII
{
namespace UI
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // KeyBoardControlItem
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    KeyBoardControlItem::KeyBoardControlItem() {}
    //-----------------------------------------------------------------------
    KeyBoardControlItem::~KeyBoardControlItem() {}
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MouseControlItem
    //-----------------------------------------------------------------------
    //------------------------------------------------------------------------
    MouseControlItem::MouseControlItem() :
        mRepeatButton(MB_Unknow),
        mRepeatElapse(0),
        mRepeat(false)
    {
    }
    //------------------------------------------------------------------------
    MouseControlItem::~MouseControlItem()
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::apply(Widget * dst)
    {
        mDest = dst;
    }
    //------------------------------------------------------------------------
    void MouseControlItem::update(TimeDurMS elapse)
    {
        // Mouse button autorepeat processing.
        if(mDest->isClickAutoEvent() && mRepeatButton != MB_Unknow)
        {
            mRepeatElapse += elapse;

            if(mRepeat)
            {
                if (mRepeatElapse > mDest->getClickRate())
                {
                    mRepeatElapse -= mDest->getClickRate();
                    // trigger the repeated event
                    rise(mRepeatButton);
                }
            }
            else
            {
                if (mRepeatElapse > mDest->getClickThreshold())
                {
                    mRepeatElapse = 0;
                    mRepeat = true;
                    // trigger the repeated event
                    rise(mRepeatButton);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MouseControlItem::onPress(const MouseArgs * arg)
    {
        // if auto repeat is enabled and we are not currently tracking
        // the button that was just pushed (need this button check because
        // it could be us that generated this event via auto-repeat).
        if(mDest->isClickAutoEvent())
        {
            if(mRepeatButton == MB_Unknow)
                mDest->focus();

            if((mRepeatButton != arg->mButton) && mDest->isFocus())
            {
                mRepeatButton = arg->mButton;
                mRepeatElapse = 0;
                mRepeat = false;
            }
        }
    }
    //------------------------------------------------------------------------
    void MouseControlItem::onRelease(const MouseArgs * arg)
    {
        if(mDest->isClickAutoEvent() && mRepeatButton != MB_Unknow)
        {
            mDest->lost();
            mRepeatButton = MB_Unknow;
        }
    }
    //------------------------------------------------------------------------
    void MouseControlItem::onMove(const MouseArgs * args)
    {
        Window * win = mDest->getParent()->getWindow();
        Vector2f pos = win->affine(win->getSheet()->getCursor()->getPos());
        CursorEventArgs ma(args, mDest);
        ma.mLocalX = pos.x;
        ma.mLocalY = pos.y;
    }
    //------------------------------------------------------------------------
    void MouseControlItem::rise(MouseButton button)
    {
        Window * win = mDest->getParent()->getWindow();
        Vector2f pos = win->affine(win->getSheet()->getCursor()->getPos());
        CursorEventArgs ma(mDest);
        ma.mLocalX = pos.x;
        ma.mLocalY = pos.y;
        ma.mMouseArg.mButton = button;
        mDest->onButtonDown(&ma);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JoyPadControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyPadControlItem::JoyPadControlItem() {}
    //------------------------------------------------------------------------
    JoyPadControlItem::~JoyPadControlItem() {}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TouchControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TouchControlItem::TouchControlItem()
    {
    }
    //------------------------------------------------------------------------
    TouchControlItem::~TouchControlItem()
    {

    }
    //------------------------------------------------------------------------
}
}