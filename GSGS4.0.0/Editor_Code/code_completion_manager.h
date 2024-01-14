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

#ifndef CODECOMPLETIONMANAGER_H
#define CODECOMPLETIONMANAGER_H

#include "gsgsPreInclude.h"
#include "project.h"
#include "gsgsEditor.h"
#include "gsgsEvent.h"
#include "gsgsThread.h"
#include "gsgsCommonHandler.h"
#include <wx/wxsqlite3.h>
#include <thread>

namespace gsgs
{
    class CxxUsingNamespaceCollectorThread : public WorkerThread
    {
    public:
        struct Request : public ThreadRequest {
            wxString filename;
            wxArrayString includePaths;
            Request() {}
        };

    public:
        CxxUsingNamespaceCollectorThread();
        virtual ~CxxUsingNamespaceCollectorThread();

        virtual void ProcessRequest(ThreadRequest* request);
    public:
        void QueueFile(const wxString &filename, const wxArrayString &searchPaths);
    };

    class CxxPreProcessorThread : public WorkerThread
    {
    public:
        struct Request : public ThreadRequest
        {
            wxString filename;
            wxArrayString definitions;
            wxArrayString includePaths;

            Request()
            {
            }
        };

    public:
        CxxPreProcessorThread();
        virtual ~CxxPreProcessorThread();

    public:
        virtual void ProcessRequest(ThreadRequest* request);

        void QueueFile(const wxString& filename, const wxArrayString& definitions, const wxArrayString& includePaths);
    };
    
    class _gsgsAPI CxxPreProcessorCache
    {
        struct CacheEntry {
            wxString filename;
            wxString project;
            wxString config;
            wxString preamble;
            wxArrayString definitions;
            time_t lastUpdated;
            CacheEntry()
                : lastUpdated(0)
            {
            }
        };

        typedef std::map<wxString, CacheEntry> Map_t;
        CxxPreProcessorCache::Map_t m_impl;

    public:
        CxxPreProcessorCache();
        virtual ~CxxPreProcessorCache();

        /**
         * @brief return the preamble for a give file
         * A Preamble of a file is a list of all include statements
         * used in the file
         */
        wxString GetPreamble(const wxString& filename) const;

        /**
         * @brief clear the cache content
         */
        void Clear();

        /**
         * @brief locate Pre Processors for a given file in a specific configuraion and project
         */
        bool Find(const wxString& filename, const wxString& project, const wxString& config, wxArrayString& definitions);

        /**
         * @brief insert item to the cache
         */
        bool
        Insert(const wxString& filename, const wxString& project, const wxString& config, const wxArrayString& definitions);
    };

    class CodeCompletionManager : public ServiceProvider
    {
    public:
        CodeCompletionManager();
        virtual ~CodeCompletionManager();
       
        /**
         * @brief force a refresh based on the current settings
         */
        void RefreshPreProcessorColouring();

        // Callback for collecting macros completed
        void OnParseThreadCollectedMacros(const wxArrayString & definitions, const wxString & filename);

        // Callback for collecting 'using namespaces' completed
        void OnFindUsingNamespaceDone(const wxArrayString & usingNamespace, const wxString & filename);

        void SetWordCompletionRefreshNeeded(bool wordCompletionRefreshNeeded)
        {
            this->m_wordCompletionRefreshNeeded = wordCompletionRefreshNeeded;
        }
        inline bool GetWordCompletionRefreshNeeded() const { return m_wordCompletionRefreshNeeded; }
        inline void SetOptions(size_t options) { this->m_options = options; }
        inline size_t GetOptions() const { return m_options; }

        static CodeCompletionManager& Get();
        static void Release();

        bool WordCompletion(Editor* editor, const wxString& expr, const wxString& word);
        bool Calltip(Editor* editor, int line, const wxString& expr, const wxString& text, const wxString& word);
        bool CodeComplete(Editor* editor, int line, const wxString& expr, const wxString& text);
        void ProcessMacros(Editor* editor);
        void ProcessUsingNamespace(Editor* editor);
        void GotoImpl(Editor* editor);
        void GotoDecl(Editor* editor);
        bool GetDefinitionsAndSearchPaths(Editor* editor, wxArrayString& searchPaths, wxArrayString& definitions);
        void UpdateParserPaths();
    protected:
        /// ctags implementions
        bool DoCtagsWordCompletion(Editor* editor, const wxString& expr, const wxString& word);
        bool DoCtagsCalltip(Editor* editor, int line, const wxString& expr, const wxString& text, const wxString& word);
        bool DoCtagsCodeComplete(Editor* editor, int line, const wxString& expr, const wxString& text);
        bool DoCtagsGotoImpl(Editor* editor);
        bool DoCtagsGotoDecl(Editor* editor);

        void DoUpdateOptions();
        void DoProcessCompileCommands();
        static void ThreadProcessCompileCommandsEntry(CodeCompletionManager* owner, const wxString& rootFolder);
        void CompileCommandsFileProcessed(const wxArrayString& includePaths);
        size_t CreateBlockCommentKeywordsList(wxCodeCompletionBoxEntry::Vec_t& entries) const;
    protected:
        // Event handlers
        void OnBuildEnded(BuildEvent& e);
        void OnFilesAdded(Event& e);
        void OnWorkspaceLoaded(wxCommandEvent& e);

        void OnBuildStarted(BuildEvent& e);
        void OnAppActivated(wxActivateEvent& e);
        void OnCompileCommandsFileGenerated(Event& event);
        void OnFileSaved(Event& event);
        void OnFileLoaded(Event& event);
        void OnWorkspaceConfig(wxCommandEvent& event);
        void OnWorkspaceClosed(wxCommandEvent& event);
        void OnEnvironmentVariablesModified(Event& event);
        void OnBlockCommentCodeComplete(CodeCompleteEvent& event);
        void OnBlockCommentWordComplete(CodeCompleteEvent& event);

        // Code completion handlers
        void OnCodeCompletion(CodeCompleteEvent& event);
        void OnWordCompletion(CodeCompleteEvent& event);
        void OnFindSymbol(CodeCompleteEvent& event);
        void OnFindDecl(CodeCompleteEvent& event);
        void OnFindImpl(CodeCompleteEvent& event);
        void OnFunctionCalltip(CodeCompleteEvent& event);
        void OnTypeInfoToolTip(CodeCompleteEvent& event);
    protected:
        size_t m_options;
        CxxPreProcessorThread m_preProcessorThread;
        CxxUsingNamespaceCollectorThread m_usingNamespaceThread;
        std::thread* m_compileCommandsThread = nullptr;
        wxFileName m_compileCommands;
        time_t m_compileCommandsLastModified = 0;
        CompileCommandsGenerator::Ptr_t m_compileCommandsGenerator;
        bool m_wordCompletionRefreshNeeded;
        bool m_buildInProgress;
    };
}
#endif
