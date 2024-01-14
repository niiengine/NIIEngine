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

#include "NiiPreProcess.h"
#include "NiiUIListEntry.h"
#include "NiiUIListContainer.h"
#include "NiiUIException.h"
#include "NiiUICursorEventArgs.h"
#include "NiiUIWidgetManager.h"

#define ContentSize 1

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdSelectable : public PropertyCmd
    {
    public:
        CmdSelectable() : PropertyCmd(N_PropertyUIListEntry_SelectEnable, _T("selectable"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListItem *>(own)->isSelectEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListItem *>(own)->setSelectEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListItem *>(own)->isSelectEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListItem *>(own)->setSelectEnable(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectable();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdSelected : public PropertyCmd
    {
    public:
        CmdSelected() : PropertyCmd(N_PropertyUIListEntry_Select, _T("selected"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListItem *>(own)->isSelect(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListItem *>(own)->select(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListItem *>(own)->isSelect();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListItem *>(own)->select(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelected();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListItemView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ListItemView::ListItemView(WidgetViewlID wmid) :
        WidgetView(wmid, N_VIEW_ListEntry)
    {
    }
    //------------------------------------------------------------------------
    N_EVENT(Container,        ListItem,        SelectChangeEvent,     0);
    N_EVENT(Container,        ListItem,        EventCount,            1);
    //------------------------------------------------------------------------
    ListItem::ListItem(WidgetID wid, FactoryID fid, Container * own, const String & name,
        ScriptTypeID stid, LangID lid) :
        Container(wid, fid, own, name, stid, lid),
        mSelect(false),
        mSelectable(false)
    {
        notifySignal(ListItem::EventCount);
    }
    //------------------------------------------------------------------------
    ListItem::~ListItem()
    {

    }
    //------------------------------------------------------------------------
    PlaneSizef ListItem::getItemPixelSize() const
    {
        if(mWidgetView != 0)
        {
            return static_cast<ListItemView *>(mWidgetView)->getItemPixelSize();
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("This function must be implemented by the window renderer module"));
        }
    }
    //------------------------------------------------------------------------
    void ListItem::select(bool set, bool notify)
    {
        if(mSelectable && set != mSelect)
        {
            mSelect = set;
            ListContainer * temp = static_cast<ListContainer *>(mParent);
            // notify the ItemListbox if there is one that we just got selected
            // to ensure selection scheme is not broken when setting selection from code
            if(temp && notify)
            {
                temp->notifyItemSelect(this, set);
            }

            WidgetEventArgs args(this);
            onSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListItem::setSelectEnable(bool b)
    {
        if(mSelectable != b)
        {
            select(false);
            mSelectable = b;
        }
    }
    //------------------------------------------------------------------------
    bool ListItem::validImpl(const WidgetView * style) const
    {
        return dynamic_cast<const ListItemView *>(style) != 0;
    }
    //------------------------------------------------------------------------
    void ListItem::onCursorTouch(const CursorEventArgs * arg)
    {
        Widget::onCursorTouch(arg);

        if(mSelectable && arg->mMouseArg.mButton == MB_Left)
        {
            ListContainer * temp = static_cast<ListContainer *>(mParent);
            if(temp)
                temp->notifyItemClick(this);
            else
                select(!isSelect());
        }
    }
    //------------------------------------------------------------------------
    void ListItem::onSelect(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SelectChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    bool ListItem::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdSelectable());
        dest->add(N_new CmdSelected());
        return true;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // DefaultListItemView
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    N_STATE(WidgetView, DefaultListItemView, SelectEnableState,    0);
    N_STATE(WidgetView, DefaultListItemView, SelectDisableState,    1);
    N_STATE(WidgetView, DefaultListItemView, StateCount,            2);
    //------------------------------------------------------------------------
    DefaultListItemView::DefaultListItemView(WidgetViewlID wmid) :
        ListItemView(wmid)
    {
    }
    //------------------------------------------------------------------------
    void DefaultListItemView::flush()
    {
        ListItem * item = static_cast<ListItem *>(mSrc);
        const Style * style = getStyle();
        const StyleLayout * ss;

        if (item->isSelectEnable() && item->isSelect())
        {
            ss = &style->getLayout(item->isFinalDisable() ?
                SelectDisableState : SelectEnableState);
        }
        else
        {
            ss = &style->getLayout(item->isFinalDisable() ? DisableState : NormalState);
        }
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    PlaneSizef DefaultListItemView::getItemPixelSize() const
    {
        // get Style for the assigned look.
        return getStyle()->getSpecial(ContentSize).getArea().getPixelRect(*mSrc).getSize();
    }
    //------------------------------------------------------------------------
}
}