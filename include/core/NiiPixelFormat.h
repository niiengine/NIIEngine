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

#ifndef _NII_PIXEL_FORMAT_H_
#define _NII_PIXEL_FORMAT_H_

#include "NiiPreInclude.h"
#include "NiiBox.h"

#define N_Linear(pf)    PixelUtil::getLinear(pf)
#define N_SRGB(pf)      PixelUtil::getSRGB(pf)

namespace NII
{
    /** 像素格式
    @version NIIEngine 3.0.0
    */
    enum PixelFormat
    {
        PF_UNKNOWN = 0,                 ///< 不明格式
        PF_L8ui = 1,                    ///< 8位亮度
        PF_L16ui = 2,                   ///< 16位亮度
        PF_A4L4i,                       ///< 4位透明度/4位亮度.
        PF_L8A8i,                       ///< 8位亮度/8位透明度
        PF_R5G6B5,
        PF_B5G6R5,
        PF_R3G3B2,
        PF_A4R4G4B4, 
        PF_A1R5G5B5,
        
        PF_A8R8G8B8,
        PF_A8B8G8R8,
        PF_R8G8B8A8UN,
        PF_R8G8B8A8UN_SRGB,
        PF_R8G8B8A8ui,
        PF_R8G8B8A8N,
        PF_R8G8B8A8i,
        
        PF_R16G16B16A16,
        PF_R16G16B16A16UN,
        PF_R16G16B16A16ui,
        PF_R16G16B16A16N,
        PF_R16G16B16A16i,
        
        PF_R32G32B32A32,
        PF_R32G32B32A32ui,
        PF_R32G32B32A32i,

        PF_R32G32B32,
        PF_R32G32B32ui,
        PF_R32G32B32i,
        
#if N_ENDIAN == N_ENDIAN_B
        PF_BYTE_RGB = PF_R8G8B8UN,      ///< 1字节红/绿/蓝
        PF_BYTE_BGR = PF_B8G8R8UN,      ///< 1字节蓝/绿/红
        PF_BYTE_BGRA = PF_B8G8R8A8UN,   ///< 1字节蓝/红/绿/透明度
        PF_BYTE_RGBA = PF_R8G8B8A8ui,   ///< 1字节红/绿/蓝/透明度
#else
        PF_BYTE_RGB = PF_B8G8R8UN,      ///< 1字节红/绿/蓝
        PF_BYTE_BGR = PF_R8G8B8UN,      ///< 1字节红/绿/蓝
        PF_BYTE_BGRA = PF_A8R8G8B8,     ///< 1字节蓝/绿/红/透明度
        PF_BYTE_RGBA = PF_A8B8G8R8,     ///< 1字节红/绿/蓝/透明度
#endif

        PF_R8G8UN,
        PF_R8G8ui,
        PF_R8G8N,
        PF_R8G8i,
        
        PF_G16R16,
        PF_G16R16ui,
        PF_R16G16,
        PF_R16G16UN,
        PF_R16G16ui,
        PF_R16G16N,
        PF_R16G16i,

        PF_G32R32,
        PF_R32G32,
        PF_R32G32ui,
        PF_R32G32i,

        PF_R8UN,
        PF_R8ui,
        PF_R8N,
        PF_R8i,
        PF_A8UN,
        PF_R1UN,
        
        PF_R16,
        PF_R16UN,
        PF_R16ui,
        PF_R16N,
        PF_R16i,
        
        PF_R32,
        PF_R32ui,
        PF_R32i,
        
        PF_Depth16UN,
        PF_Depth24UN,
        PF_Depth24UN_Stencil8ui,
        PF_Depth32f_Stencil8ui_64,
        PF_Depth32,

        PF_A2R10G10B10,
        PF_A2B10G10R10,
		PF_R10G10B10A2UN,
        PF_R10G10B10A2ui,
        PF_R11G11B10,
        
        PF_R9G9B9E5,
        /// Each 32-bit block describes a pair of pixels: (R8, G8, B8) and (R8, G8, B8)
        /// where the R8/B8 values are repeated, and the G8 values are unique to each pixel.
        PF_UYVY_R8G8B8G8UN,
        /// Each 32-bit block describes a pair of pixels: (R8, G8, B8) and (R8, G8, B8)
        /// where the R8/B8 values are repeated, and the G8 values are unique to each pixel.
        PF_YUY2_G8R8G8B8UN,

        /// BC1, aka DXT1 & DXT2
        PF_BC1UN,
        PF_BC1UN_SRGB,

        /// BC2, aka DXT3 & DXT4
        PF_BC2UN,
        PF_BC2UN_SRGB,

        /// BC3, aka DXT5
        PF_BC3UN,
        PF_BC3UN_SRGB,

        /// One channel compressed 4bpp. Ideal for greyscale.
        PF_BC4UN,
        /// Two channels compressed 8bpp. Ideal for normal maps or greyscale + alpha.
        PF_BC4N,

        PF_BC5UN,
        PF_BC5N,

        PF_B5G6R5UN,
        PF_B5G5R5A1UN,
        
        PF_X8R8G8B8,
        PF_X8B8G8R8,
        /// Avoid this one (prefer RGBA8).
        PF_B8G8R8A8UN,
        /// Avoid this one (prefer RGBA8).
        PF_B8G8R8X8UN,
        PF_R10G10B10A2UN_XR_BIAS,

        /// Avoid this one (prefer RGBA8).
        PF_B8G8R8A8UN_SRGB,
        /// Avoid this one (prefer RGBA8).
        PF_B8G8R8X8UN_SRGB,

        /// BC6H pf (unsigned 16 bit float)
        PF_BC6H_u16,
        /// BC6H pf (signed 16 bit float)
        PF_BC6H_16,

        PF_BC7_UN,
        PF_BC7_UN_SRGB,

        PF_AYUV,
        PF_Y410,
        PF_Y416,
        PF_NV12,
        PF_P010,
        PF_P016,
        PF_420_OPAQUE,
        PF_YUY2,
        PF_Y210,
        PF_Y216,
        PF_NV11,
        PF_AI44,
        PF_IA44,
        PF_P8,
        PF_A8P8,
        PF_B4G4R4A4UN,
        PF_P208,
        PF_V208,
        PF_V408,

        PF_R8G8B8UN,
        PF_R8G8B8UN_SRGB,
        PF_B8G8R8UN,
        PF_B8G8R8UN_SRGB,
        PF_R16G16B16ui,
        PF_R16G16B16UN,

        /// PVRTC (PowerVR) RGB 2 bpp
        PF_PVRTC_RGB_2BPP,
        PF_PVRTC_RGB_2BPP_SRGB,
        /// PVRTC (PowerVR) RGBA 2 bpp
        PF_PVRTC_RGBA_2BPP,
        PF_PVRTC_RGBA_2BPP_SRGB,
        /// PVRTC (PowerVR) RGB 4 bpp
        PF_PVRTC_RGB_4BPP,
        PF_PVRTC_RGB_4BPP_SRGB,
        /// PVRTC (PowerVR) RGBA 4 bpp
        PF_PVRTC_RGBA_4BPP,
        PF_PVRTC_RGBA_4BPP_SRGB,
        /// PVRTC (PowerVR) Version 2, 2 bpp
        PF_PVRTC2_2BPP,
        PF_PVRTC2_2BPP_SRGB,
        /// PVRTC (PowerVR) Version 2, 4 bpp
        PF_PVRTC2_4BPP,
        PF_PVRTC2_4BPP_SRGB,

        /// Ericsson Texture Compression
        PF_ETC1_R8G8B8UN,
        /// Ericsson Texture Compression. GLES 3.0
        PF_ETC2_R8G8B8UN,
        PF_ETC2_R8G8B8UN_SRGB,
        PF_ETC2_R8G8B8A8UN,
        PF_ETC2_R8G8B8A8UN_SRGB,
        PF_ETC2_R8G8B8A1UN,
        PF_ETC2_R8G8B8A1UN_SRGB,

        PF_EAC_R11UN,
        PF_EAC_R11N,
        PF_EAC_R11G11UN,
        PF_EAC_R11G11N,

        /// ATC (AMD_compressed_ATC_texture)
        PF_ATC_RGB,
        /// ATC (AMD_compressed_ATC_texture)
        PF_ATC_RGBA_EXPLICIT_A,
        /// ATC (AMD_compressed_ATC_texture)
        PF_ATC_RGBA_INTERPOLATED_A,

        /// ASTC (ARM Adaptive Scalable Texture Compression RGBA)
        PF_ASTC_RGBAUN_4X4_LDR,
        PF_ASTC_RGBAUN_5X4_LDR,
        PF_ASTC_RGBAUN_5X5_LDR,
        PF_ASTC_RGBAUN_6X5_LDR,
        PF_ASTC_RGBAUN_6X6_LDR,
        PF_ASTC_RGBAUN_8X5_LDR,
        PF_ASTC_RGBAUN_8X6_LDR,
        PF_ASTC_RGBAUN_8X8_LDR,
        PF_ASTC_RGBAUN_10X5_LDR,
        PF_ASTC_RGBAUN_10X6_LDR,
        PF_ASTC_RGBAUN_10X8_LDR,
        PF_ASTC_RGBAUN_10X10_LDR,
        PF_ASTC_RGBAUN_12X10_LDR,
        PF_ASTC_RGBAUN_12X12_LDR,

        /// ASTC (ARM Adaptive Scalable Texture Compression RGBA_UNORM sRGB)
        PF_ASTC_RGBAUN_4X4_SRGB,
        PF_ASTC_RGBAUN_5X4_SRGB,
        PF_ASTC_RGBAUN_5X5_SRGB,
        PF_ASTC_RGBAUN_6X5_SRGB,
        PF_ASTC_RGBAUN_6X6_SRGB,
        PF_ASTC_RGBAUN_8X5_SRGB,
        PF_ASTC_RGBAUN_8X6_SRGB,
        PF_ASTC_RGBAUN_8X8_SRGB,
        PF_ASTC_RGBAUN_10X5_SRGB,
        PF_ASTC_RGBAUN_10X6_SRGB,
        PF_ASTC_RGBAUN_10X8_SRGB,
        PF_ASTC_RGBAUN_10X10_SRGB,
        PF_ASTC_RGBAUN_12X10_SRGB,
        PF_ASTC_RGBAUN_12X12_SRGB,
        
        PF_COUNT
    };
    typedef vector<PixelFormat>::type PixelFormatList;
    
    enum PixelLayoutType
    {
        PLT_R8G8B8A8,
        PLT_B8G8R8A8,
        PLT_R8G8B8X8,
        PLT_B8G8R8X8,
        PLT_R8G8B8,
        PLT_B8G8R8,
        PLT_R8G8,
        PLT_R8,
        PLT_R16G16B16A16,
        PLT_R16G16B16,
        PLT_R16G16,
        PLT_R16,
        PLT_R32G32B32A32,
        PLT_R32G32B32,
        PLT_R32G32,
        PLT_R32,
        
        PLT_Compress,

        PLT_Count
    };

    /** 像素格式特点
    @version NIIEngine 3.0.0
    */
    enum PixelFormatFeature
    {
        PFF_None            = 0,
        PFF_Float           = 0x01,
        PFF_Float16         = 0x02,
        PFF_FloatE          = 0x40,
        PFF_Int             = 0x80,
        PFF_Int16           = 0x100,
        PFF_Signed          = 0x200,        ///< [-, +]
        PFF_Unit            = 0x400,        ///< [0.0, 1.0] or [-1.0, 1.0]
        PFF_Alpha           = 0x800,        ///< xxxA
        PFF_Stencil         = 0x1000,
        PFF_Depth           = 0x2000,
        PFF_Compress        = 0x4000,
        PFF_SRGB            = 0x8000,
        PFF_Grayscale       = 0x10000,      ///< 灰阶格式,没有颜色,只代表黑到白的过渡
        PFF_Pallete         = 0x20000,
        PFF_NonEndian       = 0x40000       ///< 一般8位基元的都是非字节序相关数据
    };
    
    class _EngineAPI PixelVolume : public BufferAlloc
    {
    public:
        PixelVolume();
        PixelVolume(NCount width, NCount height, NCount depth, PixelFormat pf);
        PixelVolume(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf);
        
        /** 设置格式
        @version NIIEngine 3.0.0
        */
        inline void setFormat(PixelFormat pf)       { mFormat = pf; }

        /** 获取格式
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const        { return mFormat; }
        
        /** 获取这个盒子的宽
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const              { return mWidth; }

        /** 获取这个盒子的高
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const             { return mHeight; }

        /** 获取这个盒子的深
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const              { return mDepth; }
        
        /** 获取这个盒子的数组
        @version NIIEngine 3.0.0
        */
        inline NCount getArray() const              { return mArray; }
        
        /** 是否容积相等
        @verison NIIEngine 3.0.0
        */
        bool equalVolume(const PixelVolume & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mDepth == o.mDepth && mArray == o.mArray && 
                mFormat == o.mFormat;
        }
        
        /** 是否片相等
        @verison NIIEngine 3.0.0
        */
        bool equalSlice(const PixelVolume & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mFormat == o.mFormat;
        }
    public:
        NCount mWidth, mHeight, mDepth, mArray; ///< 假设以后支持3D Array纹理
        PixelFormat mFormat;
    };

    /** 像素块
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBlock : public PixelVolume
    {
    public:
        /** 像素过滤
        @version NIIEngine 3.0.0
        */
        enum Filter
        {
            F_Nearest,
            F_Linear,
            F_Bilinear,
            F_Gaussian,
            F_Gaussian_Bilinear
        };
    public:
        PixelBlock();
        PixelBlock(const Box & area, PixelFormat pf = PF_UNKNOWN, Nui8 * data = 0);
        PixelBlock(NCount width, NCount height, NCount depth, NCount array = 1, PixelFormat pf = PF_UNKNOWN, Nui8 * data = 0);
        PixelBlock(NCount width, NCount height, NCount rowsize, PixelFormat pf, Nui8 * data = 0);
        PixelBlock(NCount width, NCount height, NCount depth, NCount array, NCount unitSize, NCount rowSize, NCount sliceSize, 
            PixelFormat pf = PF_UNKNOWN, Nui8 * data = 0);
        ~PixelBlock();

        /** 获取总大小
        @version NIIEngine 3.0.0
        */
        inline NCount getSize() const           { return mSliceSize * getVolume(); }

        /** 是否压缩格式
        @version NIIEngine 3.0.0
        */
        inline bool isCompress() const          { return PixelUtil::isCompress(pf); }
        
        /** 空
        @version NIIEngine 3.0.0
        */
        inline bool isEmpty() const             { return mWidth == 0 && mHeight == 0 && mDepth == 0;}
        
        /** 是否容积相等
        @version NIIEngine 3.0.0
        */
        bool equalVolume(const PixelBlock & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mDepth == o.mDepth && mArray == o.mArray;
        }
        
        /** 是否片相等
        @version NIIEngine 3.0.0
        */
        bool equalSlice(const PixelBlock & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mFormat == o.mFormat;
        }
        
        /** 是否数据结构相等
        @verison NIIEngine 3.0.0
        */
        bool equal(const PixelBlock & o) const
        {
            return mLeft == o.mLeft && mTop == o.mTop && mFront == o.mFront && mArrayIdex == o.mArrayIdex &&
                mWidth == o.mWidth && mHeight == o.mHeight && mDepth == o.mDepth && mArray == o.mArray &&
                    mUnitSize == o.mUnitSize && mRowSize == o.mRowSize && mSliceSize == o.mSliceSize;
        }
        
        /** 是否覆盖
        @verison NIIEngine 3.0.0
        */
        inline bool overlaps(const Box & o) const
        {
            return!(o.mLeft >= getRight() || o.mTop >= getBottom() || o.mFront >= getBack() || o.mFront >= getMaxArray() ||
                o.mRight <= mLeft || o.mBottom <= mTop || o.mBack <= mFront || o.getMaxArray() <= mFront);
        }
        
        /** 是否包含
        @verison NIIEngine 3.0.0
        */
        bool contains( const PixelBlock &o ) const
        {
            return o.mLeft >= mLeft && o.getRight() <= getRight() && o.mTop >= mTop && o.getBottom() <= getBottom() &&
                o.mFront >= mFront && o.getBack() <= getBack() && o.mArrayIdx >= mArrayIdx && o.getMaxArray() <= getMaxArray();
        }
        
        /** 获取这个盒子的右边
        @version NIIEngine 3.0.0
        */
        inline NCount getRight() const              { return mLeft + mWidth; }
        
        /** 获取这个盒子的下边
        @version NIIEngine 3.0.0
        */
        inline NCount getBottom() const             { return mTop + mHeight; }
        
        /** 获取这个盒子的后边
        @version NIIEngine 3.0.0
        */
        inline NCount getBack() const               { return mFront + mDepth; }
        
        /** 获取这个盒子的最大数组数
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxArray() const           { return mArrayIdx + mArray; }
        
        /** 获取这个盒子的容积
        @version NIIEngine 3.0.0
        */
        inline NCount getVolume() const             { return mDepth * mArray; }
        
        /** 获取这个盒子的容积开始
        @version NIIEngine 3.0.0
        */
        inline NCount getVolumeIdex() const         { return mFront + mArrayIdx; }

        /** 行偏量
        @version NIIEngine 3.0.0
        */
        inline NCount getRowOft() const             { return mRowSize - mWidth * mUnitSize; }

        /** 片偏量
        @version NIIEngine 3.0.0
        */
        inline NCount getSliceOft() const           { return mSliceSize - mHeight * mRowSize; }

        /** 是否一整片
        @version NIIEngine 3.0.0
        */
        bool isWholeSlice() const;

        /** 获取一整片的大小
        @version NIIEngine 3.0.0
        */
        inline NCount getWholeSliceSize() const     { return PixelUtil::getSize(mWidth, mHeight, mDepth, mArray, mFormat);}

        /** 获取头部指针
        @version NIIEngine 3.0.0
        */
        inline Nui8 * getData(NCount xoft, NCount yoft, NCount zoft) const   { return getOriginData(xoft + mLeft, yoft + mTop, zoft + getVolumeIdex()); }
        
        /** 获取头部指针
        @version NIIEngine 3.0.0
        */
        Nui8 * getOriginData(NCount xoft, NCount yoft, NCount zoft) const;
        
        /** 复制数据
        @version NIIEngine 3.0.0
        */
        void write(const PixelBlock & src);

        /** 获取子区域
        @note 使用同一个数据区域,所以子区域使用完后不能删除数据指针
        @version NIIEngine 3.0.0
        */
        PixelBlock getSub(const Box & area) const;

        /** 设置颜色
        @version NIIEngine 3.0.0
        */
        void setColour(const Colour & out, NCount x, NCount y, NCount z);

        /** 获取颜色
        @version NIIEngine 3.0.0
        */
        void getColour(Colour & out, NCount x, NCount y, NCount z);

        /** 垂直翻转像素
        @version NIIEngine 4.0.0
        */
        void verticalFlip();

        /** 转换
        @note 像素格式依照src,尺寸依照dst,dst存放缩放后的缓存(覆盖原来的缓存)
        @param[in] src 源
        @param[in] dst 目标
        @version NIIEngine 3.0.0
        */
        static void scale(const PixelBlock & src, PixelBlock & dst, Filter mode = F_Bilinear);

        /** 调整伽玛
        @param[in] size 数据大小
        @param[in] bpp 位深
        @version NIIEngine 3.0.0
        */
        static void applyGamma(Nui8 * data, NIIf gamma, NCount size, NCount bpp);
    public:
        const Nui8 * mData;
        NCount mLeft, mTop, mFront, mArrayIdex; ///< 假设以后支持3D Array纹理
        NCount mUnitSize;                       ///< 单位字节
        NCount mRowSize;                        ///< 单位字节
        NCount mSliceSize;                      ///< 单位字节
    };
    
    /** 像素数据实用功能
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelUtil
    {
    public:
        /** 转换格式
        @param[in] src 指向资源区域
        @param[in] srcpf 资源区域的像素格式
        @param[in] dst 指向目的区域
        @param[in] destpf 目的区域的像素格式
        @version NIIEngine 3.0.0
        */
        static void conv(Nui8 * src, PixelFormat srcpf, Nui8 * dst, PixelFormat destpf, NCount count);

        /** 转换格式
        @remark 资源和目的盒必须相同的尺寸. 如果资源和目标格式匹配,将简单复制
        @param[in] src PixelBlock包含资源像素,节距和格式
        @param[in] dst PixelBlock包含目的像素,节距和格式
        @version NIIEngine 3.0.0
        */
        static void conv(const PixelBlock & src, PixelBlock & dst, bool verticalFlip = false);

        /** 获取内存大小
        @note 不是压缩
        @param[in] pf 该范围的格式
        @return 字节大小
        @version NIIEngine 3.0.0
        */
        static NCount getSize(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf, NCount mipCnt = 1, NCount rowAlign = 4u);

        /** 最大Mipmap数量
        @version NIIEngine 5.0.0
        */
        static NCount getMaxMipmap(NCount resolution);
        
        /** 最大Mipmap数量
        @version NIIEngine 5.0.0
        */
        static NCount getMaxMipmap(NCount width, NCount height)                {return getMaxMipmap(std::max(width, height));}
        
        /** 最大Mipmap数量
        @version NIIEngine 5.0.0
        */
        static NCount getMaxMipmap(NCount width, NCount height, NCount depth)  { return getMaxMipmap(std::max(std::max(width, height), depth));}

        /** 获取压缩块宽
        @version NIIEngine 5.0.0
        */
        static uint32 getBlockWidth(PixelFormat pf, bool apiStrict = true);

        /** 获取压缩块高
        @version NIIEngine 5.0.0
        */
        static uint32 getBlockHeight(PixelFormat pf, bool apiStrict = true);

        /** 获取数据
        @version NIIEngine 5.0.0
        */
        static Nui8 * getData(Nui8 * base, NCount width, NCount height, NCount depth, NCount array, NCount mipLevel, PixelFormat pf);

        /** 获取 SRGB 格式
        @version NIIEngine 3.0.0
        */
        static PixelFormat getSRGB(PixelFormat pf);

        /** 获取线性格式
        @version NIIEngine 3.0.0
        */
        static PixelFormat getLinear(PixelFormat srgbPf);
        
        /** 获取图象内部标识
        @version NIIEngine 3.0.0
        */
        static StringView getMarkName(PixelFormat srcpf);

        /** 获取像素格式特征标记
        @return 如PFF_Alpha, PFF_Float, PFF_Depth 组合特征
        */
        static Nui32 getExentMark(PixelFormat pf);
        
        /** 获取族类型
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFamily(PixelFormat pf);

        /** 获取格式是否有SRGB特征
        @version NIIEngine 3.0.0
        */
        static bool isSRGB(PixelFormat pf);

        /** 获取格式是否有PalleteYUV特征
        @version NIIEngine 3.0.0
        */
        static bool isPalleteYUV(PixelFormat pf);

        /** 获取格式中是否有透明成份
        @version NIIEngine 3.0.0
        */
        static bool isAlpha(PixelFormat pf);

        /** 获取格式是否浮点
        @version NIIEngine 3.0.0
        */
        static bool isFloat(PixelFormat pf);
        
        /** 获取格式是否16位浮点
        @version NIIEngine 5.0.0
        */
        static bool isFloat16(PixelFormat pf);

        /** 获取格式是否浮点
        @version NIIEngine 5.0.0
        */
        static bool isFloatRare(PixelFormat pf);
        
        /** 获取格式是否整形特征
        @version NIIEngine 5.0.0
        */
        static bool isInteger(PixelFormat pf);
        
        /** 获取格式是否归一化特征[0.0, 1.0]
        @version NIIEngine 5.0.0
        */
        static bool isUnit(PixelFormat pf);

        /** 获取格式是否有符号特征
        @version NIIEngine 5.0.0
        */
        static bool isSigned(PixelFormat pf);

        /** 获取格式是否有压缩特征
        @version NIIEngine 3.0.0
        */
        static bool isCompress(PixelFormat pf);

        /** 获取格式是否有深度特征
        @version NIIEngine 3.0.0
        */
        static bool isDepth(PixelFormat pf);
        
        /** 获取格式是否有模板特征
        @version NIIEngine 3.0.0
        */
        static bool isStencil(PixelFormat pf);

        /** 获取格式使用本机字节顺序
        @version NIIEngine 3.0.0
        */
        static bool isNativeEndian(PixelFormat pf);

        /** 获取格式是否亮度特征.
        @version NIIEngine 3.0.0
        */
        static bool isGrayscale(PixelFormat pf);
        
        /** 获取格式是否有硬件Mipmap特征
        @version NIIEngine 5.0.0
        */
        static bool isHwMipmap(PixelFormat pf);
        
        /** 是否直接操作格式
        @return 是则返回真,否则返回假
        @version NIIEngine 3.0.0
        */
        static bool isDirect(PixelFormat srcpf);
        
        /** 是否支持软件生成层次映射
        @version NIIEngine 5.0.0
        */
        static bool isSwMipmaps( PixelFormat pf, NCount depth, NCount face, Filter filter);
        
        /** 获取(RGBA)位深
        @remark 对于非彩色格式(dxt, depth)这个返回[0,0,0,0]
        @version NIIEngine 3.0.0
        */
        static void getBitDepth(PixelFormat pf, NIIi rgba[4]);

        /** 获取(RGBA)掩码
        @note 仅本地字节序有效
        @version NIIEngine 3.0.0
        */
        static void getBitMask(PixelFormat pf, Nui32 rgba[4]);

        /** 获取(RGBA)位移
        @note 仅本地字节序有效
        @version NIIEngine 3.0.0
        */
        static void getBitShift(PixelFormat pf, Nui8 rgba[4]);

        /** 获取元素大小(字节)
        @remark 输入PF_UNKNOWN结果将返回0字节大小, BytesPrePixel
        @version NIIEngine 3.0.0
        */
        static NCount getUnitSize(PixelFormat pf);

        /** 获取像素数据成分数量
        @return 返回3(无透明度)或4(有透明度)
        @version NIIEngine 3.0.0
        */
        static NCount getDataCount(PixelFormat pf);
        
        /** 获取像素数据排列方式
        @version NIIEngine 5.0.0
        */
        static PixelLayoutType getDataLayout(PixelFormat pf);

        /** 给定名字,获取格式
        @param[in] markname 格式名字
        @param[in] direct 直接可以使用的格式
        @param[in] caseSensitive 格式名字是否区分大小写
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFormat(const String & markname, Nui32 exclusionfeature = 0, bool direct = false);

        /** 返回相似图像格式
        @return位深依照首选位深
        @param[in] pf 原格式.
        @param[in] intBits 整型像素格首选位深(像素位).0,16,32.
        @param[in] floatBits NIIf 浮点像素首选位深(像素位).0,16,32.
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFormat(PixelFormat pf, Nui16 intBits, Nui16 floatBits);

        /** 打包一个颜色值到内存
        @param[in] c 颜色
        @param[in] pf 目的像素格式
        @param[in] dest 目的内存位置
        @version NIIEngine 3.0.0
        */
        static void packColour(const Colour & c, PixelFormat dstpf, Nui8 * dst);

        /** 转换颜色值到内存
        @param[in] r,g,b,a 颜色成分
        @param[in] pf 目的像素格式
        @param[in] dest 目的内存位置
        @version NIIEngine 3.0.0
        */
        static void packColour(Nui8 r, Nui8 g, Nui8 b, Nui8 a, PixelFormat dstpf, Nui8 * dst);

        /** 转换颜色值到内存
       @param[in] r,g,b,a 颜色成分
       @param[in] pf 目的像素格式
       @param[in] dest 目的内存位置
       @version NIIEngine 3.0.0
       */
        static void packColour(NIIf r, NIIf g, NIIf b, NIIf a, PixelFormat dstpf, Nui8 * dst);

        /** 从内存中解包一个颜色值
        @param[in] dst 返回的颜色
        @param[in] pf 需读取的颜色的像素格式
        @param[in] src 资源内存位置
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Colour * dst, PixelFormat srcpf, const Nui8 * src);

        /** 从内存中解包一个颜色值
        @param[in] r,g,b,a 返回的颜色
        @param[in] pf 资源的像素格式
        @param[in] src 资源内存位置
        @remark 8位精度的颜色成分
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Nui8 & r, Nui8 & g, Nui8 & b, Nui8 & a, PixelFormat srcpf, const Nui8 * src);

        /** 从内存中解包一个颜色值
        @param[in] r,g,b,a 返回的颜色
        @param[in] pf 资源的像素格式
        @param[in] src 资源内存位置
        @version NIIEngine 3.0.0
        */
        static void unpackColour(NIIf & r, NIIf & g, NIIf & b, NIIf & a, PixelFormat pf, const Nui8 * src);
    };
}

#endif