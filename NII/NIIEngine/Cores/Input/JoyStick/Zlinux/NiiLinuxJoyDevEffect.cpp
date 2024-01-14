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
#include "NiiLinuxJoyStickRampEffect.h"

// To Signed16/Unsigned15 safe conversions
#define MaxU15Bit 0x7FFF
#define toU15(x) (__u16)((x) < 0 ? 0 : ((x) > Max15Bit ? Max15Bit : (x)))
#define MaxS16Bit  0x7FFF
#define MinS16Bit -0x7FFF
#define toS16(x) (__s16)((x) < MinS16Bit ? MinS16Bit : ((x) > MaxS16Bit ? MaxS16Bit : (x)))

// OIS to Linux duration
#define LinuxInfiniteDuration 0xFFFF
#define OISDurationUnitMS 1000 // OIS duration unit (microseconds), expressed in milliseconds (theLinux duration unit)

// linux/input.h : All duration values are expressed in ms. Values above 32767 ms (0x7fff)
//                 should not be used and have unspecified results.
#define LinuxDuration(dur) ((dur) == 0xFFFFFFFF ? LinuxInfiniteDuration : toU15((dur)/OISDurationUnitMS))

#define NIIMaxLevel 10000
#define LinuxMaxLevel 0x7FFF

// linux/input.h : Valid range for the attack and fade levels is 0x0000 - 0x7fff
#define LinuxPositiveLevel(level) toU15(LinuxMaxLevel*(long)(level)/NIIMaxLevel)

#define LinuxSignedLevel(level) toS16(LinuxMaxLevel*(long)(level)/NIIMaxLevel)

namespace NII
{
namespace NII_MEIDA
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LinuxJoyStickRampEffect
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void LinuxJoyStickRampEffect::apply(JoyDevForceFeedback * dest)
    {
        struct ff_effect event;
        memset(event, 0, sizeof(struct ff_effect));

        JoyStickEffect::Envelope * envelope = &temp->mEnvelope;
        struct ff_envelope * ffenvelope = &event.u.constant.envelope;
        //------------------------------------------------------------
        if (envelope->mAttackLength | envelope->mAttackLevel |
            envelope->mFadeLength | envelope->mFadeLevel)
        {
            ffenvelope->attack_length = LinuxDuration(envelope->mAttackLength);
            ffenvelope->attack_level = LinuxPositiveLevel(envelope->mAttackLevel);
            ffenvelope->fade_length = LinuxDuration(envelope->mFadeLength);
            ffenvelope->fade_level = LinuxPositiveLevel(envelope->mFadeLevel);
        }

        event.direction = (__u16)(1 + (mDirection * 45.0 + 135.0) * 0xFFFFUL / 360.0);

        // TODO trigger_button 0 vs. -1
        event.trigger.button = mTriggerButton; // < 0 ? 0 : effect->trigger_button;
        event.trigger.interval = LinuxDuration(mTriggerInterval);
        event.replay.length = LinuxDuration(mReplayLength);
        event.replay.delay = LinuxDuration(mReplayDelay);
        event.type = FF_RAMP;
        event.id = -1;
        event.u.ramp.start_level = LinuxSignedLevel(mStartLevel);
        event.u.ramp.end_level = LinuxSignedLevel(mEndLevel);

        LinuxJoyDevForceFeedback * temp = static_cast<LinuxJoyDevForceFeedback *>(dest);
        temp->upload(&event, this);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LinuxJoyStickPeriodicEffect
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void LinuxJoyStickPeriodicEffect::apply(JoyDevForceFeedback * dest)
    {
        struct ff_effect event;
        memset(event, 0, sizeof(struct ff_effect));

        struct ff_envelope * ffenvelope = &event.u.constant.envelope;

        if (mEnvelope->mAttackLength | mEnvelope->mAttackLevel |
            mEnvelope->mFadeLength | mEnvelope->mFadeLevel)
        {
            ffenvelope->attack_length = LinuxDuration(mEnvelope->mAttackLength);
            ffenvelope->attack_level = LinuxPositiveLevel(mEnvelope->mAttackLevel);
            ffenvelope->fade_length = LinuxDuration(mEnvelope->mFadeLength);
            ffenvelope->fade_level = LinuxPositiveLevel(mEnvelope->mFadeLevel);
        }

        event.direction = (__u16)(1 + (mDirection * 45.0 + 135.0) * 0xFFFFUL / 360.0);

        // TODO trigger_button 0 vs. -1
        event.trigger.button = mTriggerButton; // < 0 ? 0 : effect->trigger_button;
        event.trigger.interval = LinuxDuration(mTriggerInterval);
        event.replay.length = LinuxDuration(mReplayLength);
        event.replay.delay = LinuxDuration(mReplayDelay);
        event.type = FF_PERIODIC;
        event.id = -1;

        switch (mFactorType)
        {
        case JoyStickEffect::FEFT_Square:
            event.u.periodic.waveform = FF_SQUARE;
            break;
        case JoyStickEffect::FEFT_Triangle:
            event.u.periodic.waveform = FF_TRIANGLE;
            break;
        case JoyStickEffect::FEFT_Sine:
            event.u.periodic.waveform = FF_SINE;
            break;
        case JoyStickEffect::FEFT_SawToothUp:
            event.u.periodic.waveform = FF_SAW_UP;
            break;
        case JoyStickEffect::FEFT_SawToothDown:
            event.u.periodic.waveform = FF_SAW_DOWN;
            break;
        default:
            N_EXCEPT(UnImpl, _T("No such available effect for Periodic force !"));
            break;
        }

        event.u.periodic.period = LinuxDuration(mPeriod);
        event.u.periodic.magnitude = LinuxPositiveLevel(mMagnitude);
        event.u.periodic.offset = LinuxPositiveLevel(mOffset);
        event.u.periodic.phase = (__u16)(mPhase * event.u.periodic.period / 36000.0); // ?????

        // Note: No support for Custom periodic force effect for the moment
        event.u.periodic.custom_len = 0;
        event.u.periodic.custom_data = 0;

        LinuxJoyDevForceFeedback * temp = static_cast<LinuxJoyDevForceFeedback *>(dest);
        temp->upload(&event, this);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //  LinuxJoyStickConstantEffectapply
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void LinuxJoyStickConstantEffectapply::apply(JoyDevForceFeedback * dest)
    {
        struct ff_effect event;
        memset(event, 0, sizeof(struct ff_effect));
        struct ff_envelope * ffenvelope = &event.u.constant.envelope;

        if (mEnvelope->mAttackLength | mEnvelope->mAttackLevel |
            mEnvelope->mFadeLength | mEnvelope->mFadeLevel)
        {
            ffenvelope->attack_length = LinuxDuration(mEnvelope->mAttackLength);
            ffenvelope->attack_level = LinuxPositiveLevel(mEnvelope->mAttackLevel);
            ffenvelope->fade_length = LinuxDuration(mEnvelope->mFadeLength);
            ffenvelope->fade_level = LinuxPositiveLevel(mEnvelope->mFadeLevel);
        }

        event.direction = (__u16)(1 + (mDirection * 45.0 + 135.0) * 0xFFFFUL / 360.0);

        // TODO trigger_button 0 vs. -1
        event.trigger.button = mTriggerButton; // < 0 ? 0 : effect->trigger_button;
        event.trigger.interval = LinuxDuration(mTriggerInterval);
        event.replay.length = LinuxDuration(mReplayLength);
        event.replay.delay = LinuxDuration(mReplayDelay);
        event.type = FF_CONSTANT;
        event.id = -1;
        event.u.constant.level = LinuxSignedLevel(mLevel);

        LinuxJoyDevForceFeedback * temp = static_cast<LinuxJoyDevForceFeedback *>(dest);
        temp->upload(&event, this);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LinuxJoyStickConditionalEffect
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    LinuxJoyStickConditionalEffect::LinuxJoyStickConditionalEffect() :
        mCenter(0)
    {
    }
    //------------------------------------------------------------------------
    void LinuxJoyStickConditionalEffect::apply(JoyDevForceFeedback * dest)
    {
        struct ff_effect event;
        memset(event, 0, sizeof(struct ff_effect));

        event.direction = (__u16)(1 + (eff->mDirection * 45.0 + 135.0) * 0xFFFFUL / 360.0);

        // TODO trigger_button 0 vs. -1
        event.trigger.button = mTriggerButton; // < 0 ? 0 : effect->trigger_button;
        event.trigger.interval = LinuxDuration(mTriggerInterval);
        event.replay.length = LinuxDuration(mReplayLength);
        event.replay.delay = LinuxDuration(mReplayDelay);

        switch (mFactorType)
        {
        case JoyStickEffect::FEFT_Friction:
            event.type = FF_FRICTION;
            break;
        case JoyStickEffect::FEFT_Damper:
            event.type = FF_DAMPER;
            break;
        case JoyStickEffect::FEFT_Inertia:
            event.type = FF_INERTIA;
            break;
        case JoyStickEffect::FEFT_Spring:
            event.type = FF_SPRING;
            break;
        default:
            N_EXCEPT(UnImpl, _T("No such available effect for Conditional force !"));
            break;
        }

        event.id = -1;

        event.u.condition[0].right_saturation = LinuxSignedLevel(mRightSaturation);
        event.u.condition[0].left_saturation = LinuxSignedLevel(mLeftSaturation);
        event.u.condition[0].right_coeff = LinuxSignedLevel(mRightCoeff);
        event.u.condition[0].left_coeff = LinuxSignedLevel(mLeftCoeff);
        event.u.condition[0].deadband = LinuxPositiveLevel(mDeadband);// Unit ??
        event.u.condition[0].center = LinuxSignedLevel(mCenter); // Unit ?? TODO ?

        // TODO support for second condition
        event.u.condition[1] = event.u.condition[0];

        LinuxJoyDevForceFeedback * temp = static_cast<LinuxJoyDevForceFeedback *>(dest);

        temp->upload(&event, this);
    }
    //------------------------------------------------------------------------
}
}