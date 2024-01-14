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
#ifndef SEARCH_THREAD_H
#define SEARCH_THREAD_H

#include "gsgsPreInclude.h"
#include "cppwordscanner.h"
#include "gsgsSingleton.h"
#include "gsgsThread.h"
#include "wx/regex.h"
#include "gsgsJSON.h"

class wxEvtHandler;
class SearchResult;
class SearchThread;

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //----------------------------------------------------------
    // The searched data class to be passed to the search thread
    //----------------------------------------------------------
    // Possible search data options:
    enum 
    {
        wxSD_MATCHCASE = 0x00000001,
        wxSD_MATCHWHOLEWORD = 0x00000002,
        wxSD_REGULAREXPRESSION = 0x00000004,
        wxSD_SEARCH_BACKWARD = 0x00000008,
        wxSD_USE_EDITOR_ENCODING = 0x00000010,
        wxSD_PRINT_SCOPE = 0x00000020,
        wxSD_SKIP_COMMENTS = 0x00000040,
        wxSD_SKIP_STRINGS = 0x00000080,
        wxSD_COLOUR_COMMENTS = 0x00000100,
        wxSD_WILDCARD = 0x00000200,
        wxSD_ENABLE_PIPE_SUPPORT = 0x00000400,
    };

    class _gsgsAPI SearchData : public ThreadRequest
    {
        friend class SearchThread;
    public:
        SearchData()
            : ThreadRequest()
            , m_findString(wxEmptyString)
            , m_flags(0)
            , m_newTab(false)
            , m_owner(NULL){}

        SearchData(const SearchData& rhs)                   { Copy(rhs); }
        SearchData& operator=(const SearchData& rhs);

        virtual ~SearchData() {}
        SearchData& Copy(const SearchData& other);

    public:
        //------------------------------------------
        // Setters / Getters
        //------------------------------------------
        inline bool IsMatchCase() const                     { return m_flags & wxSD_MATCHCASE ? true : false; }
        inline bool IsEnablePipeSupport() const             { return m_flags & wxSD_ENABLE_PIPE_SUPPORT; }
        inline void SetEnablePipeSupport(bool b)            { SetOption(wxSD_ENABLE_PIPE_SUPPORT, b); }
        inline bool IsMatchWholeWord() const                { return m_flags & wxSD_MATCHWHOLEWORD ? true : false; }
        inline bool IsRegularExpression() const             { return m_flags & wxSD_REGULAREXPRESSION ? true : false; }
        inline const wxArrayString& GetRootDirs() const     { return m_rootDirs; }
        inline void SetMatchCase(bool matchCase)            { SetOption(wxSD_MATCHCASE, matchCase); }
        inline void SetMatchWholeWord(bool matchWholeWord)  { SetOption(wxSD_MATCHWHOLEWORD, matchWholeWord); }
        inline void SetRegularExpression(bool re)           { SetOption(wxSD_REGULAREXPRESSION, re); }
        inline void SetExtensions(const wxString& exts)     { m_validExt = exts; }
        inline void SetRootDirs(const wxArrayString& rootDirs) { m_rootDirs = rootDirs; }
        inline const wxString& GetExtensions() const;
        inline const wxString& GetFindString() const        { return m_findString; }
        inline void SetFindString(const wxString& findString) { m_findString = findString; }
        inline void SetFiles(const wxArrayString& files)    { m_files = files; }
        inline const wxArrayString& GetFiles() const        { return m_files; }
        inline void SetExcludePatterns(const wxArrayString& excludePatterns) { this->m_excludePatterns = excludePatterns; }
        inline const wxArrayString& GetExcludePatterns() const { return m_excludePatterns; }
        inline void UseNewTab(bool useNewTab)               { m_newTab = useNewTab; }
        inline bool UseNewTab() const                       { return m_newTab; }
        inline void SetEncoding(const wxString& encoding)   { this->m_encoding = encoding.c_str(); }
        inline const wxString& GetEncoding() const          { return this->m_encoding; }
        inline bool GetDisplayScope() const                 { return m_flags & wxSD_PRINT_SCOPE ? true : false; }
        inline void SetDisplayScope(bool d)                 { SetOption(wxSD_PRINT_SCOPE, d); }
        inline void SetOwner(wxEvtHandler* owner)           { this->m_owner = owner; }
        inline wxEvtHandler* GetOwner() const               { return m_owner; }
        inline bool HasCppOptions() const
        {
            return (m_flags & wxSD_SKIP_COMMENTS) || (m_flags & wxSD_SKIP_STRINGS) || (m_flags & wxSD_COLOUR_COMMENTS);
        }

        inline void SetSkipComments(bool d)                 { SetOption(wxSD_SKIP_COMMENTS, d); }
        inline void SetSkipStrings(bool d)                  { SetOption(wxSD_SKIP_STRINGS, d); }
        inline void SetColourComments(bool d)               { SetOption(wxSD_COLOUR_COMMENTS, d); }
        inline bool GetSkipComments() const                 { return (m_flags & wxSD_SKIP_COMMENTS); }
        inline bool GetSkipStrings() const                  { return (m_flags & wxSD_SKIP_STRINGS); }
        inline bool GetColourComments() const               { return (m_flags & wxSD_COLOUR_COMMENTS); }
        inline const wxString& GetReplaceWith() const       { return m_replaceWith; }
        inline void SetReplaceWith(const wxString& replaceWith) { this->m_replaceWith = replaceWith; }

    private:
        // An internal helper function that set/remove an option bit
        void SetOption(int option, bool set)
        {
            if(set) 
            {
                m_flags |= option;
            } else {
                m_flags &= ~(option);
            }
        }
    private:
        size_t m_flags;
        wxArrayString m_rootDirs;
        wxString m_findString;
        wxString m_replaceWith;
        wxString m_validExt;
        wxArrayString m_files;
        wxEvtHandler* m_owner;
        wxString m_encoding;
        wxArrayString m_excludePatterns;
        bool m_newTab;
    };

    //------------------------------------------
    // class containing the search result
    //------------------------------------------
    class _gsgsAPI SearchResult : public wxObject
    {
    public:
        // ctor-dtor, copy constructor and assignment operator
        SearchResult() {}

        virtual ~SearchResult() {}

        SearchResult(const SearchResult& rhs) { *this = rhs; }

        SearchResult& operator=(const SearchResult& rhs)
        {
            if(this == &rhs) return *this;
            m_position = rhs.m_position;
            m_column = rhs.m_column;
            m_lineNumber = rhs.m_lineNumber;
            m_pattern = rhs.m_pattern.c_str();
            m_fileName = rhs.m_fileName.c_str();
            m_len = rhs.m_len;
            m_findWhat = rhs.m_findWhat.c_str();
            m_flags = rhs.m_flags;
            m_columnInChars = rhs.m_columnInChars;
            m_lenInChars = rhs.m_lenInChars;
            m_matchState = rhs.m_matchState;
            m_scope = rhs.m_scope.c_str();
            return *this;
        }

        JSONObject ToJSON() const;
        void FromJSON(const JSONObject& json);

        //------------------------------------------------------
        // Setters/getters

        inline void SetFlags(size_t flags)                      { m_flags = flags; }

        inline size_t GetFlags() const                          { return m_flags; }

        inline void SetPattern(const wxString& pat)             { m_pattern = pat.c_str(); }
        inline void SetPosition(const int& position)            { m_position = position; }
        inline void SetLineNumber(const int& line)              { m_lineNumber = line; }
        inline void SetColumn(const int& col)                   { m_column = col; }
        inline void SetFileName(const wxString& fileName)       { m_fileName = fileName.c_str(); }

        inline const int& GetPosition() const                   { return m_position; }
        inline const int& GetLineNumber() const                 { return m_lineNumber; }
        inline const int& GetColumn() const                     { return m_column; }
        inline const wxString& GetPattern() const               { return m_pattern; }
        inline const wxString& GetFileName() const              { return m_fileName; }

        inline void SetLen(int len)                             { m_len = len; }
        inline int GetLen() const                               { return m_len; }

        // Setters
        inline void SetFindWhat(const wxString& findWhat)       { this->m_findWhat = findWhat.c_str(); }
        // Getters
        inline const wxString& GetFindWhat() const              { return m_findWhat; }

        inline void SetColumnInChars(const int& col)            { this->m_columnInChars = col; }
        inline const int& GetColumnInChars() const              { return m_columnInChars; }

        inline void SetLenInChars(const int& len)               { this->m_lenInChars = len; }
        inline const int& GetLenInChars() const                 { return m_lenInChars; }

        inline void SetMatchState(short matchState)             { this->m_matchState = matchState; }
        inline short GetMatchState() const                      { return m_matchState; }

        inline void SetScope(const wxString& scope)             { this->m_scope = scope.c_str(); }
        inline const wxString& GetScope() const                 { return m_scope; }
        // return a foramtted message
        inline wxString GetMessage() const
        {
            wxString msg;
            msg << GetFileName() << wxT("(") << GetLineNumber() << wxT(",") << GetColumn() << wxT(",") << GetLen()
                << wxT("): ") << GetPattern();
            return msg;
        }
    private:
        wxString m_pattern;
        wxString m_fileName;
        wxString m_findWhat;
        wxString m_scope;
        int m_position;
        int m_lineNumber;
        int m_column;
        int m_len;
        int m_columnInChars;
        int m_lenInChars;
        short m_matchState;
        size_t m_flags;
    };

    typedef std::list<SearchResult> SearchResultList;

    class _gsgsAPI SearchSummary : public wxObject
    {
    public:
        SearchSummary()
            : m_fileScanned(0)
            , m_matchesFound(0)
            , m_elapsed(0)
        {
        }

        virtual ~SearchSummary() {}

        SearchSummary(const SearchSummary& rhs)                 { *this = rhs; }

        SearchSummary& operator=(const SearchSummary& rhs)
        {
            if(this == &rhs) return *this;

            m_fileScanned = rhs.m_fileScanned;
            m_matchesFound = rhs.m_matchesFound;
            m_elapsed = rhs.m_elapsed;
            m_failedFiles = rhs.m_failedFiles;
            m_findWhat = rhs.m_findWhat;
            m_replaceWith = rhs.m_replaceWith;
            return *this;
        }

        JSONObject ToJSON() const;
        void FromJSON(const JSONObject& json);

        inline void SetFindWhat(const wxString& findWhat)       { this->m_findWhat = findWhat; }
        inline void SetReplaceWith(const wxString& replaceWith) { this->m_replaceWith = replaceWith; }
        inline const wxString& GetFindWhat() const              { return m_findWhat; }
        inline const wxString& GetReplaceWith() const           { return m_replaceWith; }
        inline const wxArrayString& GetFailedFiles() const      { return m_failedFiles; }
        inline wxArrayString& GetFailedFiles()                  { return m_failedFiles; }

        inline int GetNumFileScanned() const                    { return m_fileScanned; }
        inline int GetNumMatchesFound() const                   { return m_matchesFound; }

        inline void SetNumFileScanned(const int& num)           { m_fileScanned = num; }
        inline void SetNumMatchesFound(const int& num)          { m_matchesFound = num; }
        inline void SetElapsedTime(long elapsed)                { m_elapsed = elapsed; }
        wxString GetMessage() const
        {
            wxString msg(wxString(wxT("====== ")) + _("Number of files scanned: "));
            msg << m_fileScanned << wxT(",");
            msg << _(" Matches found: ");
            msg << m_matchesFound;
            int secs = m_elapsed / 1000;
            int msecs = m_elapsed % 1000;

            msg << _(", elapsed time: ") << secs << wxT(".") << msecs << _(" seconds") << wxT(" ======");
            if(!m_failedFiles.IsEmpty()) {
                msg << "\n";
                msg << "====== " << _("Failed to open the following files for scan:") << "\n";
                for(size_t i = 0; i < m_failedFiles.size(); ++i) {
                    msg << m_failedFiles.Item(i) << "\n";
                }
            }
            return msg;
        }
    private:
        int m_fileScanned;
        int m_matchesFound;
        int m_elapsed;
        wxArrayString m_failedFiles;
        wxString m_findWhat;
        wxString m_replaceWith;
    };

    //----------------------------------------------------------
    // The search thread
    //----------------------------------------------------------

    class _gsgsAPI SearchThread : public WorkerThread
    {
    public:
        SearchThread();
        virtual ~SearchThread();

        /**
         * Process request from caller
         */
        void ProcessRequest(ThreadRequest* req);

        /**
         * Add a request to the search thread to start
         * \param data SearchData class
         */
        void PerformSearch(const SearchData& data);

        /**
         * Stops the current search operation
         * \note This call must be called from the context of other thread (e.g. main thread)
         */
        void StopSearch(bool stop = true);

        /**
         *  The search thread has several functions that operate on words,
         *  which are defined to be contiguous sequences of characters from a particular set of characters.
         *  Defines which characters are members of that set. The default is set to:
         * "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"
         * \param chars sequence of characters that are considered part of a word
         */
        void SetWordChars(const wxString& chars);

    private:
        /**
         * Return files to search
         * \param files output
         * \param data search data
         */
        void GetFiles(const SearchData* data, wxArrayString& files);

        /**
         * Index the word chars from the array into a map
         */
        void IndexWordChars();

        // Test to see if user asked to cancel the search
        bool TestStopSearch();

        /**
         * Do the actual search operation
         * \param data inpunt contains information about the search
         */
        void DoSearchFiles(ThreadRequest* data);

        // Perform search on a single file
        void DoSearchFile(const wxString& fileName, const SearchData* data);

        // Perform search on a line
        void DoSearchLine(const wxString& line, const int lineNum, const int lineOffset, const wxString& fileName,
                          const SearchData* data, const wxString& findWhat, const wxArrayString& filters,
                          TextStatesPtr statesPtr);

        // Perform search on a line using regular expression
        void DoSearchLineRE(const wxString& line, const int lineNum, const int lineOffset, const wxString& fileName,
                            const SearchData* data, TextStatesPtr statesPtr);

        // Send an event to the notified window
        void SendEvent(wxEventType type, wxEvtHandler* owner);

        // return a compiled regex object for the expression
        wxRegEx& GetRegex(const wxString& expr, bool matchCase);

        // Internal function
        bool AdjustLine(wxString& line, int& pos, const wxString& findString);

        // filter 'files' according to the files spec
        void FilterFiles(wxArrayString& files, const SearchData* data);
    private:
        friend class SearchThreadST;
        wxString m_wordChars;
        std::unordered_map<wxChar, bool> m_wordCharsMap; //< Internal
        SearchResultList m_results;
        SearchSummary m_summary;
        wxString m_reExpr;
        wxRegEx m_regex;
        wxCriticalSection m_cs;
        int m_counter = 0;
        bool m_matchCase;
        bool m_stopSearch;
    };

    class _gsgsAPI SearchThreadST
    {
    public:
        static SearchThread * Get();
        static void Free();
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SEARCH_THREAD_MATCHFOUND, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SEARCH_THREAD_SEARCHEND, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SEARCH_THREAD_SEARCHSTARTED, wxCommandEvent);
}
#endif // SEARCH_THREAD_H
