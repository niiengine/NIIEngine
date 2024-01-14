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
#include "OgreDirectionRandomiserAffector.h"
#include "OgreParticleSpace.h"
#include "OgreParticle.h"
#include "OgreStringConverter.h"

namespace Ogre 
{
    // Instantiate statics
    DirectionRandomiserAffector::CmdRandomness DirectionRandomiserAffector::msRandomnessCmd;
    DirectionRandomiserAffector::CmdScope DirectionRandomiserAffector::msScopeCmd;
    DirectionRandomiserAffector::CmdKeepVelocity DirectionRandomiserAffector::msKeepVelocityCmd;

    //-----------------------------------------------------------------------
    DirectionRandomiserAffector::DirectionRandomiserAffector(ParticleSpace* psys)
       : ParticleEffect(psys)
    {
        mType = "DirectionRandomiser";

        // defaults
        mRandomness = 1.0;
        mScope = 1.0;
        mKeepVelocity = false;

        // Set up parameters
        if(createParamDictionary("DirectionRandomiserAffector"))
        {
            // Add extra paramaters
            PropertyCmdSet * dict = getCmdObj();
            dict->addParameter(ParameterDef("randomness",
                "The amount of randomness (chaos) to apply to the particle movement.",
                PT_REAL), &msRandomnessCmd);
            dict->addParameter(ParameterDef("scope",
                "The percentage of particles which is affected.",
                PT_REAL), &msScopeCmd);
            dict->addParameter(ParameterDef("keep_velocity",
                "Determines whether the velocity of the particles is changed.",
                PT_BOOL), &msKeepVelocityCmd);
        }
    }
    //-----------------------------------------------------------------------
    void DirectionRandomiserAffector::effectImpl(ParticleList & list, NIIf timeElapsed)
    {
        Particle * p;
        NIIf length = 0;
        
        ParticleList i, iend = list.end();
        for(i = list.begin(); i != iend; ++i)
        {
            p = *i;
            if (mScope > Math::randomf())
            {
                NIIf pdlen = p->mDirection.lengthSquared();
                if(pdlen > 0.000000000001)
                {
                    if (mKeepVelocity)
                    {
                        length = p->mDirection.length();
                    }

                    p->mDirection += Vector3(Math::random(-mRandomness, mRandomness) * timeElapsed,
                        Math::random(-mRandomness, mRandomness) * timeElapsed,
                        Math::random(-mRandomness, mRandomness) * timeElapsed);

                    if (mKeepVelocity)
                    {
                        p->mDirection *= length / p->mDirection.length();
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void DirectionRandomiserAffector::setRandomness(NIIf force)
    {
        mRandomness = force;
    }
    //-----------------------------------------------------------------------
    void DirectionRandomiserAffector::setScope(NIIf scope)
    {
        mScope = scope;
    }
    //-----------------------------------------------------------------------
    void DirectionRandomiserAffector::setKeepVelocity(bool keepVelocity)
    {
        mKeepVelocity = keepVelocity;
    }
    //-----------------------------------------------------------------------
    NIIf DirectionRandomiserAffector::getRandomness(void) const
    {
        return mRandomness;
    }
    //-----------------------------------------------------------------------
    NIIf DirectionRandomiserAffector::getScope(void) const
    {
        return mScope;
    }
    //-----------------------------------------------------------------------
    bool DirectionRandomiserAffector::getKeepVelocity(void) const
    {
        return mKeepVelocity;
    }

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String DirectionRandomiserAffector::CmdRandomness::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const DirectionRandomiserAffector*>(target)->getRandomness() );
    }
    void DirectionRandomiserAffector::CmdRandomness::doSet(void* target, const String& val)
    {
        static_cast<DirectionRandomiserAffector*>(target)->setRandomness(StringConverter::parseReal(val));
    }

    String DirectionRandomiserAffector::CmdScope::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const DirectionRandomiserAffector*>(target)->getScope() );
    }
    void DirectionRandomiserAffector::CmdScope::doSet(void* target, const String& val)
    {
        static_cast<DirectionRandomiserAffector*>(target)->setScope(StringConverter::parseReal(val));
    }
    String DirectionRandomiserAffector::CmdKeepVelocity::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const DirectionRandomiserAffector*>(target)->getKeepVelocity() );
    }
    void DirectionRandomiserAffector::CmdKeepVelocity::doSet(void* target, const String& val)
    {
        static_cast<DirectionRandomiserAffector*>(target)->setKeepVelocity(StringConverter::parseBool(val));
    }

}
