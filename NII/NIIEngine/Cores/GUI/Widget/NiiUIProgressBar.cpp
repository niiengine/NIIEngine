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
#include "NiiUIProgressBar.h"
#include "NiiUIWidgetManager.h"

#define ProgressArea 1

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdCurrentProgress : public PropertyCmd
    {
    public:
        CmdCurrentProgress() : PropertyCmd(N_PropertyUIProgressBar_Progress, _T("current_progress"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ProgressBar *>(own)->getProgress(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ProgressBar *>(own)->setProgress(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ProgressBar *>(own)->getProgress();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ProgressBar *>(own)->setProgress(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCurrentProgress();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdStepSize : public PropertyCmd
    {
    public:
        CmdStepSize() : PropertyCmd(N_PropertyUIProgressBar_Step, _T("step_size"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ProgressBar *>(own)->getStep(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ProgressBar *>(own)->setStep(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ProgressBar *>(own)->getStep();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ProgressBar *>(own)->setStep(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdStepSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0");
        }
    };
    //------------------------------------------------------------------------
    //const StyleID ProgressBar::Type = "";
    //------------------------------------------------------------------------
    N_EVENT(Widget,        ProgressBar,    PrcChangeEvent,    0);
    N_EVENT(Widget,        ProgressBar,    PrcDoneEvent,      1);
    N_EVENT(Widget,        ProgressBar,    EventCount,        2);
    //------------------------------------------------------------------------
    ProgressBar::ProgressBar(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Widget(N_CmdObj_ProgressBar, wid, fid, own, name, lid),
        mProgress(0),
        mStep(0.01f)
    {
        notifySignal(ProgressBar::EventCount);
        setupCmd(N_CmdObj_ProgressBar);
    }
    //------------------------------------------------------------------------
    ProgressBar::~ProgressBar()
    {
    }
    //------------------------------------------------------------------------
    void ProgressBar::setProgress(NIIf progress)
    {
        // legal progress rangeis : 0.0f <= progress <= 1.0f
        progress = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;

        if(progress != mProgress)
        {
            // update progress and fire off event.
            mProgress = progress;
            WidgetEventArgs args(this);
            onPrcChange(&args);

            // if new progress is 100%, fire off the 'done' event as well.
            if(mProgress == 1.0f)
            {
                onPrcDone(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    NIIf ProgressBar::getProgress() const 
    {
        return mProgress;
    }
    //------------------------------------------------------------------------
    void ProgressBar::setStep(NIIf step) 
    {
        mStep = step;
    }
    //------------------------------------------------------------------------
    NIIf ProgressBar::getStep() const 
    {
        return mStep;
    }
    //------------------------------------------------------------------------
    void ProgressBar::step() 
    {
        setProgress(mProgress + mStep);
    }
    //------------------------------------------------------------------------
    void ProgressBar::adjust(NIIf delta) 
    {
        setProgress(mProgress + delta);
    }
    //------------------------------------------------------------------------
    void ProgressBar::onPrcChange(const WidgetEventArgs * arg)
    {
        refresh();
        signal(PrcChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ProgressBar::onPrcDone(const WidgetEventArgs * arg)
    {
        signal(PrcDoneEvent, arg);
    }
    //------------------------------------------------------------------------
    bool ProgressBar::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdCurrentProgress());
        dest->add(N_new CmdStepSize());
        return true;
    }
    //------------------------------------------------------------------------
    /**
    Named Areas:
            - ProgressArea

    Property initialiser definitions:
            - VerticalProgress - boolean property.
              Determines whether the progress widget is horizontal or vertical.
              Default is horizontal.  Optional.

            - ReversedProgress - boolean property.
              Determines whether the progress grows in the opposite direction to
              what is considered 'usual'.  Set to "True" to have progress grow
              towards the left or bottom of the progress area.  Optional.
    */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ProgressBarView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdVerticalProgress : public PropertyCmd
    {
    public:
        CmdVerticalProgress() : PropertyCmd(N_PropertyUIProgressBar_Vertical, _T("VerticalProgress"),
            _T("Property to get/set whether the ProgressBar operates in the vertical direction."
            "  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<const Widget *>(own)->getWidgetView());

            StrConv::conv(obj->isVertical(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<Widget *>(own)->getWidgetView());
                
            bool temp;
            StrConv::conv(val, temp);
            obj->setVertical(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<const Widget *>(own)->getWidgetView());

            bool * temp = (bool *)out;
            *temp = obj->isVertical();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<const Widget *>(own)->getWidgetView());

            bool * temp = (bool *)in;
            obj->setVertical(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVerticalProgress();
        }

        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdReversedProgress : public PropertyCmd
    {
    public:
        CmdReversedProgress() : PropertyCmd(N_PropertyUIProgressBar_Reverse, _T("ReversedProgress"),
            _T("Property to get/set whether the ProgressBar operates in reversed direction."
            "  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<const Widget *>(own)->getWidgetView());

            StrConv::conv(obj->isReversed(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<Widget *>(own)->getWidgetView());

            bool temp;
            StrConv::conv(val, temp);
            obj->setReversed(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<const Widget *>(own)->getWidgetView());

            bool * temp = (bool *)out;
            *temp = obj->isReversed();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ProgressBarView * obj = static_cast<ProgressBarView *>(
                static_cast<const Widget *>(own)->getWidgetView());

            bool * temp = (bool *)in;
            obj->setReversed(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdReversedProgress();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    N_STATE(WidgetView, ProgressBarView, EnableProgressState,   0);
    N_STATE(WidgetView, ProgressBarView, DisableProgressState,  1);
    N_STATE(WidgetView, ProgressBarView, StateCount,            2);
    //------------------------------------------------------------------------
    ProgressBarView::ProgressBarView(WidgetViewlID type) :
        WidgetView(type, N_VIEW_ProgressBar),
        mVDirection(false),
        mReverse(false)
    {
        add(N_new CmdVerticalProgress());
        add(N_new CmdReversedProgress());
    }
    //------------------------------------------------------------------------
    void ProgressBarView::flush()
    {
        const StyleLayout * mode;

        // get Style for the assigned look.
        const Style * style = getStyle();
        // try and get mode for our current state
        mode = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        // peform the rendering operation.
        mode->render(mSrc);

        // get mode for actual progress rendering
        mode = &style->getLayout(mSrc->isFinalDisable() ? DisableProgressState : EnableProgressState);

        // get target rect for this mode
        Rectf progressRect(style->getSpecial(ProgressArea).getArea().getPixelRect(*mSrc));

        // calculate a clipper according to the current progress.
        Rectf progressClipper(progressRect);

        ProgressBar * w = (ProgressBar *)mSrc;
        if (mVDirection)
        {
            NIIf height = N_ToPixel(progressClipper.getHeight() * w->getProgress());

            if (mReverse)
            {
                progressClipper.setHeight(height);
            }
            else
            {
                progressClipper.setTop(progressClipper.bottom() - height);
            }
        }
        else
        {
            NIIf width = N_ToPixel(progressClipper.getWidth() * w->getProgress());

            if (mReverse)
            {
                progressClipper.setLeft(progressClipper.right() - width);
            }
            else
            {
                progressClipper.setWidth(width);
            }
        }

        // peform the rendering operation.
        mode->render(mSrc, progressRect, 0, &progressClipper);
    }
    //------------------------------------------------------------------------
    bool ProgressBarView::isVertical() const
    {
        return mVDirection;
    }
    //------------------------------------------------------------------------
    bool ProgressBarView::isReversed() const
    {
        return mReverse;
    }
    //------------------------------------------------------------------------
    void ProgressBarView::setVertical(bool set)
    {
        mVDirection = set;
    }
    //------------------------------------------------------------------------
    void ProgressBarView::setReversed(bool set)
    {
        mReverse = set;
    }
    //------------------------------------------------------------------------
}
}