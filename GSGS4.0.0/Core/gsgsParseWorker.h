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

#ifndef gsgs_PARSE_THREAD_H
#define gsgs_PARSE_THREAD_H

#include "gsgsPreInclude.h"
#include "gsgsTag.h"
#include "gsgsSingleton.h"
#include "gsgsThread.h"
#include "gsgsProcess.h"
#include "gsgsEvent.h"
#include <wx/stopwatch.h>
#include "istorage.h"
#include <memory>

class ITagsStorage;

namespace gsgs
{
    /**
     * @class ParseRequest
     * @author eran
     * @date 10/04/09
     * @file gsgsParseWorker.h
     * @brief a class representing a parsing request
     */
    class _gsgsAPI ParseRequest : public ThreadRequest
    {
    public:
        enum {
            PR_FILESAVED,
            PR_PARSEINCLUDES,
            PR_PARSE_AND_STORE,
            PR_DELETE_TAGS_OF_FILES,
            PR_PARSE_FILE_NO_INCLUDES,
            PR_PARSE_INCLUDE_STATEMENTS,
            PR_SUGGEST_HIGHLIGHT_WORDS,
            PR_SOURCE_TO_TAGS,
        };
    public:
        ParseRequest(wxEvtHandler* handler)
            : _type(PR_FILESAVED)
            , _evtHandler(handler)
            , _quickRetag(false)
            , _uid(-1){}
        virtual ~ParseRequest();

        // accessors

        inline void SetDefinitions(const wxArrayString& definitions)    { this->m_definitions = definitions; }
        inline void SetIncludePaths(const wxArrayString& includePaths)  { this->m_includePaths = includePaths; }
        inline const wxArrayString& GetDefinitions() const              { return m_definitions; }
        inline const wxArrayString& GetIncludePaths() const             { return m_includePaths; }
        void setFile(const wxString& file);
        void setDbFile(const wxString& dbfile);
        void setTags(const wxString& tags);

        // Getters
        inline const wxString& getDbfile() const                        { return _dbfile; }

        inline const wxString& getFile() const                          { return _file; }

        inline const wxString& getTags() const                          { return _tags; }

        inline void setType(int _type)                                  { this->_type = _type; }
        inline int getType() const                                      { return _type; }
        // copy ctor
        ParseRequest(const ParseRequest& rhs);

        // assignment operator
        ParseRequest& operator=(const ParseRequest& rhs);
    public:
        wxEvtHandler* _evtHandler;
        std::vector<std::string> _workspaceFiles;
        bool _quickRetag;
        int _uid;
    private:
        wxString _file;
        wxString _dbfile;
        wxString _tags;
        int _type;
        wxArrayString m_definitions;
        wxArrayString m_includePaths;
    };

    /**
     * @class ParseThread
     * @author eran
     * @date 10/04/09
     * @file gsgsParseWorker.h
     * @brief
     */
    class _gsgsAPI ParseThread : public WorkerThread
    {
        friend class ParseThreadST;
    public:
        inline void SetCrawlerEnabeld(bool b)       { wxCriticalSectionLocker locker(m_cs); m_crawlerEnabled = b;}
        void SetSearchPaths(const wxArrayString& paths, const wxArrayString& exlucdePaths);
        void GetSearchPaths(wxArrayString& paths, wxArrayString& excludePaths);
        inline bool IsCrawlerEnabled()              { wxCriticalSectionLocker locker(m_cs); return m_crawlerEnabled;}
        /**
         * @brief add search paths to the parser. This method is thread safe
         */
        void AddPaths(const wxArrayString & inc, const wxArrayString & exc);

        /**
         * @brief clear the current search paths
         */
        void ClearPaths();
    private:
        ParseThread();
        virtual ~ParseThread();

        void DoStoreTags(const wxString& tags, const wxString& filename, int& count, ITagsStoragePtr db);

        TagTreePtr DoTreeFromTags(const wxString& tags, int& count);

        void DoNotifyReady(wxEvtHandler* caller, int requestType);

        /**
         * Process request from the editor.
         * \param request the request to process
         */
        void ProcessRequest(ThreadRequest* request);
        /**
         * @brief parse include files and retrieve a list of all
         * include files that should be tagged and inserted into
         * the external database
         * @param filename
         */
        void ParseIncludeFiles(ParseRequest* req, const wxString& filename, ITagsStoragePtr db);

        void ProcessSimple(ParseRequest* req);
        void ProcessSourceToTags(ParseRequest* req);
        void ProcessIncludes(ParseRequest* req);
        void ProcessParseAndStore(ParseRequest* req);
        void ProcessDeleteTagsOfFiles(ParseRequest* req);
        void ProcessSimpleNoIncludes(ParseRequest* req);
        void ProcessIncludeStatements(ParseRequest* req);
        void ProcessColourRequest(ParseRequest* req);
        void GetFileListToParse(const wxString& filename, wxArrayString& arrFiles);
        void ParseAndStoreFiles(ParseRequest* req, const wxArrayString& arrFiles, int initalCount, ITagsStoragePtr db);

        void FindIncludedFiles(ParseRequest* req, std::set<wxString>* newSet);

    private:
        wxStopWatch m_watch;
        wxArrayString m_searchPaths;
        wxArrayString m_excludePaths;
        wxCriticalSection m_cs;
        bool m_crawlerEnabled;
    };

    class _gsgsAPI ParseThreadST
    {
    public:
        static void Free();
        static ParseThread* Get();
    };

    // ClientData is set to wxString* which must be deleted by the handler
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_MESSAGE, wxCommandEvent);
    // ClientData is set to std::set<std::string> *newSet which must deleted by the handler
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_RETAGGING_PROGRESS, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_RETAGGING_COMPLETED, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_INCLUDE_STATEMENTS_DONE, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_READY, wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_PARSE_THREAD_SOURCE_TAGS, gsgs::Event);
}
#endif 
