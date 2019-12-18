/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-8

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

#ifndef _NII_JOYSTICK_CONDITIONAL_EFFECT_H_
#define _NII_JOYSTICK_CONDITIONAL_EFFECT_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickEffect.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickConditionalEffect : public JoyStickEffect
    {
	public:
        JoyStickConditionalEffect() :
			mRightCoeff(0),
			mLeftCoeff(0),
			mRightSaturation(0),
			mLeftSaturation(0),
			mDeadband(0) {}

		NIIs mRightCoeff;
		NIIs mLeftCoeff;
		Nui16 mRightSaturation;
		Nui16 mLeftSaturation;
		Nui16 mDeadband;
    };
}
}
#endif