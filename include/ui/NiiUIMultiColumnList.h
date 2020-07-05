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

#ifndef _NII_UI_MultiColumnList_H_
#define _NII_UI_MultiColumnList_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetView.h"
#include "NiiUIListHeader.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GridCellIndx
    {
    public:
        GridCellIndx(Nidx r, Nidx c) : mRow(r), mColumn(c) {}

        inline GridCellIndx &   operator= (const GridCellIndx & o){mColumn = o.mColumn; mRow = o.mRow; return *this; }
        inline bool             operator< (const GridCellIndx & o) const{ return (mRow < o.mRow) || ((mRow == o.mRow) && (mColumn < o.mColumn)); }
        bool operator<=(const GridCellIndx & o) const;
        bool operator> (const GridCellIndx & o) const;
        bool operator>=(const GridCellIndx & o) const;
        inline bool             operator==(const GridCellIndx & o) const{ return (mColumn == o.mColumn) && (mRow == o.mRow); }
        inline bool             operator!=(const GridCellIndx & o) const{ return (mColumn != o.mColumn) || (mRow != o.mRow);}
        
        Nidx mRow;
        Nidx mColumn;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiColumnList : public Container
    {
        friend class MultiColumnListView;
    public:
        /**
        @version NIIEngine 3.0.0
        */
        enum SelectMode
        {
            SM_SingleRow,
            SM_MultiRow,
            SM_SingleColumn,
            SM_MultiColumn,
            SM_SingleCell,
            SM_MultiCell,
            SM_ReserveSingleColumn,
            SM_ReserveMultiColumn,
            SM_ReserveSingleRow,
            SM_ReserveMultiRow
        };
        class _EngineAPI Row
        {
        public:
            inline ListboxItem * & operator[](Nui32 idx) const { return mItemList[idx]; }
            bool operator<(const Row & o) const;
            bool operator>(const Row & o) const;

            typedef vector<ListboxItem *>::type ItemList;
            mutable ItemList mItemList;
            Nidx mSortColumn;
            WidgetID mRowID;
        };
    public:
        MultiColumnList(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~MultiColumnList();

        /**
        @version NIIEngine 3.0.0
        */
        void setSortEnable(bool set);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSortEnable() const{ return getListHeader()->getSortMode() == ListHeaderItem::SM_None;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setColumnSizeEnable(bool set){ getListHeader()->setColumnSizeEnable(set); }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isColumnSizeEnable() const{ return getListHeader()->isColumnSizeEnable(); }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void setColumnDragEnable(bool set){ getListHeader()->setColumnDragEnable(set);}

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isColumnDragEnable() const{ return getListHeader()->isColumnDragEnable(); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getColumnCount() const{ return mColumnCount; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getRowCount() const{ return mGrid.size(); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline Nidx getColumnIndex() const{ return getListHeader()->getColumnIndex(getListHeader()->getSortColumn()); }

        /**
        @version NIIEngine 3.0.0
        */
        WidgetID getSortColumnID() const;

        /**
        @version NIIEngine 3.0.0
        */
        inline Nidx getColumnIndex(WidgetID cidx) const{ return getListHeader()->getColumnIndex(cidx);}

        /**
        @version NIIEngine 3.0.0
        */
        inline Nidx getColumnIndex(const String & text) const{ return getListHeader()->getColumnIndex(text); }

        /**
        @version NIIEngine 3.0.0
        */
        RelPosf getColumnTotalWidth() const;
        
        /**
        @version NIIEngine 3.0.0
        */
        NIIf getRowTotalHeight() const;

        /**
        @version NIIEngine 3.0.0
        */
        inline RelPosf getColumnWidth(Nidx cidx) const{ return getListHeader()->getColumnWidth(cidx); }
        
        /** 
        @version NIIEngine 3.0.0
        */
        void setSortMode(ListHeaderItem::SortMode mode);

        /**
        @version NIIEngine 3.0.0
        */
        inline ListHeaderItem::SortMode getSortMode() const{ return getListHeader()->getSortMode(); }

        /**
        @version NIIEngine 3.0.0
        */
        inline ListHeaderItem * getColumn(Nidx cidx) const{ return getListHeader()->getColumn(cidx); }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline WidgetID getColumnID(Nidx idx) const{ return getListHeader()->getColumn(idx)->getID(); }

        /**
        @version NIIEngine 3.0.0
        */
        Nidx getItemRowIndex(const ListboxItem * item) const;

        /**
        @version NIIEngine 3.0.0
        */
        Nidx getItemColumnIndex(const ListboxItem * item) const;

        /**
        @version NIIEngine 3.0.0
        */
        GridCellIndx getItemGridIndex(const ListboxItem * item) const;

        /**
        @version NIIEngine 3.0.0
        */
        ListboxItem * getItem(const GridCellIndx & idx) const;

        /**
        @version NIIEngine 3.0.0
        */
        bool isColumnExist(const ListboxItem * item, Nidx cidx) const;

        /** 
        @version NIIEngine 3.0.0
        */
        bool isRowExist(const ListboxItem * item, Nidx ridx) const;

        /**
        @version NIIEngine 3.0.0
        */
        bool isItemExist(const ListboxItem * item) const;

        /** 
        @version NIIEngine 3.0.0
        */
        ListboxItem * getColumnItem(const String & text, Nidx cidx, const ListboxItem * begin) const;

        /**
        @version NIIEngine 3.0.0
        */
        ListboxItem * getRowItem(const String & text, Nidx ridx, const ListboxItem * begin) const;

        /**
        @version NIIEngine 3.0.0
        */
        ListboxItem * getItem(const String & text, const ListboxItem* begin) const;

        /** 
        @version NIIEngine 3.0.0
        */
        ListboxItem * getFirstSelect() const;

        /** 
        @version NIIEngine 3.0.0
        */
        ListboxItem * getNextSelect(const ListboxItem * begin) const;

        /** 
        @version NIIEngine 3.0.0
        */
        NCount getSelectCount() const;

        /** 
        @version NIIEngine 3.0.0
        */
        bool isItemSelect(const GridCellIndx & index) const;

        /**
        @version NIIEngine 3.0.0
        */
        WidgetID getReserveSelectColumnID() const;

        /** 
        @version NIIEngine 3.0.0
        */
        Nidx getReserveSelectColumn() const;

        /** 
        @version NIIEngine 3.0.0
        */
        Nidx getReserveSelectRow() const;
        
        /** 
        @version NIIEngine 3.0.0
        */
        void setSelectionMode(MultiColumnList::SelectMode mode);

        /**
        @version NIIEngine 3.0.0
        */
        inline MultiColumnList::SelectMode getSelectionMode() const{ return mSelectMode; }

        /** 
        @version NIIEngine 3.0.0
        */
        WidgetID getRowID(Nidx ridx) const;

        /**
        @version NIIEngine 3.0.0
        */
        Nidx getRowIndex(WidgetID rid) const;

        /** 
        @version NIIEngine 3.0.0
        */
        Rectf getListRenderArea() const;

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
        ListHeader * getListHeader() const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getMaxColumnWidth(Nidx cidx) const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getMaxRowHeight(Nidx ridx) const;
        
        /**
        @version NIIEngine 3.0.0
        */
        void setColumnAutoSize(bool set);

        /**
        @version NIIEngine 3.0.0
        */
        bool isColumnAutoSize() const;

        /**
        @version NIIEngine 3.0.0
        */
        void removeAll(void);

        /**
        @version NIIEngine 3.0.0
        */
        void addColumn(const String & text, WidgetID cidx, const RelPosf & width);
        
        /**
        @version NIIEngine 3.0.0
        */
        void addColumn(const String & value);

        /**
        @version NIIEngine 3.0.0
        */
        void insertColumn(const String & text, WidgetID cidx, const RelPosf & width, Nui32 pos);

        /**
        @verison NIIEngine 3.0.0
        */
        void removeColumn(Nidx cidx);

        /**
        @version NIIEngine 3.0.0
        */
        void removeColumn(WidgetID cidx);

        /**
        @version NIIEngine 3.0.0
        */
        void moveColumn(Nidx cidx, Nidx pos);

        /**
        @version NIIEngine 3.0.0
        */
        void moveColumn(WidgetID cidx, Nidx pos);

        /**
        @version NIIEngine 3.0.0
        */
        Nidx addRow(WidgetID rid = 0);

        /**
        @version NIIEngine 3.0.0
        */
        Nidx addRow(ListboxItem * item, WidgetID cid, WidgetID rid = 0);

        /**
        @version NIIEngine 3.0.0
        */
        Nidx insertRow(Nidx ridx, WidgetID rid = 0);

        /**
        @version NIIEngine 3.0.0
        */
        Nidx insertRow(ListboxItem * item, WidgetID cid, Nidx ridx, WidgetID rid = 0);

        /** 
        @version NIIEngine 3.0.0
        */
        void removeRow(Nidx ridx);

        /** 
        @version NIIEngine 3.0.0
        */
        void setItem(ListboxItem * item, const GridCellIndx & index);

        /** 
        @version NIIEngine 3.0.0
        */
        void setItem(ListboxItem * item, WidgetID cidx, Nidx ridx);

        /** 
        @version NIIEngine 3.0.0
        */
        void setReserveSelectColumn(WidgetID cid);

        /** 
        @version NIIEngine 3.0.0
        */
        void setReserveSelectColumn(Nidx cidx);

        /**
        @version NIIEngine 3.0.0
        */
        void setReserveSelectRow(Nidx ridx);

        /**
        @version NIIEngine 3.0.0
        */
        void setSortColumn(Nidx cidx);

        /**
        @version NIIEngine 3.0.0
        */
        void setSortColumn(WidgetID wid);

        /**
        @version NIIEngine 3.0.0
        */
        void SetForceVScrollbar(bool set);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isForceVScrollbar() const{ return mForceVScroll; }

        /**
        @version NIIEngine 3.0.0
        */
        void SetForceHScrollbar(bool set);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isForceHScrollbar() const{ return mForceHScroll; }

        /** 
        @version NIIEngine 3.0.0
        */
        void setItemSelect(ListboxItem * item, bool state);

        /**
        @version NIIEngine 3.0.0
        */
        void setItemSelect(const GridCellIndx & index, bool state);
        
        /** 
        @version NIIEngine 3.0.0
        */
        void unselectAll();

        /**
        @version NIIEngine 3.0.0
        */
        void notifyItemUpdate();

        /**
        @version NIIEngine 3.0.0
        */
        inline void setColumnWidth(Nidx cidx, const RelPosf & width){ getListHeader()->setColumnWidth(cidx, width); }

        /**
        @version NIIEngine 3.0.0
        */
        void setRowID(Nui32 ridx, Nui32 row_id);

        /**
        @version NIIEngine 3.0.0
        */
        void visibleItem(const ListboxItem * item);

        /**
        @verison NIIEngine 3.0.0
        */
        void visibleItem(const GridCellIndx & index);

        /**
        @version NIIEngine 3.0.0
        */
        void visibleItemRow(const ListboxItem * item);

        /**
        @verison NIIEngine 3.0.0
        */
        void visibleItemRow(Nidx ridx);
        
        /**
        @version NIIEngine 3.0.0
        */
        void visibleItemColumn(const ListboxItem * item);

        /**
        @verison NIIEngine 3.0.0
        */
        void visibleItemColumn(Nidx cidx);
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);
    protected:
        void updateScrollbar();

        /**
        @version NIIEngine 3.0.0
        */
        bool selectRange(const GridCellIndx & start, const GridCellIndx & end);
        
        /**
        @verison NIIEngine 3.0.0
        */
        void resortList();

        /**
        @version NIIEngine 3.0.0
        */
        bool unselectImpl();

        /**
        @version NIIEngine 3.0.0
        */
        ListboxItem * getItem(const Vector2f & pt) const;

        /** 
        @version NIIEngine 3.0.0
        */
        bool setCellSelect(const GridCellIndx & index, bool state);

        /** 
        @version NIIEngine 3.0.0
        */
        void setRowSelect(Nidx ridx, bool state);

        /**
        @version NIIEngine 3.0.0
        */
        void setColumnSelect(Nidx cidx, bool state);

        /** 
        @version NIIEngine 3.0.0
        */
        void moveColumnImpl(Nidx cidx, Nidx ncidx);
        
        /**
        @version NIIEngine 3.0.0
        */
        bool removeAllImpl();

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;

        /// @copydetails Container::writeImpl
        NCount writeImpl(XmlSerializer * dest) const;

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSelectMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onReserveSelectColumn(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onReserveSelectRow(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onVScrollbarMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onHScrollbarMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSelect(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemList(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSortColumn(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSortMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onColumnResize(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onColumnMove(const WidgetEventArgs * arg);
        
        /// @copydetails Container::onSize
        virtual void onSize(const WidgetEventArgs * arg);
        
        /// @copydetails Widget::onFont
        virtual void onFont(const FontEventArgs * arg);
        
        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);
        
        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        void HeaderScrollPrc(const EventArgs * e);
        void HeaderMovePrc(const EventArgs * e);
        void HeaderColumnReizePrc(const EventArgs * e);
        void HeaderSortColumnPrc(const EventArgs * e);
        void HeaderSortModePrc(const EventArgs * e);
        void HeaderItemDClickPrc(const EventArgs * e);
        void HScrollbarPrc(const EventArgs * e);
        void VScrollbarPrc(const EventArgs * e);
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReserveSelectColumnEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReserveSelectRowEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID VScrollModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID HScrollModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ItemListEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SortColumnEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SortModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnResizeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnMoveEvent;

        static const EventID EventCount;

        static const WidgetID VScrollbarID;

        static const WidgetID HScrollbarID;

        static const WidgetID ListHeaderID;
    protected:
        typedef vector<Row>::type ListItemGrid;
    protected:
        SelectMode mSelectMode;
        Nui32 mReserveSelectColumn;
        Nui32 mReserveSelectRow;
        NCount mColumnCount;
        ListboxItem * mCurrentSelect;
        ListItemGrid mGrid;

        bool mAutoSizeColumn;
        bool mMultiSelect;
        bool mSingleRowSelect;
        bool mSingleColumnSelect;
        bool mReserveSingleRow;
        bool mReserveSingleColumn;
        bool mForceVScroll;
        bool mForceHScroll;

    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiColumnListView : public WidgetView
    {
    public:
        MultiColumnListView(WidgetViewlID wmid);

        /**
        @version NIIEngine 3.0.0
        */
        Rectf getListRenderArea() const;

        /// @copydetails WidgetView::flush
        void flush();
    };
}
}
#endif