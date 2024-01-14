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
#include "NiiVertexUnitKeyFrameTrack.h"
#include "NiiVertexUnitKeyFrame.h"
#include "NiiVertexUnitFusion.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiSimdUtil.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // VertexUnitKeyFrameTrack
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    VertexUnitKeyFrameTrack::VertexUnitKeyFrameTrack(const Animation * own,
        KeyFrameTrackID id) :
            VertexKeyFrameTrack(own, id)
    {
    }
    //------------------------------------------------------------------------
    VertexUnitKeyFrameTrack::VertexUnitKeyFrameTrack(const Animation * own,
        KeyFrameTrackID id, VertexData * target, CalcMode mode) :
            VertexKeyFrameTrack(own, id, target, mode)
    {
    }
    //------------------------------------------------------------------------
    void VertexUnitKeyFrameTrack::getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const
    {
        (void)dst;
        (void)tt;
        (void)kf;
    }
    //------------------------------------------------------------------------
    bool VertexUnitKeyFrameTrack::isNromalsFusion() const
    {
        bool normals = false;
        KeyFrames::const_iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            VertexUnitKeyFrame * kf = static_cast<VertexUnitKeyFrame *>(*i);
            bool thisnorm = kf->getValue()->getUnitSize() > 12;
            if(i == mKeyFrames.begin())
                normals = thisnorm;
            else
                // Only support normals if ALL keyframes include them
                normals = normals && thisnorm;
        }
        return normals;
    }
    //------------------------------------------------------------------------
    void VertexUnitKeyFrameTrack::apply(AnimationFusion * dst, TrackTime * tt,
        NIIf weight, NIIf scale) const
    {
        apply(dst, mTarget, tt, weight);
    }
    //------------------------------------------------------------------------
    void VertexUnitKeyFrameTrack::apply(AnimationFusion * dst, VertexData * data,
        TrackTime * tt, NIIf weight) const
    {
        // Nothing to do if no keyframes or no vertex data
        if(mKeyFrames.empty() || !data)
            return;

        // Get keyframes
        KeyFrame * tsrc1, * tsrc2;
        NIIf t = get(tt, tsrc1, tsrc2);
        VertexUnitKeyFrame * src1 = static_cast<VertexUnitKeyFrame *>(tsrc1);
        VertexUnitKeyFrame * src2 = static_cast<VertexUnitKeyFrame *>(tsrc2);

        if(mMode == KeyFrameTrack::M_DEV)
        {
            // If target mode is hardware, need to bind our 2 keyframe buffers,
            // one to main pos, one to morph target texcoord
            N_assert(!data->mHWAni->mFusionData.empty(), "不能设立硬件顶点动画 !");

            // 除法线外,其他元素不能和位置元素在同个缓存中
            const VertexUnit * pos = data->get(VT_Pos);
            // Set keyframe1 data as original position
            data->attach(pos->mSource, src1->getValue());
            // Set keyframe2 data as derived
            data->attach(data->mHWAni->mFusionData[0].mBufferIndex, src2->getValue());
            // save T for use later
            data->mHWAni->mFusionData[0].mFactor = t;
        }
        else
        {
            fusion(src1->getValue(), src2->getValue(), t, data);
        }
    }
    //------------------------------------------------------------------------
    KeyFrame * VertexUnitKeyFrameTrack::createImpl(TimeDurMS time)
    {
        return N_new VertexUnitKeyFrame(this, time);
    }
    //------------------------------------------------------------------------
    bool VertexUnitKeyFrameTrack::isValid() const
    {
        return !mKeyFrames.empty();
    }
    //------------------------------------------------------------------------
    void VertexUnitKeyFrameTrack::reset()
    {

    }
    //------------------------------------------------------------------------
    void VertexUnitKeyFrameTrack::optimise()
    {
        // TODO - remove sequences of duplicate pose references?
    }
    //------------------------------------------------------------------------
    AnimationFusion * VertexUnitKeyFrameTrack::createFusion(AnimationObj * obj, 
        Animation * ani, NIIf weight, bool enable) const
    {
        return N_new VertexUnitFusion(obj, ani, 0, ani->getDur(), weight, enable);
    }
    //------------------------------------------------------------------------
    KeyFrameTrack * VertexUnitKeyFrameTrack::clone(const Animation * o) const
    {
        VertexUnitKeyFrameTrack * re = N_new VertexUnitKeyFrameTrack(o, mID, mTarget, mMode);
        re->copyImpl(this);
        return re;
    }
    //------------------------------------------------------------------------
    void VertexUnitKeyFrameTrack::fusion(const VertexBuffer * b1, const VertexBuffer * b2,
        NIIf t, VertexData * dest)
    {
        NIIf * pb1 = (NIIf *)const_cast<VertexBuffer *>(b1)->lock(Buffer::MM_READ);
        NIIf * pb2;
        if(b1 != b2)
        {
            pb2 = (NIIf *)const_cast<VertexBuffer *>(b2)->lock(Buffer::MM_READ);
        }
        else
        {
            // Same buffer - track with only one entry or time index exactly matching
            // one keyframe
            // For simplicity of main code, interpolate still but with same val
            pb2 = pb1;
        }

        const VertexUnit * destpos = dest->get(VT_Pos);
        const VertexUnit * destnorm = dest->get(VT_Normals);
        N_assert(destpos, "error");

        bool morphNormals = false;
        if(destnorm && destnorm->mSource == destpos->mSource &&
            b1->getUnitSize() == 24 && b2->getUnitSize() == 24)
            morphNormals = true;

        VertexBuffer * destBuf = dest->getAttach(destpos->mSource);
        N_assert((destpos->getSize() == destBuf->getUnitSize()
            || (morphNormals && destpos->getSize() + destnorm->getSize() == destBuf->getUnitSize())),
                "Positions (or positions & normals) must be in a buffer on their own for morphing");

        NIIf * destData = (NIIf *)destBuf->lock(Buffer::MM_WHOLE);
        SIMDUtil::getImplementation()->softwareVertexMorph(t, pb1, pb2, destData,
            b1->getUnitSize(), b2->getUnitSize(), destBuf->getUnitSize(),
                dest->mCount, morphNormals);

        destBuf->unlock();

        const_cast<VertexBuffer *>(b1)->unlock();
        if(b1 != b2)
            const_cast<VertexBuffer *>(b2)->unlock();
    }
    //------------------------------------------------------------------------
}