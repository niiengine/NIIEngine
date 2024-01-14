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
#include "NiiCamera.h"
#include "NiiMath.h"
#include "NiiMatrix3.h"
#include "NiiSpaceManager.h"
#include "NiiSpaceNode.h"
#include "NiiAABox.h"
#include "NiiSphere.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiViewport.h"
#include "NiiCommon.h"
#include "NiiLight.h"
#include "NiiShadowMappingPattern.h"

#if N_PLAT == N_PLAT_IOS
    #include "macUtils.h"
#endif

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CameraListener
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CameraListener::~CameraListener()
    {
    }
    //------------------------------------------------------------------------
    void CameraListener::onInit(Camera *)
    {
    }
    //------------------------------------------------------------------------
    void CameraListener::onDestroy(Camera *)
    {
    }
    //------------------------------------------------------------------------
    void CameraListener::onCull(Camera *)
    {
    }
    //------------------------------------------------------------------------
    void CameraListener::onRender(Camera *)
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Camera
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Camera::Camera(SpaceID sid, SpaceManager * sm) :
        Frustum(sid, sm),
        mOrientation(Quaternion::IDENTITY),
        mPosition(Vector3f::ZERO),
        mPolygonMode(PM_SOLID),
        mFocusNode(0),
        mFocusOffset(Vector3f::ZERO),
        mLodFactor(1.0f),
        mPixelRatio(0),
        mViewport(0),
        mCull(0),
        mLod(0),
        mAutoAspectRatio(false),
        mSubMode(false),
        mExpandMode(false),
        mDisLimit(true),
        mPixelLimit(false)
    {
        // Reasonable defaults to camera params
        mFOV = Radian(Math::PI/4.0f);
        mNearExtent = 100.0f;
        mFarExtent = 100000.0f;
        mAspect = 1.33333333333333f;
        mProjType = Frustum::T_Perspective;
        setCustomYaw(true);    // Default to fixed yaw, like freelook since most people expect this

        notifyProj();
        notifyView();

        // Init matrices
        mViewMatrix = Matrix4f::ZERO;
        mSysProjMatrix = Matrix4f::ZERO;

        mSpaceNode = 0;
        // no reflection
        mReflect = false;
        mVisible = false;
        resetBound();
    }
    //-----------------------------------------------------------------------
    Camera::~Camera()
    {
        CameraListenerList::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onDestroy(this);
        }
    }
    //-----------------------------------------------------------------------
    void Camera::lookAt(const Vector3f & p)
    {
        updateView();
        setDirection(p - mLocalPos);
    }
    //-----------------------------------------------------------------------
    void Camera::setDirection(const Vector3f & vec)
    {
        // Do nothing if given a zero vector
        // (Replaced N_assert since this could happen with auto tracking camera and
        //  camera passes through the lookAt point)
        if(vec == Vector3f::ZERO)
            return;

        // Remember, camera points down -Z of local axes!
        // Therefore reverse direction of direction vector before determining local Z
        Vector3f zAdjustVec = -vec;
        zAdjustVec.normalise();

        Quaternion targetWorldOrientation;

        if(mCustomYaw)
        {
            Vector3f xVec = mCustomYawAxis.crossProduct(zAdjustVec);
            xVec.normalise();

            Vector3f yVec = zAdjustVec.crossProduct(xVec);
            yVec.normalise();

            targetWorldOrientation.from(xVec, yVec, zAdjustVec);
        }
        else
        {
            // Get axes from current quaternion
            Vector3f axes[3];
            updateView();
            mLocalOri.to(axes);
            Quaternion rotQuat;
            if((axes[2] + zAdjustVec).lengthSquared() < 0.00005f)
            {
                // Oops, a 180 degree turn (infinite possible rotation axes)
                // Default to yaw i.e. use current UP
                rotQuat.from(Radian(Math::PI), axes[1]);
            }
            else
            {
                // Derive shortest arc to new direction
                rotQuat = Quaternion::getRotation(axes[2], zAdjustVec);
            }
            targetWorldOrientation = rotQuat * mLocalOri;
        }

        // transform to parent space
        if(mSpaceNode)
        {
            mOrientation = mSpaceNode->getSpaceOri().inverse() * targetWorldOrientation;
        }
        else
        {
            mOrientation = targetWorldOrientation;
        }

        // TODO If we have a fixed yaw axis, we mustn't break it by using the
        // shortest arc because this will sometimes cause a relative yaw
        // which will tip the camera

        notifyView();
    }
    //-----------------------------------------------------------------------
    void Camera::roll(const Radian & angle)
    {
        // Rotate around local Z axis
        Vector3f z = mOrientation * Vector3f::Z;
        rotate(z, angle);

        notifyView();
    }
    //-----------------------------------------------------------------------
    void Camera::yaw(const Radian & angle)
    {
        Vector3f y;

        if(mCustomYaw)
        {
            // Rotate around fixed yaw axis
            y = mCustomYawAxis;
        }
        else
        {
            // Rotate around local Y axis
            y = mOrientation * Vector3f::Y;
        }

        rotate(y, angle);
        notifyView();
    }
    //-----------------------------------------------------------------------
    void Camera::pitch(const Radian & angle)
    {
        // Rotate around local X axis
        Vector3f x = mOrientation * Vector3f::X;
        rotate(x, angle);

        notifyView();
    }
    //-----------------------------------------------------------------------
    void Camera::rotate(const Quaternion & q)
    {
        // Note the order of the mult, i.e. q comes after

        // Normalise the quat to avoid cumulative problems with precision
        Quaternion qnorm = q;
        qnorm.normalise();
        mOrientation = qnorm * mOrientation;

        notifyView();
    }
    //-----------------------------------------------------------------------
    void Camera::rotate(const Vector3f & axis, const Radian & angle)
    {
        Quaternion q;
        q.from(angle, axis);
        rotate(q);
    }
    //-----------------------------------------------------------------------
    bool Camera::isViewValid() const
    {
        // Overridden from Frustum to use local orientation / position offsets
        // Attached to node?
        if(mSpaceNode != 0)
        {
            if(!mViewValid || mSpaceNode->getSpaceOri() != mLastNodeDir ||
                mSpaceNode->getSpacePos() != mLastNodePos)
            {
                // Ok, we're out of date with SpaceNode we're attached to
                mLastNodeDir = mSpaceNode->getSpaceOri();
                mLastNodePos = mSpaceNode->getSpacePos();
                mLocalOri = mLastNodeDir * mOrientation;
                mLocalPos = (mLastNodeDir * mPosition) + mLastNodePos;
                mViewValid = false;
                mSubClipDirty = true;
            }
        }
        else
        {
            // Rely on own updates
            mLocalOri = mOrientation;
            mLocalPos = mPosition;
        }

        if(mReflect && !(mLastReflectPlane == mReflectPlane))
        {
            mReflectPlane.getReflect(&mReflectMatrix);
            mLastReflectPlane = mReflectPlane;
            mViewValid = false;
            mSubClipDirty = true;
        }

        // Deriving reflected orientation / position
        if(!mViewValid)
        {
            if(mReflect)
            {
                // Calculate reflected orientation, use up-vector as fallback axis.
                Vector3f dir = mLocalOri * Vector3f::INV_Z;
                Vector3f rdir = dir.reflect(mReflectPlane.mNormal);
                Vector3f up = mLocalOri * Vector3f::Y;
                mSpaceOrientation = Quaternion::getRotation(dir, rdir, up) * mLocalOri;

                // Calculate reflected position.
                mSpacePos = mReflectMatrix.affine(mLocalPos);
            }
            else
            {
                mSpaceOrientation = mLocalOri;
                mSpacePos = mLocalPos;
            }
        }
        return mViewValid;
    }
    // -------------------------------------------------------------------
    void Camera::notifyView() const
    {
        mSubClipDirty = true;
        Frustum::notifyView();
    }
    // -------------------------------------------------------------------
    void Camera::notifyProj() const
    {
        mSubClipDirty = true;
        Frustum::notifyProj();
    }
    //-----------------------------------------------------------------------
    void Camera::cull(Viewport * vp, const RenderQueueFusion * rqfusion)
    {
        //update the pixel display ratio
        if(mProjType == Frustum::T_Perspective)
        {
            mPixelRatio = (2 * Math::Tan(mFOV * 0.5f)) / vp->getPixelHeight();
        }
        else
        {
            mPixelRatio = (mTop - mBottom) / vp->getPixelHeight();
        }
        CameraListenerList::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onCull(this);
        }
        
    }
    //-----------------------------------------------------------------------
    void Camera::render(Viewport * vp, const RenderQueueFusion * rqfusion)
    {
        mManager->renderSpace(this, vp, rqfusion);

        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onRender(this);
        }
    }
    //---------------------------------------------------------------------
    void Camera::add(CameraListener * l)
    {
        if(std::find(mListeners.begin(), mListeners.end(), l) == mListeners.end())
        {
            mListeners.push_back(l);
            l->onInit(this);
        }
    }
    //---------------------------------------------------------------------
    void Camera::remove(CameraListener * l)
    {
        CameraListenerList::iterator i = std::find(mListeners.begin(), mListeners.end(), l);
        if (i != mListeners.end())
        {
            mListeners.erase(i);
        }
    }
    //---------------------------------------------------------------------
    void Camera::setCustomYaw(bool enable, const Vector3f & o)
    {
        mCustomYaw = enable;
        mCustomYawAxis = o;
    }
    //-----------------------------------------------------------------------
    void Camera::setOri(const Quaternion & q)
    {
        mOrientation = q;
        mOrientation.normalise();
        notifyView();
    }
    //-----------------------------------------------------------------------
    const Vector3f & Camera::getSpacePos() const
    {
        const_cast<Camera *>(this)->updateView();
        return mSpacePos;
    }
    //-----------------------------------------------------------------------
    const Quaternion & Camera::getSpaceOri() const
    {
        const_cast<Camera *>(this)->updateView();
        return mSpaceOrientation;
    }
    //-----------------------------------------------------------------------
    const Vector3f & Camera::getSpaceDirection() const
    {
        // Direction points down -Z
        const_cast<Camera *>(this)->updateView();
        mSpaceDirection = mSpaceOrientation * Vector3f::INV_Z;
        return mSpaceDirection;
    }
    //-----------------------------------------------------------------------
    void Camera::getMatrix(Matrix4f * out, NCount & count) const
    {
        const_cast<Camera *>(this)->updateView();

        Vector3f scale(1.0, 1.0, 1.0);
        if(mSpaceNode)
            scale = mSpaceNode->getSpaceScale();

        out->set(mSpacePos, scale, mSpaceOrientation);
        count = 1;
    }
    //-----------------------------------------------------------------------
    FactoryID Camera::getFactoryID() const
    {
        return N_FACTORY_Camera;
    }
    //-----------------------------------------------------------------------
    void Camera::setAutoFocus(PosNode * obj, const Vector3f & offset)
    {
        mFocusNode = obj;
        mFocusOffset = offset;
    }
    //-----------------------------------------------------------------------
    void Camera::updateFocus()
    {
        // NB assumes that all scene nodes have been updated
        if(mFocusNode && mAutoFocus)
        {
            lookAt(mFocusNode->getSpacePos() + mFocusOffset);
        }
    }
    //-----------------------------------------------------------------------
    Ray Camera::getRay(NIIf screenX, NIIf screenY) const
    {
        Ray ret;

        Matrix4f inverseVP = (getProjMatrix() * getViewMatrix(false)).inverse();
#if NII_VIEWPORT_DIR_NONE == 0
        // We need to convert screen point to our oriented viewport (temp solution)
        NIIf tX = screenX;
        NIIf a = Frustum::getDirection() * Math::Rad_Ang;
        screenX = Math::Cos(a) * (tX-0.5f) + Math::Sin(a) * (screenY-0.5f) + 0.5f;
        screenY = Math::Sin(a) * (tX-0.5f) + Math::Cos(a) * (screenY-0.5f) + 0.5f;
        if(Frustum::getDirection() == 90 || Frustum::getDirection() == 270)
            screenY = 1.f - screenY;
#endif

        NIIf nx = (2.0f * screenX) - 1.0f;
        NIIf ny = 1.0f - (2.0f * screenY);
        Vector3f nearPoint(nx, ny, -1.f);
        // Use midPoint rather than far point to avoid issues with infinite projection
        Vector3f midPoint (nx, ny,  0.0f);

        // Get ray origin and ray target on near plane in world space
        Vector3f rayOrigin, rayTarget;

        rayOrigin = inverseVP * nearPoint;
        rayTarget = inverseVP * midPoint;

        Vector3f rayDirection = rayTarget - rayOrigin;
        rayDirection.normalise();

        ret.setOrigin(rayOrigin);
        ret.setDirection(rayDirection);
        return ret;
    }
    //---------------------------------------------------------------------
    void Camera::getVolume(PlaneSpace & plans, NIIf screenLeft, NIIf screenTop, NIIf screenRight, NIIf screenBottom)
    {
        if(mProjType == Frustum::T_Perspective)
        {
            // Use the corner rays to generate planes
            Ray ul = getRay(screenLeft, screenTop);
            Ray ur = getRay(screenRight, screenTop);
            Ray bl = getRay(screenLeft, screenBottom);
            Ray br = getRay(screenRight, screenBottom);

            Vector3f normal;
            // top plane
            normal = ul.getDirection().crossProduct(ur.getDirection());
            normal.normalise();
            plans.mPlanes.push_back(Plane(normal, getSpacePos()));

            // right plane
            normal = ur.getDirection().crossProduct(br.getDirection());
            normal.normalise();
            plans.mPlanes.push_back(Plane(normal, getSpacePos()));

            // bottom plane
            normal = br.getDirection().crossProduct(bl.getDirection());
            normal.normalise();
            plans.mPlanes.push_back(Plane(normal, getSpacePos()));

            // left plane
            normal = bl.getDirection().crossProduct(ul.getDirection());
            normal.normalise();
            plans.mPlanes.push_back(Plane(normal, getSpacePos()));
        }
        else
        {
            // ortho planes are parallel to frustum planes
            Ray ul = getRay(screenLeft, screenTop);
            Ray br = getRay(screenRight, screenBottom);

            updateFrustumPlanes();
            plans.mPlanes.push_back(Plane(mFrustumPlanes[Frustum::PT_Top].mNormal, ul.getOrigin()));
            plans.mPlanes.push_back(Plane(mFrustumPlanes[Frustum::PT_Right].mNormal, br.getOrigin()));
            plans.mPlanes.push_back(Plane(mFrustumPlanes[Frustum::PT_Bottom].mNormal, br.getOrigin()));
            plans.mPlanes.push_back(Plane(mFrustumPlanes[Frustum::PT_Left].mNormal, ul.getOrigin()));
        }

        // near & far plane applicable to both projection types
        plans.mPlanes.push_back(getClipPlane(Frustum::PT_Near));
        plans.mPlanes.push_back(getClipPlane(Frustum::PT_Far));
    }
    //---------------------------------------------------------------------
    void Camera::resetBound()
    {
        mAABB[ShadowType::ST_Normal].setNull();
        mAABB[ShadowType::ST_Cast].setNull();
        mAABB[ShadowType::ST_Receive].setNull();
        mMin[ShadowType::ST_Normal] = mMin[ShadowType::ST_Cast] = mMin[ShadowType::ST_Receive] = N_TYPE_INFINITY(NIIf);
        mMax[ShadowType::ST_Normal] = mMax[ShadowType::ST_Cast] = mMax[ShadowType::ST_Receive] = 0;
    }
    //---------------------------------------------------------------------
    void Camera::expandBound(const AABox & b1, const Sphere & b2, Nmark mark)
    {
        if(mExpandMode)
        {
            mAABB[ShadowType::ST_Normal].merge(b1);
            if(mark & ShadowType::ST_Receive)
            {
                mAABB[ShadowType::ST_Receive].merge(b1);
            }
            // 使用视图矩阵来确定距离,在自定义视图矩阵中工作
            Vector3f spos = getViewMatrix(false) * b2.getCenter();
            NIIf camDistToCenter = spos.length();
            if(mark & ShadowType::ST_Normal)
            {
                mMin[ShadowType::ST_Normal] = std::min(mMin[ShadowType::ST_Normal], std::max((NIIf)0, camDistToCenter - b2.getRadius()));
                mMax[ShadowType::ST_Normal] = std::max(mMax[ShadowType::ST_Normal], camDistToCenter + b2.getRadius());
            }
            if(mark & ShadowType::ST_Cast)
            {
                mMin[ShadowType::ST_Cast] = std::min(mMin[ShadowType::ST_Cast], std::max((NIIf)0, camDistToCenter - b2.getRadius()));
                mMax[ShadowType::ST_Cast] = std::max(mMax[ShadowType::ST_Cast], camDistToCenter + b2.getRadius());
            }
        }
    }
    // -------------------------------------------------------------------
    void Camera::setSubView(NIIf left, NIIf top, NIIf right, NIIf bottom)
    {
        mSubLeft = left;
        mSubTop = top;
        mSubRight = right;
        mSubBottom = bottom;

        mSubClipDirty = true;
    }
    // -------------------------------------------------------------------
    const PlaneList & Camera::getSubClip()
    {
        updateView();

        if(!mSubMode || !mSubClipDirty)
            return mSubClips;

        // Calculate general projection parameters
        NIIf vpLeft, vpRight, vpBottom, vpTop;
        updateAspect(vpLeft, vpRight, vpBottom, vpTop);

        NIIf vpWidth = vpRight - vpLeft;
        NIIf vpHeight = vpTop - vpBottom;

        NIIf wvpLeft   = vpLeft + mSubLeft * vpWidth;
        NIIf wvpRight  = vpLeft + mSubRight * vpWidth;
        NIIf wvpTop    = vpTop - mSubTop * vpHeight;
        NIIf wvpBottom = vpTop - mSubBottom * vpHeight;

        Vector3f vp_ul (wvpLeft, wvpTop, -mNearExtent);
        Vector3f vp_ur (wvpRight, wvpTop, -mNearExtent);
        Vector3f vp_bl (wvpLeft, wvpBottom, -mNearExtent);
        Vector3f vp_br (wvpRight, wvpBottom, -mNearExtent);

        Matrix4f inv = mViewMatrix.inverseAffine();

        Vector3f vw_ul = inv.affine(vp_ul);
        Vector3f vw_ur = inv.affine(vp_ur);
        Vector3f vw_bl = inv.affine(vp_bl);
        Vector3f vw_br = inv.affine(vp_br);

        mSubClips.clear();
        if(mProjType == Frustum::T_Perspective)
        {
            Vector3f position = getNodePos();
            mSubClips.push_back(Plane(position, vw_bl, vw_ul));
            mSubClips.push_back(Plane(position, vw_ul, vw_ur));
            mSubClips.push_back(Plane(position, vw_ur, vw_br));
            mSubClips.push_back(Plane(position, vw_br, vw_bl));
        }
        else
        {
            Vector3f x_axis(inv[0][0], inv[0][1], inv[0][2]);
            Vector3f y_axis(inv[1][0], inv[1][1], inv[1][2]);
            x_axis.normalise();
            y_axis.normalise();
            mSubClips.push_back(Plane( x_axis, vw_bl));
            mSubClips.push_back(Plane(-x_axis, vw_ur));
            mSubClips.push_back(Plane( y_axis, vw_bl));
            mSubClips.push_back(Plane(-y_axis, vw_ur));
        }

        mSubClipDirty = false;
        return mSubClips;
    }
    // -------------------------------------------------------------------
    NIIf Camera::getCenterRange() const
    {
        // return a little bigger than the near distance
        // just to keep things just outside
        return mNearExtent * 1.5f;
    }
    //--------------------------------------------------------------------
    const Vector3f & Camera::getNodePos() const
    {
        // Note no update, because we're calling this from the update!
        return mLocalPos;
    }
    //--------------------------------------------------------------------
    const Quaternion & Camera::getNodeOri() const
    {
        return mLocalOri;
    }
    //--------------------------------------------------------------------
    bool Camera::isIntersect(const AABox & bound) const
    {
        if(mCull)
        {
            return mCull->isIntersect(bound);
        }
        else
        {
            return Frustum::isIntersect(bound);
        }
    }
    //--------------------------------------------------------------------
    bool Camera::isIntersect(const Sphere & bound) const
    {
        if (mCull)
        {
            return mCull->isIntersect(bound);
        }
        else
        {
            return Frustum::isIntersect(bound);
        }
    }
    //-----------------------------------------------------------------------
    bool Camera::isIntersect(const Vector3f & vert) const
    {
        if (mCull)
        {
            return mCull->isIntersect(vert);
        }
        else
        {
            return Frustum::isIntersect(vert);
        }
    }
    //-----------------------------------------------------------------------
    const Vector3f * Camera::getClipPlaneIntersect() const
    {
        if(mCull)
        {
            return mCull->getClipPlaneIntersect();
        }
        else
        {
            return Frustum::getClipPlaneIntersect();
        }
    }
    //-----------------------------------------------------------------------
    const Plane & Camera::getClipPlane(Frustum::PlaneType plane) const
    {
        if(mCull)
        {
            return mCull->getClipPlane(plane);
        }
        else
        {
            return Frustum::getClipPlane(plane);
        }
    }
    //-----------------------------------------------------------------------
    bool Camera::project(const Sphere & sphere, Rectf & rect) const
    {
        if(mCull)
        {
            return mCull->project(sphere, rect);
        }
        else
        {
            return Frustum::project(sphere, rect);
        }
    }
    //-----------------------------------------------------------------------
    NIIf Camera::getNearClipDistance() const
    {
        if (mCull)
        {
            return mCull->getNearClipDistance();
        }
        else
        {
            return Frustum::getNearClipDistance();
        }
    }
    //-----------------------------------------------------------------------
    NIIf Camera::getFarClipDistance() const
    {
        if (mCull)
        {
            return mCull->getFarClipDistance();
        }
        else
        {
            return Frustum::getFarClipDistance();
        }
    }
    //-----------------------------------------------------------------------
    const Matrix4f & Camera::getViewMatrix() const
    {
        if(mCull)
        {
            return mCull->getViewMatrix();
        }
        else
        {
            return Frustum::getViewMatrix();
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CameraSetup
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CameraSetup::CameraSetup()
    {
    }
    //------------------------------------------------------------------------
    CameraSetup::~CameraSetup()
    {
    }
    //------------------------------------------------------------------------
    void CameraSetup::setupShadow(Camera * texCam, const SpaceManager * sm, 
        const Camera * cam, const Viewport * vp, const Light * light, NCount level) const
    {
        Vector3f pos, dir;

        // reset custom view / projection matrix in case already set
        texCam->setCustomViewMatrix(false);
        texCam->setCustomProjMatrix(false);
        texCam->setNearClipDistance(light->getShadowNearClip(cam->getNearClipDistance()));
        texCam->setFarClipDistance(light->getShadowFarClip(cam->getFarClipDistance()));

        // get the shadow frustum's far distance
        NIIf shadowDist = light->getShadowExtent();
        if(!shadowDist)
        {
            // need a shadow distance, make one up
            shadowDist = cam->getNearClipDistance() * 300;
        }
        NIIf shadowOffset = shadowDist *
            static_cast<ShadowMappingPattern *>(sm->getRenderPattern())->getExtentOffsetFactor();

        // Directional lights
        if(light->getType() == LT_DIRECTIONAL)
        {
            // set up the shadow texture
            // Set ortho projection
            texCam->setProjType(Frustum::T_Orthogonal);
            // set ortho window so that texture covers far dist
            texCam->setOrthoAspect(shadowDist * 2, shadowDist * 2);

            // Calculate look at position
            // We want to look at a spot shadowOffset away from near plane
            // 0.5 is a litle too close for angles
            Vector3f target = cam->getSpacePos() +
                (cam->getSpaceDirection() * shadowOffset);

            // Calculate direction, which same as directional light direction
            dir = - light->getSpaceDirection(); // backwards since point down -z
            dir.normalise();

            // Calculate position
            // We want to be in the -ve direction of the light direction
            // far enough to project for the dir light extrusion distance
            pos = target + dir * sm->getRenderPattern()->getShadowExtent();

            // Round local x/y position based on a world-space texel; this helps to reduce
            // jittering caused by the projection moving with the camera
            // Viewport is 2 * near clip distance across (90 degree fov)
            //~ NIIf worldTexelSize = (texCam->getNearClipDistance() * 20) / vp->getPixelWidth();
            //~ pos.x -= fmod(pos.x, worldTexelSize);
            //~ pos.y -= fmod(pos.y, worldTexelSize);
            //~ pos.z -= fmod(pos.z, worldTexelSize);
            NIIf worldTexelSize = (shadowDist * 2) / texCam->getViewport()->getPixelWidth();

             //get texCam orientation

             Vector3f up = Vector3f::Y;
             // Check it's not coincident with dir
             if(Math::Abs(up.dotProduct(dir)) >= 1.0f)
             {
                // Use camera up
                up = Vector3f::Z;
             }
             // cross twice to rederive, only direction is unaltered
             Vector3f left = dir.crossProduct(up);
             left.normalise();
             up = dir.crossProduct(left);
             up.normalise();
             // Derive quaternion from axes
             Quaternion q;
             q.from(left, up, dir);

             //convert world space camera position into light space
             Vector3f lightSpacePos = q.inverse() * pos;

             //snap to nearest texel
             lightSpacePos.x -= fmod(lightSpacePos.x, worldTexelSize);
             lightSpacePos.y -= fmod(lightSpacePos.y, worldTexelSize);

             //convert back to world space
             pos = q * lightSpacePos;
        }
        // Spotlight
        else if(light->getType() == LT_SPOT)
        {
            // Set perspective projection
            texCam->setProjType(Frustum::T_Perspective);
            // set FOV slightly larger than the spotlight range to ensure coverage
            Radian fovy = light->getSpotlightOuter() * 1.2f;
            // limit angle
            if(fovy.getAngle() > 175)
                fovy = Angle(175);
            texCam->setFOV(fovy);

            // Calculate position, which same as spotlight position
            pos = light->getSpacePos();

            // Calculate direction, which same as spotlight direction
            dir = - light->getSpaceDirection(); // backwards since point down -z
            dir.normalise();
        }
        // Point light
        else
        {
            // Set perspective projection
            texCam->setProjType(Frustum::T_Perspective);
            // Use 120 degree FOV for point light to ensure coverage more area
            texCam->setFOV(Angle(120));

            // Calculate look at position
            // We want to look at a spot shadowOffset away from near plane
            // 0.5 is a litle too close for angles
            Vector3f target = cam->getSpacePos() + (cam->getSpaceDirection() * shadowOffset);

            // Calculate position, which same as point light position
            pos = light->getSpacePos();

            dir = (pos - target); // backwards since point down -z
            dir.normalise();
        }

        // Finally set position
        texCam->setPos(pos);

        // Calculate orientation based on direction calculated above
        /*
        // Next section (camera oriented shadow map) abandoned
        // Always point in the same direction, if we don't do this then
        // we get 'shadow swimming' as camera rotates
        // As it is, we get swimming on moving but this is less noticeable

        // calculate up vector, we want it aligned with cam direction
        Vector3 up = cam->getSpaceDirection();
        // Check it's not coincident with dir
        if (up.dotProduct(dir) >= 1.0f)
        {
        // Use camera up
        up = cam->getUp();
        }
        */
        Vector3f up = Vector3f::Y;
        // Check it's not coincident with dir
        if(Math::Abs(up.dotProduct(dir)) >= 1.0f)
        {
            // Use camera up
            up = Vector3f::Z;
        }
        // cross twice to rederive, only direction is unaltered
        Vector3f left = dir.crossProduct(up);
        left.normalise();
        up = dir.crossProduct(left);
        up.normalise();
        // Derive quaternion from axes
        Quaternion q;
        q.from(left, up, dir);
        texCam->setOri(q);
    }
    //------------------------------------------------------------------------------
}