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
#include "OgreDeflectorPlaneAffector.h"
#include "OgreParticleSpace.h"
#include "OgreParticle.h"
#include "OgreStringConverter.h"

namespace Ogre
{
    // Instantiate statics
    DeflectorPlaneAffector::CmdPlanePoint DeflectorPlaneAffector::msPlanePointCmd;
    DeflectorPlaneAffector::CmdPlaneNormal DeflectorPlaneAffector::msPlaneNormalCmd;
    DeflectorPlaneAffector::CmdBounce DeflectorPlaneAffector::msBounceCmd;
    //-----------------------------------------------------------------------
    DeflectorPlaneAffector::DeflectorPlaneAffector(ParticleSpace * psys) :
        ParticleEffect(psys)
    {
        mType = "DeflectorPlane";

        // defaults
        mPlanePoint = Vector3::ZERO;
        mPlaneNormal = Vector3::Y;
        mBounce = 1.0;

        // Set up parameters
        if(createParamDictionary("DeflectorPlaneAffector"))
        {
            // Add extra paramaters
            PropertyCmdSet* dict = getCmdObj();
            dict->addParameter(ParameterDef("plane_point",
                "A point on the deflector plane. Together with the normal vector it defines the plane.",
                PT_VECTOR3), &msPlanePointCmd);
            dict->addParameter(ParameterDef("plane_normal",
                "The normal vector of the deflector plane. Together with the point it defines the plane.",
                PT_VECTOR3), &msPlaneNormalCmd);
            dict->addParameter(ParameterDef("bounce",
                "The amount of bouncing when a particle is deflected. 0 means no deflection and 1 stands for 100 percent reflection.",
                PT_REAL), &msBounceCmd);
        }
    }
    //-----------------------------------------------------------------------
    void DeflectorPlaneAffector::effectImpl(ParticleList & list, NIIf timeElapsed)
    {
        // precalculate distance of plane from origin
        NIIf planeDistance = - mPlaneNormal.dotProduct(mPlanePoint) / Math::sqrt(mPlaneNormal.dotProduct(mPlaneNormal));
		Vector3 directionPart;
        Particle * p;

        ParticleList::const_iterator i, iend = list.end();
        for(i = list.begin(); i != iend; ++i)
        {
            p = *i;

            Vector3 direction(p->mDirection * timeElapsed);
            if(mPlaneNormal.dotProduct(p->mPosition + direction) + planeDistance <= 0.0)
            {
                NIIf a = mPlaneNormal.dotProduct(p->mPosition) + planeDistance;
                if (a > 0.0)
                {
                    // for intersection point
					directionPart = direction * (- a / direction.dotProduct( mPlaneNormal ));
                    // set new mPosition
					p->mPosition = (p->mPosition + ( directionPart )) + (((directionPart) - direction) * mBounce);

                    // reflect direction vector
                    p->mDirection = (p->mDirection - (2.0 * p->mDirection.dotProduct( mPlaneNormal ) * mPlaneNormal)) * mBounce;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void DeflectorPlaneAffector::setPlanePoint(const Vector3 & pos)
    {
        mPlanePoint = pos;
    }
    //-----------------------------------------------------------------------
    void DeflectorPlaneAffector::setPlaneNormal(const Vector3 & normal)
    {
        mPlaneNormal = normal;
    }
    //-----------------------------------------------------------------------
    void DeflectorPlaneAffector::setBounce(NIIf bounce)
    {
        mBounce = bounce;
    }

    //-----------------------------------------------------------------------
    Vector3 DeflectorPlaneAffector::getPlanePoint(void) const
    {
        return mPlanePoint;
    }
    //-----------------------------------------------------------------------
    Vector3 DeflectorPlaneAffector::getPlaneNormal(void) const
    {
        return mPlaneNormal;
    }
    //-----------------------------------------------------------------------
    NIIf DeflectorPlaneAffector::getBounce(void) const
    {
        return mBounce;
    }

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String DeflectorPlaneAffector::CmdPlanePoint::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const DeflectorPlaneAffector *>(target)->getPlanePoint() );
    }
    void DeflectorPlaneAffector::CmdPlanePoint::doSet(void * target, const String & val)
    {
        static_cast<DeflectorPlaneAffector *>(target)->setPlanePoint(
            StringConverter::parseVector3(val));
    }
    //-----------------------------------------------------------------------
    String DeflectorPlaneAffector::CmdPlaneNormal::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const DeflectorPlaneAffector*>(target)->getPlaneNormal() );
    }
    void DeflectorPlaneAffector::CmdPlaneNormal::doSet(void* target, const String& val)
    {
        static_cast<DeflectorPlaneAffector*>(target)->setPlaneNormal(
            StringConverter::parseVector3(val));
    }
    //-----------------------------------------------------------------------
    String DeflectorPlaneAffector::CmdBounce::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const DeflectorPlaneAffector*>(target)->getBounce() );

    }
    void DeflectorPlaneAffector::CmdBounce::doSet(void* target, const String& val)
    {
        static_cast<DeflectorPlaneAffector*>(target)->setBounce(
            StringConverter::parseReal(val));
    }

}
