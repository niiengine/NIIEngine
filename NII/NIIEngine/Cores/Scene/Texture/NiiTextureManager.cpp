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
#include "NiiTextureManager.h"
#include "NiiLogManager.h"
#include "NiiGpuBuffer.h"
#include "NiiException.h"
#include "NiiPixelFormat.h"
#include "NiiDefaultStream.h"
#include "NiiEngine.h"
#include "NiiShadowMappingPattern.h"
#include "NiiResourceSchemeManager.h"
#include "NiiStrConv.h"

#if !OGRE_NO_JSON
    #include "rapidjson/document.h"
    #include "rapidjson/error/en.h"
    #include "OgreStringConverter.h"
#endif

#define TODO_grow_pool 1

#define reuse_count 15

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TextureDefine
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TextureDefine::TextureDefine() :
        mWidth(512),
        mHeight(512),
        mFormat(PF_X8R8G8B8),
        mFSAA(0),
        mGroup(1)
    {
    }
    //------------------------------------------------------------------------
    bool TextureDefine::operator== (const TextureDefine & o)
    {
        if(mWidth != o.mWidth || mHeight != o.mHeight || mFormat != o.mFormat)
        {
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool TextureDefine::operator!= (const TextureDefine & o)
    {
        if(mWidth != o.mWidth || mHeight != o.mHeight || mFormat != o.mFormat)
        {
            return true;
        }
        return false;
    }
    /** This is a Default implementation of TextureGpuManagerListener based on heuristics.
        These heuristics can be adjusted by modifying its member variables directly.
    */
    class _EngineAPI TextureGpuManagerListener : public Texture::Process
    {
    public:
        TextureGpuManagerListener() : mPackNonPow2(false)
        {
            mMinSlicesPerPool[0] = 16;
            mMinSlicesPerPool[1] = 8;
            mMinSlicesPerPool[2] = 4;
            mMinSlicesPerPool[3] = 2;
            mMaxResolutionToApplyMinSlices[0] = 256;
            mMaxResolutionToApplyMinSlices[1] = 512;
            mMaxResolutionToApplyMinSlices[2] = 1024;
            mMaxResolutionToApplyMinSlices[3] = 4096;
        }

        virtual size_t getNumSlicesFor(Texture * tex, TextureManager * mag);
        {
            uint32 maxResolution = std::max(tex->getWidth(), tex->getHeight());
            uint16 minSlicesPerPool = 1u;

            for(int i = 0; i < 4; ++i)
            {
                if(maxResolution <= mMaxResolutionToApplyMinSlices[i])
                {
                    minSlicesPerPool = mMinSlicesPerPool[i];
                    break;
                }
            }

            if(!mPackNonPow2)
            {
                if(!Bitwise::isPO2(tex->getWidth()) || !Bitwise::isPO2(tex->getHeight()))
                    minSlicesPerPool = 1u;
            }

            return minSlicesPerPool;
        }
        virtual void process(const Item & item){}
        
        /// Minimum slices per pool, regardless of maxBytesPerPool.
        /// It's also the starting num of slices. See mMaxResolutionToApplyMinSlices
        uint16 mMinSlicesPerPool[4];

        /// If texture resolution is <= maxResolutionToApplyMinSlices[i];
        /// we'll apply minSlicesPerPool[i]. Otherwise, we'll apply minSlicesPerPool[i+1]
        /// If resolution > mMaxResolutionToApplyMinSlices[N]; then minSlicesPerPool = 1;
        uint32 mMaxResolutionToApplyMinSlices[4];

        /// Whether non-power-of-2 textures should also be pooled, or we should return 1.
        bool mPackNonPow2;
    };
    //------------------------------------------------------------------------
    namespace TextureFilter
    {
        class FilterBase;
    }

    typedef vectorL<TextureFilter::FilterBase *> FilterBaseArray;

    class _EngineAPI TextureOpBuffer : public ResourceAlloc
    {
    public:
        class Cmd
        {
        public:
            virtual ~Cmd() {}
            virtual void execute() = 0;
        };
    protected:
        vectorL<Nui8> mCommandAllocator;
        vectorL<Cmd *> mCommandBuffer;

        void * requestMemory(size_t sizeBytes)
        {
            void * retVal = 0;
            const size_t newSize = mCommandAllocator.size() + sizeBytes;

            if( newSize > mCommandAllocator.capacity() )
            {
                const Nui8 * prevBuffStart = mCommandAllocator.begin();
                mCommandAllocator.resize( newSize );
                Nui8 * newBuffStart = mCommandAllocator.begin();

                vectorL<Cmd *>::iterator it = mCommandBuffer.begin();
                vectorL<Cmd *>::iterator end  = mCommandBuffer.end();

                while( it != end )
                {
                    const uintptr_t ptrDiff = reinterpret_cast<Nui8 *>( *it ) - prevBuffStart;
                    *it = reinterpret_cast<Cmd *>( newBuffStart + ptrDiff );
                    ++it;
                }
            }
            else
            {
                mCommandAllocator.resize(newSize);
            }

            retVal = mCommandAllocator.end() - sizeBytes;

            return retVal;
        }
    public:
        ~TextureOpBuffer()
        {
            clear();
        }

        void clear(void)
        {
            vectorL<Cmd *>::const_iterator it = mCommandBuffer.begin();
            vectorL<Cmd *>::const_iterator end  = mCommandBuffer.end();

            while( it != end )
            {
                (*it)->~Cmd();
                ++it;
            }

            mCommandBuffer.clear();
            mCommandAllocator.clear();
        }

        void execute(void)
        {
            vectorL<Cmd *>::const_iterator it = mCommandBuffer.begin();
            vectorL<Cmd *>::const_iterator end  = mCommandBuffer.end();

            while( it != end )
            {
                (*it)->execute();
                ++it;
            }
        }

        template <typename T> T * addCommand()
        {
            T * newCmd = reinterpret_cast<T *>(requestMemory(sizeof(T)));
            mCommandBuffer.push_back(newCmd);
            return newCmd;
        }

        class TransitionToLoaded : public Cmd
        {
        public:
            TransitionToLoaded(Texture * tex, Nui8 * data, StorageType st):
                mTexture(tex),
                mData(data),
                mStorage(st)
            {
                N_assert1(mStorage != StorageType::ST_Disk);
            }

            virtual void execute()
            {
                mTexture->load( mStorage, mData);
                N_assert1( !mTexture->isManual() );

                //Do not update metadata cache when loading from ST_Disk to ST_Cpu as
                //it may have tainted (incomplete, mostly mipmaps) data. Only when going Resident.
                if( mStorage == StorageType::ST_Gpu )
                    N_Only(Texture)._updateMetadata( mTexture );
            }

            Nui8 * mData;
            Texture * mTexture;
            StorageType mStorage;
        };
       

        class OutOfDateCache : public Cmd
        {
        public:
            OutOfDateCache(Texture * tex, Image * image) :
                mTexture(tex),
                mSrc(image){}

            virtual void execute()
            {
                N_assert1( mTexture->getCurrentStorage() == StorageType::ST_Gpu );

                mTexture->process(Texture::Process::O_MetadataDirty, true);
                mTexture->schedule(Texture::Task(StorageType::ST_Reload, mSrc, true, false ,true));
            }

            Image * mSrc;
            Texture * mTexture;
        };

        class ExceptionThrown : public Cmd
        {
        public:
            ExceptionThrown(Texture * tex, const Exception & exception) :
                mTexture(tex),
                mException(exception){}
                
            virtual void execute()
            {
                mTexture->process(Texture::Process::O_Error, &exception);
            }

            Texture * mTexture;
            Exception mException;
        };

        class ReadFromMapped : public Cmd
        {
        public:
            ReadFromMapped(MappedTexture * mt, const PixelBlock & box, Texture * dstTex, const PixelBlock & dstBox, NCount mipLevel) :
                mMappedTexture(mt),
                mBox(box),
                mDstTexture(dstTex),
                mDstBox(dstBox),
                mMipMap(mipLevel){}

            virtual void execute()
            {
                //OgreProfileExhaustive( "TextureOpBuffer::ReadFromMapped::execute" );
                mMappedTexture->read(mDstTexture, mMipMap, &mDstBox, mBox, true);
                mMappedTexture->read(mDstTexture, mMipMap, &mDstBox, mBox);
            }

            MappedTexture * mMappedTexture;
            PixelBlock mBox;
            Texture * mDstTexture;
            PixelBlock mDstBox;
            NCount mMipMap;
        };

        class NotifyDataIsReady : public Cmd
        {
        public:
            NotifyDataIsReady(Texture * tex, FilterBaseArray & inOutFilters) :
                mTexture(tex)
            {
                mFilterList.swap(inOutFilters);
            }

            virtual void execute()
            {
                //OgreProfileExhaustive( "TextureOpBuffer::NotifyDataIsReady::execute" );

                FilterBaseArray::const_iterator it = mFilterList.begin();
                FilterBaseArray::const_iterator end  = mFilterList.end();

                while( it != end )
                {
                    (*it)->_executeSerial( mTexture );
                    OGRE_DELETE *it;
                    ++it;
                }
                mFilterList.destroy(); //Destroy manually as ~NotifyDataIsReady won't be called.

                mTexture->notifyGpuData();
            }
            
            Texture * mTexture;
            FilterBaseArray mFilterList;
        };
    };

    struct TextureRequest
    {
        TextureRequest(ResourceID rid, VFS * vfs, Image * image, Texture * tex, NCount volume,
            Nmark mark, bool autoimage, bool cpu) :
            mName(rid), mArchive(vfs), mImage(image), mTexture(tex),
            mVolume(volume), mMark(mark), mAutoImage(autoimage), mCpuData(cpu) {}
                
        String mName;
        VFS * mArchive;
        Image * mImage;
        Texture * mTexture;
        NCount mVolume;
        Nmark mMark;
        bool mAutoImage;
        bool mCpuData;
    };

    typedef vector<TextureRequest>::type RequestList;
        
    struct ThreadData
    {
        TextureOpBuffer * mOpBuffer;
        MappedTextureList mMappedList;
        RequestList mRequestList;
    };
    
    /**
    @class ImageSlice
        When loading a texture (i.e. TextureManager::scheduleLoadRequest the following will happen:

        1. Find if the texture has any pending transition or finishing loading
            1a. If so place a task in mTextureTaskList
            1b. Otherwise execute the task immediately
        2. The texture is placed from main thread in mThreadData[].loadRequests
        3. Worker thread parses that request. Assuming all goes smoothly (e.g. Image loaded
            successfully, metadata cache was not out of date, etc) the Image is fully
            loaded from disk into memory as an Image.
        4. The texture is transitioned to Resident* since all metadata is available now,
            but not yet ready for rendering. This step cannot happen if we must maintain a
            SystemRam copy (i.e. strategy is CPUGPU; or we were asked
            to load to ST_Cpu, instead of Resident)
        6. A ImageSlice is created.
        7. The TextureRequest is destroyed.
        8. Worker thread loops again, processing QueuedImages and LoadRequests

        ImageSlice will proceed to transfer from system RAM (step 3) to StagingTextures, and
        will remain alive until all of its data has been transfered. It will also be
        responsible for requesting more StagingTextures if needed.

        Once the ImageSlice is done, it will signal a TextureOpBuffer::NotifyDataIsReady.

        If step 4 could not happen, then the ImageSlice will also perform the transition
        to resident* when we're done.

        (*) Transitions happen in main thread, however they're requested from worker thread
        via TextureOpBuffer::TransitionToLoaded. See TextureManager::addTransitionToLoadedCmd code

    @remarks
        ImageSlice class is used when is NEVER used when transitioning from ST_Disk -> ST_Cpu

        There's a case in which the texture must be loaded in parts and thus more than one
        ImageSlice will be needed. See TextureManager::TextureSlice.

        In this case, then NotifyDataIsReady (and TransitionToLoaded if step 4 could not run)
        can only happen after all pending ImageSlice are over.
    */
    struct ImageSlice
    {
        ImageSlice(Image & srcImage, NCount numMips, NCount array, Texture * dsttex,
            NCount dstVolume, FilterBaseArray & inOutFilters):
                mDstTexture(dsttex),
                mArray(array),
                mAutoImage(srcImage.getAutoDelete()),
                mDstVolume(dstVolume)
        {
            N_assert1( mArray >= 1u );

            mFilterList.swap(inOutFilters);

            //Prevent destroying the internal data in srcImage if ImageSliceList
            //holding us gets resized. (we do not follow the rule of 3)
            srcImage._setAutoDelete(false);
            mImage = srcImage;

            NCount numMipSlices = numMips * mArray;

            N_assert1(numMipSlices < 256u);

            for(int i = 0; i < 4; ++i)
            {
                if(numMipSlices >= 64u)
                {
                    mMipMapBit[i] = 0xffffffffffffffff;
                    numMipSlices -= 64u;
                }
                else
                {
                    mMipMapBit[i] = (uint64(1ul) << numMipSlices) - uint64(1ul);
                    numMipSlices = 0;
                }
            }

            if(srcImage.getTextureType() == Texture::T_3D)
            {
                //For 3D textures, depth is not constant per mip level. If we don't unqueue those
                //now we will later get stuck in an infinite loop (empty() will never return true)
                uint32 currDepth = std::max<uint32>(mArray >> 1u, 1u);
                for(Nui8 mip = 1u; mip < numMips; ++mip)
                {
                    for(uint32 slice = currDepth; slice < mArray; ++slice)
                        unqueueMipSlice(mip, static_cast<Nui8>(slice));
                    currDepth = std::max<uint32>( currDepth >> 1u, 1u);
                }
            }
        }

        void destroy()
        {
            if( mAutoImage && mDstTexture->getRecoverType() != Texture::RT_CpuGpu )
            {
                //Do not delete the internal pointer if the Texture will be owning it.
                mImage._setAutoDelete( true );
                mImage.freeMemory();
            }

            N_assert( mFilterList.empty(), "Internal Error: Failed to send mFilterList to the main thread for destruction. These mFilterList will leak" );
        }

        inline bool empty() const
        {
            return mMipMapBit[0] == 0ul && mMipMapBit[1] == 0ul && mMipMapBit[2] == 0ul && mMipMapBit[3] == 0ul;
        }

        bool isMipSliceQueued(Nui8 mipLevel, Nui8 slice) const
        {
            uint32 mipSlice = mipLevel * mArray + slice;
            size_t idx  = mipSlice / 64u;
            uint64 mask = mipSlice % 64u;
            mask = ((uint64)1ul) << mask;
            return (mMipMapBit[idx] & mask) != 0;
        }

        void unqueueMipSlice(Nui8 mipLevel, Nui8 slice)
        {
            uint32 mipSlice = mipLevel * mArray + slice;
            size_t idx  = mipSlice / 64u;
            uint64 mask = mipSlice % 64u;
            mask = ((uint64)1ul) << mask;
            mMipMapBit[idx] = mMipMapBit[idx] & ~mask;
        }

        NCount getMinMipLevel() const
        {
            if( mMipMapBit[0] != 0u )
            {
                NCount firstBitSet = static_cast<NCount>( Bitwise::ctz64( mMipMapBit[0] ) );
                return (firstBitSet + 64u * 0) / mArray;
            }
            if( mMipMapBit[1] != 0u )
            {
                NCount firstBitSet = static_cast<NCount>( Bitwise::ctz64( mMipMapBit[1] ) );
                return (firstBitSet + 64u * 1) / mArray;
            }
            if( mMipMapBit[2] != 0u )
            {
                NCount firstBitSet = static_cast<NCount>( Bitwise::ctz64( mMipMapBit[2] ) );
                return (firstBitSet + 64u * 2) / mArray;
            }
            if( mMipMapBit[3] != 0u )
            {
                NCount firstBitSet = static_cast<NCount>( Bitwise::ctz64( mMipMapBit[3] ) );
                return (firstBitSet + 64u * 3) / mArray;
            }

            return 255u;
        }

        NCount getMaxMipLevelPlusOne() const
        {
            if( mMipMapBit[3] != 0u )
            {
                NCount lastBitSet = static_cast<NCount>( 64u - Bitwise::clz64( mMipMapBit[3] ) + 64u * 3 );
                return (lastBitSet + mArray - 1u) / mArray;
            }
            
            if( mMipMapBit[2] != 0u )
            {
                NCount lastBitSet = static_cast<NCount>( 64u - Bitwise::clz64( mMipMapBit[2] ) + 64u * 2 );
                return (lastBitSet + mArray - 1u) / mArray;
            }
            
            if( mMipMapBit[1] != 0u )
            {
                NCount lastBitSet = static_cast<NCount>( 64u - Bitwise::clz64( mMipMapBit[1] ) + 64u * 1 );
                return (lastBitSet + mArray - 1u) / mArray;
            }
            
            if( mMipMapBit[0] != 0u )
            {
                NCount lastBitSet = static_cast<NCount>( 64u - Bitwise::clz64( mMipMapBit[0] ) + 64u * 0 );
                return (lastBitSet + mArray - 1u) / mArray;
            }

            return 0u;
        }

        Image mImage;
        uint64 mMipMapBit[4];
        Texture * mDstTexture;
        NCount mArray;
        NCount mDstVolume;
        
        FilterBaseArray mFilterList;
        bool mAutoImage;
    };

    typedef vector<ImageSlice>::type ImageSliceList;
    
    struct MappedUsage
    {
        MappedUsage(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf) :
            mWidth(width),
            mHeight(height),
            mFormat(pf),
            mSize(PixelUtil::getSize(width, height, depth, array, pf)),
            mLoopCount(0){}
            
        NCount mWidth;
        NCount mHeight;
        NCount mSize;
        NCount mLoopCount;
        PixelFormat mFormat;
    };
    typedef vector<MappedUsage>::type MappedUsageList;  
    
    struct TextureSlice
    {
        TextureSlice() : mData(0), mArray(0), mCpuData(false) {}
        TextureSlice(void * data, bool cpu) : mData(data), mArray(0), mCpuData(cpu) {}

        void * mData;
        NCount mArray;
        bool mCpuData;
    };

    typedef map<Texture *, TextureSlice>::type TextureSliceList;
    
    struct TextureThreadData
    {
        ThreadData mThreadData[2];
        MappedTextureList mFreeMappedList;
        ImageSliceList mImageList;
        TextureSliceList mTextureSliceList;
        MappedUsageList mMappedUsage;
        MappedUsageList mPreMappedUsage;
        size_t mProcessSize;
        size_t mMappedMaxDim;
        size_t mMappedMaxSize;

        /// Resheduled textures are textures which were transitioned to Resident
        /// preemptively using the metadata cache, but it turned out to be wrong
        /// (out of date), so we need to do some ping pong first
        ///
        /// Used by worker thread. No protection needed.
        set<Texture *>::type mReTaskList;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TexturePool
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool TexturePool::isFree() const
    {
        return !mFreeList.empty() || mNextSlice < mPool->getArray();
    }
    //------------------------------------------------------------------------
    bool TexturePool::empty() const
    {
        const size_t array = mPool->getArray();
        return (mFreeList.size() + (array - mNextSlice)) == array;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MappedPool
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool MappedPool::operator () (const MappedPool & _l, const MappedPool & _r) const
    {
        //Biggest ones come first
        const size_t lSize = PixelUtil::getSize(_l.mDim, _l.mDim, 1u, _l.mArray, _l.mFormat);
        const size_t rSize = PixelUtil::getSize(_r.mDim, _r.mDim, 1u, _r.mArray, _r.mFormat);
        return lSize > rSize;
    }
    //------------------------------------------------------------------------
    template<> TextureManager * Singleton<TextureManager>::mOnly = 0;
    //------------------------------------------------------------------------
    #define MainThread 0
    #define WorkerThread 1
    //------------------------------------------------------------------------
    static TextureGpuManagerListener sDefaultTextureGpuManagerListener;
    //------------------------------------------------------------------------
    NCount TextureManager_getSize(const MappedTextureList & mappedlist) const
    {
        NCount re = 0;
        MappedTextureList::const_iterator it = mappedlist.begin();
        MappedTextureList::const_iterator end  = mappedlist.end();
        while( it != end )
        {
            re += (*it)->getSize();
            ++it;
        }

        return re;
    }
    //------------------------------------------------------------------------
    TextureManager::TextureManager(GBufferManager * mag) :
        mMipmapGen(MipmapGenType::HwMode),
        mMipmapGenCube(MipmapGenType::SwMode),
        mShuttingDown(false),
        mMaxProcessRequest(3u),
#if N_PLAT != N_PLAT_IOS && N_PLAT != N_PLAT_ANDROID && N_ARCH != N_ARCH_32
        mMappedPoolMaxSize(256u * 1024u * 1024u),
#else
        mMappedPoolMaxSize(128u * 1024u * 1024u),
#endif
        mDoProcess(false),
        ResourceManager(RT_Texture),
        mBitDepth(0),
        mMipMapCount(N_TEX_MIPMAP)
    {
        mPlugin = 0;
        mScriptLevel = 75;
        // Subclasses should register (when this is fully constructed)
        memset( mDummyData, 0, sizeof( mDummyData ) );

#if N_PLAT != N_PLAT_IOS && N_PLAT != N_PLAT_ANDROID
    #if N_ARCH == N_ARCH_32
        // 32-bit have tighter limited addresse memory. They pay the price
        // in slower streaming (more round trips between main and worker threads)
        mTTData.mMappedMaxDim = 2048u;
    #else
        // 64-bit have plenty of virtual addresss to spare. We can reserve much more.
        mTTData.mMappedMaxDim = 4096u;
    #endif
        const uint32 maxResolution = mTTData.mMappedMaxDim;
        //32MB / 128MB for RGBA8, that's two 4096x4096 / 2048x2048 texture.
        mMappedPoolList.push_back( MappedPool( PixelUtil::getFamily( PFG_RGBA8_UNORM ), maxResolution, 2u ) );
        //4MB / 16MB for BC1, that's two 4096x4096 / 2048x2048 texture.
        mMappedPoolList.push_back( MappedPool( PixelUtil::getFamily( PF_BC1UN ), maxResolution, 2u ) );
        //4MB / 16MB for BC3, that's one 4096x4096 / 2048x2048 texture.
        mMappedPoolList.push_back( MappedPool( PixelUtil::getFamily( PF_BC3UN ), maxResolution, 1u ) );
        //4MB / 16MB for BC5, that's one 4096x4096 / 2048x2048 texture.
        mMappedPoolList.push_back( MappedPool( PixelUtil::getFamily( PF_BC5UN ), maxResolution, 1u ) );
#else
        mTTData.mMappedMaxDim = 2048u;
        //Mobile platforms don't support compressed formats, and have tight memory constraints
        //8MB for RGBA8, that's two 2048x2048 texture.
        mMappedPoolList.push_back( MappedPool( PixelUtil::getFamily( PFG_RGBA8_UNORM ), 2048u, 2u ) );
#endif

        //Sort in descending order.
        std::sort( mMappedPoolList.begin(), mMappedPoolList.end(), MappedPool() );

#if N_PLAT == N_PLAT_IOS || N_PLAT == N_PLAT_ANDROID
        //Mobile platforms are tight on memory. Keep the limits low.
        mMaxProcessSize = 32u * 1024u * 1024u;
#else
    #if N_ARCH == N_ARCH_32
        //32-bit architectures are more limited.
        //The default 256MB can cause Out of Memory conditions due to memory fragmentation.
        mMaxProcessSize = 128u * 1024u * 1024u;
    #else
        mMaxProcessSize = 256u * 1024u * 1024u;  //A value of 512MB begins to shake driver bugs.
    #endif
#endif

        mTTData.mProcessSize = 0;
        mTTData.mMappedMaxSize = 64u * 1024u * 1024u;

        mThreadData[0].mOpBuffer = new TextureOpBuffer();
        mThreadData[1].mOpBuffer = new TextureOpBuffer();

        // Generate warning texture
        PixelBlock pixels(8, 8, 1, PF_R5G6B5);
        MemStream * data = N_new MemStream(pixels.getWholeSliceSize());
        pixels.mData = data->getData();

        // Yellow/black stripes
        Colour black(0, 1, 0), yellow(1, 1, 0);
        NCount w = pixels.getWidth();
        NCount h = pixels.getHeight();
        for (size_t y = 0; y < h; ++y)
        {
            for (size_t x = 0; x < w; ++x)
            {
                pixels.setColour((((x + y) % 8) < 4) ? black : yellow, x, y, 0);
            }
        }

        mWarningTexture = const_cast<TextureManager *>(this)->load(0, GroupInner, data, w, h, Texture::T_2D, pixels.mFormat);
        mWarningTexture->ref(false);
        N_delete data;
    }
    //------------------------------------------------------------------------
    TextureManager::~TextureManager()
    {
        mFactoryList.clear();
        mShuttingDown = true;
#if N_PLAT != N_PLAT_EMSCRIPTEN && !OGRE_FORCE_TEXTURE_STREAMING_ON_MAIN_THREAD
        mRequestMutex.lock();
        mRequestCond.notifyAll();
        mRequestMutex.unlock();
        //Threads::WaitForThreads( 1u, &mWorkerThread );
#endif
        N_assert( mFreeMappedList.empty() && "Derived class didn't call destroyAll!" );
        N_assert( mMappedList.empty() && "Derived class didn't call destroyAll!" );
        N_assert( mItemList.empty() && "Derived class didn't call destroyAll!" );
        N_assert( mPool.empty() && "Derived class didn't call destroyAll!" );

        if (mWarningTexture)
            N_delete mWarningTexture;
        
        delete mThreadData[0].mOpBuffer;
        delete mThreadData[0].mOpBuffer;
        mThreadData[1].mOpBuffer = 0;
        mThreadData[1].mOpBuffer = 0;
        
        clearShadow();
    }
    //------------------------------------------------------------------------
    void TextureManager::destroyAll()
    {
        waitAll();

        mMutex.lock();
        destroyAllMapped();
        destroyAllAsync();
        destroyAllTexture();
        destroyAllPool();
        mMutex.unlock();
    }
    //------------------------------------------------------------------------
    void TextureManager::destroyAllMapped()
    {
        MappedTextureList::iterator it = mTTData.mFreeMappedList.begin();
        MappedTextureList::iterator end = mTTData.mFreeMappedList.end();
        while(it != end)
        {
            (*it)->unmap();
            ++it;
        }
        mTTData.mFreeMappedList.clear();

        it = mFreeMappedList.begin();
        end  = mFreeMappedList.end();
        while(it != end)
        {
            destroyMappedImpl(*it);
            delete *it;
            ++it;
        }
        mFreeMappedList.clear();

        it = mMappedList.begin();
        end  = mMappedList.end();
        while(it != end)
        {
            destroyMappedImpl(*it);
            delete *it;
            ++it;
        }
        mMappedList.clear();
    }
    //------------------------------------------------------------------------
    void TextureManager::destroyAllTexture()
    {
        ItemList::const_iterator it = mItemList.begin();
        ItemList::const_iterator end  = mItemList.end();

        while( it != end )
        {
            const Item & entry = it->second;
            delete entry.mTexture;
            ++it;
        }

        mItemList.clear();
    }
    //------------------------------------------------------------------------
    void TextureManager::getTexture(const TextureDefineList & config, TextureList & out)
    {
        out.clear();
        set<Texture *>::type histex;

        TextureDefineList::const_iterator c, ciend = config.end();
        for(c = config.begin(); c != ciend; ++c)
        {
            const TextureDefine & stc = *c;
            bool found = false;
            TextureList::iterator t, tend = mShadowTextureList.end();
            for(t = mShadowTextureList.begin(); t != tend; ++t)
            {
                Texture * tex = *t;

                if(histex.find(tex) != histex.end())
                    continue;

                if(stc.mWidth == tex->getWidth() && stc.mHeight == tex->getHeight()
                    && stc.mFormat == tex->getFormat() && stc.mFSAA == tex->getFSAA())
                {
                    out.push_back(tex);
                    histex.insert(tex);
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                Texture * tex = N_Only(Texture).createTexture(0, GroupInner, Texture::T_2D,
                    stc.mFormat, stc.mWidth, stc.mHeight, 1, 0, Texture::MM_FRAME_RGB, 
                    NULL, NULL, false, stc.mFSAA);
                tex->load();
                out.push_back(tex);
                histex.insert(tex);
                mShadowTextureList.push_back(tex);
            }
        }
    }
    //------------------------------------------------------------------------
    Texture * TextureManager::getDummy(PixelFormat pf)
    {
        TextureList::iterator t, tend = mDummyTextureList.end();
        for(t = mDummyTextureList.begin(); t != tend; ++t)
        {
            Texture * tex = *t;
            if(pf == tex->getFormat())
            {
                return tex;
            }
        }

        Texture * tex = N_Only(Texture).createTexture(0, GroupInner, Texture::T_2D, 
            pf, 1, 1, 1, 0, Buffer::M_DEV | Buffer::M_WRITE);
        mDummyTextureList.push_back(tex);

        tex->getBuffer()->lock(Buffer::MM_WHOLE);

        const PixelBlock & box = tex->getBuffer()->getMutexData();
        PixelUtil::packColour(1.0f, 1.0f, 1.0f, 1.0f, pf, box.mData);

        tex->getBuffer()->unlock();

        return tex;
    }
    //------------------------------------------------------------------------
    void TextureManager::recoverShadow()
    {
        TextureList::iterator i = mShadowTextureList.begin();
        for(; i != mShadowTextureList.end();)
        {
            if((*i)->getRefCount() == 0)
            {
                N_Only(Texture).destroy((*i)->getPrc());
                i = mShadowTextureList.erase(i);
            }
            else
            {
                ++i;
            }
        }

        for(i = mDummyTextureList.begin(); i != mDummyTextureList.end();)
        {
            if((*i)->getRefCount() == 0)
            {
                N_Only(Texture).destroy((*i)->getPrc());
                i = mDummyTextureList.erase(i);
            }
            else
            {
                ++i;
            }
        }
    }
    //------------------------------------------------------------------------
    void TextureManager::clearShadow()
    {
        TextureList::iterator i, iend = mShadowTextureList.end();
        for(i = mShadowTextureList.begin(); i != iend; ++i)
        {
            N_Only(Texture).destroy((*i)->getPrc());
        }
        mShadowTextureList.clear();
    }
    //------------------------------------------------------------------------
    void TextureManager::destroyAllPool()
    {
        TexturePoolList::const_iterator it = mPool.begin();
        TexturePoolList::const_iterator end  = mPool.end();

        while( it != end )
        {
            delete it->mPool;
            ++it;
        }

        mPool.clear();
    }
    //-----------------------------------------------------------------------------------
    Texture * TextureManager::createPool(GroupID poolId, NCount width, NCount height, 
        NCount array, NCount mipmaps, PixelFormat pf)
    {
        TexturePool re;
        re.mPool = createTextureImpl(Texture::RT_Disk, 0, Texture::MM_POOL, Texture::T_2DArray);

        re.mPrimary = true;
        re.mNextSlice = 0;
        re.mUsedList.reserve(array);

        re.mPool->setResolution(width, height, array);
        re.mPool->setFormat(pf);
        re.mPool->setMipmapCount(mipmaps);
        re.mPool->mPoolId = poolId;

        mPool.push_back(re);

        re.mPool->load(StorageType::ST_Gpu, 0);
        re.mPool->notifyGpuData();

        return re.mPool;
    }
    //-----------------------------------------------------------------------------------
    bool TextureManager::isPoolExist(GroupID poolId) const
    {
        TexturePoolList::const_iterator itPool = mPool.begin();
        TexturePoolList::const_iterator enPool = mPool.end();

        while(itPool != enPool)
        {
            const TexturePool & pool = *itPool;
            if(pool.mPool->getPoolId() == poolId)
            {
                return pool.mPool;
            }

            ++itPool;
        }

        return false;
    }
    //-----------------------------------------------------------------------------------
    bool TextureManager::isPoolExist(NCount width, NCount height, NCount mipmaps, PixelFormat pf) const
    {
        TexturePoolList::const_iterator itPool = mPool.begin();
        TexturePoolList::const_iterator enPool = mPool.end();

        while( itPool != enPool)
        {
            const TexturePool & pool = *itPool;
            if( pool.mPool->getWidth() == width && pool.mPool->getHeight() == height &&
                pool.mPool->getFormat() == pf && pool.mPool->getMipmapCount() == mipmaps)
            {
                return true;
            }

            ++itPool;
        }

        return false;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::destroyTexture(Texture * tex)
    {
        if( tex->isPool() )
        {
            //Textures that are owners of a pool that were created
            //with createPool. Texture pool owners that weren't
            //created with that function (i.e. automatically / on demand)
            //are released automatically in poolFree
            TexturePoolList::iterator it = mPool.begin();
            TexturePoolList::iterator end  = mPool.end();

            while( it != end && it->mPool != tex )
                ++it;

            if( it == mPool.end() )
            {
                N_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Texture with name '" + tex->getName().getFriendlyText() +
                    "' owner of a TexturePool not found. Perhaps already destroyed?");
            }

            N_assert( it->mPrimary, "Pools that were created automatically "
                             "should not be destroyed manually via TextureManager::destroyTexture."
                             " These pools will be destroyed automatically once they're empty" );

            if( !it->empty() )
            {
                N_EXCEPT( Exception::ERR_ITEM_NOT_FOUND,
                             "Texture with name '" + tex->getName().getFriendlyText() +
                             "' cannot be deleted! It's a TexturePool and it still has "
                             "live textures using it! You must release those first by "
                             "removing them from being Resident" );
            }

            delete it->mPool;
            mPool.erase( it );
        }
        else
        {
            //Almost all textures
            ItemList::iterator it = mItemList.find( tex->getName() );

            if( it == mItemList.end() )
            {
                N_EXCEPT( Exception::ERR_ITEM_NOT_FOUND,
                    "Texture with name '" + tex->getName().getFriendlyText() + "' not found. Perhaps already destroyed?" );
            }

            if( it->second.mDestroyTask )
            {
                N_EXCEPT( Exception::ERR_INVALID_STATE,
                    "Texture with name '" + tex->getName().getFriendlyText() + "' has already been scheduled for destruction!" );
            }

            it->second.mDestroyTask = true;

            Texture::Task task;
            task.mType = ST_Destroy;

            if( tex->getTaskCount() == 0 )
            {
                //If the Texture is in the worker thread, the following may be true:
                //  1. Texture is not yet Resident. Thus getTaskCount cannot be 0
                //  2. Texture is Resident, but being loaded. Thus getTaskCount will be 0
                //     but isValidImpl returns false
                //  3. Texture will become ST_Cpu, after it finishes loading. Thus
                //     getTaskCount cannot be 0
                //
                //Thus we know for sure the Texture is not in the worker thread with this if statement
                if( tex->isValidImpl() || tex->getCurrentStorage() != StorageType::ST_Gpu )
                {
                    //There are no pending tasks. We can execute it right now
                    executeTask( tex, Texture::Process::O_OutGpu, task );
                }
                else
                {
                    //No pending tasks, but the texture is being loaded. Delay execution
                    ++tex->mTaskCount;
                    mTextureTaskList[tex].push_back( task );
                }
            }
            else
            {
                ++tex->mTaskCount;
                mTextureTaskList[tex].push_back( task );
            }
        }
    }
    //-----------------------------------------------------------------------------------
    MappedTexture* TextureManager::createMapped(const PixelVolume & vol, NIIf usedThreshold )
    {
        //OgreProfileExhaustive( "TextureManager::createMapped" );
        N_assert( usedThreshold <= 1.0, "Invalid consumptionRatioThreshold value!" );

/*#if OGRE_DEBUG_MEMORY_CONSUMPTION
        {
            char tmpBuffer[512];
            LwString text( LwString::FromEmptyPointer( tmpBuffer, sizeof(tmpBuffer) ) );
            text.a( "TextureManager::createMapped: ", vol.mWidth, "x", vol.mHeight, "x" );
            text.a( vol.mDepth, "x", vol.mArray, " ", PixelUtil::toString( vol.mFormat ) );
            text.a( " (", (uint32)PixelUtil::getSize( vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray, vol.mFormat) / 1024u / 1024u, " MB)");
            LogManager::getSingleton().logMessage( text.c_str() );
        }
#endif*/

        MappedTextureList::iterator bestRe = mFreeMappedList.end();
        MappedTextureList::iterator it = mFreeMappedList.begin();
        MappedTextureList::iterator end  = mFreeMappedList.end();

        while( it != end )
        {
            MappedTexture * temp = *it;

            if( temp->request(vol) && (bestRe == end || temp->isSmallerThan( *bestRe )) )
            {
                if( !temp->isBlock() )
                    bestRe = it;
            }

            ++it;
        }

        MappedTexture * re = 0;

        if( bestRe != end && usedThreshold != 0u )
        {
            const size_t requiredSize = PixelUtil::getSize( vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray, vol.mFormat);
            const NIIf ratio = requiredSize / (*bestRe)->getSize();
            if( ratio < usedThreshold )
                bestRe = end;
        }

        if( bestRe != end )
        {
            re = *bestRe;
            mMappedList.push_back( *bestRe );
            mFreeMappedList.erase( bestRe );
        }
        else
        {
            //Couldn't find an existing MappedTexture that could handle our request.
            //Check that our memory budget isn't exceeded.
            const size_t requiredSize = PixelUtil::getSize( vol.mWidth, vol.mHeight, vol.mDepth, vol.mArray, vol.mFormat);

            size_t consumedBytes = TextureManager_getSize( mFreeMappedList );

            if( consumedBytes + requiredSize > mMappedPoolMaxSize )
            {
                LogManager::getSingleton().logMessage( "Texture memory budget exceeded. Stalling GPU." );

                //NVIDIA driver can let the staging textures accumulate and skyrocket the
                //memory consumption until the process runs out of memory and crashes
                //(if it has a lot of textures to load).
                //Worst part this only repros in some machines, not driver specific.
                //Flushing here fixes it.
                N_Engine().getRender()->_clear();

                set<uint32>::type waitedFrames;

                //Before freeing memory, check if we can make some of
                //the existing staging textures available for use.
                MappedTextureList::iterator bestRe = mFreeMappedList.end();
                MappedTextureList::iterator it = mFreeMappedList.begin();
                MappedTextureList::iterator end  = mFreeMappedList.end();
                while( it != end && bestRe == end )
                {
                    MappedTexture * stagingTexture = *it;
                    const uint32 frameUsed = stagingTexture->getLastFrame();

                    if( waitedFrames.find( frameUsed ) == waitedFrames.end() )
                    {
                        N_Only(GBuffer).wait( frameUsed );
                        waitedFrames.insert( frameUsed );
                    }

                    if( stagingTexture->request( vol) && (bestRe == end || stagingTexture->getSize() < bestRe->getSize()))
                    {
                        const size_t ratio = (requiredSize * 100u) / (*it)->getSize();
                        if( ratio >= usedThreshold )
                            bestRe = it;
                    }

                    ++it;
                }

                //MappedTexture *re = 0;

                if( bestRe == end )
                {
                    //LogManager::getSingleton().logMessage( "Stalling was not enough. Freeing memory." );

                    //Could not find any best candidate even after stalling.
                    //Start deleting staging textures until we've freed enough space.
                    it = mFreeMappedList.begin();
                    while( it != end && (consumedBytes + requiredSize > mMappedPoolMaxSize) )
                    {
                        consumedBytes -= (*it)->getSize();
                        destroyMappedImpl( *it );
                        delete *it;
                        ++it;
                    }

                    mFreeMappedList.erase(mFreeMappedList.begin(), it);

                    N_Engine().getRender()->_clear();
                }
                else
                {
                    re = *bestRe;
                    mMappedList.push_back(*bestRe);
                    mFreeMappedList.erase(bestRe);
                }
            }
        
            if( !re )
            {
                re = createMappedImpl(vol);
                mMappedList.push_back(re);
            }
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::removeMapped(MappedTexture * mapped)
    {
        //Reverse search to speed up since most removals are
        //likely to remove what has just been requested.
        MappedTextureList::reverse_iterator ritor = std::find(mMappedList.rbegin(), mMappedList.rend(), mapped);

        //N_assert( ritor != mMappedList.rend() && "MappedTexture does not belong to this TextureManager or already removed" );

        MappedTextureList::iterator it = ritor.base() - 1u;
        remove( mMappedList, it );

        mFreeMappedList.push_back( mapped );
    }
    //-----------------------------------------------------------------------------------
    TextureAsync * TextureManager::createAsync(NCount width, NCount height, NCount depth, NCount array,
        Texture::Type type, PixelFormat pf)
    {
        TextureAsync * re = createAsyncImpl(width, height, depth, array, type, PixelUtil::getFamily(pf));
        mAsyncList.push_back(re);
        return re;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::destroyAsync(TextureAsync * ticket)
    {
        //Reverse search to speed up since most removals are
        //likely to remove what has just been requested.
        TextureAsyncList::reverse_iterator ritor = std::find( mAsyncList.rbegin(), mAsyncList.rend(), ticket );
        TextureAsyncList::iterator it = ritor.base() - 1u;
        remove( mAsyncList, it );
        //N_assert( ritor != mAsyncList.rend() && "TextureAsync does not belong to this TextureManager or already removed" );

        N_delete ticket;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::destroyAllAsync()
    {
        TextureAsyncList::const_iterator it = mAsyncList.begin();
        TextureAsyncList::const_iterator end  = mAsyncList.end();

        while( it != end )
        {
            N_delete *it;
            ++it;
        }

        mAsyncList.clear();
    }
    //-----------------------------------------------------------------------------------
    /*void TextureManager::saveTexture(Texture * tex, const String &folderPath,
        set<String>::type & savedTextures, bool saveOitd, bool saveOriginal, HlmsTextureExportListener *listener )
    {
        String resourceName = tex->getSrc();

        //Render Targets are... complicated. Let's not, for now.
        if( savedTextures.find( resourceName ) != savedTextures.end() || tex->isFrame() )
            return;

        DataStreamPtr inFile;
        if( saveOriginal )
        {
            const String aliasName = tex->getName();
            String savingFilename = resourceName;
            if( listener )
            {
                listener->savingChangeTextureNameOriginal( aliasName, resourceName, savingFilename );
            }

            try
            {
                GroupID resourceGroup = tex->getGroup();
                if( GroupUnknow == resourceGroup)
                    resourceGroup = ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;

                inFile = ResourceGroupManager::getSingleton().openResource( resourceName, resourceGroup );
            }
            catch( FileNotFoundException &e )
            {
                //Try opening as an absolute path
                std::fstream *ifs = OGRE_NEW_T( std::fstream, MEMCATEGORY_GENERAL )( resourceName.c_str(), std::ios::binary|std::ios::in );

                if( ifs->is_open() )
                {
                    inFile = DataStreamPtr( N_new FileStreamDataStream( resourceName, ifs, true ) );
                }
                else
                {
                    LogManager::getSingleton().logMessage(
                                "WARNING: Could not find texture file " + aliasName +
                                " (" + resourceName + ") for copying to export location. "
                                "Error: " + e.getFullDescription() );
                }
            }
            catch( Exception &e )
            {
                LogManager::getSingleton().logMessage(
                            "WARNING: Could not find texture file " + aliasName +
                            " (" + resourceName + ") for copying to export location. "
                            "Error: " + e.getFullDescription() );
            }

            if( inFile )
            {
                size_t fileSize = inFile->size();
                vector<Nui8>::type fileData;
                fileData.resize( fileSize );
                inFile->read( &fileData[0], fileData.size() );
                std::ofstream outFile( (folderPath + "/" + savingFilename).c_str(), std::ios::binary | std::ios::out );
                outFile.write( (const char*)&fileData[0], fileData.size() );
                outFile.close();
            }
        }

        if( saveOitd )
        {
            String texName = resourceName;
            if( listener )
                listener->savingChangeTextureNameOitd( texName, tex );

            if( tex->getStorage() == StorageType::ST_Gpu )
            {
                Image image;
                tex->read( &image, 0u, tex->getMipmapCount(), true );

                image.save( folderPath + "/" + texName + ".oitd", 0, image.getMipmapCount() );
            }
        }

        savedTextures.insert( resourceName );
    }*/
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // Metadata
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    TextureManager::Metadata::Metadata() :
        mWidth( 0 ),
        mHeight( 0 ),
        mDepth( 0 ),
        mFormat( PF_UNKNOWN ),
        mPoolId( 0 ),
        mType( Texture::T_Unknow ),
        numMipmaps( 0 )
    {
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // TextureManager
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    bool TextureManager::_setMetadata(Texture * tex)
    {
        bool re = false;
        MetadataList::const_iterator it = mMetadataList.find( tex->getName() );
        if(it != mMetadataList.end())
        {
            const Metadata & cacheEntry = it->second;
            tex->setResolution( cacheEntry.mWidth, cacheEntry.mHeight, cacheEntry.mDepth );
            tex->setMipmapCount( cacheEntry.numMipmaps );
            tex->setTextureType( cacheEntry.mType );
            tex->setFormat( cacheEntry.mFormat );
            tex->mPoolId = cacheEntry.mPoolId;
            re = true;
        }
        return re;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::_updateMetadata(Texture * tex)
    {
        ItemList::const_iterator it = mItemList.find(tex->getName());
        if(it != mItemList.end())
        {
            Metadata entry;
            entry.mName = it->second.alias;
            entry.mWidth = tex->getWidth();
            entry.mHeight = tex->getHeight();
            entry.mDepth = tex->getVolume();
            entry.mFormat = tex->getFormat();
            entry.mPoolId = tex->getPoolId();
            entry.mType = tex->getType();
            entry.numMipmaps = tex->getMipmapCount();

            mMetadataList[tex->getName()] = entry;
        }
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::_removeMetadata(Texture * tex)
    {
        mMetadataList.erase(tex->getName());
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::readMetadata(DataStream * in, bool createpool )
    //readMetadata( const String &filename, const char *in, bool createpool )
    {
        DataStream
#if !OGRE_NO_JSON
        rapidjson::Document d;
        d.Parse( in );

        if( d.HasParseError() )
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Invalid JSON string in file " + filename + " at line " +
                         StrConv::conv( d.GetErrorOffset() ) + " Reason: " + rapidjson::GetParseError_En( d.GetParseError() ) );
        }

        rapidjson::Value::ConstMemberIterator it;
        it = d.FindMember( "reserved_pool_ids" );

        if( it != d.MemberEnd() && it->value.IsArray() && createpool )
        {
            const rapidjson::Value & jsonVal = it->value;
            const rapidjson::SizeType arraySize = jsonVal.Size();
            for( rapidjson::SizeType i=0; i<arraySize; ++i )
            {
                if( jsonVal[i].IsObject() )
                {
                    Metadata entry;

                    it = jsonVal[i].FindMember( "poolId" );
                    if( it != jsonVal[i].MemberEnd() && it->value.IsUint() )
                        entry.mPoolId = it->value.GetUint();

                    it = jsonVal[i].FindMember( "resolution" );
                    if( it != jsonVal[i].MemberEnd() &&
                        it->value.IsArray() && it->value.Size() >= 3u &&
                        it->value[0].IsUint() && it->value[1].IsUint() && it->value[2].IsUint() )
                    {
                        entry.mWidth = it->value[0].GetUint();
                        entry.mHeight = it->value[1].GetUint();
                        entry.mDepth = it->value[2].GetUint();
                    }

                    entry.numMipmaps = 1u;
                    it = jsonVal[i].FindMember( "mipmaps" );
                    if( it != jsonVal[i].MemberEnd() && it->value.IsUint() )
                        entry.numMipmaps = static_cast<Nui8>( it->value.GetUint() );

                    it = jsonVal[i].FindMember( "format" );
                    if( it != jsonVal[i].MemberEnd() && it->value.IsString() )
                    {
                        entry.mFormat = PixelUtil::getFormatFromName( it->value.GetString() );
                    }

                    if( entry.mWidth > 0u && entry.mHeight > 0u && entry.mDepth > 0u && entry.mFormat != PF_UNKNOWN &&
                        !isPoolExist( entry.mPoolId) )
                    {
                        createPool( entry.mPoolId, entry.mWidth, entry.mHeight, entry.mDepth, entry.numMipmaps, entry.mFormat );
                    }
                }
            }
        }

        it = d.FindMember( "textures" );
        if( it != d.MemberEnd() && it->value.IsObject() )
        {
            rapidjson::Value::ConstMemberIterator itTex = it->value.MemberBegin();
            rapidjson::Value::ConstMemberIterator enTex = it->value.MemberEnd();

            while( itTex != enTex )
            {
                if( itTex->value.IsObject() )
                {
                    IdString aliasName = itTex->name.GetString();
                    Metadata entry;

                    entry.mName = itTex->name.GetString();

                    it = itTex->value.FindMember( "poolId" );
                    if( it != itTex->value.MemberEnd() && it->value.IsUint() )
                        entry.mPoolId = it->value.GetUint();

                    entry.mType = Texture::T_2D;
                    it = itTex->value.FindMember( "texture_type" );
                    if( it != itTex->value.MemberEnd() && it->value.IsUint() )
                    {
                        entry.mType = static_cast<Texture::Type>(std::clamp<uint32>( it->value.GetUint(), 0u, Texture::T_3D ) );
                    }

                    it = itTex->value.FindMember( "resolution" );
                    if( it != itTex->value.MemberEnd() &&
                        it->value.IsArray() && it->value.Size() >= 3u &&
                        it->value[0].IsUint() && it->value[1].IsUint() && it->value[2].IsUint() )
                    {
                        entry.mWidth = it->value[0].GetUint();
                        entry.mHeight = it->value[1].GetUint();
                        entry.mDepth = it->value[2].GetUint();
                    }

                    entry.numMipmaps = 1u;
                    it = itTex->value.FindMember( "mipmaps" );
                    if( it != itTex->value.MemberEnd() && it->value.IsUint() )
                        entry.numMipmaps = static_cast<Nui8>( it->value.GetUint() );

                    it = itTex->value.FindMember( "format" );
                    if( it != itTex->value.MemberEnd() && it->value.IsString() )
                    {
                        entry.mFormat = PixelUtil::getFormatFromName( it->value.GetString() );
                    }

                    mMetadataList[aliasName] = entry;
                }

                ++itTex;
            }
        }
#else
        N_EXCEPT( UnImpl, "Ogre must be built with JSON support to call this function!");
#endif
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::writeMetadata( String & out )
    {
        char tmpBuffer[4096];
        LwString jsonStr(LwString::FromEmptyPointer(tmpBuffer, sizeof(tmpBuffer)));

        jsonStr.a( "{" );
        jsonStr.a( "\n\t\"reserved_pool_ids\" :\n\t[" );

        bool firstIteration = true;
        {
            TexturePoolList::const_iterator it = mPool.begin();
            TexturePoolList::const_iterator end  = mPool.end();

            while( it != end )
            {
                const TexturePool &pool = *it;
                if( pool.mPrimary )
                {
                    if( !firstIteration )
                        jsonStr.a( "," );
                    jsonStr.a( "\n\t\t{\n\t\t\t\"poolId\" : ", pool.mPool->getPoolId() );
                    jsonStr.a( ",\n\t\t\t\"resolution\" : [",
                               pool.mPool->getWidth(), ", ",
                               pool.mPool->getHeight(), ", ",
                               pool.mPool->getVolume(), "]" );
                    jsonStr.a( ",\n\t\t\t\"mipmaps\" : ", pool.mPool->getMipmapCount() );
                    jsonStr.a( ",\n\t\t\t\"format\" : \"",
                               PixelUtil::toString( pool.mPool->getFormat() ),
                               "\"" );
                    jsonStr.a( "\n\t\t}" );
                    firstIteration = false;

                    out += jsonStr.c_str();
                    jsonStr.clear();
                }
                ++it;
            }
        }

        jsonStr.a( "\n\t],\n\t\"textures\" :\n\t{" );
        firstIteration = true;
        MetadataList::const_iterator it = mMetadataList.begin();
        MetadataList::const_iterator end  = mMetadataList.end();

        while( it != end )
        {
            const Metadata &entry = it->second;

            if( !firstIteration )
                jsonStr.a( "," );

            jsonStr.a( "\n\t\t\"", entry.mName.c_str(), "\" : \n\t\t{" );
            //jsonStr.a( "\n\t\t\t\"resource\" : \"", entry.resourceName.c_str(), "\"" );
            jsonStr.a( "\n\t\t\t\"resolution\" : [",
                       entry.mWidth, ", ",
                       entry.mHeight, ", ",
                       entry.mDepth, "]" );
            jsonStr.a( ",\n\t\t\t\"mipmaps\" : ", entry.numMipmaps );
            jsonStr.a( ",\n\t\t\t\"format\" : \"", PixelUtil::toString( entry.mFormat ), "\"" );
            jsonStr.a( ",\n\t\t\t\"texture_type\" : ", (int)entry.mType );
            jsonStr.a( ",\n\t\t\t\"poolId\" : ", entry.mPoolId );
            jsonStr.a( "\n\t\t}" );

            out += jsonStr.c_str();
            jsonStr.clear();

            firstIteration = false;

            ++it;
        }
        jsonStr.a( "\n\t}\n}" );
        out += jsonStr.c_str();
        jsonStr.clear();
    }
    //-----------------------------------------------------------------------------------
    /*void TextureManager::getMemoryStats( size_t & outTextureBytesCpu, size_t & outTextureBytesGpu,
        size_t & outUsedStagingTextureBytes, size_t & outAvailableStagingTextureBytes )
    {
        outUsedStagingTextureBytes = TextureManager_getSize( mFreeMappedList );
        outAvailableStagingTextureBytes = TextureManager_getSize( mMappedList );

        size_t textureBytesCpu = 0;
        size_t textureBytesGpu = 0;

        ItemList::const_iterator it = mItemList.begin();
        ItemList::const_iterator end  = mItemList.end();

        while( it != end )
        {
            const Item &entry = it->second;
            StorageType residency = entry.mTexture->getCurrentStorage();
            if( residency != StorageType::ST_Disk )
            {
                const size_t sizeBytes = entry.mTexture->getSize();
                if( residency == StorageType::ST_Gpu )
                    textureBytesGpu += sizeBytes;

                if( residency == StorageType::ST_Cpu || entry.mTexture->getRecoverType() == Texture::RT_CpuGpu )
                {
                    textureBytesCpu += sizeBytes;
                }
            }

            ++it;
        }

        outTextureBytesCpu = textureBytesCpu;
        outTextureBytesGpu = textureBytesGpu;
    }*/
    //-----------------------------------------------------------------------------------
    /*void TextureManager::dumpStats(void) const
    {
        char tmpBuffer[512];
        LwString text( LwString::FromEmptyPointer( tmpBuffer, sizeof(tmpBuffer) ) );

        LogManager & logMgr = LogManager::getSingleton();

        const size_t availSizeBytes = TextureManager_getSize( mFreeMappedList );
        const size_t usedSizeBytes = TextureManager_getSize( mMappedList );

        text.clear();
        text.a( "Available Staging Textures\t|", (uint32)mFreeMappedList.size(), "|",
                (uint32)availSizeBytes  / (1024u * 1024u), " MB\t\t |In use:\t|",
                (uint32)usedSizeBytes  / (1024u * 1024u), " MB");
        logMgr.logMessage( text.c_str() );
    }*/
    //-----------------------------------------------------------------------------------
    /*void TextureManager::dumpMemoryUsage(Log * log) const
    {
        Log* logActual = log == NULL ? LogManager::getSingleton().getDefaultLog() : log;

        logActual->logMessage(
                    "==============================="
                    "Start dump of TextureManager"
                    "===============================",
                    LML_CRITICAL );

        logActual->logMessage( "== Dumping Pools ==" );

        logActual->logMessage(
                    "||Width|Height|Format|Mipmaps|Size in bytes|"
                    "Num. active textures|Total texture capacity|Pool ID|Texture Names",
                    LML_CRITICAL );

        size_t bytesInPoolInclWaste = 0;
        size_t bytesInPoolExclWaste = 0;

        vector<char>::type tmpBuffer;
        tmpBuffer.resize( 512 * 1024 ); //512kb per line should be way more than enough
        LwString text( LwString::FromEmptyPointer( &tmpBuffer[0], tmpBuffer.size() ) );

        TexturePoolList::const_iterator itPool = mPool.begin();
        TexturePoolList::const_iterator enPool = mPool.end();

        while( itPool != enPool )
        {
            const TexturePool &pool = *itPool;
            text.clear();
            text.a( "||", pool.mPool->getWidth(), "|", pool.mPool->getHeight(), "|" );
            text.a( PixelUtil::toString( pool.mPool->getFormat() ), "|",
                    pool.mPool->getMipmapCount(), "|" );
            const size_t bytesInPool = pool.mPool->getSize();
            text.a( (uint32)bytesInPool, "|" );
            text.a( pool.mNextSlice - (uint16)pool.mFreeList.size(), "|",
                    pool.mPool->getVolume() );
            text.a( "|", pool.mPool->getPoolId() );

            bytesInPoolInclWaste += bytesInPool;

            TextureList::const_iterator itTex = pool.mUsedList.begin();
            TextureList::const_iterator enTex = pool.mUsedList.end();

            while( itTex != enTex )
            {
                Texture *tex = *itTex;
                text.a( "|", tex->getName().c_str() );
                bytesInPoolExclWaste += tex->getSize();
                ++itTex;
            }

            logActual->logMessage( text.c_str(), LML_CRITICAL );

            ++itPool;
        }

        size_t bytesOutsidePool = 0;

        logActual->logMessage(
                    "|Alias|Resource Name|Width|Height|Depth|Num Slices|Format|Mipmaps|MSAA|Size in bytes|"
                    "RTT|UAV|Manual|MSAA Explicit|MM_AlterView|AutomaticBatched",
                    LML_CRITICAL );

        ItemList::const_iterator itEntry = mItemList.begin();
        ItemList::const_iterator enEntry = mItemList.end();

        while( itEntry != enEntry )
        {
            const Item & entry = itEntry->second;
            text.clear();

            const size_t bytesTexture = entry.mTexture->getSize();

            text.a( "|", entry.mTexture->getName().c_str() );
            text.a( "|", entry.mTexture->getSrc().c_str() );
            text.a( "|", entry.mTexture->getWidth(), "|", entry.mTexture->getHeight(), "|" );
            text.a( entry.mTexture->getDepth(), "|", entry.mTexture->getArray(), "|" );
            text.a( PixelUtil::toString( entry.mTexture->getFormat() ), "|",
                    entry.mTexture->getMipmapCount(), "|" );
            text.a( entry.mTexture->getOriginSample().getMultiSampling(), "|",
                    (uint32)bytesTexture, "|" );
            text.a( entry.mTexture->isFrame(), "|",
                    entry.mTexture->isBufferStroge(), "|",
                    entry.mTexture->isManual(), "|" );
            text.a( entry.mTexture->isManualMSAA(), "|",
                    entry.mTexture->isAlterView(), "|",
                    entry.mTexture->isPoolSlice() );

            if( !entry.mTexture->isPoolSlice() )
                bytesOutsidePool += bytesTexture;

            logActual->logMessage( text.c_str(), LML_CRITICAL );

            ++itEntry;
        }

        float fMBytesInPoolInclWaste = bytesInPoolInclWaste / (1024.0f * 1024.0f);
        float fMBytesInPoolExclWaste = bytesInPoolExclWaste / (1024.0f * 1024.0f);
        float fMBytesOutsidePool = bytesOutsidePool  / (1024.0f * 1024.0f);

        text.clear();
        text.a( "\n|MBs in pools (excluding waste):|", LwString::Float( fMBytesInPoolExclWaste, 2 ),
                "\n|MBs in pools (including waste):|", LwString::Float( fMBytesInPoolInclWaste, 2 ) );
        text.a( "\n|MBs outside of pools:|", LwString::Float( fMBytesOutsidePool, 2 ),
                "\n|Total MBs (excl. waste):|",
                LwString::Float( fMBytesInPoolExclWaste + fMBytesOutsidePool, 2 ) );
        text.a( "\n|Total MBs (incl. waste):|",
                LwString::Float( fMBytesInPoolInclWaste + fMBytesOutsidePool, 2 ) );
        logActual->logMessage( text.c_str(), LML_CRITICAL );

        //dumpStats();

        logActual->logMessage(
                    "============================="
                    "End dump of TextureManager"
                    "=============================",
                    LML_CRITICAL );
    }*/
    //-----------------------------------------------------------------------------------
    void TextureManager::setMappedMaxDim(NCount dim)
    {
        mTTData.mMappedMaxDim = dim;
    }
    //-----------------------------------------------------------------------------------
    NCount TextureManager::getMappedMaxDim() const
    {
        return mTTData.mMappedMaxDim;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::setMappedMaxSize(NCount size)
    {
        mTTData.mMappedMaxSize = std::max<size_t>( 1u, size);
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::getMappedMaxSize() const
    {
        return mTTData.mMappedMaxSize;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::taskLoadToSysRamOrResident(Texture * tex, const Texture::Task & task)
    {
        const StorageType st = task.mType;

        if( tex->getCurrentStorage() == StorageType::ST_Disk )
        {
            N_assert1( st == StorageType::ST_Gpu || st == StorageType::ST_Cpu );

            ResourceID name;
            GroupID resourceGroup;
            uint32 filters = 0;
            ItemList::const_iterator it = mItemList.find( tex->getOriginID() );
            if( it != mItemList.end() )
            {
                name = it->second.mRID;
                resourceGroup = it->second.mGroup;
                filters = it->second.mMark;
            }

            if( tex->getType() != Texture::T_CUBE )
            {
                VFS * archive = 0;

                if( resourceGroup != GroupUnknow )
                {
                    archive = N_Only(ResourceScheme).getVFS( name, resourceGroup );
                }

                if(st != StorageType::ST_Cpu && tex->getRecoverType() != Texture::RT_CpuGpu )
                {
                    if( _setMetadata( tex ) )
                        tex->load( StorageType::ST_Gpu, 0 );
                }
                    
                mRequestMutex.lock();
                    
                mThreadData[MainThread].mRequestList.push_back( TextureRequest( name, archive, task.mImage, tex, 
                    std::numeric_limits<NCount>::max(), filters, task.mAutoImage, st == StorageType::ST_Cpu ) );
                mRequestCond.notifyAll();
                    
                mRequestMutex.unlock();
            }
            else
            {
                String baseName;
                String ext;
                String::size_type pos = name.find_last_of( '.' );
                if( pos != String::npos )
                {
                    baseName    = name.substr( 0, pos );
                    ext         = name.substr( pos + 1u );
                }
                else
                {
                    baseName = name;
                }

                String lowercaseExt = ext;
                StringUtil::toLowerCase( lowercaseExt );

                if( lowercaseExt == "dds" || lowercaseExt == "ktx" || lowercaseExt == "oitd" )
                {
                    // XX HACK there should be a better way to specify whether
                    // all faces are in the same file or not
                    VFS * archive = 0;

                    if( resourceGroup != GroupUnknow )
                    {
                        archive = N_Only(ResourceScheme).getVFS( name, resourceGroup );
                    }

                    if(st != StorageType::ST_Cpu && tex->getRecoverType() != Texture::RT_CpuGpu )
                    {
                        if( _setMetadata( tex ) )
                            tex->load( StorageType::ST_Gpu, 0 );
                    }
                        
                    mRequestMutex.lock();
                        
                    mThreadData[MainThread].mRequestList.push_back( TextureRequest( name, archive, task.mImage, tex, 
                        std::numeric_limits<NCount>::max(), filters, task.mAutoImage, st == StorageType::ST_Cpu ) );
                    mRequestCond.notifyAll();
                        
                    mRequestMutex.unlock();
                }
                else
                {
                    static const String suffixes[6] = { "_rt.", "_lf.", "_up.", "_dn.", "_fr.", "_bk." };

                    for( uint32 i = 0; i < 6u; ++i )
                    {
                        VFS * archive = 0;

                        if( resourceGroup != GroupUnknow )
                        {
                            archive = N_Only(ResourceScheme).getVFS( baseName + suffixes[i] + ext, resourceGroup );
                        }

                        if( i == 0 && st != StorageType::ST_Cpu && tex->getRecoverType() != Texture::RT_CpuGpu )
                        {
                            if( _setMetadata( tex ) )
                                tex->load( StorageType::ST_Gpu, 0 );
                        }
                            
                        mRequestMutex.lock();
                            
                        mThreadData[MainThread].mRequestList.push_back( TextureRequest( baseName + suffixes[i] + ext, archive, i == 0 ? task.mImage : 0, tex, i, filters, task.mAutoImage, st == StorageType::ST_Cpu ) );
                        mRequestCond.notifyAll();
                            
                        mRequestMutex.unlock();
                    }
                }
            }
        }
        else
        {
            N_assert1( st == StorageType::ST_Gpu );
            N_assert1( tex->getCurrentStorage() == StorageType::ST_Cpu );

            Image * image = new Image();
            Nui8 * rawBuffer = tex->getData(0 );
            image->load( rawBuffer, tex->getWidth(), tex->getHeight(), tex->getDepth(), 
                tex->getFormat(), tex->getArray(), tex->getMipmapCount(), tex->getRecoverType() != Texture::RT_CpuGpu);

            ResourceID name;
            uint32 filters = 0;
            ItemList::const_iterator it = mItemList.find( tex->getName() );
            if( it != mItemList.end() )
            {
                name = it->second.name;
                filters = it->second.mMark;
            }

            //Only allow applying mipmap generation filter, since it's the only filter
            //that may have been skipped when loading from ST_Disk -> ST_Cpu
            filters &= TextureFilter::MM_MIPMAP;
            if( filters & TextureFilter::MM_MIPMAP )
            {
                //We will transition to Resident, we must ensure the number of mipmaps is set
                //as the HW mipmap filter cannot change it from the background thread.
                Nui8 numMipmaps = tex->getMipmapCount();
                PixelFormat pf = tex->getFormat();
                TextureFilter::FilterBase::simulateFiltersForCacheConsistency( filters, *image, this, numMipmaps, pf );
                if( tex->getMipmapCount() != numMipmaps )
                {
                    const bool oldValue = mIgnoreScheduledTasks;
                    mIgnoreScheduledTasks = true;
                    //These load calls are unscheduled and will decrease mTaskCount
                    //to wrong values that would cause schedule to think the Texture
                    //is done, thus we need to counter that.
                    tex->mTaskCount += 2;
                    tex->setAutoData(false);
                    tex->load( StorageType::ST_Disk, rawBuffer, false );
                    tex->setMipmapCount( numMipmaps );
                    tex->load( StorageType::ST_Cpu, rawBuffer, false );
                    mIgnoreScheduledTasks = oldValue;
                }
            }

            tex->load( StorageType::ST_Gpu, tex->getData( 0 ), false );

            mRequestMutex.lock();
            
            mThreadData[MainThread].mRequestList.push_back(TextureRequest(name, 0, image, tex, 0, filters, true, false));
            mRequestCond.notifyAll();
            
            mRequestMutex.unlock();
        }
    }
    //-----------------------------------------------------------------------------------
    bool TextureManager::executeTask( Texture * tex, Texture::Process::Operation reason, const Texture::Task & task )
    {
        bool taskExecuted = true;
        
        if(task.mType == Texture::ST_Destroy)
        {
            switch(reason)
            {
            case Texture::Process::O_DiskToCpu:
            case Texture::Process::O_CpuToDisk:
            case Texture::Process::O_OutGpu:
            case Texture::Process::O_GpuSyncCpu:
                {
                    //OgreProfileExhaustive( "TextureManager::destroyTexture" );
                    ItemList::iterator it = mItemList.find(tex->getName());
                    if(it == mItemList.end())
                    {
                        N_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Texture with name '" + tex->getName().getFriendlyText() + "' not found. Perhaps already destroyed?");
                    }

                    tex->process(Texture::Process::O_Delete);

                    delete tex;
                    mItemMutex.lock();
                    mItemList.erase(it);
                    mItemMutex.unlock();
                }
                break;
            default:
                taskExecuted = false;
                break;
            }
        }
        else
        {
            switch( reason )
            {
            case Texture::Process::O_DiskToCpu:
                //Possible transitions we can do from here:
                //ST_Cpu   -> ST_Disk
                //ST_Cpu   -> Resident
                N_assert1( task.mType != StorageType::ST_Cpu );
                if( task.mType == StorageType::ST_Gpu )
                {
                    taskLoadToSysRamOrResident( tex, task );
                }
                else
                {
                    tex->load(task.mType, tex->getData(0));
                }
                break;
            case Texture::Process::O_CpuToGpu:
                //Possible transitions we can do from here:
                //ST_Disk     -> ST_Cpu
                //ST_Disk     -> Resident
                if( task.mType == StorageType::ST_Gpu || task.mType == StorageType::ST_Cpu )
                {
                    taskLoadToSysRamOrResident( tex, task );
                }
                break;
            case Texture::Process::O_OutGpu:
                //Possible transitions we can do from here:
                //ST_Disk     -> ST_Cpu
                //ST_Disk     -> Resident
                //ST_Cpu   -> ST_Disk
                //ST_Cpu   -> Resident
                {
                    if( task.mType == StorageType::ST_Gpu || task.mType == StorageType::ST_Cpu )
                    {
                        taskLoadToSysRamOrResident( tex, task );
                    }
                    else
                    {
                        const StorageType st = task.mType;
                        N_assert1( st == StorageType::ST_Disk || (tex->getCurrentStorage() == StorageType::ST_Gpu && st == StorageType::ST_Cpu) );

                        tex->load( st, tex->getData( 0 ) );
                    }
                }
                break;
            case Texture::Process::O_GpuSyncCpu:
            case Texture::Process::O_OutGpu:
                //Possible transitions we can do from here:
                //Resident      -> ST_Cpu
                //Resident      -> ST_Disk
                {
                    const StorageType st = task.mType;
                    N_assert1( st == StorageType::ST_Disk || (tex->getCurrentStorage() == StorageType::ST_Gpu && st == StorageType::ST_Cpu) );

                    tex->load( st, tex->getData( 0 ) );
                }
                break;
            default:
                taskExecuted = false;
                break;
            }
        }
        return taskExecuted;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::process(const Item & item)
    {
        processSchedule(item.mTexture, item.mOp, !item.mImmediate);
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::processSchedule(Texture * tex, Texture::Process::Operation reason, bool immediate)
    {
        if(mDoProcess && !immediate)
        {
            //Nested processSchedule calls is a problem. We will execute them later
            mProcessList.push_back(Item(tex, reason));
        }
        else
        {
            TextureTaskList::iterator it = mTextureTaskList.find(tex);
            if(it != mTextureTaskList.end())
            {
                mDoProcess = true;
                TaskList::iterator itTask = it->second.begin();
                bool taskExecuted = executeTask( tex, reason, *itTask );
                if( taskExecuted )
                {
                    it->second.erase( itTask );
                    if( it->second.empty() )
                        mTextureTaskList.erase( it );

                    if( !immediate )
                    {
                        Texture::Process::ItemList tempProcesslist;
                        tempProcesslist.swap(mProcessList);
                        while(!tempProcesslist.empty())
                        {
                            const Item & missed = tempProcesslist.front();
                            processSchedule( missed.mTexture, missed.mOp, true );
                            tempProcesslist.pop_front();
                            //This iteration may have added more entries to mProcessList
                            //We need to execute them right after this entry.
                            tempProcesslist.insert( tempProcesslist.begin(),
                                                            mProcessList.begin(),
                                                            mProcessList.end() );
                            mProcessList.clear();
                        }
                    }
                }
                mDoProcess = false;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::schedule(Texture * dst, const Task & task )
    {
        //getTaskCount should be > 1 because it gets incremented by caller
        N_assert1( dst->getTaskCount() != 0u );

        if( dst->getTaskCount() == 1u || task.mImmediate )
        {
            //If we're here, there are no pending tasks that will perform further work
            //on the dst (with one exception: if isValidImpl does not return true; which
            //means the dst is still in the worker thread and will later get stuffed with
            //the actual data)

            if( task.mType == StorageType::ST_Gpu )
            {
                //If we're here then we're doing one of the following transitions:
                //ST_Disk     -> Resident
                //ST_Cpu   -> Resident
                //If we're going to Resident, then we're currently not. Start loading
                executeTask( dst, Texture::Process::O_OutGpu, task );
            }
            else if( task.mType == StorageType::ST_Cpu )
            {
                const StorageType currentResidency = dst->getCurrentStorage();
                if( currentResidency == StorageType::ST_Disk )
                {
                    //ST_Disk     -> ST_Cpu
                    executeTask( dst, Texture::Process::O_DiskToCpu, task );
                }
                else if( currentResidency == StorageType::ST_Gpu )
                {
                    //Resident      -> ST_Cpu
                    if( dst->isValidImpl() || task.mImmediate )
                        executeTask( dst, Texture::Process::O_OutGpu, task );
                    else
                    {
                        //No pending tasks, but the dst is being loaded. Delay execution
                        mTextureTaskList[dst].push_back( task );
                    }
                }
            }
            else //if( task.mType == StorageType::ST_Disk )
            {
                const StorageType currentResidency = dst->getCurrentStorage();
                if( currentResidency == StorageType::ST_Cpu )
                {
                    //ST_Cpu   -> ST_Disk
                    executeTask( dst, Texture::Process::O_CpuToDisk, task );
                }
                else if( currentResidency == StorageType::ST_Gpu )
                {
                    //Resident      -> ST_Disk
                    if( dst->isValidImpl() || task.mImmediate )
                        executeTask( dst, Texture::Process::O_OutGpu, task );
                    else
                    {
                        //No pending tasks, but the dst is being loaded. Delay execution
                        mTextureTaskList[dst].push_back( task );
                    }
                }
            }
        }
        else
        {
            TextureTaskList::iterator it = mTextureTaskList.find( dst );
            if( it == mTextureTaskList.end() )
            {
                mTextureTaskList[dst] = TaskList();
                it = mTextureTaskList.find( dst );
            }

            it->second.push_back( task );
        }
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::syncToCpu(Texture * dst, bool storage)
    {
        SyncTask entry;

        entry.mTexture = dst;
        entry.mStorage = storage;
        
        entry.mData = dst->getData(0, true);

        NCount mip, numMips = dst->getMipmapCount();
        for(mip = 0; mip < numMips; ++mip)
        {
            NCount width = std::max( dst->getWidth() >> mip, 1u );
            NCount height = std::max( dst->getHeight() >> mip, 1u );
            NCount depth = std::max( dst->getDepth() >> mip, 1u );

            TextureAsync * async = createAsync( width, height, depth, dst->getArray(), dst->getType(), dst->getFormat() );
            async->write(dst, mip, 0);
            entry.mAsyncList.push_back(async);
        }

        mSyncList.push_back( entry );
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::poolAlloc(Texture * tex)
    {
        bool matchFound = false;

        TexturePoolList::iterator it = mPool.begin();
        TexturePoolList::iterator end  = mPool.end();

        while( it != end && !matchFound )
        {
            const TexturePool & pool = *it;

            matchFound =
                    pool.isFree() &&
                    pool.mPool->getWidth() == tex->getWidth() &&
                    pool.mPool->getHeight() == tex->getHeight() &&
                    pool.mPool->getFormat() == tex->getFormat() &&
                    pool.mPool->getMipmapCount() == tex->getMipmapCount() &&
                    pool.mPool->getPoolId() == tex->getPoolId();

            TODO_grow_pool;

            if( !matchFound )
                ++it;
        }

        if( it == end )
        {
            TexturePool newPool;
            newPool.mPool = createTextureImpl( Texture::RT_Disk, 0, Texture::MM_POOL, Texture::T_2DArray );
            const uint16 mArray = getNumSlicesFor( tex, this );

            newPool.mPrimary = false;
            newPool.mNextSlice = 0;
            newPool.mUsedList.reserve( mArray );

            newPool.mPool->setResolution( tex->getWidth(), tex->getHeight(), mArray );
            newPool.mPool->setFormat( tex->getFormat() );
            newPool.mPool->setMipmapCount( tex->getMipmapCount() );
            newPool.mPool->mPoolId = tex->getPoolId() ;

            mPool.push_back( newPool );
            it = --mPool.end();

            it->mPool->load( StorageType::ST_Gpu, 0 );
            it->mPool->notifyGpuData();
        }

        uint16 sliceIdx = 0;
        //See if we can reuse a slot that was previously acquired and released
        if(it->mFreeList.size() )
        {
            sliceIdx = it->mFreeList.back();
            it->mFreeList.pop_back();
        }
        else
        {
            sliceIdx = it->mNextSlice++;
        }
        it->mUsedList.push_back( tex );
        tex->poolImpl( &(*it), sliceIdx );
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::poolFree(Texture * tex)
    {
        //const_cast? Yes. We own it. We could do a linear search to mPool;
        //but it's O(N) vs O(1); and O(N) can quickly turn into O(N!).
        TexturePool * texturePool = const_cast<TexturePool*>( tex->getPool() );
        TextureList::iterator it = std::find(texturePool->mUsedList.begin(), texturePool->mUsedList.end(), tex);
        N_assert1( it != texturePool->mUsedList.end() );
        remove( texturePool->mUsedList, it );

        const uint16 internalSliceStart = tex->getPoolArray();
        if( texturePool->mNextSlice == internalSliceStart + 1u )
            --texturePool->mNextSlice;
        else
            texturePool->mFreeList.push_back( internalSliceStart );

        if( texturePool->empty() && !texturePool->mPrimary )
        {
            //Destroy the pool if it's no longer needed
            delete texturePool->mPool;
            TexturePoolList::iterator itPool = mPool.begin();
            TexturePoolList::iterator enPool = mPool.end();
            while( itPool != enPool && &(*itPool) != texturePool )
                ++itPool;
            mPool.erase( itPool );
        }

        tex->poolImpl( 0, 0 );
    }
    //-----------------------------------------------------------------------------------
    bool OrderByMappedTexture( const MappedTexture *_l, const MappedTexture *_r )
    {
        return _l->getSize() < _r->getSize();
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::processSlice(ImageSlice * slice)
    {
        //OgreProfileExhaustive( "TextureManager::processSlice" );

        Image & img = slice->mImage;
        Texture * tex = slice->mDstTexture;
        TextureOpBuffer * cmdbuf = mThreadData[WorkerThread].mOpBuffer;

        const bool is3DVolume = img.getDepth() > 1u;

        const NCount firstMip = slice->getMinMipLevel();
        const NCount numMips = slice->getMaxMipLevelPlusOne();

        for( NCount i = firstMip; i < numMips; ++i)
        {
            PixelBlock srcBox = img.getPixel(-1, i );
            NCount z, volume = srcBox.getVolume();
            for(z = 0; z < volume; ++z)
            {
                if(slice->isMipSliceQueued(i, z))
                {
                    srcBox.mFront = is3DVolume ? z : 0;
                    srcBox.mArrayIdx = is3DVolume ? 0 : z;
                    srcBox.mDepth = 1u;
                    srcBox.mArray = 1u;

                    MappedTexture * mt = 0;
                    PixelBlock dstBox;
                    MappedTextureList::iterator it = mThreadData[WorkerThread].mMappedList.begin();
                    MappedTextureList::iterator end  = mThreadData[WorkerThread].mMappedList.end();

                    while(it != end && !dstBox.mData)
                    {
                        //request will return false if it could never fit, or the format is not compatible.
                        if((*it)->request(srcBox))
                        {
                            dstBox = (*it)->map(srcBox);
                            if(dstBox.mData)
                                mt = *it;
                        }

                        ++it;
                    }

                    it = mTTData.mFreeMappedList.begin();
                    end  = mTTData.mFreeMappedList.end();

                    while(!dstBox.mData && it != end)
                    {
                        if((*it)->request(srcBox))
                        {
                            dstBox = (*it)->map(srcBox);
                            if(dstBox.mData)
                            {
                                mt = *it;
                                mThreadData[WorkerThread].mMappedList.push_back(*it);
                                it = remove(mTTData.mFreeMappedList, it);
                                //end  = mTTData.mFreeMappedList.end();
                            }
                            else
                            {
                                ++it;
                            }
                        }
                        else
                        {
                            ++it;
                        }
                    }

                    const PixelFormat fpf = PixelUtil::getFamily(srcBox.mFormat);
                    MappedUsageList::iterator itu = mTTData.mMappedUsage.begin();
                    MappedUsageList::iterator enu = mTTData.mMappedUsage.end();

                    //Always split tracking of textures that are bigger than c_maxSplitResolution in any dimension
                    if(srcBox.mWidth >= mTTData.mMappedMaxDim || srcBox.mHeight >= mTTData.mMappedMaxDim)
                    {
                        itu = enu;
                    }

                    while(itu != enu && itu->mFormat != fpf && itu->mSize < mTTData.mMappedMaxSize)
                    {
                        ++itu;
                    }

                    const size_t reqsize = PixelUtil::getSize(srcBox.mWidth, srcBox.mHeight, srcBox.mDepth, srcBox.mArray, fpf);
                    if(itu == enu)
                    {
                        mTTData.mMappedUsage.push_back(MappedUsage(srcBox.mWidth, srcBox.mHeight, srcBox.mDepth, srcBox.mArray, fpf));
                    }
                    else
                    {
                        itu->mWidth = std::max(itu->mWidth, srcBox.mWidth);
                        itu->mHeight = std::max(itu->mHeight, srcBox.mHeight);
                        itu->mSize += reqsize;
                    }

                    mTTData.mProcessSize += reqsize;
                    if( dstBox.mData )
                    {
                        //Upload to staging area. CPU -> GPU
                        dstBox.write( srcBox );
                        if( slice->mDstVolume != std::numeric_limits<NCount>::max() )
                        {
                            if( is3DVolume )
                                srcBox.mFront += slice->mDstVolume;
                            else
                                srcBox.mArrayIdx += slice->mDstVolume;
                        }

                        //Schedule a command to copy from staging to final texture, GPU -> GPU
                        TextureOpBuffer::ReadFromMapped *uploadCmd = cmdbuf->addCommand<TextureOpBuffer::ReadFromMapped>();
                        new (uploadCmd) TextureOpBuffer::ReadFromMapped(mt, dstBox, tex, srcBox, i);
                        //This mip has been processed, flag it as done.
                        slice->unqueueMipSlice( i, z );
                    }
                }
            }
        }

        if( slice->empty() )
        {
            //We're done uploading this task.mImage. Time to run NotifyDataIsReady,
            //unless there's more ImageSlice like us because the Texture is
            //being loaded from multiple files.
            TextureSliceList::iterator it = mTTData.mTextureSliceList.find(tex);
            TextureSliceList::iterator end  = mTTData.mTextureSliceList.end();

            if( it != end )
                it->second.mArray += img.getVolume();

            if( it == end || it->second.mArray == tex->getVolume() )
            {
                if( it != end )
                {
                    if( it->second.mData )
                    {
                        //We couldn't transition earlier, so we have to do it now that we're done

                        TextureOpBuffer::TransitionToLoaded * transitionCmd = cmdbuf->addCommand<TextureOpBuffer::TransitionToLoaded>();
                        new (transitionCmd) TextureOpBuffer::TransitionToLoaded( tex, it->second.mData, it->second.mCpuData ? StorageType::ST_Cpu : StorageType::ST_Gpu );
                    }
                    mTTData.mTextureSliceList.erase( it );
                }

                //Filters will be destroyed by NotifyDataIsReady in main thread
                TextureOpBuffer::NotifyDataIsReady * cmd = cmdbuf->addCommand<TextureOpBuffer::NotifyDataIsReady>();
                new (cmd) TextureOpBuffer::NotifyDataIsReady( tex, slice->mFilterList );
            }
            else
            {
                TextureFilter::FilterBase::destroyFilters( slice->mFilterList );
            }

            slice->destroy();
        }
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::processLoadRequest(const TextureRequest & req)
    {
        //OgreProfileExhaustive( "TextureManager::processLoadRequest TextureRequest for first time" );

        bool reTask = false;

        //WARNING: req.mTexture->isMetadataValid and
        //req.mTexture->getCurrentStorage are NOT thread safe
        //if it's in mTTData.mReTaskList and
        //req.mVolume != 0 or uint32::max
        set<Texture *>::type::iterator itReschedule = mTTData.mReTaskList.find( req.mTexture );
        if( itReschedule != mTTData.mReTaskList.end() )
        {
            if( (req.mVolume == std::numeric_limits<NCount>::max() || req.mVolume == 0) )
            {
                //This is the original first load request that is making it's
                //roundtrip back to us: Worker -> Main -> Worker
                //because the metadata cache lied the last time we parsed it
                mTTData.mReTaskList.erase( itReschedule );
            }
            else
            {
                //The first slice was already rescheduled. We cannot process this request
                //further until the first slice comes back to us (which will also request
                //all the slices again). Drop the whole thing, do not load anything.
                reTask = true;
            }
        }

        DataStreamPtr data;
        if( !req.mImage )
        {
            data = req.mArchive->open( req.mName );
        }

        //Load the image from file into system RAM
        Image imgStack;
        Image *img = req.mImage;
        
        TextureOpBuffer * cmdbuf = mThreadData[WorkerThread].mOpBuffer;

        if( !img )
        {
            img = &imgStack;
            if( !reTask )
            {
                try
                {
                    img->load( data );
                }
                catch( Exception & e )
                {
                    //Log the exception
                    LogManager::getSingleton().logMessage( e.getFullDescription() );
                    //Tell the main thread this happened
                    TextureOpBuffer::ExceptionThrown *exceptionCmd = cmdbuf->addCommand<TextureOpBuffer::ExceptionThrown>();
                    new (exceptionCmd) TextureOpBuffer::ExceptionThrown( req.mTexture, e );
                    //Continue loading using a fallback
                    img->loadDynamicImage( mDummyData, 2u, 2u, 1u, req.mTexture->getType(),
                        PFG_RGBA8_UNORM_SRGB, false, 1u );
                }
            }
        }

        if( (req.mVolume == std::numeric_limits<NCount>::max() || req.mVolume == 0) &&
            req.mTexture->getCurrentStorage() != StorageType::ST_Disk )
        {
            NCount numMipmaps = img->getMipmapCount();
            PixelFormat pf = img->getFormat();
            if( req.mTexture->isPreferSRGB() )
                pf = PixelUtil::getSRGB( pf );
            TextureFilter::FilterBase::simulateFiltersForCacheConsistency(req.mMark, *img, this, numMipmaps, pf);

            //Check the metadata cache was not out of date
            if( req.mTexture->getWidth() != img->getWidth() ||
                req.mTexture->getHeight() != img->getHeight() ||
                (req.mTexture->getVolume() != img->getVolume() &&
                 req.mVolume == std::numeric_limits<NCount>::max()) ||
                req.mTexture->getFormat() != pf ||
                req.mTexture->getMipmapCount() != numMipmaps ||
                (req.mTexture->getType() != img->getTextureType() &&
                 req.mVolume == std::numeric_limits<NCount>::max() &&
                 (img->getHeight() != 1u || req.mTexture->getTextureType() != Texture::T_1D) ) )
            {
                //It's out of date. Send it back to the main thread to remove residency,
                //and they can send it back to us. A ping pong.
                TextureOpBuffer::OutOfDateCache * transitionCmd = cmdbuf->addCommand<TextureOpBuffer::OutOfDateCache>();
                new (transitionCmd) TextureOpBuffer::OutOfDateCache( req.mTexture, *img );
                mTTData.mReTaskList.insert( req.mTexture );
                reTask = true;

                LogManager::getSingleton().logMessage( "[INFO] Texture Metadata cache out of date for " +
                    req.mName + " (Alias: " + req.mTexture->getName() + ")" );
            }
        }

        if(!reTask)
        //{
            //if( req.mAutoImage )
            //    delete req.mImage;
        //}
        //else
        {
            FilterBaseArray filters;
            TextureFilter::FilterBase::createFilters(req.mMark, filters, req.mTexture, *img, req.mCpuData);

            if( req.mVolume == std::numeric_limits<NCount>::max() || req.mVolume == 0 )
            {
                if( req.mTexture->getCurrentStorage() == StorageType::ST_Disk )
                {
                    req.mTexture->setResolution( img->getWidth(), img->getHeight(), img->getVolume() );
                    if( req.mVolume == std::numeric_limits<NCount>::max() )
                    {
                        //If the mTexture had already been set it to 1D
                        //and it is viable, then keep the 1D setting.
                        if( img->getHeight() != 1u || req.mTexture->getType() != Texture::T_1D )
                        {
                            req.mTexture->setType( img->getTextureType() );
                        }
                    }
                    req.mTexture->setFormat( img->getFormat() );
                    req.mTexture->setMipmapCount( img->getMipmapCount() );
                }

                FilterBaseArray::const_iterator itFilters = filters.begin();
                FilterBaseArray::const_iterator enFilters = filters.end();
                while( itFilters != enFilters )
                {
                    (*itFilters)->_executeStreaming( *img, req.mTexture );
                    ++itFilters;
                }

                const bool needsMultipleImages = img->getTextureType() != req.mTexture->getType() && req.mTexture->getType() != Texture::T_1D;
                const bool mustKeepSysRamPtr = req.mCpuData || req.mTexture->getRecoverType() == Texture::RT_CpuGpu;

                N_assert1( !needsMultipleImages || (needsMultipleImages && req.mVolume != std::numeric_limits<NCount>::max()) );

                void * sysRamCopy = 0;
                if( mustKeepSysRamPtr )
                {
                    if( !needsMultipleImages && img->getMipmapCount() == req.mTexture->getMipmapCount() )
                    {
                        //Pass the raw pointer and transfer ownership to Texture
                        sysRamCopy = img->getPixel(-1, 0).mData;
                        img->_setAutoDelete( false );
                    }
                    else
                    {
                        //Posibility 1:
                        //  We're loading this texture in parts, i.e. loading each cubemap face
                        //  from multiple files. Thus the pointer in img is not big enough to hold
                        //  all faces.
                        //
                        //Posibility 2:
                        //  The texture will use a HW mipmap filter. We need to reallocate sysRamCopy
                        //  into something much bigger that can hold all mips.
                        //  img & imgDst will still think they can hold just 1 mipmap, but the
                        //  internal pointer from sysRamCopy has room for when it gets passed
                        //  to the Texture
                        //
                        //Both possibilities can happen at the same time
                        sysRamCopy = reinterpret_cast<Nui8 *>(N_alloc_simd(req.mTexture->getSize()) );

                        Image imgDst;
                        imgDst.load( sysRamCopy, image->getWidth(), image->getHeight(), image->getDepth(), imag->getArray(),
                            image->getFormat(), image->getMipmapCount(), false);

                        NCount mip, numMips = img->getMipmapCount();
                        for(mip = 0; mip < numMips; ++mip)
                        {
                            PixelBlock srcBox = img->getPixel(-1, mip);
                            PixelBlock dstBox = imgDst.getPixel(-1, mip);
                            dstBox.write(srcBox);
                        }
                    }
                }

                if( needsMultipleImages )
                {
                    //We'll need more than one Image to load this mTexture, so track progress
                    mTTData.mTextureSliceList[req.mTexture] = TextureSlice(sysRamCopy, req.mCpuData);
                }

                //Note cannot transition yet if this is loaded using multiple images
                //and we must keep the SysRamPtr from the worker thread
                if( req.mTexture->getCurrentStorage() == StorageType::ST_Disk && (!needsMultipleImages || !mustKeepSysRamPtr))
                {
                    //We have enough to transition the texture to ST_Cpu / Resident.
                    TextureOpBuffer::TransitionToLoaded *transitionCmd = commandBuffer->addCommand<TextureOpBuffer::TransitionToLoaded>();
                    new (transitionCmd) TextureOpBuffer::TransitionToLoaded(req.mTexture, sysRamCopy, req.mCpuData ? StorageType::ST_Cpu : StorageType::ST_Gpu);
                }
            }
            else
            {
                FilterBaseArray::const_iterator itFilters = filters.begin();
                FilterBaseArray::const_iterator enFilters = filters.end();
                while(itFilters != enFilters)
                {
                    (*itFilters)->_executeStreaming(*img, req.mTexture);
                    ++itFilters;
                }

                if( req.mCpuData || req.mTexture->getRecoverType() == Texture::RT_CpuGpu)
                {
                    TextureSliceList::iterator itPartImg = mTTData.mTextureSliceList.find(req.mTexture);

                    N_assert1(itPartImg != mTTData.mTextureSliceList.end());
                    N_assert1(itPartImg->second.mData);

                    Image imgDst;
                    imgDst.load(itPartImg->second.mData, image->getWidth(), image->getHeight(), image->getDepth(), 
                        image->getArray(), image->getFormat(), image->getMipmapCount(), false);

                    NCount mip, numMips = img->getMipmapCount();
                    for(mip = 0; mip < numMips; ++mip)
                    {
                        PixelBlock srcBox = img->getPixel(-1, mip );
                        PixelBlock dstBox = imgDst.getPixel(-1, mip );
                        if( img->getTextureType() != Texture::T_3D )
                            dstBox.mArrayIdx = req.mVolume;
                        else
                            dstBox.mFront = req.mVolume;
                        dstBox.write( srcBox );
                    }

                    if( req.mCpuData )
                    {
                        itPartImg->second.mArray += img->getVolume();

                        if( itPartImg->second.mArray == req.mTexture->getVolume() )
                        {
                            //We couldn't transition earlier, so we have to do it now that we're done
                            TextureOpBuffer::TransitionToLoaded *transitionCmd = commandBuffer->addCommand<TextureOpBuffer::TransitionToLoaded>();
                            new (transitionCmd) TextureOpBuffer::TransitionToLoaded( req.mTexture, itPartImg->second.mData, StorageType::ST_Cpu);
                            mTTData.mTextureSliceList.erase( itPartImg );

                            //Filters will be destroyed by NotifyDataIsReady in main thread
                            TextureOpBuffer::NotifyDataIsReady *cmd = commandBuffer->addCommand<TextureOpBuffer::NotifyDataIsReady>();
                            new (cmd) TextureOpBuffer::NotifyDataIsReady( req.mTexture, filters );
                        }
                        else
                        {
                            TextureFilter::FilterBase::destroyFilters( filters );
                        }
                    }
                }
            }

            if(req.mCpuData)
            {
                if( req.mAutoImage )
                    delete req.mImage;
            }
            else
            {
                //Queue the image for upload to GPU.
                mTTData.mImageList.push_back(ImageSlice( *img, img->getMipmapCount(), img->getVolume(),
                    req.mTexture, req.mVolume, filters ) );

                if( req.mAutoImage )
                    delete req.mImage;

                //Try to upload the queued image right now (all of its mipmaps).
                processSlice( mTTData.mImageList.back());

                if( mTTData.mImageList.back().empty() )
                    mTTData.mImageList.pop_back();
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::swapWorker()
    {
        //OgreProfileExhaustive( "TextureManager::swapWorker" );

        /*
        Thread Input                Thread Output
        ------------------------------------------
        Fresh StagingTextures       Used StagingTextures
        Load Requests               Filled memory
        Empty CommandBuffers        Set textures (resolution, type, pixel format)
                                    Upload commands
                                    Rare Requests

        Load Requests are protected by mRequestMutex (short lock) to prevent
        blocking main thread every time a texture is created.

        Set textures is not protected, so reading pixel format, resolution or type
        could potentially invoke a race condition.

        The rest is protected by mMutex, which takes longer. That means the worker
        thread processes a batch of textures together and when it cannot continue
        (whether it's because it ran out of space or it ran out of work) it delivers
        the commands to the main thread.
        */

        mMutex.lock();

        ThreadData & workerData  = mThreadData[WorkerThread];
        ThreadData & mainData    = mThreadData[MainThread];

        mRequestMutex.lock();
        //Lock while inside mMutex because _update has access to our
        //workerData.mRequestList. We still need mRequestMutex
        //to keep our access to mainData.mRequestList as short as possible
        //(we don't want to block the main thread for long).
        //if(workerData.mRequestList.empty())
        //{
        //    workerData.mRequestList.swap(mainData.mRequestList);
        //}
        //else
        //{
            workerData.mRequestList.insert(workerData.mRequestList.end(), mainData.mRequestList.begin(), mainData.mRequestList.end());
            mainData.mRequestList.clear();
        //}
        mRequestMutex.unlock();

        //TextureOpBuffer * commandBuffer = workerData.mOpBuffer;

        const bool processedAnyImage = workerData.mRequestList.size() || mTTData.mImageList.size();

        //First, try to upload the queued images that failed in the previous iteration.
        ImageSliceList::iterator itQueue = mTTData.mImageList.begin();
        ImageSliceList::iterator enQueue = mTTData.mImageList.end();

        while( itQueue != enQueue && mTTData.mProcessSize < mMaxProcessSize )
        {
            processSlice( *itQueue);
            if( itQueue->empty() )
            {
                itQueue = remove( mTTData.mImageList, itQueue );
                enQueue = mTTData.mImageList.end();
            }
            else
            {
                ++itQueue;
            }
        }

        //Now process new requests from main thread
        RequestList::const_iterator it = workerData.mRequestList.begin();
        RequestList::const_iterator end  = workerData.mRequestList.end();
        NCount prcReqCnt = 0;
        while( it != end && prcReqCnt < mMaxProcessRequest && mTTData.mProcessSize < mMaxProcessSize )
        {
            processLoadRequest(*it);
            ++prcReqCnt;
            ++it;
        }

        //Two cases:
        //  1. We did something this iteration, and finished 100%.
        //     Main thread could be waiting for us. Let them know.
        //  2. We couldn't do everything in this iteration, which means
        //     we need something from main thread. Wake it up.
        //Note that normally main thread isn't sleeping, but it could be if
        //waitAll was called.
        bool wakeUpMainThread = false;
        if( (processedAnyImage && mTTData.mImageList.empty()) || mTTData.mImageList.size() )
        {
            wakeUpMainThread = true;
        }

        workerData.mRequestList.erase(workerData.mRequestList.begin(), workerData.mRequestList.begin() + entriesProcessed);
        
        //The sole purpose of this function is to perform a moving average
        //(https://en.wikipedia.org/wiki/Moving_average) between past records
        //of requests and new ones, where the newest request is given full
        //weight if it needs more memory than past records.
        //This allows us to accomodate to spikes of RAM demand (i.e. we suddenly
        //have a lot of textures to load), while slowly dying off the memory
        //we reserve over time if no more new requests are seen.
        //
        //There may be more than one entry with the same mFormat (due to
        //mTTData.mMappedMaxDim & mMappedMaxSize)
        //but we only perform moving average on one of the entries, while
        //letting all the other entries to die off quickly (by setting a very
        //low mLoopCount)

        MappedUsageList::const_iterator it = mTTData.mMappedUsage.begin();
        MappedUsageList::const_iterator end  = mTTData.mMappedUsage.end();

        while( it != end )
        {
            MappedUsageList::iterator preit = mTTData.mPreMappedUsage.begin();
            MappedUsageList::iterator preend = mTTData.mPreMappedUsage.end();

            while( preit != preend && (preit->mFormat != it->mFormat || preit->mLoopCount <= 2u) )
            {
                ++preit;
            }

            if( preit != preend )
            {
                const uint32 blockWidth = PixelUtil::getBlockWidth( preit->mFormat, false );
                const uint32 blockHeight = PixelUtil::getBlockHeight( preit->mFormat, false );

                //Average current stats with the previous one.
                //But if current one's was bigger, keep current.
                if( blockWidth != 0 )
                {
                    preit->mWidth = std::max( it->mWidth, (preit->mWidth + it->mWidth) >> 1u );
                    preit->mWidth = multiCeil( preit->mWidth, blockWidth );
                }
                else
                {
                    preit->mWidth = it->mWidth;
                }
                if( blockHeight != 0 )
                {
                    preit->mHeight = std::max( it->mHeight, (preit->mHeight + it->mHeight) >> 1u );
                    preit->mHeight = multiCeil( preit->mHeight, blockHeight );
                }
                else
                {
                    preit->mHeight = it->mHeight;
                }
                preit->mSize = std::max( it->mSize, (preit->mSize + it->mSize) >> 1u );
                preit->mLoopCount = reuse_count;
            }
            else
            {
                mTTData.mPreMappedUsage.push_back( *it );
                if( it->mWidth <= mTTData.mMappedMaxDim || it->mHeight <= mTTData.mMappedMaxDim )
                {
                    mTTData.mPreMappedUsage.back().mLoopCount = reuse_count;
                }
                else
                {
                    mTTData.mPreMappedUsage.back().mLoopCount = 2u;
                }
            }

            ++it;
        }

        mTTData.mMappedUsage.clear();

        if( wakeUpMainThread )
            mCond.notifyAll();
        
        mMutex.unlock();
    }
    //-----------------------------------------------------------------------------------
    bool TextureManager::_update(bool sync)
    {
        //OgreProfileExhaustive( "TextureManager::_update" );

#if N_PLAT == N_PLAT_EMSCRIPTEN || OGRE_FORCE_TEXTURE_STREAMING_ON_MAIN_THREAD
        swapWorker();
#endif
        bool isDone = false;

        ThreadData & mainData = mThreadData[MainThread];
        ThreadData & workerData = mThreadData[WorkerThread];
        {
            bool lockSucceeded = false;

            if(sync)
            {
                lockSucceeded = true;
                mMutex.lock();
            }
            else
            {
                lockSucceeded = mMutex.tryLock();
            }

            if( lockSucceeded )
            {
                std::swap(mainData.mOpBuffer, workerData.mOpBuffer);
                mainData.mMappedList.swap(workerData.mMappedList);
                //OgreProfileExhaustive( "TextureManager::fullfillBudget" );
                MappedTextureList tempMT;
                //Ensure mFreeMappedList is sorted in ascending order
                std::sort( mTTData.mFreeMappedList.begin(), mTTData.mFreeMappedList.end(), OrderByMappedTexture );

                MappedUsageList::iterator itu = mTTData.mPreMappedUsage.begin();
                MappedUsageList::iterator enu = mTTData.mPreMappedUsage.end();

                while( itu != enu )
                {
                    --itu->mLoopCount;
                    if( itu->mLoopCount == 0 )
                    {
                        itu = remove( mTTData.mPreMappedUsage, itu );
                        enu = mTTData.mPreMappedUsage.end();
                    }
                    else
                    {
                        size_t oneSliceBytes = PixelUtil::getSize(itu->mWidth, itu->mHeight, 1u, 1u, itu->mFormat);

                        //Round up.
                        const size_t array = (itu->mSize + oneSliceBytes - 1u) / oneSliceBytes;

                        bool isSupported = false;

                        MappedTextureList::iterator it = mTTData.mFreeMappedList.begin();
                        MappedTextureList::iterator end  = mTTData.mFreeMappedList.end();

                        while(!isSupported && it != end)
                        {
                            //Check if the free StagingTextures can take the current usage load.
                            isSupported = (*it)->request(PixelVolume(itu->mWidth, itu->mHeight, 1u, array, itu->mFormat));

                            if( isSupported )
                            {
                                tempMT.push_back( *it );
                                it = mTTData.mFreeMappedList.erase( it );
                                //end  = mTTData.mFreeMappedList.end();
                            }
                            else
                            {
                                ++it;
                            }
                        }

                        if( !isSupported )
                        {
                            //It cannot. We need a bigger MappedTexture (or one that supports a specific format)
                            MappedTexture * newmt = createMapped(PixelVolume(itu->mWidth, itu->mHeight, 1u, array, itu->mFormat), 0.5);
                            tempMT.push_back(newmt);
                        }

                        ++itu;
                    }
                }
                MappedPoolList::const_iterator itBudget = mMappedPoolList.begin();
                MappedPoolList::const_iterator enBudget = mMappedPoolList.end();

                while( itBudget != enBudget )
                {
                    bool isSupported = false;

                    MappedTextureList::iterator it = mTTData.mFreeMappedList.begin();
                    MappedTextureList::iterator end  = mTTData.mFreeMappedList.end();

                    while(!isSupported && it != end )
                    {
                        if( (*it)->getFormat() == itBudget->mFormat )
                        {
                            isSupported = (*it)->request(PixelVolume(itBudget->mDim, itBudget->mDim, 1u, itBudget->mArray, itBudget->mFormat));
                        }

                        if( isSupported )
                        {
                            tempMT.push_back( *it );
                            it = mTTData.mFreeMappedList.erase( it );
                            //end  = mTTData.mFreeMappedList.end();
                        }
                        else
                        {
                            ++it;
                        }
                    }

                    //We now have to look in tempMT
                    //already created a staging texture that fulfills the minimum budget.
                    it = tempMT.begin();
                    end  = tempMT.end();

                    while( it != end && !isSupported )
                    {
                        if( (*it)->getFormat() == itBudget->mFormat )
                        {
                            isSupported = (*it)->request(PixelVolume(itBudget->mDim, itBudget->mDim, 1u, itBudget->mArray, itBudget->mFormat));
                        }

                        ++it;
                    }

                    if( !isSupported )
                    {
                        MappedTexture * newmt = createMapped(PixelVolume(itBudget->mDim, itBudget->mDim, 1u, itBudget->mArray, itBudget->mFormat), 0.5);
                        tempMT.push_back( newmt );
                    }

                    ++itBudget;
                }

                
                MappedTextureList::const_iterator it = mTTData.mFreeMappedList.begin();
                MappedTextureList::const_iterator end  = mTTData.mFreeMappedList.end();

                while( it != end )
                {
                    (*it)->unmap();
                    removeMapped( *it );
                    ++it;
                }

                mTTData.mFreeMappedList.swap(tempMT);
                mTTData.mProcessSize = 0;

                mRequestMutex.lock();
                isDone = mainData.mRequestList.empty() && workerData.mRequestList.empty() && mTTData.mImageList.empty();
                mRequestMutex.unlock();
                
                mMutex.unlock();
            }
        }

        {
            //OgreProfileExhaustive( "TextureManager::_update destroy old StagingTextures" );

            MappedTextureList::iterator it = mFreeMappedList.begin();
            MappedTextureList::iterator end  = mFreeMappedList.end();

            const uint32 numFramesThreshold = N_Only(GBuffer).getDynamicBufferMultiplier() + 2u;

            //They're kept in order.
            while( it != end && N_Only(GBuffer).getFrameCount() - (*it)->getLastFrame() > numFramesThreshold )
            {
                destroyMappedImpl( *it );
                delete *it;
                ++it;
            }

            mFreeMappedList.erase(mFreeMappedList.begin(), it);

            //OgreProfileExhaustive( "TextureManager::_update cmd buffer execution" );
            mainData.mOpBuffer->execute();
            mainData.mOpBuffer->clear();
            
            /////
            MappedTextureList::const_iterator citor = mainData.mMappedList.begin();
            MappedTextureList::const_iterator cend  = mainData.mMappedList.end();

            //while( citor != cend )
            //{
            //    (*citor)->unmap();
            //    ++citor;
            //}
            ////

            //citor = mainData.mMappedList.begin();
            //cend  = mainData.mMappedList.end();

            while( citor != cend )
            {
                (*citor)->unmap();//
                removeMapped( *citor );
                ++citor;
            }

            mainData.mMappedList.clear();
        }

        {
            SyncList readyTextures;

            SyncList::iterator it = mSyncList.begin();
            SyncList::iterator end  = mSyncList.end();

            while( it != end )
            {
                Texture * ttex = it->mTexture;
                Image image; //Use an Image as helper for calculating offsets
                image.load(it->mData, ttex->getWidth(), ttex->getHeight(), ttex->getDepth(), 
                    ttex->getArray(), ttex->getFormat(), ttex->getMipmapCount(), false);

                bool pending = false;
                TextureAsyncList::iterator itTicket = it->mAsyncList.begin();
                TextureAsyncList::iterator enTicket = it->mAsyncList.end();

                while( itTicket != enTicket )
                {
                    TextureAsync * texAsync = *itTicket;
                    if( texAsync )
                    {
                        if(texAsync->isDataValid() )
                        {
                            const Nui8 currentMip = static_cast<Nui8>( itTicket - it->mAsyncList.begin() );
                            PixelBlock dstBox = image.getPixel(-1, currentMip );

                            if( texAsync->getArray() > 1)
                            {
                                dstBox.write(texAsync->map( 0 ));
                                texAsync->unmap();
                            }
                            else
                            {
                                const NCount array = ttex->getArray();

                                for(NCount i = 0; i < array; ++i)
                                {
                                    dstBox.write(texAsync->map(i));
                                    //dstBox.mData = dstBox.getOriginData(0, 0, 1u);
                                    ++dstBox.mArrayIdx;
                                    --dstBox.mArray;
                                    texAsync->unmap();
                                }
                            }

                            destroyAsync(texAsync);

                            *itTicket = 0;
                        }
                        else
                        {
                            pending = true;
                        }
                    }

                    ++itTicket;
                }

                if( pending )
                {
                    ++it;
                }
                else
                {
                    it->mAsyncList.clear();
                    readyTextures.push_back(*it);
                    it = mSyncList.erase(it);
                    end  = mSyncList.end();
                }
            }

            it = readyTextures.begin();
            end  = readyTextures.end();

            while(it != end)
            {
                it->mTexture->notifyCpuData(it->mStorage);
                ++it;
            }
        }

        mRequestMutex.lock();
        mRequestCond.notifyAll();
        mRequestMutex.unlock();

//#if OGRE_DEBUG_MODE && 0
        //dumpStats();
//#endif

        return isDone;
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::waitAll(void)
    {
        //OgreProfileExhaustive( "TextureManager::waitAll" );

        bool bDone = false;
        while( !bDone )
        {
            bool workDone = _update( true );
            bDone = workDone && mSyncList.empty() && mTextureTaskList.empty();
            if( !bDone )
            {
                N_Only(GBuffer)._update();
                if(!workDone)
                //{
                    //We're waiting for GPU -> CPU transfers or for the next task to be executed
                //    Threads::Sleep( 1 );
                //}
                //else
                {
                    mMutex.lock();
                    mCond.wait(mMutex);
                    mMutex.unlock();
                }
            }
//#if OGRE_DEBUG_MEMORY_CONSUMPTION
          //dumpStats();
//#endif
        }
    }
    //-----------------------------------------------------------------------------------
    void TextureManager::wait( Texture * tex, bool metadataOnly, bool cpudata )
    {
        bool bDone = false;
        
        if(cpudata)
        {
            while(!bDone)
            {
                SyncList::iterator it = mSyncList.begin();
                SyncList::iterator end  = mSyncList.end();

                //Only stall for the texture we're looking for, where mStorage == false;
                //since those are from textures currently Resident that will remain Resident.
                //The cases where mStorage == true are handled by the residency transition
                while(it != end && it->mTexture != tex)
                    ++it;

                if(it == end)
                    bDone = true;
                else
                {
                    if(!it->mStorage)
                    {
                        _update( true );
                        N_Only(GBuffer)._update();
                        //Threads::Sleep( 1 );
                    }
                }
            }
        }
        else
        {
            while( !bDone )
            {
                bool workDone = _update( true );
                bDone = workDone && mSyncList.empty() && mTextureTaskList.empty();
                if( !bDone )
                {
                    if( tex->getTaskCount() == 0u )
                    {
                        if( tex->isValid() )
                            bDone = true;
                        else if( metadataOnly && tex->isMetadataValid() )
                            bDone = true;
                    }

                    if( !bDone )
                    {
                        N_Only(GBuffer)._update();
                        if(!workDone )
                        //    Threads::Sleep( 1 );
                        //else
                        {
                            mMutex.lock();
                            mCond.wait(mMutex);
                            mMutex.unlock();
                        }
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void TextureManager::addMappedPool(PixelFormat pf, NCount dim, NCount array)
    {
        if( array > 1u && dim >= mMappedMaxDim )
        {
            LogManager::getSingleton().logMessage(
                "[WARNING] setWorkerThreadMinimumBudget called with mArray = " +
                StrConv::conv( it->mArray ) + " and mDim = " +
                StrConv::conv( it->mDim ) + " which can be very "
                "suboptimal given that mMappedMaxDim = " +
                StrConv::conv( mMappedMaxDim ), LML_CRITICAL );
        }
        mMappedPoolList.push_back(MappedPool(pf, dim, array));
        //Sort in descending order.
        //std::sort( mMappedPoolList.begin(), mMappedPoolList.end(), MappedPool() );
    }
    //-----------------------------------------------------------------------
    bool TextureManager::isMappedPoolExist(PixelFormat pf, NCount dim)
    {
        MappedPoolList::const_iterator it, end = mMappedPoolList.end();

        for(it = mMappedPoolList.begin(); it != end; ++it)
        {
            const MappedPool& temp = *it;
            if(temp.mFormat == pf && temp.mDim == dim)
                return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void TextureManager::setBitDepth(NCount bits, bool reload)
    {
        mBitDepth = bits;

        if(reload)
        {
            Groups::iterator j, jend = mGroups.end();
            for(j = mGroups.begin(); j != jend; ++j)
            {
                Resources::iterator i, iend = j->second.end();
                for(i = j->second.begin(); i != iend; ++i)
                {
                    Texture * tex = static_cast<Texture *>(i->second);
                    // Reload loaded and reloadable texture only
                    if(tex->getState() == LS_LOADED && tex->isReloadable())
                    {
                        tex->unload();
                        if(bits == 16)
                        {
                            tex->addModMark(Texture::MM_DEPTH_BIT_16);
                            tex->removeModMark(Texture::MM_DEPTH_BIT_32);
                        }
                        else if(bits = 32)
                        {
                            tex->addModMark(Texture::MM_DEPTH_BIT_32);
                            tex->removeModMark(Texture::MM_DEPTH_BIT_16);
                        }
                        tex->load();
                    }
                    else
                    {
                        if(bits == 16)
                        {
                            tex->addModMark(Texture::MM_DEPTH_BIT_16);
                            tex->removeModMark(Texture::MM_DEPTH_BIT_32);
                        }
                        else if(bits = 32)
                        {
                            tex->addModMark(Texture::MM_DEPTH_BIT_32);
                            tex->removeModMark(Texture::MM_DEPTH_BIT_16);
                        }
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    Texture * TextureManager::createTexture(ResourceID rid, GroupID gid, GroupID poolId, 
        RecoverType rtype, Nmark usage, Texture::Type type, Nmark filters)
    {
        //OgreProfileExhaustive( "TextureManager::createTexture" );

        if( mItemList.find( rid ) != mItemList.end() )
        {
            N_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "A texture with id '" + rid + "' already exists. " );
        }

        if( type != Texture::T_Unknow && type != Texture::T_2D && usage & Texture::MM_POOLSLICE )
        {
            N_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Only Type2D textures can use Texture::MM_POOLSLICE." );
        }

        Texture * re = createTextureImpl(rtype, rid, usage, type);
        re->mPoolId = poolId;

        mItemMutex.lock();
        mItemList[rid] = Item( rid, gid, re, filters );
        mItemMutex.unlock();

        return re;
    }
    //-----------------------------------------------------------------------------------
    Texture * TextureManager::createTexture(ResourceID rid, GroupID gid, GroupID poolId, 
        RecoverType rtype, Nmark usage, Texture::Type type, Nmark filters)
    {
        Texture * re = 0;

        ItemList::const_iterator it = mItemList.find( rid );
        if( it != mItemList.end() && !it->second.mDestroyTask )
        {
            re = it->second.mTexture;
        }
        else
        {
            if( it->second.mDestroyTask )
            {
                //The use requested to destroy the texture. It will soon become a dangling pointer
                //and invalidate the iterator. Wait for that to happen.
                //We can't use Texture::wait because 'this' will become dangling while
                //inside that function
                waitAll();
            }

            re = createTexture( rid, rtype, usage, type, gid, filters, poolId );
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    Texture* TextureManager::createTexture(ResourceID rid, GroupID gid, GroupID poolId
        RecoverType rtype, Texture::FeatureType type)
    {
        uint32 usage = Texture::MM_POOLSLICE;
        uint32 filters = Texture::MM_MIPMAP;
        Texture::Type texType = Texture::T_2D;
        if( type == Texture::FeatureType::Diffuse || type == Texture::FeatureType::EnvMap )
            usage |= Texture::MM_PreferSRGB;
        if( type == Texture::FeatureType::NormalMap )
            filters |= Texture::MM_Normals;
        if( type == Texture::FeatureType::Monochrome )
            filters |= Texture::MM_Red;
        if( type == Texture::FeatureType::EnvMap )
        {
            usage &= ~static_cast<uint32>( Texture::MM_POOLSLICE );
            texType = Texture::T_CUBE;
        }

        return createTexture( rid, rtype, usage, texType, gid, filters, poolId );
    }
    //------------------------------------------------------------------------
    Texture * TextureManager::createTexture(ResourceID rid, GroupID gid, Texture::Type type,
        PixelFormat pf, const String & file)
    {
        Texture * re = static_cast<Texture *>(create(rid, gid, 0, 0));
        re->setType(type);
        re->setFormat(pf);
        //re->setWidth(w);
        //re->setHeight(h);
        re->setDepth(1);
        re->setMipmapCount(0);
        re->setSrc(file);
        return re;
    }
    //-----------------------------------------------------------------------
    Texture * TextureManager::createTexture(ResourceID rid, GroupID gid, Texture::Type type,
        PixelFormat pf, NCount width, NCount height, NCount depth, NIIi mips, Nmark usage,
        ResLoadScheme * ls, ResResultScheme * rs, bool hwGamma, NCount fsaa, const String & fsaaHint)
    {
        const RenderFeature * caps = N_Engine().getRender()->getFeature();
        if (((type == Texture::T_3D) || (type == Texture::T_2DArray)) && !caps->isSupport(GF_Tex_3D))
            return 0;

        Texture * re = static_cast<Texture *>(create(rid, gid, ls, rs));
        re->setType(type);
        re->setFormat(pf);
        re->setWidth(width);
        re->setHeight(height);
        re->setDepth(depth);
        re->setMipmapCount((mips == -1) ? mMipMapCount : (NCount)mips);
        re->setModeMark(usage);

        re->setHardwareSRGB(hwGamma);
        re->setFSAA(fsaa);
        re->setFSAAExt(fsaaHint);
        re->createInternalImpl();
        re->mInnerValid = true;
        return re;
    }
    //-----------------------------------------------------------------------
    Texture * TextureManager::load(ResourceID rid, GroupID gid, Texture::Type type,
        PixelFormat destpf, NIIi mips, NIIf gamma, Nmark mark)
    {
        TouchResult res = ResourceManager::ref(rid, gid, 0, 0, 0);

        if(res.second)
        {
            Texture * tex = static_cast<Texture *>(res.first);
            tex->setType(type);
            tex->setMipmapCount((mips == -1) ? mMipMapCount : (NCount)mips);
            tex->setGamma(gamma);

            if((mark & Texture::MM_COLOUR_RGB) == 0)
                tex->removeModMark(Texture::MM_COLOUR_RGB);
            if((mark & Texture::MM_SRGB) == 0)
                tex->removeModMark(Texture::MM_SRGB);

            tex->setFormat(destpf);
        }
        Texture * tex = static_cast<Texture *>(res.first);
        tex->load();

        return tex;
    }
    //-----------------------------------------------------------------------
    Texture * TextureManager::load(ResourceID rid, GroupID gid, const Image & img,
        Texture::Type type, PixelFormat destpf, NIIi mips, NIIf gamma, Nmark mark)
    {
        Texture * tex = static_cast<Texture *>(create(rid, gid, 0, 0));

        tex->setType(type);
        tex->setMipmapCount((mips == -1) ? mMipMapCount : (NCount)mips);
        tex->setGamma(gamma);

        if((mark & Texture::MM_COLOUR_RGB) == 0)
            tex->removeModMark(Texture::MM_COLOUR_RGB);
        if((mark & Texture::MM_SRGB) == 0)
            tex->removeModMark(Texture::MM_SRGB);

        tex->setFormat(destpf);
        tex->load(&img);

        return tex;
    }
    //-----------------------------------------------------------------------
    Texture * TextureManager::load(ResourceID rid, GroupID gid, DataStream * stream,
        NCount w, NCount h, Texture::Type type, PixelFormat pf, NIIi mipmap, NIIf gamma,
            Nmark mark)
    {
        Texture * tex = static_cast<Texture *>(create(rid, gid, 0 , 0));

        tex->setType(type);
        tex->setMipmapCount((mipmap == -1) ? mMipMapCount : (NCount)mipmap);
        tex->setGamma(gamma);

        if((mark & Texture::MM_SRGB) == 0)
            tex->removeModMark(Texture::MM_SRGB);

        tex->load(stream, w, h, pf);

        return tex;
    }
    //-----------------------------------------------------------------------
    Texture * TextureManager::plan(ResourceID rid, GroupID gid, Texture::Type type,
        PixelFormat destpf, NIIi mipmapMark, NIIf gamma, Nmark mark)
    {
        TouchResult res = ResourceManager::ref(rid, gid, 0, 0, 0);
        if(res.second)
        {
            Texture * tex = static_cast<Texture *>(res.first);
            tex->setType(type);
            tex->setMipmapCount((mipmapMark == -1) ? mMipMapCount : (NCount)mipmapMark);
            tex->setGamma(gamma);

            if((mark & Texture::MM_COLOUR_RGB) == 0)
                tex->removeModMark(Texture::MM_COLOUR_RGB);
            if((mark & Texture::MM_SRGB) == 0)
                tex->removeModMark(Texture::MM_SRGB);

            tex->setFormat(destpf);
        }

        Texture * tex = static_cast<Texture *>(res.first);
        tex->plan();
        return tex;
    }
    //-----------------------------------------------------------------------
    void TextureManager::convert(Image & dst, Texture * src)
    {
        //NCount mmapcount = mipmaps ? src->getSrcMipmapCount() + 1 : 1;
        NCount mmapcount = 1;
        NCount dataSize = PixelUtil::getSize(src->getWidth(), src->getHeight(), src->getDepth(), src->getArray(), src->getFormat(), mmapcount);

        Nui8 * pixData = (Nui8 *)N_alloc(dataSize);
        Nui8 * currentPixData = pixData;
        for(NCount face = 0; face < src->getUnitCount(); ++face)
        {
            for(NCount mip = 0; mip < mmapcount; ++mip)
            {
                NCount mipDataSize = PixelUtil::getSize(src->getWidth(), src->getHeight(), src->getDepth(), src->getArray() src->getFormat());

                PixelBlock pixBox(src->getWidth(), src->getHeight(), src->getDepth(), src->getFormat(), currentPixData);

                src->getCubeBuffer((Texture::CubeFace)face, mip)->
                    read(Box(0, 0, 0, src->mWidth, src->mHeight, src->mDepth), pixBox);

                currentPixData = currentPixData + mipDataSize;
            }
        }

        dst.load(pixData, src->getWidth(), src->getHeight(), src->getDepth(), src->getUnitCount(),
            src->getFormat(), mmapcount - 1, true);
    }
    //-----------------------------------------------------------------------
    void TextureManager::setPlugin(PluginID pid)
    {
        FactoryList::iterator i, iend = mFactoryList.end();
        for(i = mFactoryList.begin(); i != iend; ++i)
        {
            if(i->first == pid)
            {
                mPlugin = i->second;
                //mPlugin->init();    //Now call overridden Init function
                return;
            }
        }
        mPlugin = 0;
        N_Only(Log).log(_I18n("TextureManager::setPlugin(ENUM) failed setting texture plugin "));
    }
    //---------------------------------------------------------------
    void TextureManager::destroyTexture(PluginID pid, ResourceID rid, GroupID gid)
    {
        FactoryList::iterator i, iend = mFactoryList.end();
        for(i = mFactoryList.begin(); i != iend; ++i)
        {
            //Broadcast to every registered System... Only the true one will destroy texture
            i->second->destroyTexture(rid, gid);
        }
    }
    //---------------------------------------------------------------
    void TextureManager::setPlugin(PluginID pid, TextureFactory * factory)
    {
        N_Only(Log).log( _I18n("Registering Texture Controller: Type = ")
            + pid + _I18n(" Name = ") + factory->getName());

        FactoryList::iterator i;

        for(i = mFactoryList.begin(); i != mFactoryList.end(); ++i)
        {
            if(i->first == pid)
            {
                N_Only(Log).log( _I18n("Shutting Down Texture Controller: ")
                    + i->second->getName()
                    + _I18n(" To be replaced by: ")
                    + factory->getName());

                //i->second->shutdown();                //Only one plugIn of Sent Type can be registered at a time
                                                    //so shut down old plugin before starting new plugin
                i->second = factory;
                // **Moved this line b/c Rendersystem needs to be selected before things
                // such as framelistners can be added
                // factory->Initialise();
                return;
            }
        }
        mFactoryList[pid] = factory;    //If we got here then add it to map
    }
    //---------------------------------------------------------------
    TextureFactory * TextureManager::getPlugin(PluginID pid)
    {
        FactoryList::iterator i, iend = mFactoryList.end();
        for(i = mFactoryList.begin(); i != iend; ++i)
        {
            if(i->first == pid)
                return i->second;
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void TextureManager::run(void *)
    {
        while( !mShuttingDown )
        {
            mRequestMutex.lock();
            mRequestCond.wait(mRequestMutex);
            mRequestMutex.unlock();
            swapWorker();
        }
    }
    //-----------------------------------------------------------------------
    void TextureManager::init()
    {

    }
    //-----------------------------------------------------------------------
}