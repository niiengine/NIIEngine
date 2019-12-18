/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2017-5-7

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
#ifndef _NII_ColourArea_H_
#define _NII_ColourArea_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"

namespace NII
{
    /** ͼ��ר����ɫ����
    @remark
        �Ľ���ɫ����,�м���ɫ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ColourArea : public UIAlloc
    {
    public:
        ColourArea();
        ColourArea(const Colour & c);
        ColourArea(const Colour & tl, const Colour & tr, const Colour & bl, const Colour & br);

        /** ��ֵ
        @version NIIEngine 3.0.0
        */
        void set(const Colour & c);

        /** ��ȡ
        @version NIIEngine 3.0.0
        */
        void get(Colour & c);

        /** ��ֵ
        @note ��ȸ����������?
        @version NIIEngine 3.0.0
        */
        void set(const Colour & tl, const Colour & tr, const Colour & bl, const Colour & br);

        /** ��ȡ
        @note ��ȸ����������?
        @version NIIEngine 3.0.0
        */
        void get(Colour & tl, Colour & tr, Colour & bl, Colour & br);

        /** ��ȡ����ɫ��
        @version NIIEngine 3.0.0
        */
        ColourArea get(const Rectf & rect) const;

        /** ���� +
        @version nIIEngine 3.0.0
        */
        ColourArea operator+(NIIf factor) const;

        /** ���� - 
        @version nIIEngine 3.0.0
        */
        ColourArea operator-(NIIf factor) const;

        /** ���� *
        @version nIIEngine 3.0.0
        */
        ColourArea operator*(NIIf factor) const;

        /** ���� /
        @version nIIEngine 3.0.0
        */
        ColourArea operator/(NIIf factor) const;

        /** ���� *=
        @version NIIEngine 3.0.0
        */
        ColourArea & operator*=(const ColourArea & o);

        /** ���� =
        @version NIIEngine 3.0.0
        */
        ColourArea & operator=(const ColourArea & o);
        
        /** ���� +
        @version NIIEngine 3.0.0
        */
        ColourArea operator+(const ColourArea & o) const;

        /** ���� -
        @version NIIEngine 3.0.0
        */
        ColourArea operator-(const ColourArea & o) const;

        /** ���� *
        @version NIIEngine 3.0.0
        */
        ColourArea operator*(const ColourArea & o) const;

        /** ���� /
        @version NIIEngine 3.0.0
        */
        ColourArea operator/(const ColourArea & o) const;

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        void setAlpha(NIIf a);

        /** ����͸��������
        @version NIIEngine 3.0.0
        */
        void setAlphaFactor(NIIf a);

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        void setLeftAlpha(NIIf a);

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        void setTopAlpha(NIIf a);

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        void setRightAlpha(NIIf a);

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        void setBottomAlpha(NIIf a);

        /** �Ƿ�ֻ��һ����ɫ
        @version NIIEngine 3.0.0
        */
        bool isOneColour() const;

        /** ��ȡ��λ��ɫ
        @version NIIEngine 3.0.0
        */
        Colour get(NIIf x, NIIf y) const;
    public:
        Nid mID;
        Colour mTL;
        Colour mTR;
        Colour mBL;
        Colour mBR;
    };
}
#endif