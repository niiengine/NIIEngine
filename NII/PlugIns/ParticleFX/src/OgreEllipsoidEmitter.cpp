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
#include "OgreEllipsoidEmitter.h"
#include "OgreParticle.h"
#include "OgreException.h"
#include "OgreStringConverter.h"


/* Implements an Emitter whose emitting points all lie inside an ellipsoid.
   See <http://mathworld.wolfram.com/Ellipsoid.html> for mathematical details.

  If the lengths of two axes of an ellipsoid are the same, the figure is
  called a 'spheroid' (depending on whether c < a or c > a, an 'oblate
  spheroid' or 'prolate spheroid', respectively), and if all three are the
  same, it is a 'sphere' (ball).
*/

namespace Ogre 
{
    //-----------------------------------------------------------------------
    EllipsoidEmitter::EllipsoidEmitter(ParticleSpace * psys) : 
        AreaEmitter(psys)
    {
        initDefaults("Ellipsoid");
    }
    //-----------------------------------------------------------------------
    void EllipsoidEmitter::initImpl(Particle * pParticle)
    {
        NIIf x, y, z;

        // Call superclass
        AreaEmitter::initImpl(pParticle);
        // First we create a random point inside a bounding sphere with a
        // radius of 1 (this is easy to do). The distance of the point from
        // 0,0,0 must be <= 1 (== 1 means on the surface and we count this as
        // inside, too).

        while(true)
        {
            // three random values for one random point in 3D space

            x = Math::randomNegPos();
            y = Math::randomNegPos();
            z = Math::randomNegPos();

            // the distance of x,y,z from 0,0,0 is sqrt(x*x+y*y+z*z), but
            // as usual we can omit the sqrt(), since sqrt(1) == 1 and we
            // use the 1 as boundary:
            if(x*x + y*y + z*z <= 1)
            {
                break;          // found one valid point inside
            }
        }       

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
}