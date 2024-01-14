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
#include "NiiWin32JoyPadControlPattern.h"

#include <cassert>

#undef DIJOFS_BUTTON
#define DIJOFS_BUTTON(n) (FIELD_OFFSET(DIJOYSTATE2, rgbButtons) + (n))

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    Win32JoyPadControlPattern::Win32JoyPadControlPattern(Nid id, IDirectInput8 * pdi,
        GUID guid) :
            JoyPadControlPattern(id),
            mDirectInput(pdi),
            mFfeedback(0),
            mJoyStick(0),
            mGuidInstance(guid)
    {
    }
    //------------------------------------------------------------------------
    Win32JoyPadControlPattern::~Win32JoyPadControlPattern()
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
    NIIb Win32JoyPadControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb Win32JoyPadControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void Win32JoyPadControlPattern::_init()
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
    void Win32JoyPadControlPattern::_update()
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

            for(i = 0; i < count; ++i)
            {
                //First check to see if event entry is a Axis we enumerated earlier
                if((NIIs)(diff[i].uAppData >> 16) == 0x1313)
                {
                    Nui8 axis = (Nui8)(0x0000FFFF & diff[i].uAppData); //Mask out the high bit
                    mAxisBuffer[axis] = diff[i].dwData;
                    ax[axis] = true;
                }
                else
                {
                    NIIi temp = DIJOFS_BUTTON(0);
                    //Handle Button Events Easily using the DX Offset Macros
                    if(diff[i].dwOfs >= temp && diff[i].dwOfs < DIJOFS_BUTTON(128))
                    {
                        Nui8 button = diff[i].dwOfs - temp;
                        if(diff[i].dwData & 0x80)
                            press(button);
                        else
                            release(button);
                    }
                }
            }

            vector<bool>::type::iterator it, itend = ax.end();
            for(i = 0, it = ax.begin(); it != itend; ++i, ++it)
            {
                if(*itend)
                {
                    twist(i);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Win32JoyPadControlPattern::_reset()
    {
        HWND hwin;
        DWORD Coop;
        if(mOwn->getPlatformData("WINDOW", &hwin))
            if(mOwn->getPlatformData("JD", &Coop))
                mJoyStick->SetCooperativeLevel(hwin, Coop);
    }
    //------------------------------------------------------------------------
    void Win32JoyPadControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void Win32JoyPadControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void Win32JoyPadControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID Win32JoyPadControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void Win32JoyPadControlPattern::enumCom()
    {
        //We can check force feedback here too
        mCaps.dwSize = sizeof(DIDEVCAPS);
        mJoyStick->GetCapabilities(&mCaps);

        setButtonCount(mCaps.dwButtons);

        setAxisCount(mCaps.dwAxes);

        //Reset the axis mapping enumeration value
        mCurrentAxis = 0;

        //Enumerate Force Feedback (if any)
        mJoyStick->EnumEffects(enumEff, this, DIEFT_ALL);

        //Enumerate and set axis constraints (and check FF Axes)
        mJoyStick->EnumObjects(enumDev, this, DIDFT_AXIS);
    }
    //------------------------------------------------------------------------
    BOOL CALLBACK Win32JoyPadControlPattern::enumDev(
        LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID p)
    {
        Win32JoyPadControlPattern * temp = (Win32JoyPadControlPattern *)p;

        //Setup mappings
        DIPROPPOINTER diptr;
        diptr.diph.dwSize = sizeof(DIPROPPOINTER);
        diptr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diptr.diph.dwHow = DIPH_BYID;
        diptr.diph.dwObj = lpddoi->dwType;
        //Add the high bit in so that an axis value of zero does not mean a null userdata
        diptr.uData = 0x13130000 | temp->mCurrentAxis;

        if (FAILED(temp->mJoyStick->SetProperty(DIPROP_APPDATA, &diptr.diph)))
        {	//If for some reason we could not set needed user data, just ignore this axis
            return DIENUM_CONTINUE;
        }

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
    BOOL CALLBACK Win32JoyPadControlPattern::enumEff(LPCDIEFFECTINFO pdei, LPVOID p)
    {
        Win32JoyPadControlPattern * temp = (Win32JoyPadControlPattern *)p;

        //Create the FF class after we know there is at least one effect type
        if(temp->mFfeedback == 0)
            temp->mFfeedback = N_new Win32JoyDevForceFeedback(temp->mJoyStick, &temp->mCaps);

        temp->mFfeedback->_addEffectSupport(pdei);

        return DIENUM_CONTINUE;
    }
    //------------------------------------------------------------------------
}
}
