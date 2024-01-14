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

#ifndef REFACTORENGINE_H
#define REFACTORENGINE_H

#include "gsgsPreInclude.h"
#include "gsgsTag.h"
#include "cppwordscanner.h"
#include "gsgsEvent.h"

namespace gsgs
{
    class ProgressDlg;
    //----------------------------------------------------------------------------------

    struct RefactorSource {
        wxString name;
        wxString scope;
        bool isClass;

        RefactorSource()
            : name(wxEmptyString)
            , scope(wxEmptyString)
            , isClass(false)
        {
        }

        void Reset()
        {
            name.clear();
            scope.clear();
            isClass = false;
        }
    };

    //-----------------------------------------------------------------------------------

    // --------------------------------------------------------------
    // Refactoring event
    // --------------------------------------------------------------
    class _gsgsAPI RefactoringEvent : public Event
    {
        CppToken::Vec_t m_matches;
        CppToken::Vec_t m_possibleMatches;

    public:
        RefactoringEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
            : gsgs::Event(commandType, winid)
        {
        }
        RefactoringEvent(const RefactoringEvent& event) { *this = event; }
        RefactoringEvent& operator=(const RefactoringEvent& src)
        {
            gsgs::Event::operator=(src);
            m_matches = src.m_matches;
            m_possibleMatches = src.m_possibleMatches;
            return *this;
        }

        virtual ~RefactoringEvent() {}
        void SetMatches(const CppToken::Vec_t& matches) { this->m_matches = matches; }
        void SetPossibleMatches(const CppToken::Vec_t& possibleMatches) { this->m_possibleMatches = possibleMatches; }
        const CppToken::Vec_t& GetMatches() const { return m_matches; }
        const CppToken::Vec_t& GetPossibleMatches() const { return m_possibleMatches; }
        virtual wxEvent* Clone() const { return new RefactoringEvent(*this); }
    };

    typedef void (wxEvtHandler::*clRefactoringEventFunction)(RefactoringEvent&);
    #define clRefactoringEventHandler(func) wxEVENT_HANDLER_CAST(clRefactoringEventFunction, func)

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_REFACTOR_ENGINE_REFERENCES, RefactoringEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_REFACTOR_ENGINE_RENAME_SYMBOL, RefactoringEvent);

    class _gsgsAPI RefactoringEngine : public wxEvtHandler
    {
        friend class ScopeCleaner;

    public:
        enum eActionType {
            kNone,
            kFindReferences,
            kRenameSymbol,
        };

        class ScopeCleaner
        {
        public:
            ScopeCleaner() {}
            ~ScopeCleaner() { RefactoringEngine::Instance()->DoCleanup(); }
        };
    public:
        static RefactoringEngine* Instance();
        static void Shutdown();

        bool IsBusy() const { return m_currentAction != kNone; }
        void SetCandidates(const CppToken::Vec_t& candidates) { this->m_candidates = candidates; }
        void SetPossibleCandidates(const CppToken::Vec_t& possibleCandidates)
        {
            this->m_possibleCandidates = possibleCandidates;
        }
        const CppToken::Vec_t& GetCandidates() const { return m_candidates; }
        const CppToken::Vec_t& GetPossibleCandidates() const { return m_possibleCandidates; }
        wxString GetExpression(int pos, TextStatesPtr states);

        void Clear();
        /**
         * @brief rename global symbol. Global Symbol can be one of:
         * <li>member
         * <li>global variable
         * <li>class
         * <li>struct
         * <li>union
         * <li>namespace
         * <li>function
         * <li>prototype
         * <li>typedef
         * @param symname the name of the symbol
         * @param fn current file
         * @param line the symbol is loacted in this file
         * @param pos at that position
         * @param files perform the refactoring on these files
         */
        void RenameGlobalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos,
                                const wxFileList_t& files);

        /**
         * @param rename local variable
         * @param symname symbol we want to rename
         * @param fn the current file name (full path)
         * @param line the line where our symbol appears
         * @param pos the position of the symbol (this should be pointing to the *start* of the symbol)
         */
        void RenameLocalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos);

        /**
         * @brief find usages of given symbol in a list of files
         * @param symname symbol to search for
         * @param fn active file open which holds the context of the symbol
         * @param line the line where the symbol exists
         * @param pos the position of the symbol (this should be pointing to the *start* of the symbol)
         * @param files list of files to search in
         */
        void FindReferences(const wxString& symname, const wxFileName& fn, int line, int pos, const wxFileList_t& files);

        /**
         * @brief given a location (file:line:pos) use the current location function signature
         * and return the propsed counter-part tag
         */
        TagEntryPtr SyncSignature(const wxFileName& fn, int line, int pos, const wxString& word, const wxString& text,
                                  const wxString& expr);
    protected:
        RefactoringEngine();
        ~RefactoringEngine();
        bool DoResolveWord(TextStatesPtr states, const wxFileName& fn, int pos, int line, const wxString& word,
                           RefactorSource* rs);

        void OnSearchStarted(wxCommandEvent& event);
        void OnSearchEnded(wxCommandEvent& event);
        void OnSearchMatch(wxCommandEvent& event);

    #if wxUSE_GUI
        ProgressDlg* CreateProgressDialog(const wxString& title, int maxValue);
    #endif
        void DoFindReferences(const wxString& symname, const wxFileName& fn, int line, int pos, const wxFileList_t& files,
                              bool onlyDefiniteMatches, eActionType type);
        void DoCompleteFindReferences();
        void DoCleanup();
    protected:
        CppToken::Vec_t m_candidates;
        CppToken::Vec_t m_possibleCandidates;
        wxEvtHandler* m_evtHandler;

        RefactorSource m_refactorSource;

        eActionType m_currentAction = kNone;
        CppToken::Vec_t m_tokens;
        wxString m_symbolName;
        bool m_onlyDefiniteMatches = false;

        SearchThread* m_seartchThread = nullptr;
    };
}
#endif