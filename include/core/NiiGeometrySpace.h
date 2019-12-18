/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-4-21

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

#ifndef _NII_GeometrySpace_H_
#define _NII_GeometrySpace_H_

#include "NiiPreInclude.h"
#include "NiiShadowVolumeGen.h"
#include "NiiQuaternion.h"
#include "NiiAnimation.h"

namespace NII
{
    class AniTempBuffer;

    /** 空间对象
    @remark
        可以用于描述主角对象,这些对象有灵活的骨骼,同时也可以拥有面部表情,也可以附加道
        具模型,也能呈现阴影
    @note
        这不是资源级/序列级对象,也就是说它可以拥有/存在当前状态的概念
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometrySpace : public ShadowVolumeGen, public ResourceListener
    {
        friend class VertexUnitFusion;
        friend class VertexOffsetFusion;
        friend class GeometryCom;
        friend class GeometrySpaceFactory;
    public:
        typedef vector<GeometrySpace *>::type Lods;
        typedef vector<GeometryCom *>::type Coms;
        typedef set<GeometrySpace *>::type GeoList;
        typedef map<SpaceID, SpaceObj *>::type Childs;
    public:
        ~GeometrySpace();

        /** 附加空间对象
        @param[in] bid 骨头ID
        @param[in] obj 空间对象
        @param[in] oftpos 相对于骨头的位置偏移
        @param[in] oftori 相对于骨头的方向偏移
        @version NIIEngine 3.0.0
        */
        TagBone * attach(Nidx boneidx, SpaceObj * obj,
            const Vector3f & oftpos = Vector3f::ZERO,
            const Quaternion & oftori = Quaternion::IDENTITY);

        /** 分离空间对象
        @param[in] id 空间对象ID
        */
        SpaceObj * detach(SpaceID sid);

        /** 分离空间对象
        @param[in] obj 空间对象
        */
        void detach(SpaceObj * obj);

        /** 分离所有空间对象
        @version NIIEngine 3.0.0
        */
        void detachAll();

        /** 获取附加空间对象列表
        @version NIIEngine 3.0.0
        */
        const Childs & getAttachList();

        /** 获取子部分
        @version NIIEngine 3.0.0
        */
        GeometryCom * getCom(Nidx index) const;

        /** 获取子部分数量
        @version NIIEngine 3.0.0
        */
        NCount getComCount() const;

        /** 获取子部分
        @version NIIEngine 3.0.0
        */
        AABox getComAABB() const;

        /** 检测动画状态有效性
        @version NIIEngine 3.0.0 高级api
        */
        AnimationMode checkAnimation();

        /** 确定硬件动画有效性
        @version NIIEngine 3.0.0 高级api
        */
        void checkHWAnimation();

        /** 更新动画
        @remark 异步更新动画
        @version NIIEngine 3.0.0
        */
        void updateAnimation();

        /** 更新动画
        @remark 实时更新动画
        @version NIIEngine 3.0.0 高级api
        */
        void _updateAnimation();

        /** 设置是否自动更新骨骼
        @version NIIEngine 3.0.0
        */
        void setAutoSkeleton(bool b);

        /** 获取是否自动更新骨骼
        @version NIIEngine 3.0.0
        */
        bool isAutoSkeleton() const;

        /** 获取是否Lod状态也自动更新骨骼
        @version NIIEngine 3.0.0
        */
        void setLODAutoSkeleton(bool b);

        /** 获取是否Lod状态也自动更新骨骼
        @version NIIEngine 3.0.0
        */
        bool isLODAutoSkeleton() const;

        /** 获取是否存在有效动画
        @version NIIEngine 3.0.0
        */
        bool isAnimationValid() const;

        /** 动画是否也混合法线
        @version NIIEngine 3.0.0
        */
        bool isNromalsFusion() const;

        /** 获取是否存在骨骼动画
        @version NIIEngine 3.0.0
        */
        bool isSkeletonValid() const;

        /** 硬件处理动画混合
        @version NIIEngine 3.0.0
        */
        bool isHWAnimation() const;

        /** 连接骨骼
        @remark 使用同个骨骼混合结果
        @version NIIEngine 3.0.0
        */
        void linkSkeleton(GeometrySpace * obj);

        /** 断开链接骨骼
        @remark 结束使用同个骨骼混合结果
        @version NIIEngine 3.0.0
        */
        void unlinkSkeleton();

        /** 设置是否显示骨骼
        @version NIIEngine 3.0.0
        */
        void setBoneView(bool b);

        /** 获取是否显示骨骼
        @version NIIEngine 3.0.0
        */
        bool getBoneView() const;

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid, GroupID gid = GroupUnknow);

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(const Material * obj);

        /** 是否启用动画
        @version NIIEngine 3.0.0
        */
        void setAnimation(bool b);

        /** 是否启用动画
        @version NIIEngine 3.0.0
        */
        bool isAnimation() const;

        /** 获取动画混合实例
        @version NIIEngine 3.0.0
        */
        AnimationFusion * getFusion(AnimationID id) const;

        /** 是否存在动画混合实例
        @version NIIEngine 3.0.0
        */
        bool isFusionExist(AnimationID id) const;

        /** 获取动画混合
        @version NIIEngine 3.0.0
        */
        AnimationFusionObj * getAnimationCtrl() const;

        /** 获取骨骼实例
        @version NIIEngine 3.0.0
        */
        SkeletonFusion * getSkeleton() const;

        /** 获取网格.
        @version NIIEngine 3.0.0
        */
        const Mesh * getMesh() const;

        /** 获取当前渲染的LOD
        @version NIIEngine 3.0.0
        */
        Nui16 getEvnLOD();

        /** 设置网格LOD偏量控制
        @remark 小索引高细节,高索引低细节
        @param[in] factor 正比系数
        @version NIIEngien 3.0.0 高级api
        */
        void setMeshLodBias(NIIf factor, Nui16 min = 0, Nui16 max = 50);

        /** 设置材质LOD偏量控制
        @remark 小索引高细节,高索引低细节
        @param[in] factor 正比系数
        @version NIIEngien 3.0.0 高级api
        */
        void setMaterialLodBias(NIIf factor, Nui16 min = 0, Nui16 max = 50);

        /** 获取LOD空间对象
        @param[in] index 0是原体
        @version NIIEngine 3.0.0
        */
        GeometrySpace * getLOD(Nidx index) const;

        /** 获取LOD空间对象
        @remark 包含原体的数量
        @version NIIEngine 3.0.0
        */
        NCount getLODCount() const;

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

        /** 获取用于渲染的顶点
        @version NIIEngine 3.0.0
        */
        VertexData * getRenderVertex() const;

        /** 构建结构
        @param[in] force 强制
        @version NIIEngine 3.0.0
        */
        bool _init(bool force = false);

        /** 删除结构
        @version NIIEngine 3.0.0
        */
        void _destroy();

        /** 副本
        @param[in] nid 新ID
        @version NIIEngine 3.0.0 高级api
        */
        GeometrySpace * clone(SpaceID nid) const;

        /// @copydetails ShadowVolumeGen::getEdgeList
        GeoEdget * getEdgeList();

        /// @copydetails ShadowVolumeGen::isEdgeExist
        bool isEdgeExist();

        /// @copydetails ShadowVolumeGen::getLightCapAABB
        const AABox & getLightCapAABB() const;

        /// @copydetails ShadowVolumeGen::getExtrudeDistance
        NIIf getExtrudeDistance(const Light * l) const;

        /// @copydetails ShadowVolumeGen::getVolumeList
        const ShadowVolumeList & getVolumeList(const Light * src, NIIf extent,
             Nmark flags, IndexBuffer * index);

        /** @copydetails ResourceListener::onLoadEnd
        @version NIIEngine 3.0.0
        */
        void onLoadEnd(Resource * res);

        using SpaceObj::query;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /// @copydetails SpaceObj::setRenderGroup
        void setRenderGroup(Nui16 rqid, Nui16 level = 0);

        /// @copydetails SpaceObj::getSpaceBS
        const Sphere & getSpaceBS(bool update = false) const;

        /// @copydetails SpaceObj::attach
        void attach(PosNode * parent, bool tag = false);

        /// @copydetails SpaceObj::_notify
        void _notify(Camera * cam);

        /// @copydetails SpaceObj::getAABB
        const AABox & getAABB() const;

        /// @copydetails SpaceObj::getSpaceAABB
        const AABox & getSpaceAABB(bool derive = false) const;

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails SpaceObj::queue
        void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::getTypeMark
        Nui32 getTypeMark() const;
    protected:
        GeometrySpace();
        GeometrySpace(SpaceID sid, const Mesh * mesh);

        /** 附加空间对象实现
        @version NIIEngine 3.0.0
        */
        void attachImpl(SpaceObj * obj, TagBone * tag);

        /** 分离空间对象实现
        @version NIIEngine 3.0.0
        */
        void detachImpl(SpaceObj * obj);

        /** 分离所有空间对象实现
        @version NIIEngine 3.0.0
        */
        void detachAllImpl();

        /** 构建子部件实现
        @versioin NIIEngine 3.0.0
        */
        void buildComImpl(const Mesh * mesh);

        /** 更新动画实现实现
        @version NIIEngine 3.0.0
        */
        void updateAniImpl();

        /** 应用顶点动画实现
        @version NIIEngine 3.0.0
        */
        void updateVertImpl(bool hw, bool sw);

        /** 更新骨骼空间实现
        @version NIIEngine 3.0.0
        */
        bool updateSkelImpl();

        /** 检测是否支持硬件动画实现
        @version NIIEngine 3.0.0
        */
        bool checkHWAniImpl();

        /** 获取软件混合顶点
        @version NIIEngine 3.0.0
        */
        const VertexData * getSWFusionVertex(const VertexData * src);

        /** 引用顶点动画缓存副本
        @version NIIEngine 3.0.0
        */
        bool touchVertAniCopy() const;

        /** 引用骨骼动画缓存副本
        @version NIIEngine 3.0.0
        */
        bool touchSkelAniCopy(bool normals) const;

        /** 构建动画缓存
        @version NIIEngine 3.0.0
        */
        void setupAnimationBuffer();

        /** 准备动画混合
        @version NIIEngine 3.0.0
        */
        void finishAnimationBlend(bool hw);
    protected:
        typedef map<SchemeID, bool>::type HWAnimationList;
    protected:
        Coms mComs;
        Childs mChilds;
        Lods mLods;
        Mesh * mMesh;
        NCount mMeshResState;
        GeoList * mLinkList;
        SkeletonFusion * mSkeleton;
        Nmark * mSkeletonFrame;
        AnimationFusionObj * mAnimationCtrl;
        Nmark mAnimationFrame;
        AniTempBuffer * mAniPrc;
        VertexData ** mAniData;
        AnimationMode mAnimationMode;
        mutable HWAnimationList mHWAnimationList;
        ShadowVolumeList mShadowVolumeList;
        Matrix4f * mBoneSpace;
        Matrix4f mLastSpace;
        mutable AABox mAABB;

        Nui16 mMeshLod;
        Nui16 mMinMeshLod;
        Nui16 mMaxMeshLod;
        Nui32 mMeshLodFactor;
        Nui16 mMinMaterialLod;
        Nui16 mMaxMaterialLod;
        NIIf mMaterialLodBias;
        Nui32 mMaterialLodFactor;
        Nui16 mVertexFusionCount;

        bool mAutoSkeleton;
        bool mLODAutoSkeleton;
        bool mHWAnimation;
        bool mVPEffect;
        bool mBoneView;
        bool mInit;
        bool mAnimationEnable;
        bool mNromalsFusion;
        bool mVertexShadowVolume;
        bool mAnimationCheck;
    };
}
#endif