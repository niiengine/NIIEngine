/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-7

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

#ifndef _NII_PIXEL_BLOCK_H_
#define _NII_PIXEL_BLOCK_H_

#include "NiiPreInclude.h"
#include "NiiPixelFormat.h"
#include "NiiBox.h"
namespace NII
{
    /** ���ؿ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBlock : public Box, public BufferAlloc
    {
    public:
        /** ���ع���
        @version NIIEngine 3.0.0
        */
        enum Filter
        {
            F_Nearest,
            F_Linear,
            F_Bilinear
        };
    public:
        PixelBlock();
        PixelBlock(const Box & area, PixelFormat pf, Nui8 * data = 0);
        PixelBlock(NCount w, NCount h, NCount d, PixelFormat pf, Nui8 * data = 0);

        ~PixelBlock();

        /** ��ƫ��
        @version NIIEngine 3.0.0
        */
        NCount getRowOft() const;

        /** Ƭƫ��
        @version NIIEngine 3.0.0
        */
        NCount getSliceOft() const;

        /** �Ƿ��������洢
        @version NIIEngine 3.0.0
        */
        bool isConsecutive() const;

        /** ��ȡ�����洢ʱ�Ĵ�С
        @version NIIEngine 3.0.0
        */
        NCount getConsecutiveSize() const;

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

        /** ת��
        @note ���ظ�ʽ����src,�ߴ�����dst,dst������ź�Ļ���(����ԭ���Ļ���)
        @param[in] src Դ
        @param[in] dst Ŀ��
        @version NIIEngine 3.0.0
        */
        static void scale(const PixelBlock & src, const PixelBlock & dst,
            Filter mode = F_Bilinear);

        /** ����٤��
        @param[in] size ���ݴ�С
        @param[in] bpp λ��
        @version NIIEngine 3.0.0
        */
        static void applyGamma(Nui8 * data, NIIf gamma, NCount size, NCount bpp);
    public:
        Nui8 * mData;
        PixelFormat mFormat;
        NCount mRowPitch;
        NCount mSlicePitch;
    };
}
#endif