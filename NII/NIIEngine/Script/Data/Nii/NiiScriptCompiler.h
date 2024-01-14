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
    /** �ű�����
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

    /** �ű�������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptCompiler : public ScriptAlloc
    {
    public:
        ScriptCompiler();
        ~ScriptCompiler();

        /** ���˽ű�
        @param[in] str �ű�
        @param[in] file �����ļ�
        @param[in] gid ��Դ��
        @version NIIEngine 3.0.0
        */
        bool compile(const String & str, const String & file, GroupID gid);

        /** ���ִ���
        @param[in] node ���ִ���Ľڵ�
        @param[in] code ����Ĵ���
        @param[in] msg �������Ϣ
        @version NIIEngine 3.0.0
        */
        void error(const ScriptNode & node, ScriptError::ExpectType type, const String & msg = _T(""));

        /** ���ִ���
        @param[in] node ���ִ���Ľڵ�
        @param[in] code ����Ĵ���
        @param[in] msg �������Ϣ
        @version NIIEngine 3.0.0
        */
        void error(const TokenNode & node, ScriptError::ExpectType type, const String & msg = _T(""));

        /** ��մ���
        @version NIIEngine 3.0.0
        */
        void clear();

        /** ���ȫ�ֱ���
        @version NIIEngine 3.0.0
        */
        void addEnvValue(const String & name, const String & value);

        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
        GroupID getResourceGroup() const;
    private:
        /** �����ⲿ
        @version NIIEngine 3.0.0 �ڲ�api
        */
        ScriptNodeList * loadExtern(const String & name);

        /** �ҵ�����
        @version NIIEngine 3.0.0 �ڲ�api
        */
        ScriptNode * findObj(ScriptNodeList * nodes, const String & obj);

        /** ������������
        @version NIIEngine 3.0.0 �ڲ�api
        */
        void processLanguage(ScriptNodeList * nodes);

        /** �����ⲿ����
        @version NIIEngine 3.0.0 �ڲ�api
        */
        void processExtern(ScriptNodeList * nodes);

        /** ��������
        @version NIIEngine 3.0.0 �ڲ�api
        */
        void processObject(ScriptNodeList * nodes, ScriptNodeList * top);

        /** �������
        @version NIIEngine 3.0.0 �ڲ�api
        */
        void processRefValue(ScriptNodeList * nodes);

        /** �̳�����
        @version NIIEngine 3.0.0 �ڲ�api
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