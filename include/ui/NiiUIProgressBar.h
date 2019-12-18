/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-7

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

#ifndef _NII_UI_ProgressBar_H_
#define _NII_UI_ProgressBar_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** �������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProgressBar : public Widget
    {
    public:
        ProgressBar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        virtual ~ProgressBar();

        /** �������Զ�����
        @version NIIEngine 3.0.0
        */
        void step();

        /** ������ǰ����
        @version NIIEngine 3.0.0
        */
        void adjust(NIIf delta);

        /** ���õ�ǰ����
        @version NIIEngine 3.0.0
        */
        void setProgress(NIIf p);

        /** ��ȡ��ǰ����
        @version NIIEngine 3.0.0
        */
        NIIf getProgress() const;

        /** ���ò���
        @version NIIEngine 3.0.0
        */
        void setStep(NIIf p);

        /** ���ò���
        @version NIIEngine 3.0.0
        */
        NIIf getStep() const;
    protected:
        /// @copydetials PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** ���ȱ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual	void onPrcChange(const WidgetEventArgs * arg);

        /** ���ȱ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onPrcDone(const WidgetEventArgs * arg);
    public:
        /** ���ȿ�ʼ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PrcStartEvent;

        /** ���ȱ仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PrcChangeEvent;

        /** ��������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PrcDoneEvent;

        /// �¼�����
		static const EventID EventCount;
    protected:
        NIIf mProgress;
        NIIf mStep;
    };
}
}
#endif