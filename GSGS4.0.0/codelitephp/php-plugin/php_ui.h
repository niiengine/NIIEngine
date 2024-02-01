//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: php_ui.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITEPHP_PHP_PLUGIN_PHP_UI_BASE_CLASSES_H
#define _CODELITEPHP_PHP_PLUGIN_PHP_UI_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include "php_file_layout_tree.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/dataview.h>
#include "gsgsTreeCtrl.h"
#include <wx/treebook.h>
#include <wx/panel.h>
#include <wx/imaglist.h>
#include <wx/stc/stc.h>
#include <wx/choicebk.h>
#include <wx/checkbox.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/property.h>
#include <wx/propgrid/advprops.h>
#include <wx/toolbar.h>
#include "gsgsToolBar.h"
#include <wx/gauge.h>
#include <wx/treectrl.h>
#include "my_tree_view.h"
#include <wx/aui/auibook.h>
#include "gsgsNotebook.h"
#include "gsgsTreeCtrl.h"
#include <wx/notebook.h>
#include <wx/choice.h>
#include <wx/arrstr.h>
#include <wx/simplebook.h>
#include <wx/combobox.h>
#include <wx/wizard.h>
#include <vector>
#include <wx/bannerwindow.h>
#include <wx/radiobox.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

class QuickOutlineDlgBase : public wxDialog
{
protected:
    PHPFileLayoutTree* m_treeCtrlLayout;

protected:
public:
    PHPFileLayoutTree* GetTreeCtrlLayout() { return m_treeCtrlLayout; }
    QuickOutlineDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT(""),
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400, 300),
                        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxBORDER_SUNKEN);
    virtual ~QuickOutlineDlgBase();
};

class NewPHPWorkspaceBaseDlg : public wxDialog
{
protected:
    wxStaticText* m_staticText3;
    wxTextCtrl* m_textCtrlPath;
    wxButton* m_button49;
    wxStaticText* m_staticText2;
    wxTextCtrl* m_textCtrlName;
    wxTextCtrl* m_textCtrlPreview;
    wxStdDialogButtonSizer* m_stdBtnSizer685;
    wxButton* m_button687;
    wxButton* m_button689;

protected:
    virtual void OnFolderSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnBrowse(wxCommandEvent& event) { event.Skip(); }
    virtual void OnNameUpdated(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOK(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOKUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText3() { return m_staticText3; }
    wxTextCtrl* GetTextCtrlPath() { return m_textCtrlPath; }
    wxButton* GetButton49() { return m_button49; }
    wxStaticText* GetStaticText2() { return m_staticText2; }
    wxTextCtrl* GetTextCtrlName() { return m_textCtrlName; }
    wxTextCtrl* GetTextCtrlPreview() { return m_textCtrlPreview; }
    NewPHPWorkspaceBaseDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New PHP Workspace"),
                           const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                           long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~NewPHPWorkspaceBaseDlg();
};

class NewFileDlgBase : public wxDialog
{
protected:
    wxStaticText* m_staticText4;
    wxTextCtrl* m_textCtrlName;
    wxStaticText* m_staticText5;
    wxDirPickerCtrl* m_dirPickerPath;
    wxButton* m_button5;
    wxButton* m_button6;

protected:
public:
    wxStaticText* GetStaticText4() { return m_staticText4; }
    wxTextCtrl* GetTextCtrlName() { return m_textCtrlName; }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxDirPickerCtrl* GetDirPickerPath() { return m_dirPickerPath; }
    wxButton* GetButton5() { return m_button5; }
    wxButton* GetButton6() { return m_button6; }
    NewFileDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New File"),
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                   long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~NewFileDlgBase();
};

class OpenResourceDlgBase : public wxDialog
{
protected:
    wxStaticText* m_staticText9;
    wxTextCtrl* m_textCtrlFilter;
    gsgs::ThemedListCtrl* m_dvListCtrl;

protected:
    virtual void OnKeyDown(wxKeyEvent& event) { event.Skip(); }
    virtual void OnFilterText(wxCommandEvent& event) { event.Skip(); }
    virtual void OnFilterEnter(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDVItemActivated(wxDataViewEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText9() { return m_staticText9; }
    wxTextCtrl* GetTextCtrlFilter() { return m_textCtrlFilter; }
    gsgs::ThemedListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
    OpenResourceDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open resource..."),
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400, 300),
                        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~OpenResourceDlgBase();
};

class PHPSettingsBaseDlg : public wxDialog
{
protected:
    wxTreebook* m_treebook9;
    wxPanel* m_panel11;
    wxStaticText* m_staticText9;
    wxFilePickerCtrl* m_filePickerPHPPath;
    wxStaticText* m_staticText11;
    wxTextCtrl* m_textCtrlErrorReporting;
    wxStaticText* m_staticText14;
    wxButton* m_buttonBrowseIncludePath;
    wxStyledTextCtrl* m_stcIncludePaths;
    wxPanel* m_panel15;
    wxStaticText* m_staticText13;
    wxButton* m_button15;
    wxTextCtrl* m_textCtrCClIncludePath;
    wxPanel* m_panel13;
    wxStaticText* m_staticTextHost;
    wxTextCtrl* m_textCtrlHost;
    wxStaticText* m_staticText10;
    wxTextCtrl* m_textCtrlXDebugPort;
    wxStaticText* m_staticText152;
    wxTextCtrl* m_textCtrlIdeKey;
    wxButton* m_button9;
    wxButton* m_button10;

protected:
    virtual void OnBrowseForIncludePath(wxCommandEvent& event) { event.Skip(); }
    virtual void OnAddCCPath(wxCommandEvent& event) { event.Skip(); }
    virtual void OnUpdateApplyUI(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOK(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText9() { return m_staticText9; }
    wxFilePickerCtrl* GetFilePickerPHPPath() { return m_filePickerPHPPath; }
    wxStaticText* GetStaticText11() { return m_staticText11; }
    wxTextCtrl* GetTextCtrlErrorReporting() { return m_textCtrlErrorReporting; }
    wxStaticText* GetStaticText14() { return m_staticText14; }
    wxButton* GetButtonBrowseIncludePath() { return m_buttonBrowseIncludePath; }
    wxStyledTextCtrl* GetStcIncludePaths() { return m_stcIncludePaths; }
    wxPanel* GetPanel11() { return m_panel11; }
    wxStaticText* GetStaticText13() { return m_staticText13; }
    wxButton* GetButton15() { return m_button15; }
    wxTextCtrl* GetTextCtrCClIncludePath() { return m_textCtrCClIncludePath; }
    wxPanel* GetPanel15() { return m_panel15; }
    wxStaticText* GetStaticTextHost() { return m_staticTextHost; }
    wxTextCtrl* GetTextCtrlHost() { return m_textCtrlHost; }
    wxStaticText* GetStaticText10() { return m_staticText10; }
    wxTextCtrl* GetTextCtrlXDebugPort() { return m_textCtrlXDebugPort; }
    wxStaticText* GetStaticText152() { return m_staticText152; }
    wxTextCtrl* GetTextCtrlIdeKey() { return m_textCtrlIdeKey; }
    wxPanel* GetPanel13() { return m_panel13; }
    wxTreebook* GetTreebook9() { return m_treebook9; }
    wxButton* GetButton9() { return m_button9; }
    wxButton* GetButton10() { return m_button10; }
    PHPSettingsBaseDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("PHP General Settings"),
                       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                       long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~PHPSettingsBaseDlg();
};

class PHPProjectSettingsBase : public wxDialog
{
protected:
    wxTreebook* m_treebook41;
    wxPanel* m_panel43;
    wxStaticText* m_staticText457;
    wxChoicebook* m_choicebook1;
    wxPanel* m_panel5;
    wxStaticText* m_staticText19;
    wxFilePickerCtrl* m_filePickerPHPExe;
    wxStaticText* m_staticText75;
    wxFilePickerCtrl* m_filePickerPhpIni;
    wxStaticText* m_staticText15;
    wxFilePickerCtrl* m_filePickerIndex;
    wxStaticText* m_staticText18;
    wxDirPickerCtrl* m_dirPickerWorkingDirectory;
    wxStaticText* m_staticText41;
    wxTextCtrl* m_textCtrlProgramArgs;
    wxCheckBox* m_checkBoxPauseWhenExecutionEnds;
    wxPanel* m_panel6;
    wxStaticText* m_staticText16;
    wxTextCtrl* m_textCtrlWebSiteURL;
    wxCheckBox* m_checkBoxSystemBrowser;
    wxPanel* m_panel59;
    wxStaticText* m_staticText20;
    wxButton* m_button17;
    wxTextCtrl* m_textCtrlPHPIncludePath;
    wxPanel* m_panel447;
    wxPropertyGridManager* m_pgMgrView;
    wxPGProperty* m_pgPropFileTypes;
    wxPGProperty* m_pgPropExcludeFolders;
    wxPanel* m_panel45;
    wxStaticText* m_staticText105;
    wxDataViewListCtrl* m_dvListCtrlFileMapping;
    wxPanel* m_panel47;
    wxStaticText* m_staticText13;
    wxButton* m_button15;
    wxTextCtrl* m_textCtrlCCIncludePath;
    wxButton* m_button12;
    wxButton* m_button13;
    wxButton* m_button14;

protected:
    virtual void OnPageChanged(wxChoicebookEvent& event) { event.Skip(); }
    virtual void OnPHPExecChanged(wxFileDirPickerEvent& event) { event.Skip(); }
    virtual void OnPHPIniSelected(wxFileDirPickerEvent& event) { event.Skip(); }
    virtual void OnIndexFileSelected(wxFileDirPickerEvent& event) { event.Skip(); }
    virtual void OnWorkingDirectoryChanged(wxFileDirPickerEvent& event) { event.Skip(); }
    virtual void OnUpdateApplyUI(wxCommandEvent& event) { event.Skip(); }
    virtual void OnPauseWhenExeTerminates(wxCommandEvent& event) { event.Skip(); }
    virtual void OnProjectURLChanged(wxCommandEvent& event) { event.Skip(); }
    virtual void OnUseSystemBrowser(wxCommandEvent& event) { event.Skip(); }
    virtual void OnAddIncludePath(wxCommandEvent& event) { event.Skip(); }
    virtual void OnPgmgrviewPgChanged(wxPropertyGridEvent& event) { event.Skip(); }
    virtual void OnFileMappingMenu(wxDataViewEvent& event) { event.Skip(); }
    virtual void OnFileMappingItemActivated(wxDataViewEvent& event) { event.Skip(); }
    virtual void OnAddCCPath(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOK(wxCommandEvent& event) { event.Skip(); }
    virtual void OnApply(wxCommandEvent& event) { event.Skip(); }
    virtual void OnApplyUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText457() { return m_staticText457; }
    wxStaticText* GetStaticText19() { return m_staticText19; }
    wxFilePickerCtrl* GetFilePickerPHPExe() { return m_filePickerPHPExe; }
    wxStaticText* GetStaticText75() { return m_staticText75; }
    wxFilePickerCtrl* GetFilePickerPhpIni() { return m_filePickerPhpIni; }
    wxStaticText* GetStaticText15() { return m_staticText15; }
    wxFilePickerCtrl* GetFilePickerIndex() { return m_filePickerIndex; }
    wxStaticText* GetStaticText18() { return m_staticText18; }
    wxDirPickerCtrl* GetDirPickerWorkingDirectory() { return m_dirPickerWorkingDirectory; }
    wxStaticText* GetStaticText41() { return m_staticText41; }
    wxTextCtrl* GetTextCtrlProgramArgs() { return m_textCtrlProgramArgs; }
    wxCheckBox* GetCheckBoxPauseWhenExecutionEnds() { return m_checkBoxPauseWhenExecutionEnds; }
    wxPanel* GetPanel5() { return m_panel5; }
    wxStaticText* GetStaticText16() { return m_staticText16; }
    wxTextCtrl* GetTextCtrlWebSiteURL() { return m_textCtrlWebSiteURL; }
    wxCheckBox* GetCheckBoxSystemBrowser() { return m_checkBoxSystemBrowser; }
    wxPanel* GetPanel6() { return m_panel6; }
    wxChoicebook* GetChoicebook1() { return m_choicebook1; }
    wxStaticText* GetStaticText20() { return m_staticText20; }
    wxButton* GetButton17() { return m_button17; }
    wxTextCtrl* GetTextCtrlPHPIncludePath() { return m_textCtrlPHPIncludePath; }
    wxPanel* GetPanel59() { return m_panel59; }
    wxPropertyGridManager* GetPgMgrView() { return m_pgMgrView; }
    wxPanel* GetPanel447() { return m_panel447; }
    wxPanel* GetPanel43() { return m_panel43; }
    wxStaticText* GetStaticText105() { return m_staticText105; }
    wxDataViewListCtrl* GetDvListCtrlFileMapping() { return m_dvListCtrlFileMapping; }
    wxPanel* GetPanel45() { return m_panel45; }
    wxStaticText* GetStaticText13() { return m_staticText13; }
    wxButton* GetButton15() { return m_button15; }
    wxTextCtrl* GetTextCtrlCCIncludePath() { return m_textCtrlCCIncludePath; }
    wxPanel* GetPanel47() { return m_panel47; }
    wxTreebook* GetTreebook41() { return m_treebook41; }
    wxButton* GetButton12() { return m_button12; }
    wxButton* GetButton13() { return m_button13; }
    wxButton* GetButton14() { return m_button14; }
    PHPProjectSettingsBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Project Settings"),
                           const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                           long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~PHPProjectSettingsBase();
};

class FileMappingDlgBase : public wxDialog
{
protected:
    wxStaticText* m_staticText119;
    wxDirPickerCtrl* m_dirPickerSource;
    wxStaticText* m_staticText123;
    wxTextCtrl* m_textCtrlRemote;
    wxStdDialogButtonSizer* m_stdBtnSizer111;
    wxButton* m_buttonOK;
    wxButton* m_buttonCancel;

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText119() { return m_staticText119; }
    wxDirPickerCtrl* GetDirPickerSource() { return m_dirPickerSource; }
    wxStaticText* GetStaticText123() { return m_staticText123; }
    wxTextCtrl* GetTextCtrlRemote() { return m_textCtrlRemote; }
    FileMappingDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("File Mapping"),
                       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                       long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~FileMappingDlgBase();
};

class PHPWorkspaceViewBase : public wxPanel
{
protected:
    gsgs::ToolBar* m_toolbar;
    wxGauge* m_gaugeParseProgress;
    MyTreeView* m_treeCtrlView;

protected:
    virtual void OnMenu(wxTreeEvent& event) { event.Skip(); }
    virtual void OnItemActivated(wxTreeEvent& event) { event.Skip(); }

public:
    gsgs::ToolBar* GetToolbar() { return m_toolbar; }
    wxGauge* GetGaugeParseProgress() { return m_gaugeParseProgress; }
    MyTreeView* GetTreeCtrlView() { return m_treeCtrlView; }
    PHPWorkspaceViewBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
    virtual ~PHPWorkspaceViewBase();
};

class PHPDebugPaneBase : public wxPanel
{
protected:
    Notebook* m_auiBook;
    wxPanel* m_panel140;
    gsgs::ThemedListCtrl* m_dvListCtrlStackTrace;
    wxPanel* m_panel142;
    wxToolBar* m_tbBreakpoints;
    gsgs::ThemedListCtrl* m_dvListCtrlBreakpoints;

protected:
    virtual void OnCallStackItemActivated(wxDataViewEvent& event) { event.Skip(); }
    virtual void OnCallStackMenu(wxDataViewEvent& event) { event.Skip(); }
    virtual void OnBreakpointItemActivated(wxDataViewEvent& event) { event.Skip(); }

public:
    gsgs::ThemedListCtrl* GetDvListCtrlStackTrace() { return m_dvListCtrlStackTrace; }
    wxPanel* GetPanel140() { return m_panel140; }
    wxToolBar* GetTbBreakpoints() { return m_tbBreakpoints; }
    gsgs::ThemedListCtrl* GetDvListCtrlBreakpoints() { return m_dvListCtrlBreakpoints; }
    wxPanel* GetPanel142() { return m_panel142; }
    Notebook* GetAuiBook() { return m_auiBook; }
    PHPDebugPaneBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxSize(300, 300), long style = wxTAB_TRAVERSAL);
    virtual ~PHPDebugPaneBase();
};

class LocalsViewBase : public wxPanel
{
protected:
    gsgs::DefaultTreeCtrl* m_tree;

protected:
public:
    gsgs::DefaultTreeCtrl* GetTree() { return m_tree; }
    LocalsViewBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
    virtual ~LocalsViewBase();
};

class EvalPaneBase : public wxPanel
{
protected:
    wxNotebook* m_notebook257;
    wxPanel* m_panel259;
    wxStyledTextCtrl* m_stcOutput;
    wxTextCtrl* m_textCtrlExpression;
    wxButton* m_buttonSend;
    wxPanel* m_panel261;
    wxStyledTextCtrl* m_stcOutputXDebug;
    wxTextCtrl* m_textCtrlExpressionXdebug;
    wxButton* m_buttonSendXdebug;

protected:
    virtual void OnEnter(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSend(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSendUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnSendXDebugCommand(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSendXDebugCommandUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStyledTextCtrl* GetStcOutput() { return m_stcOutput; }
    wxTextCtrl* GetTextCtrlExpression() { return m_textCtrlExpression; }
    wxButton* GetButtonSend() { return m_buttonSend; }
    wxPanel* GetPanel259() { return m_panel259; }
    wxStyledTextCtrl* GetStcOutputXDebug() { return m_stcOutputXDebug; }
    wxTextCtrl* GetTextCtrlExpressionXdebug() { return m_textCtrlExpressionXdebug; }
    wxButton* GetButtonSendXdebug() { return m_buttonSendXdebug; }
    wxPanel* GetPanel261() { return m_panel261; }
    wxNotebook* GetNotebook257() { return m_notebook257; }
    EvalPaneBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL);
    virtual ~EvalPaneBase();
};

class PHPDebugStartDlgBase : public wxDialog
{
protected:
    wxChoice* m_choice;
    wxSimplebook* m_simpleBook;
    wxPanel* m_panelDebugURL;
    wxStaticText* m_staticText377;
    wxComboBox* m_comboBoxURL;
    wxPanel* m_panelCommandLine;
    wxStaticText* m_staticText383;
    wxTextCtrl* m_textCtrlScriptToDebug;
    wxCheckBox* m_checkBoxDebugActiveEditor;
    wxStdDialogButtonSizer* m_stdBtnSizer361;
    wxButton* m_button363;
    wxButton* m_button365;

protected:
    virtual void OnDebugMethodChanged(wxCommandEvent& event) { event.Skip(); }
    virtual void OnScriptToDebugUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnUseActiveEditor(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOkUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxChoice* GetChoice() { return m_choice; }
    wxStaticText* GetStaticText377() { return m_staticText377; }
    wxComboBox* GetComboBoxURL() { return m_comboBoxURL; }
    wxPanel* GetPanelDebugURL() { return m_panelDebugURL; }
    wxStaticText* GetStaticText383() { return m_staticText383; }
    wxTextCtrl* GetTextCtrlScriptToDebug() { return m_textCtrlScriptToDebug; }
    wxCheckBox* GetCheckBoxDebugActiveEditor() { return m_checkBoxDebugActiveEditor; }
    wxPanel* GetPanelCommandLine() { return m_panelCommandLine; }
    wxSimplebook* GetSimpleBook() { return m_simpleBook; }
    PHPDebugStartDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("PHP Run / Debug"),
                         const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                         long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~PHPDebugStartDlgBase();
};

class NewPHPProjectWizardBase : public wxWizard
{
protected:
    std::vector<wxWizardPageSimple*> m_pages;

    wxWizardPageSimple* m_wizardPageCreateMethod;
    wxBannerWindow* m_banner495;
    wxRadioBox* m_radioBoxCreateMethod;
    wxWizardPageSimple* m_wizardPageProjectDetails;
    wxBannerWindow* m_banner517;
    wxStaticText* m_staticText521;
    wxTextCtrl* m_textCtrlName;
    wxStaticText* m_staticText525;
    wxDirPickerCtrl* m_dirPickerPath;
    wxCheckBox* m_checkBoxSeparateFolder;
    wxStaticText* m_staticText535;
    wxTextCtrl* m_textCtrlPreview;
    wxWizardPageSimple* m_wizardPageFinalize;
    wxBannerWindow* m_banner4951;
    wxStaticText* m_staticText3173;
    wxChoice* m_choiceProjectType;
    wxStaticText* m_staticText3215;
    wxFilePickerCtrl* m_filePickerPhpExe;
    wxWizardPageSimple* m_wizardPage634;
    wxBannerWindow* m_banner49511;
    wxStaticText* m_staticText653;
    wxButton* m_button651;
    wxTextCtrl* m_textCtrlCCPaths;

protected:
    virtual void OnFinish(wxWizardEvent& event) { event.Skip(); }
    virtual void OnPageChanging(wxWizardEvent& event) { event.Skip(); }
    virtual void OnNameUpdated(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDirSelected(wxFileDirPickerEvent& event) { event.Skip(); }
    virtual void OnCheckSeparateFolder(wxCommandEvent& event) { event.Skip(); }
    virtual void OnBrowseForCCFolder(wxCommandEvent& event) { event.Skip(); }

public:
    wxBannerWindow* GetBanner495() { return m_banner495; }
    wxRadioBox* GetRadioBoxCreateMethod() { return m_radioBoxCreateMethod; }
    wxWizardPageSimple* GetWizardPageCreateMethod() { return m_wizardPageCreateMethod; }
    wxBannerWindow* GetBanner517() { return m_banner517; }
    wxStaticText* GetStaticText521() { return m_staticText521; }
    wxTextCtrl* GetTextCtrlName() { return m_textCtrlName; }
    wxStaticText* GetStaticText525() { return m_staticText525; }
    wxDirPickerCtrl* GetDirPickerPath() { return m_dirPickerPath; }
    wxCheckBox* GetCheckBoxSeparateFolder() { return m_checkBoxSeparateFolder; }
    wxStaticText* GetStaticText535() { return m_staticText535; }
    wxTextCtrl* GetTextCtrlPreview() { return m_textCtrlPreview; }
    wxWizardPageSimple* GetWizardPageProjectDetails() { return m_wizardPageProjectDetails; }
    wxBannerWindow* GetBanner4951() { return m_banner4951; }
    wxStaticText* GetStaticText3173() { return m_staticText3173; }
    wxChoice* GetChoiceProjectType() { return m_choiceProjectType; }
    wxStaticText* GetStaticText3215() { return m_staticText3215; }
    wxFilePickerCtrl* GetFilePickerPhpExe() { return m_filePickerPhpExe; }
    wxWizardPageSimple* GetWizardPageFinalize() { return m_wizardPageFinalize; }
    wxBannerWindow* GetBanner49511() { return m_banner49511; }
    wxStaticText* GetStaticText653() { return m_staticText653; }
    wxButton* GetButton651() { return m_button651; }
    wxTextCtrl* GetTextCtrlCCPaths() { return m_textCtrlCCPaths; }
    wxWizardPageSimple* GetWizardPage634() { return m_wizardPage634; }
    NewPHPProjectWizardBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New PHP Project"),
                            const wxBitmap& bmp = wxNullBitmap, const wxPoint& pos = wxDefaultPosition,
                            long style = wxDEFAULT_DIALOG_STYLE);
    wxWizardPageSimple* GetFirstPage() const
    {
        if(!m_pages.empty()) return m_pages.at(0);
        return NULL;
    }
    virtual ~NewPHPProjectWizardBase();
};

class PHPXDebugSetupWizardBase : public wxWizard
{
protected:
    std::vector<wxWizardPageSimple*> m_pages;

    wxWizardPageSimple* m_wizardPagePort;
    wxBannerWindow* m_bannerPort;
    wxStaticText* m_staticText585;
    wxTextCtrl* m_textCtrlPort;
    wxWizardPageSimple* m_wizardPageHost;
    wxBannerWindow* m_bannerHost;
    wxStaticText* m_staticText5853;
    wxTextCtrl* m_textCtrlIP;
    wxWizardPageSimple* m_wizardPageIDEKey;
    wxBannerWindow* m_bannerSessionID;
    wxStaticText* m_staticText58537;
    wxTextCtrl* m_textCtrlKey;
    wxWizardPageSimple* m_wizardPagePHP;
    wxBannerWindow* m_bannerSessionID10;
    wxStaticText* m_staticText625;
    wxTextCtrl* m_textCtrlPHPIni;

protected:
    virtual void OnPageChanging(wxWizardEvent& event) { event.Skip(); }
    virtual void OnFinished(wxWizardEvent& event) { event.Skip(); }

public:
    wxBannerWindow* GetBannerPort() { return m_bannerPort; }
    wxStaticText* GetStaticText585() { return m_staticText585; }
    wxTextCtrl* GetTextCtrlPort() { return m_textCtrlPort; }
    wxWizardPageSimple* GetWizardPagePort() { return m_wizardPagePort; }
    wxBannerWindow* GetBannerHost() { return m_bannerHost; }
    wxStaticText* GetStaticText5853() { return m_staticText5853; }
    wxTextCtrl* GetTextCtrlIP() { return m_textCtrlIP; }
    wxWizardPageSimple* GetWizardPageHost() { return m_wizardPageHost; }
    wxBannerWindow* GetBannerSessionID() { return m_bannerSessionID; }
    wxStaticText* GetStaticText58537() { return m_staticText58537; }
    wxTextCtrl* GetTextCtrlKey() { return m_textCtrlKey; }
    wxWizardPageSimple* GetWizardPageIDEKey() { return m_wizardPageIDEKey; }
    wxBannerWindow* GetBannerSessionID10() { return m_bannerSessionID10; }
    wxStaticText* GetStaticText625() { return m_staticText625; }
    wxTextCtrl* GetTextCtrlPHPIni() { return m_textCtrlPHPIni; }
    wxWizardPageSimple* GetWizardPagePHP() { return m_wizardPagePHP; }
    PHPXDebugSetupWizardBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("XDebug Setup"),
                             const wxBitmap& bmp = wxNullBitmap, const wxPoint& pos = wxDefaultPosition,
                             long style = wxDEFAULT_DIALOG_STYLE);
    wxWizardPageSimple* GetFirstPage() const
    {
        if(!m_pages.empty()) return m_pages.at(0);
        return NULL;
    }
    virtual ~PHPXDebugSetupWizardBase();
};

class PHPSettersGettersDialogBase : public wxDialog
{
protected:
    wxDataViewListCtrl* m_dvListCtrlFunctions;
    wxCheckBox* m_checkBoxLowercase;
    wxCheckBox* m_checkBoxPrefixGetter;
    wxCheckBox* m_checkBoxReurnThis;
    wxStdDialogButtonSizer* m_stdBtnSizer659;
    wxButton* m_buttonOK;
    wxButton* m_buttonCancel;

protected:
public:
    wxDataViewListCtrl* GetDvListCtrlFunctions() { return m_dvListCtrlFunctions; }
    wxCheckBox* GetCheckBoxLowercase() { return m_checkBoxLowercase; }
    wxCheckBox* GetCheckBoxPrefixGetter() { return m_checkBoxPrefixGetter; }
    wxCheckBox* GetCheckBoxReurnThis() { return m_checkBoxReurnThis; }
    PHPSettersGettersDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY,
                                const wxString& title = _("Generate Setters / Getters"),
                                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~PHPSettersGettersDialogBase();
};

#endif