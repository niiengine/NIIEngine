/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-4

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

#ifndef _NII_DOME_SKY_H_
#define _NII_DOME_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DomeSky : public Sky
    {
    public:
        DomeSky();
        DomeSky(const SpaceManager * gm);
        virtual ~DomeSky();

        /** ���ò���
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf cuvfactor = 10, NIIf range = 1000, NIIf xtiling = 20,
            NIIf ytiling = 20, NIIi xsegments = 20, NIIi ysegments = 20,
                const Quaternion & ori_oft = Quaternion::IDENTITY);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);

        /** ����һ����
        @version NIIEngine 3.0.0
        */
        Mesh * createPlaneImpl(ResourceID & rid, GroupID gid, PlaneType bp,
            NIIf cuvfactor, NIIf range, const Quaternion & ori_oft,
                NIIf xtiling, NIIf ytiling, NIIi xsegments, NIIi ysegments) const;
    protected:
        SpaceNode * mNode;
        GeometrySpace * mGeo[5];
        ResourceID mResource[5];
        ResourceID mMaterial;
        NIIf mRange;
        NIIf mCurveFactor;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
    };
}
#endif