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

#include "NiiLinuxControlPatternManager.h"
#include "NiiLinuxKeyboardControlPattern.h"
#include "NiiLinuxMouseControlPattern.h"
#include "NiiLinuxJoyPadControlPattern.h"
#include "NiiLinuxJoyStickControlPattern.h"
#include "NiiLinuxJoyHandleControlPattern.h"
#include "NiiLinuxJoyWheelControlPattern.h"
#include "NiiLinuxTouchControlPattern.h"

namespace NII
{
    //------------------------------------------------------------------------
    LinuxControlPatternManager::LinuxControlPatternManager() :
        mMark(0)
    {
    }
    //------------------------------------------------------------------------
    LinuxControlPatternManager::~LinuxControlPatternManager()
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
    }
    //------------------------------------------------------------------------
    void LinuxControlPatternManager::init()
    {
        enumDev();
    }
    //------------------------------------------------------------------------
    void LinuxControlPatternManager::retrieve(const ViewWindow * main, ControlPattern * dest)
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
    NCount LinuxControlPatternManager::getCount(ControlDevType type) const
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
    ControlPattern * LinuxControlPatternManager::createImpl(const ViewWindow * main,
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
    void LinuxControlPatternManager::enumDev()
    {
        // RATS: Only used in snprintf
        Ni8 name[64];
        // RATS: Use ok
        Ni8 buf[256] = { 0 };
        int i, fd, ver;
        Nui8 mask[EV_MAX / 8 + 1];
        ControlPattern * re;

        for (i = 0; i < 64; ++i)
        {
            snprintf(name, sizeof(name), "/dev/input/event%d", i);
            if ((fd = open(name, O_RDWR | O_NONBLOCK, 0)) >= 0)
            {
                Nui8 t[1 + KEY_MAX / 8];
                memset(t, 0, sizeof(t));

                memset(mask, 0, sizeof(mask));
                ioctl(fd, EVIOCGVERSION, &ver);

                if(ioctl(fd, EVIOCGBIT(0, sizeof(mask)), mask) == -1)
                    continue;
                if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(t)), t) == -1)
                    continue;

                if(test_bit(mask, EV_KEY) && test_bit(mask, EV_LED) &&
                    !test_bit(mask, EV_ABS))
                {
                    for(j = 0; j < KEY_MAX; ++j)
                    {
                        if(test_bit(t, j) && (j > KEY_RESERVED && j < BTN_MISC))
                        {
                            re = N_new LinuxKeyboardControlPattern(mMark++, fd);
                            break;
                        }
                    }
                }
                else if(test_bit(mask, EV_REL) && test_bit(mask, EV_KEY))
                {
                    for(j = 0; j < KEY_MAX; ++j)
                    {
                        if(test_bit(t, j) && (j >= BTN_MOUSE && j <= BTN_TASK))
                        {
                            re = N_new LinuxMouseControlPattern(mMark++, fd);
                            break;
                        }
                    }
                }
                else if(test_bit(mask, EV_ABS) && test_bit(mask, EV_REL) &&
                    test_bit(mask, EV_KEY))
                {

                    Nui8 t1[1 + ABS_MAX / 8];
                    memset(t1, 0, sizeof(t1));

                    if (ioctl(deviceID, EVIOCGBIT(EV_ABS, sizeof(t1)), t1) == -1)
                        continue;

                    for(j = 0; j < KEY_MAX; ++j)
                    {
                        if(test_bit(t, j) && (j >= BTN_JOYSTICK && j < BTN_GAMEPAD))
                            re = N_new LinuxJoyStickControlPattern(mMark++, fd);
                        else if(test_bit(t, j) && (j >= BTN_GAMEPAD && j < BTN_DIGI))
                            re = N_new LinuxJoyPadControlPattern(mMark++, fd);
                        else if(test_bit(t, j) && (j >= BTN_DIGI && j < BTN_WHEEL)
                            re = N_new LinuxTouchControlPattern(mMark++, fd);
                        else if(test_bit(t, j) && (j >= BTN_WHEEL && j < KEY_OK))
                            re = N_new LinuxJoyWheelControlPattern(mMark++, fd);
                        break;
                    }
                }
                else
                {
                    re = NULL;
                    close(fd);
                    fd = -1;
                }

                if(fd != -1)
                {
                    if(ioctl(fd, EVIOCGNAME(sizeof(buf)), buf) >= 0)
                    {
                        re->setVendor(buf);
                        mDev.push_back(re);
                        re->_pause();
                        N_Engine().add(re);
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
}