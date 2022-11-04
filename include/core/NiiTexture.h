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
        SampleType(Nui8 msaa = 1u, CoordType type = CT_Unknow) :
            mMultiSampling(msaa),
            mCoverageSampling(0),
            mType(type){}
        
        explicit SampleType(const String & hint)
        {
            parseString( hint );
        }
        
        bool operator !=(const SampleType & o) const
        {
            return mMultiSampling != o.mMultiSampling || mCoverageSampling != o.mCoverageSampling || 
                   mType != o.mType;
        }

        bool operator ==(const SampleType & o) const
        {
            return mMultiSampling == o.mMultiSampling && mCoverageSampling == o.mCoverageSampling &&
                   mType == o.mType;
        }
        
        inline bool isMultiSample() const              { return mMultiSampling > 1u; }

        inline Nui8 getMultiSampling() const           { return mMultiSampling; }
        
        inline Nui8 getCoverageSampling() const        { return mCoverageSampling; }
        
        inline Nui8 getMaxSampling() const             { return std::max(mCoverageSampling, mMultiSampling); }
        
        inline CoordType getType() const               { return mType; }

        void set(Nui8 msaa, Nui8 csaa, CoordType type);

        void setMsaa(Nui8 msaa, CoordType type = CT_Unknow);

        bool isMsaa() const;

        /**
            8x CSAA, setCsaa(8, false)
            8x CSAA(Quality),setCsaa(8, true)
            16x CSAA, setCsaa(16, false)
            16x CSAA(Quality),setCsaa(16, true)
        @param csaa 8 or 16
        */
        void setCsaa(Nui8 csaa, bool quality);

        /**
            2f4x EQAA call setEqaa( 2u, 4u )
            4f8x EQAA call setEqaa( 4u, 8u )
            8f16x EQAA call setEqaa( 8u, 16u )
        */
        void setEqaa(Nui8 msaa, Nui8 csaa);

        void parseString(const String & in);

        void getString(String & out) const;
    protected:
        Nui8 mMultiSampling;
        Nui8 mCoverageSampling;
        CoordType mType;
    };

    /** 纹理
    @remark 蒙皮使用的像素
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Texture : public Resource, public RenderSysData, public Buffer
    {
    public:
        friend class TextureManager;
        friend class RenderSys;
    public:
        class _EngineAPI Process
        {
        public:
            virtual ~Process();

            enum Operation
            {
                O_Unknown,
                O_DiskToCpu,
                O_CpuToGpu,
                O_CpuToDisk,//write to disk
                O_GpuToDisk,//write to disk
                O_InGpu,
                O_InCpu,
                O_Valid,
                O_OutCpu,
                O_OutGpu,
                O_GpuSyncCpu,
                O_Delete,
                O_FsaaAlter,
                O_PoolAlter,
                O_MetadataDirty,
                O_Error
            };

            struct Item
            {
                Item(Texture * tex, Operation op) : mTexture(tex), mOp(op) {}
                Texture * mTexture;
                Operation mOp;
                NIIi mErrorCode;
                bool mImmediate;
            };                

            typedef list<Item>::type ItemList;
            virtual void process(const Item & item) = 0;
        };
        vector<Process *>::type ProcessList;
    
        /** 存储类型
        @version NIIEngine 3.0.0
        */
        enum StorageType
        {
            ST_Disk,
            ST_Cpu,
            ST_Gpu,
            ST_Reload,
            ST_Destroy
        };

        /** 回收类型
        @version NIIEngine 5.0.0
        */
        enum RecoverType
        {
            RT_Disk,
            RT_Cpu,
            RT_CpuGpu
        };
        
        enum FeatureType
        {
            FT_Diffuse,
            FT_Monochrome,
            FT_Gray,
            FT_NormalMap,
            FT_EnvMap,
            FT_HeightMap
        };
        
        enum MipmapGenType
        {
            MGT_None,
            MGT_HwMode,
            MGT_SwMode
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
            MM_SRGB             = Buffer::M_EXT5,   ///< 硬件色域校正
            MM_SRGB_Write       = Buffer::M_EXT6,   ///< 硬件色域校正(写入)
            MM_SRGB_Read        = Buffer::M_EXT7,   ///< 硬件色域校正(读取)
            MM_COLOUR_RGB       = Buffer::M_EXT8,   ///< 包含RGB颜色(默认)
            MM_COLOUR_ALPHA     = Buffer::M_EXT9,   ///< 包含A成分(默认)
            MM_MIPMAP_DEFAULT   = Buffer::M_EXT10,  ///< N_TEX_MIPMAP
            MM_MIPMAP_AUTO      = Buffer::M_EXT11,  ///< 自动生成mipmaps
            MM_MIPMAP_HW        = Buffer::M_EXT12,  ///< 硬件生成mipmaps
            MM_MIPMAP_SW        = Buffer::M_EXT13,  ///< 硬件生成mipmaps
            MM_DEPTH_BIT_16     = Buffer::M_EXT14,  ///< MM_DEPTH_BIT_X MM_DEPTH_FLOAT 只能其一
            MM_DEPTH_BIT_32     = Buffer::M_EXT15,  ///< MM_DEPTH_BIT_X MM_DEPTH_FLOAT 只能其一
            MM_DEPTH_FLOAT      = Buffer::M_EXT16,  ///< MM_DEPTH_BIT_X MM_DEPTH_FLOAT 只能其一
            MM_FRAME            = Buffer::M_EXT17,
            MM_Window           = Buffer::M_EXT18,
            MM_BufferStroge     = Buffer::M_EXT19,
            MM_MIPMAP_CHECK     = Buffer::M_EXT20,
            MM_ManualMSAA       = Buffer::M_EXT21,
            MM_AlterView        = Buffer::M_EXT22,
            MM_PreferSRGB       = Buffer::M_EXT23,
            MM_VerticalFlip     = Buffer::M_EXT24,
            MM_Manual           = Buffer::M_EXT25,
            MM_POOLSLICE        = Buffer::M_EXT26,
            MM_POOL             = Buffer::M_EXT27,
            
            MM_Normals          = Buffer::M_EXT31,
            MM_Red              = Buffer::M_EXT32,

            MM_MIPMAP           = MM_MIPMAP_SW | MM_MIPMAP_HW
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
            T_3DArray       = 51,   ///< 3D纹理数组(基础不支持,stroagebuf衍生)
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
        
        struct Task
        {
            Task(StorageType type, Image2 * image = 0, bool autoImage = true, bool check = false, bool immediate = false) :
                mType(type), mImage(image), mAutoImage(autoImage), mCheck(check), mImmediate(immediate){}
            
            StorageType mType;
            Image2 * mImage;
            Nui8 * mData = 0;
            bool mAutoImage = true;
            bool mCheck = false;
            bool mImmediate = false;
        }
    public:
        Texture(ResourceID rid, Type type, Nmark usage, RecoverType rtype);
                    
        virtual ~Texture();
        
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
        
        /** 复制数据结构模型
        @version NIIEngine 5.0.0
        */
        void copyModel(Texture * src);

        /** 复制数据结构模型是否相同
        @version NIIEngine 5.0.0
        */
        bool isModelEqual(const Texture & o) const;
        
        /** 等待纹理完成加载
        @version NIIEngine 6.0.0
        */
        void wait(bool metadata);
        
        /** 执行任务
        @version NIIEngine 5.0.0
        */
        void schedule(const Task & task);
        
        /** 设置期望的存储方式
        @version NIIEngine 3.0.0
        */
        void setStorage(StorageType type)           { mNextStorage = type; }
        
        /** 获取期望的存储方式
        @version NIIEngine 3.0.0
        */
        StorageType getStorage() const              { return mNextStorage; }
        
        /** 获取当前存储方式
        @version NIIEngine 3.0.0
        */
        StorageType getCurrentStorage() const       { return mCurrentStorage; }
        
        /** 获取回收类型
        @version NIIEngine 5.0.0
        */
        RecoverType getRecoverType() const          { return mRecoverType;}
        
        /** 设置自动控制数据
        @version NIIEngine 3.0.0
        */
        void setAutoData(bool d)                    { mAutoData = d; }
        
        /** 是否自动控制数据
        @version NIIEngine 3.0.0
        */
        bool isAutoData() const                     { return mAutoData; }

        /** 获取任务数量
        @version NIIEngine 3.0.0
        */
        Nui32 getTaskCount() const                 { return mTaskCount; }

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
        
        /** 加载数据
        @version NIIEngine 5.0.0
        */
        void load(StorageType type, Nui8 * data, bool immediate = false);
        
        /** 生成mipmap
        @version NIIEngine 5.0.0
        */
        virtual void mipmap() = 0;
        
        /** 生成抗锯齿结果
        @version NIIEngine 6.0.0
        */
        void msaa(Texture * dst);
        
        /** 设置纹理宽度/高度/深度/数组
        @remark 期望宽度,需在加载前设置
        @version NIIEngine 3.0.0
        */
        void setResolution(NCount width, NCount height, NCount depth = 1, NCount array = 1);

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
        
        /** 返回这个纹理的面数量,
        @remark 正方映射纹理为6
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitCount() const          { return mArray;}
        
        /** 获取容积
        @version NIIEngine 5.0.0
        */
        inline NCount getVolume() const             { return mArray * mDepth; }
        
        /** 设置样本类型
        @version NIIEngine 5.0.0
        */
        void setSample(SampleType type);
        
        /** 获取样本类型
        @version NIIEngine 5.0.0
        */
        inline SampleType getSample() const         { return mSample;}
        
        /** 设置样本类型
        @version NIIEngine 5.0.0
        */
        void setOriginSample(SampleType type);
        
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
        inline Type getActualType() const           { return mMark & MM_POOLSLICE ? Texture::T_2DArray : mTextureType;}

        /** 返回这个纹理的来源,
        @remark 正方映射纹理为6
        @version NIIEngine 3.0.0
        */
        void setUnitResource(const StringList & reslist);

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
        TextureBuffer * createWindow(PixelFormat pf);
        
        /** 删除访问视图
        @version NIIEngine 3.0.0
        */
        void destroyView(PixelFormat pf);
        
        /** 删除所有访问视图
        @version NIIEngine 3.0.0
        */
        void destroyAllView();
        
        /** 获取实际存储缓存
        @version NIIEngine 3.0.0
        */
        GpuBuffer * getGpuBuffer() const            { return mMainGpuBuffer; }

        /** 获取纹理原数据类型
        @verison NIIEngine 3.0.0
        */
        CodecID getRawType() const;
        
        /** 获取数据是否有效
        @version NIIEngine 6.0.0
        */
        bool isValid() const;
        
        /** 获取Cpu数据是否有效
        @version NIIEngine 6.0.0
        */
        bool isMetadataValid() const;

        /** 获取池中的位置
        @version NIIEngine 5.0.0
        */
        uint32 getPoolArray() const                 { return mPoolArray;}
        
        /** 是否多样本
        @version NIIEngine 5.0.0
        */
        bool isMultiSample() const                  { return mSrcSample.isMultiSample(); }

        /** 是否支持采样模式
        @version NIIEngine 5.0.0
        */
        virtual bool isSupport(CoordType pattern);

        /** 获取样本采样位置
        @param location [-1; 1] for earch
        */
        virtual void getMultiSampleCoord(Vector2List & location) = 0;

        bool isPool() const                         { return (mMark & MM_POOL) == MM_POOL;}
        bool isPoolSlice() const                    { return (mMark & MM_POOLSLICE) == MM_POOLSLICE;}
        bool isTexture() const                      { return (mMark & (MM_FRAME | MM_Window)) == 0;}
        bool isFrame() const                        { return (mMark & MM_FRAME) == MM_FRAME; }
        bool isBufferStroge() const                 { return (mMark & MM_BufferStroge) != MM_BufferStroge;}
        bool isAutoMipmap() const                   { return (mMark & MM_MIPMAP_AUTO) == MM_MIPMAP_AUTO;}
        bool isAutoGenMipmap() const                { return (mMark & MM_MIPMAP_CHECK) == MM_MIPMAP_CHECK;}
        bool isManualMSAA() const                   { return (mMark & MM_ManualMSAA) == MM_ManualMSAA;}
        bool isAlterView() const                    { return (mMark & MM_AlterView) == MM_AlterView; }
        bool isPreferSRGB() const                   { return (mMark & MM_PreferSRGB) == MM_PreferSRGB;}
        bool isWindow() const                       { return (mMark & MM_Window) == MM_Window;}
        bool isFlipping() const                     { return (mMark & MM_VertFlip) == MM_VertFlip;}
        bool isManual() const                       { return (mMark & (MM_BufferStroge | MM_FRAME | MM_Manual)) != 0;}
        
        /**
        @version NIIEngine 5.0.0
        */
        inline GroupID getPoolId() const		                    { return mPoolId; }
        
        /**
        @version NIIEngine 5.0.0
        */
        inline const TexturePool * getPool() const                  { return mPool; }

        /**
        @version NIIEngine 5.0.0
        */
        void addProcess(Process * prc);

        /**
        @version NIIEngine 5.0.0
        */
        void removeProcess(Process * prc);
        
        /**
        @version NIIEngine 5.0.0
        */
        const ProcessList & getListeners() const                    { return mListeners; }

        /**
        @version NIIEngine 5.0.0
        */
        void process(Process::Operation prc, bool immediate = false);

        /**
        @version NIIEngine 5.0.0
        */
        virtual bool isDepthStencilSupport(Texture * colour) const;

        /**
        @version NIIEngine 5.0.0
        */
        void read(PixelBlock & dst, const PixelBlock & src, bool msaa = true);
        
        /**
        @version NIIEngine 5.0.0
        */
        void read(Image * dst, NCount srcMinMip, NCount srcMaxMip, bool msaa = true);
        
        /**
        @version NIIEngine 5.0.0
        */
        virtual void memcpy(Texture * dst, const PixelBlock & dstBox, Nui8 dstMip, const PixelBlock & srcBox, Nui8 srcMip, bool msaa = true);

        /** 
        @version NIIEngine 5.0.0
        */
        void write(Image * src, NCount minMip, NCount maxMip, NCount depthidx = 0u, NCount depthcnt = -1, NCount arrayidx = 0, NCount arraycnt = -1);
        
        /**
        @version NIIEngine 5.0.0
        */
        PixelBlock getEmptyBox(NCount mipLevel);

        /**
        @version NIIEngine 3.0.0
        */
        PixelBlock getDataBlock(NCount mipLevel);

        /**
        @version NIIEngine 3.0.0
        */
        Nui8 * getData(NCount mipLevel, bool create = false);

        /**
        @version NIIEngine 3.0.0
        */
        NCount getRowSize(NCount mipLevel);

        /**
        @version NIIEngine 3.0.0
        */
        NCount getSliceSize(NCount mipLevel);
        
        static PixelFormat DepthFormat;
    protected:
        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydoc Resource::planImpl
        void planImpl();

        /// @copydoc Resource::unplanImpl
        void unplanImpl();

        /// @copydoc Resource::loadImpl
        void loadImpl();
        
        /// @copydoc Resource::calcSize
        NCount calcSize() const;
        
        /** 创建内部纹理资源的实现
        @version NIIEngine 3.0.0
        */
        virtual void createInternalImpl() = 0;

        /** 释放内部纹理资源的实现
        @version NIIEngine 3.0.0
        */
        virtual void freeInternalImpl() = 0;
        
        /** 纹理是否已经有效
        @version NIIEngine 5.0.0
        */
        virtual bool isValidImpl() const = 0;
        
        /** 池方式分配纹理资源的实现
        @version NIIEngine 3.0.0
        */
        virtual void poolImpl(const TexturePool * pool, Nui16 array);
        
        /** 创建访问视图的实现
        @version NIIEngine 3.0.0
        */
        virtual TextureBuffer * createViewImpl(PixelFormat pf) = 0;

        virtual void notifyGpuData(bool immediate) = 0;

        virtual void notifyCpuData(bool storage);

        virtual void downloadImpl();

        void checkValidSettings();
    protected:
        ImageList mLoadedImages;
        FrameBufferList	mSurfaceList;
        StringList mOriginList;
        TextureBufferList mViewList;
        GpuBuffer * mMainGpuBuffer;
        ProcessList mListeners;
        TexturePool * mPool;
        GroupID mPoolId;
        NCount mWidth;                  ///< 纹理的宽(实际显示)
        NCount mHeight;                 ///< 纹理的高(实际显示)
        NCount mDepth;                  ///< 纹理的深(实际显示)
        NCount mArray;                  ///< 纹理的数组(实际显示)
        NCount mSrcWidth;               ///< 原纹理(图片/像素)的宽
        NCount mSrcHeight;              ///< 原纹理(图片/像素)的高
        NCount mSrcDepth;               ///< 原纹理(图片/像素)的深
        PixelFormat mFormat;            ///< 象素格式(实际显示)
        PixelFormat mSrcFormat;         ///< 原象素(图片/像素)格式
        PixelFormat mDSFormat;
        SampleType mSample;             ///< 样本类型
        SampleType mSrcSample;          ///< 原样本类型
        NCount mMipmapCount;            ///< 纹理要求的Mipmaps
        NCount mSrcMipmapCount;         ///< 纹理的Mipmaps
        StorageType mCurrentStorage;    ///< 当前存储类型
        StorageType mNextStorage;       ///< 需要的存储类型
        RecoverType mRecoverType;       ///< 回收类型
        Type mTextureType;              ///< 纹理类型
        NIIf mGamma;                    ///< 加码值
        Nui16 mFSAA;                    ///< 抗锯齿
        Nui16 mPoolArray;
        Nui16 mDSPool;
        String mFSAAExt;                ///< 抗锯齿平台扩展
        Nmark mMark;                    ///< 掩码
        Nui32 mTaskCount;
        Nui32 mLastFrameUsed;

        Nui8 * mData;
        bool mDSTexture;
        bool mAutoData;
        bool mInnerValid;
    };
}
#endif