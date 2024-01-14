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
#include "NiiUIContainer.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWindow.h"
#include "NiiUIException.h"
#include "NiiXmlSerializer.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdDistributeCapturedInputs : public PropertyCmd
    {
    public:
        CmdDistributeCapturedInputs() : PropertyCmd(N_PropertyUIContainer_BroadcastInput,
            _T("distribute_captured_inputs"), _I18n("")) {}

        /// @copydetails PropertyCmd::
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Container *>(own)->isTransmitCursorEvent(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Container *>(own)->setTransmitCursorEvent(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Container *>(own)->isTransmitCursorEvent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Container *>(own)->setTransmitCursorEvent(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDistributeCapturedInputs();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Widget,        Container,    AddEvent,                      0);
    N_EVENT(Widget,        Container,    RemoveEvent,                   1);
    N_EVENT(Widget,        Container,    ChildMoveEvent,                2);
    N_EVENT(Widget,        Container,    CursorEnterAreaEvent,          3);
    N_EVENT(Widget,        Container,    CursorLeaveAreaEvent,          4);
    N_EVENT(Widget,        Container,    EventCount,                    5);
    //------------------------------------------------------------------------
    Container::Container(WidgetID wid, FactoryID fid, Container * own,const String & name,
        ScriptTypeID stid, LangID lid) :
        Widget(stid, wid, fid, own, name, lid),
        mTransmitCursorEvent(false),
        mCursorEnter(false)
    {
        notifySignal(Container::EventCount);
    }
    //------------------------------------------------------------------------
    Container::~Container()
    {
        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            N_Only(Widget).destroy(*i);
        }
    }
    //------------------------------------------------------------------------
    Widget * Container::create(WidgetID wid, StyleViewID sid, const String & name)
    {
        Widget * re = N_Only(Widget).create(sid, wid, this);
        re->setName(name);
        re->mSheet = mSheet;
        add(re);

        return re;
    }
    //------------------------------------------------------------------------
    void Container::destroy(Widget * obj)
    {
        assert(isExist(obj) && "Widget you are attempting to destroy is not a child!");

        N_Only(Widget).destroy(obj);
    }
    //------------------------------------------------------------------------
    void Container::destroy(const String & path)
    {
        destroy(get(path));
    }
    //------------------------------------------------------------------------
    void Container::update(TimeDurMS cost)
    {
        Widget::update(cost);

        NCount i, iend = mChilds.size();
        for(i = 0; i < iend; ++i)
        {
            // update children based on their RefreshMode setting.
            if(mChilds[i]->mRefreshMode == RM_Alway || 
                (mChilds[i]->mRefreshMode == RM_Visible && mChilds[i]->isVisible()))
            {
                mChilds[i]->update(cost);
            }
        }
    }
    //------------------------------------------------------------------------
    Widget * Container::get(const String & name_path, bool nonpath) const
    {
        Widget * w = getImpl(name_path, nonpath);

        if(w)
            return w;

        return 0;
    }
    //------------------------------------------------------------------------
    NCount Container::getCount() const
    {
        return mChilds.size();
    }
    //------------------------------------------------------------------------
    Widget * Container::getImpl(const String & str, bool non_path) const
    {
        NCount i, count = mChilds.size();
        if(non_path)
        {
            std::queue<Widget *> all;

            for (i = 0; i < count; ++i) // load all children into the queue
            {
                Widget * child = mChilds[i];
                all.push(child);
            }

            while (!all.empty()) // breadth-first search for the child to find
            {
                Widget * child = all.front();
                all.pop();

                if (child)
                {
                    if (child->getName() == str)
                    {
                        return child;
                    }
                }
                if (child->isContainer())
                {
                    Container * temp1 = static_cast<Container *>(child);
                    count = temp1->mChilds.size();
                    for (i = 0; i < count; ++i)
                    {
                        all.push(temp1->mChilds[i]);
                    }
                }
            }
            return 0;
        }
        else
        {
            NCount sep = str.find_first_of(_T('/'));
            String base_child(str.substr(0, sep));

            for(i = 0; i < count; ++i)
            {
                Widget * child = mChilds[i];

                if (!child)
                    continue;

                if(child->getName() == base_child)
                {
                    if(sep != String::npos && sep < str.length() - 1 && child->isContainer())
                        return static_cast<Container *>(child)->getImpl(str.substr(sep + 1));
                    else
                        return child;
                }
            }
            return 0;
        }
    }
    //------------------------------------------------------------------------
    void Container::add(Widget * e)
    {
        N_assert(e != this, "ÄÚ²¿´íÎó - ");

        addImpl(e);
        WidgetEventArgs args(e);
        onAdd(&args);
    }
    //------------------------------------------------------------------------
    void Container::remove(WidgetID id)
    {
        NCount i, iend = mChilds.size();

        for(i = 0; i < iend; ++i)
        {
            if(mChilds[i]->getID() == id)
            {
                remove(mChilds[i]);
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    Widget * Container::get(Nidx index) const
    {
        return mChilds[index];
    }
    //------------------------------------------------------------------------
    void Container::remove(Widget * e)
    {
        if(!e)
            N_EXCEPT(InvalidRequest, _I18n("NULL can't be a child of any Widget, ")
                _I18n("it makes little sense to ask for its removal"));

        removeImpl(e);
        WidgetEventArgs args(e);
        onRemove(&args);
    }
    //------------------------------------------------------------------------
    void Container::remove(const String & path)
    {
        Widget * e = getImpl(path);

        if(e)
            remove(e);
    }
    //------------------------------------------------------------------------
    bool Container::isExist(const Widget * e) const
    {
        return std::find(mChilds.begin(), mChilds.end(), e) != mChilds.end();
    }
    //------------------------------------------------------------------------
    bool Container::isExist(WidgetID id, bool r) const
    {
        NCount i, iend = mChilds.size();

        if(r)
        {
            for(i = 0; i < iend; ++i)
            {
                if (mChilds[i]->getID() == id ||  (mChilds[i]->isContainer() && 
                    static_cast<Container *>(mChilds[i])->isExist(id, true)))
                {
                    return true;
                }
            }
        }
        else
        {
            for(i = 0; i < iend; ++i)
            {
                if (mChilds[i]->getID() == id)
                {
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Container::isExist(const String & str, bool non_path) const
    {
        return getImpl(str, non_path) != 0;
    }
    //------------------------------------------------------------------------
    void Container::moveChildFront(Widget * obj)
    {
        WidgetList::iterator pos = std::find(mChilds.begin(), mChilds.end(), obj);
        if (pos != mChilds.end())
        {
            mChilds.erase(pos);
        }
        // calculate position where window should be added for drawing
        WidgetList::reverse_iterator rpos = mChilds.rbegin();
        if (!obj->isTop())
        {
            // find last non-topmost window
            while ((rpos != mChilds.rend()) && ((*rpos)->isTop()))
                ++rpos;
        }

        mChilds.insert(rpos.base(), obj);
    }
    //------------------------------------------------------------------------
    void Container::moveChildBack(Widget * obj)
    {
        WidgetList::iterator pos = std::find(mChilds.begin(), mChilds.end(), obj);
        if (pos != mChilds.end())
        {
            mChilds.erase(pos);
        }
        // calculate position where window should be added for drawing
        WidgetList::iterator rpos = mChilds.begin();
        if (obj->isTop())
        {
            // find first topmost window
            while ((rpos != mChilds.end()) && (!(*rpos)->isTop()))
                ++rpos;
        }
        // add window to draw list
        mChilds.insert(rpos, obj);
    }
    //------------------------------------------------------------------------
    NCount Container::getIndex(Widget * e) const
    {
        NCount i, iend = mChilds.size();
        for(i = 0; i < iend; ++i)
        {
            if(get(i) == e)
            {
                return i;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool Container::isChildFocus() const
    {
        return isExist(mSheet->getFocus());
    }
    //------------------------------------------------------------------------
    Widget * Container::get(WidgetID id, bool r) const
    {
        NCount i, iend = mChilds.size();
        if(r)
        {
            std::queue<Widget *> all;

            for(i = 0; i < iend; ++i) // load all children into the queue
            {
                Widget * child = mChilds[i];
                all.push(child);
            }

            while(!all.empty()) // breadth-first search for the child to find
            {
                Widget * child = all.front();
                all.pop();

                if(child)
                {
                    if (child->getID() == id)
                    {
                        return child;
                    }
                }

                Container * container = dynamic_cast<Container *>(child);
                if(container)
                {
                    iend = container->mChilds.size();
                    for(i = 0; i < iend; ++i)
                    {
                        all.push(container->mChilds[i]);
                    }
                }
            }
        }
        else
        {
            for (i = 0; i < iend; ++i)
            {
                if (mChilds[i]->getID() == id)
                {
                    return mChilds[i];
                }
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    Widget * Container::get(const Vector2f & pos) const
    {
        return get(pos, &Widget::intersects);
    }
    //------------------------------------------------------------------------
    Widget * Container::get(const Vector2f & pos, intersect f, bool disable) const
    {
        Vector2f p;
        if(mWindow)
        {
            UISubSheet * temp = mWindow->getRender();
            if(temp)
                temp->affine(pos, p);
            else
                p = pos;
        }
        else
        {
            p = pos;
        }

        WidgetList::const_reverse_iterator i, iend = mChilds.rend();
        for(i = mChilds.rbegin(); i != iend; ++i)
        {
            if((*i)->isFinalVisible())
            {
                // recursively scan for hit on children of this child window...
                if ((*i)->isContainer())
                {
                    Widget * wnd = static_cast<Container *>(*i)->get(p, f, disable);
                    return wnd;
                }
                // see if this child is hit and return it's pointer if it is
                else if (((*i)->*f)(p, disable))
                {
                    return *i;
                }
            }
        }

        return 0;
    }
    //------------------------------------------------------------------------
    Widget * Container::getTarget(const Vector2f & pos, bool disable) const
    {
        return get(pos, &Widget::isPos, disable);
    }
    //------------------------------------------------------------------------
    Widget * Container::getActiveChild() const
    {
        if(!isFinalActive())
            return 0;

        WidgetList::const_reverse_iterator it, itend = mChilds.rend();
        for(it = mChilds.rbegin(); it != itend; ++it)
        {
            // don't need full backward scan for activeness as we already know
            // 'this' is active.  NB: This uses the draw-ordered child list, as that
            // should be quicker in most cases.

            Widget * wnd = *it;
            if(wnd->mActive && wnd->isContainer())
            {
                return static_cast<Container *>(wnd)->getActiveChild();
            }
        }

        // no child was active, therefore we are the topmost active window
        return const_cast<Container *>(this);
    }
    //------------------------------------------------------------------------
    void Container::setTransmitCursorEvent(bool b)
    {
        mTransmitCursorEvent = b;
    }
    //------------------------------------------------------------------------
    bool Container::isTransmitCursorEvent() const
    {
        return mTransmitCursorEvent;
    }
    //------------------------------------------------------------------------
    void Container::layout()
    {
        Widget::layout();

        if(!mValidClipInnerRect || !mValidClipRect)
        {
            NCount i, count = mChilds.size();
            for(i = 0; i < count; ++i)
            {
                WidgetEventArgs args(this);
                mChilds[i]->onParentSize(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    bool Container::isContainer() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    void Container::clearChild()
    {
        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++iend)
        {
            Widget * e = *i;
            remove(e);
            if(e->isAutoDestroy())
                N_Only(Widget).destroy(e);
        }
    }
    //------------------------------------------------------------------------
    const CalcRect & Container::getChildArea() const
    {
        return getArea();
    }
    //------------------------------------------------------------------------
    const CalcRect & Container::getChildInnerArea() const
    {
        return getInnerArea();
    }
    //------------------------------------------------------------------------
    const CalcRect & Container::getChildArea(bool content) const
    {
        return content ? getChildInnerArea() : getChildArea();
    }
    //------------------------------------------------------------------------
    void Container::setRestorePreFocus(bool b)
    {
        Widget::setRestorePreFocus(b);

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++iend)
        {
            (*i)->setRestorePreFocus(b);
        }
    }
    //------------------------------------------------------------------------
    bool Container::writeChild(XmlSerializer * dest) const
    {
        if(!mDataWrite)
            return false;

        // we temporarily output to this string stream to see if have to do the tag
        // at all.  Make sure this stream does UTF-8
        Nostringstream ss;
        XmlSerializer xml(&ss, this);
        xml.begin(N_PropertyUIWidget_AutoWindow);
        // Create the XML Child Tree
        // write out properties.
        writeImpl(&xml);
        xml.end();
        if(xml.getTagCount() <= 1)
            return false;

        // output opening AutoWindow tag
        dest->begin(N_PropertyUIWidget_AutoWindow);
        // write name suffix attribute
        dest->attribute(N_PropertyUIWidget_Name, getID());
        // Inefficient : do the XML serialization again
        // write out properties.
        writeImpl(dest);
        dest->end();

        return true;
    }
    //------------------------------------------------------------------------
    void Container::queuePixelImpl()
    {
        Widget::queuePixelImpl();

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            (*i)->queuePixelImpl();
        }
    }
    //------------------------------------------------------------------------
    void Container::initChild()
    {
        // do noting
    }
    //------------------------------------------------------------------------
    void Container::addImpl(Widget * e)
    {
        if(isExist(e))
            return;

        if(e)
        {
            Widget * existing = getImpl(e->getName());

            if (existing && e != existing)
                N_EXCEPT(UniqueRepeat, _I18n("Failed to add Widget named: ") +
                    e->getName() + _I18n(" to widget at: ") + getNamePath() +
                    _I18n(" since an Widget with that name is already attached."));
        }

        // if widget is attached elsewhere, detach it first (will fire normal events)
        Container * oldp = e->mParent;
        if(oldp)
            oldp->remove(e);

        // add widget to child list
        mChilds.push_back(e);

        // set the parent widget
        e->mParent = this;
        e->mSheet = mParent->mSheet;

        // update area rects and content for the added widget
        e->notifyPosChange();

        // correctly call parent sized notification if needed.
        if(!oldp || oldp->getPixelSize() != getPixelSize())
        {
            WidgetEventArgs args(this);
            e->onParentSize(&args);
        }
        e->refresh(true);

        e->ZOrderImpl();
    }
    //------------------------------------------------------------------------
    void Container::removeImpl(Widget * e)
    {
        if(e)
        {
            Widget * f = mSheet->getFocus();
            if(f)
            {
                if(f == e || f->isAncestor(e))
                {
                    f->lost();
                }
            }

            // remove from draw list
            //removeWindowFromDrawList(*e);

            WidgetList::iterator i = std::find(mChilds.begin(), mChilds.end(), e);
            if(i != mChilds.end())
            {
                e->setParent(0);
                e->setWrite(N_PropertyUIWidget_RestorePreFocus, true);
                mChilds.erase(i);
            }

            e->ZOrderImpl();
        }
    }
    //------------------------------------------------------------------------
    void Container::refreshImpl(bool r)
    {
        Widget::refreshImpl(r);
        if(r)
        {
            WidgetList::iterator i, iend = mChilds.end();
            for(i = mChilds.begin(); i != iend; ++i)
            {
                (*i)->refreshImpl(true);
            }
        }
    }
    //------------------------------------------------------------------------
    void Container::cloneImpl(Widget * dest, bool r) const
    {
        Container * temp = static_cast<Container *>(dest);
        Widget::cloneImpl(temp, r);

        if(r)
        {
            WidgetList::const_iterator i, iend = mChilds.end();
            for(i = mChilds.begin(); i != iend; ++i)
            {
                Widget * c = *i;
                if(c->isAutoWindow())
                {
                    c->cloneImpl(temp, r);
                    continue;
                }
                Widget * nc = c->clone(r);
                temp->add(nc);
            }
        }
    }
    //------------------------------------------------------------------------
    void Container::notifyPosChange()
    {
        Widget::notifyPosChange();

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            (*i)->notifyPosChange();
        }
    }
    //------------------------------------------------------------------------
    void Container::notifyClipChange()
    {
        Widget::notifyClipChange();

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            Widget * w = *i;
            if(w->isAutoClip())
                w->notifyClipChange();
        }
    }
    //------------------------------------------------------------------------
    NCount Container::writeImpl(XmlSerializer * dest) const
    {
        Widget::writeImpl(dest);

        NCount count = 0;
        WidgetList::const_iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            const Widget * c = *i;

            if(!c->isAutoWindow())
            {
                if (c->isContainer())
                    static_cast<const Container *>(c)->writeChild(dest);
                else
                    c->write(dest);
                ++count;
            }
        }

        return count;
    }
    //------------------------------------------------------------------------
    bool Container::initCmd(PropertyCmdSet * dest)
    {
        Widget::initCmd(dest);
        dest->add(N_new CmdDistributeCapturedInputs());
        return true;
    }
    //------------------------------------------------------------------------
    void Container::onSize(const WidgetEventArgs * arg)
    {
        Widget::onSize(arg);

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            WidgetEventArgs args(this);
            (*i)->onParentSize(&args);
        }
    }
    //------------------------------------------------------------------------
    void Container::onEnable(const WidgetEventArgs * arg)
    {
        Widget::onEnable(arg);

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            Widget * c = *i;
            if(c->mEnable)
            {
                WidgetEventArgs args(c);
                c->onEnable(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void Container::onDisable(const WidgetEventArgs * arg)
    {
        Widget::onDisable(arg);

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            Widget * c = *i;
            if(c->mEnable)
            {
                WidgetEventArgs args(c);
                c->onDisable(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void Container::onAlpha(const WidgetEventArgs * arg)
    {
        Widget::onAlpha(arg);

        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            Widget * c = *i;
            if(c->isEnvAlpha())
            {
                WidgetEventArgs args(c);
                c->onAlpha(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void Container::onCursorEnterArea(const CursorEventArgs * arg)
    {
        mCursorEnter = true;
        signal(CursorEnterAreaEvent, arg);
    }
    //------------------------------------------------------------------------
    void Container::onCursorLeaveArea(const CursorEventArgs * arg)
    {
        mCursorEnter = false;
        signal(CursorLeaveAreaEvent, arg);
    }
    //------------------------------------------------------------------------
    void Container::onInactivate(const ActivationEventArgs * arg)
    {
        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            Widget * c = *i;
            if(c->isFinalActive())
            {
                ActivationEventArgs ce(c);
                ce.mActWidget = arg->mActWidget;
                c->onInactivate(&ce);
            }
        }
        Widget::onInactivate(arg);
    }
    //------------------------------------------------------------------------
    void Container::onStyleAttach(const StyleEventArgs * arg)
    {
        Widget::onStyleAttach(arg);
        initChild();
    }
    //------------------------------------------------------------------------
    void Container::onDestroy(const WidgetEventArgs * arg)
    {
        Widget::onDestroy(arg);
        clear();
    }
    //------------------------------------------------------------------------
    void Container::onSheet(const SheetEventArgs * arg)
    {
        Widget::onSheet(arg);
        WidgetList::iterator i, iend = mChilds.end();
        for(i = mChilds.begin(); i != iend; ++i)
        {
            (*i)->onSheet(arg);
        }
    }
    //------------------------------------------------------------------------
    void Container::onButtonDown(const CursorEventArgs * arg)
    {
        Widget::onButtonDown(arg);
        Widget * last = getActiveSibling();
        if(arg->mWidget && arg->mWidget != last)
        {
            ActivationEventArgs arg(last);
            arg.mActWidget = this;
            last->onInactivate(&arg);
        }
    }
    //------------------------------------------------------------------------
    void Container::onAdd(const WidgetEventArgs * arg)
    {
        mSheet->redraw();
        signal(AddEvent, arg);
    }
    //------------------------------------------------------------------------
    void Container::onRemove(const WidgetEventArgs * arg)
    {
        mSheet->redraw();
        signal(RemoveEvent, arg);
    }
    //------------------------------------------------------------------------
    void Container::onChildMove(const WidgetEventArgs * arg)
    {
        signal(ChildMoveEvent, arg);
    }
    //------------------------------------------------------------------------
}
}