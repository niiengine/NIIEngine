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
#include "NiiUIScrolledListContainer.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIScrollbar.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdForceVertScrollbar : public PropertyCmd
    {
    public:
        CmdForceVertScrollbar() : PropertyCmd(N_PropertyUIScrolledListContainer_VScrollAlwaysShow, _T("force_vert_scrollbar"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrolledListContainer *>(own)->isForceVScrollbar(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ScrolledListContainer *>(own)->SetForceVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ScrolledListContainer *>(own)->isForceVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ScrolledListContainer *>(own)->SetForceVScrollbar(*temp);
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
        CmdForceHorzScrollbar() : PropertyCmd(N_PropertyUIScrolledListContainer_HScrollAlwaysShow, _T("force_horz_scrollbar"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ScrolledListContainer *>(own)->isForceHScrollbar(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ScrolledListContainer *>(own)->SetForceHScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ScrolledListContainer *>(own)->isForceHScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ScrolledListContainer *>(own)->SetForceHScrollbar(*temp);
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
    N_EVENT(ListContainer, ScrolledListContainer, VScrollModeEvent, 0);
    N_EVENT(ListContainer, ScrolledListContainer, HScrollModeEvent, 1);
    N_EVENT(ListContainer, ScrolledListContainer, EventCount,                    2);
    // component widget name suffixes
    const WidgetID ScrolledListContainer::VScrollbarID(1); //"__auto_vscrollbar__"
    const WidgetID ScrolledListContainer::HScrollbarID(2); //"__auto_hscrollbar__"
    //------------------------------------------------------------------------
    ScrolledListContainer::ScrolledListContainer(WidgetID wid, FactoryID fid,
        Container * own, const String & name, ScriptTypeID stid, LangID lid) :
        ListContainer(wid, fid, own, name, stid, lid),
        mContainerAreaView(0),
        mAlwayVScroll(false),
        mAlwayHScroll(false)
    {
        notifySignal(ScrolledListContainer::EventCount);

        setupCmd(N_CmdObj_ScrolledListContainer);
    }
    //------------------------------------------------------------------------
    ScrolledListContainer::~ScrolledListContainer()
    {
        if(mContainerArea && mContainerArea != this)
            N_OnlyPtr(Widget)->destroy(mContainerArea);
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::initChild()
    {
        // base class handling
        ListContainer::initChild();

        if(mContainerArea == this)
            mContainerArea = static_cast<Container *>(N_OnlyPtr(Widget)->create(mContainerAreaView, 0, this, N_WIDGET_Container));
        mContainerArea->setAutoWindow(true);
        mContainerArea->setCursorSubmitEvent(true);

        Scrollbar * v = getVScrollbar();
        Scrollbar * h = getHScrollbar();

        v->setTop(true);
        h->setTop(true);

        v->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&ScrolledListContainer::prcVScrollCB);

        h->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&ScrolledListContainer::prcHScrollCB);

        v->hide();
        h->hide();
    }
    //------------------------------------------------------------------------
    Scrollbar * ScrolledListContainer::getVScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + VScrollbarID, false));
    }
    //------------------------------------------------------------------------
    Scrollbar * ScrolledListContainer::getHScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + HScrollbarID, false));
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::updateScrollbar(const PlaneSizef & doc_size)
    {
        Scrollbar * v = getVScrollbar();
        Scrollbar * h = getHScrollbar();

        bool old_vert_visible = v->isVisible();
        bool old_horz_visible = h->isVisible();

        PlaneSizef render_area_size = getItemArea().getSize();

        // setup the pane size
        NIIf pane_size_w = N_MAX(NIIf, doc_size.mWidth, render_area_size.mWidth);
        RelPlaneSizef pane_size(RelPosf(0, pane_size_w), RelPosf(0, doc_size.mHeight));

        mContainerArea->setMinSize(pane_size);
        mContainerArea->setMaxSize(pane_size);

        // "fix" scrollbar visibility
        if (mAlwayVScroll || doc_size.mHeight > render_area_size.mHeight)
        {
            v->show();
        }
        else
        {
            v->hide();
        }

        if (mAlwayHScroll || doc_size.mWidth > render_area_size.mWidth)
        {
            h->show();
        }
        else
        {
            h->hide();
        }

        // if some change occurred, invalidate the inner rect area caches.
        if((old_vert_visible != v->isVisible()) || (old_horz_visible != h->isVisible()))
        {
            mInnerRect.resetCached();
            mValidClipInnerRect = false;
        }

        // get a fresh item render area
        Rectf render_area = getItemArea();
        render_area_size = render_area.getSize();

        // update the pane clipper area
        mContainerArea->setSize(RelPlaneSizef(RelPosf(0, render_area.width()), RelPosf(0, render_area.getHeight())));

        // setup vertical scrollbar
        v->setDocSize(doc_size.mHeight);
        v->setPageSize(render_area_size.mHeight);
        v->setStepSize(N_MAX(NIIf, 1.0f, render_area_size.mHeight / 10.0f));
        v->setCurrent(v->getCurrent());

        // setup horizontal scrollbar
        h->setDocSize(doc_size.mWidth);
        h->setPageSize(render_area_size.mWidth);
        h->setStepSize(N_MAX(NIIf, 1.0f, render_area_size.mWidth / 10.0f));
        h->setCurrent(h->getCurrent());
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::onCursorDrag(const CursorEventArgs * arg)
    {
        ListContainer::onCursorDrag(arg);

        NCount count = getItemCount();
        Scrollbar * v = getVScrollbar();

        // dont do anything if we are no using scrollbars
        // or have'nt got any items
        if (!v->isVisible() || !count)
        {
            return;
        }

        NIIf pixH = mContainerArea->getArea().getIntCache().height();
        NIIf delta = (pixH/NIIf(count)) * -arg->mMouseArg.mScroll;
        v->setCurrent(v->getCurrent() + delta);
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::prcVScrollCB(const EventArgs * e)
    {
        const WidgetEventArgs * we = static_cast<const WidgetEventArgs *>(e);
        Scrollbar * v = static_cast<Scrollbar *>(we->mWidget);
        NIIf newpos = -v->getCurrent();
        mContainerArea->setPos(RelVector2f(mContainerArea->getPos().x, RelPosf(0, newpos)));
    }
    //----------------------------------------------------------------------
    void ScrolledListContainer::prcHScrollCB(const EventArgs * e)
    {
        const WidgetEventArgs * we = static_cast<const WidgetEventArgs *>(e);
        Scrollbar * h = static_cast<Scrollbar *>(we->mWidget);
        NIIf newpos = -h->getCurrent();
        mContainerArea->setPos(RelVector2f(RelPosf(0, newpos), mContainerArea->getPos().y));
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::SetForceVScrollbar(bool mode)
    {
        if (mode != mAlwayVScroll)
        {
            mAlwayVScroll = mode;
            WidgetEventArgs e(this);
            onVScrollbarMode(&e);
        }
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::SetForceHScrollbar(bool mode)
    {
        if(mode != mAlwayHScroll)
        {
            mAlwayHScroll = mode;
            WidgetEventArgs e(this);
            onHScrollbarMode(&e);
        }
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::onVScrollbarMode(const WidgetEventArgs * arg)
    {
        signal(VScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ScrolledListContainer::onHScrollbarMode(const WidgetEventArgs * arg)
    {
        signal(HScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    bool ScrolledListContainer::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdForceVertScrollbar());
        dest->add(N_new CmdForceHorzScrollbar());
        return true;
    }
    //-----------------------------------------------------------------------
    void ScrolledListContainer::showVert(const ListItem & item)
    {
        Scrollbar * v = getVScrollbar();
        const Rectf & render_area = getItemArea();
        const NIIf currPos = v->getCurrent();

        const NIIf top = item.getPos().y.abs(getPixelSize().mHeight) - currPos;
        const NIIf bottom = top + item.getItemPixelSize().mHeight;

        // if top is above the view area, or if item is too big, scroll item to top
        if((top < render_area.mTop) || ((bottom - top) > render_area.getHeight()))
            v->setCurrent(currPos + top);
        // if bottom is below the view area, scroll item to bottom of list
        else if(bottom >= render_area.mBottom)
            v->setCurrent(currPos + bottom - render_area.getHeight());
    }
    //-----------------------------------------------------------------------
    void ScrolledListContainer::showHorz(const ListItem & item)
    {
        const Rectf & render_area = getItemArea();
        Scrollbar * const h = getHScrollbar();
        const NIIf currPos = h->getCurrent();

        const NIIf left = item.getPos().x.abs(getPixelSize().mWidth) - currPos;
        const NIIf right = left + item.getItemPixelSize().mWidth;

        // if left is left of the view area, or if item too big, scroll item to left
        if((left < render_area.mLeft) || ((right - left) > render_area.getWidth()))
            h->setCurrent(currPos + left);
        // if right is right of the view area, scroll item to right of list
        else if(right >= render_area.mRight)
            h->setCurrent(currPos + right - render_area.getWidth());
    }
    //-----------------------------------------------------------------------
}
}