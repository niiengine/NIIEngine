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

#ifndef _NII_SCRIPT_COMPILER_H_
#define _NII_SCRIPT_COMPILER_H_

#include "NiiPreInclude.h"
#include "NiiScriptNode.h"
#include "NiiScriptLexerParser.h"

namespace NII
{
    /** 脚本错误
    @version NIIEngine 3.0.0
    */
    class _EngineInner ScriptError : public ScriptAlloc
    {
    public:
        enum ExpectType
        {
            ET_ScriptError  = 0,
            ET_StringError  = 1,
            ET_NumberError  = 2,
            ET_ValueError   = 3,
            ET_UnRefError   = 4,
            ET_ObjIDError   = 5,
            ET_UniqueError  = 6,
            ET_ParamCountError = 7,
            ET_InvalidParamError = 8,
            ET_ObjExistError = 9
        };

        ScriptError(ExpectType type, const String & file, NIIi line, const String & msg);

        static const String & getErrorStr(ExpectType type);
    public:
        typedef StringList ErrorMap;
        static ErrorMap mErrorMap;
    public:
        ExpectType mErrorNo;
        String mMsg;
        String mFile;
        NIIi mLine;
    };

    /** 脚本编译器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptCompiler : public ScriptAlloc
    {
    public:
        ScriptCompiler();
        ~ScriptCompiler();

        /** 便宜脚本
        @param[in] str 脚本
        @param[in] file 所在文件
        @param[in] gid 资源组
        @version NIIEngine 3.0.0
        */
        bool compile(const String & str, const String & file, GroupID gid);

        /** 出现错误
        @param[in] node 出现错误的节点
        @param[in] code 错误的代码
        @param[in] msg 错误的消息
        @version NIIEngine 3.0.0
        */
        void error(const ScriptNode & node, ScriptError::ExpectType type, const String & msg = _T(""));

        /** 出现错误
        @param[in] node 出现错误的节点
        @param[in] code 错误的代码
        @param[in] msg 错误的消息
        @version NIIEngine 3.0.0
        */
        void error(const TokenNode & node, ScriptError::ExpectType type, const String & msg = _T(""));

        /** 清空错误
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 添加全局变量
        @version NIIEngine 3.0.0
        */
        void addEnvValue(const String & name, const String & value);

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        GroupID getResourceGroup() const;
    private:
        /** 加载外部
        @version NIIEngine 3.0.0 内部api
        */
        ScriptNodeList * loadExtern(const String & name);

        /** 找到对象
        @version NIIEngine 3.0.0 内部api
        */
        ScriptNode * findObj(ScriptNodeList * nodes, const String & obj);

        /** 处理语言类型
        @version NIIEngine 3.0.0 内部api
        */
        void processLanguage(ScriptNodeList * nodes);

        /** 处理外部引用
        @version NIIEngine 3.0.0 内部api
        */
        void processExtern(ScriptNodeList * nodes);

        /** 处理类型
        @version NIIEngine 3.0.0 内部api
        */
        void processObject(ScriptNodeList * nodes, ScriptNodeList * top);

        /** 处理变量
        @version NIIEngine 3.0.0 内部api
        */
        void processRefValue(ScriptNodeList * nodes);

        /** 继承属性
        @version NIIEngine 3.0.0 内部api
        */
        void inheritProperty(ObjectScriptNode * dest, const ScriptNode * source);
    private:
        void destroyExtern();
    protected:
        typedef map<String, String>::type MapValue;
        typedef map<String, ScriptNodeList *>::type ExternNodeList;
        typedef multimap<String, String>::type ExternList;
        typedef list<ScriptError *>::type ErrorList;

        LangID mLanguage;
        MapValue mMapValue;
        ScriptNodeList mBaseTable;
        ScriptNodeList mExternTable;
        ExternNodeList mExternNodeList;
        ExternList mExternList;
        GroupID mGroup;
        ErrorList mErrors;
    };
}

#endif