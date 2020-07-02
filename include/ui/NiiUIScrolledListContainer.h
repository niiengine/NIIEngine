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
#ifndef _NII_UI_ScrolledListContainer_H_
#define _NII_UI_ScrolledListContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUIListContainer.h"

namespace NII
{
namespace UI
{
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScrolledListContainer : public ListContainer
    {
    public:
        ScrolledListContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_ScrolledListContainer, LangID lid = N_PrimaryLang);

        virtual ~ScrolledListContainer();

        /**
        @version NIIEngine 3.0.0
        */
        void SetForceVScrollbar(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        void SetForceHScrollbar(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isForceVScrollbar() const{  return mAlwayVScroll; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isForceHScrollbar() const{ return mAlwayHScroll; }

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
        void showVert(const ListItem & item);

        /**
        @version NIIEngine 3.0.0
        */
        void showHorz(const ListItem & item);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);
        
        /// @copydetails ListContainer::initChild
        virtual void initChild();

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void updateScrollbar(const PlaneSizef & size);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onVScrollbarMode(const WidgetEventArgs * arg);
        
        /**
        @version NIIEngine 3.0.0
        */
        virtual void onHScrollbarMode(const WidgetEventArgs * arg);

        void prcVScrollCB(const EventArgs * e);
        
        void prcHScrollCB(const EventArgs * e);
    public:
        static const WidgetID VScrollbarID;
        static const WidgetID HScrollbarID;

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
        bool mAlwayVScroll;
        bool mAlwayHScroll;
    };
}
}
#endif