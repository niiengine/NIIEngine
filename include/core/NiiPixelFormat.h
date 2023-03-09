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
    /** ���ظ�ʽ
    @version NIIEngine 3.0.0
    */
    enum PixelFormat
    {
        PF_UNKNOWN = 0,                 ///< ������ʽ
        PF_L8ui = 1,                    ///< 8λ����
        PF_L16ui = 2,                   ///< 16λ����
        PF_A4L4i,                       ///< 4λ͸����/4λ����.
        PF_L8A8i,                       ///< 8λ����/8λ͸����
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
        PF_BYTE_RGB = PF_R8G8B8UN,      ///< 1�ֽں�/��/��
        PF_BYTE_BGR = PF_B8G8R8UN,      ///< 1�ֽ���/��/��
        PF_BYTE_BGRA = PF_B8G8R8A8UN,   ///< 1�ֽ���/��/��/͸����
        PF_BYTE_RGBA = PF_R8G8B8A8ui,   ///< 1�ֽں�/��/��/͸����
#else
        PF_BYTE_RGB = PF_B8G8R8UN,      ///< 1�ֽں�/��/��
        PF_BYTE_BGR = PF_R8G8B8UN,      ///< 1�ֽں�/��/��
        PF_BYTE_BGRA = PF_A8R8G8B8,     ///< 1�ֽ���/��/��/͸����
        PF_BYTE_RGBA = PF_A8B8G8R8,     ///< 1�ֽں�/��/��/͸����
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

    /** ���ظ�ʽ�ص�
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
        PFF_Grayscale       = 0x10000,      ///< �ҽ׸�ʽ,û����ɫ,ֻ����ڵ��׵Ĺ���
        PFF_Pallete         = 0x20000,
        PFF_NonEndian       = 0x40000       ///< һ��8λ��Ԫ�Ķ��Ƿ��ֽ����������
    };
    
    class _EngineAPI PixelVolume : public BufferAlloc
    {
    public:
        PixelVolume();
        PixelVolume(NCount width, NCount height, NCount depth, PixelFormat pf);
        PixelVolume(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf);
        
        /** ���ø�ʽ
        @version NIIEngine 3.0.0
        */
        inline void setFormat(PixelFormat pf)       { mFormat = pf; }

        /** ��ȡ��ʽ
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const        { return mFormat; }
        
        /** ��ȡ������ӵĿ�
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const              { return mWidth; }

        /** ��ȡ������ӵĸ�
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const             { return mHeight; }

        /** ��ȡ������ӵ���
        @version NIIEngine 3.0.0
        */
        inline NCount getDepth() const              { return mDepth; }
        
        /** ��ȡ������ӵ�����
        @version NIIEngine 3.0.0
        */
        inline NCount getArray() const              { return mArray; }
        
        /** �Ƿ��ݻ����
        @verison NIIEngine 3.0.0
        */
        bool equalVolume(const PixelVolume & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mDepth == o.mDepth && mArray == o.mArray && 
                mFormat == o.mFormat;
        }
        
        /** �Ƿ�Ƭ���
        @verison NIIEngine 3.0.0
        */
        bool equalSlice(const PixelVolume & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mFormat == o.mFormat;
        }
    public:
        NCount mWidth, mHeight, mDepth, mArray; ///< �����Ժ�֧��3D Array����
        PixelFormat mFormat;
    };

    /** ���ؿ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBlock : public PixelVolume
    {
    public:
        /** ���ع���
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

        /** ��ȡ�ܴ�С
        @version NIIEngine 3.0.0
        */
        inline NCount getSize() const           { return mSliceSize * getVolume(); }

        /** �Ƿ�ѹ����ʽ
        @version NIIEngine 3.0.0
        */
        inline bool isCompress() const          { return PixelUtil::isCompress(pf); }
        
        /** ��
        @version NIIEngine 3.0.0
        */
        inline bool isEmpty() const             { return mWidth == 0 && mHeight == 0 && mDepth == 0;}
        
        /** �Ƿ��ݻ����
        @version NIIEngine 3.0.0
        */
        bool equalVolume(const PixelBlock & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mDepth == o.mDepth && mArray == o.mArray;
        }
        
        /** �Ƿ�Ƭ���
        @version NIIEngine 3.0.0
        */
        bool equalSlice(const PixelBlock & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight && mFormat == o.mFormat;
        }
        
        /** �Ƿ����ݽṹ���
        @verison NIIEngine 3.0.0
        */
        bool equal(const PixelBlock & o) const
        {
            return mLeft == o.mLeft && mTop == o.mTop && mFront == o.mFront && mArrayIdex == o.mArrayIdex &&
                mWidth == o.mWidth && mHeight == o.mHeight && mDepth == o.mDepth && mArray == o.mArray &&
                    mUnitSize == o.mUnitSize && mRowSize == o.mRowSize && mSliceSize == o.mSliceSize;
        }
        
        /** �Ƿ񸲸�
        @verison NIIEngine 3.0.0
        */
        inline bool overlaps(const Box & o) const
        {
            return!(o.mLeft >= getRight() || o.mTop >= getBottom() || o.mFront >= getBack() || o.mFront >= getMaxArray() ||
                o.mRight <= mLeft || o.mBottom <= mTop || o.mBack <= mFront || o.getMaxArray() <= mFront);
        }
        
        /** �Ƿ����
        @verison NIIEngine 3.0.0
        */
        bool contains( const PixelBlock &o ) const
        {
            return o.mLeft >= mLeft && o.getRight() <= getRight() && o.mTop >= mTop && o.getBottom() <= getBottom() &&
                o.mFront >= mFront && o.getBack() <= getBack() && o.mArrayIdx >= mArrayIdx && o.getMaxArray() <= getMaxArray();
        }
        
        /** ��ȡ������ӵ��ұ�
        @version NIIEngine 3.0.0
        */
        inline NCount getRight() const              { return mLeft + mWidth; }
        
        /** ��ȡ������ӵ��±�
        @version NIIEngine 3.0.0
        */
        inline NCount getBottom() const             { return mTop + mHeight; }
        
        /** ��ȡ������ӵĺ��
        @version NIIEngine 3.0.0
        */
        inline NCount getBack() const               { return mFront + mDepth; }
        
        /** ��ȡ������ӵ����������
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxArray() const           { return mArrayIdx + mArray; }
        
        /** ��ȡ������ӵ��ݻ�
        @version NIIEngine 3.0.0
        */
        inline NCount getVolume() const             { return mDepth * mArray; }
        
        /** ��ȡ������ӵ��ݻ���ʼ
        @version NIIEngine 3.0.0
        */
        inline NCount getVolumeIdex() const         { return mFront + mArrayIdx; }

        /** ��ƫ��
        @version NIIEngine 3.0.0
        */
        inline NCount getRowOft() const             { return mRowSize - mWidth * mUnitSize; }

        /** Ƭƫ��
        @version NIIEngine 3.0.0
        */
        inline NCount getSliceOft() const           { return mSliceSize - mHeight * mRowSize; }

        /** �Ƿ�һ��Ƭ
        @version NIIEngine 3.0.0
        */
        bool isWholeSlice() const;

        /** ��ȡһ��Ƭ�Ĵ�С
        @version NIIEngine 3.0.0
        */
        inline NCount getWholeSliceSize() const     { return PixelUtil::getSize(mWidth, mHeight, mDepth, mArray, mFormat);}

        /** ��ȡͷ��ָ��
        @version NIIEngine 3.0.0
        */
        inline Nui8 * getData(NCount xoft, NCount yoft, NCount zoft) const   { return getOriginData(xoft + mLeft, yoft + mTop, zoft + getVolumeIdex()); }
        
        /** ��ȡͷ��ָ��
        @version NIIEngine 3.0.0
        */
        Nui8 * getOriginData(NCount xoft, NCount yoft, NCount zoft) const;
        
        /** ��������
        @version NIIEngine 3.0.0
        */
        void write(const PixelBlock & src);

        /** ��ȡ������
        @note ʹ��ͬһ����������,����������ʹ�������ɾ������ָ��
        @version NIIEngine 3.0.0
        */
        PixelBlock getSub(const Box & area) const;

        /** ������ɫ
        @version NIIEngine 3.0.0
        */
        void setColour(const Colour & out, NCount x, NCount y, NCount z);

        /** ��ȡ��ɫ
        @version NIIEngine 3.0.0
        */
        void getColour(Colour & out, NCount x, NCount y, NCount z);

        /** ��ֱ��ת����
        @version NIIEngine 4.0.0
        */
        void verticalFlip();

        /** ת��
        @note ���ظ�ʽ����src,�ߴ�����dst,dst������ź�Ļ���(����ԭ���Ļ���)
        @param[in] src Դ
        @param[in] dst Ŀ��
        @version NIIEngine 3.0.0
        */
        static void scale(const PixelBlock & src, PixelBlock & dst, Filter mode = F_Bilinear);

        /** ����٤��
        @param[in] size ���ݴ�С
        @param[in] bpp λ��
        @version NIIEngine 3.0.0
        */
        static void applyGamma(Nui8 * data, NIIf gamma, NCount size, NCount bpp);
    public:
        const Nui8 * mData;
        NCount mLeft, mTop, mFront, mArrayIdex; ///< �����Ժ�֧��3D Array����
        NCount mUnitSize;                       ///< ��λ�ֽ�
        NCount mRowSize;                        ///< ��λ�ֽ�
        NCount mSliceSize;                      ///< ��λ�ֽ�
    };
    
    /** ��������ʵ�ù���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelUtil
    {
    public:
        /** ת����ʽ
        @param[in] src ָ����Դ����
        @param[in] srcpf ��Դ��������ظ�ʽ
        @param[in] dst ָ��Ŀ������
        @param[in] destpf Ŀ����������ظ�ʽ
        @version NIIEngine 3.0.0
        */
        static void conv(Nui8 * src, PixelFormat srcpf, Nui8 * dst, PixelFormat destpf, NCount count);

        /** ת����ʽ
        @remark ��Դ��Ŀ�ĺб�����ͬ�ĳߴ�. �����Դ��Ŀ���ʽƥ��,���򵥸���
        @param[in] src PixelBlock������Դ����,�ھ�͸�ʽ
        @param[in] dst PixelBlock����Ŀ������,�ھ�͸�ʽ
        @version NIIEngine 3.0.0
        */
        static void conv(const PixelBlock & src, PixelBlock & dst, bool verticalFlip = false);

        /** ��ȡ�ڴ��С
        @note ����ѹ��
        @param[in] pf �÷�Χ�ĸ�ʽ
        @return �ֽڴ�С
        @version NIIEngine 3.0.0
        */
        static NCount getSize(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf, NCount mipCnt = 1, NCount rowAlign = 4u);

        /** ���Mipmap����
        @version NIIEngine 5.0.0
        */
        static NCount getMaxMipmap(NCount resolution);
        
        /** ���Mipmap����
        @version NIIEngine 5.0.0
        */
        static NCount getMaxMipmap(NCount width, NCount height)                {return getMaxMipmap(std::max(width, height));}
        
        /** ���Mipmap����
        @version NIIEngine 5.0.0
        */
        static NCount getMaxMipmap(NCount width, NCount height, NCount depth)  { return getMaxMipmap(std::max(std::max(width, height), depth));}

        /** ��ȡѹ�����
        @version NIIEngine 5.0.0
        */
        static uint32 getBlockWidth(PixelFormat pf, bool apiStrict = true);

        /** ��ȡѹ�����
        @version NIIEngine 5.0.0
        */
        static uint32 getBlockHeight(PixelFormat pf, bool apiStrict = true);

        /** ��ȡ����
        @version NIIEngine 5.0.0
        */
        static Nui8 * getData(Nui8 * base, NCount width, NCount height, NCount depth, NCount array, NCount mipLevel, PixelFormat pf);

        /** ��ȡ SRGB ��ʽ
        @version NIIEngine 3.0.0
        */
        static PixelFormat getSRGB(PixelFormat pf);

        /** ��ȡ���Ը�ʽ
        @version NIIEngine 3.0.0
        */
        static PixelFormat getLinear(PixelFormat srgbPf);
        
        /** ��ȡͼ���ڲ���ʶ
        @version NIIEngine 3.0.0
        */
        static StringView getMarkName(PixelFormat srcpf);

        /** ��ȡ���ظ�ʽ�������
        @return ��PFF_Alpha, PFF_Float, PFF_Depth �������
        */
        static Nui32 getExentMark(PixelFormat pf);
        
        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFamily(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ���SRGB����
        @version NIIEngine 3.0.0
        */
        static bool isSRGB(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ���PalleteYUV����
        @version NIIEngine 3.0.0
        */
        static bool isPalleteYUV(PixelFormat pf);

        /** ��ȡ��ʽ���Ƿ���͸���ɷ�
        @version NIIEngine 3.0.0
        */
        static bool isAlpha(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ񸡵�
        @version NIIEngine 3.0.0
        */
        static bool isFloat(PixelFormat pf);
        
        /** ��ȡ��ʽ�Ƿ�16λ����
        @version NIIEngine 5.0.0
        */
        static bool isFloat16(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ񸡵�
        @version NIIEngine 5.0.0
        */
        static bool isFloatRare(PixelFormat pf);
        
        /** ��ȡ��ʽ�Ƿ���������
        @version NIIEngine 5.0.0
        */
        static bool isInteger(PixelFormat pf);
        
        /** ��ȡ��ʽ�Ƿ��һ������[0.0, 1.0]
        @version NIIEngine 5.0.0
        */
        static bool isUnit(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ��з�������
        @version NIIEngine 5.0.0
        */
        static bool isSigned(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ���ѹ������
        @version NIIEngine 3.0.0
        */
        static bool isCompress(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ����������
        @version NIIEngine 3.0.0
        */
        static bool isDepth(PixelFormat pf);
        
        /** ��ȡ��ʽ�Ƿ���ģ������
        @version NIIEngine 3.0.0
        */
        static bool isStencil(PixelFormat pf);

        /** ��ȡ��ʽʹ�ñ����ֽ�˳��
        @version NIIEngine 3.0.0
        */
        static bool isNativeEndian(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ���������.
        @version NIIEngine 3.0.0
        */
        static bool isGrayscale(PixelFormat pf);
        
        /** ��ȡ��ʽ�Ƿ���Ӳ��Mipmap����
        @version NIIEngine 5.0.0
        */
        static bool isHwMipmap(PixelFormat pf);
        
        /** �Ƿ�ֱ�Ӳ�����ʽ
        @return ���򷵻���,���򷵻ؼ�
        @version NIIEngine 3.0.0
        */
        static bool isDirect(PixelFormat srcpf);
        
        /** �Ƿ�֧��������ɲ��ӳ��
        @version NIIEngine 5.0.0
        */
        static bool isSwMipmaps( PixelFormat pf, NCount depth, NCount face, Filter filter);
        
        /** ��ȡ(RGBA)λ��
        @remark ���ڷǲ�ɫ��ʽ(dxt, depth)�������[0,0,0,0]
        @version NIIEngine 3.0.0
        */
        static void getBitDepth(PixelFormat pf, NIIi rgba[4]);

        /** ��ȡ(RGBA)����
        @note �������ֽ�����Ч
        @version NIIEngine 3.0.0
        */
        static void getBitMask(PixelFormat pf, Nui32 rgba[4]);

        /** ��ȡ(RGBA)λ��
        @note �������ֽ�����Ч
        @version NIIEngine 3.0.0
        */
        static void getBitShift(PixelFormat pf, Nui8 rgba[4]);

        /** ��ȡԪ�ش�С(�ֽ�)
        @remark ����PF_UNKNOWN���������0�ֽڴ�С, BytesPrePixel
        @version NIIEngine 3.0.0
        */
        static NCount getUnitSize(PixelFormat pf);

        /** ��ȡ�������ݳɷ�����
        @return ����3(��͸����)��4(��͸����)
        @version NIIEngine 3.0.0
        */
        static NCount getDataCount(PixelFormat pf);
        
        /** ��ȡ�����������з�ʽ
        @version NIIEngine 5.0.0
        */
        static PixelLayoutType getDataLayout(PixelFormat pf);

        /** ��������,��ȡ��ʽ
        @param[in] markname ��ʽ����
        @param[in] direct ֱ�ӿ���ʹ�õĸ�ʽ
        @param[in] caseSensitive ��ʽ�����Ƿ����ִ�Сд
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFormat(const String & markname, Nui32 exclusionfeature = 0, bool direct = false);

        /** ��������ͼ���ʽ
        @returnλ��������ѡλ��
        @param[in] pf ԭ��ʽ.
        @param[in] intBits �������ظ���ѡλ��(����λ).0,16,32.
        @param[in] floatBits NIIf ����������ѡλ��(����λ).0,16,32.
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFormat(PixelFormat pf, Nui16 intBits, Nui16 floatBits);

        /** ���һ����ɫֵ���ڴ�
        @param[in] c ��ɫ
        @param[in] pf Ŀ�����ظ�ʽ
        @param[in] dest Ŀ���ڴ�λ��
        @version NIIEngine 3.0.0
        */
        static void packColour(const Colour & c, PixelFormat dstpf, Nui8 * dst);

        /** ת����ɫֵ���ڴ�
        @param[in] r,g,b,a ��ɫ�ɷ�
        @param[in] pf Ŀ�����ظ�ʽ
        @param[in] dest Ŀ���ڴ�λ��
        @version NIIEngine 3.0.0
        */
        static void packColour(Nui8 r, Nui8 g, Nui8 b, Nui8 a, PixelFormat dstpf, Nui8 * dst);

        /** ת����ɫֵ���ڴ�
       @param[in] r,g,b,a ��ɫ�ɷ�
       @param[in] pf Ŀ�����ظ�ʽ
       @param[in] dest Ŀ���ڴ�λ��
       @version NIIEngine 3.0.0
       */
        static void packColour(NIIf r, NIIf g, NIIf b, NIIf a, PixelFormat dstpf, Nui8 * dst);

        /** ���ڴ��н��һ����ɫֵ
        @param[in] dst ���ص���ɫ
        @param[in] pf ���ȡ����ɫ�����ظ�ʽ
        @param[in] src ��Դ�ڴ�λ��
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Colour * dst, PixelFormat srcpf, const Nui8 * src);

        /** ���ڴ��н��һ����ɫֵ
        @param[in] r,g,b,a ���ص���ɫ
        @param[in] pf ��Դ�����ظ�ʽ
        @param[in] src ��Դ�ڴ�λ��
        @remark 8λ���ȵ���ɫ�ɷ�
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Nui8 & r, Nui8 & g, Nui8 & b, Nui8 & a, PixelFormat srcpf, const Nui8 * src);

        /** ���ڴ��н��һ����ɫֵ
        @param[in] r,g,b,a ���ص���ɫ
        @param[in] pf ��Դ�����ظ�ʽ
        @param[in] src ��Դ�ڴ�λ��
        @version NIIEngine 3.0.0
        */
        static void unpackColour(NIIf & r, NIIf & g, NIIf & b, NIIf & a, PixelFormat pf, const Nui8 * src);
    };
}

#endif