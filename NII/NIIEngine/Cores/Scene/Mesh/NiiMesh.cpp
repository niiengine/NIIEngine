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
#include "NiiMesh.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "NiiVertexOffset.h"
#include "NiiMeshSerializer.h"
#include "NiiSkeletonManager.h"
#include "NiiGBufferManager.h"
#include "NiiGpuBuffer.h"
#include "NiiStrConv.h"
#include "NiiSkeleton.h"
#include "NiiMeshManager.h"
#include "NiiEdgeListBuilder.h"
#include "NiiSimdUtil.h"
#include "NiiTangentSpace.h"
#include "NiiVertexOffsetFusion.h"
#include "NiiVertexUnitFusion.h"
#include "NiiDefaultStream.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiException.h"
#include "NiiMaterialManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    SubMesh::SubMesh() :
        mMesh(0),
        mVertex(0),
        mMaterialID(0),
        mType(GeometryRaw::OT_TRIANGLE_LIST),
        mSkeletonVexrtexValid(true),
        mSelfVertex(false),
        mAutoLod(true)
    {
        mIndexList.push_back(N_new IndexData());
    }
    //-----------------------------------------------------------------------
    SubMesh::SubMesh(const Mesh * obj) :
        mMesh(const_cast<Mesh *>(obj)),
        mVertex(0),
        mMaterialID(0),
        mType(GeometryRaw::OT_TRIANGLE_LIST),
        mSkeletonVexrtexValid(true),
        mSelfVertex(false),
        mAutoLod(true)
    {
        mIndexList.push_back(N_new IndexData());
    }
    //-----------------------------------------------------------------------
    SubMesh::~SubMesh()
    {
        if (mVertex != 0)
        {
            N_delete mVertex;
            mVertex = 0;
        }
        removeAllLod();

        if (mIndexList[0] != 0)
        {
            N_delete mIndexList[0];
            mIndexList[0] = 0;
        }
        mIndexList.clear();

        mEdgePoints.clear();
    }
    //-----------------------------------------------------------------------
    void SubMesh::setIndexData(Nidx lod, IndexData * obj)
    {
        if (mIndexList.size() < lod + 1)
        {
            NCount i, iend = lod + 1 - mIndexList.size();
            for (i = 0; i < iend; ++i)
            {
                mIndexList.push_back(0);
            }
        }

        if (mIndexList[lod] != 0)
            N_delete mIndexList[lod];
        mIndexList[lod] = obj;
    }
    //-----------------------------------------------------------------------
    void SubMesh::setLodCount(NCount c)
    {
        if (mIndexList.size() > c)
        {
            IndexDataList::iterator j, jend = mIndexList.end();
            for (j = mIndexList.begin() + c - 1; j != jend; ++j)
            {
                if (*j != 0)
                    N_delete *j;
            }
            mIndexList.erase(mIndexList.begin() + c - 1, jend);
        }
        else if (mIndexList.size() < c)
        {
            NCount i, iend = c - mIndexList.size();
            for (i = 0; i < iend; ++i)
            {
                mIndexList.push_back(0);
            }
        }
    }
    //-----------------------------------------------------------------------
    NCount SubMesh::getLodCount() const
    {
        return mIndexList.size();
    }
    //-----------------------------------------------------------------------
    void SubMesh::getGeometry(GeometryRaw & gr, Nidx lod) const
    {
        if (mIndexList[0]->mCount != 0)
        {
            gr.add(GRT_Index);
        }
        else
        {
            gr.remove(GRT_Index);
        }

        if (lod > 0 && lod < mIndexList.size())
        {
            gr.mIndex = mIndexList[lod];
        }
        else
        {
            gr.mIndex = mIndexList[0];
        }

        gr.mType = mType;
        gr.mVertex = mSelfVertex ? mVertex : mMesh->mVertex;
    }
    //-----------------------------------------------------------------------
    void SubMesh::addSkeleton(const SkeletonVertex & sv)
    {
        if (mSelfVertex == false)
        {
            N_EXCEPT(InvalidParam, _I18n("This SubMesh uses shared geometry,  you must assign bones to the Mesh, not the SubMesh"));
        }
        mSkeletonVertexList.insert(SkeletonVertexList::value_type(sv.mVertexIndex, sv));
        mSkeletonVexrtexValid = false;
    }
    //-----------------------------------------------------------------------
    void SubMesh::removeSkeleton(Nidx vindex)
    {
        SkeletonVertexList::iterator i, temp, iend = mSkeletonVertexList.end();
        for (i = mSkeletonVertexList.begin(); i != iend;)
        {
            temp = ++i;
            if (i->second.mVertexIndex == vindex)
            {
                mSkeletonVertexList.erase(temp);
            }
        }
    }
    //-----------------------------------------------------------------------
    void SubMesh::clearSkeleton()
    {
        mSkeletonVertexList.clear();
        mSkeletonVexrtexValid = false;
    }
    //-----------------------------------------------------------------------
    void SubMesh::buildSkeleton()
    {
        if (mSkeletonVexrtexValid == false)
        {
            SkeletonManager::combine(mVertex, mSkeletonVertexList, mMatrixs);
            mSkeletonVexrtexValid = true;
        }
    }
    //---------------------------------------------------------------------
    void SubMesh::removeAllLod()
    {
        IndexDataList::iterator i, iend = mIndexList.end();
        for (i = mIndexList.begin() + 1; i != iend; ++i)
        {
            N_delete *i;
        }
        mIndexList.erase(mIndexList.begin() + 1, iend);
    }
    //---------------------------------------------------------------------
    /* 从不同的域中找到我们需要的点这些域来源于网格的不同地方
     * 实现一个简单的 Heckbert 量化算法.
     *
     * 这个结构像AxisAlignedBox,有一些专门的方法做量化
     */
    struct Cluster
    {
        Vector3f mMin, mMax;
        set<Nui32>::type mIndices;

        Cluster()
        {
        }
        //---------------------------------------------
        bool empty() const
        {
            if (mIndices.empty())
                return true;
            if (mMin == mMax)
                return true;
            return false;
        }
        //---------------------------------------------
        NIIf volume() const
        {
            return (mMax.x - mMin.x) * (mMax.y - mMin.y) * (mMax.z - mMin.z);
        }
        //---------------------------------------------
        void extend(NIIf * v)
        {
            if (v[0] < mMin.x)
                mMin.x = v[0];
            if (v[1] < mMin.y)
                mMin.y = v[1];
            if (v[2] < mMin.z)
                mMin.z = v[2];
            if (v[0] > mMax.x)
                mMax.x = v[0];
            if (v[1] > mMax.y)
                mMax.y = v[1];
            if (v[2] > mMax.z)
                mMax.z = v[2];
        }
        //---------------------------------------------
        void computeBBox(const VertexUnit * pos, Nui8 * vdata, NCount vsz)
        {
            mMin.x = mMin.y = mMin.z = Math::POS_INFINITY;
            mMax.x = mMax.y = mMax.z = Math::NEG_INFINITY;

            set<Nui32>::type::const_iterator i, iend = mIndices.end();
            for (i = mIndices.begin(); i != iend; ++i)
            {
                NIIf * v;
                pos->getData(vdata + *i * vsz, v);
                extend(v);
            }
        }
        //---------------------------------------------
        Cluster split(NIIi split_axis, const VertexUnit * pos, Nui8 * vdata, NCount vsz)
        {
            NIIf r = (mMin[split_axis] + mMax[split_axis]) * 0.5f;
            Cluster newbox;

            // 分隔所有点到新的bbox中
            set<Nui32>::type::iterator i = mIndices.begin();
            for (; i != mIndices.end();)
            {
                NIIf * v;
                pos->getData(vdata + *i * vsz, v);
                if (v[split_axis] > r)
                {
                    newbox.mIndices.insert(*i);
                    set<Nui32>::type::iterator x = i++;
                    mIndices.erase(x);
                }
                else
                {
                    ++i;
                }
            }

            computeBBox(pos, vdata, vsz);
            newbox.computeBBox(pos, vdata, vsz);

            return newbox;
        }
    };
    //---------------------------------------------------------------------
    void SubMesh::buildEdge(NCount count)
    {
        mEdgePoints.clear();

        /* Currently this uses just one criteria: the points must be
         * as far as possible from each other. This at least ensures
         * that the extreme points characterise the submesh as
         * detailed as it's possible.
         */

        VertexData * vert = mSelfVertex ? mVertex : mMesh->mVertex;
        const VertexUnit * poselem = vert->get(VT_Pos);
        VertexBuffer * vbuf = vert->getAttach(poselem->mSource);
        //如果同个源中包含了其他元素这里会产生一个问题
        Nui8 * vdata = (Nui8 *)vbuf->lock(Buffer::MM_READ);

        NCount vsz = vbuf->getUnitSize();

        vector<Cluster>::type boxes;
        boxes.reserve(count);

        boxes.push_back(Cluster());

        if (mIndexList[0]->mCount > 0)
        {
            Nui32 elsz = mIndexList[0]->mBuffer->getUnitSize() == 32 ? 4 : 2;
            Nui8 * idata = (Nui8 *)mIndexList[0]->mBuffer->lock(mIndexList[0]->mOffset * elsz,
                mIndexList[0]->mCount * elsz, IndexBuffer::MM_READ);

            for (NCount i = 0; i < mIndexList[0]->mCount; i++)
            {
                NIIi idx = (elsz == 2) ? ((Nui16 *)idata)[i] : ((Nui32 *)idata)[i];
                boxes[0].mIndices.insert(idx);
            }
            mIndexList[0]->mBuffer->unlock();
        }
        else
        {
            // 插入所有索引
            for (NCount i = mVertex->mOffset; i < mVertex->mCount; i++)
            {
                boxes[0].mIndices.insert(i);
            }
        }
        // 首先,找到子网格的最小和最大边界盒子
        boxes[0].computeBBox(poselem, vdata, vsz);

        // 记住子网格的几何中心
        Vector3f center = (boxes[0].mMax + boxes[0].mMin) * 0.5;

        // Ok, now loop until we have as many boxes, as we need extremes
        while (boxes.size() < count)
        {
            // 找到最大的bbox去分割
            Cluster * split_box = NULL;
            NIIf split_volume = -1;
            for (vector<Cluster>::type::iterator b = boxes.begin(); b != boxes.end(); ++b)
            {
                if (b->empty())
                    continue;

                NIIf v = b->volume();
                if (v > split_volume)
                {
                    split_volume = v;
                    split_box = &*b;
                }
            }

            // If we don't have what to split, break
            if (!split_box)
                break;

            // Find the coordinate axis to split the box into two
            NIIi split_axis = 0;
            NIIf split_length = split_box->mMax.x - split_box->mMin.x;
            for (NIIi i = 1; i < 3; i++)
            {
                //这里假设点的布局是均匀的,寻找最多点的一个轴去分割
                NIIf l = split_box->mMax[i] - split_box->mMin[i];
                if (l > split_length)
                {
                    split_length = l;
                    split_axis = i;
                }
            }
            // Now split the box into halves
            boxes.push_back(split_box->split(split_axis, poselem, vdata, vsz));
        }

        // Fine, now from every cluster choose the vertex that is most
        // distant from the geometrical center and from other extremes.
        for (vector<Cluster>::type::const_iterator b = boxes.begin(); b != boxes.end(); ++b)
        {
            NIIf rating = 0;
            Vector3f best_vertex;
            set<Nui32>::type::const_iterator i, iend = b->mIndices.end();
            //遍历一个盒子里的索引,找出一个最好的顶点
            for (i = b->mIndices.begin(); i != iend; ++i)
            {
                NIIf * v;
                poselem->getData(vdata + *i * vsz, v);

                Vector3f vv(v[0], v[1], v[2]);
                NIIf r = (vv - center).lengthSquared();
                vector<Vector3f >::type::const_iterator e, eend = mEdgePoints.end();
                // 和现存的比较,因为这些点不能挨得太近,选择与其他点距离和 最远的一个
                for (e = mEdgePoints.begin(); e != eend; ++e)
                {
                    r += (*e - vv).lengthSquared();
                }
                if (r > rating)
                {
                    rating = r;
                    best_vertex = vv;
                }
            }
            if (rating > 0)
                mEdgePoints.push_back(best_vertex);
        }
        vbuf->unlock();
    }
    //---------------------------------------------------------------------
    void SubMesh::setAutoLod(bool b)
    {
        mAutoLod = b;
        if (mMesh)
        {
            MeshLodScheme * scheme = mMesh->getLodScheme();
            scheme->destroy();
            mMesh->setAutoLod(true);
        }
    }
    //----------------------------------------------------------------------
    void __log(Log * l, GeoEdget * dst)
    {
        GeoEdget::EdgeGroupList::iterator i, iend;
        GeoEdget::EdgeList::iterator ei, eiend;
        GeoEdget::TriangleList::iterator ti, tiend;
        tiend = dst->mTriangleList.end();
        l->log(_T("Edge Data"));
        l->log(_T("---------"));
        NCount num = 0;
        for (ti = dst->mTriangleList.begin(); ti != tiend; ++ti, ++num)
        {
            GeoEdget::Triangle & t = *ti;
            l->log(_T("Triangle ") + N_conv(num) +                _T(" = {") +
                _T("indexSet=") + N_conv(t.mIndexDataIndex) +    _T(", ") +
                _T("vertexSet=") + N_conv(t.mVertexDataIndex) +    _T(", ") +
                _T("v0=") + N_conv(t.mVertIndex[0]) +            _T(", ") +
                _T("v1=") + N_conv(t.mVertIndex[1]) +            _T(", ") +
                _T("v2=") + N_conv(t.mVertIndex[2]) +            _T("}"));
        }
        iend = dst->mGeoList.end();
        for (i = dst->mGeoList.begin(); i != iend; ++i)
        {
            num = 0;
            eiend = i->mEdgeList.end();
            l->log(_T("Edge Group vertexSet=") + N_conv(i->mVertexDataIndex));
            for (ei = i->mEdgeList.begin(); ei != eiend; ++ei, ++num)
            {
                GeoEdget::Edge& e = *ei;
                l->log(
                    _T("Edge ") + N_conv(num) +                _T(" = {\n") +
                    _T("  tri0=") + N_conv(e.mTriIndex[0]) + _T(", \n") +
                    _T("  tri1=") + N_conv(e.mTriIndex[1]) + _T(", \n") +
                    _T("  v0=") + N_conv(e.mVertIndex[0]) +    _T(", \n") +
                    _T("  v1=") + N_conv(e.mVertIndex[1]) +    _T(", \n")
                    _T("  alone =") + N_conv(e.mAlone) +        _T(" \n")
                    _T("}"));
            }
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Mesh
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Mesh::Mesh(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, 
        ScriptTypeID stid, LangID lid) :
        Resource(stid, rid, gid, ls, rs, lid),
        mCenterRange(0.0f),
        mVertexMode(Buffer::M_NORMAL  | Buffer::M_HOST),
        mIndexMode(Buffer::M_NORMAL  | Buffer::M_HOST),
        mPrefabType(GSP_None),
        mVertex(0),
        mSkeleton(0),
        mCoordIndex(0),
        mTangentIndex(0),
        mRangeFactor(0.01f)
    {
        mMark = M_Skeleton | M_AutoLod;

        mLodScheme = static_cast<MeshLodScheme *>(N_Only(LodScheme).createDefault(N_LOD_Mesh));
        mLodScheme->add(0.0f, 0, getOriginID(), mGroup, true); // 基础
        mCreator = N_OnlyPtr(Mesh);
    }
    //-----------------------------------------------------------------------
    Mesh::~Mesh()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();
        if(mSkeleton != 0)
        {
            mSkeleton->unref();
            mSkeleton = 0;
        }
        GeoEdgetList::iterator i, iend = mEdgetList.end();
        for(i = mEdgetList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mEdgetList.clear();

        if(mLodScheme != 0)
        {
            N_delete mLodScheme;
            mLodScheme = 0;
        }
    }
    //-----------------------------------------------------------------------
    void Mesh::build(Mode mark)
    {
        switch(mark)
        {
        case M_ShadowVol :
        {
            if(mMark & M_ShadowVol)
                return;

            if(mVertex)
            {
                mVertex->rebuild(M_ShadowVol);
            }

            SubMeshList::iterator i, iend = mSubMeshList.end();
            for(i = mSubMeshList.begin(); i != iend; ++i)
            {
                SubMesh * s = *i;
                GeometryRaw::OperationType smtype = s->getGeometryType();
                if(s->mSelfVertex &&
                    (smtype == GeometryRaw::OT_TRIANGLE_FAN ||
                    smtype == GeometryRaw::OT_TRIANGLE_LIST ||
                    smtype == GeometryRaw::OT_TRIANGLE_STRIP))
                {
                    s->mVertex->rebuild(M_ShadowVol);
                }
            }
            mMark |= M_ShadowVol;
        }
        break;
        case M_Skeleton :
        {
            if((mMark & M_Skeleton) == 0)
            {
                SkeletonManager::combine(mVertex, mSkeletonVertexList, mMatrixs);
            }

            SubMeshList::iterator i, iend = mSubMeshList.end();
            for(i = mSubMeshList.begin(); i != iend; ++i)
            {
                (*i)->buildSkeleton();
            }
            mMark |= M_Skeleton;
        }
        break;
        case M_Edge :
        {
            if(mEdgetList.empty())
            {
                NCount i, iend = mLodScheme->getCount();
                for(i = 0; i < iend; ++i)
                {
                    if(mLodScheme->isRelation(i))
                    {
                        buildEdgeList(mLodScheme->getRelationIndex(i));
                    }
                }
                mMark |= M_Edge;
            }
        }
        break;
        case M_AutoLod:
        {
            mMark |= M_AutoLod;
        }
        break;
        default:
            break;
        }
    }
    //-----------------------------------------------------------------------
    void Mesh::remove(Mode mode)
    {
        mMark &= ~mode;
    }
    //-----------------------------------------------------------------------
    SubMesh * Mesh::createSub()
    {
        SubMesh * sub = N_new SubMesh(this);
        mSubMeshList.push_back(sub);

        if(mState == LS_LOADED)
            notify();

        return sub;
    }
    //-----------------------------------------------------------------------
    void Mesh::destroySub(Nidx index)
    {
        N_assert(index < mSubMeshList.size(), "error");

        SubMeshList::iterator sub = mSubMeshList.begin();
        std::advance(sub, index);
        N_delete *sub;
        mSubMeshList.erase(sub);

        GeoEdgetList::iterator i, iend = mEdgetList.end();
        for(i = mEdgetList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mEdgetList.clear();

        if(mMark & M_Edge)
        {
            NCount i, iend = mLodScheme->getCount();
            for(i = 0; i < iend; ++i)
            {
                if(mLodScheme->isRelation(i))
                {
                    buildEdgeList(mLodScheme->getRelationIndex(i));
                }
            }
        }

        if(mState == LS_LOADED)
            notify();
    }
    //-----------------------------------------------------------------------
    void Mesh::loadEndImpl()
    {
        if(N_Engine().getProcessSpace()->isShadowVolume(0))
        {
            if(mMark & M_AutoLod)
            {
                build(M_ShadowVol);
            }
            setGenEdget(true);
        }

        if(mMark & M_AutoLod)
        {
            mLodScheme->build();
            mLodScheme->map();
        }

        if(mMark & M_Edge)
        {
            NCount i, iend = mLodScheme->getCount();
            for(i = 0; i < iend; ++i)
            {
                if(mLodScheme->isRelation(i))
                {
                    buildEdgeList(mLodScheme->getRelationIndex(i));
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void Mesh::planImpl()
    {
#if (NII_USERMODE != NII_APPMODE)
        if(getCreator()->getVerbose())
            N_Only(Log).log(_I18n("加载网格,资源ID: ") + mOriginID);
#endif
        mSrcStream = N_Only(ResourceScheme).open(mSrc, mGroup, true, this);

        // 充分预先填充缓冲到主机内存
        mSrcStream = N_new MemStream(mSrcStream, mSrc);
    }
    //-----------------------------------------------------------------------
    void Mesh::unplanImpl()
    {
        N_delete mSrcStream;
    }
    //------------------------------------------------------------------------
    void Mesh::loadImpl()
    {
        MeshSerializer serl(0);
        //serl.setListener(N_Only(Mesh).getListener());

        serl.read(mSrcStream, this);
    }
    //-----------------------------------------------------------------------
    void Mesh::unloadImpl()
    {
        SubMeshList::iterator i, iend = mSubMeshList.end();
        for(i = mSubMeshList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        // Clear SubMesh lists
        mSubMeshList.clear();

        if(mVertex)
        {
            N_delete mVertex;
            mVertex = 0;
        }

        // Removes all LOD data
        removeAllLod();
        mMark &= ~M_ShadowVol;
        // remove all poses & animations
        removeAllAnimations();
        removeAllOffset();

        mSkeletonVertexList.clear();
        mMark |= M_Skeleton;
        setSkeletonID(0);
    }
    //-----------------------------------------------------------------------
    Mesh * Mesh::clone(ResourceID rid, GroupID ngid)
    {
        // This is a bit like a copy constructor, but with the additional aspect of registering the clone with
        //  the MeshManager

        // New Mesh is assumed to be manually defined rather than loaded since you're cloning it for a reason
        GroupID gid;
        if(ngid == N_TYPE_MAX(GroupID))
        {
            gid = this->getGroup();
        }
        else
        {
            gid = ngid;
        }
        Mesh * newMesh = N_Only(Mesh).createManual(rid, gid);

        SubMesh * newSub;
        SubMeshList::iterator subi, subiend = mSubMeshList.end();
        IndexDataList::const_iterator facei, faceiend;
        for(subi = mSubMeshList.begin(); subi != subiend; ++subi)
        {
            newSub = newMesh->createSub();
            newSub->mMaterialID = (*subi)->mMaterialID;
            newSub->setGeometryType((*subi)->getGeometryType());
            newSub->mSelfVertex = (*subi)->mSelfVertex;
            newSub->mEdgePoints = (*subi)->mEdgePoints;

            if((*subi)->mSelfVertex)
            {
                // Copy unique vertex data
                newSub->mVertex = (*subi)->mVertex->clone();
                // Copy unique index map
                newSub->mMatrixs = (*subi)->mMatrixs;
            }

            N_delete newSub->mIndexList[0];
            newSub->mIndexList.reserve((*subi)->mIndexList.size());
            newSub->mIndexList.clear();
            for(facei = (*subi)->mIndexList.begin(), faceiend = (*subi)->mIndexList.end(); facei != faceiend; ++facei)
            {
                IndexData * nid = (*facei)->clone();
                newSub->mIndexList.push_back(nid);
            }
            // Copy any bone svl
            newSub->mSkeletonVertexList = (*subi)->mSkeletonVertexList;
            newSub->mSkeletonVexrtexValid = (*subi)->mSkeletonVexrtexValid;
        }

        // Copy shared geometry and index map, if any
        if(mVertex)
        {
            newMesh->mVertex = mVertex->clone();
            newMesh->mMatrixs = mMatrixs;
        }

        // Copy any bone svl
        newMesh->mSkeletonVertexList = mSkeletonVertexList;
        // Copy bounds
        newMesh->mAABB = mAABB;
        newMesh->mCenterRange = mCenterRange;

        newMesh->mLodScheme = static_cast<MeshLodScheme *>(mLodScheme->clone());

        newMesh->mVertexMode = mVertexMode;
        newMesh->mIndexMode = mIndexMode;

        newMesh->setSkeleton(mSkeleton);

        newMesh->mMark = mMark;

        // Clone vertex animation
        AnimationList::iterator ia, iaend = mAnimations.end();
        for(ia = mAnimations.begin(); ia != iaend; ++ia)
        {
            Animation * newAnim = ia->second->clone(ia->second->getID());
            newMesh->mAnimations[ia->second->getID()] = newAnim;
        }

        // Clone pose list
        VertexOffsetList::iterator ip, ipend = mVertexOffsetList.end();
        for(ip = mVertexOffsetList.begin(); ip != ipend; ++ip)
        {
            VertexOffset * newPose = (*ip)->clone();
            newMesh->mVertexOffsetList.push_back(newPose);
        }

        newMesh->ref(true);

        return newMesh;
    }
    //-----------------------------------------------------------------------
    Vector3f Mesh::calcTangent(const Vector3f & position1,
        const Vector3f & position2, const Vector3f & position3,
            NIIf u1, NIIf v1, NIIf u2, NIIf v2, NIIf u3, NIIf v3)
    {
        //side0 is the vector along one side of the triangle of vertices passed in,
        //and side1 is the vector along another side. Taking the cross product of these returns the normal.
        Vector3f side0 = position1 - position2;
        Vector3f side1 = position3 - position1;
        //Calculate face normal
        Vector3f normal = side1.crossProduct(side0);
        normal.normalise();
        //Now we use a formula to calculate the tangent.
        NIIf deltaV0 = v1 - v2;
        NIIf deltaV1 = v3 - v1;
        Vector3f tangent = deltaV1 * side0 - deltaV0 * side1;
        tangent.normalise();
        //Calculate binormal
        NIIf deltaU0 = u1 - u2;
        NIIf deltaU1 = u3 - u1;
        Vector3f binormal = deltaU1 * side0 - deltaU0 * side1;
        binormal.normalise();
        //Now, we take the cross product of the tangents to get a vector which
        //should point in the same direction as our normal calculated above.
        //If it points in the opposite direction (the dot product between the normals is less than zero),
        //then we need to reverse the s and t tangents.
        //This is because the triangle has been mirrored when going from tangent space to object space.
        //reverse tangents if necessary
        Vector3f tangentCross = tangent.crossProduct(binormal);
        if(tangentCross.dotProduct(normal) < 0.0f)
        {
            tangent = -tangent;
            binormal = -binormal;
        }

        return tangent;
    }
    //-----------------------------------------------------------------------
    void Mesh::setRangeFactor(NIIf f)
    {
        mRangeFactor = f;

        if(mRangeFactor != 0.0)
        {
            Vector3f max = mAABB.getMax();
            Vector3f min = mAABB.getMin();
            // Pad out the AABB a little, helps with most bounds tests
            Vector3f scaler = (max - min) * mRangeFactor;
            mAABB.setMin(min - scaler);
            mAABB.setMax(max + scaler);
            // Pad out the sphere a little too
            mCenterRange = mCenterRange + (mCenterRange * mRangeFactor);
        }
    }
    //-----------------------------------------------------------------------
    void Mesh::setSkeletonID(SkeletonID sid)
    {
        if(sid != mSkeletonID)
        {
            mSkeletonID = sid;

            if(mSkeleton != 0)
            {
                mSkeleton->unref();
                mSkeleton = 0;
            }
            if(sid != 0)
            {
                try
                {
                    mSkeleton = static_cast<Skeleton *>(N_Only(Skeleton).load(sid, mGroup));
                    mSkeleton->ref();
                }
                catch(...)
                {
                    // Log this error
                    String msg = _I18n("不能加载骨骼 ");
                    msg += sid + _I18n(" for Mesh ") + N_conv(mOriginID)
                        + _I18n(". This Mesh will not be animated. ")
                        + _I18n("You can ignore this message if you are using an offline tool.");
                    N_Only(Log).log(msg);
                }
            }
            if(mState == LS_LOADED)
                notify();
        }
    }
    //-----------------------------------------------------------------------
    bool Mesh::isSkeletonFusion() const
    {
        return mSkeletonID != 0;
    }
    //---------------------------------------------------------------------
    void Mesh::setSkeleton(Skeleton * obj)
    {
        if(mSkeleton != obj && mSkeleton != 0)
        {
            mSkeleton->unref();
            mSkeleton = 0;

            if(obj != 0)
            {
                obj->ref();
                mSkeleton = obj;
                mSkeletonID = obj->getOriginID();
            }
        }
        else if(mSkeleton != obj)
        {
            if(obj != 0)
            {
                obj->ref();
                mSkeleton = obj;
                mSkeletonID = obj->getOriginID();
            }
        }
    }
    //-----------------------------------------------------------------------
    void Mesh::setVertexData(VertexData * vd)
    {
        if(mVertex)
            N_delete mVertex;
        mVertex = vd;
    }
    //-----------------------------------------------------------------------
    void Mesh::addSkeleton(const SkeletonVertex & vba)
    {
        mSkeletonVertexList.insert(SkeletonVertexList::value_type(vba.mVertexIndex, vba));
        mMark &= ~M_Skeleton;
    }
    //-----------------------------------------------------------------------
    void Mesh::removeSkeleton(Nidx vindex)
    {
        SkeletonVertexList::iterator i, temp, iend = mSkeletonVertexList.end();
        for(i = mSkeletonVertexList.begin(); i != iend;)
        {
            temp = ++i;
            if(i->second.mVertexIndex == vindex)
            {
                mSkeletonVertexList.erase(temp);
            }
        }
    }
    //-----------------------------------------------------------------------
    void Mesh::clearSkeleton()
    {
        mSkeletonVertexList.clear();
        mMark &= ~M_Skeleton;
    }
    //-----------------------------------------------------------------------
    void Mesh::_init(AnimationFusionObj * obj)
    {
        if(mSkeleton != 0)
        {
            mSkeleton->_init(obj);
            build(M_Skeleton);
        }

        // Animation states for vertex animation
        AnimationList::iterator i, iend = mAnimations.end();
        for(i = mAnimations.begin(); i != iend; ++i)
        {
            Animation * ani = i->second;
            if(!obj->isFusionExist(ani->getID()))
            {
                obj->addFusion(ani->createFusion(this));
            }
        }
    }
    //---------------------------------------------------------------------
    void Mesh::_append(AnimationFusionObj * obj)
    {

    }
    //---------------------------------------------------------------------
    void Mesh::_update(AnimationFusionObj * obj)
    {
        if(mSkeleton != 0)
        {
            mSkeleton->_update(obj);
        }

        AnimationList::iterator i, iend = mAnimations.end();
        for(i = mAnimations.begin(); i != iend; ++i)
        {
            Animation * ani = i->second;
            // Create animation at time index 0, default params mean this has weight 1 and is disabled
            AnimationFusion * f = obj->getFusion(ani->getID());
            if(f && f->getAnimationObj() == this)
            {
                // Update length incase changed
                f->setDur(ani->getDur());
                f->seek(std::min(ani->getDur(), f->tell()));
            }
            else
            {
                obj->addFusion(ani->createFusion(this));
            }
        }
    }
    //---------------------------------------------------------------------
    void Mesh::setLodScheme(MeshLodScheme * obj)
    {
        if (obj != mLodScheme)
        {
            if (mLodScheme != 0)
                N_delete mLodScheme;

            mLodScheme = obj;

            if (mLodScheme)
            {
                mLodScheme->map();
            }
        }
    }
    //---------------------------------------------------------------------
    void Mesh::setLodCount(NCount cnt)
    {
        N_assert(cnt >= 1, "最少一层");

        mLodScheme->setCount(cnt);
    }
    //---------------------------------------------------------------------
    NCount Mesh::getLodCount() const
    {
        return mLodScheme->getCount();
    }
    //---------------------------------------------------------------------
    void Mesh::removeAllLod()
    {
        SubMeshList::iterator i, iend = mSubMeshList.end();
        for(i = mSubMeshList.begin(); i != iend; ++i)
        {
            (*i)->removeAllLod();
        }

        mLodScheme->removeAll();
        mLodScheme->add(0, 0, getOriginID(), mGroup, true); // 基础
    }
    //---------------------------------------------------------------------
    void Mesh::buildEdgeList(Nidx lodidx)
    {
        if(mEdgetList.size() > lodidx && mEdgetList[lodidx] != 0)
            return;

        EdgeListBuilder eb;
        NCount vetsetcnt = 0;
        bool ebvalid = false;

        if(mVertex)
        {
            eb.addVertex(mVertex);
            vetsetcnt++;
        }

        // Prepare the builder using the submesh information
        SubMeshList::iterator i, iend = mSubMeshList.end();
        for(i = mSubMeshList.begin(); i != iend; ++i)
        {
            SubMesh * s = *i;
            GeometryRaw::OperationType smtype = s->getGeometryType();
            if(smtype != GeometryRaw::OT_TRIANGLE_FAN &&
                smtype != GeometryRaw::OT_TRIANGLE_LIST &&
                    smtype != GeometryRaw::OT_TRIANGLE_STRIP)
            {
                continue;
            }
            if(!s->isSelfVertex())
            {
                // Use shared vertex data, index as set 0
                eb.addIndex(s->getIndexData(lodidx), 0, smtype);
            }
            else if(s->isAutoLod())
            {
                // own vertex data, add it and reference it directly
                eb.addVertex(s->getVertexData());
                // LOD index data
                eb.addIndex(s->getIndexData(lodidx), vetsetcnt++, smtype);
            }
            ebvalid = true;
        }
        GeoEdget * edget;
        if(ebvalid)
        {
            edget = eb.build();
#if N_DEBUG
            Log * log = N_Only(Log).create(N_conv(mOriginID) + _T("_Lod_") +
                N_conv(lodidx) + _T("_EdgeList.log"), false, false);
            __log(log, edget);
            N_Only(Log).destroyLog(log->getName());
#endif
        }
        else
        {
            edget = N_new GeoEdget();
        }
        setEdgeList(lodidx, edget);
    }
    //---------------------------------------------------------------------
    void Mesh::buildTangentBuffer(VertexType dtype, bool build4D,
        bool mirrorUV, bool rotationUV)
    {
        TangentSpace tangentsCalc;
        tangentsCalc.setSplitMirrored(mirrorUV);
        tangentsCalc.setSplitRotated(rotationUV);
        tangentsCalc.setStoreParityInW(build4D);

        // 首先是共享几何
        if(mVertex)
        {
            tangentsCalc.setVertexData(mVertex);
            bool found = false;
            SubMeshList::iterator c, cend = mSubMeshList.end();
            for(c = mSubMeshList.begin(); c != cend; ++c)
            {
                SubMesh * sm = *c;
                if(!sm->mSelfVertex)
                {
                    tangentsCalc.addIndexData(sm->mIndexList[0]);
                    found = true;
                }
            }
            if(found)
            {
                TangentSpace::Result res = tangentsCalc.build(dtype, mTangentIndex, mCoordIndex);
                // todo 需要删除旧设置？
                // If any vertex splitting happened, we have to give them bone svl
                if(isSkeletonFusion())
                {
                    for(TangentSpace::IndexRemapList::iterator r = res.indexesRemapped.begin();
                        r != res.indexesRemapped.end(); ++r)
                    {
                        TangentSpace::IndexRemap & remap = *r;
                        // Copy all bone svl from the split vertex
                        SkeletonVertexList::iterator vbstart = mSkeletonVertexList.lower_bound(remap.splitVertex.first);
                        SkeletonVertexList::iterator vbend = mSkeletonVertexList.upper_bound(remap.splitVertex.first);
                        for(SkeletonVertexList::iterator vba = vbstart; vba != vbend; ++vba)
                        {
                            SkeletonVertex newAsgn = vba->second;
                            newAsgn.mVertexIndex = static_cast<Nui32>(remap.splitVertex.second);
                            // multimap insert doesn't invalidate iterators
                            addSkeleton(newAsgn);
                        }
                    }
                }
                // todo 需要删除旧设置？
                // Update poses (some vertices might have been duplicated)
                // we will just check which vertices have been split and copy
                // the offset for the original vertex to the corresponding new vertex
                VertexOffsetList::iterator i, iend = mVertexOffsetList.end();
                for(i = mVertexOffsetList.begin(); i != iend; ++i)
                {
                    VertexOffset * current_pose = *i;
                    ++i;
                    const VertexOffset::OffsetMap & offset_map = current_pose->getVertexOffsets();

                    for(TangentSpace::VertexSplits::iterator it = res.vertexSplits.begin();
                        it != res.vertexSplits.end(); ++it)
                    {
                        TangentSpace::VertexSplit & split = *it;

                        VertexOffset::OffsetMap::const_iterator found_offset = offset_map.find(split.first);

                        // copy the offset
                        if(found_offset != offset_map.end())
                        {
                            current_pose->add(split.second, found_offset->second);
                        }
                    }
                }
            }
        }
        // 子网格专用几何
        for(SubMeshList::iterator i = mSubMeshList.begin(); i != mSubMeshList.end(); ++i)
        {
            SubMesh * sm = *i;
            if(sm->mSelfVertex)
            {
                tangentsCalc.clear();
                tangentsCalc.setVertexData(sm->mVertex);
                tangentsCalc.addIndexData(sm->mIndexList[0]);
                TangentSpace::Result res = tangentsCalc.build(dtype, mTangentIndex, mCoordIndex);

                // If any vertex splitting happened, we have to give them bone svl
                if(isSkeletonFusion())
                {
                    for(TangentSpace::IndexRemapList::iterator r = res.indexesRemapped.begin();
                        r != res.indexesRemapped.end(); ++r)
                    {
                        TangentSpace::IndexRemap & remap = *r;
                        // Copy all bone svl from the split vertex
                        SkeletonVertexList::const_iterator vbstart =
                            sm->getSkeletonList().lower_bound(remap.splitVertex.first);
                        SkeletonVertexList::const_iterator vbend =
                            sm->getSkeletonList().upper_bound(remap.splitVertex.first);
                        for(SkeletonVertexList::const_iterator vba = vbstart; vba != vbend; ++vba)
                        {
                            SkeletonVertex newAsgn = vba->second;
                            newAsgn.mVertexIndex = static_cast<Nui32>(remap.splitVertex.second);
                            // multimap insert doesn't invalidate iterators
                            sm->addSkeleton(newAsgn);
                        }
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    bool Mesh::checkTangentBuffer(bool coordtype)
    {
        // Go through all the vertex data and locate source and dest (must agree)
        bool sharedGeometryDone = false;
        bool find = false;
        bool firstOne = true;
        SubMeshList::iterator i, iend = mSubMeshList.end();
        for(i = mSubMeshList.begin(); i != iend; ++i)
        {
            SubMesh * sm = *i;
            VertexData * vertexData;
            // 如果是共享几何处理一次就行了
            if(sm->mSelfVertex)
            {
                vertexData = sm->mVertex;
            }
            else
            {
                if(sharedGeometryDone)
                    continue;
                vertexData = mVertex;
                sharedGeometryDone = true;
            }

            Nui16 targetIndex = 0;
            const VertexUnit * srce = 0;
            for(targetIndex = 0; targetIndex < NII_MAX_TextureCoord; ++targetIndex)
            {
                const VertexUnit * exve = vertexData->get(VT_Tex_Coord, targetIndex);
                if(!exve)
                    break; // finish if we've run out, t will be the target

                if(!srce)
                {
                    // 我们还在寻找来源纹理坐标
                    if(exve->mType == DT_Float2x)
                    {
                        // Ok, we found it
                        srce = exve;
                    }
                }
                // 如果找到则不会再执行
                if(!find && coordtype)
                {
                    // We're looking for the destination
                    // Check to see if we've found a possible
                    if(exve->mType == DT_Float3x)
                    {
                        // 这是一个3D的集,可能是切线
                        find = true;
                    }
                }
            }
            // 如果找到则不会再执行
            if(!find && !coordtype)
            {
                targetIndex = 0;
                // Look for existing semantic
                const VertexUnit * exve = vertexData->get(VT_Tangent, targetIndex);
                if(exve)
                {
                    find = true;
                }
            }

            // After iterating, we should have a source and a possible destination (t)
            if(!srce)
            {
                N_EXCEPT(NotExist, _I18n("Cannot locate an appropriate 2D texture coordinate set for all the vertex data in this mesh to create tangents from. "));
            }
            // Check that we agree with previous decisions, if this is not the
            // first one, and if we're not just using the existing one
            if(!firstOne && !find)
            {
                if(srce->mIndex != mCoordIndex)
                {
                    N_EXCEPT(InvalidParam,
                        _I18n("Multiple sets of vertex data in this mesh disagree on the appropriate index to use for the source texture coordinates.This ambiguity must be rectified before tangents can be generated."));
                }
                if(targetIndex != mTangentIndex)
                {
                    N_EXCEPT(InvalidParam,
                        _I18n("Multiple sets of vertex data in this mesh disagree on the appropriate index to use for the target texture coordinates.This ambiguity must be rectified before tangents can be generated."));
                }
            }

            // Otherwise, save this result
            mCoordIndex = srce->mIndex;
            mTangentIndex = targetIndex;

            firstOne = false;
        }
        return find;
    }
    //---------------------------------------------------------------------
    void Mesh::setEdgeList(Nidx lod, GeoEdget * edg)
    {
        NIIi iend = lod - mEdgetList.size();
        if(iend >= 0)
        {
            for(NIIi i = 0; i <= iend; ++i)
            {
                mEdgetList.push_back(0);
            }
        }
        else if(mEdgetList[lod])
        {
            N_delete mEdgetList[lod];
        }

        mEdgetList[lod] = edg;
    }
    //---------------------------------------------------------------------
    GeoEdget * Mesh::getEdgeList(Nidx lod) const
    {
        GeoEdget * edg;

        bool rel = mLodScheme->isRelation(lod);
        if(rel)
        {
            Nidx localidx = mLodScheme->getRelationIndex(lod);
            if(localidx > mEdgetList.size())
            {
                if(mEdgetList[localidx] == 0)
                    const_cast<Mesh*>(this)->buildEdgeList(localidx);
            }
            else
            {
                const_cast<Mesh*>(this)->buildEdgeList(localidx);
            }

            return mEdgetList[localidx];
        }
        else
        {
            mLodScheme->get(lod, edg);
        }
        return edg;
    }
    //---------------------------------------------------------------------
    void Mesh::setAutoLod(bool b)
    {
        if(b)
            mMark |= M_AutoLod;
        else
            mMark &= ~M_AutoLod;
    }
    //---------------------------------------------------------------------
    bool Mesh::getAutoLod() const
    {
        return mMark & M_AutoLod;
    }
    //---------------------------------------------------------------------
    void Mesh::setGenEdget(bool b)
    {
        if(b)
            mMark |= M_Edge;
        else
            mMark &= ~M_Edge;
    }
    //---------------------------------------------------------------------
    bool Mesh::isGenEdget() const
    {
        return mMark & M_Edge;
    }
    //---------------------------------------------------------------------
    NCount Mesh::calcSize() const
    {
        // calculate GPU size
        NCount ret = 0;
        Nui16 i;

        if(mVertex)
        {
            for(i = 0; i < mVertex->getAttachCount(); ++i)
            {
                ret += mVertex->getAttach(i)->getSize();
            }
        }

        SubMeshList::const_iterator si, siend = mSubMeshList.end();
        for(si = mSubMeshList.begin(); si != siend; ++si)
        {
            // Dedicated vertices
            if((*si)->mSelfVertex)
            {
                for(i = 0; i < (*si)->mVertex->getAttachCount(); ++i)
                {
                    ret += (*si)->mVertex->getAttach(i)->getSize();
                }
            }
            if((*si)->mIndexList[0]->mBuffer != 0)
            {
                // Index data
                ret += (*si)->mIndexList[0]->mBuffer->getSize();
            }
        }
        return ret;
    }
    //-----------------------------------------------------------------------------
    bool Mesh::isVertexFusion() const
    {
        return !mAnimations.empty();
    }
    //---------------------------------------------------------------------
    Animation * Mesh::createAnimation(AnimationID id, TimeDurMS dur)
    {
        Animation * re = AnimationObj::createAnimation(id, dur);
        re->setObj(this);
        return re;
    }
    //---------------------------------------------------------------------
    VertexOffset * Mesh::createOffset(Nid id, Nui16 vertidx)
    {
        VertexOffset * re = N_new VertexOffset(id, vertidx);
        mVertexOffsetList.push_back(re);
        return re;
    }
    //---------------------------------------------------------------------
    VertexOffset * Mesh::getOffset(Nidx index)
    {
        if(index >= getOffsetCount())
        {
            N_EXCEPT(InvalidParam, _I18n("Index out of bounds"));
        }
        return mVertexOffsetList[index];
    }
    //---------------------------------------------------------------------
    VertexOffset * Mesh::getOffset(Nid id)
    {
        VertexOffsetList::iterator i, iend = mVertexOffsetList.end();
        for(i = mVertexOffsetList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == id)
                return *i;
        }
        StringStream str;
        str << _I18n("No pose called ") << id << _I18n(" found in Mesh ") << mOriginID;
        N_EXCEPT(NotExist, str.str());
    }
    //---------------------------------------------------------------------
    void Mesh::removeOffset(Nidx index)
    {
        if(index >= getOffsetCount())
        {
            N_EXCEPT(InvalidParam, _I18n("Index out of bounds"));
        }
        VertexOffsetList::iterator i = mVertexOffsetList.begin();
        std::advance(i, index);
        N_delete *i;
        mVertexOffsetList.erase(i);
    }
    //---------------------------------------------------------------------
    void Mesh::removeOffset(Nid id)
    {
        VertexOffsetList::iterator i, iend = mVertexOffsetList.end();
        for(i = mVertexOffsetList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == id)
            {
                N_delete *i;
                mVertexOffsetList.erase(i);
                return;
            }
        }
        StringStream str;
        str << _I18n("No pose called ") << id << _I18n(" found in Mesh ") << mOriginID;
        N_EXCEPT(NotExist, str.str());
    }
    //---------------------------------------------------------------------
    void Mesh::removeAllOffset()
    {
        VertexOffsetList::iterator i, iend = mVertexOffsetList.end();
        for (i = mVertexOffsetList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mVertexOffsetList.clear();
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // PatchMesh
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PatchMesh::PatchMesh(ResourceID rid, GroupID gid, VertexData * vd,
        Nmark vbmode, Nmark ibmode, LangID lid) :
            Mesh(rid, gid, 0, 0, N_CmdObj_PatchMesh, lid),
            mData(vd)
    {
        mVertexMode = vbmode;
        mIndexMode = ibmode;
    }
    //-----------------------------------------------------------------------
    PatchMesh:: ~PatchMesh()
    {
    }
    //-----------------------------------------------------------------------
    void PatchMesh::define(NCount width, NCount height, NCount umaxLevel, NCount vmaxLevel, FaceSideType side)
    {
        mSurface.define(mData, width, height, CurvedSurface::T_Bezier, umaxLevel, vmaxLevel, side);
    }
    //-----------------------------------------------------------------------
    void PatchMesh::update(NCount width, NCount height, NCount umaxLevel, NCount vmaxLevel, FaceSideType side)
    {
        mSurface.define(mData, width, height, CurvedSurface::T_Bezier, umaxLevel, vmaxLevel, side);
        SubMesh * sm = getSub(0);
        
        VertexData * vd = sm->isSelfVertex() ? sm->getVertexData() : this->getVertexData();
        VertexBuffer * vb = vd->getAttach(vd->get(VT_Pos)->mSource);

        mSurface.build(vb, 0, sm->getIndexData(0)->mBuffer, 0);
    }
    //-----------------------------------------------------------------------
    void PatchMesh::setSubdivision(NIIf factor)
    {
        mSurface.setSubdivisionFactor(factor);
        getSub(0)->getIndexData(0)->mCount = mSurface.getCurrentIndexCount();
    }
    //-----------------------------------------------------------------------
    void PatchMesh::loadImpl(void)
    {
        SubMesh * sm = createSub();
        VertexData * smvd;
        N_Only(GBuffer).create(smvd);
        
        sm->setVertexData(smvd);
        sm->setSelfVertex(true);

        smvd->mOffset = 0;
        smvd->mCount = mSurface.getVertexCount();

        VertexUnitList::const_iterator i, iend = mData->getUnits().end();
        for(i = mData->getUnits().begin(); i != iend; ++i)
        {
            smvd->add(*i);
        }
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, mData->getUnitSize(0), smvd->mCount, mVertexMode);
        smvd->attach(0, vbuf);

        // Set up index buffer
        sm->getIndexData(0)->mOffset = 0;
        sm->getIndexData(0)->mCount = mSurface.getIndexCount();
        N_Only(GBuffer).create(sm->getIndexData(0)->mBuffer,
            16, // only 16-bit indexes supported, patches shouldn't be bigger than that
            sm->getIndexData(0)->mCount, mIndexMode);

        // Build patch
        mSurface.build(vbuf, 0, sm->getIndexData(0)->mBuffer, 0);

        setAABB(mSurface.getAABB());
        setRangeFactor(0.01f);
        setCenterRange(mSurface.getCenterRange());
    }
    //------------------------------------------------------------------------
    void PatchMesh::planImpl()
    {
    }
    //------------------------------------------------------------------------
}