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
#include "NiiUIException.h"
#include "NiiUIWidgetManager.h"
#include "NiiAnimationManager.h"
#include "NiiPropertyFusion.h"

namespace NII
{
namespace UI
{
    class _EngineInner StyleOuter
    {
    public:
        typedef map<WidgetID, StyleWidget *>::type WidgetMap;
        typedef map<EventID, ListenerEvent *>::type ListenerEventMap;
        typedef map<PropertyID, AssignProperty *>::type AssignPropertyMap;
        typedef map<PropertyID, StylePropertyCmd *>::type StylePropertyMap;
        typedef map<PropertyID, ListenerProperty *>::type ListenerPropertyMap;
        typedef map<AnimationID, Animation *>::type AnimationMap;

        static void get(const Style * obj, WidgetMap & out)
        {
            if (obj->mInheritID != 0)
            {
                get(N_Only(Widget).getStyle(obj->mInheritID), out);
            }
            Style::Widgets::const_iterator i, iend = obj->mWidgets.end();
            for(i = obj->mWidgets.begin(); i != iend; ++i)
            {
                out[i->getID()] = const_cast<StyleWidget *>(&*i);
            }
        }
        static void get(const Style * obj, StylePropertyMap & out)
        {
            if (obj->mInheritID != 0)
            {
                get(N_Only(Widget).getStyle(obj->mInheritID), out);
            }
            Style::StylePropertyList::const_iterator i, iend = obj->mStylePropertyList.end();
            for(i = obj->mStylePropertyList.begin(); i != iend; ++i)
            {
                out[(*i)->getID()] = *i;
            }
        }
        static void get(const Style * obj, ListenerPropertyMap & out)
        {
            if (obj->mInheritID != 0)
            {
                get(N_Only(Widget).getStyle(obj->mInheritID), out);
            }
            Style::ListenerPropertyList::const_iterator i, iend = obj->mListenerPropertyList.end();
            for(i = obj->mListenerPropertyList.begin(); i != iend; ++i)
            {
                out[(*i)->getID()] = *i;
            }
        }
        static void get(const Style * obj, AssignPropertyMap & out)
        {
            if (obj->mInheritID != 0)
            {
                get(N_Only(Widget).getStyle(obj->mInheritID), out);
            }
            Style::AssignPropertyList::const_iterator i, iend = obj->mAssignPropertyList.end();
            for(i = obj->mAssignPropertyList.begin(); i != iend; ++i)
            {
                out[i->getProperty()] = const_cast<AssignProperty *>(&*i);
            }
        }
        static void get(const Style * obj, ListenerEventMap & out)
        {
            if (obj->mInheritID != 0)
            {
                get(N_Only(Widget).getStyle(obj->mInheritID), out);
            }
            Style::ListenerEvents::const_iterator i, iend = obj->mListenerEvents.end();
            for(i = obj->mListenerEvents.begin(); i != iend; ++i)
            {
                out[i->getID()] = const_cast<ListenerEvent *>(&*i);
            }
        }
        static void get(const Style * obj, AnimationMap & out)
        {
            if (obj->mInheritID != 0)
            {
                get(N_Only(Widget).getStyle(obj->mInheritID), out);
            }
            AnimationObj::AnimationList::const_iterator i, iend = obj->mAnimations.end();
            for (i = obj->mAnimations.begin(); i != iend; ++i)
            {
                out[i->first] = i->second;
            }
        }
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleEventArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleEventArgs::StyleEventArgs() :
        mStyle(0)
    {
    }
    //------------------------------------------------------------------------
    StyleEventArgs::StyleEventArgs(Widget * w, Style * obj) :
        WidgetEventArgs(w),
        mStyle(obj)
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Style
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Style::Style()
    {
    }
    //------------------------------------------------------------------------
    Style::Style(StyleID sid, StyleID psid) :
        mID(sid),
        mInheritID(psid)
    {
    }
    //------------------------------------------------------------------------
    Style::Style(const Style & o) :
        AnimationFusionObj(o),
        mID(o.mID),
        mInheritID(o.mInheritID),
        mDesigns(o.mDesigns),
        mWidgets(o.mWidgets),
        mModes(o.mModes),
        mAssignPropertyList(o.mAssignPropertyList),
        mSpecials(o.mSpecials),
        //mAnimationObj(o.mAnimationObj),
        mListenerEvents(o.mListenerEvents)
    {
        mAnimations = o.mAnimations;
        StylePropertyList::const_iterator i, iend = o.mStylePropertyList.end();
        for(i = o.mStylePropertyList.begin(); i != iend; ++i)
        {
            mStylePropertyList.push_back(static_cast<StylePropertyCmd *>((*i)->clone()));
        }
        ListenerPropertyList::const_iterator j, jend = o.mListenerPropertyList.end();
        for(j = o.mListenerPropertyList.begin(); j != jend; ++j)
        {
            mListenerPropertyList.push_back(static_cast<ListenerProperty *>((*j)->clone()));
        }
    }
    //------------------------------------------------------------------------
    Style & Style::operator=(const Style & o)
    {
        mID = o.mID;
        mInheritID = o.mInheritID;
        mDesigns = o.mDesigns;
        mWidgets = o.mWidgets;
        mModes = o.mModes;
        mAssignPropertyList = o.mAssignPropertyList;
        mSpecials = o.mSpecials;
        mStylePropertyList = o.mStylePropertyList;
        mListenerPropertyList = o.mListenerPropertyList;
        mAnimations = o.mAnimations;
        //mAnimationObj = o.mAnimationObj;
        mListenerEvents = o.mListenerEvents;

        return *this;
    }
    //------------------------------------------------------------------------
    Style::~Style()
    {
        StylePropertyList::reverse_iterator i, iend = mStylePropertyList.rend();
        for (i = mStylePropertyList.rbegin(); i != iend; ++i)
        {
            N_delete(*i);
        }
        ListenerPropertyList::reverse_iterator j, jend = mListenerPropertyList.rend();
        for (j = mListenerPropertyList.rbegin(); j != jend; ++j)
        {
            N_delete(*j);
        }
    }
    //------------------------------------------------------------------------
    void Style::layout(Widget * src) const
    {
        StyleOuter::WidgetMap wm;
        StyleOuter::get(this, wm);
        StyleOuter::WidgetMap::const_iterator i, iend = wm.end();
        for(i = wm.begin(); i != iend; ++i)
        {
            i->second->layout(src);
        }
    }
    //------------------------------------------------------------------------
    void Style::attach(Widget * widget) const
    {
        // add new property definitions
        StyleOuter::StylePropertyMap spm;
        StyleOuter::get(this, spm);
        StyleOuter::StylePropertyMap::const_iterator spmi, spmiend = spm.end();
        for(spmi = spm.begin(); spmi != spmiend; ++spmi)
        {
            // add the property to the window
            widget->getProperty()->add(spmi->second);
        }

        // add required child widgets
        StyleOuter::WidgetMap wm;
        StyleOuter::get(this, wm);
        StyleOuter::WidgetMap::const_iterator wmi, wmiend = wm.end();
        for(wmi = wm.begin(); wmi != wmiend; ++wmi)
        {
            wmi->second->attach(widget);
        }

        // add new property link definitions
        StyleOuter::ListenerPropertyMap lpm;
        StyleOuter::get(this, lpm);
        StyleOuter::ListenerPropertyMap::const_iterator lpmi, lpmiend = lpm.end();
        for(lpmi = lpm.begin(); lpmi != lpmiend; ++lpmi)
        {
            // add the property to the window
            widget->getProperty()->add(lpmi->second);
        }
        // apply properties to the parent window
        StyleOuter::AssignPropertyMap pic;
        StyleOuter::get(this, pic);
        StyleOuter::AssignPropertyMap::const_iterator pi, piend = pic.end();
        for(pi = pic.begin(); pi != piend; ++pi)
        {
            pi->second->apply(widget);
        }

        // setup linked events
        StyleOuter::ListenerEventMap lem;
        StyleOuter::get(this, lem);
        StyleOuter::ListenerEventMap::const_iterator lemi, lemiend = lem.end();
        for(lemi = lem.begin(); lemi != lemiend; ++lemi)
        {
            lemi->second->attach(widget);
        }

        // create animation instances
        StyleOuter::AnimationMap am;
        StyleOuter::get(this, am);
        StyleOuter::AnimationMap::const_iterator ami, amiend = am.end();
        for(ami = am.begin(); ami != amiend; ++ami)
        {
            PropertyFusion * fusion = N_new PropertyFusion(widget, 
                const_cast<Style *>(this), ami->second);
            const_cast<Style *>(this)->addFusion(fusion, widget->getID());
        }
    }
    //------------------------------------------------------------------------
    void Style::detach(Widget * widget) const
    {
        if(widget->getStyle() != getID())
        {
            N_EXCEPT(InvalidRequest, _I18n("The window '") + widget->getNamePath() +
                _I18n("' does not have this WidgetView assigned"));
        }

        // remove added child widgets
        StyleOuter::WidgetMap wm;
        StyleOuter::get(this, wm);
        StyleOuter::WidgetMap::const_iterator wmi, wmiend = wm.end();
        for(wmi = wm.begin(); wmi != wmiend; ++wmi)
        {
            wmi->second->detach(widget);
        }

        // delete added named Events
        StyleOuter::ListenerEventMap lem;
        StyleOuter::get(this, lem);
        StyleOuter::ListenerEventMap::const_iterator lemi, lemiend = lem.end();
        for(lemi = lem.begin(); lemi != lemiend; ++lemi)
        {
            lemi->second->detach(widget);
        }

        // remove added property definitions
        StyleOuter::StylePropertyMap spm;
        StyleOuter::get(this, spm);
        StyleOuter::StylePropertyMap::const_iterator spmi, spmiend = spm.end();
        for(spmi = spm.begin(); spmi != spmiend; ++spmi)
        {
            // remove the property from the window
            widget->getProperty()->remove(spmi->first);
        }

        // remove added property link definitions
        StyleOuter::ListenerPropertyMap lpm;
        StyleOuter::get(this, lpm);
        StyleOuter::ListenerPropertyMap::const_iterator lpmi, lpmiend = lpm.end();
        for(lpmi = lpm.begin(); lpmi != lpmiend; ++lpmi)
        {
            // remove the property from the window
            widget->getProperty()->remove(lpmi->first);
        }

        const_cast<Style *>(this)->removeAllFusion(widget->getID());

        /*
        // clean up animation instances assoicated wit the window.
        AnimationObj::const_iterator anim;
        while((anim = mAnimationObj.find(widget)) != mAnimationObj.end())
        {
            N_Only(Animation).removeFusion(anim->second);
            mAnimationObj.erase(anim);
        }
        */
    }
    //------------------------------------------------------------------------
    void Style::add(const StyleSection & obj)
    {
        if (mDesigns.find(obj.getID()) != mDesigns.end())
        {
            N_Only(Log).log(_I18n("已经存在风格设计区域,ID: " + obj.getID()));
        }

        mDesigns[obj.getID()] = obj;
    }
    //------------------------------------------------------------------------
    void Style::add(const StyleWidget & obj)
    {
        mWidgets.push_back(obj);
    }
    //------------------------------------------------------------------------
    void Style::add(const StyleLayout & obj)
    {
        if (mModes.find(obj.getID()) != mModes.end())
        {
            N_Only(Log).log(_I18n("Defintion for obj '") + N_conv(obj.getID()) +
                _I18n("' already exists.  Replacing previous definition."));
        }
        mModes[obj.getID()] = obj;
    }
    //------------------------------------------------------------------------
    void Style::add(const AssignProperty & obj)
    {
        mAssignPropertyList.push_back(obj);
    }
    //------------------------------------------------------------------------
    void Style::add(const StyleSpecial & spec)
    {
        if(mSpecials.find(spec.getID()) != mSpecials.end())
            N_Only(Log).log(_I18n("区域已经存在!,ID: '") + N_conv(spec.getID()) + 
            _I18n("' 将覆盖之前定义"));

        mSpecials[spec.getID()] = spec;
    }
    //------------------------------------------------------------------------
    void Style::add(StylePropertyCmd * obj)
    {
        mStylePropertyList.push_back(obj);
    }
    //------------------------------------------------------------------------
    void Style::add(ListenerProperty * obj)
    {
        mListenerPropertyList.push_back(obj);
    }
    //------------------------------------------------------------------------
    void Style::add(const ListenerEvent & obj)
    {
        mListenerEvents.push_back(obj);
    }
    //------------------------------------------------------------------------
    const StyleLayout & Style::getLayout(Nid id) const
    {
        States::const_iterator i = mModes.find(id);
        if(i != mModes.end())
            return (*i).second;

        if(mInheritID == 0)
            N_EXCEPT(UnknownObject, _I18n("不明状态,ID: '") + N_conv(id) +
            _I18n("' 在UI风格,ID: '") + N_conv(mID) + _T("'."));
        return N_Only(Widget).getStyle(mInheritID)->getLayout(id);
    }
    //------------------------------------------------------------------------
    const StyleSection & Style::getDesign(Nid id) const
    {
        Designs::const_iterator i = mDesigns.find(id);
        if(i != mDesigns.end())
            return (*i).second;

        if(mInheritID == 0)
            N_EXCEPT(UnknownObject, _I18n("不明画片区域,ID: '") + N_conv(id) +
            _I18n("' 在UI风格,ID: '") + N_conv(mID) + _T("'."));
        return N_Only(Widget).getStyle(mInheritID)->getDesign(id);
    }
    //------------------------------------------------------------------------
    bool Style::isStateExist(Nid id) const
    {
        States::const_iterator i = mModes.find(id);

        if(i != mModes.end())
            return true;

        if(mInheritID == 0)
            return false;

        return N_Only(Widget).getStyle(mInheritID)->isStateExist(id);
    }
    //------------------------------------------------------------------------
    const StyleSpecial & Style::getSpecial(Nid id) const
    {
        Specials::const_iterator i = mSpecials.find(id);
        if(i != mSpecials.end())
            return (*i).second;

        if(mInheritID == 0)
            N_EXCEPT(UnknownObject, _I18n("不明区域: ID: '") + N_conv(id) + 
            _I18n("' 在UI风格ID: '") + N_conv(mID) + _T("'."));

        return N_Only(Widget).getStyle(mInheritID)->getSpecial(id);
    }
    //------------------------------------------------------------------------
    bool Style::isSpecialExist(Nid id) const
    {
        Specials::const_iterator i = mSpecials.find(id);
        if(i != mSpecials.end())
            return true;

        if(mInheritID == 0)
            return false;

        return N_Only(Widget).getStyle(mInheritID)->isSpecialExist(id);
    }
    //------------------------------------------------------------------------
    void Style::write(XmlSerializer * out) const
    {
        out->begin(_T("WidgetView")).attribute(_T("name"), mID).
            attribute(_T("inherits"), mInheritID);

        // These sub-scopes of the loops avoid the "'curr'-already-initialized"
        // compile error on VC6++
        {
            // output property definitions
            StylePropertyList::const_iterator curr, currend = mStylePropertyList.end();
            for(curr = mStylePropertyList.begin(); curr != currend; ++curr)
            {
                (*curr)->write(out);
            }
        }
        {
            // output property link definitions
            ListenerPropertyList::const_iterator curr, currend = mListenerPropertyList.end();
            for(curr = mListenerPropertyList.begin(); curr != currend; ++curr)
            {
                (*curr)->write(out);
            }
        }
        {
            // output property initialisers.
            AssignPropertyList::const_iterator curr, currend = mAssignPropertyList.end();
            for(curr = mAssignPropertyList.begin(); curr != currend; ++curr)
            {
                (*curr).write(out);
            }
        }
        {
            // output named areas
            Specials::const_iterator curr, currend = mSpecials.end();
            for(curr = mSpecials.begin(); curr != currend; ++curr)
            {
                (*curr).second.write(out);
            }
        }
        {
            // output child widgets
            Widgets::const_iterator curr, currend = mWidgets.end();
            for(curr = mWidgets.begin(); curr != currend; ++curr)
            {
                (*curr).write(out);
            }
        }
        {
            // output imagery sections
            Designs::const_iterator curr, currend = mDesigns.end();
            for(curr = mDesigns.begin(); curr != currend; ++curr)
            {
                (*curr).second.write(out);
            }
        }
        {
            // output states
            States::const_iterator curr, currend = mModes.end();
            for(curr = mModes.begin(); curr != currend; ++curr)
            {
                (*curr).second.write(out);
            }
        }
        out->end();
    }
    //------------------------------------------------------------------------
    const AssignProperty * Style::getAssign(PropertyID pid) const
    {
        StyleOuter::AssignPropertyMap apm;
        StyleOuter::get(this, apm);
        StyleOuter::AssignPropertyMap::const_iterator i = apm.find(pid);
        if(i == apm.end())
            return 0;

        return i->second;
    }
    //------------------------------------------------------------------------
    const ListenerProperty * Style::getListener(PropertyID pid) const
    {
        StyleOuter::ListenerPropertyMap lpm;
        StyleOuter::get(this, lpm);
        StyleOuter::ListenerPropertyMap::const_iterator i = lpm.find(pid);
        if(i == lpm.end())
            return 0;

        return i->second;
    }
    //------------------------------------------------------------------------
    const StyleWidget * Style::getWidget(WidgetID wid) const
    {
        StyleOuter::WidgetMap wm;
        StyleOuter::get(this, wm);
        StyleOuter::WidgetMap::const_iterator i = wm.find(wid);
        if(i == wm.end())
            return 0;

        return i->second;
    }
    //------------------------------------------------------------------------
    bool Style::notifyFont(Widget * window, const Font * font) const
    {
        bool result = false;

        Designs::const_iterator i, iend = mDesigns.end();
        for(i = mDesigns.begin(); i != iend; ++i)
        {
            result |= i->second.notifyFont(window, font);
        }

        Widgets::const_iterator j, jend = mWidgets.end();
        for(j = mWidgets.begin(); j != jend; ++j)
        {
            result |= j->notifyFont(window, font);
        }

        if(mInheritID != 0)
            result |= N_Only(Widget).getStyle(mInheritID)->notifyFont(window, font);

        return result;
    }
    //------------------------------------------------------------------------
}
}