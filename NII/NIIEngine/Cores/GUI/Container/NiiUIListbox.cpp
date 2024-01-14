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
#include "NiiUIException.h"
#include "NiiUIListbox.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIManager.h"
#include "NiiUIContainer.h"
#include "NiiUIScrollbar.h"
#include "NiiUISheet.h"
#include "NiiFontManager.h"
#include "NiiPixelBuffer.h"
#include "NiiUIScrollbar.h"
#include "NiiUITooltip.h"
#include "NiiPixelManager.h"
#include "NiiColour.h"

#if defined (CEGUI_USE_FRIBIDI)
#include "NiiUIFribidiText.h"
#elif defined (CEGUI_USE_MINIBIDI)
#include "NiiUIMinibidiText.h"
#else
#include "NiiUIBidiText.h"
#endif

#define ItemRenderingArea           0 
#define ItemRenderingAreaHVScroll   1
#define ItemRenderingAreaVScroll    2
#define ItemRenderingAreaHScroll    3
#define ItemRenderArea              4
#define ItemRenderAreaHVScroll      5
#define ItemRenderAreaVScroll       6
#define ItemRenderAreaHScroll       7
    
    
namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ListboxItem::ListboxItem(const String & text, Nui32 item_id, void * data,
        bool disable, bool autoDestroy) :
#ifndef CEGUI_BIDI_SUPPORT
        mBidiText(0),
#elif defined (CEGUI_USE_FRIBIDI)
        mBidiText(N_new FribidiText),
#elif defined (CEGUI_USE_MINIBIDI)
        mBidiText(N_new MinibidiText),
#else
#error "BIDI Configuration is inconsistant, check your config!"
#endif
        mID(item_id),
        mCustomData(data),
        mSelect(false),
        mDisable(disable),
        mAutoDestroy(autoDestroy),
        mParent(0),
        mSelectColour(WidgetManager::DefaultSelectionColour),
        mSelectView(0)
    {
        setText(text);
    }
    //------------------------------------------------------------------------
    ListboxItem::~ListboxItem()
    {
        N_delete mBidiText;
    }
    //------------------------------------------------------------------------
    bool ListboxItem::operator< (const ListboxItem & o) const
    {
        return getText() < o.getText();
    }
    //------------------------------------------------------------------------
    bool ListboxItem::operator> (const ListboxItem & o) const
    {
        return getText() > o.getText();
    }
    //------------------------------------------------------------------------
    void ListboxItem::setText(const String & text)
    {
        mRawText = text;
        mValidBidi = false;
    }
    //------------------------------------------------------------------------
    const String & ListboxItem::getVisualText() const
    {
        if (!mBidiText)
            return mRawText;

        if (!mValidBidi)
        {
            mBidiText->setSrc(mRawText);
            mValidBidi = true;
        }

        return mBidiText->getDst();
    }
    //------------------------------------------------------------------------
    bool ListboxItem::notifyFont(const Font * const)
    {
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ListboxTextItem::ListboxTextItem(const String & text, Nui32 id,
        void * data, bool disable, bool autoDestroy) :
        ListboxItem(text, id, data, disable, autoDestroy),
        mColour(WidgetManager::DefaultTextColour),
        mFont(0),
        mGridValid(false),
        mTextLayout(true)
    {
    }
    //------------------------------------------------------------------------
    void ListboxTextItem::setFont(Font * font)
    {
        mFont = font;
        mGridValid = false;
    }
    //------------------------------------------------------------------------
    Font * ListboxTextItem::getFont() const
    {
        Font * re;
        if (mFont)
        {
            re = const_cast<Font *>(mFont);
        }
        else if (mParent)
        {
            re = mParent->getFont();
        }
        else
        {
            re = N_Only(UI).getMainSheet()->getFont();
        }
        return re;
    }
    //------------------------------------------------------------------------
    void ListboxTextItem::setColour(const ColourArea & cols)
    {
        mColour = cols;
        mGridValid = false;
    }
    //------------------------------------------------------------------------
    PlaneSizef ListboxTextItem::getPixelSize() const
    {
        const Font * fnt = getFont();

        if (!fnt)
            return PlaneSizef(0, 0);

        if (!mGridValid)
        {
            if (mTextLayout)
                mPixelGrid = WidgetManager::gDefaultTextView->parse(getVisualText(), getFont(), &mColour);
            else
                mPixelGrid = WidgetManager::gNormalTextView->parse(getVisualText(), getFont(), &mColour);

            mGridValid = true;
        }
        PlaneSizef sz(0.0f, 0.0f);
        NCount i, iend = mPixelGrid.getLineCount();
        for (i = 0; i < iend; ++i)
        {
            const PlaneSizef line_sz(mPixelGrid.getPixelSize(mParent, i));
            sz.mHeight += line_sz.mHeight;

            if (line_sz.mWidth > sz.mWidth)
                sz.mWidth = line_sz.mWidth;
        }

        return sz;
    }
    //------------------------------------------------------------------------
    void ListboxTextItem::draw(GeometryPixel & buffer, const Rectf & targetRect,
        NIIf alpha, const Rectf * clipper) const
    {
        if (mSelect && mSelectView != 0)
        {
            ColourArea tc = mSelectColour;
            tc.modulate(alpha);
            buffer.add(*mSelectView, targetRect, clipper, tc);
        }

        const Font * font = getFont();
        if (font)
        {
            Vector2f draw_pos(targetRect.getPos());

            draw_pos.y += N_ToPixel((font->getLineSpacing() - font->getFontHeight()) * 0.5f);

            if (!mGridValid)
            {
                if (mTextLayout)
                    mPixelGrid = WidgetManager::gDefaultTextView->parse(getVisualText(), getFont(), &mColour);
                else
                    mPixelGrid = WidgetManager::gNormalTextView->parse(getVisualText(), getFont(), &mColour);

                mGridValid = true;
            }
            ColourArea final_colours(0xFFFFFFFF);
            final_colours.modulate(alpha);
            NCount i, iend = mPixelGrid.getLineCount();
            for (i = 0; i < iend; ++i)
            {
                mPixelGrid.draw(mParent, i, buffer, draw_pos, &final_colours, clipper, 0.0f);
                draw_pos.y += mPixelGrid.getPixelSize(mParent, i).mHeight;
            }
        }
    }
    //------------------------------------------------------------------------
    void ListboxTextItem::setText(const String & text)
    {
        ListboxItem::setText(text);
        mGridValid = false;
    }
    //------------------------------------------------------------------------
    void ListboxTextItem::setTextLayout(bool set)
    {
        mTextLayout = set;
        mGridValid = false;
    }
    //------------------------------------------------------------------------
    bool ListboxTextItem::notifyFont(const Font * font)
    {
        return getFont() == font;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdSort : public PropertyCmd
    {
    public:
        CmdSort() : PropertyCmd(N_PropertyUIListbox_SortEnable, _T("sort"), _I18n("sort"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Listbox *>(own)->isSortEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Listbox *>(own)->setSortEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Listbox *>(own)->isSortEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Listbox *>(own)->setSortEnable(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSort();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdMultiSelect : public PropertyCmd
    {
    public:
        CmdMultiSelect() : PropertyCmd(N_PropertyUIListbox_MultiselectEnable, _T("multi_select"), _I18n("Select"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Listbox *>(own)->isMultiSelect(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Listbox *>(own)->setMultiSelect(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Listbox *>(own)->isMultiSelect();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Listbox *>(own)->setMultiSelect(*temp);
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

    class _EngineInner CmdForceVertScrollbar : public PropertyCmd
    {
    public:
        CmdForceVertScrollbar() : PropertyCmd(N_PropertyUIListbox_VScrollAlwaysShow, _T("force_vert_scrollbar"), _I18n("ForceVertScrollbar"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Listbox *>(own)->isForceVScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Listbox *>(own)->SetForceVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Listbox *>(own)->isForceVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Listbox *>(own)->SetForceVScrollbar(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdForceVertScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdForceHorzScrollbar : public PropertyCmd
    {
    public:
        CmdForceHorzScrollbar() : PropertyCmd(N_PropertyUIListbox_HScrollAlwaysShow, _T("force_horz_scrollbar"), _I18n("ForceHorzScrollbar "))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Listbox *>(own)->isForceHScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Listbox *>(own)->SetForceHScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Listbox *>(own)->isForceHScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Listbox *>(own)->SetForceHScrollbar(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdForceHorzScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdItemTooltips : public PropertyCmd
    {
    public:
        CmdItemTooltips() : PropertyCmd(N_PropertyUIListbox_TooltipsEnable, _T("item_tooltips"), _I18n("Tooltips"))
        {
        }
        /// @copydetails PropertyCmd:: get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Listbox *>(own)->isItemTipsEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Listbox *>(own)->setItemTipsEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Listbox *>(own)->isItemTipsEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Listbox *>(own)->setItemTipsEnable(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdItemTooltips();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Widget, Listbox, ItemListEvent,                     0);
    N_EVENT(Widget, Listbox, SelectEvent,                       1);
    N_EVENT(Widget, Listbox, SortModeEvent,                     2);
    N_EVENT(Widget, Listbox, MultiSelectModeEvent,              3);
    N_EVENT(Widget, Listbox, VScrollModeEvent,                  4);
    N_EVENT(Widget, Listbox, HScrollModeEvent,                  5);
    N_EVENT(Widget, Listbox, EventCount,                        6);
    //------------------------------------------------------------------------
	#define VScrollbarID	1					//"__auto_vscrollbar__"
	#define HScrollbarID	2					//"__auto_hscrollbar__"
    //------------------------------------------------------------------------
    Listbox::Listbox(WidgetID wid, FactoryID fid, Container * own, const String & name,
        ScriptTypeID stid, LangID lid) :
        Container(wid, fid, own, name, stid, lid),
        mSort(false),
        mMultiSelect(false),
        mForceVScroll(false),
        mForceHScroll(false),
        mTipsEnable(false),
        mCurrentSelect(0)
    {
        notifySignal(Listbox::EventCount);
        setupCmd(N_CmdObj_Listbox);
    }
    //------------------------------------------------------------------------
    Listbox::~Listbox()
    {
        removeAllImpl();
    }
    //------------------------------------------------------------------------
    void Listbox::initChild()
    {
        // get the component sub-widgets
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        vertScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&Listbox::ScrollPrc);

        horzScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&Listbox::ScrollPrc);

        updateScrollbar();
        layout();
    }
    //------------------------------------------------------------------------
    NCount Listbox::getSelectCount() const
    {
        NCount count = 0;
        NCount i, iend = mItemList.size();
        for (i = 0; i < iend; ++i)
        {
            if (mItemList[i]->isSelect())
            {
                count++;
            }
        }
        return count;
    }
    //------------------------------------------------------------------------
    ListboxItem * Listbox::getFirstSelect() const
    {
        return getNextSelect(0);
    }
    //------------------------------------------------------------------------
    ListboxItem * Listbox::getNextSelect(const ListboxItem * start_item) const
    {
        // if start_item is NULL begin search at begining, else start at item after start_item
        NCount index = (start_item == 0) ? 0 : (getItemIndex(start_item) + 1);

        while (index < mItemList.size())
        {
            // return pointer to this item if it's selected.
            if (mItemList[index]->isSelect())
            {
                return mItemList[index];
            }
            // not selected, advance to next
            else
            {
                index++;
            }
        }
        // no more selected items.
        return 0;
    }
    //------------------------------------------------------------------------
    ListboxItem * Listbox::getItem(NCount index) const
    {
        if(index < mItemList.size())
        {
            return mItemList[index];
        }

        return 0;
    }
    //------------------------------------------------------------------------
    Nidx Listbox::getItemIndex(const ListboxItem * item) const
    {
        ListboxItemList::const_iterator pos = std::find(mItemList.begin(), mItemList.end(), item);

        if(pos != mItemList.end())
        {
            return std::distance(mItemList.begin(), pos);
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified ListboxItem is not attached to this Listbox."));
        }
    }
    //------------------------------------------------------------------------
    bool Listbox::isItemSelect(NCount index) const
    {
        if(index < mItemList.size())
        {
            return mItemList[index]->isSelect();
        }
        return false;
    }
    //------------------------------------------------------------------------
    ListboxItem * Listbox::getItem(const String & text, const ListboxItem * begin)
    {
        NCount index = (!begin) ? 0 : (getItemIndex(begin) + 1);
        while(index < mItemList.size())
        {
            if(mItemList[index]->getText() == text)
            {
                return mItemList[index];
            }
            else
            {
                index++;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool Listbox::isItemExist(const ListboxItem * item) const
    {
        return std::find(mItemList.begin(), mItemList.end(), item) != mItemList.end();
    }
    //------------------------------------------------------------------------
    void Listbox::removeAllItem()
    {
        if (removeAllImpl())
        {
            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::addItem(ListboxItem * item)
    {
        if (item)
        {
            // establish ownership
            item->setOwnerWindow(this);

            // if sorting is enabled, re-sort the list
            if (isSortEnable())
            {
                mItemList.insert(std::upper_bound(mItemList.begin(), mItemList.end(), item, &less), item);

            }
            // not sorted, just stick it on the end.
            else
            {
                mItemList.push_back(item);
            }

            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::insertItem(ListboxItem * item, const ListboxItem * position)
    {
        // if the list is sorted, it's the same as a normal add operation
        if(isSortEnable())
        {
            addItem(item);
        }
        else if (item)
        {
            // establish ownership
            item->setOwnerWindow(this);

            // if position is NULL begin insert at begining, else insert after item 'position'
            ListboxItemList::iterator ins_pos;

            if (!position)
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
                        _I18n("the specified ListboxItem for parameter 'position' is not attached to this Listbox."));
                }

            }

            mItemList.insert(ins_pos, item);

            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::removeItem(const ListboxItem * item)
    {
        if(item)
        {
            ListboxItemList::iterator pos = std::find(mItemList.begin(), mItemList.end(), item);
            if(pos != mItemList.end())
            {
                // disown item
                (*pos)->setOwnerWindow(0);

                // remove item
                mItemList.erase(pos);

                // if item was the last selected item, reset that to NULL
                if(item == mCurrentSelect)
                {
                    mCurrentSelect = 0;
                }

                if (item->isAutoDestroy())
                {
                    N_delete item;
                }

                WidgetEventArgs args(this);
                onItemList(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void Listbox::unselectAll(void)
    {
        if (unselectImpl())
        {
            WidgetEventArgs args(this);
            onSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::setSortEnable(bool set)
    {
        if (mSort != set)
        {
            mSort = set;

            if (mSort)
            {
                std::sort(mItemList.begin(), mItemList.end(), &less);
            }
            WidgetEventArgs args(this);
            onSortMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::setMultiSelect(bool set)
    {
        if (mMultiSelect != set)
        {
            mMultiSelect = set;

            WidgetEventArgs args(this);
            if ((!mMultiSelect) && (getSelectCount() > 1))
            {
                ListboxItem* itm = getFirstSelect();

                while ((itm = getNextSelect(itm)))
                {
                    itm->select(false);
                }
                onSelect(&args);
            }
            onMultiSelectMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::SetForceVScrollbar(bool set)
    {
        if(mForceVScroll != set)
        {
            mForceVScroll = set;

            updateScrollbar();
            WidgetEventArgs args(this);
            onVScrollbarMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::SetForceHScrollbar(bool set)
    {
        if(mForceHScroll != set)
        {
            mForceHScroll = set;

            updateScrollbar();
            WidgetEventArgs args(this);
            onHScrollbarMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::setItemSelect(ListboxItem * item, bool state)
    {
        ListboxItemList::iterator pos = std::find(mItemList.begin(), mItemList.end(), item);

        if (pos != mItemList.end())
        {
            setItemSelect(std::distance(mItemList.begin(), pos), state);
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified ListboxItem is not attached to this Listbox."));
        }
    }
    //------------------------------------------------------------------------
    void Listbox::setItemSelect(NCount item_index, bool state)
    {
        if (item_index < getItemCount())
        {
            // only do this if the setting is changing
            if (mItemList[item_index]->isSelect() != state)
            {
                if (state && !mMultiSelect)
                {
                    unselectImpl();
                }

                mItemList[item_index]->select(state);
                WidgetEventArgs args(this);
                onSelect(&args);
            }
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("the value passed in the 'item_index' parameter is out of range for this Listbox."));
        }
    }
    //------------------------------------------------------------------------
    void Listbox::notifyItemUpdate()
    {
        if(mSort)
            std::sort(mItemList.begin(), mItemList.end(), &less);

        updateScrollbar();
        refresh();
    }
    //------------------------------------------------------------------------
    void Listbox::updateScrollbar()
    {
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        NIIf totalHeight = getMaxHeight();
        NIIf widestItem = getMaxWidth();

        //
        // First show or hide the scroll bars as needed (or requested)
        //
        // show or hide vertical scroll bar as required (or as specified by option)
        if ((totalHeight > getListRenderArea().getHeight()) || mForceVScroll)
        {
            vertScrollbar->show();

            // show or hide horizontal scroll bar as required (or as specified by option)
            if ((widestItem > getListRenderArea().getWidth()) || mForceHScroll)
            {
                horzScrollbar->show();
            }
            else
            {
                horzScrollbar->hide();
            }
        }
        else
        {
            // show or hide horizontal scroll bar as required (or as specified by option)
            if ((widestItem > getListRenderArea().getWidth()) || mForceHScroll)
            {
                horzScrollbar->show();

                // show or hide vertical scroll bar as required (or as specified by option)
                if ((totalHeight > getListRenderArea().getHeight()) || mForceVScroll)
                {
                    vertScrollbar->show();
                }
                else
                {
                    vertScrollbar->hide();
                }

            }
            else
            {
                vertScrollbar->hide();
                horzScrollbar->hide();
            }
        }
        //
        // Set up scroll bar values
        //
        Rectf renderArea(getListRenderArea());

        vertScrollbar->setDocSize(totalHeight);
        vertScrollbar->setPageSize(renderArea.getHeight());
        vertScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.getHeight() / 10.0f));
        vertScrollbar->setCurrent(vertScrollbar->getCurrent());

        horzScrollbar->setDocSize(widestItem);
        horzScrollbar->setPageSize(renderArea.getWidth());
        horzScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.getWidth() / 10.0f));
        horzScrollbar->setCurrent(horzScrollbar->getCurrent());
    }
    //------------------------------------------------------------------------
    void Listbox::selectRange(Nidx start, Nidx end)
    {
        // only continue if list has some items
        if (!mItemList.empty())
        {
            // if start is out of range, start at begining.
            if (start > mItemList.size())
            {
                start = 0;
            }

            // if end is out of range end at the last item.
            if (end >= mItemList.size())
            {
                end = mItemList.size() - 1;
            }

            // ensure start becomes before the end.
            if (start > end)
            {
                NCount tmp;
                tmp = start;
                start = end;
                end = tmp;
            }

            // perform selections
            for( ; start <= end; ++start)
            {
                mItemList[start]->select(true);
            }
        }
    }
    //------------------------------------------------------------------------
    NIIf Listbox::getMaxHeight(void) const
    {
        NIIf height = 0;

        for(NCount i = 0; i < getItemCount(); ++i)
        {
            height += mItemList[i]->getPixelSize().mHeight;
        }
        return height;
    }
    //------------------------------------------------------------------------
    NIIf Listbox::getMaxWidth(void) const
    {
        NIIf widest = 0;

        for(NCount i = 0; i < getItemCount(); ++i)
        {
            NIIf thisWidth = mItemList[i]->getPixelSize().mWidth;

            if (thisWidth > widest)
            {
                widest = thisWidth;
            }
        }
        return widest;
    }
    //------------------------------------------------------------------------
    bool Listbox::unselectImpl(void)
    {
        bool modified = false;

        for(NCount index = 0; index < mItemList.size(); ++index)
        {
            if (mItemList[index]->isSelect())
            {
                mItemList[index]->select(false);
                modified = true;
            }
        }
        return modified;
    }
    //------------------------------------------------------------------------
    ListboxItem * Listbox::getItem(const Vector2f & pt) const
    {
        Vector2f local_pos(pt.x - getActualPosX(), pt.y - getActualPosY());
        Rectf renderArea(getListRenderArea());

        // point must be within the rendering area of the Listbox.
        if(renderArea.intersects(local_pos.x, local_pos.y))
        {
            NIIf y = renderArea.mLeft - getVScrollbar()->getCurrent();

            // test if point is above first item
            if(local_pos.y >= y)
            {
                for(NCount i = 0; i < getItemCount(); ++i)
                {
                    y += mItemList[i]->getPixelSize().mHeight;

                    if (local_pos.y < y)
                    {
                        return mItemList[i];
                    }
                }
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void Listbox::onItemList(const WidgetEventArgs * arg)
    {
        updateScrollbar();
        refresh();
        signal(ItemListEvent, arg);
    }
    //------------------------------------------------------------------------
    void Listbox::onSelect(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void Listbox::onSortMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SortModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Listbox::onMultiSelectMode(const WidgetEventArgs * arg)
    {
        signal(MultiSelectModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Listbox::onVScrollbarMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(VScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Listbox::onHScrollbarMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(HScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Listbox::onSize(const WidgetEventArgs * arg)
    {
        Container::onSize(arg);
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void Listbox::onButtonDown(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onButtonDown(arg);
        Nmark wck = mSheet->getWinComKey();
        if(arg->mMouseArg.mButton == MB_Left)
        {
            bool modified = false;

            // clear old selections if no control key is pressed or if multi-select is off
            if(!(wck & WCK_KT_A_Ctrl) || !mMultiSelect)
            {
                modified = unselectImpl();
            }

            ListboxItem * item = getItem(Vector2f(arg->mLocalX, arg->mLocalY));

            if(item)
            {
                modified = true;

                // select range or item, depending upon keys and last selected item
                if(((wck & WCK_KT_A_Shift) && (mCurrentSelect != 0)) && mMultiSelect)
                {
                    selectRange(getItemIndex(item), getItemIndex(mCurrentSelect));
                }
                else
                {
                    item->select(item->isSelect() ^ true);
                }

                // update last selected item
                mCurrentSelect = item->isSelect() ? item : 0;
            }

            // fire event if needed
            if (modified)
            {
                WidgetEventArgs args(this);
                onSelect(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void Listbox::onCursorDrag(const CursorEventArgs * arg)
    {
        // base class processing.
        Widget::onCursorDrag(arg);

        Scrollbar* vertScrollbar = getVScrollbar();
        Scrollbar* horzScrollbar = getHScrollbar();

        if (vertScrollbar->isFinalVisible() && (vertScrollbar->getDocSize() > vertScrollbar->getPageSize()))
        {
            vertScrollbar->setCurrent(vertScrollbar->getCurrent() + vertScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
        else if (horzScrollbar->isFinalVisible() && (horzScrollbar->getDocSize() > horzScrollbar->getPageSize()))
        {
            horzScrollbar->setCurrent(horzScrollbar->getCurrent() + horzScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
    }
    //------------------------------------------------------------------------
    void Listbox::onCursorMove(const CursorEventArgs * arg)
    {
        if(mTipsEnable)
        {
            static ListboxItem * lastItem = 0;

            ListboxItem * item = getItem(Vector2f(arg->mLocalX, arg->mLocalY));
            if(item != lastItem)
            {
                if(item)
                {
                    setTooltipText(item->getTooltipText());
                }
                else
                {
                    setTooltipText(_T(""));
                }
                lastItem = item;
            }

            // must check the result from getTooltip(), as the tooltip object could
            // be 0 at any time for various reasons.
            Tooltip * tooltip = mAide2->getTooltip();

            if(tooltip)
            {
                if(tooltip->getDest() != this)
                    tooltip->setDest(this);
                else
                    tooltip->updatePos();
            }
        }
        Widget::onCursorMove(arg);
    }
    //------------------------------------------------------------------------
    void Listbox::visibleItem(Nidx item_index)
    {
        Scrollbar * vertScrollbar = getVScrollbar();

        // handle simple "scroll to the bottom" case
        if (item_index >= getItemCount())
        {
            vertScrollbar->setCurrent(vertScrollbar->getDocSize() - vertScrollbar->getPageSize());
        }
        else
        {
            NIIf bottom;
            NIIf listHeight = getListRenderArea().getHeight();
            NIIf top = 0;

            // get height to top of item
            NCount i;
            for (i = 0; i < item_index; ++i)
            {
                top += mItemList[i]->getPixelSize().mHeight;
            }

            // calculate height to bottom of item
            bottom = top + mItemList[i]->getPixelSize().mHeight;

            // account for current scrollbar value
            NIIf currPos = vertScrollbar->getCurrent();
            top        -= currPos;
            bottom    -= currPos;

            // if top is above the view area, or if item is too big to fit
            if ((top < 0.0f) || ((bottom - top) > listHeight))
            {
                // scroll top of item to top of box.
                vertScrollbar->setCurrent(currPos + top);
            }
            // if bottom is below the view area
            else if (bottom >= listHeight)
            {
                // position bottom of item at the bottom of the list
                vertScrollbar->setCurrent(currPos + bottom - listHeight);
            }

            // Item is already fully visible - nothing more to do.
        }
    }
    //------------------------------------------------------------------------
    void Listbox::visibleItem(const ListboxItem* item)
    {
        visibleItem(getItemIndex(item));
    }
    //------------------------------------------------------------------------
    bool Listbox::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdSort());
        dest->add(N_new CmdMultiSelect());
        dest->add(N_new CmdForceVertScrollbar());
        dest->add(N_new CmdForceHorzScrollbar());
        dest->add(N_new CmdItemTooltips());
        return true;
    }
    //------------------------------------------------------------------------
    bool Listbox::removeAllImpl(void)
    {
        if(getItemCount() == 0)
        {
            return false;
        }
        else
        {
            for(NCount i = 0; i < getItemCount(); ++i)
            {
                if(mItemList[i]->isAutoDestroy())
                {
                    N_delete mItemList[i];
                }
            }
            mItemList.clear();

            mCurrentSelect = 0;

            return true;
        }
    }
    //------------------------------------------------------------------------
    void Listbox::ScrollPrc(const EventArgs *)
    {
        refresh();
    }
    //------------------------------------------------------------------------
    Scrollbar * Listbox::getVScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + VScrollbarID, false));
    }
    //------------------------------------------------------------------------
    Scrollbar * Listbox::getHScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + HScrollbarID, false));
    }
    //------------------------------------------------------------------------
    Rectf Listbox::getListRenderArea() const
    {
        if (mWidgetView != 0)
        {
            ListboxView * wr = (ListboxView *)mWidgetView;
            return wr->getListRenderArea();
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("This function must be implemented by the window renderer module"));
        }
    }
    //--------------------------------------------------------------------------
    bool Listbox::validImpl(const WidgetView * style) const
    {
        return dynamic_cast<const ListboxView *>(style) != 0;
    }
    //------------------------------------------------------------------------
    bool less(const ListboxItem * a, const ListboxItem * b)
    {
        return *a < *b;
    }
    //------------------------------------------------------------------------
    bool greater(const ListboxItem * a, const ListboxItem * b)
    {
        return *a > *b;
    }
    //------------------------------------------------------------------------
    /*
    Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"
    */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListboxView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ListboxView::ListboxView(WidgetViewlID wmid) :
        WidgetView(wmid, N_VIEW_Listbox)
    {
    }
    //------------------------------------------------------------------------
    Rectf ListboxView::getListRenderArea(void) const
    {
        Listbox * lb = (Listbox *)mSrc;
        return getRenderArea(lb->getHScrollbar()->isVisible(), lb->getVScrollbar()->isVisible());
    }
    //------------------------------------------------------------------------
    Rectf ListboxView::getRenderArea(bool hscroll, bool vscroll) const
    {
        const Listbox * const lb = static_cast<Listbox *>(mSrc);
        const Style * style = getStyle();
        Nid sid, sid2;

        if (vscroll)
        {
            if (hscroll)
            {
                sid = ItemRenderingAreaHVScroll;
                sid2 = ItemRenderAreaHVScroll;
            }
            else
            {
                sid = ItemRenderingAreaVScroll;
                sid2 = ItemRenderAreaVScroll;
            }
        }
        else
        {
            if (hscroll)
            {
                sid = ItemRenderingAreaHScroll;
                sid2 = ItemRenderAreaHScroll;
            }
            else
            {
                sid = ItemRenderingArea;
                sid2 = ItemRenderArea;
            }
        }

        if(style->isSpecialExist(sid))
            return style->getSpecial(sid).getArea().getPixelRect(*lb);

        if(style->isSpecialExist(sid2))
            return style->getSpecial(sid2).getArea().getPixelRect(*lb);

        if(style->isSpecialExist(ItemRenderingArea))
            return style->getSpecial(ItemRenderingArea).getArea().getPixelRect(*lb);
        else
            return style->getSpecial(ItemRenderArea).getArea().getPixelRect(*lb);
    }
    //------------------------------------------------------------------------
    void ListboxView::fit(bool fit_width, bool fit_height) const
    {
        Listbox * const lb = static_cast<Listbox *>(mSrc);

        const Rectf totalArea(lb->getArea().getIntCache());

        const Rectf contentArea(getRenderArea(
            fit_width ? false : lb->getHScrollbar()->isVisible(),
            fit_height ? false : lb->getVScrollbar()->isVisible()));

        const Rectf withScrollContentArea(getRenderArea(true, true));

        const PlaneSizef frameSize(totalArea.getSize() - contentArea.getSize());
        const PlaneSizef withScrollFrameSize(totalArea.getSize() -
            withScrollContentArea.getSize());
        const PlaneSizef contentSize(lb->getMaxWidth(), lb->getMaxHeight());

        const PlaneSizef parentSize(lb->calcParentArea());
        const PlaneSizef maxSize(parentSize.mWidth - lb->getPos().x.abs(parentSize.mWidth),
            parentSize.mHeight - lb->getPos().y.abs(parentSize.mHeight));

        PlaneSizef requiredSize(frameSize + contentSize + PlaneSizef(1, 1));

        if (fit_height)
        {
            if (requiredSize.mHeight > maxSize.mHeight)
            {
                requiredSize.mHeight = maxSize.mHeight;
                requiredSize.mWidth = N_MIN(NIIf, maxSize.mWidth,
                    requiredSize.mWidth - frameSize.mWidth + withScrollFrameSize.mWidth);
            }
        }

        if (fit_width)
        {
            if (requiredSize.mWidth > maxSize.mWidth)
            {
                requiredSize.mWidth = maxSize.mWidth;
                requiredSize.mHeight = N_MIN(NIIf, maxSize.mHeight,
                    requiredSize.mHeight - frameSize.mHeight + withScrollFrameSize.mHeight);
            }
        }

        if (fit_height)
            lb->setHeight(RelPosf(0, requiredSize.mHeight));

        if (fit_width)
            lb->setWidth(RelPosf(0, requiredSize.mWidth));
    }
    //------------------------------------------------------------------------
    void ListboxView::flush()
    {
        Listbox * lb = (Listbox *)mSrc;
        const StyleLayout * ss = &getStyle()->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        ss->render(mSrc);

        //
        // Render list items
        //
        Vector3f itemPos;
        PlaneSizef itemSize;
        Rectf itemClipper, itemRect;
        const NIIf widest = lb->getMaxWidth();

        // calculate position of area we have to render into
        Rectf itemsArea(getListRenderArea());

        // set up some initial positional details for items
        itemPos.x = itemsArea.left() - lb->getHScrollbar()->getCurrent();
        itemPos.y = itemsArea.top() - lb->getVScrollbar()->getCurrent();
        itemPos.z = 0.0f;

        const NIIf alpha = lb->getAlphaEffect();

        // loop through the items
        NCount itemCount = lb->getItemCount();

        for (NCount i = 0; i < itemCount; ++i)
        {
            ListboxItem * listItem = lb->getItem(i);
            itemSize.mHeight = listItem->getPixelSize().mHeight;

            // allow item to have full width of box if this is wider than items
            itemSize.mWidth = N_MAX(NIIf, itemsArea.getWidth(), widest);

            // calculate destination area for this item.
            itemRect.setLeft(itemPos.x);
            itemRect.setTop(itemPos.y);
            itemRect.setSize(itemSize);
            itemClipper = itemRect.intersects(itemsArea);

            // skip this item if totally clipped
            if (itemClipper.getWidth() == 0)
            {
                itemPos.y += itemSize.mHeight;
                continue;
            }
            GeometryPixel * dstgp = lb->getPixelData();
            // draw this item
            listItem->draw(*dstgp, itemRect, alpha, &itemClipper);

            // update position ready for next item
            itemPos.y += itemSize.mHeight;
        }
    }
    //------------------------------------------------------------------------
    bool ListboxView::notifyFont(const Font * font)
    {
        bool res = WidgetView::notifyFont(font);

        if (!res)
        {
            Listbox * const listbox = static_cast<Listbox *>(mSrc);
            NCount i, iend = listbox->getItemCount();
            for (i = 0; i < iend; ++i)
            {
                res |= listbox->getItem(i)->notifyFont(font);
            }
            if (res)
            {
                listbox->refresh();
            }
        }

        return res;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ComboDropList
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(Listbox, ComboDropList, SelectAcceptEvent, 0);
    N_EVENT(Listbox, ComboDropList, EventCount, 1);
    //------------------------------------------------------------------------
    ComboDropList::ComboDropList(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Listbox(wid, fid, own, name, N_CmdObj_ComboDropList, lid)
    {
        notifySignal(ComboDropList::EventCount);
        mAutoDropDown = false;
        mDropDown = false;
        mSelectAccept = 0;

        hide();

        // pass captured inputs to children to enable scrollbars
        setTransmitCursorEvent(true);
        setWrite(N_PropertyUIContainer_BroadcastInput, false);
    }
    //------------------------------------------------------------------------
    ComboDropList::~ComboDropList()
    {
    }
    //------------------------------------------------------------------------
    void ComboDropList::initChild()
    {
        Listbox::initChild();

        // set-up scroll bars so they return capture to us.
        getVScrollbar()->setRestorePreFocus(true);
        getHScrollbar()->setRestorePreFocus(true);

        // ban these properties from being written
        getVScrollbar()->setWrite(N_PropertyUIWidget_RestorePreFocus, false);
        getHScrollbar()->setWrite(N_PropertyUIWidget_RestorePreFocus, false);
    }
    //------------------------------------------------------------------------
    void ComboDropList::fit(bool width, bool height)
    {
        if (!mWidgetView)
            N_EXCEPT(InvalidRequest, _I18n("Function requires a valid WidgetView object to be set."));

        static_cast<ListboxView *>(mWidgetView)->fit(width, height);
    }
    //------------------------------------------------------------------------
    void ComboDropList::onSelectAccept(const WidgetEventArgs * arg)
    {
        mSelectAccept = getFirstSelect();
        signal(SelectAcceptEvent, arg);
    }
    //------------------------------------------------------------------------
    void ComboDropList::onItemList(const WidgetEventArgs * arg)
    {
        // basically see if our 'sticky' selection was removed
        if ((mSelectAccept) && !isItemExist(mSelectAccept))
            mSelectAccept = 0;

        // base class processing
        Listbox::onItemList(arg);
    }
    //------------------------------------------------------------------------
    void ComboDropList::onSelect(const WidgetEventArgs * arg)
    {
        if (!isFinalActive())
            mSelectAccept = getFirstSelect();

        Listbox::onSelect(arg);
    }
    //------------------------------------------------------------------------
    void ComboDropList::onCursorMove(const CursorEventArgs * arg)
    {
        Listbox::onCursorMove(arg);
        Vector2f tp(arg->mLocalX, arg->mLocalY);
        // if mouse is within our area (but not our children)
        if (intersects(tp))
        {
            if (!get(tp))
            {
                // handle auto-arm
                if (mAutoDropDown)
                {
                    mDropDown = true;
                }

                if (mDropDown)
                {
                    // check for an item under the mouse
                    ListboxItem * selItem = getItem(tp);

                    // if an item is under mouse, select it
                    if (selItem)
                    {
                        setItemSelect(selItem, true);
                    }
                    else
                    {
                        unselectAll();
                    }
                }
            }
        }
        // not within the list area
        else
        {
            if (mSheet->getWinComKey() & WCK_MB_Left)
            {
                unselectAll();
            }
        }
    }
    //------------------------------------------------------------------------
    void ComboDropList::onButtonDown(const CursorEventArgs * arg)
    {
        Listbox::onButtonDown(arg);

        if (arg->mMouseArg.mButton == MB_Left)
        {
            if (!intersects(Vector2f(arg->mLocalX, arg->mLocalY)))
            {
                unselectAll();
                lost();
            }
            else
            {
                mDropDown = true;
            }
        }
    }
    //------------------------------------------------------------------------
    void ComboDropList::onButtonUp(const CursorEventArgs * arg)
    {
        Listbox::onButtonUp(arg);

        if (arg->mMouseArg.mButton == MB_Left)
        {
            if (mDropDown && (get(Vector2f(arg->mLocalX, arg->mLocalY)) == 0))
            {
                if (getSelectCount() > 0)
                {
                    WidgetEventArgs args(this);
                    onSelectAccept(&args);
                }
                lost();
            }
            // if we are not already armed, in response to a left button up event, we auto-arm.
            else
            {
                mDropDown = true;
            }
        }
    }
    //------------------------------------------------------------------------
    void ComboDropList::onLost(const WidgetEventArgs * arg)
    {
        Listbox::onLost(arg);
        mDropDown = false;
        hide();

        // ensure 'sticky' selection remains.
        if ((mSelectAccept) && !mSelectAccept->isSelect())
        {
            unselectImpl();
            setItemSelect(mSelectAccept, true);
        }
    }
    //------------------------------------------------------------------------
    void ComboDropList::onActivate(const ActivationEventArgs * arg)
    {
        Listbox::onActivate(arg);
    }
    //------------------------------------------------------------------------

}
}
