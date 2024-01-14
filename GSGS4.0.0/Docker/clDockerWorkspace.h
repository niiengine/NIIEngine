#ifndef CLDOCKERWORKSPACE_H
#define CLDOCKERWORKSPACE_H

#include "IWorkspace.h"
#include "clDockerDriver.h"
#include "clDockerWorkspaceSettings.h"
#include "gsgsEvent.h"

class clDockerWorkspaceView;
class clDockerWorkspace : public IWorkspace
{
public:
    clDockerWorkspace(bool bindEvents, Docker* plugin, clDockerDriver::Ptr_t driver);
    virtual ~clDockerWorkspace();

    static clDockerWorkspace* Get();
    static void Initialise(Docker* plugin);
    static void Shutdown();
    
    clDockerWorkspaceView* GetView() { return m_view; }

    /// @copydetails IWorkspace::GetActiveProjectName
    virtual wxString GetActiveProjectName() const;
    
    /// @copydetails IWorkspace::GetFileName
    virtual wxFileName GetFileName() const;
    
    /// @copydetails IWorkspace::GetFilesMask
    virtual wxString GetFilesMask() const;
    
    /// @copydetails IWorkspace::GetProjectFileName
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    
    /// @copydetails IWorkspace::GetProjectFiles
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    
    /// @copydetails IWorkspace::GetProjectFromFile
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    
    /// @copydetails IWorkspace::GetWorkspaceFiles
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    
    /// @copydetails IWorkspace::GetWorkspaceProjects
    virtual wxArrayString GetWorkspaceProjects() const;
    
    /// @copydetails IWorkspace::IsBuildSupported
    virtual bool IsBuildSupported() const;
    
    /// @copydetails IWorkspace::IsProjectSupported
    virtual bool IsProjectSupported() const;

    const clDockerWorkspaceSettings & GetSettings() const { return m_settings; }
    
    clDockerWorkspaceSettings & GetSettings() { return m_settings; }
 
    void Open(const wxFileName& path);

    void Close();

    bool IsOpen() const;

    bool Create(const wxFileName& filename);

    void BuildDockerfile(const wxFileName& dockerfile);
    void BuildDockerCompose(const wxFileName& dockerfile);
    void RunDockerfile(const wxFileName& dockerfile);
    void RunDockerCompose(const wxFileName& dockerfile);
protected:
    // Event handlers
    void OnOpenWorkspace(gsgs::Event& event);
    void OnCloseWorkspace(gsgs::Event& event);
    void OnNewWorkspace(gsgs::Event& event);

    // Session management
    void OnSaveSession(gsgs::Event& event);

    // Build events
    void OnIsBuildInProgress(gsgs::BuildEvent& event);
    void OnBuildStarting(gsgs::BuildEvent& event);
    void OnStopBuild(gsgs::BuildEvent& event);

    // Run events
    void OnRun(gsgs::ExecuteEvent& event);
    void OnStop(gsgs::ExecuteEvent& event);

    //===-----
    //===-----
    void RestoreSession();
private:
    wxFileName m_filename;
    clDockerWorkspaceSettings m_settings;
    clDockerWorkspaceView* m_view = nullptr;
    clDockerDriver::Ptr_t m_driver;
    Docker* m_plugin = nullptr;
    bool m_isOpen = false;
    bool m_clangOldFlag = false;
    bool m_bindEvents = false;
};

#endif // CLDOCKERWORKSPACE_H
