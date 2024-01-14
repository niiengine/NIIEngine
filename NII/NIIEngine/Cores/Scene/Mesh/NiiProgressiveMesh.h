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

// The underlying algorithms in this class are based heavily on:
/*
 *  Progressive Mesh type Polygon Reduction Algorithm
 *  by Stan Melax (c) 1998
 */

#ifndef _NII_ProgressiveMesh_H_
#define _NII_ProgressiveMesh_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiVector2.h"
#include "NiiGpuBuffer.h"
#include "NiiGeometryRaw.h"
#include "NiiSmallVector.h"

namespace NII
{
    class _EngineAPI BitArray
    {
    public:
        BitArray(): bits_ptr(NULL) {}
        BitArray(NIIi bits_count): bits_ptr(NULL) { resize(bits_count); }
        BitArray & operator=(const BitArray & ba)
        { bits = ba.bits; bits_ptr = bits.size() > 0 ? &bits.front() : NULL; return *this; }

        inline bool getBit(NCount i) const { return bits_ptr[i >> 3] & bit_mask[i & 7]; }
        inline void setBit(NCount i)       { bits_ptr[i >> 3] |= bit_mask[i & 7]; }
        inline void clearBit(NCount i)     { bits_ptr[i >> 3] &= ~bit_mask[i & 7]; }
        inline void clearAllBits()         { memset(bits_ptr, 0, bits.size()); }

        inline bool empty() const          { return bits.empty(); }
        void resize(NCount bits_count)
        {
            bits.resize((bits_count + 7) / 8);
            bits_ptr = bits.size() > 0 ? &bits.front() : NULL;
            clearAllBits();
        }

        NCount getBitsCount() const
        {
            NCount count = 0;
            for(Nui8 *ptr = bits_ptr, *end_ptr = bits_ptr + bits.size(); ptr != end_ptr; ++ptr)
            {
                const Nui8 b = *ptr;
                count += bit_count[b & 0xF] + bit_count[b >> 4];
            }
            return count;
        }
    private:
        Nui8 *             bits_ptr;// it`s so performance critical, so we place raw data pointer before all other members
        vector<Nui8>::type bits;

        const static Nui8  bit_mask[8];// = { 1, 2, 4, 8, 16, 32, 64, 128 };
        const static Nui8  bit_count[16];// = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
    };

    /** 这个类降低给定几何的复杂性,这个类专用于减少给定网格三角
        形数量.考虑到缝合复合框架,几何和纹理坐标还有网格
    @par
        The primary use for this is generating LOD versions of Mesh objects, but it can be
        used by any geometry provider. The only limitation at the moment is that the
        provider uses a common vertex buffer for all LODs and one index buffer per LOD.
        Therefore at the moment this class can only handle indexed geometry.
    @par
        注意,当支持编译顶点缓存时,这个类的接口肯定会改变
    */
    class _EngineAPI ProgressiveMesh : public ProgMeshAlloc
    {
    protected:
        class PMTriangle;
        class PMVertex;
        struct vertexLess;
    public:
        typedef vector<NIIf>::type LodValueList;
        /** 得出每个LOD减少顶点的指标
        @version NIIEngine 3.0.0
        */
        enum VertexReductionQuota
        {
            VRQ_CONSTANT,       ///< 每次减少一个一定数量的顶点集
            VRQ_PROPORTIONAL,   ///< 每次减少后,留下一定数量的比例
            /// All vertices with reduction error cost less than reductionValue * sqr(lodDistance[lodLevel] / lodDistance[0])
            /// are removed at each reduction. Error cost is calculated as introduced error area divided by squared mesh diagonal
            VRQ_ERROR_COST
        };

        /// 一顶点作为一个面使用,记录顶点实际索引,一个共同顶点指针用于面赋值
        class _EngineInner PMFaceVertex
        {
        public:
            NCount realIndex;
            PMVertex * commonVertex;
        };
    public:
        /** Automatically generates lower level of detail versions of this mesh for use
            when a simpler version of the model is acceptable for rendering.
        @remarks
            There are 2 ways that you can create level-of-detail (LOD) versions of a mesh;
            the first is to call this method, which does fairly extensive calculations to
            work out how to simplify the mesh whilst having the minimum affect on the model.
            The alternative is to actually create simpler versions of the mesh yourself in
            a modelling tool, and having exported them, tell the 'master' mesh to use these
            alternative meshes for lower detail versions;
        @par
            As well as creating the lower detail versions of the mesh, this method will
            also associate them with depth values. As soon as an object is at least as far
            away from the camera as the depth value associated with it's LOD, it will drop
            to that level of detail.
        @par
            I recommend calling this method before mesh export, not at runtime.
        @param[in] lodValues A list of lod values indicating the values at which new lods should be
            generated. These are 'user values', before being potentially
            transformed by the strategy, so for the distance strategy this is an
            unsquared distance for example.
        @param[in] reductionMethod The way to determine the number of vertices collapsed per LOD
        @param[in] reductionValue Meaning depends on reductionMethod, typically either the proportion
            of remaining vertices to collapse or a fixed number of vertices.
        */
        static bool generateLodLevels(Mesh * pMesh, const LodValueList & lodValues,
            VertexReductionQuota reductionMethod, NIIf reductionValue);

        /** Automatically generates lower level of detail versions of this mesh for use
            when a simpler version of the model is acceptable for rendering.
        @remarks
            Useful for importing of external mesh with unknown size and structure into something manageable.
        @par
            Simplifies vertex structure to { pos, norm, tex0 } stored in single stream.
            Removes unused vertices, performing reindexing.
        @par
            Can optionally discard first LOD level (i.e. original geometry), unused vertices would be removed.
        */
        static Mesh * generateSimplifiedMesh(ResourceID rid, GroupID gid,
            Mesh * inMesh, bool dropOriginalGeometry, const LodValueList & lodValues,
                VertexReductionQuota reductionMethod, NIIf reductionValue,
                    NCount * removedVertexDuplicatesCount);
    protected:
        /** 构造函数,采用几何数据和索引缓存
        @remark
            DO NOT pass write-only, unshadowed buffers to this method! They will not
            work. ShaderCh only shadowed buffers, or better yet perform mesh reduction as
            an offline process using MemGBufferManager to manage vertex
            buffers in system memory.
        */
        ProgressiveMesh(SubMesh * obj);
        virtual ~ProgressiveMesh();

        typedef vector<ProgressiveMesh *>::type ProgressiveMeshList;

        /// Allocates internal resources
        static void initializeProgressiveMeshList(ProgressiveMeshList & pmList, Mesh * pMesh);

        /// Deletes allocated internal resources.
        static void freeProgressiveMeshList(ProgressiveMeshList * pmList);

        /** 添加一个额外顶点位置缓存.
        @remark
            As well as the main vertex buffer, the client of this class may add extra versions
            of the vertex buffer which will also be taken into account when the cost of
            simplifying the mesh is taken into account. This is because the cost of
            simplifying an animated mesh cannot be calculated from just the reference position,
            multiple positions needs to be assessed in order to find the best simplification option.
        @par
            DO NOT pass write-only, unshadowed buffers to this method! They will not
            work. ShaderCh only shadowed buffers, or better yet perform mesh reduction as
            an offline process using MemGBufferManager to manage vertex
            buffers in system memory.
        @param[in] buffer Pointer to x/y/z buffer with vertex positions. The number of vertices
            must be the same as in the original GeometryData passed to the constructor.
        */
        virtual void addExtraVertexPositionBuffer(const VertexData * vertex);

        /** 生成指定数量等级的任意网格
        @param[in] numLevels The number of levels to include in the output excluding the full detail version.
        @param[in] outList Pointer to a list of LOD geometry data which will be completed by the application.
            Each entry is a reduced form of the mesh, in decreasing order of detail.
        @param[in] quota The way to derive the number of vertices removed at each LOD
        @param[in] reductionValue Either the proportion of vertices to remove at each level, or a fixed
        number of vertices to remove at each level, depending on the value of quota
        */
        static bool build(ProgressiveMeshList & pmInList, const MeshLodScheme * scheme,
            const LodValueList & lodValues, VertexReductionQuota quota, NIIf reductionValue = 0.5f);
    protected:
        /// 一个在任意网格的三角形,保存额外资料,如面法线.
        class _EngineInner PMTriangle
        {
        public:
            PMTriangle();
            void setDetails(NCount index, PMFaceVertex * v0, PMFaceVertex * v1, PMFaceVertex * v2);
            void computeNormal(void);
            void replaceVertex(PMFaceVertex * vold, PMFaceVertex * vnew);
            bool hasCommonVertex(PMVertex * v) const;
            bool hasFaceVertex(PMFaceVertex * v) const;
            PMFaceVertex* getFaceVertexFromCommon(PMVertex * commonVert);
            void notifyRemoved(void);

            PMFaceVertex * vertex[3];    // the 3 points that make this tri
            Vector3f normal;        // unit vector orthogonal to this face
            NIIf area;
            bool removed;    // true if this tri is now removed
            NCount index;
        };

        /** 一个在任意网格的顶点,保存像面转点消耗等信息,这个顶点实际在最后模式中是描
            述几个顶点,因为顶点到纹理缝合处等有重复,为了正确赋值面的属性,一个单一的
            共同点用于它们的重复,还有 面 保存了重复顶点的细节
        */
        class _EngineInner PMVertex
        {
        public:
            enum BorderStatus { BS_UNKNOWN = 0, BS_NOT_BORDER, BS_BORDER };
            typedef SmallVector<PMVertex *, 8> NeighborList;
            typedef SmallVector<PMTriangle *, 8> FaceList;
        public:
            PMVertex() : mBorderStatus(BS_UNKNOWN), removed(false) {}
            void setDetails(NCount index, const Vector3f & pos, const Vector3f & normal,
                const Vector2f & uv);
            bool isNearEnough(PMVertex * other) const;
            void removeIfNonNeighbor(PMVertex * n);
            /// Set mBorderStatus to BS_BORDER if this vertex is on the edge of an open geometry patch
            void initBorderStatus(void);
            bool isManifoldEdgeWith(PMVertex * v); // is edge this->src a manifold edge?
            void notifyRemoved(void);
            void calculateNormal();
            Vector3f position;  // location of point in euclidean space
            Vector3f normal;
            Vector2f uv;
            NCount index;       // place of vertex in original list
            BorderStatus mBorderStatus;
            bool removed;   // true if this vert is now removed
            bool toBeRemoved; // debug

            NIIf collapseCost;  // cached cost of collapsing edge
            PMVertex * collapseTo; // candidate vertex for collapse

            NeighborList neighbor; // adjacent vertices
            FaceList face;     // adjacent triangles
        };

        typedef vector<PMTriangle>::type TriangleList;
        typedef vector<PMFaceVertex>::type FaceVertexList;
        typedef vector<PMVertex>::type CommonVertexList;
        typedef std::pair<NIIf, Nui32> CostIndexPair;
        typedef vector<CostIndexPair>::type WorstCostList;

        /// 用于计算面转点消耗的数据
        struct PMWorkingData
        {
            TriangleList mTriList;          ///< 面列表
            FaceVertexList mFaceVertList;   ///< The vertex details referenced by the triangles
            CommonVertexList mVertList;     ///< 公共电点的主要列表
        };

        typedef vector<PMWorkingData>::type WorkingDataList;

        /// We can defragment mesh, removing unused vertices and re-indexing other, storing old-to-new mapping in index map
        typedef std::pair<unsigned, PMVertex *> IndexVertexPair;
    protected:
        /// 内部方法,从几何数据中建立PMWorkingData
        void addWorkingData(const VertexData * vertex, const IndexData * index);

        void mergeWorkingDataBorders();

        /// 内部方法,初始化边缘面转点消耗
        void initialiseEdgeCollapseCosts(void);

        /// 内部计算方法 从u到v获取面转点消耗
        NIIf computeEdgeCollapseCost(PMVertex * src, PMVertex * dest) const;

        /// Internal calculation method, return true if edge collapse flip some neighbor face normal
        bool collapseInvertsNormals(PMVertex * src, PMVertex * dest) const;

        /// 内部方法 为单一缓存从这个顶点中估计所有面转点消耗和拾获最低的
        NIIf computeEdgeCostAtVertexForBuffer(PMVertex * v);

        /// 内部方法 为每个缓存从这个顶点中估计所有面转点消耗和返回最差的
        NIIf computeEdgeCostAtVertex(NCount vertIndex);

        /// 内部方法 为所有边缘计算边缘消耗
        void computeAllCosts(void);

        /// Internal methods for lazy costs recomputing
        static NCount getInvalidCostCount(ProgressiveMesh::ProgressiveMeshList & pmList);

        static bool recomputeInvalidCosts(ProgressiveMeshList & pmInList);

        void recomputeInvalidCosts();

        void sortIndexesByCost();

        static NIIi cmpByCost(const void * p1, const void * p2); // comparator for mWorstCosts sorting

        /// Internal methods for getting the index of next best vertex to collapse among all submeshes
        static void getNextCollapser(ProgressiveMeshList & pmList, ProgressiveMesh *& pm, CostIndexPair *& bestCollapser);

        CostIndexPair * getNextCollapser();

        /// 内部方法 建立一个基于当前状态的新LOD
        void bakeNewLOD(IndexData * pData);

        /// Internal method builds an LODs usage, possibly skipping first LOD, that can be used as original geometry
        static void bakeLodUsage(Mesh * pMesh, MeshLodScheme * scheme, const LodValueList & lodValues, bool skipFirstLodLevel = false);

        /** 内部方法, 面转点顶点到保存面转点的目标
        @remark 这个方法更新工作中的三角形列表去删除一个三角形和重新计算瓦解目标
            的边缘瓦解消耗,它还更新有关缓存的所有工作汇总的顶点列表
        */
        void collapse(PMVertex * collapser);

        /// Optionally discards first LOD level (i.e. original geometry), removes unused vertices, remaps indexes.
        static void bakeSimplifiedMesh(Mesh * destMesh, Mesh * srcMesh, ProgressiveMeshList & pmList, bool dropFirstLodLevel = false);

        /// Defragments vertices, removing unused. Useful if original geometry is redundant or dropped at all.
        static void    createSimplifiedVertexData(vector<IndexVertexPair>::type & usedVertices, VertexData * inVData, VertexData *& outVData, AABox & aabox);

        /// During vertices defragmentation vertices are re-indexed, so old-to-new mapping is stored in index map by this function.
        static void createIndexMap(vector<IndexVertexPair>::type & usedVertices, unsigned allVertexCount, vector<unsigned>::type & indexMap);

        /// 内部调式方法
        void dumpContents(const String & log);
    protected:
        /// Can be NULL for non-indexed subMeshes, such PM would be skipped
        SubMesh * mSubMesh;
        VertexData * mVertexData;
        IndexData * mIndexData;
        IndexDataList mLodFaceList;

        NCount mRemovedVertexDuplicatesCount;
        NCount mCurrNumIndexes;
        NIIf mInvSquaredBoundBoxDiagonal;
        NIIi mVertexComponentFlags;

        /// 多映射副本, 每一个顶点缓存的
        WorkingDataList mWorkingData;

        /// 为每个顶点的所有顶点缓存极端消耗
        WorstCostList mWorstCosts; // sorted by cost, but some of entries are invalidated, so check invalidCostMask
        BitArray mInvalidCostMask; // indexed by vertex index
        NCount mInvalidCostCount;
        NCount mWorstCostsSize;
        NCount mNextWorstCostHint; // getNextCollapser() uses it to reduce complexity from O(n^2) to O(n)

        /// Temporary variable used in computeEdgeCollapseCost, declared here to avoid multiple memory allocations
        mutable PMVertex::FaceList mEdgeAdjacentSides;
    };
    template <typename T> struct HardwareBufferLockGuard
    {
        HardwareBufferLockGuard(const T & p, Buffer::MuteMode options) : pBuf(p)
        {
            pData = pBuf->lock(options);
        }
        HardwareBufferLockGuard(const T& p, NCount offset, NCount length,
            Buffer::MuteMode options) : pBuf(p)
        {
            pData = pBuf->lock(offset, length, options);
        }
        ~HardwareBufferLockGuard()
        {
            pBuf->unlock();
        }
        const T & pBuf;
        void * pData;
    };
    typedef HardwareBufferLockGuard<VertexBuffer *> VertexBufferLockGuard;
    typedef HardwareBufferLockGuard<IndexBuffer *> IndexBufferLockGuard;
    /** @} */
    /** @} */
}
#endif