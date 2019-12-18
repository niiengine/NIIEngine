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

#ifndef _NII_SHADOWVOLUME_H_
#define _NII_SHADOWVOLUME_H_

#include "NiiPreInclude.h"
#include "NiiGeometryObj.h"

namespace NII
{
    /** ��Ӱ�ݻ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShadowVolume : public GeometryObj, public ShadowAlloc
    {
        friend class ShadowVolumeObj;
    public:
        ShadowVolume();
        virtual ~ShadowVolume();

        /** �����ݻ�����
        @version NIIEngine 3.0.0
        */
        void setMaterial(Material * m);

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        ShadowVolume * getLightCap();

        /** �Ƿ�ɼ�
        @version NIIEngine 3.0.0
        */
        virtual bool isVisible() const;

        /** ����ʵ��ʹ�õ���������
        @version NIIEngine 3.0.0
        */
        virtual void setIndex(IndexBuffer * index);

        /// @see GeometryObj::getMaterial
        const Material * getMaterial() const;

        /// @see GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const;

        /// @see GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera *) const;

        /// @see GeometryObj::getLight
        void getLight(LightList & ll) const;
    protected:
        GeometryRaw mRenderOp;
        Material * mMaterial;
        ShadowVolume * mLightCap;
    };
}
#endif