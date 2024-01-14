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
#ifndef CommonController_h
#define CommonController_h

#include "gsgsPreInclude.h"
#include "gsgsWindowManager.h"
#include "gsgsBreakpointManager.h"
#include "gsgsDebugPane.h"
#include "gsgsToolsetManager.h"
#include "gsgsDebuggerManager.h"
#include "gsgsOptionsConfig.h"
#include "project_settings_base_dlg.h"
#include "compilersfoundmodel.h"
#include "gsgsLexerStyle.h"
#include "gsgsPluginManagerDlg.h"
#include "gsgsFilePicker.h"
#include "gsgsSerializer.h"
#include "findreplacedlg.h"
#include "gsgsBuildConfig.h"
#include "gsgsSearchWorker.h"
#include "gsgsImageManager.h"
#include "gsgsSessionManager.h"
#include "debuggerobserver.h"
#include "NewProjectDialog.h"
#include "gsgsTagManager.h"
#include "cpptoken.h"
#include "gsgsTag.h"
#include "gsgsEvent.h"
#include "debugger.h"
#include "project.h"
#include "foldermodel.h"
#include <wx/checkbox.h>
#include <wx/propgrid/property.h>
#include <wx/persist/window.h>
#include <wx/timer.h>
#include <wx/hyperlink.h>
#include "stalefilesmodel.h"
#include "assignedfilesmodel.h"

class BreakpointsListctrl : public ThemedListCtrl
{
    enum column_ids {
        col_id,
        col_type,
        col_enabled,
        col_file,
        col_lineno,
        col_functionname,
        col_at,
        col_memory,
        col_what,
        col_ignorecount,
        col_extras
    };

public:
    BreakpointsListctrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDV_ROW_LINES)
        : ThemedListCtrl(parent, id, pos, size, style)
    {
        // Disable any kind of sorting as we want the item in the order we insert them
        SetSortFunction(nullptr);
        const wxString column_headers[] = { _("ID"), _("Type"),   _("Enabled"), _("File"),    _("Line"),  _("Function"),
                                            _("At"), _("Memory"), _("What"),    _("Ignored"), _("Extras") };
        for (int n = col_id; n <= col_extras; ++n) {
            AddHeader(column_headers[n]);
        }
    }

    virtual ~BreakpointsListctrl() {}

    void Initialise(std::vector<gsgs::BreakpointInfo>& bps);

    int GetLinenoColumn()               { return col_lineno; }

    int GetFileColumn()                 { return col_file; }
};

class AddIncludeFileDlg : public wxDialog
{
public:
    /** Constructor */
    AddIncludeFileDlg(wxWindow* parent, const wxString& fullpath, const wxString& text, int lineNo, wxWindowID id = wxID_ANY,
                          const wxString& title = _("Add a missing header file"),
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                          long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~AddIncludeFileDlg();
    const wxString& GetLineToAdd() const { return m_lineToAdd; }
    int GetLine() const { return m_line; }
    gsgs::ToolBar* GetToolbar() { return m_toolbar; }
    wxStyledTextCtrl* GetTextCtrlPreview() { return m_textCtrlPreview; }
    wxStaticText* GetStaticText21() { return m_staticText21; }
    wxStaticText* GetStaticTextPreview() { return m_staticTextPreview; }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxTextCtrl* GetTextCtrlFullPath() { return m_textCtrlFullPath; }
    wxPanel* GetMainPanel() { return m_mainPanel; }
protected:

    void SetAndMarkLine();
    void OnTextUpdated(wxCommandEvent& e);
    void OnButtonDown(wxCommandEvent& event);
    void OnButtonUp(wxCommandEvent& event);
    void OnButtonOK(wxCommandEvent& e);
    void OnClearCachedPaths(wxCommandEvent& e);
    void UpdateLineToAdd();

    void OnPreviewKeyDown(wxKeyEvent& event);
    void OnIdle(wxIdleEvent& event);
protected:
    wxString m_fullpath;
    wxString m_text;
    int m_line;
    static wxArrayString m_includePath;
    wxString m_lineToAdd;
    gsgs::ToolBar* m_toolbar;
    wxPanel* m_mainPanel;
    wxStyledTextCtrl* m_textCtrlPreview;
    wxStaticText* m_staticText21;
    wxStaticText* m_staticTextPreview;
    wxStaticText* m_staticText5;
    wxTextCtrl* m_textCtrlFullPath;
    DialogButtonSizer* m_stdBtnSizer23;
    gsgs::Button* m_button25;
    gsgs::Button* m_button27;
};

class AttachDbgProcDlg : public wxDialog
{
public:
    AttachDbgProcDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Attach debugger to process:"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~AttachDbgProcDlg();

    wxString GetProcessId() const;
    wxString GetExeName() const;
    wxString GetDebugger() const { return m_choiceDebugger->GetStringSelection(); }

    wxStaticText* GetStaticText3() { return m_staticText3; }
    wxChoice* GetChoiceDebugger() { return m_choiceDebugger; }
    wxStaticText* GetStaticText31() { return m_staticText31; }
    wxTextCtrl* GetTextCtrlFilter() { return m_textCtrlFilter; }
    wxStaticText* GetStaticText4() { return m_staticText4; }
    wxDataViewListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
    gsgs::Button* GetButtonOk() { return m_buttonOk; }
    gsgs::Button* GetButton2() { return m_button2; }
    gsgs::Button* GetButtonRefresh() { return m_buttonRefresh; }
    
    void RefreshProcessesList(wxString filter);
protected:
    // events
    virtual void OnBtnAttachUI(wxUpdateUIEvent& event);
    virtual void OnFilter(wxCommandEvent& event);
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
private:
    wxStaticText* m_staticText3;
    wxChoice* m_choiceDebugger;
    wxStaticText* m_staticText31;
    wxTextCtrl* m_textCtrlFilter;
    wxStaticText* m_staticText4;
    wxDataViewListCtrl* m_dvListCtrl;
    gsgs::Button* m_buttonOk;
    gsgs::Button* m_button2;
    gsgs::Button* m_buttonRefresh;
};

#define QUERY_NUM_CHILDS 500
#define LIST_WATCH_CHILDS 501

class WatchDropTarget : public wxTextDropTarget
{
public:
    WatchDropTarget(WatchesTable* parent)
        : m_parent(parent)
    {
    }
    virtual ~WatchDropTarget() {}
    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text);
protected:
    WatchesTable* m_parent;
};

// Since a breakpoint will always have an internal id, but a valid debugger one only when the debugger's running, store
// both here
struct bpd_IDs {
    double debugger_id;
    double internal_id;
    bpd_IDs(const gsgs::BreakpointInfo& bp)
    {
        debugger_id = bp.debugger_id;
        internal_id = bp.internal_id;
    }
    // If the debugger is running, debugger_id will (should) be >0. Otherwise use the internal_id
    double GetBestId() { return debugger_id == -1 ? internal_id : debugger_id; }
    wxString GetIdAsString() // Internal IDs start at FIRST_INTERNAL_ID + 1, == 10001
    {
        double id = (GetBestId() > FIRST_INTERNAL_ID ? GetBestId() - FIRST_INTERNAL_ID : GetBestId());
        wxString idstr;
        idstr << id;
        return idstr;
    }
};

class BreakpointDlg : public wxPanel
{
public:
    BreakpointDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
    ~BreakpointDlg();
    void Initialize();
        BreakpointsListctrl* GetDvListCtrlBreakpoints() { return m_dvListCtrlBreakpoints; }
    DefaultButton* GetButtonAdd() { return m_buttonAdd; }
    DefaultButton* GetButtonEdit() { return m_buttonEdit; }
    DefaultButton* GetButtonDelete() { return m_buttonDelete; }
    DefaultButton* GetButtonDeleteAll() { return m_buttonDeleteAll; }
    DefaultButton* GetButtonApplyPending() { return m_buttonApplyPending; }
protected:
    void OnBreakpointActivated(wxDataViewEvent& event);
    void OnContextMenu(wxDataViewEvent& event);
    void OnAdd(wxCommandEvent& e);
    void OnEdit(wxCommandEvent& e);
    void OnDelete(wxCommandEvent& e);
    void OnDeleteAll(wxCommandEvent& e);
    void OnApplyPending(wxCommandEvent& e);
protected:
    std::vector<bpd_IDs> m_ids;
    BreakpointsListctrl* m_dvListCtrlBreakpoints;
    DefaultButton* m_buttonAdd;
    DefaultButton* m_buttonEdit;
    DefaultButton* m_buttonDelete;
    DefaultButton* m_buttonDeleteAll;
    DefaultButton* m_buttonApplyPending;
};

/** Implementing BreakpointPropertiesDlg */
class BreakptPropertiesDlg : public wxDialog
{
public:
    BreakptPropertiesDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                                const wxString& title = _("Breakpoint and Watchpoint Properties"),
                                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    ~BreakptPropertiesDlg();

    /**
     * \brief Insert the data from an existing breakpoint into the dialog fields
     * \param parent
     * \param bp
     */
    void EnterBPData(const gsgs::BreakpointInfo& bp);
    
    wxStaticText* GetStaticText9() { return m_staticText9; }
    wxStaticText* GetStaticText51() { return m_staticText51; }
    wxCheckBox* GetCheckLineno() { return m_checkLineno; }
    wxTextCtrl* GetTextLineno() { return m_textLineno; }
    wxCheckBox* GetCheckBreakFunction() { return m_checkBreakFunction; }
    wxTextCtrl* GetTextFunctionname() { return m_textFunctionname; }
    wxCheckBox* GetCheckRegex() { return m_checkRegex; }
    wxCheckBox* GetCheckBreakMemory() { return m_checkBreakMemory; }
    wxTextCtrl* GetTextBreakMemory() { return m_textBreakMemory; }
    wxStaticText* GetStaticText6() { return m_staticText6; }
    wxTextCtrl* GetTextFilename() { return m_textFilename; }
    gsgs::Button* GetButtonBrowse() { return m_buttonBrowse; }
    wxStaticLine* GetStaticline5() { return m_staticline5; }
    wxPanel* GetPanel1() { return m_panel1; }
    wxRadioBox* GetRadioWatchtype() { return m_radioWatchtype; }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxTextCtrl* GetTextWatchExpression() { return m_textWatchExpression; }
    wxPanel* GetPanel2() { return m_panel2; }
    wxChoicebook* GetChoicebook() { return m_choicebook; }
    wxStaticText* GetStaticText1() { return m_staticText1; }
    wxTextCtrl* GetTextCond() { return m_textCond; }
    wxStaticText* GetStaticText11() { return m_staticText11; }
    wxTextCtrl* GetTextCommands() { return m_textCommands; }
    wxCheckBox* GetCheckDisable() { return m_checkDisable; }
    wxCheckBox* GetCheckTemp() { return m_checkTemp; }
    wxStaticText* GetStaticText7() { return m_staticText7; }
    wxSpinCtrl* GetSpinIgnore() { return m_spinIgnore; }
    gsgs::Button* GetButton6() { return m_button6; }
    gsgs::Button* GetButton7() { return m_button7; }
protected:
    void EndModal(int retCode);
    // Handlers for BreakptPropertiesDlg events.
    void OnCheckBreakLineno(wxCommandEvent& event);
    void OnCheckBreakFunction(wxCommandEvent& event);
    void OnCheckBreakMemory(wxCommandEvent& event);
    void OnBrowse(wxCommandEvent& event);
    void OnCheckBreakLinenoUI(wxUpdateUIEvent& event);
    void OnCheckBreakFunctionUI(wxUpdateUIEvent& event);
    void OnCheckBreakMemoryUI(wxUpdateUIEvent& event);
    void OnPageChanging(wxChoicebookEvent& event);
    void OnPageChanged(wxChoicebookEvent& event);

    bool its_a_breakpt; // Holds whether it's a break or a watch. Used for UpdateUI
    enum whichbreakchk { wbc_line, wbc_function, wbc_memory };
    enum whichbreakchk
        whichBreakcheck; // Holds which of lineno, function or memory checks should be ticked. Used for UpdateUI
public:
    gsgs::BreakpointInfo b; // Used to return the amended settings
    wxStaticText* m_staticText9;
    wxChoicebook* m_choicebook;
    wxPanel* m_panel1;
    wxStaticText* m_staticText51;
    wxCheckBox* m_checkLineno;
    wxTextCtrl* m_textLineno;
    wxCheckBox* m_checkBreakFunction;
    wxTextCtrl* m_textFunctionname;
    wxCheckBox* m_checkRegex;
    wxCheckBox* m_checkBreakMemory;
    wxTextCtrl* m_textBreakMemory;
    wxStaticText* m_staticText6;
    wxTextCtrl* m_textFilename;
    gsgs::Button* m_buttonBrowse;
    wxStaticLine* m_staticline5;
    wxPanel* m_panel2;
    wxRadioBox* m_radioWatchtype;
    wxStaticText* m_staticText5;
    wxTextCtrl* m_textWatchExpression;
    wxStaticText* m_staticText1;
    wxTextCtrl* m_textCond;
    wxStaticText* m_staticText11;
    wxTextCtrl* m_textCommands;
    wxCheckBox* m_checkDisable;
    wxCheckBox* m_checkTemp;
    wxStaticText* m_staticText7;
    wxSpinCtrl* m_spinIgnore;
    gsgs::Button* m_button6;
    gsgs::Button* m_button7;
};

class ThreadListPanel : public wxPanel
{
public:
    ThreadListPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
    virtual ~ThreadListPanel();
    
    gsgs::ThemedListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
        
    void PopulateList(const ThreadEntryArray& threads);
    void Clear();
protected:
    // Handlers for ThreadListBasePanel events.
    void OnItemActivated(wxDataViewEvent& event);
    bool IsTheSame(const ThreadEntryArray& threads1, const ThreadEntryArray& threads2);
protected:
    ThreadEntryArray m_threads;
    gsgs::ThemedListCtrl* m_dvListCtrl;
};

class CompilersFoundDlg : public wxDialog
{
public:
    CompilersFoundDlg(wxWindow* parent, const ToolsetLocator::CompilerVec_t& compilers, wxWindowID id = wxID_ANY, 
        const wxString& title = _("Find Installed Compilers"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~CompilersFoundDlg();
    
    CompilerPtr GetCompiler(const wxDataViewItem & item) const;
    bool IsDefaultCompiler(CompilerPtr compiler) const;
    wxBannerWindow* GetBanner10()           { return m_banner10; }
    wxDataViewCtrl* GetDataview()           { return m_dataview; }
    wxStaticBitmap* GetStaticBitmap35()     { return m_staticBitmap35; }
    wxStaticText* GetStaticText29()         { return m_staticText29; }
private:
    /**
     * @brief under MSW, update the toolchain (make) for VC and Clang compilers
     * using the active MinGW compiler
     */
    void MSWUpdateToolchain(CompilerPtr compiler);
protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
protected:
    std::map<wxString, CompilerPtr> m_defaultCompilers;
    ToolsetLocator::CompilerVec_t m_allCompilers;
    wxBannerWindow* m_banner10;
    wxDataViewCtrl* m_dataview;
    wxObjectDataPtr<CompilersFoundModel> m_dataviewModel;

    wxStaticBitmap* m_staticBitmap35;
    wxStaticText* m_staticText29;
    DialogButtonSizer* m_stdBtnSizer4;
    gsgs::Button* m_buttonCancel;
    gsgs::Button* m_buttonOK;
};

class CompilersModifiedDlg : public wxDialog
{
public:
    CompilersModifiedDlg(wxWindow* parent, const gsgsStringSet& deletedCompilers, wxWindowID id = wxID_ANY, 
        const wxString& title = _("Could not find selected compiler..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~CompilersModifiedDlg();
    
    const gsgsStringMap& GetReplacementTable() const {
        return m_table;
    }
    wxStaticText* GetStaticText27() { return m_staticText27; }
    wxPropertyGridManager* GetPgMgrCompilers() { return m_pgMgrCompilers; }
protected:
    virtual void OnValueChanged(wxPropertyGridEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
protected:
    wxStaticText* m_staticText27;
    wxPropertyGridManager* m_pgMgrCompilers;
    wxPGProperty* m_pgPropHeader;
    DialogButtonSizer* m_stdBtnSizer18;
    gsgs::Button* m_buttonOK;
    gsgs::Button* m_buttonCancel;
    wxVector<wxPGProperty*> m_props;
    gsgsStringMap           m_table;
    bool                    m_enableOKButton;
};

// ================------------------------------
// Compiler configuration page
// ================------------------------------

enum { IFS_INCLUDE_FILES_WO_EXT = 0x00000001 };

class ImportFilesSettings : public ObjectData
{
    wxString m_fileMask;
    size_t m_flags;
    wxString m_baseDir;

public:
    ImportFilesSettings();
    virtual ~ImportFilesSettings();

    virtual void DeSerialize(gsgs::Serializer& arch);
    virtual void Serialize(gsgs::Serializer& arch);

    // Setters
    void SetFileMask(const wxString& fileMask)
    {
        this->m_fileMask = fileMask;
    }
    void SetFlags(const size_t& flags)
    {
        this->m_flags = flags;
    }
    // Getters
    const wxString& GetFileMask() const
    {
        return m_fileMask;
    }
    const size_t& GetFlags() const
    {
        return m_flags;
    }

    void SetBaseDir(const wxString& baseDir)
    {
        this->m_baseDir = baseDir;
    }
    const wxString& GetBaseDir() const
    {
        return m_baseDir;
    }
};

typedef std::unordered_map<wxString, bool>      wxStringBoolMap_t;

class ImportFilesDialogNew : public wxDialog
{
public:
    ImportFilesDialogNew(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Import Files"), 
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~ImportFilesDialogNew();
    void GetDirectories(wxStringBoolMap_t &dirs);
    bool ExtlessFiles();
    wxString GetFileMask();
    wxString GetBaseDir();
    
    wxVariant MakeIconText(const wxString &text, const wxBitmap& bmp) const;
    
        wxBannerWindow* GetBanner1() { return m_banner1; }
    wxTextCtrl* GetTextCtrlDir() { return m_textCtrlDir; }
    gsgs::Button* GetButtonBrowse() { return m_buttonBrowse; }
    wxDataViewCtrl* GetDataview() { return m_dataview; }
    wxStaticText* GetStaticText1() { return m_staticText1; }
    wxTextCtrl* GetTextCtrSpec() { return m_textCtrSpec; }
    wxCheckBox* GetCheckBoxFilesWOExt() { return m_checkBoxFilesWOExt; }
protected:
    virtual void OnBrowse(wxCommandEvent& event);
    virtual void OnItemExpanding(wxDataViewEvent& event);
    void DoCheckChildren(const wxDataViewItem& parent, bool check);
    void DoGetCheckedDirs(const wxDataViewItem& parent, wxStringBoolMap_t &dirs);
    virtual void OnValueChanged(wxDataViewEvent& event);
    virtual void OnDirChanged(wxCommandEvent& event);
    void DoBuildTree(const wxDataViewItem& parent, const wxDir& dir, bool initialState);
    void DoBuildTree();
private:
    wxDataViewItem m_root;
    wxBannerWindow* m_banner1;
    wxTextCtrl* m_textCtrlDir;
    gsgs::Button* m_buttonBrowse;
    wxDataViewCtrl* m_dataview;
    wxObjectDataPtr<FolderModel> m_dataviewModel;

    wxStaticText* m_staticText1;
    wxTextCtrl* m_textCtrSpec;
    wxCheckBox* m_checkBoxFilesWOExt;
    DialogButtonSizer* m_stdBtnSizer7;
    gsgs::Button* m_buttonOK;
    gsgs::Button* m_buttonCancel;
};

/** Implementing DependenciesPage */
class DependenciesPage : public wxPanel
{
public:
    /** Constructor */
    DependenciesPage( wxWindow* parent, const wxString &projectName , wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxTAB_TRAVERSAL);
    virtual ~DependenciesPage();
    void Save();
    
    wxStaticText* GetStaticText3() { return m_staticText3; }
    wxChoice* GetChoiceProjectConfig() { return m_choiceProjectConfig; }
    wxStaticText* GetStaticText1() { return m_staticText1; }
    wxCheckListBox* GetCheckListProjectList() { return m_checkListProjectList; }
    wxStaticText* GetStaticText2() { return m_staticText2; }
    wxListBox* GetListBoxBuildOrder() { return m_listBoxBuildOrder; }
    gsgs::Button* GetButtonUp() { return m_buttonUp; }
    gsgs::Button* GetButtonDown() { return m_buttonDown; }
    gsgs::Button* GetButtonApply() { return m_buttonApply; }
protected:
    // Handlers for DependenciesPage events.
    void OnConfigChanged( wxCommandEvent& event );
    void OnCheckListItemToggled( wxCommandEvent& event );
    void OnMoveUp( wxCommandEvent& event );
    void OnMoveDown( wxCommandEvent& event );
    void OnApplyButton(wxCommandEvent &event);
    void OnApplyButtonUI(wxUpdateUIEvent &event);

protected:
    void Init();
    void OnUpCommand(wxListBox *list);
    void OnDownCommand(wxListBox *list);
    void DoPopulateControl(const wxString &configuration);
private:
    wxStaticText* m_staticText3;
    wxChoice* m_choiceProjectConfig;
    wxStaticText* m_staticText1;
    wxCheckListBox* m_checkListProjectList;
    wxStaticText* m_staticText2;
    wxListBox* m_listBoxBuildOrder;
    gsgs::Button* m_buttonUp;
    gsgs::Button* m_buttonDown;
    gsgs::Button* m_buttonApply;
    wxString m_projectName;
    wxString m_currentSelection;
        bool m_dirty;
};

class FileCheckList : public wxDialog
{
public:
    FileCheckList( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select Files"),
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxRESIZE_BORDER);
    ~FileCheckList();

    void SetCancellable(bool can) {
        m_buttonCancel->Show(can);
    }
    void SetCaption(const wxString &caption);
    void SetBaseDir(const wxFileName &dir);

    void SetFiles(const std::vector<std::pair<wxFileName, bool> > &files);
    const std::vector<std::pair<wxFileName,bool> > & GetFiles() const {
        return m_files;
    }
    
    wxStaticText* GetCaption()              { return m_caption; }
    wxCheckListBox* GetFileCheckList()      { return m_fileCheckList; }
    gsgs::Button* GetClearAll()                 { return m_clearAll; }
    gsgs::Button* GetCheckAllButton()           { return m_checkAllButton; }
    wxTextCtrl* GetSelectedFilePath()       { return m_selectedFilePath; }
protected:
    // Handlers for filechecklistbase events.
    void OnCheckAll( wxCommandEvent& event );
    void OnClearAll( wxCommandEvent& event );
    void OnFileSelected( wxCommandEvent& event );
    void OnFileCheckChanged( wxCommandEvent& event );

    void ShowFilePath(size_t n);
private:
    std::vector<std::pair<wxFileName,bool> > m_files;
    wxFileName m_baseDir;
    wxStaticText* m_caption;
    wxCheckListBox* m_fileCheckList;
    gsgs::Button* m_clearAll;
    gsgs::Button* m_checkAllButton;
    wxTextCtrl* m_selectedFilePath;
    DialogButtonSizer* m_stdBtnSizer2;
    gsgs::Button* m_buttonOK;
    gsgs::Button* m_buttonCancel;
};

class FindInFilesLocationsDlg : public wxDialog
{
public:
    FindInFilesLocationsDlg(wxWindow* parent, const wxArrayString& locations, wxWindowID id = wxID_ANY,
                                const wxString& title = _("Edit Search Locations"),
                                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~FindInFilesLocationsDlg();

    wxArrayString GetLocations() const;
    
        wxCheckListBox* GetCheckListBoxLocations() { return m_checkListBoxLocations; }
    gsgs::Button* GetButtonAdd() { return m_buttonAdd; }
    gsgs::Button* GetButtonDelete() { return m_buttonDelete; }
protected:
    virtual void OnAddPath(wxCommandEvent& event);
    virtual void OnDeletePath(wxCommandEvent& event);
    virtual void OnDeletePathUI(wxUpdateUIEvent& event);
    void DoAppendItem(const wxString& str);
    void DoAppendItem(const wxString& str, bool check);
private:
    wxArrayString m_initialLocations;
    wxCheckListBox* m_checkListBoxLocations;
    gsgs::Button* m_buttonAdd;
    gsgs::Button* m_buttonDelete;
    DialogButtonSizer* m_stdBtnSizer113;
    gsgs::Button* m_button115;
    gsgs::Button* m_button117;
};

class FindInFilesDialog : public wxDialog
{
protected:
    wxArrayString GetPathsAsArray() const;

protected:
    virtual void OnLookInKeyDown(wxKeyEvent& event);
    virtual void OnReplaceUI(wxUpdateUIEvent& event);
    virtual void OnButtonClose(wxCommandEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnReplace(wxCommandEvent& event);
    void DoSearch();
    void DoSearchReplace();
    void DoSaveSearchPaths();
    SearchData DoGetSearchData();
    void DoSaveOpenFiles();
    void DoSetFileMask();
    void DoAddProjectFiles(const wxString& projectName, wxArrayString& files);

    // Set new search paths
    void DoSetSearchPaths(const wxString& paths);

    // Event Handlers
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnAddPath(wxCommandEvent& event);

    virtual void OnFindWhatUI(wxUpdateUIEvent& event);

    void OnUseDiffColourForCommentsUI(wxUpdateUIEvent& event);
    size_t GetSearchFlags();
    void BuildFindReplaceData();

public:
    FindInFilesDialog(wxWindow* parent, FindReplaceData& data, wxWindowID id = wxID_ANY, const wxString& title = _("Find In Files"),
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                          long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~FindInFilesDialog();
    void SetSearchPaths(const wxString& paths, bool transient = false);
    void SetFileMask(const wxString& mask);
    int ShowDialog();
    
    wxStaticText* GetStaticText1() { return m_staticText1; }
    wxComboBox* GetFindString() { return m_findString; }
    wxStaticText* GetStaticText102() { return m_staticText102; }
    wxComboBox* GetReplaceString() { return m_replaceString; }
    wxStaticText* GetStaticText3() { return m_staticText3; }
    wxComboBox* GetFileTypes() { return m_fileTypes; }
    wxStaticText* GetStaticText2() { return m_staticText2; }
    wxStyledTextCtrl* GetStcPaths() { return m_stcPaths; }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxChoice* GetChoiceEncoding() { return m_choiceEncoding; }
    gsgs::Button* GetFind() { return m_find; }
    gsgs::Button* GetReplaceAll() { return m_replaceAll; }
    gsgs::Button* GetCancel() { return m_cancel; }
    gsgs::Button* GetBtnAddPath() { return m_btnAddPath; }
    wxPanel* GetPanelMainPanel() { return m_panelMainPanel; }
    wxStaticLine* GetStaticLine139() { return m_staticLine139; }
    wxCheckBox* GetMatchCase() { return m_matchCase; }
    wxCheckBox* GetMatchWholeWord() { return m_matchWholeWord; }
    wxCheckBox* GetRegualrExpression() { return m_regualrExpression; }
    wxCheckBox* GetCheckBoxPipeForGrep() { return m_checkBoxPipeForGrep; }
    wxCheckBox* GetCheckBoxSaveFilesBeforeSearching() { return m_checkBoxSaveFilesBeforeSearching; }
private:
    wxPanel* m_panelMainPanel;
    wxStaticText* m_staticText1;
    wxComboBox* m_findString;
    wxStaticText* m_staticText102;
    wxComboBox* m_replaceString;
    wxStaticText* m_staticText3;
    wxComboBox* m_fileTypes;
    wxStaticText* m_staticText2;
    wxStyledTextCtrl* m_stcPaths;
    wxStaticText* m_staticText5;
    wxChoice* m_choiceEncoding;
    gsgs::Button* m_find;
    gsgs::Button* m_replaceAll;
    gsgs::Button* m_cancel;
    gsgs::Button* m_btnAddPath;
    wxStaticLine* m_staticLine139;
    wxCheckBox* m_matchCase;
    wxCheckBox* m_matchWholeWord;
    wxCheckBox* m_regualrExpression;
    wxCheckBox* m_checkBoxPipeForGrep;
    wxCheckBox* m_checkBoxSaveFilesBeforeSearching;
    FindReplaceData& m_data;
    wxArrayString m_pluginFileMask;
    bool m_transient = false;
};

class FilesModifiedDlg : public wxDialog
{
public:
    enum {
        ID_BUTTON_CHOOSE = 5842,
        ID_BUTTON_IGNORE = 5843,
        ID_BUTTON_LOAD = 5844,
    };
    
    enum {
        kID_BUTTON_CHOOSE = ID_BUTTON_CHOOSE,
        kID_BUTTON_LOAD   = ID_BUTTON_LOAD,
        kID_BUTTON_IGNORE = ID_BUTTON_IGNORE,
    };
    
    FilesModifiedDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Files were modified outside the editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~FilesModifiedDlg();
    
    bool GetRememberMyAnswer() const {
        return m_checkBoxRemember->IsChecked();
    }
    
    wxStaticText* GetStaticText34() { return m_staticText34; }
    wxCommandLinkButton* GetCmdLnkBtn30() { return m_cmdLnkBtn30; }
    wxCommandLinkButton* GetCmdLnkBtn28() { return m_cmdLnkBtn28; }
    wxCommandLinkButton* GetCmdLnkBtnNone() { return m_cmdLnkBtnNone; }
    wxCheckBox* GetCheckBoxRemember() { return m_checkBoxRemember; }
    
protected:
    virtual void OnIgnore(wxCommandEvent& event);
    virtual void OnChoose(wxCommandEvent& event);
    virtual void OnLoad(wxCommandEvent& event);
private:
    wxStaticText* m_staticText34;
    wxCommandLinkButton* m_cmdLnkBtn30;
    wxCommandLinkButton* m_cmdLnkBtn28;
    wxCommandLinkButton* m_cmdLnkBtnNone;
    wxCheckBox* m_checkBoxRemember;
};

class DebuggerEditItemDlg : public wxDialog
{
public:
    DebuggerEditItemDlg(wxWindow* parent, const wxString &initialValue, wxWindowID id = wxID_ANY, const wxString& title = _("Edit Item"),
                              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400, -1),
                              long style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
    virtual ~DebuggerEditItemDlg();
    
    wxString GetValue() const;
    void SetValue(const wxString &value) {
        m_textCtrl38->ChangeValue( value );
    }
    wxStaticText* GetStaticText36() { return m_staticText36; }
    wxTextCtrl* GetTextCtrl38() { return m_textCtrl38; }
    wxPanel* GetPanel32() { return m_panel32; }
    gsgs::Button* GetButton28() { return m_button28; }
    gsgs::Button* GetButton30() { return m_button30; }
protected:
    wxPanel* m_panel32;
    wxStaticText* m_staticText36;
    wxTextCtrl* m_textCtrl38;
    gsgs::Button* m_button28;
    gsgs::Button* m_button30;
};

class PSGeneralPage;
class ProjectSettingsDlg;
class wxPGProperty;
class wxStyledTextCtrl;
class wxPopupWindow;

namespace gsgs
{
    class WorkspaceTab;
}
/////////////////////////////////////////////////////////////
// base class for the project settings pages
/////////////////////////////////////////////////////////////
class IProjectSettingsPage
{
public:
    IProjectSettingsPage() {}
    virtual ~IProjectSettingsPage() {}

    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr) = 0;
    virtual void Load(BuildConfigPtr buildConf) = 0;
    virtual void Clear() = 0;

    virtual bool PopupAddOptionDlg(wxTextCtrl * ctrl);
    virtual bool PopupAddOptionDlg(wxString & value);
    virtual bool SelectChoiceWithGlobalSettings(wxChoice * c, const wxString & text);
    virtual void SelectChoiceWithGlobalSettings(wxPGProperty * p, const wxString & text);
    virtual bool
    PopupAddOptionCheckDlg(wxTextCtrl * ctrl, const wxString& title, const Compiler::CmpCmdLineOptions& options);
    virtual bool PopupAddOptionCheckDlg(wxString & v, const wxString& title, const Compiler::CmpCmdLineOptions& options);
};

/**
 * Implementing GlobalSettingsBasePanel
 */
class GlobalSettingsPanel : public GlobalSettingsBasePanel, public IProjectSettingsPage
{
    const wxString& m_projectName;
    ProjectSettingsDlg* m_dlg;
    PSGeneralPage* m_gp;

public:
    GlobalSettingsPanel(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg, PSGeneralPage* gp);

    virtual void Clear();
    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);

protected:
    virtual void OnCustomEditorClicked(wxCommandEvent& event);
    virtual void OnValueChanged(wxPropertyGridEvent& event);
};

/**
 * Implementing ProjectSettingsBaseDlg
 */
class ProjectSettingsDlg : public ProjectSettingsBaseDlg
{
public:
    virtual void OnButtonCancel(wxCommandEvent& event);
    inline void SetIsProjectEnabled(bool set)           { this->m_isProjectEnabled = set; }
    inline bool IsProjectEnabled() const                { return m_isProjectEnabled; }
    inline bool IsCustomBuildEnabled() const            { return m_isCustomBuild; }
    inline void SetCustomBuildEnabled(bool b)
    {
        if(b != m_isCustomBuild) 
        {
            SetIsDirty(true);
        }
        m_isCustomBuild = b;
    }
    inline void SetIsDirty(bool isDirty)                { this->m_isDirty = isDirty; }
    inline bool GetIsDirty() const                      { return m_isDirty; }

    /**
     * @brief show an info bar at the top of the dialog to indicate that this project is disabled
     */
    void ShowHideDisabledMessage();

    /**
     * @brief show an info bar at the top of dialog to indicate that the options are disabled
     */
    void ShowCustomProjectMessage(bool show);

public:
    /** Constructor */
    ProjectSettingsDlg(wxWindow* parent,
                       gsgs::WorkspaceTab* workspaceTab,
                       const wxString& configName,
                       const wxString& projectName,
                       const wxString& title);
    virtual ~ProjectSettingsDlg();

    inline const wxString& GetConfigName() const        { return m_configName; }
    inline const wxString& GetProjectName() const       { return m_projectName; }

    DECLARE_EVENT_TABLE()

    virtual void OnButtonApply(wxCommandEvent& event);
    virtual void OnButtonOK(wxCommandEvent& event);
    virtual void OnButtonHelp(wxCommandEvent& e);
    virtual void OnButtonApplyUI(wxUpdateUIEvent& event);
    virtual void OnConfigurationChanged(wxCommandEvent& event);
    virtual void OnProjectSelected(wxCommandEvent& e);
    virtual void OnWorkspaceClosed(wxCommandEvent& e);
protected:
    virtual void OnPageChanged(wxTreebookEvent& event);
    void SaveValues();
    void ClearValues();
    void LoadValues(const wxString& configName);
    void BuildTree();
    void DoClearDialog();
    void DoGetAllBuildConfigs();
private:
    wxString m_projectName;
    wxString m_configName;
    gsgs::WorkspaceTab* m_workspaceTab;
    bool m_isDirty;
    bool m_isCustomBuild;
    bool m_isProjectEnabled;
};

/** Implementing MoveFuncImplBaseDlg */
class MoveFuncImplDlg : public wxDialog
{
public:
    /** Constructor */
    MoveFuncImplDlg(wxWindow* parent, const wxString& text, const wxString& fileName, wxWindowID id = wxID_ANY, 
        const wxString& title = _("Move Function Implementation Preview"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    ~MoveFuncImplDlg();
    void SetText(const wxString& text);
    wxString GetText();

    void SetFileName(const wxString& fileName);
    wxString GetFileName();
    
    wxStaticText* GetStaticText4() { return m_staticText4; }
    gsgs::FilePicker* GetFilePicker() { return m_filePicker; }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxStyledTextCtrl* GetPreview() { return m_preview; }
protected:
    void OnButtonCancel(wxCommandEvent& e);
    void OnButtonOK(wxCommandEvent& e);
protected:
    wxStaticText* m_staticText4;
    gsgs::FilePicker* m_filePicker;
    wxStaticText* m_staticText5;
    wxStyledTextCtrl* m_preview;
    DialogButtonSizer* m_stdBtnSizer2;
    gsgs::Button* m_buttonOK;
    gsgs::Button* m_buttonCancel;
};

class CodeCompletionSettingsDialog : public wxDialog
{
public:
    CodeCompletionSettingsDialog(wxWindow* parent, const TagsOptionsData& data, wxWindowID id = wxID_ANY, const wxString& title = _("Code Completion"),
                       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                       long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~CodeCompletionSettingsDialog();
    TagsOptionsData& GetData() { return m_data; }

    void Parse();
    
    wxStaticText* GetStaticText14() { return m_staticText14; }
    wxTextCtrl* GetSpinCtrlNumberOfCCItems() { return m_spinCtrlNumberOfCCItems; }
    wxStaticText* GetStaticText123() { return m_staticText123; }
    wxTextCtrl* GetTextCtrlFileSpec() { return m_textCtrlFileSpec; }
    wxStaticLine* GetStaticLine129() { return m_staticLine129; }
    wxCheckBox* GetCheckDisplayTypeInfo() { return m_checkDisplayTypeInfo; }
    wxCheckBox* GetCheckBoxEnableCaseSensitiveCompletion() { return m_checkBoxEnableCaseSensitiveCompletion; }
    wxCheckBox* GetCheckDisplayFunctionTip() { return m_checkDisplayFunctionTip; }
    wxCheckBox* GetCheckBoxKeepFunctionSignature() { return m_checkBoxKeepFunctionSignature; }
    wxCheckBox* GetCheckCppKeywordAssist() { return m_checkCppKeywordAssist; }
    wxCheckBox* GetCheckBoxretagWorkspaceOnStartup() { return m_checkBoxretagWorkspaceOnStartup; }
    wxCheckBox* GetCheckDisableParseOnSave() { return m_checkDisableParseOnSave; }
    wxCheckBox* GetCheckBoxDeepUsingNamespaceResolving() { return m_checkBoxDeepUsingNamespaceResolving; }
    wxPanel* GetPaneDisplayAndBehavior() { return m_paneDisplayAndBehavior; }
    wxPropertyGridManager* GetPgMgrColouring() { return m_pgMgrColouring; }
    wxPanel* GetPaneColouring() { return m_paneColouring; }
    wxCheckBox* GetCheckWordAssist() { return m_checkWordAssist; }
    wxStaticText* GetStaticTextMinWordLen() { return m_staticTextMinWordLen; }
    wxSlider* GetSliderMinWordLen() { return m_sliderMinWordLen; }
    wxCheckBox* GetCheckAutoInsertSingleChoice() { return m_checkAutoInsertSingleChoice; }
    wxPanel* GetPaneTriggering() { return m_paneTriggering; }
    wxStyledTextCtrl* GetTextCtrlCtagsSearchPaths() { return m_textCtrlCtagsSearchPaths; }
    gsgs::Button* GetButtonAddSearchPath() { return m_buttonAddSearchPath; }
    gsgs::Button* GetButtonCtagsSuggest() { return m_buttonCtagsSuggest; }
    wxPanel* GetPanel38() { return m_panel38; }
    wxStyledTextCtrl* GetTextCtrlCtagsExcludePaths() { return m_textCtrlCtagsExcludePaths; }
    gsgs::Button* GetButtonAddExcludePath() { return m_buttonAddExcludePath; }
    wxPanel* GetPanel40() { return m_panel40; }
    wxStyledTextCtrl* GetTextPrep() { return m_textPrep; }
    wxStaticText* GetStaticText9() { return m_staticText9; }
    wxHyperlinkCtrl* GetHyperlink1() { return m_hyperlink1; }
    wxPanel* GetPanelClangGeneral() { return m_panelClangGeneral; }
    wxStaticText* GetStaticText91() { return m_staticText91; }
    wxStyledTextCtrl* GetTextTypes() { return m_textTypes; }
    wxPanel* GetPanel5() { return m_panel5; }
    wxNotebook* GetNotebook2() { return m_notebook2; }
    wxTextCtrl* GetTextCtrlFilesList() { return m_textCtrlFilesList; }
    gsgs::Button* GetButtonParse() { return m_buttonParse; }
    wxPanel* GetPanelCtagsAdvanced() { return m_panelCtagsAdvanced; }
    wxNotebook* GetNotebook93() { return m_notebook93; }
    wxPanel* GetPageCtags() { return m_pageCtags; }
    wxNotebook* GetNotebook87() { return m_notebook87; }
    gsgs::Button* GetButton104() { return m_button104; }
    gsgs::Button* GetButton106() { return m_button106; }
protected:
    virtual void OnAddExcludePath(wxCommandEvent& event);
    virtual void OnAddSearchPath(wxCommandEvent& event);
    virtual void OnAutoShowWordAssitUI(wxUpdateUIEvent& event);
    virtual void OnFileSelectedUI(wxUpdateUIEvent& event);
    virtual void OnParse(wxCommandEvent& event);
    virtual void OnButtonCancel(wxCommandEvent& event);
    virtual void OnButtonOk(wxCommandEvent& event);
    virtual void OnColouringPropertyValueChanged(wxPropertyGridEvent& event);
    virtual void OnSuggestCtags(wxCommandEvent& event);
    wxArrayString GetCTagsSearchPaths() const;
    void DoSuggest(wxStyledTextCtrl* textCtrl);
    void DoSetEditEventsHandler(wxWindow* win);
protected:
    void SetFlag(CodeCompletionOpts flag, bool set);
    void SetColouringFlag(CodeCompletionColourOpts flag, bool set);
    void CopyData();
protected:
    wxNotebook* m_notebook87;
    wxPanel* m_paneDisplayAndBehavior;
    wxStaticText* m_staticText14;
    wxTextCtrl* m_spinCtrlNumberOfCCItems;
    wxStaticText* m_staticText123;
    wxTextCtrl* m_textCtrlFileSpec;
    wxStaticLine* m_staticLine129;
    wxCheckBox* m_checkDisplayTypeInfo;
    wxCheckBox* m_checkBoxEnableCaseSensitiveCompletion;
    wxCheckBox* m_checkDisplayFunctionTip;
    wxCheckBox* m_checkBoxKeepFunctionSignature;
    wxCheckBox* m_checkCppKeywordAssist;
    wxCheckBox* m_checkBoxretagWorkspaceOnStartup;
    wxCheckBox* m_checkDisableParseOnSave;
    wxCheckBox* m_checkBoxDeepUsingNamespaceResolving;
    wxPanel* m_paneColouring;
    wxPropertyGridManager* m_pgMgrColouring;
    wxPGProperty* m_pgPropTrackPreProcessors;
    wxPGProperty* m_pgPropColourLocalVariables;
    wxPanel* m_paneTriggering;
    wxCheckBox* m_checkWordAssist;
    wxStaticText* m_staticTextMinWordLen;
    wxSlider* m_sliderMinWordLen;
    wxCheckBox* m_checkAutoInsertSingleChoice;
    wxPanel* m_pageCtags;
    wxNotebook* m_notebook93;
    wxPanel* m_panel38;
    wxStyledTextCtrl* m_textCtrlCtagsSearchPaths;
    gsgs::Button* m_buttonAddSearchPath;
    gsgs::Button* m_buttonCtagsSuggest;
    wxPanel* m_panel40;
    wxStyledTextCtrl* m_textCtrlCtagsExcludePaths;
    gsgs::Button* m_buttonAddExcludePath;
    wxPanel* m_panelCtagsAdvanced;
    wxNotebook* m_notebook2;
    wxPanel* m_panelClangGeneral;
    wxStyledTextCtrl* m_textPrep;
    wxStaticText* m_staticText9;
    wxHyperlinkCtrl* m_hyperlink1;
    wxPanel* m_panel5;
    wxStaticText* m_staticText91;
    wxStyledTextCtrl* m_textTypes;
    wxTextCtrl* m_textCtrlFilesList;
    gsgs::Button* m_buttonParse;
    gsgs::Button* m_button104;
    gsgs::Button* m_button106;
    TagsOptionsData m_data;
    std::vector<gsgs::EditEventsHandler::Ptr_t> m_handlers;
};

namespace gsgs
{ 
class LocalWorkspace;
}

class CodeCompletionPage : public wxPanel
{
public:
    enum {
        TypeWorkspace,
        TypeProject
    };
public:
    CodeCompletionPage(wxWindow *parent, int type, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500,300), long style = wxTAB_TRAVERSAL);
    virtual ~CodeCompletionPage();

    void Save();

    wxArrayString GetIncludePaths() const;
    wxString      GetMacros() const;
    wxString      GetIncludePathsAsString() const;
    bool          IsCpp11Enabled() const {
        return m_checkBoxCpp11->IsChecked();
    }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxTextCtrl* GetTextCtrlSearchPaths() { return m_textCtrlSearchPaths; }
    wxPanel* GetPanel8() { return m_panel8; }
    wxStaticText* GetStaticText12() { return m_staticText12; }
    wxTextCtrl* GetTextCtrlMacros() { return m_textCtrlMacros; }
    wxCheckBox* GetCheckBoxCpp11() { return m_checkBoxCpp11; }
    wxCheckBox* GetCheckBoxCpp14() { return m_checkBoxCpp14; }
    wxCheckBox* GetCheckBoxCpp17() { return m_checkBoxCpp17; }
    wxCheckBox* GetCheckBoxSWTLW() { return m_checkBoxSWTLW; }
    wxPanel* GetPanel6() { return m_panel6; }
    wxSplitterWindow* GetSplitter1() { return m_splitter1; }
protected:
    // Event handlers
    void OnCCContentModified(wxCommandEvent& event);
private:
    wxSplitterWindow* m_splitter1;
    wxPanel* m_panel8;
    wxStaticText* m_staticText5;
    wxTextCtrl* m_textCtrlSearchPaths;
    wxPanel* m_panel6;
    wxStaticText* m_staticText12;
    wxTextCtrl* m_textCtrlMacros;
    wxCheckBox* m_checkBoxCpp11;
    wxCheckBox* m_checkBoxCpp14;
    wxCheckBox* m_checkBoxCpp17;
    wxCheckBox* m_checkBoxSWTLW;
    int  m_type;
    bool m_ccChanged;
};

/** Implementing WorkspaceSettingsBase */
class WorkspaceSettingsDlg : public wxDialog
{
public:
	WorkspaceSettingsDlg( wxWindow* parent, gsgs::LocalWorkspace *localWorkspace, wxWindowID id = wxID_ANY, const wxString& title = _("Workspace Settings"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
	virtual ~WorkspaceSettingsDlg();

	wxArrayString GetIncludePaths() const;
	wxArrayString GetExcludePaths() const;
        wxStaticText* GetStaticText3() { return m_staticText3; }
    wxStaticText* GetStaticText4() { return m_staticText4; }
    wxChoice* GetChoiceEnvSets() { return m_choiceEnvSets; }
    wxStaticLine* GetStaticline2() { return m_staticline2; }
    wxStaticText* GetStaticText6() { return m_staticText6; }
    wxTextCtrl* GetTextCtrlWspEnvVars() { return m_textCtrlWspEnvVars; }
    wxPanel* GetPanelEnv() { return m_panelEnv; }
    wxNotebook* GetNotebook1() { return m_notebook1; }
    wxStaticLine* GetStaticline1() { return m_staticline1; }
    gsgs::Button* GetButtonOk() { return m_buttonOk; }
    gsgs::Button* GetButtonCancel() { return m_buttonCancel; }
protected:
    virtual void OnEnvSelected(wxCommandEvent& event) { event.Skip(); }
	// Handlers for WorkspaceSettingsBase events.
	//void OnAddIncludePath( wxCommandEvent& event );
	//void OnAddExcludePath( wxCommandEvent& event );
	void OnButtonOK( wxCommandEvent& event );
protected:
	gsgs::LocalWorkspace* m_localWorkspace;
	CodeCompletionPage* m_ccPage;
private:
    wxNotebook* m_notebook1;
    wxPanel* m_panelEnv;
    wxStaticText* m_staticText3;
    wxStaticText* m_staticText4;
    wxChoice* m_choiceEnvSets;
    wxStaticLine* m_staticline2;
    wxStaticText* m_staticText6;
    wxTextCtrl* m_textCtrlWspEnvVars;
    wxStaticLine* m_staticline1;
    gsgs::Button* m_buttonOk;
    gsgs::Button* m_buttonCancel;
};

class QuickDebugDlg : public wxDialog
{
public:
    QuickDebugDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Quick Debug"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~QuickDebugDlg();
    wxArrayString GetStartupCmds();
    wxString GetExe();
    wxString GetDebuggerName();
    wxString GetWorkingDirectory();
    wxString GetArguments();
    wxString GetAlternateDebuggerExe() const { return m_textCtrlDebuggerExec->GetValue(); }

    wxStaticText* GetStaticText2() { return m_staticText2; }
    wxComboBox* GetExeFilepath() { return m_ExeFilepath; }
    gsgs::Button* GetButtonBrowseExe() { return m_buttonBrowseExe; }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxComboBox* GetWD() { return m_WD; }
    gsgs::Button* GetButtonBrowseWD() { return m_buttonBrowseWD; }
    wxStaticText* GetStaticText4() { return m_staticText4; }
    wxTextCtrl* GetTextCtrlArgs() { return m_textCtrlArgs; }
    wxStaticText* GetStaticText3() { return m_staticText3; }
    wxChoice* GetChoiceDebuggers() { return m_choiceDebuggers; }
    wxStaticText* GetStaticText12() { return m_staticText12; }
    wxTextCtrl* GetTextCtrlDebuggerExec() { return m_textCtrlDebuggerExec; }
    gsgs::Button* GetButtonBrowseDebugger() { return m_buttonBrowseDebugger; }
    wxStaticText* GetStaticText6() { return m_staticText6; }
    wxTextCtrl* GetTextCtrlCmds() { return m_textCtrlCmds; }

protected:

    virtual void OnSelectAlternateDebugger(wxCommandEvent& event);
    void OnButtonBrowseExe(wxCommandEvent& event);
    void OnButtonDebug(wxCommandEvent& event);
    void OnButtonCancel(wxCommandEvent& event);
    void OnButtonBrowseWD(wxCommandEvent& event);
    void Initialize();
protected:
    wxStaticText* m_staticText2;
    wxComboBox* m_ExeFilepath;
    gsgs::Button* m_buttonBrowseExe;
    wxStaticText* m_staticText5;
    wxComboBox* m_WD;
    gsgs::Button* m_buttonBrowseWD;
    wxStaticText* m_staticText4;
    wxTextCtrl* m_textCtrlArgs;
    wxStaticText* m_staticText3;
    wxChoice* m_choiceDebuggers;
    wxStaticText* m_staticText12;
    wxTextCtrl* m_textCtrlDebuggerExec;
    gsgs::Button* m_buttonBrowseDebugger;
    wxStaticText* m_staticText6;
    wxTextCtrl* m_textCtrlCmds;
    DialogButtonSizer* m_stdBtnSizer2;
    gsgs::Button* m_buttonOK;
    gsgs::Button* m_buttonCancel1;
};

class PersistentDebuggerTip : public wxPersistentWindow<wxPopupWindow>
{
public:
    PersistentDebuggerTip(wxPopupWindow* puw)
        : wxPersistentWindow<wxPopupWindow>(puw)
    {
    }

    virtual void Save() const;

    virtual bool Restore();

    virtual wxString GetKind() const { return "CLDebuggerTip"; }
};

/** Implementing NewQuickWatch */
class DisplayVariableDlg : public gsgs::ResizableTooltip
{
public:
    DisplayVariableDlg(wxWindow* parent);
    virtual ~DisplayVariableDlg();

    void AddItems(const wxString& varname, const VariableObjChildren& children);
    void UpdateValue(const wxString& varname, const wxString& value);
    void BuildTree(const VariableObjChildren& children, gsgs::IDebugger* debugger);
    void HideDialog();
    void ShowDialog(bool center);
    void OnCreateVariableObjError(const DebuggerEventData& event);
protected:
    bool IsFakeItem(const wxTreeItemId& item);
    wxString DoGetItemPath(const wxTreeItemId& item);
    void DoEditItem(const wxTreeItemId& item);

protected:
    void OnItemExpanding(wxTreeEvent& event);
    void OnBtnCancel(wxCommandEvent& e);
    void OnCloseEvent(wxCloseEvent& e);
    void DoAddChildren(wxTreeItemId& item, const VariableObjChildren& children);
    void DoCleanUp();
    void OnItemMenu(wxTreeEvent& event);
    void OnMenuSelection(wxCommandEvent& e);
public:
    gsgs::IDebugger* m_debugger;
    std::map<wxString, wxTreeItemId> m_gdbId2Item;
    std::map<wxString, wxTreeItemId> m_gdbId2ItemLeaf;
    wxString m_mainVariableObject;
    wxString m_variableName;
    wxString m_expression;
    wxString m_fullpath;
    wxString m_itemOldValue;
    bool m_editDlgIsUp;
};

class NewVirtualFolderDlg : public wxDialog
{
public:
    NewVirtualFolderDlg(wxWindow* parent, const wxString& currentVD, wxWindowID id = wxID_ANY, 
        const wxString& title = _("New Virtual Folder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1),
            long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~NewVirtualFolderDlg();
    wxString GetName() const            { return m_textCtrlName->GetValue(); }
    bool GetCreateOnDisk() const        { return m_checkBoxCreateOnDisk->IsChecked(); }
    wxString GetDiskPath() const        { return m_textCtrlPath->GetValue(); }
    wxStaticText* GetStaticText12()     { return m_staticText12; }
    wxTextCtrl* GetTextCtrlName()       { return m_textCtrlName; }
    wxCheckBox* GetCheckBoxCreateOnDisk() { return m_checkBoxCreateOnDisk; }
    wxTextCtrl* GetTextCtrlPath()       { return m_textCtrlPath; }
    gsgs::Button* GetButton6()              { return m_button6; }
    gsgs::Button* GetButton8()              { return m_button8; }
protected:
    void DoUpdatePath();
protected:
    virtual void OnCreateOnFolderChecked(wxCommandEvent& event);
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnCreateOnDiskUI(wxUpdateUIEvent& event);
    virtual void OnNameUpdated(wxCommandEvent& event);
private:
    wxString m_basePath;
    wxStaticText* m_staticText12;
    wxTextCtrl* m_textCtrlName;
    wxCheckBox* m_checkBoxCreateOnDisk;
    wxTextCtrl* m_textCtrlPath;
    gsgs::Button* m_button6;
    gsgs::Button* m_button8;
};

class ProjectCustomBuildTragetDlg : public ProjectCustomBuildTragetDlgBase
{
public:
    static const wxString CUSTOM_TARGET_BUILD               ;
    static const wxString CUSTOM_TARGET_CLEAN               ;
    static const wxString CUSTOM_TARGET_REBUILD             ;
    static const wxString CUSTOM_TARGET_COMPILE_SINGLE_FILE ;
    static const wxString CUSTOM_TARGET_PREPROCESS_FILE     ;

public:
    ProjectCustomBuildTragetDlg(wxWindow* parent, const wxString &targetName, const wxString& targetCommand);
    virtual ~ProjectCustomBuildTragetDlg();

    wxString GetTargetName() const {
        return m_textCtrlTargetName->GetValue();
    }

    wxString GetTargetCommand() const {
        return m_textCtrlCommand->GetValue();
    }
    
    static bool IsPredefinedTarget(const wxString& name);
    
protected:
    virtual void OnEditTargetNameUI(wxUpdateUIEvent& event);
};

/** Implementing PSBuildEventsBasePage */
class PSBuildEventsPage : public PSBuildEventsBasePage, public IProjectSettingsPage
{
    bool m_isPreEvents;
    ProjectSettingsDlg* m_dlg;

protected:
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);
    // Handlers for PSBuildEventsBasePage events.
    void OnCmdEvtVModified(wxCommandEvent& event);

public:
    /** Constructor */
    PSBuildEventsPage(wxWindow* parent, bool preEvents, ProjectSettingsDlg* dlg);
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();
};

/** Implementing PSCompilerPageBase */
class PSCompilerPage : public PSCompilerPageBase, public IProjectSettingsPage
{
    ProjectSettingsDlg* m_dlg;
    wxString m_projectName;
    PSGeneralPage* m_gp;

protected:
    virtual void OnCompilerNeeded(wxCommandEvent& event);
    virtual void OnCustomEditorClicked(wxCommandEvent& event);
    virtual void OnUpdateUI(wxUpdateUIEvent& event);
    virtual void OnPropertyChanged(wxPropertyGridEvent& event);
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);

public:
    /** Constructor */
    PSCompilerPage(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg, PSGeneralPage* gp);
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();
};

/** Implementing PSCompletionBase */
class PSCompletionPage : public PSCompletionBase, public IProjectSettingsPage
{
protected:
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);
    ProjectSettingsDlg* m_dlg;
    bool                m_ccSettingsModified;

protected:
    // Handlers for PSCompletionBase events.
    void OnCmdEvtVModified( wxCommandEvent& event );
    void OnBrowsePCH(wxCommandEvent& event);

public:
    /** Constructor */
    PSCompletionPage( wxWindow* parent, ProjectSettingsDlg* dlg );
    //// end generated class members

    virtual void Clear();
    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);

};

/** Implementing PSCustomBuildBasePage */
class PSCustomBuildPage : public PSCustomBuildBasePage, public IProjectSettingsPage
{
    const wxString& m_projectName;
    
    ProjectSettingsDlg* m_dlg;
    StringManager m_stringManager;

protected:
    virtual void OnEnableTableUI(wxUpdateUIEvent& event);
    virtual void OnTargetActivated(wxDataViewEvent& event);
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);
    // Handlers for PSCustomBuildBasePage events.
    void OnCustomBuildEnabled(wxCommandEvent& event);
    void OnCustomBuildEnabledUI(wxUpdateUIEvent& event);
    void OnCmdEvtVModified(wxCommandEvent& event);
    void OnBrowseCustomBuildWD(wxCommandEvent& event);
    void OnNewTarget(wxCommandEvent& event);
    void OnEditTarget(wxCommandEvent& event);
    void OnEditTargetUI(wxUpdateUIEvent& event);
    void OnDeleteTarget(wxCommandEvent& event);
    void OnDeleteTargetUI(wxUpdateUIEvent& event);
    void OnCustomBuildCBEnabledUI(wxUpdateUIEvent& event);

public:
    /** Constructor */
    PSCustomBuildPage(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg);
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();

protected:
    void DoEditTarget(wxDataViewItem item);
    void DoUpdateTarget(wxDataViewItem item, const wxString& target, const wxString& cmd);
    wxString GetTargetCommand(const wxString& target);
    ProjectSettingsDlg* GetDlg();
};

/** Implementing PSCustomMakefileBasePage */
class PSCustomMakefileRulesPage : public PSCustomMakefileBasePage, public IProjectSettingsPage
{
    ProjectSettingsDlg *m_dlg;
protected:
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);
    // Handlers for PSCustomMakefileBasePage events.
    void OnProjectCustumBuildUI( wxUpdateUIEvent& event );
    void OnCmdEvtVModified( wxCommandEvent& event );
public:
    /** Constructor */
    PSCustomMakefileRulesPage( wxWindow* parent, ProjectSettingsDlg *dlg );
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();

};

/** Implementing PSDebuggerPageBase */
class PSDebuggerPage : public PSDebuggerPageBase, public IProjectSettingsPage
{
    ProjectSettingsDlg *m_dlg;

protected:
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnAddDebuggerSearchPath(wxCommandEvent& event);
    virtual void OnDeleteDebuggerSearchPath(wxCommandEvent& event);
    virtual void OnDeleteDebuggerSearchPathUI(wxUpdateUIEvent& event);
    virtual void OnBrowseForDebuggerPath(wxCommandEvent& event);

    // Handlers for PSDebuggerPageBase events.
    void OnCmdEvtVModified( wxCommandEvent& event );
    void OnRemoteDebugUI( wxUpdateUIEvent& event );
    void DoAddPath(wxClientData* path);

public:
    /** Constructor */
    PSDebuggerPage( wxWindow* parent, ProjectSettingsDlg* dlg );
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();

};

/** Implementing PSResourcesPageBase */
class PSResourcesPage : public PSResourcesPageBase, public IProjectSettingsPage
{
    ProjectSettingsDlg *m_dlg;

protected:
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);
    virtual void OnResourcesEnabledUI(wxUpdateUIEvent& event);
    virtual void OnCustomEditorClicked(wxCommandEvent& event);
    virtual void OnValueChanged(wxPropertyGridEvent& event);

public:
    /** Constructor */
    PSResourcesPage( wxWindow* parent, ProjectSettingsDlg *dlg );
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();

};

/** Implementing PSLinkPageBase */
class PSLinkerPage : public PSLinkPageBase, public IProjectSettingsPage
{
    ProjectSettingsDlg *m_dlg;
    PSGeneralPage*      m_gp;

protected:
    virtual void OnPropertyChanged(wxPropertyGridEvent& event);
    virtual void OnCheckLinkerNeeded(wxCommandEvent& event);
    virtual void OnLinkerNotNeededUI(wxUpdateUIEvent& event);
    virtual void OnProjectCustumBuildUI(wxUpdateUIEvent& event);
    virtual void OnCustomEditorClicked(wxCommandEvent& event);
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);

public:
    /** Constructor */
    PSLinkerPage( wxWindow* parent, ProjectSettingsDlg *dlg, PSGeneralPage *gp );
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();

};

/** Implementing PSEnvironmentBasePage */
class PSEnvironmentPage : public PSEnvironmentBasePage, public IProjectSettingsPage
{
    ProjectSettingsDlg *m_dlg;

protected:
    virtual void OnProjectEnabledUI(wxUpdateUIEvent& event);
    // Handlers for PSEnvironmentBasePage events.
    void OnCmdEvtVModified( wxCommandEvent& event );

public:
    /** Constructor */
    PSEnvironmentPage( wxWindow* parent, ProjectSettingsDlg* dlg );
    //// end generated class members

    virtual void Load(BuildConfigPtr buildConf);
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Clear();

};

/** Implementing PSGeneralPageBase */
class PSGeneralPage : public PSGeneralPageBase, public IProjectSettingsPage
{
    ProjectSettingsDlg *m_dlg;
    wxString            m_projectName;
    wxString            m_configName;

protected:
    virtual void OnCustomEditorClicked(wxCommandEvent& event);
    virtual void OnProjectEnabled(wxCommandEvent& event);
    virtual void OnValueChanged(wxPropertyGridEvent& event);

    wxString GetPropertyAsString(wxPGProperty* prop) const;
    bool GetPropertyAsBool(wxPGProperty* prop) const;

protected:
    // Handlers for PSGeneralPageBase events.
    void OnProjectCustumBuildUI( wxUpdateUIEvent& event );

public:

    /** Constructor */
    PSGeneralPage( wxWindow* parent, const wxString &projectName, const wxString &conf, ProjectSettingsDlg *dlg );
    //// end generated class members
    virtual void Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr);
    virtual void Load(BuildConfigPtr buildConf);
    virtual void Clear();
    wxString GetCompiler() const {
        return m_pgPropCompiler->GetValueAsString();
    }
};

class LoadTabGroupDlg : public wxDialog
{
public:
	LoadTabGroupDlg(wxWindow* parent, const wxString& path, const wxArrayString& previousgroups, wxWindowID id = wxID_ANY, 
        const wxString& title = _("Load a tab group"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
	virtual ~LoadTabGroupDlg();

	void SetListTabs(const wxArrayString& tabs) {
		m_listBox->Set(tabs);
	}

	void InsertListItem(const wxString& item);

	void EnableReplaceCheck(bool value) {
		m_replaceCheck->Enable(value);
	}

	wxListBox* GetListBox() {
		return m_listBox;
	}

	bool GetReplaceCheck() {
		return m_replaceCheck->IsChecked();
	}
    
    wxStaticText* GetStaticText1() { return m_staticText1; }
    gsgs::Button* GetButtonBrowse() { return m_buttonBrowse; }

    gsgs::Button* GetButton6() { return m_button6; }
    gsgs::Button* GetButton5() { return m_button5; }
protected:
	virtual void OnBrowse( wxCommandEvent& WXUNUSED(event) );
	virtual void OnItemActivated( wxCommandEvent& WXUNUSED(event) );
protected:
    wxStaticText* m_staticText1;
    gsgs::Button* m_buttonBrowse;
    wxListBox* m_listBox;
    wxCheckBox* m_replaceCheck;
    gsgs::Button* m_button6;
    gsgs::Button* m_button5;
};

class SaveTabGroupDlg : public wxDialog
{
public:
	SaveTabGroupDlg(wxWindow* parent, const wxArrayString& previousgroups, wxWindowID id = wxID_ANY, 
        const wxString& title = _("Save a tab group"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
	virtual ~SaveTabGroupDlg();
	void SetListTabs(const wxArrayString& tabs) {
		m_ListTabs->Set(tabs);
		for ( unsigned int n=0; n < m_ListTabs->GetCount(); ++n ) {
			m_ListTabs->Check(n, true);
		}
	}

	void SetTextName(const wxString& name) {
		m_textName->SetValue(name);
	}

	bool GetChoices(wxArrayInt& intArr) const;

	wxString GetTabgroupName() const {
		return m_textName->GetValue();
	}

    bool GetSaveInWorkspace() const {
        return m_radioBoxWorkspaceOrGlobal->IsShown() && m_radioBoxWorkspaceOrGlobal->GetSelection() == 0;
    }
    
    wxStaticText* GetStaticText1() { return m_staticText1; }
    wxCheckListBox* GetListTabs() { return m_ListTabs; }
    gsgs::Button* GetButtonCheckAll() { return m_buttonCheckAll; }
    gsgs::Button* GetButtonClearAll() { return m_buttonClearAll; }
    wxStaticText* GetStaticText4() { return m_staticText4; }
    wxTextCtrl* GetTextName() { return m_textName; }
    wxRadioBox* GetRadioBoxWorkspaceOrGlobal() { return m_radioBoxWorkspaceOrGlobal; }
    gsgs::Button* GetButton6() { return m_button6; }
    gsgs::Button* GetButton5() { return m_button5; }
protected:
	virtual void OnCheckAll( wxCommandEvent& event );
	virtual void OnCheckAllUpdateUI( wxUpdateUIEvent& event );
	virtual void OnClearAll( wxCommandEvent& event );
	virtual void OnClearAllUpdateUI( wxUpdateUIEvent& event );
protected:
    wxStaticText* m_staticText1;
    wxCheckListBox* m_ListTabs;
    gsgs::Button* m_buttonCheckAll;
    gsgs::Button* m_buttonClearAll;
    wxStaticText* m_staticText4;
    wxTextCtrl* m_textName;
    wxRadioBox* m_radioBoxWorkspaceOrGlobal;
    gsgs::Button* m_button6;
    gsgs::Button* m_button5;
};

class RenameSymbol : public wxDialog
{
public:
    enum {
        ID_CHECK_ALL = 1001,
        ID_UNCHECK_ALL = 1002,
    };

    
    RenameSymbol(wxWindow* parent, const gsgs::CppToken::Vec_t& candidates, const gsgs::CppToken::Vec_t& possCandidates,
                 const wxString& oldname = wxEmptyString, wxWindowID id = wxID_ANY, const wxString& title = _("Rename Symbol"), 
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~RenameSymbol();
    void GetMatches(gsgs::CppToken::Vec_t& matches);
    wxString GetWord() const { return m_textCtrlNewName->GetValue(); }
    wxStaticText* GetStaticText31() { return m_staticText31; }
    wxTextCtrl* GetTextCtrlNewName() { return m_textCtrlNewName; }
    wxDataViewListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
    gsgs::Button* GetButtonCheckAll() { return m_buttonCheckAll; }
    gsgs::Button* GetButtonUncheckAll() { return m_buttonUncheckAll; }
    wxPanel* GetSplitterPage11() { return m_splitterPage11; }
    wxStyledTextCtrl* GetPreview() { return m_preview; }
    wxPanel* GetSplitterPage15() { return m_splitterPage15; }
    wxSplitterWindow* GetSplitter7() { return m_splitter7; }
    wxStaticLine* GetStaticLine55() { return m_staticLine55; }
    gsgs::Button* GetButtonOk() { return m_buttonOk; }
    gsgs::Button* GetButtonCancel() { return m_buttonCancel; }
protected:
    virtual void OnCheckAll(wxCommandEvent& event);
    virtual void OnUncheckAll(wxCommandEvent& event);
    virtual void OnSelection(wxDataViewEvent& event);
    void OnButtonOK(wxCommandEvent& e);
    void AddMatch(const gsgs::CppToken& token, bool check);
    void DoSelectFile(const gsgs::CppToken& token);
private:
    gsgs::CppToken::Vec_t m_tokens;
    wxString m_filename;

    wxStaticText* m_staticText31;
    wxTextCtrl* m_textCtrlNewName;
    wxSplitterWindow* m_splitter7;
    wxPanel* m_splitterPage11;
    wxDataViewListCtrl* m_dvListCtrl;
    gsgs::Button* m_buttonCheckAll;
    gsgs::Button* m_buttonUncheckAll;
    wxPanel* m_splitterPage15;
    wxStyledTextCtrl* m_preview;
    wxStaticLine* m_staticLine55;
    gsgs::Button* m_buttonOk;
    gsgs::Button* m_buttonCancel;
};

class ReconcileProjectDlg : public wxDialog
{
public:
    ReconcileProjectDlg(wxWindow* parent, const wxString& projname, wxWindowID id = wxID_ANY, const wxString& title = _("Reconcile Project"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~ReconcileProjectDlg();

    /*!
     * \brief Searches the filesystem and project to find new and stale files, then fills the dialog's fields with them
     * \return success
     */
    bool LoadData();
    void DistributeFiles(bool usingAutoallocate);

    /*!
     * \brief Removes StaleFiles from project
     * \param files the files to be added
     * \param vdPath the virtual dir to which to add them
     * \return A wxArrayString containing the items successfully removed
     */
    wxArrayString AddMissingFiles(const wxArrayString& files, const wxString& vdPath);
    /*!
     * \brief Removes StaleFiles from project
     * \param StaleFiles the items that no longer exist and that the user has chosen to remove
     * \return A wxArrayString containing the items successfully removed
     */
    wxArrayString RemoveStaleFiles(const wxArrayString& StaleFiles) const;
    
    wxBannerWindow* GetBanner270() { return m_banner270; }
    wxDataViewListCtrl* GetDvListCtrl1Unassigned() { return m_dvListCtrl1Unassigned; }
    wxBitmapButton* GetBmpButtonAdd() { return m_bmpButtonAdd; }
    wxBitmapButton* GetBmpButtonRemove() { return m_bmpButtonRemove; }
    wxBitmapButton* GetBmpButtonAutoAdd() { return m_bmpButtonAutoAdd; }
    wxPanel* GetSplitterPage1() { return m_splitterPage1; }
    wxDataViewCtrl* GetDataviewAssigned() { return m_dataviewAssigned; }
    wxPanel* GetSplitterPage2() { return m_splitterPage2; }
    wxSplitterWindow* GetSplitter() { return m_splitter; }
    gsgs::Button* GetButton274() { return m_button274; }
    gsgs::Button* GetButton280() { return m_button280; }
    wxPanel* GetPanel220() { return m_panel220; }
    wxBannerWindow* GetBanner272() { return m_banner272; }
    wxDataViewCtrl* GetDataviewStaleFiles() { return m_dataviewStaleFiles; }
    gsgs::Button* GetButton266() { return m_button266; }
    gsgs::Button* GetButton282() { return m_button282; }
    wxPanel* GetPanel222() { return m_panel222; }
    wxNotebook* GetNotebook214() { return m_notebook214; }
    gsgs::Button* GetButton258() { return m_button258; }
protected:
    void DoFindFiles();
    wxBitmap GetBitmap(const wxString& filename) const;
    bool GuessNewVirtualDirName(wxString& suggestedPath, wxString& suggestedName) const;
protected:
    virtual void OnApply(wxCommandEvent& event);
    virtual void OnApplyUI(wxUpdateUIEvent& event);
    virtual void OnApplyAllUI(wxUpdateUIEvent& event);
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnDeleteStaleFiles(wxCommandEvent& event);
    virtual void OnDeleteStaleFilesUI(wxUpdateUIEvent& event);
    virtual void OnDeleteAllStaleFilesUI(wxUpdateUIEvent& event);
    virtual void OnAddFile(wxCommandEvent& event);
    virtual void OnAddFileUI(wxUpdateUIEvent& event);
    virtual void OnAutoAssignUI(wxUpdateUIEvent& event);
    virtual void OnAutoSuggest(wxCommandEvent& event);
    virtual void OnUndoSelectedFiles(wxCommandEvent& event);
    virtual void OnUndoSelectedFilesUI(wxUpdateUIEvent& event);
    void OnDeleteSelectedNewFiles(wxCommandEvent& e);

    void OnDVLCContextMenu(wxDataViewEvent& event);

    wxNotebook* m_notebook214;
    wxPanel* m_panel220;
    wxBannerWindow* m_banner270;
    wxSplitterWindow* m_splitter;
    wxPanel* m_splitterPage1;
    wxDataViewListCtrl* m_dvListCtrl1Unassigned;
    wxBitmapButton* m_bmpButtonAdd;
    wxBitmapButton* m_bmpButtonRemove;
    wxBitmapButton* m_bmpButtonAutoAdd;
    wxPanel* m_splitterPage2;
    wxDataViewCtrl* m_dataviewAssigned;
    wxObjectDataPtr<AssignedFilesModel> m_dataviewAssignedModel;

    gsgs::Button* m_button274;
    gsgs::Button* m_button280;
    wxPanel* m_panel222;
    wxBannerWindow* m_banner272;
    wxDataViewCtrl* m_dataviewStaleFiles;
    wxObjectDataPtr<StaleFilesModel> m_dataviewStaleFilesModel;

    gsgs::Button* m_button266;
    gsgs::Button* m_button282;
    gsgs::Button* m_button258;
    wxString m_projname;
    wxArrayString m_regexes;
    gsgsStringSet m_newfiles;
    Project::FilesMap_t m_stalefiles;
    gsgsStringSet m_allfiles;
    wxString m_toplevelDir;
    bool m_projectModified;

    typedef std::multimap<wxString, wxString> StringMultimap_t;
};

class ReconcileProjectFiletypesDlg : public wxDialog
{
public:
    ReconcileProjectFiletypesDlg(wxWindow* parent, const wxString& projname, wxWindowID id = wxID_ANY, const wxString& title = _("Select filetypes to reconcile"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~ReconcileProjectFiletypesDlg();

    void SetData();
    void GetData(wxString& toplevelDir, wxString& types, wxString& ignoreFiles, wxArrayString& excludePaths,
                 wxArrayString& regexes) const;
                 
    wxStaticText* GetStaticText116() { return m_staticText116; }
    wxDirPickerCtrl* GetDirPickerToplevel() { return m_dirPickerToplevel; }
    wxStaticText* GetStaticText11617() { return m_staticText11617; }
    wxTextCtrl* GetTextExtensions() { return m_textExtensions; }
    wxStaticText* GetStaticText11618216() { return m_staticText11618216; }
    wxListBox* GetListIgnoreFiles() { return m_listIgnoreFiles; }
    gsgs::Button* GetButton11921620() { return m_button11921620; }
    gsgs::Button* GetButton125721() { return m_button125721; }
    wxStaticText* GetStaticText11618() { return m_staticText11618; }
    wxListBox* GetListExclude() { return m_listExclude; }
    gsgs::Button* GetButton11921() { return m_button11921; }
    gsgs::Button* GetButton125() { return m_button125; }
    wxStaticText* GetStaticText116182() { return m_staticText116182; }
    wxListCtrl* GetListCtrlRegexes() { return m_listCtrlRegexes; }
    gsgs::Button* GetButton119216() { return m_button119216; }
    gsgs::Button* GetButton1257() { return m_button1257; }
protected:
    void SetRegex(const wxString& regex); // Takes a VD|regex string, splits and inserts into listctrl cols
    wxArrayString GetRegexes()
        const; // Extracts the regex and VD data from listctrl, joins each to VD|regex string, and puts in arraystring
    virtual void OnIgnoreBrowse(wxCommandEvent& event);
    virtual void OnIgnoreRemove(wxCommandEvent& event);
    virtual void OnIgnoreFileBrowse(wxCommandEvent& event);
    virtual void OnIgnoreFileRemove(wxCommandEvent& event);
    virtual void OnAddRegex(wxCommandEvent& event);
    virtual void OnRemoveRegex(wxCommandEvent& event);
    virtual void OnIgnoreRemoveUpdateUI(wxUpdateUIEvent& event);
    virtual void OnIgnoreFileRemoveUpdateUI(wxUpdateUIEvent& event);
    virtual void OnRemoveRegexUpdateUI(wxUpdateUIEvent& event);

    const wxString m_projname;
protected:
    wxStaticText* m_staticText116;
    wxDirPickerCtrl* m_dirPickerToplevel;
    wxStaticText* m_staticText11617;
    wxTextCtrl* m_textExtensions;
    wxStaticText* m_staticText11618216;
    wxListBox* m_listIgnoreFiles;
    gsgs::Button* m_button11921620;
    gsgs::Button* m_button125721;
    wxStaticText* m_staticText11618;
    wxListBox* m_listExclude;
    gsgs::Button* m_button11921;
    gsgs::Button* m_button125;
    wxStaticText* m_staticText116182;
    wxListCtrl* m_listCtrlRegexes;
    gsgs::Button* m_button119216;
    gsgs::Button* m_button1257;
    DialogButtonSizer* m_stdBtnSizer120;
    gsgs::Button* m_button121;
    gsgs::Button* m_button122;
};

class ReconcileByRegexDlg : public wxDialog
{
public:
    ReconcileByRegexDlg(wxWindow* parent, const wxString& projname, wxWindowID id = wxID_ANY, const wxString& title = _("Allocate by regex"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~ReconcileByRegexDlg();

    wxString GetRegex() { return m_textCtrlVirtualFolder->GetValue() + '|' + m_textCtrlRegex->GetValue(); }

    wxStaticText* GetStaticText145()        { return m_staticText145; }
    wxTextCtrl* GetTextCtrlRegex()          { return m_textCtrlRegex; }
    wxStaticText* GetStaticText147()        { return m_staticText147; }
    wxTextCtrl* GetTextCtrlVirtualFolder()  { return m_textCtrlVirtualFolder; }
    wxBitmapButton* GetBmpButton21010()     { return m_bmpButton21010; }
protected:
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnVDBrowse(wxCommandEvent& event);
    virtual void OnRegexOKCancelUpdateUI(wxUpdateUIEvent& event);

    const wxString m_projname;
protected:
    wxStaticText* m_staticText145;
    wxTextCtrl* m_textCtrlRegex;
    wxStaticText* m_staticText147;
    wxTextCtrl* m_textCtrlVirtualFolder;
    wxBitmapButton* m_bmpButton21010;
    DialogButtonSizer* m_stdBtnSizer12011;
    gsgs::Button* m_button12112;
    gsgs::Button* m_buttonOK;
};

class SelectProjectsDlg : public wxDialog
{
public:
    enum {
        ID_CHECK_ALL = 1001,
        ID_UNCHECK_ALL = 1002,
    };

    SelectProjectsDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Rename Symbol Scope"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER);
    virtual ~SelectProjectsDlg();
    wxArrayString GetProjects() const;
    
        wxBannerWindow* GetBanner45() { return m_banner45; }
    wxCheckListBox* GetCheckListBoxProjects() { return m_checkListBoxProjects; }
    gsgs::Button* GetButton37() { return m_button37; }
    gsgs::Button* GetButton39() { return m_button39; }
    gsgs::Button* GetButton29() { return m_button29; }
    gsgs::Button* GetButton31() { return m_button31; }
protected:

    virtual void OnSelectAll(wxCommandEvent& event);
    virtual void OnUnSelectAll(wxCommandEvent& event);
protected:
    wxBannerWindow* m_banner45;
    wxCheckListBox* m_checkListBoxProjects;
    gsgs::Button* m_button37;
    gsgs::Button* m_button39;
    gsgs::Button* m_button29;
    gsgs::Button* m_button31;
};

class SettersGetterData : public ObjectData
{
public:
    enum {
        FunctionStartWithUpperCase = (1 << 0),
        FormatFileWhenDone = (1 << 1),
        SettersReturnReferenceToSelf = (1 << 2),
    };

    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;

    SettersGetterData();
    virtual ~SettersGetterData();

    void SetFlags(size_t flags) { m_flags = flags; }
    size_t GetFlags() const { return m_flags; }
private:
    size_t m_flags;
};

class SettersGettersTreeData : public wxTreeItemData
{
public:
    enum { Kind_Getter = 0, Kind_Setter, Kind_Parent, Kind_Root };

    TagEntryPtr m_tag;
    int m_kind;
    bool m_checked;

public:
    SettersGettersTreeData(TagEntryPtr tag, int kind, bool checked)
        : m_tag(tag)
        , m_kind(kind)
        , m_checked(checked)
    {
    }
    virtual ~SettersGettersTreeData() {}
};

class SettersGettersDlg : public wxDialog
{
public:
    /** Constructor */
    SettersGettersDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                          const wxString& title = _("Generate Setters/Getters for class"),
                          const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                          long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~SettersGettersDlg();
    wxString GetGenCode();
    bool GetFormatText() const { return m_checkBoxForamtFileWhenDone->IsChecked(); }

    bool Init(const std::vector<TagEntryPtr>& tags, const wxFileName& file, int lineno);
    
    wxSearchCtrl* GetSearchCtrl()               { return m_searchCtrl; }
    gsgs::ThemedListCtrl* GetDvListCtrl()       { return m_dvListCtrl; }
    gsgs::Button* GetButtonCheckAll()                 { return m_buttonCheckAll; }
    gsgs::Button* GetButtonUncheckAll()               { return m_buttonUncheckAll; }
    wxCheckBox* GetCheckStartWithUppercase()    { return m_checkStartWithUppercase; }
    wxCheckBox* GetCheckBoxForamtFileWhenDone() { return m_checkBoxForamtFileWhenDone; }
    wxCheckBox* GetCheckBoxReturnSelf()         { return m_checkBoxReturnSelf; }
protected:
    virtual void OnSettersReturnReference(wxCommandEvent& event);
    virtual void OnFilter(wxCommandEvent& event);
    virtual void OnValueChanged(wxDataViewEvent& event);
    void OnCheckStartWithUpperCase(wxCommandEvent& event);
    void OnCheckAll(wxCommandEvent& e);
    void OnUncheckAll(wxCommandEvent& e);
    void OnUpdatePreview(wxCommandEvent& e);
    void OnButtonOk(wxCommandEvent& e);
    int BuildTree();
    void UpdateTree();
    void Clear();

    wxString GenerateFunctions();
    wxString GenerateSetter(TagEntryPtr tag, bool& alreadyExist, wxString& displayName);
    wxString GenerateSetter(TagEntryPtr tag);
    wxString GenerateGetter(TagEntryPtr tag, bool& alreadyExist, wxString& displayName);
    wxString GenerateGetter(TagEntryPtr tag);
    SettersGettersTreeData* GetItemData(const wxDataViewItem& item);
    SettersGettersTreeData* GetItemData(size_t row);
    void DoCheckAll(bool checked);

    void FormatName(wxString& name);
    void UpdatePreview();
    void GenerateGetters(wxString& code);
    void GenerateSetters(wxString& code);
    bool DoCheckExistance(const wxString& scope, const wxString& name, const wxString& method_signature);
private:
    struct Candidate {
        TagEntryPtr tag;
        bool suggestGetter;
        bool suggestSetter;
        wxString setterName;
        wxString getterName;

        Candidate()
            : suggestGetter(false)
            , suggestSetter(false)
        {
        }
    };

    wxSearchCtrl* m_searchCtrl;
    gsgs::ThemedListCtrl* m_dvListCtrl;
    gsgs::Button* m_buttonCheckAll;
    gsgs::Button* m_buttonUncheckAll;
    wxCheckBox* m_checkStartWithUppercase;
    wxCheckBox* m_checkBoxForamtFileWhenDone;
    wxCheckBox* m_checkBoxReturnSelf;
    DialogButtonSizer* m_stdBtnSizer16;
    gsgs::Button* m_button18;
    gsgs::Button* m_button20;
    
    std::vector<Candidate> m_candidates;
    std::vector<TagEntryPtr> m_members;
    wxFileName m_file;
    int m_lineno;
    std::map<wxString, TagEntryPtr> m_tagsMap;
    wxString m_code;
    bool m_checkForDuplicateEntries;
    SettersGetterData m_settings;
};
#endif
