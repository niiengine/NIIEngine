/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-7-1

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

#ifndef _NII_ViewPixelXmlAnalyze_H_
#define _NII_ViewPixelXmlAnalyze_H_

#include "NiiPreInclude.h"
#include "NiiXmlAnalyze.h"
#include "NiiPixelBufferProperty.h"

namespace NII
{
    /** ����Ԫ�ؼ�xml����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBufferXmlAnalyze : public XmlAnalyze, public PixelBufferProperty
    {
    public:
        PixelBufferXmlAnalyze(LangID lid = N_PrimaryLang);
        ~PixelBufferXmlAnalyze();

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;

        /** ��ȡ��ǰ������ʹ�õ�����ID
        @version NIIEngine 3.0.0
        */
        ResourceID getTextureID() const;
    protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        /** �������غ���
        @version NIIEngine 3.0.0
        */
		void PrcPixelBufferCB(const PropertyData * pdc);
    protected:
        ResourceID mTextureID;
    };
}
#endif