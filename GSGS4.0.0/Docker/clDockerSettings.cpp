#include "clDockerSettings.h"
#include "gsgsPathManager.h"
#include "globals.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

clDockerSettings::clDockerSettings()
    : ObjectData("Docker")
{
    // The defaults
    wxArrayString hints;
    hints.Add("/usr/local/bin");
    hints.Add("/usr/bin");
#ifdef __WXMSW__
    {
        wxRegKey regkey(wxRegKey::HKCU, "Environment");
        wxString docker_path;
        if(regkey.QueryValue("DOCKER_TOOLBOX_INSTALL_PATH", docker_path) && wxDirExists(docker_path)) {
            hints.Add(docker_path);
        }
    }
#endif
    ::FindExec("docker", m_docker, hints);
    ::FindExec("docker-compose", m_dockerCompose, hints);
}

clDockerSettings::~clDockerSettings() {}

void clDockerSettings::FromJSON(const JSONObject& json)
{
    wxString temp;
    json.getObject("docker").getValue(temp, m_docker.GetFullPath());
    m_docker = temp;
    json.getObject("docker-compose").getValue(temp, m_dockerCompose.GetFullPath());
    m_dockerCompose = temp;
    json.getObject("flags").getValue(m_flags, m_flags);
}

JSONObject clDockerSettings::ToJSON() const
{
    JSONObject json = JSONObject::createObject(GetName());
    json.addProperty("docker", m_docker.GetFullPath());
    json.addProperty("docker-compose", m_dockerCompose.GetFullPath());
    json.addProperty("flags", m_flags);
    return json;
}

void clDockerSettings::Load()
{
    wxFileName fn(gsgs_Path().GetUserDataDir(), "docker.conf");
    fn.AppendDir("config");
    gsgs::Config conf(fn.GetFullPath());
    conf.ReadItem(this);
}

void clDockerSettings::Save()
{
    wxFileName fn(gsgs_Path().GetUserDataDir(), "docker.conf");
    fn.AppendDir("config");
    gsgs::Config conf(fn.GetFullPath());
    conf.WriteItem(this);
}
