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
#include "NiiVertexOffsetKeyFrameTrack.h"
#include "NiiVertexOffsetKeyFrame.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiVertexOffsetFusion.h"

namespace NII
{
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // VertexOffsetKeyFrameTrack
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    VertexOffsetKeyFrameTrack::VertexOffsetKeyFrameTrack(const Animation * own,
        KeyFrameTrackID id) :
            VertexKeyFrameTrack(own, id)
    {
    }
    //--------------------------------------------------------------------------
    VertexOffsetKeyFrameTrack::VertexOffsetKeyFrameTrack(const Animation * own,
        KeyFrameTrackID id, VertexData * target, CalcMode mode) :
            VertexKeyFrameTrack(own, id, target, mode)
    {
    }
    //--------------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::getFusion(AnimationFusion *, TrackTime * tt, KeyFrame * kf) const
    {
        // Get keyframes
        KeyFrame * tsrc1, * tsrc2;
        NIIf t = get(tt, tsrc1, tsrc2);

        VertexOffsetKeyFrame * src1 = static_cast<VertexOffsetKeyFrame *>(tsrc1);
        VertexOffsetKeyFrame * src2 = static_cast<VertexOffsetKeyFrame *>(tsrc2);
        VertexOffsetKeyFrame * kfout = static_cast<VertexOffsetKeyFrame *>(kf);

        // For each pose reference in key 1, we need to locate the entry in
        // key 2 and interpolate the weight
        const VertexOffsetKeyFrame::Offsets & pose1 = src1->getValue();
        const VertexOffsetKeyFrame::Offsets & pose2 = src2->getValue();
        VertexOffsetKeyFrame::Offsets::const_iterator p11, p11end = pose1.end();
        for(p11 = pose1.begin(); p11 != p11end; ++p11)
        {
            NIIf startInfluence = p11->mWeight;
            NIIf endInfluence = 0;
            // Search for entry in keyframe 2 list (if not there, will be 0)
            VertexOffsetKeyFrame::Offsets::const_iterator p2, p2end = pose2.end();
            for(p2 = pose2.begin(); p2 != p2end; ++p2)
            {
                if(p11->mTargetIndex == p2->mTargetIndex)
                {
                    endInfluence = p2->mWeight;
                    break;
                }
            }
            // Interpolate weight
            NIIf lweight = startInfluence + t * (endInfluence - startInfluence);

            kfout->add(p11->mTargetIndex, lweight);
        }
        // Now deal with any poses in key 2 which are not in key 1
        VertexOffsetKeyFrame::Offsets::const_iterator p2, p2end = pose2.end();
        for(p2 = pose2.begin(); p2 != p2end; ++p2)
        {
            bool found = false;
            VertexOffsetKeyFrame::Offsets::const_iterator p12, p1end = pose1.end();
            for(p12 = pose1.begin(); p12 != p1end; ++p12)
            {
                if(p12->mTargetIndex == p2->mTargetIndex)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                // Need to apply this pose too, scaled from 0 start
                NIIf lweight = t * p2->mWeight;

                kfout->add(p2->mTargetIndex, lweight);
            }
        } // key 2 iteration
    }
    //--------------------------------------------------------------------------
    bool VertexOffsetKeyFrameTrack::isNromalsFusion() const
    {
        return false;
    }
    //--------------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::apply(AnimationFusion * dst, TrackTime * tt,
        NIIf weight, NIIf scale) const
    {
        apply(dst, mTarget, tt, weight);
    }
    //--------------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::apply(AnimationFusion * dst, VertexData * data,
        TrackTime * tt, NIIf weight) const
    {
        if(mKeyFrames.empty() || !data)
            return;

        const VertexOffsetList & poseList = static_cast<VertexOffsetFusion *>(dst)->getOffsetList();

        // Get keyframes
        KeyFrame *tsrc1, *tsrc2;
        NIIf t = get(tt, tsrc1, tsrc2);
        VertexOffsetKeyFrame * src1 = static_cast<VertexOffsetKeyFrame *>(tsrc1);
        VertexOffsetKeyFrame * src2 = static_cast<VertexOffsetKeyFrame *>(tsrc2);
        // For each pose reference in key 1, we need to locate the entry in
        // key 2 and interpolate the weight
        const VertexOffsetKeyFrame::Offsets & pose1 = src1->getValue();
        const VertexOffsetKeyFrame::Offsets & pose2 = src2->getValue();
        VertexOffsetKeyFrame::Offsets::const_iterator p11, p11end = pose1.end();
        for(p11 = pose1.begin(); p11 != p11end; ++p11)
        {
            NIIf startInfluence = p11->mWeight;
            NIIf endInfluence = 0;
            // Search for entry in keyframe 2 list (if not there, will be 0)
            VertexOffsetKeyFrame::Offsets::const_iterator p2, p2end = pose2.end();
            for(p2 = pose2.begin(); p2 != p2end; ++p2)
            {
                if(p11->mTargetIndex == p2->mTargetIndex)
                {
                    endInfluence = p2->mWeight;
                    break;
                }
            }
            // Interpolate weight
            NIIf lweight = startInfluence + t * (endInfluence - startInfluence);
            // Scale by animation weight
            lweight = weight * lweight;
            // Get pose
            N_assert (p11->mTargetIndex < poseList.size(), "error");
            const VertexOffset * pose = poseList[p11->mTargetIndex];

            if(mMode == KeyFrameTrack::M_DEV)
            {
                // Hardware
                // If target mode is hardware, need to bind our pose buffer
                // to a target texcoord
                N_assert(!data->mHWAni->mFusionData.empty(), "没有顶点动画数据!");

                // Set pose target as required
                NCount hwIndex = data->mHWAni->mFusionIndex++;
                // If we try to use too many poses, ignore extras
                if(hwIndex < data->mHWAni->mFusionData.size())
                {
                    HardwareAnimation::Fusion & animData = data->mHWAni->mFusionData[hwIndex];
                    data->attach(animData.mBufferIndex, pose->getFusion(data));
                    // save final weight in parametric
                    animData.mFactor = lweight;
                }
            }
            else
            {
                fusion(data, pose->getVertexOffsets(), pose->getNormals(), lweight);
            }
        }
        // 现在处理帧2中不存在于帧1的姿态
        VertexOffsetKeyFrame::Offsets::const_iterator p2, p2end = pose2.end();
        for(p2 = pose2.begin(); p2 != p2end; ++p2)
        {
            bool found = false;
            VertexOffsetKeyFrame::Offsets::const_iterator p12, p12end = pose1.end();
            for(p12 = pose1.begin(); p12 != p12end; ++p12)
            {
                if(p12->mTargetIndex == p2->mTargetIndex)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                // Need to apply this pose too, scaled from 0 start
                NIIf lweight = t * p2->mWeight;
                // Scale by animation weight
                lweight = weight * lweight;
                // Get pose
                N_assert (p2->mTargetIndex <= poseList.size(), "error");
                const VertexOffset * pose = poseList[p2->mTargetIndex];

                if(mMode == KeyFrameTrack::M_DEV)
                {
                    // Hardware
                    // If target mode is hardware, need to bind our pose buffer
                    // to a target texcoord
                    N_assert(!data->mHWAni->mFusionData.empty(), "没有有顶点动画数据!");

                    // Set pose target as required
                    NCount hwIndex = data->mHWAni->mFusionIndex++;
                    // If we try to use too many poses, ignore extras
                    if(hwIndex < data->mHWAni->mFusionData.size())
                    {
                        HardwareAnimation::Fusion & animData = data->mHWAni->mFusionData[hwIndex];
                        data->attach(animData.mBufferIndex, pose->getFusion(data));
                        // save final weight in parametric
                        animData.mFactor = lweight;
                    }
                }
                else
                {
                    fusion(data, pose->getVertexOffsets(), pose->getNormals(), lweight);
                }
            }
        } // key 2 iteration
    }
    //--------------------------------------------------------------------------
    KeyFrame * VertexOffsetKeyFrameTrack::createImpl(TimeDurMS time)
    {
        return N_new VertexOffsetKeyFrame(this, time);
    }
    //--------------------------------------------------------------------------
    bool VertexOffsetKeyFrameTrack::isValid() const
    {
        KeyFrames::const_iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            const VertexOffsetKeyFrame * kf = static_cast<const VertexOffsetKeyFrame *>(*i);
            VertexOffsetKeyFrame::Offsets::const_iterator i, iend = kf->getValue().end();
            for(i = kf->getValue().begin(); i != iend; ++i)
            {
                if(i->mWeight > 0.0f)
                    return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::reset()
    {
    }
    //---------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::optimise()
    {
        // TODO - remove sequences of duplicate pose references?
    }
    //---------------------------------------------------------------------
    AnimationFusion * VertexOffsetKeyFrameTrack::createFusion(AnimationObj * obj,
        Animation * ani, NIIf weight, bool enable) const
    {
        return N_new VertexOffsetFusion(obj, ani, 0, ani->getDur(), weight, enable);
    }
    //---------------------------------------------------------------------
    KeyFrameTrack * VertexOffsetKeyFrameTrack::clone(const Animation * o) const
    {
        VertexOffsetKeyFrameTrack * re = N_new VertexOffsetKeyFrameTrack(o,
            mID, mTarget, mMode);
        re->copyImpl(this);
        return re;
    }
    //---------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::setOrigin(const KeyFrameTrack * org, TrackTime * time)
    {
        VertexOffsetKeyFrame base(org, time->getTimePos());
        org->getFusion(0, time, &base);

        KeyFrames::iterator z, zend = mKeyFrames.end();
        for(z = mKeyFrames.begin(); z != zend; ++z)
        {
            VertexOffsetKeyFrame * kf = static_cast<VertexOffsetKeyFrame *>(*z);
            VertexOffsetKeyFrame::Offsets::iterator i, iend = kf->mOffsets.end();
            for(i = kf->mOffsets.begin(); i != iend; ++i)
            {
                VertexOffsetKeyFrame::VertexOffset & src = *i;
                VertexOffsetKeyFrame::Offsets::const_iterator j, jend = base.getValue().end();
                for(j = base.getValue().begin(); j != jend; ++j)
                {
                    const VertexOffsetKeyFrame::VertexOffset & ori = *j;
                    if(ori.mTargetIndex == src.mTargetIndex)
                    {
                        src.mWeight -= ori.mWeight;
                        break;
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::fusion(VertexData * dest,
        const VertexOffset::OffsetMap & vertPosOft,
            const VertexOffset::NormalsMap & vertNorls, NIIf weight)
    {
        // Do nothing if no weight
        if(weight == 0.0f)
            return;

        const VertexUnit * destpos = dest->get(VT_Pos);
        const VertexUnit * destnorm = dest->get(VT_Normals);

        N_assert(destpos, "error");
        // Support normals if they're in the same buffer as positions and pose includes them
        bool normals = destnorm && !vertNorls.empty() && destpos->mSource == destnorm->mSource;

        VertexBuffer * destBuf = dest->getAttach(destpos->mSource);
        NCount elemsPerVertex = destBuf->getUnitSize() / sizeof(NIIf);

        // Have to lock in normal mode since this is incremental
        NIIf * pBase = (NIIf *)destBuf->lock(Buffer::MM_READ | Buffer::MM_WRITE);

        VertexOffset::OffsetMap::const_iterator j, jend = vertPosOft.end();
        for(j = vertPosOft.begin(); j != jend; ++j)
        {
            // Adjust pointer
            NIIf * destbuf = pBase + j->first * elemsPerVertex;
            *destbuf = *destbuf + (j->second.x * weight);
            ++destbuf;
            *destbuf = *destbuf + (j->second.y * weight);
            ++destbuf;
            *destbuf = *destbuf + (j->second.z * weight);
            ++destbuf;
        }

        if(normals)
        {
            NIIf * pNormBase;
            destnorm->getData((Nui8 *)pBase, pNormBase);
            VertexOffset::NormalsMap::const_iterator i, iend = vertNorls.end();
            for(i = vertNorls.begin(); i != iend; ++i)
            {
                // Adjust pointer
                NIIf * destbuf = pNormBase + i->first * elemsPerVertex;

                *destbuf = *destbuf + (i->second.x * weight);
                ++destbuf;
                *destbuf = *destbuf + (i->second.y * weight);
                ++destbuf;
                *destbuf = *destbuf + (i->second.z * weight);
                ++destbuf;
            }
        }

        destBuf->unlock();
    }
    //---------------------------------------------------------------------
    void VertexOffsetKeyFrameTrack::memcpy(const VertexData * src, VertexData * dest, bool normal)
    {
        // First time through for a piece of pose animated vertex data
        // We need to copy the original position values to the temp accumulator
        const VertexUnit * origelem = src->get(VT_Pos);
        const VertexUnit * destelem = dest->get(VT_Pos);
        VertexBuffer * origBuffer = src->getAttach(origelem->mSource);
        VertexBuffer * destBuffer = dest->getAttach(destelem->mSource);
        destBuffer->write(origBuffer, 0, 0, destBuffer->getSize());

        // If normals are included in animation, we want to reset the normals to zero
        if(normal)
        {
            const VertexUnit * normElem = dest->get(VT_Normals);

            if(normElem)
            {
                VertexBuffer * buf = dest->getAttach(normElem->mSource);

                Nui8 * pBase = (Nui8 *)buf->lock(Buffer::MM_READ | Buffer::MM_WRITE);
                pBase += dest->mOffset * buf->getUnitSize();
                for(NCount v = 0; v < dest->mCount; ++v)
                {
                    NIIf * pNorm;
                    normElem->getData(pBase, pNorm);
                    *pNorm++ = 0.0f;
                    *pNorm++ = 0.0f;
                    *pNorm++ = 0.0f;

                    pBase += buf->getUnitSize();
                }
                buf->unlock();
            }
        }
    }
    //---------------------------------------------------------------------
}