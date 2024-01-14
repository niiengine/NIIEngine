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
#include "NiiUIScrollbar.h"
#include "NiiUIButton.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIException.h"

#define ThumbTrackArea 1

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdDocumentSize : public PropertyCmd
    {
    public:
        CmdDocumentSize() : PropertyCmd(N_PropertyUIScrollbar_DocSize, _T("document_size"),
            _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Scrollbar *>(own)->getDocSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Scrollbar *>(own)->setDocSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Scrollbar *>(own)->getDocSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Scrollbar *>(own)->setDocSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDocumentSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)1.0f, out);
        }
    };

    class _EngineInner CmdPos : public PropertyCmd
    {
    public:
        CmdPos() : PropertyCmd(N_PropertyUIScrollbar_PageSize, _T("page_size"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Scrollbar *>(own)->getPageSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Scrollbar *>(own)->setPageSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Scrollbar *>(own)->getPageSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp= (NIIf *)in;
            static_cast<Scrollbar *>(own)->setPageSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPos();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.0f, out);
        }
    };

    class _EngineInner CmdStepSize : public PropertyCmd
    {
    public:
        CmdStepSize() : PropertyCmd(N_PropertyUIScrollbar_StepSize, _T("step_size"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Scrollbar *>(own)->getStepSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Scrollbar *>(own)->setStepSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Scrollbar *>(own)->getStepSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Scrollbar *>(own)->setStepSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdStepSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)1.0f, out);
        }
    };

    class _EngineInner CmdOverlapSize : public PropertyCmd
    {
    public:
        CmdOverlapSize() : PropertyCmd(N_PropertyUIScrollbar_OverlapSize, _T("overlap_size"),
            _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Scrollbar *>(own)->getOverlapSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Scrollbar *>(own)->setOverlapSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Scrollbar *>(own)->getOverlapSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Scrollbar *>(own)->setOverlapSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdOverlapSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.0f, out);
        }
    };

    class _EngineInner CmdScrollPosition : public PropertyCmd
    {
    public:
        CmdScrollPosition() : PropertyCmd(N_PropertyUIScrollbar_Current,
            _T("scroll_position"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Scrollbar *>(own)->getCurrent(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Scrollbar *>(own)->setCurrent(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Scrollbar *>(own)->getCurrent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Scrollbar *>(own)->setCurrent(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdScrollPosition();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.0f, out);
        }
    };

    class _EngineInner CmdUnitIntervalScrollPosition : public PropertyCmd
    {
    public:
        CmdUnitIntervalScrollPosition() : PropertyCmd(N_PropertyUIScrollbar_CurrentUnit, _T("unit_interval_scroll_position"),
            _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Scrollbar *>(own)->getCurrentUnit(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Scrollbar *>(own)->setCurrentUnit(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Scrollbar *>(own)->getCurrentUnit();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Scrollbar *>(own)->setCurrentUnit(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdUnitIntervalScrollPosition();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.0f, out);
        }
    };

    class _EngineInner CmdEndLockEnabled : public PropertyCmd
    {
    public:
        CmdEndLockEnabled() : PropertyCmd(N_PropertyUIScrollbar_AutoEndPos, _T("end_lock_enabled"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Scrollbar *>(own)->isAutoEndPos(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Scrollbar *>(own)->setAutoEndPos(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Scrollbar *>(own)->isAutoEndPos();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Scrollbar *>(own)->setAutoEndPos(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdEndLockEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, Scrollbar,        CurrentChageEvent,     0);
    N_EVENT(Container, Scrollbar,        ThumbMoveBeginEvent,   1);
    N_EVENT(Container, Scrollbar,        ThumbMoveEndEvent,     2);
    N_EVENT(Container, Scrollbar,        ConfigChangeEvent,     3);
    N_EVENT(Container, Scrollbar,        EventCount,            4);
    //------------------------------------------------------------------------
    Scrollbar::Scrollbar(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_Scrollbar, lid),
        mDocSize(1.0f),
        mPageSize(0.0f),
        mStepSize(1.0f),
        mPageRemian(0.0f),
        mCurrentPos(0.0f),
        mThumb(0),
        mNegative(0),
        mPositive(0),
        mThumbView(0),
        mNegativeView(0),
        mPositiveView(0),
        mAutoEnd(false)
    {
        notifySignal(Scrollbar::EventCount);
        setupCmd(N_CmdObj_Scrollbar);
    }
    //------------------------------------------------------------------------
    Scrollbar::~Scrollbar()
    {
        if(mThumb == 0)
        {
            N_Only(Widget).destroy(mThumb);
            mThumb = 0;
        }
        if(mNegative == 0)
        {
            N_Only(Widget).destroy(mNegative);
            mNegative = 0;
        }
        if(mPositive == 0)
        {
            N_Only(Widget).destroy(mPositive);
            mPositive = 0;
        }
    }
    //------------------------------------------------------------------------
    void Scrollbar::initChild()
    {
        if(mThumb == 0)
            mThumb = static_cast<PosButton *>(N_Only(Widget).create(mThumbView, 0, this, N_WIDGET_PosButton));

        if(mNegative == 0)
            mNegative = static_cast<PushButton *>(N_Only(Widget).create(mNegativeView, 0, this, N_WIDGET_PushButton));

        if(mPositive == 0)
            mPositive = static_cast<PushButton *>(N_Only(Widget).create(mPositiveView, 0, this, N_WIDGET_PushButton));

        mThumb->bind(PosButton::PosChangeEvent, this, (CommandObjFunctor)&Scrollbar::ThumbMoveMFunc);
        mThumb->bind(PosButton::MoveBeginEvent, this, (CommandObjFunctor)&Scrollbar::ThumbBeginMFunc);
        mThumb->bind(PosButton::MoveEndEvent, this, (CommandObjFunctor)&Scrollbar::ThumbEndMFunc);
        mPositive->bind(PushButton::ButtonDownEvent, this, (CommandObjFunctor)&Scrollbar::PositiveMFunc);
        mNegative->bind(PushButton::ButtonDownEvent, this, (CommandObjFunctor)&Scrollbar::NegativeMFunc);
        // do initial layout
        layout();
    }
    //------------------------------------------------------------------------
    void Scrollbar::setDocSize(NIIf size)
    {
        if(mDocSize != size)
        {
            bool autoend = mAutoEnd && isCurrentEnd();

            mDocSize = size;

            if (autoend)
                setCurrent(getCurrentMax());
            else
                static_cast<ScrollbarView *>(mWidgetView)->updateThumb();

            WidgetEventArgs args(this);
            onConfigChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void Scrollbar::setPageSize(NIIf size)
    {
        if(mPageSize != size)
        {
            const bool autoend = mAutoEnd && isCurrentEnd();

            mPageSize = size;

            if (autoend)
                setCurrent(getCurrentMax());
            else
                static_cast<ScrollbarView *>(mWidgetView)->updateThumb();

            WidgetEventArgs args(this);
            onConfigChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void Scrollbar::setStepSize(NIIf size)
    {
        if(mStepSize != size)
        {
            mStepSize = size;

            WidgetEventArgs args(this);
            onConfigChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void Scrollbar::setOverlapSize(NIIf size)
    {
        if(mPageRemian != size)
        {
            mPageRemian = size;

            WidgetEventArgs args(this);
            onConfigChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void Scrollbar::setCurrent(NIIf position)
    {
        bool modified = setCurrentImpl(position);
        static_cast<ScrollbarView *>(mWidgetView)->updateThumb();

        // notification if required
        if (modified)
        {
            WidgetEventArgs args(this);
            onCurrentChange(&args);
        }
    }
    //------------------------------------------------------------------------
    bool Scrollbar::validImpl(const WidgetView * view) const
    {
        return dynamic_cast<const ScrollbarView *>(view) != 0;
    }
    //------------------------------------------------------------------------
    void Scrollbar::onCurrentChange(const WidgetEventArgs * arg)
    {
        signal(CurrentChageEvent, arg);
    }
    //------------------------------------------------------------------------
    void Scrollbar::onTumbMoveBegin(const WidgetEventArgs * arg)
    {
        signal(ThumbMoveBeginEvent, arg);
    }
    //------------------------------------------------------------------------
    void Scrollbar::onThumbMoveEnd(const WidgetEventArgs * arg)
    {
        signal(ThumbMoveEndEvent, arg);
    }
    //------------------------------------------------------------------------
    void Scrollbar::onConfigChange(const WidgetEventArgs * arg)
    {
        layout();
        signal(ConfigChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Scrollbar::onButtonDown(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton != MB_Left)
            return;

        NIIf adj = static_cast<ScrollbarView *>(mWidgetView)->getDirection(Vector2f(arg->mLocalX, arg->mLocalY));

        if(adj > 0)
            posPage();
        else if(adj < 0)
            negPage();
    }
    //------------------------------------------------------------------------
    void Scrollbar::onCursorDrag(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onCursorDrag(arg);

        // scroll by e.mScroll * stepSize
        setCurrent(mCurrentPos + mStepSize * -arg->mMouseArg.mScroll);
    }
    //------------------------------------------------------------------------
    void Scrollbar::ThumbMoveMFunc(const EventArgs *)
    {
        // adjust scroll bar position as required.
        setCurrent(static_cast<ScrollbarView *>(mWidgetView)->getThumbValue());
    }
    //------------------------------------------------------------------------
    void Scrollbar::PositiveMFunc(const EventArgs * e)
    {
        if(static_cast<const CursorEventArgs *>(e)->mMouseArg.mButton != MB_Left)
            return;

        posStep();
    }
    //------------------------------------------------------------------------
    void Scrollbar::NegativeMFunc(const EventArgs * e)
    {
        if(static_cast<const CursorEventArgs *>(e)->mMouseArg.mButton != MB_Left)
            return;

        negStep();
    }
    //------------------------------------------------------------------------
    void Scrollbar::ThumbBeginMFunc(const EventArgs *)
    {
        // simply trigger our own version of this event
        WidgetEventArgs args(this);
        onTumbMoveBegin(&args);
    }
    //------------------------------------------------------------------------
    void Scrollbar::ThumbEndMFunc(const EventArgs *)
    {
        // simply trigger our own version of this event
        WidgetEventArgs args(this);
        onThumbMoveEnd(&args);
    }
    //------------------------------------------------------------------------
    bool Scrollbar::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdDocumentSize());
        dest->add(N_new CmdPos());
        dest->add(N_new CmdStepSize());
        dest->add(N_new CmdOverlapSize());
        dest->add(N_new CmdScrollPosition());
        dest->add(N_new CmdUnitIntervalScrollPosition());
        dest->add(N_new CmdEndLockEnabled());
        return true;
    }
    //------------------------------------------------------------------------
    void Scrollbar::disableWrite()
    {
        Widget::disableWrite();

        setWrite(N_PropertyUIScrollbar_DocSize, false);
        setWrite(N_PropertyUIScrollbar_PageSize, false);
        setWrite(N_PropertyUIScrollbar_StepSize, false);
        setWrite(N_PropertyUIScrollbar_OverlapSize, false);
        setWrite(N_PropertyUIScrollbar_Current, false);
        setWrite(N_PropertyUIWidget_Visible, false);
    }
    //------------------------------------------------------------------------
    bool Scrollbar::setCurrentImpl(NIIf position)
    {
        const NIIf old_pos = mCurrentPos;
        const NIIf max_pos = getCurrentMax();

        // limit position to valid range:  0 <= position <= max_pos
        mCurrentPos = (position >= 0) ? ((position <= max_pos) ? position : max_pos) : 0.0f;

        return mCurrentPos != old_pos;
    }
    //------------------------------------------------------------------------
    void Scrollbar::setConfig(NIIf doc, NIIf page_size, NIIf step_size, NIIf overlap_size, NIIf position)
    {
        const bool autoend = mAutoEnd && isCurrentEnd();
        bool config_changed = false;
        bool position_changed = false;

        if(doc && (mDocSize != doc))
        {
            mDocSize = doc;
            config_changed = true;
        }

        if(page_size && (mPageSize != page_size))
        {
            mPageSize = page_size;
            config_changed = true;
        }

        if(step_size && (mStepSize != step_size))
        {
            mStepSize = step_size;
            config_changed = true;
        }

        if(overlap_size && (mPageRemian != overlap_size))
        {
            mPageRemian = overlap_size;
            config_changed = true;
        }

        if(position)
            position_changed = setCurrentImpl(position);
        else if(autoend)
            position_changed = setCurrentImpl(getCurrentMax());

        // _always_ update the thumb to keep things in sync.  (though this
        // can cause a double-trigger of CurrentChageEvent, which
        // also happens with setCurrent anyway).
        static_cast<ScrollbarView *>(mWidgetView)->updateThumb();

        //
        // Fire appropriate events based on actions we took.
        //
        if(config_changed)
        {
            WidgetEventArgs args(this);
            onConfigChange(&args);
        }

        if(position_changed)
        {
            WidgetEventArgs args(this);
            onCurrentChange(&args);
        }
    }
    //------------------------------------------------------------------------
    NIIf Scrollbar::getCurrentMax() const
    {
        // max position is (docSize - pageSize)
        // but must be at least 0 (in case doc size is very small)
        return N_MAX(NIIf, (mDocSize - mPageSize), 0.0f);
    }
    //------------------------------------------------------------------------
    bool Scrollbar::isCurrentEnd() const
    {
        return mCurrentPos >= getCurrentMax();
    }
    //------------------------------------------------------------------------
    void Scrollbar::setCurrentUnit(NIIf position)
    {
        const NIIf range = mDocSize - mPageSize;

        setCurrent(range > 0 ? position * range : 0.0f);
    }
    //------------------------------------------------------------------------
    NIIf Scrollbar::getCurrentUnit() const
    {
        const NIIf range = mDocSize - mPageSize;

        return (range > 0) ? mCurrentPos / range : 0.0f;
    }
    //------------------------------------------------------------------------
    void Scrollbar::posStep()
    {
        setCurrent(mCurrentPos + mStepSize);
    }
    //------------------------------------------------------------------------
    void Scrollbar::negStep()
    {
        setCurrent(mCurrentPos - mStepSize);
    }
    //------------------------------------------------------------------------
    void Scrollbar::posPage()
    {
        setCurrent(mCurrentPos + (mPageSize - mPageRemian));
    }
    //------------------------------------------------------------------------
    void Scrollbar::negPage()
    {
        setCurrent(mCurrentPos - (mPageSize - mPageRemian));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ScrollbarView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdVerticalScrollbar : public PropertyCmd
    {
    public:
        CmdVerticalScrollbar() : PropertyCmd(N_PropertyUIScrollbar_Vertical, _T("VerticalScrollbar"),
            _T("Property to get/set whether the Scrollbar operates in the vertical direction."
            "  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ScrollbarView * obj = static_cast<ScrollbarView *>(static_cast<const Widget *>(own)->getWidgetView());

            StrConv::conv(obj->isVertical(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ScrollbarView * obj = static_cast<ScrollbarView *>(static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setVertical(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ScrollbarView * obj = static_cast<ScrollbarView *>(
                static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isVertical();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ScrollbarView * obj = static_cast<ScrollbarView *>(static_cast<Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setVertical(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVerticalScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    /*Named Areas:
            - ThumbTrackArea
    Property initialiser definitions:
            - VerticalScrollbar - boolean property.
            Indicates whether this scrollbar will operate in the vertical or
            horizontal direction.  Default is for horizontal.  Optional.*/
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ScrollbarView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ScrollbarView::ScrollbarView(WidgetViewlID wsid) :
        WidgetView(wsid, N_WIDGET_Scrollbar),
        mYDirection(false)
    {
        add(N_new CmdVerticalScrollbar());
    }
    //------------------------------------------------------------------------
    void ScrollbarView::flush()
    {
        // get Style for the assigned look.
        const Style * style = getStyle();
        // try and get mode for our current state
        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        // peform the rendering operation.
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    void ScrollbarView::layout()
    {
        updateThumb();
    }
    //------------------------------------------------------------------------
    void ScrollbarView::updateThumb(void)
    {
        Scrollbar * w = (Scrollbar*)mSrc;
        const Style * style = getStyle();
        Rectf area(style->getSpecial(ThumbTrackArea).getArea().getPixelRect(*w));

        PosButton * pb = w->getThumb();

        NIIf posExtent = w->getDocSize() - w->getPageSize();
        NIIf slideExtent;

        if (mYDirection)
        {
            slideExtent = area.getHeight() - pb->getPixelSize().mHeight;
            pb->setYRange(area.top() / w->getPixelSize().mHeight, (area.top() + slideExtent) / w->getPixelSize().mHeight);
            pb->setPos(RelVector2f(RelPosf(0, area.left()),
                RelPosf((area.top() + (w->getCurrent() * (slideExtent / posExtent))) / w->getPixelSize().mHeight, 0)));
        }
        else
        {
            slideExtent = area.getWidth() - pb->getPixelSize().mWidth;
            pb->setXRange(area.left() / w->getPixelSize().mWidth, (area.left() + slideExtent) / w->getPixelSize().mWidth);
            pb->setPos(RelVector2f(RelPosf((area.left() + (w->getCurrent() * (slideExtent / posExtent))) / w->getPixelSize().mWidth, 0),
                RelPosf(0, area.top())));
        }
    }
    //------------------------------------------------------------------------
    NIIf ScrollbarView::getThumbValue() const
    {
        Scrollbar * w = (Scrollbar *)mSrc;
        const Style * style = getStyle();
        const Rectf area(style->getSpecial(ThumbTrackArea).getArea().getPixelRect(*w));

        PosButton * pb = w->getThumb();
        NIIf posExtent = w->getDocSize() - w->getPageSize();

        if (mYDirection)
        {
            NIIf slideExtent = area.getHeight() - pb->getPixelSize().mHeight;
            return (pb->getPos().y.abs(w->getPixelSize().mHeight) - area.top()) / (slideExtent / posExtent);
        }
        else
        {
            NIIf slideExtent = area.getWidth() - pb->getPixelSize().mWidth;
            return (pb->getPos().x.abs(w->getPixelSize().mWidth) - area.left()) / (slideExtent / posExtent);
        }
    }
    //------------------------------------------------------------------------
    NIIf ScrollbarView::getDirection(const Vector2f & pt) const
    {
        Scrollbar * w = (Scrollbar *)mSrc;
        const Rectf & absrect(w->getThumb()->getArea().getIntCache());

        if ((mYDirection && (pt.y > absrect.bottom())) || (!mYDirection && (pt.x > absrect.right())))
        {
            return 1;
        }
        else if ((mYDirection && (pt.y < absrect.top())) || (!mYDirection && (pt.x < absrect.left())))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    //------------------------------------------------------------------------
}
}