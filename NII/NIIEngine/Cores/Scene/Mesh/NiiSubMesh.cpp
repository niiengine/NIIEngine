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
#include "NiiSubMesh.h"
#include "NiiVertexData.h"
#include "NiiException.h"
#include "NiiMeshManager.h"
#include "NiiMaterialManager.h"
#include "NiiSkeletonManager.h"
#include "NiiMeshLodScheme.h"
#include "NiiStrConv.h"

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
        if(mVertex != 0)
        {
            N_delete mVertex;
            mVertex = 0;
        }
        removeAllLod();

        if(mIndexList[0] != 0)
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
        if(mIndexList.size() < lod + 1)
        {
            NCount i, iend = lod + 1 - mIndexList.size();
            for(i = 0 ; i < iend; ++i)
            {
                mIndexList.push_back(0);
            }
        }

        if(mIndexList[lod] != 0)
            N_delete mIndexList[lod];
        mIndexList[lod] = obj;
    }
    //-----------------------------------------------------------------------
    void SubMesh::setLodCount(NCount c)
    {
        if(mIndexList.size() > c)
        {
            IndexDataList::iterator j, jend = mIndexList.end();
            for(j = mIndexList.begin() + c - 1; j != jend; ++j)
            {
                if(*j != 0)
                    N_delete *j;
            }
            mIndexList.erase(mIndexList.begin() + c - 1, jend);
        }
        else if(mIndexList.size() < c)
        {
            NCount i, iend = c - mIndexList.size();
            for(i = 0 ; i < iend; ++i)
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
        if(mIndexList[0]->mCount != 0)
        {
            gr.add(GRT_Index);
        }
        else
        {
            gr.remove(GRT_Index);
        }

        if(lod > 0 && lod < mIndexList.size())
        {
            gr.mIndex = mIndexList[lod];
        }
        else
        {
            gr.mIndex = mIndexList[0];
        }

        gr.mType = mType;
        gr.mVertex = mSelfVertex ?  mVertex : mMesh->mVertex;
    }
    //-----------------------------------------------------------------------
    void SubMesh::addSkeleton(const SkeletonVertex & sv)
    {
        if(mSelfVertex == false)
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
    void SubMesh::clearSkeleton()
    {
        mSkeletonVertexList.clear();
        mSkeletonVexrtexValid = false;
    }
    //-----------------------------------------------------------------------
    void SubMesh::buildSkeleton()
    {
        if(mSkeletonVexrtexValid == false)
        {
            SkeletonManager::combine(mVertex, mSkeletonVertexList, mMatrixs);
            mSkeletonVexrtexValid = true;
        }
    }
    //---------------------------------------------------------------------
    void SubMesh::removeAllLod()
    {
        IndexDataList::iterator i, iend = mIndexList.end();
        for(i = mIndexList.begin() + 1; i != iend; ++i)
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
            if(mIndices.empty ())
                return true;
            if(mMin == mMax)
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
            if(v [0] < mMin.x)
                mMin.x = v[0];
            if(v [1] < mMin.y)
                mMin.y = v[1];
            if(v [2] < mMin.z)
                mMin.z = v[2];
            if(v [0] > mMax.x)
                mMax.x = v[0];
            if(v [1] > mMax.y)
                mMax.y = v[1];
            if(v [2] > mMax.z)
                mMax.z = v[2];
        }
        //---------------------------------------------
        void computeBBox(const VertexUnit * pos, Nui8 * vdata, NCount vsz)
        {
            mMin.x = mMin.y = mMin.z = Math::POS_INFINITY;
            mMax.x = mMax.y = mMax.z = Math::NEG_INFINITY;

            set<Nui32>::type::const_iterator i, iend = mIndices.end();
            for(i = mIndices.begin(); i != iend; ++i)
            {
                NIIf * v;
                pos->getData(vdata + *i * vsz, v);
                extend (v);
            }
        }
        //---------------------------------------------
        Cluster split(NIIi split_axis, const VertexUnit * pos, Nui8 * vdata, NCount vsz)
        {
            NIIf r = (mMin[split_axis] + mMax[split_axis]) * 0.5f;
            Cluster newbox;

            // 分隔所有点到新的bbox中
            set<Nui32>::type::iterator i = mIndices.begin();
            for(; i != mIndices.end();)
            {
                NIIf * v;
                pos->getData(vdata + *i * vsz, v);
                if(v[split_axis] > r)
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

        if(mIndexList[0]->mCount > 0)
        {
            Nui32 elsz = mIndexList[0]->mBuffer->getUnitSize() == 32 ? 4 : 2;
            Nui8 * idata = (Nui8 *)mIndexList[0]->mBuffer->lock(mIndexList[0]->mOffset * elsz,
                mIndexList[0]->mCount * elsz, IndexBuffer::MM_READ);

            for(NCount i = 0; i < mIndexList[0]->mCount; i++)
            {
                NIIi idx = (elsz == 2) ? ((Nui16 *)idata)[i] : ((Nui32 *)idata)[i];
                boxes[0].mIndices.insert(idx);
            }
            mIndexList[0]->mBuffer->unlock();
        }
        else
        {
            // 插入所有索引
            for(NCount i = mVertex->mOffset; i < mVertex->mCount; i++)
            {
                boxes[0].mIndices.insert(i);
            }
        }
        // 首先,找到子网格的最小和最大边界盒子
        boxes [0].computeBBox (poselem, vdata, vsz);

        // 记住子网格的几何中心
        Vector3f center = (boxes [0].mMax + boxes [0].mMin) * 0.5;

        // Ok, now loop until we have as many boxes, as we need extremes
        while(boxes.size () < count)
        {
            // 找到最大的bbox去分割
            Cluster * split_box = NULL;
            NIIf split_volume = -1;
            for(vector<Cluster>::type::iterator b = boxes.begin(); b != boxes.end(); ++b)
            {
                if(b->empty())
                    continue;

                NIIf v = b->volume();
                if(v > split_volume)
                {
                    split_volume = v;
                    split_box = &*b;
                }
            }

            // If we don't have what to split, break
            if(!split_box)
                break;

            // Find the coordinate axis to split the box into two
            NIIi split_axis = 0;
            NIIf split_length = split_box->mMax.x - split_box->mMin.x;
            for(NIIi i = 1; i < 3; i++)
            {
                //这里假设点的布局是均匀的,寻找最多点的一个轴去分割
                NIIf l = split_box->mMax [i] - split_box->mMin [i];
                if(l > split_length)
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
        for(vector<Cluster>::type::const_iterator b = boxes.begin(); b != boxes.end(); ++b)
        {
            NIIf rating = 0;
            Vector3f best_vertex;
            set<Nui32>::type::const_iterator i, iend = b->mIndices.end();
            //遍历一个盒子里的索引,找出一个最好的顶点
            for(i = b->mIndices.begin(); i != iend; ++i)
            {
                NIIf * v;
                poselem->getData(vdata + *i * vsz, v);

                Vector3f vv (v[0], v[1], v[2]);
                NIIf r = (vv - center).lengthSquared ();
                vector<Vector3f >::type::const_iterator e, eend = mEdgePoints.end();
                // 和现存的比较,因为这些点不能挨得太近,选择与其他点距离和 最远的一个
                for(e = mEdgePoints.begin(); e != eend; ++e)
                {
                    r += (*e - vv).lengthSquared();
                }
                if(r > rating)
                {
                    rating = r;
                    best_vertex = vv;
                }
            }
            if(rating > 0)
                mEdgePoints.push_back(best_vertex);
        }
        vbuf->unlock();
    }
    //---------------------------------------------------------------------
    void SubMesh::setAutoLod(bool b)
    {
        mAutoLod = b;
        if(mMesh)
        {
            MeshLodScheme * scheme = mMesh->getLodScheme();
            scheme->destroy();
            mMesh->setAutoLod(true);
        }
    }
    //----------------------------------------------------------------------
}