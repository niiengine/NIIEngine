#include "gsgsStyleManager.h"
#include "TailPanel.h"
#include "gsgsImageManager.h"
#include "gsgsConfig.h"
#include "gsgsEvent.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "gsgsLexerStyle.h"
#include "tail.h"
#include <imanager.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>

TailPanel::TailPanel(wxWindow* parent, Tail* plugin)
    : TailPanelBase(parent)
    , m_lastPos(0)
    , m_plugin(plugin)
    , m_isDetached(false)
    , m_frame(NULL)
{
    gsgs_Style().RegisterWindow(this);
    gsgs_Style().RegisterWindow(m_staticTextFileName);
    
    DoBuildToolbar();
    m_fileWatcher.reset(new FileWatch());
    m_fileWatcher->SetOwner(this);
    Bind(wxEVT_FILE_MODIFIED, &TailPanel::OnFileModified, this);

    wxCommandEvent dummy;
    OnTheme(dummy);
    gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &TailPanel::OnTheme, this);
}

TailPanel::~TailPanel()
{
    gsgs_Style().UnRegisterWindow(this);
    gsgs_Style().UnRegisterWindow(m_staticTextFileName);
    Unbind(wxEVT_FILE_MODIFIED, &TailPanel::OnFileModified, this);
    gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &TailPanel::OnTheme, this);
}

void TailPanel::OnPause(wxCommandEvent& event) { m_fileWatcher->Stop(); }

void TailPanel::OnPauseUI(wxUpdateUIEvent& event) { event.Enable(m_file.IsOk() && m_fileWatcher->IsRunning()); }

void TailPanel::OnPlay(wxCommandEvent& event) { m_fileWatcher->Start(); }

void TailPanel::OnPlayUI(wxUpdateUIEvent& event) { event.Enable(m_file.IsOk() && !m_fileWatcher->IsRunning()); }

void TailPanel::DoClear()
{
    m_fileWatcher->Stop();
    m_fileWatcher->Clear();

    m_file.Clear();
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
    m_lastPos = 0;

    m_staticTextFileName->SetLabel(_("<No opened file>"));
    SetFrameTitle();
    Layout();
}

void TailPanel::OnFileModified(gsgs::FileEvent& event)
{
    wxFileName fn(event.GetPath());
    // Get the current file size
    size_t cursize = FileUtil::GetFileSize(m_file);
    wxFFile fp(m_file.GetFullPath(), "rb");
    if(fp.IsOpened() && fp.Seek(m_lastPos)) {
        if(cursize > m_lastPos) {
            size_t bufferSize = cursize - m_lastPos;
            char* buffer = new char[bufferSize + 1];
            if(fp.Read(buffer, bufferSize) == bufferSize) {
                buffer[bufferSize] = '\0';
                wxString content((const char*)buffer, wxConvUTF8);
                DoAppendText(content);
            }
            wxDELETEA(buffer);
        } else {
            DoAppendText(_("\n>>> File truncated <<<\n"));
        }
        m_lastPos = cursize;
    }
}

void TailPanel::DoAppendText(const wxString& text)
{
    m_stc->SetReadOnly(false);
    m_stc->AppendText(text);
    m_stc->SetReadOnly(true);
    m_stc->SetSelectionEnd(m_stc->GetLength());
    m_stc->SetSelectionStart(m_stc->GetLength());
    m_stc->SetCurrentPos(m_stc->GetLength());
    m_stc->EnsureCaretVisible();
}

void TailPanel::OnTheme(wxCommandEvent& event)
{
    event.Skip(); // must call this to allow other handlers to work
    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
    if(lexer) { lexer->Apply(m_stc); }
    m_stc->SetEOLMode(wxSTC_EOL_CRLF);
    m_stc->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
}

void TailPanel::OnClear(wxCommandEvent& event)
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}

void TailPanel::OnClearUI(wxUpdateUIEvent& event) { event.Enable(!m_stc->IsEmpty()); }

void TailPanel::OnClose(wxCommandEvent& event) { DoClear(); }

void TailPanel::OnCloseUI(wxUpdateUIEvent& event) { event.Enable(m_file.IsOk()); }

void TailPanel::OnOpen(wxCommandEvent& event)
{
    wxString filepath = ::wxFileSelector();
    if(filepath.IsEmpty() || !wxFileName::Exists(filepath)) { return; }

    DoClear();
    DoOpen(filepath);
}

void TailPanel::OnOpenMenu(wxCommandEvent& event)
{
    wxMenu menu;
    DoPrepareRecentItemsMenu(menu);
    m_toolbar->ShowMenuForButton(XRCID("tail_open"), &menu);
}

void TailPanel::DoOpen(const wxString& filename)
{
    m_file = filename;
    m_lastPos = FileUtil::GetFileSize(m_file);

    wxArrayString recentItems;
    gsgs_Config().Read("tail", recentItems, wxArrayString());
    if(recentItems.Index(m_file.GetFullPath()) == wxNOT_FOUND) {
        // add it
        recentItems.Add(m_file.GetFullPath());
        recentItems.Sort();
        gsgs_Config().Write("tail", recentItems);
    }

    // Stop the current watcher
    m_fileWatcher->SetFile(m_file);
    m_fileWatcher->Start();
    m_staticTextFileName->SetLabel(m_file.GetFullPath());
    SetFrameTitle();

    Layout();
}

void TailPanel::DoPrepareRecentItemsMenu(wxMenu& menu)
{
    m_recentItemsMap.clear();
    wxArrayString recentItems;
    gsgs_Config().Read("tail", recentItems, wxArrayString());
    for(size_t i = 0; i < recentItems.size(); ++i) {
        int id = ::wxNewId();
        m_recentItemsMap.insert(std::make_pair(id, recentItems.Item(i)));
        menu.Append(id, recentItems.Item(i));
    }

    menu.Bind(wxEVT_MENU, &TailPanel::OnOpenRecentItem, this);
}

void TailPanel::OnOpenRecentItem(wxCommandEvent& event)
{
    if(m_recentItemsMap.count(event.GetId()) == 0) return;
    wxString filepath = m_recentItemsMap[event.GetId()];
    DoClear(); // Clear the old content first
    DoOpen(filepath);
    m_recentItemsMap.clear();
}

void TailPanel::OnDetachWindow(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_plugin->CallAfter(&Tail::DetachTailWindow, m_plugin->GetView()->GetTailData());
    m_isDetached = true;
}

void TailPanel::OnDetachWindowUI(wxUpdateUIEvent& event) { event.Enable(!m_isDetached); }

void TailPanel::Initialize(const TailData& tailData)
{
    DoClear();
    if(tailData.filename.IsOk() && tailData.filename.Exists()) {
        DoOpen(tailData.filename.GetFullPath());
        DoAppendText(tailData.displayedText);
        m_lastPos = tailData.lastPos;
        SetFrameTitle();
    }
}

TailData TailPanel::GetTailData() const
{
    TailData dt;
    dt.displayedText = m_stc->GetText();
    dt.filename = m_file;
    dt.lastPos = m_lastPos;
    return dt;
}

wxString TailPanel::GetTailTitle() const
{
    wxString title;
    if(IsDetached()) {
        if(IsOpen()) {
            title << m_file.GetFullName() << " (" << m_file.GetFullPath() << ")";
        } else {
            title = "Tail";
        }
    }
    return title;
}

void TailPanel::SetFrameTitle()
{
    wxFrame* parent = dynamic_cast<wxFrame*>(GetParent());
    if(parent) { parent->SetLabel(GetTailTitle()); }
}

void TailPanel::DoBuildToolbar()
{
    m_toolbar = new gsgs::ToolBar(this);
    m_toolbar->AddTool(XRCID("tail_open"), _("Open file"), gsgs_Image().LoadBitmap("folder"), "",
                       wxITEM_DROPDOWN);
    m_toolbar->AddTool(XRCID("tail_close"), _("Close file"), gsgs_Image().LoadBitmap("file_close"));
    m_toolbar->AddTool(XRCID("tail_clear"), _("Clear"), gsgs_Image().LoadBitmap("clear"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("tail_pause"), _("Pause"), gsgs_Image().LoadBitmap("interrupt"));
    m_toolbar->AddTool(XRCID("tail_play"), _("Play"), gsgs_Image().LoadBitmap("debugger_start"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("tail_detach"), _("Detach window"), gsgs_Image().LoadBitmap("windows"));

    // Bind events
    m_toolbar->Bind(wxEVT_TOOL, &TailPanel::OnOpen, this, XRCID("tail_open"));
    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &TailPanel::OnOpenMenu, this, XRCID("tail_open"));
    m_toolbar->Bind(wxEVT_TOOL, &TailPanel::OnClose, this, XRCID("tail_close"));
    m_toolbar->Bind(wxEVT_TOOL, &TailPanel::OnClear, this, XRCID("tail_clear"));
    m_toolbar->Bind(wxEVT_TOOL, &TailPanel::OnPause, this, XRCID("tail_pause"));
    m_toolbar->Bind(wxEVT_TOOL, &TailPanel::OnPlay, this, XRCID("tail_play"));
    m_toolbar->Bind(wxEVT_TOOL, &TailPanel::OnDetachWindow, this, XRCID("tail_detach"));

    m_toolbar->Bind(wxEVT_UPDATE_UI, &TailPanel::OnCloseUI, this, XRCID("tail_close"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &TailPanel::OnClearUI, this, XRCID("tail_clear"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &TailPanel::OnPauseUI, this, XRCID("tail_pause"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &TailPanel::OnPlayUI, this, XRCID("tail_play"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &TailPanel::OnDetachWindowUI, this, XRCID("tail_detach"));
    m_toolbar->Realize();

    GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);
}
