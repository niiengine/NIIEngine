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

#ifndef _NII_ParticleEffect_H_
#define _NII_ParticleEffect_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmdObj.h"
#include "NiiString.h"
#include "NiiParticle.h"
#include "NiiVector3.h"
#include "NiiVector2.h"

namespace NII
{
    /** ����Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleEffect : public PropertyCmdObj, public ParticleAlloc
    {
    public:
        ParticleEffect(ParticleSpace * space, LangID lid = N_PrimaryLang);
        virtual ~ParticleEffect();

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        const String & getType() const;

        /** ��ʼ������
        @version NIIEngine 3.0.0
        */
        virtual void initImpl(Particle * obj);

        /** Ӧ������Ч��(����)
        @version NIIEngine 3.0.0
        */
        virtual void effectImpl(Particle * obj, TimeDurMS cost);
        
        /** Ӧ������Ч��(��Ⱥ)
        @version NIIEngine 3.0.0
        */
        virtual void effectImpl(ParticleList & list, TimeDurMS cost);
    protected:
        /** λ�ñ仯ʱ
        @version NIIEngine 3.0.0
        */
        virtual void onPos(const Vector3f & o);
        
        /** ��С�仯ʱ
        @version NIIEngine 3.0.0
        */
        virtual void onSize(const Vector2f & s);
        
        /** ��ת�仯ʱ
        @version NIIEngine 3.0.0
        */
        virtual void onRotation(const Radian & r);
    protected:
        ParticleSpace * mParent;
        String mType;
    };
}

#endif