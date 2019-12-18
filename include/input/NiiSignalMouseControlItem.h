/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-6-24

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

#ifndef _NII_SignalMOUSE_CONTROLITEM_H_
#define _NII_SignalMOUSE_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiMouseControlItem.h"
#include "NiiCommandObj.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace NII_MEDIA
{
    /** �����Ƶ�Ԫ
    @remark ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���
        �Ĵ�����������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SignalMouseControlItem : public MouseControlItem, public CommandObj
    {
    public:
        SignalMouseControlItem();

        virtual ~SignalMouseControlItem();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseLeftEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseRightEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressMiddleEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseMiddleEvent;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton3Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton4Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton5Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton6Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID PressButton7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReleaseButton7Event;
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEvent;
    public:
        /// @copydetails MouseControlItem::onPressLeft
        virtual void onPressLeft(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseLeft
        virtual void onReleaseLeft(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressRight
        virtual void onPressRight(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseRight
        virtual void onReleaseRight(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressMiddle
        virtual void onPressMiddle(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseMiddle
        virtual void onReleaseMiddle(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton3
        virtual void onPressButton3(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton3
        virtual void onReleaseButton3(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton4
        virtual void onPressButton4(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton4
        virtual void onReleaseButton4(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton5
        virtual void onPressButton5(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton5
        virtual void onReleaseButton5(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton6
        virtual void onPressButton6(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton6
        virtual void onReleaseButton6(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onPressButton7
        virtual void onPressButton7(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onReleaseButton7
        virtual void onReleaseButton7(const MousePressArgs * arg);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MouseMoveArgs * arg);
    };
}
}
#endif