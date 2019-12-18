/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-7-29

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

#ifndef _NII_XmlAnalyzeManager_H_
#define _NII_XmlAnalyzeManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiXmlAnalyzeFactory.h"

namespace NII
{
    /** Xml����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlAnalyzeManager : public Singleton<XmlAnalyzeManager>, public ManagerAlloc
    {
    public:
        XmlAnalyzeManager(LangID lid = N_PrimaryLang);
        ~XmlAnalyzeManager();

        /** ��ӹ�����
        @version NIIEngine 3.0.0
        */
        bool addFactory(XmlAnalyzeFactory * f);

        /** ��ȥ������
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        XmlAnalyzeFactory * getFactory(FactoryID fid) const;

        /** ����xml������
        @version NIIEngine 3.0.0
        */
        XmlAnalyze * create(FactoryID fid, LangID lid);

        /** ɾ��xml������
        @version NIIEngine 3.0.0
        */
        void destroy(XmlAnalyze * obj);

        /// @copydetails Singleton::getOnly
        static XmlAnalyzeManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static XmlAnalyzeManager * getOnlyPtr();
    protected:
        typedef map<FactoryID, XmlAnalyzeFactory *>::type XmlAnalyzeFactoryList;
        XmlAnalyzeFactoryList mXmlAnalyzes;
        LangID mLangID;
    };
}
#endif