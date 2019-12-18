/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

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

#ifndef _NII_JOY_STICK_FORCE_FEEDBACK_H_
#define _NII_JOY_STICK_FORCE_FEEDBACK_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickEffect.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevForceFeedback : public PatternAlloc
    {
    public:
		typedef multimap<JoyStickEffect::ForceEffectType, JoyStickEffect::ForceEffectFactorType>::type SupportedEffectList;
    public:
        JoyDevForceFeedback();
        virtual ~JoyDevForceFeedback();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void _setGainSupport(bool set);

        /**
        @remark
        @version NIIEngine 3.0.0
        */
		void _setAutoCenterSupport(bool set);

		/**
		@remarks
		@param level
        @version NIIEngine 3.0.0
		*/
		virtual void setMasterGain(float level) = 0;

		/**
		@remarks
		@param set
        @version NIIEngine 3.0.0
		*/
		virtual void setAutoCenterMode(bool set) = 0;

		/**
		@remarks
        @version NIIEngine 3.0.0
		*/
		virtual void upload(const JoyStickEffect * effect) = 0;

		/**
		@remarks 
        @version NIIEngine 3.0.0
		*/
		virtual void modify(const JoyStickEffect * effect) = 0;

		/**
		@remarks 
        @version NIIEngine 3.0.0
		*/
		virtual void remove(const JoyStickEffect * effect) = 0;

		/**
		@remarks 
        @version NIIEngine 3.0.0
		*/
        virtual short getFFAxesNumber() = 0;

		/**
		@remarks 
        @version NIIEngine 3.0.0
		*/
		virtual unsigned short getFFMemoryLoad() = 0;
        
		/**
		@remarks
        @version NIIEngine 3.0.0
		*/
		const SupportedEffectList& getSupportedEffects() const;

		/**
		@remarks
        @version NIIEngine 3.0.0
		*/
		bool supportsEffect(JoyStickEffect::ForceEffectType force,
            JoyStickEffect::ForceEffectFactorType type) const;

		void _addEffectTypes(JoyStickEffect::ForceEffectType force,
            JoyStickEffect::ForceEffectFactorType type);
    protected:
		SupportedEffectList mSupportedEffects;
		bool mSetGainSupport;
		bool mSetAutoCenterSupport;
    };
}
}
#endif
