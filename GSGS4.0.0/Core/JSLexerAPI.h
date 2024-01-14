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

#ifndef JSLexerAPI_H__
#define JSLexerAPI_H__

#include <gsgsPreInclude.h>
#include <wx/variant.h>

#define DEBUGMSG(...) \
    do {              \
    } while(false)

enum eJSLexerOptions {
    kJSLexerOpt_None = 0x00000000,
    kJSLexerOpt_ReturnComments = 0x00000001,
};

struct _gsgsAPI JSLexerException
{
    wxString message;
    JSLexerException(const wxString& msg)
        : message(msg)
    {
    }
};

struct _gsgsAPI JSLexerToken
{
    int lineNumber;
    int column;
    wxString text;
    int type;
    wxString comment;
    JSLexerToken()
        : lineNumber(0)
        , column(0)
        , type(0)
    {
    }
    
    JSLexerToken(int tokenType)
        : lineNumber(0)
        , column(0)
        , type(tokenType)
    {
    }
    
    void Clear() {
        lineNumber = 0;
        column = 0;
        type = 0;
        text.Clear();
    }
    typedef std::vector<JSLexerToken> Vec_t;
};

/**
 * @class JSLexerUserData
 */
struct _gsgsAPI JSLexerUserData
{
private:
    size_t m_flags;
    wxString m_comment;
    int m_commentStartLine;
    int m_commentEndLine;
    FILE* m_currentPF;
    
public:
    void* parserData;

public:
    void Clear()
    {
        if(m_currentPF) {
            ::fclose(m_currentPF);
            m_currentPF = NULL;
        }

        ClearComment();
        parserData = NULL;
    }

    JSLexerUserData(size_t options)
        : m_flags(options)
        , m_commentStartLine(wxNOT_FOUND)
        , m_commentEndLine(wxNOT_FOUND)
        , m_currentPF(NULL)
        , parserData(NULL)
    {
    }

    ~JSLexerUserData() { Clear(); }

    void SetCurrentPF(FILE* currentPF) { this->m_currentPF = currentPF; }
    /**
     * @brief do we collect comments?
     */
    bool IsCollectingComments() const { return m_flags & kJSLexerOpt_ReturnComments; }
    //==--------------------
    // Comment management
    //==--------------------
    void AppendToComment(const wxString& str) { m_comment << str; }
    void SetCommentEndLine(int commentEndLine) { this->m_commentEndLine = commentEndLine; }
    void SetCommentStartLine(int commentStartLine) { this->m_commentStartLine = commentStartLine; }
    int GetCommentEndLine() const { return m_commentEndLine; }
    int GetCommentStartLine() const { return m_commentStartLine; }
    const wxString& GetComment() const { return m_comment; }
    bool HasComment() const { return !m_comment.IsEmpty(); }
    /**
     * @brief clear all info collected for the last comment
     */
    void ClearComment()
    {
        m_comment.Clear();
        m_commentStartLine = wxNOT_FOUND;
        m_commentEndLine = wxNOT_FOUND;
    }
};

typedef void* JSScanner_t;
/**
 * @brief create a new Lexer for a file content
 */
_gsgsAPI JSScanner_t jsLexerNew(const wxString& content, size_t options = kJSLexerOpt_None);

/**
 * @brief create a scanner for a given file name
 */
_gsgsAPI JSScanner_t jsLexerNew(const wxFileName& filename, size_t options);
/**
 * @brief destroy the current lexer and perform cleanup
 */
_gsgsAPI void jsLexerDestroy(JSScanner_t* scanner);

/**
 * @brief return the next token, its type, line number and columns
 */
_gsgsAPI bool jsLexerNext(JSScanner_t scanner, JSLexerToken& token);

/**
 * @brief unget the last token
 */
_gsgsAPI void jsLexerUnget(JSScanner_t scanner);

/**
 * @brief return the current lexer token
 */
_gsgsAPI wxString jsLexerCurrentToken(JSScanner_t scanner);

/**
 * @brief return the associated data with this scanner
 */
_gsgsAPI JSLexerUserData* jsLexerGetUserData(JSScanner_t scanner);
 
#endif
