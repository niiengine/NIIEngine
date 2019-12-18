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

#ifndef _NII_SPHERE_SPACE_QUERY_H_
#define _NII_SPHERE_SPACE_QUERY_H_

#include "NiiPreInclude.h"
#include "NiiAreaSpaceQuery.h"
#include "NiiSphere.h"

namespace NII
{
    /** Sphere�ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SphereSpaceQuery : public AreaSpaceQuery
    {
    public:
        SphereSpaceQuery(SpaceManager * mag);
        virtual ~SphereSpaceQuery();

        /** ����������.
        @version NIIEngine 3.0.0
        */
        void setSphere(const Sphere & area);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        const Sphere & getSphere() const;
        
        /// @copydetails SpaceQuery::execute
        virtual void execute(SpaceQueryCmd * cmd);
    protected:
        Sphere mSphere;
    };
}
#endif