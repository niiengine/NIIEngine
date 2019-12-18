/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-22

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
        void updateCustom(GpuProgramParam * params, const GpuSyncParamBind & bind) const;
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
        Nui16 mVertexFusionCount;
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