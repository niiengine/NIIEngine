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
#include "OgreColourFaderAffector.h"
#include "OgreParticleSpace.h"
#include "OgreStringConverter.h"
#include "OgreParticle.h"

namespace Ogre
{
    // init statics
    ColourFaderAffector::CmdRedAdjust ColourFaderAffector::msRedCmd;
    ColourFaderAffector::CmdGreenAdjust ColourFaderAffector::msGreenCmd;
    ColourFaderAffector::CmdBlueAdjust ColourFaderAffector::msBlueCmd;
    ColourFaderAffector::CmdAlphaAdjust ColourFaderAffector::msAlphaCmd;

    //-----------------------------------------------------------------------
    ColourFaderAffector::ColourFaderAffector(ParticleSpace* psys) : ParticleEffect(psys)
    {
        mRedAdj = mGreenAdj = mBlueAdj = mAlphaAdj = 0;
        mType = "ColourFader";

        // Init parameters
        if (createParamDictionary("ColourFaderAffector"))
        {
            PropertyCmdSet * dict = getCmdObj();

            dict->addParameter(ParameterDef("red",
                "The amount by which to adjust the red component of particles per second.",
                PT_REAL), &msRedCmd);
            dict->addParameter(ParameterDef("green",
                "The amount by which to adjust the green component of particles per second.",
                PT_REAL), &msGreenCmd);
            dict->addParameter(ParameterDef("blue",
                "The amount by which to adjust the blue component of particles per second.",
                PT_REAL), &msBlueCmd);
            dict->addParameter(ParameterDef("alpha",
                "The amount by which to adjust the alpha component of particles per second.",
                PT_REAL), &msAlphaCmd);


        }
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::effectImpl(ParticleList & list, TimeDurMS timeElapsed)
    {
        Particle *p;
        float dr, dg, db, da;

        // Scale adjustments by time
        dr = mRedAdj * timeElapsed;
        dg = mGreenAdj * timeElapsed;
        db = mBlueAdj * timeElapsed;
        da = mAlphaAdj * timeElapsed;

        ParticleList::iterator i, iend = list.end();
        for(i = list.begin(); i != iend; ++i)
        {
            p = *i;
            applyAdjustWithClamp(&p->colour.r, dr);
            applyAdjustWithClamp(&p->colour.g, dg);
            applyAdjustWithClamp(&p->colour.b, db);
            applyAdjustWithClamp(&p->colour.a, da);
        }
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setAdjust(float red, float green, float blue, float alpha)
    {
        mRedAdj = red;
        mGreenAdj = green;
        mBlueAdj = blue;
        mAlphaAdj = alpha;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setRedAdjust(float red)
    {
        mRedAdj = red;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getRedAdjust(void) const
    {
        return mRedAdj;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setGreenAdjust(float green)
    {
        mGreenAdj = green;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getGreenAdjust(void) const
    {
        return mGreenAdj;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setBlueAdjust(float blue)
    {
        mBlueAdj = blue;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getBlueAdjust(void) const
    {
        return mBlueAdj;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setAlphaAdjust(float alpha)
    {
        mAlphaAdj = alpha;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getAlphaAdjust(void) const
    {
        return mAlphaAdj;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdRedAdjust::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const ColourFaderAffector*>(target)->getRedAdjust() );
    }
    void ColourFaderAffector::CmdRedAdjust::doSet(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setRedAdjust(
            StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdGreenAdjust::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const ColourFaderAffector*>(target)->getGreenAdjust() );
    }
    void ColourFaderAffector::CmdGreenAdjust::doSet(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setGreenAdjust(
            StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdBlueAdjust::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const ColourFaderAffector*>(target)->getBlueAdjust() );
    }
    void ColourFaderAffector::CmdBlueAdjust::doSet(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setBlueAdjust(
            StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdAlphaAdjust::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const ColourFaderAffector*>(target)->getAlphaAdjust() );
    }
    void ColourFaderAffector::CmdAlphaAdjust::doSet(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setAlphaAdjust(
            StringConverter::parseReal(val));
    }

}



