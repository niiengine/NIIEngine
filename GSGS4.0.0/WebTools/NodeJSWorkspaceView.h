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

#ifndef NODEJSWORKSPACEVIEW_H
#define NODEJSWORKSPACEVIEW_H

#include "gsgsTreeCtrlPanel.h"
#include "gsgsEvent.h"
#include "NodeJSDebuggerDlg.h"
#include "TerminalEmulator.h"
#include "gsgsFileUtil.h"

class NodeJSWorkspaceView : public gsgs::TreeCtrlPanel
{
    gsgs::Config m_config;

protected:
    void OnContextMenu(ContextMenuEvent& event);
    void OnContextMenuFile(ContextMenuEvent& event);
    void OnFolderDropped(gsgs::Event& event);
    void OnShowHiddenFiles(wxCommandEvent& event);
    void OnCloseWorkspace(wxCommandEvent& event);
    void OnFileSystemUpdated(gsgs::FileEvent& event);
    void OnFindInFilesDismissed(gsgs::FindInFilesEvent& event);
    void OnFindInFilesShowing(gsgs::FindInFilesEvent& event);
    void OnOpenPackageJsonFile(wxCommandEvent& event);
    void OnProjectDebug(wxCommandEvent& event);
    void OnProjectRun(wxCommandEvent& event);
    void OnNpmInit(wxCommandEvent& event);
    void OnNpmInstall(wxCommandEvent& event);
    void OnTerminalClosed(gsgs::ProcessEvent& event);
    void DoExecuteProject(NodeJSDebuggerDlg::eDialogType type);
    void OnItemExpanding(wxTreeEvent& event);
    /**
     * @brief return the selected project path
     * A "selected project" means the entry which is highlighted on the tree
     * view. If no project is selected, return an empty string
     */
    bool GetSelectProjectPath(wxString& path, wxTreeItemId& item);

public:
    NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~NodeJSWorkspaceView();

    void RebuildTree();
    void ShowHiddenFiles(bool show);
};

#endif // NODEJSWORKSPACEVIEW_H
