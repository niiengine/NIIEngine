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
#include "NiiUITitlebar.h"
#include "NiiUIWindow.h"
#include "NiiUICursor.h"
#include "NiiUIWidgetManager.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdDraggingEnabled : public PropertyCmd
    {
    public:
        CmdDraggingEnabled() : PropertyCmd(N_PropertyUITitlebar_DragEnable, _T("dragging_enabled"),
            _T("Property to get/set the state of the dragging enabled setting for the Titlebar.  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Titlebar * >(own)->isDragEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Titlebar *>(own)->setDragEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Titlebar * >(own)->isDragEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Titlebar *>(own)->setDragEnable(*temp);
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
    Titlebar::Titlebar(WidgetID wid, FactoryID fid, Container * own, const String & name, 
        LangID lid) :
        Widget(N_CmdObj_Titlebar, wid, fid, own, name, lid)
    {
        setupCmd(N_CmdObj_Titlebar);
        setTop(true);

        // basic initialisation
        mDrag = false;
        mDragEnable = true;
    }
    //------------------------------------------------------------------------
    Titlebar::~Titlebar()
    {
    }
    //------------------------------------------------------------------------
    void Titlebar::setDragEnable(bool b)
    {
        if(mDragEnable != b)
        {
            mDragEnable = b;

            // stop dragging now if the setting has been disabled.
            if((!mDragEnable) && mDrag)
            {
                lost();
            }
        }
    }
    //------------------------------------------------------------------------
    bool Titlebar::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdDraggingEnabled());
        return true;
    }
    //------------------------------------------------------------------------
    void Titlebar::onCursorMove(const CursorEventArgs * arg)
    {
        // Base class processing.
        Widget::onCursorMove(arg);
        if(mDrag && (mParent != 0))
        {
            Vector2f delta(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
            // calculate amount that window has been moved
            delta -= mDragPos;

            RelVector2f uOffset(RelPosf(0, delta.x), RelPosf(0, delta.y));

            // move the window.  *** Again: Titlebar objects should only be attached to FrameWindow derived classes. ***
            mParent->setPos(mParent->getPos() + uOffset);
        }
    }
    //------------------------------------------------------------------------
    void Titlebar::onButtonDown(const CursorEventArgs * arg)
    {
        // Base class processing
        Widget::onButtonDown(arg);
        if(arg->mMouseArg.mButton == MB_Left)
        {
            if((mParent != 0) && mDragEnable)
            {
                // we want all mouse inputs from now on
                if(focus())
                {
                    mDrag = true;
                    mDragPos = Vector2f(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
                    // store old constraint area
                    mDragArea = mSheet->getCursor()->getMoveArea();

                    // setup new constraint area to be the intersection of the old area and our grand-parent's clipped inner-area
                    Recti constrainArea;

                    if ((mParent == 0) || (mParent->getParent() == 0))
                    {
                        Recti screen(0, 0, getRenderSize());
                        constrainArea = screen.intersects(mDragArea);
                    }
                    else
                    {
                        Rectf tempi(mDragArea.mLeft, mDragArea.mTop, mDragArea.mRight, mDragArea.mBottom);
                        tempi = mParent->getParent()->getInnerClipArea().intersects(tempi);
                        constrainArea.mLeft = tempi.mLeft;
                        constrainArea.mTop = tempi.mTop;
                        constrainArea.mRight = tempi.mRight;
                        constrainArea.mBottom = tempi.mBottom;
                    }
                    mSheet->getCursor()->setMoveArea(&constrainArea);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Titlebar::onButtonUp(const CursorEventArgs * arg)
    {
        // Base class processing
        Widget::onButtonUp(arg);
        if (arg->mMouseArg.mButton == MB_Left)
        {
            lost();
        }
    }
    //------------------------------------------------------------------------
    void Titlebar::onCursorMultiTouch(const CursorEventArgs * arg)
    {
        // Base class processing
        Widget::onCursorMultiTouch(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            // if we do not have a parent window, then obviously nothing should happen.
            if(mParent)
            {
                // we should only ever be attached to a FrameWindow (or derived) class
                ((FrameWindow *)mParent)->rollup();
            }
        }
    }
    //------------------------------------------------------------------------
    void Titlebar::onLost(const WidgetEventArgs * arg)
    {
        // Base class processing
        Widget::onLost(arg);
        // when we lose out hold on the mouse inputs, we are no longer dragging.
        mDrag = false;

        // restore old constraint area
        mSheet->getCursor()->setMoveArea(&mDragArea);
    }
    //------------------------------------------------------------------------
    void Titlebar::onFont(const FontEventArgs * arg)
    {
        onFont(arg);
        if(mParent)
        {
            mParent->layout();
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TitlebarView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(WidgetView, TitlebarView, ActiveState, 0);
    N_STATE(WidgetView, TitlebarView, InactiveState, 1);
    N_STATE(WidgetView, TitlebarView, StateCount, 2);
    //------------------------------------------------------------------------
    TitlebarView::TitlebarView(WidgetViewlID wsid) :
        WidgetView(wsid, N_VIEW_Titlebar)
    {
    }
    //------------------------------------------------------------------------
    void TitlebarView::flush()
    {
        const StyleLayout * ss;
        try
        {
            // get Style for the assigned look.
            const Style * style = getStyle();
            // try and get mode for our current state
            if (!mSrc->isFinalDisable())
                ss = &style->getLayout((mSrc->getParent() && mSrc->getParent()->isFinalActive()) ?
                    ActiveState : InactiveState);
            else
                ss = &style->getLayout(DisableState);
        }
        catch (UnknownObjectException &)
        {
            // log error so we know mode is missing, and then quit.
            return;
        }
        // peform the rendering operation.
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
}
}