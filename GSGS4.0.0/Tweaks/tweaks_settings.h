
#ifndef TWEAKSSETTINGS_H
#define TWEAKSSETTINGS_H

#include "gsgsConfig.h"
#include <wx/colour.h>
#include <map>

class ProjectTweaks : public ObjectData
{
public:
    typedef std::map<wxString, ProjectTweaks> Map_t;

public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;
    ProjectTweaks();
    virtual ~ ProjectTweaks();
    
    void ResetColours();
    void SetBitmapFilename(const wxString& bitmapFilename) { m_bitmapFilename = bitmapFilename; }
    const wxString& GetBitmapFilename() const { return m_bitmapFilename; }
    bool IsOk() const {
        return !m_projectName.IsEmpty() && m_tabBgColour.IsOk() && m_tabFgColour.IsOk();
    }
    void SetTabBgColour(const wxColour& colour) { m_tabBgColour = colour; }
    void SetTabFgColour(const wxColour& colour) { m_tabFgColour = colour; }
    const wxColour& GetTabBgColour() const { return m_tabBgColour; }
    const wxColour& GetTabFgColour() const { return m_tabFgColour; }
    void SetProjectName(const wxString& projectName) { m_projectName = projectName; }
    const wxString& GetProjectName() const { return m_projectName; }
private:
    wxColour m_tabFgColour;
    wxColour m_tabBgColour;
    wxString m_projectName;
    wxString m_bitmapFilename;
};

class TweaksSettings : public ObjectData
{
public:
    enum {
        kDontPromptForProjectReload = 0x00000001
    };
    
public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;

    TweaksSettings();
    virtual ~TweaksSettings();

    TweaksSettings& Load();
    void Clear();
    void ResetColours();
    void Save();
    
    bool HasFlag(int flag) const {
        return m_flags & kDontPromptForProjectReload;
    }
    void EnableFlag(int flag, bool b) {
        if ( b ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
    
    const ProjectTweaks::Map_t& GetProjects() const { return m_projects; }

    void SetGlobalBgColour(const wxColour& colour) { m_globalBgColour = colour; }
    void SetGlobalFgColour(const wxColour& colour) { m_globalFgColour = colour; }
    void SetProjects(const ProjectTweaks::Map_t& projects) { m_projects = projects; }
    const wxColour& GetGlobalBgColour() const { return m_globalBgColour; }
    const wxColour& GetGlobalFgColour() const { return m_globalFgColour; }
    void SetEnableTweaks(bool enableTweaks) { m_enableTweaks = enableTweaks; }
    bool IsEnableTweaks() const { return m_enableTweaks; }
    const ProjectTweaks& GetProjectTweaks(const wxString &project) const;
    ProjectTweaks& GetProjectTweaks(const wxString &project);
    void UpdateProject(const ProjectTweaks& pt);
    void DeleteProject(const wxString& pt);
private:
    ProjectTweaks::Map_t m_projects;
    wxColour             m_globalBgColour;
    wxColour             m_globalFgColour;
    bool                 m_enableTweaks;
    size_t               m_flags;
};

#endif // TWEAKSSETTINGS_H
