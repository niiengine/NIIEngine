#include "NodeJSWorkspaceConfiguration.h"
#include <algorithm>

NodeJSWorkspaceConfiguration::NodeJSWorkspaceConfiguration(const wxFileName& filename)
    : ObjectData("NodeJS")
    , m_isOk(false)
    , m_showHiddenFiles(false)
    , m_filename(filename)
{
}

NodeJSWorkspaceConfiguration::~NodeJSWorkspaceConfiguration() {}

void NodeJSWorkspaceConfiguration::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("folders").getValue(m_folders);
    ConvertToRelative(m_folders);
    
    json.getObject("m_showHiddenFiles").getValue(m_showHiddenFiles, m_showHiddenFiles);

    m_isOk = false;
    if(json.isObjectExist("metadata")) {
        gsgs::JSONObject e = json.getObject("metadata");
        wxString temp;
        e.getObject("type").getValue(temp);
        if(e.isObjectExist("type")) { m_isOk = (temp == "NodeJS"); }
    }
}

gsgs::JSONObject NodeJSWorkspaceConfiguration::ToJSON() const
{
    // add the metadata
    gsgs::JSONObject json = gsgs::JSONObject::createObject(GetName());
    gsgs::JSONObject metadata = gsgs::JSONObject::createObject("metadata");
    json.arrayAppend(metadata);
    metadata.addProperty("version", "1.0");
    metadata.addProperty("ide", "gsgs");
    metadata.addProperty("type", "NodeJS");

    // now add some real properties
    json.addProperty("folders", m_folders);
    json.addProperty("m_showHiddenFiles", m_showHiddenFiles);
    return json;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::Load()
{
    gsgs::Config conf(m_filename.GetFullPath());
    conf.ReadItem(this);
    return *this;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::Save()
{
    gsgs::Config conf(m_filename.GetFullPath());
    conf.WriteItem(this);
    return *this;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::SetFolders(const wxArrayString& folders)
{
    this->m_folders = folders;
    ConvertToRelative(this->m_folders);
    return *this;
}

wxArrayString NodeJSWorkspaceConfiguration::GetFolders() const
{
    // Convert the folders to absolute path
    wxArrayString folders;
    std::for_each(this->m_folders.begin(), this->m_folders.end(), [&](const wxString& folder) {
        wxFileName fnFolder(folder, "dummy.txt");
        fnFolder.MakeAbsolute(m_filename.GetPath());
        folders.Add(fnFolder.GetPath());
    });
    return folders;
}

void NodeJSWorkspaceConfiguration::ConvertToRelative(wxString& folder) const
{
    wxFileName fnFolder(folder, "dummy.txt");
    if(fnFolder.IsAbsolute()) { fnFolder.MakeRelativeTo(m_filename.GetPath()); }
    folder = fnFolder.GetPath(wxPATH_GET_VOLUME, wxPATH_UNIX);
    if(folder.IsEmpty()) { folder = "."; }
}

void NodeJSWorkspaceConfiguration::ConvertToRelative(wxArrayString& folders) const
{
    for(size_t i = 0; i < folders.size(); ++i) {
        ConvertToRelative(folders.Item(i));
    }
}
