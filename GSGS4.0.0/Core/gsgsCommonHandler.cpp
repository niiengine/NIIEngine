#include "gsgsCommonHandler.h"
#include "gsgsStyleManager.h"
#include "gsgsPathManager.h"
#include "gsgsLogManager.h"
#include "gsgsProcess.h"
#include "gsgsEditorConfig.h"
#include "gsgsTreeCtrl.h"
#include "gsgsWorkspace.h"
#include "process_impl.h"
//#include "gsgsFrame.h"
#include "gsgsWindowManager.h"
#include "gsgsStatusBar.h"
#include "gsgsImageManager.h"
#include "imanager.h"
#include "gsgsFileUtil.h"
#include "gsgsJSON.h"
#include "gsgsCommon.h"
#include "gsgsCompilerCmdParser.h"
#include "gsgsPluginCommon.h"
#include "gsgsZip.h"
#include "gsgsJob.h"
#include "ieditor.h"
#include "globals.h"
#include <thread>
#include <wx/frame.h>
#include <wx/fileconf.h>
#include <wx/listctrl.h>
#include <wx/dataview.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/uiaction.h>
#include <wx/dcmemory.h>
#include <wx/aui/framemanager.h>
#define IS_TYPEOF(Type, Win) (dynamic_cast<Type*>(Win))

namespace gsgs
{
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // ThemeHandlerHelper
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    ThemeHandlerHelper::ThemeHandlerHelper(wxWindow* win)
        : m_window(win)
    {
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &ThemeHandlerHelper::OnTheme, this);
        //gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ThemeHandlerHelper::OnColoursUpdated, this);
        //gsgs_Event().Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &ThemeHandlerHelper::OnPreferencesUpdated, this);
    }
    //---------------------------------------------------------------------------------------
    ThemeHandlerHelper::~ThemeHandlerHelper()
    {
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &ThemeHandlerHelper::OnTheme, this);
        //gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ThemeHandlerHelper::OnColoursUpdated, this);
        //gsgs_Event().Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &ThemeHandlerHelper::OnPreferencesUpdated, this);
    }
    //---------------------------------------------------------------------------------------
    void ThemeHandlerHelper::OnTheme(wxCommandEvent& e)
    {
        e.Skip();
        UpdateColours(m_window);
    }
    //---------------------------------------------------------------------------------------
    void ThemeHandlerHelper::UpdateColours(wxWindow* toplevel)
    {
        // Collect all toolbars
        std::queue<wxWindow*> q;
        std::vector<wxAuiToolBar*> toolbars;
        std::vector<wxWindow*> candidateWindows;
        q.push(toplevel);

        const wxColour & bgColour = gsgs_Style().GetAppBgColour();
        const wxColour & fgColour = gsgs_Style().GetAppTextColour();

        LexerStyle::Ptr_t textLexer = gsgs_EditorConfig().GetLexer("text");
        while(!q.empty()) {
            wxWindow* w = q.front();
            q.pop();
            if(dynamic_cast<wxAuiToolBar*>(w)) {
                toolbars.push_back(dynamic_cast<wxAuiToolBar*>(w));
            } 
            else if(IS_TYPEOF(wxListBox, w) || IS_TYPEOF(wxDataViewCtrl, w) || IS_TYPEOF(wxListCtrl, w)) 
            {
                w->SetBackgroundColour(bgColour);
                w->SetForegroundColour(fgColour);
                //w->Refresh();
            } 
            else if(IS_TYPEOF(wxStyledTextCtrl, w)) 
            {
                // wxSTC requires different method
                wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(w);
                if(stc->GetLexer() == wxSTC_LEX_NULL) 
                {
                    if(!textLexer) {
                        // Only modify text lexers
                        for(int i = 0; i < wxSTC_STYLE_MAX; i++) {
                            stc->StyleSetBackground(i, bgColour);
                            stc->StyleSetForeground(i, fgColour);
                        }
                    } 
                    else 
                    {
                        textLexer->Apply(stc);
                    }
                }
                //w->Refresh();
            }
            wxWindowList::compatibility_iterator iter = w->GetChildren().GetFirst();
            while (iter) {
                q.push(iter->GetData());
                iter = iter->GetNext();
            }
        }
        toplevel->Refresh();
        //    std::for_each(toolbars.begin(), toolbars.end(), [&](wxAuiToolBar* tb) {
        //        // Update the art if needed
        //        MainAuiTBArt* art = dynamic_cast<MainAuiTBArt*>(tb->GetArtProvider());
        //        if(!art) { tb->SetArtProvider(new MainAuiTBArt()); }
        //
        //#ifndef __WXOSX__
        //        for(size_t i = 0; i < tb->GetToolCount(); ++i) {
        //            wxAuiToolBarItem* tbItem = tb->FindToolByIndex(i);
        //            if(tbItem->GetBitmap().IsOk() &&
        //               (tbItem->GetKind() == wxITEM_NORMAL || tbItem->GetKind() == wxITEM_CHECK ||
        //                tbItem->GetKind() == wxITEM_DROPDOWN || tbItem->GetKind() == wxITEM_RADIO)) {
        //                tbItem->SetDisabledBitmap(gsgs_Style().CreateDisabledBitmap(tbItem->GetBitmap()));
        //            }
        //        }
        //#endif
        //        tb->Refresh();
        //    });

        //DoUpdateNotebookStyle(m_window);
    }
    //---------------------------------------------------------------------------------------
    void ThemeHandlerHelper::DoUpdateNotebookStyle(wxWindow* win)
    {
        if (win)
        {
            if (dynamic_cast<Notebook*>(win)) {
                Notebook* book = dynamic_cast<Notebook*>(win);
                book->SetArt(TabView::CreateRenderer(book->GetStyle()));
                LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
                wxColour activeTabBgColuor;
                if (lexer) { activeTabBgColuor = lexer->GetProperty(0).GetBgColour(); }

                // Enable tab switching using the mouse scrollbar
                book->EnableStyle(kNotebook_MouseScrollSwitchTabs,
                    gsgs_EditorConfig().GetOptions()->IsMouseScrollSwitchTabs());
            }

            wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
            while (pclNode) {
                wxWindow* pclChild = pclNode->GetData();
                this->DoUpdateNotebookStyle(pclChild);
                pclNode = pclNode->GetNext();
            }
        }
    }
    //---------------------------------------------------------------------------------------
    void ThemeHandlerHelper::OnPreferencesUpdated(wxCommandEvent& e)
    {
        e.Skip();
        DoUpdateNotebookStyle(m_window);
    }
    //---------------------------------------------------------------------------------------
    void ThemeHandlerHelper::OnColoursUpdated(Event & e) 
    { 
        e.Skip(); 
    }
    //---------------------------------------------------------------------------------------
    void ThemeHandlerHelper::UpdateNotebookColours(wxWindow * toplevel)
    {
        DoUpdateNotebookStyle(toplevel);
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // GenericSTCStyler
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    GenericSTCStyler::GenericSTCStyler(wxStyledTextCtrl* stc)
        : m_ctrl(stc)
        , m_nextAvailStyle(kLastStyle)
    {
        m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
        m_ctrl->Bind(wxEVT_STC_STYLENEEDED, &GenericSTCStyler::OnStyleNeeded, this);
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &GenericSTCStyler::OnThemChanged, this);
        ResetStyles();
    }
    //---------------------------------------------------------------------------------------
    GenericSTCStyler::~GenericSTCStyler()
    {
        m_ctrl->Unbind(wxEVT_STC_STYLENEEDED, &GenericSTCStyler::OnStyleNeeded, this);
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &GenericSTCStyler::OnThemChanged, this);
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::OnStyleNeeded(wxStyledTextEvent& event)
    {
        int startPos = m_ctrl->GetEndStyled();
        int endPos = event.GetPosition();
        wxString text = m_ctrl->GetTextRange(startPos, endPos);

    #if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
        // The scintilla syntax in wx3.1.1 changed
        m_ctrl->StartStyling(startPos);
    #else
        m_ctrl->StartStyling(startPos, 0x1f);
    #endif
        wxString lineText;
        while(GetNextLine(text, lineText)) {
            text = text.Mid(lineText.length());
            int style = GetStyleForLine(lineText);
            m_ctrl->SetStyling(lineText.length(), style);
        }
    }
    //---------------------------------------------------------------------------------------
    bool GenericSTCStyler::GetNextLine(const wxString& inText, wxString& lineText) const
    {
        lineText.Clear();
        wxString::const_iterator iter = inText.begin();
        while(iter != inText.end()) {
            const wxUniChar& ch = *iter;
            lineText << ch;
            ++iter;
            if(ch == '\n') {
                // found EOL
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------------------
    int GenericSTCStyler::GetStyleForLine(const wxString& lineText) const
    {
        wxString lcLine = lineText.Lower();
        for(size_t i = 0; i < m_words.size(); ++i) {
            if(lcLine.Contains(m_words[i].first)) { return m_words[i].second; }
        }

        // Return the default style, which is 0
        return 0;
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::ResetStyles()
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        if(!lexer) return;
        lexer->Apply(m_ctrl);
        m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
        InitDefaultStyles();
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::AddStyle(const wxArrayString& words, GenericSTCStyler::eStyles style)
    {
        if(words.IsEmpty()) { return; }
        for(size_t i = 0; i < words.size(); ++i) {
            m_words.push_back({ words.Item(i).Lower(), (int)style });
        }
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::AddUserStyle(const wxArrayString& words, const wxColour& fgColour, const wxColour& bgColour)
    {
        if(words.IsEmpty()) { return; }
        m_styleInfo.push_back(std::make_tuple(m_nextAvailStyle, fgColour, bgColour));
        for(size_t i = 0; i < words.size(); ++i) {
            m_words.push_back({ words.Item(i).Lower(), m_nextAvailStyle });
        }
        ++m_nextAvailStyle;
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::ApplyStyles()
    {
        std::for_each(m_styleInfo.begin(), m_styleInfo.end(), [&](const std::tuple<int, wxColour, wxColour>& t) {
            int style = std::get<0>(t);
            const wxColour& fgColour = std::get<1>(t);
            const wxColour& bgColour = std::get<2>(t);
            m_ctrl->StyleSetForeground(style, fgColour);
            if(bgColour.IsOk()) { m_ctrl->StyleSetBackground(style, bgColour); }
        });
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::OnThemChanged(wxCommandEvent& event)
    {
        event.Skip();
        ResetStyles();
        ApplyStyles();
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::ClearAllStyles()
    {
        m_styleInfo.clear();
        m_words.clear();
        ResetStyles();
    }
    //---------------------------------------------------------------------------------------
    void GenericSTCStyler::InitDefaultStyles()
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        if(!lexer) return;

        const StyleProperty& defaultStyle = lexer->GetProperty(0);
        bool isDark = lexer->IsDark();
        m_styleInfo.resize(kLastStyle);
        m_styleInfo[kDefault] = std::make_tuple(kDefault, defaultStyle.GetFgColour(), defaultStyle.GetBgColour());
        m_styleInfo[kInfo] = std::make_tuple(kInfo, isDark ? wxColour("rgb(167, 226, 46)") : wxColour("rgb(80, 161, 79)"),
                                             defaultStyle.GetBgColour());
        m_styleInfo[kWarning] = std::make_tuple(
            kWarning, isDark ? wxColour("rgb(150,155,73)") : wxColour("rgb(255,201,14)"), defaultStyle.GetBgColour());
        m_styleInfo[kError] =
            std::make_tuple(kError, isDark ? wxColour("rgb(255,128,128)") : *wxRED, defaultStyle.GetBgColour());
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // AuiCaptionEnabler
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    AuiCaptionEnabler::AuiCaptionEnabler()
        : m_aui(NULL)
        , m_window(NULL)
        , m_timer(NULL)
        , m_captionAdded(false)
        , m_ensureCaptionsVisible(false)
    {
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &AuiCaptionEnabler::OnEditorSettingsChanged, this);
    }
    //---------------------------------------------------------------------------------------
    AuiCaptionEnabler::~AuiCaptionEnabler()
    {
        gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &AuiCaptionEnabler::OnEditorSettingsChanged, this);
        if(m_timer) {
            m_timer->Stop();
            Unbind(wxEVT_TIMER, &AuiCaptionEnabler::OnTimer, this, m_timer->GetId());
        }
        wxDELETE(m_timer);
    }
    //---------------------------------------------------------------------------------------
    void AuiCaptionEnabler::Initialize(wxWindow* window, const wxString& windowCaption, wxAuiManager* aui)
    {
    #ifdef __WXMSW__
        m_ensureCaptionsVisible = gsgs_EditorConfig().GetOptions()->IsEnsureCaptionsVisible();
        m_window = window;
        m_aui = aui;
        m_caption = windowCaption;

        CHECK_PTR_RET(m_window);
        CHECK_PTR_RET(m_aui);
        CHECK_PTR_RET(!m_caption.IsEmpty());

        m_timer = new wxTimer(this);
        m_timer->Start(100);
        Bind(wxEVT_TIMER, &AuiCaptionEnabler::OnTimer, this, m_timer->GetId());
    #endif
    }
    //---------------------------------------------------------------------------------------
    void AuiCaptionEnabler::OnTimer(wxTimerEvent& event)
    {
        if(m_ensureCaptionsVisible) {
            // Do the logic here
            // mousePt in screen coords
            wxPoint mousePt = ::wxGetMousePosition();
            wxAuiPaneInfo& pi = m_aui->GetPane(m_caption);
            if(pi.IsOk() && !pi.IsFloating() && !pi.HasCaption()) {
                // rect in screen coords
                wxRect rect = m_window->GetScreenRect();
                // No caption, make it visible
                if(rect.Contains(mousePt)) {
                    pi.CaptionVisible();
                    m_aui->Update();
                    m_captionAdded = true;
                }
            } else if(m_captionAdded && pi.IsOk() && !pi.IsFloating() && pi.HasCaption()) {
                wxRect rect = m_window->GetScreenRect();
                rect.Inflate(30); // Incase the mouse is at the top of the window
                // Remove the caption if needed
                if(!rect.Contains(mousePt)) {
                    m_captionAdded = false;
                    pi.CaptionVisible(false);
                    m_aui->Update();
                }
            }        
        }
    }
    //---------------------------------------------------------------------------------------
    void AuiCaptionEnabler::OnEditorSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
    #ifdef __WXMSW__
        m_ensureCaptionsVisible = gsgs_EditorConfig().GetOptions()->IsEnsureCaptionsVisible();
    #endif
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // FindResultsStyler
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    FindResultsStyler::FindResultsStyler()
        : m_stc(NULL)
        , m_curstate(kStartOfLine)
    {
    }
    //---------------------------------------------------------------------------------------
    FindResultsStyler::FindResultsStyler(wxStyledTextCtrl* stc)
        : m_stc(stc)
        , m_curstate(kStartOfLine)
    {
        SetStyles(m_stc);
        m_stc->SetLexer(wxSTC_LEX_CONTAINER);
        m_stc->Bind(wxEVT_STC_STYLENEEDED, &FindResultsStyler::OnStyleNeeded, this);
    }
    //---------------------------------------------------------------------------------------
    FindResultsStyler::~FindResultsStyler()
    {
        if(m_stc) { m_stc->Unbind(wxEVT_STC_STYLENEEDED, &FindResultsStyler::OnStyleNeeded, this); }
    }
    //---------------------------------------------------------------------------------------
    void FindResultsStyler::SetStyles(wxStyledTextCtrl* sci)
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("c++");
        if(!lexer) { lexer = gsgs_Style().GetLexer("text"); }

        const StyleProperty& defaultStyle = lexer->GetProperty(0);
        wxFont defaultFont = lexer->GetFontForSyle(0);

        for(size_t i = 0; i < wxSTC_STYLE_MAX; ++i) {
            sci->StyleSetForeground(i, defaultStyle.GetFgColour());
            sci->StyleSetBackground(i, defaultStyle.GetBgColour());
            sci->StyleSetFont(i, defaultFont);
        }

        // Show/hide whitespace
        sci->SetViewWhiteSpace(gsgs_EditorConfig().GetOptions()->GetShowWhitspaces());
        StyleProperty::Map_t& props = lexer->GetLexerProperties();
        // Set the whitespace colours
        sci->SetWhitespaceForeground(true, props[WHITE_SPACE_ATTR_ID].GetFgColour());

        sci->StyleSetForeground(LEX_FIF_HEADER, props[11].GetFgColour());
        sci->StyleSetBackground(LEX_FIF_HEADER, props[11].GetBgColour());

        // 33 is the style for line numbers
        sci->StyleSetForeground(LEX_FIF_LINE_NUMBER, props[33].GetFgColour());

        // 11 is the style number for "identifier"
        sci->StyleSetForeground(LEX_FIF_MATCH, props[11].GetFgColour());

        // 16 is the stule for colouring classes
        sci->StyleSetForeground(LEX_FIF_SCOPE, props[16].GetFgColour());

        sci->StyleSetForeground(LEX_FIF_MATCH_COMMENT, props[wxSTC_C_COMMENTLINE].GetFgColour());

        sci->StyleSetForeground(LEX_FIF_FILE, props[wxSTC_C_WORD].GetFgColour());
        sci->StyleSetEOLFilled(LEX_FIF_FILE, true);

        sci->StyleSetForeground(LEX_FIF_DEFAULT, props[11].GetFgColour());
        sci->StyleSetBackground(LEX_FIF_DEFAULT, props[11].GetBgColour());

        sci->StyleSetHotSpot(LEX_FIF_MATCH, true);
        sci->StyleSetHotSpot(LEX_FIF_FILE, true);
        sci->StyleSetHotSpot(LEX_FIF_MATCH_COMMENT, true);

        sci->SetHotspotActiveForeground(true, lexer->IsDark() ? "WHITE" : "BLACK");
        sci->SetHotspotActiveUnderline(false);
        sci->MarkerDefine(7, wxSTC_MARK_ARROW);

    #if wxVERSION_NUMBER < 3100
        // On GTK we dont have the wxSTC_INDIC_TEXTFORE symbol yet (old wx version)
        sci->MarkerDefine(7, wxSTC_MARK_ARROW);
        sci->MarkerSetBackground(7, lexer->IsDark() ? "CYAN" : "ORANGE");
        sci->MarkerSetForeground(7, lexer->IsDark() ? "CYAN" : "ORANGE");

        sci->IndicatorSetForeground(1, lexer->IsDark() ? "CYAN" : "ORANGE");
        sci->IndicatorSetStyle(1, wxSTC_INDIC_ROUNDBOX);
    #else
        sci->MarkerDefine(7, wxSTC_MARK_BACKGROUND);
        sci->MarkerSetBackground(7, lexer->IsDark() ? *wxWHITE : *wxGREEN);
        sci->MarkerSetForeground(7, lexer->IsDark() ? *wxWHITE : *wxGREEN);
        sci->MarkerSetAlpha(7, 40);

        sci->IndicatorSetForeground(1, lexer->IsDark() ? "#FFD700" : "#FF4500");
        sci->IndicatorSetStyle(1, wxSTC_INDIC_TEXTFORE);
    #endif
        sci->IndicatorSetUnder(1, true);

        sci->SetMarginWidth(0, 0);
        sci->SetMarginWidth(1, 16);
        sci->SetMarginWidth(2, 0);
        sci->SetMarginWidth(3, 0);
        sci->SetMarginWidth(4, 0);
        sci->SetMarginSensitive(1, true);
        sci->HideSelection(true);

        // Indentation
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        sci->SetUseTabs(options->GetIndentUsesTabs());
        sci->SetTabWidth(options->GetIndentWidth());
        sci->SetIndent(options->GetIndentWidth());
        sci->Refresh();
    }
    //---------------------------------------------------------------------------------------
    void FindResultsStyler::StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope)
    {
        int startPos = ctrl->GetEndStyled();
        int endPos = e.GetPosition();
        wxString text = ctrl->GetTextRange(startPos, endPos);
    #if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
        // The scintilla syntax in wx3.1.1 changed
        ctrl->StartStyling(startPos);
    #else
        ctrl->StartStyling(startPos, 0x1f);
    #endif

        wxString::const_iterator iter = text.begin();
        size_t headerStyleLen = 0;
        size_t filenameStyleLen = 0;
        size_t lineNumberStyleLen = 0;
        size_t scopeStyleLen = 0;
        size_t matchStyleLen = 0;
        size_t i = 0;
        for(; iter != text.end(); ++iter) {
            bool advance2Pos = false;
            const wxUniChar& ch = *iter;
            if((long)ch >= 128) { advance2Pos = true; }

            switch(m_curstate) {
            default:
                break;
            case kStartOfLine:
                if(ch == '=') {
                    m_curstate = kHeader;
                    headerStyleLen = 1;
                } else if(ch == ' ') {
                    // start of a line number
                    lineNumberStyleLen = 1;
                    m_curstate = kLineNumber;
                } else if(ch == '\n') {
                    ctrl->SetStyling(1, LEX_FIF_DEFAULT);
                } else {
                    // File name
                    filenameStyleLen = 1;
                    m_curstate = kFile;
                }
                break;
            case kLineNumber:
                ++lineNumberStyleLen;
                if(ch == ':') {
                    ctrl->SetStyling(lineNumberStyleLen, LEX_FIF_LINE_NUMBER);
                    lineNumberStyleLen = 0;
                    if(hasSope) {
                        // the scope showed by displayed after the line number
                        m_curstate = kScope;
                    } else {
                        // No scope, from hereon, match until EOF
                        m_curstate = kMatch;
                    }
                }
                break;
            case kScope:
                ++scopeStyleLen;

                if(ch == ']') {
                    // end of scope
                    ctrl->SetStyling(scopeStyleLen, LEX_FIF_SCOPE);
                    scopeStyleLen = 0;
                    m_curstate = kMatch;
                }
                break;
            case kMatch:
                ++matchStyleLen;
                if(advance2Pos) { ++matchStyleLen; }
                if(ch == '\n') {
                    m_curstate = kStartOfLine;
                    ctrl->SetStyling(matchStyleLen, LEX_FIF_MATCH);
                    matchStyleLen = 0;
                }
                break;
            case kFile:
                ++filenameStyleLen;
                if(ch == '\n') {
                    m_curstate = kStartOfLine;
                    ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 2 | wxSTC_FOLDLEVELHEADERFLAG);
                    ctrl->SetStyling(filenameStyleLen, LEX_FIF_FILE);
                    filenameStyleLen = 0;
                }
                break;
            case kHeader:
                ++headerStyleLen;
                if(ch == '\n') {
                    m_curstate = kStartOfLine;
                    ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 1 | wxSTC_FOLDLEVELHEADERFLAG);
                    ctrl->SetStyling(headerStyleLen, LEX_FIF_HEADER);
                    headerStyleLen = 0;
                }
                break;
            }
            if(advance2Pos) {
                i += 2;
            } else {
                ++i;
            }
        }

        // Left overs...
        if(headerStyleLen) {
            ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 1 | wxSTC_FOLDLEVELHEADERFLAG);
            ctrl->SetStyling(headerStyleLen, LEX_FIF_HEADER);
            headerStyleLen = 0;
        }

        if(filenameStyleLen) {
            ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 2 | wxSTC_FOLDLEVELHEADERFLAG);
            ctrl->SetStyling(filenameStyleLen, LEX_FIF_FILE);
            filenameStyleLen = 0;
        }

        if(matchStyleLen) {
            ctrl->SetStyling(matchStyleLen, LEX_FIF_MATCH);
            matchStyleLen = 0;
        }

        if(lineNumberStyleLen) {
            ctrl->SetStyling(lineNumberStyleLen, LEX_FIF_LINE_NUMBER);
            lineNumberStyleLen = 0;
        }
    }
    //---------------------------------------------------------------------------------------
    void FindResultsStyler::Reset() { m_curstate = kStartOfLine; }
    //---------------------------------------------------------------------------------------
    int FindResultsStyler::HitTest(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, int& line)
    {
        long pos = e.GetPosition();
        line = ctrl->LineFromPosition(pos);
        return ctrl->GetStyleAt(pos);
    }
    //---------------------------------------------------------------------------------------
    int FindResultsStyler::TestToggle(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e)
    {
        int line = wxNOT_FOUND;
        int style = HitTest(ctrl, e, line);
        if(style == LEX_FIF_FILE || style == LEX_FIF_HEADER) {
            return line;
        } else {
            return wxNOT_FOUND;
        }
    }
    //---------------------------------------------------------------------------------------
    void FindResultsStyler::OnStyleNeeded(wxStyledTextEvent& e)
    {
        e.Skip();
        StyleText(m_stc, e, false);
    }

    int FindResultsStyler::HitTest(wxStyledTextEvent& e, int& line) { return HitTest(m_stc, e, line); }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // WorkspaceView
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    WorkspaceView::WorkspaceView(wxSimplebook* book)
        : m_simpleBook(book)
        , m_defaultPage(_("Default"))
    {
        m_simpleBook->SetBackgroundStyle(wxBG_STYLE_PAINT);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &WorkspaceView::OnWorkspaceClosed, this);
    }
    //---------------------------------------------------------------------------------------
    WorkspaceView::~WorkspaceView()
    {
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &WorkspaceView::OnWorkspaceClosed, this);
    }
    //---------------------------------------------------------------------------------------
    size_t WorkspaceView::GetPageIndex(const wxString& name) const
    {
        for(size_t i = 0; i < m_simpleBook->GetPageCount(); ++i) {
            if(m_simpleBook->GetPageText(i) == name) {
                return i;
            }
        }
        return wxString::npos;
    }
    //---------------------------------------------------------------------------------------
    wxWindow* WorkspaceView::GetPage(const wxString& name) const
    {
        size_t index = GetPageIndex(name);
        if(index != wxString::npos) {
            return m_simpleBook->GetPage(index);
        } else if(m_windows.count(name)) {
            return m_windows.find(name)->second;
        }
        return NULL;
    }
    //---------------------------------------------------------------------------------------
    void WorkspaceView::AddPage(wxWindow* page, const wxString& name, bool book)
    {
        if(book) {
            m_simpleBook->AddPage(page, name);
        } else {
            m_windows.insert(std::make_pair(name, page));
        }
    }
    //---------------------------------------------------------------------------------------
    void WorkspaceView::SelectPage(const wxString& name)
    {
        size_t index = GetPageIndex(name);
        if(index != wxString::npos) {
            m_simpleBook->ChangeSelection(index);
        }
    }
    //---------------------------------------------------------------------------------------
    void WorkspaceView::RemovePage(const wxString& name)
    {
        size_t index = GetPageIndex(name);
        if(index != wxString::npos) {
            m_simpleBook->RemovePage(index);
        } else {
            // try to locate it in the internal map
            if(m_windows.count(name)) {
                m_windows.erase(name);
            }
        }
    }
    //---------------------------------------------------------------------------------------
    void WorkspaceView::OnWorkspaceClosed(wxCommandEvent& event)
    {
        event.Skip();
        SelectPage(GetDefaultPage());
    }
    //---------------------------------------------------------------------------------------
    void WorkspaceView::GetAllPages(std::map<wxString, wxWindow*> & outlist) const
    {
        for(size_t i = 0; i < m_simpleBook->GetPageCount(); ++i) {
            if(m_simpleBook->GetPageText(i) == _("Default")) continue;
            outlist.insert(std::make_pair(m_simpleBook->GetPageText(i), m_simpleBook->GetPage(i)));
        }
        outlist.insert(m_windows.begin(), m_windows.end());
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // ProfileHandler
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    ProfileHandler::ProfileHandler()
    {
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &ProfileHandler::OnWorkspaceClosed, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_LOADED, &ProfileHandler::OnWorkspaceLoaded, this);
        gsgs_Event().Bind(wxEVT_GOING_DOWN, &ProfileHandler::OnGoingDown, this);

        m_cxxOutputTabs = { "Clang",    "Build",    "References", "UnitTest++", "Trace",
                            "CppCheck", "MemCheck", "CScope",     "BuildQ" };
        m_cxxWorkspaceTabs = { "CMake Help", "wxCrafter", "SFTP" };
    }
    //---------------------------------------------------------------------------------------
    ProfileHandler::~ProfileHandler()
    {
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &ProfileHandler::OnWorkspaceClosed, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_LOADED, &ProfileHandler::OnWorkspaceLoaded, this);
        gsgs_Event().Unbind(wxEVT_GOING_DOWN, &ProfileHandler::OnGoingDown, this);
    }
    //---------------------------------------------------------------------------------------
    void ProfileHandler::OnWorkspaceClosed(wxCommandEvent& e)
    {
        e.Skip();
        RestoreTabs(m_cxxOutputTabsToRestore, wxEVT_SHOW_OUTPUT_TAB);
        RestoreTabs(m_cxxWorkspaceTabsToRestore, wxEVT_SHOW_WORKSPACE_TAB);
    }
    //---------------------------------------------------------------------------------------
    void ProfileHandler::OnWorkspaceLoaded(wxCommandEvent& e)
    {
        e.Skip();
        if(gsgs_CppWorker().IsOpen()) {
            // we just opened a C++ workspace, restore all C++ related tabs
            HandleOutputTabs(true);
            HandleWorkspaceTabs(true);
        } else {
            // Hide any C++ related tabs
            HandleOutputTabs(false);
            HandleWorkspaceTabs(false);
        }
    }
    //---------------------------------------------------------------------------------------
    ProfileHandler& ProfileHandler::Get()
    {
        static ProfileHandler handler;
        return handler;
    }
    //---------------------------------------------------------------------------------------
    void ProfileHandler::HandleWorkspaceTabs(bool show)
    {
        if(show) {
            RestoreTabs(m_cxxWorkspaceTabsToRestore, wxEVT_SHOW_WORKSPACE_TAB);
        } else {
            HideTabs(m_cxxWorkspaceTabs, GetManager()->GetWorkspacePaneNotebook(), wxEVT_SHOW_WORKSPACE_TAB,
                     m_cxxWorkspaceTabsToRestore);
        }
    }
    //---------------------------------------------------------------------------------------
    void ProfileHandler::HandleOutputTabs(bool show)
    {
        if(show) {
            RestoreTabs(m_cxxOutputTabsToRestore, wxEVT_SHOW_OUTPUT_TAB);
        } else {
            HideTabs(m_cxxOutputTabs, GetManager()->GetOutputPaneNotebook(), wxEVT_SHOW_OUTPUT_TAB,
                     m_cxxOutputTabsToRestore);
        }
    }
    //---------------------------------------------------------------------------------------
    bool ProfileHandler::IsPageExistsInBook(Notebook* book, const wxString& label) const
    {
        for(size_t i = 0; i < book->GetPageCount(); ++i) {
            if(book->GetPageText(i) == label) {
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------------------
    void ProfileHandler::RestoreTabs(gsgsStringSet& tabs, wxEventType eventType)
    {
        std::for_each(tabs.begin(), tabs.end(), [&](const wxString& tab) {
            Event eventShow(eventType);
            eventShow.SetSelected(true).SetString(tab);
            gsgs_Event().AddPendingEvent(eventShow);
        });
        tabs.clear();
    }
    //---------------------------------------------------------------------------------------
    void ProfileHandler::HideTabs(const gsgsStringSet & candidates, Notebook * book, wxEventType eventType, gsgsStringSet & tabsHidden)
    {
        tabsHidden.clear();
        std::for_each(candidates.begin(), candidates.end(), [&](const wxString& tab) {
            if(IsPageExistsInBook(book, tab)) {
                tabsHidden.insert(tab);
                Event eventHide(eventType);
                eventHide.SetSelected(false).SetString(tab);
                gsgs_Event().AddPendingEvent(eventHide);
            }
        });
    }
    //---------------------------------------------------------------------------------------
    void ProfileHandler::OnGoingDown(Event& e)
    {
        e.Skip();
        RestoreTabs(m_cxxOutputTabsToRestore, wxEVT_SHOW_OUTPUT_TAB);
        RestoreTabs(m_cxxWorkspaceTabsToRestore, wxEVT_SHOW_WORKSPACE_TAB);
    }

    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // JSCTags
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    class clJSCTagsZipJob : public Job
    {
    protected:
        JSCTags* m_jsctags;
        wxString m_zipfile;
        wxString m_targetFolder;

    public:
        clJSCTagsZipJob(const wxString& zipfile, const wxString& targetFolder)
            : Job()
            , m_zipfile(zipfile)
            , m_targetFolder(targetFolder)
        {
        }

        virtual ~clJSCTagsZipJob() {}

        void Process(wxThread* thread)
        {
            wxUnusedVar(thread);
            // Extract the zip file
            wxFileName::Mkdir(m_targetFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            ZipReader zipReader(m_zipfile);
            gsgsDEBUG() << "Extracting zip file:" << m_zipfile << clEndl;
            zipReader.Extract("*", m_targetFolder);
            gsgsDEBUG() << "Extracting zip file:" << m_zipfile << "...done" << clEndl;
        }
    };
    //---------------------------------------------------------------------------------------
    JSCTags::JSCTags()
        : m_zipExtracted(false)
    {
        gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &JSCTags::OnEditorChanged, this);
        gsgs_Event().Bind(wxEVT_FILE_SAVED, &JSCTags::OnEditorSaved, this);
        gsgs_Event().Bind(wxEVT_INIT_DONE, &JSCTags::OnInitDone, this);
        gsgs_Event().Bind(wxEVT_EDITOR_CLOSING, &JSCTags::OnEditorClosing, this);
    }
    //---------------------------------------------------------------------------------------
    JSCTags::~JSCTags()
    {
        gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &JSCTags::OnEditorChanged, this);
        gsgs_Event().Unbind(wxEVT_FILE_SAVED, &JSCTags::OnEditorSaved, this);
        gsgs_Event().Unbind(wxEVT_INIT_DONE, &JSCTags::OnInitDone, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_CLOSING, &JSCTags::OnEditorClosing, this);
    }
    //---------------------------------------------------------------------------------------
    void JSCTags::OnEditorSaved(Event& event)
    {
        event.Skip();
        if(!m_zipExtracted) return;

        wxString filename = event.GetFileName();
    }
    //---------------------------------------------------------------------------------------
    void JSCTags::OnEditorChanged(wxCommandEvent& event)
    {
        event.Skip();
        if(!m_zipExtracted) return;

        IEditor* editor = GetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);
    }
    //---------------------------------------------------------------------------------------
    void JSCTags::ZipExtractCompleted() { m_zipExtracted = true; }
    //---------------------------------------------------------------------------------------
    void JSCTags::OnInitDone(wxCommandEvent& event)
    {
        event.Skip();
        // Extract the zip file
        wxFileName jsctagsZip(gsgs_Path().GetDataDir(), "jsctags.zip");
        if(!jsctagsZip.Exists()) return;
        wxFileName targetDir(gsgs_Path().GetUserDataDir(), "");
        targetDir.AppendDir("webtools");
        targetDir.AppendDir("jsctags");
    #if 0
    #ifndef __WXGTK__
        JobQueueSingleton::Instance()->PushJob(new clJSCTagsZipJob(jsctagsZip.GetFullPath(), targetDir.GetPath()));
    #else
        clJSCTagsZipJob job(jsctagsZip.GetFullPath(), targetDir.GetPath());
        job.Process(NULL);
    #endif
    #endif
        ZipExtractCompleted();
    }
    //---------------------------------------------------------------------------------------
    void JSCTags::OnEditorClosing(wxCommandEvent& e)
    {
        e.Skip();
        IEditor* editor = (IEditor*)e.GetClientData();
        CHECK_PTR_RET(editor);

        wxString closingpath = editor->GetFileName().GetFullPath();
        // Clear this file's cache
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // TabTogglerHelper
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    TabTogglerHelper::TabTogglerHelper(const wxString& outputTabName, wxWindow* outputTab, const wxString& workspaceTabName, wxWindow* workspaceTab)
        : m_outputTabName(outputTabName)
        , m_outputTab(outputTab)
        , m_workspaceTabName(workspaceTabName)
        , m_workspaceTab(workspaceTab)
    {
        if(m_workspaceTab && !m_workspaceTabName.IsEmpty()) {
            gsgs_Event().Bind(wxEVT_SHOW_WORKSPACE_TAB, &TabTogglerHelper::OnToggleWorkspaceTab, this);
            GetManager()->AddWorkspaceTab(m_workspaceTabName);
        }
        if(m_outputTab && !m_outputTabName.IsEmpty()) {
            gsgs_Event().Bind(wxEVT_SHOW_OUTPUT_TAB, &TabTogglerHelper::OnToggleOutputTab, this);
            GetManager()->AddOutputTab(m_outputTabName);
        }
    }
    //---------------------------------------------------------------------------------------
    TabTogglerHelper::~TabTogglerHelper()
    {
        if(m_workspaceTab && !m_workspaceTabName.IsEmpty()) {
            gsgs_Event().Unbind(wxEVT_SHOW_WORKSPACE_TAB, &TabTogglerHelper::OnToggleWorkspaceTab, this);
        }
        if(m_outputTab && !m_outputTabName.IsEmpty()) {
            gsgs_Event().Unbind(wxEVT_SHOW_OUTPUT_TAB, &TabTogglerHelper::OnToggleOutputTab, this);
        }
    }
    //---------------------------------------------------------------------------------------
    void TabTogglerHelper::OnToggleOutputTab(Event& event)
    {
        if(event.GetString() != m_outputTabName) {
            event.Skip();
            return;
        }

        Notebook* book = GetManager()->GetOutputPaneNotebook();
        if(event.IsSelected()) {
            // show it
            int where = IsTabInNotebook(book, m_outputTabName);
            if(where == wxNOT_FOUND) {
                // Only show it if it does not exists in the notebook
                GetManager()->GetOutputPaneNotebook()->AddPage(m_outputTab, m_outputTabName, true, m_outputTabBmp);
            } else {
                GetManager()->GetOutputPaneNotebook()->SetSelection(where);
            }
        } else {
            int where = GetManager()->GetOutputPaneNotebook()->GetPageIndex(m_outputTabName);
            if(where != wxNOT_FOUND) { GetManager()->GetOutputPaneNotebook()->RemovePage(where); }
        }
    }
    //---------------------------------------------------------------------------------------
    void TabTogglerHelper::OnToggleWorkspaceTab(Event& event)
    {
        if(event.GetString() != m_workspaceTabName) {
            event.Skip();
            return;
        }

        Notebook* book = GetManager()->GetWorkspacePaneNotebook();
        if(event.IsSelected()) {
            // show it
            int where = IsTabInNotebook(book, m_workspaceTabName);
            if(where == wxNOT_FOUND) {
                // Only show it if it does not exists in the notebook
                GetManager()->GetWorkspacePaneNotebook()->AddPage(m_workspaceTab, m_workspaceTabName, true,
                                                                    m_workspaceTabBmp);
            } else {
                // The tab already in the notebook, just select it
                GetManager()->GetWorkspacePaneNotebook()->SetSelection(where);
            }
        } else {
            int where = GetManager()->GetWorkspacePaneNotebook()->GetPageIndex(m_workspaceTabName);
            if(where != wxNOT_FOUND) { GetManager()->GetWorkspacePaneNotebook()->RemovePage(where); }
        }
    }
    //---------------------------------------------------------------------------------------
    int TabTogglerHelper::IsTabInNotebook(Notebook* book, const wxString& tabname)
    {
        for(size_t i = 0; i < book->GetPageCount(); ++i) {
            if(book->GetPageText(i) == tabname) return i;
        }
        return wxNOT_FOUND;
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // CompileCommandsGenerator
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    class _gsgsAPI CompileCommandsJSON
    {
        wxFileName m_filename;
        wxArrayString m_macros;
        wxArrayString m_includes;
        wxArrayString m_others;

    public:
        CompileCommandsJSON(const wxString& filename);
        virtual ~CompileCommandsJSON();
        
        void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
        void SetIncludes(const wxArrayString& includes) { this->m_includes = includes; }
        void SetMacros(const wxArrayString& macros) { this->m_macros = macros; }
        void SetOthers(const wxArrayString& others) { this->m_others = others; }
        const wxFileName& GetFilename() const { return m_filename; }
        const wxArrayString& GetIncludes() const { return m_includes; }
        const wxArrayString& GetMacros() const { return m_macros; }
        const wxArrayString& GetOthers() const { return m_others; }
    };

    CompileCommandsJSON::CompileCommandsJSON(const wxString& filename)
        : m_filename(filename)
    {
        if(m_filename.FileExists()) {
            JSON json(m_filename);
            JSONObject arr = json.getObject();
            const int count = arr.getArraySize();
            for(int i = 0; i < count; ++i) {
                wxString command, workingDirectory;
                arr.getArray(i).getObject("command").getValue(command);
                arr.getArray(i).getObject("directory").getValue(workingDirectory);
                
                // Use the workingDirectory to convert all paths to full path
                CompilerCmdParser cclp(command, workingDirectory);
                m_includes = cclp.GetIncludes();
                m_macros = cclp.GetMacros();
                m_others = cclp.GetOtherOptions();
            }
        }
    }

    CompileCommandsJSON::~CompileCommandsJSON() {}

    class _gsgsAPI CompileFlagsTxt
    {
        wxFileName m_filename;
        wxArrayString m_includes;
        wxArrayString m_macros;
        wxArrayString m_others;

    public:
        CompileFlagsTxt(const wxFileName& filename);
        virtual ~CompileFlagsTxt();
        void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
        void SetIncludes(const wxArrayString& includes) { this->m_includes = includes; }
        void SetMacros(const wxArrayString& macros) { this->m_macros = macros; }
        void SetOthers(const wxArrayString& others) { this->m_others = others; }
        const wxFileName& GetFilename() const { return m_filename; }
        const wxArrayString& GetIncludes() const { return m_includes; }
        const wxArrayString& GetMacros() const { return m_macros; }
        const wxArrayString& GetOthers() const { return m_others; }
    };

    CompileFlagsTxt::CompileFlagsTxt(const wxFileName& filename)
        : m_filename(filename)
    {
        wxString data;
        gsgsStringSet uniqueIncludePaths;
        gsgsStringSet uniqueMacros;
        gsgsStringSet uniqueOthers;

        if(FileUtil::ReadFileContent(m_filename, data)) {
            wxArrayString lines = ::wxStringTokenize(data, "\n", wxTOKEN_STRTOK);
            for(size_t i = 0; i < lines.GetCount(); ++i) {
                wxString& line = lines.Item(i);
                line.Trim().Trim(false);
                if(line.StartsWith("-I")) {
                    line.Remove(0, 2); // remove the "-I"
                    if(uniqueIncludePaths.count(line) == 0) {
                        uniqueIncludePaths.insert(line);
                        m_includes.Add(line);
                    }
                } else if(line.StartsWith("-D")) {
                    line.Remove(0, 2); // remove the "-D"
                    if(uniqueMacros.count(line) == 0) {
                        uniqueMacros.insert(line);
                        m_macros.Add(line);
                    }
                } else {
                    // others
                    if(uniqueOthers.count(line) == 0) {
                        uniqueOthers.insert(line);
                        m_others.Add(line);
                    }
                }
            }
        }
    }

    CompileFlagsTxt::~CompileFlagsTxt() {}

    wxDEFINE_EVENT(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, Event);
    //---------------------------------------------------------------------------------------
    CompileCommandsGenerator::CompileCommandsGenerator()
    {
        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
    }
    //---------------------------------------------------------------------------------------
    CompileCommandsGenerator::~CompileCommandsGenerator()
    {
        // If the child process is still running, detach from it. i.e. OnProcessTeraminated() event is not called
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &CompileCommandsGenerator::OnProcessTeraminated, this);
        if(m_process) { m_process->Detach(); }
        wxDELETE(m_process);
    }
    //---------------------------------------------------------------------------------------
    typedef wxString CheckSum_t;
    static CheckSum_t ComputeFileCheckSum(const wxFileName& fn) { return wxMD5::GetDigest(fn); }
    //---------------------------------------------------------------------------------------
    void CompileCommandsGenerator::OnProcessTeraminated(gsgs::ProcessEvent& event)
    {
        // dont call event.Skip() so we will delete the m_process ourself
        wxDELETE(m_process);
        GetManager()->SetStatusMessage(_("Ready"));

        static std::unordered_map<wxString, CheckSum_t> m_checksumCache;

        // Process the compile_flags.txt files starting from the "compile_commands.json" root folder
        // Notify about completion
        std::thread thr(
            [=](const wxString& compile_commands) {
                // Calculate the new file checksum
                CheckSum_t ck = ComputeFileCheckSum(compile_commands);
                CheckSum_t oldCk;
                gsgsDEBUG() << "New checksum is:" << ck;
                if(m_checksumCache.count(compile_commands)) {
                    oldCk = m_checksumCache.find(compile_commands)->second;
                } else {
                    gsgsDEBUG() << "File:" << compile_commands << "is not found in the cache";
                }
                gsgsDEBUG() << "Old checksum is:" << oldCk;
                if(ck == oldCk) {
                    gsgsDEBUG() << "No changes detected in file:" << compile_commands << "processing is ignored";
                    return;
                }

                // store the new checksum
                m_checksumCache.erase(compile_commands);
                m_checksumCache.insert({ compile_commands, ck });

                FileScan scanner;
                wxArrayString includePaths;
                gsgsStringSet includeSet;
                std::vector<wxString> files;
                if(scanner.Scan(wxFileName(compile_commands).GetPath(), files, "compile_flags.txt")) {
                    for(const wxString& file : files) {
                        CompileFlagsTxt f(file);
                        includePaths = f.GetIncludes();
                        includeSet.insert(includePaths.begin(), includePaths.end());
                    }
                }

                CompileCommandsJSON compileCommands(compile_commands);
                const wxArrayString& paths = compileCommands.GetIncludes();
                for(const wxString& path : paths) {
                    if(includeSet.count(path) == 0) {
                        includeSet.insert(path);
                        includePaths.Add(path);
                    }
                }

                gsgsDEBUG() << "wxEVT_COMPILE_COMMANDS_JSON_GENERATED paths:\n" << includePaths;

                // Notify about it
                Event eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
                eventCompileCommandsGenerated.SetFileName(compile_commands); // compile_commands.json
                eventCompileCommandsGenerated.SetStrings(
                    includePaths); // include paths found and gathered from all the compile_flags.txt files scanned
                gsgs_Event().AddPendingEvent(eventCompileCommandsGenerated);
            },
            m_outputFile.GetFullPath());
        thr.detach();
    }
    //---------------------------------------------------------------------------------------
    void CompileCommandsGenerator::GenerateCompileCommands()
    {
        // Kill any previous process running
        if(m_process) { m_process->Detach(); }
        wxDELETE(m_process);

        wxFileName gsgsMake(gsgs_Path().GetBinFolder(), "gsgs-make");
    #ifdef __WXMSW__
        gsgsMake.SetExt("exe");
    #endif

        if(!gsgs_CppWorker().IsOpen()) { return; }
        if(!gsgs_CppWorker().GetActiveProject()) { return; }

        if(!gsgsMake.FileExists()) {
            gsgsWARNING() << "Could not find" << gsgsMake;
            return;
        }

        wxString command;
        command << gsgsMake.GetFullPath();
        ::WrapWithQuotes(command);

        wxString workspaceFile = gsgs_CppWorker().GetFileName().GetFullPath();
        ::WrapWithQuotes(workspaceFile);

        wxString configName =
            gsgs_CppWorker().GetSelectedConfig() ? gsgs_CppWorker().GetSelectedConfig()->GetName() : "";
        command << " --workspace=" << workspaceFile << " --verbose --json --config=" << configName;

        // since we might be activated with a different settings directory
        // pass the build_settings.xml to gsgs-make

        wxFileName xmlFile(gsgs_Path().GetUserDataDir(), "build_settings.xml");
        xmlFile.AppendDir("config");
        wxString xmlPath = xmlFile.GetFullPath();
        ::WrapWithQuotes(xmlPath);
        command << " --settings=" << xmlPath;

        gsgsDEBUG() << "Executing:" << command;

        EnvSetter env(gsgs_CppWorker().GetActiveProject());
        m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault);

        m_outputFile = workspaceFile;
        m_outputFile.SetFullName("compile_commands.json");

        GetManager()->SetStatusMessage(wxString() << _("Generating ") << m_outputFile.GetFullPath(), 2);
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // TreeKeyboardInput
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    TreeKeyboardInput::TreeKeyboardInput(TreeCtrl* tree)
        : m_tree(tree)
    {
        m_tree->EnableStyle(wxTR_ENABLE_SEARCH, true);
        m_tree->Bind(wxEVT_TREE_SEARCH_TEXT, &TreeKeyboardInput::OnSearch, this);
        m_tree->Bind(wxEVT_TREE_CLEAR_SEARCH, &TreeKeyboardInput::OnClearSearch, this);
    }
    //---------------------------------------------------------------------------------------
    TreeKeyboardInput::~TreeKeyboardInput()
    {
        m_tree->Unbind(wxEVT_TREE_SEARCH_TEXT, &TreeKeyboardInput::OnSearch, this);
        m_tree->Unbind(wxEVT_TREE_CLEAR_SEARCH, &TreeKeyboardInput::OnClearSearch, this);
    }
    //---------------------------------------------------------------------------------------
    void TreeKeyboardInput::OnSearch(wxTreeEvent& event)
    {
        wxString find_what = event.GetString();
        m_tree->ClearAllHighlights();
        wxTreeItemId item = m_tree->FindNext(m_tree->GetFocusedItem(), find_what, 0, wxTR_SEARCH_DEFAULT);
        if(item.IsOk()) {
            m_tree->SelectItem(item);
            m_tree->HighlightText(item, true);
            m_tree->EnsureVisible(item);
        } else {
            m_tree->ClearAllHighlights();
        }
    }
    //---------------------------------------------------------------------------------------
    void TreeKeyboardInput::OnClearSearch(wxTreeEvent& event) { m_tree->ClearAllHighlights(); }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // MainAuiTBArt
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    MainAuiTBArt::MainAuiTBArt(bool isMainBook)
        : m_isMainBook(isMainBook)
    {
        gsgs_Event().Connect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(MainAuiTBArt::OnTheme), NULL,
                                      this);
    }
    //---------------------------------------------------------------------------------------
    MainAuiTBArt::~MainAuiTBArt()
    {
        gsgs_Event().Disconnect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(MainAuiTBArt::OnTheme),
                                         NULL, this);
    }
    //---------------------------------------------------------------------------------------
    void MainAuiTBArt::DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
    {
        wxUnusedVar(wnd);
        wxBitmap bmp(rect.GetSize());
        wxMemoryDC dcMem;
        dcMem.SelectObject(bmp);
        dcMem.SetPen(gsgs_Style().GetPanelBgColour());
        dcMem.SetBrush(gsgs_Style().GetPanelBgColour());
        dcMem.DrawRectangle(rect);
        dcMem.SelectObject(wxNullBitmap);
        dc.DrawBitmap(bmp, wxPoint(0, 0));
    }
    //---------------------------------------------------------------------------------------
    void MainAuiTBArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) { DrawPlainBackground(dc, wnd, rect); }
    //---------------------------------------------------------------------------------------
    void MainAuiTBArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
    {
    #ifdef __WXGTK3__
        if(!item.GetBitmap().IsOk()) { return; }
    #endif
        wxAuiDefaultToolBarArt::DrawButton(dc, wnd, item, rect);
    }
    //---------------------------------------------------------------------------------------
    void MainAuiTBArt::DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
    {
    #ifdef __WXGTK3__
        if(!item.GetBitmap().IsOk()) { return; }
    #endif
        wxAuiDefaultToolBarArt::DrawDropDownButton(dc, wnd, item, rect);
    }
    //---------------------------------------------------------------------------------------
    void MainAuiTBArt::OnTheme(wxCommandEvent& event) { event.Skip(); }
    //---------------------------------------------------------------------------------------
    void MainAuiTBArt::DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect)
    {
        wxUnusedVar(dc);
        wxUnusedVar(wnd);
        wxUnusedVar(rect);
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // BitmapOverlayCtrl
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    BitmapOverlayCtrl::BitmapOverlayCtrl(wxWindow* win, const wxString& bitmapname, size_t flags)
        : m_win(win)
        , m_name(bitmapname)
        , m_flags(flags)
        , m_bmpWindow(NULL)
    {
        m_bmp = gsgs_Image().LoadBitmap(bitmapname);
        DoPosition();
        m_win->Bind(wxEVT_SIZE, &BitmapOverlayCtrl::OnSize, this);
    }

    BitmapOverlayCtrl::~BitmapOverlayCtrl()
    {
        if(m_bmpWindow) {
            m_win->RemoveChild(m_bmpWindow);
            m_bmpWindow->Destroy();
        }
        m_win->Unbind(wxEVT_SIZE, &BitmapOverlayCtrl::OnSize, this);
    }

    void BitmapOverlayCtrl::DoPosition()
    {
        if(m_bmp.IsOk() && m_win) {
            wxRect rect = m_win->GetRect();
            int xx = 0;
            int yy = 0;
            int spacer = 5;
            if(m_flags & wxRIGHT) {
                xx = rect.GetWidth() - m_bmp.GetScaledWidth() - spacer;
            } else {
                xx = spacer;
            }

            if(m_flags & wxCENTER) {
                yy = (rect.GetHeight() - m_bmp.GetScaledHeight()) / 2;
            } else if(m_flags & wxBOTTOM) {
                yy = (rect.GetHeight() - m_bmp.GetScaledHeight());
            } else {
                // Top
                yy = 0;
            }
            if(!m_bmpWindow) {
                m_bmpWindow = new wxStaticBitmap(m_win, wxID_ANY, m_bmp, wxPoint(xx, yy));
            }
            m_bmpWindow->Move(wxPoint(xx, yy));
        }
    }

    void BitmapOverlayCtrl::OnSize(wxSizeEvent& event)
    {
        event.Skip();
        DoPosition();
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // IncludePathLocator
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    static wxArrayString ExecCommand(const wxString& cmd)
    {
        wxArrayString outputArr;
        EnvSetter setter;
        ProcUtils::SafeExecuteCommand(cmd, outputArr);
        return outputArr;
    }

    IncludePathLocator::IncludePathLocator(IManager* mgr)
        : m_mgr(mgr)
    {
    }

    IncludePathLocator::~IncludePathLocator() {}

    void IncludePathLocator::Locate(wxArrayString& paths, wxArrayString& excludePaths, bool thirdPartyLibs,
                                    const wxString& tool)
    {
        // Common compiler paths - should be placed at top of the include path!
        wxString tmpfile1 = wxFileName::CreateTempFileName(wxT("gsgs"));
        wxString command;
        wxString tmpfile = tmpfile1;
        tmpfile += wxT(".cpp");

        wxString bin = tool;
        if(bin.IsEmpty()) { bin = wxT("gcc"); }

        wxRenameFile(tmpfile1, tmpfile);

        // GCC prints parts of its output to stdout and some to stderr
        // redirect all output to stdout
    #if defined(__WXMAC__) || defined(__WXGTK__)
        // Mac does not like the standard command
        command = wxString::Format(wxT("%s -v -x c++ /dev/null -fsyntax-only"), bin.c_str());
    #else
        command = wxString::Format(wxT("%s -v -x c++ %s -fsyntax-only"), bin.c_str(), tmpfile.c_str());
    #endif

        wxString outputStr = wxShellExec(command, wxEmptyString);
        gsgsRemoveFile(tmpfile);

        wxArrayString outputArr = wxStringTokenize(outputStr, wxT("\n\r"), wxTOKEN_STRTOK);
        // Analyze the output
        bool collect(false);
        for(size_t i = 0; i < outputArr.GetCount(); i++) {
            if(outputArr[i].Contains(wxT("#include <...> search starts here:"))) {
                collect = true;
                continue;
            }

            if(outputArr[i].Contains(wxT("End of search list."))) { break; }

            if(collect) {

                wxString file = outputArr.Item(i).Trim().Trim(false);

                // on Mac, (framework directory) appears also,
                // but it is harmless to use it under all OSs
                file.Replace(wxT("(framework directory)"), wxT(""));
                file.Trim().Trim(false);

                wxFileName includePath(file, wxT(""));
                includePath.Normalize();

                paths.Add(includePath.GetPath());
            }
        }

        if(thirdPartyLibs) {
            // try to locate QMAKE
            wxFileConfig qmakeConf(wxEmptyString, wxEmptyString, m_mgr->GetStartupDirectory() + wxT("/config/qmake.ini"));
            wxString groupName;
            long index(0);
            wxArrayString out;
            wxString qmake(wxT("qmake"));

            if(qmakeConf.GetFirstGroup(groupName, index)) {
                // we got qmake configuration, use it instead of the default qmake command
                qmake = qmakeConf.Read(groupName + wxT("/qmake"));
            }

            // Run: qmake -query QT_INSTALL_PREFIX
            wxString cmd;
            cmd << qmake << wxT(" -query QT_INSTALL_PREFIX");

    #ifdef __WXGTK__
            cmd << wxT(" 2>/dev/null");
    #endif

            out = ExecCommand(cmd);

            if(out.IsEmpty() == false) {

                wxString qt_output(out.Item(0));
                qt_output.Trim().Trim(false);

    #if defined(__WXGTK__) || defined(__WXMAC__)
                wxString pathQt4, pathQt3, pathQt;
                pathQt4 << qt_output << gsgsPathSep << wxT("include") << gsgsPathSep
                        << wxT("qt4");
                pathQt3 << qt_output << gsgsPathSep << wxT("include") << gsgsPathSep
                        << wxT("qt3");
                pathQt << qt_output << gsgsPathSep << wxT("include");

                if(wxDir::Exists(pathQt4)) {
                    wxString tmpPath;

                    tmpPath = pathQt4 + wxT("/QtCore");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                    tmpPath = pathQt4 + wxT("/QtGui");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                    tmpPath = pathQt4 + wxT("/QtXml");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                } else if(wxDir::Exists(pathQt3)) {

                    wxString tmpPath;

                    tmpPath = pathQt3 + wxT("/QtCore");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                    tmpPath = pathQt3 + wxT("/QtGui");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                    tmpPath = pathQt3 + wxT("/QtXml");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                } else if(wxDir::Exists(pathQt)) {

                    wxString tmpPath;

                    tmpPath = pathQt + wxT("/QtCore");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);

                    tmpPath = pathQt + wxT("/QtGui");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(pathQt);

                    tmpPath = pathQt + wxT("/QtXml");
                    if(wxFileName::DirExists(tmpPath)) paths.Add(tmpPath);
                }

    #else // __WXMSW__
                wxString pathWin;
                pathWin << qt_output << gsgsPathSep << wxT("include") << gsgsPathSep;
                if(wxDir::Exists(pathWin)) {

                    wxString tmpPath;

                    tmpPath = pathWin + wxT("QtCore");
                    if(wxFileName::DirExists(tmpPath)) {
                        wxFileName fn(tmpPath, wxT(""));
                        paths.Add(fn.GetPath());
                    }

                    tmpPath = pathWin + wxT("QtGui");
                    if(wxFileName::DirExists(tmpPath)) {
                        wxFileName fn(tmpPath, wxT(""));
                        paths.Add(fn.GetPath());
                    }

                    tmpPath = pathWin + wxT("QtXml");
                    if(wxFileName::DirExists(tmpPath)) {
                        wxFileName fn(tmpPath, wxT(""));
                        paths.Add(fn.GetPath());
                    }
                }
    #endif
            }

            // Try wxWidgets
    #ifdef __WXMSW__
            // On Windows, just read the content of the WXWIN environment variable
            wxString wxwin;
            if(wxGetEnv(wxT("WX_INCL_HOME"), &wxwin)) {
                // we got the path to the installation of wxWidgets
                if(wxDir::Exists(wxwin)) {
                    paths.Add(wxwin);
                    excludePaths.Add(wxwin + wxT("\\univ"));
                    excludePaths.Add(wxwin + wxT("\\unix"));
                }
            }
    #else
            // run wx-config and parse the output
            out.Clear();
            out = ExecCommand(wxT("wx-config --cxxflags 2>/dev/null"));
            if(out.IsEmpty() == false) {
                wxString line(out.Item(0));
                int where = line.Find(wxT(" -I"));
                while(where != wxNOT_FOUND) {
                    line = line.Mid(where + 3);
                    paths.Add(line.BeforeFirst(wxT(' ')));
                    where = line.Find(wxT(" -I"));
                }
            }
    #endif
        }
    }
    //---------------------------------------------------------------------------------------
}