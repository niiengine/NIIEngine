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

#ifndef _NII_JOYSTICK_CONTROL_SLIDER_ARGS_H_
#define _NII_JOYSTICK_CONTROL_SLIDER_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiJoyDevControlArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlSliderArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlSliderArgs(Nui8 s, const Vector2<NIIi> & data) :
			JoyDevControlArgs(JDCT_Slider),
			mSlider(s),
			mData(data) {}

        const Vector2<NIIi> & mData;        ///< ����������
        Nui8 mSlider;                       ///< slider����
    };
}
}
#endif