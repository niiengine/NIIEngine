/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-31

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
        friend class TextureManager;
    public:
        /** 枚举确定纹理用法
        @version NIIEngine 3.0.0
        */
        enum MemMode
        {
            MM_AGP_COLOUR   = Buffer::M_EXT1,       ///< 颜色缓存(Frame)
            MM_AGP_ALPHA    = Buffer::M_EXT2,       ///< A缓存(Frame)
            MM_AGP_DEPTH    = Buffer::M_EXT3,       ///< 深度缓存(Frame)
            MM_AGP_STENCIL  = Buffer::M_EXT4,       ///< 蒙板缓存(Frame)
            MM_FRAME_SRGB   = Buffer::M_EXT5,       ///< 硬件色域校正
            MM_FRAME_SRGB_Write = Buffer::M_EXT6,   ///< 硬件色域校正(写入)
            MM_FRAME_SRGB_Read  = Buffer::M_EXT7,   ///< 硬件色域校正(读取)
            MM_COLOUR_RGB       = Buffer::M_EXT8,   ///< 包含RGB颜色(默认)
            MM_COLOUR_ALPHA     = Buffer::M_EXT9,   ///< 包含A成分(默认)
            MM_MIPMAP_DEFAULT   = Buffer::M_EXT10,  ///< N_TEX_MIPMAP
            MM_MIPMAP_AUTO      = Buffer::M_EXT11,  ///< 自动生成mipmaps
            MM_MIPMAP_HW        = Buffer::M_EXT12,  ///< 硬件生成mipmaps
            MM_DEPTH_BIT_16     = Buffer::M_EXT13,  ///< MM_DEPTH_BIT_X 只能其一
            MM_DEPTH_BIT_32     = Buffer::M_EXT14,  ///< MM_DEPTH_BIT_X 只能其一
            MM_DEPTH_FLOAT      = Buffer::M_EXT13,  ///<
        };
        /** 枚举确定纹理类型
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_1D        = 1,    ///< 1D纹理
            T_2D        = 2,    ///< 2D纹理
            T_3D        = 3,    ///< 3D纹理
            T_2D_COM    = 4,    ///< 2D纹理数组
            T_CUBE      = 5,    ///< 3D方体映射使用3D纹理坐标
            T_CUBE_6    = 6     ///< 6面元的方体映射
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
        Texture(ResourceID rid, const String & file, GroupID gid, LangID lid = N_PrimaryLang);

        /** 机制构造函数
        @param[in] ls 加载策略
        @param[in] rs 结果策略
        @version NIIEngine 3.0.0
        */
        Texture(ResourceID rid, GroupID gid, ResourceLoadScheme * ls = 0, 
            ResourceResultScheme * rs= 0, LangID lid = N_PrimaryLang);

        virtual ~Texture();

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
        virtual void load(const Image * src);

        /** 从一个图象集合中加载转换成纹理
        @remark
            主要用于加载类似gif文件,立方体文件复合型图片,这个复合概念并非纹理中的
            mipmap,而是有多张图片组成的一个纹理结构体
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0
        */
        virtual void load(const Images & src);

        /** 从一个图象中加载数据转换成纹理,并控制mipmap映射
        @remark
            从一个图片对象中加载纹理,并且控制mipmap数量和具体内容,这个方法可以用于
            speedtree概念里的远/近树成像控制.
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0 高级api
        */
        virtual void load(const Image & org, const Images & src);

        /** 加载图片文件到纹理
        @param[in] file 文件名
        @param[in] gid 群组
        @version NIIEngine 3.0.0
        */
        virtual void load(const String & file, GroupID gid);

        /** 从已经解码的数据中加载转换成纹理
        @param[in] buffer 数据,此参数内存不自动释放
        @param[in] w 图片的宽度
        @param[in] h 图片的高度
        @param[in] pf 像素数据的格式
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0 高级api
        */
        virtual void load(Nui8 * buffer, NCount w, NCount h, PixelFormat pf);

        /** 从已经解码的文件数据流中加载转换成纹理
        @param[in] stream 已经解码的文件数据流
        @param[in] w 图片的宽度
        @param[in] h 图片的高度
        @param[in] pf 像素数据的格式
        @note 此方法不属于加载策略器管理范畴
        @version NIIEngine 3.0.0
        */
        virtual void load(DataStream * stream, NCount w, NCount h, PixelFormat pf);

        using Resource::load;

        /** 设置纹理宽度
        @remark 期望宽度,需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual void setWidth(NCount w);

        /** 获取纹理宽度
        @remark 期望宽度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual NCount getWidth() const;

        /** 返回原输入像素的宽度
        @remark 来源像素的宽度
        @version NIIEngine 3.0.0
        */
        virtual NCount getOriginWidth() const;

        /** 设置纹理高度
        @remark 期望高度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual void setHeight(NCount h);

        /** 获取纹理高度
        @remark 期望高度，需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual NCount getHeight() const;

        /** 返回原输入像素的宽度
        @remark 来源像素的宽度
        @version NIIEngine 3.0.0
        */
        virtual NCount getOriginHeight() const;

        /** 设置纹理深度
        @remark 期望深度，需在加载前设置
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        virtual void setDepth(NCount d);

        /** 返回纹理深度
        @remark 期望深度，需在加载前设置
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        virtual NCount getDepth() const;

        /** 返回原输入纹理的深度.
        @remark 来源像素的深度
        @par 3D纹理特性
        @version NIIEngine 3.0.0
        */
        virtual NCount getSrcDepth() const;

        /** 为纹理面设置像素格式
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual void setFormat(PixelFormat pf);

        /** 获取纹理需要像素格式
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual PixelFormat getFormat() const;

        /** 获取原输入纹理的像素格式
        @version NIIEngine 3.0.0
        */
        virtual PixelFormat getSrcFormat() const;

        /** 设置使用的mipmaps数量
        @remark 需在加载前设置, mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        virtual void setMipmapCount(NCount num);

        /** 获取使用的mipmaps数量
        @remark 需在加载前设置, mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        virtual NCount getMipmapCount() const;

        /** 获取原输入纹理的映射层数量
        @remark mipmap 就是纹理的LOD概念
        @version NIIEngine 3.0.0
        */
        NCount getSrcMipmapCount() const;
        
        /** 设置纹理类型
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual void setType(Type type);

        /** 获取纹理类型
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual Type getType() const;

        /** 返回这个纹理的面数量,
        @remark 正方映射纹理为6
        @version NIIEngine 3.0.0
        */
        virtual NCount getUnitCount() const;

        /** 设置应用在纹理加载的伽玛调整系数
        @remark 加载和转换管道为浮点值的时候让硬件去做伽玛颜色矫正
        @version NIIEngine 3.0.0
        */
        virtual void setHardwareSRGB(bool b);

        /** 设置应用在纹理加载的伽玛调整系数
        @version NIIEngine 3.0.0
        */
        virtual bool isHardwareSRGB() const;

        /** 设置 MemMode 标识符
        @remark 需在加载前设置
        @param[in] m MemMode 和 Buffer::Mode 多个组合
        @note 将会覆盖所有原来的属性
        @version NIIEngine 3.0.0 顶级api
        */
        void setModeMark(Nmark m);

        /** 获取 MemMode 标识符
        @remark 需在加载前设置
        @version NIIEngine 3.0.0 顶级api
        I*/
        Nmark getModeMark() const;

        /** 添加 MemMode 标识符
        @param[in] m 单个
        @version NIIEngine 3.0.0 高级api
        */
        void addModMark(Nmark m);

        /** 移去 MemMode 标识符
        @param[in] m 单个
        @version NIIEngine 3.0.0 高级api
        */
        void removeModMark(Nmark m);

        /** 是否存在 MemMode 标识符
        @version NIIEngine 3.0.0
        */
        bool isModMark(Nmark m) const;

        /** 设置伽玛调整因子
        @remark 需在加载前设置
        @note setHardwareGamma
        @version NIIEngine 3.0.0
        */
        virtual void setGamma(NIIf g);

        /** 返回伽玛调整系数
        @remark 需在加载前设置
        @version NIIEngine 3.0.0
        */
        virtual NIIf getGamma() const;

        /** 设置全屏抗锯齿,当纹理是AGP缓存对象时
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @param[in] fsaa 样本数量
        @version NIIEngine 3.0.0
        */
        virtual void setFSAA(Nui32 fsaa);

        /** 获取多重采样
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getFSAA() const;

        /** 设置多重采样扩展
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @version NIIEngine 3.0.0
        */
        void setFSAAExt(const String & ext);

        /** 获取多重采样扩展
        @remark 如果不存在 MM_AGP_XXXXX 成分或硬件不支持,忽略这个选项
        @version NIIEngine 3.0.0
        */
        const String & getFSAAExt() const;

        /** 返回硬件像素缓存
        @remark 这个函数很可能会访问 AGP 内存,很有可能会阻塞当前调用线程
        @param[in] mipmap 映射层
        @version NIIEngine 3.0.0
        */
        virtual FrameBuffer * getBuffer(Nidx mipmap = 0) = 0;

        /** 返回硬件像素缓存(多层纹理专用)
        @remark 这个函数很可能会访问 AGP 内存,很有可能会阻塞当前调用线程
        @param[in] face fn(0) = x, fn(1) = -x, fn(2) = y,
                        fn(3) = -y, fn(4) = z, fn(5) = -z
        @param[in] mipmap 映射层
        @version NIIEngine 3.0.0
        */
        virtual FrameBuffer * getCubeBuffer(CubeFace face = CF_RIGHT, Nidx mipmap = 0) = 0;

        /** 获取纹理原数据类型
        @verison NIIEngine 3.0.0
        */
        CodecID getRawType() const;

        /** 创建纹理内部资源.
        @remark 一般是指预先创建GPU/AGP内存,实际内容以后添加或者添加后改变
        @version NIIEngine 3.0.0
        */
        virtual void createInternal();

        /** 释放纹理内部资源
        @remark 一般是指预先创建GPU/AGP内存,实际内容以后添加或者添加后改变
        @version NIIEngine 3.0.0
        */
        virtual void freeInternal();
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
    protected:
        NCounts mWidth;                 ///< 纹理的宽(实际显示)
        NCounts mHeight;                ///< 纹理的高(实际显示)
        NCounts mDepth;                 ///< 纹理的深(实际显示)
        NCounts mSrcWidth;              ///< 原纹理(图片/像素)的宽
        NCounts mSrcHeight;             ///< 原纹理(图片/像素)的高
        NCounts mSrcDepth;              ///< 原纹理(图片/像素)的深
        PixelFormat mFormat;            ///< 象素格式(实际显示)
        PixelFormat mSrcFormat;         ///< 原象素(图片/像素)格式
        NCounts mMipmapCount;           ///< 纹理要求的Mipmaps
        NCounts mSrcMipmapCount;        ///< 纹理的Mipmaps
        Type mTextureType;              ///< 纹理类型
        NIIf mGamma;                    ///< 加码值
        Nui32 mFSAA;                    ///< 抗锯齿
        String mFSAAExt;                ///< 抗锯齿平台扩展
        Nmark mMark;                    ///< 掩码
        bool mInnerValid;
    };
}
#endif