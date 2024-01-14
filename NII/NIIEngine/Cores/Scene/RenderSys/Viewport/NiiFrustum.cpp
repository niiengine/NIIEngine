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
#include "NiiFrustum.h"
#include "NiiSpaceNode.h"
#include "NiiSphere.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiGBufferManager.h"
#include "NiiGpuBuffer.h"
#include "NiiMaterialManager.h"
#include "NiiGeometryObj.h"

#define INFINITE_FAR_PLANE_ADJUST 0.00001f

namespace NII
{
    //-----------------------------------------------------------------------
    Frustum::Frustum() :
        SpaceObj(),
        mProjType(Frustum::T_Perspective),
        mFOV(Radian(Math::PI/4.0f)),
        mFarExtent(100000.0f),
        mNearExtent(100.0f),
        mUnit(1.0f),
        mAspect(1.33333333333333f),
        mOrthoWidth(1333.3333333f),
        mOrthoHeight(1000),
        mOffset(Vector2f::ZERO),
        mLastNodeDir(Quaternion::IDENTITY),
        mLastNodePos(Vector3f::ZERO),
        mDirection(0),
        mProjValid(false),
        mViewValid(false),
        mPlaneValid(false),
        mPlaneIntersectValid(false),
        mDebugVertexValid(false),
        mCustomViewMatrix(false),
        mCustomProjMatrix(false),
        mCustomAspect(false),
        mReflect(false),
        mOblique(false)
    {
        mMaterial = const_cast<Material *>(MaterialManager::WhiteColour);
        mMaterial->ref(false);

        mVisible = false;
        mSpaceNode = 0;

        mLastReflectPlane.mNormal = Vector3f::ZERO;
        mLastObliqueProjPlane.mNormal = Vector3f::ZERO;

        updateView();
        updateProj();
    }
    //-----------------------------------------------------------------------
    Frustum::Frustum(SpaceID sid, SpaceManager * mag) :
        SpaceObj(sid, mag),
        mProjType(Frustum::T_Perspective),
        mFOV(Radian(Math::PI/4.0f)),
        mFarExtent(100000.0f),
        mNearExtent(100.0f),
        mUnit(1.0f),
        mAspect(1.33333333333333f),
        mOrthoWidth(1333),
        mOrthoHeight(1000),
        mOffset(Vector2f::ZERO),
        mLastNodeDir(Quaternion::IDENTITY),
        mLastNodePos(Vector3f::ZERO),
        mDirection(0),
        mProjValid(false),
        mViewValid(false),
        mPlaneValid(false),
        mPlaneIntersectValid(false),
        mDebugVertexValid(false),
        mCustomViewMatrix(false),
        mCustomProjMatrix(false),
        mCustomAspect(false),
        mReflect(false),
        mOblique(false)
    {
        mMaterial = const_cast<Material *>(MaterialManager::WhiteColour);
        mMaterial->ref(false);
        GeometryObj::on(GeometryObj::M_Debug);

        mVisible = false;
        mSpaceNode = 0;

        mLastReflectPlane.mNormal = Vector3f::ZERO;
        mLastObliqueProjPlane.mNormal = Vector3f::ZERO;

        updateView();
        updateProj();
    }
    //-----------------------------------------------------------------------
    Frustum::~Frustum()
    {
        mMaterial->unref();
    }
    //-----------------------------------------------------------------------
    void Frustum::setFOV(const Radian & angle)
    {
        mFOV = angle;
        notifyProj();
    }
    //-----------------------------------------------------------------------
    void Frustum::setFarClipDistance(NIIf farPlane)
    {
        mFarExtent = farPlane;
        notifyProj();
    }
    //-----------------------------------------------------------------------
    NIIf Frustum::getFarClipDistance() const
    {
        return mFarExtent;
    }
    //-----------------------------------------------------------------------
    void Frustum::setNearClipDistance(NIIf extent)
    {
        if(extent <= 0)
            extent = 0.01f;
        mNearExtent = extent;
        notifyProj();
    }
    //-----------------------------------------------------------------------
    NIIf Frustum::getNearClipDistance() const
    {
        return mNearExtent;
    }
    //---------------------------------------------------------------------
    void Frustum::setAspectOffset(const Vector2f & oft)
    {
        mOffset = oft;
        notifyProj();
    }
    //---------------------------------------------------------------------
    const Vector2f & Frustum::getAspectOffset() const
    {
        return mOffset;
    }
    //---------------------------------------------------------------------
    void Frustum::setAspectUnit(NIIf unit)
    {
        if(unit <= 0)
            N_EXCEPT(InvalidParam, _I18n("Focal length must be greater than zero."));

        mUnit = unit;
        notifyProj();
    }
    //---------------------------------------------------------------------
    NIIf Frustum::getAspectUnit() const
    {
        return mUnit;
    }
    //-----------------------------------------------------------------------
    const Matrix4f & Frustum::getViewMatrix() const
    {
        const_cast<Frustum *>(this)->updateView();
        return mViewMatrix;
    }
    //-----------------------------------------------------------------------
    const Matrix4f & Frustum::getProjMatrix() const
    {
        const_cast<Frustum *>(this)->updateProj();
        return mProjMatrix;
    }
    //-----------------------------------------------------------------------
    const Matrix4f & Frustum::getSysProjMatrix() const
    {
        const_cast<Frustum *>(this)->updateProj();
        return mSysProjMatrix;
    }
    //-----------------------------------------------------------------------
    const Plane & Frustum::getClipPlane(Frustum::PlaneType plane) const
    {
        // Make any pending updates to the calculated frustum planes
        const_cast<Frustum *>(this)->updateFrustumPlanes();
        return mFrustumPlanes[plane];
    }
    //-----------------------------------------------------------------------
    void Frustum::setReflection(bool b, const Plane & p)
    {
        if(b)
        {
            mReflect = b;
            mReflectPlane = p;
            p.getReflect(&mReflectMatrix);
            mLastReflectPlane = p;
            
        }
        else
        {
            mReflect = b;
            mLastReflectPlane.mNormal = Vector3f::ZERO;
            notifyView();
        }
    }
    //-----------------------------------------------------------------------
    const Vector3f * Frustum::getClipPlaneIntersect() const
    {
        const_cast<Frustum *>(this)->updateView();

        if(!mPlaneIntersectValid)
        {
            Matrix4f eyeToWorld = mViewMatrix.inverseAffine();

            // Note: Even though we can dealing with general projection matrix here,
            //       but because it's incompatibly with infinite far plane, thus, we
            //       still need to working with projection parameters.

            // Calc near plane corners
            NIIf nearLeft, nearRight, nearBottom, nearTop;
            updateAspect(nearLeft, nearRight, nearBottom, nearTop);

            // Treat infinite fardist as some arbitrary far value
            NIIf farDist = (mFarExtent == 0) ? 100000 : mFarExtent;

            // Calc far palne corners
            NIIf radio = mProjType == Frustum::T_Perspective ? farDist / mNearExtent : 1;
            NIIf farLeft = nearLeft * radio;
            NIIf farRight = nearRight * radio;
            NIIf farBottom = nearBottom * radio;
            NIIf farTop = nearTop * radio;

            // near
            mPlaneIntersect[0] = eyeToWorld.affine(Vector3f(nearRight, nearTop,    -mNearExtent));
            mPlaneIntersect[1] = eyeToWorld.affine(Vector3f(nearLeft,  nearTop,    -mNearExtent));
            mPlaneIntersect[2] = eyeToWorld.affine(Vector3f(nearLeft,  nearBottom, -mNearExtent));
            mPlaneIntersect[3] = eyeToWorld.affine(Vector3f(nearRight, nearBottom, -mNearExtent));
            // far
            mPlaneIntersect[4] = eyeToWorld.affine(Vector3f(farRight,  farTop,     -farDist));
            mPlaneIntersect[5] = eyeToWorld.affine(Vector3f(farLeft,   farTop,     -farDist));
            mPlaneIntersect[6] = eyeToWorld.affine(Vector3f(farLeft,   farBottom,  -farDist));
            mPlaneIntersect[7] = eyeToWorld.affine(Vector3f(farRight,  farBottom,  -farDist));

            mPlaneIntersectValid = true;
        }

        return mPlaneIntersect;
    }
    //-----------------------------------------------------------------------
    bool Frustum::isIntersect(const AABox & bound) const
    {
        // Null boxes always invisible
        if(bound.isNull())
            return false;

        // Make any pending updates to the calculated frustum planes
        const_cast<Frustum *>(this)->updateFrustumPlanes();

        // For each plane, see if all points are on the negative side
        // If so, object is not visible
        for(NIIi plane = 0; plane < 6; ++plane)
        {
            // Skip far plane if infinite view frustum
            if(plane == Frustum::PT_Far && mFarExtent == 0)
                continue;

            Plane::PositionType side = mFrustumPlanes[plane].getSide(bound);
            if(side == Plane::PT_Negative)
            {
                // ALL corners on negative side therefore out of view
                return false;
            }

        }
        return true;
    }
    //-----------------------------------------------------------------------
    bool Frustum::isIntersect(const Vector3f & vert) const
    {
        // Make any pending updates to the calculated frustum planes
        const_cast<Frustum *>(this)->updateFrustumPlanes();

        // For each plane, see if all points are on the negative side
        // If so, object is not visible
        for(NIIi plane = 0; plane < 6; ++plane)
        {
            // Skip far plane if infinite view frustum
            if(plane == Frustum::PT_Far && mFarExtent == 0)
                continue;

            if(mFrustumPlanes[plane].getSide(vert) == Plane::PT_Negative)
            {
                // ALL corners on negative side therefore out of view
                return false;
            }
        }
        return true;
    }
    //-----------------------------------------------------------------------
    bool Frustum::isIntersect(const Sphere & sphere) const
    {
        // Make any pending updates to the calculated frustum planes
        const_cast<Frustum *>(this)->updateFrustumPlanes();

        // For each plane, see if sphere is on negative side
        // If so, object is not visible
        for(NIIi plane = 0; plane < 6; ++plane)
        {
            // Skip far plane if infinite view frustum
            if (plane == Frustum::PT_Far && mFarExtent == 0)
                continue;

            // If the distance from sphere center to plane is negative, and 'more negative'
            // than the radius of the sphere, sphere is outside frustum
            if(mFrustumPlanes[plane].distance(sphere.getCenter()) < -sphere.getRadius())
            {
                // ALL corners on negative side therefore out of view
                return false;
            }
        }
        return true;
    }
    //---------------------------------------------------------------------
    Nui32 Frustum::getTypeMark() const
    {
        return SpaceManager::ViewSpaceMark;
    }
    //-----------------------------------------------------------------------
    void Frustum::updateAspect(NIIf & left, NIIf & right, NIIf & bottom, NIIf & top) const
    {
        if(mCustomProjMatrix)
        {
            // Convert clipspace corners to camera space
            Matrix4f invProj = mProjMatrix.inverse();
            Vector3f topLeft(-0.5f, 0.5f, 0.0f);
            Vector3f bottomRight(0.5f, -0.5f, 0.0f);

            topLeft = invProj * topLeft;
            bottomRight = invProj * bottomRight;

            left = topLeft.x;
            top = topLeft.y;
            right = bottomRight.x;
            bottom = bottomRight.y;
        }
        else
        {
            if(mCustomAspect)
            {
                left = mLeft;
                right = mRight;
                top = mTop;
                bottom = mBottom;
            }
            // Calculate general projection parameters
            else if(mProjType == Frustum::T_Perspective)
            {
                Radian thetaY (mFOV * 0.5f);
                NIIf tanThetaY = Math::Tan(thetaY);
                NIIf tanThetaX = tanThetaY * mAspect;

                NIIf nearFocal = mNearExtent / mUnit;
                NIIf nearOffsetX = mOffset.x * nearFocal;
                NIIf nearOffsetY = mOffset.y * nearFocal;
                NIIf half_w = tanThetaX * mNearExtent;
                NIIf half_h = tanThetaY * mNearExtent;

                left   = - half_w + nearOffsetX;
                right  = + half_w + nearOffsetX;
                bottom = - half_h + nearOffsetY;
                top    = + half_h + nearOffsetY;

                mLeft = left;
                mRight = right;
                mTop = top;
                mBottom = bottom;
            }
            else
            {
                // Unknown how to apply frustum offset to orthographic camera, just ignore here
                NIIf half_w = getOrthoAspectWidth() * 0.5f;
                NIIf half_h = getOrthoAspectHeight() * 0.5f;

                left   = - half_w;
                right  = + half_w;
                bottom = - half_h;
                top    = + half_h;

                mLeft = left;
                mRight = right;
                mTop = top;
                mBottom = bottom;
            }
        }
    }
    //-----------------------------------------------------------------------
    void Frustum::updateProjImpl()
    {
        // Common calcs
        NIIf left, right, bottom, top;
        updateAspect(left, right, bottom, top);

        if(!mCustomProjMatrix)
        {
            // The code below will dealing with general projection
            // parameters, similar glFrustum and glOrtho.
            // Doesn't optimise manually except division operator, so the
            // code more self-explaining.

            NIIf inv_w = 1 / (right - left);
            NIIf inv_h = 1 / (top - bottom);
            NIIf inv_d = 1 / (mFarExtent - mNearExtent);

            // Recalc if frustum params changed
            if(mProjType == Frustum::T_Perspective)
            {
                // Calc matrix elements
                NIIf A = 2 * mNearExtent * inv_w;
                NIIf B = 2 * mNearExtent * inv_h;
                NIIf C = (right + left) * inv_w;
                NIIf D = (top + bottom) * inv_h;
                NIIf q, qn;
                if (mFarExtent == 0)
                {
                    // Infinite far plane
                    q = INFINITE_FAR_PLANE_ADJUST - 1;
                    qn = mNearExtent * (INFINITE_FAR_PLANE_ADJUST - 2);
                }
                else
                {
                    q = - (mFarExtent + mNearExtent) * inv_d;
                    qn = -2 * (mFarExtent * mNearExtent) * inv_d;
                }

                // NB: This creates 'uniform' perspective projection matrix,
                // which depth range [-1,1], right-handed rules
                //
                // [ A   0   C   0  ]
                // [ 0   B   D   0  ]
                // [ 0   0   q   qn ]
                // [ 0   0   -1  0  ]
                //
                // A = 2 * near / (right - left)
                // B = 2 * near / (top - bottom)
                // C = (right + left) / (right - left)
                // D = (top + bottom) / (top - bottom)
                // q = - (far + near) / (far - near)
                // qn = - 2 * (far * near) / (far - near)

                mProjMatrix = Matrix4f::ZERO;
                mProjMatrix[0][0] = A;
                mProjMatrix[0][2] = C;
                mProjMatrix[1][1] = B;
                mProjMatrix[1][2] = D;
                mProjMatrix[2][2] = q;
                mProjMatrix[2][3] = qn;
                mProjMatrix[3][2] = -1;

                if(mOblique)
                {
                    // Translate the plane into view space

                    // Don't use getViewMatrix here, incase overrided by
                    // camera and return a cull frustum view matrix
                    updateView();
                    Plane plane(mObliqueProjPlane);
                    plane.transform(mViewMatrix);

                    // Thanks to Eric Lenyel for posting this calculation
                    // at www.terathon.com

                    // Calculate the clip-space corner point opposite the
                    // clipping plane
                    // as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
                    // transform it into camera space by multiplying it
                    // by the inverse of the projection matrix

                    /* generalised version
                    Vector4 q = matrix.inverse() *
                    Vector4(Math::Sign(plane.normal.x),
                    Math::Sign(plane.normal.y), 1.0f, 1.0f);
                    */
                    Vector4f q;
                    q.x = (Math::Sign(plane.mNormal.x) + mProjMatrix[0][2]) / mProjMatrix[0][0];
                    q.y = (Math::Sign(plane.mNormal.y) + mProjMatrix[1][2]) / mProjMatrix[1][1];
                    q.z = -1;
                    q.w = (1 + mProjMatrix[2][2]) / mProjMatrix[2][3];

                    // Calculate the scaled plane vector
                    Vector4f clipPlane4d(plane.mNormal.x, plane.mNormal.y, plane.mNormal.z, plane.mD);
                    Vector4f c = clipPlane4d * (2 / (clipPlane4d.dotProduct(q)));

                    // Replace the third row of the projection matrix
                    mProjMatrix[2][0] = c.x;
                    mProjMatrix[2][1] = c.y;
                    mProjMatrix[2][2] = c.z + 1;
                    mProjMatrix[2][3] = c.w;
                }
            } // perspective
            else if(mProjType == Frustum::T_Orthogonal)
            {
                NIIf A = 2 * inv_w;
                NIIf B = 2 * inv_h;
                NIIf C = - (right + left) * inv_w;
                NIIf D = - (top + bottom) * inv_h;
                NIIf q, qn;
                if(mFarExtent == 0)
                {
                    // Can not do infinite far plane here, avoid divided zero only
                    q = - INFINITE_FAR_PLANE_ADJUST / mNearExtent;
                    qn = - INFINITE_FAR_PLANE_ADJUST - 1;
                }
                else
                {
                    q = - 2 * inv_d;
                    qn = - (mFarExtent + mNearExtent)  * inv_d;
                }

                // NB: This creates 'uniform' orthographic projection matrix,
                // which depth range [-1,1], right-handed rules
                //
                // [ A   0   0   C  ]
                // [ 0   B   0   D  ]
                // [ 0   0   q   qn ]
                // [ 0   0   0   1  ]
                //
                // A = 2 * / (right - left)
                // B = 2 * / (top - bottom)
                // C = - (right + left) / (right - left)
                // D = - (top + bottom) / (top - bottom)
                // q = - 2 / (far - near)
                // qn = - (far + near) / (far - near)

                mProjMatrix = Matrix4f::ZERO;
                mProjMatrix[0][0] = A;
                mProjMatrix[0][3] = C;
                mProjMatrix[1][1] = B;
                mProjMatrix[1][3] = D;
                mProjMatrix[2][2] = q;
                mProjMatrix[2][3] = qn;
                mProjMatrix[3][3] = 1;
            } // ortho
        } // !mCustomProjMatrix

#if NII_VIEWPORT_DIR_NONE == 0
        Quaternion temp(Angle(mDirection), Vector3f::Z);
        // Deal with orientation mode
        mProjMatrix = mProjMatrix * Matrix4f(temp);
#endif

        RenderSys * rsys = N_Engine().getRender();
        rsys->getSysProj(mProjMatrix, mSysProjMatrix);

        // Calculate bounding box (local)
        // Box is from 0, down -Z, max dimensions as determined from far plane
        // If infinite view frustum just pick a far value
        NIIf farDist = (mFarExtent == 0) ? 100000 : mFarExtent;
        // Near plane bounds
        Vector3f min(left, bottom, -farDist);
        Vector3f max(right, top, 0);

        if(mCustomProjMatrix)
        {
            // Some custom projection matrices can have unusual inverted settings
            // So make sure the AABB is the right way around to start with
            Vector3f tmp = min;
            min = min.floor(max);
            max = max.ceil(tmp);
        }

        if(mProjType == Frustum::T_Perspective)
        {
            // Merge with far plane bounds
            NIIf radio = farDist / mNearExtent;
            min = min.floor(Vector3f(left * radio, bottom * radio, -farDist));
            max = max.ceil(Vector3f(right * radio, top * radio, 0));
        }
        mBoundingBox.setMin(min);
        mBoundingBox.setMax(max);
        mProjValid = true;

        // Signal to update frustum clipping planes
        mPlaneValid = false;
    }
    //-----------------------------------------------------------------------
    void Frustum::updateProj()
    {
        if(!isProjValid())
        {
            updateProjImpl();
        }
    }
    //-----------------------------------------------------------------------
    bool Frustum::isViewValid() const
    {
        // Attached to node?
        if(mSpaceNode)
        {
            if(!mViewValid || mSpaceNode->getSpaceOri() != mLastNodeDir ||
                mSpaceNode->getSpacePos() != mLastNodePos)
            {
                // Ok, we're out of date with SpaceNode we're attached to
                mLastNodeDir = mSpaceNode->getSpaceOri();
                mLastNodePos = mSpaceNode->getSpacePos();
                mViewValid = false;
            }
        }
        // Deriving reflection from linked plane?
        if(mLastReflectPlane != mReflectPlane)
        {
            mReflectPlane.getReflect(&mReflectMatrix);
            mLastReflectPlane = mReflectPlane;
            mViewValid = false;
        }

        return mViewValid;
    }
    //-----------------------------------------------------------------------
    bool Frustum::isProjValid() const
    {
        // Deriving custom near plane from linked plane?
        if(mOblique)
        {
            // Out of date when view out of data since plane needs to be in view space
            if(!isViewValid())
            {
                mProjValid = false;
            }
            // Update derived plane
            if(mLastObliqueProjPlane != mObliqueProjPlane)
            {
                mLastObliqueProjPlane = mObliqueProjPlane;
                mProjValid = false;
            }
        }
        return mProjValid;
    }
    //-----------------------------------------------------------------------
    void Frustum::updateViewImpl()
    {
        // ----------------------
        // Update the view matrix
        // ----------------------

        // Get orientation from quaternion
        if(!mCustomViewMatrix)
        {
            Matrix3f rot;
            const Quaternion & orientation = getNodeOri();
            const Vector3f & position = getNodePos();

            updateViewMatrix(position, orientation, mReflect? &mReflectMatrix : 0);
        }

        mViewValid = true;

        // Signal to update frustum clipping planes
        mPlaneValid = false;
        // Signal to update world space corners
        mPlaneIntersectValid = false;
        // Signal to update frustum if oblique plane enabled,
        // since plane needs to be in view space
        if(mOblique)
        {
            mProjValid = false;
        }
    }
    //-----------------------------------------------------------------------
    void Frustum::updateView()
    {
        if(!isViewValid())
        {
            updateViewImpl();
        }
    }
    //-----------------------------------------------------------------------
    void Frustum::updateFrustumPlanes(void)
    {
        updateView();
        updateProj();

        if(!mPlaneValid)
        {
            // -------------------------
            // Update the frustum planes
            // -------------------------
            Matrix4f combo = mProjMatrix * mViewMatrix;

            mFrustumPlanes[Frustum::PT_Left].mNormal.x = combo[3][0] + combo[0][0];
            mFrustumPlanes[Frustum::PT_Left].mNormal.y = combo[3][1] + combo[0][1];
            mFrustumPlanes[Frustum::PT_Left].mNormal.z = combo[3][2] + combo[0][2];
            mFrustumPlanes[Frustum::PT_Left].mD = combo[3][3] + combo[0][3];

            mFrustumPlanes[Frustum::PT_Right].mNormal.x = combo[3][0] - combo[0][0];
            mFrustumPlanes[Frustum::PT_Right].mNormal.y = combo[3][1] - combo[0][1];
            mFrustumPlanes[Frustum::PT_Right].mNormal.z = combo[3][2] - combo[0][2];
            mFrustumPlanes[Frustum::PT_Right].mD = combo[3][3] - combo[0][3];

            mFrustumPlanes[Frustum::PT_Top].mNormal.x = combo[3][0] - combo[1][0];
            mFrustumPlanes[Frustum::PT_Top].mNormal.y = combo[3][1] - combo[1][1];
            mFrustumPlanes[Frustum::PT_Top].mNormal.z = combo[3][2] - combo[1][2];
            mFrustumPlanes[Frustum::PT_Top].mD = combo[3][3] - combo[1][3];

            mFrustumPlanes[Frustum::PT_Bottom].mNormal.x = combo[3][0] + combo[1][0];
            mFrustumPlanes[Frustum::PT_Bottom].mNormal.y = combo[3][1] + combo[1][1];
            mFrustumPlanes[Frustum::PT_Bottom].mNormal.z = combo[3][2] + combo[1][2];
            mFrustumPlanes[Frustum::PT_Bottom].mD = combo[3][3] + combo[1][3];

            mFrustumPlanes[Frustum::PT_Near].mNormal.x = combo[3][0] + combo[2][0];
            mFrustumPlanes[Frustum::PT_Near].mNormal.y = combo[3][1] + combo[2][1];
            mFrustumPlanes[Frustum::PT_Near].mNormal.z = combo[3][2] + combo[2][2];
            mFrustumPlanes[Frustum::PT_Near].mD = combo[3][3] + combo[2][3];

            mFrustumPlanes[Frustum::PT_Far].mNormal.x = combo[3][0] - combo[2][0];
            mFrustumPlanes[Frustum::PT_Far].mNormal.y = combo[3][1] - combo[2][1];
            mFrustumPlanes[Frustum::PT_Far].mNormal.z = combo[3][2] - combo[2][2];
            mFrustumPlanes[Frustum::PT_Far].mD = combo[3][3] - combo[2][3];

            // Renormalise any normals which were not unit length
            for(NIIi i=0; i<6; i++ )
            {
                NIIf length = mFrustumPlanes[i].mNormal.normalise();
                mFrustumPlanes[i].mD /= length;
            }

            mPlaneValid = true;
        }
    }
    //-----------------------------------------------------------------------
    void Frustum::setAspectRatio(NIIf r)
    {
        mAspect = r;
        notifyProj();
    }
    //-----------------------------------------------------------------------
    NIIf Frustum::getAspectRatio() const
    {
        return mAspect;
    }
    //-----------------------------------------------------------------------
    const AABox & Frustum::getAABB() const
    {
        return mBoundingBox;
    }
    //-----------------------------------------------------------------------
    NIIf Frustum::getCenterRange() const
    {
        return (mFarExtent == 0)? 100000 : mFarExtent;
    }
    //-----------------------------------------------------------------------
    void Frustum::queue(RenderQueue * queue)
    {
        if(mDebugMode)
        {
            // Add self
            queue->add(this);
        }
    }
    //-----------------------------------------------------------------------
    FactoryID Frustum::getFactoryID() const
    {
        return N_FACTORY_Frustum;
    }
    //-----------------------------------------------------------------------
    const Material * Frustum::getMaterial() const
    {
        return mMaterial;
    }
    //-----------------------------------------------------------------------
    void Frustum::getGeometry(GeometryRaw & op) const
    {
        if(!mDebugVertexValid)
        {
            if(mVertexData.getAttachCount() <= 0)
            {
                VertexBuffer * temp;
                // Initialise vertex & index data
                mVertexData.add(DT_Float3x, VT_Pos, 0, 0);
                mVertexData.mCount = 32;
                mVertexData.mOffset = 0;
                N_Only(GBuffer).create(temp, sizeof(NIIf) * 3, 32, Buffer::M_DEVREAD | M_DEVWRITE);
                mVertexData.attach(0, temp);
            }

            // Note: Even though we can dealing with general projection matrix here,
            //       but because it's incompatibly with infinite far plane, thus, we
            //       still need to working with projection parameters.

            // Calc near plane corners
            NIIf vpLeft, vpRight, vpBottom, vpTop;
            updateAspect(vpLeft, vpRight, vpBottom, vpTop);

            // Treat infinite fardist as some arbitrary far value
            NIIf farDist = (mFarExtent == 0) ? 100000 : mFarExtent;

            // Calc far plane corners
            NIIf radio = mProjType == Frustum::T_Perspective ? farDist / mNearExtent : 1;
            NIIf farLeft = vpLeft * radio;
            NIIf farRight = vpRight * radio;
            NIIf farBottom = vpBottom * radio;
            NIIf farTop = vpTop * radio;

            // Calculate vertex positions (local)
            // 0 is the origin
            // 1, 2, 3, 4 are the points on the near plane, top left first, clockwise
            // 5, 6, 7, 8 are the points on the far plane, top left first, clockwise
            VertexBuffer * vbuf = mVertexData.getAttach(0);
            NIIf * pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));

            // near plane (remember frustum is going in -Z direction)
            *pFloat++ = vpLeft;  *pFloat++ = vpTop;    *pFloat++ = -mNearExtent;
            *pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearExtent;

            *pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearExtent;
            *pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;

            *pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;
            *pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;

            *pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;
            *pFloat++ = vpLeft;  *pFloat++ = vpTop;    *pFloat++ = -mNearExtent;

            // far plane (remember frustum is going in -Z direction)
            *pFloat++ = farLeft;  *pFloat++ = farTop;    *pFloat++ = -farDist;
            *pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;

            *pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;
            *pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;

            *pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;
            *pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;

            *pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;
            *pFloat++ = farLeft;  *pFloat++ = farTop;    *pFloat++ = -farDist;

            // Sides of the pyramid
            *pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
            *pFloat++ = vpLeft;  *pFloat++ = vpTop;  *pFloat++ = -mNearExtent;

            *pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
            *pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearExtent;

            *pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
            *pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;

            *pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
            *pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;

            // Sides of the box

            *pFloat++ = vpLeft;  *pFloat++ = vpTop;  *pFloat++ = -mNearExtent;
            *pFloat++ = farLeft;  *pFloat++ = farTop;  *pFloat++ = -farDist;

            *pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearExtent;
            *pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;

            *pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;
            *pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;

            *pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearExtent;
            *pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;

            vbuf->unlock();

            mDebugVertexValid = true;
        }

        op.mType = GeometryRaw::OT_LINE_LIST;
        op.mVertex = &mVertexData;
        op.remove(GRT_Index | GRT_Instancing);
    }
    //-----------------------------------------------------------------------
    void Frustum::getMatrix(Matrix4f * out, NCount & count) const
    {
        if(mSpaceNode)
            *out = mSpaceNode->getSpaceMatrix();
        else
            *out = Matrix4f::IDENTITY;
        count = 1;
    }
    //-----------------------------------------------------------------------
    NIIf Frustum::distanceSquared(const Camera * cam) const
    {
        // Calc from centre
        if(mSpaceNode)
            return (cam->getSpacePos() - mSpaceNode->getSpacePos()).lengthSquared();
        else
            return 0;
    }
    //-----------------------------------------------------------------------
    void Frustum::getLight(LightList & ll) const
    {
        ll.clear();
    }
    //-----------------------------------------------------------------------
    void Frustum::_notify(Camera * cam)
    {
        // 确保边界盒更新
        updateProj();
        SpaceObj::_notify(cam);
    }
    // -------------------------------------------------------------------
    void Frustum::notifyProj() const
    {
        mProjValid = false;
        mPlaneValid = false;
        mDebugVertexValid = false;
        mPlaneIntersectValid = false;
    }
    // -------------------------------------------------------------------
    void Frustum::notifyView() const
    {
        mViewValid = false;
        mPlaneValid = false;
        mPlaneIntersectValid = false;
    }
    //-------------------------------------------------------------------
    void Frustum::updateViewMatrix(const Vector3f & pos, const Quaternion & dir, const Matrix4f * space)
    {
        // View matrix is:
        //
        //  [ Lx  Uy  Dz  Tx  ]
        //  [ Lx  Uy  Dz  Ty  ]
        //  [ Lx  Uy  Dz  Tz  ]
        //  [ 0   0   0   1   ]
        //
        // Where T = -(Transposed(Rot) * Pos)

        // This is most efficiently done using 3x3 Matrices
        Matrix3f rot;
        dir.to(rot);

        // Make the translation relative to new axes
        Matrix3f rotT = rot.T();
        Vector3f trans = -rotT * pos;

        // Make final matrix
        mViewMatrix = Matrix4f::IDENTITY;
        mViewMatrix = rotT; // fills upper 3x3
        mViewMatrix[0][3] = trans.x;
        mViewMatrix[1][3] = trans.y;
        mViewMatrix[2][3] = trans.z;

        // Deal with reflections
        if(space)
        {
            mViewMatrix = mViewMatrix * (*space);
        }
    }
    //-----------------------------------------------------------------------
    const Vector3f & Frustum::getNodePos() const
    {
        return mLastNodePos;
    }
    //-----------------------------------------------------------------------
    const Quaternion & Frustum::getNodeOri() const
    {
        return mLastNodeDir;
    }
    //-----------------------------------------------------------------------
    bool Frustum::project(const Sphere & sphere, Rectf & rect) const
    {
        // See http://www.gamasutra.com/features/20021011/lengyel_06.htm
        // Transform light position into camera space

        const_cast<Frustum *>(this)->updateView();
        Vector3f eyeSpacePos = mViewMatrix.affine(sphere.getCenter());

        // init
        rect.mLeft = rect.mBottom = -1.0f;
        rect.mRight = rect.mTop = 1.0f;

        if(eyeSpacePos.z < 0)
        {
            const_cast<Frustum *>(this)->updateProj();
            const Matrix4f& projMatrix = getProjMatrix();
            NIIf r = sphere.getRadius();
            NIIf rsq = r * r;

            // early-exit
            if (eyeSpacePos.lengthSquared() <= rsq)
                return false;

            NIIf Lxz = Math::Sqr(eyeSpacePos.x) + Math::Sqr(eyeSpacePos.z);
            NIIf Lyz = Math::Sqr(eyeSpacePos.y) + Math::Sqr(eyeSpacePos.z);

            // Find the tangent planes to the sphere
            // XZ first
            // calculate quadratic discriminant: b*b - 4ac
            // x = Nx
            // a = Lx^2 + Lz^2
            // b = -2rLx
            // c = r^2 - Lz^2
            NIIf a = Lxz;
            NIIf b = -2.0f * r * eyeSpacePos.x;
            NIIf c = rsq - Math::Sqr(eyeSpacePos.z);
            NIIf D = b*b - 4.0f*a*c;

            // two roots?
            if (D > 0)
            {
                NIIf sqrootD = Math::sqrt(D);
                // solve the quadratic to get the components of the normal
                NIIf Nx0 = (-b + sqrootD) / (2 * a);
                NIIf Nx1 = (-b - sqrootD) / (2 * a);

                // Derive Z from this
                NIIf Nz0 = (r - Nx0 * eyeSpacePos.x) / eyeSpacePos.z;
                NIIf Nz1 = (r - Nx1 * eyeSpacePos.x) / eyeSpacePos.z;

                // Get the point of tangency
                // Only consider points of tangency in front of the camera
                NIIf Pz0 = (Lxz - rsq) / (eyeSpacePos.z - ((Nz0 / Nx0) * eyeSpacePos.x));
                if (Pz0 < 0)
                {
                    // Project point onto near plane in worldspace
                    NIIf nearx0 = (Nz0 * mNearExtent) / Nx0;
                    // now we need to map this to viewport coords
                    // use projection matrix since that will take into account all factors
                    Vector3f relx0 = projMatrix * Vector3f(nearx0, 0, -mNearExtent);

                    // find out whether this is a left side or right side
                    NIIf Px0 = -(Pz0 * Nz0) / Nx0;
                    if (Px0 > eyeSpacePos.x)
                    {
                        rect.mRight = std::min(rect.mRight, relx0.x);
                    }
                    else
                    {
                        rect.mLeft = std::max(rect.mLeft, relx0.x);
                    }
                }
                NIIf Pz1 = (Lxz - rsq) / (eyeSpacePos.z - ((Nz1 / Nx1) * eyeSpacePos.x));
                if (Pz1 < 0)
                {
                    // Project point onto near plane in worldspace
                    NIIf nearx1 = (Nz1 * mNearExtent) / Nx1;
                    // now we need to map this to viewport coords
                    // use projection matrix since that will take into account all factors
                    Vector3f relx1 = projMatrix * Vector3f(nearx1, 0, -mNearExtent);

                    // find out whether this is a left side or right side
                    NIIf Px1 = -(Pz1 * Nz1) / Nx1;
                    if(Px1 > eyeSpacePos.x)
                    {
                        rect.mRight = std::min(rect.mRight, relx1.x);
                    }
                    else
                    {
                        rect.mLeft = std::max(rect.mLeft, relx1.x);
                    }
                }
            }

            // Now YZ
            // calculate quadratic discriminant: b*b - 4ac
            // x = Ny
            // a = Ly^2 + Lz^2
            // b = -2rLy
            // c = r^2 - Lz^2
            a = Lyz;
            b = -2.0f * r * eyeSpacePos.y;
            c = rsq - Math::Sqr(eyeSpacePos.z);
            D = b*b - 4.0f*a*c;

            // two roots?
            if (D > 0)
            {
                NIIf sqrootD = Math::sqrt(D);
                // solve the quadratic to get the components of the normal
                NIIf Ny0 = (-b + sqrootD) / (2 * a);
                NIIf Ny1 = (-b - sqrootD) / (2 * a);

                // Derive Z from this
                NIIf Nz0 = (r - Ny0 * eyeSpacePos.y) / eyeSpacePos.z;
                NIIf Nz1 = (r - Ny1 * eyeSpacePos.y) / eyeSpacePos.z;

                // Get the point of tangency
                // Only consider points of tangency in front of the camera
                NIIf Pz0 = (Lyz - rsq) / (eyeSpacePos.z - ((Nz0 / Ny0) * eyeSpacePos.y));
                if (Pz0 < 0)
                {
                    // Project point onto near plane in worldspace
                    NIIf neary0 = (Nz0 * mNearExtent) / Ny0;
                    // now we need to map this to viewport coords
                    // use projection matriy since that will take into account all factors
                    Vector3f rely0 = projMatrix * Vector3f(0, neary0, -mNearExtent);

                    // find out whether this is a top side or bottom side
                    NIIf Py0 = -(Pz0 * Nz0) / Ny0;
                    if (Py0 > eyeSpacePos.y)
                    {
                        rect.mTop = std::min(rect.mTop, rely0.y);
                    }
                    else
                    {
                        rect.mBottom = std::max(rect.mBottom, rely0.y);
                    }
                }
                NIIf Pz1 = (Lyz - rsq) / (eyeSpacePos.z - ((Nz1 / Ny1) * eyeSpacePos.y));
                if (Pz1 < 0)
                {
                    // Project point onto near plane in worldspace
                    NIIf neary1 = (Nz1 * mNearExtent) / Ny1;
                    // now we need to map this to viewport coords
                    // use projection matriy since that will take into account all factors
                    Vector3f rely1 = projMatrix * Vector3f(0, neary1, -mNearExtent);

                    // find out whether this is a top side or bottom side
                    NIIf Py1 = -(Pz1 * Nz1) / Ny1;
                    if (Py1 > eyeSpacePos.y)
                    {
                        rect.mTop = std::min(rect.mTop, rely1.y);
                    }
                    else
                    {
                        rect.mBottom = std::max(rect.mBottom, rely1.y);
                    }
                }
            }
        }
        return (rect.mLeft != -1.0f) || (rect.mTop != 1.0f) ||
            (rect.mRight != 1.0f) || (rect.mBottom != -1.0f);
    }
    //---------------------------------------------------------------------
    void Frustum::setCustomNearClip(bool b, const Plane & plane)
    {
        mObliqueProjPlane = plane;
        if(b)
        {
            mOblique = b;
        }
        else
        {
            mOblique = b;
            notifyProj();
        }
    }
    //---------------------------------------------------------------------
    void Frustum::setCustomViewMatrix(const Matrix4f & view)
    {
        if(mCustomViewMatrix)
        {
            N_assert(view.isAffine(), "error");
            mViewMatrix = view;
        }
        notifyView();
    }
    //---------------------------------------------------------------------
    void Frustum::setCustomProjMatrix(const Matrix4f & proj)
    {
        if(mCustomProjMatrix)
        {
            mProjMatrix = proj;
        }
        notifyProj();
    }
    //---------------------------------------------------------------------
    void Frustum::setOrthoAspect(NIIf w, NIIf h)
    {
        mOrthoWidth = w;
        mOrthoHeight = h;
        mAspect = w / h;
        notifyProj();
    }
    //---------------------------------------------------------------------
    void Frustum::query(GeometryQuery * query)
    {
        // Only displayed in debug
        if(query->isDebugMode())
        {
            query->query(this);
        }
    }
    //---------------------------------------------------------------------
    void Frustum::setCustomAspect(bool b)
    {
        if(b != mCustomAspect)
        {
            mCustomAspect = b;
            notifyProj();
        }
    }
    //---------------------------------------------------------------------
    void Frustum::setCustomAspect(NIIf left, NIIf right, NIIf top, NIIf bottom)
    {
        mCustomAspect = true;
        mLeft = left;
        mRight = right;
        mTop = top;
        mBottom = bottom;

        notifyProj();
    }
    //---------------------------------------------------------------------
    void Frustum::setDirection(NIIf dir)
    {
#if NII_VIEWPORT_DIR_NONE != 0
        N_EXCEPT(UnImpl, _I18n("Setting Frustrum orientation mode is not supported"));
#endif
        mDirection = dir;
        notifyProj();
    }
    //---------------------------------------------------------------------
} // namespace NII