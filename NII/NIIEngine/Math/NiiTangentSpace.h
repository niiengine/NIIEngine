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

#ifndef _NII_TangentSpace_H_
#define _NII_TangentSpace_H_

#include "NiiPreInclude.h"
#include "NiiVector2.h"
#include "NiiVector3.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiGeometryRaw.h"

namespace NII
{
    /// 相切空间计算
    class _EngineAPI TangentSpace
    {
    public:
        typedef std::pair<NCount, NCount> VertexSplit;
        struct IndexRemap
        {
            NCount indexSet;        ///< 索引数据集(如果索引数据添加了,大于0)
            NCount faceIndex;       ///< 在索引缓存中受影响的位置
            VertexSplit splitVertex;///< 旧;新顶点索引

            IndexRemap(){}

            IndexRemap(NCount i, NCount f, const VertexSplit & s) :
                indexSet(i), faceIndex(f), splitVertex(s){}
        };
        typedef list<IndexRemap>::type IndexRemapList;
        typedef list<VertexSplit>::type VertexSplits;

        /// 切线空间结果
        struct Result
        {
            VertexSplits vertexSplits;
            IndexRemapList indexesRemapped;
        };
    public:
        TangentSpace();

        virtual ~TangentSpace();

        /** 重置计算对象
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 设置输入顶点数据(将被修改)
        @version NIIEngine 3.0.0
        */
        void setVertexData(VertexData * in);

        /** 添加顶点索引
        @version NIIEngine 3.0.0
        */
        void addIndexData(IndexData * in,
            GeometryRaw::OperationType type = GeometryRaw::OT_TRIANGLE_LIST);

        /** 设置是否存储4成分的切线空间
        @remark
            默认的格式使用 DT_Float3x,足够准确处理切线,不设置任何纹理坐标镜像.如果
            期望在模型中使用UV镜像,使用这个方法必须启用4成分切线,`w`坐标填充三角形
            奇偶校验(+1 或 -1).
        @param[in] enable
            true,启用4成分切线(默认false).如果启用这个,可能需要启用镜像分割(setSplitMirrored)
                    @version NIIEngine 3.3.0
        */
        void setStoreParityInW(bool set) { mStoreParityInW = set; }

        /**  获取是否存储4成分切线空间
        @version NIIEngine 3.3.0
        */
        bool getStoreParityInW() const { return mStoreParityInW; }

        /** 设置当镜像切线空间发生时是否划分顶点
        @remark
            这个默认为false,这是最安全的选项,当使用不可预料模型的资源,不可避免在
            表面多余的接缝.如果模型是已经处理好的话,可以隐藏模型折叠的纹理接缝,可
            以打开此选项,可以防止平滑到其他区域产生的接缝
        @note 如果含概带条和扇形物的基本几何,这个选项自动禁用
        */
        void setSplitMirrored(bool set) { mSplitMirrored = set; }

        /** 获取当镜像切线空间发生时是否划分顶点
        @version NIIEngine 3.0.0
        */
        bool getSplitMirrored() const { return mSplitMirrored; }

        /** 设置顶点在切线空间绕转超过90度时是否被划分
        @remark
            这个默认为false,这是最安全的选项,当使用不可预料模型的资源,不可避免在
            表面多余的接缝.如果模型是已经处理好的话,可以隐藏模型折叠的纹理接缝,可
            以打开此选项,可以防止平滑到其他区域产生的接缝
        @note 如果含概带条和扇形物的基本几何,这个选项自动禁用
        @version NIIEngine 3.0.0
        */
        void setSplitRotated(bool set) { mSplitRotated = set; }

        /** 获取顶点在切线空间绕转超过90度时是否被划分
        @version NIIEngine 3.0.0
        */
        bool getSplitRotated() const { return mSplitRotated; }

        /** 建立切线空间
        @remark 仅充许索引三角形列表.不支持带条和扇形物
        @param[in] type 存储在切线的语意,明确的切线绑定,仅硬件(Shader Model 2)需要强调
        @param[in] texcoordSet 纹理坐标索引,作为纹理坐标资源使用
        @param[in] index 如果type是VT_Tex_Coord时使用, 用于存储3d坐标,描述的是每顶点的切线向量.
        @return
            一个结构包含切线空间建立的结果.数据顶点通常被修改,也可能对索引进行修改.因为镜像使用
            到网格时候会发生,导致切线空间翻转到边缘的两侧,这是不连续的,因此顶点要沿着这条边分裂,
            产生新的顶点。
        */
        Result build(VertexType type = VT_Tangent, Nui16 index = 1, Nui16 texcoordSet = 0);
    protected:
        void normaliseVertices();
        void remapIndexes(Result & res);
        void extendBuffers(VertexSplits & splits);
        void processFaces(Result & result);
        void populateVertexArray(Nui16 sourceTexCoordSet);

        void insertTangents(Result & res, VertexType targetSemantic,
            Nui16 sourceTexCoordSet, Nui16 index);

        void calculateFaceTangentSpace(const NCount * vertInd, Vector3f & tsU,
            Vector3f & tsV, Vector3f & tsN);

        NIIf calculateAngleWeight(NCount v0, NCount v1, NCount v2);
        NIIi calculateParity(const Vector3f & u, const Vector3f & v, const Vector3f & n);

        void addFaceTangentSpaceToVertices(NCount indexSet, NCount faceIndex,
            NCount * localVertInd, const Vector3f & faceTsU, const Vector3f & faceTsV,
                const Vector3f & faceNorm, Result & result);
    protected:
        VertexData * mVData;
        typedef vector<GeometryRaw::OperationType>::type OpTypeList;
        IndexDataList mIDataList;
        OpTypeList mOpTypes;
        bool mSplitMirrored;
        bool mSplitRotated;
        bool mStoreParityInW;

        struct VertexInfo
        {
            Vector3f pos;
            Vector3f norm;
            Vector2f uv;
            Vector3f tangent;
            Vector3f binormal;
            NIIi parity;
            NCount oppositeParityIndex;

            VertexInfo() : tangent(Vector3f::ZERO), binormal(Vector3f::ZERO), parity(0),
                oppositeParityIndex(0) {}
        };
        typedef vector<VertexInfo>::type VertexInfoArray;
        VertexInfoArray mVertexArray;
    };
}
#endif