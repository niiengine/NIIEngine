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
#include "NiiUIWindow.h"
#include "NiiUIManager.h"
#include "NiiLogManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiPixelManager.h"
#include "NiiUICursor.h"
#include "NiiUITitlebar.h"
#include "NiiUIButton.h"
#include "NiiUIWidgetList.h"

#define ClientWithTitleWithFrame    1
#define ClientWithTitleNoFrame      2
#define ClientNoTitleWithFrame      3
#define ClientNoTitleNoFrame        4

// URGENT FIXME: I commented instances of PixelAligned in here, I think they are not necessary but it should be checked!

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdMousePassThroughEnabled : public PropertyCmd
    {
    public:
        CmdMousePassThroughEnabled() : PropertyCmd(N_PropertyUIWindow_Overgo, _T("mouse_pass_through_enabled"),
            _T("鼠标事件直接穿透本窗体"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Window *>(own)->isOvergo(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Window *>(own)->setOvergo(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Window *>(own)->isOvergo();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Window *>(own)->setOvergo(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMousePassThroughEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdAutoRenderingSurface : public PropertyCmd
    {
    public:
        CmdAutoRenderingSurface() : PropertyCmd(N_PropertyUIWindow_AutoRender, _T("auto_rendering_surface"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Window *>(own)->isAutoRender(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Window *>(own)->setAutoRender(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Window *>(own)->isAutoRender();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Window *>(own)->setAutoRender(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoRenderingSurface();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdRotation : public PropertyCmd
    {
    public:
        CmdRotation() : PropertyCmd(N_PropertyUIWindow_Rotation, _T("rotation"),
            _T("Property to get/set the Window's rotation. Value is a quaternion: "
            "\"w:[w_float] x:[x_float] y:[y_float] z:[z_float]\""
            "or \"x:[x_float] y:[y_float] z:[z_float]\" to convert from Euler angles (in degrees)."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Window *>(own)->getRotation(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Quaternion temp;
            StrConv::conv(val, temp);
            static_cast<Window *>(own)->setRotation(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Quaternion * temp = (Quaternion *)out;
            *temp = static_cast<const Window *>(own)->getRotation();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Quaternion * temp = (Quaternion *)in;
            static_cast<Window *>(own)->setRotation(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRotation();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("1.0 0.0 0.0 0.0");
        }
    };
    //------------------------------------------------------------------------
    Window::Window(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet,
        const String & name, ScriptTypeID stid, LangID lid) :
            Container(wid, fid, own, name, stid, lid),
            mRender(0),
            mAutoCtx(false),
            mOvergo(false)
    {
        if (sheet == 0)
            mSheet = N_Only(UI).getMainSheet();
        else
            mSheet = sheet;
        mWindow = this;
    }
    //------------------------------------------------------------------------
    Window::~Window()
    {

    }
    //------------------------------------------------------------------------
    void Window::setRotation(const Quaternion & rot)
    {
        mRot = rot;

        WidgetEventArgs args(this);
        onRotate(&args);
    }
    //------------------------------------------------------------------------
    void Window::setModal(bool b)
    {
        if(isModal() == b)
            return;

        if(b)
        {
            activate();
            mSheet->setModal(this);
        }
        // clear the modal target
        else
        {
            if(isModal())
            {
                deactivate();
                mSheet->setModal(0);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Window::isModal() const
    {
        return(mSheet->getModal() == this);
    }
    //------------------------------------------------------------------------
    Vector2f Window::affine(const Vector2f & pos) const
    {
        if(mRender)
        {
            // get first target UISubSheet
            UISubSheet * temp = mRender;

            // setup for loop
            Vector2f out(pos);

            while(temp)
            {
                // unproject the point for the current rw
                const Vector2f in(out);
                temp->affine(in, out);

                if(mParent)
                {
                    temp = mParent->getWindow()->mRender;
                }
                else
                {
                    temp = 0;
                }
            }
        }
        return pos;
    }
    //------------------------------------------------------------------------
    bool Window::isPos(const Vector2f & pos, bool disable) const
    {
        return !isOvergo() && intersects(pos, disable);
    }
    //------------------------------------------------------------------------
    void Window::activate()
    {
        mSheet->setActive(this);
        Container::activate();
        moveFront();
    }
    //-----------------------------------------------------------------------
    void Window::setAutoRender(bool b)
    {
        if(b)
        {
            allocRenderCtx();
        }
        else
        {
            freeRenderCtx();
        }

        // while the actal area on screen may not have changed, the arrangement of
        // surfaces and geometry did...
        notifyPosChange();
    }
    //-----------------------------------------------------------------------
    void Window::setRender(UISubSheet * i)
    {
        if(mRender == i)
            return;

        if(mAutoCtx)
            setAutoRender(false);

        mRender = i;

        if(mRender)
        {
            SheetEventArgs arg(this, mSheet, mRender);
            onSheet(&arg);
            notifyPosChange();
        }
    }
    //------------------------------------------------------------------------
    void Window::setSheet(UISheet * obj)
    {
        if(mSheet == obj)
            return;

        if(obj == 0)
            mSheet = N_Only(UI).getMainSheet();
        else
            mSheet = obj;

        if(mRender != 0)
            mSheet->attach(mRender);

        SheetEventArgs arg(this, mSheet, 0);
        onSheet(&arg);
    }
    //-----------------------------------------------------------------------
    void Window::allocRenderCtx()
    {
        if(!mAutoCtx)
        {
            mAutoCtx = true;

            if(mParent)
            {
                UISubSheet * ti = mParent->getWindow()->mRender;
                if(ti)
                    ti->create(mRender);
                else
                    mSheet->create(mRender);
            }
            else
            {
                mSheet->create(mRender);
            }
            SheetEventArgs arg(this, mSheet, mRender);
            onSheet(&arg);

            // set size and position of UISubSheet
            mRender->setSize(getPixelSize());
            mRender->setPos(getArea().getIntCache().getPos());

            mSheet->redraw();
        }
    }
    //------------------------------------------------------------------------
    void Window::freeRenderCtx()
    {
        if(mAutoCtx && mRender)
        {
            UISubSheet * pre = mRender;
            mAutoCtx = false;
            mRender = 0;
            SheetEventArgs arg(this, mSheet, 0);
            onSheet(&arg);

            UISubSheet * rel = pre->getRel();
            if(rel)
                rel->destroy(pre);
            else
                pre->getSheet()->destroy(pre);

            mSheet->redraw();
        }
    }
    //------------------------------------------------------------------------
    void Window::onRotate(const WidgetEventArgs * arg)
    {
        signal(RotateEvent, arg);
        // if we have no surface set, enable the auto surface
        if(!mRender)
        {
            N_Only(Log).log(_I18n("需要独立渲染UI单元: ") + mName + _I18n(" 才能有旋转效果"));

            setAutoRender(true);

            if(!mRender)
            {
                N_Only(Log).log(_I18n("UI单元: ") + mName + _I18n("' 没有独立的渲染面,旋转效果无效."));
                return;
            }
        }

        // Checks / setup complete!  Now we can finally set the rotation.
        mRender->setRotation(mRot);
        mRender->setOrigin(Vector3f(mPixelSize.mWidth / 2.0f, mPixelSize.mHeight / 2.0f, 0.0f));
    }
    //------------------------------------------------------------------------
    void Window::updateImpl(TimeDurMS t)
    {
        Container::updateImpl(t);

        if(mRender)
            mRender->update(t);
    }
    //------------------------------------------------------------------------
    void Window::updateImpl(SheetContext * sc)
    {
        // move the underlying UISubSheet if we're using such a thing
        if(sc->mRoot == this)
        {
            sc->mFace->setPos(getArea().getIntCache().getPos());

            mRender->setOrigin(Vector3f(mPixelSize.mWidth / 2.0f, mPixelSize.mHeight / 2.0f, 0.0f));

            mPixelData->setOffset(Vector3f(0.0f, 0.0f, 0.0f));

            if (mAutoClip && mParent)
            {
                Rectf temp = mParent->getClipArea(!isContainer());
                sc->mFace->setClip(Recti(temp.mLeft, temp.mTop, temp.mRight, temp.mBottom));
            }
            else
                sc->mFace->setClip(Recti(0, 0, getRenderSize()));

            mPixelData->setClip(Recti(0, 0, PlaneSizei(mPixelSize.mWidth, mPixelSize.mHeight)));
        }
        else
        {
            Container::updateImpl(sc);
        }
    }
    //------------------------------------------------------------------------
    void Window::getContextImpl(SheetContext & sc) const
    {
        if(mRender)
        {
            sc.mSheet = mSheet;
            sc.mFace = mRender;
            sc.mRoot = this;
            sc.mPosOffset = getArea().getIntCache().getPos();
            sc.mQueue = GL_Base;
        }
        else
        {
            sc.mSheet = mSheet;
            sc.mFace = 0;
            sc.mRoot = 0;
            sc.mPosOffset = Vector2f(0, 0);
            sc.mQueue = GL_Base;
        }
    }
    //------------------------------------------------------------------------
    void Window::refreshFaceImpl()
    {
        if(mRender)
        {
            mRender->refresh();
        }
    }
    //------------------------------------------------------------------------
    void Window::onSize(const WidgetEventArgs * arg)
    {
        Container::onSize(arg);
        if(mRender)
            mRender->setSize(getPixelSize());
    }
    //------------------------------------------------------------------------
    void Window::onChildMove(const WidgetEventArgs * arg)
    {
        Container::onChildMove(arg);
        if(mRender)
        {
            mRender->refresh();
            mSheet->redraw();
        }
    }
    //------------------------------------------------------------------------
    void Window::onDestroy(const WidgetEventArgs * arg)
    {
        Container::onDestroy(arg);
        freeRenderCtx();
    }
    //------------------------------------------------------------------------
    void Window::onButtonDown(const CursorEventArgs * arg)
    {
        Container::onButtonDown(arg);
        if(mZOrder && mZOrderClick && !isTopPos())
        {
            mParent->moveChildFront(this);
            ZOrderImpl();
        }
    }
    //------------------------------------------------------------------------
    void Window::onRemove(const WidgetEventArgs * arg)
    {
        if(mRender)
        {
            mRender->refresh();
        }
        else
        {
            mSheet->redraw();
        }
        Container::onRemove(arg);
    }
    //------------------------------------------------------------------------
    Rectf Window::getClipAreaImpl() const
    {
        if(mRender)
            return getArea().getIntCache();

        return Container::getClipAreaImpl();
    }
    //------------------------------------------------------------------------
    Rectf Window::getClipInnerAreaImpl() const
    {
        if(mRender)
            return getInnerArea().getIntCache();

        return Container::getClipInnerAreaImpl();
    }
    //------------------------------------------------------------------------
    void Window::queuePixelImpl()
    {
        if(isFinalVisible())
        {
            SheetContext sc;
            getContext(sc);

            if(sc.mRoot == this && sc.mFace)
                sc.mFace->clear();

            if(!mRender || mRender->isRefresh())
            {
                Container::queuePixelImpl();
            }

            if(sc.mRoot == this && sc.mFace)
                sc.mFace->flush();
        }
    }
    //------------------------------------------------------------------------
    bool Window::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdMousePassThroughEnabled());
        dest->add(N_new CmdAutoRenderingSurface());
        dest->add(N_new CmdRotation());
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DefaultWindow
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DefaultWindow::DefaultWindow(WidgetID wid, FactoryID fid, Container * own, 
        UISheet * sheet, const String & name, LangID lid) :
        Window(wid, fid, own, sheet, name, N_CmdObj_DefaultWindow, lid)
    {
        RelPlaneSizef sz(RelPosf(1.0f, 0), RelPosf(1.0f, 0));
        setMaxSize(sz);
        setSize(sz);
    }
    //------------------------------------------------------------------------
    DefaultWindow::~DefaultWindow()
    {
    }
    //------------------------------------------------------------------------
    void DefaultWindow::onCursorMove(const CursorEventArgs * arg)
    {
        // always call the base class handler
        Widget::onCursorMove(arg);
    }
    //------------------------------------------------------------------------
    void DefaultWindow::onCursorDrag(const CursorEventArgs * arg)
    {
        // always call the base class handler
        Widget::onCursorDrag(arg);
    }
    //------------------------------------------------------------------------
    void DefaultWindow::onButtonDown(const CursorEventArgs * arg)
    {
        // always call the base class handler
        Widget::onButtonDown(arg);
    }
    //------------------------------------------------------------------------
    void DefaultWindow::onButtonUp(const CursorEventArgs * arg)
    {
        // always call the base class handler
        Widget::onButtonUp(arg);
    }
    //------------------------------------------------------------------------
    void DefaultWindow::onCursorTouch(const CursorEventArgs * arg)
    {
        // always call the base class handler
        Widget::onCursorTouch(arg);
    }
    //------------------------------------------------------------------------
    void DefaultWindow::onCursorMultiTouch(const CursorEventArgs * arg)
    {
        // always call the base class handler
        Widget::onCursorMultiTouch(arg);
    }
    //------------------------------------------------------------------------
    bool DefaultWindow::moveFrontImpl(bool click)
    {
        bool m = Window::moveFrontImpl(click);

        if(!mParent && mOvergo)
            return false;
        else
            return m;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdSizingEnabled : public PropertyCmd
    {
    public:
        CmdSizingEnabled() : PropertyCmd(N_PropertyUIFrameWindow_SizingEnable, _T("sizing_enabled"),
            _T("Property to get/set the state of the sizable setting for the FrameWindow. Value is either \"True\" or \"False\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const FrameWindow * obj = static_cast<const FrameWindow *>(own);
            StrConv::conv(obj->isDragResize(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setDragResize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const FrameWindow *>(own)->isDragResize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<FrameWindow *>(own)->setDragResize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSizingEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    class _EngineInner CmdFrameEnabled : public PropertyCmd
    {
    public:
        CmdFrameEnabled() : PropertyCmd(N_PropertyUIFrameWindow_FrameEnable, _T("frame_enabled"),
            _T("Property to get/set the setting for whether the window frame will be displayed. Value is either \"True\" or \"False\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const FrameWindow * obj = static_cast<const FrameWindow *>(own);
            StrConv::conv(obj->isFrameEnable(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setFrameEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const FrameWindow *>(own)->isFrameEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<FrameWindow *>(own)->setFrameEnable(*temp);
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

    class _EngineInner CmdTitlebarEnabled : public PropertyCmd
    {
    public:
        CmdTitlebarEnabled() : PropertyCmd(N_PropertyUIFrameWindow_TitleBarEnable, _T("titlebar_enabled"),
            _T("Property to get/set the setting for whether the window title-bar will be enabled (or displayed depending upon choice of final widget type). Value is either \"True\" or \"False\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const FrameWindow * obj = static_cast<const FrameWindow *>(own);
            StrConv::conv(obj->isTitleBarEnable(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setTitleBarEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const FrameWindow *>(own)->isTitleBarEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<FrameWindow *>(own)->setTitleBarEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTitlebarEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdCloseButtonEnabled : public PropertyCmd
    {
    public:
        CmdCloseButtonEnabled() : PropertyCmd(N_PropertyUIFrameWindow_CloseButtonEnable, _T("close_button_enabled"),
            _T("Property to get/set the setting for whether the window close button will be enabled (or displayed depending upon choice of final widget type). Value is either \"True\" or \"False\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const FrameWindow * obj = static_cast<const FrameWindow *>(own);
            StrConv::conv(obj->isCloseButtonEnable(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setCloseButtonEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const FrameWindow *>(own)->isCloseButtonEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<FrameWindow *>(own)->setCloseButtonEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCloseButtonEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdRollUpEnabled : public PropertyCmd
    {
    public:
        CmdRollUpEnabled() : PropertyCmd(N_PropertyUIFrameWindow_RollUpEnable, _T("roll_up_enabled"),
            _T("Property to get/set the setting for whether the user is able to roll-up / shade the window. Value is either \"True\" or \"False\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const FrameWindow *>(own)->isRollUpEnable(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setRollUpEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const FrameWindow *>(own)->isRollUpEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<FrameWindow *>(own)->setRollUpEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRollUpEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    class _EngineInner CmdRollUpState : public PropertyCmd
    {
    public:
        CmdRollUpState() : PropertyCmd(N_PropertyUIFrameWindow_RollUp, _T("roll_up_state"),
            _T("Property to get/set the roll-up / shade state of the window.  Value is either \"True\" or \"False\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const FrameWindow *>(own)->isRollUp(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setRollUp(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const FrameWindow *>(own)->isRollUp();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<FrameWindow *>(own)->setRollUp(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRollUpState();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    class _EngineInner CmdDragMovingEnabled : public PropertyCmd
    {
    public:
        CmdDragMovingEnabled() : PropertyCmd(N_PropertyUIFrameWindow_DragTitleBar, _T("drag_moving_enabled"),
            _T("Property to get/set the setting for whether the user may drag the window around by its title bar. Value is either \"True\" or \"False\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const FrameWindow *>(own)->isDragMove(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setDragMove(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const FrameWindow *>(own)->isDragMove();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<FrameWindow *>(own)->setDragMove(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDragMovingEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdSizingBorderThickness : public PropertyCmd
    {
    public:
        CmdSizingBorderThickness() : PropertyCmd(N_PropertyUIFrameWindow_BorderSize, _T("sizing_border_thickness"),
            _T("Property to get/set the setting for the sizing border thickness. Value is a NIIf specifying the border thickness in pixels."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const FrameWindow *>(own)->getBorderSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<FrameWindow *>(own)->setBorderSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const FrameWindow *>(own)->getBorderSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameWindow *>(own)->setBorderSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSizingBorderThickness();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(8.0f, out);
        }
    };

    class _EngineInner CmdNSSizingCursorImage : public PropertyCmd
    {
    public:
        CmdNSSizingCursorImage() : PropertyCmd(N_PropertyUIFrameWindow_TBSizingCursor, _T("NS_sizing_cursor_image"),
            _T("Property to get/set the N-S (up-down) sizing cursor image for the FrameWindow. Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const FrameWindow * obj = static_cast<const FrameWindow *>(own);
            const PixelBuffer * pb = obj->getCursor(FrameWindow::RT_Bottom);
            out = pb ? N_conv(pb->getGroupID()) +_T("|")+ N_conv(pb->getID()) : _T("0|0");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            FrameWindow * obj = static_cast<FrameWindow *>(own);
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);

            obj->setCursor(FrameWindow::RT_Bottom, N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui64 * temp = (Nui64 *)out;
            const PixelBuffer * pb = static_cast<const FrameWindow *>(own)->getCursor(FrameWindow::RT_Bottom);
            *temp = pb ? ((Nui64)pb->getGroupID() << 32) | pb->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<FrameWindow *>(own)->setCursor(FrameWindow::RT_Bottom, N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNSSizingCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };

    class _EngineInner CmdEWSizingCursorImage : public PropertyCmd
    {
    public:
        CmdEWSizingCursorImage() : PropertyCmd(N_PropertyUIFrameWindow_LRSizingCursor, _T("EWSizingCursorImage"),
            _T("Property to get/set the E-W (left-right) sizing cursor image for the FrameWindow. Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const PixelBuffer * pb = static_cast<const FrameWindow *>(own)->getCursor(FrameWindow::RT_Right);
            out = pb ? N_conv(pb->getGroupID()) + _T("|") + N_conv(pb->getID()) : _T("0|0");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);
            static_cast<FrameWindow *>(own)->setCursor(FrameWindow::RT_Right, N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui64 * temp = (Nui64 *)out;
            const PixelBuffer * pb = static_cast<const FrameWindow *>(own)->getCursor(FrameWindow::RT_Right);
            *temp = pb ? ((Nui64)pb->getGroupID() << 32) | pb->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<FrameWindow *>(own)->setCursor(FrameWindow::RT_Right, N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdEWSizingCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };

    class _EngineInner CmdNWSESizingCursorImage : public PropertyCmd
    {
    public:
        CmdNWSESizingCursorImage() : PropertyCmd(N_PropertyUIFrameWindow_BRSizingCursor, _T("NWSE_sizing_cursor_image"),
            _T("Property to get/set the NW-SE diagonal sizing cursor image for the FrameWindow. Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const PixelBuffer * pb = static_cast<const FrameWindow *>(own)->getCursor(FrameWindow::RT_BottomRight);
            out = pb ? N_conv(pb->getGroupID()) + _T("|") + N_conv(pb->getID()) : _T("0|0");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);

            static_cast<FrameWindow *>(own)->setCursor(FrameWindow::RT_BottomRight, N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui64 * temp = (Nui64 *)out;
            const PixelBuffer * pb = static_cast<const FrameWindow *>(own)->getCursor(FrameWindow::RT_BottomRight);
            *temp = pb ? ((Nui64)pb->getGroupID() << 32) | pb->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<FrameWindow *>(own)->setCursor(FrameWindow::RT_BottomRight, N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNWSESizingCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };

    class _EngineInner CmdNESWSizingCursorImage : public PropertyCmd
    {
    public:
        CmdNESWSizingCursorImage() : PropertyCmd(N_PropertyUIFrameWindow_BLSizingCursor, _T("NESW_sizing_cursor_image"),
            _T("Property to get/set the NE-SW diagonal sizing cursor image for the FramwWindow. Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const PixelBuffer * pb = static_cast<const FrameWindow *>(own)->getCursor(FrameWindow::RT_BottomLeft);
            out = pb ? N_conv(pb->getGroupID()) + _T("|") + N_conv(pb->getID()) : _T("0|0");
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);

            static_cast<FrameWindow *>(own)->setCursor(FrameWindow::RT_BottomLeft, N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui64 * temp = (Nui64 *)out;
            const PixelBuffer * pb = static_cast<const FrameWindow *>(own)->getCursor(FrameWindow::RT_BottomLeft);
            *temp = pb ? ((Nui64)pb->getGroupID() << 32) | pb->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<FrameWindow *>(own)->setCursor(FrameWindow::RT_BottomLeft, N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNESWSizingCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Window, FrameWindow, RollUpEvent,               0);
    N_EVENT(Window, FrameWindow, CloseButtonEvent,          1);
    N_EVENT(Window, FrameWindow, DragResizeBeginEvent,      2);
    N_EVENT(Window, FrameWindow, DragResizeEndEvent,        3);
    N_EVENT(Window, FrameWindow, EventCount,                4);
    //------------------------------------------------------------------------
    FrameWindow::FrameWindow(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet,
        const String & name, LangID lid) :
        Window(wid, fid, own, sheet, name, N_CmdObj_FrameWindow, lid),
        mTitlebar(0),
        mPushButton(0)
    {
        notifySignal(FrameWindow::EventCount);
        mDragTitleBarMove = true;
        mFrameEnable = true;
        mRollUpEnable = true;
        mResizeEnable = true;
        mRollUp = false;
        mSizing = false;

        mBorderSize = 8.0f;

        mResizeCursor[0] = mResizeCursor[1] = mResizeCursor[3] = mResizeCursor[2] = 0;

        setupCmd(N_CmdObj_FrameWindow);
    }
    //------------------------------------------------------------------------
    FrameWindow::~FrameWindow()
    {
        if(mTitlebar)
        {
            N_Only(Widget).destroy(mTitlebar);
            mTitlebar = 0;
        }
        if(!mPushButton)
        {
            N_Only(Widget).destroy(mPushButton);
            mPushButton = 0;
        }
    }
    //------------------------------------------------------------------------
    void FrameWindow::initChild()
    {
        if(!mTitlebar)
            mTitlebar = static_cast<Titlebar *>(N_Only(Widget).create(mTitlebarView, 0, this, N_WIDGET_Titlebar));

        if(!mPushButton)
            mPushButton = static_cast<PushButton *>(N_Only(Widget).create(mCloseButtonView, 0, this, N_WIDGET_PushButton));

        // configure titlebar
        mTitlebar->setDragEnable(mDragTitleBarMove);
        mTitlebar->setText(mText);

        // ban some properties on components, since they are linked to settings
        // defined here.
        mTitlebar->setWrite(N_PropertyUIWidget_Text, false);
        mTitlebar->setWrite(N_PropertyUIWidget_Visible, false);
        mTitlebar->setWrite(N_PropertyUIWidget_Disable, false);
        mPushButton->setWrite(N_PropertyUIWidget_Visible, false);
        mPushButton->setWrite(N_PropertyUIWidget_Disable, false);

        // bind handler to close button 'Click' event
        mPushButton->bind(PushButton::PushEvent, this, (CommandObjFunctor)&FrameWindow::CloseButtonMFunc);

        layout();
    }
    //------------------------------------------------------------------------
    void FrameWindow::setTitleBarText(const String & str)
    {
        mText = str;
        WidgetEventArgs e(this);
        onText(&e);
        // pass this onto titlebar component.
        mTitlebar->setText(mText);
        // maybe the user is using a fontdim for titlebar size ;)
        layout();
    }
    //------------------------------------------------------------------------
    bool FrameWindow::isTitleBarEnable() const
    {
        return !mTitlebar->isDisable();
    }
    //------------------------------------------------------------------------
    bool FrameWindow::isCloseButtonEnable() const
    {
        return !mPushButton->isDisable();
    }
    //------------------------------------------------------------------------
    bool FrameWindow::isDragResize() const
    {
        return mResizeEnable && isFrameEnable();
    }
    //------------------------------------------------------------------------
    void FrameWindow::setFrameEnable(bool b)
    {
        mFrameEnable = b;
        refresh();
    }
    //------------------------------------------------------------------------
    void FrameWindow::setTitleBarEnable(bool b)
    {
        mTitlebar->setEnable(b);
        mTitlebar->setVisible(b);
    }
    //------------------------------------------------------------------------
    void FrameWindow::setCloseButtonEnable(bool b)
    {
        mPushButton->setEnable(b);
        mPushButton->setVisible(b);
    }
    //------------------------------------------------------------------------
    void FrameWindow::setRollUpEnable(bool b)
    {
        if((b == false) && isRollUp())
        {
            rollup();
        }

        mRollUpEnable = b;
    }
    //------------------------------------------------------------------------
    void FrameWindow::setCursor(ResizeType type, PixelBuffer * pb)
    {
        switch(type)
        {
        case RT_Top:
        case RT_Bottom:
            mResizeCursor[0] = pb;
            break;
        case RT_Left:
        case RT_Right:
            mResizeCursor[1] = pb;
            break;
        case RT_TopLeft:
        case RT_BottomRight:
            mResizeCursor[2] = pb;
            break;
        case RT_TopRight:
        case RT_BottomLeft:
            mResizeCursor[3] = pb;
            break;
        default:
            break;
        }
    }
    //------------------------------------------------------------------------
    PixelBuffer * FrameWindow::getCursor(ResizeType type) const
    {
        switch(type)
        {
        case RT_Top:
        case RT_Bottom:
            return mResizeCursor[0];
            break;
        case RT_Left:
        case RT_Right:
            return mResizeCursor[1];
            break;
        case RT_TopLeft:
        case RT_BottomRight:
            return mResizeCursor[2];
            break;
        case RT_TopRight:
        case RT_BottomLeft:
            return mResizeCursor[3];
            break;
        default:
            break;
        }
        return mAide1->getMouseCursor();
    }
    //------------------------------------------------------------------------
    void FrameWindow::rollup()
    {
        if(isRollUpEnable())
        {
            mRollUp ^= true;

            // event notification.
            WidgetEventArgs args(this);
            onRollUp(&args);

            mSheet->resetCursorOver();
        }
    }
    //------------------------------------------------------------------------
    void FrameWindow::setRollUp(bool b)
    {
        if(b != isRollUp())
        {
            rollup();
        }
    }
    //------------------------------------------------------------------------
    FrameWindow::ResizeType FrameWindow::getBorderResizeType(const Vector2f & pt) const
    {
        Rectf frame(0, 0, mPixelSize.mWidth, mPixelSize.mHeight);

        // we can only size if the frame is enabled and sizing is on
        if(isDragResize() && isFrameEnable())
        {
            // point must be inside the outer edge
            if(frame.intersects(pt.x, pt.y))
            {
                // adjust rect to get inner edge
                frame.mLeft += mBorderSize;
                frame.mTop += mBorderSize;
                frame.mRight -= mBorderSize;
                frame.mBottom -= mBorderSize;

                // detect which edges we are on
                bool top = (pt.y < frame.mTop);
                bool bottom = (pt.y >= frame.mBottom);
                bool left = (pt.x < frame.mLeft);
                bool right = (pt.x >= frame.mRight);

                // return appropriate 'ResizeType' value
                if (top && left)
                {
                    return RT_TopLeft;
                }
                else if (top && right)
                {
                    return RT_TopRight;
                }
                else if (bottom && left)
                {
                    return RT_BottomLeft;
                }
                else if (bottom && right)
                {
                    return RT_BottomRight;
                }
                else if (top)
                {
                    return RT_Top;
                }
                else if (bottom)
                {
                    return RT_Bottom;
                }
                else if (left)
                {
                    return RT_Left;
                }
                else if (right)
                {
                    return RT_Right;
                }
            }
        }

        return RT_None;
    }
    //------------------------------------------------------------------------
    bool FrameWindow::resizeLeft(NIIf delta, RelRectf & out_area)
    {
        NIIf orgWidth = mPixelSize.mWidth;

        // ensure that we only size to the set constraints.
        //
        // NB: We are required to do this here due to our virtually unique sizing nature; the
        // normal system for limiting the window size is unable to supply the information we
        // require for updating our internal state used to manage the dragging, etc.
        NIIf maxWidth(mMaxSize.mWidth.abs(getRenderSize().mWidth));
        NIIf minWidth(mMinSize.mWidth.abs(getRenderSize().mWidth));
        NIIf newWidth = orgWidth - delta;

        if(maxWidth != 0.0f && newWidth > maxWidth)
            delta = orgWidth - maxWidth;
        else if (newWidth < minWidth)
            delta = orgWidth - minWidth;

        // ensure adjustment will be whole pixel
        NIIf adjustment = delta;

        if(mHAlign == HA_RIGHT)
        {
            out_area.mRight.mOffset -= adjustment;
        }
        else if(mHAlign == HA_CENTRE)
        {
            out_area.mRight.mOffset -= adjustment * 0.5f;
            out_area.mLeft.mOffset += adjustment * 0.5f;
        }
        else
        {
            out_area.mLeft.mOffset += adjustment;
        }

        return mHAlign == HA_LEFT;
    }
    //------------------------------------------------------------------------
    bool FrameWindow::resizeRight(NIIf delta, RelRectf & out_area)
    {
        // store this so we can work out how much size actually changed
        NIIf orgWidth = mPixelSize.mWidth;

        // ensure that we only size to the set constraints.
        //
        // NB: We are required to do this here due to our virtually unique sizing nature; the
        // normal system for limiting the window size is unable to supply the information we
        // require for updating our internal state used to manage the dragging, etc.
        NIIf maxWidth(mMaxSize.mWidth.abs(getRenderSize().mWidth));
        NIIf minWidth(mMinSize.mWidth.abs(getRenderSize().mWidth));
        NIIf newWidth = orgWidth + delta;

        if (maxWidth != 0.0f && newWidth > maxWidth)
            delta = maxWidth - orgWidth;
        else if (newWidth < minWidth)
            delta = minWidth - orgWidth;

        // ensure adjustment will be whole pixel
        NIIf adjustment = delta;

        out_area.mRight.mOffset += adjustment;

        if(mHAlign == HA_RIGHT)
        {
            out_area.mRight.mOffset += adjustment;
            out_area.mLeft.mOffset += adjustment;
        }
        else if (mHAlign == HA_CENTRE)
        {
            out_area.mRight.mOffset += adjustment * 0.5f;
            out_area.mLeft.mOffset += adjustment * 0.5f;
        }

        // move the dragging point so mouse remains 'attached' to edge of window
        mDragPos.x += adjustment;

        return mHAlign == HA_RIGHT;
    }
    //------------------------------------------------------------------------
    bool FrameWindow::resizeTop(NIIf delta, RelRectf & out_area)
    {
        NIIf orgHeight = mPixelSize.mHeight;

        // ensure that we only size to the set constraints.
        //
        // NB: We are required to do this here due to our virtually unique sizing nature; the
        // normal system for limiting the window size is unable to supply the information we
        // require for updating our internal state used to manage the dragging, etc.
        NIIf maxHeight(mMaxSize.mHeight.abs(getRenderSize().mHeight));
        NIIf minHeight(mMinSize.mHeight.abs(getRenderSize().mHeight));
        NIIf newHeight = orgHeight - delta;

        if (maxHeight != 0.0f && newHeight > maxHeight)
            delta = orgHeight - maxHeight;
        else if (newHeight < minHeight)
            delta = orgHeight - minHeight;

        // ensure adjustment will be whole pixel
        NIIf adjustment = delta;

        if (mVAlign == VA_BOTTOM)
        {
            out_area.mBottom.mOffset -= adjustment;
        }
        else if (mVAlign == VA_CENTRE)
        {
            out_area.mBottom.mOffset -= adjustment * 0.5f;
            out_area.mTop.mOffset += adjustment * 0.5f;
        }
        else
        {
            out_area.mTop.mOffset += adjustment;
        }

        return mVAlign == VA_TOP;
    }
    //------------------------------------------------------------------------
    bool FrameWindow::resizeBottom(NIIf delta, RelRectf & out_area)
    {
        // store this so we can work out how much size actually changed
        NIIf orgHeight = mPixelSize.mHeight;

        // ensure that we only size to the set constraints.
        //
        // NB: We are required to do this here due to our virtually unique sizing nature; the
        // normal system for limiting the window size is unable to supply the information we
        // require for updating our internal state used to manage the dragging, etc.
        NIIf maxHeight(mMaxSize.mHeight.abs(getRenderSize().mHeight));
        NIIf minHeight(mMinSize.mHeight.abs(getRenderSize().mHeight));
        NIIf newHeight = orgHeight + delta;

        if (maxHeight != 0.0f && newHeight > maxHeight)
            delta = maxHeight - orgHeight;
        else if (newHeight < minHeight)
            delta = minHeight - orgHeight;

        // ensure adjustment will be whole pixel
        NIIf adjustment = delta;

        out_area.mBottom.mOffset += adjustment;

        if (mVAlign == VA_BOTTOM)
        {
            out_area.mBottom.mOffset += adjustment;
            out_area.mTop.mOffset += adjustment;
        }
        else if (mVAlign == VA_CENTRE)
        {
            out_area.mBottom.mOffset += adjustment * 0.5f;
            out_area.mTop.mOffset += adjustment * 0.5f;
        }

        // move the dragging point so mouse remains 'attached' to edge of window
        mDragPos.y += adjustment;

        return mVAlign == VA_BOTTOM;
    }
    //------------------------------------------------------------------------
    void FrameWindow::alterCursor(const Vector2f & cursorpos) const
    {
        switch(getBorderResizeType(cursorpos))
        {
        case RT_Top:
        case RT_Bottom:
            mSheet->getCursor()->setImage(mResizeCursor[0]);
            break;
        case RT_Left:
        case RT_Right:
            mSheet->getCursor()->setImage(mResizeCursor[1]);
            break;
        case RT_TopLeft:
        case RT_BottomRight:
            mSheet->getCursor()->setImage(mResizeCursor[2]);
            break;
        case RT_TopRight:
        case RT_BottomLeft:
            mSheet->getCursor()->setImage(mResizeCursor[3]);
            break;
        default:
            mSheet->getCursor()->setImage(mAide1->getMouseCursor());
            break;
        }
    }
    //------------------------------------------------------------------------
    void FrameWindow::CloseButtonMFunc(const EventArgs * arg)
    {
        onCloseButton(static_cast<const WidgetEventArgs *>(arg));
    }
    //------------------------------------------------------------------------
    void FrameWindow::onRollUp(const WidgetEventArgs * arg)
    {
        refresh(true);
        notifyPosChange();
        WidgetEventArgs size_args(arg->mWidget);
        onSize(&size_args);

        signal(RollUpEvent, arg);
    }
    //------------------------------------------------------------------------
    void FrameWindow::onCloseButton(const WidgetEventArgs * arg)
    {
        signal(CloseButtonEvent, arg);
    }
    //------------------------------------------------------------------------
    void FrameWindow::onCursorMove(const CursorEventArgs * arg)
    {
        Widget::onCursorMove(arg);
        // if we are not the window containing the mouse, do NOT change the cursor
        if(mSheet->getCursorOver() != this)
        {
            return;
        }

        if(isDragResize())
        {
            Vector2f localMousePos(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
            if(mSizing)
            {
                ResizeType dragEdge = getBorderResizeType(mDragPos);

                // calculate sizing deltas...
                NIIf deltaX = localMousePos.x - mDragPos.x;
                NIIf deltaY = localMousePos.y - mDragPos.y;

                RelRectf new_area(mArea);
                bool top_left_sizing = false;
                // size left or right edges
                if((dragEdge == RT_Left) || (dragEdge == RT_TopLeft) || (dragEdge == RT_BottomLeft))
                {
                    top_left_sizing |= resizeLeft(deltaX, new_area);
                }
                else if((dragEdge == RT_Right) || (dragEdge == RT_TopRight) || (dragEdge == RT_BottomRight))
                {
                    top_left_sizing |= resizeRight(deltaX, new_area);
                }

                // size top or bottom edges
                if((dragEdge == RT_Top) || (dragEdge == RT_TopLeft) || (dragEdge == RT_TopRight))
                {
                    top_left_sizing |= resizeTop(deltaY, new_area);
                }
                else if((dragEdge == RT_Bottom) || (dragEdge == RT_BottomLeft) || (dragEdge == RT_BottomRight))
                {
                    top_left_sizing |= resizeBottom(deltaY, new_area);
                }
                setAreaImpl(RelVector2f(new_area.mLeft, new_area.mTop), new_area.getSize());
            }
            else
            {
                alterCursor(localMousePos);
            }
        }
    }
    //------------------------------------------------------------------------
    void FrameWindow::onButtonDown(const CursorEventArgs * arg)
    {
        // default processing (this is now essential as it controls event firing).
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            if(isDragResize())
            {
                // get position of mouse as co-ordinates local to this window.
                Vector2f localPos(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
                // if the mouse is on the sizing border
                if(getBorderResizeType(localPos) != RT_None)
                {
                    // ensure all inputs come to us for now
                    if(focus())
                    {
                        // setup the 'dragging' state variables
                        mSizing = true;
                        mDragPos = localPos;

                        // do drag-sizing started notification
                        WidgetEventArgs args(this);
                        onDragResizeBegin(&args);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void FrameWindow::onButtonUp(const CursorEventArgs * arg)
    {
        // default processing (this is now essential as it controls event firing).
        Widget::onButtonUp(arg);

        if(arg->mMouseArg.mButton == MB_Left && isFocus())
        {
            // release our capture on the input data
            lost();
        }
    }
    //------------------------------------------------------------------------
    void FrameWindow::onLost(const WidgetEventArgs * arg)
    {
        // default processing (this is now essential as it controls event firing).
        Widget::onLost(arg);

        // reset sizing state
        mSizing = false;

        // do drag-sizing ended notification
        WidgetEventArgs _args(this);
        onDragResizeEnd(&_args);
    }
    //------------------------------------------------------------------------
    void FrameWindow::onActivate(const ActivationEventArgs * e)
    {
        Widget::onActivate(e);
        mTitlebar->refresh();
    }
    //------------------------------------------------------------------------
    void FrameWindow::onInactivate(const ActivationEventArgs * e)
    {
        Container::onInactivate(e);
        mTitlebar->refresh();
    }
    //------------------------------------------------------------------------
    void FrameWindow::setDragMove(bool b)
    {
        if(mDragTitleBarMove != b)
        {
            mDragTitleBarMove = b;

            mTitlebar->setDragEnable(b);
        }
    }
    //------------------------------------------------------------------------
    bool FrameWindow::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdSizingEnabled());
        dest->add(N_new CmdFrameEnabled());
        dest->add(N_new CmdTitlebarEnabled());
        dest->add(N_new CmdCloseButtonEnabled());
        dest->add(N_new CmdRollUpEnabled());
        dest->add(N_new CmdRollUpState());
        dest->add(N_new CmdDragMovingEnabled());
        dest->add(N_new CmdSizingBorderThickness());
        dest->add(N_new CmdNSSizingCursorImage());
        dest->add(N_new CmdEWSizingCursorImage());
        dest->add(N_new CmdNWSESizingCursorImage());
        dest->add(N_new CmdNESWSizingCursorImage());
        return true;
    }
    //------------------------------------------------------------------------
    bool FrameWindow::intersects(const Vector2f & pos, bool disable) const
    {
        return Widget::intersects(pos, disable) && !mRollUp;
    }
    //------------------------------------------------------------------------
    void FrameWindow::onDragResizeBegin(const WidgetEventArgs * arg)
    {
        signal(DragResizeBeginEvent, arg);
    }
    //------------------------------------------------------------------------
    void FrameWindow::onDragResizeEnd(const WidgetEventArgs * arg)
    {
        signal(DragResizeEndEvent, arg);
    }
    //------------------------------------------------------------------------
    /*
        - ClientWithTitleWithFrame
        - ClientWithTitleNoFrame
        - ClientNoTitleWithFrame
        - ClientNoTitleNoFrame
    */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FrameWindowView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(WidgetView, FrameWindowView, ActiveTitleFrameState, 0);
    N_STATE(WidgetView, FrameWindowView, InactiveTitleFrameState, 1);
    N_STATE(WidgetView, FrameWindowView, DisableTitleFrameState, 2);
    N_STATE(WidgetView, FrameWindowView, ActiveTitleState, 3);
    N_STATE(WidgetView, FrameWindowView, InactiveTitleState, 4);
    N_STATE(WidgetView, FrameWindowView, DisableTitleState, 5);
    N_STATE(WidgetView, FrameWindowView, ActiveFrameState, 6);
    N_STATE(WidgetView, FrameWindowView, InactiveFrameState, 7);
    N_STATE(WidgetView, FrameWindowView, DisableFrameState, 8);
    N_STATE(WidgetView, FrameWindowView, ActiveState, 9);
    N_STATE(WidgetView, FrameWindowView, InactiveState, 10);
    N_STATE(WidgetView, FrameWindowView, StateCount, 11);
    //------------------------------------------------------------------------
    FrameWindowView::FrameWindowView(WidgetViewlID type) :
        WidgetView(type, N_VIEW_FrameWindow)
    {}
    //------------------------------------------------------------------------
    void FrameWindowView::flush()
    {
        FrameWindow * w = (FrameWindow *)mSrc;
        // do not render anything for the rolled-up state.
        if (w->isRollUp())
            return;

        StateID stateName;
        // build state name
        if (w->isFinalDisable())
        {
            if (w->isTitleBarEnable())
            {
                if (w->isFrameEnable())
                    stateName = DisableTitleFrameState;
                else
                    stateName = DisableTitleState;
            }
            else
            {
                if (w->isFrameEnable())
                    stateName = DisableFrameState;
                else
                    stateName = DisableState;
            }
        }
        else if (w->isFinalActive())
        {
            if (w->isTitleBarEnable())
            {
                if (w->isFrameEnable())
                    stateName = ActiveTitleFrameState;
                else
                    stateName = ActiveTitleState;
            }
            else
            {
                if (w->isFrameEnable())
                    stateName = ActiveFrameState;
                else
                    stateName = ActiveState;
            }
        }
        else
        {
            if (w->isTitleBarEnable())
            {
                if (w->isFrameEnable())
                    stateName = InactiveTitleFrameState;
                else
                    stateName = InactiveTitleState;
            }
            else
            {
                if (w->isFrameEnable())
                    stateName = InactiveFrameState;
                else
                    stateName = InactiveState;
            }
        }
        const StyleLayout * mode;

        try
        {
            // try and get mode for our current state
            mode = &getStyle()->getLayout(stateName);
        }
        catch (UnknownObjectException &)
        {
            // log error so we know mode is missing, and then quit.
            return;
        }

        // peform the rendering operation.
        mode->render(w);
    }
    //------------------------------------------------------------------------
    Rectf FrameWindowView::getArea() const
    {
        FrameWindow * w = (FrameWindow *)mSrc;
        if (w->isRollUp())
            return Rectf(0, 0, 0, 0);

        Nid sid;
        if (w->isTitleBarEnable())
        {
            if (w->isFrameEnable())
            {
                sid = ClientWithTitleWithFrame;
            }
            else
            {
                sid = ClientWithTitleNoFrame;
            }
        }
        else
        {
            if (w->isFrameEnable())
            {
                sid = ClientNoTitleWithFrame;
            }
            else
            {
                sid = ClientNoTitleNoFrame;
            }
        }
        return getStyle()->getSpecial(sid).getArea().getPixelRect(*w, w->getArea().getIntCache());
    }
    //------------------------------------------------------------------------
}
}