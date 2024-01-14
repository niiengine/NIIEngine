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

#include "NiiPreProcess.h"
#include "NiiParticleEffect.h"

namespace NII
{
    //------------------------------------------------------------------------
    ParticleEffect::ParticleEffect(ParticleSpace * space, LangID lid) :
        PropertyCmdObj(N_CmdObj_ParticleEffect, lid),
        mParent(space)
    {
    }
    //------------------------------------------------------------------------
    ParticleEffect::~ParticleEffect()
    {
    }
    //------------------------------------------------------------------------
    void ParticleEffect::initImpl(Particle *)
    {
    }
    //------------------------------------------------------------------------
    void ParticleEffect::effectImpl(Particle *, TimeDurMS)
    {
    
    }
    //------------------------------------------------------------------------
    void ParticleEffect::effectImpl(ParticleList &, TimeDurMS)
    {
    }
    //------------------------------------------------------------------------
    void ParticleEffect::onPos(const Vector3f &)
    {
    
    }
    //------------------------------------------------------------------------
    void ParticleEffect::onSize(const Vector2f &)
    {
    }
    //------------------------------------------------------------------------
    void ParticleEffect::onRotation(const Radian &)
    {
    }
    //------------------------------------------------------------------------
}