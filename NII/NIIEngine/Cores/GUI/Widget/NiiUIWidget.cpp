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
#include "NiiUIWidget.h"
#include "NiiUICursor.h"
#include "NiiUIWindow.h"
#include "NiiUIException.h"
#include "NiiUITooltip.h"
#include "NiiUIManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIStyleWidget.h"
#include "NiiUIPixel.h"
#include "NiiGBufferManager.h"
#include "NiiString.h"
#include "NiiPropertyObj.h"
#include "NiiFontManager.h"
#include "NiiLogManager.h"
#include "NiiPixelManager.h"
#include "NiiUIScrollbar.h"
#include "NiiUIStrConv.h"
#include "NiiUICursorEventArgs.h"
#include "NiiFont.h"

#if defined (USE_FRIBIDI)
    #include "NiiUIFribidiText.h"
#elif defined (USE_MINIBIDI)
    #include "NiiUIMinibidiText.h"
#else
    #include "NiiUIBidiText.h"
#endif

#define inner_rect 1
using namespace NII::UI;

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // WidgetView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(NII, WidgetView, NormalState,       0);
    N_STATE(NII, WidgetView, DisableState,      1);
    N_STATE(NII, WidgetView, HoverState,        2);
    N_STATE(NII, WidgetView, ActiveState,       3);
    N_STATE(NII, WidgetView, StateCount,        4);
    //------------------------------------------------------------------------
    WidgetView::WidgetView(WidgetViewlID wsid, WidgetViewlID type) :
        mSrc(0),
        mID(wsid),
        mType(type)
    {
    }
    //------------------------------------------------------------------------
    WidgetView::~WidgetView()
    {
    }
    //------------------------------------------------------------------------
    WidgetView & WidgetView::operator=(const WidgetView &)
    {
        return *this;
    }
    //------------------------------------------------------------------------
    Style * WidgetView::getStyle() const
    {
        return N_Only(Widget).getStyle(mSrc->getStyle());
    }
    //------------------------------------------------------------------------
    void WidgetView::update(TimeDurMS cost)
    {
        (void)cost;
    }
    //------------------------------------------------------------------------
    Rectf WidgetView::getArea() const
    {
        const Style * style = N_Only(Widget).getStyle(mSrc->getStyle());

        if (style->isSpecialExist(inner_rect))
            return style->getSpecial(inner_rect).getArea().getPixelRect(*mSrc, mSrc->getArea().getIntCache());
        else
            return mSrc->getArea().getIntCache();
    }
    //------------------------------------------------------------------------
    void WidgetView::layout()
    {
    }
    //------------------------------------------------------------------------
    void WidgetView::add(PropertyCmd * obj, bool serl)
    {
        mCmdList.push_back(Npair(obj, serl));
    }
    //------------------------------------------------------------------------
    void WidgetView::onAttach()
    {
        PropertyCmdList::iterator i = mCmdList.begin();
        while (i != mCmdList.end())
        {
            mSrc->getProperty()->add(i->first);

            if (!i->second)
                mSrc->setDepthWrite(i->first->getID(), false);

            ++i;
        }
    }
    //------------------------------------------------------------------------
    void WidgetView::onDetach()
    {
        PropertyCmdList::reverse_iterator i, iend = mCmdList.rend();
        for (i = mCmdList.rbegin(); i != iend; ++i)
        {
            if (!i->second)
                mSrc->setDepthWrite(i->first->getID(), true);

            mSrc->getProperty()->remove(i->first->getID());
        }
    }
    //------------------------------------------------------------------------
    void WidgetView::onStyleAttach()
    {
    }
    //------------------------------------------------------------------------
    void WidgetView::onStyleDetach()
    {
    }
    //------------------------------------------------------------------------
    void WidgetView::getContext(SheetContext & ctx) const
    {
        mSrc->getContextImpl(ctx);
    }
    //------------------------------------------------------------------------
    bool WidgetView::notifyFont(const Font * obj)
    {
        return N_Only(Widget).getStyle(mSrc->getStyle())->notifyFont(mSrc, obj);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DefaultView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DefaultView::DefaultView(WidgetViewlID wsid) :
        WidgetView(wsid, N_VIEW_Default)
    {
    }
    //--------------------------------------------------------------------------
    void DefaultView::flush()
    {
        const Style * style = getStyle();
        style->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState).render(mSrc);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StaticView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdFrameEnabled : public PropertyCmd
    {
    public:
        CmdFrameEnabled() : PropertyCmd(N_PropertyUIStatic_FrameEnable, _T("FrameEnabled"),
            _T("Property to get/set the state of the frame enabled setting for the StaticView widget."
                "  Value is either \"True\" or \"False\".")) {}

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const StaticView * obj = static_cast<StaticView *>(static_cast<const Widget *>(own)->getWidgetView());

            StrConv::conv(obj->isFrame(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StaticView * obj = static_cast<StaticView *>(static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setFrame(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            const StaticView * obj = static_cast<StaticView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isFrame();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            StaticView * obj = static_cast<StaticView *>(static_cast<Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setFrame(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFrameEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdBackgroundEnabled : public PropertyCmd
    {
    public:
        CmdBackgroundEnabled() : PropertyCmd(N_PropertyUIStatic_BgEnable, _T("BackgroundEnabled"),
            _T("Property to get/set the state of the frame background setting for the StaticView widget."
                "  Value is either \"True\" or \"False\"."))
        {}

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const StaticView * obj = static_cast<StaticView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->isBackground(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StaticView * obj = static_cast<StaticView *>(static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setBackground(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            const StaticView * obj = static_cast<StaticView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isBackground();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            StaticView * obj = static_cast<StaticView *>(static_cast<Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setBackground(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBackgroundEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    N_STATE(WidgetView, StaticView, WithFrameState, 0);
    N_STATE(WidgetView, StaticView, NoFrameState, 1);
    N_STATE(WidgetView, StaticView, WithFrameAndBgState, 2);
    N_STATE(WidgetView, StaticView, WithFrameNoBgState, 3);
    N_STATE(WidgetView, StaticView, WithBgNoFrameState, 4);
    N_STATE(WidgetView, StaticView, NoFrameAndBgState, 5);
    N_STATE(WidgetView, StaticView, StateCount, 6);
    //------------------------------------------------------------------------
    StaticView::StaticView(WidgetViewlID wsid, WidgetViewlID type) :
        WidgetView(wsid, type),
        mFrame(false),
        mBackground(false)
    {
        add(N_new CmdFrameEnabled());
        add(N_new CmdBackgroundEnabled());
    }
    //------------------------------------------------------------------------
    void StaticView::setFrame(bool set)
    {
        if (mFrame != set)
        {
            mFrame = set;
            mSrc->refresh();
        }
    }
    //------------------------------------------------------------------------
    void StaticView::setBackground(bool set)
    {
        if (mBackground != set)
        {
            mBackground = set;
            mSrc->refresh();
        }
    }
    //------------------------------------------------------------------------
    void StaticView::flush()
    {
        const Style * style = getStyle();

        bool lastFrame = !mSrc->isFinalDisable();

        if (mFrame)
        {
            style->getLayout(lastFrame ? WithFrameState : NoFrameState).render(mSrc);
        }

        // render background section
        if (mBackground)
        {
            const StyleLayout * mode;
            if (mFrame)
            {
                mode = &style->getLayout(lastFrame ? WithFrameAndBgState : WithFrameNoBgState);
            }
            else
            {
                mode = &style->getLayout(lastFrame ? WithBgNoFrameState : NoFrameAndBgState);
            }
            // peform the rendering operation.
            mode->render(mSrc);
        }

        // render basic mode
        style->getLayout(lastFrame ? NormalState : DisableState).render(mSrc);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //StaticImageView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdHorzFormatting : public PropertyCmd
    {
    public:
        CmdHorzFormatting() : PropertyCmd(N_PropertyUIText_HLayout, _T("HorzFormatting"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            HTextLayout temp = obj->getHLayout();
            UI::StrConv::conv(temp, out);

        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<Widget *>(own)->getWidgetView());
            HTextLayout temp;
            UI::StrConv::conv(val, temp);
            obj->setHLayout(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            HTextLayout * temp = (HTextLayout *)out;
            *temp = obj->getHLayout();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<Widget *>(own)->getWidgetView());
            HTextLayout * temp = (HTextLayout *)in;
            obj->setHLayout(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzFormatting();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("left");
        }
    };

    class _EngineInner CmdVertFormatting : public PropertyCmd
    {
    public:
        CmdVertFormatting() : PropertyCmd(N_PropertyUIText_VLayout, _T("VertFormatting"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            UI::StrConv::conv(obj->getVLayout(), out);

        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<Widget *>(own)->getWidgetView());
            VTextLayout temp;
            UI::StrConv::conv(val, temp);
            obj->setVLayout(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            VTextLayout * temp = (VTextLayout *)out;
            *temp = obj->getVLayout();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            VTextLayout * temp = (VTextLayout *)in;
            obj->setVLayout(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertFormatting();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("centre");
        }
    };

    class _EngineInner CmdVertScrollbar : public PropertyCmd
    {
    public:
        CmdVertScrollbar() : PropertyCmd(N_PropertyUIText_VScrollbarEnable, _T("VertScrollbar"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->isVScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setVScrollbar(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdHorzScrollbar : public PropertyCmd
    {
    public:
        CmdHorzScrollbar() : PropertyCmd(N_PropertyUIText_HScrollbarEnable, _T("HorzScrollbar"), _T(""))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->isHScrollbar(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setHScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isHScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setHScrollbar(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdHorzExtent : public PropertyCmd
    {
    public:
        CmdHorzExtent() : PropertyCmd(N_PropertyUIText_HTextExtent, _T("HorzExtent"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->getHTextExtent(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            (void)own;
            (void)val;
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            NIIf * temp = (NIIf *)out;
            *temp = obj->getHTextExtent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            (void)own;
            (void)in;
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzExtent();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.0f, out);
        }

        /// @copydetails PropertyCmd::isWritable
        bool isWritable() const
        {
            return false;
        }

        /// @copydetails PropertyCmd::isSerialize
        bool isSerialize() const
        {
            return false;
        }
    };

    class _EngineInner CmdVertExtent : public PropertyCmd
    {
    public:
        CmdVertExtent() : PropertyCmd(N_PropertyUIText_VTextExtent, _T("VertExtent"), _T(""))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->getVTextExtent(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            (void)own;
            (void)val;
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            StaticTextView * obj = static_cast<StaticTextView *>(static_cast<const Widget *>(own)->getWidgetView());
            NIIf * temp = (NIIf *)out;
            *temp = obj->getVTextExtent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            (void)own;
            (void)in;
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertExtent();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.0f, out);
        }

        /// @copydetails PropertyCmd::isSerialize
        bool isSerialize() const
        {
            return false;
        }

        /// @copydetails PropertyCmd::isWritable
        bool isWritable() const
        {
            return false;
        }
    };
    //------------------------------------------------------------------------
    N_STATE(StaticView, StaticTextView, FrameAreaState, 0);
    N_STATE(StaticView, StaticTextView, FrameHScrollAreaState, 1);
    N_STATE(StaticView, StaticTextView, FrameVScrollAreaState, 2);
    N_STATE(StaticView, StaticTextView, FrameHVScrollAreaState, 3);
    N_STATE(StaticView, StaticTextView, AreaState, 4);
    N_STATE(StaticView, StaticTextView, HScrollAreaState, 5);
    N_STATE(StaticView, StaticTextView, VScrollAreaState, 6);
    N_STATE(StaticView, StaticTextView, HVScrollAreaState, 7);
    N_STATE(StaticView, StaticTextView, StateCount, 8);
    //------------------------------------------------------------------------
    const WidgetID StaticTextView::VScrollbarID(1); //"__auto_vscrollbar__"
    const WidgetID StaticTextView::HScrollbarID(2); //"__auto_hscrollbar__"
    //------------------------------------------------------------------------
    StaticTextView::StaticTextView(WidgetViewlID wsid) :
        StaticView(wsid, N_VIEW_StaticText),
        mHLayout(HTL_Left),
        mVLayout(VTL_Centre),
        mFormatText(0),
        mVScrollbar(false),
        mHScrollbar(false),
        mFormatValid(false)
    {
        add(N_new CmdHorzFormatting());
        add(N_new CmdVertFormatting());
        add(N_new CmdVertScrollbar());
        add(N_new CmdHorzScrollbar());
        add(N_new CmdHorzExtent());
        add(N_new CmdVertExtent());
    }
    //------------------------------------------------------------------------
    StaticTextView::~StaticTextView()
    {
        if (mFormatText)
            N_delete mFormatText;
    }
    //------------------------------------------------------------------------
    void StaticTextView::flush()
    {
        StaticView::flush();

        // get destination area for the text.
        const Rectf clipper(getRenderArea());
        Rectf absarea(clipper);

        if (!mFormatValid)
            updateFormatText(clipper.getSize());

        // see if we may need to adjust horizontal position
        const Scrollbar * horzScrollbar = getHScrollbar();
        if (horzScrollbar->isFinalVisible())
        {
            NIIf range = horzScrollbar->getDocSize() - horzScrollbar->getPageSize();
            switch (mHLayout)
            {
            case HTL_Left:
            case HTL_Wrap_Left:
            case HTL_Normal:
            case HTL_Wrap_Expand:
                absarea -= Vector2f(horzScrollbar->getCurrent(), 0);
                break;

            case HTL_Centre:
            case HTL_Wrap_Centre:
                absarea.setWidth(horzScrollbar->getDocSize());
                absarea += Vector2f(range / 2 - horzScrollbar->getCurrent(), 0);
                break;

            case HTL_Right:
            case HTL_Wrap_Right:
                absarea += Vector2f(range - horzScrollbar->getCurrent(), 0);
                break;
            }
        }

        // adjust y positioning according to formatting option
        NIIf textHeight = mFormatText->getVExtent(mSrc);
        const Scrollbar * const vertScrollbar = getVScrollbar();
        const NIIf vertScrollPosition = vertScrollbar->getCurrent();
        // if scroll bar is in use, position according to that.
        if (vertScrollbar->isFinalVisible())
            absarea.mTop -= vertScrollPosition;
        // no scrollbar, so adjust position according to formatting set.
        else
        {
            switch (mVLayout)
            {
            case VTL_Centre:
                absarea.mTop += N_ToPixel((absarea.height() - textHeight) * 0.5f);
                break;
            case VTL_Bottom:
                absarea.mTop = absarea.mBottom - textHeight;
                break;
            }
        }
        // calculate final colours
        ColourArea final_cols = mSrc->getTextColour();
        final_cols.setAlphaFactor(mSrc->getAlphaEffect());
        // cache the text for rendering.
        mFormatText->draw(mSrc, *mSrc->getPixelData(), absarea.getPos(), &final_cols, &clipper);
    }
    //------------------------------------------------------------------------
    Scrollbar * StaticTextView::getVScrollbar() const
    {
        return static_cast<Scrollbar *>(static_cast<Container *>(mSrc)->get((mID << N_Level_Bit_Escape) + VScrollbarID));
    }
    //------------------------------------------------------------------------
    Scrollbar * StaticTextView::getHScrollbar() const
    {
        return static_cast<Scrollbar *>(static_cast<Container *>(mSrc)->get((mID << N_Level_Bit_Escape) + HScrollbarID));
    }
    //------------------------------------------------------------------------
    Rectf StaticTextView::getRenderArea() const
    {
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();
        bool v_visible = vertScrollbar->isVisible();
        bool h_visible = horzScrollbar->isVisible();

        // get Style for the assigned look.
        const Style * style = getStyle();
        Nid sid;
        if (mFrame)
        {
            if (v_visible || h_visible)
            {
                if (h_visible)
                {
                    if (v_visible)
                    {
                        sid = FrameHVScrollAreaState;
                    }
                    else
                    {
                        sid = FrameHScrollAreaState;
                    }
                }
                else
                {
                    if (v_visible)
                    {
                        sid = FrameVScrollAreaState;
                    }
                    else
                    {
                        sid = FrameAreaState;
                    }
                }
            }
            else
            {
                sid = FrameAreaState;
            }
        }
        else
        {
            if (v_visible || h_visible)
            {
                if (h_visible)
                {
                    if (v_visible)
                    {
                        sid = HVScrollAreaState;
                    }
                    else
                    {
                        sid = HScrollAreaState;
                    }
                }
                else
                {
                    if (v_visible)
                    {
                        sid = VScrollAreaState;
                    }
                    else
                    {
                        sid = AreaState;
                    }
                }
            }
            else
            {
                sid = AreaState;
            }
        }

        if (style->isSpecialExist(sid))
        {
            return style->getSpecial(sid).getArea().getPixelRect(*mSrc);
        }

        return style->getSpecial(FrameAreaState).getArea().getPixelRect(*mSrc);
    }
    //------------------------------------------------------------------------
    PlaneSizef StaticTextView::getDocumentSize(const Rectf & renderArea) const
    {
        if (!mFormatValid)
            updateFormatText(renderArea.getSize());

        return PlaneSizef(mFormatText->getHExtent(mSrc),
            mFormatText->getVExtent(mSrc));
    }
    //------------------------------------------------------------------------
    void StaticTextView::setVLayout(VTextLayout mode)
    {
        if (mode != mVLayout)
        {
            mVLayout = mode;
            updateScrollbar();
            mSrc->refresh();
        }
    }
    //------------------------------------------------------------------------
    void StaticTextView::setHLayout(HTextLayout mode)
    {
        if (mode != mHLayout)
        {
            mHLayout = mode;
            if (mFormatText)
            {
                N_delete mFormatText;
                mFormatText = 0;
            }
            mFormatValid = false;

            switch (mHLayout)
            {
            case HTL_Left:
                mFormatText = N_new LeftLayout(mSrc->getRenderText());
                break;
            case HTL_Right:
                mFormatText = N_new RightLayout(mSrc->getRenderText());
                break;
            case HTL_Centre:
                mFormatText = N_new CenterLayout(mSrc->getRenderText());
                break;
            case HTL_Normal:
                mFormatText = N_new JustifyLayout(mSrc->getRenderText());
                break;
            case HTL_Wrap_Left:
                mFormatText = N_new WrapLayout<LeftLayout>(mSrc->getRenderText());
                break;
            case HTL_Wrap_Right:
                mFormatText = N_new WrapLayout<RightLayout>(mSrc->getRenderText());
                break;
            case HTL_Wrap_Centre:
                mFormatText = N_new WrapLayout<CenterLayout>(mSrc->getRenderText());
                break;
            case HTL_Wrap_Expand:
                mFormatText = N_new WrapLayout<JustifyLayout>(mSrc->getRenderText());
                break;
            }
            updateScrollbar();
            mSrc->refresh();
        }
    }
    //------------------------------------------------------------------------
    void StaticTextView::setVScrollbar(bool set)
    {
        mVScrollbar = set;
        updateScrollbar();
        mSrc->layout();
        mFormatValid = false;
        mSrc->refresh();
    }
    //------------------------------------------------------------------------
    void StaticTextView::setHScrollbar(bool set)
    {
        mHScrollbar = set;
        updateScrollbar();
        mSrc->layout();
        mFormatValid = false;
        mSrc->refresh();
    }
    //------------------------------------------------------------------------
    void StaticTextView::updateScrollbar(void)
    {
        // get the scrollbars
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        // get the sizes we need
        Rectf renderArea(getRenderArea());
        PlaneSizef renderAreaSize(renderArea.getSize());
        PlaneSizef documentSize(getDocumentSize(renderArea));

        // show or hide vertical scroll bar as required (or as specified by option)
        bool showVert = ((documentSize.mHeight > renderAreaSize.mHeight) && mVScrollbar);
        bool showHorz = ((documentSize.mWidth > renderAreaSize.mWidth) && mHScrollbar);

        vertScrollbar->setVisible(showVert);
        horzScrollbar->setVisible(showHorz);

        // if scrollbar visibility just changed we have might have a better TextRenderArea
        // if so we go with that instead
        const Rectf updatedRenderArea = getRenderArea();
        if (renderArea != updatedRenderArea)
        {
            mFormatValid = false;
            renderArea = updatedRenderArea;
            renderAreaSize = renderArea.getSize();
            documentSize = getDocumentSize(renderArea);
        }

        // Set up scroll bar values
        vertScrollbar->setDocSize(documentSize.mHeight);
        vertScrollbar->setPageSize(renderAreaSize.mHeight);
        vertScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderAreaSize.mHeight / 10.0f));

        horzScrollbar->setDocSize(documentSize.mWidth);
        horzScrollbar->setPageSize(renderAreaSize.mWidth);
        horzScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderAreaSize.mWidth / 10.0f));
    }
    //------------------------------------------------------------------------
    /*void StaticTextView::FontChangeMFunc(const EventArgs * arg)
    {
        onFont(static_cast<const FontEventArgs *>(arg));
    }*/
    //------------------------------------------------------------------------
    void StaticTextView::onText(const EventArgs *)
    {
        mFormatValid = false;
        updateScrollbar();
        mSrc->refresh();
    }
    //------------------------------------------------------------------------
    void StaticTextView::onSize(const EventArgs *)
    {
        mFormatValid = false;
        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void StaticTextView::onFont(const FontEventArgs *)
    {
        mFormatValid = false;
        updateScrollbar();
        mSrc->refresh();
    }
    //------------------------------------------------------------------------
    void StaticTextView::onCursorDrag(const EventArgs * event)
    {
        const CursorEventArgs * e = static_cast<const CursorEventArgs *>(event);

        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        const bool vertScrollbarVisible = vertScrollbar->isFinalVisible();
        const bool horzScrollbarVisible = horzScrollbar->isFinalVisible();

        if (vertScrollbarVisible && (vertScrollbar->getDocSize() > vertScrollbar->getPageSize()))
        {
            vertScrollbar->setCurrent(vertScrollbar->getCurrent() + vertScrollbar->getStepSize() * -e->mMouseArg.mScroll);
        }
        else if (horzScrollbarVisible && (horzScrollbar->getDocSize() > horzScrollbar->getPageSize()))
        {
            horzScrollbar->setCurrent(horzScrollbar->getCurrent() + horzScrollbar->getStepSize() * -e->mMouseArg.mScroll);
        }
    }
    //------------------------------------------------------------------------
    void StaticTextView::onScrollbarChange(const EventArgs *)
    {
        mSrc->refresh();
    }
    //------------------------------------------------------------------------
    void StaticTextView::onStyleAttach()
    {
        // do initial scrollbar setup
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        vertScrollbar->hide();
        horzScrollbar->hide();

        mSrc->layout();

        // scrollbar events
        vertScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&StaticTextView::onScrollbarChange);
        horzScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&StaticTextView::onScrollbarChange);

        // events that scrollbars should react to
        mSignalList.push_back(mSrc->bind(Widget::TextEvent, this, (CommandObjFunctor)&StaticTextView::onText));

        mSignalList.push_back(mSrc->bind(Widget::SizeEvent, this, (CommandObjFunctor)&StaticTextView::onSize));

        mSignalList.push_back(mSrc->bind(Widget::FontEvent, this, &StaticTextView::onFont));

        mSignalList.push_back(mSrc->bind(Widget::CursorDragEvent, this, (CommandObjFunctor)&StaticTextView::onCursorDrag));
    }
    //------------------------------------------------------------------------
    void StaticTextView::onStyleDetach()
    {
        // clean up connections that rely on widgets created by the look and feel
        SignalPtrs::iterator i = mSignalList.begin();
        while (i != mSignalList.end())
        {
            (*i)->disconnect();
            ++i;
        }
        mSignalList.clear();
    }
    //------------------------------------------------------------------------
    NIIf StaticTextView::getHTextExtent() const
    {
        if (!mFormatValid)
            updateFormatText(getRenderArea().getSize());

        return mFormatText ? mFormatText->getHExtent(mSrc) : 0.0f;
    }
    //------------------------------------------------------------------------
    NIIf StaticTextView::getVTextExtent() const
    {
        if (!mFormatValid)
            updateFormatText(getRenderArea().getSize());

        return mFormatText ? mFormatText->getVExtent(mSrc) : 0.0f;
    }
    //------------------------------------------------------------------------
    void StaticTextView::updateFormatText(const PlaneSizef & sz) const
    {
        if (!mSrc)
            return;

        if (!mFormatText)
        {
            if (mFormatText)
            {
                N_delete mFormatText;
                mFormatText = 0;
            }
            mFormatValid = false;

            switch (mHLayout)
            {
            case HTL_Left:
                mFormatText = N_new LeftLayout(mSrc->getRenderText());
                break;
            case HTL_Right:
                mFormatText = N_new RightLayout(mSrc->getRenderText());
                break;
            case HTL_Centre:
                mFormatText = N_new CenterLayout(mSrc->getRenderText());
                break;
            case HTL_Normal:
                mFormatText = N_new JustifyLayout(mSrc->getRenderText());
                break;
            case HTL_Wrap_Left:
                mFormatText = N_new WrapLayout<LeftLayout>(mSrc->getRenderText());
                break;
            case HTL_Wrap_Right:
                mFormatText = N_new WrapLayout<RightLayout>(mSrc->getRenderText());
                break;
            case HTL_Wrap_Centre:
                mFormatText = N_new WrapLayout<CenterLayout>(mSrc->getRenderText());
                break;
            case HTL_Wrap_Expand:
                mFormatText = N_new WrapLayout<JustifyLayout>(mSrc->getRenderText());
                break;
            }
        }

        // 'ref' the window's rendered string to ensure it's re-parsed if needed.
        mSrc->getRenderText();

        mFormatText->layout(mSrc, sz);
        mFormatValid = true;
    }
    //------------------------------------------------------------------------
    bool StaticTextView::notifyFont(const Font * font)
    {
        bool res = WidgetView::notifyFont(font);

        if (mSrc->getFont() == font)
        {
            mSrc->refresh();
            mFormatValid = false;
            return true;
        }

        return res;
    }
    //------------------------------------------------------------------------
    /*    States:
            - EnabledFrame                - frame rendering for enabled state
            - DisabledFrame               - frame rendering for disabled state.
            - WithFrameEnabledBackground  - backdrop rendering for enabled state with frame enabled.
            - WithFrameDisabledBackground - backdrop rendering for disabled state with frame enabled.
            - NoFrameEnabledBackground    - backdrop rendering for enabled state with frame disabled.
            - NoFrameDisabledBackground   - backdrop rendering for disabled state with frame disabled.
            - WithFrameImage              - image rendering when frame is enabled
            - NoFrameImage                - image rendering when frame is disabled (defaults to WithFrameImage if not present)
            */
    //------------------------------------------------------------------------
    StaticImageView::StaticImageView(WidgetViewlID wsid) :
        StaticView(wsid, N_VIEW_StaticImage)
    {
    }
    //------------------------------------------------------------------------
    void StaticImageView::flush()
    {
        StaticView::flush();
        if (mSrc->getBGImage() != 0)
        {
            const Style * style = getStyle();
            StateID mode = (!mFrame && style->isStateExist(NoFrameState)) ? NoFrameState : WithFrameState;
            style->getLayout(mode).render(mSrc);
        }
    }
    //------------------------------------------------------------------------
    class _EngineInner CmdArea : public PropertyCmd
    {
    public:
        CmdArea() : PropertyCmd(N_PropertyUIWidget_RelArea, _T("area"), _T("UI所占区域"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelRectf & temp = static_cast<const Widget *>(own)->getRelArea();

            Ntchar buff[512];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g %g %g %g %g"),
                temp.mLeft.mScale, temp.mLeft.mOffset,
                temp.mTop.mScale, temp.mTop.mOffset,
                temp.mRight.mScale, temp.mRight.mOffset,
                temp.mBottom.mScale, temp.mBottom.mOffset);
            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Widget * obj = static_cast<Widget *>(own);

            RelRectf temp;
            Nsscanf(val.c_str(),_T("%g %g %g %g %g %g %g %g"),
                &temp.mLeft.mScale, &temp.mLeft.mOffset,
                &temp.mTop.mScale, &temp.mTop.mOffset,
                &temp.mRight.mScale, &temp.mRight.mOffset,
                &temp.mBottom.mScale, &temp.mBottom.mOffset);

            obj->setRelArea(RelRectf(temp.mLeft, temp.mTop, temp.getSize()));

        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelRectf * temp = (RelRectf *)out;
            *temp = static_cast<const Widget *>(own)->getRelArea();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelRectf * temp = (RelRectf *)in;
            static_cast<Widget *>(own)->setRelArea(RelRectf(temp->mLeft, temp->mTop, temp->getSize()));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdArea();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");
        }
    };

    class _EngineInner CmdPosition : public PropertyCmd
    {
    public:
        CmdPosition() : PropertyCmd(N_PropertyUIWidget_Pos, _T("position"), _T("UI位置/左上角"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelVector2f & temp = static_cast<const Widget *>(own)->getPos();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"),
                temp.x.mScale, temp.x.mOffset, temp.y.mScale, temp.y.mOffset);
            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            RelVector2f temp;
            Nsscanf(val.c_str(), _T("%g %g %g %g"),
                &temp.x.mScale, &temp.x.mOffset, &temp.y.mScale, &temp.y.mOffset);
            static_cast<Widget *>(own)->setPos(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelVector2f * temp = (RelVector2f *)out;
            *temp = static_cast<const Widget *>(own)->getPos();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelVector2f * temp = (RelVector2f *)in;
            static_cast<Widget *>(own)->setPos(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPosition();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
        /// @copydetails PropertyCmd::isSerialize
        bool isSerialize() const
        {
            return false;
        }
    };

    class _EngineInner CmdYAlign : public PropertyCmd
    {
    public:
        CmdYAlign() : PropertyCmd(N_PropertyUIWidget_VAlign, _T("y_align"), _T("水平方向/y方向对齐方式"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            VAlign temp = static_cast<const Widget *>(own)->getYAlign();

            if(temp == VA_CENTRE)
            {
                out = _T("Centre");
            }
            else if(temp == VA_BOTTOM)
            {
                out = _T("Bottom");
            }
            else if(temp == VA_TOP)
            {
                out = _T("Top");
            }
            else
                out = _T("Centre");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            VAlign temp;

            if(val == _T("Centre"))
            {
                temp = VA_CENTRE;
            }
            else if (val == _T("Bottom"))
            {
                temp = VA_BOTTOM;
            }
            else
            {
                temp = VA_TOP;
            }
            static_cast<Widget *>(own)->setYAlign(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            VAlign * temp = (VAlign *)out;
            *temp = static_cast<const Widget *>(own)->getYAlign();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            VAlign * temp = (VAlign *)in;
            static_cast<Widget *>(own)->setYAlign(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdYAlign();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("Top");
        }
    };

    class _EngineInner CmdXAlign : public PropertyCmd
    {
    public:
        CmdXAlign() : PropertyCmd(N_PropertyUIWidget_HAlign, _T("x_align"), _T("水平方向/x方向对齐方式"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            HAlign temp = static_cast<const Widget *>(own)->getXAlign();
            if(temp == HA_CENTRE)
            {
                out = _T("Centre");
            }
            else if(temp == HA_RIGHT)
            {
                out = _T("Right");
            }
            else if(temp == HA_LEFT)
            {
                out = _T("Left");
            }
            else
                out = _T("Centre");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            HAlign temp;
            if(val == _T("Centre"))
            {
                temp = HA_CENTRE;
            }
            else if(val == _T("Right"))
            {
                temp = HA_RIGHT;
            }
            else
            {
                temp = HA_LEFT;
            }
            static_cast<Widget *>(own)->setXAlign(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            HAlign * temp = (HAlign *)out;
            *temp = static_cast<const Widget *>(own)->getXAlign();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            HAlign * temp = (HAlign *)in;
            static_cast<Widget *>(own)->setXAlign(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdXAlign();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("Left");
        }
    };

    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //StaticImageView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdPixelBuffer : public PropertyCmd
    {
    public:
        CmdPixelBuffer() : PropertyCmd(N_PropertyUIWidget_Image, _T("PixelBuffer"),
            _T("Property to get/set the image for the StaticImageView widget."
                "  Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            PixelBuffer * temp = static_cast<const Widget *>(own)->getBGImage();
            out = temp ? N_conv(temp->getGroupID()) + _T("|") + N_conv(temp->getID()) : _T("0|0");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);
            static_cast<Widget *>(own)->setBGImage(N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            PixelBuffer * pb = static_cast<const Widget *>(own)->getBGImage();
            Nui64 * temp = (Nui64 *)out;
            *temp = pb ? ((Nui64)pb->getGroupID() << 32) | pb->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<Widget *>(own)->setBGImage(N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPixelBuffer();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };

    class _EngineInner CmdSize : public PropertyCmd
    {
    public:
        CmdSize() : PropertyCmd(N_PropertyUIWidget_Size, _T("size"), _T("UI尺寸"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            RelPlaneSizef temp = static_cast<const Widget *>(own)->getSize();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"), temp.mWidth.mScale, temp.mWidth.mOffset,
                temp.mHeight.mScale, temp.mHeight.mOffset);
            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            RelPlaneSizef temp;
            Nsscanf(val.c_str(), _T("%g %g %g %g"), &temp.mWidth.mScale, &temp.mWidth.mOffset,
                &temp.mHeight.mScale, &temp.mHeight.mOffset);
            static_cast<Widget *>(own)->setSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelPlaneSizef * temp = (RelPlaneSizef *)out;
            *temp = static_cast<const Widget *>(own)->getSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelPlaneSizef * temp = (RelPlaneSizef *)in;
            static_cast<Widget *>(own)->setSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
        /// @copydetails PropertyCmd::isSerialize
        bool isSerialize() const
        {
            return false;
        }
    };

    class _EngineInner CmdMinSize : public PropertyCmd
    {
    public:
        CmdMinSize() : PropertyCmd(N_PropertyUIWidget_MinSize, _T("min_size"), _I18n("UI最小尺寸"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelPlaneSizef & temp = static_cast<const Widget *>(own)->getMinSize();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"), temp.mWidth.mScale, temp.mWidth.mOffset,
                temp.mHeight.mScale, temp.mHeight.mOffset);
            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            RelPlaneSizef temp;
            Nsscanf(val.c_str(), _T("%g %g %g %g"), &temp.mWidth.mScale, &temp.mWidth.mOffset,
                &temp.mHeight.mScale, &temp.mHeight.mOffset);
            static_cast<Widget *>(own)->setMinSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelPlaneSizef * temp = (RelPlaneSizef *)out;
            *temp = static_cast<const Widget *>(own)->getMinSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelPlaneSizef * temp = (RelPlaneSizef *)in;
            static_cast<Widget *>(own)->setMinSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMinSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
    };

    class _EngineInner CmdMaxSize : public PropertyCmd
    {
    public:
        CmdMaxSize() : PropertyCmd(N_PropertyUIWidget_MaxSize, _T("max_size"), _T("UI最大尺寸"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelPlaneSizef & temp = static_cast<const Widget *>(own)->getMaxSize();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"), temp.mWidth.mScale, temp.mWidth.mOffset,
                temp.mHeight.mScale, temp.mHeight.mOffset);

            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            RelPlaneSizef temp;
            Nsscanf(val.c_str(), _T("%g %g %g %g"), &temp.mWidth.mScale, &temp.mWidth.mOffset,
                &temp.mHeight.mScale, &temp.mHeight.mOffset);

            static_cast<Widget *>(own)->setMaxSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelPlaneSizef * temp = (RelPlaneSizef *)out;
            *temp = static_cast<const Widget *>(own)->getMaxSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelPlaneSizef * temp = (RelPlaneSizef *)in;
            static_cast<Widget *>(own)->setMaxSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
    };

    class _EngineInner CmdScaleMode : public PropertyCmd
    {
    public:
        CmdScaleMode() : PropertyCmd(N_PropertyUIWidget_ScaleMode, _T("scale_mode"), _T("高宽缩放模式"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            AspectMode temp = static_cast<const Widget *>(own)->getScaleMode();

            if(temp == AM_None)
            {
                out = _T("Ignore");
            }
            else if(temp == AM_Shrink)
            {
                out = _T("Shrink");
            }
            else if(temp == AM_Expand)
            {
                out = _T("Expand");
            }
            else
                out = _T("Ignore");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            AspectMode temp;

            if(val == _T("Shrink"))
            {
                temp = AM_Shrink;
            }
            else if(val == _T("Expand"))
            {
                temp = AM_Expand;
            }
            else
            {
                temp = AM_None;
            }
            static_cast<Widget *>(own)->setScaleMode(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            AspectMode * temp = (AspectMode *)out;
            *temp = static_cast<const Widget *>(own)->getScaleMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            AspectMode * temp = (AspectMode *)in;
            static_cast<Widget *>(own)->setScaleMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdScaleMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("Ignore");
        }
    };

    class _EngineInner CmdScaleFactor : public PropertyCmd
    {
    public:
        CmdScaleFactor() : PropertyCmd(N_PropertyUIWidget_ScaleFactor, _T("scale_factor"), _I18n("高宽比"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->getScaleFactor(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setScaleFactor(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const Widget *>(own)->getScaleFactor();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<Widget *>(own)->setScaleFactor(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdScaleFactor();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("1.0");
        }
    };

    class _EngineInner CmdPixelAligne : public PropertyCmd
    {
    public:
        CmdPixelAligne() : PropertyCmd(N_PropertyUIWidget_PixelAlign, _T("pixel_aligne"), _I18n("像素对齐,位置取整操作"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->getPixelAlign(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setPixelAlign(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->getPixelAlign();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setPixelAlign(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdPixelAligne();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdName : public PropertyCmd
    {
    public:
        CmdName() : PropertyCmd(N_PropertyUIWidget_Name, _T("name"), _I18n("命名"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const Widget *>(own)->getName();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<Widget *>(own)->setName(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const Widget *>(own)->getName();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            String * temp = (String *)in;
            static_cast<Widget *>(own)->setName(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdName();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };

    class _EngineInner CmdNamePath : public PropertyCmd
    {
    public:
        CmdNamePath() : PropertyCmd(N_PropertyUIWidget_NamePath, _T("name_path"), _I18n("命名路径"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const Widget *>(own)->getNamePath();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            (void)own;
            (void)val;
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const Widget *>(own)->getNamePath();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            (void)own;
            (void)in;
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNamePath();
        }
        /// @copydetails PropertyCmd::isWirtable
        bool isWirtable() const
        {
            return false;
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };

    class _EngineInner CmdAlpha : public PropertyCmd
    {
    public:
        CmdAlpha() : PropertyCmd(N_PropertyUIWidget_Alpha, _T("alpha"), _I18n("透明度"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->getAlpha(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setAlpha(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->getAlpha();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setAlpha(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAlpha();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("1.0");
        }
    };

    class _EngineInner CmdAutoClip : public PropertyCmd
    {
    public:
        CmdAutoClip() : PropertyCmd(N_PropertyUIWidget_AutoClip, _T("auto_clip"), _I18n("自动裁剪,和所在区域有关"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isAutoClip(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setAutoClip(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isAutoClip();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setAutoClip(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoClip();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdAutoDestroy : public PropertyCmd
    {
    public:
        CmdAutoDestroy() : PropertyCmd(N_PropertyUIWidget_AutoDestroy, _T("auto_destroy"), _I18n("自动回收所占用的内存"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isAutoDestroy(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setAutoDestroy(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isAutoDestroy();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setAutoDestroy(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoDestroy();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdID : public PropertyCmd
    {
    public:
        CmdID() : PropertyCmd(N_PropertyUIWidget_ID, _T("id"), _I18n("UI单元的唯一识别码,系统内部使用"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->getID(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setID(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Widget *>(own)->getID();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Widget *>(own)->setID(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdID();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };

    class _EngineInner CmdEnvAlpha : public PropertyCmd
    {
    public:
        CmdEnvAlpha() : PropertyCmd(N_PropertyUIWidget_EnvAlpha, _T("evn_alpha"), _I18n("环境透明度,和它所在的区域原透明度有关"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isEnvAlpha(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setEnvAlpha(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isEnvAlpha();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setEnvAlpha(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdEnvAlpha();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdVisible : public PropertyCmd
    {
    public:
        CmdVisible() : PropertyCmd(N_PropertyUIWidget_Visible, _T("visible"), _I18n("可见"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isVisible(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setVisible(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isVisible();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setVisible(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVisible();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdDisable : public PropertyCmd
    {
    public:
        CmdDisable() : PropertyCmd(N_PropertyUIWidget_Disable, _T("disable"), _I18n("禁用"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isDisable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            temp ? static_cast<Widget *>(own)->disable() : static_cast<Widget *>(own)->enable();
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isDisable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            *temp ? static_cast<Widget *>(own)->disable() : static_cast<Widget *>(own)->enable();
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDisable();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdMargin : public PropertyCmd
    {
    public:
        CmdMargin() : PropertyCmd(N_PropertyUIWidget_Margin, _T("margin"), _I18n("边缘"))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const RelRectf & temp = static_cast<const Widget *>(own)->getMargin();
            Ntchar buff[512];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g %g %g %g %g"),
                temp.mTop.mScale, temp.mTop.mOffset,
                temp.mLeft.mScale, temp.mLeft.mOffset,
                temp.mBottom.mScale, temp.mBottom.mOffset,
                temp.mRight.mScale, temp.mRight.mOffset);

            out = buff;
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            RelRectf temp;
            Nsscanf(val.c_str(), _T("%g %g %g %g %g %g %g %g"),
                &temp.mTop.mScale, &temp.mTop.mOffset,
                &temp.mLeft.mScale, &temp.mLeft.mOffset,
                &temp.mBottom.mScale, &temp.mBottom.mOffset,
                &temp.mRight.mScale, &temp.mRight.mOffset);
            static_cast<Widget *>(own)->setMargin(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RelRectf * temp = (RelRectf *)out;
            *temp = static_cast<const Widget *>(own)->getMargin();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RelRectf * temp = (RelRectf *)in;
            static_cast<Widget *>(own)->setMargin(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMargin();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0");
        }
    };

    class _EngineInner CmdRestorePreFocus : public PropertyCmd
    {
    public:
        CmdRestorePreFocus() : PropertyCmd(N_PropertyUIWidget_RestorePreFocus,
            _T("restore_pre_focus"), _I18n("存储上次对被本级对焦的单元"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isRestorePreFocus(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setRestorePreFocus(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isRestorePreFocus();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setRestorePreFocus(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRestorePreFocus();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdMultiClickEvent : public PropertyCmd
    {
    public:
        CmdMultiClickEvent() : PropertyCmd(N_PropertyUIWidget_CursorMultiClick, _T("multi_click_event"), _I18n("光标多按键事件"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isCursorMultiClick(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setCursorMultiClick(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isCursorMultiClick();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setCursorMultiClick(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMultiClickEvent();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdWidgetModel : public PropertyCmd
    {
    public:
        CmdWidgetModel() : PropertyCmd(N_PropertyUIWidget_View, _T("widget_style"), _I18n("UI风格"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const Widget * obj = static_cast<const Widget *>(own);
            if(obj->getWidgetView())
                StrConv::conv(obj->getWidgetView()->getID(), out);
            else
                StrConv::conv(0, out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setWidgetView(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            const Widget * obj = static_cast<const Widget *>(own);
            if (obj->getWidgetView())
                *temp = obj->getWidgetView()->getID();
            else
                *temp = 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Widget *>(own)->setWidgetView(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdWidgetModel();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };

    class _EngineInner CmdStyle : public PropertyCmd
    {
    public:
        CmdStyle() : PropertyCmd(N_PropertyUIWidget_Style, _T("style"), _I18n("使用的风格集"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->getStyle(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 id;
            StrConv::conv(val, id);
            static_cast<Widget *>(own)->setStyle(id);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Widget *>(own)->getStyle();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Widget *>(own)->setStyle(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdStyle();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };

    class _EngineInner CmdUpdateMode : public PropertyCmd
    {
    public:
        CmdUpdateMode() : PropertyCmd(N_PropertyUIWidget_UpdateMode, _T("update_mode"), _I18n("更新方式"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            RefreshMode temp = static_cast<const Widget *>(own)->getUpdateMode();
            if (temp == RM_Alway)
            {
                out = _T("Always");
            }
            else if (temp == RM_Never)
            {
                out = _T("Never");
            }
            else if (temp == RM_Visible)
            {
                out = _T("Visible");
            }
            else
                out = _T("Always");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            RefreshMode temp;
            if (val == _T("Always"))
            {
                temp = RM_Alway;
            }
            else if (val == _T("Never"))
            {
                temp = RM_Never;
            }
            else
            {
                temp = RM_Visible;
            }
            static_cast<Widget *>(own)->setUpdateMode(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            RefreshMode * temp = (RefreshMode *)out;
            *temp = static_cast<const Widget *>(own)->getUpdateMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            RefreshMode * temp = (RefreshMode *)in;
            static_cast<Widget *>(own)->setUpdateMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdUpdateMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("Visible");
        }
    };
    class _EngineInner CmdMouseAutoRepeatEnabled : public PropertyCmd
    {
    public:
        CmdMouseAutoRepeatEnabled() : PropertyCmd(N_PropertyUIMouseControlItem_AutoEvent, _T("mouse_auto_repeat_enabled"),
            _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isClickAutoEvent(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setClickAutoEvent(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isClickAutoEvent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setClickAutoEvent(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMouseAutoRepeatEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    // CmdAutoRepeatRate
    class _EngineInner CmdAutoRepeatRate : public PropertyCmd
    {
    public:
        CmdAutoRepeatRate() : PropertyCmd(N_PropertyUIMouseControlItem_Rate, _T("auto_repeat_rate"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->getClickRate(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setClickRate(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Widget *>(own)->getClickRate();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Widget *>(own)->setClickRate(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoRepeatRate();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.06f, out);
        }
    };
    // CmdAutoRepeatDelay
    class _EngineInner CmdAutoRepeatDelay : public PropertyCmd
    {
    public:
        CmdAutoRepeatDelay() : PropertyCmd(N_PropertyUIMouseControlItem_Threshold, _T("auto_repeat_delay"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->getClickThreshold(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setClickThreshold(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Widget *>(own)->getClickThreshold();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Widget *>(own)->setClickThreshold(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoRepeatDelay();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.3f, out);
        }
    };

    class _EngineInner CmdCursorSubmitEvent : public PropertyCmd
    {
    public:
        CmdCursorSubmitEvent() : PropertyCmd(N_PropertyUIWidget_SubmitCursorEvent, _T("submit_cursor_event"),
            _T("传达光标事件,是否向所属下级元素传达光标事件"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isCursorSubmitEvent(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setCursorSubmitEvent(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isCursorSubmitEvent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setCursorSubmitEvent(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCursorSubmitEvent();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdAutoWidget : public PropertyCmd
    {
    public:
        CmdAutoWidget() : PropertyCmd(N_PropertyUIWidget_AutoWindow, _T("auto_widget"),
            _T("自动UI单元,这个单元并非手动创建的,而是复合UI单元的组成部分,由系统创建"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isAutoWindow(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setAutoWindow(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isAutoWindow();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setAutoWindow(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoWidget();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdTextParsingEnabled : public PropertyCmd
    {
    public:
        CmdTextParsingEnabled() : PropertyCmd(N_PropertyUIWidget_TextLayout, _T("text_parsing_enabled"),
            _T("Property to get/set the text parsing setting for the Widget.  "
                "Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isTextLayout(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setTextLayout(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isTextLayout();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setTextLayout(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTextParsingEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdText : public PropertyCmd
    {
    public:
        CmdText() : PropertyCmd(N_PropertyUIWidget_Text, _T("text"),
            _T("Property to get/set the text / caption for the Widget. Value is the text string to use. Meaning of this property heavily depends on the type of the Widget."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const Widget *>(own)->getText();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<Widget *>(own)->setText(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const Widget *>(own)->getText();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            String * temp = (String *)in;
            static_cast<Widget *>(own)->setText(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdText();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };

    class _EngineInner CmdTextColour : public PropertyCmd
    {
    public:
        CmdTextColour() : PropertyCmd(N_PropertyUIWidget_TextColour, _T("TextColours"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            UI::StrConv::conv(static_cast<const Widget *>(own)->getTextColour(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ColourArea temp;
            UI::StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setTextColour(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ColourArea * temp = (ColourArea *)out;
            *temp = static_cast<const Widget *>(own)->getTextColour();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ColourArea * temp = (ColourArea *)in;
            static_cast<Widget *>(own)->setTextColour(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTextColour();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            UI::StrConv::conv(ColourArea(0xFFFFFFFF), out);
        }
    };

    class _EngineInner CmdFont : public PropertyCmd
    {
    public:
        CmdFont() : PropertyCmd(N_PropertyUIWidget_Font, _T("font"),
            _T("Property to get/set the font for the Widget.  Value is the name of the font to use (must be loaded already)."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const Font * val = static_cast<const Widget *>(own)->getFont(0);
            out = val ? N_conv(val->getID()) : _T("");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 fid;
            StrConv::conv(val, fid);
            static_cast<Widget *>(own)->setFont(N_Only(Font).getFont(fid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            const Font * val = static_cast<const Widget *>(own)->getFont(0);
            *temp = val ? val->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Widget *>(own)->setFont(N_Only(Font).getFont(*temp));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFont();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    class _EngineInner CmdRiseOnClickEnabled : public PropertyCmd
    {
    public:
        CmdRiseOnClickEnabled() : PropertyCmd(N_PropertyUIWidget_ZOrderClick, _T("rise_on_click_enabled"),
            _I18n("游标点击时,本窗体是否跳到最表层"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isZOrderClick(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setZOrderClick(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isZOrderClick();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setZOrderClick(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRiseOnClickEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdAlwaysOnTop : public PropertyCmd
    {
    public:
        CmdAlwaysOnTop() : PropertyCmd(N_PropertyUIWidget_Top, _T("always_on_top"), _I18n("窗体总是在最前"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isTop(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setTop(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isTop();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setTop(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAlwaysOnTop();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdZOrderingEnabled : public PropertyCmd
    {
    public:
        CmdZOrderingEnabled() : PropertyCmd(N_PropertyUIWidget_ZOrder, _T("z_ordering_enabled"),
            _T("Property to get/set the 'z-order changing enabled' setting for the Widget. Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Widget *>(own)->isZOrder(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Widget *>(own)->setZOrder(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Widget *>(own)->isZOrder();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Widget *>(own)->setZOrder(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdZOrderingEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, Widget, IDEvent,                    0);
    N_EVENT(CommandObj, Widget, NameEvent,                  1);
    N_EVENT(CommandObj, Widget, AlphaEvent,                 2);
    N_EVENT(CommandObj, Widget, SizeEvent,                  3);
    N_EVENT(CommandObj, Widget, ParentSizeEvent,            4);
    N_EVENT(CommandObj, Widget, MoveEvent,                  5);
    N_EVENT(CommandObj, Widget, HAlignModeEvent,            6);
    N_EVENT(CommandObj, Widget, VAlignModeEvent,            7);
    N_EVENT(CommandObj, Widget, RotateEvent,                8);
    N_EVENT(CommandObj, Widget, ShowEvent,                  9);
    N_EVENT(CommandObj, Widget, HideEvent,                  10);
    N_EVENT(CommandObj, Widget, EnableEvent,                11);
    N_EVENT(CommandObj, Widget, DisableEvent,               12);
    N_EVENT(CommandObj, Widget, ActivateEvent,              13);
    N_EVENT(CommandObj, Widget, DeactivateEvent,            14);
    N_EVENT(CommandObj, Widget, StyleAttachEvent,           15);
    N_EVENT(CommandObj, Widget, StyleDetachEvent,           16);
    N_EVENT(CommandObj, Widget, EnvAlphaEvent,              17);
    N_EVENT(CommandObj, Widget, InvalidEvent,               18);
    N_EVENT(CommandObj, Widget, UpdataEvent,                19);
    N_EVENT(CommandObj, Widget, DestroyStartEvent,          20);
    N_EVENT(CommandObj, Widget, AutoClipModeEvent,          21);
    N_EVENT(CommandObj, Widget, AutoDestroyModeEvent,       22);
    N_EVENT(CommandObj, Widget, RenderCtxEvent,             23);
    N_EVENT(CommandObj, Widget, FocusEvent,                 24);
    N_EVENT(CommandObj, Widget, LostEvent,                  25);
    N_EVENT(CommandObj, Widget, RenderBeginEvent,           26);
    N_EVENT(CommandObj, Widget, RenderEndEvent,             27);
    N_EVENT(CommandObj, Widget, ViewAttachEvent,            28);
    N_EVENT(CommandObj, Widget, ViewDetachEvent,            29);
    N_EVENT(CommandObj, Widget, CursorEnterEvent,           30);
    N_EVENT(CommandObj, Widget, CursorLeaveEvent,           31);
    N_EVENT(CommandObj, Widget, CursorMoveEvent,            32);
    N_EVENT(CommandObj, Widget, CursorDragEvent,            33);
    N_EVENT(CommandObj, Widget, ButtonDownEvent,            34);
    N_EVENT(CommandObj, Widget, ButtonUpEvent,              35);
    N_EVENT(CommandObj, Widget, CursorTouchEvent,           36);
    N_EVENT(CommandObj, Widget, CursorMultiTouchEvent,      37);
    N_EVENT(CommandObj, Widget, KeyDownEvent,               38);
    N_EVENT(CommandObj, Widget, KeyUpEvent,                 39);
    N_EVENT(CommandObj, Widget, CharEvent,                  40);
    N_EVENT(CommandObj, Widget, MarginModeEvent,            41);
    N_EVENT(CommandObj, Widget, ZOrderEvent,                42);
    N_EVENT(CommandObj, Widget, TopModeEvent,               43);
    N_EVENT(CommandObj, Widget, FontEvent,                  44);
    N_EVENT(CommandObj, Widget, TextEvent,                  45);
    N_EVENT(CommandObj, Widget, TextLayoutModeEvent,        46);
    N_EVENT(CommandObj, Widget, EventCount,                 47);
    //------------------------------------------------------------------------
    Widget::Widget(ScriptTypeID stid, WidgetID wid, FactoryID type, Container * own,
        const String & name, LangID lid) :
        PropertyCmdObj(stid, lid),
        mID(wid),
        mType(type),
        mParent(own),
        mName(name),
        mRefreshMode(RM_Visible),
        mPreFocus(0),
        mWidgetView(0),
        mArea(RelPosf(0, 0), RelPosf(0, 0), RelPosf(0, 0), RelPosf(0, 0)),
        mHAlign(HA_LEFT),
        mVAlign(VA_TOP),
        mMargin(RelRectf(RelPosf(0, 0))),
        mAlpha(1.0f),
        mMinSize(RelPosf(0, 0), RelPosf(0, 0)),
        mMaxSize(RelPosf(0, 0), RelPosf(0, 0)),
        mScaleMode(AM_None),
        mPixelSize(0.0f, 0.0f),
        mRot(Quaternion::IDENTITY),
        mRect(this, &Widget::getAreaImpl),
        mInnerRect(this, &Widget::getInnerAreaImpl),
        mClipRect(0, 0, 0, 0),
        mClipInnerRect(0, 0, 0, 0),
        mIntersectRect(0, 0, 0, 0),
        mForegroundColour(0xFFFFFFFF),
        mClickThreshold(300),
        mClickRate(60),
        mSpaceMark(0),
        mScaleFactor(1.0 / 1.0),
        mFont(0),
        mTextView(0),
        mStyleID(0),
        mViewID(0),
        mImage(0),
#ifndef CEGUI_BIDI_SUPPORT
        mBidiText(0),
#elif defined (CEGUI_USE_FRIBIDI)
        mBidiText(N_new FribidiText),
#elif defined (CEGUI_USE_MINIBIDI)
        mBidiText(N_new MinibidiText),
#else
        #error "BIDI Configuration is inconsistant, check your config!"
#endif
        mZOrderClick(true),
        mZOrder(true),
        mValidBidi(false),
        mValidText(false),
        mTextLayout(true),
        mPixelAlign(true),
        mAutoGen(false),
        mInit(false),
        mDestroy(false),
        mEnable(true),
        mTop(false),
        mVisible(true),
        mActive(false),
        mAutoDestroy(true),
        mAutoClip(true),
        mRedraw(true),
        mRestorePreFocus(false),
        mValidClipRect(false),
        mValidClipInnerRect(false),
        mValidIntersectRect(false),
        mEnvAlpha(true),
        mDataWrite(true),
        mClickAutoEvent(false),
        mCursorSubmitEvent(false),
        mMultiClickEventEnable(true)
    {
        notifySignal(Widget::EventCount);
        N_Only(GBuffer).create(mPixelData);
        if(mParent)
            mSheet = mParent->mSheet;
        setupCmd(N_CmdObj_Widget);

        //mAide1 = CursorWidgetAide();
        //mAide2 = TooltipWidgetAide();
    }
    //------------------------------------------------------------------------
    Widget::Widget(const Widget & w):
        mRect(this, &Widget::getAreaImpl),
        mInnerRect(this, &Widget::getInnerAreaImpl)
    {
    }
    //------------------------------------------------------------------------
    Widget::~Widget()
    {
        if(mBidiText)
        {
            N_delete mBidiText;
            mBidiText = 0;
        }
        N_Only(GBuffer).destroy(mPixelData);
    }
    //------------------------------------------------------------------------
    Widget & Widget::operator=(const Widget & w)
    {
        return *this;
    }
    //------------------------------------------------------------------------
    void Widget::update(TimeDurMS t)
    {
        updateImpl(t);

        UpdateEventArgs arg(this, t);
        signal(UpdataEvent, &arg);
    }
    //------------------------------------------------------------------------
    void Widget::addAide(WidgetAide * aide)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++ i)
        {
            if (aide == *i)
                return;
        }
        mAideList.push_back(aide);
    }
    //------------------------------------------------------------------------
    void Widget::removeAide(WidgetAide * aide)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++i)
        {
            if (aide == *i)
            {
                mAideList.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    Widget * Widget::clone(bool r) const
    {
        Widget * re = N_Only(Widget).create(getStyleView(), 0, mParent);
        cloneImpl(re, r);
        return re;
    }
    //------------------------------------------------------------------------
    void Widget::destroy()
    {
        // because we know that people do not read the API ref properly,
        // here is some protection to ensure that WidgetManager does the
        // destruction and not anyone else.
        WidgetManager * mag = N_OnlyPtr(Widget);

        if(mag->isExist(this))
        {
            mag->destroy(this);
            return;
        }
        lost();

        // let go of the tooltip if we have it
        Tooltip * tip = mSheet->getTooltip();
        if(tip && tip->getDest()==this)
            tip->setDest(0);

        // clean up looknfeel related things
        if(mStyleID != 0)
        {
            if(mWidgetView)
                mWidgetView->onStyleDetach();
            N_Only(Widget).getStyle(mStyleID)->detach(this);
        }

        // free any assigned WidgetView
        if(mWidgetView)
        {
            mWidgetView->onDetach();
            N_Only(Widget).destroyView(mWidgetView);
            mWidgetView = 0;
        }

        // double check we are detached from parent
        if(mParent)
            mParent->remove(this);

        // signal our imminent destruction
        WidgetEventArgs args(this);
        onDestroy(&args);

        refresh();
    }
    //------------------------------------------------------------------------
    bool Widget::isContainer() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    void Widget::activate()
    {
        if(!isFinalVisible())
            return;

        // force complete release of input capture.
        // NB: This is not done via releaseCapture() because that has
        // different behaviour depending on the restoreOldCapture setting.
        Widget * const old = mSheet->getActive();
        if(old && old != this)
        {
            WidgetEventArgs a1(this);
            old->onLost(&a1);
            ActivationEventArgs a2(this);
            a2.mActWidget = old;
            onActivate(&a2);
        }
    }
    //------------------------------------------------------------------------
    void Widget::deactivate()
    {
        ActivationEventArgs args(this);
        args.mActWidget = 0;
        onInactivate(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setID(WidgetID ID)
    {
        if(mID == ID)
            return;

        mID = ID;

        WidgetEventArgs args(this);
        onID(&args);
    }
    //------------------------------------------------------------------------
    Window * Widget::getRoot() const
    {
        if(mParent)
            return mParent->getRoot();
        return static_cast<Window *>(mParent);
    }
    //------------------------------------------------------------------------
    void Widget::setStyle(StyleID sid)
    {
        if(mStyleID == sid)
            return;

        if(!mWidgetView)
            N_EXCEPT(NotExist, _I18n("There must be a window renderer assigned to the window '") +
                mName + _I18n("' to set its style"));

        WidgetManager & mag = N_Only(Widget);
        if(mStyleID != 0)
        {
            mWidgetView->onStyleDetach();
            Style * o = mag.getStyle(mStyleID);
            StyleEventArgs arg(this, o);
            onStyleDetach(&arg);
        }

        mStyleID = sid;
        N_Only(Log).log(_I18n("Assigning Style '") + N_conv(sid) + _I18n("' to window '") + mName + _T("'."));

        Style * n = mag.getStyle(sid);
        StyleEventArgs arg(this, n);
        onStyleAttach(&arg);
        mWidgetView->onStyleAttach();

        refresh();
        layout();
    }
    //------------------------------------------------------------------------
    void Widget::setAutoDestroy(bool b)
    {
        if(mAutoDestroy == b)
            return;

        mAutoDestroy = b;

        WidgetEventArgs args(this);
        onAutoDestroyMode(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setName(const String & t)
    {
        if(mName == t)
            return;

        if(mParent && mParent->isExist(t))
        {
            N_EXCEPT(UniqueRepeat, _I18n("Failed to rename Widget at: ") +
                getNamePath() + _I18n(" as: ") + t + _I18n(". A Widget ")
                    _I18n("with that name is already attached as a sibling."));
        }

        // log this under informative level
        N_Only(Log).log(_I18n("Renamed element at: ") + getNamePath() + _I18n(" as: ") + t);

        mName = t;

        WidgetEventArgs args(this);
        onName(&args);
    }
    //------------------------------------------------------------------------
    String Widget::getNamePath() const
    {
        String path;

        if(mParent)
            path = mParent->getNamePath() + _T('/') + getName();
        else
            path = getName();

        return path;
    }
    //------------------------------------------------------------------------
    bool Widget::isAncestor(WidgetID id) const
    {
        if(!mParent)
            return false;

        if(mParent->getID() == id)
            return true;

        return mParent->isAncestor(id);
    }
    //------------------------------------------------------------------------
    bool Widget::isAncestor(const Widget * w) const
    {
        if(!mParent)
            return false;

        if(mParent == w)
            return true;

        return mParent->isAncestor(w);
    }
    //------------------------------------------------------------------------
    bool Widget::isAncestor(const String & name) const
    {
        Widget const * current = this;

        while(1)
        {
            const Widget * parent = current->mParent;

            if(!parent)
                return false;

            if(parent->getName() == N_StrBlank)
                return false;

            if(parent->getName() == name)
                return true;

            current = parent;
        }
    }
    //------------------------------------------------------------------------
    const Widget * Widget::getCommonAncestor(const Widget * o) const
    {
        const Widget * widget = o;
        const Widget * pwidget = widget->mParent;

        while (pwidget)
        {
            if (isAncestor(pwidget))
                break;

            widget = pwidget;
            pwidget = pwidget->mParent;
        }

        return pwidget ? widget : 0;
    }
    //------------------------------------------------------------------------
    NIIf Widget::getActualPosX() const
    {
        PlaneSizei sheetsize = getRenderSize();
        Rectf urect(0, 0, sheetsize.mWidth, sheetsize.mHeight);
        const Rectf prect(mParent ? mParent->getChildArea(true).getIntCache() : urect);

        NIIf pwidth = prect.width();
        NIIf baseX = prect.mLeft + getRelArea().mLeft.abs(pwidth);

        switch (mHAlign)
        {
        case HA_CENTRE:
            baseX += (pwidth - getPixelSize().mWidth) * 0.5f;
            break;
        case HA_RIGHT:
            baseX += pwidth - getPixelSize().mWidth;
            break;
        default:
            break;
        }

        return N_ToPixel(baseX);
    }
    //------------------------------------------------------------------------
    NIIf Widget::getActualPosY() const
    {
        PlaneSizei sheetsize = getRenderSize();
        Rectf urect(0, 0, sheetsize.mWidth, sheetsize.mHeight);
        const Rectf prect(mParent ? mParent->getChildArea(true).getIntCache() : urect);

        NIIf pheight = prect.height();
        NIIf baseY = prect.mTop + getRelArea().mTop.abs(pheight);

        switch (mVAlign)
        {
        case VA_CENTRE:
            baseY += (pheight - getPixelSize().mHeight) * 0.5f;
            break;
        case VA_BOTTOM:
            baseY += pheight - getPixelSize().mHeight;
            break;
        default:
            break;
        }

        return N_ToPixel(baseY);
    }
    //------------------------------------------------------------------------
    void Widget::notifyPosChange()
    {
        mValidClipRect = false;
        mValidClipInnerRect = false;
        mValidIntersectRect = false;
        mRect.resetCached();
        mInnerRect.resetCached();
        SheetContext ctx;
        getContext(ctx);
        updateImpl(&ctx);
    }
    //------------------------------------------------------------------------
    void Widget::setClickAutoEvent(bool set)
    {
        if (mClickAutoEvent == set)
            return;

        mClickAutoEvent = set;

        // FIXME: There is a potential issue here if this setting is
        // FIXME: changed _while_ the mouse is auto-repeating, and
        // FIXME: the 'captured' state of input could get messed up.
        // FIXME: The alternative is to always release here, but that
        // FIXME: has a load of side effects too - so for now nothing
        // FIXME: is done.  This whole aspect of the system needs a
        // FIXME: review an reworking - though such a change was
        // FIXME: beyond the scope of the bug-fix that originated this
        // FIXME: comment block.  PDT - 30/10/06
    }
    //------------------------------------------------------------------------
    void Widget::setXAlign(HAlign align)
    {
        if(mHAlign == align)
            return;

        mHAlign = align;

        WidgetEventArgs args(this);
        onHAlignMode(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setYAlign(VAlign align)
    {
        if(mVAlign == align)
            return;

        mVAlign = align;

        WidgetEventArgs args(this);
        onVAlignMode(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setMargin(const RelRectf & margin)
    {
        mMargin = margin;

        WidgetEventArgs args(this);
        onMargin(&args);
    }
    //------------------------------------------------------------------------
    RelVector2f Widget::getMarginOft() const
    {
        return RelVector2f(mMargin.mLeft, mMargin.mTop);
    }
    //------------------------------------------------------------------------
    RelVector2f Widget::getMarginSize() const
    {
        const PlaneSizef & pixelSize = getPixelSize();

        // we rely on pixelSize rather than mixed absolute and relative getSize
        // this seems to solve problems when windows overlap because their size
        // is constrained by min size
        const RelVector2f size(RelPosf(0, pixelSize.mWidth), RelPosf(0, pixelSize.mHeight));
        // todo: we still do mixed absolute/relative margin, should we convert the
        //       value to absolute?

        return RelVector2f(mMargin.mLeft + size.x + mMargin.mRight,
            mMargin.mTop + size.y + mMargin.mBottom);
    }
    //------------------------------------------------------------------------
    void Widget::setMinSize(const RelPlaneSizef & size)
    {
        mMinSize = size;

        // TODO: Perhaps we could be more selective and skip this if min size won't
        //       affect the size
        setSize(getSize());
    }
    //------------------------------------------------------------------------
    void Widget::setMaxSize(const RelPlaneSizef & size)
    {
        mMaxSize = size;

        // TODO: Perhaps we could be more selective and skip this if min size won't
        //       affect the size
        setSize(getSize());
    }
    //------------------------------------------------------------------------
    void Widget::setScaleMode(AspectMode mode)
    {
        if(mScaleMode == mode)
            return;

        mScaleMode = mode;

        // TODO: We want an Event and more smart rect update handling

        // Ensure the area is calculated with the new aspect mode
        // TODO: This potentially wastes effort, we should just mark it as dirty
        //       and postpone the calculation for as long as possible
        setRelArea(RelRectf(mArea.mLeft, mArea.mTop, mArea.getSize()));
    }
    //------------------------------------------------------------------------
    void Widget::setScaleFactor(NIIf ratio)
    {
        if(mScaleFactor == ratio)
            return;

        mScaleFactor = ratio;

        // TODO: We want an Event and more smart rect update handling

        // Ensure the area is calculated with the new aspect ratio
        // TODO: This potentially wastes effort, we should just mark it as dirty
        //       and postpone the calculation for as long as possible
        setRelArea(RelRectf(mArea.mLeft, mArea.mTop, mArea.getSize()));
    }
    //------------------------------------------------------------------------
    void Widget::setTop(bool b)
    {
        // only react to an actual change
        if (isTop() == b)
            return;

        mTop = b;
        // move us in front of sibling windows with the same 'always-on-top'
        // set as we have.
        if (mParent)
        {
            mParent->removeImpl(this);
            mParent->addImpl(this);

            ZOrderImpl();
        }

        WidgetEventArgs args(this);
        onTopMode(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setStyle(StyleID sid, WidgetViewlID wsid)
    {
        if(wsid != 0)
            setWidgetView(wsid);

        if(sid != 0)
            setStyle(sid);
    }
    //------------------------------------------------------------------------
    void Widget::setAutoClip(bool b)
    {
        // only react if setting has changed
        if(mAutoClip == b)
            return;

        mAutoClip = b;
        WidgetEventArgs args(this);
        onClipMode(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setRestorePreFocus(bool b)
    {
        mRestorePreFocus = b;
    }
    //------------------------------------------------------------------------
    bool Widget::isRestorePreFocus() const
    {
        return mRestorePreFocus;
    }
    //------------------------------------------------------------------------
    void Widget::setVisible(bool b)
    {
        if(mVisible == b)
            return;

        mVisible = b;
        WidgetEventArgs args(this);
        mVisible ? onShow(&args) : onHide(&args);

        mSheet->resetCursorOver();
    }
    //------------------------------------------------------------------------
    void Widget::setEnable(bool b)
    {
        if(mEnable == b)
            return;

        mEnable = b;
        WidgetEventArgs args(this);

        if(mEnable)
        {
            // check to see if the window is actually enabled (which depends
            // upon all ancestor windows being enabled) we do this so that
            // events we fire give an accurate indication of the state of a
            // window.
            if((mParent && ! mParent->isDisable()) || !mParent)
                onEnable(&args);
        }
        else
        {
            onDisable(&args);
        }

        mSheet->resetCursorOver();
    }
    //------------------------------------------------------------------------
    void Widget::setPixelAlign(bool b)
    {
        if(mPixelAlign == b)
            return;

        mPixelAlign = b;

        setRelArea(RelRectf(mArea.mLeft, mArea.mTop, mArea.getSize()));
    }
    //------------------------------------------------------------------------
    Vector2f Widget::getPixelPos() const
    {
        return mRect.getIntCache().getPos();
    }
    //------------------------------------------------------------------------
    void Widget::setWidgetView(WidgetViewlID wsid)
    {
        if(mWidgetView && mWidgetView->getID() == wsid)
            return;

        WidgetManager & mag = N_Only(Widget);
        if(mWidgetView != 0)
        {
            if(mWidgetView->getID() == wsid)
                return;

            WidgetEventArgs e(this);
            onViewDetach(&e);
            mag.destroyView(mWidgetView);
        }

        if(wsid != 0)
        {
            N_Only(Log).log(_I18n("设置风格: ") + N_conv(wsid) + _I18n("' 到UI单元 :") + mName);
            mWidgetView = mag.createWindow(wsid);
            WidgetEventArgs e(this);
            onViewAttach(&e);
        }
        else
            N_EXCEPT(InvalidRequest, _I18n("设置了一个无效的风格到UI单元 :") + mName);
    }
    //------------------------------------------------------------------------
    void Widget::setStyleView(StyleViewComID wsid)
    {
        if (wsid == mViewID)
            return;
        mViewID = wsid;
    }
    //------------------------------------------------------------------------
    const String & Widget::getTooltipText() const
    {
        if (mParentTipsText && mParent && mTipsText.empty())
            return mParent->getTooltipText();
        else
            return mTipsText;
    }
    //------------------------------------------------------------------------
    PlaneSizef Widget::calcArea(bool pixelAlign) const
    {
        // calculate pixel sizes for everything, so we have a common format for
        // comparisons.
        PlaneSizei tempsize = getRenderSize();
        PlaneSizef absMin(abs(mMinSize, PlaneSizef(tempsize.mWidth, tempsize.mHeight), false));
        PlaneSizef absMax(abs(mMaxSize, PlaneSizef(tempsize.mWidth, tempsize.mHeight), false));

        PlaneSizef base_size;
        if(pixelAlign)
        {
            base_size = PlaneSizef(mParent ?
                mParent->getInnerArea().getIntCache().getSize() : calcParentArea());
        }
        else
        {
            base_size = PlaneSizef(mParent ?
                mParent->getInnerArea().getFloatCache().getSize() : calcParentArea(false));
        }

        PlaneSizef ret = abs(mArea.getSize(), base_size, false);

        // in case absMin components are larger than absMax ones,
        // max size takes precedence
        if(absMax.mWidth != 0.0f && absMin.mWidth > absMax.mWidth)
        {
            absMin.mWidth = absMax.mWidth;
            N_Only(Log).log("MinSize resulted in an absolute pixel size with "
                _I18n("width larger than what MaxSize resulted in"));
        }

        if(absMax.mHeight != 0.0f && absMin.mHeight > absMax.mHeight)
        {
            absMin.mHeight = absMax.mHeight;
            N_Only(Log).log("MinSize resulted in an absolute pixel size with "
                 _I18n("height larger than what MaxSize resulted in"));
        }

        // limit new pixel size to: minSize <= newSize <= maxSize
        if(ret.mWidth < absMin.mWidth)
            ret.mWidth = absMin.mWidth;
        else if (absMax.mWidth != 0.0f && ret.mWidth > absMax.mWidth)
            ret.mWidth = absMax.mWidth;

        if(ret.mHeight < absMin.mHeight)
            ret.mHeight = absMin.mHeight;
        else if (absMax.mHeight != 0.0f && ret.mHeight > absMax.mHeight)
            ret.mHeight = absMax.mHeight;

        if(mScaleMode != AM_None)
        {
            // make sure we respect current aspect mode and ratio
            ret.scale(mScaleMode, mScaleFactor);

            // make sure we haven't blown any of the hard limits
            // still maintain the aspect when we do this
            if(mScaleMode == AM_Shrink)
            {
                NIIf ratio = 1.0f;
                // check that we haven't blown the min size
                if(ret.mWidth < absMin.mWidth)
                {
                    ratio = absMin.mWidth / ret.mWidth;
                }
                if(ret.mHeight < absMin.mHeight)
                {
                    const NIIf newRatio = absMin.mHeight / ret.mHeight;
                    if(newRatio > ratio)
                        ratio = newRatio;
                }
                ret.mWidth *= ratio;
                ret.mHeight *= ratio;
            }
            else if(mScaleMode == AM_Expand)
            {
                NIIf ratio = 1.0f;
                // check that we haven't blown the min size
                if(absMax.mWidth != 0.0f && ret.mWidth > absMax.mWidth)
                {
                    ratio = absMax.mWidth / ret.mWidth;
                }
                if(absMax.mHeight != 0.0f && ret.mHeight > absMax.mHeight)
                {
                    const NIIf newRatio = absMax.mHeight / ret.mHeight;
                    if(newRatio > ratio)
                        ratio = newRatio;
                }
                ret.mWidth *= ratio;
                ret.mHeight *= ratio;
            }
            // NOTE: When the hard min max limits are unsatisfiable with the aspect lock mode,
            //       the result won't be limited by both limits!
        }

        if(mPixelAlign)
        {
            ret.mWidth = N_ToPixel(ret.mWidth);
            ret.mHeight = N_ToPixel(ret.mHeight);
        }
        return ret;
    }
    //------------------------------------------------------------------------
    PlaneSizef Widget::calcParentArea(bool pixelAlign) const
    {
        if(mParent)
        {
            return pixelAlign ? mParent->getPixelSize() : mParent->calcArea(false);
        }
        else
        {
            PlaneSizei temp = getRenderSize();
            return PlaneSizef(temp.mWidth, temp.mHeight);
        }
    }
    //------------------------------------------------------------------------
    void Widget::getContext(SheetContext & ctx) const
    {
        if(mWidgetView)
            mWidgetView->getContext(ctx);
        else
            getContextImpl(ctx);
    }
    //------------------------------------------------------------------------
    void Widget::cloneImpl(Widget * dest, bool) const
    {
        PropertyCmdMap::iterator i, iend = mProperty->getList().end();
        for(i = mProperty->getList().begin(); i != iend; ++i)
        {
            PropertyID pid = i->first;
            String pvalue;
            get(pid, pvalue);

            // we never copy stuff that doesn't get written into XML
            if(!isWrite(pid))
                continue;

            // some cases when pvalue is "" could lead to exception throws
            if(pvalue.empty())
            {
                // special case, this causes exception throw when no window renderer
                // is assigned to the window
                if(pid == N_PropertyUIWidget_Style)
                    continue;

                // special case, this causes exception throw because we are setting
                // 'null' window renderer
                if(pid == N_PropertyUIWidget_View)
                    continue;
            }
            dest->set(pid, pvalue);
        }
    }
    //------------------------------------------------------------------------
    void Widget::getContextImpl(SheetContext & ctx) const
    {
        return mParent->getContext(ctx);
    }
    //------------------------------------------------------------------------
    void Widget::renderImpl()
    {
        // nothing
    }
    //------------------------------------------------------------------------
    void Widget::ZOrderImpl()
    {
        if(!mParent)
        {
            WidgetEventArgs args(this);
            onZOrder(&args);
        }
        else
        {
            WidgetList::iterator i, iend = mParent->mChilds.end();
            for(i = mParent->mChilds.begin(); i != iend; ++iend)
            {
                Widget * w = *i;
                if(w)
                {
                    WidgetEventArgs args(w);
                    w->onZOrder(&args);
                }
            }

        }
        mSheet->resetCursorOver();
    }
    //------------------------------------------------------------------------
    bool Widget::moveFrontImpl(bool click)
    {
        bool act = false;

        if (!mParent)
        {
            if (!isFinalActive())
            {
                act = true;
                ActivationEventArgs args(this);
                args.mActWidget = 0;
                onActivate(&args);
            }
            return act;
        }
        act = mParent->moveFrontImpl(click);

        Widget * const aw = getActiveSibling();
        if (aw != this)
        {
            act = true;

            // notify ourselves that we have become active
            ActivationEventArgs args(this);
            args.mActWidget = aw;
            onActivate(&args);

            // notify any previously active window that it is no longer active
            if (aw)
            {
                args.mWidget = aw;
                args.mActWidget = this;
                aw->onInactivate(&args);
            }
        }

        // bring us to the front of our siblings
        if (mZOrder && (!click || mZOrderClick) && !isTopPos())
        {
            act = true;

            mParent->moveChildFront(this);

            ZOrderImpl();
        }
        return act;
    }
    //------------------------------------------------------------------------
    void Widget::refreshFaceImpl()
    {
        if(mParent)
            mParent->refreshFaceImpl();
    }
    //------------------------------------------------------------------------
    void Widget::setAutoWindow(bool b)
    {
        mAutoGen = b;

        if(mAutoGen)
            disableWrite();
    }
    //------------------------------------------------------------------------
    void Widget::write(XmlSerializer * out) const
    {
        if(mDataWrite)
        {
            // output opening Widget tag
            out->begin(N_XML_TAG_Widget).attribute(TypeProperty, N_conv(mType));

            // write name if not auto-generated
            if(getID() < N_Max_Valid_ID)
            {
                out->attribute(N_PropertyUIWidget_Name, N_conv(getID()));
            }
            writeImpl(out);

            // now ouput closing Widget tag
            out->end();
        }
    }
    //------------------------------------------------------------------------
    void Widget::setWrite(PropertyID pid, bool b)
    {
        if(b)
        {
            mNonSerlProperty.erase(pid);
        }
        else
        {
            PropertyCmd * pcmd = getProperty()->get(pid);
            if(!pcmd->isWritable())
            {
                return;
            }

            // check if the insertion failed
            if(!mNonSerlProperty.insert(pid).second)
                N_Only(Log).log(_I18n("PropertyCmd,ID: '") + N_conv(pid) +
                    _I18n("' 限制在UI单元,ID: '") + mName + _T("'"));
        }
    }
    //------------------------------------------------------------------------
    NCount Widget::writeImpl(XmlSerializer * out) const
    {
        NCount count = 0;
        PropertyCmdMap::const_iterator i, iend = mProperty->getList().end();
        for(i = mProperty->getList().begin(); i != iend; ++i)
        {
            // first we check to make sure the property is'nt banned from XML
            if(isWrite(i->first))
            {
                try
                {
                    if(!isDefault(i->second))
                    {
                        i->second->write(this, out);
                        ++count;
                    }
                }
                catch(InvalidRequestException &)
                {
                    N_Only(Log).log(_I18n("property receiving failed.Continuing..."));
                }
            }
        }
        return count;
    }
    //------------------------------------------------------------------------
    void Widget::disableWrite()
    {
        setWrite(N_PropertyUIWidget_AutoWindow, false);
        setWrite(N_PropertyUIWidget_AutoDestroy, false);
        setWrite(N_PropertyUIWidget_VAlign, false);
        setWrite(N_PropertyUIWidget_HAlign, false);
        setWrite(N_PropertyUIWidget_RelArea, false);
        setWrite(N_PropertyUIWidget_Pos, false);
        setWrite(N_PropertyUIWidget_Size, false);
        setWrite(N_PropertyUIWidget_MinSize, false);
        setWrite(N_PropertyUIWidget_MaxSize, false);
        setWrite(N_PropertyUIWidget_View, false);
        setWrite(N_PropertyUIWidget_Style, false);
    }
    //------------------------------------------------------------------------
    void Widget::onBeginInit()
    {
        mInit = true;
    }
    //------------------------------------------------------------------------
    void Widget::onEndInit()
    {
        mInit = false;
    }
    //------------------------------------------------------------------------
    void Widget::refresh(bool r)
    {
        refreshImpl(r);
        mSheet->redraw();
    }
    //------------------------------------------------------------------------
    bool Widget::focus()
    {
        if(!isFinalActive())
            return false;

        if(!isFocus())
        {
            Widget * old = mSheet->getFocus();
            mSheet->setFocus(this);
            WidgetEventArgs args(this);

            // inform window which previously had capture that it doesn't anymore.
            if(old && old != this && !mRestorePreFocus)
                old->onLost(&args);

            if(mRestorePreFocus)
                mPreFocus = old;

            onFocus(&args);
        }
        return true;
    }
    //------------------------------------------------------------------------
    void Widget::lost()
    {
        // if we are not the window that has capture, do nothing
        if(isFocus())
        {
            // restore old captured window if that mode is set
            if(mRestorePreFocus)
            {
                mSheet->setFocus(mPreFocus);
                // check for case when there was no previously captured window
                if(mPreFocus)
                {
                    if(mPreFocus->isContainer())
                    {
                        static_cast<Container *>(mPreFocus)->moveFront();
                    }
                    mPreFocus = 0;
                }
            }
            else
            {
                mSheet->setFocus(0);
            }
            WidgetEventArgs args(this);
            onLost(&args);
        }
    }
    //------------------------------------------------------------------------
    void Widget::setFont(Font * obj)
    {
        mFont = obj;
        mValidText = false;
        FontEventArgs args(obj);
        onFont(&args);
    }
    //------------------------------------------------------------------------
    Font * Widget::getFont(FontID fid) const
    {
        Font * re;
        if(!mFont)
        {
            if(fid == 0)
                re = mSheet->getFont();
            else
                re = 0;
        }
        else
        {
            re = mFont;
        }
        return re;
    }
    //------------------------------------------------------------------------
    void Widget::setText(const String & t)
    {
        mText = t;
        mValidText = false;
        mValidBidi = false;

        WidgetEventArgs args(this);
        onText(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setTextColour(const ColourArea & colours)
    {
        mForegroundColour = colours;
        refresh();
    }
    //------------------------------------------------------------------------
    void Widget::insertText(const String & text, Nidx pos)
    {
        mText.insert(pos, text);
        mValidText = false;
        mValidBidi = false;

        WidgetEventArgs args(this);
        onText(&args);
    }
    //------------------------------------------------------------------------
    void Widget::appendText(const String & text)
    {
        mText.append(text);
        mValidText = false;
        mValidBidi = false;

        WidgetEventArgs args(this);
        onText(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setTextLayout(bool b)
    {
        mTextLayout = b;
        mValidText = false;

        WidgetEventArgs args(this);
        onTextLayoutMode(&args);
    }
    //------------------------------------------------------------------------
    bool Widget::onCopyData(Clipboard &)
    {
        // deny copying by default
        return false;
    }
    //------------------------------------------------------------------------
    bool Widget::onCutData(Clipboard &)
    {
        // deny cutting by default
        return false;
    }
    //------------------------------------------------------------------------
    bool Widget::onPasteData(Clipboard &)
    {
        // deny pasting by default
        return false;
    }
    //------------------------------------------------------------------------
    const String & Widget::getVisualText() const
    {
        if(!mBidiText)
            return mText;

        if(!mValidBidi)
        {
            mBidiText->setSrc(mText);
            mValidBidi = true;
        }
        return mBidiText->getDst();
    }
    //------------------------------------------------------------------------
    const PixelUnitGrid * Widget::getRenderText() const
    {
        if(!mValidText)
        {
            mRenderText = getRenderTextView()->parse(getVisualText(), const_cast<Font *>(getFont()), 0);
            mValidText = true;
        }
        return &mRenderText;
    }
    //------------------------------------------------------------------------
    void Widget::setTextView(TextView * parser)
    {
        mTextView = parser;
        mValidText = false;
    }
    //------------------------------------------------------------------------
    TextView * Widget::getRenderTextView() const
    {
        if(!mTextLayout)
            return N_Only(UI).TextView0;

        if(mTextView)
            return mTextView;

        TextView * uiview = N_Only(UI).getView();
        if(uiview)
            return uiview;

        return N_Only(UI).TextView1;
    }
    //------------------------------------------------------------------------
    void Widget::setAlpha(NIIf alpha)
    {
        mAlpha = N_MAX(NIIf, N_MIN(NIIf, alpha, 1.0f), 0.0f);
        WidgetEventArgs args(this);
        onAlpha(&args);
    }
    //------------------------------------------------------------------------
    void Widget::setEnvAlpha(bool set)
    {
        if(mEnvAlpha != set)
        {
            // store old effective alpha so we can test if alpha value changes due
            // to new set.
            NIIf oldAlpha = getAlphaEffect();

            // notify about the set change.
            mEnvAlpha = set;

            WidgetEventArgs args(this);
            onEnvAlpha(&args);

            if(oldAlpha != getAlphaEffect())
            {
                onAlpha(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    NIIf Widget::getAlphaEffect() const
    {
        if(!mParent || !isEnvAlpha())
            return mAlpha;

        return mAlpha * mParent->getAlphaEffect();
    }
    //------------------------------------------------------------------------
    void Widget::setBGImage(PixelBuffer * img)
    {
        mImage = img;
        refresh();
    }
    //------------------------------------------------------------------------
    bool Widget::isFinalDisable() const
    {
        bool f = !mParent ? false : mParent->isFinalDisable();

        return !mEnable || f;
    }
    //------------------------------------------------------------------------
    bool Widget::isFinalVisible() const
    {
        bool pvisible = !mParent ? true : mParent->isFinalVisible();

        return mVisible && pvisible;
    }
    //------------------------------------------------------------------------
    bool Widget::isFinalActive() const
    {
        bool pactive = !mParent ? true : mParent->isFinalActive();

        return mActive && pactive;
    }
    //------------------------------------------------------------------------
    bool Widget::intersects(const Vector2f & pos, bool disable) const
    {
        if(!disable && isFinalDisable())
            return false;

        const Rectf test(getIntersectArea());

        if((test.getWidth() == 0.0f) || (test.getHeight() == 0.0f))
            return false;

        return test.intersects(pos.x, pos.y);
    }
    //------------------------------------------------------------------------
    Widget * Widget::getActiveSibling()
    {
        // initialise with this if we are active, else 0
        Widget * re = isFinalActive() ? this : 0;

        // if active window not already known, and we have a parent window
        if(!re && mParent)
        {
            // scan backwards through the draw list, as this will
            // usually result in the fastest result.
            WidgetList::reverse_iterator it, itend = mParent->mChilds.rend();
            for(it = mParent->mChilds.rbegin(); it != itend; ++it)
            {
                Widget * wnd = *it;
                // if this child is active
                if(wnd->isFinalActive())
                {
                    // set the return value
                    re = wnd;
                    // exit loop early, as we have found what we needed
                    break;
                }
            }
        }

        return re;
    }
    //------------------------------------------------------------------------
    bool Widget::isPos(const Vector2f & pos, bool disable) const
    {
        return intersects(pos, disable);
    }
    //------------------------------------------------------------------------
    bool Widget::isTopPos() const
    {
        if (!mParent)
            return true;

        WidgetList::reverse_iterator pos;
        if (!mTop)
        {
            pos = mParent->mChilds.rbegin();
            while ((pos != mParent->mChilds.rend()) && (*pos)->isTop())
                ++pos;
        }
        return *pos == this;
    }
    //------------------------------------------------------------------------
    Nidx Widget::getPosLevel() const
    {
        if (!mParent)
            return 0;

        NCount z;
        WidgetList::iterator i, iend = mParent->mChilds.end();
        for (z = 0, i = mParent->mChilds.begin(); i != iend; ++i)
        {
            if (*i != this)
                ++z;
            else
                break;
        }
        if (i != iend)
            return z;
        else
            return 0;
    }
    //------------------------------------------------------------------------
    bool Widget::isFront(const Widget * obj) const
    {
        // children are always in front of their ancestors
        if (isAncestor(obj))
            return true;

        // conversely, ancestors are always behind their children
        if (obj->isAncestor(this))
            return false;

        const Widget * w1 = getCommonAncestor(obj);

        // seems not to be in same window hierarchy
        if (!w1)
            return false;

        const Widget * w2 = obj->getCommonAncestor(this);

        // at this point, w1 and w2 share the same parent.
        return w2->getPosLevel() > w1->getPosLevel();
    }
    //------------------------------------------------------------------------
    bool Widget::isBehind(const Widget * obj) const
    {
        return !isFront(obj);
    }
    //------------------------------------------------------------------------
    void Widget::moveFront(const Widget * obj)
    {
        if (!obj || !mParent || obj->mParent != mParent || obj == this ||
            obj->mTop != mTop || !mZOrder)
            return;

        WidgetList::iterator p = std::find(mParent->mChilds.begin(), mParent->mChilds.end(), this);
        mParent->mChilds.erase(p);

        WidgetList::iterator i = std::find(mParent->mChilds.begin(), mParent->mChilds.end(), obj);
        mParent->mChilds.insert(++i, this);

        ZOrderImpl();
    }
    //------------------------------------------------------------------------
    void Widget::moveBack(const Widget * obj)
    {
        if (!obj || !mParent || obj->mParent != mParent || obj == this ||
            obj->mTop != mTop || !mZOrder)
            return;

        WidgetList::iterator p(std::find(mParent->mChilds.begin(), mParent->mChilds.end(), this));
        mParent->mChilds.erase(p);

        WidgetList::iterator i(std::find(mParent->mChilds.begin(), mParent->mChilds.end(), obj));
        mParent->mChilds.insert(i, this);

        ZOrderImpl();
    }
    //------------------------------------------------------------------------
    void Widget::moveFront()
    {
        moveFrontImpl(false);
    }
    //------------------------------------------------------------------------
    void Widget::moveBack()
    {
        // if the window is active, de-activate it.
        if (isFinalActive())
        {
            ActivationEventArgs args(this);
            args.mActWidget = 0;
            onInactivate(&args);
        }

        // we only proceed if we have a parent (otherwise we can have no siblings)
        if (mParent)
        {
            if (mZOrder)
            {
                mParent->moveChildBack(this);
                ZOrderImpl();
            }
            mParent->moveBack();
        }
    }
    //------------------------------------------------------------------------
    void Widget::setWidth(const RelPosf & width)
    {
        setSize(RelPlaneSizef(width, getSize().mHeight));
    }
    //------------------------------------------------------------------------
    void Widget::setHeight(const RelPosf & height)
    {
        setSize(RelPlaneSizef(getSize().mWidth, height));
    }
    //------------------------------------------------------------------------
    void Widget::layout()
    {
        const PlaneSizef old_size(mPixelSize);
        mPixelSize = calcArea();

        if(mStyleID == 0)
            return;

        try
        {
            N_Only(Widget).getStyle(mStyleID)->layout(this);
        }
        catch(UnknownObjectException &)
        {
            N_Only(Log).log(_I18n("UI单元风格,名字: '") + N_conv(mStyleID) + _I18n("' 找不到!"));
        }

        mValidClipRect = (mPixelSize == old_size);
        mValidClipInnerRect = mInnerRect.isIntCache();

        if(mWidgetView)
            mWidgetView->layout();
    }
    //------------------------------------------------------------------------
    void Widget::setAreaImpl(const RelVector2f & pos, const RelPlaneSizef & size, bool fireEvents)
    {
        mValidClipRect = false;
        mValidClipInnerRect = false;
        mValidIntersectRect = false;
        mRect.resetCached();
        mInnerRect.resetCached();

        const PlaneSizef oldSize(mPixelSize);
        mArea.setSize(size);
        mPixelSize = calcArea();

        bool sized = (mPixelSize != oldSize);
        bool moved = (pos != RelVector2f(mArea.mLeft, mArea.mTop));

        if(moved)
            mArea.setPos(pos);

        if(fireEvents)
        {
            if(moved)
            {
                WidgetEventArgs args(this);
                onMove(&args);
            }

            if(sized)
            {
                WidgetEventArgs args(this);
                onSize(&args);
            }
        }

        //if (moved || sized)
        // FIXME: This is potentially wasteful
        mSheet->resetCursorOver();

        // update geometry position and clipping if nothing from above appears to
        // have done so already (NB: may be occasionally wasteful, but fixes bugs!)
        if(!mRect.isIntCache())
        {
            SheetContext ctx;
            getContext(ctx);
            updateImpl(&ctx);
        }
    }
    //------------------------------------------------------------------------
    void Widget::setParent(Container * c)
    {
        if(c != mParent)
        {
            if(mParent)
                mParent->remove(this);

            if(c)
            {
                c->add(this);
                mParent = c;
                mSheet = mParent->mSheet;
            }
            else
            {
                mParent = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    const Rectf & Widget::getClipArea() const
    {
        if(!mValidClipRect)
        {
            mClipRect = getClipAreaImpl();
            mValidClipRect = true;
        }
        return mClipRect;
    }
    //------------------------------------------------------------------------
    const Rectf & Widget::getInnerClipArea() const
    {
        if(!mValidClipInnerRect)
        {
            mClipInnerRect = getClipInnerAreaImpl();
            mValidClipInnerRect = true;
        }
        return mClipInnerRect;
    }
    //------------------------------------------------------------------------
    const Rectf & Widget::getIntersectArea() const
    {
        if(!mValidIntersectRect)
        {
            mIntersectRect = getIntersectAreaImpl();
            mValidIntersectRect = true;
        }
        return mIntersectRect;
    }
    //------------------------------------------------------------------------
    Rectf Widget::getClipAreaImpl() const
    {
        if(mParent && mAutoClip)
            return getArea().getIntCache().intersects(mParent->getClipArea(!isContainer()));

        PlaneSizei temps = getRenderSize();
        Rectf in(0, 0, temps.mWidth, temps.mHeight);
        return getArea().getIntCache().intersects(in);
    }
    //------------------------------------------------------------------------
    Rectf Widget::getClipInnerAreaImpl() const
    {
        if (mParent && mAutoClip)
            return getInnerArea().getIntCache().intersects(mParent->getClipArea(!isContainer()));

        PlaneSizei temps = getRenderSize();
        Rectf in(0, 0, temps.mWidth, temps.mHeight);
        return getInnerArea().getIntCache().intersects(in);
    }
    //------------------------------------------------------------------------
    Rectf Widget::getIntersectAreaImpl() const
    {
        // if clipped by parent wnd, hit test area is the intersection of our
        // outer rect with the parent's hit test area intersected with the
        // parent's clipper.
        if(mParent && mAutoClip)
        {
            return getArea().getIntCache().intersects(
                mParent->getIntersectArea().intersects(mParent->getClipArea(!isContainer())));
        }
        // not clipped to parent wnd, so get intersection with screen area.
        else
        {
            PlaneSizei temp = getRenderSize();
            Rectf urect(0, 0, temp.mWidth, temp.mHeight);
            return getArea().getIntCache().intersects(urect);
        }
    }
    //------------------------------------------------------------------------
    Rectf Widget::getAreaImpl(bool pixelAlign) const
    {
        const PlaneSizef pixel_size = pixelAlign ? getPixelSize() : calcArea(false);
        Rectf prect;

        if(mParent)
        {
            const CalcRect & base = mParent->getChildArea(true);
            prect = pixelAlign ? base.getIntCache() : base.getFloatCache();
        }
        else
        {
            PlaneSizei precttemp = getRenderSize();
            prect = Rectf(0, 0, precttemp.mWidth, precttemp.mHeight);
        }

        const PlaneSizef psize = prect.getSize();

        Vector2f offset = prect.min() + abs(RelVector2f(mArea.mLeft, mArea.mTop), psize, false);

        switch(getXAlign())
        {
        case HA_CENTRE:
            offset.x += (psize.mWidth - pixel_size.mWidth) * 0.5f;
            break;
        case HA_RIGHT:
            offset.x += psize.mWidth - pixel_size.mWidth;
            break;
        default:
            break;
        }

        switch(getYAlign())
        {
        case VA_CENTRE:
            offset.y += (psize.mHeight - pixel_size.mHeight) * 0.5f;
            break;
        case VA_BOTTOM:
            offset.y += psize.mHeight - pixel_size.mHeight;
            break;
        default:
            break;
        }

        if(mPixelAlign && pixelAlign)
        {
            offset = Vector2f(N_ToPixel(offset.x), N_ToPixel(offset.y));
        }
        return Rectf(offset, pixel_size + offset);
    }
    //------------------------------------------------------------------------
    Rectf Widget::getInnerAreaImpl(bool pixelAlign) const
    {
        return mWidgetView ? mWidgetView->getArea() :
            (pixelAlign ? mRect.getIntCache() : mRect.getFloatCache());
    }
    //------------------------------------------------------------------------
    void Widget::refreshImpl(bool r)
    {
        mRedraw = true;
        refreshFaceImpl();

        WidgetEventArgs args(this);
        onRefresh(&args);
    }
    //------------------------------------------------------------------------
    void Widget::queuePixelImpl()
    {
        getRenderText();
        if(isFinalVisible())
        {
            SheetContext ctx;
            getContext(ctx);

            if(mRedraw)
            {
                mPixelData->clear();

                WidgetEventArgs args(this);
                onPreRender(&args);

                if(mWidgetView)
                    mWidgetView->flush();
                else
                    renderImpl();

                onRenderEnd(&args);
                mRedraw = false;
            }
            if(ctx.mFace)
            {
                ctx.mFace->add(ctx.mQueue, mPixelData);
            }
            else
            {
                ctx.mSheet->add(ctx.mQueue, mPixelData);
            }
        }
    }
    //------------------------------------------------------------------------
    void Widget::updateImpl(SheetContext * ctx)
    {
        // position is the offset of the window on the dest surface.
        const Rectf ucrect(getArea().getIntCache());
        mPixelData->setOffset(Vector3f(
            ucrect.mLeft - ctx->mPosOffset.x,
            ucrect.mTop - ctx->mPosOffset.y, 0.0f));

        Rectf gclip(getClipArea());

        if(gclip.getWidth() != 0.0f && gclip.getHeight() != 0.0f)
            gclip -= Vector2f(ctx->mPosOffset.x, ctx->mPosOffset.y);

        mPixelData->setClip(Recti(gclip.mLeft, gclip.mTop, gclip.mRight, gclip.mBottom));
    }
    //------------------------------------------------------------------------
    bool Widget::isWrite(PropertyID pid) const
    {
        NonSerlProperty::const_iterator i = mNonSerlProperty.find(pid);
        // generally, there will always less banned properties than all properties,
        // so it makes sense to check that first before querying the property instance
        if(i != mNonSerlProperty.end())
        {
            return true;
        }

        // properties that don't write any XML code are implicitly banned

        // read-only properties are implicitly banned
        // (such stored information wouldn't be of any value in the XML anyways,
        // no way to apply it to the widget)
        PropertyCmd * instance = getProperty()->get(pid);
        return (!instance->isSerialize() || !instance->isWritable());
    }
    //------------------------------------------------------------------------
    bool Widget::isDefault(const PropertyCmd * obj) const
    {
        // if we have a looknfeel we examine it for defaults
        if(mStyleID != 0)
        {
            if(mParent && ! mParent->getStyle() == 0)
            {
                const Style * style = N_Only(Widget).getStyle(mParent->getStyle());

                // if this property is a target of a PropertyLink, we always report
                // as being at default.  NB: This check is only performed on the
                // immediate parent.
                if(style->getListener(obj->getID()))
                    return true;

                // for an auto-window see if the property is is set via a PropertyCmd
                // tag within the StyleWidget that defines it.
                if(mAutoGen)
                {
                    // find the widget component if possible
                    const StyleWidget * wstyle = style->getWidget(getID());
                    if(wstyle)
                    {
                        const AssignProperty * propinit = wstyle->getAssignProperty(obj->getID());
                        if(propinit)
                        {
                            String temp;
                            get(obj->getID(), temp);
                            return (temp == propinit->getValue());
                        }
                    }
                }
            }

            // if the looknfeel has a new default for this property we compare
            // against that
            const Style * style = N_Only(Widget).getStyle(mStyleID);
            const AssignProperty * propinit = style->getAssign(obj->getID());

            if(propinit)
            {
                String temp;
                get(obj->getID(), temp);
                return (temp == propinit->getValue());
            }
        }

        // we don't have a looknfeel with a new value for this property so we rely
        // on the hardcoded default
        return obj->isDefault(this);
    }
    //------------------------------------------------------------------------
    void Widget::notifyClipChange()
    {
        mValidClipRect = false;
        mValidClipInnerRect = false;
        mValidIntersectRect = false;
    }
    //------------------------------------------------------------------------
    void Widget::updateImpl(TimeDurMS cost)
    {
        if(mWidgetView)
            mWidgetView->update(cost);
    }
    //------------------------------------------------------------------------
    bool Widget::validImpl(const WidgetView *) const
    {
        return true;
    }
    //------------------------------------------------------------------------
    void Widget::onSize(const WidgetEventArgs * arg)
    {
        // 通知位置变化
        notifyPosChange();
        layout();
        refresh();
        signal(SizeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onParentSize(const WidgetEventArgs * arg)
    {
        mRect.resetCached();
        mInnerRect.resetCached();

        const PlaneSizef oldSize(mPixelSize);
        mPixelSize = calcArea();
        bool sized = (mPixelSize != oldSize) || !mInnerRect.isIntCache();

        bool moved = ((mArea.mLeft.mScale != 0) || (mArea.mTop.mScale != 0) ||
            (mHAlign != HA_LEFT) || (mVAlign != VA_TOP));

        if(moved)
        {
            WidgetEventArgs args(this);
            onMove(&args);
        }

        if(sized)
        {
            WidgetEventArgs args(this);
            onSize(&args);
        }

        layout();

        signal(ParentSizeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onMove(const WidgetEventArgs * arg)
    {
        notifyPosChange();
        mParent->onChildMove(arg);
        signal(MoveEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onAlpha(const WidgetEventArgs * arg)
    {
        refresh();
        signal(AlphaEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onID(const WidgetEventArgs * arg)
    {
        signal(IDEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onShow(const WidgetEventArgs * arg)
    {
        refresh();
        signal(ShowEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onHide(const WidgetEventArgs * arg)
    {
        if(isFinalActive())
            deactivate();

        refresh();
        signal(HideEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onEnable(const WidgetEventArgs * arg)
    {
        refresh();
        signal(EnableEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onDisable(const WidgetEventArgs * arg)
    {
        refresh();
        signal(DisableEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onClipMode(const WidgetEventArgs * arg)
    {
        refresh();
        notifyClipChange();
        signal(AutoClipModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onTopMode(const WidgetEventArgs * arg)
    {
        // we no longer want a total redraw here, instead we just get each window
        // to resubmit it's imagery to the UIManager.
        mSheet->redraw();
        signal(TopModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onZOrder(const WidgetEventArgs * arg)
    {
        // we no longer want a total redraw here, instead we just get each window
        // to resubmit it's imagery to the UIManager.
        mSheet->redraw();
        signal(ZOrderEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onAutoDestroyMode(const WidgetEventArgs * arg)
    {
        signal(AutoDestroyModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onMargin(const WidgetEventArgs * arg)
    {
        signal(MarginModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onEnvAlpha(const WidgetEventArgs * arg)
    {
        refresh();
        signal(EnvAlphaEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onHAlignMode(const WidgetEventArgs * arg)
    {
        notifyPosChange();
        signal(HAlignModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onVAlignMode(const WidgetEventArgs * arg)
    {
        notifyPosChange();
        signal(VAlignModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onName(const WidgetEventArgs * arg)
    {
        signal(NameEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onFocus(const WidgetEventArgs * arg)
    {
        signal(FocusEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onLost(const WidgetEventArgs * arg)
    {
        // handle restore of previous capture window as required.
        if(mRestorePreFocus && (mPreFocus != 0))
        {
            mPreFocus->onLost(arg);
            mPreFocus = 0;
        }
        signal(LostEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onRefresh(const WidgetEventArgs * arg)
    {
        signal(InvalidEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onPreRender(const WidgetEventArgs * arg)
    {
        signal(RenderBeginEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onRenderEnd(const WidgetEventArgs * arg)
    {
        signal(RenderEndEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onDestroy(const WidgetEventArgs * arg)
    {
        mDestroy = true;
        signal(DestroyStartEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onActivate(const ActivationEventArgs * arg)
    {
        mActive = true;
        refresh();
        signal(ActivateEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onInactivate(const ActivationEventArgs * arg)
    {
        mActive = false;
        refresh();
        signal(DeactivateEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onCursorEnter(const CursorEventArgs * arg)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++i)
            (*i)->onCursorEnter(arg);
        signal(CursorEnterEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onCursorLeave(const CursorEventArgs * arg)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++i)
            (*i)->onCursorLeave(arg);
        signal(CursorLeaveEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onCursorMove(const CursorEventArgs * arg)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++i)
            (*i)->onCursorMove(arg);
        signal(CursorMoveEvent, arg);
        if(mParent && this != mSheet->getModal() && mCursorSubmitEvent)
        {
            arg->mWidget = this;
            mParent->onCursorMove(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onCursorDrag(const CursorEventArgs * arg)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++i)
            (*i)->onCursorDrag(arg);
        signal(CursorDragEvent, arg);

        if(mParent && this != mSheet->getModal() && mCursorSubmitEvent)
        {
            arg->mWidget = this;
            mParent->onCursorDrag(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onButtonDown(const CursorEventArgs * arg)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++i)
            (*i)->onButtonDown(arg);
        signal(ButtonDownEvent, arg);

        if((arg->mMouseArg.mButton == MB_Left))
        {
            if(!isFinalActive())
            {
                ActivationEventArgs args(this);
                args.mActWidget = mSheet->getFocus();
                onActivate(&args);
                mSheet->setFocus(this);
            }
        }

        if(mParent && mCursorSubmitEvent)
        {
            arg->mWidget = this;
            mParent->onButtonDown(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onButtonUp(const CursorEventArgs * arg)
    {
        WidgetAideList::iterator i, iend = mAideList.end();
        for (i = mAideList.begin(); i != iend; ++i)
            (*i)->onButtonUp(arg);
        signal(ButtonUpEvent, arg);

        if(mParent && this != mSheet->getModal() && mCursorSubmitEvent)
        {
            arg->mWidget = this;
            mParent->onButtonUp(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onCursorTouch(const CursorEventArgs * arg)
    {
        signal(CursorTouchEvent, arg);

        if(mParent && this != mSheet->getModal() && mCursorSubmitEvent)
        {
            arg->mWidget = this;
            mParent->onCursorTouch(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onCursorMultiTouch(const CursorEventArgs * arg)
    {
        signal(CursorMultiTouchEvent, arg);

        if(mParent && this != mSheet->getModal() && mCursorSubmitEvent)
        {
            arg->mWidget = this;
            mParent->onCursorMultiTouch(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onKeyDown(const KeyEventArgs * arg)
    {
        signal(KeyDownEvent, arg);

        if(mParent && this != mSheet->getModal())
        {
            arg->mWidget = this;
            mParent->onKeyDown(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onKeyUp(const KeyEventArgs * arg)
    {
        signal(KeyUpEvent, arg);

        if(mParent && this != mSheet->getModal())
        {
            arg->mWidget = this;
            mParent->onKeyUp(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onChar(const KeyEventArgs * arg)
    {
        signal(CharEvent, arg);

        if(mParent && this != mSheet->getModal())
        {
            arg->mWidget = this;
            mParent->onChar(arg);
        }
    }
    //------------------------------------------------------------------------
    void Widget::onSheet(const SheetEventArgs * arg)
    {
        if(mParent)
            mSheet = mParent->mSheet;
        signal(RenderCtxEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onViewAttach(const WidgetEventArgs * arg)
    {
        if(!validImpl(mWidgetView))
            N_EXCEPT(InvalidRequest, _I18n("UI风格,ID:'") + N_conv(mWidgetView->getID()) +
                _I18n("' 不合适这个UI:") + N_conv(mType));

        mWidgetView->mSrc = this;
        mWidgetView->onAttach();
        signal(ViewAttachEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onViewDetach(const WidgetEventArgs * arg)
    {
        mWidgetView->onDetach();
        mWidgetView->mSrc = 0;
        signal(ViewDetachEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onStyleAttach(const StyleEventArgs * arg)
    {
        // Get style to initialise the widget as it needs.
        arg->mStyle->attach(arg->mWidget);
        signal(StyleAttachEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onStyleDetach(const StyleEventArgs * arg)
    {
        arg->mStyle->detach(arg->mWidget);
        signal(StyleDetachEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onFont(const FontEventArgs * arg)
    {
        if(!mWidgetView)
            mWidgetView->notifyFont(arg->mFont);

        // This was added to enable the Falagard FontSize to work
        // properly.  A better, more selective, solution would
        // probably be to do something funky with events ;)
        layout();

        refresh();
        signal(FontEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onText(const WidgetEventArgs * arg)
    {
        refresh();
        signal(TextEvent, arg);
    }
    //------------------------------------------------------------------------
    void Widget::onTextLayoutMode(const WidgetEventArgs * arg)
    {
        signal(TextLayoutModeEvent, arg);
    }
    //------------------------------------------------------------------------
    bool Widget::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdArea());
        dest->add(N_new CmdPosition());
        dest->add(N_new CmdYAlign());
        dest->add(N_new CmdXAlign());
        dest->add(N_new CmdSize());
        dest->add(N_new CmdMinSize());
        dest->add(N_new CmdMaxSize());
        dest->add(N_new CmdScaleMode());
        dest->add(N_new CmdScaleFactor());
        dest->add(N_new CmdPixelAligne());
        dest->add(N_new CmdPixelBuffer());
        dest->add(N_new CmdName());
        dest->add(N_new CmdNamePath());
        dest->add(N_new CmdAlpha());
        dest->add(N_new CmdAutoClip());
        dest->add(N_new CmdAutoDestroy());
        dest->add(N_new CmdID());
        dest->add(N_new CmdEnvAlpha());
        dest->add(N_new CmdVisible());
        dest->add(N_new CmdRestorePreFocus());
        dest->add(N_new CmdMultiClickEvent());
        dest->add(N_new CmdWidgetModel());
        dest->add(N_new CmdStyle());
        dest->add(N_new CmdUpdateMode());
        dest->add(N_new CmdCursorSubmitEvent());
        dest->add(N_new CmdMouseAutoRepeatEnabled());
        dest->add(N_new CmdAutoRepeatRate());
        dest->add(N_new CmdAutoRepeatDelay());
        dest->add(N_new CmdAutoWidget());
        dest->add(N_new CmdMargin());
        dest->add(N_new CmdTextParsingEnabled());
        dest->add(N_new CmdText());
        dest->add(N_new CmdTextColour());
        dest->add(N_new CmdFont());
        dest->add(N_new CmdRiseOnClickEnabled());
        dest->add(N_new CmdZOrderingEnabled());
        dest->add(N_new CmdAlwaysOnTop());
        return true;
    }
    //------------------------------------------------------------------------
}
}