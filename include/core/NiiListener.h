/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-10-31

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

#ifndef _NII_LISTENER_H_
#define _NII_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI Listener : public CommandAlloc
    {
    public
        virtual ~Listener();

        /**
        @version
        */
        virtual void onNew(void * obj);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onDelete(void * obj);
    };
}
#endif