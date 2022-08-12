/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
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
        NIIf getCurrent() const             { return mCurrentValue; }

        /** ���ò���ֵ
        @version NIIEngine 3.0.0
        */
        void setStepValue(NIIf value);

        /** ��ȡ����ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getStepValue() const           { return mStepValue; }

        /** �������ֵ
        @version NIIEngine 3.0.0
        */
        void setMaxValue(NIIf value);

        /** ��ȡ���ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getMaxValue() const            { return mMaxValue; }

        /** ������Сֵ
        @version NIIEngine 3.0.0
        */
        void setMinValue(NIIf value);

        /** ��ȡ��Сֵ
        @version NIIEngine 3.0.0
        */
        NIIf getMinValue() const            { return mMinValue; }

        /** ������ֵģʽ
        @version NIIEngine 3.0.0
        */
        void setNumMode(NumMode mode);

        /** ��ȡ��ֵģʽ
        @version NIIEngine 3.0.0
        */
        NumMode getNumMode() const          { return mNumMode; }

        /** �����ı�����
        @version NIIEngine 3.0.0
        */
        void setTextAreaView(StyleViewID id) { mTextAreaView = id; }

        /** ��ȡ�ı�����
        @version NIIEngine 3.0.0
        */
        StyleViewID getTextAreaView() const { return mTextAreaView; }

        /** ������ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        void setIncreaseView(StyleViewID id) { mIncreaseView = id; }

        /** ��ȡ��ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        StyleViewID getIncreaseView() const { return mIncreaseView; }

        /** ���ü�ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        void setDecreaseView(StyleViewID id) { mDecreaseView = id; }

        /** ��ȡ��ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        StyleViewID getDecreaseView() const { return mDecreaseView; }

        /** ��ȡ�ı�����
        @version NIIEngine 3.0.0
        */
        Editbox * getTextArea() const { return mTextArea; }

        /** ��ȡ��ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getIncrease() const { return mIncrease; }

        /** ��ȡ��ֵ��Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getDecrease() const { return mDecrease; }
    protected:
        /// @copydetails Container::initChild
        void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

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
        virtual void onFont(const FontEventArgs * arg);

        /** ��ֵģʽʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onNumModeChange(const WidgetEventArgs * arg);

        /** ���ı��ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onText(const WidgetEventArgs * arg);

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
        static const EventID ValueEvent;

        /** �����ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID StepValueEvent;

        /** ���ֵ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MaxValueEvent;

        /** ��Сֵ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MinValueEvent;

        /** ��ֵģʽ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID NumModeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Editbox * mTextArea;
        PushButton * mIncrease;
        PushButton * mDecrease;
        StyleViewID mTextAreaView;
        StyleViewID mIncreaseView;
        StyleViewID mDecreaseView;
        NumMode mNumMode;
        NIIf mStepValue;
        NIIf mCurrentValue;
        NIIf mMinValue;
        NIIf mMaxValue;
    };
}
}
#endif