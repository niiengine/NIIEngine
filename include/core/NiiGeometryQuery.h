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

#ifndef _NII_GEOMETRY_QUERY_H_
#define _NII_GEOMETRY_QUERY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���β�ѯ��
    @remark �������Ա����ѯ���
    @version NIIEngine 3.0.0 �߼�api
    */
    class GeometryQuery
    {
    public:
        GeometryQuery();
        virtual ~GeometryQuery();

        /** ����ģ�͵���ģʽ
        @version NIIEngine 3.0.0
        */
        void setDebugMode(bool b);
        
        /** ��ȡ����ģʽģʽ
        @version NIIEngine 3.0.0
        */
        bool isDebugMode() const;
        
        /** ִ�в�ѯ
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void query(GeometryObj * obj) = 0;
    protected:
        bool mDebug;
    };
}
#endif