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
#include "NiiJoyDevEffect.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Envelope
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyStickEffect::Envelope::Envelope() :
        mAttackLength(0),
        mAttackLevel(0),
        mFadeLength(0),
        mFadeLevel(0){}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyStickEffect::JoyStickEffect() : 
        mType(EFT_Unknown),
        mFactorType(FEFT_Unknown),
        mAxes(1){}
    //------------------------------------------------------------------------
    JoyStickEffect::JoyStickEffect(ForceEffectType type, ForceEffectFactorType factor) : 
        mType(type),
        mFactorType(factor),
        mDirection(FED_North),
        mTriggerButton(-1),
        mTriggerInterval(0),
        mReplayLength(0xFFFFFFFF),
        mReplayDelay(0),
        mHandle(-1),
        mAxes(1)
    {
        N_EXCEPT(UnImpl, _I18n("Sorry, Requested Force not Implemented yet !"));
    }
    //------------------------------------------------------------------------
    JoyStickEffect::~JoyStickEffect(){}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JoyDevForceFeedback
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JoyDevForceFeedback::JoyDevForceFeedback() :
        mSetGainSupport(false),
        mSetAutoCenterSupport(true)
    {
    }
    //------------------------------------------------------------------------
    JoyDevForceFeedback::~JoyDevForceFeedback()
    {
    }
    //------------------------------------------------------------------------
    void JoyDevForceFeedback::_setGainSupport(bool b)
    {
        mSetGainSupport = b;
    }
    //------------------------------------------------------------------------
    void JoyDevForceFeedback::_setAutoCenterSupport(bool b)
    {
        mSetAutoCenterSupport = b;
    }
    //------------------------------------------------------------------------
    const JoyDevForceFeedback::SupportedEffectList & JoyDevForceFeedback::getSupportedEffects() const
    {
        return mSupportList;
    }
    //------------------------------------------------------------------------
    bool JoyDevForceFeedback::supportsEffect(JoyStickEffect::ForceEffectType force, 
        JoyStickEffect::ForceEffectFactorType type) const
    {
        const std::pair<SupportedEffectList::const_iterator, SupportedEffectList::const_iterator> 
            iterRange = mSupportList.equal_range(force);
        SupportedEffectList::const_iterator iter;
        for (iter = iterRange.first; iter != iterRange.second; iter++)
        {
          if ((*iter).second == type)
            return true;
        }

        return false;
    }
    //------------------------------------------------------------------------
    void JoyDevForceFeedback::_addEffectTypes(JoyStickEffect::ForceEffectType force, 
        JoyStickEffect::ForceEffectFactorType type)
    {
        if(force <= JoyStickEffect::EFT_Unknown || force >= JoyStickEffect::EFT_Custom ||
            type <= JoyStickEffect::FEFT_Unknown || type >= JoyStickEffect::FEFT_Custom)
            N_EXCEPT(Internal, _T("Can't add unknown effect Force/Type to the supported list"));

        mSupportList.insert(Npair(force, type));
    }
    //------------------------------------------------------------------------
}
}