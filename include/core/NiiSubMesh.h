/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-10-25

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
        void setName(const String & n);

        /** 获取名字(辅助)
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** 获取子网格所属的网格
        @version NIIEngine 3.0.0
        */
        Mesh * getMesh() const;

        /** 渲染几何
        @param[in] gr 几何数据
        @param[in] lod 所使用的LOD索引.
        @version NIIEngine 3.0.0
        */
        void getGeometry(GeometryRaw & gr, Nidx lod = 0) const;

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
        const BoneIndexList & getMatrixList() const;

        /** 获取骨骼顶点列表
        @version NIIEngine 3.0.0
        */
        const SkeletonVertexList & getSkeletonList() const;

        /** 设置是否使用独立的顶点数据
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setSelfVertex(bool b);

        /** 获取使用独立的顶点数据
        @version NIIEngine 3.0.0
        */
        bool isSelfVertex() const;

        /** 设置几何类型
        @version NIIEngine 3.0.0
        */
        void setGeometryType(GeometryRaw::OperationType type);

        /** 获取几何类型
        @version NIIEngine 3.0.0
        */
        GeometryRaw::OperationType getGeometryType() const;

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
        VertexData * getVertexData() const;

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
        IndexData * getIndexData(Nidx lod = 0) const;

        /** 设置是否自动生成LOD
        @version NIIEngine 3.0.0
        */
        void setAutoLod(bool b);

        /** 获取是否自动生成LOD
        @version NIIEngine 3.0.0
        */
        bool isAutoLod() const;

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 获取材质
        @version NIIEngine 3.0.0
        */
        ResourceID getMaterial() const;

        /** 添加LOD顶点索引数据
        @param[in] id
        @version NIIEngine 3.0.0
        */
        void addIndexData(IndexData * id);

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
        void buildEdgePoints(NCount count);

        /** 添加边缘点
        @version NIIEngine 3.0.0
        */
        void addEdgePoint(const Vector3f & v);

        /** 移去所有边缘点
        @version NIIEngine 3.0.0
        */
        void removeAllEdgePoints();

        /** 获取边缘点
        @version NIIEngine 3.0.0
        */
        const Vector3List & getEdgePoints() const;
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