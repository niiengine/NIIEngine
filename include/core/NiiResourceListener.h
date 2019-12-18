/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-23

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

#ifndef _NII_RESOURCE_LISTENER_H_
#define _NII_RESOURCE_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��Դ����
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI ResourceListener
    {
    public:
        ResourceListener();
        virtual ~ResourceListener();

        /** ��Դ���ز��Բ���󴥷�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onPlanEnd(Resource * r);

        /** ��������󴥷�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onLoadEnd(Resource * r);

        /** ��Դת��������ɺ󴥷�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onResultEnd(Resource * r);

        /** ����Դж����ɺ󴥷�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onUnloadEnd(Resource * r);

        /** �����ز�������ʱ����
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onLoadScheme(Resource * r, LoadScheme * s, bool enable);

        /** �������������ʱ����
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onResultScheme(Resource * r, ResultScheme * s, bool enable);
    };
}

#endif