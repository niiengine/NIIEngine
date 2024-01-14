#include "clDockerComposeFile.h"

clDockerComposeFile::clDockerComposeFile(const wxString& path)
    : clDockerBuildableFile(path, eDockerFileType::kDockerCompose)
{
}

clDockerComposeFile::clDockerComposeFile()
    : clDockerBuildableFile("", eDockerFileType::kDockerCompose)
{
}

clDockerComposeFile::~clDockerComposeFile() {}

void clDockerComposeFile::FromJSON(const gsgs::JSONObject& json, const wxString& workspaceDir)
{
    clDockerBuildableFile::FromJSON(json, workspaceDir);
}

gsgs::JSONObject clDockerComposeFile::ToJSON(const wxString& workspaceDir) const
{
    gsgs::JSONObject json = clDockerBuildableFile::ToJSON(workspaceDir);
    return json;
}