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
#ifndef _NII_UI_NUMSPINNER_H_
#define _NII_UI_NUMSPINNER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** ��ֵ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NumSpinner : public Container
    {
    public:
        /** ����ģʽ
        @version NIIEngine 3.0.0
        */
        enum NumMode
        {
            NM_Int,
            NM_Float,
            NM_Hex,
            NM_Octal
        };
    public:
        NumSpinner(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        virtual ~NumSpinner();

        /** ���õ�ǰֵ
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf value);

        /** ��ȡ��ǰֵ
        @version NIIEngine 3.0.0
        */
        NIIf getCurrent() const;

        /** ���ò���ֵ
        @version NIIEngine 3.0.0
        */
        void setStepValue(NIIf value);

        /** ��ȡ����ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getStepValue() const;

        /** �������ֵ
        @version NIIEngine 3.0.0
        */
        void setMaxValue(NIIf value);

        /** ��ȡ���ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getMaxValue() const;

        /** ������Сֵ
        @version NIIEngine 3.0.0
        */
        void setMinValue(NIIf value);

        /** ��ȡ��Сֵ
        @version NIIEngine 3.0.0
        */
        NIIf getMinValue() const;

        /** ������ֵģʽ
        @version NIIEngine 3.0.0
        */
        void setNumMode(NumMode mode);

        /** ��ȡ��ֵģʽ
        @version NIIEngine 3.0.0
        */
        NumMode getNumMode() const;
    protected:
	    /// @copydetails Container::init
        void init();

        /// @copydetails PropertyCmdObj::init
		bool init(PropertyCmdSet * dest);

        /** ��ȡ�ı�����
        @version NIIEngine 3.0.0
        */
        Editbox * getTextArea() const;

        /** ��ȡ��ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getIncrease() const;

        /** ��ȡ��ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getDecrease() const;

        /** ��ȡ����ֵ
        @version NIIEngine 3.0.0
        */
		NIIf getAreaValue() const;

        /** ��ȡֵ�ַ���
        @version NIIEngine 3.0.0
        */
        String getValueText() const;

        /** ���ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onActivate(const ActivationEventArgs * arg);

        /** ������仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual	void onFontChange(const FontEventArgs * arg);

        /** ��ֵģʽʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onNumModeChange(const WidgetEventArgs * arg);

        /** ���ı��ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onTextChange(const WidgetEventArgs * arg);

        /** ��ֵ�仯��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onValueChange(const WidgetEventArgs * arg);

        /** ������ֵ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onStepValueChange(const WidgetEventArgs * arg);

        /** �����ֵ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onMaxValueChange(const WidgetEventArgs * arg);

        /** ����Сֵ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onMinValueChange(const WidgetEventArgs * arg);

        /** ��ֵ����
        @version NIIEngine 3.0.0
        */
        void IncreaseMFunc(const EventArgs * e);

        /** ��ֵ����
        @version NIIEngine 3.0.0
        */
		void DecreaseMFunc(const EventArgs * e);

        /** �ı��ı亯��
        @version NIIEngine 3.0.0
        */
        void TextChangeMFunc(const EventArgs * e);
    public:
        /** ֵ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ValueChangeEvent;

        /** �����ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID StepValueChangeEvent;

        /** ���ֵ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MaxValueChangeEvent;

        /** ��Сֵ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MinValueChangeEvent;

        /** ��ֵģʽ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID NumModeChangeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Editbox * mTextArea;
        PushButton * mIncrease;
        PushButton * mDecrease;
        NumMode mNumMode;
        NIIf mStepValue;
        NIIf mCurrentValue;
        NIIf mMinValue;
        NIIf mMaxValue;
    };
}
}
#endif