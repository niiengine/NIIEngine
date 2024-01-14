#include "DockerfileSettingsDlg.h"
#include "clDockerSettings.h"
#include "clDockerWorkspace.h"
#include "clDockerWorkspaceSettings.h"
#include "clDockerWorkspaceView.h"
#include "gsgsEvent.h"
#include "gsgsEvent.h"
#include <wx/menu.h>
#include <gsgsPreInclude.h>

clDockerWorkspaceView::clDockerWorkspaceView(wxWindow* parent)
    : gsgs::TreeCtrlPanel(parent)
    , m_config("docker-view.conf")
{
    // Set the view options
    SetConfig(&m_config);
    clDockerSettings settings;
    size_t options = 0;
    settings.Load();
    if(settings.IsLinkEditor()) { options |= kLinkToEditor; }
    SetOptions(options);

    SetViewName("Docker");
    SetNewFileTemplate("new", wxStrlen("new"));

    gsgs_Event().Bind(wxEVT_WORKSPACE_LOADED, &clDockerWorkspaceView::OnWorkspaceOpened, this);
    gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &clDockerWorkspaceView::OnWorkspaceClosed, this);
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_FILE, &clDockerWorkspaceView::OnFileContextMenu, this);
    gsgs_Event().Bind(wxEVT_FINDINFILES_DLG_DISMISSED, &clDockerWorkspaceView::OnFindInFilesDismissed, this);
}

clDockerWorkspaceView::~clDockerWorkspaceView()
{
    gsgs_Event().Unbind(wxEVT_WORKSPACE_LOADED, &clDockerWorkspaceView::OnWorkspaceOpened, this);
    gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &clDockerWorkspaceView::OnWorkspaceClosed, this);
    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_FILE, &clDockerWorkspaceView::OnFileContextMenu, this);
    gsgs_Event().Unbind(wxEVT_FINDINFILES_DLG_DISMISSED, &clDockerWorkspaceView::OnFindInFilesDismissed, this);
}

void clDockerWorkspaceView::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();

    // Store the view settings
    clDockerSettings settings;
    settings.Load();
    settings.SetLinkEditor(m_options & kLinkToEditor);
    settings.Save();
}

void clDockerWorkspaceView::OnWorkspaceOpened(wxCommandEvent& event)
{
    event.Skip();
    if(clDockerWorkspace::Get()->IsOpen()) {
        wxFileName workspaceFile(event.GetString());
        AddFolder(workspaceFile.GetPath());
    }
}

void clDockerWorkspaceView::OnFileContextMenu(ContextMenuEvent& event)
{
    event.Skip();
    if(event.GetEventObject() != this) { return; }
    const wxArrayString& contextMenuFiles = event.GetStrings();
    if(contextMenuFiles.size() != 1) { return; }

    wxFileName fn(contextMenuFiles.Item(0));
    if(fn.GetFullName() == "Dockerfile") {
        DoDockerfileContextMenu(event.GetMenu(), contextMenuFiles.Item(0));
    } else if(fn.GetFullName() == "docker-compose.yml") {
        DoDockerComposeContextMenu(event.GetMenu(), contextMenuFiles.Item(0));
    }
}

void clDockerWorkspaceView::DoDockerfileContextMenu(wxMenu* menu, const wxString& dockerfile)
{
    menu->PrependSeparator();
    menu->Prepend(XRCID("run_dockerfile"), _("Run..."));
    menu->Prepend(XRCID("build_dockerfile"), _("Build..."));
    menu->AppendSeparator();
    menu->Append(XRCID("ID_DOCKERFILE_SETTINGS"), _("Settings..."));
    menu->Bind(wxEVT_MENU,
               [=](wxCommandEvent& evt) {
                   // Context menu requested for a "Dockerfile"
                   clDockerBuildableFile::Ptr_t fileInfo;
                   clDockerWorkspaceSettings& settings = clDockerWorkspace::Get()->GetSettings();

                   wxArrayString folders, files;
                   GetSelections(folders, files);
                   if(files.size() != 1) { return; }

                   fileInfo = settings.GetFileInfo(files.Item(0));
                   if(!fileInfo) { fileInfo = clDockerBuildableFile::New(eDockerFileType::kDockerfile); }
                   if(fileInfo->GetType() != eDockerFileType::kDockerfile) return;

                   fileInfo->SetPath(files.Item(0));
                   DockerfileSettingsDlg dlg(gsgs_Event().TopFrame(), fileInfo);
                   if(dlg.ShowModal() != wxID_OK) { return; }
                   settings.SetFileInfo(files.Item(0), fileInfo);
                   settings.Save(clDockerWorkspace::Get()->GetFileName());
               },
               XRCID("ID_DOCKERFILE_SETTINGS"));
    menu->Bind(wxEVT_MENU, [=](wxCommandEvent& evt) { clDockerWorkspace::Get()->BuildDockerfile(dockerfile); },
               XRCID("build_dockerfile"));
    menu->Bind(wxEVT_MENU, [=](wxCommandEvent& evt) { clDockerWorkspace::Get()->RunDockerfile(dockerfile); },
               XRCID("run_dockerfile"));
}

void clDockerWorkspaceView::DoDockerComposeContextMenu(wxMenu* menu, const wxString& docker_compose)
{
    menu->PrependSeparator();
    menu->Prepend(XRCID("run_dockerfile"), _("Run..."));
    menu->Prepend(XRCID("build_dockerfile"), _("Build..."));
    menu->AppendSeparator();
    menu->Append(XRCID("ID_DOCKERFILE_SETTINGS"), _("Settings..."));
    menu->Bind(wxEVT_MENU,
               [=](wxCommandEvent& evt) {
                   // Context menu requested for a "Dockerfile"
                   clDockerBuildableFile::Ptr_t fileInfo;
                   clDockerWorkspaceSettings& settings = clDockerWorkspace::Get()->GetSettings();

                   wxArrayString folders, files;
                   GetSelections(folders, files);
                   if(files.size() != 1) { return; }

                   fileInfo = settings.GetFileInfo(files.Item(0));
                   if(!fileInfo) { fileInfo = clDockerBuildableFile::New(eDockerFileType::kDockerCompose); }
                   if(fileInfo->GetType() != eDockerFileType::kDockerCompose) return;

                   fileInfo->SetPath(files.Item(0));
                   DockerfileSettingsDlg dlg(gsgs_Event().TopFrame(), fileInfo);
                   if(dlg.ShowModal() != wxID_OK) { return; }
                   settings.SetFileInfo(files.Item(0), fileInfo);
                   settings.Save(clDockerWorkspace::Get()->GetFileName());
               },
               XRCID("ID_DOCKERFILE_SETTINGS"));
    menu->Bind(wxEVT_MENU, [=](wxCommandEvent& evt) { clDockerWorkspace::Get()->BuildDockerCompose(docker_compose); },
               XRCID("build_dockerfile"));
    menu->Bind(wxEVT_MENU, [=](wxCommandEvent& evt) { clDockerWorkspace::Get()->RunDockerCompose(docker_compose); },
               XRCID("run_dockerfile"));
}

void clDockerWorkspaceView::OnFindInFilesDismissed(gsgs::FindInFilesEvent& event)
{
    event.Skip();
    if(clDockerWorkspace::Get()->IsOpen()) {
        gsgs_Config().Write("FindInFiles/Docker/Mask", event.GetFileMask());
        gsgs_Config().Write("FindInFiles/Docker/LookIn", event.GetPaths());
    }
}

void clDockerWorkspaceView::OnFindInFilesShowing(gsgs::FindInFilesEvent& event)
{
    event.Skip();
    gsgs::TreeCtrlPanel::OnFindInFilesShowing(event);
    if(clDockerWorkspace::Get()->IsOpen()) {
        // store the find in files mask
        // Load the Docker workspace values from the configuration
        wxString mask = "Dockerfile;docker-compose.yml;*.txt";
        gsgs_Config().Read("FindInFiles/Docker/Mask", mask, mask);
        event.SetFileMask(mask);

        wxString lookIn;
        lookIn << SEARCH_IN_WORKSPACE_FOLDER;
        gsgs_Config().Read("FindInFiles/Docker/LookIn", lookIn, lookIn);
        event.SetPaths(lookIn);
    }
}
