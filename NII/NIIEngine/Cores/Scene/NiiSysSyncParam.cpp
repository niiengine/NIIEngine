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
#include "NiiSysSyncParam.h"
#include "NiiGeometryObj.h"
#include "NiiFrameObj.h"
#include "NiiEquationManager.h"
#include "NiiEngine.h"
#include "NiiRenderPattern.h"
#include "NiiAnimationManager.h"

namespace NII
{
    //-----------------------------------------------------------------------------
    const Matrix4f PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE(
        0.5f,    0.0f,    0.0f,  0.5f,
        0.0f,   -0.5f,    0.0f,  0.5f,
        0.0f,    0.0f,    1.0f,  0.0f,
        0.0f,    0.0f,    0.0f,  1.0f);
    //-----------------------------------------------------------------------------
    SysSyncParam::SysSyncParam() : 
        mWorldMatrixDirty(true),
         mViewMatrixDirty(true),
         mProjMatrixDirty(true),
         mWorldViewMatrixDirty(true),
         mViewProjMatrixDirty(true),
         mWorldViewProjMatrixDirty(true),
         mInverseWorldMatrixDirty(true),
         mInverseWorldViewMatrixDirty(true),
         mInverseTransposeWorldMatrixDirty(true),
         mInverseTransposeWorldViewMatrixDirty(true),
         mCameraPositionObjectSpaceDirty(true),
         mCameraPositionDirty(true),
         mInverseViewMatrixDirty(true),
         mSceneDepthRangeDirty(true),
         mLodCameraPositionDirty(true),
         mLodCameraPositionObjectSpaceDirty(true),
         mCurrentGeometryObj(0),
         mCurrentCamera(0),
         mCameraRelativeRendering(false),
         mCurrentLightList(0),
         mCurrentFBO(0),
         mCurrentViewport(0),
         mCurrentSpaceManager(0),
         mCurrentShaderCh(0)
    {
        mBlankLight.setDiffuse(Colour::Black);
        mBlankLight.setSpecular(Colour::Black);
        mBlankLight.setRange(0);
        mBlankLight.setAttenuationConstant(1);
        mBlankLight.setAttenuationLinear(0);
        mBlankLight.setAttenuationQuadric(0);
        for(NCount i = 0; i < NII_MAX_LIGHT; ++i)
        {
            mTextureViewProjMatrixDirty[i] = true;
            mTextureWorldViewProjMatrixDirty[i] = true;
            mSpotlightViewProjMatrixDirty[i] = true;
            mSpotlightWorldViewProjMatrixDirty[i] = true;
            mCurrentTextureProjector[i] = 0;
            mShadowCamDepthRangesDirty[i] = false;
        }

    }
    //-----------------------------------------------------------------------------
    SysSyncParam::~SysSyncParam()
    {
    }
    //-----------------------------------------------------------------------------
    Light * SysSyncParam::getLight(NCount index) const
    {
        // If outside light range, return a blank light to ensure zeroised for program
        if(mCurrentLightList && index < mCurrentLightList->size())
        {
            return (*mCurrentLightList)[index];
        }
        else
        {
            return const_cast<Light *>(&mBlankLight);
        }
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setCurrentRenderable(const GeometryObj * rend)
    {
        mCurrentGeometryObj = rend;
        mWorldMatrixDirty = true;
        mViewMatrixDirty = true;
        mProjMatrixDirty = true;
        mWorldViewMatrixDirty = true;
        mViewProjMatrixDirty = true;
        mWorldViewProjMatrixDirty = true;
        mInverseWorldMatrixDirty = true;
        mInverseViewMatrixDirty = true;
        mInverseWorldViewMatrixDirty = true;
        mInverseTransposeWorldMatrixDirty = true;
        mInverseTransposeWorldViewMatrixDirty = true;
        mCameraPositionObjectSpaceDirty = true;
        mLodCameraPositionObjectSpaceDirty = true;
        for(NCount i = 0; i < NII_MAX_LIGHT; ++i)
        {
            mTextureWorldViewProjMatrixDirty[i] = true;
            mSpotlightWorldViewProjMatrixDirty[i] = true;
        }
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setCurrentCamera(const Camera * cam, bool useCameraRelative)
    {
        mCurrentCamera = cam;
        mCameraRelativeRendering = useCameraRelative;
        mCameraRelativePosition = cam->getSpacePos();
        mViewMatrixDirty = true;
        mProjMatrixDirty = true;
        mWorldViewMatrixDirty = true;
        mViewProjMatrixDirty = true;
        mWorldViewProjMatrixDirty = true;
        mInverseViewMatrixDirty = true;
        mInverseWorldViewMatrixDirty = true;
        mInverseTransposeWorldViewMatrixDirty = true;
        mCameraPositionObjectSpaceDirty = true;
        mCameraPositionDirty = true;
        mLodCameraPositionObjectSpaceDirty = true;
        mLodCameraPositionDirty = true;
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setCurrentLightList(const LightList & ll)
    {
        mCurrentLightList = &ll;
        for(NCount i = 0; i < ll.size() && i < NII_MAX_LIGHT; ++i)
        {
            mSpotlightViewProjMatrixDirty[i] = true;
            mSpotlightWorldViewProjMatrixDirty[i] = true;
        }
    }
    //-----------------------------------------------------------------------------
    const Colour SysSyncParam::getLightDiffuseColourWithPower(NCount index) const
    {
        Light * l = getLight(index);
        Colour scaled(l->getDiffuse());
        NIIf power = l->getBrightFactor();
        // scale, but not alpha
        scaled.r *= power;
        scaled.g *= power;
        scaled.b *= power;
        return scaled;
    }
    //-----------------------------------------------------------------------------
    const Colour SysSyncParam::getLightSpecularColourWithPower(NCount index) const
    {
        Light * l = getLight(index);
        Colour scaled(l->getSpecular());
        NIIf power = l->getBrightFactor();
        // scale, but not alpha
        scaled.r *= power;
        scaled.g *= power;
        scaled.b *= power;
        return scaled;
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getLightAttenuation(NCount index) const
    {
        // range, const, linear, quad
        Light * l = getLight(index);
        return Vector4f(l->getRange(), l->getAttenuationConstant(),
            l->getAttenuationLinear(), l->getAttenuationQuadric());
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getSpotlightParams(NCount index) const
    {
        // inner, outer, fallof, isSpot
        Light * l = getLight(index);
        if (l->getType() == LT_SPOT)
        {
            return Vector4f(Math::Cos(l->getSpotlightInner().getValue() * 0.5f),
                Math::Cos(l->getSpotlightOuter().getValue() * 0.5f),
                    l->getSpotlightFalloff(), 1.0);
        }
        else
        {
            // Use safe values which result in no change to point & dir light calcs
            // The spot factor applied to the usual lighting calc is
            // pow((dot(spotDir, lightDir) - y) / (x - y), z)
            // Therefore if we set z to 0.0f then the factor will always be 1
            // since pow(anything, 0) == 1
            // However we also need to ensure we don't overflow because of the division
            // therefore set x = 1 and y = 0 so divisor doesn't change scale
            return Vector4f(1.0, 0.0, 0.0, 1.0); // since the main op is pow(.., vec4.z), this will result in 1.0
        }
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setWorldMatrices(const Matrix4f * m, NCount count)
    {
        mWorldMatrixArray = m;
        mWorldMatrixCount = count;
        mWorldMatrixDirty = false;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getWorldMatrix() const
    {
        if(mWorldMatrixDirty)
        {
            mWorldMatrixArray = mWorldMatrix;
            mCurrentGeometryObj->getMatrix(mWorldMatrix, mWorldMatrixCount);
            if(mCameraRelativeRendering && !mCurrentGeometryObj->isOn(GeometryObj::M_IdentityView))
            {
                for(NCount i = 0; i < mWorldMatrixCount; ++i)
                {
                    mWorldMatrix[i].setTrans(mWorldMatrix[i].getTrans() - mCameraRelativePosition);
                }
            }
            mWorldMatrixDirty = false;
        }
        return mWorldMatrixArray[0];
    }
    //-----------------------------------------------------------------------------
    NCount SysSyncParam::getWorldMatrixCount() const
    {
        // trigger derivation
        getWorldMatrix();
        return mWorldMatrixCount;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f* SysSyncParam::getWorldMatrixArray() const
    {
        // trigger derivation
        getWorldMatrix();
        return mWorldMatrixArray;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f& SysSyncParam::getViewMatrix() const
    {
        if (mViewMatrixDirty)
        {
            if (mCurrentGeometryObj && mCurrentGeometryObj->isOn(GeometryObj::M_IdentityView))
                mViewMatrix = Matrix4f::IDENTITY;
            else
            {
                mViewMatrix = mCurrentCamera->getViewMatrix(false);
                if (mCameraRelativeRendering)
                {
                    mViewMatrix.setTrans(Vector3f::ZERO);
                }

            }
            mViewMatrixDirty = false;
        }
        return mViewMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getViewProjectionMatrix() const
    {
        if(mViewProjMatrixDirty)
        {
            mViewProjMatrix = getProjMatrix() * getViewMatrix();
            mViewProjMatrixDirty = false;
        }
        return mViewProjMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getProjMatrix() const
    {
        if(mProjMatrixDirty)
        {
            // NB use API-independent projection matrix since GPU programs
            // bypass the API-specific handedness and use right-handed coords
            if(mCurrentGeometryObj && mCurrentGeometryObj->isOn(GeometryObj::M_IdentityProj))
            {
                // Use identity projection matrix, still need to take RS depth into account.
                N_Engine().getRender()->getSysProj(Matrix4f::IDENTITY, mProjectionMatrix, true);
            }
            else
            {
                N_Engine().getRender()->getSysProj(mCurrentCamera->getProjMatrix(), mProjectionMatrix, true);
            }
            if (mCurrentFBO && mCurrentFBO->isFlipping())
            {
                // Because we're not using setProjectionMatrix, this needs to be done here
                // Invert transformed y
                mProjectionMatrix[1][0] = -mProjectionMatrix[1][0];
                mProjectionMatrix[1][1] = -mProjectionMatrix[1][1];
                mProjectionMatrix[1][2] = -mProjectionMatrix[1][2];
                mProjectionMatrix[1][3] = -mProjectionMatrix[1][3];
            }
            mProjMatrixDirty = false;
        }
        return mProjectionMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getWorldViewMatrix() const
    {
        if(mWorldViewMatrixDirty)
        {
            mWorldViewMatrix = getViewMatrix().affine(getWorldMatrix());
            mWorldViewMatrixDirty = false;
        }
        return mWorldViewMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getWorldViewProjMatrix() const
    {
        if(mWorldViewProjMatrixDirty)
        {
            mWorldViewProjMatrix = getProjMatrix() * getWorldViewMatrix();
            mWorldViewProjMatrixDirty = false;
        }
        return mWorldViewProjMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getInverseWorldMatrix() const
    {
        if(mInverseWorldMatrixDirty)
        {
            mInverseWorldMatrix = getWorldMatrix().inverseAffine();
            mInverseWorldMatrixDirty = false;
        }
        return mInverseWorldMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f& SysSyncParam::getInverseWorldViewMatrix() const
    {
        if (mInverseWorldViewMatrixDirty)
        {
            mInverseWorldViewMatrix = getWorldViewMatrix().inverseAffine();
            mInverseWorldViewMatrixDirty = false;
        }
        return mInverseWorldViewMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f& SysSyncParam::getInverseViewMatrix() const
    {
        if (mInverseViewMatrixDirty)
        {
            mInverseViewMatrix = getViewMatrix().inverseAffine();
            mInverseViewMatrixDirty = false;
        }
        return mInverseViewMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getInverseTransposeWorldMatrix() const
    {
        if(mInverseTransposeWorldMatrixDirty)
        {
            mInverseTransposeWorldMatrix = getInverseWorldMatrix().T();
            mInverseTransposeWorldMatrixDirty = false;
        }
        return mInverseTransposeWorldMatrix;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f& SysSyncParam::getInverseTransposeWorldViewMatrix() const
    {
        if (mInverseTransposeWorldViewMatrixDirty)
        {
            mInverseTransposeWorldViewMatrix = getInverseWorldViewMatrix().T();
            mInverseTransposeWorldViewMatrixDirty = false;
        }
        return mInverseTransposeWorldViewMatrix;
    }
    //-----------------------------------------------------------------------------
    const Vector4f& SysSyncParam::getCameraPosition() const
    {
        if(mCameraPositionDirty)
        {
            Vector3f vec3 = mCurrentCamera->getSpacePos();
            if (mCameraRelativeRendering)
            {
                vec3 -= mCameraRelativePosition;
            }
            mCameraPosition[0] = vec3[0];
            mCameraPosition[1] = vec3[1];
            mCameraPosition[2] = vec3[2];
            mCameraPosition[3] = 1.0;
            mCameraPositionDirty = false;
        }
        return mCameraPosition;
    }
    //-----------------------------------------------------------------------------
    const Vector4f & SysSyncParam::getCameraPositionObjectSpace() const
    {
        if(mCameraPositionObjectSpaceDirty)
        {
            if(mCameraRelativeRendering)
            {
                mCameraPositionObjectSpace = getInverseWorldMatrix().affine(Vector3f::ZERO);
            }
            else
            {
                mCameraPositionObjectSpace = getInverseWorldMatrix().affine(mCurrentCamera->getSpacePos());
            }
            mCameraPositionObjectSpaceDirty = false;
        }
        return mCameraPositionObjectSpace;
    }
    //-----------------------------------------------------------------------------
    const Vector4f& SysSyncParam::getLodCameraPosition() const
    {
        if(mLodCameraPositionDirty)
        {
            Vector3f vec3 = mCurrentCamera->getExtLod()->getSpacePos();
            if(mCameraRelativeRendering)
            {
                vec3 -= mCameraRelativePosition;
            }
            mLodCameraPosition[0] = vec3[0];
            mLodCameraPosition[1] = vec3[1];
            mLodCameraPosition[2] = vec3[2];
            mLodCameraPosition[3] = 1.0;
            mLodCameraPositionDirty = false;
        }
        return mLodCameraPosition;
    }
    //-----------------------------------------------------------------------------
    const Vector4f & SysSyncParam::getLodCameraPositionObjectSpace() const
    {
        if(mLodCameraPositionObjectSpaceDirty)
        {
            if(mCameraRelativeRendering)
            {
                mLodCameraPositionObjectSpace = getInverseWorldMatrix().affine(
                    mCurrentCamera->getExtLod()->getSpacePos() - mCameraRelativePosition);
            }
            else
            {
                mLodCameraPositionObjectSpace = getInverseWorldMatrix().affine(
                    mCurrentCamera->getExtLod()->getSpacePos());
            }
            mLodCameraPositionObjectSpaceDirty = false;
        }
        return mLodCameraPositionObjectSpace;
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getTextureSize(Nidx index) const
    {
        Vector4f size = Vector4f(1,1,1,1);

        if(index < mCurrentShaderCh->getTexture().getCount())
        {
            const Texture * tex = mCurrentShaderCh->getTexture().get(index)->getTexture();
            if(tex != 0)
            {
                size.x = (NIIf)tex->getWidth();
                size.y = (NIIf)tex->getHeight();
                size.z = (NIIf)tex->getDepth();
            }
        }

        return size;
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getInverseTextureSize(NCount index) const
    {
        Vector4f size = getTextureSize(index);
        return 1 / size;
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getPackedTextureSize(NCount index) const
    {
        Vector4f size = getTextureSize(index);
        return Vector4f(size.x, size.y, 1 / size.x, 1 / size.y);
    }
    //-----------------------------------------------------------------------------
    Colour SysSyncParam::getSceneColour() const
    {
        Colour result = getEnvAmbient() + getChEmissive();
        result.a = getChDiffuse().a;
        return result;
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setFog(const ShaderChFog & fog)
    {
        mFogColour = fog.mColour;
        mFogParams.x = fog.mDensity;
        mFogParams.y = fog.mBegin;
        mFogParams.z = fog.mEnd;
        mFogParams.w = fog.mEnd != fog.mBegin ? 1 / (fog.mEnd - fog.mBegin) : 0;
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setPoint(const ShaderChPoint & point)
    {
        mPointParams.x = point.getSize();
        mPointParams.y = point.getConstant();
        mPointParams.z = point.getLinear();
        mPointParams.w = point.getQuadratic();
        if (point.isAttenuateEnable())
            mPointParams[0] *= getViewportHeight();
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setTextureProjector(const Frustum * frust, NCount index = 0)
    {
        if (index < NII_MAX_LIGHT)
        {
            mCurrentTextureProjector[index] = frust;
            mTextureViewProjMatrixDirty[index] = true;
            mTextureWorldViewProjMatrixDirty[index] = true;
            mShadowCamDepthRangesDirty[index] = true;
        }
    }
    //-----------------------------------------------------------------------------
    void SysSyncParam::setMainCameraBound(Camera * info)                   
    { 
        mSceneDepthRangeDirty = true; 
    }
    //-----------------------------------------------------------------------------
    const Matrix4f& SysSyncParam::getTextureViewProjMatrix(NCount index) const
    {
        if (index < NII_MAX_LIGHT)
        {
            if (mTextureViewProjMatrixDirty[index] && mCurrentTextureProjector[index])
            {
                if (mCameraRelativeRendering)
                {
                    // World positions are now going to be relative to the camera position
                    // so we need to alter the projector view matrix to compensate

                    Matrix4f tmp4 = Matrix4f::IDENTITY;
                    tmp4.setTrans(mCurrentCamera->getSpacePos());
                    Matrix4f viewMatrix = mCurrentTextureProjector[index]->getViewMatrix() * tmp4;

                    RenderSys * rsys = N_Engine().getRender();
                    rsys->getSysProj(mCurrentTextureProjector[index]->getProjMatrix(), tmp4, true);

                    mTextureViewProjMatrix[index] =
                        PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE * tmp4 * viewMatrix;
                }
                else
                {
                    Matrix4f tmp4;

                    RenderSys * rsys = N_Engine().getRender();
                    rsys->getSysProj(mCurrentTextureProjector[index]->getProjMatrix(), tmp4, true);

                    mTextureViewProjMatrix[index] =
                        PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE *
                            tmp4 * mCurrentTextureProjector[index]->getViewMatrix();
                }
                mTextureViewProjMatrixDirty[index] = false;
            }
            return mTextureViewProjMatrix[index];
        }
        else
            return Matrix4f::IDENTITY;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f& SysSyncParam::getTextureWorldViewProjMatrix(NCount index) const
    {
        if (index < NII_MAX_LIGHT)
        {
            if (mTextureWorldViewProjMatrixDirty[index] && mCurrentTextureProjector[index])
            {
                mTextureWorldViewProjMatrix[index] =
                    getTextureViewProjMatrix(index) * getWorldMatrix();
                mTextureWorldViewProjMatrixDirty[index] = false;
            }
            return mTextureWorldViewProjMatrix[index];
        }
        else
            return Matrix4f::IDENTITY;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getSpotlightViewProjMatrix(NCount index) const
    {
        if(index < NII_MAX_LIGHT)
        {
            Light * l = getLight(index);

            if(l != &mBlankLight && l->getType() == LT_SPOT &&
                mSpotlightViewProjMatrixDirty[index])
            {
                Frustum frust;
                SpaceNode dummyNode(0);
                dummyNode.attach(&frust);

                frust.setProjType(Frustum::T_Perspective);
                frust.setFOV(l->getSpotlightOuter());
                frust.setAspectRatio(1.0f);
                // set near clip the same as main camera, since they are likely
                // to both reflect the nature of the scene
                frust.setNearClipDistance(mCurrentCamera->getNearClipDistance());
                // Calculate position, which same as spotlight position, in camera-relative coords if required
                dummyNode.setPos(l->getRelSpacePos());
                // Calculate direction, which same as spotlight direction
                Vector3f dir = - l->getSpaceDirection(); // backwards since point down -z
                dir.normalise();
                Vector3f up = Vector3f::Y;
                // Check it's not coincident with dir
                if (Math::Abs(up.dotProduct(dir)) >= 1.0f)
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
                dummyNode.setOri(q);

                Matrix4f tmp4;
                RenderSys * rsys = N_Engine().getRender();
                rsys->getSysProj(frust.getProjMatrix(), tmp4, true);

                // The view matrix here already includes camera-relative changes if necessary
                // since they are built into the frustum position
                mSpotlightViewProjMatrix[index] =
                    PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE * tmp4 * frust.getViewMatrix();

                mSpotlightViewProjMatrixDirty[index] = false;
            }
            return mSpotlightViewProjMatrix[index];
        }
        else
            return Matrix4f::IDENTITY;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f& SysSyncParam::getSpotlightWorldViewProjMatrix(NCount index) const
    {
        if(index < NII_MAX_LIGHT)
        {
            Light * l = getLight(index);

            if(l != &mBlankLight && l->getType() == LT_SPOT &&
                mSpotlightWorldViewProjMatrixDirty[index])
            {
                mSpotlightWorldViewProjMatrix[index] =
                    getSpotlightViewProjMatrix(index) * getWorldMatrix();
                mSpotlightWorldViewProjMatrixDirty[index] = false;
            }
            return mSpotlightWorldViewProjMatrix[index];
        }
        else
            return Matrix4f::IDENTITY;
    }
    //-----------------------------------------------------------------------------
    const Matrix4f & SysSyncParam::getTextureTransformMatrix(Nidx index) const
    {
        // make sure the current pass is set
        N_assert(mCurrentShaderCh, "current pass is NULL!");
        // check if there is a texture unit with the given index in the current pass
        if(index < mCurrentShaderCh->getTexture().getCount())
        {
            // texture unit existent, return its currently set transform
            return mCurrentShaderCh->getTexture().get(index)->getMatrix();
        }
        else
        {
            // no such texture unit, return unity
            return Matrix4f::IDENTITY;
        }
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getShadowExtrusionDistance() const
    {
        Light * l = getLight(0); // only ever applies to one light at once
        if(l->getType() == LT_DIRECTIONAL)
        {
            // use constant
            return mDirLightExtrusionDistance;
        }
        else
        {
            // Calculate based on object space light distance
            // compared to light attenuation range
            Vector3f objPos = getInverseWorldMatrix().affine(l->getRelSpacePos());
            return l->getRange() - objPos.length();
        }
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getTime_0_X(TimeDurMS x) const
    {
        return fmodf(N_Only(Animation).getCostTime() / 1000.0f, x / 1000.0f);
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getCosTime_0_X(TimeDurMS x) const
    {
        return cosf(getTime_0_X(x));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getSinTime_0_X(TimeDurMS x) const
    {
        return sinf(getTime_0_X(x));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getTanTime_0_X(TimeDurMS x) const
    {
        return tanf(getTime_0_X(x));
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getTime_0_X_packed(TimeDurMS x) const
    {
        NIIf t = getTime_0_X(x);
        return Vector4f(t, sin(t), cos(t), tan(t));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getTime_0_1(TimeDurMS x) const
    {
        return getTime_0_X(x) / (x / 1000);
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getCosTime_0_1(TimeDurMS x) const
    {
        return cos(getTime_0_1(x));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getSinTime_0_1(TimeDurMS x) const
    {
        return sin(getTime_0_1(x));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getTanTime_0_1(TimeDurMS x) const
    {
        return tan(getTime_0_1(x));
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getTime_0_1_packed(TimeDurMS x) const
    {
        NIIf t = getTime_0_1(x);
        return Vector4f(t, sin(t), cos(t), tan(t));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getTime_0_2Pi(TimeDurMS x) const
    {
        return (NIIf)getTime_0_X(x) / x * 2 * Math::PI;
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getCosTime_0_2Pi(TimeDurMS x) const
    {
        return cos(getTime_0_2Pi(x));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getSinTime_0_2Pi(TimeDurMS x) const
    {
        return sin(getTime_0_2Pi(x));
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getTanTime_0_2Pi(TimeDurMS x) const
    {
        return tan(getTime_0_2Pi(x));
    }
    //-----------------------------------------------------------------------------
    Vector4f SysSyncParam::getTime_0_2Pi_packed(TimeDurMS x) const
    {
        NIIf t = getTime_0_2Pi(x);
        return Vector4f(t, sin(t), cos(t), tan(t));
    }
    //-----------------------------------------------------------------------------
    TimeDurMS SysSyncParam::getFrameTime() const
    {
        return N_Only(Animation).getLastTimeValue()->getValue();
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getFPS() const
    {
        return mCurrentFBO->getFPS().mLast;
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getViewportWidth() const
    {
        return static_cast<NIIf>(mCurrentViewport->getPixelWidth());
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getViewportHeight() const
    {
        return static_cast<NIIf>(mCurrentViewport->getPixelHeight());
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getInverseViewportWidth() const
    {
        return 1.0f/mCurrentViewport->getPixelWidth();
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getInverseViewportHeight() const
    {
        return 1.0f/mCurrentViewport->getPixelHeight();
    }
    //-----------------------------------------------------------------------------
    Vector3f SysSyncParam::getViewDirection() const
    {
        return mCurrentCamera->getSpaceDirection();
    }
    //-----------------------------------------------------------------------------
    Vector3f SysSyncParam::getViewSideVector() const
    {
        return mCurrentCamera->getSpaceRight();
    }
    //-----------------------------------------------------------------------------
    Vector3f SysSyncParam::getViewUpVector() const
    {
        return mCurrentCamera->getSpaceUp();
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getFOV() const
    {
        return mCurrentCamera->getFOV().getValue();
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getNearClipDistance() const
    {
        return mCurrentCamera->getNearClipDistance();
    }
    //-----------------------------------------------------------------------------
    NIIf SysSyncParam::getFarClipDistance() const
    {
        return mCurrentCamera->getFarClipDistance();
    }
    //-----------------------------------------------------------------------------
    const Vector4f & SysSyncParam::getSceneDepthRange() const
    {
        static Vector4f dummy(0, 100000, 100000, 1/100000);

        if(mSceneDepthRangeDirty)
        {
            // calculate depth information
            NIIf depthRange = mCurrentCamera->getBoundMax(ST_Cast) - mCurrentCamera->getBoundMin(ST_Cast);
            if(depthRange > N_TYPE_EPSILON(NIIf))
            {
                mSceneDepthRange = Vector4f(
                    mCurrentCamera->getBoundMin(ST_Cast),
                    mCurrentCamera->getBoundMax(ST_Cast),
                    depthRange,
                    1.0f / depthRange);
            }
            else
            {
                mSceneDepthRange = dummy;
            }
            mSceneDepthRangeDirty = false;
        }
        return mSceneDepthRange;
    }
    //-----------------------------------------------------------------------------
    const Vector4f & SysSyncParam::getShadowSceneDepthRange(NCount index) const
    {
        static Vector4f dummy(0, 100000, 100000, 1/100000);

        if(!mCurrentSpaceManager->getRenderPattern()->isTextureShadow())
            return dummy;

        if(index < NII_MAX_LIGHT)
        {
            if(mShadowCamDepthRangesDirty[index] && mCurrentTextureProjector[index])
            {
                const Camera * c = (Camera *)mCurrentTextureProjector[index];

                NIIf depthRange = c->getBoundMax(ST_Cast) - c->getBoundMin(ST_Cast);
                if(depthRange > N_TYPE_EPSILON(NIIf))
                {
                    mShadowCamDepthRanges[index] = Vector4f(
                        c->getBoundMin(ST_Cast),
                        c->getBoundMax(ST_Cast),
                        depthRange,
                        1.0f / depthRange);
                }
                else
                {
                    mShadowCamDepthRanges[index] = dummy;
                }

                mShadowCamDepthRangesDirty[index] = false;
            }
            return mShadowCamDepthRanges[index];
        }
        else
            return dummy;
    }
    //--------------------------------------------------------------------------
}