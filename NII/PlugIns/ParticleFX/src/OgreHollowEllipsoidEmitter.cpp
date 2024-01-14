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
// Original author: Tels <http://bloodgate.com>, released as public domain
#include "OgreHollowEllipsoidEmitter.h"
#include "OgreParticle.h"
#include "OgreException.h"
#include "OgreStringConverter.h"
#include "OgreMath.h"

/* Implements an Emitter whose emitting points all lie inside an ellipsoid.
   See <http://mathworld.wolfram.com/Ellipsoid.html> for mathematical details.

  If the lengths of two axes of an ellipsoid are the same, the figure is
  called a 'spheroid' (depending on whether c < a or c > a, an 'oblate
  spheroid' or 'prolate spheroid', respectively), and if all three are the
  same, it is a 'sphere' (ball).
*/

namespace Ogre 
{
    HollowEllipsoidEmitter::CmdInnerX HollowEllipsoidEmitter::msCmdInnerX;
    HollowEllipsoidEmitter::CmdInnerY HollowEllipsoidEmitter::msCmdInnerY;
    HollowEllipsoidEmitter::CmdInnerZ HollowEllipsoidEmitter::msCmdInnerZ;
    //-----------------------------------------------------------------------
    HollowEllipsoidEmitter::HollowEllipsoidEmitter(ParticleSpace * psys) : 
        EllipsoidEmitter(psys)
    {
        if(initDefaults("HollowEllipsoid"))
        {
            // Add custom parameters
            PropertyCmdSet * pDict = getCmdObj();

            pDict->addParameter(ParameterDef("inner_width", "Parametric value describing the proportion of the "
                "shape which is hollow.", PT_REAL), &msCmdInnerX);
            pDict->addParameter(ParameterDef("inner_height", "Parametric value describing the proportion of the "
                "shape which is hollow.", PT_REAL), &msCmdInnerY);
            pDict->addParameter(ParameterDef("inner_depth", "Parametric value describing the proportion of the "
                "shape which is hollow.", PT_REAL), &msCmdInnerZ);
        }
        // default is half empty
        setInnerSize(0.5,0.5,0.5);
    }
    //-----------------------------------------------------------------------
    void HollowEllipsoidEmitter::initImpl(Particle * pParticle)
    {
        NIIf a, b, c, x, y, z;

        // Init dimensions
        pParticle->inheritSize(true);

        // create two random angles alpha and beta
        // with these two angles, we are able to select any point on an
        // ellipsoid's surface
        Radian alpha(Math::random(0, Math::PI2X));
        Radian beta(Math::random(0, Math::PI));

        // create three random radius values that are bigger than the inner
        // size, but smaller/equal than/to the outer size 1.0 (inner size is
        // between 0 and 1)
        a = Math::random(mInnerSize.x, 1.0);
        b = Math::random(mInnerSize.y, 1.0);
        c = Math::random(mInnerSize.z, 1.0);

        // with a,b,c we have defined a random ellipsoid between the inner
        // ellipsoid and the outer sphere (radius 1.0)
        // with alpha and beta we select on point on this random ellipsoid
        // and calculate the 3D coordinates of this point
		NIIf sinbeta ( Math::Sin(beta) );
        x = a * Math::Cos(alpha) * sinbeta;
        y = b * Math::Sin(alpha) * sinbeta;
        z = c * Math::Cos(beta);

        // scale the found point to the ellipsoid's size and move it
        // relatively to the center of the emitter point

        pParticle->mPosition = mPosition + 
         + x * mXRange + y * mYRange + z * mZRange;

        // Generate complex data by reference
        initColourImpl(pParticle);
        initDirectionImpl(pParticle);
        initSpeedImpl(pParticle);
        initTTLImpl(pParticle);
    }
    //-----------------------------------------------------------------------
    void HollowEllipsoidEmitter::setInnerSize(NIIf x, NIIf y, NIIf z)
    {
        assert((x > 0) && (x < 1.0) &&
            (y > 0) && (y < 1.0) &&
            (z > 0) && (z < 1.0));

        mInnerSize.x = x;
        mInnerSize.y = y;
        mInnerSize.z = z;
    }
    //-----------------------------------------------------------------------
    void HollowEllipsoidEmitter::setInnerSizeX(NIIf x)
    {
        assert(x > 0 && x < 1.0);

        mInnerSize.x = x;
    }
    //-----------------------------------------------------------------------
    void HollowEllipsoidEmitter::setInnerSizeY(NIIf y)
    {
        assert(y > 0 && y < 1.0);

        mInnerSize.y = y;
    }
    //-----------------------------------------------------------------------
    void HollowEllipsoidEmitter::setInnerSizeZ(NIIf z)
    {
        assert(z > 0 && z < 1.0);

        mInnerSize.z = z;
    }
    //-----------------------------------------------------------------------
    NIIf HollowEllipsoidEmitter::getInnerSizeX(void) const
    {
        return mInnerSize.x;
    }
    //-----------------------------------------------------------------------
    NIIf HollowEllipsoidEmitter::getInnerSizeY(void) const
    {
        return mInnerSize.y;
    }
    //-----------------------------------------------------------------------
    NIIf HollowEllipsoidEmitter::getInnerSizeZ(void) const
    {
        return mInnerSize.z;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String HollowEllipsoidEmitter::CmdInnerX::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const HollowEllipsoidEmitter*>(target)->getInnerSizeX() );
    }
    void HollowEllipsoidEmitter::CmdInnerX::doSet(void* target, const String& val)
    {
        static_cast<HollowEllipsoidEmitter*>(target)->setInnerSizeX(StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String HollowEllipsoidEmitter::CmdInnerY::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const HollowEllipsoidEmitter*>(target)->getInnerSizeY() );
    }
    void HollowEllipsoidEmitter::CmdInnerY::doSet(void* target, const String& val)
    {
        static_cast<HollowEllipsoidEmitter*>(target)->setInnerSizeY(StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String HollowEllipsoidEmitter::CmdInnerZ::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const HollowEllipsoidEmitter*>(target)->getInnerSizeZ() );
    }
    void HollowEllipsoidEmitter::CmdInnerZ::doSet(void* target, const String& val)
    {
        static_cast<HollowEllipsoidEmitter*>(target)->setInnerSizeZ(StringConverter::parseReal(val));
    }


}


