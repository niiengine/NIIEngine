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
#include "gsgsPreProcess.h"
#include "gsgsStyleManager.h"
#include "gsgsPathManager.h"
#include "gsgsLogManager.h"
#include "gsgsCommon.h"
#include "gsgsEditorConfig.h"
#include "gsgsEvent.h"
#include "globals.h"
#include "xmlutils.h"
#include <wx/xml/xml.h>
#include "wx/regex.h"
#include "gsgsSerializer.h"
#include "gsgsMacroManager.h"
#include "gsgsWorkspace.h"

namespace gsgs
{
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // EnvMap
    //---------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    EnvMap::EnvMap()
    {
    }
    // --------------------------------------------------------------------------
    EnvMap::~EnvMap()
    {
    }
    // --------------------------------------------------------------------------
    bool EnvMap::Get(const wxString& key, wxString& val)
    {
        int where = m_keys.Index(key);
        if (where == wxNOT_FOUND)
            return false;

        val = m_values.Item(where);
        return true;
    }
    // --------------------------------------------------------------------------
    void EnvMap::Put(const wxString& key, const wxString& val)
    {
        // We allow duplicate entries to exists
        m_keys.Add(key);
        m_values.Add(val);
    }
    // --------------------------------------------------------------------------
    void EnvMap::Clear()
    {
        m_keys.Clear();
        m_values.Clear();
    }
    // --------------------------------------------------------------------------
    size_t EnvMap::GetCount()
    {
        return m_keys.GetCount();
    }
    // --------------------------------------------------------------------------
    bool EnvMap::Get(size_t index, wxString& key, wxString& val)
    {
        if (index >= m_keys.GetCount())
            return false;

        key = m_keys.Item(index);
        val = m_values.Item(index);
        return true;
    }
    // --------------------------------------------------------------------------
    bool EnvMap::Contains(const wxString& key)
    {
        return m_keys.Index(key) != wxNOT_FOUND;
    }
    // --------------------------------------------------------------------------
    wxString EnvMap::String()
    {
        wxString s;
        for (size_t i = 0; i < m_keys.GetCount(); i++)
        {
            s << m_keys.Item(i) << wxT("=") << m_values.Item(i) << wxT("\n");
        }

        if (s.IsEmpty() == false)
            s.RemoveLast();

        return s;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // EvnVarList
    //---------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    EvnVarList::EvnVarList()
        : mCurrent(wxT("Default"))
    {
    }
    // --------------------------------------------------------------------------
    EvnVarList::~EvnVarList() {}
    // --------------------------------------------------------------------------
    void EvnVarList::DeSerialize(Serializer& arch)
    {
        arch.Read(wxT("EnvList"), mEnvList);
        arch.Read(wxT("Current"), mCurrent);
    }
    // --------------------------------------------------------------------------
    void EvnVarList::Serialize(Serializer& arch)
    {
        arch.Write(wxT("EnvList"), mEnvList);
        arch.Write(wxT("Current"), mCurrent);
    }
    // --------------------------------------------------------------------------
    void EvnVarList::AddVariable(const wxString& setName, const wxString& name, const wxString& value)
    {
        wxString newEntry, actualSetName;
        newEntry << name << wxT("=") << value;

        wxString currentValueStr = DoGetSetVariablesStr(setName, actualSetName);
        wxArrayString currentValues = wxStringTokenize(currentValueStr, wxT("\r\n"), wxTOKEN_STRTOK);

        if (currentValues.Index(newEntry) == wxNOT_FOUND) currentValues.Add(newEntry);

        currentValueStr.Clear();
        for (size_t i = 0; i < currentValues.GetCount(); i++)
        {
            currentValueStr << currentValues.Item(i) << wxT("\n");
        }

        if (currentValueStr.IsEmpty() == false)
            currentValueStr.RemoveLast();

        mEnvList[actualSetName] = currentValueStr;
    }
    // --------------------------------------------------------------------------
    void EvnVarList::InsertVariable(const wxString& setName, const wxString& name, const wxString& value)
    {
        wxString actualSetName;

        DoGetSetVariablesStr(setName, actualSetName);

        EnvMap set = GetVariables(actualSetName, false, wxEmptyString, wxEmptyString);
        if (!set.Contains(name))
        {
            set.Put(name, value);
        }
        mEnvList[actualSetName] = set.String();
    }
    // --------------------------------------------------------------------------
    EnvMap EvnVarList::GetVariables(const wxString& setName, bool includeWorkspaceEnvs, const wxString& projectName,
        const wxString& configName)
    {
        EnvMap variables;
        wxString actualSetName;

        wxString currentValueStr = DoGetSetVariablesStr(setName, actualSetName);

        if (includeWorkspaceEnvs && !gsgs_CppWorker().GetName().IsEmpty())
        {
            currentValueStr.Trim().Trim(false);
            currentValueStr << wxT("\n");
            currentValueStr << gsgs_CppWorker().GetEnvironmentVariabels();

            if (projectName.IsEmpty() == false)
            {
                currentValueStr.Trim().Trim(false);
                BuildConfigPtr buildConf = gsgs_CppWorker().GetProjBuildConf(projectName, configName);
                if (buildConf) {
                    currentValueStr << wxT("\n");
                    currentValueStr << buildConf->GetEnvvars();
                }
            }
        }

        wxArrayString currentValues = wxStringTokenize(currentValueStr, wxT("\r\n"), wxTOKEN_STRTOK);
        for (size_t i = 0; i < currentValues.GetCount(); i++)
        {
            wxString entry = currentValues.Item(i);

            // remove any comment from the line
            int where = entry.Find(wxT("#"));
            if (where != wxNOT_FOUND) {
                entry = entry.Left(where);
            }

            entry.Trim().Trim(false);
            if (entry.IsEmpty()) {
                continue;
            }

            wxString varname = entry.BeforeFirst(wxT('='));
            wxString varvalue = entry.AfterFirst(wxT('='));

            // Expand macros (which are not environment variables)
            varvalue = MacroManager::Instance()->ExpandNoEnv(varvalue, projectName, configName);
            variables.Put(varname, varvalue);
        }
        return variables;
    }
    // --------------------------------------------------------------------------
    wxString EvnVarList::DoGetSetVariablesStr(const wxString& setName, wxString& selectedSetName)
    {
        wxString currentValueStr;

        selectedSetName = setName;
        gsgsStringMap::iterator iter = mEnvList.find(setName);
        if (iter != mEnvList.end())
            currentValueStr = iter->second;
        else
        {
            iter = mEnvList.find(mCurrent);
            if (iter != mEnvList.end())
            {
                currentValueStr = iter->second;
                selectedSetName = mCurrent;
            }
            else
            {
                selectedSetName = wxT("Default");
                iter = mEnvList.find(selectedSetName);
                if (iter != mEnvList.end()) currentValueStr = iter->second;
            }
        }
        return currentValueStr;
    }
    // --------------------------------------------------------------------------
    bool EvnVarList::IsSetExist(const wxString & setName)
    {
        return mEnvList.find(setName) != mEnvList.end();
    }
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    // DollarEscaper
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    DollarEscaper::DollarEscaper(wxString& str) :
        m_str(str)
    {
        m_str.Replace("$$", "@@ESC_DOLLAR@@");
    }
    // --------------------------------------------------------------------------
    DollarEscaper::~DollarEscaper()
    {
        // we restore it to non escaped $ !!
        m_str.Replace("@@ESC_DOLLAR@@", "$");
    }
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    // SimpleLongValue
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    const wxString __NO_SUCH_ENV__ = wxT("__NO_SUCH_ENV__");
    // --------------------------------------------------------------------------
    SimpleLongValue::SimpleLongValue() {}
    //--------------------------------------------------------------------------
    SimpleLongValue::~SimpleLongValue() {}
    // --------------------------------------------------------------------------
    void SimpleLongValue::Serialize(Serializer& arch) { arch.Write(wxT("m_value"), m_value); }
    //--------------------------------------------------------------------------
    void SimpleLongValue::DeSerialize(Serializer& arch) { arch.Read(wxT("m_value"), m_value); }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // SimpleStringValue
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    SimpleStringValue::SimpleStringValue() {}
    //--------------------------------------------------------------------------
    SimpleStringValue::~SimpleStringValue() {}
    //--------------------------------------------------------------------------
    void SimpleStringValue::Serialize(Serializer& arch) { arch.Write(wxT("m_value"), m_value); }
    //--------------------------------------------------------------------------
    void SimpleStringValue::DeSerialize(Serializer& arch) { arch.Read(wxT("m_value"), m_value); }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // SimpleRectValue
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    SimpleRectValue::SimpleRectValue() {}
    //--------------------------------------------------------------------------
    SimpleRectValue::~SimpleRectValue() {}
    //--------------------------------------------------------------------------
    void SimpleRectValue::DeSerialize(Serializer& arch)
    {
        wxPoint pos;
        wxSize size;

        arch.Read(wxT("TopLeft"), pos);
        arch.Read(wxT("Size"), size);

        m_rect = wxRect(pos, size);
    }
    //--------------------------------------------------------------------------
    void SimpleRectValue::Serialize(Serializer& arch)
    {
        arch.Write(wxT("TopLeft"), m_rect.GetTopLeft());
        arch.Write(wxT("Size"), m_rect.GetSize());
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorConfig::EditorConfig()
        : m_transcation(false),
        m_envApplied(0),
        mConfig(0)
    {
        m_doc = new wxXmlDocument();
    }
    //--------------------------------------------------------------------------
    EditorConfig::~EditorConfig()
    {
        wxDELETE(m_doc);
        if (mConfig)
            delete mConfig;
    }
    //--------------------------------------------------------------------------
    bool EditorConfig::DoLoadDefaultSettings()
    {
        // try to load the default settings
        m_fileName = wxFileName(m_installDir + wxT("/config/gsgs.xml.default"));
        m_fileName.MakeAbsolute();

        if (!m_fileName.FileExists())
        {
            return false;
        }

        return m_doc->Load(m_fileName.GetFullPath());
    }
    //--------------------------------------------------------------------------
    bool EditorConfig::Load()
    {
        m_cacheLongValues.clear();
        m_cacheStringValues.clear();

        // first try to load the user's settings
        m_fileName = gsgs_Path().GetUserDataDir() + gsgsPathSep + wxT("config/gsgs.xml");
        wxString localFileName = m_fileName.GetFullPath();

        {
            // Make sure that the directory exists
            wxLogNull noLog;
            wxMkdir(m_fileName.GetPath());
            wxMkdir(gsgs_Path().GetUserDataDir() + gsgsPathSep + wxT("lexers"));
        }

        bool userSettingsLoaded(false);
        bool loadSuccess(false);

        if (!m_fileName.FileExists())
        {
            loadSuccess = DoLoadDefaultSettings();

            if (loadSuccess)
            {
                // Copy the content of the default gsgs.xml file into the user's local file
                wxCopyFile(m_fileName.GetFullPath(), localFileName);
            }

        }
        else
        {
            userSettingsLoaded = true;
            loadSuccess = m_doc->Load(m_fileName.GetFullPath());
        }

        if (!loadSuccess) { return false; }

        // Check the gsgs-version for this file
        wxString version;
        bool found = m_doc->GetRoot()->XmlGetPropVal(wxT("Version"), &version);
        if (userSettingsLoaded) {
            if (!found || (found && version != this->m_version)) {
                if (DoLoadDefaultSettings() == false) { return false; }
            }
        }

        // load gsgs lexers
        // LoadLexers(false);

        // make sure that the file name is set to .xml and not .default
        m_fileName = gsgs_Path().GetUserDataDir() + gsgsPathSep + wxT("config/gsgs.xml");
        return true;
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SaveLexers() 
    { 
        gsgs_Style().Save(); 
    }
    //--------------------------------------------------------------------------
    wxXmlNode* EditorConfig::GetLexerNode(const wxString& lexerName)
    {
        wxXmlNode* lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
        if (lexersNode) { return XmlUtils::FindNodeByName(lexersNode, wxT("Lexer"), lexerName); }
        return NULL;
    }
    //--------------------------------------------------------------------------
    LexerStyle::Ptr_t EditorConfig::GetLexerForFile(const wxString& filename)
    {
        return gsgs_Style().GetLexerForFile(filename);
    }
    //--------------------------------------------------------------------------
    LexerStyle::Ptr_t EditorConfig::GetLexer(const wxString& lexerName)
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer(lexerName);
        return lexer;
    }
    //--------------------------------------------------------------------------
    wxString EditorConfig::GetCurrentOutputviewFgColour() const
    {
        return gsgs_Style().GetOutputPaneFgColour().GetAsString(wxC2S_HTML_SYNTAX | wxC2S_CSS_SYNTAX);
    }
    //--------------------------------------------------------------------------
    wxString EditorConfig::GetCurrentOutputviewBgColour() const
    {
        return gsgs_Style().GetOutputPaneBgColour().GetAsString(wxC2S_HTML_SYNTAX | wxC2S_CSS_SYNTAX);
    }
    // --------------------------------------------------------------------------
    wxString EditorConfig::ExpandVariables(const wxString& in, bool applyEnvironment)
    {
        EnvSetter* env = NULL;
        if (applyEnvironment)
        {
            env = new EnvSetter();
        }
        wxString expandedValue = DoExpandVariables(in);

        wxDELETE(env);
        return expandedValue;
    }
    // --------------------------------------------------------------------------
    void EditorConfig::ApplyEnv(gsgsStringMap* overrideMap, const wxString& project, const wxString& config)
    {
        // We lock the CS here and it will be released in UnApplyEnv
        // this is safe to call without Locker since the UnApplyEnv
        // will always be called after ApplyEnv (ApplyEnv and UnApplyEnv are
        // protected functions that can only be called from EnvSetter class
        // which always call UnApplyEnv in its destructor)
        m_cs.Enter();
        ++m_envApplied;

        if (m_envApplied > 1)
        {
            // gsgs_Debug("Thread-%d: Applying environment variables... (not needed)", (int)wxThread::GetCurrentId());
            return;
        }

        // gsgs_Debug("Thread-%d: Applying environment variables...", (int)wxThread::GetCurrentId());
        // read the environments variables
        EvnVarList vars;
        ReadObject(wxT("Variables"), &vars);

        // get the active environment variables set
        EnvMap variables = vars.GetVariables(wxEmptyString, true, project, config);

        // if we have an "override map" place all the entries from the override map
        // into the global map before applying the environment
        if (overrideMap)
        {
            gsgsStringMap::iterator it = overrideMap->begin();
            for (; it != overrideMap->end(); it++)
            {
                variables.Put(it->first, it->second);
            }
        }

        m_envSnapshot.clear();
        for (size_t i = 0; i < variables.GetCount(); i++)
        {

            wxString key, val;
            variables.Get(i, key, val);

            // keep old value before changing it
            wxString oldVal(wxEmptyString);
            if (wxGetEnv(key, &oldVal) == false)
            {
                oldVal = __NO_SUCH_ENV__;
            }

            // keep the old value, however, don't override it if it
            // already exists as it might cause the variable to grow in size...
            // Simple case:
            // PATH=$(PATH);\New\Path
            // PATH=$(PATH);\Another\New\Path
            // If we replace the value, PATH will contain the original PATH + \New\Path
            if (m_envSnapshot.count(key) == 0)
            {
                m_envSnapshot.insert(std::make_pair(key, oldVal));
            }

            // Incase this line contains other environment variables, expand them before setting this environment variable
            wxString newVal = DoExpandVariables(val);

            // set the new value
            wxSetEnv(key, newVal);
        }
    }
    // --------------------------------------------------------------------------
    void EditorConfig::UnApplyEnv()
    {
        --m_envApplied;
        if (m_envApplied == 0)
        {
            // gsgs_Debug("Thread-%d: Un-Applying environment variables", (int)wxThread::GetCurrentId());
            // loop over the old values and restore them
            gsgsStringMap::iterator iter = m_envSnapshot.begin();
            for (; iter != m_envSnapshot.end(); iter++) {
                wxString key = iter->first;
                wxString value = iter->second;
                if (value == __NO_SUCH_ENV__) {
                    // Remove the environment completely
                    ::wxUnsetEnv(key);
                }
                else {
                    // Restore old value
                    ::wxSetEnv(key, value);
                }
            }
            m_envSnapshot.clear();
        }
        m_cs.Leave();
    }
    // --------------------------------------------------------------------------
    wxString EditorConfig::DoExpandVariables(const wxString& in)
    {
        wxString result(in);
        wxString varName, text;

        DollarEscaper de(result);
        gsgsStringMap unresolvedVars;
        int counter(0);
        while (MacroManager::Instance()->FindVariable(result, varName, text))
        {
            wxString replacement;
            if (varName == wxT("MAKE"))
            {
                // ignore this variable, since it is probably was passed here
                // by the makefile generator
                replacement = wxT("___MAKE___");

            }
            else
            {
                // search for an environment with this name
                if (!wxGetEnv(varName, &replacement))
                {
                    replacement << "__unresolved__var__" << ++counter;
                    unresolvedVars.insert(std::make_pair(replacement, text));
                }
            }

            // dont allow recrusive replacements
            if (replacement.Contains(text)) { break; }
            result.Replace(text, replacement);
        }

        // restore the ___MAKE___ back to $(MAKE)
        result.Replace(wxT("___MAKE___"), wxT("$(MAKE)"));

        // and restore all those unresolved variables
        std::for_each(unresolvedVars.begin(), unresolvedVars.end(),
            [&](const std::pair<wxString, wxString>& p) { result.Replace(p.first, p.second); });
        return result;
    }
    // --------------------------------------------------------------------------
    wxArrayString EditorConfig::GetActiveSetEnvNames(bool includeWorkspace, const wxString& project)
    {
        // read the environments variables
        EvnVarList vars;
        ReadObject(wxT("Variables"), &vars);

        wxArrayString envnames;
        // get the active environment variables set
        EnvMap variables = vars.GetVariables(wxEmptyString, includeWorkspace, project, wxEmptyString);
        for (size_t i = 0; i < variables.GetCount(); ++i)
        {
            wxString key, val;
            variables.Get(i, key, val);
            envnames.Add(key);
        }
        return envnames;
    }
    //--------------------------------------------------------------------------
    OptionsConfig * EditorConfig::GetOptions() const
    {
        if (!mConfig)
        {
            wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
            // node can be null ...
            mConfig = new OptionsConfig(node);

            // import legacy tab-width setting into opts
            long tabWidth = const_cast<EditorConfig*>(this)->GetInteger(wxT("EditorTabWidth"), -1);
            if (tabWidth != -1) 
            { 
                mConfig->SetTabWidth(tabWidth); 
            }
        }
        return mConfig;
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetOptions(OptionsConfig * opts)
    {
        // remove legacy tab-width setting
        wxXmlNode* child = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("Info"), wxT("EditorTabWidth"));
        if (child) {
            m_doc->GetRoot()->RemoveChild(child);
            delete child;
        }

        // locate the current node
        wxString nodeName = wxT("Options");
        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
        if (node) {
            m_doc->GetRoot()->RemoveChild(node);
            delete node;
        }
        m_doc->GetRoot()->AddChild(opts->ToXml());

        DoSave(nodeName);
        //wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
        //evt.SetString(nodeName);
        //gsgs_Event().AddPendingEvent(evt);
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetTagsDatabase(const wxString& dbName)
    {
        wxString nodeName = wxT("TagsDatabase");
        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
        if (node) {
            XmlUtils::UpdateProperty(node, wxT("Path"), dbName);
        }
        else {
            // create new node
            node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, nodeName);
            node->XmlAddProperty(wxT("Path"), dbName);
            m_doc->GetRoot()->AddChild(node);
        }

        DoSave(nodeName);
        //wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
        //evt.SetString(nodeName);
        //gsgs_Event().AddPendingEvent(evt);
    }
    //--------------------------------------------------------------------------
    wxString EditorConfig::GetTagsDatabase() const
    {
        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("TagsDatabase"));
        if (node) {
            return XmlUtils::ReadString(node, wxT("Path"));
        }
        else {
            return wxEmptyString;
        }
    }
    //--------------------------------------------------------------------------
    int clSortStringsFunc(const wxString& first, const wxString& second)
    {
        wxFileName f1(first);
        wxFileName f2(second);
        return f2.GetFullName().CmpNoCase(f1.GetFullName());
    }
    //--------------------------------------------------------------------------
    void EditorConfig::GetRecentItems(wxArrayString& files, const wxString& nodeName)
    {
        if (nodeName.IsEmpty()) { return; }

        if (m_cacheRecentItems.count(nodeName)) {
            files = m_cacheRecentItems.find(nodeName)->second;
            // files.Sort(clSortStringsFunc);
            return;
        }

        // find the root node
        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
        if (node) {
            wxXmlNode* child = node->GetChildren();
            while (child) {
                if (child->GetName() == wxT("File")) {
                    wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
                    // wxXmlDocument Saves/Loads items in reverse order, so prepend, not add.
                    if (wxFileExists(fileName)) files.Insert(fileName, 0);
                }
                child = child->GetNext();
            }
        }
        // files.Sort(clSortStringsFunc);
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetRecentItems(const wxArrayString& files, const wxString& nodeName)
    {
        if (nodeName.IsEmpty()) { return; }

        // find the root node
        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), nodeName);
        if (node) {
            wxXmlNode* root = m_doc->GetRoot();
            root->RemoveChild(node);
            delete node;
        }
        // create new entry in the configuration file
        node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, nodeName);
        m_doc->GetRoot()->AddChild(node);
        for (size_t i = 0; i < files.GetCount(); i++) {
            wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
            child->XmlAddProperty(wxT("Name"), files.Item(i));
            node->AddChild(child);
        }

        // Update the cache
        if (m_cacheRecentItems.count(nodeName)) { m_cacheRecentItems.erase(nodeName); }
        m_cacheRecentItems.insert(std::make_pair(nodeName, files));

        // save the data to disk
        DoSave(nodeName);
        //wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
        //evt.SetString(nodeName);
        //gsgs_Event().AddPendingEvent(evt);
    }
    //--------------------------------------------------------------------------
    bool EditorConfig::WriteObject(const wxString & name, ObjectData * obj, bool event)
    {
        if (m_doc->IsOk())
        {
            if (!XmlUtils::StaticWriteObject(m_doc->GetRoot(), name, obj))
                return false;

            //bool res = DoSave();
            if (event)
            {
                wxCommandEvent evt(wxEVT_EDITOR_CONFIG_CHANGED);
                evt.SetString(name);
                gsgs_Event().AddPendingEvent(evt);
            }
            //return res;
            return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------
    bool EditorConfig::ReadObject(const wxString & name, ObjectData * obj)
    {
        if (m_doc->IsOk())
        {
            return XmlUtils::StaticReadObject(m_doc->GetRoot(), name, obj);
        }
        return false;
    }
    //--------------------------------------------------------------------------
    wxString EditorConfig::GetRevision() const
    {
        return XmlUtils::ReadString(m_doc->GetRoot(), wxT("Revision"), wxEmptyString);
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetRevision(const wxString & rev)
    {
        wxXmlNode * root = m_doc->GetRoot();
        if (root)
        {
            XmlUtils::UpdateProperty(root, wxT("Revision"), rev);
            DoSave(wxT("Revision"));
        }
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetInteger(const wxString & name, long value)
    {
        SimpleLongValue data;
        data.SetValue(value);
        WriteObject(name, &data);
        m_cacheLongValues[name] = value;
    }
    //--------------------------------------------------------------------------
    long EditorConfig::GetInteger(const wxString & name, long defaultValue)
    {
        // Check the cache first
        std::map<wxString, long>::iterator iter = m_cacheLongValues.find(name);
        if (iter != m_cacheLongValues.end()) return iter->second;

        SimpleLongValue data;
        if (!ReadObject(name, &data)) { return defaultValue; }

        // update the cache
        m_cacheLongValues[name] = data.GetValue();
        return data.GetValue();
    }
    //--------------------------------------------------------------------------
    wxString EditorConfig::GetString(const wxString & key, const wxString & defaultValue)
    {
        // Check the cache first
        std::map<wxString, wxString>::iterator iter = m_cacheStringValues.find(key);
        if (iter != m_cacheStringValues.end()) return iter->second;

        SimpleStringValue data;
        if (!ReadObject(key, &data)) { return defaultValue; }

        m_cacheStringValues[key] = data.GetValue();
        return data.GetValue();
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetString(const wxString& key, const wxString& value)
    {
        SimpleStringValue data;
        data.SetValue(value);
        WriteObject(key, &data);
        // update the cache
        m_cacheStringValues[key] = value;
    }
    //--------------------------------------------------------------------------
    void EditorConfig::Begin()
    {
        m_transcation = true;
    }
    //--------------------------------------------------------------------------
    void EditorConfig::Save()
    {
        m_transcation = false;
        DoSave();
    }
    //--------------------------------------------------------------------------
    bool EditorConfig::DoSave(const wxString & node) const
    {
        if (m_transcation)
        {
            return true;
        }

        // Notify that the editor configuration was modified
        wxCommandEvent event(wxEVT_EDITOR_CONFIG_CHANGED);
        gsgs_Event().AddPendingEvent(event);
        return XmlUtils::SaveXmlToFile(m_doc, m_fileName.GetFullPath());
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetInstallDir(const wxString& instlDir) 
    { 
        m_installDir = instlDir; 
    }
    //--------------------------------------------------------------------------
    bool EditorConfig::GetPaneStickiness(const wxString& caption)
    {
        if (caption == _("Build")) {
            return GetOptions()->GetHideOutputPaneNotIfBuild();

        }
        else if (caption == _("Search")) {
            return GetOptions()->GetHideOutputPaneNotIfSearch();
        }
        else if (caption == _("Replace")) {
            return GetOptions()->GetHideOutputPaneNotIfReplace();
        }
        else if (caption == _("References")) {
            return GetOptions()->GetHideOutputPaneNotIfReferences();
        }
        else if (caption == _("Output")) {
            return GetOptions()->GetHideOutputPaneNotIfOutput();
        }
        else if (caption == _("Debug")) {
            return GetOptions()->GetHideOutputPaneNotIfDebug();
        }
        else if (caption == _("Trace")) {
            return GetOptions()->GetHideOutputPaneNotIfTrace();
        }
        else if (caption == _("Tasks")) {
            return GetOptions()->GetHideOutputPaneNotIfTasks();
        }
        else if (caption == _("BuildQ")) {
            return GetOptions()->GetHideOutputPaneNotIfBuildQ();
        }
        else if (caption == _("CppCheck")) {
            return GetOptions()->GetHideOutputPaneNotIfCppCheck();
        }
        else if (caption == _("Subversion")) {
            return GetOptions()->GetHideOutputPaneNotIfSvn();
        }
        else if (caption == _("CScope")) {
            return GetOptions()->GetHideOutputPaneNotIfCscope();
        }
        else if (caption == _("Git")) {
            return GetOptions()->GetHideOutputPaneNotIfGit();
        }
        else if (caption == _("MemCheck")) {
            return GetOptions()->GetHideOutputPaneNotIfMemCheck();
        }

        // How did we get here?
        return false;
    }
    //--------------------------------------------------------------------------
    void EditorConfig::SetPaneStickiness(const wxString& caption, bool stickiness)
    {
        OptionsConfig * options = GetOptions();
        if (caption == _("Build")) {
            options->SetHideOutputPaneNotIfBuild(stickiness);
        }
        else if (caption == _("Search")) {
            options->SetHideOutputPaneNotIfSearch(stickiness);
        }
        else if (caption == _("Replace")) {
            options->SetHideOutputPaneNotIfReplace(stickiness);
        }
        else if (caption == _("References")) {
            options->SetHideOutputPaneNotIfReferences(stickiness);
        }
        else if (caption == _("Output")) {
            options->SetHideOutputPaneNotIfOutput(stickiness);
        }
        else if (caption == _("Debug")) {
            options->SetHideOutputPaneNotIfDebug(stickiness);
        }
        else if (caption == _("Trace")) {
            options->SetHideOutputPaneNotIfTrace(stickiness);
        }
        else if (caption == _("Tasks")) {
            options->SetHideOutputPaneNotIfTasks(stickiness);
        }
        else if (caption == _("BuildQ")) {
            options->SetHideOutputPaneNotIfBuildQ(stickiness);
        }
        else if (caption == _("CppCheck")) {
            options->SetHideOutputPaneNotIfCppCheck(stickiness);
        }
        else if (caption == _("Subversion")) {
            options->SetHideOutputPaneNotIfSvn(stickiness);
        }
        else if (caption == _("Cscope")) {
            options->SetHideOutputPaneNotIfCscope(stickiness);
        }
        else if (caption == _("Git")) {
            options->SetHideOutputPaneNotIfGit(stickiness);
        }
        else if (caption == _("MemCheck")) {
            options->SetHideOutputPaneNotIfMemCheck(stickiness);
        }
        else {
            return;
        }

        SetOptions(options);
        Save();
    }
    //--------------------------------------------------------------------------
    // Singleton
    static EditorConfig* gs_EditorConfig = NULL;
    //--------------------------------------------------------------------------
    void EditorConfig::Release()
    {
        if (gs_EditorConfig) 
        {
            delete gs_EditorConfig;
            gs_EditorConfig = NULL;
        }
    }
    //--------------------------------------------------------------------------
    EditorConfig & EditorConfig::singleton()
    {
        if (gs_EditorConfig == NULL) 
        { 
            gs_EditorConfig = new EditorConfig; 
        }
        return *gs_EditorConfig;
    }
    //--------------------------------------------------------------------------
}