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
#include "NiiMaterialLodScheme.h"
#include "NiiViewport.h"
#include "NiiPosNode.h"
#include "NiiException.h"
#include <limits>

namespace NII
{
    //------------------------------------------------------------------------
    struct lod_view_greater_mat : public std::binary_function<const std::pair<NIIf, Nui32> &,
        const std::pair<NIIf, Nui32> &, bool>
    {
        bool operator() (const std::pair<NIIf, Nui32> & a, const std::pair<NIIf, Nui32> & b)
        {
            // sort decending
            return a.first > b.first;
        }
    };
    //------------------------------------------------------------------------
    struct lod_camera_less_mat : public std::binary_function<const std::pair<NIIf, Nui32> &,
        const std::pair<NIIf, Nui32> &, bool>
    {
        bool operator()(const std::pair<NIIf, Nui32> & a, const std::pair<NIIf, Nui32> & b)
        {
            // sort ascending
            return a.first < b.first;
        }
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MaterialLodScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MaterialLodScheme::MaterialLodScheme(SchemeID sid) :
        LodScheme(sid)
    {
        mLods.push_back(Npair(0.0f, getDest(0.0f)));
    }
    //------------------------------------------------------------------------
    MaterialLodScheme::~MaterialLodScheme()
    {
        mLods.clear();
    }
    //------------------------------------------------------------------------
    void MaterialLodScheme::getLodList(SrcList & list) const
    {
        LodList::const_iterator i, iend = mLods.end();
        for (i = mLods.begin(); i != iend; ++i)
        {
            list.push_back(i->first);
        }
    }
    //------------------------------------------------------------------------
    void MaterialLodScheme::add(NIIf src)
    {
        mLods.push_back(Npair(src, getDest(src)));
    }
    //------------------------------------------------------------------------
    void MaterialLodScheme::removeAll()
    {
        mLods.clear();
        mLods.push_back(Npair(0.0f, getDest(0.0f)));
    }
    //------------------------------------------------------------------------
    void MaterialLodScheme::map()
    {
        LodList::iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i)
        {
            i->second = getDest(i->first);
        }
    }
    //------------------------------------------------------------------------
    const MaterialLodScheme::LodList & MaterialLodScheme::getList() const
    {
        return mLods;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MaterialViewLodScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MaterialViewLodScheme::MaterialViewLodScheme(SchemeID sid) :
        MaterialLodScheme(sid)
    {
    }
    //------------------------------------------------------------------------
    NIIf MaterialViewLodScheme::getOrigin() const
    {
        // Use the maximum possible value as base
        return N_TYPE_MAX(NIIf);
    }
    //------------------------------------------------------------------------
    Nidx MaterialViewLodScheme::get(Nui32 value) const
    {
        Nidx index = 0;
        LodList::const_iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i, ++index)
        {
            if(i->second < value)
            {
                return index ? index - 1 : 0;
            }
        }
        return mLods.size() - 1;
    }
    //------------------------------------------------------------------------
    void MaterialViewLodScheme::sort()
    {
        std::sort(mLods.begin(), mLods.end(), lod_view_greater_mat());
    }
    //------------------------------------------------------------------------
    bool MaterialViewLodScheme::valid() const
    {
        LodList::const_iterator it = mLods.begin();
        NIIf prev = it->first;
        for(++it; it != mLods.end(); ++it)
        {
            NIIf cur = it->first;
            if(cur > prev)
                return false;
            prev = cur;
        }
        return true;
    }
    //------------------------------------------------------------------------
    NIIf MaterialViewLodScheme::getValueImpl(const SpaceObj * obj, const Camera * camera) const
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
    LodScheme * MaterialViewLodScheme::clone() const
    {
        MaterialViewLodScheme * re = N_new MaterialViewLodScheme(mSid);
        re->mThreshold = mThreshold;
        re->mFactor = mFactor;
        re->mLods = mLods;
        return re;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MaterialCameraLodScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MaterialCameraLodScheme::MaterialCameraLodScheme(SchemeID sid) :
        MaterialLodScheme(sid),
        mRelation(false),
        mRefArea(-1)
    {
    }
    //------------------------------------------------------------------------
    void MaterialCameraLodScheme::setOriArea(NIIf w, NIIf h, Radian fovY)
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
    void MaterialCameraLodScheme::setRefArea(bool b)
    {
        // Ensure reference value has been set before being enabled
        N_assert(!b && mRefArea != -1, "Reference view must be set before being enabled!");

        mRelation = b;
    }
    //------------------------------------------------------------------------
    bool MaterialCameraLodScheme::isRefArea() const
    {
        return mRelation;
    }
    //------------------------------------------------------------------------
    Nui32 MaterialCameraLodScheme::getDest(NIIf src) const
    {
        // Square user-supplied distance
        return Math::Sqr(src) * mFactor;
    }
    //------------------------------------------------------------------------
    NIIf MaterialCameraLodScheme::getBias(NIIf f) const
    {
        N_assert(f > 0.0f, "Bias factor must be > 0!");
        return 1.0f / f * mFactor;
    }
    //------------------------------------------------------------------------
    Nidx MaterialCameraLodScheme::get(Nui32 value) const
    {
        Nidx index = 0;
        LodList::const_iterator i, iend = mLods.end();
        for(i = mLods.begin(); i != iend; ++i, ++index)
        {
            if(i->second > value)
            {
                return index ? index - 1 : 0;
            }
        }
        return mLods.size() - 1;
    }
    //------------------------------------------------------------------------
    void MaterialCameraLodScheme::sort()
    {
        std::sort(mLods.begin(), mLods.end(), lod_camera_less_mat());
    }
    //------------------------------------------------------------------------
    bool MaterialCameraLodScheme::valid() const
    {
        LodList::const_iterator it = mLods.begin();
        NIIf prev = it->first;
        for(++it; it != mLods.end(); ++it)
        {
            NIIf cur = it->first;
            if(cur < prev)
                return false;
            prev = cur;
        }
        return true;
    }
    //------------------------------------------------------------------------
    NIIf MaterialCameraLodScheme::getValueImpl(const SpaceObj * obj, const Camera * camera) const
    {
        // Get squared depth taking into account bounding radius
        // (d - r) ^ 2 = d^2 - 2dr + r^2, but this requires a lot
        // more computation (including a sqrt) so we approximate
        // it with d^2 - r^2, which is good enough for determining
        // lod.
        NIIf squaredDepth = obj->getSpaceNode()->distanceSquared(camera) - Math::Sqr(obj->getCenterRange());

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
    LodScheme * MaterialCameraLodScheme::clone() const
    {
        MaterialCameraLodScheme * re = N_new MaterialCameraLodScheme(mSid);
        re->mThreshold = mThreshold;
        re->mFactor = mFactor;
        re->mRelation = mRelation;
        re->mRefArea = mRefArea;
        re->mLods = mLods;
        return re;
    }
    //------------------------------------------------------------------------
}