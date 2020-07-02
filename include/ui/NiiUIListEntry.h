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

#ifndef _NII_UI_ITEMENTRY_H_
#define _NII_UI_ITEMENTRY_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListItemView : public WidgetView
    {
    public:
        ListItemView(WidgetModelID wmid);

        /**
        @version NIIEngine 3.0.0
        */
        virtual PlaneSizef getItemPixelSize() const = 0;

    };

    /** 项条目
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListItem : public Container
    {
        friend class ListContainer;
    public:
        ListItem(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_ListEntry, LangID lid = N_PrimaryLang);
        virtual ~ListItem();

        /**
        @version NIIEngine 3.0.0
        */
        void setSelectEnable(bool set);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSelectEnable() const{ return mSelectable; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline void select(bool set){ select(set, true); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void select(){ select(true, true); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void deselect(){ select(false, true); }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void select(bool state, bool notify);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSelect() const{ return mSelect; }

        /**
        @version NIIEngine 3.0.0
        */
        PlaneSizef getItemPixelSize() const;
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectChangeEvent;

        static const EventID EventCount;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;

        /// @copydetails Widget::onCursorTouch
        virtual void onCursorTouch(const CursorEventArgs * arg);

        /** 选中状态发生变化时触发
        @remark 变化后触发
        */
        virtual void onSelect(const WidgetEventArgs * arg);
    protected:
        bool mSelect;       ///< 选中状态
        bool mSelectable;   ///< 是否可选
    };
    
    /** 
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI DefaultListItemView : public ListItemView
    {
    public:
        DefaultListItemView(WidgetModelID wmid);

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails ListItemView::getItemPixelSize
        PlaneSizef getItemPixelSize() const;
    public:
        static const StateID SelectEnableState;
        static const StateID SelectDisableState;
        static const StateID StateCount;
    };
}
}
#endif