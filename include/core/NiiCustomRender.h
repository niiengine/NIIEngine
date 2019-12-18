/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-2-27

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

#ifndef _NII_CUSTOM_RENDER_H_
#define _NII_CUSTOM_RENDER_H_

#include "NiiPreInclude.h"
#include "NiiCustomQueue.h"

namespace NII
{
    /** �Զ�����Ⱦ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomRender : public RenderAlloc
    {
    public:
        CustomRender(Nid id);
        virtual ~CustomRender();

        /** ����Զ������
        @note �����ڴ潫����������
        @version NIIEngine 3.0.0
        */
        void add(CustomQueue * obj);

        /** ����Զ������
        @version NIIEngine 3.0.0
        */
        CustomQueue * add(GroupID qid, const String & name);

        /** ��ȡ�Զ������
        @version NIIEngine 3.0.0
        */
        CustomQueue * get(Nidx index);

        /** ��ȥ�Զ������
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** ��ȥ�����Զ������
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        Nid getID() const;

        /** ��ȡ�Զ������
        @version NIIEngine 3.0.0 �߼�api
        */
        const CustomQueueList & getList();
    protected:
        Nid mID;
        CustomQueueList mList;
    };
}
#endif