/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-1

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_PIXEL_FORMAT_H_
#define _NII_PIXEL_FORMAT_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���ظ�ʽ
    @version NIIEngine 3.0.0
    */
    enum PixelFormat
    {
        PF_UNKNOWN = 0,                 ///< ������ʽ
        PF_L8 = 1,                      ///< 8λ����
        PF_L16 = 2,                     ///< 16λ����
        PF_A8 = 3,                      ///< 8λ͸����.
        PF_A4L4 = 4,                    ///< 4λ͸����/4λ����.
        PF_L8A8 = 5,                    ///< 8λ����/8λ͸����
        PF_R8 = 6,                      ///< 8λ��
        PF_RG8 = 7,                     ///< 8λ��/��
        PF_SHORT_RGBA = 8,              ///< 16λ��/��/��/͸����
        PF_SHORT_GR = 9,                ///< 16λ��/��
        PF_SHORT_RGB = 10,              ///< 16λ��/��/��
        PF_R5G6B5 = 11,                 ///< 5λ��/6λ��/5λ��
        PF_B5G6R5 = 12,                 ///< 5λ��/6λ��/5λ��
        PF_R3G3B2 = 13,                 ///< 2λ��/3λ��/3λ��
        PF_A4R4G4B4 = 14,               ///< 4λ͸����/��/��/��
        PF_A1R5G5B5 = 15,               ///< 5λ��/��/��/1λ͸����
        PF_R8G8B8 = 16,                 ///< 8λ��/��/��
        PF_B8G8R8 = 17,                 ///< 8λ��/��/��
        PF_A8R8G8B8 = 18,               ///< 8λ͸����/��/��/��
        PF_A8B8G8R8 = 19,               ///< 8λ��/��/��/͸����
        PF_B8G8R8A8 = 20,               ///< 8λ��/��/��/͸����
        PF_R8G8B8A8 = 21,               ///< 8λ��/��/��/͸����
        PF_X8R8G8B8 = 22,               ///< 8λ��/��/��,��͸����
        PF_X8B8G8R8 = 23,               ///< 8λ��/��/��,��͸����
#if N_ENDIAN == N_ENDIAN_B
        PF_BYTE_RGB = PF_R8G8B8,        ///< 1�ֽں�/��/��
        PF_BYTE_BGR = PF_B8G8R8,        ///< 1�ֽ���/��/��
        PF_BYTE_BGRA = PF_B8G8R8A8,     ///< 1�ֽ���/��/��/͸����
        PF_BYTE_RGBA = PF_R8G8B8A8,     ///< 1�ֽں�/��/��/͸����
#else
        PF_BYTE_RGB = PF_B8G8R8,        ///< 1�ֽں�/��/��
        PF_BYTE_BGR = PF_R8G8B8,        ///< 1�ֽں�/��/��
        PF_BYTE_BGRA = PF_A8R8G8B8,     ///< 1�ֽ���/��/��/͸����
        PF_BYTE_RGBA = PF_A8B8G8R8,     ///< 1�ֽں�/��/��/͸����
#endif
        PF_A2R10G10B10 = 24,            ///< 10λ��/��/��/2λ͸����
        PF_A2B10G10R10 = 25,            ///< 10λ��/��/��/2λ͸����
        PF_FLOAT16_R = 26,              ///< 16λ��
        PF_FLOAT16_RGB = 27,            ///< 16λ��/��/��
        PF_FLOAT16_RGBA = 28,           ///< 16λ��/��/��/͸����
        PF_FLOAT32_R = 29,              ///< 32λ��
        PF_FLOAT32_RGB = 30,            ///< 32λ��/��/��
        PF_FLOAT32_RGBA = 31,           ///< 32λ��/��/��/͸����
        PF_FLOAT16_GR = 32,             ///< 16λ��/��
        PF_FLOAT32_GR = 33,             ///< 32λ��/��
        PF_DEPTH = 34,                  ///< �����ʽ
        PF_DXT1 = 35,                   ///< DirectDraw Surface��ʽ
        PF_DXT2 = 36,                   ///< DirectDraw Surface��ʽ
        PF_DXT3 = 37,                   ///< DirectDraw Surface��ʽ
        PF_DXT4 = 38,                   ///< DirectDraw Surface��ʽ
        PF_DXT5 = 39,                   ///< DirectDraw Surface��ʽ
        PF_PVRTC_RGB2 = 40,             ///< PowerVR ��ʽ
        PF_PVRTC_RGBA2 = 41,            ///< PowerVR ��ʽ
        PF_PVRTC_RGB4 = 42,             ///< PowerVR ��ʽ
        PF_PVRTC_RGBA4 = 43,            ///< PowerVR ��ʽ
        PF_COUNT = 44
    };
    typedef vector<PixelFormat>::type PixelFormatList;

    /** ���ػ�Ԫ���ݸ�ʽ
    @version NIIEngine 3.0.0
    */
    enum PixelDataType
    {
        PDT_Unknow  = 0,
        PDT_Byte    = 0,
        PDT_Short   = 1,
        PDT_UShort  = 2,
        PDT_Int     = 3,
        PDT_UInt    = 4,
        PDT_Float16 = 5,
        PDT_Float   = 6,
        PDT_Count   = 7
    };

    /** ���ظ�ʽ�ص�
    @version NIIEngine 3.0.0
    */
    enum PixelFormatFeature
    {
        PFF_None = 0,
        PFF_Float = 0x01,
        PFF_Alpha = 0x02,
        PFF_Depth = 0x04,
        PFF_Compress = 0x08,
        PFF_Grayscale = 0x20,     ///< �ҽ׸�ʽ,û����ɫ,ֻ����ڵ��׵Ĺ���
        PFF_NonEndian = 0x10      ///< һ��8λ��Ԫ�Ķ��Ƿ��ֽ����������
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
        static void conv(const PixelBlock & src, const PixelBlock & dst);

        /** ��ȡ�ڴ��С
        @note ����ѹ��
        @param[in] pf �÷�Χ�ĸ�ʽ
        @return �ֽڴ�С
        @version NIIEngine 3.0.0
        */
        static NCount getMemorySize(NCount w, NCount h, NCount d, PixelFormat pf);

        /** ��ȡ���ظ�ʽ�������
        @return ��PFF_Alpha, PFF_ISCOMPRESSED,PFF_Float, PFF_Depth, PFF_Grayscale
            �������
        */
        static Nui32 getExentMark(PixelFormat pf);

        /** ��ȡ��ʽ���Ƿ���alpha�ɷ�
        @version NIIEngine 3.0.0
        */
        static bool isAlpha(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ񸡵�
        @version NIIEngine 3.0.0
        */
        static bool isFloat(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ�ѹ��
        @version NIIEngine 3.0.0
        */
        static bool isCompress(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ����������
        @version NIIEngine 3.0.0
        */
        static bool isDepth(PixelFormat pf);

        /** ��ȡ��ʽʹ�ñ����ֽ�˳��
        @version NIIEngine 3.0.0
        */
        static bool isNativeEndian(PixelFormat pf);

        /** ��ȡ��ʽ�Ƿ�����������.
        @version NIIEngine 3.0.0
        */
        static bool isGrayscale(PixelFormat pf);

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
        @remark ����PF_UNKNOWN���������0�ֽڴ�С
        @version NIIEngine 3.0.0
        */
        static NCount getUnitSize(PixelFormat pf);

        /** ��ȡͼ���ڲ���ʶ
        @version NIIEngine 3.0.0
        */
        static String getMarkName(PixelFormat srcpf);

        /** �Ƿ�ֱ�Ӳ�����ʽ
        @return ���򷵻���,���򷵻ؼ�
        @version NIIEngine 3.0.0
        */
        static bool isDirect(PixelFormat srcpf);

        /** ��ȡ��Ԫ�����ݸ�ʽ
        @version NIIEngine 3.0.0
        */
        static PixelDataType getDataType(PixelFormat pf);

        /** ��ȡ�������ݳɷ�����
        @return���� 3(��͸����)��4(��͸����)
        @version NIIEngine 3.0.0
        */
        static NCount getDataCount(PixelFormat pf);

        /** ��������,��ȡ��ʽ
        @param[in] markname ��ʽ����
        @param[in] direct ֱ�ӿ���ʹ�õĸ�ʽ
        @param[in] caseSensitive ��ʽ�����Ƿ����ִ�Сд
        @version NIIEngine 3.0.0
        */
        static PixelFormat getFormat(const String & markname, bool direct = false, bool caseSensitive = false);

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
        @param[in] colour ���ص���ɫ
        @param[in] pf ���ȡ����ɫ�����ظ�ʽ
        @param[in] src ��Դ�ڴ�λ��
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Colour * colour, PixelFormat srcpf, const Nui8 * src);

        /** ���ڴ��н��һ����ɫֵ
        @param[in] r,g,b,a ���ص���ɫ (Ϊ byte)
        @param[in] pf ��Դ�����ظ�ʽ
        @param[in] src ��Դ�ڴ�λ��
        @remark 8λ���ȵ���ɫ�ɷ�
        @version NIIEngine 3.0.0
        */
        static void unpackColour(Nui8 & r, Nui8 & g, Nui8 & b, Nui8 & a, PixelFormat srcpf, const Nui8 * src);

        /** ���ڴ��н��һ����ɫֵ
        @param[in] r,g,b,a ���ص���ɫ (Ϊ NIIf)
        @param[in] pf ��Դ�����ظ�ʽ
        @param[in] src ��Դ�ڴ�λ��
        @version NIIEngine 3.0.0
        */
        static void unpackColour(NIIf & r, NIIf & g, NIIf & b, NIIf & a, PixelFormat pf, const Nui8 * src);
    };
}

#endif