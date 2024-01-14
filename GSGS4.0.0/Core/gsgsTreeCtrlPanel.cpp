
#include "gsgsPluginCommon.h"
#include "gsgsFileUtil.h"
#include "gsgsCommonHandler.h"
#include "gsgsConfig.h"
#include "gsgsLogManager.h"
#include "gsgsTreeCtrlPanel.h"
#include "gsgsToolBar.h"
#include "globals.h"
#include "ieditor.h"
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/richmsgdlg.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include <wx/settings.h>
#include <wx/dcbuffer.h>

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // TreeCtrlPanelDefaultPage
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    class TreeCtrlPanelDefaultPage : public wxPanel
    {
    public:
        TreeCtrlPanelDefaultPage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL);
        virtual ~TreeCtrlPanelDefaultPage();

        wxStaticText* GetStaticText177() { return m_staticText177; }

        wxPanel* GetPanel169() { return m_panel169; }
    protected:
        virtual void OnDefaultPageContextMenu(wxContextMenuEvent& event);
        void OnOpenFolder(wxCommandEvent& event);
        void OnPaint(wxPaintEvent& event);
        void OnColoursChanged(Event& event);
    private:
        wxPanel* m_panel169;
        wxStaticText* m_staticText177;
        Colour m_colours;
    };
    //-----------------------------------------------------------------------------------
    TreeCtrlPanelDefaultPage::TreeCtrlPanelDefaultPage(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style, wxT("TreeCtrlPanelDefaultPage"))
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        wxBoxSizer* boxSizer167 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer167);

        m_panel169 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxTAB_TRAVERSAL | wxBORDER_NONE);
        m_panel169->SetBackgroundColour(wxColour(wxT("rgb(220,220,220)")));

        boxSizer167->Add(m_panel169, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

        wxBoxSizer* boxSizer171 = new wxBoxSizer(wxVERTICAL);
        m_panel169->SetSizer(boxSizer171);

        boxSizer171->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

        m_staticText177 = gsgs_Style().createStaticText(m_panel169, wxID_ANY, _("DRAG AND DROP\nA FOLDER HERE"), wxDefaultPosition,
            wxDLG_UNIT(m_panel169, wxSize(-1, -1)), wxALIGN_CENTRE);
        m_staticText177->SetForegroundColour(wxColour(wxT("rgb(128,128,128)")));
        wxFont m_staticText177Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_staticText177Font.SetWeight(wxFONTWEIGHT_BOLD);
        m_staticText177->SetFont(m_staticText177Font);

        boxSizer171->Add(m_staticText177, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        boxSizer171->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

        // Connect events
        m_panel169->Connect(wxEVT_CONTEXT_MENU,
            wxContextMenuEventHandler(TreeCtrlPanelDefaultPage::OnDefaultPageContextMenu), NULL,
            this);

        m_colours.setBase(gsgs_Style().GetAppBgColour());
        
        SetDropTarget(new FileOrFolderDropTarget(parent));
        m_staticText177->SetBackgroundColour(m_colours.GetBgColour());
        m_staticText177->SetForegroundColour(m_colours.GetItemTextColour());
        GetPanel169()->SetBackgroundColour(m_colours.GetBgColour());
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &TreeCtrlPanelDefaultPage::OnColoursChanged, this);
        Bind(wxEVT_PAINT, &TreeCtrlPanelDefaultPage::OnPaint, this);
    }
    //-----------------------------------------------------------------------------------
    TreeCtrlPanelDefaultPage::~TreeCtrlPanelDefaultPage()
    {
        m_panel169->Disconnect(wxEVT_CONTEXT_MENU,
            wxContextMenuEventHandler(TreeCtrlPanelDefaultPage::OnDefaultPageContextMenu), NULL, this);
        Unbind(wxEVT_PAINT, &TreeCtrlPanelDefaultPage::OnPaint, this);
        gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &TreeCtrlPanelDefaultPage::OnColoursChanged, this);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanelDefaultPage::OnDefaultPageContextMenu(wxContextMenuEvent& event)
    {
        wxMenu menu;
        menu.Append(XRCID("open_folder_default_page"), _("Open Folder..."));
        menu.Bind(wxEVT_MENU, &TreeCtrlPanelDefaultPage::OnOpenFolder, this, XRCID("open_folder_default_page"));
        PopupMenu(&menu);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanelDefaultPage::OnOpenFolder(wxCommandEvent& event)
    {
        wxCommandEvent eventOpenFolder(wxEVT_MENU, XRCID("open_folder"));
        wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(eventOpenFolder);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanelDefaultPage::OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBrush(m_colours.GetBgColour());
        dc.SetPen(m_colours.GetBgColour());
        dc.DrawRectangle(GetClientRect());
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanelDefaultPage::OnColoursChanged(Event& event)
    {
        event.Skip();
        m_colours.setBase(gsgs_Style().GetAppBgColour());
        m_staticText177->SetForegroundColour(m_colours.GetBgColour());
        GetPanel169()->SetBackgroundColour(m_colours.GetBgColour());
        m_staticText177->SetForegroundColour(m_colours.GetItemTextColour());
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // TreeCtrlPanel
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    TreeCtrlPanel::TreeCtrlPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        long style)
        : wxPanel(parent, id, pos, size, style, wxT("TreeCtrlPanel"))
        , m_config(NULL)
        , m_newfileTemplate("new.txt")
        , m_newfileTemplateHighlightLen(wxStrlen("new"))
        , m_options(kShowHiddenFiles | kShowHiddenFolders | kLinkToEditor)
    {
        wxBoxSizer* boxSizer151 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer151);

        m_treeCtrl = new gsgs::FileViewerTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxTR_DEFAULT_STYLE | wxTR_MULTIPLE | wxTR_HIDE_ROOT |
            wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES);

        boxSizer151->Add(m_treeCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

        // Connect events
        ::MSWSetNativeTheme(GetTreeCtrl());
        m_bmpLoader = &gsgs_Image();
        GetTreeCtrl()->SetFont(gsgs_Style().GetDefaultGuiFont());

        m_toolbar = new ToolBar(this);
        GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);
        m_toolbar->AddTool(XRCID("link_editor"), _("Link Editor"), m_bmpLoader->LoadBitmap("link_editor"), "",
                           wxITEM_CHECK);
        m_toolbar->Realize();
        m_toolbar->Bind(wxEVT_TOOL, &TreeCtrlPanel::OnLinkEditor, this, XRCID("link_editor"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &TreeCtrlPanel::OnLinkEditorUI, this, XRCID("link_editor"));

        // Allow DnD
        SetDropTarget(new FileOrFolderDropTarget(this));
        GetTreeCtrl()->SetDropTarget(new FileOrFolderDropTarget(this));
        GetTreeCtrl()->SetBitmaps(&m_bmpLoader->GetBitmaps());
        GetTreeCtrl()->AddRoot(_("Folders"), wxNOT_FOUND, wxNOT_FOUND, new TreeCtrlData(TreeCtrlData::kRoot));
        GetTreeCtrl()->Hide();

        m_defaultView = new TreeCtrlPanelDefaultPage(this);
        GetSizer()->Add(m_defaultView, 1, wxEXPAND);

        m_treeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(TreeCtrlPanel::OnItemExpanding),
            NULL, this);
        m_treeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(TreeCtrlPanel::OnItemActivated),
            NULL, this);
        m_treeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(TreeCtrlPanel::OnContextMenu), NULL,
            this);
        Bind(wxEVT_DND_FOLDER_DROPPED, &TreeCtrlPanel::OnFolderDropped, this);
        gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &TreeCtrlPanel::OnActiveEditorChanged, this);
        gsgs_Event().Bind(wxEVT_INIT_DONE, &TreeCtrlPanel::OnInitDone, this);
        gsgs_Event().Bind(wxEVT_FINDINFILES_DLG_SHOWING, &TreeCtrlPanel::OnFindInFilesShowing, this);
    }
    //-----------------------------------------------------------------------------------
    TreeCtrlPanel::~TreeCtrlPanel()
    {
        m_treeCtrl->Disconnect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(TreeCtrlPanel::OnItemExpanding),
            NULL, this);
        m_treeCtrl->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(TreeCtrlPanel::OnItemActivated),
            NULL, this);
        m_treeCtrl->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(TreeCtrlPanel::OnContextMenu), NULL,
            this);
        Unbind(wxEVT_DND_FOLDER_DROPPED, &TreeCtrlPanel::OnFolderDropped, this);
        m_toolbar->Unbind(wxEVT_TOOL, &TreeCtrlPanel::OnLinkEditor, this, XRCID("link_editor"));
        m_toolbar->Unbind(wxEVT_UPDATE_UI, &TreeCtrlPanel::OnLinkEditorUI, this, XRCID("link_editor"));
        gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &TreeCtrlPanel::OnActiveEditorChanged, this);
        gsgs_Event().Unbind(wxEVT_INIT_DONE, &TreeCtrlPanel::OnInitDone, this);
        gsgs_Event().Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &TreeCtrlPanel::OnFindInFilesShowing, this);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnContextMenu(wxTreeEvent& event)
    {
        wxTreeItemId item = event.GetItem();
        CHECK_ITEM_RET(item);

        TreeCtrlData* cd = GetItemData(item);
        if(cd && cd->IsFolder()) {
            // Prepare a folder context menu
            wxMenu menu;
            menu.Append(wxID_REFRESH, _("Refresh"));
            menu.AppendSeparator();

            menu.Append(XRCID("tree_ctrl_new_folder"), _("New Folder"));
            menu.Append(XRCID("tree_ctrl_new_file"), _("New File"));
            if(!IsTopLevelFolder(item)) {
                menu.AppendSeparator();
                menu.Append(XRCID("tree_ctrl_delete_folder"), _("Delete"));
            }

            if(!IsTopLevelFolder(item)) {
                menu.AppendSeparator();
                menu.Append(XRCID("tree_ctrl_rename_folder"), _("Rename..."));
            }

            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_find_in_files_folder"), _("Find in Files"));
            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_open_containig_folder"), _("Open Containing Folder"));
            menu.Append(XRCID("tree_ctrl_open_shell_folder"), _("Open Shell"));

            if(IsTopLevelFolder(item)) {
                menu.AppendSeparator();
                menu.Append(XRCID("tree_ctrl_close_folder"), _("Close"));
            }

            // Now that we added the basic menu, let the plugin
            // adjust it
            wxArrayString files, folders;
            GetSelections(folders, files);

            ContextMenuEvent dirMenuEvent(wxEVT_CONTEXT_MENU_FOLDER);
            dirMenuEvent.SetEventObject(this);
            dirMenuEvent.SetMenu(&menu);
            dirMenuEvent.SetPath(cd->GetPath());
            gsgs_Event().ProcessEvent(dirMenuEvent);

            // Connect events
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnCloseFolder, this, XRCID("tree_ctrl_close_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnNewFolder, this, XRCID("tree_ctrl_new_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnNewFile, this, XRCID("tree_ctrl_new_file"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnDeleteSelections, this, XRCID("tree_ctrl_delete_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnRenameFolder, this, XRCID("tree_ctrl_rename_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnFindInFilesFolder, this, XRCID("tree_ctrl_find_in_files_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnOpenContainingFolder, this, XRCID("tree_ctrl_open_containig_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnOpenShellFolder, this, XRCID("tree_ctrl_open_shell_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnRefresh, this, wxID_REFRESH);
            PopupMenu(&menu);

        } else if(cd && cd->IsFile()) {
            // File context menu
            // Prepare a folder context menu
            wxMenu menu;

            menu.Append(XRCID("tree_ctrl_open_file"), _("Open"));
            menu.Append(XRCID("tree_ctrl_rename_file"), _("Rename"));
            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_open_with_default_app"), _("Open with default application"));
            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_delete_file"), _("Delete"));

            menu.AppendSeparator();
            menu.Append(XRCID("tree_ctrl_open_containig_folder"), _("Open Containing Folder"));
            menu.Append(XRCID("tree_ctrl_open_shell_folder"), _("Open Shell"));

            // Now that we added the basic menu, let the plugin
            // adjust it
            wxArrayString files, folders;
            GetSelections(folders, files);

            ContextMenuEvent fileMenuEvent(wxEVT_CONTEXT_MENU_FILE);
            fileMenuEvent.SetEventObject(this);
            fileMenuEvent.SetMenu(&menu);
            fileMenuEvent.SetStrings(files);
            gsgs_Event().ProcessEvent(fileMenuEvent);

            // Connect events
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnOpenFile, this, XRCID("tree_ctrl_open_file"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnRenameFile, this, XRCID("tree_ctrl_rename_file"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnDeleteSelections, this, XRCID("tree_ctrl_delete_file"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnOpenWithDefaultApplication, this,
                      XRCID("tree_ctrl_open_with_default_app"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnOpenContainingFolder, this, XRCID("tree_ctrl_open_containig_folder"));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnOpenShellFolder, this, XRCID("tree_ctrl_open_shell_folder"));

            PopupMenu(&menu);
        } else {
            // context menu elsewhere
            wxMenu menu;
            menu.Append(XRCID("open_folder_default_page"), _("Open Folder..."));
            menu.Bind(wxEVT_MENU, &TreeCtrlPanel::OnOpenFolder, this, XRCID("open_folder_default_page"));
            PopupMenu(&menu);
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnItemActivated(wxTreeEvent& event)
    {
        event.Skip();
        wxCommandEvent dummy;
        OnOpenFile(dummy);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnItemExpanding(wxTreeEvent& event)
    {
        event.Skip();
        wxTreeItemId item = event.GetItem();
        CHECK_ITEM_RET(item);
        DoExpandItem(item, true);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnFolderDropped(Event& event)
    {
        const wxArrayString& folders = event.GetStrings();
        for(size_t i = 0; i < folders.size(); ++i) {
            AddFolder(folders.Item(i));
        }
        ::GetManager()->GetWorkspaceView()->SelectPage(GetViewName());
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::DoExpandItem(const wxTreeItemId& parent, bool expand)
    {
        TreeCtrlData* cd = GetItemData(parent);
        CHECK_PTR_RET(cd);

        // we only know how to expand folders...
        if(!cd->IsFolder()) return;
        wxString folderPath = cd->GetPath();

        if(!m_treeCtrl->ItemHasChildren(parent)) return;
        // Test the first item for dummy

        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, cookie);
        CHECK_ITEM_RET(child);

        cd = GetItemData(child);
        CHECK_PTR_RET(cd);

        // If not dummy - already expanded do nothing here
        if(!cd->IsDummy()) return;

        m_treeCtrl->Delete(child);
        cd = NULL;

        // Get the top level folders
        wxDir dir(folderPath);
        if(!dir.IsOpened()) return;
        wxBusyCursor bc;
        wxString filename;
        bool cont = dir.GetFirst(&filename, wxEmptyString);
        while(cont) {
            wxFileName fullpath(folderPath, filename);
            // Make sure we don't add the same folder twice
            if(wxFileName::DirExists(fullpath.GetFullPath())) {
                // a folder
                if(!(m_options & kShowHiddenFolders) && FileUtil::IsHidden(fullpath)) {
                    cont = dir.GetNext(&filename);
                    continue;
                }
                DoAddFolder(parent, fullpath.GetFullPath());
            } else {
                // make sure we don't add the same file twice
                if(!(m_options & kShowHiddenFiles) && FileUtil::IsHidden(fullpath)) {
                    cont = dir.GetNext(&filename);
                    continue;
                }
                DoAddFile(parent, fullpath.GetFullPath());
            }
            cont = dir.GetNext(&filename);
        }

        // Sort the parent
        if(GetTreeCtrl()->ItemHasChildren(parent)) {
            if(expand) { GetTreeCtrl()->Expand(parent); }
            SelectItem(parent);
        }
    }
    //-----------------------------------------------------------------------------------
    TreeCtrlData* TreeCtrlPanel::GetItemData(const wxTreeItemId& item) const
    {
        CHECK_ITEM_RET_NULL(item);
        TreeCtrlData* cd = dynamic_cast<TreeCtrlData*>(m_treeCtrl->GetItemData(item));
        return cd;
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::AddFolder(const wxString& path)
    {
        wxTreeItemId itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), path);
        DoExpandItem(itemFolder, false);
        ToggleView();
    }
    //-----------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlPanel::DoAddFile(const wxTreeItemId& parent, const wxString& path)
    {
        wxFileName filename(path);
        TreeCtrlData* parentData = GetItemData(parent);
        if(!parentData) { return wxTreeItemId(); }
        if(parentData->GetIndex()) {
            wxTreeItemId cachedItem = parentData->GetIndex()->Find(filename.GetFullName());
            if(cachedItem.IsOk()) {
                // the item already exists, return it
                return cachedItem;
            }
        }

        TreeCtrlData* cd = new TreeCtrlData(TreeCtrlData::kFile);
        cd->SetPath(filename.GetFullPath());

        int imgIdx = m_bmpLoader->GetMimeImageId(filename.GetFullName());
        if(imgIdx == wxNOT_FOUND) { imgIdx = m_bmpLoader->GetMimeImageId(FileUtil::TypeText); }
        wxTreeItemId fileItem = GetTreeCtrl()->AppendItem(parent, filename.GetFullName(), imgIdx, imgIdx, cd);
        // Add this entry to the index
        if(parentData->GetIndex()) { parentData->GetIndex()->Add(filename.GetFullName(), fileItem); }
        return fileItem;
    }
    //-----------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlPanel::DoAddFolder(const wxTreeItemId& parent, const wxString& path)
    {
        // If we already have this folder opened, dont re-add it
        wxArrayString topFolders;
        wxArrayTreeItemIds topFoldersItems;
        GetTopLevelFolders(topFolders, topFoldersItems);
        int where = topFolders.Index(path);
        if(where != wxNOT_FOUND) { return topFoldersItems.Item(where); }

        wxFileName filename(path, "");
        TreeCtrlData* cd = new TreeCtrlData(TreeCtrlData::kFolder);
        cd->SetPath(path);
        wxString displayName = (m_options & kShowRootFullPath) ? cd->GetPath() : cd->GetName();

        TreeCtrlData* parentData = GetItemData(parent);
        if(!parentData) {
            wxDELETE(cd);
            return wxTreeItemId();
        }

        // Check the index before adding new folder
        if(parentData->GetIndex()) {
            wxTreeItemId cachedItem = parentData->GetIndex()->Find(displayName);
            if(cachedItem.IsOk()) {
                // the item already exists, return it
                wxDELETE(cd);
                return cachedItem;
            }
        }

        int imgIdx = m_bmpLoader->GetMimeImageId(FileUtil::TypeFolder);
        int imgOpenedIDx = m_bmpLoader->GetMimeImageId(FileUtil::TypeFolderExpanded);
        wxTreeItemId itemFolder = GetTreeCtrl()->AppendItem(parent, displayName, imgIdx, imgOpenedIDx, cd);

        // Add this entry to the index
        if(parentData->GetIndex()) { parentData->GetIndex()->Add(displayName, itemFolder); }

        // Append the dummy item
        GetTreeCtrl()->AppendItem(itemFolder, "Dummy", -1, -1, new TreeCtrlData(TreeCtrlData::kDummy));

        // Pin this folder
        if(GetConfig() && IsTopLevelFolder(itemFolder)) {
            wxArrayString pinnedFolders;
            GetConfig()->Read("ExplorerFolders", pinnedFolders, pinnedFolders);
            int where = pinnedFolders.Index(cd->GetPath());
            if(where == wxNOT_FOUND) { pinnedFolders.Add(cd->GetPath()); }
            GetConfig()->Write("ExplorerFolders", pinnedFolders);
        }
        return itemFolder;
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::GetSelections(wxArrayString& folders, wxArrayString& files)
    {
        wxArrayTreeItemIds d1, d2;
        GetSelections(folders, d1, files, d2);
    }
    //-----------------------------------------------------------------------------------
    TreeItemInfo TreeCtrlPanel::GetSelectedItemInfo()
    {
        TreeItemInfo info;
        wxArrayString folders, files;
        GetSelections(folders, files);

        folders.insert(folders.end(), files.begin(), files.end());
        if(folders.empty()) return info;

        info.m_paths = folders;
        info.m_item = wxTreeItemId();
        return info;
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnCloseFolder(wxCommandEvent& event)
    {
        wxArrayString paths, files;
        wxArrayTreeItemIds items, fileItems;
        GetSelections(paths, items, files, fileItems);

        if(items.IsEmpty()) return;
        for(size_t i = 0; i < items.GetCount(); ++i) {
            DoCloseFolder(items.Item(i));
        }
    }
    //-----------------------------------------------------------------------------------
    bool TreeCtrlPanel::IsTopLevelFolder(const wxTreeItemId& item)
    {
        TreeCtrlData* cd = GetItemData(item);
        return (cd && cd->IsFolder() && GetTreeCtrl()->GetItemParent(item) == GetTreeCtrl()->GetRootItem());
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnNewFile(wxCommandEvent& event)
    {
        wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        TreeCtrlData* cd = GetItemData(item);
        CHECK_PTR_RET(cd);
        CHECK_COND_RET(cd->IsFolder());

        wxString filename = ::GetUserText(_("New File"), _("Set the file name:"), m_newfileTemplate);
        if(filename.IsEmpty()) return; // user cancelled

        wxFileName file(cd->GetPath(), filename);

        // Write the file content
        if(!FileUtil::WriteFileContent(file, "")) return;
        wxTreeItemId newFile = DoAddFile(item, file.GetFullPath());
        GetTreeCtrl()->SortChildren(item);
        // Open the file in the editor
        GetManager()->OpenFile(file.GetFullPath());
        CallAfter(&TreeCtrlPanel::SelectItem, newFile);

        // Notify about file creation
        FileEvent fsEvent(wxEVT_FILE_CREATED);
        fsEvent.SetPath(file.GetFullPath());
        fsEvent.SetFileName(file.GetFullName());
        gsgs_Event().AddPendingEvent(fsEvent);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnNewFolder(wxCommandEvent& event)
    {
        wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        TreeCtrlData* cd = GetItemData(item);
        CHECK_PTR_RET(cd);
        CHECK_COND_RET(cd->IsFolder());

        wxString foldername = ::GetUserText(_("New Folder"), _("Set the folder name:"), "New Folder");
        if(foldername.IsEmpty()) return; // user cancelled

        wxFileName file(cd->GetPath(), "");
        file.AppendDir(foldername);

        // Create the folder
        wxFileName::Mkdir(file.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        // Add it to the tree view
        wxTreeItemId newFile = DoAddFolder(item, file.GetPath());
        GetTreeCtrl()->SortChildren(item);
        CallAfter(&TreeCtrlPanel::SelectItem, newFile);

        // Notify about folder creation
        FileEvent fsEvent(wxEVT_FOLDER_CREATED);
        fsEvent.SetPath(file.GetPath());
        gsgs_Event().AddPendingEvent(fsEvent);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::GetSelections(wxArrayString& folders, wxArrayTreeItemIds& folderItems, wxArrayString& files,
                                        wxArrayTreeItemIds& fileItems)
    {
        folders.clear();
        files.clear();
        folderItems.clear();
        fileItems.clear();

        wxArrayTreeItemIds items;
        if(GetTreeCtrl()->GetSelections(items)) {
            for(size_t i = 0; i < items.size(); ++i) {
                TreeCtrlData* cd = GetItemData(items.Item(i));
                if(cd) {
                    if(cd->IsFile()) {
                        files.Add(cd->GetPath());
                        fileItems.Add(items.Item(i));
                    } else if(cd->IsFolder()) {
                        folders.Add(cd->GetPath());
                        folderItems.Add(items.Item(i));
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::SelectItem(const wxTreeItemId& item)
    {
        CHECK_ITEM_RET(item);
        wxArrayTreeItemIds items;
        if(GetTreeCtrl()->GetSelections(items)) {
            for(size_t i = 0; i < items.size(); ++i) {
                GetTreeCtrl()->SelectItem(items.Item(i), false);
            }
        }

        GetTreeCtrl()->SelectItem(item);
        // GetTreeCtrl()->SetFocusedItem(item);
        GetTreeCtrl()->EnsureVisible(item);
    }
    //-----------------------------------------------------------------------------------
    struct FileOrFolder {
        wxTreeItemId item;
        bool folder;
        wxString path;
    };
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnDeleteSelections(wxCommandEvent& event)
    {
        wxArrayString folders, files;
        wxArrayTreeItemIds folderItems, fileItems;
        GetSelections(folders, folderItems, files, fileItems);
        if(files.empty() && folders.empty()) return;

        std::set<wxTreeItemId> selectedItems;
        std::vector<FileOrFolder> v;

        selectedItems.insert(folderItems.begin(), folderItems.end());
        selectedItems.insert(fileItems.begin(), fileItems.end());

        // loop over the selections and remove all items that their parents
        // also exists in the selected items list
        for(size_t i = 0; i < folderItems.size(); ++i) {
            wxTreeItemId item = folderItems.Item(i);
            bool foundParent = false;
            wxTreeItemId itemParent = GetTreeCtrl()->GetItemParent(item);
            while(itemParent.IsOk()) {
                if(selectedItems.count(itemParent)) {
                    // item's parent is in the list, don't delete it as it will get deleted
                    // by its parent
                    foundParent = true;
                    break;
                }
                itemParent = GetTreeCtrl()->GetItemParent(itemParent);
            }

            if(!foundParent) {
                FileOrFolder fof;
                fof.folder = true;
                fof.item = item;
                fof.path = folders.Item(i);
                v.push_back(fof);
            }
        }

        for(size_t i = 0; i < fileItems.size(); ++i) {
            wxTreeItemId item = fileItems.Item(i);
            bool foundParent = false;
            wxTreeItemId itemParent = GetTreeCtrl()->GetItemParent(item);
            while(itemParent.IsOk()) {
                if(selectedItems.count(itemParent)) {
                    // item's parent is in the list, don't delete it as it will get deleted
                    // by its parent
                    foundParent = true;
                    break;
                }
                itemParent = GetTreeCtrl()->GetItemParent(itemParent);
            }

            if(!foundParent) {
                FileOrFolder fof;
                fof.folder = false;
                fof.item = item;
                fof.path = files.Item(i);
                v.push_back(fof);
            }
        }

        // At this point "v" contains a unique list of items to delete
        // the items in "v" have no common parent
        wxString message;
        message << _("Are you sure you want to delete the selected items?");

        wxRichMessageDialog dialog(gsgs_Event().TopFrame(), message, _("Confirm"),
                                   wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER | wxICON_WARNING);

        wxWindowUpdateLocker locker(GetTreeCtrl());
        wxArrayTreeItemIds deletedItems;
        wxArrayString deletedFiles, deletedFolders;
        if(dialog.ShowModal() == wxID_YES) {
            wxLogNull nl;
            for(size_t i = 0; i < v.size(); ++i) {
                if(v.at(i).folder) {
                    if(wxFileName::Rmdir(v.at(i).path, wxPATH_RMDIR_RECURSIVE)) {
                        deletedItems.Add(v.at(i).item);
                        deletedFolders.Add(v.at(i).path);
                    }
                } else {
                    if(gsgsRemoveFile(v.at(i).path)) {
                        deletedItems.Add(v.at(i).item);
                        deletedFiles.Add(v.at(i).path);
                    }
                }
            }
        }

        // Notify about the folder/files deletion
        {
            FileEvent evt(wxEVT_FILE_DELETED);
            evt.SetPaths(deletedFiles);
            evt.SetEventObject(this);
            gsgs_Event().AddPendingEvent(evt);
        }
        {
            FileEvent evt(wxEVT_FOLDER_DELETED);
            evt.SetPaths(deletedFolders);
            evt.SetEventObject(this);
            gsgs_Event().AddPendingEvent(evt);
        }

        // Update the UI
        for(size_t i = 0; i < deletedItems.size(); ++i) {
            // Before we delete the item from the tree, update the parent cache
            UpdateItemDeleted(deletedItems.Item(i));
            // And now remove the item from the tree
            GetTreeCtrl()->Delete(deletedItems.Item(i));
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnOpenFile(wxCommandEvent& event)
    {
        wxArrayString folders, files;
        GetSelections(folders, files);

        for(size_t i = 0; i < files.size(); ++i) {
            // Fire an event before resolving to the default action
            Event fevent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
            fevent.SetEventObject(this);
            fevent.SetFileName(files.Item(i));
            if(!gsgs_Event().ProcessEvent(fevent)) { GetManager()->OpenFile(files.Item(i)); }
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnRenameFile(wxCommandEvent& event)
    {
        wxArrayString files, folders;
        wxArrayTreeItemIds fileItems, folderItems;
        GetSelections(folders, folderItems, files, fileItems);

        if(files.empty()) return;

        // Prompt and rename each file
        for(size_t i = 0; i < files.size(); ++i) {
            wxFileName oldname(files.Item(i));

            wxString newname =
                ::GetUserText(_("Rename File"), _("New Name:"), oldname.GetFullName(), wxStrlen(oldname.GetName()));
            if(!newname.IsEmpty() && (newname != oldname.GetFullName())) {
                TreeCtrlData* d = GetItemData(fileItems.Item(i));
                if(d) {
                    wxFileName oldpath = d->GetPath();
                    wxFileName newpath = oldpath;
                    newpath.SetFullName(newname);
                    if(::wxRenameFile(oldpath.GetFullPath(), newpath.GetFullPath(), false)) {
                        DoRenameItem(fileItems.Item(i), oldname.GetFullName(), newname);
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    bool TreeCtrlPanel::ExpandToFile(const wxFileName& filename)
    {
        wxArrayString topFolders;
        wxArrayTreeItemIds topFoldersItems;
        GetTopLevelFolders(topFolders, topFoldersItems);

        int where = wxNOT_FOUND;
        wxString fullpath = filename.GetFullPath();
        for(size_t i = 0; i < topFolders.size(); ++i) {
            if(fullpath.StartsWith(topFolders.Item(i))) {
                where = i;
                break;
            }
        }

        // Could not find a folder that matches the filename
        if(where == wxNOT_FOUND) return false;
        wxString topFolder = topFolders.Item(where);
        wxTreeItemId closestItem = topFoldersItems.Item(where);
        fullpath.Remove(0, topFolder.length());
        wxFileName left(fullpath);

        wxArrayString parts = left.GetDirs();
        parts.Add(filename.GetFullName());
        TreeCtrlData* d = GetItemData(closestItem);
        while(!parts.IsEmpty()) {
            if(!d->GetIndex()) return false; // ??
            wxTreeItemId child = d->GetIndex()->Find(parts.Item(0));
            if(!child.IsOk()) {
                // Probably the this folder was not expanded just yet...
                if(d->IsFolder()) {
                    DoExpandItem(closestItem, true);
                    // Try again
                    child = d->GetIndex()->Find(parts.Item(0));
                    if(!child.IsOk()) { return false; }
                }
            }
            closestItem = child;
            d = GetItemData(closestItem);
            parts.RemoveAt(0);
        }

        if(parts.IsEmpty()) {
            // we found our file!
            SelectItem(closestItem);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::GetTopLevelFolders(wxArrayString& paths, wxArrayTreeItemIds& items) const
    {
        wxTreeItemIdValue cookie;
        const TreeCtrl* tree = m_treeCtrl;
        wxTreeItemId child = tree->GetFirstChild(tree->GetRootItem(), cookie);
        while(child.IsOk()) {
            TreeCtrlData* clientData = GetItemData(child);
            paths.Add(clientData->GetPath());
            items.Add(child);
            child = tree->GetNextChild(tree->GetRootItem(), cookie);
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnActiveEditorChanged(wxCommandEvent& event)
    {
        event.Skip();
        if(GetManager()->GetActiveEditor() && (m_options & kLinkToEditor)) {
            ExpandToFile(GetManager()->GetActiveEditor()->GetFileName());
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::UpdateItemDeleted(const wxTreeItemId& item)
    {
        wxTreeItemId parent = GetTreeCtrl()->GetItemParent(item);
        CHECK_ITEM_RET(parent);

        TreeCtrlData* parentData = GetItemData(parent);
        wxString text = GetTreeCtrl()->GetItemText(item);

        // Update the parent cache
        if(parentData->GetIndex()) { parentData->GetIndex()->Delete(text); }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::DoRenameItem(const wxTreeItemId& item, const wxString& oldname, const wxString& newname)
    {
        // Update the UI + client data
        TreeCtrlData* d = GetItemData(item);
        if(d->IsFile()) {
            wxFileName fn(d->GetPath());
            fn.SetFullName(newname);
            d->SetPath(fn.GetFullPath());
        } else if(d->IsFolder()) {
            // FIXME:
        }

        GetTreeCtrl()->SetItemText(item, newname);

        // Update the parent's cache
        wxTreeItemId parent = GetTreeCtrl()->GetItemParent(item);
        CHECK_ITEM_RET(parent);
        TreeCtrlData* parentData = GetItemData(parent);

        // Update the parent cache
        if(parentData->GetIndex()) {
            parentData->GetIndex()->Delete(oldname);
            parentData->GetIndex()->Add(newname, item);
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnFindInFilesFolder(wxCommandEvent& event)
    {
        wxArrayString folders, files;
        GetSelections(folders, files);

        if(folders.IsEmpty()) return;
        GetManager()->OpenFindInFileForPaths(folders);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnOpenContainingFolder(wxCommandEvent& event)
    {
        wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        TreeCtrlData* cd = GetItemData(item);

        if(cd && cd->IsFolder()) {
            FileUtil::OpenFileExplorer(cd->GetPath());
        } else if(cd && cd->IsFile()) {
            wxFileName fn(cd->GetPath());
            FileUtil::OpenFileExplorerAndSelect(fn);
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnOpenShellFolder(wxCommandEvent& event)
    {
        wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        TreeCtrlData* cd = GetItemData(item);

        if(cd && cd->IsFolder()) {
            FileUtil::OpenTerminal(cd->GetPath());
        } else if(cd && cd->IsFile()) {
            wxFileName fn(cd->GetPath());
            FileUtil::OpenTerminal(fn.GetPath());
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnInitDone(wxCommandEvent& event)
    {
        event.Skip();
        if(GetConfig()) {
            wxArrayString pinnedFolders;
            GetConfig()->Read("FileExplorer/Options", m_options, m_options);
            GetConfig()->Read("ExplorerFolders", pinnedFolders, pinnedFolders);
            for(size_t i = 0; i < pinnedFolders.size(); ++i) {
                AddFolder(pinnedFolders.Item(i));
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::Clear()
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId item = GetTreeCtrl()->GetFirstChild(GetTreeCtrl()->GetRootItem(), cookie);
        while(item.IsOk()) {
            DoCloseFolder(item);
            item = GetTreeCtrl()->GetNextChild(GetTreeCtrl()->GetRootItem(), cookie);
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::DoCloseFolder(const wxTreeItemId& item)
    {
        CHECK_ITEM_RET(item);
        if(!IsTopLevelFolder(item)) return;

        if(GetConfig()) {
            // If this folder is a pinned one, remove it
            wxArrayString pinnedFolders;
            GetConfig()->Read("ExplorerFolders", pinnedFolders, pinnedFolders);
            TreeCtrlData* d = GetItemData(item);
            int where = pinnedFolders.Index(d->GetPath());
            if(where != wxNOT_FOUND) { pinnedFolders.RemoveAt(where); }
            GetConfig()->Write("ExplorerFolders", pinnedFolders);
        }
        // Now, delete the item
        GetTreeCtrl()->Delete(item);

        ToggleView();
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::ToggleView()
    {
        wxArrayString paths;
        wxArrayTreeItemIds items;
        GetTopLevelFolders(paths, items);

        if(paths.IsEmpty()) {
            GetTreeCtrl()->Hide();
            m_defaultView->Show();
        } else {
            GetTreeCtrl()->Show();
            m_defaultView->Hide();
        }
        GetSizer()->Layout();
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnRefresh(wxCommandEvent& event)
    {
        wxArrayString paths, files;
        wxArrayTreeItemIds items, fileItems;
        GetSelections(paths, items, files, fileItems);
        if(items.IsEmpty()) { return; }

        // If we have a top level folder, ignore any non top level folder
        bool hasTopLevelFolder = false;
        for(size_t i = 0; i < items.GetCount(); ++i) {
            if(IsTopLevelFolder(items.Item(i))) {
                hasTopLevelFolder = true;
                break;
            }
        }

        if(!hasTopLevelFolder) {
            // Non top level folders
            for(size_t i = 0; i < items.GetCount(); ++i) {
                RefreshNonTopLevelFolder(items.Item(i));
            }
        } else {
            // Close the selected folders
            std::vector<std::pair<wxString, bool> > topFolders;
            for(size_t i = 0; i < items.GetCount(); ++i) {
                topFolders.push_back(std::make_pair(paths.Item(i), GetTreeCtrl()->IsExpanded(items.Item(i))));
                DoCloseFolder(items.Item(i));
            }

            // Re-add them
            for(size_t i = 0; i < topFolders.size(); ++i) {
                wxTreeItemId itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), topFolders.at(i).first);
                DoExpandItem(itemFolder, topFolders.at(i).second);
            }
            GetTreeCtrl()->SortChildren(GetTreeCtrl()->GetRootItem());
            ToggleView();
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::SetNewFileTemplate(const wxString& newfile, size_t charsToHighlight)
    {
        m_newfileTemplate = newfile;
        m_newfileTemplateHighlightLen = charsToHighlight;
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnOpenFolder(wxCommandEvent& event)
    {
        wxCommandEvent eventOpenFolder(wxEVT_MENU, XRCID("open_folder"));
        wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(eventOpenFolder);
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnOpenWithDefaultApplication(wxCommandEvent& event)
    {
        wxArrayString folders, files;
        GetSelections(folders, files);

        for(size_t i = 0; i < files.size(); ++i) {
            ::wxLaunchDefaultApplication(files.Item(i));
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnFindInFilesShowing(FindInFilesEvent& event)
    {
        event.Skip();
        if(IsShownOnScreen() && GetTreeCtrl()->HasFocus()) {
            wxArrayString folders, files;
            GetSelections(folders, files);

            wxString paths = event.GetTransientPaths();
            paths.Trim().Trim(false);
            if(!paths.IsEmpty()) { paths << "\n"; }
            for(size_t i = 0; i < folders.size(); ++i) {
                paths << folders.Item(i) << "\n";
            }
            paths.Trim();
            event.SetTransientPaths(paths);
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnLinkEditor(wxCommandEvent& event)
    {
        if(event.IsChecked()) {
            m_options |= kLinkToEditor;
        } else {
            m_options &= ~kLinkToEditor;
        }
        if(GetConfig()) { GetConfig()->Write("FileExplorer/Options", m_options); }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnLinkEditorUI(wxUpdateUIEvent& event)
    {
        if(IsFolderOpened()) {
            event.Enable(true);
            event.Check(m_options & kLinkToEditor);
        } else {
            event.Enable(false);
        }
    }
    //-----------------------------------------------------------------------------------
    bool TreeCtrlPanel::IsFolderOpened() const
    {
        wxArrayString paths;
        wxArrayTreeItemIds items;
        GetTopLevelFolders(paths, items);
        return !paths.IsEmpty();
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::RefreshNonTopLevelFolder(const wxTreeItemId& item)
    {
        CHECK_ITEM_RET(item);
        TreeCtrlData* cd = GetItemData(item);
        CHECK_PTR_RET(cd);
        CHECK_COND_RET(cd->IsFolder());

        // Collpase the item if needed
        bool expandItem = GetTreeCtrl()->IsExpanded(item);
        if(expandItem) { GetTreeCtrl()->Collapse(item); }

        // Clear the item children
        GetTreeCtrl()->DeleteChildren(item);

        // Append the dummy item
        GetTreeCtrl()->AppendItem(item, "Dummy", -1, -1, new TreeCtrlData(TreeCtrlData::kDummy));

        // Clear the folder index
        if(cd->GetIndex()) { cd->GetIndex()->Clear(); }

        // Re-expand the item
        if(expandItem) { CallAfter(&TreeCtrlPanel::DoExpandItem, item, true); }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::OnRenameFolder(wxCommandEvent& event)
    {
        wxArrayString files, folders;
        wxArrayTreeItemIds fileItems, folderItems;
        GetSelections(folders, folderItems, files, fileItems);

        if(folderItems.size() != 1) { return; }
        wxTreeItemId item = folderItems.Item(0);
        if(IsTopLevelFolder(item)) {
            gsgsWARNING() << "Renaming of top level folder is not supported";
            return;
        }

        TreeCtrlData* d = GetItemData(item);
        CHECK_PTR_RET(d);
        CHECK_COND_RET(d->IsFolder());
        wxString newName = ::GetUserText(_("Rename folder"), _("Type the new folder name:"), d->GetName());
        if((newName == d->GetName()) || newName.IsEmpty()) { return; }
        wxFileName oldFullPath(d->GetPath(), "");
        wxFileName newFullPath(oldFullPath);
        newFullPath.RemoveLastDir();
        newFullPath.AppendDir(newName);
        gsgsDEBUG1() << "Renaming:" << oldFullPath.GetPath() << "->" << newFullPath.GetPath();
        if(::wxRename(oldFullPath.GetPath(), newFullPath.GetPath()) == 0) {
            // Rename was successful
            d->SetPath(newFullPath.GetPath());
            m_treeCtrl->SetItemText(item, newName);
            CallAfter(&TreeCtrlPanel::RefreshNonTopLevelFolder, item);

        } else {
            gsgsWARNING() << "Failed to rename folder:" << oldFullPath << "->" << newFullPath;
        }
    }
    //-----------------------------------------------------------------------------------
    void TreeCtrlPanel::RefreshTree()
    {
        // Close the selected folders
        wxArrayString paths;
        wxArrayTreeItemIds items;
        GetTopLevelFolders(paths, items);

        for(size_t i = 0; i < items.size(); ++i) {
            bool isExpanded = GetTreeCtrl()->IsExpanded(items.Item(i));
            DoCloseFolder(items.Item(i));
            wxTreeItemId itemFolder = DoAddFolder(GetTreeCtrl()->GetRootItem(), paths.Item(i));
            DoExpandItem(itemFolder, isExpanded);
        }

        GetTreeCtrl()->SortChildren(GetTreeCtrl()->GetRootItem());
        ToggleView();
    }
    //-----------------------------------------------------------------------------------
}