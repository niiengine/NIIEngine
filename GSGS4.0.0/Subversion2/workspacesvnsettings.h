
#ifndef WORKSPACESVNSETTINGS_H
#define WORKSPACESVNSETTINGS_H

#include "gsgsConfig.h"

class WorkspaceSvnSettings : public gsgs::ObjectData
{
public:
    WorkspaceSvnSettings(const wxFileName& fn);
    WorkspaceSvnSettings();
    virtual ~WorkspaceSvnSettings();

    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;

    WorkspaceSvnSettings& Load();
    void Save();

    void SetRepoPath(const wxString& repoPath) {
        this->m_repoPath = repoPath;
    }
    const wxString& GetRepoPath() const {
        return m_repoPath;
    }
    wxFileName GetLocalConfigFile() const;
private:
    wxString m_repoPath;
    wxFileName m_workspaceFileName;
};

#endif // WORKSPACESVNSETTINGS_H
