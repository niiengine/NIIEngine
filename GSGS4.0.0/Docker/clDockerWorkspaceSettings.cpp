#include "clDockerWorkspaceSettings.h"

#define DOCKER_VERSION wxString("Docker for gsgs v1.0")

clDockerWorkspaceSettings::clDockerWorkspaceSettings()
    : ObjectData("Docker")
{
}

clDockerWorkspaceSettings::~clDockerWorkspaceSettings() {}

void clDockerWorkspaceSettings::FromJSON(const JSONObject& json)
{
    m_files.clear();
    json.getObject("Version").getValue(m_version);
    JSONObject files = json.getObject("files");
    int filesCount = files.getArraySize();
    for(int i = 0; i < filesCount; ++i) {
        JSONObject fileJson = files.getArray(i);
        int temp;
        fileJson.getObject("type").getValue(temp, (int)eDockerFileType::kDockerfile);
        clDockerBuildableFile::Ptr_t f = clDockerBuildableFile::New((eDockerFileType)temp);
        if(!f) { continue; }
        f->FromJSON(fileJson, m_workspaceFile.GetPath());
        m_files.insert({ f->GetPath(), f });
    }
}

JSONObject clDockerWorkspaceSettings::ToJSON() const
{
    JSONObject json = JSONObject::createObject(GetName());
    json.addProperty("Version", DOCKER_VERSION);
    JSONObject files = JSONObject::createArray("files");
    json.arrayAppend(files);
    std::for_each(m_files.begin(), m_files.end(), [&](const clDockerBuildableFile::Map_t::value_type& vt) {
        files.arrayAppend(vt.second->ToJSON(m_workspaceFile.GetPath()));
    });
    return json;
}

clDockerWorkspaceSettings& clDockerWorkspaceSettings::Load(const wxFileName& filename)
{
    m_workspaceFile = filename;
    gsgs::Config conf(filename.GetFullPath());
    conf.ReadItem(this);
    return *this;
}

clDockerWorkspaceSettings& clDockerWorkspaceSettings::Save(const wxFileName& filename)
{
    m_workspaceFile = filename;
    gsgs::Config conf(filename.GetFullPath());
    conf.WriteItem(this);
    return *this;
}

bool clDockerWorkspaceSettings::IsOk() const { return m_version.Contains("Docker for gsgs"); }

void clDockerWorkspaceSettings::Clear()
{
    m_files.clear();
    m_version.clear();
}

clDockerBuildableFile::Ptr_t clDockerWorkspaceSettings::GetFileInfo(const wxFileName& file) const
{
    if(m_files.count(file.GetFullPath()) == 0) { return clDockerBuildableFile::Ptr_t(); }
    return m_files.find(file.GetFullPath())->second;
}

void clDockerWorkspaceSettings::SetFileInfo(const wxFileName& file, clDockerBuildableFile::Ptr_t info)
{
    if(m_files.count(file.GetFullPath())) { m_files.erase(file.GetFullPath()); }
    m_files.insert({ file.GetFullPath(), info });
}
