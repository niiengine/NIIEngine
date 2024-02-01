#ifndef __wxcrafter_plugin__
#define __wxcrafter_plugin__

#include "NewFormWizard.h"
#include "gsgsNotebook.h"
#include "gsgsEvent.h" // gsgs::DebugEvent
#include "import_dlg.h"
#include "main.h"
#include "plugin.h"
#include "wxcNetworkManager.h"

class wxcNetworkThread;
class EventsEditorPane;
class GUICraftMainPanel;
class wxcTreeView;

struct GeneratedClass {
    wxString classname;
    wxFileName derivedSource;
    wxFileName derivedHeader;

    void Clear()
    {
        classname.Clear();
        derivedSource.Clear();
        derivedHeader.Clear();
    }
};

class wxCrafterPlugin : public Plugin
{
    GUICraftMainPanel* m_mainPanel;
    wxcTreeView* m_treeView;
    wxMenuItem* m_separatorItem;
    GeneratedClass m_generatedClassInfo;
    bool m_allEditorsClosing;
    bool m_addFileMenu;
    bool m_useFrame;
    MainFrame* m_mainFrame;
    bool m_serverMode;
    wxcNetworkThread* m_networkThread;
    wxcNetworkManager m_netManager;
    wxFileName m_selectedFile;

protected:
    wxMenu* DoCreateFolderMenu();
    wxMenu* DoProjectMenu();
    void DoUpdateDerivedClassEventHandlers();
    bool DoReadFileContentAndPrompt(const wxFileName& fn, wxString& content, IEditor** editor);
    void DoWriteFileContent(const wxFileName& fn, const wxString& content, IEditor* editor);
    void DoGenerateCode(const NewFormDetails& fd);
    /**
     * @brief return true if the tab was created, false otherwise.
     * Note that this function only return true if the tab was allocated
     * by this function
     */
    bool DoShowDesigner(bool createIfNotExist = true);
    bool DoCreateVirtualFolder(const wxString& vdFullPath);
    void DoSelectWorkspaceTab();
    void DoLoadAfterImport(ImportDlg::ImportFileData& data);
    void DoImportFB(const wxString& filename = "");
    void DoInitDone(wxObject* obj = NULL);
    void DoLoadWxcProject(const wxFileName& filename);
    void UpdateFileNameInStatusBar();

protected:
    // Event handlers
    void OnInitDone(wxCommandEvent& e);
    void OnShowDesigner(wxCommandEvent& e);
    void OnReGenerateForProject(wxCommandEvent& e);
    void OnDesignerDelete(wxCommandEvent& e);
    void OnBitmapCodeGenerationCompleted(wxCommandEvent& e);
    void OnSourceFilesParsingDone(wxCommandEvent& e);
    void OnNewForm(wxCommandEvent& e);
    void OnOpenFile(gsgs::Event& e);
    void OnPageClosing(wxNotifyEvent& e);
    void OnProjectModified(wxCommandEvent& e);
    void OnProjectSynched(wxCommandEvent& e);
    void OnPageChanged(wxCommandEvent& e);
    void OnWorkspaceTabSelected(wxBookCtrlEvent& e);
    void OnDesignerItemSelected(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnBuildStarting(wxCommandEvent& e);
    void OnAllEditorsClosing(wxCommandEvent& e);
    void OnAllEditorsClosed(wxCommandEvent& e);
    void OnDebugStarting(gsgs::DebugEvent& e);
    void OnSave(wxCommandEvent& e);
    void OnSaveAll(gsgs::Event& e);
    void OnSaveProject(wxCommandEvent& e);
    void OnSaveProjectUI(wxUpdateUIEvent& e);
    void OnOpenProject(wxCommandEvent& e);
    void OnSettings(wxCommandEvent& e);
    void OnOpenProjectUI(wxUpdateUIEvent& e);
    void OnCloseProject(wxCommandEvent& e);
    void OnCloseProjectUI(wxUpdateUIEvent& e);
    void OnImportwxFBProject(wxCommandEvent& e);
    void OnImportwxSmith(wxCommandEvent& e);
    void OnImportXRC(wxCommandEvent& e);
    void OnDefineCustomControls(wxCommandEvent& e);
    void OnEditCustomControls(wxCommandEvent& e);
    void OnDeleteCustomControls(wxCommandEvent& e);
    void OnAbout(wxCommandEvent& e);
    void OnProjectLoaded(wxCommandEvent& e);
    void OnFileContextMenu(ContextMenuEvent& event);
    void OnVirtualFolderContextMenu(ContextMenuEvent& event);
    void OnProjectContextMenu(ContextMenuEvent& event);
    void OnImportFBProject(wxCommandEvent& event);
    void OnOpenWxcpProject(wxCommandEvent& event);
    void OnImportwxSmithProject(wxCommandEvent& event);
    void OnToggleView(gsgs::Event& event);
    void OnSaveUI(gsgs::Event& event);

public:
    wxCrafterPlugin(IManager* manager, bool serverMode);
    ~wxCrafterPlugin();

    MainFrame* GetMainFrame() const { return m_mainFrame; }

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();

    IManager* GetManager() { return m_mgr; }
    bool IsTabMode() const { return !m_mainFrame; }

    bool IsMainViewActive();
};

#endif //__wxcrafter_plugin__