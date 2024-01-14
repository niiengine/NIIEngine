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
        // ��Ե�����Ӱ����Ⱦ��Ӧ����1:1��
        N_assert(srcedge->mGeoList.size() == out.size(), "error");

        // �Ƿ�ʹ�� McGuire ����, һ�������ȸ�����������
        // �⽫�޷��ڶ�������ı�ԵȺ��������ʹ��
        bool useMcGuire = srcedge->mGeoList.size() <= 1;
        GeoEdget::EdgeGroupList::const_iterator egi, egiend = srcedge->mGeoList.end();

        LightType lightType = srclight->getType();

        // ÿ����Ҫ���������ݵĸ���,�������һ����С��������������,�ڲ�ͬ��GLƽ̨���кܴ�Ĳ�ͬ
        NCount preCountIndexes = 0;
        // Ԥ�ȼ�����Ҫ��������С

        for(egi = srcedge->mGeoList.begin(); egi != egiend; ++egi)
        {
            const GeoEdget::EdgeGroup & eg = *egi;
            bool firstDarkCapTri = true;

            GeoEdget::EdgeList::const_iterator i, iend = eg.mEdgeList.end();
            for(i = eg.mEdgeList.begin(); i != iend; ++i)
            {
                const GeoEdget::Edge & edge = *i;

                // ������Ե,��2������������Եĵƹ�����,���ߵ�ֻ��1����Ч�ĵƹ�������������˻��ı�Ե
                NIIb lightFacing = srcedge->mViewPosList[edge.mTriIndex[0]];
                if((edge.mAlone && lightFacing) || (!edge.mAlone &&
                        (lightFacing != srcedge->mViewPosList[edge.mTriIndex[1]])))
                {
                    preCountIndexes += 3;

                    // �Ƿ�ʹ�����޼�ѹ����?
                    if(!(lightType == LT_DIRECTIONAL && (flags & ShadowObj::VT_Infinity)))
                    {
                        preCountIndexes += 3;
                    }

                    if(useMcGuire)
                    {
                        // ������������,ʹ��McGuire et al����,һ�������ȸ�������������һ��
                        // ��(��ȡ�����������)
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
                // ��������
                if(flags & ShadowObj::VT_FrontCap)
                {
                    // ����ʹ��������㼯��������
                    GeoEdget::TriangleList::const_iterator ti, tiend;
                    GeoEdget::ViewPosList::const_iterator lfi;
                    ti = srcedge->mTriangleList.begin() + eg.mOffset;
                    tiend = ti + eg.mCount;
                    lfi = srcedge->mViewPosList.begin() + eg.mOffset;
                    for ( ; ti != tiend; ++ti, ++lfi)
                    {
                        N_assert(ti->mVertexDataIndex == eg.mVertexDataIndex, "error");
                        // ������Ƿ�����ƹ�
                        if (*lfi)
                        {
                            preCountIndexes += 3;
                        }
                    }
                }
            }
            else
            {
                // ����������
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
                        // ������Ƿ�������ƹ�
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
        // ������������д��,������С����Ҫ��ƥ��
        Nui16 * pIdx = (Nui16 *)index->lock(Buffer::MM_WHOLE, 0, sizeof(Nui16) * preCountIndexes);
        NCount numIndices = 0;
        ShadowVolumeList::const_iterator si;
        // ��������ÿ����������������Ӱ����Ⱦ��
        for(egi = srcedge->mGeoList.begin(), si = out.begin(); egi != egiend; ++egi, ++si)
        {
            const GeoEdget::EdgeGroup & eg = *egi;
            // Ϊ�����Ӱ����Ⱦ���ʼ��������ʼ
            IndexData * indexData = (*si)->mRenderOp.mIndex;
            if(indexData->mBuffer != index)
            {
                (*si)->setIndex(index);
                indexData = (*si)->mRenderOp.mIndex;
            }
            indexData->mOffset = numIndices;
            // ԭʼ��������(��������ѹ����)
            NCount originalVertexCount = eg.mVertex->mCount;
            bool firstDarkCapTri = true;
            Nui16 darkCapStart = 0;

            GeoEdget::EdgeList::const_iterator i, iend = eg.mEdgeList.end();
            for(i = eg.mEdgeList.begin(); i != iend; ++i)
            {
                const GeoEdget::Edge & edge = *i;

                // ������Ե,��2������������Եĵƹ�����,���ߵ�ֻ��1����Ч��,�ƹ�������������˻��ı�Ե
                NIIb lightFacing = srcedge->mViewPosList[edge.mTriIndex[0]];
                if((edge.mAlone && lightFacing) || (!edge.mAlone &&
                    (lightFacing != srcedge->mViewPosList[edge.mTriIndex[1]])))
                {
                    NCount v0 = edge.mVertIndex[0];
                    NCount v1 = edge.mVertIndex[1];
                    if(!lightFacing)
                    {
                        // ������Ե����,��t1������ƹ�ʱ
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

                    // �Ƿ����޼�ѹ?
                    if(!(lightType == LT_DIRECTIONAL && (flags & ShadowObj::VT_Infinity)))
                    {
                        // ��Ӷ����������γ��ķ�
                        *pIdx++ = static_cast<Nui16>(v0 + originalVertexCount);
                        *pIdx++ = static_cast<Nui16>(v1 + originalVertexCount);
                        *pIdx++ = static_cast<Nui16>(v1);
                        numIndices += 3;
                    }

                    if(useMcGuire)
                    {
                        // ������������,ʹ��McGuire et al����,һ�������ȸ�������������һ��
                        // ��(��ȡ�����������)
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
            // ��McGuire et al���� && ������
            if(!useMcGuire)
            {
                // ������
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
                        // ����Ƿ�ƹ�������
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

            // �����Ƹ�
            if(flags & ShadowObj::VT_FrontCap)
            {
                // �ָ�����?
                if ((*si)->getLightCap() != 0)
                {
                    // ���������Ӱ����Ⱦ�����������
                    indexData->mCount = numIndices - indexData->mOffset;

                    // ��ȡ���������������ڸ���
                    indexData = (*si)->getLightCap()->mRenderOp.mIndex;
                    // �������Ⱦ��Ŀ�ʼ����,��Ϊ���������Ӱ����Ⱦ����������������洢��
                    indexData->mOffset = numIndices;
                }

                // ����������㼯�е�������
                GeoEdget::TriangleList::const_iterator ti, tiend;
                GeoEdget::ViewPosList::const_iterator lfi;
                ti = srcedge->mTriangleList.begin() + eg.mOffset;
                tiend = ti + eg.mCount;
                lfi = srcedge->mViewPosList.begin() + eg.mOffset;
                for( ; ti != tiend; ++ti, ++lfi)
                {
                    const GeoEdget::Triangle & t = *ti;
                    N_assert(t.mVertexDataIndex == eg.mVertexDataIndex, "error");
                    // ������Ƿ�������ƹ���
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
            // ������������
            indexData->mCount = numIndices - indexData->mOffset;
        }

        // �����������
        index->unlock();

        N_assert(numIndices == preCountIndexes, "error");

        // �ڵ���ģʽ��,�������û�г�����������
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

        // TODO:����Ӧ����Ӷ����(��ʹ�õ�����) ����ȷ��Դand��Ŀ�Ļ�������ͬ���ֽڶ���,��΢��������
        NIIf * dst = src + count * 3;

        SIMDUtil::getImplementation()->extrude(light, extent, src, dst, count);

        vertex->unlock();
    }
    //-------------------------------------------------------------------------
}