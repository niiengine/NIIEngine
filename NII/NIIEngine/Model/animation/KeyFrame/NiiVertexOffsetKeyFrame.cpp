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
#include "NiiVertexOffsetKeyFrame.h"

namespace NII
{
    //------------------------------------------------------------------------
    VertexOffsetKeyFrame::VertexOffsetKeyFrame(const KeyFrameTrack * own, TimeDurMS pos):
        KeyFrame(own, pos)
    {
    }
    //------------------------------------------------------------------------
    VertexOffsetKeyFrame::~VertexOffsetKeyFrame()
    {
    }
    //------------------------------------------------------------------------
    void VertexOffsetKeyFrame::add(Nui16 index, NIIf weight)
    {
        VertexOffset temp;
        temp.mTargetIndex = index;
        temp.mWeight = weight;
        mOffsets.push_back(temp);
    }
    //------------------------------------------------------------------------
    void VertexOffsetKeyFrame::set(Nui16 index, NIIf weight)
    {
        Offsets::iterator i, iend = mOffsets.end();
        for(i = mOffsets.begin(); i != iend; ++i)
        {
            if(i->mTargetIndex == index)
            {
                i->mWeight = weight;
                return;
            }
        }
        add(index, weight);
    }
    //------------------------------------------------------------------------
    void VertexOffsetKeyFrame::remove(Nui16 index)
    {
        Offsets::iterator i, iend = mOffsets.end();
        for(i = mOffsets.begin(); i != iend; ++i)
        {
            if(i->mTargetIndex == index)
            {
                mOffsets.erase(i);
                break;
            }
        }
    }
    //---------------------------------------------------------------------
    void VertexOffsetKeyFrame::removeAll()
    {
        mOffsets.clear();
    }
    //---------------------------------------------------------------------
    const VertexOffsetKeyFrame::Offsets & VertexOffsetKeyFrame::getValue() const
    {
        return mOffsets;
    }
    //---------------------------------------------------------------------
    KeyFrame * VertexOffsetKeyFrame::clone(const KeyFrameTrack * o) const
    {
        VertexOffsetKeyFrame * temp = N_new VertexOffsetKeyFrame(o, mTime);
        temp->mOffsets = mOffsets;
        return temp;
    }
    //---------------------------------------------------------------------
}