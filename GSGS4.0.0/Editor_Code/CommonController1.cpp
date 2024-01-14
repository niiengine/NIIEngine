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

#include "gsgsPreProcess.h"
#include "CommonController1.h"
#include "CommonController.h"
#include "gsgsFileUtil.h"
#include "NewProjectDialog.h"
#include "gsgsPluginManager.h"
#include "gsgsWindowManager.h"
#include "gsgsImageManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsFrame.h"
#include "gsgsLexerStyle.h"
#include "gsgsConfig.h"
#include "gsgsDebuggerManager.h"
#include "gsgsBuildConfig.h"
#include "gsgsBuilderGnuMake.h"
#include "gsgsProcess.h"
#include "gsgsCommon.h"
#include "gsgsWorkspacePane.h"
#include "gsgsCreateWorkspace.h"
#include "gsgsCommonHandler.h"
#include "gsgsToolsetManager.h"
#include "gsgsCommonDialog.h"
#include "gsgsBuildTab.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsFilePicker.h"
#include "gsgsSearchWorker.h"
#include "gsgsSessionManager.h"
#include "gsgsStyleManager.h"
#include "gsgsWorkspace.h"
#include "gsgsTagManager.h"
#include "gsgsPluginCommon.h"
#include "debuggerobserver.h"
#include "gsgsPathManager.h"
#include "EclipseThemeImporterManager.h"
#include "gsgsZip.h"
#include "gsgsTree.h"
#include "gsgsContextManager.h"
#include <wx/choicdlg.h>
#include <wx/notebook.h>
#include <wx/richtooltip.h>
#include <wx/treebook.h>
#include <wx/busyinfo.h>
#include <wx/generic/textdlgg.h>
#include <wx/persist.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>
#include "wx/persist/window.h"
#include "wx/popupwin.h"
#include <wx/cursor.h>
#include <wx/menu.h>
#include <wx/timer.h>
#include <wx/xrc/xmlres.h>
#include <wx/display.h>
#include "pp_include.h"
#include "pptable.h"
#include "ieditor.h"
#include "plugin.h"
#include "gsgsManager.h"
#include "imanager.h"
#include "globals.h"
#include <wx/fontmap.h>
#include <wx/dirdlg.h>
#include <wx/any.h>
#include <wx/fontdlg.h>
#include <wx/textdlg.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/regex.h>
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// AddIncludeFileDlg
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
wxArrayString AddIncludeFileDlg::m_includePath;
//-------------------------------------------------------------------------------
struct SAscendingSort {
    bool operator()(const wxString& s1, const wxString& s2) { return s1.Len() > s2.Len(); }
};
//-------------------------------------------------------------------------------
AddIncludeFileDlg::AddIncludeFileDlg(wxWindow* parent, const wxString& fullpath, const wxString& text, int lineNo, wxWindowID id, const wxString& title, const wxPoint& pos,
                                             const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_fullpath(fullpath)
    , m_text(text)
    , m_line(lineNo)
{
    wxBoxSizer* bSizer7 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer7);

    m_toolbar = new gsgs::ToolBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTB_FLAT);
    m_toolbar->SetToolBitmapSize(wxSize(16, 16));

    bSizer7->Add(m_toolbar, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

    bSizer7->Add(m_mainPanel, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer9 = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(bSizer9);

    m_textCtrlPreview =
        new wxStyledTextCtrl(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_mainPanel, wxSize(400, 300)), 0);
#ifdef __WXMSW__
    // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to
    // wxFONTFAMILY_TELETYPE
    wxFont m_textCtrlPreviewFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_textCtrlPreviewFont.SetFamily(wxFONTFAMILY_TELETYPE);
#else
    wxFont m_textCtrlPreviewFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    m_textCtrlPreviewFont.SetFamily(wxFONTFAMILY_TELETYPE);
#endif
    m_textCtrlPreview->SetFont(m_textCtrlPreviewFont);
    m_textCtrlPreview->SetFocus();
    // Configure the fold margin
    m_textCtrlPreview->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_textCtrlPreview->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_textCtrlPreview->SetMarginSensitive(4, true);
    m_textCtrlPreview->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_textCtrlPreview->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_textCtrlPreview->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_textCtrlPreview->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_textCtrlPreview->SetMarginWidth(2, 0);
    m_textCtrlPreview->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    int m_textCtrlPreview_PixelWidth = 4 + 5 * m_textCtrlPreview->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_textCtrlPreview->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_textCtrlPreview->SetMarginWidth(0, m_textCtrlPreview_PixelWidth);

    // Configure the line symbol margin
    m_textCtrlPreview->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_textCtrlPreview->SetMarginMask(3, 0);
    m_textCtrlPreview->SetMarginWidth(3, 0);
    // Select the lexer
    m_textCtrlPreview->SetLexer(wxSTC_LEX_CPP);
    // Set default font / styles
    m_textCtrlPreview->StyleClearAll();
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_textCtrlPreview->StyleSetFont(i, m_textCtrlPreviewFont);
    }
    m_textCtrlPreview->SetWrapMode(0);
    m_textCtrlPreview->SetIndentationGuides(0);
    m_textCtrlPreview->SetKeyWords(0, wxT(""));
    m_textCtrlPreview->SetKeyWords(1, wxT(""));
    m_textCtrlPreview->SetKeyWords(2, wxT(""));
    m_textCtrlPreview->SetKeyWords(3, wxT(""));
    m_textCtrlPreview->SetKeyWords(4, wxT(""));

    bSizer9->Add(m_textCtrlPreview, 1, wxLEFT | wxRIGHT | wxEXPAND, WXC_FROM_DIP(5));

    wxFlexGridSizer* flexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer4->SetFlexibleDirection(wxBOTH);
    flexGridSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer4->AddGrowableCol(1);

    bSizer9->Add(flexGridSizer4, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText21 = gsgs_Style().createStaticText(m_mainPanel, wxID_ANY, _("Line to add:"), wxDefaultPosition,
                                      wxDLG_UNIT(m_mainPanel, wxSize(-1, -1)), 0);
    wxFont m_staticText21Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticText21Font.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticText21->SetFont(m_staticText21Font);

    flexGridSizer4->Add(m_staticText21, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticTextPreview =
        gsgs_Style().createStaticText(m_mainPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_mainPanel, wxSize(-1, -1)), 0);
#ifdef __WXMSW__
    // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to
    // wxFONTFAMILY_TELETYPE
    wxFont m_staticTextPreviewFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticTextPreviewFont.SetFamily(wxFONTFAMILY_TELETYPE);
#else
    wxFont m_staticTextPreviewFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    m_staticTextPreviewFont.SetFamily(wxFONTFAMILY_TELETYPE);
#endif
    m_staticTextPreview->SetFont(m_staticTextPreviewFont);

    flexGridSizer4->Add(m_staticTextPreview, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticText5 = gsgs_Style().createStaticText(m_mainPanel, wxID_ANY, _("File's full path:"), wxDefaultPosition,
                                     wxDLG_UNIT(m_mainPanel, wxSize(-1, -1)), 0);
    wxFont m_staticText5Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticText5Font.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticText5->SetFont(m_staticText5Font);

    flexGridSizer4->Add(m_staticText5, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlFullPath = gsgs_Style().createTextCtrl(m_mainPanel, wxID_ANY, wxT(""), wxDefaultPosition,
                                        wxDLG_UNIT(m_mainPanel, wxSize(400, -1)), wxTE_READONLY);

    flexGridSizer4->Add(m_textCtrlFullPath, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_stdBtnSizer23 = new DialogButtonSizer();

    bSizer7->Add(m_stdBtnSizer23, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

    m_button25 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button25->SetDefault();
    m_stdBtnSizer23->AddButton(m_button25);

    m_button27 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer23->AddButton(m_button27);
    m_stdBtnSizer23->Realize();

    if(GetParent()) {
        CentreOnParent();
    } else {
        CentreOnScreen();
    }

    // Connect events
    m_button25->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddIncludeFileDlg::OnButtonOK), NULL,
                        this);
                        
    // Initialise the preview window
    UpdateLineToAdd();
    LexerStyle::Ptr_t cppLex = gsgs_EditorConfig().GetLexer("C++");
    cppLex->Apply(m_textCtrlPreview, true);

    m_toolbar->AddTool(wxID_UP, _("Move Up"), gsgs_Image().LoadBitmap("up"));
    m_toolbar->AddTool(wxID_DOWN, _("Move Up"), gsgs_Image().LoadBitmap("down"));
    m_toolbar->AddTool(wxID_CLEAR, _("Clear"), gsgs_Image().LoadBitmap("clear"));
    m_toolbar->Bind(wxEVT_TOOL, &AddIncludeFileDlg::OnButtonUp, this, wxID_UP);
    m_toolbar->Bind(wxEVT_TOOL, &AddIncludeFileDlg::OnButtonDown, this, wxID_DOWN);
    m_toolbar->Bind(wxEVT_TOOL, &AddIncludeFileDlg::OnClearCachedPaths, this, wxID_CLEAR);
    m_toolbar->Realize();
    //---------------------------------------------------------
    m_textCtrlPreview->MarkerDefine(0x7, wxSTC_MARK_ARROW);
    m_textCtrlPreview->MarkerSetBackground(0x7, wxT("YELLOW GREEN"));
    m_textCtrlPreview->MarkerSetAlpha(0x7, 70);

    m_textCtrlPreview->Bind(wxEVT_KEY_DOWN, &AddIncludeFileDlg::OnPreviewKeyDown, this);

    // Set the initial text
    m_textCtrlPreview->SetReadOnly(false);
    m_textCtrlPreview->AddText(m_text);
    SetAndMarkLine();
    m_textCtrlPreview->EmptyUndoBuffer();
    m_textCtrlPreview->SetFocus();

    // Only call OnModified when text was deleted or added
    Bind(wxEVT_IDLE, &AddIncludeFileDlg::OnIdle, this);

    int numOfLinesVisible = m_textCtrlPreview->LinesOnScreen();
    int firstVisibleLine = m_line - (numOfLinesVisible / 2);
    if(firstVisibleLine < 0) { firstVisibleLine = 0; }
    m_textCtrlPreview->SetFirstVisibleLine(firstVisibleLine);
}
//-------------------------------------------------------------------------------
AddIncludeFileDlg::~AddIncludeFileDlg() 
{
    m_button25->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddIncludeFileDlg::OnButtonOK), NULL,
        this);
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::UpdateLineToAdd()
{
    wxString line;
    wxFileName fn(m_fullpath);
    m_textCtrlFullPath->ChangeValue(fn.GetFullPath());

    // try to get a match in the include path for this file
    wxString pp = fn.GetFullPath();
    pp.Replace(wxT("\\"), wxT("/"));

    wxString rest;
    for(size_t i = 0; i < m_includePath.GetCount(); i++) {
        if(pp.StartsWith(m_includePath.Item(i), &rest)) { break; }
    }

    if(rest.IsEmpty()) { rest = fn.GetFullName(); }

    wxString errMsg;
    wxString projectName = gsgs_Frame()->GetMainBook()->GetActiveEditor()->GetProject();
    ProjectPtr proj = gsgs_CppWorker().FindProjectByName(projectName, errMsg);
    if(proj) {
        wxArrayString incls = proj->GetIncludePaths();
        std::sort(incls.begin(), incls.end(), SAscendingSort());

        for(size_t i = 0; i < incls.GetCount(); i++) {
            wxString path = incls.Item(i);
#ifdef __WXMSW__
            path.MakeLower();
#endif
            if(m_fullpath.StartsWith(path, &rest)) { break; }
        }
    }

    rest.Replace(wxT("\\"), wxT("/"));
    if(rest.StartsWith(wxT("/"))) { rest.Remove(0, 1); }

    if(!gsgs_Manager()->IsFileInWorkspace(m_fullpath)) {
        line << wxT("#include <") << rest << wxT(">");

    } else {
        line << wxT("#include \"") << rest << wxT("\"");
    }
    m_lineToAdd = line;
    // m_textCtrlLineToAdd->ChangeValue(line);
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::SetAndMarkLine()
{
// restore the initial text
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(m_textCtrlPreview);
#endif

    m_textCtrlPreview->SetReadOnly(false);

    // Make the line to add at the center of the display
    int numOfLinesVisible = m_textCtrlPreview->LinesOnScreen();
    int firstVisibleLine = m_line - (numOfLinesVisible / 2);
    if(firstVisibleLine < 0) { firstVisibleLine = 0; }

    m_textCtrlPreview->MarkerDeleteAll(0x7);
    m_textCtrlPreview->SetText(m_text);
    long pos = m_textCtrlPreview->PositionFromLine(m_line);
    m_textCtrlPreview->InsertText(pos, m_lineToAdd + wxT("\n"));
    m_textCtrlPreview->MarkerAdd(m_line, 0x7);
    m_textCtrlPreview->SetFirstVisibleLine(firstVisibleLine);
    m_textCtrlPreview->SetCurrentPos(pos);
    m_textCtrlPreview->SetSelectionStart(pos);
    m_textCtrlPreview->SetSelectionEnd(pos);
    m_textCtrlPreview->EmptyUndoBuffer();
    m_textCtrlPreview->SetReadOnly(true);
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::OnTextUpdated(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SetAndMarkLine();
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::OnButtonDown(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_line + 2 >= m_textCtrlPreview->GetLineCount()) { return; }
    m_line++;
    SetAndMarkLine();
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::OnButtonUp(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_line - 1 < 0) { return; }
    m_line--;
    SetAndMarkLine();
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::OnButtonOK(wxCommandEvent& e)
{
    wxUnusedVar(e);
    // get the include file to add
    wxString fullpath = m_textCtrlFullPath->GetValue();
    static wxRegEx reIncludeFile(wxT("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.]*)"));
    wxString relativePath;

    if(reIncludeFile.Matches(m_lineToAdd)) { relativePath = reIncludeFile.GetMatch(m_lineToAdd, 1); }

    fullpath.Replace(wxT("\\"), wxT("/"));
    relativePath.Replace(wxT("\\"), wxT("/"));
    wxFileName fn(fullpath);

    wxString inclPath;
    if(fullpath.EndsWith(relativePath, &inclPath) &&
       fullpath != relativePath &&         // don't save the '.' path this is done by default
       fn.GetFullName() != relativePath) { // if the relative path is only file name, nothing to cache
        m_includePath.Add(inclPath);
    }
    EndModal(wxID_OK);
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::OnClearCachedPaths(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_includePath.Clear();
    UpdateLineToAdd();
    SetAndMarkLine();
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::OnPreviewKeyDown(wxKeyEvent& event)
{
    event.Skip(false);
    m_textCtrlPreview->SetEditable(false);
    bool isOnMarkerLine = m_line == m_textCtrlPreview->GetCurrentLine();
    wxCommandEvent dummy;
    if(event.GetKeyCode() == WXK_DOWN) {
        // update the line to add string and move the line down
        // m_lineToAdd = m_textCtrlPreview->GetLine(m_line);
        OnButtonDown(dummy);

    } else if(event.GetKeyCode() == WXK_UP) {
        // update the line to add string and move the line up
        // m_lineToAdd = m_textCtrlPreview->GetLine(m_line);
        OnButtonUp(dummy);
    } else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        OnButtonOK(dummy);

    } else if(isOnMarkerLine) {
        m_textCtrlPreview->SetEditable(true);
        event.Skip();
    }
}
//-------------------------------------------------------------------------------
void AddIncludeFileDlg::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    if(m_textCtrlPreview->MarkerNext(wxNOT_FOUND, (1 << 0x7)) != wxNOT_FOUND) {
        // we have a marker
        m_lineToAdd = m_textCtrlPreview->GetLine(m_line);
        m_lineToAdd.Trim();

        if(m_staticTextPreview->GetLabel() != m_lineToAdd) {
            m_staticTextPreview->CallAfter(&wxStaticText::SetLabel, m_lineToAdd);
        }
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// AttachDbgProcDlg
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
/// Ascending sorting function
struct PIDSorter {
    bool operator()(const ProcessEntry& rStart, const ProcessEntry& rEnd) { return rEnd.pid < rStart.pid; }
};
//-------------------------------------------------------------------------------
struct NameSorter {
    bool operator()(const ProcessEntry& rStart, const ProcessEntry& rEnd)
    {
        return rEnd.name.CmpNoCase(rStart.name) < 0;
    }
};
//-------------------------------------------------------------------------------
AttachDbgProcDlg::AttachDbgProcDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer1);
    
    wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
    fgSizer1->SetFlexibleDirection( wxBOTH );
    fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    fgSizer1->AddGrowableCol(1);
    
    bSizer1->Add(fgSizer1, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Debugger to use:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    fgSizer1->Add(m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
    
    wxArrayString m_choiceDebuggerArr;
    m_choiceDebugger = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceDebuggerArr, 0);
    
    fgSizer1->Add(m_choiceDebugger, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText31 = gsgs_Style().createStaticText(this, wxID_ANY, _("Filter:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    fgSizer1->Add(m_staticText31, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
    
    m_textCtrlFilter = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_PROCESS_ENTER);
    fgSizer1->Add(m_textCtrlFilter, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Processes:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer1->Add(m_staticText4, 0, wxALL, WXC_FROM_DIP(5));
    
    m_dvListCtrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,150)), wxDV_VERT_RULES|wxDV_ROW_LINES|wxDV_SINGLE);
    
    bSizer1->Add(m_dvListCtrl, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_dvListCtrl->AppendTextColumn(_("PID"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(100), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Name"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    wxBoxSizer* bSizer2 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer1->Add(bSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));
    
    m_buttonOk = new Button(this, wxID_OK, _("&Attach"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonOk->SetDefault();
    
    bSizer2->Add(m_buttonOk, 0, wxALL, WXC_FROM_DIP(5));
    
    m_button2 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer2->Add(m_button2, 0, wxALL, WXC_FROM_DIP(5));
    
    m_buttonRefresh = new Button(this, wxID_REFRESH, _("&Refresh"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer2->Add(m_buttonRefresh, 0, wxALL, WXC_FROM_DIP(5));
    
    if(GetParent()) {
        CentreOnParent();
    } else {
        CentreOnScreen();
    }

    // Connect events
    m_textCtrlFilter->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AttachDbgProcDlg::OnFilter), NULL, this);
    m_textCtrlFilter->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(AttachDbgProcDlg::OnEnter), NULL, this);
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(AttachDbgProcDlg::OnItemActivated), NULL, this);
    m_buttonOk->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AttachDbgProcDlg::OnBtnAttachUI), NULL, this);
    m_buttonRefresh->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AttachDbgProcDlg::OnRefresh), NULL, this);
    
    
    wxArrayString choices = DebuggerMgr::Get().GetAvailableDebuggers();
    m_choiceDebugger->Append(choices);

    if(choices.IsEmpty() == false) {
        m_choiceDebugger->SetSelection(0);
    }

    wxString filter;
    gsgs_Config().Read("AttachDebuggerDialog/Filter", filter, wxString());
    m_textCtrlFilter->ChangeValue(filter);
    CallAfter(&AttachDbgProcDlg::RefreshProcessesList, filter);
    m_textCtrlFilter->SetFocus();
}
//-------------------------------------------------------------------------------
void AttachDbgProcDlg::RefreshProcessesList(wxString filter)
{
    wxWindowUpdateLocker locker(m_dvListCtrl);
    m_dvListCtrl->DeleteAllItems();

    filter.Trim().Trim(false);

    // Populate the list with list of processes
    std::vector<ProcessEntry> proclist;
    ProcUtils::GetProcessList(proclist);

    filter.MakeLower();
    for(size_t i = 0; i < proclist.size(); ++i) {

        // Use case in-sensitive match for the filter
        wxString entryName(proclist.at(i).name);

        // Append only processes that matches the filter string
        if(filter.IsEmpty() || FileUtil::FuzzyMatch(filter, entryName)) {
            const ProcessEntry& entry = proclist.at(i);
            if(entry.pid == (long)wxGetProcessId()) continue;
            wxVector<wxVariant> cols;
            cols.push_back(wxString() << entry.pid);
            cols.push_back(entry.name);
            m_dvListCtrl->AppendItem(cols);
        }
    }
}
//-------------------------------------------------------------------------------
wxString AttachDbgProcDlg::GetExeName() const
{
    wxVariant v;
    int sel = m_dvListCtrl->GetSelectedRow();
    if(sel != wxNOT_FOUND) {
        m_dvListCtrl->GetValue(v, sel, 1);
        return v.GetString();
    }
    return wxEmptyString;
}
//-------------------------------------------------------------------------------
wxString AttachDbgProcDlg::GetProcessId() const
{
    wxVariant v;
    int sel = m_dvListCtrl->GetSelectedRow();
    if(sel != wxNOT_FOUND) {
        m_dvListCtrl->GetValue(v, sel, 0);
        return v.GetString();
    }
    return wxEmptyString;
}
//-------------------------------------------------------------------------------
void AttachDbgProcDlg::OnBtnAttachUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrl->GetSelectedRow() != wxNOT_FOUND);
}
//-------------------------------------------------------------------------------
AttachDbgProcDlg::~AttachDbgProcDlg()
{
    m_textCtrlFilter->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AttachDbgProcDlg::OnFilter), NULL, this);
    m_textCtrlFilter->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(AttachDbgProcDlg::OnEnter), NULL, this);
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(AttachDbgProcDlg::OnItemActivated), NULL, this);
    m_buttonOk->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AttachDbgProcDlg::OnBtnAttachUI), NULL, this);
    m_buttonRefresh->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AttachDbgProcDlg::OnRefresh), NULL, this);
    gsgs_Config().Write("AttachDebuggerDialog/Filter", m_textCtrlFilter->GetValue());
}
//-------------------------------------------------------------------------------
void AttachDbgProcDlg::OnFilter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    RefreshProcessesList(m_textCtrlFilter->GetValue());
}
//-------------------------------------------------------------------------------
void AttachDbgProcDlg::OnRefresh(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // Clear all filters and refresh the processes list
    m_textCtrlFilter->Clear();
    RefreshProcessesList(wxEmptyString);
    m_textCtrlFilter->SetFocus();
}
//-------------------------------------------------------------------------------
void AttachDbgProcDlg::OnEnter(wxCommandEvent& event)
{
    if(!GetProcessId().IsEmpty()) {
        EndModal(wxID_OK);
    }
}
//-------------------------------------------------------------------------------
void AttachDbgProcDlg::OnItemActivated(wxDataViewEvent& event) { EndModal(wxID_OK); }
//-----------------------------------------------------------------------------
bool WatchDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
    m_parent->AddExpression(text);
    return false; // so the text won't get cut from the editor...
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// BreakpointDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BreakpointDlg::BreakpointDlg(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                     long style)
    : wxPanel(parent, id, pos, size, style)
{
    wxBoxSizer* bSizer5 = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(bSizer5);

    m_dvListCtrlBreakpoints =
        new BreakpointsListctrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                wxDV_ROW_LINES | wxDV_SINGLE | wxBORDER_NONE);

    bSizer5->Add(m_dvListCtrlBreakpoints, 1, wxEXPAND, WXC_FROM_DIP(2));

    wxBoxSizer* bSizer6 = new wxBoxSizer(wxVERTICAL);

    bSizer5->Add(bSizer6, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonAdd = new DefaultButton(this, wxID_ANY, _("&Add"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonAdd->SetToolTip(_("Add a new breakpoint or watchpoint"));

    bSizer6->Add(m_buttonAdd, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonEdit =
        new DefaultButton(this, wxID_ANY, _("&Edit"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonEdit->SetToolTip(_("Edit the selected item"));

    bSizer6->Add(m_buttonEdit, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonDelete =
        new DefaultButton(this, wxID_ANY, _("&Delete"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonDelete->SetToolTip(_("Delete the selected item"));

    bSizer6->Add(m_buttonDelete, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonDeleteAll =
        new DefaultButton(this, wxID_ANY, _("Delete &All"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonDeleteAll->SetToolTip(_("Delete all breakpoints and watchpoints"));

    bSizer6->Add(m_buttonDeleteAll, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonApplyPending =
        new DefaultButton(this, wxID_ANY, _("Set Pending"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonApplyPending->SetToolTip(
        _("If this button is visible, there are breakpoints that you tried to set, but that the debugger refused. This "
          "most often happens when the breakpoint is inside a library that hadn't been loaded when the debugger "
          "started.\n\nClick to offer the breakpoints to the debugger again."));

    bSizer6->Add(m_buttonApplyPending, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    // Connect events
    m_dvListCtrlBreakpoints->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
                                     wxDataViewEventHandler(BreakpointDlg::OnContextMenu), NULL, this);
    m_dvListCtrlBreakpoints->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                                     wxDataViewEventHandler(BreakpointDlg::OnBreakpointActivated), NULL, this);
    m_buttonAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnAdd), NULL, this);
    m_buttonEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnEdit), NULL, this);
    m_buttonDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnDelete), NULL,
                            this);
    m_buttonDeleteAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnDeleteAll),
                               NULL, this);
    m_buttonApplyPending->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                                  wxCommandEventHandler(BreakpointDlg::OnApplyPending), NULL, this);
                                  
    Initialize();
}
//-----------------------------------------------------------------------------
BreakpointDlg::~BreakpointDlg()
{
    m_dvListCtrlBreakpoints->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
                                        wxDataViewEventHandler(BreakpointDlg::OnContextMenu), NULL, this);
    m_dvListCtrlBreakpoints->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                                        wxDataViewEventHandler(BreakpointDlg::OnBreakpointActivated), NULL, this);
    m_buttonAdd->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnAdd), NULL, this);
    m_buttonEdit->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnEdit), NULL,
                             this);
    m_buttonDelete->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnDelete), NULL,
                               this);
    m_buttonDeleteAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakpointDlg::OnDeleteAll),
                                  NULL, this);
    m_buttonApplyPending->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                     wxCommandEventHandler(BreakpointDlg::OnApplyPending), NULL, this);
}
//-----------------------------------------------------------------------------
void BreakpointDlg::Initialize()
{
    std::vector<gsgs::BreakpointInfo> bps;
    gsgs_Manager()->GetBreakpointsMgr()->GetBreakpoints(bps);

    // This does the display stuff
    m_dvListCtrlBreakpoints->Initialise(bps);

    // Store the internal and external ids
    m_ids.clear();
    std::vector<gsgs::BreakpointInfo>::iterator iter = bps.begin();
    for(; iter != bps.end(); ++iter) {
        struct bpd_IDs IDs(*iter);
        m_ids.push_back(IDs);
    }

    bool hasItems = !m_dvListCtrlBreakpoints->IsEmpty();
    if(hasItems) { m_dvListCtrlBreakpoints->Select(m_dvListCtrlBreakpoints->RowToItem(0)); }

    // Since any change results in Initialize() being rerun, we can do updateUI here
    m_buttonEdit->Enable(hasItems);
    m_buttonDelete->Enable(hasItems);
    // The 'Apply Pending' button is more complicated: it should be hidden,
    // unless there are pending bps to apply,and the debugger is running
    bool pending = gsgs_Manager()->GetBreakpointsMgr()->PendingBreakpointsExist();
    gsgs::IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    m_buttonApplyPending->Show(pending && dbgr && dbgr->IsRunning());
    Layout();
    // Enable DeleteAll if there are either bps or pending bps
    m_buttonDeleteAll->Enable(hasItems || pending);
}
//-----------------------------------------------------------------------------
void BreakpointDlg::OnDelete(wxCommandEvent& e)
{
    wxDataViewItem item = m_dvListCtrlBreakpoints->GetSelection();
    if(!item.IsOk()) { return; }

    // Delete by this item's id, which was carefully stored in Initialize()
    int id = m_ids[m_dvListCtrlBreakpoints->ItemToRow(item)].GetBestId();
    gsgs_Manager()->GetBreakpointsMgr()->DelBreakpoint(id);

    gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Breakpoint successfully deleted"));
    Initialize(); // ReInitialise, as either a bp was deleted, or the data was corrupt
}
//-----------------------------------------------------------------------------
void BreakpointDlg::OnDeleteAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gsgs_Manager()->GetBreakpointsMgr()->DelAllBreakpoints();
    Initialize();

    gsgs_Frame()->GetMainStatusBar()->SetMessage(_("All Breakpoints deleted"));

    wxCommandEvent evtDelAll(wxEVT_GSGS_ALL_BREAKPOINTS_DELETED);
    gsgs_Event().AddPendingEvent(evtDelAll);
}
//-----------------------------------------------------------------------------
void BreakpointDlg::OnApplyPending(wxCommandEvent& e)
{
    wxUnusedVar(e);
    gsgs_Manager()->GetBreakpointsMgr()->ApplyPendingBreakpoints();
    Initialize();

    gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Pending Breakpoints reapplied"));
}
//-----------------------------------------------------------------------------
void BreakpointDlg::OnBreakpointActivated(wxDataViewEvent& event)
{
    wxString file = m_dvListCtrlBreakpoints->GetItemText(event.GetItem(), m_dvListCtrlBreakpoints->GetFileColumn());
    wxString line = m_dvListCtrlBreakpoints->GetItemText(event.GetItem(), m_dvListCtrlBreakpoints->GetLinenoColumn());
    long line_number;
    line.ToLong(&line_number);
    gsgs_Frame()->GetMainBook()->OpenFile(file, wxEmptyString, line_number - 1, wxNOT_FOUND, OF_AddJump, false);
}
//-----------------------------------------------------------------------------
void BreakpointDlg::OnEdit(wxCommandEvent& e)
{
    wxDataViewItem item = m_dvListCtrlBreakpoints->GetSelection();
    if(!item.IsOk()) { return; }
    
    int row = m_dvListCtrlBreakpoints->ItemToRow(item);
    bool bpExist;
    gsgs_Manager()->GetBreakpointsMgr()->EditBreakpoint(row, bpExist);

    if(!bpExist) {
        // the breakpoint does not exist! remove it from the UI as well
        m_dvListCtrlBreakpoints->DeleteItem(row);
    }

    Initialize(); // Make any changes visible
}
//-----------------------------------------------------------------------------
void BreakpointDlg::OnAdd(wxCommandEvent& e)
{
    wxUnusedVar(e);

    gsgs_Manager()->GetBreakpointsMgr()->AddBreakpoint();
    Initialize(); // Make any changes visible
}
//-----------------------------------------------------------------------------
void BreakpointDlg::OnContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("edit_breakpoint"), _("Edit Breakpoint..."));
    menu.Append(XRCID("delete_breakpoint"), _("Delete Breakpoint"));

    int where = GetPopupMenuSelectionFromUser(menu);
    if(where == wxID_NONE) return;
    if(where == XRCID("edit_breakpoint")) {
        wxCommandEvent dummy;
        OnEdit(event);
    } else if(where == XRCID("delete_breakpoint")) {
        wxCommandEvent dummy;
        OnDelete(event);
    }
}
//-----------------------------------------------------------------------------
void BreakpointsListctrl::Initialise(std::vector<gsgs::BreakpointInfo>& bps)
{
    DeleteAllItems();
    std::vector<BreakpointInfo>::iterator iter = bps.begin();
    for(; iter != bps.end(); ++iter) {

        // Store the internal and external ids
        bpd_IDs IDs(*iter);
        wxVector<wxVariant> cols;
        cols.push_back(IDs.GetIdAsString());

        wxString type;
        if(iter->is_temp) { type = _("Temp. "); }
        type += ((iter->bp_type == BP_type_watchpt) ? _("Watchpoint") : _("Breakpoint"));
        cols.push_back(type);

        wxString disabled;
        if(!iter->is_enabled) { disabled = _("disabled"); }
        cols.push_back(disabled);
        cols.push_back(iter->file);
        cols.push_back((wxString() << iter->lineno));
        cols.push_back(iter->function_name);
        cols.push_back(iter->at);
        cols.push_back(iter->memory_address);
        cols.push_back(iter->what);
        cols.push_back((wxString() << iter->ignore_number));

        wxString extras; // Extras are conditions, or a commandlist. If both (unlikely!) just show the condition
        if(!iter->conditions.IsEmpty()) {
            extras = iter->conditions;
        } else if(!iter->commandlist.IsEmpty()) {
            extras = iter->commandlist;
        }
        if(!extras.IsEmpty()) {
            // We don't want to try to display massive commandlist spread over several lines...
            int index = extras.Find(wxT("\\n"));
            if(index != wxNOT_FOUND) { extras = extras.Left(index) + wxT("..."); }
        }
        cols.push_back(extras);
        AppendItem(cols);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// BreakptPropertiesDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BreakptPropertiesDlg::BreakptPropertiesDlg(wxWindow* parent, wxWindowID id, const wxString& title,
                                                         const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer3);

    m_staticText9 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select the type of the breakpoint:"), wxDefaultPosition,
                                     wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    bSizer3->Add(m_staticText9, 0, wxALL, WXC_FROM_DIP(5));

    m_choicebook = new wxChoicebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_choicebook->SetName(wxT("m_choicebook"));

    bSizer3->Add(m_choicebook, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_panel1 = new wxPanel(m_choicebook, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_choicebook, wxSize(-1, -1)),
                           wxTAB_TRAVERSAL);
    m_choicebook->AddPage(m_panel1, _("Breakpoint"), false);

    wxBoxSizer* bSizer14 = new wxBoxSizer(wxVERTICAL);
    m_panel1->SetSizer(bSizer14);

    m_staticText51 = gsgs_Style().createStaticText(m_panel1, wxID_ANY, _("Break by line, function or memory address:"),
                                      wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);

    bSizer14->Add(m_staticText51, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer111 = new wxBoxSizer(wxVERTICAL);

    bSizer14->Add(bSizer111, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    fgSizer2->SetFlexibleDirection(wxBOTH);
    fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    fgSizer2->AddGrowableCol(1);

    bSizer111->Add(fgSizer2, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_checkLineno = new wxCheckBox(m_panel1, wxID_ANY, _("Line number"), wxDefaultPosition,
                                   wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_checkLineno->SetValue(false);

    fgSizer2->Add(m_checkLineno, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textLineno =
        gsgs_Style().createTextCtrl(m_panel1, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_textLineno->SetToolTip(_("Enter the line-number on which you wish to break. It's assumed to refer to the current "
                               "file: if it doesn't, please enter the correct filepath below."));

    fgSizer2->Add(m_textLineno, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkBreakFunction = new wxCheckBox(m_panel1, wxID_ANY, _("Function name"), wxDefaultPosition,
                                          wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_checkBreakFunction->SetValue(false);

    fgSizer2->Add(m_checkBreakFunction, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textFunctionname =
        gsgs_Style().createTextCtrl(m_panel1, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_textFunctionname->SetToolTip(_(
        "If you wish to break when a particular function is entered, insert its name here. In C just the name will do "
        "e.g. 'main' or 'myFoo'. For C++ class methods, you need to do 'MyClass::myFoo'\n\nAlternatively you can enter "
        "a regular expression, and tick the checkbox below. A breakpoint will then be set on all matching functions."));

    fgSizer2->Add(m_textFunctionname, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    fgSizer2->Add(0, 0, 1, 0, WXC_FROM_DIP(5));

    m_checkRegex = new wxCheckBox(m_panel1, wxID_ANY, _("This is a regex"), wxDefaultPosition,
                                  wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_checkRegex->SetValue(false);
    m_checkRegex->SetToolTip(_("If you wish to insert a breakpoint on several functions, you can tick this box, then "
                               "enter a suitable regular expression in the textctrl above."));

    fgSizer2->Add(m_checkRegex, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkBreakMemory = new wxCheckBox(m_panel1, wxID_ANY, _("Memory address:"), wxDefaultPosition,
                                        wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_checkBreakMemory->SetValue(false);

    fgSizer2->Add(m_checkBreakMemory, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textBreakMemory =
        gsgs_Style().createTextCtrl(m_panel1, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_textBreakMemory->SetToolTip(
        _("To break on a memory address, enter the address here.\ne.g. 0x0a1b2c3d or 12345678"));

    fgSizer2->Add(m_textBreakMemory, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText6 =
        gsgs_Style().createStaticText(m_panel1, wxID_ANY, _("In file:"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);

    fgSizer2->Add(m_staticText6, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer18 = new wxBoxSizer(wxHORIZONTAL);

    fgSizer2->Add(bSizer18, 0, wxTOP | wxBOTTOM | wxEXPAND, WXC_FROM_DIP(5));

    m_textFilename =
        gsgs_Style().createTextCtrl(m_panel1, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
    m_textFilename->SetToolTip(
        _("If you've entered a line-number, its assumed to refer to the current file. If it isn't, enter the correct "
          "filename here.\n\nFor a function, a file is usually not required. However, if you have several functions "
          "with the same name, in several different files (do people _really_ do that?) and you want to break on only "
          "one of those, enter the correct filename here."));

    bSizer18->Add(m_textFilename, 1, wxALL, WXC_FROM_DIP(5));

    m_buttonBrowse =
        new Button(m_panel1, wxID_ANY, _("Browse"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);

    bSizer18->Add(m_buttonBrowse, 0, wxALL, WXC_FROM_DIP(5));

    m_staticline5 =
        new wxStaticLine(m_panel1, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), wxLI_HORIZONTAL);

    bSizer14->Add(m_staticline5, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_panel2 = new wxPanel(m_choicebook, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_choicebook, wxSize(-1, -1)),
                           wxTAB_TRAVERSAL);
    m_choicebook->AddPage(m_panel2, _("Watchpoint"), false);

    wxBoxSizer* bSizer13 = new wxBoxSizer(wxVERTICAL);
    m_panel2->SetSizer(bSizer13);

    wxArrayString m_radioWatchtypeArr;
    m_radioWatchtypeArr.Add(_("write only"));
    m_radioWatchtypeArr.Add(_("read only"));
    m_radioWatchtypeArr.Add(_("read-write"));
    m_radioWatchtype = new wxRadioBox(m_panel2, wxID_ANY, _("Type of watchpoint:"), wxDefaultPosition,
                                      wxDLG_UNIT(m_panel2, wxSize(-1, -1)), m_radioWatchtypeArr, 3, wxRA_SPECIFY_ROWS);
    m_radioWatchtype->SetToolTip(_("The usual type of watchpoint is 'write-only': that is, it's triggered whenever the "
                                   "target is changed.\n\nAlternatively you can choose for it to trigger when the "
                                   "target is read from, or either written to or read from."));
    m_radioWatchtype->SetSelection(0);

    bSizer13->Add(m_radioWatchtype, 0, wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_staticText5 = gsgs_Style().createStaticText(m_panel2, wxID_ANY, _("Data to watch:"), wxDefaultPosition,
                                     wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);

    bSizer13->Add(m_staticText5, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_textWatchExpression = gsgs_Style().createTextCtrl(m_panel2, wxID_ANY, wxT(""), wxDefaultPosition,
                                           wxDLG_UNIT(m_panel2, wxSize(-1, -1)), wxTE_MULTILINE);
    m_textWatchExpression->SetToolTip(
        _("Identify the data to be watched. It can be one of:\n1) Any variable name e.g. 'foo'\n2) A memory address, "
          "suitably cast e.g.*(int*)0x12345678 will watch an int-sized block starting at this address.\nDon't include "
          "spaces in the expression: gdb can't understand them.\n3) A complex expression e.g. a*b + c/d'. The "
          "expression can use any operators valid in the program's native language.\n\nNB. A watchpoint set on a local "
          "variable will automatically be removed when the variable loses scope."));

    bSizer13->Add(m_textWatchExpression, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer5 = new wxBoxSizer(wxVERTICAL);

    bSizer3->Add(bSizer5, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Conditional Breaks: Add any condition here"), wxDefaultPosition,
                                     wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    bSizer5->Add(m_staticText1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_textCond =
        gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_MULTILINE);
    m_textCond->SetToolTip(
        _("You can add a condition to any breakpoint or watchpoint. The debugger will then stop only if the condition "
          "is met.\nThe condition can be any simple or complex expression in your programming language,providing it "
          "returns a bool. However any variables that you use must be in scope.\nIf you've previously set a condition "
          "and no longer want it, just clear this textctrl."));

    bSizer5->Add(m_textCond, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText11 = gsgs_Style().createStaticText(this, wxID_ANY, _("Command List: Add any command(s) here"), wxDefaultPosition,
                                      wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    bSizer5->Add(m_staticText11, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_textCommands =
        gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_MULTILINE);
    m_textCommands->SetToolTip(
        _("You can add a list of commands to any breakpoint or watchpoint. When the breakpoint is hit and the program "
          "interrupted, those commands will be executed.\nFor example, to print the value of the variable foo and then "
          "continue running the program, enter:\n\tprint foo\n\tcont\nIf you've previously entered commands, and no "
          "longer want them, just clear this textctrl."));

    bSizer5->Add(m_textCommands, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(2, 4, 0, 0);
    fgSizer1->SetFlexibleDirection(wxBOTH);
    fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    fgSizer1->AddGrowableCol(0);
    fgSizer1->AddGrowableCol(1);
    fgSizer1->AddGrowableCol(2);
    fgSizer1->AddGrowableCol(3);

    bSizer5->Add(fgSizer1, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_checkDisable =
        new wxCheckBox(this, wxID_ANY, _("Disable"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkDisable->SetValue(false);
    m_checkDisable->SetToolTip(_("If you check this box, the breakpoint (or watchpoint) will still exist, but it won't "
                                 "trigger. If you uncheck it in the future, the breakpoint will work again."));

    fgSizer1->Add(m_checkDisable, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_checkTemp =
        new wxCheckBox(this, wxID_ANY, _("Make temporary"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkTemp->SetValue(false);
    m_checkTemp->SetToolTip(_("A temporary breakpoint (or watchpoint) is one that works only once. When it's hit it "
                              "behaves like any other, except that it's then deleted."));

    fgSizer1->Add(m_checkTemp, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticText7 =
        gsgs_Style().createStaticText(this, wxID_ANY, _("Ignore count:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_staticText7->SetToolTip(
        _("Enter a count >0 to ignore this breakpoint (or watchpoint) for that number of times. It then behaves as "
          "though it is disabled, except that every time it would have triggered, the ignore count decrements.\nWhen "
          "the count reaches zero, the breakpoint becomes active again."));

    fgSizer1->Add(m_staticText7, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_spinIgnore =
        new wxSpinCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxSP_ARROW_KEYS);
    m_spinIgnore->SetRange(0, 1000000);
    m_spinIgnore->SetValue(0);

    fgSizer1->Add(m_spinIgnore, 1, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer9 = new wxBoxSizer(wxHORIZONTAL);

    bSizer3->Add(bSizer9, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button6 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button6->SetDefault();

    bSizer9->Add(m_button6, 0, wxALL, WXC_FROM_DIP(5));

    m_button7 = new Button(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    bSizer9->Add(m_button7, 0, wxALL, WXC_FROM_DIP(5));


    if(GetParent()) {
        CentreOnParent();
    } else {
        CentreOnScreen();
    }

    // Connect events
    m_choicebook->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED,
                          wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanged), NULL, this);
    m_choicebook->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
                          wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanging), NULL, this);
    m_checkLineno->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                           wxCommandEventHandler(BreakptPropertiesDlg::OnCheckBreakLineno), NULL, this);
    m_checkLineno->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BreakptPropertiesDlg::OnCheckBreakLinenoUI),
                           NULL, this);
    m_checkBreakFunction->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                  wxCommandEventHandler(BreakptPropertiesDlg::OnCheckBreakFunction), NULL, this);
    m_checkBreakFunction->Connect(
        wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BreakptPropertiesDlg::OnCheckBreakFunctionUI), NULL, this);
    m_checkBreakMemory->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                wxCommandEventHandler(BreakptPropertiesDlg::OnCheckBreakMemory), NULL, this);
    m_checkBreakMemory->Connect(wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(BreakptPropertiesDlg::OnCheckBreakMemoryUI), NULL, this);
    m_buttonBrowse->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BreakptPropertiesDlg::OnBrowse),
                            NULL, this);
    its_a_breakpt = true;       // Default to breakpoint for new items
    whichBreakcheck = wbc_line; // and assume it'll be a line-number type
}
//-----------------------------------------------------------------------------
BreakptPropertiesDlg::~BreakptPropertiesDlg() 
{
    m_choicebook->Disconnect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED,
                             wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanged), NULL, this);
    m_choicebook->Disconnect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
                             wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanging), NULL, this);
    m_checkLineno->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                              wxCommandEventHandler(BreakptPropertiesDlg::OnCheckBreakLineno), NULL, this);
    m_checkLineno->Disconnect(wxEVT_UPDATE_UI,
                              wxUpdateUIEventHandler(BreakptPropertiesDlg::OnCheckBreakLinenoUI), NULL, this);
    m_checkBreakFunction->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                     wxCommandEventHandler(BreakptPropertiesDlg::OnCheckBreakFunction), NULL,
                                     this);
    m_checkBreakFunction->Disconnect(
        wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BreakptPropertiesDlg::OnCheckBreakFunctionUI), NULL, this);
    m_checkBreakMemory->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                   wxCommandEventHandler(BreakptPropertiesDlg::OnCheckBreakMemory), NULL, this);
    m_checkBreakMemory->Disconnect(
        wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BreakptPropertiesDlg::OnCheckBreakMemoryUI), NULL, this);
    m_buttonBrowse->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                               wxCommandEventHandler(BreakptPropertiesDlg::OnBrowse), NULL, this);
}
//-----------------------------------------------------------------------------
// Insert the data from an existing breakpoint into the dialog fields
void BreakptPropertiesDlg::EnterBPData(const gsgs::BreakpointInfo& bp)
{
    // The class BreakpointInfo& b will become the new bp, so copy the old ids
    b.debugger_id = bp.debugger_id;
    b.internal_id = bp.internal_id;

    if(bp.bp_type == BP_type_watchpt) {
        its_a_breakpt = false; // UpdateUI will then tick the checkbox
        m_radioWatchtype->SetSelection(bp.watchpoint_type);
        m_textWatchExpression->SetValue(bp.watchpt_data);
    } else {
        its_a_breakpt = true;
        if(bp.memory_address.IsEmpty() == false) {
            m_checkBreakMemory->SetValue(true);
            m_textBreakMemory->Clear();
            *m_textBreakMemory << bp.memory_address;
            whichBreakcheck = wbc_memory;
        } else {
            m_textFilename->SetValue(bp.file);
            if(bp.lineno != -1) {
                m_checkLineno->SetValue(true);
                m_textLineno->Clear();
                *m_textLineno << bp.lineno;
                whichBreakcheck = wbc_line;
            } else if(!bp.function_name.IsEmpty()) {
                m_checkBreakFunction->SetValue(true);
                m_textFunctionname->SetValue(bp.function_name);
                m_checkRegex->SetValue(bp.regex == true);
                whichBreakcheck = wbc_function;
            }
        }
    }

    // We need temporarily to Disconnect this event, otherwise it's impossible to edit a watchpoint while the debugger's
    // running
    // as SetSelection(1) causes a wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING event, which is vetoed
    m_choicebook->Disconnect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
                             wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanging),
                             NULL,
                             this);
    if(its_a_breakpt) {
        m_choicebook->SetSelection(0);
    } else {
        m_choicebook->SetSelection(1);
    }
    m_choicebook->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
                          wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanging),
                          NULL,
                          this);

    m_checkDisable->SetValue(!bp.is_enabled);
    m_checkTemp->SetValue(bp.is_temp);
    m_checkTemp->Show(its_a_breakpt); // Watchpoints can't be temporary
    m_spinIgnore->SetValue(bp.ignore_number);
    m_textCond->SetValue(bp.conditions);
    m_textCommands->SetValue(bp.commandlist);
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::EndModal(int retCode)
{
    if(retCode != wxID_OK) {
        return wxDialog::EndModal(retCode);
    }

    long l;
    wxString contents;
    int selectedPage = m_choicebook->GetSelection();
    if(m_choicebook->GetPageText((size_t)selectedPage) == _("Watchpoint")) {
        b.bp_type = BP_type_watchpt;
        b.watchpoint_type = (WatchpointType)m_radioWatchtype->GetSelection();
        b.watchpt_data = m_textWatchExpression->GetValue();
        if(b.watchpt_data.IsEmpty()) {
            wxMessageBox(_("You don't seem to have entered a variable for the watchpoint to watch. Please try again."),
                         wxT(":/"),
                         wxICON_ERROR);
            return;
        }
    } else {

        // reset the fields to only the proper ones are copied
        b.lineno = wxNOT_FOUND;
        b.memory_address.Clear();
        b.file.Clear();
        b.function_name.Clear();

        // It's some flavour of breakpoint (assume standard for now). Only insert enabled data, in case a lineno sort is
        // now a function bp
        b.bp_type = BP_type_break;
        switch(whichBreakcheck) {
        case wbc_line:
            contents = m_textLineno->GetValue();
            if(!contents.ToLong(&l, 0)) {
                wxMessageBox(_("The breakpoint's line-number is invalid. Please try again."), _(":/"), wxICON_ERROR);
                return;
            }
            b.lineno = (int)l;
            b.file = m_textFilename->GetValue();
            break;

        case wbc_function:
            b.function_name = m_textFunctionname->GetValue();
            if(b.function_name.IsEmpty()) {
                wxMessageBox(_("You don't seem to have entered a name for the function. Please try again."),
                             _(":/"),
                             wxICON_ERROR);
                return;
            }
            b.regex = m_checkRegex->IsChecked();
            b.file = m_textFilename->GetValue();
            break;

        case wbc_memory:
            contents = m_textBreakMemory->GetValue();
            b.memory_address = contents;
            break;
        }
    }

    b.is_enabled = !m_checkDisable->GetValue();
    b.is_temp = m_checkTemp->GetValue();
    b.ignore_number = m_spinIgnore->GetValue();
    b.conditions = m_textCond->GetValue();
    b.commandlist = m_textCommands->GetValue();

    wxDialog::EndModal(retCode);
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnCheckBreakLineno(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        whichBreakcheck = wbc_line;
    }
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnCheckBreakFunction(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        whichBreakcheck = wbc_function;
    }
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnCheckBreakMemory(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        whichBreakcheck = wbc_memory;
    }
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnBrowse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString newfilepath, filepath(m_textFilename->GetValue());
    if((!filepath.IsEmpty()) && wxFileName::FileExists(filepath)) {
        newfilepath = wxFileSelector(_("Select file:"), filepath.c_str());
    } else {
        newfilepath = wxFileSelector(_("Select file:"));
    }

    if(!newfilepath.IsEmpty()) {
        m_textFilename->SetValue(newfilepath);
    }
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnCheckBreakLinenoUI(wxUpdateUIEvent& event)
{
    m_checkLineno->SetValue(whichBreakcheck == wbc_line);
    m_textLineno->Enable(m_checkLineno->IsChecked() && m_checkLineno->IsEnabled());
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnCheckBreakFunctionUI(wxUpdateUIEvent& event)
{
    m_checkBreakFunction->SetValue(whichBreakcheck == wbc_function);
    m_textFunctionname->Enable(m_checkBreakFunction->IsChecked() && m_checkBreakFunction->IsEnabled());
    m_checkRegex->Enable(m_textFunctionname->IsEnabled());
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnCheckBreakMemoryUI(wxUpdateUIEvent& event)
{
    m_checkBreakMemory->SetValue(whichBreakcheck == wbc_memory);
    m_textBreakMemory->Enable(m_checkBreakMemory->IsChecked() && m_checkBreakMemory->IsEnabled());
    // Disable the filename textctrl if the memory check is ticked
    m_textFilename->Enable(!m_textBreakMemory->IsEnabled());
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnPageChanging(wxChoicebookEvent& event)
{
    if(b.debugger_id != -1) {
        wxMessageBox(
            _("Sorry, you can't change a breakpoint to a watchpoint, or vice versa, while the debugger is running"),
            _("Not possible"),
            wxICON_ERROR | wxOK);
        event.Veto();
    }
}
//-----------------------------------------------------------------------------
void BreakptPropertiesDlg::OnPageChanged(wxChoicebookEvent& event)
{
    its_a_breakpt = m_choicebook->GetPageText(m_choicebook->GetSelection()) == _("Breakpoint");
    // Watchpoints can't have conditions set direct in MI; they need to be created without, then edited to add
    m_textCond->Enable(its_a_breakpt || GetTitle().StartsWith("Properties")); // The dlg title starts with Properties when editing
    if (!GetTitle().StartsWith("Properties")) {
        wxString tip =  (its_a_breakpt ?
                            "You can add a condition to any breakpoint. The debugger will then stop only if the condition is met.\n \
The condition can be any simple or complex expression in your programming language,providing it returns a bool. However any variables that you use must be in scope.\n \
If you've previously set a condition and no longer want it, just clear this textctrl." :
                        "It is not possible to Add a conditional watchpoint. Create a normal watchpoint first, then Edit it to add the conditon"
                        );
        m_textCond->SetToolTip(tip);
    }
    m_checkTemp->Show(its_a_breakpt); // Watchpoints can't be temporary
    m_checkTemp->GetContainingSizer()->Layout();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// ThreadListPanel
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ThreadListClientData : public wxClientData
{
    ThreadEntry m_threadEntry;

public:
    ThreadListClientData(const ThreadEntry& te) { m_threadEntry = te; }

    virtual ~ThreadListClientData() {}
    const ThreadEntry& GetThreadEntry() const { return m_threadEntry; }
    ThreadEntry& GetThreadEntry() { return m_threadEntry; }
};
//-----------------------------------------------------------------------------
ThreadListPanel::ThreadListPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                         long style)
    : wxPanel(parent, id, pos, size, style)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);

    m_dvListCtrl = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                        wxDV_ROW_LINES | wxDV_SINGLE);

    mainSizer->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(2));

    m_dvListCtrl->AppendTextColumn(_("#"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Active?"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Function"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("File"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    m_dvListCtrl->AppendTextColumn(_("Line"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);

    // Connect events
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                          wxDataViewEventHandler(ThreadListPanel::OnItemActivated), NULL, this);
}
//-----------------------------------------------------------------------------
ThreadListPanel::~ThreadListPanel() 
{
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
                             wxDataViewEventHandler(ThreadListPanel::OnItemActivated), NULL, this);
}
//-----------------------------------------------------------------------------
void ThreadListPanel::OnItemActivated(wxDataViewEvent& event)
{
    if(!event.GetItem().IsOk()) return;

    long threadId;
    wxString str_id = m_dvListCtrl->GetItemText(event.GetItem(), 0);    
    if(str_id.ToCLong(&threadId)) {
        Manager* mgr = gsgs_Manager();
        mgr->DbgSetThread(threadId);
    }
}
//-----------------------------------------------------------------------------
void ThreadListPanel::PopulateList(const ThreadEntryArray& threads)
{
    // Check if the new thread list is the same as the current one
    if(IsTheSame(m_threads, threads)) {
        // No need to repopulate the list, just set the active thread indicator

        // Loop over the table and set all threads to "NO"
        for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            ThreadListClientData* d = (ThreadListClientData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i));
            d->GetThreadEntry().active = false;
            wxVariant v = "NO";
            m_dvListCtrl->SetValue(v, i, 1);
        }

        // Replace the current thread stack with the new one
        m_threads.clear();
        m_threads.insert(m_threads.end(), threads.begin(), threads.end());

        long threadID = wxNOT_FOUND;
        for(size_t i = 0; i < m_threads.size(); ++i) {
            if(m_threads.at(i).active) {
                threadID = m_threads.at(i).dbgid;
                break;
            }
        }

        if(threadID != wxNOT_FOUND) {
            // Update the new active thread
            for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
                ThreadListClientData* d = (ThreadListClientData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i));
                if(d->GetThreadEntry().dbgid == threadID) {
                    d->GetThreadEntry().active = true;
                    wxVariant v = "YES";
                    m_dvListCtrl->SetValue(v, i, 1);
                }
            }
        }

    } else {
        Clear();
        // Replace the thread list
        m_threads.clear();
        m_threads.insert(m_threads.end(), threads.begin(), threads.end());

        int sel = wxNOT_FOUND;
        if(m_threads.empty()) return;

        for(int i = (int)(m_threads.size() - 1); i >= 0; --i) {
            const ThreadEntry& entry = m_threads.at(i);

            wxString str_id;
            wxString str_active;

            str_id << entry.dbgid;
            str_active = entry.active ? "YES" : "NO";
            if(entry.active) {
                sel = i;
            }

            wxVector<wxVariant> cols;
            cols.push_back(str_id);
            cols.push_back(str_active);
            cols.push_back(entry.function);
            cols.push_back(entry.file);
            cols.push_back(entry.line);
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new ThreadListClientData(entry));
        }
        m_dvListCtrl->Update();
        // Ensure that the active thread is visible
        if(sel != wxNOT_FOUND) {
            wxDataViewItem item = m_dvListCtrl->RowToItem(sel);
            m_dvListCtrl->EnsureVisible(item);
        }
    }
}
//-----------------------------------------------------------------------------
void ThreadListPanel::Clear()
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        ThreadListClientData* d = (ThreadListClientData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i));
        delete d;
    }
    m_dvListCtrl->DeleteAllItems();
}
//-----------------------------------------------------------------------------
bool ThreadListPanel::IsTheSame(const ThreadEntryArray& threads1, const ThreadEntryArray& threads2)
{
    if(threads1.size() != threads2.size()) {
        return false;
    }

    for(size_t i = 0; i < threads1.size(); ++i) {
        const ThreadEntry& entry1 = threads1.at(i);
        const ThreadEntry& entry2 = threads2.at(i);
        if((entry1.file != entry2.file) || (entry1.function != entry2.function) || (entry1.line != entry2.line)) {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MyCompilersFoundModel
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class MyCompilersFoundModel : public CompilersFoundModel
{
    CompilersFoundDlg* m_dlg;

public:
    MyCompilersFoundModel(CompilersFoundDlg* dlg)
        : m_dlg(dlg)
    {
    }
    virtual ~MyCompilersFoundModel() {}

    virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
    {
        CompilerPtr compiler = m_dlg->GetCompiler(item);
        if(compiler && m_dlg->IsDefaultCompiler(compiler)) {
            attr.SetBold(true);
            return true;
        }
        return false;
    }
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CompilersFoundDlgItemData
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CompilersFoundDlgItemData : public wxClientData
{
    CompilerPtr m_compiler;

public:
    CompilersFoundDlgItemData(CompilerPtr compiler)
        : m_compiler(compiler)
    {
    }
    virtual ~CompilersFoundDlgItemData() {}

    CompilerPtr GetCompiler() const { return m_compiler; }
};

extern void wxCFE1CInitBitmapResources();

static bool bBitmapLoaded = false;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CompilersFoundDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CompilersFoundDlg::CompilersFoundDlg(wxWindow* parent, const ToolsetLocator::CompilerVec_t& compilers, wxWindowID id,
    const wxString& title, const wxPoint& pos, const wxSize& size, long style) : 
        wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCFE1CInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer2);
    
    m_banner10 = new wxBannerWindow(this, wxID_ANY, wxTOP, wxDefaultPosition, wxSize(-1,-1), wxBORDER_SIMPLE);
    m_banner10->SetBitmap(wxNullBitmap);
    m_banner10->SetText(_("Found Compilers"), _("Below is a list of compilers found on your computer.\nClick 'OK' to replace the current list of compilers with this list. 'Cancel' to abort."));
    m_banner10->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK), wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    m_banner10->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    
    boxSizer2->Add(m_banner10, 0, wxALL|wxEXPAND, 5);
    
    m_dataview = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(300,300), wxDV_ROW_LINES|wxDV_SINGLE);
    
    m_dataviewModel = new CompilersFoundModel;
    m_dataviewModel->SetColCount( 2 );
    m_dataview->AssociateModel(m_dataviewModel.get() );
    
    boxSizer2->Add(m_dataview, 1, wxALL|wxEXPAND, 5);
    
    m_dataview->AppendTextColumn(_("Compiler Name / Family"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT);
    m_dataview->AppendTextColumn(_("Installation Path"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 400, wxALIGN_LEFT);
    wxBoxSizer* boxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer2->Add(boxSizer31, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_staticBitmap35 = new wxStaticBitmap(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("bulb")), wxDefaultPosition, wxSize(-1,-1), 0 );
    
    boxSizer31->Add(m_staticBitmap35, 0, wxALL, 5);
    
    m_staticText29 = gsgs_Style().createStaticText(this, wxID_ANY, _("Double click a compiler to make it the default for its compiler family"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer31->Add(m_staticText29, 0, wxALIGN_CENTER_VERTICAL, 5);
    
    m_stdBtnSizer4 = new DialogButtonSizer();
    
    boxSizer2->Add(m_stdBtnSizer4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer4->AddButton(m_buttonCancel);
    
    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_buttonOK->SetDefault();
    m_stdBtnSizer4->AddButton(m_buttonOK);
    m_stdBtnSizer4->Realize();

    CentreOnParent(wxBOTH);

    // Connect events
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(CompilersFoundDlg::OnItemActivated), NULL, this);
    
    m_allCompilers = compilers;

    // Replace the model with a custom one
    unsigned int colCount = m_dataviewModel->GetColCount();
    m_dataviewModel = new MyCompilersFoundModel(this);
    m_dataviewModel->SetColCount(colCount);
    m_dataview->AssociateModel(m_dataviewModel.get());

    // Add the categories
    std::map<wxString, wxDataViewItem> categories;
    for(size_t i = 0; i < compilers.size(); ++i) {
        if(categories.count(compilers.at(i)->GetCompilerFamily()) == 0) {
            wxVector<wxVariant> cols;
            cols.push_back(compilers.at(i)->GetCompilerFamily());
            wxDataViewItem item = m_dataviewModel->AppendItem(wxDataViewItem(0), cols);
            categories.insert(std::make_pair(compilers.at(i)->GetCompilerFamily(), item));
        }
    }

    for(size_t i = 0; i < compilers.size(); ++i) {
        CompilerPtr compiler = compilers.at(i);
        wxDataViewItem parent = categories.find(compiler->GetCompilerFamily())->second;
        wxVector<wxVariant> cols;
        cols.push_back(compiler->GetName());
        cols.push_back(compiler->GetInstallationPath());
        m_dataviewModel->AppendItem(parent, cols, new CompilersFoundDlgItemData(compiler));
        if(m_defaultCompilers.count(compiler->GetCompilerFamily()) == 0) {
            compiler->SetIsDefault(true); // Per family
            m_defaultCompilers.insert(std::make_pair(compiler->GetCompilerFamily(), compiler));
            MSWUpdateToolchain(compiler);
        }
    }

    std::map<wxString, wxDataViewItem>::iterator iter = categories.begin();
    for(; iter != categories.end(); ++iter) {
        m_dataview->Expand(iter->second);
    }
    SetName("CompilersFoundDlg");
    gsgs::WindowManager::Load(this);
}
//-----------------------------------------------------------------------------
CompilersFoundDlg::~CompilersFoundDlg() 
{
        m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(CompilersFoundDlg::OnItemActivated), NULL, this);
}
//-----------------------------------------------------------------------------
void CompilersFoundDlg::OnItemActivated(wxDataViewEvent& event)
{
    CompilerPtr compiler = GetCompiler(event.GetItem());
    if(compiler) {
        m_defaultCompilers.erase(compiler->GetCompilerFamily());
        compiler->SetIsDefault(true);
        m_defaultCompilers.insert(std::make_pair(compiler->GetCompilerFamily(), compiler));
        m_dataview->UnselectAll();
        m_dataview->CallAfter(&wxDataViewCtrl::Refresh, true, (const wxRect*)NULL);
        MSWUpdateToolchain(compiler);
    }
}
//-----------------------------------------------------------------------------
CompilerPtr CompilersFoundDlg::GetCompiler(const wxDataViewItem& item) const
{
    CompilersFoundDlgItemData* itemData =
        dynamic_cast<CompilersFoundDlgItemData*>(m_dataviewModel->GetClientObject(item));
    if(itemData) {
        return itemData->GetCompiler();
    }
    return CompilerPtr(NULL);
}
//-----------------------------------------------------------------------------
bool CompilersFoundDlg::IsDefaultCompiler(CompilerPtr compiler) const
{
    if(m_defaultCompilers.count(compiler->GetCompilerFamily()) == 0) return false;
    CompilerPtr defaultCompiler = m_defaultCompilers.find(compiler->GetCompilerFamily())->second;
    return defaultCompiler->GetName() == compiler->GetName();
}
//-----------------------------------------------------------------------------
void CompilersFoundDlg::MSWUpdateToolchain(CompilerPtr compiler)
{
    wxUnusedVar(compiler);
#ifdef __WXMSW__
    if(compiler->GetCompilerFamily() == COMPILER_FAMILY_MINGW) {
        // Clang and VC lacks 2 tools: make and windres
        // so we copy those from the default MinGW compiler
        wxString make = compiler->GetTool("MAKE");
        wxString resourceCompiler = compiler->GetTool("ResourceCompiler");

        for(size_t i = 0; i < m_allCompilers.size(); ++i) {
            CompilerPtr c = m_allCompilers.at(i);

            if(c->GetCompilerFamily() == COMPILER_FAMILY_VC) {
                wxString vcMake = c->GetTool("MAKE");
                if (!vcMake.IsEmpty()) {
                    vcMake += " & ";
                }
                vcMake += make;
                c->SetTool("MAKE", vcMake);
            }

            if(c->GetCompilerFamily() == COMPILER_FAMILY_CLANG) {
                c->SetTool("MAKE", make);
                c->SetTool("ResourceCompiler", resourceCompiler);

                // Clang under Windows, needs the include paths from the MinGW compiler
                IncludePathLocator locator(NULL);
                wxArrayString includePaths, excludePaths;
                locator.Locate(includePaths, excludePaths, false, compiler->GetTool("CXX"));

                // Convert the include paths to semi colon separated list
                wxString mingwIncludePaths = wxJoin(includePaths, ';');
                c->SetGlobalIncludePath(mingwIncludePaths);
            }
        }
    }
#endif
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CompilersModifiedDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define SELECT_COMPILER "<Click to select a compiler...>"

CompilersModifiedDlg::CompilersModifiedDlg(wxWindow* parent, const gsgsStringSet& deletedCompilers, 
    wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_enableOKButton(false)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCFE1CInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer16 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer16);
    
    m_staticText27 = gsgs_Style().createStaticText(this, wxID_ANY, _("Some of the compilers referred  by the workspace no longer exist.\nDefine each missing compiler by cloning an existing compiler."), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer16->Add(m_staticText27, 0, wxALL, 5);
    
    wxArrayString m_pgMgrCompilersArr;
    wxUnusedVar(m_pgMgrCompilersArr);
    wxArrayInt m_pgMgrCompilersIntArr;
    wxUnusedVar(m_pgMgrCompilersIntArr);
    m_pgMgrCompilers = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxPG_DESCRIPTION|wxPG_SPLITTER_AUTO_CENTER|wxPG_BOLD_MODIFIED);
    
    boxSizer16->Add(m_pgMgrCompilers, 1, wxALL|wxEXPAND, 5);
    
    m_pgPropHeader = m_pgMgrCompilers->Append(  new wxPropertyCategory( _("Compilers") ) );
    m_pgPropHeader->SetHelpString(wxT(""));
    
    m_stdBtnSizer18 = new DialogButtonSizer();
    
    boxSizer16->Add(m_stdBtnSizer18, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_buttonOK->SetDefault();
    m_stdBtnSizer18->AddButton(m_buttonOK);
    
    m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer18->AddButton(m_buttonCancel);
    m_stdBtnSizer18->Realize();
    
    CentreOnParent(wxBOTH);

    // Connect events
    m_pgMgrCompilers->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CompilersModifiedDlg::OnValueChanged), NULL, this);
    m_buttonOK->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilersModifiedDlg::OnOKUI), NULL, this);
    
    wxArrayString compilers;
    compilers = gsgs_Settings().GetAllCompilersNames();
    compilers.Insert(SELECT_COMPILER, 0);
    
    gsgsStringSet::const_iterator iter = deletedCompilers.begin();
    for(; iter != deletedCompilers.end(); ++iter ) {
        m_table.insert( std::make_pair(*iter, SELECT_COMPILER) );
        wxPGProperty* prop = m_pgMgrCompilers->AppendIn( m_pgPropHeader,  new wxEnumProperty( *iter, wxPG_LABEL, compilers) );
        m_props.push_back( prop );
        
        wxString message;
        message << _("Create a new compiler named '") << *iter << "' by cloning an existing compiler";
        prop->SetHelpString( message );
    }
    SetName("CompilersModifiedDlg");
    gsgs::WindowManager::Load(this);
}
//-----------------------------------------------------------------------------
CompilersModifiedDlg::~CompilersModifiedDlg()
{
    m_pgMgrCompilers->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CompilersModifiedDlg::OnValueChanged), NULL, this);
    m_buttonOK->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CompilersModifiedDlg::OnOKUI), NULL, this);
}
//-----------------------------------------------------------------------------
void CompilersModifiedDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( m_enableOKButton );
}
//-----------------------------------------------------------------------------
void CompilersModifiedDlg::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    wxString newCompiler = event.GetProperty()->GetValueAsString();
    wxString oldCompiler = event.GetPropertyName();
    
    m_table.erase( oldCompiler );
    m_table[ oldCompiler ] = newCompiler;
    
    m_enableOKButton = true;
    gsgsStringMap::iterator iter = m_table.begin();
    for(; iter != m_table.end(); ++iter ) {
        if ( iter->second == SELECT_COMPILER ) {
            m_enableOKButton = false;
            break;
        }
    }
}

class ImportFilesDlgData : public wxClientData
{
    wxString m_path;
    bool m_isChecked;
    bool m_isDummy;

public:
    ImportFilesDlgData(const wxString& path, bool checked, bool isDummy = false)
        : m_path(path)
        , m_isChecked(checked)
        , m_isDummy(isDummy)
    {
    }

    virtual ~ImportFilesDlgData() {}

    void SetIsChecked(bool isChecked) { this->m_isChecked = isChecked; }
    void SetPath(const wxString& path) { this->m_path = path; }
    bool IsChecked() const { return m_isChecked; }
    const wxString& GetPath() const { return m_path; }
    void SetIsDummy(bool isDummy) { this->m_isDummy = isDummy; }
    bool IsDummy() const { return m_isDummy; }
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// ImportFilesSettings
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ImportFilesSettings::ImportFilesSettings()
    : m_fileMask("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++")
    , m_flags(0)
{
}
//-----------------------------------------------------------------------------
ImportFilesSettings::~ImportFilesSettings()
{
}
//-----------------------------------------------------------------------------
void ImportFilesSettings::DeSerialize(Serializer& arch)
{
    arch.Read(wxT("m_fileMask"), m_fileMask);
    arch.Read(wxT("m_flags"), m_flags);
    arch.Read(wxT("m_baseDir"), m_baseDir);
}
//-----------------------------------------------------------------------------
void ImportFilesSettings::Serialize(Serializer& arch)
{
    arch.Write(wxT("m_fileMask"), m_fileMask);
    arch.Write(wxT("m_flags"), m_flags);
    arch.Write(wxT("m_baseDir"), m_baseDir);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// ImportFilesDialogNew
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ImportFilesDialogNew::ImportFilesDialogNew(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);
    
    m_banner1 = new wxBannerWindow(this, wxID_ANY, wxTOP, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxBORDER_THEME);
    m_banner1->SetBitmap(wxNullBitmap);
    m_banner1->SetText(_("Import Files"), _("Select the directories to import from"));
    m_banner1->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE), wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    m_banner1->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    
    mainSizer->Add(m_banner1, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    
    mainSizer->Add(boxSizer12, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_textCtrlDir = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_PROCESS_ENTER);
    boxSizer12->Add(m_textCtrlDir, 1, wxRIGHT|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
    
    m_buttonBrowse = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxBU_EXACTFIT);
    
    boxSizer12->Add(m_buttonBrowse, 0, wxTOP|wxBOTTOM, WXC_FROM_DIP(5));
    
    m_dataview = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,200)), wxDV_VERT_RULES|wxDV_ROW_LINES|wxDV_SINGLE);
    m_dataview->SetToolTip(_("Check the folders you wish to import\nfiles from"));
    
    m_dataviewModel = new FolderModel;
    m_dataviewModel->SetColCount( 2 );
    m_dataview->AssociateModel(m_dataviewModel.get() );
    
    mainSizer->Add(m_dataview, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_dataview->AppendToggleColumn(_("?"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_ACTIVATABLE, WXC_FROM_DIP(30), wxALIGN_LEFT);
    m_dataview->AppendIconTextColumn(_("Folder"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_ACTIVATABLE, WXC_FROM_DIP(500), wxALIGN_LEFT);
    m_dataview->SetMinSize(wxSize(-1,200));
    
    m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Files extension to import (semicolon delimited):"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    mainSizer->Add(m_staticText1, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_textCtrSpec = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    mainSizer->Add(m_textCtrSpec, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_checkBoxFilesWOExt = new wxCheckBox(this, wxID_ANY, _("Import files without extensions"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxFilesWOExt->SetValue(false);
    m_checkBoxFilesWOExt->SetToolTip(_("If you wish to import files without extensions, tick this option"));
    
    mainSizer->Add(m_checkBoxFilesWOExt, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_stdBtnSizer7 = new DialogButtonSizer();
    
    mainSizer->Add(m_stdBtnSizer7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonOK->SetDefault();
    m_stdBtnSizer7->AddButton(m_buttonOK);
    
    m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer7->AddButton(m_buttonCancel);
    m_stdBtnSizer7->Realize();
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_textCtrlDir->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ImportFilesDialogNew::OnDirChanged), NULL, this);
    m_buttonBrowse->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ImportFilesDialogNew::OnBrowse), NULL, this);
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED, wxDataViewEventHandler(ImportFilesDialogNew::OnValueChanged), NULL, this);
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, wxDataViewEventHandler(ImportFilesDialogNew::OnItemExpanding), NULL, this);
    
    m_dataview->SetExpanderColumn(m_dataview->GetColumn(1));
    m_dataview->SetIndent(WXC_FROM_DIP(16));
    ImportFilesSettings options;
    if(!gsgs_EditorConfig().ReadObject(wxT("import_dir_options"), &options)) {
        // first time, read the settings from the ctags options
        options.SetFileMask(TagsManagerST::Get()->GetCtagsOptions().GetFileSpec());
        bool noExt = TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_PARSE_EXT_LESS_FILES ? true : false;
        size_t flags(0);
        if(noExt) {
            flags |= IFS_INCLUDE_FILES_WO_EXT;
        }
        options.SetFlags(flags);
    }

    options.SetBaseDir(PluginManager::Get()->GetSelectedTreeItemInfo(TreeFileView).m_fileName.GetPath());
    if(options.GetBaseDir().IsEmpty()) {
        options.SetBaseDir(wxGetCwd());
    }

    m_textCtrlDir->ChangeValue(options.GetBaseDir());
    m_textCtrSpec->SetValue(options.GetFileMask());
    m_checkBoxFilesWOExt->SetValue(options.GetFlags() & IFS_INCLUDE_FILES_WO_EXT);
    
    CentreOnParent();
    DoBuildTree();
}
//-----------------------------------------------------------------------------
ImportFilesDialogNew::~ImportFilesDialogNew()
{
    m_textCtrlDir->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ImportFilesDialogNew::OnDirChanged), NULL, this);
    m_buttonBrowse->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ImportFilesDialogNew::OnBrowse), NULL, this);
    m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED, wxDataViewEventHandler(ImportFilesDialogNew::OnValueChanged), NULL, this);
    m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, wxDataViewEventHandler(ImportFilesDialogNew::OnItemExpanding), NULL, this);
    
    ImportFilesSettings options;
    options.SetBaseDir(m_textCtrlDir->GetValue());
    options.SetFileMask(m_textCtrSpec->GetValue());

    size_t flags(0);
    if(m_checkBoxFilesWOExt->IsChecked()) flags |= IFS_INCLUDE_FILES_WO_EXT;
    options.SetFlags(flags);
    gsgs_EditorConfig().WriteObject(wxT("import_dir_options"), &options);
}
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::DoBuildTree(const wxDataViewItem& parent, const wxDir& dir, bool initialState)
{
    wxString path;
    bool cont = dir.GetFirst(&path, "", wxDIR_DIRS);
    wxBitmap folderBmp = gsgs_Image().LoadBitmap("mime/16/folder-yellow");

    // Collect list of directories
    std::vector<wxString> D;
    while(cont) {
        D.push_back(dir.GetNameWithSep() + path);
        cont = dir.GetNext(&path);
    }

    // Sort the directories
    std::sort(D.begin(), D.end());
    
    // Now build the tree
    std::for_each(D.begin(), D.end(), [&](const wxString& path) {
        wxDir childDir(path);
        wxVector<wxVariant> cols;
        cols.push_back(initialState);
        cols.push_back(MakeIconText(wxFileName(path, "").GetDirs().Last(), folderBmp));
        
        wxDataViewItem child =
            m_dataviewModel->AppendItem(parent, cols, new ImportFilesDlgData(childDir.GetName(), initialState));
        // Add dummy columns
        if(childDir.IsOpened() && childDir.HasSubDirs()) {
            wxVector<wxVariant> dummyCols;
            dummyCols.push_back(false);
            dummyCols.push_back(MakeIconText("dummy", folderBmp));
            m_dataviewModel->AppendItem(child, dummyCols, new ImportFilesDlgData("", false, true));
        }
    });
}
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::OnDirChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoBuildTree();
}
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::DoBuildTree()
{
    if(!wxFileName::DirExists(m_textCtrlDir->GetValue())) return;
    m_dataviewModel->Clear();

    wxString curpath = m_textCtrlDir->GetValue();
    if(!wxDir::Exists(curpath)) {
        m_textCtrlDir->ChangeValue(::wxGetCwd());
    }

    wxVector<wxVariant> cols;
    cols.push_back(false);
    cols.push_back(MakeIconText(m_textCtrlDir->GetValue(),
                                gsgs_Image().LoadBitmap("mime/16/folder-yellow")));

    m_root =
        m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new ImportFilesDlgData(m_textCtrlDir->GetValue(), false));

    // For performance, we add only the direct children of the root node
    wxDir dir(m_textCtrlDir->GetValue());
    if(dir.IsOpened() && dir.HasSubDirs()) {
        DoBuildTree(m_root, dir, false);
    }
    m_dataview->Expand(m_root);
}
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::OnValueChanged(wxDataViewEvent& event)
{
    event.Skip();
    wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(event.GetItem());
    if(cols.size() > 1) {
        bool isChecked = cols.at(0).GetBool();
        ImportFilesDlgData* cd =
            reinterpret_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(event.GetItem()));
        if(cd) {
            cd->SetIsChecked(isChecked);
            DoCheckChildren(event.GetItem(), isChecked);
        }
#ifdef __WXGTK__
        Refresh();
#endif
    }
}
//-----------------------------------------------------------------------------
wxVariant ImportFilesDialogNew::MakeIconText(const wxString& text, const wxBitmap& bmp) const
{
    wxIcon icn;
    icn.CopyFromBitmap(bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::DoCheckChildren(const wxDataViewItem& parent, bool check)
{
    if(m_dataviewModel->HasChildren(parent)) {
        wxDataViewItemArray children;
        m_dataviewModel->GetChildren(parent, children);
        for(size_t i = 0; i < children.GetCount(); ++i) {

            // First, update the UI by replacing the columns
            wxDataViewItem item = children.Item(i);
            m_dataviewModel->SetValue(wxVariant(check), item, 0);

            // Update the client data
            ImportFilesDlgData* cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(item));
            if(cd) {
                cd->SetIsChecked(check);
            }

            // Check if this child has children
            if(m_dataviewModel->HasChildren(item)) {
                DoCheckChildren(item, check);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::OnItemExpanding(wxDataViewEvent& event)
{
    event.Skip();
    wxDataViewItemArray children;
    if(m_dataviewModel->GetChildren(event.GetItem(), children)) {
        wxDataViewItem child = children.Item(0);
        ImportFilesDlgData* cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(child));
        if(cd && cd->IsDummy()) {
            cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(event.GetItem()));
            m_dataviewModel->DeleteItem(child);
            wxDir dir(cd->GetPath());
            DoBuildTree(event.GetItem(), dir, cd->IsChecked());
        }
    }
}
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::GetDirectories(wxStringBoolMap_t& dirs) { DoGetCheckedDirs(m_root, dirs); }
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::DoGetCheckedDirs(const wxDataViewItem& parent, wxStringBoolMap_t& dirs)
{
    wxDataViewItemArray children;
    bool itemExpanded = false;
    ImportFilesDlgData* cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(parent));
    if(cd) {
        bool bParentFolderChecked = cd->IsChecked();
        wxString dirname = cd->GetPath();
        bool recurse = false;
        // check if this item was expanded before
        if(m_dataviewModel->HasChildren(parent) && m_dataviewModel->GetChildren(parent, children)) {
            wxDataViewItem child = children.Item(0);
            cd = dynamic_cast<ImportFilesDlgData*>(m_dataviewModel->GetClientObject(child));

            // If the directory is checked and it was never expanded
            // we should recurse into it
            if(cd && cd->IsDummy()) {
                recurse = true;

            } else if(cd) {
                itemExpanded = true;
            }
        }

        if(bParentFolderChecked) {
            dirs.insert(std::make_pair(dirname, recurse));
        }
    }

    // if the parent has children and it was expanded by the user, keep on recursing
    if(!children.IsEmpty() && itemExpanded) {
        for(size_t i = 0; i < children.GetCount(); ++i) {
            DoGetCheckedDirs(children.Item(i), dirs);
        }
    }
}
//-----------------------------------------------------------------------------
bool ImportFilesDialogNew::ExtlessFiles() { return m_checkBoxFilesWOExt->IsChecked(); }
//-----------------------------------------------------------------------------
wxString ImportFilesDialogNew::GetBaseDir() { return m_textCtrlDir->GetValue(); }
//-----------------------------------------------------------------------------
wxString ImportFilesDialogNew::GetFileMask() { return m_textCtrSpec->GetValue(); }
//-----------------------------------------------------------------------------
void ImportFilesDialogNew::OnBrowse(wxCommandEvent& event)
{
    wxString new_path = wxDirSelector(_("Select working directory:"), m_textCtrlDir->GetValue(), wxDD_DEFAULT_STYLE,
                                      wxDefaultPosition, this);
    if(new_path.IsEmpty()) return;
    m_textCtrlDir->ChangeValue(new_path);
    DoBuildTree();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DependenciesPage
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DependenciesPage::DependenciesPage(wxWindow* parent, const wxString& projectName, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
    , m_projectName(projectName)
    , m_dirty(false)
{
    wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer1);
    
    m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select build order for configuration:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    boxSizer1->Add(m_staticText3, 0, wxALL, 5);
    
    wxArrayString m_choiceProjectConfigArr;
    m_choiceProjectConfig = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_choiceProjectConfigArr, 0);
    
    boxSizer1->Add(m_choiceProjectConfig, 0, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer1->Add(bSizer3, 1, wxEXPAND, 5);
    
    wxBoxSizer* bSizer31 = new wxBoxSizer(wxVERTICAL);
    
    bSizer3->Add(bSizer31, 5, wxEXPAND, 5);
    
    m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Projects:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer31->Add(m_staticText1, 0, wxLEFT|wxRIGHT|wxTOP, 5);
    
    wxArrayString m_checkListProjectListArr;
    m_checkListProjectList = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), m_checkListProjectListArr, wxLB_SINGLE);
    
    bSizer31->Add(m_checkListProjectList, 1, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);
    
    bSizer3->Add(bSizer4, 6, wxEXPAND, 5);
    
    m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Build Order:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer4->Add(m_staticText2, 0, wxLEFT|wxRIGHT|wxTOP, 5);
    
    wxBoxSizer* bSizer11 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer4->Add(bSizer11, 1, wxEXPAND, 5);
    
    wxBoxSizer* bSizer5 = new wxBoxSizer(wxVERTICAL);
    
    bSizer11->Add(bSizer5, 1, wxEXPAND, 5);
    
    wxArrayString m_listBoxBuildOrderArr;
    m_listBoxBuildOrder = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_listBoxBuildOrderArr, 0);
    
    bSizer5->Add(m_listBoxBuildOrder, 1, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* bSizer6 = new wxBoxSizer(wxVERTICAL);
    
    bSizer11->Add(bSizer6, 0, wxEXPAND, 5);
    
    wxBoxSizer* bSizer8 = new wxBoxSizer(wxVERTICAL);
    
    bSizer6->Add(bSizer8, 1, wxEXPAND, 5);
    
    m_buttonUp = new Button(this, wxID_UP, _("Up"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer8->Add(m_buttonUp, 0, wxALL|wxEXPAND, 5);
    
    m_buttonDown = new Button(this, wxID_DOWN, _("Down"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer8->Add(m_buttonDown, 0, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* bSizer7 = new wxBoxSizer(wxVERTICAL);
    
    bSizer6->Add(bSizer7, 0, 0, 5);
    
    m_buttonApply = new Button(this, wxID_APPLY, _("Apply"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer7->Add(m_buttonApply, 0, wxALL|wxEXPAND, 5);
    
    CentreOnParent(wxBOTH);
    // Connect events
    m_choiceProjectConfig->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(DependenciesPage::OnConfigChanged), NULL, this);
    m_checkListProjectList->Connect(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(DependenciesPage::OnCheckListItemToggled), NULL, this);
    m_buttonUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DependenciesPage::OnMoveUp), NULL, this);
    m_buttonDown->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DependenciesPage::OnMoveDown), NULL, this);
    m_buttonApply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DependenciesPage::OnApplyButton), NULL, this);
    m_buttonApply->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DependenciesPage::OnApplyButtonUI), NULL, this);
    
    Init();
}
DependenciesPage::~DependenciesPage()
{
    m_choiceProjectConfig->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(DependenciesPage::OnConfigChanged), NULL, this);
    m_checkListProjectList->Disconnect(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(DependenciesPage::OnCheckListItemToggled), NULL, this);
    m_buttonUp->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DependenciesPage::OnMoveUp), NULL, this);
    m_buttonDown->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DependenciesPage::OnMoveDown), NULL, this);
    m_buttonApply->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DependenciesPage::OnApplyButton), NULL, this);
    m_buttonApply->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DependenciesPage::OnApplyButtonUI), NULL, this);
    
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnConfigChanged(wxCommandEvent& event)
{
    if(m_dirty) {
        // save old configuration
        if(wxMessageBox(
               wxString::Format(_("Build order for configuration '%s' has been modified, would you like to save it?"),
                                m_currentSelection.GetData()),
               _("gsgs"), wxYES_NO | wxICON_QUESTION) == wxYES) {
            Save();
        }
        m_dirty = false;
    }

    m_currentSelection = event.GetString();
    // switch to new configuration
    DoPopulateControl(m_currentSelection);
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnMoveUp(wxCommandEvent& event)
{
    wxUnusedVar(event);
    OnUpCommand(m_listBoxBuildOrder);
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnMoveDown(wxCommandEvent& event)
{
    wxUnusedVar(event);
    OnDownCommand(m_listBoxBuildOrder);
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnUpCommand(wxListBox* list)
{
    wxString selectedString = list->GetStringSelection();

    int sel = list->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }

    sel--;
    if(sel < 0) {
        return;
    }

    // sel contains the new position we want to place the selection string
    list->Delete(sel + 1);
    list->Insert(selectedString, sel);
    list->Select(sel);
    m_dirty = true;
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnDownCommand(wxListBox* list)
{
    int sel = list->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }

    sel++;
    if(sel >= (int)list->GetCount()) {
        return;
    }

    // sel contains the new position we want to place the selection string
    wxString oldStr = list->GetString(sel);

    list->Delete(sel);
    list->Insert(oldStr, sel - 1);
    list->Select(sel);
    m_dirty = true;
}
//-----------------------------------------------------------------------------
void DependenciesPage::Save()
{
    // Save only if its dirty...
    if(m_dirty) {
        ProjectPtr proj = gsgs_Manager()->GetProject(m_projectName);

        wxArrayString depsArr;
        for(size_t i = 0; i < m_listBoxBuildOrder->GetCount(); i++) {
            depsArr.Add(m_listBoxBuildOrder->GetString((unsigned int)i));
        }

        if(m_currentSelection.IsEmpty()) {
            return;
        }

        proj->SetDependencies(depsArr, m_currentSelection);
    }
    m_dirty = false;
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnCheckListItemToggled(wxCommandEvent& event)
{
    int item = event.GetSelection();
    wxString name = m_checkListProjectList->GetString((unsigned int)item);
    if(!m_checkListProjectList->IsChecked((unsigned int)item)) {
        unsigned int buildOrderId = m_listBoxBuildOrder->FindString(name);
        if(buildOrderId != (unsigned int)wxNOT_FOUND) {
            m_listBoxBuildOrder->Delete(buildOrderId);
        }
    } else {
        m_listBoxBuildOrder->Append(name);
    }
    m_dirty = true;
}
//-----------------------------------------------------------------------------
void DependenciesPage::Init()
{
    wxString errMsg;
    ProjectPtr proj = gsgs_CppWorker().FindProjectByName(m_projectName, errMsg);
    if(proj) {

        // populate the choice control with the list of available configurations for this project
        ProjectSettingsPtr settings = proj->GetSettings();
        if(settings) {
            ProjectSettingsCookie cookie;
            BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
            while(bldConf) {
                m_choiceProjectConfig->Append(bldConf->GetName());
                bldConf = settings->GetNextBuildConfiguration(cookie);
            }
        }

        // by default select the first configuration
        if(m_choiceProjectConfig->GetCount() > 0) {
            m_choiceProjectConfig->SetSelection(0);
        }

        // select the active configuration
        BuildConfigPtr selBuildConf = gsgs_CppWorker().GetProjBuildConf(m_projectName, wxEmptyString);
        if(selBuildConf) {
            int where = m_choiceProjectConfig->FindString(selBuildConf->GetName());
            if(where != wxNOT_FOUND) {
                m_choiceProjectConfig->SetSelection(where);
            }
        }

        m_currentSelection = m_choiceProjectConfig->GetStringSelection();
        DoPopulateControl(m_choiceProjectConfig->GetStringSelection());

    } else {
        wxMessageBox(errMsg, _("光栅故事"));
        return;
    }
}
//-----------------------------------------------------------------------------
void DependenciesPage::DoPopulateControl(const wxString& configuration)
{
    wxString errMsg;
    ProjectPtr proj = gsgs_CppWorker().FindProjectByName(m_projectName, errMsg);
    if(!proj) {
        return;
    }

    m_listBoxBuildOrder->Clear();
    m_checkListProjectList->Clear();

    // initialize the build order listbox
    wxArrayString depArr = proj->GetDependencies(configuration);
    size_t i = 0;
    for(i = 0; i < depArr.GetCount(); i++) {
        wxString item = depArr.Item(i);
        m_listBoxBuildOrder->Append(item);
    }

    // initialize the project dependencies check list
    wxArrayString projArr;
    gsgs_Manager()->GetProjectList(projArr);

    for(i = 0; i < projArr.GetCount(); i++) {

        if(projArr.Item(i) != m_projectName) {
            int idx = m_checkListProjectList->Append(projArr.Item(i));
            m_checkListProjectList->Check(idx, depArr.Index(projArr.Item(i)) != wxNOT_FOUND);
        }
    }
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnApplyButton(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Save();
}
//-----------------------------------------------------------------------------
void DependenciesPage::OnApplyButtonUI(wxUpdateUIEvent& event) { event.Enable(m_dirty); }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// FileCheckList
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

FileCheckList::FileCheckList(wxWindow* parent,
                             wxWindowID id,
                             const wxString& title,
                             const wxPoint& pos,
                             const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_baseDir(wxGetCwd())
{
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer1);
    
    m_caption = gsgs_Style().createStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer1->Add(m_caption, 0, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer1->Add(bSizer3, 1, wxEXPAND, 5);
    
    wxArrayString m_fileCheckListArr;
    m_fileCheckList = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxSize(400,300), m_fileCheckListArr, wxLB_SINGLE);
    
    bSizer3->Add(m_fileCheckList, 1, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);
    
    bSizer3->Add(bSizer4, 0, wxEXPAND, 5);
    
    m_clearAll = new Button(this, wxID_ANY, _("C&lear All"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer4->Add(m_clearAll, 0, wxALL|wxEXPAND, 5);
    
    m_checkAllButton = new Button(this, wxID_ANY, _("Check &All"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer4->Add(m_checkAllButton, 0, wxALL|wxEXPAND, 5);
    
    m_selectedFilePath = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), wxTE_READONLY);
    bSizer1->Add(m_selectedFilePath, 0, wxALL|wxEXPAND, 5);
    
    m_stdBtnSizer2 = new DialogButtonSizer();
    
    bSizer1->Add(m_stdBtnSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer2->AddButton(m_buttonOK);
    
    m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer2->AddButton(m_buttonCancel);
    m_stdBtnSizer2->Realize();
    
    CentreOnParent(wxBOTH);

    // Connect events
    m_clearAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileCheckList::OnClearAll), NULL, this);
    m_checkAllButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileCheckList::OnCheckAll), NULL, this);
  
}
//-----------------------------------------------------------------------------
FileCheckList::~FileCheckList() 
{
    m_clearAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileCheckList::OnClearAll), NULL, this);
    m_checkAllButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileCheckList::OnCheckAll), NULL, this);
}
//-----------------------------------------------------------------------------
void FileCheckList::OnCheckAll(wxCommandEvent&)
{
    for(size_t i = 0; i < m_files.size(); i++) {
        m_files[i].second = true;
        m_fileCheckList->Check(i, true);
    }
}
//-----------------------------------------------------------------------------
void FileCheckList::OnClearAll(wxCommandEvent&)
{
    for(size_t i = 0; i < m_files.size(); i++) {
        m_files[i].second = false;
        m_fileCheckList->Check(i, false);
    }
}
//-----------------------------------------------------------------------------
void FileCheckList::OnFileSelected(wxCommandEvent& event)
{
    ShowFilePath(event.GetSelection());
}
//-----------------------------------------------------------------------------
void FileCheckList::OnFileCheckChanged(wxCommandEvent& event)
{
    m_files[event.GetInt()].second = m_fileCheckList->IsChecked(event.GetInt());
}
//-----------------------------------------------------------------------------
void FileCheckList::ShowFilePath(size_t n)
{
    wxFileName file = m_files[n].first;
    file.MakeRelativeTo(m_baseDir.GetFullPath());
    m_selectedFilePath->SetValue(file.GetFullPath());
}
//-----------------------------------------------------------------------------
void FileCheckList::SetCaption(const wxString& caption) 
{ 
    m_caption->SetLabel(caption); 
}
//-----------------------------------------------------------------------------
void FileCheckList::SetBaseDir(const wxFileName& dir)
{
    m_baseDir = dir;
    m_baseDir.MakeAbsolute();
}
//-----------------------------------------------------------------------------
void FileCheckList::SetFiles(const std::vector<std::pair<wxFileName, bool> >& files)
{
    m_files = files;
    m_fileCheckList->Clear();
    for(size_t i = 0; i < m_files.size(); i++) {
        m_fileCheckList->Append(m_files[i].first.GetFullName());
        m_fileCheckList->Check(i, m_files[i].second);
    }
    m_selectedFilePath->Clear();
    if(!m_files.empty()) {
        m_fileCheckList->Select(0);
        ShowFilePath(0);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// FindInFilesLocationsDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FindInFilesLocationsDlg::FindInFilesLocationsDlg(wxWindow* parent, const wxArrayString& locations, wxWindowID id, const wxString& title,
                                                         const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_initialLocations(locations)
{
    wxBoxSizer* boxSizer111 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer111);

    wxBoxSizer* boxSizer120 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer111->Add(boxSizer120, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxArrayString m_checkListBoxLocationsArr;
    m_checkListBoxLocations = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                                 m_checkListBoxLocationsArr, wxLB_SINGLE);
    m_checkListBoxLocations->SetFocus();

    boxSizer120->Add(m_checkListBoxLocations, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer125 = new wxBoxSizer(wxVERTICAL);

    boxSizer120->Add(boxSizer125, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP, WXC_FROM_DIP(5));

    m_buttonAdd = new Button(this, wxID_ANY, _("Add..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonAdd->SetToolTip(_("Add Folder..."));

    boxSizer125->Add(m_buttonAdd, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP, WXC_FROM_DIP(5));

    m_buttonDelete = new Button(this, wxID_ANY, _("Remove"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonDelete->SetToolTip(_("Remove the selected path"));

    boxSizer125->Add(m_buttonDelete, 0, wxALL, WXC_FROM_DIP(5));

    m_stdBtnSizer113 = new DialogButtonSizer();

    boxSizer111->Add(m_stdBtnSizer113, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

    m_button115 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button115->SetDefault();
    m_stdBtnSizer113->AddButton(m_button115);

    m_button117 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer113->AddButton(m_button117);
    m_stdBtnSizer113->Realize();

    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_buttonAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesLocationsDlg::OnAddPath),
                         NULL, this);
    m_buttonDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                            wxCommandEventHandler(FindInFilesLocationsDlg::OnDeletePath), NULL, this);
    m_buttonDelete->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindInFilesLocationsDlg::OnDeletePathUI), NULL,
                            this);
                            
    // Add the default locations
    // Populate the
    DoAppendItem(SEARCH_IN_WORKSPACE_FOLDER);
    m_initialLocations.Remove(SEARCH_IN_WORKSPACE_FOLDER);
    DoAppendItem(SEARCH_IN_WORKSPACE);
    m_initialLocations.Remove(SEARCH_IN_WORKSPACE);
    DoAppendItem(SEARCH_IN_PROJECT);
    m_initialLocations.Remove(SEARCH_IN_PROJECT);
    DoAppendItem(SEARCH_IN_CURR_FILE_PROJECT);
    m_initialLocations.Remove(SEARCH_IN_CURR_FILE_PROJECT);
    DoAppendItem(SEARCH_IN_CURRENT_FILE);
    m_initialLocations.Remove(SEARCH_IN_CURRENT_FILE);
    DoAppendItem(SEARCH_IN_OPEN_FILES);
    m_initialLocations.Remove(SEARCH_IN_OPEN_FILES);

    for(size_t i = 0; i < m_initialLocations.size(); ++i) {
        DoAppendItem(m_initialLocations.Item(i));
    }
    m_initialLocations.clear();
}
//-----------------------------------------------------------------------------
FindInFilesLocationsDlg::~FindInFilesLocationsDlg() 
{
    m_buttonAdd->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesLocationsDlg::OnAddPath),
                            NULL, this);
    m_buttonDelete->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                               wxCommandEventHandler(FindInFilesLocationsDlg::OnDeletePath), NULL, this);
    m_buttonDelete->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindInFilesLocationsDlg::OnDeletePathUI),
                               NULL, this);
}
//-----------------------------------------------------------------------------
void FindInFilesLocationsDlg::DoAppendItem(const wxString& str)
{
    int pos = m_checkListBoxLocations->Append(str);
    m_checkListBoxLocations->Check(pos, (m_initialLocations.Index(str) != wxNOT_FOUND));
}
//-----------------------------------------------------------------------------
wxArrayString FindInFilesLocationsDlg::GetLocations() const
{
    wxArrayString locs;
    for(size_t i = 0; i < m_checkListBoxLocations->GetCount(); ++i) {
        if(m_checkListBoxLocations->IsChecked(i)) {
            locs.Add(m_checkListBoxLocations->GetString(i));
        }
    }
    return locs;
}
//-----------------------------------------------------------------------------
void FindInFilesLocationsDlg::OnAddPath(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector();
    if(path.IsEmpty()) return;
    DoAppendItem(path, true);
}
//-----------------------------------------------------------------------------
void FindInFilesLocationsDlg::OnDeletePath(wxCommandEvent& event)
{
    int sel = m_checkListBoxLocations->GetSelection();
    if(sel == wxNOT_FOUND) return;
    m_checkListBoxLocations->Delete(sel);
}
//-----------------------------------------------------------------------------
void FindInFilesLocationsDlg::OnDeletePathUI(wxUpdateUIEvent& event)
{
    wxString selectedStr = m_checkListBoxLocations->GetStringSelection();
    event.Enable(!selectedStr.IsEmpty() && !selectedStr.StartsWith("<"));
}
//-----------------------------------------------------------------------------
void FindInFilesLocationsDlg::DoAppendItem(const wxString& str, bool check)
{
    int pos = m_checkListBoxLocations->Append(str);
    m_checkListBoxLocations->Check(pos, true);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// FindInFilesDialog
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FindInFilesDialog::FindInFilesDialog(wxWindow* parent, FindReplaceData& data, wxWindowID id, const wxString& title, const wxPoint& pos,
                                             const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_data(data)
{
    wxBoxSizer* boxSizer7 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer7);

    m_panelMainPanel =
        new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

    boxSizer7->Add(m_panelMainPanel, 1, wxALL | wxEXPAND, WXC_FROM_DIP(10));

    wxBoxSizer* boxSizer132 = new wxBoxSizer(wxHORIZONTAL);
    m_panelMainPanel->SetSizer(boxSizer132);

    wxBoxSizer* boxSizer95 = new wxBoxSizer(wxVERTICAL);

    boxSizer132->Add(boxSizer95, 1, wxEXPAND, WXC_FROM_DIP(5));

    wxFlexGridSizer* fgSizer41 = new wxFlexGridSizer(0, 2, 0, 0);
    fgSizer41->SetFlexibleDirection(wxBOTH);
    fgSizer41->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    fgSizer41->AddGrowableCol(1);
    fgSizer41->AddGrowableRow(3);

    boxSizer95->Add(fgSizer41, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText1 = gsgs_Style().createStaticText(m_panelMainPanel, wxID_ANY, _("Find What :"), wxDefaultPosition,
                                     wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);

    fgSizer41->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxArrayString m_findStringArr;
    m_findString = new wxComboBox(m_panelMainPanel, wxID_ANY, wxT(""), wxDefaultPosition,
                                  wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), m_findStringArr, 0);
    m_findString->SetToolTip(_("Find what"));
    m_findString->SetFocus();
#if wxVERSION_NUMBER >= 3000
    m_findString->SetHint(_("Find what"));
#endif

    fgSizer41->Add(m_findString, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticText102 = gsgs_Style().createStaticText(m_panelMainPanel, wxID_ANY, _("Replace With:"), wxDefaultPosition,
                                       wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);

    fgSizer41->Add(m_staticText102, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxArrayString m_replaceStringArr;
    m_replaceString = new wxComboBox(m_panelMainPanel, wxID_ANY, wxT(""), wxDefaultPosition,
                                     wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), m_replaceStringArr, 0);
#if wxVERSION_NUMBER >= 3000
    m_replaceString->SetHint(_("Replace with"));
#endif

    fgSizer41->Add(m_replaceString, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticText3 = gsgs_Style().createStaticText(m_panelMainPanel, wxID_ANY, _("Files:"), wxDefaultPosition,
                                     wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);
    m_staticText3->SetToolTip(_("File extensions to include in the search\nWildcards are supported"));

    fgSizer41->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxArrayString m_fileTypesArr;
    m_fileTypesArr.Add(wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc"));
    m_fileTypes = new wxComboBox(m_panelMainPanel, wxID_ANY, wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc"),
                                 wxDefaultPosition, wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), m_fileTypesArr, 0);
    m_fileTypes->SetToolTip(_("Search these file types"));
#if wxVERSION_NUMBER >= 3000
    m_fileTypes->SetHint(wxT(""));
#endif
    m_fileTypes->SetSelection(0);

    fgSizer41->Add(m_fileTypes, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_staticText2 = gsgs_Style().createStaticText(m_panelMainPanel, wxID_ANY, _("Search in:"), wxDefaultPosition,
                                     wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);
    m_staticText2->SetToolTip(
        _("Search in these folders\nTo exclude a file from the search, use wildcard that starts with an hyphen "
          "(\"-\")\nFor example, to exclude all matches from the node_modules folder, one can use something "
          "like:\n\n/home/user/path/to/root/folder\n-*node_modules*"));

    fgSizer41->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_TOP, WXC_FROM_DIP(5));

    m_stcPaths = new wxStyledTextCtrl(m_panelMainPanel, wxID_ANY, wxDefaultPosition,
                                      wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_stcPaths->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcPaths->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcPaths->SetMarginSensitive(4, true);
    m_stcPaths->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_stcPaths->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_stcPaths->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcPaths->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcPaths->SetMarginWidth(2, 0);
    m_stcPaths->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_stcPaths->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcPaths->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_stcPaths->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcPaths->SetMarginMask(3, 0);
    m_stcPaths->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcPaths->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcPaths->StyleClearAll();
    m_stcPaths->SetWrapMode(0);
    m_stcPaths->SetIndentationGuides(0);
    m_stcPaths->SetEOLMode(2);
    m_stcPaths->SetKeyWords(0, wxT(""));
    m_stcPaths->SetKeyWords(1, wxT(""));
    m_stcPaths->SetKeyWords(2, wxT(""));
    m_stcPaths->SetKeyWords(3, wxT(""));
    m_stcPaths->SetKeyWords(4, wxT(""));

    fgSizer41->Add(m_stcPaths, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText5 = gsgs_Style().createStaticText(m_panelMainPanel, wxID_ANY, _("Files Encoding:"), wxDefaultPosition,
                                     wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);

    fgSizer41->Add(m_staticText5, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxArrayString m_choiceEncodingArr;
    m_choiceEncoding = new wxChoice(m_panelMainPanel, wxID_ANY, wxDefaultPosition,
                                    wxDLG_UNIT(m_panelMainPanel, wxSize(300, -1)), m_choiceEncodingArr, 0);
    m_choiceEncoding->SetToolTip(_("Use this file encoding when scanning files for matches"));

    fgSizer41->Add(m_choiceEncoding, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer134 = new wxBoxSizer(wxVERTICAL);

    boxSizer132->Add(boxSizer134, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_find = new Button(m_panelMainPanel, wxID_OK, _("&Find"), wxDefaultPosition,
                          wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);
    m_find->SetDefault();
    m_find->SetToolTip(_("Begin search"));

    boxSizer134->Add(m_find, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_replaceAll = new Button(m_panelMainPanel, wxID_REPLACE, _("&Replace"), wxDefaultPosition,
                                wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);
    m_replaceAll->SetToolTip(
        _("Search for matches and place them in the 'Replace' window as candidates for possible replace operation"));

    boxSizer134->Add(m_replaceAll, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_cancel = new Button(m_panelMainPanel, wxID_CANCEL, _("Close"), wxDefaultPosition,
                            wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);
    m_cancel->SetToolTip(_("Close this dialog"));

    boxSizer134->Add(m_cancel, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_btnAddPath = new Button(m_panelMainPanel, wxID_ANY, _("Add Path..."), wxDefaultPosition,
                                wxDLG_UNIT(m_panelMainPanel, wxSize(-1, -1)), 0);
    m_btnAddPath->SetToolTip(_("Add new search location"));

    boxSizer134->Add(m_btnAddPath, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticLine139 =
        new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxLI_HORIZONTAL);

    boxSizer7->Add(m_staticLine139, 0, wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer135 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer7->Add(boxSizer135, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_matchCase = new wxCheckBox(this, wxID_ANY, _("Case"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_matchCase->SetValue(false);
    m_matchCase->SetToolTip(_("Toggle case sensitive search"));

    boxSizer135->Add(m_matchCase, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_matchWholeWord =
        new wxCheckBox(this, wxID_ANY, _("Word"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_matchWholeWord->SetValue(false);
    m_matchWholeWord->SetToolTip(_("Toggle whole word search"));

    boxSizer135->Add(m_matchWholeWord, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_regualrExpression =
        new wxCheckBox(this, wxID_ANY, _("Regex"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_regualrExpression->SetValue(false);
    m_regualrExpression->SetToolTip(_("The 'Find What' field is a regular expression"));

    boxSizer135->Add(m_regualrExpression, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkBoxPipeForGrep =
        new wxCheckBox(this, wxID_ANY, _("Pipe filter"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxPipeForGrep->SetValue(false);
    m_checkBoxPipeForGrep->SetToolTip(
        _("Use the pipe character (\"|\") as a special separator for applying additional filters. This has the similar "
          "effect as using the \"grep\" command line tool"));

    boxSizer135->Add(m_checkBoxPipeForGrep, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxSaveFilesBeforeSearching =
        new wxCheckBox(this, wxID_ANY, _("Save before"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxSaveFilesBeforeSearching->SetValue(false);
    m_checkBoxSaveFilesBeforeSearching->SetToolTip(_("Save any modified files before search starts"));

    boxSizer135->Add(m_checkBoxSaveFilesBeforeSearching, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_find->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnFind), NULL, this);
    m_find->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindInFilesDialog::OnFindWhatUI), NULL, this);
    m_replaceAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnReplace), NULL,
                          this);
    m_replaceAll->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindInFilesDialog::OnReplaceUI), NULL, this);
    m_cancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnButtonClose), NULL,
                      this);
    m_btnAddPath->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnAddPath), NULL,
                          this);

    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
    if(lexer) { lexer->Apply(m_stcPaths); }
    m_stcPaths->SetEOLMode(wxSTC_EOL_LF);

    // Store the find-in-files data
    wxString filemask = gsgs_Session().GetFindInFilesMaskForCurrentWorkspace();
    if(!filemask.IsEmpty()) { m_data.SetSelectedMask(filemask); }
    DoSetSearchPaths(m_data.GetSearchPaths());

    // Search for
    m_findString->Clear();
    m_findString->Append(m_data.GetFindStringArr());
    m_findString->SetValue(m_data.GetFindString());
    m_replaceString->Append(m_data.GetReplaceStringArr());
    m_replaceString->SetValue(m_data.GetReplaceString());
    m_fileTypes->SetSelection(0);

    m_matchCase->SetValue(m_data.GetFlags() & wxFRD_MATCHCASE);
    m_matchWholeWord->SetValue(m_data.GetFlags() & wxFRD_MATCHWHOLEWORD);
    m_regualrExpression->SetValue(m_data.GetFlags() & wxFRD_REGULAREXPRESSION);
    m_checkBoxSaveFilesBeforeSearching->SetValue(m_data.GetFlags() & wxFRD_SAVE_BEFORE_SEARCH);
    m_checkBoxPipeForGrep->SetValue(m_data.GetFlags() & wxFRD_ENABLE_PIPE_SUPPORT);
    // Set encoding
    wxArrayString astrEncodings;
    wxFontEncoding fontEnc;
    int selection(0);

    size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
    for(size_t i = 0; i < iEncCnt; i++) {
        fontEnc = wxFontMapper::GetEncoding(i);
        if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
            continue;
        }
        wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
        size_t pos = astrEncodings.Add(encodingName);

        if(m_data.GetEncoding() == encodingName) { selection = static_cast<int>(pos); }
    }

    m_choiceEncoding->Append(astrEncodings);
    if(m_choiceEncoding->IsEmpty() == false) { m_choiceEncoding->SetSelection(selection); }

    // Set the file mask
    DoSetFileMask();
}
//-----------------------------------------------------------------------------
FindInFilesDialog::~FindInFilesDialog() 
{ 
    m_find->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnFind), NULL, this);
    m_find->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindInFilesDialog::OnFindWhatUI), NULL, this);
    m_replaceAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnReplace),
                             NULL, this);
    m_replaceAll->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindInFilesDialog::OnReplaceUI), NULL, this);
    m_cancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnButtonClose),
                         NULL, this);
    m_btnAddPath->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnAddPath),
                             NULL, this);
    BuildFindReplaceData(); 
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::DoSetFileMask()
{
    // Get the output
    wxArrayString fileTypes = m_data.GetFileMask();
    wxArrayString::iterator iter = std::unique(fileTypes.begin(), fileTypes.end());

    // remove the non unique parts
    fileTypes.resize(std::distance(fileTypes.begin(), iter));

    // Create a single mask array
    m_fileTypes->Clear();

    // Remove empty entries
    wxArrayString tempMaskArr;
    std::for_each(fileTypes.begin(), fileTypes.end(), [&](wxString& item) {
        item.Trim().Trim(false);
        if(!item.IsEmpty()) { tempMaskArr.Add(item); }
    });
    fileTypes.swap(tempMaskArr);

    if(!fileTypes.IsEmpty()) {
        m_fileTypes->Append(fileTypes);
        wxString selectedMask = m_data.GetSelectedMask();
        SetFileMask(selectedMask);
    }
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::DoSearchReplace()
{
    SearchData data = DoGetSearchData();
    data.SetOwner(gsgs_Frame()->GetOutputPane()->GetReplaceResultsTab());
    DoSaveOpenFiles();
    SearchThreadST::Get()->PerformSearch(data);
    BuildFindReplaceData();
    EndModal(wxID_OK);
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::DoSearch()
{
    SearchData data = DoGetSearchData();
    data.SetOwner(gsgs_Frame()->GetOutputPane()->GetFindResultsTab());

    // check to see if we require to save the files
    DoSaveOpenFiles();
    SearchThreadST::Get()->PerformSearch(data);
    BuildFindReplaceData();
    EndModal(wxID_OK);
}
//-----------------------------------------------------------------------------
SearchData FindInFilesDialog::DoGetSearchData()
{
    SearchData data;
    wxString findStr(m_data.GetFindString());
    if(!m_findString->GetValue().IsEmpty()) { findStr = m_findString->GetValue(); }

    data.SetFindString(findStr);
    data.SetReplaceWith(m_replaceString->GetValue());

    m_data.SetFlags(GetSearchFlags());
    size_t flags = m_data.GetFlags();

    // If the 'Skip comments' is ON, remove the
    // 'colour comments' flag
    if(flags & wxFRD_SKIP_COMMENTS) { flags &= ~wxFRD_COLOUR_COMMENTS; }

    data.SetMatchCase((flags & wxFRD_MATCHCASE) != 0);
    data.SetMatchWholeWord((flags & wxFRD_MATCHWHOLEWORD) != 0);
    data.SetRegularExpression((flags & wxFRD_REGULAREXPRESSION) != 0);
    data.SetDisplayScope((flags & wxFRD_DISPLAYSCOPE) != 0);
    data.SetEncoding(m_choiceEncoding->GetStringSelection());
    data.SetSkipComments(flags & wxFRD_SKIP_COMMENTS);
    data.SetSkipStrings(flags & wxFRD_SKIP_STRINGS);
    data.SetColourComments(flags & wxFRD_COLOUR_COMMENTS);
    data.SetEnablePipeSupport(flags & wxFRD_ENABLE_PIPE_SUPPORT);
    wxArrayString searchWhere = GetPathsAsArray();
    wxArrayString files;
    wxArrayString rootDirs;
    wxArrayString excludePattern;

    for(size_t i = 0; i < searchWhere.GetCount(); ++i) {
        const wxString& rootDir = searchWhere.Item(i);
        if(rootDir.StartsWith("-")) {
            // An exclude pattern
            excludePattern.Add(rootDir.Mid(1));
        } else if(rootDir.StartsWith("F: ")) {
            // a file
            files.Add(rootDir.Mid(3));
        } else if(rootDir.StartsWith("W: ")) {
            // Add all project files
            wxArrayString projects;
            gsgs_CppWorker().GetProjectList(projects);
            for(size_t n = 0; n < projects.size(); ++n) {
                DoAddProjectFiles(projects.Item(n), files);
            }
        } else if(rootDir.StartsWith("V: ")) {
            // a virtual folder
            if(gsgs_CppWorker().IsOpen()) {
                wxString vd = rootDir.Mid(3);
                vd.Replace("/", ":"); // Virtual directory is expecting colons as the separator
                wxString projectName = vd.BeforeFirst(':');
                vd = vd.AfterFirst(':');
                ProjectPtr p = gsgs_CppWorker().GetProject(projectName);
                if(p) {
                    wxArrayString vdFiles;
                    p->GetFilesByVirtualDir(vd, vdFiles, true);
                    files.insert(files.end(), vdFiles.begin(), vdFiles.end());
                }
            }
        } else if(rootDir.StartsWith("P: ")) {
            // Project
            wxString projectName = rootDir.Mid(3);
            DoAddProjectFiles(projectName, files);

        } else if((rootDir == wxGetTranslation(SEARCH_IN_WORKSPACE_FOLDER)) &&
                  gsgs_Workspace().IsValid()) {
            // Add the workspace folder
            rootDirs.Add(gsgs_Workspace().GetWorkspace()->GetFileName().GetPath());

        } else if((rootDir == wxGetTranslation(SEARCH_IN_WORKSPACE)) || (rootDir == SEARCH_IN_WORKSPACE)) {
            if(!gsgs_Workspace().IsValid()) { continue; }
            gsgs_Workspace().GetWorkspace()->GetWorkspaceFiles(files);

        } else if((rootDir == wxGetTranslation(SEARCH_IN_PROJECT)) || (rootDir == SEARCH_IN_PROJECT)) {
            if(!gsgs_Workspace().IsValid()) continue;
            if(gsgs_Workspace().GetWorkspace()->IsProjectSupported()) {
                // get the active project files
                gsgs_Workspace().GetWorkspace()->GetProjectFiles("", files);
            } else {
                // search the entire workspace
                gsgs_Workspace().GetWorkspace()->GetWorkspaceFiles(files);
            }

        } else if((rootDir == wxGetTranslation(SEARCH_IN_CURR_FILE_PROJECT)) ||
                  (rootDir == SEARCH_IN_CURR_FILE_PROJECT)) {

            if(!gsgs_Workspace().IsValid()) continue;
            gsgs::IEditor* editor = GetManager()->GetActiveEditor();
            if(!editor) continue;

            if(gsgs_Workspace().GetWorkspace()->IsProjectSupported()) {
                wxString projectName =
                    gsgs_Workspace().GetWorkspace()->GetProjectFromFile(editor->GetFileName());
                gsgs_Workspace().GetWorkspace()->GetProjectFiles(projectName, files);
            } else {
                // search the entire workspace
                gsgs_Workspace().GetWorkspace()->GetWorkspaceFiles(files);
            }

        } else if((rootDir == wxGetTranslation(SEARCH_IN_CURRENT_FILE)) || (rootDir == SEARCH_IN_CURRENT_FILE)) {
            gsgs::Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
            if(editor) { files.Add(editor->GetFileName().GetFullPath()); }
        } else if((rootDir == wxGetTranslation(SEARCH_IN_OPEN_FILES)) || (rootDir == SEARCH_IN_OPEN_FILES)) {
            std::vector<gsgs::Editor*> editors;
            gsgs_Frame()->GetMainBook()->GetAllEditors(editors, gsgs::Book::kGetAll_IncludeDetached);

            for(size_t n = 0; n < editors.size(); ++n) {
                gsgs::Editor* editor = dynamic_cast<gsgs::Editor*>(*(editors.begin() + n));
                if(editor) { files.Add(editor->GetFileName().GetFullPath()); }
            }
        } else if(wxFileName::DirExists(searchWhere.Item(i))) {
            rootDirs.Add(searchWhere.Item(i));
        }
    }

    // Remove duplicates
    gsgsStringSet filesSet;
    wxArrayString uniqueFiles;
    // Unique files may contain up to files.size() elements
    uniqueFiles.Alloc(files.size());
    std::for_each(files.begin(), files.end(), [&](const wxString& file) {
        if(filesSet.count(file) == 0) {
            filesSet.insert(file);
            uniqueFiles.Add(file);
        }
    });

    // Release unused memory
    uniqueFiles.Shrink();
    files.swap(uniqueFiles);

    data.SetFiles(files);       // list of files
    data.SetRootDirs(rootDirs); // folders
    data.UseNewTab(false);
    data.SetExcludePatterns(excludePattern);
    data.SetExtensions(m_fileTypes->GetValue());
    return data;
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnClose(wxCloseEvent& e) { Destroy(); }
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnAddPath(wxCommandEvent& event)
{
#ifdef __WXOSX__
    // There is a bug in OSX that prevents popup menu from being displayed from dialogs
    // so we use an alternative way
    FindInFilesLocationsDlg dlg(this, GetPathsAsArray());
    if(dlg.ShowModal() == wxID_OK) { 
        wxString paths = wxJoin(dlg.GetLocations(), '\n');
        DoSetSearchPaths(paths); 
    }
#else
    // Show a popup menu
    wxMenu menu;
    int firstItem = 8994;
    menu.Append(firstItem + 6, "Add Folder...");
    menu.AppendSeparator();
    menu.Append(firstItem + 0, SEARCH_IN_WORKSPACE_FOLDER);
    menu.Append(firstItem + 1, SEARCH_IN_WORKSPACE);
    menu.Append(firstItem + 2, SEARCH_IN_PROJECT);
    menu.Append(firstItem + 3, SEARCH_IN_CURR_FILE_PROJECT);
    menu.Append(firstItem + 4, SEARCH_IN_CURRENT_FILE);
    menu.Append(firstItem + 5, SEARCH_IN_OPEN_FILES);
    menu.AppendSeparator();
    menu.Append(firstItem + 7, "Add Exclude Pattern");

    std::map<int, wxString> options;
    options.insert(std::make_pair(firstItem + 0, SEARCH_IN_WORKSPACE_FOLDER));
    options.insert(std::make_pair(firstItem + 1, SEARCH_IN_WORKSPACE));
    options.insert(std::make_pair(firstItem + 2, SEARCH_IN_PROJECT));
    options.insert(std::make_pair(firstItem + 3, SEARCH_IN_CURR_FILE_PROJECT));
    options.insert(std::make_pair(firstItem + 4, SEARCH_IN_CURRENT_FILE));
    options.insert(std::make_pair(firstItem + 5, SEARCH_IN_OPEN_FILES));

    // Menu will be shown in client coordinates
    wxRect size = m_btnAddPath->GetSize();
    wxPoint menuPos(0, size.GetHeight());
    int selection = m_btnAddPath->GetPopupMenuSelectionFromUser(menu, menuPos);

    if(selection == wxID_NONE) return;
    if(selection == (firstItem + 7)) {
        // Add exclude pattern
        int lastPos = m_stcPaths->GetLastPosition();
        wxChar ch = m_stcPaths->GetCharAt(lastPos);
        if(ch != '\n') {
            m_stcPaths->SetInsertionPointEnd();
            m_stcPaths->AddText("\n");
        }
        lastPos = m_stcPaths->GetLastPosition();
        m_stcPaths->SetSelectionStart(lastPos);
        m_stcPaths->SetSelectionEnd(lastPos);
        m_stcPaths->SetCurrentPos(lastPos);
        m_stcPaths->InsertText(lastPos, "-*PATTERN*");
        int selStart = lastPos + 1;
        int selEnd = selStart + wxStrlen("*PATTERN*");
        m_stcPaths->SetSelection(selStart, selEnd);
        m_stcPaths->CallAfter(&wxStyledTextCtrl::SetFocus);

    } else if(selection == (firstItem + 6)) {
        wxString folder = ::wxDirSelector();
        if(folder.IsEmpty()) return;
        DoSetSearchPaths(folder);

    } else if(options.count(selection)) {
        DoSetSearchPaths(options.find(selection)->second);
    }
#endif
}
//-----------------------------------------------------------------------------
int FindInFilesDialog::ShowDialog()
{
    // Update the combobox
    m_findString->Clear();
    m_findString->Append(m_data.GetFindStringArr());
    m_findString->SetValue(m_data.GetFindString());

    gsgs::Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
    if(editor) {
        // if we have an open editor, and a selected text, make this text the search string
        wxString selText = editor->GetSelectedText();
        if(!selText.IsEmpty()) { m_findString->SetValue(selText); }
    }

    m_findString->SetSelection(-1, -1); // select all
    m_findString->CallAfter(&wxTextCtrl::SetFocus);
    return wxDialog::ShowModal();
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::DoSaveSearchPaths() { m_data.SetSearchPaths(m_stcPaths->GetText()); }
//-----------------------------------------------------------------------------
void FindInFilesDialog::DoSaveOpenFiles()
{
    if(m_checkBoxSaveFilesBeforeSearching->IsChecked()) { gsgs_Frame()->GetMainBook()->SaveAll(false, false); }
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnFindWhatUI(wxUpdateUIEvent& event)
{
    wxArrayString paths = GetPathsAsArray();
    event.Enable(!m_findString->GetValue().IsEmpty() && !paths.IsEmpty());
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnUseDiffColourForCommentsUI(wxUpdateUIEvent& event)
{
    // event.Enable(m_checkBoxSkipMatchesFoundInComments->IsChecked() == false);
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnFind(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSearch();
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnReplace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSearchReplace();
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnButtonClose(wxCommandEvent& event) { EndModal(wxID_CANCEL); }
//-----------------------------------------------------------------------------
size_t FindInFilesDialog::GetSearchFlags()
{
    size_t flags = 0;
    if(m_matchCase->IsChecked()) flags |= wxFRD_MATCHCASE;
    if(m_matchWholeWord->IsChecked()) flags |= wxFRD_MATCHWHOLEWORD;
    if(m_regualrExpression->IsChecked()) flags |= wxFRD_REGULAREXPRESSION;
    if(m_checkBoxSaveFilesBeforeSearching->IsChecked()) flags |= wxFRD_SAVE_BEFORE_SEARCH;
    if(m_checkBoxPipeForGrep->IsChecked()) flags |= wxFRD_ENABLE_PIPE_SUPPORT;
    return flags;
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::SetSearchPaths(const wxString& paths, bool transient)
{
    m_stcPaths->ClearAll();
    m_transient = transient;
    DoSetSearchPaths(paths);
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::DoSetSearchPaths(const wxString& path)
{
    wxString text = m_stcPaths->GetText();
    text.Replace("\r", ""); // No \r
    text.Trim().Trim(false);
    if(!text.IsEmpty()) { text << "\n"; }
    text << path;
    text.Trim().Trim(false);
    m_stcPaths->SetText(text);
    ::CalcHScrollBar(m_stcPaths);
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnReplaceUI(wxUpdateUIEvent& event)
{
    wxArrayString paths = GetPathsAsArray();
    event.Enable(!m_findString->GetValue().IsEmpty() && !paths.IsEmpty());
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::OnLookInKeyDown(wxKeyEvent& event) { event.Skip(); }
//-----------------------------------------------------------------------------
void FindInFilesDialog::DoAddProjectFiles(const wxString& projectName, wxArrayString& files)
{
    if(!gsgs_CppWorker().IsOpen()) { return; }
    ProjectPtr p = gsgs_CppWorker().GetProject(projectName);
    if(p) {
        const Project::FilesMap_t& filesMap = p->GetFiles();
        if(!filesMap.empty()) {
            wxArrayString tmpArr;
            tmpArr.Alloc(filesMap.size());
            std::for_each(filesMap.begin(), filesMap.end(),
                          [&](const Project::FilesMap_t::value_type& vt) { tmpArr.Add(vt.second->GetFilename()); });
            files.insert(files.end(), tmpArr.begin(), tmpArr.end());
        }
    }
}
//-----------------------------------------------------------------------------
wxArrayString FindInFilesDialog::GetPathsAsArray() const
{
    wxString str = m_stcPaths->GetText();
    str.Replace("\r", "");
    wxArrayString arr = ::wxStringTokenize(str, "\n", wxTOKEN_STRTOK);
    return arr;
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::BuildFindReplaceData()
{
    m_data.SetFlags(GetSearchFlags());
    m_data.SetFindString(m_findString->GetValue());
    m_data.SetReplaceString(m_replaceString->GetValue());
    m_data.SetEncoding(m_choiceEncoding->GetStringSelection());
    wxString value = m_fileTypes->GetValue();
    value.Trim().Trim(false);

    wxArrayString masks = m_fileTypes->GetStrings();
    if(masks.Index(value) == wxNOT_FOUND) { masks.Insert(value, 0); }

    m_data.SetSelectedMask(value);
    m_data.SetFileMask(masks);

    // If the search paths were provided by the user, don't store them
    m_data.SetSearchPaths(m_stcPaths->GetText());
}
//-----------------------------------------------------------------------------
void FindInFilesDialog::SetFileMask(const wxString& mask)
{
    if(!mask.IsEmpty()) {
        int where = m_fileTypes->FindString(mask);
        if(where == wxNOT_FOUND) {
            // Add it
            where = m_fileTypes->Append(mask);
        }
        m_fileTypes->SetSelection(where);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// FilesModifiedDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilesModifiedDlg::FilesModifiedDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{

    wxBoxSizer* boxSizer26 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer26);
    
    m_staticText34 = gsgs_Style().createStaticText(this, wxID_ANY, _("Some files were modified outside of the editor.\nWhat would you like to do?"), wxDefaultPosition, wxSize(-1,-1), 0);
    wxFont m_staticText34Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticText34Font.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticText34->SetFont(m_staticText34Font);
    
    boxSizer26->Add(m_staticText34, 0, wxALL, 5);
    
    m_cmdLnkBtn30 = new wxCommandLinkButton(this, ID_BUTTON_LOAD, _("Reload."), _("Reload all the externally modified files"), wxDefaultPosition, wxSize(-1,-1), wxBU_LEFT);
    
    boxSizer26->Add(m_cmdLnkBtn30, 1, wxALL|wxEXPAND, 5);
    
    m_cmdLnkBtn28 = new wxCommandLinkButton(this, ID_BUTTON_CHOOSE, _("Choose."), _("Let me choose which file or files to reload"), wxDefaultPosition, wxSize(-1,-1), wxBU_LEFT);
    m_cmdLnkBtn28->SetFocus();
    
    boxSizer26->Add(m_cmdLnkBtn28, 1, wxALL|wxEXPAND, 5);
    
    m_cmdLnkBtnNone = new wxCommandLinkButton(this, ID_BUTTON_IGNORE, _("Ignore."), _("Don't reload any the externally modified files"), wxDefaultPosition, wxSize(-1,-1), wxBU_LEFT);
    
    boxSizer26->Add(m_cmdLnkBtnNone, 1, wxALL|wxEXPAND, 5);
    
    m_checkBoxRemember = new wxCheckBox(this, wxID_ANY, _("Remember my answer"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_checkBoxRemember->SetValue(false);
    
    boxSizer26->Add(m_checkBoxRemember, 0, wxALL, 5);
    
    CentreOnParent(wxBOTH);

    // Connect events
    m_cmdLnkBtn30->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FilesModifiedDlg::OnLoad), NULL, this);
    m_cmdLnkBtn28->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FilesModifiedDlg::OnChoose), NULL, this);
    m_cmdLnkBtnNone->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FilesModifiedDlg::OnIgnore), NULL, this);
    
}
//-----------------------------------------------------------------------------
FilesModifiedDlg::~FilesModifiedDlg() 
{
    m_cmdLnkBtn30->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FilesModifiedDlg::OnLoad), NULL, this);
    m_cmdLnkBtn28->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FilesModifiedDlg::OnChoose), NULL, this);
    m_cmdLnkBtnNone->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FilesModifiedDlg::OnIgnore), NULL, this);
}
//-----------------------------------------------------------------------------
void FilesModifiedDlg::OnChoose(wxCommandEvent& event)
{
    event.StopPropagation();
    CallAfter(&FilesModifiedDlg::EndModal, ID_BUTTON_CHOOSE);
}
//-----------------------------------------------------------------------------
void FilesModifiedDlg::OnLoad(wxCommandEvent& event)
{
    event.StopPropagation();
    CallAfter(&FilesModifiedDlg::EndModal, ID_BUTTON_LOAD);
}
//-----------------------------------------------------------------------------
void FilesModifiedDlg::OnIgnore(wxCommandEvent& event)
{
    event.StopPropagation();
    CallAfter(&FilesModifiedDlg::EndModal, ID_BUTTON_IGNORE);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DebuggerEditItemDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DebuggerEditItemDlg::DebuggerEditItemDlg(wxWindow* parent, const wxString &initialValue, wxWindowID id, const wxString& title,
                                                     const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* boxSizer24 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer24);

    m_panel32 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

    boxSizer24->Add(m_panel32, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer34 = new wxBoxSizer(wxVERTICAL);
    m_panel32->SetSizer(boxSizer34);

    m_staticText36 = gsgs_Style().createStaticText(m_panel32, wxID_ANY, _("Enter new expression:"), wxDefaultPosition,
                                      wxDLG_UNIT(m_panel32, wxSize(-1, -1)), 0);

    boxSizer34->Add(m_staticText36, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_textCtrl38 = gsgs_Style().createTextCtrl(m_panel32, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel32, wxSize(-1, -1)), 0);
    boxSizer34->Add(m_textCtrl38, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer26 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer24->Add(boxSizer26, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button28 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button28->SetDefault();

    boxSizer26->Add(m_button28, 0, wxALL, WXC_FROM_DIP(5));

    m_button30 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer26->Add(m_button30, 0, wxALL, WXC_FROM_DIP(5));

    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
    SetValue( initialValue );
}
//-----------------------------------------------------------------------------
DebuggerEditItemDlg::~DebuggerEditItemDlg()
{
}
//-----------------------------------------------------------------------------
wxString DebuggerEditItemDlg::GetValue() const
{
    return m_textCtrl38->GetValue().Trim();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// ProjectSettingsDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ProjectSettingsDlg, ProjectSettingsBaseDlg)
END_EVENT_TABLE()
//-----------------------------------------------------------------------------
ProjectSettingsDlg::ProjectSettingsDlg(wxWindow* parent, WorkspaceTab* workspaceTab, const wxString& configName,
                                       const wxString& projectName, const wxString& title)
#ifdef __WXMAC__
    : ProjectSettingsBaseDlg(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
                             wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
#else
    : ProjectSettingsBaseDlg(parent, wxID_ANY, title)
#endif
    , m_projectName(projectName)
    , m_configName(configName)
    , m_isDirty(false)
    , m_isCustomBuild(false)
    , m_isProjectEnabled(true)
    , m_workspaceTab(workspaceTab)
{
    DoGetAllBuildConfigs();
    MSWSetNativeTheme(m_treebook->GetTreeCtrl());
    BuildTree();
    LoadValues(m_configName);

    m_treebook->SetFocus();
    GetSizer()->Fit(this);

    wxSize sz = GetSize();
    CentreOnParent();
    SetName("ProjectSettingsDlg");
    gsgs::WindowManager::Load(this);

    // Make sure that all the controls are visible
    wxSize newSize = GetSize();
    if(newSize.x <= sz.x && newSize.y <= sz.y) { GetSizer()->Fit(this); }

    gsgs_Event().Connect(wxEVT_PROJECT_TREEITEM_CLICKED,
                                  wxCommandEventHandler(ProjectSettingsDlg::OnProjectSelected), NULL, this);
    gsgs_Event().Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(ProjectSettingsDlg::OnWorkspaceClosed),
                                  NULL, this);

    // No effects plz
    m_infobar->SetShowHideEffects(wxSHOW_EFFECT_NONE, wxSHOW_EFFECT_NONE);

    ShowHideDisabledMessage();
    if(!wxPersistenceManager::Get().Find(m_treebook)) {
        wxPersistenceManager::Get().RegisterAndRestore(m_treebook);
    } else {
        wxPersistenceManager::Get().Restore(m_treebook);
    }
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::DoClearDialog()
{
    m_choiceConfig->Clear();
    ClearValues();
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::BuildTree()
{
    PSGeneralPage* gp = new PSGeneralPage(m_treebook, m_projectName, m_choiceConfig->GetStringSelection(), this);
    m_treebook->AddPage(gp, _("General"), true);
    m_treebook->AddPage(new PSCompilerPage(m_treebook, m_projectName, this, gp), _("Compiler"));
    m_treebook->AddPage(new PSLinkerPage(m_treebook, this, gp), _("Linker"));
    m_treebook->AddPage(new PSEnvironmentPage(m_treebook, this), _("Environment"));
    m_treebook->AddPage(new PSDebuggerPage(m_treebook, this), _("Debugger"));
    m_treebook->AddPage(new PSResourcesPage(m_treebook, this), _("Resources"));

    m_treebook->AddPage(0, _("Pre / Post Build Commands"));
    m_treebook->AddSubPage(new PSBuildEventsPage(m_treebook, true, this), _("Pre Build"));
    m_treebook->AddSubPage(new PSBuildEventsPage(m_treebook, false, this), _("Post Build"));

    m_treebook->AddPage(0, _("Customize"));
    m_treebook->AddSubPage(new PSCustomBuildPage(m_treebook, m_projectName, this), _("Custom Build"));
    m_treebook->AddSubPage(new PSCustomMakefileRulesPage(m_treebook, this), _("Custom Makefile Rules"));

    m_treebook->AddPage(new PSCompletionPage(m_treebook, this), _("Code Completion"));
    m_treebook->AddPage(new GlobalSettingsPanel(m_treebook, m_projectName, this, gp), _("Global Settings"));
}
//-----------------------------------------------------------------------------
ProjectSettingsDlg::~ProjectSettingsDlg()
{
    m_workspaceTab->ProjectSettingsDlgClosed();

    gsgs_Event().Disconnect(wxEVT_PROJECT_TREEITEM_CLICKED,
                                     wxCommandEventHandler(ProjectSettingsDlg::OnProjectSelected), NULL, this);
    gsgs_Event().Disconnect(wxEVT_WORKSPACE_CLOSED,
                                     wxCommandEventHandler(ProjectSettingsDlg::OnWorkspaceClosed), NULL, this);

    PluginManager::Get()->UnHookProjectSettingsTab(m_treebook, m_projectName, wxEmptyString /* all tabs */);
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
    event.Skip();
    SaveValues();

    // Clear the project include paths cache
    gsgs_CppWorker().ClearIncludePathCache();
    gsgs_Manager()->CallAfter(&Manager::UpdateParserPaths, true);

#ifndef __WXMAC__
    Destroy();
#endif
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnButtonApply(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SaveValues();

    // Clear the project include paths cache
    gsgs_CppWorker().ClearIncludePathCache();
    gsgs_Manager()->CallAfter(&Manager::UpdateParserPaths, false);
    // Clear the dirty flag to (this disables the "Apply" button in the UI)
    SetIsDirty(false);
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::SaveValues()
{
    ProjectSettingsPtr projSettingsPtr = gsgs_Manager()->GetProjectSettings(m_projectName);
    BuildConfigPtr buildConf = projSettingsPtr->GetBuildConfiguration(m_configName);
    if(!buildConf) { return; }

    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i = 0; i < pageCount; i++) {
        wxWindow* page = m_treebook->GetPage(i);
        if(!page) continue;
        IProjectSettingsPage* p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) { p->Save(buildConf, projSettingsPtr); }
    }

    // save settings
    gsgs_Manager()->SetProjectSettings(m_projectName, projSettingsPtr);

    // Notify the plugins to save their data
    gsgs::ProjectSettingsEvent eventProjSettingsSaved(wxEVT_CMD_PROJ_SETTINGS_SAVED);
    eventProjSettingsSaved.SetProjectName(m_projectName);
    eventProjSettingsSaved.SetConfigName(m_configName);
    gsgs_Event().AddPendingEvent(eventProjSettingsSaved);
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::LoadValues(const wxString& configName)
{
    int sel = m_treebook->GetSelection();
    // Load the new tab for the new configuration
    PluginManager::Get()->HookProjectSettingsTab(m_treebook, m_projectName, configName);
    BuildConfigPtr buildConf;
    ProjectSettingsPtr projSettingsPtr = gsgs_Manager()->GetProjectSettings(m_projectName);
    buildConf = projSettingsPtr->GetBuildConfiguration(configName);
    if(!buildConf) { return; }

    if(buildConf->GetProjectType().IsEmpty()) {
        // no project type is defined by the
        buildConf->SetProjectType(projSettingsPtr->GetProjectType(wxEmptyString));
    }
    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i = 0; i < pageCount; i++) {
        wxWindow* page = m_treebook->GetPage(i);
        if(!page) continue; // NULL page ...
        IProjectSettingsPage* p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) {
            GlobalSettingsPanel* globalPage = dynamic_cast<GlobalSettingsPanel*>(page);
            if(globalPage) {
                // update the project name
                // globalPage->SetProjectName(m_projectName);
            }
            p->Load(buildConf);
        }
    }

    if(sel != wxNOT_FOUND) { m_treebook->SetSelection(sel); }
    m_isDirty = false;
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::ClearValues()
{
    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i = 0; i < pageCount; i++) {
        wxWindow* page = m_treebook->GetPage(i);
        if(!page) continue;

        IProjectSettingsPage* p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) { p->Clear(); }
    }
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnButtonHelp(wxCommandEvent& e)
{
    wxUnusedVar(e);

    ProjectPtr project = gsgs_Manager()->GetProject(m_projectName);
    gsgs::IEditor* editor = PluginManager::Get()->GetActiveEditor();

    MacrosDlg dlg(this, MacrosDlg::MacrosProject, project, editor);
    dlg.ShowModal();
#ifdef __WXMAC__
    Raise();
#endif
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnButtonApplyUI(wxUpdateUIEvent& event) { event.Enable(GetIsDirty()); }
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnConfigurationChanged(wxCommandEvent& event)
{
    event.Skip();
    if(m_isDirty) {
        if(wxMessageBox(_("Save changes before loading new configuration?"), _("Save Changes"),
                        wxICON_QUESTION | wxYES_NO | wxCENTER) == wxYES) {
            SaveValues();
        } else {
            ClearValues();
        }
    }

    m_configName = event.GetString();

    gsgs::WindowUpdateLocker locker(this);
    LoadValues(m_configName);

    m_treebook->SetFocus();
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnProjectSelected(wxCommandEvent& e)
{
    e.Skip();

    // Make sure we know which configuration to load for the new project

    BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(e.GetString(), "");
    CHECK_PTR_RET(bldConf);

    if(m_isDirty) {
        int answer = ::wxMessageBox(_("Save changes before loading new configuration?"), _("Save Changes"),
                                    wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCENTER);
        switch(answer) {
        case wxYES:
            SaveValues();
            break;
        case wxNO:
            break;
        default:
            // abort
            return;
        }
    }

    ClearValues();

    // another project was selected in the tree view
    m_projectName = e.GetString();
    m_configName = bldConf->GetName();

    // determine the correct configuration to load
    SetTitle(wxString() << m_projectName << " Project Settings");

    DoGetAllBuildConfigs();
    LoadValues(m_configName);
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::DoGetAllBuildConfigs()
{
    m_choiceConfig->Clear();
    wxArrayString configs;
    ProjectSettingsPtr projSettingsPtr = gsgs_Manager()->GetProjectSettings(m_projectName);
    ProjectSettingsCookie cookie;
    BuildConfigPtr conf = projSettingsPtr->GetFirstBuildConfiguration(cookie);
    while(conf) {
        configs.Add(conf->GetName());
        conf = projSettingsPtr->GetNextBuildConfiguration(cookie);
    }

    m_choiceConfig->Append(configs);
    int where = m_choiceConfig->FindString(m_configName);
    if(where != wxNOT_FOUND) {

        m_choiceConfig->SetSelection(where);

    } else if(!m_choiceConfig->IsEmpty()) {

        m_configName = m_choiceConfig->GetString(0);
        m_choiceConfig->SetSelection(0);

    } else {
        m_configName.clear();
    }
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
#ifndef __WXMAC__
    Destroy();
#endif
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnButtonCancel(wxCommandEvent& event)
{
    event.Skip();
#ifndef __WXMAC__
    Destroy();
#endif
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::ShowHideDisabledMessage()
{
    if(!m_isProjectEnabled) {
        m_infobar->ShowMessage(_("This project is disabled"), wxICON_WARNING);
    } else {
        m_infobar->Dismiss();
    }
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::ShowCustomProjectMessage(bool show)
{
    if(show) {
        m_infobar->ShowMessage(_("The settings on this page are ignored during build"), wxICON_INFORMATION);
    } else {
        m_infobar->Dismiss();
    }
}
//-----------------------------------------------------------------------------
void ProjectSettingsDlg::OnPageChanged(wxTreebookEvent& event)
{
    event.Skip();

    // Do nothing if the project is disabled
    if(!IsProjectEnabled()) return;

    int sel = m_treebook->GetSelection();
    if(sel != wxNOT_FOUND && IsCustomBuildEnabled()) {
        wxWindow* page = m_treebook->GetPage(sel);

        if(!page || dynamic_cast<PSCustomBuildPage*>(page) || dynamic_cast<PSGeneralPage*>(page) ||
           dynamic_cast<PSBuildEventsPage*>(page) || dynamic_cast<PSEnvironmentPage*>(page) ||
           dynamic_cast<PSDebuggerPage*>(page) || dynamic_cast<PSCompletionPage*>(page)) {
            ShowCustomProjectMessage(false);

        } else {
            ShowCustomProjectMessage(true);
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// GlobalSettingsPanel
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GlobalSettingsPanel::GlobalSettingsPanel(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg,
                                         PSGeneralPage* gp)
    : GlobalSettingsBasePanel(parent)
    , m_projectName(projectName)
    , m_dlg(dlg)
    , m_gp(gp)
{
    GetSizer()->Fit(this);
    Centre();
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
}
//-----------------------------------------------------------------------------
void GlobalSettingsPanel::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        if(iter.GetProperty() && !iter.GetProperty()->IsCategory()) { iter.GetProperty()->SetValueToUnspecified(); }
    }
}
//-----------------------------------------------------------------------------
void GlobalSettingsPanel::Load(BuildConfigPtr buildConf)
{
    ProjectSettingsPtr projSettingsPtr = gsgs_Manager()->GetProjectSettings(m_projectName);
    BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
    if(!globalSettings) {
        Clear();
        return;
    }

    m_pgPropCCmpOpts->SetValue(globalSettings->GetCCompileOptions());
    m_pgPropCppCmpOpts->SetValue(globalSettings->GetCompileOptions());
    m_pgPropPreProcessors->SetValue(globalSettings->GetPreprocessor());
    m_pgPropIncludePaths->SetValue(globalSettings->GetIncludePath());
    m_pgPropOptions->SetValue(globalSettings->GetLinkOptions());
    m_pgPropLIbs->SetValue(globalSettings->GetLibraries());
    m_pgPropLibPath->SetValue(globalSettings->GetLibPath());
    m_pgPropResCmpOptions->SetValue(globalSettings->GetResCompileOptions());
    m_pgPropResCmpSearchPath->SetValue(globalSettings->GetResCmpIncludePath());
}
//-----------------------------------------------------------------------------
void GlobalSettingsPanel::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    wxUnusedVar(buildConf);
    wxUnusedVar(projSettingsPtr);

    BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
    if(!globalSettings) { return; }

    globalSettings->SetCompileOptions(m_pgPropCppCmpOpts->GetValueAsString());
    globalSettings->SetCCompileOptions(m_pgPropCCmpOpts->GetValueAsString());
    globalSettings->SetIncludePath(m_pgPropIncludePaths->GetValueAsString());
    globalSettings->SetPreprocessor(m_pgPropPreProcessors->GetValueAsString());

    globalSettings->SetLibPath(m_pgPropLibPath->GetValueAsString());
    globalSettings->SetLibraries(m_pgPropLIbs->GetValueAsString());
    globalSettings->SetLinkOptions(m_pgPropOptions->GetValueAsString());

    globalSettings->SetResCmpIncludePath(m_pgPropResCmpSearchPath->GetValueAsString());
    globalSettings->SetResCmpOptions(m_pgPropResCmpOptions->GetValueAsString());

    // save settings
    gsgs_Manager()->SetProjectGlobalSettings(m_projectName, globalSettings);
}
//-----------------------------------------------------------------------------
void GlobalSettingsPanel::OnCustomEditorClicked(wxCommandEvent& event)
{
    m_dlg->SetIsDirty(true);
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);

    wxString cmpName = m_gp->GetCompiler();
    CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpName);

    if(prop == m_pgPropCCmpOpts || prop == m_pgPropCppCmpOpts) {
        wxString v = prop->GetValueAsString();
        if(PopupAddOptionCheckDlg(v, _("Compiler Options"), cmp->GetCompilerOptions())) { prop->SetValueFromString(v); }
    } else if(prop == m_pgPropIncludePaths || prop == m_pgPropPreProcessors || prop == m_pgPropLibPath ||
              prop == m_pgPropLIbs || prop == m_pgPropResCmpOptions || prop == m_pgPropResCmpSearchPath) {
        wxString v = prop->GetValueAsString();
        if(PopupAddOptionDlg(v)) { prop->SetValueFromString(v); }
    } else if(prop == m_pgPropOptions) {
        wxString v = prop->GetValueAsString();
        if(PopupAddOptionCheckDlg(v, _("Linker Options"), cmp->GetLinkerOptions())) { prop->SetValueFromString(v); }
    }
}
//-----------------------------------------------------------------------------
void GlobalSettingsPanel::OnValueChanged(wxPropertyGridEvent& event) { m_dlg->SetIsDirty(true); }
//-----------------------------------------------------------------------------
bool IProjectSettingsPage::PopupAddOptionDlg(wxTextCtrl* ctrl)
{
    OptionAddDlg dlg;
    dlg.SetValue(ctrl->GetValue());
    if(dlg.ShowModal() == wxID_OK) {
        ctrl->SetValue(dlg.GetValue());
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
bool IProjectSettingsPage::PopupAddOptionDlg(wxString& value)
{
    OptionAddDlg dlg;
    dlg.SetValue(value);
    if(dlg.ShowModal() == wxID_OK) {
        value.Clear();
        value << dlg.GetValue();
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
bool IProjectSettingsPage::SelectChoiceWithGlobalSettings(wxChoice* c, const wxString& text)
{
    if(text == BuildConfig::APPEND_TO_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::APPEND_TO_GLOBAL_SETTINGS));

    } else if(text == BuildConfig::OVERWRITE_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::OVERWRITE_GLOBAL_SETTINGS));

    } else if(text == BuildConfig::PREPEND_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::PREPEND_GLOBAL_SETTINGS));

    } else {
        c->Select(c->FindString(BuildConfig::APPEND_TO_GLOBAL_SETTINGS));
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool IProjectSettingsPage::PopupAddOptionCheckDlg(wxTextCtrl* ctrl, const wxString& title,
                                                  const Compiler::CmpCmdLineOptions& options)
{
    AddOptionCheckDlg dlg(NULL, title, options, ctrl->GetValue());
    if(dlg.ShowModal() == wxID_OK) {
        ctrl->SetValue(dlg.GetValue());
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
bool IProjectSettingsPage::PopupAddOptionCheckDlg(wxString& v, const wxString& title,
                                                  const Compiler::CmpCmdLineOptions& options)
{
    AddOptionCheckDlg dlg(NULL, title, options, v);
    if(dlg.ShowModal() == wxID_OK) {
        v = dlg.GetValue();
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void IProjectSettingsPage::SelectChoiceWithGlobalSettings(wxPGProperty* p, const wxString& text)
{
    wxPGChoices choices;
    choices.Add(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);
    choices.Add(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);
    choices.Add(BuildConfig::PREPEND_GLOBAL_SETTINGS);
    p->SetChoices(choices);
    p->SetChoiceSelection(choices.Index(text));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MoveFuncImplDlg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MoveFuncImplDlg::MoveFuncImplDlg(wxWindow* parent, const wxString& text, const wxString& fileName, wxWindowID id, 
    const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* bSizer6 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer6);
    
    wxBoxSizer* bSizer7 = new wxBoxSizer(wxVERTICAL);
    
    bSizer6->Add(bSizer7, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select the file in which to place the function implementation:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer7->Add(m_staticText4, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_filePicker = new gsgs::FilePicker(this);
    bSizer7->Add(m_filePicker, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText5 = gsgs_Style().createStaticText(this, wxID_ANY, _("Function implementation (you can edit the code below):"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer7->Add(m_staticText5, 0, wxALL, WXC_FROM_DIP(5));
    
    m_preview = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    wxFont m_previewFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Sans"));
    m_preview->SetFont(m_previewFont);
    // Configure the fold margin
    m_preview->SetMarginType     (4, wxSTC_MARGIN_SYMBOL);
    m_preview->SetMarginMask     (4, wxSTC_MASK_FOLDERS);
    m_preview->SetMarginSensitive(4, true);
    m_preview->SetMarginWidth    (4, 0);
    
    // Configure the tracker margin
    m_preview->SetMarginWidth(1, 0);
    
    // Configure the symbol margin
    m_preview->SetMarginType (2, wxSTC_MARGIN_SYMBOL);
    m_preview->SetMarginMask (2, ~(wxSTC_MASK_FOLDERS));
    m_preview->SetMarginWidth(2, 0);
    m_preview->SetMarginSensitive(2, true);
    
    // Configure the line numbers margin
    int m_preview_PixelWidth = 4 + 5 *m_preview->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_preview->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_preview->SetMarginWidth(0,m_preview_PixelWidth);
    
    // Configure the line symbol margin
    m_preview->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_preview->SetMarginMask(3, 0);
    m_preview->SetMarginWidth(3,0);
    // Select the lexer
    m_preview->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_preview->StyleClearAll();
    for(int i=0; i<wxSTC_STYLE_MAX; ++i) {
        m_preview->StyleSetFont(i, m_previewFont);
    }
    m_preview->SetWrapMode(0);
    m_preview->SetIndentationGuides(0);
    m_preview->SetKeyWords(0, wxT(""));
    m_preview->SetKeyWords(1, wxT(""));
    m_preview->SetKeyWords(2, wxT(""));
    m_preview->SetKeyWords(3, wxT(""));
    m_preview->SetKeyWords(4, wxT(""));
    
    bSizer7->Add(m_preview, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_stdBtnSizer2 = new DialogButtonSizer();
    
    bSizer6->Add(m_stdBtnSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));
    
    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonOK->SetDefault();
    m_stdBtnSizer2->AddButton(m_buttonOK);
    
    m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer2->AddButton(m_buttonCancel);
    m_stdBtnSizer2->Realize();

    if(GetParent()) {
        CentreOnParent();
    } else {
        CentreOnScreen();
    }

    // Connect events
    m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MoveFuncImplDlg::OnButtonOK), NULL, this);
    m_buttonCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MoveFuncImplDlg::OnButtonCancel), NULL, this);
    
    
    m_filePicker->SetPath(fileName);
    LexerStyle::Ptr_t lexerCpp = gsgs_Style().GetLexer("c++");
    if(lexerCpp) {
        lexerCpp->Apply(m_preview, true);
    }
    
    // Format the source code before using it
    gsgs::SourceFormatEvent event(wxEVT_FORMAT_STRING);
    event.SetInputString(text);
    event.SetFileName(wxFileName(fileName).GetFullName());
    
    gsgs_Event().ProcessEvent(event);
    if(!event.GetFormattedString().IsEmpty()) {
        m_preview->SetText(event.GetFormattedString());
    } else {
        m_preview->SetText(text);
    }
    m_preview->CallAfter(&wxStyledTextCtrl::SetFocus);
    CentreOnParent();
}
//-----------------------------------------------------------------------------
MoveFuncImplDlg::~MoveFuncImplDlg()
{
    m_buttonOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MoveFuncImplDlg::OnButtonOK), NULL, this);
    m_buttonCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MoveFuncImplDlg::OnButtonCancel), NULL, this);
}
//-----------------------------------------------------------------------------
void MoveFuncImplDlg::SetText(const wxString& text) { m_preview->SetText(text); }
//-----------------------------------------------------------------------------
wxString MoveFuncImplDlg::GetText() { return m_preview->GetText(); }
//-----------------------------------------------------------------------------
void MoveFuncImplDlg::SetFileName(const wxString& fileName) { m_filePicker->SetPath(fileName); }
//-----------------------------------------------------------------------------
wxString MoveFuncImplDlg::GetFileName() { return m_filePicker->GetPath(); }
//-----------------------------------------------------------------------------
void MoveFuncImplDlg::OnButtonCancel(wxCommandEvent& e)
{
    wxUnusedVar(e);
    EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------
void MoveFuncImplDlg::OnButtonOK(wxCommandEvent& e)
{
    wxUnusedVar(e);
    // make sure that the file exist
    if(!wxFileName::FileExists(m_filePicker->GetPath())) {
        wxMessageBox(
            _("File: ") + m_filePicker->GetPath() + _(" does not exist"), _("gsgs"), wxICON_WARNING | wxOK);
        return;
    }
    EndModal(wxID_OK);
}
//-----------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// CodeCompletionSettingsDialog
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CodeCompletionSettingsDialog::CodeCompletionSettingsDialog(wxWindow* parent, const TagsOptionsData& data, wxWindowID id, 
    const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_data(data)
{
    wxBoxSizer* boxSizer100 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer100);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    boxSizer100->Add(mainSizer, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    m_notebook87 = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBK_DEFAULT);
    m_notebook87->SetName(wxT("m_notebook87"));

    mainSizer->Add(m_notebook87, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_paneDisplayAndBehavior = new wxPanel(m_notebook87, wxID_ANY, wxDefaultPosition,
                                           wxDLG_UNIT(m_notebook87, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebook87->AddPage(m_paneDisplayAndBehavior, _("General"), true);

    wxBoxSizer* bSizer19 = new wxBoxSizer(wxVERTICAL);
    m_paneDisplayAndBehavior->SetSizer(bSizer19);

    wxFlexGridSizer* flexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer12->SetFlexibleDirection(wxBOTH);
    flexGridSizer12->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer12->AddGrowableCol(1);

    bSizer19->Add(flexGridSizer12, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText14 =
        gsgs_Style().createStaticText(m_paneDisplayAndBehavior, wxID_ANY, _("Number of items to display in the completion box:"),
                         wxDefaultPosition, wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);

    flexGridSizer12->Add(m_staticText14, 0, wxLEFT | wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_spinCtrlNumberOfCCItems = gsgs_Style().createTextCtrl(m_paneDisplayAndBehavior, wxID_ANY, wxT("50"), wxDefaultPosition,
                                               wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    flexGridSizer12->Add(m_spinCtrlNumberOfCCItems, 0, wxALL | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(5));

    m_staticText123 = gsgs_Style().createStaticText(m_paneDisplayAndBehavior, wxID_ANY, _("Additional file extensions to parse:"),
                                       wxDefaultPosition, wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);

    flexGridSizer12->Add(m_staticText123, 0, wxALL, WXC_FROM_DIP(5));

    m_textCtrlFileSpec = gsgs_Style().createTextCtrl(m_paneDisplayAndBehavior, wxID_ANY, wxT(""), wxDefaultPosition,
                                        wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_textCtrlFileSpec->SetToolTip(_("In addition to the default C/C++ file extensions, you may add here\nadditional "
                                     "file extensions so gsgs will know to parse them as\nC/C++ files"));

    flexGridSizer12->Add(m_textCtrlFileSpec, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticLine129 = new wxStaticLine(m_paneDisplayAndBehavior, wxID_ANY, wxDefaultPosition,
                                       wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), wxLI_HORIZONTAL);

    bSizer19->Add(m_staticLine129, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxFlexGridSizer* flexGridSizer127 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer127->SetFlexibleDirection(wxBOTH);
    flexGridSizer127->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    bSizer19->Add(flexGridSizer127, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkDisplayTypeInfo = new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Display type info tooltips"),
                                            wxDefaultPosition, wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkDisplayTypeInfo->SetValue(false);
    m_checkDisplayTypeInfo->SetToolTip(_("Display information about the hovered text"));

    flexGridSizer127->Add(m_checkDisplayTypeInfo, 0, wxALL | wxALIGN_LEFT, WXC_FROM_DIP(5));

    m_checkBoxEnableCaseSensitiveCompletion =
        new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Code Completion is case sensitive"), wxDefaultPosition,
                       wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkBoxEnableCaseSensitiveCompletion->SetValue(false);
    m_checkBoxEnableCaseSensitiveCompletion->SetToolTip(
        _("When enabled, the code completion search engine will use case sensitive searches. \nSo 'QString' is NOT "
          "equal 'qstring'"));

    flexGridSizer127->Add(m_checkBoxEnableCaseSensitiveCompletion, 0, wxALL | wxALIGN_LEFT, WXC_FROM_DIP(5));

    m_checkDisplayFunctionTip =
        new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Display function calltip"), wxDefaultPosition,
                       wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkDisplayFunctionTip->SetValue(false);
    m_checkDisplayFunctionTip->SetToolTip(_("Display function argument list after typing an open brace '('"));

    flexGridSizer127->Add(m_checkDisplayFunctionTip, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxKeepFunctionSignature =
        new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Keep function signature un-formatted"), wxDefaultPosition,
                       wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkBoxKeepFunctionSignature->SetValue(false);

    flexGridSizer127->Add(m_checkBoxKeepFunctionSignature, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkCppKeywordAssist =
        new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Display completion box for language keywords"),
                       wxDefaultPosition, wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkCppKeywordAssist->SetValue(false);
    m_checkCppKeywordAssist->SetToolTip(
        _("When enabled, gsgs will auto show the code completion box for C/C++ keywords after typing 2 chars"));

    flexGridSizer127->Add(m_checkCppKeywordAssist, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxretagWorkspaceOnStartup =
        new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Re-parse on workspace loaded"), wxDefaultPosition,
                       wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkBoxretagWorkspaceOnStartup->SetValue(false);
    m_checkBoxretagWorkspaceOnStartup->SetToolTip(_("Retag workspace once loaded"));

    flexGridSizer127->Add(m_checkBoxretagWorkspaceOnStartup, 0, wxALL, WXC_FROM_DIP(5));

    m_checkDisableParseOnSave =
        new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Do not parse the file after saving it"),
                       wxDefaultPosition, wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkDisableParseOnSave->SetValue(false);
    m_checkDisableParseOnSave->SetToolTip(_("Do not trigger file parsing after saving a file"));

    flexGridSizer127->Add(m_checkDisableParseOnSave, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxDeepUsingNamespaceResolving =
        new wxCheckBox(m_paneDisplayAndBehavior, wxID_ANY, _("Crawl all files to locate 'using namespace' calls"),
                       wxDefaultPosition, wxDLG_UNIT(m_paneDisplayAndBehavior, wxSize(-1, -1)), 0);
    m_checkBoxDeepUsingNamespaceResolving->SetValue(false);
    m_checkBoxDeepUsingNamespaceResolving->SetToolTip(
        _("Always search for 'using namespace' statements in all included files"));

    flexGridSizer127->Add(m_checkBoxDeepUsingNamespaceResolving, 0, wxALL, WXC_FROM_DIP(5));

    m_paneColouring = new wxPanel(m_notebook87, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook87, wxSize(-1, -1)),
                                  wxTAB_TRAVERSAL);
    m_notebook87->AddPage(m_paneColouring, _("Colouring"), false);

    wxBoxSizer* bSizer16 = new wxBoxSizer(wxVERTICAL);
    m_paneColouring->SetSizer(bSizer16);

    wxArrayString m_pgMgrColouringArr;
    wxUnusedVar(m_pgMgrColouringArr);
    wxArrayInt m_pgMgrColouringIntArr;
    wxUnusedVar(m_pgMgrColouringIntArr);
    m_pgMgrColouring = new wxPropertyGridManager(m_paneColouring, wxID_ANY, wxDefaultPosition,
                                                 wxDLG_UNIT(m_paneColouring, wxSize(-1, -1)),
                                                 wxPG_DESCRIPTION | wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED);

    bSizer16->Add(m_pgMgrColouring, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_pgPropTrackPreProcessors =
        m_pgMgrColouring->Append(new wxBoolProperty(_("Track PreProcessor blocks"), wxPG_LABEL, 0));
    m_pgPropTrackPreProcessors->SetHelpString(
        _("Track Pre Processor blocks in the code and colour unreachable code with grey text (\"disabled text\")"));

    m_pgPropColourLocalVariables =
        m_pgMgrColouring->Append(new wxBoolProperty(_("Apply context aware colouring"), wxPG_LABEL, 1));
    m_pgPropColourLocalVariables->SetHelpString(
        _("When enabled, gsgs will colour classes, enums, local variables etc\nwith their own custom colour to "
          "diferentiate them from the rest of the code"));

    m_paneTriggering = new wxPanel(m_notebook87, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook87, wxSize(-1, -1)),
                                   wxTAB_TRAVERSAL);
    m_notebook87->AddPage(m_paneTriggering, _("Triggering"), false);

    wxBoxSizer* bSizer17 = new wxBoxSizer(wxVERTICAL);
    m_paneTriggering->SetSizer(bSizer17);

    wxStaticBoxSizer* sbSizer7 =
        new wxStaticBoxSizer(new wxStaticBox(m_paneTriggering, wxID_ANY, _("Automatic Word Completion:")), wxVERTICAL);

    bSizer17->Add(sbSizer7, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxFlexGridSizer* fgSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    fgSizer4->SetFlexibleDirection(wxBOTH);
    fgSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    fgSizer4->AddGrowableCol(1);

    sbSizer7->Add(fgSizer4, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkWordAssist = new wxCheckBox(m_paneTriggering, wxID_ANY, _("Auto display code completion box when typing"),
                                       wxDefaultPosition, wxDLG_UNIT(m_paneTriggering, wxSize(-1, -1)), 0);
    m_checkWordAssist->SetValue(false);
    m_checkWordAssist->SetToolTip(
        _("When enabled, gsgs will auto show the code completion box after N chars were typed"));

    fgSizer4->Add(m_checkWordAssist, 0, wxALL, WXC_FROM_DIP(5));

    fgSizer4->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_staticTextMinWordLen = gsgs_Style().createStaticText(m_paneTriggering, wxID_ANY, _("Minimum chars to type:"),
                                              wxDefaultPosition, wxDLG_UNIT(m_paneTriggering, wxSize(-1, -1)), 0);
    m_staticTextMinWordLen->SetToolTip(_("Minimum chars to type:"));

    fgSizer4->Add(m_staticTextMinWordLen, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_sliderMinWordLen =
        new wxSlider(m_paneTriggering, wxID_ANY, 3, 1, 10, wxDefaultPosition,
                     wxDLG_UNIT(m_paneTriggering, wxSize(-1, -1)), wxSL_LABELS | wxSL_AUTOTICKS | wxSL_HORIZONTAL);
    m_sliderMinWordLen->SetToolTip(_("Number of chars to type before showing the code completion box"));

    fgSizer4->Add(m_sliderMinWordLen, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_checkAutoInsertSingleChoice = new wxCheckBox(m_paneTriggering, wxID_ANY, _("Auto insert single match"),
                                                   wxDefaultPosition, wxDLG_UNIT(m_paneTriggering, wxSize(-1, -1)), 0);
    m_checkAutoInsertSingleChoice->SetValue(true);
    m_checkAutoInsertSingleChoice->SetToolTip(
        _("When there is only a single match don't show the code completion box but rather insert the match"));

    fgSizer4->Add(m_checkAutoInsertSingleChoice, 0, wxALL, WXC_FROM_DIP(5));

    m_pageCtags = new wxPanel(m_notebook87, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook87, wxSize(-1, -1)),
                              wxTAB_TRAVERSAL);
    m_notebook87->AddPage(m_pageCtags, _("CTags"), false);

    wxBoxSizer* boxSizer91 = new wxBoxSizer(wxVERTICAL);
    m_pageCtags->SetSizer(boxSizer91);

    m_notebook93 =
        new wxNotebook(m_pageCtags, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_pageCtags, wxSize(-1, -1)), wxBK_DEFAULT);
    m_notebook93->SetName(wxT("m_notebook93"));

    boxSizer91->Add(m_notebook93, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_panel38 = new wxPanel(m_notebook93, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook93, wxSize(-1, -1)),
                            wxTAB_TRAVERSAL);
    m_notebook93->AddPage(m_panel38, _("Search Paths"), true);

    wxBoxSizer* boxSizer30 = new wxBoxSizer(wxVERTICAL);
    m_panel38->SetSizer(boxSizer30);

    wxBoxSizer* bSizer9 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer30->Add(bSizer9, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_textCtrlCtagsSearchPaths =
        new wxStyledTextCtrl(m_panel38, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel38, wxSize(-1, -1)), 0);
    wxFont m_textCtrlCtagsSearchPathsFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT);
    m_textCtrlCtagsSearchPaths->SetFont(m_textCtrlCtagsSearchPathsFont);
    // Configure the fold margin
    m_textCtrlCtagsSearchPaths->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_textCtrlCtagsSearchPaths->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_textCtrlCtagsSearchPaths->SetMarginSensitive(4, true);
    m_textCtrlCtagsSearchPaths->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_textCtrlCtagsSearchPaths->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_textCtrlCtagsSearchPaths->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_textCtrlCtagsSearchPaths->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_textCtrlCtagsSearchPaths->SetMarginWidth(2, 0);
    m_textCtrlCtagsSearchPaths->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_textCtrlCtagsSearchPaths->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_textCtrlCtagsSearchPaths->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_textCtrlCtagsSearchPaths->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_textCtrlCtagsSearchPaths->SetMarginMask(3, 0);
    m_textCtrlCtagsSearchPaths->SetMarginWidth(3, 0);
    // Select the lexer
    m_textCtrlCtagsSearchPaths->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_textCtrlCtagsSearchPaths->StyleClearAll();
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_textCtrlCtagsSearchPaths->StyleSetFont(i, m_textCtrlCtagsSearchPathsFont);
    }
    m_textCtrlCtagsSearchPaths->SetWrapMode(0);
    m_textCtrlCtagsSearchPaths->SetIndentationGuides(0);
    m_textCtrlCtagsSearchPaths->SetKeyWords(0, wxT(""));
    m_textCtrlCtagsSearchPaths->SetKeyWords(1, wxT(""));
    m_textCtrlCtagsSearchPaths->SetKeyWords(2, wxT(""));
    m_textCtrlCtagsSearchPaths->SetKeyWords(3, wxT(""));
    m_textCtrlCtagsSearchPaths->SetKeyWords(4, wxT(""));

    bSizer9->Add(m_textCtrlCtagsSearchPaths, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer8 = new wxBoxSizer(wxVERTICAL);

    bSizer9->Add(bSizer8, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonAddSearchPath =
        new Button(m_panel38, wxID_ANY, _("&Add..."), wxDefaultPosition, wxDLG_UNIT(m_panel38, wxSize(-1, -1)), 0);

    bSizer8->Add(m_buttonAddSearchPath, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonCtagsSuggest =
        new Button(m_panel38, wxID_ANY, _("Suggest"), wxDefaultPosition, wxDLG_UNIT(m_panel38, wxSize(-1, -1)), 0);
    m_buttonCtagsSuggest->SetToolTip(_("Suggest search paths based on the installed compilers"));

    bSizer8->Add(m_buttonCtagsSuggest, 0, wxALL, WXC_FROM_DIP(5));

    m_panel40 = new wxPanel(m_notebook93, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook93, wxSize(-1, -1)),
                            wxTAB_TRAVERSAL);
    m_notebook93->AddPage(m_panel40, _("Exclude Paths"), false);

    wxBoxSizer* boxSizer42 = new wxBoxSizer(wxVERTICAL);
    m_panel40->SetSizer(boxSizer42);

    wxBoxSizer* bSizer91 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer42->Add(bSizer91, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_textCtrlCtagsExcludePaths =
        new wxStyledTextCtrl(m_panel40, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel40, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_textCtrlCtagsExcludePaths->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_textCtrlCtagsExcludePaths->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_textCtrlCtagsExcludePaths->SetMarginSensitive(4, true);
    m_textCtrlCtagsExcludePaths->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_textCtrlCtagsExcludePaths->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_textCtrlCtagsExcludePaths->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_textCtrlCtagsExcludePaths->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_textCtrlCtagsExcludePaths->SetMarginWidth(2, 0);
    m_textCtrlCtagsExcludePaths->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_textCtrlCtagsExcludePaths->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_textCtrlCtagsExcludePaths->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_textCtrlCtagsExcludePaths->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_textCtrlCtagsExcludePaths->SetMarginMask(3, 0);
    m_textCtrlCtagsExcludePaths->SetMarginWidth(3, 0);
    // Select the lexer
    m_textCtrlCtagsExcludePaths->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_textCtrlCtagsExcludePaths->StyleClearAll();
    m_textCtrlCtagsExcludePaths->SetWrapMode(0);
    m_textCtrlCtagsExcludePaths->SetIndentationGuides(0);
    m_textCtrlCtagsExcludePaths->SetKeyWords(0, wxT(""));
    m_textCtrlCtagsExcludePaths->SetKeyWords(1, wxT(""));
    m_textCtrlCtagsExcludePaths->SetKeyWords(2, wxT(""));
    m_textCtrlCtagsExcludePaths->SetKeyWords(3, wxT(""));
    m_textCtrlCtagsExcludePaths->SetKeyWords(4, wxT(""));

    bSizer91->Add(m_textCtrlCtagsExcludePaths, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer81 = new wxBoxSizer(wxVERTICAL);

    bSizer91->Add(bSizer81, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonAddExcludePath =
        new Button(m_panel40, wxID_ANY, _("&Add..."), wxDefaultPosition, wxDLG_UNIT(m_panel40, wxSize(-1, -1)), 0);

    bSizer81->Add(m_buttonAddExcludePath, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_panelCtagsAdvanced = new wxPanel(m_notebook93, wxID_ANY, wxDefaultPosition,
                                       wxDLG_UNIT(m_notebook93, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebook93->AddPage(m_panelCtagsAdvanced, _("Advanced"), false);

    wxBoxSizer* bSizer6 = new wxBoxSizer(wxVERTICAL);
    m_panelCtagsAdvanced->SetSizer(bSizer6);

    m_notebook2 = new wxNotebook(m_panelCtagsAdvanced, wxID_ANY, wxDefaultPosition,
                                 wxDLG_UNIT(m_panelCtagsAdvanced, wxSize(-1, -1)), 0);
    m_notebook2->SetName(wxT("m_notebook2"));

    bSizer6->Add(m_notebook2, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_panelClangGeneral =
        new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_panelClangGeneral->SetToolTip(_("List here list of tokens to be pre-processed by gsgs-indexer. \nUsually, "
                                      "you would like to add here macros which confuse the parser\nClick the below "
                                      "link to read more about this feature and the syntax supported.\n"));
    m_notebook2->AddPage(m_panelClangGeneral, _("Tokens"), false);

    wxBoxSizer* bSizer5 = new wxBoxSizer(wxVERTICAL);
    m_panelClangGeneral->SetSizer(bSizer5);

    m_textPrep = new wxStyledTextCtrl(m_panelClangGeneral, wxID_ANY, wxDefaultPosition,
                                      wxDLG_UNIT(m_panelClangGeneral, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_textPrep->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_textPrep->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_textPrep->SetMarginSensitive(4, true);
    m_textPrep->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_textPrep->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_textPrep->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_textPrep->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_textPrep->SetMarginWidth(2, 0);
    m_textPrep->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_textPrep->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_textPrep->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_textPrep->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_textPrep->SetMarginMask(3, 0);
    m_textPrep->SetMarginWidth(3, 0);
    // Select the lexer
    m_textPrep->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_textPrep->StyleClearAll();
    m_textPrep->SetWrapMode(0);
    m_textPrep->SetIndentationGuides(0);
    m_textPrep->SetKeyWords(0, wxT(""));
    m_textPrep->SetKeyWords(1, wxT(""));
    m_textPrep->SetKeyWords(2, wxT(""));
    m_textPrep->SetKeyWords(3, wxT(""));
    m_textPrep->SetKeyWords(4, wxT(""));

    bSizer5->Add(m_textPrep, 1, wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer130 = new wxBoxSizer(wxVERTICAL);

    bSizer5->Add(boxSizer130, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText9 = gsgs_Style().createStaticText(m_panelClangGeneral, wxID_ANY,
                                     _("List here list of tokens to be pre-processed by gsgs-indexer usually, you "
                                       "would like to add here\nmacros which confuse the parser"),
                                     wxDefaultPosition, wxDLG_UNIT(m_panelClangGeneral, wxSize(-1, -1)), 0);
    m_staticText9->SetToolTip(_("List here list of tokens to be pre-processed by gsgs-indexer. \nUsually, you "
                                "would like to add here macros which confuse the parser\nClick the below link to read "
                                "more about this feature and the syntax supported.\n"));

    boxSizer130->Add(m_staticText9, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_hyperlink1 = new wxHyperlinkCtrl(m_panelClangGeneral, wxID_ANY, _("Macros Handling"),
                                       wxT("http://niiengine.com"), wxDefaultPosition,
                                       wxDLG_UNIT(m_panelClangGeneral, wxSize(-1, -1)), wxHL_DEFAULT_STYLE);

    boxSizer130->Add(m_hyperlink1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer131 = new wxBoxSizer(wxHORIZONTAL);

    bSizer5->Add(bSizer131, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_panel5 =
        new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebook2->AddPage(m_panel5, _("Types"), false);

    wxBoxSizer* bSizer13 = new wxBoxSizer(wxVERTICAL);
    m_panel5->SetSizer(bSizer13);

    wxStaticBoxSizer* sbSizer521 = new wxStaticBoxSizer(new wxStaticBox(m_panel5, wxID_ANY, wxT("")), wxVERTICAL);

    bSizer13->Add(sbSizer521, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText91 = gsgs_Style().createStaticText(m_panel5, wxID_ANY,
                                      _("Specify here a list of types which are to be specially handled while parsing "
                                        "C and C++\nsource files in the format of TYPE1=TYPE2. So when TYPE1 is found, "
                                        "gsgs will offer\ncompletion as if it was TYPE2 was found"),
                                      wxDefaultPosition, wxDLG_UNIT(m_panel5, wxSize(-1, -1)), 0);

    sbSizer521->Add(m_staticText91, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_textTypes = new wxStyledTextCtrl(m_panel5, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel5, wxSize(-1, -1)), 0);
    // Configure the fold margin
    m_textTypes->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_textTypes->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_textTypes->SetMarginSensitive(4, true);
    m_textTypes->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_textTypes->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_textTypes->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_textTypes->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_textTypes->SetMarginWidth(2, 0);
    m_textTypes->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_textTypes->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_textTypes->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_textTypes->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_textTypes->SetMarginMask(3, 0);
    m_textTypes->SetMarginWidth(3, 0);
    // Select the lexer
    m_textTypes->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_textTypes->StyleClearAll();
    m_textTypes->SetWrapMode(0);
    m_textTypes->SetIndentationGuides(0);
    m_textTypes->SetKeyWords(0, wxT(""));
    m_textTypes->SetKeyWords(1, wxT(""));
    m_textTypes->SetKeyWords(2, wxT(""));
    m_textTypes->SetKeyWords(3, wxT(""));
    m_textTypes->SetKeyWords(4, wxT(""));

    bSizer13->Add(m_textTypes, 1, wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer14 = new wxBoxSizer(wxVERTICAL);

    bSizer6->Add(bSizer14, 0, wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer15 = new wxBoxSizer(wxHORIZONTAL);

    bSizer14->Add(bSizer15, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_textCtrlFilesList = gsgs_Style().createTextCtrl(m_panelCtagsAdvanced, wxID_ANY, wxT(""), wxDefaultPosition,
                                         wxDLG_UNIT(m_panelCtagsAdvanced, wxSize(-1, -1)), 0);
    m_textCtrlFilesList->SetToolTip(_("gsgs can suggest a list of 'Tokens' that will be added to the 'Tokens' "
                                      "table based on parsing the following header files"));

    bSizer15->Add(m_textCtrlFilesList, 1, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_buttonParse = new Button(m_panelCtagsAdvanced, wxID_ANY, _("Parse!"), wxDefaultPosition,
                                 wxDLG_UNIT(m_panelCtagsAdvanced, wxSize(-1, -1)), 0);

    bSizer15->Add(m_buttonParse, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer102 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer100->Add(boxSizer102, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button104 = new Button(this, wxID_OK, _("OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button104->SetDefault();

    boxSizer102->Add(m_button104, 0, wxALL, WXC_FROM_DIP(5));

    m_button106 = new Button(this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    boxSizer102->Add(m_button106, 0, wxALL, WXC_FROM_DIP(5));

    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_pgMgrColouring->Connect(
        wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CodeCompletionSettingsDialog::OnColouringPropertyValueChanged), NULL, this);
    m_staticTextMinWordLen->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CodeCompletionSettingsDialog::OnAutoShowWordAssitUI),
                                    NULL, this);
    m_sliderMinWordLen->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CodeCompletionSettingsDialog::OnAutoShowWordAssitUI),
                                NULL, this);
    m_buttonAddSearchPath->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                                   wxCommandEventHandler(CodeCompletionSettingsDialog::OnAddSearchPath), NULL, this);
    m_buttonCtagsSuggest->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                                  wxCommandEventHandler(CodeCompletionSettingsDialog::OnSuggestCtags), NULL, this);
    m_buttonAddExcludePath->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                                    wxCommandEventHandler(CodeCompletionSettingsDialog::OnAddExcludePath), NULL, this);
    m_buttonParse->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeCompletionSettingsDialog::OnParse), NULL,
                           this);
    m_buttonParse->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CodeCompletionSettingsDialog::OnFileSelectedUI), NULL, this);
    m_button104->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeCompletionSettingsDialog::OnButtonOk), NULL,
                         this);
    m_button106->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeCompletionSettingsDialog::OnButtonCancel), NULL,
                         this);
                         
    DoSetEditEventsHandler(this);
    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_textCtrlCtagsExcludePaths);
        lexer->Apply(m_textCtrlCtagsSearchPaths);
        lexer->Apply(m_textPrep);
        lexer->Apply(m_textTypes);
    }

    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrColouring->GetGrid());
    // Set default values

    //------------------------------------------------------------------
    // Display and behavior
    //------------------------------------------------------------------
    m_checkDisplayFunctionTip->SetValue(m_data.GetFlags() & CC_DISP_FUNC_CALLTIP ? true : false);
    m_checkDisplayTypeInfo->SetValue(m_data.GetFlags() & CC_DISP_TYPE_INFO ? true : false);
    m_checkCppKeywordAssist->SetValue(m_data.GetFlags() & CC_CPP_KEYWORD_ASISST ? true : false);
    m_checkDisableParseOnSave->SetValue(m_data.GetFlags() & CC_DISABLE_AUTO_PARSING ? true : false);
    m_checkBoxretagWorkspaceOnStartup->SetValue(m_data.GetFlags() & CC_RETAG_WORKSPACE_ON_STARTUP ? true : false);
    m_checkBoxDeepUsingNamespaceResolving->SetValue(m_data.GetFlags() & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING ? true
                                                                                                               : false);
    m_checkBoxEnableCaseSensitiveCompletion->SetValue(m_data.GetFlags() & CC_IS_CASE_SENSITIVE ? true : false);
    m_checkBoxKeepFunctionSignature->SetValue(m_data.GetFlags() & CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED);
    m_spinCtrlNumberOfCCItems->ChangeValue(StringUtils::wxIntToString(m_data.GetCcNumberOfDisplayItems()));
    m_textCtrlFileSpec->ChangeValue(m_data.GetFileSpec());

    //------------------------------------------------------------------
    // Colouring
    //------------------------------------------------------------------
    m_pgPropColourLocalVariables->SetValue(m_data.GetFlags() & CC_COLOUR_VARS ? true : false);
    m_pgPropTrackPreProcessors->SetValue(m_data.GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS ? true : false);

    //----------------------------------------------------
    // Triggering
    //----------------------------------------------------
    m_checkWordAssist->SetValue(m_data.GetFlags() & CC_WORD_ASSIST ? true : false);
    m_checkAutoInsertSingleChoice->SetValue(m_data.GetFlags() & CC_AUTO_INSERT_SINGLE_CHOICE ? true : false);
    m_sliderMinWordLen->SetValue(m_data.GetMinWordLen());

    //----------------------------------------------------
    // CTags search paths
    //----------------------------------------------------
    m_textCtrlCtagsSearchPaths->SetValue(wxImplode(m_data.GetParserSearchPaths(), wxT("\n")));
    m_textCtrlCtagsExcludePaths->SetValue(wxImplode(m_data.GetParserExcludePaths(), wxT("\n")));

    //----------------------------------------------------
    // CTags advanced page
    //----------------------------------------------------
    m_textPrep->SetValue(m_data.GetTokens());
    m_textTypes->SetValue(m_data.GetTypes());
    m_textCtrlFilesList->SetValue(m_data.GetMacrosFiles());
}
//-------------------------------------------------------------------------
CodeCompletionSettingsDialog::~CodeCompletionSettingsDialog() 
{
    m_pgMgrColouring->Disconnect(
        wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CodeCompletionSettingsDialog::OnColouringPropertyValueChanged), NULL, this);
    m_staticTextMinWordLen->Disconnect(wxEVT_UPDATE_UI,
                                       wxUpdateUIEventHandler(CodeCompletionSettingsDialog::OnAutoShowWordAssitUI), NULL, this);
    m_sliderMinWordLen->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CodeCompletionSettingsDialog::OnAutoShowWordAssitUI),
                                   NULL, this);
    m_buttonAddSearchPath->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                      wxCommandEventHandler(CodeCompletionSettingsDialog::OnAddSearchPath), NULL, this);
    m_buttonCtagsSuggest->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                     wxCommandEventHandler(CodeCompletionSettingsDialog::OnSuggestCtags), NULL, this);
    m_buttonAddExcludePath->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                       wxCommandEventHandler(CodeCompletionSettingsDialog::OnAddExcludePath), NULL, this);
    m_buttonParse->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeCompletionSettingsDialog::OnParse), NULL,
                              this);
    m_buttonParse->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CodeCompletionSettingsDialog::OnFileSelectedUI), NULL,
                              this);
    m_button104->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeCompletionSettingsDialog::OnButtonOk), NULL,
                            this);
    m_button106->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeCompletionSettingsDialog::OnButtonCancel),
                            NULL, this);
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnButtonOk(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CopyData();
    EndModal(wxID_OK);
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::CopyData()
{
    //----------------------------------------------------
    // Display and behavior
    //----------------------------------------------------
    SetFlag(CC_DISP_FUNC_CALLTIP, m_checkDisplayFunctionTip->IsChecked());
    SetFlag(CC_DISP_TYPE_INFO, m_checkDisplayTypeInfo->IsChecked());
    SetFlag(CC_CPP_KEYWORD_ASISST, m_checkCppKeywordAssist->IsChecked());
    SetFlag(CC_RETAG_WORKSPACE_ON_STARTUP, m_checkBoxretagWorkspaceOnStartup->IsChecked());
    SetFlag(CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING, m_checkBoxDeepUsingNamespaceResolving->IsChecked());
    SetFlag(CC_DISABLE_AUTO_PARSING, m_checkDisableParseOnSave->IsChecked());
    SetFlag(CC_IS_CASE_SENSITIVE, m_checkBoxEnableCaseSensitiveCompletion->IsChecked());
    SetFlag(CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED, m_checkBoxKeepFunctionSignature->IsChecked());
    m_data.SetCcNumberOfDisplayItems(StringUtils::wxStringToInt(m_spinCtrlNumberOfCCItems->GetValue(), 100));
    m_data.SetFileSpec(m_textCtrlFileSpec->GetValue());

    //----------------------------------------------------
    // Colouring
    //----------------------------------------------------
    // colouring is updated on the fly. See OnColouringPropertyValueChanged() function below

    //----------------------------------------------------
    // Triggering
    //----------------------------------------------------
    SetFlag(CC_AUTO_INSERT_SINGLE_CHOICE, m_checkAutoInsertSingleChoice->IsChecked());
    SetFlag(CC_WORD_ASSIST, m_checkWordAssist->IsChecked());
    m_data.SetMinWordLen(m_sliderMinWordLen->GetValue());

    //----------------------------------------------------
    // CTags search paths
    //----------------------------------------------------
    m_data.SetParserSearchPaths(
        ::wxStringTokenize(m_textCtrlCtagsSearchPaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK));
    m_data.SetParserExcludePaths(
        ::wxStringTokenize(m_textCtrlCtagsExcludePaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK));

    //----------------------------------------------------
    // CTags advanced paths
    //----------------------------------------------------
    m_data.SetTokens(m_textPrep->GetValue());
    m_data.SetTypes(m_textTypes->GetValue());
    m_data.SetMacrosFiles(m_textCtrlFilesList->GetValue());
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::SetFlag(CodeCompletionOpts flag, bool set)
{
    if(set) {
        m_data.SetFlags(m_data.GetFlags() | flag);
    } else {
        m_data.SetFlags(m_data.GetFlags() & ~(flag));
    }
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::SetColouringFlag(CodeCompletionColourOpts flag, bool set)
{
    if(set) {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() | flag);
    } else {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() & ~(flag));
    }
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::Parse()
{
    // Prepate list of files to work on
    wxArrayString files = wxStringTokenize(m_textCtrlFilesList->GetValue(), wxT(" \t"), wxTOKEN_STRTOK);
    wxArrayString searchPaths = GetCTagsSearchPaths();
    wxArrayString fullpathsArr;

    for(size_t i = 0; i < files.size(); i++) {
        wxString file = files[i].Trim().Trim(false);
        if(file.IsEmpty()) continue;

        for(size_t xx = 0; xx < searchPaths.size(); xx++) {
            wxString fullpath;
            fullpath << searchPaths.Item(xx) << gsgsPathSep << file;
            wxFileName fn(fullpath);
            if(fn.FileExists()) {
                fullpathsArr.Add(fn.GetFullPath());
                break;
            }
        }
    }

    // Clear the PreProcessor table
    PPTable::Instance()->Clear();
    for(size_t i = 0; i < fullpathsArr.size(); i++)
        PPScan(fullpathsArr.Item(i), true);

    // Open an editor and print out the results
    gsgs::IEditor* editor = PluginManager::Get()->NewEditor();
    if(editor) {
        editor->AppendText(PPTable::Instance()->Export());
        CopyData();
        EndModal(wxID_OK);
    }
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnAddExcludePath(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString new_path =
        wxDirSelector(_("Add ctags Parser Exclude Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        wxString currPathsStr = m_textCtrlCtagsExcludePaths->GetValue();
        wxArrayString currPaths = wxStringTokenize(currPathsStr, wxT("\n\r"), wxTOKEN_STRTOK);
        if(currPaths.Index(new_path) == wxNOT_FOUND) {
            currPathsStr.Trim().Trim(false);
            if(currPathsStr.IsEmpty() == false) { currPathsStr << wxT("\n"); }
            currPathsStr << new_path;
            m_textCtrlCtagsExcludePaths->ChangeValue(currPathsStr);
        }
    }
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnAddSearchPath(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString new_path =
        wxDirSelector(_("Add ctags Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        wxString currPathsStr = m_textCtrlCtagsSearchPaths->GetValue();
        wxArrayString currPaths = wxStringTokenize(currPathsStr, wxT("\n\r"), wxTOKEN_STRTOK);
        if(currPaths.Index(new_path) == wxNOT_FOUND) {

            currPathsStr.Trim().Trim(false);
            if(currPathsStr.IsEmpty() == false) { currPathsStr << wxT("\n"); }
            currPathsStr << new_path;

            m_textCtrlCtagsSearchPaths->ChangeValue(currPathsStr);
        }
    }
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnAutoShowWordAssitUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkWordAssist->IsChecked());
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnFileSelectedUI(wxUpdateUIEvent& event)
{
    event.Enable(m_textCtrlFilesList->GetValue().IsEmpty() == false);
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnParse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Parse();
}
//-------------------------------------------------------------------------
wxArrayString CodeCompletionSettingsDialog::GetCTagsSearchPaths() const
{
    return wxStringTokenize(m_textCtrlCtagsSearchPaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK);
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnSuggestCtags(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSuggest(m_textCtrlCtagsSearchPaths);
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::DoSuggest(wxStyledTextCtrl* textCtrl)
{
    CompilerPtrVec_t allCompilers = gsgs_Settings().GetAllCompilers();

    // We only support auto retrieval of compilers from the GCC family
    wxArrayString compilerNames;
    std::for_each(allCompilers.begin(), allCompilers.end(), [&](CompilerPtr c) {
        if(c->GetCompilerFamily() == COMPILER_FAMILY_CLANG || c->GetCompilerFamily() == COMPILER_FAMILY_MINGW ||
           c->GetCompilerFamily() == COMPILER_FAMILY_GCC) {
            compilerNames.Add(c->GetName());
        } else if(::IsCygwinEnv() && c->GetCompilerFamily() == COMPILER_FAMILY_CYGWIN) {
            compilerNames.Add(c->GetName());
        }
    });

    wxString selection;
    if(compilerNames.size() > 1) {
        WindowList compilerPanels;
        compilerPanels.resize(compilerNames.size(), 0);
        // we have more than one compiler defined, ask the user which one to use
        gsgs::SingleChoiceDialog dlg(this);
        dlg.initItem(compilerNames, compilerPanels, 0);
        dlg.SetTitle(_("Select the compiler to use:"));

        if(dlg.ShowModal() != wxID_OK) return;
        selection = dlg.GetSelection();
    } else if(compilerNames.size() == 1) {
        selection = compilerNames.Item(0);
    }

    if(selection.IsEmpty()) return;

    CompilerPtr comp = gsgs_Settings().GetCompiler(selection);
    CHECK_PTR_RET(comp);

    wxArrayString paths;
    paths = comp->GetDefaultIncludePaths();

    wxString suggestedPaths;
    for(size_t i = 0; i < paths.GetCount(); i++) {
        suggestedPaths << paths.Item(i) << wxT("\n");
    }

    suggestedPaths.Trim().Trim(false);
    if(!suggestedPaths.IsEmpty()) {
        if(::wxMessageBox(_("Accepting this suggestion will replace your old search paths with these paths\nContinue?"),
                          "gsgs", wxYES_NO | wxYES_DEFAULT | wxCANCEL | wxICON_QUESTION) != wxYES) {
            return;
        }
        textCtrl->Clear();
        textCtrl->ChangeValue(suggestedPaths);
    }
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnColouringPropertyValueChanged(wxPropertyGridEvent& event)
{
    // Enable the local variables colouring feature
    SetFlag(CC_COLOUR_VARS, m_pgPropColourLocalVariables->GetValue().GetBool());

    // Enable pre processor tracking (must come after we set the flags above)
    SetColouringFlag(CC_COLOUR_MACRO_BLOCKS, m_pgPropTrackPreProcessors->GetValue().GetBool());
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::OnButtonCancel(wxCommandEvent& event)
{
    event.Skip();
    EndModal(wxID_CANCEL);
}
//-------------------------------------------------------------------------
void CodeCompletionSettingsDialog::DoSetEditEventsHandler(wxWindow* win)
{
    // wxTextCtrl needs some extra special handling
    if(dynamic_cast<wxStyledTextCtrl*>(win)) {
        gsgs::EditEventsHandler::Ptr_t handler(new gsgs::EditEventsHandler(dynamic_cast<wxStyledTextCtrl*>(win)));
        m_handlers.push_back(handler);
    }

    // Check the children
    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        wxWindow* pclChild = pclNode->GetData();
        this->DoSetEditEventsHandler(pclChild);
        pclNode = pclNode->GetNext();
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// CodeCompletionPage
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CodeCompletionPage::CodeCompletionPage(wxWindow* parent, int type, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
    , m_type(type)
    , m_ccChanged(false)
{
    wxBoxSizer* bSizer9 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer9);
    
    m_splitter1 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSP_LIVE_UPDATE|wxSP_NO_XP_THEME|wxSP_3DSASH);
    m_splitter1->SetSashGravity(0.5);
    m_splitter1->SetMinimumPaneSize(1);
    
    bSizer9->Add(m_splitter1, 1, wxEXPAND, 5);
    
    m_panel8 = new wxPanel(m_splitter1, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
    
    wxBoxSizer* bSizer24 = new wxBoxSizer(wxVERTICAL);
    m_panel8->SetSizer(bSizer24);
    
    m_staticText5 = gsgs_Style().createStaticText(m_panel8, wxID_ANY, _("Search paths:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer24->Add(m_staticText5, 0, wxLEFT|wxRIGHT|wxTOP, 5);
    
    m_textCtrlSearchPaths = gsgs_Style().createTextCtrl(m_panel8, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), wxTE_RICH2|wxTE_PROCESS_TAB|wxTE_MULTILINE);
    #ifdef __WXMSW__
    // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to wxFONTFAMILY_TELETYPE
    wxFont m_textCtrlSearchPathsFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_textCtrlSearchPathsFont.SetFamily(wxFONTFAMILY_TELETYPE);
    #else
    wxFont m_textCtrlSearchPathsFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    m_textCtrlSearchPathsFont.SetFamily(wxFONTFAMILY_TELETYPE);
    #endif
    m_textCtrlSearchPaths->SetFont(m_textCtrlSearchPathsFont);
    m_textCtrlSearchPaths->SetToolTip(_("Add here search paths used by clang / ctags for locating include files"));
    
    bSizer24->Add(m_textCtrlSearchPaths, 1, wxALL|wxEXPAND, 5);
    
    m_panel6 = new wxPanel(m_splitter1, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
    m_splitter1->SplitHorizontally(m_panel8, m_panel6, 0);
    
    wxBoxSizer* bSizer221 = new wxBoxSizer(wxVERTICAL);
    m_panel6->SetSizer(bSizer221);
    
    m_staticText12 = gsgs_Style().createStaticText(m_panel6, wxID_ANY, _("Macros (clang):"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer221->Add(m_staticText12, 0, wxLEFT|wxRIGHT|wxTOP, 5);
    
    m_textCtrlMacros = gsgs_Style().createTextCtrl(m_panel6, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), wxTE_RICH2|wxTE_PROCESS_TAB|wxTE_MULTILINE);
    #ifdef __WXMSW__
    // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to wxFONTFAMILY_TELETYPE
    wxFont m_textCtrlMacrosFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_textCtrlMacrosFont.SetFamily(wxFONTFAMILY_TELETYPE);
    #else
    wxFont m_textCtrlMacrosFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    m_textCtrlMacrosFont.SetFamily(wxFONTFAMILY_TELETYPE);
    #endif
    m_textCtrlMacros->SetFont(m_textCtrlMacrosFont);
    m_textCtrlMacros->SetToolTip(_("Add here macros to pass to clang when generating PCH files\nOne macro per line"));
    
    bSizer221->Add(m_textCtrlMacros, 1, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* boxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer221->Add(boxSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_checkBoxCpp11 = new wxCheckBox(m_panel6, wxID_ANY, _("Enable C++11 Standard"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_checkBoxCpp11->SetValue(false);
    
    boxSizer3->Add(m_checkBoxCpp11, 0, wxALL, 5);
    
    m_checkBoxCpp14 = new wxCheckBox(m_panel6, wxID_ANY, _("Enable C++14 Standard"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_checkBoxCpp14->SetValue(false);
    
    boxSizer3->Add(m_checkBoxCpp14, 0, wxALL, 5);
	
	m_checkBoxCpp17 = new wxCheckBox(m_panel6, wxID_ANY, _("Enable C++17 Standard"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_checkBoxCpp17->SetValue(false);
    
    boxSizer3->Add(m_checkBoxCpp17, 0, wxALL, 5);
    
    m_checkBoxSWTLW = new wxCheckBox(m_panel6, wxID_ANY, _("Sync to Workspace File"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_checkBoxSWTLW->SetValue(false);
    m_checkBoxSWTLW->SetToolTip(_("When enabled search paths folders for Code Completion will be synced between the Workspace file and the local search paths database."));
    
    boxSizer3->Add(m_checkBoxSWTLW, 0, wxALL, 5);

    // Connect events
    m_textCtrlSearchPaths->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CodeCompletionPage::OnCCContentModified), NULL, this);
    m_textCtrlMacros->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CodeCompletionPage::OnCCContentModified), NULL, this);
    
    if(m_type == TypeWorkspace) {

        wxArrayString excludePaths, includePaths;
        wxString opts, c_opts;
        wxString macros;

        LocalWorkspace* lw = gsgs_CppWorker().GetLocalWorkspace();
        lw->GetParserPaths(includePaths, excludePaths);
        lw->GetParserMacros(macros);

        m_textCtrlSearchPaths->SetValue(wxImplode(includePaths, wxT("\n")));
        m_textCtrlMacros->SetValue(macros);

        m_checkBoxCpp11->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableCpp11);
        m_checkBoxCpp14->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableCpp14);
        m_checkBoxCpp17->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableCpp17);
        m_checkBoxSWTLW->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableSWTLW);
    }
}
//-------------------------------------------------------------------------
CodeCompletionPage::~CodeCompletionPage() 
{
        m_textCtrlSearchPaths->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CodeCompletionPage::OnCCContentModified), NULL, this);
    m_textCtrlMacros->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CodeCompletionPage::OnCCContentModified), NULL, this);
}
//-------------------------------------------------------------------------
wxArrayString CodeCompletionPage::GetIncludePaths() const
{
    return wxStringTokenize(m_textCtrlSearchPaths->GetValue(), wxT("\n\r"), wxTOKEN_STRTOK);
}
//-------------------------------------------------------------------------
wxString CodeCompletionPage::GetMacros() const { return m_textCtrlMacros->GetValue(); }
//-------------------------------------------------------------------------
wxString CodeCompletionPage::GetIncludePathsAsString() const { return m_textCtrlSearchPaths->GetValue(); }
//-------------------------------------------------------------------------
void CodeCompletionPage::Save()
{
    if(m_type == TypeWorkspace) {
        size_t flags = 0;
        LocalWorkspace* lw = gsgs_CppWorker().GetLocalWorkspace();
        lw->SetParserPaths(GetIncludePaths(), wxArrayString());
        lw->SetParserMacros(GetMacros());

        if(m_checkBoxCpp11->IsChecked()) flags |= LocalWorkspace::EnableCpp11;
        if(m_checkBoxCpp14->IsChecked()) flags |= LocalWorkspace::EnableCpp14;
        if(m_checkBoxCpp17->IsChecked()) flags |= LocalWorkspace::EnableCpp17;
        if(m_checkBoxSWTLW->IsChecked()) flags |= LocalWorkspace::EnableSWTLW;
        lw->SetParserFlags(flags);
        lw->Flush();
    }
}
//-------------------------------------------------------------------------
void CodeCompletionPage::OnCCContentModified(wxCommandEvent& event) { m_ccChanged = true; }
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// WorkspaceSettingsDlg
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
WorkspaceSettingsDlg::WorkspaceSettingsDlg(wxWindow* parent, gsgs::LocalWorkspace* localWorkspace, wxWindowID id, 
    const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_localWorkspace(localWorkspace)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);
    
    m_notebook1 = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), 0);
    m_notebook1->SetName(wxT("m_notebook1"));
    
    mainSizer->Add(m_notebook1, 1, wxALL|wxEXPAND, 5);
    
    m_panelEnv = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
    m_notebook1->AddPage(m_panelEnv, _("Environment"), false);
    
    wxBoxSizer* bSizer81 = new wxBoxSizer(wxVERTICAL);
    m_panelEnv->SetSizer(bSizer81);
    
    wxStaticBoxSizer* sbSizer3 = new wxStaticBoxSizer( new wxStaticBox(m_panelEnv, wxID_ANY, wxT("")), wxVERTICAL);
    
    bSizer81->Add(sbSizer3, 0, wxALL, 5);
    
    m_staticText3 = gsgs_Style().createStaticText(m_panelEnv, wxID_ANY, _("By default, gsgs uses the current active environment variables set as defined in the Settings > Environment Variables dialog.\nHowever, you may choose a different set to become the active set when this workspace is loaded selecting it here."), wxDefaultPosition, wxSize(-1, -1), 0);
    
    sbSizer3->Add(m_staticText3, 0, wxALL, 5);
    
    m_staticText4 = gsgs_Style().createStaticText(m_panelEnv, wxID_ANY, _("Environment sets:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer81->Add(m_staticText4, 0, wxALL|wxEXPAND, 5);
    
    wxArrayString m_choiceEnvSetsArr;
    m_choiceEnvSetsArr.Add(wxT("Default"));
    m_choiceEnvSets = new wxChoice(m_panelEnv, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_choiceEnvSetsArr, 0);
    m_choiceEnvSets->SetSelection(0);
    
    bSizer81->Add(m_choiceEnvSets, 0, wxALL|wxEXPAND, 5);
    
    m_staticline2 = new wxStaticLine(m_panelEnv, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLI_HORIZONTAL);
    
    bSizer81->Add(m_staticline2, 0, wxALL|wxEXPAND, 5);
    
    m_staticText6 = gsgs_Style().createStaticText(m_panelEnv, wxID_ANY, _("Specify here an additional environment variables that will be shared with other people who are using this workspace:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer81->Add(m_staticText6, 0, wxALL|wxEXPAND, 5);
    
    m_textCtrlWspEnvVars = gsgs_Style().createTextCtrl(m_panelEnv, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), wxTE_RICH2|wxTE_PROCESS_TAB|wxTE_MULTILINE);
    #ifdef __WXMSW__
    // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to wxFONTFAMILY_TELETYPE
    wxFont m_textCtrlWspEnvVarsFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_textCtrlWspEnvVarsFont.SetFamily(wxFONTFAMILY_TELETYPE);
    #else
    wxFont m_textCtrlWspEnvVarsFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    m_textCtrlWspEnvVarsFont.SetFamily(wxFONTFAMILY_TELETYPE);
    #endif
    m_textCtrlWspEnvVars->SetFont(m_textCtrlWspEnvVarsFont);
    
    bSizer81->Add(m_textCtrlWspEnvVars, 1, wxALL|wxEXPAND, 5);
    
    m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLI_HORIZONTAL);
    
    mainSizer->Add(m_staticline1, 0, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_buttonOk = new Button(this, wxID_OK, _("&Ok"), wxDefaultPosition, wxSize(-1, -1), 0);
    m_buttonOk->SetDefault();
    
    buttonSizer->Add(m_buttonOk, 0, wxALL, 5);
    
    m_buttonCancel = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    buttonSizer->Add(m_buttonCancel, 0, wxALL, 5);
    
    
    #if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(m_notebook1)){
        wxPersistenceManager::Get().RegisterAndRestore(m_notebook1);
    } else {
        wxPersistenceManager::Get().Restore(m_notebook1);
    }
    #endif
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_choiceEnvSets->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(WorkspaceSettingsDlg::OnEnvSelected), NULL, this);
    m_buttonOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WorkspaceSettingsDlg::OnButtonOK), NULL, this);
   
    m_ccPage = new CodeCompletionPage(m_notebook1, CodeCompletionPage::TypeWorkspace);
    m_notebook1->AddPage(m_ccPage, wxT("Code Completion"), false);

    EvnVarList vars;
    gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);
    const gsgsStringMap& envSets = vars.GetEnvVarSets();
    wxString activePage = vars.GetActiveSet();
    m_choiceEnvSets->Clear();

    gsgsStringMap::const_iterator iter = envSets.begin();
    int useActiveSetIndex = m_choiceEnvSets->Append(wxGetTranslation(USE_GLOBAL_SETTINGS));

    for(; iter != envSets.end(); iter++) {
        m_choiceEnvSets->Append(iter->first);
    }

    // select the current workspace active set name
    wxString activeEnvSet;
    wxString tmpSet = localWorkspace->GetActiveEnvironmentSet();

    if(tmpSet == _("<Use Active Set>")) {
        tmpSet = wxGetTranslation(USE_GLOBAL_SETTINGS);
    }

    int where = m_choiceEnvSets->FindString(tmpSet);
    if(where == wxNOT_FOUND) {
        activeEnvSet = activePage;
        m_choiceEnvSets->SetSelection(useActiveSetIndex);

    } else {
        activeEnvSet = tmpSet;
        m_choiceEnvSets->SetSelection(where);
    }

    if(activeEnvSet.IsEmpty() == false) {
        vars.SetActiveSet(activeEnvSet);
        gsgs_EditorConfig().WriteObject(wxT("Variables"), &vars);
    }

    wxString envvars = gsgs_CppWorker().GetEnvironmentVariabels();
    envvars.Trim().Trim(false);

    m_textCtrlWspEnvVars->SetValue(envvars);
}
//-------------------------------------------------------------------------
WorkspaceSettingsDlg::~WorkspaceSettingsDlg() 
{
        m_choiceEnvSets->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(WorkspaceSettingsDlg::OnEnvSelected), NULL, this);
    m_buttonOk->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WorkspaceSettingsDlg::OnButtonOK), NULL, this);
}
//-------------------------------------------------------------------------
// void WorkspaceSettingsDlg::OnAddIncludePath( wxCommandEvent& event )
//{
//	wxUnusedVar(event);
//	wxString new_path = wxDirSelector(_("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition,
//this);
//	if (new_path.IsEmpty() == false) {
//		wxString curpaths = m_textCtrlIncludePaths->GetValue();
//		curpaths.Trim().Trim(false);
//		if(curpaths.IsEmpty()) {
//			curpaths << wxT("\n");
//		}
//		curpaths << new_path;
//		m_textCtrlIncludePaths->SetValue(curpaths);
//	}
//}
//
// void WorkspaceSettingsDlg::OnAddExcludePath( wxCommandEvent& event )
//{
//	wxUnusedVar(event);
//	wxString new_path = wxDirSelector(_("Add Parser Exclude Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition,
//this);
//	if (new_path.IsEmpty() == false) {
//		wxString curpaths = m_textCtrlExcludePaths->GetValue();
//		curpaths.Trim().Trim(false);
//		if(curpaths.IsEmpty()) {
//			curpaths << wxT("\n");
//		}
//		curpaths << new_path;
//		m_textCtrlExcludePaths->SetValue(curpaths);
//	}
//}
//
//-------------------------------------------------------------------------
wxArrayString WorkspaceSettingsDlg::GetExcludePaths() const { return wxArrayString(); }
//-------------------------------------------------------------------------
wxArrayString WorkspaceSettingsDlg::GetIncludePaths() const { return m_ccPage->GetIncludePaths(); }
//-------------------------------------------------------------------------
void WorkspaceSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
    m_localWorkspace->SetActiveEnvironmentSet(m_choiceEnvSets->GetStringSelection());
    gsgs_CppWorker().SetEnvironmentVariabels(m_textCtrlWspEnvVars->GetValue());
    m_ccPage->Save();
    event.Skip();
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// QuickDebugDlg
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
QuickDebugDlg::QuickDebugDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer1);

    wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);

    bSizer1->Add(bSizer3, 1, wxALL | wxEXPAND, 5);

    wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
    fgSizer1->SetFlexibleDirection(wxBOTH);
    fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    fgSizer1->AddGrowableCol(1);

    bSizer3->Add(fgSizer1, 0, wxALL | wxEXPAND, 5);

    m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select executable:"), wxDefaultPosition, wxSize(-1, -1), 0);

    fgSizer1->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

    wxArrayString m_ExeFilepathArr;
    m_ExeFilepath = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), m_ExeFilepathArr, 0);
    m_ExeFilepath->SetToolTip(_("Enter the filepath to the program that you want to debug.\nAlternatively, if you enter the path below, putting just the filename here will suffice."));
    m_ExeFilepath->SetFocus();
#if wxVERSION_NUMBER >= 3000
    m_ExeFilepath->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_ExeFilepath, 0, wxALL | wxEXPAND, 5);

    m_buttonBrowseExe = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
    m_buttonBrowseExe->SetToolTip(_("Select executale to debug"));

    fgSizer1->Add(m_buttonBrowseExe, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText5 = gsgs_Style().createStaticText(this, wxID_ANY, _("Working directory:"), wxDefaultPosition, wxSize(-1, -1), 0);

    fgSizer1->Add(m_staticText5, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

    wxArrayString m_WDArr;
    m_WD = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), m_WDArr, 0);
    m_WD->SetToolTip(_("Optionally, enter the path where the program that you want to debug can be found"));
#if wxVERSION_NUMBER >= 3000
    m_WD->SetHint(wxT(""));
#endif

    fgSizer1->Add(m_WD, 0, wxALL | wxEXPAND, 5);

    m_buttonBrowseWD = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
    m_buttonBrowseWD->SetToolTip(_("Browse for working directory"));

    fgSizer1->Add(m_buttonBrowseWD, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Program arguments:"), wxDefaultPosition, wxSize(-1, -1), 0);

    fgSizer1->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

    m_textCtrlArgs = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    fgSizer1->Add(m_textCtrlArgs, 0, wxALL | wxEXPAND, 5);

    fgSizer1->Add(0, 0, 1, wxALL, 5);

    m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select debugger:"), wxDefaultPosition, wxSize(-1, -1), 0);

    fgSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

    wxArrayString m_choiceDebuggersArr;
    m_choiceDebuggers = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_choiceDebuggersArr, 0);

    fgSizer1->Add(m_choiceDebuggers, 0, wxALL | wxEXPAND, 5);

    fgSizer1->Add(0, 0, 1, wxALL, 5);

    m_staticText12 = gsgs_Style().createStaticText(this, wxID_ANY, _("Alternate debugger executable:"), wxDefaultPosition, wxSize(-1, -1), 0);

    fgSizer1->Add(m_staticText12, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

    m_textCtrlDebuggerExec = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_textCtrlDebuggerExec->SetToolTip(_("Provide an alternate debugger executable to use.\nThis is currently only supported for GDB"));

    fgSizer1->Add(m_textCtrlDebuggerExec, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

    m_buttonBrowseDebugger = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxSize(-1, -1), wxBU_EXACTFIT);
    m_buttonBrowseDebugger->SetToolTip(_("Select debugger executable"));

    fgSizer1->Add(m_buttonBrowseDebugger, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_staticText6 = gsgs_Style().createStaticText(this, wxID_ANY, _("Startup commands:"), wxDefaultPosition, wxSize(-1, -1), 0);

    bSizer3->Add(m_staticText6, 0, wxALL, 5);

    m_textCtrlCmds = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), wxTE_RICH2 | wxTE_PROCESS_TAB | wxTE_MULTILINE);
#ifdef __WXMSW__
    // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to wxFONTFAMILY_TELETYPE
    wxFont m_textCtrlCmdsFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_textCtrlCmdsFont.SetFamily(wxFONTFAMILY_TELETYPE);
#else
    wxFont m_textCtrlCmdsFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    m_textCtrlCmdsFont.SetFamily(wxFONTFAMILY_TELETYPE);
#endif
    m_textCtrlCmds->SetFont(m_textCtrlCmdsFont);

    bSizer3->Add(m_textCtrlCmds, 1, wxALL | wxEXPAND, 5);
    m_textCtrlCmds->SetMinSize(wxSize(-1, 150));

    m_stdBtnSizer2 = new DialogButtonSizer();

    bSizer1->Add(m_stdBtnSizer2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_buttonOK->SetDefault();
    m_stdBtnSizer2->AddButton(m_buttonOK);

    m_buttonCancel1 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer2->AddButton(m_buttonCancel1);
    m_stdBtnSizer2->Realize();

    if (GetParent()) {
        CentreOnParent(wxBOTH);
    }
    else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_buttonBrowseExe->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonBrowseExe), NULL, this);
    m_buttonBrowseWD->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonBrowseWD), NULL, this);
    m_buttonBrowseDebugger->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnSelectAlternateDebugger), NULL, this);
    m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonDebug), NULL, this);
    m_buttonCancel1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonCancel), NULL, this);

    Initialize();
}
//-------------------------------------------------------------------------
QuickDebugDlg::~QuickDebugDlg() 
{
    m_buttonBrowseExe->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonBrowseExe), NULL, this);
    m_buttonBrowseWD->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonBrowseWD), NULL, this);
    m_buttonBrowseDebugger->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnSelectAlternateDebugger), NULL, this);
    m_buttonOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonDebug), NULL, this);
    m_buttonCancel1->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickDebugDlg::OnButtonCancel), NULL, this);
}
//-------------------------------------------------------------------------
void QuickDebugDlg::Initialize()
{
    QuickDebugInfo info;
    gsgs_EditorConfig().ReadObject(wxT("QuickDebugDlg"), &info);

    m_choiceDebuggers->Append(DebuggerMgr::Get().GetAvailableDebuggers());
    if(m_choiceDebuggers->GetCount()) {
        m_choiceDebuggers->SetSelection(0);
    }
    if(m_choiceDebuggers->GetCount() > (unsigned int)info.GetSelectedDbg()) {
        m_choiceDebuggers->SetSelection(info.GetSelectedDbg());
    }

    m_ExeFilepath->Append(info.GetExeFilepaths());
    if(m_ExeFilepath->GetCount() > 0) {
        m_ExeFilepath->SetSelection(0);
    }

    wxArrayString wds = info.GetWds();
    wxString homeDir = wxStandardPaths::Get().GetUserConfigDir();
    if(wds.Index(homeDir) == wxNOT_FOUND) {
        wds.Add(homeDir);
    }

    m_WD->Append(wds);
    if(m_WD->GetCount() > 0) {
        m_WD->SetSelection(0);
    }
    m_textCtrlArgs->ChangeValue(info.GetArguments());

    wxString startupCmds;
    for(size_t i = 0; i < info.GetStartCmds().GetCount(); i++) {
        startupCmds << info.GetStartCmds().Item(i) << wxT("\n");
    }
    m_textCtrlCmds->ChangeValue(startupCmds);
    m_textCtrlDebuggerExec->ChangeValue(info.GetAlternateDebuggerExec());
}
//-------------------------------------------------------------------------
void QuickDebugDlg::OnButtonBrowseExe(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString path, ans;
    wxFileName fn(GetExe());
    if(fn.FileExists()) {
        // Use the serialised path as the wxFileSelector default path
        path = fn.GetPath();
    } else {
        // Otherwise use any working dir entry, which might just have been altered
        path = wxStandardPaths::Get().GetUserConfigDir();
    }

    ans = wxFileSelector(_("Select file:"), path);
    if(!ans.empty()) {
        m_ExeFilepath->Insert(ans, 0);
        m_ExeFilepath->SetSelection(0);
    }
}
//-------------------------------------------------------------------------
void QuickDebugDlg::OnButtonDebug(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // save values
    const size_t MAX_NO_ITEMS = 10;
    QuickDebugInfo info;
    info.SetSelectedDbg(m_choiceDebuggers->GetSelection());
    info.SetExeFilepaths(ReturnWithStringPrepended(m_ExeFilepath->GetStrings(), GetExe(), MAX_NO_ITEMS));
    info.SetWDs(ReturnWithStringPrepended(m_WD->GetStrings(), GetWorkingDirectory(), MAX_NO_ITEMS));
    info.SetStartCmds(GetStartupCmds());
    info.SetArguments(m_textCtrlArgs->GetValue());
    info.SetAlternateDebuggerExec(m_textCtrlDebuggerExec->GetValue());
    gsgs_EditorConfig().WriteObject(wxT("QuickDebugDlg"), &info);

    EndModal(wxID_OK);
}
//-------------------------------------------------------------------------
void QuickDebugDlg::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);

    EndModal(wxID_CANCEL);
}
//-------------------------------------------------------------------------
wxString QuickDebugDlg::GetArguments() { return m_textCtrlArgs->GetValue(); }
//-------------------------------------------------------------------------
wxString QuickDebugDlg::GetDebuggerName() { return m_choiceDebuggers->GetStringSelection(); }
//-------------------------------------------------------------------------
wxString QuickDebugDlg::GetExe() { return m_ExeFilepath->GetValue(); }
//-------------------------------------------------------------------------
wxArrayString QuickDebugDlg::GetStartupCmds()
{
    wxString cmds = m_textCtrlCmds->GetValue();
    cmds.Trim().Trim(false);

    return wxStringTokenize(cmds, wxT("\n\r"), wxTOKEN_STRTOK);
}
//-------------------------------------------------------------------------
wxString QuickDebugDlg::GetWorkingDirectory() { return m_WD->GetValue(); }
//-------------------------------------------------------------------------
void QuickDebugDlg::OnButtonBrowseWD(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString ans, path(GetWorkingDirectory());
    if(!wxFileName::DirExists(path)) {
        path = wxStandardPaths::Get().GetUserConfigDir();
    }

    ans = wxDirSelector(_("Select working directory:"), path);
    if(!ans.empty()) {
        m_WD->Insert(ans, 0);
        m_WD->SetSelection(0);
    }
}
//-------------------------------------------------------------------------
void QuickDebugDlg::OnSelectAlternateDebugger(wxCommandEvent& event)
{
    wxString debuggerPath = ::wxFileSelector(_("Choose debugger:"), wxStandardPaths::Get().GetUserConfigDir());
    if(debuggerPath.IsEmpty()) return;
    m_textCtrlDebuggerExec->ChangeValue(debuggerPath);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// DisplayVariableDlg
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
static wxRect s_Rect;

class QWTreeData : public wxTreeItemData
{
public:
    VariableObjChild _voc;

    QWTreeData(const VariableObjChild& voc)
        : _voc(voc)
    {
    }
    virtual ~QWTreeData() {}
};
//-------------------------------------------------------------------------
DisplayVariableDlg::DisplayVariableDlg(wxWindow* parent)
    : ResizableTooltip(parent)
    , m_debugger(NULL)
    , m_editDlgIsUp(false)
{
    Hide();
    Centre();
    SetName("DebuggerEditItemDlg");
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_MENU, &DisplayVariableDlg::OnItemMenu, this);
}
//-------------------------------------------------------------------------
DisplayVariableDlg::~DisplayVariableDlg()
{
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_MENU, &DisplayVariableDlg::OnItemMenu, this);
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()) {
        if(m_treeCtrl->ItemHasChildren(item)) {
            wxTreeItemIdValue kookie;
            wxTreeItemId child = m_treeCtrl->GetFirstChild(item, kookie);
            while(child.IsOk()) {
                if(m_treeCtrl->GetItemText(child) == wxT("<dummy>")) {
                    // Dummy node, remove it and ask the debugger for information
                    m_treeCtrl->SetItemText(child, _("Loading..."));

                    QWTreeData* data = (QWTreeData*)m_treeCtrl->GetItemData(item);
                    if(data) {
                        // Ask the debugger for information
                        m_debugger->ListChildren(data->_voc.gdbId, DBG_USERR_QUICKWACTH);
                        m_gdbId2Item[data->_voc.gdbId] = item;
                    }
                    break;
                }
                child = m_treeCtrl->GetNextChild(item, kookie);
            }
        }
    }
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::BuildTree(const VariableObjChildren& children, gsgs::IDebugger* debugger)
{
    m_debugger = debugger;
    m_gdbId2Item.clear();
    m_gdbId2ItemLeaf.clear();
    m_treeCtrl->DeleteAllItems();

    VariableObjChild vob;
    vob.gdbId = m_mainVariableObject;
    vob.isAFake = false;

    wxTreeItemId root = m_treeCtrl->AddRoot(m_variableName, -1, -1, new QWTreeData(vob));

    // Mac does not return value for the root item...
    // we need to force another evaluate call here
#ifdef __WXMAC__
    m_debugger->EvaluateVariableObject(m_mainVariableObject, DBG_USERR_QUICKWACTH);
    m_gdbId2ItemLeaf[m_mainVariableObject] = root;
#endif

    if(children.empty()) return;
    DoAddChildren(root, children);
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::AddItems(const wxString& varname, const VariableObjChildren& children)
{
    std::map<wxString, wxTreeItemId>::iterator iter = m_gdbId2Item.find(varname);
    if(iter != m_gdbId2Item.end()) {
        wxTreeItemId item = iter->second;
        DoAddChildren(item, children);
    }
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::DoAddChildren(wxTreeItemId& item, const VariableObjChildren& children)
{
    if(item.IsOk() == false) return;

    if(m_treeCtrl->GetRootItem() != item && m_treeCtrl->ItemHasChildren(item)) {
        // delete the <dummy> node
        wxTreeItemIdValue kookie;
        wxTreeItemId child = m_treeCtrl->GetFirstChild(item, kookie);
        while(child.IsOk()) {
            wxString itemText = m_treeCtrl->GetItemText(child);
            if(itemText == wxT("<dummy>") || itemText == _("Loading...")) { m_treeCtrl->Delete(child); }
            child = m_treeCtrl->GetNextChild(item, kookie);
        }
    }

    for(size_t i = 0; i < children.size(); i++) {
        VariableObjChild ch = children.at(i);

        // Dont use ch.isAFake here since it will also returns true of inheritance
        if(ch.varName != wxT("public") && ch.varName != wxT("private") && ch.varName != wxT("protected")) {
            // Real node
            wxTreeItemId child = m_treeCtrl->AppendItem(item, ch.varName, -1, -1, new QWTreeData(ch));
            if(ch.numChilds > 0) {
                // add fake node to this item, so it will have the [+] on the side
                m_treeCtrl->AppendItem(child, wxT("<dummy>"));
            }

            // ask gdb for the value for this node

            m_debugger->EvaluateVariableObject(ch.gdbId, DBG_USERR_QUICKWACTH);
            m_gdbId2ItemLeaf[ch.gdbId] = child;

        } else {

            // Fake node, ask for expansion only if this node is visible
            m_debugger->ListChildren(ch.gdbId, DBG_USERR_QUICKWACTH);
            m_gdbId2Item[ch.gdbId] = item;
        }
    }
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::OnBtnCancel(wxCommandEvent& e)
{
    DoCleanUp();
    e.Skip();
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::UpdateValue(const wxString& varname, const wxString& value)
{
    wxTreeItemId nodeId;
    std::map<wxString, wxTreeItemId>::iterator iter = m_gdbId2ItemLeaf.find(varname);
    if(iter != m_gdbId2ItemLeaf.end()) {
        wxTreeItemId item = iter->second;
        if(item.IsOk()) {
            wxString curtext = m_treeCtrl->GetItemText(item);
#ifdef __WXMAC__
            if(item == m_treeCtrl->GetRootItem()) { curtext = curtext.BeforeFirst(wxT('=')); }
#endif
            curtext << wxT(" = ") << value;
            m_treeCtrl->SetItemText(item, curtext);

        } else if(item.IsOk()) {
            nodeId = item;
        }
    } else if(varname == m_mainVariableObject) {

        // Handle Root
        nodeId = m_treeCtrl->GetRootItem();
    }
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::OnCloseEvent(wxCloseEvent& e)
{
    DoCleanUp();
    e.Skip();
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::DoCleanUp()
{
    if(m_debugger && m_mainVariableObject.IsEmpty() == false) {
        m_debugger->DeleteVariableObject(m_mainVariableObject);
    }
    m_gdbId2Item.clear();
    m_gdbId2ItemLeaf.clear();
    m_mainVariableObject = wxT("");
    m_variableName = wxT("");
    m_expression = wxT("");
    m_itemOldValue.Clear();
    m_editDlgIsUp = false;
    wxSetCursor(wxNullCursor);
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::HideDialog()
{
    DoCleanUp();
    // asm("int3");
    ResizableTooltip::Hide();
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::ShowDialog(bool center)
{
    wxUnusedVar(center);
    ResizableTooltip::ShowTip();
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::OnItemMenu(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();

    //if(item.IsOk()) { m_treeCtrl->SelectItem(item); }

    // Dont show popup menu for fake nodes
    if(IsFakeItem(item)) return;

    // Popup the menu
    wxMenu menu;

    menu.Append(XRCID("tip_add_watch"), _("Add Watch"));
    menu.Append(XRCID("tip_copy_value"), _("Copy Value to Clipboard"));
    menu.Append(XRCID("edit_item"), _("Edit..."));

    menu.Connect(XRCID("tip_add_watch"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DisplayVariableDlg::OnMenuSelection), NULL, this);
    menu.Connect(XRCID("tip_copy_value"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DisplayVariableDlg::OnMenuSelection), NULL, this);
    menu.Connect(XRCID("edit_item"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DisplayVariableDlg::OnMenuSelection), NULL, this);

    m_treeCtrl->PopupMenu(&menu);
}
//-------------------------------------------------------------------------
wxString DisplayVariableDlg::DoGetItemPath(const wxTreeItemId& treeItem)
{
    wxString fullpath;
    wxTreeItemId item = treeItem;
    while(item.IsOk()) {
        wxString text = m_treeCtrl->GetItemText(item);
        text = text.BeforeFirst(wxT('='));

#ifdef __WXMAC__
        // Mac puts the type in square brackets, remove them as well
        text = text.BeforeFirst(wxT('['));
#endif
        text.Trim().Trim(false);

        if(item != m_treeCtrl->GetRootItem()) {
            if(IsFakeItem(item) == false) {
                text.Prepend(wxT("."));
                fullpath.Prepend(text);
            }
        } else {
            // Root item
            fullpath.Prepend(text);
        }

        // Are we at root yet?
        if(m_treeCtrl->GetRootItem() == item) break;

        // Surround this expression with parenthesiss
        item = m_treeCtrl->GetItemParent(item);
    }

    wxString exprWithParentheses;
    wxArrayString items = ::wxStringTokenize(fullpath, wxT("."), wxTOKEN_STRTOK);
    for(size_t i = 0; i < items.GetCount(); i++) {
        exprWithParentheses << items.Item(i);
        exprWithParentheses.Prepend(wxT("(")).Append(wxT(")."));
    }

    if(!items.IsEmpty()) { exprWithParentheses.RemoveLast(); }

    return exprWithParentheses;
}
//-------------------------------------------------------------------------
bool DisplayVariableDlg::IsFakeItem(const wxTreeItemId& item)
{
    if(item.IsOk() == false) return true; // fake

    if(item != m_treeCtrl->GetRootItem()) {
        QWTreeData* data = (QWTreeData*)m_treeCtrl->GetItemData(item);
        if(data) return data->_voc.isAFake;

        return false;

    } else {
        return false;
    }
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::OnMenuSelection(wxCommandEvent& e)
{
    wxTreeItemId item = m_treeCtrl->GetSelection();
    if(item.IsOk() && !IsFakeItem(item)) {
        if(e.GetId() == XRCID("tip_add_watch")) {
            wxString fullpath = DoGetItemPath(item);
            gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->AddExpression(fullpath);
            gsgs_Frame()->GetDebuggerPane()->SelectTab(DebuggerPane::WATCHES);
            gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->RefreshValues();

        } else if(e.GetId() == XRCID("tip_copy_value")) {
            wxString itemText = m_treeCtrl->GetItemText(item);
            itemText = itemText.AfterFirst(wxT('='));
            CopyToClipboard(itemText.Trim().Trim(true));

        } else if(e.GetId() == XRCID("edit_item")) {
            DoEditItem(item);
        }
    }
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::OnCreateVariableObjError(const DebuggerEventData& event) 
{ 
    wxUnusedVar(event); 
}
//-------------------------------------------------------------------------
void DisplayVariableDlg::DoEditItem(const wxTreeItemId& item)
{
    if(item.IsOk() == false) return;

    wxString oldText = m_treeCtrl->GetItemText(item);
    oldText = oldText.BeforeFirst(wxT('='));
    oldText.Trim().Trim(false);

#ifdef __WXGTK__
    wxPoint oldPos = ::wxGetMousePosition();
    oldPos = ScreenToClient(oldPos);
#endif

    m_editDlgIsUp = true;
    DebuggerEditItemDlg dlg(this, oldText);
    // We need to Hide() the tip before running the edit dialog, otherwise the dialog is covered by the tip
    // (and can't be entered or cancelled...
    Hide();
    int res = dlg.ShowModal();
    Show();
    m_editDlgIsUp = false;

#ifdef __WXGTK__
    wxWindow::WarpPointer(oldPos.x, oldPos.y);
#endif

    if(res != wxID_OK) { return; }

    wxString newText = dlg.GetValue();
    if(newText.IsEmpty()) return;

    wxString newExpr = DoGetItemPath(item);
    m_treeCtrl->SetItemText(item, newText);

    // Create a new expression and ask GDB to evaluate it for us
    wxString typecast = newText;
    typecast.Trim().Trim(false);

    int where = typecast.Find(oldText);
    if(where == wxNOT_FOUND || where == 0) {
        // The new text edited by the user does not contain the "old" expression
        // OR it does contain it, but with an additional text to the END
        newExpr = DoGetItemPath(item);

    } else {
        typecast.Replace(oldText, wxT(""));
        typecast.Trim().Trim(false);

        if(!typecast.IsEmpty()) {
            if(!typecast.StartsWith(wxT("("))) typecast.Prepend(wxT("("));

            if(!typecast.EndsWith(wxT(")"))) typecast.Append(wxT(")"));
        }

        newExpr.Prepend(typecast);
    }

    s_Rect = GetScreenRect();
    HideDialog();
    m_debugger->CreateVariableObject(newExpr, false, DBG_USERR_QUICKWACTH);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PersistentDebuggerTip
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void PersistentDebuggerTip::Save() const
{
    const wxPopupWindow* const puw = Get();
    const wxSize size = puw->GetSize();
    SaveValue("w", size.x);
    SaveValue("h", size.y);
}
//-------------------------------------------------------------------------
bool PersistentDebuggerTip::Restore()
{
    wxPopupWindow* const puw = Get();

    long w(-1), h(-1);
    const bool hasSize = RestoreValue("w", &w) && RestoreValue("h", &h);

    if(hasSize) puw->SetSize(w, h);

    return hasSize;
}
//-------------------------------------------------------------------------
inline wxPersistentObject* wxCreatePersistentObject(wxPopupWindow* puw) { return new PersistentDebuggerTip(puw); }
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// NewVirtualFolderDlg
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
NewVirtualFolderDlg::NewVirtualFolderDlg(wxWindow* parent, const wxString& currentVD, wxWindowID id, 
    const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer2);
    
    wxFlexGridSizer* flexGridSizer10 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer10->SetFlexibleDirection( wxBOTH );
    flexGridSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer10->AddGrowableCol(1);
    
    boxSizer2->Add(flexGridSizer10, 1, wxALL|wxEXPAND, 5);
    
    m_staticText12 = gsgs_Style().createStaticText(this, wxID_ANY, _("Name:"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer10->Add(m_staticText12, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_textCtrlName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer10->Add(m_textCtrlName, 0, wxALL|wxEXPAND, 5);
    
    flexGridSizer10->Add(0, 0, 1, wxALL, 5);
    
    m_checkBoxCreateOnDisk = new wxCheckBox(this, wxID_ANY, _("Create the folder on the file system as well"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_checkBoxCreateOnDisk->SetValue(false);
    
    flexGridSizer10->Add(m_checkBoxCreateOnDisk, 0, wxALL, 5);
    
    flexGridSizer10->Add(0, 0, 1, wxALL, 5);
    
    m_textCtrlPath = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), wxTE_READONLY);
    flexGridSizer10->Add(m_textCtrlPath, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
    
    wxBoxSizer* boxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer2->Add(boxSizer4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_button6 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer4->Add(m_button6, 0, wxALL, 5);
    
    m_button8 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_button8->SetDefault();
    
    boxSizer4->Add(m_button8, 0, wxALL, 5);
    
    CentreOnParent(wxBOTH);

    // Connect events
    m_textCtrlName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(NewVirtualFolderDlg::OnNameUpdated), NULL, this);
    m_checkBoxCreateOnDisk->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewVirtualFolderDlg::OnCreateOnFolderChecked), NULL, this);
    m_textCtrlPath->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewVirtualFolderDlg::OnCreateOnDiskUI), NULL, this);
    m_button8->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewVirtualFolderDlg::OnOkUI), NULL, this);
    
    bool temp;
    gsgs_Config().Read(kConfigCreateVirtualFoldersOnDisk, temp, false);
    m_checkBoxCreateOnDisk->SetValue( temp );
    wxString project_name = currentVD.BeforeFirst(':');
    wxString vd_path = currentVD.AfterFirst(':');
    vd_path.Replace(":", wxFILE_SEP_PATH);
    wxString errmsg;
    ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project_name, errmsg);
    wxString projectPath = proj->GetFileName().GetPath();
    m_basePath = wxFileName(projectPath + wxFILE_SEP_PATH + vd_path, "").GetPath();
}
//--------------------------------------------------------------------------
NewVirtualFolderDlg::~NewVirtualFolderDlg()
{
    m_textCtrlName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(NewVirtualFolderDlg::OnNameUpdated), NULL, this);
    m_checkBoxCreateOnDisk->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(NewVirtualFolderDlg::OnCreateOnFolderChecked), NULL, this);
    m_textCtrlPath->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewVirtualFolderDlg::OnCreateOnDiskUI), NULL, this);
    m_button8->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewVirtualFolderDlg::OnOkUI), NULL, this);
    gsgs_Config().Write(kConfigCreateVirtualFoldersOnDisk, m_checkBoxCreateOnDisk->IsChecked());
}
//--------------------------------------------------------------------------
void NewVirtualFolderDlg::OnCreateOnDiskUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxCreateOnDisk->IsChecked());
}
//--------------------------------------------------------------------------
void NewVirtualFolderDlg::OnNameUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if ( m_checkBoxCreateOnDisk->IsChecked() ) {
        DoUpdatePath();
    }
}
//--------------------------------------------------------------------------
void NewVirtualFolderDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlName->IsEmpty() );
}
//--------------------------------------------------------------------------
void NewVirtualFolderDlg::OnCreateOnFolderChecked(wxCommandEvent& event)
{
    if ( event.IsChecked() ) {
        DoUpdatePath();
    } else {
        m_textCtrlPath->Clear();
    }
}
//--------------------------------------------------------------------------
void NewVirtualFolderDlg::DoUpdatePath()
{
    wxString curpath;
    curpath << m_basePath << wxFILE_SEP_PATH << m_textCtrlName->GetValue();
    m_textCtrlPath->ChangeValue( wxFileName(curpath, "").GetPath() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// ProjectCustomBuildTragetDlg
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD               = wxT("Build");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN               = wxT("Clean");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD             = wxT("Rebuild");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE = wxT("Compile Single File");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE     = wxT("Preprocess File");
//--------------------------------------------------------------------------
ProjectCustomBuildTragetDlg::ProjectCustomBuildTragetDlg(wxWindow* parent, const wxString &targetName, const wxString& targetCommand)
    : ProjectCustomBuildTragetDlgBase(parent)
{
    m_textCtrlCommand->ChangeValue( targetCommand );
    m_textCtrlTargetName->ChangeValue( targetName );
    SetName("ProjectCustomBuildTragetDlg");
    gsgs::WindowManager::Load(this);
}
//--------------------------------------------------------------------------
ProjectCustomBuildTragetDlg::~ProjectCustomBuildTragetDlg()
{
    
}
//--------------------------------------------------------------------------
void ProjectCustomBuildTragetDlg::OnEditTargetNameUI(wxUpdateUIEvent& event)
{
    event.Enable( !IsPredefinedTarget(GetTargetName()) );
}
//--------------------------------------------------------------------------
bool ProjectCustomBuildTragetDlg::IsPredefinedTarget(const wxString& name)
{
    return name == CUSTOM_TARGET_BUILD || 
           name == CUSTOM_TARGET_CLEAN || 
           name == CUSTOM_TARGET_REBUILD || 
           name == CUSTOM_TARGET_COMPILE_SINGLE_FILE ||
           name == CUSTOM_TARGET_PREPROCESS_FILE;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSBuildEventsPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSBuildEventsPage::PSBuildEventsPage(wxWindow* parent, bool preEvents, ProjectSettingsDlg* dlg)
    : PSBuildEventsBasePage(parent)
    , m_isPreEvents(preEvents)
    , m_dlg(dlg)
{
}
//--------------------------------------------------------------------------
void PSBuildEventsPage::OnCmdEvtVModified(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSBuildEventsPage::Load(BuildConfigPtr buildConf)
{
    Clear();

    BuildCommandList buildCmds;
    wxString text;
    if(m_isPreEvents) {
        buildConf->GetPreBuildCommands(buildCmds);
        text = _("Set the commands to run in the pre build stage");

    } else {
        buildConf->GetPostBuildCommands(buildCmds);
        text = _("Set the commands to run in the post build stage");
    }
    text << _("\nCommands starting with the hash sign ('#'), will not be executed");
    m_staticText11->SetLabel(text);
    BuildCommandList::const_iterator iter = buildCmds.begin();
    m_textCtrlBuildEvents->Clear();
    for(; iter != buildCmds.end(); iter++) {
        wxString cmdText = iter->GetCommand();
        cmdText.Trim().Trim(false);
        if(iter->GetEnabled() == false && !cmdText.StartsWith(wxT("#"))) {
            cmdText.Prepend(wxT("#"));
        }
        cmdText.Append(wxT("\n"));
        m_textCtrlBuildEvents->AppendText(cmdText);
    }

    m_textCtrlBuildEvents->Connect(
        wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(PSBuildEventsPage::OnCmdEvtVModified), NULL, this);
}
//--------------------------------------------------------------------------
void PSBuildEventsPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    BuildCommandList cmds;
    wxArrayString commands = ::SplitString(m_textCtrlBuildEvents->GetValue(), true);
    for(size_t i = 0; i < commands.GetCount(); i++) {
        wxString command = commands.Item(i).Trim().Trim(false);
        bool enabled = !command.StartsWith(wxT("#"));
        BuildCommand cmd(command, enabled);
        cmds.push_back(cmd);
    }

    if(m_isPreEvents) {
        buildConf->SetPreBuildCommands(cmds);

    } else {
        buildConf->SetPostBuildCommands(cmds);
    }
}
//--------------------------------------------------------------------------
void PSBuildEventsPage::Clear() { m_textCtrlBuildEvents->Clear(); }
//--------------------------------------------------------------------------
void PSBuildEventsPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSCompilerPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSCompilerPage::PSCompilerPage(wxWindow* parent,
                               const wxString& projectName,
                               ProjectSettingsDlg* dlg,
                               PSGeneralPage* gp)
    : PSCompilerPageBase(parent)
    , m_dlg(dlg)
    , m_projectName(projectName)
    , m_gp(gp)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
}
//--------------------------------------------------------------------------
void PSCompilerPage::Load(BuildConfigPtr buildConf)
{
    m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
    m_pgPropCppOpts->SetValueFromString(buildConf->GetCompileOptions());
    m_pgPropIncludePaths->SetValueFromString(buildConf->GetIncludePath());
    m_pgPropPreProcessors->SetValueFromString(buildConf->GetPreprocessor());
    m_pgPropPreCmpHeaderFile->SetValue(buildConf->GetPrecompiledHeader());
    m_pgPropCOpts->SetValue(buildConf->GetCCompileOptions());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildCmpWithGlobalSettings());
    m_pgPropIncludePCH->SetValue(buildConf->GetPchInCommandLine());
    m_pgPropPCHCompileLine->SetValue(buildConf->GetPchCompileFlags());
    m_pgPropAssembler->SetValue(buildConf->GetAssmeblerOptions());
    m_pgPropPCHPolicy->SetValue((int)buildConf->GetPCHFlagsPolicy());
}
//--------------------------------------------------------------------------
void PSCompilerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetCompilerRequired(!m_checkCompilerNeeded->IsChecked());
    buildConf->SetCompileOptions(m_pgPropCppOpts->GetValueAsString());
    buildConf->SetIncludePath(m_pgPropIncludePaths->GetValueAsString());
    buildConf->SetPreprocessor(m_pgPropPreProcessors->GetValueAsString());
    buildConf->SetPrecompiledHeader(m_pgPropPreCmpHeaderFile->GetValueAsString());
    buildConf->SetCCompileOptions(m_pgPropCOpts->GetValueAsString());
    buildConf->SetPchInCommandLine(m_pgPropIncludePCH->GetValue().GetBool());
    buildConf->SetPchCompileFlags(m_pgPropPCHCompileLine->GetValueAsString());
    buildConf->SetAssmeblerOptions(m_pgPropAssembler->GetValueAsString());
    buildConf->SetBuildCmpWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings->GetValueAsString());
    buildConf->SetPCHFlagsPolicy((BuildConfig::ePCHPolicy)m_pgPropPCHPolicy->GetValue().GetInteger());
}
//--------------------------------------------------------------------------
void PSCompilerPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        if(iter.GetProperty() && !iter.GetProperty()->IsCategory()) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkCompilerNeeded->SetValue(false);
}
//--------------------------------------------------------------------------
void PSCompilerPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
//--------------------------------------------------------------------------
void PSCompilerPage::OnPropertyChanged(wxPropertyGridEvent& event) { m_dlg->SetIsDirty(true); }
//--------------------------------------------------------------------------
void PSCompilerPage::OnUpdateUI(wxUpdateUIEvent& event) { event.Enable(true); }
//--------------------------------------------------------------------------
void PSCompilerPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);
    m_dlg->SetIsDirty(true);

    if(prop == m_pgPropPreProcessors || prop == m_pgPropIncludePaths || prop == m_pgPropAssembler) {
        wxString value = prop->GetValueAsString();
        if(PopupAddOptionDlg(value)) {
            prop->SetValueFromString(value);
        }

    } else if(prop == m_pgPropCppOpts || prop == m_pgPropCOpts) {
        wxString value = prop->GetValueAsString();
        wxString cmpName = m_gp->GetCompiler();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpName);
        if(PopupAddOptionCheckDlg(value, _("Compiler options"), cmp->GetCompilerOptions())) {
            prop->SetValueFromString(value);
        }
    } else if(prop == m_pgPropPreCmpHeaderFile) {
        wxFileName curvalue = prop->GetValueAsString();
        wxString program = ::wxFileSelector(_("Choose a file"), curvalue.GetPath());
        if(!program.IsEmpty()) {
            program.Replace("\\", "/");
            prop->SetValue(program);
        }
    }
}
//--------------------------------------------------------------------------
void PSCompilerPage::OnCompilerNeeded(wxCommandEvent& event) { m_dlg->SetIsDirty(true); }
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSCompletionPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSCompletionPage::PSCompletionPage(wxWindow* parent, ProjectSettingsDlg* dlg)
    : PSCompletionBase(parent)
    , m_dlg(dlg)
    , m_ccSettingsModified(false)
{
}
//--------------------------------------------------------------------------
void PSCompletionPage::OnCmdEvtVModified(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
    m_ccSettingsModified = true;
}
//--------------------------------------------------------------------------
void PSCompletionPage::Clear()
{
    m_textCtrlSearchPaths->Clear();
    m_textCtrlMacros->Clear();
    m_textCtrlSearchPaths->Clear();
    m_ccSettingsModified = false;
}
//--------------------------------------------------------------------------
void PSCompletionPage::Load(BuildConfigPtr buildConf)
{
    m_textCtrlMacros->SetValue(buildConf->GetClangPPFlags());
    m_textCtrlSearchPaths->SetValue(buildConf->GetCcSearchPaths());
    m_checkBoxC11->SetValue(buildConf->IsClangC11());
    m_checkBoxC14->SetValue(buildConf->IsClangC14());
    m_checkBoxC17->SetValue(buildConf->IsClangC17());
}
//--------------------------------------------------------------------------
void PSCompletionPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetClangPPFlags(m_textCtrlMacros->GetValue());
    buildConf->SetCcSearchPaths(m_textCtrlSearchPaths->GetValue());
    buildConf->SetClangC11(m_checkBoxC11->IsChecked());
    buildConf->SetClangC14(m_checkBoxC14->IsChecked());
    buildConf->SetClangC17(m_checkBoxC17->IsChecked());
    // Save was requested
    if(m_ccSettingsModified) { m_ccSettingsModified = false; }
}
//--------------------------------------------------------------------------
void PSCompletionPage::OnBrowsePCH(wxCommandEvent& event) {}
//--------------------------------------------------------------------------
void PSCompletionPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSCustomBuildPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSCustomBuildPage::PSCustomBuildPage(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg)
    : PSCustomBuildBasePage(parent)
    , m_projectName(projectName)
    , m_dlg(dlg)
{
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnCustomBuildEnabled(wxCommandEvent& event)
{
    GetDlg()->SetCustomBuildEnabled(event.IsChecked());
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnCustomBuildEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable(GetDlg()->IsCustomBuildEnabled());
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnCmdEvtVModified(wxCommandEvent& event) { GetDlg()->SetIsDirty(true); }
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnBrowseCustomBuildWD(wxCommandEvent& event)
{
    DirSaver ds;

    // Since all paths are relative to the project, set the working directory to the
    // current project path
    ProjectPtr p = gsgs_Manager()->GetProject(m_projectName);
    if(p) {
        wxSetWorkingDirectory(p->GetFileName().GetPath());
    }

    wxFileName fn(m_textCtrlCustomBuildWD->GetValue());
    wxString initPath(wxEmptyString);

    if(fn.DirExists()) {
        fn.MakeAbsolute();
        initPath = fn.GetFullPath();
    }

    wxString new_path =
        wxDirSelector(_("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        m_textCtrlCustomBuildWD->SetValue(new_path);
    }
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnNewTarget(wxCommandEvent& event)
{
    wxUnusedVar(event);

    ProjectCustomBuildTragetDlg dlg(this, "", "");
    if(dlg.ShowModal() == wxID_OK) {
        GetDlg()->SetIsDirty(true);
        if(GetTargetCommand(dlg.GetName()).IsEmpty() == false) {
            wxMessageBox(wxString::Format(_("Target '%s' already exist!"), dlg.GetName().c_str()),
                         _("光栅故事"),
                         wxICON_WARNING | wxCENTER | wxOK,
                         this);
            return;
        }
        wxVector<wxVariant> cols;
        cols.push_back(dlg.GetTargetName());
        cols.push_back(dlg.GetTargetCommand());
        m_dvListCtrlTargets->AppendItem(cols);
    }
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnEditTarget(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    CHECK_ITEM_RET(item);
    DoEditTarget(item);
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnEditTargetUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    event.Enable(item.IsOk() && m_checkEnableCustomBuild->IsChecked());
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnDeleteTarget(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    if(item.IsOk()) {
        m_dvListCtrlTargets->DeleteItem(m_dvListCtrlTargets->ItemToRow(item));
        GetDlg()->SetIsDirty(true);
    }
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnDeleteTargetUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    if(item.IsOk()) {
        wxVariant vTarget;
        m_dvListCtrlTargets->GetValue(vTarget, m_dvListCtrlTargets->ItemToRow(item), 0);
        wxString name = vTarget.GetString();
        event.Enable(name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE &&
                     m_checkEnableCustomBuild->IsChecked());
    } else {
        event.Enable(false);
    }
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::DoEditTarget(wxDataViewItem item)
{
    CHECK_ITEM_RET(item);
    wxVariant varname, varcommand;
    m_dvListCtrlTargets->GetValue(varname, m_dvListCtrlTargets->ItemToRow(item), 0);
    m_dvListCtrlTargets->GetValue(varcommand, m_dvListCtrlTargets->ItemToRow(item), 1);
    wxString target = varname.GetString();
    wxString cmd = varcommand.GetString();
    ProjectCustomBuildTragetDlg dlg(this, target, cmd);
    if(dlg.ShowModal() == wxID_OK) {
        DoUpdateTarget(item, dlg.GetTargetName(), dlg.GetTargetCommand());
        GetDlg()->SetIsDirty(true);
    }

#ifdef __WXMAC__
    GetDlg()->Raise();
#endif
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::DoUpdateTarget(wxDataViewItem item, const wxString& target, const wxString& cmd)
{
    CHECK_ITEM_RET(item);
    int row = m_dvListCtrlTargets->ItemToRow(item);

    m_dvListCtrlTargets->SetValue(target, row, 0);
    m_dvListCtrlTargets->SetValue(cmd, row, 1);
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnCustomBuildCBEnabledUI(wxUpdateUIEvent& event)
{
    GetDlg()->SetCustomBuildEnabled(m_checkEnableCustomBuild->IsChecked());
}
//-------------------------------------------------------------------------
ProjectSettingsDlg* PSCustomBuildPage::GetDlg() { return m_dlg; }
//-------------------------------------------------------------------------
wxString PSCustomBuildPage::GetTargetCommand(const wxString& target)
{
    for(int i = 0; i < m_dvListCtrlTargets->GetItemCount(); i++) {
        wxVariant variantTarget;
        m_dvListCtrlTargets->GetValue(variantTarget, i, 0);
        if(variantTarget.GetString() == target) {
            wxVariant command;
            m_dvListCtrlTargets->GetValue(command, i, 1);
            return command.GetString();
        }
    }
    return wxEmptyString;
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::Load(BuildConfigPtr buildConf)
{
    m_checkEnableCustomBuild->SetValue(buildConf->IsCustomBuild());
    m_textCtrlCustomBuildWD->SetValue(buildConf->GetCustomBuildWorkingDir());

    m_dvListCtrlTargets->DeleteAllItems();

    wxVector<wxVariant> cols;

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD);
    cols.push_back(buildConf->GetCustomBuildCmd());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN);
    cols.push_back(buildConf->GetCustomCleanCmd());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD);
    cols.push_back(buildConf->GetCustomRebuildCmd());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE);
    cols.push_back(buildConf->GetSingleFileBuildCommand());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE);
    cols.push_back(buildConf->GetPreprocessFileCommand());
    m_dvListCtrlTargets->AppendItem(cols);

    // Initialize the custom build targets
    std::map<wxString, wxString> targets = buildConf->GetCustomTargets();
    std::map<wxString, wxString>::iterator titer = targets.begin();
    for(; titer != targets.end(); ++titer) {

        if(ProjectCustomBuildTragetDlg::IsPredefinedTarget(titer->first)) continue;

        cols.clear();
        cols.push_back(titer->first);
        cols.push_back(titer->second);
        m_dvListCtrlTargets->AppendItem(cols);
    }
    m_dlg->SetCustomBuildEnabled(m_checkEnableCustomBuild->IsChecked());
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    // loop over the list and create the targets map
    std::map<wxString, wxString> targets;
    for(int i = 0; i < m_dvListCtrlTargets->GetItemCount(); i++) {
        wxVariant vTarget, vCommand;
        m_dvListCtrlTargets->GetValue(vTarget, i, 0);
        m_dvListCtrlTargets->GetValue(vCommand, i, 1);
        if(ProjectCustomBuildTragetDlg::IsPredefinedTarget(vTarget.GetString())) continue;

        targets[vTarget.GetString()] = vCommand.GetString();
    }

    buildConf->SetCustomTargets(targets);
    buildConf->SetCustomBuildCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD));
    buildConf->SetCustomCleanCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN));
    buildConf->SetCustomRebuildCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD));
    buildConf->SetSingleFileBuildCommand(
        GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE));
    buildConf->SetPreprocessFileCommand(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE));
    buildConf->EnableCustomBuild(m_checkEnableCustomBuild->IsChecked());
    buildConf->SetCustomBuildWorkingDir(m_textCtrlCustomBuildWD->GetValue());
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::Clear()
{
    m_checkEnableCustomBuild->SetValue(false);
    m_dvListCtrlTargets->DeleteAllItems();
    m_textCtrlCustomBuildWD->ChangeValue(wxT("$(WorkspacePath)"));
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnTargetActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent dummy;
    OnEditTarget(dummy);
}
//-------------------------------------------------------------------------
void PSCustomBuildPage::OnEnableTableUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkEnableCustomBuild->IsChecked());
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSCustomMakefileRulesPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSCustomMakefileRulesPage::PSCustomMakefileRulesPage( wxWindow* parent, ProjectSettingsDlg *dlg )
    : PSCustomMakefileBasePage( parent )
    , m_dlg(dlg)
{
}
//--------------------------------------------------------------------------
void PSCustomMakefileRulesPage::OnProjectCustumBuildUI( wxUpdateUIEvent& event )
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() );
}
//--------------------------------------------------------------------------
void PSCustomMakefileRulesPage::OnCmdEvtVModified( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSCustomMakefileRulesPage::Load(BuildConfigPtr buildConf)
{
    //set the custom pre-prebuild step
    wxString customPreBuild = buildConf->GetPreBuildCustom();

    //extract the dependencies
    wxString deps, rules;
    deps = customPreBuild.BeforeFirst(wxT('\n'));
    rules = customPreBuild.AfterFirst(wxT('\n'));

    rules = rules.Trim();
    rules = rules.Trim(false);

    deps = deps.Trim();
    deps = deps.Trim(false);

    m_textDeps->SetValue(deps);
    m_textPreBuildRule->SetValue(rules);
}
//--------------------------------------------------------------------------
void PSCustomMakefileRulesPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    //set the pre-build step
    wxString rules = m_textPreBuildRule->GetValue();
    wxString deps = m_textDeps->GetValue();

    rules = rules.Trim();
    rules = rules.Trim(false);
    deps = deps.Trim();
    deps = deps.Trim(false);

    wxString prebuilstep;
    prebuilstep << deps << wxT("\n");
    prebuilstep << rules;
    prebuilstep << wxT("\n");

    // Set the content only if there is real content to add
    wxString tmpPreBuildStep(prebuilstep);
    tmpPreBuildStep.Trim().Trim(false);
    if(tmpPreBuildStep.IsEmpty() == false) {
        buildConf->SetPreBuildCustom(prebuilstep);
    } else {
        buildConf->SetPreBuildCustom(wxT(""));
    }
}
//--------------------------------------------------------------------------
void PSCustomMakefileRulesPage::Clear()
{
    m_textPreBuildRule->Clear();
    m_textDeps->Clear();
}
//--------------------------------------------------------------------------
void PSCustomMakefileRulesPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSDebuggerPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSDebuggerPage::PSDebuggerPage( wxWindow* parent, ProjectSettingsDlg* dlg )
    : PSDebuggerPageBase( parent )
    , m_dlg(dlg)
{
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnCmdEvtVModified( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnRemoteDebugUI( wxUpdateUIEvent& event )
{
    event.Enable(m_checkBoxDbgRemote->IsChecked());
}
//--------------------------------------------------------------------------
void PSDebuggerPage::Load(BuildConfigPtr buildConf)
{
    Clear();
    m_textCtrlDbgCmds->ChangeValue(buildConf->GetDebuggerStartupCmds());
    m_textCtrlDbgPostConnectCmds->ChangeValue(buildConf->GetDebuggerPostRemoteConnectCmds());
    m_checkBoxDbgRemote->SetValue(buildConf->GetIsDbgRemoteTarget());
    m_textCtrl1DbgHost->ChangeValue(buildConf->GetDbgHostName());
    m_textCtrlDbgPort->ChangeValue(buildConf->GetDbgHostPort());
    m_textCtrlDebuggerPath->ChangeValue(buildConf->GetDebuggerPath());
    m_checkBoxDbgRemoteExt->SetValue(buildConf->GetIsDbgRemoteExtended());

    const wxArrayString &searchPaths = buildConf->GetDebuggerSearchPaths();
    for(size_t i=0; i<searchPaths.GetCount(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back( searchPaths.Item(i) );
        m_dvListCtrlDebuggerSearchPaths->AppendItem( cols , (wxUIntPtr)NULL );
    }
}
//--------------------------------------------------------------------------
void PSDebuggerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetDebuggerStartupCmds(m_textCtrlDbgCmds->GetValue());
    buildConf->SetDebuggerPostRemoteConnectCmds(m_textCtrlDbgPostConnectCmds->GetValue());
    buildConf->SetIsDbgRemoteTarget(m_checkBoxDbgRemote->IsChecked());
    buildConf->SetDbgHostName(m_textCtrl1DbgHost->GetValue());
    buildConf->SetDbgHostPort(m_textCtrlDbgPort->GetValue());
    buildConf->SetDebuggerPath(m_textCtrlDebuggerPath->GetValue());
    buildConf->SetIsDbgRemoteExtended(m_checkBoxDbgRemoteExt->IsChecked());

    wxArrayString searchPaths;
    int nCount = m_dvListCtrlDebuggerSearchPaths->GetItemCount();
    for(int i=0; i<nCount; ++i) {
        wxVariant colValue;
        m_dvListCtrlDebuggerSearchPaths->GetValue(colValue, i, 0);
        if ( !colValue.IsNull() ) {
            searchPaths.Add( colValue.GetString() );
        }
    }
    buildConf->SetDebuggerSearchPaths(searchPaths);
}
//--------------------------------------------------------------------------
void PSDebuggerPage::Clear()
{
    m_textCtrlDebuggerPath->Clear();
    m_textCtrl1DbgHost->Clear();
    m_textCtrlDbgCmds->Clear();
    m_textCtrlDbgPort->Clear();
    m_textCtrlDbgPostConnectCmds->Clear();
    m_dvListCtrlDebuggerSearchPaths->DeleteAllItems();
    m_checkBoxDbgRemote->SetValue(false);
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnBrowseForDebuggerPath(wxCommandEvent& event)
{
    wxString debugger_path = ::wxFileSelector(_("Select debugger:"));

    if ( !debugger_path.IsEmpty() ) {
        wxString errMsg;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(m_dlg->GetProjectName(), errMsg);
        if ( proj ) {
            wxFileName fnDebuggerPath( debugger_path );
            wxString project_path = proj->GetFileName().GetPath();
            if ( fnDebuggerPath.MakeRelativeTo( project_path ) ) {
                debugger_path = fnDebuggerPath.GetFullPath();
            }
        }
        m_textCtrlDebuggerPath->ChangeValue( debugger_path );
        m_dlg->SetIsDirty(true);
    }
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnAddDebuggerSearchPath(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector();
    if ( !path.IsEmpty() ) {
        wxVector<wxVariant> cols;
        cols.push_back( path );
        m_dvListCtrlDebuggerSearchPaths->AppendItem(cols, (wxUIntPtr)NULL);
        m_dlg->SetIsDirty(true);
    }
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnDeleteDebuggerSearchPath(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrlDebuggerSearchPaths->GetSelections( items );
    if ( items.IsEmpty() )
        return;

    for(size_t i=0; i<items.GetCount(); ++i) {
        m_dvListCtrlDebuggerSearchPaths->DeleteItem( m_dvListCtrlDebuggerSearchPaths->ItemToRow( items.Item(i) ) );
    }
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnDeleteDebuggerSearchPathUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dvListCtrlDebuggerSearchPaths->GetSelectedItemsCount() );
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnItemActivated(wxDataViewEvent& event)
{
    wxVariant value;
    m_dvListCtrlDebuggerSearchPaths->GetValue(value, m_dvListCtrlDebuggerSearchPaths->ItemToRow(event.GetItem()), 0);

    if ( !value.IsNull() ) {

        wxString path = value.GetString();
        path = ::wxDirSelector(_("Select a folder"), path);

        if ( !path.IsEmpty() ) {
            m_dvListCtrlDebuggerSearchPaths->DeleteItem( m_dvListCtrlDebuggerSearchPaths->ItemToRow(event.GetItem()) );
            ::PostCall(this, (EventFunc_t) &PSDebuggerPage::DoAddPath, new wxStringClientData(path));

        }
    }
}
//--------------------------------------------------------------------------
void PSDebuggerPage::DoAddPath(wxClientData * _path)
{
    wxStringClientData * path = static_cast<wxStringClientData *> (_path);
    wxVector<wxVariant> cols;
    cols.push_back( path->GetData() );
    m_dvListCtrlDebuggerSearchPaths->AppendItem(cols, (wxUIntPtr)NULL);
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSDebuggerPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSResourcesPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
#ifdef __WXMSW__
#    define IS_WINDOWS true
#else
#    define IS_WINDOWS false
#endif
//--------------------------------------------------------------------------
PSResourcesPage::PSResourcesPage( wxWindow* parent, ProjectSettingsDlg *dlg )
    : PSResourcesPageBase( parent )
    , m_dlg(dlg)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
}
//--------------------------------------------------------------------------
void PSResourcesPage::Load(BuildConfigPtr buildConf)
{
    m_pgPropResCmpOptions->SetValue(buildConf->GetResCompileOptions());
    m_pgPropResCmpSearchPath->SetValue(buildConf->GetResCmpIncludePath());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildResWithGlobalSettings());
}
//--------------------------------------------------------------------------
void PSResourcesPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetResCmpIncludePath( m_pgPropResCmpSearchPath->GetValueAsString() );
    buildConf->SetResCmpOptions( m_pgPropResCmpOptions->GetValueAsString() );
    buildConf->SetBuildResWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings->GetValueAsString());
}
//--------------------------------------------------------------------------
void PSResourcesPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for( ; !iter.AtEnd(); ++iter ) {
        if ( iter.GetProperty() && !iter.GetProperty()->IsCategory() ) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
}
//--------------------------------------------------------------------------
void PSResourcesPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    m_dlg->SetIsDirty(true);
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);

    if ( prop == m_pgPropResCmpOptions || prop == m_pgPropResCmpSearchPath ) {
        wxString v = prop->GetValueAsString();
        if ( PopupAddOptionDlg( v ) ) {
            prop->SetValue( v );
        }
    }
}
//--------------------------------------------------------------------------
void PSResourcesPage::OnValueChanged(wxPropertyGridEvent& event)
{
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSResourcesPage::OnResourcesEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() );
}
//--------------------------------------------------------------------------
void PSResourcesPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSLinkerPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSLinkerPage::PSLinkerPage(wxWindow* parent, ProjectSettingsDlg* dlg, PSGeneralPage* gp)
    : PSLinkPageBase(parent)
    , m_dlg(dlg)
    , m_gp(gp)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
}
//--------------------------------------------------------------------------
void PSLinkerPage::OnLinkerNotNeededUI(wxUpdateUIEvent& event) { event.Enable(!m_checkLinkerNeeded->IsChecked()); }
//--------------------------------------------------------------------------
void PSLinkerPage::Load(BuildConfigPtr buildConf)
{
    m_checkLinkerNeeded->SetValue(!buildConf->IsLinkerRequired());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildLnkWithGlobalSettings());
    m_pgPropLibraries->SetValue(buildConf->GetLibraries());
    m_pgPropLibraryPaths->SetValue(buildConf->GetLibPath());
    m_pgPropOptions->SetValue(buildConf->GetLinkOptions());
}
//--------------------------------------------------------------------------
void PSLinkerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetLinkerRequired(!m_checkLinkerNeeded->IsChecked());
    buildConf->SetLibPath(m_pgPropLibraryPaths->GetValueAsString());
    buildConf->SetLibraries(m_pgPropLibraries->GetValueAsString());
    buildConf->SetLinkOptions(m_pgPropOptions->GetValueAsString());
    buildConf->SetBuildLnkWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings->GetValueAsString());
}
//--------------------------------------------------------------------------
void PSLinkerPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        if(iter.GetProperty() && !iter.GetProperty()->IsCategory()) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkLinkerNeeded->SetValue(false);
}
//--------------------------------------------------------------------------
void PSLinkerPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);
    m_dlg->SetIsDirty(true);

    if(prop == m_pgPropLibraries || prop == m_pgPropLibraryPaths) {
        wxString value = prop->GetValueAsString();
        if(PopupAddOptionDlg(value)) {
            prop->SetValueFromString(value);
        }

    } else if(prop == m_pgPropOptions) {
        wxString value = prop->GetValueAsString();
        wxString cmpName = m_gp->GetCompiler();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpName);
        if(PopupAddOptionCheckDlg(value, _("Linker options"), cmp->GetLinkerOptions())) {
            prop->SetValueFromString(value);
        }
    }
}
//--------------------------------------------------------------------------
void PSLinkerPage::OnCheckLinkerNeeded(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSLinkerPage::OnProjectCustumBuildUI(wxUpdateUIEvent& event) { event.Enable(!m_dlg->IsCustomBuildEnabled()); }
//--------------------------------------------------------------------------
void PSLinkerPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
//--------------------------------------------------------------------------
void PSLinkerPage::OnPropertyChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSEnvironmentPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSEnvironmentPage::PSEnvironmentPage(wxWindow* parent, ProjectSettingsDlg* dlg)
    : PSEnvironmentBasePage(parent)
    , m_dlg(dlg)
{
}
//--------------------------------------------------------------------------
void PSEnvironmentPage::OnCmdEvtVModified(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}
//--------------------------------------------------------------------------
void PSEnvironmentPage::Load(BuildConfigPtr buildConf)
{
    ///////////////////////////////////////////////////////////////////////////
    // Set the environment page
    ///////////////////////////////////////////////////////////////////////////
    m_choiceEnv->Clear();
    gsgsStringMap envSets;
    EvnVarList vars;
    gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);
    envSets = vars.GetEnvVarSets();
    gsgsStringMap::const_iterator iterI = envSets.begin();
    int useActiveSetIndex = m_choiceEnv->Append(wxGetTranslation(USE_WORKSPACE_ENV_VAR_SET));

    for(; iterI != envSets.end(); iterI++) {
        m_choiceEnv->Append(iterI->first);
    }
    int selEnv = m_choiceEnv->FindString(buildConf->GetEnvVarSet());
    m_choiceEnv->SetSelection(selEnv == wxNOT_FOUND ? useActiveSetIndex : selEnv);

    m_choiceDbgEnv->Clear();
    useActiveSetIndex = m_choiceDbgEnv->Append(wxGetTranslation(USE_GLOBAL_SETTINGS));

    DebuggerSettingsPreDefMap data;
    gsgs_Config().Read(wxT("DebuggerCommands"), &data);
    const std::map<wxString, DebuggerPreDefinedTypes>& preDefTypes = data.GePreDefinedTypesMap();
    std::map<wxString, DebuggerPreDefinedTypes>::const_iterator iterB = preDefTypes.begin();
    for(; iterB != preDefTypes.end(); iterB++) {
        m_choiceDbgEnv->Append(iterB->first);
    }

    int selDbg = m_choiceDbgEnv->FindString(buildConf->GetDbgEnvSet());
    m_choiceDbgEnv->SetSelection(selEnv == wxNOT_FOUND ? useActiveSetIndex : selDbg);
    m_textCtrlEnvvars->SetValue(buildConf->GetEnvvars());
}
//--------------------------------------------------------------------------
void PSEnvironmentPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    wxString env = m_choiceDbgEnv->GetStringSelection();
    if(env == wxGetTranslation(USE_GLOBAL_SETTINGS)) {
        // Save it untranslated
        env = USE_GLOBAL_SETTINGS;
    }
    buildConf->SetDbgEnvSet(env);

    env = m_choiceEnv->GetStringSelection();
    if(env == wxGetTranslation(USE_WORKSPACE_ENV_VAR_SET)) {
        env = USE_WORKSPACE_ENV_VAR_SET;
    }
    buildConf->SetEnvVarSet(env);
    buildConf->SetEnvvars(m_textCtrlEnvvars->GetValue().Trim().Trim(false));
}
//--------------------------------------------------------------------------
void PSEnvironmentPage::Clear()
{
    m_choiceDbgEnv->Clear();
    m_choiceEnv->Clear();
}
//--------------------------------------------------------------------------
void PSEnvironmentPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSGeneralPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PSGeneralPage::PSGeneralPage(
    wxWindow* parent, const wxString& projectName, const wxString& conf, ProjectSettingsDlg* dlg)
    : PSGeneralPageBase(parent)
    , m_dlg(dlg)
    , m_projectName(projectName)
    , m_configName(conf)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr136->GetGrid());
}

void PSGeneralPage::OnProjectCustumBuildUI(wxUpdateUIEvent& event)
{
    // do not disable the General page even when in custom build
    event.Enable(m_checkBoxEnabled->IsChecked());
}

void PSGeneralPage::Load(BuildConfigPtr buildConf)
{
    Clear();
    m_configName = buildConf->GetName();
    m_checkBoxEnabled->SetValue(buildConf->IsProjectEnabled());
    m_pgPropArgs->SetValue(buildConf->GetCommandArguments());
    m_pgPropDebugArgs->SetValueFromString(buildConf->GetDebugArgs());
    m_pgPropIntermediateFolder->SetValueFromString(buildConf->GetIntermediateDirectory());
    m_pgPropGUIApp->SetValue(buildConf->IsGUIProgram());
    m_pgPropOutputFile->SetValueFromString(buildConf->GetOutputFileName());
    m_pgPropPause->SetValue(buildConf->GetPauseWhenExecEnds());
    m_pgPropProgram->SetValueFromString(buildConf->GetCommand());
    m_pgPropWorkingDirectory->SetValue(buildConf->GetWorkingDirectory());
    // Project type
    wxPGChoices choices;
    choices.Add(PROJECT_TYPE_STATIC_LIBRARY);
    choices.Add(PROJECT_TYPE_DYNAMIC_LIBRARY);
    choices.Add(PROJECT_TYPE_EXECUTABLE);
    m_pgPropProjectType->SetChoices(choices);

    int sel = choices.Index(buildConf->GetProjectType());
    if(sel != wxNOT_FOUND) {
        m_pgPropProjectType->SetChoiceSelection(sel);
    }

    // Builders
    wxPGChoices builders;
    std::list<wxString> buildersList;
    gsgs_Build().GetBuilders(buildersList);
    std::for_each(buildersList.begin(), buildersList.end(), [&](const wxString& builder) { builders.Add(builder); });
    m_pgPropMakeGenerator->SetChoices(builders);
    m_pgPropMakeGeneratorArgs->SetValue(buildConf->GetBuildSystemArguments());
    m_pgPropMakeGenerator->SetExpanded(false);
    
    sel = builders.Index(buildConf->GetBuildSystem());
    if(sel != wxNOT_FOUND) {
        m_pgPropMakeGenerator->SetChoiceSelection(sel);
    }
    
    // Compilers
    choices.Clear();
    wxString cmpType = buildConf->GetCompilerType();
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = gsgs_Settings().GetFirstCompiler(cookie);
    while(cmp) {
        choices.Add(cmp->GetName());
        cmp = gsgs_Settings().GetNextCompiler(cookie);
    }
    m_pgPropCompiler->SetChoices(choices);
    sel = choices.Index(buildConf->GetCompiler()->GetName());
    if(sel != wxNOT_FOUND) {
        m_pgPropCompiler->SetChoiceSelection(sel);
    }

    // Debuggers
    choices.Clear();
    wxString dbgType = buildConf->GetDebuggerType();
    wxArrayString dbgs = DebuggerMgr::Get().GetAvailableDebuggers();
    choices.Add(dbgs);
    m_pgPropDebugger->SetChoices(choices);
    sel = choices.Index(buildConf->GetDebuggerType());
    if(sel != wxNOT_FOUND) {
        m_pgPropDebugger->SetChoiceSelection(sel);
    }
    m_pgPropUseSeparateDebuggerArgs->SetValue(buildConf->GetUseSeparateDebugArgs());
    m_dlg->SetIsProjectEnabled(buildConf->IsProjectEnabled());
}

void PSGeneralPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetOutputFileName(GetPropertyAsString(m_pgPropOutputFile));
    buildConf->SetIntermediateDirectory(GetPropertyAsString(m_pgPropIntermediateFolder));
    buildConf->SetCommand(GetPropertyAsString(m_pgPropProgram));
    buildConf->SetCommandArguments(GetPropertyAsString(m_pgPropArgs));
    buildConf->SetWorkingDirectory(GetPropertyAsString(m_pgPropWorkingDirectory));

    // Get the project type selection, unlocalised
    projSettingsPtr->SetProjectType(GetPropertyAsString(m_pgPropProjectType));
    buildConf->SetBuildSystemArguments(GetPropertyAsString(m_pgPropMakeGeneratorArgs));
    buildConf->SetBuildSystem(GetPropertyAsString(m_pgPropMakeGenerator));
    buildConf->SetCompilerType(GetPropertyAsString(m_pgPropCompiler));
    buildConf->SetDebuggerType(GetPropertyAsString(m_pgPropDebugger));
    buildConf->SetPauseWhenExecEnds(GetPropertyAsBool(m_pgPropPause));
    buildConf->SetProjectType(GetPropertyAsString(m_pgPropProjectType));
    buildConf->SetDebugArgs(GetPropertyAsString(m_pgPropDebugArgs));
    buildConf->SetIsGUIProgram(GetPropertyAsBool(m_pgPropGUIApp));
    buildConf->SetIsProjectEnabled(m_checkBoxEnabled->IsChecked());
    buildConf->SetUseSeparateDebugArgs(GetPropertyAsBool(m_pgPropUseSeparateDebuggerArgs));
}

void PSGeneralPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr136->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        if(iter.GetProperty() && !iter.GetProperty()->IsCategory()) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkBoxEnabled->SetValue(true);
}

void PSGeneralPage::OnValueChanged(wxPropertyGridEvent& event) { m_dlg->SetIsDirty(true); }

bool PSGeneralPage::GetPropertyAsBool(wxPGProperty* prop) const
{
    wxVariant v = prop->GetValue();
    bool b = v.GetBool();
    return b;
}

wxString PSGeneralPage::GetPropertyAsString(wxPGProperty* prop) const
{
    wxString s = prop->GetValueAsString();
    return s;
}

void PSGeneralPage::OnProjectEnabled(wxCommandEvent& event)
{
    m_dlg->SetIsProjectEnabled(event.IsChecked());
    m_dlg->SetIsDirty(true);
    m_dlg->ShowHideDisabledMessage();
}

void PSGeneralPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr136->GetSelectedProperty();
    CHECK_PTR_RET(prop);
    m_dlg->SetIsDirty(true);

    if(prop == m_pgPropProgram) {
        wxFileName curvalue = prop->GetValueAsString();
        wxString program = ::wxFileSelector(_("Choose a file"), curvalue.GetPath());
        if(!program.IsEmpty()) {
            program.Replace("\\", "/");
            prop->SetValue(program);
        }

    } else if(prop == m_pgPropWorkingDirectory) {
        wxString curpath = prop->GetValueAsString();
        wxFileName fp(curpath, "");
        wxString newPath = ::wxDirSelector(_("Choose a directory"), fp.GetPath());
        if(!newPath.IsEmpty()) {
            newPath.Replace("\\", "/");
            prop->SetValue(newPath);
        }
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// LoadTabGroupDlg
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
LoadTabGroupDlg::LoadTabGroupDlg(wxWindow* parent, const wxString& path, const wxArrayString& previousgroups, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* mainsizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainsizer);
    
    wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
    
    mainsizer->Add(bSizer2, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer2->Add(bSizer3, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select a tab group, or browse for one"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer3->Add(m_staticText1, 1, wxALL|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
    
    m_buttonBrowse = new Button(this, wxID_ANY, _("..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonBrowse->SetToolTip(_("Click to search for other groups"));
    
    bSizer3->Add(m_buttonBrowse, 0, wxALL, WXC_FROM_DIP(5));
    
    wxArrayString m_listBoxArr;
    m_listBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_listBoxArr, 0);
    m_listBox->SetToolTip(_("Double-click to choose one of these groups"));
    
    bSizer2->Add(m_listBox, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    m_listBox->SetMinSize(wxSize(450,100));
    
    m_replaceCheck = new wxCheckBox(this, wxID_ANY, _("Replace current tabs"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_replaceCheck->SetValue(false);
    m_replaceCheck->SetToolTip(_("By default, the tabs of this group will be added to the current tabs. Tick this to replace the current tabs instead."));
    
    bSizer2->Add(m_replaceCheck, 0, wxALL, WXC_FROM_DIP(5));
    
    wxBoxSizer* bSizer11 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer2->Add(bSizer11, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_button6 = new Button(this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button6->SetDefault();
    
    bSizer11->Add(m_button6, 0, wxALL, WXC_FROM_DIP(5));
    
    m_button5 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer11->Add(m_button5, 0, wxALL, WXC_FROM_DIP(5));
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_buttonBrowse->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LoadTabGroupDlg::OnBrowse), NULL, this);
    m_listBox->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(LoadTabGroupDlg::OnItemActivated), NULL, this);
    
    
    for(size_t n = 0; n < previousgroups.GetCount(); ++n) {
        InsertListItem(previousgroups.Item(n));
    }

    // Add any groups in the passed dir (which is probably the workspace dir)
    wxArrayString pwdfiles;
    wxDir::GetAllFiles(path, &pwdfiles, wxT("*.tabgroup"), wxDIR_FILES);
    for(size_t n = 0; n < pwdfiles.GetCount(); ++n) {
        InsertListItem(pwdfiles.Item(n));
    }
}
//--------------------------------------------------------------------------
LoadTabGroupDlg::~LoadTabGroupDlg() 
{
    m_buttonBrowse->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LoadTabGroupDlg::OnBrowse), NULL, this);
    m_listBox->Disconnect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(LoadTabGroupDlg::OnItemActivated), NULL, this);
}
//--------------------------------------------------------------------------
void LoadTabGroupDlg::InsertListItem(const wxString& item)
{
    if(item.IsEmpty() == false) {
        int AlreadyThere = m_listBox->FindString(item, true);

        if(AlreadyThere != wxNOT_FOUND) {
            m_listBox->SetSelection(AlreadyThere);
        } else {
            m_listBox->Insert(item, 0);
            m_listBox->SetSelection(0);
        }
    }
}
//--------------------------------------------------------------------------
void LoadTabGroupDlg::OnBrowse(wxCommandEvent& WXUNUSED(event))
{
    wxString group = wxFileSelector(_("Select a tab group:"),
                                    wxT(""),
                                    wxT(""),
                                    tabgroupTag,
                                    wxString(_("Tab groups")) + wxT(" (*.tabgroup)|*.tabgroup"),
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                                    this);
    if(!group.IsEmpty()) {
        // Need to insert/select the new item, otherwise it won't be used
        InsertListItem(group);
        EndModal(wxID_OK);
    }
}
//--------------------------------------------------------------------------
void LoadTabGroupDlg::OnItemActivated(wxCommandEvent& WXUNUSED(event))
{
    int idx = m_listBox->GetSelection();
    EndModal((idx == wxNOT_FOUND) ? wxID_CANCEL : wxID_OK);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// SaveTabGroupDlg
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
SaveTabGroupDlg::SaveTabGroupDlg(wxWindow* parent, const wxArrayString& previousgroups, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{

    wxBoxSizer* mainsizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainsizer);
    
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
    
    mainsizer->Add(bSizer1, 1, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select which tabs you want to be in the group"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer1->Add(m_staticText1, 0, wxALL, WXC_FROM_DIP(5));
    
    wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer1->Add(bSizer3, 1, wxALL|wxEXPAND, WXC_FROM_DIP(0));
    
    wxArrayString m_ListTabsArr;
    m_ListTabs = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_ListTabsArr, 0);
    
    bSizer3->Add(m_ListTabs, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    m_ListTabs->SetMinSize(wxSize(400,-1));
    
    wxBoxSizer* bSizer13 = new wxBoxSizer(wxVERTICAL);
    
    bSizer3->Add(bSizer13, 0, wxALL, WXC_FROM_DIP(5));
    
    m_buttonCheckAll = new Button(this, wxID_ANY, _("Check All"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonCheckAll->SetToolTip(_("Click to select all items"));
    
    bSizer13->Add(m_buttonCheckAll, 0, wxALL, WXC_FROM_DIP(5));
    
    m_buttonClearAll = new Button(this, wxID_ANY, _("Clear All"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonClearAll->SetToolTip(_("Click to clear all items"));
    
    bSizer13->Add(m_buttonClearAll, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    wxBoxSizer* bSizer10 = new wxBoxSizer(wxVERTICAL);
    
    bSizer1->Add(bSizer10, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Choose a name for the group:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer10->Add(m_staticText4, 0, wxALL|wxALIGN_LEFT, WXC_FROM_DIP(5));
    
    m_textName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    bSizer10->Add(m_textName, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxArrayString m_radioBoxWorkspaceOrGlobalArr;
    m_radioBoxWorkspaceOrGlobalArr.Add(_("Workspace"));
    m_radioBoxWorkspaceOrGlobalArr.Add(_("Global"));
    m_radioBoxWorkspaceOrGlobal = new wxRadioBox(this, wxID_ANY, _("Save in the current Workspace, or Globally"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), m_radioBoxWorkspaceOrGlobalArr, 1, wxRA_SPECIFY_ROWS);
    m_radioBoxWorkspaceOrGlobal->SetToolTip(_("Tabgroups are usually specific to a single workspace. However if you want to use one in multiple workspaces, or when a workspace is not open, save it globally."));
    m_radioBoxWorkspaceOrGlobal->SetSelection(0);
    
    bSizer10->Add(m_radioBoxWorkspaceOrGlobal, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* bSizer11 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer1->Add(bSizer11, 0, wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_button6 = new Button(this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button6->SetDefault();
    
    bSizer11->Add(m_button6, 0, wxALL, WXC_FROM_DIP(5));
    
    m_button5 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    
    bSizer11->Add(m_button5, 0, wxALL, WXC_FROM_DIP(5));
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_buttonCheckAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveTabGroupDlg::OnCheckAll), NULL, this);
    m_buttonCheckAll->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SaveTabGroupDlg::OnCheckAllUpdateUI), NULL, this);
    m_buttonClearAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveTabGroupDlg::OnClearAll), NULL, this);
    m_buttonClearAll->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SaveTabGroupDlg::OnClearAllUpdateUI), NULL, this);
   
    m_radioBoxWorkspaceOrGlobal->Show(gsgs_Workspace().IsOpen());
}
//--------------------------------------------------------------------------
SaveTabGroupDlg::~SaveTabGroupDlg() 
{
    m_buttonCheckAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveTabGroupDlg::OnCheckAll), NULL, this);
    m_buttonCheckAll->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SaveTabGroupDlg::OnCheckAllUpdateUI), NULL, this);
    m_buttonClearAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveTabGroupDlg::OnClearAll), NULL, this);
    m_buttonClearAll->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SaveTabGroupDlg::OnClearAllUpdateUI), NULL, this);
}
//--------------------------------------------------------------------------
bool SaveTabGroupDlg::GetChoices(wxArrayInt& intArr) const
{
    bool SomeChecked = false;
    for(unsigned int n = 0; n < m_ListTabs->GetCount(); ++n) {
        bool item = m_ListTabs->IsChecked(n);
        intArr.Add(item);
        if(item) {
            SomeChecked = true;
        }
    }

    return SomeChecked;
}
//--------------------------------------------------------------------------
void SaveTabGroupDlg::OnCheckAll(wxCommandEvent& WXUNUSED(event))
{
    if(m_ListTabs == NULL) return;

    for(unsigned int n = 0; n < m_ListTabs->GetCount(); ++n) {
        m_ListTabs->Check(n, true);
    }
}
//--------------------------------------------------------------------------
void SaveTabGroupDlg::OnCheckAllUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_ListTabs && m_ListTabs->GetCount()); }
//--------------------------------------------------------------------------
void SaveTabGroupDlg::OnClearAll(wxCommandEvent& WXUNUSED(event))
{
    if(m_ListTabs == NULL) return;

    for(unsigned int n = 0; n < m_ListTabs->GetCount(); ++n) {
        m_ListTabs->Check(n, false);
    }
}
//--------------------------------------------------------------------------
void SaveTabGroupDlg::OnClearAllUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_ListTabs && m_ListTabs->GetCount()); }
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// RenameSymbol
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
class RenameSymbolData : public wxClientData
{
public:
    CppToken m_token;

public:
    RenameSymbolData(const CppToken& token)
        : m_token(token)
    {
    }
    ~RenameSymbolData() {}
};
//--------------------------------------------------------------------------
RenameSymbol::RenameSymbol(wxWindow* parent, const CppToken::Vec_t& candidates, const CppToken::Vec_t& possCandidates,
                           const wxString& oldname, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer1);
    
    wxBoxSizer* bSizer4 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer1->Add(bSizer4, 0, wxALL|wxEXPAND, 5);
    
    m_staticText31 = gsgs_Style().createStaticText(this, wxID_ANY, _("New Symbol Name:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer4->Add(m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_textCtrlNewName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    bSizer4->Add(m_textCtrlNewName, 1, wxALL, 5);
    
    m_splitter7 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxSP_LIVE_UPDATE|wxSP_NO_XP_THEME|wxSP_3DSASH);
    m_splitter7->SetSashGravity(0.5);
    m_splitter7->SetMinimumPaneSize(10);
    
    bSizer1->Add(m_splitter7, 1, wxALL|wxEXPAND, 5);
    
    m_splitterPage11 = new wxPanel(m_splitter7, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTAB_TRAVERSAL);
    
    wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
    m_splitterPage11->SetSizer(bSizer2);
    
    m_dvListCtrl = new wxDataViewListCtrl(m_splitterPage11, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxDV_ROW_LINES|wxDV_SINGLE);
    
    bSizer2->Add(m_dvListCtrl, 1, wxALL|wxEXPAND, 5);
    
    m_dvListCtrl->AppendToggleColumn(_("?"), wxDATAVIEW_CELL_ACTIVATABLE, -2, wxALIGN_LEFT);
    m_dvListCtrl->AppendTextColumn(_("Location"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT);
    m_dvListCtrl->AppendTextColumn(_("Position"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT);
    wxBoxSizer* boxSizer53 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer2->Add(boxSizer53, 0, wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_buttonCheckAll = new Button(m_splitterPage11, ID_CHECK_ALL, _("&Check All"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer53->Add(m_buttonCheckAll, 0, wxALL|wxEXPAND|wxALIGN_LEFT, 5);
    
    m_buttonUncheckAll = new Button(m_splitterPage11, ID_UNCHECK_ALL, _("&Uncheck All"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer53->Add(m_buttonUncheckAll, 0, wxALL|wxEXPAND|wxALIGN_LEFT, 5);
    
    m_splitterPage15 = new wxPanel(m_splitter7, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTAB_TRAVERSAL);
    m_splitter7->SplitVertically(m_splitterPage11, m_splitterPage15, 0);
    
    wxBoxSizer* boxSizer3 = new wxBoxSizer(wxVERTICAL);
    m_splitterPage15->SetSizer(boxSizer3);
    
    m_preview = new wxStyledTextCtrl(m_splitterPage15, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), 0);
    // Configure the fold margin
    m_preview->SetMarginType     (4, wxSTC_MARGIN_SYMBOL);
    m_preview->SetMarginMask     (4, wxSTC_MASK_FOLDERS);
    m_preview->SetMarginSensitive(4, true);
    m_preview->SetMarginWidth    (4, 0);
    
    // Configure the tracker margin
    m_preview->SetMarginWidth(1, 0);
    
    // Configure the symbol margin
    m_preview->SetMarginType (2, wxSTC_MARGIN_SYMBOL);
    m_preview->SetMarginMask (2, ~(wxSTC_MASK_FOLDERS));
    m_preview->SetMarginWidth(2, 0);
    m_preview->SetMarginSensitive(2, true);
    
    // Configure the line numbers margin
    int m_preview_PixelWidth = 4 + 5 *m_preview->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    m_preview->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_preview->SetMarginWidth(0,m_preview_PixelWidth);
    
    // Configure the line symbol margin
    m_preview->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_preview->SetMarginMask(3, 0);
    m_preview->SetMarginWidth(3,0);
    // Select the lexer
    m_preview->SetLexer(wxSTC_LEX_CPP);
    // Set default font / styles
    m_preview->StyleClearAll();
    m_preview->SetWrapMode(0);
    m_preview->SetIndentationGuides(0);
    m_preview->SetKeyWords(0, wxT(""));
    m_preview->SetKeyWords(1, wxT(""));
    m_preview->SetKeyWords(2, wxT(""));
    m_preview->SetKeyWords(3, wxT(""));
    m_preview->SetKeyWords(4, wxT(""));
    
    boxSizer3->Add(m_preview, 1, wxALL|wxEXPAND, 5);
    
    m_staticLine55 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxLI_HORIZONTAL);
    
    bSizer1->Add(m_staticLine55, 0, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
    
    bSizer1->Add(bSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_buttonOk = new Button(this, wxID_OK, _("&Rename"), wxDefaultPosition, wxSize(-1, -1), 0);
    m_buttonOk->SetDefault();
    
    bSizer3->Add(m_buttonOk, 0, wxALL, 5);
    
    m_buttonCancel = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer3->Add(m_buttonCancel, 0, wxALL, 5);
    
    if(GetParent()) {
        CentreOnParent();
    } else {
        CentreOnScreen();
    }

    // Connect events
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(RenameSymbol::OnSelection), NULL, this);
    m_buttonCheckAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RenameSymbol::OnCheckAll), NULL, this);
    m_buttonUncheckAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RenameSymbol::OnUncheckAll), NULL, this);
    m_buttonOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RenameSymbol::OnButtonOK), NULL, this);
    
    m_preview->SetReadOnly(true);
    gsgs_EditorConfig().GetLexer("C++")->Apply(m_preview, true);
    m_tokens.clear();

    CppToken::Vec_t::const_iterator iter = candidates.begin();
    for(; iter != candidates.end(); ++iter) {
        AddMatch(*iter, true);
        m_tokens.push_back(*iter);
    }

    iter = possCandidates.begin();
    for(; iter != possCandidates.end(); iter++) {
        AddMatch(*iter, false);
        m_tokens.push_back(*iter);
    }

    if(m_tokens.empty() == false) { DoSelectFile(m_tokens.at((size_t)0)); }

    m_textCtrlNewName->SetValue(oldname);
    m_textCtrlNewName->SetFocus();
    ::SetTLWindowBestSizeAndPosition(this);
}
//--------------------------------------------------------------------------
void RenameSymbol::AddMatch(const CppToken& token, bool check)
{
    wxString relativeTo = gsgs_CppWorker().GetFileName().GetPath();
    wxFileName fn(token.getFilename());
    fn.MakeRelativeTo(relativeTo);

    wxVector<wxVariant> cols;
    cols.push_back(check);
    cols.push_back(fn.GetFullPath());
    cols.push_back(wxString() << token.getOffset());
    m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new RenameSymbolData(token));
}
//--------------------------------------------------------------------------
void RenameSymbol::OnButtonOK(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(!IsValidCppIndetifier(m_textCtrlNewName->GetValue())) {
        wxMessageBox(_("Invalid C/C++ symbol name"), _("光栅故事"), wxICON_WARNING | wxOK);
        return;
    }

    EndModal(wxID_OK);
}
//--------------------------------------------------------------------------
void RenameSymbol::GetMatches(CppToken::Vec_t& matches)
{
    wxVariant v;
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->GetValue(v, i, 0);

        if(v.GetBool()) {
            matches.push_back(((RenameSymbolData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i)))->m_token);
        }
    }
}
//--------------------------------------------------------------------------
void RenameSymbol::DoSelectFile(const CppToken& token)
{
    m_preview->SetReadOnly(false);

    wxString file_name(token.getFilename());
    if(m_filename != file_name) {
        m_preview->LoadFile(file_name);
        m_filename = file_name;
    }

    m_preview->ClearSelections();
    m_preview->SetSelection(token.getOffset(), token.getOffset() + token.getName().length());
    int line_number = m_preview->LineFromPosition(token.getOffset());

    int linesOnScreen = m_preview->LinesOnScreen();
    // To place our line in the middle, the first visible line should be
    // the: line - (linesOnScreen / 2)
    int firstVisibleLine = line_number - (linesOnScreen / 2);
    if(firstVisibleLine < 0) { firstVisibleLine = 0; }
    m_preview->EnsureVisible(firstVisibleLine);
    m_preview->SetFirstVisibleLine(firstVisibleLine);
    m_preview->SetReadOnly(true);
}
//--------------------------------------------------------------------------
void RenameSymbol::OnSelection(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());

    RenameSymbolData* data = (RenameSymbolData*)m_dvListCtrl->GetItemData(event.GetItem());
    if(data) { DoSelectFile(data->m_token); }
}
//--------------------------------------------------------------------------
RenameSymbol::~RenameSymbol() 
{
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(RenameSymbol::OnSelection), NULL, this);
    m_buttonCheckAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RenameSymbol::OnCheckAll), NULL, this);
    m_buttonUncheckAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RenameSymbol::OnUncheckAll), NULL, this);
    m_buttonOk->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RenameSymbol::OnButtonOK), NULL, this);
}
//--------------------------------------------------------------------------
void RenameSymbol::OnCheckAll(wxCommandEvent& event)
{
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetValue(true, i, 0);
    }
}
//--------------------------------------------------------------------------
void RenameSymbol::OnUncheckAll(wxCommandEvent& event)
{
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetValue(false, i, 0);
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// PSCustomBuildPage
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// ---------------------------------------------------------
class VirtualDirectoryTree
{
    typedef std::deque<VirtualDirectoryTree*> dBranches;

public:
    VirtualDirectoryTree()
        : m_parent(NULL)
    {
    }
    VirtualDirectoryTree(const VirtualDirectoryTree* parent, const wxString& displayname, const wxString& vdPath)
        : m_parent(parent)
        , m_displayName(displayname)
        , m_vdPath(vdPath)
    {
    }
    void BuildTree(const wxString& projName);
    wxString FindBestMatchVDir(const wxString& path, const wxString& ext) const;
    wxString GetDisplayname() const { return m_displayName; }
    wxString GetVPath() const { return m_vdPath; }

protected:
    VirtualDirectoryTree* FindParent(const wxString& vdChildPath); // Used by root in the initial construction of the tree
    void StoreChild(const wxString& displayname, const wxString& vdPath);

    dBranches m_children;
    const VirtualDirectoryTree* m_parent;
    wxString m_displayName;
    wxString m_vdPath;
};
// ---------------------------------------------------------
bool IsSourceVD(const wxString& name)
{
    return (name == "src" || name == "source" || name == "cpp" || name == "c" || name == "cc");
}

bool IsHeaderVD(const wxString& name)
{
    return (name == "include" || name == "includes" || name == "header" || name == "headers" || name == "hpp" ||
            name == "h");
}

bool IsResourceVD(const wxString& name) { return (name == "rc" || name == "resource" || name == "resources"); }

// ---------------------------------------------------------

class ReconcileFileItemData : public wxClientData
{
    wxString m_filename;
    wxString m_virtualFolder;

public:
    ReconcileFileItemData() {}
    ReconcileFileItemData(const wxString& filename, const wxString& vd)
        : m_filename(filename)
        , m_virtualFolder(vd)
    {
    }
    virtual ~ReconcileFileItemData() {}
    void SetFilename(const wxString& filename) { this->m_filename = filename; }
    void SetVirtualFolder(const wxString& virtualFolder) { this->m_virtualFolder = virtualFolder; }
    const wxString& GetFilename() const { return m_filename; }
    const wxString& GetVirtualFolder() const { return m_virtualFolder; }
};
// ---------------------------------------------------------
// Declare the bitmap loading function
extern void wxCrafterjdrz3kInitBitmapResources();

static bool bBitmapLoaded3 = false;
// ---------------------------------------------------------
// ---------------------------------------------------------
// ReconcileProjectDlg
// ---------------------------------------------------------
// ---------------------------------------------------------
ReconcileProjectDlg::ReconcileProjectDlg(wxWindow* parent, const wxString& projname, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_projname(projname)
    , m_projectModified(false)
{
    if ( !bBitmapLoaded3 ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafterjdrz3kInitBitmapResources();
        bBitmapLoaded3 = true;
    }
    
    wxBoxSizer* boxSizer83 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer83);
    
    m_notebook214 = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxBK_DEFAULT);
    m_notebook214->SetName(wxT("m_notebook214"));
    
    boxSizer83->Add(m_notebook214, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_panel220 = new wxPanel(m_notebook214, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook214, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_notebook214->AddPage(m_panel220, _("New Files"), true);
    
    wxBoxSizer* boxSizer224 = new wxBoxSizer(wxVERTICAL);
    m_panel220->SetSizer(boxSizer224);
    
    m_banner270 = new wxBannerWindow(m_panel220, wxID_ANY, wxTOP, wxDefaultPosition, wxDLG_UNIT(m_panel220, wxSize(-1,-1)), wxBORDER_THEME);
    m_banner270->SetBitmap(wxNullBitmap);
    m_banner270->SetText(_("New Files"), _("Select files from the left pane and add them to the project by clicking on the right arrow button"));
    m_banner270->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE), wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    m_banner270->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    
    boxSizer224->Add(m_banner270, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer276 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer224->Add(boxSizer276, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_splitter = new wxSplitterWindow(m_panel220, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel220, wxSize(-1,-1)), wxSP_LIVE_UPDATE|wxSP_3DSASH);
    m_splitter->SetSashGravity(0.5);
    m_splitter->SetMinimumPaneSize(10);
    
    boxSizer276->Add(m_splitter, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_splitterPage1 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    
    wxBoxSizer* boxSizer248 = new wxBoxSizer(wxHORIZONTAL);
    m_splitterPage1->SetSizer(boxSizer248);
    
    wxBoxSizer* boxSizer179 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer248->Add(boxSizer179, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer183 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer179->Add(boxSizer183, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_dvListCtrl1Unassigned = new wxDataViewListCtrl(m_splitterPage1, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitterPage1, wxSize(-1,-1)), wxDV_ROW_LINES|wxDV_MULTIPLE);
    m_dvListCtrl1Unassigned->SetToolTip(_("These files have not yet been assigned a Virtual Directory. You can do this yourself by selecting one or more files and clicking the 'Forward' arrow button. A Virtual Directory selector will then appear. After your choice the selection(s) will be moved to the right-hand pane.\nAlternatively click the 'Wizard' button for best-guess auto-allocation."));
    
    boxSizer183->Add(m_dvListCtrl1Unassigned, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_dvListCtrl1Unassigned->AppendIconTextColumn(_("Unassigned files:"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(300), wxALIGN_LEFT);
    wxBoxSizer* boxSizer192 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer183->Add(boxSizer192, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer194 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer248->Add(boxSizer194, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    boxSizer194->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));
    
    m_bmpButtonAdd = new wxBitmapButton(m_splitterPage1, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("forward")), wxDefaultPosition, wxDLG_UNIT(m_splitterPage1, wxSize(-1,-1)), wxBU_AUTODRAW);
    m_bmpButtonAdd->SetToolTip(_("Add selected files"));
    
    boxSizer194->Add(m_bmpButtonAdd, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_bmpButtonRemove = new wxBitmapButton(m_splitterPage1, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("back")), wxDefaultPosition, wxDLG_UNIT(m_splitterPage1, wxSize(-1,-1)), wxBU_AUTODRAW);
    m_bmpButtonRemove->SetToolTip(_("Remove selected files"));
    
    boxSizer194->Add(m_bmpButtonRemove, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_bmpButtonAutoAdd = new wxBitmapButton(m_splitterPage1, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("wand")), wxDefaultPosition, wxDLG_UNIT(m_splitterPage1, wxSize(-1,-1)), wxBU_AUTODRAW);
    m_bmpButtonAutoAdd->SetToolTip(_("Wherever possible, automatically allocate files to the appropriate virtual directory"));
    
    boxSizer194->Add(m_bmpButtonAutoAdd, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    boxSizer194->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));
    
    m_splitterPage2 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_splitter->SplitVertically(m_splitterPage1, m_splitterPage2, 0);
    
    wxBoxSizer* boxSizer250 = new wxBoxSizer(wxVERTICAL);
    m_splitterPage2->SetSizer(boxSizer250);
    
    wxBoxSizer* boxSizer181 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer250->Add(boxSizer181, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_dataviewAssigned = new wxDataViewCtrl(m_splitterPage2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitterPage2, wxSize(-1,-1)), wxDV_ROW_LINES|wxDV_MULTIPLE);
    m_dataviewAssigned->SetToolTip(_("Each file here has been assigned a Virtual Directory. If you're happy with the choice, select the file and click 'Apply'. Otherwise select the file and use the 'back' button to return it to the Unassigned Files section."));
    
    m_dataviewAssignedModel = new AssignedFilesModel;
    m_dataviewAssignedModel->SetColCount( 2 );
    m_dataviewAssigned->AssociateModel(m_dataviewAssignedModel.get() );
    
    boxSizer181->Add(m_dataviewAssigned, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_dataviewAssigned->AppendIconTextColumn(_("Assigned files:"), m_dataviewAssigned->GetColumnCount(), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(300), wxALIGN_LEFT);
    m_dataviewAssigned->AppendTextColumn(_("Virtual Folder"), m_dataviewAssigned->GetColumnCount(), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT);
    wxBoxSizer* boxSizer278 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer276->Add(boxSizer278, 0, wxEXPAND, WXC_FROM_DIP(5));
    
    m_button274 = new Button(m_panel220, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDLG_UNIT(m_panel220, wxSize(-1,-1)), 0);
    
    boxSizer278->Add(m_button274, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button280 = new Button(m_panel220, wxID_ANY, _("A&pply All"), wxDefaultPosition, wxDLG_UNIT(m_panel220, wxSize(-1,-1)), 0);
    
    boxSizer278->Add(m_button280, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_panel222 = new wxPanel(m_notebook214, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook214, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_notebook214->AddPage(m_panel222, _("Stale Files"), false);
    
    wxBoxSizer* boxSizer226 = new wxBoxSizer(wxVERTICAL);
    m_panel222->SetSizer(boxSizer226);
    
    m_banner272 = new wxBannerWindow(m_panel222, wxID_ANY, wxTOP, wxDefaultPosition, wxDLG_UNIT(m_panel222, wxSize(-1,-1)), wxBORDER_THEME);
    m_banner272->SetBitmap(wxNullBitmap);
    m_banner272->SetText(_("Stale Files"), _("The list below contains files that exist in the project but not on the file system"));
    m_banner272->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE), wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    m_banner272->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    
    boxSizer226->Add(m_banner272, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer262 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer226->Add(boxSizer262, 1, wxEXPAND, WXC_FROM_DIP(5));
    
    m_dataviewStaleFiles = new wxDataViewCtrl(m_panel222, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel222, wxSize(-1,-1)), wxDV_NO_HEADER|wxDV_ROW_LINES|wxDV_MULTIPLE);
    m_dataviewStaleFiles->SetToolTip(_("The files listed below are contained in the project, but no longer exist in reality. You can select individual items and delete them from the project, or use the Delete All button."));
    
    m_dataviewStaleFilesModel = new StaleFilesModel;
    m_dataviewStaleFilesModel->SetColCount( 1 );
    m_dataviewStaleFiles->AssociateModel(m_dataviewStaleFilesModel.get() );
    
    boxSizer262->Add(m_dataviewStaleFiles, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_dataviewStaleFiles->AppendIconTextColumn(_("File path"), m_dataviewStaleFiles->GetColumnCount(), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(600), wxALIGN_LEFT);
    wxBoxSizer* boxSizer264 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer262->Add(boxSizer264, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button266 = new Button(m_panel222, wxID_DELETE, _("&Delete"), wxDefaultPosition, wxDLG_UNIT(m_panel222, wxSize(-1,-1)), 0);
    
    boxSizer264->Add(m_button266, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button282 = new Button(m_panel222, wxID_ANY, _("Delete &All"), wxDefaultPosition, wxDLG_UNIT(m_panel222, wxSize(-1,-1)), 0);
    
    boxSizer264->Add(m_button282, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer252 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer83->Add(boxSizer252, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_button258 = new Button(this, wxID_CLOSE, _("&Close"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_button258->SetDefault();
    
    boxSizer252->Add(m_button258, 0, wxALL, WXC_FROM_DIP(5));
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_bmpButtonAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnAddFile), NULL, this);
    m_bmpButtonAdd->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnAddFileUI), NULL, this);
    m_bmpButtonRemove->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnUndoSelectedFiles), NULL, this);
    m_bmpButtonRemove->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnUndoSelectedFilesUI), NULL, this);
    m_bmpButtonAutoAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnAutoSuggest), NULL, this);
    m_bmpButtonAutoAdd->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnAutoAssignUI), NULL, this);
    m_button274->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnApply), NULL, this);
    m_button274->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnApplyUI), NULL, this);
    m_button280->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnApply), NULL, this);
    m_button280->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnApplyAllUI), NULL, this);
    m_button266->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnDeleteStaleFiles), NULL, this);
    m_button266->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnDeleteStaleFilesUI), NULL, this);
    m_button282->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnDeleteStaleFiles), NULL, this);
    m_button282->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnDeleteAllStaleFilesUI), NULL, this);
    m_button258->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnClose), NULL, this);
   
    m_dvListCtrl1Unassigned->Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
                                  wxDataViewEventHandler(ReconcileProjectDlg::OnDVLCContextMenu), this);
}
// ---------------------------------------------------------
ReconcileProjectDlg::~ReconcileProjectDlg() 
{
    m_bmpButtonAdd->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnAddFile), NULL, this);
    m_bmpButtonAdd->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnAddFileUI), NULL, this);
    m_bmpButtonRemove->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnUndoSelectedFiles), NULL, this);
    m_bmpButtonRemove->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnUndoSelectedFilesUI), NULL, this);
    m_bmpButtonAutoAdd->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnAutoSuggest), NULL, this);
    m_bmpButtonAutoAdd->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnAutoAssignUI), NULL, this);
    m_button274->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnApply), NULL, this);
    m_button274->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnApplyUI), NULL, this);
    m_button280->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnApply), NULL, this);
    m_button280->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnApplyAllUI), NULL, this);
    m_button266->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnDeleteStaleFiles), NULL, this);
    m_button266->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnDeleteStaleFilesUI), NULL, this);
    m_button282->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnDeleteStaleFiles), NULL, this);
    m_button282->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectDlg::OnDeleteAllStaleFilesUI), NULL, this);
    m_button258->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectDlg::OnClose), NULL, this);
}
// ---------------------------------------------------------
bool ReconcileProjectDlg::LoadData()
{
    ReconcileProjectFiletypesDlg dlg(gsgs_Frame(), m_projname);
    dlg.SetData();
    if(dlg.ShowModal() != wxID_OK) { return false; }
    wxString toplevelDir, filespec, ignorefilespec;
    wxArrayString excludeFolders, regexes;
    dlg.GetData(toplevelDir, filespec, ignorefilespec, excludeFolders, regexes);
    m_regexes = regexes;

    wxDir dir(toplevelDir);
    if(!dir.IsOpened()) { return false; }

    m_toplevelDir = toplevelDir;

    m_allfiles.clear();
    {
        wxBusyInfo wait("Searching for files...", this);
        wxSafeYield();

        FileScan scanner;
        std::vector<wxString> filesOutput;
        gsgsStringSet excludeFoldersSet;
        std::for_each(excludeFolders.begin(), excludeFolders.end(),
                      [&](const wxString& folder) { excludeFoldersSet.insert(folder); });
        if(scanner.Scan(toplevelDir, filesOutput, filespec, ignorefilespec, excludeFoldersSet)) {
            m_allfiles.insert(filesOutput.begin(), filesOutput.end());
            DoFindFiles();
        }
    }

    if(m_newfiles.empty() && m_stalefiles.empty()) {
        wxMessageBox(_("No new or stale files found. The project is up-to-date"), _("光栅故事"),
                     wxICON_INFORMATION | wxOK, this);
        return false;
    }

    DistributeFiles(false);
    return true;
}
// ---------------------------------------------------------
void ReconcileProjectDlg::DistributeFiles(bool usingAutoallocate)
{
    VirtualDirectoryTree vdTree;
    wxArrayString selectedFiles;
    bool onlySelections;

    if(usingAutoallocate) {
        vdTree.BuildTree(m_projname);
        // If we're autoallocating, cache the current selections as
        // 1) we only want to operate on those, and 2) m_dvListCtrl1Unassigned is about to be cleared!
        wxDataViewItemArray selecteditems;
        m_dvListCtrl1Unassigned->GetSelections(selecteditems);
        for(size_t i = 0; i < selecteditems.GetCount(); ++i) {
            wxVariant v;
            m_dvListCtrl1Unassigned->GetValue(v, m_dvListCtrl1Unassigned->GetStore()->GetRow(selecteditems.Item(i)), 0);
            wxDataViewIconText iv;
            if(!v.IsNull()) {
                iv << v;
                selectedFiles.Add(iv.GetText());
            }
        }
        onlySelections = !selectedFiles.empty();
    }

    //---------------------------------------------------------
    // populate the 'new files' tab
    //---------------------------------------------------------

    {
        m_dataviewAssignedModel->Clear();
        m_dvListCtrl1Unassigned->DeleteAllItems();

        std::vector<wxString> newFilesV;
        newFilesV.reserve(m_newfiles.size());
        newFilesV.insert(newFilesV.end(), m_newfiles.begin(), m_newfiles.end());
        std::sort(newFilesV.begin(), newFilesV.end()); // Sort the files
        for(size_t i = 0; i < newFilesV.size(); ++i) {
            const wxString& filename = newFilesV[i];
            wxFileName fn(filename);
            fn.MakeRelativeTo(m_toplevelDir);

            // Even without auto-allocation, apply any regex as that'll be most likely to reflect the user's choice
            bool bFileAllocated = false;
            for(size_t i = 0; i < m_regexes.GetCount(); ++i) {
                wxString virtualFolder(m_regexes.Item(i).BeforeFirst('|'));
                wxRegEx regex(m_regexes.Item(i).AfterFirst('|'));
                if(regex.IsValid() && regex.Matches(filename)) {
                    wxVector<wxVariant> cols;
                    cols.push_back(::MakeIconText(fn.GetFullPath(), GetBitmap(filename)));
                    cols.push_back(virtualFolder);
                    ReconcileFileItemData* data = new ReconcileFileItemData(filename, virtualFolder);
                    m_dataviewAssignedModel->AppendItem(wxDataViewItem(0), cols, data);
                    bFileAllocated = true;
                    break;
                }
            }

            if(usingAutoallocate) {
                bool attemptAllocation(true);
                // First see if we should only process selected files and, if so, was this file selected
                if(onlySelections) {
                    if(selectedFiles.Index(fn.GetFullPath()) == wxNOT_FOUND) { attemptAllocation = false; }
                }

                if(attemptAllocation) {
                    wxString virtualFolder = vdTree.FindBestMatchVDir(fn.GetPath(), fn.GetExt());
                    if (virtualFolder.empty() && fn.GetPath().empty()) {
                        virtualFolder = m_projname; // This must be a top-level file
                    }
                    if(!virtualFolder.empty()) {
                        wxVector<wxVariant> cols;
                        cols.push_back(::MakeIconText(fn.GetFullPath(), GetBitmap(filename)));
                        cols.push_back(virtualFolder);
                        ReconcileFileItemData* data = new ReconcileFileItemData(filename, virtualFolder);
                        m_dataviewAssignedModel->AppendItem(wxDataViewItem(0), cols, data);
                        bFileAllocated = true;
                    }
                }
            }

            if(!bFileAllocated) {
                wxVector<wxVariant> cols;
                cols.push_back(::MakeIconText(fn.GetFullPath(), GetBitmap(filename)));
                m_dvListCtrl1Unassigned->AppendItem(cols, (wxUIntPtr)NULL);
            }
        }
    }
    //---------------------------------------------------------
    // populate the 'stale files' tab
    //---------------------------------------------------------
    {
        std::vector<clProjectFile::Ptr_t> staleFiles;
        staleFiles.reserve(m_stalefiles.size());
        std::for_each(m_stalefiles.begin(), m_stalefiles.end(),
                      [&](const Project::FilesMap_t::value_type& vt) { staleFiles.push_back(vt.second); });
        std::sort(staleFiles.begin(), staleFiles.end(), [&](clProjectFile::Ptr_t a, clProjectFile::Ptr_t b) {
            return a->GetFilename() < b->GetFilename();
        }); // Sort the files

        m_dataviewStaleFilesModel->Clear();

        std::for_each(staleFiles.begin(), staleFiles.end(), [&](clProjectFile::Ptr_t file) {
            wxVector<wxVariant> cols;
            cols.push_back(::MakeIconText(file->GetFilename(), GetBitmap(file->GetFilename())));
            m_dataviewStaleFilesModel->AppendItem(
                wxDataViewItem(0), cols, new ReconcileFileItemData(file->GetFilename(), file->GetVirtualFolder()));
        });
    }
}
// ---------------------------------------------------------
wxArrayString ReconcileProjectDlg::RemoveStaleFiles(const wxArrayString& StaleFiles) const
{
    wxArrayString removals;

    ProjectPtr proj = gsgs_Manager()->GetProject(m_projname);
    wxCHECK_MSG(proj, removals, "Can't find a Project with the supplied name");

    for(size_t n = 0; n < StaleFiles.GetCount(); ++n) {
        // Reconstruct the VD path in projectname:foo:bar format
        int index = StaleFiles[n].Find(": ");
        wxCHECK_MSG(index != wxNOT_FOUND, removals, "Badly-formed stalefile string");
        wxString vdPath = StaleFiles[n].Left(index);
        wxString filepath = StaleFiles[n].Mid(index + 2);

        if(proj->RemoveFile(filepath, vdPath)) { removals.Add(StaleFiles[n]); }
    }

    return removals;
}
// ---------------------------------------------------------
wxArrayString ReconcileProjectDlg::AddMissingFiles(const wxArrayString& files, const wxString& vdPath)
{
    wxArrayString additions;

    ProjectPtr proj = gsgs_Manager()->GetProject(m_projname);
    wxCHECK_MSG(proj, additions, "Can't find a Project with the supplied name");

    wxString VD = vdPath;
    if(VD.empty()) {
        // If we were called from the root panel (so the user is trying to add unallocated files, or all files at once)
        // we need to know which VD to use
        VirtualDirectorySelectorDlg selector(this, &gsgs_CppWorker(), "", m_projname);
        selector.SetText("Please choose the Virtual Directory to which to add the files");
        if(selector.ShowModal() == wxID_OK) {
            VD = selector.GetVirtualDirectoryPath();
        } else {
            return additions;
        }
    }

    VD = VD.AfterFirst(':'); // Remove the projectname

    for(size_t n = 0; n < files.GetCount(); ++n) {
        if(proj->FastAddFile(files[n], VD)) { additions.Add(files[n]); }
    }

    return additions;
}
// ---------------------------------------------------------
void ReconcileProjectDlg::DoFindFiles()
{
    m_stalefiles.clear();
    m_newfiles.clear();

    ProjectPtr proj = gsgs_Manager()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    // get list of files from the project
    const Project::FilesMap_t& files = proj->GetFiles();
    gsgsStringSet projectfilesSet;
    std::for_each(files.begin(), files.end(), [&](const Project::FilesMap_t::value_type& vt) {
        projectfilesSet.insert(vt.first);
        if(!wxFileName::FileExists(vt.second->GetFilename())) {
            m_stalefiles.insert({ vt.second->GetFilename(), vt.second });
        }
    });

    std::for_each(m_allfiles.begin(), m_allfiles.end(), [&](const wxString& file) {
        if(projectfilesSet.count(file) == 0) { m_newfiles.insert(file); }
    });
}
// ---------------------------------------------------------
wxBitmap ReconcileProjectDlg::GetBitmap(const wxString& filename) const
{
    return gsgs_Image().GetBitmap(filename);
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnAddFile(wxCommandEvent& event)
{
    wxString suggestedPath, suggestedName;
    bool guessed = GuessNewVirtualDirName(suggestedPath, suggestedName);
    VirtualDirectorySelectorDlg selector(this, &gsgs_CppWorker(), suggestedPath, m_projname);
    if(guessed && !suggestedPath.empty()) {
        selector.SelectPath(m_projname + ':' + suggestedPath);
    } else {
        selector.SelectPath(m_projname);  // Either a top-level file, or a top-level dir that's not yet in the VDir tree
    }
    selector.SetSuggestedName(suggestedName);

    if(selector.ShowModal() == wxID_OK) {
        wxString vd = selector.GetVirtualDirectoryPath();
        wxDataViewItemArray items;
        m_dvListCtrl1Unassigned->GetSelections(items);

        for(size_t i = 0; i < items.GetCount(); ++i) {
            wxVariant v;
            m_dvListCtrl1Unassigned->GetValue(v, m_dvListCtrl1Unassigned->GetStore()->GetRow(items.Item(i)), 0);

            wxString path;
            wxDataViewIconText iv;
            if(!v.IsNull()) {
                iv << v;
                path = iv.GetText();
            }

            wxFileName fn(path);
            fn.MakeAbsolute(m_toplevelDir);

            wxVector<wxVariant> cols;
            cols.push_back(::MakeIconText(path, GetBitmap(path)));
            cols.push_back(vd);
            m_dataviewAssignedModel->AppendItem(wxDataViewItem(0), cols,
                                                new ReconcileFileItemData(fn.GetFullPath(), vd));
            m_dvListCtrl1Unassigned->DeleteItem(m_dvListCtrl1Unassigned->GetStore()->GetRow(items.Item(i)));
        }
    }
}
// ---------------------------------------------------------
bool ReconcileProjectDlg::GuessNewVirtualDirName(wxString& suggestedPath, wxString& suggestedName) const
{
    wxDataViewItemArray items;
    m_dvListCtrl1Unassigned->GetSelections(items);
    if(!items.GetCount()) { return false; }

    // Test only the first item. For this to be useful, all the selections must have the same destination anyway
    wxVariant v;
    m_dvListCtrl1Unassigned->GetValue(v, m_dvListCtrl1Unassigned->GetStore()->GetRow(items.Item(0)), 0);

    wxString path;
    wxDataViewIconText iv;
    if(!v.IsNull()) {
        iv << v;
        path = iv.GetText();
    }

    wxFileName fn(path);
    wxString pathSegments(fn.GetPath());
    pathSegments.Replace(wxFILE_SEP_PATH, ":");

    if (pathSegments.empty()) { return true; } // This must be a top-level file

    VirtualDirectoryTree vdTree;
    vdTree.BuildTree(m_projname);
    do {
        wxString virtualFolder = vdTree.FindBestMatchVDir(path, fn.GetExt());
        if(!virtualFolder.empty()) {
            suggestedPath = path;
            suggestedPath.Replace(wxFILE_SEP_PATH, ":");
            suggestedName = pathSegments.Mid(suggestedPath.Len()+1);
            return true;
        }

        path=path.BeforeLast(wxFILE_SEP_PATH);
    } while(!path.empty());

    suggestedName = pathSegments; // This may be a top-level dir not in the VDir tree. pathSegments holds the likely name
    return false;
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnAddFileUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrl1Unassigned->GetSelectedItemsCount());
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnAutoAssignUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrl1Unassigned->GetItemCount());
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnAutoSuggest(wxCommandEvent& event) { DistributeFiles(true); }
// ---------------------------------------------------------
void ReconcileProjectDlg::OnUndoSelectedFiles(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dataviewAssigned->GetSelections(items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxVariant v;
        ReconcileFileItemData* data =
            dynamic_cast<ReconcileFileItemData*>(m_dataviewAssignedModel->GetClientObject(items.Item(i)));
        if(data) {
            wxFileName fn(data->GetFilename());
            fn.MakeRelativeTo(m_toplevelDir);

            wxVector<wxVariant> cols;
            cols.push_back(::MakeIconText(fn.GetFullPath(), GetBitmap(fn.GetFullName())));
            m_dvListCtrl1Unassigned->AppendItem(cols, (wxUIntPtr)NULL);
        }
    }

    // get the list of items
    wxArrayString allfiles;
    for(int i = 0; i < m_dvListCtrl1Unassigned->GetItemCount(); ++i) {
        wxVariant v;
        m_dvListCtrl1Unassigned->GetValue(v, i, 0);
        wxDataViewIconText it;
        it << v;
        allfiles.Add(it.GetText());
    }

    m_dataviewAssignedModel->DeleteItems(wxDataViewItem(0), items);

    // Could not find a nicer way of doing this, but
    // we want the files to be sorted again
    m_dvListCtrl1Unassigned->DeleteAllItems();

    std::sort(allfiles.begin(), allfiles.end());
    for(size_t i = 0; i < allfiles.GetCount(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(::MakeIconText(allfiles.Item(i), GetBitmap(allfiles.Item(i))));
        m_dvListCtrl1Unassigned->AppendItem(cols, (wxUIntPtr)NULL);
    }
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnUndoSelectedFilesUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dataviewAssigned->GetSelectedItemsCount());
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnDeleteStaleFiles(wxCommandEvent& event)
{
    ProjectPtr proj = gsgs_Manager()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    wxDataViewItemArray items;
    if(event.GetId() == wxID_DELETE) {
        m_dataviewStaleFiles->GetSelections(items);
    } else {
        m_dataviewStaleFilesModel->GetChildren(wxDataViewItem(0), items);
    }

    proj->BeginTranscation();
    for(size_t i = 0; i < items.GetCount(); ++i) {
        ReconcileFileItemData* data =
            dynamic_cast<ReconcileFileItemData*>(m_dataviewStaleFilesModel->GetClientObject(items.Item(i)));
        if(data) { proj->RemoveFile(data->GetFilename(), data->GetVirtualFolder()); }
        m_projectModified = true;
    }
    proj->CommitTranscation();
    m_dataviewStaleFilesModel->DeleteItems(wxDataViewItem(0), items);
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnDeleteStaleFilesUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dataviewStaleFiles->GetSelectedItemsCount());
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnDeleteAllStaleFilesUI(wxUpdateUIEvent& event)
{
    wxDataViewItemArray items;
    event.Enable(m_dataviewStaleFilesModel->GetChildren(wxDataViewItem(0), items) > 0);
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnClose(wxCommandEvent& event)
{
    // reload the workspace
    if(m_projectModified) {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
        evt.SetEventObject(gsgs_Frame());
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(evt);
    }
    EndModal(wxID_CLOSE);
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnApply(wxCommandEvent& event)
{
    // get the list of files to add to the project
    wxDataViewItemArray items;
    if(event.GetId() == wxID_APPLY) {
        m_dataviewAssigned->GetSelections(items);
    } else {
        m_dataviewAssignedModel->GetChildren(wxDataViewItem(0), items);
    }

    // virtual folder to file name
    gsgsStringSet vds;
    StringMultimap_t filesToAdd;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        ReconcileFileItemData* data =
            dynamic_cast<ReconcileFileItemData*>(m_dataviewAssignedModel->GetClientObject(items.Item(i)));
        if(data) {
            filesToAdd.insert(std::make_pair(data->GetVirtualFolder(), data->GetFilename()));
            vds.insert(data->GetVirtualFolder());
        }
    }

    gsgsStringSet::const_iterator iter = vds.begin();
    for(; iter != vds.end(); ++iter) {
        std::pair<StringMultimap_t::iterator, StringMultimap_t::iterator> range = filesToAdd.equal_range(*iter);
        StringMultimap_t::iterator from = range.first;
        wxArrayString vdFiles;
        for(; from != range.second; ++from) {
            vdFiles.Add(from->second);
        }
        wxArrayString additions = AddMissingFiles(vdFiles, *iter);

        if(additions.GetCount()) { m_projectModified = true; }
        // We must also remove the processed files from m_newfiles, otherwise a rerun of the wizard will offer them for
        // insertion again
        for(size_t n = 0; n < additions.GetCount(); ++n) {
            m_newfiles.erase(additions.Item(n));
        }
    }
    m_dataviewAssignedModel->DeleteItems(wxDataViewItem(0), items);
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnApplyUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dataviewAssigned->GetSelectedItemsCount());
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnApplyAllUI(wxUpdateUIEvent& event)
{
    wxDataViewItemArray items;
    event.Enable(m_dataviewAssignedModel->GetChildren(wxDataViewItem(0), items) > 0);
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnDVLCContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(wxID_DELETE);
    menu.Connect(wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(ReconcileProjectDlg::OnDeleteSelectedNewFiles), NULL, this);
    m_dvListCtrl1Unassigned->PopupMenu(&menu);
}
// ---------------------------------------------------------
void ReconcileProjectDlg::OnDeleteSelectedNewFiles(wxCommandEvent& e)
{
    wxDataViewItemArray items;
    m_dvListCtrl1Unassigned->GetSelections(items);
    if(items.IsEmpty()) return;

    wxString msg;
    if(items.GetCount() > 1) {
        msg = wxString::Format(_("Delete the %i selected files from the filesystem?"), (int)items.GetCount());
    } else {
        msg = wxString::Format(_("Delete the selected file from the filesystem?"));
    }

    if(::wxMessageBox(msg, "gsgs", wxICON_WARNING | wxYES_NO, this) != wxYES) { return; }

    int successes(0);
    for(size_t n = 0; n < items.GetCount(); ++n) {
        wxVariant v;
        int row = m_dvListCtrl1Unassigned->GetStore()->GetRow(items.Item(n));
        m_dvListCtrl1Unassigned->GetValue(v, row, 0);
        if(v.IsNull()) { continue; }

        wxDataViewIconText iv;
        iv << v;
        wxFileName fn(iv.GetText());
        fn.MakeAbsolute(m_toplevelDir);

        wxLogNull NoAnnoyingFileSystemMessages;
        if(gsgsRemoveFile(fn.GetFullPath())) {
            m_dvListCtrl1Unassigned->DeleteItem(row);
            ++successes;
        }
    }
    gsgs_Frame()->GetMainStatusBar()->SetMessage(wxString::Format(_("%i file(s) successfully deleted"), successes));
}
// ---------------------------------------------------------
// ---------------------------------------------------------
// ReconcileProjectFiletypesDlg
// ---------------------------------------------------------
// ---------------------------------------------------------
ReconcileProjectFiletypesDlg::ReconcileProjectFiletypesDlg(wxWindow* parent, const wxString& projname, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_projname(projname)
{
    if ( !bBitmapLoaded3 ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafterjdrz3kInitBitmapResources();
        bBitmapLoaded3 = true;
    }
    
    wxBoxSizer* boxSizer114 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer114);
    
    wxBoxSizer* boxSizer160 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer114->Add(boxSizer160, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText116 = gsgs_Style().createStaticText(this, wxID_ANY, _("Look for files starting with this directory:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer160->Add(m_staticText116, 0, wxALL, WXC_FROM_DIP(5));
    
    m_dirPickerToplevel = new wxDirPickerCtrl(this, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxDIRP_DEFAULT_STYLE|wxDIRP_USE_TEXTCTRL|wxDIRP_DIR_MUST_EXIST);
    m_dirPickerToplevel->SetFocus();
    
    boxSizer160->Add(m_dirPickerToplevel, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer16016 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer114->Add(boxSizer16016, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText11617 = gsgs_Style().createStaticText(this, wxID_ANY, _("Extensions to consider when looking for missing files:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer16016->Add(m_staticText11617, 0, wxALL, WXC_FROM_DIP(5));
    
    m_textExtensions = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    m_textExtensions->SetToolTip(_("A semi-colon separated list of extensions e.g. cpp;h;xrc\nIf you really want to find all possible files, just add *"));

    boxSizer16016->Add(m_textExtensions, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxFlexGridSizer* flexGridSizer11517115 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer11517115->SetFlexibleDirection( wxBOTH );
    flexGridSizer11517115->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer11517115->AddGrowableCol(0);
    flexGridSizer11517115->AddGrowableRow(1);
    
    boxSizer114->Add(flexGridSizer11517115, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText11618216 = gsgs_Style().createStaticText(this, wxID_ANY, _("Files to ignore:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    flexGridSizer11517115->Add(m_staticText11618216, 0, wxALL, WXC_FROM_DIP(5));
    
    flexGridSizer11517115->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));
    
    wxArrayString m_listIgnoreFilesArr;
    m_listIgnoreFiles = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), m_listIgnoreFilesArr, wxLB_SINGLE);
    m_listIgnoreFiles->SetToolTip(_("Here you can add the names of any files that you want to ignore. Standard wildcards will work e.g. moc_*"));
    
    flexGridSizer11517115->Add(m_listIgnoreFiles, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer124519 = new wxBoxSizer(wxVERTICAL);
    
    flexGridSizer11517115->Add(boxSizer124519, 1, wxRIGHT|wxBOTTOM|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button11921620 = new Button(this, wxID_ANY, _("Add"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer124519->Add(m_button11921620, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button125721 = new Button(this, wxID_ANY, _("Remove"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer124519->Add(m_button125721, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxFlexGridSizer* flexGridSizer11517 = new wxFlexGridSizer(2, 2, 0, 0);
    flexGridSizer11517->SetFlexibleDirection( wxBOTH );
    flexGridSizer11517->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer11517->AddGrowableCol(0);
    flexGridSizer11517->AddGrowableRow(1);
    
    boxSizer114->Add(flexGridSizer11517, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText11618 = gsgs_Style().createStaticText(this, wxID_ANY, _("Paths to ignore:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    flexGridSizer11517->Add(m_staticText11618, 0, wxALL, WXC_FROM_DIP(5));
    
    flexGridSizer11517->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));
    
    wxArrayString m_listExcludeArr;
    m_listExclude = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), m_listExcludeArr, wxLB_SINGLE);
    m_listExclude->SetToolTip(_("The filepaths of any folders that shouldn't be searched for missing files"));
    
    flexGridSizer11517->Add(m_listExclude, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer124 = new wxBoxSizer(wxVERTICAL);
    
    flexGridSizer11517->Add(boxSizer124, 1, wxRIGHT|wxBOTTOM|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button11921 = new Button(this, wxID_ANY, _("Add"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer124->Add(m_button11921, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button125 = new Button(this, wxID_ANY, _("Remove"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer124->Add(m_button125, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxFlexGridSizer* flexGridSizer115171 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer115171->SetFlexibleDirection( wxBOTH );
    flexGridSizer115171->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer115171->AddGrowableCol(0);
    flexGridSizer115171->AddGrowableRow(1);
    
    boxSizer114->Add(flexGridSizer115171, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText116182 = gsgs_Style().createStaticText(this, wxID_ANY, _("Regexs to use (optional):"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    flexGridSizer115171->Add(m_staticText116182, 0, wxALL, WXC_FROM_DIP(5));
    
    flexGridSizer115171->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));
    
    m_listCtrlRegexes = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxLC_SINGLE_SEL|wxLC_REPORT);
    m_listCtrlRegexes->SetToolTip(_("Most of the time you should find that files automatically get added to the most appropriate virtual directory. If yours don't, here you can add one or more regular expressions suitable for your situation. They'll be remembered for this project."));
    
    flexGridSizer115171->Add(m_listCtrlRegexes, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer1245 = new wxBoxSizer(wxVERTICAL);
    
    flexGridSizer115171->Add(boxSizer1245, 1, wxRIGHT|wxBOTTOM|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button119216 = new Button(this, wxID_ANY, _("Add"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer1245->Add(m_button119216, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button1257 = new Button(this, wxID_ANY, _("Remove"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    boxSizer1245->Add(m_button1257, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_stdBtnSizer120 = new DialogButtonSizer();
    
    boxSizer114->Add(m_stdBtnSizer120, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_button121 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer120->AddButton(m_button121);
    
    m_button122 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button122->SetDefault();
    m_stdBtnSizer120->AddButton(m_button122);
    m_stdBtnSizer120->Realize();
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_button11921620->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreFileBrowse), NULL, this);
    m_button125721->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreFileRemove), NULL, this);
    m_button125721->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreFileRemoveUpdateUI), NULL, this);
    m_button11921->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreBrowse), NULL, this);
    m_button125->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreRemove), NULL, this);
    m_button125->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreRemoveUpdateUI), NULL, this);
    m_button119216->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnAddRegex), NULL, this);
    m_button1257->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnRemoveRegex), NULL, this);
    m_button1257->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectFiletypesDlg::OnRemoveRegexUpdateUI), NULL, this);
    
    m_listCtrlRegexes->AppendColumn("Regex");
    m_listCtrlRegexes->AppendColumn("Virtual Directory");
}
// ---------------------------------------------------------
ReconcileProjectFiletypesDlg::~ReconcileProjectFiletypesDlg() 
{
    m_button11921620->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreFileBrowse), NULL, this);
    m_button125721->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreFileRemove), NULL, this);
    m_button125721->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreFileRemoveUpdateUI), NULL, this);
    m_button11921->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreBrowse), NULL, this);
    m_button125->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreRemove), NULL, this);
    m_button125->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectFiletypesDlg::OnIgnoreRemoveUpdateUI), NULL, this);
    m_button119216->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnAddRegex), NULL, this);
    m_button1257->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileProjectFiletypesDlg::OnRemoveRegex), NULL, this);
    m_button1257->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileProjectFiletypesDlg::OnRemoveRegexUpdateUI), NULL, this);
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::SetData()
{
    ProjectPtr proj = gsgs_Manager()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    wxString topleveldir, types;
    wxArrayString ignorefiles, excludes, regexes;
    proj->GetReconciliationData(topleveldir, types, ignorefiles, excludes, regexes);

    if(topleveldir.empty()) { topleveldir = proj->GetFileName().GetPath(); }
    wxFileName tld(topleveldir);
    if(tld.IsRelative()) { tld.MakeAbsolute(proj->GetFileName().GetPath()); }
    m_dirPickerToplevel->SetPath(tld.GetFullPath());

    if(types.empty()) { types << "cpp;c;h;hpp;xrc;wxcp;fbp"; }
    m_textExtensions->ChangeValue(types);

    m_listIgnoreFiles->Clear();
    m_listIgnoreFiles->Append(ignorefiles);

    m_listExclude->Clear();
    m_listExclude->Append(excludes);

    m_listCtrlRegexes->DeleteAllItems();
    for(size_t n = 0; n < regexes.GetCount(); ++n) {
        SetRegex(regexes[n]);
    }
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::GetData(wxString& toplevelDir, wxString& types, wxString& ignoreFiles,
                                           wxArrayString& excludePaths, wxArrayString& regexes) const
{
    toplevelDir = m_dirPickerToplevel->GetPath();
    types = m_textExtensions->GetValue();
    wxArrayString ignoreFilesArr = m_listIgnoreFiles->GetStrings();
    wxArrayString rawExcludePaths = m_listExclude->GetStrings();
    regexes = GetRegexes();

    // Fix the types to fit a standard mask string
    wxArrayString typesArr = ::wxStringTokenize(types, "|,;", wxTOKEN_STRTOK);
    for(size_t i = 0; i < typesArr.size(); ++i) {
        wxString& fileExt = typesArr.Item(i);
        if(!fileExt.StartsWith("*")) { fileExt.Prepend("*."); }
    }
    types = wxJoin(typesArr, ';');

    // Fix the the ignore files
    ignoreFiles = wxJoin(ignoreFilesArr, ';');

    // Fix the exclude paths. First make absolute
    wxString tld(toplevelDir);
    if(tld.Last() != wxFILE_SEP_PATH) { tld << wxFILE_SEP_PATH; }
    for(size_t i = 0; i < rawExcludePaths.size(); ++i) {
        wxString& path = rawExcludePaths.Item(i);
        if(!path.StartsWith(wxFILE_SEP_PATH)) { path.Prepend(tld); }
        // Now fix any symlinks in the path and add to the array
        excludePaths.Add(RealPath(path));
    }

    // While we're here, save the current data
    ProjectPtr proj = gsgs_Manager()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    proj->SetReconciliationData(wxFileName(toplevelDir).GetFullPath(wxPATH_UNIX), types, ignoreFilesArr, excludePaths,
                                regexes);
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::SetRegex(const wxString& regex)
{
    int n = m_listCtrlRegexes->GetItemCount();
    AppendListCtrlRow(m_listCtrlRegexes);
    SetColumnText(m_listCtrlRegexes, n, 0, regex.AfterFirst('|'));
    SetColumnText(m_listCtrlRegexes, n, 1, regex.BeforeFirst('|'));
}
// ---------------------------------------------------------
wxArrayString ReconcileProjectFiletypesDlg::GetRegexes() const
{
    wxArrayString array;
    for(int n = 0; n < m_listCtrlRegexes->GetItemCount(); ++n) {
        wxString regex = GetColumnText(m_listCtrlRegexes, n, 0);
        wxString VD = GetColumnText(m_listCtrlRegexes, n, 1);
        array.Add(VD + '|' +
                  regex); // Store the data as a VD|regex string, as the regex might contain a '|' but the VD won't
    }
    return array;
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnIgnoreBrowse(wxCommandEvent& WXUNUSED(event))
{
    ProjectPtr proj = gsgs_Manager()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    wxString topleveldir, types;
    wxArrayString ignorefiles, excludes, regexes;
    proj->GetReconciliationData(topleveldir, types, ignorefiles, excludes, regexes);

    if(topleveldir.empty()) { topleveldir = proj->GetFileName().GetPath(); }

    wxFileName tld(topleveldir);
    if(tld.IsRelative()) { tld.MakeAbsolute(proj->GetFileName().GetPath()); }
    wxString new_exclude = wxDirSelector(_("Select a directory to ignore:"), tld.GetFullPath(), wxDD_DEFAULT_STYLE,
                                         wxDefaultPosition, this);

    if(!new_exclude.empty()) {
        if(m_listExclude->FindString(new_exclude) == wxNOT_FOUND) { m_listExclude->Append(new_exclude); }
    }
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnIgnoreRemove(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_listExclude->GetSelection();
    if(sel != wxNOT_FOUND) { m_listExclude->Delete(sel); }
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnIgnoreRemoveUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listExclude->GetSelection() != wxNOT_FOUND);
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnIgnoreFileBrowse(wxCommandEvent& WXUNUSED(event))
{
    wxString name = wxGetTextFromUser("Enter the filename to ignore e.g. foo*.cpp", _("光栅故事"), "", this);
    if(!name.empty()) {
        if(m_listIgnoreFiles->FindString(name) == wxNOT_FOUND) { m_listIgnoreFiles->Append(name); }
    }
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnIgnoreFileRemove(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_listIgnoreFiles->GetSelection();
    if(sel != wxNOT_FOUND) { m_listIgnoreFiles->Delete(sel); }
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnIgnoreFileRemoveUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listIgnoreFiles->GetSelection() != wxNOT_FOUND);
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnAddRegex(wxCommandEvent& event)
{
    ReconcileByRegexDlg dlg(this, m_projname);
    if(dlg.ShowModal() == wxID_OK) { SetRegex(dlg.GetRegex()); }
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnRemoveRegex(wxCommandEvent& event)
{
    wxUnusedVar(event);

    long selecteditem = m_listCtrlRegexes->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(selecteditem != wxNOT_FOUND) { m_listCtrlRegexes->DeleteItem(selecteditem); }
}
// ---------------------------------------------------------
void ReconcileProjectFiletypesDlg::OnRemoveRegexUpdateUI(wxUpdateUIEvent& event)
{
    long selecteditem = m_listCtrlRegexes->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    event.Enable(selecteditem != wxNOT_FOUND);
}
// ---------------------------------------------------------
// ---------------------------------------------------------
// ReconcileByRegexDlg
// ---------------------------------------------------------
// ---------------------------------------------------------
ReconcileByRegexDlg::ReconcileByRegexDlg(wxWindow* parent, const wxString& projname, wxWindowID id, const wxString& title, 
    const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_projname(projname)
{
    if ( !bBitmapLoaded3 ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafterjdrz3kInitBitmapResources();
        bBitmapLoaded3 = true;
    }
    
    wxBoxSizer* boxSizer141 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer141);
    
    wxFlexGridSizer* flexGridSizer157 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer157->SetFlexibleDirection( wxBOTH );
    flexGridSizer157->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer157->AddGrowableCol(0);
    
    boxSizer141->Add(flexGridSizer157, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText145 = gsgs_Style().createStaticText(this, wxID_ANY, _("Enter the regex:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    flexGridSizer157->Add(m_staticText145, 0, wxLEFT|wxRIGHT|wxTOP, WXC_FROM_DIP(5));
    
    flexGridSizer157->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));
    
    m_textCtrlRegex = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_PROCESS_ENTER);
    
    flexGridSizer157->Add(m_textCtrlRegex, 0, wxLEFT|wxTOP|wxBOTTOM|wxEXPAND, WXC_FROM_DIP(5));
    
    flexGridSizer157->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));
    
    m_staticText147 = gsgs_Style().createStaticText(this, wxID_ANY, _("Destination Virtual Directory:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
    
    flexGridSizer157->Add(m_staticText147, 0, wxLEFT|wxRIGHT|wxTOP, WXC_FROM_DIP(10));
    
    flexGridSizer157->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));
    
    m_textCtrlVirtualFolder = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTE_PROCESS_ENTER);
    m_textCtrlVirtualFolder->SetToolTip(_("Select the virtual folder in which to place the matching files"));
    
    flexGridSizer157->Add(m_textCtrlVirtualFolder, 1, wxLEFT|wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
    
    m_bmpButton21010 = new wxBitmapButton(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("wxdirpickerctrl")), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxBU_AUTODRAW|wxBORDER_NONE);
    m_bmpButton21010->SetToolTip(_("Browse for virtual folder"));
    
    flexGridSizer157->Add(m_bmpButton21010, 0, wxALL|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
    
    flexGridSizer157->Add(0, 0, 0, wxALL, WXC_FROM_DIP(5));
    
    wxBoxSizer* boxSizer2088 = new wxBoxSizer(wxHORIZONTAL);
    
    flexGridSizer157->Add(boxSizer2088, 0, wxRIGHT|wxBOTTOM|wxEXPAND, WXC_FROM_DIP(5));
    
    m_stdBtnSizer12011 = new DialogButtonSizer();
    
    boxSizer141->Add(m_stdBtnSizer12011, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));
    
    m_button12112 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer12011->AddButton(m_button12112);
    
    m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonOK->SetDefault();
    m_stdBtnSizer12011->AddButton(m_buttonOK);
    m_stdBtnSizer12011->Realize();
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_textCtrlRegex->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ReconcileByRegexDlg::OnTextEnter), NULL, this);
    m_textCtrlVirtualFolder->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ReconcileByRegexDlg::OnTextEnter), NULL, this);
    m_bmpButton21010->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileByRegexDlg::OnVDBrowse), NULL, this);
    m_buttonOK->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileByRegexDlg::OnRegexOKCancelUpdateUI), NULL, this);
   
}
// ---------------------------------------------------------
ReconcileByRegexDlg::~ReconcileByRegexDlg() 
{
    m_textCtrlRegex->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ReconcileByRegexDlg::OnTextEnter), NULL, this);
    m_textCtrlVirtualFolder->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ReconcileByRegexDlg::OnTextEnter), NULL, this);
    m_bmpButton21010->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ReconcileByRegexDlg::OnVDBrowse), NULL, this);
    m_buttonOK->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ReconcileByRegexDlg::OnRegexOKCancelUpdateUI), NULL, this);
}
// ---------------------------------------------------------
void ReconcileByRegexDlg::OnTextEnter(wxCommandEvent& event)
{
    if(m_buttonOK->IsEnabled()) { EndModal(wxID_OK); }
}
// ---------------------------------------------------------
void ReconcileByRegexDlg::OnVDBrowse(wxCommandEvent& WXUNUSED(event))
{
    VirtualDirectorySelectorDlg selector(this, &gsgs_CppWorker(), m_textCtrlVirtualFolder->GetValue(),
                                         m_projname);
    if(selector.ShowModal() == wxID_OK) { m_textCtrlVirtualFolder->ChangeValue(selector.GetVirtualDirectoryPath()); }
}
// ---------------------------------------------------------
void ReconcileByRegexDlg::OnRegexOKCancelUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlRegex->IsEmpty() && !m_textCtrlVirtualFolder->IsEmpty());
}
// ---------------------------------------------------------
// ---------------------------------------------------------
// VirtualDirectoryTree
// ---------------------------------------------------------
// ---------------------------------------------------------
void VirtualDirectoryTree::BuildTree(const wxString& projName)
{
    ProjectPtr proj = gsgs_Manager()->GetProject(projName);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    ProjectTreePtr tree = proj->AsTree();
    TreeWalker<wxString, ProjectItem> walker(tree->GetRoot());

    for(; !walker.End(); walker++) {
        ProjectTreeNode* node = walker.GetNode();
        wxString displayname(node->GetData().GetDisplayName());
        if(node->GetData().GetKind() == ProjectItem::TypeVirtualDirectory) {
            wxString vdPath = displayname;
            ProjectTreeNode* tempnode = node->GetParent();
            while(tempnode) {
                vdPath = tempnode->GetData().GetDisplayName() + ':' + vdPath;
                tempnode = tempnode->GetParent();
            }

            VirtualDirectoryTree* parent = FindParent(vdPath.BeforeLast(':'));
            if(parent) {
                parent->StoreChild(displayname, vdPath);
            } else {
                // Any orphans must be root's top-level children, and we're root
                StoreChild(displayname, vdPath);
            }
        }
    }
}
// ---------------------------------------------------------
VirtualDirectoryTree* VirtualDirectoryTree::FindParent(const wxString& vdChildPath)
{
    if(!vdChildPath.empty()) {
        if(m_vdPath == vdChildPath) { return this; }
        for(size_t n = 0; n < m_children.size(); ++n) {
            VirtualDirectoryTree* item = m_children[n]->FindParent(vdChildPath);
            if(item) { return item; }
        }
    }

    return NULL;
}
// ---------------------------------------------------------
void VirtualDirectoryTree::StoreChild(const wxString& displayname, const wxString& vdPath)
{
    VirtualDirectoryTree* child = new VirtualDirectoryTree(this, displayname, vdPath);
    if(IsSourceVD(displayname.Lower()) || IsHeaderVD(displayname.Lower()) || IsResourceVD(displayname.Lower())) {
        m_children.push_back(child); // We want these processed last, so push_back
    } else {
        m_children.push_front(child);
    }
}
// ---------------------------------------------------------
wxString VirtualDirectoryTree::FindBestMatchVDir(const wxString& path, const wxString& ext) const
{
    // Try all children first
    for(size_t n = 0; n < m_children.size(); ++n) {
        wxString vdir = m_children[n]->FindBestMatchVDir(path, ext);
        if(!vdir.empty()) { return vdir; }
    }

    // Now try here. If there's an exact match, we're the correct one _unless_ there's a src/header/resource immediate
    // child
    wxString vdpath(m_vdPath.AfterFirst(':')); // We need to compare without the projectname
    vdpath.Replace(":", wxString(wxFILE_SEP_PATH));
    if(vdpath == path) {
        // Try for a src/header/etc immediate child. If there is one, it's presumably where files with a matching ext
        // should go
        for(size_t c = 0; c < m_children.size(); ++c) {
            wxString childname = m_children[c]->GetDisplayname();
            if(IsSourceVD(childname.Lower())) {
                if(ext == "cpp" || ext == "c" || ext == "cc") { return m_children[c]->GetVPath(); }
            }

            if(IsHeaderVD(childname.Lower())) {
                if(ext == "h" || ext == "hpp" || ext == "hh") { return m_children[c]->GetVPath(); }
            }

            if(IsResourceVD(childname.Lower())) {
                if(ext == "rc") { return m_children[c]->GetVPath(); }
            }
        }

        // None found so return us
        return m_vdPath;
    }

    return "";
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// SelectProjectsDlg
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
SelectProjectsDlg::SelectProjectsDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    wxBoxSizer* boxSizer25 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer25);
    
    m_banner45 = new wxBannerWindow(this, wxID_ANY, wxTOP, wxDefaultPosition, wxSize(-1,-1), wxBORDER_THEME);
    m_banner45->SetBitmap(wxNullBitmap);
    m_banner45->SetText(_("Select Projects"), _("Select the 'rename symbol' project scope"));
    m_banner45->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
    m_banner45->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
    
    boxSizer25->Add(m_banner45, 0, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* boxSizer33 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer25->Add(boxSizer33, 1, wxALL|wxEXPAND, 5);
    
    wxArrayString m_checkListBoxProjectsArr;
    m_checkListBoxProjects = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), m_checkListBoxProjectsArr, wxLB_SINGLE);
    
    boxSizer33->Add(m_checkListBoxProjects, 1, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* boxSizer35 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer33->Add(boxSizer35, 0, 0, 5);
    
    m_button37 = new Button(this, ID_CHECK_ALL, _("&Check All"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer35->Add(m_button37, 0, wxALL|wxEXPAND, 5);
    
    m_button39 = new Button(this, ID_UNCHECK_ALL, _("&Uncheck All"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer35->Add(m_button39, 0, wxALL|wxEXPAND, 5);
    
    wxBoxSizer* boxSizer27 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer25->Add(boxSizer27, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_button29 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_button29->SetDefault();
    
    boxSizer27->Add(m_button29, 0, wxALL, 5);
    
    m_button31 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer27->Add(m_button31, 0, wxALL, 5);
    
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    // Connect events
    m_button37->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SelectProjectsDlg::OnSelectAll), NULL, this);
    m_button39->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SelectProjectsDlg::OnUnSelectAll), NULL, this);
    
    // Populate the project list and check them all
    m_checkListBoxProjects->Clear();
    wxArrayString projects;
    gsgs_Manager()->GetProjectList( projects );
    
    for(size_t i=0; i<projects.GetCount(); ++i) {
        unsigned int pos = m_checkListBoxProjects->Append(projects.Item(i));
        m_checkListBoxProjects->Check(pos, true);
    }
}
//--------------------------------------------------------------------------
SelectProjectsDlg::~SelectProjectsDlg()
{
    m_button37->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SelectProjectsDlg::OnSelectAll), NULL, this);
    m_button39->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SelectProjectsDlg::OnUnSelectAll), NULL, this);
}
//--------------------------------------------------------------------------
void SelectProjectsDlg::OnSelectAll(wxCommandEvent& event)
{
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        m_checkListBoxProjects->Check(i);
    }
}
//--------------------------------------------------------------------------
void SelectProjectsDlg::OnUnSelectAll(wxCommandEvent& event)
{
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        m_checkListBoxProjects->Check(i, false);
    }
}
//--------------------------------------------------------------------------
wxArrayString SelectProjectsDlg::GetProjects() const
{
    wxArrayString projects;
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        if ( m_checkListBoxProjects->IsChecked(i) ) {
            projects.Add( m_checkListBoxProjects->GetString(i) );
        }
    }
    return projects;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// SettersGettersDlg
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
SettersGettersDlg::SettersGettersDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                                             const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_checkForDuplicateEntries(false)
{
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer1);

    m_searchCtrl = new wxSearchCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                    wxTE_PROCESS_ENTER);
    m_searchCtrl->SetToolTip(_("Type to filter the options"));
    m_searchCtrl->SetFocus();
    m_searchCtrl->ShowSearchButton(true);
    m_searchCtrl->ShowCancelButton(false);

    bSizer1->Add(m_searchCtrl, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* bSizer4 = new wxBoxSizer(wxHORIZONTAL);

    bSizer1->Add(bSizer4, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_dvListCtrl = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                        wxDV_ROW_LINES | wxDV_SINGLE);

    bSizer4->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_dvListCtrl->AppendTextColumn(_("Function"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                   wxDATAVIEW_COL_RESIZABLE);
    wxBoxSizer* bSizer5 = new wxBoxSizer(wxVERTICAL);

    bSizer4->Add(bSizer5, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonCheckAll =
        new Button(this, wxID_ANY, _("Check &All"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonCheckAll->SetDefault();

    bSizer5->Add(m_buttonCheckAll, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonUncheckAll =
        new Button(this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    bSizer5->Add(m_buttonUncheckAll, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxGridSizer* gridSizer12 = new wxGridSizer(0, 2, 0, 0);

    bSizer1->Add(gridSizer12, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_checkStartWithUppercase = new wxCheckBox(this, wxID_ANY, _("Function name starts with an upper case letter"),
                                               wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkStartWithUppercase->SetValue(false);

    gridSizer12->Add(m_checkStartWithUppercase, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxForamtFileWhenDone = new wxCheckBox(this, wxID_ANY, _("Format the file when done"), wxDefaultPosition,
                                                  wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxForamtFileWhenDone->SetValue(false);

    gridSizer12->Add(m_checkBoxForamtFileWhenDone, 0, wxALL, WXC_FROM_DIP(5));

    m_checkBoxReturnSelf = new wxCheckBox(this, wxID_ANY, _("Setter returns a reference to the object"),
                                          wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_checkBoxReturnSelf->SetValue(false);
    m_checkBoxReturnSelf->SetToolTip(_("A setter function will return a reference to the object, for example:\nFoo& "
                                       "SetFoo(const Obj& foo) {this->m_foo = foo; return *this;}"));

    gridSizer12->Add(m_checkBoxReturnSelf, 0, wxALL, WXC_FROM_DIP(5));

    m_stdBtnSizer16 = new DialogButtonSizer();

    bSizer1->Add(m_stdBtnSizer16, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

    m_button18 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button18->SetDefault();
    m_stdBtnSizer16->AddButton(m_button18);

    m_button20 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer16->AddButton(m_button20);
    m_stdBtnSizer16->Realize();

    if(GetParent()) {
        CentreOnParent();
    } else {
        CentreOnScreen();
    }

    // Connect events
    m_searchCtrl->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SettersGettersDlg::OnFilter), NULL,
                          this);
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED,
                          wxDataViewEventHandler(SettersGettersDlg::OnValueChanged), NULL, this);
    m_buttonCheckAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettersGettersDlg::OnCheckAll),
                              NULL, this);
    m_buttonUncheckAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                                wxCommandEventHandler(SettersGettersDlg::OnUncheckAll), NULL, this);
    m_checkStartWithUppercase->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                       wxCommandEventHandler(SettersGettersDlg::OnCheckStartWithUpperCase), NULL,
                                       this);
    m_checkBoxReturnSelf->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                  wxCommandEventHandler(SettersGettersDlg::OnSettersReturnReference), NULL, this);
                                  
    gsgs_Config().ReadItem(&m_settings);
    m_checkStartWithUppercase->SetValue(m_settings.GetFlags() & SettersGetterData::FunctionStartWithUpperCase);
    m_checkBoxForamtFileWhenDone->SetValue(m_settings.GetFlags() & SettersGetterData::FormatFileWhenDone);
    m_checkBoxReturnSelf->SetValue(m_settings.GetFlags() & SettersGetterData::SettersReturnReferenceToSelf);

    m_dvListCtrl->SetBitmaps(&gsgs_Image().GetBitmaps());
}
//-----------------------------------------------------------------------------
bool SettersGettersDlg::Init(const std::vector<TagEntryPtr>& tags, const wxFileName& file, int lineno)
{
    m_file = file;
    m_lineno = lineno;
    m_members = tags;
    return BuildTree() > 0;
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnCheckStartWithUpperCase(wxCommandEvent& event)
{
    wxUnusedVar(event);
    UpdateTree();
}
//-----------------------------------------------------------------------------
wxString SettersGettersDlg::GenerateFunctions()
{
    bool old_value = m_checkForDuplicateEntries;
    m_checkForDuplicateEntries = true;
    wxString code;
    GenerateSetters(code);
    if(code.IsEmpty() == false) { code << wxT("\n\n"); }
    wxString settersCode;
    GenerateGetters(settersCode);
    if(settersCode.IsEmpty() == false) { code << settersCode << wxT("\n"); }
    m_checkForDuplicateEntries = old_value;
    return code;
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::GenerateGetters(wxString& code)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        bool already_exists = false;
        wxString display_name;
        SettersGettersTreeData* data = GetItemData(i);
        if(data && (data->m_kind == SettersGettersTreeData::Kind_Getter) && data->m_checked) {
            wxString getter_code = GenerateGetter(data->m_tag, already_exists, display_name);
            if(!already_exists) { code << getter_code << "\n"; }
        }
    }
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::GenerateSetters(wxString& code)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        bool already_exists = false;
        wxString display_name;
        SettersGettersTreeData* data = GetItemData(i);
        if(data && (data->m_kind == SettersGettersTreeData::Kind_Setter) && data->m_checked) {
            wxString setter_code = GenerateSetter(data->m_tag, already_exists, display_name);
            if(!already_exists) { code << setter_code << "\n"; }
        }
    }
}
//-----------------------------------------------------------------------------
wxString SettersGettersDlg::GenerateSetter(TagEntryPtr tag, bool& alreadyExist, wxString& displayName)
{
    alreadyExist = false;
    bool startWithUpper = m_checkStartWithUppercase->IsChecked();
    bool returnSelf = m_checkBoxReturnSelf->IsChecked();

    Variable var;
    wxString method_name, method_signature;

    if(LanguageST::Get()->VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
        wxString func;
        wxString scope = _U(var.m_typeScope.c_str());
        if(returnSelf) {
            func << tag->GetParent() << "& ";
        } else {
            func << wxT("void ");
        }

        if(startWithUpper) {
            method_name << wxT("Set");
        } else {
            method_name << wxT("set");
        }

        wxString name = _U(var.m_name.c_str());
        FormatName(name);
        method_name << name;

        // add the method name
        func << method_name;

        // add the signature
        if(var.m_isBasicType) {
            method_signature << wxT("(");

        } else if(!var.m_isPtr) {
            method_signature << wxT("(const ");

        } else {
            method_signature << wxT("(");
        }

        if(!scope.IsEmpty() && !(scope == wxT("<global>"))) { method_signature << scope << wxT("::"); }

        method_signature << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str());
        if(var.m_isBasicType) {
            method_signature << wxT(" ");

        } else if(!var.m_isPtr) {
            method_signature << wxT("& ");

        } else {
            method_signature << wxT(" ");
        }

        wxString tmpName = _U(var.m_name.c_str());
        tmpName.StartsWith(wxT("m_"), &tmpName);

        method_signature << tmpName << wxT(")");
        func << method_signature;

        // at this point, func contains the display_name (i.e. the function without the implementation)
        displayName << func;

        // add the implementation
        func << wxT(" {this->") << _U(var.m_name.c_str()) << wxT(" = ") << tmpName << wxT(";");
        if(returnSelf) { func << " return *this;"; }
        func << "}";

        if(m_checkForDuplicateEntries) {
            alreadyExist = DoCheckExistance(tag->GetScope(), method_name, method_signature);
        }

        return func;
    }
    return wxEmptyString;
}
//-----------------------------------------------------------------------------
wxString SettersGettersDlg::GenerateGetter(TagEntryPtr tag, bool& alreadyExist, wxString& displayName)
{
    alreadyExist = false;
    bool startWithUpper = m_checkStartWithUppercase->IsChecked();

    Variable var;
    int midFrom(0);

    wxString method_name, method_signature;
    if(LanguageST::Get()->VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
        wxString func;
        wxString scope = _U(var.m_typeScope.c_str());

        wxString tagName = tag->GetName();
        tagName.MakeLower();

        bool isBool = (var.m_isBasicType && (var.m_type.find("bool") != std::string::npos));
        // Incase the member is named 'isXX'
        // disable the "isBool" functionality
        if(isBool && tagName.StartsWith(wxT("is"))) {
            isBool = false;

        } else if(isBool && (tagName.StartsWith(wxT("m_is")) || tagName.StartsWith(wxT("_is")))) {
            midFrom = 2;
        }

        if(!var.m_isPtr && !var.m_isBasicType) {
            func << wxT("const ");
            if(!scope.IsEmpty() && !(scope == wxT("<global>"))) { func << scope << wxT("::"); }
            func << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str()) << wxT("& ");

        } else {
            // generate different code for pointer
            if(!scope.IsEmpty() && !(scope == wxT("<global>"))) { func << scope << wxT("::"); }
            func << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str()) << wxT(" ");
        }

        // Prepare the prefix.
        // Make sure that boolean getters are treated differently
        // by making the getter in the format of 'IsXXX' or 'isXXX'
        wxString prefix = wxT("get");
        if(isBool) { prefix = wxT("is"); }

        if(startWithUpper) {
            wxString captializedPrefix = prefix.Mid(0, 1);
            captializedPrefix.MakeUpper().Append(prefix.Mid(1));
            prefix.swap(captializedPrefix);
        }

        wxString name = _U(var.m_name.c_str());
        FormatName(name);

        method_name << prefix; // Add the "Get"
        if(midFrom) { name = name.Mid(midFrom); }
        method_name << name; // Add the name

        // add the method name
        func << method_name;
        if(!var.m_isPtr) {
            method_signature << wxT("() const");

        } else {
            method_signature << wxT("()");
        }

        // add the signature
        func << method_signature;

        if(m_checkForDuplicateEntries) {
            alreadyExist = DoCheckExistance(tag->GetScope(), method_name, method_signature);
        }

        displayName << func;

        // add the implementation
        func << wxT(" {return ") << _U(var.m_name.c_str()) << wxT(";}");

        return func;
    }
    return wxEmptyString;
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::FormatName(wxString& name)
{
    if(name.StartsWith(wxT("m_"))) {
        name = name.Mid(2);

    } else if(name.StartsWith(wxT("_"))) {
        name = name.Mid(1);
    }

    wxStringTokenizer tkz(name, wxT("_"));
    name.Clear();
    while(tkz.HasMoreTokens()) {
        wxString token = tkz.NextToken();
        wxString pre = token.Mid(0, 1);
        token.Remove(0, 1);
        pre.MakeUpper();
        token.Prepend(pre);
        name << token;
    }
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::UpdatePreview()
{
    m_code.Clear();
    m_code = GenerateFunctions();
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::DoCheckAll(bool checked)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), checked);
    }
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnCheckAll(wxCommandEvent& e) { DoCheckAll(true); }
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnUncheckAll(wxCommandEvent& e) { DoCheckAll(false); }
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnUpdatePreview(wxCommandEvent& e)
{
    wxUnusedVar(e);
    UpdatePreview();
}
//-----------------------------------------------------------------------------
bool SettersGettersDlg::DoCheckExistance(const wxString& scope, const wxString& name, const wxString& method_signature)
{
    std::vector<TagEntryPtr> tmp_tags;
    TagsManagerST::Get()->FindByNameAndScope(name, scope.IsEmpty() ? wxT("<global>") : scope, tmp_tags);
    for(size_t i = 0; i < tmp_tags.size(); i++) {
        TagEntryPtr t = tmp_tags.at(i);
        wxString sig_one = TagsManagerST::Get()->NormalizeFunctionSig(t->GetSignature());
        wxString sig_two = TagsManagerST::Get()->NormalizeFunctionSig(method_signature);
        if(sig_one == sig_two) { return true; }
    }
    return false;
}
//-----------------------------------------------------------------------------
wxString SettersGettersDlg::GenerateSetter(TagEntryPtr tag)
{
    bool dummy;
    wxString s_dummy;
    return GenerateSetter(tag, dummy, s_dummy);
}
//-----------------------------------------------------------------------------
wxString SettersGettersDlg::GenerateGetter(TagEntryPtr tag)
{
    bool dummy;
    wxString s_dummy;
    return GenerateGetter(tag, dummy, s_dummy);
}
//-----------------------------------------------------------------------------
int SettersGettersDlg::BuildTree()
{
    Clear();
    m_checkForDuplicateEntries = true;

    std::vector<TagEntryPtr> tmpTags = m_members;
    m_members.clear();
    m_candidates.clear();
    for(size_t i = 0; i < tmpTags.size(); i++) {

        // add two children to generate the name of the next entries
        bool getter_exist(false);
        bool setter_exist(false);
        wxString setter_display_name;
        wxString getter_display_name;

        wxString getter = GenerateGetter(tmpTags.at(i), getter_exist, getter_display_name);
        wxString setter = GenerateSetter(tmpTags.at(i), setter_exist, setter_display_name);
        if(getter_exist && setter_exist) continue;

        m_members.push_back(tmpTags.at(i));
        Candidate candidate;
        candidate.tag = tmpTags.at(i);
        if(!getter_exist) {
            candidate.suggestGetter = true;
            candidate.getterName = getter_display_name;
        }

        if(!setter_exist) {
            candidate.suggestSetter = true;
            candidate.setterName = setter_display_name;
        }

        m_candidates.push_back(candidate);
        wxVector<wxVariant> cols;
        if(!setter_exist) {
            cols.clear();
            cols.push_back(
                ::MakeCheckboxVariant(setter_display_name, false,
                                      gsgs_Image().GetMimeImageId(gsgs::ImageManager::kFunctionPublic)));
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new SettersGettersTreeData(
                                               tmpTags.at(i), SettersGettersTreeData::Kind_Setter, false));
        }

        if(!getter_exist) {
            cols.clear();
            cols.push_back(
                ::MakeCheckboxVariant(getter_display_name, false,
                                      gsgs_Image().GetMimeImageId(gsgs::ImageManager::kFunctionPublic)));
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new SettersGettersTreeData(
                                               tmpTags.at(i), SettersGettersTreeData::Kind_Getter, false));
        }
    }

    m_checkForDuplicateEntries = false;
    return m_members.size();
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::UpdateTree()
{
    Clear();
    std::vector<Candidate> candidates;
    if(m_searchCtrl->GetValue().IsEmpty()) {
        // show all
        candidates = m_candidates;
    } else {
        // Filter the matches according to the search criteria
        for(size_t i = 0; i < m_candidates.size(); ++i) {
            Candidate tmpCandidate;

            const Candidate& candidate = m_candidates.at(i);
            if(candidate.suggestGetter && FileUtil::FuzzyMatch(m_searchCtrl->GetValue(), candidate.getterName)) {
                tmpCandidate.suggestGetter = true;
                tmpCandidate.getterName = candidate.getterName;
            }

            if(candidate.suggestSetter && FileUtil::FuzzyMatch(m_searchCtrl->GetValue(), candidate.setterName)) {
                tmpCandidate.suggestSetter = true;
                tmpCandidate.setterName = candidate.setterName;
            }

            if(tmpCandidate.suggestGetter || tmpCandidate.suggestSetter) {
                tmpCandidate.tag = candidate.tag;
                candidates.push_back(tmpCandidate);
            }
        }
    }

    for(size_t i = 0; i < candidates.size(); ++i) {
        const Candidate& candidate = candidates.at(i);
        if(candidate.suggestSetter) {
            wxVector<wxVariant> cols;
            cols.push_back(
                ::MakeCheckboxVariant(candidate.setterName, false,
                                      gsgs_Image().GetMimeImageId(gsgs::ImageManager::kFunctionPublic)));
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new SettersGettersTreeData(
                                               candidate.tag, SettersGettersTreeData::Kind_Setter, false));
        }

        if(candidate.suggestGetter) {
            wxVector<wxVariant> cols;
            cols.push_back(
                ::MakeCheckboxVariant(candidate.getterName, false,
                                      gsgs_Image().GetMimeImageId(gsgs::ImageManager::kFunctionPublic)));
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new SettersGettersTreeData(
                                               candidate.tag, SettersGettersTreeData::Kind_Getter, false));
        }
    }
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnButtonOk(wxCommandEvent& e) { e.Skip(); }
//-----------------------------------------------------------------------------
SettersGettersDlg::~SettersGettersDlg()
{
    m_searchCtrl->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SettersGettersDlg::OnFilter), NULL,
                             this);
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED,
                             wxDataViewEventHandler(SettersGettersDlg::OnValueChanged), NULL, this);
    m_buttonCheckAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettersGettersDlg::OnCheckAll),
                                 NULL, this);
    m_buttonUncheckAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                   wxCommandEventHandler(SettersGettersDlg::OnUncheckAll), NULL, this);
    m_checkStartWithUppercase->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                          wxCommandEventHandler(SettersGettersDlg::OnCheckStartWithUpperCase), NULL,
                                          this);
    m_checkBoxReturnSelf->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                                     wxCommandEventHandler(SettersGettersDlg::OnSettersReturnReference), NULL,
                                     this);
                                     
    size_t flags(0);
    if(m_checkStartWithUppercase->IsChecked()) flags |= SettersGetterData::FunctionStartWithUpperCase;
    if(m_checkBoxForamtFileWhenDone->IsChecked()) flags |= SettersGetterData::FormatFileWhenDone;
    if(m_checkBoxReturnSelf->IsChecked()) flags |= SettersGetterData::SettersReturnReferenceToSelf;

    m_settings.SetFlags(flags);
    gsgs_Config().WriteItem(&m_settings);
}
//-----------------------------------------------------------------------------
wxString SettersGettersDlg::GetGenCode()
{
    UpdatePreview();
    return m_code;
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnValueChanged(wxDataViewEvent& event)
{
    SettersGettersTreeData* data = GetItemData(event.GetItem());
    if(!data) { return; }
    data->m_checked = m_dvListCtrl->IsItemChecked(event.GetItem());
}

//----------------------------------------------------
//----------------------------------------------------

SettersGetterData::SettersGetterData()
    : ObjectData("SettersGetterData")
    , m_flags(FormatFileWhenDone | FunctionStartWithUpperCase | SettersReturnReferenceToSelf)
{
}
//-----------------------------------------------------------------------------
SettersGetterData::~SettersGetterData() {}
//-----------------------------------------------------------------------------
void SettersGetterData::FromJSON(const JSONObject& json) { json.getObject("m_flags").getValue(m_flags, m_flags); }
//-----------------------------------------------------------------------------
JSONObject SettersGetterData::ToJSON() const
{
    JSONObject element = JSONObject::createObject(GetName());
    element.addProperty("m_flags", m_flags);
    return element;
}
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnFilter(wxCommandEvent&) { UpdateTree(); }
//-----------------------------------------------------------------------------
void SettersGettersDlg::OnSettersReturnReference(wxCommandEvent&)
{
    UpdateTree();
}
//-----------------------------------------------------------------------------
SettersGettersTreeData* SettersGettersDlg::GetItemData(const wxDataViewItem& item)
{
    return reinterpret_cast<SettersGettersTreeData*>(m_dvListCtrl->GetItemData(item));
}
//-----------------------------------------------------------------------------
SettersGettersTreeData* SettersGettersDlg::GetItemData(size_t row) { return GetItemData(m_dvListCtrl->RowToItem(row)); }
//-----------------------------------------------------------------------------
void SettersGettersDlg::Clear()
{
    m_dvListCtrl->DeleteAllItems([&](wxUIntPtr data) {
        SettersGettersTreeData* d = reinterpret_cast<SettersGettersTreeData*>(data);
        wxDELETE(d);
    });
}
//--------------------------------------------------------------------------