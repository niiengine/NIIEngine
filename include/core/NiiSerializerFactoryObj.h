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

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI SerializerFactoryObj
    {
    public:
        SerializerFactoryObj();
        virtual ~SerializerFactoryObj();

        /** ��ȡ����ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual Serializer * create(const DataStream * src) = 0;

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual Serializer * create(const FileSerializer * src) = 0;

        /** ɾ������
        @version NIIEngine 3.0.0
        */
        virtual void destroy(Serializer * obj) = 0;
    };
}