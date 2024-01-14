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
#include "NiiGBufferManager.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiLogManager.h"
#include "NiiGeometryPixel.h"
#include "NiiEngine.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GBufferManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    template<> GBufferManager * Singleton<GBufferManager>::mOnly = 0;
    //------------------------------------------------------------------------
    GBufferManager::GBufferManager():
        mForcePoolGroup(true),
        //mDefaultStagingBufferUnfencedTime( 300000 - 1000 ), //4 minutes, 59 seconds
        //mDefaultStagingBufferLifetime( 300000 ), // 5 minutes
        //mDynamicBufferMultiplier( 3 ),
        //mFrameMark( 0 ),
        mNextCheck( ~0 ),
        mMaxVertexAttribs(16),
        mFrameCount(0),
        mValidVAO(0),
        mAlignment[GBT_Struct](256),
        mAlignment[GBT_Texture](256),
        mAlignment[GBT_Storage](256),
        mMaxSize[GBT_Struct](16 * 1024 * 1024), //Minimum guaranteed by GL.
        mMaxSize[GBT_Texture](64 * 1024 * 1024),  //Minimum guaranteed by GL. Intel HD Graphics 3000-5000/Iris provide 64M only
        mMaxSize[GBT_Storage](16 * 1024 * 1024)    //Minimum guaranteed by GL.
    {
        mTimer = N_new Timer();
    }
    //------------------------------------------------------------------------
    GBufferManager::~GBufferManager()
    {
        MappedBufferList::const_iterator itor = mRefMappedList.begin();
        MappedBufferList::const_iterator end  = mRefMappedList.end();

        while(itor != end)
        {
            N_delete *itor;
            ++itor;
        }

        itor = mFreeMappedList.begin();
        end  = mFreeMappedList.end();

        while(itor != end)
        {
            N_delete *itor;
            ++itor;
        }

        N_delete mTimer;
        mTimer = 0;

        unsetupAllVAO();
        destroyAll();
    }
    //------------------------------------------------------------------------
    bool GBufferManager::addPool(GpuGroupID gid, BufferModeMark mode, StrategyType stype, NCount size)
    {
        GpuGroupList::iterator i, iend = mGpuGroupList.end();
        for(i = mGpuGroupList.begin(); i != iend; ++i)
        {
            if(*i == gid)
                return false;
        }

        return createPoolImpl(gid, mode, stype, size);
    }
    //------------------------------------------------------------------------
    bool GBufferManager::removePool(GpuGroupID gid)
    {
        GpuGroupList::iterator i, iend = mGpuGroupList.end();
        for(i = mGpuGroupList.begin(); i != iend; ++i)
        {
            if(*i == gid)
            {
                destroyPoolImpl(gid);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::removeAllPool()
    {
        GpuGroupList::iterator i, iend = mGpuGroupList.end();
        for(i = mGpuGroupList.begin(); i != iend; ++i)
        {
            destroyPoolImpl(gid);
        }
        mGpuGroupList.clear();
    }
    //------------------------------------------------------------------------
    void GBufferManager::unsetupVAO(GeometryRaw * obj)
    {
        VAOList::iterator itor = VAOList.find(obj);

        if(itor != VAOList.end())
        {
            VAOList.erase(itor);
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::unsetupAllVAO()
    {
        VAOList::const_iterator itor = VAOList.begin();
        VAOList::const_iterator end  = VAOList.end();

        while(itor != end)
        {
            unsetupVAO(*itor);
            ++itor;
        }

        VAOList.clear();
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(VertexBuffer * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(VertexBuffer * obj)
    {
        VertexBufferList::iterator itor = mVertexBuffers.find(obj);
        if(itor != mVertexBuffers.end())
        {
            if(N_MarkTrue(obj->mMark, M_NORMAL))
            {
                Recover rec(obj, mFrameCount, mFrameMark);
                mRecoverList.push_back(rec);
            }
            else
            {
                N_delete obj;
            }
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(IndexBuffer * obj)
    {
        IndexBufferList::iterator itor = mIndexBuffers.find(obj);
        if(itor != mIndexBuffers.end())
        {
            if(N_MarkTrue(obj->mMark, M_NORMAL))
            {
                Recover rec(obj, mFrameCount, mFrameMark);
                mRecoverList.push_back(rec);
            }
            else
            {
                N_delete obj;
            }
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(CounterBuffer * obj)
    {
        CounterBufferList::iterator itor = mCounterBufferList.find(obj);
        if(itor != mCounterBufferList.end())
        {
            if(N_MarkTrue(obj->mMark, M_NORMAL))
            {
                Recover rec(obj, mFrameCount, mFrameMark);
                mRecoverList.push_back(rec);
            }
            else
            {
                N_delete obj;
            }
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(StructBuffer * obj)
    {
        StructBufferList::iterator itor = mStructBufferList.find(obj);
        if(itor != mStructBufferList.end())
        {
            if(N_MarkTrue(obj->mMark, M_NORMAL))
            {
                Recover rec(obj, mFrameCount, mFrameMark);
                mRecoverList.push_back(rec);
            }
            else
            {
                N_delete obj;
            }
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(TextureBuffer * obj)
    {
        TextureBufferList::iterator itor = mTextureBufferList.find(obj);
        if(itor != mTextureBufferList.end())
        {
            if(N_MarkTrue(obj->mMark, M_NORMAL))
            {
                Recover rec(obj, mFrameCount, mFrameMark);
                mRecoverList.push_back(rec);
            }
            else
            {
                N_delete obj;
            }
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(StorageBuffer * obj)
    {
        StorageBufferList::iterator itor = mStorageBufferList.find(obj);
        if(itor != mStorageBufferList.end())
        {
            N_delete *itor;
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(IndirectBuffer * obj)
    {
        IndirectBufferList::iterator itor = mIndirectBufferList.find(obj);
        if(itor != mIndirectBufferList.end())
        {
            if(N_MarkTrue(obj->mMark, M_NORMAL))
            {
                Recover rec(obj, mFrameCount, mFrameMark);
                mRecoverList.push_back(rec);
            }
            else
            {
                N_delete obj;
            }
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(DisplayBuffer * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    void GBufferManager::create(VertexData *& out)
    {
        N_lock_mutex(mVertexDataListMutex)
        createImpl(out);
        mVertexDataList.insert(out);
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(VertexData * obj)
    {
        N_lock_mutex(mVertexDataListMutex)
        mVertexDataList.erase(obj);
        destroyImpl(obj);
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroyAll()
    {
        VertexBufferList::iterator i1, iend1 = mVertexBuffers.end();
        for (i1 = mVertexBuffers.begin(); i1 != iend1; ++i1)
        {
            N_delete *i1;
        }
        mVertexBuffers.clear();

        IndexBufferList::iterator i2, iend2 = mIndexBuffers.end();
        for (i2 = mIndexBuffers.begin(); i2 != iend2; ++i2)
        {
            N_delete *i2;
        }
        mIndexBuffers.clear();

        CounterBufferList::iterator i6, iend6 = mCounterBufferList.end();
        for (i6 = mCounterBufferList.begin(); i6 != iend6; ++i6)
        {
            N_delete *i6;
        }
        mCounterBufferList.clear();
        
        StructBufferList::iterator i5, iend5 = mStructBufferList.end();
        for (i5 = mStructBufferList.begin(); i5 != iend5; ++i5)
        {
            N_delete *i5;
        }
        mStructBufferList.clear();
        
        IndirectBufferList::iterator i7, iend7 = mIndirectBufferList.end();
        for (i7 = mIndirectBufferList.begin(); i7 != iend7; ++i7)
        {
            N_delete *i7;
        }
        mIndirectBufferList.clear();

        TextureBufferList::iterator i8, iend8 = mTextureBufferList.end();
        for (i8 = mTextureBufferList.begin(); i8 != iend8; ++i8)
        {
            N_delete *i8;
        }
        mTextureBufferList.clear();

        StorageBufferList::iterator i9, iend9 = mStorageBufferList.end();
        for (i9 = mStorageBufferList.begin(); i9 != iend9; ++i9)
        {
            N_delete *i9;
        }
        mStorageBufferList.clear();

        VertexDataList::iterator i3, iend3 = mVertexDataList.end();
        for (i3 = mVertexDataList.begin(); i3 != iend3; ++i3)
        {
            destroyImpl(*i3);
        }
        mVertexDataList.clear();

        GeometryPixelList::iterator i4, iend4 = mGeoPixelList.end();
        for (i4 = mGeoPixelList.begin(); i4 != iend4; ++i4)
        {
            destroy(*i4);
        }
        mGeoPixelList.clear();

        {
            RecoverList::const_iterator itor = mRecoverList.begin();
            RecoverList::const_iterator end  = mRecoverList.end();

            while(itor != end)
            {
                N_delete itor->mBuffer;
                ++itor;
            }

            mRecoverList.clear();
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::create(IndexData *& out)
    {
        out = N_new IndexData(); 
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(IndexData * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    void GBufferManager::create(IndirectData *& out, IndirectBuffer::Type type)
    {
        out = N_new IndirectData(type); 
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(IndirectData * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    void GBufferManager::create(GeometryPixel *& out)
    {
        N_lock_mutex(mGeoPixelListMutex)
        out = N_new GeometryPixel(N_Engine().getRender());
        mGeoPixelList.insert(out);
    }
    //------------------------------------------------------------------------
    void GBufferManager::destroy(GeometryPixel * obj)
    {
        N_lock_mutex(mGeoPixelListMutex)
        mGeoPixelList.erase(obj);
        N_delete obj;
    }
    //------------------------------------------------------------------------
    void GBufferManager::onDestroy(VertexBuffer * obj)
    {
        N_lock_mutex(mVertexBuffersMutex)
        VertexBufferList::iterator i = mVertexBuffers.find(obj);
        if (i != mVertexBuffers.end())
        {
            mVertexBuffers.erase(i);
            recoverTemp(obj);
        }
    }
    //-----------------------------------------------------------------------
    void GBufferManager::onDestroy(IndexBuffer * obj)
    {
        N_lock_mutex(mIndexBuffersMutex)
        IndexBufferList::iterator i = mIndexBuffers.find(obj);
        if(i != mIndexBuffers.end())
        {
            mIndexBuffers.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void GBufferManager::onDestroy(CounterBuffer * obj)
    {
        N_lock_mutex(mCounterBuffersMutex)
        CounterBufferList::iterator i = mCounterBufferList.find(obj);
        if (i != mCounterBufferList.end())
        {
            mCounterBufferList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void GBufferManager::onDestroy(StructBuffer * obj)
    {
        N_lock_mutex(mStructBuffersMutex)
        StructBufferList::iterator i = mStructBufferList.find(obj);
        if (i != mStructBufferList.end())
        {
            mStructBufferList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::onDestroy(IndirectBuffer * obj)
    {
        N_lock_mutex(mIndirectBuffersMutex)
        IndirectBufferList::iterator i = mIndirectBufferList.find(obj);
        if (i != mIndirectBufferList.end())
        {
            mIndirectBufferList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::onDestroy(TextureBuffer * obj)
    {
        N_lock_mutex(mTextureBuffersMutex)
        TextureBufferList::iterator i = mTextureBufferList.find(obj);
        if (i != mTextureBufferList.end())
        {
            mTextureBufferList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::onDestroy(StorageBuffer * obj)
    {
        N_lock_mutex(mShaderBuffersMutex)
        StorageBufferList::iterator i = mStorageBufferList.find(obj);
        if (i != mStorageBufferList.end())
        {
            mStorageBufferList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::onDestroy(GeometryPixel * obj)
    {
        N_lock_mutex(mGeoPixelListMutex)
        GeometryPixelList::iterator i = mGeoPixelList.find(obj);
        if(i != mGeoPixelList.end())
        {
            mGeoPixelList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void GBufferManager::onRecover(StrategyType type, FrameID frame, QueueID qid)
    {

    }
    //-----------------------------------------------------------------------------------
    void GBufferManager::request(MappedBuffer *& out, NCount size, BufferModeMark modemark)
    {
        MappedBuffer *candidates[BT_Count];
        memset(candidates, 0, sizeof(candidates));

        MappedBufferList::const_iterator itor = mFreeMappedList.begin();
        MappedBufferList::const_iterator end  = mFreeMappedList.end();

        while(itor != end)
        {
            if(size <= (*itor)->getMaxSize())
            {
                BlockType stallType = (*itor)->request(size, modemark);
                    
                if(forUpload)
                    candidates[stallType] = *itor;

                if(stallType == BT_None)
                {
                    if(!forUpload)
                        candidates[0] = *itor;
                    break;
                }
            }

            ++itor;
        }

        out = candidates[BT_All];

        for(size_t i = 0; i < BT_Count && !out; ++i)
            out = candidates[i];

        if(out)
        {
            out->addRef();
        }
        else
        {
            createImpl(out, minSizeBytes, modemark);
        }
    }
    //-----------------------------------------------------------------------------------
    void GBufferManager::destroyRecover(Nui32 frameMark)
    {
        RecoverList::iterator itor = mRecoverList.begin();
        RecoverList::iterator end  = mRecoverList.end();

        while(itor != end)
        {
            if(itor->mFrameMark != frameMark || itor->frame == mFrameCount)
                break;

            N_assert(mFrameCount - itor->frame == mDynamicBufferMultiplier &&
                    "Delayed buffer must be destroyed in the last buffered frame!");

            N_delete itor->mBuffer;

            ++itor;
        }

        mRecoverList.erase(mRecoverList.begin(), itor);
    }
    //-----------------------------------------------------------------------------------
    void GBufferManager::destroyAllRecover()
    {
        RecoverList::iterator itor = mRecoverList.begin();
        RecoverList::iterator end  = mRecoverList.end();

        while(itor != end)
        {
            N_delete itor->mBuffer;
            ++itor;
        }

        mRecoverList.clear();
    }
    //-----------------------------------------------------------------------------------
    void GBufferManager::_update()
    {
        Root::getSingleton()._renderingFrameEnded();
        ++mFrameCount;
    }
    //-----------------------------------------------------------------------------------
    void GBufferManager::onUnRef(MappedBuffer * buf)
    {
        MappedBufferList::iterator itor = std::find(mRefMappedList.begin(), mRefMappedList.end(), buf);

        assert(itor != mRefMappedList.end());
        remove(mRefMappedList, itor);

        mFreeMappedList.push_back(buf);
    }
    //-----------------------------------------------------------------------------------
    void GBufferManager::onRef(MappedBuffer * buf)
    {
        MappedBufferList::iterator itor = std::find(mFreeMappedList.begin(), mFreeMappedList.end(), buf);

        assert(itor != mFreeMappedList.end());
        remove(mFreeMappedList, itor);

        mRefMappedList.push_back(buf);
    }
    //-----------------------------------------------------------------------------------
    /**

    struct _EngineAPI MemoryStatsEntry
    {
        MemoryStatsEntry( uint32 type, NCount oft, NCount size, NCount reserve ) :
            poolType( type ), offset( oft ),
            sizeBytes( size ), poolCapacity( reserve ) {}

        uint32 poolType;
        NCount offset;
        NCount sizeBytes;
        NCount poolCapacity; /// This value is the same for all entries with same poolType & poolIdx
    };

    typedef vector<MemoryStatsEntry>::type MemoryStatsList;

    virtual void getMemoryStats(MemoryStatsList & outStats, size_t & outCapacityBytes, size_t & outFreeBytes, Log * log) const = 0;
    void GBufferManager::setDefaultStagingBufferlifetime( uint32 lifetime, uint32 unfencedTime )
    {
        if( unfencedTime > lifetime )
        {
            N_EXCEPT(Internal, "unfencedTime must be equal or lower than lifetime");
        }

        mDefaultStagingBufferLifetime       = lifetime;
        mDefaultStagingBufferUnfencedTime   = unfencedTime;
    }

    inline uint32 getDefaultStagingBufferUnfencedTime() const       { return mDefaultStagingBufferUnfencedTime; }

    inline uint32 getDefaultStagingBufferLifetime() const           { return mDefaultStagingBufferLifetime; }*/
    //-----------------------------------------------------------------------------------
}
