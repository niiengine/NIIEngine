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
    class _EngineAPI Texture : public Resource, public RenderSysData
    {
    public:
        friend class TextureManager;
    public:
        /** 枚举确定纹理用法
        @version NIIEngine 3.0.0
        */
        enum MemMode
        {
            MM_AGP_COLOUR       = Buffer::M_EXT1,   ///< 颜色缓存(Frame)
            MM_AGP_ALPHA        = Buffer::M_EXT2,   ///< A缓存(Frame)
            MM_AGP_DEPTH        = Buffer::M_EXT3,   ///< 深度缓存(Frame)
            MM_AGP_STENCIL      = Buffer::M_EXT4,   ///< 蒙板缓存(Frame)
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
        };
        /** 枚举确定纹理类型
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_Buffer        = 1,    ///< 
            T_ShaderBuffer  = 2,    ///< 
            T_1D            = 3,    ///< 1D纹理
            T_1DArray       = 4,    ///< 1D纹理数组
            T_2D            = 5,    ///< 2D纹理
            T_2DArray       = 6,    ///< 2D纹理数组
            T_3D            = 7,    ///< 3D纹理
            T_CUBE          = 8,    ///< 3D方体映射使用3D纹理坐标
            T_CUBEArray     = 9,    ///< 3D纹理数组
            T_CUBE_6        = 10    ///< 6面元的方体映射
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
        inline void setWidth(NCount w)          { mWidth = mSrcWidth = w; }

        /** 获取纹理宽度
        @remark 期望宽度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const          { return mWidth; }

        /** 返回原输入像素的宽度
        @remark 来源像素的宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getOriginWidth() const    { return mSrcWidth; }

        /** 设置纹理高度
        @remark 期望高度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline void setHeight(NCount h)         { mHeight = mSrcHeight = h; }

        /** 获取纹理高度
        @remark 期望高度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const         { return mHeight; }

        /** 返回原输入像素的宽度
        @remark 来源像素的宽度
        @version NIIEngine 3.0.0
        */
        inline NCount getOriginHeight() const   { return mSrcHeight;  }

        /** 设置纹理深度
        @remark 期望深度，需在加载前设置
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        inline void setDepth(NCount d)          { mDepth = mSrcDepth = d; }

        /** 返回纹理深度
        @remark 期望深度，需在加载前设置
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const          { return mDepth;  }

        /** 返回原输入纹理的深度.
        @remark 来源像素的深度
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        inline NCount getOriginDepth() const    { return mSrcDepth;  }

        /** 为纹理面设置像素格式
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline void setFormat(PixelFormat pf)   { mFormat = pf;  }

        /** 获取纹理需要像素格式
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const    { return mFormat;  }

        /** 获取原输入纹理的像素格式
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getSrcFormat() const { return mSrcFormat; }

        /** 设置使用的mipmaps数量
        @remark 需在加载前设置, mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        inline void setMipmapCount(NCount num)  { mMipmapCount = num; }

        /** 获取使用的mipmaps数量
        @remark 需在加载前设置, mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        inline NCount getMipmapCount() const    { return mMipmapCount;  }

        /** 获取原输入纹理的映射层数量
        @remark mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        inline NCount getSrcMipmapCount() const { return mSrcMipmapCount;  }
        
        /** 设置纹理类型
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline void setType(Type type)          { mTextureType = type;  }

        /** 获取纹理类型
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        inline Type getType() const             { return mTextureType;  }

        /** 返回这个纹理的来源,
        @remark 正方映射纹理为6
        @version NIIEngine 3.0.0
        */
        void setUnitRsource(const StringList & reslist);

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
        GpuBuffer * createView(PixelFormat pf, Type type);
        
        /** 删除访问视图
        @version NIIEngine 3.0.0
        */
        void destroyView(PixelFormat pf, Type type);
        
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
        virtual GpuBuffer * createViewImpl(PixelFormat pf, Type type) = 0;

        /// @copydoc Resource::planImpl
        void planImpl();

        /// @copydoc Resource::unplanImpl
        void unplanImpl();

        /// @copydoc Resource::loadImpl
        void loadImpl();
    protected:
        ImageList mLoadedImages;
        FrameBufferList	mSurfaceList;
        StringList mOriginList;
        GpuBufferList mViewList;
        GpuBuffer * mMainGpuBuffer;
        NCount mWidth;                  ///< 纹理的宽(实际显示)
        NCount mHeight;                 ///< 纹理的高(实际显示)
        NCount mDepth;                  ///< 纹理的深(实际显示)
        NCount mSrcWidth;               ///< 原纹理(图片/像素)的宽
        NCount mSrcHeight;              ///< 原纹理(图片/像素)的高
        NCount mSrcDepth;               ///< 原纹理(图片/像素)的深
        PixelFormat mFormat;            ///< 象素格式(实际显示)
        PixelFormat mSrcFormat;         ///< 原象素(图片/像素)格式
        NCount mMipmapCount;            ///< 纹理要求的Mipmaps
        NCount mSrcMipmapCount;         ///< 纹理的Mipmaps
        Type mTextureType;              ///< 纹理类型
        NIIf mGamma;                    ///< 加码值
        Nui32 mFSAA;                    ///< 抗锯齿
        String mFSAAExt;                ///< 抗锯齿平台扩展
        Nmark mMark;                    ///< 掩码
        bool mInnerValid;
    };
}
#endif