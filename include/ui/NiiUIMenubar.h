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
#ifndef _NII_UI_Menubar_H_
#define _NII_UI_Menubar_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiUIListContainer.h"

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
            ScriptTypeID stid = N_CmdObj_Menubar, LangID lid = N_PrimaryLang);
        virtual ~Menubar();

        /** ���õ�����
        @version NIIEngine 3.0.0
        */
        void setPopupItem(MenuItem * item);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        inline MenuItem * getPopupItem() const{ return mPopupItem; }

        /** �������
        @version NIIEngine 3.0.0
        */
        void setItemSpace(NIIf space);

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        inline NIIf getItemSpace() const{ return mItemSpace; }

        /** �����Ƿ�൯��ģʽ
        @version NIIEngine 3.0.0
        */
        void setMultiPopup(bool b);

        /** ��ȡ�Ƿ�൯��ģʽ
        @version NIIEngine 3.0.0
        */
        inline bool isMultiPopup() const{ return mMultiPopups; }

        /** �����Զ��رյ�����
        @version NIIEngine 3.0.0
        */
        inline void setAutoClosePopup(bool b){ mAutoClosePopups = b; }

        /** ��ȡ�Զ��رյ�����
        @version NIIEngine 3.0.0
        */
        inline bool isAutoClosePopup() const{ return mAutoClosePopups; }
    protected:
        bool initCmd(PropertyCmdSet * dest);

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
    class _EngineAPI DefaultListContainerView : public ListContainerView
    {
    public:
        DefaultListContainerView(WidgetViewlID wsid);

        /// @copydetails WidgetView::flush
        void flush();

        Rectf getRenderArea() const;

        static const StateID ItemRenderAreaState;
        static const StateID StateCount;
    };
}
}
#endif