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

#ifndef _NII_LISTBOX_H_
#define _NII_LISTBOX_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUIListboxItem.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Listbox : public Container
    {
        friend class ListboxView;
    public:
        Listbox(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_Listbox, LangID lid = N_PrimaryLang);

        virtual ~Listbox();
        
        /** 
        @version NIIEngine 3.0.0
        */
        void addItem(ListboxItem * item);

        /**
        @version NIIEngine 3.0.0
        */
        void insertItem(ListboxItem * item, const ListboxItem * position);

        /** 
        @version NIIEngine 3.0.0
        */
        void removeItem(const ListboxItem * item);
        
        /**
        @version NIIEngine 3.0.0
        */
        void removeAllItem();
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getItemCount() const {return mItemList.size();}

        /** 
        @version NIIEngine 3.0.0
        */
        void setItemSelect(ListboxItem * item, bool state);

        /** 
        @version NIIEngine 3.0.0
        */
        void setItemSelect(NCount idx, bool state);
        
        /** 
        @version NIIEngine 3.0.0
        */
        void unselectAll();
        
        /** 
        @version NIIEngine 3.0.0
        */
        NCount getSelectCount() const;

        /** 
        @version NIIEngine 3.0.0
        */
        ListboxItem * getFirstSelect() const;

        /** 
        @version NIIEngine 3.0.0
        */
        ListboxItem * getNextSelect(const ListboxItem * item) const;

        /** 
        @version NIIEngine 3.0.0
        */
        ListboxItem * getItem(NCount index) const;
        
        /**
        @verison NIIEngine 3.0.0
        */
        ListboxItem * getItem(const Vector2f & pos) const;
        
        /** 
        @version NIIEngine 3.0.0
        */
        ListboxItem * getItem(const String & text, const ListboxItem * begin);

        /** 
        @version NIIEngine 3.0.0
        */
        Nidx getItemIndex(const ListboxItem * item) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void setMultiSelect(bool set);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isMultiSelect() const {return mMultiSelect;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setItemTipsEnable(bool set){ mTipsEnable = set; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isItemTipsEnable() const {return mTipsEnable;}

        /** 
        @version NIIEngine 3.0.0
        */
        bool isItemSelect(NCount index) const;

        /** 
        @version NIIEngine 3.0.0
        */
        bool isItemExist(const ListboxItem * item) const;

        /**
        @version NIIEngine 3.0.0
        */
        void SetForceVScrollbar(bool set);
        
        /**
        @version NIIEngine 3.0.0        
        */
        inline bool isForceVScrollbar() const  {return mForceVScroll;}

        /**
        @version NIIEngine 3.0.0
        */
        void SetForceHScrollbar(bool set);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isForceHScrollbar() const { return mForceHScroll;}
        
        /**
        @version NIIEngine 3.0.0
        */
        void setSortEnable(bool set);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSortEnable() const {return mSort;}

        /** 
        @version NIIEngine 3.0.0
        */
        void notifyItemUpdate();

        /** 
        @version NIIEngine 3.0.0
        */
        void visibleItem(Nidx idx);

        /** 
        @version NIIEngine 3.0.0
        */
        void visibleItem(const ListboxItem * item);

        /** 
        @version NIIEngine 3.0.0
        */
        virtual Rectf getListRenderArea() const;

        /**
        @version NIIEngine 3.0.0
        */
        Scrollbar * getVScrollbar() const;

        /**
        @version NIIEngine 3.0.0
        */
        Scrollbar * getHScrollbar() const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getMaxHeight() const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getMaxWidth() const;
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ItemListEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SortModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MultiSelectModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID VScrollModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID HScrollModeEvent;

        static const EventID EventCount; 
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void updateScrollbar();

        /** 
        @version NIIEngine 3.0.0
        */
        void selectRange(Nidx start, Nidx end);

        /** 
        @version NIIEngine 3.0.0
        */
        bool unselectImpl();

        /** 
        @version NIIEngine 3.0.0
        */
        bool removeAllImpl();

        /** 
        @version NIIEngine 3.0.0
        */
        void ScrollPrc(const EventArgs * args);

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onItemList(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSelect(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSortMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onMultiSelectMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onVScrollbarMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onHScrollbarMode(const WidgetEventArgs * arg);

        /// @copydetails Container::onSize
        virtual void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);
    protected:
        ListboxItemList mItemList;
        ListboxItem * mCurrentSelect;
        bool mSort;
        bool mMultiSelect;
        bool mForceVScroll;
        bool mForceHScroll;
        bool mTipsEnable;
    };

    /**
    @version NIIEngine 3.0.0
    */
    bool less(const ListboxItem * l, const ListboxItem * r);

    /** 
    @version NIIEngine 3.0.0
    */
    bool greater(const ListboxItem * l, const ListboxItem * r);

    /**
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI ListboxView : public WidgetView
    {
    public:
        ListboxView(WidgetViewlID wmid);

        ///@copydetails WidgetView::flush
        void flush();

		/// @copydetails WidgetView::notifyFont
		bool notifyFont(const Font * font);

        /**
        @version NIIEngine 3.0.0
        */
        Rectf getListRenderArea() const;

        /**
        @version NIIEngine 3.0.0
        */
        void fit(bool width, bool height) const;
    protected:
        Rectf getRenderArea(bool hscroll, bool vscroll) const;
    };
}
}
#endif