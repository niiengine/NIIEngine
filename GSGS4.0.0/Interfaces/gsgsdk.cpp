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

#include "imanager.h"
#include "plugin.h"
#if defined(__WXMAC__) || defined(__WXGTK__)
# include <dlfcn.h>
#endif

namespace gsgs
{
    const wxFont gsgsFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("ËÎÌו"));
    const wxFont gsgsTitleFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("ËÎÌו"));
    const wxFont gsgsBigTitleFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("ËÎÌו"));
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // IManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void IManager::AddWorkspaceTab(const wxString& tabLabel)
    {
        if (mWSNameList.Index(tabLabel) == wxNOT_FOUND)
        {
            mWSNameList.Add(tabLabel);
        }
    }
    //------------------------------------------------------------------------
    void IManager::AddOutputTab(const wxString& tabLabel)
    {
        if (mOPNameList.Index(tabLabel) == wxNOT_FOUND)
        {
            mOPNameList.Add(tabLabel);
        }
    }
    //------------------------------------------------------------------------
    void IManager::AddPerspectiveTab(const wxString& tabLabel)
    {
        if (mPTNameList.Index(tabLabel) == wxNOT_FOUND)
        {
            mPTNameList.Add(tabLabel);
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // IPlugin
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    const wxString& Plugin::GetShortName() const { return m_shortName; }
    //------------------------------------------------------------------------
    const wxString& Plugin::GetLongName() const { return m_longName; }

    //------------------------------------------------------------------------
    PluginInfo::PluginInfo()
        : m_flags(kNone)
    {
    }
    //------------------------------------------------------------------------
    PluginInfo::~PluginInfo() {}
    //------------------------------------------------------------------------
    void PluginInfo::FromJSON(const JSONObject& json)
    {
        json.getObject("name").getValue(m_name);
        json.getObject("author").getValue(m_author);
        json.getObject("description").getValue(m_description);
        json.getObject("version").getValue(m_version);
        json.getObject("flags").getValue(m_flags);
        json.getObject("license").getValue(mLicense);
        json.getObject("dependence").getValue(mDependence);
    }
    //------------------------------------------------------------------------
    JSONObject PluginInfo::ToJSON() const
    {
        JSONObject e = JSONObject::createObject();
        e.addProperty("name", m_name);
        e.addProperty("author", m_author);
        e.addProperty("description", m_description);
        e.addProperty("version", m_version);
        e.addProperty("flags", m_flags);
        e.addProperty("license", mLicense);
        e.addProperty("dependence", mDependence);
        return e;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PluginInfoArray
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PluginInfoArray::PluginInfoArray()
        : ObjectData("codelite-plugins")
    {
    }
    //------------------------------------------------------------------------
    PluginInfoArray::~PluginInfoArray() {}
    //------------------------------------------------------------------------
    void PluginInfoArray::DisablePugins(const wxArrayString& plugins) { m_disabledPlugins = plugins; }
    //------------------------------------------------------------------------
    bool PluginInfoArray::CanLoad(const PluginInfo& plugin) const
    {
        if (m_disabledPlugins.Index(plugin.GetName()) != wxNOT_FOUND) {
            // If the plugin is in the "disabled plugins" list - return false
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void PluginInfoArray::FromJSON(const JSONObject& json)
    {
        json.getObject("disabledPlugins").getValue(m_disabledPlugins);
        m_plugins.clear();
        JSONObject arr = json.getObject("installed-plugins");
        for (int i = 0; i < arr.getArraySize(); ++i) {
            PluginInfo pi;
            pi.FromJSON(arr.getArray(i));
            m_plugins.insert(std::make_pair(pi.GetName(), pi));
        }
    }
    //------------------------------------------------------------------------
    JSONObject PluginInfoArray::ToJSON() const
    {
        JSONObject el = JSONObject::createObject(GetName());
        el.addProperty("disabledPlugins", m_disabledPlugins);

        JSONObject arr = JSONObject::createArray("installed-plugins");
        PluginInfo::PluginMap_t::const_iterator iter = m_plugins.begin();
        for (; iter != m_plugins.end(); ++iter) {
            arr.arrayAppend(iter->second.ToJSON());
        }
        el.addProperty(arr);
        return el;
    }
    //------------------------------------------------------------------------
    void PluginInfoArray::AddPlugin(const PluginInfo& plugin)
    {
        if (m_plugins.count(plugin.GetName())) m_plugins.erase(plugin.GetName());

        m_plugins.insert(std::make_pair(plugin.GetName(), plugin));
    }
    //------------------------------------------------------------------------
    void PluginInfoArray::DisablePlugin(const wxString& plugin)
    {
        if (m_disabledPlugins.Index(plugin) == wxNOT_FOUND) m_disabledPlugins.Add(plugin);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DynModule
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DynModule::DynModule()
    #if defined(__WXMAC__) || defined(__WXGTK__)
            :
            m_dllhandle(NULL)
    #endif
    {
    }
    //------------------------------------------------------------------------
    DynModule::~DynModule()
    {
    #if defined(__WXMAC__) || defined(__WXGTK__)
        if(m_dllhandle){
            dlclose(m_dllhandle);
            m_dllhandle = NULL;
        }
    #endif
    }
    //------------------------------------------------------------------------
    bool DynModule::Load(const wxString &name)
    {
        m_error.Clear();
    #if defined (__WXMSW__)
        return m_lib.Load(name, wxDL_NOSHARE);
    #else
        // open the library
    //#if defined(__WXGTK__) && defined (ON_64_BIT)
    //	// on GTK we need to pass RTLD_DEEPBIND otherwise symbols clashes
    //	m_dllhandle = dlopen(_C(name), RTLD_LAZY| RTLD_LOCAL | RTLD_DEEPBIND);
    //#else
        m_dllhandle = dlopen(_C(name), RTLD_LAZY);
    //#endif

        if (!m_dllhandle) {
            m_error = wxString(dlerror(), wxConvUTF8);
            return false;
        }
        return true;
    #endif
    }
    //------------------------------------------------------------------------
    void DynModule::Detach()
    {
        m_error.Clear();
    #if defined (__WXMSW__)
        m_lib.Detach();
    #else
        if (m_dllhandle) {
            dlclose(m_dllhandle);
            m_dllhandle = NULL;
        }
    #endif
    }
    //------------------------------------------------------------------------
    void *DynModule::GetSymbol(const wxString &name, bool *success)
    {
        m_error.Clear();
    #if defined (__WXMSW__)
        bool rc;
        void *symb = m_lib.GetSymbol(name, &rc);
        *success = rc;
        return symb;
    #else
        dlerror(); // reset errors

        // load the symbol
        void *symb = dlsym(m_dllhandle, _C(name));
        if(symb){
            *success = true;
        }else{
            *success = false;
            m_error = wxString(dlerror(), wxConvUTF8);
        }
        return symb;
    #endif
    }
    //------------------------------------------------------------------------
}