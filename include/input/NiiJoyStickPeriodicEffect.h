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

#ifndef _NII_JOYSTICK_PERIODIC_EFFECT_H_
#define _NII_JOYSTICK_PERIODIC_EFFECT_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickEffect.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickPeriodicEffect : public JoyStickEffect
    {
	public:
        JoyStickPeriodicEffect() :
			mMagnitude(0),
			mOffset(0),
			mPhase(0),
			mPeriod(0) {}

        Envelope mEnvelope;     ///< �����ɷ�
		Nui16 mMagnitude;       ///< 0 �� 10,0000
		NIIs mOffset;
		Nui16 mPhase;           ///< Position at which playback begins 0 �� 35,999
		Nui32 mPeriod;          ///< Period of effect (microseconds)
    };
}
}
#endif