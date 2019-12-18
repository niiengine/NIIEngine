/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-29

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

#ifndef _NII_XML_Analyze_H_
#define _NII_XML_Analyze_H_

#include "NiiPreInclude.h"
#include "NiiScriptProperty.h"

namespace NII
{        
	typedef void (XmlAnalyze::*PrcCB)(const PropertyData * pdc);
    /** xml�ɷ���
    @remark �����԰汾���Խڵ�,<?xml version="1.0" encoding="UTF-8"?>,����xml���ı������
        ������ʹ�ú�������ͬ��utf-8.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyze : public ScriptProperty, public ScriptAlloc
    {
    public:
        typedef map<PropertyID, PrcCB>::type PrcCBMap;
    public:
        XmlAnalyze(VersionID version, ScriptTypeID stid, LangID lid = N_PrimaryLang);
        virtual ~XmlAnalyze();

        /** �����Ƿ񲻴�������(��)�ɷ���
        @note Ĭ�ϴ��������ɷ���
        @version NIIEngine 3.0.0
        */
        void setSkipOther(bool b);

        /** ��ȡ�Ƿ񲻴��������ɷ���
        @note Ĭ�ϴ��������ɷ���
        @version NIIEngine 3.0.0
        */
        bool isSkipOther() const;

        /** ���ÿ�ʼ������
        @param[in] pid ����
        @param[in] prc ����ص�
        @version NIIEngine 3.0.0
        */
        void setBeginPrc(PropertyID pid, PrcCB prc);

        /** �����ʼ������
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void unBeginPrc(PropertyID pid);

        /** ���ý���������
        @param[in] pid ����
        @param[in] prc ����ص�
        @version NIIEngine 3.0.0
        */
        void setEndPrc(PropertyID pid, PrcCB prc);

        /** �������������
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void unEndPrc(PropertyID pid);

        /** �����ǩ
        @param[in] own ����ǩ
        @param[in] pid ��ǩ����
        @param[in] pdc ��ǩ���ݳɷ�
        @version NIIEngine 3.0.0
        */
        virtual void onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** �����ǩ
        @param[in] own ����ǩ
        @param[in] pid ����ID
        @version NIIEngine 3.0.0
        */
        virtual void onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** ��������
        @param data ����ֵ
        @version NIIEngine 3.0.0
        */
        virtual void onData(const String & data);

        /** ��������
        @param data ����ֵ
        @version NIIEngine 3.0.0
        */
        virtual void onData(const Nutf8 * data);

        /** ��ȡXml���Ͷ����ļ�
        @return �ļ�(�����ļ�����չ��)
        @version NIIEngine 3.0.0
        */
        virtual const String & getXSD() const;

        /** ��ȡXml�ɷ�ID
        @note �������ʵ��
        @version NIIEngine 3.0.0
        */
        virtual XmlUnitID getUnit() const = 0;

        /** ��ȡȺ��ID
        @note �������ʵ��
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup() const = 0;
    protected:
        /** �����ڿ�ʼ
        @version NIIEngine 3.0.0
        */
        virtual void PrcTagUnitCB(const PropertyData * pdc) = 0;

        /** �����ڽ���
        @version NIIEngine 3.0.0
        */
        virtual void PrcTagUnitECB(const PropertyData * pdc);
    protected:
        PrcCBMap mBeginMap;
        PrcCBMap mEndMap;
        XmlAnalyze * mTag;
        VersionID mVersion;
        bool mAutoTag;
        bool mSkipOther;
    };
}
#endif