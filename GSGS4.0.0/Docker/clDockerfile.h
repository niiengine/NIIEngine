#ifndef CLDOCKERFILE_H
#define CLDOCKERFILE_H

#include "clDockerBuildableFile.h"
#include "gsgsJSON.h"

class clDockerfile : public clDockerBuildableFile
{
public:
    clDockerfile(const wxString& path);
    clDockerfile();
    virtual ~clDockerfile();

    void FromJSON(const gsgs::JSONObject& json, const wxString& workspaceDir);
    gsgs::JSONObject ToJSON(const wxString& workspaceDir) const;
};

#endif // CLDOCKERFILE_H
