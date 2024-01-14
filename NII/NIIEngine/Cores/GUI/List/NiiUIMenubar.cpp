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
#include "NiiUIMenubar.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWindow.h"
#include "NiiLogManager.h"

#define HasPopupContentSize     1

#define ItemRenderArea          1
#define ContentSize             2

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MenuItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdPopupOffset : public PropertyCmd
    {
    public:
        CmdPopupOffset() : PropertyCmd(N_PropertyUIMenuItem_PopupOffset, _T("popup_offset"), _T(""))
        {}
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelVector2f & temp = static_cast<const MenuItem *>(own)->getPopupOffset();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"), temp.x.mScale, temp.x.mOffset, temp.y.mScale, temp.y.mOffset);

            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            RelVector2f temp;
            Nsscanf(val.c_str(), _T("%g %g %g %g"), &temp.x.mScale, &temp.x.mOffset, &temp.y.mScale, &temp.y.mOffset);

            static_cast<MenuItem *>(own)->setPopupOffset(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelVector2f * temp = (RelVector2f *)out;
            *temp = static_cast<const MenuItem *>(own)->getPopupOffset();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelVector2f * temp = (RelVector2f *)in;
            static_cast<MenuItem *>(own)->setPopupOffset(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPopupOffset();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
    };

    class _EngineInner CmdAutoPopupTimeout : public PropertyCmd
    {
    public:
        CmdAutoPopupTimeout() : PropertyCmd(N_PropertyUIMenuItem_AutoPopupTimeout, _T("auto_popup_timeout"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MenuItem *>(own)->getAutoPopupTimeout(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            TimeDurMS temp;
            StrConv::conv(val, temp);
            static_cast<MenuItem *>(own)->setAutoPopupTimeout(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            TimeDurMS * temp = (TimeDurMS *)out;
            *temp = static_cast<const MenuItem *>(own)->getAutoPopupTimeout();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            TimeDurMS * temp = (TimeDurMS *)in;
            static_cast<MenuItem *>(own)->setAutoPopupTimeout(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoPopupTimeout();
        }

        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(ListItem, MenuItem, ClickEvent, 0);
    N_EVENT(ListItem, MenuItem, EventCount, 1);
    //------------------------------------------------------------------------
    MenuItem::MenuItem(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        ListItem(wid, fid, own, name, N_CmdObj_MenuItem, lid),
        mPress(false),
        mOver(false),
        mOpen(false),
        mPopupClosing(false),
        mPopupOpening(false),
        mAutoPopupTimeout(0),
        mAutoPopupCost(0),
        mDest(0)
    {
        notifySignal(MenuItem::EventCount);
        // menuitems dont want multi-click events
        setCursorMultiClick(false);
        mPopupOffset.x = RelPosf(0, 0);
        mPopupOffset.y = RelPosf(0, 0);
        setupCmd(N_CmdObj_MenuItem);
    }
    //------------------------------------------------------------------------
    MenuItem::~MenuItem()
    {
    }
    //------------------------------------------------------------------------
    void MenuItem::update(const Vector2f & cursorpos)
    {
        bool oldstate = mOver;

        // assume not hovering
        mOver = false;

        // if input is captured, but not by 'this', then we never hover highlight
        const Widget * capture_wnd = mSheet->getFocus();

        if (capture_wnd == 0)
            mOver = (mSheet->getCursorOver() == this && intersects(cursorpos));
        else
            mOver = (capture_wnd == this && intersects(cursorpos));

        // if state has changed, trigger a re-draw
        // and possible make the parent menu open another popup
        if (oldstate != mOver)
        {
            // are we attached to a menu ?
            Menubar * menu = dynamic_cast<Menubar *>(mParent);
            if (menu)
            {
                if (mOver)
                {
                    // does this menubar only allow one popup open? and is there a popup open?
                    const MenuItem * curpopup = menu->getPopupItem();

                    if (!menu->isMultiPopup())
                    {
                        if (curpopup != this && curpopup != 0)
                        {
                            if (!isAutoPopup())
                            {
                                // open this popup instead
                                openPopupMenu();
                            }
                            else
                            {
                                // start close timer on current popup
                                MenuItem * tmi = menu->getPopupItem();
                                if (tmi)
                                {
                                    tmi->toggleClose();
                                }
                                toggleOpen();
                            }
                        }
                        else
                        {
                            toggleOpen();
                        }
                    }
                }
            }
            refresh();
        }
    }
    //------------------------------------------------------------------------
    void MenuItem::setDestImpl(PopupMenu * popup, bool add_as_child)
    {
        // is it the one we have already ?
        if (popup == mDest)
        {
            // then do nothing;
            return;
        }

        // keep the old one around
        PopupMenu * old_popup = mDest;
        // update the internal state pointer
        mDest = popup;
        mOpen = false;

        // is there already a popup ?
        if (old_popup)
        {
            remove(old_popup);

            // should we destroy it as well?
            if (old_popup->isAutoDestroy())
            {
                // then do so
                N_OnlyPtr(Widget)->destroy(old_popup);
            }
        }
        // we are setting a new popup and not just clearing. and we are told to add the child
        if (popup != 0 && add_as_child)
        {
            add(popup);
        }
        refresh();
    }
    //------------------------------------------------------------------------
    void MenuItem::openPopupMenu(bool notify)
    {
        if (mDest == 0 || mOpen)
            return;

        mPopupOpening = false;
        mPopupClosing = false;

        // should we notify ?
        // if so, and we are attached to a menu bar or popup menu, we let it handle the "activation"
        Widget * p = mParent;

        if (notify && p)
        {
            if (dynamic_cast<Menubar *>(p))
            {
                // align the popup to the bottom-left of the menuitem
                RelVector2f pos(RelPosf(0, 0), RelPosf(0, mPixelSize.mHeight));
                mDest->setPos(pos + mPopupOffset);

                static_cast<Menubar *>(p)->setPopupItem(this);
                return; // the rest is handled when the menu bar eventually calls us itself
            }
            // or maybe a popup menu?
            else if (dynamic_cast<PopupMenu *>(p))
            {
                // align the popup to the top-right of the menuitem
                RelVector2f pos(RelPosf(0, mPixelSize.mWidth), RelPosf(0, 0));
                mDest->setPos(pos + mPopupOffset);

                static_cast<PopupMenu *>(p)->setPopupItem(this);
                return; // the rest is handled when the popup menu eventually calls us itself
            }
        }
        // by now we must handle it ourselves
        // match up with Menubar::changePopupMenu
        mDest->toggleOpen(false);

        mOpen = true;
        refresh();
    }
    //------------------------------------------------------------------------
    void MenuItem::closePopupMenu(bool notify)
    {
        // no popup? or not open...
        if (mDest == 0 || !mOpen)
            return;

        mPopupOpening = false;
        mPopupClosing = false;

        // should we notify the parent menu base?
        // if we are attached to a menu base, we let it handle the "deactivation"
        Menubar * menu = dynamic_cast<Menubar *>(mParent);
        if (notify && menu)
        {
            // only if the menu base does not allow multiple popups
            if (!menu->isMultiPopup())
            {
                menu->setPopupItem(0);
                return; // the rest is handled when the menu base eventually call us again itself
            }
        }
        // otherwise we do ourselves
        else
        {
            // match up with Menubar::changePopupMenu
            //mDest->hide();
            mDest->toggleClose(false);
        }
        mOpen = false;
        refresh();
    }
    //------------------------------------------------------------------------
    bool MenuItem::togglePopupMenu()
    {
        if (mOpen)
        {
            closePopupMenu();
            return false;
        }

        openPopupMenu();
        return true;
    }
    //------------------------------------------------------------------------
    void MenuItem::toggleClose()
    {
        mPopupOpening = false;

        if (mOpen)
        {
            mAutoPopupCost = 0;
            mPopupClosing = true;
            refresh();
        }
        else
        {
            mPopupClosing = false;
        }
    }
    //------------------------------------------------------------------------
    void MenuItem::toggleOpen()
    {
        mPopupClosing = false;

        if (mOpen)
        {
            mPopupOpening = false;
        }
        else
        {
            mAutoPopupCost = 0;
            mPopupOpening = true;
        }
    }
    //------------------------------------------------------------------------
    void MenuItem::closeAllPopup()
    {
        if (mParent)
        {
            if (dynamic_cast<Menubar *>(mParent))
            {
                closePopupMenu();
                return;
            }

            PopupMenu * pop = dynamic_cast<PopupMenu *>(mParent);
            if (pop)
            {
                MenuItem * mi = dynamic_cast<MenuItem *>(pop->getParent());
                if (mi)
                {
                    mi->closeAllPopup();
                }
                else
                {
                    pop->toggleClose(false);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MenuItem::onClick(const WidgetEventArgs * arg)
    {
        // close the popup if we did'nt spawn a child
        if (!mOpen && !mClose)
        {
            closeAllPopup();
        }

        mClose = false;
        signal(ClickEvent, arg);
    }
    //------------------------------------------------------------------------
    void MenuItem::onCursorMove(const CursorEventArgs * arg)
    {
        // this is needed to discover whether mouse is in the widget area or not.
        // The same thing used to be done each frame in the rendering method,
        // but in this version the rendering method may not be called every frame
        // so we must discover the internal widget state here - which is actually
        // more efficient anyway.

        // base class processing
        ListItem::onCursorMove(arg);

        update(Vector2f(arg->mLocalX, arg->mLocalY));
    }
    //------------------------------------------------------------------------
    void MenuItem::onButtonDown(const CursorEventArgs * arg)
    {
        ListItem::onButtonDown(arg);

        if (arg->mMouseArg.mButton == MB_Left)
        {
            mClose = false;

            if (focus())
            {
                mPress = true;
                update(Vector2f(arg->mLocalX, arg->mLocalY));
                mClose = !togglePopupMenu();
                refresh();
            }
        }
    }
    //------------------------------------------------------------------------
    void MenuItem::onButtonUp(const CursorEventArgs * arg)
    {
        ListItem::onButtonUp(arg);
        if (arg->mMouseArg.mButton == MB_Left)
        {
            lost();

            // was the button released over this window?
            // (use mouse position, as e.mX e.mY in args has been unprojected)
            if (!mClose)
            {
                if (mSheet->getFocus() == this || (mSheet->getFocus()->isContainer() &&
                    static_cast<Container *>(mSheet->getFocus())->getTarget(mSheet->getCursor()->getPos()) == this))
                {
                    WidgetEventArgs we(this);
                    onClick(&we);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MenuItem::onLost(const WidgetEventArgs * arg)
    {
        ListItem::onLost(arg);
        mPress = false;
        update(mParent->getWindow()->affine(mSheet->getCursor()->getPos()));
        refresh();
    }
    //------------------------------------------------------------------------
    void MenuItem::onCursorLeave(const CursorEventArgs * arg)
    {
        ListItem::onCursorLeave(arg);
        mOver = false;
        refresh();
    }
    //------------------------------------------------------------------------
    void MenuItem::onText(const WidgetEventArgs * arg)
    {
        ListItem::onText(arg);

        // if we are attached to a ListContainer, we make it update as necessary
        ListContainer * ilb = dynamic_cast<ListContainer *>(mParent);
        if (ilb)
        {
            ilb->notifyItemUpdate();
        }
    }
    //------------------------------------------------------------------------
    void MenuItem::updateImpl(TimeDurMS elapsed)
    {
        ListItem::updateImpl(elapsed);

        //handle delayed popup closing/opening when hovering with the mouse
        if (mAutoPopupTimeout != 0 && (mPopupOpening || mPopupClosing))
        {
            // stop timer if the hovering state isn't set appropriately anymore
            if (mOver)
            {
                mPopupClosing = false;
            }
            else
            {
                mPopupOpening = false;
            }

            //check if the timer elapsed and take action appropriately
            mAutoPopupCost += elapsed;

            if (mAutoPopupCost > mAutoPopupTimeout)
            {
                if (mPopupOpening)
                {
                    mPopupOpening = false;
                    openPopupMenu(true);
                }
                else if (mPopupClosing)
                {
                    mPopupClosing = false;
                    closePopupMenu(true);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    bool MenuItem::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdPopupOffset());
        dest->add(N_new CmdAutoPopupTimeout());
        return true;
    }
    //------------------------------------------------------------------------
    /*Named Areas:
        ContentSize
        HasPopupContentSize
        */
        //------------------------------------------------------------------------
        //------------------------------------------------------------------------
        // MenuItemView
        //------------------------------------------------------------------------
        //------------------------------------------------------------------------
    N_STATE(ListItemView, MenuItemView, EnableHoverState, 0);
    N_STATE(ListItemView, MenuItemView, EnablePushState, 1);
    N_STATE(ListItemView, MenuItemView, EnablePushOffState, 2);
    N_STATE(ListItemView, MenuItemView, EnablePopupOpenState, 3);
    N_STATE(ListItemView, MenuItemView, DisableHoverState, 4);
    N_STATE(ListItemView, MenuItemView, DisablePushState, 5);
    N_STATE(ListItemView, MenuItemView, DisablePushOffState, 6);
    N_STATE(ListItemView, MenuItemView, DisablePopupOpenState, 7);
    N_STATE(ListItemView, MenuItemView, PopupCloseIconState, 8);
    N_STATE(ListItemView, MenuItemView, PopupOpenIconState, 9);
    N_STATE(ListItemView, MenuItemView, StateCount, 10);
    //------------------------------------------------------------------------
    MenuItemView::MenuItemView(WidgetViewlID wsid) :
        ListItemView(wsid)
    {
    }
    //------------------------------------------------------------------------
    PlaneSizef MenuItemView::getItemPixelSize() const
    {
        return getContentNamedArea().getArea().getPixelRect(*mSrc).getSize();
    }
    //------------------------------------------------------------------------
    const StyleSpecial & MenuItemView::getContentNamedArea() const
    {
        const Style * style = getStyle();

        if (static_cast<MenuItem *>(mSrc)->getDest() && style->isSpecialExist(HasPopupContentSize) &&
            !(mSrc->getParent() && dynamic_cast<const Menubar *>(mSrc->getParent())))
        {
            return style->getSpecial(HasPopupContentSize);
        }
        else
        {
            return style->getSpecial(ContentSize);
        }
    }
    //------------------------------------------------------------------------
    void MenuItemView::flush()
    {
        MenuItem * w = static_cast<MenuItem *>(mSrc);
        // build name of state we're in
        StateID stateName = w->isFinalDisable() ? DisableState : NormalState;

        StateID suffix;

        // only show opened mode if the menu items popup window is not closing
        // (otherwise it might look odd)
        if (w->isOpen() && !(w->isAutoPopup() && w->isPopupClosing()))
        {
            if (stateName == DisableState)
                suffix = DisablePopupOpenState;
            else
                suffix = EnablePopupOpenState;
        }
        else if (w->isPress())
        {
            if (stateName == DisableState)
                suffix = w->isHover() ? DisablePushState : DisablePushOffState;
            else
                suffix = w->isHover() ? EnablePushState : EnablePushOffState;
        }
        else if (w->isHover())
        {
            if (stateName == DisableState)
                suffix = DisableHoverState;
            else
                suffix = EnableHoverState;
        }
        const StyleLayout * mode;
        const Style * style = getStyle();

        // try and get mode for our current state
        if (style->isStateExist(suffix))
        {
            mode = &style->getLayout(suffix);
        }
        else
        {
            mode = &style->getLayout(stateName);
        }

        // peform the rendering operation.
        mode->render(w);
        // only draw popup-open/closed-icon if we have a popup menu, and parent is not a menubar
        bool not_menubar = (!w->getParent()) ? true : !dynamic_cast<Menubar *>(w->getParent());

        if (w->getDest() && not_menubar)
        {
            // get mode for popup open/closed state
            mode = &style->getLayout(w->isOpen() ? PopupOpenIconState : PopupCloseIconState);
            // peform the rendering operation.
            mode->render(w);
        }
    }
    //------------------------------------------------------------------------
    bool MenuItemView::notifyFont(const Font * obj)
    {
        if (getContentNamedArea().notifyFont(mSrc, obj))
        {
            if (mSrc->getParent())
                mSrc->getParent()->layout();

            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Menubar
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdItemSpacing : public PropertyCmd
    {
    public:
        CmdItemSpacing() : PropertyCmd(N_PropertyUIMenubar_ItemSpace, _T("item_spacing"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Menubar *>(own)->getItemSpace(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Menubar *>(own)->setItemSpace(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Menubar *>(own)->getItemSpace();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Menubar *>(own)->setItemSpace(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdItemSpacing();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("10.0");
        }
    };

    class _EngineInner CmdAllowMultiplePopups : public PropertyCmd
    {
    public:
        CmdAllowMultiplePopups() : PropertyCmd(N_PropertyUIMenubar_MultiPopup, _T("allow_multiple_popups"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Menubar *>(own)->isMultiPopup(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Menubar *>(own)->setMultiPopup(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Menubar *>(own)->isMultiPopup();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Menubar *>(own)->setMultiPopup(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAllowMultiplePopups();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdAutoCloseNestedPopups : public PropertyCmd
    {
    public:
        CmdAutoCloseNestedPopups() : PropertyCmd(N_PropertyUIMenubar_AutoClosePopup, _T("auto_close_nested_popups"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const Menubar *>(own)->isAutoClosePopup();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Menubar *>(own)->setAutoClosePopup(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Menubar *>(own)->isAutoClosePopup();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Menubar *>(own)->setAutoClosePopup(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoCloseNestedPopups();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(ListContainer, Menubar, OpenPopupEvent,         0);
    N_EVENT(ListContainer, Menubar, ClosePopupEvent,        1);
    N_EVENT(ListContainer, Menubar, EventCount,             2);
    //------------------------------------------------------------------------
    Menubar::Menubar(WidgetID wid, FactoryID fid, Container * own, const String & name,
        ScriptTypeID stid, LangID lid) :
        ListContainer(wid, fid, own, name, stid, lid),
        mPopupItem(0),
        mMultiPopups(false),
        mAutoClosePopups(false)
    {
        notifySignal(Menubar::EventCount);
        setupCmd(N_CmdObj_Menubar);
        mItemSpace = 10;
    }
    //------------------------------------------------------------------------
    Menubar::~Menubar()
    {
    }
    //------------------------------------------------------------------------
    void Menubar::layoutContent()
    {
        Rectf render_rect = getItemArea();
        NIIf x0 = N_ToPixel(render_rect.left());

        RelRectf rect;

        ListEntryList::iterator item = mItemList.begin();
        while(item != mItemList.end())
        {
            const PlaneSizef & optimal = (*item)->getItemPixelSize();

            (*item)->setYAlign(VA_CENTRE);
           // rect.setPos(RelVector2f(RelPosf(0, x0), RelPosf(0, 0)));
           // rect.setSize(RelPlaneSizef(RelPosf(0, N_ToPixel(optimal.mWidth)),
           //     RelPosf(0, N_ToPixel(optimal.mHeight))));

            //(*item)->setRelArea(RelRectf(rect.mLeft, rect.mTop, rect.getSize()));

            (*item)->setRelArea(RelRectf(RelVector2f(RelPosf(0, x0), RelPosf(0, 0)), 
                RelPlaneSizef(RelPosf(0, N_ToPixel(optimal.mWidth)), RelPosf(0, N_ToPixel(optimal.mHeight)))));

            x0 += optimal.mWidth + mItemSpace;
            ++item;
        }
    }
    //------------------------------------------------------------------------
    PlaneSizef Menubar::getContentSize() const
    {
        // find the content sizes
        NIIf tallest = 0;
        NIIf total_width = 0;

        NCount i = 0;
        NCount max = mItemList.size();
        while(i < max)
        {
            const PlaneSizef & sz = mItemList[i]->getItemPixelSize();
            if(sz.mHeight > tallest)
                tallest = sz.mHeight;
            total_width += sz.mWidth;

            i++;
        }

        const NIIf count = NIIf(i);

        // horz item spacing
        if(count >= 2)
        {
            total_width += (count-1) * mItemSpace;
        }

        // return the content size
        return PlaneSizef(total_width, tallest);
    }
    //------------------------------------------------------------------------
    void Menubar::setPopupItem(MenuItem * item)
    {
        if(!mMultiPopups && mPopupItem == item)
            return;

        if(!mMultiPopups && mPopupItem != 0)
        {
            WidgetEventArgs we(mPopupItem->getDest());
            mPopupItem->closePopupMenu(false);
            mPopupItem = 0;
            onClosePopup(&we);
        }

        if(item)
        {
            mPopupItem = item;
            mPopupItem->openPopupMenu(false);
            WidgetEventArgs we(mPopupItem->getDest());
            onOpenPopup(&we);
        }
    }
    //------------------------------------------------------------------------
    void Menubar::setItemSpace(NIIf spacing)
    {
        mItemSpace = spacing;
        notifyItemUpdate();
    }
    //------------------------------------------------------------------------
    void Menubar::onOpenPopup(const WidgetEventArgs * arg)
    {
        signal(OpenPopupEvent, arg);
    }
    //------------------------------------------------------------------------
    void Menubar::onClosePopup(const WidgetEventArgs * arg)
    {
        signal(ClosePopupEvent, arg);
    }
    //------------------------------------------------------------------------
    bool Menubar::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdItemSpacing());
        dest->add(N_new CmdAllowMultiplePopups());
        dest->add(N_new CmdAutoCloseNestedPopups());
        return true;
    }
    //------------------------------------------------------------------------
    void Menubar::setMultiPopup(bool b)
    {
        if(mMultiPopups != b)
        {
            // TODO :
            // close all popups except perhaps the last one opened!
            mMultiPopups = b;
        }
    }
    //------------------------------------------------------------------------
    void Menubar::onRemove(const WidgetEventArgs * e)
    {
        // if the removed window was our tracked popup item, zero ptr to it.
        if(e->mWidget == mPopupItem)
            mPopupItem = 0;

        // base class version
        ListContainer::onRemove(e);
    }
    //------------------------------------------------------------------------
    void Menubar::onHide(const WidgetEventArgs *)
    {
        if(!isAutoClosePopup())
            return;

        setPopupItem(0);

        if(mMultiPopups)
        {
            NCount i, iend = mItemList.size();
            for(i = 0; i < iend; ++i)
            {
                if(!mItemList[i])
                    continue;

                MenuItem * menuItem = dynamic_cast<MenuItem *>(mItemList[i]);
                if(!menuItem)
                    continue;

                if(!menuItem->getDest())
                    continue;

                WidgetEventArgs we(menuItem->getDest());
                menuItem->closePopupMenu(false);
                onClosePopup(&we);
            }
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DefaultListContainerView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(ListContainerView, DefaultListContainerView, ItemRenderAreaState, 0);
    N_STATE(ListContainerView, DefaultListContainerView, StateCount, 1);
    //------------------------------------------------------------------------
    DefaultListContainerView::DefaultListContainerView(WidgetViewlID wsid) :
        ListContainerView(wsid)
    {
    }
    //------------------------------------------------------------------------
    void DefaultListContainerView::flush()
    {
        // get Style for the assigned look.
        const Style * style = getStyle();
        // try and get mode for our current state
        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        // peform the rendering operation.
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    Rectf DefaultListContainerView::getRenderArea() const
    {
        const Style * style = getStyle();
        return style->getSpecial(ItemRenderArea).getArea().getPixelRect(*mSrc);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PopupMenu
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdFadeInTime : public PropertyCmd
    {
    public:
        CmdFadeInTime() : PropertyCmd(N_PropertyUIFadeInTime, _T("fade_in_time"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const PopupMenu *>(own)->getFadeInTime(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<PopupMenu *>(own)->setFadeInTime(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const PopupMenu *>(own)->getFadeInTime();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<PopupMenu *>(own)->setFadeInTime(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFadeInTime();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdFadeOutTime : public PropertyCmd
    {
    public:
        CmdFadeOutTime() : PropertyCmd(N_PropertyUIFadeOutTime, _T("fade_out_time"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const PopupMenu *>(own)->getFadeOutTime(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<PopupMenu *>(own)->setFadeOutTime(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const PopupMenu *>(own)->getFadeOutTime();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<PopupMenu *>(own)->setFadeOutTime(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFadeOutTime();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //------------------------------------------------------------------------
    PopupMenu::PopupMenu(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Menubar(wid, fid, own, name, N_CmdObj_PopupMenu, lid),
        mOrgAlpha(mAlpha),
        mPopupFadeOut(0),
        mPopupFadeIn(0),
        mFading(false),
        mFadeOut(false),
        mOpenState(false)
    {
        mItemSpace = 2;

        // enable auto resizing
        mAutoResize = true;

        // disable parent clipping
        setAutoClip(false);

        // hide by default
        hide();

        setupCmd(N_CmdObj_PopupMenu);
    }
    //------------------------------------------------------------------------
    PopupMenu::~PopupMenu()
    {
    }
    //------------------------------------------------------------------------
    void PopupMenu::toggleOpen(bool notify)
    {
        // already open and not fading, or fading in?
        if (mOpenState && (!mFading || !mFadeOut))
        {
            // then don't do anything
            return;
        }

        // should we let the parent menu item initiate the open?
        if (notify && mParent && dynamic_cast<MenuItem *>(mParent))
        {
            static_cast<MenuItem *>(mParent)->openPopupMenu();
            return; // the rest will be handled when MenuItem calls us itself
        }

        // we'll handle it ourselves then.
        // are we fading, and fading out?
        if (mFading && mFadeOut)
        {
            if (mPopupFadeIn > 0.0f && mPopupFadeOut > 0.0f)
            {
                // jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
                mFadeCost = ((mPopupFadeOut - mFadeCost) / mPopupFadeOut)*mPopupFadeIn;
            }
            else
            {
                // start the fade in from the beginning
                mFadeCost = 0;
            }
            // change to fade in
            mFadeOut = false;
        }
        // otherwise just start normal fade in!
        else if (mPopupFadeIn > 0.0f)
        {
            mFading = true;
            mFadeOut = false;
            setAlpha(0.0f);
            mFadeCost = 0;
        }
        // should not fade!
        else
        {
            mFading = false;
            setAlpha(mOrgAlpha);
        }

        show();
        moveFront();
    }
    //------------------------------------------------------------------------
    void PopupMenu::toggleClose(bool notify)
    {
        // already closed?
        if (!mOpenState)
        {
            // then do nothing
            return;
        }

        // should we let the parent menu item close initiate the close?
        if (notify && mParent && dynamic_cast<MenuItem *>(mParent))
        {
            static_cast<MenuItem *>(mParent)->closePopupMenu();
            return; // the rest will be handled when MenuItem calls us itself
        }

        // we'll do it our selves then.
        // are we fading, and fading in?
        if (mFading && !mFadeOut)
        {
            // make sure the "fade back out" is smooth - if possible !
            if (mPopupFadeOut > 0.0f&&mPopupFadeIn > 0.0f)
            {
                // jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
                mFadeCost = ((mPopupFadeIn - mFadeCost) / mPopupFadeIn)*mPopupFadeOut;
            }
            else
            {
                // start the fade in from the beginning
                mFadeCost = 0;
            }
            // change to fade out
            mFadeOut = true;
        }
        // otherwise just start normal fade out!
        else if (mPopupFadeOut > 0.0f)
        {
            mFading = true;
            mFadeOut = true;
            setAlpha(mOrgAlpha);
            mFadeCost = 0;
        }
        // should not fade!
        else
        {
            mFading = false;
            hide();
        }
    }
    //------------------------------------------------------------------------
    void PopupMenu::updateImpl(TimeDurMS elapse)
    {
        Menubar::updateImpl(elapse);

        // handle fading
        if (mFading)
        {
            mFadeCost += elapse;

            // fading out
            if (mFadeOut)
            {
                if (mFadeCost >= mPopupFadeOut)
                {
                    hide();
                    mFading = false;
                    setAlpha(mOrgAlpha); // set real alpha so users can show directly without having to restore it
                }
                else
                {
                    setAlpha(mOrgAlpha * (mPopupFadeOut - mFadeCost) / mPopupFadeOut);
                }

            }

            // fading in
            else
            {
                if (mFadeCost >= mPopupFadeIn)
                {
                    mFading = false;
                    setAlpha(mOrgAlpha);
                }
                else
                {
                    setAlpha(mOrgAlpha * mFadeCost / mPopupFadeIn);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void PopupMenu::layoutContent()
    {
        // get render area
        Rectf render_rect = getItemArea();

        // get starting position
        NIIf x0 = N_ToPixel(render_rect.mLeft);
        NIIf y0 = N_ToPixel(render_rect.mTop);

        RelRectf rect;
        RelVector2f sz(RelPosf(0, N_ToPixel(render_rect.getWidth())), RelPosf(0, 0)); // set item width

        // iterate through all items attached to this window
        ListEntryList::iterator i, iend = mItemList.end();
        for (i = mItemList.begin(); i != iend; ++i)
        {
            // get the "optimal" height of the item and use that!
            sz.y.mOffset = N_ToPixel((*i)->getItemPixelSize().mHeight); // rounding errors ?

            // set destination rect
            rect.setPos(RelVector2f(RelPosf(0, x0), RelPosf(0, y0)));
            // todo: vector vs size
            rect.setSize(RelPlaneSizef(sz.x, sz.y));
            (*i)->setRelArea(RelRectf(rect.mLeft, rect.mTop, rect.getSize()));
            // next position
            y0 += N_ToPixel(sz.y.mOffset + mItemSpace);
        }
    }
    //------------------------------------------------------------------------
    PlaneSizef PopupMenu::getContentSize() const
    {
        NIIf widest = 0;
        NIIf total_height = 0;

        NCount i, iend = mItemList.size();
        for (i = 0; i < iend; ++i)
        {
            PlaneSizef sz = mItemList[i]->getItemPixelSize();
            if (sz.mWidth > widest)
                widest = sz.mWidth;
            total_height += sz.mHeight;
        }

        const NIIf count = NIIf(i);

        if (count >= 2)
        {
            total_height += (count - 1) * mItemSpace;
        }

        // return the content size
        return PlaneSizef(widest, total_height);
    }
    //------------------------------------------------------------------------
    void PopupMenu::onAlpha(const WidgetEventArgs * arg)
    {
        Menubar::onAlpha(arg);

        // if we are not fading, this is a real alpha change request and we save a copy of the value
        if (!mFading)
        {
            mOrgAlpha = mAlpha;
        }
    }
    //------------------------------------------------------------------------
    void PopupMenu::onDestroy(const WidgetEventArgs * arg)
    {
        // if we are attached to a menuitem, we make sure that gets updated
        MenuItem * temp = dynamic_cast<MenuItem *>(mParent);
        if (temp)
        {
            temp->setDest(0);
        }
        Menubar::onDestroy(arg);
    }
    //------------------------------------------------------------------------
    void PopupMenu::onShow(const WidgetEventArgs * arg)
    {
        mOpenState = true;
        Menubar::onShow(arg);
    }
    //------------------------------------------------------------------------
    void PopupMenu::onHide(const WidgetEventArgs * arg)
    {
        mOpenState = false;
        Menubar::onHide(arg);
    }
    //------------------------------------------------------------------------
    void PopupMenu::onButtonDown(const CursorEventArgs * arg)
    {
        Menubar::onButtonDown(arg);
    }
    //------------------------------------------------------------------------
    void PopupMenu::onButtonUp(const CursorEventArgs * arg)
    {
        Menubar::onButtonUp(arg);
    }
    //------------------------------------------------------------------------
    bool PopupMenu::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdFadeInTime());
        dest->add(N_new CmdFadeOutTime());
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PopupMenuView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(ListContainerView, PopupMenuView, ItemRenderAreaState, 0);
    N_STATE(ListContainerView, PopupMenuView, StateCount, 1);
    //------------------------------------------------------------------------
    PopupMenuView::PopupMenuView(WidgetViewlID wmid) :
        ListContainerView(wmid)
    {
    }
    //------------------------------------------------------------------------
    void PopupMenuView::flush()
    {
        const Style * style = getStyle();
        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    Rectf PopupMenuView::getRenderArea() const
    {
        const Style * style = getStyle();
        return style->getSpecial(ItemRenderAreaState).getArea().getPixelRect(*mSrc);
    }
    //------------------------------------------------------------------------
}
}