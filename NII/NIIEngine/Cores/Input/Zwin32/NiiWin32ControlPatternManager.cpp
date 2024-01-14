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

#include "NiiWin32ControlPatternManager.h"
#include "NiiWin32KeyboardControlPattern.h"
#include "NiiWin32MouseControlPattern.h"
#include "NiiWin32JoyPadControlPattern.h"
#include "NiiWin32JoyStickControlPattern.h"
#include "NiiWin32JoyHandleControlPattern.h"
#include "NiiWin32JoyWheelControlPattern.h"
#include "NiiWin32TouchlControlPattern.h"

namespace NII
{
    //------------------------------------------------------------------------
    Win32ControlPatternManager::Win32ControlPatternManager() :
        mKBMark(0),
        mMSMark(0),
        mJDMark(0),
        mMark(0)
    {
#ifdef NII_STATIC_LIB
        HINSTANCE hInst = GetModuleHandle(0);
#else
		HINSTANCE hInst = GetModuleHandle("Win32ControlPattern.dll");
#endif
        HRESULT hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
            (VOID**)&mDirectInput, NULL);

        if (FAILED(hr))
            N_EXCEPT(DeviceInner, _T("Create control input error !"));

        mDirectInput->EnumDevices(NULL, enumDevCB, this, DIEDFL_ATTACHEDONLY);
    }
    //------------------------------------------------------------------------
    Win32ControlPatternManager::~Win32ControlPatternManager()
    {
        WindowControlList::iterator a, aend = mIdleDev.end();
        for(a = mIdleDev.begin(); a != aend; ++a)
        {
            (a->second).clear();
        }
        mIdleDev.clear();

        ControlList::iterator i, iend = mDev.end();
        for(i = mDev.begin(); i != iend; ++i)
        {
            N_Engine().remove(*i);
            N_delete *i;
        }
        mDev.clear();

        if(mDirectInput)
        {
            mDirectInput->Release();
            mDirectInput = 0;
        }
    }
    //------------------------------------------------------------------------
    void Win32ControlPatternManager::init()
    {
    }
    //------------------------------------------------------------------------
    void Win32ControlPatternManager::retrieve(const ViewWindow * main,
        ControlPattern * dest)
    {
        WindowControlList::iterator c = mIdleDev.find(main);
        if(c != mIdleDev.end())
        {
            ControlList::iterator it, itend = (c->second).end();
            for(it = (c->second).begin(); it != itend; ++it)
            {
                if((*it) == dest)
                {
                    (c->second).erase(it);
                    break;
                }
            }
            if((c->second).empty())
                mIdleDev.erase(c);
        }
    }
    //------------------------------------------------------------------------
    NCount Win32ControlPatternManager::getCount(ControlDevType type) const
    {
        NCount count = 0;
        ControlList::iterator i, iend = mDev.end();
        for(i = mDev.begin(); i != iend; ++i)
        {
            if((*i)->getType() == type)
                ++ count;
        }
        return count;
    }
    //------------------------------------------------------------------------
    ControlPattern * Win32ControlPatternManager::createImpl(const ViewWindow * main,
        ControlDevType type)
    {
        ControlPattern * re = NULL;
        WindowControlList::iterator c = mIdleDev.find(main);
        if(c != mIdleDev.end())
        {
            ControlList::iterator i, iend = (c->second).end();
            for(i = (c->second).begin(); i != iend; ++i)
            {
                if((*i)->getType() == type)
                {
                    (c->second).erase(i);
                    re = *i;
                    break;
                }
            }
        }
        else
        {
            ControlList temp;
            temp.insert(temp.begin(), mDev.begin(), mDev.end());
            ControlList::iterator i, iend = temp.end();
            for(i = temp.begin(); i != iend; ++i)
            {
                if((*i)->getType() == type)
                {
                    temp.erase(i);
                    re = *i;
                    break;
                }
            }
            mIdleDev.insert(WindowControlList::value_type(main, temp));
        }

        return re;
    }
    //-----------------------------------------------------------------------
    BOOL CALLBACK Win32ControlPatternManager::enumDevCB(LPCDIDEVICEINSTANCE dev,
        LPVOID p)
    {
        ControlPattern * re;
        Win32ControlPatternManager * temp = static_cast<Win32ControlPatternManager *>(p);

        switch(GET_DIDEVICE_TYPE(dev->dwDevType))
        {
        case DI8DEVTYPE_KEYBOARD :
            re = N_new Win32KeyboardControlPattern(temp->mMark++, mDirectInput);
            break;
        case DI8DEVTYPE_MOUSE :
            re = N_new Win32MouseControlPattern(temp->mMark++, mDirectInput);
            break;
        case DI8DEVTYPE_JOYSTICK :
        case DI8DEVTYPE_FLIGHT :
            re = N_new Win32JoyStickControlPattern(temp->mMark++, mDirectInput,
                dev->guidInstance);
            break;
        case DI8DEVTYPE_GAMEPAD :
            re = N_new Win32JoyPadControlPattern(temp->mMark++, mDirectInput,
                dev->guidInstance);
            break;
        case DI8DEVTYPE_1STPERSON :
            re = N_new Win32JoyHandleControlPattern(temp->mMark++, mDirectInput,
                dev->guidInstance);
            break;
        case DI8DEVTYPE_DRIVING :
            re = N_new Win32JoyWheelControlPattern(temp->mMark++, mDirectInput,
                dev->guidInstance);
            break;
        case DI8DEVTYPE_SCREENPOINTER :
            re = N_new Win32TouchControlPattern(temp->mMark++);
            break;
        default:
            break;
        }
        re->setVendor(dev->tszInstanceName);
        temp->mDev.push_back(re);
        re->_pause();
        N_Engine().add(re);
        return DIENUM_CONTINUE;
    }
    //-----------------------------------------------------------------------
}