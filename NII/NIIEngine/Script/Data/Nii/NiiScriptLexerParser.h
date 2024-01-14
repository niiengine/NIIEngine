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

#ifndef _NII_ScriptLexer_Parser_H_
#define _NII_ScriptLexer_Parser_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include "NiiScriptNode.h"

namespace NII
{
    /** 这些是解析节点枚举类型
    @version NIIEngine 3.0.0
    */
    enum TokenNodeType
    {
        TNT_LBrace,
        TNT_RBrace,
        TNT_LMBrace,
        TNT_RMBrace,
        TNT_LSBrace,
        TNT_RSBrace,
        TNT_Extern,
        TNT_Text,
        TNT_Quote,
        TNT_Colon,
        TNT_SetValue,
        TNT_AssignValue,
    };

    struct TokenNode;
    typedef list<TokenNode *>::type TokenNodeList;

    /** 标签节点
    @version NIIEngine 3.0.0
    */
    struct TokenNode : public ScriptAlloc
    {
        TokenNode * mParent;
        TokenNodeList mChildren;
        TokenNodeType mType;
        String mToken;
        String mFile;
        Nui32 mLineNo;
    };

    struct ScriptToken;

    /** 脚本语法解析器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptLexerParser : public ScriptAlloc
    {
    public:
        ScriptLexerParser(ScriptCompiler * compiler, const String & file);
        virtual ~ScriptLexerParser();

        /** 解析脚本节点
        @note 使用完参数2后必须调用 free 函数
        @version NIIEngine 3.0.0
        */
        void parse(const String & data, TokenNodeList *& out);

        /** 解析脚本节点
        @note 使用完参数2后必须调用 free 函数
        @version NIIEngine 3.0.0
        */
        void parseText(const String & data, TokenNodeList *& out);

        /** 释放
        @note 使用 parse parseText 后调用
        @version NIIEngine 3.0.0
        */
        static void free(TokenNodeList * out);

        /** 创建脚本节点列表
        @note 使用完参数1后必须调用 destroy 函数
        @version NIIEngine 3.0.0
        */
        static void create(ScriptNodeList *& out);
        
        /** 删除节点列表
        @note 使用完 create 后调用
        @version NIIEngine 3.0.0
        */
        static void destroy(ScriptNodeList * in);
        
        /** 分析脚本节点
        @version NIIEngine 3.0.0
        */
        void analyze(const TokenNodeList & tokens, ScriptNodeList * out);
    private:
        /** 添加节点
        @version NIIEngine 3.0.0
        */
        void add(const String & token, Nui32 line);

        /** 分割数据
        @version NIIEngine 3.0.0
        */
        void token(const String & data);

        /** 是否为空格
        @version NIIEngine 3.0.0
        */
        bool isSpace(Nui32 c) const;

        /** 是否为新行
        @version NIIEngine 3.0.0
        */
        bool isNewline(Nui32 c) const;
    private:
        typedef vector<ScriptToken *>::type ScriptTokenList;
    private:
        String mFileName;
        ScriptNode * mCurrentProcess;
        ScriptCompiler * mCompiler;
        ScriptTokenList mScriptTokenList;
    };
}
#endif