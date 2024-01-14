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
#include "OgreLinearForceAffector.h"
#include "OgreParticleSpace.h"
#include "OgreParticle.h"
#include "OgreStringConverter.h"

namespace Ogre 
{
    // Instantiate statics
    LinearForceAffector::CmdForceVector LinearForceAffector::msForceVectorCmd;
    LinearForceAffector::CmdForceApp LinearForceAffector::msForceAppCmd;
    //-----------------------------------------------------------------------
    LinearForceAffector::LinearForceAffector(ParticleSpace* psys)
        :ParticleEffect(psys)
    {
        mType = "LinearForce";

        // Default to gravity-like
        mForceApplication = FA_ADD;
        mForceVector.x = mForceVector.z = 0;
        mForceVector.y = -100;

        // Set up parameters
        if(createParamDictionary("LinearForceAffector"))
        {
            // Add extra paramaters
            PropertyCmdSet* dict = getCmdObj();
            dict->addParameter(ParameterDef("force_vector", "The vector representing the force to apply.", PT_VECTOR3),&msForceVectorCmd);
            dict->addParameter(ParameterDef("force_application", "How to apply the force vector to particles.", PT_STRING),&msForceAppCmd);
        }
    }
    //-----------------------------------------------------------------------
    void LinearForceAffector::effectImpl(ParticleList & list, TimeDurMS timeElapsed)
    {
        Particle * p;
        Vector3 scaledVector = Vector3::ZERO;

        // Precalc scaled force for optimisation
        if(mForceApplication == FA_ADD)
        {
            // Scale force by time
            scaledVector = mForceVector * timeElapsed;
        }
        
        ParticleList::const_iterator i, iend = list.end();
        for(i = list.begin(); i != iend; ++i)
        {
            p = *i;
            if (mForceApplication == FA_ADD)
            {
                p->mDirection += scaledVector;
            }
            else // FA_AVERAGE
            {
                p->mDirection = (p->mDirection + mForceVector) / 2;
            }
        }
        
    }
    //-----------------------------------------------------------------------
    void LinearForceAffector::setForceVector(const Vector3& force)
    {
        mForceVector = force;
    }
    //-----------------------------------------------------------------------
    void LinearForceAffector::setForceApplication(ForceApplication fa)
    {
        mForceApplication = fa;
    }
    //-----------------------------------------------------------------------
    Vector3 LinearForceAffector::getForceVector(void) const
    {
        return mForceVector;
    }
    //-----------------------------------------------------------------------
    LinearForceAffector::ForceApplication LinearForceAffector::getForceApplication(void) const
    {
        return mForceApplication;
    }

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String LinearForceAffector::CmdForceVector::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const LinearForceAffector*>(target)->getForceVector() );
    }
    void LinearForceAffector::CmdForceVector::doSet(void* target, const String& val)
    {
        static_cast<LinearForceAffector*>(target)->setForceVector(
            StringConverter::parseVector3(val));
    }
    //-----------------------------------------------------------------------
    String LinearForceAffector::CmdForceApp::doGet(const void* target) const
    {
        ForceApplication app = static_cast<const LinearForceAffector*>(target)->getForceApplication();
        switch(app)
        {
        case LinearForceAffector::FA_AVERAGE:
            return "average";
            break;
        case LinearForceAffector::FA_ADD:
            return "add";
            break;
        }
        // Compiler nicety
        return "";
    }
    void LinearForceAffector::CmdForceApp::doSet(void* target, const String& val)
    {
        if (val == "average")
        {
            static_cast<LinearForceAffector*>(target)->setForceApplication(FA_AVERAGE);
        }
        else if (val == "add")
        {
            static_cast<LinearForceAffector*>(target)->setForceApplication(FA_ADD);
        }
    }


}

