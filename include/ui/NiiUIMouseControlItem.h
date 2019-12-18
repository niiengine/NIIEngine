/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

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

#ifndef _NII_UI_MOUSE_CONTROL_ITEM_H_
#define _NII_UI_MOUSE_CONTROL_ITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiMouseControlItem.h"

using namespace NII::NII_MEDIA;

namespace NII
{
namespace UI
{
    /** UI��Ԫר�õ������Ƶ�Ԫ
    @remark
        ��UI��ϵ�о����ѻ���ת�����������ʹ��,�ܻ�ȡ����Ĺ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlItem : public NII_MEDIA::MouseControlItem, public PropertyCmdObj
    {
    public:
        MouseControlItem(LangID lid = N_PrimaryLang);
        virtual ~MouseControlItem();

        /** Ӧ�õ���Ԫ
        @version NIIEngine 3.0.0
        */
        void apply(Widget * dst);

        /** ����ʱ��״̬
        @param[in] cost
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** ���������Զ��¼�������Ҫ��ʱ�䷧
        @param delay ��λ��
        @return Nothing.
        */
        void setThreshold(TimeDurMS delay);

        /** �������Զ��¼�������Ҫ��ʱ�䷧
        @return ��λ��
        */
        TimeDurMS getThreshold() const;

        /** �����Զ��¼�������ɺ�,�Զ�������ʱ����
        @param rate ��λ��
        @version NIIEngine 3.0.0
        */
        void setRate(TimeDurMS rate);

        /** �����Զ��¼�������ɺ�,�Զ�������ʱ����
        @return ��λ��
        @version NIIEngine 3.0.0
        */
        TimeDurMS getRate() const;

        /** �����Ƿ��Զ��ظ�������갴���¼�
        @param set true ����;false ����
        */
        void setAutoEvent(bool set);

        /** �Ƿ��Զ��ظ�������갴���¼�
        */
        bool isAutoEvent() const;

        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dst);
    protected:
        /// @copydetails MouseControlItem::onPress
        virtual void onPress(const MousePressArgs * args);

        /// @copydetails MouseControlItem::onRelease
        virtual void onRelease(const MousePressArgs * args);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MousePressArgs * args);
    private:
        /**
        @brief Fires off a repeated mouse button down event for this window.
        */
        void rise(MouseButton button);
    protected:
        Widget * mDest;             ///< ��ǰ��Ӧ��Ŀ��
        TimeDurMS mRate;            ///< �����Զ��¼�������ɺ�,�Զ�������ʱ��
        TimeDurMS mThreshold;       ///< �����Զ��¼�������Ҫ��ʱ�䷧
        MouseButton mRepeatButton;  ///< �����Զ��¼��İ���
        TimeDurMS mRepeatElapse;    ///< ��ǰ��ɵ�ʱ�䷧
        bool mAutoEvent;            ///< �Ƿ��Զ��ظ�������갴���¼�
        bool mRepeat;               ///< �����Զ��¼����
    };
}
}
#endif