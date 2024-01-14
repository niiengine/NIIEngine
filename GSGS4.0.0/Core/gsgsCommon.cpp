
#include "gsgsPreProcess.h"
#include "gsgsCommon.h"
#include <iostream>

#define STATE_NORMAL 0
#define STATE_IN_SINGLE_QUOTES 1
#define STATE_IN_DOUBLE_QUOTES 2

#define BOOL_TO_STR(b) b ? "true" : "false"

namespace gsgs
{
    CmdParser::CmdParser(const wxString & str, size_t flags)
        : m_commandline(str)
        , m_flags(flags)
    {
        DoParse();
    }

    CmdParser::~CmdParser() {}

    #define PUSH_TOKEN()                                    \
        if(!curtoken.IsEmpty()) { m_tokens.Add(curtoken); } \
        curtoken.Clear();

    #define HANDLE_NEWLINE() \
        if(!(m_flags & kIgnoreNewLines)) { curtoken << ch; }

    void CmdParser::DoParse()
    {
        wxString tmpstr = m_commandline;
        wxString curtoken;

        int state = STATE_NORMAL;
        for (size_t i = 0; i < tmpstr.length(); ++i) {
            wxChar ch = tmpstr.at(i);
            switch (state) {
            case STATE_NORMAL: {
                switch (ch) {
                case '\n':
                case '\r':
                    HANDLE_NEWLINE();
                    break;
                case ' ':
                case '\t':
                    // whitespace, start new token
                    PUSH_TOKEN();
                    break;
                case '"':
                    PUSH_TOKEN();
                    state = STATE_IN_DOUBLE_QUOTES;
                    break;
                case '\'':
                    PUSH_TOKEN();
                    state = STATE_IN_SINGLE_QUOTES;
                    break;
                default: {
                    // Normal
                    curtoken << ch;
                    break;
                }
                }
                break;
            }
            case STATE_IN_DOUBLE_QUOTES:
                if (ch == '"') {
                    PUSH_TOKEN();
                    state = STATE_NORMAL;
                }
                else {
                    curtoken << ch;
                }
                break;
            case STATE_IN_SINGLE_QUOTES:
                if (ch == '\'') {
                    PUSH_TOKEN();
                    state = STATE_NORMAL;
                }
                else {
                    curtoken << ch;
                }
                break;
            }
        }
        PUSH_TOKEN();
    }

    wxFont FontHelper::FromString(const wxString& str)
    {
        wxArrayString parts = ::wxStringTokenize(str, ";", wxTOKEN_STRTOK);
        if(parts.size() != 5) return wxNullFont;

        long iPointSize, iFamily, iWeight, iStyle;
        wxString facename = parts.Item(0);
        parts.Item(1).ToCLong(&iPointSize);
        parts.Item(2).ToCLong(&iFamily);
        parts.Item(3).ToCLong(&iWeight);
        parts.Item(4).ToCLong(&iStyle);

        bool bold = (iWeight == wxFONTWEIGHT_BOLD);
        bool italic = (iStyle == wxFONTSTYLE_ITALIC);
        wxFont font(wxFontInfo(iPointSize).Bold(bold).Italic(italic).FaceName(facename).Family((wxFontFamily)iFamily));
        return font;
    }

    wxString FontHelper::ToString(const wxFont& font)
    {
        if(!font.IsOk()) {
            return "";
        }
        wxString str;
        str << font.GetFaceName() << ";" << font.GetPointSize() << ";" << (int)font.GetFamily() << ";"
            << (int)font.GetWeight() << ";" << (int)font.GetStyle();
        return str;
    }

    Function::Function() { Reset(); }

    Function::~Function() {}

    void Function::Reset()
    {
        m_name = "";
        m_scope = "";
        m_returnValue.Reset();
        m_name = "";
        m_signature = "";
        m_lineno = 0;
        m_retrunValusConst = "";
        m_isVirtual = false;
        m_isPureVirtual = false;
        m_isConst = false;
    }

    void Function::Print()
    {
        fprintf(stdout,
            "{m_name=%s, m_isConst=%s, m_lineno=%d, m_scope=%s, m_signature=%s, m_isVirtual=%s, m_isPureVirtual=%s, "
            "m_retrunValusConst=%s, m_throws=%s\nm_returnValue=",
            m_name.c_str(),
            m_isConst ? "yes" : "no",
            m_lineno,
            m_scope.c_str(),
            m_signature.c_str(),
            m_isVirtual ? "yes" : "no",
            m_isPureVirtual ? "yes" : "no",
            m_retrunValusConst.c_str(),
            m_throws.c_str());

        m_returnValue.Print();
        fprintf(stdout, "}\n");
        fflush(stdout);
    }

    Variable::Variable()
    {
        Reset();
    }

    Variable::~Variable()
    {
    }

    Variable::Variable(const Variable &src)
    {
        *this = src;
    }

    Variable & Variable::operator =(const Variable &src)
    {
        m_type          = src.m_type;
        m_templateDecl  = src.m_templateDecl;
        m_name          = src.m_name;
        m_isTemplate    = src.m_isTemplate;
        m_isPtr         = src.m_isPtr;
        m_typeScope     = src.m_typeScope;
        m_pattern       = src.m_pattern;
        m_starAmp       = src.m_starAmp;
        m_lineno        = src.m_lineno;
        m_isConst       = src.m_isConst;
        m_defaultValue  = src.m_defaultValue;
        m_arrayBrackets = src.m_arrayBrackets;
        m_isEllipsis    = src.m_isEllipsis;
        m_isBasicType   = src.m_isBasicType;
        m_rightSideConst= src.m_rightSideConst;
        m_completeType  = src.m_completeType;
        m_isVolatile    = src.m_isVolatile;
        m_isAuto        = src.m_isAuto;
        m_enumInTypeDecl= src.m_enumInTypeDecl;
        return *this;
    }

    void Variable::Reset()
    {
        m_type = "";
        m_templateDecl = "";
        m_name = "";
        m_isTemplate = false;
        m_isPtr = false;
        m_typeScope = "";
        m_pattern = "";
        m_starAmp = "";
        m_lineno = 0;
        m_isConst = false;
        m_defaultValue = "";
        m_arrayBrackets = "";
        m_isEllipsis = false;
        m_isBasicType = false;
        m_rightSideConst = "";
        m_completeType = "";
        m_isVolatile = false;
        m_isAuto = false;
        m_enumInTypeDecl = false;
    }

    void Variable::Print()
    {
        std::cout << "------------------" << "\n"
                  << "m_name           :" << m_name.c_str() << "\n"
                  << "m_defaultValue   :" << m_defaultValue.c_str() << "\n"
                  << "m_lineno         :" << m_lineno << "\n"
                  << "m_starAmp        :" << m_starAmp.c_str() << "\n"
                  << "m_type           :" << m_type.c_str() << "\n"
                  << "m_isConst        :" << m_isConst << "\n"
                  << "m_rightSideConst :" << m_rightSideConst.c_str() << "\n"
                  << "m_typeScope      :" << m_typeScope.c_str() << "\n"
                  << "m_templateDecl   :" << m_templateDecl.c_str() << "\n"
                  << "m_arrayBrackets  :" << m_arrayBrackets.c_str() << "\n"
                  << "m_isPtr          :" << m_isPtr << "\n"
                  << "m_isTemplate     :" << m_isTemplate << "\n"
                  << "m_isEllips       :" << m_isEllipsis << "\n"
                  << "m_isBasicType    :" << m_isBasicType << "\n"
                  << "m_pattern        :" << m_pattern.c_str() << "\n"
                  << "m_completeType   :" << m_completeType.c_str() << "\n"
                  << "m_isVolatile     :" << m_isVolatile << "\n"
                  << "m_isAuto         :" << m_isAuto << "\n"
                  << "m_enumInTypeDecl :" << m_enumInTypeDecl << "\n";
    }

    Comment::Comment(const Comment& o)
    {
        *this = o;
    }

    Comment::Comment(const wxString & comment, const wxString & file, const int line)
            : m_comment( comment )
            , m_file( file )
            , m_line( line )
    {
        m_comment.erase(m_comment.find_last_not_of(_T("\n\r\t"))+1);
    }

    Comment & Comment::operator=(const Comment &o)
    {
        if ( this == &o )
            return *this;

        m_comment = o.m_comment;
        m_file    = o.m_file;
        m_line    = o.m_line;
        return *this;
    }

    ExpressionResult::ExpressionResult()
    {
        Reset();
    }

    ExpressionResult::~ExpressionResult()
    {
    }

    void ExpressionResult::Print()
    {
        printf("%s\n", ToString().c_str());
    }

    std::string ExpressionResult::ToString() const
    {
        char tmp[256];
        sprintf(tmp, "{m_name:%s, m_isFunc:%s, m_isTemplate:%s, m_isThis:%s, m_isaType:%s, m_isPtr:%s, m_scope:%s, m_templateInitList:%s}", 
                    m_name.c_str(), 
                    BOOL_TO_STR(m_isFunc), 
                    BOOL_TO_STR(m_isTemplate),
                    BOOL_TO_STR(m_isThis),
                    BOOL_TO_STR(m_isaType),
                    BOOL_TO_STR(m_isPtr),
                    m_scope.c_str(),
                    m_templateInitList.c_str());
        return tmp;
    }

    void ExpressionResult::Reset()
    {
        m_isFunc = false;
        m_name = "";
        m_isThis = false;
        m_isaType = false;
        m_isPtr = false;
        m_scope = "";
        m_isTemplate = false;
        m_isGlobalScope = false;
        m_templateInitList = "";
    }
}