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
#include "OgrePointEmitter.h"
#include "OgreParticle.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------
    PointEmitter::PointEmitter(ParticleSpace* psys)
        :SpreadParticle(psys)
    {
        mType = "Point";
        // Set up parameters
        if(createParamDictionary("PointEmitter"))
        {
    
        }
        // No custom parameters
    }
    //-----------------------------------------------------------------------
    void PointEmitter::initImpl(Particle * pParticle)
    {
        // Very simple emitter, uses default implementations with no modification

        // Call superclass
        SpreadParticle::initImpl(pParticle);

        // Point emitter emits from own mPosition
        pParticle->mPosition = mPosition;

        // Generate complex data by reference
        initColourImpl(pParticle);
        initDirectionImpl(pParticle);
        initSpeedImpl(pParticle);
        initTTLImpl(pParticle);
    }
    //-----------------------------------------------------------------------
}