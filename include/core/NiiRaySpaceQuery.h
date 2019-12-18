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

#ifndef _NII_RAY_SPACEQUERY_H_
#define _NII_RAY_SPACEQUERY_H_

#include "NiiPreInclude.h"
#include "NiiAreaSpaceQuery.h"
#include "NiiRay.h"

namespace NII
{
    /** ���߲�ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RaySpaceQuery : public AreaSpaceQuery
    {
    public:
        RaySpaceQuery(SpaceManager * mag);
        virtual ~RaySpaceQuery();

        /** ���ò�ѯ����
        @version NIIEngine 3.0.0
        */
        virtual void setRay(const Ray & ray);

        /** ��ȡ��ѯ����
        @version NIIEngine 3.0.0
        */
        virtual const Ray & getRay() const;

        /** �����Ƿ��������
        @version NIIEngine 3.0.0
        */
        void setSortDistance(bool set);

        /** �Ƿ��������
        @version NIIEngine 3.0.0
        */
        bool isSortDistance() const;

        /// @copydetails SpaceQueryCmd::execute
        virtual void execute();
        
        /// @copydetails SpaceQuery::execute
        virtual void execute(SpaceQueryCmd * cmd);
    protected:
        Ray mRay;
        bool mSortDistance;
    };
}

#endif