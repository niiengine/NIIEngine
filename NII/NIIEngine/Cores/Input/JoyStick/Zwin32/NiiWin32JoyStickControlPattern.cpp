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
#include "NiiWin32JoyStickControlPattern.h"

#include <cassert>

#undef DIJOFS_BUTTON
#undef DIJOFS_POV

#define DIJOFS_BUTTON(n) (FIELD_OFFSET(DIJOYSTATE2, rgbButtons) + (n))
#define DIJOFS_POV(n)     (FIELD_OFFSET(DIJOYSTATE2, rgdwPOV)+(n)*sizeof(DWORD))
#define DIJOFS_SLIDER0(n) (FIELD_OFFSET(DIJOYSTATE2, rglSlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER1(n) (FIELD_OFFSET(DIJOYSTATE2, rglVSlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER2(n) (FIELD_OFFSET(DIJOYSTATE2, rglASlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER3(n) (FIELD_OFFSET(DIJOYSTATE2, rglFSlider)+(n) * sizeof(LONG))

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    Win32JoyStickControlPattern::Win32JoyStickControlPattern(Nid id,
        IDirectInput8 * pdi, GUID guid) :
            JoyStickControlPattern(id),
            mDirectInput(pdi),
            mFfeedback(0),
            mJoyStick(0),
            mGuidInstance(guid)
    {
    }
    //------------------------------------------------------------------------
    Win32JoyStickControlPattern::~Win32JoyStickControlPattern()
    {
        if(mFfeedback)
            N_delete mFfeedback;

        if(mJoyStick)
        {
            mJoyStick->Unacquire();
            mJoyStick->Release();
            mJoyStick = 0;
        }

        // 回收这个对象
    }
    //------------------------------------------------------------------------
    NIIb Win32JoyStickControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb Win32JoyStickControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::_init()
    {
        if(mFfeedback)
        {
            N_delete mFfeedback;
            mFfeedback = 0;
        }

        DIPROPDWORD dipdw;

        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = JOYSTICK_DX_BUFFERSIZE;

        if(FAILED(mDirectInput->CreateDevice(mGuidInstance, &mJoyStick, NULL)))
            N_EXCEPT(CreateDevice, _T("Could not initialize joy device !"));

        if(FAILED(mJoyStick->SetDataFormat(&c_dfDIJoystick2)))
            N_EXCEPT(CreateDevice, _T("Set data format error !"));

        if( FAILED(mJoyStick->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)) )
            N_EXCEPT(CreateDevice, _T("Failed to set buffer size property !"));

        //Enumerate all axes/buttons/sliders/etc before aquiring
        enumCom();
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::_update()
    {
        if(mActive)
        {
            DIDEVICEOBJECTDATA diff[JOYSTICK_DX_BUFFERSIZE];
            DWORD count = JOYSTICK_DX_BUFFERSIZE;

            // Poll the device to read the current state
            HRESULT hr = mJoyStick->Poll();
            if(hr == DI_OK)
                hr = mJoyStick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diff, &count, 0);

            if(hr != DI_OK)
            {
                hr = mJoyStick->Acquire();
                while(hr == DIERR_INPUTLOST)
                    hr = mJoyStick->Acquire();

                // Poll the device to read the current state
                mJoyStick->Poll();
                hr = mJoyStick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diff, &count, 0);
                //Perhaps the user just tabbed away
                if(FAILED(hr))
                    return;
            }

            NCount i;
            vector<bool>::type ax(mAxisBuffer.size(), false);
            vector<bool>::type sx(mSliderBuffer.size(), false);

            for(i = 0; i < count; ++i)
            {
                //First check to see if event entry is a Axis we enumerated earlier
                if((NIIs)(diff[i].uAppData >> 16) == 0x1313)
                {
                    Nui8 axis = (Nui8)(0x0000FFFF & diff[i].uAppData); //Mask out the high bit
                    mAxisBuffer[axis] = diff[i].dwData;
                    ax[axis] = true;
                }
                else if(diff[i].dwOfs >= DIJOFS_BUTTON(0) && diff[i].dwOfs < DIJOFS_BUTTON(128))
                {
                    Nui8 button = diff[i].dwOfs - DIJOFS_BUTTON(0);
                    if(diff[i].dwData & 0x80)
                        press(button);
                    else
                        release(button);
                }
                else
                {
                    switch(diff[i].dwOfs)
                    {
                    case DIJOFS_SLIDER0(0):
                        sx[0] = true;
                        mSliderBuffer[0].x = diff[i].dwData;
                        break;
                    case DIJOFS_SLIDER0(1):
                        sx[0] = true;
                        mSliderBuffer[0].y = diff[i].dwData;
                        break;
                    case DIJOFS_POV(0):
                        push(0, diff[i]);
                        break;
                    case DIJOFS_POV(1):
                        push(1, diff[i]);
                        break;
                    case DIJOFS_POV(2):
                        push(2, diff[i]);
                        break;
                    case DIJOFS_POV(3):
                        push(3, diff[i]);
                        break;
                    case DIJOFS_SLIDER1(0):
                        sx[1] = true;
                        mSliderBuffer[1].x = diff[i].dwData;
                        break;
                    case DIJOFS_SLIDER1(1):
                        sx[1] = true;
                        mSliderBuffer[1].y  = diff[i].dwData;
                        break;
                    case DIJOFS_SLIDER2(0):
                        sx[2] = true;
                        mSliderBuffer[2].x  = diff[i].dwData;
                        break;
                    case DIJOFS_SLIDER2(1):
                        sx[2] = true;
                        mSliderBuffer[2].y = diff[i].dwData;
                        break;
                    case DIJOFS_SLIDER3(0):
                        sx[3] = true;
                        mSliderBuffer[3].x = diff[i].dwData;
                        break;
                    case DIJOFS_SLIDER3(1):
                        sx[3] = true;
                        mSliderBuffer[3].y = diff[i].dwData;
                        break;
                    }
                }
            }

            vector<bool>::type::iterator it, itend = ax.end();

            for(i = 0, it = ax.begin(); it != itend; ++i, ++it)
            {
                if(*it)
                    twist(i);
            }
            itend = sx.end();
            for(i = 0, it = sx.begin(); it != itend; ++i, ++it)
            {
                if(*it)
                    slip(i);
            }
        }
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::_reset()
    {
        HWND hwin;
        DWORD coop;
        if(mOwn->getPlatformData("WINDOW", &hwin))
            if(mOwn->getPlatformData("JD", &coop))
                mJoyStick->SetCooperativeLevel(hwin, coop);
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID Win32JoyStickControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::enumCom()
    {
        //We can check force feedback here too
        mCaps.dwSize = sizeof(DIDEVCAPS);
        mJoyStick->GetCapabilities(&mCaps);

        setButtonCount(mCaps.dwButtons);

        setAxisCount(mCaps.dwAxes);

        setPovCount(mCaps.dwPOVs);

        setSliderCount(0);
        //Reset the axis mapping enumeration value
        mCurrentAxis = 0;

        //Enumerate Force Feedback (if any)
        mJoyStick->EnumEffects(enumEff, this, DIEFT_ALL);

        //Enumerate and set axis constraints (and check FF Axes)
        mJoyStick->EnumObjects(enumDev, this, DIDFT_AXIS);
    }
    //------------------------------------------------------------------------
    void Win32JoyStickControlPattern::push(Nui8 pov, DIDEVICEOBJECTDATA & di)
    {
        if(LOWORD(di[i].dwData) == 0xFFFF)
        {
            push(pov, PT_Center);
        }
        else
        {
            switch(di[i].dwData)
            {
            case 0:
                push(pov, PT_North);
                break;
            case 4500:
                push(pov, PT_NorthEast);
                break;
            case 9000:
                push(pov, PT_East);
                break;
            case 13500:
                push(pov, PT_SouthEast);
                break;
            case 18000:
                push(pov, PT_South);
                break;
            case 22500:
                push(pov, PT_SouthWest);
                break;
            case 27000:
                push(pov, PT_West);
                break;
            case 31500:
                push(pov, PT_NorthWest);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    BOOL CALLBACK Win32JoyStickControlPattern::enumDev(
        LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID p)
    {
        Win32JoyStickControlPattern * temp = (Win32JoyStickControlPattern *)p;

        //Setup mappings
        DIPROPPOINTER diptr;
        diptr.diph.dwSize = sizeof(DIPROPPOINTER);
        diptr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diptr.diph.dwHow = DIPH_BYID;
        diptr.diph.dwObj = lpddoi->dwType;
        //Add the high bit in so that an axis value of zero does not mean a null userdata
        diptr.uData = 0x13130000 | temp->mCurrentAxis;
        NCount slider = getSliderCount();
        NCount axis = getAxisCount();
        // dinput中把slider当成axis
        if(GUID_Slider == lpddoi->guidType)
        {
            ++slider;
            // 移去末尾的
            --axis;
        }
        else if (FAILED(temp->mJoyStick->SetProperty(DIPROP_APPDATA, &diptr.diph)))
        {	//If for some reason we could not set needed user data, just ignore this axis
            return DIENUM_CONTINUE;
        }
        setSliderCount(slider);
        setAxisCount(axis);
        //Increase for next time through
        temp->mCurrentAxis += 1;

        //Set range
        DIPROPRANGE diprg;
        diprg.diph.dwSize = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow = DIPH_BYID;
        diprg.diph.dwObj = lpddoi->dwType;
        diprg.lMin = MIN_AXIS;
        diprg.lMax = MAX_AXIS;

        if (FAILED(temp->mJoyStick->SetProperty(DIPROP_RANGE, &diprg.diph)))
            N_EXCEPT(DeviceInner, _T("Failed to set min/max range property !"));

        //Check if FF Axes, and if so, increment counter
        if((lpddoi->dwFlags & DIDOI_FFACTUATOR) != 0)
        {
            if(temp->mFfeedback)
            {
                temp->mFfDevice->_addFFAxis();
            }
        }

        //Force the flags for gain and auto-center support to true,
        //as DInput has no API to query the device for these capabilities
        //(the only way to know is to try them ...)
        if(temp->mFfeedback)
        {
            temp->mFfeedback->_setGainSupport(true);
            temp->mFfeedback->_setAutoCenterSupport(true);
        }

        return DIENUM_CONTINUE;
    }
    //------------------------------------------------------------------------
    BOOL CALLBACK Win32JoyStickControlPattern::enumEff(
        LPCDIEFFECTINFO pdei, LPVOID p)
    {
        Win32JoyStickControlPattern * temp = (Win32JoyStickControlPattern *)p;

        //Create the FF class after we know there is at least one effect type
        if(temp->mFfeedback == 0)
            temp->mFfeedback = N_new Win32JoyDevForceFeedback(temp->mJoyStick, &temp->mCaps);

        temp->mFfeedback->_addEffectSupport(pdei);

        return DIENUM_CONTINUE;
    }
    //------------------------------------------------------------------------
}
}
