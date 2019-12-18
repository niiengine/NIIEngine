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

#ifndef _NII_FONT_XMLAnalyze_H_
#define _NII_FONT_XMLAnalyze_H_

#include "NiiPreInclude.h"
#include "NiiXmlAnalyze.h"
#include "NiiFont.h"
#include "NiiString.h"

namespace NII
{
    /** ����Ԫ��xml����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontXmlAnalyze : public FontProperty, public XmlAnalyze
    {
    public:
        FontXmlAnalyze(LangID lid = N_PrimaryLang, FontType type = FT_TrueType);
        ~FontXmlAnalyze();

        /** ��ȡ�����������
        @version NIIEngine 3.0.0
        */
        const FontList & getObjectList() const;

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;

        /** �����Xml����
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out, const Font * obj) const;
    protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        /** �����ַ�����
        @version NIIEngine 3.0.0
        */
        void PrcDefineCB(const PropertyData * pdc);

		void writeTrueType(XmlSerializer * out, const Font * src) const;

		void writePixel(XmlSerializer * out, const Font * src) const;
    private:
        Font * mCurrentFont;
        FontList mFontList;
		FontType mFontType;
    };
}
#endif