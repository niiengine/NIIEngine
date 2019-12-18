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

#ifndef _NII_BOX_SKY_H_
#define _NII_BOX_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** �����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BoxSky : public Sky
    {
    public:
        BoxSky();
        BoxSky(const SpaceManager * gm);
        virtual ~BoxSky();
        
        /** ���ò���
        @param[in] rid ��ԴID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);
        
        /** ���÷���ƫ��
        @version NIIEngine 3.0.0
        */
        void setOriOft(const Quaternion & o);
        
        /** ��ȡ����ƫ��
        @version NIIEngine 3.0.0
        */
        const Quaternion & getOriOft() const;
        
        /** ��������
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf distance, const Quaternion & ori_oft);
    
        /** ��ȡ���ӿڵľ���
        @version NIIEngine 3.0.0
        */
        NIIf getRange() const;
    protected:
        /// @copydetails BoxSky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        NIIf mRange;
        CustomSpaceObj * mGeo;
        Quaternion mOriOft;
        ResourceID mMaterial[7];
    };
}
#endif