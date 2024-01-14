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
#include "NiiJoyPadControlPattern.h"
#include "NiiInputManager.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SignalJoyPadControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, SignalJoyPadControlItem, ButtonEvent, 0);
    N_EVENT(CommandObj, SignalJoyPadControlItem, TwistAxisEvent, 1);
    N_EVENT(CommandObj, SignalJoyPadControlItem, SlipSliderEvent, 2);
    N_EVENT(CommandObj, SignalJoyPadControlItem, PushPOVEvent, 3);
    N_EVENT(CommandObj, SignalJoyPadControlItem, EventCount, 4);
    //------------------------------------------------------------------------
    SignalJoyPadControlItem::SignalJoyPadControlItem()
    {
        notifySignal(SignalJoyPadControlItem::EventCount);
    }
    //------------------------------------------------------------------------
    SignalJoyPadControlItem::~SignalJoyPadControlItem() {}
    //------------------------------------------------------------------------
    void SignalJoyPadControlItem::onButton(const JoyPadControlButtonArgs * args)
    {
        signal(ButtonEvent, args);
    }
    //------------------------------------------------------------------------
    void SignalJoyPadControlItem::onTwistAxis(const JoyPadControlAxisArgs * args)
    {
        signal(TwistAxisEvent, args);
    }
    //------------------------------------------------------------------------
    void SignalJoyPadControlItem::onSlipSlider(const JoyStickControlSliderArgs * arg)
    {
        signal(SlipSliderEvent, arg);
    }
    //------------------------------------------------------------------------
    void SignalJoyPadControlItem::onPushPOV(const JoyStickControlPovArgs * arg)
    {
        signal(PushPOVEvent, arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SignalJoyHandleControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(SignalJoyPadControlItem, SignalJoyHandleControlItem, HandleVectorEvent, 0);
    N_EVENT(SignalJoyPadControlItem, SignalJoyHandleControlItem, EventCount, 1);
    //------------------------------------------------------------------------
    SignalJoyHandleControlItem::SignalJoyHandleControlItem()
    {
        notifySignal(SignalJoyHandleControlItem::EventCount);
    }
    //------------------------------------------------------------------------
    SignalJoyHandleControlItem::~SignalJoyHandleControlItem()
    {
    }
    //------------------------------------------------------------------------
    void SignalJoyHandleControlItem::onHandleVector(const JoyHandleControlVectorArgs * arg)
    {
        signal(HandleVectorEvent, arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SignalJoyWheelControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(SignalJoyPadControlItem, SignalJoyWheelControlItem, WheelEvent, 0);
    N_EVENT(SignalJoyPadControlItem, SignalJoyWheelControlItem, EventCount, 1);
    //------------------------------------------------------------------------
    SignalJoyWheelControlItem::SignalJoyWheelControlItem()
    {
        notifySignal(SignalJoyWheelControlItem::EventCount);
    }
    //------------------------------------------------------------------------
    SignalJoyWheelControlItem::~SignalJoyWheelControlItem() {}
    //------------------------------------------------------------------------
    void SignalJoyWheelControlItem::onWheel(const JoyWheelControlArgs * args)
    {
        signal(WheelEvent, args);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JoyPadSerialButtonItemArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyPadSerialButtonItemArgs::JoyPadSerialButtonItemArgs(Nui8 b) :
        JoyDevSerialItemArgs(JDCT_Button),
        mButton(b) {}
    //------------------------------------------------------------------------
    bool JoyPadSerialButtonItemArgs::operator == (const EventArgs * o) const
    {
        const JoyPadControlButtonArgs * temp =
            static_cast<const JoyPadControlButtonArgs *>(o);

        if ((mType == temp->mType) && (mButton == temp->mButton))
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool JoyPadSerialButtonItemArgs::operator != (const EventArgs * o) const
    {
        const JoyPadControlButtonArgs * temp =
            static_cast<const JoyPadControlButtonArgs *>(o);

        if ((mType == temp->mType) && (mButton == temp->mButton))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JoyStickSerialPovItemArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyStickSerialPovItemArgs::JoyStickSerialPovItemArgs(Nui8 pov, PovType dir) :
        JoyDevSerialItemArgs(JDCT_Pov),
        mPov(pov),
        mDirection(dir) {}
    //------------------------------------------------------------------------
    bool JoyStickSerialPovItemArgs::operator == (const EventArgs * o) const
    {
        const JoyStickControlPovArgs * temp =
            static_cast<const JoyStickControlPovArgs *>(o);

        if ((temp->mPov == mPov) && (temp->mDirection == mDirection) &&
            (temp->mType == mType))
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool JoyStickSerialPovItemArgs::operator != (const EventArgs * o) const
    {
        const JoyStickControlPovArgs * temp =
            static_cast<const JoyStickControlPovArgs *>(o);

        if ((temp->mPov == mPov) && (temp->mDirection == mDirection) &&
            (temp->mType == mType))
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JoyPadControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    const NIIi JoyPadControlPattern::MIN_AXIS = -32768;
    //------------------------------------------------------------------------
    const NIIi JoyPadControlPattern::MAX_AXIS = 32767;
    //------------------------------------------------------------------------
    JoyPadControlPattern::JoyPadControlPattern()
    {
    }
    //------------------------------------------------------------------------
    JoyPadControlPattern::JoyPadControlPattern(Nid id, const ViewWindow * own) :
        ControlPattern(id, own)
    {
    }
    //------------------------------------------------------------------------
    JoyPadControlPattern::~JoyPadControlPattern()
    {
        mButtonBuffer.clear();
        mAxisBuffer.clear();
        mPovBuffer.clear();
        mSliderBuffer.clear();
        detach();
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::apply(JoyStickEffect * eff)
    {

    }
    //------------------------------------------------------------------------
    bool JoyPadControlPattern::isPress(Nui8 key)
    {
        return ((mButtonBuffer[key >> 5] & N32_1OrMark[key % 32]) == 0) ? false : true;
    }
    //------------------------------------------------------------------------
    bool JoyPadControlPattern::isRelease(Nui8 key)
    {
        return ((mButtonBuffer[key >> 5] & N32_1OrMark[key % 32]) == 0) ? true : false;
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::press(Nui8 button)
    {
        mButtonBuffer[button >> 5] |= N32_1OrMark[button % 32];
        JoyPadControlButtonArgs args(button, true);
        mTarget->onButton(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::release(Nui8 button)
    {
        mButtonBuffer[button >> 5] &= N32_1NotMark[button % 32];
        JoyPadControlButtonArgs args(button, false);
        mTarget->onButton(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::slip(Nui8 slider, NIIi x, NIIi y)
    {
        mSliderBuffer[slider].x = x;
        mSliderBuffer[slider].y = y;
        JoyStickControlSliderArgs args(slider, mSliderBuffer[slider]);
        mTarget->onSlipSlider(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::slip(Nui8 slider)
    {
        JoyStickControlSliderArgs args(slider, mSliderBuffer[slider]);
        mTarget->onSlipSlider(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::push(Nui8 pov, PovType dir)
    {
        mPovBuffer[pov] = dir;
        JoyStickControlPovArgs args(pov, dir);
        mTarget->onPushPOV(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::push(Nui32 pov)
    {
        JoyStickControlPovArgs args(pov, mPovBuffer[pov]);
        mTarget->onPushPOV(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::setPovCount(NCount num)
    {
        mPovBuffer.resize(num);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::setSliderCount(NCount num)
    {
        mSliderBuffer.resize(num);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::twist(Nui8 axis, Ni32 data)
    {
        mAxisBuffer[axis] = data;
        JoyPadControlAxisArgs args(axis, data);
        mTarget->onTwistAxis(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::twist(Nui8 axis)
    {
        JoyPadControlAxisArgs args(axis, mAxisBuffer[axis]);
        mTarget->onTwistAxis(&args);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::setButtonCount(NCount num)
    {
        mButtonBuffer.resize((num + 31) >> 5);
        Buttons::iterator i, iend = mButtonBuffer.end();
        for(i = mButtonBuffer.begin(); i != iend; ++i)
            (*i) = 0;
        mButtonCount = num;
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::setAxisCount(NCount num)
    {
        mAxisBuffer.resize(num);
        Axes::iterator i, iend = mAxisBuffer.end();
        for(i = mAxisBuffer.begin(); i != iend; ++i)
            (*i) = 0;
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::apply(ControlItem * item)
    {
        item->setInfluence(this);
        mTarget = static_cast<JoyPadControlItem *>(item);
    }
    //------------------------------------------------------------------------
    void JoyPadControlPattern::detach()
    {
        mTarget->setInfluence(NULL);
        mTarget = N_Only(Input).getDummyJoyPad();
    }
    //------------------------------------------------------------------------
    ControlDevType JoyPadControlPattern::getType() const
    {
        return CDT_JoyPad;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JoyHandleControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyHandleControlPattern::JoyHandleControlPattern() {}
    //------------------------------------------------------------------------
    JoyHandleControlPattern::JoyHandleControlPattern(Nid id, const ViewWindow * own) :
        JoyPadControlPattern(id, own) {}
    //------------------------------------------------------------------------
    JoyHandleControlPattern::~JoyHandleControlPattern() {}
    //------------------------------------------------------------------------
    void JoyHandleControlPattern::apply(ControlItem * item)
    {
        item->setInfluence(this);
        mTarget = static_cast<JoyHandleControlItem *>(item);
    }
    //------------------------------------------------------------------------
    void JoyHandleControlPattern::detach()
    {
        mTarget->setInfluence(this);
        mTarget = N_Only(Input).getDummyJoyHandle();
    }
    //------------------------------------------------------------------------
    ControlDevType JoyHandleControlPattern::getType() const
    {
        return CDT_JoyHandle;
    }
    //------------------------------------------------------------------------
    void JoyHandleControlPattern::handle(NIIi v, NIIf x, NIIf y, NIIf z)
    {
        JoyHandleControlVectorArgs arg(v, x, y, z);
        static_cast<JoyHandleControlItem *>(mTarget)->onHandleVector(&arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JoyWheelControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyWheelControlPattern::JoyWheelControlPattern() {}
    //------------------------------------------------------------------------
    JoyWheelControlPattern::JoyWheelControlPattern(Nid id, const ViewWindow * own) :
        JoyPadControlPattern(id, own) {}
    //------------------------------------------------------------------------
    JoyWheelControlPattern::~JoyWheelControlPattern()
    {
    }
    //------------------------------------------------------------------------
    void JoyWheelControlPattern::wheel(NIIf num)
    {
        mWheel = num;
        JoyWheelControlArgs args(num);
        mTarget->onWheel(&args);
    }
    //------------------------------------------------------------------------
    void JoyWheelControlPattern::wheel()
    {
        JoyWheelControlArgs args(mWheel);
        mTarget->onWheel(&args);
    }
    //------------------------------------------------------------------------
    void JoyWheelControlPattern::apply(ControlItem * item)
    {
        item->setInfluence(this);
        mTarget = static_cast<JoyWheelControlItem *>(item);
    }
    //------------------------------------------------------------------------
    void JoyWheelControlPattern::detach()
    {
        mTarget->setInfluence(0);
        mTarget = N_Only(Input).getDummyJoyWheel();
    }
    //------------------------------------------------------------------------
    ControlDevType JoyWheelControlPattern::getType() const
    {
        return CDT_JoyWheel;
    }
    //------------------------------------------------------------------------
}
}