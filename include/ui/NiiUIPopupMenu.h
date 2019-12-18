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
#ifndef _NII_UI_PopupMenu_H_
#define _NII_UI_PopupMenu_H_

#include "NiiUIPreInclude.h"
#include "NiiUIMenubar.h"
#include "NiiUIListContainerWidgetModel.h"

namespace NII
{
namespace UI
{
    /** �����˵���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PopupMenu : public Menubar
    {
	public:
        PopupMenu(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);

        virtual ~PopupMenu();

        /** �Ƿ��ڴ�״̬
        @version NIIEngine 3.0.0
        */
        bool isOpenState() const;

        /** ����̸��ʱ��
        @version NIIEngine 3.0.0
        */
        void setFadeInTime(TimeDurMS time);

        /** ��ȡ̸��ʱ��
        @version NIIEngine 3.0.0
        */
        TimeDurMS getFadeInTime() const;

        /** ����̸��ʱ��
        @version NIIEngine 3.0.0
        */
        void setFadeOutTime(TimeDurMS time);

        /** ��ȡ̸��ʱ��
        @version NIIEngine 3.0.0
        */
        TimeDurMS getFadeOutTime() const;

        /** ������
        @version NIIEngine 3.0.0
        */
        void toggleOpen(bool parent = true);

        /** �����ر�
        @version NIIEngine 3.0.0
        */
        void toggleClose(bool parent = true);
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);
    protected:
        /// @copydetails Widget::updateImpl
        virtual void updateImpl(TimeDurMS cost);

        /// @copydetails PopupMenu::layoutContent
        virtual void layoutContent();

        /// @copydetails PopupMenu::getContentSize
        virtual PlaneSizef getContentSize() const;

        /// @copydetails Container::onAlphaChange
        virtual void onAlphaChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onShow
        virtual void onShow(const WidgetEventArgs * arg);

        /// @copydetails Menubar::onHide
        virtual void onHide(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);
    protected:
        TimeDurMS mPopupFadeOut;
        TimeDurMS mPopupFadeIn;
        TimeDurMS mFadeCost;
        NIIf mOrgAlpha;
        bool mFading;
        bool mFadeOut;
        bool mOpenState;
    };

	/**
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI DefaultPopupMenuWidgetModel : public ListContainerWidgetModel
	{
	public:
		DefaultPopupMenuWidgetModel(WidgetModelID wmid);

		/// @copydetails ListContainerWidgetModel::flush
		void flush();

		Rectf getItemRenderArea() const;

		static const StateID ItemRenderAreaState;
		static const StateID StateCount;
	};
}
}
#endif