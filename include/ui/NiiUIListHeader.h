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

#ifndef _NII_UI_ListHeader_H_
#define _NII_UI_ListHeader_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUIListHeaderItem.h"
#include "NiiUIStyle.h"

namespace NII
{
namespace UI
{
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ColumnEventArgs : public WidgetEventArgs
    {
    public:
        ColumnEventArgs(Widget * wnd, Nui32 idx, Nui32 nidx) :
            WidgetEventArgs(wnd),
            mIndex(idx),
            mNewIndex(nidx) {};

        Nui32 mIndex;
        Nui32 mNewIndex;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListHeader : public Container
    {
    public:
        ListHeader(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
            LangID lid = N_PrimaryLang);

        virtual ~ListHeader();
        
        /**
        @version NIIEngine 3.0.0
        */
        void addColumn(const String & text, WidgetID id, const RelPosf & width);

        /**
        @version NIIEngine 3.0.0
        */
        void insertColumn(const String & text, WidgetID id, const RelPosf & width, Nui32 pos);

        /**
        @version NIIEngine 3.0.0
        */
        void insertColumn(const String & text, WidgetID id, const RelPosf & width, const ListHeaderItem * pos);

        /**
        @version NIIEngine 3.0.0
        */
        void removeColumn(Nidx idx);

        /**
        @version NIIEngine 3.0.0
        */
        void removeColumn(const ListHeaderItem * item);
        
        /**
        @version NIIEngine 3.0.0
        */
		inline NCount getColumnCount() const { return mItemList.size(); }

        /**
        @version NIIEngine 3.0.0
        */
        inline ListHeaderItem * getColumn(Nidx idx) const{ return mItemList[idx]; }

        /**
        @version NIIEngine 3.0.0
        */
        ListHeaderItem * getColumn(WidgetID id) const;

        /**
        @version NIIEngine 3.0.0
        */
        inline WidgetID getSortColumnID() const { return getSortColumn()->getID(); }

        /**
        @version NIIEngine 3.0.0
        */
        Nidx getColumnIndex(const ListHeaderItem * item) const;

        /**
        @version NIIEngine 3.0.0
        */
        Nidx getColumnIndex(WidgetID id) const;

        /**
        @version NIIEngine 3.0.0
        */
        Nidx getColumnIndex(const String & text) const;
        
        /**
        @version NIIEngine 3.0.0
        */
        void setColumnOffset(NIIf offset);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline NIIf getColumnOffset() const {return mColumnOffset;}

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getColumnPixelOffset(const ListHeaderItem & item) const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getColumnPixelOffset(Nidx column) const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getMaxColumnPixelOffset() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setColumnWidth(Nidx column, const RelPosf & width);

        /** 
        @version NIIEngine 3.0.0
        */
        RelPosf getColumnWidth(Nidx column) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void setSortMode(ListHeaderItem::SortMode mode);

        /**
        @version NIIEngine 3.0.0
        */
        inline ListHeaderItem::SortMode getSortMode() const{  return mSortMode; }

        /**
        @version NIIEngine 3.0.0
        */
        void setColumnSizeEnable(bool set);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isColumnSizeEnable() const { return mResizeEnable; }
        
        /**
        @version NIIEngine 3.0.0
        */
        void setColumnDragEnable(bool set);
        
        /** 
        @version NIIEngine 3.0.0
        */
        bool isColumnDragEnable() const{ return mDragMove; }

        /** 
        @version NIIEngine 3.0.0
        */
        void setSortColumn(const ListHeaderItem * item);
        
        /**
        @version NIIEngine 3.0.0
        */
        void setSortColumn(Nidx idx);
        
        /**
        @version NIIEngine 3.0.0
        */
        void getSortColumn(WidgetID id);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline ListHeaderItem * getSortColumn() const{ return mSortItem; }

        /**
        @version NIIEngine 3.0.0
        */
        void moveColumn(Nidx column, Nidx pos);

        /**
        @version NIIEngine 3.0.0
        */
        void moveColumn(Nidx column, const ListHeaderItem * pos);

        /**
        @version NIIEngine 3.0.0
        */
        void moveColumn(const ListHeaderItem * item, Nidx pos);

        /**
        @version NIIEngine 3.0.0
        */
        void moveColumn(const ListHeaderItem * item, const ListHeaderItem * pos);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * view) const;

        /**
        @version NIIEngine 3.0.0
        */
        void layoutItem();

        /**
        @version NIIEngine 3.0.0
        */
        ListHeaderItem * createItem(WidgetID name) const;

        /**
        @version NIIEngine 3.0.0
        */
        void destroyItem(ListHeaderItem * item) const;

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
        virtual void onSplitterDClick(const WidgetEventArgs * arg);

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onColumnResize(const WidgetEventArgs * arg);

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onColumnClick(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onColumnIndex(const WidgetEventArgs * arg);

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onColumnAdd(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onColumnRemove(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onColumnMoveMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onColumnSizeMode(const WidgetEventArgs * arg);

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void onColumnOffset(const WidgetEventArgs * arg);

        void ItemResizePrc(const EventArgs * e);
        
        void ItemMovePrc(const EventArgs * e);
        
        void ItemClickPrc(const EventArgs * e);
        
        void ItemDClickPrc(const EventArgs * e);
        
        void ItemDragPrc(const EventArgs * e);
    public:
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID SortModeEvent;
        
        /** 
        @version NIIEngine 3.0.0
         */
        static const EventID SortColumnEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnResizeEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnClickEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID SplitterDClickEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnIndexEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnAddEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnRemoveEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnMoveModeEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnSizeModeEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ColumnOffsetEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;

        static const NIIf ScrollSpeed;
        static const NIIf ItemMinWidth;
    protected:
        ListHeaderItemList mItemList;
        ListHeaderItem::SortMode mSortMode;
        ListHeaderItem * mSortItem;
        NIIf mColumnOffset;
        bool mResizeEnable;
        bool mDragMove;
    };
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListHeaderView : public WidgetView
    {
    public:
        ListHeaderView(WidgetViewlID type);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setItemType(StyleID type){ mItemType = type;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline StyleID getItemType() const{ return mItemType; }

        /**
        @version NIIEngine 3.0.0
        */
        ListHeaderItem * createItem(WidgetID wid) const;

        /**
        @version NIIEngine 3.0.0
        */
        void destroyItem(ListHeaderItem * item) const;
        
        /// @copydetails WidgetView::flush
        void flush();
    protected:
        StyleID mItemType;
    };
}
}
#endif