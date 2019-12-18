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

#ifndef _NII_UI_TAB_BUTTON_H_
#define _NII_UI_TAB_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIButton.h"

namespace NII
{
namespace UI
{
    /** ��ǩ�ؼ�ר�ð�Ŧ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabButton : public Button
    {
	public:
		TabButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);

        virtual ~TabButton();

        /** �����Ƿ���ѡ��
        version NIIEngine 3.0.0
        */
        virtual void select(bool b);

        /** ��ȡ�Ƿ���ѡ��
        @version NIIEngine 3.0.0
        */
        bool isSelect() const;

        /** ����Ŀ���ǩ����
        @version NIIEngine 3.0.0
        */
        void setDest(Container * c);

        /** ��ȡĿ���ǩ����
        @version NIIEngine 3.0.0
        */
        Container * getDest();
    protected:
        /** ���ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onClick(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);
    public:
        /** ����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ClickEvent;

        /** ��ק�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DragEvent;

        /** �����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ScrollEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Container * mDest;  ///< ��ǩ����
        bool mSelect;       ///< 
        bool mDrag;         ///< 
    };
}
}
#endif