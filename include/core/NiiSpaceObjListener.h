/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-10-28

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

#ifndef _NII_SPACEOBJ_LISTENER_H_
#define _NII_SPACEOBJ_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �¼�����
    @remark ��������������/��Ӱ��Ⱦ������
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI SpaceObjListener
    {
    public:
        SpaceObjListener();
        virtual ~SpaceObjListener();

        /** ������ʱ
        @version NIIEngine 3.0.0
        */
        virtual void create(SpaceObj * obj);
        
        /** ɾ��ʱ
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpaceObj * obj);
        
        /** λ�÷����ı�
        @version NIIEngine 3.0.0
        */
        virtual void notify(SpaceObj * obj);

        /** ���ӵ��ڵ�ʱ
        @note ���ڲ���2��0ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void attach(SpaceObj * obj, const PosNode * np);

        /** �ӽڵ��з���ʱ
        @note ���ڲ���2��0ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void detach(SpaceObj * obj, const PosNode * op);

        /** ʹ��ָ����������ƿռ�
        @return �Ƿ���ʵ�ʻ���Ч��,û��Ч�������
        @version NIIEngine 3.0.0
        */
        virtual bool render(SpaceObj * obj, const Camera * view);

        /** ��ѯʵ��Ӱ��ռ����ĵƹ�
        @version NIIEngine 3.0.0
        */
        virtual void query(const SpaceObj * obj, LightList & out);
    };
}

#endif