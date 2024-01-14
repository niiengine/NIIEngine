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
#include "NiiWin32JoyDevForceFeedback.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    Win32JoyDevForceFeedback::Win32JoyDevForceFeedback(IDirectInputDevice8 * ipid,
        const DIDEVCAPS * dcap) :
            mJoyStick(ipid),
            mCaps(dcap),
            mCurrentNum(0),
            mFFAxes(0)
    {
    }
    //------------------------------------------------------------------------
    Win32JoyDevForceFeedback::~Win32JoyDevForceFeedback()
    {
        Effects::iterator i, iend = mEffects.end()
        for(i = mEffects.begin(); i != iend; ++i)
        {
            LPDIRECTINPUTEFFECT dxEffect = i->second;
            if(dxEffect)
            {
                dxEffect->Unload();
                dxEffect->Release();
            }
        }

        mEffects.clear();
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::_addEffectSupport(LPCDIEFFECTINFO pdei)
    {
        JoyStickEffect::ForceEffectType eForce;
        switch (DIEFT_GETTYPE(pdei->dwEffType))
        {
            case DIEFT_CONSTANTFORCE:
                eForce = JoyStickEffect::EFT_Constant;
                break;
            case DIEFT_RAMPFORCE:
                eForce = JoyStickEffect::EFT_Ramp;
                break;
            case DIEFT_PERIODIC:
                eForce = JoyStickEffect::EFT_Periodic;
                break;
            case DIEFT_CONDITION:
                eForce = JoyStickEffect::EFT_Conditional;
                break;
            case DIEFT_CUSTOMFORCE:
                eForce = JoyStickEffect::EFT_Custom;
                break;
            default:
                eForce = JoyStickEffect::EFT_Unknown;
                return;
        }

        //Determine what the effect type is and how it corresponds to our OIS's Enums
        //We could save the GUIDs too, however, we will just use the predefined ones later
        if(pdei->guid == GUID_ConstantForce)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Constant);
        else if(pdei->guid == GUID_Triangle)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Triangle);
        else if(pdei->guid == GUID_Spring)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Spring);
        else if(pdei->guid == GUID_Friction)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Friction);
        else if(pdei->guid == GUID_Square)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Square);
        else if(pdei->guid == GUID_Sine)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Sine);
        else if(pdei->guid == GUID_SawtoothUp)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_SawToothUp);
        else if(pdei->guid == GUID_SawtoothDown)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_SawToothDown);
        else if(pdei->guid == GUID_Damper)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Damper);
        else if(pdei->guid == GUID_Inertia)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Inertia);
        else if(pdei->guid == GUID_CustomForce)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Custom);
        else if(pdei->guid == GUID_RampForce)
            _addEffectTypes(eForce, JoyStickEffect::FEFT_Ramp);
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::_addFFAxis()
    {
        ++mFFAxes;
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::setMasterGain(NIIf level)
    {
        //Between 0 - 10,000
        NIIi gain_level = (NIIi)(10000.0f * level);

        if(gain_level > 10000)
            gain_level = 10000;
        else if(gain_level < 0)
            gain_level = 0;

        DIPROPDWORD DIPropGain;
        DIPropGain.diph.dwSize = sizeof(DIPropGain);
        DIPropGain.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        DIPropGain.diph.dwObj = 0;
        DIPropGain.diph.dwHow = DIPH_DEVICE;
        DIPropGain.dwData = gain_level;

        mJoyStick->SetProperty(DIPROP_FFGAIN, &DIPropGain.diph);
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::setAutoCenterMode(bool set)
    {
        DIPROPDWORD DIPropAutoCenter;
        DIPropAutoCenter.diph.dwSize = sizeof(DIPropAutoCenter);
        DIPropAutoCenter.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        DIPropAutoCenter.diph.dwObj = 0;
        DIPropAutoCenter.diph.dwHow = DIPH_DEVICE;
        DIPropAutoCenter.dwData = (set ? DIPROPAUTOCENTER_ON : DIPROPAUTOCENTER_OFF);

        mJoyStick->SetProperty(DIPROP_AUTOCENTER, &DIPropAutoCenter.diph);
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::upload(const JoyStickEffect * eff)
    {
        eff->apply(this);
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::modify(const JoyStickEffect * eff)
    {
        eff->apply(this);
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::remove(const JoyStickEffect * eff)
    {
        //Get the effect - if it exists
        Effects::iterator i = mEffects.find(eff->mHandle);
        if(i != mEffects.end())
        {
            LPDIRECTINPUTEFFECT dxEffect = i->second;
            if(dxEffect)
            {
                dxEffect->Stop();
                //We care about the return value - as the effect might not
                //have been unlaoded
                if(SUCCEEDED(dxEffect->Unload()))
                {
                    dxEffect->Release();
                    mEffects.erase(i);
                }
            }
            else
                mEffects.erase(i);
        }
    }
    //------------------------------------------------------------------------
    short Win32JoyDevForceFeedback::getFFAxesNumber()
    {
        return mFFAxes;
    }
    //------------------------------------------------------------------------
    unsigned short Win32JoyDevForceFeedback::getFFMemoryLoad()
    {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0; // device property
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = 0; // In case of any error.

        const HRESULT hr = mJoyStick->GetProperty(DIPROP_FFLOAD, &dipdw.diph);
        if(FAILED(hr))
        {
            if (hr == DIERR_NOTEXCLUSIVEACQUIRED)
                N_EXCEPT(DeviceInner, _T("Can't query FF memory load as device was not acquired in exclusive mode !"));
            else
                N_EXCEPT(DeviceInner, _T("Unknown error querying FF memory load !"));
        }

        return (Nui16)dipdw.dwData;
    }
    //------------------------------------------------------------------------
    void Win32JoyDevForceFeedback::upload(GUID guid, DIEFFECT * dieff,
        const JoyStickEffect * eff)
    {
        LPDIRECTINPUTEFFECT dxeff = 0;

        //Get the effect - if it exists
        Effects::iterator i = mEffects.find(eff->mHandle);
        //It has been created already
        if(i != mEffects.end())
            dxeff = i->second;
        else //This effect has not yet been created - generate a handle
            eff->mHandle = mCurrentNum++;

        if(dxeff == 0)
        {
            //This effect has not yet been created, so create it
            HRESULT hr = mJoyStick->CreateEffect(guid, dieff, &dxeff, NULL);
            if(SUCCEEDED(hr))
            {
                mEffects[eff->mHandle] = dxeff;
                dxeff->Start(INFINITE,0);
            }
            else if( hr == DIERR_DEVICEFULL )
                N_EXCEPT(DeviceFull, _T("Remove an effect before adding more !"));
            else
                N_EXCEPT(CreateDevice, _T("Unknown error creating effect !"));
        }
        else
        {
            //ToDo -- Update the Effect
            HRESULT hr = dxeff->SetParameters(dieff, DIEP_DIRECTION |
                DIEP_DURATION | DIEP_ENVELOPE | DIEP_STARTDELAY | DIEP_TRIGGERBUTTON |
                DIEP_TRIGGERREPEATINTERVAL | DIEP_TYPESPECIFICPARAMS | DIEP_START);

            if(FAILED(hr))
                N_EXCEPT(InvalidParam, _T("Error updating device !"));
        }
    }
    //------------------------------------------------------------------------
}
}