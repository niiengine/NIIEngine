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

#include "gsgsCompilerSettings.h"
#include "gsgsWindowManager.h"
#include "gsgsToolsetManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsBuildConfig.h"
#include "gsgsBuildTab.h"
#include "gsgsBuilderGnuMake.h"
#include "CommonController.h"
#include "gsgsFrame.h"
#include "globals.h"
#include "gsgsManager.h"
#include <wx/fontdlg.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>

// Declare the bitmap loading function
extern void wxCAD0CInitBitmapResources();
namespace gsgs
{
    static bool bBitmapLoaded = false;
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // CompilerOptionDialog
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    CompilerOptionDialog::CompilerOptionDialog(wxWindow* parent, const wxString& name, const wxString& help,
        const wxString& title, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        wxWindowUpdateLocker llt(this);
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxCAD0CInitBitmapResources();
            bBitmapLoaded = true;
        }
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizer23 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer23);

        m_staticTextTitle = new wxStaticText(this, wxID_ANY, title + " : " + name, wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticTextTitle->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        m_staticTextTitle->SetFont(gsgsTitleFont);
        bSizer23->Add(m_staticTextTitle, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 10);

        wxFlexGridSizer* fgSizer7 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer7->SetFlexibleDirection(wxBOTH);
        fgSizer7->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer7->AddGrowableCol(1);

        bSizer23->Add(fgSizer7, 0, wxALL | wxEXPAND, 5);

        m_staticText26 = gsgs_Style().createStaticText(this, wxID_ANY, _("Value :"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer7->Add(m_staticText26, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl18 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer7->Add(m_textCtrl18, 0, wxALL | wxEXPAND, 5);

        m_staticText27 = gsgs_Style().createStaticText(this, wxID_ANY, _("Help:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer7->Add(m_staticText27, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl19 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer7->Add(m_textCtrl19, 0, wxALL | wxEXPAND, 5);

        bSizer23->Add(0, 0, 1, wxEXPAND, 5);

        wxBoxSizer* bSizer24 = new wxBoxSizer(wxHORIZONTAL);

        bSizer23->Add(bSizer24, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_buttonOK = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonOK->SetDefault();

        bSizer24->Add(m_buttonOK, 0, wxALL, 5);

        m_buttonCancel = new Button(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(-1, -1), 0);

        bSizer24->Add(m_buttonCancel, 0, wxALL, 5);

        m_textCtrl18->ChangeValue(name);
        m_textCtrl19->ChangeValue(help);

        if (GetParent()) {
            CentreOnParent();
        }
        else {
            CentreOnScreen();
        }
        Layout();
    }
    //-----------------------------------------------------------------------------
    CompilerOptionDialog::~CompilerOptionDialog()
    {
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // CompilerSelectDialog
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    CompilerSelectDialog::CompilerSelectDialog(wxWindow* parent, const wxString& name, const wxString& help,
        const wxString& title, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style),
        mSrc(0)
    {
        wxWindowUpdateLocker llt(this);
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxCAD0CInitBitmapResources();
            bBitmapLoaded = true;
        }
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizer23 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer23);

        m_staticTextTitle = new wxStaticText(this, wxID_ANY, title + " : " + name, wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticTextTitle->SetForegroundColour(gsgs_Style().GetActiveMarkerColour());
        m_staticTextTitle->SetFont(gsgsTitleFont);
        bSizer23->Add(m_staticTextTitle, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 10);

        wxFlexGridSizer* fgSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
        fgSizer7->SetFlexibleDirection(wxBOTH);
        fgSizer7->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer7->AddGrowableCol(1);

        bSizer23->Add(fgSizer7, 0, wxALL | wxEXPAND, 5);

        m_staticText26 = gsgs_Style().createStaticText(this, wxID_ANY, _("Value :"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer7->Add(m_staticText26, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl18 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer7->Add(m_textCtrl18, 0, wxALL | wxEXPAND, 5);

        m_buttonPick = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer7->Add(m_buttonPick, 0, wxUP | wxDOWN | wxRIGHT | wxEXPAND, 5);

        m_staticText27 = gsgs_Style().createStaticText(this, wxID_ANY, _("Help:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer7->Add(m_staticText27, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl19 = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer7->Add(m_textCtrl19, 0, wxALL | wxEXPAND, 5);

        fgSizer7->AddSpacer(10);

        bSizer23->Add(0, 0, 1, wxEXPAND, 5);

        wxBoxSizer* bSizer24 = new wxBoxSizer(wxHORIZONTAL);

        bSizer23->Add(bSizer24, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_buttonOK = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonOK->SetDefault();

        bSizer24->Add(m_buttonOK, 0, wxALL, 5);

        m_buttonCancel = new Button(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(-1, -1), 0);

        bSizer24->Add(m_buttonCancel, 0, wxALL, 5);

        m_textCtrl18->ChangeValue(name);
        m_textCtrl19->ChangeValue(help);

        if (GetParent()) {
            CentreOnParent();
        }
        else {
            CentreOnScreen();
        }
        Layout();
    }
    //-----------------------------------------------------------------------------
    CompilerSelectDialog::~CompilerSelectDialog()
    {
        if (mSrc)
        {
            m_buttonPick->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &CompilerSelectDialog::OnCustomEditorButtonClicked, this);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerSelectDialog::SetTreeCtrlEvent(TreeCtrl * src)
    {
        if (src)
        {
            mSrc = src;
            m_buttonPick->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CompilerSelectDialog::OnCustomEditorButtonClicked, this);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerSelectDialog::OnCustomEditorButtonClicked(wxCommandEvent& event)
    {
        wxTreeItemId sle = mSrc->GetSelection();
        if (sle.IsOk())
        {
            wxFileName fn(mSrc->GetItemText(sle, 1));

            wxString newPath = ::wxFileSelector(_("Select a file"), fn.GetPath());
            if (newPath.size()) {
                ::WrapWithQuotes(newPath);
                mSrc->SetItemText(sle, newPath, 1);
            }
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // CompilerPatternDlg
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    CompilerPatternDlg::CompilerPatternDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        wxWindowUpdateLocker tttl(this);
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxCAD0CInitBitmapResources();
            bBitmapLoaded = true;
        }
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizerError = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizerError);

        wxFlexGridSizer* fgSizer41 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer41->SetFlexibleDirection(wxBOTH);
        fgSizer41->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer41->AddGrowableCol(1);

        bSizerError->Add(fgSizer41, 0, wxALL | wxEXPAND, 5);

        m_staticText5 = gsgs_Style().createStaticText(this, wxID_ANY, _("Regex Pattern:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer41->Add(m_staticText5, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textPattern = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textPattern->SetToolTip(_("Compiler regular expression"));

        fgSizer41->Add(m_textPattern, 1, wxALL | wxEXPAND, 5);

        m_staticText6 = gsgs_Style().createStaticText(this, wxID_ANY, _("File Index in Pattern:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer41->Add(m_staticText6, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textFileIndex = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textFileIndex->SetToolTip(_("The capture index in the regex that holds the file path"));

        fgSizer41->Add(m_textFileIndex, 0, wxALL | wxEXPAND, 5);

        m_staticText7 = gsgs_Style().createStaticText(this, wxID_ANY, _("Line Number in Pattern:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer41->Add(m_staticText7, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textLineNumber = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textLineNumber->SetToolTip(_("The capture index in the regex that holds the line number"));

        fgSizer41->Add(m_textLineNumber, 0, wxALL | wxEXPAND, 5);

        m_staticText235 = gsgs_Style().createStaticText(this, wxID_ANY, _("Column Index in Pattern:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer41->Add(m_staticText235, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textColumn = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textColumn->SetToolTip(_("The capture index in the regex that holds the column number"));

        fgSizer41->Add(m_textColumn, 0, wxALL | wxEXPAND, 5);

        bSizerError->Add(0, 0, 1, wxEXPAND, 5);

        m_stdBtnSizer229 = new DialogButtonSizer();

        bSizerError->Add(m_stdBtnSizer229, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_button231 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_button231->SetDefault();
        m_stdBtnSizer229->AddButton(m_button231);

        m_button233 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_stdBtnSizer229->AddButton(m_button233);
        m_stdBtnSizer229->Realize();

        if (GetParent()) {
            CentreOnParent();
        }
        else {
            CentreOnScreen();
        }
        Layout();
        // Connect events
        m_button231->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerPatternDlg::OnSubmit), NULL, this);
    }
    //-----------------------------------------------------------------------------
    CompilerPatternDlg::~CompilerPatternDlg()
    {
        m_button231->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerPatternDlg::OnSubmit), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void CompilerPatternDlg::SetPattern(const wxString& pattern,
        const wxString& lineIdx, const wxString& fileIdx, const wxString& columnIndex)
    {
        m_textPattern->ChangeValue(pattern);
        m_textLineNumber->ChangeValue(lineIdx);
        m_textFileIndex->ChangeValue(fileIdx);
        m_textColumn->ChangeValue(columnIndex);
    }
    //-----------------------------------------------------------------------------
    void CompilerPatternDlg::OnSubmit(wxCommandEvent& event)
    {
        if (GetPattern().Trim().IsEmpty() || GetFileIndex().Trim().IsEmpty() || GetLineIndex().Trim().IsEmpty()) {
            wxMessageBox(_("Please fill all the fields"), _("光栅故事"), wxOK | wxICON_INFORMATION, this);
            return;
        }
        EndModal(wxID_OK);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // NewCompilerDlg
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    NewCompilerDlg::NewCompilerDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        wxWindowUpdateLocker lls(this);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxCAD0CInitBitmapResources();
            bBitmapLoaded = true;
        }
        wxBoxSizer* boxSizer73 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer73);

        wxFlexGridSizer* flexGridSizer82 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer82->SetFlexibleDirection(wxBOTH);
        flexGridSizer82->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer82->AddGrowableCol(1);

        boxSizer73->Add(flexGridSizer82, 1, wxALL | wxEXPAND, 5);

        m_staticText84 = gsgs_Style().createStaticText(this, wxID_ANY, _("New compiler name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer82->Add(m_staticText84, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlCompilerName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textCtrlCompilerName->SetFocus();

        flexGridSizer82->Add(m_textCtrlCompilerName, 0, wxALL | wxEXPAND, 5);

        m_staticText88 = gsgs_Style().createStaticText(this, wxID_ANY, _("Copy settings from:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer82->Add(m_staticText88, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString m_choiceCompilersArr;
        m_choiceCompilers = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_choiceCompilersArr, 0);

        flexGridSizer82->Add(m_choiceCompilers, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        m_stdBtnSizer75 = new DialogButtonSizer();

        boxSizer73->Add(m_stdBtnSizer75, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_stdBtnSizer75->AddButton(m_buttonCancel);

        m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer75->AddButton(m_buttonOK);
        m_stdBtnSizer75->Realize();

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_buttonOK->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewCompilerDlg::OnOkUI), NULL, this);

        BuildSettingsConfigCookie cookie;
        m_choiceCompilers->Append("<None>");
        CompilerPtr cmp = gsgs_Settings().GetFirstCompiler(cookie);
        while (cmp) {
            m_choiceCompilers->Append(cmp->GetName());
            cmp = gsgs_Settings().GetNextCompiler(cookie);
        }

        m_choiceCompilers->SetStringSelection("<None>");
        Layout();
    }
    //-----------------------------------------------------------------------------
    NewCompilerDlg::~NewCompilerDlg()
    {
        m_buttonOK->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewCompilerDlg::OnOkUI), NULL, this);

    }
    //-------------------------------------------------------------------------
    void NewCompilerDlg::OnOkUI(wxUpdateUIEvent& event)
    {
        event.Enable(!m_textCtrlCompilerName->IsEmpty());
    }
    //-------------------------------------------------------------------------
    wxString NewCompilerDlg::GetMasterCompiler() const
    {
        wxString compilerName = m_choiceCompilers->GetStringSelection();
        if (compilerName == "<None>") {
            return wxEmptyString;
        }
        return compilerName;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // CompilerMainPage
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    CompilerMainPage::CompilerMainPage(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style), m_isDirty(false)
        , m_compiler(NULL)
    {
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxCAD0CInitBitmapResources();
            bBitmapLoaded = true;
        }
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* boxSizer114 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer114);

        /*m_auibar241 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxAUI_TB_DEFAULT_STYLE);
        m_auibar241->SetToolBitmapSize(wxSize(16, 16));

        boxSizer114->Add(m_auibar241, 0, wxEXPAND, 5);

        m_auibar241->AddTool(ID_TOOL_NEW_COMPILER, _("New"), wxXmlResource::Get()->LoadBitmap(wxT("16-plus")), wxNullBitmap, wxITEM_NORMAL, _("Add an existing compiler"), _("Add an existing compiler"), NULL);

        m_auibar241->AddTool(ID_TOOL_COPY_COMPILER, _("Copy"), wxXmlResource::Get()->LoadBitmap(wxT("16-copy")), wxNullBitmap, wxITEM_NORMAL, _("Create a new compiler by copying an existing\ncompiler"), _("Create a new compiler by copying an existing\ncompiler"), NULL);

        m_auibar241->AddTool(ID_TOOL_SCAN_COMPILERS, _("Scan"), wxXmlResource::Get()->LoadBitmap(wxT("16-find_in_files")), wxNullBitmap, wxITEM_NORMAL, _("Scan for compilers on this computer"), _("Scan for compilers on this computer"), NULL);
        m_auibar241->Realize();*/

        wxBoxSizer* boxSizer220 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer114->Add(boxSizer220, 1, wxEXPAND | wxALL, 5);

        wxBoxSizer* boxSizerc20 = new wxBoxSizer(wxVERTICAL);

        boxSizer220->Add(boxSizerc20, 0, wxEXPAND | wxRIGHT, 1);

        m_listCompilers = new DefaultTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1),
            wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        m_listCompilers->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listCompilers->SetShowHeader(FALSE);
        m_listCompilers->SetSortEnable(true);
        boxSizerc20->Add(m_listCompilers, 1, wxEXPAND, 0);

        m_buttonNewCompiler = new Button(this, wxID_ANY, _("New..."), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonNewCompiler->SetDefault();
        boxSizerc20->Add(m_buttonNewCompiler, 0, wxUP | wxDOWN | wxEXPAND, 5);

        m_buttonDeleteCompiler = new Button(this, wxID_ANY, _("Delete"), wxDefaultPosition, wxSize(-1, -1), 0);
        boxSizerc20->Add(m_buttonDeleteCompiler, 0, wxRIGHT | wxLEFT | wxEXPAND, 0);

        m_auiBook = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);
        boxSizer220->Add(m_auiBook, 1, wxLEFT | wxEXPAND, 1);

        m_panelTools = new wxPanel(m_auiBook, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        m_auiBook->AddPage(m_panelTools, _("Toolset"), true);

        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        m_panelTools->SetSizer(boxSizer2);

        m_listSwitches = new DefaultTreeCtrl(m_panelTools, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        m_listSwitches->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listSwitches->SetShowHeader(true);

        boxSizer2->Add(m_listSwitches, 1, wxALL | wxEXPAND, 5);

        wxBoxSizer* bSizer12 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer2->Add(bSizer12, 1, wxEXPAND, 0);

        m_listCtrlFileTypes = new DefaultTreeCtrl(m_panelTools, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        m_listCtrlFileTypes->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listCtrlFileTypes->SetShowHeader(true);

        bSizer12->Add(m_listCtrlFileTypes, 1, wxDOWN | wxRIGHT | wxLEFT | wxEXPAND, 5);

        wxBoxSizer* bSizer111 = new wxBoxSizer(wxVERTICAL);

        bSizer12->Add(bSizer111, 0, wxEXPAND, 5);

        m_buttonNewFileType = new Button(m_panelTools, wxID_ANY, _("New..."), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonNewFileType->SetDefault();

        bSizer111->Add(m_buttonNewFileType, 0, wxDOWN | wxRIGHT | wxEXPAND, 5);

        m_buttonDeleteFileType = new Button(m_panelTools, wxID_ANY, _("Delete"), wxDefaultPosition, wxSize(-1, -1), 0);

        bSizer111->Add(m_buttonDeleteFileType, 0, wxRIGHT | wxEXPAND, 5);

        m_panelComilerOptions = new wxPanel(m_auiBook, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        m_auiBook->AddPage(m_panelComilerOptions, _("Compiler"), false);

        wxBoxSizer* bSizer12_CmpOptions = new wxBoxSizer(wxHORIZONTAL);

        m_panelComilerOptions->SetSizer(bSizer12_CmpOptions);
        m_listCompilerOptions = new DefaultTreeCtrl(m_panelComilerOptions, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        m_listCompilerOptions->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listCompilerOptions->SetShowHeader(true);

        bSizer12_CmpOptions->Add(m_listCompilerOptions, 1, wxALL | wxEXPAND, 5);

        wxBoxSizer* bSizer_1 = new wxBoxSizer(wxVERTICAL);

        bSizer12_CmpOptions->Add(bSizer_1, 0, wxEXPAND, 5);

        m_buttonCompilerOption = new Button(m_panelComilerOptions, wxID_ANY, _("New..."), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonCompilerOption->SetDefault();

        bSizer_1->Add(m_buttonCompilerOption, 0, wxDOWN | wxUP | wxRIGHT | wxEXPAND, 5);

        m_buttonDeleteCompilerOption = new Button(m_panelComilerOptions, wxID_ANY, _("Delete"), wxDefaultPosition, wxSize(-1, -1), 0);

        bSizer_1->Add(m_buttonDeleteCompilerOption, 0, wxRIGHT | wxEXPAND, 5);

        m_panelLinkerOptions = new wxPanel(m_auiBook, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        m_auiBook->AddPage(m_panelLinkerOptions, _("Linker"), false);

        wxBoxSizer* bSizer123 = new wxBoxSizer(wxHORIZONTAL);
        m_panelLinkerOptions->SetSizer(bSizer123);

        m_listLinkerOptions = new DefaultTreeCtrl(m_panelLinkerOptions, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        m_listLinkerOptions->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listLinkerOptions->SetShowHeader(true);

        bSizer123->Add(m_listLinkerOptions, 1, wxALL | wxEXPAND, 5);

        wxBoxSizer* bSizer_2 = new wxBoxSizer(wxVERTICAL);

        bSizer123->Add(bSizer_2, 0, wxEXPAND, 5);

        m_buttonLinkerOption = new Button(m_panelLinkerOptions, wxID_ANY, _("New..."), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonLinkerOption->SetDefault();

        bSizer_2->Add(m_buttonLinkerOption, 0, wxDOWN | wxUP | wxRIGHT | wxEXPAND, 5);

        m_buttonDeleteLinkerOption = new Button(m_panelLinkerOptions, wxID_ANY, _("Delete"), wxDefaultPosition, wxSize(-1, -1), 0);

        bSizer_2->Add(m_buttonDeleteLinkerOption, 0, wxRIGHT | wxEXPAND, 5);

        m_panelPatterns = new wxPanel(m_auiBook, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        m_auiBook->AddPage(m_panelPatterns, _("Output"), false);

        wxBoxSizer* bSizer14 = new wxBoxSizer(wxVERTICAL);
        m_panelPatterns->SetSizer(bSizer14);

        wxBoxSizer* bSizerError = new wxBoxSizer(wxVERTICAL);

        bSizer14->Add(bSizerError, 1, wxEXPAND, 5);

        m_staticText161 = gsgs_Style().createStaticText(m_panelPatterns, wxID_ANY, _("Errors Patterns:"), wxDefaultPosition, wxSize(-1, -1), 0);
        bSizerError->Add(m_staticText161, 0, wxALL, 5);

        wxBoxSizer* bSizer25 = new wxBoxSizer(wxHORIZONTAL);

        bSizerError->Add(bSizer25, 1, wxEXPAND, 5);

        m_listErrPatterns = new DefaultTreeCtrl(m_panelPatterns, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        m_listErrPatterns->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listErrPatterns->SetShowHeader(true);
        bSizer25->Add(m_listErrPatterns, 1, wxALL | wxEXPAND, 5);

        wxBoxSizer* bSizer26 = new wxBoxSizer(wxVERTICAL);

        bSizer25->Add(bSizer26, 0, 0, 5);

        m_btnAddErrPattern = new Button(m_panelPatterns, wxID_ANY, _("Add"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_btnAddErrPattern->SetToolTip(_("Add new error pattern"));

        bSizer26->Add(m_btnAddErrPattern, 0, wxDOWN | wxUP | wxRIGHT | wxEXPAND, 5);

        m_btnDelErrPattern = new Button(m_panelPatterns, wxID_ANY, _("Delete"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_btnDelErrPattern->SetToolTip(_("Delete the selected error pattern"));

        bSizer26->Add(m_btnDelErrPattern, 0, wxRIGHT | wxEXPAND, 5);

        m_btnUpdateErrPattern = new Button(m_panelPatterns, wxID_ANY, _("Edit..."), wxDefaultPosition, wxSize(-1, -1), 0);
        m_btnUpdateErrPattern->SetToolTip(_("Edit the selected error pattern"));

        bSizer26->Add(m_btnUpdateErrPattern, 0, wxRIGHT | wxEXPAND, 5);

        wxBoxSizer* bSizerWarnings = new wxBoxSizer(wxVERTICAL);

        bSizer14->Add(bSizerWarnings, 1, wxEXPAND, 5);

        m_staticText17 = gsgs_Style().createStaticText(m_panelPatterns, wxID_ANY, _("Warnings Patterns:"), wxDefaultPosition, wxSize(-1, -1), 0);
        bSizerWarnings->Add(m_staticText17, 0, wxALL, 5);

        wxBoxSizer* bSizer251 = new wxBoxSizer(wxHORIZONTAL);

        bSizerWarnings->Add(bSizer251, 1, wxEXPAND, 5);

        m_listWarnPatterns = new DefaultTreeCtrl(m_panelPatterns, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        m_listWarnPatterns->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        m_listWarnPatterns->SetShowHeader(true);
        bSizer251->Add(m_listWarnPatterns, 1, wxALL | wxEXPAND, 5);

        wxBoxSizer* bSizer261 = new wxBoxSizer(wxVERTICAL);

        bSizer251->Add(bSizer261, 0, 0, 5);

        m_btnAddWarnPattern = new Button(m_panelPatterns, wxID_ANY, _("Add"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_btnAddWarnPattern->SetToolTip(_("Add new warning pattern"));

        bSizer261->Add(m_btnAddWarnPattern, 0, wxDOWN | wxUP | wxRIGHT | wxEXPAND, 5);

        m_btnDelWarnPattern = new Button(m_panelPatterns, wxID_ANY, _("Delete"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_btnDelWarnPattern->SetToolTip(_("Delete the selected warning pattern"));

        bSizer261->Add(m_btnDelWarnPattern, 0, wxRIGHT | wxEXPAND, 5);

        m_btnUpdateWarnPattern = new Button(m_panelPatterns, wxID_ANY, _("Edit..."), wxDefaultPosition, wxSize(-1, -1), 0);
        m_btnUpdateWarnPattern->SetToolTip(_("Edit the selected warning pattern"));

        bSizer261->Add(m_btnUpdateWarnPattern, 0, wxRIGHT | wxEXPAND, 5);

        m_panelAdvanced = new wxPanel(m_auiBook, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        m_auiBook->AddPage(m_panelAdvanced, _("Misc"), false);

        wxBoxSizer* bSizer11 = new wxBoxSizer(wxVERTICAL);
        m_panelAdvanced->SetSizer(bSizer11);

        wxBoxSizer* bSizer7 = new wxBoxSizer(wxVERTICAL);

        bSizer11->Add(bSizer7, 0, wxALL | wxEXPAND, 5);

        wxFlexGridSizer* fgSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
        fgSizer3->SetFlexibleDirection(wxBOTH);
        fgSizer3->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer3->AddGrowableCol(1);

        bSizer7->Add(fgSizer3, 0, wxEXPAND, 5);

        m_staticText18 = gsgs_Style().createStaticText(m_panelAdvanced, wxID_ANY, _("Global Paths"), wxDefaultPosition, wxSize(-1, -1), 0);
        wxFont m_staticText18Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_staticText18Font.SetWeight(wxFONTWEIGHT_BOLD);
        m_staticText18->SetFont(m_staticText18Font);

        fgSizer3->Add(m_staticText18, 0, wxALL | wxALIGN_RIGHT, 5);

        fgSizer3->Add(0, 0, 0, wxALL, 5);

        fgSizer3->Add(0, 0, 1, wxALL, 5);

        m_staticText141 = gsgs_Style().createStaticText(m_panelAdvanced, wxID_ANY, _("Include Path:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer3->Add(m_staticText141, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlGlobalIncludePath = gsgs_Style().createTextCtrl(m_panelAdvanced, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textCtrlGlobalIncludePath->SetToolTip(_("Set here an additional include paths. Each path should be separated with a semi-colon\nNote that usually you don't need to modify this field and it should be left empty"));

        fgSizer3->Add(m_textCtrlGlobalIncludePath, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        m_button67 = new Button(m_panelAdvanced, wxID_ANY, _("Edit..."), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer3->Add(m_button67, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        m_staticText16 = gsgs_Style().createStaticText(m_panelAdvanced, wxID_ANY, _("Libraries Path:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer3->Add(m_staticText16, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlGlobalLibPath = gsgs_Style().createTextCtrl(m_panelAdvanced, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textCtrlGlobalLibPath->SetToolTip(_("Set here an additional library search paths. Each path should be separated with a semi-colon\nNote that usually you don't need to modify this field and it should be left empty"));

        fgSizer3->Add(m_textCtrlGlobalLibPath, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        m_button69 = new Button(m_panelAdvanced, wxID_ANY, _("Edit..."), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer3->Add(m_button69, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        m_staticText19 = gsgs_Style().createStaticText(m_panelAdvanced, wxID_ANY, _("Misc"), wxDefaultPosition, wxSize(-1, -1), 0);
        wxFont m_staticText19Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_staticText19Font.SetWeight(wxFONTWEIGHT_BOLD);
        m_staticText19->SetFont(m_staticText19Font);

        fgSizer3->Add(m_staticText19, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        fgSizer3->Add(0, 0, 0, wxALL, 5);

        fgSizer3->Add(0, 0, 1, wxALL, 5);

        m_staticText3 = gsgs_Style().createStaticText(m_panelAdvanced, wxID_ANY, _("Objects extension:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer3->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textObjectExtension = gsgs_Style().createTextCtrl(m_panelAdvanced, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer3->Add(m_textObjectExtension, 1, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        fgSizer3->Add(0, 0, 1, wxALL, 5);

        m_staticText24 = gsgs_Style().createStaticText(m_panelAdvanced, wxID_ANY, _("Depends extension:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer3->Add(m_staticText24, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textDependExtension = gsgs_Style().createTextCtrl(m_panelAdvanced, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer3->Add(m_textDependExtension, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        fgSizer3->Add(0, 0, 1, wxALL, 5);

        m_staticText25 = gsgs_Style().createStaticText(m_panelAdvanced, wxID_ANY, _("Preprocessed extension:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer3->Add(m_staticText25, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textPreprocessExtension = gsgs_Style().createTextCtrl(m_panelAdvanced, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);

        fgSizer3->Add(m_textPreprocessExtension, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        fgSizer3->Add(0, 0, 0, wxALL, 5);

        fgSizer3->Add(0, 0, 1, wxALL, 5);

        m_checkBoxGenerateDependenciesFiles = new wxCheckBox(m_panelAdvanced, wxID_ANY, _("Generate dependencies files (*.o.d)"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxGenerateDependenciesFiles->SetValue(false);

        fgSizer3->Add(m_checkBoxGenerateDependenciesFiles, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        fgSizer3->Add(0, 0, 1, wxALL, 5);

        fgSizer3->Add(0, 0, 0, wxALL, 5);

        m_checkBoxObjectNameSameAsFileName = new wxCheckBox(m_panelAdvanced, wxID_ANY, _("Object name is same as the file name"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxObjectNameSameAsFileName->SetValue(false);
        m_checkBoxObjectNameSameAsFileName->SetToolTip(_("By default, gsgs will prepend the relative directory to the file name to compose an object name (e.g. src/a.cpp will generate object: src_a.o).\nUncheck this option to make the object name exactly as the file name without any prefixes"));

        fgSizer3->Add(m_checkBoxObjectNameSameAsFileName, 0, wxALL, 5);

        fgSizer3->Add(0, 0, 1, wxALL, 5);

        fgSizer3->Add(0, 0, 0, wxALL, 5);

        m_checkBoxReadObjectsFromFile = new wxCheckBox(m_panelAdvanced, wxID_ANY, _("Pass object list to the linker via file"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxReadObjectsFromFile->SetValue(false);

        fgSizer3->Add(m_checkBoxReadObjectsFromFile, 0, wxALL, 5);

        // Connect events
        //this->Connect(ID_TOOL_NEW_COMPILER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CompilerMainPage::OnAddExistingCompiler), NULL, this);
        //this->Connect(ID_TOOL_COPY_COMPILER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCloneCompiler), NULL, this);
        //this->Connect(ID_TOOL_SCAN_COMPILERS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CompilerMainPage::OnScanCompilers), NULL, this);
        m_listCompilers->Bind(wxEVT_TREE_SEL_CHANGED, &CompilerMainPage::OnCompilerSelected, this);
        m_listCompilers->Bind(wxEVT_TREE_ITEM_MENU, &CompilerMainPage::OnContextMenu, this);
        m_listErrPatterns->Bind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnErrItemActivated, this);
        m_btnAddErrPattern->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnAddErrPattern), NULL, this);
        m_btnDelErrPattern->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnDelErrPattern), NULL, this);
        m_btnDelErrPattern->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnErrorPatternSelectedUI), NULL, this);
        m_btnUpdateErrPattern->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnUpdateErrPattern), NULL, this);
        m_btnUpdateErrPattern->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnErrorPatternSelectedUI), NULL, this);
        m_listWarnPatterns->Bind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnWarnItemActivated, this);
        m_btnAddWarnPattern->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnAddWarnPattern), NULL, this);
        m_btnDelWarnPattern->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnDelWarnPattern), NULL, this);
        m_btnDelWarnPattern->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnWarningPatternSelectedUI), NULL, this);
        m_btnUpdateWarnPattern->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnUpdateWarnPattern), NULL, this);
        m_btnUpdateWarnPattern->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnWarningPatternSelectedUI), NULL, this);
        m_listCompilerOptions->Bind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnCompilerOptionActivated, this);
        //m_listCompilerOptions->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(CompilerMainPage::OnCompilerOptionDeSelected), NULL, this);
        //m_listCompilerOptions->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnCompilerOptionSelected), NULL, this);
        m_buttonCompilerOption->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewCompilerOption), NULL, this);
        m_buttonDeleteCompilerOption->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteCompilerOption), NULL, this);
        m_listLinkerOptions->Bind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnLinkerOptionActivated, this);
        //m_listLinkerOptions->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(CompilerMainPage::OnLinkerOptionDeSelected), NULL, this);
        //m_listLinkerOptions->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnLinkerOptionSelected), NULL, this);
        m_buttonLinkerOption->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewLinkerOption), NULL, this);
        m_buttonDeleteLinkerOption->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteLinkerOption), NULL, this);
        m_listSwitches->Bind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnItemActivated, this);
        //m_listSwitches->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnItemSelected), NULL, this);
        m_listCtrlFileTypes->Bind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnFileTypeActivated, this);
        //m_listCtrlFileTypes->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(CompilerMainPage::OnFileTypeDeSelected), NULL, this);
        //m_listCtrlFileTypes->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnFileTypeSelected), NULL, this);
        m_buttonNewCompiler->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewCompile), NULL, this);
        m_buttonDeleteCompiler->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteCompile), NULL, this);
        m_buttonNewFileType->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewFileType), NULL, this);
        m_buttonDeleteFileType->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteFileType), NULL, this);
        m_textCtrlGlobalIncludePath->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_button67->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnEditIncludePaths), NULL, this);
        m_textCtrlGlobalLibPath->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_button69->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnEditLibraryPaths), NULL, this);
        m_textObjectExtension->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_textDependExtension->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_textPreprocessExtension->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_checkBoxGenerateDependenciesFiles->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_checkBoxObjectNameSameAsFileName->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_checkBoxReadObjectsFromFile->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);

        // =============-----------------------------
        // Patterns page initialization
        // =============-----------------------------

        m_listErrPatterns->AddHeader(_("Pattern"));
        m_listErrPatterns->AddHeader(_("File name index"));
        m_listErrPatterns->AddHeader(_("Line number index"));
        m_listErrPatterns->AddHeader(_("Column index"));

        m_listWarnPatterns->AddHeader(_("Pattern"));
        m_listWarnPatterns->AddHeader(_("File name index"));
        m_listWarnPatterns->AddHeader(_("Line number index"));
        m_listWarnPatterns->AddHeader(_("Column index"));

        // ==============------------------
        // Switches
        // ==============------------------
        m_listSwitches->AddHeader(_("Name"));
        m_listSwitches->AddHeader(_("Value"));
        // =============----------------------
        // File Types
        // =============----------------------
        m_listCtrlFileTypes->AddHeader(_("Extension"));
        m_listCtrlFileTypes->AddHeader(_("Tyle"));
        m_listCtrlFileTypes->AddHeader(_("Operation"));

        // =====---------------------------------
        // Compiler options
        // =====---------------------------------
        m_listCompilerOptions->AddHeader(_("Option"));
        m_listCompilerOptions->AddHeader(_("Help"));
        // =====---------------------------------
        // Linker options
        // =====---------------------------------
        m_listLinkerOptions->AddHeader(_("Option"));
        m_listLinkerOptions->AddHeader(_("Help"));

        m_listCompilers->AddHeader(_T("Compiler"));
    }
    //-----------------------------------------------------------------------------
    CompilerMainPage::~CompilerMainPage()
    {
        //this->Disconnect(ID_TOOL_NEW_COMPILER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CompilerMainPage::OnAddExistingCompiler), NULL, this);
        //this->Disconnect(ID_TOOL_COPY_COMPILER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCloneCompiler), NULL, this);
        //this->Disconnect(ID_TOOL_SCAN_COMPILERS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CompilerMainPage::OnScanCompilers), NULL, this);
        m_listCompilers->Unbind(wxEVT_TREE_SEL_CHANGED, &CompilerMainPage::OnCompilerSelected,  this);
        m_listCompilers->Unbind(wxEVT_TREE_ITEM_MENU, &CompilerMainPage::OnContextMenu, this);
        m_listErrPatterns->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnErrItemActivated, this);
        m_btnAddErrPattern->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnAddErrPattern), NULL, this);
        m_btnDelErrPattern->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnDelErrPattern), NULL, this);
        m_btnDelErrPattern->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnErrorPatternSelectedUI), NULL, this);
        m_btnUpdateErrPattern->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnUpdateErrPattern), NULL, this);
        m_btnUpdateErrPattern->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnErrorPatternSelectedUI), NULL, this);
        m_listWarnPatterns->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnWarnItemActivated, this);
        m_btnAddWarnPattern->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnAddWarnPattern), NULL, this);
        m_btnDelWarnPattern->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnDelWarnPattern), NULL, this);
        m_btnDelWarnPattern->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnWarningPatternSelectedUI), NULL, this);
        m_btnUpdateWarnPattern->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnBtnUpdateWarnPattern), NULL, this);
        m_btnUpdateWarnPattern->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilerMainPage::OnWarningPatternSelectedUI), NULL, this);
        m_listCompilerOptions->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnCompilerOptionActivated, this);
        //m_listCompilerOptions->Disconnect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(CompilerMainPage::OnCompilerOptionDeSelected), NULL, this);
        //m_listCompilerOptions->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnCompilerOptionSelected), NULL, this);
        m_buttonCompilerOption->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewCompilerOption), NULL, this);
        m_buttonDeleteCompilerOption->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteCompilerOption), NULL, this);
        m_listLinkerOptions->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnLinkerOptionActivated, this);
        //m_listLinkerOptions->Disconnect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(CompilerMainPage::OnLinkerOptionDeSelected), NULL, this);
        //m_listLinkerOptions->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnLinkerOptionSelected), NULL, this);
        m_buttonLinkerOption->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewLinkerOption), NULL, this);
        m_buttonDeleteLinkerOption->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteLinkerOption), NULL, this);
        m_listSwitches->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnItemActivated, this);
        //m_listSwitches->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnItemSelected), NULL, this);
        m_listCtrlFileTypes->Unbind(wxEVT_TREE_ITEM_ACTIVATED, &CompilerMainPage::OnFileTypeActivated, this);
        //m_listCtrlFileTypes->Disconnect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(CompilerMainPage::OnFileTypeDeSelected), NULL, this);
        //m_listCtrlFileTypes->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CompilerMainPage::OnFileTypeSelected), NULL, this);
        m_buttonNewCompiler->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewCompile), NULL, this);
        m_buttonDeleteCompiler->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteCompile), NULL, this);
        m_buttonNewFileType->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnNewFileType), NULL, this);
        m_buttonDeleteFileType->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnDeleteFileType), NULL, this);
        m_textCtrlGlobalIncludePath->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_button67->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnEditIncludePaths), NULL, this);
        m_textCtrlGlobalLibPath->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_button69->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CompilerMainPage::OnEditLibraryPaths), NULL, this);
        m_textObjectExtension->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_textDependExtension->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_textPreprocessExtension->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_checkBoxGenerateDependenciesFiles->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_checkBoxObjectNameSameAsFileName->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);
        m_checkBoxReadObjectsFromFile->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CompilerMainPage::OnCmdModify), NULL, this);

    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnBtnAddErrPattern(wxCommandEvent& event)
    {
        CompilerPatternDlg dlg(wxGetTopLevelParent(this), wxID_ANY, _("Add compiler error pattern"));
        if (dlg.ShowModal() == wxID_OK) {
            wxTreeItemId sle = m_listErrPatterns->AppendItem(dlg.GetPattern());
            m_listErrPatterns->SetItemText(sle, dlg.GetFileIndex(), 1);
            m_listErrPatterns->SetItemText(sle, dlg.GetLineIndex(), 2);
            m_listErrPatterns->SetItemText(sle, dlg.GetColumnIndex(), 3);
            m_isDirty = true;
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnBtnAddWarnPattern(wxCommandEvent& event)
    {
        CompilerPatternDlg dlg(wxGetTopLevelParent(this), wxID_ANY, _("Add compiler warning pattern"));
        if (dlg.ShowModal() == wxID_OK) {
            wxTreeItemId sle = m_listWarnPatterns->AppendItem(dlg.GetPattern());
            m_listWarnPatterns->SetItemText(sle, dlg.GetFileIndex(), 1);
            m_listWarnPatterns->SetItemText(sle, dlg.GetLineIndex(), 2);
            m_listWarnPatterns->SetItemText(sle, dlg.GetColumnIndex(), 3);
            m_isDirty = true;
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnBtnDelErrPattern(wxCommandEvent& event)
    {
        if (m_listErrPatterns->GetSelection().IsOk())
        {
            m_listErrPatterns->DeleteSelection();
            m_isDirty = true;
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnBtnDelWarnPattern(wxCommandEvent& event)
    {
        if (m_listWarnPatterns->GetSelection().IsOk())
        {
            m_listWarnPatterns->DeleteSelection();
            m_isDirty = true;
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnBtnUpdateErrPattern(wxCommandEvent& event)
    {
        DoUpdateErrPattern();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnBtnUpdateWarnPattern(wxCommandEvent& event)
    {
        DoUpdateWarnPattern();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnCompilerOptionActivated(wxTreeEvent& event)
    {
        wxTreeItemId sele = m_listCompilerOptions->GetSelection();
        if (sele.IsOk()) {
            wxString name = m_listCompilerOptions->GetItemText(sele, 0);
            wxString help = m_listCompilerOptions->GetItemText(sele, 1);
            CompilerOptionDialog dlg(wxGetTopLevelParent(this), name, help, _("Compiler option"));
            if (dlg.ShowModal() == wxID_OK) {
                m_listCompilerOptions->SetItemText(sele, dlg.GetName(), 0);
                m_listCompilerOptions->SetItemText(sele, dlg.GetHelp(), 1);
                m_isDirty = true;
            }
        }
    }
    //-----------------------------------------------------------------------------
    /*void CompilerMainPage::OnCompilerOptionDeSelected(wxListEvent& event)
    {
#ifndef __WXMAC__
        m_selectedCmpOption = wxNOT_FOUND;
#endif
        event.Skip();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnCompilerOptionSelected(wxListEvent& event)
    {
        m_selectedCmpOption = event.m_itemIndex;
        event.Skip();
    }*/
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnDeleteCompilerOption(wxCommandEvent& event)
    {
        wxTreeItemId sele = m_listCompilerOptions->GetSelection();
        if (sele.IsOk()) {
            if (wxMessageBox(_("Are you sure you want to delete this compiler option?"),
                _("gsgs"),
                wxYES_NO | wxCANCEL) == wxYES) {
                m_listCompilerOptions->DeleteSelection();
                m_isDirty = true;
            }
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnDeleteFileType(wxCommandEvent& event)
    {
        if (m_listCtrlFileTypes->GetSelection().IsOk()) {
            if (wxMessageBox(_("Make sure delete this options?"), _("光栅故事"), wxYES_NO | wxCANCEL) == wxYES) 
            {
                m_listCtrlFileTypes->DeleteSelection();
                m_isDirty = true;
            }
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnDeleteLinkerOption(wxCommandEvent& event)
    {
        if (m_listLinkerOptions->GetSelection().IsOk()) {
            if (wxMessageBox(_("Are you sure you want to delete this linker option?"), _("光栅故事"), wxYES_NO | wxCANCEL) ==
                wxYES) {
                m_isDirty = true;
                m_listLinkerOptions->DeleteSelection();
            }
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnEditIncludePaths(wxCommandEvent& event)
    {
        wxString curIncludePath = m_textCtrlGlobalIncludePath->GetValue();
        curIncludePath = wxJoin(::wxStringTokenize(curIncludePath, ";", wxTOKEN_STRTOK), '\n', '\0');
        wxString newIncludePath = EditDlg::GetUserString(curIncludePath, gsgs_Event().TopFrame());
        newIncludePath.Trim().Trim(false);
        if (!newIncludePath.IsEmpty()) {
            m_isDirty = true;
            newIncludePath = wxJoin(::wxStringTokenize(newIncludePath, "\n\r", wxTOKEN_STRTOK), ';', '\0');
            m_textCtrlGlobalIncludePath->ChangeValue(newIncludePath);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnEditLibraryPaths(wxCommandEvent& event)
    {
        wxString curLibPath = m_textCtrlGlobalLibPath->GetValue();
        curLibPath = wxJoin(::wxStringTokenize(curLibPath, ";", wxTOKEN_STRTOK), '\n', '\0');
        wxString newLibPath = EditDlg::GetUserString(curLibPath, gsgs_Event().TopFrame());
        newLibPath.Trim().Trim(false);
        if (!newLibPath.IsEmpty()) {
            m_isDirty = true;
            newLibPath = wxJoin(::wxStringTokenize(newLibPath, "\n\r", wxTOKEN_STRTOK), ';', '\0');
            m_textCtrlGlobalLibPath->ChangeValue(newLibPath);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnErrItemActivated(wxTreeEvent& event)
    {
        DoUpdateErrPattern();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnErrorPatternSelectedUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_listErrPatterns->GetSelection().IsOk());
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::DoUpdateWarnPattern()
    {
        wxTreeItemId sle = m_listWarnPatterns->GetSelection();
        if (sle.IsOk())
        {
            wxString pattern = m_listWarnPatterns->GetItemText(sle, 0);
            wxString fileIdx = m_listWarnPatterns->GetItemText(sle, 1);
            wxString lineIdx = m_listWarnPatterns->GetItemText(sle, 2);
            wxString colIdx = m_listWarnPatterns->GetItemText(sle, 3);

            CompilerPatternDlg dlg(wxGetTopLevelParent(this), wxID_ANY, _("Update compiler warning pattern"));
            dlg.SetPattern(pattern, lineIdx, fileIdx, colIdx);
            if (dlg.ShowModal() == wxID_OK) {
                m_listWarnPatterns->SetItemText(sle, dlg.GetPattern(), 0);
                m_listWarnPatterns->SetItemText(sle, dlg.GetFileIndex(), 1);
                m_listWarnPatterns->SetItemText(sle, dlg.GetLineIndex(), 2);
                m_listWarnPatterns->SetItemText(sle, dlg.GetColumnIndex(), 3);
                m_isDirty = true;
            }
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::DoUpdateErrPattern()
    {
        wxTreeItemId sle = m_listErrPatterns->GetSelection();
        if (sle.IsOk())
        {
            wxString pattern = m_listErrPatterns->GetItemText(sle, 0);
            wxString fileIdx = m_listErrPatterns->GetItemText(sle, 1);
            wxString lineIdx = m_listErrPatterns->GetItemText(sle, 2);
            wxString colIdx = m_listErrPatterns->GetItemText(sle, 3);

            CompilerPatternDlg dlg(wxGetTopLevelParent(this), wxID_ANY, _("Update compiler error pattern"));
            dlg.SetPattern(pattern, lineIdx, fileIdx, colIdx);
            if (dlg.ShowModal() == wxID_OK) {
                m_listErrPatterns->SetItemText(sle, dlg.GetPattern(), 0);
                m_listErrPatterns->SetItemText(sle, dlg.GetFileIndex(), 1);
                m_listErrPatterns->SetItemText(sle, dlg.GetLineIndex(), 2);
                m_listErrPatterns->SetItemText(sle, dlg.GetColumnIndex(), 3);
                m_isDirty = true;
            }
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnFileTypeActivated(wxTreeEvent& event)
    {
        wxTreeItemId select = m_listCtrlFileTypes->GetSelection();
        if (select.IsOk()) {
            EditCmpFileInfo dlg(wxGetTopLevelParent(this));

            dlg.SetCompilationLine(m_listCtrlFileTypes->GetItemText(select, 2));
            dlg.SetExtension(m_listCtrlFileTypes->GetItemText(select, 0));
            dlg.SetKind(m_listCtrlFileTypes->GetItemText(select, 1));

            if (dlg.ShowModal() == wxID_OK) {
                m_listCtrlFileTypes->SetItemText(select, dlg.GetCompilationLine(), 2);
                m_listCtrlFileTypes->SetItemText(select, dlg.GetExtension().Lower(), 0);
                m_listCtrlFileTypes->SetItemText(select, dlg.GetKind(), 1);
            }
            m_isDirty = true;
        }
        event.Skip();
    }
    //-----------------------------------------------------------------------------
    /*void CompilerMainPage::OnFileTypeDeSelected(wxListEvent& event)
    {
#ifndef __WXMAC__
        m_selectedFileType = wxNOT_FOUND;
#endif
        event.Skip();
    }*/
    //-----------------------------------------------------------------------------
    /*void CompilerMainPage::OnFileTypeSelected(wxListEvent& event)
    {
        m_selectedFileType = event.m_itemIndex;
        event.Skip();
    }*/
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnItemActivated(wxTreeEvent& event)
    {
        wxTreeItemId sle = m_listSwitches->GetSelection();
        if (sle.IsOk())
        {
            const wxString & name = m_listSwitches->GetItemText(sle, 0);
            const wxString & value = m_listSwitches->GetItemText(sle, 1);
            const wxString & tip = m_listSwitches->GetItemTooltip(sle, 1);
            int ind = m_listSwitches->GetItemIntData(sle);
            wxString message(name + _(" Option:"));
            if (ind == 1)
            {
                CompilerSelectDialog dlg(wxGetTopLevelParent(this), value, tip, name);
                dlg.SetTreeCtrlEvent(m_listSwitches);
                if (dlg.ShowModal() == wxID_OK) {
                    m_isDirty = true;
                    const wxString & newVal = dlg.GetName();
                    m_compiler->SetSwitch(name, newVal);

                    m_listSwitches->SetItemText(sle, newVal, 0);
                    m_listSwitches->SetItemTooltip(sle, dlg.GetHelp(), 0);

                    //InitializeToolset();
                }
            }
            else
            {
                CompilerOptionDialog dlg(wxGetTopLevelParent(this), value, tip, name);
                if (dlg.ShowModal() == wxID_OK) {
                    m_isDirty = true;
                    const wxString & newVal = dlg.GetName();
                    m_compiler->SetSwitch(name, newVal);

                    m_listSwitches->SetItemText(sle, newVal, 0);
                    m_listSwitches->SetItemTooltip(sle, dlg.GetHelp(), 0);

                    //InitializeToolset();
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnLinkerOptionActivated(wxTreeEvent& event)
    {
        wxTreeItemId select = m_listLinkerOptions->GetSelection();
        if (select.IsOk())
        {
            wxString name = m_listLinkerOptions->GetItemText(select);
            wxString help = m_listLinkerOptions->GetItemText(select, 1);
            CompilerOptionDialog dlg(wxGetTopLevelParent(this), name, help, _("Linker option"));
            if (dlg.ShowModal() == wxID_OK) {
                m_isDirty = true;
                m_listLinkerOptions->SetItemText(select, dlg.GetName(), 0);
                m_listLinkerOptions->SetItemText(select, dlg.GetHelp(), 1);
            }
        }
    }
    //-----------------------------------------------------------------------------
    /*void CompilerMainPage::OnLinkerOptionDeSelected(wxListEvent& event)
    {
#ifndef __WXMAC__
        m_selectedLnkOption = wxNOT_FOUND;
#endif
        event.Skip();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnLinkerOptionSelected(wxListEvent& event)
    {
        m_selectedLnkOption = event.m_itemIndex;
        event.Skip();
    }*/
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnNewCompilerOption(wxCommandEvent& event)
    {
        CompilerOptionDialog dlg(wxGetTopLevelParent(this), wxEmptyString, wxEmptyString, _("Compiler option"));
        if (dlg.ShowModal() == wxID_OK) {
            wxTreeItemId sle = m_listCompilerOptions->AppendItem(dlg.GetName());
            m_listCompilerOptions->SetItemText(sle, dlg.GetHelp(), 1);
            m_isDirty = true;
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnNewCompile(wxCommandEvent& event)
    {
        BuildSettingsDialog* dlg = dynamic_cast<BuildSettingsDialog*>(wxGetTopLevelParent(this));
        dlg->CallAfter(&BuildSettingsDialog::OnAddExistingCompiler);
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnDeleteCompile(wxCommandEvent& event)
    {
        wxTreeItemId selection = m_listCompilers->GetSelection();
        if (selection.IsOk())
        {
            wxString compilerName = m_listCompilers->GetItemText(selection);
            if (::wxMessageBox(wxString() << _("Are you sure you want to delete compiler\n'")
                << compilerName << "'?",
                _("Delete Compiler"),
                wxYES_NO | wxCENTER | wxICON_WARNING) != wxYES)
                return;

            gsgs_Settings().DeleteCompiler(compilerName);

            // Reload the content
            LoadCompilers();
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnNewFileType(wxCommandEvent& event)
    {
        EditCmpFileInfo dlg(wxGetTopLevelParent(this));
        if (dlg.ShowModal() == wxID_OK) {
            wxTreeItemId tempid = m_listCtrlFileTypes->AppendItem(dlg.GetExtension().Lower());
            m_listCtrlFileTypes->SetItemText(tempid, dlg.GetCompilationLine(), 2);
            m_listCtrlFileTypes->SetItemText(tempid, dlg.GetKind(), 1);

            m_isDirty = true;
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnNewLinkerOption(wxCommandEvent& event)
    {
        CompilerOptionDialog dlg(wxGetTopLevelParent(this), wxEmptyString, wxEmptyString, _("Linker option"));
        if (dlg.ShowModal() == wxID_OK) {
            m_isDirty = true;
            wxTreeItemId sle = m_listLinkerOptions->AppendItem(dlg.GetName());
            m_listLinkerOptions->SetItemText(sle, dlg.GetHelp(), 1);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnWarnItemActivated(wxTreeEvent& event)
    {
        DoUpdateWarnPattern();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnWarningPatternSelectedUI(wxUpdateUIEvent& event)
    {
        //int sel = m_listWarnPatterns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        event.Enable(m_listWarnPatterns->GetSelection().IsOk());
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::InitializePatterns()
    {
        // Clear the content
        m_listErrPatterns->DeleteAllItems();
        m_listWarnPatterns->DeleteAllItems();

        m_listErrPatterns->AddRoot(_T("ErrPatterns"));
        m_listWarnPatterns->AddRoot(_T("WarnPatterns"));
        // Populate with new content
        CHECK_PTR_RET(m_compiler);

        const Compiler::CmpListInfoPattern& errPatterns = m_compiler->GetErrPatterns();
        Compiler::CmpListInfoPattern::const_iterator itPattern;
        for (itPattern = errPatterns.begin(); itPattern != errPatterns.end(); ++itPattern) {
            wxTreeItemId sl = m_listErrPatterns->AppendItem(itPattern->pattern);
            m_listErrPatterns->SetItemText(sl, itPattern->fileNameIndex, 1);
            m_listErrPatterns->SetItemText(sl, itPattern->lineNumberIndex, 2);
            m_listErrPatterns->SetItemText(sl, itPattern->columnIndex, 3);
        }

        const Compiler::CmpListInfoPattern& warnPatterns = m_compiler->GetWarnPatterns();
        for (itPattern = warnPatterns.begin(); itPattern != warnPatterns.end(); ++itPattern) {
            wxTreeItemId sl = m_listWarnPatterns->AppendItem(itPattern->pattern);
            m_listWarnPatterns->SetItemText(sl, itPattern->fileNameIndex, 1);
            m_listWarnPatterns->SetItemText(sl, itPattern->lineNumberIndex, 2);
            m_listWarnPatterns->SetItemText(sl, itPattern->columnIndex, 3);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::LoadCompiler(const wxString& compilerName)
    {
        // Save before we switch
        if (m_isDirty) {
            Save();
        }
        m_compiler = gsgs_Settings().GetCompiler(compilerName);

        // Start initialization
        Initialize();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::SavePatterns()
    {
        CHECK_PTR_RET(m_compiler);
        Compiler::CmpListInfoPattern errPatterns;

        const RowEntry* rentry = m_listErrPatterns->GetModel().GetRoot();
        if (rentry)
        {
            const std::vector<RowEntry*> & ccli = rentry->GetChildren();
            std::vector<RowEntry*>::const_iterator i, iend = ccli.end();
            for (i = ccli.begin(); i != iend; ++i) {
                RowEntry* it = *i;
                Compiler::CmpInfoPattern infoPattern;
                infoPattern.pattern = it->GetLabel(0);
                infoPattern.fileNameIndex = it->GetLabel(1);
                infoPattern.lineNumberIndex = it->GetLabel(2);
                infoPattern.columnIndex = it->GetLabel(3);
                errPatterns.push_back(infoPattern);
            }
        }

        m_compiler->SetErrPatterns(errPatterns);

        Compiler::CmpListInfoPattern warnPatterns;
        rentry = m_listWarnPatterns->GetModel().GetRoot();
        if (rentry)
        {
            const std::vector<RowEntry*> & ccli = rentry->GetChildren();
            std::vector<RowEntry*>::const_iterator i, iend = ccli.end();
            for (i = ccli.begin(); i != iend; ++i) {
                RowEntry* it = *i;
                Compiler::CmpInfoPattern infoPattern;
                infoPattern.pattern = it->GetLabel(0);
                infoPattern.fileNameIndex = it->GetLabel(1);
                infoPattern.lineNumberIndex = it->GetLabel(2);
                infoPattern.columnIndex = it->GetLabel(3);
                warnPatterns.push_back(infoPattern);
            }
        }
        m_compiler->SetWarnPatterns(warnPatterns);
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::InitializeToolset()
    {
        CHECK_PTR_RET(m_compiler);

        // Clear content
        wxWindowUpdateLocker locker(m_listSwitches);
        m_listSwitches->DeleteAllItems();
        m_listSwitches->AddRoot(_T("Options"));

        m_pgPropCXX = m_listSwitches->AppendItem(_("C++ Compiler"));
        m_listSwitches->SetItemText(m_pgPropCXX, m_compiler->GetTool(wxT("CXX")), 1);
        m_listSwitches->SetItemTooltip(m_pgPropCXX, _("The C++ compiler path (plus optional flags). This tool is represented in the Makefile as $(CXX)"), 1);
        m_listSwitches->SetItemIntData(m_pgPropCXX, 1);

        m_pgPropCC = m_listSwitches->AppendItem(_("C Compiler"));
        m_listSwitches->SetItemText(m_pgPropCC, m_compiler->GetTool(wxT("CC")), 1);
        m_listSwitches->SetItemTooltip(m_pgPropCC, _("The C++ compiler path (plus optional flags). This tool is represented in the Makefile as $(CC)"), 1);
        m_listSwitches->SetItemIntData(m_pgPropCC, 1);

        m_pgPropAS = m_listSwitches->AppendItem(_("Assembler Name"));
        m_listSwitches->SetItemText(m_pgPropAS, m_compiler->GetTool("AS"), 1);
        m_listSwitches->SetItemTooltip(m_pgPropAS, _("The assembler tool path. This tool is referred in the Makefile as $(AS)"), 1);
        m_listSwitches->SetItemIntData(m_pgPropAS, 1);

        m_pgPropLD = m_listSwitches->AppendItem(_("Linker"));
        m_listSwitches->SetItemText(m_pgPropLD, m_compiler->GetTool("LinkerName"), 1);
        m_listSwitches->SetItemTooltip(m_pgPropLD, _("The linker tool. Usually similar to the 'C++ Compiler' tool path"), 1);
        m_listSwitches->SetItemIntData(m_pgPropLD, 1);

        m_pgPropSharedObjectLD = m_listSwitches->AppendItem(_("Shared Object Linker"));
        m_listSwitches->SetItemText(m_pgPropSharedObjectLD, m_compiler->GetTool("SharedObjectLinkerName"), 1);
        m_listSwitches->SetItemTooltip(m_pgPropSharedObjectLD, _("The tool to create shared objects"), 1);
        m_listSwitches->SetItemIntData(m_pgPropSharedObjectLD, 1);

        m_pgPropAR = m_listSwitches->AppendItem(_("Serializer"));
        m_listSwitches->SetItemText(m_pgPropAR, m_compiler->GetTool(wxT("AR")), 1);
        m_listSwitches->SetItemTooltip(m_pgPropAR, _("The static archive tool \"ar\". This tool is referred in the Makefile as $(AR)"), 1);
        m_listSwitches->SetItemIntData(m_pgPropAR, 1);

        m_pgPropResourceCompiler = m_listSwitches->AppendItem(_("Resource Compiler"));
        m_listSwitches->SetItemText(m_pgPropResourceCompiler, m_compiler->GetTool("ResourceCompiler"), 1);
        m_listSwitches->SetItemTooltip(m_pgPropResourceCompiler, _("The resource compiler. (Windows only)"), 1);
        m_listSwitches->SetItemIntData(m_pgPropResourceCompiler, 1);

        m_pgPropMAKE = m_listSwitches->AppendItem(_("Make"));
        m_listSwitches->SetItemText(m_pgPropMAKE, m_compiler->GetTool("MAKE"), 1);
        m_listSwitches->SetItemTooltip(m_pgPropMAKE, _("The Make tool. on Windows / MinGW this is usually mingw32-make.exe while on other OSs its simply 'make'"), 1);
        m_listSwitches->SetItemIntData(m_pgPropMAKE, 1);

        m_pgPropMkdir = m_listSwitches->AppendItem(_("mkdir"));
        m_listSwitches->SetItemText(m_pgPropMkdir, m_compiler->GetTool("MakeDirCommand"), 1);
        m_listSwitches->SetItemTooltip(m_pgPropMkdir, _("Set the 'mkdir' for your OS.\nLeave it empty to use the default for your OS"), 1);
        m_listSwitches->SetItemIntData(m_pgPropMkdir, 1);

        m_pgPropDebugger = m_listSwitches->AppendItem(_("Gdb"));
        m_listSwitches->SetItemText(m_pgPropDebugger, m_compiler->GetTool("Debugger"), 1);
        m_listSwitches->SetItemTooltip(m_pgPropDebugger, _("On various platform (e.g. Cygwin) it is recommended to use their own sepcial gdb executable rather than the global one\nYou can specify one here, or leave this empty to use the default"), 1);
        m_listSwitches->SetItemIntData(m_pgPropDebugger, 1);

        // Validate compiler and add new content
        Compiler::ConstIterator iter = m_compiler->SwitchesBegin();
        for (; iter != m_compiler->SwitchesEnd(); iter++) {
            wxTreeItemId sle = m_listSwitches->AppendItem(iter->first);
            m_listSwitches->SetItemText(sle, iter->second, 1);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::SaveToolset()
    {
        CHECK_PTR_RET(m_compiler);
        m_compiler->SetTool(wxT("CXX"), m_listSwitches->GetItemText(m_pgPropCXX, 1));
        m_compiler->SetTool(wxT("CC"), m_listSwitches->GetItemText(m_pgPropCC, 1));
        m_compiler->SetTool(wxT("AR"), m_listSwitches->GetItemText(m_pgPropAR, 1));
        m_compiler->SetTool(wxT("LinkerName"), m_listSwitches->GetItemText(m_pgPropLD, 1));
        m_compiler->SetTool(wxT("SharedObjectLinkerName"), m_listSwitches->GetItemText(m_pgPropSharedObjectLD, 1));
        m_compiler->SetTool(wxT("ResourceCompiler"), m_listSwitches->GetItemText(m_pgPropResourceCompiler, 1));
        m_compiler->SetTool("MAKE", m_listSwitches->GetItemText(m_pgPropMAKE, 1));
        m_compiler->SetTool("AS", m_listSwitches->GetItemText(m_pgPropAS, 1));
        m_compiler->SetTool("MakeDirCommand", m_listSwitches->GetItemText(m_pgPropMkdir, 1));
        m_compiler->SetTool("Debugger", m_listSwitches->GetItemText(m_pgPropDebugger, 1));
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::Initialize()
    {
        InitializeToolset();
        InitializePatterns();
        InitializeFileTypes();
        InitializeAdvancePage();
        InitializeCompilerOptions();
        InitializeLinkerOptions();
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::Save()
    {
        SaveToolset();
        SavePatterns();
        SaveFileTypes();
        SaveAdvancedPage();
        SaveComilerOptions();
        SaveLinkerOptions();

        // save the compiler to the file system
        gsgs_Settings().SetCompiler(m_compiler);
        m_isDirty = false;
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::InitializeFileTypes()
    {
        CHECK_PTR_RET(m_compiler);

        // populate the list control
        wxWindowUpdateLocker locker(m_listCtrlFileTypes);
        m_listCtrlFileTypes->DeleteAllItems();

        m_listCtrlFileTypes->AddRoot(_("FileTypes"));
        Compiler::CmpFileTypeInfos::const_iterator iter, iterend = m_compiler->GetFileTypes().end();
        for (iter = m_compiler->GetFileTypes().begin(); iter != iterend; iter++) {
            const Compiler::CmpFileTypeInfo & ft = iter->second;

            wxTreeItemId tempid = m_listCtrlFileTypes->AppendItem(ft.extension);
            m_listCtrlFileTypes->SetItemText(tempid, ft.compilation_line, 2);
            m_listCtrlFileTypes->SetItemText(tempid, ft.kind == Compiler::CmpFileKindSource ? _("Source") : _("Resource"), 1);

        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::SaveFileTypes()
    {
        CHECK_PTR_RET(m_compiler);

        Compiler::CmpFileTypeInfos fileTypes;
        const RowEntry* rentry = m_listCtrlFileTypes->GetModel().GetRoot();
        if (rentry)
        {
            const std::vector<RowEntry*> & ccli = rentry->GetChildren();
            std::vector<RowEntry*>::const_iterator i, iend = ccli.end();
            for (i = ccli.begin(); i != iend; ++i) {
                RowEntry* it = *i;
                Compiler::CmpFileTypeInfo ft;
                ft.extension = it->GetLabel(0);
                ft.kind = it->GetLabel(1) == _("Resource") ? Compiler::CmpFileKindResource : Compiler::CmpFileKindSource;
                ft.compilation_line = it->GetLabel(2);

                fileTypes[ft.extension] = ft;
            }
            m_compiler->SetFileTypes(fileTypes);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::InitializeAdvancePage()
    {
        // Clear old conetnt
        m_textObjectExtension->ChangeValue("");
        m_textDependExtension->ChangeValue("");
        m_textPreprocessExtension->ChangeValue("");
        m_checkBoxGenerateDependenciesFiles->SetValue(false);
        m_textCtrlGlobalIncludePath->ChangeValue("");
        m_textCtrlGlobalLibPath->ChangeValue("");
        m_checkBoxReadObjectsFromFile->SetValue(false);
        m_checkBoxObjectNameSameAsFileName->SetValue(false);

        CHECK_PTR_RET(m_compiler);
        m_textObjectExtension->ChangeValue(m_compiler->GetObjectSuffix());
        m_textDependExtension->ChangeValue(m_compiler->GetDependSuffix());
        m_textPreprocessExtension->ChangeValue(m_compiler->GetPreprocessSuffix());
        m_checkBoxGenerateDependenciesFiles->SetValue(m_compiler->GetGenerateDependeciesFile());
        m_textCtrlGlobalIncludePath->ChangeValue(m_compiler->GetGlobalIncludePath());
        m_textCtrlGlobalLibPath->ChangeValue(m_compiler->GetGlobalLibPath());
        m_checkBoxReadObjectsFromFile->SetValue(m_compiler->GetReadObjectFilesFromList());
        m_checkBoxObjectNameSameAsFileName->SetValue(m_compiler->GetObjectNameIdenticalToFileName());
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::SaveAdvancedPage()
    {
        CHECK_PTR_RET(m_compiler);
        m_compiler->SetGenerateDependeciesFile(m_checkBoxGenerateDependenciesFiles->IsChecked());
        m_compiler->SetGlobalIncludePath(m_textCtrlGlobalIncludePath->GetValue());
        m_compiler->SetGlobalLibPath(m_textCtrlGlobalLibPath->GetValue());
        m_compiler->SetObjectSuffix(m_textObjectExtension->GetValue());
        m_compiler->SetDependSuffix(m_textDependExtension->GetValue());
        m_compiler->SetPreprocessSuffix(m_textPreprocessExtension->GetValue());
        m_compiler->SetReadObjectFilesFromList(m_checkBoxReadObjectsFromFile->IsChecked());
        m_compiler->SetObjectNameIdenticalToFileName(m_checkBoxObjectNameSameAsFileName->IsChecked());
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::InitializeCompilerOptions()
    {
        wxWindowUpdateLocker locker(m_listCompilerOptions);
        m_listCompilerOptions->DeleteAllItems();
        m_listCompilerOptions->AddRoot(_T("CompilerOptions"));
        CHECK_PTR_RET(m_compiler);
        const Compiler::CmpCmdLineOptions& cmpOptions = m_compiler->GetCompilerOptions();
        Compiler::CmpCmdLineOptions::const_iterator itCmpOption = cmpOptions.begin();
        for (; itCmpOption != cmpOptions.end(); ++itCmpOption) {
            const Compiler::CmpCmdLineOption& cmpOption = itCmpOption->second;
            wxTreeItemId sle = m_listCompilerOptions->AppendItem(cmpOption.name);
            m_listCompilerOptions->SetItemText(sle, cmpOption.help, 1);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::SaveComilerOptions()
    {
        CHECK_PTR_RET(m_compiler);
        Compiler::CmpCmdLineOptions cmpOptions;
        const RowEntry* rentry = m_listCompilerOptions->GetModel().GetRoot();
        if (rentry)
        {
            const std::vector<RowEntry*> & ccli = rentry->GetChildren();
            std::vector<RowEntry*>::const_iterator i, iend = ccli.end();
            for (i = ccli.begin(); i != iend; ++i) {
                RowEntry* it = *i;
                Compiler::CmpCmdLineOption cmpOption;
                cmpOption.name = it->GetLabel(0);
                cmpOption.help = it->GetLabel(1);

                cmpOptions[cmpOption.name] = cmpOption;
            }
        }
        m_compiler->SetCompilerOptions(cmpOptions);
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::InitializeLinkerOptions()
    {
        wxWindowUpdateLocker locker(m_listLinkerOptions);
        m_listLinkerOptions->DeleteAllItems();
        CHECK_PTR_RET(m_compiler);
        m_listLinkerOptions->AddRoot(_("LinkerOptions"));
        const Compiler::CmpCmdLineOptions& lnkOptions = m_compiler->GetLinkerOptions();
        Compiler::CmpCmdLineOptions::const_iterator itLnkOption = lnkOptions.begin();
        for (; itLnkOption != lnkOptions.end(); ++itLnkOption) {
            const Compiler::CmpCmdLineOption& lnkOption = itLnkOption->second;
            wxTreeItemId sle = m_listLinkerOptions->AppendItem(lnkOption.name);
            m_listLinkerOptions->SetItemText(sle, lnkOption.help, 1);
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::SaveLinkerOptions()
    {
        CHECK_PTR_RET(m_compiler);
        Compiler::CmpCmdLineOptions lnkOptions;
        const RowEntry* rentry = m_listLinkerOptions->GetModel().GetRoot();
        if (rentry)
        {
            const std::vector<RowEntry*> & ccli = rentry->GetChildren();
            std::vector<RowEntry*>::const_iterator i, iend = ccli.end();
            for (i = ccli.begin(); i != iend; ++i) {
                RowEntry* it = *i;

                Compiler::CmpCmdLineOption lnkOption;
                lnkOption.name = it->GetLabel(0);
                lnkOption.help = it->GetLabel(1);

                lnkOptions[lnkOption.name] = lnkOption;
            }
        }
        m_compiler->SetLinkerOptions(lnkOptions);
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::LoadCompilers()
    {
        // Populate the compilers list
        wxWindowUpdateLocker tttl(m_listCompilers);
        m_listCompilers->DeleteAllItems();
        m_listCompilers->AddRoot(_T("Compiler"));
        wxString cmpType;
        if (gsgs_CppWorker().IsOpen() && gsgs_CppWorker().GetActiveProject()) {
            BuildConfigPtr bldConf = gsgs_CppWorker().GetActiveProject()->GetBuildConfiguration();
            if (bldConf) {
                cmpType = bldConf->GetCompilerType();
            }
        }

        BuildSettingsConfigCookie cookie;
        CompilerPtr cmp = gsgs_Settings().GetFirstCompiler(cookie);
        wxTreeItemId sel;
        while (cmp) {
            wxTreeItemId curidx = m_listCompilers->AppendItem(cmp->GetName());
            if (!cmpType.IsEmpty() && (cmp->GetName() == cmpType)) {
                sel = curidx;
            }
            cmp = gsgs_Settings().GetNextCompiler(cookie);
        }

        if (m_listCompilers->GetItemCount()) 
        {
            if(sel.IsOk())
                m_listCompilers->SelectItem(sel, true);
            LoadCompiler(m_listCompilers->GetItemText(sel));
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnCompilerSelected(wxTreeEvent& event) 
    { 
        LoadCompiler(m_listCompilers->GetSelectionText());
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnContextMenu(wxTreeEvent& event)
    {
        wxMenu menu;
        menu.Append(XRCID("rename_compiler"), _("Rename"));
        menu.Append(XRCID("clone_compiler"), _("Clone"));
        //menu.Append(wxID_DELETE);

        //wxTreeItemId selection = m_listCompilers->GetSelection();
        //menu.Enable(wxID_DELETE, selection.IsOk());
        //menu.Enable(XRCID("rename_compiler"), selection.IsOk());

        m_listCompilers->Bind(
            wxEVT_COMMAND_MENU_SELECTED, &CompilerMainPage::OnRenameCompiler, this, XRCID("rename_compiler"));
        m_listCompilers->Bind(
            wxEVT_COMMAND_MENU_SELECTED, &CompilerMainPage::OnCloneCompiler, this, XRCID("clone_compiler"));
        //m_listCompilers->Bind(wxEVT_COMMAND_MENU_SELECTED, &CompilerMainPage::OnDeleteCompiler, this, wxID_DELETE);
        
        m_listCompilers->PopupMenu(&menu);
    }
    //-----------------------------------------------------------------------------
    /*void CompilerMainPage::OnDeleteCompiler(wxCommandEvent& event)
    {
        wxTreeItemId selection = m_listCompilers->GetSelection();
        if (selection.IsOk())
        {
            wxString compilerName = m_listCompilers->GetItemText(selection);
            if (::wxMessageBox(wxString() << _("Are you sure you want to delete compiler\n'")
                << compilerName << "'?",
                _("Delete Compiler"),
                wxYES_NO | wxCENTER | wxICON_WARNING) != wxYES)
                return;

            gsgs_Settings().DeleteCompiler(compilerName);

            // Reload the content
            LoadCompilers();
        }
    }*/
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnRenameCompiler(wxCommandEvent& event)
    {
        wxTreeItemId selection = m_listCompilers->GetSelection();
        if (selection.IsOk())
        {
            wxString compilerName = m_listCompilers->GetItemText(selection);

            CompilerOptionDialog dlg(wxGetTopLevelParent(this), compilerName, "", _T("Compiler Rename"));
            if (dlg.ShowModal() == wxID_OK)
            {
                wxString newName = dlg.GetName();
                if (newName.size())
                {
                    CompilerPtr compiler = gsgs_Settings().GetCompiler(compilerName);
                    if (compiler)
                    {
                        // Delete the old compiler
                        gsgs_Settings().DeleteCompiler(compiler->GetName());

                        // Create new one with differet name
                        compiler->SetName(newName);
                        gsgs_Settings().SetCompiler(compiler);

                        // Reload the content
                        LoadCompilers();
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnCmdModify(wxCommandEvent& event)
    {
        event.Skip();
        m_isDirty = true;
    }
    //-----------------------------------------------------------------------------
    /*void CompilerMainPage::OnAddExistingCompiler(wxCommandEvent& event)
    {
        BuildSettingsDialog* dlg = dynamic_cast<BuildSettingsDialog*>(wxGetTopLevelParent(this));
        dlg->CallAfter(&BuildSettingsDialog::OnAddExistingCompiler);
    }*/
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnCloneCompiler(wxCommandEvent& event)
    {
        BuildSettingsDialog* dlg = dynamic_cast<BuildSettingsDialog*>(wxGetTopLevelParent(this));
        dlg->SetCurrentCompiler(m_listCompilers->GetSelectionText());
        dlg->CallAfter(&BuildSettingsDialog::OnButtonNewClicked);
    }
    //-----------------------------------------------------------------------------
    void CompilerMainPage::OnScanCompilers(wxCommandEvent& event)
    {
        BuildSettingsDialog* dlg = dynamic_cast<BuildSettingsDialog*>(wxGetTopLevelParent(this));
        dlg->CallAfter(&BuildSettingsDialog::OnScanAndSuggestCompilers);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // BuildTabSetting
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BuildTabSetting::BuildTabSetting(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
        wxPanel(parent, id, pos, size, style),
        m_isModified(false)
    {
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        wxArrayString m_pgMgrArr;
        wxArrayInt m_pgMgrIntArr;
        m_pgMgr = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxSize(400, 400), wxPG_DESCRIPTION | wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED);
        m_pgMgr->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        bSizer1->Add(m_pgMgr, 1, wxALL | wxEXPAND, 5);

        CAT_COLOURS_AND_FONTS = m_pgMgr->Append(new wxPropertyCategory(_("Colours and Fonts")));
        CAT_COLOURS_AND_FONTS->SetHelpString(wxT(""));

        m_pgPropFont = m_pgMgr->AppendIn(CAT_COLOURS_AND_FONTS, new wxStringProperty(_("Font"), wxPG_LABEL, wxT("")));
        m_pgPropFont->SetHelpString(_("Select the font to use in the build output tab"));
        m_pgPropFont->SetEditor(wxT("TextCtrlAndButton"));

        m_pgPropErrorColour = m_pgMgr->AppendIn(CAT_COLOURS_AND_FONTS, new wxSystemColourProperty(_("Error colour"), wxPG_LABEL, wxColour(wxT("rgb(255,0,0)"))));
        m_pgPropErrorColour->SetHelpString(_("Use this colour to highlight build error messages"));

        m_pgPropWarningColour = m_pgMgr->AppendIn(CAT_COLOURS_AND_FONTS, new wxSystemColourProperty(_("Warnings colour"), wxPG_LABEL, wxColour(wxT("rgb(128,128,0)"))));
        m_pgPropWarningColour->SetHelpString(_("Use this colour to highlight build warning messages"));

        CAT_GENERAL = m_pgMgr->Append(new wxPropertyCategory(_("General")));
        CAT_GENERAL->SetHelpString(wxT(""));

        m_pgPropJumpWarnings = m_pgMgr->AppendIn(CAT_GENERAL, new wxBoolProperty(_("Skip warnings"), wxPG_LABEL, 1));
        m_pgPropJumpWarnings->SetHelpString(_("When using the menu to jump to errors, skip warnings"));

        m_pgPropAutoHideBuildPane = m_pgMgr->AppendIn(CAT_GENERAL, new wxBoolProperty(_("Auto hide build pane"), wxPG_LABEL, 1));
        m_pgPropAutoHideBuildPane->SetHelpString(_("Automatically hide the build pane when there are neither errors nor warnings"));

        m_pgMgrArr.Add(_("When build starts"));
        m_pgMgrArr.Add(_("When build ends"));
        m_pgMgrArr.Add(_("Don't automatically show"));
        m_pgMgrIntArr.Add(0);
        m_pgMgrIntArr.Add(1);
        m_pgMgrIntArr.Add(2);
        m_pgPropAutoShowBuildPane = m_pgMgr->AppendIn(CAT_GENERAL, new wxEnumProperty(_("Auto show build pane"), wxPG_LABEL, m_pgMgrArr, m_pgMgrIntArr, 0));
        m_pgPropAutoShowBuildPane->SetHelpString(_("Select when to show the build pane"));

        m_pgMgrArr.Clear();
        m_pgMgrIntArr.Clear();
        m_pgMgrArr.Add(_("The first error"));
        m_pgMgrArr.Add(_("The first warning or error"));
        m_pgMgrArr.Add(_("The end"));
        m_pgMgrIntArr.Add(0);
        m_pgMgrIntArr.Add(1);
        m_pgMgrIntArr.Add(2);
        m_pgPropAutoScroll = m_pgMgr->AppendIn(CAT_GENERAL, new wxEnumProperty(_("When build ends scroll to..."), wxPG_LABEL, m_pgMgrArr, m_pgMgrIntArr, 1));
        m_pgPropAutoScroll->SetHelpString(_("After build finishes, if showing the build pane scroll to..."));

        CAT_MARKERS = m_pgMgr->Append(new wxPropertyCategory(_("Build error indicators")));
        CAT_MARKERS->SetHelpString(wxT(""));

        m_pgPropUseMarkers = m_pgMgr->AppendIn(CAT_MARKERS, new wxBoolProperty(_("Use markers"), wxPG_LABEL, 1));
        m_pgPropUseMarkers->SetHelpString(_("Mark the line that contains the build error with a red marker on the left margin"));

        m_pgPropUseAnnotations = m_pgMgr->AppendIn(CAT_MARKERS, new wxBoolProperty(_("Use annotations"), wxPG_LABEL, 1));
        m_pgPropUseAnnotations->SetHelpString(_("If checked, any errors or warnings will be displayed in the editor alongside the failing code."));

        // Connect events
        m_pgMgr->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BuildTabSetting::OnCustomButtonClicked), NULL, this);
        m_pgMgr->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(BuildTabSetting::OnAppearanceChanged), NULL, this);

        ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
        BuildTabSettingsData options;
        gsgs_EditorConfig().ReadObject(wxT("build_tab_settings"), &options);
        m_pgPropJumpWarnings->SetValue(options.GetSkipWarnings());

        wxVariant errorColour, warningColour;
        errorColour << wxColour(options.GetErrorColour());
        warningColour << wxColour(options.GetWarnColour());

        m_pgPropErrorColour->SetValue(errorColour);
        m_pgPropWarningColour->SetValue(warningColour);

        const wxFont& font = options.GetBuildFont();
        m_pgPropFont->SetValue(FontHelper::ToString(font));

        m_pgPropAutoShowBuildPane->SetValueFromInt(options.GetShowBuildPane());
        m_pgPropAutoHideBuildPane->SetValue((bool)options.GetAutoHide());
        m_pgPropAutoScroll->SetValueFromInt(options.GetBuildPaneScrollDestination());
        m_pgPropUseMarkers->SetValue((bool)(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Bookmarks));
        m_pgPropUseAnnotations->SetValue((bool)(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Annotate));
    }
    //-----------------------------------------------------------------------------
    BuildTabSetting::~BuildTabSetting()
    {
        m_pgMgr->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BuildTabSetting::OnCustomButtonClicked), NULL, this);
        m_pgMgr->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(BuildTabSetting::OnAppearanceChanged), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void BuildTabSetting::Save()
    {
        BuildTabSettingsData options;

        wxColour defaultErrorColour(*wxRED);
        wxColour defaultWarningColour("rgb(128, 128, 0)");

        wxColourPropertyValue errorColour, warningColour;
        errorColour << m_pgPropErrorColour->GetValue();
        warningColour << m_pgPropWarningColour->GetValue();

        options.SetErrorColour(errorColour.m_colour.GetAsString(wxC2S_HTML_SYNTAX));
        options.SetWarnColour(warningColour.m_colour.GetAsString(wxC2S_HTML_SYNTAX));
        options.SetBuildFont(FontHelper::FromString(m_pgPropFont->GetValue().GetString()));
        options.SetSkipWarnings(m_pgPropJumpWarnings->GetValue().GetBool());
        options.SetShowBuildPane(m_pgPropAutoShowBuildPane->GetValue().GetInteger());
        options.SetAutoHide(m_pgPropAutoHideBuildPane->GetValue().GetBool());
        options.SetBuildPaneScrollDestination(m_pgPropAutoScroll->GetValue().GetInteger());

        int flag(BuildTabSettingsData::EWS_NoMarkers);
        if (m_pgPropUseMarkers->GetValue().GetBool()) {
            flag |= BuildTabSettingsData::EWS_Bookmarks;
        }

        if (m_pgPropUseAnnotations->GetValue().GetBool()) {
            flag |= BuildTabSettingsData::EWS_Annotate;
        }

        options.SetErrorWarningStyle(flag);
        gsgs_EditorConfig().WriteObject(wxT("build_tab_settings"), &options);
        m_isModified = false;
    }
    //-----------------------------------------------------------------------------
    void BuildTabSetting::OnUpdateUI(wxUpdateUIEvent& event) 
    {
    }
    //-----------------------------------------------------------------------------
    void BuildTabSetting::OnCustomButtonClicked(wxCommandEvent& event)
    {
        wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
        CHECK_PTR_RET(prop);

        if (prop == m_pgPropFont)
        {
            CallAfter(&BuildTabSetting::SelectFont);
        }
    }
    //-----------------------------------------------------------------------------
    void BuildTabSetting::SelectFont()
    {
        wxFontDialog dlg(gsgs_Event().TopFrame());
        if (dlg.ShowModal() == wxID_OK)
        {
            const wxFontData& fntdata = dlg.GetFontData();
            wxFont font = fntdata.GetChosenFont();
            m_pgPropFont->SetValue(FontHelper::ToString(font));
        }
    }
    //-----------------------------------------------------------------------------
    void BuildTabSetting::OnAppearanceChanged(wxPropertyGridEvent& event)
    {
        event.Skip();
        m_isModified = true;
    }
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    // BuildPage
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    class BuildSystemPage : public wxPanel
    {
    public:
        BuildSystemPage(wxWindow *parent, wxString name)
            : wxPanel(parent)
            , m_name(name)
        {
            wxBoxSizer* bSizer6 = new wxBoxSizer(wxVERTICAL);
            this->SetSizer(bSizer6);
            this->Layout();
        }
        virtual ~BuildSystemPage() {}

        void Save()
        {
            // update cached builders
            BuilderPtr builder = gsgs_Build().GetBuilder(m_name);
            gsgs_Build().AddBuilder(builder);

            // Save the configuration
            gsgs_Settings().SaveBuilderConfig(builder);
        }
        void SetSelected()
        {
            BuilderPtr builder = gsgs_Build().GetBuilder(m_name);
            builder->SetActive();
        }
    private:
        wxStaticText* m_staticText17;
        FilePicker* m_filePicker;
        wxStaticText* m_staticText18;
        wxString m_name;
        wxStaticText* m_staticText19;
    };
    //-----------------------------------------------------------------------------
    BuildPage::BuildPage(wxWindow* parent, int id, wxPoint pos, wxSize size, int style)
        : wxPanel(parent, id, pos, size, style)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        m_staticText = gsgs_Style().createStaticText(this, wxID_ANY, _("Available Build Systems:"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText->SetLabelMarkup(_("Available Build Systems:"));
        m_staticText->Wrap(-1);
        mainSizer->Add(m_staticText, 0, wxALL, 5);

        m_bookBuildSystems = new wxChoicebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT);
        mainSizer->Add(m_bookBuildSystems, 0, wxEXPAND | wxALL, 5);

        m_fixOnStartup = new wxCheckBox(this, wxID_ANY, _("Fix build tool path on startup"));
        mainSizer->Add(m_fixOnStartup, 0, wxEXPAND | wxALL, 5);

        this->SetSizer(mainSizer);
        //this->Layout();

        long fix = gsgs_EditorConfig().GetInteger(wxT("FixBuildToolOnStartup"), 1);
        m_fixOnStartup->SetValue(fix ? true : false);
        CustomInit();
    }
    //-----------------------------------------------------------------------------
    void BuildPage::CustomInit()
    {
        // rest of builders list
        std::list<wxString> builders;
        gsgs_Build().GetBuilders(builders);

        std::list<wxString>::iterator iter = builders.begin();
        for (; iter != builders.end(); iter++) {
            BuilderPtr builder = gsgs_Build().GetBuilder(*iter);
            m_bookBuildSystems->AddPage(CreateBuildSystemPage(*iter), *iter, builder->IsActive());
        }
    }
    //-----------------------------------------------------------------------------
    wxPanel* BuildPage::CreateBuildSystemPage(const wxString& name)
    {
        return new BuildSystemPage(m_bookBuildSystems, name);
    }
    //-----------------------------------------------------------------------------
    void BuildPage::Save()
    {
        gsgs_EditorConfig().SetInteger(wxT("FixBuildToolOnStartup"), m_fixOnStartup->IsChecked() ? 1 : 0);

        // Save current page displayed as 'selected' builder
        int sel = (int)m_bookBuildSystems->GetSelection();

        // gsgsLogMessage(wxString::Format( wxT("selection:%d"), sel ));
        BuildSystemPage* page = dynamic_cast<BuildSystemPage*>(m_bookBuildSystems->GetPage(sel));
        if (page) 
        {
            page->SetSelected();
        }

        int count = (int)m_bookBuildSystems->GetPageCount();
        for (int i = 0; i < count; i++) 
        {
            BuildSystemPage* page = dynamic_cast<BuildSystemPage*>(m_bookBuildSystems->GetPage(i));
            if (page) {
                page->Save();
            }
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerPageStartupCmds
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    DebuggerPageStartupCmds::DebuggerPageStartupCmds(wxWindow* parent, const wxString& title,
        wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style), 
        m_title(title)
    {
        wxBoxSizer* bSizer7 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer7);

        m_textCtrlStartupCommands = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        // Configure the fold margin
        m_textCtrlStartupCommands->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
        m_textCtrlStartupCommands->SetMarginMask(4, wxSTC_MASK_FOLDERS);
        m_textCtrlStartupCommands->SetMarginSensitive(4, true);
        m_textCtrlStartupCommands->SetMarginWidth(4, 0);

        // Configure the tracker margin
        m_textCtrlStartupCommands->SetMarginWidth(1, 0);

        // Configure the symbol margin
        m_textCtrlStartupCommands->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_textCtrlStartupCommands->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
        m_textCtrlStartupCommands->SetMarginWidth(2, 0);
        m_textCtrlStartupCommands->SetMarginSensitive(2, true);

        // Configure the line numbers margin
        m_textCtrlStartupCommands->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_textCtrlStartupCommands->SetMarginWidth(0, 0);

        // Configure the line symbol margin
        m_textCtrlStartupCommands->SetMarginType(3, wxSTC_MARGIN_FORE);
        m_textCtrlStartupCommands->SetMarginMask(3, 0);
        m_textCtrlStartupCommands->SetMarginWidth(3, 0);
        // Select the lexer
        m_textCtrlStartupCommands->SetLexer(wxSTC_LEX_NULL);
        // Set default font / styles
        m_textCtrlStartupCommands->StyleClearAll();
        m_textCtrlStartupCommands->SetWrapMode(0);
        m_textCtrlStartupCommands->SetIndentationGuides(0);
        m_textCtrlStartupCommands->SetKeyWords(0, wxT(""));
        m_textCtrlStartupCommands->SetKeyWords(1, wxT(""));
        m_textCtrlStartupCommands->SetKeyWords(2, wxT(""));
        m_textCtrlStartupCommands->SetKeyWords(3, wxT(""));
        m_textCtrlStartupCommands->SetKeyWords(4, wxT(""));

        bSizer7->Add(m_textCtrlStartupCommands, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        if (lexer) { lexer->Apply(m_textCtrlStartupCommands); }

        DebuggerInformation info;
        if (DebuggerMgr::Get().GetDebuggerInformation(title, info)) {
            m_textCtrlStartupCommands->SetText(info.startupCommands);
        }
    }
    //-----------------------------------------------------------------------------
    DebuggerPageStartupCmds::~DebuggerPageStartupCmds() {}
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // PreDefinedTypesPage
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    PreDefinedTypesPage::PreDefinedTypesPage(wxWindow* parent, const DebuggerPreDefinedTypes& preDefTypes,
        wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style), 
        m_selectedItem(wxNOT_FOUND)
    {
        wxBoxSizer* bSizer11 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer11);

        m_panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

        bSizer11->Add(m_panel2, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);
        m_panel2->SetSizer(bSizer4);

        wxBoxSizer* bSizer5 = new wxBoxSizer(wxHORIZONTAL);

        bSizer4->Add(bSizer5, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_listCtrl1 = new wxListCtrl(m_panel2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)),
            wxLC_VRULES | wxLC_HRULES | wxLC_SINGLE_SEL | wxLC_REPORT);

        bSizer5->Add(m_listCtrl1, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer6 = new wxBoxSizer(wxVERTICAL);

        bSizer5->Add(bSizer6, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_buttonNewType = new Button(m_panel2, wxID_ANY, _("&New..."), wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);

        bSizer6->Add(m_buttonNewType, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_buttonEdit = new Button(m_panel2, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);

        bSizer6->Add(m_buttonEdit, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_buttonDelete = new Button(m_panel2, wxID_ANY, _("&Delete"), wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);

        bSizer6->Add(m_buttonDelete, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        // Connect events
        m_listCtrl1->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            wxListEventHandler(PreDefinedTypesPage::OnItemActivated), NULL, this);
        m_listCtrl1->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED,
            wxListEventHandler(PreDefinedTypesPage::OnItemDeselected), NULL, this);
        m_listCtrl1->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(PreDefinedTypesPage::OnItemSelected),
            NULL, this);
        m_buttonNewType->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(PreDefinedTypesPage::OnNewShortcut), NULL, this);
        m_buttonEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreDefinedTypesPage::OnEditShortcut),
            NULL, this);
        m_buttonDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(PreDefinedTypesPage::OnDeleteShortcut), NULL, this);

        m_data = preDefTypes;
        m_listCtrl1->InsertColumn(0, _("Type"));
        m_listCtrl1->InsertColumn(1, _("Expression"));
        m_listCtrl1->InsertColumn(2, _("Debugger Command"));

        //Populate the list with the items from the configuration file
        DebuggerCmdDataVec cmds = m_data.GetCmds();
        for (size_t i = 0; i < cmds.size(); i++) {
            DebuggerCmdData cmd = cmds.at(i);

            long item = AppendListCtrlRow(m_listCtrl1);
            SetColumnText(m_listCtrl1, item, 0, cmd.GetName());
            SetColumnText(m_listCtrl1, item, 1, cmd.GetCommand());
            SetColumnText(m_listCtrl1, item, 2, cmd.GetDbgCommand());
        }
        m_listCtrl1->SetColumnWidth(0, 100);
        m_listCtrl1->SetColumnWidth(1, 200);
    }
    //-----------------------------------------------------------------------------
    PreDefinedTypesPage::~PreDefinedTypesPage()
    {
        m_listCtrl1->Disconnect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            wxListEventHandler(PreDefinedTypesPage::OnItemActivated), NULL, this);
        m_listCtrl1->Disconnect(wxEVT_COMMAND_LIST_ITEM_DESELECTED,
            wxListEventHandler(PreDefinedTypesPage::OnItemDeselected), NULL, this);
        m_listCtrl1->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED,
            wxListEventHandler(PreDefinedTypesPage::OnItemSelected), NULL, this);
        m_buttonNewType->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(PreDefinedTypesPage::OnNewShortcut), NULL, this);
        m_buttonEdit->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(PreDefinedTypesPage::OnEditShortcut), NULL, this);
        m_buttonDelete->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(PreDefinedTypesPage::OnDeleteShortcut), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::OnItemActivated(wxListEvent& event)
    {
        m_selectedItem = event.m_itemIndex;
        DoEditItem();
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::OnItemDeselected(wxListEvent& event)
    {
        wxUnusedVar(event);
        m_selectedItem = wxNOT_FOUND;
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::OnItemSelected(wxListEvent& event)
    {
        m_selectedItem = event.m_itemIndex;
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::OnNewShortcut(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        DbgCommandDlg dlg(this);

        if (dlg.ShowModal() == wxID_OK) {

            //add new command to the table
            wxString name = dlg.GetName();
            wxString expression = dlg.GetExpression();
            wxString dbgCmd = dlg.GetDbgCommand();

            // Make sure that the expression does not exist
            int count = m_listCtrl1->GetItemCount();
            for (int i = 0; i < count; i++) {
                wxString existingName = GetColumnText(m_listCtrl1, i, 0);
                if (name == existingName) {
                    wxMessageBox(_("A Debugger type with that name already exists"), _("光栅故事"), wxOK | wxICON_INFORMATION);
                    return;
                }
            }

            // Set the item display name
            long item = ::AppendListCtrlRow(m_listCtrl1);

            SetColumnText(m_listCtrl1, item, 0, name);
            SetColumnText(m_listCtrl1, item, 1, expression);
            SetColumnText(m_listCtrl1, item, 2, dbgCmd);

            m_listCtrl1->SetColumnWidth(0, -1);
            m_listCtrl1->SetColumnWidth(1, -1);
            m_listCtrl1->SetColumnWidth(2, -1);
        }
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::OnEditShortcut(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        DoEditItem();
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::OnDeleteShortcut(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        DoDeleteItem();
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::DoEditItem()
    {
        //Edit the selection
        if (m_selectedItem == wxNOT_FOUND) {
            return;
        }

        //popup edit dialog
        DbgCommandDlg dlg(this);

        wxString name = GetColumnText(m_listCtrl1, m_selectedItem, 0);
        wxString expr = GetColumnText(m_listCtrl1, m_selectedItem, 1);
        wxString dbgCmd = GetColumnText(m_listCtrl1, m_selectedItem, 2);

        dlg.SetName(name);
        dlg.SetExpression(expr);
        dlg.SetDbgCommand(dbgCmd);

        if (dlg.ShowModal() == wxID_OK) {
            SetColumnText(m_listCtrl1, m_selectedItem, 0, dlg.GetName());
            SetColumnText(m_listCtrl1, m_selectedItem, 1, dlg.GetExpression());
            SetColumnText(m_listCtrl1, m_selectedItem, 2, dlg.GetDbgCommand());
        }
    }
    //-----------------------------------------------------------------------------
    void PreDefinedTypesPage::DoDeleteItem()
    {
        if (m_selectedItem == wxNOT_FOUND) {
            return;
        }
        m_listCtrl1->DeleteItem(m_selectedItem);
        m_selectedItem = wxNOT_FOUND;
    }
    //-----------------------------------------------------------------------------
    DebuggerPreDefinedTypes PreDefinedTypesPage::GetPreDefinedTypes()
    {
        int count = m_listCtrl1->GetItemCount();
        DebuggerCmdDataVec cmdArr;

        for (int i = 0; i < count; i++) {
            DebuggerCmdData cmd;
            cmd.SetName(GetColumnText(m_listCtrl1, i, 0));
            cmd.SetCommand(GetColumnText(m_listCtrl1, i, 1));
            cmd.SetDbgCommand(GetColumnText(m_listCtrl1, i, 2));
            cmdArr.push_back(cmd);
        }

        m_data.SetCmds(cmdArr);
        return m_data;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // NewPreDefinedSetDlg
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    NewPreDefinedSetDlg::NewPreDefinedSetDlg(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizer17 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer17);

        wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer2->SetFlexibleDirection(wxBOTH);
        fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer2->AddGrowableCol(1);

        bSizer17->Add(fgSizer2, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrlName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        fgSizer2->Add(m_textCtrlName, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Copy Values From:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceCopyFromArr;
        m_choiceCopyFrom =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceCopyFromArr, 0);

        fgSizer2->Add(m_choiceCopyFrom, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        fgSizer2->Add(0, 0, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxMakeActive = new wxCheckBox(this, wxID_ANY, _("Make this 'PreDefined Types' set active"),
            wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_checkBoxMakeActive->SetValue(false);

        fgSizer2->Add(m_checkBoxMakeActive, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        bSizer17->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer18 = new wxBoxSizer(wxHORIZONTAL);

        bSizer17->Add(bSizer18, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_button9 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_button9->SetDefault();

        bSizer18->Add(m_button9, 0, wxALL, WXC_FROM_DIP(5));

        m_button10 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        bSizer18->Add(m_button10, 0, wxALL, WXC_FROM_DIP(5));

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
    }
    //-----------------------------------------------------------------------------
    NewPreDefinedSetDlg::~NewPreDefinedSetDlg() {}
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerPage
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    DebuggerPage::DebuggerPage(wxWindow* parent, const wxString & title, wxWindowID id, 
        const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style),
        m_title(title)
    {
        wxBoxSizer* bSizer18 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer18);

        m_notebook73 = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);
        m_notebook73->SetName(wxT("m_notebook73"));

        bSizer18->Add(m_notebook73, 1, wxEXPAND | wxALL, 5);

        m_panelGeneral = new wxPanel(m_notebook73, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook73, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_notebook73->AddPage(m_panelGeneral, _("General"), true);

        wxBoxSizer* boxSizer81 = new wxBoxSizer(wxVERTICAL);
        m_panelGeneral->SetSizer(boxSizer81);

        wxFlexGridSizer* flexGridSizer49 = new wxFlexGridSizer(0, 3, 0, 0);
        flexGridSizer49->SetFlexibleDirection(wxBOTH);
        flexGridSizer49->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer49->AddGrowableCol(1);

        boxSizer81->Add(flexGridSizer49, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText1 = gsgs_Style().createStaticText(m_panelGeneral, wxID_ANY, _("Debugger path:"), wxDefaultPosition, wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        flexGridSizer49->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrDbgPath = gsgs_Style().createTextCtrl(m_panelGeneral, wxID_ANY, wxT(""), wxDefaultPosition,
            wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);

        flexGridSizer49->Add(m_textCtrDbgPath, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_buttonBrowse = new Button(m_panelGeneral, wxID_ANY, _("Browse"), wxDefaultPosition,
            wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);

        flexGridSizer49->Add(m_buttonBrowse, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer86 = new wxBoxSizer(wxVERTICAL);

        boxSizer81->Add(boxSizer86, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxEnablePendingBreakpoints =
            new wxCheckBox(m_panelGeneral, wxID_ANY, _("Enable pending breakpoints"), wxDefaultPosition,
                wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        m_checkBoxEnablePendingBreakpoints->SetValue(false);
        m_checkBoxEnablePendingBreakpoints->SetToolTip(
            _("Some breakpoints can't be applied before the program is run, or even later. This is especially a problem "
                "when trying to debug inside a library that is dynamically loaded (gsgs itself contains examples of "
                "this).\n\ngdb has an option to 'remember' any breakpoints that it can't initially set, and automatically to "
                "set them when it becomes possible. It doesn't always work! However, tick this box to tell gdb to try."));

        boxSizer86->Add(m_checkBoxEnablePendingBreakpoints, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxSetBreakpointsAfterMain =
            new wxCheckBox(m_panelGeneral, wxID_ANY, _("Apply breakpoints after main function is hit"), wxDefaultPosition,
                wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        m_checkBoxSetBreakpointsAfterMain->SetValue(false);
        m_checkBoxSetBreakpointsAfterMain->SetToolTip(
            _("Sometimes, some breakpoints won't apply cleanly until after main() has been reached. If this box is ticked, "
                "gsgs won't try to apply them earlier."));

        boxSizer86->Add(m_checkBoxSetBreakpointsAfterMain, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBreakAtWinMain = new wxCheckBox(m_panelGeneral, wxID_ANY, _("Automatically set breakpoint at main"),
            wxDefaultPosition, wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        m_checkBreakAtWinMain->SetValue(false);
        m_checkBreakAtWinMain->SetToolTip(
            _("Every time the debugger runs, set a breakpoint at main(). You may wish to stop then anyway; but it's "
                "especially useful when you want to set breakpoints that won't 'take' earlier (however, first try enabling "
                "Pending breakpoints, or 'Apply breakpoints after main is hit'"));

        boxSizer86->Add(m_checkBreakAtWinMain, 0, wxALL, WXC_FROM_DIP(5));

        m_catchThrow = new wxCheckBox(m_panelGeneral, wxID_ANY, _("Break when a C++ exception is thrown"),
            wxDefaultPosition, wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        m_catchThrow->SetValue(false);

        boxSizer86->Add(m_catchThrow, 0, wxALL, WXC_FROM_DIP(5));

        m_raiseOnBpHit = new wxCheckBox(m_panelGeneral, wxID_ANY, _("Raise gsgs when a breakpoint is hit"),
            wxDefaultPosition, wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        m_raiseOnBpHit->SetValue(true);
        m_raiseOnBpHit->SetToolTip(
            _("Normally, when a breakpoint is hit, you'll want gsgs to be raise to the top of the window z-order, so "
                "that you can examine values of variables etc.\nHowever you won't always want that to happen; in particular, "
                "not if the breakpoint has commands, which end in 'continue'. If so untick this box to stop it happening."));

        boxSizer86->Add(m_raiseOnBpHit, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxRunAsSuperuser =
            new wxCheckBox(m_panelGeneral, wxID_ANY, _("On platforms that supports it, run gdb as superuser"),
                wxDefaultPosition, wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        m_checkBoxRunAsSuperuser->SetValue(false);
        m_checkBoxRunAsSuperuser->SetToolTip(
            _("On platforms that supports it, run gdb as superuser.\nThis is done by running gdb with 'sudo'"));

        boxSizer86->Add(m_checkBoxRunAsSuperuser, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxDefaultHexDisplay = new wxCheckBox(m_panelGeneral, wxID_ANY, _("Use default Hex Display"),
            wxDefaultPosition, wxDLG_UNIT(m_panelGeneral, wxSize(-1, -1)), 0);
        m_checkBoxDefaultHexDisplay->SetValue(false);

        boxSizer86->Add(m_checkBoxDefaultHexDisplay, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_panelTooltip = new wxPanel(m_notebook73, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook73, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_notebook73->AddPage(m_panelTooltip, _("Tooltip"), false);

        wxBoxSizer* boxSizer83 = new wxBoxSizer(wxVERTICAL);
        m_panelTooltip->SetSizer(boxSizer83);

        m_showTooltipsRequiresControl =
            new wxCheckBox(m_panelTooltip, wxID_ANY, _("Use CTRL key to evaluate expressions under the cursor"),
                wxDefaultPosition, wxDLG_UNIT(m_panelTooltip, wxSize(-1, -1)), 0);
        m_showTooltipsRequiresControl->SetValue(true);
        m_showTooltipsRequiresControl->SetToolTip(
            _("When enabled, gsgs will evaluate the expression under the cursor only if the CTRL key is down. "
                "\nOtherwise, it will evaluate it automatically"));

        boxSizer83->Add(m_showTooltipsRequiresControl, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxAutoExpand = new wxCheckBox(m_panelTooltip, wxID_ANY, _("Auto expand items under the cursor"),
            wxDefaultPosition, wxDLG_UNIT(m_panelTooltip, wxSize(-1, -1)), 0);
        m_checkBoxAutoExpand->SetValue(false);

        boxSizer83->Add(m_checkBoxAutoExpand, 0, wxALL, WXC_FROM_DIP(5));

        m_panelDisplay = new wxPanel(m_notebook73, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook73, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_notebook73->AddPage(m_panelDisplay, _("Display"), false);

        wxBoxSizer* boxSizer85 = new wxBoxSizer(wxVERTICAL);
        m_panelDisplay->SetSizer(boxSizer85);

        wxFlexGridSizer* fgSizer21 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer21->SetFlexibleDirection(wxBOTH);
        fgSizer21->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        boxSizer85->Add(fgSizer21, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_staticText2 = gsgs_Style().createStaticText(m_panelDisplay, wxID_ANY, _("Number of elements to display for arrays / strings:"),
            wxDefaultPosition, wxDLG_UNIT(m_panelDisplay, wxSize(-1, -1)), 0);
        m_staticText2->SetToolTip(_("For no limit, set it to 0"));

        fgSizer21->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_spinCtrlNumElements = new wxSpinCtrl(m_panelDisplay, wxID_ANY, wxT("200"), wxDefaultPosition,
            wxDLG_UNIT(m_panelDisplay, wxSize(-1, -1)), wxSP_ARROW_KEYS);
        m_spinCtrlNumElements->SetToolTip(_("For no limit, set it to 0"));
        m_spinCtrlNumElements->SetRange(0, 10000);
        m_spinCtrlNumElements->SetValue(200);

        fgSizer21->Add(m_spinCtrlNumElements, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_checkBoxExpandLocals = new wxCheckBox(m_panelDisplay, wxID_ANY, _("Use 'PreDefined types for the 'Locals' view"),
            wxDefaultPosition, wxDLG_UNIT(m_panelDisplay, wxSize(-1, -1)), 0);
        m_checkBoxExpandLocals->SetValue(false);

        fgSizer21->Add(m_checkBoxExpandLocals, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        fgSizer21->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxCharArrAsPtr = new wxCheckBox(m_panelDisplay, wxID_ANY, _("Auto cast 'char[]' into 'char*'"),
            wxDefaultPosition, wxDLG_UNIT(m_panelDisplay, wxSize(-1, -1)), 0);
        m_checkBoxCharArrAsPtr->SetValue(false);

        fgSizer21->Add(m_checkBoxCharArrAsPtr, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        fgSizer21->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxUsePrettyPrinting = new wxCheckBox(m_panelDisplay, wxID_ANY, _("Enable GDB Pretty Printing"),
            wxDefaultPosition, wxDLG_UNIT(m_panelDisplay, wxSize(-1, -1)), 0);
        m_checkBoxUsePrettyPrinting->SetValue(false);
        m_checkBoxUsePrettyPrinting->SetToolTip(
            _("If ticked, examining the contents of e.g. std::string, wxString, wxArrayString will be much easier"));

        fgSizer21->Add(m_checkBoxUsePrettyPrinting, 0, wxALL, WXC_FROM_DIP(5));

        fgSizer21->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxPrintObjectOn = new wxCheckBox(m_panelDisplay, wxID_ANY, _("Print object ON"), wxDefaultPosition,
            wxDLG_UNIT(m_panelDisplay, wxSize(-1, -1)), 0);
        m_checkBoxPrintObjectOn->SetValue(false);
        m_checkBoxPrintObjectOn->SetToolTip(
            _("When displaying a pointer to an object, identify the actual (derived) type of the object rather than the "
                "declared type, using the virtual function table."));

        fgSizer21->Add(m_checkBoxPrintObjectOn, 0, wxALL, WXC_FROM_DIP(5));

        // Connect events
        m_buttonBrowse->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DebuggerPage::OnBrowse), NULL,
            this);
        m_checkBoxRunAsSuperuser->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPage::OnSuperuserUI), NULL,
            this);
        DebuggerInformation info;
        if (DebuggerMgr::Get().GetDebuggerInformation(m_title, info)) {
            m_textCtrDbgPath->SetValue(info.path);
            m_checkBoxEnablePendingBreakpoints->SetValue(info.enablePendingBreakpoints);
            m_checkBreakAtWinMain->SetValue(info.breakAtWinMain);
            m_catchThrow->SetValue(info.catchThrow);
            m_spinCtrlNumElements->SetValue(info.maxDisplayStringSize);
            m_showTooltipsRequiresControl->SetValue(info.showTooltipsOnlyWithControlKeyIsDown);
            m_checkBoxAutoExpand->SetValue(info.autoExpandTipItems);
            m_checkBoxExpandLocals->SetValue(info.resolveLocals);
            m_checkBoxSetBreakpointsAfterMain->SetValue(info.applyBreakpointsAfterProgramStarted);
            m_raiseOnBpHit->SetValue(info.whenBreakpointHitRaiseGSGS);
            m_checkBoxCharArrAsPtr->SetValue(info.charArrAsPtr);
            m_checkBoxUsePrettyPrinting->SetValue(info.enableGDBPrettyPrinting);
            m_checkBoxPrintObjectOn->SetValue(!(info.flags & DebuggerInformation::kPrintObjectOff));
            m_checkBoxRunAsSuperuser->SetValue(info.flags & DebuggerInformation::kRunAsSuperuser);
            m_checkBoxDefaultHexDisplay->SetValue(info.defaultHexDisplay);
        }
    }
    //-----------------------------------------------------------------------------
    DebuggerPage::~DebuggerPage()
    {
        m_buttonBrowse->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DebuggerPage::OnBrowse), NULL,
            this);
        m_checkBoxRunAsSuperuser->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPage::OnSuperuserUI),
            NULL, this);
    }
    //-----------------------------------------------------------------------------
    void DebuggerPage::OnBrowse(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxString newfilepath, filepath(m_textCtrDbgPath->GetValue());
        if ((!filepath.IsEmpty()) && wxFileName::FileExists(filepath)) {
            newfilepath = wxFileSelector(wxT("Select file:"), filepath.c_str());
        }
        else {
            newfilepath = wxFileSelector(wxT("Select file:"));
        }

        if (!newfilepath.IsEmpty()) { m_textCtrDbgPath->SetValue(newfilepath); }
    }
    //-----------------------------------------------------------------------------
    void DebuggerPage::OnDebugAssert(wxCommandEvent& e)
    {
        if (e.IsChecked()) { m_checkBoxEnablePendingBreakpoints->SetValue(true); }
    }
    //-----------------------------------------------------------------------------
    void DebuggerPage::OnWindowsUI(wxUpdateUIEvent& event)
    {
        // enabloe the Cygwin/MinGW part only when under Windows
        static bool OS_WINDOWS = wxGetOsVersion() & wxOS_WINDOWS ? true : false;
        event.Enable(OS_WINDOWS);
    }
    //-----------------------------------------------------------------------------
    void DebuggerPage::OnSuperuserUI(wxUpdateUIEvent& event)
    {
        event.Check(false);
        event.Enable(false);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerPageMisc
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    DebuggerPageMisc::DebuggerPageMisc(wxWindow* parent, const wxString& title, wxWindowID id, 
        const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style), m_title(title)
    {
        wxBoxSizer* bSizer19 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer19);

        m_panel7 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

        bSizer19->Add(m_panel7, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer17 = new wxBoxSizer(wxVERTICAL);
        m_panel7->SetSizer(bSizer17);

        wxStaticBoxSizer* sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(m_panel7, wxID_ANY, wxT("")), wxVERTICAL);

        bSizer17->Add(sbSizer2, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer10 = new wxBoxSizer(wxVERTICAL);

        sbSizer2->Add(boxSizer10, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkUseRelativePaths =
            new wxCheckBox(m_panel7, wxID_ANY, _("Use file name only for breakpoints (NO full paths)"), wxDefaultPosition,
                wxDLG_UNIT(m_panel7, wxSize(-1, -1)), 0);
        m_checkUseRelativePaths->SetValue(false);

        boxSizer10->Add(m_checkUseRelativePaths, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxEnableLog = new wxCheckBox(m_panel7, wxID_ANY, _("Enable full debugger logging"), wxDefaultPosition,
            wxDLG_UNIT(m_panel7, wxSize(-1, -1)), 0);
        m_checkBoxEnableLog->SetValue(false);

        boxSizer10->Add(m_checkBoxEnableLog, 0, wxALL, WXC_FROM_DIP(5));

        m_checkShowTerminal = new wxCheckBox(m_panel7, wxID_ANY, _("Show debugger terminal"), wxDefaultPosition,
            wxDLG_UNIT(m_panel7, wxSize(-1, -1)), 0);
        m_checkShowTerminal->SetValue(false);

        boxSizer10->Add(m_checkShowTerminal, 0, wxALL, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer4 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer10->Add(boxSizer4, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_staticText6 = gsgs_Style().createStaticText(m_panel7, wxID_ANY, _("Max number of frames to allow in a call-stack"),
            wxDefaultPosition, wxDLG_UNIT(m_panel7, wxSize(-1, -1)), 0);
        boxSizer4->Add(m_staticText6, 1, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_maxFramesSpinCtrl = new wxSpinCtrl(m_panel7, wxID_ANY, wxT("500"), wxDefaultPosition,
            wxDLG_UNIT(m_panel7, wxSize(70, -1)), wxSP_ARROW_KEYS);
        m_maxFramesSpinCtrl->SetToolTip(
            _("The maximum number of frames that GSGS will display in the Call Stack tab. This protects against a very "
                "long hang while trying to show 100,000 frames in an infinite recursion situation."));
        m_maxFramesSpinCtrl->SetRange(1, 999999);
        m_maxFramesSpinCtrl->SetValue(500);

        boxSizer4->Add(m_maxFramesSpinCtrl, 0, wxALL, WXC_FROM_DIP(5));

        wxStaticBoxSizer* sbSizer6 =
            new wxStaticBoxSizer(new wxStaticBox(m_panel7, wxID_ANY, _("MinGW / Cygwin:")), wxVERTICAL);

        bSizer17->Add(sbSizer6, 0, wxLEFT | wxRIGHT | wxEXPAND, WXC_FROM_DIP(5));

        wxGridSizer* gSizer5 = new wxGridSizer(0, 1, 0, 0);

        sbSizer6->Add(gSizer5, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxDebugAssert = new wxCheckBox(m_panel7, wxID_ANY, _("Break at assertion failure (MinGW only)"),
            wxDefaultPosition, wxDLG_UNIT(m_panel7, wxSize(-1, -1)), 0);
        m_checkBoxDebugAssert->SetValue(false);

        gSizer5->Add(m_checkBoxDebugAssert, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText5 = gsgs_Style().createStaticText(m_panel7, wxID_ANY, _("Cygwin path conversion command:"), wxDefaultPosition,
            wxDLG_UNIT(m_panel7, wxSize(-1, -1)), 0);
        m_staticText5->SetToolTip(_("Set here the command to use in order to convert cygwin paths into native Windows "
            "paths (use $(File) as a place holder for the file name)"));

        gSizer5->Add(m_staticText5, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrlCygwinPathCommand =
            gsgs_Style().createTextCtrl(m_panel7, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel7, wxSize(-1, -1)), 0);
        m_textCtrlCygwinPathCommand->SetToolTip(
            _("Set here the command to use in order to convert cygwin paths into native Windows paths (use $(File) as a "
                "place holder for the file name)"));

        gSizer5->Add(m_textCtrlCygwinPathCommand, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND | wxALIGN_CENTER_VERTICAL,
            WXC_FROM_DIP(5));

        // Connect events
        m_checkBoxDebugAssert->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(DebuggerPageMisc::OnDebugAssert), NULL, this);
        m_checkBoxDebugAssert->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPageMisc::OnWindowsUI), NULL, this);
        m_staticText5->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPageMisc::OnWindowsUI), NULL, this);
        m_textCtrlCygwinPathCommand->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPageMisc::OnWindowsUI), NULL,
            this);

        DebuggerInformation info;
        if (DebuggerMgr::Get().GetDebuggerInformation(title, info)) {
            m_checkBoxEnableLog->SetValue(info.enableDebugLog);
            m_checkShowTerminal->SetValue(info.showTerminal);
            m_checkUseRelativePaths->SetValue(info.useRelativeFilePaths);
            m_maxFramesSpinCtrl->SetValue(info.maxCallStackFrames);
#ifdef __WXMSW__
            m_checkBoxDebugAssert->SetValue(info.debugAsserts);
#endif
            m_textCtrlCygwinPathCommand->SetValue(info.cygwinPathCommand);
        }

#ifndef __WXMSW__
        m_checkBoxDebugAssert->SetValue(false);
        m_checkBoxDebugAssert->Enable(false);
        m_textCtrlCygwinPathCommand->Enable(false);
        m_staticText5->Enable(false);
#endif
    }
    //-----------------------------------------------------------------------------
    DebuggerPageMisc::~DebuggerPageMisc()
    {
        m_checkBoxDebugAssert->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(DebuggerPageMisc::OnDebugAssert), NULL, this);
        m_checkBoxDebugAssert->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPageMisc::OnWindowsUI), NULL,
            this);
        m_staticText5->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPageMisc::OnWindowsUI), NULL, this);
        m_textCtrlCygwinPathCommand->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerPageMisc::OnWindowsUI), NULL,
            this);
    }
    //-----------------------------------------------------------------------------
    void DebuggerPageMisc::OnDebugAssert(wxCommandEvent& event) {}
    //-----------------------------------------------------------------------------
    void DebuggerPageMisc::OnWindowsUI(wxUpdateUIEvent& event) {}
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DbgPagePreDefTypes
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    DbgPagePreDefTypes::DbgPagePreDefTypes(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style)
    {
        wxBoxSizer* bSizer21 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer21);

        m_panel6 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

        bSizer21->Add(m_panel6, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer16 = new wxBoxSizer(wxVERTICAL);
        m_panel6->SetSizer(bSizer16);

        wxBoxSizer* bSizer14 = new wxBoxSizer(wxHORIZONTAL);

        bSizer16->Add(bSizer14, 0, wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_buttonNewSet =
            new Button(m_panel6, wxID_NEW, _("&New"), wxDefaultPosition, wxDLG_UNIT(m_panel6, wxSize(-1, -1)), 0);
        m_buttonNewSet->SetToolTip(_("Create new 'PreDefined' set"));

        bSizer14->Add(m_buttonNewSet, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_buttonDeleteSet =
            new Button(m_panel6, wxID_DELETE, _("&Delete"), wxDefaultPosition, wxDLG_UNIT(m_panel6, wxSize(-1, -1)), 0);
        m_buttonDeleteSet->SetToolTip(_("Delete the currently selected set"));

        bSizer14->Add(m_buttonDeleteSet, 0, wxALL, WXC_FROM_DIP(5));

        wxStaticBoxSizer* sbSizer5 =
            new wxStaticBoxSizer(new wxStaticBox(m_panel6, wxID_ANY, _("Available sets:")), wxVERTICAL);

        bSizer16->Add(sbSizer5, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_notebookPreDefTypes =
            new wxChoicebook(m_panel6, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel6, wxSize(-1, -1)), 0);
        m_notebookPreDefTypes->SetName(wxT("m_notebookPreDefTypes"));

        sbSizer5->Add(m_notebookPreDefTypes, 1, wxEXPAND, WXC_FROM_DIP(5));

        // Connect events
        m_buttonNewSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DbgPagePreDefTypes::OnNewSet), NULL,
            this);
        m_buttonDeleteSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DbgPagePreDefTypes::OnDeleteSet),
            NULL, this);
        m_buttonDeleteSet->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DbgPagePreDefTypes::OnDeleteSetUI), NULL,
            this);
        m_notebookPreDefTypes->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED,
            wxChoicebookEventHandler(DbgPagePreDefTypes::OnPageChanged), NULL, this);

        // add items from the saved items
        DebuggerSettingsPreDefMap data;
        gsgs_Config().Read(wxT("DebuggerCommands"), &data);

        std::map<wxString, DebuggerPreDefinedTypes>::const_iterator iter = data.GePreDefinedTypesMap().begin();
        for (; iter != data.GePreDefinedTypesMap().end(); iter++) {
            m_notebookPreDefTypes->AddPage(new PreDefinedTypesPage(m_notebookPreDefTypes, iter->second), iter->first,
                iter->second.IsActive());
        }
    }
    //-----------------------------------------------------------------------------
    DbgPagePreDefTypes::~DbgPagePreDefTypes()
    {
        m_buttonNewSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DbgPagePreDefTypes::OnNewSet),
            NULL, this);
        m_buttonDeleteSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(DbgPagePreDefTypes::OnDeleteSet), NULL, this);
        m_buttonDeleteSet->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DbgPagePreDefTypes::OnDeleteSetUI), NULL,
            this);
        m_notebookPreDefTypes->Disconnect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED,
            wxChoicebookEventHandler(DbgPagePreDefTypes::OnPageChanged), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void DbgPagePreDefTypes::Save()
    {
        // copy the commands the serialized object m_data
        DebuggerSettingsPreDefMap preDefMap;
        std::map<wxString, DebuggerPreDefinedTypes> typesMap;

        for (size_t i = 0; i < m_notebookPreDefTypes->GetPageCount(); i++) {
            PreDefinedTypesPage* page = dynamic_cast<PreDefinedTypesPage*>(m_notebookPreDefTypes->GetPage(i));
            if (page) {
                DebuggerPreDefinedTypes types = page->GetPreDefinedTypes();
                types.SetActive(i == (size_t)m_notebookPreDefTypes->GetSelection());
                typesMap[types.GetName()] = types;
            }
        }
        preDefMap.SePreDefinedTypesMap(typesMap);

        // save the debugger commands
        gsgs_Config().Write(wxT("DebuggerCommands"), &preDefMap);
    }
    //-----------------------------------------------------------------------------
    void DbgPagePreDefTypes::OnDeleteSet(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        int sel = m_notebookPreDefTypes->GetSelection();
        if (sel == wxNOT_FOUND) return;

        wxString name = m_notebookPreDefTypes->GetPageText((size_t)sel);
        if (wxMessageBox(
            wxString::Format(wxT("You are about to delete 'PreDefined Types' set '%s'\nContinue ?"), name.c_str()),
            wxT("Confirm deleting 'PreDefined Types' set"), wxYES_NO | wxCENTER | wxICON_QUESTION, this) == wxYES) {
            m_notebookPreDefTypes->DeletePage((size_t)sel);
        }
    }
    //-----------------------------------------------------------------------------
    void DbgPagePreDefTypes::OnDeleteSetUI(wxUpdateUIEvent& event)
    {
        int sel = m_notebookPreDefTypes->GetSelection();
        event.Enable(sel != wxNOT_FOUND && m_notebookPreDefTypes->GetPageText((size_t)sel) != wxT("Default"));
    }
    //-----------------------------------------------------------------------------
    void DbgPagePreDefTypes::OnNewSet(wxCommandEvent& event)
    {
        NewPreDefinedSetDlg dlg(this);
        dlg.GetCheckBoxMakeActive()->SetValue(false);

        wxArrayString copyFromArr;
        // Make sure that a set with this name does not already exists
        copyFromArr.Add(wxT("None"));
        for (size_t i = 0; i < m_notebookPreDefTypes->GetPageCount(); i++) {
            copyFromArr.Add(m_notebookPreDefTypes->GetPageText((size_t)i));
        }
        dlg.GetChoiceCopyFrom()->Append(copyFromArr);
        dlg.GetChoiceCopyFrom()->SetSelection(0);
        dlg.GetTextCtrlName()->SetFocus();

        if (dlg.ShowModal() == wxID_OK) {
            wxString newName = dlg.GetTextCtrlName()->GetValue();
            newName.Trim().Trim(false);
            if (newName.IsEmpty()) return;

            // Make sure that a set with this name does not already exists
            for (size_t i = 0; i < m_notebookPreDefTypes->GetPageCount(); i++) {
                if (m_notebookPreDefTypes->GetPageText((size_t)i) == newName) {
                    wxMessageBox(wxT("A set with this name already exist"), wxT("Name Already Exists"),
                        wxICON_WARNING | wxOK | wxCENTER);
                    return;
                }
            }

            DebuggerPreDefinedTypes initialValues;
            wxString copyFrom = dlg.GetChoiceCopyFrom()->GetStringSelection();
            if (copyFrom != wxT("None")) {
                for (size_t i = 0; i < m_notebookPreDefTypes->GetPageCount(); i++) {
                    PreDefinedTypesPage* page = dynamic_cast<PreDefinedTypesPage*>(m_notebookPreDefTypes->GetPage(i));
                    if (page && m_notebookPreDefTypes->GetPageText(i) == copyFrom) {
                        initialValues = page->GetPreDefinedTypes();
                        break;
                    }
                }
            }

            initialValues.SetName(newName);
            m_notebookPreDefTypes->AddPage(new PreDefinedTypesPage(m_notebookPreDefTypes, initialValues),
                initialValues.GetName(), dlg.GetCheckBoxMakeActive()->IsChecked());
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // BuildSettingsDialog
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BuildSettingsDialog::BuildSettingsDialog(wxWindow* parent, size_t selected_page, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style) : 
        wxDialog(parent, id, title, pos, size, style)
    {
        wxWindowUpdateLocker llt(this);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_rightclickMenu = wxXmlResource::Get()->LoadMenu(wxT("delete_compiler_menu"));

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_notebook = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxSize(500, 300), gsgsNookbookStyle);
        m_notebook->SetName(wxT("m_notebook"));

        mainSizer->Add(m_notebook, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_stdBtnSizer2 = new DialogButtonSizer();

        mainSizer->Add(m_stdBtnSizer2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer2->AddButton(m_buttonCancel);

        m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer2->AddButton(m_buttonOK);

        m_buttonApply = new Button(this, wxID_APPLY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer2->AddButton(m_buttonApply);
        m_stdBtnSizer2->Realize();

        m_compilersPage = new CompilerMainPage(m_notebook);
        m_notebook->AddPage(m_compilersPage, _("Compilers"), true);

        m_buildSettings = new BuildTabSetting(m_notebook);
        m_notebook->AddPage(m_buildSettings, _("Build Output Appearance"));

        m_buildPage = new BuildPage(m_notebook);
        m_notebook->AddPage(m_buildPage, _("Build Systems"));

        // for each debugger, add page
        DebuggerPage* p = new DebuggerPage(m_notebook, "GNU gdb debugger");
        m_notebook->AddPage(p, wxT("GNU gdb debugger"));

        Notebook* innerBook = p->GetNotebook73();
        DebuggerPageMisc* misc = new DebuggerPageMisc(innerBook, "GNU gdb debugger");
        innerBook->AddPage(misc, wxT("Misc"));

        DebuggerPageStartupCmds* cmds = new DebuggerPageStartupCmds(innerBook, "GNU gdb debugger");
        innerBook->AddPage(cmds, wxT("Startup Commands"));

        DbgPagePreDefTypes* types = new DbgPagePreDefTypes(m_notebook);
        m_notebook->AddPage(types, wxT("Pre-Defined Types"));

        if (GetParent()) {
            CentreOnParent();
        }
        else {
            CentreOnScreen();
        }
        // Connect events
        m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BuildSettingsDialog::OnButtonOKClicked), NULL, this);
        m_buttonApply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BuildSettingsDialog::OnApply), NULL, this);
        m_buttonApply->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BuildSettingsDialog::OnApplyUI), NULL, this);

        m_pages.push_back(p);
        m_pages.push_back(misc);
        m_pages.push_back(cmds);
        m_pages.push_back(types);
        LoadCompilers();
    }
    //-----------------------------------------------------------------------------
    BuildSettingsDialog::~BuildSettingsDialog()
    {
        m_buttonOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BuildSettingsDialog::OnButtonOKClicked), NULL, this);
        m_buttonApply->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BuildSettingsDialog::OnApply), NULL, this);
        m_buttonApply->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BuildSettingsDialog::OnApplyUI), NULL, this);
        wxDELETE(m_rightclickMenu);
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::LoadCompilers() 
    { 
        m_compilersPage->LoadCompilers(); 
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnButtonNewClicked()
    {
        NewCompilerDlg dlg(this);
        if(mCurrentCompiler.size())
            dlg.GetChoiceCompilers()->SetStringSelection(mCurrentCompiler);
        if(dlg.ShowModal() == wxID_OK) {
            CreateNewCompiler(dlg.GetCompilerName(), dlg.GetMasterCompiler());
            LoadCompilers();
        }
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnButtonOKClicked(wxCommandEvent& event)
    {
        OnApply(event);
        // go over the debuggers and set the debugger path
        for (size_t i = 0; i < (size_t)m_pages.size(); ++i) 
        {
            wxWindow* win = m_pages[i];
            if (!win) continue;

            DebuggerPage* page = dynamic_cast<DebuggerPage*>(win);
            if (page) {
                // find the debugger
                DebuggerInformation info;
                DebuggerMgr::Get().GetDebuggerInformation(page->m_title, info);

                // populate the information and save it
                info.enablePendingBreakpoints = page->m_checkBoxEnablePendingBreakpoints->GetValue();
                info.path = page->m_textCtrDbgPath->GetValue();
                info.name = page->m_title;
                info.breakAtWinMain = page->m_checkBreakAtWinMain->IsChecked();
                info.consoleCommand = gsgs_EditorConfig().GetOptions()->GetProgramConsoleCommand();
                info.catchThrow = page->m_catchThrow->IsChecked();
                info.showTooltipsOnlyWithControlKeyIsDown = page->m_showTooltipsRequiresControl->IsChecked();
                info.maxDisplayStringSize = page->m_spinCtrlNumElements->GetValue();
                info.resolveLocals = page->m_checkBoxExpandLocals->IsChecked();
                info.autoExpandTipItems = page->m_checkBoxAutoExpand->IsChecked();
                info.applyBreakpointsAfterProgramStarted = page->m_checkBoxSetBreakpointsAfterMain->IsChecked();
                info.whenBreakpointHitRaiseGSGS = page->m_raiseOnBpHit->IsChecked();
                info.charArrAsPtr = page->m_checkBoxCharArrAsPtr->IsChecked();
                info.enableGDBPrettyPrinting = page->m_checkBoxUsePrettyPrinting->IsChecked();
                info.defaultHexDisplay = page->m_checkBoxDefaultHexDisplay->IsChecked();

                // Update the flags
                if (page->m_checkBoxPrintObjectOn->IsChecked()) {
                    info.flags &= ~DebuggerInformation::kPrintObjectOff;
                }
                else {
                    info.flags |= DebuggerInformation::kPrintObjectOff;
                }
                if (page->m_checkBoxRunAsSuperuser->IsChecked()) {
                    info.flags |= DebuggerInformation::kRunAsSuperuser;
                }
                else {
                    info.flags &= ~DebuggerInformation::kRunAsSuperuser;
                }
                DebuggerMgr::Get().SetDebuggerInformation(page->m_title, info);
            }

            DebuggerPageMisc* miscPage = dynamic_cast<DebuggerPageMisc*>(win);
            if (miscPage) {
                // find the debugger
                DebuggerInformation info;
                DebuggerMgr::Get().GetDebuggerInformation(miscPage->m_title, info);

                // populate the information and save it
                info.enableDebugLog = miscPage->m_checkBoxEnableLog->GetValue();
                info.showTerminal = miscPage->m_checkShowTerminal->IsChecked();
                info.useRelativeFilePaths = miscPage->m_checkUseRelativePaths->IsChecked();
                info.maxCallStackFrames = miscPage->m_maxFramesSpinCtrl->GetValue();
#ifdef __WXMSW__
                info.debugAsserts = miscPage->m_checkBoxDebugAssert->IsChecked();
#endif
                info.cygwinPathCommand = miscPage->m_textCtrlCygwinPathCommand->GetValue();
                DebuggerMgr::Get().SetDebuggerInformation(miscPage->m_title, info);
            }

            DebuggerPageStartupCmds* suCmds = dynamic_cast<DebuggerPageStartupCmds*>(win);
            if (suCmds) {
                // find the debugger
                DebuggerInformation info;
                DebuggerMgr::Get().GetDebuggerInformation(suCmds->m_title, info);
                info.startupCommands = suCmds->m_textCtrlStartupCommands->GetText();
                DebuggerMgr::Get().SetDebuggerInformation(suCmds->m_title, info);
            }

            DbgPagePreDefTypes* pd = dynamic_cast<DbgPagePreDefTypes*>(win);
            if (pd) { pd->Save(); }
        }

        EndModal(wxID_OK);
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::SaveCompilers()
    {
        std::map<wxString, std::vector<CompilerPage*> >::const_iterator iter = m_compilerPagesMap.begin();
        for(; iter != m_compilerPagesMap.end(); iter++) {
            //wxString cmpname = iter->first;
            CompilerPtr cmp = gsgs_Settings().GetCompiler(iter->first);
            if(cmp) {
                const std::vector<CompilerPage*> & items = iter->second;
                for(size_t i = 0; i < items.size(); i++) {
                    CompilerPage* p = items.at(i);
                    p->Save(cmp);
                }
                gsgs_Settings().SetCompiler(cmp); // save changes
            }
        }
    }
    //-----------------------------------------------------------------------------
    bool BuildSettingsDialog::CreateNewCompiler(const wxString& name, const wxString& copyFrom)
    {
        if(gsgs_Settings().IsCompilerExist(name)) {
            wxMessageBox(_("A compiler with this name already exists"), _("Error"), wxOK | wxICON_HAND);
            return false;
        }

        CompilerPtr cmp;
        if (copyFrom.IsEmpty())
        {
            cmp = gsgs_Settings().GetCompiler(name);
        }
        else
        {
            cmp = gsgs_Settings().GetCompiler(copyFrom);
        }
        cmp->SetName(name);
        gsgs_Settings().SetCompiler(cmp);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool BuildSettingsDialog::DeleteCompiler(const wxString& name)
    {
        if(wxMessageBox(_("Remove Compiler?"), _("Confirm"), wxYES_NO | wxICON_QUESTION) == wxYES) {
            gsgs_Settings().DeleteCompiler(name);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnContextMenu(wxContextMenuEvent& e)
    {
        //    wxTreeCtrl *tree = m_compilersNotebook->GetTreeCtrl();
        //    wxTreeItemId item = tree->GetSelection();
        //
        //    // only compilers have children
        //    if(item.IsOk() && tree->HasChildren(item)) {
        //        PopupMenu(m_rightclickMenu);
        //    }
    }
    //-----------------------------------------------------------------------------
    #define ID_MENU_AUTO_DETECT_COMPILERS 1001
    #define ID_MENU_ADD_COMPILER_BY_PATH 1002
    #define ID_MENU_CLONE_COMPILER 1003
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnAutoDetectCompilers(Button* btn)
    {
        // Launch the auto detect compilers code

        wxMenu menu;
        menu.Append(ID_MENU_ADD_COMPILER_BY_PATH, _("Add an existing compiler"));
        menu.Append(ID_MENU_CLONE_COMPILER, _("Clone a compiler"));
        menu.AppendSeparator();
        menu.Append(ID_MENU_AUTO_DETECT_COMPILERS, _("Scan computer for installed compilers"));

        // Menu will be shown in client coordinates
        wxRect size = btn->GetSize();
        wxPoint menuPos(0, size.GetHeight());

        int res = btn->GetPopupMenuSelectionFromUser(menu, menuPos);
        if(res == ID_MENU_AUTO_DETECT_COMPILERS) {
            if(m_compilersDetector.Locate()) {
                CallAfter(&BuildSettingsDialog::OnCompilersDetected, m_compilersDetector.GetCompilersFound());
            }

        } else if(res == ID_MENU_CLONE_COMPILER) {
            mCurrentCompiler = "";
            CallAfter(&BuildSettingsDialog::OnButtonNewClicked);

        } else if(res == ID_MENU_ADD_COMPILER_BY_PATH) {
            CallAfter(&BuildSettingsDialog::OnAddExistingCompiler);
        }
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnCompilersDetected(const ToolsetLocator::CompilerVec_t& compilers)
    {
        CompilersFoundDlg dlg(this, compilers);
        if(dlg.ShowModal() == wxID_OK) {
            // Replace the current compilers with a new one
            gsgs_Settings().SetCompilers(compilers);

            // update the code completion search paths
            gsgs_Frame()->CallAfter(&Frame::UpdateParserSearchPathsFromDefaultCompiler);

            // Dismiss this dialog and reload it
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("advance_settings"));
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(event);
            EndModal(wxID_OK);
        }
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnApply(wxCommandEvent&)
    {
        // save the build page
        m_compilersPage->Save();
        m_buildPage->Save();
        m_buildSettings->Save();

        // mark all the projects as dirty
        wxArrayString projects;
        gsgs_CppWorker().GetProjectList(projects);
        for(size_t i = 0; i < projects.size(); i++) 
        {
            ProjectPtr proj = gsgs_Manager()->GetProject(projects.Item(i));
            if(proj) 
            {
                proj->SetModified(true);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnApplyUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_compilersPage->IsDirty() || m_buildSettings->IsModified());
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnAddExistingCompiler()
    {
        wxString folder = ::wxDirSelector(_("Select the compiler folder"));
        if(folder.IsEmpty()) {
            return;
        }

        CompilerPtr cmp = m_compilersDetector.Locate(folder);
        if(cmp) {
            // We found new compiler
            // Prompt the user to give it a name
            while(true) {
                wxString name =
                    ::wxGetTextFromUser(_("Set a name to the compiler"), _("New compiler found!"), cmp->GetName());
                if(name.IsEmpty()) {
                    return;
                }
                // Add the compiler
                if(gsgs_Settings().IsCompilerExist(name)) {
                    continue;
                }
                cmp->SetName(name);
                break;
            }

            // Save the new compiler
            gsgs_Settings().SetCompiler(cmp);

            // Reload the dialog
            LoadCompilers();
        }
    }
    //-----------------------------------------------------------------------------
    void BuildSettingsDialog::OnScanAndSuggestCompilers()
    {
        if(m_compilersDetector.Locate()) {
            CallAfter(&BuildSettingsDialog::OnCompilersDetected, m_compilersDetector.GetCompilersFound());
        }
    }
    //-----------------------------------------------------------------------------
}