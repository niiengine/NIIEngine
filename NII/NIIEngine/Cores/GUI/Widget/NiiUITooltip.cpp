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
#include "NiiUITooltip.h"
#include "NiiFont.h"
#include "NiiPixelBuffer.h"
#include "NiiUIWidgetManager.h"

#define TextArea 1

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdHoverTime : public PropertyCmd
    {
    public:
        CmdHoverTime() : PropertyCmd(N_PropertyUITooltip_HoverTime, _T("hover_time"),
            _T("Property to get/set the hover timeout value in seconds.  Value is a NIIf."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Tooltip *>(own)->getHoverTime(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Tooltip *>(own)->setHoverTime(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Tooltip *>(own)->getHoverTime();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Tooltip *>(own)->setHoverTime(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHoverTime();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("400");
        }
    };

    class _EngineInner CmdDisplayTime : public PropertyCmd
    {
    public:
        CmdDisplayTime() : PropertyCmd(N_PropertyUITooltip_DisplayTime, _T("display_time"),
            _T("Property to get/set the display timeout value in seconds.  Value is a Nui32."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv((Nui32)static_cast<const Tooltip *>(own)->getDisplayTime(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Tooltip *>(own)->setDisplayTime(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Tooltip *>(own)->getDisplayTime();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Tooltip *>(own)->setDisplayTime(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDisplayTime();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("7500");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Widget,        Tooltip,    HoverTimeEvent,              0);
    N_EVENT(Widget,        Tooltip,    ShowTimeEvent,               1);
    N_EVENT(Widget,        Tooltip,    TooltipActiveEvent,          2);
    N_EVENT(Widget,        Tooltip,    TooltipInactiveEvent,        3);
    N_EVENT(Widget,        Tooltip,    TooltipDestEvent,            4);
    N_EVENT(Widget,        Tooltip,    EventCount,                  5);
    //------------------------------------------------------------------------
    Tooltip::Tooltip(WidgetID wid, FactoryID fid, Container * own, const String & name, LangID lid) :
        Widget(N_CmdObj_Tooltip, wid, fid, own, name, lid),
        mHoverTime(400),
        mShowTime(7500),
        mPosMutex(false)
    {
        notifySignal(Tooltip::EventCount);
        setupCmd(N_CmdObj_Tooltip);

        setAutoClip(false);
        setAutoDestroy(false);
        setTop(true);

        // we need updates even when not visible
        setUpdateMode(RM_Alway);

        mShowState = false;
        mCost = 0;
        WidgetEventArgs args(this);
        onTooltipInactive(&args);
        mDest = 0;
        
        hide();
    }
    //------------------------------------------------------------------------
    Tooltip::~Tooltip()
    {
    }
    //------------------------------------------------------------------------
    void Tooltip::updatePos()
    {
        if(mPosMutex)
            return;

        mPosMutex = true;

        Cursor * cursor = mSheet->getCursor();
        Recti screen(0, 0, getRenderSize());
        Rectf tipRect(getArea().getIntCache());
        const PixelBuffer * mouseImage = cursor->getImage();

        Vector2f mousePos(cursor->getPos());
        PlaneSizef mouseSz(0,0);

        if(mouseImage)
        {
            mouseSz = mouseImage->getSize();
        }

        Vector2f tmpPos(mousePos.x + mouseSz.mWidth, mousePos.y + mouseSz.mHeight);
        tipRect.setPos(tmpPos);

        // if tooltip would be off the right of the screen,
        // reposition to the other side of the mouse cursor.
        if(screen.right() < tipRect.right())
        {
            tmpPos.x = mousePos.x - tipRect.getWidth() - 5;
        }

        // if tooltip would be off the bottom of the screen,
        // reposition to the other side of the mouse cursor.
        if(screen.bottom() < tipRect.bottom())
        {
            tmpPos.y = mousePos.y - tipRect.getHeight() - 5;
        }

        // set final position of tooltip window.
        setPos(RelVector2f(RelPosf(0, tmpPos.x), RelPosf(0, tmpPos.y)));

        mPosMutex = false;
    }
    //------------------------------------------------------------------------
    void Tooltip::setDest(Widget * obj)
    {
        if(!obj)
        {
            mDest = obj;
        }
        else if(obj != this)
        {
            if(mDest != obj)
            {
                obj->getParent()->add(this);
                mDest = obj;
            }

            // set text to that of the tooltip text of the target
            setText(obj->getTooltipText());

            PlaneSizef textSize(getViewSize());

            setSize(RelPlaneSizef(RelPosf(0, textSize.mWidth), RelPosf(0, textSize.mHeight)));
            updatePos();
        }

        resetState();

        if(mShowState)
        {
            WidgetEventArgs args(this);
            onTooltipTransition(&args);
        }
    }
    //------------------------------------------------------------------------
    const Widget * Tooltip::getDest()
    {
        return mDest;
    }
    //------------------------------------------------------------------------
    PlaneSizef Tooltip::getViewSize() const
    {
        if (mWidgetView != 0)
        {
            TooltipView * wr = (TooltipView *)mWidgetView;
            return wr->getTextSize();
        }
        else
        {
            return getTextSize();
        }
    }
    //------------------------------------------------------------------------
    PlaneSizef Tooltip::getTextSize() const
    {
        const PixelUnitGrid * rs = getRenderText();
        PlaneSizef sz(0.0f, 0.0f);

        for(NCount i = 0; i < rs->getLineCount(); ++i)
        {
            const PlaneSizef line_sz(rs->getPixelSize(this, i));
            sz.mHeight += line_sz.mHeight;

            if (line_sz.mWidth > sz.mWidth)
                sz.mWidth = line_sz.mWidth;
        }

        return sz;
    }
    //------------------------------------------------------------------------
    void Tooltip::resetState(void)
    {
        mCost = 0;
    }
    //------------------------------------------------------------------------
    void Tooltip::setHoverTime(TimeDurMS ms)
    {
        if(mHoverTime != ms)
        {
            mHoverTime = ms;

            WidgetEventArgs args(this);
            getHoverTime(&args);
        }
    }
    //------------------------------------------------------------------------
    void Tooltip::setDisplayTime(TimeDurMS ms)
    {
        if (mShowTime != ms)
        {
            mShowTime = ms;

            WidgetEventArgs args(this);
            getShowTime(&args);
        }
    }
    //------------------------------------------------------------------------
    bool Tooltip::validImpl(const WidgetView * view) const
    {
        return dynamic_cast<const TooltipView *>(view) != 0;
    }
    //------------------------------------------------------------------------
    void Tooltip::updateImpl(TimeDurMS cost)
    {
        Widget::updateImpl(cost);

        if(mShowState)
        {
            if (!mDest || mDest->getTooltipText().empty())
            {
                hide();
                mShowState = false;
                mCost = 0;
                WidgetEventArgs args(this);
                onTooltipInactive(&args);
                mDest = 0;
            }
            else if ((mShowTime > 0) && ((mCost += cost) >= mShowTime))
            {
                mShowState = false;
                mCost = 0;
                WidgetEventArgs args(this);
                onTooltipInactive(&args);
                mDest = 0;
            }
        }
        else
        {
            if(mDest && !mDest->getTooltipText().empty() && ((mCost += cost) >= mHoverTime))
            {
                updatePos();
                show();

                mShowState = true;
                mCost = 0;

                WidgetEventArgs args(this);
                onTooltipActive(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Tooltip::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdHoverTime());
        dest->add(N_new CmdDisplayTime());
        return true;
    }
    //------------------------------------------------------------------------
    void Tooltip::onHide(const WidgetEventArgs * arg)
    {
        Widget::onHide(arg);

        // The animation will take care of fade out or whatnot,
        // at the end it will hide the tooltip to let us know the transition
        // is done. At this point we will remove the tooltip from the
        // previous parent.

        // NOTE: There has to be a fadeout animation! Even if it's a 0 second
        //       immediate hide animation.

        if(mParent)
        {
            mParent->remove(this);
        }
    }
    //------------------------------------------------------------------------
    void Tooltip::onCursorEnter(const CursorEventArgs * arg)
    {
        updatePos();

        Widget::onCursorEnter(arg);
    }
    //------------------------------------------------------------------------
    void Tooltip::onText(const WidgetEventArgs * arg)
    {
        onText(arg);
        
        PlaneSizef textSize(getViewSize());
        setSize(RelPlaneSizef(RelPosf(0, textSize.mWidth), RelPosf(0, textSize.mHeight)));
        updatePos();

        // we do not signal we handled it, in case user wants to hear
        // about text changes too.
    }
    //------------------------------------------------------------------------
    void Tooltip::getHoverTime(const WidgetEventArgs * arg)
    {
        signal(HoverTimeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tooltip::getShowTime(const WidgetEventArgs * arg)
    {
        signal(ShowTimeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tooltip::onTooltipActive(const WidgetEventArgs * arg)
    {
        signal(TooltipActiveEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tooltip::onTooltipInactive(const WidgetEventArgs * arg)
    {
        signal(TooltipInactiveEvent, arg);
    }
    //------------------------------------------------------------------------
    void Tooltip::onTooltipTransition(const WidgetEventArgs * arg)
    {
        signal(TooltipDestEvent, arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TooltipView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TooltipView::TooltipView(WidgetViewlID wmid) :
        WidgetView(wmid, N_VIEW_Tooltip)
    {
    }
    //------------------------------------------------------------------------
    void TooltipView::flush()
    {
        const Style * style = getStyle();
        const StyleLayout * ss = &style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    PlaneSizef TooltipView::getTextSize() const
    {
        Tooltip * w = static_cast<Tooltip *>(mSrc);
        PlaneSizef sz(w->getTextSize());

        const Style * style = getStyle();

        const Rectf textArea(style->getSpecial(TextArea).getArea().getPixelRect(*w));
        const Rectf wndArea(abs(w->getRelArea(), w->calcParentArea()));

        sz.mWidth = N_ToPixel(sz.mWidth + wndArea.getWidth() - textArea.getWidth());
        sz.mHeight = N_ToPixel(sz.mHeight + wndArea.getHeight() - textArea.getHeight());
        return sz;
    }
    //------------------------------------------------------------------------
}
}