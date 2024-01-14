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
#include "OgreBoxEmitter.h"
#include "OgreParticle.h"
#include "OgreException.h"
#include "OgreStringConverter.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------
    BoxEmitter::BoxEmitter(ParticleSpace* psys)
        : AreaEmitter(psys)
    {
        initDefaults("Box");
    }
    //-----------------------------------------------------------------------
    void BoxEmitter::initImpl(Particle * pParticle)
    {
        Vector3 xOff, yOff, zOff;

        // Call superclass
        SpreadParticle::initImpl(pParticle);

        xOff = Math::randomNegPos() * mXRange;
        yOff = Math::randomNegPos() * mYRange;
        zOff = Math::randomNegPos() * mZRange;

        pParticle->mPosition = mPosition + xOff + yOff + zOff;

        // Generate complex data by reference
        initColourImpl(pParticle);
        initDirectionImpl(pParticle);
        initSpeedImpl(pParticle);
        initTTLImpl(pParticle);
    }
    //-----------------------------------------------------------------------
}