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
#include "OgreColourInterpolatorAffector.h"
#include "OgreParticleSpace.h"
#include "OgreStringConverter.h"
#include "OgreParticle.h"

namespace Ogre 
{
    // init statics
    ColourInterpolatorAffector::CmdColourAdjust		ColourInterpolatorAffector::msColourCmd[MAX_STAGES];
    ColourInterpolatorAffector::CmdTimeAdjust		ColourInterpolatorAffector::msTimeCmd[MAX_STAGES];

    //-----------------------------------------------------------------------
    ColourInterpolatorAffector::ColourInterpolatorAffector(ParticleSpace* psys)
        : ParticleEffect(psys)
    {
		for (int i=0;i<MAX_STAGES;i++)
		{
			// set default colour to transparent grey, transparent since we might not want to display the particle here
			// grey because when a colour component is 0.5f the maximum difference to another colour component is 0.5f
			mColourAdj[i]	= Colour(0.5f, 0.5f, 0.5f, 0.0f);
			mTimeAdj[i]		= 1.0f;
		}

        mType = "ColourInterpolator";

        // Init parameters
        if(createParamDictionary("ColourInterpolatorAffector"))
        {
            PropertyCmdSet * dict = getCmdObj();

			for(int i = 0; i < MAX_STAGES; ++i)
			{
				msColourCmd[i].mIndex	= i;
				msTimeCmd[i].mIndex		= i;

				StringUtil::StrStreamType stage;
				stage << i;
				String	colour_title	= String("colour") + stage.str();
				String	time_title		= String("time") + stage.str();
				String	colour_descr	= String("Stage ") + stage.str() + String(" colour.");
				String	time_descr		= String("Stage ") + stage.str() + String(" time.");

				dict->addParameter(ParameterDef(colour_title, colour_descr, PT_COLOURVALUE), &msColourCmd[i]);
				dict->addParameter(ParameterDef(time_title,   time_descr,   PT_REAL),		 &msTimeCmd[i]);
			}
        }
    }
    //-----------------------------------------------------------------------
    void ColourInterpolatorAffector::effectImpl(ParticleList & list, NIIf timeElapsed)
    {
        Particle * p;
		ParticleList::iterator i, iend = list.end();
        for(i = list.begin(); i != iend; ++i)
        {
            p = *i;
			const NIIf life_time = p->mTotalTTL;
			NIIf particle_time = 1.0f - (p->mRemainTTL / life_time);

			if (particle_time <= mTimeAdj[0])
			{
				p->colour = mColourAdj[0];
			} else
			if (particle_time >= mTimeAdj[MAX_STAGES - 1])
			{
				p->colour = mColourAdj[MAX_STAGES-1];
			} else
			{
				for (int i=0;i<MAX_STAGES-1;i++)
				{
					if (particle_time >= mTimeAdj[i] && particle_time < mTimeAdj[i + 1])
					{
						particle_time -= mTimeAdj[i];
						particle_time /= (mTimeAdj[i+1]-mTimeAdj[i]);
						p->colour.r = ((mColourAdj[i+1].r * particle_time) + (mColourAdj[i].r * (1.0f - particle_time)));
						p->colour.g = ((mColourAdj[i+1].g * particle_time) + (mColourAdj[i].g * (1.0f - particle_time)));
						p->colour.b = ((mColourAdj[i+1].b * particle_time) + (mColourAdj[i].b * (1.0f - particle_time)));
						p->colour.a = ((mColourAdj[i+1].a * particle_time) + (mColourAdj[i].a * (1.0f - particle_time)));
						break;
					}
				}
			}
		}
    }

	//-----------------------------------------------------------------------
    void ColourInterpolatorAffector::setColourAdjust(size_t index, Colour colour)
    {
        mColourAdj[index] = colour;
    }
    //-----------------------------------------------------------------------
    Colour ColourInterpolatorAffector::getColourAdjust(size_t index) const
    {
        return mColourAdj[index];
    }


    //-----------------------------------------------------------------------
    void ColourInterpolatorAffector::setTimeAdjust(size_t index, NIIf time)
    {
        mTimeAdj[index] = time;
    }
    //-----------------------------------------------------------------------
    NIIf ColourInterpolatorAffector::getTimeAdjust(size_t index) const
    {
        return mTimeAdj[index];
    }


	//-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String ColourInterpolatorAffector::CmdColourAdjust::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const ColourInterpolatorAffector*>(target)->getColourAdjust(mIndex) );
    }
    void ColourInterpolatorAffector::CmdColourAdjust::doSet(void* target, const String& val)
    {
        static_cast<ColourInterpolatorAffector*>(target)->setColourAdjust(mIndex,
            StringConverter::parseColourValue(val));
    }
	//-----------------------------------------------------------------------
    String ColourInterpolatorAffector::CmdTimeAdjust::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const ColourInterpolatorAffector*>(target)->getTimeAdjust(mIndex) );
    }
    void ColourInterpolatorAffector::CmdTimeAdjust::doSet(void* target, const String& val)
    {
        static_cast<ColourInterpolatorAffector*>(target)->setTimeAdjust(mIndex,
            StringConverter::parseReal(val));
    }

}



