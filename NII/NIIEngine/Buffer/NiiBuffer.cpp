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
#include "NiiBuffer.h"
#include "NiiException.h"
#include "NiiBufferManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // BufferOp
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    BufferOp::BufferOp(Buffer * buf) : mBuffer(buf){}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Buffer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Buffer::Buffer(BufferManager * mag, NCount unitsize, NCount unitcnt, NCount multi, BufferModeMark mode, Nui8 * initData, GroupID gid, NCount goft, NCount padSize) :
        mMag(mag),
        mMark(mode),
        mRefCount(0),
        mCurrentMulti(0),
        mMuteOft(0),
        mMuteSize(0),
        mMuteMulti(0),
        mUnitSize(unitsize),
        mUnitCount(unitcnt),
        mSize(unitsize * unitcnt),
        mReserveSize(mSize + padSize),
        mGroupID(gid),
        mOffset(goft),
        mShadow(NULL),
        mSecond(NULL),
        mOp(NULL),
        mGroupID(0),
        mShadowRefresh(false),
        mMapping(false)
    {
        if((mode & Buffer::M_HOST) && (mode & Buffer::M_DEV))
        {
            mMark |= M_HOSTWRITE;
        }
        
        if(mode & M_Multi)
        {
            (multi < 2) ? mMutilCount = 2 : mMutilCount = multi;
        }
        else
        {
            mMutilCount = 1;
        }
        mMark |= M_SYN;
        mAutoDestroy = true;
    }
    //------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        mUnitCount = 0;
        mUnitSize = 0;
        mReserveSize = 0;
        
        if(!mMapping)
        {
            unlock(UT_UnlockMap);
        }

        if(mOp)
        {
            N_delete mOp;
            mOp = 0;
        }

        if(mShadow)
        {
            N_delete mShadow;
            mShadow = 0;
        }
    }
    //------------------------------------------------------------------------
    Nui8 * Buffer::getHostData() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void * Buffer::lock(MuteMark mm, NCount oft, NCount size, NCount multiIdx)
    {
        N_assert(!isMapping(), "Cannot lock this buffer, it is already locked!");
        N_assert((size + oft) <= mSize, "Lock request out of bounds.");

        void * re = NULL;
        mMuteMulti = mCurrentMulti + multiIdx;

        if(mShadow)
        {
            if(mm & MM_WRITE)
            {
                mShadowRefresh = true;
            }

            re = mShadow->lock(mm, oft, size);
            mMuteOft = oft;
            mMuteSize = size;
        }
        else
        {
            if(mOp)
                re = mOp->lockImpl(mm, oft + mMuteMulti * mReserveSize, size);
            else
                re = lockImpl(mm, oft + mMuteMulti * mReserveSize, size);
        }
        return re;
    }
    //-------------------------------------------------------------------------
    void Buffer::unlock(UnlockType type)
    {
        N_assert(isMapping(), "Cannot unlock this buffer, it is not locked!");

        if(mShadow && mShadow->isMapping())
        {
            mShadow->unlock(type);
            syncImpl();
            mMuteOft = 0;
            mMuteSize = 0;
        }
        else
        {
            unlock(type, 0, mMuteSize);
        }
    }
    //------------------------------------------------------------------------------
    void Buffer::unlock(UnlockType type, NCount oft, NCount size)
    {
        if(mOp)
            mOp->unlockImpl(type, oft, size);
        else
            unlockImpl(type, oft, size);
        mMuteOft = 0;
        mMuteSize = 0;
    }
    //---------------------------------------------------------------------------
    bool Buffer::resize(NCount newSize, BufferModeMark newMode, bool paddingUnit, bool oldData)
    {
        bool result = true;
        if(paddingUnit)
            newSize = newSize + (newSize % mUnitSize);
        if (newSize > mReserveSize)
        {
            if(mOp)
                result = mOp->reserveImpl(newSize, newMode, oldData);
            else
                result = reserveImpl(newSize, newMode, oldData);
        }
        return result;
    }
    //---------------------------------------------------------------------------
    void Buffer::wait()
    {
        if(mOp)
            mOp->waitImpl();
        else
            waitImpl();
    }
    //---------------------------------------------------------------------------
    NCount Buffer::ref()
    {
        ++mRefCount;
        return mRefCount;
    }
    //---------------------------------------------------------------------------
    NCount Buffer::unref()
    {
        --mRefCount;
        if(mRefCount == 0 && mAutoDestroy)
            mMag->recover(this);
        return mRefCount;
    }
    //---------------------------------------------------------------------------
    void Buffer::syncImpl()
    {
        if(mShadow && mShadowRefresh && (mMark & M_SYN))
        {
            if(mMuteSize == 0)
                mMuteSize = mSize;
            const void * src = mShadow->lockImpl(MM_READ, mMuteOft, mMuteSize);
            Nui32 mm;
            if(mMuteOft == 0 && mMuteSize == mSize)
            {
                mm = MM_WHOLE;
            }
            else
            {
                mm = MM_READ | MM_WRITE;
            }
            void dest;
            if(mOp)
                dest = mOp->lockImpl(mm, mMuteOft, mMuteSize);
            else
                dest = lockImpl(mm, mMuteOft, mMuteSize);
                
            ::memcpy(dest, src, mMuteSize);

            unlock(UT_UnlockMap, mMuteOft, mMuteSize);
            mShadow->unlock(UT_UnlockMap);

            mShadowRefresh = false;
        }
    }
    //--------------------------------------------------------------------------------
    void * Buffer::lockImpl(MuteMark m, NCount oft, NCount size)
    {
    }
    //--------------------------------------------------------------------------------
    void Buffer::unlockImpl(UnlockType type, NCount oft, NCount size)
    {
    }
    //--------------------------------------------------------------------------------
    bool Buffer::reserveImpl(NCount size, BufferModeMark newMode, bool oldData)
    {
    }
    //---------------------------------------------------------------------------
    void Buffer::memcpyImpl(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
    }
    //--------------------------------------------------------------------------------
    void Buffer::waitImpl()
    {
    }
    //--------------------------------------------------------------------------------
    void Buffer::setSync(bool set)
    {
        if(set)
        {
            mMark |= M_SYN;
            syncImpl();
        }
        else
        {
            mMark &= ~M_SYN;
        }
    }
    //--------------------------------------------------------------------------------
    void Buffer::setMultiCount(NCount cnt)
    {
        mMutilCount = cnt;
    }
    //--------------------------------------------------------------------------------
    void Buffer::setOperation(BufferOp * op)
    {
        mOp = op;
        mOp->mBuffer = this;
    }
    //--------------------------------------------------------------------------------
    void Buffer::read(void * out, NCount oft, NCount size)
    {
        if(N_MarkTrue(mMark, M_HOSTREAD))
        {
            assert(mMapping == false);
            void * dstData = lock(MM_Read, oft, size);
            memcpy(out, dstData, size);
            unlock(UT_UnlockMap);
        }
        else
        {
            MappedBuffer * mapped;
            mMag->request(mapped, size, M_DEVWRITE);
            oft = mapped->asyncWrite(this, oft, size);

            void * dstData = mapped->readMap(size);
            memcpy(out, dstData + oft, size);
            mapped->unlock();

            mapped->unref();
        }
    }
    //--------------------------------------------------------------------------------
    OperationReason Buffer::readAsync(void * out, NCount oft, NCount size)
    {
        MappedBuffer * mapbuf;
        GpuBufferAsync * ayncbuf;
        
        mMag->request(mapbuf, size, M_DEVWRITE);
        mMag->create(ayncbuf, this, buffer, oft, size);
        
        OperationReason last = ayncbuf->isDataValid();
        if(last == OR_OK)
        {
            const void * src = ayncbuf->map();
            memcpy(out, src, size);
        }
        return last;
    }
    //--------------------------------------------------------------------------------
    void Buffer::write(const void * in, NCount oft, NCount size)
    {
        if(mShadow)
        {
            mShadow->write(in, oft, size);
        }

        if(N_MarkTrue(mMark, M_HOSTWRITE))
        {
            assert(mMapping == false);
            void * dstData = lock(Mm_WRITE, oft, size);
            memcpy(dstData, in, size);
            unlock(UT_UnlockMap);
        }
        else
        {
            writeAsync(in, oft, size);
        }
    }
    //--------------------------------------------------------------------------------
    void Buffer::write(Buffer * src, NCount srcOft, NCount oft, NCount size)
    {
        if(src->mShadow && mShadow)
        {
            mShadow->write(src->mShadow, srcOft, oft, size);
        }

        if(src->getOperation())
            src->getOperation()->wait();

        if(mOp)
        {
            mOp->wait();
            mOp->memcpyImpl(src, srcOft + src->mCurrentMulti * src->mReserveSize, oft + mCurrentMulti * mReserveSize, size);
        }
        else
        {
            memcpyImpl(src, srcOft + src->mCurrentMulti * src->mReserveSize, oft + mCurrentMulti * mReserveSize, size);
        }
    }
    //--------------------------------------------------------------------------------
    void Buffer::writeAsync(const void * in, NCount oft, NCount size)
    {
        MappedBuffer * mapped = mMag->request(size);

        void * dstData = mapped->writeMap(size);
        memcpy(dstData, in, size);
        mapped->unlock();

        mapped->asyncRead(this, oft, size);
        mapped->unref();
    }
    //--------------------------------------------------------------------------------
}