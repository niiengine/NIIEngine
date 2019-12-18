/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

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
#ifndef _NII_UI_SCROLLBAR_H_
#define _NII_UI_SCROLLBAR_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Scrollbar : public Container
    {
    public:
        Scrollbar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        ~Scrollbar();

        /** ������ҳ��С
        @version NIIEngine 3.0.0
        */
        void setDocSize(NIIf size);

        /** ��ȡ��ҳ��С
        @version NIIEngine 3.0.0
        */
        NIIf getDocSize() const;

        /** ���ð�������С
        @version NIIEngine 3.0.0
        */
        void setStepSize(NIIf size);
        
        /** ��ȡ��������С
        @version NIIEngine 3.0.0
        */
        NIIf getStepSize() const;

        /** ����ҳ��С
        @version NIIEngine 3.0.0
        */
        void setPageSize(NIIf size);

        /** ��ȡҳ��С
        @version NIIEngine 3.0.0
        */
        NIIf getPageSize() const;
        
        /** ����ҳ���С
        @version NIIEngine 3.0.0
        */
        void setOverlapSize(NIIf size);
        
        /** ��ȡҳ���С
        @version NIIEngine 3.0.0
        */
        NIIf getOverlapSize() const;

        /** ���õ�ǰλ��
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf pos);
        
        /** ��ȡ��ǰλ��
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        NIIf getCurrent() const;

        /** ���õ�λ����ǰλ��
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        void setCurrentUnit(NIIf upos);        
        
        /** ��ȡ��λ����ǰλ��
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentUnit() const;

        /** ���򲽽�
        @version NIIEngine 3.0.0
        */
        void negStep();
        
        /** ���򲽽�
        @version NIIEngine 3.0.0
        */
        void posStep();

        /** ����ҳ��
        @version NIIEngine 3.0.0
        */
        void negPage();
        
        /** ����ҳ��
        @version NIIEngine 3.0.0
        */
        void posPage();
        
        /** ��ȡλ��ָ��
        @version NIIEngine 3.0.0
        */
        PosButton * getThumb() const;

        /** ��ȡ����Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getNegative() const;
        
        /** ��ȡ����Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getPositive() const;

        /** �����Ƿ��Զ�ĩβ
        @version NIIEngine 3.0.0
        */
        void setAutoEndPos(bool b);

        /** ��ȡ�Ƿ��Զ�ĩβ
        @version NIIEngine 3.0.0
        */
        bool isAutoEndPos() const;
        
        /** �ۺ�����
        @version NIIEngine 3.0.0
        */
        void setConfig(NIIf doc, NIIf page, NIIf step, NIIf pageremain, NIIf pos);
    protected:
        /// @copydetails Container::init
        void init();

        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** ���õ�ǰλ��
        @version NIIEngine 3.0.0
         */
        bool setCurrentImpl(NIIf pos);

        /** ��ȡ���λ��
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentMax() const;
        
        /** �Ƿ���ĩβλ��
        @version NIIEngine 3.0.0
        */
        bool isCurrentEnd() const;

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbMoveMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void PositiveMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void NegativeMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbBeginMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbEndMFunc(const EventArgs * e);

        /// @copydetail Widget::check
        virtual bool check(const WidgetModel * style) const;

        /** ��ǰλ�ñ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCurrentChange(const WidgetEventArgs * arg);

        /** λ��ָ�꿪ʼ�ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onTumbMoveBegin(const WidgetEventArgs * arg);

        /** λ��ָ������ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveEnd(const WidgetEventArgs * arg);

        /** ���÷����仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onConfigChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
		/** 
		@version NIIEngine 3.0.0
		*/
		static const PropertyID DocSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PageSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID StepSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID OverlapSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID CurrentProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PropertyCount;

        /** ��ǰλ�ñ仯ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID CurrentChageEvent;

        /** λ��ָ�꿪ʼ�ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveBeginEvent;

        /** λ��ָ������ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveEndEvent;

        /** ���÷����仯ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ConfigChangeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        PosButton * mThumb;
        PushButton * mNegative;
        PushButton * mPositive;
        NIIf mDocSize;
        NIIf mPageSize;
        NIIf mStepSize;
        NIIf mPageRemian;
        NIIf mCurrentPos;
        bool mAutoEnd;
    };
}
}
#endif