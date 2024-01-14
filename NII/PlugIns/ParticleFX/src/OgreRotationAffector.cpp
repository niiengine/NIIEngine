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
#include "OgreRotationAffector.h"
#include "OgreParticleSpace.h"
#include "OgreStringConverter.h"
#include "OgreParticle.h"

namespace Ogre
{
    // init statics
	RotationAffector::CmdRotationSpeedRangeStart	RotationAffector::msRotationSpeedRangeStartCmd;
    RotationAffector::CmdRotationSpeedRangeEnd		RotationAffector::msRotationSpeedRangeEndCmd;
    RotationAffector::CmdRotationRangeStart			RotationAffector::msRotationRangeStartCmd;
    RotationAffector::CmdRotationRangeEnd			RotationAffector::msRotationRangeEndCmd;

    //-----------------------------------------------------------------------
	RotationAffector::RotationAffector(ParticleSpace* psys) :
        ParticleEffect(psys),
		mRotationSpeedRangeStart(0),
		mRotationSpeedRangeEnd(0),
		mRotationRangeStart(0),
		mRotationRangeEnd(0)
    {
		mType = "Rotator";

        // Init parameters
        if (createParamDictionary("RotationAffector"))
        {
            PropertyCmdSet* dict = getCmdObj();

            dict->addParameter(ParameterDef("rotation_speed_range_start",
				"The start of a range of rotation speed to be assigned to emitted particles.", PT_REAL),
				&msRotationSpeedRangeStartCmd);

			dict->addParameter(ParameterDef("rotation_speed_range_end",
				"The end of a range of rotation speed to be assigned to emitted particles.", PT_REAL),
				&msRotationSpeedRangeEndCmd);

			dict->addParameter(ParameterDef("rotation_range_start",
				"The start of a range of rotation angles to be assigned to emitted particles.", PT_REAL),
				&msRotationRangeStartCmd);

			dict->addParameter(ParameterDef("rotation_range_end",
				"The end of a range of rotation angles to be assigned to emitted particles.", PT_REAL),
				&msRotationRangeEndCmd);
        }
    }

    //-----------------------------------------------------------------------
	void RotationAffector::initImpl(Particle * pParticle)
	{
		pParticle->setRotation(mRotationRangeStart +
            (Math::randomf() * (mRotationRangeEnd - mRotationRangeStart)));
        pParticle->setRotationFactor(mRotationSpeedRangeStart +
            (Math::randomf() * (mRotationSpeedRangeEnd - mRotationSpeedRangeStart)));
	}
	//-----------------------------------------------------------------------
    void RotationAffector::effectImpl(ParticleList & list, TimeDurMS timeElapsed)
    {
        Particle * p;
        NIIf ds;
        // Rotation adjustments by time
        ds = timeElapsed;

		Radian NewRotation;

        ParticleList::const_iterator i, iend = list.end();
        for(i = list.begin(); i != iend; ++i)
        {
            p = *i;

			NewRotation = p->getRotation() + (ds * p->getRotationFactor());
			p->setRotation( NewRotation );
        }

    }
    //-----------------------------------------------------------------------
    const Radian& RotationAffector::getRotationSpeedRangeStart(void) const
    {
        return mRotationSpeedRangeStart;
    }
    //-----------------------------------------------------------------------
    const Radian& RotationAffector::getRotationSpeedRangeEnd(void) const
    {
        return mRotationSpeedRangeEnd;
    }
    //-----------------------------------------------------------------------
    void RotationAffector::setRotationSpeedRangeStart(const Radian& val)
    {
        mRotationSpeedRangeStart = val;
    }
    //-----------------------------------------------------------------------
    void RotationAffector::setRotationSpeedRangeEnd(const Radian& val )
    {
        mRotationSpeedRangeEnd = val;
    }
    //-----------------------------------------------------------------------
    const Radian& RotationAffector::getRotationRangeStart(void) const
    {
        return mRotationRangeStart;
    }
    //-----------------------------------------------------------------------
    const Radian& RotationAffector::getRotationRangeEnd(void) const
    {
        return mRotationRangeEnd;
    }
    //-----------------------------------------------------------------------
    void RotationAffector::setRotationRangeStart(const Radian& val)
    {
        mRotationRangeStart = val;
    }
    //-----------------------------------------------------------------------
    void RotationAffector::setRotationRangeEnd(const Radian& val )
    {
        mRotationRangeEnd = val;
    }
	//-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String RotationAffector::CmdRotationSpeedRangeEnd::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const RotationAffector*>(target)->getRotationSpeedRangeEnd() );
    }
    void RotationAffector::CmdRotationSpeedRangeEnd::doSet(void* target, const String& val)
    {
        static_cast<RotationAffector*>(target)->setRotationSpeedRangeEnd(StringConverter::parseAngle(val));
    }
    //-----------------------------------------------------------------------
    String RotationAffector::CmdRotationSpeedRangeStart::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const RotationAffector*>(target)->getRotationSpeedRangeStart() );
    }
    void RotationAffector::CmdRotationSpeedRangeStart::doSet(void* target, const String& val)
    {
        static_cast<RotationAffector*>(target)->setRotationSpeedRangeStart(StringConverter::parseAngle(val));
    }

	//-----------------------------------------------------------------------
    String RotationAffector::CmdRotationRangeEnd::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const RotationAffector*>(target)->getRotationRangeEnd() );
    }
    void RotationAffector::CmdRotationRangeEnd::doSet(void* target, const String& val)
    {
        static_cast<RotationAffector*>(target)->setRotationRangeEnd(StringConverter::parseAngle(val));
    }
    //-----------------------------------------------------------------------
    String RotationAffector::CmdRotationRangeStart::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const RotationAffector*>(target)->getRotationRangeStart() );
    }
    void RotationAffector::CmdRotationRangeStart::doSet(void* target, const String& val)
    {
        static_cast<RotationAffector*>(target)->setRotationRangeStart(StringConverter::parseAngle(val));
    }
}



