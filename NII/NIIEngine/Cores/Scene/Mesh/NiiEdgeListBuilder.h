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

#ifndef _NII_EDGET_LIST_BUILDER_H_
#define _NII_EDGET_LIST_BUILDER_H_

#include "NiiPreProcess.h"
#include "NiiGeoEdget.h"

namespace NII
{
    /** 网格边缘计算
    @remark
        你可以添加多哥顶点或索引数据集去建立边缘里表,将会在各种集和已经存在的集中建立
        边缘; 这将充许你使用一个由多个网格(使用独立索引和可选的顶点数据)仍然有同样的
        连接信息组成的模型.一个重点是边缘的索引将由一个单一顶点缓存所约束(在渲染边缘
        时,这是必须的)
    */
    class _EngineAPI EdgeListBuilder
    {
    public:
        /** 一个顶点实际上在最终模型里可以代表数个顶点,因为沿顶点纹理缝合等将重复.
        为了正确评估表面性质,一个共同的顶点是用于这些重复,而且面保存这些重复
        顶点的详细情况.
        @version NIIEngine 3.0.0
        */
        struct CommonVertex
        {
            NCount index;
            Vector3f position;
            NCount vertexSet;
            NCount indexSet;
            NCount originalIndex;
        };

        /// 索引几何数据集
        struct Geometry
        {
            NCount indexSet;                    ///< 这个几何数据引用的索引数据集
            NCount vertexSet;                   ///< 这个几何数据引用的顶点数据集
            const IndexData * mIndex;           ///< 索引信息,描述三角形
            GeometryRaw::OperationType opType;  ///< 用于渲染这个几何的操作类型
        };
    public:
        EdgeListBuilder();
        ~EdgeListBuilder();

        /** 添加一系列顶点几何数据到边缘产生器.
        @remark 在调用建立方法前,你必须添加不少于一个顶点数据集到构建器中
        @version NIIEngine 3.0.0
        */
        void addVertex(const VertexData * vd);

        /** 加一系列索引几何数据到边缘产生器.
        @remark 在调用建立方法前,你必须添加不少于一个索引数据集到构建器中
        @param[in] index 索引信息,描述三角形的
        @param[in] vdidx 这个索引数据引用的顶点数据,你需要改变它,如果你添加了多个顶点集
        @param[in] opType 用于渲染这些索引的操作类型. 仅三角形类型支持(非点或线之类的)
        @version NIIEngine 3.0.0
        */
        void addIndex(const IndexData * index, NCount vdidx = 0,
            GeometryRaw::OperationType type = GeometryRaw::OT_TRIANGLE_LIST);

        /** 基于目前为止建立的信息,生成的边缘信息.
        @note 返回对象需要手动删除
        @version NIIEngine 3.0.0
        */
        GeoEdget * build();

        /** 调试方法
        @versioin NIIEngine 3.0.0
        */
        void log(Log * l);
    protected:
        void buildTrianglesEdges(const Geometry & geometry);

        /** 找到一个存在的共同顶点,或着插入一个新的
        @version NIIEngine 3.0.0
        */
        NCount CreateVertex(const Vector3f & vec, NCount vdidx, NCount indexSet, NCount originalIndex);

        /** 连接一个存在的边缘或创建一个新边缘 - 在建立时实用的方法
        @version NIIEngine 3.0.0
        */
        void CreateEdge(NCount vdidx, NCount triangleIndex, NCount vertIndex0,
            NCount vertIndex1, NCount common0, NCount common1);
    protected:
        struct vectorLess
        {
            bool operator()(const Vector3f & a, const Vector3f & b) const;
        };
        typedef vector<Geometry>::type GeometryList;
        typedef vector<CommonVertex>::type CommonVertexList;
        typedef vector<const VertexData *>::type VertexDataList;
        typedef map<Vector3f, NCount, vectorLess>::type CommonVertexMap;
        typedef multimap<std::pair<NCount, NCount>, std::pair<NCount, NCount> >::type EdgeMap;

        GeometryList mGeometryList;
        VertexDataList mVertexDataList;
        CommonVertexList mVertices;
        GeoEdget * mEdgeData;
        CommonVertexMap mCommonVertexMap;
        EdgeMap mEdgeMap;
    };
}

#endif