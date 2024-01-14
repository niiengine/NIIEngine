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
#include "NiiSkeletonManager.h"
#include "NiiGBufferManager.h"
#include "NiiSkeleton.h"

namespace NII
{
    //-----------------------------------------------------------------------
    template<> SkeletonManager * Singleton<SkeletonManager>::mOnly = 0;
    /*SkeletonManager * SkeletonManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    SkeletonManager & SkeletonManager::getOnly()
    {
        N_assert(mOnly, "error");
        return(*mOnly);
    }*/
    //-----------------------------------------------------------------------
    SkeletonManager::SkeletonManager() :
        ResourceManager(RT_Skeleton)
    {
        mScriptLevel = 300;
        N_Only(ResourceScheme).add(mType, this);
    }
    //-----------------------------------------------------------------------
    SkeletonManager::~SkeletonManager()
    {
        N_Only(ResourceScheme).remove(mType);
    }
    //-----------------------------------------------------------------------
    void SkeletonManager::init()
    {

    }
    //-----------------------------------------------------------------------
    Resource * SkeletonManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs,
            const PropertyData * params)
    {
        return N_new Skeleton(rid, gid, ls, rs);
    }
    //---------------------------------------------------------------------
    void SkeletonManager::combine(VertexData * dest, const SkeletonVertexList & svl,
        BoneIndexList & matmap)
    {
        // Create or reuse blend weight / indexes buffer
        // Indices are always a UBYTE4 no matter how many weights per vertex
        // Weights are more specific though since they are Reals
        Nui16 bindIndex;

        // Build the index map brute-force. It's possible to store the index map
        // in .mesh, but maybe trivial.
        BoneIndexList boneTblend;

        NCount wcount = SkeletonManager::opt(const_cast<SkeletonVertexList &>(svl), dest->mCount);
        if(wcount == 0)
            return;

        if(svl.empty())
        {
            // Just in case
            boneTblend.clear();
            matmap.clear();
            return;
        }

        typedef set<Nui16>::type BoneIndexSet;
        BoneIndexSet boneIndex;

        SkeletonVertexList::const_iterator b, bend = svl.end();
        for(b = svl.begin(); b != bend; ++b)
        {
            boneIndex.insert(b->second.mBoneIndex);
        }

        // 分配索引映射空间
        matmap.resize(boneIndex.size());
        boneTblend.resize(*boneIndex.rbegin() + 1);

        Nui16 blendIndex = 0;
        // 生成骨头索引和混合索引的索引映射
        BoneIndexSet::const_iterator bi, biend = boneIndex.end();
        for(bi = boneIndex.begin(); bi != biend; ++bi, ++blendIndex)
        {
            boneTblend[*bi] = blendIndex;
            matmap[blendIndex] = *bi;
        }

        const VertexUnit * exve = dest->get(VT_Matrix_Index);
        if(exve)
        {
            // 删除已经存在
            bindIndex = exve->mSource;
            dest->detach(bindIndex);
            dest->remove(VT_Matrix_Index);
            dest->remove(VT_Matrix_Weight);
        }
        else
        {
            // 获取新的绑定
            bindIndex = dest->getFreeAttachIndex();
        }

        VertexBuffer * vbuf;        //index             weight
        N_Only(GBuffer).create(vbuf, sizeof(Nui8) * 4 + sizeof(NIIf) * wcount,
            dest->mCount, Buffer::M_NORMAL  | Buffer::M_HOST);
        dest->attach(bindIndex, vbuf);

        const VertexUnit * pIdxElem, * pWeightElem;

        // add new vertex elements
        // Note, insert directly after all elements using the same source as
        // position to abide by pre-Dx9 format restrictions
        const VertexUnit * firstElem = dest->get(0);
        if(firstElem->mVType == VT_Pos)
        {
            Nidx insertPoint = 1;
            while(insertPoint < dest->getCount() &&
                dest->get(insertPoint)->mSource == firstElem->mSource)
            {
                ++insertPoint;
            }
            const VertexUnit * idxElem = dest->insert(insertPoint, DT_UByte4x, VT_Matrix_Index,
                bindIndex, 0);
            //这里的offset 要继承上面的
            const VertexUnit * wtElem = dest->insert(insertPoint + 1,
                DataTypeUtil::getMultiType(DT_Float1x, wcount), VT_Matrix_Weight,
                    bindIndex, sizeof(Nui8) * 4);

            pIdxElem = idxElem;
            pWeightElem = wtElem;
        }
        else
        {
            // Position is not the first semantic, therefore this declaration is
            // not pre-Dx9 compatible anyway, so just tack it on the end
            const VertexUnit * idxElem = dest->add(DT_UByte4x, VT_Matrix_Index, bindIndex, 0);

            //这里的offset 要继承上面的
            const VertexUnit * wtElem = dest->add(DataTypeUtil::getMultiType(DT_Float1x, wcount),
                VT_Matrix_Weight, bindIndex, sizeof(Nui8) * 4);

            pIdxElem = idxElem;
            pWeightElem = wtElem;
        }

        // 分配数据
        NCount v;
        SkeletonVertexList::const_iterator i = svl.begin();
        SkeletonVertexList::const_iterator iend = svl.end();

        Nui8 * pBase = static_cast<Nui8 *>(vbuf->lock(Buffer::MM_WHOLE));
        NIIf * pWeight;
        Nui8 * pIndex;
        for(v = 0; v < dest->mCount; ++v)
        {
            // 转换为指定的指针
            pWeightElem->getData(pBase, pWeight);
            pIdxElem->getData(pBase, pIndex);
            for(NCount bone = 0; bone < wcount; ++bone)
            {
                // 我们有这个顶点的数据吗?
                if(i != iend && i->second.mVertexIndex == v)
                {
                    // If so, write weight
                    *pWeight++ = i->second.mWeight;
                    *pIndex++ = static_cast<Nui8>(boneTblend[i->second.mBoneIndex]);
                    ++i;
                }
                else
                {
                    // Ran out of svl for this vertex, use weight 0 to indicate empty.
                    // If no bones are defined (an error in itself) set bone 0 as the assigned bone.
                    *pWeight++ = (bone == 0) ? 1.0f : 0.0f;
                    *pIndex++ = 0;
                }
            }
            pBase += vbuf->getUnitSize();
        }
        vbuf->unlock();
    }
    //-----------------------------------------------------------------------
    typedef multimap<NIIf, SkeletonVertexList::iterator>::type WeightIteratorMap;
    //-----------------------------------------------------------------------
    NCount SkeletonManager::opt(SkeletonVertexList & svl, NCount vexcnt)
    {
        // Iterate through, finding the largest # bones per vertex
        NCount maxBones = 0;
        bool nonbonevex = false;
        SkeletonVertexList::iterator i;

        for(NCount v = 0; v < vexcnt; ++v)
        {
            NCount currBones = svl.count(v);
            if(currBones <= 0)
                nonbonevex = true;
            if(maxBones < currBones)
                maxBones = currBones;

            if(currBones > NII_MAX_BlendFactor)
            {
                // 映射,通过权重排序
                WeightIteratorMap weightToAssignmentMap;
                // 这个顶点有太多骨头分配
                // Find the start & end (end is in iterator terms ie exclusive)
                std::pair<SkeletonVertexList::iterator, SkeletonVertexList::iterator> range;
                range = svl.equal_range(v);
                for(i = range.first; i != range.second; ++i)
                {
                    // 插入值 weight->iterator
                    weightToAssignmentMap.insert(WeightIteratorMap::value_type(i->second.mWeight, i));
                }
                // 反向遍历权重映射, 移去最低的n个
                NCount numToRemove = currBones - NII_MAX_BlendFactor;
                WeightIteratorMap::iterator remIt = weightToAssignmentMap.begin();

                while(numToRemove--)
                {
                    // Erase this one
                    svl.erase(remIt->second);
                    ++remIt;
                }

                if(maxBones > NII_MAX_BlendFactor)
                {
                    maxBones = NII_MAX_BlendFactor;
                }
            }

            NIIf totalWeight = 0;
            std::pair<SkeletonVertexList::iterator,
                SkeletonVertexList::iterator> normalise_range = svl.equal_range(v);
            for(i = normalise_range.first; i != normalise_range.second; ++i)
            {
                totalWeight += i->second.mWeight;
            }
            if(!Math::isEqual(totalWeight, 1.0f))
            {
                for(i = normalise_range.first; i != normalise_range.second; ++i)
                {
                    i->second.mWeight = i->second.mWeight / totalWeight;
                }
            }
        }

        if(nonbonevex)
        {
            // Warn that we've non-skinned vertices
           // N_Only(Log).log(_I18n("警告:网格 '") + StrConv::conv(mOriginID) + _I18n(" '含有顶点成分不包含骨骼系数成分"));
        }
        return maxBones;
    }
    //-----------------------------------------------------------------------
}