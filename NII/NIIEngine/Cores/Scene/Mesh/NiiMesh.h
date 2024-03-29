﻿/*
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

#ifndef _NII_MESH_H_
#define _NII_MESH_H_

#include "NiiPreInclude.h"
#include "NiiAnimationObj.h"
#include "NiiMeshLodScheme.h"
#include "NiiResource.h"
#include "NiiDataStream.h"
#include "NiiGeometryRaw.h"
#include "NiiSkeleton.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiAABox.h"
#include "NiiGeoEdget.h"
#include "NiiCurvedSurface.h"
#include "NiiIDManager.h"

namespace NII
{
    class MeshLodScheme;

    /** 网格成分
    @remark 主要是为了维持网格多样化设计的网格成分类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SubMesh : public VertexDataAlloc
    {
        friend class Mesh;
        friend class GeometryCom;
    public:
        SubMesh();
        SubMesh(const Mesh * obj);
        ~SubMesh();

        /** 设置名字(辅助)
        @version NIIEngine 3.0.0
        */
        inline void setName(const String & n)       { mName = n; }

        /** 获取名字(辅助)
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const       { return mName; }

        /** 获取子网格所属的网格
        @version NIIEngine 3.0.0
        */
        inline Mesh * getMesh() const               { return mMesh; }

        /** 渲染几何
        @param[in] gr 几何数据
        @param[in] lod 所使用的LOD索引.
        @version NIIEngine 3.0.0
        */
        void getGeometry(GeometryRaw & raw, Nidx lod = 0) const;

        /** 添加骨骼顶点
        @version NIIEngine 3.0.0
        */
        void addSkeleton(const SkeletonVertex & sv);

        /** 移去骨骼顶点
        @param[in] vindex 顶点索引
        @version NIIEngine 3.0.0
        */
        void removeSkeleton(Nidx vindex);

        /** 移去所有骨骼顶点
        @version NIIEngine 3.0.0
        */
        void clearSkeleton();

        /** 骨骼矩阵
        @version NIIEngine 3.0.0
        */
        inline const BoneIndexList & getMatrixList() const              { return mMatrixs; }

        /** 获取骨骼顶点列表
        @version NIIEngine 3.0.0
        */
        inline const SkeletonVertexList & getSkeletonList() const       { return mSkeletonVertexList; }

        /** 设置是否使用独立的顶点数据
        @param[in] b
        @version NIIEngine 3.0.0
        */
        inline void setSelfVertex(bool b)                               { mSelfVertex = b; }

        /** 获取使用独立的顶点数据
        @version NIIEngine 3.0.0
        */
        inline bool isSelfVertex() const                                { return mSelfVertex; }

        /** 设置几何类型
        @version NIIEngine 3.0.0
        */
        inline void setGeometryType(GeometryRaw::OperationType type)    { mType = type; }

        /** 获取几何类型
        @version NIIEngine 3.0.0
        */
        inline GeometryRaw::OperationType getGeometryType() const       { return mType; }

        /** 设置顶点数据
        @remark 这是个复杂的体系,包含了法线,切线,骨骼混合等
        @note 设置后内存将由这个网格控制
        @version NIIEngine 3.0.0 顶级api
        */
        inline void setVertexData(VertexData * vd)                      { mVertex = vd; }

        /** 获取顶点数据
        @remark 这是个复杂的体系,包含了法线,切线,骨骼混合等
        @version NIIEngine 3.0.0 高级api
        */
        inline VertexData * getVertexData() const                       { return mVertex; }

        /** 设置LOD顶点索引数据
        @param[in] lod LOD等级
        @param[in] id 索引
        @version NIIEngine 3.0.0 高级api
        */
        void setIndexData(Nidx lod, IndexData * id);

        /** 获取LOD顶点索引数据
        @param[in] lod LOD等级
        @version NIIEngine 3.0.0 高级api
        */
        inline IndexData * getIndexData(Nidx lod = 0) const             { return mIndexList[lod]; }

        /** 设置是否自动生成LOD
        @version NIIEngine 3.0.0
        */
        void setAutoLod(bool b);

        /** 获取是否自动生成LOD
        @version NIIEngine 3.0.0
        */
        inline bool isAutoLod() const                   { return mAutoLod; }

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        inline void setMaterial(ResourceID rid)         { mMaterialID = rid; }

        /** 获取材质
        @version NIIEngine 3.0.0
        */
        inline ResourceID getMaterial() const           { return mMaterialID; }

        /** 添加LOD顶点索引数据
        @param[in] id
        @version NIIEngine 3.0.0
        */
        inline void addIndexData(IndexData * id)        { mIndexList.push_back(id); }

        /** 设置LOD数量
        @version NIIEngine 3.0.0
        */
        void setLodCount(NCount c);

        /** 获取LOD数量
        @version NIIEngine 3.0.0
        */
        NCount getLodCount() const;

        /** 移去所有LOD
        @version NIIEngine 3.0.0
        */
        void removeAllLod();

        /** 构建骨骼结构到几何数据中
        @version NIIEngine 3.0.0
        */
        void buildSkeleton();

        /** 建立边缘点
        @param[in] count 数量
        @version NIIEngine 3.0.0
        */
        void buildEdge(NCount count);

        /** 添加边缘点
        @version NIIEngine 3.0.0
        */
        inline void addEdgePoint(const Vector3f & v)        { mEdgePoints.push_back(v); }

        /** 移去所有边缘点
        @version NIIEngine 3.0.0
        */
        inline void removeAllEdgePoint()                    { mEdgePoints.clear(); }

        /** 获取边缘点
        @version NIIEngine 3.0.0
        */
        inline const Vector3List & getEdgePoints() const    { return mEdgePoints; }
    protected:
        Mesh * mMesh;                               ///< 父对象
        String mName;                               ///< 辅助名字
        ResourceID mMaterialID;                     ///< 材质ID
        GeometryRaw::OperationType mType;           ///< 渲染操作类型,用于渲染这个子网格
        VertexData * mVertex;                       ///< 顶点数据
        IndexDataList mIndexList;                   ///< 顶点索引
        Vector3List mEdgePoints;                    ///< 边缘顶点
        BoneIndexList mMatrixs;                     ///< 顶点骨骼矩阵
        SkeletonVertexList mSkeletonVertexList;
        bool mSkeletonVexrtexValid;
        bool mSelfVertex;
        bool mAutoLod;
    };

    /** 网格
    @remark
        这个类直接和模型格式切切相关,它的设计直接关系到以后的模型从blend3d softimage 3dmax
        等软件中导出,而且还有存在关于扩展格式的(.3ds, .md5, .mdl等)的直接使用,和以后的工作
        效率关系大.还有一点就是录制骨骼最好使用三轴传感器(集)设备(Triaxial sensor)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Mesh : public AnimationObj, public Resource
    {
        friend class GeometrySpace;
        friend class SubMesh;
        friend class VertexOffsetFusion;
        friend class VertexUnitFusion;
        friend class MeshSerializer;
    public:
        /** 网格中顶点的成分类型
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_None          = 0,
            M_Edge          = 0x01,
            M_Skeleton      = 0x02,
            M_ShadowVol     = 0x04,
            M_ObjSpace      = 0x08,
            M_TangentSpace  = 0x10,
            M_NormalsSpace  = 0x20,
            M_ShareVertex   = 0x40,
            M_AutoLod       = 0x80
        };
    public:
        Mesh(ResourceID rid, GroupID gid, ResLoadScheme * rls = 0, 
            ResResultScheme * rrs = 0, ScriptTypeID stid = N_CmdObj_Mesh, LangID lid = N_PrimaryLang);

        virtual ~Mesh();

        using Resource::remove;

        /** 构建网格扩展模式
        @param[in] mode 扩展模式
        @version NIIEngine 3.0.0
        */
        void build(Mode mode);

        /** 移去网格扩展模式
        @version NIIEngine 3.0.0
        */
        void remove(Mode mode);

        /** 是否存在网格扩展模式
        @version NIIEngine 3.0.0
        */
        bool isExist(Mode mode)                 { return mMark & mode; }

        /** 顶点内存模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setVertexMode(Nmark mode)          { mVertexMode = mode; }

        /** 顶点内存模式
        @version NIIEngine 3.0.0
        */
        Nmark getVertexMode() const             { return mVertexMode; }

        /** 顶点索引内存模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setIndexMode(Nmark mode)           { mIndexMode = mode; }

        /** 顶点索引内存模式.
        @version NIIEngine 3.0.0
        */
        Nmark getIndexMode() const              { return mIndexMode; }

        /** 手动设置这个网格的AABB.
        @version NIIEngine 3.0.0
        */
        void setAABB(const AABox & aab)         { mAABB = aab; mCenterRange = mAABB.calcRadius(); }

        /** 获取这个网格的AABB.
        @version NIIEngine 3.0.0
        */
        const AABox & getAABB() const           { return mAABB; }

        /** 手动设置边界半径.
        @param[in] f 半径
        @version NIIEngine 3.0.0
        */
        void setCenterRange(NIIf r)             { mCenterRange = r; }

        /** 获取围绕这个网格的边界球
        @version NIIEngine 3.0.0
        */
        NIIf getCenterRange() const             { return mCenterRange; }

        /** 设置范围因子
        @param[in] f
        @verion NIIEngine 3.0.0
        */
        void setRangeFactor(NIIf f);

        /** 获取范围因子
        @version NIIEngine 3.0.0
        */
        NIIf getRangeFactor() const             { return mRangeFactor; }

        /** 设置预制类型
        @note 仅在网格加载前有效
        @version NIIEngine 3.0.0
        */
        void setPrefabType(GeometryPrefab type) { mPrefabType = type; }

        /** 获取预制类型
        @note 仅在网格加载前有效
        @version NIIEngine 3.0.0
        */
        GeometryPrefab getPrefabType() const    { return mPrefabType; }

        /** 设置是否自动构建Lod
        @version NIIEngine 3.0.0
        */
        void setAutoLod(bool b);

        /** 获取是否自动构建Lod
        @version NIIEngine 3.0.0
        */
        bool getAutoLod() const;

        /** 设置是否构建几何边缘
        @version NIIEngine 3.0.0
        */
        void setGenEdget(bool b);

        /** 获取是否构建几何边缘
        @version NIIEngine 3.0.0
        */
        bool isGenEdget() const;

        /** 设置骨骼资源ID
        @version NIIEngine 3.0.0
        */
        void setSkeletonID(SkeletonID sid);

        /** 获取骨骼资源ID
        @version NIIEngine 3.0.0
        */
        SkeletonID getSkeletonID() const        { return mSkeletonID; }

        /** 设置骨骼
        @version NIIEngine 3.0.0
        */
        void setSkeleton(Skeleton * obj);

        /** 获取骨骼
        @version NIIEngine 3.0.0
        */
        Skeleton * getSkeleton() const          { return mSkeleton; }

        /** 设置顶点数据
        @remark 这是个复杂的体系,包含了法线,切线,骨骼混合等
        @note 设置后内存将由这个网格控制
        @version NIIEngine 3.0.0 顶级api
        */
        void setVertexData(VertexData * vd);

        /** 获取顶点数据
        @remark 这是个复杂的体系,包含了法线,切线,骨骼混合等
        @version NIIEngine 3.0.0 高级api
        */
        VertexData * getVertexData() const      { return mVertex; }

        /** 创建子网格
        @version NIIEngine 3.0.0
        */
        SubMesh * createSub();

        /** 删除子网格
        @version NIIEngine 3.0.0
        */
        void destroySub(Nidx idx);

        /** 获取子网格
        @version NIIEngine 3.0.0
        */
        inline SubMesh * getSub(Nidx idx) const         { N_assert(idx < mSubMeshList.size(), "error"); return mSubMeshList[idx]; }

        /** 获取子网格数量
        @version NIIEngine 3.0.0
        */
        inline NCount getSubCount() const               { return mSubMeshList.size(); }

        /** 获取子网格列表
        @version NIIEngine 3.0.0
        */
        inline const SubMeshList & getSubList() const   { return mSubMeshList; }

        /** 加入骨骼顶点
        @version NIIEngine 3.0.0
        */
        void addSkeleton(const SkeletonVertex & vba);

        /** 移去骨骼顶点
        @param[in] vindex 顶点索引
        @version NIIEngine 3.0.0
        */
        void removeSkeleton(Nidx vindex);

        /** 移去所有骨骼顶点
        @param[in]
        @version NIIEngine 3.0.0
        */
        void clearSkeleton();

        /** 获取距阵映射
        @version NIIEngine 3.0.0
        */
        const BoneIndexList & getMatrixList() const             { return mMatrixs; }

        /** 获取到骨头分配列表
        @version NIIEngine 3.0.0 高级api
        */
        const SkeletonVertexList & getSkeletonList() const      { return mSkeletonVertexList; }

        /** 创建顶点偏移集
        @param[in] id 偏移的ID
        @param[in] vertidx 使用偏移的顶点
        @version NIIEngine 3.0.0
        */
        VertexOffset * createOffset(Nid id, Nui16 vertidx);

        /** 获取顶点偏移集
        @param[in] id
        @version NIIEngine 3.0.0
        */
        VertexOffset * getOffset(Nid id);

        /** 获取顶点偏移集
        @param[in] index
        @version NIIEngine 3.0.0
        */
        VertexOffset * getOffset(Nidx index);

        /** 移去顶点偏移集
        @param[in] id
        @version NIIEngine 3.0.0
        */
        void removeOffset(Nid id);

        /** 移去顶点偏移集
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void removeOffset(Nidx index);

        /** 移去所有顶点偏移集
        @version NIIEngine 3.0.0
        */
        void removeAllOffset();

        /** 获取顶点偏移集数量
        @version NIIEngine 3.0.0
        */
        inline NCount getOffsetCount() const                    { return mVertexOffsetList.size(); }

        /** 获取偏移集
        @version NIIEngine 3.0.0
        */
        inline const VertexOffsetList & getOffsetList() const   { return mVertexOffsetList; }

        /** 设置LOD数量
        @version NIIEngine 3.0.0
        */
        void setLodCount(NCount c);

        /** 获取LOD数量
        @remark 这个数量包含原始模型
        */
        NCount getLodCount() const;

        /** 移去所有Lod
        @version NIIEngine 3.0.0
        */
        void removeAllLod();

        /** 设置这个网格使用的LOD策略.
        @param[in] ls LOD策略
        @version NIIEngine 3.0.0
        */
        void setLodScheme(MeshLodScheme * ls);

        /** 获取网格LOD
        @version NIIEngine 3.0.0
        */
        MeshLodScheme * getLodScheme() const            { return mLodScheme; }

        /** 设置LOD网格边缘
        @note 参数2内存将交由本类管理
        @version NIIEngine 3.0.0 高级api
        */
        void setEdgeList(Nidx lod, GeoEdget * edg);

        /** 获取LOD网格边缘
        @version NIIEngine 3.0.0
        */
        GeoEdget * getEdgeList(Nidx lod = 0) const;

        /** 构建切线
        @remark 用于顶点和片段着色程序中
        @param[in] dtype 存储切线实际的类型
        @param[in] build4D 是否分割顶点
        @param[in] mirrorUV 是否对称UV
        @param[in] rotationUV 是否旋转UV
        @version NIIEngine 3.0.0 高级api
        */
        void buildTangentBuffer(VertexType dtype = VT_Tangent, bool build4D = false,
            bool mirrorUV = false, bool rotationUV = false);

        /** 检测切线
        @param[in] coordtype 是否使用纹理坐标类型存储切线
        @version NIIEngine 3.0.0 高级api
        */
        bool checkTangentBuffer(bool coordtype);

        /** 副本
        @param[in] rid 新资源ID
        @param[in] gid 新群主ID
        @version NIIEngine 3.0.0 高级
        */
        Mesh * clone(ResourceID rid, GroupID gid = GroupUnknow);

        /** 计算切线空间向量
        @version NIIEngine 3.0.0
        */
        static Vector3f calcTangent(const Vector3f & p1, const Vector3f & p2,
            const Vector3f & p3, NIIf u1, NIIf v1, NIIf u2, NIIf v2, NIIf u3, NIIf v3);

        /// @copydetails AnimationObj::createAnimation
        virtual Animation * createAnimation(AnimationID id, TimeDurMS dur);

        /// @copydetails AnimationObj::_init
        virtual void _init(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::_append
        virtual void _append(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::_update
        virtual void _update(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::isVertexFusion
        virtual bool isVertexFusion() const;

        /// @copydetails AnimationObj::isSkeletonFusion
        virtual bool isSkeletonFusion() const;
    protected:
        /** 构建边缘列表
        @version NIIEngine 3.0.0 内部api
        */
        void buildEdgeList(Nidx lod = 0);

        /// @copydetails Resource::planImpl
        void planImpl();

        /// @copydetails Resource::unplanImpl
        void unplanImpl();

        /// @see Resource::loadImpl
        void loadImpl();

        /// @see Resource::loadEndImpl
        void loadEndImpl();

        /// @see Resource::unloadImpl
        void unloadImpl();

        /// @see Resource::calcSize
        NCount calcSize() const;
    protected:
        DataStream * mSrcStream;                    ///< 网格数据流
        Skeleton * mSkeleton;                       ///< 骨骼
        MeshLodScheme * mLodScheme;                 ///< LOD策略
        VertexData * mVertex;                       ///< 顶点数据
        SkeletonID mSkeletonID;                     ///< 骨骼资源ID
        GeometryPrefab mPrefabType;                 ///< 预制类型
        Nmark mVertexMode;                          ///< 顶点缓存模式
        Nmark mIndexMode;                           ///< 索引缓存模式
        BoneIndexList mMatrixs;                     ///< 骨骼矩阵
        SubMeshList mSubMeshList;                   ///< 子网格
        GeoEdgetList mEdgetList;                    ///< 边缘列表
        VertexOffsetList mVertexOffsetList;         ///< 顶点偏移
        SkeletonVertexList mSkeletonVertexList;     ///< 骨骼顶点分配
        Nui16 mCoordIndex;                          ///< 纹理位置索引
        Nui16 mTangentIndex;                        ///< 切线位置索引
        NIIf mCenterRange;                          ///< 范围半径
        NIIf mRangeFactor;                          ///< 范围缩放因子
        AABox mAABB;                                ///< 轴对称盒
        Nmark mMark;
    };
    
    /** 网格专用补丁
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PatchMesh : public Mesh
    {
    public:
        PatchMesh(ResourceID rid, GroupID gid, VertexData * vd,
            Nmark vbmode = Buffer::M_DEV | Buffer::M_WRITE, Nmark ibmode = Buffer::M_WRITE, 
            LangID lid = N_PrimaryLang);
            
        ~PatchMesh();

        /** 定义
        @version NIIEngine 3.0.0
        */
        void define(NCount w, NCount h, NCount umaxLevel = -1, NCount vmaxLevel = -1, FaceSideType type = FST_Front);

        /** 更新
        @version NIIEngine 3.0.0
        */
        void update(NCount w, NCount h, NCount umaxLevel, NCount vmaxLevel, FaceSideType type);

        /** 设置细分因子
        @param[in] factor
        */
        void setSubdivision(NIIf factor);
    protected:
        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::planImpl
        void planImpl();
    protected:
        CurvedSurface mSurface;
        VertexData * mData;
    };
}
#endif