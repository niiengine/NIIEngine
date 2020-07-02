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

#ifndef _NII_UI_ComboDropList_H_
#define _NII_UI_ComboDropList_H_

#include "NiiUIPreInclude.h"
#include "NiiUIListbox.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ComboDropList : public Listbox
    {
    public:
        ComboDropList(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~ComboDropList();

        /**
        @version NIIEngine 3.0.0
        */
        inline void setAutoDropDown(bool b){ mAutoDropDown = b; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDropDown() const { return mAutoDropDown;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void setDropDown(bool b) {mDropDown = b; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isDropDown() const {return mDropDown; }

        /**
        @version NIIEngine 3.0.0
        */
        void fit(bool width, bool height);
    protected:
        /// @copydetails ListBox::initChild
        virtual void initChild();
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectAcceptEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);

        /// @copydetails Widget::onActivate
        virtual void onActivate(const ActivationEventArgs * arg);

		/// @copydetails Listbox::onSelect
		virtual void onSelect(const WidgetEventArgs * arg);

        virtual void onItemList(const WidgetEventArgs * arg);

        virtual void onSelectAccept(const WidgetEventArgs * arg);
    protected:
        ListboxItem * mSelectAccept;
        bool mAutoDropDown;
        bool mDropDown;
    };
}
}
#endif