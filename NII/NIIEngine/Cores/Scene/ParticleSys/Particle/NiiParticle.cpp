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
#include "NiiParticle.h"
#include "NiiParticleSpace.h"

namespace NII
{
    //-----------------------------------------------------------------------
    Particle::Particle() :
        mCtrl(0),
        mEffect(0),
        mRotation(0),
        mPosition(Vector3f::ZERO),
        mDirection(Vector3f::ZERO),
        mColour(Colour::White),
        mRemainTTL(10),
        mTotalTTL(10),
        mRotFactor(0),
        mType(T_Alone),
        mAuto(false),
        mInheritSize(true)
    {
    }
    //-----------------------------------------------------------------------
    void Particle::setRotation(const Radian & rot)
    {
        mRotation = rot;
        if(mRotation != Radian(0))
            mCtrl->onParticleRotate();
    }
    //-----------------------------------------------------------------------
    void Particle::setSize(NIIf width, NIIf height)
    {
        mInheritSize = false;
        mWidth = width;
        mHeight = height;
        mCtrl->onParticleResize();
    }
    //-----------------------------------------------------------------------
    NIIf Particle::getCtrlWidth() const
    { 
        return mCtrl->getParticleWidth(); 
    }
    //-----------------------------------------------------------------------
    NIIf Particle::getCtrlHeight() const
    { 
        return mCtrl->getParticleHeight(); 
    }
    //-----------------------------------------------------------------------
}
