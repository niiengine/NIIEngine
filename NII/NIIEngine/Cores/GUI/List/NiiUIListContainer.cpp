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
#include "NIIUIListContainer.h"
#include "NiiUIException.h"
#include "NiiUIWidgetManager.h"
#include "NIIUIListEntry.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdAutoResizeEnabled : public PropertyCmd
    {
    public:
        CmdAutoResizeEnabled() : PropertyCmd(N_PropertyUIListContainer_AutoResize, _T("auto_resize_enabled"),
            _T("Property to get/set the state of the auto resizing enabled setting for the ListContainer.  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListContainer *>(own)->isAutoResize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListContainer *>(own)->setAutoResize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListContainer *>(own)->isAutoResize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListContainer *>(own)->setAutoResize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoResizeEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdSortMode : public PropertyCmd
    {
    public:
        CmdSortMode() : PropertyCmd(N_PropertyUIListContainer_SortMode, _T("sort_mode"),
            _T("Property to get/set the sorting mode for the ListContainer.  Value is either \"SM_Ascend\", \"SM_Descend\" or \"SM_Custom\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ListContainer::SortMode val = static_cast<const ListContainer *>(own)->getSortMode();
            if(val == ListContainer::SM_Ascend)
            {
                out = _T("SM_Ascend");
            }
            else if(val == ListContainer::SM_Descend)
            {
                out = _T("SM_Descend");
            }
            else if(val == ListContainer::SM_Custom)
            {
                out = _T("SM_Custom");
            }
            else if(val == ListContainer::SM_None)
            {
                out = _T("SM_None");
            }
            else
            {
                out = _T("SM_Ascend");
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & str)
        {
            ListContainer::SortMode val;
            if(str == _T("SM_Ascend"))
            {
                val = ListContainer::SM_Ascend;
            }
            else if(str == _T("SM_Descend"))
            {
                val = ListContainer::SM_Descend;
            }
            else if(str == _T("SM_None"))
            {
                val = ListContainer::SM_None;
            }
            else
            {
                val = ListContainer::SM_Custom;
            }

            static_cast<ListContainer *>(own)->setSortMode(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ListContainer::SortMode * temp = (ListContainer::SortMode *)out;
            *temp = static_cast<const ListContainer *>(own)->getSortMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ListContainer::SortMode * temp = (ListContainer::SortMode *)in;
            static_cast<ListContainer *>(own)->setSortMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSortMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("SM_None");
        }
    };
    //------------------------------------------------------------------------
    static bool ListEntry_less(const ListItem * a, const ListItem * b)
    {
        return a->getText() < b->getText();
    }
    //------------------------------------------------------------------------
    static bool ListEntry_greater(const ListItem * a, const ListItem * b)
    {
        return (a->getText() > b->getText());
    }
    //------------------------------------------------------------------------
    N_EVENT(Container, ListContainer, ItemListEvent,            0);
    N_EVENT(Container, ListContainer, SortModeEvent,            1);
    N_EVENT(Container, ListContainer, EventCount,               2);
    //------------------------------------------------------------------------
    ListContainer::ListContainer(WidgetID wid, FactoryID fid, Container * own, const String & name,
        ScriptTypeID stid, LangID lid) :
        Container(wid, fid, own, name, stid, lid),
        mSortMode(SM_None),
        mSortFunc(0),
        mAutoResize(false),
        mResort(false)
    {
        notifySignal(ListContainer::EventCount);
        // by default we dont have a content pane, but to make sure things still work
        // we "emulate" it by setting it to this
        mContainerArea = this;

        setupCmd(N_CmdObj_ListContainer);
    }
    //------------------------------------------------------------------------
    ListContainer::~ListContainer()
    {
    }
    //------------------------------------------------------------------------
    void ListContainer::initChild()
    {
        // this pane may be ourselves, and in fact is by default...
        mContainerArea->bind(Container::RemoveEvent, this, (CommandObjFunctor)&ListContainer::onItemRmove);
    }
    //------------------------------------------------------------------------
    ListItem * ListContainer::getItem(Nidx index) const
    {
        if(index < mItemList.size())
        {
            return mItemList[index];
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified index is out of range for this ListContainer."));
        }
    }
    //------------------------------------------------------------------------
    Nidx ListContainer::getItemIndex(const ListItem * item) const
    {
        ListEntryList::const_iterator pos = std::find(mItemList.begin(), mItemList.end(), item);

        if(pos != mItemList.end())
        {
            return std::distance(mItemList.begin(), pos);
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified ListItem is not attached to this ListContainer."));
        }
    }
    //------------------------------------------------------------------------
    ListItem * ListContainer::findItem(Nidx index, const String & text)
    {
        while(index < mItemList.size())
        {
            if(mItemList[index]->getText() == text)
            {
                return mItemList[index];
            }
            // no matching text, advance to next item
            else
            {
                index++;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ListContainer::setAutoResize(bool b)
    {
        bool old = mAutoResize;
        mAutoResize = b;

        // if not already enabled, trigger a resize - only if not currently initialising
        if(mAutoResize && !old && !mInit)
        {
            autoResizeImpl();
        }
    }
    //------------------------------------------------------------------------
    bool ListContainer::isItemExist(const ListItem * item) const
    {
        //return std::find(mItemList.begin(), mItemList.end(), item) != mItemList.end();
        return (item->mParent == this);
    }
    //------------------------------------------------------------------------
    void ListContainer::removeAllItem()
    {
        if(removeAllImpl())
        {
            notifyItemUpdate();
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::addItem(ListItem * item)
    {
        // make sure the item is valid and that we dont already have it in our list
        if (item && item->mParent != this)
        {
            // if sorting is enabled, re-sort the list
            if(mSortMode != SM_None)
            {
                mItemList.insert(std::upper_bound(mItemList.begin(),
                    mItemList.end(), item, mSortFunc), item);
            }
            // just stick it on the end.
            else
            {
                mItemList.push_back(item);
            }
            // make sure it gets added properly
            item->mParent = this;
            add(item);
            notifyItemUpdate();
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::insertItem(ListItem * item, const ListItem * position)
    {
        if(mSortMode != SM_None)
        {
            addItem(item);
        }
        else if(item && item->mParent != this)
        {
            // if position is NULL begin insert at begining, else insert after item 'position'
            ListEntryList::iterator ins_pos;

            if(!position)
            {
                ins_pos = mItemList.begin();
            }
            else
            {
                ins_pos = std::find(mItemList.begin(), mItemList.end(), position);

                // throw if item 'position' is not in the list
                if (ins_pos == mItemList.end())
                {
                    N_EXCEPT(InvalidRequest,
                        _I18n("the specified ListItem for parameter 'position' is not attached to this ListContainer."));
                }

            }

            mItemList.insert(ins_pos, item);
            item->mParent = this;
            add(item);

            notifyItemUpdate();
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::removeItem(ListItem * item)
    {
        if(item && item->mParent == this)
        {
            mContainerArea->remove(item);
            if(item->isAutoDestroy())
            {
                N_Only(Widget).destroy(item);
            }
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::notifyItemClick(ListItem *)
    {
    }
    //------------------------------------------------------------------------
    void ListContainer::notifyItemSelect(ListItem *, bool)
    {
    }
    //------------------------------------------------------------------------
    void ListContainer::notifyItemUpdate(bool resort)
    {
        if(!mDestroy)
        {
            mResort |= resort;
            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::onItemList(const WidgetEventArgs * arg)
    {
        // if we are not currently initialising we might have things todo
        if(!mInit)
        {
            refresh();

            // if auto resize is enabled - do it
            if(mAutoResize)
                autoResizeImpl();

            // resort list if requested and enabled
            if(mResort && mSortMode != SM_None)
                sortList(false);
            mResort = false;

            // redo the item layout and fire our event
            layoutContent();
            signal(ItemListEvent, arg);
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::onParentSize(const WidgetEventArgs * arg)
    {
        Widget::onParentSize(arg);

        if(mAutoResize)
            autoResizeImpl();
    }
    //------------------------------------------------------------------------
    bool ListContainer::removeAllImpl()
    {
        // just return false if the list is already empty
        if(mItemList.empty())
        {
            return false;
        }
        // we have items to be removed and possible deleted
        else
        {
            // delete any items we are supposed to
            while(!mItemList.empty())
            {
                ListItem * item = mItemList[0];
                mContainerArea->remove(item);
                if(item->isAutoDestroy())
                {
                    N_Only(Widget).destroy(item);
                }
            }
            return true;
        }
    }
    //------------------------------------------------------------------------
    bool ListContainer::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdAutoResizeEnabled());
        dest->add(N_new CmdSortMode());
        return true;
    }
    //------------------------------------------------------------------------
    void ListContainer::addImpl(Widget * sub)
    {
        ListItem * item = dynamic_cast<ListItem *>(sub);

        // if this is an ListItem we add it like one, but only if it is not already in the list!
        if (item)
        {
            // add to the pane if we have one
            if (mContainerArea != this)
            {
                mContainerArea->add(item);
            }
            // add item directly to us
            else
            {
                Container::addImpl(item);
            }

            if (item->mParent != this)
            {
                // perform normal addItem
                // if sorting is enabled, re-sort the list
                if (mSortMode != SM_None)
                {
                    mItemList.insert(std::upper_bound(mItemList.begin(),
                        mItemList.end(), item, mSortFunc), item);
                }
                // just stick it on the end.
                else
                {
                    mItemList.push_back(item);
                }
                item->mParent = this;
                notifyItemUpdate();
            }
        }
        // otherwise it's base class processing
        else
        {
            Container::addImpl(sub);
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::onEndInit(void)
    {
        Widget::onEndInit();
        notifyItemUpdate(true);
    }
    //------------------------------------------------------------------------
    void ListContainer::layout()
    {
        Container::layout();
        // if we are not currently initialising
        if(!mInit)
        {
            // Redo the item layout.
            // We don't just call handleUpdateItemData, as that could trigger a resize,
            // which is not what is being requested.
            // It would also cause infinite recursion... so lets just avoid that :)
            layoutContent();
        }
    }
    //------------------------------------------------------------------------
    void ListContainer::autoResizeImpl()
    {
        Rectf renderArea(getItemArea());
        Rectf wndArea(abs(getRelArea(), calcParentArea()));

        // get size of content
        PlaneSizef sz(getContentSize());

        // calculate the full size with the frame accounted for and resize the window to this
        sz.mWidth  += wndArea.getWidth() - renderArea.getWidth();
        sz.mHeight += wndArea.getHeight() - renderArea.getHeight();
        setSize(RelPlaneSizef(RelPosf(0, sz.mWidth), RelPosf(0, sz.mHeight)));
    }
    //------------------------------------------------------------------------
    Rectf ListContainer::getItemArea() const
    {
        if(mWidgetView != 0)
        {
            ListContainerView * wr = static_cast<ListContainerView *>(mWidgetView);
            return wr->getRenderArea();
        }
        else
        {
            //return getItemRenderArea_impl();
            N_EXCEPT(InvalidRequest, _I18n("This function must be implemented by the window renderer module"));
        }
    }
    //-----------------------------------------------------------------------
    void ListContainer::onItemRmove(const EventArgs * e)
    {
        Widget * wnd = static_cast<const WidgetEventArgs *>(e)->mWidget;

        // make sure it is removed from the itemlist if we have an ListItem
        ListItem * item = dynamic_cast<ListItem *>(wnd);
        if(item)
        {
            ListEntryList::iterator pos = std::find(mItemList.begin(), mItemList.end(), item);
            if (pos != mItemList.end())
            {
                // make sure the item is no longer related to us
                (*pos)->mParent = 0;
                // remove item
                mItemList.erase(pos);
                // trigger list update
                notifyItemUpdate();
            }
        }
    }
    //-----------------------------------------------------------------------
    void ListContainer::setSortFunc(SortFunc cb)
    {
        if (mSortFunc != cb)
        {
            mSortFunc = cb;
            if (mSortMode != SM_None && !mInit)
            {
                sortList();
            }
            notifyItemUpdate(true);
        }
    }
    //-----------------------------------------------------------------------
    void ListContainer::onSortMode(const WidgetEventArgs * arg)
    {
        signal(SortModeEvent, arg);
    }
    //-----------------------------------------------------------------------
    void ListContainer::sortList(bool relayout)
    {
        std::sort(mItemList.begin(), mItemList.end(), mSortFunc);
        if(relayout)
        {
            layoutContent();
        }
    }
    //----------------------------------------------------------------------
    bool ListContainer::validImpl(const WidgetView * model) const
    {
        return dynamic_cast<const ListContainerView *>(model) != 0;
    }
    //----------------------------------------------------------------------
    void ListContainer::setSortMode(SortMode mode)
    {
        if(mSortMode != mode)
        {
            mSortMode = mode;
            switch(mSortMode)
            {
            case SM_Ascend:
                mSortFunc = &ListEntry_less;
            case SM_Descend:
                mSortFunc = &ListEntry_greater;
            case SM_Custom:
                mSortFunc = (mSortFunc != 0) ? mSortFunc : &ListEntry_less;
            default:
                mSortFunc = &ListEntry_less;
            }

            if(mSortMode != SM_None && !mInit)
                sortList();

            WidgetEventArgs e(this);
            onSortMode(&e);
        }

    }
    //----------------------------------------------------------------------
}
}
