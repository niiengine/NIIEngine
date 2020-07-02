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

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabButtonView : public WidgetView
    {
    public:
        TabButtonView(WidgetModelID wsid);

        /// @copydetails WidgetView::flush
        void flush();

        static const StateID BottomEnableState;
        static const StateID BottomDisableState;
        static const StateID BottomSelectState;
        static const StateID BottomPushState;
        static const StateID BottomHoverState;
        static const StateID TopEnableState;
        static const StateID TopDisableState;
        static const StateID TopSelectState;
        static const StateID TopPushState;
        static const StateID TopHoverState;
        static const StateID StateCount;
    };
}
}
#endif