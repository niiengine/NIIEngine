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
#include "NiiMeshLodScheme.h"
#include "NiiMeshManager.h"
#include "NiiViewport.h"
#include <limits>

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshLod
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshLodScheme::MeshLod::MeshLod() {}
    //------------------------------------------------------------------------
    MeshLodScheme::MeshLod::MeshLod(NIIf src, Nui32 dst, bool rel) :
        mSrc(src),
        mDest(dst),
        mGid(GroupUnknow),
        mRid(0),
        mMesh(NULL),
        mIdxBuf(0),
        mRelLod(rel)
    {
    }
    //------------------------------------------------------------------------
    MeshLodScheme::MeshLod::MeshLod(NIIf src, Nui32 dst, ResourceID rid, GroupID gid,
        Nidx bufidx, bool rel) :
        mSrc(src),
        mDest(dst),
        mRid(rid),
        mGid(gid),
        mMesh(NULL),
        mIdxBuf(bufidx),
        mRelLod(rel)
    {
    }
    //------------------------------------------------------------------------
    MeshLodScheme::MeshLod & MeshLodScheme::MeshLod::operator =(const MeshLod & o)
    {
        mSrc = o.mSrc;
        mDest = o.mDest;
        mRid = o.mRid;
        mGid = o.mGid;
        mMesh = o.mMesh;
        mIdxBuf = o.mIdxBuf;
        mRelLod = o.mRelLod;
        return *this;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshLodScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshLodScheme::MeshLodScheme() :
        mValidLod(false)
    {
    }
    //------------------------------------------------------------------------
    MeshLodScheme::MeshLodScheme(SchemeID sid) :
        LodScheme(sid),
        mValidLod(false)
    {
    }
    //------------------------------------------------------------------------
    MeshLodScheme::~MeshLodScheme()
    {
        LodList::iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i)
        {
            MeshLod & mld = *i;
            if(mld.mMesh)
            {
                mld.mMesh->unref();
                mld.mMesh = NULL;
            }
        }
        mLods.clear();
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::add(NIIf src)
    {
        Nui32 dst = getDest(src);
        MeshLod mld(src, dst, false);
        mLods.push_back(mld);
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::add(const MeshLod & lod)
    {
        mLods.push_back(lod);
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::add(NIIf src, Nidx idxbuf, ResourceID rid, GroupID gid, 
        bool rel)
    {
        Nui32 dst = getDest(src);
        MeshLod mld(src, dst, rid, gid, idxbuf, rel);
        mLods.push_back(mld);
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::add(NIIf src, Nui32 dst, Nidx idxbuf, ResourceID rid, 
        GroupID gid, bool rel)
    {
        MeshLod mld(src, dst, rid, gid, idxbuf, rel);
        mLods.push_back(mld);
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::set(Nidx index, NIIf src)
    {
        N_assert(index < mLods.size(), "error");
        mLods[index].mSrc = src;
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::set(Nidx index, ResourceID rid, Nidx idxbuf, bool rel)
    {
        if(index > 0 && index < mLods.size())
        {
            MeshLod & mld = mLods[index];
            if(mld.mRid != rid)
            {
                if(mld.mMesh)
                {
                    mld.mMesh->unref();
                    mld.mMesh = NULL;
                }
                mld.mRid = rid;
                mld.mIdxBuf = idxbuf;
                mld.mRelLod = rel;

                load(index);
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::get(Nidx index, ResourceID & out) const
    {
        N_assert(index < mLods.size(), "error");
        out = mLods[index].mRid;
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::get(Nidx index, Mesh *& out) const
    {
        N_assert(index < mLods.size(), "error");
        const_cast<MeshLodScheme *>(this)->load(index);
        out = mLods[index].mMesh;
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::get(Nidx index, GeoEdget *& out) const
    {
        N_assert(index < mLods.size(), "error");
        const_cast<MeshLodScheme *>(this)->load(index);
        if(mLods[index].mRelLod)
        {
            out = mLods[index].mMesh->getEdgeList(mLods[index].mIdxBuf);
        }
        else
        {
            out = mLods[index].mMesh->getEdgeList(0);
        }
    }
    //------------------------------------------------------------------------
    const MeshLodScheme::MeshLod & MeshLodScheme::getInfo(Nidx index) const
    {
        N_assert(index < mLods.size(), "error");
        return mLods[index];
    }
    //------------------------------------------------------------------------
    bool MeshLodScheme::isRelation(Nidx index) const
    {
        N_assert(index < mLods.size(), "error");
        return mLods[index].mRelLod;
    }
    //------------------------------------------------------------------------
    Nidx MeshLodScheme::getRelationIndex(Nidx index) const
    {
        N_assert(index < mLods.size(), "error");
        return mLods[index].mIdxBuf;
    }
    //------------------------------------------------------------------------
    NCount MeshLodScheme::getRelationCount() const
    {
        NCount re = 0;
        LodList::const_iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i)
        {
            if(i->mRelLod)
            {
                ++re;
            }
        }
        return re;
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::removeAll()
    {
        destroy();
        mLods.clear();
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::setCount(NCount count)
    {
        if(mLods.size() > count)
        {
            LodList::iterator i, iend = mLods.end();
            for(i = mLods.begin() + count; i != iend; ++i)
            {
                MeshLod & mld = *i;
                if(mld.mMesh)
                {
                    mld.mMesh->unref();
                    mld.mMesh = NULL;
                }
            }
            mLods.erase(mLods.begin() + count, iend);
            mLods.resize(count);
        }
    }
    //------------------------------------------------------------------------
    NCount MeshLodScheme::getCount() const
    {
        return mLods.size();
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::build()
    {
        if(mValidLod == false)
        {
            Nidx i, iend = mLods.size();
            for(i = 0; i < mLods.size(); ++i)
            {
                load(i);
            }
            mValidLod = true;
        }
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::destroy()
    {
        if(mValidLod)
        {

            LodList::iterator i, iend = mLods.end();
            for(i = mLods.begin(); i != iend; ++i)
            {
                MeshLod & mld = *i;
                if(mld.mMesh)
                {
                    mld.mMesh->unref();
                    mld.mMesh = NULL;
                }
            }
            mValidLod = false;
        }
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::map()
    {
        LodList::iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i)
        {
            i->mDest = getDest(i->mSrc);
        }
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::getLodList(SrcList & list) const
    {
        LodList::const_iterator i, iend = mLods.end();
        for (i = mLods.begin(); i != iend; ++i)
        {
            list.push_back(i->mSrc);
        }
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::setValid(bool s)
    {
        if(mValidLod != s && mValidLod == false)
        {
            destroy();
        }
        mValidLod = s;
    }
    //------------------------------------------------------------------------
    bool MeshLodScheme::isValid() const
    {
        return mValidLod;
    }
    //------------------------------------------------------------------------
    void MeshLodScheme::load(Nidx index)
    {
        N_assert(index < mLods.size(), "error");
        MeshLod & lod = mLods[index];
        if(lod.mMesh == NULL && lod.mRelLod != true)
        {
            lod.mMesh = N_Only(Mesh).load(lod.mRid, (lod.mGid == 0) ? GroupUnknow : lod.mGid);
            if(lod.mMesh)
            {
                lod.mMesh->ref();
                if(mLods[0].mMesh->isExist(Mesh::M_Edge))
                    lod.mMesh->build(Mesh::M_Edge);
            }
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshViewLodScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshViewLodScheme::MeshViewLodScheme(SchemeID sid):
        MeshLodScheme(sid)
    {
    }
    //------------------------------------------------------------------------
    NIIf MeshViewLodScheme::getOrigin() const
    {
        // Use the maximum possible value as base
        return N_TYPE_MAX(NIIf);
    }
    //------------------------------------------------------------------------
    Nidx MeshViewLodScheme::get(Nui32 dest) const
    {
        Nidx index = 0;
        LodList::const_iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i, ++index)
        {
            if(i->mDest < dest)
            {
                return index ? index - 1 : 0;
            }
        }
        // If we fall all the way through, use the highest value
        return mLods.size() - 1;
    }
    //------------------------------------------------------------------------
    struct lod_view_greater_mesh : public std::binary_function<const MeshLodScheme::MeshLod &,
        const MeshLodScheme::MeshLod &, bool>
    {
        bool operator() (const MeshLodScheme::MeshLod & a, const MeshLodScheme::MeshLod & b)
        {
            // sort decending
            return a.mDest > b.mDest;
        }
    };
    //------------------------------------------------------------------------
    void MeshViewLodScheme::sort()
    {
        // Perform standard sort
        std::sort(mLods.begin(), mLods.end(), lod_view_greater_mesh());
    }
    //------------------------------------------------------------------------
    bool MeshViewLodScheme::valid() const
    {
        LodList::const_iterator i = mLods.begin();
        NIIf prev = (*i).mSrc;
        for(++i; i != mLods.end(); ++i)
        {
            NIIf cur = (*i).mSrc;
            if(cur > prev)
                return false;
            prev = cur;
        }
        return true;
    }
    //------------------------------------------------------------------------
    NIIf MeshViewLodScheme::getValueImpl(const SpaceObj * obj, const Camera * camera) const
    {
        // Get viewport
        const Viewport * viewport = camera->getViewport();
        // Get viewport area
        NIIf viewportArea = static_cast<NIIf>(viewport->getPixelWidth() * viewport->getPixelHeight());
        // Get area of unprojected circle with object bounding radius
        NIIf boundingArea = Math::PI * Math::Sqr(obj->getCenterRange());
        // Base computation on projection type
        switch (camera->getProjType())
        {
        case Frustum::T_Perspective:
            {
                // Get camera distance
                NIIf distanceSquared = obj->getSpaceNode()->distanceSquared(camera);

                // Check for 0 distance
                if(distanceSquared <= N_TYPE_EPSILON(NIIf))
                    return getOrigin();

                // Get projection matrix (this is done to avoid computation of tan(fov / 2))
                const Matrix4f & projectionMatrix = camera->getProjMatrix();

                // Estimate pixel count
                return (boundingArea * viewportArea * projectionMatrix[0][0] * projectionMatrix[1][1]) / distanceSquared;
            }
        case Frustum::T_Orthogonal:
            {
                // Compute orthographic area
                NIIf orthoArea = camera->getOrthoAspectHeight() * camera->getOrthoAspectWidth();

                // Check for 0 orthographic area
                if(orthoArea <= N_TYPE_EPSILON(NIIf))
                    return getOrigin();

                // Estimate pixel count
                return (boundingArea * viewportArea) / orthoArea;
            }
        default:
            {
                // This case is not covered for obvious reasons
                throw;
            }
        }
    }
    //------------------------------------------------------------------------
    LodScheme * MeshViewLodScheme::clone() const
    {
        MeshViewLodScheme * re = N_new MeshViewLodScheme(mSid);
        re->mLods = mLods;
        re->mValidLod = mValidLod;

        LodList::iterator i, iend = re->mLods.end();
        for (i = re->mLods.begin(); i != iend; ++i)
        {
            if (i->mMesh)
            {
                i->mMesh->ref();
            }
        }
        return re;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshCameraLodScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshCameraLodScheme::MeshCameraLodScheme(SchemeID sid):
        MeshLodScheme(sid),
        mRelation(false),
        mRefArea(-1)
    {
    }
    //------------------------------------------------------------------------
    void MeshCameraLodScheme::setOriArea(NIIf w, NIIf h, Radian fovY)
    {
        // Determine x FOV based on aspect ratio
        Radian fovX = fovY * (w / h);
        // Determine viewport area
        NIIf viewportArea = h * w;
        // Compute reference view value based on viewport area and FOVs
        mRefArea = viewportArea * Math::Tan(fovX * 0.5f) * Math::Tan(fovY * 0.5f);
        // Enable use of reference view
        mRelation = true;
    }
    //------------------------------------------------------------------------
    void MeshCameraLodScheme::setRefArea(bool b)
    {
        // Ensure reference value has been set before being enabled
        N_assert(!b && mRefArea != -1, "Reference view must be set before being enabled!");

        mRelation = b;
    }
    //------------------------------------------------------------------------
    bool MeshCameraLodScheme::isRefArea() const
    {
        return mRelation;
    }
    //-----------------------------------------------------------------------
    Nui32 MeshCameraLodScheme::getDest(NIIf src) const
    {
        // Square user-supplied distance
        return Math::Sqr(src) * mFactor;
    }
    //---------------------------------------------------------------------
    NIIf MeshCameraLodScheme::getBias(NIIf f) const
    {
        N_assert(f > 0.0f, "Bias factor must be > 0!");
        return 1.0f / f * mFactor;
    }
    //------------------------------------------------------------------------
    Nidx MeshCameraLodScheme::get(Nui32 dest) const
    {
        Nidx index = 0;
        LodList::const_iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i, ++index)
        {
            if(i->mDest > dest)
            {
                return index ? index - 1 : 0;
            }
        }
        // If we fall all the way through, use the highest value
        return mLods.size() - 1;
    }
    //------------------------------------------------------------------------
    struct lod_camera_less : public std::binary_function<const MeshLodScheme::MeshLod &,
        const MeshLodScheme::MeshLod &, bool>
    {
        bool operator()(const MeshLodScheme::MeshLod & a, const MeshLodScheme::MeshLod & b)
        {
            // sort ascending
            return a.mDest < b.mDest;
        }
    };
    //------------------------------------------------------------------------
    void MeshCameraLodScheme::sort()
    {
        std::sort(mLods.begin(), mLods.end(), lod_camera_less());
    }
    //------------------------------------------------------------------------
    bool MeshCameraLodScheme::valid() const
    {
        LodList::const_iterator i = mLods.begin();
        NIIf prev = (*i).mSrc;
        for(++i; i != mLods.end(); ++i)
        {
            NIIf cur = (*i).mSrc;
            if(cur < prev)
                return false;
            prev = cur;
        }
        return true;
    }
    //------------------------------------------------------------------------
    NIIf MeshCameraLodScheme::getValueImpl(const SpaceObj * obj, const Camera * camera) const
    {
        // Get squared depth taking into account bounding radius
        // (d - r) ^ 2 = d^2 - 2dr + r^2, but this requires a lot
        // more computation (including a sqrt) so we approximate
        // it with d^2 - r^2, which is good enough for determining
        // lod.
        NIIf squaredDepth = obj->getSpaceNode()->distanceSquared(camera) -
            Math::Sqr(obj->getCenterRange());

        // Check if reference view needs to be taken into account
        if(mRelation)
        {
            // Reference view only applicable to perspective projection
            N_assert(camera->getProjType() == Frustum::T_Perspective, "Camera projection type must be perspective!");

            // Get camera viewport
            Viewport * viewport = camera->getViewport();

            // Get viewport area
            NIIf viewportArea = static_cast<NIIf>(viewport->getPixelWidth() * viewport->getPixelHeight());

            // Get projection matrix (this is done to avoid computation of tan(fov / 2))
            const Matrix4f & projectionMatrix = camera->getProjMatrix();

            // Compute bias value (note that this is similar to the method used for ViewLodScheme)
            NIIf biasValue = viewportArea * projectionMatrix[0][0] * projectionMatrix[1][1];

            // Scale squared depth appropriately
            squaredDepth *= (mRefArea / biasValue);
        }

        // 二次方距离永远不会小于0
        squaredDepth = std::max(squaredDepth, NIIf(0));

        // Now adjust it by the camera bias and return the computed value
        return squaredDepth * (1.0f / camera->getLodBias());
    }
    //------------------------------------------------------------------------
    LodScheme * MeshCameraLodScheme::clone() const
    {
        MeshCameraLodScheme * re = N_new MeshCameraLodScheme(mSid);
        re->mLods = mLods;
        re->mValidLod = mValidLod;

        LodList::iterator i, iend = re->mLods.end();
        for (i = re->mLods.begin(); i != iend; ++i)
        {
            if (i->mMesh)
            {
                i->mMesh->ref();
            }
        }
        return re;
    }
    //------------------------------------------------------------------------
}