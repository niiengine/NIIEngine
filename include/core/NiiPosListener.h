/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-2

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

#ifndef _NII_PosListener_H_
#define _NII_PosListener_H_

#include "NiiPreProcess.h"

namespace NII
{
    /** λ�ýڼ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosListener
    {
    public:
        PosListener();
        virtual ~PosListener();

        /** ����ʱ
        @version NIIEngine 3.0.0
        */
        virtual void create(PosNode * o);

        /** ɾ��ʱ
        @version NIIEngine 3.0.0
        */
        virtual void destroy(PosNode * o);

        /** ����Ӷ���ʱ
        @version NIIEngine 3.0.0
        */
        virtual void add(PosNode * obj, PosNode * child);

        /** ��ȥ�Ӷ���ʱ
        @version NIIEngine 3.0.0
        */
        virtual void remove(PosNode * o, PosNode * child);

        /** ���ӵ���������ʱ
        @version NIIEngine 3.0.0
        */
        virtual void attach(PosNode * o);

        /** ������������ʱ
        @version NIIEngine 3.0.0
        */
        virtual void detach(PosNode * o);

        /** ����ʱ
        @version NIIEngine 3.0.0
        */
        virtual void update(PosNode * o);
    };
}
#endif