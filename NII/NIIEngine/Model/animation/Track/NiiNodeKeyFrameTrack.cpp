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
#include "NiiNodeKeyFrameTrack.h"
#include "NiiLinearSpline.h"
#include "NiiRotationSpline.h"
#include "NiiNodeFusion.h"
#include "NiiNodeKeyFrame.h"
#include "NiiSimdUtil.h"

namespace NII
{
    //------------------------------------------------------------------------
    struct NodeKeyFrameSpline
    {
        LinearSpline mPos;
        LinearSpline mScale;
        RotationSpline mRot;
        bool mSplineValid;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // NodeKeyFrameTrack
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    NodeKeyFrameTrack::NodeKeyFrameTrack(const Animation * o, KeyFrameTrackID id) :
        KeyFrameTrack(o, id),
        mTarget(0),
        mShortPath(true)
    {
        mSplines = N_new_t(NodeKeyFrameSpline);
        mSplines->mSplineValid = true;
    }
    //------------------------------------------------------------------------
    NodeKeyFrameTrack::NodeKeyFrameTrack(const Animation * o, KeyFrameTrackID id, PosNode * target) :
        KeyFrameTrack(o, id),
        mTarget(target),
        mShortPath(true)
    {
        mSplines = N_new_t(NodeKeyFrameSpline);
        mSplines->mSplineValid = true;
    }
    //---------------------------------------------------------------------
    NodeKeyFrameTrack::~NodeKeyFrameTrack()
    {
        N_delete_t(mSplines, NodeKeyFrameSpline);
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const
    {
        NodeKeyFrame * kret = static_cast<NodeKeyFrame *>(kf);
        KeyFrame * tk1, * tk2;

        Nidx leftkf = tt->getLocalKeyIndex();
        NIIf t = get(tt, tk1, tk2);
        NodeKeyFrame * k1 = static_cast<NodeKeyFrame *>(tk1);
        NodeKeyFrame * k2 = static_cast<NodeKeyFrame *>(tk2);

        if(t == 0.0)
        {
            // Just use k1
            kret->setRotation(k1->getRotation());
            kret->setTranslate(k1->getTranslate());
            kret->setScale(k1->getScale());
        }
        else
        {
            NodeFusion * obj = static_cast<NodeFusion *>(dst);
            NodeFusion::Mode im = obj ? obj->getMode() : NodeFusion::M_SPLINE;
            NodeFusion::RotationMode rim = obj ? obj->getRotationMode() : NodeFusion::RM_SPHERICAL;

            switch(im)
            {
            case NodeFusion::M_LINEAR:
            {
                // rotate
                if (rim == NodeFusion::RM_LINEAR)
                {
                    kret->setRotation(Quaternion::nlerp(t, k1->getRotation(), k2->getRotation(), mShortPath));
                }
                else // NodeFusion::RM_SPHERICAL
                {
                    kret->setRotation(Quaternion::slerp(t, k1->getRotation(), k2->getRotation(), mShortPath));
                }

                // translate
                const Vector3f & oft = k1->getTranslate();
                kret->setTranslate(oft + ((k2->getTranslate() - oft) * t));

                // Scale
                const Vector3f & base = k1->getScale();
                kret->setScale(base + ((k2->getScale() - base) * t));
            }
                break;
            case NodeFusion::M_SPLINE:
            {
                // rotate
                kret->setRotation(mSplines->mRot.interpolate(leftkf, t, mShortPath));
                // translate
                kret->setTranslate(mSplines->mPos.interpolate(leftkf, t));
                // scale
                kret->setScale(mSplines->mScale.interpolate(leftkf, t));

            }
                break;
            }
        }
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::apply(AnimationFusion * dst, TrackTime * tt, NIIf weight, NIIf scale) const
    {
        apply(dst, mTarget, tt, weight, scale);
    }
    //---------------------------------------------------------------------
    PosNode * NodeKeyFrameTrack::getTarget(void) const
    {
        return mTarget;
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::setTarget(PosNode * node)
    {
        mTarget = node;
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::apply(AnimationFusion * dst, PosNode * com, 
        TrackTime * tt, NIIf weight, NIIf scl) const
    {
        // Nothing to do if no keyframes or zero weight or no node
        if(mKeyFrames.empty() || !weight || !com)
            return;

        NodeKeyFrame kf(0, tt->getTimePos());
        getFusion(dst, tt, &kf);

        // add to existing. Weights are not relative, but treated as absolute multipliers for the animation
        Vector3f translate = kf.getTranslate() * weight * scl;
        com->translate(translate);

        // interpolate between no-rotation and full rotation, to point 'weight', so 0 = no rotate, 1 = full
        Quaternion rotate;
        NodeFusion::RotationMode rim = static_cast<NodeFusion *>(dst)->getRotationMode();
        if(rim == NodeFusion::RM_LINEAR)
        {
            rotate = Quaternion::nlerp(weight, Quaternion::IDENTITY, kf.getRotation(), mShortPath);
        }
        else //if (rim == NodeFusion::RM_SPHERICAL)
        {
            rotate = Quaternion::slerp(weight, Quaternion::IDENTITY, kf.getRotation(), mShortPath);
        }
        com->rotate(rotate);

        Vector3f scale = kf.getScale();
        // Not sure how to modify scale for cumulative anims... leave it alone
        //scale = ((Vector3::UNIT - kf.getScale()) * weight) + Vector3::UNIT;
        if(scale != Vector3f::UNIT)
        {
            if(scl != 1.0f)
                scale = Vector3f::UNIT + (scale - Vector3f::UNIT) * scl;
            else if(weight != 1.0f)
                scale = Vector3f::UNIT + (scale - Vector3f::UNIT) * weight;
        }
        com->scale(scale);
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::buildSpline() const
    {
        if(!mSplines->mSplineValid)
        {
            mSplines->mPos.clear();
            mSplines->mRot.clear();
            mSplines->mScale.clear();

            KeyFrames::const_iterator i, iend = mKeyFrames.end();
            for(i = mKeyFrames.begin(); i != iend; ++i)
            {
                NodeKeyFrame * kf = static_cast<NodeKeyFrame*>(*i);
                mSplines->mPos.add(kf->getTranslate());
                mSplines->mRot.add(kf->getRotation());
                mSplines->mScale.add(kf->getScale());
            }

            mSplines->mPos.rebuild();
            mSplines->mRot.rebuild();
            mSplines->mScale.rebuild();

            mSplines->mSplineValid = true;
        }
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::setShortPath(bool b)
    {
        mShortPath = b ;
    }
    //---------------------------------------------------------------------
    bool NodeKeyFrameTrack::getShortPath() const
    {
        return mShortPath ;
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::notifyImpl() const
    {
        mSplines->mSplineValid = false;
    }
    //---------------------------------------------------------------------
    bool NodeKeyFrameTrack::isValid() const
    {
        KeyFrames::const_iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            // look for keyframes which have any component which is non-zero
            // Since exporters can be a little inaccurate sometimes we use a
            // tolerance value rather than looking for nothing
            NodeKeyFrame * kf = static_cast<NodeKeyFrame *>(*i);
            const Vector3f & trans = kf->getTranslate();
            const Vector3f & scale = kf->getScale();
            Vector3f axis;
            Radian angle;
            kf->getRotation().to(angle, axis);
            NIIf tolerance = 1e-3f;
            if(!trans.equal(Vector3f::ZERO, tolerance) ||
                !scale.equal(Vector3f::UNIT, tolerance) ||
                    !Math::isEqual(angle.getValue(), 0.0f, tolerance))
            {
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::optimise()
    {
        // Eliminate duplicate keyframes from 2nd to penultimate keyframe
        // NB only eliminate middle keys from sequences of 5+ identical keyframes
        // since we need to preserve the boundary keys in place, and we need
        // 2 at each end to preserve tangents for spline interpolation
        Vector3f lasttrans = Vector3f::ZERO;
        Vector3f lastscale = Vector3f::ZERO;
        Quaternion lastori;
        KeyFrames::iterator i = mKeyFrames.begin();
        Radian quatTolerance(1e-3f);
        list<Nui16>::type removeList;
        Nui16 k = 0;
        Nui16 dropcnt = 0;
        for(; i != mKeyFrames.end(); ++i, ++k)
        {
            NodeKeyFrame * kf = static_cast<NodeKeyFrame *>(*i);
            const Vector3f & newtrans = kf->getTranslate();
            const Vector3f & newscale = kf->getScale();
            const Quaternion & newori = kf->getRotation();
            // Ignore first keyframe; now include the last keyframe as we eliminate
            // only k-2 in a group of 5 to ensure we only eliminate middle keys
            if(i != mKeyFrames.begin() &&
                newtrans.equal(lasttrans) &&
                newscale.equal(lastscale) &&
                newori.equals(lastori, quatTolerance))
            {
                ++dropcnt;

                // 4 indicates this is the 5th duplicate keyframe
                if(dropcnt == 4)
                {
                    // remove the 'middle' keyframe
                    removeList.push_back(k - 2);
                    --dropcnt;
                }
            }
            else
            {
                // reset
                dropcnt = 0;
                lasttrans = newtrans;
                lastscale = newscale;
                lastori = newori;
            }
        }

        // Now remove keyframes, in reverse order to avoid index revocation
        list<Nui16>::type::reverse_iterator r, rend = removeList.rend();
        for(r = removeList.rbegin(); r != rend; ++r)
        {
            destroy(*r);
        }
    }
    //--------------------------------------------------------------------------
    KeyFrame * NodeKeyFrameTrack::createImpl(TimeDurMS time)
    {
        return N_new NodeKeyFrame(this, time);
    }
    //--------------------------------------------------------------------------
    AnimationFusion * NodeKeyFrameTrack::createFusion(AnimationObj * obj, 
        Animation * ani, NIIf weight, bool enable) const
    {
        return N_new NodeFusion(obj, ani, 0, ani->getDur());
    }
    //--------------------------------------------------------------------------
    KeyFrameTrack * NodeKeyFrameTrack::clone(const Animation * o) const
    {
        NodeKeyFrameTrack * re = N_new NodeKeyFrameTrack(o, mID, mTarget);
        re->mShortPath = mShortPath;
        re->copyImpl(this);
        return re;
    }
    //--------------------------------------------------------------------------
    void NodeKeyFrameTrack::setOrigin(const KeyFrameTrack * org, TrackTime * time)
    {
        NodeKeyFrame base(org, time->getTimePos());
        org->getFusion(0, time, &base);

        KeyFrames::iterator i, iend = mKeyFrames.end();
        for(i = mKeyFrames.begin(); i != iend; ++i)
        {
            NodeKeyFrame * kf = static_cast<NodeKeyFrame *>(*i);
            kf->setTranslate(kf->getTranslate() - base.getTranslate());
            kf->setRotation(base.getRotation().inverse() * kf->getRotation());
            kf->setScale(kf->getScale() * (Vector3f::UNIT / base.getScale()));
        }
    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::fusion(const Vector3f * src, Vector3f * dst,
        const Matrix4f * const * space, NCount count)
    {

    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::fusion(const Vector3f * src, const Vector3f * srcnormal,
        Vector3f * dst, Vector3f * dstnormal, const Matrix4f * const * space, NCount count)
    {

    }
    //---------------------------------------------------------------------
    void NodeKeyFrameTrack::fusion(const VertexData * src, VertexData * dest,
        const Matrix4f * const * space, NCount count, bool normal)
    {
        NIIf * pSrcPos = 0;
        NIIf * pSrcNorm = 0;
        NIIf * pDestPos = 0;
        NIIf * pDestNorm = 0;
        NIIf * pBlendWeight = 0;
        Nui8 * pBlendIdx = 0;
        NCount srcPosStride = 0;
        NCount srcNormStride = 0;
        NCount destPosStride = 0;
        NCount destNormStride = 0;
        NCount blendWeightStride = 0;
        NCount blendIdxStride = 0;

        // Get elements for source
        const VertexUnit * srcElemPos = src->get(VT_Pos);
        const VertexUnit * srcElemNorm = src->get(VT_Normals);
        const VertexUnit * srcElemBlendIndices = src->get(VT_Matrix_Index);
        const VertexUnit * srcElemBlendWeights = src->get(VT_Matrix_Weight);
        N_assert (srcElemPos && srcElemBlendIndices && srcElemBlendWeights,
            "You must supply at least positions, blend indices and blend weights");
        // Get elements for target
        const VertexUnit * destElemPos = dest->get(VT_Pos);
        const VertexUnit * destElemNorm = dest->get(VT_Normals);

        // Do we have normals and want to blend them?
        bool includeNormals = normal && (srcElemNorm != NULL) && (destElemNorm != NULL);


        // Get buffers for source
        VertexBuffer * srcPosBuf = src->getAttach(srcElemPos->mSource);
        VertexBuffer * srcIdxBuf = src->getAttach(srcElemBlendIndices->mSource);
        VertexBuffer * srcWeightBuf = src->getAttach(srcElemBlendWeights->mSource);
        VertexBuffer * srcNormBuf = 0;

        srcPosStride = srcPosBuf->getUnitSize();

        blendIdxStride = srcIdxBuf->getUnitSize();

        blendWeightStride = srcWeightBuf->getUnitSize();
        if(includeNormals)
        {
            srcNormBuf = src->getAttach(srcElemNorm->mSource);
            srcNormStride = srcNormBuf->getUnitSize();
        }
        // Get buffers for target
        VertexBuffer * destPosBuf = dest->getAttach(destElemPos->mSource);
        VertexBuffer * destNormBuf = 0;
        destPosStride = destPosBuf->getUnitSize();
        if(includeNormals)
        {
            destNormBuf = dest->getAttach(destElemNorm->mSource);
            destNormStride = destNormBuf->getUnitSize();
        }

        // Lock source buffers for reading
        Nui8 * pBuffer = (Nui8 *)srcPosBuf->lock(Buffer::MM_READ);
        srcElemPos->getData(pBuffer, pSrcPos);
        if(includeNormals)
        {
            if(srcNormBuf != srcPosBuf)
            {
                // Different buffer
                pBuffer = (Nui8 *)srcNormBuf->lock(Buffer::MM_READ);
            }
            srcElemNorm->getData(pBuffer, pSrcNorm);
        }

        // Indices must be 4 bytes
        N_assert(srcElemBlendIndices->mType == DT_UByte4x, "Blend indices must be DT_UByte");
        pBuffer = (Nui8 *)srcIdxBuf->lock(Buffer::MM_READ);
        srcElemBlendIndices->getData(pBuffer, pBlendIdx);
        if(srcWeightBuf != srcIdxBuf)
        {
            // Lock buffer
            pBuffer = (Nui8 *)srcWeightBuf->lock(Buffer::MM_READ);
        }
        srcElemBlendWeights->getData(pBuffer, pBlendWeight);
        NCount numWeightsPerVertex = DataTypeUtil::getCount(srcElemBlendWeights->mType);

        // Lock destination buffers for writing
        pBuffer = (Nui8 *)destPosBuf->lock(
            (destNormBuf != destPosBuf && destPosBuf->getUnitSize() == destElemPos->getSize()) ||
                (destNormBuf == destPosBuf && destPosBuf->getUnitSize() == destElemPos->getSize() + destElemNorm->getSize()) ?
                    Buffer::MM_WHOLE : (Buffer::MM_READ | Buffer::MM_WRITE));
        destElemPos->getData(pBuffer, pDestPos);

        if(includeNormals)
        {
            if(destNormBuf != destPosBuf)
            {
                pBuffer = (Nui8 *)destNormBuf->lock(destNormBuf->getUnitSize() == destElemNorm->getSize() ?
                    Buffer::MM_WHOLE : (Buffer::MM_READ | Buffer::MM_WRITE));
            }
            destElemNorm->getData(pBuffer, pDestNorm);
        }

        SIMDUtil::getImplementation()->softwareVertexSkinning(
            pSrcPos, pDestPos, pSrcNorm, pDestNorm,
            pBlendWeight, pBlendIdx,
            space,
            srcPosStride, destPosStride, srcNormStride, destNormStride,
            blendWeightStride, blendIdxStride,
            numWeightsPerVertex,
            dest->mCount);

        // Unlock source buffers
        srcPosBuf->unlock();
        srcIdxBuf->unlock();
        if(srcWeightBuf != srcIdxBuf)
        {
            srcWeightBuf->unlock();
        }
        if(includeNormals && srcNormBuf != srcPosBuf)
        {
            srcNormBuf->unlock();
        }
        // Unlock destination buffers
        destPosBuf->unlock();
        if(includeNormals && destNormBuf != destPosBuf)
        {
            destNormBuf->unlock();
        }
    }
    //--------------------------------------------------------------------------
}