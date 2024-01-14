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

#ifndef CppLexerAPI_H__
#define CppLexerAPI_H__

#include "gsgsPreInclude.h"
#include <wx/variant.h>

#if 0
#define DEBUGMSG wxPrintf
#define gsgs_Debug wxPrintf
#define gsgs_Debug1 wxPrintf
#else

#define DEBUGMSG(...) \
    do {              \
    } while(false)

#if 0
#define gsgs_Debug(...) \
    do {              \
    } while(false)

#define gsgs_Debug1(...) \
    do {               \
    } while(false)

#endif

#endif

enum eLexerOptions {
    /// Options
    kLexerOpt_None = 0,
    kLexerOpt_ReturnComments = (1 << 0),
    kLexerOpt_ReturnWhitespace = (1 << 1),
    kLexerOpt_CollectMacroValueNumbers = (1 << 2),
    kLexerOpt_DontCollectMacrosDefinedInThisFile = (1 << 3),
    /// states
    kLexerState_InPreProcessor = (1 << 10),
};

enum class eCxxStandard { kCxx03, kCxx11 };

struct _gsgsAPI CxxLexerException
{
    wxString message;
    CxxLexerException(const wxString& msg)
        : message(msg)
    {
    }
};

struct _gsgsAPI CxxLexerToken
{
private:
    int lineNumber;
    int column;
    char* text;
    int type;
    std::string comment;

private:
    bool m_owned;

private:
    void deleteText()
    {
        if(m_owned && text) {
            free(text);
        }
        m_owned = false;
        text = nullptr;
    }

public:
    void SetColumn(int column) { this->column = column; }
    void SetComment(const std::string& comment) { this->comment = comment; }
    void SetLineNumber(int lineNumber) { this->lineNumber = lineNumber; }
    void SetOwned(bool owned) { this->m_owned = owned; }
    void SetType(int type) { this->type = type; }
    int GetColumn() const { return column; }
    const std::string& GetComment() const { return comment; }
    int GetLineNumber() const { return lineNumber; }
    bool IsOwned() const { return m_owned; }
    const char* GetText() const { return text; }
    void SetText(char* p) { text = p; }
    wxString GetWXComment() const { return wxString(comment.c_str(), wxConvISO8859_1); }
    wxString GetWXString() const { return wxString(text, wxConvISO8859_1); }
    int GetType() const { return type; }
    
    CxxLexerToken()
        : lineNumber(0)
        , column(0)
        , text(NULL)
        , type(0)
        , m_owned(false)
    {
    }

    CxxLexerToken(int tokenType)
        : lineNumber(0)
        , column(0)
        , text(NULL)
        , type(tokenType)
        , m_owned(false)
    {
    }

    CxxLexerToken(const CxxLexerToken& other)
    {
        if(this == &other) return;
        *this = other;
    }

    CxxLexerToken& operator=(const CxxLexerToken& other)
    {
        deleteText();
        lineNumber = other.lineNumber;
        column = other.column;
        type = other.type;
        if(other.text) {
            m_owned = true;
#ifdef __WXMSW__
            text = _strdup(other.text);
#else
            text = strdup(other.text);
#endif
        }
        return *this;
    }

    ~CxxLexerToken() { deleteText(); }
    bool IsEOF() const { return type == 0; }

    typedef std::vector<CxxLexerToken> Vect_t;
    typedef std::list<CxxLexerToken> List_t;
};

struct _gsgsAPI CxxPreProcessorToken
{
    wxString name;
    wxString value;
    bool deleteOnExit;
    CxxPreProcessorToken()
        : deleteOnExit(false)
    {
    }
    typedef std::unordered_map<wxString, CxxPreProcessorToken> Map_t;
};
/**
 * @class CppLexerUserData
 */
struct _gsgsAPI CppLexerUserData
{
private:
    size_t m_flags;
    std::string m_comment;
    std::string m_rawStringLabel;
    int m_commentStartLine;
    int m_commentEndLine;
    FILE* m_currentPF;

public:
    void Clear()
    {
        if(m_currentPF) {
            ::fclose(m_currentPF);
            m_currentPF = NULL;
        }

        ClearComment();
        m_rawStringLabel.clear();
    }

    CppLexerUserData(size_t options)
        : m_flags(options)
        , m_commentStartLine(wxNOT_FOUND)
        , m_commentEndLine(wxNOT_FOUND)
        , m_currentPF(NULL)
    {
    }

    ~CppLexerUserData() { Clear(); }

    void SetCurrentPF(FILE* currentPF) { this->m_currentPF = currentPF; }
    /**
     * @brief do we collect comments?
     */
    bool IsCollectingComments() const { return m_flags & kLexerOpt_ReturnComments; }
    bool IsCollectingWhitespace() const { return m_flags & kLexerOpt_ReturnWhitespace; }
    bool IsInPreProcessorSection() const { return m_flags & kLexerState_InPreProcessor; }
    void SetPreProcessorSection(bool b)
    {
        b ? m_flags |= kLexerState_InPreProcessor : m_flags &= ~kLexerState_InPreProcessor;
    }

    //==--------------------
    // Comment management
    //==--------------------
    void AppendToComment(const std::string& str) { m_comment.append(str); }
    void AppendToComment(char ch) { m_comment.append(1, ch); }
    void SetCommentEndLine(int commentEndLine) { this->m_commentEndLine = commentEndLine; }
    void SetCommentStartLine(int commentStartLine) { this->m_commentStartLine = commentStartLine; }
    int GetCommentEndLine() const { return m_commentEndLine; }
    int GetCommentStartLine() const { return m_commentStartLine; }
    const std::string& GetComment() const { return m_comment; }
    bool HasComment() const { return !m_comment.empty(); }
    /**
     * @brief clear all info collected for the last comment
     */
    void ClearComment()
    {
        m_comment.clear();
        m_commentStartLine = wxNOT_FOUND;
        m_commentEndLine = wxNOT_FOUND;
    }
};

typedef void* Scanner_t;
/**
 * @brief create a new Lexer for a buffer
 */
_gsgsAPI Scanner_t LexerNew(const wxString& buffer, size_t options = kLexerOpt_None);

/**
 * @brief create a scanner for a given file name
 */
_gsgsAPI Scanner_t LexerNew(const wxFileName& filename, size_t options);
/**
 * @brief destroy the current lexer and perform cleanup
 */
_gsgsAPI void LexerDestroy(Scanner_t* scanner);

/**
 * @brief return the next token, its type, line number and columns
 */
_gsgsAPI bool LexerNext(Scanner_t scanner, CxxLexerToken& token);

/**
 * @brief unget the last token
 */
_gsgsAPI void LexerUnget(Scanner_t scanner);

/**
 * @brief return the current lexer token
 */
_gsgsAPI wxString LexerCurrentToken(Scanner_t scanner);

/**
 * @brief return the associated data with this scanner
 */
_gsgsAPI CppLexerUserData* LexerGetUserData(Scanner_t scanner);
#endif
