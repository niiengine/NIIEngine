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
#include "NiiUISizeContainer.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdContentPaneAutoSized : public PropertyCmd
    {
    public:
        CmdContentPaneAutoSized() : PropertyCmd(N_PropertyUISizeContainer_AutoSize, _T("content_pane_auto_sized"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const SizeContainer *>(own)->isAutoSize(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<SizeContainer *>(own)->setAutoSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const SizeContainer *>(own)->isAutoSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<SizeContainer *>(own)->setAutoSize(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdContentPaneAutoSized();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdContentArea : public PropertyCmd
    {
    public:
        CmdContentArea() : PropertyCmd(N_PropertyUISizeContainer_ContentArea, _T("content_area"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const Rectf & val = static_cast<const SizeContainer *>(own)->getContentArea();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"),
                val.mLeft, val.mTop, val.mRight, val.mBottom);

            out = buff;
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Rectf temp(0, 0, 0, 0);
            Nsscanf(val.c_str(), _T("%g %g %g %g"),
                &temp.mLeft, &temp.mTop, &temp.mRight, &temp.mBottom);
            static_cast<SizeContainer *>(own)->setContentArea(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Rectf * temp = (Rectf *)out;
            *temp = static_cast<const SizeContainer *>(own)->getContentArea();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Rectf * temp = (Rectf *)in;
            static_cast<SizeContainer *>(own)->setContentArea(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdContentArea();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdChildExtentsArea : public PropertyCmd
    {
    public:
        CmdChildExtentsArea() : PropertyCmd(N_PropertyUISizeContainer_AutoContentArea, _T("child_extents_area"), _T(""))
        {

        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const SizeContainer * obj = static_cast<const SizeContainer *>(own);
            Rectf val = static_cast<const SizeContainer *>(own)->getAutoContentArea();
            Ntchar buff[256];
            Nsntprintf(buff, sizeof(buff), _T("%g %g %g %g"),
                val.mLeft, val.mTop, val.mRight, val.mBottom);

            out = buff;
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            (void)own;
            (void)val;
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Rectf * temp = (Rectf *)out;
            *temp = static_cast<const SizeContainer *>(own)->getAutoContentArea();
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
            return N_new CmdChildExtentsArea();
        }
        
        bool isWritable() const
        {
            return false;
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container,    SizeContainer,    AutoSizeChangeEvent,         0);
    N_EVENT(Container,    SizeContainer,    ContentAreaChangeEvent,      1);
    N_EVENT(Container,    SizeContainer,    EventCount,                  2);
    //------------------------------------------------------------------------
    SizeContainer::SizeContainer(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_SizeContainer, lid),
        mChildArea(0, 0, 0, 0),
        mAutoSize(true),
        mClientArea(this, (CalcMethod)(&SizeContainer::getChildAreaImpl))
    {
        notifySignal(SizeContainer::EventCount);
        setupCmd(N_CmdObj_SizeContainer);
        setCursorSubmitEvent(true);
    }
    //------------------------------------------------------------------------
    SizeContainer::~SizeContainer()
    {
    }
    //------------------------------------------------------------------------
    void SizeContainer::setAutoSize(bool b)
    {
        if(mAutoSize != b)
        {
            mAutoSize = b;

            // Fire events
            WidgetEventArgs args(this);
            onAutoSizeChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void SizeContainer::setContentArea(const Rectf & area)
    {
        if(!mAutoSize)
        {
            mChildArea = area;

            // Fire event
            WidgetEventArgs args(this);
            onContentAreaChange(&args);
       }
    }
    //------------------------------------------------------------------------
    const CalcRect & SizeContainer::getChildInnerArea() const
    {
        return mClientArea;
    }
    //------------------------------------------------------------------------
    const CalcRect & SizeContainer::getChildArea() const
    {
        return mClientArea;
    }
    //------------------------------------------------------------------------
    void SizeContainer::notifyPosChange()
    {
        mClientArea.resetCached();

        Container::notifyPosChange();
    }
    //------------------------------------------------------------------------
    Rectf SizeContainer::getAutoContentArea() const
    {
        Rectf extents(0, 0, 0, 0);

        NCount end = getCount();
        if(end == 0)
            return extents;

        for(NCount i = 0; i < end; ++i)
        {
            const Widget * const wnd = get(i);
            const Rectf area(abs(wnd->getPos(), mPixelSize),
                wnd->getPixelSize());

            if(area.mLeft < extents.mLeft)
                extents.mLeft = area.mLeft;

            if(area.mTop < extents.mTop)
                extents.mTop = area.mTop;

            if(area.mRight > extents.mRight)
                extents.mRight = area.mRight;

            if(area.mBottom > extents.mBottom)
                extents.mBottom = area.mBottom;
        }

        return extents;
    }
    //------------------------------------------------------------------------
    void SizeContainer::onContentAreaChange(const WidgetEventArgs * arg)
    {
        if(mAutoSize)
        {
            mChildArea = getAutoContentArea();
        }
        signal(ContentAreaChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void SizeContainer::onAutoSizeChange(const WidgetEventArgs * arg)
    {
        signal(AutoSizeChangeEvent, arg);

        if(mAutoSize)
        {
            WidgetEventArgs arg1(this);
            onContentAreaChange(&arg1);
        }
    }
    //------------------------------------------------------------------------
    void SizeContainer::ChildSizeMFunc(const EventArgs *)
    {
        // Fire event that notifies that a child's area has changed.
        WidgetEventArgs args(this);
        onContentAreaChange(&args);
    }
    //------------------------------------------------------------------------
    void SizeContainer::ChildMoveMFunc(const EventArgs *)
    {
        // Fire event that notifies that a child's area has changed.
        WidgetEventArgs args(this);
        onContentAreaChange(&args);
    }
    //------------------------------------------------------------------------
    void SizeContainer::queuePixelImpl()
    {

    }
    //------------------------------------------------------------------------
    Rectf SizeContainer::getInnerAreaImpl(bool pixelAlign) const
    {
        return mParent ?
            (pixelAlign ? mParent->getInnerArea().getIntCache() : mParent->getInnerArea().getFloatCache()) :
                Widget::getInnerAreaImpl(pixelAlign);
    }
    //------------------------------------------------------------------------
    Rectf SizeContainer::getClipInnerAreaImpl() const
    {
        return mParent ? mParent->getInnerClipArea() : Widget::getClipInnerAreaImpl();
    }
    //------------------------------------------------------------------------
    Rectf SizeContainer::getIntersectAreaImpl() const
    {
        return mParent ? mParent->getIntersectArea() : Widget::getIntersectAreaImpl();
    }
    //------------------------------------------------------------------------
    Rectf SizeContainer::getChildAreaImpl(bool pixelAlign) const
    {
        if(!mParent)
        {
            return pixelAlign ? Widget::getInnerArea().getIntCache() :
                Widget::getInnerArea().getFloatCache();
        }
        else
        {
            if(pixelAlign)
            {
                return Rectf(getArea().getIntCache().getPos(),
                    mParent->getInnerArea().getIntCache().getSize());
            }
            else
            {
                return Rectf(getArea().getFloatCache().getPos(),
                    mParent->getInnerArea().getFloatCache().getSize());
            }
        }
    }
    //------------------------------------------------------------------------
    void SizeContainer::onAdd(const WidgetEventArgs * arg)
    {
        Container::onAdd(arg);
        Widget * widget = arg->mWidget;

        // subscribe to some events on this child
        mSignalList.insert(Npair(widget, widget->bind(Widget::SizeEvent,
            this, (CommandObjFunctor)&SizeContainer::ChildSizeMFunc)));
        mSignalList.insert(Npair(widget, widget->bind(Widget::MoveEvent,
            this, (CommandObjFunctor)&SizeContainer::ChildMoveMFunc)));

        // force window to update what it thinks it's screen / pixel areas are.
        widget->notifyPosChange();

        // perform notification.
        WidgetEventArgs arg1(this);
        onContentAreaChange(&arg1);
    }
    //------------------------------------------------------------------------
    void SizeContainer::onRemove(const WidgetEventArgs * arg)
    {
        Container::onRemove(arg);

        // disconnect from events for this window.
        SignalList::iterator conn, itend = mSignalList.end();
        while((conn = mSignalList.find(arg->mWidget)) != itend)
        {
            conn->second->disconnect();
            mSignalList.erase(conn);
        }

        // perform notification only if we're not currently being destroyed
        if(!mDestroy)
        {
            WidgetEventArgs arg1(this);
            onContentAreaChange(&arg1);
        }
    }
    //------------------------------------------------------------------------
    void SizeContainer::onParentSize(const WidgetEventArgs * arg)
    {
        Widget::onParentSize(arg);

        // perform notification.
        WidgetEventArgs arg1(this);
        onContentAreaChange(&arg1);
    }
    //------------------------------------------------------------------------
    bool SizeContainer::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdContentPaneAutoSized());
        dest->add(N_new CmdContentArea());
        dest->add(N_new CmdChildExtentsArea());
        return true;
    }
    //------------------------------------------------------------------------
    void SizeContainer::setAreaImpl(const RelVector2f & pos, const RelPlaneSizef & size,
        bool fireEvents)
    {
        mClientArea.resetCached();
        Widget::setAreaImpl(pos, size, fireEvents);
    }
    //------------------------------------------------------------------------
}
}