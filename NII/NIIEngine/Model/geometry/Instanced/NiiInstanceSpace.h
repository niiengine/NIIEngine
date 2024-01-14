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

#ifndef NII_InstanceSpace_H
#define NII_InstanceSpace_H

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiRenderObj.h"
#include "NiiAnimationObj.h"

namespace NII
{
    /** 网格批次,高效场景实例几何
    @version NIIEngine 3.0.0
    */
    class _EngineAPI InstanceSpace : public BatchObjectAlloc
    {
    public:
        class LODBatch;
        class MaterialBatch;
        class BatchObj;
        class InstancedObject;
        class OptMeshGeo;
        struct OptMeshGeoRef;
        struct MeshRef;
        struct Geometry;

        typedef vector<OptMeshGeo *>::type OptMeshGeoList;
        typedef vector<OptMeshGeoRef>::type OptMeshGeoRefList;
        typedef vector<MeshRef *>::type MeshRefList;
        typedef vector<Geometry *>::type GeometryList;
        typedef map<SubMesh *, OptMeshGeoRefList *>::type MeshGeoRefList;

        /// 几何批次
        class _EngineAPI GeometryBatch : public RenderObj
        {
        public:
            GeometryBatch(MaterialBatch * batch, GeometryBatch * obatch);
            GeometryBatch(MaterialBatch * batch, const VertexData * vdata, const IndexData * idata);
            GeometryBatch(SpaceID sid, MaterialBatch * batch, GeometryBatch * obatch);
            GeometryBatch(SpaceID sid, MaterialBatch * batch, const VertexData * vdata, const IndexData * idata);
            ~GeometryBatch();

            /** 添加几何
            @version NIIEngine 3.0.0
            */
            bool add(Geometry * geo);

            /** 创建
            @version NIIEngine 3.0.0
            */
            void build();

            /// 获取顶点数据
            inline const VertexData * getVertexData() const { return mVertexData; }

            /// 获取索引数据
            inline const IndexData * getIndexData() const { return mIndexData; }

            /// 获取材质池
            inline MaterialBatch * getMaterialBatch() { return mMaterialBatch; }

            /// @copydetails SpaceObj::getCenterRange
            NIIf getCenterRange() const;

            /// @copydetails GeometryObj::getMaterial
            const Material * getMaterial() const;

            /// @copydetails GeometryObj::getShaderFusion
            ShaderFusion * getShaderFusion() const;

            /// @copydetails GeometryObj::getMatrix
            void getMatrix(Matrix4f * out, NCount & count) const;

            /// @copydetails GeometryObj::getMatrixCount
            virtual NCount getMatrixCount() const ;

            /// @copydetails GeometryObj::distanceSquared
            NIIf distanceSquared(const Camera * cam) const;

            /// @copydetails GeometryObj::getLight
            void getLight(LightList & ll) const;

            /// @copydetails GeometryObj::isCastShadow
            bool isCastShadow(void) const;

            /// @copydetails SpaceObj::getAABB
            AABox & getAABB(void){ return mAABB; }

            /// @copydetails SpaceObj::query
            void query(GeometryQuery * query);
        protected:
            void setupData(const VertexData * vdata, const IndexData * idata);

            void setupData(GeometryBatch * batch);
        protected:
            InstanceSpace * mManager;
            MaterialBatch * mMaterialBatch;
            VertexData * mVertexData;
            IndexData * mIndexData;
            GeometryList mGeometryList;
            Nui16 mTexCoordIndex;
            AABox mAABB;
            bool mIdx32;
        };
        typedef vector<GeometryBatch *>::type GeometryBatchList;

        /// 材质批次
        class _EngineAPI MaterialBatch : public BatchObjectAlloc
        {
            friend class InstanceSpace;
            friend class GeometryBatch;
        public:
            typedef map<String, GeometryBatch *>::type GeoBatchMarkList;
        public:
            MaterialBatch(LODBatch * batch, ResourceID mid);
            virtual ~MaterialBatch();

            /// 添加
            void add(Geometry * geo);

            /// 创建
            void build();

            /// 排列到渲染队列
            void queue(RenderQueue * queue, GroupID group, NIIf dstlod);

            /// 查询几何
            void query(GeometryQuery * query);

            /// 获取批次
            inline LODBatch * getLODBatch() { return mLODBatch; }

            /// 获取材质ID
            inline ResourceID getMaterialID() const { return mMaterialID; }

            /// 获取材质
            inline const Material * getMaterial() const { return mMaterial; }

            /// 获取混合
            inline ShaderFusion * getShaderFusion() const { return mShaderFusion; }

            /// 获去几何列表
            const GeometryBatchList & getGeometryList() const;
        protected:
            LODBatch * mLODBatch;
            ResourceID mMaterialID;
            Material * mMaterial;
            ShaderFusion * mShaderFusion;
            GeometryBatchList mGeometryBatchList;
            GeoBatchMarkList mGeoBatchMarkList;
            NIIi mLastIndex;
        };

        /// LOD批次
        class _EngineAPI LODBatch : public BatchObjectAlloc
        {
            friend class InstanceSpace;
        public:
            typedef map<ResourceID, MaterialBatch *>::type MaterialBatchList;
        public:
            LODBatch(BatchObj * batch, Nui16 lod);
            virtual ~LODBatch();

            inline BatchObj * getBatchObj() const { return mBatchObj; }

            /// 获取lod索引
            Nui16 getLod() const { return mLod; }

            /// 添加
            void add(MeshRef * ref, Nui16 lod);

            /// 创建
            void build();

            /// 查询
            void query(GeometryQuery * query);

            /// 排列到渲染队列
            void queue(RenderQueue * queue, GroupID group, NIIf dstlod);

            /// 获取材质批次列表
            const MaterialBatchList & getMaterialList() const;
        protected:
            BatchObj * mBatchObj;
            GeometryList mGeometryList;
            MaterialBatchList mMaterialBatchList;
            Nui16 mLod; 
        };
        typedef vector<LODBatch *>::type LODBatchList;

        class _EngineAPI InstancedObject : public AnimationFusionObj
        {
            friend class GeometryBatch;
            friend class InstanceSpace;
        public:
            enum SpaceMode
            {
                SM_Local,
                SM_Env,
                SM_Space
            };
        public:
            InstancedObject(Nui16 index);
            InstancedObject(Nui16 index, SkeletonFusion * fusion, AnimationFusionObj * ani);
            ~InstancedObject();

            void setPos(Vector3f pos);

            const Vector3f& getPos() const;

            void setScale(const Vector3f & scale);

            const Vector3f & getScale() const;

            void setOri(const Quaternion & q);

            const Quaternion & getOri();

            void translate(const Matrix3f & axes, const Vector3f & move);

            void translate(const Vector3f & d);

            void yaw(const Radian & angle);

            void pitch(const Radian & angle);

            void roll(const Radian & angle);

            void rotate(const Quaternion & rot);

            void updateAnimation();
        protected:
            Nui16 mIndex;
            Matrix4f mMatrix;
            Vector3f mScale;
            Vector3f mPosition;
            Quaternion mOrientation;
            SkeletonFusion * mSkeletonFusion;
            Matrix4f * mBoneMatrix;
            NCount mBoneCount;
            GeometryBatchList mGeometryBatchList;
        };

        /** 批次对象
        @version NIIEngine 3.0.0
        */
        class _EngineAPI BatchObj : public SpaceObj
        {
            friend class InstanceSpace;
            friend class MaterialBatch;
        public:
            typedef map<Nui16, InstancedObject *>::type ObjectList;
            typedef vector<NIIf>::type LodValueList;
        public:
            BatchObj(InstanceSpace * parent, SpaceID sid, SpaceManager * mag, Nid bid);
            ~BatchObj();

            inline InstanceSpace * getParent() const { return mParent; }

            /** 添加对象
            @param[in] index 实例对象ID
            */
            void addObject(Nui16 index, InstancedObject * object);

            /** 对象是否存在
            @param[in] index 实例对象的ID
            */
            InstancedObject * isObjectExist(Nui16 index);

            /// 获取对象列表
            inline const ObjectList & getObjectList() const     { return mObjectList; }

            /// 添加
            void add(MeshRef * qmesh);

            /// 构建
            void build();

            /// 获取ID
            inline Nui32 getID() const                          { return mBatchID; }

            /// 获取所相对的场景节点
            inline SpaceNode * getSceneNode() const             { return mNode; }

            /// 获取LOD列表
            const LODBatchList & getLODList() const ;

            /// 更新边界盒
            void updateAABB();

            using SpaceObj::query;

            /// @copydetails SpaceObj::getFactoryID
            FactoryID getFactoryID() const;

            /// @copydetails SpaceObj::_notify
            void _notify(Camera * cam);

            /// @copydetails SpaceObj::getAABB
            const AABox & getAABB() const;

            /// @copydetails SpaceObj::setAABB
            void setAABB(AABox & box);

            /// @copydetails SpaceObj::getCenterRange
            NIIf getCenterRange() const;

            /// @copydetails SpaceObj::_update
            void queue(RenderQueue * queue);

            /// @copydetails SpaceObj::isVisible
            bool isVisible() const;

            /// @copydetails SpaceObj::query
            void query(GeometryQuery * query);
        protected:
            SpaceManager * mManager;
            InstanceSpace * mParent;
            SpaceNode * mNode;
            Nui32 mBatchID;
            MeshRefList mMeshRefList;
            AABox mAABB;
            NIIf mBoundRange;
            Nui16 mLodIdx;
            NIIf mLodValue;
            Camera * mCamera;
            NIIf mSquaredViewDepth;
            ObjectList mObjectList;
            LODBatchList mLODBatchList;
            const MeshLodScheme * mLodScheme;
        };
        typedef map<Nui32, BatchObj *>::type BatchObjectList;
    public:
        InstanceSpace(SpaceManager * mag, SpaceID sid);
        ~InstanceSpace();

        /** 获取这个对象ID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const { return mID; }

        /** 添加对象
        @version NIIEngine 3.0.0
        */
        void add(GeometrySpace * obj, const Vector3f & pos, const Quaternion & rot = Quaternion::IDENTITY, 
            const Vector3f & scale = Vector3f::UNIT);

        /** 添加对象
        @version NIIEngine 3.0.0
        */
        void add(const SpaceNode * node);

        /** 移去所有
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 建立几何
        @version NIIEngine 3.0.0
        */
        void build();

        /** 删除几何
        @version NIIEngine 3.0.0
        */
        void destroy();

        /** 查询几何
        @version NIIEngine 3.0.0
        */
        void query(GeometryQuery * query);

        /** 设置可见距离
        @version NIIEngien 3.0.0
        */
        void setVisibleDistance(NIIf dist);

        /** 获取可见距离
        @version NIIEngine 3.0.0
        */
        inline NIIf getVisibleDistance() const { return mVisibleDistance; }

        /** 设置是否可见
        @version NIIEngine 3.0.0
        */
        void setVisible(bool set);

        /** 获取是否可见
        @version NIIEngine 3.0.0
        */
        inline bool isVisible() const { return mVisible; }

        /** 是否投影阴影
        @version NIIEngine 3.0.0
        */
        void setCastShadow(bool set);

        /** 获取是否投影阴影
        @version NIIEngine 3.0.0
        */
        inline bool isCastShadow() const { return mCastShadow; }

        /** 获取批次尺寸
        @version NIIEngine 3.0.0
        */
        void setBatchSize(const Vector3f & size);

        /** 设置批次尺寸
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getBatchSize() const { return mBatchSize; }

        /** 设置几何原点
        @version NIIEngine 3.0.0
        */
        void setOrigin(const Vector3f & origin);

        /** 获取几何原点
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getOrigin() const { return mOrigin; }

        /** 设置渲染群组
        @version NIIEngine 3.0.0
        */
        void setRenderGroup(Nui16 rqid);

        /** 获取渲染群组
        @version NIIEngine 3.0.0
        */
        Nui16 getRenderGroup() const;

        /// 获取批次列表
        const BatchObjectList & getBatchObjList() const;
    protected:
        /// 获取批次
        BatchObj * getBatchObj(const AABox & aabb);

        /// 获取批次
        BatchObj * getBatchObj(const Vector3f & point);

        /// 获取批次
        BatchObj * getBatchObj(Nui16 x, Nui16 y, Nui16 z);

        /// 获取BatchObj索引
        virtual void getBatchIndex(const Vector3f & point, Nui16& x, Nui16& y, Nui16& z);

        /// 获取体积相交体积
        virtual NIIf getVolume(const AABox & intersect, Nui16 x, Nui16 y, Nui16 z);

        /// 获取批次AABB
        virtual AABox getBatchAABB(Nui16 x, Nui16 y, Nui16 z);

        /// 获取批次中心
        virtual Vector3f getBatchCentre(Nui16 x, Nui16 y, Nui16 z);

        /// 创建引用列表
        OptMeshGeoRefList * createRefList(SubMesh * sm);

        /// 创建引用
        void createRef(OptMeshGeoRef * ref, VertexData * vd, IndexData * id);
    protected:
        SpaceID mID;
        SpaceManager * mManager;
        OptMeshGeoList mOptMeshGeoList;
        MeshGeoRefList mMeshGeoRefList;
        BatchObjectList mBatchObjectList;
        GeometryRawList mGeometryRawList;
        Vector3f mBatchSize;
        Vector3f mOrigin;
        Nui16 mRenderGroup;
        Nui32 mObjectCount;
        NIIf mVisibleDistance;
        MeshRefList mMeshRefList;
        BatchObj * mBatchObj;
        Skeleton * mSkeleton;
        SkeletonFusion * mSkeletonFusion;
        AnimationFusionObj * mAnimationCtrl;
        bool mVisible;
        bool mCastShadow;
        bool mBuilt;
    };
}
#endif