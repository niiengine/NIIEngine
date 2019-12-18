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

#ifndef _NII_AREA_SPACEQUERY_H_
#define _NII_AREA_SPACEQUERY_H_

#include "NiiPreInclude.h"
#include "NiiSpaceQuery.h"
#include "NiiSpaceQueryCmd.h"

namespace NII
{
    /** ����ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AreaSpaceQuery : public SpaceQuery, public SpaceQueryCmd
    {
    public:
        AreaSpaceQuery(SpaceManager * mag);
        virtual ~AreaSpaceQuery();

        using SpaceQuery::execute;

        /// @copydetails SpaceQuery::execute
        void execute();

        /// @copydetails SpaceQueryCmd::onQuery
        bool onQuery(const QueryDetail & result);
    };
}
#endif