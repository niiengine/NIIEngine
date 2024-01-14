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

#ifndef MARKUP_H__
#define MARKUP_H__

#include "gsgsPreInclude.h"
#include <wx/regex.h>

#define MARKUP_VOID     269
#define BOLD_START      270
#define BOLD_END        271
#define HORIZONTAL_LINE 272
#define NEW_LINE        273
#define CODE_START      274
#define CODE_END        275
#define COLOR_START     276
#define COLOR_END       277
#define ITALIC_START    278
#define ITALIC_END      279
#define LINK_START      280
#define LINK_END        281
#define LINK_URL        282

class _gsgsAPI MarkupSearchPattern
{
public:
    typedef std::list<MarkupSearchPattern> List_t;

public:
    MarkupSearchPattern() : m_isRegex(false), m_type(0), m_regex(NULL) {}
    MarkupSearchPattern(const wxString &search, int type, bool isRegex = false, int matchIndex = 0);
    virtual ~MarkupSearchPattern();
    bool Match(wxString &inString, int& type, wxString &matchString) ;
protected:
    wxString             m_pattern;
    bool                 m_isRegex;
    int                  m_type;
    wxSharedPtr<wxRegEx> m_regex;
    int                  m_matchIndex;
};

class _gsgsAPI MarkupParser
{
public:
    MarkupParser(const wxString &tip);
    virtual ~MarkupParser() {}
    bool Next();
    
    const wxString& GetToken() const {
        return m_token;
    }
    int GetType() const {
        return m_type;
    }
protected:
    bool IsMatchPattern(wxString &match, int &type);
private:
    MarkupSearchPattern::List_t m_patterns;
    wxString                    m_tip;
    wxString                    m_token;
    int                         m_type;
};

#endif // MARKUP_H__

