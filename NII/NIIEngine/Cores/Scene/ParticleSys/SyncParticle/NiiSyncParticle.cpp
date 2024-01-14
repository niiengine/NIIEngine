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
#include "NiiSyncParticle.h"
#include "NiiParticleCtrl.h"

namespace NII
{
    //-----------------------------------------------------------------------
    SyncParticle::SyncParticle():
        mCtrlTexCoord(0),
        mPosition(Vector3f::ZERO),
        mDirection(Vector3f::ZERO),
        mColour(Colour::White),
        mRotation(0),
        mSizeValid(false),
        mTexCoordValid(false),
        mCtrl(0)
    {
    }
    //-----------------------------------------------------------------------
    SyncParticle::~SyncParticle()
    {
    }
    //-----------------------------------------------------------------------
    SyncParticle::SyncParticle(const Vector3f & position, ParticleCtrl * ctrl, const Colour & colour):
        mSizeValid(false),
        mTexCoordValid(false),
        mCtrlTexCoord(0),
        mPosition(position),
        mDirection(Vector3f::ZERO),
        mCtrl(ctrl),
        mColour(colour),
        mRotation(0)
    {
    }
    //-----------------------------------------------------------------------
    void SyncParticle::setRotation(const Radian & rot)
    {
        mRotation = rot;
        if(mRotation != Radian(0))
            mCtrl->onParticleRotate();
    }
    //-----------------------------------------------------------------------
    void SyncParticle::setSize(NIIf width, NIIf height)
    {
        mSizeValid = true;
        mWidth = width;
        mHeight = height;
        mCtrl->onParticleSize();
    }
    //-----------------------------------------------------------------------
}