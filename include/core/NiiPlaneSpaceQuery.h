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

#ifndef _NII_PLANE_SPACE_QUERY_H_
#define _NII_PLANE_SPACE_QUERY_H_

#include "NiiPreInclude.h"
#include "NiiAreaSpaceQuery.h"
#include "NiiPlaneSpace.h"

namespace NII
{
    /** Plane�ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSpaceQuery : public AreaSpaceQuery
    {
    public:
        PlaneSpaceQuery(SpaceManager * mag);
        virtual ~PlaneSpaceQuery();

        /** ���ÿռ�
        @version NIIEngine 3.0.0
        */
        void setSpaceList(const PlaneSpaceList & list);

        /** ��ȡ�ռ�
        @version NIIEngine 3.0.0
        */
        const PlaneSpaceList & getSpaceList() const;
        
        /// @copydetails SpaceQuery::execute
        void execute(SpaceQueryCmd * cmd);
    protected:
        PlaneSpaceList mSpaceList;
    };
}

#endif