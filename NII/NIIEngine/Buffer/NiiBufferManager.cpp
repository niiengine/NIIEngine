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
#include "NiiBufferManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TempBufferCtl
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TempBufferCtl::TempBufferCtl(bool autoDsy) :
        mAutoDestroy(autoDsy)
    {
    }
    //------------------------------------------------------------------------
    TempBufferCtl::~TempBufferCtl()
    {
    }
    //------------------------------------------------------------------------
    void TempBufferCtl::task(Nmark)
    {
    }
    //------------------------------------------------------------------------
    void TempBufferCtl::expire(Buffer *)
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner TempBufferCtlPrc : public BufferAlloc
    {
    public:
        TempBufferCtlPrc(const Buffer * src, NCount delay, Buffer * buf, TempBufferCtl * lic) :
            mSrc(src),
            mThreshold(delay),
            mCopy(buf),
            mLic(lic)
        {
        }
        const Buffer * mSrc;    ///< 源头缓存
        Buffer * mCopy;         ///< 副本缓存
        TempBufferCtl * mLic;   ///< 这里面的 desbuffer 过期后应该与上面的 mCopy 相同
        NCount mThreshold;      ///< 控制释放时间
    };
    //------------------------------------------------------------------------
    // 100fps每30秒
    #define GBufferManager_FRAME_THRESHOLD 3000
    // 70fps每1秒
    #define GBufferManager_THRESHOLD_MARK 70
    //------------------------------------------------------------------------
    BufferManager::BufferManager() :
        mFrameMark(0)
    {
    }
    //------------------------------------------------------------------------
    BufferManager::~BufferManager()
    {
        recover();
    }
    //------------------------------------------------------------------------
    Buffer * BufferManager::allocTemp(const Buffer * obj, TempBufferCtl * lic)
    {
        N_lock_mutex(mTempBuffersMutex)

        Buffer * re;
        CopyBuffers::iterator i = mCopyBuffers.find(obj);
        if(i == mCopyBuffers.end())
        {
            re = obj->clone();
        }
        else
        {
            re = i->second;
            mCopyBuffers.erase(i);
        }

        TempBufferCtlPrc * temp = N_new TempBufferCtlPrc(obj, GBufferManager_THRESHOLD_MARK, re, lic);
        mTempBuffers.insert(Npair(re, temp));
        return re;
    }
    //------------------------------------------------------------------------
    void BufferManager::freeTemp(Buffer * obj)
    {
        N_lock_mutex(mTempBuffersMutex)

        TempBuffers::iterator i = mTempBuffers.find(obj);
        if(i != mTempBuffers.end())
        {
            TempBufferCtlPrc * prc = i->second;
            prc->mLic->expire(prc->mCopy);
            mCopyBuffers.insert(CopyBuffers::value_type(prc->mSrc, prc->mCopy));
            N_delete prc;
            mTempBuffers.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void BufferManager::recoverTemp(const Buffer * source)
    {
        N_lock_mutex(mTempBuffersMutex)

        TempBuffers::iterator i = mTempBuffers.begin();
        while(i != mTempBuffers.end())
        {
            TempBuffers::iterator icur = i++;
            TempBufferCtlPrc * prc = icur->second;
            if(prc->mSrc == source)
            {
                prc->mLic->expire(prc->mCopy);
                N_delete prc;
                mTempBuffers.erase(icur);
            }
        }

        typedef CopyBuffers::iterator CopyBufferIter;
        std::pair<CopyBufferIter, CopyBufferIter> range = mCopyBuffers.equal_range(source);
        if(range.first != range.second)
        {
            for(CopyBufferIter it = range.first; it != range.second; ++it)
            {
                N_delete it->second;
            }
            mCopyBuffers.erase(range.first, range.second);
        }
    }
    //------------------------------------------------------------------------
    void BufferManager::refillTemp(Buffer * obj)
    {
        N_lock_mutex(mTempBuffersMutex)

        TempBuffers::iterator i = mTempBuffers.find(obj);
        if(i != mTempBuffers.end())
        {
            TempBufferCtlPrc * tbcp = i->second;
            N_assert(tbcp->mLic->isAutoDestroy() == true, "error");
            tbcp->mThreshold = GBufferManager_THRESHOLD_MARK;
        }
    }
    //-----------------------------------------------------------------------
    void BufferManager::recoverTemp(TimeDurMS cost, bool _recover)
    {
        N_lock_mutex(mTempBuffersMutex)

        NCount numUnused = mCopyBuffers.size();
        NCount numUsed = mTempBuffers.size();

        TempBuffers::iterator i = mTempBuffers.begin();
        while(i != mTempBuffers.end())
        {
            TempBuffers::iterator icur = i++;
            TempBufferCtlPrc * tbcp = icur->second;
            if((tbcp->mLic->isAutoDestroy() == true) && (_recover || --tbcp->mThreshold <= 0))
            {
                tbcp->mLic->expire(tbcp->mCopy);
                mCopyBuffers.insert(CopyBuffers::value_type(tbcp->mSrc, tbcp->mCopy));
                N_delete tbcp;
                mTempBuffers.erase(icur);
            }
        }

        // 测试是否释放没有使用过的临时顶点缓存
        if(_recover)
        {
            recover();
            mFrameMark = 0;
        }
        else
        {
            if(numUsed < numUnused)
            {
                ++mFrameMark;
                if(mFrameMark >= GBufferManager_FRAME_THRESHOLD)
                {
                    recover();
                    mFrameMark = 0;
                }
            }
            else
            {
                mFrameMark = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    void BufferManager::recover(Buffer * obj)
    {
        N_lock_mutex(mBufferRecoverMutex)
        mRecoverList.push_back(obj);
    }
    //------------------------------------------------------------------------
    void BufferManager::recover()
    {
        {
            N_lock_mutex(mBufferRecoverMutex)
            BufferList::iterator i, iend = mRecoverList.end();
            for(i = mRecoverList.begin(); i != iend; ++i)
            {
                N_delete *i;
            }
            mRecoverList.clear();
        }
        {
            N_lock_mutex(mTempBuffersMutex)

            CopyBuffers::iterator i = mCopyBuffers.begin();
            while(i != mCopyBuffers.end())
            {
                CopyBuffers::iterator icur = i++;
                N_delete icur->second;
                mCopyBuffers.erase(icur);
            }
        }
    }
    //------------------------------------------------------------------------
}