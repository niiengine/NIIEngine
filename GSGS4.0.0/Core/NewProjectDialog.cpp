#include "NewProjectDialog.h"
#include "globals.h"
#include "gsgsCommonDialog.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsBuildConfig.h"
#include "gsgsBuilderGnuMake.h"
#include "gsgsDebuggerManager.h"
#include "CxxTemplateFunction.h"
#include "gsgsStyleManager.h"
#include "gsgsLogManager.h"
#include "gsgsBoxTip.h"
#include "gsgsTree.h"
#include "gsgsConfig.h"
#include "gsgsCodeBoxManager.h"
#include "gsgsTagManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsPluginCommon.h"
#include "gsgsWorkspace.h"
#include "gsgsLexerStyle.h"
#include "gsgsToolBar.h"
#include "gsgsOptionsConfig.h"
#include "gsgsWizard.h"
#include "plugin.h"
#include "ieditor.h"
#include "imanager.h"
#include <wx/app.h>
#include <wx/wxcrt.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>
#include <wx/wupdlock.h>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/stc/stc.h>
#include <wx/dcgraph.h>
#include <wx/display.h>
#include <wx/regex.h>
#include <wx/clipbrd.h>
#include <wx/radiobox.h>

namespace gsgs
{
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // NewProjectDialog
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    NewProjectDialog::NewProjectDialog(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer2);

        wxFlexGridSizer* flexGridSizer10 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer10->SetFlexibleDirection(wxBOTH);
        flexGridSizer10->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer10->AddGrowableCol(1);

        boxSizer2->Add(flexGridSizer10, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText12 = gsgs_Style().createStaticText(this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer10->Add(m_staticText12, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrlName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(500, -1)), 0);
        flexGridSizer10->Add(m_textCtrlName, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText20 = gsgs_Style().createStaticText(this, wxID_ANY, _("Path:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer10->Add(m_staticText20, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_dirPicker = new wxDirPickerCtrl(this, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), wxDIRP_SMALL | wxDIRP_USE_TEXTCTRL);

        flexGridSizer10->Add(m_dirPicker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText16 = gsgs_Style().createStaticText(this, wxID_ANY, _("Type:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        flexGridSizer10->Add(m_staticText16, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceTypeArr;
        m_choiceType =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceTypeArr, 0);

        flexGridSizer10->Add(m_choiceType, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText24 = gsgs_Style().createStaticText(this, wxID_ANY, _("Compiler:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer10->Add(m_staticText24, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_choiceCompiler =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceTypeArr, 0);

        flexGridSizer10->Add(m_choiceCompiler, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText28 = gsgs_Style().createStaticText(this, wxID_ANY, _("Debugger:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        flexGridSizer10->Add(m_staticText28, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_choiceDebugger =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceTypeArr, 0);

        flexGridSizer10->Add(m_choiceDebugger, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText32 = gsgs_Style().createStaticText(this, wxID_ANY, _("Build System:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer10->Add(m_staticText32, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_choiceBuild =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceTypeArr, 0);

        flexGridSizer10->Add(m_choiceBuild, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_stdBtnSizer4 = new DialogButtonSizer();

        boxSizer2->Add(m_stdBtnSizer4, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

        m_button6 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_button6->SetDefault();
        m_stdBtnSizer4->AddButton(m_button6);

        m_button8 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer4->AddButton(m_button8);
        m_stdBtnSizer4->Realize();

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_button6->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewProjectDialog::OnOKUI), NULL, this);

        ::GetProjectTemplateList(m_list);

        // If we have a workspace, set the project path in the workspace path
        if(gsgs_Workspace().IsValid()) {
            m_dirPicker->SetPath(gsgs_Workspace().GetWorkspace()->GetFileName().GetPath());
        }

        // Populate the project type choices
        std::unordered_set<wxString> S;
        for(auto& p : m_list) {
            if(S.count(p->GetName()) == 0) {
                S.insert(p->GetName());
            } else {
                continue;
            }
            int where = m_choiceType->Append(p->GetName());
            m_projectsMap.insert({ where, p });
        }

        if(!m_choiceType->IsEmpty()) { m_choiceType->SetSelection(0); }

        // list of compilers
        wxArrayString compilerChoices;

        // Get list of compilers from configuration file
        BuildSettingsConfigCookie cookie;
        CompilerPtr cmp = gsgs_Settings().GetFirstCompiler(cookie);
        while(cmp) {
            compilerChoices.Add(cmp->GetName());
            cmp = gsgs_Settings().GetNextCompiler(cookie);
        }
        m_choiceCompiler->Append(compilerChoices);
        if(!compilerChoices.IsEmpty()) { m_choiceCompiler->SetSelection(0); }

        // Get list of debuggers
        wxArrayString debuggerChoices;
        wxArrayString knownDebuggers = DebuggerMgr::Get().GetAvailableDebuggers();
        debuggerChoices.insert(debuggerChoices.end(), knownDebuggers.begin(), knownDebuggers.end());
        m_choiceDebugger->Append(debuggerChoices);
        if(!m_choiceDebugger->IsEmpty()) { m_choiceDebugger->SetSelection(0); }

        // build system
        {
            std::list<wxString> builders;
            wxArrayString knownBuilders;
            gsgs_Build().GetBuilders(builders);
            for(const wxString& builderName : builders) {
                knownBuilders.Add(builderName);
            }
            m_choiceBuild->Append(knownBuilders);
            if(!m_choiceBuild->IsEmpty()) { m_choiceBuild->SetSelection(0); }
        }
    }
    //-----------------------------------------------------------------
    NewProjectDialog::~NewProjectDialog() 
    {
        m_button6->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewProjectDialog::OnOKUI), NULL, this);
    }
    //-----------------------------------------------------------------
    void NewProjectDialog::OnOKUI(wxUpdateUIEvent& event)
    {
        event.Enable(!m_textCtrlName->IsEmpty() && !m_dirPicker->GetPath().IsEmpty());
    }
    //-----------------------------------------------------------------
    ProjectData NewProjectDialog::GetProjectData() const
    {
        int sel = m_choiceType->GetSelection();
        if(sel == wxNOT_FOUND) { return ProjectData(); }
        if(m_projectsMap.count(sel) == 0) { return ProjectData(); }

        ProjectData data;
        data.m_builderName = m_choiceBuild->GetStringSelection();
        data.m_cmpType = m_choiceCompiler->GetStringSelection();
        data.m_name = m_textCtrlName->GetValue();
        data.m_debuggerType = m_choiceDebugger->GetStringSelection();
        data.m_path = m_dirPicker->GetPath();
        data.m_sourceTemplate = "C++ Project";
        data.m_srcProject = m_projectsMap.find(sel)->second;
        return data;
    }
    //-----------------------------------------------------------------
    static const struct wxKeyName {
        wxKeyCode code;
        const char* name;
    } wxKeyNames[] = {
        { WXK_DELETE, wxTRANSLATE("DEL") }, { WXK_DELETE, wxTRANSLATE("DELETE") }, { WXK_BACK, wxTRANSLATE("BACK") },
        { WXK_INSERT, wxTRANSLATE("INS") }, { WXK_RETURN, wxTRANSLATE("ENTER") }, { WXK_RETURN, wxTRANSLATE("RETURN") },
        { WXK_PAGEUP, wxTRANSLATE("PGUP") }, { WXK_PAGEDOWN, wxTRANSLATE("PGDN") }, { WXK_LEFT, wxTRANSLATE("LEFT") },
        { WXK_RIGHT, wxTRANSLATE("RIGHT") }, { WXK_UP, wxTRANSLATE("UP") }, { WXK_DOWN, wxTRANSLATE("DOWN") },
        { WXK_HOME, wxTRANSLATE("HOME") }, { WXK_END, wxTRANSLATE("END") }, { WXK_SPACE, wxTRANSLATE("SPACE") },
        { WXK_TAB, wxTRANSLATE("TAB") }, { WXK_ESCAPE, wxTRANSLATE("ESC") }, { WXK_ESCAPE, wxTRANSLATE("ESCAPE") },
        { WXK_CANCEL, wxTRANSLATE("CANCEL") }, { WXK_CLEAR, wxTRANSLATE("CLEAR") }, { WXK_MENU, wxTRANSLATE("MENU") },
        { WXK_PAUSE, wxTRANSLATE("PAUSE") }, { WXK_CAPITAL, wxTRANSLATE("CAPITAL") }, { WXK_SELECT, wxTRANSLATE("SELECT") },
        { WXK_PRINT, wxTRANSLATE("PRINT") }, { WXK_EXECUTE, wxTRANSLATE("EXECUTE") },
        { WXK_SNAPSHOT, wxTRANSLATE("SNAPSHOT") }, { WXK_HELP, wxTRANSLATE("HELP") }, { WXK_ADD, wxTRANSLATE("ADD") },
        { WXK_SEPARATOR, wxTRANSLATE("SEPARATOR") }, { WXK_SUBTRACT, wxTRANSLATE("SUBTRACT") },
        { WXK_DECIMAL, wxTRANSLATE("DECIMAL") }, { WXK_DIVIDE, wxTRANSLATE("DIVIDE") },
        { WXK_NUMLOCK, wxTRANSLATE("NUM_LOCK") }, { WXK_SCROLL, wxTRANSLATE("SCROLL_LOCK") },
        { WXK_PAGEUP, wxTRANSLATE("PAGEUP") }, { WXK_PAGEDOWN, wxTRANSLATE("PAGEDOWN") },
        { WXK_NUMPAD_SPACE, wxTRANSLATE("SPACE") }, { WXK_NUMPAD_TAB, wxTRANSLATE("TAB") },
        { WXK_NUMPAD_ENTER, wxTRANSLATE("ENTER") }, { WXK_NUMPAD_HOME, wxTRANSLATE("HOME") },
        { WXK_NUMPAD_LEFT, wxTRANSLATE("LEFT") }, { WXK_NUMPAD_UP, wxTRANSLATE("UP") },
        { WXK_NUMPAD_RIGHT, wxTRANSLATE("RIGHT") }, { WXK_NUMPAD_DOWN, wxTRANSLATE("DOWN") },
        { WXK_NUMPAD_PAGEUP, wxTRANSLATE("PRIOR") }, { WXK_NUMPAD_PAGEUP, wxTRANSLATE("PGUP") },
        { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("NEXT") }, { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("PGDN") },
        { WXK_NUMPAD_END, wxTRANSLATE("END") }, { WXK_NUMPAD_BEGIN, wxTRANSLATE("BEGIN") },
        { WXK_NUMPAD_INSERT, wxTRANSLATE("INS") }, { WXK_NUMPAD_DELETE, wxTRANSLATE("DEL") },
        { WXK_NUMPAD_EQUAL, wxTRANSLATE("=") }, { WXK_NUMPAD_MULTIPLY, wxTRANSLATE("*") },
        { WXK_NUMPAD_ADD, wxTRANSLATE("+") }, { WXK_NUMPAD_SEPARATOR, wxTRANSLATE("KP_SEPARATOR") },
        { WXK_NUMPAD_SUBTRACT, wxTRANSLATE("-") }, { WXK_NUMPAD_DECIMAL, wxTRANSLATE(".") },
        { WXK_NUMPAD_DIVIDE, wxTRANSLATE("/") }, { WXK_WINDOWS_LEFT, wxTRANSLATE("WINDOWS_LEFT") },
        { WXK_WINDOWS_RIGHT, wxTRANSLATE("WINDOWS_RIGHT") }, { WXK_WINDOWS_MENU, wxTRANSLATE("WINDOWS_MENU") },
        { WXK_COMMAND, wxTRANSLATE("COMMAND") },
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // NewKeyShortcutDlg
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    NewKeyShortcutDlg::NewKeyShortcutDlg(wxWindow* parent, const WindowManager::HotKey& mid, 
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : 
        wxDialog(parent, id, title, pos, size, style)
        , m_mid(mid)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer3);

        m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);

        bSizer3->Add(m_panel1, 1, wxALL | wxEXPAND, 5);

        wxBoxSizer* boxSizer37 = new wxBoxSizer(wxVERTICAL);
        m_panel1->SetSizer(boxSizer37);

        wxBoxSizer* bSizer5 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer37->Add(bSizer5, 0, 0, 0);

        wxFlexGridSizer* flexGridSizer8 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer8->SetFlexibleDirection(wxBOTH);
        flexGridSizer8->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer8->AddGrowableCol(1);

        bSizer5->Add(flexGridSizer8, 1, wxALL | wxEXPAND, 5);

        m_staticTextActionLabel = gsgs_Style().createStaticText(m_panel1, wxID_ANY, _("Action:"), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer8->Add(m_staticTextActionLabel, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_staticTextAction = gsgs_Style().createStaticText(m_panel1, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer8->Add(m_staticTextAction, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        m_staticText10 = gsgs_Style().createStaticText(m_panel1, wxID_ANY, _("Key:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer8->Add(m_staticText10, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrl1 = gsgs_Style().createTextCtrl(m_panel1, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), wxTE_PROCESS_TAB | wxTE_PROCESS_ENTER);
        m_textCtrl1->SetToolTip(_("Hit any keyboard key. Don't use the modifier keys (e.g. 'Shift') here, use the checkboxes below"));
        flexGridSizer8->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

        m_staticText12 = gsgs_Style().createStaticText(m_panel1, wxID_ANY, _("Modifiers:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer8->Add(m_staticText12, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxBoxSizer* bSizer6 = new wxBoxSizer(wxHORIZONTAL);

        flexGridSizer8->Add(bSizer6, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_checkBoxCtrl = new wxCheckBox(m_panel1, wxID_ANY, _("Ctrl"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxCtrl->SetValue(false);

        bSizer6->Add(m_checkBoxCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxAlt = new wxCheckBox(m_panel1, wxID_ANY, _("Alt"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxAlt->SetValue(false);

        bSizer6->Add(m_checkBoxAlt, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxShift = new wxCheckBox(m_panel1, wxID_ANY, _("Shift"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxShift->SetValue(false);

        bSizer6->Add(m_checkBoxShift, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        wxBoxSizer* boxSizer34 = new wxBoxSizer(wxVERTICAL);

        bSizer5->Add(boxSizer34, 0, wxALL | wxEXPAND, 5);

        m_buttonClear = new Button(m_panel1, wxID_CLEAR, _("&Clear"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonClear->SetToolTip(_("Clear the keyboard shortcut"));

        boxSizer34->Add(m_buttonClear, 0, wxALL, 5);

        m_button30 = new Button(m_panel1, ID_SUGGEST, _("Suggest..."), wxDefaultPosition, wxSize(-1, -1), 0);
        m_button30->SetToolTip(_("Show list of all un-assigned keyboard shortcuts"));

        boxSizer34->Add(m_button30, 0, wxALL, 5);

        m_staticLine39 = new wxStaticLine(m_panel1, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLI_HORIZONTAL);

        boxSizer37->Add(m_staticLine39, 0, wxALL | wxEXPAND, 5);

        m_stdBtnSizer22 = new DialogButtonSizer();

        bSizer3->Add(m_stdBtnSizer22, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

        m_button24 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_button24->SetDefault();
        m_stdBtnSizer22->AddButton(m_button24);

        m_button26 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_stdBtnSizer22->AddButton(m_button26);
        m_stdBtnSizer22->Realize();

        if (GetParent()) {
            CentreOnParent();
        }
        else {
            CentreOnScreen();
        }

        // Connect events
        m_textCtrl1->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(NewKeyShortcutDlg::OnKeyDown), NULL, this);
        m_buttonClear->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewKeyShortcutDlg::OnClear), NULL, this);
        m_buttonClear->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewKeyShortcutDlg::OnClearUI), NULL, this);
        m_button30->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewKeyShortcutDlg::OnSuggest), NULL, this);

    #ifdef __WXOSX__
        m_checkBoxCtrl->SetLabel("Cmd");
    #else
        m_checkBoxCtrl->SetLabel("Ctrl");
    #endif
        Initialise(mid.accel);
    }
    //-----------------------------------------------------------------
    NewKeyShortcutDlg::~NewKeyShortcutDlg()
    {
        m_textCtrl1->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(NewKeyShortcutDlg::OnKeyDown), NULL, this);
        m_buttonClear->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewKeyShortcutDlg::OnClear), NULL, this);
        m_buttonClear->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewKeyShortcutDlg::OnClearUI), NULL, this);
        m_button30->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewKeyShortcutDlg::OnSuggest), NULL, this);
    }
    //-----------------------------------------------------------------
    void NewKeyShortcutDlg::Initialise(const wxString& accel)
    {
        KeyboardShortcut ks = FromString(accel);
        m_staticTextAction->SetLabel(m_mid.action);
        m_textCtrl1->ChangeValue(ks.key);

        m_checkBoxAlt->SetValue(ks.modifiers & kAlt);
        m_checkBoxCtrl->SetValue(ks.modifiers & kCtrl);
        m_checkBoxShift->SetValue(ks.modifiers & kShift);
    }
    //-----------------------------------------------------------------
    void NewKeyShortcutDlg::OnKeyDown(wxKeyEvent& event)
    {
        wxString text = ToString(event);
        if(text.IsEmpty()) {
            return;
        }
        m_textCtrl1->ChangeValue(text);
    }
    //-----------------------------------------------------------------
    wxString NewKeyShortcutDlg::ToString(wxKeyEvent& e)
    {
        wxString text;

        // int flags = e.GetModifiers();
        // if ( flags & wxMOD_ALT )
        //     text += wxT("Alt-");
        // if ( flags & wxMOD_CONTROL )
        //     text += wxT("Ctrl-");
        // if ( flags & wxMOD_SHIFT )
        //     text += wxT("Shift-");

        const int code = e.GetKeyCode();

        if(code >= WXK_F1 && code <= WXK_F12)
            text << _("F") << code - WXK_F1 + 1;
        else if(code >= WXK_NUMPAD0 && code <= WXK_NUMPAD9)
            text << code - WXK_NUMPAD0;
        else if(code >= WXK_SPECIAL1 && code <= WXK_SPECIAL20)
            text << _("SPECIAL") << code - WXK_SPECIAL1 + 1;
        else { // check the named keys
            size_t n;
            for(n = 0; n < WXSIZEOF(wxKeyNames); n++) {
                const wxKeyName& kn = wxKeyNames[n];
                if(code == kn.code && kn.code != WXK_COMMAND) {
                    text << wxGetTranslation(kn.name);
                    break;
                }
            }

            if(n == WXSIZEOF(wxKeyNames)) {
                // must be a simple key
                if(isascii(code)) {
                    text << (wxChar)code;
                } else {
                    return wxEmptyString;
                }
            }
        }

        return text;
    }
    //-----------------------------------------------------------------
    NewKeyShortcutDlg::KeyboardShortcut NewKeyShortcutDlg::FromString(const wxString& accelString)
    {
        wxString tmp_accel = accelString;
        tmp_accel.MakeLower();

        NewKeyShortcutDlg::KeyboardShortcut ks;
        wxArrayString tokens = ::wxStringTokenize(tmp_accel, "-+", wxTOKEN_STRTOK);

        for(size_t i = 0; i < tokens.GetCount(); ++i) {
            wxString token = tokens.Item(i);
            token.MakeLower();
            if(token == "shift") {
                ks.modifiers |= kShift;
            } else if(token == "alt") {
                ks.modifiers |= kAlt;
            } else if(token == "ctrl") {
                ks.modifiers |= kCtrl;
            } else {
                ks.key = tokens.Item(i);
            }
        }
        return ks;
    }
    //-----------------------------------------------------------------
    wxString NewKeyShortcutDlg::GetAccel() const
    {
        wxString accel;
        if(m_checkBoxCtrl->IsChecked()) accel << "Ctrl-";
        if(m_checkBoxAlt->IsChecked()) accel << "Alt-";
        if(m_checkBoxShift->IsChecked()) accel << "Shift-";
        accel << m_textCtrl1->GetValue();
        if(accel.EndsWith("-")) {
            accel.RemoveLast();
        }
        return accel;
    }
    //-----------------------------------------------------------------
    void NewKeyShortcutDlg::OnClear(wxCommandEvent& event)
    {
        // Clear the modifiers checkboxs
        m_checkBoxAlt->SetValue(false);
        m_checkBoxCtrl->SetValue(false);
        m_checkBoxShift->SetValue(false);

        m_textCtrl1->ChangeValue("");
    }
    //-----------------------------------------------------------------
    void NewKeyShortcutDlg::OnClearUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_checkBoxAlt->IsChecked() || m_checkBoxCtrl->IsChecked() || m_checkBoxShift->IsChecked() ||
            !m_textCtrl1->IsEmpty());
    }
    //-----------------------------------------------------------------
    void NewKeyShortcutDlg::OnSuggest(wxCommandEvent& event)
    {
        wxArrayString temp;
        WindowList temppanel;
        gsgs_Window().getFreeHotKeyList(temp);
        temppanel.resize(temp.size(), 0);
        SingleChoiceDialog dlg(this);
        dlg.initItem(temp, temppanel, 0);
        dlg.SetLabel(_("Select a Keyboard Shortcut"));
        if(dlg.ShowModal() == wxID_OK) {
            Initialise(dlg.GetSelection());
        }
    }
    //-----------------------------------------------------------------
    static int LINES_PER_PAGE = 8;
    static int Y_SPACER = 2;
    static int SCROLLBAR_WIDTH = 12;
    static int BOX_WIDTH = 400 + SCROLLBAR_WIDTH;
    //-----------------------------------------------------------------
    CodeBox::BmpVec_t CodeBox::m_defaultBitmaps;
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // CodeBox
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    CodeBox::CodeBox(wxWindow* parent, wxEvtHandler* eventObject, size_t flags)
        : wxPopupWindow(parent, wxBORDER_NONE)
        , m_index(0)
        , m_stc(NULL)
        , m_startPos(wxNOT_FOUND)
        , m_useLightColours(false)
        , m_eventObject(eventObject)
        , m_tipWindow(NULL)
        , m_flags(flags)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        mainSizer = new wxBoxSizer(wxHORIZONTAL);
        this->SetSizer(mainSizer);

        m_panelComposite = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);

        mainSizer->Add(m_panelComposite, 1, wxEXPAND, 5);

        boxSizer = new wxBoxSizer(wxHORIZONTAL);
        m_panelComposite->SetSizer(boxSizer);

        m_canvas = new wxPanel(m_panelComposite, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);

        boxSizer->Add(m_canvas, 1, wxALL | wxEXPAND, 0);

        CentreOnParent(wxBOTH);

        // Use the active editor's font (if any)
        IEditor* editor = GetManager()->GetActiveEditor();
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        m_ccFont = lexer->GetFontForSyle(0);
        SetCursor(wxCURSOR_HAND);

        // Set the default bitmap list
        ImageManager* bmpLoader = &gsgs_Image();
        m_bitmaps.push_back(bmpLoader->LoadBitmap("class")); // 0
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindClass, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("struct")); // 1
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindStruct, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("namespace")); // 2
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindModule, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("member_public")); // 3
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindField, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("typedef")); // 4
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindReference, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("member_private")); // 5
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("member_public")); // 6
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("member_protected")); // 7
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindVariable, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("function_private")); // 8
        m_bitmaps.push_back(bmpLoader->LoadBitmap("function_public"));  // 9
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindFunction, m_bitmaps.size() - 1 });
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindMethod, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("function_protected")); // 10

        m_bitmaps.push_back(bmpLoader->LoadBitmap("macro")); // 11
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindText, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("enum")); // 12
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindEnum, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("enumerator")); // 13
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindValue, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("mime-cpp"));    // 14
        m_bitmaps.push_back(bmpLoader->LoadBitmap("mime-h"));      // 15
        m_bitmaps.push_back(bmpLoader->LoadBitmap("mime-text"));   // 16
        m_bitmaps.push_back(bmpLoader->LoadBitmap("cpp_keyword")); // 17
        m_lspCompletionItemImageIndexMap.insert({ LSP::CompletionItem::kKindKeyword, m_bitmaps.size() - 1 });

        m_bitmaps.push_back(bmpLoader->LoadBitmap("enum")); // 18

        InitializeDefaultBitmaps();
        // Set the control width
        {
            wxMemoryDC memDC;
            wxBitmap bmp(1, 1);
            memDC.SelectObject(bmp);
            wxGCDC gcdc(memDC);
            gcdc.SetFont(m_ccFont);
            wxString sampleString('X', 50);
            wxSize sz = gcdc.GetTextExtent(sampleString);
            BOX_WIDTH = sz.GetWidth() + SCROLLBAR_WIDTH;
        }

        m_lineHeight = GetSingleLineHeight();
        int bitmapHeight = m_bitmaps[0].GetScaledHeight();
        if(bitmapHeight > m_lineHeight) { m_lineHeight = bitmapHeight; }

        // Calculate the size of the box
        int boxHeight = m_lineHeight * LINES_PER_PAGE;
        int boxWidth = BOX_WIDTH; // 100 pixels
        wxSize boxSize = wxSize(boxWidth, boxHeight);
        wxRect rect(boxSize);

        // Increase the size by 2 pixel for each dimension
        rect.Inflate(2, 2);
        SetSize(rect);
        m_canvas->SetBackgroundStyle(wxBG_STYLE_PAINT);

        m_canvas->Bind(wxEVT_PAINT, wxPaintEventHandler(CodeBox::OnPaint), this);
        m_canvas->Bind(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(CodeBox::OnEraseBackground), this);
        m_canvas->Bind(wxEVT_LEFT_DOWN, &CodeBox::OnLeftDClick, this);
        m_canvas->Bind(wxEVT_LEFT_DCLICK, &CodeBox::OnLeftDClick, this);
        m_canvas->Bind(wxEVT_MOUSEWHEEL, &CodeBox::OnMouseScroll, this);

        // Default colorus
        Colour colours = gsgs_Style().GetColours();

        m_useLightColours = true;
        if(editor) {
            wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
            if(Colour::IsDark(bgColour)) {
                colours.setBase(bgColour);
                m_useLightColours = !Colour::IsDark(bgColour);
            } else {
                colours.setBase(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
            }
        }

        // Default colours
        m_bgColour = colours.GetBgColour();
        m_penColour = colours.GetBorderColour();
        m_separatorColour = m_bgColour.ChangeLightness(98);
        m_textColour = colours.GetItemTextColour();
        m_selectedTextColour = colours.GetSelItemTextColour();
        m_selectedTextBgColour = colours.GetSelItemBgColour();
        m_alternateRowColour = colours.GetAlternateColour();

        m_bmpDown = wxXmlResource::Get()->LoadBitmap("cc-box-down");
        m_bmpDownEnabled = m_bmpDown.ConvertToDisabled();

        m_bmpUp = wxXmlResource::Get()->LoadBitmap("cc-box-up");
        m_bmpUpEnabled = m_bmpUp.ConvertToDisabled();

        if(m_useLightColours) {
            // swap between the disabled and enabeld bitmaps
            {
                wxBitmap tmpBitmap;
                tmpBitmap = m_bmpDown;
                m_bmpDown = m_bmpDownEnabled;
                m_bmpDownEnabled = tmpBitmap;
            }

            {
                wxBitmap tmpBitmap;
                tmpBitmap = m_bmpUp;
                m_bmpUp = m_bmpUpEnabled;
                m_bmpUpEnabled = tmpBitmap;
            }
        }
    }
    //-----------------------------------------------------------------
    CodeBox::~CodeBox()
    {
        m_canvas->Unbind(wxEVT_PAINT, wxPaintEventHandler(CodeBox::OnPaint), this);
        m_canvas->Unbind(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(CodeBox::OnEraseBackground), this);
        m_canvas->Unbind(wxEVT_LEFT_DOWN, &CodeBox::OnLeftDClick, this);
        m_canvas->Unbind(wxEVT_LEFT_DCLICK, &CodeBox::OnLeftDClick, this);
        m_canvas->Unbind(wxEVT_MOUSEWHEEL, &CodeBox::OnMouseScroll, this);
        DoDestroyTipWindow();
    }
    //-----------------------------------------------------------------
    void CodeBox::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }
    //-----------------------------------------------------------------
    void CodeBox::OnPaint(wxPaintEvent& event)
    {
        // Paint the background colour
        wxAutoBufferedPaintDC bdc(m_canvas);
        wxGCDC dc(bdc);

        // Invalidate all item rects before we draw them
        for(size_t i = 0; i < m_entries.size(); ++i) {
            m_entries.at(i)->m_itemRect = wxRect();
        }

        wxRect rect = GetClientRect();
        m_scrollArea = wxRect(rect.GetWidth() - SCROLLBAR_WIDTH + rect.GetTopLeft().x, rect.GetTopLeft().y, SCROLLBAR_WIDTH,
                              rect.GetHeight());

        dc.SetFont(m_ccFont);
        int singleLineHeight = m_lineHeight;

        m_canvas->PrepareDC(dc);
        // Draw the entire box with single solid colour
        dc.SetBrush(m_bgColour);
        dc.SetPen(m_penColour);
    #ifdef __WXOSX__
        rect.Inflate(1, 1);
    #endif
        dc.DrawRectangle(rect);

        // Shrink the rectangle by 2 to provide a 2 pixle
        // border
        rect.Deflate(1, 1);
        dc.SetBrush(m_bgColour);
        dc.SetPen(m_bgColour);
        dc.DrawRectangle(rect);

        // We want to have a scrollbar alike. We do this by reducing the size of the
        // items area by 5 pixels
        rect.SetWidth(rect.GetWidth() - SCROLLBAR_WIDTH);
        int firstIndex = m_index;
        int lastIndex = m_index + LINES_PER_PAGE;
        if(lastIndex > (int)m_entries.size()) { lastIndex = m_entries.size(); }

        // if the number of items to display is less from the number of lines
        // on the box, try prepending items from the top
        while(firstIndex > 0 && ((lastIndex - firstIndex) != LINES_PER_PAGE)) {
            --firstIndex;
        }

        // Paint the entries, starting from m_index => m_index + 7
        wxCoord y = 1; // Initial coord for drawing the first line
        wxCoord textX = m_bitmaps.empty() ? GetScaledSize(2) : (m_bitmaps.at(0).GetScaledWidth() + GetScaledSize(4));
        wxCoord bmpX = GetScaledSize(2);

        // Draw all items from firstIndex => lastIndex
        size_t rowCounter = 0;
        for(int i = firstIndex; i < lastIndex; ++i) {
            bool isSelected = (i == m_index);
            bool isLast = ((firstIndex + 1) == lastIndex);
            wxRect itemRect(0, y, rect.GetWidth(), singleLineHeight);

            // We colour lines in an alternate colours
            dc.SetBrush((rowCounter % 2) == 0 ? m_bgColour : m_alternateRowColour);
            dc.SetPen((rowCounter % 2) == 0 ? m_bgColour : m_alternateRowColour);
            dc.DrawRectangle(itemRect);
            ++rowCounter;

            if(isSelected) {
                // highlight the selection
                dc.SetBrush(m_selectedTextBgColour);
                dc.SetPen(m_selectedTextBgColour);
                dc.DrawRectangle(itemRect);
            }

            dc.SetPen(m_separatorColour);

            // If this item has a bitmap, draw it
            wxCodeCompletionBoxEntry::Ptr_t entry = m_entries.at(i);
            if(entry->GetImgIndex() != wxNOT_FOUND && entry->GetImgIndex() < (int)m_bitmaps.size()) {
                // Use a stock bitmap image
                const wxBitmap& bmp = m_bitmaps.at(entry->GetImgIndex());
                wxCoord bmpY = ((singleLineHeight - bmp.GetScaledHeight()) / 2) + itemRect.y;

                dc.DrawBitmap(bmp, bmpX, bmpY);
            } else if(entry->GetAlternateBitmap().IsOk()) {
                // We have an alternate bitmap, use it
                const wxBitmap& bmp = entry->GetAlternateBitmap();
                wxCoord bmpY = ((singleLineHeight - bmp.GetScaledHeight()) / 2) + itemRect.y;
                dc.DrawBitmap(bmp, bmpX, bmpY);
            }

            // Draw the text
            dc.SetClippingRegion(itemRect);

            // Truncate the text to fit the screen
            wxString choppedText;
            gsgs_Style().TruncateText(entry->GetText(), itemRect.GetWidth(), dc, choppedText);

            wxSize itemTextSize = dc.GetTextExtent(choppedText);
            wxCoord textY = ((singleLineHeight - itemTextSize.GetHeight()) / 2) + itemRect.y;
            dc.SetTextForeground(isSelected ? m_selectedTextColour : m_textColour);
            dc.DrawText(choppedText, textX, textY);
            dc.DestroyClippingRegion();
            y += singleLineHeight;
            dc.SetPen(m_separatorColour);
            if(!isLast) { dc.DrawLine(2, y, itemRect.GetWidth() + 2, y); }
            entry->m_itemRect = itemRect;
        }

        //===================--------------------------------------
        // draw the scrollbar
        //===================--------------------------------------

        wxRect scrollRect = m_scrollArea;
        dc.SetPen(m_penColour);
        dc.SetBrush(m_penColour);
        dc.DrawRectangle(scrollRect);
        DoDrawBottomScrollButton(dc);
        DoDrawTopScrollButton(dc);

        // Redraw the box border
        dc.SetPen(m_penColour);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(GetClientRect());
    }
    //-----------------------------------------------------------------
    void CodeBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries)
    {
        m_index = 0;
        m_stc = ctrl;
        m_allEntries = entries;

        // Keep the start position
        if(m_startPos == wxNOT_FOUND) { m_startPos = m_stc->WordStartPosition(m_stc->GetCurrentPos(), true); }

        // Fire "Showing" event
        if(!(m_flags & kNoShowingEvent)) {
            CodeCompleteEvent ccEvent(wxEVT_CCBOX_SHOWING);
            ccEvent.SetEntries(m_allEntries);
            ccEvent.SetEventObject(this);
            ccEvent.SetWord(GetFilter());
            gsgs_Event().ProcessEvent(ccEvent);
            m_allEntries.swap(ccEvent.GetEntries());
        }
        // Filter all duplicate entries from the list (based on simple string match)
        RemoveDuplicateEntries();

        // Filter results based on user input
        FilterResults();

        // If we got a single match - insert it
        if((m_entries.size() == 1) && (m_flags & kInsertSingleMatch)) {
            // single match
            InsertSelection();
            DoDestroy();
            return;
        }

        // Let the plugins modify the list of the entries
        int start = m_startPos;
        int end = m_stc->GetCurrentPos();

        wxString word = m_stc->GetTextRange(start, end); // the current word
        if(m_entries.empty()) {
            // no entries to display
            DoDestroy();
            return;
        }

        DoShowCompletionBox();

        if(m_stc) {
            // Set the focus back to the completion control
            m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
        }

        // Display the help window
        DoDisplayTipWindow();
    }
    //-----------------------------------------------------------------
    void CodeBox::DoDisplayTipWindow()
    {
        // Display the tooltip
        if(m_index >= 0 && m_index < (int)m_entries.size()) {
            wxString docComment = m_entries.at(m_index)->GetComment();
            docComment.Trim().Trim(false);
            if(docComment.IsEmpty() && m_entries.at(m_index)->m_tag) {
                // Format the comment on demand if the origin was a tag entry
                docComment = m_entries.at(m_index)->m_tag->FormatComment();
            }

            if(docComment.IsEmpty()) {
                // No tip to display
                DoDestroyTipWindow();

            } else if(!docComment.IsEmpty() && docComment != m_displayedTip) {
                // destroy old tip window
                DoDestroyTipWindow();

                // keep the old tip
                m_displayedTip = docComment;

                // Construct a new tip window and display the tip
                m_tipWindow = new BoxTipWindow(GetParent(), true, docComment, 1, false);
                m_tipWindow->PositionRelativeTo(this, m_stc->PointFromPosition(m_stc->GetCurrentPos()));

                // restore focus to the editor
                m_stc->CallAfter(&wxStyledTextCtrl::SetFocus);
            }

        } else {
            // Nothing to display, just destroy the old tooltip
            DoDestroyTipWindow();
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::StcCharAdded(wxStyledTextEvent& event)
    {
        event.Skip();
        int keychar = m_stc->GetCharAt(m_stc->PositionBefore(m_stc->GetCurrentPos()));
        if(((keychar >= 65) && (keychar <= 90)) ||  // A-Z
           ((keychar >= 97) && (keychar <= 122)) || // a-z
           ((keychar >= 48) && (keychar <= 57)) ||  // 0-9
           (keychar == 95) ||                       // _
           (keychar == 33))                         // !
        {
            DoUpdateList();
        } else {
            DoDestroy();
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::StcModified(wxStyledTextEvent& event)
    {
        event.Skip();
        DoUpdateList();
    }
    //-----------------------------------------------------------------
    void CodeBox::StcKeyDown(wxKeyEvent& event)
    {
        switch(event.GetKeyCode()) {
        case WXK_UP:
            DoScrollUp();
            break;
        case WXK_DOWN:
            DoScrollDown();
            break;
        case WXK_PAGEDOWN: {
            DoPgDown();
            break;
        }
        case WXK_PAGEUP: {
            DoPgUp();
            break;
        }
        case WXK_ESCAPE:
        case WXK_LEFT:
        case WXK_RIGHT:
        case WXK_HOME:
        case WXK_END:
        case WXK_DELETE:
        case WXK_NUMPAD_DELETE:
        case WXK_ALT:
        case WXK_WINDOWS_LEFT:
        case WXK_WINDOWS_RIGHT:
        case WXK_CONTROL:
            DoDestroy();
            event.Skip();
            break;
        case WXK_TAB:
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            // Insert the selection
            InsertSelection();
            DoDestroy();
            break;
        default:
            event.Skip();
            break;
        }
    }
    //-----------------------------------------------------------------
    int CodeBox::GetSingleLineHeight() const
    {
        wxBitmap bmp(1, 1);
        wxMemoryDC memDC(bmp);
        wxGCDC gcdc(memDC);
        gcdc.SetFont(m_ccFont);
        m_canvas->PrepareDC(gcdc);
        wxSize size = gcdc.GetTextExtent("Tp");
        int singleLineHeight = size.y + (2 * Y_SPACER) + 2; // the extra pixel is for the border line
        return singleLineHeight;
    }
    //-----------------------------------------------------------------
    bool CodeBox::FilterResults()
    {
        wxString word = GetFilter();
        if(word.IsEmpty()) {
            m_entries = m_allEntries;
            return false;
        }

        m_entries.clear();
        wxString lcFilter = word.Lower();
        // Smart sorting:
        // We preare the list of matches in the following order:
        // Exact matches
        // Starts with
        // Contains
        wxCodeCompletionBoxEntry::Vec_t exactMatches, exactMatchesI, startsWith, startsWithI, contains, containsI;
        for(size_t i = 0; i < m_allEntries.size(); ++i) {
            wxString entryText = m_allEntries.at(i)->GetText().BeforeFirst('(');
            entryText.Trim().Trim(false);
            wxString lcEntryText = entryText.Lower();

            // Exact match:
            if(word == entryText) {
                exactMatches.push_back(m_allEntries.at(i));

            } else if(lcEntryText == lcFilter) {
                exactMatchesI.push_back(m_allEntries.at(i));

            } else if(entryText.StartsWith(word)) {
                startsWith.push_back(m_allEntries.at(i));

            } else if(lcEntryText.StartsWith(lcFilter)) {
                startsWithI.push_back(m_allEntries.at(i));

            } else if(entryText.Contains(word)) {
                contains.push_back(m_allEntries.at(i));

            } else if(lcEntryText.Contains(lcFilter)) {
                containsI.push_back(m_allEntries.at(i));
            }
        }

        // Merge the results
        m_entries.insert(m_entries.end(), exactMatches.begin(), exactMatches.end());
        m_entries.insert(m_entries.end(), exactMatchesI.begin(), exactMatchesI.end());
        m_entries.insert(m_entries.end(), startsWith.begin(), startsWith.end());
        m_entries.insert(m_entries.end(), startsWithI.begin(), startsWithI.end());
        m_entries.insert(m_entries.end(), contains.begin(), contains.end());
        m_entries.insert(m_entries.end(), containsI.begin(), containsI.end());
        m_index = 0;
        return exactMatches.empty() && exactMatchesI.empty() && startsWith.empty() && startsWithI.empty();
    }
    //-----------------------------------------------------------------
    void CodeBox::InsertSelection()
    {
        if((m_index >= 0 && m_index < (int)m_entries.size()) && m_stc) {
            wxCodeCompletionBoxEntry::Ptr_t match = m_entries.at(m_index);
            // Let the owner override the default behavior
            CodeCompleteEvent e(wxEVT_CCBOX_SELECTION_MADE);
            e.SetWord(match->GetInsertText());
            e.SetEventObject(m_eventObject);
            e.SetEntry(match);
            if(!gsgs_Event().ProcessEvent(e)) {
                // execute the default behavior
                if(match->m_tag && match->m_tag->IsTemplateFunction()) {
                    CxxTemplateFunction tf(match->m_tag);
                    if(!tf.CanTemplateArgsDeduced()) {
                        // Insert a template function
                        CodeBoxManager::Get().CallAfter(
                            &CodeBoxManager::InsertSelectionTemplateFunction, match->GetText());
                        return;
                    }
                }
                CodeBoxManager::Get().CallAfter(&CodeBoxManager::InsertSelection, match);
            }
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::StcLeftDown(wxMouseEvent& event)
    {
        event.Skip();
        DoDestroy();
    }
    //-----------------------------------------------------------------
    void CodeBox::OnLeftDClick(wxMouseEvent& event)
    {
        // Check the scroll area
        if(m_scrollBottomRect.Contains(event.GetPosition())) {
            DoScrollDown();
        } else if(m_scrollTopRect.Contains(event.GetPosition())) {
            DoScrollUp();
        } else {
            for(size_t i = 0; i < m_entries.size(); ++i) {
                if(m_entries.at(i)->m_itemRect.Contains(event.GetPosition())) {
                    m_index = i;
                    InsertSelection();
                    CallAfter(&CodeBox::DoDestroy);
                    return;
                }
            }
        }
    }
    //-----------------------------------------------------------------
    wxCodeCompletionBoxEntry::Vec_t CodeBox::TagsToEntries(const TagEntryPtrList& tags)
    {
        wxCodeCompletionBoxEntry::Vec_t entries;
        for(size_t i = 0; i < tags.size(); ++i) {
            TagEntryPtr tag = tags[i];
            wxCodeCompletionBoxEntry::Ptr_t entry = TagToEntry(tag);
            entries.push_back(entry);
        }
        return entries;
    }
    //-----------------------------------------------------------------
    int CodeBox::GetImageId(TagEntryPtr entry)
    {
        wxString kind = entry->GetKind();
        wxString access = entry->GetAccess();
        if(kind == wxT("class")) return 0;
        if(kind == wxT("struct")) return 1;
        if(kind == wxT("namespace")) return 2;
        if(kind == wxT("variable")) return 3;
        if(kind == wxT("typedef")) return 4;
        if(kind == wxT("member") && access.Contains(wxT("private"))) return 5;
        if(kind == wxT("member") && access.Contains(wxT("public"))) return 6;
        if(kind == wxT("member") && access.Contains(wxT("protected"))) return 7;
        // member with no access? (maybe part of namespace??)
        if(kind == wxT("member")) return 6;
        if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("private"))) return 8;
        if((kind == wxT("function") || kind == wxT("prototype")) && (access.Contains(wxT("public")) || access.IsEmpty()))
            return 9;
        if((kind == wxT("function") || kind == wxT("prototype")) && access.Contains(wxT("protected"))) return 10;
        if(kind == wxT("macro")) return 11;
        if(kind == wxT("enum")) return 12;
        if(kind == wxT("enumerator")) return 13;
        if(kind == wxT("cpp_keyword")) return 17;
        if(kind == "cenum") return 18;
        return wxNOT_FOUND;
    }
    //-----------------------------------------------------------------
    void CodeBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrList& tags)
    {
        ShowCompletionBox(ctrl, TagsToEntries(tags));
    }
    //-----------------------------------------------------------------
    void CodeBox::DoUpdateList()
    {
        bool refreshList = FilterResults();

        int curpos = m_stc->GetCurrentPos();
        if(m_entries.empty() || curpos < m_startPos || refreshList) {
            if((m_entries.empty() || refreshList) && (m_flags & kRefreshOnKeyType)) {
                // Trigger a new CC box
                wxCommandEvent event(wxEVT_MENU, XRCID("complete_word"));
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
            }
            DoDestroy();

        } else {
            DoDisplayTipWindow();
            Refresh();
        }
    }
    //-----------------------------------------------------------------
    wxCodeCompletionBoxEntry::Ptr_t CodeBox::TagToEntry(TagEntryPtr tag)
    {
        wxString text = tag->GetDisplayName().Trim().Trim(false);
        int imgIndex = GetImageId(tag);
        wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(text, imgIndex);
        entry->m_tag = tag;
        entry->SetInsertText(text.BeforeFirst('('));
        entry->SetIsFunction(tag->IsMethod());
        entry->SetIsTemplateFunction(tag->IsTemplateFunction());

        wxString sig = tag->GetSignature();
        sig = sig.AfterFirst('(').BeforeLast(')');
        entry->SetSignature(sig);
        return entry;
    }
    //-----------------------------------------------------------------
    void CodeBox::DoScrollDown()
    {
        if(CanScrollDown()) {
            ++m_index;
            DoDisplayTipWindow();
            Refresh();
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::DoScrollUp()
    {
        if(CanScrollUp()) {
            --m_index;
            DoDisplayTipWindow();
            Refresh();
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::DoDestroy() { CodeBoxManager::Get().DestroyCCBox(); }
    //-----------------------------------------------------------------
    void CodeBox::DoDrawBottomScrollButton(wxDC& dc)
    {
        wxRect scrollRect = m_scrollArea;

        // Separate the scrollbar area into 2 big buttons: up and down
        m_scrollBottomRect =
            wxRect(wxPoint(scrollRect.GetTopLeft().x, scrollRect.GetTopLeft().y + scrollRect.GetHeight() / 2),
                   wxSize(scrollRect.GetWidth(), scrollRect.GetHeight() / 2));
        // Draw the up arrow
        wxCoord x, y;
        x = m_scrollBottomRect.x + ((m_scrollBottomRect.GetWidth() - m_bmpDown.GetWidth()) / 2);
        y = m_scrollBottomRect.y + m_scrollBottomRect.GetHeight() - (2 * m_bmpDown.GetHeight());

        wxBitmap bmp = CanScrollDown() ? m_bmpDownEnabled : m_bmpDown;
        dc.DrawBitmap(bmp, x, y);
    }
    //-----------------------------------------------------------------
    void CodeBox::DoDrawTopScrollButton(wxDC& dc)
    {
        wxRect scrollRect = m_scrollArea;

        // Separate the scrollbar area into 2 big buttons: up and down
        m_scrollTopRect = wxRect(scrollRect.GetTopLeft(), wxSize(scrollRect.GetWidth(), scrollRect.GetHeight() / 2));

        // Draw the up arrow
        wxCoord x, y;
        x = m_scrollTopRect.x + ((m_scrollTopRect.GetWidth() - m_bmpUp.GetWidth()) / 2);
        y = m_scrollTopRect.y + m_bmpUp.GetHeight();

        wxBitmap bmp = CanScrollUp() ? m_bmpUpEnabled : m_bmpUp;
        dc.DrawBitmap(bmp, x, y);
    }
    //-----------------------------------------------------------------
    bool CodeBox::CanScrollDown() { return ((m_index + 1) < (int)m_entries.size()); }
    //-----------------------------------------------------------------
    bool CodeBox::CanScrollUp() { return ((m_index - 1) >= 0); }
    //-----------------------------------------------------------------
    void CodeBox::DoDestroyTipWindow()
    {
        if(m_tipWindow) {
            m_tipWindow->Hide();
            m_tipWindow->Destroy();
            m_tipWindow = NULL;
            m_displayedTip.Clear();
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::DoShowCompletionBox()
    {
        CHECK_PTR_RET(m_stc);

        // guesstimate a line height
        wxMemoryDC dc;
        wxBitmap bmp(1, 1);
        dc.SelectObject(bmp);
        wxFont font = m_stc->StyleGetFont(0);
        dc.SetFont(font);
        wxSize textSize = dc.GetTextExtent("Tp");

        int lineHeight = textSize.y + 3; // 3 pixels margins
        wxRect rect = GetRect();

        // determine the box x position
        int wordStart = m_startPos;
        wxPoint pt = m_stc->PointFromPosition(wordStart);
        pt = m_stc->ClientToScreen(pt);
        pt.y += lineHeight;

        wxRect screenSize = GetDisplaySize();
        int displayIndex = wxDisplay::GetFromPoint(pt);
        if(displayIndex != wxNOT_FOUND) { screenSize = wxDisplay(displayIndex).GetGeometry(); }

        // Check Y axis
        if((pt.y + rect.GetHeight()) > screenSize.GetHeight()) {
            // the completion box goes out of the Y axis, move it up
            pt.y -= lineHeight;
            pt.y -= rect.GetHeight();
        }

        // Check X axis
        if((pt.x + rect.GetWidth()) > (screenSize.GetX() + screenSize.GetWidth())) {
            // the completion box goes out of the X axis. Move it to the left
            pt.x = (screenSize.GetX() + screenSize.GetWidth()) - rect.GetWidth();
        }
        Move(pt);
        Show();
    }
    //-----------------------------------------------------------------
    void CodeBox::RemoveDuplicateEntries()
    {
        gsgsStringSet matches;
        wxCodeCompletionBoxEntry::Vec_t uniqueList;
        for(size_t i = 0; i < m_allEntries.size(); ++i) {
            wxCodeCompletionBoxEntry::Ptr_t entry = m_allEntries.at(i);
            if(matches.count(entry->GetText()) == 0) {
                // new entry
                matches.insert(entry->GetText());
                uniqueList.push_back(entry);
            }
        }
        m_allEntries.swap(uniqueList);
    }
    //-----------------------------------------------------------------
    wxBitmap CodeBox::GetBitmap(TagEntryPtr tag)
    {
        InitializeDefaultBitmaps();
        int imgId = GetImageId(tag);
        if((imgId < 0) || (imgId >= (int)m_defaultBitmaps.size())) return wxNullBitmap;
        return m_defaultBitmaps.at(imgId);
    }
    //-----------------------------------------------------------------
    void CodeBox::InitializeDefaultBitmaps()
    {
        if(m_defaultBitmaps.empty()) {
            ImageManager* bmpLoader = &gsgs_Image();
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/class"));              // 0
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/struct"));             // 1
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/namespace"));          // 2
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_public"));      // 3
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/typedef"));            // 4
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_private"));     // 5
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_public"));      // 6
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/member_protected"));   // 7
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_private"));   // 8
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_public"));    // 9
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/function_protected")); // 10
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/typedef"));            // 11
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enum"));               // 12
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enumerator"));         // 13
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("mime/16/cpp"));              // 14
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("mime/16/h"));                // 15
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("mime/16/text"));             // 16
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/cpp_keyword"));        // 17
            m_defaultBitmaps.push_back(bmpLoader->LoadBitmap("cc/16/enum"));               // 18
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::OnMouseScroll(wxMouseEvent& event) { DoMouseScroll(event); }
    //-----------------------------------------------------------------
    void CodeBox::DoPgUp()
    {
        int newindex = m_index - (LINES_PER_PAGE - 1);
        if(newindex < 0) { newindex = 0; }
        // only refresh if there was an actual change
        if(m_index != newindex) {
            m_index = newindex;
            DoDisplayTipWindow();
            Refresh();
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::DoPgDown()
    {
        int newindex = m_index + (LINES_PER_PAGE - 1);
        if(newindex >= (int)m_entries.size()) { newindex = (int)m_entries.size() - 1; }
        // only refresh if there was an actual change
        if(m_index != newindex) {
            m_index = newindex;
            DoDisplayTipWindow();
            Refresh();
        }
    }
    //-----------------------------------------------------------------
    void CodeBox::DoMouseScroll(wxMouseEvent& event)
    {
        if(event.GetWheelRotation() < 0) {
            // down
            DoScrollDown();
        } else {
            DoScrollUp();
        }
    }
    //-----------------------------------------------------------------
    wxString CodeBox::GetFilter()
    {
        if(!m_stc) return "";
        int start = m_startPos;
        int end = m_stc->GetCurrentPos();

        return m_stc->GetTextRange(start, end);
    }
    //-----------------------------------------------------------------
    void CodeBox::ShowCompletionBox(wxStyledTextCtrl* ctrl, const LSP::CompletionItem::Vec_t& completions)
    {
        ShowCompletionBox(ctrl, LSPCompletionsToEntries(completions));
    }
    //-----------------------------------------------------------------
    wxCodeCompletionBoxEntry::Vec_t
    CodeBox::LSPCompletionsToEntries(const LSP::CompletionItem::Vec_t& completions)
    {
        wxCodeCompletionBoxEntry::Vec_t entries;
        for(size_t i = 0; i < completions.size(); ++i) {
            LSP::CompletionItem::Ptr_t completion = completions[i];
            wxString text = completion->GetLabel();
            int imgIndex = GetImageId(completion);
            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(text, imgIndex);

            wxString comment;
            if(!completion->GetDetail().IsEmpty()) { comment << completion->GetDetail(); }
            if(!completion->GetDocumentation().IsEmpty()) { comment << "\n" << completion->GetDocumentation(); }

            // if 'insertText' is provided, use it instead of the label
            wxString insertText;
            insertText = completion->GetInsertText().IsEmpty() ? completion->GetLabel() : completion->GetInsertText();
            if(completion->HasTextEdit()) {
                // According to the spec: if textEdit exists, we ignore 'insertText'
                insertText = completion->GetTextEdit()->GetNewText();
            }
            entry->SetInsertText(insertText);
            entry->SetImgIndex(imgIndex);
            entry->SetComment(comment);
            entry->SetIsFunction(completion->GetKind() == LSP::CompletionItem::kKindConstructor ||
                                 completion->GetKind() == LSP::CompletionItem::kKindFunction ||
                                 completion->GetKind() == LSP::CompletionItem::kKindMethod);
            entry->SetIsTemplateFunction(completion->GetLabel().Contains("<") && completion->GetLabel().Contains(">"));
            if(entry->IsFunction()) {
                // extract the function signature from the label
                wxString label = completion->GetLabel();
                wxString signature = label.AfterFirst('(');
                signature = signature.BeforeLast(')');
                signature.Trim().Trim(false);
                entry->SetSignature(signature);
            }
            entries.push_back(entry);
        }
        return entries;
    }
    //-----------------------------------------------------------------
    int CodeBox::GetImageId(LSP::CompletionItem::Ptr_t entry) const
    {
        if(m_lspCompletionItemImageIndexMap.count(entry->GetKind())) {
            return m_lspCompletionItemImageIndexMap.find(entry->GetKind())->second;
        }
        return m_lspCompletionItemImageIndexMap.find(LSP::CompletionItem::kKindText)->second;
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // NameAndDescDlg
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    NameAndDescDlg::NameAndDescDlg(wxWindow* parent, IManager* manager, const wxString& name,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        fgSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer1->AddGrowableCol(1);

        bSizer1->Add(fgSizer1, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Project name:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_textCtrlName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_textCtrlName->SetToolTip(_("Give a name to the template"));

        fgSizer1->Add(m_textCtrlName, 0, wxALL | wxEXPAND, 5);

        m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Project category:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString m_choiceTypeArr;
        m_choiceType = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), m_choiceTypeArr, 0);
        m_choiceType->SetToolTip(_("Choose a category for this template"));
#if wxVERSION_NUMBER >= 3000
        m_choiceType->SetHint(wxT(""));
#endif

        fgSizer1->Add(m_choiceType, 0, wxALL | wxEXPAND, 5);

        bSizer3 = new wxBoxSizer(wxVERTICAL);

        bSizer1->Add(bSizer3, 1, wxALL | wxEXPAND, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Description:"), wxDefaultPosition, wxSize(-1, -1), 0);
        bSizer3->Add(m_staticText2, 0, wxLEFT | wxRIGHT | wxTOP, 5);

        m_textCtrlDescription = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), wxTE_RICH2 | wxTE_MULTILINE);
        m_textCtrlDescription->SetToolTip(_("Set a description to this template"));

        bSizer3->Add(m_textCtrlDescription, 1, wxALL | wxEXPAND, 5);

        m_stdBtnSizer2 = new DialogButtonSizer();

        bSizer1->Add(m_stdBtnSizer2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer2->AddButton(m_buttonOK);

        m_button6 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_stdBtnSizer2->AddButton(m_button6);
        m_stdBtnSizer2->Realize();

        CentreOnParent(wxBOTH);

        std::list<ProjectPtr> projectList;
        GetProjectTemplateList(projectList);

        m_choiceType->Clear();
        std::list<ProjectPtr>::iterator iter = projectList.begin();
        std::set<wxString> categories;

        // Add the 'All' category
        categories.insert(wxT("All"));
        for(; iter != projectList.end(); iter++) {
            wxString internalType = (*iter)->GetProjectInternalType();
            if(internalType.IsEmpty()) internalType = wxT("Others");
            categories.insert(internalType);
        }

        std::set<wxString>::iterator cIter = categories.begin();
        for(; cIter != categories.end(); cIter++) {
            m_choiceType->Append((*cIter));
        }

        // Select the 'Console' to be the default
        int where = m_choiceType->FindString(wxT("Console"));
        if(where == wxNOT_FOUND) {
            where = 0;
        }
        m_choiceType->SetSelection(where);
        m_textCtrlName->SetValue(name);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_TOOLTIP_DESTROY, Event);
    wxDEFINE_EVENT(wxEVT_TOOLTIP_ITEM_EXPANDING, Event);
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // ResizableTooltip
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    ResizableTooltip::ResizableTooltip(wxEvtHandler* owner)
        : wxMiniFrame(gsgs_Event().TopFrame(), wxID_ANY, _("Inspect Variable"), wxDefaultPosition, 
            wxSize(300,200), wxRESIZE_BORDER|wxFRAME_NO_TASKBAR)
        , m_owner(owner)
    {
        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer2);

        m_treeCtrl = new DefaultTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTR_DEFAULT_STYLE);

        boxSizer2->Add(m_treeCtrl, 1, wxEXPAND, WXC_FROM_DIP(5));
        
        m_treeCtrl->SetSortFunction(nullptr);
        int height, width;
        gsgs_Config().Read("Tooltip/Height", height, 200);
        gsgs_Config().Read("Tooltip/Width", width, 300);
        wxSize toolsize(width, height);
        if((toolsize.GetWidth() < 300) || (toolsize.GetHeight() < 200)) { toolsize = wxSize(300, 200); }
        SetSize(toolsize);
        
        if(GetParent()) {
            CentreOnParent(wxBOTH);
        } else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_treeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(ResizableTooltip::OnItemExpanding), NULL, this);
        m_treeCtrl->Connect(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(ResizableTooltip::OnKeyDown), NULL, this);
    }
    //-----------------------------------------------------------------
    ResizableTooltip::~ResizableTooltip()
    {
        m_treeCtrl->Disconnect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(ResizableTooltip::OnItemExpanding), NULL, this);
        m_treeCtrl->Disconnect(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(ResizableTooltip::OnKeyDown), NULL, this);
        gsgs_Config().Write("Tooltip/Height", GetSize().GetHeight());
        gsgs_Config().Write("Tooltip/Width", GetSize().GetWidth());
    }
    //-----------------------------------------------------------------
    void ResizableTooltip::OnItemExpanding(wxTreeEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------
    void ResizableTooltip::Clear() { m_treeCtrl->DeleteAllItems(); }
    //-----------------------------------------------------------------
    void ResizableTooltip::ShowTip()
    {
        m_topLeft = ::wxGetMousePosition();
        Move(m_topLeft);
        gsgsDEBUG() << "Moving tooltip dialog to: (" << m_topLeft.x << "," << m_topLeft.y << ")";
        CallAfter(&ResizableTooltip::DoSetFocus);
    }
    //-----------------------------------------------------------------
    void ResizableTooltip::OnKeyDown(wxTreeEvent& event)
    {
        event.Skip();
        if(event.GetKeyCode() == WXK_ESCAPE) {
            //  Cancel this tip
            Event destroyEvent(wxEVT_TOOLTIP_DESTROY);
            gsgs_Event().AddPendingEvent(destroyEvent);
        }
    }
    //-----------------------------------------------------------------
    void ResizableTooltip::DoSetFocus()
    {
        Show(true);
        if(!m_treeCtrl->IsEmpty()) {
            m_treeCtrl->SetFocus();
            m_treeCtrl->SelectItem(m_treeCtrl->GetRootItem());
        }
    }
    //-----------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_SYMBOL_SELECTED, Event);
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // SelectSymbolDialog
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    SelectSymbolDialog::SelectSymbolDialog(wxWindow* parent, const SelectSymbolDialogEntry::List_t& entries)
        : wxDialog(parent, wxID_ANY, _("Select Symbol"), wxDefaultPosition, wxSize(-1,-1), 
            wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
    {
        boxSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer2);
        
        boxSizer10 = new wxBoxSizer(wxVERTICAL);
        
        boxSizer2->Add(boxSizer10, 1, wxALL|wxEXPAND, 5);
        
        m_dvListCtrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(500,200), wxDV_VERT_RULES|wxDV_ROW_LINES|wxDV_SINGLE);
        m_dvListCtrl->SetFocus();
        
        boxSizer10->Add(m_dvListCtrl, 1, wxALL|wxEXPAND, 5);
        
        m_dvListCtrl->AppendIconTextColumn(_("Name"), wxDATAVIEW_CELL_INERT, 500, wxALIGN_LEFT);
        m_dvListCtrl->AppendTextColumn(_("Ext"), wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT);
        m_stdBtnSizer4 = new DialogButtonSizer();
        
        boxSizer2->Add(m_stdBtnSizer4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
        
        m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer4->AddButton(m_buttonOK);
        
        m_buttonCancel = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_stdBtnSizer4->AddButton(m_buttonCancel);
        m_stdBtnSizer4->Realize();

        // Connect events
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(SelectSymbolDialog::OnItemActivated), NULL, this);
        m_buttonOK->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SelectSymbolDialog::OnOKUI), NULL, this);
        
        CenterOnParent();
        std::for_each(entries.begin(), entries.end(), [&](const SelectSymbolDialogEntry& entry) {
            AddSymbol(entry.name, entry.bmp, entry.help, entry.clientData);
        });
    
        if(m_dvListCtrl->GetItemCount()) {
            m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
        }
    }
    //-----------------------------------------------------------------
    SelectSymbolDialog::~SelectSymbolDialog()
    {
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(SelectSymbolDialog::OnItemActivated), NULL, this);
        m_buttonOK->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SelectSymbolDialog::OnOKUI), NULL, this);
        
        for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            wxClientData* cd = reinterpret_cast<wxClientData*>(m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i)));
            wxDELETE(cd);
        }
    }
    //-----------------------------------------------------------------
    void SelectSymbolDialog::AddSymbol(const wxString& name,
                                         const wxBitmap& bmp,
                                         const wxString& help,
                                         wxClientData* clientData)
    {
        wxVector<wxVariant> cols;
        cols.push_back(::MakeIconText(name, bmp));
        cols.push_back(help);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)clientData);
    }
    //-----------------------------------------------------------------
    void SelectSymbolDialog::OnOKUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(m_dvListCtrl->GetSelection().IsOk()); 
    }
    //-----------------------------------------------------------------
    wxClientData* SelectSymbolDialog::GetSelection() const
    {
        wxDataViewItem item = m_dvListCtrl->GetSelection();
        if (item)
        {
            wxClientData* cd = reinterpret_cast<wxClientData*>(m_dvListCtrl->GetItemData(item));
            return cd;
        }
    }
    //-----------------------------------------------------------------------------
    void SelectSymbolDialog::OnItemActivated(wxDataViewEvent& event) 
    { 
        EndModal(wxID_OK); 
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // MemoryView
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    MemoryView::MemoryView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTAB_TRAVERSAL)
    {
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);
        
        wxFlexGridSizer* flexGridSizer6 = new wxFlexGridSizer(0, 6, 0, 0);
        flexGridSizer6->SetFlexibleDirection( wxBOTH );
        flexGridSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
        flexGridSizer6->AddGrowableCol(1);
        
        bSizer1->Add(flexGridSizer6, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
        
        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Address:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        flexGridSizer6->Add(m_staticText2, 0, wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
        
        m_textCtrlExpression = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_PROCESS_ENTER);
        m_textCtrlExpression->SetToolTip(_("Address or pointer to watch"));
        
        flexGridSizer6->Add(m_textCtrlExpression, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
        
        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Size:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_staticText1->SetToolTip(_("Memory size to view"));
        
        flexGridSizer6->Add(m_staticText1, 0, wxLEFT|wxTOP|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
        
        wxArrayString m_choiceSizeArr;
        m_choiceSizeArr.Add(wxT("32"));
        m_choiceSizeArr.Add(wxT("64"));
        m_choiceSizeArr.Add(wxT("128"));
        m_choiceSizeArr.Add(wxT("256"));
        m_choiceSizeArr.Add(wxT("512"));
        m_choiceSizeArr.Add(wxT("1024"));
        m_choiceSizeArr.Add(wxT("2048"));
        m_choiceSizeArr.Add(wxT("4096"));
        m_choiceSize = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), m_choiceSizeArr, 0);
        m_choiceSize->SetToolTip(_("Memory size to view"));
        m_choiceSize->SetSelection(0);
        
        flexGridSizer6->Add(m_choiceSize, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
        
        m_staticText8 = gsgs_Style().createStaticText(this, wxID_ANY, _("Number of columns:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), 0);
        flexGridSizer6->Add(m_staticText8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));
        
        wxArrayString m_choiceColsArr;
        m_choiceColsArr.Add(wxT("4"));
        m_choiceColsArr.Add(wxT("8"));
        m_choiceColsArr.Add(wxT("16"));
        m_choiceColsArr.Add(wxT("32"));
        m_choiceCols = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), m_choiceColsArr, 0);
        m_choiceCols->SetToolTip(_("Number of columns to use per row"));
        m_choiceCols->SetSelection(2);
        
        flexGridSizer6->Add(m_choiceCols, 0, wxALL, WXC_FROM_DIP(5));
        
        wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
        
        bSizer1->Add(bSizer3, 1, wxEXPAND, WXC_FROM_DIP(5));
        
        m_textCtrlMemory = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_RICH2|wxTE_MULTILINE|wxTE_DONTWRAP);
        #ifdef __WXMSW__
        // To get the newer version of the font on MSW, we use font wxSYS_DEFAULT_GUI_FONT with family set to wxFONTFAMILY_TELETYPE
        wxFont m_textCtrlMemoryFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_textCtrlMemoryFont.SetFamily(wxFONTFAMILY_TELETYPE);
        #else
        wxFont m_textCtrlMemoryFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
        m_textCtrlMemoryFont.SetFamily(wxFONTFAMILY_TELETYPE);
        #endif
        m_textCtrlMemory->SetFont(m_textCtrlMemoryFont);
        
        bSizer3->Add(m_textCtrlMemory, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, WXC_FROM_DIP(5));
        
        wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);
        
        bSizer3->Add(bSizer4, 0, 0, WXC_FROM_DIP(5));
        
        m_buttonEvaluate = new Button(this, wxID_ANY, _("Evaluate"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonEvaluate->SetDefault();
        m_buttonEvaluate->SetToolTip(_("Evaluate the expression in the \"Address\" field"));
        
        bSizer4->Add(m_buttonEvaluate, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
        
        m_buttonUpdate = new Button(this, wxID_ANY, _("Update"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonUpdate->SetToolTip(_("Update the memory in the main display area to apply your changes"));
        
        bSizer4->Add(m_buttonUpdate, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
        
        // Connect events
        m_textCtrlExpression->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(MemoryView::OnTextEntered), NULL, this);
        m_choiceSize->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MemoryView::OnMemorySize), NULL, this);
        m_choiceCols->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MemoryView::OnNumberOfRows), NULL, this);
        m_textCtrlMemory->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MemoryView::OnTextDClick), NULL, this);
        m_buttonEvaluate->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MemoryView::OnEvaluate), NULL, this);
        m_buttonEvaluate->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MemoryView::OnEvaluateUI), NULL, this);
        m_buttonUpdate->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MemoryView::OnUpdate), NULL, this);
        m_buttonUpdate->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MemoryView::OnUpdateUI), NULL, this);
        m_textCtrlExpression->Connect(wxEVT_COMMAND_TEXT_ENTER, wxTextEventHandler(MemoryView::OnEvaluate), NULL, this);
    }
    //-----------------------------------------------------------------------------
    MemoryView::~MemoryView()
    {
        m_textCtrlExpression->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(MemoryView::OnTextEntered), NULL, this);
        m_choiceSize->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MemoryView::OnMemorySize), NULL, this);
        m_choiceCols->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MemoryView::OnNumberOfRows), NULL, this);
        m_textCtrlMemory->Disconnect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MemoryView::OnTextDClick), NULL, this);
        m_buttonEvaluate->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MemoryView::OnEvaluate), NULL, this);
        m_buttonEvaluate->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MemoryView::OnEvaluateUI), NULL, this);
        m_buttonUpdate->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MemoryView::OnUpdate), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnEvaluate(wxCommandEvent&)
    {
        UpdateDebuggerPane();
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnEvaluateUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_textCtrlExpression->GetValue().IsEmpty() == false);
    }
    //-----------------------------------------------------------------------------
    void MemoryView::Clear()
    {
        m_textCtrlExpression->Clear();
        m_textCtrlMemory->Clear();
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnUpdateUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(m_textCtrlMemory->IsModified()); 
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnMemorySize(wxCommandEvent&)
    {
        UpdateDebuggerPane();
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnNumberOfRows(wxCommandEvent & )
    {
        UpdateDebuggerPane();
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnTextDClick(wxMouseEvent &) 
    {
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnTextEntered(wxCommandEvent &)
    {
        UpdateDebuggerPane();
    }
    //-----------------------------------------------------------------------------
    void MemoryView::UpdateDebuggerPane()
    {
        DebugEvent dbgevent(wxEVT_DEBUGGER_REFRESH_PANE);
        gsgs_Event().AddPendingEvent(dbgevent);
    }
    //-----------------------------------------------------------------------------
    size_t MemoryView::GetColumns() const
    {
        long value(8);
        m_choiceCols->GetStringSelection().ToLong(&value);
        return value;
    }
    //-----------------------------------------------------------------------------
    size_t MemoryView::GetSize() const
    {
        long value(32);
        m_choiceSize->GetStringSelection().ToLong(&value);
        return value;
    }
    //-----------------------------------------------------------------------------
    void MemoryView::SetViewString(const wxString& text)
    {
        m_textCtrlMemory->Freeze();
        // get old value
        wxString oldValue = m_textCtrlMemory->GetValue();

        m_textCtrlMemory->Clear();
        // first check that we are trying to view the same addresses
        wxString newAddr = text.BeforeFirst(wxT(':'));
        wxString oldAddr = oldValue.BeforeFirst(wxT(':'));

        // set the new value
        m_textCtrlMemory->SetValue(text);

        if(newAddr == oldAddr) {

            size_t shortLen = text.Length() < oldValue.Length() ? text.Length() : oldValue.Length();

            // same address, loop over the strings
            wxTextAttr style;
            style.SetTextColour(wxT("RED"));

            long start(wxNOT_FOUND);
            bool needColouring(false);
            long possibleStart(0);

            for(size_t i = 0; i < shortLen; i++) {

                // reset word
                if((text.GetChar(i) == wxT(' ') || text.GetChar(i) == wxT('\n')) && needColouring) {

                    if(text.GetChar(i) != wxT('\n')) {
                        m_textCtrlMemory->SetStyle(start, (long)i, style);
                    }

                    start = wxNOT_FOUND;
                    needColouring = false;
                }

                if(text.GetChar(i) == wxT(' ') || text.GetChar(i) == wxT('\n')) {
                    possibleStart = (long)i;
                }

                if(text.GetChar(i) != oldValue.GetChar(i)) {
                    needColouring = true;
                    if(start == wxNOT_FOUND) {
                        start = possibleStart;
                    }
                }
            }
        }

        // loop over the text and set the address in grey colour
        wxTextAttr addrAttr;
        addrAttr.SetTextColour(wxT("GREY"));
        wxFont addrFont = m_textCtrlMemory->GetFont();
        addrFont.SetWeight(wxFONTWEIGHT_BOLD);
        addrFont.SetStyle(wxFONTSTYLE_ITALIC);

        addrAttr.SetFont(addrFont);

        wxArrayString lines = wxStringTokenize(text, wxT("\n"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); i++) {
            long addr_end = lines.Item(i).Find(wxT(':'));
            if(addr_end != wxNOT_FOUND) {
                long pos = m_textCtrlMemory->XYToPosition(0, (long)i);
                m_textCtrlMemory->SetStyle(pos, pos + addr_end, addrAttr);
            }
        }

        m_textCtrlMemory->DiscardEdits(); // make this operation undoable
        m_textCtrlMemory->Thaw();
    }
    //-----------------------------------------------------------------------------
    void MemoryView::OnUpdate(wxCommandEvent& e)
    {
        static wxRegEx reHex(wxT("[0][x][0-9a-fA-F][0-9a-fA-F]"));

        // extract the text memory from the text control and pass it to the debugger
        wxString memory;
        wxArrayString lines = wxStringTokenize(m_textCtrlMemory->GetValue(), wxT("\n"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); i++) {
            wxString line = lines.Item(i).AfterFirst(wxT(':')).BeforeFirst(wxT(':')).Trim().Trim(false);
            wxArrayString hexValues = wxStringTokenize(line, wxT(" "), wxTOKEN_STRTOK);
            for(size_t y = 0; y < hexValues.GetCount(); y++) {
                wxString hex = hexValues.Item(y);
                if(reHex.Matches(hex) && hex.Len() == 4) {
                    // OK
                    continue;
                } else {
                    wxMessageBox(wxString::Format(_("Invalid memory value: %s"), hex), _("光栅故事"),
                                 wxICON_WARNING | wxOK);
                    // update the pane to old value
                    UpdateDebuggerPane();
                    return;
                }
            }

            if(line.IsEmpty() == false) {
                memory << line << wxT(" ");
            }
        }

        // set the new memory
        memory = memory.Trim().Trim(false);
        DebugEvent memoryEvent(wxEVT_DEBUGGER_SET_MEMORY);
        memoryEvent.SetMemoryAddress(m_textCtrlExpression->GetValue());
        memoryEvent.SetMemoryBlockSize(GetSize());
        memoryEvent.SetMemoryBlockValue(memory);
        gsgs_Event().AddPendingEvent(memoryEvent);

        // update the view
        UpdateDebuggerPane();
    }
    //-----------------------------------------------------------------------------

    BEGIN_EVENT_TABLE(OpenResourceDialog, wxDialog)
    EVT_TIMER(XRCID("OR_TIMER"), OpenResourceDialog::OnTimer)
    END_EVENT_TABLE()
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // OpenResourceDialog
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    OpenResourceDialog::OpenResourceDialog(wxWindow* parent, IManager* manager, const wxString& initialSelection, 
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
        , m_manager(manager)
        , m_needRefresh(false)
        , m_lineNumber(wxNOT_FOUND)
    {
        Hide();

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_textCtrlResourceName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_PROCESS_ENTER);
        m_textCtrlResourceName->SetToolTip(_(
            "Type resource name to open.\nYou may use a space delimited list of words to narrow down the list of "
            "choices\ne.g. Typing: 'Open Dialog' will include results that contain both words \"Open\" _and_ \"Dialog\""));

        mainSizer->Add(m_textCtrlResourceName, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_dataview = new ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, 200)),
            wxDV_ROW_LINES | wxDV_MULTIPLE);

        mainSizer->Add(m_dataview, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_dataview->AppendTextColumn(_("Name"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dataview->AppendTextColumn(_("Impl"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dataview->AppendTextColumn(_("Full Name"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        wxBoxSizer* boxSizer19 = new wxBoxSizer(wxHORIZONTAL);

        mainSizer->Add(boxSizer19, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        boxSizer19->Add(fgSizer1, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_checkBoxFiles =
            new wxCheckBox(this, wxID_ANY, _("Show &files"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_checkBoxFiles->SetValue(false);

        fgSizer1->Add(m_checkBoxFiles, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxShowSymbols =
            new wxCheckBox(this, wxID_ANY, _("Show &symbols"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_checkBoxShowSymbols->SetValue(false);

        fgSizer1->Add(m_checkBoxShowSymbols, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        boxSizer19->Add(0, 0, 1, wxALL, WXC_FROM_DIP(5));

        m_stdBtnSizer2 = new DialogButtonSizer();

        boxSizer19->Add(m_stdBtnSizer2, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(10));

        m_buttonOK = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer2->AddButton(m_buttonOK);

        m_button6 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer2->AddButton(m_button6);
        m_stdBtnSizer2->Realize();

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_textCtrlResourceName->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(OpenResourceDialog::OnKeyDown), NULL, this);
        m_textCtrlResourceName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(OpenResourceDialog::OnText),
            NULL, this);
        m_textCtrlResourceName->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(OpenResourceDialog::OnEnter),
            NULL, this);
        m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,
            wxDataViewEventHandler(OpenResourceDialog::OnEntrySelected), NULL, this);
        m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(OpenResourceDialog::OnEntryActivated), NULL, this);
        m_checkBoxFiles->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(OpenResourceDialog::OnCheckboxfilesCheckboxClicked), NULL, this);
        m_checkBoxShowSymbols->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(OpenResourceDialog::OnCheckboxshowsymbolsCheckboxClicked),
            NULL, this);
        m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OpenResourceDialog::OnOK), NULL, this);
        m_buttonOK->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OpenResourceDialog::OnOKUI), NULL, this);

        m_dataview->SetBitmaps(&gsgs_Image().GetBitmaps());

        // initialize the file-type hash
        m_fileTypeHash["class"] = ImageManager::kClass;
        m_fileTypeHash["struct"] = ImageManager::kStruct;
        m_fileTypeHash["namespace"] = ImageManager::kNamespace;
        m_fileTypeHash["variable"] = ImageManager::kMemberPublic;
        m_fileTypeHash["typedef"] = ImageManager::kTypedef;
        m_fileTypeHash["member_private"] = ImageManager::kMemberPrivate;
        m_fileTypeHash["member_public"] = ImageManager::kMemberPublic;
        m_fileTypeHash["member_protected"] = ImageManager::kMemberProtected;
        m_fileTypeHash["member_protected"] = ImageManager::kMemberProtected;
        m_fileTypeHash["member"] = ImageManager::kMemberPublic;
        m_fileTypeHash["function"] = ImageManager::kFunctionPublic;
        m_fileTypeHash["function_private"] = ImageManager::kFunctionPrivate;
        m_fileTypeHash["function_public"] = ImageManager::kFunctionPublic;
        m_fileTypeHash["function_protected"] = ImageManager::kFunctionProtected;
        m_fileTypeHash["prototype"] = ImageManager::kFunctionPublic;
        m_fileTypeHash["prototype_public"] = ImageManager::kFunctionPublic;
        m_fileTypeHash["prototype_private"] = ImageManager::kFunctionPrivate;
        m_fileTypeHash["prototype_protected"] = ImageManager::kFunctionProtected;
        m_fileTypeHash["method"] = ImageManager::kFunctionPublic;
        m_fileTypeHash["method_public"] = ImageManager::kFunctionPublic;
        m_fileTypeHash["method_private"] = ImageManager::kFunctionPrivate;
        m_fileTypeHash["method_protected"] = ImageManager::kFunctionProtected;
        m_fileTypeHash["macro"] = ImageManager::kMacro;
        m_fileTypeHash["enum"] = ImageManager::kEnum;
        m_fileTypeHash["enumerator"] = ImageManager::kEnumerator;

        m_timer = new wxTimer(this, XRCID("OR_TIMER"));

        m_textCtrlResourceName->SetFocus();
        SetLabel(_("Open resource..."));

        // load all files from the workspace
        if(m_manager->IsOpen()) {
            wxArrayString projects;
            m_manager->GetWorkspace()->GetProjectList(projects);

            for(size_t i = 0; i < projects.GetCount(); i++) {
                ProjectPtr p = m_manager->GetWorkspace()->GetProject(projects.Item(i));
                if(p) {
                    const Project::FilesMap_t& files = p->GetFiles();
                    // convert std::vector to wxArrayString
                    std::for_each(files.begin(), files.end(), [&](const Project::FilesMap_t::value_type& vt) {
                        wxFileName fn(vt.second->GetFilename());
                        m_files.insert(std::make_pair(fn.GetFullName(), fn.GetFullPath()));
                    });
                }
            }
        }

        wxString lastStringTyped;
        gsgs_Config().Read("OpenResourceDialog/SearchString", lastStringTyped, wxString());
        bool showFiles, showSymbols;
        gsgs_Config().Read("OpenResourceDialog/ShowFiles", showFiles, true);
        gsgs_Config().Read("OpenResourceDialog/ShowSymbols", showSymbols, true);

        // Set the initial selection
        // We use here 'SetValue' so an event will get fired and update the control
        if(!initialSelection.IsEmpty()) {
            m_textCtrlResourceName->SetValue(initialSelection);
            m_textCtrlResourceName->SelectAll();
        } else if(!lastStringTyped.IsEmpty()) {
            m_textCtrlResourceName->SetValue(lastStringTyped);
            m_textCtrlResourceName->SelectAll();
        }

        m_checkBoxFiles->SetValue(showFiles);
        m_checkBoxShowSymbols->SetValue(showSymbols);
    }
    //-----------------------------------------------------------------
    OpenResourceDialog::~OpenResourceDialog()
    {
        m_textCtrlResourceName->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(OpenResourceDialog::OnKeyDown), NULL,
            this);
        m_textCtrlResourceName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED,
            wxCommandEventHandler(OpenResourceDialog::OnText), NULL, this);
        m_textCtrlResourceName->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(OpenResourceDialog::OnEnter),
            NULL, this);
        m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED,
            wxDataViewEventHandler(OpenResourceDialog::OnEntrySelected), NULL, this);
        m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(OpenResourceDialog::OnEntryActivated), NULL, this);
        m_checkBoxFiles->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(OpenResourceDialog::OnCheckboxfilesCheckboxClicked), NULL,
            this);
        m_checkBoxShowSymbols->Disconnect(
            wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(OpenResourceDialog::OnCheckboxshowsymbolsCheckboxClicked), NULL, this);
        m_buttonOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OpenResourceDialog::OnOK), NULL,
            this);
        m_buttonOK->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OpenResourceDialog::OnOKUI), NULL, this);

        m_timer->Stop();
        wxDELETE(m_timer);

        // Store current values
        gsgs_Config().Write("OpenResourceDialog/ShowFiles", m_checkBoxFiles->IsChecked());
        gsgs_Config().Write("OpenResourceDialog/ShowSymbols", m_checkBoxShowSymbols->IsChecked());
        gsgs_Config().Write("OpenResourceDialog/SearchString", m_textCtrlResourceName->GetValue());
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnText(wxCommandEvent& event)
    {
        event.Skip();
        m_timer->Stop();
        m_timer->Start(200, true);

        wxString filter = m_textCtrlResourceName->GetValue();
        filter.Trim().Trim(false);
        m_needRefresh = true;
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnUsePartialMatching(wxCommandEvent&)
    {
        DoPopulateList();
        m_textCtrlResourceName->SetFocus();
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnEntryActivated(wxDataViewEvent& event)
    {
        CHECK_ITEM_RET(event.GetItem());
        OpenResourceDialogItemData* data =
            reinterpret_cast<OpenResourceDialogItemData*>(m_dataview->GetItemData(event.GetItem()));
        if(data) { EndModal(wxID_OK); }
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::DoPopulateList()
    {
        wxString name = m_textCtrlResourceName->GetValue();
        name.Trim().Trim(false);
        if(name.IsEmpty()) { return; }

        Clear();

        wxWindowUpdateLocker locker(m_dataview);

        // First add the workspace files
        long nLineNumber;
        wxString modFilter;
        GetLineNumberFromFilter(name, modFilter, nLineNumber);
        name.swap(modFilter);

        m_lineNumber = nLineNumber;

        // Prepare the user filter
        m_userFilters.Clear();
        m_userFilters = ::wxStringTokenize(name, " \t", wxTOKEN_STRTOK);
        for(size_t i = 0; i < m_userFilters.GetCount(); ++i) {
            m_userFilters.Item(i).MakeLower();
        }

        // Build the filter class
        if(m_checkBoxFiles->IsChecked()) { DoPopulateWorkspaceFile(); }
        if(m_checkBoxShowSymbols->IsChecked() && (nLineNumber == -1)) { DoPopulateTags(); }
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::DoPopulateTags()
    {
        // Next, add the tags
        TagEntryPtrList tags;
        if(m_userFilters.IsEmpty()) return;
        m_manager->GetTagsManager()->GetTagsByPartialNames(m_userFilters, tags);
        for(size_t i = 0; i < tags.size(); i++) {
            TagEntryPtr tag = tags.at(i);

            // Filter out non relevanting entries
            if(!m_filters.IsEmpty() && m_filters.Index(tag->GetKind()) == wxNOT_FOUND) continue;

            if(!MatchesFilter(tag->GetFullDisplayName())) { continue; }

            wxString name(tag->GetName());

            // keep the fullpath
            wxString fullname;
            if(tag->IsMethod()) {
                fullname = wxString::Format(wxT("%s::%s%s"), tag->GetScope().c_str(), tag->GetName().c_str(),
                                            tag->GetSignature().c_str());
                DoAppendLine(tag->GetName(), fullname, (tag->GetKind() == wxT("function")),
                             new OpenResourceDialogItemData(tag->GetFile(), tag->GetLine(), tag->GetPattern(),
                                                            tag->GetName(), tag->GetScope()),
                             DoGetTagImg(tag));
            } else {

                fullname = wxString::Format(wxT("%s::%s"), tag->GetScope().c_str(), tag->GetName().c_str());
                DoAppendLine(tag->GetName(), fullname, false,
                             new OpenResourceDialogItemData(tag->GetFile(), tag->GetLine(), tag->GetPattern(),
                                                            tag->GetName(), tag->GetScope()),
                             DoGetTagImg(tag));
            }
        }
        wxString filter = (m_userFilters.GetCount() == 1) ? m_userFilters.Item(0) : "";
        if(!filter.IsEmpty()) {
            wxDataViewItem matchedItem =
                m_dataview->FindNext(wxDataViewItem(nullptr), filter, 0,
                                     wxDV_SEARCH_ICASE | wxDV_SEARCH_METHOD_EXACT | wxDV_SEARCH_INCLUDE_CURRENT_ITEM);
            if(matchedItem.IsOk()) { DoSelectItem(matchedItem); }
        }
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::DoPopulateWorkspaceFile()
    {
        // do we need to include files?
        if(!m_filters.IsEmpty() && m_filters.Index(KIND_FILE) == wxNOT_FOUND) return;

        if(!m_userFilters.IsEmpty()) {

            std::unordered_multimap<wxString, wxString>::iterator iter = m_files.begin();
            const int maxFileSize = 100;
            int counter = 0;
            for(; (iter != m_files.end()) && (counter < maxFileSize); iter++) {
                const wxString& fullpath = iter->second;
                if(!MatchesFilter(fullpath)) continue;

                wxFileName fn(iter->second);
                int imgId = gsgs_Image().GetMimeImageId(fn.GetFullName());
                DoAppendLine(fn.GetFullName(), fn.GetFullPath(), false,
                             new OpenResourceDialogItemData(fn.GetFullPath(), -1, wxT(""), fn.GetFullName(), wxT("")),
                             imgId);
                ++counter;
            }
        }
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::Clear()
    {
        // list control does not own the client data, we need to free it ourselves
        for(size_t i = 0; i < m_dataview->GetItemCount(); ++i) {
            OpenResourceDialogItemData* cd = GetItemData(m_dataview->RowToItem(i));
            wxDELETE(cd);
        }
        m_dataview->DeleteAllItems();
        m_userFilters.Clear();
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OpenSelection(const OpenResourceDialogItemData& selection, IManager* manager)
    {
        // send event to the plugins to see if they want
        // to open this file
        wxString file_path = selection.m_file;
        Event activateEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
        activateEvent.SetFileName(file_path);
        if(gsgs_Event().ProcessEvent(activateEvent)) return;

        if(manager && manager->OpenFile(selection.m_file, wxEmptyString, selection.m_line - 1)) {
            IEditor* editor = manager->GetActiveEditor();
            if(editor && !selection.m_name.IsEmpty() && !selection.m_pattern.IsEmpty()) {
                editor->FindAndSelectV(selection.m_pattern, selection.m_name);
            }
        }
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnKeyDown(wxKeyEvent& event)
    {
        event.Skip();
        if(m_dataview->IsEmpty()) { return; }

        if(event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_NUMPAD_UP ||
           event.GetKeyCode() == WXK_NUMPAD_DOWN) {
            event.Skip(false);

            if(GetDataview()->GetSelectedItemsCount() == 0) {
                // Just select the first entry
                DoSelectItem(GetDataview()->RowToItem(0));
            } else {
                // fire char hook event to the DV control
                // so it will handle the keyboard movement itself
                wxKeyEvent charHook = event;
                charHook.SetEventObject(m_dataview);
                charHook.SetEventType(wxEVT_CHAR_HOOK);
                GetDataview()->GetEventHandler()->ProcessEvent(charHook);
            }
        
            // Set the focus back to the text control
            m_textCtrlResourceName->CallAfter(&wxTextCtrl::SetFocus);
        }
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnOK(wxCommandEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_dataview->GetSelectedItemsCount()); }
    //-----------------------------------------------------------------
    bool OpenResourceDialogItemData::IsOk() const { return m_file.IsEmpty() == false; }
    //-----------------------------------------------------------------
    void OpenResourceDialog::DoSelectItem(const wxDataViewItem& item)
    {
        CHECK_ITEM_RET(item);
        GetDataview()->Select(item);
        GetDataview()->EnsureVisible(item);
        GetDataview()->Refresh();
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::DoAppendLine(const wxString& name, const wxString& fullname, bool boldFont,
                                          OpenResourceDialogItemData* clientData, int imgid)
    {
        wxString prefix;
        clientData->m_impl = boldFont;
        wxVector<wxVariant> cols;
        cols.push_back(::MakeBitmapIndexText(prefix + name, imgid));
        cols.push_back(clientData->m_impl ? wxString("X") : wxString());
        cols.push_back(fullname);
        m_dataview->AppendItem(cols, (wxUIntPtr)clientData);
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnTimer(wxTimerEvent &)
    {
        if(m_needRefresh) { DoPopulateList(); }

        m_needRefresh = false;

        // If there is only 1 item in the resource window then highlight it.
        // This allows the user to hit ENTER immediately after to open the item, nice shortcut.
        if(m_dataview->GetItemCount() == 1) { DoSelectItem(m_dataview->RowToItem(0)); }
    }
    //-----------------------------------------------------------------
    int OpenResourceDialog::DoGetTagImg(TagEntryPtr tag)
    {
        wxString kind = tag->GetKind();
        wxString access = tag->GetAccess();

        // Build the access string
        wxString accessString;
        accessString << kind;
        if(!access.IsEmpty()) { accessString << "_" << access; }

        int imgId = ImageManager::kMemberPublic; // The default
        if(m_fileTypeHash.count(accessString)) { imgId = m_fileTypeHash[accessString]; }
        return gsgs_Image().GetMimeImageId(imgId);
    }
    //-----------------------------------------------------------------
    bool OpenResourceDialog::MatchesFilter(const wxString& name)
    {
        wxString filter = m_textCtrlResourceName->GetValue();
        if(filter.Contains(':') == true) { filter = filter.BeforeLast(':'); }
        return FileUtil::FuzzyMatch(filter, name);
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnCheckboxfilesCheckboxClicked(wxCommandEvent& event) { DoPopulateList(); }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnCheckboxshowsymbolsCheckboxClicked(wxCommandEvent& event) { DoPopulateList(); }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnEnter(wxCommandEvent& event)
    {
        event.Skip();
        EndModal(wxID_OK);
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::OnEntrySelected(wxDataViewEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------
    std::vector<OpenResourceDialogItemData*> OpenResourceDialog::GetSelections() const
    {
        std::vector<OpenResourceDialogItemData*> selections;
        wxDataViewItemArray items;
        m_dataview->GetSelections(items);
        if(items.IsEmpty()) { return selections; }

        for(size_t i = 0; i < items.GetCount(); ++i) {
            OpenResourceDialogItemData* data = GetItemData(items.Item(i));
            if(data) {
                if(m_lineNumber != wxNOT_FOUND) { data->m_line = m_lineNumber; }
                selections.push_back(data);
            }
        }
        return selections;
    }
    //-----------------------------------------------------------------
    void OpenResourceDialog::GetLineNumberFromFilter(const wxString& filter, wxString& modFilter, long& lineNumber)
    {
        modFilter = filter;
        lineNumber = -1;
        static wxRegEx reNumber(":([0-9]+)", wxRE_ADVANCED);
        if(reNumber.IsValid() && reNumber.Matches(modFilter)) {
            wxString strLineNumber;
            strLineNumber = reNumber.GetMatch(modFilter, 1);
            strLineNumber.ToCLong(&lineNumber);
            reNumber.Replace(&modFilter, "");
        }
    }
    //-----------------------------------------------------------------
    OpenResourceDialogItemData* OpenResourceDialog::GetItemData(const wxDataViewItem& item) const
    {
        return reinterpret_cast<OpenResourceDialogItemData*>(m_dataview->GetItemData(item));
    }
    //-----------------------------------------------------------------
    BEGIN_EVENT_TABLE(MacrosDlg, wxDialog)
    EVT_MENU(XRCID("copy_macro"), MacrosDlg::OnCopy)
    END_EVENT_TABLE()
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // MacrosDlg
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    MacrosDlg::MacrosDlg(wxWindow* parent, int content, ProjectPtr project, IEditor* editor,
        wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
        , m_item(wxNOT_FOUND)
        , m_content(content)
        , m_project(project)
        , m_editor(editor)
    {

        wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer3);

        wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);

        bSizer3->Add(bSizer4, 1, wxEXPAND, 5);

        m_listCtrlMacros = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLC_VRULES | wxLC_HRULES | wxLC_SINGLE_SEL | wxLC_REPORT);

        bSizer4->Add(m_listCtrlMacros, 1, wxALL | wxEXPAND, 5);

        m_stdBtnSizer2 = new DialogButtonSizer();

        bSizer3->Add(m_stdBtnSizer2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_button4 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_button4->SetDefault();
        m_stdBtnSizer2->AddButton(m_button4);

        m_button6 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_stdBtnSizer2->AddButton(m_button6);
        m_stdBtnSizer2->Realize();

        // Connect events
        m_listCtrlMacros->Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(MacrosDlg::OnItemRightClick), NULL, this);

        Initialize();

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
    }
    //-----------------------------------------------------------------
    MacrosDlg::~MacrosDlg()
    {
        m_listCtrlMacros->Disconnect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(MacrosDlg::OnItemRightClick), NULL, this);
    }
    //-----------------------------------------------------------------
    void MacrosDlg::OnItemRightClick(wxListEvent& event)
    {
        m_item = event.m_itemIndex;
        wxMenu menu;
        menu.Append(XRCID("copy_macro"), wxT("Copy macro name"), NULL);
        PopupMenu(&menu);
    }
    //-----------------------------------------------------------------
    void MacrosDlg::Initialize()
    {
        m_listCtrlMacros->InsertColumn(0, _("Macro"));
        m_listCtrlMacros->InsertColumn(1, _("Description"));

        // Only show third column if we can expand the macros
        if(m_project && m_editor) {
            m_listCtrlMacros->InsertColumn(2, wxT("Value"));
        }

        switch(m_content) {
        case MacrosExternalTools:
            AddMacro(wxT("$(CurrentSelection)"), _("Expands to the selected text in the active editor"));
            AddMacro(wxT("$(CurrentSelectionRange)"),
                     _("Expands to the selected text range in bytes from beginning of file, eg. 150:200"));
            AddMacro(wxT("$(ProjectOutputFile)"), _("Expands to the project binary output file"));
            AddMacro(wxT("$(ProjectWorkingDirectory)"), _("Expands to the project's build working directory"));
            AddMacro(wxT("$(ProjectRunWorkingDirectory)"), _("Expands to the project's run working directory"));
        // fall ...
        case MacrosProject:
            AddMacro(wxT("$(ProjectPath)"), _("Expands to project's path"));
            AddMacro(wxT("$(WorkspacePath)"), _("Expands to workspace's path"));
            AddMacro(wxT("$(WorkspaceConfiguration)"), _("Expands to the workspace selected configuration"));
            AddMacro(wxT("$(ProjectName)"), _("Expands to the current project name as appears in the 'File View'"));
            AddMacro(wxT("$(IntermediateDirectory)"),
                     _("Expands to the current project intermediate directory path, as set in the project settings"));
            AddMacro(wxT("$(ConfigurationName)"), _("Expands to the current project selected configuration"));
            AddMacro(wxT("$(OutDir)"), _("An alias to $(IntermediateDirectory)"));
            AddMacro(wxT("$(CurrentFileName)"), _("Expands to current file name (without extension and path)"));
            AddMacro(wxT("$(CurrentFilePath)"), _("Expands to current file path"));
            AddMacro(wxT("$(CurrentFileFullPath)"), _("Expands to current file full path (path and full name)"));
            AddMacro(wxT("$(CurrentFileFullName)"), _("Expands to current file full name (name and extension)"));
            AddMacro(wxT("$(User)"), _("Expands to logged-in user as defined by the OS"));
            AddMacro(wxT("$(Date)"), _("Expands to current date"));
            AddMacro(wxT("$(gsgsPath)"),
                     _("Expands to gsgs's startup directory on (e.g. on Unix it expands to ~/.gsgs/"));
            AddMacro(
                wxT("$(ProjectFiles)"),
                _("A space delimited string containing all of the project files in a relative path to the project file"));
            AddMacro(wxT("$(ProjectFilesAbs)"),
                     _("A space delimited string containing all of the project files in an absolute path"));
            AddMacro(wxT("`expression`"), _("backticks: evaluates the expression inside the backticks into a string"));
            AddMacro(wxT("$(OutputFile)"), _("The output file"));
            break;

        case MacrosCompiler:
            AddMacro(wxT("$(CXX)"), _("Expands to the compiler name as set in the Tools tab"));
            AddMacro(wxT("$(Operation)"), _("Expands to the source switch (usually, -c)"));
            AddMacro(wxT("$(FileFullPath)"), _("The file full path (includes path+name+extension)"));
            AddMacro(wxT("$(FileFullName)"), _("The file full name (includes name+extension)"));
            AddMacro(wxT("$(FileName)"), _("The file name (name only)"));
            AddMacro(wxT("$(FilePath)"), _("The file's path with UNIX slashes, including terminating separator"));
            AddMacro(wxT("$(CXXFLAGS)"), _("Expands to the compiler options as set in the project settings"));
            AddMacro(wxT("$(RcCompilerName)"), _("Expands to the resource compiler name"));
            AddMacro(wxT("$(IntermediateDirectory)"),
                     _("Expands to the current project intermediate directory path, as set in the project settings"));
            AddMacro(wxT("$(ConfigurationName)"), _("Expands to the current project selected configuration"));
            AddMacro(wxT("$(OutDir)"), _("An alias to $(IntermediateDirectory)"));
            AddMacro(wxT("$(LinkerName)"), _("Expands to the linker name as set in the Tools tab"));
            AddMacro(wxT("$(AR)"), _("Expands to the archive tool (e.g. ar) name as set in the Tools tab"));
            AddMacro(wxT("$(SharedObjectLinkerName)"),
                     _("Expands to the shared object linker name as set in the Tools tab"));
            AddMacro(wxT("$(ObjectSuffix)"), _("Objects suffix (usually set to .o)"));
            AddMacro(wxT("$(ObjectName)"), _("The object name (without the suffix)"));
            AddMacro(wxT("$(DependSuffix)"), _("Objects suffix (usually set to .o.d)"));
            AddMacro(wxT("$(PreprocessSuffix)"), _("Objects suffix (usually set to .o.i)"));
            AddMacro(wxT("$(IncludeSwitch)"), _("The compiler include switch"));
            AddMacro(wxT("$(LibrarySwitch)"), _("The library switch (e.g. -l)"));
            AddMacro(wxT("$(OutputSwitch)"), _("The output switch (e.g. -o)"));
            AddMacro(wxT("$(LibraryPathSwitch)"), _("Library switch (e.g. -L)"));
            AddMacro(wxT("$(PreprocessorSwitch)"), _("Preprocessor switch (e.g. -D)"));
            AddMacro(wxT("$(Preprocessors)"),
                     _("Expands to all preprocessors set in the project setting where each entry "
                       "is prefixed with $(PreprocessorSwitch)"));
            AddMacro(wxT("$(ArchiveOutputSwitch)"), _("Archive switch, usually not needed (VC compiler sets it to /OUT:"));
            AddMacro(wxT("$(PreprocessOnlySwitch)"), _("The compiler preprocess-only switch (e.g. -E)"));
            AddMacro(wxT("$(LinkOptions)"), _("The linker options as set in the project settings"));
            AddMacro(wxT("$(IncludePath)"), _("All include paths prefixed with $(IncludeSwitch)"));
            AddMacro(wxT("$(RcIncludePath)"), _("Resource compiler include path as set in the project settings"));
            AddMacro(wxT("$(Libs)"), _("List of libraries to link with. Each library is prefixed with $(LibrarySwitch)"));
            AddMacro(wxT("$(LibPath)"),
                     _("List of library paths to link with. Each library is prefixed with $(LibraryPathSwitch)"));
            AddMacro(wxT("$(ProjectOutputFile)"), _("The output file"));
            AddMacro(wxT("$(OutputFile)"), _("The output file, same as $(ProjectOutputFile)"));
            break;
        }

        // resize columns
        m_listCtrlMacros->SetColumnWidth(0, wxLIST_AUTOSIZE);
        m_listCtrlMacros->SetColumnWidth(1, wxLIST_AUTOSIZE);

        // Only resize third column if we can and may expand the macros
        if(m_project && m_editor) {
            // Do not autosize this column as it may grow very, very wide
            m_listCtrlMacros->SetColumnWidth(2, 220);
        }
    }
    //-----------------------------------------------------------------
    void MacrosDlg::AddMacro(const wxString& name, const wxString& desc)
    {
        long row = AppendListCtrlRow(m_listCtrlMacros);
        SetColumnText(m_listCtrlMacros, row, 0, name);
        SetColumnText(m_listCtrlMacros, row, 1, desc);

        // Only fill third column if we can and may expand the macros
        if(m_project && m_editor && name != "$(ProjectFiles)" && name != "$(ProjectFilesAbs)") {
            wxString value = ExpandVariables(name, m_project, m_editor);
            SetColumnText(m_listCtrlMacros, row, 2, value);

        } else {
            // No third column here... don't fill it or we get an assertion
        }
    }
    //-----------------------------------------------------------------
    void MacrosDlg::OnCopy(wxCommandEvent& e)
    {
        if(m_item != wxNOT_FOUND) {
            wxString value = GetColumnText(m_listCtrlMacros, m_item, 0);
    #if wxUSE_CLIPBOARD
            if(wxTheClipboard->Open()) {
                wxTheClipboard->UsePrimarySelection(false);
                if(!wxTheClipboard->SetData(new wxTextDataObject(value))) {
                    // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
                }
                wxTheClipboard->Close();
            } else {
                wxPrintf(wxT("Failed to open the clipboard"));
            }
    #endif
        }
        m_item = wxNOT_FOUND;
    }
    //-----------------------------------------------------------------
    class MyVdTreeItemData : public wxTreeItemData
    {
        int m_itemType = ProjectItem::TypeInvalid;

    public:
        MyVdTreeItemData(int type)
            : m_itemType(type)
        {
        }
        virtual ~MyVdTreeItemData() {}

        bool IsFolder() const { return m_itemType == ProjectItem::TypeVirtualDirectory; }
        bool IsProject() const { return m_itemType == ProjectItem::TypeProject; }
    };
    //-----------------------------------------------------------------
    VirtualDirectorySelectorDlg::VirtualDirectorySelectorDlg(wxWindow* parent, CxxWS* wsp,
        const wxString& initialPath, const wxString& projectname, wxWindowID id,
        const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
        , m_workspace(wsp)
        , m_projectName(projectname)
        , m_initialPath(initialPath)
        , m_reloadTreeNeeded(false)
    {
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select Virtual Folder:"), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        bSizer1->Add(m_staticText1, 0, wxALL | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(5));

        m_treeCtrl = new DefaultTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(300, 200)),
            wxTR_DEFAULT_STYLE | wxTR_SINGLE);
        m_treeCtrl->SetFocus();

        bSizer1->Add(m_treeCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxStaticBoxSizer* sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("")), wxVERTICAL);

        bSizer1->Add(sbSizer1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticTextPreview = gsgs_Style().createStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        sbSizer1->Add(m_staticTextPreview, 0, wxEXPAND | wxALL, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer2 = new wxBoxSizer(wxHORIZONTAL);

        bSizer1->Add(bSizer2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_buttonOk = new Button(this, wxID_OK, _("OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonOk->SetDefault();

        bSizer2->Add(m_buttonOk, 0, wxALL, WXC_FROM_DIP(5));

        m_buttonCancel =
            new Button(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        bSizer2->Add(m_buttonCancel, 0, wxALL, WXC_FROM_DIP(5));

        m_button1 = new Button(this, wxID_NEW, _("New..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_button1->SetToolTip(_("Create new virtual folder..."));

        bSizer2->Add(m_button1, 0, wxALL, WXC_FROM_DIP(5));

        m_treeCtrl->SetFocus();
        DoBuildTree();

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
        // Connect events
        m_treeCtrl->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED,
            wxTreeEventHandler(VirtualDirectorySelectorDlg::OnItemSelected), NULL, this);
        m_buttonOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(VirtualDirectorySelectorDlg::OnButtonOK), NULL, this);
        m_buttonOk->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(VirtualDirectorySelectorDlg::OnButtonOkUI),
            NULL, this);
        m_buttonCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(VirtualDirectorySelectorDlg::OnButtonCancel), NULL, this);
        m_button1->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(VirtualDirectorySelectorDlg::OnNewVD), NULL, this);
        m_button1->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(VirtualDirectorySelectorDlg::OnNewVDUI), NULL,
            this);
    }
    //-----------------------------------------------------------------
    VirtualDirectorySelectorDlg::~VirtualDirectorySelectorDlg() 
    {
        m_treeCtrl->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED,
            wxTreeEventHandler(VirtualDirectorySelectorDlg::OnItemSelected), NULL, this);
        m_buttonOk->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(VirtualDirectorySelectorDlg::OnButtonOK), NULL, this);
        m_buttonOk->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(VirtualDirectorySelectorDlg::OnButtonOkUI),
            NULL, this);
        m_buttonCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(VirtualDirectorySelectorDlg::OnButtonCancel), NULL, this);
        m_button1->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(VirtualDirectorySelectorDlg::OnNewVD), NULL, this);
        m_button1->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(VirtualDirectorySelectorDlg::OnNewVDUI),
            NULL, this);
    }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::OnItemSelected(wxTreeEvent& event)
    {
        m_staticTextPreview->SetLabel(DoGetPath(m_treeCtrl, event.GetItem(), true));
    }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::OnButtonOK(wxCommandEvent&)
    {
        EndModal(wxID_OK);

        if(m_reloadTreeNeeded) {
            m_reloadTreeNeeded = false;
            wxCommandEvent buildTree(wxEVT_REBUILD_WORKSPACE_TREE);
            gsgs_Event().AddPendingEvent(buildTree);
        }
    }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::OnButtonCancel(wxCommandEvent&)
    {
        EndModal(wxID_CANCEL);

        // Even though the user cancelled,  we still need to reload if a new VDir was created
        if(m_reloadTreeNeeded) {
            m_reloadTreeNeeded = false;
            wxCommandEvent buildTree(wxEVT_REBUILD_WORKSPACE_TREE);
            gsgs_Event().AddPendingEvent(buildTree);
        }
    }
    //-----------------------------------------------------------------
    wxString VirtualDirectorySelectorDlg::DoGetPath(TreeCtrl* tree, const wxTreeItemId& item, bool validateFolder)
    {
        if(!item.IsOk()) { return wxEmptyString; }
        if(validateFolder) {
            MyVdTreeItemData* cd = dynamic_cast<MyVdTreeItemData*>(tree->GetItemData(item));
            if(!(cd && cd->IsFolder())) { return ""; }
        }

        std::deque<wxString> queue;
        wxString text = tree->GetItemText(item);
        queue.push_front(text);

        wxTreeItemId p = tree->GetItemParent(item);
        while(true) {

            if(!p.IsOk() || p == tree->GetRootItem()) break;

            FilewViewTreeItemData* data = dynamic_cast<FilewViewTreeItemData*>(tree->GetItemData(p));
            if(data && (data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder)) {
                // We reached the top level
                break;
            }

            text = tree->GetItemText(p);
            queue.push_front(text);
            p = tree->GetItemParent(p);
        }

        wxString path;
        size_t count = queue.size();
        for(size_t i = 0; i < count; i++) {
            path += queue.front();
            path += wxT(":");
            queue.pop_front();
        }

        if(!queue.empty()) {
            path += queue.front();
        } else {
            path = path.BeforeLast(wxT(':'));
        }

        return path;
    }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::DoBuildTree()
    {
        ImageManager * imagemag = &gsgs_Image();
        m_treeCtrl->DeleteAllItems();
        m_treeCtrl->SetBitmaps(&imagemag->GetBitmaps());

        int workspaceImgId = imagemag->GetMimeImageId(FileUtil::TypeWorkspace);
        int folderImgId = imagemag->GetMimeImageId(FileUtil::TypeFolder);
        int folderImgIdExpanded = imagemag->GetMimeImageId(FileUtil::TypeFolderExpanded);
        int projectImgId = imagemag->GetMimeImageId(FileUtil::TypeProject);
        int projectImgIdExpanded = imagemag->GetMimeImageId(FileUtil::TypeProjectExpanded);

        if(m_workspace) {
            wxArrayString projects;
            m_workspace->GetProjectList(projects);

            VisualWorkspaceNode nodeData;
            nodeData.name = m_workspace->GetName();
            nodeData.type = ProjectItem::TypeWorkspace;

            TreeNode<wxString, VisualWorkspaceNode>* tree =
                new TreeNode<wxString, VisualWorkspaceNode>(m_workspace->GetName(), nodeData);

            for(size_t i = 0; i < projects.GetCount(); i++) {
                if(!m_projectName.empty() && projects.Item(i) != m_projectName) {
                    // If we were passed a specific project, display only that one
                    continue;
                }

                wxString err;
                ProjectPtr p = m_workspace->FindProjectByName(projects.Item(i), err);
                if(p) { p->GetVirtualDirectories(tree); }
            }

            // create the tree
            wxTreeItemId root = m_treeCtrl->AddRoot(nodeData.name, workspaceImgId, workspaceImgId,
                                                    new MyVdTreeItemData(ProjectItem::TypeWorkspace));
            tree->GetData().itemId = root;
            TreeWalker<wxString, VisualWorkspaceNode> walker(tree);

            for(; !walker.End(); walker++) {
                // Add the item to the tree
                TreeNode<wxString, VisualWorkspaceNode>* node = walker.GetNode();

                // Skip root node
                if(node->IsRoot()) continue;

                wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
                if(parentHti.IsOk() == false) { parentHti = root; }

                int imgId; // Virtual folder
                int imgIdExpanded;
                switch(node->GetData().type) {
                case ProjectItem::TypeWorkspace:
                    imgId = 0;
                    imgIdExpanded = 0;
                    break;
                case ProjectItem::TypeProject:
                    imgId = projectImgId;
                    imgIdExpanded = projectImgIdExpanded;
                    break;
                case ProjectItem::TypeVirtualDirectory:
                default:
                    imgId = folderImgId;
                    imgIdExpanded = folderImgIdExpanded;
                    break;
                }

                // add the item to the tree
                node->GetData().itemId = m_treeCtrl->AppendItem(parentHti,            // parent
                                                                node->GetData().name, // display name
                                                                imgId,                // item image index
                                                                imgIdExpanded,        // selected item image
                                                                new MyVdTreeItemData(node->GetData().type));
            }
            if(root.IsOk() && m_treeCtrl->ItemHasChildren(root)) { m_treeCtrl->Expand(root); }
            wxDELETE(tree);
        }
        // if a initialPath was provided, try to find and select it
        if(SelectPath(m_initialPath)) { m_treeCtrl->Expand(m_treeCtrl->GetSelection()); }
    }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::OnButtonOkUI(wxUpdateUIEvent& event)
    {
        wxTreeItemId id = m_treeCtrl->GetSelection();
        if(id.IsOk()) {
            MyVdTreeItemData* cd = dynamic_cast<MyVdTreeItemData*>(m_treeCtrl->GetItemData(id));
            event.Enable(cd->IsFolder());
        }
        else
        {
            event.Enable(false);
        }
    }
    //-----------------------------------------------------------------
    wxTreeItemId VirtualDirectorySelectorDlg::FindItemForPath(const wxString& path)
    {
        if(path.empty()) { return wxTreeItemId(); }

        wxArrayString tokens = wxStringTokenize(path, wxT(":"), wxTOKEN_STRTOK);
        wxTreeItemId item = m_treeCtrl->GetRootItem();
        if(m_treeCtrl->GetWindowStyle() & wxTR_HIDE_ROOT) {
            if(!item.IsOk() || !m_treeCtrl->HasChildren(item)) { return wxTreeItemId(); }
        }
        // We need to pump-prime with the first token, otherwise the loop is never entered
        wxTreeItemIdValue cookie;
        item = m_treeCtrl->GetFirstChild(item, cookie);
        if(m_treeCtrl->GetItemText(item) == path) {
            return item; // Root's first child, the top VDir, was the item to find
        }

        for(size_t i = 1; i < tokens.GetCount(); ++i) {
            if(item.IsOk() && m_treeCtrl->HasChildren(item)) {
                // loop over the children of this node, and search for a match
                wxTreeItemIdValue cookie;
                wxTreeItemId child = m_treeCtrl->GetFirstChild(item, cookie);
                while(child.IsOk()) {
                    if(m_treeCtrl->GetItemText(child) == tokens.Item(i)) {
                        item = child;
                        if(i + 1 == tokens.GetCount()) {
                            return item; // Success!
                        }
                        break;
                    }
                    child = m_treeCtrl->GetNextChild(item, cookie);
                }
            }
        }

        return wxTreeItemId();
    }
    //-----------------------------------------------------------------
    bool VirtualDirectorySelectorDlg::SelectPath(const wxString& path)
    {
        wxTreeItemId item = FindItemForPath(path);
        if(!item.IsOk()) {
            // No match, so try to find a sensible default
            // Start with the root, but this will fail for a hidden root...
            item = m_treeCtrl->GetRootItem();
            if(m_treeCtrl->GetWindowStyle() & wxTR_HIDE_ROOT) {
                if(!item.IsOk() || !m_treeCtrl->HasChildren(item)) { return false; }

                wxTreeItemIdValue cookie;
                item = m_treeCtrl->GetFirstChild(item, cookie);
            }
        }

        if(item.IsOk()) {
            m_treeCtrl->EnsureVisible(item);
            m_treeCtrl->SelectItem(item);
            return true;
        }

        return false;
    }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::SetText(const wxString& text) { m_staticText1->SetLabel(text); }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::OnNewVD(wxCommandEvent& event)
    {
        static int counter = 0;
        wxTreeItemId id = m_treeCtrl->GetSelection();
        if(id.IsOk() == false) return;

        wxString curpath = DoGetPath(m_treeCtrl, id, false);
        wxTreeItemId item = FindItemForPath(m_projectName + ':' + m_suggestedName);
        wxString name;

        if(!item.IsOk()) { // We don't want to suggest an already-existing path!
            name = m_suggestedName;
            wxString rest;
            if(name.StartsWith(curpath.AfterFirst(':'), &rest)) {
                name = rest;
                if(!name.empty() && (name.GetChar(0) == ':')) { // Which it probably will be
                    name = name.AfterFirst(':');
                }
            }
        }

        if(name.empty()) { name << "Folder" << ++counter; }
        wxString newname = wxGetTextFromUser(_("New Virtual Folder Name:"), _("New Virtual Folder"), name);
        newname.Trim().Trim(false);

        if(newname.IsEmpty()) return;

        /*    if ( newname.Contains(":") ) { // No, don't check this: we may have been passed a multi-segment path, or be
           trying to create one
                wxMessageBox(_("':' is not a valid virtual folder character"), "gsgs");
                return;
            }*/

        curpath << ":" << newname;
        wxString errmsg;
        if(!gsgs_CppWorker().CreateVirtualDirectory(curpath, errmsg, true)) {
            wxMessageBox(_("Error occurred while creating virtual folder:\n") + errmsg, "gsgs",
                         wxOK | wxICON_WARNING | wxCENTER);
            return;
        }

        m_initialPath = curpath;
        m_reloadTreeNeeded = true;
        DoBuildTree();
    }
    //-----------------------------------------------------------------
    void VirtualDirectorySelectorDlg::OnNewVDUI(wxUpdateUIEvent& event)
    {
        wxTreeItemId id = m_treeCtrl->GetSelection();
        if(id.IsOk() == false) {
            event.Enable(false);
            return;
        }
        MyVdTreeItemData* cd = dynamic_cast<MyVdTreeItemData*>(m_treeCtrl->GetItemData(id));
        event.Enable(cd->IsProject() || cd->IsFolder()); // project or virtual folder
    }
    //-----------------------------------------------------------------
    EnvironmentVariablesDlg::EnvironmentVariablesDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
        , m_editEventsHander(m_textCtrlDefault)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        m_staticText1 = new wxStaticText(this, wxID_ANY, _("Define here set of environment variables which will be applied by gsgs before launching processes.\nVariables are defined in the format of NAME=VALUE"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_staticText1->SetBackgroundColour(gsgs_Style().GetActiveMarkerColour());

        bSizer1->Add(m_staticText1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Available environment sets:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        bSizer1->Add(m_staticText2, 0, wxALL, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer6 = new wxBoxSizer(wxHORIZONTAL);

        bSizer1->Add(bSizer6, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);

        bSizer6->Add(bSizer2, 1, wxEXPAND, WXC_FROM_DIP(5));

        //m_book = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBK_DEFAULT);
        m_book = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);

        bSizer2->Add(m_book, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_panel1 = new wxPanel(m_book, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_book, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_book->AddPage(m_panel1, _("Default"), false);

        wxBoxSizer* bSizer5 = new wxBoxSizer(wxVERTICAL);
        m_panel1->SetSizer(bSizer5);

        m_textCtrlDefault = new wxStyledTextCtrl(m_panel1, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        // Configure the fold margin
        m_textCtrlDefault->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
        m_textCtrlDefault->SetMarginMask(4, wxSTC_MASK_FOLDERS);
        m_textCtrlDefault->SetMarginSensitive(4, true);
        m_textCtrlDefault->SetMarginWidth(4, 0);

        // Configure the tracker margin
        m_textCtrlDefault->SetMarginWidth(1, 0);

        // Configure the symbol margin
        m_textCtrlDefault->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_textCtrlDefault->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
        m_textCtrlDefault->SetMarginWidth(2, 0);
        m_textCtrlDefault->SetMarginSensitive(2, true);

        // Configure the line numbers margin
        m_textCtrlDefault->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_textCtrlDefault->SetMarginWidth(0, 0);

        // Configure the line symbol margin
        m_textCtrlDefault->SetMarginType(3, wxSTC_MARGIN_FORE);
        m_textCtrlDefault->SetMarginMask(3, 0);
        m_textCtrlDefault->SetMarginWidth(3, 0);
        // Select the lexer
        m_textCtrlDefault->SetLexer(wxSTC_LEX_NULL);
        // Set default font / styles
        m_textCtrlDefault->StyleClearAll();
        m_textCtrlDefault->SetWrapMode(0);
        m_textCtrlDefault->SetIndentationGuides(0);
        m_textCtrlDefault->SetKeyWords(0, wxT(""));
        m_textCtrlDefault->SetKeyWords(1, wxT(""));
        m_textCtrlDefault->SetKeyWords(2, wxT(""));
        m_textCtrlDefault->SetKeyWords(3, wxT(""));
        m_textCtrlDefault->SetKeyWords(4, wxT(""));

        bSizer5->Add(m_textCtrlDefault, 1, wxEXPAND, 0);

        wxBoxSizer* bSizer7 = new wxBoxSizer(wxVERTICAL);

        bSizer6->Add(bSizer7, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_buttonNewSet = new Button(this, wxID_ANY, _("&New Set..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        bSizer7->Add(m_buttonNewSet, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_buttonDeleteSet = new Button(this, wxID_ANY, _("Delete Set"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        bSizer7->Add(m_buttonDeleteSet, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_button5 = new Button(this, wxID_ANY, _("&Export"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_button5->SetToolTip(_("Export the current set to a platform \nspecific environment file"));

        bSizer7->Add(m_button5, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer12 = new wxBoxSizer(wxHORIZONTAL);

        bSizer1->Add(boxSizer12, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

        m_button14 = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_button14->SetDefault();

        boxSizer12->Add(m_button14, 0, wxALL, WXC_FROM_DIP(5));

        m_button16 = new Button(this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        boxSizer12->Add(m_button16, 0, wxALL, WXC_FROM_DIP(5));

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(EnvironmentVariablesDlg::OnClose), NULL, this);
        m_buttonNewSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnNewSet), NULL, this);
        m_buttonDeleteSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnDeleteSet), NULL, this);
        m_buttonDeleteSet->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EnvironmentVariablesDlg::OnDeleteSetUI), NULL, this);
        m_button5->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnExport), NULL, this);
        m_button16->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnCancel), NULL, this);
        m_button14->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnButtonOk), NULL, this);

        EvnVarList vars;
        gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);
        gsgsStringMap envSets = vars.GetEnvVarSets();
        wxString activePage = vars.GetActiveSet();

        wxStyledTextCtrl* sci = m_textCtrlDefault;
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        if(lexer) {
            lexer->Apply(sci);
        }

        SetName("EnvVarsTableDlg");
        gsgsStringMap::iterator iter = envSets.begin();
        for(; iter != envSets.end(); iter++) {
            wxString name = iter->first;
            wxString content = iter->second;

            if(name == wxT("Default")) {
                m_textCtrlDefault->SetText(content);
                m_textCtrlDefault->SetSavePoint();
                m_textCtrlDefault->EmptyUndoBuffer();
            } else {
                DoAddPage(name, content, false);
            }
        }

        m_book->SetSelection(0);
        for(size_t i = 0; i < m_book->GetPageCount(); i++) {
            if(m_book->GetPageText(i) == activePage) {
                m_book->GetPage(i)->SetFocus();
                m_book->SetSelection(i);
            }
            wxWindow* page = m_book->GetPage(i);
            wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(page);
            if(ctrl) {
                ctrl->SetSavePoint();
                ctrl->EmptyUndoBuffer();
            }
        }
    }
    //-----------------------------------------------------------------
    EnvironmentVariablesDlg::~EnvironmentVariablesDlg() 
    {
        this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(EnvironmentVariablesDlg::OnClose), NULL, this);
        m_buttonNewSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnNewSet), NULL, this);
        m_buttonDeleteSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnDeleteSet), NULL, this);
        m_buttonDeleteSet->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EnvironmentVariablesDlg::OnDeleteSetUI), NULL, this);
        m_button5->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnExport), NULL, this);
        m_button16->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnCancel), NULL, this);
        m_button14->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EnvironmentVariablesDlg::OnButtonOk), NULL, this);
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::DoAddPage(const wxString& name, const wxString& content, bool select)
    {
        wxStyledTextCtrl* page = new wxStyledTextCtrl(m_book, wxID_ANY, wxDefaultPosition, wxSize(0, 0));
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        if(lexer) {
            lexer->Apply(page);
        }

        page->SetText(content);
        m_book->AddPage(page, name, select);
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::OnButtonOk(wxCommandEvent& event)
    {
        EvnVarList vars;

        gsgsStringMap envSets;

        wxString content = m_textCtrlDefault->GetText();
        wxString name = wxT("Default");
        content.Trim().Trim(false);
        envSets[name] = content;

        for(size_t i = 1; i < m_book->GetPageCount(); i++) {
            if(i == (size_t)m_book->GetSelection()) {
                vars.SetActiveSet(m_book->GetPageText(i));
            }

            wxStyledTextCtrl* page = (wxStyledTextCtrl*)m_book->GetPage(i);
            wxString content = page->GetText();
            wxString name = m_book->GetPageText(i);
            content.Trim().Trim(false);
            envSets[name] = content;
        }
        vars.SetEnvVarSets(envSets);
        gsgs_EditorConfig().WriteObject(wxT("Variables"), &vars);

        // Notify that the environment variables were modified
        Event eventSave(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED);
        gsgs_Event().AddPendingEvent(eventSave);
        event.Skip();
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::OnDeleteSet(wxCommandEvent& )
    {
        int selection = m_book->GetSelection();
        if(selection == wxNOT_FOUND) return;

        wxString name = m_book->GetPageText((size_t)selection);
        if(wxMessageBox(wxString::Format(wxT("Delete environment variables set\n'%s' ?"), name.c_str()), wxT("Confirm"),
                        wxYES_NO | wxICON_QUESTION, this) != wxYES)
            return;
        m_book->DeletePage((size_t)selection);
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::OnDeleteSetUI(wxUpdateUIEvent& event)
    {
        int i = m_book->GetSelection();
        event.Enable(i != wxNOT_FOUND && m_book->GetPageText(i) != wxT("Default"));
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::OnExport(wxCommandEvent& event)
    {
        int selection = m_book->GetSelection();
        if(selection == wxNOT_FOUND) return;

    #ifdef __WXMSW__
        bool isWindows = true;
    #else
        bool isWindows = false;
    #endif

        wxString text;
        if(selection == 0) {
            text = m_textCtrlDefault->GetText();
        } else {
            wxStyledTextCtrl* page = dynamic_cast<wxStyledTextCtrl*>(m_book->GetPage((size_t)selection));
            if(page) {
                text = page->GetText();
            }
        }

        if(text.IsEmpty()) return;

        wxArrayString lines = wxStringTokenize(text, wxT("\r\n"), wxTOKEN_STRTOK);
        wxString envfile;
        if(isWindows) {
            envfile << wxT("environment.bat");
        } else {
            envfile << wxT("environment");
        }

        wxFileName fn(wxGetCwd(), envfile);
        wxFFile fp(fn.GetFullPath(), wxT("w+b"));
        if(fp.IsOpened() == false) {
            wxMessageBox(wxString::Format(_("Failed to open file: '%s' for write"), fn.GetFullPath().c_str()),
                         wxT("gsgs"), wxOK | wxCENTER | wxICON_WARNING, this);
            return;
        }

        for(size_t i = 0; i < lines.GetCount(); i++) {

            wxString sLine = lines.Item(i).Trim().Trim(false);
            if(sLine.IsEmpty()) continue;

            static wxRegEx reVarPattern(wxT("\\$\\(( *)([a-zA-Z0-9_]+)( *)\\)"));
            if(isWindows) {
                while(reVarPattern.Matches(sLine)) {
                    wxString varName = reVarPattern.GetMatch(sLine, 2);
                    wxString text = reVarPattern.GetMatch(sLine);
                    sLine.Replace(text, wxString::Format(wxT("%%%s%%"), varName.c_str()));
                }
                sLine.Prepend(wxT("set "));
                sLine.Append(wxT("\r\n"));

            } else {
                while(reVarPattern.Matches(sLine)) {
                    wxString varName = reVarPattern.GetMatch(sLine, 2);
                    wxString text = reVarPattern.GetMatch(sLine);
                    sLine.Replace(text, wxString::Format(wxT("$%s"), varName.c_str()));
                }
                sLine.Prepend(wxT("export "));
                sLine.Append(wxT("\n"));
            }
            fp.Write(sLine);
        }

        wxMessageBox(wxString::Format(_("Environment exported to: '%s' successfully"), fn.GetFullPath()), wxT("gsgs"),
                     wxOK | wxCENTRE, this);
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::OnNewSet(wxCommandEvent& event) 
    { 
        CallAfter(&EnvironmentVariablesDlg::DoAddNewSet); 
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::OnClose(wxCloseEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::OnCancel(wxCommandEvent& event)
    {
        event.Skip();
        EndModal(wxID_CANCEL);
    }
    //-----------------------------------------------------------------
    void EnvironmentVariablesDlg::DoAddNewSet()
    {
        wxTextEntryDialog dlg(this, _("Name:"), wxT("Create a new set"), "My New Set");
        if(dlg.ShowModal() == wxID_OK) {
            wxString name = dlg.GetValue();
            if(name.IsEmpty()) return;
            DoAddPage(name, wxT(""), false);
        }
    }
    //-----------------------------------------------------------------
    enum class eAnagramFlag {
        kNone = 0,
        kIgnoreWhitespace = (1 << 0),
    };

    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // gsgsAnagram
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    class gsgsAnagram
    {
    public:
        gsgsAnagram()
            : m_flags(0)
        {
        }
        gsgsAnagram(const wxString& needle, size_t flags = 0);
        virtual ~gsgsAnagram();

        /**
         * @brief reset the anagram with a new pattern
         * See eAnagramFlag for possible flags
         */
        void Reset(const wxString& needle, size_t flags = 0);

        /**
         * @brief check haystack contains this anagram
         */
        bool Matches(const wxString& haystack) const;

        /**
         * @brief search for needle in haystack in order
         */
        bool MatchesInOrder(const wxString& haystack) const;

        /**
         * @brief return true if the needle is an empty string
         */
        inline bool IsEmpty() const { return m_needle.IsEmpty();  }
    protected:
        bool HasFlag(eAnagramFlag flag) const { return (m_flags & (size_t)flag); }
    private:
        std::unordered_map<wxChar, int> m_charCounts;
        wxString m_needle;
        size_t m_flags;
    };

    #define IS_WHITESPACE(ch) (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
    //--------------------------------------------------------------------------------
    gsgsAnagram::gsgsAnagram(const wxString& needle, size_t flags)
        : m_flags(flags)
    {
        Reset(needle, m_flags);
    }
    //--------------------------------------------------------------------------------
    gsgsAnagram::~gsgsAnagram() {}
    //--------------------------------------------------------------------------------
    bool gsgsAnagram::Matches(const wxString& haystack) const
    {
        std::unordered_map<wxChar, int> charsCount = m_charCounts;
        for(size_t i = 0; i < haystack.size(); ++i) {
            wxChar ch = haystack[i];
            ch = wxTolower(ch);
            if(charsCount.count(ch)) {
                int& counter = charsCount[ch];
                counter--;
                if(counter == 0) { charsCount.erase(ch); }
                if(charsCount.empty()) { return true; }
            }
        }
        return false;
    }
    //--------------------------------------------------------------------------------
    bool gsgsAnagram::MatchesInOrder(const wxString& haystack) const
    {
        if(m_needle.IsEmpty()) { return true; }
        size_t index = 0;
        size_t maxIndex = m_needle.size();
        for(size_t i = 0; i < haystack.size(); ++i) {
            wxChar ch = haystack[i];
            ch = wxTolower(ch);
            if(ch == m_needle[index]) { ++index; }
            if(maxIndex == index) { return true; }
        }
        return false;
    }
    //--------------------------------------------------------------------------------
    void gsgsAnagram::Reset(const wxString& needle, size_t flags)
    {
        m_flags = flags;
        m_needle.Clear();
        if(HasFlag(eAnagramFlag::kIgnoreWhitespace)) {
            for(size_t i = 0; i < needle.size(); ++i) {
                wxChar ch = needle[i];
                if(IS_WHITESPACE(ch)) { continue; }
                m_needle.Append(wxTolower(ch));
            }
        } else {
            m_needle = needle.Lower();
        }

        m_charCounts.clear();
        std::for_each(m_needle.begin(), m_needle.end(), [&](wxChar ch) {
            if(m_charCounts.count(ch) == 0) {
                m_charCounts[ch] = 1;
            } else {
                m_charCounts[ch]++;
            }
        });
    }
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // GotoAnythingDlg
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    GotoAnythingDlg::GotoAnythingDlg(wxWindow* parent, const std::vector<GotoEntry>& entries, wxWindowID id,
        const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
        , m_allEntries(entries)
    {
        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer2);

        m_textCtrlSearch = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_PROCESS_ENTER);
#if wxVERSION_NUMBER >= 3000
        m_textCtrlSearch->SetHint(_("Search..."));
#endif

        boxSizer2->Add(m_textCtrlSearch, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(300, 100)), wxDV_ROW_LINES | wxDV_SINGLE);

        boxSizer2->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrl->AppendIconTextColumn(_("Matches"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(400), wxALIGN_LEFT);
        m_dvListCtrl->AppendTextColumn(_("Shortcut"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT);

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        this->Connect(wxEVT_IDLE, wxIdleEventHandler(GotoAnythingDlg::OnIdle), NULL, this);
        m_textCtrlSearch->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(GotoAnythingDlg::OnKeyDown), NULL, this);
        m_textCtrlSearch->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(GotoAnythingDlg::OnEnter), NULL, this);
        m_dvListCtrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(GotoAnythingDlg::OnKeyDown), NULL, this);
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(GotoAnythingDlg::OnItemActivated), NULL, this);

        DoPopulate(m_allEntries);
        CallAfter(&GotoAnythingDlg::UpdateLastSearch);
    }
    //--------------------------------------------------------------------------------
    GotoAnythingDlg::~GotoAnythingDlg()
    {
        this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(GotoAnythingDlg::OnIdle), NULL, this);
        m_textCtrlSearch->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(GotoAnythingDlg::OnKeyDown), NULL, this);
        m_textCtrlSearch->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(GotoAnythingDlg::OnEnter), NULL, this);
        m_dvListCtrl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(GotoAnythingDlg::OnKeyDown), NULL, this);
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(GotoAnythingDlg::OnItemActivated), NULL, this);
        // gsgs_Config().Write("GotoAnything/LastSearch", m_textCtrlSearch->GetValue());
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::OnKeyDown(wxKeyEvent& event)
    {
        event.Skip();
        if(event.GetKeyCode() == WXK_ESCAPE) {
            event.Skip(false);
            EndModal(wxID_CANCEL);
        } else if(event.GetKeyCode() == WXK_DOWN) {
            event.Skip(false);
            int row = m_dvListCtrl->GetSelectedRow();
            if((row + 1) < m_dvListCtrl->GetItemCount()) {
                row++;
                DoSelectItem(m_dvListCtrl->RowToItem(row));
            }
        } else if(event.GetKeyCode() == WXK_UP) {
            event.Skip(false);
            int row = m_dvListCtrl->GetSelectedRow();
            if((row - 1) >= 0) {
                row--;
                DoSelectItem(m_dvListCtrl->RowToItem(row));
            }
        }
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::OnEnter(wxCommandEvent&)
    {
        DoExecuteActionAndClose();
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::DoPopulate(const std::vector<GotoEntry>& entries, const std::vector<int>& indexes)
    {
        m_dvListCtrl->DeleteAllItems();
        static wxBitmap placeHolderBmp = gsgs_Image().LoadBitmap("placeholder");
        for(size_t i = 0; i < entries.size(); ++i) {
            const GotoEntry& entry = entries[i];
            wxVector<wxVariant> cols;
            cols.push_back(::MakeIconText(entry.GetDesc(), entry.GetBitmap().IsOk() ? entry.GetBitmap() : placeHolderBmp));
            cols.push_back(entry.GetKeyboardShortcut());
            m_dvListCtrl->AppendItem(cols, indexes.empty() ? i : indexes[i]);
        }
        if(!entries.empty()) { m_dvListCtrl->SelectRow(0); }
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::DoExecuteActionAndClose()
    {
        int row = m_dvListCtrl->GetSelectedRow();
        if(row == wxNOT_FOUND) return;

        // Execute the action
        int index = m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(row));
        const GotoEntry& entry = m_allEntries[index];
        gsgsDEBUG() << "GotoAnythingDlg: action selected:" << entry.GetDesc() << clEndl;

        GotoEvent evtAction(wxEVT_GOTO_ANYTHING_SELECTED);
        evtAction.SetEntry(entry);
        gsgs_Event().AddPendingEvent(evtAction);
        EndModal(wxID_OK);
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::OnIdle(wxIdleEvent& e)
    {
        e.Skip();
        ApplyFilter();
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::UpdateLastSearch() 
    {
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::ApplyFilter()
    {
        // Create a list the matches the typed text
        wxString filter = m_textCtrlSearch->GetValue();
        if(m_currentFilter == filter) return;

        // Update the last applied filter
        m_currentFilter = filter;
        if(filter.IsEmpty()) {
            DoPopulate(m_allEntries);
        } else {

            // Filter the list
            gsgsAnagram anagram(filter);
            std::vector<GotoEntry> matchedEntries;
            std::vector<int> matchedEntriesIndex;
            for(size_t i = 0; i < m_allEntries.size(); ++i) {
                const GotoEntry& entry = m_allEntries[i];
                if(anagram.MatchesInOrder(entry.GetDesc())) {
                    matchedEntries.push_back(entry);
                    matchedEntriesIndex.push_back(i);
                }
            }

            // And populate the list
            DoPopulate(matchedEntries, matchedEntriesIndex);
        }
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::OnItemActivated(wxDataViewEvent& )
    {
        DoExecuteActionAndClose();
    }
    //--------------------------------------------------------------------------------
    void GotoAnythingDlg::DoSelectItem(const wxDataViewItem& item)
    {
        CHECK_ITEM_RET(item);
        m_dvListCtrl->UnselectAll();
        m_dvListCtrl->Select(item);
        m_dvListCtrl->EnsureVisible(item);
    }
    //--------------------------------------------------------------------------------
}

