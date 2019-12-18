/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_FontGlyph_H_
#define _NII_FontGlyph_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontGlyph : public UIAlloc
    {
    public:
        FontGlyph(){}
        FontGlyph(const Rectf & coord, const Vector2f & oft, NIIf extent) :
            mTexCoord(coord),
            mPixelOffset(oft),
            mHoriAdvance(extent){}

        /** ������������
        @version NIIEngine 3.0.0 �߼� api
        */
        void setTextureCoord(const Rectf & coord)
        {
            mTexCoord = coord;
        }

        /** ��ȡ��������
        @version NIIEngine 3.0.0 �߼� api
        */
        const Rectf & getTextureCoord() const
        {
            return mTexCoord;
        }

        /** ����������ʾλ��ƫ��
        @version NIIEngine 3.0.0 �߼� api
        */
        void setOffset(const Vector2f & oft)
        {
            mPixelOffset = oft;
        }

        /** ��ȡ������ʾλ��ƫ��
        @version NIIEngine 3.0.0 �߼� api
        */
        const Vector2f & getOffset() const
        {
            return mPixelOffset;
        }

        /** ����ˮƽƫ��
        @version NIIEngine 3.0.0 �߼� api
        */
        void setHoriAdvance(NIIf adv)
        {
            mHoriAdvance = adv;
        }

        /** ��ȡˮƽƫ��
        @version NIIEngine 3.0.0 �߼� api
        */
        NIIf getHoriAdvance() const
        {
            return mHoriAdvance;
        }
    protected:
        Rectf mTexCoord;
        Vector2f mPixelOffset;
        NIIf mHoriAdvance;
    };
    typedef vector<FontGlyph *>::type FontGlyphList;
}
#endif