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

#ifndef XMLLexerAPI_H__
#define XMLLexerAPI_H__

#include <gsgsPreInclude.h>
#include <wx/variant.h>

struct _gsgsAPI XMLLexerToken {
    int lineNumber;
    int column;
    wxString text;
    int type;
    wxString comment;
    XMLLexerToken()
        : lineNumber(0)
        , column(0)
        , type(0)
    {
    }

    XMLLexerToken(int tokenType)
        : lineNumber(0)
        , column(0)
        , type(tokenType)
    {
    }

    void Clear()
    {
        lineNumber = 0;
        column = 0;
        type = 0;
        text.Clear();
    }
    typedef std::vector<XMLLexerToken> Vec_t;
};

/**
 * @class XMLLexerUserData
 */
struct _gsgsAPI XMLLexerUserData {
public:
    FILE* m_currentPF;
    void* parserData;

public:
    void Clear()
    {
        if(m_currentPF) {
            ::fclose(m_currentPF);
            m_currentPF = NULL;
        }
        parserData = NULL;
    }

    XMLLexerUserData()
        : m_currentPF(NULL)
        , parserData(NULL)
    {
    }

    ~XMLLexerUserData() { Clear(); }
    void SetCurrentPF(FILE* currentPF) { this->m_currentPF = currentPF; }
};

typedef void* XMLScanner_t;

/**
 * @brief create a new Lexer for a file content
 */
_gsgsAPI XMLScanner_t xmlLexerNew(const wxString& content);

/**
 * @brief create a scanner for a given file name
 */
_gsgsAPI XMLScanner_t xmlLexerNew(const wxFileName& filename);

/**
 * @brief destroy the current lexer and perform cleanup
 */
_gsgsAPI void xmlLexerDestroy(XMLScanner_t* scanner);

/**
 * @brief return the next token, its type, line number and columns
 */
_gsgsAPI bool xmlLexerNext(XMLScanner_t scanner, XMLLexerToken& token);

/**
 * @brief unget the last token
 */
_gsgsAPI void xmlLexerUnget(XMLScanner_t scanner);

/**
 * @brief return the current lexer token
 */
_gsgsAPI wxString xmlLexerCurrentToken(XMLScanner_t scanner);

/**
 * @brief return the associated data with this scanner
 */
_gsgsAPI XMLLexerUserData* xmlLexerGetUserData(XMLScanner_t scanner);

#endif
