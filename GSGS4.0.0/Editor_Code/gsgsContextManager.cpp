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
#include "gsgsContextManager.h"
#include "context_cpp.h"
#include "context_diff.h"
#include "context_html.h"
#include "ContextPhp.h"
#include "gsgsEditorConfig.h"
#include "gsgsStyleManager.h"
#include "ContextJavaScript.h"
#include "gsgsEditor.h"
#include "gsgsLogManager.h"
#include "gsgsEvent.h"
#include "gsgsComment.h"
#include "gsgsTagManager.h"
#include "gsgsFrame.h"
#include "plugin.h"
#include <wx/regex.h>
#include <wx/xrc/xmlres.h>
#include <wx/stc/stc.h>

namespace gsgs
{
    // static wxColor GetInactiveColor(const wxColor& col)
    //{
    //    wxUnusedVar(col);
    //#ifdef __WXGTK__
    //    return wxColor(wxT("GREY"));
    //#else
    //    return wxColor(wxT("LIGHT GREY"));
    //#endif
    //}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ContextBase
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ContextBase::ContextBase(Editor* container)
        : m_container(container)
        , m_name(wxEmptyString)
    {
    }

    ContextBase::ContextBase(const wxString& name)
        : m_name(name)
    {
    }

    ContextBase::~ContextBase() {}

    // provide basic indentation
    void ContextBase::AutoIndent(const wxChar& ch)
    {
        Editor& rCtrl = GetCtrl();
        int prevpos(wxNOT_FOUND);
        int curpos = rCtrl.GetCurrentPos();
        int line = rCtrl.LineFromPosition(curpos);

        if(ch == wxT('\n')) {
            wxChar prevCh = rCtrl.PreviousChar(curpos, prevpos);
            if(prevCh == '{') {
                // an enter was hit just after an open brace
                int prevLine = rCtrl.LineFromPosition(prevpos);
                rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
                rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));

            } else {
                // just copy the previous line indentation
                int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
                rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line - 1));
                // place the caret at the end of the line
                rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
                rCtrl.ChooseCaretX();
            }
        } else if(ch == '}' && !IsCommentOrString(curpos)) {
            long matchPos = wxNOT_FOUND;
            if(!rCtrl.MatchBraceBack(wxT('}'), rCtrl.PositionBefore(curpos), matchPos)) return;
            int secondLine = rCtrl.LineFromPosition(matchPos);
            if(secondLine == line) return;
            rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(secondLine));
        }
    }

    void ContextBase::DoApplySettings(LexerStyle::Ptr_t lexPtr) { lexPtr->Apply(&GetCtrl()); }

    int ContextBase::GetHyperlinkRange(int pos, int& start, int& end)
    {
        Editor& rCtrl = GetCtrl();
        if(!IsCommentOrString(rCtrl.GetCurrentPos())) {
            // get tag as hyperlink
            start = rCtrl.WordStartPos(pos, true);
            end = rCtrl.WordEndPos(pos, true);
            if(start < end) return XRCID("find_tag");
        }
        return wxID_NONE;
    }

    void ContextBase::GoHyperlink(int start, int end, int type, bool alt)
    {
        wxUnusedVar(start);
        wxUnusedVar(end);
        wxUnusedVar(type);
        wxUnusedVar(alt);
    }

    wxMenu* ContextBase::GetMenu() { return wxXmlResource::Get()->LoadMenu(wxT("editor_right_click_default")); }

    void ContextBase::PrependMenuItem(wxMenu* menu, const wxString& text, wxObjectEventFunction func, int eventId)
    {
        wxMenuItem* item;
        wxString menuItemText;
        if(eventId == -1) eventId = wxNewId();

        item = new wxMenuItem(menu, eventId, text);
        menu->Prepend(item);
        menu->Connect(eventId, wxEVT_COMMAND_MENU_SELECTED, func, NULL, this);
        m_dynItems.push_back(item);
    }

    void ContextBase::PrependMenuItem(wxMenu* menu, const wxString& text, int id)
    {
        wxMenuItem* item;
        wxString menuItemText;
        item = new wxMenuItem(menu, id, text);
        menu->Prepend(item);
        m_dynItems.push_back(item);
    }

    void ContextBase::PrependMenuItemSeparator(wxMenu* menu)
    {
        wxMenuItem* item;
        item = new wxMenuItem(menu, wxID_SEPARATOR);
        menu->Prepend(item);
        m_dynItems.push_back(item);
    }

    int ContextBase::DoGetCalltipParamterIndex()
    {
        int index(0);
        Editor& ctrl = GetCtrl();
        int pos = ctrl.DoGetOpenBracePos();
        if(pos != wxNOT_FOUND) {

            // loop over the text from pos -> current position and count the number of commas found
            int depth(0);
            while(pos < ctrl.GetCurrentPos()) {
                wxChar ch = ctrl.SafeGetChar(pos);
                // wxChar ch_before = ctrl.SafeGetChar(ctrl.PositionBefore(pos));

                if(IsCommentOrString(pos)) {
                    pos = ctrl.PositionAfter(pos);
                    continue;
                }

                switch(ch) {
                case wxT(','):
                    if(depth == 0) index++;
                    break;
                case wxT('('):
                    depth++;
                    break;
                case wxT(')'):
                    depth--;
                    break;
                default:
                    break;
                }
                pos = ctrl.PositionAfter(pos);
            }
        } else {
            return wxNOT_FOUND;
        }
        return index;
    }

    void ContextBase::OnUserTypedXChars(const wxString& word)
    {
        // user typed more than X chars
        // trigger code complete event (as if the user typed ctrl-space)
        // if no one handles this event, fire a word completion event
        if(IsCommentOrString(GetCtrl().GetCurrentPos())) { return; }

        const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
        if(options.GetFlags() & CC_WORD_ASSIST) {
            // Try to call code completion
            CodeCompleteEvent ccEvt(wxEVT_CC_CODE_COMPLETE);
            ccEvt.SetEditor(&GetCtrl());
            ccEvt.SetInsideCommentOrString(IsCommentOrString(GetCtrl().GetCurrentPos()));
            ccEvt.SetTriggerKind(LSP::CompletionItem::kTriggerKindInvoked);
            ccEvt.SetPosition(GetCtrl().GetCurrentPos());
            ccEvt.SetWord(word);

            if(!ServiceManager::Get().ProcessEvent(ccEvt)) {
                // This is ugly, since gsgs should not be calling
                // the plugins... we take comfort in the fact that it
                // merely fires an event and not calling it directly
                wxCommandEvent wordCompleteEvent(wxEVT_MENU, XRCID("simple_word_completion"));
                gsgs_Event().TopFrame()->GetEventHandler()->ProcessEvent(wordCompleteEvent);
            }
        }
    }

    void ContextBase::AutoAddComment()
    {
        Editor& rCtrl = GetCtrl();

        CommentConfigData data;
        gsgs_EditorConfig().ReadObject(wxT("CommentConfigData"), &data);

        int curpos = rCtrl.GetCurrentPos();
        int line = rCtrl.LineFromPosition(curpos);
        int cur_style = rCtrl.GetStyleAt(curpos);
        wxString text = rCtrl.GetLine(line - 1).Trim(false);

        bool dontadd = false;
        if(IsAtLineComment()) {
            dontadd = !text.StartsWith(wxT("//")) || !data.GetContinueCppComment();
        } else if(IsAtBlockComment()) {
            dontadd = !data.GetAddStarOnCComment();

        } else {
            dontadd = true;
        }

        if(dontadd) {
            ContextBase::AutoIndent(wxT('\n'));
            return;
        }

        wxString toInsert;
        if(IsAtLineComment()) {
            if(text.StartsWith(wxT("//"))) { toInsert = wxT("// "); }
        } else if(IsAtBlockComment()) {
            // Check the text typed before this char
            int startPos = rCtrl.PositionBefore(curpos);
            startPos -= 3; // for "/**"
            if(startPos >= 0) {
                wxString textTyped = rCtrl.GetTextRange(startPos, rCtrl.PositionBefore(curpos));
                if(((textTyped == "/**") || (textTyped == "/*!")) && data.IsAutoInsert()) {
                    // Let the plugins/gsgs check if they can provide a doxy comment
                    // for the current entry
                    CodeCompleteEvent event(wxEVT_CC_GENERATE_DOXY_BLOCK);
                    event.SetEditor(&rCtrl);
                    if(gsgs_Event().ProcessEvent(event) && !event.GetTooltip().IsEmpty()) {
                        rCtrl.BeginUndoAction();

                        // To make the doxy block fit in, we need to prepend each line
                        // with the exact whitespace of the line that starts with "/**"
                        int lineStartPos = rCtrl.PositionFromLine(rCtrl.LineFromPos(startPos));
                        wxString whitespace = rCtrl.GetTextRange(lineStartPos, startPos);
                        // Break the comment, for each line, prepend the 'whitespace' buffer
                        wxArrayString lines = ::wxStringTokenize(event.GetTooltip(), "\n", wxTOKEN_STRTOK);
                        for(size_t i = 0; i < lines.GetCount(); ++i) {
                            if(i) { // don't add it to the first line (it already exists in the editor)
                                lines.Item(i).Prepend(whitespace);
                            }
                        }

                        // Join the lines back
                        wxString doxyBlock = ::JoinLinesWithEOL(lines, rCtrl.GetEOL());

                        rCtrl.SetSelection(startPos, curpos);
                        rCtrl.ReplaceSelection(doxyBlock);

                        // Try to place the caret after the @brief
                        wxRegEx reBrief("[@\\]brief[ \t]*");
                        if(reBrief.IsValid() && reBrief.Matches(doxyBlock)) {
                            wxString match = reBrief.GetMatch(doxyBlock);
                            // Get the index
                            int where = doxyBlock.Find(match);
                            if(where != wxNOT_FOUND) {
                                where += match.length();
                                int caretPos = startPos + where;
                                rCtrl.SetCaretAt(caretPos);

                                // Remove the @brief as its non standard in the PHP world
                                rCtrl.DeleteRange(caretPos - match.length(), match.length());
                            }
                        }
                        rCtrl.EndUndoAction();
                        return;
                    }
                }
            }

            if(rCtrl.GetStyleAt(rCtrl.PositionBefore(rCtrl.PositionBefore(curpos))) == cur_style) {
                toInsert = rCtrl.GetCharAt(rCtrl.GetLineIndentPosition(line - 1)) == wxT('*') ? wxT("* ") : wxT(" * ");
            }
        }

        if(!toInsert.IsEmpty()) {
            rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line - 1));
            int insertPos = rCtrl.GetLineIndentPosition(line);
            rCtrl.InsertText(insertPos, toInsert);
            rCtrl.SetCaretAt(insertPos + toInsert.Length());
            rCtrl.ChooseCaretX(); // set new column as "current" column
        }
    }

    bool ContextBase::IsStringTriggerCodeComplete(const wxString& str) const
    {
        // default behavior is to check if 'str' exists in the m_completionTriggerStrings container
        if(GetCtrl().GetLexer() == wxSTC_LEX_XML) {
            return str == "<" || str == "</";
        } else if(GetCtrl().GetLexer() == wxSTC_LEX_CSS) {
            return str == ":";
        } else {
            return (m_completionTriggerStrings.count(str) > 0);
        }
    }

    int ContextBase::FindNext(const wxString& what, int& pos, bool wholePage)
    {
        wxStyledTextCtrl* ctrl = GetCtrl().GetCtrl();
        int startpos(wxNOT_FOUND);
        int lastLine(wxNOT_FOUND);
        int endpos(wxNOT_FOUND);
        if(wholePage) {
            startpos = ctrl->GetCurrentPos();
            endpos = ctrl->GetLastPosition();
        } else {
            startpos = ctrl->PositionFromLine(ctrl->GetFirstVisibleLine());
            lastLine = ctrl->GetFirstVisibleLine() + ctrl->LinesOnScreen();
            endpos = ctrl->GetLineEndPosition(lastLine);
        }
        if((pos < startpos) || (pos > endpos)) return wxNOT_FOUND;
        int where = ctrl->FindText(pos, endpos, what);
        if(where != wxNOT_FOUND) { pos = where + what.length(); }
        return where;
    }

    int ContextBase::FindPrev(const wxString& what, int& pos, bool wholePage)
    {
        wxStyledTextCtrl* ctrl = GetCtrl().GetCtrl();
        int startpos(wxNOT_FOUND);
        int lastLine(wxNOT_FOUND);
        int endpos(wxNOT_FOUND);
        if(wholePage) {
            startpos = 0;
            endpos = ctrl->GetCurrentPos();
        } else {
            startpos = ctrl->PositionFromLine(ctrl->GetFirstVisibleLine());
            lastLine = ctrl->GetFirstVisibleLine() + ctrl->LinesOnScreen();
            endpos = ctrl->GetLineEndPosition(lastLine);
        }
        if((pos < startpos) || (pos > endpos)) return wxNOT_FOUND;
        int where = ctrl->FindText(pos, startpos, what);
        if(where != wxNOT_FOUND) { pos = where; }
        return where;
    }

    void ContextBase::BlockCommentComplete()
    {
        wxStyledTextCtrl* stc = GetCtrl().GetCtrl();
        int curPos = stc->GetCurrentPos();
        int start = stc->WordStartPosition(stc->GetCurrentPos(), true);
        if(curPos < start) return;

        // Fire an event indicating user typed '@' in a block comment
        CodeCompleteEvent ccEvent(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE);
        ccEvent.SetEditor(&GetCtrl());
        ccEvent.SetEventObject(&GetCtrl());
        ccEvent.SetWord(stc->GetTextRange(start, curPos));
        gsgs_Event().ProcessEvent(ccEvent);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ContextManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class clEditorXmlHelper
    {
        wxStyledTextCtrl* m_stc;
        wxString m_wordChars;
        std::unordered_set<int> m_chars;

    protected:
        int GetCharAt(int pos) const
        {
            if(pos < 0 || pos > m_stc->GetLastPosition()) 
                return 0;
            return m_stc->GetCharAt(pos);
        }
    public:
        clEditorXmlHelper(wxStyledTextCtrl* stc)
            : m_stc(stc)
        {
            static const wxString wordChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-";
            std::for_each(wordChars.begin(), wordChars.end(), [&](const wxChar& ch) { m_chars.insert((int)ch); });
        }
        ~clEditorXmlHelper(){}

        /**
         * @brief return the word from given pos
         * This function uses the 'm_wordChars' to decided on teh word boundaries
         */
        wxString GetXmlTagAt(int pos, int& startPos, int& endPos) const
        {
            startPos = wxNOT_FOUND;
            endPos = wxNOT_FOUND;
            if(pos > m_stc->GetLastPosition() || pos < 0) return "";

            // going backward
            startPos = pos;
            int tmppos = startPos - 1;
            while(tmppos >= 0) {
                char ch = GetCharAt(tmppos);
                if(m_chars.count((int)ch)) {
                    startPos = tmppos;
                    --tmppos;
                    continue;
                }
                break;
            }

            if((GetCharAt(startPos - 1) == (int)'/') && (GetCharAt(startPos - 2) == (int)'<')) {
                // a closing XML tag
                startPos -= 2;
            } else if(GetCharAt(startPos - 1) == (int)'<') {
                // an opening tag
                startPos -= 1;
            } else {
                // Not an XML tag
                return "";
            }

            endPos = pos;
            int lastPos = m_stc->GetLastPosition();
            while(endPos < lastPos) {
                char ch = GetCharAt(endPos);
                if(m_chars.count((int)ch)) {
                    ++endPos;
                    continue;
                }
                break;
            }

            // Did we get anything?
            if((GetCharAt(endPos) == (int)'>')) {
                // a closing XML tag
                endPos += 1;
            } else if((GetCharAt(endPos) != (int)' ') && (GetCharAt(endPos) != (int)'\t')) {
                // a tag can end in only 2 ways:
                // with '>' or with space | tab, this tag does not end with either
                return "";
            }
            return m_stc->GetTextRange(startPos, endPos);
        }
    };

    class clEditorColouriseLocker
    {
        wxStyledTextCtrl* m_stc;
        int m_firstPos;
        int m_lastPos;

    public:
        clEditorColouriseLocker(wxStyledTextCtrl* stc)
            : m_stc(stc)
            , m_firstPos(0)
            , m_lastPos(0)
        {
            m_firstPos = m_stc->PositionFromLine(m_stc->GetFirstVisibleLine());
            int lastLine = m_stc->GetFirstVisibleLine() + m_stc->LinesOnScreen();
            m_lastPos = m_stc->GetLineEndPosition(lastLine);
        }
        ~clEditorColouriseLocker()
        {
            if(m_firstPos > m_lastPos) {
                m_stc->Colourise(m_firstPos, m_lastPos);
            }
        }
    };

    ContextGeneric::ContextGeneric(Editor* container, const wxString& name)
        : ContextBase(container)
    {
        SetName(name);
        ApplySettings();
    }

    ContextGeneric::~ContextGeneric() {}

    ContextBase* ContextGeneric::NewInstance(Editor* container) { return new ContextGeneric(container, GetName()); }

    void ContextGeneric::ApplySettings()
    {
        LexerStyle::Ptr_t lexPtr;
        if(gsgs_EditorConfig().IsOk()) {
            lexPtr = gsgs_EditorConfig().GetLexer(GetName());
        }
        Editor& rCtrl = GetCtrl();
        if(lexPtr) {
            rCtrl.SetLexer(lexPtr->GetLexerId());
            for(int i = 0; i <= 4; ++i) {
                wxString keyWords = lexPtr->GetKeyWords(i);
                keyWords.Replace(wxT("\n"), wxT(" "));
                keyWords.Replace(wxT("\r"), wxT(" "));
                rCtrl.SetKeyWords(i, keyWords);
            }

            if(lexPtr->GetName() == "css") {
                // set the word characters for the CSS lexer
                GetCtrl().SetWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-@");
            }
        } else {
            rCtrl.SetLexer(wxSTC_LEX_NULL);
        }
        DoApplySettings(lexPtr);
    }

    void ContextGeneric::ProcessIdleActions()
    {
        Editor& ctrl = GetCtrl();
        if((ctrl.GetLexerId() == wxSTC_LEX_XML) || (ctrl.GetLexerId() == wxSTC_LEX_PHPSCRIPT) ||
           (ctrl.GetLexerId() == wxSTC_LEX_HTML)) {
            // XML lexer, highlight XML tags
            // Update the word chars
            clEditorXmlHelper xmlHelper(ctrl.GetCtrl());
            clEditorColouriseLocker colouriseLocker(ctrl.GetCtrl());

            // Highlight matching tags
            int startPos, endPos;
            wxString word = xmlHelper.GetXmlTagAt(ctrl.GetCurrentPosition(), startPos, endPos);
            if(word.IsEmpty()) {
                ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
                ctrl.IndicatorClearRange(0, ctrl.GetLength());
                return;
            }

            static wxRegEx reOpenHtmlTag("<([a-z_\\-0-9]+)", wxRE_ADVANCED | wxRE_ICASE);
            static wxRegEx reCloseHtmlTag("</([a-z_\\-0-9]+)>", wxRE_ADVANCED | wxRE_ICASE);

            wxString searchWhat;
            wxString closeTag;

            if(reOpenHtmlTag.Matches(word)) {
                searchWhat = reOpenHtmlTag.GetMatch(word, 1);
                closeTag << "</" << searchWhat << ">";
                wxRegEx reOpenTag("<" + searchWhat + "[>]?", wxRE_ADVANCED | wxRE_ICASE);

                int pos = endPos;
                int depth = 0;
                int where = FindNext(searchWhat, pos, true);

                while(where != wxNOT_FOUND) {
                    int startPos2, endPos2;
                    word = xmlHelper.GetXmlTagAt(where, startPos2, endPos2);
                    if((closeTag == word) && (depth == 0)) {
                        // We got the closing brace
                        ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
                        ctrl.IndicatorClearRange(0, ctrl.GetLength());

                        // Set the new markers
                        ctrl.IndicatorFillRange(startPos, endPos - startPos);
                        ctrl.IndicatorFillRange(startPos2, endPos2 - startPos2);
                        return;

                    } else if(closeTag == word) {
                        --depth;
                    } else if(reOpenTag.Matches(word)) {
                        depth++;
                    }
                    where = FindNext(searchWhat, pos, true);
                }

            } else if(reCloseHtmlTag.Matches(word)) {
                searchWhat = reCloseHtmlTag.GetMatch(word, 1);
                closeTag << "</" << searchWhat << ">";
                
                wxString reString = "<" + searchWhat + "[>]?";
                wxRegEx reOpenTag(reString, wxRE_DEFAULT | wxRE_ICASE);
                if(!reOpenTag.IsValid()) {
                    gsgsDEBUG() << "Invalid regex:" << reString << clEndl;
                }
                
                int pos = startPos;
                int depth = 0;
                int where = FindPrev(searchWhat, pos, true);

                while(where != wxNOT_FOUND) {
                    int startPos2, endPos2;
                    word = xmlHelper.GetXmlTagAt(where, startPos2, endPos2);
                    if(reOpenTag.Matches(word) && (depth == 0)) {
                        // We got the closing brace
                        ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
                        ctrl.IndicatorClearRange(0, ctrl.GetLength());

                        // Set the new markers
                        ctrl.IndicatorFillRange(startPos, endPos - startPos);
                        ctrl.IndicatorFillRange(startPos2, endPos2 - startPos2);
                        return;

                    } else if(closeTag == word) {
                        ++depth;
                    } else if(reOpenTag.Matches(word)) {
                        --depth;
                    }
                    where = FindPrev(searchWhat, pos, true);
                }
            }

            // Clear the current selection
            ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
            ctrl.IndicatorClearRange(0, ctrl.GetLength());
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ContextManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ContextManager::ContextManager()
    {
        Initialize();
    }
    //------------------------------------------------------------------------
    ContextManager::~ContextManager()
    {
    }
    //------------------------------------------------------------------------
    ContextBasePtr ContextManager::NewContext(Editor* parent, const wxString& lexerName)
    {
        // this function is actually a big switch ....
        std::map<wxString, ContextBasePtr>::iterator iter = m_contextPool.find(lexerName.Lower());
        if(iter == m_contextPool.end()) {
            return m_contextPool["text"]->NewInstance(parent);
        }

        return iter->second->NewInstance((Editor*)parent);
    }
    //------------------------------------------------------------------------
    ContextBasePtr ContextManager::NewContextByFileName(Editor* parent, const wxFileName& fileName)
    {
        LexerStyle::Ptr_t lexer = gsgs_EditorConfig().GetLexerForFile(fileName.GetFullPath());
        if(!lexer) {
            // could not locate a lexer for this file name, return the default text lexer
            return ContextManager::Get()->NewContext(parent, wxT("Text"));
        }
        return ContextManager::Get()->NewContext(parent, lexer->GetName());
    }
    //------------------------------------------------------------------------
    void ContextManager::Initialize()
    {
        // Popuplate the contexts available
        m_contextPool.clear();

        // register available contexts
        m_contextPool["c++"] = new ContextCpp();
        m_contextPool["diff"] = new ContextDiff();
        m_contextPool["html"] = new ContextHtml();
        m_contextPool["php"] = new ContextPhp();
        m_contextPool["javascript"] = new ContextJavaScript();

        // load generic lexers
        wxArrayString names = gsgs_Style().GetAllLexersNames();
        for(size_t i = 0; i < names.GetCount(); ++i) {
            if(m_contextPool.find(names.Item(i)) == m_contextPool.end()) {
                m_contextPool[names.Item(i)] = new ContextGeneric(names.Item(i));
            }
        }

        // make sure there is a "fallback" lexer for unrecognized file types
        if(m_contextPool.find("text") == m_contextPool.end()) {
            m_contextPool[wxT("text")] = new ContextGeneric(wxT("text"));
        }
    }
    //------------------------------------------------------------------------
}