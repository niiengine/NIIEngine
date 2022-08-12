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

#ifndef _NII_TEXTURE_H_
#define _NII_TEXTURE_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
#include "NiiRenderSysData.h"
#include "NiiBuffer.h"
#include "NiiImage.h"

namespace NII
{
    /// FSAA (MSAA, CSAA,...)
    struct _EngineAPI SampleType
    {
    public:
        enum CoordType
        {
            CT_Unknow,
            CT_Default,
            CT_Standard,
            CT_Center
        };
    public:
        SampleType( Nui8 msaa = 1u, CoordType type = CT_Unknow ) :
            mMultiSampling( msaa ),
            mCoverageSampling( 0 ),
            mType( type )
        {
        }
        
        explicit SampleType( const String & hint )
        {
            parseString( hint );
        }

        bool operator==( const SampleType & o ) const
        {
            return mMultiSampling == o.mMultiSampling && mCoverageSampling == o.mCoverageSampling &&
                   mType == o.mType;
        }
        
        inline bool isMultisample() const              { return mMultiSampling > 1u; }

        inline Nui8 getMultiSampling() const           { return mMultiSampling; }
        
        inline Nui8 getCoverageSampling() const        { return mCoverageSampling; }
        
        inline Nui8 getMaxSampling() const             { return std::max( mCoverageSampling, mMultiSampling ); }
        
        inline CoordType getType() const               { return mType; }

        void set( Nui8 msaa, Nui8 csaa, CoordType type);

        void setMsaa(Nui8 msaa, CoordType type = CT_Unknow);

        bool isMsaa() const;

        /** Set CSAA by NVIDIA's marketing names e.g.
                8x CSAA call setCsaa( 8u, false )
                8x CSAA (Quality) then call setCsaa( 8u, true )
                16x CSAA call setCsaa( 16u, false )
                16x CSAA (Quality) then call setCsaa( 16u, true )
        @param cass Marketing value. Can be 8 or 16
        @param quality True to use the 'quality' variation, false otherwise
        */
        void setCsaa( Nui8 cass, bool quality );

        /** Set EQAA by its marketing number (which coincides with its technical spec) e.g.
                2f4x EQAA call setEqaa( 2u, 4u )
                4f8x EQAA call setEqaa( 4u, 8u )
                8f16x EQAA call setEqaa( 8u, 16u )
        */
        void setEqaa( Nui8 msaa, Nui8 csaa );

        void parseString( const String & hint );
        
        /// Appends the FSAA description to the string
        void getFsaaDesc( LwString &outFsaaSetting ) const;
        
        /// Appends the FSAA description to the string
        void getFsaaDesc( String &outFsaaSetting ) const;
    protected:
        Nui8 mMultiSampling;
        Nui8 mCoverageSampling;
        CoordType mType;
    };

    /** 纹理
    @remark 蒙皮使用的像素
    @par
        关于多线程加载问题,纹理在处理上也仅仅只能在图片解码过程中使用动态加载技术,
        因为各种图形驱动中原数据到纹理创建都一个函数就把它创建出来的,如果图形驱动
        并没有多线程控制,即使这个过程会瞬间产生顿卡,也没有任何办法去解决.还有一点
        就是多线程在创建纹理这个话题上不是一个很好控制的东西(试想下如果主控线程等
        待一个工作繁忙的线程,那么主控线程也仅仅只能等待它完成后才能继续下面的工作)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Texture : public Resource, public RenderSysData, public Buffer, public GpuResource
    {
    public:
        friend class TextureManager;
    public:
    
        class _EngineAPI TextureGpuListener
        {
        public:
            virtual ~TextureGpuListener();

            enum Reason
            {
                Unknown,
                /// ST_Disk   -> ST_Cpu
                FromStorageToSysRam,
                /// ST_Cpu -> ST_Disk
                FromSysRamToStorage,
                /// ST_Disk   -> Resident
                /// ST_Cpu -> Resident
                /// See ReadyForRendering
                GainedResidency,
                /// Resident    -> ST_Disk
                /// Resident    -> ST_Cpu
                LostResidency,
                PoolTextureSlotChanged,
                /// Only called while TextureGpu is still Resident, and strategy is
                /// CPUGPU. This listener happens when something was
                /// done to the TextureGpu that modifies its contents in the GPU, and
                /// were thus forced to sync those values back to SystemRam.
                /// This listener calls tells that sync is over.
                ResidentToSysRamSync,
                /// The Metadata cache was out of date and we had to do a ping-pong.
                /// Expect this to be followed by at least LostResidency and GainedResidency calls
                ///
                /// This is very important, because if you were expecting certain sequence of calls
                /// (e.g. you were expecting a LostResidency soon to arrive), expect that to be
                /// changed.
                ///
                /// See TextureGpuManager for details about the metadata cache.
                MetadataCacheOutOfDate,
                /// Called when the worker thread caught an exception. This exception has already
                /// been logged, and the texture resumed loading normally with a white 2x2 RGBA8 fallback.
                ///
                /// This listener will get called from the main thread.
                ///
                /// The texture may still have pending residency transitions (e.g. it may still be
                /// loading the 2x2 fallback)
                ///
                /// Cast Exception *e = reinterpret_cast<Exception*>( extraData );
                /// to know more info
                ExceptionThrown,
                /// Requested FSAA (MSAA / CSAA / EQAA / etc) is not supported by the API, and thus
                /// the setting had to be downgraded. Note this may happen on device lost, and a new
                /// GPU became in use; thus it's possible for a TextureGpu to initially support
                /// certain FSAA but later change.
                FsaaSettingAlteredByApi,
                /// This Reason is called when TextureGpu::notifyDataIsReady is called.
                /// This normally means worker thread is done loading texture from file
                /// and uploading it to GPU; and can now be used for rendering.
                /// It does NOT mean that Ogre has finished issueing rendering commands to
                /// a RenderTexture and is now ready to be presented to the monitor.
                ReadyForRendering,
                Deleted
            };

            /// Called when a TextureGpu changed in a way that affects how it is displayed:
            ///		1. TextureGpu::notifyDataIsReady got called (texture is ready to be displayed)
            ///		2. Texture changed residency status.
            ///     3. Texture is being deleted. It won't be a valid pointer after this call.
            virtual void notifyTextureChanged(TextureGpu * texture, TextureGpuListener::Reason reason, void * extraData) = 0;

            /// Return true if this TextureGpu should likely stay loaded or else
            /// graphical changes could occur.
            ///
            /// Return false if it is certainly safe to unload.
            virtual bool shouldStayLoaded(TextureGpu * texture)                    { return true; }
        };
    
        enum StorageType
        {
            ST_Disk,
            ST_Cpu,
            ST_Gpu
        };

        enum RecoverType
        {
            RT_Disk,
            RT_Cpu,
            RT_CpuGpu
        };
        
        /** 枚举确定纹理用法
        @version NIIEngine 3.0.0
        */
        enum MemMode
        {
            MM_FRAME_RGB        = Buffer::M_EXT1,   ///< 颜色缓存(Frame)
            MM_FRAME_A          = Buffer::M_EXT2,   ///< A缓存(Frame)
            MM_FRAME_DEPTH      = Buffer::M_EXT3,   ///< 深度缓存(Frame)
            MM_FRAME_STENCIL    = Buffer::M_EXT4,   ///< 蒙板缓存(Frame)
            MM_FRAME_SRGB       = Buffer::M_EXT5,   ///< 硬件色域校正
            MM_FRAME_SRGB_Write = Buffer::M_EXT6,   ///< 硬件色域校正(写入)
            MM_FRAME_SRGB_Read  = Buffer::M_EXT7,   ///< 硬件色域校正(读取)
            MM_COLOUR_RGB       = Buffer::M_EXT8,   ///< 包含RGB颜色(默认)
            MM_COLOUR_ALPHA     = Buffer::M_EXT9,   ///< 包含A成分(默认)
            MM_MIPMAP_DEFAULT   = Buffer::M_EXT10,  ///< N_TEX_MIPMAP
            MM_MIPMAP_AUTO      = Buffer::M_EXT11,  ///< 自动生成mipmaps
            MM_MIPMAP_HW        = Buffer::M_EXT12,  ///< 硬件生成mipmaps
            MM_DEPTH_BIT_16     = Buffer::M_EXT13,  ///< MM_DEPTH_BIT_X MM_DEPTH_FLOAT 只能其一
            MM_DEPTH_BIT_32     = Buffer::M_EXT14,  ///< MM_DEPTH_BIT_X MM_DEPTH_FLOAT 只能其一
            MM_DEPTH_FLOAT      = Buffer::M_EXT15,  ///< MM_DEPTH_BIT_X MM_DEPTH_FLOAT 只能其一
            
            /// Texture cannot be used as a regular texture (bound to SRV in D3D11 terms)
            NotTexture          = 1u << 0u,
            /// Texture can be used as an RTT (FBO in GL terms)
            RenderToTexture     = 1u << 1u,
            /// Texture can be used as an UAV
            Uav                 = 1u << 2u,
            /// Texture can use mipmap autogeneration. This flag is NOT necessary
            /// for TextureFilter::TypeGenerateHwMipmaps, as this filter will
            /// create a temporary resource.
            /// AllowAutomipmaps is thought for RenderToTexture textures.
            AllowAutomipmaps    = 1u << 3u,
            /// Texture will auto generate mipmaps every time it's dirty, automatically.
            /// Requires AllowAutomipmaps.
            AutomipmapsAuto     = 1u << 4u,
            /** MSAA rendering is an antialiasing technique. MSAA works by rendering
                to a special surface (an MSAA surface) and once we're done, "resolving"
                from MSAA surface into a regular texture for later sampling.
            @par
                Without explicit resolves, Ogre will automatically resolve the MSAA
                surface into the texture whenever it detects you will be sampling
                from this texture.
            @par
                However there are cases where you want to directly access the MSAA
                surface directly for advanced special effects (i.e. via Texture2DMS in
                HLSL).
                For cases like that, use MsaaExplicitResolve; which will let you to
                manually manage MSAA surfaces and when you want to resolve it.
            */
            MsaaExplicitResolve = 1u << 5u,
            /// When present, you may be creating another TextureGpu that accesses
            /// the internal resources of this TextureGpu but with a different format
            /// (e.g. useful for viewing a PFG_RGBA8_UNORM_SRGB as PFG_RGBA8_UNORM)
            Reinterpretable     = 1u << 6u,
            /// Prefer loading FROM FILES as sRGB when possible.
            /// e.g. load PFG_RGBA8_UNORM as PFG_RGBA8_UNORM_SRGB
            /// This flag does not affect RenderTextures, UAVs, or manually created textures.
            /// If you're manually creating sRGB textures, set PFG_RGBA8_UNORM_SRGB directly
            PrefersLoadingFromFileAsSRGB = 1u << 7u,
            /// Indicates this texture contains a RenderWindow. In several APIs render windows
            /// have particular limitations:
            ///     * Cannot be sampled as textures (i.e. NotTexture will be set)
            ///     * OpenGL cannot share the depth & stencil buffers with other textures.
            ///     * Metal requires special maintenance.
            ///     * etc.
            RenderWindowSpecific    = 1u << 9u,
            RequiresTextureFlipping = 1u << 10u,
            /// Indicates this texture will be filled by the user, and won't be loaded
            /// from file or a listener from within a worker thread. This flag
            /// is implicit if NotTexture, RenderToTexture or Uav are set.
            ManualTexture           = 1u << 11u,
            /// When not present:
            /// The Texture is exactly the type requested (e.g. 2D texture won't
            /// get a 2D array instead)
            /// While a texture is transitioning to Resident, no 64x64 is used,
            /// but the 4x4 dummy one will be used instead (blank texture).
            ///
            /// When this bit is set:
            /// The Texture can be of a different type. Most normally we’ll treat
            /// 2D textures internally as a slice to a 2D array texture.
            /// Ogre will keep three API objects:
            ///     1. A single 4x4 texture. Blank.
            ///     2. An array of 2D textures of 64x64. One of its slices will
            ///        contain the mips of the texture being loaded
            ///     3. An array of 2D textures in which one of its slices the fully
            ///        resident texture will live.
            /// Each time we change the internal API object, HlmsDatablocks need to be
            /// notified so it can pack the arrays, update the slices to the GPU, and
            /// compute the texture hashes.
            /// All of that (except updating slices to the GPU) can be done in a
            /// worker thread, then all the values swapped w/ the Datablock’s.
            AutomaticBatching   = 1u << 12u,
            /// For internal use. Indicates whether this texture is the owner
            /// of a TextureGpuManager::TexturePool, which are used
            /// to hold regular textures using AutomaticBatching
            PoolOwner           = 1u << 13u
        };
        
        /** 纹理类型
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_Unknow        = 0,
            T_Array         = 1,
            T_Frame         = 10,   ///< FBO缓存
            T_1D            = 20,   ///< 1D纹理
            T_1DArray       = 21,   ///< 1D纹理数组
            T_2D            = 30,   ///< 2D纹理
            T_2D_RECT       = 40,
            T_2DArray       = 31,   ///< 2D纹理数组
            T_3D            = 50,   ///< 3D纹理
            T_3DArray       = 51,   ///< 3D纹理数组
            T_CUBE          = 60,   ///< 方体映射使用3D纹理坐标
            T_CUBEArray     = 61,   ///< 方体映射数组
            T_CUBE_2D       = 70    ///< 方体映射形态的2D纹理
        };

        /// 正方映射面类型
        enum CubeFace
        {
            CF_RIGHT    = 0,
            CF_LEFT     = 1,
            CF_UP       = 2,
            CF_DOWN     = 3,
            CF_FRONT    = 4,
            CF_BACK     = 5
        };
    public:
        TextureGpu(RecoverType strategy, GBufferManager * mag, IdString name, uint32 textureFlags, 
            Type type, TextureGpuManager * mag);
                    
        virtual ~TextureGpu();

        StorageType getResidency() const            { return mStorage; }
        
        void _setNextResidency(StorageType next )   { mNextStorage = next; }
        
        StorageType getNextResidency() const        { return mNextStorage; }

        RecoverType getGpuStrategy() const          { return mRecoverType;}

        void _addPendingResidency( uint32 value )   { mPendingResidencyChanges += value;}

        uint32 getPendingResidency() const          { return mPendingResidencyChanges; }
        
        /** 简易构造函数
        @version NIIEngine 3.0.0
        */
        Texture(ResourceID rid, GroupID gid, LangID lid = N_PrimaryLang);

        /** 简易构造函数
        @param[in] file 本地文件获取包含协议的文件全名字
        @version NIIEngine 3.0.0
        */
        Texture(ResourceID rid, const StringList & file, GroupID gid, LangID lid = N_PrimaryLang);

        /** 机制构造函数
        @param[in] ls 加载策略
        @param[in] rs 结果策略
        @version NIIEngine 3.0.0
        */
        Texture(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs= 0, LangID lid = N_PrimaryLang);

        virtual ~Texture();

        using Resource::load;

        /** 从另个对象中加载
        @version NIIEngine 3.0.0
        */
        virtual void load(const Texture * src);

        /** 从一个图象中加载数据转换成纹理
        @remark 最基础的方法,从一个图片对象中加载纹理
        @param[in] src 图象来源体
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0
        */
        void load(const Image * src);

        /** 从一个图象集合中加载转换成纹理
        @remark
            主要用于加载类似gif文件,立方体文件复合型图片,这个复合概念并非纹理中的
            mipmap,而是有多张图片组成的一个纹理结构体
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0
        */
        void load(const ImageList & src);

        /** 从一个图象中加载数据转换成纹理,并控制mipmap映射
        @remark
            从一个图片对象中加载纹理,并且控制mipmap数量和具体内容,这个方法可以用于
            speedtree概念里的远/近树成像控制.
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0 高级api
        */
        void load(const Image & org, const ImageList & src);

        /** 加载图片文件到纹理
        @param[in] file 文件名
        @param[in] gid 群组
        @version NIIEngine 3.0.0
        */
        void load(const String & file, GroupID gid);

        /** 从已经解码的数据中加载转换成纹理
        @param[in] buffer 数据,此参数内存不自动释放
        @param[in] w 图片的宽度
        @param[in] h 图片的高度
        @param[in] pf 像素数据的格式
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0 高级api
        */
        void load(Nui8 * buffer, NCount w, NCount h, PixelFormat pf);

        /** 从已经解码的文件数据流中加载转换成纹理
        @param[in] stream 已经解码的文件数据流
        @param[in] w 图片的宽度
        @param[in] h 图片的高度
        @param[in] pf 像素数据的格式
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0
        */
        void load(DataStream * stream, NCount w, NCount h, PixelFormat pf);

        /** 设置纹理宽度
        @remark 期望宽度,需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline void setWidth(NCount w)              { mWidth = mSrcWidth = w; }

        /** 获取纹理宽度
        @remark 期望宽度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const              { return mWidth; }

        /** 返回原输入像素的宽度
        @remark 来源像素的宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getOriginWidth() const        { return mSrcWidth; }

        /** 设置纹理高度
        @remark 期望高度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline void setHeight(NCount h)             { mHeight = mSrcHeight = h; }

        /** 获取纹理高度
        @remark 期望高度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const             { return mHeight; }

        /** 返回原输入像素的宽度
        @remark 来源像素的宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getOriginHeight() const       { return mSrcHeight;  }

        /** 设置纹理深度
        @remark 期望深度，需在加载前设置
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        inline void setDepth(NCount d)              { mDepth = mSrcDepth = d; }

        /** 返回纹理深度
        @remark 期望深度，需在加载前设置
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const              { return mDepth;  }

        /** 返回原输入纹理的深度.
        @remark 来源像素的深度
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        inline NCount getOriginDepth() const        { return mSrcDepth;  }

        /** 返回纹理数组
        @version NIIEngine 3.0.0
        */
        inline NCount getArray() const              { return mArray; }
        
        /** 获取容积
        @version NIIEngine 5.0.0
        */
        inline NCount getVolume() const             { return mArray * mDepth; }
        
        /** 设置样本类型
        @version NIIEngine 5.0.0
        */
        void setSample( SampleType type);
        
        /** 获取样本类型
        @version NIIEngine 5.0.0
        */
        inline SampleType getSample() const         { return mSample;}
        
        /** 设置样本类型
        @version NIIEngine 5.0.0
        */
        void setOriginSample(SampleType srcType);
        
        /** 获取样本类型
        @version NIIEngine 5.0.0
        */
        inline SampleType getOriginSample() const   { return mSrcSample;}

        /** 为纹理面设置像素格式
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        void setFormat(PixelFormat pf);

        /** 获取纹理需要像素格式
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const        { return mFormat;  }

        /** 获取原输入纹理的像素格式
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getOriginFormat() const  { return mSrcFormat; }

        /** 设置使用的mipmaps数量
        @remark 需在加载前设置, mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        inline void setMipmapCount(NCount num)      { mMipmapCount = num; }

        /** 获取使用的mipmaps数量
        @remark 需在加载前设置, mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        inline NCount getMipmapCount() const        { return mMipmapCount;  }

        /** 获取原输入纹理的映射层数量
        @remark mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        inline NCount getSrcMipmapCount() const     { return mSrcMipmapCount;  }
        
        /** 设置纹理类型
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        void setType(Type type);

        /** 获取纹理类型
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline Type getType() const                 { return mTextureType;  }
        
        /** 获取纹理实际类型
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline Type getActualType() const           { return isPoolType() ? Texture::T_2DArray : mTextureType;}

        /** 返回这个纹理的来源,
        @remark 正方映射纹理为6
        @version NIIEngine 3.0.0
        */
        void setUnitResource(const StringList & reslist);

        /** 返回这个纹理的面数量,
        @remark 正方映射纹理为6
        @version NIIEngine 3.0.0
        */
        NCount getUnitCount() const;

        /** 设置应用在纹理加载的伽玛调整系数
        @remark 加载和转换管道为浮点值的时候让硬件去做伽玛颜色矫正
        @version NIIEngine 3.0.0
        */
        void setHardwareSRGB(bool b);

        /** 设置应用在纹理加载的伽玛调整系数
        @version NIIEngine 3.0.0
        */
        bool isHardwareSRGB() const;

        /** 设置 MemMode 标识符
        @remark 需在加载前设置
        @param[in] m MemMode 和 Buffer::Mode 多个组合
        @note 将会覆盖所有原来的属性
        @version NIIEngine 3.0.0 顶级api
        */
        inline void setModeMark(Nmark m)            { mMark = m;  }

        /** 获取 MemMode 标识符
        @remark 需在加载前设置
        @version NIIEngine 3.0.0 顶级api
        I*/
        inline Nmark getModeMark() const            { return mMark; }

        /** 添加 MemMode 标识符
        @param[in] m 单个
        @version NIIEngine 3.0.0 高级api
        */
        inline void addModMark(Nmark m)             { mMark |= m; }

        /** 移去 MemMode 标识符
        @param[in] m 单个
        @version NIIEngine 3.0.0 高级api
        */
        inline void removeModMark(Nmark m)          { mMark &= ~m;  }

        /** 是否存在 MemMode 标识符
        @version NIIEngine 3.0.0
        */
        inline bool isModMark(Nmark m) const        { return mMark & m; }

        /** 设置伽玛调整因子
        @remark 需在加载前设置
        @note setHardwareGamma
        @version NIIEngine 3.0.0
        */
        inline void setGamma(NIIf g)                { mGamma = g;  }

        /** 返回伽玛调整系数
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline NIIf getGamma() const                { return mGamma; }

        /** 设置全屏抗锯齿,当纹理是AGP缓存对象时
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @param[in] fsaa 样本数量
        @version NIIEngine 3.0.0
        */
        inline void setFSAA(Nui32 fsaa)             { mFSAA = fsaa;  }

        /** 获取多重采样
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @version NIIEngine 3.0.0
        */
        inline Nui32 getFSAA() const                { return mFSAA;  }

        /** 设置多重采样扩展
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @version NIIEngine 3.0.0
        */
        inline void setFSAAExt(const String & ext)  { mFSAAExt = ext;  }

        /** 获取多重采样扩展
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @version NIIEngine 3.0.0
        */
        inline const String & getFSAAExt() const    { return mFSAAExt; }

        /** 返回硬件像素缓存
        @remark 这个函数很可能会访问 AGP 内存,很有可能会阻塞当前调用线程
        @param[in] mipmap 映射层
        @version NIIEngine 3.0.0
        */
        virtual FrameBuffer * getBuffer(Nidx mipmap = 0);

        /** 返回硬件像素缓存(多层纹理专用)
        @remark 这个函数很可能会访问 AGP 内存,很有可能会阻塞当前调用线程
        @param[in] face fn(0) = x, fn(1) = -x, fn(2) = y,
                        fn(3) = -y, fn(4) = z, fn(5) = -z
        @param[in] mipmap 映射层
        @version NIIEngine 3.0.0
        */
        virtual FrameBuffer * getCubeBuffer(CubeFace face = CF_RIGHT, Nidx mipmap = 0);
        
        /** 创建访问视图
        @version NIIEngine 3.0.0
        */
        TextureBuffer * createView(PixelFormat pf);
        
        /** 删除访问视图
        @version NIIEngine 3.0.0
        */
        void destroyView(PixelFormat pf);
        
        /** 删除所有访问视图
        @version NIIEngine 3.0.0
        */
        void destroyAllView();
        
        /** 获取实际存储缓存
        @note 仅在调用 createInternal 后才有效
        @version NIIEngine 3.0.0
        */
        GpuBuffer * getGpuBuffer() const            { return mMainGpuBuffer; }

        /** 获取纹理原数据类型
        @verison NIIEngine 3.0.0
        */
        CodecID getRawType() const;

        /** 创建纹理内部资源.
        @remark 一般是指预先创建GPU/AGP内存,实际内容以后添加或者添加后改变
        @version NIIEngine 3.0.0
        */
        void createInternal();

        /** 释放纹理内部资源
        @remark 操作和 createInternal 相对应
        @version NIIEngine 3.0.0
        */
        void freeInternal();
        
        /**
        */
        void _resetTextureManager(void);

        /**
        */
        IdString getName() const                    { return mName;}

        /**
        */
        virtual String getNameStr() const;

        /// Returns the real name (e.g. disk in file) of the resource.
        virtual String getRealResourceNameStr() const;

        /**
        @version NIIEngine 
        */
        virtual String getResourceGroupStr() const;

        /**
        */
        String getSettingsDesc() const;

        /** Schedules an async transition in residency. If transitioning from
            ST_Disk to Resident, it will read from file (ResourceGroup was set in createTexture)
            If transitioning from ST_Cpu to Resident, it will read from the pointer it has.
            Multiple transitions can be stack together.
        @remarks
            If you're not loading from file (i.e. you're creating it programatically),
            call _transitionTo & _setNextResidency directly.
            Once you've called scheduleTransitionTo at least once, calling _transitionTo
            is very dangerous, as there are race conditions.

            @see    TextureGpu::scheduleTransitionTo
        @param next
            The residency to change to.
        @param image
            Pointer to image if you want to load the texture from memory instead of loading
            it from file or a listener.
            Pointer must be null if this is a manual texture.
            Pointer must NOT be a stack variable nor be deleted immediately.
            The actual loading is postponed until the request reaches the worker thread.
            That means the image pointer is safe to delete once you receive the
            TextureGpuListener::Reason::ReadyForRendering message.
        @param autoDeleteImage
            Whether we should call "delete image" once we're done using the image.
            Otherwise you must listen for TextureGpuListener::ReadyForRendering
            message to know when we're done using the image.
        */
        void unsafeScheduleTransitionTo(StorageType next, Image2 * image = 0, bool autoDeleteImage = true);

        /// Same as unsafeScheduleTransitionTo, but first checks if we're already
        /// in the residency state we want to go to, or if it has already
        /// been scheduled; thus it can be called multiple times
        void scheduleTransitionTo(StorageType next, Image2 * image = 0, bool autoDeleteImage = true);

        void setResolution(NCount width, NCount height, NCount depth = 1u, NCount array = 1);

        uint32 getArrayOffset() const               { return mArrayOffset;}
        
        bool isMultisample() const                  { return mSrcSample.isMultisample(); }

        void copyModel(TextureGpu * src);

        bool isModelEqual(const TextureGpu & o) const;

        virtual bool isSupport( CoordType pattern );

        /** Get the MSAA subsample locations.
            mSrcSample.pattern must not be CoordType::CT_Unknow.
        @param locations
            Outputs an array with the locations for each subsample. Values are in range [-1; 1]
        */
        virtual void getMultiSampleCoord(Vector2List & locations ) = 0;

        /** This function may be called manually (if user is manually managing a texture)
            or automatically (e.g. loading from file, or automatic batching is enabled)
            Once you call this function, you're no longer in ST_Disk mode; and will
            transition to either ST_Cpu or Resident depending on whether auto
            batching is enabled.
        @remarks
            Do NOT call this function yourself if you've created this function with
            AutomaticBatching as Ogre will call this, from a worker thread!

            Make sure you're done using mSysRamCopy before calling this function,
            as we may free that pointer (unless autoDeleteSysRamCopyOnResident = false).

            If you're calling _transitionTo yourself (i.e. you're not using scheduleTransitionTo)
            then you'll need to call _setNextResidency too, so that both getResidency
            and getNextResidency agree.
        @param data
            System RAM copy that backs this GPU data. May be null.
            Must've been allocated with OGRE_MALLOC_SIMD( size, MEMCATEGORY_RESOURCE );
            We will deallocate it.
            MUST respect _getSysRamCopyBytesPerRow & _getSysRamCopyBytesPerImage.
            If in doubt, use PixelFormatGpuUtils::getSizeBytes with rowAlignment = 4u;

            This param must be nullptr or equal to get_getSysRamCopy when going from
            Resident to ST_Cpu and strategy is not CPUGPU; as we
            will async download the content from the GPU.
        @param autoDeleteSysRamCopy
            When true, we free mSysRamCopy as we should.
            When false, caller is responsible for deleting this pointer else it will leak!
        */
        void _transitionTo( StorageType type, Nui8 * data, bool autoDeleteSysRamCopy = true );

        /// Notifies it is safe to use the real data. Everything has been uploaded.
        virtual void notifyDataIsReady(void) = 0;

        /// Forces downloading data from GPU to CPU, usually because the data on GPU changed
        /// and we're in strategy CPUGPU. May stall.
        void _syncGpuResidentToSystemRam(void);
        
        /// Do not call directly. Will change mStorage from StorageType::ST_Gpu to
        /// StorageType::ST_Cpu
        void _notifySysRamDownloadIsReady( Nui8 * data, bool resyncOnly );

        /**
        @param dst
        @param dstBox
        @param dstMipLevel
        @param srcBox
        @param srcMipLevel
        @param keepResolvedTexSynced
            When true, if dst is an MSAA texture and is implicitly resolved
            (i.e. dst->isAutoMSAA() == false); the resolved texture
            is also kept up to date.

            Typically the reason to set this to false is if you plane on rendering more
            stuff to dst texture and then resolve.
        */
        virtual void copyTo( TextureGpu * dst, const PixelBlock & dstBox, uint8 dstMipLevel,
            const PixelBlock & srcBox, uint8 srcMipLevel, bool keepResolvedTexSynced = true );

        /** These 3 values  are used as defaults for the compositor to use, but they may be
            explicitly overriden by a FrameBufferObject.
            Particularly required when passing the textures between nodes as input and
            output (since only the TextureGpu pointer is passed, and thus this information is lost)
        @remarks
            Changing these settings won't take immediate effect because they're only used
            when creating the compositor.
        @param depthBufferPoolId
            Sets the pool ID this RenderTarget should query from. Default value is POOL_DEFAULT.
            Set to POOL_NO_DEPTH to avoid using a DepthBuffer (or manually controlling it)
        @param preferDepthTexture
            Whether this RT should be attached to a depth texture, or a regular depth buffer.
            On older GPUs, preferring depth textures may result in certain depth precisions
            to not be available (or use integer precision instead of floating point, etc).
            True to use depth textures. False otherwise (default).
        @param desiredDepthBufferFormat
            Ogre will try to honour this request, but if it's not supported,
            you may get lower precision.
            All formats will try to fallback to PF_D24_UNORM_S8_UINT if not supported.
            Must be one of the following:
                PFG_D24_UNORM_S8_UINT
                PFG_D16_UNORM
                PFG_D32_FLOAT
                PFG_D32_FLOAT_X24_S8_UINT
        */
        virtual void _setDepthBufferDefaults(uint16 depthBufferPoolId, bool preferDepthTexture, PixelFormat desiredDepthBufferFormat);

        virtual uint16 getDepthBufferPoolId() const;

        virtual bool getPreferDepthTexture() const;

        virtual PixelFormat getDesiredDepthBufferFormat() const;

        /** Immediately resolves this texture to the resolve argument.
            Source must be MSAA texture, destination must be non-MSAA.
        @remarks
            This function may be slow on some APIs and should only be used when required,
            for example, to capture the screen from an explicit MSAA target and save it
            to disk only on user demand.
            If you need to call this often (like once per frame or more), then consider setting
            a Compositor with CompositorNode::mLocalRtvs::resolveTextureName set so that the
            compositor automatically resolves the texture every frame as efficiently as
            possible.
        */
        void _resolveTo(TextureGpu * resolve);

        /// Tells the API to let the HW autogenerate mipmaps. Assumes the
        /// isAutoMipmaps() == true and isRenderToTexture() == true
        virtual void _autogenerateMipmaps(void) = 0;
        
        /// Only valid for TextureGpu classes.
        /// TODO: This may be moved to a different class.
        virtual void swapBuffers(void) {}

        bool isPoolType(void) const                                 { return (mTextureFlags & AutomaticBatching) != 0;}
        bool isTexture(void) const                                  { return (mTextureFlags & NotTexture) == 0;}
        bool isRenderToTexture(void) const                          { return (mTextureFlags & RenderToTexture) != 0; }
        bool isUav(void) const                                      { return (mTextureFlags & Uav) != 0;}
        bool isAutoMipmaps(void) const                              { return (mTextureFlags & AllowAutomipmaps) != 0;}
        bool isAutoGenMipmap(void) const                            { return (mTextureFlags & AutomipmapsAuto) != 0;}
        bool isAutoMSAA(void) const                                 { return (mTextureFlags & MsaaExplicitResolve) != 0;}
        bool isReinterpretable(void) const                          { return (mTextureFlags & Reinterpretable) != 0; }
        bool prefersLoadingFromFileAsSRGB(void) const               { return (mTextureFlags & PrefersLoadingFromFileAsSRGB) != 0;}
        bool isRenderWindowSpecific(void) const                     { return (mTextureFlags & RenderWindowSpecific) != 0;}
        bool requiresTextureFlipping(void) const                    { return (mTextureFlags & RequiresTextureFlipping) != 0;}
        bool _isManualTextureFlagPresent(void) const                { return (mTextureFlags & ManualTexture) != 0;}
        bool isManualTexture(void) const                            { return (mTextureFlags & (NotTexture | Uav | RenderToTexture | ManualTexture)) != 0;}
        bool isPoolOwner(void) const                                { return (mTextureFlags & PoolOwner) != 0;}

        /// OpenGL RenderWindows are a bit specific:
        ///     * Their origins are upside down. Which means we need to flip Y.
        ///     * They can access resolved contents of MSAA even if isAutoMSAA = true
        virtual bool isOpenGLRenderWindow(void) const;

        virtual void _setToDisplayDummyTexture(void) = 0;

        virtual void _notifyTextureSlotChanged( const TexturePool * newPool, uint16 slice );

        /** 2D Texture with automatic batching will be merged with other textures into the
            same pool as one big 2D Array texture behind the scenes.

            For two textures to be placed in the same pool (assuming it's not full)
            the following must match:
                Width, Height, PixelFormat, number of mipmaps, poolID

            Pool ID is an arbitrary value with no actual meaning. This is ID
            allows you to prevent certain textures from being group together.
            For example, you may want all textures from Level 0 to be grouped
            together while Level 1 gets grouped together in a different pool

            @see	Texture::AutomaticBatching
            @see	TextureGpuManager::reservePoolId
        @remarks
            This value cannot be changed while the texture is resident (i.e. because
            it has already been assigned to a pool)
        @param poolId
            Arbitrary value. Default value is 0.
        */
        void setTexturePoolId(uint32 poolId)                        { N_assert( mStorage != StorageType::ST_Gpu ); mPoolId = poolId;}
        
        inline uint32 getTexturePoolId() const		                { return mPoolId; }
        
        inline const TexturePool* getTexturePool() const            { return mTexturePool; }

        void addListener( TextureGpuListener * listener );

        void removeListener( TextureGpuListener * listener );

        void notifyAllListenersTextureChanged(uint32 reason, void * extraData = 0);

        const vector<TextureGpuListener*>::type & getListeners(void) const;

        virtual bool supportsAsDepthBufferFor(TextureGpu * colourTarget) const;

        /// Writes the current contents of the render target to the named file.
        void writeContentsToFile(const String & filename, uint8 minMip, uint8 maxMip, bool automaticResolve = true);

        /// Writes the current contents of the render target to the memory.
        void copyContentsToMemory(PixelBlock src, PixelBlock dst, PixelFormat dstFormat, bool automaticResolve = true);

        static const IdString msFinalTextureBuffer;

        static const IdString msMsaaTextureBuffer;

        virtual void getCustomAttribute( IdString name, void *pData ) {}

        TextureGpuManager * getTextureManager() const                { return mTextureManager;}

        PixelBlock getEmptyBox( uint8 mipLevel );

        PixelBlock _getSysRamCopyAsBox( uint8 mipLevel );

        uint8* _getSysRamCopy( uint8 mipLevel );

        /// Note: Returns non-zero even if there is no system ram copy.
        size_t _getSysRamCopyBytesPerRow( uint8 mipLevel );

        /// Note: Returns non-zero even if there is no system ram copy.
        size_t _getSysRamCopyBytesPerImage( uint8 mipLevel );

        /// Returns total size in bytes used in GPU by this texture (not by its pool)
        /// including mipmaps.
        size_t getSizeBytes(void) const;

        /** It is threadsafe to call this function from main thread.
            If this returns false, then the following functions are not threadsafe:
            Setters must not be called, and getters may change from a worker thread:
                * setResolution
                * getWidth, getHeight, getDepth, getVolume, getArray
                * set/getFormat
                * set/getNumMipmaps
                * set/getTextureType
                * getTexturePool
            Note that this function may return true but the worker thread
            may still be uploading to this texture. Use isDataReady to
            see if the worker thread is fully done with this texture.

        @remarks
            Function for querying/waiting for data and metadata to be ready
            are for blocking the main thread when a worker thread is loading
            the texture from file or a listener (i.e. isManualTexture returns false)
            otherwise you don't need to call these functions.
        */
        bool isMetadataReady(void) const                    { return ( (mStorage == StorageType::ST_Gpu &&
                                                                mNextStorage == StorageType::ST_Gpu) ||
                                                                (mStorage == StorageType::ST_Cpu &&
                                                                mNextStorage != StorageType::ST_Disk) ) &&
                                                                mPendingResidencyChanges == 0;}

        /// For internal use. Do not call directly.
        ///
        /// This function is the same isDataReady except it ignores pending residency changes,
        /// which is important when TextureGpuManager needs to know this information but the
        /// TextureGpu is transitioning (thus mPendingResidencyChanges is in an inconsistent state)
        virtual bool _isDataReadyImpl(void) const = 0;

        /// True if this texture is fully ready to be used for displaying.
        ///
        /// IMPORTANT: Always returns true if getResidency != StorageType::ST_Gpu
        /// and there are no pending residency transitions.
        ///
        /// Returns false while there are pending residency status
        ///
        /// If this is true, then isMetadataReady is also true.
        /// See isMetadataReady.
        bool isDataReady(void)                              {return _isDataReadyImpl() && mPendingResidencyChanges == 0u;}

        /// Blocks main thread until metadata is ready. Afterwards isMetadataReady
        /// should return true. If it doesn't, then there was a problem loading
        /// the texture.
        /// See isMetadataReady remarks.
        void waitForMetadata(void);

        /// Blocks main thread until data is ready. Afterwards isDataReady
        /// should return true. If it doesn't, then there was a problem loading
        /// the texture.
        /// See isMetadataReady remarks.
        void waitForData(void);
    protected:
        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Resource::calcSize
        NCount calcSize() const;

        /** 创建内部纹理资源的实现
        @version NIIEngine 3.0.0
        */
        virtual void createInternalImpl() = 0;

        /** 释放内部纹理资源的实现
        @version NIIEngine 3.0.0
        */
        virtual void freeInternalImpl() = 0;
        
        /** 创建访问视图的实现
        @version NIIEngine 3.0.0
        */
        virtual TextureBuffer * createViewImpl(PixelFormat pf) = 0;

        /// @copydoc Resource::planImpl
        void planImpl();

        /// @copydoc Resource::unplanImpl
        void unplanImpl();

        /// @copydoc Resource::loadImpl
        void loadImpl();
        
        /// Stalls until GPU -> CPU transfers (i.e. _syncGpuResidentToSystemRam) are done
        /// waitForData implicitly does this. This function exists because there are times
        /// where Ogre needs to know this info, and calling waitForData would never return
        /// true because the texture is in an inconsistent state.
        void waitForPendingSyncs();

        virtual void destroyInternalResourcesImpl(void) = 0;

        void checkValidSettings(void);

        void transitionToResident(void);
    protected:
        IdString mName;
        ImageList mLoadedImages;
        FrameBufferList	mSurfaceList;
        StringList mOriginList;
        TextureBufferList mViewList;
        GpuBuffer * mMainGpuBuffer;
        NCount mWidth;                  ///< 纹理的宽(实际显示)
        NCount mHeight;                 ///< 纹理的高(实际显示)
        NCount mDepth;                  ///< 纹理的深(实际显示)
        NCount mArray;                  ///< 纹理的数组(实际显示)
        NCount mSrcWidth;               ///< 原纹理(图片/像素)的宽
        NCount mSrcHeight;              ///< 原纹理(图片/像素)的高
        NCount mSrcDepth;               ///< 原纹理(图片/像素)的深
        PixelFormat mFormat;            ///< 象素格式(实际显示)
        PixelFormat mSrcFormat;         ///< 原象素(图片/像素)格式
        SampleType mSample;             ///< 样本类型
        SampleType mSrcSample;          ///< 原样本类型
        NCount mMipmapCount;            ///< 纹理要求的Mipmaps
        NCount mSrcMipmapCount;         ///< 纹理的Mipmaps
        StorageType mStorage;           ///< 当前存储类型
        StorageType mNextStorage;       ///< 需要的存储类型
        RecoverType mRecoverType;       ///< 回收类型
        Type mTextureType;              ///< 纹理类型
        NIIf mGamma;                    ///< 加码值
        Nui32 mFSAA;                    ///< 抗锯齿
        String mFSAAExt;                ///< 抗锯齿平台扩展
        Nmark mMark;                    ///< 掩码
        bool mInnerValid;
        
        Nui32 mPendingResidencyChanges;

        Nui32 mRank;
        Nui32 mLastFrameUsed;

        float mLowestDistanceToCamera;
        Nui32 mTextureFlags;

        uint8 * mSysRamCopy;

        VaoManager * mMag;
        TextureGpuManager * mTextureManager;

        TexturePool const * mTexturePool;
        Nui32 mPoolId;
        Nui32 mArrayOffset;

        vector<TextureGpuListener*>::type mListeners;
    };
}
#endif