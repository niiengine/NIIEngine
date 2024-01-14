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
#include "NiiUIScrollContainer.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWidgetList.h"

#define ViewableArea            1
#define ViewableAreaScroll      2
#define ViewableAreaHScroll     3
#define ViewableAreaVScroll     4
#define ViewableAreaHVScroll    5

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdForceVertScrollbar : public PropertyCmd
    {
    public:
        CmdForceVertScrollbar() : PropertyCmd(N_PropertyUIForceVScrollbar_ForceVScrollbar, _T("force_vert_scrollbar"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->isForceVScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setForceVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ScrollContainer *>(own)->isForceVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ScrollContainer *>(own)->setForceVScrollbar(*temp);
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
    //------------------------------------------------------------------------
    class _EngineInner CmdForceHorzScrollbar : public PropertyCmd
    {
    public:
        CmdForceHorzScrollbar() : PropertyCmd(N_PropertyUIForceVScrollbar_ForceHScrollbar, _T("force_horz_scrollbar"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->isForceHScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setForceHScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ScrollContainer *>(own)->isForceHScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ScrollContainer *>(own)->setForceHScrollbar(*temp);
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
    //------------------------------------------------------------------------
    class _EngineInner CmdHorzStepSize : public PropertyCmd
    {
    public:
        CmdHorzStepSize() : PropertyCmd(N_PropertyUIForceVScrollbar_HStepSize, _T("horz_step_size"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->getHStepSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setHStepSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ScrollContainer *>(own)->getHStepSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ScrollContainer *>(own)->setHStepSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzStepSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.1f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdHorzOverlapSize : public PropertyCmd
    {
    public:
        CmdHorzOverlapSize() : PropertyCmd(N_PropertyUIForceVScrollbar_HPageRemain, _T("horz_over_lap_size"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->getHPageRemain(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setHPageRemain(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ScrollContainer *>(own)->getHPageRemain();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ScrollContainer *>(own)->setHPageRemain(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzOverlapSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.01f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdHorzScrollPosition : public PropertyCmd
    {
    public:
        CmdHorzScrollPosition() : PropertyCmd(N_PropertyUIForceVScrollbar_HScrollPos, _T("horz_scroll_position"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->getHScrollPos(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setHScrollPos(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ScrollContainer *>(own)->getHScrollPos();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ScrollContainer *>(own)->setHScrollPos(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzScrollPosition();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdVertStepSize : public PropertyCmd
    {
    public:
        CmdVertStepSize() : PropertyCmd(N_PropertyUIForceVScrollbar_VStepSize, _T("VertStepSize"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->getVStepSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setVStepSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ScrollContainer *>(own)->getVStepSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ScrollContainer *>(own)->setVStepSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertStepSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.1f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdVertOverlapSize : public PropertyCmd
    {
    public:
        CmdVertOverlapSize() : PropertyCmd(N_PropertyUIForceVScrollbar_VPageRemain, _T("vert_overlap_size"),
            _T("Property to get/set the overlap size for the vertical Scrollbar.  Value is a NIIf."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->getVPageRemain(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setVPageRemain(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ScrollContainer *>(own)->getVPageRemain();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ScrollContainer *>(own)->setVPageRemain(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertOverlapSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.01f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdVertScrollPosition : public PropertyCmd
    {
    public:
        CmdVertScrollPosition() : PropertyCmd(N_PropertyUIForceVScrollbar_VScrollPos, _T("vert_scroll_position"),
            _T("Property to get/set the scroll position of the vertical Scrollbar as a fraction.  Value is a NIIf."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->getVScrollPos(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setVScrollPos(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ScrollContainer *>(own)->getVScrollPos();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ScrollContainer *>(own)->setVScrollPos(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertScrollPosition();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdContentPaneAutoSized : public PropertyCmd
    {
    public:
        CmdContentPaneAutoSized() : PropertyCmd(N_PropertyUIForceVScrollbar_ContentAutoSize, _T("content_pane_auto_sized"),
            _T("Property to get/set the setting which controls whether the content pane will auto-size itself.  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrollContainer *>(own)->isContentAutoSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ScrollContainer *>(own)->setContentAutoSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ScrollContainer *>(own)->isContentAutoSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ScrollContainer *>(own)->setContentAutoSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdContentPaneAutoSized();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdContentArea : public PropertyCmd
    {
    public:
        CmdContentArea() : PropertyCmd(N_PropertyUIForceVScrollbar_ContentArea, _T("content_area"),
            _T("Property to get/set the current content area rectangle of the content pane.  Value is \"l:[NIIf] t:[NIIf] r:[NIIf] b:[NIIf]\" (where l is left, t is top, r is right, and b is bottom)."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const Rectf & val = static_cast<const ScrollContainer *>(own)->getContentArea();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"),
                val.mLeft, val.mTop, val.mRight, val.mBottom);

            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Rectf temp(0, 0, 0, 0);
            Nsscanf(val.c_str(), _T("%g %g %g %g"),
                &temp.mLeft, &temp.mTop, &temp.mRight, &temp.mBottom);
            static_cast<ScrollContainer *>(own)->setContentArea(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Rectf * temp = (Rectf *)out;
            *temp = static_cast<const ScrollContainer *>(own)->getContentArea();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Rectf * temp = (Rectf *)in;
            static_cast<ScrollContainer *>(own)->setContentArea(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdContentArea();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, ScrollContainer, ContentAreaChangeEvent,         0);
    N_EVENT(Container, ScrollContainer, VScrollbarSetEvent,             1);
    N_EVENT(Container, ScrollContainer, HScrollbarSetEvent,             2);
    N_EVENT(Container, ScrollContainer, AutoSizeChangeEvent,            3);
    N_EVENT(Container, ScrollContainer, ContentAreaScrollEvent,         4);
    N_EVENT(Container, ScrollContainer, EventCount,                     5);
    //------------------------------------------------------------------------
    ScrollContainer::ScrollContainer(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_ScrollContainer, lid),
        mForceVScroll(false),
        mForceHScroll(false),
        mContentRect(0, 0, 0, 0),
        mVScrollStep(0.1f),
        mVScrollRemain(0.01f),
        mHScrollStep(0.1f),
        mHScrollRemain(0.01f),
        mHScrollbar(0),
        mVScrollbar(0),
        mContentContainer(0),
        mContentContainerView(0),
        mHScrollbarView(0),
        mVScrollbarView(0)
    {
        notifySignal(ScrollContainer::EventCount);
        setupCmd(N_CmdObj_ScrollContainer);
    }
    //------------------------------------------------------------------------
    ScrollContainer::~ScrollContainer()
    {
        if(mHScrollbar != 0)
        {
            N_Only(Widget).destroy(mHScrollbar);
            mHScrollbar = 0;
        }
        if(mVScrollbar != 0)
        {
            N_Only(Widget).destroy(mVScrollbar);
            mVScrollbar = 0;
        }
        if(mContentContainer != 0)
        {
            N_Only(Widget).destroy(mContentContainer);
            mContentContainer = 0;
        }
    }
    //------------------------------------------------------------------------
    void ScrollContainer::setForceVScrollbar(bool b)
    {
        if(mForceVScroll != b)
        {
            mForceVScroll = b;

            updateScrollbar();
            WidgetEventArgs args(this);
            onVScrollbarSetChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void ScrollContainer::setForceHScrollbar(bool b)
    {
        if(mForceHScroll != b)
        {
            mForceHScroll = b;

            updateScrollbar();
            WidgetEventArgs args(this);
            onHScrollbarSetChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void ScrollContainer::setHStepSize(NIIf step)
    {
        mHScrollStep = step;
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void ScrollContainer::setHPageRemain(NIIf f)
    {
        mHScrollRemain = f;
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void ScrollContainer::setVStepSize(NIIf step)
    {
        mVScrollStep = step;
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void ScrollContainer::setVPageRemain(NIIf overlap)
    {
        mVScrollRemain = overlap;
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void ScrollContainer::initChild()
    {
        if(mContentContainer == 0)
            mContentContainer = static_cast<SizeContainer *>(N_Only(Widget).create(mContentContainerView, 0, this, N_WIDGET_ScrollContainer));

        if(mHScrollbar == 0)
            mHScrollbar = static_cast<Scrollbar *>(N_Only(Widget).create(mHScrollbarView, 0, this, N_WIDGET_Scrollbar));

        if(mVScrollbar == 0)
            mVScrollbar = static_cast<Scrollbar *>(N_Only(Widget).create(mVScrollbarView, 0, this, N_WIDGET_Scrollbar));

        // do a bit of initialisation
        mHScrollbar->setTop(true);
        mVScrollbar->setTop(true);
        mContentContainer->setAutoWindow(true);
        // container pane is always same size as this parent pane,
        // scrolling is actually implemented via positioning and clipping tricks.
        mContentContainer->setSize(RelPlaneSizef(RelPosf(1.0f, 0), RelPosf(1.0f, 0)));

        // subscribe to events we need to hear about
        mVScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&ScrollContainer::ContentAreaScrollMFunc);
        mHScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&ScrollContainer::ContentAreaScrollMFunc);
        mContentChangeSingal = mContentContainer->bind(SizeContainer::ContentAreaChangeEvent,
            this, (CommandObjFunctor)&ScrollContainer::ContentAreaMFunc);
        mAutoSizeChangeSingal = mContentContainer->bind(SizeContainer::AutoSizeChangeEvent,
            this, (CommandObjFunctor)&ScrollContainer::AutoSizeChangeMFunc);

        // finalise setup
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void ScrollContainer::updateScrollbar()
    {
        // controls should all be valid by this stage
        bool horzScrollBarWasVisible = mHScrollbar->isVisible();
        bool vertScrollBarWasVisible = mVScrollbar->isVisible();

        // enable required scrollbars
        mVScrollbar->setVisible(((fabs(mContentRect.getHeight()) > static_cast<ScrollContainerView *>(mWidgetView)->getViewableArea().getHeight()) ||
            mForceVScroll));
        mHScrollbar->setVisible(((fabs(mContentRect.getWidth()) > static_cast<ScrollContainerView *>(mWidgetView)->getViewableArea().getWidth()) ||
            mForceHScroll));

        // Check if the addition of the horizontal scrollbar means we
        // now also need the vertical bar.
        if(mHScrollbar->isVisible())
            mVScrollbar->setVisible(((fabs(mContentRect.getHeight()) > static_cast<ScrollContainerView *>(mWidgetView)->getViewableArea().getHeight()) ||
            mForceVScroll));

        if(horzScrollBarWasVisible != mHScrollbar->isVisible() ||
            vertScrollBarWasVisible != mVScrollbar->isVisible())
        {
            WidgetEventArgs args(this);
            onSize(&args);
        }

        layout();

        // get viewable area
        const Rectf viewableArea(static_cast<ScrollContainerView *>(mWidgetView)->getViewableArea());

        // set up vertical scroll bar values
        mVScrollbar->setDocSize(fabsf(mContentRect.getHeight()));
        mVScrollbar->setPageSize(viewableArea.getHeight());
        mVScrollbar->setStepSize(N_MAX(NIIf, 1.0f, viewableArea.getHeight() * mVScrollStep));
        mVScrollbar->setOverlapSize(N_MAX(NIIf, 1.0f, viewableArea.getHeight() * mVScrollRemain));
        mVScrollbar->setCurrent(mVScrollbar->getCurrent());

        // set up horizontal scroll bar values
        mHScrollbar->setDocSize(fabsf(mContentRect.getWidth()));
        mHScrollbar->setPageSize(viewableArea.getWidth());
        mHScrollbar->setStepSize(N_MAX(NIIf, 1.0f, viewableArea.getWidth() * mHScrollStep));
        mHScrollbar->setOverlapSize(N_MAX(NIIf, 1.0f, viewableArea.getWidth() * mHScrollRemain));
        mHScrollbar->setCurrent(mHScrollbar->getCurrent());
    }
    //------------------------------------------------------------------------
    void ScrollContainer::updateContentArea()
    {
        // basePos is the position represented by the scrollbars
        // (these are negated so pane is scrolled in the correct directions)
        RelVector2f basePos(RelPosf(0, -mHScrollbar->getCurrent()),
            RelPosf(0, -mVScrollbar->getCurrent()));

        // this bias is the absolute position that 0 on the scrollbars represent.
        // Allows the pane to function correctly with negatively positioned content.
        RelVector2f bias(RelPosf(0, mContentRect.mLeft), RelPosf(0, mContentRect.mTop));

        // set the new container pane position to be what the scrollbars request
        // minus any bias generated by the location of the content.
        mContentContainer->setPos(basePos - bias);
    }
    //------------------------------------------------------------------------
    bool ScrollContainer::validImpl(const WidgetView * view) const
    {
        return dynamic_cast<const ScrollContainerView *>(view) != 0;
    }
    //------------------------------------------------------------------------
    void ScrollContainer::ContentAreaScrollMFunc(const EventArgs * arg)
    {
        onContentAreaScroll(static_cast<const WidgetEventArgs *>(arg));
    }
    //------------------------------------------------------------------------
    void ScrollContainer::AutoSizeChangeMFunc(const EventArgs * arg)
    {
        onAutoSizeChange(static_cast<const WidgetEventArgs *>(arg));
    }
    //------------------------------------------------------------------------
    void ScrollContainer::onContentAreaChange(const WidgetEventArgs * arg)
    {
        signal(ContentAreaChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ScrollContainer::onVScrollbarSetChange(const WidgetEventArgs * arg)
    {
        signal(VScrollbarSetEvent, arg);
    }
    //------------------------------------------------------------------------
    void ScrollContainer::onHScrollbarSetChange(const WidgetEventArgs * arg)
    {
        signal(HScrollbarSetEvent, arg);
    }
    //------------------------------------------------------------------------
    void ScrollContainer::onAutoSizeChange(const WidgetEventArgs * arg)
    {
        signal(AutoSizeChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ScrollContainer::onContentAreaScroll(const WidgetEventArgs * arg)
    {
        updateContentArea();
        signal(ContentAreaScrollEvent, arg);
    }
    //------------------------------------------------------------------------
    void ScrollContainer::ContentAreaMFunc(const EventArgs *)
    {
        // get updated extents of the content
        const Rectf contentArea(mContentContainer->getContentArea());

        // calculate any change on the top and left edges.
        const NIIf xChange = contentArea.mLeft - mContentRect.mLeft;
        const NIIf yChange = contentArea.mTop - mContentRect.mTop;

        // store new content extents information
        mContentRect = contentArea;

        updateScrollbar();

        // update scrollbar positions (which causes container pane to be moved as needed).
        mHScrollbar->setCurrent(mHScrollbar->getCurrent() - xChange);
        mVScrollbar->setCurrent(mVScrollbar->getCurrent() - yChange);

        // this call may already have been made if the scroll positions changed.  The call
        // is required here for cases where the top/left 'bias' has changed; in which
        // case the scroll position notification may or may not have been fired.
        if(xChange || yChange)
            updateContentArea();

        // fire event
        WidgetEventArgs args(this);
        onContentAreaChange(&args);
    }
    //------------------------------------------------------------------------
    void ScrollContainer::addImpl(Widget * sub)
    {
        if(sub->isAutoWindow())
        {
            // This is an internal widget, so should be added normally.
            Container::addImpl(sub);
        }
        // this is a client window/widget, so should be added to the pane container.
        else
        {
            // container should always be valid by the time we're adding client
            // controls
            mContentContainer->add(sub);
        }
    }
    //------------------------------------------------------------------------
    void ScrollContainer::removeImpl(Widget * sub)
    {
        if (sub->isAutoWindow())
        {
            // This is an internal widget, so should be removed normally.
            Container::removeImpl(sub);
        }
        // this is a client window/widget, so should be removed from the pane
        // container.
        else
        {
            // container should always be valid by the time we're handling client
            // controls
            mContentContainer->remove(sub);
        }
    }
    //------------------------------------------------------------------------
    void ScrollContainer::onSize(const WidgetEventArgs * arg)
    {
        updateScrollbar();
        updateContentArea();
        Container::onSize(arg);
    }
    //------------------------------------------------------------------------
    void ScrollContainer::onCursorDrag(const CursorEventArgs * arg)
    {
        // base class processing.
        Widget::onCursorDrag(arg);

        if(mVScrollbar->isFinalVisible() &&
            (mVScrollbar->getDocSize() > mVScrollbar->getPageSize()))
        {
            mVScrollbar->setCurrent(mVScrollbar->getCurrent() +
                mVScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
        else if(mHScrollbar->isFinalVisible() &&
            (mHScrollbar->getDocSize() > mHScrollbar->getPageSize()))
        {
            mHScrollbar->setCurrent(mHScrollbar->getCurrent() +
                mHScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
    }
    //------------------------------------------------------------------------
    bool ScrollContainer::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdForceVertScrollbar());
        dest->add(N_new CmdForceHorzScrollbar());
        dest->add(N_new CmdHorzStepSize());
        dest->add(N_new CmdHorzOverlapSize());
        dest->add(N_new CmdHorzScrollPosition());
        dest->add(N_new CmdVertStepSize());
        dest->add(N_new CmdVertOverlapSize());
        dest->add(N_new CmdVertScrollPosition());
        dest->add(N_new CmdContentPaneAutoSized());
        dest->add(N_new CmdContentArea());
        return true;
    }
    //------------------------------------------------------------------------
    void ScrollContainer::destroy()
    {
        // detach from events on content pane
        mContentChangeSingal->disconnect();
        mAutoSizeChangeSingal->disconnect();

        // now do the cleanup
        Widget::destroy();
    }
    //------------------------------------------------------------------------
    Widget * ScrollContainer::getImpl(const String & name_path, bool r) const
    {
        // FIXME: This is horrible
        //
        if (name_path.substr(0, 7) == _T("__auto_"))
            return Container::getImpl(name_path, r);
        else
            return mContentContainer->getImpl(name_path, r);
    }
    //------------------------------------------------------------------------
    /*Named Areas:
            - ViewableArea         - area where content will be drawn when no scrollbars are visible.
            - ViewableAreaHScroll  - area where content will be drawn when the horizontal scrollbar is visible.
            - ViewableAreaVScroll  - area where content will be drawn when the vertical scrollbar is visible.
            - ViewableAreaHVScroll - area where content will be drawn when both scrollbars are visible.
    Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"*/
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ScrollContainerView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ScrollContainerView::ScrollContainerView(WidgetViewlID wsid) :
        WidgetView(wsid, N_VIEW_ScrollContainer),
        mStyleValid(false)
    {
    }
    //------------------------------------------------------------------------
    Rectf ScrollContainerView::getViewableArea() const
    {
        ScrollContainer * w = (ScrollContainer *)mSrc;
        const Style * style = getStyle();
        bool v_visible = w->getVScrollbar()->isVisible();
        bool h_visible = w->getHScrollbar()->isVisible();
        
        Nid sid;
        // if either of the scrollbars are visible, we might want to use another text rendering area
        if (v_visible || h_visible)
        {
            if (h_visible)
            {
                if (v_visible)
                {
                    sid = ViewableAreaHVScroll;
                }
                else
                {
                    sid = ViewableAreaHScroll;
                }
            }
            else
            {
                if (v_visible)
                {
                    sid = ViewableAreaVScroll;
                }
                else
                {
                    sid = ViewableAreaScroll;
                }
            }

            if (style->isSpecialExist(sid))
            {
                return style->getSpecial(sid).getArea().getPixelRect(*w);
            }
        }
        return style->getSpecial(ViewableArea).getArea().getPixelRect(*w);
    }
    //------------------------------------------------------------------------
    void ScrollContainerView::flush()
    {
        const Style * style = getStyle();

        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);

        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    void ScrollContainerView::onStyleAttach()
    {
        mStyleValid = true;
    }
    //------------------------------------------------------------------------
    void ScrollContainerView::onStyleDetach()
    {
        mStyleValid = false;
    }
    //------------------------------------------------------------------------
    Rectf ScrollContainerView::getArea() const
    {
        if (!mStyleValid)
            return mSrc->getArea().getIntCache();

        Rectf lr(getViewableArea());
        lr += mSrc->getArea().getIntCache().min();
        return lr;
    }
    //------------------------------------------------------------------------
}
}