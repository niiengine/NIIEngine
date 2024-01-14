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
#include "OgreRingEmitter.h"
#include "OgreParticle.h"
#include "OgreException.h"
#include "OgreStringConverter.h"


/* Implements an Emitter whose emitting points all lie inside a ring.
*/

namespace Ogre {

    RingEmitter::CmdInnerX RingEmitter::msCmdInnerX;
    RingEmitter::CmdInnerY RingEmitter::msCmdInnerY;

    //-----------------------------------------------------------------------
    RingEmitter::RingEmitter(ParticleSpace* psys)
        : AreaEmitter(psys)
    {
        if(initDefaults("Ring"))
        {
            // Add custom parameters
            PropertyCmdSet * pDict = getCmdObj();

            pDict->addParameter(ParameterDef("inner_width", "Parametric value describing the proportion of the "
                "shape which is hollow.", PT_REAL), &msCmdInnerX);
            pDict->addParameter(ParameterDef("inner_height", "Parametric value describing the proportion of the "
                "shape which is hollow.", PT_REAL), &msCmdInnerY);
        }
        // default is half empty
        setInnerSize(0.5,0.5);
    }
    //-----------------------------------------------------------------------
    void RingEmitter::initImpl(Particle * pParticle)
    {
        NIIf a, b, x, y, z;

        // Call superclass
        AreaEmitter::initImpl(pParticle);
        // create a random angle from 0 .. PI*2
        Radian alpha(Math::random(0,Math::PI2X));
  
        // create two random radius values that are bigger than the inner size
        a = Math::random(mInnerSizex, 1.0);
        b = Math::random(mInnerSizey, 1.0);

        // with a and b we have defined a random ellipse inside the inner
        // ellipse and the outer circle (radius 1.0)
        // with alpha, and a and b we select a random point on this ellipse
        // and calculate it's coordinates
        x = a * Math::Sin(alpha);
        y = b * Math::Cos(alpha);
        // the height is simple -1 to 1
        z = Math::randomNegPos();     

        // scale the found point to the ring's size and move it
        // relatively to the center of the emitter point

        pParticle->mPosition = mPosition + x * mXRange + y * mYRange + z * mZRange;

        // Generate complex data by reference
        initColourImpl(pParticle);
        initDirectionImpl(pParticle);
        initSpeedImpl(pParticle);
        initTTLImpl(pParticle);
    }
    //-----------------------------------------------------------------------
    void RingEmitter::setInnerSize(NIIf x, NIIf y)
    {
        // TODO: should really throw some exception
        if ((x > 0) && (x < 1.0) &&
            (y > 0) && (y < 1.0))
            {
            mInnerSizex = x;
            mInnerSizey = y;
            }
    }
    //-----------------------------------------------------------------------
    void RingEmitter::setInnerSizeX(NIIf x)
    {
        assert(x > 0 && x < 1.0);

        mInnerSizex = x;
    }
    //-----------------------------------------------------------------------
    void RingEmitter::setInnerSizeY(NIIf y)
    {
        assert(y > 0 && y < 1.0);

        mInnerSizey = y;
    }
    //-----------------------------------------------------------------------
    NIIf RingEmitter::getInnerSizeX(void) const
    {
        return mInnerSizex;
    }
    //-----------------------------------------------------------------------
    NIIf RingEmitter::getInnerSizeY(void) const
    {
        return mInnerSizey;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String RingEmitter::CmdInnerX::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const RingEmitter*>(target)->getInnerSizeX() );
    }
    void RingEmitter::CmdInnerX::doSet(void* target, const String& val)
    {
        static_cast<RingEmitter*>(target)->setInnerSizeX(StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String RingEmitter::CmdInnerY::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const RingEmitter*>(target)->getInnerSizeY() );
    }
    void RingEmitter::CmdInnerY::doSet(void* target, const String& val)
    {
        static_cast<RingEmitter*>(target)->setInnerSizeY(StringConverter::parseReal(val));
    }

}


