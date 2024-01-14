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
#ifndef FILE_VIEW_TREE_H
#define FILE_VIEW_TREE_H

#include "gsgsPreInclude.h"
#include "gsgsTreeCtrl.h"
#include "gsgsPluginManager.h"
#include "project.h"
#include "imanager.h"
#include "wx/treectrl.h"
#include "wx/menu.h"

namespace gsgs
{
    struct FileViewItem {
        wxString virtualDir;
        wxString fullpath;
        wxString displayName;
    };

    class FileViewTree : public DefaultTreeCtrl
    {
    public:
        /**
         * Parameterized constructor.
         * @param parent Tree parent window
         * @param id Window id
         * @param pos Window position
         * @param size Window size
         * @param style Window style
         */
        FileViewTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = wxBORDER_NONE);

        /**
         * Destructor .
         */
        virtual ~FileViewTree();
        
        const wxString & GetLastCreateFile() const      { return mLastCreateFile; }
        /**
         * @brief build project context menu
         */
        void CreateProjectContextMenu(wxMenu& menu, const wxString& projectName, bool usedByFileView = true);

        /**
         * @brief mark project with given name as active project
         * @param projectName
         */
        void MarkActive(const wxString& projectName);

        /**
         * @brief return the current selected item information
         */
        TreeItemInfo GetSelectedItemInfo();

        /**
         * Make sure that fileName under project is visible
         * @param &project
         * @param &fileName
         */
        void ExpandToPath(const wxString& project, const wxFileName& fileName);

        /**
         * @brief create and add file to the tree
         * @param filename new file full path
         * @param vdFullPath files' virtual folder full path (format-> project:path:to:vd)
         */
        bool CreateAndAddFile(const wxString& filename, const wxString& vdFullPath);

        bool AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths);
        bool AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths);
        bool AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths);
        void RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files);
        bool CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName);

        /**
         * @brief return the currently selected project *not the active*
         * If a file is selected, we go up until we find the first project item
         */
        ProjectPtr GetSelectedProject() const;

        /**
         * @brief find the parent project of a given item
         */
        ProjectPtr GetItemProject(const wxTreeItemId& item) const;

        /**
         * @brief public access to the "OnFolderDropped" function
         * @param event
         */
        void FolderDropped(const wxArrayString& folders);

        // Build the actual tree from the workspace
        void BuildTree();
    protected:
        void OnItemExpanding(wxTreeEvent& e);
        virtual void OnPopupMenu(wxTreeEvent& event);
        virtual void OnItemActivated(wxTreeEvent& event);
        virtual void OnTreeKeyDown(wxTreeEvent& event);
        virtual void OnSelectionChanged(wxTreeEvent& e);
        virtual void OnOpenInEditor(wxCommandEvent& event);
        virtual void OnRemoveProject(wxCommandEvent& event);
        virtual void OnRenameProject(wxCommandEvent& event);
        virtual void OnSetActive(wxCommandEvent& event);
        virtual void OnNewItem(wxCommandEvent& event);
        virtual void OnAddExistingItem(wxCommandEvent& event);
        virtual void OnNewVirtualFolder(wxCommandEvent& event);
        virtual void OnLocalPrefs(wxCommandEvent& event);
        virtual void OnProjectProperties(wxCommandEvent& event);
        virtual void OnRemoveVirtualFolder(wxCommandEvent& event);
        virtual void OnRemoveItem(wxCommandEvent& event);
        virtual void OnSaveAsTemplate(wxCommandEvent& event);
        virtual void OnBuildOrder(wxCommandEvent& event);
        virtual void OnClean(wxCommandEvent& event);
        virtual void OnBuild(wxCommandEvent& event);
        virtual void OnPinProject(wxCommandEvent& event);
        virtual void OnReBuild(wxCommandEvent& event);
        virtual void OnRunPremakeStep(wxCommandEvent& event);
        virtual void OnBuildProjectOnly(wxCommandEvent& event);
        virtual void OnCleanProjectOnly(wxCommandEvent& event);
        virtual void OnRebuildProjectOnly(wxCommandEvent& event);
        virtual void OnStopBuild(wxCommandEvent& event);
        virtual void OnRetagProject(wxCommandEvent& event);
        virtual void OnRetagWorkspace(wxCommandEvent& event);
        virtual void OnBuildInProgress(wxUpdateUIEvent& event);
        virtual void OnExcludeFromBuildUI(wxUpdateUIEvent& event);
        virtual void OnRetagInProgressUI(wxUpdateUIEvent& event);
        virtual void OnItemBeginDrag(wxTreeEvent& event);
        virtual void OnItemEndDrag(wxTreeEvent& event);
        virtual void OnImportDirectory(wxCommandEvent& e);
        virtual void OnReconcileProject(wxCommandEvent& e);
        virtual void OnRenameItem(wxCommandEvent& e);
        virtual void OnCompileItem(wxCommandEvent& e);
        virtual void OnOpenShellFromFilePath(wxCommandEvent& e);
        virtual void OnOpenFileExplorerFromFilePath(wxCommandEvent& e);
        virtual void OnExcludeFromBuild(wxCommandEvent& e);
        virtual void OnPreprocessItem(wxCommandEvent& e);
        virtual void OnRenameVirtualFolder(wxCommandEvent& e);
        virtual void OnSetBgColourVirtualFolder(wxCommandEvent& e);
        virtual void OnClearBgColourVirtualFolder(wxCommandEvent& e);
        virtual wxTreeItemId ItemByFullPath(const wxString& fullPath);
        virtual void OnLocalWorkspaceSettings(wxCommandEvent& e);
        virtual void OnOpenWithDefaultApplication(wxCommandEvent& event);
        virtual void OnBuildTree(wxCommandEvent& e);
        void OnFolderDropped(Event& event);
        void OnFindInFilesShowing(FindInFilesEvent& event);
        
        // Called from the context menu of a workspace folder
        void OnWorkspaceNewWorkspaceFolder(wxCommandEvent& evt);
        void OnNewProject(wxCommandEvent& evt);
        // Called from the workspace context menu
        void OnWorkspaceFolderNewFolder(wxCommandEvent& evt);
        void OnAddProjectToWorkspaceFolder(wxCommandEvent& evt);

        void OnWorkspaceFolderDelete(wxCommandEvent& evt);

        // Tree sorting
        virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
        int OnCompareItems(const FilewViewTreeItemData* a, const FilewViewTreeItemData* b);

        void ShowWorkspaceContextMenu();
        void ShowWorkspaceFolderContextMenu();
        void ShowProjectContextMenu(const wxString& projectName);
        void ShowVirtualFolderContextMenu(FilewViewTreeItemData* itemData);
        void ShowFileContextMenu();

        // internal
        void OnBuildProjectOnlyInternal(wxCommandEvent& e);
        void OnCleanProjectOnlyInternal(wxCommandEvent& e);
        void OnBuildConfigChanged(wxCommandEvent& e);

        /**
         * @brief clear the "active" marker from all the projects
         */
        void UnselectAllProject();

        /**
         * @brief return the item data for an item
         */
        FilewViewTreeItemData* ItemData(const wxTreeItemId& item) const;
    protected:
        void DoFilesEndDrag(wxTreeItemId& itemDst);
        void DoProjectsEndDrag(wxTreeItemId& itemDst);
        void DoSetItemBackgroundColour(const wxTreeItemId& item, const FolderColour::List_t& colours, const ProjectItem& projectItem);

        void ExcludeFileFromBuildUI(const wxTreeItemId& item, bool exclude);
        bool IsItemExcludedFromBuild(const wxTreeItemId& item, const wxString& configName) const;
    private:
        // Build project node
        void BuildProjectNode(const wxString& projectName);
        void DoAddChildren(const wxTreeItemId& parentItem);
        void DoBuildSubTreeIfNeeded(const wxTreeItemId& parent);

        /**
         * @brief add a workspace folder
         * @param folderPath the path, separated by "/"
         */
        wxTreeItemId AddWorkspaceFolder(const wxString& folderPath);

        int GetIconIndex(const ProjectItem& item);
        wxString GetItemPath(const wxTreeItemId& item, const wxChar& sep = ':') const;
        bool IsFileExcludedFromBuild(const wxTreeItemId& item) const;

        void DoGetProjectIconIndex(const wxString& projectName, int& iconIndex, bool& fromPlugin);
        bool DoAddNewItem(wxTreeItemId& item, const wxString& filename, const wxString& vdFullpath);
        void DoRemoveProject(const wxString& name);
        void DoSetProjectActive(wxTreeItemId& item);
        wxTreeItemId DoAddVirtualFolder(wxTreeItemId& parent, const wxString& text);
        void DoRemoveVirtualFolder(wxTreeItemId& parent);
        void DoRemoveItems();
        void DoItemActivated(wxTreeItemId& item, wxEvent& event);
        void DoAddItem(ProjectPtr proj, const FileViewItem& item);
        void DoImportFolder(ProjectPtr proj, const wxString& baseDir, const wxArrayString& all_files,
                            const wxString& filespec, bool extlessFiles);

        wxTreeItemId DoGetItemByText(const wxTreeItemId& parent, const wxString& text);

        wxTreeItemId GetSingleSelection();
        size_t GetMultiSelection(wxArrayTreeItemIds& arr);
        wxTreeItemId FindItemByPath(wxTreeItemId& parent, const wxString& projectPath, const wxString& fileName);
        DECLARE_EVENT_TABLE()
    private:
        wxArrayTreeItemIds m_draggedFiles;
        wxArrayTreeItemIds m_draggedProjects;
        std::unordered_map<wxString, wxTreeItemId> m_workspaceFolders;
        std::unordered_map<wxString, wxTreeItemId> m_projectsMap;
        std::unordered_map<wxString, wxTreeItemId> m_excludeBuildFiles;
        TreeCtrlColourHelper::Ptr_t m_colourHelper;
        wxString mLastCreateFile;
    };
}
#endif
