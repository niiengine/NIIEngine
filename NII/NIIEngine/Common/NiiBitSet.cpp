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
#include "NiiBitSet.h"
#include "NiiBitwise.h"

namespace NII
{
    //-----------------------------------------------------------------
    static const NCount NiiContentMark[256] =
    {
        0,
        1,  1,  2,  1,  2,  2,  3,  1,  2,  2,
        3,  2,  3,  3,  4,  1,  2,  2,  3,  2,
        3,  3,  4,  2,  3,  3,  4,  3,  4,  4,
        5,  1,  2,  2,  3,  2,  3,  3,  4,  2,
        3,  3,  4,  3,  4,  4,  5,  2,  3,  3,
        4,  3,  4,  4,  5,  3,  4,  4,  5,  4,
        5,  5,  6,  1,  2,  2,  3,  2,  3,  3,
        4,  2,  3,  3,  4,  3,  4,  4,  5,  2,
        3,  3,  4,  3,  4,  4,  5,  3,  4,  4,
        5,  4,  5,  5,  6,  2,  3,  3,  4,  3,
        4,  4,  5,  3,  4,  4,  5,  4,  5,  5,
        6,  3,  4,  4,  5,  4,  5,  5,  6,  4,
        5,  5,  6,  5,  6,  6,  7,  1,  2,  2,
        3,  2,  3,  3,  4,  2,  3,  3,  4,  3,
        4,  4,  5,  2,  3,  3,  4,  3,  4,  4,
        5,  3,  4,  4,  5,  4,  5,  5,  6,  2,
        3,  3,  4,  3,  4,  4,  5,  3,  4,  4,
        5,  4,  5,  5,  6,  3,  4,  4,  5,  4,
        5,  5,  6,  4,  5,  5,  6,  5,  6,  6,
        7,  2,  3,  3,  4,  3,  4,  4,  5,  3,
        4,  4,  5,  4,  5,  5,  6,  3,  4,  4,
        5,  4,  5,  5,  6,  4,  5,  5,  6,  5,
        6,  6,  7,  3,  4,  4,  5,  4,  5,  5,
        6,  4,  5,  5,  6,  5,  6,  6,  7,  4,
        5,  5,  6,  5,  6,  6,  7,  5,  6,  6,
        7,  6,  7,  7,  8
    };
    //------------------------------------------------------------------------
    BitSet::BitSet() :
        mData(NULL),
        mCount(0)
    {
    }
    //------------------------------------------------------------------------
    BitSet::BitSet(NCount count)
    {
        mCount = (count + IOValid - 1) >> IOMark;
        mData = N_alloc_t(IOCell, mCount);
        memset(mData, 0, IOByteCount * mCount);
    }
    //------------------------------------------------------------------------
    BitSet::~BitSet()
    {
        if(mData != NULL)
            N_free(mData);
    }
    //------------------------------------------------------------------------
    void BitSet::resize(NCount count)
    {
        if (mData != NULL)
        {
            NCount t1 = (count + IOValid - 1) >> IOMark;
            if (t1 <= mCount)
                return;
            IOCell * nData = N_alloc_t(IOCell, t1);
            memcpy(nData, mData, IOByteCount * mCount);
            memset(nData + (IOByteCount * mCount), 0, IOByteCount * (t1 - mCount));
            N_free(mData);
            mData = nData;
            mCount = t1;
        }
        else
        {
            mCount = (count + IOValid - 1) >> IOMark;
            mData = N_alloc_t(IOCell, mCount);
            memset(mData, 0, IOByteCount * mCount);
        }
    }
    //------------------------------------------------------------------------
    void BitSet::setTrue(Nidx io)
    {
        NCount r = io >> IOMark;
        mData[r].data |= N32_1OrMark[io % IOValid];

        for(++r; r < mCount; ++r)
            ++mData[r].mark;
    }
    //------------------------------------------------------------------------
    void BitSet::setTrue(Nidx r, Nidx sr)
    {
        mData[r].data |= N32_1OrMark[sr];

        for(++r; r < mCount; ++r)
            ++mData[r].mark;
    }
    //------------------------------------------------------------------------
    void BitSet::setFalse(Nidx io)
    {
        NCount r = io >> IOMark;
        mData[r].data &= N32_1NotMark[io % IOValid];

        for(++r; r < mCount; ++r)
            --mData[r].mark;
    }
    //------------------------------------------------------------------------
    void BitSet::setFalse(Nidx r, Nidx sr)
    {
        mData[r].data &= N32_1NotMark[sr];

        for(++r; r < mCount; ++r)
            --mData[r].mark;
    }
    //------------------------------------------------------------------------
    NCount BitSet::getMostSignificant(Nidx io)
    {
        NCount temp = io >> IOMark;
        IOType t1 = mData[temp].data & N32_1AndMark[io % IOValid];
        return Bitwise::mostSignificantBitSet(t1) + mData[temp].mark;;
    }
    //------------------------------------------------------------------------
    NCount BitSet::getMostSignificant(Nidx r, Nidx sr)
    {
        IOType t1 = mData[r].data & N32_1AndMark[sr];
        return Bitwise::mostSignificantBitSet(t1) + mData[r].mark;;
    }
    //------------------------------------------------------------------------
    NCount BitSet::getCount(Nidx io)
    {
        NCount temp = io >> IOMark;
        IOType t1 = mData[temp].data & N32_1AndMark[io % IOValid];

        Nui8 * t2 = (Nui8 *)&t1;
#if N_ARCH == N_ARCH_32
        NCount count =
            NiiContentMark[t2[0]] + NiiContentMark[t2[1]] +
            NiiContentMark[t2[2]] + NiiContentMark[t2[3]]
#elif N_ARCH == N_ARCH_64
        NCount count =
            NiiContentMark[t2[0]] + NiiContentMark[t2[1]] +
            NiiContentMark[t2[2]] + NiiContentMark[t2[3]] +
            NiiContentMark[t2[4]] + NiiContentMark[t2[5]] +
            NiiContentMark[t2[6]] + NiiContentMark[t2[7]]
#elif N_ARCH == N_ARCH_128 // 改 使用并行算术
        NCount count =
            NiiContentMark[t2[0]] + NiiContentMark[t2[1]] +
            NiiContentMark[t2[2]] + NiiContentMark[t2[3]] +
            NiiContentMark[t2[4]] + NiiContentMark[t2[5]] +
            NiiContentMark[t2[6]] + NiiContentMark[t2[7]] +
            NiiContentMark[t2[8]] + NiiContentMark[t2[9]] +
            NiiContentMark[t2[10]] + NiiContentMark[t2[11]] +
            NiiContentMark[t2[12]] + NiiContentMark[t2[13]] +
            NiiContentMark[t2[14]] + NiiContentMark[t2[15]]
#endif
        + mData[temp].mark;

        return count;
    }
    //------------------------------------------------------------------------
    NCount BitSet::getCount(Nidx r, Nidx sr)
    {
        IOType t1 = mData[r].data & N32_1AndMark[sr];

        Nui8 * t2 = (Nui8 *)&t1;
#if N_ARCH == N_ARCH_32
        NCount count =
            NiiContentMark[t2[0]] + NiiContentMark[t2[1]] +
            NiiContentMark[t2[2]] + NiiContentMark[t2[3]]
#elif N_ARCH == N_ARCH_64
        NCount count =
            NiiContentMark[t2[0]] + NiiContentMark[t2[1]] +
            NiiContentMark[t2[2]] + NiiContentMark[t2[3]] +
            NiiContentMark[t2[4]] + NiiContentMark[t2[5]] +
            NiiContentMark[t2[6]] + NiiContentMark[t2[7]]
#elif N_ARCH == N_ARCH_128 // 改 使用并行算术
        NCount count =
            NiiContentMark[t2[0]] + NiiContentMark[t2[1]] +
            NiiContentMark[t2[2]] + NiiContentMark[t2[3]] +
            NiiContentMark[t2[4]] + NiiContentMark[t2[5]] +
            NiiContentMark[t2[6]] + NiiContentMark[t2[7]] +
            NiiContentMark[t2[8]] + NiiContentMark[t2[9]] +
            NiiContentMark[t2[10]] + NiiContentMark[t2[11]] +
            NiiContentMark[t2[12]] + NiiContentMark[t2[13]] +
            NiiContentMark[t2[14]] + NiiContentMark[t2[15]]
#endif
        + mData[r].mark;

        return count;
    }
    //------------------------------------------------------------------------
}