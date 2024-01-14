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
#include "OgreColourImageAffector.h"
#include "OgreParticleSpace.h"
#include "OgreStringConverter.h"
#include "OgreParticle.h"
#include "OgreException.h"
#include "OgreResourceLoadManager.h"

namespace Ogre
{
    // init statics
	ColourImageAffector::CmdImageAdjust ColourImageAffector::msImageCmd;

    //-----------------------------------------------------------------------
    ColourImageAffector::ColourImageAffector(ParticleSpace * psys) :
        ParticleEffect(psys), mColourImageLoaded(false)
    {
        mType = "ColourImage";

        // Init parameters
        if (createParamDictionary("ColourImageAffector"))
        {
            PropertyCmdSet* dict = getCmdObj();

			dict->addParameter(ParameterDef("image", "image where the colours come from", PT_STRING), &msImageCmd);
        }
    }
    //-----------------------------------------------------------------------
    void ColourImageAffector::initImpl(Particle * obj)
	{
        if(!mColourImageLoaded)
        {
            _loadImage();
        }

        mColourImage.getColour(obj->colour, 0, 0, 0);
	}
    //-----------------------------------------------------------------------
    void ColourImageAffector::effectImpl(ParticleList & list, NIIf timeElapsed)
    {
        Particle * p;
        if(!mColourImageLoaded)
        {
            _loadImage();
        }

		int width = (int)mColourImage.getWidth()  - 1;

        ParticleList::const_iterator i, iend = list.end();
		for(i = list.begin(); i != iend; ++i)
		{
			p = *i;
			const NIIf life_time = p->mTotalTTL;
			NIIf particle_time = 1.0f - (p->mRemainTTL / life_time);

			if (particle_time > 1.0f)
				particle_time = 1.0f;
			if (particle_time < 0.0f)
				particle_time = 0.0f;

			const NIIf float_index = particle_time * width;
			const int index = (int)float_index;

            if(index < 0)
            {
				mColourImage.getColour(p->colour, 0, 0, 0);
            }
            else if(index >= width)
            {
                mColourImage.getColour(p->colour, width, 0, 0);
            }
            else
            {
                // Linear interpolation
				const NIIf		fract		= float_index - (NIIf)index;
				const NIIf		to_colour	= fract;
				const NIIf		from_colour	= 1.0f - to_colour;

                Colour from, to;
                mColourImage.getColour(from, index, 0, 0),
                mColourImage.getColour(to, index+1, 0, 0);

				p->colour.r = from.r*from_colour + to.r*to_colour;
                p->colour.g = from.g*from_colour + to.g*to_colour;
                p->colour.b = from.b*from_colour + to.b*to_colour;
                p->colour.a = from.a*from_colour + to.a*to_colour;
			}
		}
    }
    //-----------------------------------------------------------------------
    void ColourImageAffector::setImageAdjust(String name)
    {
		mColourImageName = name;
        mColourImageLoaded = false;
	}
    //-----------------------------------------------------------------------
    void ColourImageAffector::_loadImage(void)
    {
        mColourImage.load(mColourImageName, mParent->getResourceGroup());

		PixelFormat	format = mColourImage.getFormat();

		if(!PixelUtil::isAccessible(format))
		{
			N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Error: Image is not accessible (rgba) image.");
		}

        mColourImageLoaded = true;
	}
    //-----------------------------------------------------------------------
    String ColourImageAffector::getImageAdjust(void) const
    {
        return mColourImageName;
    }


	//-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String ColourImageAffector::CmdImageAdjust::doGet(const void* target) const
    {
        return static_cast<const ColourImageAffector*>(target)->getImageAdjust();
    }
    void ColourImageAffector::CmdImageAdjust::doSet(void* target, const String& val)
    {
        static_cast<ColourImageAffector*>(target)->setImageAdjust(val);
    }

}



