/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#ifndef gsgsWORKSPACE_PANE_H
#define gsgsWORKSPACE_PANE_H

#include "gsgsPreInclude.h"
#include "gsgsNotebook.h"
#include "gsgsImageManager.h"
#include "gsgsTreeCtrlPanel.h"
#include "gsgsToolBar.h"
#include "gsgsCommonHandler.h"
#include "gsgsFileView.h"
#include "CommonController1.h"
#include "imanager.h"
#include "wxcrafter.h"
#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/gauge.h>
#include <wx/treectrl.h>
#include <utility>

class ProjectSettingsDlg;

namespace gsgs
{
    class WorkspaceView;
    class FileExplorer : public wxPanel
    {
    public:
        FileExplorer(wxWindow* parent, const wxString& caption);
        virtual ~FileExplorer();
        inline const wxString& GetCaption() const       { return m_caption; }
        inline TreeItemInfo GetItemInfo()               { return m_view->GetSelectedItemInfo(); }
        inline TreeCtrl* GetTree()                      { return m_view->GetTreeCtrl(); }
        void OpenFolder(const wxString& path);
    protected:
        void OnFolderDropped(Event& event);
        void OnOpenFolder(wxCommandEvent& event);
    private:
        TreeCtrlPanel* m_view;
        wxString m_caption;
        ThemeHandlerHelper* m_themeHelper;
    };

    class TabgroupsTab : public wxPanel
    {
    public:
        TabgroupsTab() { m_node = NULL; }
        TabgroupsTab(wxWindow* parent, const wxString& caption);
        ~TabgroupsTab();
        void DisplayTabgroups(bool isGlobal = false);
        bool AddNewTabgroupToTree(bool isGlobal, const wxString& newfilepath, wxTreeItemId selection = wxTreeItemId());
        void FileDropped(const wxString& filename);
        /*!
         * \brief Returns the 'root' item for either Global or Workspace tabgroups
         */
        wxTreeItemId GetRootItemForTabgroup(bool global);

    protected:
        void AddFile(const wxString& filename);

        void AddTreeItem(bool isGlobal,
            const wxString& tabgroupname,
            const wxArrayString& tabfilepaths,
            const wxTreeItemId insertafter = wxTreeItemId());
        void AddTabgroupItem();
        void PasteTabgroupItem(wxTreeItemId itemtopaste = wxTreeItemId());
        void DeleteTabgroup();
        void DuplicateTabgroup();
        void CopyTabgroupItem(wxTreeItemId itemtocopy = wxTreeItemId());
        void DeleteTabgroupItem(bool DoCut = false, wxTreeItemId itemtocopy = wxTreeItemId());
        int DoGetIconIndex(const wxString& filename);

        void OnContextMenu(wxCommandEvent& event);
        // The next 4 methods deal with keypresses, not the context menu
        void OnCopy(wxCommandEvent& WXUNUSED(event))        { CopyTabgroupItem(); }
        void OnPaste(wxCommandEvent& WXUNUSED(event))       { PasteTabgroupItem(); }
        void OnCut(wxCommandEvent& WXUNUSED(event))         { DeleteTabgroupItem(true); }
        void OnDelete(wxCommandEvent& WXUNUSED(event));

        void OnItemActivated(wxTreeEvent& event);
        void OnItemRtClick(wxTreeEvent& event);
        void OnBeginLabelEdit(wxTreeEvent& event);
        void OnEndLabelEdit(wxTreeEvent& event);

        void OnBeginDrag(wxTreeEvent& event);
        void OnEndDrag(wxTreeEvent& event);
        void OnWorkspaceClosed(wxCommandEvent& e);
        void OnInitDone(wxCommandEvent& e);
    protected:
        enum tabgroupmenuitems {
            TGM_ID_Add = 10000,
            TGM_ID_Paste,
            TGM_ID_Delete,
            TGM_ID_Duplicate,
            TGM_ID_CopyItem,
            TGM_ID_CutItem,
            TGM_ID_RemoveItem
        };

        ThemeHandlerHelper* m_themeHelper;

        DefaultTreeCtrl* m_tree;
        /*!
         * \brief Stores the dragged item during DnD
         */
        wxTreeItemId m_draggedItem;
        /*!
         * \brief Used as a temporary store for a copyied/cut item's data
         */
        wxXmlNode* m_node;
        /*!
         * \brief Used as a temporary store for the filepath of a copied/cut item
         */
        wxString m_copieditem_filepath;
    };

    class OpenWindowsTab : public wxPanel
    {
    public:
        OpenWindowsTab(wxWindow* parent, const wxString& caption, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxSize(500, 300), long style = wxWANTS_CHARS | wxTAB_TRAVERSAL);
        ~OpenWindowsTab();
        
        const wxString & GetCaption() const         { return m_caption; }
        ThemedListCtrl * GetDvListCtrl() const      { return m_dvListCtrl; }
    protected:
        wxString GetEditorPath(wxDataViewItem item);
        bool IsEditor(wxDataViewItem item) const;
        void DoSaveItem(wxDataViewItem item);
        void DoCloseItem(wxDataViewItem item);
        void DoSelectItem(IEditor* editor);
        void DoSelectItem(wxWindow* win);
        void SortAlphabetically();
        void SortByEditorOrder();
        void AppendEditor(const Tab& tab);
        void Clear();
        void PopulateView();
        void DoMarkModify(const wxString& filename, bool b);
        wxVariant PrepareValue(const Tab& tab);
    protected:
        virtual void OnMenu(wxDataViewEvent& event);
        virtual void OnTabActivated(wxDataViewEvent& event);
        virtual void OnTabSelected(wxDataViewEvent& event);
        //void OnInitDone(wxCommandEvent& event);

        // Handlers for OpenWindowsPanelBase events.
        void OnSortItems(wxCommandEvent& event);
        void OnSortItemsUpdateUI(wxUpdateUIEvent& event);

        // Handlers for workspace events
        void OnActiveEditorChanged(wxCommandEvent& e);
        void OnActivePageChanged(wxCommandEvent& e);
        void OnAllEditorsClosed(wxCommandEvent& e);

        void OnCloseSelectedFiles(wxCommandEvent& e);
        void OnSaveSelectedFiles(wxCommandEvent& e);
        void OnInitDone(wxIdleEvent& event);
        void OnEditorModified(Event& event);
        void OnEditorSaved(Event& event);
        void OnWorkspaceClosed(wxCommandEvent& event);
        void OnWorkspaceClosing(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()
    protected:
        std::map<wxString, wxDataViewItem> m_editors;
        ThemedListCtrl* m_dvListCtrl;
        ToolBar* m_toolbar;
        wxString m_caption;
        IManager* m_mgr;
        bool m_workspaceClosing;
        bool m_workspaceOpened;
        bool m_initDone;
    };

    class WorkspaceTab : public wxPanel
    {
        friend class ProjectSettingsDlg;
    public:
        WorkspaceTab(wxWindow* parent, const wxString& caption, wxWindowID id = wxID_ANY, 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), 
                long style = wxTAB_TRAVERSAL);
        ~WorkspaceTab();

        void OpenProjectSettings(const wxString& project = "");

        ToolBar* GetToolbar580() const                      { return m_toolbar580; }
        ConfigurationPanel* GetConfigPanel() const          { return m_configPanel; }
        ThemedListCtrl* GetDvListCtrlPinnedProjects() const { return m_dvListCtrlPinnedProjects; }
        //wxPanel* GetSplitterPagePinnedProjects() const      { return m_splitterPagePinnedProjects; }
        //wxPanel* GetSplitterPageTreeView() const            { return m_splitterPageTreeView; }
        SplitterWindow* GetSplitter() const                 { return m_splitter; }
        wxPanel* GetPanelCxx() const                        { return m_panelCxx; }
        wxSimplebook* GetSimpleBook() const                 { return m_simpleBook; }

        inline FileViewTree* GetFileView()                  { return m_fileView; }
        inline const wxString& GetCaption() const           { return m_caption; }
        void FreezeThaw(bool freeze = true);

        /**
         * @brief return the workspace view class
         * @return
         */
        WorkspaceView* GetView() { return m_view; }
        /**
         * @brief
         * @param project
         */
        void AddPinnedProject(const wxString& project);
    protected:
        virtual void OnPinnedCxxProjectSelected(wxDataViewEvent& event);
        virtual void OnPinnedCxxProjectContextMenu(wxDataViewEvent& event);
        void ProjectSettingsDlgClosed();
        void DoGoHome();
        void DoConfigChanged(const wxString& newConfigName);
        void LoadCxxPinnedProjects();
        void SaveCxxPinnedProjects();

        void ShowPinnedProjectMenu(const wxString& project);

    protected:
        virtual void OnWorkspaceOpenUI(wxUpdateUIEvent& event);
        virtual void OnLinkEditorUI(wxUpdateUIEvent& event);

        void OnFolderDropped(Event& event);
        void OnPaint(wxPaintEvent& event);
        void DoWorkspaceConfig();
        void DoUpdateChoiceWithProjects();

        void OnLinkEditor(wxCommandEvent& e);
        void OnCollapseAll(wxCommandEvent& e);
        void OnCollapseAllUI(wxUpdateUIEvent& e);
        void OnGoHome(wxCommandEvent& e);
        void OnGoHomeUI(wxUpdateUIEvent& e);
        void OnProjectSettingsUI(wxUpdateUIEvent& e);
        void OnProjectSettings(wxCommandEvent& e);
        void OnShowFile(wxCommandEvent& e);
        void OnShowFileUI(wxUpdateUIEvent& e);

        void OnWorkspaceLoaded(wxCommandEvent& e);
        void OnWorkspaceClosed(wxCommandEvent& e);
        void OnProjectAdded(Event& e);
        void OnProjectRemoved(Event& e);
        void OnActiveEditorChanged(wxCommandEvent& e);
        void OnEditorClosing(wxCommandEvent& e);
        void OnWorkspaceConfig(wxCommandEvent& e);
        void OnConfigurationManager(wxCommandEvent& e);
        void OnConfigChanged(Event& e);
        void OnActiveProjectChanged(ProjectSettingsEvent& e);
    private:
        wxString m_caption;
        wxSimplebook* m_simpleBook;
        wxPanel* m_panelCxx;
        ToolBar* m_toolbar580;
        ConfigurationPanel* m_configPanel;
        SplitterWindow* m_splitter;
        //wxPanel* m_splitterPagePinnedProjects;
        ThemedListCtrl* m_dvListCtrlPinnedProjects;
        //wxPanel* m_splitterPageTreeView;
        FileViewTree* m_fileView;
        ThemeHandlerHelper* m_themeHelper;
        ProjectSettingsDlg* m_dlg;
        WorkspaceView* m_view;
        wxColour m_bgColour;
        wxArrayString m_cxxPinnedProjects;
        BitmapList m_bitmaps;
        bool m_isLinkedToEditor;
    };

    class WorkspacePane : public wxPanel
    {
    public:
        WorkspacePane(wxWindow* parent, const wxString & caption, wxAuiManager * mag);
        ~WorkspacePane();

        void UpdateProgress(int val);
        void ClearProgress();
        void ApplySavedTabOrder() const;
        void SaveWorkspaceViewTabOrder() const;
        bool IsTabVisible(int flag);

        inline const wxString& GetCaption() const       { return m_caption; }
        inline Notebook* GetNotebook() const            { return m_book; }
        inline WorkspaceTab* GetWorkspaceTab() const    { return m_workspaceTab; }
        inline FileExplorer* GetFileExplorer() const    { return m_explorer; }
        //inline TabgroupsTab* GetTabgroupsTab() const   { return m_TabgroupsPane; }
        inline TabgroupsTab* GetServiceTab() const     { return m_ServicePane; }
        /**
         * @brief set an active tab by its title
         * @param tabTitle the tab to select
         */
        void SelectTab(const wxString& tabTitle);
    protected:
        void DoShowTab(bool show, const wxString& title);
        wxWindow* DoGetControlByName(const wxString& title);
        //TabViewPtr_t GetNotebookRenderer();

        void OnInitDone(wxCommandEvent& event);
        void OnSettingsChanged(wxCommandEvent& event);
        void OnToggleWorkspaceTab(Event& event);
        void OnWorkspaceBookFileListMenu(ContextMenuEvent& event);
    protected:
        struct Tab 
        {
            Tab(const wxString& label, wxWindow* win, const wxBitmap& bmp = wxNullBitmap)
                : m_label(label)
                , m_window(win)
                , m_bmp(bmp){}

            Tab() : m_window(NULL){}

            wxString m_label;
            wxWindow* m_window;
            wxBitmap m_bmp;
        };

        std::map<wxString, Tab> m_tabs;
    private:
        wxString m_caption;
        wxAuiManager * m_mgr;
        wxGauge * m_parsingProgress;
        //wxStaticText * m_staticText;
        Notebook * m_book;
        //TabgroupsTab * m_TabgroupsPane;
        TabgroupsTab * m_ServicePane;
    #ifndef __WXOSX__
        OpenWindowsTab * m_openWindowsPane;
    #endif

        FileExplorer * m_explorer;
        WorkspaceTab * m_workspaceTab;
        AuiCaptionEnabler m_captionEnabler;
    };
}
#endif
