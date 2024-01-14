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
#include "NiiLinuxControlPatternManager.h"
#include "NiiLinuxJoyPadControlPattern.h"

#define EV_MAKE   1  // when key pressed
#define EV_BREAK  0  // when key released
#define EV_REPEAT 2  // when key switches to repeating after short delay

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    LinuxJoyPadControlPattern::LinuxJoyPadControlPattern(Nid id, NIIi main) :
        JoyPadControlPattern(id),
        mJoyPad(main),
        mEff(NULL)
    {
    }
    //------------------------------------------------------------------------
    LinuxJoyPadControlPattern::~LinuxJoyPadControlPattern()
    {
        if(mEff != NULL)
            N_delete mEff;
        if(mJoyPad != -1)
            close(mJoyPad);
    }
    //------------------------------------------------------------------------
    NIIb LinuxJoyPadControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb LinuxJoyPadControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void LinuxJoyPadControlPattern::_init()
    {
        if(mJoyPad == -1)
            N_EXCEPT(CreateDevice, _T("Could not initialize joy device !"));

        enumCom();
        enumEff();
    }
    //------------------------------------------------------------------------
    void LinuxJoyPadControlPattern::_update()
    {
        if(mActive)
        {
            NCount i;
            vector<bool>::type ax(mAxisBuffer.size(), false);

            //We are in non blocking mode - we just read once, and try to fill up buffer
            struct input_event js[JOY_BUFFERSIZE];
            ssize_t count = read(mJoyPad, &js, sizeof(js)) / sizeof(struct input_event);
            for(i = 0; i < count; ++i)
            {
                switch(js[i].type)
                {
                case EV_KEY:
                    if(js[i].value == EV_MAKE)
                        press(js[i].code - mButtonOffset);
                    else if(js[i].value == EV_BREAK)
                        release(js[i].code - mButtonOffset);
                    break;
                case EV_ABS:
                    if(js[i].code <= ABS_BRAKE)
                    {
                        Nid axis = js[i].code - mAxisOffset;
                        mAxisBuffer[axis] = js[i].value * mRanges[axis].frist + mRanges[axis].second;
                        ax[axis] = true;
                    }
                default: break;
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
    void LinuxJoyPadControlPattern::_reset()
    {
        // do nothing
    }
    //------------------------------------------------------------------------
    void LinuxJoyPadControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void LinuxJoyPadControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void LinuxJoyPadControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID LinuxJoyPadControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void LinuxJoyPadControlPattern::enumCom()
    {
        NCount amin = 0, amax = 0;
        NCount bmin = 0, bmax = 0;
        Nui8 t1[1 + ABS_MAX / 8];
        memset(t1, 0, sizeof(t1));
        Nui8 t2[1 + KEY_MAX / 8];
        memset(t2, 0, sizeof(t2));
        if (ioctl(mJoyPad, EVIOCGBIT(EV_ABS, sizeof(t1)), t1) == -1)
            N_EXCEPT(DeviceInner, _T("Could not read device com !"));
        if (ioctl(mJoyPad, EVIOCGBIT(EV_KEY, sizeof(t2)), t2) == -1)
            N_EXCEPT(DeviceInner, _T("Could not read device com !"));

        int i;
        NIIi scale;
        NIIi offset;
        NCount t11 = 0;
        NCount t22 = 0;
        for (i = 0; i < ABS_MAX; ++i)
        {
            if(test_bit(t1, i) && i <= ABS_BRAKE)
            {
                if(amin > i)
                    mAxisOffset = i;
                if(amax < i)
                    amax = i;

                input_absinfo info;
                if (ioctl(mJoyPad, EVIOCGABS(i), &info) == -1)
                    N_EXCEPT(DeviceInner, _T("Could not read device absolute axis!"));
                t11 = info.maximum - info.minimum;
                t22 = MAX_AXIS - MIN_AXIS;
                scale = t22 / t11;
                t11 = info.maximum * scale;
                if(MAX_AXIS > t11)
                    offset = MAX_AXIS - t11;
                else
                    offset = t11 - MAX_AXIS;
                mRanges.push_back(std::pair<NIIi, NIIi>(scale, offset));
            }
        }

        for (i = 0; i < KEY_MAX; ++i)
        {
            if(test_bit(t2, i))
            {
                if(bmin > i)
                    mButtonOffset = j;
                if(bmax < i)
                    bmax = j;
            }
        }

        setAxisCount(amax - mAxisOffset + 1);
        setButtonCount(bmax - mButtonOffset + 1);
    }
    //------------------------------------------------------------------------
    void LinuxJoyPadControlPattern::enumEff()
    {
        //Linux Event to OIS Event Mappings
        map<int, JoyStickEffect::ForceEffectFactorType>::type eff;
        eff[FF_CONSTANT] = JoyStickEffect::FEFT_Constant;
        eff[FF_RAMP]     = JoyStickEffect::FEFT_Ramp;
        eff[FF_SPRING]   = JoyStickEffect::FEFT_Spring;
        eff[FF_FRICTION] = JoyStickEffect::FEFT_Friction;
        eff[FF_SQUARE]   = JoyStickEffect::FEFT_Square;
        eff[FF_TRIANGLE] = JoyStickEffect::FEFT_Triangle;
        eff[FF_SINE]     = JoyStickEffect::FEFT_Sine;
        eff[FF_SAW_UP]   = JoyStickEffect::FEFT_SawToothUp;
        eff[FF_SAW_DOWN] = JoyStickEffect::FEFT_SawToothDown;
        eff[FF_DAMPER]   = JoyStickEffect::FEFT_Damper;
        eff[FF_INERTIA]  = JoyStickEffect::FEFT_Inertia;
        eff[FF_CUSTOM]   = JoyStickEffect::FEFT_Custom;

        map<int, JoyStickEffect::ForceEffectType>::type force;
        force[FF_CONSTANT] = JoyStickEffect::EFT_Constant;
        force[FF_RAMP]     = JoyStickEffect::EFT_Ramp;
        force[FF_SPRING]   = JoyStickEffect::EFT_Conditional;
        force[FF_FRICTION] = JoyStickEffect::EFT_Conditional;
        force[FF_SQUARE]   = JoyStickEffect::EFT_Periodic;
        force[FF_TRIANGLE] = JoyStickEffect::EFT_Periodic;
        force[FF_SINE]     = JoyStickEffect::EFT_Periodic;
        force[FF_SAW_UP]   = JoyStickEffect::EFT_Periodic;
        force[FF_SAW_DOWN] = JoyStickEffect::EFT_Periodic;
        force[FF_DAMPER]   = JoyStickEffect::EFT_Conditional;
        force[FF_INERTIA]  = JoyStickEffect::EFT_Conditional;
        force[FF_CUSTOM]   = JoyStickEffect::EFT_Custom;

        //Remove any previously existing memory and create fresh
        if(mEff != NULL)
            N_delete mEff;
        *mEff = new LinuxJoyDevForceFeedback(mJoyPad);

        //Read overall force feedback features
        Nui8 t[1 + FF_MAX / 8];
        memset(t, 0, sizeof(t));

        if (ioctl(mJoyPad, EVIOCGBIT(EV_FF, sizeof(t)), t) == -1)
            N_EXCEPT(CreateDevice, _T("Could not read device force feedback !"));

        //FF Effects
        for(NCount effect = FF_EFFECT_MIN; effect <= FF_WAVEFORM_MAX; ++effect)
        {
            // The RUMBLE force type is ignored, as periodic force one is more powerfull.
            // The PERIODIC force type is processed later, for each associated periodic effect type.
            if (effect == FF_RUMBLE || effect == FF_PERIODIC)
                continue;

            if(test_bit(t, effect))
                (*mEff)->_addEffectTypes(force[effect], eff[effect]);
        }

        //FF device properties
        if (test_bit(t, FF_GAIN))
            (*mEff)->_setGainSupport(true);

        if (test_bit(t, FF_AUTOCENTER))
            (*mEff)->_setAutoCenterSupport(true);

        //Check to see if any effects were added, else destroy the pointer
        const ForceFeedback::SupportedEffectList & list = (*mEff)->getSupportedEffects();
        if(list.size() == 0)
            N_delete mEff;
    }
    //------------------------------------------------------------------------
}
}
