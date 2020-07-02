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

#ifndef _NII_UI_ListHeaderItem_H_
#define _NII_UI_ListHeaderItem_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiUIWidgetView.h"

namespace NII
{
namespace UI
{
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListHeaderItem : public Widget
    {
    public:
        /**
        @version NIIEngine 3.0.0
        */
        enum SortMode
        {
            SM_None,
            SM_Ascend,
            SM_Descend
        };
    public:
        ListHeaderItem(WidgetID wid, FactoryID fid, Container * own, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);
        virtual ~ListHeaderItem();

        /**
        @version NIIEngine 3.0.0
        */
        void setDragResize(bool set);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isDragResize() const {return mResizeEnable; }

        /**
        @version NIIEngine 3.0.0
        */
        void setSortMode(SortMode mode);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline SortMode getSortMode() const {return mSortMode;}

        /** 
        @version NIIEngine 3.0.0
        */
        void setDragMove(bool set);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isDragMove() const {return mDragMove;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const Vector2f & getDragOffset() const {return mDragOffset;}

        /**
        @version NIIEngine 3.0.0
        */
        void setClickEnable(bool set);
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isClickEnable() const {return mClickEnable;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void setSizingView(PixelBuffer * pb){ mSizingView = pb;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * getSizingView() const{ return mSizingView; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void setMovingView(PixelBuffer * pb){ mMovingView = pb;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * getMovingView() const { return mMovingView; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isItemHover() const {return mItemHover;}

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isItemClick() const {return mItemClick;}

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSplitterHover() const {return mSplitterHover;}

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isDragMoving() const {return mDragMoving;}

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isDragResizing() const {return mDragSizing;}
    public:
        /** 
        @version NIIEngine 3.0.0
         */
        static const EventID ItemClickEvent;
        
        /** 
        @version NIIEngine 3.0.0
         */
        static const EventID SplitterDClickEvent;
        
        /** 
        @version NIIEngine 3.0.0
         */
        static const EventID ResizeEnableEvent;
        
        /**
        @version NIIEngine 3.0.0
         */
        static const EventID SortModeEvent;
        
        /**
        @version NIIEngine 3.0.0
         */
        static const EventID MoveModeEvent;
        
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ClickModeEvent;
        
        /**
        @version NIIEngine 3.0.0
         */
        static const EventID ItemDragBeginEvent;
        
        /**
        @version NIIEngine 3.0.0
         */
        static const EventID ItemDragEndEvent;
        
        /** 
        @version NIIEngine 3.0.0
         */
        static const EventID ItemDragMoveEvent;
        
        /**
        @version NIIEngine 3.0.0
         */
        static const EventID ItemResizeEvent;
        
        static const EventID EventCount;
    protected:
        /// @copydetail PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemClick(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSplitterDClick(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onResizeMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSortMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onMoveMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemDragBegin(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemDragEnd(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemDragMove(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemResize(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onClickMode(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);
        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);
        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);
        /// @copydetails Widget::onCursorMultiTouch
        virtual void onCursorMultiTouch(const CursorEventArgs * arg);
        /// @copydetails Widget::onCursorLeave
        virtual void onCursorLeave(const CursorEventArgs * arg);
        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    protected:
        PixelBuffer * mSizingView;
        PixelBuffer * mMovingView;
        SortMode mSortMode;
        Vector2f mDragPos;
        Vector2f mDragOffset;
        NIIf mSplitterSize;
        bool mItemHover;
        bool mItemClick;
        bool mResizeEnable;
        bool mDragMove;
        bool mDragMoving;
        bool mSplitterHover;
        bool mDragSizing;
        bool mClickEnable;
    };
    typedef vector<ListHeaderItem *>::type ListHeaderItemList;

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListHeaderItemView : public WidgetView
    {
    public:
        ListHeaderItemView(WidgetModelID type);

        ///@copydetail WidgetView::flush
        void flush();
    public:
        static const StateID HoverState;
        static const StateID SplitterHoverState;
        static const StateID DragState;
        static const StateID AscIconState;
        static const StateID DescIconState;
        static const StateID DragAscIconState;
        static const StateID DragDescIconState;
        static const StateID StateCount;
    };
}
}
#endif