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

#ifndef _NII_JOYHANDLE_CONTROL_ITEM_H_
#define _NII_JOYHANDLE_CONTROL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickControlItem.h"
#include "NiiJoyDevControlArgs.h"
#include "NiiJoyPadControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ��Ϸ�ְ����Ϳ�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyHandleControlItem : public JoyStickControlItem
    {
    public:
        JoyHandleControlItem() {}
        virtual ~JoyHandleControlItem() {}
        
        ///@copydetails ControlItem::getType
        ControlDevType getType() const
        {
            return CDT_JoyHandle;
        }
    public:
        /** ��ҡ���е��������ʱ����
        @param[in] args ҡ����������¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onHandleVector(const JoyHandleControlVectorArgs * args) {}
    };

    class _EngineAPI DummyJoyHandleControlItem : public JoyHandleControlItem, public ControlAlloc
    {
    public:
        DummyJoyHandleControlItem() {}
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListenerJoyHandleControlItem : public JoyHandleControlItem, public ControlAlloc
    {
    public:
        ListenerJoyHandleControlItem() {};
        virtual ~ListenerJoyHandleControlItem() {};
    };

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