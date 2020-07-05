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

#ifndef _NII_UI_ItemListbox_H_
#define _NII_UI_ItemListbox_H_

#include "NiiUIPreInclude.h"
#include "NiiUIScrolledListContainer.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ItemListbox : public ScrolledListContainer
    {
    public:
        ItemListbox(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
            LangID lid = N_PrimaryLang);

        virtual ~ItemListbox();

        /**
        @version NIIEngine 3.0.0
        */
        void setMultiSelect(bool set);
        
        /**
        @verison NIIEngine 3.0.0
        */
        inline bool isMultiSelect() const {return mMultiSelect; }
        
        /**
        @version NIIEngine 3.0.0
        */
        NCount getSelectCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        inline ListItem * getLastSelect() const {return mLastSelect;}

        /**
        @version NIIEngine 3.0.0
        */
        ListItem * getFirstSelect(Nidx idx = 0) const;

        /**
        @version NIIEngine 3.0.0
        */
        ListItem * getNextSelect() const;

        /**
        @version NIIEngine 3.0.0
        */
        bool isSelect(Nidx index) const;

        /** 
        @verison NIIEngine 3.0.0
        */
        void selectRange(Nidx begin, Nidx end);

        /**
        @version NIIEngine 3.0.0
        */
        void selectAll();
        
        /**
        @version NIIEngine 3.0.0
        */
        void unselectAll();

        /// @copydetails ListContainer::notifyItemClick
        virtual void notifyItemClick(ListItem * li);

        /// @copydetails ListContainer::notifyItemSelect
        virtual void notifyItemSelect(ListItem * li, bool state);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails ListContainer::layoutContent
        virtual void layoutContent();
        
        /// @copydetails ListContainer::getContentSize
        virtual PlaneSizef getContentSize() const;
        
        /// @copydetails Widget::onKeyDown
        virtual void onKeyDown(const KeyEventArgs * arg);
       
        /// @copydetails ListContainer::onItemRmove
        virtual void onItemRmove(const EventArgs * e);

        virtual void onSelect(const WidgetEventArgs * arg);

        virtual void onMultiSelectMode(const WidgetEventArgs * arg);

        ListItem * findSelect(Nidx begin) const;        
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MultiSelectModeChangeEvent;

        static const EventID EventCount;
    protected:
        bool mMultiSelect;
        ListItem * mLastSelect;
        mutable NCount mNextSelect;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ItemListboxView : public ListContainerView
    {
    public:
        ItemListboxView(WidgetViewlID wsid);

        /// @copydetails ListContainerView::flush
        void flush();

        /// @copydetails ListContainerView::getRenderArea
        Rectf getRenderArea() const;

        /// @copydetails WidgetView::getArea
        Rectf getArea() const;
    public:
        static const StateID AreaState;
        static const StateID VScrollState;
        static const StateID HScrollState;
        static const StateID HVScrollState;

        static const StateID AmiAreaState;
        static const StateID AmiVScrollState;
        static const StateID AmiHScrollState;
        static const StateID AmiHVScrollState;
        static const StateID StateCount;
    protected:
        Rectf getRenderArea(bool hscroll, bool vscroll) const;

        /// @copydetails WidgetView::onStyleAttach
        void onStyleAttach();

        /// @copydetails WidgetView::onStyleDetach
        void onStyleDetach();
    protected:
        bool mStyleValid;
    };
}
}
#endif