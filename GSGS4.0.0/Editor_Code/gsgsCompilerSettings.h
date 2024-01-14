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

#ifndef __advanced_settings__
#define __advanced_settings__

#include "gsgsToolsetManager.h"
#include "gsgsDebuggerManager.h"
#include "gsgsManager.h"
#include <wx/button.h>
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>
#include <wx/choicebk.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>

namespace gsgs
{
    class CompilerOptionDialog : public wxDialog
    {
    public:
        CompilerOptionDialog(wxWindow* parent, const wxString& name, const wxString& help, const wxString& title = wxT(""),
            wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsOptionDlgSize, long style = gsgsDlgStyle);
        virtual ~CompilerOptionDialog();

        wxString GetName() const { return m_textCtrl18->GetValue(); }

        wxString GetHelp() const { return m_textCtrl19->GetValue(); }

        wxStaticText* GetStaticText26() { return m_staticText26; }
        wxTextCtrl* GetTextCtrl18() { return m_textCtrl18; }
        wxStaticText* GetStaticText27() { return m_staticText27; }
        wxTextCtrl* GetTextCtrl19() { return m_textCtrl19; }
        Button* GetButtonOK() { return m_buttonOK; }
        Button* GetButtonCancel() { return m_buttonCancel; }
    protected:
        wxStaticText* m_staticTextTitle;
        wxStaticText* m_staticText26;
        wxTextCtrl* m_textCtrl18;
        wxStaticText* m_staticText27;
        wxTextCtrl* m_textCtrl19;
        Button* m_buttonOK;
        Button* m_buttonCancel;
    };

    class CompilerSelectDialog : public wxDialog
    {
    public:
        CompilerSelectDialog(wxWindow* parent, const wxString& name, const wxString& help, const wxString& title = wxT(""),
            wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsOptionDlgSize, long style = gsgsDlgStyle);
        virtual ~CompilerSelectDialog();

        wxString GetName() const    { return m_textCtrl18->GetValue(); }

        wxString GetHelp() const    { return m_textCtrl19->GetValue(); }

        void SetTreeCtrlEvent(TreeCtrl * src);
        wxStaticText* GetStaticText26() { return m_staticText26; }
        wxTextCtrl* GetTextCtrl18() { return m_textCtrl18; }
        wxStaticText* GetStaticText27() { return m_staticText27; }
        wxTextCtrl* GetTextCtrl19() { return m_textCtrl19; }
        Button* GetButtonOK() { return m_buttonOK; }
        Button* GetButtonCancel() { return m_buttonCancel; }

        void OnCustomEditorButtonClicked(wxCommandEvent& event);
    protected:
        wxStaticText* m_staticTextTitle;
        wxStaticText* m_staticText26;
        TreeCtrl * mSrc;
        wxTextCtrl* m_textCtrl18;
        wxStaticText* m_staticText27;
        wxTextCtrl* m_textCtrl19;
        Button* m_buttonPick;
        Button* m_buttonOK;
        Button* m_buttonCancel;
    };

    class CompilerPatternDlg : public wxDialog
    {
    public:
        CompilerPatternDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT(""), 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsOptionDlgSize, long style = gsgsDlgStyle);
        virtual ~CompilerPatternDlg();

        void SetPattern(const wxString& pattern, const wxString& lineIdx, const wxString& fileIdx, const wxString& columnIndex);

        wxString GetPattern() const { return m_textPattern->GetValue(); }
        wxString GetFileIndex() const { return m_textFileIndex->GetValue(); }
        wxString GetLineIndex() const { return m_textLineNumber->GetValue(); }
        wxString GetColumnIndex() const { return m_textColumn->GetValue(); }

        wxStaticText* GetStaticText5() { return m_staticText5; }
        wxTextCtrl* GetTextPattern() { return m_textPattern; }
        wxStaticText* GetStaticText6() { return m_staticText6; }
        wxTextCtrl* GetTextFileIndex() { return m_textFileIndex; }
        wxStaticText* GetStaticText7() { return m_staticText7; }
        wxTextCtrl* GetTextLineNumber() { return m_textLineNumber; }
        wxStaticText* GetStaticText235() { return m_staticText235; }
        wxTextCtrl* GetTextColumn() { return m_textColumn; }
    protected:
        virtual void OnSubmit(wxCommandEvent& event);
    protected:
        wxStaticText* m_staticText5;
        wxTextCtrl* m_textPattern;
        wxStaticText* m_staticText6;
        wxTextCtrl* m_textFileIndex;
        wxStaticText* m_staticText7;
        wxTextCtrl* m_textLineNumber;
        wxStaticText* m_staticText235;
        wxTextCtrl* m_textColumn;
        DialogButtonSizer* m_stdBtnSizer229;
        Button* m_button231;
        Button* m_button233;
    };

    class NewCompilerDlg : public wxDialog
    {
    public:
        NewCompilerDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Compiler"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsMinDlgSize, long style = gsgsDlgStyle);
        virtual ~NewCompilerDlg();
        wxStaticText* GetStaticText84() { return m_staticText84; }
        wxTextCtrl* GetTextCtrlCompilerName() { return m_textCtrlCompilerName; }
        wxStaticText* GetStaticText88() { return m_staticText88; }
        wxChoice* GetChoiceCompilers() { return m_choiceCompilers; }

        wxString GetCompilerName() const {
            return m_textCtrlCompilerName->GetValue();
        }

        wxString GetMasterCompiler() const;
    protected:
        virtual void OnOkUI(wxUpdateUIEvent& event);
    protected:
        wxStaticText* m_staticText84;
        wxTextCtrl* m_textCtrlCompilerName;
        wxStaticText* m_staticText88;
        wxChoice* m_choiceCompilers;
        DialogButtonSizer* m_stdBtnSizer75;
        Button* m_buttonCancel;
        Button* m_buttonOK;
    };

    class CompilerMainPage : public wxPanel
    {
    public:
        /*enum {
            ID_TOOL_COPY_COMPILER = 8501,
            ID_TOOL_NEW_COMPILER = 8502,
            ID_TOOL_SCAN_COMPILERS = 8503,
        };*/

    public:
        CompilerMainPage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~CompilerMainPage();

        void LoadCompilers();

        void Save();

        bool IsDirty() const { return m_isDirty; }

        wxAuiToolBar* GetAuibar241() const              { return m_auibar241; }
        wxPanel* GetPanelTools() const                  { return m_panelTools; }
        wxStaticText* GetStaticText161() const          { return m_staticText161; }
        Button* GetBtnAddErrPattern() const             { return m_btnAddErrPattern; }
        Button* GetBtnDelErrPattern() const             { return m_btnDelErrPattern; }
        Button* GetBtnUpdateErrPattern() const          { return m_btnUpdateErrPattern; }
        wxStaticText* GetStaticText17() const           { return m_staticText17; }
        Button* GetBtnAddWarnPattern() const            { return m_btnAddWarnPattern; }
        Button* GetBtnDelWarnPattern() const            { return m_btnDelWarnPattern; }
        Button* GetBtnUpdateWarnPattern() const         { return m_btnUpdateWarnPattern; }
        wxPanel* GetPanelPatterns() const               { return m_panelPatterns; }
        Button* GetButtonCompilerOption() const         { return m_buttonCompilerOption; }
        Button* GetButtonDeleteCompilerOption() const   { return m_buttonDeleteCompilerOption; }
        wxPanel* GetPanelComilerOptions() const         { return m_panelComilerOptions; }
        wxStaticText* GetStaticText23_L() const         { return m_staticText23_L; }
        Button* GetButtonLinkerOption() const           { return m_buttonLinkerOption; }
        Button* GetButtonDeleteLinkerOption() const     { return m_buttonDeleteLinkerOption; }
        wxPanel* GetPanelLinkerOptions() const          { return m_panelLinkerOptions; }
        wxStaticText* GetStaticText23() const           { return m_staticText23; }
        Button* GetButtonNewFileType() const            { return m_buttonNewFileType; }
        Button* GetButtonDeleteFileType() const         { return m_buttonDeleteFileType; }
        wxStaticText* GetStaticText18() const           { return m_staticText18; }
        wxStaticText* GetStaticText141() const          { return m_staticText141; }
        wxTextCtrl* GetTextCtrlGlobalIncludePath() const { return m_textCtrlGlobalIncludePath; }
        Button* GetButton67() const                     { return m_button67; }
        wxStaticText* GetStaticText16() const           { return m_staticText16; }
        wxTextCtrl* GetTextCtrlGlobalLibPath() const    { return m_textCtrlGlobalLibPath; }
        Button* GetButton69() const                     { return m_button69; }
        wxStaticText* GetStaticText19() const           { return m_staticText19; }
        wxStaticText* GetStaticText3() const            { return m_staticText3; }
        wxTextCtrl* GetTextObjectExtension()  const     { return m_textObjectExtension; }
        wxStaticText* GetStaticText24() const           { return m_staticText24; }
        wxTextCtrl* GetTextDependExtension() const      { return m_textDependExtension; }
        wxStaticText* GetStaticText25()  const          { return m_staticText25; }
        wxTextCtrl* GetTextPreprocessExtension() const  { return m_textPreprocessExtension; }
        wxCheckBox* GetCheckBoxGenerateDependenciesFiles() { return m_checkBoxGenerateDependenciesFiles; }
        wxCheckBox* GetCheckBoxObjectNameSameAsFileName() { return m_checkBoxObjectNameSameAsFileName; }
        wxCheckBox* GetCheckBoxReadObjectsFromFile() { return m_checkBoxReadObjectsFromFile; }
        wxPanel* GetPanelAdvanced() const               { return m_panelAdvanced; }
        Notebook* GetAuiBook() const                    { return m_auiBook; }
    protected:
        //virtual void OnAddExistingCompiler(wxCommandEvent& event);
        virtual void OnCloneCompiler(wxCommandEvent& event);
        virtual void OnScanCompilers(wxCommandEvent& event);
        virtual void OnCmdModify(wxCommandEvent& event);
        //virtual void OnValueChanged(wxPropertyGridEvent& event);
        virtual void OnRenameCompiler(wxCommandEvent& event);
        //virtual void OnDeleteCompiler(wxCommandEvent& event);
        virtual void OnContextMenu(wxTreeEvent& event);
        virtual void OnCompilerSelected(wxTreeEvent& event);

        void Initialize();
        // Tools
        void InitializeToolset();
        void SaveToolset();

        // Patterns
        void InitializePatterns();
        void SavePatterns();
        void DoUpdateErrPattern();
        void DoUpdateWarnPattern();

        // File Types
        void InitializeFileTypes();
        void SaveFileTypes();

        // Advanced page
        void InitializeAdvancePage();
        void SaveAdvancedPage();

        // Compiler options
        void InitializeCompilerOptions();
        void SaveComilerOptions();

        // Compiler options
        void InitializeLinkerOptions();
        void SaveLinkerOptions();

        void LoadCompiler(const wxString& compilerName);
    protected:
        virtual void OnBtnAddErrPattern(wxCommandEvent& event);
        virtual void OnBtnAddWarnPattern(wxCommandEvent& event);
        virtual void OnBtnDelErrPattern(wxCommandEvent& event);
        virtual void OnBtnDelWarnPattern(wxCommandEvent& event);
        virtual void OnBtnUpdateErrPattern(wxCommandEvent& event);
        virtual void OnBtnUpdateWarnPattern(wxCommandEvent& event);
        virtual void OnCompilerOptionActivated(wxTreeEvent& event);
        //virtual void OnCompilerOptionDeSelected(wxListEvent& event);
        //virtual void OnCompilerOptionSelected(wxListEvent& event);
        //virtual void OnCustomEditorButtonClicked(wxCommandEvent& event);
        virtual void OnDeleteCompilerOption(wxCommandEvent& event);
        virtual void OnDeleteFileType(wxCommandEvent& event);
        virtual void OnDeleteLinkerOption(wxCommandEvent& event);
        virtual void OnEditIncludePaths(wxCommandEvent& event);
        virtual void OnEditLibraryPaths(wxCommandEvent& event);
        virtual void OnErrItemActivated(wxTreeEvent& event);
        virtual void OnErrorPatternSelectedUI(wxUpdateUIEvent& event);
        virtual void OnFileTypeActivated(wxTreeEvent& event);
        //virtual void OnFileTypeDeSelected(wxListEvent& event);
        //virtual void OnFileTypeSelected(wxListEvent& event);
        virtual void OnItemActivated(wxTreeEvent& event);
        //virtual void OnItemSelected(wxListEvent& event);
        virtual void OnLinkerOptionActivated(wxTreeEvent& event);
        //virtual void OnLinkerOptionDeSelected(wxListEvent& event);
        //virtual void OnLinkerOptionSelected(wxListEvent& event);
        virtual void OnNewCompilerOption(wxCommandEvent& event);
        virtual void OnNewCompile(wxCommandEvent& event);
        virtual void OnDeleteCompile(wxCommandEvent& event);
        virtual void OnNewFileType(wxCommandEvent& event);
        virtual void OnNewLinkerOption(wxCommandEvent& event);
        virtual void OnWarnItemActivated(wxTreeEvent& event);
        virtual void OnWarningPatternSelectedUI(wxUpdateUIEvent& event);
    protected:
        wxAuiToolBar* m_auibar241;
        TreeCtrl* m_listCompilers;
        Notebook* m_auiBook;
        wxPanel* m_panelTools;
        wxTreeItemId m_pgPropCXX;
        wxTreeItemId m_pgPropCC;
        wxTreeItemId m_pgPropAS;
        wxTreeItemId m_pgPropLD;
        wxTreeItemId m_pgPropSharedObjectLD;
        wxTreeItemId m_pgPropAR;
        wxTreeItemId m_pgPropResourceCompiler;
        wxTreeItemId m_pgPropMAKE;
        wxTreeItemId m_pgPropMkdir;
        wxTreeItemId m_pgPropDebugger;
        wxPanel* m_panelPatterns;
        wxStaticText* m_staticText161;
        TreeCtrl* m_listErrPatterns;
        Button* m_btnAddErrPattern;
        Button* m_btnDelErrPattern;
        Button* m_btnUpdateErrPattern;
        wxStaticText* m_staticText17;
        TreeCtrl* m_listWarnPatterns;
        Button* m_btnAddWarnPattern;
        Button* m_btnDelWarnPattern;
        Button* m_btnUpdateWarnPattern;
        wxPanel* m_panelComilerOptions;
        TreeCtrl * m_listCompilerOptions;
        Button* m_buttonCompilerOption;
        Button* m_buttonDeleteCompilerOption;
        wxPanel* m_panelLinkerOptions;
        wxStaticText* m_staticText23_L;
        TreeCtrl* m_listLinkerOptions;
        Button* m_buttonLinkerOption;
        Button* m_buttonDeleteLinkerOption;
        wxStaticText* m_staticText8;
        TreeCtrl* m_listSwitches;
        wxStaticText* m_staticText23;
        TreeCtrl * m_listCtrlFileTypes;
        Button* m_buttonNewCompiler;
        Button* m_buttonDeleteCompiler;
        Button* m_buttonNewFileType;
        Button* m_buttonDeleteFileType;
        wxPanel* m_panelAdvanced;
        wxStaticText* m_staticText18;
        wxStaticText* m_staticText141;
        wxTextCtrl* m_textCtrlGlobalIncludePath;
        Button* m_button67;
        wxStaticText* m_staticText16;
        wxTextCtrl* m_textCtrlGlobalLibPath;
        Button* m_button69;
        wxStaticText* m_staticText19;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textObjectExtension;
        wxStaticText* m_staticText24;
        wxTextCtrl* m_textDependExtension;
        wxStaticText* m_staticText25;
        wxTextCtrl* m_textPreprocessExtension;
        wxCheckBox* m_checkBoxGenerateDependenciesFiles;
        wxCheckBox* m_checkBoxObjectNameSameAsFileName;
        wxCheckBox* m_checkBoxReadObjectsFromFile;
        CompilerPtr m_compiler;
        bool m_isDirty;
    };
    class BuildTabSetting : public wxPanel
    {
    public:
        wxPropertyGridManager* GetPgMgr() { return m_pgMgr; }
        BuildTabSetting(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~BuildTabSetting();

        void SetIsModified(bool isModified) { this->m_isModified = isModified; }
        bool IsModified() const { return m_isModified; }

        void SelectFont();
        void Save();

    protected:
        void OnUpdateUI(wxUpdateUIEvent& event);

        virtual void OnCustomButtonClicked(wxCommandEvent& event);
        virtual void OnAppearanceChanged(wxPropertyGridEvent& event);
    protected:
        wxPropertyGridManager* m_pgMgr;
        wxPGProperty* CAT_COLOURS_AND_FONTS;
        wxPGProperty* m_pgPropFont;
        wxPGProperty* m_pgPropErrorColour;
        wxPGProperty* m_pgPropWarningColour;
        wxPGProperty* CAT_GENERAL;
        wxPGProperty* m_pgPropJumpWarnings;
        wxPGProperty* m_pgPropAutoHideBuildPane;
        wxPGProperty* m_pgPropAutoShowBuildPane;
        wxPGProperty* m_pgPropAutoScroll;
        wxPGProperty* CAT_MARKERS;
        wxPGProperty* m_pgPropUseMarkers;
        wxPGProperty* m_pgPropUseAnnotations;
        bool m_isModified;
    };

    class BuildPage : public wxPanel
    {
    public:
        BuildPage(wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize(500, 300), int style = wxTAB_TRAVERSAL);
        void Save();
    protected:
        void CustomInit();
        wxPanel *CreateBuildSystemPage(const wxString &name);
    private:
        wxStaticText * m_staticText;
        wxChoicebook * m_bookBuildSystems;
        wxCheckBox * m_fixOnStartup;
    };

    class DebuggerPageStartupCmds : public wxPanel
    {
        friend class BuildSettingsDialog;
    public:
        DebuggerPageStartupCmds(wxWindow* parent, const wxString& title, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);

        virtual ~DebuggerPageStartupCmds();

        wxStyledTextCtrl* GetTextCtrlStartupCommands()      { return m_textCtrlStartupCommands; }
    protected:
        wxStyledTextCtrl* m_textCtrlStartupCommands;
        wxString m_title;
    };

    class PreDefinedTypesPage : public wxPanel
    {
    public:
        PreDefinedTypesPage(wxWindow* parent, const DebuggerPreDefinedTypes& preDefTypes, 
            wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~PreDefinedTypesPage();

        DebuggerPreDefinedTypes GetPreDefinedTypes();
        wxListCtrl* GetListCtrl1() const                    { return m_listCtrl1; }
        Button* GetButtonNewType() const                    { return m_buttonNewType; }
        Button* GetButtonEdit() const                       { return m_buttonEdit; }
        Button* GetButtonDelete() const                     { return m_buttonDelete; }
        wxPanel* GetPanel2() const                          { return m_panel2; }
    protected:
        virtual void OnItemActivated(wxListEvent& event);
        virtual void OnItemDeselected(wxListEvent& event);
        virtual void OnItemSelected(wxListEvent& event);
        virtual void OnNewShortcut(wxCommandEvent& event);
        virtual void OnEditShortcut(wxCommandEvent& event);
        virtual void OnDeleteShortcut(wxCommandEvent& event);

        void DoEditItem();
        void DoDeleteItem();
    protected:
        wxPanel* m_panel2;
        wxListCtrl* m_listCtrl1;
        Button* m_buttonNewType;
        Button* m_buttonEdit;
        Button* m_buttonDelete;
        long m_selectedItem;
        DebuggerPreDefinedTypes m_data;
    };

    class NewPreDefinedSetDlg : public wxDialog
    {
    public:
        NewPreDefinedSetDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
            const wxString& title = _("Create a new 'PreDefined Types' set..."),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsMinDlgSize,
            long style = wxSIMPLE_BORDER);
        virtual ~NewPreDefinedSetDlg();

        wxStaticText* GetStaticText3() const            { return m_staticText3; }
        wxTextCtrl* GetTextCtrlName() const             { return m_textCtrlName; }
        wxStaticText* GetStaticText4() const            { return m_staticText4; }
        wxChoice* GetChoiceCopyFrom() const             { return m_choiceCopyFrom; }
        wxCheckBox* GetCheckBoxMakeActive() const       { return m_checkBoxMakeActive; }
        Button* GetButton9() const                      { return m_button9; }
        Button* GetButton10() const                     { return m_button10; }
    protected:
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrlName;
        wxStaticText* m_staticText4;
        wxChoice* m_choiceCopyFrom;
        wxCheckBox* m_checkBoxMakeActive;
        Button* m_button9;
        Button* m_button10;
    };

    class DebuggerPage : public wxPanel
    {
        friend class BuildSettingsDialog;
    public:
        DebuggerPage(wxWindow* parent, const wxString & title, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~DebuggerPage();

        wxStaticText* GetStaticText1()                      { return m_staticText1; }
        wxTextCtrl* GetTextCtrDbgPath()                     { return m_textCtrDbgPath; }
        Button* GetButtonBrowse()                           { return m_buttonBrowse; }
        wxCheckBox* GetCheckBoxEnablePendingBreakpoints()   { return m_checkBoxEnablePendingBreakpoints; }
        wxCheckBox* GetCheckBoxSetBreakpointsAfterMain()    { return m_checkBoxSetBreakpointsAfterMain; }
        wxCheckBox* GetCheckBreakAtWinMain()                { return m_checkBreakAtWinMain; }
        wxCheckBox* GetCatchThrow()                         { return m_catchThrow; }
        wxCheckBox* GetRaiseOnBpHit()                       { return m_raiseOnBpHit; }
        wxCheckBox* GetCheckBoxRunAsSuperuser()             { return m_checkBoxRunAsSuperuser; }
        wxCheckBox* GetCheckBoxDefaultHexDisplay()          { return m_checkBoxDefaultHexDisplay; }
        wxPanel* GetPanelGeneral()                          { return m_panelGeneral; }
        wxCheckBox* GetShowTooltipsRequiresControl()        { return m_showTooltipsRequiresControl; }
        wxCheckBox* GetCheckBoxAutoExpand()                 { return m_checkBoxAutoExpand; }
        wxPanel* GetPanelTooltip()                          { return m_panelTooltip; }
        wxStaticText* GetStaticText2()                      { return m_staticText2; }
        wxSpinCtrl* GetSpinCtrlNumElements()                { return m_spinCtrlNumElements; }
        wxCheckBox* GetCheckBoxExpandLocals()               { return m_checkBoxExpandLocals; }
        wxCheckBox* GetCheckBoxCharArrAsPtr()               { return m_checkBoxCharArrAsPtr; }
        wxCheckBox* GetCheckBoxUsePrettyPrinting()          { return m_checkBoxUsePrettyPrinting; }
        wxCheckBox* GetCheckBoxPrintObjectOn()              { return m_checkBoxPrintObjectOn; }
        wxPanel* GetPanelDisplay()                          { return m_panelDisplay; }
        Notebook* GetNotebook73()                           { return m_notebook73; }
        wxPanel* GetPanel6()                                { return m_panel6; }
    protected:
        virtual void OnBrowse(wxCommandEvent& event);
        virtual void OnSuperuserUI(wxUpdateUIEvent& event);
        virtual void OnDebugAssert(wxCommandEvent& e);
        virtual void OnWindowsUI(wxUpdateUIEvent& event);

    protected:
        wxPanel* m_panel6;
        Notebook* m_notebook73;
        wxPanel* m_panelGeneral;
        wxStaticText* m_staticText1;
        wxTextCtrl* m_textCtrDbgPath;
        Button* m_buttonBrowse;
        wxCheckBox* m_checkBoxEnablePendingBreakpoints;
        wxCheckBox* m_checkBoxSetBreakpointsAfterMain;
        wxCheckBox* m_checkBreakAtWinMain;
        wxCheckBox* m_catchThrow;
        wxCheckBox* m_raiseOnBpHit;
        wxCheckBox* m_checkBoxRunAsSuperuser;
        wxCheckBox* m_checkBoxDefaultHexDisplay;
        wxPanel* m_panelTooltip;
        wxCheckBox* m_showTooltipsRequiresControl;
        wxCheckBox* m_checkBoxAutoExpand;
        wxPanel* m_panelDisplay;
        wxStaticText* m_staticText2;
        wxSpinCtrl* m_spinCtrlNumElements;
        wxCheckBox* m_checkBoxExpandLocals;
        wxCheckBox* m_checkBoxCharArrAsPtr;
        wxCheckBox* m_checkBoxUsePrettyPrinting;
        wxCheckBox* m_checkBoxPrintObjectOn;
        wxString m_title;
    };

    class DebuggerPageMisc : public wxPanel
    {
        friend class BuildSettingsDialog;
    public:
        DebuggerPageMisc(wxWindow* parent, const wxString& title, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~DebuggerPageMisc();

        wxCheckBox* GetCheckUseRelativePaths() const        { return m_checkUseRelativePaths; }
        wxCheckBox* GetCheckBoxEnableLog() const            { return m_checkBoxEnableLog; }
        wxCheckBox* GetCheckShowTerminal() const            { return m_checkShowTerminal; }
        wxStaticText* GetStaticText6() const                { return m_staticText6; }
        wxSpinCtrl* GetMaxFramesSpinCtrl() const            { return m_maxFramesSpinCtrl; }
        wxCheckBox* GetCheckBoxDebugAssert() const          { return m_checkBoxDebugAssert; }
        wxStaticText* GetStaticText5() const                { return m_staticText5; }
        wxTextCtrl* GetTextCtrlCygwinPathCommand() const    { return m_textCtrlCygwinPathCommand; }
        wxPanel* GetPanel7() const                          { return m_panel7; }
    protected:
        virtual void OnDebugAssert(wxCommandEvent& event);
        virtual void OnWindowsUI(wxUpdateUIEvent& event);
    protected:
        wxString m_title;
        wxPanel* m_panel7;
        wxCheckBox* m_checkUseRelativePaths;
        wxCheckBox* m_checkBoxEnableLog;
        wxCheckBox* m_checkShowTerminal;
        wxStaticText* m_staticText6;
        wxSpinCtrl* m_maxFramesSpinCtrl;
        wxCheckBox* m_checkBoxDebugAssert;
        wxStaticText* m_staticText5;
        wxTextCtrl* m_textCtrlCygwinPathCommand;
    };

    class DbgPagePreDefTypes : public wxPanel
    {
        friend class BuildSettingsDialog;
    public:
        DbgPagePreDefTypes(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~DbgPagePreDefTypes();

        Button* GetButtonNewSet()                         { return m_buttonNewSet; }
        Button* GetButtonDeleteSet()                      { return m_buttonDeleteSet; }
        wxChoicebook* GetNotebookPreDefTypes()              { return m_notebookPreDefTypes; }
        wxPanel* GetPanel6()                                { return m_panel6; }

        void Save();
    protected:
        virtual void OnNewSet(wxCommandEvent& event);
        virtual void OnDeleteSet(wxCommandEvent& event);
        virtual void OnDeleteSetUI(wxUpdateUIEvent& event);
        virtual void OnPageChanged(wxChoicebookEvent& event) { event.Skip(); }
    protected:
        wxPanel* m_panel6;
        Button* m_buttonNewSet;
        Button* m_buttonDeleteSet;
        wxChoicebook* m_notebookPreDefTypes;
    };

    class DbgTreeItemData : public wxTreeItemData
    {
    public:
        enum {
            Locals = 0x00000001,
            FuncArgs = 0x00000002,
            VariableObject = 0x00000004,
            Watch = 0x00000010,
            FuncRetValue = 0x00000020
        };

        DbgTreeItemData() : _kind(Locals), _isFake(false){}

        DbgTreeItemData(const wxString& gdbId) : _gdbId(gdbId), _isFake(false){}

        virtual ~DbgTreeItemData(){}
    public:
        wxString _gdbId;
        wxString _retValueGdbValue;
        size_t _kind;
        bool _isFake;
    };

    class CompilerPage
    {
    public:
        CompilerPage() {}
        virtual ~CompilerPage() {}
        virtual void Save(CompilerPtr cmp) = 0;
    };

    class BuildSettingsDialog : public wxDialog
    {
    public:
        BuildSettingsDialog(wxWindow* parent, size_t selected_page, wxWindowID id = wxID_ANY, 
            const wxString& title = _("Toolset"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsDlgSize,
            long style = wxSIMPLE_BORDER);
        virtual ~BuildSettingsDialog();

        Notebook* GetNotebook() const                       { return m_notebook; }

        void SetCurrentCompiler(const wxString & name)      { mCurrentCompiler = name; }
        void OnButtonNewClicked();
        void OnScanAndSuggestCompilers();
        void OnAddExistingCompiler();
        void OnContextMenu(wxContextMenuEvent& e);

        void LoadCompilers();
        bool CreateNewCompiler(const wxString& name, const wxString& copyFrom);
        bool DeleteCompiler(const wxString& name);
        void SaveCompilers();
        void OnCompilersDetected(const ToolsetLocator::CompilerVec_t& compilers);

        void OnAutoDetectCompilers(Button* btn);
    protected:
        virtual void OnButtonOKClicked(wxCommandEvent& event);
        virtual void OnApply(wxCommandEvent& event);
        virtual void OnApplyUI(wxUpdateUIEvent& event);
    protected:
        std::vector<wxWindow*> m_pages;
        std::map<wxString, std::vector<CompilerPage*> > m_compilerPagesMap;
        wxString mCurrentCompiler;
        Notebook* m_notebook;
        DialogButtonSizer* m_stdBtnSizer2;
        Button* m_buttonCancel;
        Button* m_buttonOK;
        Button* m_buttonApply;
        wxPanel* m_compilersMainPanel;
        CompilerMainPage* m_compilersPage;

        wxStaticText* m_staticText1;
        wxStaticLine* m_staticline2;
        wxStaticLine* m_staticline10;
        BuildPage* m_buildPage;
        BuildTabSetting* m_buildSettings;
        wxMenu* m_rightclickMenu;
        ToolsetManager m_compilersDetector;
    };
}
#endif
