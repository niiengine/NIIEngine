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

#ifndef _NII_StaticSpace_H_
#define _NII_StaticSpace_H_

#include "NiiPreInclude.h"
#include "NiiShadowVolumeObj.h"
#include "NiiMesh.h"
#include "NiiLodScheme.h"

namespace NII
{
    /** 网格批次,高效场景静态几何
    @verssion NIIEngine 4.0.0
    */
    class _EngineAPI StaticSpace : public BatchObjectAlloc
    {
    public:
        class LODBatch;
        class MaterialBatch;
        class BatchObj;
        class OptMeshGeo;
        struct OptMeshGeoRef;
        struct MeshRef;
        struct Geometry;
        typedef vector<OptMeshGeo *>::type OptMeshGeoList;
        typedef vector<OptMeshGeoRef>::type OptMeshGeoRefList;
        typedef vector<MeshRef *>::type MeshRefList;
        typedef vector<Geometry *>::type GeometryList;
        typedef map<SubMesh *, OptMeshGeoRefList *>::type MeshGeoRefList;

        /** 批次几何
        @version NIIEngine 3.0.0
        */
        class _EngineAPI GeometryBatch : public GeometryObj, public BatchObjectAlloc
        {
        public:
            GeometryBatch(MaterialBatch * batch, const VertexData * vdata, const IndexData * idata);
            ~GeometryBatch();

            /** 添加几何
            @version NIIEngine 3.0.0
            */
            bool add(Geometry * geo);

            /** 创建
            @version NIIEngine 3.0.0
            */
            void build(bool stencilShadow);

            /// 材质批次
            inline MaterialBatch * getMaterialBatch() { return mMaterialBatch; }

            /// 获取顶点数据
            inline const VertexData * getVertexData() const { return mVertexData; }

            /// 获取索引数据
            inline const IndexData * getIndexData() const { return mIndexData; }

            /// @copydetails GeometryObj::getMaterial
            const Material * getMaterial() const;

            /// @copydetails GeometryObj::getShaderFusion
            ShaderFusion * getShaderFusion() const;

            /// @copydetails GeometryObj::getGeometry
            void getGeometry(GeometryRaw & op) const;

            /// @copydetails GeometryObj::getMatrix
            void getMatrix(Matrix4f * out, NCount & count) const;

            /// @copydetails GeometryObj::distanceSquared
            NIIf distanceSquared(const Camera * cam) const;

            /// @copydetails GeometryObj::getLight
            void getLight(LightList & ll) const;

            /// @copydetails GeometryObj::isCastShadow
            bool isCastShadow() const;
        protected:
            GeometryList mGeometryList;
            MaterialBatch * mMaterialBatch;
            VertexData * mVertexData;
            IndexData * mIndexData;
            bool mIdx32;
        };
        typedef vector<GeometryBatch *>::type GeometryBatchList;

        /** 批次材质几何
        @version NIIEngine 3.0.0
        */
        class _EngineAPI MaterialBatch : public BatchObjectAlloc
        {
        public:
            typedef map<String, GeometryBatch *>::type GeoBatchMarkList;
        public:
            MaterialBatch(LODBatch * parent, ResourceID rid);

            ~MaterialBatch();

            /// 添加几何
            void add(Geometry * geo);

            /// 创建
            void build(bool stencilShadow);

            /// 查询几何
            void query(GeometryQuery * query);

            /// 排列到渲染队列
            void queue(RenderQueue * queue, Nui16 group, NIIf lod);

            /// 获取批次
            inline LODBatch * getLODBatch() const { return mLODBatch; }

            /// 获取材质ID
            inline ResourceID getMaterialID() const { return mMaterialID; }

            /// 为这个桶获取材质
            inline const Material * getMaterial() const { return mMaterial; }

            /// 获取混合
            inline ShaderFusion * getShaderFusion() const { return mShaderFusion; }

            /// 获几何列表
            const GeometryBatchList & getGeometryList() const;
        protected:
            LODBatch * mLODBatch;
            Material * mMaterial;
            ResourceID mMaterialID;
            ShaderFusion * mShaderFusion;
            GeoBatchMarkList mGeoBatchMarkList;
            GeometryBatchList mGeometryBatchList;
        };

        /** 批次同细节材质几何
        @version NIIEngine 3.0.0
        */
        class _EngineAPI LODBatch : public BatchObjectAlloc
        {
            friend class BatchObj;
            class ShadowVolume;
        public:
            typedef map<ResourceID, MaterialBatch *>::type MaterialBatchList;
        public:
            LODBatch(BatchObj * parent, Nui16 lod);
            ~LODBatch();

            /// 获取批次对象
            inline BatchObj * getBatchObj() const { return mBatchObj; }

            /// 添加
            void add(MeshRef * ref, Nui16 lod);

            /// 创建
            void build(bool stencilShadow);

            /// 获取LOD索引
            inline Nui16 getLod() const { return mLod; }

            /// 查询几何
            void query(GeometryQuery * query);

            /// 排列到渲染队列
            void queue(RenderQueue * queue, Nui16 group, NIIf lodValue);

            /// 获取材质批次列表
            const MaterialBatchList & getMaterialList() const;
        protected:
            BatchObj * mBatchObj;
            ShadowVolume * mShadowVolume;
            MaterialBatchList mMaterialBatchList;
            GeometryList mGeometryList;
            Nui16 mLod;
        };
        typedef vector<LODBatch *>::type LODBatchList;

        /** 批次对象
        @version NIIEngine 3.0.0
        */
        class _EngineAPI BatchObj : public ShadowVolumeObj
        {
            friend class MaterialBatch;
            friend class GeometryBatch;
        public:
            typedef vector<NIIf>::type LodValueList;
        public:
            BatchObj(StaticSpace * space, SpaceID sid, SpaceManager * mag,
                Nui32 regionID, const Vector3f & centre);
            ~BatchObj();

            inline StaticSpace * getParent() const { return mParent;}

            /// 添加
            void add(MeshRef * ref);

            /// 构建
            void build(bool stencilShadow);

            /// 获取ID
            inline Nui32 getID() const { return mRegionID; }

            /// 获取中心点
            inline const Vector3f & getCentre() const { return mCentre; }

            /// 获取LOD列表
            const LODBatchList & getLODList() const;

            using SpaceObj::query;

            /// @copydetails SpaceObj::getFactoryID
            FactoryID getFactoryID() const;

            /// @copydetails SpaceObj::_notify
            void _notify(Camera * cam);

            /// @copydetails SpaceObj::getAABB
            const AABox & getAABB() const;

            /// @copydetails SpaceObj::getCenterRange
            NIIf getCenterRange() const;

            /// @copydetails SpaceObj::_update
            void queue(RenderQueue * queue);

            /// @copydetails SpaceObj::query
            void query(GeometryQuery * query);

            /// @copydetails SpaceObj::isVisible
            bool isVisible() const;

            /// @copydetails SpaceObj::getTypeMark
            Nui32 getTypeMark() const;

            /// @copydetails ShadowVolumeObj::getEdgeList
            GeoEdget * getEdgeList();

            /// @copydetails ShadowVolumeObj::isEdgeExist
            bool isEdgeExist(void);

            /// @copydetails ShadowVolumeObj::getFrontCapAABB
            const AABox & getFrontCapAABB() const;
            
            /// @copydetails ShadowVolumeObj::getVolumeList
            const ShadowVolumeList & getVolumeList(const Light * srclight, NIIf extent, 
                 Nmark flags, IndexBuffer * destindex);
                
            /// @copydetails ShadowVolumeObj::getExtrudeDistance
            NIIf getExtrudeDistance(const Light * l) const;
        protected:
            Nui32 mRegionID;
            Camera * mCamera;
            SpaceNode * mNode;
            StaticSpace * mParent;
            SpaceManager * mManager;
            MeshRefList mMeshRefList;
            LODBatchList mLODBatchList;
            AABox mAABB;
            Vector3f mCentre;
            NIIf mBoundRange;
            Nui16 mLodIdx;
            NIIf mLodValue;
            const MeshLodScheme * mLodScheme;
            NIIf mSquaredViewDepth;
        };

        typedef map<Nui32, BatchObj *>::type BatchList;
    public:
        StaticSpace(SpaceManager * mag, SpaceID sid);
        ~StaticSpace();

        /** 获取这个对象ID
        @version NIIEngine 3.0.0
        */
        inline SpaceID getID() const { return mID; }

        /** 查询几何
        @version NIIEngine 3.0.0
        */
        void query(GeometryQuery * query);

        /** 添加对象
        @version NIIEngine 3.0.0
        */
        void add(GeometrySpace * obj, const Vector3f & pos,
            const Quaternion & rot = Quaternion::IDENTITY, const Vector3f& scale = Vector3f::UNIT);

        /** 添加节点
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
        bool isVisible() const;

        /** 是否投影阴影
        @version NIIEngine 3.0.0
        */
        void setCastShadow(bool set);

        /** 获取是否投影阴影
        @version NIIEngine 3.0.0
        */
        bool isCastShadow() const;

        /** 设置
        @version NIIEngine 3.0.0
        */
        void setBatchSize(const Vector3f & size);

        /** 设置
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getBatchSize() const { return mBatchSize; }

        /** 设置原点
        @version NIIEngine 3.0.0
        */
        void setOrigin(const Vector3f & origin);

        /** 获取原点
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getOrigin() const { return mOrigin; }

        /** 可见标记
        @version NIIEngine 3.0.0
        */
        void setVisibleMark(Nmark mark);

        /** 可见标记
        @version NIIEngine 3.0.0
        */
        Nmark getVisibleMark() const;

        /** 设置渲染群组
        @version NIIEngine 3.0.0
        */
        void setRenderGroup(Nui16 gid);

        /** 获取渲染群组
        @version NIIEngine 3.0.0
        */
        Nui16 getRenderGroup() const;

        /** 获取批次列表
        @version NIIEngine 3.0.0
        */
        const BatchList & getBatchList() const;
    protected:
        /// 获取 BatchObj
        BatchObj * getBatchObj(const AABox & bounds);

        /// 获取 BatchObj
        BatchObj * getBatchObj(const Vector3f & point);

        /// 获取 BatchObj
        BatchObj * getBatchObj(Nui16 idxx, Nui16 idxy, Nui16 idxz);

        /// 获取索引
        void getBatchIndex(const Vector3f & point, Nui16 & idxx, Nui16 & idxy, Nui16 & idxz);

        /// 获取边界
        AABox getBatchAABB(Nui16 x, Nui16 y, Nui16 z);

        /// 获取中心
        Vector3f getBatchCentre(Nui16 x, Nui16 y, Nui16 z);

        /// 使用体积
        NIIf getVolume(const AABox & intersect, Nui16 idxx, Nui16 idxy, Nui16 idxz);

        /// 创建引用数据列表
        OptMeshGeoRefList * createRefList(SubMesh * sm);

        /// 创建引用数据
        void createRef(OptMeshGeoRef * ref, VertexData * vd, IndexData * id);
    protected:
        SpaceID mID;
        SpaceManager * mManager;
        NIIf mVisibleDistance;
        Vector3f mBatchSize;
        Vector3f mOrigin;
        Nui16 mRenderGroup;
        Nui32 mVisibleMark;
        MeshRefList mMeshRefList;
        OptMeshGeoList mOptMeshGeoList;
        MeshGeoRefList mMeshGeoRefList;
        BatchList mBatchList;
        bool mCastShadow;
        bool mBuilt;
        bool mVisible;
    };
}
#endif