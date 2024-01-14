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

#ifndef _GSGS_COMMON_H
#define _GSGS_COMMON_H

#include "gsgsPreInclude.h"
#include "gsgsSharedPtr.h"
#include <wx/font.h>

#define YY_NEVER_INTERACTIVE 1
#define CURRENT_GCC_VERSION ((__GNUC__*1000)+(__GNUC_MINOR__*100))
#define GCC_VERSION(major, minor) ((major*1000)+(minor*100))

#if CURRENT_GCC_VERSION < GCC_VERSION(4,9)
#include <sstream>
namespace std {
    template<typename T>
    std::string to_string(T x) {
        std::stringstream ss;
        ss << x;
        return ss.str();
    }
}
#endif

#if defined(_WIN32) && !defined(_WIN64)
#define fileno(fp) (fp->_file)
#define strdup _strdup
#endif

namespace gsgs
{
    class _gsgsAPI FontHelper
    {
    public:
        static wxString ToString(const wxFont& font);
        static wxFont FromString(const wxString& str);
    };

    class _gsgsAPI ExpressionResult
    {
    public:
        ExpressionResult();
        virtual ~ExpressionResult();
        void Reset();
        void Print();
        std::string ToString() const;
    public:
        bool        m_isFunc;
        std::string m_name;
        bool        m_isThis;
        bool        m_isaType;
        bool        m_isPtr;
        std::string m_scope;
        bool        m_isTemplate;
        std::string m_templateInitList;
        bool        m_isGlobalScope;
    };

    class Comment
    {
    public:
        Comment(const wxString & comment, const wxString & file, const int line);

        Comment(const Comment & o);

        virtual ~Comment() {};

        /// asgginment operator
        Comment& operator=(const Comment & o);

        
        inline const wxString & GetFile() const {
            return m_file;
        }

        inline const wxString & GetComment() const {
            return m_comment;
        }

        inline const int & GetLine() const {
            return m_line;
        }
    private:
        wxString m_comment;
        wxString m_file;
        int      m_line;
    };

    typedef SharedPtr<Comment> CommentPtr;

    class _gsgsAPI Variable
    {
    public:
        std::string     m_name;
        bool            m_isTemplate;
        std::string     m_templateDecl;
        bool            m_isPtr;
        std::string     m_type; //as in 'int a;' -> type=int
        std::string     m_typeScope;//as in 'std::string a;' -> typeScope = std, type=string
        std::string     m_pattern;
        std::string     m_completeType;
        std::string     m_starAmp;
        int             m_lineno;
        bool            m_isConst;
        std::string     m_rightSideConst;
        std::string     m_defaultValue;	// used mainly for function arguments with default values foo(int = 0);
        std::string     m_arrayBrackets;
        bool            m_isEllipsis;
        bool            m_isBasicType;
        bool            m_isVolatile;
        bool            m_isAuto;
        bool            m_enumInTypeDecl; // e.g. enum MyENum e

    public:
        Variable();
        virtual ~Variable();

        //copy ctor
        Variable(const Variable& src);

        //operator =
        Variable& operator=(const Variable& src);

        //clear the class content
        void Reset();

        //print the variable to stdout
        void Print();
    };

    typedef std::list<Variable> VariableList;

    class _gsgsAPI Function
    {
    public:
        Function();
        virtual ~Function();

        // clear the class content
        void Reset();

        // print the variable to stdout
        void Print();
    public:
        std::string m_name;
        std::string m_scope; // functions' scope
        std::string m_retrunValusConst; // is the return value a const?
        std::string m_signature;
        Variable m_returnValue;
        int m_lineno;
        bool m_isVirtual;
        bool m_isPureVirtual;
        bool m_isConst;
        std::string m_throws;
    };

    typedef std::list<Function> FunctionList;

    class _gsgsAPI DirSaver
    {
    public:
        DirSaver() { m_curDir = wxGetCwd(); }

        virtual ~DirSaver() { wxSetWorkingDirectory(m_curDir); }
    private:
        wxString m_curDir;
    };

    class BrowseRecord
    {
    public:
        BrowseRecord(): 
            filename(wxEmptyString), 
            project(wxEmptyString), 
            lineno(wxNOT_FOUND), 
            position(wxNOT_FOUND), 
            firstLineInView(wxNOT_FOUND){}

        ~BrowseRecord() {}
    public:
        wxString filename;
        wxString project;
        int lineno;
        int position;
        int firstLineInView;
    };

    class _gsgsAPI Typedef
    {
    public:
        Typedef()  {}
        ~Typedef() {}
        
        void print() {
            printf("Name: %s\n", m_name.c_str());
            m_realType.Print();
        }
        
        void clear() {
            m_realType.Reset();
            m_name.clear();
        }
    public:
        std::string m_name;
        Variable    m_realType;
    };

    typedef std::list<Typedef> TypedefList;

    class _gsgsAPI IncludeStatement {
    public:
        std::string file;
        int         line;
        std::string includedFrom;
        std::string pattern;
    };
    typedef std::vector<IncludeStatement> IStatementList;
    extern _gsgsAPI int IncludeFinder( const char* filePath, IStatementList &includes );

    class _gsgsAPI CmdParser
    {
    public:
        enum {
            // ignore both \r and \n
            kIgnoreNewLines = 0x00000001,
        };
    public:
        CmdParser(const wxString& str, size_t flags = kIgnoreNewLines);
        virtual ~CmdParser();

        const wxArrayString & ToArray() const { return m_tokens; }
    protected:
        void DoParse();
    private:
        wxString m_commandline;
        size_t m_flags;
        wxArrayString m_tokens;
    };
}

#endif
