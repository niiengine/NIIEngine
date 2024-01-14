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
#include "NiiWin32MouseControlPattern.h"

namespace NII
{
    //------------------------------------------------------------------------
    Win32MouseControlPattern::Win32MouseControlPattern(){}
    //------------------------------------------------------------------------
    Win32MouseControlPattern::Win32MouseControlPattern(Nid id, IDirectInput8 * pdi) :
            MouseControlPattern(id),
            mDirectInput(pdi),
            mMouse(0){}
    //------------------------------------------------------------------------
    Win32MouseControlPattern::~Win32MouseControlPattern()
    {
        if (mMouse)
        {
            mMouse->Unacquire();
            mMouse->Release();
            mMouse = 0;
        }
    }
    //------------------------------------------------------------------------
    NIIb Win32MouseControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb Win32MouseControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void Win32MouseControlPattern::_init()
    {
        mX = 0;
        mY = 0;
        DIPROPDWORD dipdw;

        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = MOUSE_DX_BUFFERSIZE;

        if(FAILED(mDirectInput->CreateDevice(GUID_SysMouse, &mMouse, NULL)))
            N_EXCEPT(CreateDevice, _T("Could not init device !"));

        if(FAILED(mMouse->SetDataFormat(&c_dfDIMouse2)))
            N_EXCEPT(CreateDevice, _T("Could not set format !"));

        if(FAILED(mMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            N_EXCEPT(CreateDevice, _T("Could not set property !"));

        HRESULT hr = mMouse->Acquire();
        if (FAILED(hr) && hr != DIERR_OTHERAPPHASPRIO)
            N_EXCEPT(DeviceRun, _T("Could not aquire mouse!"));
    }
    //------------------------------------------------------------------------
    void Win32MouseControlPattern::_update()
    {
        if(mActive)
        {
            DIDEVICEOBJECTDATA diff[MOUSE_DX_BUFFERSIZE];
            DWORD count = MOUSE_DX_BUFFERSIZE;

            HRESULT hr = mMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diff, &count, 0);
            if(hr != DI_OK)
            {
                hr = mMouse->Acquire();
                while(hr == DIERR_INPUTLOST)
                    hr = mMouse->Acquire();

                hr = mMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diff, &count, 0);

                //Perhaps the user just tabbed away, and coop settings
                //are nonexclusive..so just ignore
                if(FAILED(hr))
                    return;
            }

            bool moved = false;
            Ni32 tempx = 0;
            Ni32 tempy = 0;
            Ni32 tempz = 0;
            Nid temp = DIMOFS_BUTTON0;
            //Accumulate all axis movements for one axesMove message..
            //Buttons are fired off as they are found
            for(NCount i = 0; i < count; ++i)
            {
                if(diff[i].dwData & 0x80)
                {
                    switch(diff[i].dwOfs)
                    {
                        case DIMOFS_X:
                            tempx += diff[i].dwData;
                            moved = true;
                            break;
                        case DIMOFS_Y:
                            tempy += diff[i].dwData;
                            moved = true;
                            break;
                        case DIMOFS_Z:
                            tempz += diff[i].dwData;
                            moved = true;
                            break;
                        default:
                            press((MouseButton)(diff[i].dwOfs - temp));
                    } //end switch
                }//end for
                else
                {
                    switch(diff[i].dwOfs)
                    {
                        case DIMOFS_X:
                            tempx += diff[i].dwData;
                            moved = true;
                            break;
                        case DIMOFS_Y:
                            tempy += diff[i].dwData;
                            moved = true;
                            break;
                        case DIMOFS_Z:
                            tempz += diff[i].dwData;
                            moved = true;
                            break;
                        default:
                            release((MouseButton)(diff[i].dwOfs - temp));
                    } //end switch
                }//end for
            }

            if(moved)
                move(tempx, tempy, tempz);
        }
    }
    //------------------------------------------------------------------------
    void Win32MouseControlPattern::_reset()
    {
        HWND Hwin;
		DWORD Coop;
        if(mOwn->getPlatformData("WINDOW", &Hwin))
            if(mOwn->getPlatformData("MS", &Coop))
                mMouse->SetCooperativeLevel(Hwin, Coop)
    }
    //------------------------------------------------------------------------
    void Win32MouseControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void Win32MouseControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void Win32MouseControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID Win32MouseControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}