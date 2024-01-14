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

#ifndef gsgsTREECTRLPANEL_H
#define gsgsTREECTRLPANEL_H

#include "gsgsImageManager.h"
#include "gsgsFileViwerTreeCtrl.h"
#include "gsgsStyleManager.h"
#include "gsgsConfig.h"
#include "wxcrafter_plugin.h"
#include "imanager.h"

namespace gsgs
{
    class TreeCtrlPanelDefaultPage;

    class _gsgsAPI TreeCtrlPanel : public wxPanel
    {
    public:
        enum {
            kShowHiddenFiles = (1 << 0),
            kShowHiddenFolders = (1 << 1),
            kLinkToEditor = (1 << 2),
            kShowRootFullPath = (1 << 3),
        };
    public:
        TreeCtrlPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~TreeCtrlPanel();

        inline FileViewerTreeCtrl* GetTreeCtrl() { return m_treeCtrl; }

        /**
         * @brief getter for the toolbar
         */
        inline ToolBar* GetToolBar()                    { return m_toolbar; }

        /**
         * @brief set the tree options
         */
        inline void SetOptions(int options)             { m_options = options; }

        /**
         * @brief set the new file template (default is "Untitled.txt")
         */
        void SetNewFileTemplate(const wxString& newfile, size_t charsToHighlight);

        inline void SetViewName(const wxString& name)   { m_viewName = name; }
        inline const wxString & GetViewName() const     { return m_viewName; }

        /**
         * @brief return the configuration tool used for storing information about
         * this tree. Override it to provide a custom configuration tool
         */
        inline void SetConfig(Config * cfg)             { m_config = cfg; }

        inline Config* GetConfig() const                { return m_config; }

        /**
         * @brief clear the view (i.e. close all top level folders)
         */
        void Clear();

        /**
         * @brief add top level folder
         */
        void AddFolder(const wxString& path);

        /**
         * @brief return an info about the selected items in the tree
         * @return
         */
        TreeItemInfo GetSelectedItemInfo();

        /**
         * @brief return 2 arrays of the selected items
         * @param folders [output]
         * @param files [output]
         */
        void GetSelections(wxArrayString& folders, wxArrayString& files);

        /**
         * @brief select a given filename in the tree. Expand the tree if needed
         */
        bool ExpandToFile(const wxFileName& filename);

        /**
         * @brief return true if a folder is opened in this view
         */
        bool IsFolderOpened() const;
        
        /**
         * @brief refresh the entire tree, unconditionally 
         */
        void RefreshTree();
    protected:
        void ToggleView();
        void RefreshNonTopLevelFolder(const wxTreeItemId& item);
        void UpdateItemDeleted(const wxTreeItemId& item);
        void GetTopLevelFolders(wxArrayString& paths, wxArrayTreeItemIds& items) const;

        /**
         * @brief ensure that item is selected (single selection)
         */
        void SelectItem(const wxTreeItemId& item);
        /**
         * @brief return list of selected files and folders. In addition return the
         * tree ctrl items. You can always assume that the folders and the folderItems are of the same
         * size. Same for the file arrays
         */
        void GetSelections(wxArrayString& folders, wxArrayTreeItemIds& folderItems, wxArrayString& files,
                           wxArrayTreeItemIds& fileItems);

        // Make the event handler functions virtual
        // so any subclass could override them
        virtual void OnActiveEditorChanged(wxCommandEvent& event);
        virtual void OnFindInFilesShowing(FindInFilesEvent& event);
        virtual void OnInitDone(wxCommandEvent& event);
        virtual void OnContextMenu(wxTreeEvent& event);
        virtual void OnItemActivated(wxTreeEvent& event);
        virtual void OnItemExpanding(wxTreeEvent& event);
        virtual void OnCloseFolder(wxCommandEvent& event);
        virtual void OnNewFolder(wxCommandEvent& event);
        virtual void OnNewFile(wxCommandEvent& event);
        virtual void OnOpenFile(wxCommandEvent& event);
        virtual void OnOpenWithDefaultApplication(wxCommandEvent& event);
        virtual void OnRenameFile(wxCommandEvent& event);
        virtual void OnDeleteSelections(wxCommandEvent& event);
        virtual void OnRenameFolder(wxCommandEvent& event);
        virtual void OnFindInFilesFolder(wxCommandEvent& event);
        virtual void OnOpenContainingFolder(wxCommandEvent& event);
        virtual void OnOpenShellFolder(wxCommandEvent& event);
        virtual void OnFolderDropped(Event& event);
        virtual void OnRefresh(wxCommandEvent& event);
        void OnOpenFolder(wxCommandEvent& event);
        // Helpers
        void DoExpandItem(const wxTreeItemId& parent, bool expand);
        void DoRenameItem(const wxTreeItemId& item, const wxString& oldname, const wxString& newname);

        bool IsTopLevelFolder(const wxTreeItemId& item);

        TreeCtrlData* GetItemData(const wxTreeItemId& item) const;
        wxTreeItemId DoAddFolder(const wxTreeItemId& parent, const wxString& path);
        wxTreeItemId DoAddFile(const wxTreeItemId& parent, const wxString& path);
        void DoCloseFolder(const wxTreeItemId& item);
    protected:
        virtual void OnLinkEditor(wxCommandEvent& event);
        virtual void OnLinkEditorUI(wxUpdateUIEvent& event);
    protected:
        ImageManager* m_bmpLoader;
        Config* m_config;
        ToolBar* m_toolbar;
        wxString m_newfileTemplate;
        wxString m_viewName;
        TreeCtrlPanelDefaultPage* m_defaultView;
        FileViewerTreeCtrl* m_treeCtrl;
        size_t m_newfileTemplateHighlightLen;
        int m_options;
    };
}
#endif
