//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NoteJSWorkspace.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef NOTEJSWORKSPACE_H
#define NOTEJSWORKSPACE_H

#include "IWorkspace.h"
#include "NodeDebugger.h"
#include "TerminalEmulator.h"
#include "gsgsEvent.h"

class NodeDebugger;
class NodeJSWorkspaceView;
class NodeJSWorkspace : public IWorkspace
{
public:
    NodeJSWorkspace(bool dummy);
    virtual ~NodeJSWorkspace();

    static NodeJSWorkspace* Get();
    static void Free();
        
    void SetFilename(const wxFileName& filename) { m_filename = filename; }
    /// @copydetails IWorkspace::GetFileName
    wxFileName GetFileName() const { return m_filename; }
    /// @copydetails IWorkspace::GetActiveProjectName
    virtual wxString GetActiveProjectName() const;
    /// @copydetails IWorkspace::GetProjectFileName
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    /// @copydetails IWorkspace::GetWorkspaceProjects
    virtual wxArrayString GetWorkspaceProjects() const;
    /// @copydetails IWorkspace::GetProjectFiles
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    /// @copydetails IWorkspace::GetWorkspaceFiles
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    /// @copydetails IWorkspace::GetProjectFromFile
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    /// @copydetails IWorkspace::GetFilesMask
    virtual wxString GetFilesMask() const;
    /// @copydetails IWorkspace::IsBuildSupported
    virtual bool IsBuildSupported() const;
    /// @copydetails IWorkspace::IsProjectSupported
    virtual bool IsProjectSupported() const;

    NodeJSWorkspaceView * GetView() { return m_view; }
    /**
     * @brief create a debugger instance for this workspace
     */
    void AllocateDebugger();
    
    NodeDebugger::Ptr_t GetDebugger();

    /**
     * @brief is this workspace opened?
     */
    bool IsOpen() const;

    /**
     * @brief create nodejs at the given path
     */
    bool Create(const wxFileName & filename);

    /**
     * @brief open nodejs workspace and load its view
     */
    bool Open(const wxFileName & filename);

    void Close();

    void Save();
    
    void SetFolders(const wxArrayString & folders) { m_folders = folders; }
    const wxArrayString & GetFolders() const { return m_folders; }
    wxArrayString & GetFolders() { return m_folders; }
protected:
    NodeJSWorkspace();
    
    int GetNodeJSMajorVersion() const;
    void DoAllocateDebugger();
    void DoClear();
    bool DoOpen(const wxFileName& filename);

    // Fallback, incase we fail to allocate a proper debugger
    void OnDebugStart(gsgs::DebugEvent& event);

    /**
     * @brief gsgs requests to close the workspace
     */
    void OnCloseWorkspace(gsgs::Event& e);
    /**
     * @brief create new workspace (originated from the menu bar)
     */
    void OnNewWorkspace(gsgs::Event& e);

    /**
     * @brief open a workspace
     */
    void OnOpenWorkspace(gsgs::Event& event);

    /**
     * @brief all editors have been closed
     */
    void OnAllEditorsClosed(wxCommandEvent& event);

    /**
     * @brief handle save session command from gsgs
     */
    void OnSaveSession(gsgs::Event& event);

    /**
     * @brief execute the current script
     */
    void OnExecute(gsgs::ExecuteEvent& event);

    void OnStopExecute(gsgs::ExecuteEvent& event);
    
    void OnIsExecuteInProgress(gsgs::ExecuteEvent& event);
    
    void OnProcessOutput(gsgs::Event& event);
    
    void OnProcessTerminated(gsgs::Event& event);

    /**
     * @brief restore the workspace session
     */
    void RestoreSession();
protected:
    wxFileName m_filename;
    wxArrayString m_folders;
    NodeJSWorkspaceView* m_view;
    bool m_clangOldFlag;
    bool m_dummy;
    bool m_showWelcomePage;
    NodeDebugger::Ptr_t m_debugger;
    TerminalEmulator m_terminal;
    static NodeJSWorkspace* ms_workspace;
};

#endif // NOTEJSWORKSPACE_H
