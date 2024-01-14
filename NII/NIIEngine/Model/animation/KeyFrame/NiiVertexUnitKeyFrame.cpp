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
#include "NiiVertexUnitKeyFrame.h"

namespace NII
{
    //------------------------------------------------------------------------
    VertexUnitKeyFrame::VertexUnitKeyFrame(const KeyFrameTrack * parent, TimeDurMS pos) :
        KeyFrame(parent, pos)
    {
    }
    //------------------------------------------------------------------------
    VertexUnitKeyFrame::~VertexUnitKeyFrame()
    {
    }
    //------------------------------------------------------------------------
    void VertexUnitKeyFrame::setValue(VertexBuffer * buf)
    {
        mBuffer = buf;
    }
    //------------------------------------------------------------------------
    VertexBuffer * VertexUnitKeyFrame::getValue() const
    {
        return mBuffer;
    }
    //------------------------------------------------------------------------
    KeyFrame * VertexUnitKeyFrame::clone(const KeyFrameTrack * o) const
    {
        VertexUnitKeyFrame * re = N_new VertexUnitKeyFrame(o, mTime);
        re->mBuffer = mBuffer;
        return re;
    }
    //------------------------------------------------------------------------
}