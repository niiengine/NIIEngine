#include "clDockerfile.h"

//------------------------------------------------------------------------------------
clDockerfile::clDockerfile(const wxString& path)
    : clDockerBuildableFile(path, eDockerFileType::kDockerfile)
{
}
//------------------------------------------------------------------------------------
clDockerfile::clDockerfile()
    : clDockerBuildableFile("", eDockerFileType::kDockerfile)
{
}
//------------------------------------------------------------------------------------
clDockerfile::~clDockerfile() {}
//------------------------------------------------------------------------------------
void clDockerfile::FromJSON(const gsgs::JSONObject& json, const wxString& workspaceDir)
{
    clDockerBuildableFile::FromJSON(json, workspaceDir);
}
//------------------------------------------------------------------------------------
gsgs::JSONObject clDockerfile::ToJSON(const wxString& workspaceDir) const
{
    gsgs::JSONObject json = clDockerBuildableFile::ToJSON(workspaceDir);
    return json;
}

//------------------------------------------------------------------------------------
