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

#include "gsgsCommonDialog.h"
#include "gsgsFileUtil.h"
#include "gsgsLexerStyle.h"
#include "gsgsEditorConfig.h"
#include "gsgsWindowManager.h"
#include "gsgsStyleManager.h"
#include "gsgsImageManager.h"
#include "gsgsBookmarkManager.h"
#include "gsgsNotebook.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsConfig.h"
#include "gsgsLogManager.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "gsgsOptionsConfig.h"
#include "IWorkspace.h"
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/menu.h>
#include <wx/renderer.h>
#include <wx/settings.h>

#define X_SPACER 10
#define Y_SPACER 5

namespace gsgs
{
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // EditDlg
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    EditDlg::EditDlg(wxWindow* parent, const wxString& text, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
                         long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer2);

        m_stc10 = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stc10->SetFocus();
        // Configure the fold margin
        m_stc10->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
        m_stc10->SetMarginMask(4, wxSTC_MASK_FOLDERS);
        m_stc10->SetMarginSensitive(4, true);
        m_stc10->SetMarginWidth(4, 0);

        // Configure the tracker margin
        m_stc10->SetMarginWidth(1, 0);

        // Configure the symbol margin
        m_stc10->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_stc10->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
        m_stc10->SetMarginWidth(2, 0);
        m_stc10->SetMarginSensitive(2, true);

        // Configure the line numbers margin
        m_stc10->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_stc10->SetMarginWidth(0, 0);

        // Configure the line symbol margin
        m_stc10->SetMarginType(3, wxSTC_MARGIN_FORE);
        m_stc10->SetMarginMask(3, 0);
        m_stc10->SetMarginWidth(3, 0);
        // Select the lexer
        m_stc10->SetLexer(wxSTC_LEX_NULL);
        // Set default font / styles
        m_stc10->StyleClearAll();
        m_stc10->SetWrapMode(0);
        m_stc10->SetIndentationGuides(0);
        m_stc10->SetKeyWords(0, wxT(""));
        m_stc10->SetKeyWords(1, wxT(""));
        m_stc10->SetKeyWords(2, wxT(""));
        m_stc10->SetKeyWords(3, wxT(""));
        m_stc10->SetKeyWords(4, wxT(""));

        boxSizer2->Add(m_stc10, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer4 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer2->Add(boxSizer4, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_button6 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_button6->SetDefault();

        boxSizer4->Add(m_button6, 0, wxALL, WXC_FROM_DIP(5));

        m_button8 = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        boxSizer4->Add(m_button8, 0, wxALL, WXC_FROM_DIP(5));

        if(GetParent()) {
            CentreOnParent(wxBOTH);
        } else {
            CentreOnScreen(wxBOTH);
        }
        LexerStyle::Ptr_t lex = gsgs_Style().GetLexer("text", "Default");
        lex->Apply(m_stc10);
        m_stc10->SetText(text);
        m_stc10->SetMultiPaste(true);
        m_stc10->SetMultipleSelection(true);
        m_stc10->SetAdditionalSelectionTyping(true);
    }
    //----------------------------------------------------------------------------
    EditDlg::~EditDlg() {}
    //----------------------------------------------------------------------------
    wxString EditDlg::GetUserString(const wxString& initialValue, wxWindow* parent)
    {
        EditDlg dlg(parent, initialValue);
        if(dlg.ShowModal() == wxID_OK) {
            return dlg.GetText();
        }
        return wxEmptyString;
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // EnvVarImporterDlg
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    EnvVarImporterDlg::EnvVarImporterDlg(wxWindow* parent,
                                         const wxString& projectName,
                                         const wxString& cfgName,
                                         std::set<wxString> listEnvVar,
                                         BuildConfigPtr le_conf,
                                         bool* showDlg)
        : wxDialog(parent, wxID_ANY, _("Import - Environment variable"), wxDefaultPosition, wxSize(600,360), wxSTAY_ON_TOP|wxCAPTION)
        , le_conf(le_conf)
        , showDlg(showDlg)
    {

        boxSizer4 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer4);
        
        boxSizer18 = new wxBoxSizer(wxHORIZONTAL);
        
        boxSizer4->Add(boxSizer18, 1, wxALL|wxEXPAND, 5);
        
        m_staticText26 = gsgs_Style().createStaticText(this, wxID_ANY, _("Project Name:"), wxDefaultPosition, wxSize(-1,-1), 0);
        
        boxSizer18->Add(m_staticText26, 0, wxALL, 5);
        
        m_projectName = gsgs_Style().createStaticText(this, wxID_ANY, _("Static Text Label"), wxDefaultPosition, wxSize(-1,-1), 0);
        m_projectName->SetForegroundColour(wxColour(wxT("rgb(255,0,0)")));
        
        boxSizer18->Add(m_projectName, 0, wxALL, 5);
        
        boxSizer18->Add(0, 0, 1, wxALL, 5);
        
        m_staticText20 = gsgs_Style().createStaticText(this, wxID_ANY, _("Configuration Name:"), wxDefaultPosition, wxSize(-1,-1), 0);
        
        boxSizer18->Add(m_staticText20, 0, wxALL, 5);
        
        m_confName = gsgs_Style().createStaticText(this, wxID_ANY, _("Static Text Label"), wxDefaultPosition, wxSize(-1,-1), 0);
        m_confName->SetForegroundColour(wxColour(wxT("rgb(255,0,0)")));
        
        boxSizer18->Add(m_confName, 0, wxALL, 5);
        
        boxSizer18->Add(0, 0, 1, wxALL, 5);
        
        boxSizer22 = new wxBoxSizer(wxVERTICAL);
        
        boxSizer4->Add(boxSizer22, 1, wxALL|wxEXPAND, 5);
        
        m_staticText6 = gsgs_Style().createStaticText(this, wxID_ANY, _("Please define the value of the following variables:"), wxDefaultPosition, wxSize(-1,-1), 0);
        
        boxSizer22->Add(m_staticText6, 0, wxALL, 5);
        
        m_envVars = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), wxTE_MULTILINE);
        
        boxSizer4->Add(m_envVars, 0, wxALL|wxEXPAND, 5);
        m_envVars->SetMinSize(wxSize(600,200));
        
        boxSizer16 = new wxBoxSizer(wxHORIZONTAL);
        
        boxSizer4->Add(boxSizer16, 1, wxALL|wxEXPAND, 0);
        
        m_btnImport = new Button(this, wxID_ANY, _("Import"), wxDefaultPosition, wxSize(-1,-1), 0);
        
        boxSizer16->Add(m_btnImport, 0, wxALL, 5);
        
        m_btnContinue = new Button(this, wxID_ANY, _("Continue"), wxDefaultPosition, wxSize(-1,-1), 0);
        
        boxSizer16->Add(m_btnContinue, 0, wxALL, 5);
        
        m_btnNotImport = new Button(this, wxID_ANY, _("Skip"), wxDefaultPosition, wxSize(-1,-1), 0);
        
        boxSizer16->Add(m_btnNotImport, 0, wxALL, 5);
        
        SetSizeHints(600,360);
        if ( GetSizer() ) {
             GetSizer()->Fit(this);
        }
        CentreOnParent(wxBOTH);
    #if wxVERSION_NUMBER >= 2900
        if(!wxPersistenceManager::Get().Find(this)) {
            wxPersistenceManager::Get().RegisterAndRestore(this);
        } else {
            wxPersistenceManager::Get().Restore(this);
        }
    #endif
        // Connect events
        m_btnImport->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvVarImporterDlg::OnImport), NULL, this);
        m_btnContinue->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvVarImporterDlg::OnContinue), NULL, this);
        m_btnNotImport->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvVarImporterDlg::OnSkip), NULL, this);
        wxString value = wxT("");

        for(wxString envVar : listEnvVar) {
            value += envVar + wxT("=?") + wxT("\n");
        }

        m_projectName->SetLabel(projectName);
        m_confName->SetLabel(cfgName);
        m_envVars->SetValue(value);
    }
    //----------------------------------------------------------------------------
    EnvVarImporterDlg::~EnvVarImporterDlg()
    {
        m_btnImport->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvVarImporterDlg::OnImport), NULL, this);
        m_btnContinue->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvVarImporterDlg::OnContinue), NULL, this);
        m_btnNotImport->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvVarImporterDlg::OnSkip), NULL, this);
    }
    //----------------------------------------------------------------------------
    void EnvVarImporterDlg::OnImport(wxCommandEvent& event)
    {
        le_conf->SetEnvvars(m_envVars->GetValue());
        Close();
    }
    //----------------------------------------------------------------------------
    void EnvVarImporterDlg::OnContinue(wxCommandEvent& event)
    {
        Close();
    }
    //----------------------------------------------------------------------------
    void EnvVarImporterDlg::OnSkip(wxCommandEvent& event)
    {
        *showDlg = false;
        Close();
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // TableLineEditorDlg
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    
    TableLineEditorDlg::TableLineEditorDlg(wxWindow* parent, const wxArrayString& columns, const wxArrayString& data)
        : TableLineEditorBaseDlg(parent)
        , m_columns(columns)
        , m_data(data)
    {
        if(!m_columns.IsEmpty()) {
            m_listBoxColumns->Append(m_columns);
            m_listBoxColumns->SetSelection(0);
            DoItemSelected(0);
        }
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        lexer->Apply(m_stc);
        m_stc->SetReadOnly(true);
        CenterOnParent();
    }
    //----------------------------------------------------------------------------
    TableLineEditorDlg::~TableLineEditorDlg() {}
    //----------------------------------------------------------------------------
    void TableLineEditorDlg::OnColumnSelected(wxCommandEvent& event) { DoItemSelected(event.GetSelection()); }
    //----------------------------------------------------------------------------
    void TableLineEditorDlg::DoItemSelected(int index)
    {
        if((index != wxNOT_FOUND) && (index < (int)m_data.size())) {
            m_stc->SetReadOnly(false);
            m_stc->SetText(m_data[index]);
            m_stc->SetReadOnly(true);
        }
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // DiffSelectFoldersDlg
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    DiffSelectFoldersDlg::DiffSelectFoldersDlg(wxWindow* parent, const wxString& left, const wxString& right, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        wxBoxSizer* boxSizer38 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer38);

        m_panel46 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

        boxSizer38->Add(m_panel46, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer48 = new wxBoxSizer(wxVERTICAL);
        m_panel46->SetSizer(boxSizer48);

        m_staticText54 = gsgs_Style().createStaticText(m_panel46, wxID_ANY, _("Select folders for comparison:"), wxDefaultPosition,
            wxDLG_UNIT(m_panel46, wxSize(-1, -1)), 0);

        boxSizer48->Add(m_staticText54, 0, wxALL, WXC_FROM_DIP(5));

        m_dirPickerLeft = new wxDirPickerCtrl(m_panel46, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition,
            wxDLG_UNIT(m_panel46, wxSize(-1, -1)),
            wxDIRP_SMALL | wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);

        boxSizer48->Add(m_dirPickerLeft, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_dirPickerRight = new wxDirPickerCtrl(m_panel46, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition,
            wxDLG_UNIT(m_panel46, wxSize(-1, -1)),
            wxDIRP_SMALL | wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);

        boxSizer48->Add(m_dirPickerRight, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_stdBtnSizer40 = new DialogButtonSizer();

        boxSizer38->Add(m_stdBtnSizer40, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer40->AddButton(m_buttonOK);

        m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer40->AddButton(m_buttonCancel);
        m_stdBtnSizer40->Realize();

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_buttonOK->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DiffSelectFoldersDlg::OnOKUI), NULL, this);

        wxSize parentSize = GetParent()->GetSize();
        parentSize.SetWidth(parentSize.GetWidth() / 2);
        SetSize(parentSize.GetWidth(), -1);
        m_dirPickerLeft->SetPath(left);
        m_dirPickerRight->SetPath(right);
    }
    //----------------------------------------------------------------------------
    DiffSelectFoldersDlg::~DiffSelectFoldersDlg() 
    {
        m_buttonOK->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DiffSelectFoldersDlg::OnOKUI), NULL, this);
    }
    //----------------------------------------------------------------------------
    void DiffSelectFoldersDlg::OnOKUI(wxUpdateUIEvent& event)
    {
        wxString left(m_dirPickerLeft->GetPath());
        wxString right(m_dirPickerRight->GetPath());

        event.Enable(left != right && wxFileName::DirExists(left) && wxFileName::DirExists(right));
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // EditorBar
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    EditorBar::EditorBar(wxWindow* parent)
        : EditorBarBase(parent)
    {
        gsgs_Style().RegisterWindow(this);

        m_functionBmp = gsgs_Image().LoadBitmap("function_public", 16);
        CreateBookmarksBitmap();

        gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &EditorBar::OnEditorChanged, this);
        gsgs_Event().Bind(wxEVT_CMD_PAGE_CHANGED, &EditorBar::OnEditorChanged, this);
        gsgs_Event().Bind(wxEVT_ALL_EDITORS_CLOSED, &EditorBar::OnEditorChanged, this);
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &EditorBar::OnTheme, this);
        gsgs_Event().Bind(wxEVT_MARKER_CHANGED, &EditorBar::OnMarkerChanged, this);
        m_buttonScope->SetBitmap(m_functionBmp);
        m_buttonScope->SetHasDropDownMenu(true);
        m_buttonFilePath->SetHasDropDownMenu(true);
        m_buttonBookmarks->SetHasDropDownMenu(true);
    }
    //---------------------------------------------------------------------
    EditorBar::~EditorBar()
    {
        gsgs_Style().UnRegisterWindow(this);
        gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &EditorBar::OnEditorChanged, this);
        gsgs_Event().Unbind(wxEVT_CMD_PAGE_CHANGED, &EditorBar::OnEditorChanged, this);
        gsgs_Event().Unbind(wxEVT_ALL_EDITORS_CLOSED, &EditorBar::OnEditorChanged, this);
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &EditorBar::OnTheme, this);
        gsgs_Event().Unbind(wxEVT_MARKER_CHANGED, &EditorBar::OnMarkerChanged, this);
    }
    //---------------------------------------------------------------------
    void EditorBar::OnEditorChanged(wxCommandEvent& e)
    {
        e.Skip();
        CallAfter(&EditorBar::DoRefreshColoursAndFonts);
    }
    //---------------------------------------------------------------------
    void EditorBar::SetMessage(const wxString& className, const wxString& function)
    {
        if ((className != m_classname) || (function != m_function)) {
            m_classname = className;
            m_function = function;
            CallAfter(&EditorBar::DoRefreshColoursAndFonts);
        }
    }
    //---------------------------------------------------------------------
    void EditorBar::DoShow(bool s)
    {
        m_shouldShow = s;
        if (Show(s)) 
        { 
            GetParent()->GetSizer()->Layout(); 
            //GetParent()->SendSizeEvent();
        }
        CallAfter(&EditorBar::DoRefreshColoursAndFonts);
    }
    //---------------------------------------------------------------------
    void EditorBar::OnTheme(wxCommandEvent& e)
    {
        e.Skip();
        CallAfter(&EditorBar::DoRefreshColoursAndFonts);
    }
    //---------------------------------------------------------------------
    void EditorBar::DoRefreshColoursAndFonts()
    {
        // Set the defaults
        m_filename.clear();
        m_projectFile.clear();
        m_projectName.clear();
        m_filenameRelative.clear();
        m_bookmarks.clear();

        if (!m_shouldShow) { return; }

        IEditor* editor = GetManager()->GetActiveEditor();
        if (editor)
        {
            if (!IsShown()) { Show(); }
            // Update bookmarks button
            LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("c++");
            editor->GetFindMarkers(m_bookmarks);

            if (!m_bookmarks.empty())
            {
                CreateBookmarksBitmap();
                if (!m_buttonBookmarks->IsShown())
                {
                    m_buttonBookmarks->Show();
                }
                m_buttonBookmarks->SetText(_("Bookmarks"));
                m_buttonBookmarks->SetBitmap(m_bookmarksBmp);
            }
            else
            {
                m_buttonBookmarks->Hide();
            }

            // Update file path button
            if (!m_buttonFilePath->IsShown())
            {
                m_buttonFilePath->Show();
            }
            m_buttonFilePath->SetText(editor->GetFileName().GetFullPath());
            m_filename = editor->GetFileName().GetFullPath();

            wxString scope;
            if (!m_classname.IsEmpty()) { scope << m_classname << "::"; }
            if (!m_function.IsEmpty()) { scope << m_function; }
            if (scope.IsEmpty()) {
                m_buttonScope->Hide();
            }
            else {
                if (!m_buttonScope->IsShown()) { m_buttonScope->Show(); }
                m_buttonScope->SetText(scope);
            }
        }
        else {
            m_classname.clear();
            m_function.clear();
            m_buttonScope->SetText("");
            m_buttonFilePath->SetText("");
            m_buttonBookmarks->SetText("");
            m_buttonScope->Hide();
            m_buttonFilePath->Hide();
            m_buttonBookmarks->Hide();
            Hide();
        }
        GetParent()->GetSizer()->Layout();
        //GetParent()->SendSizeEvent();
    }
    //---------------------------------------------------------------------
    void EditorBar::DoRefresh() { Refresh(); }
    //---------------------------------------------------------------------
    void EditorBar::OnMarkerChanged(Event& event)
    {
        event.Skip();
        if (!IsShown()) { return; }
        CallAfter(&EditorBar::DoRefreshColoursAndFonts);
    }
    //---------------------------------------------------------------------
    void EditorBar::CreateBookmarksBitmap()
    {
        m_bookmarksBmp = wxBitmap(16, 16);
        wxMemoryDC memDC(m_bookmarksBmp);
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        wxColour bgColour = options->GetBookmarkBgColour(0);
        wxColour fgColour = options->GetBookmarkFgColour(0);
        memDC.SetPen(fgColour);
        memDC.SetBrush(bgColour);
        memDC.DrawRectangle(0, 0, 16, 16);
        memDC.SelectObject(wxNullBitmap);
    }
    //---------------------------------------------------------------------
    void EditorBar::OnButtonActions(wxCommandEvent& event)
    {
        wxUnusedVar(event);

        wxMenu menu;
        wxString text;

        if (wxFileName(m_filename).GetPath().IsEmpty())
        {
            text << _(" Temporary file can not copy file paths !");
            menu.Append(wxID_ANY, text);
            m_buttonFilePath->ShowMenu(menu);
            return;
        }
        text << _("Copy '") << m_filename << _("' to the clipboard");
        wxMenuItem* idCopyFullPath = menu.Append(wxID_ANY, text);

        text.Clear();
        text << _("Copy '") << wxFileName(m_filename).GetFullName() << _("' to the clipboard");
        wxMenuItem* idCopyName = menu.Append(wxID_ANY, text);

        text.Clear();
        text << _("Copy '") << wxFileName(m_filename).GetPath() << _("' to the clipboard");
        wxMenuItem* idCopyPath = menu.Append(wxID_ANY, text);

        menu.AppendSeparator();
        wxMenuItem *idOpenExplorer, *idOpenShell;
        {
            idOpenShell = new wxMenuItem(NULL, wxID_ANY, _("Open Shell"), _("Open Shell"));
            idOpenShell->SetBitmap(gsgs_Image().LoadBitmap("console"));
            menu.Append(idOpenShell);
        }

        {
            idOpenExplorer = new wxMenuItem(NULL, wxID_ANY, _("Open Containing Folder"), _("Open Containing Folder"));
            idOpenExplorer->SetBitmap(gsgs_Image().LoadBitmap("folder-yellow"));
            menu.Append(idOpenExplorer);
        }

        // Capture all menu items in a single callback that simply keeps the selected menu item id
        int selection = wxID_NONE;
        menu.Bind(wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
        m_buttonFilePath->ShowMenu(menu);

        if (selection == wxID_NONE) return;

        text.Clear();
        if (selection == idCopyFullPath->GetId()) {
            text = m_filename;
        }
        else if (selection == idCopyName->GetId()) {
            text = wxFileName(m_filename).GetFullName();
        }
        else if (selection == idCopyPath->GetId()) {
            text = wxFileName(m_filename).GetPath();
        }
        else if (selection == idOpenExplorer->GetId()) {
            FileUtil::OpenFileExplorerAndSelect(wxFileName(m_filename).GetFullPath());
        }
        else if (selection == idOpenShell->GetId()) {
            FileUtil::OpenTerminal(wxFileName(m_filename).GetPath());
        }
        if (!text.IsEmpty())
        {
            ::CopyToClipboard(text);
            GetManager()->SetStatusMessage((wxString() << "'" << text << _("' copied!")), 2);
        }
    }
    //---------------------------------------------------------------------
    void EditorBar::OnButtonBookmarks(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        IEditor* editor = GetManager()->GetActiveEditor();
        std::vector<std::pair<int, wxString> > V;
        if (editor && editor->GetFindMarkers(V)) {
            // Show bookmarks menu
            wxMenu menu;
            std::unordered_map<int, int> M;
            std::for_each(V.begin(), V.end(), [&](const std::pair<int, wxString>& p) {
                wxString text = wxString::Format("%5u: ", p.first);
                text << p.second;
                M[menu.Append(wxID_ANY, text)->GetId()] = p.first; // Make the menu item ID with the line number
            });

            // We got something to display
            int selection = wxID_NONE;
            menu.Bind(wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
            m_buttonBookmarks->ShowMenu(menu);

            if (selection == wxID_NONE) return;
            if (M.count(selection)) {
                int lineNumber = M[selection] - 1;
                editor->CenterLine(lineNumber);
                editor->GetCtrl()->EnsureVisible(lineNumber);
                editor->GetCtrl()->EnsureCaretVisible();
                editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
            }
        }
    }
    //---------------------------------------------------------------------
    void EditorBar::OnButtonScope(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxMenu menu;
        ContextMenuEvent menuEvent(wxEVT_NAVBAR_SCOPE_MENU_SHOWING);
        menuEvent.SetMenu(&menu);
        gsgs_Event().ProcessEvent(menuEvent);

        if (menu.GetMenuItemCount()) {
            // We got something to display
            // Keep track of the menu items
            std::unordered_map<int, wxString> M;
            const wxMenuItemList& list = menu.GetMenuItems();
            wxMenuItemList::const_iterator iter = list.begin();
            for (; iter != list.end(); ++iter) {
                wxMenuItem* menuItem = *iter;
                M[menuItem->GetId()] = menuItem->GetItemLabel();
            }

            // Popup the menu
            int selection = wxID_NONE;
            menu.Bind(wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
            m_buttonScope->ShowMenu(menu);

            if (selection == wxID_NONE) return;
            if (M.count(selection)) {
                // Fire selection made event
                Event selectionEvent(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE);
                selectionEvent.SetString(M[selection]);
                gsgs_Event().AddPendingEvent(selectionEvent);
            }
        }
    }
    //---------------------------------------------------------------------
}