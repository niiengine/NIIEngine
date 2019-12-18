/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-6-29

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

#ifndef _NII_FusionDataCollector_H_
#define _NII_FusionDataCollector_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����������ݼ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionDataCollector
    {
    public:
        FusionDataCollector();
        virtual ~FusionDataCollector();

        /** �����������
        @version NIIEngine 3.0.0
        */
        virtual FusionData * createFusionData(Nid id);

        /** ��ȡ��ϼ�������
        @version NIIEngine 3.0.0
        */
        virtual TypeID getFusionCollectorType() const;
    protected:
        /** ������������б�
        @version NIIEngine 3.0.0
        */
        void setupFusionData() const;

        /** ��ʼ����������б�
        @version NIIEngine 3.0.0
        */
        virtual void initFusionCollector(IDList * ids) const = 0;
    protected:
        typedef map<TypeID, IDList>::type FusionCollectorList;
        static FusionCollectorList mFusionCollectorList;
    };
}
#endif