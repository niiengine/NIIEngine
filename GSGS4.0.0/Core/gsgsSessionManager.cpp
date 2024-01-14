
#include "gsgsSessionManager.h"
#include "xmlutils.h"

#include <memory>
#include "gsgsPathManager.h"
#include "gsgsConfig.h"
#include "gsgsWorkspaceManager.h"

namespace gsgs
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // SessionEntry
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    SessionEntry::SessionEntry() {}
    //-----------------------------------------------------------------------------
    SessionEntry::~SessionEntry() {}
    //-----------------------------------------------------------------------------
    void SessionEntry::DeSerialize(Serializer& arch)
    {
        arch.Read(wxT("m_selectedTab"), m_selectedTab);
        arch.Read(wxT("m_tabs"), m_tabs);
        arch.Read(wxT("m_workspaceName"), m_workspaceName);
        arch.Read(wxT("m_breakpoints"), (ObjectData*)&m_breakpoints);
        arch.Read(wxT("m_findInFilesMask"), m_findInFilesMask);
        
        arch.Read(wxT("TabInfoArray"), m_vTabInfoArr);
        // initialize tab info array from m_tabs if in config file wasn't yet tab info array
        if(m_vTabInfoArr.size() == 0 && m_tabs.GetCount() > 0) {
            for(size_t i = 0; i < m_tabs.GetCount(); i++) {
                TabInfoData oTabInfo;
                oTabInfo.SetFileName(m_tabs.Item(i));
                oTabInfo.SetFirstVisibleLine(0);
                oTabInfo.SetCurrentLine(0);
                m_vTabInfoArr.push_back(oTabInfo);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void SessionEntry::Serialize(Serializer& arch)
    {
        arch.Write(wxT("m_selectedTab"), m_selectedTab);
        // since tabs are saved in TabInfoArray we don't save tabs as string array,
        // there are only read due to read config saved in older version where wasn't TabInfoArray
        // arch.Write(wxT("m_tabs"), m_tabs);
        arch.Write(wxT("m_workspaceName"), m_workspaceName);
        arch.Write(wxT("TabInfoArray"), m_vTabInfoArr);
        arch.Write(wxT("m_breakpoints"), (ObjectData*)&m_breakpoints);
        arch.Write("m_findInFilesMask", m_findInFilesMask);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // TabGroupEntry
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    void TabGroupEntry::DeSerialize(Serializer& arch)
    {
        arch.Read(wxT("m_TabgroupName"), m_tabgroupName);
        arch.Read(wxT("TabInfoArray"), m_vTabInfoArr);
    }
    //-----------------------------------------------------------------------------
    void TabGroupEntry::Serialize(Serializer& arch)
    {
        arch.Write(wxT("m_TabgroupName"), m_tabgroupName);
        arch.Write(wxT("TabInfoArray"), m_vTabInfoArr);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // SessionManager
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    SessionManager& SessionManager::singleton()
    {
        static SessionManager theManager;
        return theManager;
    }
    //-----------------------------------------------------------------------------
    SessionManager::SessionManager() {}
    //-----------------------------------------------------------------------------
    SessionManager::~SessionManager() {}
    //-----------------------------------------------------------------------------
    bool SessionManager::Load(const wxString& fileName)
    {
        m_fileName = wxFileName(fileName);

        if(!m_fileName.FileExists()) {
            // no such file or directory
            // create an empty one
            wxFFile newFile(fileName, wxT("a+"));
            newFile.Write(wxT("<Sessions/>"));
            newFile.Close();
        }

        m_doc.Load(m_fileName.GetFullPath());
        return m_doc.IsOk();
    }
    //-----------------------------------------------------------------------------
    wxFileName SessionManager::GetSessionFileName(const wxString& name, const wxString& suffix /*=wxT("")*/) const
    {
        if(defaultSessionName == name) {
            wxFileName sessionFileName = wxFileName(gsgs_Path().GetUserDataDir(), "Default.session");
            sessionFileName.AppendDir("config");
            return sessionFileName;

        } else {
            wxFileName sessionFileName(name);
            if(suffix != "tabgroup") {
                sessionFileName.AppendDir(".gsgsworkspace");
            }
            sessionFileName.SetExt(suffix.IsEmpty() ? wxString("session") : suffix);
            return sessionFileName;
        }
    }
    //-----------------------------------------------------------------------------
    bool SessionManager::GetSession(const wxString& workspaceFile, SessionEntry& session, const wxString& suffix, const wxChar* Tag)
    {
        if(!m_doc.GetRoot()) {
            return false;
        }

        wxFileName sessionFileName = GetSessionFileName(workspaceFile, suffix);
        wxXmlDocument doc;
        
        if(sessionFileName.FileExists()) {
            if(!doc.Load(sessionFileName.GetFullPath()) || !doc.IsOk()) return false;
        } else {
            doc.SetRoot(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, Tag));
        }

        wxXmlNode* const node = doc.GetRoot();
        if(!node || node->GetName() != Tag) return false;

        Serializer arch;
        arch.SetXmlNode(node);
        session.DeSerialize(arch);

        return true;
    }
    //-----------------------------------------------------------------------------
    bool SessionManager::Save(const wxString& name, SessionEntry& session, const wxString& suffix, const wxChar* Tag)
    {
        if(!m_doc.GetRoot()) {
            return false;
        }

        if(name.empty()) return false;

        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, Tag);
        child->XmlAddProperty(wxT("Name"), name);

        Serializer arch;
        arch.SetXmlNode(child);
        session.Serialize(arch);

        wxXmlDocument doc;
        doc.SetRoot(child);

        // If we're saving a tabgroup, suffix will be ".tabgroup", not the default ".session"
        const wxFileName& sessionFileName = GetSessionFileName(name, suffix);
        return doc.Save(sessionFileName.GetFullPath());
    }
    //-----------------------------------------------------------------------------
    void SessionManager::SetLastSession(const wxString& name)
    {
        if(!m_doc.GetRoot()) {
            return;
        }
        // first delete the old entry
        wxXmlNode* node = m_doc.GetRoot()->GetChildren();
        while(node) {
            if(node->GetName() == wxT("LastSession")) {
                m_doc.GetRoot()->RemoveChild(node);
                delete node;
                break;
            }
            node = node->GetNext();
        }

        // set new one
        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LastSession"));
        m_doc.GetRoot()->AddChild(child);
        XmlUtils::SetNodeContent(child, name);

        // save changes
        m_doc.Save(m_fileName.GetFullPath());
    }
    //-----------------------------------------------------------------------------
    wxString SessionManager::GetLastSession()
    {
        if(!m_doc.GetRoot()) {
            return defaultSessionName;
        }
        // try to locate the 'LastSession' entry
        // if it does not exist or it exist with value empty return 'Default'
        // otherwise, return its content
        wxXmlNode* node = m_doc.GetRoot()->GetChildren();
        while(node) {
            if(node->GetName() == wxT("LastSession")) {
                if(node->GetNodeContent().IsEmpty()) {
                    return defaultSessionName;
                } else {
                    return node->GetNodeContent();
                }
            }
            node = node->GetNext();
        }
        return defaultSessionName;
    }
    //-----------------------------------------------------------------------------
    void SessionManager::UpdateFindInFilesMaskForCurrentWorkspace(const wxString& mask)
    {
        if(gsgs_Workspace().IsValid()) {
            wxFileName fn = gsgs_Workspace().GetWorkspace()->GetFileName();
            SessionEntry s;
            if(GetSession(fn.GetFullPath(), s)) {
                s.SetFindInFilesMask(mask);
                Save(fn.GetFullPath(), s);
            }
        }
    }
    //-----------------------------------------------------------------------------
    wxString SessionManager::GetFindInFilesMaskForCurrentWorkspace()
    {
        if(gsgs_Workspace().IsValid()) {
            wxFileName fn = gsgs_Workspace().GetWorkspace()->GetFileName();
            SessionEntry s;
            if(GetSession(fn.GetFullPath(), s)) {
                return s.GetFindInFilesMask();
            }
        }
        return "";
    }
    //-----------------------------------------------------------------------------
}