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
#ifndef _GSGS_COMMENT_H
#define _GSGS_COMMENT_H

#include "gsgsPreInclude.h"
#include "gsgsTag.h"
#include "gsgsSerializer.h"

namespace gsgs
{
    class _gsgsAPI CommentConfigData : public ObjectData
    {
        bool m_addStarOnCComment = true;
        bool m_continueCppComment = false;
        bool m_autoInsert = true;
        bool m_useQtStyle = false;
        wxString m_classPattern;
        wxString m_functionPattern;

    public:
        CommentConfigData();
        virtual ~CommentConfigData();

    public:
        virtual void DeSerialize(Serializer& arch);
        virtual void Serialize(Serializer& arch);

        // Setters
        void SetAddStarOnCComment(const bool& addStarOnCComment) { this->m_addStarOnCComment = addStarOnCComment; }
        void SetClassPattern(const wxString& classPattern) { this->m_classPattern = classPattern; }
        void SetContinueCppComment(const bool& continueCppComment) { this->m_continueCppComment = continueCppComment; }
        void SetFunctionPattern(const wxString& functionPattern) { this->m_functionPattern = functionPattern; }

        const bool& GetAddStarOnCComment() const { return m_addStarOnCComment; }
        const wxString& GetClassPattern() const { return m_classPattern; }
        const bool& GetContinueCppComment() const { return m_continueCppComment; }
        const wxString& GetFunctionPattern() const { return m_functionPattern; }
        void SetUseQtStyle(bool useQtStyle) { this->m_useQtStyle = useQtStyle; }
        bool IsUseQtStyle() const { return m_useQtStyle; }
        void SetAutoInsert(bool b) { this->m_autoInsert = b; }
        bool IsAutoInsert() const { return m_autoInsert; }
        wxString GetCommentBlockPrefix() const { return IsUseQtStyle() ? wxString("/*!") : wxString("/**"); }
    };

    class _gsgsAPI CommentParseResult
    {
    public:
        typedef SharedPtr<CommentParseResult> Ptr_t;
    public:
        void addComment(const std::string& comment, size_t line, bool cppComment)
        {
            // try to group this comment with the one above it
            std::string cmt = comment;
            if(cppComment && line) {
                size_t prevLine = line - 1;
                std::map<size_t, std::string>::iterator iter = m_comments.find(prevLine);
                if(iter != m_comments.end()) {
                    cmt = iter->second;
                    cmt += "\n";
                    cmt += comment;

                    // remove the previous comment from the map
                    m_comments.erase(iter);
                }
            }
            m_comments[line] = cmt;
        }

        std::string getCommentForLine(size_t line) const
        {
            std::map<size_t, std::string>::const_iterator iter = m_comments.find(line);
            if(iter == m_comments.end()) return "";
            return iter->second;
        }

        void print()
        {
            std::map<size_t, std::string>::const_iterator iter = m_comments.begin();
            for(; iter != m_comments.end(); iter++) {
                printf("Line   : %u\n", (unsigned int)iter->first);
                printf("Comment:\n%s\n", iter->second.c_str());
            }
        }

        void setFilename(const std::string& filename) { m_filename = filename; }

        const std::string& getFilename() const { return m_filename; }

        void clear()
        {
            m_comments.clear();
            m_filename.clear();
        }
    private:
        std::map<size_t, std::string> m_comments;
        std::string m_filename;
    };

    extern _gsgsAPI int ParseComments(const char* filePath, CommentParseResult& comments);

    class _gsgsAPI CommentCreator
    {
    protected:
        wxChar m_keyPrefix;
        
    public:
        CommentCreator(wxChar keyPrefix = wxT('\\')) : m_keyPrefix(keyPrefix) {}
        virtual ~CommentCreator(){}
        virtual wxString CreateComment() = 0;
    };

    class _gsgsAPI CppCommentCreator : public CommentCreator
    {
        TagEntryPtr m_tag;
    public:
        CppCommentCreator(TagEntryPtr tag, wxChar keyPrefix);
        virtual ~CppCommentCreator();
        virtual wxString CreateComment();

    private:
        wxString FunctionComment();
    };
}
#endif
