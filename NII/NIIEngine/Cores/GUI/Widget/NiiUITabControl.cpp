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
#include "NiiUITabControl.h"
#include "NiiUIException.h"
#include "NiiUIButton.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWidgetList.h"
#include "NiiLogManager.h"
#include "NiiFont.h"

#define makeButtonName(x) (mID << TabButtonName) + x->getID()
namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdTabHeight : public PropertyCmd
    {
    public:
        CmdTabHeight() : PropertyCmd(N_PropertyUITabControl_TabHeight, _T("tab_height"), _T("Property to get/set the height of the tabs."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelPosf & temp = static_cast<const TabControl *>(own)->getTabHeight();
            Ntchar buff[128];
            Nsntprintf(buff, sizeof(buff), _T("%g %g"), temp.mScale, temp.mOffset);
            out = buff;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            RelPosf temp;
            Nsscanf(val.c_str(), _T("%g %g"), &temp.mScale, &temp.mOffset);
            static_cast<TabControl *>(own)->setTabHeight(temp);
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelPosf * temp = (RelPosf *)out;
            *temp = static_cast<const TabControl *>(own)->getTabHeight();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelPosf * temp = (RelPosf *)in;
            static_cast<TabControl *>(own)->setTabHeight(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTabHeight();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.05 0.0");
        }
    };

    class _EngineInner CmdTabTextPadding : public PropertyCmd
    {
    public:
        CmdTabTextPadding() : PropertyCmd(N_PropertyUITabControl_TabTextPadding, _T("tab_text_padding"),
            _T("Property to get/set the padding either side of the tab buttons."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelPosf & temp = static_cast<const TabControl *>(own)->getTabTextOffset();
            Ntchar buff[128];
            Nsntprintf(buff, sizeof(buff), _T("%g %g"), temp.mScale, temp.mOffset);
            out = buff;
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            RelPosf temp;
            Nsscanf(val.c_str(), _T("%g %g"), &temp.mScale, &temp.mOffset);
            static_cast<TabControl *>(own)->setTabTextOffset(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelPosf * temp = (RelPosf *)out;
            *temp = static_cast<const TabControl *>(own)->getTabTextOffset();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelPosf * temp = (RelPosf *)in;
            static_cast<TabControl *>(own)->setTabTextOffset(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTabTextPadding();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.5");
        }
    };

    class _EngineInner CmdTabPanePosition : public PropertyCmd
    {
    public:
        CmdTabPanePosition() : PropertyCmd(N_PropertyUITabControl_TabType, _T("tab_pane_position"),
            _T("Property to get/set the position of the buttons pane."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            TabControl::TabType temp = static_cast<const TabControl *>(own)->getTabType();
            if (temp == TabControl::Top)
            {
                out = _T("Top");
            }
            else if (temp == TabControl::Bottom)
            {
                out = _T("Bottom");
            }
            else
            {
                out = _T("Top");
            }
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            TabControl::TabType temp;
            if (val == _T("Bottom"))
            {
                temp = TabControl::Bottom;
            }
            else
            {
                temp = TabControl::Top;
            }
            static_cast<TabControl *>(own)->setTabType(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            TabControl::TabType * temp = (TabControl::TabType *)out;
            *temp = static_cast<const TabControl *>(own)->getTabType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            TabControl::TabType * temp = (TabControl::TabType *)in;
            static_cast<TabControl *>(own)->setTabType(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTabPanePosition();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("Top");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, TabControl, SelectEvent,					0);
    N_EVENT(Container, TabControl, EventCount,                  1);
    //------------------------------------------------------------------------
    #define ContentPaneName     1                   //"__auto_TabPane__"
    #define TabButtonName       10                  //"__auto_btn"
    #define TabButtonPaneName   3                   //"__auto_TabPane__Buttons"
    #define ButtonScrollLeft    4                   //"__auto_TabPane__ScrollLeft"
    #define ButtonScrollRight   5                   //"__auto_TabPane__ScrollRight"
    //------------------------------------------------------------------------
    static const String EnableTop = _T("EnableTop");
    static const String EnableBottom = _T("EnableBottom");
    static const String n0 = _T("0");
    static const String n1 = _T("1");
    //------------------------------------------------------------------------
    TabControl::TabControl(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_TabControl, lid),
        mTabHeight(0, -1), // means 'to be initialized later'
        mTextOffset(0, 5),
        mTabOffset(0),
        mTabType(Top)
    {
        notifySignal(TabControl::EventCount);
        setupCmd(N_CmdObj_TabControl);
    }
    //------------------------------------------------------------------------
    TabControl::~TabControl()
    {
        // Should be handled in superclass (all child windows)
    }
    //------------------------------------------------------------------------
    void TabControl::initChild()
    {
        layout();

        Widget * temp = get((mID << N_Level_Bit_Escape) + ButtonScrollLeft, false);
        if(temp)
            temp->bind(PushButton::PushEvent, this, (CommandObjFunctor)&TabControl::TabScrollPrc);

        temp = get((mID << N_Level_Bit_Escape) + ButtonScrollRight, false);
        if(temp)
            temp->bind(PushButton::PushEvent, this, (CommandObjFunctor)&TabControl::TabScrollPrc);
    }
    //------------------------------------------------------------------------
    bool TabControl::isTabContentSelect(Container * obj) const
    {
        for(NCount i = 0; i < mTabList.size (); ++i)
            if(mTabList[i]->getDest () == obj)
                return mTabList [i]->isSelect();

        return false;
    }
    //------------------------------------------------------------------------
    Nidx TabControl::getSelectIndex() const
    {
        Nidx i, iend = mTabList.size();
        for (i = 0; i < iend; ++i)
            if (mTabList [i]->isSelect())
                return i;

        N_EXCEPT(UnknownObject, _I18n("Current tab not in list?"));
    }
    //------------------------------------------------------------------------
    void TabControl::selectTab(const String & name)
    {
        selectTabImpl(getTabContentPane()->get(name));
    }
    //------------------------------------------------------------------------
    void TabControl::selectTab(WidgetID id)
    {
        selectTabImpl(getTabContentPane()->get(id, false));
    }
    //------------------------------------------------------------------------
    void TabControl::selectTab(Nidx index)
    {
        selectTabImpl(getTabContent(index));
    }
    //------------------------------------------------------------------------
    void TabControl::visibleTab(const String & name)
    {
        visibleTabImpl(getTabContentPane()->get(name));
    }
    //------------------------------------------------------------------------
    void TabControl::visibleTab(WidgetID id)
    {
        visibleTabImpl(getTabContentPane()->get(id, false));
    }
    //------------------------------------------------------------------------
    void TabControl::visibleTab(Nidx index)
    {
        visibleTabImpl(getTabContent(index));
    }
    //------------------------------------------------------------------------
    void TabControl::addTab(Container * wnd)
    {
        // abort attempts to add null window pointers, but log it for tracking.
        if(!wnd)
        {
            N_Only(Log).log(_I18n("Attempt to add null window pointer as tab to TabControl '") +
                N_conv(getID()) + _I18n("'.  Ignoring!"));

            return;
        }

        // Create a new TabButton
        TabButton * tb = createTabButton(makeButtonName(wnd));
        tb->setFont(getFont());
        tb->setDest(wnd);
        mTabList.push_back(tb);
        getTabButtonPane()->add(tb);
        tb->bind(TabButton::ClickEvent, this, (CommandObjFunctor)&TabControl::TabButtonClickPrc);
        tb->bind(TabButton::DragEvent, this, (CommandObjFunctor)&TabControl::TabDragPrc);
        tb->bind(TabButton::ScrollEvent, this, (CommandObjFunctor)&TabControl::TabWheelPrc);
        getTabContentPane()->add(wnd);

        if(getTabCount() == 1)
            selectTab(wnd->getID());
        else
            wnd->setVisible(false);

        // when adding the 1st page, autosize tab pane height
        if(mTabHeight.mScale == 0 && mTabHeight.mOffset == -1)
            mTabHeight.mOffset = 8 + getFont()->getFontHeight ();

        // Just request redraw
        layout();
        refresh();
        // Subscribe to text changed event so that we can resize as needed
        mSignalList[wnd] = wnd->bind(Widget::TextEvent, this,
            (CommandObjFunctor)&TabControl::TabTextPrc);
    }
    //------------------------------------------------------------------------
    void TabControl::removeTab(const String & name)
    {
        if(getTabContentPane()->isExist(name))
            removeTabImpl(static_cast<Container *>(getTabContentPane()->get(name)));
    }
    //------------------------------------------------------------------------
    void TabControl::removeTab(WidgetID id)
    {
        if(getTabContentPane()->isExist(id))
            removeTabImpl(static_cast<Container *>(getTabContentPane()->get(id, false)));
    }
    //------------------------------------------------------------------------
    void TabControl::selectTabImpl(Widget * wnd)
    {
        visibleTabImpl(wnd);

        bool modified = false;
        NCount i, iend = mTabList.size();
        for (i = 0; i < iend; ++i)
        {
            // get corresponding tab button and content window
            TabButton * tb = mTabList [i];
            Widget * child = tb->getDest();
            // Should we be selecting?
            bool selectThis = (child == wnd);
            // Are we modifying this tab?
            modified = modified || (tb->isSelect() != selectThis);
            // Select tab & set visible if this is the window, not otherwise
            tb->select(selectThis);
            child->setVisible(selectThis);
        }
        if (modified)
        {
            WidgetEventArgs args(this);
            onSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    void TabControl::visibleTabImpl(Widget * wnd)
    {
        TabButton * tb = 0;
        for(NCount i = 0; i < mTabList.size (); ++i)
        {
            // get corresponding tab button and content window
            tb = mTabList [i];
            Widget * child = tb->getDest();
            if(child == wnd)
                break;
            tb = 0;
        }

        if (!tb)
            return;

        NIIf ww = getPixelSize().mWidth;
        NIIf x = tb->getPos().x.abs(ww);
        NIIf w = tb->getPixelSize().mWidth;
        NIIf lx = 0, rx = ww;

        Widget * scrollLeftBtn = 0, *scrollRightBtn = 0;
        scrollLeftBtn = get((mID << N_Level_Bit_Escape) + ButtonScrollLeft, false);
        if(scrollLeftBtn)
        {
            lx = scrollLeftBtn->getRelArea().mRight.abs(ww);
            scrollLeftBtn->setCursorMultiClick(false);
        }
        scrollRightBtn = get((mID << N_Level_Bit_Escape) + ButtonScrollRight, false);
        if(scrollRightBtn)
        {
            rx = scrollRightBtn->getPos().x.abs(ww);
            scrollRightBtn->setCursorMultiClick(false);
        }

        if(x < lx)
        {
            mTabOffset += lx - x;
        }
        else
        {
            if(x + w <= rx)
                return; // nothing to do

            mTabOffset += rx - (x + w);
        }

        layout();
    }
    //------------------------------------------------------------------------
    bool TabControl::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdTabHeight());
        dest->add(N_new CmdTabTextPadding());
        dest->add(N_new CmdTabPanePosition());
        return true;
    }
    //------------------------------------------------------------------------
    NCount TabControl::writeImpl(XmlSerializer * dest) const
    {
        NCount count = Container::writeImpl(dest);

        // since TabControl content is actually added to the component tab
        // content pane window, this overridden function exists to dump those
        // out as if they were our own children.
        for(NCount i = 0; i < getTabCount(); ++i)
        {
            getTabContent(i)->write(dest);
            ++count;
        }
        return count;
    }
    //------------------------------------------------------------------------
    void TabControl::addImpl(Widget * widget)
    {
        if(widget->isAutoWindow())
        {
            Container::addImpl(widget);
        }
        else
        {
            addTab(static_cast<Container *>(widget));
        }
    }
    //------------------------------------------------------------------------
    void TabControl::removeImpl(Widget * widget)
    {
        if(widget->isAutoWindow())
        {
            Container::removeImpl(widget);
        }
        else
        {
            removeTab(widget->getID());
        }
    }
    //------------------------------------------------------------------------
    void TabControl::onSelect(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void TabControl::onFont(const FontEventArgs *)
    {
        NCount i, iend = mTabList.size();
        for(i = 0; i < iend; ++i)
        {
            mTabList[i]->setFont(getFont());
        }
    }
    //------------------------------------------------------------------------
    void TabControl::layout()
    {
        Container * tbp = getTabButtonPane();
        Container * tcp = getTabContentPane();

        // Enable top/bottom edges of the tabPane control,
        /*
        if(tcp->getProperty()->isExist(EnableTop))
            tcp->set(EnableTop, (mTabType == Top) ? n0 : n1);
        if(tcp->getProperty()->isExist(EnableBottom))
            tcp->set(EnableBottom, (mTabType == Top) ? n1 : n0);
        if(tbp->getProperty()->isExist(EnableTop))
            tbp->set(EnableTop, (mTabType == Top) ? n0 : n1);
        if(tbp->getProperty()->isExist(EnableBottom))
            tbp->set(EnableBottom, (mTabType == Top) ? n1 : n0);
        */
        Container::layout();

        // Calculate the size & position of the tab scroll buttons
        Widget * scrollLeftBtn = 0, *scrollRightBtn = 0;
        scrollLeftBtn = get((mID << N_Level_Bit_Escape) + ButtonScrollLeft, false);
        scrollRightBtn = get((mID << N_Level_Bit_Escape) + ButtonScrollRight, false);

        // Calculate the positions and sizes of the tab buttons
        if(mTabOffset > 0)
            mTabOffset = 0;

        for(;;)
        {
            NCount i, iend = mTabList.size();
            for (i = 0; i < iend; ++i)
            {
                TabButton * btn = mTabList [i];
                RelVector2f position(RelPosf(0, 0.0f), RelPosf(0, 0.0f));
                RelPlaneSizef size(RelPosf(0, 0.0f), RelPosf(1.0f, 0));

                // x position is based on previous button
                if(!i)
                {
                    // First button
                    position.x = RelPosf(0, mTabOffset);
                }
                else
                {
                    Widget * prevButton = mTabList[i - 1];

                    // position is prev pos + width
                    position.x = prevButton->getRelArea().mRight;
                }

                size.mWidth = RelPosf(0, btn->getRenderText()->getHExtent(btn)) +
                    getTabTextOffset() + getTabTextOffset();

                btn->setPos(position);
                btn->setSize(size);

                const NIIf left_x = position.x.mOffset;
                btn->setVisible ((left_x < getPixelSize ().mWidth) && (left_x + btn->getPixelSize ().mWidth > 0));
                btn->refresh();
            }

            if (mTabList.empty ())
            {
                if (scrollRightBtn)
                    scrollRightBtn->setVisible (false);
                if (scrollLeftBtn)
                    scrollLeftBtn->setVisible (false);
                break;
            }

            // Now check if tab pane wasn't scrolled too far
            --i;
            NIIf xmax = mTabList [i]->getPos().x.mOffset + mTabList [i]->getPixelSize ().mWidth;
            NIIf width = tbp->getPixelSize ().mWidth;

            // If right button margin exceeds right window margin,
            // or leftmost button is at offset 0, finish
            if((xmax > (width - 0.5)) || (mTabOffset == 0))
            {
                if(scrollLeftBtn)
                    scrollLeftBtn->setVisible (mTabOffset < 0);
                if(scrollRightBtn)
                    scrollRightBtn->setVisible (xmax > width);
                break;
            }

            // Scroll the tab pane until the rightmost button
            // touches the right margin
            mTabOffset += width - xmax;
            // If we scrolled it too far, set leftmost button offset to 0
            if (mTabOffset > 0)
                mTabOffset = 0;
        }
    }
    //------------------------------------------------------------------------
    void TabControl::TabTextPrc(const EventArgs * args)
    {
        const WidgetEventArgs * arg = static_cast<const WidgetEventArgs *>(args);
        getTabButtonPane()->get(makeButtonName(arg->mWidget), false)->setText(arg->mWidget->getText());
        
        layout();
        refresh();
    }
    //------------------------------------------------------------------------
    void TabControl::TabButtonClickPrc(const EventArgs * args)
    {
        const WidgetEventArgs * arg = static_cast<const WidgetEventArgs *>(args);
        selectTab(static_cast<TabButton *>(arg->mWidget)->getDest()->getID());
    }
    //------------------------------------------------------------------------
    Container * TabControl::getTabButtonPane() const
    {
        return static_cast<Container *>(get((mID << N_Level_Bit_Escape) + TabButtonPaneName));
    }
    //------------------------------------------------------------------------
    Container * TabControl::getTabContentPane() const
    {
        return static_cast<Container *>(get((mID << N_Level_Bit_Escape) + ContentPaneName));
    }
    //------------------------------------------------------------------------
    bool TabControl::validImpl(const WidgetView * view) const
    {
        return dynamic_cast<const TabControlView *>(view) != 0;
    }
    //------------------------------------------------------------------------
    TabButton * TabControl::createTabButton(WidgetID name) const
    {
        if(mWidgetView != 0)
        {
            return static_cast<TabControlView *>(mWidgetView)->createTabButton(name);
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("This function must be implemented by the window renderer module"));
        }
    }
    //------------------------------------------------------------------------
    void TabControl::TabScrollPrc(const EventArgs * e)
    {
        const WidgetEventArgs * arg = static_cast<const WidgetEventArgs *>(e);

        NCount i;
        NIIf delta = 0;
        // Find the leftmost visible button
        for(i = 0; i < mTabList.size(); ++i)
        {
            if (mTabList [i]->isVisible())
                break;
            delta = mTabList [i]->getPixelSize ().mWidth;
        }

        if(arg->mWidget->getID () == ButtonScrollLeft)
        {
            if(delta == 0.0f && i < mTabList.size ())
                delta = mTabList [i]->getPixelSize ().mWidth;

            // scroll button pane to the right
            mTabOffset += delta;
        }
        else if (i < mTabList.size())
        {
            // scroll button pane to left
            mTabOffset -= mTabList[i]->getPixelSize().mWidth;
        }
        layout();
    }
    //------------------------------------------------------------------------
    void TabControl::TabDragPrc(const EventArgs * e)
    {
        const CursorEventArgs * me = static_cast<const CursorEventArgs *>(e);

        if(me->mMouseArg.mButton == MB_Middle)
        {
            // This is the middle-mouse-click event, remember initial drag position
            Container * but_pane = getTabButtonPane();
            mDragPos = (me->mLocalX - but_pane->getClipArea().mLeft) - mTabOffset;
        }
        else if(me->mMouseArg.mButton == MB_Unknow)
        {
            // Regular mouse move event
            Container * but_pane = getTabButtonPane();
            NIIf new_to = (me->mLocalX - but_pane->getClipArea().mLeft) - mDragPos;
            if((new_to < mTabOffset - 0.9) || (new_to > mTabOffset + 0.9))
            {
                mTabOffset = new_to;
                layout();
            }
        }
    }
    //------------------------------------------------------------------------
    void TabControl::TabWheelPrc(const EventArgs * e)
    {
        const CursorEventArgs * arg = static_cast<const CursorEventArgs *>(e);

        Widget * but_pane = getTabButtonPane();
        NIIf delta = but_pane->getClipArea().getWidth() / 20;

        mTabOffset += arg->mMouseArg.mScroll * delta;
        layout();
    }
    //------------------------------------------------------------------------
    void TabControl::removeTabImpl(Container * wnd)
    {
        // silently abort if wnd to be removed is 0.
        if(!wnd)
            return;

        // delete connection to event we subscribed earlier
        mSignalList.erase(wnd);
        // Was this selected?
        bool reselect = wnd->isFinalVisible();
        // Tab buttons are the 2nd onward children
        getTabContentPane()->remove(wnd);

        // remove button too
        TabButton * tb = static_cast<TabButton *>(getTabButtonPane()->get(makeButtonName(wnd), false));
        tb->unbind(TabButton::ClickEvent, this, (CommandObjFunctor)&TabControl::TabButtonClickPrc);
        tb->unbind(TabButton::DragEvent, this, (CommandObjFunctor)&TabControl::TabDragPrc);
        tb->unbind(TabButton::ScrollEvent, this, (CommandObjFunctor)&TabControl::TabWheelPrc);
        TabList::iterator i, iend = mTabList.end();
        for(i = mTabList.begin(); i != iend; ++i)
        {
            if (*i == tb)
            {
                mTabList.erase (i);
                break;
            }
        }
        getTabButtonPane()->remove(tb);
        N_Only(Widget).destroy(tb);

        if(reselect && (getTabCount() > 0))
            selectTab(getTabContentPane()->get(0)->getID());

        layout();

        refresh();
    }
    //------------------------------------------------------------------------
    Widget * TabControl::getImpl(const String & name_path, bool r) const
    {
        // FIXME: This is horrible
        //
        if (name_path.substr(0, 7) == _T("__auto_"))
            return Container::getImpl(name_path, r);
        else
            return Container::getImpl(_T("__auto_TabPane__") + _T('/') + name_path, r);
    }
    //------------------------------------------------------------------------
    /**
        Child Widgets:
            TabPane based widget with name suffix "__auto_TabPane__"
            optional: DefaultWindow to contain tab buttons with name suffix "__auto_TabPane__Buttons"

        Property initialiser definitions:
            - TabButtonType - specifies a TabButton based widget type to be
              created each time a new tab button is required.
    */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TabControlView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdTabButtonType : public PropertyCmd
    {
    public:
        CmdTabButtonType() : PropertyCmd(N_PropertyUITabControl_ButtonType, _T("TabButtonType"),
            _T("Property to get/set the widget type used when creating tab buttons.  Value should be \"[widgetTypeName]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            TabControlView * obj = static_cast<TabControlView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv((Nui32)obj->getTabType(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            TabControlView * obj = static_cast<TabControlView *>(static_cast<Widget *>(own)->getWidgetView());
            Nui32 temp;
            StrConv::conv(val, temp);
            obj->setTabType(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            TabControlView * obj = static_cast<TabControlView *>(static_cast<const Widget *>(own)->getWidgetView());
            Nui32 * temp = (Nui32 *)out;
            *temp = obj->getTabType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            TabControlView * obj = static_cast<TabControlView *>(static_cast<Widget *>(own)->getWidgetView());
            Nui32 * temp = (Nui32 *)in;
            obj->setTabType(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTabButtonType();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    TabControlView::TabControlView(WidgetViewlID wmid) :
        WidgetView(wmid, N_VIEW_TabControl)
    {
        add(N_new CmdTabButtonType());
    }
    //------------------------------------------------------------------------
    void TabControlView::flush()
    {
        const Style * style = getStyle();

        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);

        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    TabButton * TabControlView::createTabButton(WidgetID wid) const
    {
        if (mTabType == 0)
        {
            N_EXCEPT(InvalidRequest, _I18n("TabType has not been set!"));
        }

        Widget * button = N_Only(Widget).create(mTabType, wid, 0);
        button->setAutoWindow(true);
        return static_cast<TabButton *>(button);
    }
    //------------------------------------------------------------------------
}
}
