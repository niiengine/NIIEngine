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

#ifndef _NII_GeometrySpace_H_
#define _NII_GeometrySpace_H_

#include "NiiPreInclude.h"
#include "NiiShadowObj.h"
#include "NiiSpaceObj.h"
#include "NiiAnimationObj.h"
#include "NiiQuaternion.h"
#include "NiiGeometryObj.h"
#include "NiiAnimation.h"

namespace NII
{
    class AniTempBuffer;

    /** 创建实体实例的工厂对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometrySpaceFactory : public SpaceObjFactory
    {
    public:
        GeometrySpaceFactory();
        ~GeometrySpaceFactory();

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        /// @copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };

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
        inline GeometrySpace * getParent() const { return mParent; }

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
        inline ResourceID getMaterialID() const { return mMaterialID; }

        /** 是否可见
        @version NIIEngine 3.0.0
        */
        inline void setVisible(bool b) { mVisible = b; }

        /** 是否可见
        @version NIIEngine 3.0.0
        */
        inline bool isVisible() const { return mVisible; }

        /** 设置渲染组
        @version NIIEngine 3.0.0
        */
        void setRenderGroup(GroupID qid, Nui16 level = 0);

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        inline GroupID getRenderGroup() const { return mRenderGroup; }

        /** 获取渲染等级
        @version NIIEngine 3.0.0
        */
        inline Nui16 getRenderLevel() const { return mRenderLevel; }

        /** 获取相应的几何对象
        @version NIIEngine 3.0.0
        */
        inline SubMesh * getMesh() const { return mMesh; }

        /** 设置是否启用动画
        @version NIIEngine 3.0.0
        */
        inline void setAnimation(bool b) { mAnimationEnable = b; }

        /** 获取是否启用动画
        @version NIIEngine 3.0.0
        */
        inline bool getAnimation() const { return mAnimationEnable; }

        /** 获取动画混合类型
        @version NIIEngine 3.0.0
        */
        AnimationMode checkAnimation();

        /** 动画是否包含法线混合
        @version NIIEngine 3.0.0
        */
        inline bool isNromalsFusion() const { return mNromalsFusion; }

        /** 获取软件混合顶点
        @note 用于动画混合
        @version NIIEngine 3.0.0
        */
        inline VertexData * getSwFusionVertex() const { return mAniData[0]; }

        /** 获取硬件混合顶点
        @note 用于动画混合
        @version NIIEngine 3.0.0
        */
        inline VertexData * getHwFusionVertex() const { return mAniData[1]; }

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

        /// @copydetails CustomGpuParam::updateCustom
        void updateCustom(GpuProgramParam * params, const GpuParamBlock & bind) const;
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
    /** 空间对象
    @remark
        可以用于描述主角对象,这些对象有灵活的骨骼,同时也可以拥有面部表情,也可以附加道
        具模型,也能呈现阴影
    @note
        这不是资源级/序列级对象,也就是说它可以拥有/存在当前状态的概念
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometrySpace : public ShadowObj, public ResourceListener
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
        inline const Childs & getAttachList()           { return mChilds; }

        /** 获取子部分
        @version NIIEngine 3.0.0
        */
        inline GeometryCom * getCom(Nidx index) const   { N_assert(index < mComs.size(), "超出下标"); return mComs[index]; }

        /** 获取子部分数量
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitCount() const              { return mComs.size(); }

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
        inline void setAutoSkeleton(bool b)             { mAutoSkeleton = b; }

        /** 获取是否自动更新骨骼
        @version NIIEngine 3.0.0
        */
        inline bool isAutoSkeleton() const              { return mAutoSkeleton; }

        /** 获取是否Lod状态也自动更新骨骼
        @version NIIEngine 3.0.0
        */
        inline void setLODAutoSkeleton(bool b)          { mLODAutoSkeleton = b; }

        /** 获取是否Lod状态也自动更新骨骼
        @version NIIEngine 3.0.0
        */
        inline bool isLODAutoSkeleton() const           { return mLODAutoSkeleton; }

        /** 获取是否存在有效动画
        @version NIIEngine 3.0.0
        */
        bool isAnimationValid() const;

        /** 动画是否也混合法线
        @version NIIEngine 3.0.0
        */
        inline bool isNromalsFusion() const             { return mNromalsFusion; }

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
        inline void setBoneView(bool set)               { mBoneView = set; }

        /** 获取是否显示骨骼
        @version NIIEngine 3.0.0
        */
        inline bool getBoneView() const                 { return mBoneView; }

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
        inline bool isAnimation() const                         { return mAnimationEnable; }

        /** 获取动画混合实例
        @version NIIEngine 3.0.0
        */
        inline AnimationFusion * getFusion(AnimationID id) const{ N_assert(mAnimationCtrl, "error"); return mAnimationCtrl->getFusion(id); }

        /** 是否存在动画混合实例
        @version NIIEngine 3.0.0
        */
        bool isFusionExist(AnimationID id) const;

        /** 获取动画混合
        @version NIIEngine 3.0.0
        */
        inline AnimationFusionObj * getAnimationCtrl() const    { return mAnimationCtrl; }

        /** 获取骨骼实例
        @version NIIEngine 3.0.0
        */
        inline SkeletonFusion * getSkeleton() const             { return mSkeleton; }

        /** 获取网格.
        @version NIIEngine 3.0.0
        */
        inline const Mesh * getMesh() const                     { return mMesh; }

        /** 获取当前渲染的LOD
        @version NIIEngine 3.0.0
        */
        inline Nui16 getEvnLOD()                                { return mMeshLod; }

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
        inline GeometrySpace * getLOD(Nidx index) const     { N_assert(index < mLods.size(), "error"); return mLods[index]; }

        /** 获取LOD空间对象
        @remark 包含原体的数量
        @version NIIEngine 3.0.0
        */
        inline NCount getLODCount() const                   { return mLods.size();}

        /** 获取软件混合顶点
        @note 用于动画混合
        @version NIIEngine 3.0.0
        */
        inline VertexData * getSwFusionVertex() const       { return mAniData[0]; }

        /** 获取硬件混合顶点
        @note 用于动画混合
        @version NIIEngine 3.0.0
        */
        inline VertexData * getHwFusionVertex() const       { return mAniData[1]; }

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

        /// @copydetails ShadowObj::getEdgeList
        GeoEdget * getEdgeList();

        /// @copydetails ShadowObj::isEdgeExist
        bool isEdgeExist();

        /// @copydetails ShadowObj::getFrontCapAABB
        const AABox & getFrontCapAABB() const;

        /// @copydetails ShadowObj::getExtrudeDistance
        NIIf getExtrudeDistance(const Light * l) const;

        /// @copydetails ShadowObj::getVolumeList
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