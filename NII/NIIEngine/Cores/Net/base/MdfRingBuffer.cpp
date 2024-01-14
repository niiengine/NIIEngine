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
#include "MdfRingBuffer.h"

namespace Mdf
{
    //-----------------------------------------------------------------------
    RingBuffer::RingBuffer()
    {
        mData = 0;
        mAllocSize = 0;
        mWriteSize = 0;
    }
    //-----------------------------------------------------------------------
    RingBuffer::~RingBuffer()
    {
        mAllocSize = 0;
        mWriteSize = 0;
        if(mData)
        {
            free(mData);
            mData = 0;
        }
    }
    //-----------------------------------------------------------------------
    void RingBuffer::enlarge(MCount cnt)
    {
        mAllocSize = mWriteSize + cnt;
        mAllocSize += mAllocSize >> 2;
        if (mData)
        {
            Mui8 * nbuf = (Mui8*)realloc(mData, mAllocSize);
            mData = nbuf;
        }
        else
        {
            mData = (Mui8*)malloc(mAllocSize);
        }

    }
    //-----------------------------------------------------------------------
    void RingBuffer::readSkip(MCount cnt)
    {
        if (cnt > mWriteSize)
            cnt = mWriteSize;

        mWriteSize -= cnt;
        memmove(mData, mData + cnt, mWriteSize);
    }
    //-----------------------------------------------------------------------
    void RingBuffer::writeSkip(MCount cnt)
    { 
        mWriteSize += cnt;
    }
    //-----------------------------------------------------------------------
    Mui32 RingBuffer::write(const void * in, MCount cnt)
    {
        if (mWriteSize + cnt > mAllocSize)
        {
            enlarge(cnt);
        }

        if (in)
        {
            memcpy(mData + mWriteSize, in, cnt);
        }

        mWriteSize += cnt;

        return cnt;
    }
    //-----------------------------------------------------------------------
    Mui32 RingBuffer::read(void * out, MCount cnt) const
    {
        if (cnt > mWriteSize)
            cnt = mWriteSize;

        if (out)
            memcpy(out, mData, cnt);

        mWriteSize -= cnt;
        memmove(mData, mData + cnt, mWriteSize);
        return cnt;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RingLoopBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    RingLoopBuffer::RingLoopBuffer()
    {
        mData = NULL;
        mAllocSize = 0;
        mWriteSize = 0;
        mHead = 0;
        mTail = 0;
    }
    //-----------------------------------------------------------------------
    RingLoopBuffer::~RingLoopBuffer()
    {
        destroy_buffer();
    }
    //-----------------------------------------------------------------------
    bool RingLoopBuffer::alloc(Mui32 nsize)
    {
        free();
        int dstsize = (nsize / 4 + 1) * 4;
        mData = (Mui8 *)malloc(dstsize);
        memset(mData, 0, dstsize);
        mAllocSize = dstsize;
        mWriteSize = 0;
        return true;
    }
    //-----------------------------------------------------------------------
    void RingLoopBuffer::free()
    {
        if (mData != NULL)
        {
            free(mData);
            mData = NULL;
            mHead = 0;
            mTail = 0;
            mWriteSize = 0;
        }
    }
    //-----------------------------------------------------------------------
    Mui32 RingLoopBuffer::write(const void * in, int size)
    {
        if (mAllocSize - mWriteSize < size)
        {
            return 0;
        }

        if (mTail < mHead)
        {
            memcpy(&mData[mTail], in, size);
        }
        else
        {
            int nrest_tail = mAllocSize - mTail;
            if (nrest_tail >= size)
            {
                memcpy(&mData[mTail], in, size);
            }
            else
            {
                memcpy(&mData[mTail], in, nrest_tail);
                memcpy(&mData[0], &in[nrest_tail], size - nrest_tail);
            }
        }
        mTail = (mTail + size) % mAllocSize;
        mWriteSize += size;
        return size;
    }
    //-----------------------------------------------------------------------
    Mui32 RingLoopBuffer::read(void * out, int nbuffer_size) const
    {
        int dstsize = (nbuffer_size < getWriteSize() ? nbuffer_size : getWriteSize());
        if (mHead <= mTail)
        {
            memcpy(out, &mData[mHead], dstsize);
        }
        else
        {
            int nrestsize = mAllocSize - mHead;
            if (dstsize <= nrestsize)
            {
                memcpy(out, &mData[mHead], dstsize);
            }
            else
            {
                memcpy(out, &mData[mHead], nrestsize);
                memcpy(&out[nrestsize], &mData[0], dstsize - nrestsize);
            }
        }
        mWriteSize -= dstsize;
        mHead = (mHead + dstsize) % mAllocSize;
        return dstsize;
    }
    //-----------------------------------------------------------------------
    Mui32 RingLoopBuffer::peek(void * out, int size) const
    {
        int dstsize = (size < mWriteSize ? size : mWriteSize);
        if (mHead <= mTail)
        {
            memcpy(out, &mData[mHead], dstsize);
        }
        else
        {
            int nrestsize = mAllocSize - mHead;
            if (dstsize <= nrestsize)
            {
                memcpy(out, &mData[mHead], dstsize);
            }
            else
            {
                memcpy(out, &mData[mHead], nrestsize);
                memcpy(&out[nrestsize], &mData[0], dstsize - nrestsize);
            }
        }
        return dstsize;
    }
    //-----------------------------------------------------------------------
}