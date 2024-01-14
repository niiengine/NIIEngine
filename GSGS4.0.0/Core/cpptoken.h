
#ifndef __cpptoken__
#define __cpptoken__

#include "gsgsPreInclude.h"
#include <wx/wxsqlite3.h>
#include "gsgsJob.h"

namespace gsgs
{
    typedef std::vector<wxFileName> wxFileList_t;
    class _gsgsAPI CppToken
    {
        int m_id;
        wxString name; // the name of the token
        size_t offset; // file offset
        wxString filename;
        size_t lineNumber;

    public:
        typedef std::vector<CppToken> Vec_t;

        class wxFileNameToString
        {
        public:
            wxString operator()(const wxFileName& fn) { return fn.GetFullPath(); }
        };

        // Required by later versions of clang compiler
        bool operator<(const CppToken& rhs) const;

        /**
         * @class Predicate
         * a helper class that can be used to filter tokens using std:remove_if
         * this Predicate filters files that do not exist in m_files member
         */
        class RemoveIfNotIn
        {
            std::unordered_set<wxString> m_files;

        public:
            RemoveIfNotIn(const wxFileList_t& fileList)
            {

                // first convert vector<wxFileName> to vector<wxString>
                std::vector<wxString> v;
                v.resize(fileList.size());
                std::transform(fileList.begin(), fileList.end(), v.begin(), wxFileNameToString());

                // construct a set based on the vector content
                m_files = std::unordered_set<wxString>(v.begin(), v.end());
            }

            RemoveIfNotIn(const std::unordered_set<wxString>& filesSet)
                : m_files(filesSet)
            {
            }

            /**
             * @brief return true if the token's file does _not_ exists in the
             * m_files member (which was initialized while constructing the Predicate)
             */
            bool operator()(const CppToken& token) { return m_files.count(token.getFilename()) == 0; }
        };

    public:
        CppToken();
        CppToken(wxSQLite3ResultSet& res);
        ~CppToken();

        void reset();
        void append(wxChar ch);

        /**
         * @brief store the token into the datbase.
         * @param db database pointer
         * @param fileId the file ID for this token
         */
        int store(wxSQLite3Database* db, wxLongLong fileId) const;

        /**
         * @brief load tokens from the cache by file/name
         * @param db
         * @param name
         * @param file_name
         * @return
         */
        static CppToken::Vec_t loadByNameAndFile(wxSQLite3Database* db, const wxString& name, wxLongLong fileID);
        /**
         * @brief
         * @param db
         * @param name
         * @return
         */
        static CppToken::Vec_t loadByName(wxSQLite3Database* db, const wxString& name);

        void setName(const wxString& name) { this->name = name; }

        void setOffset(const size_t& offset) { this->offset = offset; }

        const wxString& getName() const { return name; }
        const size_t& getOffset() const { return offset; }

        void setFilename(const wxString& filename) { this->filename = filename; }

        const wxString& getFilename() const { return filename; }

        void setId(const int& id) { this->m_id = id; }

        const int& getId() const { return m_id; }

        size_t getLineNumber() const { return lineNumber; }

        void setLineNumber(size_t lineNo) { lineNumber = lineNo; }

        void print();

        // Provide lower-than operator so we can use std::vector::sort method
        bool operator<(const CppToken& rhs) { return filename < rhs.filename; }
    };

    class _gsgsAPI CppTokensMap
    {
        std::unordered_map<wxString, std::vector<CppToken>*> m_tokens;

    public:
        CppTokensMap();
        ~CppTokensMap();

        /**
         * @brief return true if any token with given name exists in the map
         * @param name token's name to search
         */
        bool contains(const wxString& name);
        /**
         * @brief return list of tokens with given name
         * @param name token name
         * @param tokens [output]
         */
        void findTokens(const wxString& name, std::vector<CppToken>& tokens);
        /**
         * @brief add token to the map. if token with same name already exists, it will be appended so multiple tokens with
         * same name is allowed
         * @param token token to add
         */
        void addToken(const CppToken& token);

        /**
         * @brief add list of tokens for a given name to the map
         */
        void addToken(const wxString& name, const CppToken::Vec_t& list);

        /**
         * @brief clear all token
         */
        void clear();

        /**
         * @brief return true if no tokens were found
         */
        bool is_empty();
    };

    class _gsgsAPI RefactorIndexBuildJob : public Job
    {
        std::vector<wxFileName> m_files;
        wxString m_word;

    public:
        RefactorIndexBuildJob(const std::vector<wxFileName> &files, const wxChar* word);
        virtual ~RefactorIndexBuildJob();
        void Process(wxThread *thread);

    public:
        void Parse(const wxString &word, gsgs::CppTokensMap &l);
    };
}
#endif