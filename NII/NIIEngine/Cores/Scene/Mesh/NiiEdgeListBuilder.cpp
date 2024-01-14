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

#include "NiiPreProcess.h"
#include "NiiEdgeListBuilder.h"
#include "NiiGpuBuffer.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiException.h"

namespace NII
{
    /// 通过顶点集比较排序几何
    struct geometryLess
    {
        bool operator()(const EdgeListBuilder::Geometry & a, const EdgeListBuilder::Geometry & b) const
        {
            if(a.vertexSet < b.vertexSet)
                return true;
            if(a.vertexSet > b.vertexSet)
                return false;
            return a.indexSet < b.indexSet;
        }  
    };
        
    bool EdgeListBuilder::vectorLess::operator()(const Vector3f & a, const Vector3f & b) const
    {
        if(a.x < b.x)
            return true;
        if(a.x > b.x)
            return false;
        if(a.y < b.y)
            return true;
        if(a.y > b.y)
            return false;
        return a.z < b.z;
    }
    //---------------------------------------------------------------------
    EdgeListBuilder::EdgeListBuilder() : mEdgeData(0)
    {
    }
    //---------------------------------------------------------------------
    EdgeListBuilder::~EdgeListBuilder()
    {
    }
    //---------------------------------------------------------------------
    void EdgeListBuilder::addVertex(const VertexData * data)
    {
        if(data->mOffset != 0)
        {
            N_EXCEPT(InvalidParam, _I18n("The base vertex index of the vertex data must be zero for build edge list."));
        }
        mVertexDataList.push_back(data);
    }
    //---------------------------------------------------------------------
    void EdgeListBuilder::addIndex(const IndexData * index, NCount vdidx, GeometryRaw::OperationType opType)
    {
        if(opType != GeometryRaw::OT_TRIANGLE_LIST &&
            opType != GeometryRaw::OT_TRIANGLE_FAN &&
                opType != GeometryRaw::OT_TRIANGLE_STRIP)
        {
            N_EXCEPT(InvalidParam, _I18n("Only triangle list, fan and strip are supported to build edge list."));
        }

        Geometry geometry;
        geometry.mIndex = index;
        geometry.vertexSet = vdidx;
        geometry.opType = opType;
        geometry.indexSet = mGeometryList.size();
        mGeometryList.push_back(geometry);
    }
    //---------------------------------------------------------------------
    GeoEdget * EdgeListBuilder::build()
    {
        /* Ok, here's the algorithm:
        For each set of indices in turn
          For each set of 3 indexes
            Create a new Triangle entry in the list
            For each vertex referenced by the tri indexes
              Get the position of the vertex as a Vector3 from the correct vertex buffer
              Attempt to locate this position in the existing common vertex set
              If not found
                Create a new common vertex entry in the list
              End If
              Populate the original vertex index and common vertex index
            Next vertex
            Connect to existing edge(v1, v0) or create a new edge(v0, v1)
            Connect to existing edge(v2, v1) or create a new edge(v1, v2)
            Connect to existing edge(v0, v2) or create a new edge(v2, v0)
          Next set of 3 indexes
        Next index set

        Note that all edges 'belong' to the index set which originally caused them
        to be created, which also means that the 2 vertices on the edge are both referencing the
        vertex buffer which this index set uses.
        */


        /*
        There is a major consideration: 'What is a common vertex'? This is a
        crucial decision, since to form a completely close hull, you need to treat
        vertices which are not physically the same as equivalent. This is because
        there will be 'seams' in the model, where discrepancies in vertex components
        other than position (such as normals or texture coordinates) will mean
        that there are 2 vertices in the same place, and we MUST 'weld' them
        into a single common vertex in order to have a closed hull. Just looking
        at the unique vertex indices is not enough, since these seams would render
        the hull invalid.

        So, we look for positions which are the same across vertices, and treat
        those as as single vertex for our edge calculation. However, this has
        it's own problems. There are OTHER vertices which may have a common
        position that should not be welded. Imagine 2 cubes touching along one
        single edge. The common vertices on that edge, if welded, will cause
        an ambiguous hull, since the edge will have 4 triangles attached to it,
        whilst a manifold mesh should only have 2 triangles attached to each edge.
        This is a problem.

        We deal with this with allow welded multiple pairs of edges. Using this
        techniques, we can build a individual hull even if the model which has a
        potentially ambiguous hull. This is feasible, because in the case of
        multiple hulls existing, each hull can cast same shadow in any situation.
        Notice: For stencil shadow, we intent to build a valid shadow volume for
        the mesh, not the valid hull for the mesh.
        */

        // Sort the geometries in the order of vertex set, so we can grouping
        // triangles by vertex set easy.
        std::sort(mGeometryList.begin(), mGeometryList.end(), geometryLess());
        // Initialize edge data
        mEdgeData = N_new GeoEdget();
        // resize the edge group list to equal the number of vertex sets
        mEdgeData->mGeoList.resize(mVertexDataList.size());
        // Initialise edge group data
        for (NCount vSet = 0; vSet < mVertexDataList.size(); ++vSet)
        {
            mEdgeData->mGeoList[vSet].mVertexDataIndex = vSet;
            mEdgeData->mGeoList[vSet].mVertex = const_cast<VertexData *>(mVertexDataList[vSet]);
            mEdgeData->mGeoList[vSet].mOffset = 0;
            mEdgeData->mGeoList[vSet].mCount = 0;
        }

        // 创建三角行和边缘列表
        GeometryList::const_iterator i, iend = mGeometryList.end();
        for(i = mGeometryList.begin(); i != iend; ++i)
        {
            buildTrianglesEdges(*i);
        }

        // 为照射面计算分配空间
        mEdgeData->mViewPosList.resize(mEdgeData->mTriangleList.size());

        // 记录关闭,即网格是多样的
        mEdgeData->mCloseUp = mEdgeMap.empty();

        return mEdgeData;
    }
    //---------------------------------------------------------------------
    void EdgeListBuilder::buildTrianglesEdges(const Geometry & geometry)
    {
        NCount indexSet = geometry.indexSet;
        NCount vertexSet = geometry.vertexSet;
        const IndexData * indexData = geometry.mIndex;
        GeometryRaw::OperationType opType = geometry.opType;

        NCount iterations;

        switch(opType)
        {
        case GeometryRaw::OT_TRIANGLE_LIST:
            iterations = indexData->mCount / 3;
            break;
        case GeometryRaw::OT_TRIANGLE_FAN:
        case GeometryRaw::OT_TRIANGLE_STRIP:
            iterations = indexData->mCount - 2;
            break;
        default:
            return; // Just in case
        };

        // 我们现在处理的边缘组
        GeoEdget::EdgeGroup & eg = mEdgeData->mGeoList[vertexSet];

        // 定位 位置元素和缓存
        const VertexData * vertexData = mVertexDataList[vertexSet];
        const VertexUnit * posElem = vertexData->get(VT_Pos);
        VertexBuffer * vbuf = vertexData->getAttach(posElem->mSource);
        // 锁定缓存用于处理它
        Nui8 * pBaseVertex = (Nui8 *)vbuf->lock(Buffer::MM_READ);

        // 为读取准备索引
        bool idx32bit = (indexData->mBuffer->getUnitSize() == 32);
        NCount indexSize = idx32bit ? sizeof(Nui32) : sizeof(Nui16);
#if defined(_MSC_VER) && _MSC_VER <= 1300
        // NB: Can't use un-named union with VS.NET 2002 when /RTC1 compile flag enabled.
        Nui8 * pIndex = (Nui8 *)indexData->mBuffer->lock(Buffer::MM_READ);
        pIndex = pIndex + indexData->mOffset * indexSize;
        Nui16 * p16Idx = (Nui16 *)pIndex;
        Nui32 * p32Idx = (Nui32 *)pIndex;
#else
        union
        {
            Nui8 * pIndex;
            Nui16 * p16Idx;
            Nui32 * p32Idx;
        };
        pIndex = (Nui8 *)indexData->mBuffer->lock(Buffer::MM_READ);
        pIndex = pIndex + indexData->mOffset * indexSize;
#endif

        // 遍历所有组的3个索引,过渡量
        Nui32 index[3];
        // 获取三角形的开始,如果我们有少于1个的索引集,这个为0
        NCount triangleIndex = mEdgeData->mTriangleList.size();
        // 如果第一次处理边缘组,为它建立mOffset 。注意我们假设几何通过顶点集排序
        if (!eg.mCount)
        {
            eg.mOffset = triangleIndex;
        }
        // Pre-reserve memory for less thrashing
        mEdgeData->mTriangleList.reserve(triangleIndex + iterations);
        mEdgeData->mNormalsList.reserve(triangleIndex + iterations);
        for (NCount t = 0; t < iterations; ++t)
        {
            GeoEdget::Triangle tri;
            tri.mIndexDataIndex = indexSet;
            tri.mVertexDataIndex = vertexSet;

            if(opType == GeometryRaw::OT_TRIANGLE_LIST || t == 0)
            {
                // Standard 3-index read for tri list or first tri in strip / fan
                if(idx32bit)
                {
                    index[0] = p32Idx[0];
                    index[1] = p32Idx[1];
                    index[2] = p32Idx[2];
                    p32Idx += 3;
                }
                else
                {
                    index[0] = p16Idx[0];
                    index[1] = p16Idx[1];
                    index[2] = p16Idx[2];
                    p16Idx += 3;
                }
            }
            else
            {
                // Strips are formed from last 2 indexes plus the current one for
                // triangles after the first.
                // For fans, all the triangles share the first vertex, plus last
                // one index and the current one for triangles after the first.
                // We also make sure that all the triangles are process in the
                // _anti_ clockwise orientation
                index[(opType == GeometryRaw::OT_TRIANGLE_STRIP) && (t & 1) ? 0 : 1] = index[2];
                // Read for the last tri index
                if (idx32bit)
                    index[2] = *p32Idx++;
                else
                    index[2] = *p16Idx++;
            }

            Vector3f v[3];
            for(NCount i = 0; i < 3; ++i)
            {
                // 填充三角形原顶点索引
                tri.mVertIndex[i] = index[i];

                // 检索顶点的位置
                Nui8 * pVertex = pBaseVertex + (index[i] * vbuf->getUnitSize());
                NIIf * pFloat;
                posElem->getData(pVertex, pFloat);
                v[i].x = *pFloat++;
                v[i].y = *pFloat++;
                v[i].z = *pFloat++;
                // 找到这个顶点在存在的顶点映射或创建它
                tri.mLocalVertIndex[i] = CreateVertex(v[i], vertexSet, indexSet, index[i]);
            }

            // 忽略退化三角形
            if(tri.mLocalVertIndex[0] != tri.mLocalVertIndex[1] &&
                tri.mLocalVertIndex[1] != tri.mLocalVertIndex[2] &&
                    tri.mLocalVertIndex[2] != tri.mLocalVertIndex[0])
            {
                Vector3f normal = (v[1] - v[0]).crossProduct(v[2] - v[0]);

                // 计算三角形法线(注意需要重新计算骨骼动画网格)
                mEdgeData->mNormalsList.push_back(
                    Vector4f(normal.x, normal.y, normal.z, -(normal.dotProduct(v[0]))));
                // 添加三角形到列表中
                mEdgeData->mTriangleList.push_back(tri);
                // 从公共列表中连接或创建边缘
                CreateEdge(vertexSet, triangleIndex, tri.mVertIndex[0], tri.mVertIndex[1],
                    tri.mLocalVertIndex[0], tri.mLocalVertIndex[1]);
                CreateEdge(vertexSet, triangleIndex, tri.mVertIndex[1], tri.mVertIndex[2],
                    tri.mLocalVertIndex[1], tri.mLocalVertIndex[2]);
                CreateEdge(vertexSet, triangleIndex, tri.mVertIndex[2], tri.mVertIndex[0],
                    tri.mLocalVertIndex[2], tri.mLocalVertIndex[0]);
                ++triangleIndex;
            }
        }

        // Update mCount for the edge group. Note that we are assume
        // geometries sorted by vertex set.
        eg.mCount = triangleIndex - eg.mOffset;

        indexData->mBuffer->unlock();
        vbuf->unlock();
    }
    //---------------------------------------------------------------------
    void EdgeListBuilder::CreateEdge(NCount vertexSet, NCount triangleIndex,
        NCount vertIndex0, NCount vertIndex1, NCount sharedVertIndex0,
            NCount sharedVertIndex1)
    {
        // Find the existing edge (should be reversed order) on shared vertices
        EdgeMap::iterator emi = mEdgeMap.find(std::pair<NCount, NCount>(sharedVertIndex1, sharedVertIndex0));
        if(emi != mEdgeMap.end())
        {
            // 边缘存在,连接它
            GeoEdget::Edge & e = mEdgeData->mGeoList[emi->second.first].mEdgeList[emi->second.second];
            // 更新第二部分
            e.mTriIndex[1] = triangleIndex;
            e.mAlone = false;

            mEdgeMap.erase(emi);
        }
        else
        {
            // 没有找到,创建新的边缘
            mEdgeMap.insert(EdgeMap::value_type(
                std::pair<NCount, NCount>(sharedVertIndex0, sharedVertIndex1),
                    std::pair<NCount, NCount>(vertexSet, mEdgeData->mGeoList[vertexSet].mEdgeList.size())));
            GeoEdget::Edge e;
            e.mTriIndex[0] = triangleIndex;
            e.mTriIndex[1] = static_cast<NCount>(~0);
            e.mLocalVertIndex[0] = sharedVertIndex0;
            e.mLocalVertIndex[1] = sharedVertIndex1;
            e.mVertIndex[0] = vertIndex0;
            e.mVertIndex[1] = vertIndex1;
            e.mAlone = true;
            mEdgeData->mGeoList[vertexSet].mEdgeList.push_back(e);
        }
    }
    //---------------------------------------------------------------------
    NCount EdgeListBuilder::CreateVertex(const Vector3f & vec,
        NCount vertexSet, NCount indexSet, NCount originalIndex)
    {
        // Because the algorithm doesn't care about manifold or not, we just identifying
        // the common vertex by EXACT same position.
        // Hint: We can use quantize method for welding almost same position vertex fastest.
        std::pair<CommonVertexMap::iterator, bool> inserted =
            mCommonVertexMap.insert(CommonVertexMap::value_type(vec, mVertices.size()));
        if(!inserted.second)
        {
            return inserted.first->second;
        }

        CommonVertex newCommon;
        newCommon.index = mVertices.size();
        newCommon.position = vec;
        newCommon.vertexSet = vertexSet;
        newCommon.indexSet = indexSet;
        newCommon.originalIndex = originalIndex;
        mVertices.push_back(newCommon);
        return newCommon.index;
    }
    //---------------------------------------------------------------------
    void EdgeListBuilder::log(Log * l)
    {
        l->log(_T("EdgeListBuilder Log"));
        l->log(_T("-------------------"));
        l->log(_T("Number of vertex sets: ") + N_conv(mVertexDataList.size()));
        l->log(_T("Number of index sets: ") + N_conv(mGeometryList.size()));

        NCount i, j;
        // Log original vertex data
        for(i = 0; i < mVertexDataList.size(); ++i)
        {
            const VertexData * vData = mVertexDataList[i];
            l->log(_T("."));
            l->log(_T("Original vertex set ") + N_conv(i) + _T(" - vertex count ") +
                N_conv(vData->mCount));
            const VertexUnit * posElem = vData->get(VT_Pos);
            VertexBuffer * vbuf = vData->getAttach(posElem->mSource);
            // lock the buffer for reading
            Nui8 * pBaseVertex = (Nui8 *)vbuf->lock(Buffer::MM_READ);
            NIIf * pFloat;
            for (j = 0; j < vData->mCount; ++j)
            {
                posElem->getData(pBaseVertex, pFloat);
                l->log(_T("Vertex ") + N_conv(j) + _T(": (") + N_conv(pFloat[0]) +
                    _T(", ") + N_conv(pFloat[1]) + _T(", ") + N_conv(pFloat[2]) + _T(")"));
                pBaseVertex += vbuf->getUnitSize();
            }
            vbuf->unlock();
        }

        // Log original index data
        for(i = 0; i < mGeometryList.size(); i++)
        {
            const IndexData * iData = mGeometryList[i].mIndex;
            l->log(_T("."));
            l->log(_T("Original triangle set ") + N_conv(mGeometryList[i].indexSet) +
                _T(" - index count ") + N_conv(iData->mCount) + _T(" - ") +
                    _T("vertex set ") + N_conv(mGeometryList[i].vertexSet) + _T(" - ") +
                        _T("OpType ") + N_conv(mGeometryList[i].opType));
            // Get the indexes ready for reading
            Nui16 * p16Idx = 0;
            Nui32 * p32Idx = 0;

            if (iData->mBuffer->getUnitSize() == 32)
            {
                p32Idx = (Nui32 *)iData->mBuffer->lock(Buffer::MM_READ);
            }
            else
            {
                p16Idx = (Nui16 *)iData->mBuffer->lock(Buffer::MM_READ);
            }

            for (j = 0; j < iData->mCount;)
            {
                if (iData->mBuffer->getUnitSize() == 32)
                {
                    if (mGeometryList[i].opType == GeometryRaw::OT_TRIANGLE_LIST
                        || j == 0)
                    {
                        Nui32 n1 = *p32Idx++;
                        Nui32 n2 = *p32Idx++;
                        Nui32 n3 = *p32Idx++;
                        l->log(_T("Triangle ") + N_conv(j) + _T(": (") + N_conv(n1) +
                            _T(", ") + N_conv(n2) + _T(", ") + N_conv(n3) + _T(")"));
                        j += 3;
                    }
                    else
                    {
                        l->log(_T("Triangle ") + N_conv(j) + _T(": (") + N_conv(*p32Idx++) + _T(")"));
                        j++;
                    }
                }
                else
                {
                    if(mGeometryList[i].opType == GeometryRaw::OT_TRIANGLE_LIST || j == 0)
                    {
                        Nui16 n1 = *p16Idx++;
                        Nui16 n2 = *p16Idx++;
                        Nui16 n3 = *p16Idx++;
                        l->log(_T("Index ") + N_conv(j) + _T(": (") + N_conv(n1) +
                            _T(", ") + N_conv(n2) + _T(", ") + N_conv(n3) + _T(")"));
                        j += 3;
                    }
                    else
                    {
                        l->log(_T("Triangle ") + N_conv(j) + _T(": (") + N_conv(*p16Idx++) + _T(")"));
                        j++;
                    }
                }
            }
            iData->mBuffer->unlock();
            // Log common vertex list
            l->log(_T("."));
            l->log(_T("Common vertex list - vertex count ") + N_conv(mVertices.size()));
            for(i = 0; i < mVertices.size(); ++i)
            {
                const CommonVertex & c = mVertices[i];
                l->log(_T("Common vertex ") + N_conv(i) +
                    _T(": (vertexSet=") + N_conv(c.vertexSet) +
                        _T(", originalIndex=") + N_conv(c.originalIndex) +
                            _T(", position=") + N_conv(c.position));
            }
        }
    }
    //---------------------------------------------------------------------
}