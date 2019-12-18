/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-7

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

#ifndef _NII_ADDITIVE_SHADOWMAPPING_PATTERN_H
#define _NII_ADDITIVE_SHADOWMAPPING_PATTERN_H

#include "NiiPreInclude.h"
#include "NiiShadowMappingPattern.h"

namespace NII
{
    class AdditiveShadowMappingPattern : public ShadowMappingPattern
    {
    public:
        AdditiveShadowMappingPattern(SpaceManager * parent, RenderSys * rsys, SysSyncParam * param);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderQueue * queue);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderGroup * group);

        /// @copydetails Pattern::getQuality
		Nui8 getQuality() const { return 0; }

        /// @copydetails Pattern::getPerformance
		Nui8 getPerformance() const { return 0; }
    protected:
        /// @copydetails RenderPattern::renderImpl
        void renderImpl(RenderGroup * rg);

        /// @copydetails RenderPattern::deriveReceiver
		ShaderCh * deriveReceiver(const ShaderCh * src);

        /// @copydetails RenderPattern::setTextureFade
        void setTextureFade(NIIf begin, NIIf end);

        /// @copydetails ShadowMappingPattern::setTextureAmbient
        void setTextureAmbient();

        /// @copydetails RenderPattern::setColourBlend
        void setColourBlend(ShaderChTextureUnit * src);

        /// @copydetails Pattern::_init
		bool _init() { return true; }

        /// @copydetails Pattern::_update
        void _update() {}

        /// @copydetails Pattern::_end
        bool _end(){ return true; }

        /// @copydetails Pattern::getPID
        PatternID getPID() const { return 0; }
    };
}
#endif