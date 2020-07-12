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

#ifndef _NII_SUBMESH_H_
#define _NII_SUBMESH_H_

#include "NiiPreInclude.h"
#include "NiiGeometryRaw.h"
#include "NiiBone.h"
#include "NiiIndexData.h"

namespace NII
{
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
        inline void setName(const String & n)           { mName = n;}

        /** 获取名字(辅助)
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const           { return mName; }

        /** 获取子网格所属的网格
        @version NIIEngine 3.0.0
        */
        inline Mesh * getMesh() const                   { return mMesh; }

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
        inline bool isSelfVertex() const                                {return mSelfVertex;}

        /** 设置几何类型
        @version NIIEngine 3.0.0
        */
        inline void setGeometryType(GeometryRaw::OperationType type)    {  mType = type; }

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
        inline IndexData * getIndexData(Nidx lod = 0) const { return mIndexList[lod]; }

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
        inline void removeAllEdgePoint()                   { mEdgePoints.clear(); }

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
}
#endif