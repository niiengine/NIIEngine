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
#ifndef __quickfindbar__
#define __quickfindbar__

#include "gsgsPreInclude.h"
#include "gsgsPluginCommon.h"
#include "gsgsButton.h"
#include <wx/combobox.h>
#include <wx/stattext.h>

namespace gsgs
{
    class _gsgsAPI QuickFindBar : public wxPanel
    {
    public:
        enum eRegexType {
            kRegexNone,
            kRegexWildcard,
            kRegexPosix,
        };
        enum {
            wxID_FIND_ALL = 1001,
        };
        enum {
            ID_TOOL_REPLACE = 1000,
            ID_TOOL_CLOSE,
            ID_TOOL_FIND,
        };

        enum {
            ID_MENU_REGEX = 2000,
            ID_MENU_WILDCARD,
            ID_MENU_NO_REGEX,
        };

        enum {
            // We mix between wxSTC flags and our own, to avoid the mixing the values
            // we start from the 16th bit
            kSearchForward = (1 << 16),
            kDisableDisplayErrorMessages = (1 << 17),
            kBreakWhenWrapSearch = (1 << 18),
        };

    public:
        QuickFindBar(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxSize(-1, -1),
                     long style = wxTAB_TRAVERSAL | wxTRANSPARENT_WINDOW | wxBORDER_NONE);
        virtual ~QuickFindBar();
        int GetCloseButtonId();
        bool ShowForPlugins();
        bool Show(bool s = true);
        bool Show(const wxString& findWhat, bool showReplace);
        wxStyledTextCtrl* GetEditor() { return m_sci; }
        void SetEditor(wxStyledTextCtrl* sci);

        /**
         * @brief search a stc control for 'findwhat'. Use kSearchForward to indicate searching forward, pass 0
         * for backward.
         * 'This' is used internally, so pass it NULL
         */
        static bool Search(wxStyledTextCtrl* ctrl, const wxString& findwhat, size_t search_flags,
                           QuickFindBar* This = NULL);

        wxString GetFindWhat() const { return m_textCtrlFind->GetValue(); }
        void SetFindWhat(const wxString& findwhat) { m_textCtrlFind->ChangeValue(findwhat); }
        
        gsgs::ToolBar* GetToolbar() { return m_toolbar; }
        wxTextCtrl* GetTextCtrlFind() { return m_textCtrlFind; }
        DefaultButton* GetButtonFind() { return m_buttonFind; }
        DefaultButton* GetButtonFindPrev() { return m_buttonFindPrev; }
        DefaultButton* GetButtonFindAll() { return m_buttonFindAll; }
        wxTextCtrl* GetTextCtrlReplace() { return m_textCtrlReplace; }
        DefaultButton* GetButtonReplace() { return m_buttonReplace; }
        DefaultButton* GetButtonReplaceAll() { return m_buttonReplaceAll; }
    
    protected:
        virtual void OnReplaceKeyDown(wxKeyEvent& event);
        bool DoSearch(size_t searchFlags);
        void DoSearchCB(size_t searchFlags) { DoSearch(searchFlags); }
        void DoReplace();
        void DoSetCaretAtEndOfText();
        void DoFixRegexParen(wxString& findwhat);
        wxString DoGetSelectedText();
        void DoSelectAll(bool addMarkers);
        void DoHighlightMatches(bool checked);
        bool DoShow(bool s, const wxString& findWhat, bool showReplace = false);

        wxStyledTextCtrl* DoCheckPlugins();

        // General events
        static void DoEnsureLineIsVisible(wxStyledTextCtrl* sci, int line = wxNOT_FOUND);

        // Control events
        void OnHide(wxCommandEvent& e);
        void OnNext(wxCommandEvent& e);
        void OnPrev(wxCommandEvent& e);
        void OnFindAll(wxCommandEvent& e);
        void OnButtonNext(wxCommandEvent& e);
        void OnButtonPrev(wxCommandEvent& e);
        void OnText(wxCommandEvent& e);
        void OnKeyDown(wxKeyEvent& e);
        void OnFindMouseWheel(wxMouseEvent& e);
        void OnButtonReplace(wxCommandEvent& e);
        void OnReplaceAll(wxCommandEvent& e);
        void OnEnter(wxCommandEvent& e);
        void OnReplace(wxCommandEvent& e);
        //void OnUpdateUI(wxUpdateUIEvent& e);
        void OnReplaceEnter(wxCommandEvent& e);
        void OnQuickFindCommandEvent(wxCommandEvent& event);
        void OnReceivingFocus(wxFocusEvent& event);
        void OnReleaseEditor(FindEvent& e);
        void OnPaint(wxPaintEvent& e);
        void OnFindNextCaret(wxCommandEvent& e);
        void OnFindPreviousCaret(wxCommandEvent& e);

    protected:
        virtual void OnButtonKeyDown(wxKeyEvent& event);
        virtual void OnReplaceAllUI(wxUpdateUIEvent& event);
        virtual void OnReplaceUI(wxUpdateUIEvent& event);
        virtual void OnFind(wxCommandEvent& event);
        virtual void OnFindAllUI(wxUpdateUIEvent& event);
        virtual void OnFindPrev(wxCommandEvent& event);
        virtual void OnFindPrevUI(wxUpdateUIEvent& event);
        virtual void OnFindUI(wxUpdateUIEvent& event);
        virtual void OnReplaceTextEnter(wxCommandEvent& event);
        virtual void OnReplaceTextUpdated(wxCommandEvent& event);

        void DoArrowDown(TerminalHistory& history, wxTextCtrl* ctrl);
        void DoArrowUp(TerminalHistory& history, wxTextCtrl* ctrl);

    private:
        size_t DoGetSearchFlags() const;
        void DoReplaceAll(bool selectionOnly);
    protected:
        wxStyledTextCtrl* m_sci;
        wxString m_lastText;
        eRegexType m_regexType;
        EditEventsHandler::Ptr_t m_findEventsHandler;
        EditEventsHandler::Ptr_t m_replaceEventsHandler;
        TerminalHistory m_searchHistory;
        TerminalHistory m_replaceHistory;
        wxStaticText* m_matchesFound = nullptr;
        ToolBar* m_toolbar;
        wxTextCtrl* m_textCtrlFind;
        DefaultButton* m_buttonFind;
        DefaultButton* m_buttonFindPrev;
        DefaultButton* m_buttonFindAll;
        wxTextCtrl* m_textCtrlReplace;
        DefaultButton* m_buttonReplace;
        DefaultButton* m_buttonReplaceAll;
        wchar_t* m_lastTextPtr;
        size_t m_searchFlags;
        bool m_highlightMatches;
        bool m_replaceInSelection;
        bool m_eventsConnected;
        bool m_onNextPrev;
        bool m_disableTextUpdateEvent;
    };
}
#endif // __quickfindbar__
