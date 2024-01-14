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
#include "NiiPixelChain.h"
#include "NiiRenderObj.h"
#include "NiiGBufferManager.h"
#include "NiiMaterialManager.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiGeometryObj.h"
#include "NiiStrConv.h"
#include <limits>

namespace NII
{
    //-----------------------------------------------------------------------
    PixelChain::PixelChain(SpaceID sid, NCount maxElements,
        NCount numberOfChains, bool useTextureCoords, bool useColours):
            SpaceObj(sid),
            mMaterialID(0),
            mMaterial(0),
            mPixelCount(maxElements),
            mChainCount(numberOfChains),
            mTexCoord(useTextureCoords),
            mTexColour(useColours),
            mOriBase(Vector3f::X),
            mRadius(0.0f),
            mTexCoordU(true),
            mFaceCamera(true),
            mUnitChange(true),
            mReGenGeoBuffer(true),
            mAABBValid(false),
            mIndexValid(false)
    {
        N_Only(GBuffer).create(mVertexData);
        mIndexData = N_new IndexData();

        mTexCoordMin = 0.0f;
        mTexCoordMax = 1.0f;

        resizeGeoCount();

        mVertexData->mOffset = 0;
        // index data set up later
        // set basic white material
        this->setMaterial(MaterialManager::WhiteColour->getOriginID());
    }
    //-----------------------------------------------------------------------
    PixelChain::~PixelChain()
    {
        N_delete mVertexData;
        N_delete mIndexData;
    }
    //-----------------------------------------------------------------------
    void PixelChain::add(Nidx index, const Pixel & p)
    {
        if(index >= mChainCount)
        {
            N_EXCEPT(NotExist, _I18n("index out of bounds"));
        }

        Chain & seg = mChainList[index];
        if(seg.mStart == N_Max_Num_Value)
        {
            // Tail starts at end, mStart grows backwards
            seg.mEnd = mPixelCount - 1;
            seg.mStart = seg.mEnd;
            mIndexValid = false;
        }
        else
        {
            if(seg.mStart == 0)
            {
                // Wrap backwards
                seg.mStart = mPixelCount - 1;
            }
            else
            {
                // Just step backward
                --seg.mStart;
            }
            // Run out of elements?
            if(seg.mStart == seg.mEnd)
            {
                // Move mEnd backwards too, losing the end of the segment and re-using
                // it in the mStart
                if (seg.mEnd == 0)
                    seg.mEnd = mPixelCount - 1;
                else
                    --seg.mEnd;
            }
        }

        // Set the details
        mPixelList[seg.mBegin + seg.mStart] = p;

        mIndexValid = false;
        mAABBValid = false;

        if(mSpaceNode)
            mSpaceNode->notify();
    }
    //-----------------------------------------------------------------------
/*     void PixelChain::removeChainElement(Nidx index)
    {
        if(index >= mChainCount)
        {
            N_EXCEPT(NotExist, _I18n("index out of bounds"));
        }

        Chain & seg = mChainList[index];
        if(seg.mStart == N_Max_Num_Value)
            return; // do nothing, nothing to remove

        if(seg.mEnd == seg.mStart)
        {
            // last item
            seg.mStart = seg.mEnd = N_Max_Num_Value;
        }
        else if(seg.mEnd == 0)
        {
            seg.mEnd = mPixelCount - 1;
        }
        else
        {
            --seg.mEnd;
        }

        // we removed an entry so indexes need updating
        mIndexValid = false;
        mAABBValid = false;

        if(mSpaceNode)
            mSpaceNode->notify();
    } */
    //-----------------------------------------------------------------------
    void PixelChain::resizeGeoCount()
    {
        // Allocate enough space for everything
        mPixelList.resize(mChainCount * mPixelCount);
        mVertexData->mCount = mPixelList.size() * 2;

        // Configure chains
        mChainList.resize(mChainCount);
        for(NCount i = 0; i < mChainCount; ++i)
        {
            Chain & seg = mChainList[i];
            seg.mBegin = i * mPixelCount;
            seg.mEnd = seg.mStart = N_Max_Num_Value;
        }
    }
    //-----------------------------------------------------------------------
    void PixelChain::genGeoBuffer()
    {
        if(mUnitChange)
        {
            mVertexData->removeAll();

            NCount offset = 0;
            mVertexData->add(DT_Float3x, VT_Pos, 0, offset);
            offset += DataTypeUtil::getSize(DT_Float3x);

            if(mTexColour)
            {
                mVertexData->add(DT_Colour, VT_Diffuse, 0, offset);
                offset += DataTypeUtil::getSize(DT_Colour);
            }

            if(mTexCoord)
            {
                mVertexData->add(DT_Float2x, VT_Tex_Coord, 0, offset);
            }

            if(!mTexCoord && !mTexColour)
            {
                N_Only(Log).log(_I18n("Error - PixelChain '") + N_conv(mID) +
                    _I18n("' is using neither texture coordinates or vertex colours; ")
                        _I18n("it will not be visible on some rendering APIs so you should change this so you use one or the other."));
            }
            mUnitChange = false;
        }

        if(mReGenGeoBuffer)
        {
            mVertexData->detachAll();

            VertexBuffer * pBuffer;
            N_Only(GBuffer).create(pBuffer, mVertexData->getUnitSize(0), mVertexData->mCount, Buffer::M_NORMAL | Buffer::M_WHOLE);
            mVertexData->attach(0, pBuffer);

            if(mIndexData->mBuffer != 0)
                mIndexData->mBuffer->unref();

            N_Only(GBuffer).create(mIndexData->mBuffer, 16, mChainCount * mPixelCount * 6, Buffer::M_NORMAL );
            // NB we don't set the indexCount on IndexData here since we will
            // probably use less than the maximum number of indices

            mReGenGeoBuffer = false;
        }
    }
    //-----------------------------------------------------------------------
    void PixelChain::setMaxPixelCount(NCount count)
    {
        mPixelCount = count;
        resizeGeoCount();
        mReGenGeoBuffer = true;
        mIndexValid = false;
    }
    //-----------------------------------------------------------------------
    NCount PixelChain::getMaxPixelCount() const
    {
        return mPixelCount;
    }
    //-----------------------------------------------------------------------
    void PixelChain::setChainCount(NCount count)
    {
        mChainCount = count;
        resizeGeoCount();
        mReGenGeoBuffer = true;
        mIndexValid = false;
    }
    //-----------------------------------------------------------------------
    NCount PixelChain::getChainCount() const
    {
        return mChainCount;
    }
    //-----------------------------------------------------------------------
    void PixelChain::setTextureCoord(bool b, NIIf start, NIIf end)
    {
        mTexCoordMin = start;
        mTexCoordMax = end;
        mTexCoord = b;
        mUnitChange = mReGenGeoBuffer = true;
        mIndexValid = false;
    }
    //-----------------------------------------------------------------------
    void PixelChain::setVertexColour(bool b)
    {
        mTexColour = b;
        mUnitChange = mReGenGeoBuffer = true;
        mIndexValid = false;
    }
    //-----------------------------------------------------------------------
    void PixelChain::removeAll(Nidx index)
    {
        if(index >= mChainCount)
        {
            N_EXCEPT(NotExist, _I18n("chainIndex out of bounds"));
        }
        Chain & seg = mChainList[index];

        // Just reset mStart & mEnd
        seg.mEnd = seg.mStart = N_Max_Num_Value;

        // we removed an entry so indexes need updating
        mIndexValid = false;
        mAABBValid = false;
        // tell parent node to update bounds
        if(mSpaceNode)
            mSpaceNode->notify();
    }
    //-----------------------------------------------------------------------
    void PixelChain::removeAll()
    {
        for(NCount i = 0; i < mChainCount; ++i)
        {
            removeAll(i);
        }
    }
    //-----------------------------------------------------------------------
    void PixelChain::modify(Nidx chainIndex, Nidx elementIndex, const Pixel & p)
    {
        if(chainIndex >= mChainCount)
        {
            N_EXCEPT(NotExist, _I18n("chainIndex out of bounds"));
        }
        Chain & seg = mChainList[chainIndex];
        if (seg.mStart == N_Max_Num_Value)
        {
            N_EXCEPT(NotExist, _I18n("Chain segment is empty"));
        }

        NCount idx = seg.mStart + elementIndex;
        // adjust for the edge and mBegin
        idx = (idx % mPixelCount) + seg.mBegin;

        mPixelList[idx] = p;

        mAABBValid = false;
        // tell parent node to update bounds
        if(mSpaceNode)
            mSpaceNode->notify();
    }
    //-----------------------------------------------------------------------
    const Pixel & PixelChain::getPixel(Nidx chainIndex, NCount elementIndex) const
    {
        if(chainIndex >= mChainCount)
        {
            N_EXCEPT(NotExist, _I18n("chainIndex out of bounds"));
        }
        const Chain & seg = mChainList[chainIndex];

        NCount idx = seg.mStart + elementIndex;
        // adjust for the edge and mBegin
        idx = (idx % mPixelCount) + seg.mBegin;

        return mPixelList[idx];
    }
    //-----------------------------------------------------------------------
    NCount PixelChain::getCount(Nidx chainIndex) const
    {
        if(chainIndex >= mChainCount)
        {
            N_EXCEPT(NotExist, _I18n("chainIndex out of bounds"));
        }
        const Chain & seg = mChainList[chainIndex];

        if(seg.mEnd < seg.mStart)
        {
            return seg.mEnd - seg.mStart + mPixelCount + 1;
        }
        else
        {
            return seg.mEnd - seg.mStart + 1;
        }
    }
    //-----------------------------------------------------------------------
    void PixelChain::_notify(Camera * cam)
    {
        genGeoBuffer();
        VertexBuffer * pBuffer = mVertexData->getAttach(0);
        void * pBufferStart = pBuffer->lock(Buffer::MM_WHOLE);

        const Vector3f & camPos = cam->getSpacePos();
        Vector3f eyePos = mSpaceNode->getSpaceOri().inverse() *
            (camPos - mSpaceNode->getSpacePos()) / mSpaceNode->getSpaceScale();

        Vector3f chainTangent;
        for(ChainList::iterator segi = mChainList.begin();
            segi != mChainList.end(); ++segi)
        {
            Chain & seg = *segi;

            // Skip 0 or 1 element segment counts
            if(seg.mStart != N_Max_Num_Value && seg.mStart != seg.mEnd)
            {
                NCount laste = seg.mStart;
                for(NCount e = seg.mStart; ; ++e) // until break
                {
                    // Wrap forwards
                    if (e == mPixelCount)
                        e = 0;

                    const Pixel & elem = mPixelList[e + seg.mBegin];
                    N_assert (((e + seg.mBegin) * 2) < 65536, "Too many elements!");
                    Nui16 baseIdx = static_cast<Nui16>((e + seg.mBegin) * 2);

                    // Determine base pointer to vertex #1
                    void * pBase = (void *)(
                        static_cast<NIIb *>(pBufferStart) +
                            pBuffer->getUnitSize() * baseIdx);

                    // Get index of next item
                    NCount nexte = e + 1;
                    if (nexte == mPixelCount)
                        nexte = 0;

                    if (e == seg.mStart)
                    {
                        // No laste, use next item
                        chainTangent = mPixelList[nexte + seg.mBegin].mPos - elem.mPos;
                    }
                    else if (e == seg.mEnd)
                    {
                        // No nexte, use only last item
                        chainTangent = elem.mPos - mPixelList[laste + seg.mBegin].mPos;
                    }
                    else
                    {
                        // A mid mPos, use tangent across both prev and next
                        chainTangent = mPixelList[nexte + seg.mBegin].mPos - mPixelList[laste + seg.mBegin].mPos;

                    }

                    Vector3f vP1ToEye;

                    if(mFaceCamera)
                        vP1ToEye = eyePos - elem.mPos;
                    else
                        vP1ToEye = elem.mOri * mOriBase;
                    Vector3f vPerpendicular = chainTangent.crossProduct(vP1ToEye);
                    vPerpendicular.normalise();
                    vPerpendicular *= (elem.mDim * 0.5f);

                    Vector3f pos0 = elem.mPos - vPerpendicular;
                    Vector3f pos1 = elem.mPos + vPerpendicular;

                    NIIf * pFloat = static_cast<NIIf *>(pBase);
                    // pos1
                    *pFloat++ = pos0.x;
                    *pFloat++ = pos0.y;
                    *pFloat++ = pos0.z;

                    pBase = static_cast<void *>(pFloat);

                    if(mTexColour)
                    {
                        RGBA * pCol = static_cast<RGBA *>(pBase);
                        RenderSys * rs = N_Engine().getRender();
                        rs->convertType(elem.mColour, pCol);
                        pCol++;
                        pBase = static_cast<void *>(pCol);
                    }

                    if(mTexCoord)
                    {
                        pFloat = static_cast<NIIf *>(pBase);
                        if(mTexCoordU == true)
                        {
                            *pFloat++ = elem.mCoord;
                            *pFloat++ = mTexCoordMin;
                        }
                        else
                        {
                            *pFloat++ = mTexCoordMin;
                            *pFloat++ = elem.mCoord;
                        }
                        pBase = static_cast<void*>(pFloat);
                    }

                    // pos2
                    pFloat = static_cast<NIIf *>(pBase);
                    *pFloat++ = pos1.x;
                    *pFloat++ = pos1.y;
                    *pFloat++ = pos1.z;
                    pBase = static_cast<void*>(pFloat);

                    if(mTexColour)
                    {
                        RGBA* pCol = static_cast<RGBA*>(pBase);
                        RenderSys * rs = N_Engine().getRender();
                        rs->convertType(elem.mColour, pCol);
                        pCol++;
                        pBase = static_cast<void*>(pCol);
                    }

                    if(mTexCoord)
                    {
                        pFloat = static_cast<NIIf*>(pBase);
                        if(mTexCoordU == true)
                        {
                            *pFloat++ = elem.mCoord;
                            *pFloat++ = mTexCoordMax;
                        }
                        else
                        {
                            *pFloat++ = mTexCoordMax;
                            *pFloat++ = elem.mCoord;
                        }
                    }

                    if (e == seg.mEnd)
                        break; // last one

                    laste = e;

                } // element
            } // segment valid?

        } // each segment

        pBuffer->unlock();
    }
    //-----------------------------------------------------------------------
    NIIf PixelChain::distanceSquared(const Camera * cam) const
    {
        Vector3f min, max, mid, dist;
        min = mAABB.getMin();
        max = mAABB.getMax();
        mid = ((max - min) * 0.5) + min;
        dist = cam->getSpacePos() - mid;

        return dist.lengthSquared();
    }
    //-----------------------------------------------------------------------
    NIIf PixelChain::getCenterRange() const
    {
        return mRadius;
    }
    //-----------------------------------------------------------------------
    const AABox & PixelChain::getAABB() const
    {
        if(!mAABBValid)
        {
            mAABB.setNull();
            Vector3f widthVector;
            for(ChainList::const_iterator segi = mChainList.begin();
                segi != mChainList.end(); ++segi)
            {
                const Chain& seg = *segi;

                if(seg.mStart != N_Max_Num_Value)
                {

                    for(NCount e = seg.mStart; ; ++e) // until break
                    {
                        // Wrap forwards
                        if (e == mPixelCount)
                            e = 0;

                        const Pixel & elem = mPixelList[seg.mBegin + e];

                        widthVector.x = widthVector.y = widthVector.z = elem.mDim;
                        mAABB.merge(elem.mPos - widthVector);
                        mAABB.merge(elem.mPos + widthVector);

                        if (e == seg.mEnd)
                            break;
                    }
                }
            }

            // Set the current radius
            if(mAABB.isNull())
            {
                mRadius = 0.0f;
            }
            else
            {
                mRadius = Math::sqrt(std::max(mAABB.getMin().lengthSquared(),
                    mAABB.getMax().lengthSquared()));
            }

            mAABBValid = true;
        }
        return mAABB;
    }
    //-----------------------------------------------------------------------
    const Material * PixelChain::getMaterial() const
    {
        return mMaterial;
    }
    //-----------------------------------------------------------------------
    void PixelChain::setMaterial(ResourceID rid, GroupID gid)
    {
        if(mMaterialID != rid)
        {
            mMaterialID = rid;
            if(mMaterial != 0)
                mMaterial->unref();

            mMaterial = static_cast<Material *>(N_Only(Material).get(mMaterialID, gid));
            if(mMaterial == 0)
            {
                N_Only(Log).log(_I18n("不能分配材质ID: ") + N_conv(rid) +
                    _I18n(" to PixelChain ") + N_conv(mID) +
                    _I18n(" because this Material does not exist. Have you forgotten to define it in a .material script?"));
                mMaterial = const_cast<Material *>(MaterialManager::WhiteColour);
                if(mMaterial == 0)
                {
                    N_EXCEPT(Internal, _I18n("Can't assign default material to PixelChain of ") +
                        N_conv(mID) + _I18n(". Did you forget to call MaterialManager::init()?"));
                }
                mMaterial->ref(false);
            }
            // Ensure new material loaded (will not load again if already loaded)
            mMaterial->load();
        }
    }
    //-----------------------------------------------------------------------
    ResourceID PixelChain::getMaterialID() const
    {
        return mMaterialID;
    }
    //-----------------------------------------------------------------------
    FactoryID PixelChain::getFactoryID() const
    {
        return N_FACTORY_PixelChain;
    }
    //-----------------------------------------------------------------------
    void PixelChain::queue(RenderQueue * queue)
    {
        genGeoBuffer();
        if(!mIndexValid)
        {
            Nui16 * pShort = static_cast<Nui16 *>(mIndexData->mBuffer->lock(Buffer::MM_WHOLE));
            mIndexData->mCount = 0;

            for(ChainList::iterator segi = mChainList.begin();
                segi != mChainList.end(); ++segi)
            {
                Chain & seg = *segi;

                // Skip 0 or 1 element segment counts
                if (seg.mStart != N_Max_Num_Value && seg.mStart != seg.mEnd)
                {
                    // Start from mStart + 1 since it's only useful in pairs
                    NCount laste = seg.mStart;
                    while(1) // until break
                    {
                        NCount e = laste + 1;
                        // Wrap forwards
                        if(e == mPixelCount)
                            e = 0;
                        // indexes of this element are (e * 2) and (e * 2) + 1
                        // indexes of the last element are the same, -2
                        N_assert (((e + seg.mBegin) * 2) < 65536, "Too many elements!");
                        Nui16 baseIdx = static_cast<Nui16>((e + seg.mBegin) * 2);
                        Nui16 lastBaseIdx = static_cast<Nui16>((laste + seg.mBegin) * 2);
                        *pShort++ = lastBaseIdx;
                        *pShort++ = lastBaseIdx + 1;
                        *pShort++ = baseIdx;
                        *pShort++ = lastBaseIdx + 1;
                        *pShort++ = baseIdx + 1;
                        *pShort++ = baseIdx;

                        mIndexData->mCount += 6;


                        if (e == seg.mEnd)
                            break; // last one

                        laste = e;
                    }
                }
            }
            mIndexData->mBuffer->unlock();

            mIndexValid = true;
        }

        if(mIndexData->mCount > 0)
        {
            if(mRenderLevel != 0)
                queue->add(this, mRenderGroup, mRenderLevel);
            else if(mRenderGroup != RQG_Unknow)
                queue->add(this, mRenderGroup);
            else
                queue->add(this);
        }
    }
    //-----------------------------------------------------------------------
    void PixelChain::getGeometry(GeometryRaw & op) const
    {
        op.mIndex = mIndexData;
        op.mVertex = mVertexData;
        op.mType = GeometryRaw::OT_TRIANGLE_LIST;
        op.add(GRT_Index);
    }
    //-----------------------------------------------------------------------
    void PixelChain::getMatrix(Matrix4f * out, NCount & count) const
    {
        *out = getSpaceMatrix();
        count = 1;
    }
    //-----------------------------------------------------------------------
    void PixelChain::getLight(LightList & ll) const
    {
        query(ll);
    }
    //---------------------------------------------------------------------
    void PixelChain::query(GeometryQuery * query)
    {
        // only one renderable
        query->query(this);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PixelChainFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FactoryID PixelChainFactory::getID() const
    {
        return N_FACTORY_PixelChain;
    }
    //-----------------------------------------------------------------------
    SpaceObj * PixelChainFactory::create(SpaceID sid, SpaceManager * mag,
        const PropertyData * params)
    {
        NCount maxElements;
        NCount numberOfChains;
        bool useTex;
        bool useCol;
        if(params != 0)
        {
            params->get(N_PropertyPixelChain_MaxElement, maxElements, 20);
            params->get(N_PropertyPixelChain_NumOfChain, numberOfChains, 1);
            params->get(N_PropertyPixelChain_UseTexCoord, useTex, true);
            params->get(N_PropertyPixelChain_UseVerColour, useCol, true);
        }

        PixelChain * re = N_new PixelChain(sid, maxElements, numberOfChains, useTex, useCol);
        re->setFactory(this);
        re->setManager(mag);
        return re;
    }
    //-----------------------------------------------------------------------
    void PixelChainFactory::destroy(void * obj)
    {
        PixelChain * temp = static_cast<PixelChain *>(obj);
        N_delete temp;
    }
    //------------------------------------------------------------------------
}