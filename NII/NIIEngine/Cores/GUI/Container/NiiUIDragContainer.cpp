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
#include "NiiUIDragContainer.h"
#include "NiiUIWindow.h"
#include "NiiUICursorEventArgs.h"
#include "NiiPixelManager.h"

namespace NII
{
namespace UI
{
    /** 可放拖拽容器的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DragDropWindow : public Container
    {
    public:
        DragDropWindow(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~DragDropWindow();

        /** 设置是否启用可拖放
        @version NIIEngine 3.0.0
        */
        void setDragDropEnable(bool b) { mDragDropEnable = b; }

        /** 获取是否启用可拖放
        @version NIIEngine 3.0.0
        */
        bool isDragDropEnable() const { return mDragDropEnable; }

        /** 通知有容器移入
        @version NIIEngine 3.0.0
        */
        void notifyEnter(DragContainer * c);

        /** 通知有容器放入
        @version NIIEngine 3.0.0
        */
        void notifyDrop(DragContainer * c);

        /** 通知有容器移出
        @version NIIEngine 3.0.0
        */
        void notifyLeave(DragContainer * c);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);
    protected:
        /** 有容器移入时触发
        @version NIIEngine 3.0.0
        */
        virtual void onEnter(const DragDropEventArgs * arg);

        /** 有容器放入时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDrop(const DragDropEventArgs * arg);

        /** 有容器移出时触发
        @version NIIEngine 3.0.0
        */
        virtual void onLeave(const DragDropEventArgs * arg);
    public:
        /** 有容器移入事件
        @version NIIEngine 3.0.0
        */
        static const EventID EnterEvent;

        /** 有容器放入事件
        @version NIIEngine 3.0.0
        */
        static const EventID DropEvent;

        /** 有容器移出事件
        @version NIIEngine 3.0.0
        */
        static const EventID LeaveEvent;

        /// 事件总数
        static const EventID EventCount;
    protected:
        bool mDragDropEnable;
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdDragDropTarget : public PropertyCmd
    {
    public:
        CmdDragDropTarget() : PropertyCmd(N_PropertyUIDragDropWindow_DragDropEnable, _T("drag_drop_target"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const DragDropWindow *>(own)->isDragDropEnable(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<DragDropWindow *>(own)->setDragDropEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const DragDropWindow *>(own)->isDragDropEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<DragDropWindow *>(own)->setDragDropEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDragDropTarget();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, DragDropWindow, EnterEvent, 0);
    N_EVENT(Container, DragDropWindow, LeaveEvent, 1);
    N_EVENT(Container, DragDropWindow, DropEvent, 2);
    N_EVENT(Container, DragDropWindow, EventCount, 3);
    //------------------------------------------------------------------------
    DragDropWindow::DragDropWindow(WidgetID wid, FactoryID fid, Container * own, const String & name, LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_DragDropWindow, lid),
        mDragDropEnable(true)
    {
        notifySignal(DragDropWindow::EventCount);
    }
    //------------------------------------------------------------------------
    DragDropWindow::~DragDropWindow()
    {
    }
    //------------------------------------------------------------------------
    void DragDropWindow::notifyEnter(DragContainer * c)
    {
        if (c)
        {
            DragDropEventArgs args(this);
            args.mItem = c;
            onEnter(&args);
        }
    }
    //------------------------------------------------------------------------
    void DragDropWindow::notifyLeave(DragContainer * c)
    {
        if (c)
        {
            DragDropEventArgs args(this);
            args.mItem = c;
            onLeave(&args);
        }
    }
    //------------------------------------------------------------------------
    void DragDropWindow::notifyDrop(DragContainer * c)
    {
        if (c)
        {
            DragDropEventArgs args(this);
            args.mItem = c;
            onDrop(&args);
        }
    }
    //------------------------------------------------------------------------
    bool DragDropWindow::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdDragDropTarget());
        return true;
    }
    //------------------------------------------------------------------------
    void DragDropWindow::onEnter(const DragDropEventArgs * arg)
    {
        signal(EnterEvent, arg);
    }
    //------------------------------------------------------------------------
    void DragDropWindow::onLeave(const DragDropEventArgs * arg)
    {
        signal(LeaveEvent, arg);
    }
    //------------------------------------------------------------------------
    void DragDropWindow::onDrop(const DragDropEventArgs * arg)
    {
        signal(DropEvent, arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdDraggingEnabled : public PropertyCmd
    {
    public:
        CmdDraggingEnabled() : PropertyCmd(N_PropertyUIDragContainer_DragEnable, _T("dragging_enabled"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const DragContainer *>(own)->isDragEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<DragContainer *>(own)->setDragEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const DragContainer *>(own)->isDragEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<DragContainer *>(own)->setDragEnable(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDraggingEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdDragAlpha : public PropertyCmd
    {
    public:
        CmdDragAlpha() : PropertyCmd(N_PropertyUIDragContainer_DragAlpha, _T("drag_alpha"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = N_conv(static_cast<const DragContainer *>(own)->getDragAlpha());
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<DragContainer *>(own)->setDragAlpha(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const DragContainer *>(own)->getDragAlpha();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<DragContainer *>(own)->setDragAlpha(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDragAlpha();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = N_conv((NIIf)0.5f);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdDragThreshold : public PropertyCmd
    {
    public:
        CmdDragThreshold() : PropertyCmd(N_PropertyUIDragContainer_DragThreshold, _T("drag_threshold"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const DragContainer *>(own)->getDragThreshold(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<DragContainer *>(own)->setDragThreshold(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const DragContainer *>(own)->getDragThreshold();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<DragContainer *>(own)->setDragThreshold(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDragThreshold();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)8.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdDragCursorImage : public PropertyCmd
    {
    public:
        CmdDragCursorImage() : PropertyCmd(N_PropertyUIDragContainer_DragCursor, _T("drag_cursor_image"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const PixelBuffer * val = static_cast<const DragContainer *>(own)->getDragCursor();
            out = val ? N_conv(val->getGroupID()) + _T("|") + N_conv(val->getID()) : _T("0|0");
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));

            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);
            static_cast<DragContainer *>(own)->setDragCursor(N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            const PixelBuffer * val = static_cast<const DragContainer *>(own)->getDragCursor();
            Nui64 * temp = (Nui64 *)out;
            *temp = val ? ((Nui64)val->getGroupID() << 32) | val->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<DragContainer *>(own)->setDragCursor(N_Only(Pixel).get(rid, pid));
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDragCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdStickyMode : public PropertyCmd
    {
    public:
        CmdStickyMode() : PropertyCmd(N_PropertyUIDragContainer_StickyMode, _T("sticky_mode"),
            _T("Property to get/set the state of the sticky mode setting for the "
                "DragContainer.  Value is either \"True\" or \"False\"."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const DragContainer *>(own)->isStickyMode(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<DragContainer *>(own)->setStickyMode(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const DragContainer *>(own)->isStickyMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<DragContainer *>(own)->setStickyMode(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdStickyMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, DragContainer, DragBeginEvent,      0);
    N_EVENT(Container, DragContainer, DragEndEvent,        1);
    N_EVENT(Container, DragContainer, DragPosEvent,        2);
    N_EVENT(Container, DragContainer, DragEnableEvent,     3);
    N_EVENT(Container, DragContainer, DragAlphaEvent,      4);
    N_EVENT(Container, DragContainer, DragCursorEvent,     5);
    N_EVENT(Container, DragContainer, DragThresholdEvent,  6);
    N_EVENT(Container, DragContainer, DragDropWindowEvent, 7);
    N_EVENT(Container, DragContainer, EventCount,          8);
    //------------------------------------------------------------------------
    DragContainer::DragContainer(WidgetID wid, FactoryID fid, Container * own, const String & name, 
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_DragContainer, lid),
        mDragThreshold(8.0f),
        mDragAlpha(0.5f),
        mTarget(0),
        mDragCursor(0),
        mStickyMode(false),
        mPick(false),
        mDragEnable(true),
        mDragButtomDown(false),
        mDrag(false)
    {
        notifySignal(DragContainer::EventCount);
        setupCmd(N_CmdObj_DragContainer);
    }
    //------------------------------------------------------------------------
    DragContainer::~DragContainer()
    {
    }
    //------------------------------------------------------------------------
    void DragContainer::setDragEnable(bool b)
    {
        if(mDragEnable != b)
        {
            mDragEnable = b;
            WidgetEventArgs args(this);
            onDragEnableChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::setDragThreshold(NIIf oft)
    {
        if(mDragThreshold != oft)
        {
            mDragThreshold = oft;
            WidgetEventArgs args(this);
            onDragThresholdChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::setDragAlpha(NIIf alpha)
    {
        if(mDragAlpha != alpha)
        {
            mDragAlpha = alpha;
            WidgetEventArgs args(this);
            onDragAlphaChange(&args);
        }
    }
    //------------------------------------------------------------------------
    PixelBuffer * DragContainer::getDragCursor() const
    {
        return mDragCursor ? mDragCursor : mSheet->getCursor()->getDefaultImage();
    }
    //------------------------------------------------------------------------
    void DragContainer::setDragCursor(PixelBuffer * pb)
    {
        if(mDragCursor != pb)
        {
            mDragCursor = pb;
            WidgetEventArgs args(this);
            onDragCursorChange(&args);
        }
    }
    //------------------------------------------------------------------------
    bool DragContainer::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdDraggingEnabled());
        dest->add(N_new CmdDragAlpha());
        dest->add(N_new CmdDragThreshold());
        dest->add(N_new CmdDragCursorImage());
        dest->add(N_new CmdStickyMode());
        //dest->add(N_new CmdFixedDragOffset());
        //dest->add(N_new CmdUseFixedDragOffset());
        return true;
    }
    //------------------------------------------------------------------------
    bool DragContainer::isReachDrag(const Vector2f & local_mouse)
    {
        // calculate amount mouse has moved.
        NIIf deltaX = fabsf(local_mouse.x - mDragBeginPos.x.abs(mPixelSize.mWidth));
        NIIf deltaY = fabsf(local_mouse.y - mDragBeginPos.y.abs(mPixelSize.mHeight));

        // see if mouse has moved far enough to start dragging operation
        return (deltaX > mDragThreshold || deltaY > mDragThreshold) ? true : false;
    }
    //------------------------------------------------------------------------
    void DragContainer::_notifyDrag()
    {
        // only proceed if dragging is actually enabled
        if(mDragEnable)
        {
            mPreClip = mAutoClip;
            setAutoClip(false);
            mPreAlpha = mAlpha;
            setAlpha(mDragAlpha);
            mBeginPos = getPos();

            mDrag = true;
            notifyPosChange();

            // Now drag mode is set, change cursor as required
            mSheet->getCursor()->setImage(mDrag ? getDragCursor() : mAide1->getMouseCursor());
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::dragging(const Vector2f & local_mouse)
    {
        // calculate amount to move
        RelVector2f oft(RelPosf(0, local_mouse.x), RelPosf(0, local_mouse.y));
        oft -= mDragBeginPos;
        // set new position
        setPos(getPos() + oft);

        // Perform event notification
        WidgetEventArgs args(this);
        onDragPosChange(&args);
    }
    //------------------------------------------------------------------------
    void DragContainer::onButtonDown(const CursorEventArgs * arg)
    {
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            if(focus())
            {
                // get position of mouse as co-ordinates local to this window.
                Vector2f localPos(arg->mLocalX - getActualPosX(), 
                    arg->mLocalY - getActualPosY());
                // store drag point for possible sizing or moving operation.
                mDragBeginPos.x = RelPosf(0, localPos.x);
                mDragBeginPos.y = RelPosf(0, localPos.y);
                mDragButtomDown = true;
            }
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onButtonUp(const CursorEventArgs * arg)
    {
        Widget::onButtonUp(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            if(mDrag)
            {
                // release picked up state
                if(mPick)
                    mPick = false;

                // fire off event
                WidgetEventArgs args(this);
                onDragEnd(&args);
            }
            // check for sticky pick up
            else if(mStickyMode && !mPick)
            {
                _notifyDrag();
                mPick = true;
                // in this case, do not proceed to release inputs.
                return;
            }

            // release our capture on the input data
            lost();
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onCursorMove(const CursorEventArgs * arg)
    {
        Widget::onCursorMove(arg);

        // get position of mouse as co-ordinates local to this window.
        Vector2f localMousePos(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
        // handle dragging
        if(mDrag)
        {
            dragging(localMousePos);
        }
        // not dragging
        else
        {
            // if mouse button is down (but we're not yet being dragged)
            if(mDragButtomDown)
            {
                if(isReachDrag(localMousePos))
                {
                    // Trigger the event
                    WidgetEventArgs args(this);
                    onDragBegin(&args);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onLost(const WidgetEventArgs * e)
    {
        Widget::onLost(e);

        // reset state
        if(mDrag)
        {
            // restore windows 'normal' state.
            mDrag = false;
            setPos(mBeginPos);
            setAutoClip(mPreClip);
            setAlpha(mPreAlpha);

            notifyPosChange();

            // restore normal mouse cursor
            mSheet->getCursor()->setImage(mDrag ? getDragCursor() : mAide1->getMouseCursor());
        }

        mDragButtomDown = false;
        mTarget = 0;
    }
    //------------------------------------------------------------------------
    void DragContainer::onAlpha(const WidgetEventArgs * arg)
    {
        // store new value and re-set dragging alpha as required.
        if(mDrag)
        {
            mPreAlpha = mAlpha;
            mAlpha = mDragAlpha;
        }

        Container::onAlpha(arg);
    }
    //------------------------------------------------------------------------
    void DragContainer::onClipMode(const WidgetEventArgs * arg)
    {
        // store new value and re-set clipping for drag as required.
        if(mDrag)
        {
            mPreClip = mAutoClip;
            mAutoClip = false;
        }

        Widget::onClipMode(arg);
    }
    //------------------------------------------------------------------------
    void DragContainer::onMove(const WidgetEventArgs * arg)
    {
        Widget::onMove(arg);
        if(mDrag)
        {
            mBeginPos = getPos();
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragBegin(const WidgetEventArgs * arg)
    {
        _notifyDrag();
        signal(DragBeginEvent, arg);
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragEnd(const WidgetEventArgs * arg)
    {
        signal(DragEndEvent, arg);
        // did we drop over a window?
        if(mTarget)
        {
            // Notify that item was dropped in the target window
            mTarget->notifyDrop(this);
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragPosChange(const WidgetEventArgs * arg)
    {
        signal(DragPosEvent, arg);
        Window * root = mSheet->getActive();

        if(root != 0)
        {
            // this hack with the 'enabled' state is so that get
            // returns something useful instead of a pointer back to 'this'.
            // This hack is only acceptable because I am CrazyEddie!
            bool wasEnabled = mEnable;
            mEnable = false;
            // find out which child of root window has the mouse in it
            Widget * eventWindow = root->getTarget(mSheet->getCursor()->getPos());
            mEnable = wasEnabled;

            // use root itself if no child was hit
            if(!eventWindow)
            {
                eventWindow = root;
            }

            // if the window with the mouse is different to current drop target
            if(eventWindow != mTarget)
            {
                DragDropEventArgs args(eventWindow);
                args.mItem = this;
                onDragDropWindowChange(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragEnableChange(const WidgetEventArgs * arg)
    {
        signal(DragEnableEvent, arg);
        // abort current drag operation if dragging gets disabled part way through
        if(!mDragEnable && mDrag)
        {
            lost();
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragAlphaChange(const WidgetEventArgs * arg)
    {
        signal(DragAlphaEvent, arg);
        if(mDrag)
        {
            mAlpha = mPreAlpha;
            onAlpha(arg);
        }
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragCursorChange(const WidgetEventArgs * arg)
    {
        signal(DragCursorEvent, arg);
        mSheet->getCursor()->setImage(mDrag ? getDragCursor() : mAide1->getMouseCursor());
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragThresholdChange(const WidgetEventArgs * arg)
    {
        signal(DragThresholdEvent, arg);
    }
    //------------------------------------------------------------------------
    void DragContainer::onDragDropWindowChange(const DragDropEventArgs * arg)
    {
        signal(DragDropWindowEvent, arg);

        // Notify old target that drop item has left
        if(mTarget)
        {
            mTarget->notifyLeave(this);
        }

        // update to new target
        mTarget = static_cast<DragDropWindow *>(arg->mWidget);

        while((mTarget != 0) && !mTarget->isDragDropEnable())
            mTarget = static_cast<DragDropWindow *>(mTarget->getParent());

        // Notify new target window that someone has dragged a DragContainer over it
        if(mTarget)
            mTarget->notifyEnter(this);
    }
    //------------------------------------------------------------------------
    void DragContainer::getContextImpl(SheetContext & ctx) const
    {
        if(!mDrag)
        {
            Widget::getContextImpl(ctx);
            return;
        }

        Window * w = getRoot();
        ctx.mSheet = w->getSheet();
        ctx.mFace = w->getRender();
        ctx.mRoot = (ctx.mFace != 0) ? w : 0;
        // ensure use of correct oft for the surface we're targetting
        ctx.mPosOffset = ctx.mRoot ? ctx.mRoot->getClipArea().getPos() : Vector2f(0, 0);
        ctx.mQueue = GL_Overlay;
    }
    //------------------------------------------------------------------------
    bool DragContainer::pickUp(const Vector2f & pos)
    {
        // check if we're already picked up or if dragging is disabled.
        if(mPick || !mDragEnable)
            return true;

        // see if we need to force sticky mode switch
        if(!mStickyMode)
            setStickyMode(true);

        // can only pick up if sticky
        if(mStickyMode)
        {
            // force immediate release of any current input capture (unless it's us)
            Widget * of = mSheet->getFocus();
            if(of && of != this)
                of->lost();
            // activate ourselves and try to capture input
            activate();
            if(focus())
            {
                // set the dragging point to the centre of the container.
                mDragBeginPos.x = RelPosf(0, mPixelSize.mWidth / 2);
                mDragBeginPos.y = RelPosf(0, mPixelSize.mHeight / 2);

                _notifyDrag();

                // get position of mouse as co-ordinates local to this window.
                Vector2f localMousePos(mSheet->getCursor()->getPos() -
                    Vector2f(getActualPosX(), getActualPosY()));
                dragging(localMousePos);

                mPick = true;
            }
        }

        return mPick;
    }
    //------------------------------------------------------------------------
}
}