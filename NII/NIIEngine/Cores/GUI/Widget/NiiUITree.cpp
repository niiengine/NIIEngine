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
#include "NiiUIWidgetManager.h"
#include "NiiUITree.h"
#include "NiiUIScrollbar.h"
#include "NiiUITooltip.h"
#include "NiiFont.h"
#include "NiiUIWidgetList.h"
#include "NiiUIManager.h"
#include "NiiUIWidget.h"
#include "NiiPixelManager.h"
#include "NiiFontManager.h"

#if defined (CEGUI_USE_FRIBIDI)
#include "NiiUIFribidiText.h"
#elif defined (CEGUI_USE_MINIBIDI)
#include "NiiUIMinibidiText.h"
#else
#include "NiiUIBidiText.h"
#endif

#define ItemRenderingArea 1
#define ItemRenderingAreaHScroll 2
#define ItemRenderingAreaVScroll 3
#define ItemRenderingAreaHVScroll 4

#define HORIZONTAL_STEP_SIZE_DIVISOR 20.0f

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TreeItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TreeItem::TreeItem(const String & text, Nui32 id, void * data,
        bool disabled, bool autoDestroy) :
#ifndef CEGUI_BIDI_SUPPORT
        mBidiText(0),
#elif defined (CEGUI_USE_FRIBIDI)
        mBidiText(N_new FribidiText),
#elif defined (CEGUI_USE_MINIBIDI)
        mBidiText(N_new MinibidiText),
#else
#error "BIDI Configuration is inconsistant, check your config!"
#endif
        mID(id),
        mCustomData(data),
        mSelect(false),
        mDisable(disabled),
        mAutoDestroy(autoDestroy),
        mToggleArea(Rectf(0, 0, 0, 0)),
        mParent(0),
        mSelectColour(WidgetManager::DefaultSelectionColour),
        mSelectView(0),
        mColour(WidgetManager::DefaultTextColour),
        mFont(0),
        mIconView(0),
        mValidBidi(false),
        mUnfold(false),
        mGridValid(false)
    {
        setText(text);
    }
    //------------------------------------------------------------------------
    TreeItem::~TreeItem()
    {
        NCount i, iend = getItemCount();
        for (i = 0; i < iend; ++i)
        {
            if (mItemList[i]->isAutoDestroy())
            {
                N_delete mItemList[i];
            }
        }

        N_delete mBidiText;
    }
    //------------------------------------------------------------------------
    bool TreeItem::operator<(const TreeItem & o) const
    {
        return getText() < o.getText();
    }
    //------------------------------------------------------------------------
    bool TreeItem::operator>(const TreeItem & o) const
    {
        return getText() > o.getText();
    }
    //------------------------------------------------------------------------
    const Font * TreeItem::getFont() const
    {
        const Font * re;
        if (mFont != 0)
            re = mFont;
        else if (mParent != 0)
            re = mParent->getFont();
        else
            re = N_Only(UI).getMainSheet()->getFont();

        return re;
    }
    //------------------------------------------------------------------------
    void TreeItem::setFont(const Font * font)
    {
        mFont = font;

        mGridValid = false;
    }
    //------------------------------------------------------------------------
    PlaneSizef TreeItem::getPixelSize() const
    {
        const Font * fnt = getFont();

        if (!fnt)
            return PlaneSizef(0, 0);

        if (!mGridValid)
        {
            mPixelGrid = WidgetManager::gDefaultTextView->parse(getVisualText(), getFont(), &mColour);
            mGridValid = true;
        }
        PlaneSizef sz(0.0f, 0.0f);

        for (NCount i = 0; i < mPixelGrid.getLineCount(); ++i)
        {
            const PlaneSizef line_sz(mPixelGrid.getPixelSize(mParent, i));
            sz.mHeight += line_sz.mHeight;

            if (line_sz.mWidth > sz.mWidth)
                sz.mWidth = line_sz.mWidth;
        }

        return sz;
    }
    //------------------------------------------------------------------------
    void TreeItem::addItem(TreeItem * item)
    {
        if (item != 0)
        {
            Tree * parentWindow = (Tree *)getOwnerWindow();

            // establish ownership
            item->setOwnerWindow(parentWindow);

            // if sorting is enabled, re-sort the tree
            if (parentWindow->isSortEnable())
            {
                mItemList.insert(std::upper_bound(mItemList.begin(), mItemList.end(), item, &less), item);
            }
            // not sorted, just stick it on the end.
            else
            {
                mItemList.push_back(item);
            }

            WidgetEventArgs args(parentWindow);
            parentWindow->onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void TreeItem::removeItem(const TreeItem * item)
    {
        if (item)
        {
            Tree * parentWindow = (Tree *)getOwnerWindow();

            TreeItemList::iterator pos = std::find(mItemList.begin(), mItemList.end(), item);
            if (pos != mItemList.end())
            {
                (*pos)->setOwnerWindow(0);
                mItemList.erase(pos);

                if (item == parentWindow->mCurrentSelect)
                    parentWindow->mCurrentSelect = 0;

                if (item->isAutoDestroy())
                    N_delete item;

                WidgetEventArgs args(parentWindow);
                parentWindow->onItemList(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void TreeItem::draw(GeometryPixel & buffer, const Rectf & targetRect,
        NIIf alpha, const Rectf * clipper) const
    {
        Rectf finalRect(targetRect);

        if (mIconView != 0)
        {
            Rectf finalPos(finalRect);
            finalPos.setWidth(targetRect.getHeight());
            finalPos.setHeight(targetRect.getHeight());
            ColourArea tc(Colour(1, 1, 1, alpha));
            buffer.add(*mIconView, finalPos, clipper, tc);

            finalRect.mLeft += targetRect.getHeight();
        }

        if (mSelect && mSelectView != 0)
        {
            ColourArea tc = mSelectColour;
            tc.modulate(alpha);
            buffer.add(*mSelectView, finalRect, clipper, tc);
        }
        const Font * font = getFont();

        if (!font)
            return;

        Vector2f draw_pos(finalRect.getPos());
        draw_pos.y -= (font->getLineSpacing() - font->getBaseline()) * 0.5f;

        if (!mGridValid)
        {
            mPixelGrid = WidgetManager::gDefaultTextView->parse(getVisualText(), getFont(), &mColour);
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
    //------------------------------------------------------------------------
    void TreeItem::setText(const String & text)
    {
        mRawText = text;
        mValidBidi = false;
        mGridValid = false;
    }
    //------------------------------------------------------------------------
    const String & TreeItem::getVisualText() const
    {
        // no bidi support
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
    void TreeItem::setColour(const ColourArea & colour)
    {
        mColour = colour;
        mGridValid = false;
    }
    //------------------------------------------------------------------------
    bool TreeItem::notifyFont(const Font * font)
    {
        if (getFont() == font)
            return true;

        NCount i, iend = getItemCount();
        for (i = 0; i < iend; ++i)
        {
            if (mItemList[i]->notifyFont(font))
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Tree
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdSort : public PropertyCmd
    {
    public:
        CmdSort() : PropertyCmd(N_PropertyUITree_SortEnable, _T("sort"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Tree *>(own)->isSortEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Tree *>(own)->setSortEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Tree *>(own)->isSortEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Tree *>(own)->setSortEnable(*temp);
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
        CmdMultiSelect() : PropertyCmd(N_PropertyUITree_MultiselectEnable, _T("multi_select"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Tree *>(own)->isMultiSelect(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Tree *>(own)->setMultiSelect(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Tree *>(own)->isMultiSelect();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Tree *>(own)->setMultiSelect(*temp);
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
        CmdForceVertScrollbar() : PropertyCmd(N_PropertyUITree_VScrollAlwaysShow, _T("force_vert_scrollbar"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Tree * >(own)->isForceVScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Tree *>(own)->SetForceVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Tree * >(own)->isForceVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Tree *>(own)->SetForceVScrollbar(*temp);
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
        CmdForceHorzScrollbar() : PropertyCmd(N_PropertyUITree_HScrollAlwaysShow, _T("force_horz_scrollbar"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Tree *>(own)->isForceHScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Tree *>(own)->SetForceHScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Tree *>(own)->isForceHScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Tree *>(own)->SetForceHScrollbar(*temp);
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
        CmdItemTooltips() : PropertyCmd(N_PropertyUITree_ItemTooltipsEnable, _T("item_tooltips"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Tree *>(own)->isItemTipsEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Tree *>(own)->setItemTipsEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Tree *>(own)->isItemTipsEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Tree *>(own)->setItemTipsEnable(*temp);
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
    TreeItem * getNextSelectedItemFromList(const TreeItemList & itemList, const TreeItem * startItem, bool & foundStartItem)
    {
        NCount itemCount = itemList.size();

        for (NCount index = 0; index < itemCount; ++index)
        {
            if (foundStartItem == true)
            {
                // Already found the startItem, now looking for next selected item.
                if (itemList[index]->isSelect())
                    return itemList[index];
            }
            else
            {
                // Still looking for startItem.  Is this it?
                if (itemList[index] == startItem)
                    foundStartItem = true;
            }

            if (itemList[index]->getItemCount() > 0)
            {
                if (itemList[index]->isUnfold())
                {
                    TreeItem *foundSelectedTree;
                    foundSelectedTree = getNextSelectedItemFromList(itemList[index]->getItemList(), startItem, foundStartItem);
                    if (foundSelectedTree != 0)
                        return foundSelectedTree;
                }
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    TreeItem* findItemWithTextFromList(const TreeItemList &itemList, const String& text, const TreeItem * startItem, bool foundStartItem)
    {
        NCount itemCount = itemList.size();
        for (NCount index = 0; index < itemCount; ++index)
        {
            if (foundStartItem == true)
            {
                // Already found the startItem, now looking for the actual text.
                if (itemList[index]->getText() == text)
                    return itemList[index];
            }
            else
            {
                // Still looking for startItem.  Is this it?
                if (itemList[index] == startItem)
                    foundStartItem = true;
            }

            if (itemList[index]->getItemCount() > 0)
            {
                // Search the current item's itemList regardless if it's open or not.
                TreeItem *foundSelectedTree;
                foundSelectedTree = findItemWithTextFromList(itemList[index]->getItemList(), text, startItem, foundStartItem);
                if (foundSelectedTree != 0)
                    return foundSelectedTree;
            }
        }

        return 0;
    }
    //------------------------------------------------------------------------
    TreeItem * findItemWithIDFromList(const TreeItemList & itemList, Nui32 searchID, const TreeItem * startItem, bool foundStartItem)
    {
        NCount itemCount = itemList.size();
        for (NCount index = 0; index < itemCount; ++index)
        {
            if (foundStartItem == true)
            {
                if (itemList[index]->getID() == searchID)
                    return itemList[index];
            }
            else
            {
                if (itemList[index] == startItem)
                    foundStartItem = true;
            }

            if (itemList[index]->getItemCount() > 0)
            {
                // Search the current item's itemList regardless if it's open or not.
                TreeItem *foundSelectedTree;
                foundSelectedTree = findItemWithIDFromList(itemList[index]->getItemList(), searchID, startItem, foundStartItem);
                if (foundSelectedTree != 0)
                    return foundSelectedTree;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool containsOpenItemRecursive(const TreeItemList & itemList, TreeItem * item)
    {
        NCount i, iend = itemList.size();
        for (i = 0; i < iend; ++i)
        {
            if (itemList[i] == item)
                return true;

            if (itemList[i]->getItemCount() > 0)
            {
                if (itemList[i]->isUnfold())
                {
                    if (containsOpenItemRecursive(itemList[i]->getItemList(), item))
                        return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void getTotalItemsInListHeight(const TreeItemList &itemList, NIIf *heightSum)
    {
        NCount itemCount = itemList.size();
        for (NCount index = 0; index < itemCount; ++index)
        {
            *heightSum += itemList[index]->getPixelSize().mHeight;
            if (itemList[index]->isUnfold() && (itemList[index]->getItemCount() > 0))
                getTotalItemsInListHeight(itemList[index]->getItemList(), heightSum);
        }
    }
    //------------------------------------------------------------------------
    void getWidestItemWidthInList(const TreeItemList & itemList, int itemDepth, NIIf ext, NIIf & widest)
    {
        NCount itemCount = itemList.size();
        for (NCount index = 0; index < itemCount; ++index)
        {
            const Rectf & barea = itemList[index]->getToggleArea();
            NIIf thisWidth = itemList[index]->getPixelSize().mWidth + barea.getWidth() +
				ext  + (itemDepth * 20);

            if (thisWidth > widest)
                widest = thisWidth;

            if (itemList[index]->isUnfold() && (itemList[index]->getItemCount() > 0))
                getWidestItemWidthInList(itemList[index]->getItemList(), itemDepth + 1, ext, widest);
        }
    }
    //-----------------------------------------------------------------------
    void drawItemList(TreeItemList & itemList, Rectf & itemsArea, NIIf widest,
        Vector2f & itemPos, GeometryPixel & geometry, NIIf alpha, const StyleSection * osection,
			const StyleSection * csection, const Widget * widget)
    {
        if (itemList.empty())
            return;

        // loop through the items
        PlaneSizef itemSize;
        Rectf itemClipper, itemRect;
        NCount itemCount = itemList.size();
        bool itemIsVisible;
        for (NCount i = 0; i < itemCount; ++i)
        {
            itemSize.mHeight = itemList[i]->getPixelSize().mHeight;

            // allow item to have full width of box if this is wider than items
            itemSize.mWidth = N_MAX(NIIf, itemsArea.getWidth(), widest);

            // calculate destination area for this item.
            itemRect.setLeft(itemPos.x);
            itemRect.setTop(itemPos.y);
            itemRect.setSize(itemSize);
            itemClipper = itemRect.intersects(itemsArea);
            itemRect.mLeft += 20; // start text past open/close buttons

            if (itemClipper.getHeight() > 0)
            {
                itemIsVisible = true;
                itemList[i]->draw(geometry, itemRect, alpha, &itemClipper);
            }
            else
            {
                itemIsVisible = false;
            }

            // Process this item's list if it has items in it.
            if (itemList[i]->getItemCount() > 0)
            {
                Rectf buttonRenderRect;
                buttonRenderRect.setLeft(itemPos.x);
                buttonRenderRect.setRight(buttonRenderRect.left() + 10);
                buttonRenderRect.setTop(itemPos.y);
                buttonRenderRect.setBottom(buttonRenderRect.top() + 10);
                itemList[i]->setToggleArea(buttonRenderRect);

                if (itemList[i]->isUnfold())
                {
                    if(itemIsVisible)
						csection->render(widget, buttonRenderRect, 0, &itemClipper);

                    itemPos.y += itemSize.mHeight;

                    itemPos.x += 20;
                    drawItemList(itemList[i]->getItemList(), itemsArea, widest, itemPos, geometry, alpha,
						osection, csection, widget);
                    itemPos.x -= 20;
                }
                else
                {
                    if(itemIsVisible)
						osection->render(widget, buttonRenderRect, 0, &itemClipper);

                    itemPos.y += itemSize.mHeight;
                }
            }
            else
            {
                itemPos.y += itemSize.mHeight;
            }
        }
        // Successfully drew all items, so vertical scrollbar not needed.
        //   SetForceVScrollbar(false);
    }
    //------------------------------------------------------------------------
    bool getHeightToItemInList(const TreeItemList & itemList, const TreeItem * item, int itemDepth, NIIf * height)
    {
        NCount itemCount = itemList.size();
        for (NCount index = 0; index < itemCount; ++index)
        {
            if (item == itemList[index])
                return true;

            *height += itemList[index]->getPixelSize().mHeight;

            if (itemList[index]->isUnfold() && (itemList[index]->getItemCount() > 0))
            {
                if (getHeightToItemInList(itemList[index]->getItemList(), item, itemDepth + 1, height))
                    return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    TreeItem * getItemFromListAtPoint(const TreeItemList & itemList, NIIf * bottomY, const Vector2f & pt)
    {
        NCount itemCount = itemList.size();

        for (NCount i = 0; i < itemCount; ++i)
        {
            *bottomY += itemList[i]->getPixelSize().mHeight;
            if (pt.y < *bottomY)
                return itemList[i];

            if (itemList[i]->getItemCount() > 0)
            {
                if (itemList[i]->isUnfold())
                {
                    TreeItem * foundPointedAtTree;
                    foundPointedAtTree = getItemFromListAtPoint(itemList[i]->getItemList(), bottomY, pt);
                    if (foundPointedAtTree != 0)
                        return foundPointedAtTree;
                }
            }
        }

        return 0;
    }
    //------------------------------------------------------------------------
    N_EVENT(Container,    Tree,    ItemListEvent,                   0);
    N_EVENT(Container,    Tree,    SelectEvent,						1);
    N_EVENT(Container,    Tree,    SortModeEvent,                   2);
    N_EVENT(Container,    Tree,    MultiSelectModeEvent,            3);
    N_EVENT(Container,    Tree,    VScrollModeEvent,                4);
    N_EVENT(Container,    Tree,    HScrollModeEvent,                5);
    N_EVENT(Container,    Tree,    ItemUnfoldEvent,                 6);
    N_EVENT(Container,    Tree,    ItemCloseEvent,                  7);
    N_EVENT(Container,    Tree,    EventCount,                      8);
	const WidgetID Tree::VScrollbarID(1);
	const WidgetID Tree::HScrollbarID(2);
    const StateID Tree::OpenView(N_Inner_State_ID + 1);
    const StateID Tree::CloseTag(N_Inner_State_ID + 2);
    //------------------------------------------------------------------------
    Tree::Tree(WidgetID wid, FactoryID fid, Container * own, const String & name, LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_Tree, lid),
        mVScrollbar(0),
        mHScrollbar(0),
        mCurrentSelect(0),
        mOpenButtonView(0),
        mCloseButtonView(0),
        mItemArea(0, 0, 0, 0),
        mSort(false),
        mMultiSelect(false),
        mForceVScroll(false),
        mForceHScroll(false),
        mTipsEnable(false)
    {
        notifySignal(Tree::EventCount);
        setupCmd(N_CmdObj_Tree);
        
        bind(ItemListEvent);
        bind(SelectEvent);
        bind(SortModeEvent);
        bind(MultiSelectModeEvent);
        bind(VScrollModeEvent);
        bind(HScrollModeEvent);
        bind(ItemUnfoldEvent);
        bind(ItemCloseEvent);
    }
    //------------------------------------------------------------------------
    void Tree::setStyle(StyleID sid)
    {
        Widget::setStyle(sid);
        
        const Style * style = N_Only(Widget).getStyle(mStyleID);
        const StyleSection & tempOpenImagery = style->getDesign(OpenView);
        const StyleSection & tempCloseImagery = style->getDesign(CloseTag);
        mOpenButtonView = &tempOpenImagery;
        mCloseButtonView = &tempCloseImagery;

        mVScrollbar = createVertScrollbar(N_ChildID(mID, VScrollbarID));
        mHScrollbar = createHorzScrollbar(N_ChildID(mID, HScrollbarID));

        add(mVScrollbar);
        add(mHScrollbar);

        mVScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&Tree::ScrollPrc);

        mHScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&Tree::ScrollPrc);

        updateScrollbar();
        layout();
    }
    //------------------------------------------------------------------------
    Tree::~Tree()
    {
        removeAllImpl();
    }
    //------------------------------------------------------------------------
    NCount Tree::getSelectCount() const
    {
        NCount count = 0;
        NCount i, iend = mItemList.size();
        for (i = 0; i < iend; ++i)
        {
            if (mItemList[i]->isSelect())
                count++;
        }

        return count;
    }
    //------------------------------------------------------------------------
    TreeItem * Tree::getFirstSelect() const
    {
        bool found_first = true;
        return getNextSelectedItemFromList(mItemList, 0, found_first);
    }
    //------------------------------------------------------------------------
    TreeItem * Tree::getNextSelect(const TreeItem * start_item) const
    {
        bool found_first = (start_item == 0);
        return getNextSelectedItemFromList(mItemList, start_item, found_first);
    }
    //------------------------------------------------------------------------
    TreeItem * Tree::getItem(const String & text, const TreeItem * begin)
    {
        return findItemWithTextFromList(mItemList, text, begin, false);
    }
    //------------------------------------------------------------------------
    TreeItem * Tree::getItem(const String & text)
    {
        return findItemWithTextFromList(mItemList, text, 0, true);
    }
    //------------------------------------------------------------------------
    TreeItem * Tree::getItem(Nui32 searchID, const TreeItem * begin)
    {
        return findItemWithIDFromList(mItemList, searchID, begin, false);
    }
    //------------------------------------------------------------------------
    TreeItem * Tree::getItem(Nui32 searchID)
    {
        return findItemWithIDFromList(mItemList, searchID, 0, true);
    }
    //------------------------------------------------------------------------
    bool Tree::isItemExist(const TreeItem * item) const
    {
        return std::find(mItemList.begin(), mItemList.end(), item) != mItemList.end();
    }
    //------------------------------------------------------------------------
    void Tree::removeAllItem()
    {
        if (removeAllImpl())
        {
            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void Tree::addItem(TreeItem * item)
    {
        if (item != 0)
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
    void Tree::insertItem(TreeItem * item, const TreeItem * position)
    {
        if (isSortEnable())
        {
            addItem(item);
        }
        else if (item != 0)
        {
            // establish ownership
            item->setOwnerWindow(this);

            // if position is NULL begin insert at begining, else insert after item 'position'
            TreeItemList::iterator ins_pos;

            if (position == 0)
            {
                ins_pos = mItemList.begin();
            }
            else
            {
                ins_pos = std::find(mItemList.begin(), mItemList.end(), position);

                // throw if item 'position' is not in the list
                if (ins_pos == mItemList.end())
                {
                    N_EXCEPT(InvalidRequest, _I18n("the specified TreeItem for parameter 'position' is not attached to this Tree."));
                }
            }

            mItemList.insert(ins_pos, item);

            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void Tree::removeItem(const TreeItem * item)
    {
        if (item != 0)
        {
            TreeItemList::iterator pos = std::find(mItemList.begin(), mItemList.end(), item);
            if (pos != mItemList.end())
            {
                // disown item
                (*pos)->setOwnerWindow(0);

                // remove item
                mItemList.erase(pos);

                // if item was the last selected item, reset that to NULL
                if (item == mCurrentSelect)
                {
                    mCurrentSelect = 0;
                }

                // if item is supposed to be deleted by us
                if (item->isAutoDestroy())
                {
                    // clean up this item.
                    N_delete item;
                }

                WidgetEventArgs args(this);
                onItemList(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void Tree::unselectAll()
    {
        // only fire events and update if we actually made any changes
        if (unselectImpl())
        {
            TreeEventArgs args(this);
            onSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    void Tree::setSortEnable(bool set)
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
    void Tree::setMultiSelect(bool set)
    {
        if (mMultiSelect != set)
        {
            mMultiSelect = set;

            TreeEventArgs args(this);
            if ((!mMultiSelect) && (getSelectCount() > 1))
            {
                TreeItem * itm = getFirstSelect();

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
    void Tree::SetForceVScrollbar(bool set)
    {
        if (mForceVScroll != set)
        {
            mForceVScroll = set;

            updateScrollbar();
            WidgetEventArgs args(this);
            onVScrollbarMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void Tree::SetForceHScrollbar(bool set)
    {
        if (mForceHScroll != set)
        {
            mForceHScroll = set;

            updateScrollbar();
            WidgetEventArgs args(this);
            onHScrollbarMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void Tree::setItemSelect(TreeItem * item, bool state)
    {
        if (containsOpenItemRecursive(mItemList, item))
        {
            TreeEventArgs args(this);
            args.mItem = item;

            if (state && !mMultiSelect)
                unselectImpl();

            item->select(state);
            mCurrentSelect = item->isSelect() ? item : 0;
            onSelect(&args);
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified TreeItem is not ") _I18n("attached to this Tree or not visible."));
        }
    }
    //------------------------------------------------------------------------
    void Tree::setItemSelect(Nidx idx, bool state)
    {
        if(idx < getItemCount())
        {
            // only do this if the setting is changing
            if(mItemList[idx]->isSelect() != state)
            {
                // conditions apply for single-select mode
                if(state && !mMultiSelect)
                {
                    unselectImpl();
                }

                mItemList[idx]->select(state);
                TreeEventArgs args(this);
                args.mItem = mItemList[idx];
                onSelect(&args);
            }
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("the value passed in the 'idx' parameter is out of range for this Tree."));
        }
    }
    //------------------------------------------------------------------------
    void Tree::notifyItemUpdate()
    {
        updateScrollbar();
        refresh();
    }
    //------------------------------------------------------------------------
    void Tree::renderImpl()
    {
        Vector2f itemPos;
        NIIf widest = getMaxWidth();

        // calculate position of area we have to render into
        //Rect itemsArea(getTreeRenderArea());
        //Rect itemsArea(0,0,500,500);

        // set up some initial positional details for items
        itemPos.x = mItemArea.left() - mHScrollbar->getCurrent();
        itemPos.y = mItemArea.top() - mVScrollbar->getCurrent();

        drawItemList(mItemList, mItemArea, widest, itemPos, *mPixelData, getAlphaEffect(), mOpenButtonView, mCloseButtonView, this);
    }
    //------------------------------------------------------------------------
    void Tree::updateScrollbar()
    {
        Rectf renderArea(getTreeRenderArea());

        if (!mVScrollbar)
            mVScrollbar = createVertScrollbar(N_ChildID(mID, VScrollbarID));
        if(!mHScrollbar)
            mHScrollbar = createHorzScrollbar(N_ChildID(mID, HScrollbarID));

        NIIf totalHeight = getMaxHeight();
        NIIf widestItem = getMaxWidth() + 20;

        //
        // First show or hide the scroll bars as needed (or requested)
        //
        // show or hide vertical scroll bar as required (or as specified by option)
        if ((totalHeight > renderArea.getHeight()) || mForceVScroll)
        {
            mVScrollbar->show();
            renderArea.mRight -= mVScrollbar->getWidth().mOffset + mVScrollbar->getPos().x.mOffset;
            // show or hide horizontal scroll bar as required (or as specified by option)
            if ((widestItem > renderArea.getWidth()) || mForceHScroll)
            {
                mHScrollbar->show();
                renderArea.mBottom -= mHScrollbar->getHeight().mOffset;
            }
            else
            {
                mHScrollbar->hide();
                mHScrollbar->setCurrent(0);
            }
        }
        else
        {
            // show or hide horizontal scroll bar as required (or as specified by option)
            if ((widestItem > renderArea.getWidth()) || mForceHScroll)
            {
                mHScrollbar->show();
                renderArea.mBottom -= mVScrollbar->getHeight().mOffset;

                // show or hide vertical scroll bar as required (or as specified by option)
                if ((totalHeight > renderArea.getHeight()) || mForceVScroll)
                {
                    mVScrollbar->show();
                    //            renderArea.mRight -= mVScrollbar->getAbsoluteWidth();
                    renderArea.mRight -= mVScrollbar->getWidth().mOffset;
                }
                else
                {
                    mVScrollbar->hide();
                    mVScrollbar->setCurrent(0);
                }
            }
            else
            {
                mVScrollbar->hide();
                mVScrollbar->setCurrent(0);
                mHScrollbar->hide();
                mHScrollbar->setCurrent(0);
            }
        }

        //
        // Set up scroll bar values
        //

        NIIf itemHeight;
        if (!mItemList.empty())
            itemHeight = mItemList[0]->getPixelSize().mHeight;
        else
            itemHeight = 10;

        mVScrollbar->setDocSize(totalHeight);
        mVScrollbar->setPageSize(renderArea.getHeight());
        mVScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.getHeight() / itemHeight));
        mVScrollbar->setCurrent(mVScrollbar->getCurrent());

        mHScrollbar->setDocSize(widestItem + mVScrollbar->getWidth().mOffset);
        mHScrollbar->setPageSize(renderArea.getWidth());
        mHScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.getWidth() / HORIZONTAL_STEP_SIZE_DIVISOR));
        mHScrollbar->setCurrent(mHScrollbar->getCurrent());
    }
    //------------------------------------------------------------------------
    void Tree::selectRange(Nidx start, Nidx end)
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
    NIIf Tree::getMaxHeight() const
    {
        NIIf heightSum = 0;
        getTotalItemsInListHeight(mItemList, &heightSum);
        return heightSum;
    }
    //------------------------------------------------------------------------
    NIIf Tree::getMaxWidth() const
    {
        NIIf widest = 0;
		NIIf ext = mHScrollbar->getCurrent() / HORIZONTAL_STEP_SIZE_DIVISOR;
        getWidestItemWidthInList(mItemList, 0, ext, widest);
        return widest;
    }
    //------------------------------------------------------------------------
    bool Tree::unselectImpl()
    {
        return unselect(mItemList);
    }
    //------------------------------------------------------------------------
    bool Tree::unselect(const TreeItemList & itemList)
    {
        bool modified = false;
        for (NCount index = 0; index < itemList.size(); ++index)
        {
            if (itemList[index]->isSelect())
            {
                itemList[index]->select(false);
                modified = true;
            }

            if (itemList[index]->getItemCount() > 0)
            {
                bool modifiedSubList = unselect(itemList[index]->getItemList());
                if (modifiedSubList)
                    modified = true;
            }
        }
        return modified;
    }
    //------------------------------------------------------------------------
    TreeItem * Tree::getItem(const Vector2f & pt) const
    {
        Rectf renderArea(getTreeRenderArea());

        // point must be within the rendering area of the Tree.
        if(renderArea.intersects(pt.x, pt.y))
        {
            NIIf y = renderArea.top() - mVScrollbar->getCurrent();

            // test if point is above first item
            if (pt.y >= y)
                return getItemFromListAtPoint(mItemList, &y, pt);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void Tree::onItemList(const WidgetEventArgs * arg)
    {
        updateScrollbar();
        refresh();
        signal(ItemListEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onSelect(const TreeEventArgs * arg)
    {
        refresh();
        signal(SelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onSortMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SortModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onMultiSelectMode(const WidgetEventArgs * arg)
    {
        signal(MultiSelectModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onVScrollbarMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(VScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onHScrollbarMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(HScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onItemUnfold(const TreeEventArgs * arg)
    {
        refresh();
        signal(ItemUnfoldEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onItemClose(const TreeEventArgs * arg)
    {
        refresh();
        signal(ItemCloseEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tree::onSize(const WidgetEventArgs * arg)
    {
        Container::onSize(arg);
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void Tree::onButtonDown(const CursorEventArgs * arg)
    {
        // base class processing
        // renderImpl();
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            Vector2f localPos(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
            TreeItem * item = getItem(localPos);
            Nmark wck = mSheet->getWinComKey();
            if (item != 0)
            {
                TreeEventArgs args(this);
                args.mItem = item;
                renderImpl();
                const Rectf & barea = item->getToggleArea();
                if ((localPos.x >= barea.left()) && (localPos.x <= barea.right()) &&
                    (localPos.y >= barea.top()) && (localPos.y <= barea.bottom()))
                {
                    item->toggle();
                    if (item->isUnfold())
                    {
                        TreeItem *lastItemInList = item->mItemList[item->getItemCount() - 1];
                        visibleItem(lastItemInList);
                        visibleItem(item);
                        onItemUnfold(&args);
                    }
                    else
                    {
                        onItemClose(&args);
                    }

                    // Update the item screen locations, needed to update the scrollbars.
                    //    renderImpl();

                    // Opened or closed a tree branch, so must update scrollbars.
                    updateScrollbar();
                }
                else
                {
                    // clear old selections if no control key is pressed or if multi-select is off
                    if(!(wck & WCK_KT_A_Ctrl) || !mMultiSelect)
                        unselectImpl();

                    // select range or item, depending upon keys and last selected item
#if 0 // TODO: fix this
                    if(((wck & WCK_KT_A_Shift) && (mCurrentSelect != 0)) && mMultiSelect)
                        selectRange(getItemIndex(item), getItemIndex(mCurrentSelect));
                    else
#endif
                        item->select(item->isSelect() ^ true);

                    // update last selected item
                    mCurrentSelect = item->isSelect() ? item : 0;
                    onSelect(&args);
                }
            }
            else
            {
                // clear old selections if no control key is pressed or if multi-select is off
                if(!(wck & WCK_KT_A_Ctrl) || !mMultiSelect)
                {
                    if(unselectImpl())
                    {
                        // Changes to the selections were actually made
                        TreeEventArgs args(this);
                        args.mItem = item;
                        onSelect(&args);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Tree::onCursorDrag(const CursorEventArgs * arg)
    {
        // base class processing.
        Widget::onCursorDrag(arg);

        if(mVScrollbar->isFinalVisible() && (mVScrollbar->getDocSize() > mVScrollbar->getPageSize()))
            mVScrollbar->setCurrent(mVScrollbar->getCurrent() + mVScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        else if(mHScrollbar->isFinalVisible() && (mHScrollbar->getDocSize() > mHScrollbar->getPageSize()))
            mHScrollbar->setCurrent(mHScrollbar->getCurrent() + mHScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
    }
    //------------------------------------------------------------------------
    void Tree::onCursorMove(const CursorEventArgs * arg)
    {
        if(mTipsEnable)
        {
            static TreeItem * lastItem = 0;

            Vector2f posi(arg->mMouseArg.mScroll - getActualPosX(), arg->mMouseArg.mScroll - getActualPosY());
            TreeItem * item = getItem(posi);
            if(item != lastItem)
            {
                if (item != 0)
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
    void Tree::visibleItem(const TreeItem * item)
    {
        if (!item)
            return;

        NIIf top = 0;
        if (!getHeightToItemInList(mItemList, item, 0, &top))
            return;  // item wasn't found by getHeightToItemInList

        // calculate height to bottom of item
        NIIf bottom = top + item->getPixelSize().mHeight;

        // account for current scrollbar value
        const NIIf currPos = mVScrollbar->getCurrent();
        top -= currPos;
        bottom -= currPos;

        const NIIf listHeight = getTreeRenderArea().getHeight();

        // if top is above the view area, or if item is too big to fit
        if ((top < 0.0f) || ((bottom - top) > listHeight))
        {
            // scroll top of item to top of box.
            mVScrollbar->setCurrent(currPos + top);
        }
        // if bottom is below the view area
        else if (bottom >= listHeight)
        {
            // position bottom of item at the bottom of the list
            mVScrollbar->setCurrent(currPos + bottom - listHeight);
        }
    }
    //------------------------------------------------------------------------
    bool Tree::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdSort());
        dest->add(N_new CmdMultiSelect());
        dest->add(N_new CmdForceVertScrollbar());
        dest->add(N_new CmdForceHorzScrollbar());
        dest->add(N_new CmdItemTooltips());
        return true;
    }
    //------------------------------------------------------------------------
    bool Tree::removeAllImpl()
    {
        // just return false if the list is already empty
        if (getItemCount() == 0)
        {
            return false;
        }
        // we have items to be removed and possible deleted
        else
        {
            NCount i, iend = getItemCount();
            for(i = 0; i < iend; ++i)
            {
                // if item is supposed to be deleted by us
                if (mItemList[i]->isAutoDestroy())
                {
                    // clean up this item.
                    N_delete mItemList[i];
                }
            }

            // clear out the list.
            mItemList.clear();
            mCurrentSelect = 0;
            return true;
        }
    }
    //------------------------------------------------------------------------
    void Tree::ScrollPrc(const EventArgs *)
    {
        refresh();
    }
    //------------------------------------------------------------------------
    void Tree::onFont(const FontEventArgs * args)
    {
        Widget::onFont(args);
        bool res = false;
        const Font * font = args->mFont;
        NCount i, iend = getItemCount();
        for(i = 0; i < iend; ++i)
        {
            res |= mItemList[i]->notifyFont(font);
        }

        if(res)
        {
            refresh();
        }
    }
    //------------------------------------------------------------------------
    bool less(const TreeItem * a, const TreeItem * b)
    {
        return *a < *b;
    }
    //------------------------------------------------------------------------
    bool greater(const TreeItem * a, const TreeItem * b)
    {
        return *a > *b;
    }
    //------------------------------------------------------------------------
    /*
    Named Areas:
            - ItemRenderingArea
            - ItemRenderingAreaHScroll
            - ItemRenderingAreaVScroll
            - ItemRenderingAreaHVScroll

    Child Widgets:
        Scrollbar based widget with name suffix "__auto_vscrollbar__"
        Scrollbar based widget with name suffix "__auto_hscrollbar__"
    */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TreeView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    const WidgetID TreeView::VScrollbarID(1);           //"__auto_vscrollbar__"
    const WidgetID TreeView::HScrollbarID(2);           //"__auto_hscrollbar__"
    //------------------------------------------------------------------------
    TreeView::TreeView(WidgetViewlID wsid) :
        WidgetView(wsid, N_VIEW_Tree)
    {
    }
    //------------------------------------------------------------------------
    TreeView::~TreeView()
    {
    }
    //------------------------------------------------------------------------
    Rectf TreeView::getTreeRenderArea(void) const
    {
        // get Style for the assigned look.
        const Style * style = getStyle();
        Tree * tree = (Tree *)mSrc;

        bool v_visible = tree->getVScrollbar()->isVisible();
        bool h_visible = tree->getHScrollbar()->isVisible();
        Nid sid;
        // if either of the scrollbars are visible, we might want to use another text rendering area
        if (v_visible || h_visible)
        {
            if (h_visible)
            {
                if (v_visible)
                {
                    sid = ItemRenderingAreaHVScroll;
                }
                else
                {
                    sid = ItemRenderingAreaHScroll;
                }
            }
            else
            {
                if (v_visible)
                {
                    sid = ItemRenderingAreaVScroll;
                }
            }

            if (style->isSpecialExist(sid))
            {
                return style->getSpecial(sid).getArea().getPixelRect(*tree);
            }
        }

        return style->getSpecial(ItemRenderingArea).getArea().getPixelRect(*tree);
    }
    //------------------------------------------------------------------------
    void TreeView::flush()
    {
        Tree * tree = (Tree *)mSrc;
        //Set the render area for this.
        Rectf rect = getTreeRenderArea();
        tree->setItemRenderArea(rect);
        const Style * style = getStyle();
        const StyleLayout * ss = &style->getLayout(tree->isFinalDisable() ? DisableState : NormalState);
        ss->render(tree);
        tree->doScrollbars();
        tree->doTreeRender();
    }
    //------------------------------------------------------------------------
}
}
