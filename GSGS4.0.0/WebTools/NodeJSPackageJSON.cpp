#include "NodeJSPackageJSON.h"
#include "gsgsJSON.h"
#include "NoteJSWorkspace.h"

NodeJSPackageJSON::NodeJSPackageJSON() {}

NodeJSPackageJSON::~NodeJSPackageJSON() {}

bool NodeJSPackageJSON::Load(const wxString& projectPath)
{
    wxFileName filename(projectPath, "package.json");
    filename.AppendDir(".gsgsworkspace");
    if(!filename.FileExists()) {
        return false;
    }
    
    JSON root(filename);
    if(!root.isValid()) return false;

    root.getObject().getObject("name").getValue(m_name);
    root.getObject().getObject("version").getValue(m_version);
    root.getObject().getObject("description").getValue(m_description);
    wxString temp;
    root.getObject().getObject("main").getValue(temp);
    m_script = temp;
    root.getObject().getObject("args").getValue(m_args);
    return true;
}

bool NodeJSPackageJSON::Create(const wxString& projectPath)
{
    wxFileName filename(projectPath, "package.json");
    if(!filename.FileExists()) {
        return false;
    }
    
    JSON root(filename);
    if(!root.isValid()) return false;

    root.getObject().getObject("name").getValue(m_name);
    root.getObject().getObject("version").getValue(m_version);
    root.getObject().getObject("description").getValue(m_description);
    wxString tempstr;
    root.getObject().getObject("main").getValue(tempstr);
    m_script = tempstr;
    // Convert the script into absolute path
    m_script.MakeAbsolute(filename.GetPath());
    
    // Ensure that the folder .gsgsworkspace exists and "move" the file
    // to that folder
    filename.AppendDir(".gsgsworkspace");
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    return Save(projectPath);
}

bool NodeJSPackageJSON::Save(const wxString& projectPath)
{
    wxFileName filename(projectPath, "package.json");
    filename.AppendDir(".gsgsworkspace");
    
    // Override the previous settings
    JSON root(cJSON_Object);
    JSONObject json = root.getObject();
    
    json.addProperty("name", m_name);
    json.addProperty("version", m_version);
    json.addProperty("description", m_description);
    
    if(!m_script.IsAbsolute()) {
        m_script.MakeAbsolute(filename.GetPath());
    }
    json.addProperty("main", m_script.GetFullPath());
    json.addProperty("args", m_args);
    
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    root.save(filename);
    return true;
}
