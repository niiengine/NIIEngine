
#include "workspacesvnsettings.h"
#include "gsgsPathManager.h"
#include "gsgsWorkspace.h"
#include "gsgsConfig.h"

WorkspaceSvnSettings::WorkspaceSvnSettings()
    : gsgs::ObjectData("svn-settings")
{
}

WorkspaceSvnSettings::WorkspaceSvnSettings(const wxFileName& fn)
    : ObjectData("svn-settings")
    , m_workspaceFileName(fn)
{
}

WorkspaceSvnSettings::~WorkspaceSvnSettings()
{
}

void WorkspaceSvnSettings::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_repoPath").getValue(m_repoPath);
}

gsgs::JSONObject WorkspaceSvnSettings::ToJSON() const
{
    JSONObject json = JSONObject::createObject(GetName());
    json.addProperty("m_repoPath", m_repoPath);
    return json;
}

WorkspaceSvnSettings& WorkspaceSvnSettings::Load()
{
    gsgs::Config conf(GetLocalConfigFile().GetFullPath());
    conf.ReadItem( this );
    return *this;
}

void WorkspaceSvnSettings::Save()
{
    gsgs::Config conf(GetLocalConfigFile().GetFullPath());
    conf.WriteItem( this );
}

wxFileName WorkspaceSvnSettings::GetLocalConfigFile() const
{
    if ( m_workspaceFileName.IsOk() && m_workspaceFileName.Exists() ) {
        wxFileName fn(m_workspaceFileName.GetPath(), "subversion.conf");
        fn.AppendDir(".gsgsworkspace");
        return fn;

    } else {
        wxFileName fn(gsgs_Path().GetUserDataDir(), "subversion.conf");
        fn.AppendDir("config");
        return fn;
        
    }
}


