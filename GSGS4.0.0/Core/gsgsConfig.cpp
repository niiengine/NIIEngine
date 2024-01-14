
#include "gsgsConfig.h"
#include "gsgsPathManager.h"
#include "xmlutils.h"
#include "gsgsFileUtil.h"

//---------------------------------------------------------------------------
#define ADD_OBJ_IF_NOT_EXISTS(parent, objName)                  \
    if(!parent.isObjectExist(objName)) {                         \
        JSONObject obj = JSONObject::createObject(objName);         \
        parent.addProperty(obj);                                \
    }

#define ADD_ARR_IF_NOT_EXISTS(parent, arrName)                  \
    if(!parent.isObjectExist(arrName)) {                         \
        JSONObject arr = JSONObject::createArray(arrName);          \
        parent.addProperty(arr);                                \
    }
namespace gsgs
{
    std::map<wxString, wxArrayString> Config::m_cacheRecentItems;
    //---------------------------------------------------------------------------
    Config::Config(const wxString & filename) 
        : m_fileName(wxEmptyString),
        mAutoPermanent(true)
    {
        if(wxFileName(filename).IsAbsolute()) 
        {
            m_filename = filename;
        } 
        else 
        {
            m_filename = gsgs_Path().GetUserDataDir() + gsgsPathSep + "config" +
                gsgsPathSep + filename;
        }
        m_filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        if(m_filename.FileExists()) 
        {
            m_root = new JSON(m_filename);
        } 
        else 
        {
            m_root = new JSON(cJSON_Object);
        }
    }
    //---------------------------------------------------------------------------
    Config::~Config() 
    {
        if (!mAutoPermanent)
            m_root->save(m_filename);
        wxDELETE(m_root); 
    }
    //---------------------------------------------------------------------------
    JSONObject Config::GetRootObject()
    {
        if (m_root)
        {
            return m_root->getObject();
        }
        return JSONObject(0);
    }
    //---------------------------------------------------------------------------
    JSONObject Config::AddObject(const wxString & name)
    {
        DoDeleteProperty(name);
        JSONObject e = JSONObject::createObject(name);
        m_root->addProperty(e);
        return e;
    }
    //---------------------------------------------------------------------------
    JSONObject Config::GetObject(const wxString & name)
    {
        if (m_root)
            return m_root->getObject().getObject(name);
        return JSONObject(0);
    }
    //---------------------------------------------------------------------------
    bool Config::IsObjectExist(const wxString & name)
    {
        if (m_root)
            return m_root->getObject(name).isValid();
        return false;
    }
    //---------------------------------------------------------------------------
    bool Config::GetOutputTabOrder(wxArrayString & tabs, int & selected)
    {
        if(m_root->getObject().isObjectExist("outputTabOrder")) 
        {
            JSONObject element = m_root->getObject().getObject("outputTabOrder");
            element.getObject("tabs").getValue(tabs);
            element.getObject("selected").getValue(selected);
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    void Config::SetOutputTabOrder(const wxArrayString & tabs, int selected, bool permanent)
    {
        DoDeleteProperty("outputTabOrder");
        JSONObject e = JSONObject::createObject("outputTabOrder");
        e.addProperty("tabs", tabs);
        e.addProperty("selected", selected);
        m_root->addProperty(e);
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    bool Config::GetWorkspaceTabOrder(wxArrayString & tabs, int & selected)
    {
        if(m_root->getObject().isObjectExist("workspaceTabOrder")) {
            JSONObject element = m_root->getObject().getObject("workspaceTabOrder");
            element.getObject("tabs").getValue(tabs);
            element.getObject("selected").getValue(selected);
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    void Config::SetWorkspaceTabOrder(const wxArrayString& tabs, int selected, bool permanent)
    {
        DoDeleteProperty("workspaceTabOrder");
        JSONObject e = JSONObject::createObject("workspaceTabOrder");
        e.addProperty("tabs", tabs);
        e.addProperty("selected", selected);
        m_root->addProperty(e);
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::DoDeleteProperty(const wxString& property)
    {
        if(m_root->getObject().isObjectExist(property)) 
        {
            m_root->getObject().removeProperty(property);
        }
    }
    //---------------------------------------------------------------------------
    bool Config::ReadItem(ObjectData* item, const wxString & destitem)
    {
        wxString dname = destitem.IsEmpty() ? item->GetName() : destitem;
        if(m_root->getObject().isObjectExist(dname)) 
        {
            item->FromJSON(m_root->getObject().getObject(dname));
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    void Config::WriteItem(const ObjectData* item, const wxString & destitem)
    {
        wxString dname = destitem.IsEmpty() ? item->GetName() : destitem;
        DoDeleteProperty(dname);
        m_root->addProperty(item->ToJSON());
        if(mAutoPermanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::Reload()
    {
        if(m_filename.FileExists() == false) 
            return;

        delete m_root;
        m_root = new JSON(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::Save()
    {
        if(m_root) 
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::Save(const wxFileName& fn)
    {
        if(m_root) 
            m_root->save(fn);
    }
    //---------------------------------------------------------------------------
    JSONObject Config::GetGeneralSetting()
    {
        if(!m_root->getObject().isObjectExist("General")) {
            JSONObject general = JSONObject::createObject("General");
            m_root->addProperty(general);
        }
        return m_root->getObject().getObject("General");
    }
    //---------------------------------------------------------------------------
    void Config::Write(const wxString & name, bool value)
    {
        JSONObject general = GetGeneralSetting();
        if(general.isObjectExist(name)) {
            general.removeProperty(name);
        }

        general.addProperty(name, value);
        if(mAutoPermanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    bool Config::Read(const wxString & name, bool & out, bool _default)
    {
        JSONObject general = GetGeneralSetting();
        return general.getObject(name).getValue(out, _default);
    }
    //---------------------------------------------------------------------------
    void Config::Write(const wxString & name, int value)
    {
        JSONObject general = GetGeneralSetting();
        if(general.isObjectExist(name)) 
        {
            general.removeProperty(name);
        }

        general.addProperty(name, value);
        if(mAutoPermanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    bool Config::Read(const wxString & name, int & out, int _default)
    {
        JSONObject general = GetGeneralSetting();
        return general.getObject(name).getValue(out, _default);
    }
    //---------------------------------------------------------------------------
    void Config::Write(const wxString& name, const wxString& value)
    {
        JSONObject general = GetGeneralSetting();
        if(general.isObjectExist(name)) 
        {
            general.removeProperty(name);
        }

        general.addProperty(name, value);
        if(mAutoPermanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    bool Config::Read(const wxString& name, wxString & out, const wxString& _default)
    {
        JSONObject general = GetGeneralSetting();
        if(general.getObject(name).isString()) {
            return general.getObject(name).getValue(out);
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    int Config::GetAnnoyingDlgAnswer(const wxString& name, int _default)
    {
        if(m_root->getObject().isObjectExist("AnnoyingDialogsAnswers")) {

            JSONObject element = m_root->getObject().getObject("AnnoyingDialogsAnswers");
            if(element.isObjectExist(name)) {
                int re;
                element.getObject(name).getValue(re, _default);
                return re;
            }
        }
        return _default;
    }
    //---------------------------------------------------------------------------
    void Config::SetAnnoyingDlgAnswer(const wxString& name, int value, bool permanent)
    {
        if(!m_root->getObject().isObjectExist("AnnoyingDialogsAnswers")) {
            JSONObject element = JSONObject::createObject("AnnoyingDialogsAnswers");
            m_root->addProperty(element);
        }

        JSONObject element = m_root->getObject().getObject("AnnoyingDialogsAnswers");
        if(element.isObjectExist(name)) {
            element.removeProperty(name);
        }
        element.addProperty(name, value);
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::ClearAnnoyingDlgAnswers()
    {
        DoDeleteProperty("AnnoyingDialogsAnswers");
        m_root->save(m_filename);
        Reload();
    }
    //---------------------------------------------------------------------------
    void Config::SetQuickFindSearchItems(const wxArrayString& items, bool permanent)
    {
        ADD_OBJ_IF_NOT_EXISTS(m_root->getObject(), "FindHistory");
        JSONObject quickFindBar = m_root->getObject().getObject("FindHistory");
        if(quickFindBar.isObjectExist("SearchHistory")) {
            quickFindBar.removeProperty("SearchHistory");
        }
        quickFindBar.addProperty("SearchHistory", items);
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::SetQuickFindReplaceItems(const wxArrayString& items, bool permanent)
    {
        ADD_OBJ_IF_NOT_EXISTS(m_root->getObject(), "FindHistory");
        JSONObject quickFindBar = m_root->getObject().getObject("FindHistory");
        if(quickFindBar.isObjectExist("ReplaceHistory")) {
            quickFindBar.removeProperty("ReplaceHistory");
        }
        quickFindBar.addProperty("ReplaceHistory", items);
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::AddQuickFindReplaceItem(const wxString& str, bool permanent)
    {
        ADD_OBJ_IF_NOT_EXISTS(m_root->getObject(), "FindHistory");

        JSONObject quickFindBar = m_root->getObject().getObject("FindHistory");
        ADD_ARR_IF_NOT_EXISTS(quickFindBar, "ReplaceHistory");

        JSONObject arr = quickFindBar.getObject("ReplaceHistory");
        wxArrayString items;
        arr.getValue(items);

        // Update the array
        int where = items.Index(str);
        if(where != wxNOT_FOUND) {
            items.RemoveAt(where);
            items.Insert(str, 0);

        } else {
            // remove overflow items if needed
            if(items.GetCount() > 20) {
                // remove last item
                items.RemoveAt(items.GetCount() - 1);
            }
            items.Insert(str, 0);
        }

        quickFindBar.removeProperty("ReplaceHistory");
        quickFindBar.addProperty("ReplaceHistory", items);
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::AddQuickFindSearchItem(const wxString& str, bool permanent)
    {
        ADD_OBJ_IF_NOT_EXISTS(m_root->getObject(), "FindHistory");

        JSONObject quickFindBar = m_root->getObject().getObject("FindHistory");
        ADD_ARR_IF_NOT_EXISTS(quickFindBar, "SearchHistory");

        JSONObject arr = quickFindBar.getObject("SearchHistory");
        wxArrayString items;
        arr.getValue(items);

        // Update the array
        int where = items.Index(str);
        if(where != wxNOT_FOUND) {
            items.RemoveAt(where);
        }
        items.Insert(str, 0);

        // Reudce to size to max of 20
        while(items.size() > 20) {
            items.RemoveAt(items.size() - 1);
        }

        // Update the array
        quickFindBar.removeProperty("SearchHistory");
        quickFindBar.addProperty("SearchHistory", items);
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    wxArrayString Config::GetQuickFindReplaceItems() const
    {
        ADD_OBJ_IF_NOT_EXISTS(m_root->getObject(), "FindHistory");
        JSONObject quickFindBar = m_root->getObject().getObject("FindHistory");
        ADD_ARR_IF_NOT_EXISTS(quickFindBar, "ReplaceHistory");
        wxArrayString re;
        quickFindBar.getObject("ReplaceHistory").getValue(re);
        return re;
    }
    //---------------------------------------------------------------------------
    wxArrayString Config::GetQuickFindSearchItems() const
    {
        ADD_OBJ_IF_NOT_EXISTS(m_root->getObject(), "FindHistory");
        JSONObject quickFindBar = m_root->getObject().getObject("FindHistory");
        ADD_ARR_IF_NOT_EXISTS(quickFindBar, "SearchHistory");
        wxArrayString re;
        quickFindBar.getObject("SearchHistory").getValue(re);
        return re;
    }
    //---------------------------------------------------------------------------
    bool Config::Read(const wxString& name, wxArrayString & out, const wxArrayString& _default)
    {
        JSONObject general = GetGeneralSetting();
        if(general.isObjectExist(name)) {
            return general.getObject(name).getValue(out);
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    void Config::Write(const wxString& name, const wxArrayString& value)
    {
        JSONObject general = GetGeneralSetting();
        if(general.isObjectExist(name)) {
            general.removeProperty(name);
        }

        general.addProperty(name, value);
        if(mAutoPermanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::DoAddRecentItem(const wxString& propName, const wxString& filename, bool permanent)
    {
        wxArrayString recentItems;
        DoGetRecentItems(propName, recentItems);

        // Prepend the item
        if(recentItems.Index(filename) != wxNOT_FOUND) {
            recentItems.Remove(filename);
        }

        if(!wxFileName(filename).FileExists()) {
            // Don't add non existing file
            return;
        }

        recentItems.Insert(filename, 0);

        // Make sure the list does not go over 15 items
        while(recentItems.size() >= 15) {
            recentItems.RemoveAt(recentItems.size() - 1);
        }

        // Remove non existing items
        wxArrayString existingFiles;
        for(size_t i = 0; i < recentItems.size(); ++i) {
            if(wxFileName(recentItems.Item(i)).FileExists()) {
                existingFiles.Add(recentItems.Item(i));
            }
        }
        recentItems.swap(existingFiles);

        // Remove old node if exists
        JSONObject e = m_root->getObject();
        if(e.isObjectExist(propName)) {
            e.removeProperty(propName);
        }

        // append new property
        e.addProperty(propName, recentItems);

        // update the cache
        if(m_cacheRecentItems.count(propName)) 
        {
            m_cacheRecentItems.erase(propName);
        }

        m_cacheRecentItems.insert(std::make_pair(propName, recentItems));
        if(permanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    void Config::DoClearRecentItems(const wxString& propName)
    {
        JSONObject e = m_root->getObject();
        if(e.isObjectExist(propName)) 
        {
            e.removeProperty(propName);
        }
        m_root->save(m_filename);
        // update the cache
        if(m_cacheRecentItems.count(propName)) 
        {
            m_cacheRecentItems.erase(propName);
        }
    }
    //---------------------------------------------------------------------------
    void Config::DoReadRecentItem()
    {
        wxArrayString recentFiles;
        JSONObject e = m_root->getObject();
        if (e.isObjectExist("RecentOpen"))
        {
            e.getObject("RecentOpen").getValue(recentFiles);
            m_cacheRecentItems.insert(std::make_pair("RecentOpen", recentFiles));
        }
    }
    //---------------------------------------------------------------------------
    void Config::DoGetRecentItems(const wxString& propName, wxArrayString & out) const
    {
        // Try the cache first
        if(m_cacheRecentItems.count(propName)) {
            const wxArrayString & temp = m_cacheRecentItems.find(propName)->second;
            wxArrayString::const_iterator i, iend = temp.end();
            for (i = temp.begin(); i != iend; ++i)
            {
                out.push_back(*i);
            }
        } else {
            JSONObject e = m_root->getObject();
            if(e.isObjectExist(propName)) {
                e.getObject(propName).getValue(out, wxArrayString(), false);
            }
        }
    }
    //---------------------------------------------------------------------------
    bool Config::Read(const wxString& name, wxFont & out, const wxFont& _default)
    {
        JSONObject general = GetGeneralSetting();
        if (!general.isObjectExist(name))
        {
            out = _default;
            return false;
        }

        JSONObject f = general.getObject(name);
        int ti;
        wxString ts;
        bool tb1, tb2;
        f.getObject("pointSize").getValue(ti);
        f.getObject("face").getValue(ts);
        f.getObject("bold").getValue(tb1);
        f.getObject("italic").getValue(tb2);
        out.SetPointSize(ti);
        out.SetFaceName(ts);
        out.SetWeight(tb1 ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
        out.SetStyle(tb2 ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
        return true;
    }
    //---------------------------------------------------------------------------
    void Config::Write(const wxString& name, const wxFont& value)
    {
        JSONObject font = JSONObject::createObject(name);
        font.addProperty("pointSize", value.GetPointSize());
        font.addProperty("face", value.GetFaceName());
        font.addProperty("bold", (value.GetWeight() == wxFONTWEIGHT_BOLD));
        font.addProperty("italic", (value.GetStyle() == wxFONTSTYLE_ITALIC));

        JSONObject general = GetGeneralSetting();
        if(general.isObjectExist(name)) {
            general.removeProperty(name);
        }
        general.addProperty(font);
        if(mAutoPermanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    bool Config::Read(const wxString & name, wxColour & out, const wxColour & _default)
    {
        wxString re;
        Read(name, re, wxString());
        if(re.IsEmpty()) {
            if(&out != &_default)
                out = _default;
            return false;
        }
        out = re;
        return true;
    }
    //---------------------------------------------------------------------------
    void Config::Write(const wxString & name, const wxColour & value)
    {
        wxString strValue = value.GetAsString(wxC2S_HTML_SYNTAX);
        Write(name, strValue);
        if(mAutoPermanent)
            m_root->save(m_filename);
    }
    //---------------------------------------------------------------------------
    Config * Config::gInstance = 0;
    //---------------------------------------------------------------------------
    Config & Config::singleton()
    {
        if (gInstance == 0) 
        { 
            gInstance = new Config(); 
            gInstance->setAutoPermanent(false);
            gInstance->DoReadRecentItem();
        }
        return *gInstance;
    }
    //---------------------------------------------------------------------------
    void Config::Release()
    {
        if (gInstance) 
        { 
            delete gInstance; 
        }
        gInstance = 0;
    }
    //---------------------------------------------------------------------------
    bool Config::Load(const wxString &name, const wxString &version)
    {
        wxString initialSettings = gsgs_Path().Locate(name);
        bool loaded = m_doc.Load(initialSettings);
        wxString xmlVersion;
        if (loaded) 
        {
            xmlVersion = m_doc.GetRoot()->XmlGetPropVal(wxT("Version"), wxEmptyString);
        }

        if (xmlVersion != version) 
        {
            loaded = m_doc.Load(gsgs_Path().GetInstallDir() + wxT("/") + name + wxT(".default"));
        }
        m_fileName = gsgs_Path().GetUserDataDir() + wxT("/") +name;
        return loaded;
    }
    //---------------------------------------------------------------------------
    bool Config::Write(const wxString & name, ObjectData *obj)
    {
        if (m_doc.IsOk() == false) 
        {
            return false;
        }

        if (!XmlUtils::StaticWriteObject(m_doc.GetRoot(), name, obj))
            return false;
        return XmlUtils::SaveXmlToFile(&m_doc, m_fileName);
    }
    //---------------------------------------------------------------------------
    bool Config::Read(const wxString & name, ObjectData *obj)
    {
        if (m_doc.IsOk()) 
        {
            return XmlUtils::StaticReadObject(m_doc.GetRoot(), name, obj);
        }
        return false;
    }
    //---------------------------------------------------------------------------
}