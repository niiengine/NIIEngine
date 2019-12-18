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

#ifndef _NII_JoyWheel_CONTROLITEM_H_
#define _NII_JoyWheel_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiJoyPadControlItem.h"
#include "NiiJoyWheelControlArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ҡ�˿��Ƶ�Ԫ
    @remark
        ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���Ĵ���
        ��������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyWheelControlItem : public JoyPadControlItem
    {
    public:
		JoyWheelControlItem() {}
		virtual ~JoyWheelControlItem() {}

        ///@copydetails ControlItem::getType
        ControlDevType getType() const
		{
			return CDT_JoyWheel;
		}
    public:
        /** ��ҡ���еĻ����ƶ�ʱ����
        @param[in] args ҡ�˰����¼�����
        @version NIIEngine 3.0.0
        */
		virtual void onWheel(const JoyWheelControlArgs * args) {}
    };

	class _EngineAPI DummyJoyWheelControlItem : public JoyWheelControlItem, public ControlAlloc
	{
	public:
		DummyJoyWheelControlItem() {}
	};
}
}
#endif