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
#include "NiiNodeKeyFrame.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    //------------------------------------------------------------------------
    NodeKeyFrame::NodeKeyFrame(const KeyFrameTrack * own, TimeDurMS pos) :
        KeyFrame(own, pos),
        mTranslate(Vector3f::ZERO),
        mScale(Vector3f::UNIT),
        mRotate(Quaternion::IDENTITY)
    {
    }
    //------------------------------------------------------------------------
    NodeKeyFrame::~NodeKeyFrame()
    {
    }
    //------------------------------------------------------------------------
    void NodeKeyFrame::setTranslate(const Vector3f & trans)
    {
        mTranslate = trans;
        if(mTrack)
            mTrack->notifyImpl();
    }
    //------------------------------------------------------------------------
    const Vector3f & NodeKeyFrame::getTranslate() const
    {
        return mTranslate;
    }
    //------------------------------------------------------------------------
    void NodeKeyFrame::setScale(const Vector3f & scale)
    {
        mScale = scale;
        if (mTrack)
            mTrack->notifyImpl();
    }
    //------------------------------------------------------------------------
    const Vector3f & NodeKeyFrame::getScale() const
    {
        return mScale;
    }
    //------------------------------------------------------------------------
    void NodeKeyFrame::setRotation(const Quaternion & rot)
    {
        mRotate = rot;
        if(mTrack)
            mTrack->notifyImpl();
    }
    //------------------------------------------------------------------------
    const Quaternion & NodeKeyFrame::getRotation() const
    {
        return mRotate;
    }
    //------------------------------------------------------------------------
    KeyFrame * NodeKeyFrame::clone(const KeyFrameTrack * o) const
    {
        NodeKeyFrame * newKf = N_new NodeKeyFrame(o, mTime);
        newKf->mTranslate = mTranslate;
        newKf->mScale = mScale;
        newKf->mRotate = mRotate;
        return newKf;
    }
    //------------------------------------------------------------------------
}