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

#ifndef _NII_SysSyncParam_H_
#define _NII_SysSyncParam_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiMatrix4.h"
#include "NiiLight.h"
#include "NiiEngine.h"
#include "NiiRenderPattern.h"
#include "NiiAnimationManager.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SysSyncParam : public GpuParamsAlloc
    {
    public:
        SysSyncParam();
        virtual ~SysSyncParam();

        /** 更新当前可渲染物
        @version NIIEngine 3.0.0
        */
        void setCurrentRenderable(const GeometryObj * obj);

        /** 设置全局矩阵,避免可渲染物再次查询
        @version NIIEngine 3.0.0
        */
        void setWorldMatrices(const Matrix4f * m, NCount count);

        /** 更新当前摄象机
        @version NIIEngine 3.0.0
        */
        void setCurrentCamera(const Camera * cam, bool rel);

        /** 设置要使用的灯光列表,它的索引是来自全局列表的
        */
        void setCurrentLightList(const LightList & ll);

        /** 为索引设置当前纹理投射器
        */
        void setTextureProjector(const Frustum * frust, NCount index);

        /** 设置当前渲染目标
        */
        inline void setCurrentFBO(const FrameBufferObject * fbo)        { mCurrentFBO = fbo; }

        /** 设置当前视口
        */
        inline void setCurrentViewport(const Viewport * viewport)       { mCurrentViewport = viewport; }

        /** 设置用于点灯光阴影挤压距离
        */
        inline void setShadowDirLightExtrusionDistance(NIIf dist)       { mDirLightExtrusionDistance = dist; }

        /** 设置主摄象机边界信息
        */
        inline void setMainCameraBound(Camera * info);

        /** 为查询需求设置当前scene manager
        */
        inline void setCurrentSceneManager(const SpaceManager * sm)     { mCurrentSpaceManager = sm; }

        /** 设置当前通道
        */
        inline void setCurrentPass(const ShaderCh * ch)                 { mCurrentShaderCh = ch; }

        NCount getWorldMatrixCount() const;

        const Matrix4f & getWorldMatrix() const;
        const Matrix4f * getWorldMatrixArray() const;
        const Matrix4f & getViewMatrix() const;
        const Matrix4f & getViewProjectionMatrix() const;
        const Matrix4f & getProjMatrix() const;
        const Matrix4f & getWorldViewProjMatrix() const;
        const Matrix4f & getWorldViewMatrix() const;
        const Matrix4f & getInverseWorldMatrix() const;
        const Matrix4f & getInverseWorldViewMatrix() const;
        const Matrix4f & getInverseViewMatrix() const;
        const Matrix4f & getInverseTransposeWorldMatrix() const;
        const Matrix4f & getInverseTransposeWorldViewMatrix() const;
        const Vector4f & getCameraPosition() const;
        const Vector4f & getCameraPositionObjectSpace() const;
        const Vector4f & getLodCameraPosition() const;
        const Vector4f & getLodCameraPositionObjectSpace() const;
        bool hasLightList() const                                       { return mCurrentLightList->size() != 0; }
        inline NIIf getLightNumber(NCount index) const                  { return static_cast<NIIf>(getLight(index)->getIndex()); }
        inline NIIf getLightCount() const                               { return (NIIf)mCurrentLightList->size(); }
        inline NIIf getLightCastsShadows(NCount index) const            { return getLight(index)->isCastShadow() ? 1.0f : 0.0f; }
        inline const Colour & getLightDiffuseColour(NCount index) const { return getLight(index)->getDiffuse(); }
        inline const Colour & getLightSpecularColour(NCount index) const { return getLight(index)->getSpecular(); }
        const Colour getLightDiffuseColourWithPower(NCount index) const;
        const Colour getLightSpecularColourWithPower(NCount index) const;
        inline const Vector3f & getLightPosition(NCount index) const    { return getLight(index)->getRelSpacePos(); }
        inline Vector4f getLightRelSpace4(NCount index) const           { return getLight(index)->getRelSpace4(); }
        inline const Vector3f & getLightDirection(NCount index) const   { return getLight(index)->getSpaceDirection(); }
        inline NIIf getLightPowerScale(NCount index) const              { return getLight(index)->getBrightFactor(); }
        Vector4f getLightAttenuation(NCount index) const;
        Vector4f getSpotlightParams(NCount index) const;
        inline void setAmbientColour(const Colour& ambient)             { mAmbient = ambient; }
        inline const Colour & getAmbientColour() const                  { return mAmbient; }
        inline const Colour & getChAmbientColour() const                { return mCurrentShaderCh->getColour().getAmbient(); }
        inline const Colour & getChDiffuseColour() const                { return mCurrentShaderCh->getColour().getDiffuse(); }
        inline const Colour & getChSpecularColour() const               { return mCurrentShaderCh->getColour().getSpecular(); }
        inline const Colour & getChEmissiveColour() const               { return mCurrentShaderCh->getColour().getEmissive(); }
        inline NIIf getChShininess() const                              { return mCurrentShaderCh->getColour().getShininess(); }
        inline Colour getEnvAmbientColour() const                       { return getAmbientColour() * getChAmbientColour(); }
        Colour getSceneColour() const;
        void setFog(const ShaderChFog & fog);
        inline const Colour & getFogColour() const                      { return mFogColour; }
        inline const Vector4f & getFogParams() const                    { return mFogParams; }
        void setPoint(const ShaderChPoint & point);
        inline const Vector4f & getPointParams() const                  { return mPointParams; }
        const Matrix4f & getTextureViewProjMatrix(NCount index) const;
        const Matrix4f & getTextureWorldViewProjMatrix(NCount index) const;
        const Matrix4f & getSpotlightViewProjMatrix(NCount index) const;
        const Matrix4f & getSpotlightWorldViewProjMatrix(NCount index) const;
        const Matrix4f & getTextureTransformMatrix(Nidx index) const;
        inline const FrameBufferObject * getCurrentFBO() const          { return mCurrentFBO; }
        inline const GeometryObj * getCurrentGeometryObj() const        { return mCurrentGeometryObj; }
        inline const ShaderCh * getCurrentPass() const                  { return mCurrentShaderCh; }
        Vector4f getTextureSize(Nidx index) const;
        Vector4f getInverseTextureSize(NCount index) const;
        Vector4f getPackedTextureSize(NCount index) const;
        NIIf getShadowExtrusionDistance() const;
        const Vector4f & getSceneDepthRange() const;
        const Vector4f & getShadowSceneDepthRange(NCount index) const;
        inline const Colour & getShadowColour() const               { return mCurrentSpaceManager->getRenderPattern()->getShadowColour(); }
        inline Matrix4f getInverseViewProjMatrix() const            { return getViewProjectionMatrix().inverse(); }
        inline Matrix4f getInverseTransposeViewProjMatrix() const   { return getInverseViewProjMatrix().T(); }
        inline Matrix4f getTransposeViewProjMatrix() const          { return getViewProjectionMatrix().T(); }
        inline Matrix4f getTransposeViewMatrix() const              { return getViewMatrix().T(); }
        inline Matrix4f getInverseTransposeViewMatrix() const       { return getInverseViewMatrix().T(); }
        inline Matrix4f getTransposeProjectionMatrix() const        { return getProjMatrix().T(); }
        inline Matrix4f getInverseProjectionMatrix() const          { return getProjMatrix().inverse(); }
        inline Matrix4f getInverseTransposeProjectionMatrix() const { return getInverseProjectionMatrix().T(); }
        inline Matrix4f getTransposeWorldViewProjMatrix() const     { return getWorldViewProjMatrix().T(); }
        inline Matrix4f getInverseWorldViewProjMatrix() const       { return getWorldViewProjMatrix().inverse(); }
        inline Matrix4f getInverseTransposeWorldViewProjMatrix() const { return getInverseWorldViewProjMatrix().T(); }
        inline Matrix4f getTransposeWorldViewMatrix() const         { return getWorldViewMatrix().T(); }
        inline Matrix4f getTransposeWorldMatrix() const             { return getWorldMatrix().T(); }
        inline TimeDurMS getTime() const                            { return N_Only(Animation).getCostTime(); }
        NIIf getTime_0_X(TimeDurMS x) const;
        NIIf getCosTime_0_X(TimeDurMS x) const;
        NIIf getSinTime_0_X(TimeDurMS x) const;
        NIIf getTanTime_0_X(TimeDurMS x) const;
        Vector4f getTime_0_X_packed(TimeDurMS x) const;
        NIIf getTime_0_1(TimeDurMS x) const;
        NIIf getCosTime_0_1(TimeDurMS x) const;
        NIIf getSinTime_0_1(TimeDurMS x) const;
        NIIf getTanTime_0_1(TimeDurMS x) const;
        Vector4f getTime_0_1_packed(TimeDurMS x) const;
        NIIf getTime_0_2Pi(TimeDurMS x) const;
        NIIf getCosTime_0_2Pi(TimeDurMS x) const;
        NIIf getSinTime_0_2Pi(TimeDurMS x) const;
        NIIf getTanTime_0_2Pi(TimeDurMS x) const;
        Vector4f getTime_0_2Pi_packed(TimeDurMS x) const;
        TimeDurMS getFrameTime() const;
        NIIf getFPS() const;
        NIIf getViewportWidth() const;
        NIIf getViewportHeight() const;
        NIIf getInverseViewportWidth() const;
        NIIf getInverseViewportHeight() const;
        Vector3f getViewDirection() const;
        Vector3f getViewSideVector() const;
        Vector3f getViewUpVector() const;
        NIIf getFOV() const;
        NIIf getNearClipDistance() const;
        NIIf getFarClipDistance() const;
        NIIi getPassNumber() const;
        void setPassNumber(const NIIi passNumber);
        void incPassNumber();
        void updateLightCustomGpuParameter(const GpuParamBlock & bind, GpuProgramParam *params) const;
    protected:
        Light * getLight(NCount index) const;
    protected:
        mutable Matrix4f mWorldMatrix[256];
        mutable NCount mWorldMatrixCount;
        mutable const Matrix4f * mWorldMatrixArray;
        mutable Matrix4f mWorldViewMatrix;
        mutable Matrix4f mViewProjMatrix;
        mutable Matrix4f mWorldViewProjMatrix;
        mutable Matrix4f mInverseWorldMatrix;
        mutable Matrix4f mInverseWorldViewMatrix;
        mutable Matrix4f mInverseViewMatrix;
        mutable Matrix4f mInverseTransposeWorldMatrix;
        mutable Matrix4f mInverseTransposeWorldViewMatrix;
        mutable Vector4f mCameraPosition;
        mutable Vector4f mCameraPositionObjectSpace;
        mutable Matrix4f mTextureViewProjMatrix[NII_MAX_LIGHT];
        mutable Matrix4f mTextureWorldViewProjMatrix[NII_MAX_LIGHT];
        mutable Matrix4f mSpotlightViewProjMatrix[NII_MAX_LIGHT];
        mutable Matrix4f mSpotlightWorldViewProjMatrix[NII_MAX_LIGHT];
        mutable Vector4f mShadowCamDepthRanges[NII_MAX_LIGHT];
        mutable Matrix4f mViewMatrix;
        mutable Matrix4f mProjectionMatrix;
        mutable NIIf mDirLightExtrusionDistance;
        mutable Vector4f mLodCameraPosition;
        mutable Vector4f mLodCameraPositionObjectSpace;

        mutable bool mWorldMatrixDirty;
        mutable bool mViewMatrixDirty;
        mutable bool mProjMatrixDirty;
        mutable bool mWorldViewMatrixDirty;
        mutable bool mViewProjMatrixDirty;
        mutable bool mWorldViewProjMatrixDirty;
        mutable bool mInverseWorldMatrixDirty;
        mutable bool mInverseWorldViewMatrixDirty;
        mutable bool mInverseViewMatrixDirty;
        mutable bool mInverseTransposeWorldMatrixDirty;
        mutable bool mInverseTransposeWorldViewMatrixDirty;
        mutable bool mCameraPositionDirty;
        mutable bool mCameraPositionObjectSpaceDirty;
        mutable bool mTextureViewProjMatrixDirty[NII_MAX_LIGHT];
        mutable bool mTextureWorldViewProjMatrixDirty[NII_MAX_LIGHT];
        mutable bool mSpotlightViewProjMatrixDirty[NII_MAX_LIGHT];
        mutable bool mSpotlightWorldViewProjMatrixDirty[NII_MAX_LIGHT];
        mutable bool mShadowCamDepthRangesDirty[NII_MAX_LIGHT];
        mutable Colour mAmbient;
        mutable Colour mFogColour;
        mutable Vector4f mFogParams;
        mutable Vector4f mPointParams;
        mutable NIIi mPassNumber;
        mutable Vector4f mSceneDepthRange;
        mutable bool mSceneDepthRangeDirty;
        mutable bool mLodCameraPositionDirty;
        mutable bool mLodCameraPositionObjectSpaceDirty;
        bool mCameraRelativeRendering;
        
        const GeometryObj * mCurrentGeometryObj;
        const Camera * mCurrentCamera;
        Vector3f mCameraRelativePosition;
        const LightList * mCurrentLightList;
        const Frustum * mCurrentTextureProjector[NII_MAX_LIGHT];
        const FrameBufferObject * mCurrentFBO;
        const Viewport * mCurrentViewport;
        const SpaceManager * mCurrentSpaceManager;
        const ShaderCh * mCurrentShaderCh;
        Light mBlankLight;
    };
}
#endif