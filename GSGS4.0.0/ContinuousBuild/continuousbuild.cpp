//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : continuousbuild.cpp
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

#include "gsgsToolsetManager.h"
#include "continousbuildconf.h"
#include "continousbuildpane.h"
#include "continuousbuild.h"
#include "gsgsEditorConfig.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "gsgsWorkspace.h"
#include "gsgsBuilderGnuMake.h"
#include "process_impl.h"
#include "globals.h"
#include <wx/app.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <wx/xrc/xmlres.h>

static ContinuousBuild* thePlugin = NULL;
// Define the plugin entry point
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new ContinuousBuild(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("ContinuousBuild"));
    info.SetDescription(_("Continuous build plugin which compiles files on save and report errors"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

static const wxString CONT_BUILD = _("BuildQ");

ContinuousBuild::ContinuousBuild(IManager* manager)
    : Plugin(manager)
    , m_buildInProgress(false)
{
    m_longName = _("Continuous build plugin which compiles files on save and report errors");
    m_shortName = wxT("ContinuousBuild");
    m_view = new ContinousBuildPane(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

    // add our page to the output pane notebook
    m_mgr->GetOutputPaneNotebook()->AddPage(m_view, CONT_BUILD, false, m_mgr->GetStdIcons()->LoadBitmap("execute"));
    m_tabHelper.reset(new gsgs::TabTogglerHelper(CONT_BUILD, m_view, "", NULL));
    m_tabHelper->SetOutputTabBmp(m_mgr->GetStdIcons()->LoadBitmap("execute"));

    m_topWin = m_mgr->GetTheApp();
    gsgs_Event().Connect(wxEVT_FILE_SAVED, gsgsEventHandler(ContinuousBuild::OnFileSaved), NULL, this);
    gsgs_Event().Connect(wxEVT_FILE_SAVE_BY_BUILD_START,
                                  wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved), NULL, this);
    gsgs_Event().Connect(wxEVT_FILE_SAVE_BY_BUILD_END,
                                  wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ContinuousBuild::OnBuildProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ContinuousBuild::OnBuildProcessEnded, this);
}

ContinuousBuild::~ContinuousBuild() {}

void ContinuousBuild::CreateToolBar(gsgs::ToolBar* toolbar)
{
    // Create the toolbar to be used by the plugin
    wxUnusedVar(toolbar);
}

void ContinuousBuild::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }

void ContinuousBuild::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void ContinuousBuild::UnPlug()
{
    m_tabHelper.reset(NULL);
    // before this plugin is un-plugged we must remove the tab we added
    for(size_t i = 0; i < m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if(m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            break;
        }
    }
    m_view->Destroy();

    gsgs_Event().Disconnect(wxEVT_FILE_SAVED, gsgsEventHandler(ContinuousBuild::OnFileSaved), NULL, this);
    gsgs_Event().Disconnect(wxEVT_FILE_SAVE_BY_BUILD_START,
                                     wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved), NULL, this);
    gsgs_Event().Disconnect(wxEVT_FILE_SAVE_BY_BUILD_END,
                                     wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
}

void ContinuousBuild::OnFileSaved(gsgs::Event& e)
{
    e.Skip();
    gsgsDEBUG1() << "ContinuousBuild::OnFileSaved";
    // Dont build while the main build is in progress
    if(m_buildInProgress) {
        gsgsDEBUG() << "Build already in progress, skipping";
        return;
    }

    ContinousBuildConf conf;
    m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);

    if(conf.GetEnabled()) {
        DoBuild(e.GetString());
    } else {
        gsgsDEBUG1() << "ContinuousBuild is disabled";
    }
}

void ContinuousBuild::DoBuild(const wxString& fileName)
{
    gsgsDEBUG() << "ContinuousBuild::DoBuild is called";
    // Make sure a workspace is opened
    if(!m_mgr->IsOpen()) {
        gsgsDEBUG() << "ContinuousBuild::DoBuild: No workspace opened!";
        return;
    }

    // Filter non source files
    FileUtil::FileType type = FileUtil::GetType(fileName);
    switch(type) {
    case FileUtil::TypeSourceC:
    case FileUtil::TypeSourceCpp:
    case FileUtil::TypeResource:
        break;

    default: {
        gsgsDEBUG1() << "ContinuousBuild::DoBuild: Non source file";
        return;
    }
    }

    wxString projectName = m_mgr->GetProjectNameByFile(fileName);
    if(projectName.IsEmpty()) {
        gsgsDEBUG() << "ContinuousBuild::DoBuild: project name is empty";
        return;
    }

    wxString errMsg;
    ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
    if(!project) {
        gsgsDEBUG() << "Could not find project for file";
        return;
    }

    // get the selected configuration to be build
    BuildConfigPtr bldConf = m_mgr->GetWorkspace()->GetProjBuildConf(project->GetName(), wxEmptyString);
    if(!bldConf) {
        gsgs_Debug(wxT("Failed to locate build configuration\n"));
        return;
    }

    BuilderPtr builder = bldConf->GetBuilder();
    if(!builder) {
        gsgs_Debug(wxT("Failed to located builder\n"));
        return;
    }

    // Only normal file builds are supported
    if(bldConf->IsCustomBuild()) {
        gsgs_Debug(wxT("Build is custom. Skipping\n"));
        return;
    }

    // get the single file command to use
    wxString cmd =
        builder->GetSingleFileCmd(projectName, bldConf->GetName(), bldConf->GetBuildSystemArguments(), fileName);
    WrapInShell(cmd);

    if(m_buildProcess.IsBusy()) {
        // add the build to the queue
        if(m_files.Index(fileName) == wxNOT_FOUND) {
            m_files.Add(fileName);

            // update the UI
            m_view->AddFile(fileName);
        }
        return;
    }

    gsgs::Event event(wxEVT_SHELL_COMMAND_STARTED);

    // Associate the build event details
    BuildEventDetails* eventData = new BuildEventDetails();
    eventData->SetProjectName(projectName);
    eventData->SetConfiguration(bldConf->GetName());
    eventData->SetIsCustomProject(bldConf->IsCustomBuild());
    eventData->SetIsClean(false);

    event.SetClientObject(eventData);
    // Fire it up
    gsgs_Event().AddPendingEvent(event);

    EnvSetter env(NULL, NULL, projectName, bldConf->GetName());
    gsgs_Debug(wxString::Format(wxT("cmd:%s\n"), cmd.c_str()));
    if(!m_buildProcess.Execute(cmd, fileName, project->GetFileName().GetPath(), this)) return;

    // Set some messages
    m_mgr->SetStatusMessage(
        wxString::Format(wxT("%s %s..."), _("Compiling"), wxFileName(fileName).GetFullName().c_str()), 0);

    // Add this file to the UI queue
    m_view->AddFile(fileName);
}

void ContinuousBuild::OnBuildProcessEnded(gsgs::ProcessEvent& e)
{
    // remove the file from the UI
    int pid = m_buildProcess.GetPid();
    m_view->RemoveFile(m_buildProcess.GetFileName());

    gsgs::Event event(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
    gsgs_Event().AddPendingEvent(event);

    int exitCode(-1);
    if(IProcess::GetProcessExitCode(pid, exitCode) && exitCode != 0) {
        m_view->AddFailedFile(m_buildProcess.GetFileName());
    }

    // Release the resources allocted for this build
    m_buildProcess.Stop();

    // if the queue is not empty, start another build
    if(m_files.IsEmpty() == false) {

        wxString fileName = m_files.Item(0);
        m_files.RemoveAt(0);

        DoBuild(fileName);
    }
}

void ContinuousBuild::StopAll()
{
    // empty the queue
    m_files.Clear();
    m_buildProcess.Stop();
}

void ContinuousBuild::OnIgnoreFileSaved(wxCommandEvent& e)
{
    e.Skip();

    m_buildInProgress = true;

    // Clear the queue
    m_files.Clear();

    // Clear the view
    m_view->ClearAll();
}

void ContinuousBuild::OnStopIgnoreFileSaved(wxCommandEvent& e)
{
    e.Skip();
    m_buildInProgress = false;
}

void ContinuousBuild::OnBuildProcessOutput(gsgs::ProcessEvent& e)
{
    gsgs::Event event(wxEVT_SHELL_COMMAND_ADDLINE);
    event.SetString(e.GetOutput());
    gsgs_Event().AddPendingEvent(event);
}
