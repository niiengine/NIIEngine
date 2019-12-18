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
#ifndef _NII_UI_Menubar_H_
#define _NII_UI_Menubar_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiUIListContainer.h"
#include "NiiUIListContainerWidgetModel.h"

namespace NII
{
namespace UI
{
    /** �˵���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Menubar : public ListContainer
    {
    public:
        Menubar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			ScriptTypeID stid = N_CMD_Menubar, LangID lid = N_PrimaryLang);
        virtual ~Menubar();

        /** ���õ�����
        @version NIIEngine 3.0.0
        */
        void setPopupItem(MenuItem * item);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        MenuItem * getPopupItem() const;

        /** �������
        @version NIIEngine 3.0.0
        */
        void setItemSpace(NIIf space);

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        NIIf getItemSpace() const;

        /** �����Ƿ�൯��ģʽ
        @version NIIEngine 3.0.0
        */
        void setMultiPopup(bool b);

        /** ��ȡ�Ƿ�൯��ģʽ
        @version NIIEngine 3.0.0
        */
        bool isMultiPopup() const;

        /** �����Զ��رյ�����
        @version NIIEngine 3.0.0
        */
        void setAutoClosePopup(bool b);

        /** ��ȡ�Զ��رյ�����
        @version NIIEngine 3.0.0
        */
        bool isAutoClosePopup() const;
    protected:
        bool init(PropertyCmdSet * dest);

        /// @copydetails ListContainer::layoutContent
        virtual void layoutContent();

        /// @copydetails ListContainer::getContentSize
        virtual PlaneSizef getContentSize() const;
    protected:
        /** ������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onOpenPopup(const WidgetEventArgs * arg);

        /** �ر���ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onClosePopup(const WidgetEventArgs * arg);

        /// @copydetails Widget::onHide
        virtual void onHide(const WidgetEventArgs * arg);
        
        /// @copydetails Container::onRemove
        virtual void onRemove(const WidgetEventArgs * arg);
    public:
        /** ����ʱ����
        @version NIIEngine 3.0.0
        */
        static const EventID OpenPopupEvent;

        /** �ر���ʱ����
        @version NIIEngine 3.0.0
        */
        static const EventID ClosePopupEvent;

        /// �¼�����
        static const EventID EventCount;
    protected:
        MenuItem * mPopupItem;
        NIIf mItemSpace;
        bool mMultiPopups;
        bool mAutoClosePopups;
    };

	/**
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI DefaultMenubarWidgetModel : public ListContainerWidgetModel
	{
	public:
		DefaultMenubarWidgetModel(WidgetModelID wsid);

		/// @copydetails WidgetModel::flush
		void flush();

		Rectf getItemRenderArea() const;

		static const StateID ItemRenderAreaState;
		static const StateID StateCount;
	};
}
}
#endif