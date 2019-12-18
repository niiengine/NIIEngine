/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-3

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

#ifndef _NII_PLANE_SKY_H_
#define _NII_PLANE_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** �����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSky : public Sky
    {
    public:
        PlaneSky();
        PlaneSky(const SpaceManager * gm);
        virtual ~PlaneSky();

        /** ���ò���
        @param[in] rid ��ԴID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf xtiling = 10,
            NIIf ytiling = 10, NIIi xsegments = 10, NIIi ysegments = 10, NIIf bow = 0);

        /** �����Ĵ�С����
        @return ����
        @version NIIEngine 3.0.0
        */
        NIIf getScale() const;

        /** ��������������
        @return ��������
        @version NIIEngine 3.0.0
        */
        NIIf getBow() const;

        /** ��ȡ���ƽ��Ķ���
        @version NIIEngine 3.0.0
        */
        const Plane & getPlane() const;
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        Plane mPlane;
        NIIf mScale;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
        NIIf mBow;
        ResourceID mMaterial;
        ResourceID mMesh;
        GeometrySpace * mGeo;
    };
}
#endif