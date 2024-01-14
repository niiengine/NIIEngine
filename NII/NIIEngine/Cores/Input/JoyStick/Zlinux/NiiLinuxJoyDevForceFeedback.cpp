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
#include "NiiLinuxJoyDevForceFeedback.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    LinuxJoyDevForceFeedback::LinuxJoyDevForceFeedback(NIIi devID) :
        mJoyStick(devID)
    {
    }
    //------------------------------------------------------------------------
    LinuxJoyDevForceFeedback::~LinuxJoyDevForceFeedback()
    {
        Effects::iterator i, iend = mEffects.end();
        for(i = mEffects.begin(); i != iend; ++i)
        {
            struct ff_effect * lineff = i->second;
            if(lineff)
                unload(lineff->id);
        }

        mEffects.clear();
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::setMasterGain(NIIf level)
    {
        if (!mSetGainSupport)
            return;

        struct input_event event;

        memset(&event, 0, sizeof(event));
        event.type = EV_FF;
        event.code = FF_GAIN;

        if (value < 0.0)
            value = 0.0;
        else if (value > 1.0)
            value = 1.0;
        event.value = (__s32)(value * 0xFFFFUL);

        if (write(mJoyStick, &event, sizeof(event)) != sizeof(event))
            N_EXCEPT(DeviceInner, _T("Unknown error changing master gain !"));
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::setAutoCenterMode(bool auto_on)
    {
        if (!mSetAutoCenterSupport)
            return;

        struct input_event event;

        memset(&event, 0, sizeof(event));
        event.type = EV_FF;
        event.code = FF_AUTOCENTER;
        event.value = (__s32)(enabled*0xFFFFFFFFUL);

        if (write(mJoyStick, &event, sizeof(event)) != sizeof(event))
            N_EXCEPT(DeviceInner, _T("Unknown error toggling auto-center !"));
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::upload(const JoyStickEffect * eff)
    {
        eff->apply(this);
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::modify(const JoyStickEffect * eff)
    {
        eff->apply(this);
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::remove(const JoyStickEffect * eff)
    {
        //Get the effect - if it exists
        Effects::iterator i = mEffects.find(eff->mHandle);
        if( i != mEffects.end() )
        {
            struct ff_effect * lineff = i->second;
            if(lineff)
            {
                stop(eff->mHandle);

                unload(eff->mHandle);

                free(lineff);

                mEffects.erase(i);
            }
            else
                mEffects.erase(i);
        }
    }
    //------------------------------------------------------------------------
    short LinuxJoyDevForceFeedback::getFFAxesNumber()
    {
        return -1;
    }
    //------------------------------------------------------------------------
    unsigned short LinuxJoyDevForceFeedback::getFFMemoryLoad()
    {
        int nEffects = -1;
        if (ioctl(mJoyStick, EVIOCGEFFECTS, &nEffects) == -1)
            N_EXCEPT(DeviceInner, _T("Unknown error reading max number of uploaded effects !"));

        return nEffects > 0 ? 100.0 * mEffects.size() / nEffects : 100;
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::upload(struct ff_effect * ffeffect,
        const JoyStickEffect * effect)
    {
        struct ff_effect * lineff = 0;

        //Get the effect - if it exists
        Effects::iterator i = mEffects.find(effect->mHandle);
        //It has been created already
        if(i != mEffects.end())
            lineff = i->second;

        if(lineff == 0)
        {
            //This effect has not yet been created, so create it in the device
            if (ioctl(mJoyStick, EVIOCSFF, ffeffect) == -1)
                // TODO device full check
                N_EXCEPT(DeviceRun, _T("Unknown error creating effect (may be the device is full) !"));

            // Save returned effect handle
            effect->mHandle = ffeffect->id;

            // Save a copy of the uploaded effect for later simple modifications
            lineff = (struct ff_effect *)calloc(1, sizeof(struct ff_effect));
            memcpy(lineff, ffeffect, sizeof(struct ff_effect));

            mEffects[effect->mHandle] = lineff;

            // Start playing the effect.
            start(effect->mHandle);
        }
        else
        {
            // Keep same id/handle, as this is just an update in the device.
            ffeffect->id = effect->mHandle;

            // Update effect in the device.
            if (ioctl(mJoyStick, EVIOCSFF, ffeffect) == -1)
                N_EXCEPT(DeviceRun, _T("Unknown error updating an effect !"));

            // Update local lineff for next time.
            memcpy(lineff, ffeffect, sizeof(struct ff_effect));
        }
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::stop(NIIi id)
    {
        struct input_event stop;

        stop.type = EV_FF;
        stop.code = id;
        stop.value = 0;

        if (write(mJoyStick, &stop, sizeof(stop)) != sizeof(stop))
            N_EXCEPT(DeviceInner, _T("Unknown error stopping effect !"));
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::start(NIIi id)
    {
        struct input_event play;

        play.type = EV_FF;
        play.code = id;
        play.value = 1; // Play once.

        if (write(mJoyStick, &play, sizeof(play)) != sizeof(play))
            N_EXCEPT(DeviceInner, _T("Unknown error playing effect !"));
    }
    //------------------------------------------------------------------------
    void LinuxJoyDevForceFeedback::unload(NIIi id)
    {
        if (ioctl(mJoyStick, EVIOCRMFF, id) == -1)
            N_EXCEPT(DeviceInner, _T("Unknown error removing effect !"));
    }
    //------------------------------------------------------------------------
}
}