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
#include "NiiUIItemListbox.h"
#include "NiiUIException.h"
#include "NiiUIScrollbar.h"
#include "NiiUIWidgetManager.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdMultiSelect : public PropertyCmd
    {
    public:
        CmdMultiSelect() : PropertyCmd(N_PropertyUIItemListbox_MultiSelectEnable, _T("multi_select"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ItemListbox *>(own)->isMultiSelect(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ItemListbox *>(own)->setMultiSelect(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ItemListbox *>(own)->isMultiSelect();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ItemListbox *>(own)->setMultiSelect(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMultiSelect();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(ScrolledListContainer, ItemListbox, SelectEvent,                    0);
    N_EVENT(ScrolledListContainer, ItemListbox, MultiSelectModeChangeEvent,     1);
    N_EVENT(ScrolledListContainer, ItemListbox, EventCount,                     2);
    //------------------------------------------------------------------------
    ItemListbox::ItemListbox(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        ScrolledListContainer(wid, fid, own, name, N_CmdObj_ItemListbox, lid),
        mMultiSelect(false),
        mLastSelect(0),
        mNextSelect(0)
    {
        notifySignal(ItemListbox::EventCount);
        setupCmd(N_CmdObj_ItemListbox);
    }
    //------------------------------------------------------------------------
    ItemListbox::~ItemListbox()
    {
    }
    //------------------------------------------------------------------------
    void ItemListbox::layoutContent()
    {
        NIIf y = 0;
        NIIf widest = 0;

        ListEntryList::iterator i, iend = mItemList.end();
        for(i = mItemList.begin(); i != iend; ++i)
        {
            ListItem * entry = *i;
            const PlaneSizef pxs = entry->getItemPixelSize();
            if(pxs.mWidth > widest)
            {
                widest = pxs.mWidth;
            }

            entry->setRelArea(RelRectf(RelPosf(0, 0), RelPosf(0, y),
                PlaneSize<RelPosf>(RelPosf(1, 0), RelPosf(0, pxs.mHeight))));

            y += pxs.mHeight;
        }
        // reconfigure scrollbars
        updateScrollbar(PlaneSizef(widest, y));
    }
    //------------------------------------------------------------------------
    PlaneSizef ItemListbox::getContentSize() const
    {
        NIIf h = 0;

        ListEntryList::const_iterator i = mItemList.begin();
        ListEntryList::const_iterator end = mItemList.end();
        while (i != end)
        {
            h += (*i)->getItemPixelSize().mHeight;
            ++i;
        }
        return PlaneSizef(getItemArea().width(), h);
    }
    //------------------------------------------------------------------------
    NCount ItemListbox::getSelectCount() const
    {
        if (!mMultiSelect)
        {
            return mLastSelect ? 1 : 0;
        }

        NCount count = 0;
        NCount i, iend = mItemList.size();
        for (i = 0; i < iend; ++i)
        {
            if (mItemList[i]->isSelect())
            {
                ++count;
            }
        }
        return count;
    }
    //------------------------------------------------------------------------
    ListItem * ItemListbox::findSelect(Nidx idx) const
    {
        NCount i, iend = mItemList.size();
        for (i = idx; i < iend; ++i)
        {
            ListItem * li = mItemList[i];
            if (li->isSelect())
            {
                mNextSelect = i + 1;
                return li;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    ListItem * ItemListbox::getFirstSelect(Nidx idx) const
    {
        if(!mMultiSelect)
        {
            return mLastSelect;
        }
        return findSelect(idx);
    }
    //------------------------------------------------------------------------
    ListItem * ItemListbox::getNextSelect() const
    {
        if (!mMultiSelect)
        {
            return 0;
        }
        return findSelect(mNextSelect);
    }
    //------------------------------------------------------------------------
    /*ListItem * ItemListbox::getNextSelectedItemAfter(const ListItem * start_item) const
    {
        if (start_item==0||!mMultiSelect)
        {
            return 0;
        }

        NCount max = mItemList.size();
        NCount i = getItemIndex(start_item);

        while (i < max)
        {
            ListItem * li = mItemList[i];
            if (li->isSelect())
            {
                return li;
            }
            ++i;
        }
        return 0;
    }*/
    //------------------------------------------------------------------------
    void ItemListbox::setMultiSelect(bool state)
    {
        if (state != mMultiSelect)
        {
            mMultiSelect = state;
            WidgetEventArgs e(this);
            onMultiSelectMode(&e);
        }
    }
    //------------------------------------------------------------------------
    void ItemListbox::notifyItemClick(ListItem * li)
    {
        bool sel_state = !(li->isSelect() && mMultiSelect);
        bool skip = false;

        // multiselect enabled
        if(mMultiSelect)
        {
            Nmark syskeys = mSheet->getWinComKey();
            ListItem * last = mLastSelect;

            // no KT_A_Ctrl? clear others
            if(!(syskeys & KT_A_Ctrl))
            {
                unselectAll();
                if (!sel_state)
                {
                    sel_state=true;
                }
            }

            // select range if Shift if held, and we have a 'last selection'
            if(last && (syskeys & KT_A_Shift))
            {
                selectRange(getItemIndex(last), getItemIndex(li));
                skip = true;
            }
        }
        else
        {
            unselectAll();
        }

        if (!skip)
        {
            li->select(sel_state, false);
            if (sel_state)
            {
                mLastSelect = li;
            }
            else if (mLastSelect == li)
            {
                mLastSelect = 0;
            }
        }
        WidgetEventArgs e(this);
        onSelect(&e);
    }
    //------------------------------------------------------------------------
    void ItemListbox::notifyItemSelect(ListItem * li, bool state)
    {
        // deselect
        if (!state)
        {
            // clear last selection if this one was it
            if (mLastSelect == li)
            {
                mLastSelect = 0;
            }
        }
        // if we dont support multiselect, we must clear all the other selections
        else if (!mMultiSelect)
        {
            unselectAll();
            li->select(true, false);
            mLastSelect = li;
        }
        WidgetEventArgs e(this);
        onSelect(&e);
    }
    //------------------------------------------------------------------------
    bool ItemListbox::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdMultiSelect());
        return true;
    }
    //------------------------------------------------------------------------
    bool ItemListbox::isSelect(Nidx index) const
    {
        if (index >= mItemList.size())
        {
            N_EXCEPT(InvalidRequest, _I18n("The index given is out of range for this ItemListbox"));
        }
        ListItem * li = mItemList[index];
        return li->isSelect();
    }
    //------------------------------------------------------------------------
    void ItemListbox::unselectAll()
    {
        NCount i, max = mItemList.size();
        for (i = 0; i < max; ++i)
        {
            mItemList[i]->select(false, false);
        }
        mLastSelect = 0;

        WidgetEventArgs e(this);
        onSelect(&e);
    }
    //------------------------------------------------------------------------
    void ItemListbox::selectRange(Nidx a, Nidx z)
    {
        // do nothing if the list is empty
        if (mItemList.empty())
        {
            return;
        }

        NCount max = mItemList.size();
        if (a >= max)
        {
            a = 0;
        }
        if (z >= max)
        {
            z = max-1;
        }

        if (a > z)
        {
            NCount tmp = a;
            a = z;
            z = tmp;
        }

        for (NCount i = a; i <= z; ++i)
        {
            mItemList[i]->select(true, false);
        }

        mLastSelect = mItemList[z];
        WidgetEventArgs e(this);
        onSelect(&e);
    }
    //------------------------------------------------------------------------
    void ItemListbox::selectAll()
    {
        if (!mMultiSelect)
        {
            return;
        }
        NCount i, max = mItemList.size();
        for (i = 0; i < max; ++i)
        {
            mLastSelect = mItemList[i];
            mLastSelect->select(true, false);
        }
        WidgetEventArgs e(this);
        onSelect(&e);
    }
    //------------------------------------------------------------------------
    void ItemListbox::onSelect(const WidgetEventArgs * arg)
    {
        signal(SelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void ItemListbox::onMultiSelectMode(const WidgetEventArgs * arg)
    {
        signal(SelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void ItemListbox::onKeyDown(const KeyEventArgs * arg)
    {
        ScrolledListContainer::onKeyDown(arg);
        if(mMultiSelect)
        {
            if(arg->mScanCode == Key::A && (mSheet->getWinComKey() & WCK_KT_A_Ctrl))
            {
                selectAll();
            }
        }
    }
    //------------------------------------------------------------------------
    void ItemListbox::onItemRmove(const EventArgs * e)
    {
        ListContainer::onItemRmove(e);

        // get the window that's being removed
        const Widget * w = static_cast<const WidgetEventArgs *>(e)->mWidget;
        // Clear last selected pointer if that item was just removed.
        if (w == mLastSelect)
            mLastSelect = 0;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ItemListboxView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(ListContainerView, ItemListboxView, AreaState, 0);
    N_STATE(ListContainerView, ItemListboxView, VScrollState, 1);
    N_STATE(ListContainerView, ItemListboxView, HScrollState, 2);
    N_STATE(ListContainerView, ItemListboxView, HVScrollState, 3);
    N_STATE(ListContainerView, ItemListboxView, AmiAreaState, 4);
    N_STATE(ListContainerView, ItemListboxView, AmiVScrollState, 5);
    N_STATE(ListContainerView, ItemListboxView, AmiHScrollState, 6);
    N_STATE(ListContainerView, ItemListboxView, AmiHVScrollState, 7);
    N_STATE(ListContainerView, ItemListboxView, StateCount, 8);
    //------------------------------------------------------------------------
    ItemListboxView::ItemListboxView(WidgetViewlID wsid) :
        ListContainerView(wsid),
        mStyleValid(false)
    {
    }
    //------------------------------------------------------------------------
    void ItemListboxView::flush()
    {
        // get Style for the assigned look.
        const Style * style = getStyle();
        // try and get mode for our current state
        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        // peform the rendering operation.
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    Rectf ItemListboxView::getRenderArea() const
    {
        ItemListbox * lb = static_cast<ItemListbox *>(mSrc);
        return getRenderArea(lb->getHScrollbar()->isVisible(), lb->getVScrollbar()->isVisible());
    }
    //------------------------------------------------------------------------
    Rectf ItemListboxView::getRenderArea(bool hscroll, bool vscroll) const
    {
        const ItemListbox * const lb = static_cast<ItemListbox *>(mSrc);
        const Style * style = getStyle();
        StateID sid, sid2;
        if (vscroll)
        {
            if (hscroll)
            {
                sid = HScrollState;
                sid2 = AmiHVScrollState;
            }
            else
            {
                sid = VScrollState;
                sid2 = AmiVScrollState;
            }
        }
        else
        {
            if (hscroll)
            {
                sid = HScrollState;
                sid2 = AmiHScrollState;
            }
            else
            {
                sid = AreaState;
                sid2 = AmiAreaState;
            }
        }
        if (style->isSpecialExist(sid))
        {
            return style->getSpecial(sid).getArea().getPixelRect(*lb);
        }
        if (style->isSpecialExist(sid2))
        {
            return style->getSpecial(sid2).getArea().getPixelRect(*lb);
        }

        if (style->isSpecialExist(AreaState))
            return style->getSpecial(AreaState).getArea().getPixelRect(*lb);
        else
            return style->getSpecial(AmiAreaState).getArea().getPixelRect(*lb);
    }
    //------------------------------------------------------------------------
    void ItemListboxView::onStyleAttach()
    {
        mStyleValid = true;
    }
    //------------------------------------------------------------------------
    void ItemListboxView::onStyleDetach()
    {
        mStyleValid = false;
    }
    //------------------------------------------------------------------------
    Rectf ItemListboxView::getArea() const
    {
        if (!mStyleValid)
            return mSrc->getArea().getIntCache();

        Rectf lr(getRenderArea());
        lr += mSrc->getArea().getIntCache().min();
        return lr;
    }
    //------------------------------------------------------------------------
}
}