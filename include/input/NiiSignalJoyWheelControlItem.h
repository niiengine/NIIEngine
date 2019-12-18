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

#ifndef _NII_SIGNAL_JoyWheel_CONTROLITEM_H_
#define _NII_SIGNAL_JoyWheel_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiJoyWheelControlItem.h"
#include "NiiSignalJoyPadControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** �¼��� JoyWheelControlItem
    @remark 
        ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���Ĵ���
        ��������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalJoyWheelControlItem : public JoyWheelControlItem,
		public SignalJoyPadControlItem
    {
    public:
        SignalJoyWheelControlItem();
        virtual ~SignalJoyWheelControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID WheelEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /// @copydetails SignalJoyWheelControlItem::onSlipSlider
        virtual void onWheel(const JoyWheelControlArgs * args);
    };
}
}
#endif