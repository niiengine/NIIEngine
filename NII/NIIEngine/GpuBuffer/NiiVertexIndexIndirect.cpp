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
#include "NiiVertexIndexIndirect.h"
#include "NiiGBufferManager.h"
#include "NiiEngine.h"

#define T_SHARE_BUFFER      0x01
#define T_N_SHARE_BUFFER    0xFE
#define T_POS               0x02
#define T_N_POS             0xFD
#define T_NRLS              0x04
#define T_N_NRLS            0xFB

namespace NII
{
    //-----------------------------------------------------------------------
    class Triangle
    {
    public:
        enum EdgeMatchType
        {
            AB, BC, CA, ANY, NONE
        };

        Nui32 a, b, c;

        inline Triangle() {}

        inline Triangle(Nui32 ta, Nui32 tb, Nui32 tc) : a(ta), b(tb), c(tc) {}

        inline Triangle(Nui32 t[3]) : a(t[0]), b(t[1]), c(t[2]) {}

        inline Triangle(const Triangle & t) : a(t.a), b(t.b), c(t.c) {}
        // 共边判断
        inline bool sharesEdge(const Triangle & t) const
        {
            return(
                (a == t.a && b == t.c) || (a == t.b && b == t.a) || (a == t.c && b == t.b) ||
                (b == t.a && c == t.c) || (b == t.b && c == t.a) || (b == t.c && c == t.b) ||
                (c == t.a && a == t.c) || (c == t.b && a == t.a) || (c == t.c && a == t.b)
                );
        }

        inline bool sharesEdge(const Nui32 ea, const Nui32 eb, const Triangle& t) const
        {
            return((ea == t.a && eb == t.c) || (ea == t.b && eb == t.a) ||
                (ea == t.c && eb == t.b));
        }

        inline bool sharesEdge(const EdgeMatchType edge, const Triangle& t) const
        {
            if (edge == AB)
                return sharesEdge(a, b, t);
            else if (edge == BC)
                return sharesEdge(b, c, t);
            else if (edge == CA)
                return sharesEdge(c, a, t);
            else
                return (edge == ANY) == sharesEdge(t);
        }
        // 返回是哪条边共享了
        inline EdgeMatchType endoSharedEdge(const Triangle & t) const
        {
            if (sharesEdge(a, b, t)) return AB;
            if (sharesEdge(b, c, t)) return BC;
            if (sharesEdge(c, a, t)) return CA;
            return NONE;
        }

        inline EdgeMatchType exoSharedEdge(const Triangle & t) const
        {
            return t.endoSharedEdge(*this);
        }
        //倒转时针
        inline void shiftClockwise()
        {
            Nui32 t = a;
            a = c;
            c = b;
            b = t;
        }
        //逆时针
        inline void shiftCounterClockwise()
        {
            Nui32 t = a;
            a = b;
            b = c;
            c = t;
        }
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // IndexData
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    IndexData::IndexData() :
        mCount(0),
        mOffset(0),
        mBuffer(0),
        mAutoDestroy(true)
    {
    }
    //-----------------------------------------------------------------------
    IndexData::~IndexData()
    {
        if (mBuffer != 0)
        {
            if (mAutoDestroy)
            {
                N_delete mBuffer;
            }
            else
            {
                mBuffer->unref();
            }
            mBuffer = 0;
        }
    }
    //-----------------------------------------------------------------------
    IndexData * IndexData::clone(CopyType ct, BufferModeMark mode, GBufferManager * mag) const
    {
        if(!mag)
            mag = GBufferManager::getOnlyPtr();
        
        IndexData * dest = N_new IndexData();
        if (mBuffer != 0)
        {
            if (ct == CT_Clone)
            {
                mode = mode < 0 ? mBuffer->getModeMark() : mode;

                const void * shadowBuff = mBuffer->getHostData();
                if(shadowBuff)
                {
                    void * dstData;
                    if(mode & M_HostCache)
                    {
                        dstData = N_alloc( mBuffer->getSize());
                        memcpy( dstData, shadowBuff, mBuffer->getSize() );
                    }
                    else
                    {
                        dstData = shadowBuff;
                    }
                    
                    mag->create(dest->mBuffer, mBuffer->getIndexType(), mBuffer->getUnitCount(), mBuffer->getMultiCount(), mode, dstData, mBuffer->getGroupID());
                }
                else
                {
                    if(mode & M_HostCache)
                    {
                        GpuBufferAsync * bufAsync;
                        N_Only(GBuffer).create(bufAsync, mBuffer, 0, mBuffer->getSize());
                        
                        const void * srcData = bufAsync->map();
                        void * dstData = N_alloc( mBuffer->getSize());
                        memcpy( dstData, srcData, mBuffer->getSize() );
                        bufAsync->unmap();
                    
                        mag->create(dest->mBuffer, mBuffer->getIndexType(), mBuffer->getUnitCount(), mBuffer->getMultiCount(), mode, dstData, mBuffer->getGroupID());
                    }
                    else
                    {
                        mag->create(dest->mBuffer, mBuffer->getIndexType(), mBuffer->getUnitCount(), mBuffer->getMultiCount(), mode, 0, mBuffer->getGroupID());
                        dest->mBuffer->write(mBuffer, 0, 0, mBuffer->getSize());
                    
                    }
                }
                dest->mBuffer->ref();
            }
            else if(ct == CT_Ref)
            {
                dest->mBuffer = mBuffer;
                dest->mAutoDestroy = false;
                dest->mBuffer->ref();
            }
            else
            {
                dest->mBuffer = 0;
                mAutoDestroy = true;
            }
        }
        dest->mCount = mCount;
        dest->mOffset = mOffset;
        return dest;
    }
    //-----------------------------------------------------------------------
    void IndexData::attach(IndexBuffer * buf)
    {
        if (mBuffer)
            mBuffer->unref();

        mBuffer = buf;

        if (buf)
            buf->ref();
    }
    //-----------------------------------------------------------------------
    void IndexData::detach()
    {
        if (mBuffer)
        {
            mBuffer->unref();
            mBuffer = 0;
        }
    }
    //-----------------------------------------------------------------------
    void IndexData::opt()
    {
        if(mBuffer->isMapping())
            return;

        void * buffer = mBuffer->lock(Buffer::MM_READ | Buffer::MM_WRITE);

        Triangle * triangles;
        Nui32 * dest;

        NCount nIndexes = mCount;
        NCount nTriangles = nIndexes / 3;
        NCount i, j;
        Nui16 * source = 0;
        // 一个三角形三个点,等于一个索引
        if (mBuffer->getUnitSize() == 16)
        {
            triangles = N_alloc_t(Triangle, nTriangles);
            source = (Nui16 *)buffer;
            // dest使用triangles的内存a b c点内存
            dest = (Nui32 *)triangles;
            // 分配映射
            for (i = 0; i < nIndexes; ++i)
                dest[i] = source[i];
        }
        else
            triangles = (Triangle *)buffer;

        // 基于共享边缘排序三角形
        Nui32 * destlist = N_alloc_t(Nui32, nTriangles);
        Nui8 * visited = N_alloc_t(Nui8, nTriangles);

        for (i = 0; i < nTriangles; ++i)
            visited[i] = 0;

        Nui32 start = 0, ti = 0, destcount = 0;

        bool found = false;
        // 遍历所有三角形
        for (i = 0; i < nTriangles; ++i)
        {
            if (found)
                found = false;
            else
            {   // 在到没有访问的一个,指向它
                while (visited[start++]);
                ti = start - 1;
            }
            //记录下ti到目标的列表
            destlist[destcount++] = ti;
            //ti个查看过1次
            visited[ti] = 1;

            // 从第start开始,不重复之前的
            for (j = start; j < nTriangles; ++j)
            {   // 第j个已经查看过
                if (visited[j])
                    continue;
                // 检测ti个与第j个是否共边
                if (triangles[ti].sharesEdge(triangles[j]))
                {
                    found = true;
                    ti = static_cast<Nui32>(j);
                    break;
                }
            }
        }
        // destlist为相邻三角形不重复列表
        if (mBuffer->getUnitSize() == 16)
        {
            j = 0;
            for (i = 0; i < nTriangles; ++i)
            {   // 一个三角行有3个角索引
                Triangle * t = &triangles[destlist[i]];
                // 重新排列indexbuffer
                if (source)
                {
                    source[j++] = (Nui16)t->a;
                    source[j++] = (Nui16)t->b;
                    source[j++] = (Nui16)t->c;
                }
            }
            N_free(triangles);
        }
        else
        {
            Nui32 * reflist = N_alloc_t(Nui32, nTriangles);

            // 填补 referencebuffer,目标索引到序列索引映射
            for (i = 0; i < nTriangles; ++i)
            {
                //     三角形具体位置                     序列
                reflist[destlist[i]] = static_cast<Nui32>(i);
            }

            // 重新排列 indexbuffer
            for (i = 0; i < nTriangles; ++i)
            {
                j = destlist[i];
                if (i == j)
                    continue; // 不需移动三角形

                // 交换三角形,
                Triangle t = triangles[i];
                triangles[i] = triangles[j];
                triangles[j] = t;

                /* 由于上面操作,目标索引发生的位置改变了reflist[i]是目标索引到序列索引映射
                    destlist[reflist[i]]序列索引到目标索引映射目标索引 由i改变成了j
                */
                destlist[reflist[i]] = static_cast<Nui32>(j);
            }

            N_free(reflist);
        }

        N_free(destlist);
        N_free(visited);

        mBuffer->unlock();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // VertexUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    VertexUnit::VertexUnit():
        mDivisor(1)
    {
    }
    //------------------------------------------------------------------------
    VertexUnit::VertexUnit(DataType dtype, VertexType vtype, Nui16 source, NCount offset) :
        mType(dtype),
        mVType(vtype),
        mSource(source),
        mOffset(offset),
        mDivisor(1),
        mIndex(0)
    {
    }
    //------------------------------------------------------------------------
    VertexUnit::VertexUnit(DataType dtype, VertexType vtype, Nui16 source, NCount offset,
        NCount divisor, Nui16 index) :
        mType(dtype),
        mVType(vtype),
        mSource(source),
        mOffset(offset),
        mDivisor(divisor),
        mIndex(index)
    {
    }
    //------------------------------------------------------------------------
    VertexUnit::VertexUnit(DataType dtype, VertexType vtype, Nui16 source, NCount offset,
        Nui16 index) :
            mType(dtype),
            mVType(vtype),
            mSource(source),
            mOffset(offset),
            mDivisor(1),
            mIndex(index)
    {
    }
    //------------------------------------------------------------------------
    bool VertexUnit::operator== (const VertexUnit & o) const
    {
        if(mType != o.mType || mIndex != o.mIndex || mOffset != o.mOffset ||
            mVType != o.mVType || mDivisor != o.mDivisor ||  mSource != o.mSource)
            return false;
        else
            return true;
    }
    //------------------------------------------------------------------------
    void VertexUnit::conv(VertexData * obj, DataType src, DataType dst)
    {
        if(dst != DT_Colour_ABGR && dst != DT_Colour_ARGB)
        {
            return;
        }
        if(src != DT_Colour_ABGR && src != DT_Colour_ARGB)
        {
            return;
        }
        bool conversionNeeded = false;
        VertexData::BindingList::const_iterator bindi, bindiend = obj->getAttachs().end();
        for(bindi = obj->getAttachs().begin(); bindi != bindiend; ++bindi)
        {
            VertexData::Units elems;
            obj->getUnits(bindi->first, elems);
            VertexData::Units::iterator elemi, elemiend = elems.end();
            for(elemi = elems.begin(); elemi != elemiend; ++elemi)
            {
                const VertexUnit & e = *elemi;
                if(e.mType == DT_Colour || ((e.mType == DT_Colour_ABGR ||
                    e.mType == DT_Colour_ARGB) && e.mType != dst))
                {
                    conversionNeeded = true;
                }
            }
            // 缓存是一起上锁的
            if(conversionNeeded)
            {
                Nui8 * pBase = (Nui8 *)bindi->second->lock(Buffer::MM_READ | Buffer::MM_WRITE);
                for(NCount v = 0; v < bindi->second->getUnitCount(); ++v)
                {
                    elemiend = elems.end();
                    for(elemi = elems.begin(); elemi != elemiend; ++elemi)
                    {
                        const VertexUnit & e = *elemi;
                        DataType currType = (e.mType == DT_Colour) ? src : e.mType;
                        if((e.mType == DT_Colour || ((e.mType == DT_Colour_ABGR ||
                            e.mType == DT_Colour_ARGB) && e.mType != dst)) && (currType != dst))
                        {
                            Nui32 * pRGBA;
                            e.getData(pBase, pRGBA);
                            // 转换 ARGB 和 ABGR, 通常是翻转 R/B
                            *pRGBA = ((*pRGBA & 0x00FF0000) >> 16)|((*pRGBA & 0x000000FF) << 16)|(*pRGBA & 0xFF00FF00);
                        }
                    }
                    pBase = pBase + bindi->second->getUnitSize();
                }
                bindi->second->unlock();
            }
        } // 每个缓存
        if(conversionNeeded)
        {
            // 遍历所有声明的元素,修改元素,以反映变化了的类型
            NCount i, iend = obj->getCount();
            for (i = 0; i < iend; ++i)
            {
                VertexUnit * temp = obj->get(i);
                DataType e = temp->mType;
                if (e == DT_Colour || ((e == DT_Colour_ABGR || e == DT_Colour_ARGB) && e != dst))
                {
                    temp->mType = dst;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // HardwareAnimation
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    HardwareAnimation::HardwareAnimation()
    {
        mFusionIndex = 0;
    }
    //------------------------------------------------------------------------
    HardwareAnimation::~HardwareAnimation()
    {
    }
    //------------------------------------------------------------------------
    NCount HardwareAnimation::alloc(VertexData * vd, NCount cnt, bool nrls)
    {
        Nui16 texCoord = vd->getCount(VT_Tex_Coord);
        NCount freeCount = NII_MAX_TextureCoord - texCoord;
        if (nrls)
        {
            // we need 2x the texture coords, round down
            freeCount /= 2;
        }

        NCount hwacount = std::min(freeCount, cnt);
        for (NCount c = mFusionData.size(); c < hwacount; ++c)
        {
            Fusion data;
            data.mBufferIndex = vd->getFreeAttachIndex();
            vd->add(DT_Float3x, VT_Tex_Coord, data.mBufferIndex, 0, texCoord++);
            if (nrls)
            {
                vd->add(DT_Float3x, VT_Tex_Coord, data.mBufferIndex, sizeof(NIIf) * 3, texCoord++);
            }
            mFusionData.push_back(data);
            // 顶点缓存不会受到限制,当它合适时(例如通过一个VertexKeyFrameTrack）,认为由调用者完成
        }
        return hwacount;
    }
    //------------------------------------------------------------------------
    void HardwareAnimation::free(VertexData * dst, NCount cnt, bool nrls)
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // VertexData
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool __VertexDataLess(const VertexUnit & e1, const VertexUnit & e2)
    {
        if(e1.mSource < e2.mSource)
        {
            return true;
        }
        else if(e1.mSource == e2.mSource)
        {
            if(e1.mVType < e2.mVType)
            {
                return true;
            }
            else if(e1.mVType == e2.mVType)
            {
                if(e1.mIndex < e2.mIndex)
                {
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    GeoOperationList VertexData::mGeoLayoutList;
    //------------------------------------------------------------------------
    VertexData::VertexData(GBufferManager * mag) :
        mHighIndex(0),
        mBufferCount(0),
        mCount(0),
        mExtData(0),
        mOffset(0)
    {
        mMag = mag ? mag : N_OnlyPtr(GBuffer);
        mHWAni = N_new HardwareAnimation();
    }
    //------------------------------------------------------------------------
    VertexData::~VertexData()
    {
        if(mExtData)
        {
            mExtData->unref();
            mExtData = 0;
        }
        mUnitList.clear();
        detachAll();
        N_delete mHWAni;
    }
    //------------------------------------------------------------------------
    bool VertexData::operator== (const VertexData & o) const
    {
        if(mUnitList.size() != o.mUnitList.size())
            return false;

        Units::const_iterator i, iend = mUnitList.end();
        Units::const_iterator oi, oiend = o.mUnitList.end();
        for(i = mUnitList.begin(), oi = o.mUnitList.begin(); i != iend && oi != oiend; ++i, ++oi)
        {
            if(!(*i == *oi))
                return false;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool VertexData::operator!= (const VertexData & o) const
    {
        return !(*this == o);
    }
    //------------------------------------------------------------------------
    const VertexUnit * VertexData::add(DataType dt, VertexType vt, Nui16 bidx, NCount oft, Nui16 eidx)
    {
        if(dt == DT_Colour)
        {
            dt = N_Engine().getColourFormat();
        }

        mUnitList.push_back(VertexUnit(dt, vt, bidx, oft, eidx));

        if (bidx > mBufferCount)
        {
            mBufferCount = bidx + 1;
        }

        return &(mUnitList.back());
    }
    //------------------------------------------------------------------------
    const VertexUnit * VertexData::insert(Nidx pos, DataType dt, VertexType vt,
        Nui16 bidx, NCount oft, Nui16 eidx)
    {
        if(pos >= mUnitList.size())
        {
            return add(dt, vt, bidx, oft, eidx);
        }

        Units::iterator i = mUnitList.begin();
        for(Nidx n = 0; n < pos; ++n)
            ++i;
        i = mUnitList.insert(i, VertexUnit(dt, vt, bidx, oft, eidx));

        return &*i;
    }
    //------------------------------------------------------------------------
    void VertexData::modify(Nidx pos, DataType dt, VertexType vt, Nui16 bidx,
        NCount oft, Nui16 eidx)
    {
        N_assert(pos < mUnitList.size(), "Index out of bounds");
        Units::iterator i = mUnitList.begin();
        std::advance(i, pos);

        (*i) = VertexUnit(dt, vt, bidx, oft, eidx);
    }
    //------------------------------------------------------------------------
    VertexUnit * VertexData::get(Nidx index) const
    {
        N_assert(index < mUnitList.size(), "Index out of bounds");
        Units::iterator i = mUnitList.begin();
        std::advance(i, index);
        return &(*i);
    }
    //------------------------------------------------------------------------
    VertexUnit * VertexData::get(VertexType vt, Nui16 eidx) const
    {
        Units::iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            if(i->mVType == vt && i->mIndex == eidx)
            {
                return &(*i);
            }
        }
        return NULL;
    }
    //------------------------------------------------------------------------
    VertexUnit * VertexData::getIndex(VertexType vt, Nidx & globalidx, Nui16 eidx) const
    {
        Units::iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            if(i->mVType == vt && i->mIndex == eidx)
            {
                globalIdx = i - mUnitList.begin();
                return &(*i);
            }
        }
        globalIdx = -1;
        return NULL;
    }
    //------------------------------------------------------------------------
    void VertexData::remove(Nidx idx)
    {
        N_assert(idx < mUnitList.size(), "Index out of bounds");
        Units::iterator i = mUnitList.begin();
        std::advance(i, idx);
        mUnitList.erase(i);
    }
    //------------------------------------------------------------------------
    void VertexData::remove(VertexType vt, Nui16 eidx)
    {
        Units::iterator ei, eiend = mUnitList.end();
        for(ei = mUnitList.begin(); ei != eiend; ++ei)
        {
            if(ei->mVType == vt && ei->mIndex == eidx)
            {
                mUnitList.erase(ei);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void VertexData::removeAll()
    {
        mUnitList.clear();
    }
    //------------------------------------------------------------------------
    NCount16 VertexData::getCount(VertexType vt) const
    {
        NCount16 coord = 0;
        Units::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            if((*i).mVType == vt)
            {
                ++coord;
            }
        }
        return coord;
    }
    //------------------------------------------------------------------------
    void VertexData::getUnits(Nui16 source, VertexData::Units & out) const
    {
        Units::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            if(i->mSource == source)
            {
                out.push_back(*i);
            }
        }
    }
    //------------------------------------------------------------------------
    void VertexData::getUnits(UnitsMap & out) const
    {
        NCount cnt = 0;
        out.resize( mBindingList.size() );
        VertexData::BindingList::const_iterator i, iend = mBindingList.end();
        for(i = mBindingList.begin(); i != iend; ++i, ++cnt)
        {
            UnitsMap::iterator it = out.begin();
            std::advance(it, cnt);
            getUnits(i->first, it->second);
        }
    }
    //------------------------------------------------------------------------
    void VertexData::attach(const BindingList & list)
    {
        BindingList::const_iterator i, iend = list.end();
        for (i = list.begin(); i != iend; ++i)
        {
            attach(i->first, i->second);
        }
    }
    //------------------------------------------------------------------------
    void VertexData::attach(Nui16 sidx, VertexBuffer * buffer)
    {
        BindingList::iterator i = mBindingList.find(sidx);
        if(i != mBindingList.end())
        {
            i->second->unref();
            i->second = buffer;
        }
        else
        {
            mBindingList.insert(Npair(sidx, buffer));
            mHighIndex = N_MAX(Nui16, mHighIndex, sidx + 1);
        }
        buffer->ref();
    }
    //------------------------------------------------------------------------
    void VertexData::detach(Nui16 sidx)
    {
        BindingList::iterator i = mBindingList.find(sidx);
        if(i != mBindingList.end())
        {
            i->second->unref();
            mBindingList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    bool VertexData::isAttach(Nui16 sidx) const
    {
        return mBindingList.find(sidx) != mBindingList.end();
    }
    //------------------------------------------------------------------------
    VertexBuffer * VertexData::getAttach(Nui16 sidx) const
    {
        BindingList::const_iterator i = mBindingList.find(sidx);
        if(i == mBindingList.end())
        {
            N_EXCEPT(NotExist, _I18n("指定索引不存在缓存"));
        }
        return i->second;
    }
    //------------------------------------------------------------------------
    void VertexData::detachAll()
    {
        BindingList::iterator i, iend = mBindingList.end();
        for(i = mBindingList.begin(); i != iend; ++i)
        {
            i->second->unref();
        }
        mBindingList.clear();
        mHighIndex = 0;
    }
    //------------------------------------------------------------------------
    void VertexData::optValidAttach()
    {
        set<Nui16>::type usedBuffers;

        Units::const_iterator ai, aiend = mUnitList.end();
        for(ai = mUnitList.begin(); ai != aiend; ++ai)
        {
            const VertexUnit & elem = *ai;
            usedBuffers.insert(elem.mSource);
        }

        // 复位未使用缓存绑定
        Nidx count = mBindingList.empty() ? 0 : mBindingList.rbegin()->first + 1;
        for(Nui16 index = 0; index < count; ++index)
        {
            if(usedBuffers.find(index) == usedBuffers.end() && isAttach(index))
            {
                detach(index);
            }
        }
        opt();
    }
    //------------------------------------------------------------------------
    NCount VertexData::getUnitSize(Nui16 source) const
    {
        NCount sz = 0;
        Units::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            if(i->mSource == source)
            {
                sz += i->getSize();
            }
        }
        return sz;
    }
    //------------------------------------------------------------------------
    NCount VertexData::getUnitSize(const Units & lt)
    {
        NCount sz = 0;
        Units::const_iterator i, iend = lt.end();
        for(i = lt.begin(); i != iend; ++i)
        {
            sz += i->getSize();
        }
        return sz;
    }
    //-----------------------------------------------------------------------------------
    Nidx VertexData::getVaoType(const GeoOperation & geoOp)
    {
        GeoOperationList::const_iterator itor = mGeoLayoutList.begin();
        GeoOperationList::const_iterator end  = mGeoLayoutList.end();

        while( itor != end && (itor->mUnitsList != unitslist || itor->mType != geoOp.first) )
        {
            ++itor;
        }

        if( itor == end )
        {
            N_assert(mGeoLayoutList.size() < (1u << 10u), _T("Too many input layouts!!!"));

            mGeoLayoutList.push_back(InputLayouts());
            mGeoLayoutList.back().mType = type;
            mGeoLayoutList.back().mUnitsList = unitslist;
            itor = mGeoLayoutList.end() - 1u;
        }

        //Store the idx in the first 10 bits, store the operation type in the last 6.
        uint16 retVal = static_cast<uint16>( itor - mGeoLayoutList.begin() );
        retVal |= type << 10u;

        return retVal;
    }
    //------------------------------------------------------------------------
    void VertexData::calcAABB(const Vector3f & pos, const Quaternion & rot, const Vector3f & scale, AABox & out)
    {
        const VertexUnit * com = get(VT_Pos);

        VertexBuffer * vbuf = getAttach(com->mSource);

        Nui8 * vertex = (Nui8 *)(vbuf->lock(Buffer::MM_READ));
        NIIf * pFloat;

        Vector3f min = Vector3f::ZERO, max = Vector3f::UNIT;
        bool first = true;

        for (NCount j = 0; j < mCount; ++j, vertex += vbuf->getUnitSize())
        {
            com->getData(vertex, pFloat);

            Vector3f pt;

            pt.x = (*pFloat++);
            pt.y = (*pFloat++);
            pt.z = (*pFloat++);
            pt = (rot * (pt * scale)) + pos;
            if (first)
            {
                min = max = pt;
                first = false;
            }
            else
            {
                min = min.floor(pt);
                max = max.ceil(pt);
            }

        }
        vbuf->unlock();
        out.setMin(min);
        out.setMax(max);
    }
    //------------------------------------------------------------------------
    void VertexData::copy(const VertexData * src) const
    {
        mUnitList.clear();
        Units::const_iterator i, iend = src->mUnitList.end();
        for(i = src->mUnitList.begin(); i != iend; ++i)
        {
            mUnitList.push_back(*i);
        }
    }
    //------------------------------------------------------------------------
    void VertexData::sort()
    {
        mUnitList.sort(__VertexDataLess);
    }
    //------------------------------------------------------------------------
    void VertexData::optUnit()
    {
        mUnitList.sort(__VertexDataLess);
        /*  1+1+1+2+2+2+2+3+4+5+6+7+10+10+11+11+12+13
                                    -- -- -- -- -- --
                                    8  8  9  9  10 11
        */
        Nui16 tidx = 0;
        Units::iterator i = mUnitList.begin() , iend = mUnitList.end();
        Nui16 lidx = (*i).mSource;
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            VertexUnit & e = *i;
            if(lidx != e.mSource)
            {
                ++tidx; // 当它加一时候就说明是不同一个源
                lidx = e.mSource;
            }

            if(tidx != e.mSource)
            {
                i->mSource = tidx;
            }
        }
        mBufferCount = tidx + 1;
    }
    //------------------------------------------------------------------------
    void VertexData::setExt(VertexBuffer * vb)
    {
        if(mExtData)
            mExtData->unref();
        mExtData = vb;
        mExtData->ref();
    }
    //------------------------------------------------------------------------
    void VertexData::build(Nmark mode, bool sync)
    {
        bool ani_ske = mode & M_Skeleton;
        bool ani = mode & M_Animation;
        bool ani_nrls = mode & M_Normals;
        mMark = mode;
        Nui16 buffer = 0;
        NCount offset = 0;
        VertexType prevSemantic = VT_Pos;

        Units newEle;
        Units::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            DataType type = i->mType;
            if(type == DT_Colour)
            {
                type = N_Engine().getColourFormat();
            }
            newEle.push_back(VertexUnit(type, i->mVType, 0, 0, i->mIndex));
        }
        newEle.sort(__VertexDataLess);

        Units::iterator j, jend = newEle.end();
        for(j = newEle.begin(); j != jend; ++j)
        {
            const VertexUnit & elem = *j;

            bool splitPre = false;
            bool splitNext = false;
            switch(elem.mVType)
            {
            case VT_Pos:
                // Split positions if vertex animated with only positions
                // group with normals otherwise
                splitPre = false;
                splitNext = ani && !ani_nrls;
                break;
            case VT_Normals:
                // 法线不能与 weights/indices 混合
                splitPre = (prevSemantic == VT_Matrix_Weight || prevSemantic == VT_Matrix_Index);
                // 所有动画网格不得不在法线后分割
                splitNext = (ani_ske || (ani && ani_nrls));
                break;
            case VT_Matrix_Weight:
                // blend weights/indices 仅可与它们的缓存共享
                splitPre = true;
                break;
            case VT_Matrix_Index:
                // blend weights/indices 仅可与它们的缓存共享
                splitNext = true;
                break;
            default:
            case VT_Diffuse:
            case VT_Specular:
            case VT_Tex_Coord:
            case VT_Binormal:
            case VT_Tangent:
                // Make sure position is separate if animated & there were no normals
                splitPre = prevSemantic == VT_Pos && (ani_ske || ani);
                break;
            }

            if(splitPre && offset)
            {
                ++buffer;
                offset = 0;
            }

            prevSemantic = elem.mVType;
            j->mSource = buffer;
            j->mOffset = offset;

            if(splitNext)
            {
                ++buffer;
                offset = 0;
            }
            else
            {
                offset += elem.getSize();
            }
        }
        newEle.sort(__VertexDataLess);
        /*  1+1+1+2+2+2+2+3+4+5+6+7+10+10+11+11+12+13
                                    -- -- -- -- -- --
                                    8  8  9  9  10 11
        */
        Nui16 lidx = (*i).mSource;
        for(buffer = 0, j = newEle.begin(); j != jend; ++j)
        {
            VertexUnit & e = *j;
            if(lidx != e.mSource)
            {
                ++buffer; // 当它加一时候就说明是不同一个源
                lidx = e.mSource;
            }

            if(buffer != e.mSource)
            {
                j->mSource = buffer;
            }
        }
        mBufferCount = buffer + 1;
        if(sync)
        {
            Nmark * bmode = N_alloc_t(Nmark, mBufferCount);
            for(NCount b = 0; b < mBufferCount; ++b)
            {
                bmode[b] = Buffer::M_NORMAL  | Buffer::M_WHOLE;

                Units destElems;
                for(j = newEle.begin(); j != jend; ++j)
                {
                    if(j->mSource == b)
                    {
                        destElems.push_back(*j);
                    }
                }

                Units::iterator c, cend = destElems.end();
                for(c = destElems.begin(); c != cend; ++c)
                {
                    // 目标
                    VertexUnit & destelem = *c;
                    // 来源
                    const VertexUnit * srcelem = get(destelem.mVType, destelem.mIndex);
                    VertexBuffer * src = getAttach(srcelem->mSource);
                    // 仅提高灵活性
                    if(src->getModeMark() & (Buffer::M_NORMAL ))
                    {
                        bmode[b] &= ~Buffer::M_DEV;
                        bmode[b] |= Buffer::M_NORMAL ;
                    }
                    if(src->getModeMark() & Buffer::M_READ)
                    {
                        bmode[b] &= Buffer::M_READ;
                    }
                    if(!(src->getModeMark() & Buffer::M_WHOLE))
                    {
                        bmode[b] &= ~Buffer::M_WHOLE;
                    }
                }
            }

            BindingList newBind;
            vector<Nui8 *>::type obuf;
            vector<NCount>::type obufsize;
            vector<Nui8 *>::type nbuf;
            vector<NCount>::type nbufsize;
            Nui16 buf = 0;

            BindingList::const_iterator oattach, oattachend = mBindingList.end();
            if(!mBindingList.empty())
            {
                NCount count = mBindingList.rbegin()->first + 1;
                obuf.resize(count);
                obufsize.resize(count);
            }
            for(oattach = mBindingList.begin(); oattach != oattachend; ++oattach)
            {
                N_assert(oattach->second->getUnitCount() >= mCount, "error");
                obuf[oattach->first] = (Nui8 *)oattach->second->lock(Buffer::MM_READ);
                obufsize[oattach->first] = oattach->second->getUnitSize();
            }

            // 读取和创建新缓存和锁定全部,新的绑定
            while(buf < mBufferCount)
            {
                NCount sz = 0;
                for(j = newEle.begin(); j != jend; ++j)
                {
                    if(j->mSource == buf)
                    {
                        sz += i->getSize();
                    }
                }

                VertexBuffer * out;
                mMag->create(out, sz, mCount, bmode[buf]);
                newBind.insert(Npair(buf, out));
                nbuf.push_back((Nui8 *)out->lock(Buffer::MM_WHOLE));
                nbufsize.push_back(sz);

                ++buf;
            }
            N_free(bmode);
            // 新到旧元素的映射
            typedef map<const VertexUnit *, const VertexUnit *>::type NTO;
            NTO nto;

            for(j = newEle.begin(); j != jend; ++j)
            {
                const VertexUnit * e = get((*j).mVType, (*j).mIndex);
                if(!e)
                {
                    N_EXCEPT(NotExist, _I18n("内部异常"));
                }
                // 新->旧
                nto[&(*j)] = e;
            }

            // 现在遍历新缓存,为每个顶点拉出旧数据
            for(NCount v = 0; v < mCount; ++v)
            {
                for(j = newEle.begin(); j != jend; ++j)
                {
                    const VertexUnit * newElem = &(*j);
                    const VertexUnit * oldElem = nto.find(newElem)->second;

                    Nui16 ono = oldElem->mSource;
                    Nui16 nno = newElem->mSource;
                    Nui8 * pSrcBase = obuf[ono] + v * obufsize[ono];
                    // 每个新缓存    数据 + 数据大小偏移量 + 偏移量
                    Nui8 * pDstBase = nbuf[nno] + v * nbufsize[nno];

                    void *pSrc, *pDst;
                    oldElem->getData(pSrcBase, pSrc);
                    newElem->getData(pDstBase, pDst);

                    memcpy(pDst, pSrc, newElem->getSize());
                }
            }

            // 旧
            for(oattach = mBindingList.begin(); oattach != oattachend; ++oattach)
            {
                oattach->second->unlock();
                oattach->second->unref();
            }
            // 新
            BindingList::const_iterator nattach, nattachend = newBind.end();
            for(nattach = newBind.begin(); nattach != nattachend; ++nattach)
            {
                nattach->second->unlock();
            }
            mBindingList.swap(newBind);
        }
        mUnitList.swap(newEle);
    }
    //---------------------------------------------------------------------------
    void VertexData::build(Nmark mode, Nmark * bmode, bool sync)
    {
        bool ani_ske = mode & M_Skeleton;
        bool ani = mode & M_Animation;
        bool ani_nrls = mode & M_Normals;

        Nui16 buffer = 0;
        NCount offset = 0;
        VertexType prevSemantic = VT_Pos;

        Units newEle;
        Units::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            DataType type = i->mType;
            if(type == DT_Colour)
            {
                type = N_Engine().getColourFormat();
            }
            newEle.push_back(VertexUnit(type, i->mVType, 0, 0, i->mIndex));
        }
        newEle.sort(__VertexDataLess);

        Units::iterator j, jend = newEle.end();
        for(j = newEle.begin(); j != jend; ++j)
        {
            const VertexUnit & elem = *j;

            bool splitPre = false;
            bool splitNext = false;
            switch(elem.mVType)
            {
            case VT_Pos:
                // Split positions if vertex animated with only positions
                // group with normals otherwise
                splitPre = false;
                splitNext = ani && !ani_nrls;
                break;
            case VT_Normals:
                // 法线不能与 weights/indices 混合
                splitPre = (prevSemantic == VT_Matrix_Weight || prevSemantic == VT_Matrix_Index);
                // 所有动画网格不得不在法线后分割
                splitNext = (ani_ske || (ani && ani_nrls));
                break;
            case VT_Matrix_Weight:
                // blend weights/indices 仅可与它们的缓存共享
                splitPre = true;
                break;
            case VT_Matrix_Index:
                // blend weights/indices 仅可与它们的缓存共享
                splitNext = true;
                break;
            default:
            case VT_Diffuse:
            case VT_Specular:
            case VT_Tex_Coord:
            case VT_Binormal:
            case VT_Tangent:
                // Make sure position is separate if animated & there were no normals
                splitPre = prevSemantic == VT_Pos && (ani_ske || ani);
                break;
            }

            if(splitPre && offset)
            {
                ++buffer;
                offset = 0;
            }

            prevSemantic = elem.mVType;
            j->mSource = buffer;
            j->mOffset = offset;

            if(splitNext)
            {
                ++buffer;
                offset = 0;
            }
            else
            {
                offset += elem.getSize();
            }
        }
        newEle.sort(__VertexDataLess);
        /*  1+1+1+2+2+2+2+3+4+5+6+7+10+10+11+11+12+13
                                    -- -- -- -- -- --
                                    8  8  9  9  10 11
        */
        Nui16 lidx = (*i).mSource;
        for(buffer = 0, j = newEle.begin(); j != jend; ++j)
        {
            VertexUnit & e = *j;
            if(lidx != e.mSource)
            {
                ++buffer; // 当它加一时候就说明是不同一个源
                lidx = e.mSource;
            }

            if(buffer != e.mSource)
            {
                j->mSource = buffer;
            }
        }
        mBufferCount = buffer + 1;
        if(sync)
        {
            BindingList newBind;
            vector<Nui8 *>::type obuf;
            vector<NCount>::type obufsize;
            vector<Nui8 *>::type nbuf;
            vector<NCount>::type nbufsize;
            Nui16 buf = 0;

            BindingList::const_iterator oattach, oattachend = mBindingList.end();
            if(!mBindingList.empty())
            {
                NCount count = mBindingList.rbegin()->first + 1;
                obuf.resize(count);
                obufsize.resize(count);
            }
            for(oattach = mBindingList.begin(); oattach != oattachend; ++oattach)
            {
                N_assert(oattach->second->getUnitCount() >= mCount, "error");
                obuf[oattach->first] = (Nui8 *)oattach->second->lock(Buffer::MM_READ);
                obufsize[oattach->first] = oattach->second->getUnitSize();
            }

            // 读取和创建新缓存和锁定全部,新的绑定
            while(buf < mBufferCount)
            {
                NCount sz = 0;
                for(j = newEle.begin(); j != jend; ++j)
                {
                    if(j->mSource == buf)
                    {
                        sz += i->getSize();
                    }
                }

                VertexBuffer * out;
                mMag->create(out, sz, mCount, bmode[buf]);
                newBind.insert(Npair(buf, out));
                nbuf.push_back((Nui8 *)out->lock(Buffer::MM_WHOLE));
                nbufsize.push_back(sz);

                ++buf;
            }

            // 新到旧元素的映射
            typedef map<const VertexUnit *, const VertexUnit *>::type NTO;
            NTO nto;

            for(j = newEle.begin(); j != jend; ++j)
            {
                const VertexUnit * e = get((*j).mVType, (*j).mIndex);
                if(!e)
                {
                    N_EXCEPT(NotExist, _I18n("内部异常"));
                }
                // 新->旧
                nto[&(*j)] = e;
            }

            // 现在遍历新缓存,为每个顶点拉出旧数据
            for(NCount v = 0; v < mCount; ++v)
            {
                for(j = newEle.begin(); j != jend; ++j)
                {
                    const VertexUnit * newElem = &(*j);
                    const VertexUnit * oldElem = nto.find(newElem)->second;

                    Nui16 ono = oldElem->mSource;
                    Nui16 nno = newElem->mSource;
                    Nui8 * pSrcBase = obuf[ono] + v * obufsize[ono];
                    // 每个新缓存    数据 + 数据大小偏移量 + 偏移量
                    Nui8 * pDstBase = nbuf[nno] + v * nbufsize[nno];

                    void *pSrc, *pDst;
                    oldElem->getData(pSrcBase, pSrc);
                    newElem->getData(pDstBase, pDst);

                    memcpy(pDst, pSrc, newElem->getSize());
                }
            }

            // 旧
            for(oattach = mBindingList.begin(); oattach != oattachend; ++oattach)
            {
                oattach->second->unlock();
                oattach->second->unref();
            }
            // 新
            BindingList::const_iterator nattach, nattachend = newBind.end();
            for(nattach = newBind.begin(); nattach != nattachend; ++nattach)
            {
                nattach->second->unlock();
            }
            mBindingList.swap(newBind);
        }
        mUnitList.swap(newEle);
    }
    //-----------------------------------------------------------------------
    void VertexData::rebuild(Nmark mode)
    {
        if(mode & M_ShadowVol)
        {
            /* 注意
            I would dearly, dearly love to just use a 4D position buffer in order to
            store the extra 'w' value I need to differentiate between extruded and
            non-extruded sections of the buffer, so that vertex programs could use that.
            Hey, it works fine for GL. However, D3D9 in it's infinite stupidity, does not
            support 4d position vertices in the fixed-function pipeline. If you use them,
            you just see nothing. Since we can't know whether the application is going to use
            fixed function or vertex programs, we have to stick to 3d position vertices and
            store the 'w' in a separate 1D texture coordinate buffer, which is only used
            when rendering the shadow.
            */

            NCount v;
            bool hwvp;

            RenderSys * rend = N_Engine().getRender();
            if(rend && rend->getFeature()->isSupport(GF_Program_Vertex))
            {
                hwvp = true;
            }
            else
            {
                hwvp = false;
            }
            // 寻找一个 位置 的元素
            const VertexUnit * pos = get(VT_Pos);
            if(pos)
            {
                Nui16 posOldSource = pos->mSource;
                VertexBuffer * vbuf = getAttach(posOldSource);

                bool wasSharedBuffer = false;
                if(vbuf->getUnitSize() > pos->getSize())
                {
                    wasSharedBuffer = true;
                }

                VertexBuffer * nposcom, *ocom = 0;
                if(wasSharedBuffer)
                {   // 创建新的缓存 让它存储`位置`以外的其他元素
                    static_cast<GBufferManager *>(vbuf->getManager())->create(ocom,
                        vbuf->getUnitSize() - pos->getSize(), vbuf->getUnitCount(),
                            vbuf->getModeMark());
                }
                // 分配新 位置 缓存, 将是 FLOAT3类型格式 和 2x 的大小
                NCount oldVertexCount = vbuf->getUnitCount();
                NCount newVertexCount = oldVertexCount * 2;
                static_cast<GBufferManager *>(vbuf->getManager())->create(nposcom,
                    DataTypeUtil::getSize(DT_Float3x),  newVertexCount, 
                        vbuf->getModeMark());

                // 遍历旧缓存,复制合适元素和初始化其余的
                NIIf * pSrc;
                Nui8 * pBaseSrc = (Nui8 *)vbuf->lock(Buffer::MM_READ);
                // 第一个目的指针指在新 位置 缓存的开始。另一个沿着它在中途(一半的位置上)
                NIIf * pDest = (NIIf *)nposcom->lock(Buffer::MM_WHOLE);
                NIIf * pDest2 = pDest + oldVertexCount * 3;

                // 预先计算任何顶点区域外部 位置 的尺寸
                NCount prePosVertexSize = 0, postPosVertexSize, postPosVertexOffset;
                Nui8 * pBaseDestRem = 0;
                if(wasSharedBuffer)
                {
                    pBaseDestRem = static_cast<Nui8 *>(ocom->lock(Buffer::MM_WHOLE));
                    //有3部分
                    prePosVertexSize = pos->mOffset;        // 前部分大小
                    postPosVertexOffset = prePosVertexSize + pos->getSize();// 偏移量,位置部分
                    postPosVertexSize = vbuf->getUnitSize() - postPosVertexOffset;//后部分大小

                    // 2个分隔的位在一起大小应该与剩余缓存顶点相同,中间间隔一个顶点
                    N_assert(ocom->getUnitSize() == prePosVertexSize + postPosVertexSize, "error");

                    // 遍历所有顶点
                    for(v = 0; v < oldVertexCount; ++v)
                    {
                        // 复制 位置,到这两个缓存
                        pos->getData(pBaseSrc, pSrc);
                        *pDest++ = *pDest2++ = *pSrc++;
                        *pDest++ = *pDest2++ = *pSrc++;
                        *pDest++ = *pDest2++ = *pSrc++;

                        // 现在处理其他任何元素,不包含 位置
                        if(prePosVertexSize > 0)
                            memcpy(pBaseDestRem, pBaseSrc, prePosVertexSize);
                        if(postPosVertexSize > 0)
                            memcpy(pBaseDestRem + prePosVertexSize, pBaseSrc + postPosVertexOffset, postPosVertexSize);

                        pBaseDestRem += ocom->getUnitSize();

                        pBaseSrc += vbuf->getUnitSize();

                    } // 下一个顶点
                }
                else
                {
                    // 非共享的缓存, 可以以块复制整个东西
                    memcpy(pDest, pBaseSrc, vbuf->getSize());
                    memcpy(pDest2, pBaseSrc, vbuf->getSize());
                }
                //解锁
                vbuf->unlock();
                nposcom->unlock();
                if(wasSharedBuffer)
                    ocom->unlock();

                // 在这个阶段,它原来顶点缓存将要被删除,因此我们应该强制释放任何临时副本
                vbuf->getManager()->recoverTemp(vbuf);
                //可以使用顶点着色程序？
                if(hwvp)
                {
                    // 现在建立w缓存
                    static_cast<GBufferManager *>(vbuf->getManager())->create(mExtData,
                        sizeof(NIIf), newVertexCount, Buffer::M_NORMAL );
                    // 一半填充1.0,一半填充0.0
                    pDest = static_cast<NIIf *>(mExtData->lock(Buffer::MM_WHOLE));
                    for(v = 0; v < oldVertexCount; ++v)
                    {
                        *pDest++ = 1.0f;
                    }
                    for(v = 0; v < oldVertexCount; ++v)
                    {
                        *pDest++ = 0.0f;
                    }
                    mExtData->unlock();
                    mExtData->ref();
                }

                Nui16 newPosBufferSource;
                if(wasSharedBuffer)
                {
                    // 获取新缓存绑定索引
                    newPosBufferSource = getFreeAttachIndex();
                    // 重新绑定的旧索引到剩余缓存
                    attach(posOldSource, ocom);
                }
                else
                {
                    // 仅需重新使用相同资源索引,为新的位置缓存
                    newPosBufferSource = posOldSource;
                }
                // 绑定新 位置 缓存
                attach(newPosBufferSource, nposcom);

                // 现在,转变顶点声明到改变的位置资源,和使用相同缓存元素偏移量
                Units::const_iterator elemi = mUnitList.begin();
                Units::const_iterator elemiend = mUnitList.end();
                Nui16 idx;

                // 遍历所有元素
                for(idx = 0; elemi != elemiend; ++elemi, ++idx)
                {
                    if(&(*elemi) == pos)
                    {
                        // 修改位置指向新位置缓存
                        modify(idx,
                            DT_Float3x,
                            VT_Pos,
                            newPosBufferSource, // 新资源缓存
                            0);                 // 没有偏移量
                    }
                    else if(wasSharedBuffer && elemi->mSource == posOldSource &&
                        elemi->mOffset > prePosVertexSize)
                    {
                        // 这个元素在"位置"之后,移去"位置"所占的大小,处理newRemainderBuffer中 位置 的空缺
                        modify(idx,
                            elemi->mType,
                            elemi->mVType,
                            posOldSource, // 相同旧资源
                            elemi->mOffset - pos->getSize(), // 减少偏移量
                            elemi->mIndex);
                    }
                }
                N_delete vbuf;
                //注意 我们不改变 vertexCount,因为其他缓存终究是相同大小
            }
        }
    }
    //-----------------------------------------------------------------------
    NCount VertexData::allocHWAnimate(NCount count, bool normal)
    {
        NCount re = count;
        if(mHWAni->mFusionData.size() < count)
        {
            re = mHWAni->alloc(this, count, normal);
        }
        for(NCount i = 0; i < mHWAni->mFusionData.size(); ++i)
        {
            mHWAni->mFusionData[i].mFactor = 0.0f;
        }
        mHWAni->mFusionIndex = 0;

        return re;
    }
    //-----------------------------------------------------------------------
    void VertexData::allocTangent(VertexType targetSemantic, Nui16 index, Nui16 sourceTexCoordSet)
    {
        const VertexUnit * tangentsElem = get(targetSemantic, index);
        bool needsToBeCreated = false;

        if(!tangentsElem)
        {   // no tex coords with index 1
            needsToBeCreated = true ;
        }
        else if(tangentsElem->mType != DT_Float3x)
        {
            //  buffer exists, but not 3D
            N_EXCEPT(InvalidParam, _I18n("Target semantic set already exists but is not 3D, therefore cannot contain tangents. Pick an alternative destination semantic."));
        }

        VertexBuffer * newBuffer;
        if(needsToBeCreated)
        {
            // To be most efficient with our vertex streams,
            // tack the new tangents onto the same buffer as the
            // source texture coord set
            const VertexUnit * prevTexCoordElem = get(VT_Tex_Coord, sourceTexCoordSet);
            if(!prevTexCoordElem)
            {
                N_EXCEPT(NotExist, _I18n("Cannot locate the first texture coordinate element to which to append the new tangents."));
            }
            // Find the buffer associated with  this element
            VertexBuffer * origBuffer = getAttach(prevTexCoordElem->mSource);
            // 现在创建一个新的缓冲区,其中包括以前的内容,加上额外的三维坐标空间
            N_Only(GBuffer).create(newBuffer, origBuffer->getUnitSize() + 3 * sizeof(NIIf),  mCount, origBuffer->getModeMark());
            // Add the new element
            add(DT_Float3x, targetSemantic, prevTexCoordElem->mSource, origBuffer->getUnitSize(), index);
            // Now copy the original data across
            Nui8 * pSrc = static_cast<Nui8 *>(origBuffer->lock(Buffer::MM_READ));
            Nui8 * pDest = static_cast<Nui8 *>(newBuffer->lock(Buffer::MM_WHOLE));
            NCount vertSize = origBuffer->getUnitSize();
            for(NCount v = 0; v < mCount; ++v)
            {
                // Copy original vertex data
                memcpy(pDest, pSrc, vertSize);
                pSrc += vertSize;
                pDest += vertSize;
                // 设置新的部分为0,因为现在还没有数据
                memset(pDest, 0, sizeof(NIIf) * 3);
                pDest += sizeof(NIIf) * 3;
            }
            origBuffer->unlock(); //todo 需要释放origBuffer->setNull;
            newBuffer->unlock();

            // 重新绑定新缓存
            attach(prevTexCoordElem->mSource, newBuffer);
        }
    }
    //-----------------------------------------------------------------------
    void VertexData::opt()
    {
        if(mBindingList.empty())
            return;
        if(mBindingList.rbegin()->first + 1 == (NIIi)mBindingList.size())
            return;

        Units::iterator ai, aiend = mUnitList.end();
        for(ai = mUnitList.begin(); ai != aiend; ++ai)
        {
            const VertexUnit & e = *ai;
            if(!isAttach(e.mSource))
            {
                N_EXCEPT(NotExist, _I18n("存在顶点成分无绑定数据"));
            }
        }

        typedef map<Nui16, Nui16>::type NewMap;
        NewMap newmap;
        BindingList newAttach;
        Nui16 index;

        BindingList::const_iterator it, itend = mBindingList.end();
        for(index = 0, it = mBindingList.begin(); it != itend; ++it, ++index)
        {
            newmap[it->first] = index;
            newAttach[index] = it->second;
        }

        mBindingList.swap(newAttach);
        mHighIndex = index;

        // 修改顶点元素到引用新缓存索引
        for(ai = mUnitList.begin(); ai != aiend; ++ai)
        {
            const VertexUnit & e = *ai;
            NewMap::const_iterator it = newmap.find(e.mSource);
            N_assert(it != newmap.end(), "error");
            Nui16 newsrc = it->second;
            if(e.mSource != newsrc)
            {
                ai->mSource = newsrc;
            }
        }
    }
    //-----------------------------------------------------------------------
    void VertexData::optAttach()
    {
        BindingList newmap;
        Nui16 index = 0;
        BindingList::const_iterator it, itend = mBindingList.end();
        for(it = mBindingList.begin(); it != itend; ++it, ++index)
        {
            newmap[index] = it->second;
        }

        mBindingList.swap(newmap);
        mHighIndex = index;
    }
    //------------------------------------------------------------------------
    VertexData * VertexData::clone(CopyType ct, BufferModeMark mode, GBufferManager * mag) const
    {
        GBufferManager * magr = mag ? mag : mMag;
        VertexData * dest;
        magr->create(dest);

        if(ct != CT_Struct)
        {
            BindingList::const_iterator a, aend = mBindingList.end();
            for(a = mBindingList.begin(); a != aend; ++a)
            {
                VertexBuffer * src = a->second;
                VertexBuffer * dst;

                if(ct == CT_Clone)
                {
                    mode = mode < 0 ? src->getModeMark() : mode;

                    const void * shadowBuff = src->getHostData();
                    if( shadowBuff )
                    {
                        void * dstData;
                        if(mode & M_HostCache)
                        {
                            void * dstData = N_alloc(src->getSize());
                            memcpy(dstData, shadowBuff, src->getSize());
                        }
                        else
                        {
                            dstData = shadowBuff;
                        }
                        mag->create(dst, src->getUnits(), src->getUnitCount(), src->getMultiCount(), mode, dstData, src->getGroupID());
                    }
                    else
                    {
                        GpuBufferAsync * bufAsync;
                        N_Only(GBuffer).create(bufAsync, it->second, 0, it->second->getSize());

                        if(mode & M_HostCache)
                        {
                            const void * srcData = bufAsync->map();
                            void * dstData = N_alloc(src->getSize());
                            memcpy(dstData, srcData, src->getSize());
                            bufAsync->unmap();
                            
                            mag->create(dst, src->getUnits(), src->getUnitCount(), src->getMultiCount(), mode, dstData, src->getGroupID());
                        }
                        else
                        {
                            const void *srcData = bufAsync->map();
                            mag->create(dst, src->getUnits(), src->getUnitCount(), src->getMultiCount(), mode, srcData, src->getGroupID());
                            bufAsync->unmap();
                        }
                    }
                    dest->attach(a->first, dst);
                }
                else if(ct == CT_Ref)
                {
                    dst = src;
                    dst->mAutoDestroy = false;
                    dst->ref();
                    dest->attach(a->first, dst);
                }
                else
                {
                    dst = 0;
                }
            }
        }

        dest->mOffset = mOffset;
        dest->mCount = mCount;

        Units::const_iterator ei, eiend = mUnitList.end();
        for(ei = mUnitList.begin(); ei != eiend; ++ei)
        {
            dest->add(ei->mType, ei->mVType, ei->mSource, ei->mOffset, ei->mIndex);
        }

        dest->mHWAni->mFusionData = mHWAni->mFusionData;
        dest->mHWAni->mFusionIndex = mHWAni->mFusionIndex;

        dest->mExtData = mExtData;
        dest->mBufferCount = mBufferCount;

        return dest;
    }
    //------------------------------------------------------------------------
    VertexData * VertexData::cloneUnit(bool swave, GBufferManager * mag) const
    {
        VertexData * re = clone(CT_Struct);

        Nui16 safeSource = 0xFFFF;
        const VertexUnit * blendunit = get(VT_Matrix_Index);
        if(blendunit)
        {
            //save the src in order to prevent the next stage from unbinding it.
            safeSource = blendunit->mSource;
            if(swave)
            {
                // Remove buffer reference
                re->detach(blendunit->mSource);
            }
        }
        if(swave)
        {
            // Remove blend weights
            const VertexUnit * weightunit = get(VT_Matrix_Weight);
            if(weightunit && weightunit->mSource != safeSource)
            {
                // Remove buffer reference
                re->detach(weightunit->mSource);
            }
        }

        if(swave)
        {
            re->remove(VT_Matrix_Index);
            re->remove(VT_Matrix_Weight);
            re->opt();
        }

        return re;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // IndirectData
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    IndirectData::IndirectData(Type type):
        mCount(0),
        mOffset(0),
        mBuffer(0),
        mType(type),
        mAutoDestroy(true)
    {
        type == T_Vertex ? mVertexIndirectList.reserve(10) : mIndexIndirectList.reserve(10);
    }
    //-----------------------------------------------------------------------
    IndirectData::IndirectData(Type type, NCount unitCnt):
        mCount(unitCnt),
        mOffset(0),
        mBuffer(0),
        mType(type),
        mAutoDestroy(true)
    {
        type == T_Vertex ? mVertexIndirectList.reserve(10) : mIndexIndirectList.reserve(10);
    }
    //-----------------------------------------------------------------------
    IndirectData::IndirectData() :
        mCount(0),
        mOffset(0),
        mBuffer(0),
        mAutoDestroy(true)
    {
    }
    //-----------------------------------------------------------------------
    IndirectData::~IndirectData()
    {
        if (mBuffer != 0)
        {
            if (mAutoDestroy)
            {
                N_delete mBuffer;
            }
            else
            {
                mBuffer->unref();
            }
            mBuffer = 0;
        }
    }
    //-----------------------------------------------------------------------
    IndirectData * IndirectData::clone(CopyType ct, BufferModeMark mode, GBufferManager * mag) const
    {
        if(!mag)
            mag = GBufferManager::getOnlyPtr();
        IndirectData * dest = N_new IndirectData(mType);
        if (mBuffer != 0)
        {
            if (ct == CT_Clone)
            {
                mode = mode < 0 ? mBuffer->getModeMark() : mode;

                const void * shadowBuff = mBuffer->getHostData();
                if( shadowBuff )
                {
                    void * dstData;
                    if(mode & M_HostCache)
                    {
                        dstData = N_alloc(mBuffer->getSize());
                        memcpy( dstData, shadowBuff, mBuffer->getSize() );
                    }
                    else
                    {
                        dstData = shadowBuff;
                    }
                    
                    mag->create(dest->mBuffer, mBuffer->getType(), mBuffer->getUnitCount(), mBuffer->getMultiCount(), mode, dstData, mBuffer->getGroupID());
                }
                else
                {
                    if(mode & M_HostCache)
                    {
                        GpuBufferAsync * bufAsync;
                        N_Only(GBuffer).create(bufAsync, mBuffer, 0, mBuffer->getSize());
                        const void * srcData = bufAsync->map();
                        void * dstData = N_alloc(mBuffer->getSize());
                        memcpy( dstData, srcData, mBuffer->getSize() );
                        bufAsync->unmap();
                    
                        mag->create(dest->mBuffer, mBuffer->getType(), mBuffer->getUnitCount(), mBuffer->getMultiCount(), mode, dstData, mBuffer->getGroupID());
                    }
                    else
                    {
                        mag->create(dest->mBuffer, mBuffer->getType(), mBuffer->getUnitCount(), mBuffer->getMultiCount(), mode, 0, mBuffer->getGroupID());
                        dest->mBuffer->write(mBuffer, 0, 0, mBuffer->getSize());
                    }
                }
                dest->mBuffer->ref();
            }
            else if(ct == CT_Ref)
            {
                dest->mBuffer = mBuffer;
                dest->mAutoDestroy = false;
                dest->mBuffer->ref();
            }
            else
            {
                dest->mBuffer = 0;
                mAutoDestroy = true;
            }
        }
        dest->mCount = mCount;
        dest->mOffset = mOffset;
        return dest;
    }
    //-----------------------------------------------------------------------
    NCount IndirectData::getInstanceCount() const
    {
        NCount re = 0;
        if(mType == T_Vertex)
        {
            VertexIndirectList::iterator i, iend = mVertexIndirectList.end();
            for(i = mVertexIndirectList.begin(); i != iend; ++i)
            {
                re += i->mInstanceCount;
            }
        }
        else
        {
            IndexIndirectList::iterator i, iend = mIndexIndirectList.end();
            for(i = mIndexIndirectList.begin(); i != iend; ++i)
            {
                re += i->mInstanceCount;
            }
        }
        return re;
    }
    //-----------------------------------------------------------------------
    void IndirectData::addVertex(NCount Count, NCount InstanceCount, NCount Oft, NCount InstanceOft)
    {
        mVertexIndirectList.emplace_back(Count, InstanceCount, Oft, InstanceOft);
    }
    //-----------------------------------------------------------------------
    void IndirectData::removeVertex(Nidx idx)
    {
        VertexIndirectList::iterator i = mVertexIndirectList.begin();
        std::advance(i, idx);
        mVertexIndirectList.erase(i);
    }
    //-----------------------------------------------------------------------
    void IndirectData::clearAllVertex()
    {
        mVertexIndirectList.clear();
    }
    //-----------------------------------------------------------------------
    void IndirectData::addIndex(NCount Count, NCount InstanceCount, NCount Oft, NCount VertexOft, NCount InstanceOft)
    {
        mIndexIndirectList.emplace_back(Count, InstanceCount, Oft, VertexOft, InstanceOft);
    }
    //-----------------------------------------------------------------------
    void IndirectData::removeIndex(Nidx idx)
    {
        IndexIndirectList::iterator i = mIndexIndirectList.begin();
        std::advance(i, idx);
        mIndexIndirectList.erase(i);
    }
    //-----------------------------------------------------------------------
    void IndirectData::clearAllIndex()
    {
        mIndexIndirectList.clear();
    }
    //-----------------------------------------------------------------------
    void IndirectData::attach(IndirectBuffer * buf)
    {
        if (mBuffer)
            mBuffer->unref();

        mBuffer = buf;

        if (buf)
            buf->ref();
    }
    //-----------------------------------------------------------------------
    void IndirectData::detach()
    {
        if (mBuffer)
        {
            mBuffer->unref();
            mBuffer = 0;
        }
    }
    //-----------------------------------------------------------------------
    void IndirectData::write(NCount oft, NCount bufoft, NCount cnt)
    {
        if (mBuffer)
        {
            NCount unitsize = getUnitSize();
            NCount comsize = unitsize * mCount;
            if (comsize < bufoft + cnt)
                cnt = comsize - bufoft;
            NCount tts = (oft + cnt) / unitsize;
            if (mVertexIndirectList.size() < tts)
                mVertexIndirectList.resize(tts);
            mType == T_Vertex ? mBuffer->read(&(mVertexIndirectList[0]) + oft, mOffset + bufoft, cnt) :
                mBuffer->read(&(mIndexIndirectList[0]) + oft, mOffset + bufoft, cnt);
        }
    }
    //-----------------------------------------------------------------------
    void IndirectData::read(NCount oft, NCount bufoft, NCount cnt)
    {
        if (mBuffer)
        {   
            NCount unisize = getUnitSize();
            NCount comsize = unisize * mCount;
            if (comsize < bufoft + cnt)
                cnt = comsize - bufoft;
            NCount tts = (oft + cnt) / unisize;
            if (mVertexIndirectList.size() < tts)
                cnt = mVertexIndirectList.size() * unisize - oft;
            mType == T_Vertex ? mBuffer->write(&(mVertexIndirectList[0]) + oft, mOffset + bufoft, cnt) :
                mBuffer->write(&(mIndexIndirectList[0]) + oft, mOffset + bufoft, cnt);
        }
    }
    //-----------------------------------------------------------------------
}