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

#include "OgreScaleAffector.h"
#include "OgreParticleSpace.h"
#include "OgreStringConverter.h"
#include "OgreParticle.h"

namespace Ogre
{
    //-----------------------------------------------------------------------
    ScaleAffector::CmdScaleAdjust ScaleAffector::msScaleCmd;
    //-----------------------------------------------------------------------
    ScaleAffector::ScaleAffector(ParticleSpace * psys) :
        ParticleEffect(psys)
    {
        mScaleAdj = 0;
        mType = "Scaler";

        // Init parameters
        if(createParamDictionary("ScaleAffector"))
        {
            PropertyCmdSet * dict = getCmdObj();

            dict->addParameter(ParameterDef("rate",
                "The amount by which to adjust the x and y scale components of particles per second.",
                    PT_REAL), &msScaleCmd);
        }
    }
    //-----------------------------------------------------------------------
    void ScaleAffector::effectImpl(ParticleList & list, TimeDurMS timeElapsed)
    {
        Particle * p;
        NIIf ds;

        // Scale adjustments by time
        ds = mScaleAdj * timeElapsed;

		NIIf NewWide, NewHigh;

        ParticleList::iterator i, iend = list.end();
        for(i = list.begin(); i != iend; ++i)
        {
            p = *i;

			if(p->isInheritSize() == true)
			{
            	NewWide = p->getCtrlWidth() + ds;
	            NewHigh = p->getCtrlHeight() + ds;
			}
			else
			{
            	NewWide = p->getWidth()  + ds;
            	NewHigh = p->getHeight() + ds;
			}
			p->setSize(NewWide, NewHigh);
        }
    }
    //-----------------------------------------------------------------------
    void ScaleAffector::setAdjust( NIIf rate )
    {
        mScaleAdj = rate;
    }
    //-----------------------------------------------------------------------
    NIIf ScaleAffector::getAdjust(void) const
    {
        return mScaleAdj;
    }
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String ScaleAffector::CmdScaleAdjust::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const ScaleAffector*>(target)->getAdjust() );
    }
    void ScaleAffector::CmdScaleAdjust::doSet(void* target, const String& val)
    {
        static_cast<ScaleAffector*>(target)->setAdjust(
            StringConverter::parseReal(val));
    }

}



