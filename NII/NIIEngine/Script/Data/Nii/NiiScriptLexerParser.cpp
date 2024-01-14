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

#include "NiiPreProcess.h"
#include "NiiScriptLexerParser.h"
#include "NiiException.h"
#include "NiiScriptCompiler.h"
#include "NiiStrConv.h"

#define __n_skipNewLines(i, iend, out) \
    while(i != end && (*i)->mType == STT_NewLine) \
        ++i; \
    out = i;

namespace NII
{
    /// 语标类型
    enum ScriptTokenType
    {
        STT_LBracket = 0,   // {
        STT_RBracket,       // }
        STT_LMBracket,      // [
        STT_RMBracket,      // ]
        STT_LSBracket,      // (
        STT_RSBracket,      // )
        STT_Colon,          // :
        STT_Space,          // \t SPACE \r
        STT_NewLine,        // \n
        STT_RefValue,       // $...
        STT_Text,           // ...
        STT_Quote,          // "*"
        STT_Unknow
    };

    /// 脚本语标
    struct ScriptToken
    {
        String lexeme;
        ScriptTokenType mType;
        String mFile;
        Nui32 mLineNo;
    };
    //------------------------------------------------------------------------------
    bool __ExcludeID(const String & name, ScriptNode * parent)
    {
        // Process the built-in name exclusions
        if(name == _T("emitter") || name == _T("affector"))
        {
            // emitters or affectors inside a particle_system are excluded
            while(parent && parent->mType == SNT_Object)
            {
                ObjectScriptNode * obj = reinterpret_cast<ObjectScriptNode *>(parent);
                if(obj->mClass == _T("particle_system"))
                    return true;
                parent = obj->mParent;
            }
            return false;
        }
        else if(name == _T("pass"))
        {
            // passes inside compositors are excluded
            while(parent && parent->mType == SNT_Object)
            {
                ObjectScriptNode * obj = reinterpret_cast<ObjectScriptNode *>(parent);
                if(obj->mClass == _T("compositor"))
                    return true;
                parent = obj->mParent;
            }
            return false;
        }
        else if(name == _T("texture_source"))
        {
            // Parent must be texture_unit
            while(parent && parent->mType == SNT_Object)
            {
                ObjectScriptNode * obj = reinterpret_cast<ObjectScriptNode *>(parent);
                if(obj->mClass == _T("texture_unit"))
                    return true;
                parent = obj->mParent;
            }
            return false;
        }
        return false;
    }
    //------------------------------------------------------------------------------
    ScriptLexerParser::ScriptLexerParser(ScriptCompiler * compiler, const String & file) :
        mCompiler(compiler),
        mFileName(file),
        mCurrentProcess(0)
    {
    }
    //------------------------------------------------------------------------------
    ScriptLexerParser::~ScriptLexerParser()
    {
        ScriptTokenList::iterator i, iend = mScriptTokenList.end();
        for(i = mScriptTokenList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mScriptTokenList.clear();
    }
    //------------------------------------------------------------------------------
    void ScriptLexerParser::token(const String & str)
    {
        ScriptTokenList::iterator k, kend = mScriptTokenList.end();
        for(k = mScriptTokenList.begin(); k != kend; ++k)
        {
            N_delete *k;
        }
        mScriptTokenList.clear();

        // State enums
        enum{ READY = 0, COMMENT, MULTICOMMENT, WORD, QUOTE, VAR, POSSIBLECOMMENT };

        // Set up some constant characters of interest
#if NII_STRING
        const wchar_t varopener = L'$', quote = L'\"', slash = L'/', backslash = L'\\', openbrace = L'{', closebrace = L'}', colon = L':', star = L'*';
        wchar_t c = 0, lastc = 0;
#else
        const wchar_t varopener = '$', quote = '\"', slash = '/', backslash = '\\', openbrace = '{', closebrace = '}', colon = ':', star = '*';
        NIIb c = 0, lastc = 0;
#endif
        String lexeme;
        Nui32 line = 1, state = READY, lastQuote = 0;

        // Iterate over the input
        String::const_iterator i = str.begin();
        String::const_iterator end = str.end();
        while(i != end)
        {
            lastc = c;
            c = *i;

            if(c == quote)
                lastQuote = line;

            switch(state)
            {
            case READY:
            {
                if(c == slash && lastc == slash)
                {
                    // Comment start, clear out the lexeme
                    lexeme = _T("");
                    state = COMMENT;
                }
                else if(c == star && lastc == slash)
                {
                    lexeme = _T("");
                    state = MULTICOMMENT;
                }
                else if(c == quote)
                {
                    // Clear out the lexeme ready to be filled with quotes!
                    lexeme = c;
                    state = QUOTE;
                }
                else if(c == varopener)
                {
                    // Set up to read in a variable
                    lexeme = c;
                    state = VAR;
                }
                else if(isNewline((Nui32)c))
                {
                    lexeme = c;
                    add(lexeme, line);
                }
                else if(!isSpace((Nui32)c))
                {
                    lexeme = c;
                    if(c == slash)
                        state = POSSIBLECOMMENT;
                    else
                        state = WORD;
                }
            }
            break;
            case COMMENT:
            {
                // This newline happens to be ignored automatically
                if(isNewline((Nui32)c))
                    state = READY;
            }
            break;
            case MULTICOMMENT:
            {
                if(c == slash && lastc == star)
                    state = READY;
            }
            break;
            case POSSIBLECOMMENT:
                if(c == slash && lastc == slash)
                {
                    lexeme = _T("");
                    state = COMMENT;
                    break;
                }
                else if(c == star && lastc == slash)
                {
                    lexeme = _T("");
                    state = MULTICOMMENT;
                    break;
                }
                else
                {
                    state = WORD;
                }
            case WORD:
                if(isNewline((Nui32)c))
                {
                    add(lexeme, line);
                    lexeme = c;
                    add(lexeme, line);
                    state = READY;
                }
                else if(isSpace((Nui32)c))
                {
                    add(lexeme, line);
                    state = READY;
                }
                else if(c == openbrace || c == closebrace || c == colon)
                {
                    add(lexeme, line);
                    lexeme = c;
                    add(lexeme, line);
                    state = READY;
                }
                else
                {
                    lexeme += c;
                }
                break;
            case QUOTE:
            {
                if(c != backslash)
                {
                    // Allow embedded quotes with escaping
                    if(c == quote && lastc == backslash)
                    {
                        lexeme += c;
                    }
                    else if(c == quote)
                    {
                        lexeme += c;
                        add(lexeme, line);
                        state = READY;
                    }
                    else
                    {
                        // Backtrack here and allow a backslash normally within the quote
                        if(lastc == backslash)
                            lexeme = lexeme + _T("\\") + c;
                        else
                            lexeme += c;
                    }
                }
            }
            break;
            case VAR:
            {
                if(isNewline((Nui32)c))
                {
                    add(lexeme, line);
                    lexeme = c;
                    add(lexeme, line);
                    state = READY;
                }
                else if(isSpace((Nui32)c))
                {
                    add(lexeme, line);
                    state = READY;
                }
                else if(c == openbrace || c == closebrace || c == colon)
                {
                    add(lexeme, line);
                    lexeme = c;
                    add(lexeme, line);
                    state = READY;
                }
                else
                {
                    lexeme += c;
                }
            }
            break;
            }

            // Separate check for newlines just to track line numbers
            if(isNewline((Nui32)c))
                line++;

            i++;
        }

        // Check for valid exit states
        if(state == WORD || state == VAR)
        {
            if(!lexeme.empty())
                add(lexeme, line);
        }
        else
        {
            if(state == QUOTE)
            {
                N_EXCEPT(InvalidState, _I18n("no matching \" found for \" at line ") + N_conv(lastQuote));
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void ScriptLexerParser::add(const String & lexeme, Nui32 line)
    {
#if NII_STRING
        const wchar_t openBracket = L'{', closeBracket = L'}', colon = L':', quote = L'\"', var = L'$';
#else
        const NIIb openBracket = '{', closeBracket = '}', colon = ':', quote = '\"', var = '$';
#endif

        ScriptToken * token = N_new_t(ScriptToken)();
        token->lexeme = lexeme;
        token->mLineNo = line;
        token->mFile = mFileName;
        bool ignore = false;

        // Check the user token map first
        if(lexeme.size() == 1 && isNewline((Nui32)lexeme[0]))
        {
            token->mType = STT_NewLine;
            if(!mScriptTokenList.empty() && mScriptTokenList.back()->mType == STT_NewLine)
                ignore = true;
        }
        else if(lexeme.size() == 1 && lexeme[0] == openBracket)
        {
            token->mType = STT_LBracket;
        }
        else if(lexeme.size() == 1 && lexeme[0] == closeBracket)
        {
            token->mType = STT_RBracket;
        }
        else if(lexeme.size() == 1 && lexeme[0] == colon)
        {
            token->mType = STT_Colon;
        }
        else if(lexeme[0] == var)
        {
            token->mType = STT_RefValue;
        }
        else
        {
            // This is either a non-zero length phrase or quoted phrase
            if(lexeme.size() >= 2 && lexeme[0] == quote && lexeme[lexeme.size() - 1] == quote)
            {
                token->mType = STT_Quote;
            }
            else
            {
                token->mType = STT_Text;
            }
        }

        if(!ignore)
            mScriptTokenList.push_back(token);
    }
    //---------------------------------------------------------------------------
    void ScriptLexerParser::parse(const String & data, TokenNodeList *& out)
    {
        token(data);
        out = N_new_t(TokenNodeList)();

        enum{READY, OBJECT};
        Nui32 state = READY;

        TokenNode * parent = 0;
        TokenNode * node = 0;
        ScriptToken * token = 0;
        ScriptTokenList::iterator i = mScriptTokenList.begin(), end = mScriptTokenList.end();
        while(i != end)
        {
            token = *i;

            switch(state)
            {
            case READY:
                if(token->mType == STT_Text)
                {
                    if(token->lexeme == _T("extern"))
                    {
                        node = N_new TokenNode();
                        node->mToken = token->lexeme;
                        node->mFile = token->mFile;
                        node->mLineNo = token->mLineNo;
                        node->mType = TNT_Extern;

                        // The next token is the target
                        ++i;
                        if(i == end || ((*i)->mType != STT_Text && (*i)->mType != STT_Quote))
                            N_EXCEPT(InvalidState, _I18n("expected import target at line ") + N_conv(node->mLineNo));

                        TokenNode * temp = N_new TokenNode();
                        temp->mParent = node;
                        temp->mFile = (*i)->mFile;
                        temp->mLineNo = (*i)->mLineNo;
                        temp->mType = (*i)->mType == STT_Text ? TNT_Text : TNT_Quote;
                        if(temp->mType == TNT_Quote)
                            temp->mToken = (*i)->lexeme.substr(1, token->lexeme.size() - 2);
                        else
                            temp->mToken = (*i)->lexeme;
                        node->mChildren.push_back(temp);

                        // The second-next token is the source
                        ++i;
                        //++i;
                        if(i == end || ((*i)->mType != STT_Text && (*i)->mType != STT_Quote))
                            N_EXCEPT(InvalidState, _I18n("expected import source at line ") + N_conv(node->mLineNo));

                        temp = N_new TokenNode();
                        temp->mParent = node;
                        temp->mFile = (*i)->mFile;
                        temp->mLineNo = (*i)->mLineNo;
                        temp->mType = (*i)->mType == STT_Text ? TNT_Text : TNT_Quote;
                        if(temp->mType == TNT_Quote)
                            temp->mToken = (*i)->lexeme.substr(1, (*i)->lexeme.size() - 2);
                        else
                            temp->mToken = (*i)->lexeme;
                        node->mChildren.push_back(temp);

                        // Consume all the newlines
                        __n_skipNewLines(i, end, i);

                        // Insert the node
                        if(parent)
                        {
                            node->mParent = parent;
                            parent->mChildren.push_back(node);
                        }
                        else
                        {
                            node->mParent = 0;
                            out->push_back(node);
                        }
                        node = 0;
                    }
                    else if(token->lexeme == _T("set"))
                    {
                        node = N_new TokenNode();
                        node->mToken = token->lexeme;
                        node->mFile = token->mFile;
                        node->mLineNo = token->mLineNo;
                        node->mType = TNT_SetValue;

                        // The next token is the variable
                        ++i;
                        if(i == end || (*i)->mType != STT_RefValue)
                            N_EXCEPT(InvalidState, _I18n("expected variable name at line ") + N_conv(node->mLineNo));

                        TokenNode * temp = N_new TokenNode();
                        temp->mParent = node;
                        temp->mFile = (*i)->mFile;
                        temp->mLineNo = (*i)->mLineNo;
                        temp->mType = TNT_AssignValue;
                        temp->mToken = (*i)->lexeme;
                        node->mChildren.push_back(temp);

                        // The next token is the assignment
                        ++i;
                        if(i == end || ((*i)->mType != STT_Text && (*i)->mType != STT_Quote))
                            N_EXCEPT(InvalidState, _I18n("expected variable value at line ") + N_conv(node->mLineNo));

                        temp = N_new TokenNode();
                        temp->mParent = node;
                        temp->mFile = (*i)->mFile;
                        temp->mLineNo = (*i)->mLineNo;
                        temp->mType = (*i)->mType == STT_Text ? TNT_Text : TNT_Quote;
                        if(temp->mType == TNT_Quote)
                            temp->mToken = (*i)->lexeme.substr(1, (*i)->lexeme.size() - 2);
                        else
                            temp->mToken = (*i)->lexeme;
                        node->mChildren.push_back(temp);

                        // Consume all the newlines
                        __n_skipNewLines(i, end, i);

                        // Insert the node
                        if(parent)
                        {
                            node->mParent = parent;
                            parent->mChildren.push_back(node);
                        }
                        else
                        {
                            node->mParent = 0;
                            out->push_back(node);
                        }
                        node = 0;
                    }
                    else
                    {
                        node = N_new TokenNode();
                        node->mFile = token->mFile;
                        node->mLineNo = token->mLineNo;
                        node->mType = token->mType == STT_Text ? TNT_Text : TNT_Quote;
                        if(node->mType == TNT_Quote)
                            node->mToken = token->lexeme.substr(1, token->lexeme.size() - 2);
                        else
                            node->mToken = token->lexeme;

                        // Insert the node
                        if(parent)
                        {
                            node->mParent = parent;
                            parent->mChildren.push_back(node);
                        }
                        else
                        {
                            node->mParent = 0;
                            out->push_back(node);
                        }

                        // Set the parent
                        parent = node;

                        // Switch states
                        state = OBJECT;

                        node = 0;
                    }
                }
                else if(token->mType == STT_RBracket)
                {
                    // Go up one level if we can
                    if(parent)
                        parent = parent->mParent;

                    node = N_new TokenNode();
                    node->mToken = token->lexeme;
                    node->mFile = token->mFile;
                    node->mLineNo = token->mLineNo;
                    node->mType = TNT_RBrace;

                    // Consume all the newlines
                    __n_skipNewLines(i, end, i);

                    // Insert the node
                    if(parent)
                    {
                        node->mParent = parent;
                        parent->mChildren.push_back(node);
                    }
                    else
                    {
                        node->mParent = 0;
                        out->push_back(node);
                    }

                    // Move up another level
                    if(parent)
                        parent = parent->mParent;

                    node = 0;
                }
                break;
            case OBJECT:
                if(token->mType == STT_NewLine)
                {
                    // Look ahead to the next non-newline token and if it isn't an {, this was a property
                    __n_skipNewLines(i, end, ScriptTokenList::iterator next);
                    if(next == end || (*next)->mType != STT_LBracket)
                    {
                        // Ended a property here
                        if(parent)
                            parent = parent->mParent;
                        state = READY;
                    }
                }
                else if(token->mType == STT_Colon)
                {
                    node = N_new TokenNode();
                    node->mToken = token->lexeme;
                    node->mFile = token->mFile;
                    node->mLineNo = token->mLineNo;
                    node->mType = TNT_Colon;

                    // The following token are the parent objects (base classes).
                    // Require at least one of them.

                    ScriptTokenList::iterator j = i + 1;
                    __n_skipNewLines(j, end, j);
                    if(j == end || ((*j)->mType != STT_Text && (*j)->mType != STT_Quote))
                    {
                        N_EXCEPT(InvalidState, _I18n("expected object identifier at line ") + N_conv(node->mLineNo));
                    }

                    while(j != end && ((*j)->mType == STT_Text || (*j)->mType == STT_Quote))
                    {
                        TokenNode * tempNode = N_new TokenNode();
                        tempNode->mToken = (*j)->lexeme;
                        tempNode->mFile = (*j)->mFile;
                        tempNode->mLineNo = (*j)->mLineNo;
                        tempNode->mType = (*j)->mType == STT_Text ? TNT_Text : TNT_Quote;
                        tempNode->mParent = node;
                        node->mChildren.push_back(tempNode);
                        ++j;
                    }

                    // Move it backwards once, since the end of the loop moves it forwards again anyway
                    j--;
                    i = j;

                    // Insert the node
                    if(parent)
                    {
                        node->mParent = parent;
                        parent->mChildren.push_back(node);
                    }
                    else
                    {
                        node->mParent = 0;
                        out->push_back(node);
                    }
                    node = 0;
                }
                else if(token->mType == STT_LBracket)
                {
                    node = N_new TokenNode();
                    node->mToken = token->lexeme;
                    node->mFile = token->mFile;
                    node->mLineNo = token->mLineNo;
                    node->mType = TNT_LBrace;

                    // Consume all the newlines
                    __n_skipNewLines(i, end, i);

                    // Insert the node
                    if(parent)
                    {
                        node->mParent = parent;
                        parent->mChildren.push_back(node);
                    }
                    else
                    {
                        node->mParent = 0;
                        out->push_back(node);
                    }

                    // Set the parent
                    parent = node;

                    // Change the state
                    state = READY;

                    node = 0;
                }
                else if(token->mType == STT_RBracket)
                {
                    // Go up one level if we can
                    if(parent)
                        parent = parent->mParent;

                    // If the parent is currently a { then go up again
                    if(parent && parent->mType == TNT_LBrace && parent->mParent)
                        parent = parent->mParent;

                    node = N_new TokenNode();
                    node->mToken = token->lexeme;
                    node->mFile = token->mFile;
                    node->mLineNo = token->mLineNo;
                    node->mType = TNT_RBrace;

                    // Consume all the newlines
                    __n_skipNewLines(i, end, i);

                    // Insert the node
                    if(parent)
                    {
                        node->mParent = parent;
                        parent->mChildren.push_back(node);
                    }
                    else
                    {
                        node->mParent = 0;
                        out->push_back(node);
                    }

                    // Move up another level
                    if(parent)
                        parent = parent->mParent;

                    node = 0;
                    state = READY;
                }
                else if(token->mType == STT_RefValue)
                {
                    node = N_new TokenNode();
                    node->mToken = token->lexeme;
                    node->mFile = token->mFile;
                    node->mLineNo = token->mLineNo;
                    node->mType = TNT_AssignValue;

                    // Insert the node
                    if(parent)
                    {
                        node->mParent = parent;
                        parent->mChildren.push_back(node);
                    }
                    else
                    {
                        node->mParent = 0;
                        out->push_back(node);
                    }
                    node = 0;
                }
                else if(token->mType == STT_Quote)
                {
                    node = N_new TokenNode();
                    node->mToken = token->lexeme.substr(1, token->lexeme.size() - 2);
                    node->mFile = token->mFile;
                    node->mLineNo = token->mLineNo;
                    node->mType = TNT_Quote;

                    // Insert the node
                    if(parent)
                    {
                        node->mParent = parent;
                        parent->mChildren.push_back(node);
                    }
                    else
                    {
                        node->mParent = 0;
                        out->push_back(node);
                    }
                    node = 0;
                }
                else if(token->mType == STT_Text)
                {
                    node = N_new TokenNode();
                    node->mToken = token->lexeme;
                    node->mFile = token->mFile;
                    node->mLineNo = token->mLineNo;
                    node->mType = TNT_Text;

                    // Insert the node
                    if(parent)
                    {
                        node->mParent = parent;
                        parent->mChildren.push_back(node);
                    }
                    else
                    {
                        node->mParent = 0;
                        out->push_back(node);
                    }
                    node = 0;
                }
                break;
            }
            ++i;
        }
    }
    //-------------------------------------------------------------------------------
    void ScriptLexerParser::parseText(const String & data, TokenNodeList *& out)
    {
        token(data);
        out = N_new_t(TokenNodeList)();

        TokenNode * node = 0;
        ScriptToken * token = 0;
        ScriptTokenList::const_iterator i, iend = mScriptTokenList.end();
        for(i = mScriptTokenList.begin(); i != iend; ++i)
        {
            token = *i;

            switch(token->mType)
            {
            case STT_RefValue :
                node = N_new TokenNode();
                node->mFile = token->mFile;
                node->mLineNo = token->mLineNo;
                node->mParent = 0;
                node->mToken = token->lexeme;
                node->mType = TNT_AssignValue;
                break;
            case STT_Text :
                node = N_new TokenNode();
                node->mFile = token->mFile;
                node->mLineNo = token->mLineNo;
                node->mParent = 0;
                node->mToken = token->lexeme;
                node->mType = TNT_Text;
                break;
            case STT_Quote :
                node = N_new TokenNode();
                node->mFile = token->mFile;
                node->mLineNo = token->mLineNo;
                node->mParent = 0;
                node->mToken = token->lexeme.substr(1, token->lexeme.size() - 2);
                node->mType = TNT_Quote;
            default:
                N_EXCEPT(InvalidState, _I18n("unexpected token") + token->lexeme +
                    _I18n(" at line ") + N_conv(token->mLineNo));
            }
            if(node != 0)
                out->push_back(node);
        }
    }
    //------------------------------------------------------------------------
    void ScriptLexerParser::free(TokenNodeList * out)
    {
        N_assert(out, "error");

        TokenNodeList::iterator i, iend = out->end();
        for(i = out->begin(); i != iend; ++i)
        {
            TokenNodeList::iterator j, jend;
            for(j = (*i)->mChildren.begin(), jend = (*i)->mChildren.end(); j != jend; ++j)
            {
                N_delete *j;
            }
            (*i)->mChildren.clear();
            N_delete *i;
        }
        out->clear();
        N_delete_t(out, TokenNodeList);
    }
    //--------------------------------------------------------------------------------
    void ScriptLexerParser::create(ScriptNodeList *& out)
    {
        out = N_new_t(ScriptNodeList)();
    }
    //--------------------------------------------------------------------------------
    void ScriptLexerParser::analyze(const TokenNodeList & tokens, ScriptNodeList * out)
    {
        TokenNodeList::const_iterator t, tend = tokens.end();
        for(t = tokens.begin(); t != tend; ++t)
        {
            TokenNode * node = *t;
            ScriptNode * asn = 0;
            // Import = "import" >> 2 children, mCurrentProcess == null
            if(node->mType == TNT_Extern && mCurrentProcess == 0)
            {
                if(node->mChildren.size() > 2)
                {
                    mCompiler->error(*node, ScriptError::ET_ParamCountError);
                    return;
                }
                if(node->mChildren.size() < 2)
                {
                    mCompiler->error(*node, ScriptError::ET_StringError);
                    return;
                }

                ExternScriptNode * impl = N_new ExternScriptNode();
                impl->mLine = node->mLineNo;
                impl->mFile = node->mFile;

                TokenNodeList::iterator iter = node->mChildren.begin();
                impl->mTarget = (*iter)->mToken;
                iter++;
                impl->mFile = (*iter)->mToken;

                asn = impl;
            }
            // variable set = "set" >> 2 children, children[0] == variable
            else if(node->mType == TNT_SetValue)
            {
                if(node->mChildren.size() > 2)
                {
                    mCompiler->error(*node, ScriptError::ET_ParamCountError);
                    return;
                }
                if(node->mChildren.size() < 2)
                {
                    mCompiler->error(*node, ScriptError::ET_StringError);
                    return;
                }
                if(node->mChildren.front()->mType != TNT_AssignValue)
                {
                    mCompiler->error(*node->mChildren.front(), ScriptError::ET_ValueError);
                    return;
                }

                TokenNodeList::iterator i = node->mChildren.begin();
                String name = (*i)->mToken;
                ++i;
                String value = (*i)->mToken;

                if(mCurrentProcess && mCurrentProcess->mType == SNT_Object)
                {
                    ObjectScriptNode * ptr = (ObjectScriptNode *)mCurrentProcess;
                    ptr->setVariable(name, value);
                }
                else
                {
                    mCompiler->addEnvValue(name, value);
                }
            }
            // variable = $*, no children
            else if(node->mType == TNT_AssignValue)
            {
                if(!node->mChildren.empty())
                {
                    mCompiler->error(*node, ScriptError::ET_ParamCountError);
                    return;
                }

                RefValueScriptNode * impl = N_new RefValueScriptNode(mCurrentProcess);
                impl->mLine = node->mLineNo;
                impl->mFile = node->mFile;
                impl->mRefData = node->mToken;

                asn = impl;
            }
            // Handle properties and objects here
            else if(!node->mChildren.empty())
            {
                // Grab the last two nodes
                TokenNode * temp1 = 0,* temp2 = 0;
                TokenNodeList::reverse_iterator riter = node->mChildren.rbegin();
                if(riter != node->mChildren.rend())
                {
                    temp1 = *riter;
                    riter++;
                }
                if(riter != node->mChildren.rend())
                {
                    temp2 = *riter;
                }

                // object = last 2 children == { and }
                if(temp1 != 0 && temp2 != 0 && temp1->mType == TNT_RBrace && temp2->mType == TNT_LBrace)
                {
                    if(node->mChildren.size() < 2)
                    {
                        mCompiler->error(*node, ScriptError::ET_StringError);
                        return;
                    }

                    ObjectScriptNode * impl = N_new ObjectScriptNode(mCurrentProcess);
                    impl->mLine = node->mLineNo;
                    impl->mFile = node->mFile;

                    // Create a temporary detail list
                    list<TokenNode *>::type temp;
                    if(node->mToken == _T("Base"))
                    {
                        impl->mBaseObj = true;
                        for(TokenNodeList::const_iterator i = node->mChildren.begin(); i != node->mChildren.end(); ++i)
                            temp.push_back(*i);
                    }
                    else
                    {
                        impl->mBaseObj = false;
                        temp.push_back(node);
                        for(TokenNodeList::const_iterator i = node->mChildren.begin(); i != node->mChildren.end(); ++i)
                            temp.push_back(*i);
                    }

                    // 对象类型
                    list<TokenNode *>::type::const_iterator iter = temp.begin();
                    impl->mClass = (*iter)->mToken;
                    ++iter;

                    // 对象/属性对象的名字
                    if(iter != temp.end() && ((*iter)->mType == TNT_Text || (*iter)->mType == TNT_Quote) &&
                        !__ExcludeID(impl->mClass, mCurrentProcess))
                    {
                        impl->mName = (*iter)->mToken;
                        ++iter;
                    }

                    // 对象属性值
                    while(iter != temp.end() && (*iter)->mType != TNT_Colon && (*iter)->mType != TNT_LBrace)
                    {
                        if((*iter)->mType == TNT_AssignValue)
                        {
                            RefValueScriptNode * var = N_new RefValueScriptNode(impl);
                            var->mFile = (*iter)->mFile;
                            var->mLine = (*iter)->mLineNo;
                            var->mType = SNT_RefValue;
                            var->mRefData = (*iter)->mToken;
                            impl->mParamValue.push_back(var);
                        }
                        else
                        {
                            ValueScriptNode * atom = N_new ValueScriptNode(impl);
                            atom->mFile = (*iter)->mFile;
                            atom->mLine = (*iter)->mLineNo;
                            atom->mType = SNT_Value;
                            atom->mValueData = (*iter)->mToken;
                            impl->mParamValue.push_back(atom);
                        }
                        ++iter;
                    }

                    // 找到基础对象
                    if(iter != temp.end() && (*iter)->mType == TNT_Colon)
                    {
                        // Children of the ':' are bases
                        for(TokenNodeList::iterator j = (*iter)->mChildren.begin(); j != (*iter)->mChildren.end(); ++j)
                            impl->BaseObjList.push_back((*j)->mToken);
                        ++iter;
                    }

                    asn = impl;

                    mCurrentProcess = impl;
                    // Visit the children of the {
                    analyze(temp2->mChildren, out);
                    // Go back up the stack
                    mCurrentProcess = impl->mParent;
                }
                // Otherwise, it is a property
                else
                {
                    PropertyScriptNode * impl = N_new PropertyScriptNode(mCurrentProcess);
                    impl->mLine = node->mLineNo;
                    impl->mFile = node->mFile;
                    impl->mPropertyName = node->mToken;

                    asn = impl;

                    mCurrentProcess = impl;
                    // Visit the children of the {
                    analyze(node->mChildren, out);
                    // Go back up the stack
                    mCurrentProcess = impl->mParent;
                }
            }
            // Otherwise, it is a standard atom
            else
            {
                ValueScriptNode * impl = N_new ValueScriptNode(mCurrentProcess);
                impl->mLine = node->mLineNo;
                impl->mFile = node->mFile;
                impl->mValueData = node->mToken;

                asn = impl;
            }

            // Here, we must insert the node into the tree
            if(asn != 0)
            {
                if(mCurrentProcess)
                {
                    if(mCurrentProcess->mType == SNT_Property)
                    {
                        PropertyScriptNode * impl = reinterpret_cast<PropertyScriptNode *>(mCurrentProcess);
                        impl->mValueDataList.push_back(asn);
                    }
                    else
                    {
                        ObjectScriptNode * impl = reinterpret_cast<ObjectScriptNode *>(mCurrentProcess);
                        impl->mChildren.push_back(asn);
                    }
                }
                else
                {
                    out->push_back(asn);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void ScriptLexerParser::destroy(ScriptNodeList * in)
    {
        ScriptNodeList::iterator i, iend = in->end();
        for(i = in->begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        in->clear();
        N_delete_t(in, ScriptNodeList);
    }
    //------------------------------------------------------------------------
    bool ScriptLexerParser::isSpace(Nui32 c) const
    {
        //return c == ' ' || c == '\r' || c == '\t';
        return c == 32 || c == 13 || c == 9;
    }
    //------------------------------------------------------------------------
    bool ScriptLexerParser::isNewline(Nui32 c) const
    {
        //return c == '\n' || c == '\r';
        return c == 10 || c == 13;
    }
    //------------------------------------------------------------------------
}