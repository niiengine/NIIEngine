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

#ifndef _NII_UI_MENUITEM_H_
#define _NII_UI_MENUITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiUIListEntry.h"

namespace NII
{
namespace UI
{
    /** 菜单专用项条目
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MenuItem : public ListItem
    {
    public:
        MenuItem(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~MenuItem();

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isOpen() const { return mOpen;}

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isHover() const{ return mOver; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isPress() const{ return mPress; }

        /** 
        @version NIIEngine 3.0.0
        */
        void toggleOpen();
        
        /** 
        @version NIIEngine 3.0.0
        */
        void toggleClose();

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isPopupClosing() const { return mPopupClosing; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isAutoPopup() const { return mAutoPopupTimeout > 0; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setAutoPopupTimeout(TimeDurMS ms) { mAutoPopupTimeout = ms; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getAutoPopupTimeout() const { return mAutoPopupTimeout; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setDest(PopupMenu * menu){ setDestImpl(menu); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline PopupMenu * getDest() const { return mDest; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setPopupOffset(const RelVector2f & oft) { mPopupOffset = oft; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const RelVector2f & getPopupOffset() const { return mPopupOffset; }

        /**
        @version NIIEngine 3.0.0
        */
        bool togglePopupMenu();

        /** 
        @version NIIEngine 3.0.0
        */
        void openPopupMenu(bool notify = true);

        /**
        @version NIIEngine 3.0.0
        */
        void closePopupMenu(bool notify = true);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::updateImpl
        virtual void updateImpl(TimeDurMS cost);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorLeave
        virtual void onCursorLeave(const CursorEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onClick(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onText(const WidgetEventArgs * arg);

        /** 
        @version NIIEngine 3.0.0
        */
        void closeAllPopup();

        /**
        @version NIIEngine 3.0.0
        */
        void setDestImpl(PopupMenu * popup, bool add_as_child = true);
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ClickEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /** 更新UI单元对象
        @param pos UI体系中的位
        */
        void update(const Vector2f & cursorpos);
    protected:
        PopupMenu * mDest;
        TimeDurMS mAutoPopupTimeout;
        TimeDurMS mAutoPopupCost;
        RelVector2f mPopupOffset;
        bool mPress;       
        bool mOver;         
        bool mOpen;
        bool mClose;
        bool mPopupClosing;
        bool mPopupOpening;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MenuItemView : public ListItemView
    {
    public:
        MenuItemView(WidgetViewlID wsid);

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails ListItemView::getItemPixelSize
        PlaneSizef getItemPixelSize() const;
    protected:
        /// @copydetails WidgetView::notifyFont
        bool notifyFont(const Font * obj);
    public:
        static const StateID EnableHoverState;
        static const StateID EnablePushState;
        static const StateID EnablePushOffState;
        static const StateID EnablePopupOpenState;
        static const StateID DisableHoverState;
        static const StateID DisablePushState;
        static const StateID DisablePushOffState;
        static const StateID DisablePopupOpenState;
        static const StateID PopupCloseIconState;
        static const StateID PopupOpenIconState;
        static const StateID StateCount;
    protected:
        const StyleSpecial & getContentNamedArea() const;
    };
}
}
#endif