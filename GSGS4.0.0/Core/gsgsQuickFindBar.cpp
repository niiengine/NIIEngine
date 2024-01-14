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
#include "gsgsQuickFindBar.h"
#include "gsgsImageManager.h"
#include "gsgsBookmarkManager.h"
#include "gsgsConfig.h"
#include "gsgsEvent.h"
#include "gsgsToolBar.h"
#include "gsgsStyleManager.h"
#include "gsgsEditorConfig.h"
#include "globals.h"
#include "imanager.h"
//#include "gsgsManager.h"
#include "plugin.h"
#include <wx/dcbuffer.h>
#include <wx/gdicmn.h>
#include <wx/regex.h>
//#include <wx/statline.h>
//#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

namespace gsgs
{
    DEFINE_EVENT_TYPE(QUICKFIND_COMMAND_EVENT)
    //-----------------------------------------------------------------
    #define CHECK_FOCUS_WIN(evt)                            \
        {                                                   \
            wxWindow* focus = wxWindow::FindFocus();        \
            if(focus != m_sci && focus != m_textCtrlFind) { \
                evt.Skip();                                 \
                return;                                     \
            }                                               \
                                                            \
            if(!m_sci || m_sci->GetLength() == 0) {         \
                evt.Skip();                                 \
                return;                                     \
            }                                               \
        }
    //-----------------------------------------------------------------
    void PostCommandEvent(wxWindow* destination, wxWindow* FocusedControl)
    {
        // Posts an event that signals for SelectAll() to be done after a delay
        // This is often needed in >2.9, as scintilla seems to steal the selection
        const static int DELAY_COUNT = 10;

        wxCommandEvent event(QUICKFIND_COMMAND_EVENT);
        event.SetInt(DELAY_COUNT);
        event.SetEventObject(FocusedControl);
        wxPostEvent(destination, event);
    }
    //-----------------------------------------------------------------
    QuickFindBar::QuickFindBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style)
        , m_sci(NULL)
        , m_lastTextPtr(NULL)
        , m_eventsConnected(false)
        , m_regexType(kRegexNone)
        , m_disableTextUpdateEvent(false)
        , m_searchFlags(0)
        , m_highlightMatches(false)
        , m_replaceInSelection(false)
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_toolbar =
            new ToolBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTB_NODIVIDER | wxTB_FLAT);
        m_toolbar->SetToolBitmapSize(wxSize(16, 16));

        mainSizer->Add(m_toolbar, 0, wxEXPAND, 0);

        wxFlexGridSizer* flexGridSizer107 = new wxFlexGridSizer(0, 4, 0, 0);
        flexGridSizer107->SetFlexibleDirection(wxBOTH);
        flexGridSizer107->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer107->AddGrowableCol(0);

        mainSizer->Add(flexGridSizer107, 1, wxEXPAND, 0);

        m_textCtrlFind = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                        wxTE_RICH | wxTE_PROCESS_ENTER);

        flexGridSizer107->Add(m_textCtrlFind, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 2);

        m_buttonFind =
            new DefaultButton(this, wxID_FIND, _("Find"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonFind->SetDefault();

        flexGridSizer107->Add(m_buttonFind, 0, wxALL | wxEXPAND, 2);

        m_buttonFindPrev =
            new DefaultButton(this, wxID_BACKWARD, _("Find Prev"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer107->Add(m_buttonFindPrev, 0, wxALL | wxEXPAND, 2);

        m_buttonFindAll =
            new DefaultButton(this, wxID_FIND_ALL, _("Find All"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer107->Add(m_buttonFindAll, 0, wxALL | wxEXPAND, 2);

        m_textCtrlReplace = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                           wxTE_RICH | wxTE_PROCESS_ENTER);

        flexGridSizer107->Add(m_textCtrlReplace, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 2);

        m_buttonReplace =
            new DefaultButton(this, wxID_REPLACE, _("Replace"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer107->Add(m_buttonReplace, 0, wxALL | wxEXPAND, 2);

        m_buttonReplaceAll = new DefaultButton(this, wxID_REPLACE_ALL, _("Replace All"), wxDefaultPosition,
                                                wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer107->Add(m_buttonReplaceAll, 0, wxALL | wxEXPAND, 2);

        // Connect events
        m_textCtrlFind->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QuickFindBar::OnEnter), NULL, this);
        m_textCtrlFind->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(QuickFindBar::OnText), NULL, this);
        m_textCtrlFind->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnKeyDown), NULL, this);
        m_buttonFind->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnFind), NULL, this);
        m_buttonFind->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnFindUI), NULL, this);
        m_buttonFind->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_buttonFindPrev->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnFindPrev), NULL,
                                  this);
        m_buttonFindPrev->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnFindPrevUI), NULL, this);
        m_buttonFindPrev->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_buttonFindAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnFindAll), NULL,
                                 this);
        m_buttonFindAll->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnFindAllUI), NULL, this);
        m_buttonFindAll->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_textCtrlReplace->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                                   wxCommandEventHandler(QuickFindBar::OnReplaceTextUpdated), NULL, this);
        m_textCtrlReplace->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QuickFindBar::OnReplaceTextEnter),
                                   NULL, this);
        m_textCtrlReplace->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnReplaceKeyDown), NULL, this);
        m_buttonReplace->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnReplace), NULL,
                                 this);
        m_buttonReplace->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnReplaceUI), NULL, this);
        m_buttonReplace->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_buttonReplaceAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnReplaceAll),
                                    NULL, this);
        m_buttonReplaceAll->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnReplaceAllUI), NULL, this);
        m_buttonReplaceAll->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
    
        // SetBackgroundStyle(wxBG_STYLE_PAINT);
        // Add the 'close' button
        ImageManager* bmps = &gsgs_Image();
        gsgs_Style().RegisterWindow(this);
        
        // Handle Edit events
        m_findEventsHandler.Reset(new EditEventsHandler(m_textCtrlFind));
        m_replaceEventsHandler.Reset(new EditEventsHandler(m_textCtrlReplace));
        m_findEventsHandler->NoUnbind();
        m_replaceEventsHandler->NoUnbind();
        m_toolbar->SetMiniToolBar(true);
        m_toolbar->AddTool(wxID_CLOSE, _("Close"), bmps->LoadBitmap("file_close"), _("Close"), wxITEM_NORMAL);
        m_toolbar->AddSeparator();
        m_matchesFound = gsgs_Style().createStaticText(m_toolbar, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1),
                                          wxST_NO_AUTORESIZE | wxALIGN_LEFT);
        m_toolbar->AddControl(m_matchesFound);
        m_toolbar->AddStretchableSpace();
        m_toolbar->AddTool(XRCID("case-sensitive"), _("Case Sensitive"), bmps->LoadBitmap("case-sensitive"), "", wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("whole-word"), _("Whole word"), bmps->LoadBitmap("whole-word"), "", wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("use-regex"), _("Regex"), bmps->LoadBitmap("regular-expression"), "", wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("highlight-matches"), _("Highlight matches"), bmps->LoadBitmap("marker"), "", wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("replace-in-selection"), _("Replace In Selection"), bmps->LoadBitmap("text_selection"), "",wxITEM_CHECK);
        m_toolbar->Realize();
        m_toolbar->Bind(wxEVT_TOOL, &QuickFindBar::OnHide, this, wxID_CLOSE);
        m_toolbar->Bind(wxEVT_TOOL,
                        [&](wxCommandEvent& e) {
                            if(e.IsChecked()) {
                                m_searchFlags |= wxSTC_FIND_MATCHCASE;
                            } else {
                                m_searchFlags &= ~wxSTC_FIND_MATCHCASE;
                            }
                        },
                        XRCID("case-sensitive"));
        m_toolbar->Bind(wxEVT_TOOL,
                        [&](wxCommandEvent& e) {
                            if(e.IsChecked()) {
                                m_searchFlags |= wxSTC_FIND_WHOLEWORD;
                            } else {
                                m_searchFlags &= ~wxSTC_FIND_WHOLEWORD;
                            }
                        },
                        XRCID("whole-word"));
        m_toolbar->Bind(wxEVT_TOOL,
                        [&](wxCommandEvent& e) {
                            if(e.IsChecked()) {
                                m_searchFlags |= wxSTC_FIND_REGEXP;
                            } else {
                                m_searchFlags &= ~wxSTC_FIND_REGEXP;
                            }
                        },
                        XRCID("use-regex"));
        m_toolbar->Bind(wxEVT_TOOL,
                        [&](wxCommandEvent& e) {
                            m_highlightMatches = e.IsChecked();
                            DoHighlightMatches(m_highlightMatches);
                        },
                        XRCID("highlight-matches"));
        m_toolbar->Bind(wxEVT_TOOL, [&](wxCommandEvent& e) { m_replaceInSelection = e.IsChecked(); },
                        XRCID("replace-in-selection"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_MATCHCASE); },
                        XRCID("case-sensitive"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_REGEXP); },
                        XRCID("use-regex"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_WHOLEWORD); },
                        XRCID("whole-word"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_highlightMatches); },
                        XRCID("highlight-matches"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_replaceInSelection); },
                        XRCID("replace-in-selection"));

        wxTheApp->Bind(wxEVT_MENU, &QuickFindBar::OnFindNextCaret, this, XRCID("find_next_at_caret"));
        wxTheApp->Bind(wxEVT_MENU, &QuickFindBar::OnFindPreviousCaret, this, XRCID("find_previous_at_caret"));

        gsgs_Event().Bind(wxEVT_FINDBAR_RELEASE_EDITOR, &QuickFindBar::OnReleaseEditor, this);
        Connect(QUICKFIND_COMMAND_EVENT, wxCommandEventHandler(QuickFindBar::OnQuickFindCommandEvent), NULL, this);

        // Initialize the list with the history
        m_searchHistory.SetItems(gsgs_Config().GetQuickFindSearchItems());
        m_replaceHistory.SetItems(gsgs_Config().GetQuickFindReplaceItems());

        // Update the search flags
        int tts;
        gsgs_Config().Read("FindBar/SearchFlags", tts, 0);
        m_searchFlags = tts;
        gsgs_Config().Read("FindBar/HighlightOccurences", m_highlightMatches, false);

        // Make sure that the 'Replace' field is selected when we hit TAB while in the 'Find' field
        m_textCtrlReplace->MoveAfterInTabOrder(m_textCtrlFind);
        // Bind(wxEVT_PAINT, &QuickFindBar::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& e) { wxUnusedVar(e); });
        GetSizer()->Fit(this);
        Layout();
    }
    //-----------------------------------------------------------------
    QuickFindBar::~QuickFindBar()
    {
        m_textCtrlFind->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QuickFindBar::OnEnter), NULL, this);
        m_textCtrlFind->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(QuickFindBar::OnText), NULL, this);
        m_textCtrlFind->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnKeyDown), NULL, this);
        m_buttonFind->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnFind), NULL, this);
        m_buttonFind->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnFindUI), NULL, this);
        m_buttonFind->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_buttonFindPrev->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnFindPrev),
                                     NULL, this);
        m_buttonFindPrev->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnFindPrevUI), NULL, this);
        m_buttonFindPrev->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_buttonFindAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnFindAll), NULL,
                                    this);
        m_buttonFindAll->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnFindAllUI), NULL, this);
        m_buttonFindAll->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_textCtrlReplace->Disconnect(wxEVT_COMMAND_TEXT_UPDATED,
                                      wxCommandEventHandler(QuickFindBar::OnReplaceTextUpdated), NULL, this);
        m_textCtrlReplace->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QuickFindBar::OnReplaceTextEnter),
                                      NULL, this);
        m_textCtrlReplace->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnReplaceKeyDown), NULL, this);
        m_buttonReplace->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnReplace), NULL,
                                    this);
        m_buttonReplace->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnReplaceUI), NULL, this);
        m_buttonReplace->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
        m_buttonReplaceAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QuickFindBar::OnReplaceAll),
                                       NULL, this);
        m_buttonReplaceAll->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnReplaceAllUI), NULL,
                                       this);
        m_buttonReplaceAll->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnButtonKeyDown), NULL, this);
    
        // m_findEventsHandler.Reset(nullptr);
        // m_replaceEventsHandler.Reset(nullptr);
        //Unbind(wxEVT_PAINT, &QuickFindBar::OnPaint, this);
        gsgs_Style().RegisterWindow(this);
        
        // Remember the buttons clicked
        gsgs_Config().Write("FindBar/SearchFlags", (int)DoGetSearchFlags());
        gsgs_Config().Write("FindBar/HighlightOccurences", m_highlightMatches);

        wxTheApp->Unbind(wxEVT_MENU, &QuickFindBar::OnFindNextCaret, this, XRCID("find_next_at_caret"));
        wxTheApp->Unbind(wxEVT_MENU, &QuickFindBar::OnFindPreviousCaret, this, XRCID("find_previous_at_caret"));
        gsgs_Event().Unbind(wxEVT_FINDBAR_RELEASE_EDITOR, &QuickFindBar::OnReleaseEditor, this);

        gsgs_Event().Unbind(wxEVT_ALL_EDITORS_CLOSED, [&](wxCommandEvent& event) {
            event.Skip();
            this->SetEditor(NULL);
        });

        gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, [&](wxCommandEvent& event) {
            event.Skip();
            // See if we got a new editor
            IEditor* editor = GetManager()->GetActiveEditor();
            if(editor) {
                this->SetEditor(editor->GetCtrl());
                return;
            }
            SetEditor(NULL);
        });
    }
    //-----------------------------------------------------------------
    bool QuickFindBar::Show(bool show)
    {
        if(!m_sci && show) { return false; }
        return DoShow(show, wxEmptyString);
    }
    //-----------------------------------------------------------------
    #define SHOW_STATUS_MESSAGES(searchFlags) (true)
    //-----------------------------------------------------------------
    bool QuickFindBar::DoSearch(size_t searchFlags) 
    { 
        return Search(m_sci, m_textCtrlFind->GetValue(), searchFlags, this); 
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnHide(wxCommandEvent& e)
    {
        // Kill any "...continued from start" statusbar message
        GetManager()->GetStatusBar()->SetMessage(wxEmptyString);

        // Clear all
        Show(false);
        e.Skip();
    }
    //-----------------------------------------------------------------
    #define UPDATE_FIND_HISTORY()                                                \
        if(!m_textCtrlFind->GetValue().IsEmpty()) {                              \
            m_searchHistory.Add(m_textCtrlFind->GetValue());                     \
            gsgs_Config().SetQuickFindSearchItems(m_searchHistory.GetItems(), false); \
        }
    //-----------------------------------------------------------------
    void QuickFindBar::OnNext(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        UPDATE_FIND_HISTORY();
        size_t flags = kSearchForward;
        m_onNextPrev = true;
        DoSearch(flags);
        m_onNextPrev = false;
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnPrev(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        UPDATE_FIND_HISTORY();
        size_t flags = 0;
        m_onNextPrev = true;
        DoSearch(flags);
        m_onNextPrev = false;
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnText(wxCommandEvent& e)
    {
        e.Skip();
        if(!m_replaceInSelection && !m_disableTextUpdateEvent) { CallAfter(&QuickFindBar::DoSearchCB, kSearchForward); }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnKeyDown(wxKeyEvent& e)
    {
        switch(e.GetKeyCode()) {
        case WXK_DOWN: {
            DoArrowDown(m_searchHistory, m_textCtrlFind);
            break;
        }
        case WXK_UP: {
            DoArrowUp(m_searchHistory, m_textCtrlFind);
            break;
        }
        case WXK_ESCAPE: {
            wxCommandEvent dummy;
            OnHide(dummy);
            DoHighlightMatches(false);
            break;
        }
        default: {
            e.Skip();
            break;
        }
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplaceKeyDown(wxKeyEvent& e)
    {
        switch(e.GetKeyCode()) {
        case WXK_DOWN: {
            DoArrowDown(m_replaceHistory, m_textCtrlReplace);
            break;
        }
        case WXK_UP: {
            DoArrowUp(m_replaceHistory, m_textCtrlReplace);
            break;
        }
        case WXK_ESCAPE: {
            wxCommandEvent dummy;
            OnHide(dummy);
            DoHighlightMatches(false);
            break;
        }
        default: {
            e.Skip();
            break;
        }
        }
    }
    /*
    void QuickFindBar::OnUpdateUI(wxUpdateUIEvent& e)
    {
        e.Enable(gsgs_Manager()->IsShutdownInProgress() == false && m_sci && m_sci->GetLength() > 0 &&
                 !m_textCtrlFind->GetValue().IsEmpty());
    }
    */
    //-----------------------------------------------------------------
    void QuickFindBar::OnEnter(wxCommandEvent& e)
    {
        wxUnusedVar(e);

        if(!m_textCtrlFind->GetValue().IsEmpty()) {
            m_searchHistory.Add(m_textCtrlFind->GetValue());
            gsgs_Config().SetQuickFindSearchItems(m_searchHistory.GetItems(), false);
        }

        bool shift = wxGetKeyState(WXK_SHIFT);
        if(shift) {
            OnPrev(e);
        } else {
            OnNext(e);
        }
        // Without this call, the caret is placed at the start of the searched
        // text, this at least places the caret at the end
        CallAfter(&QuickFindBar::DoSetCaretAtEndOfText);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplace(wxCommandEvent& event)
    {
        DoReplace();
        // Trigger another search
        DoSearch(kSearchForward);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoReplace()
    {
        if(!m_sci) return;

        wxString findwhat = m_textCtrlFind->GetValue();
        if(findwhat.IsEmpty()) return;

        UPDATE_FIND_HISTORY();

        wxString findWhatSciVersion = findwhat;
        DoFixRegexParen(findWhatSciVersion);

        // No selection?
        if(m_sci->GetSelections() == 0) {
            DoSearch(kSearchForward);
            return;
        }

        // No selection?
        if(m_sci->GetSelections() != 1) {
            DoSearch(kSearchForward);
            return;
        }

        // did we got a match?
        if(m_sci->GetSelections() != 1) return;

        int selStart, selEnd;
        m_sci->GetSelection(&selStart, &selEnd);
        if(selStart == selEnd) {
            // not a real selection
            DoSearch(kSearchForward);
            return;
        }

        // Ensure that the selection matches our search pattern
        size_t searchFlags = DoGetSearchFlags();
        if(m_sci->FindText(selStart, selEnd, searchFlags & wxSTC_FIND_REGEXP ? findWhatSciVersion : findwhat,
                           searchFlags) == wxNOT_FOUND) {
            // we got a selection, but it does not match our search
            return;
        }

        wxString selectedText = m_sci->GetTextRange(selStart, selEnd);

    #ifndef __WXMAC__
        int re_flags = wxRE_ADVANCED;
    #else
        int re_flags = wxRE_DEFAULT;
    #endif

        wxString replaceWith = m_textCtrlReplace->GetValue();
        if(!replaceWith.IsEmpty()) {
            m_replaceHistory.Add(replaceWith);
            gsgs_Config().SetQuickFindReplaceItems(m_replaceHistory.GetItems(), false);
        }

        size_t replacementLen = replaceWith.length();
        if(searchFlags & wxSTC_FIND_REGEXP) {

            // Regular expresson search
            if(!(searchFlags & wxSTC_FIND_MATCHCASE)) { re_flags |= wxRE_ICASE; }

            wxRegEx re(findwhat, re_flags);
            if(re.IsValid() && re.Matches(selectedText)) {
                re.Replace(&selectedText, replaceWith);

                // Keep the replacement length
                replacementLen = selectedText.length();

                // update the view
                m_sci->Replace(selStart, selEnd, selectedText);
            } else {
                return;
            }

        } else {
            // Normal search and replace
            m_sci->Replace(selStart, selEnd, replaceWith);
        }

        // Clear the selection
        m_sci->ClearSelections();
        m_sci->SetCurrentPos(selStart + replacementLen);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplaceEnter(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxCommandEvent evt(wxEVT_COMMAND_TOOL_CLICKED, ID_TOOL_REPLACE);
        GetEventHandler()->AddPendingEvent(evt);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::SetEditor(wxStyledTextCtrl* sci)
    {
        m_sci = sci;
        if(!m_sci) {
            DoShow(false, "");
            return;
        }
    }
    //-----------------------------------------------------------------
    int QuickFindBar::GetCloseButtonId() { return ID_TOOL_CLOSE; }
    //-----------------------------------------------------------------
    bool QuickFindBar::Show(const wxString& findWhat, bool showReplace)
    {
        // Same as Show() but set the 'findWhat' field with findWhat
        // and show/hide the 'Replace' section depending on the bool
        if(!m_sci) return false;
        return DoShow(true, findWhat, showReplace);
    }
    //-----------------------------------------------------------------
    bool QuickFindBar::DoShow(bool s, const wxString& findWhat, bool showReplace)
    {
    #ifdef __WXMSW__
        wxWindowUpdateLocker locker(this);
    #endif
        bool res = wxPanel::Show(s);
        if(s && m_sci) {
            // Delete the indicators
            m_sci->SetIndicatorCurrent(1);
            m_sci->IndicatorClearRange(0, m_sci->GetLength());

            if(gsgs_EditorConfig().GetOptions()->GetClearHighlitWordsOnFind()) {
                m_sci->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
                m_sci->IndicatorClearRange(0, m_sci->GetLength());
            }
        }

        if(s) {
            // Show or Hide the 'Replace' section as requested
            wxSizer* flexgridsizer = m_textCtrlFind->GetContainingSizer();
            if (flexgridsizer) {
                if (showReplace) {
                    flexgridsizer->ShowItems(true);
                } else {
                    for (size_t n=4; n < 7; ++n) {
                        flexgridsizer->Hide(n);
                    }
                }
            }
        }
        if(res) { 
            GetParent()->GetSizer()->Layout(); 
        }

        m_replaceInSelection = !findWhat.IsEmpty() && findWhat.Contains("\n");
        if(!m_sci) {
            // nothing to do

        } else if(!s) {
            // hiding
            DoHighlightMatches(false);
            m_sci->SetFocus();

        } else if(!findWhat.IsEmpty()) {

            if(findWhat.Contains("\n")) {
                // Multiline selection
                // enable the 'Replace in Selection'
                m_textCtrlFind->ChangeValue("");
                m_textCtrlFind->SetFocus();
            } else {
                m_textCtrlFind->ChangeValue(findWhat);
                m_textCtrlFind->SelectAll();
                m_textCtrlFind->SetFocus();
                if(m_highlightMatches) {
                    if(!(m_searchFlags & wxSTC_FIND_REGEXP) || m_textCtrlFind->GetValue().Length() > 2) {
                        DoHighlightMatches(true);
                    }
                }
                PostCommandEvent(this, m_textCtrlFind);
            }

        } else {
            if(m_sci->GetSelections() > 1) {}
            wxString findWhat = DoGetSelectedText().BeforeFirst(wxT('\n'));
            if(!findWhat.IsEmpty()) { m_textCtrlFind->ChangeValue(findWhat); }

            m_textCtrlFind->SelectAll();
            m_textCtrlFind->SetFocus();
            if(m_highlightMatches) {
                if(!(m_searchFlags & wxSTC_FIND_REGEXP) || m_textCtrlFind->GetValue().Length() > 2) {
                    DoHighlightMatches(true);
                }
            }
            PostCommandEvent(this, m_textCtrlFind);
        }
        return res;
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindNextCaret(wxCommandEvent& e)
    {
        CHECK_FOCUS_WIN(e);

        wxString selection(DoGetSelectedText());
        if(selection.IsEmpty()) {
            // select the word
            long pos = m_sci->GetCurrentPos();
            long start = m_sci->WordStartPosition(pos, true);
            long end = m_sci->WordEndPosition(pos, true);

            selection = m_sci->GetTextRange(start, end);
            if(selection.IsEmpty() == false) m_sci->SetCurrentPos(start);
        }

        if(selection.IsEmpty()) return;

        m_textCtrlFind->ChangeValue(selection);
        DoSearch(kSearchForward);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindPreviousCaret(wxCommandEvent& e)
    {
        CHECK_FOCUS_WIN(e);

        wxString selection(DoGetSelectedText());
        if(selection.IsEmpty()) {
            // select the word
            long pos = m_sci->GetCurrentPos();
            long start = m_sci->WordStartPosition(pos, true);
            long end = m_sci->WordEndPosition(pos, true);

            selection = m_sci->GetTextRange(start, end);
            if(selection.IsEmpty() == false) m_sci->SetCurrentPos(start);
        }

        if(selection.IsEmpty()) return;

        m_textCtrlFind->ChangeValue(selection);
        DoSearch(0);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoSelectAll(bool addMarkers)
    {
        if(!m_sci || m_sci->GetLength() == 0 || m_textCtrlFind->GetValue().IsEmpty()) return;
        GetManager()->SetStatusMessage(wxEmptyString);

        if(addMarkers) {
            m_sci->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            m_sci->IndicatorClearRange(0, m_sci->GetLength());
        }

        wxString find = m_textCtrlFind->GetValue();
        int flags = DoGetSearchFlags();

        // Since scintilla uses a non POSIX way of handling the regex paren
        // fix them
        if(flags & wxSTC_FIND_REGEXP) { DoFixRegexParen(find); }

        // Ensure that we have at least one match before we continue
        if(m_sci->FindText(0, m_sci->GetLastPosition(), find, flags) == wxNOT_FOUND) {
            GetManager()->SetStatusMessage(_("No match found"), 1);
            return;
        }

        // We got at least one match
        m_sci->SetCurrentPos(0);
        m_sci->SetSelectionEnd(0);
        m_sci->SetSelectionStart(0);

        m_sci->ClearSelections();
        m_sci->SearchAnchor();

        std::vector<std::pair<int, int> > matches; // pair of matches selStart+selEnd
        int pos = m_sci->SearchNext(flags, find);
        while(pos != wxNOT_FOUND) {
            std::pair<int, int> match;
            m_sci->GetSelection(&match.first, &match.second);
            if(match.first == match.second) {
                GetManager()->SetStatusMessage(_("No match found"), 1);
                return;
            }

            m_sci->SetCurrentPos(match.second);
            m_sci->SetSelectionStart(match.second);
            m_sci->SetSelectionEnd(match.second);
            m_sci->SearchAnchor();
            pos = m_sci->SearchNext(flags, find);
            matches.push_back(match);
        }

        if(matches.empty()) {
            GetManager()->SetStatusMessage(_("No match found"), 1);
            return;
        }

        // add selections
        m_sci->ClearSelections();
        for(size_t i = 0; i < matches.size(); ++i) {
            if(i == 0) {
                m_sci->SetSelection(matches.at(i).first, matches.at(i).second);
                m_sci->SetMainSelection(0);
                DoEnsureLineIsVisible(m_sci, m_sci->LineFromPosition(matches.at(0).first));
            } else {
                m_sci->AddSelection(matches.at(i).first, matches.at(i).second);
            }
        }
        Show(false);
        wxString message;
        message << _("Found and selected ") << matches.size() << _(" matches");
        GetManager()->SetStatusMessage(message, 2);
        m_sci->SetMainSelection(0);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoHighlightMatches(bool checked)
    {
        IEditor* editor = dynamic_cast<IEditor*>(m_sci);
        if(checked && editor && !m_textCtrlFind->GetValue().IsEmpty()) {
            int flags = DoGetSearchFlags();
            wxString findwhat = m_textCtrlFind->GetValue();
            if(!m_sci || m_sci->GetLength() == 0 || findwhat.IsEmpty()) return;

            // Do we have at least one match?
            if(m_sci->FindText(0, m_sci->GetLastPosition(), findwhat, flags) == wxNOT_FOUND) return;
            m_sci->ClearSelections();
            m_sci->SetCurrentPos(0);
            m_sci->SetSelectionEnd(0);
            m_sci->SetSelectionStart(0);
            m_sci->MarkerDeleteAll(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            editor->SetFindBookmarksActive(true);
            //editor->DelAllMarkers(smt_find_bookmark);

            m_sci->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            m_sci->IndicatorClearRange(0, m_sci->GetLength());

            int found = 0;
            while(true) {
                m_sci->SearchAnchor();
                if(m_sci->SearchNext(flags, findwhat) != wxNOT_FOUND) {
                    int selStart, selEnd;
                    m_sci->GetSelection(&selStart, &selEnd);
                    m_sci->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
                    m_sci->IndicatorFillRange(selStart, selEnd - selStart);
                    m_sci->MarkerAdd(m_sci->LineFromPosition(selStart), smt_find_bookmark);

                    // Clear the selection so the next 'SearchNext' will search forward
                    m_sci->SetCurrentPos(selEnd);
                    m_sci->SetSelectionEnd(selEnd);
                    m_sci->SetSelectionStart(selEnd);

                    found++;
                } else {
                    break;
                }
            }

            wxString matches;
            if(found) {
                matches << found << " " << (found > 1 ? _("results") : _("result"));
            } else {
                matches << _("No matches found");
            }
            m_matchesFound->SetLabel(matches);

        } else if(editor) {
            m_sci->MarkerDeleteAll(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            editor->SetFindBookmarksActive(false);
            //editor->DelAllMarkers(smt_find_bookmark);

            IEditor::List_t editors;
            GetManager()->GetAllEditors(editors);
            std::for_each(editors.begin(), editors.end(), [&](IEditor* pEditor) {
                pEditor->GetCtrl()->MarkerDeleteAll(smt_find_bookmark);
                pEditor->GetCtrl()->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
                pEditor->GetCtrl()->IndicatorClearRange(0, pEditor->GetCtrl()->GetLength());
            });
            m_matchesFound->SetLabel("");
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReceivingFocus(wxFocusEvent& event)
    {
        event.Skip();
        if((event.GetEventObject() == m_textCtrlFind) || (event.GetEventObject() == m_textCtrlReplace)) {
            PostCommandEvent(this, wxStaticCast(event.GetEventObject(), wxWindow));
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnQuickFindCommandEvent(wxCommandEvent& event)
    {
        if(event.GetInt() > 0) {
            // We need to delay further, or focus might be set too soon
            event.SetInt(event.GetInt() - 1);
            wxPostEvent(this, event);
        }

        if(event.GetEventObject() == m_textCtrlFind) {
            m_textCtrlFind->SetFocus();
            m_textCtrlFind->SelectAll();

        } else if(event.GetEventObject() == m_textCtrlReplace) {
            m_textCtrlReplace->SetFocus();
            m_textCtrlReplace->SelectAll();
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReleaseEditor(FindEvent& e)
    {
        wxStyledTextCtrl* win = e.GetCtrl();
        if(win && win == m_sci) {
            m_sci = NULL;
            Show(false);
        }
    }
    //-----------------------------------------------------------------
    wxStyledTextCtrl* QuickFindBar::DoCheckPlugins()
    {
        // Let the plugins a chance to provide their own window
        FindEvent evt(wxEVT_FINDBAR_ABOUT_TO_SHOW);
        gsgs_Event().ProcessEvent(evt);
        return evt.GetCtrl();
    }
    //-----------------------------------------------------------------
    bool QuickFindBar::ShowForPlugins()
    {
        m_sci = DoCheckPlugins();
        if(!m_sci) {
            return DoShow(false, "");
        } else {
            return DoShow(true, "");
        }
    }
    //-----------------------------------------------------------------
    wxString QuickFindBar::DoGetSelectedText()
    {
        if(!m_sci) { return wxEmptyString; }

        if(m_sci->GetSelections() > 1) {
            for(int i = 0; i < m_sci->GetSelections(); ++i) {
                int selStart = m_sci->GetSelectionNStart(i);
                int selEnd = m_sci->GetSelectionNEnd(i);
                if(selEnd > selStart) { return m_sci->GetTextRange(selStart, selEnd); }
            }
            return wxEmptyString;

        } else {
            return m_sci->GetSelectedText();
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnButtonNext(wxCommandEvent& e) { OnNext(e); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnButtonPrev(wxCommandEvent& e) { OnPrev(e); }
    //-----------------------------------------------------------------
    size_t QuickFindBar::DoGetSearchFlags() const { return m_searchFlags; }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindAll(wxCommandEvent& e) { DoSelectAll(true); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnButtonReplace(wxCommandEvent& e) { OnReplace(e); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindMouseWheel(wxMouseEvent& e)
    {
        // Do nothing and disable the mouse wheel
        // by not calling 'skip'
        wxUnusedVar(e);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoEnsureLineIsVisible(wxStyledTextCtrl* sci, int line)
    {
        if(line == wxNOT_FOUND) { line = sci->LineFromPosition(sci->GetSelectionStart()); }
        int linesOnScreen = sci->LinesOnScreen();
        if(!((line > sci->GetFirstVisibleLine()) && (line < (sci->GetFirstVisibleLine() + linesOnScreen)))) {
            // To place our line in the middle, the first visible line should be
            // the: line - (linesOnScreen / 2)
            int firstVisibleLine = line - (linesOnScreen / 2);
            if(firstVisibleLine < 0) { firstVisibleLine = 0; }
            sci->SetFirstVisibleLine(firstVisibleLine);
        }
        sci->EnsureVisible(line);
        sci->ScrollToColumn(0);
        int xScrollPosBefore = sci->GetScrollPos(wxHORIZONTAL);
        sci->EnsureCaretVisible();
        int xScrollPosAfter = sci->GetScrollPos(wxHORIZONTAL);
        if(xScrollPosBefore != xScrollPosAfter) {
            // EnsureCaretVisible scrolled the page
            // scroll it a bit more
            int scrollToPos = sci->GetSelectionStart();
            if(scrollToPos != wxNOT_FOUND) { sci->ScrollToColumn(sci->GetColumn(scrollToPos)); }
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoFixRegexParen(wxString& findwhat)
    {
        // Scintilla's REGEX group markers are \( and \)
        // while wxRegEx is usig bare ( and ) and the escaped version for
        // non regex manner
        findwhat.Replace("\\(", "/<!@#$");
        findwhat.Replace("\\)", "/>!@#$");
        findwhat.Replace("(", "<!@#$");
        findwhat.Replace(")", ">!@#$");

        findwhat.Replace("/<!@#$", "(");
        findwhat.Replace("/>!@#$", ")");
        findwhat.Replace("<!@#$", "\\(");
        findwhat.Replace(">!@#$", "\\)");
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoSetCaretAtEndOfText() { m_textCtrlFind->SetInsertionPointEnd(); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplaceAll(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        DoReplaceAll(m_replaceInSelection);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoReplaceAll(bool selectionOnly)
    {
        if(!selectionOnly) {
            m_sci->BeginUndoAction();
            m_sci->SetSelection(0, 0);
            m_sci->SetCurrentPos(0); // Start the search from the start
            size_t replaced(0);
            while(DoSearch(DoGetSearchFlags() | kDisableDisplayErrorMessages | kBreakWhenWrapSearch | kSearchForward)) {
                DoReplace();
                ++replaced;
            }
            
            wxString message;
            if (replaced) {
                message << _("Found and replaced ") << replaced << _(" matches");
            } else {
                message << _("No matches found");
            }
            GetManager()->SetStatusMessage(message, 5);
            
            m_sci->EndUndoAction();
            m_sci->ClearSelections();
        } else {
            if(!m_sci || m_sci->GetLength() == 0 || m_textCtrlFind->GetValue().IsEmpty()) return;
            UPDATE_FIND_HISTORY();
            GetManager()->SetStatusMessage(wxEmptyString);

            wxString findwhat = m_textCtrlFind->GetValue();
            int searchFlags = DoGetSearchFlags();

            // Since scintilla uses a non POSIX way of handling the regex paren
            // fix them
            if(searchFlags & wxSTC_FIND_REGEXP) { DoFixRegexParen(findwhat); }

            int from, to;
            if(selectionOnly && m_sci->GetSelectedText().IsEmpty()) return;
            if(selectionOnly) {
                m_sci->GetSelection(&from, &to);
            } else {
                from = 0;
                to = m_sci->GetLastPosition();
            }

            // Ensure that we have at least one match before we continue
            if(m_sci->FindText(from, to, findwhat, searchFlags) == wxNOT_FOUND) {
                GetManager()->SetStatusMessage(_("No match found"), 2);
                return;
            }

            int curpos = m_sci->GetCurrentPos();

            // We got at least one match
            m_sci->SetCurrentPos(0);
            m_sci->SetSelectionEnd(0);
            m_sci->SetSelectionStart(0);

            m_sci->ClearSelections();
            m_sci->SearchAnchor();
    #ifndef __WXMAC__
            int re_flags = wxRE_ADVANCED;
    #else
            int re_flags = wxRE_DEFAULT;
    #endif

            wxString replaceWith = m_textCtrlReplace->GetValue();
            if(!replaceWith.IsEmpty()) {
                m_replaceHistory.Add(replaceWith);
                gsgs_Config().SetQuickFindReplaceItems(m_replaceHistory.GetItems(), false);
            }

            m_sci->BeginUndoAction();
            int pos = m_sci->SearchNext(searchFlags, findwhat);
            size_t matchesCount = 0;
            while(pos != wxNOT_FOUND) {
                int selStart, selEnd, newpos;
                size_t replacementLen = replaceWith.length();
                m_sci->GetSelection(&selStart, &selEnd);
                if(!selectionOnly || ((pos >= from) && (pos < to))) {
                    wxString selectedText = m_sci->GetSelectedText();
                    if(searchFlags & wxSTC_FIND_REGEXP) {

                        // Regular expresson search
                        if(!(searchFlags & wxSTC_FIND_MATCHCASE)) { re_flags |= wxRE_ICASE; }

                        wxRegEx re(findwhat, re_flags);
                        if(re.IsValid() && re.Matches(selectedText)) {
                            re.Replace(&selectedText, replaceWith);

                            // Keep the replacement length
                            replacementLen = selectedText.length();

                            // update the view
                            m_sci->Replace(selStart, selEnd, selectedText);
                        } else {
                            return;
                        }

                    } else {
                        // Normal search and replace
                        m_sci->Replace(selStart, selEnd, replaceWith);
                    }
                    newpos = selStart + replacementLen;

                    // Extend the range (or shrink it) depending on the replacement
                    if(selectionOnly) {
                        int matchFoundLen = selEnd - selStart;
                        to += (replacementLen - matchFoundLen);
                    }
                } else {
                    // the match is not in the selection range
                    newpos = pos + replacementLen;
                    if(newpos <= pos) {
                        newpos = pos + 1; // make sure we dont hang
                    }

                    // Move to the next match
                    m_sci->SetCurrentPos(newpos);
                    m_sci->SetSelectionStart(newpos);
                    m_sci->SetSelectionEnd(newpos);
                    m_sci->SearchAnchor();
                    pos = m_sci->SearchNext(searchFlags, findwhat);
                    continue;
                }

                // Move to the next match
                m_sci->SetCurrentPos(newpos);
                m_sci->SetSelectionStart(newpos);
                m_sci->SetSelectionEnd(newpos);
                m_sci->SearchAnchor();
                pos = m_sci->SearchNext(searchFlags, findwhat);
                ++matchesCount;
            }
            m_sci->EndUndoAction();

            if(!matchesCount) {
                GetManager()->SetStatusMessage(_("No match found"), 2);
                return;
            }

            // add selections
            m_sci->ClearSelections();
            m_sci->SetSelectionStart(curpos);
            m_sci->SetSelectionEnd(curpos);
            m_sci->SetCurrentPos(curpos);
            DoEnsureLineIsVisible(m_sci, m_sci->LineFromPosition(curpos));

            wxString message;
            message << _("Found and replaced ") << matchesCount << _(" matches");
            GetManager()->SetStatusMessage(message, 5);

            // If needed, restore the selection
            if(selectionOnly) {
                m_sci->SetCurrentPos(from);
                m_sci->SetSelectionStart(from);
                m_sci->SetSelectionEnd(to);
            }
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFind(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        if(!gsgs_EditorConfig().GetOptions()->GetDontOverrideSearchStringWithSelection()) {
            // Highlighted text takes precedence over the current search string
            //    if(!IsShown()) {
            wxString selectedText = DoGetSelectedText();
            if(selectedText.IsEmpty() == false) {
                m_textCtrlFind->ChangeValue(selectedText);
                m_textCtrlFind->SelectAll();
            }
            //    }
        }
        DoSearch(kSearchForward);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindPrev(wxCommandEvent& event)
    {
        if(!gsgs_EditorConfig().GetOptions()->GetDontOverrideSearchStringWithSelection()) {
            wxString selectedText = DoGetSelectedText();
            if(selectedText.IsEmpty() == false) {
                m_textCtrlFind->ChangeValue(selectedText);
                m_textCtrlFind->SelectAll();
            }
        }
        DoSearch(0);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplaceTextEnter(wxCommandEvent& event) {}
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplaceTextUpdated(wxCommandEvent& event) {}
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindAllUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindPrevUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnFindUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplaceAllUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
    //-----------------------------------------------------------------
    void QuickFindBar::OnReplaceUI(wxUpdateUIEvent& event)
    {
        event.Enable(!m_textCtrlFind->IsEmpty() && !m_replaceInSelection);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoArrowDown(TerminalHistory& history, wxTextCtrl* ctrl)
    {
        const wxString& str = history.ArrowDown();
        if(!str.IsEmpty()) {
            ctrl->SelectAll();
            long from, to;
            ctrl->GetSelection(&from, &to);
            ctrl->Replace(from, to, str);
            ctrl->SelectNone();
            ctrl->SetInsertionPoint(ctrl->GetLastPosition());
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::DoArrowUp(TerminalHistory& history, wxTextCtrl* ctrl)
    {
        const wxString& str = history.ArrowUp();
        if(!str.IsEmpty()) {
            ctrl->SelectAll();
            long from, to;
            ctrl->GetSelection(&from, &to);
            ctrl->Replace(from, to, str);
            ctrl->SelectNone();
            ctrl->SetInsertionPoint(ctrl->GetLastPosition());
        }
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnButtonKeyDown(wxKeyEvent& event)
    {
        switch(event.GetKeyCode()) {
        case WXK_ESCAPE: {
            wxCommandEvent dummy;
            OnHide(dummy);
            DoHighlightMatches(false);
            break;
        }
        default: {
            event.Skip();
            break;
        }
        }
    }
    //-----------------------------------------------------------------
    bool QuickFindBar::Search(wxStyledTextCtrl* ctrl, const wxString& find_what, size_t search_flags, QuickFindBar* This)
    {
        wxString findwhat = find_what;
        if(findwhat.IsEmpty() && This) { findwhat = This->m_textCtrlFind->GetValue(); }
        if(!ctrl || ctrl->GetLength() == 0 || findwhat.IsEmpty()) return false;
        GetManager()->SetStatusMessage(wxEmptyString);
        if(This) { This->m_matchesFound->SetLabel(""); }
        if(This && (This->m_textCtrlFind->GetValue() != findwhat)) { This->m_textCtrlFind->ChangeValue(findwhat); }

        // Clear all search markers if desired
        if(gsgs_EditorConfig().GetOptions()->GetClearHighlitWordsOnFind()) {
            ctrl->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            ctrl->IndicatorClearRange(0, ctrl->GetLength());
        }

        wxString find = findwhat;
        bool fwd = search_flags & kSearchForward;
        int flags = 0;
        if(This) { flags = This->DoGetSearchFlags(); }

        // Since scintilla uses a non POSIX way of handling the paren
        // fix them
        if((flags & wxSTC_FIND_REGEXP) && This) { This->DoFixRegexParen(find); }

        int curpos = ctrl->GetCurrentPos();
        int start = wxNOT_FOUND;
        int end = wxNOT_FOUND;
        ctrl->GetSelection(&start, &end);
        if((end != wxNOT_FOUND) && fwd) {
            if(ctrl->FindText(start, end, find, flags) != wxNOT_FOUND) {
                // Incase we searching forward and the current selection matches the search string
                // Clear the selection and set the caret position to the end of the selection
                ctrl->SetCurrentPos(end);
                ctrl->SetSelectionEnd(end);
                ctrl->SetSelectionStart(end);
            }
        }

        int pos = wxNOT_FOUND;
        if(fwd) {
            ctrl->SearchAnchor();
            pos = ctrl->SearchNext(flags, find);
            if(pos == wxNOT_FOUND) {
                GetManager()->SetStatusMessage(_("Wrapped past end of file"), 1);
                if(search_flags & kBreakWhenWrapSearch) {
                    // Stop searching
                    return false;
                }
                ctrl->SetCurrentPos(0);
                ctrl->SetSelectionEnd(0);
                ctrl->SetSelectionStart(0);
                ctrl->SearchAnchor();
                pos = ctrl->SearchNext(flags, find);
            }
        } else {
            ctrl->SearchAnchor();
            pos = ctrl->SearchPrev(flags, find);
            if(pos == wxNOT_FOUND) {
                GetManager()->SetStatusMessage(_("Wrapped past end of file"), 1);
                if(search_flags & kBreakWhenWrapSearch) {
                    // Stop searching
                    return false;
                }
                int lastPos = ctrl->GetLastPosition();
                ctrl->SetCurrentPos(lastPos);
                ctrl->SetSelectionEnd(lastPos);
                ctrl->SetSelectionStart(lastPos);
                ctrl->SearchAnchor();
                pos = ctrl->SearchPrev(flags, find);
            }
        }

        if(pos == wxNOT_FOUND) {
            // Restore the caret position
            ctrl->SetCurrentPos(curpos);
            ctrl->ClearSelections();
            if(This) {
                This->DoHighlightMatches(false);
                This->m_matchesFound->SetLabel(_("No matches found"));
            }
            return false;
        }

        DoEnsureLineIsVisible(ctrl);

        if(This && This->m_highlightMatches && !This->m_onNextPrev && (pos != wxNOT_FOUND)) {
            // Fix issue when regex is enabled hanging the editor if too few chars
            if((This->m_searchFlags & wxSTC_FIND_REGEXP) && findwhat.Length() < 3) {
                return false;
            } else if(This) {
                This->DoHighlightMatches(true);
            }
        }

        int selStart, selEnd;
        ctrl->GetSelection(&selStart, &selEnd);
        return (selEnd > selStart);
    }
    //-----------------------------------------------------------------
    void QuickFindBar::OnPaint(wxPaintEvent& e)
    {
        wxAutoBufferedPaintDC dc(this);
        //dc.SetBrush(gsgs_Style().GetColour(wxSYS_COLOUR_3DFACE));
        //dc.SetPen(gsgs_Style().GetColour(wxSYS_COLOUR_3DFACE));
        dc.SetBrush(gsgs_Style().GetAppBgColour());
        dc.SetPen(gsgs_Style().GetAppBgColour());
        dc.DrawRectangle(GetClientRect());
    }
    //-----------------------------------------------------------------
}