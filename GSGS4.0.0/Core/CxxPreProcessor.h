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

#ifndef CXXPREPROCESSOR_H
#define CXXPREPROCESSOR_H

#include "gsgsPreInclude.h"
#include "CxxLexerAPI.h"
#include "CxxPreProcessorScanner.h"

namespace gsgs
{
    class _gsgsAPI CxxPreProcessor
    {
        CxxPreProcessorToken::Map_t m_tokens;
        wxArrayString m_includePaths;
        std::set<wxString> m_noSuchFiles;
        std::map<wxString, wxString> m_fileMapping;
        size_t m_options;
        int m_maxDepth;
        int m_currentDepth;

    public:
        CxxPreProcessor();
        virtual ~CxxPreProcessor();

        void SetMaxDepth(int maxDepth) { this->m_maxDepth = maxDepth; }
        int GetMaxDepth() const { return m_maxDepth; }
        void SetCurrentDepth(int currentDepth) { this->m_currentDepth = currentDepth; }
        void SetFileMapping(const std::map<wxString, wxString>& fileMapping) { this->m_fileMapping = fileMapping; }
        int GetCurrentDepth() const { return m_currentDepth; }
        const std::map<wxString, wxString>& GetFileMapping() const { return m_fileMapping; }
        void SetIncludePaths(const wxArrayString& includePaths);
        const wxArrayString& GetIncludePaths() const { return m_includePaths; }

        void SetOptions(size_t options) { this->m_options = options; }
        size_t GetOptions() const { return m_options; }
        /**
         * @brief return a command that generates a single file with all defines in it
         */
        wxString GetGxxCommand(const wxString& gxx, const wxString& filename) const;
        CxxPreProcessorToken::Map_t& GetTokens() { return m_tokens; }

        const CxxPreProcessorToken::Map_t& GetTokens() const { return m_tokens; }
        void SetTokens(const CxxPreProcessorToken::Map_t& tokens) { m_tokens = tokens; }

        /**
         * @brief add search path to the PreProcessor
         */
        void AddIncludePath(const wxString& path);

        /**
         * @brief add definition in the form of A[=B] (=B is optional)
         */
        void AddDefinition(const wxString& def);
        /**
         * @brief resolve an include statement and return the full path for it
         * @param currentFile the current file being processed. ( we use it's path to resolve relative include
         * statements)
         * @param includeStatement the include statement found by the scanner
         * @param outFile [output]
         */
        bool ExpandInclude(const wxFileName& currentFile, const wxString& includeStatement, wxFileName& outFile);
        /**
         * @brief the main entry function
         * @param filename
         */
        void Parse(const wxFileName& filename, size_t options);

        /**
         * @brief return the definitions collected as an array
         * @return
         */
        wxArrayString GetDefinitions() const;
        
        /**
         * @brief return true if we can open another include file or not (depends on the max depts set 
         * and the current depth)
         * @return 
         */
        bool CanGoDeeper() const;
        
        /**
         * @brief increase the current include depth
         */
        void IncDepth();
        
        /**
         * @brief decrease the current include depth
         */
        void DecDepth();
    };
}
#endif