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

#ifndef _NII_UI_BUTTON_H_
#define _NII_UI_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** ��ť��UI��Ԫ����
    @remark ��ť������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Button : public Widget
    {
    public:
        Button(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
			ScriptTypeID stid = N_CMD_Button, LangID lid = N_PrimaryLang);

        virtual ~Button();

        /** �����Ƿ��ڰ���״̬
        @version NIIEngine 3.0.0
        */
        void press(bool b);

		/** �����Ƿ���ѡ��״̬
		@version NIIEngine 3.0.0
		*/
		void select(bool b);

		/** ��ȡ�Ƿ���ѡ��״̬
		@version NIIEngine 3.0.0
		*/
		bool isSelect() const;
        
        /** �α��Ƿ������UI��Ԫ�����Ϸ�
        @version NIIEngine 3.0.0
        */
        bool isHover() const;

        /** ���UI��Ԫ�����Ƿ��ڱ�����״̬
        @version NIIEngine 3.0.0
        */
        bool isPress() const;
    protected:
		/// @copydetails Button::onSelectChange
		virtual void onSelectChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorLeave
        virtual void onCursorLeave(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
	public:
		/** ѡ������
		@version NIIEngine 3.0.0
		*/
		static const PropertyID SelectProperty;

		/** ��������
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PropertyCount;

		/** ѡ���¼�
		@version NIIEngine 3.0.0
		*/
		static const EventID SelectChangeEvent;

		/** �¼�����
		@version NIIEngine 3.0.0
		*/
		static const EventID EventCount;
    protected:
		///@copydetails Widget::int;
		bool init(PropertyCmdSet * dest);

        /** ����UI��Ԫ����
        @param pos UI��ϵ�е�λ
        */
        void update(const Vector2f & pos);
    protected:
        bool mPress;    ///< �Ƿ��ڱ�����״̬
        bool mOver;     ///< �α긲��״̬��
		bool mSelect;
    };
}
}
#endif