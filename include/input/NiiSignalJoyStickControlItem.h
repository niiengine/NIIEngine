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

#ifndef _NII_SIGNAL_JOYSTICK_CONTROLITEM_H_
#define _NII_SIGNAL_JOYSTICK_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickControlItem.h"
#include "NiiSignalJoyPadControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** �¼��� JoyStickControlItem
    @remark 
        ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���Ĵ���
        ��������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalJoyStickControlItem : public JoyStickControlItem,
		public SignalJoyPadControlItem
    {
    public:
        SignalJoyStickControlItem();
        virtual ~SignalJoyStickControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SlipSliderEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PushPOVEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /// @copydetails SignalJoyPadControlItem::onSlipSlider
        virtual void onSlipSlider(const JoyStickControlSliderArgs * args);

        /// @copydetails SignalJoyPadControlItem::onPushPOV
        virtual void onPushPOV(const JoyStickControlPovArgs * args);
    };
}
}
#endif