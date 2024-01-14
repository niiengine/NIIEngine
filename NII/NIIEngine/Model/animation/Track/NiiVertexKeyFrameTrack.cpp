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
#include "NiiVertexKeyFrameTrack.h"

namespace NII
{
    //------------------------------------------------------------------------
    VertexKeyFrameTrack::VertexKeyFrameTrack(const Animation * parent, KeyFrameTrackID id) :
        KeyFrameTrack(parent, id),
        mTarget(0),
        mMode(KeyFrameTrack::M_HOST)
    {
    }
    //------------------------------------------------------------------------
    VertexKeyFrameTrack::VertexKeyFrameTrack(const Animation * parent, KeyFrameTrackID id,
        VertexData * target, CalcMode mode) :
            KeyFrameTrack(parent, id),
            mTarget(target),
            mMode(mode)
    {
    }
    //------------------------------------------------------------------------
    VertexKeyFrameTrack::~VertexKeyFrameTrack()
    {

    }
    //------------------------------------------------------------------------
    void VertexKeyFrameTrack::setTarget(VertexData * obj)
    {
        mTarget = obj;
    }
    //------------------------------------------------------------------------
    VertexData * VertexKeyFrameTrack::getTarget() const
    {
        return mTarget;
    }
    //------------------------------------------------------------------------
    void VertexKeyFrameTrack::setMode(KeyFrameTrack::CalcMode m)
    {
        mMode = m;
    }
    //--------------------------------------------------------------------------
    KeyFrameTrack::CalcMode VertexKeyFrameTrack::getMode() const
    {
        return mMode;
    }
    //------------------------------------------------------------------------
};