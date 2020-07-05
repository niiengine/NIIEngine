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
#ifndef _NII_UI_TOOLTIP_H_
#define _NII_UI_TOOLTIP_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetView.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Tooltip : public Widget
    {
    public:
        Tooltip(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        ~Tooltip();

        /**
        @version NIIEngine 3.0.0
        */
        void setDest(Widget * obj);

        /** 
		@version NIIEngine 3.0.0
        */
        const Widget * getDest();

        /** 
        @version NIIEngine 3.0.0
        */
        void resetState();
        
        /**
        @version NIIEngine 3.0.0
        */
        void updatePos();

        /**
        @version NIIEngine 3.0.0
         */
        void setHoverTime(TimeDurMS ms);

        /**
		@version NIIEngine 3.0.0
        */
        inline TimeDurMS getHoverTime() const{ return mHoverTime; }

        /** 
        @version NIIEngine 3.0.0
        */
        void setDisplayTime(TimeDurMS ms);

        /**
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getDisplayTime() const{ return mShowTime;}

        /** 
        @version NIIEngine 3.0.0
        */
        PlaneSizef getViewSize() const;

        /** 
        @version NIIEngine 3.0.0
        */
        PlaneSizef getTextSize() const;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * view) const;

        /**
        @param arg
        */
        virtual void getHoverTime(const WidgetEventArgs * arg);

        /**
        @param arg
        */
        virtual void getShowTime(const WidgetEventArgs * arg);

        /**
        @param arg
        */
        virtual void onTooltipActive(const WidgetEventArgs * arg);

        /**
        @param arg
        */
        virtual void onTooltipInactive(const WidgetEventArgs * arg);

        /**
        @param arg
        */
        virtual void onTooltipTransition(const WidgetEventArgs * arg);

        /// @copydetails Widget::updateImpl
        void updateImpl(TimeDurMS cost);

        /// @copydetails Widget::onHide
        void onHide(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);

        /// @copydetails Widget::onText
        void onText(const WidgetEventArgs * arg);
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID HoverTimeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ShowTimeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID TooltipActiveEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID TooltipInactiveEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID TooltipDestEvent;

        static const EventID EventCount;
    protected:
        const Widget * mDest;
        TimeDurMS mCost;
        TimeDurMS mHoverTime;
        TimeDurMS mShowTime;
        bool mPosMutex;
        bool mShowState;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TooltipView : public WidgetView
    {
    public:
        TooltipView(WidgetViewlID wmid);

        /// @copydetails WidgetView::flush
        void flush();

        PlaneSizef getTextSize() const;
    };
}
}
#endif