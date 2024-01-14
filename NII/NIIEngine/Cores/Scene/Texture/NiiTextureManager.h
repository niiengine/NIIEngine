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

#ifndef _NII_TEXTUREMANAGER_H_
#define _NII_TEXTUREMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiTexture.h"

namespace NII
{
    /** 纹理配置
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureDefine : public ShadowAlloc
    {
    public:
        TextureDefine();

        bool operator== (const TextureDefine & o2);
        bool operator!= (const TextureDefine & o2);
    public:
        Nui32 mWidth;           ///< 宽度
        Nui32 mHeight;          ///< 高度
        Nui32 mFSAA;            ///< 抗锯齿等级
        PixelFormat mFormat;    ///< 格式
        GroupID mGroup;         ///< 群组id
    };

    typedef vector<TextureDefine>::type TextureDefineList;
    
    struct _EngineAPI TexturePool
    {
        Texture * mPool;
        SliceList mFreeList;
        TextureList mUsedList;
        Nui16 mNextSlice;
        bool mPrimary;

        bool isFree() const;
        bool empty() const;
    };

    typedef list<TexturePool>::type TexturePoolList;
    
    struct MappedPool
    {
        MappedPool() : mFormat(PF_UNKNOWN), mDim(0), mArray(0) {}
        MappedPool(PixelFormat pf, NCount dim, NCount array) :
            mFormat(pf), 
            mDim(dim), 
            mArray(array) {}

        bool operator()(const MappedPool & o1, const MappedPool & o2) const;
            
        PixelFormat mFormat;
        NCount mDim;
        NCount mArray;
    };

    typedef vector<MappedPool>::type MappedPoolList;
    class ImageSlice;

    /** 纹理管理器类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureManager : public ResourceManager, public Texture::Process, public ThreadMain, public Singleton<TextureManager>
    {
    public:
        struct Metadata
        {
            Metadata();
            
            ResourceID mName;
            GroupID mPoolId;
            PixelFormat mFormat;
            Texture::Type mType;
            NCount mWidth;
            NCount mHeight;
            NCount mDepth;
            NCount mArray;
            NCount mMipmapCount;
        };

        typedef map<ResourceID, Metadata>::type MetadataList;

        struct Item
        {
            Item() : mTexture(0) {}
            Item(ResourceID rid, GroupID gid, Texture * tex, Nmark mark) :
                mRID(rid), mGroup(gid), mTexture(tex), mMark(mark), mDestroyTask(false)
            {
            }
            
            Texture * mTexture;
            Nmark mMark;
            ResourceID mRID;
            GroupID mGroup;
            bool mDestroyTask;
        };
        typedef map<ResourceID, Item>::type ItemList;
    public:
        TextureManager();
        virtual ~TextureManager();

        /** 设置类型
        @version NIIEngine 5.0.0
        */
        inline void setMipmapGen(Texture::MipmapGenType type)       { mMipmapGen = type; }

        /** 获取类型
        @version NIIEngine 5.0.0
        */
        inline Texture::MipmapGenType getMipmapGen() const          { return mMipmapGen; }

        /** 设置类型
        @version NIIEngine 5.0.0
        */
        inline void setCubemapMipmapGen(Texture::MipmapGenType type){ mMipmapGenCube = type; }

        /** 获取类型
        @version NIIEngine 5.0.0
        */
        inline Texture::MipmapGenType getCubemapMipmapGen() const   { return mMipmapGenCube; }

        /** 更新
        @param sync 等待更新完成
        @version NIIEngine 5.0.0
        */
        bool _update(bool sync);

        /** 等待所有纹理数据完成
        @version NIIEngine 5.0.0
        */
        void waitAll();

        /** 等待纹理数据完成
        @version NIIEngine 5.0.0
        */
        void wait(Texture * tex, bool metadata, bool cpudata = false);
        
        /** 添加映射池
        @version NIIEngine 5.0.0
        */
        void addMappedPool(PixelFormat pf, NCount dim, NCount array);
        
        /** 映射池是否存在
        @version NIIEngine 5.0.0
        */
        bool isMappedPoolExist(PixelFormat pf, NCount dim);
        
        /** 获取所有映射池
        @version NIIEngine 5.0.0
        */
        inline const MappedPoolList & getMappedPoolList() const     { return mMappedPoolList; }

        /** 创建纹理池
        @version NIIEngine 3.0.0
        */
        Texture * createPool(GroupID pid, NCount width, NCount height, NCount array, NCount mipmaps, PixelFormat pf);

        /** 纹理池是否存在
        @version NIIEngine 5.0.0
        */
        bool isPoolExist(GroupID pid) const;
        
        /** 纹理池是否存在
        @version NIIEngine 5.0.0
        */
        bool isPoolExist(GroupID pid, NCount width, NCount height, NCount mipmaps, PixelFormat pf) const;
        
        /** 获取纹理池列表
        @version NIIEngine 5.0.0
        */
        inline const TexturePoolList & getPoolList() const          { return mPool; }

        /** 纹理池中分配纹理
        @version NIIEngine 5.0.0
        */
        void poolAlloc(Texture * tex);

        /** 纹理池删除纹理
        @version NIIEngine 5.0.0
        */
        void poolFree(Texture * tex);

        /** 
        @version NIIEngine 5.0.0
        */
        MappedTexture * createMapped(const PixelVolume & vol, NIIf usedThreshold = 0.25);
            
        /** 
        @version NIIEngine 5.0.0
        */
        void removeMapped(MappedTexture * mapped);
        
        /**
        @version NIIEngine 5.0.0
        */
        void destroyAllMapped();
        
        /**
        @version NIIEngine 5.0.0
        */
        void setMappedMaxDim(NCount dim);
        
        /**
        @version NIIEngine 5.0.0
        */
        NCount getMappedMaxDim() const;
        
        /** 
        @version NIIEngine 5.0.0
        */
        void setMappedMaxSize(NCount size);
        
        /** 
        @version NIIEngine 5.0.0
        */
        NCount getMappedMaxSize() const;
        
        /** 
        @version NIIEngine 5.0.0
        */
        inline void setMappedPoolMaxSize(NCount size)       { mMappedPoolMaxSize = size; }
        
        /** 
        @version NIIEngine 5.0.0
        */
        inline NCount getMappedPoolMaxSize() const          { return mMappedPoolMaxSize; }
        
        /** 获取纹理项目
        @version NIIEngine 5.0.0
        */
        inline const ItemList & getItemList() const         { return mItemList; }

        /**
        @version NIIEngine 5.0.0
        */
        TextureAsync * createAsync(NCount width, NCount height, NCount depth, NCount array, Texture::Type type, PixelFormat pf);
                    
        /**
        @version NIIEngine 5.0.0
        */
        void destroyAsync(TextureAsync * async);
        
        /**
        @version NIIEngine 5.0.0
        */
        void destroyAllAsync();

        /**
        @version NIIEngine 5.0.0
        */
        void _updateMetadata(Texture * tex);
        
        /**
        @version NIIEngine 5.0.0
        */
        void _removeMetadata(Texture * tex);
        
        /**
        @version NIIEngine 5.0.0
        */
        void readMetadata(DataStream * in, bool createpool);
        
        /**
        @version NIIEngine 5.0.0
        */
        void writeMetadata(String & out);
        
        /**
        @version NIIEngine 5.0.0
        */
        inline void setMaxProcessSize(NCount size)          { mMaxProcessSize = size; }
        
        /**
        @version NIIEngine 5.0.0
        */
        inline NCount getMaxProcessSize() const             { return mMaxProcessSize; }
        
        /**
        @version NIIEngine 5.0.0
        */
        bool executeTask(Texture * tex, Texture::Process::Operation reason, const Texture::Task & task);

        /**
        @version NIIEngine 5.0.0
        */
        virtual void process(const Item & item);

        /**
        @version NIIEngine 5.0.0
        */
        void schedule(Texture * dst, const Task & task);

        /**
        @version NIIEngine 5.0.0
        */
        void syncToCpu(Texture * dst, bool storage);
        
        /** 设置默认纹理位深
        @param[in] bit 位深
        @param[in] reload 重载所有纹理
        @version NIIEngine 3.0.0
        */
        void setBitDepth(NCount bit, bool reload = true);

        /** 获取默认纹理位深
        @version NIIEngine 3.0.0
        */
        inline NCount getBitDepth() const                   { return mBitDepth;  }

        /** 设置默认纹理LOD数量
        @note 默认为 0.
        @param[in] c LOD数量
        @version NIIEngine 3.0.0
        */
        inline void setMipMapCount(NCount c)                { mMipMapCount = c;  }

        /** 获取默认纹理LOD数量
        @version NIIEngine 3.0.0
        */
        inline NCount getMipMapCount() const                { return mMipMapCount; }
        
        /** 创建纹理
        @param[in] rid 资源句柄
        @param[in] w 宽
        @param[in] h 高
        @param[in] pf 像素格式
        @version NIIEngine 3.0.0 高级api
        */
        Texture * createTexture(ResourceID rid, GroupID gid, GroupID poolId, RecoverType rtype, Texture::FeatureType type);

        /** 创建纹理
        @param[in] rid 资源句柄
        @param[in] w 宽
        @param[in] h 高
        @param[in] pf 像素格式
        @version NIIEngine 3.0.0 高级api
        */
        Texture * createTexture(ResourceID rid, GroupID gid, GroupID poolId, RecoverType rtype, Texture::Type type = Texture::T_2D, PixelFormat pf = PF_A8B8G8R8, const String & file = N_StrBlank);

        /** 创建纹理
        @param rid 句柄
        @param gid 资源组
        @param d 深度
        @version NIIEngine 3.0.0 高级api
        */
        Texture * createTexture(ResourceID rid, GroupID gid, GroupID poolId, RecoverType rtype, Texture::Type type, 
            PixelFormat pf, NCount w, NCount h, NCount d = 1, NIIi mipmaps = -1, Nmark usage = Texture::MM_MIPMAP_AUTO | Buffer::M_NORMAL ,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, bool HWsRGB = false, 
            NCount fsaa = 0, const String & fsaaHint = StrUtil::WBLANK);
            
        /**
        @version NIIEngine 5.0.0
        */
        void destroyTexture(Texture * texture);
        
        /**
        @version NIIEngine 5.0.0
        */
        void destroyTexture(PluginID pid, ResourceID rid, GroupID gid = GroupDefault);
        
        /**
        @version NIIEngine 5.0.0
        */
        void destroyAllTexture();
        
        /** 获取
        @version NIIEngine 3.0.0
        */
        virtual void getTexture(const TextureDefineList & in, TextureList & out);

        /**
        @version NIIEngine 3.0.0
        */
        virtual Texture * getDummy(PixelFormat format);

        /** 回收所有阴影纹理
        @version NIIEngine 3.0.0
        */
        virtual void recoverShadow();

        /** 清除所有阴影纹理
        @version NIIEngine 3.0.0
        */
        virtual void clearShadow();

        /** 安排资源
        @version NIIEngine 3.0.0
        */
        virtual Texture * plan(ResourceID rid, GroupID gid, Texture::Type type = Texture::T_2D,
            PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1, NIIf gamma = 1.0f,
            Nmark mark = Texture::MM_FRAME_RGB | Texture::MM_FRAME_A);

        /** 加载
        @version NIIEngine 3.0.0
        */
        virtual Texture * load(ResourceID rid, GroupID gid, Texture::Type type = Texture::T_2D,
            PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1, NIIf gamma = 1.0f,
            Nmark mark = Texture::MM_FRAME_RGB | Texture::MM_FRAME_A);

        /** 从图片中加载
        @param[in] img 像素来源
        @version NIIEngine 3.0.0
        */
        virtual Texture * load(ResourceID rid, GroupID gid, const Image & img,
            Texture::Type type = Texture::T_2D, PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1,
            NIIf gamma = 1.0f, Nmark mark = Texture::MM_FRAME_RGB | Texture::MM_FRAME_A);

        /** 从流中加载
        @param[in] stream 像素来源,未编码的数据流
        @version NIIEngine 3.0.0
        */
        virtual Texture * load(ResourceID rid, GroupID gid, DataStream * stream, NCount w, NCount h, 
            Texture::Type type = Texture::T_2D, PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1,
            NIIf gamma = 1.0f, Nmark mark = Texture::MM_FRAME_RGB | Texture::MM_FRAME_A);

        /** 获取渲染系统实际使用的格式
        @version NIIEngine 3.0.0 高级api
        */
        virtual PixelFormat getFormat(Texture::Type type, PixelFormat pf, Nmark mark) = 0;

        /** 把纹理转换成图片
        @note 仅用纹理的顶层mimap作为转换
        @param[in] dst 目标图片
        @param[in] src 源纹理
        @version NIIEngine 3.0.0
        */
        static void convert(Image & dst, Texture * src);

        /** 获取错误加载时使用的纹理
        @version NIIEngine 3.0.0
        */
        inline Texture * getWarning() const                 { return mWarningTexture; }
        
        /**
        @version NIIEngine 6.0.0
        */
        void setPlugin(PluginID pid);

        /**
        @version NIIEngine 6.0.0
        */
        inline TextureFactory * getPlugin() const           { return mPlugin; }

        /**
        @version NIIEngine 6.0.0
        */
        TextureFactory * getPlugin(PluginID pid);

        /**
        @version NIIEngine 6.0.0
        */
        void setPlugin(PluginID pid, TextureFactory * factory);
    protected:
        /// @copydetails ThreadMain::run
        void run(void * arg); 

        /// @copydetails ResourceManager::init
        void init();
        
        /**
        @version NIIEngine 5.0.0
        */
        void destroyAll();
        
        /**
        @version NIIEngine 5.0.0
        */
        void destroyAllPool();

        /**
        @version NIIEngine 5.0.0
        */
        virtual Texture * createTextureImpl(ResourceID rid, Texture::Type type, Nmark usage, RecoverType rtype) = 0;

        /**
        @version NIIEngine 5.0.0
        */
        virtual MappedTexture * createMappedImpl(const PixelVolume & vol)=0;

        /**
        @version NIIEngine 5.0.0
        */
        virtual void destroyMappedImpl(MappedTexture * mapped) = 0;

        /**
        @version NIIEngine 5.0.0
        */
        virtual TextureAsync * createAsyncImpl(NCount width, NCount height, NCount array, Texture::Type type, PixelFormat pf) = 0;
        
        /**
        @version NIIEngine 5.0.0
        */
        void swapWorker();
    
        /**
        @version NIIEngine 5.0.0
        */
        bool _setMetadata(Texture * tex);
        
        /**
        @version NIIEngine 5.0.0
        */
        void processSchedule(Texture * texture, Texture::Process::Operation reason, bool immediate);

        /**
        @version NIIEngine 5.0.0
        */
        void processLoadRequest(const TextureRequest & req);
        
        /**
        @version NIIEngine 5.0.0
        */
        void processSlice(ImageSlice * slice);
    protected:
        typedef map<PluginID, TextureFactory *>::type FactoryList;
        typedef vector<Texture::Task>::type TaskList;
        typedef map<Texture *, TaskList>::type TextureTaskList;
        
        struct SyncTask
        {
            Texture * mTexture;
            TextureAsyncList mAsyncList;
            Nui8 * mData;
            bool mStorage;
        };
        typedef vector<SyncTask>::type SyncList;

        TextureThreadData * mTTData;
        Texture::MipmapGenType mMipmapGen;
        Texture::MipmapGenType mMipmapGenCube;
        ThreadCondition mRequestCond;
        ThreadCondition mCond;
        ThreadMutex mRequestMutex;
        ThreadMutex mMutex;
        ThreadMutex mItemMutex;
        
        ItemList mItemList;
        TexturePoolList mPool;
        MappedPoolList mMappedPoolList;
        MetadataList mMetadataList;
        TextureList mShadowTextureList;
        TextureList mDummyTextureList;
        
        NCount mMappedPoolMaxSize;
        NCount mMaxProcessRequest;
        NCount mMaxProcessSize;

        MappedTextureList mMappedList;
        MappedTextureList mFreeMappedList;

        SyncList mSyncList;
        TextureAsyncList mAsyncList;
        TextureTaskList mTextureTaskList;
        Texture::Process::ItemList mProcessList;

        TextureFactory * mPlugin;
        FactoryList mFactoryList;
        
        Nui8 mDummyData[128];
        
        NCount mBitDepth;
        NCount mMipMapCount;
        Texture * mWarningTexture;
        bool mDoProcess;
        bool mShuttingDown;
    };
}
#endif