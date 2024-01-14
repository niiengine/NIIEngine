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
#include "NiiUIButton.h"
#include "NiiUICursor.h"
#include "NiiUIWindow.h"
#include "NiiUIWidgetManager.h"
#include "NiiUICursorEventArgs.h"
#include "NiiUIContainer.h"
#include "NiiUITabControl.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    N_EVENT(Widget,         Button,        SelectChangeEvent,       0);
    N_EVENT(Widget,         Button,        EventCount,              1);
    //------------------------------------------------------------------------
    class _EngineInner CmdSelected : public PropertyCmd
    {
    public:
        CmdSelected() : PropertyCmd(N_PropertyUIButton_Select, _T("selected"), _I18n(""))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Button *>(own)->isSelect(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Button *>(own)->select(temp);
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Button *>(own)->isSelect();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Button *>(own)->select(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelected();
        }

        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Button
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Button::Button(WidgetID wid, FactoryID fid, Container * own, const String & name,
        ScriptTypeID stid, LangID lid) :
        Widget(stid, wid, fid, own, name, lid),
        mPress(false),
        mOver(false),
        mSelect(false)
    {
        notifySignal(Button::EventCount);
    }
    //------------------------------------------------------------------------
    Button::~Button()
    {

    }
    //------------------------------------------------------------------------
    void Button::select(bool b)
    {
        if (mSelect != b)
        {
            mSelect = b;
            refresh();

            WidgetEventArgs args(this);
            onSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    bool Button::initCmd(PropertyCmdSet * dest)
    {
        Widget::initCmd(dest);
        dest->add(N_new CmdSelected());
        return true;
    }
    //------------------------------------------------------------------------
    void Button::update(const Vector2f & cursorpos)
    {
        bool pre = mOver;

        Window * act = mSheet->getActive();
        if(act)
        {
            mOver = ((act->getActiveChild() == this || (act->isTransmitCursorEvent() &&
                isAncestor(act))) && intersects(cursorpos));
        }
        else
        {
            mOver = (mSheet->getCursorOver() == this);
        }
        if(pre != mOver)
        {
            refresh();
        }
    }
    //------------------------------------------------------------------------
    void Button::onCursorMove(const CursorEventArgs * arg)
    {
        // this is needed to discover whether mouse is in the widget area or not.
        // The same thing used to be done each frame in the rendering method,
        // but in this version the rendering method may not be called every frame
        // so we must discover the internal widget state here - which is actually
        // more efficient anyway.

        // base class processing
        Widget::onCursorMove(arg);
        update(Vector2f(arg->mLocalX, arg->mLocalY));
    }
    //------------------------------------------------------------------------
    void Button::onCursorLeave(const CursorEventArgs * arg)
    {
        Widget::onCursorLeave(arg);
        mOver = false;
        refresh();
    }
    //------------------------------------------------------------------------
    void Button::onButtonUp(const CursorEventArgs * arg)
    {
        Widget::onButtonUp(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            lost();
        }
    }
    //------------------------------------------------------------------------
    void Button::onButtonDown(const CursorEventArgs * arg)
    {
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            if(focus())
            {
                mPress = true;
                update(Vector2f(arg->mLocalX, arg->mLocalY));
                refresh();
            }

            if (Window * obj = mSheet->getActive())
            {
                // was mouse released over this widget
                // (use mouse position, as e.mX e.mY is already unprojected)
                if (this == obj->getTarget(mSheet->getCursor()->getPos()))
                {
                    select(true);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Button::press(bool b)
    {
        mPress = b;

        if(!b)
            update(mParent->getWindow()->affine(mSheet->getCursor()->getPos()));
        else
            mOver = true;

        refresh();
    }
    //------------------------------------------------------------------------
    void Button::onSelect(const WidgetEventArgs * arg)
    {
        signal(SelectChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Button::onLost(const WidgetEventArgs * arg)
    {
        Widget::onLost(arg);
        mPress = false;
        mSheet->resetCursorOver();
        refresh();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ButtonView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(WidgetView, ButtonView, PushState, 0);
    N_STATE(WidgetView, ButtonView, PushOffState, 1);
    N_STATE(WidgetView, ButtonView, StateCount, 2);
    //--------------------------------------------------------------------------
    ButtonView::ButtonView(WidgetViewlID wsid) :
        WidgetView(wsid, N_VIEW_Button)
    {
    }
    //--------------------------------------------------------------------------
    void ButtonView::flush()
    {
        Button * temp = static_cast<Button *>(mSrc);
        const Style * style = getStyle();

        bool norm = false;
        StateID state;

        if (temp->isFinalDisable())
        {
            state = DisableState;
        }
        else if (temp->isPress())
        {
            state = temp->isHover() ? PushState : PushOffState;
        }
        else if (temp->isHover())
        {
            state = HoverState;
        }
        else
        {
            state = NormalState;
            norm = true;
        }

        if (!norm && !style->isStateExist(alter(state)))
        {
            state = NormalState;
        }

        style->getLayout(alter(state)).render(temp);
    }
    //--------------------------------------------------------------------------
    StateID ButtonView::alter(StateID pid) const
    {
        return pid;
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // PushButton
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(Button,        PushButton,        PushEvent,          0);
    N_EVENT(Button,        PushButton,        EventCount,         1);
    //------------------------------------------------------------------------
    PushButton::PushButton(WidgetID wid, FactoryID fid, Container * own, const String & name,
        ScriptTypeID stid, LangID lid) :
        Button(wid, fid, own, name, stid, lid)
    {
        notifySignal(PushButton::EventCount);
    }
    //------------------------------------------------------------------------
    PushButton::~PushButton()
    {
    }
    //------------------------------------------------------------------------
    void PushButton::onPush(const WidgetEventArgs * arg)
    {
        signal(PushEvent, arg);
    }
    //------------------------------------------------------------------------
    void PushButton::onButtonUp(const CursorEventArgs * arg)
    {
        if((arg->mMouseArg.mButton == MB_Left) && mPress)
        {
            Window * win = mSheet->getActive();
            if(win)
            {
                // if mouse was released over this widget
                // (use position from mouse, as e.mX e.mY has been unprojected)
                if(this == win->getTarget(mSheet->getCursor()->getPos()))
                {
                    // fire event
                    WidgetEventArgs args(this);
                    onPush(&args);
                }
            }
        }
        Button::onButtonUp(arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PosButton
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdHotTracked : public PropertyCmd
    {
    public:
        CmdHotTracked() : PropertyCmd(N_PropertyUIPosButton_PosEvent, _T("HotTracked"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const PosButton *>(own)->isPosEvent(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<PosButton *>(own)->setPosEvent(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const PosButton *>(own)->isPosEvent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<PosButton *>(own)->setPosEvent(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHotTracked();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdVertRange : public PropertyCmd
    {
    public:
        CmdVertRange() : PropertyCmd(N_PropertyUIPosButton_YRange, _T("vert_range"), _T("min:[NIIf] max:[NIIf]"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf min, max;
            static_cast<const PosButton *>(own)->getYRange(min, max);
            Ntchar buff[64];
            Nsntprintf(buff, sizeof(buff), _T("%f %f"), min, max);
            out = buff;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf min = 0, max = 0;
            Nsscanf(val.c_str(), _T("%f %f"), &min, &max);
            static_cast<PosButton *>(own)->setYRange(min, max);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Vector2f * temp = (Vector2f *)out;
            static_cast<const PosButton *>(own)->getYRange(temp->x, temp->y);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Vector2f * temp = (Vector2f *)in;
            static_cast<PosButton *>(own)->setYRange(temp->x, temp->y);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertRange();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 1.0");
        }
    };

    class _EngineInner CmdHorzRange : public PropertyCmd
    {
    public:
        CmdHorzRange() : PropertyCmd(N_PropertyUIPosButton_XRange, _T("horz_range"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf min, max;
            static_cast<const PosButton * >(own)->getXRange(min, max);
            Ntchar buff[64];
            Nsntprintf(buff, sizeof(buff), _T("%f %f"), min, max);
            out = buff;
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf min = 0, max = 0;
            Nsscanf(val.c_str(), _T("%f %f"), &min, &max);
            static_cast<PosButton * >(own)->setXRange(min, max);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Vector2f * temp = (Vector2f *)out;
            static_cast<const PosButton * >(own)->getXRange(temp->x, temp->y);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Vector2f * temp = (Vector2f *)in;
            static_cast<PosButton * >(own)->setXRange(temp->x, temp->y);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzRange();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 1.0");
        }
    };

    class _EngineInner CmdVertFree : public PropertyCmd
    {
    public:
        CmdVertFree() : PropertyCmd(N_PropertyUIPosButton_YMoveEnable, _T("vert_free"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const PosButton * >(own)->isYMove(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<PosButton * >(own)->setYMove(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const PosButton * >(own)->isYMove();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<PosButton * >(own)->setYMove(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVertFree();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdHorzFree : public PropertyCmd
    {
    public:
        CmdHorzFree() : PropertyCmd(N_PropertyUIPosButton_XMoveEnable, _T("horz_free"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const PosButton * >(own)->isXMove(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<PosButton * >(own)->setXMove(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const PosButton * >(own)->isXMove();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<PosButton * >(own)->setXMove(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorzFree();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(PushButton,           PosButton,            MoveBeginEvent,      0);
    N_EVENT(PushButton,           PosButton,            MoveEndEvent,        1);
    N_EVENT(PushButton,           PosButton,            PosChangeEvent,      2);
    N_EVENT(PushButton,           PosButton,            EventCount,          3);
    //------------------------------------------------------------------------
    PosButton::PosButton(WidgetID wid, FactoryID fid, Container * own, const String & name, 
        LangID lid) :
        PushButton(wid, fid, own, name, N_CmdObj_PosButton, lid),
        mVMin(0.0f),
        mVMax(1.0f),
        mHMin(0.0f),
        mHMax(1.0f),
        mDrag(false),
        mYMove(false),
        mXMove(false),
        mSynEvent(true)
    {
        notifySignal(PosButton::EventCount);
    }
    //------------------------------------------------------------------------
    PosButton::~PosButton()
    {
    }
    //------------------------------------------------------------------------
    void PosButton::setYRange(NIIf min, NIIf max)
    {
        // ensure min <= max, swap if not.
        if(min > max)
        {
            NIIf tmp = min;
            max = min;
            min = tmp;
        }

        mVMax = max;
        mVMin = min;

        // validate current position.
        NIIf cp = (calcParentArea().mHeight != 0.0f) ? getPos().y.mOffset / calcParentArea().mHeight + getPos().y.mScale : 0.0f;

        if(cp < min)
        {
            setPos(RelVector2f(getPos().x, RelPosf(min, 0)));
        }
        else if(cp > max)
        {
            setPos(RelVector2f(getPos().x, RelPosf(max, 0)));
        }
    }
    //------------------------------------------------------------------------
    void PosButton::setXRange(NIIf min, NIIf max)
    {
        PlaneSizef parentSize(calcParentArea());
        // ensure min <= max, swap if not.
        if (min > max)
        {
            NIIf tmp = min;
            max = min;
            min = tmp;
        }

        mHMax = max;
        mHMin = min;
        // validate current position. 错误需要修正
        NIIf cp = getPos().x.abs(parentSize.mWidth);
        if (cp < min)
        {
            setPos(RelVector2f(RelPosf(0, min), getPos().y));
        }
        else if (cp > max)
        {
            setPos(RelVector2f(RelPosf(0, max), getPos().y));
        }
    }
    //------------------------------------------------------------------------
    void PosButton::onMoveBegin(const WidgetEventArgs * arg)
    {
        signal(MoveBeginEvent, arg);
    }
    //------------------------------------------------------------------------
    void PosButton::onMoveEnd(const WidgetEventArgs * arg)
    {
        signal(MoveEndEvent, arg);
    }
    //------------------------------------------------------------------------
    void PosButton::onPosChange(const WidgetEventArgs * arg)
    {
        signal(PosChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void PosButton::onCursorMove(const CursorEventArgs * arg)
    {
        PushButton::onCursorMove(arg);

        // only react if we are being dragged
        if(mDrag)
        {
            PlaneSizef parentSize(calcParentArea());

            Vector2f delta(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
            NIIf hmin, hmax, vmin, vmax;

            hmin = mHMin;
            hmax = mHMax;
            vmin = mVMin;
            vmax = mVMax;

            // calculate amount of movement
            delta -= mDragPos;
            delta.x /= parentSize.mWidth;
            delta.y /= parentSize.mHeight;

            //
            // Calculate new (pixel) position for thumb
            //
            RelVector2f newPos(getPos());

            if(mXMove)
            {
                newPos.x.mScale += delta.x;

                // limit value to within currently set range
                newPos.x.mScale = (newPos.x.mScale < hmin) ? hmin : (newPos.x.mScale > hmax) ? hmax : newPos.x.mScale;
            }

            if(mYMove)
            {
                newPos.y.mScale += delta.y;

                // limit new position to within currently set range
                newPos.y.mScale = (newPos.y.mScale < vmin) ? vmin : (newPos.y.mScale > vmax) ? vmax : newPos.y.mScale;
            }

            // update thumb position if needed
            if(newPos != getPos())
            {
                setPos(newPos);

                // send notification as required
                if(mSynEvent)
                {
                    WidgetEventArgs args(this);
                    onPosChange(&args);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void PosButton::onButtonDown(const CursorEventArgs * arg)
    {
        PushButton::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            mDrag = true;
            mDragPos = Vector2f(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
            // trigger tracking started event
            WidgetEventArgs args(this);
            onMoveBegin(&args);
        }
    }
    //------------------------------------------------------------------------
    void PosButton::onLost(const WidgetEventArgs * arg)
    {
        PushButton::onLost(arg);

        mDrag = false;

        // trigger tracking ended event
        WidgetEventArgs _args(this);
        onMoveEnd(&_args);

        // send notification whenever thumb is released
        onPosChange(&_args);
    }
    //------------------------------------------------------------------------
    void PosButton::getYRange(NIIf & min, NIIf & max) const
    {
        min = mVMin;
        max = mVMax;
    }
    //------------------------------------------------------------------------
    void PosButton::getXRange(NIIf & min, NIIf & max) const
    {
        min = mHMin;
        max = mHMax;
    }
    //------------------------------------------------------------------------
    bool PosButton::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdHotTracked());
        dest->add(N_new CmdVertRange());
        dest->add(N_new CmdHorzRange());
        dest->add(N_new CmdVertFree());
        dest->add(N_new CmdHorzFree());
        return true;
    }
    //-----------------------------------------------------------------------
    void PosButton::disableWrite()
    {
        PushButton::disableWrite();

        setWrite(N_PropertyUIPosButton_YRange,          false);
        setWrite(N_PropertyUIPosButton_XRange,          false);
        setWrite(N_PropertyUIPosButton_YMoveEnable,     false);
        setWrite(N_PropertyUIPosButton_XMoveEnable,     false);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RadioButton
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdGroupID : public PropertyCmd
    {
    public:
        CmdGroupID() : PropertyCmd(11, _T("group_id"), _I18n(""))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const RadioButton *>(own)->getGroup(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<RadioButton *>(own)->setGroup(temp);
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const RadioButton *>(own)->getGroup();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<RadioButton *>(own)->setGroup(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdGroupID();
        }
        
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((Nui32)0, out);
        }
    };
    //------------------------------------------------------------------------
    RadioButton::RadioButton(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Button(wid, fid, own, name, N_CmdObj_RadioButton, lid),
        mGID(0)
    {
        setupCmd(N_CmdObj_RadioButton);
    }
    //------------------------------------------------------------------------
    void RadioButton::setGroup(GroupID gid)
    {
        mGID = gid;

        if(mSelect)
            verify();
    }
    //------------------------------------------------------------------------
    void RadioButton::verify() const
    {
        if(!mParent)
            return;

        NCount i, iend = mParent->getCount();
        for(i = 0; i < iend; ++i)
        {
            if(RadioButton * rb = dynamic_cast<RadioButton *>(mParent->get(i)))
            {
                if(rb->isSelect() && (rb != this) && (rb->getGroup() == mGID))
                {
                    rb->select(false);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    RadioButton * RadioButton::getGroupSelect() const
    {
        // Only search we we are a child window
        if(mParent)
        {
            NCount i, iend = mParent->getCount();
            for(i = 0; i < iend; ++i)
            {
                // is this child same type as we are?
                if(mParent->get(i)->getType() == getType())
                {
                    RadioButton * rb = static_cast<RadioButton *>(mParent->get(i));

                    // is child same group and selected?
                    if(rb->isSelect() && (rb->getGroup() == mGID))
                    {
                        // return the matching RadioButton pointer (may even be 'this').
                        return rb;
                    }
                }
            }
        }
        // no selected button attached to this window is in same group
        return 0;
    }
    //------------------------------------------------------------------------
    bool RadioButton::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdGroupID());
        return true;
    }
    //------------------------------------------------------------------------
    void RadioButton::onSelect(const WidgetEventArgs * arg)
    {
        if (mSelect)
            verify();

        Button::onSelect(arg);
    }
    //------------------------------------------------------------------------
    /*- Enable            - Rendering for when the togglebutton is neither pushed or has the mouse hovering over it.
    - Hover             - Rendering for then the togglebutton has the mouse hovering over it.
    - Push            - Rendering for when the togglebutton is not selected, is pushed and has the mouse over it.
    - PushOff         - Rendering for when the togglebutton is not selected, is pushed and the mouse is not over it.
    - Disable          - Rendering for when the togglebutton is not selected and is disabled.
    - SelectedEnable    - Rendering for when the togglebutton is selected and is neither pushed or has the mouse hovering over it.
    - SelectedHover     - Rendering for then the togglebutton is selected and has the mouse hovering over it.
    - SelectedPush    - Rendering for when the togglebutton is selected, is pushed and has the mouse over it.
    - SelectedPushOff - Rendering for when the togglebutton is selected, is pushed and the mouse is not over it.
    - SelectedDisable  - Rendering for when the togglebutton is selected and is disabled.*/
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RadioButtonView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(ButtonView, RadioButtonView, SelectEnableState,     0);
    N_STATE(ButtonView, RadioButtonView, SelectHoverState,      1);
    N_STATE(ButtonView, RadioButtonView, SelectPushState,       2);
    N_STATE(ButtonView, RadioButtonView, SelectPushOffState,    3);
    N_STATE(ButtonView, RadioButtonView, SelectDisableState,    4);
    N_STATE(ButtonView, RadioButtonView, StateCount,            5);
    //------------------------------------------------------------------------
    RadioButtonView::RadioButtonView(WidgetViewlID wsid) :
        ButtonView(wsid)
    {
    }
    //------------------------------------------------------------------------
    StateID RadioButtonView::alter(StateID pid) const
    {
        bool select = static_cast<Button *>(mSrc)->isSelect();
        if (select)
        {
            if (pid == NormalState)
                return SelectEnableState;
            else if (pid == HoverState)
                return SelectHoverState;
            else if (pid == PushState)
                return SelectPushState;
            else if (pid == PushOffState)
                return SelectPushOffState;
            else if (pid == DisableState)
                return SelectDisableState;
            else
                return pid;
        }
        return pid;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TabButton
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(Button, TabButton, ClickEvent,  0);
    N_EVENT(Button, TabButton, DragEvent,   1);
    N_EVENT(Button, TabButton, ScrollEvent, 2);
    N_EVENT(Button, TabButton, EventCount,  3);
    //------------------------------------------------------------------------
    TabButton::TabButton(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Button(wid, fid, own, name, N_CmdObj_TabButton, lid),
        mDrag(false)
    {
        notifySignal(TabButton::EventCount);
    }
    //------------------------------------------------------------------------
    TabButton::~TabButton()
    {
    }
    //------------------------------------------------------------------------
    void TabButton::select(bool b)
    {
        if(b != mSelect)
        {
            mSelect = b;
            refresh();
        }
    }
    //------------------------------------------------------------------------
    void TabButton::setDest(Container * c)
    {
        mDest = c;
        // Copy initial text
        setText(c->getText());
        // Parent control will keep text up to date, since changes affect layout
    }
    //------------------------------------------------------------------------
    void TabButton::onClick(const WidgetEventArgs * arg)
    {
        signal(ClickEvent, arg);
    }
    //------------------------------------------------------------------------
    void TabButton::onButtonDown(const CursorEventArgs * arg)
    {
        Button::onButtonDown(arg);
        if(arg->mMouseArg.mButton == MB_Middle)
        {
            focus();
            mDrag = true;

            signal(DragEvent, arg);
        }
    }
    //------------------------------------------------------------------------
    void TabButton::onButtonUp(const CursorEventArgs * arg)
    {
        if((arg->mMouseArg.mButton == MB_Left) && mPress)
        {
            Window * win = mSheet->getActive();

            if(win)
            {
                // if mouse was released over this widget
                // (use mouse position, as e.mX, e.mY has been unprojected)
                if(this == win->getTarget(mSheet->getCursor()->getPos()))
                {
                    // fire event
                    WidgetEventArgs args(this);
                    onClick(&args);
                }
            }
        }
        else if(arg->mMouseArg.mButton == MB_Middle)
        {
            mDrag = false;
            lost();
        }

        Button::onButtonUp(arg);
    }
    //------------------------------------------------------------------------
    void TabButton::onCursorMove(const CursorEventArgs * arg)
    {
        if(mDrag)
        {
            signal(DragEvent, arg);
        }

        Button::onCursorMove(arg);
    }
    //------------------------------------------------------------------------
    void TabButton::onCursorDrag(const CursorEventArgs * arg)
    {
        signal(ScrollEvent, arg);

        Button::onCursorMove(arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TabButtonView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(WidgetView, TabButtonView, BottomEnableState, 0);
    N_STATE(WidgetView, TabButtonView, BottomDisableState, 1);
    N_STATE(WidgetView, TabButtonView, BottomSelectState, 2);
    N_STATE(WidgetView, TabButtonView, BottomPushState, 3);
    N_STATE(WidgetView, TabButtonView, BottomHoverState, 4);
    N_STATE(WidgetView, TabButtonView, TopEnableState, 5);
    N_STATE(WidgetView, TabButtonView, TopDisableState, 6);
    N_STATE(WidgetView, TabButtonView, TopSelectState, 7);
    N_STATE(WidgetView, TabButtonView, TopPushState, 8);
    N_STATE(WidgetView, TabButtonView, TopHoverState, 9);
    N_STATE(WidgetView, TabButtonView, StateCount, 10);
    //------------------------------------------------------------------------
    TabButtonView::TabButtonView(WidgetViewlID wsid) :
        WidgetView(wsid, N_VIEW_TabButton)
    {
    }
    //------------------------------------------------------------------------
    void TabButtonView::flush()
    {
        StateID pid;
        const Style * style = getStyle();
        TabButton * w = static_cast<TabButton *>(mSrc);
        TabControl * tc = w->getParent() ? static_cast<TabControl*>(w->getParent()) : 0;
        bool top = tc && tc->getTabType() == TabControl::Top;
        if (top)
        {
            if (w->isFinalDisable())
                pid = TopDisableState;
            else if (w->isSelect())
                pid = TopSelectState;
            else if (w->isPress())
                pid = TopPushState;
            else if (w->isHover())
                pid = TopHoverState;
            else
                pid = TopEnableState;
        }
        else
        {
            if (w->isFinalDisable())
                pid = BottomDisableState;
            else if (w->isSelect())
                pid = BottomSelectState;
            else if (w->isPress())
                pid = BottomPushState;
            else if (w->isHover())
                pid = BottomHoverState;
            else
                pid = BottomEnableState;
        }

        if (!style->isStateExist(pid))
        {
            if (top)
            {
                if (!style->isStateExist(TopEnableState))
                    pid = NormalState;
            }
            else
            {
                if (!style->isStateExist(BottomEnableState))
                    pid = NormalState;
            }
        }

        style->getLayout(pid).render(w);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ToggleButton
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdToggle : public PropertyCmd
    {
    public:
        CmdToggle() : PropertyCmd(N_PropertyUIToggleButton_On, _T("selected"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ToggleButton *>(own)->isOn(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ToggleButton *>(own)->on(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ToggleButton *>(own)->isOn();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ToggleButton *>(own)->on(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdToggle();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Button, ToggleButton, StateOnEvent,         0);
    N_EVENT(Button, ToggleButton, EventCount,           1);
    //------------------------------------------------------------------------
    ToggleButton::ToggleButton(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Button(wid, fid, own, name, N_CmdObj_ToggleButton, lid),
        mOn(false)
    {
        notifySignal(ToggleButton::EventCount);
        setupCmd(N_CmdObj_ToggleButton);
    }
    //------------------------------------------------------------------------
    void ToggleButton::on(bool b)
    {
        if(mOn != b)
        {
            mOn = b;
            refresh();

            WidgetEventArgs args(this);
            onStateOn(&args);
        }
    }
    //------------------------------------------------------------------------
    bool ToggleButton::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdToggle());
        return true;
    }
    //------------------------------------------------------------------------
    void ToggleButton::onStateOn(const WidgetEventArgs * arg)
    {
        signal(StateOnEvent, arg);
    }
    //------------------------------------------------------------------------
    void ToggleButton::onButtonUp(const CursorEventArgs * arg)
    {
        if(arg->mMouseArg.mButton == MB_Left && mPress)
        {
            if(Window * win = mSheet->getActive())
            {
                // was mouse released over this widget
                // (use mouse position, as e.mX e.mY is already unprojected)
                if(this == win->getTarget(mSheet->getCursor()->getPos()))
                {
                    on(mOn ^ true);
                }
            }
        }
        Button::onButtonUp(arg);
    }
    //------------------------------------------------------------------------
    /*- Enable            - Rendering for when the togglebutton is neither pushed or has the mouse hovering over it.
    - Hover             - Rendering for then the togglebutton has the mouse hovering over it.
    - Push           - Rendering for when the togglebutton is not selected, is pushed and has the mouse over it.
    - PushOff         - Rendering for when the togglebutton is not selected, is pushed and the mouse is not over it.
    - Disable          - Rendering for when the togglebutton is not selected and is disabled.
    - SelectedEnable    - Rendering for when the togglebutton is selected and is neither pushed or has the mouse hovering over it.
    - SelectedHover     - Rendering for then the togglebutton is selected and has the mouse hovering over it.
    - SelectedPush    - Rendering for when the togglebutton is selected, is pushed and has the mouse over it.
    - SelectedPushOff - Rendering for when the togglebutton is selected, is pushed and the mouse is not over it.
    - SelectedDisable  - Rendering for when the togglebutton is selected and is disabled. */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ToggleButtonView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(ButtonView, ToggleButtonView, SelectEnableState,        0);
    N_STATE(ButtonView, ToggleButtonView, SelecteHoverState,        1);
    N_STATE(ButtonView, ToggleButtonView, SelectePushState,         2);
    N_STATE(ButtonView, ToggleButtonView, SelectePushOffState,      3);
    N_STATE(ButtonView, ToggleButtonView, SelecteDisableState,      4);
    N_STATE(ButtonView, ToggleButtonView, StateCount,               5);
    //------------------------------------------------------------------------
    ToggleButtonView::ToggleButtonView(WidgetViewlID wsid) :
        ButtonView(wsid)
    {
    }
    //------------------------------------------------------------------------
    StateID ToggleButtonView::alter(StateID pid) const
    {
        bool select = static_cast<Button *>(mSrc)->isSelect();
        if (select)
        {
            if (pid == NormalState)
                return SelectEnableState;
            else if (pid == HoverState)
                SelecteHoverState;
            else if (pid == PushState)
                SelectePushState;
            else if (pid == PushOffState)
                SelectePushOffState;
            else if (pid == DisableState)
                SelecteDisableState;
            else
                return pid;
        }
        return pid;
    }
    //------------------------------------------------------------------------
}
}