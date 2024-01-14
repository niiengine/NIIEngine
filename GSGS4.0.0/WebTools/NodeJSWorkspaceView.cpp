#include "NodeJSDebuggerDlg.h"
#include "NodeJSPackageJSON.h"
#include "NodeJSWorkspaceConfiguration.h"
#include "NodeJSWorkspaceView.h"
#include "NoteJSWorkspace.h"
#include "gsgsImageManager.h"
#include "gsgsEvent.h"
#include "gsgsCommon.h"
#include "gsgsEditorConfig.h"
#include "gsgsEvent.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "gsgsPreInclude.h"
#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/wupdlock.h>
#include "gsgsNodeJS.h"
#include <wx/textdlg.h>
#include "gsgsConsole.h"

NodeJSWorkspaceView::NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName)
    : gsgs::TreeCtrlPanel(parent)
    , m_config("nodejs-explorer")
{
    SetConfig(&m_config);
    SetNewFileTemplate("new.js", wxStrlen("new"));
    SetViewName(viewName);
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_FOLDER, &NodeJSWorkspaceView::OnContextMenu, this);
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_FILE, &NodeJSWorkspaceView::OnContextMenuFile, this);
    gsgs_Event().Bind(wxEVT_FILE_SYSTEM_UPDATED, &NodeJSWorkspaceView::OnFileSystemUpdated, this);
    gsgs_Event().Bind(wxEVT_FINDINFILES_DLG_DISMISSED, &NodeJSWorkspaceView::OnFindInFilesDismissed, this);
    Bind(wxEVT_TERMINAL_EXIT, &NodeJSWorkspaceView::OnTerminalClosed, this);
}

NodeJSWorkspaceView::~NodeJSWorkspaceView()
{
    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_FOLDER, &NodeJSWorkspaceView::OnContextMenu, this);
    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_FILE, &NodeJSWorkspaceView::OnContextMenuFile, this);
    gsgs_Event().Unbind(wxEVT_FILE_SYSTEM_UPDATED, &NodeJSWorkspaceView::OnFileSystemUpdated, this);
    gsgs_Event().Unbind(wxEVT_FINDINFILES_DLG_DISMISSED, &NodeJSWorkspaceView::OnFindInFilesDismissed, this);
    Unbind(wxEVT_TERMINAL_EXIT, &NodeJSWorkspaceView::OnTerminalClosed, this);
}

void NodeJSWorkspaceView::OnContextMenu(ContextMenuEvent& event)
{
    event.Skip();
    if((event.GetEventObject() == this)) {
        wxMenu* menu = event.GetMenu();

        // Locate the "Close" menu entry
        int openShellPos = wxNOT_FOUND;
        wxMenuItem* closeItem = NULL;

        int pos = ::FindMenuItemPosition(menu, XRCID("tree_ctrl_close_folder"));
        if(pos != wxNOT_FOUND) { closeItem = menu->FindItem(XRCID("tree_ctrl_close_folder"), NULL); }
        openShellPos = ::FindMenuItemPosition(menu, XRCID("tree_ctrl_open_shell_folder"));
        if(openShellPos != wxNOT_FOUND) {
            ++openShellPos; // insert after
            ++openShellPos; // skip the separator
        }

        if((pos != wxNOT_FOUND) && closeItem) {
            wxMenuItem* showHiddenItem =
                menu->Insert(pos, XRCID("nodejs_show_hidden_files"), _("Show hidden files"), "", wxITEM_CHECK);
            NodeJSWorkspaceConfiguration conf(NodeJSWorkspace::Get()->GetFileName());
            showHiddenItem->Check(conf.Load().IsShowHiddenFiles());
            menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnShowHiddenFiles, this, XRCID("nodejs_show_hidden_files"));

            menu->InsertSeparator(pos);
            menu->Insert(pos, XRCID("nodejs_close_workspace"), _("Close Workspace"));
            menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnCloseWorkspace, this, XRCID("nodejs_close_workspace"));

            // Remove the 'close' menu item
            menu->Remove(closeItem);
        }

        // Add project related items
        wxArrayString folders, files;
        wxArrayTreeItemIds folderItems, fileItems;
        GetSelections(folders, folderItems, files, fileItems);

        if((openShellPos != wxNOT_FOUND) && (folderItems.size() == 1) && (fileItems.size() == 0)) {
            // only a folder is selected, check to see if this is a project folder
            // we do this by testing for the existence of the file package.json
            // under the folder path
            int npmInstallPosAfter = wxNOT_FOUND;
            wxFileName packageJSON(folders.Item(0), "package.json");
            if(packageJSON.FileExists()) {
                // A project folder
                menu->InsertSeparator(openShellPos);
                menu->Insert(openShellPos, XRCID("nodejs_project_settings"), _("Open package.json"));
                menu->Insert(openShellPos, XRCID("nodejs_project_debug"), _("Debug..."));
                menu->Insert(openShellPos, XRCID("nodejs_project_run"), _("Run..."));
                npmInstallPosAfter = XRCID("nodejs_project_run");
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnOpenPackageJsonFile, this,
                           XRCID("nodejs_project_settings"));
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnProjectDebug, this, XRCID("nodejs_project_debug"));
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnProjectRun, this, XRCID("nodejs_project_run"));
            } else {
                // Offer a "npm init" menu entry
                menu->InsertSeparator(openShellPos);
                wxMenuItem* menuItem = new wxMenuItem(NULL, XRCID("nodejs_npm_init"), _("npm init"));
                menu->Insert(openShellPos, menuItem);
                npmInstallPosAfter = XRCID("nodejs_npm_init");
                menuItem->SetBitmap(gsgs_Image().LoadBitmap("console"));
                menu->Enable(XRCID("nodejs_npm_init"), gsgs::NodeJS::Get().IsInitialised());
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnNpmInit, this, XRCID("nodejs_npm_init"));
            }

            int npmInstallPos = ::FindMenuItemPosition(menu, npmInstallPosAfter);
            if(npmInstallPos != wxNOT_FOUND) {
                npmInstallPos++; // install after it
                menu->Insert(npmInstallPos, XRCID("nodejs_npm_install"), _("npm install"));
                menu->Bind(wxEVT_MENU, &NodeJSWorkspaceView::OnNpmInstall, this, XRCID("nodejs_npm_install"));
            }
        }
    }
}

void NodeJSWorkspaceView::OnFolderDropped(gsgs::Event& event)
{
    // Add only non existent folders to the workspace
    const wxArrayString& folders = event.GetStrings();
    if(folders.IsEmpty()) return;

    if(!NodeJSWorkspace::Get()->IsOpen()) {
        wxFileName workspaceFile(folders.Item(0), "");
        if(!workspaceFile.GetDirCount()) {
            ::wxMessageBox(_("Can not create workspace in the root folder"), _("New Workspace"),
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
        workspaceFile.SetName(workspaceFile.GetDirs().Last());
        workspaceFile.SetExt("workspace");
        // Create will fail if a file with this name already exists
        NodeJSWorkspace::Get()->Create(workspaceFile);
        // Load the workspace, again, it will fail if this is not a valid
        // NodeJS workspace
        NodeJSWorkspace::Get()->Open(workspaceFile);
    }

    if(NodeJSWorkspace::Get()->IsOpen()) {
        wxArrayString& workspaceFolders = NodeJSWorkspace::Get()->GetFolders();
        for(size_t i = 0; i < folders.size(); ++i) {
            if(workspaceFolders.Index(folders.Item(i)) == wxNOT_FOUND) {
                // New folder, add it to the workspace
                workspaceFolders.Add(folders.Item(i));
                AddFolder(folders.Item(i));
            }
        }
        NodeJSWorkspace::Get()->Save();
    }
    ::GetManager()->GetWorkspaceView()->SelectPage(GetViewName());
}

void NodeJSWorkspaceView::RebuildTree()
{
    wxWindowUpdateLocker locker(this);
    wxArrayString paths;
    wxArrayTreeItemIds items;
    GetTopLevelFolders(paths, items);

    Clear();

    for(size_t i = 0; i < paths.size(); ++i) {
        AddFolder(paths.Item(i));
    }

    IEditor* editor = GetManager()->GetActiveEditor();
    if(editor) { ExpandToFile(editor->GetFileName()); }
}

void NodeJSWorkspaceView::ShowHiddenFiles(bool show)
{
    if(show) {
        m_options |= kShowHiddenFiles;
        m_options |= kShowHiddenFolders;
    } else {
        m_options &= ~kShowHiddenFiles;
        m_options &= ~kShowHiddenFolders;
    }
}

void NodeJSWorkspaceView::OnShowHiddenFiles(wxCommandEvent& event)
{
    const wxFileName& filename = NodeJSWorkspace::Get()->GetFileName();
    NodeJSWorkspaceConfiguration conf(filename);
    conf.Load().SetShowHiddenFiles(event.IsChecked()).Save();
    ShowHiddenFiles(event.IsChecked());
    RebuildTree();
}

void NodeJSWorkspaceView::OnCloseWorkspace(wxCommandEvent& event)
{
    // Simulate the menu event "Close Workspace"
    wxUnusedVar(event);
    wxCommandEvent eventCloseWorkspace(wxEVT_MENU, XRCID("close_workspace"));
    eventCloseWorkspace.SetEventObject(gsgs_Event().TopFrame());
    gsgs_Event().TopFrame()->GetEventHandler()->AddPendingEvent(eventCloseWorkspace);
}

void NodeJSWorkspaceView::OnContextMenuFile(ContextMenuEvent& event) { event.Skip(); }

void NodeJSWorkspaceView::OnOpenPackageJsonFile(wxCommandEvent& event)
{
    wxString path;
    wxTreeItemId item;
    if(!GetSelectProjectPath(path, item)) return;

    wxFileName packageJson(path, "package.json");
    GetManager()->OpenFile(packageJson.GetFullPath());
}

void NodeJSWorkspaceView::OnProjectDebug(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteProject(NodeJSDebuggerDlg::kDebug);
}

void NodeJSWorkspaceView::OnProjectRun(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteProject(NodeJSDebuggerDlg::kExecute);
}

bool NodeJSWorkspaceView::GetSelectProjectPath(wxString& path, wxTreeItemId& item)
{
    path.clear();
    wxArrayString folders, files;
    wxArrayTreeItemIds folderItems, fileItems;
    GetSelections(folders, folderItems, files, fileItems);
    if((folders.size() == 1) && (files.IsEmpty())) {
        path = folders.Item(0);
        item = folderItems.Item(0);
        return true;
    }
    return false;
}

void NodeJSWorkspaceView::DoExecuteProject(NodeJSDebuggerDlg::eDialogType type)
{
    wxString path;
    wxTreeItemId item;
    if(!GetSelectProjectPath(path, item)) return;

    NodeJSPackageJSON pj;
    if(!pj.Load(path)) {
        if(!pj.Create(path)) {
            ::wxMessageBox(_("Failed to load package.json file from path:\n") + path, "gsgs",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
    }

    // Sanity

    // No debugger?
    if(!NodeJSWorkspace::Get()->GetDebugger() &&
       ((type == NodeJSDebuggerDlg::kDebug) || (type == NodeJSDebuggerDlg::kDebugCLI)))
        return;

    NodeJSDebuggerDlg dlg(gsgs_Event().TopFrame(), type, pj.GetScript(), pj.GetArgs());
    if(dlg.ShowModal() != wxID_OK) { return; }

    // store the data for future use
    pj.SetScript(dlg.GetScript());
    pj.SetArgs(dlg.GetArgs());
    pj.Save(path);

    wxString command;
    wxString command_args;
    dlg.GetCommand(command, command_args);
    NodeJSWorkspace::Get()->GetDebugger()->StartDebugger(command, command_args, dlg.GetWorkingDirectory());
}

void NodeJSWorkspaceView::OnItemExpanding(wxTreeEvent& event)
{
    // Call the parent function to do the actual expansion
    gsgs::TreeCtrlPanel::OnItemExpanding(event);

    // Loop over the children of the item and replace the icon for all projects
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);

    gsgs::TreeCtrlData* cd = GetItemData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsFolder());

    int imageIndex = m_bmpLoader->GetMimeImageId(FileUtil::TypeProject);
    CHECK_COND_RET(imageIndex != wxNOT_FOUND);

    int imageIndexExpanded = m_bmpLoader->GetMimeImageId(FileUtil::TypeProjectExpanded);

    {
        // change the icon for the parent folder as well
        wxFileName packageJSON(cd->GetPath(), "package.json");
        if(packageJSON.FileExists()) { GetTreeCtrl()->SetItemImage(item, imageIndex, imageIndexExpanded); }
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetTreeCtrl()->GetFirstChild(item, cookie);
    while(child.IsOk()) {
        gsgs::TreeCtrlData* cd = GetItemData(child);
        if(cd && cd->IsFolder()) {
            wxFileName packageJSON(cd->GetPath(), "package.json");
            if(packageJSON.FileExists()) {
                // A project
                GetTreeCtrl()->SetItemImage(child, imageIndex);
            }
        }
        child = GetTreeCtrl()->GetNextChild(item, cookie);
    }
}

void NodeJSWorkspaceView::OnNpmInit(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString path;
    wxTreeItemId item;
    if(!GetSelectProjectPath(path, item)) return;
    gsgs::NodeJS::Get().NpmInit(path, this);
}

void NodeJSWorkspaceView::OnFileSystemUpdated(gsgs::FileEvent& event)
{
    event.Skip();
    RefreshTree();
}

void NodeJSWorkspaceView::OnNpmInstall(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item;
    wxString path;
    if(!GetSelectProjectPath(path, item)) return;

    wxString packageName = ::wxGetTextFromUser(_("Package name:"), "npm install");
    if(packageName.IsEmpty()) { return; }
    gsgs::NodeJS::Get().NpmInstall(packageName, path, "--save", this);
}

void NodeJSWorkspaceView::OnTerminalClosed(gsgs::ProcessEvent& event)
{
    // Refresh the tree after npm commands
    RefreshTree();
}

void NodeJSWorkspaceView::OnFindInFilesDismissed(gsgs::FindInFilesEvent& event)
{
    event.Skip();
    // Let the parent process this event first (it will handle any transient paths)
    if(NodeJSWorkspace::Get()->IsOpen()) {
        // store the find in files mask
        gsgs_Config().Write("FindInFiles/NodeJS/LookIn", event.GetPaths());
        gsgs_Config().Write("FindInFiles/NodeJS/Mask", event.GetFileMask());
    }
}

void NodeJSWorkspaceView::OnFindInFilesShowing(gsgs::FindInFilesEvent& event)
{
    event.Skip();
    gsgs::TreeCtrlPanel::OnFindInFilesShowing(event);
    if(NodeJSWorkspace::Get()->IsOpen()) {
        // store the find in files mask
        // Load the NodeJS workspace values from the configuration
        wxString mask =
            "*.js;*.html;*.css;*.scss;*.json;*.xml;*.ini;*.md;*.txt;*.text;.htaccess;*.sql";
        gsgs_Config().Read("FindInFiles/NodeJS/Mask", mask, mask);
        event.SetFileMask(mask);

        wxString lookIn;
        lookIn << "<Entire Workspace>\n"
               << "-*node_modules*";
        gsgs_Config().Read("FindInFiles/NodeJS/LookIn", lookIn, lookIn);
        event.SetPaths(lookIn);
    }
}
