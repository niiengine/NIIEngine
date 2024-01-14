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
#include "NiiListenerProperty.h"
#include "NiiXmlSerializer.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    void ListenerProperty::add(WidgetID widget, PropertyID pid)
    {
        mListeners.push_back(Npair(widget, pid));
    }
    //------------------------------------------------------------------------
    bool ListenerProperty::remove(WidgetID wid, PropertyID pid)
    {
        Listeners::const_iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            if(pid == i->second && wid == i->first)
            {
                mListeners.erase(i);
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ListenerProperty::isExist(WidgetID wid, PropertyID pid) const
    {
        Listeners::const_iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            if (pid == i->second && wid == i->first)
            {
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void ListenerProperty::removeAll()
    {
        mListeners.clear();
    }
    //------------------------------------------------------------------------
    Widget * ListenerProperty::getDest(const PropertyCmdObj * obj, WidgetID wid) const
    {
        const Widget * re = static_cast<const Widget *>(obj);
        if(wid == 0)
            return const_cast<Widget *>(re);
        else if(wid == N_Parent_Obj)
            return re->getParent();
        else if(re->isContainer())
            return static_cast<const Container *>(obj)->get(wid);

        return 0;
    }
    //------------------------------------------------------------------------
    void ListenerProperty::write(XmlSerializer * out) const
    {
        out->begin(_T("PropertyLinkDefinition"));

        out->attribute(_T("name"), mID);

        String def;
        getDefault(def);
        if(def != _T(""))
            out->attribute(_T("value"), def);

        if(!getVerbose().empty())
            out->attribute(_T("verbose"), getVerbose());

        if(mRefresh)
            out->attribute(_T("signal_refresh"), "true");

        if(mLayout)
            out->attribute(_T("signal_layout"), "true");

        if(mSetEvent != 0)
            out->attribute(_T("signal_event"), N_conv((Nui32)mSetEvent));

        // HACK: Here we abuse some intimate knowledge in that we know it's
        // safe to write our sub-elements out although the function is named
        // for writing pdc.  The alternative was to repeat code from the
        // base class, also demonstrating intimate knowledge ;)

        Listeners::const_iterator i = mListeners.begin();

        // if there is one target only, write it out as pdc
        if(mListeners.size() == 1)
        {
            if(i->first != 0)
                out->attribute(_T("listen_widget"), i->first);

            if(i->second != 0)
                out->attribute(_T("listen_property"), i->second);
        }
        // we have multiple targets, so write them as PropertyLinkTarget tags
        else
        {
            for( ; i != mListeners.end(); ++i)
            {
                out->begin(_T("ListenTarget"));

                if(i->first != 0)
                    out->attribute(_T("listen_widget"), i->first);

                if(i->second != 0)
                    out->attribute(_T("listen_property"), i->second);

                out->end();
            }
        }

        out->end();
    }
    //------------------------------------------------------------------------
    void ListenerProperty::get(const PropertyCmdObj * own, String & out) const
    {
        if(mListeners.empty())
        {
            getDefault(out);
            return;
        }
        Listeners::const_iterator i = mListeners.begin();
        const Widget * w = getDest(own, i->first);

        // if no target, or target (currently) invalid, return the default value
        if(!w)
        {
            getDefault(out);
            return;
        }
        // otherwise return the value of the property for first target, since
        // this is considered the 'master' target for get operations.
        w->get(i->second == 0 ? mID : i->second, out);
    }

    //------------------------------------------------------------------------
    void ListenerProperty::set(PropertyCmdObj * own, const String & value)
    {
        Listeners::const_iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            Widget * temp = getDest(own, i->first);
            // only try to set property if target is currently valid.
            if(temp)
                temp->set(i->second == 0 ? mID : i->second, value);
        }
        Widget * widget = static_cast<Widget *>(own);

        if(mLayout)
            widget->layout();

        if(mRefresh)
            widget->refresh();

        if(mSetEvent != 0)
        {
            WidgetEventArgs args(widget);
            args.mWidget->signal(mSetEvent, &args);
        }
    }
    //------------------------------------------------------------------------
    void ListenerProperty::init(PropertyCmdObj * own) const
    {
        Listeners::const_iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            Widget * temp = getDest(own, i->first);
            // only try to set property if target is currently valid.
            if(temp)
            {
                String def;
                this->getDefault(def);
                temp->set(i->second == 0 ? mID : i->second, def);
            }
        }
    }
    //------------------------------------------------------------------------
    bool ListenerProperty::isSerialize() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    void ListenerProperty::getDefault(String & out) const
    {
        out = _T("");
    }
    //------------------------------------------------------------------------
    PropertyCmd * ListenerProperty::clone() const
    {
        return N_new ListenerProperty(*this);
    }
    //------------------------------------------------------------------------
}
}