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

#ifndef _NII_UI_TREE_H_
#define _NII_UI_TREE_H_

#include "NiiPreProcess.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWidgetView.h"
#include "NiiUITreeItem.h"
#include "NiiUIScrollbar.h"

namespace NII
{
namespace UI
{
    /** 
    @version NIIEngine 3.0.0
     */
    class _EngineAPI TreeEventArgs : public WidgetEventArgs
    {
    public:
        TreeEventArgs(Widget * wnd) :
            WidgetEventArgs(wnd)
        {
            mItem = 0;
        }
        TreeItem * mItem;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Tree : public Container
    {
        friend class TreeItem;
    public:
        Tree(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~Tree();

        /// Render the actual tree
        void doTreeRender() { renderImpl(); }

        /// UpdateScrollbars
        void doScrollbars() { updateScrollbar(); }

        /**
        @version NIIEngine 3.0.0
        */
        void addItem(TreeItem * item);

        /** 
        @version NIIEngine 3.0.0
         */
        void insertItem(TreeItem * item, const TreeItem * pos);

        /** 
        @version NIIEngine 3.0.0
        */
        void removeItem(const TreeItem * item);

        /**
        @version NIIEngine 3.0.0
        */
        void unselectAll();
        
        /** 
        @version NIIEngine 3.0.0
        */
        void removeAllItem();
        
        /** 
        @version NIIEngine 3.0.0
         */
        inline NCount getItemCount() const { return mItemList.size(); }

        /** 
        @version NIIEngine 3.0.0
         */
        NCount getSelectCount() const;

        /** 
        @version NIIEngine 3.0.0
        */
        TreeItem * getFirstSelect() const;
        
        /** 
        @version NIIEngine 3.0.0
         */
        TreeItem * getNextSelect(const TreeItem * begin) const;

        /** 
        @version NIIEngine 3.0.0
         */
        inline TreeItem * getLastSelect() const{ return mCurrentSelect; }

        /** 
        @version NIIEngine 3.0.0
         */
        void setSortEnable(bool set);
        
        /**
        @version NIIEngine 3.0.0
         */
        inline bool isSortEnable() const { return mSort; }

        /**
        @version NIIEngine 3.0.0
         */
        inline void setItemRenderArea(Rectf & r) { mItemArea = r; }

        /**
        @version NIIEngine 3.0.0
         */
        inline Scrollbar * getVScrollbar() { return mVScrollbar; }

        /**
        @version NIIEngine 3.0.0
         */
        inline Scrollbar * getHScrollbar() { return mHScrollbar; }

        /**
        @version NIIEngine 3.0.0
         */
        inline void setItemTipsEnable(bool set){ mTipsEnable = set;}
        
        /**
        @version NIIEngine 3.0.0
         */
        inline bool isItemTipsEnable() const { return mTipsEnable; }
        
        /**
        @version NIIEngine 3.0.0
         */
        TreeItem * getItem(const Vector2f & pt) const;

        /** 
        @version NIIEngine 3.0.0
        */
        TreeItem * getItem(const String & text);

        /**
        @version NIIEngine 3.0.0
        */
        TreeItem * getItem(const String & text, const TreeItem * begin);

        /** 
        @version NIIEngine 3.0.0
        */
        TreeItem * getItem(Nui32 id);
        
        /**
        @version NIIEngine 3.0.0
        */
        TreeItem * getItem(Nui32 id, const TreeItem * begin);

        /**
        @version NIIEngine 3.0.0
        */
        bool isItemExist(const TreeItem * item) const;
        
        /**
        @version NIIEngine 3.0.0
        */
        void visibleItem(const TreeItem * item);

        /**
        @version NIIEngine 3.0.0
         */
        void setMultiSelect(bool set);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isMultiSelect() const { return mMultiSelect; }

        /**
        @version NIIEngine 3.0.0
        */
        void SetForceVScrollbar(bool set);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isForceVScrollbar() const { return mForceVScroll; }

        /**
        @version NIIEngine 3.0.0
         */
        void SetForceHScrollbar(bool set);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isForceHScrollbar() const { return mForceHScroll; }

        /**
        @version NIIEngine 3.0.0
        */
        void setItemSelect(TreeItem * item, bool state);

        /**
        @verison NIIEngine 3.0.0
         */
        void setItemSelect(Nidx idx, bool state);
        
        /**
        @version NIIEngine 3.0.0
         */
        bool unselect(const TreeItemList & list);

        /**
        @version NIIEngine 3.0.0
        */
        void notifyItemUpdate();
        
        /// @copydetails Tree::setStyle
        virtual void setStyle(StyleID sid);

		static const WidgetID VScrollbarID;
		static const WidgetID HScrollbarID;

        static const StateID OpenView;
        static const StateID CloseTag;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::renderImpl
        virtual void renderImpl();

        /** 
        @version NIIEngine 3.0.0
         */
        virtual Rectf getTreeRenderArea() const
        { 
            return mItemArea;
        }

        /** 
        @version NIIEngine 3.0.0
        */
        virtual Scrollbar * createVertScrollbar(WidgetID wid) const
        {
            return static_cast<Scrollbar *>(get(wid, false));
        }

        /** 
        @version NIIEngine 3.0.0
        */
        virtual Scrollbar * createHorzScrollbar(WidgetID wid) const
        {
            return static_cast<Scrollbar *>(get(wid, false));
        }

        /**
        @version NIIEngine 3.0.0
        */
        void selectRange(Nidx start, Nidx end);

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getMaxHeight() const;
        
        /**
        @version NIIEngine 3.0.0
        */
        NIIf getMaxWidth() const;
        
        /**
        @version NIIEngine 3.0.0
        */
        void updateScrollbar();

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
        virtual void onSelect(const TreeEventArgs * arg);

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

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemUnfold(const TreeEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemClose(const TreeEventArgs * arg);
        
        /// @copydetails Widget::onFont
        virtual void onFont(const FontEventArgs * args);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);
        
        /// @copydetails Container::onSize
        virtual void onSize(const WidgetEventArgs * arg);
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
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ItemUnfoldEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ItemCloseEvent;

        static const EventID EventCount;
    protected:
        Scrollbar * mVScrollbar;
        Scrollbar * mHScrollbar;
        TreeItemList mItemList;
        Rectf mItemArea;
        TreeItem * mCurrentSelect;
        bool mSort;
        bool mMultiSelect;
        bool mForceVScroll;
        bool mForceHScroll;
        bool mTipsEnable;
        const StyleSection * mOpenButtonView;
        const StyleSection * mCloseButtonView;
    };

    /**
    @version NIIEngine 3.0.0
    */
    bool less(const TreeItem * a, const TreeItem * b);

    /**
    @version NIIEngine 3.0.0
    */
    bool greater(const TreeItem * a, const TreeItem * b);

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TreeView : public WidgetView
    {
    public:
        TreeView(WidgetModelID wmid);
        ~TreeView();

        static const WidgetID VScrollbarID;
        static const WidgetID HScrollbarID;
    protected:
        Rectf getTreeRenderArea(void) const;

        /// @copydetails WidgetView::flush
        void flush();
    };
}
}
#endif