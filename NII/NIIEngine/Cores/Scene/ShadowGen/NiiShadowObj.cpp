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
#include "NiiShadowObj.h"
#include "NiiEdgeListBuilder.h"
#include "NiiSimdUtil.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiLight.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiStencilRenderPattern.h"

namespace NII
{
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //ShadowObj
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    ShadowVolume::ShadowVolume() :
        mMaterial(0),
        mLightCap(0)
    {
    }
    //-------------------------------------------------------------------------
    ShadowVolume::~ShadowVolume()
    {
        N_delete mLightCap;
    }
    //-------------------------------------------------------------------------
    bool ShadowVolume::isVisible() const
    {
        return true;
    }
    //-------------------------------------------------------------------------
    void ShadowVolume::getLight(LightList & ll) const
    {
        ll.clear();
    }
    //-------------------------------------------------------------------------
    NIIf ShadowVolume::distanceSquared(const Camera *) const
    {
        return 0;
    }
    //-------------------------------------------------------------------------
    const Material * ShadowVolume::getMaterial() const
    {
        return mMaterial;
    }
    //-------------------------------------------------------------------------
    void ShadowVolume::getGeometry(GeometryRaw & op) const
    {
        op = mRenderOp;
    }
    //-------------------------------------------------------------------------
    void ShadowVolume::setIndex(IndexBuffer * index)
    {
        mRenderOp.mIndex->mBuffer = index;
        if(mLightCap)
            mLightCap->setIndex(index);
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //ShadowObj
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    ShadowObj::ShadowObj()
    {

    }
    //-------------------------------------------------------------------------
    ShadowObj::ShadowObj(SpaceID sid) :
        SpaceObj(sid)
    {

    }
    //-------------------------------------------------------------------------
    ShadowObj::~ShadowObj()
    {
    }
    //-------------------------------------------------------------------------
    GeoEdget * ShadowObj::getEdgeList()
    {
        return NULL;
    }
    //-------------------------------------------------------------------------
    bool ShadowObj::isEdgeExist()
    {
        return false;
    }
    //-------------------------------------------------------------------------
    const AABox & ShadowObj::getBackCapAABB(const Light & light, NIIf extent) const
    {
        // Extrude own light cap bounds
        mDarkCapAABB = getFrontCapAABB();
        Vector3f extrusionDir;
        Vector4f light4 = light.getSpace4();
        if(light4.w == 0)
        {
            // Parallel projection guarantees min/max relationship remains the same
            extrusionDir.x = -light4.x;
            extrusionDir.y = -light4.y;
            extrusionDir.z = -light4.z;
            extrusionDir.normalise();
            extrusionDir *= extent;
            mDarkCapAABB.setMin(mDarkCapAABB.getMin() + extrusionDir);
            mDarkCapAABB.setMax(mDarkCapAABB.getMax() + extrusionDir);
        }
        else
        {
            Vector3f oldMin, oldMax, currentCorner, norl;
            // Getting the original values
            oldMin = mDarkCapAABB.getMin();
            oldMax = mDarkCapAABB.getMax();
            // Starting the mDarkCapAABB again with a null content
            mDarkCapAABB.setNull();

            // merging all the extruded corners

            // 0 : min min min
            currentCorner = oldMin;
            extrusionDir.x = currentCorner.x - light4.x;
            extrusionDir.y = currentCorner.y - light4.y;
            extrusionDir.z = currentCorner.z - light4.z;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);

            // 6 : min min max
            // only z has changed
            currentCorner.z = oldMax.z;
            extrusionDir.z = currentCorner.z - light4.z;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);

            // 5 : min max max
            currentCorner.y = oldMax.y;
            extrusionDir.y = currentCorner.y - light4.y;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);

            // 1 : min max min
            currentCorner.z = oldMin.z;
            extrusionDir.z = currentCorner.z - light4.z;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);

            // 2 : max max min
            currentCorner.x = oldMax.x;
            extrusionDir.x = currentCorner.x - light4.x;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);

            // 4 : max max max
            currentCorner.z = oldMax.z;
            extrusionDir.z = currentCorner.z - light4.z;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);

            // 7 : max min max
            currentCorner.y = oldMin.y;
            extrusionDir.y = currentCorner.y - light4.y;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);

            // 3 : max min min
            currentCorner.z = oldMin.z;
            extrusionDir.z = currentCorner.z - light4.z;
            extrusionDir.normalise(norl);
            mDarkCapAABB.merge(currentCorner + extent * norl);
        }
        return mDarkCapAABB;
    }
    //-------------------------------------------------------------------------
    const ShadowVolumeList & ShadowObj::getVolumeList(const Light * srclight,
        NIIf extent, Nmark flags, IndexBuffer * destindex)
    {
        static ShadowVolumeList dummyList;
        return dummyList;
    }
    //-------------------------------------------------------------------------
    void ShadowObj::generate(const GeoEdget * srcedge, IndexBuffer * index,
        const Light * srclight, Nmark flags, const ShadowVolumeList & out)
    {
        // 边缘组和阴影可渲染物应该是1:1的
        N_assert(srcedge->mGeoList.size() == out.size(), "error");

        // 是否使用 McGuire 方法, 一个三角扇覆盖整个轮廓
        // 这将无法在多个单独的边缘群体中正常使用
        bool useMcGuire = srcedge->mGeoList.size() <= 1;
        GeoEdget::EdgeGroupList::const_iterator egi, egiend = srcedge->mGeoList.end();

        LightType lightType = srclight->getType();

        // 每个需要的索引数据的个数,如果锁定一个很小的索引缓存区域,在不同的GL平台中有很大的不同
        NCount preCountIndexes = 0;
        // 预先计算需要的索引大小

        for(egi = srcedge->mGeoList.begin(); egi != egiend; ++egi)
        {
            const GeoEdget::EdgeGroup & eg = *egi;
            bool firstDarkCapTri = true;

            GeoEdget::EdgeList::const_iterator i, iend = eg.mEdgeList.end();
            for(i = eg.mEdgeList.begin(); i != iend; ++i)
            {
                const GeoEdget::Edge & edge = *i;

                // 轮廓边缘,当2个三角行有相对的灯光照射,或者当只有1个有效的灯光照射的三角形退化的边缘
                NIIb lightFacing = srcedge->mViewPosList[edge.mTriIndex[0]];
                if((edge.mAlone && lightFacing) || (!edge.mAlone &&
                        (lightFacing != srcedge->mViewPosList[edge.mTriIndex[1]])))
                {
                    preCountIndexes += 3;

                    // 是否使用无限挤压距离?
                    if(!(lightType == LT_DIRECTIONAL && (flags & ShadowObj::VT_Infinity)))
                    {
                        preCountIndexes += 3;
                    }

                    if(useMcGuire)
                    {
                        // 处理暗罩三角形,使用McGuire et al方法,一个三角扇覆盖所有轮廓和一个
                        // 点(采取最初的三角形)
                        if(flags & ShadowObj::VT_BackCap)
                        {
                            if(firstDarkCapTri)
                            {
                                firstDarkCapTri = false;
                            }
                            else
                            {
                                preCountIndexes += 3;
                            }
                        }
                    }
                }
            }

            if(useMcGuire)
            {
                // 处理亮罩
                if(flags & ShadowObj::VT_FrontCap)
                {
                    // 遍历使用这个顶点集的三角形
                    GeoEdget::TriangleList::const_iterator ti, tiend;
                    GeoEdget::ViewPosList::const_iterator lfi;
                    ti = srcedge->mTriangleList.begin() + eg.mOffset;
                    tiend = ti + eg.mCount;
                    lfi = srcedge->mViewPosList.begin() + eg.mOffset;
                    for ( ; ti != tiend; ++ti, ++lfi)
                    {
                        N_assert(ti->mVertexDataIndex == eg.mVertexDataIndex, "error");
                        // 检测它是否面向灯光
                        if (*lfi)
                        {
                            preCountIndexes += 3;
                        }
                    }
                }
            }
            else
            {
                // 处理所有罩
                NIIi increment = ((flags & ShadowObj::VT_BackCap) ? 3 : 0) + ((flags & ShadowObj::VT_FrontCap) ? 3 : 0);
                if(increment != 0)
                {
                    // Iterate over the triangles which are using this vertex set
                    GeoEdget::TriangleList::const_iterator ti, tiend;
                    GeoEdget::ViewPosList::const_iterator lfi;
                    ti = srcedge->mTriangleList.begin() + eg.mOffset;
                    tiend = ti + eg.mCount;
                    lfi = srcedge->mViewPosList.begin() + eg.mOffset;
                    for( ; ti != tiend; ++ti, ++lfi)
                    {
                        N_assert(ti->mVertexDataIndex == eg.mVertexDataIndex, "error");
                        // 检测它是否是面向灯光
                        if(*lfi)
                            preCountIndexes += increment;
                    }
                }
            }
        }
        // End pre-count
        //Check if index buffer is to small
        if(preCountIndexes > index->getUnitCount())
        {
            N_Only(Log).log(LL_Fatal,
                _I18n("Warning: shadow index buffer size to small. Auto increasing buffer size to") +
                    N_conv(sizeof(Nui16) * preCountIndexes));

            SpaceManager * pManager = N_Engine().getProcessSpace();
            if(pManager && pManager->getRenderPattern())
            {
                StencilRenderPattern * srp = dynamic_cast<StencilRenderPattern *>(pManager->getRenderPattern());
                if(srp)
                    srp->setIndexSize(preCountIndexes);
            }

            //Check that the index buffer size has actually increased
            if(preCountIndexes > index->getUnitCount())
            {
                //increasing index buffer size has failed
                N_EXCEPT(InvalidParam, _I18n("Lock request out of bounds."));
            }
        }
        // 锁定缓存用于写入,索引大小和需要的匹配
        Nui16 * pIdx = (Nui16 *)index->lock(Buffer::MM_WHOLE, 0, sizeof(Nui16) * preCountIndexes);
        NCount numIndices = 0;
        ShadowVolumeList::const_iterator si;
        // 基于它们每个照射面遍历组和阴影可渲染物
        for(egi = srcedge->mGeoList.begin(), si = out.begin(); egi != egiend; ++egi, ++si)
        {
            const GeoEdget::EdgeGroup & eg = *egi;
            // 为这个阴影可渲染物初始化索引开始
            IndexData * indexData = (*si)->mRenderOp.mIndex;
            if(indexData->mBuffer != index)
            {
                (*si)->setIndex(index);
                indexData = (*si)->mRenderOp.mIndex;
            }
            indexData->mOffset = numIndices;
            // 原始顶点数量(不包含挤压副本)
            NCount originalVertexCount = eg.mVertex->mCount;
            bool firstDarkCapTri = true;
            Nui16 darkCapStart = 0;

            GeoEdget::EdgeList::const_iterator i, iend = eg.mEdgeList.end();
            for(i = eg.mEdgeList.begin(); i != iend; ++i)
            {
                const GeoEdget::Edge & edge = *i;

                // 轮廓边缘,当2个三角行有相对的灯光照射,或者当只有1个有效的,灯光照射的三角形退化的边缘
                NIIb lightFacing = srcedge->mViewPosList[edge.mTriIndex[0]];
                if((edge.mAlone && lightFacing) || (!edge.mAlone &&
                    (lightFacing != srcedge->mViewPosList[edge.mTriIndex[1]])))
                {
                    NCount v0 = edge.mVertIndex[0];
                    NCount v1 = edge.mVertIndex[1];
                    if(!lightFacing)
                    {
                        // 交换边缘索引,当t1不面向灯光时
                        std::swap(v0, v1);
                    }

                    /* Note edge(v0, v1) run anticlockwise along the edge from
                    the light facing tri so to point shadow volume tris outward,
                    light cap indexes have to be backwards

                    We emit 2 tris if light is a point light, 1 if light
                    is directional, because directional lights cause all
                    points to converge to a single point at infinity.

                    First side tri = near1, near0, far0
                    Second tri = far0, far1, near1

                    'far' indexes are 'near' index + originalVertexCount
                    because 'far' verts are in the second half of the
                    buffer
                    */
                    N_assert(v1 < 65536 && v0 < 65536 && (v0 + originalVertexCount) < 65536,
                        "Vertex count exceeds 16-bit index limit!");
                    *pIdx++ = static_cast<Nui16>(v1);
                    *pIdx++ = static_cast<Nui16>(v0);
                    *pIdx++ = static_cast<Nui16>(v0 + originalVertexCount);
                    numIndices += 3;

                    // 是否无限挤压?
                    if(!(lightType == LT_DIRECTIONAL && (flags & ShadowObj::VT_Infinity)))
                    {
                        // 添加额外三角形形成四方
                        *pIdx++ = static_cast<Nui16>(v0 + originalVertexCount);
                        *pIdx++ = static_cast<Nui16>(v1 + originalVertexCount);
                        *pIdx++ = static_cast<Nui16>(v1);
                        numIndices += 3;
                    }

                    if(useMcGuire)
                    {
                        // 处理暗罩三角形,使用McGuire et al方法,一个三角扇覆盖所有轮廓和一个
                        // 点(采取最初的三角形)
                        if (flags & ShadowObj::VT_BackCap)
                        {
                            if (firstDarkCapTri)
                            {
                                darkCapStart = static_cast<Nui16>(v0 + originalVertexCount);
                                firstDarkCapTri = false;
                            }
                            else
                            {
                                *pIdx++ = darkCapStart;
                                *pIdx++ = static_cast<Nui16>(v1 + originalVertexCount);
                                *pIdx++ = static_cast<Nui16>(v0 + originalVertexCount);
                                numIndices += 3;
                            }
                        }
                    }
                }
            }
            // 非McGuire et al方法 && 处理暗罩
            if(!useMcGuire)
            {
                // 处理暗照
                if (flags & ShadowObj::VT_BackCap)
                {
                    // Iterate over the triangles which are using this vertex set
                    GeoEdget::TriangleList::const_iterator ti, tiend;
                    GeoEdget::ViewPosList::const_iterator lfi;
                    ti = srcedge->mTriangleList.begin() + eg.mOffset;
                    tiend = ti + eg.mCount;
                    lfi = srcedge->mViewPosList.begin() + eg.mOffset;
                    for ( ; ti != tiend; ++ti, ++lfi)
                    {
                        const GeoEdget::Triangle& t = *ti;
                        N_assert(t.mVertexDataIndex == eg.mVertexDataIndex, "error");
                        // 检测是否灯光照射面
                        if(*lfi)
                        {
                            N_assert(t.mVertIndex[0] < 65536 && t.mVertIndex[1] < 65536 &&
                                t.mVertIndex[2] < 65536, "16-bit index limit exceeded!");
                            *pIdx++ = static_cast<Nui16>(t.mVertIndex[1] + originalVertexCount);
                            *pIdx++ = static_cast<Nui16>(t.mVertIndex[0] + originalVertexCount);
                            *pIdx++ = static_cast<Nui16>(t.mVertIndex[2] + originalVertexCount);
                            numIndices += 3;
                        }
                    }
                }
            }

            // 最后处理灯盖
            if(flags & ShadowObj::VT_FrontCap)
            {
                // 分割亮罩?
                if ((*si)->getLightCap() != 0)
                {
                    // 更新这个阴影可渲染物的索引记数
                    indexData->mCount = numIndices - indexData->mOffset;

                    // 获取亮罩索引数据用于更新
                    indexData = (*si)->getLightCap()->mRenderOp.mIndex;
                    // 这个可渲染物的开始索引,因为这个环中阴影可渲染物的索引都是连续存储的
                    indexData->mOffset = numIndices;
                }

                // 遍历这个顶点集中的三角形
                GeoEdget::TriangleList::const_iterator ti, tiend;
                GeoEdget::ViewPosList::const_iterator lfi;
                ti = srcedge->mTriangleList.begin() + eg.mOffset;
                tiend = ti + eg.mCount;
                lfi = srcedge->mViewPosList.begin() + eg.mOffset;
                for( ; ti != tiend; ++ti, ++lfi)
                {
                    const GeoEdget::Triangle & t = *ti;
                    N_assert(t.mVertexDataIndex == eg.mVertexDataIndex, "error");
                    // 检测它是否是面向灯光面
                    if (*lfi)
                    {
                        N_assert(t.mVertIndex[0] < 65536 && t.mVertIndex[1] < 65536 &&
                            t.mVertIndex[2] < 65536, "16-bit index limit exceeded!");
                        *pIdx++ = static_cast<Nui16>(t.mVertIndex[0]);
                        *pIdx++ = static_cast<Nui16>(t.mVertIndex[1]);
                        *pIdx++ = static_cast<Nui16>(t.mVertIndex[2]);
                        numIndices += 3;
                    }
                }
            }
            // 更新索引计算
            indexData->mCount = numIndices - indexData->mOffset;
        }

        // 解除索引缓存
        index->unlock();

        N_assert(numIndices == preCountIndexes, "error");

        // 在调试模式下,检测我们没有超出索引缓存
        N_assert(numIndices <= index->getUnitCount(),
            "Index buffer overrun while generating shadow volume!! "
                "You must increase the size of the shadow index buffer.");
    }
    // ------------------------------------------------------------------------
    void ShadowObj::extrude(VertexBuffer * vertex, NCount count, const Vector4f & light, 
        NIIf extent)
    {
        N_assert (vertex->getUnitSize() == sizeof(NIIf) * 3,
            "Position buffer should contain only positions!");

        NIIf * src = (NIIf *)vertex->lock(Buffer::MM_READ | Buffer::MM_WRITE);

        // TODO:我们应该添加额外的(不使用的数据) 顶点确保源and和目的缓存有相同的字节对齐,轻微提升性能
        NIIf * dst = src + count * 3;

        SIMDUtil::getImplementation()->extrude(light, extent, src, dst, count);

        vertex->unlock();
    }
    //-------------------------------------------------------------------------
}