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

#ifndef _NII_GEOMETRYCOM_H_
#define _NII_GEOMETRYCOM_H_

#include "NiiPreInclude.h"
#include "NiiGeometryObj.h"
#include "NiiAnimation.h"

namespace NII
{
    class AniTempBuffer;

    /** 空间对象的子部分
    @remark 主要用于空间对象的可变性,多样性,可渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryCom : public GeometryObj, public GeometryObjAlloc
    {
        friend class GeometrySpace;
        friend class SpaceManager;
    public:
        virtual ~GeometryCom();

        /** 获取父对象
        @version NIIEngine 3.0.0
        */
        GeometrySpace * getParent() const;

        /** 设置使用的材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid, GroupID gid = GroupUnknow);

        /** 设置使用的材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(const Material * m);

        /** 获取使用的材质资源ID
        @version NIIEngine 3.0.0
        */
        ResourceID getMaterialID() const;

        /** 是否可见
        @version NIIEngine 3.0.0
        */
        void setVisible(bool b);

        /** 是否可见
        @version NIIEngine 3.0.0
        */
        bool isVisible() const;

        /** 设置渲染组
        @version NIIEngine 3.0.0
        */
        void setRenderGroup(GroupID qid, Nui16 level = 0);

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        GroupID getRenderGroup() const;

        /** 获取渲染等级
        @version NIIEngine 3.0.0
        */
        Nui16 getRenderLevel() const;

        /** 获取相应的几何对象
        @version NIIEngine 3.0.0
        */
        SubMesh * getMesh() const;

        /** 设置是否启用动画
        @version NIIEngine 3.0.0
        */
        void setAnimation(bool b);

        /** 获取是否启用动画
        @version NIIEngine 3.0.0
        */
        bool getAnimation() const;

        /** 获取动画混合类型
        @version NIIEngine 3.0.0
        */
        AnimationMode checkAnimation();

        /** 动画是否包含法线混合
        @version NIIEngine 3.0.0
        */
        bool isNromalsFusion() const;

        /** 获取软件混合顶点
        @note 用于动画混合
        @version NIIEngine 3.0.0
        */
        VertexData * getSwFusionVertex() const;
        
        /** 获取硬件混合顶点
        @note 用于动画混合
        @version NIIEngine 3.0.0
        */
        VertexData * getHwFusionVertex() const;
        
        /** 获取用于数据混合的顶点
        @version NIIEngine 3.0.0
        */
        VertexData * getRenderVertex() const;

        /// @copydetails GeometryObj::getMaterial
        const Material * getMaterial() const;

        /// @copydetails GeometryObj::getShaderFusion
        ShaderFusion * getShaderFusion() const;

        /// @copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const;

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::getMatrixCount
        NCount getMatrixCount() const;

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @see GeometryObj::getLight
        void getLight(LightList & ll) const;

        /// @see GeometryObj::isCastShadow
        bool isCastShadow() const;

        /// @copydetails GpuCustomParam::updateCustom
        void updateCustom(GpuProgramParam * params, const GpuSParamIndex & bind) const;
    protected:
        GeometryCom(GeometrySpace * parent, SubMesh * obj);

        /** 构建动画缓存
        @version NIIEngine 3.0.0
        */
        void setupAnimationBuffer();

        /** 准备动画混合
        @version NIIEngine 3.0.0
        */
        void finishAnimationBlend(bool hware);
    protected:
        GeometrySpace * mParent;
        ResourceID mMaterialID;
        GroupID mRenderGroup;
        Nui16 mRenderLevel;
        Nui16 mLodIndex;
        NCount mVertexFusionCount;
        AnimationMode mAnimationMode;
        AniTempBuffer * mAniPrc;
        VertexData ** mAniData;
        Material * mMaterial;
        SubMesh * mMesh;
        mutable Camera * mLastCamera;
        mutable NIIf mViewDist;
        bool mVisible;
        bool mAnimationEnable;
        bool mNromalsFusion;
    };
}
#endif