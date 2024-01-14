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
#ifndef gsgs_PLUGINMANAGER_H
#define gsgs_PLUGINMANAGER_H

#include "gsgsPreInclude.h"
#include "plugin.h"
#include "project.h"

namespace gsgs
{
    class WorkspaceView;
    class ImageManager;
    class PluginManager : public IManager
    {
    public:
        static PluginManager* Get();

        virtual void Load();
        virtual void UnLoad();
        virtual void EnableToolbars();

        /**
         * \brief return a map of all loaded plugins
         */
        const PluginInfoArray& GetPluginsInfo() const { return m_pluginsData; }
        void SetDockingManager(wxAuiManager* dockingManager) { m_dockingManager = dockingManager; }

        //------------------------------------
        // Implementation of IManager interface
        //------------------------------------
        void EnableClangCodeCompletion(bool b);
        IEditor* GetActiveEditor();
        ToolBar* GetToolBar();
        ConfigTool* GetConfigTool();
        TreeItemInfo GetSelectedTreeItemInfo(TreeType type);
        TreeCtrl* GetFileExplorerTree();
        TreeCtrl* GetWorkspaceTree();
        Notebook* GetOutputPaneNotebook();
        Notebook* GetWorkspacePaneNotebook();
        Notebook* GetPerspectivePaneNotebook();
        IEditor* OpenFile(const wxString& fileName, const wxString& projectName = wxEmptyString, int lineno = wxNOT_FOUND,
                          OF_extra flags = OF_AddJump);
        IEditor* OpenFile(const wxString& fileName, const wxBitmap& bmp, const wxString& tooltip = wxEmptyString);
        IEditor* OpenFile(const BrowseRecord& rec);
        wxString GetStartupDirectory() const;
        void AddProject(const wxString& path);
        bool IsOpen() const;
        TagsManager* GetTagsManager();
        CxxWS* GetWorkspace();
        bool AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths);
        bool AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths);
        bool AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths);
        int GetToolbarIconSize();
        wxAuiManager* GetAuiManager();
        EditorConfig* GetEnv();
        JobQueue* GetJobQueue();
        wxString GetProjectExecutionCommand(const wxString& projectName, wxString& wd);
        wxApp* GetTheApp();
        void ReloadWorkspace();
        Plugin* GetPlugin(const wxString& pluginName);
        wxEvtHandler* GetOutputWindow();
        bool SaveAll();
        wxString GetInstallDirectory() const;
        bool CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName);
        OptionsConfig * GetEditorSettings();
        void FindAndSelect(const wxString& pattern, const wxString& name, int pos = 0);
        TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
        bool AllowToolbar();
        void SetStatusMessage(const wxString& msg, int seconds_to_live = wxID_ANY);
        void PushQueueCommand(const QueueCommand& cmd);
        void ProcessCommandQueue();
        void StopAndClearQueue();
        bool IsBuildInProgress() const;
        bool IsBuildEndedSuccessfully() const;
        wxString GetProjectNameByFile(wxString& fullPathFileName);
        wxString GetProjectNameByFile(const wxString& fullPathFileName);
        BuildManager* GetBuildManager();
        BuildSettingsConfig* GetBuildSettingsConfigManager();
        bool ClosePage(const wxString& title);
        bool ClosePage(const wxFileName& filename);
        wxWindow* FindPage(const wxString& text);
        bool AddPage(wxWindow* win, const wxString& text, const wxString& tooltip = wxEmptyString,
                     const wxBitmap& bmp = wxNullBitmap, bool selected = false);
        bool SelectPage(wxWindow* win);
        NavMgr* GetNavigationMgr();
        IEditor* NewEditor();
        wxStyledTextCtrl * CreateEditor(wxWindow* parent);
        bool CloseEditor(IEditor* editor, bool prompt = true);
        bool IsShutdownInProgress() const;
        ImageManager* GetStdIcons();
        wxArrayString GetProjectCompileFlags(const wxString& projectName, bool isCppFile);
        void AddEditorPage(wxWindow* page, const wxString& name, const wxString& tooltip = wxEmptyString);
        wxPanel* GetEditorPaneNotebook();
        wxWindow* GetActivePage();
        wxWindow* GetPage(size_t page);
        size_t GetPageCount() const;
        wxString GetPageTitle(wxWindow* win) const;
        void SetPageTitle(wxWindow* win, const wxString& title);
        ProjectPtr GetSelectedProject() const;
        void RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files);
        IEditor* FindEditor(const wxString& filename) const;
        size_t GetAllEditors(IEditor::List_t& editors, bool inOrder = false);
        size_t GetAllTabs(TabVec_t& tabs);
        size_t GetAllBreakpoints(BreakpointInfo::Vec_t& breakpoints);
        void DeleteAllBreakpoints();
        void SetBreakpoints(const BreakpointInfo::Vec_t& breakpoints);
        void LoadPerspective(const wxString& perspectiveName);
        void SavePerspective(const wxString& perspectiveName);
        void ProcessEditEvent(wxCommandEvent& e, IEditor* editor);
        void AppendOutputTabText(eOutputPaneTab tab, const wxString& text);
        void ClearOutputTab(eOutputPaneTab tab);
        void AddWorkspaceToRecentlyUsedList(const wxFileName& filename);
        void StoreWorkspaceSession(const wxFileName& workspaceFile);
        void LoadWorkspaceSession(const wxFileName& workspaceFile);
        void OpenFindInFileForPath(const wxString& path);
        void OpenFindInFileForPaths(const wxArrayString& paths);
        void ShowOutputPane(const wxString& selectedWindow = "");
        void ToggleOutputPane(const wxString& selectedWindow = "");
        StatusBar* GetStatusBar();
        EditorBar* GetNavigationBar();
        WorkspaceView* GetWorkspaceView();
        bool IsToolBarShown() const;
        void ShowToolBar(bool show = true);

        /**
         * @brief display message to the user using the info bar
         */
        virtual void DisplayMessage(const wxString& message, int flags = wxICON_INFORMATION,
                                    const std::vector<std::pair<wxWindowID, wxString> >& buttons = {});

        //------------------------------------
        // End of IManager interface
        //------------------------------------

        // (Un)Hook the contect menus
        virtual void HookPopupMenu(wxMenu* menu, MenuType type);

        // (Un)Hook the project settings tab
        virtual void HookProjectSettingsTab(wxBookCtrlBase * book, const wxString & projectName, const wxString & configName);
        virtual void UnHookProjectSettingsTab(wxBookCtrlBase * book, const wxString & projectName, const wxString & configName);
    private:
        PluginManager();
        virtual ~PluginManager();
    private:
        std::map<wxString, Plugin*> m_plugins;
        std::list<DynModule*> m_dl;
        PluginInfoArray m_pluginsData;
        //ImageManager* m_bmpLoader;
        std::set<MenuType> m_menusToBeHooked;
        std::map<wxString, wxString> m_backticks;
        wxAuiManager* m_dockingManager;
    };
}
#endif