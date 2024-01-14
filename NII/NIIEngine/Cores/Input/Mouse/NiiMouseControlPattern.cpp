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
#include "NiiMouseControlPattern.h"
#include "NiiInputManager.h"
#include "NiiViewWindow.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    static MouseMethod _nii_ms_press_[8] = { 0 };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MouseArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MouseArgs::MouseArgs() :
        mSrc(0),
        mX(0),
        mY(0),
        mScroll(0)
    {
    }
    //------------------------------------------------------------------------
    MouseArgs::MouseArgs(const MouseArgs * o) :
        mSrc(o->mSrc),
        mX(o->mX),
        mY(o->mY),
        mScroll(o->mScroll),
        mButton(o->mButton){}
    //------------------------------------------------------------------------
    MouseArgs::MouseArgs(const MouseControlPattern * src) :
        mSrc(src),
        mX(0),
        mY(0),
        mScroll(0),
        mButton(MB_Unknow),
        mState(false){}
    //------------------------------------------------------------------------
    MouseArgs::MouseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 z, MouseButton b, bool state) :
        mSrc(src),
        mX(x),
        mY(y),
        mScroll(z),
        mButton(b),
        mState(state){}
    //------------------------------------------------------------------------
    MouseArgs::MouseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll) :
        mSrc(src),
        mX(x),
        mY(y),
        mScroll(scroll){}
    //------------------------------------------------------------------------
    MouseArgs::MouseArgs(const MouseControlPattern * src, MouseButton b, bool state) :
        mSrc(src),
        mX(0),
        mY(0),
        mScroll(0),
        mButton(b),
        mState(state){}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MouseControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MouseControlItem::MouseControlItem() :
        mFactor(1.0f)
    {
    }
    //------------------------------------------------------------------------
    MouseControlItem::~MouseControlItem()
    {
    }
    //------------------------------------------------------------------------
    ControlDevType MouseControlItem::getType() const
    {
        return CDT_Mouse;
    }
    //------------------------------------------------------------------------
    void MouseControlItem::onLeft(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onRight(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onMiddle(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onButton3(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onButton4(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onButton5(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onButton6(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onButton7(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onMove(const MouseArgs * args)
    {

    }
    //------------------------------------------------------------------------
    void MouseControlItem::onButton(const MouseArgs * args)
    {
        Nui32 button = args->mButton;
        if(args->mState)
        {
            raise(args);
        }
        // 引发普通按键事件
        (this->*_nii_ms_press_[button])(args);
    }
    //------------------------------------------------------------------------
    void MouseControlItem::init()
    {
        _nii_ms_press_[0] = &MouseControlItem::onLeft;
        _nii_ms_press_[1] = &MouseControlItem::onRight;
        _nii_ms_press_[2] = &MouseControlItem::onMiddle;
        _nii_ms_press_[3] = &MouseControlItem::onButton3;
        _nii_ms_press_[4] = &MouseControlItem::onButton4;
        _nii_ms_press_[5] = &MouseControlItem::onButton5;
        _nii_ms_press_[6] = &MouseControlItem::onButton6;
        _nii_ms_press_[7] = &MouseControlItem::onButton7;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListenerMouseControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ListenerMouseControlItem::ListenerMouseControlItem()
    {
    }
    //------------------------------------------------------------------------
    ListenerMouseControlItem::~ListenerMouseControlItem()
    {
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onLeft(const MouseArgs * arg)
    {
        mButton = MB_Left;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button left press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onRight(const MouseArgs * arg)
    {
        mButton = MB_Right;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button right press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onMiddle(const MouseArgs * arg)
    {
        mButton = MB_Middle;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button middle press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onButton3(const MouseArgs * arg)
    {
        mButton = MB_Button3;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button3 press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onButton4(const MouseArgs * arg)
    {
        mButton = MB_Button4;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button4 press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onButton5(const MouseArgs * arg)
    {
        mButton = MB_Button5;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button5 press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onButton6(const MouseArgs * arg)
    {
        mButton = MB_Button6;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button6 press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onButton7(const MouseArgs * arg)
    {
        mButton = MB_Button7;
        mButtonPress = arg->mState;
        if(mButtonPress)
            mDescrip = _T("button7 press");
        else
            mDescrip = _T("button left release");
    }
    //------------------------------------------------------------------------
    void ListenerMouseControlItem::onMove(const MouseArgs * arg)
    {
        mX = arg->mX;
        mY = arg->mY;
        mScroll = arg->mScroll;
        mDescrip = _T("button move x, y, scroll");
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SignalMouseControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, SignalMouseControlItem, LeftEvent, 0);
    N_EVENT(CommandObj, SignalMouseControlItem, RightEvent, 1);
    N_EVENT(CommandObj, SignalMouseControlItem, MiddleEvent, 2);
    N_EVENT(CommandObj, SignalMouseControlItem, Button3Event, 3);
    N_EVENT(CommandObj, SignalMouseControlItem, Button4Event, 4);
    N_EVENT(CommandObj, SignalMouseControlItem, Button5Event, 5);
    N_EVENT(CommandObj, SignalMouseControlItem, Button6Event, 6);
    N_EVENT(CommandObj, SignalMouseControlItem, Button7Event, 7);
    N_EVENT(CommandObj, SignalMouseControlItem, MoveEvent, 8);
    //------------------------------------------------------------------------
    SignalMouseControlItem::SignalMouseControlItem()
    {
        notifySignal(SignalMouseControlItem::MoveEvent);
    }
    //------------------------------------------------------------------------
    SignalMouseControlItem::~SignalMouseControlItem()
    {
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onLeft(const MouseArgs * arg)
    {
        signal(PressLeftEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onRight(const MouseArgs * arg)
    {
        signal(PressRightEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onMiddle(const MouseArgs * arg)
    {
        signal(PressMiddleEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onButton3(const MouseArgs * arg)
    {
        signal(PressButton3Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onButton4(const MouseArgs * arg)
    {
        signal(PressButton4Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onButton5(const MouseArgs * arg)
    {
        signal(PressButton5Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onButton6(const MouseArgs * arg)
    {
        signal(PressButton6Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onButton7(const MouseArgs * arg)
    {
        signal(PressButton7Event, arg);
    }
    //------------------------------------------------------------------------
    void SignalMouseControlItem::onMove(const MouseArgs * arg)
    {
        signal(MoveEvent, arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MouseControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MouseControlPattern::MouseControlPattern(Nid id, const ViewWindow * own) :
        ControlPattern(id, own),
        mX(0),
        mY(0),
        mBuffer(0)
    {
        Nui32 depth;
        NIIi left, top;
        own->getArea(left, top, mWindowSizeX, mWindowSizeY, depth);

        mTarget = N_Only(Input).getDummyMouse();
    }
    //------------------------------------------------------------------------
    MouseControlPattern::MouseControlPattern()
    {
    }
    //------------------------------------------------------------------------
    MouseControlPattern::~MouseControlPattern()
    {
        detach();
    }
    //------------------------------------------------------------------------
    void MouseControlPattern::press(MouseButton button)
    {
        mBuffer |= N32_1OrMark[button];
        MouseArgs arg(this, button, true);
        mTarget->onButton(&arg);
    }
    //------------------------------------------------------------------------
    void MouseControlPattern::release(MouseButton button)
    {
        mBuffer &= ~N32_1OrMark[button];
        MouseArgs arg(this, button, false);
        mTarget->onButton(&arg);
    }
    //------------------------------------------------------------------------
    void MouseControlPattern::move(Ni32 x, Ni32 y, Ni32 z)
    {
        Ni32 tempx = mTarget->mFactor * x;
        Ni32 tempy = mTarget->mFactor * z;

        mX += tempx;
        mY += tempy;

        if(mX > mWindowSizeX)
            mX = mWindowSizeX;
        else if(mX < 0)
            mX = 0;

        if(mY > mWindowSizeY)
            mY = mWindowSizeY;
        else if(mY < 0)
            mY = 0;

        MouseArgs arg(this, tempx, tempy, z);
        mTarget->onMove(&arg);
    }
    //------------------------------------------------------------------------
    ControlDevType MouseControlPattern::getType() const
    {
        return CDT_Mouse;
    }
    //------------------------------------------------------------------------
    void MouseControlPattern::apply(ControlItem * item)
    {
        item->setInfluence(this);
        mTarget = static_cast<MouseControlItem *>(item);
    }
    //------------------------------------------------------------------------
    void MouseControlPattern::detach()
    {
        mTarget->setInfluence(NULL);
        mTarget = N_Only(Input).getDummyMouse();
    }
    //------------------------------------------------------------------------
}
}