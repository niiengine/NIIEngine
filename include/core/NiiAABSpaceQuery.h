/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-7

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

#ifndef _NII_AAB_SPACE_QUERY_H_
#define _NII_AAB_SPACE_QUERY_H_

#include "NiiPreProcess.h"
#include "NiiAreaSpaceQuery.h"
#include "NiiAABox.h"

namespace NII
{
    /** AABB�ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AABSpaceQuery : public AreaSpaceQuery
    {
    public:
        AABSpaceQuery(SpaceManager * mag);
        virtual ~AABSpaceQuery();

        /** ���������
        @version NIIEngine 3.0.0
        */
        void setBox(const AABox & area);

        /** ��ȡ�����
        @version NIIEngine 3.0.0
        */
        const AABox & getBox() const;

        /// @copydetails SpaceQuery::execute
        virtual void execute(SpaceQueryCmd * cmd);
    protected:
        AABox mAABB;
    };
}
#endif