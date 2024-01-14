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
#include "NiiWin32JoyStickRampEffect.h"

namespace NII
{
namespace NII_MEDIA
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Win32JoyStickRampEffect
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Win32JoyStickRampEffect::apply(JoyDevForceFeedback * dest)
    {
        DIEFFECT dieff;
        DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG rglDirection[2] = { 0, 0 };
        DIENVELOPE diEnvelope;
        ZeroMemory(dieff, sizeof(DIEFFECT));

        DIRAMPFORCE rf;
        rf.lStart = mStartLevel;
        rf.lEnd = mEndLevel;

        dieff.dwSize = sizeof(DIEFFECT);
        dieff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        dieff.dwGain = DI_FFNOMINALMAX;
        dieff.dwTriggerButton = DIEB_NOTRIGGER; // effect->trigger_button; // TODO: Conversion
        dieff.dwTriggerRepeatInterval = mTriggerInterval;
        dieff.cAxes = 1; // effect->getNumAxes();
        dieff.rgdwAxes = rgdwAxes;
        dieff.rglDirection = rglDirection; // TODO: conversion from effect->direction

        if (mEnvelope->mAttackLevel | mEnvelope->mAattackLength |
            mEnvelope->mFadeLevel | mEnvelope->mFadeLength)
        {
            diEnvelope.dwSize = sizeof(DIENVELOPE);
            diEnvelope.dwAttackLevel = mEnvelope->mAttackLevel;
            diEnvelope.dwAttackTime = mEnvelope->mAttackLength;
            diEnvelope.dwFadeLevel = mEnvelope->mFadeLevel;
            diEnvelope.dwFadeTime = mEnvelope->mFadeLength;
            dieff.lpEnvelope = &diEnvelope;
        }
        else
            dieff->lpEnvelope = 0;

        dieff.dwSamplePeriod = 0;
        dieff.dwDuration = mReplayLength;
        dieff.dwStartDelay = mReplayDelay;

        dieff.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
        dieff.lpvTypeSpecificParams = &rf;

        Win32JoyDevForceFeedback * temp = static_cast<Win32JoyDevForceFeedback *>(dest);
        temp->upload(GUID_RampForce, &dieff, this);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Win32JoyStickPeriodicEffect
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Win32JoyStickPeriodicEffect::apply(JoyDevForceFeedback * dest)
    {
        DIEFFECT dieff;
        DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG rglDirection[2] = { 0, 0 };
        DIENVELOPE diEnvelope;
        ZeroMemory(dieff, sizeof(DIEFFECT));

        DIPERIODIC pf;
        pf.dwMagnitude = mMagnitude;
        pf.lOffset = mOffset;
        pf.dwPhase = mPhase;
        pf.dwPeriod = mPeriod;

        dieff.dwSize = sizeof(DIEFFECT);
        dieff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        dieff.dwGain = DI_FFNOMINALMAX;
        dieff.dwTriggerButton = DIEB_NOTRIGGER; // effect->trigger_button; // TODO: Conversion
        dieff.dwTriggerRepeatInterval = mTriggerInterval;
        dieff.cAxes = 1; // effect->getNumAxes();
        dieff.rgdwAxes = rgdwAxes;
        dieff.rglDirection = rglDirection; // TODO: conversion from effect->direction

        if (mEnvelope->mAttackLevel | mEnvelope->mAttackLength |
            mEnvelope->mFadeLevel | mEnvelope->mFadeLength)
        {
            diEnvelope.dwSize = sizeof(DIENVELOPE);
            diEnvelope.dwAttackLevel = mEnvelope->mAttackLevel;
            diEnvelope.dwAttackTime = mEnvelope->mAttackLength;
            diEnvelope.dwFadeLevel = mEnvelope->mFadeLevel;
            diEnvelope.dwFadeTime = mEnvelope->mFadeLength;
            dieff.lpEnvelope = &diEnvelope;
        }
        else
            dieff->lpEnvelope = 0;

        dieff.dwSamplePeriod = 0;
        dieff.dwDuration = mReplayLength;
        dieff.dwStartDelay = mReplayDelay;

        dieff.cbTypeSpecificParams = sizeof(DIPERIODIC);
        dieff.lpvTypeSpecificParams = &pf;

        Win32JoyDevForceFeedback * temp = static_cast<Win32JoyDevForceFeedback *>(dest);

        switch (mFactorType)
        {
        case JoyStickEffect::FEFT_Square:
            temp->upload(GUID_Square, &dieff, this);
            break;
        case JoyStickEffect::FEFT_Triangle:
            temp->upload(GUID_Triangle, &dieff, this);
            break;
        case JoyStickEffect::FEFT_Sine:
            temp->upload(GUID_Sine, &dieff, this);
            break;
        case JoyStickEffect::FEFT_SawToothUp:
            temp->upload(GUID_SawtoothUp, &dieff, this);
            break;
        case JoyStickEffect::FEFT_SawToothDown:
            temp->upload(GUID_SawtoothDown, &dieff, this);
            break;
        default:
            N_EXCEPT(UnImpl, _T("No such available effect for Periodic force !"));
            break;
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Win32JoyStickConstantEffect
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Win32JoyStickConstantEffect::apply(JoyDevForceFeedback * dest)
    {
        DIEFFECT dieff;
        DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG rglDirection[2] = { 0, 0 };
        DIENVELOPE diEnvelope;

        DICONSTANTFORCE cf;
        cf.lMagnitude = mLevel;

        ZeroMemory(dieff, sizeof(DIEFFECT));

        dieff.dwSize = sizeof(DIEFFECT);
        dieff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        dieff.dwGain = DI_FFNOMINALMAX;
        dieff.dwTriggerButton = DIEB_NOTRIGGER; // effect->trigger_button; // TODO: Conversion
        dieff.dwTriggerRepeatInterval = mTtriggerInterval;
        dieff.cAxes = 1; // effect->getNumAxes();
        dieff.rgdwAxes = rgdwAxes;
        dieff.rglDirection = rglDirection; // TODO: conversion from effect->direction

        if (mEnvelope->mAttackLevel | mEnvelope->mAttackLength |
            mEnvelope->mFadeLevel | mEnvelope->mFadeLength)
        {
            diEnvelope.dwSize = sizeof(DIENVELOPE);
            diEnvelope.dwAttackLevel = mEnvelope->mAttackLevel;
            diEnvelope.dwAttackTime = mEnvelope->mAttackLength;
            diEnvelope.dwFadeLevel = mEnvelope->mFadeLevel;
            diEnvelope.dwFadeTime = mEnvelope->mFadeLength;
            dieff.lpEnvelope = &diEnvelope;
        }
        else
            dieff->lpEnvelope = 0;

        dieff.dwSamplePeriod = 0;
        dieff.dwDuration = mReplayLength;
        dieff.dwStartDelay = mReplayDelay;

        dieff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        dieff.lpvTypeSpecificParams = &cf;

        Win32JoyDevForceFeedback * temp = static_cast<Win32JoyDevForceFeedback *>(dest);
        temp->upload(GUID_ConstantForce, &dieff, this);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Win32JoyStickConditionalEffect
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Win32JoyStickConditionalEffect::apply(JoyDevForceFeedback * dest)
    {
        DIEFFECT dieff;
        DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG rglDirection[2] = { 0, 0 };
        DIENVELOPE diEnvelope;
        ZeroMemory(dieff, sizeof(DIEFFECT));

        DICONDITION cf;
        cf.lOffset = mDeadband;
        cf.lPositiveCoefficient = mRightCoeff;
        cf.lNegativeCoefficient = mLeftCoeff;
        cf.dwPositiveSaturation = mRightSaturation;
        cf.dwNegativeSaturation = mLeftSaturation;
        cf.lDeadBand = mDeadband;

        dieff.dwSize = sizeof(DIEFFECT);
        dieff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        dieff.dwGain = DI_FFNOMINALMAX;
        dieff.dwTriggerButton = DIEB_NOTRIGGER; // effect->trigger_button; // TODO: Conversion
        dieff.dwTriggerRepeatInterval = mTriggerInterval;
        dieff.cAxes = 1; // effect->getNumAxes();
        dieff.rgdwAxes = rgdwAxes;
        dieff.rglDirection = rglDirection; // TODO: conversion from effect->direction
        dieff->lpEnvelope = 0;

        dieff.dwSamplePeriod = 0;
        dieff.dwDuration = mReplayLength;
        dieff.dwStartDelay = mReplayDelay;

        dieff.cbTypeSpecificParams = sizeof(DICONDITION);
        dieff.lpvTypeSpecificParams = &cf;

        Win32JoyDevForceFeedback * temp = static_cast<Win32JoyDevForceFeedback *>(dest);

        switch (mFactorType)
        {
        case JoyStickEffect::FEFT_Friction:
            temp->upload(GUID_Friction, &dieff, this);
            break;
        case JoyStickEffect::FEFT_Damper:
            temp->upload(GUID_Damper, &dieff, this);
            break;
        case JoyStickEffect::FEFT_Inertia:
            temp->upload(GUID_Inertia, &dieff, this);
            break;
        case JoyStickEffect::FEFT_Spring:
            temp->upload(GUID_Spring, &dieff, this);
            break;
        default:
            N_EXCEPT(UnImpl, _T("No such available effect for Conditional force !"));
            break;
        }
    }
    //-----------------------------------------------------------------------
}
}