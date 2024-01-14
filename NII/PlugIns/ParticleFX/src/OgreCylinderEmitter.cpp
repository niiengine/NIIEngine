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
#include "OgreCylinderEmitter.h"
#include "OgreParticle.h"
#include "OgreQuaternion.h"
#include "OgreException.h"
#include "OgreStringConverter.h"


/* Implements an Emitter whose emitting points all lie inside a cylinder.
*/

namespace Ogre 
{
    //-----------------------------------------------------------------------
    CylinderEmitter::CylinderEmitter(ParticleSpace* psys)
        : AreaEmitter(psys)
    {
        initDefaults("Cylinder");
    }
    //-----------------------------------------------------------------------
    void CylinderEmitter::initImpl(Particle* pParticle)
    {
        NIIf x, y, z;

        // Call superclass
        AreaEmitter::initImpl(pParticle);

        // First we create a random point inside a bounding cylinder with a
        // radius and height of 1 (this is easy to do). The distance of the
        // point from 0,0,0 must be <= 1 (== 1 means on the surface and we
        // count this as inside, too).

        while (true)
        {
/* ClearSpace not yet implemeted

*/
                // three random values for one random point in 3D space
                x = Math::randomNegPos();
                y = Math::randomNegPos();
                z = Math::randomNegPos();

                // the distance of x,y from 0,0 is sqrt(x*x+y*y), but
                // as usual we can omit the sqrt(), since sqrt(1) == 1 and we
                // use the 1 as boundary. z is not taken into account, since
                // all values in the z-direction are inside the cylinder:
                if(x * x + y * y <= 1)
                {
                        break;          // found one valid point inside
                }
        }       

        // scale the found point to the cylinder's size and move it
        // relatively to the center of the emitter point

        pParticle->mPosition = mPosition + x * mXRange + y * mYRange + z * mZRange;

        // Generate complex data by reference
        initColourImpl(pParticle);
        initDirectionImpl(pParticle);
        initSpeedImpl(pParticle);
        initTTLImpl(pParticle);
    }
}