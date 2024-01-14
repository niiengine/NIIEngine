
#include "gsgsStyleManager.h"
#include "gsgsImageManager.h"
#include "gsgsToolBar.h"
#include "gsgsConfig.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "gsgsLexerStyle.h"
#include "outline_symbol_tree.h"
#include "outline_tab.h"
#include "plugin.h"
#include <imanager.h>
#include <wx/menu.h>
#include <wx/stc/stc.h>
#include <wx/wupdlock.h>

const wxEventType wxEVT_SV_GOTO_DEFINITION = wxNewEventType();
const wxEventType wxEVT_SV_GOTO_DECLARATION = wxNewEventType();
const wxEventType wxEVT_SV_FIND_REFERENCES = wxNewEventType();
const wxEventType wxEVT_SV_RENAME_SYMBOL = wxNewEventType();
const wxEventType wxEVT_SV_OPEN_FILE = wxNewEventType();

#define OUTLINE_TAB_CXX 0
#define OUTLINE_TAB_PHP 1
#define OUTLINE_PLACE_HOLDER_PAGE 2

#define CHECK_ENABLED()                                      \
    if(!m_isEnabled) {                                       \
        gsgsDEBUG1() << "Outline: view is disabled" << clEndl; \
        return;                                              \
    }

OutlineTab::OutlineTab(wxWindow* parent, IManager* mgr)
    : OutlineTabBaseClass(parent)
    , m_mgr(mgr)
    , m_isEnabled(false)
{
    m_tree = new svSymbolTree(m_panelCxx, m_mgr, wxID_ANY);
    gsgs_Config().Read("OutlineView/SortCxxAlphabetically", m_sortCxxTreeAlphabetically, false);
    m_tree->SetSortByLineNumber(!m_sortCxxTreeAlphabetically);

    m_panelCxx->GetSizer()->Add(m_tree, 1, wxEXPAND);
    m_tree->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OutlineTab::OnMenu), NULL, this);

    m_tree->SetBitmaps(&gsgs_Image().GetBitmaps());
    m_treeCtrlPhp->SetManager(m_mgr);

    gsgs_Event().Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OutlineTab::OnActiveEditorChanged),
                                  NULL, this);
    gsgs_Event().Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OutlineTab::OnEditorClosed), NULL, this);
    gsgs_Event().Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OutlineTab::OnAllEditorsClosed), NULL,
                                  this);
    gsgs_Event().Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(OutlineTab::OnWorkspaceClosed), NULL,
                                  this);
    gsgs_Event().Connect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(OutlineTab::OnFilesTagged), NULL,
                                  this);
    gsgs_Event().Bind(wxEVT_FILE_SAVED, &OutlineTab::OnEditorSaved, this);
    gsgs_Event().Bind(wxEVT_CMD_PAGE_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    Connect(wxEVT_SV_GOTO_DEFINITION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
            this);
    Connect(wxEVT_SV_GOTO_DECLARATION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
            this);
    Connect(wxEVT_SV_FIND_REFERENCES, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
            this);
    Connect(wxEVT_SV_RENAME_SYMBOL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    m_themeHelper = new ThemeHandlerHelper(this);
    m_toolbar = new gsgs::ToolBar(m_panelCxx);
    m_toolbar->AddTool(wxID_SORT_ASCENDING, _("Sort"), gsgs_Image().LoadBitmap("sort"), "",
                       wxITEM_CHECK);
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &OutlineTab::OnSortAlpha, this, wxID_SORT_ASCENDING);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &OutlineTab::OnSortAlphaUI, this, wxID_SORT_ASCENDING);
    m_panelCxx->GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);
}

OutlineTab::~OutlineTab()
{
    wxDELETE(m_themeHelper);
    m_toolbar->Unbind(wxEVT_TOOL, &OutlineTab::OnSortAlpha, this, wxID_SORT_ASCENDING);
    m_toolbar->Unbind(wxEVT_UPDATE_UI, &OutlineTab::OnSortAlphaUI, this, wxID_SORT_ASCENDING);
    m_tree->Disconnect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OutlineTab::OnMenu), NULL, this);

    gsgs_Event().Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(OutlineTab::OnActiveEditorChanged), NULL, this);
    gsgs_Event().Unbind(wxEVT_CMD_PAGE_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    gsgs_Event().Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OutlineTab::OnEditorClosed), NULL,
                                     this);
    gsgs_Event().Disconnect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OutlineTab::OnAllEditorsClosed),
                                     NULL, this);
    gsgs_Event().Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(OutlineTab::OnWorkspaceClosed), NULL,
                                     this);
    gsgs_Event().Disconnect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(OutlineTab::OnFilesTagged), NULL,
                                     this);
    gsgs_Event().Unbind(wxEVT_FILE_SAVED, &OutlineTab::OnEditorSaved, this);
    Disconnect(wxEVT_SV_GOTO_DEFINITION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
    Disconnect(wxEVT_SV_GOTO_DECLARATION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
    Disconnect(wxEVT_SV_FIND_REFERENCES, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
    Disconnect(wxEVT_SV_RENAME_SYMBOL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
}

void OutlineTab::OnSearchSymbol(wxCommandEvent& event)
{
    event.Skip();
    if(m_simpleBook->GetSelection() == OUTLINE_TAB_PHP) {
        // PHP
        m_treeCtrlPhp->Select(m_textCtrlSearch->GetValue());

    } else {
        // C++
        wxString name = m_textCtrlSearch->GetValue();
        name.Trim().Trim(false);
        m_tree->SelectItemByName(name);
    }
}

void OutlineTab::OnSearchEnter(wxCommandEvent& event)
{
    event.Skip();
    if(m_simpleBook->GetSelection() == OUTLINE_TAB_PHP) {
        wxTreeItemId selection = m_treeCtrlPhp->GetSelection();
        if(selection.IsOk()) { m_treeCtrlPhp->ItemSelected(selection, true); }

    } else {
        wxString name = m_textCtrlSearch->GetValue();
        name.Trim().Trim(false);
        if(name.IsEmpty() == false) { m_tree->ActivateSelectedItem(); }
    }
}

void OutlineTab::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    EditorChanged();
}

void OutlineTab::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    m_tree->Clear();
    m_tree->ClearCache();
    m_treeCtrlPhp->Clear();
}

void OutlineTab::OnEditorClosed(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    if(editor) {
        if(m_tree->GetFilename() == editor->GetFileName()) {
            m_tree->Clear();
            m_tree->ClearCache();

        } else if(m_treeCtrlPhp->GetFilename() == editor->GetFileName()) {
            m_treeCtrlPhp->Clear();
        }
    }
}

void OutlineTab::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    wxWindowUpdateLocker locker(this);
    m_tree->Clear();
    m_treeCtrlPhp->DeleteAllItems();
}

void OutlineTab::OnFilesTagged(wxCommandEvent& e)
{
    e.Skip();
    CHECK_ENABLED();
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) {

        wxWindow* oldFocusedWindow = wxWindow::FindFocus();
        m_tree->BuildTree(editor->GetFileName(), false);
        wxWindow* focusedWindow = wxWindow::FindFocus();
        if(oldFocusedWindow != focusedWindow && oldFocusedWindow) {
            // restore the focus back the old window
            oldFocusedWindow->SetFocus();
        }

    } else {
        m_tree->Clear();
    }
}
void OutlineTab::OnMenu(wxContextMenuEvent& e)
{
    wxMenu menu;

    if(IsIncludeFileNode()) {
        menu.Append(wxEVT_SV_OPEN_FILE, _("Open..."));
        menu.Connect(wxEVT_SV_OPEN_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutlineTab::OnOpenFile),
                     NULL, this);

    } else {
        menu.Append(wxEVT_SV_GOTO_DECLARATION, _("Goto Declaration"));
        menu.Append(wxEVT_SV_GOTO_DEFINITION, _("Goto Implementation"));
        menu.AppendSeparator();
        menu.Append(wxEVT_SV_FIND_REFERENCES, _("Find References..."));
        menu.AppendSeparator();
        menu.Append(wxEVT_SV_RENAME_SYMBOL, _("Rename Symbol..."));

        menu.Connect(wxEVT_SV_GOTO_DEFINITION, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnGotoImpl), NULL, this);
        menu.Connect(wxEVT_SV_GOTO_DECLARATION, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnGotoDecl), NULL, this);
        menu.Connect(wxEVT_SV_FIND_REFERENCES, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnFindReferenes), NULL, this);
        menu.Connect(wxEVT_SV_RENAME_SYMBOL, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnRenameSymbol), NULL, this);
    }

    m_tree->PopupMenu(&menu);
}

void OutlineTab::OnGotoDecl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_decl"));
    gsgs_Event().TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnGotoImpl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_impl"));
    gsgs_Event().TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnFindReferenes(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_references"));
    gsgs_Event().TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnRenameSymbol(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("rename_symbol"));
    gsgs_Event().TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnItemSelectedUI(wxUpdateUIEvent& e)
{
    IEditor* editor = m_mgr->GetActiveEditor();
    e.Enable(editor && editor->GetSelection().IsEmpty() == false);
}

bool OutlineTab::IsIncludeFileNode() { return m_tree->IsSelectedItemIncludeFile(); }

void OutlineTab::OnOpenFile(wxCommandEvent& e)
{
    wxString includedFile = m_tree->GetSelectedIncludeFile();
    if(includedFile.IsEmpty()) return;

    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("open_include_file"));
    evt.SetString(includedFile);
    gsgs_Event().TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnPhpItemSelected(wxTreeEvent& event)
{
    event.Skip();
    m_treeCtrlPhp->ItemSelected(event.GetItem(), false);
}

void OutlineTab::OnEditorSaved(gsgs::Event& event)
{
    event.Skip();
    CHECK_ENABLED();
    wxFileName filename(event.GetFileName());
    if(FileUtil::IsPHPFile(filename)) {
        m_treeCtrlPhp->BuildTree(filename);
    } else if(FileUtil::IsCxxFile(filename)) {
        gsgsDEBUG() << "Outline: editor saved";
        m_tree->BuildTree(filename, true);
    }
}

void OutlineTab::OnPhpItemActivated(wxTreeEvent& event)
{
    event.Skip();
    m_treeCtrlPhp->ItemSelected(event.GetItem(), true);
}

void OutlineTab::OnSortAlpha(wxCommandEvent& event)
{
    m_sortCxxTreeAlphabetically = event.IsChecked();
    gsgs_Config().Write("OutlineView/SortCxxAlphabetically", m_sortCxxTreeAlphabetically);
    m_tree->SetSortByLineNumber(!m_sortCxxTreeAlphabetically);
    CallAfter(&OutlineTab::DoRefreshCxxView);
}

void OutlineTab::OnSortAlphaUI(wxUpdateUIEvent& event) { event.Check(m_sortCxxTreeAlphabetically); }

void OutlineTab::DoRefreshCxxView()
{
    wxFileName fn = m_tree->GetFilename();
    m_tree->Clear();
    m_tree->BuildTree(fn, true);
}

void OutlineTab::EditorChanged()
{
    CHECK_ENABLED();
    IEditor* editor = m_mgr->GetActiveEditor();
    LexerStyle::Ptr_t phpLexer = gsgs_Style().GetLexer("php");
    LexerStyle::Ptr_t cxxLexer = gsgs_Style().GetLexer("c++");

    gsgsDEBUG() << "Outline: editor changed event";

    // Use the lexer to determine if we can show outline
    if(editor && cxxLexer && FileUtil::IsCxxFile(editor->GetFileName())) {
        m_tree->BuildTree(editor->GetFileName(), true);
        m_simpleBook->SetSelection(OUTLINE_TAB_CXX);
        m_textCtrlSearch->Enable(true);

    } else if(editor && phpLexer && FileUtil::IsPHPFile(editor->GetFileName())) {
        m_tree->Clear();
        m_treeCtrlPhp->BuildTree(editor->GetFileName());
        m_simpleBook->SetSelection(OUTLINE_TAB_PHP);
        m_textCtrlSearch->Enable(true);

    } else {
        if(editor) { gsgsDEBUG() << "Could not match an Outline to file:" << editor->GetFileName(); }
        m_simpleBook->SetSelection(OUTLINE_PLACE_HOLDER_PAGE);
        m_textCtrlSearch->Enable(false);
    }
}
