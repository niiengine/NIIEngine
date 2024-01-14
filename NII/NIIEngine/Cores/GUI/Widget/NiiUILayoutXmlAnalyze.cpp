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
#include "NiiUILayoutXmlAnalyze.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIException.h"
#include "NiiUIManager.h"
#include "NiiUIContainer.h"
#include "NiiLogManager.h"
#include "NiiScriptModule.h"
#include "NiiXmlAnalyzeList.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    UILayoutXmlAnalyze::UILayoutXmlAnalyze(LangID lid, PropertyCallback * callback, 
        void * userdata) :
        XmlAnalyze(4, N_Script_UILayout, _T("UILayout.xsd"), lid),
        mRootWindow(0),
        mPropertyCB(callback),
        mCustomData(userdata)
    {
        setBeginPrc(N_PropertyUILayout_Define, (PrcCB)&UILayoutXmlAnalyze::PrcLayoutImportCB);  // LayoutImport
        setBeginPrc(N_PropertyUILayout_Event, (PrcCB)&UILayoutXmlAnalyze::PrcEventCB);          // Event
    }
    //------------------------------------------------------------------------
    UILayoutXmlAnalyze::~UILayoutXmlAnalyze()
    {
    }
    //------------------------------------------------------------------------
    const String & UILayoutXmlAnalyze::getXSD() const
    {
        return mXSD;
    }
    //------------------------------------------------------------------------
    XmlUnitID UILayoutXmlAnalyze::getUnit() const
    {
        return N_XML_TAG_UILayout;
    }
    //------------------------------------------------------------------------
    GroupID UILayoutXmlAnalyze::getGroup() const
    {
        return N_Only(Widget).getGroup();
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc)
    {
        // handle Widget pid (create window and make an entry on our "window stack")
        if(pid == N_XML_TAG_Widget)
        {
            PrcWidgetCB(pdc);
        }
        // handle AutoWindow pid (get an auto child from the window on top of the "window stack")
        else if(pid == N_PropertyUIWidget_AutoWindow)
        {
            PrcAutoWidgetCB(pdc);
        }
        // handle UserString pid (set user string for window at top of stack)
        else if(pid == N_PropertyUIWidget_ExtData)
        {
            PrcUserDataCB(pdc);
        }
        // handle PropertyCmd pid (set property for window at top of stack)
        else if(pid == Widget::PropertyDefine)
        {
            PrcPropertyCB(pdc);
        }
        // anything else is an error which *should* have already been caught by XML validation
        else
        {
            N_Only(Log).log(_I18n("Unexpected data was found while parsing the gui-layout file: '") +
                N_conv(pid) + _I18n("' is unknown."));
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc)
    {
        // handle Widget pid
        if(pid == N_XML_TAG_Widget)
        {
            PrcWidgetEndCB();
        }
        // handle Widget pid
        else if(pid == N_PropertyUIWidget_AutoWindow)
        {
            PrcAutoWidgetEnd();
        }
        // handle UserString pid
        else if(pid == N_PropertyUIWidget_ExtData)
        {
            PrcUserDataEndCB();
        }
        // handle PropertyCmd pid
        else if(pid == Widget::PropertyDefine)
        {
            PrcPropertyEnd();
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::onData(const String & text)
    {
        mLastPropertyValue += text;
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::cleanBuild()
    {
        // Notes: We could just destroy the root window of the layout, which normally would also destroy
        // all attached windows.  Since the client may have specified that certain windows are not auto-destroyed
        // we can't rely on this, so we work backwards detaching and deleting windows instead.
        while(!mWidgetList.empty())
        {
            // only destroy if not an auto window
            if(mWidgetList.back().second)
            {
                Widget * wnd = mWidgetList.back().first;

                // detach from parent
                if(wnd->getParent())
                {
                    wnd->getParent()->remove(wnd);
                }
                // destroy the window
                N_Only(Widget).destroy(wnd);
            }
            // pop window from stack
            mWidgetList.pop_back();
        }
        mRootWindow = 0;
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcTagUnitCB(const PropertyData * pdc)
    {
        Nui32 version;
        pdc->get(VersionProperty, version, 0);

        if(version != mVersion)
        {
            N_EXCEPT(InvalidRequest,
                _I18n("You are attempting to load a layout of version '") + N_conv(version) +
                _I18n("' but this CEGUI version is only meant to load layouts of version '") +
                N_conv(mVersion) + _I18n("'. Consider using the migrate.py script bundled with CEGUI Unified Editor to migrate your data."));
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcWidgetCB(const PropertyData * pdc)
    {
        // get type of window to create
        Nui32 windowType, windowName;
        pdc->get(TypeProperty, windowType);
        // get name for new window
        pdc->get(NameProperty, windowName);
        // attempt to create window
        try
        {
            Widget * wnd = N_Only(Widget).create(windowType, windowName, 0);

            // add this window to the current parent (if any)
            if (!mWidgetList.empty())
                static_cast<Container *>(mWidgetList.back().first)->add(wnd);
            else
                mRootWindow = wnd;

            // make this window the top of the stack
            mWidgetList.push_back(WidgetObj(wnd,true));

            // tell it that it is being initialised
            wnd->onBeginInit();
        }
        catch (UniqueRepeatException &)
        {
            // delete all windows created
            cleanBuild();

            // signal error - with more info about what we have done.
            N_EXCEPT(InvalidRequest, _I18n("layout loading has been aborted since Widget named '") + 
                N_conv(windowName) + _I18n("' already exists."));
        }
        catch (UnknownObjectException &)
        {
            // delete all windows created
            cleanBuild();

            // signal error - with more info about what we have done.
            N_EXCEPT(InvalidRequest,
                _I18n("layout loading has been aborted since no WidgetFactory is available for '") + 
                N_conv(windowType) + _I18n("' objects."));
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcAutoWidgetCB(const PropertyData * pdc)
    {
        const String & name_path(pdc->get(N_PropertyUIWidget_Name));

        try
        {
            // we need a window to fetch children
            if(!mWidgetList.empty())
            {
                Widget * wnd = static_cast<Container *>(mWidgetList.back().first)->get(name_path);
                // make this window the top of the stack
                mWidgetList.push_back(WidgetObj(wnd,false));
            }
        }
        catch(UnknownObjectException &)
        {
            cleanBuild();
            // signal error - with more info about what we have done.
            N_EXCEPT(InvalidRequest, _I18n("layout loading has been aborted since auto window '") +
                name_path + _I18n("' could not be referenced."));
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcUserDataCB(const PropertyData * pdc)
    {
        Nid userStringName;
        String userStringValue;
        pdc->get(NameProperty, userStringName);
        pdc->get(ValueProperty, userStringValue);

        if(!userStringValue.empty())
        {
            mLastProperty = 0;
            try
            {
                // need a window to be able to set properties!
                if(!mWidgetList.empty())
                {
                    // get current window being defined.
                    Widget * curwindow = mWidgetList.back().first;

                    curwindow->setExtData(userStringName, userStringValue);
                }
            }
            catch(RuntimeException &)
            {
                // Don't do anything here, but the error will have been logged.
            }
        }
        else
        {
            // Store name for later use
            mLastProperty = userStringName;
            // reset the property (user string) value buffer
            mLastPropertyValue.clear();
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcPropertyCB(const PropertyData * pdc)
    {
        PropertyID propertyName;
        String propertyValue;

        pdc->get(NameProperty, propertyName);
        pdc->get(ValueProperty, propertyValue);

        if(!propertyValue.empty())
        {
            mLastProperty = 0;
            try
            {
                // need a window to be able to set properties!
                if(!mWidgetList.empty())
                {
                    // get current window being defined.
                    Widget * curwindow = mWidgetList.back().first;

                    bool useit = true;

                    // if client defined a callback, call it and discover if we should
                    // set the property.
                    if(mPropertyCB)
                        useit = (*mPropertyCB)(curwindow, propertyName,
                            propertyValue, mCustomData);

                    // set the property as needed
                    if(useit)
                        curwindow->set(propertyName, propertyValue);
                }
            }
            catch(RuntimeException &)
            {
                // Don't do anything here, but the error will have been logged.
            }
        }
        else
        {
            // Store name for later use
            mLastProperty = propertyName;
            // reset the property value buffer
            mLastPropertyValue.clear();
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcLayoutImportCB(const PropertyData * pdc)
    {
        try
        {
            Nui32 gid;
            pdc->get(GroupProperty, gid);
            // attempt to load the imported sub-layout
            Widget * subLayout = N_Only(Widget).load(pdc->get(SourceProperty), gid);
                //mPropertyCB, mCustomData);

            // attach the imported layout to the window being defined
            if((subLayout != 0) && (!mWidgetList.empty()))
                static_cast<Container *>(mWidgetList.back().first)->add(subLayout);
        }
        // something failed when loading the sub-layout
        catch(RuntimeException &)
        {
            cleanBuild();
            // signal error - with more info about what we have done.
            N_EXCEPT(Internal, _I18n("layout loading aborted due to imported layout load failure (see error(s) above)."));
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcEventCB(const PropertyData * pdc)
    {
        EventID eid;
        String func;
        pdc->get(NameProperty, eid);
        pdc->get(N_PropertyUILayout_FuncName, func);    // function

        try
        {
            if(!mWidgetList.empty())
                mWidgetList.back().first->addScript(N_ScriptModule_UI, eid, func);
        }
        catch (RuntimeException &)
        {
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcWidgetEndCB()
    {
        if(!mWidgetList.empty())
        {
            mWidgetList.back().first->onEndInit();
            mWidgetList.pop_back();
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcAutoWidgetEnd()
    {
        if(!mWidgetList.empty())
        {
            mWidgetList.pop_back();
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcUserDataEndCB()
    {
        // only do something if this is a "long" user string
        if(mLastProperty == 0)
        {
            return;
        }
        try
        {
            // need a window to be able to set user strings!
            if (!mWidgetList.empty())
            {
                // get current window being defined.
                Widget * curwindow = mWidgetList.back().first;

                curwindow->setExtData(mLastProperty, mLastPropertyValue);
            }
        }
        catch(RuntimeException &)
        {
            // Don't do anything here, but the error will have been logged.
        }
    }
    //------------------------------------------------------------------------
    void UILayoutXmlAnalyze::PrcPropertyEnd()
    {
        // only do something if this is a "long" property
        if (mLastProperty == 0)
        {
            return;
        }
        try
        {
            // need a window to be able to set properties!
            if (!mWidgetList.empty())
            {
                // get current window being defined.
                Widget * curwindow = mWidgetList.back().first;

                bool useit = true;

                // if client defined a callback, call it and discover if we should
                // set the property.
                if (mPropertyCB)
                    useit = (*mPropertyCB)(curwindow, mLastProperty,
                        mLastPropertyValue, mCustomData);
                // set the property as needed
                if (useit)
                    curwindow->set(mLastProperty, mLastPropertyValue);
            }
        }
        catch (RuntimeException &)
        {
            // Don't do anything here, but the error will have been logged.
        }
    }
    //------------------------------------------------------------------------
}
}