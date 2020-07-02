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
        inline NCount getRowOft() const{ return mRowPitch - getWidth(); }

        /** Ƭƫ��
        @version NIIEngine 3.0.0
        */
        inline NCount getSliceOft() const{ return mSlicePitch - (getHeight() * mRowPitch); }

        /** �Ƿ��������洢
        @version NIIEngine 3.0.0
        */
        bool isConsecutive() const;

        /** ��ȡ�����洢ʱ�Ĵ�С
        @version NIIEngine 3.0.0
        */
        NCount getConsecutiveSize() const;

        /** ��ȡͷ��ָ��
        @version NIIEngine 3.0.0
        */
        Nui8 * getTopLeftFrontPixelPtr() const;

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
        static void scale(const PixelBlock & src, const PixelBlock & dst, Filter mode = F_Bilinear);

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