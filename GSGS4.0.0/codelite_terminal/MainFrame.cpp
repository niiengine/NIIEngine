#include "MainFrame.h"
#include <wx/aboutdlg.h>
#include "wxTerminalCtrl.h"
#include "SettingsDlg.h"
#include "wxTerminalOptions.h"

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
    wxTerminalOptions& options = wxTerminalOptions::Get();
    options.Load();
    m_terminal = new wxTerminalCtrl(GetMainPanel());
    m_terminal->SetWorkingDirectory(options.GetWorkingDirectory());
    m_terminal->SetPauseOnExit(options.IsWaitOnExit());
    m_terminal->SetPrintTTY(options.IsPrintTTY(), options.GetTtyfile());
    m_terminal->SetLogfile(options.GetLogfile());
    m_terminal->Start(options.GetCommand());

    GetMainPanel()->GetSizer()->Add(m_terminal, 1, wxEXPAND);
    GetSizer()->Fit(this);
    SetLabel(options.GetTitle().IsEmpty() ? "gsgs terminal" : options.GetTitle());
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_DONE, &MainFrame::OnTerminalExit, this);
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_SET_TITLE, &MainFrame::OnSetTitle, this);
    m_terminal->CallAfter(&wxTerminalCtrl::Focus);
    PostSizeEvent();
}

MainFrame::~MainFrame() {}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Hide();
    CallAfter(&MainFrame::DoClose);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetName("gsgs-terminal");
    info.AddDeveloper("NIIEngine Team Group");
    info.SetLicence(_("Commercial authorization"));
    info.SetDescription(_("gsgs terminal"));
    ::wxAboutBox(info);
}

void MainFrame::OnTerminalExit(gsgs::Event& event)
{
    Hide();
    CallAfter(&MainFrame::DoClose);
}

void MainFrame::DoClose()
{
    const wxTerminalHistory& history = m_terminal->GetHistory();
    wxTerminalOptions::Get().SetHistory(history.GetCommands());
    wxTerminalOptions::Get().Save();
    wxTheApp->ExitMainLoop();
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    SettingsDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        dlg.Save();
        m_terminal->ReloadSettings();
        m_terminal->ClearScreen();
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    Hide();
    CallAfter(&MainFrame::DoClose);
}

void MainFrame::OnSetTitle(gsgs::Event& event) 
{ 
    SetLabel(event.GetString()); 
}

void MainFrame::OnClearScreen(wxCommandEvent& event) 
{ 
    m_terminal->ClearScreen(); 
}
