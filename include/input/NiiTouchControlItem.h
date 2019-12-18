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

#ifndef _NII_TOUCH_CONTROL_ITEM_H_
#define _NII_TOUCH_CONTROL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlItem.h"
#include "NiiTouchControlArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TouchControlItem : public ControlItem
    {
    public:
		TouchControlItem();
        
        /** ������ʱ�򴥷�
        @param[in] args �����¼�����
        */
        virtual void onPress(const TouchControlArgs * args);
        
        /** �������ͷ�ʱ����
        @param[in] args �����¼�����
        */
        virtual void onRelease(const TouchControlArgs * args);
        
        /** �����º��ƶ�ʱ����
        @param[in] args �����¼�����
        */
        virtual void onMove(const TouchControlArgs * args);
        
        /** ��ȡ��ʱ����
        @param[in] args �����¼�����
        */
        virtual void onCancel(const TouchControlArgs * args);

		/// @copydetails ControlItem::getType
		ControlDevType getType() const;
    protected:
        /// @copydetails ControlItem::raise
        void raise(const EventArgs * arg);
    };
    
    /// ����
    class DummyMultiTouchControlItem : public TouchControlItem, public ControlAlloc
    {
    public:
        DummyMultiTouchControlItem(){}
        ~DummyMultiTouchControlItem(){}
    };    
}
}
#endif