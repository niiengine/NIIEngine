/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-5-7

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

#ifndef _NII_IntersectSpaceQuery_H_
#define _NII_IntersectSpaceQuery_H_

#include "NiiPreInclude.h"
#include "NiiAreaSpaceQuery.h"

namespace NII
{
    /** �ཻ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IntersectSpaceQuery : public AreaSpaceQuery
    {
    public:
        IntersectSpaceQuery(SpaceManager * mag);
        virtual ~IntersectSpaceQuery();

        /// @copydetails SpaceQuery::execute
        void execute(SpaceQueryCmd * cmd);
    };
}

#endif