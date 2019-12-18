/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-2-8

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
#ifndef _NII_UI_SLIDER_H_
#define _NII_UI_SLIDER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** ���鵥Ԫ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Slider : public Container
    {
		friend class DefaultSliderWidgetModel;
    public:
        Slider(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        virtual ~Slider();

        /** ���õ�ǰֵ
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf value);
        
        /** ��ȡ��ǰֵ
        @version NIIEngine 3.0.0
        */
        NIIf getCurrent() const;

        /** �������ֵ
        @version NIIEngine 3.0.0
        */
        void setMaxValue(NIIf value);
        
        /** ��ȡ���ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getMaxValue() const;

        /** ���ò���ֵ
        @version NIIEngine 3.0.0
        */
        void setStepValue(NIIf value);
        
        /** ��ȡ����ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getStepValue() const;

        /** ��ȡλ��ָ��
        @version NIIEngine 3.0.0
        */
        PosButton * getThumb() const;
    protected:
        /// @copydetails Container::init
        virtual	void init();

        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual	void onCursorDrag(const CursorEventArgs * arg);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbMoveMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbBeginMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbEndMFunc(const EventArgs * e);

        /// @copydetails Widget::check
        virtual bool check(const WidgetModel * style) const;

        /** ֵ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onValueChange(const WidgetEventArgs * arg);

        /** ָ�꿪ʼ�ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveBegin(const WidgetEventArgs * arg);

        /** ָ������ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveEnd(const WidgetEventArgs * arg);
    public:
        /** ֵ�ı�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ValueChangeEvent;

        /** ָ�꿪ʼ�ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveBeginEvent;

        /** ָ������ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID TumbMoveEndEvent;
        
        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        PosButton * mThumb;
        NIIf mCurrentValue;
        NIIf mMaxValue;
        NIIf mStepValue;
    };
}
}
#endif