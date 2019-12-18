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

#ifndef _NII_SIGNAL_JOYHANDLE_CONTROL_ITEM_H_
#define _NII_SIGNAL_JOYHANDLE_CONTROL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiJoyHandleControlItem.h"
#include "NiiSignalJoyPadControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ��Ϸ�ְ����Ϳ�����
    @remark 
        ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���Ĵ���
        ��������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalJoyHandleControlItem : public JoyHandleControlItem,
		public SignalJoyPadControlItem
    {
    public:
		SignalJoyHandleControlItem();
        virtual ~SignalJoyHandleControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID HandleVectorEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    public:
        /** ��ҡ���е��������ʱ����
        @param[in] args ҡ����������¼�����
        */
        virtual void onHandleVector(const JoyHandleControlVectorArgs * args);
    };
}
}

#endif