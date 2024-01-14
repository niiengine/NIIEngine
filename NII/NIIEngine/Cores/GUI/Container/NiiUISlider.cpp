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
#include "NiiUISlider.h"
#include "NiiUIButton.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIException.h"
#include "NiiUIWidgetList.h"

#define ThumbTrackArea 1

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdCurrentValue : public PropertyCmd
    {
    public:
        CmdCurrentValue() : PropertyCmd(N_PropertyUISlider_Current, _T("current_value"), _I18n("current value"))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Slider *>(own)->getCurrent(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Slider *>(own)->setCurrent(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Slider *>(own)->getCurrent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Slider *>(own)->setCurrent(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCurrentValue();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.0f, out);
        }
    };

    class _EngineInner CmdMaximumValue : public PropertyCmd
    {
    public:
        CmdMaximumValue() : PropertyCmd(N_PropertyUISlider_MaxValue, _T("maximum_value"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Slider *>(own)->getMaxValue(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Slider *>(own)->setMaxValue(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Slider *>(own)->getMaxValue();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Slider *>(own)->setMaxValue(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaximumValue();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)1.0f, out);
        }
    };

    class _EngineInner CmdClickStepSize : public PropertyCmd
    {
    public:
        CmdClickStepSize() : PropertyCmd(N_PropertyUISlider_StepValue, _T("click_step_size"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Slider *>(own)->getStepValue(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Slider *>(own)->setStepValue(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Slider *>(own)->getStepValue();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Slider *>(own)->setStepValue(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdClickStepSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.01f, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, Slider, ValueEvent,              0);
    N_EVENT(Container, Slider, ThumbMoveBeginEvent,     1);
    N_EVENT(Container, Slider, TumbMoveEndEvent,        2);
    N_EVENT(Container, Slider, EventCount,              3);
    //------------------------------------------------------------------------
    Slider::Slider(WidgetID wid, FactoryID fid, Container * own, const String & name, 
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_Slider, lid),
        mCurrentValue(0.0f),
        mMaxValue(1.0f),
        mStepValue(0.01f),
        mThumbView(0),
        mThumb(0)
    {
        notifySignal(Slider::EventCount);
        setupCmd(N_CmdObj_Slider);
    }
    //------------------------------------------------------------------------
    Slider::~Slider()
    {
        if(mThumb == 0)
        {
            N_Only(Widget).destroy(mThumb);
            mThumb = 0;
        }
    }
    //------------------------------------------------------------------------
    void Slider::initChild()
    {
        if(mThumb == 0)
            mThumb = static_cast<PosButton *>(N_Only(Widget).create(mThumbView, 0, this, N_WIDGET_PosButton));

        mThumb->bind(PosButton::PosChangeEvent, this, (CommandObjFunctor)&Slider::ThumbMoveMFunc);
        mThumb->bind(Slider::ThumbMoveBeginEvent, this, (CommandObjFunctor)&Slider::ThumbBeginMFunc);
        mThumb->bind(Slider::TumbMoveEndEvent, this, (CommandObjFunctor)&Slider::ThumbEndMFunc);

        layout();
    }
    //------------------------------------------------------------------------
    void Slider::setMaxValue(NIIf maxVal)
    {
        mMaxValue = maxVal;

        NIIf oldval = mCurrentValue;

        // limit current value to be within new max
        if (mCurrentValue > mMaxValue)
        {
            mCurrentValue = mMaxValue;
        }

        static_cast<SliderView *>(mWidgetView)->updateThumb();

        // send notification if slider value changed.
        if (mCurrentValue != oldval)
        {
            WidgetEventArgs args(this);
            onValueChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void Slider::setCurrent(NIIf value)
    {
        NIIf oldval = mCurrentValue;

        // range for value: 0 <= value <= maxValue
        mCurrentValue = (value >= 0.0f) ? ((value <= mMaxValue) ? value : mMaxValue) : 0.0f;

        static_cast<SliderView *>(mWidgetView)->updateThumb();

        // send notification if slider value changed.
        if(mCurrentValue != oldval)
        {
            WidgetEventArgs args(this);
            onValueChange(&args);
        }
    }
    //------------------------------------------------------------------------
    bool Slider::validImpl(const WidgetView * style) const
    {
        return dynamic_cast<const SliderView *>(style) != 0;
    }
    //------------------------------------------------------------------------
    void Slider::onValueChange(const WidgetEventArgs * arg)
    {
        signal(ValueEvent, arg);
    }
    //------------------------------------------------------------------------
    void Slider::onThumbMoveBegin(const WidgetEventArgs * arg)
    {
        signal(ThumbMoveBeginEvent, arg);
    }
    //------------------------------------------------------------------------
    void Slider::onThumbMoveEnd(const WidgetEventArgs * arg)
    {
        signal(TumbMoveEndEvent, arg);
    }
    //------------------------------------------------------------------------
    void Slider::onButtonDown(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            NIIf adj = static_cast<SliderView *>(mWidgetView)->getDirection(Vector2f(arg->mLocalX, arg->mLocalY));
            // adjust slider position in whichever direction as required.
            if(adj != 0)
            {
                setCurrent(mCurrentValue + (adj * mStepValue));
            }
        }
    }
    //------------------------------------------------------------------------
    void Slider::onCursorDrag(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onCursorDrag(arg);

        // scroll by e.mScroll * stepSize
        setCurrent(mCurrentValue + mStepValue * arg->mMouseArg.mScroll);
    }
    //------------------------------------------------------------------------
    void Slider::ThumbMoveMFunc(const EventArgs *)
    {
        setCurrent(static_cast<SliderView *>(mWidgetView)->getThumbValue());
    }
    //------------------------------------------------------------------------
    void Slider::ThumbBeginMFunc(const EventArgs *)
    {
        // simply trigger our own version of this event
        WidgetEventArgs args(this);
        onThumbMoveBegin(&args);
    }
    //------------------------------------------------------------------------
    void Slider::ThumbEndMFunc(const EventArgs *)
    {
        // simply trigger our own version of this event
        WidgetEventArgs args(this);
        onThumbMoveEnd(&args);
    }
    //------------------------------------------------------------------------
    bool Slider::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdCurrentValue());
        dest->add(N_new CmdMaximumValue());
        dest->add(N_new CmdClickStepSize());
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SliderView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdVerticalSlider : public PropertyCmd
    {
    public:
        CmdVerticalSlider() : PropertyCmd(N_PropertyUISlider_Vertical, _T("VerticalSlider"),
            _T("Property to get/set whether the Slider operates in the vertical direction."
            "  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            SliderView * obj =
                static_cast<SliderView *>(
                    static_cast<const Widget *>(own)->getWidgetView());

            StrConv::conv(obj->isVertical(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            SliderView * obj =
                static_cast<SliderView *>(
                    static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setVertical(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            SliderView * obj =
                static_cast<SliderView *>(
                    static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isVertical();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            SliderView * obj = static_cast<SliderView *>(
                static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setVertical(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVerticalSlider();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdReversedDirection : public PropertyCmd
    {
    public:
        CmdReversedDirection() : PropertyCmd(N_PropertyUISlider_Reverse, _T("ReversedDirection"),
            _T("Property to get/set whether the Slider operates in reversed direction."
            "  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            SliderView * obj = static_cast<SliderView *>(
                static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->isReversedDirection(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            SliderView * obj = static_cast<SliderView *>(
                static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setReversedDirection(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            SliderView * obj = static_cast<SliderView *>(
                static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isReversedDirection();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            SliderView * obj = static_cast<SliderView *>(
                static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setReversedDirection(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdReversedDirection();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    SliderView::SliderView(WidgetViewlID type) :
        WidgetView(type, N_VIEW_Slider),
        mVDirection(false),
        mReverse(false)
    {
        add(N_new CmdVerticalSlider());
        add(N_new CmdReversedDirection());
    }
    //------------------------------------------------------------------------
    void SliderView::flush()
    {
        // get Style for the assigned look.
        const Style * style = getStyle();
        // try and get mode for our current state
        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        // peform the rendering operation.
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    void SliderView::layout()
    {
        updateThumb();
    }
    //------------------------------------------------------------------------
    void SliderView::updateThumb()
    {
        Slider * w = (Slider *)mSrc;
        // get area the thumb is supposed to use as it's area.
        const Style * style = getStyle();
        Rectf area(style->getSpecial(ThumbTrackArea).getArea().getPixelRect(*w));
        // get accesss to the thumb
        PosButton * theThumb = w->getThumb();

        const PlaneSizef w_pixel_size(w->getPixelSize());

        const NIIf thumbRelXPos = w_pixel_size.mWidth == 0.0f ? 0.0f : (area.left() / w_pixel_size.mWidth);
        const NIIf thumbRelYPos = w_pixel_size.mHeight == 0.0f ? 0.0f : (area.top() / w_pixel_size.mHeight);
        // get base location for thumb widget
        RelVector2f thumbPosition(RelPosf(thumbRelXPos, 0), RelPosf(thumbRelYPos, 0));

        // Is this a vertical slider
        if (mVDirection)
        {
            // pixel extent of total available area the thumb moves in
            NIIf slideExtent = area.height() - theThumb->getPixelSize().mHeight;

            // Set range of motion for the thumb widget
            if (w_pixel_size.mHeight != 0.0f)
                theThumb->setYRange(area.top() / w_pixel_size.mHeight,
                (area.top() + slideExtent) / w_pixel_size.mHeight);
            else
                theThumb->setYRange(0.0f, 0.0f);

            // calculate vertical positon for thumb
            NIIf thumbOffset = w->getCurrent() * (slideExtent / w->getMaxValue());

            if (w_pixel_size.mHeight != 0.0f)
                thumbPosition.y.mScale += (mReverse ? thumbOffset : slideExtent - thumbOffset) / w_pixel_size.mHeight;
        }
        // Horizontal slider
        else
        {
            // pixel extent of total available area the thumb moves in
            NIIf slideExtent = area.width() - theThumb->getPixelSize().mWidth;

            // Set range of motion for the thumb widget
            if (w_pixel_size.mWidth != 0.0f)
                theThumb->setXRange(area.left() / w_pixel_size.mWidth,
                (area.left() + slideExtent) / w_pixel_size.mWidth);
            else
                theThumb->setXRange(0.0f, 0.0f);


            // calculate horizontal positon for thumb
            NIIf thumbOffset = w->getCurrent() * (slideExtent / w->getMaxValue());

            if (w_pixel_size.mWidth != 0.0f)
                thumbPosition.x.mScale += (mReverse ? slideExtent - thumbOffset : thumbOffset) / w_pixel_size.mWidth;
        }

        // set new position for thumb.
        theThumb->setPos(thumbPosition);
    }
    //------------------------------------------------------------------------
    NIIf SliderView::getThumbValue(void) const
    {
        Slider * w = (Slider *)mSrc;
        // get area the thumb is supposed to use as it's area.
        const Style * style = getStyle();
        const Rectf area(style->getSpecial(ThumbTrackArea).getArea().getPixelRect(*w));
        // get accesss to the thumb
        PosButton * theThumb = w->getThumb();

        // slider is vertical
        if (mVDirection)
        {
            // pixel extent of total available area the thumb moves in
            NIIf slideExtent = area.height() - theThumb->getPixelSize().mHeight;
            // calculate value represented by current thumb position
            NIIf thumbValue = (theThumb->getPos().y.abs(w->getPixelSize().mHeight) - area.top()) /
                (slideExtent / w->getMaxValue());
            // return final thumb value according to slider settings
            return mReverse ? thumbValue : w->getMaxValue() - thumbValue;
        }
        // slider is horizontal
        else
        {
            // pixel extent of total available area the thumb moves in
            NIIf slideExtent = area.getWidth() - theThumb->getPixelSize().mWidth;
            // calculate value represented by current thumb position
            NIIf thumbValue = (theThumb->getPos().x.abs(w->getPixelSize().mWidth) - area.left()) /
                (slideExtent / w->getMaxValue());
            // return final thumb value according to slider settings
            return mReverse ? w->getMaxValue() - thumbValue : thumbValue;
        }
    }
    //------------------------------------------------------------------------
    NIIf SliderView::getDirection(const Vector2f & pt) const
    {
        Slider* w = (Slider *)mSrc;
        const Rectf absrect(w->getThumb()->getArea().getIntCache());

        if ((mVDirection && (pt.y < absrect.top())) ||
            (!mVDirection && (pt.x > absrect.right())))
        {
            return mReverse ? -1.0f : 1.0f;
        }
        else if ((mVDirection && (pt.y > absrect.bottom())) ||
            (!mVDirection && (pt.x < absrect.left())))
        {
            return mReverse ? 1.0f : -1.0f;
        }
        else
        {
            return 0;
        }
    }
    //------------------------------------------------------------------------
    bool SliderView::isVertical() const
    {
        return mVDirection;
    }
    //------------------------------------------------------------------------
    void SliderView::setVertical(bool b)
    {
        mVDirection = b;
    }
    //------------------------------------------------------------------------
    bool SliderView::isReversedDirection() const
    {
        return mReverse;
    }
    //------------------------------------------------------------------------
    void SliderView::setReversedDirection(bool b)
    {
        mReverse = b;
    }
    //------------------------------------------------------------------------
}
}