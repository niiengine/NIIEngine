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

#ifndef _NII_SCRIPT_NODE_H_
#define _NII_SCRIPT_NODE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 节点类型
    @version NIIEngine 3.0.0
    */
    enum ScriptNodeType
    {
        SNT_Unknow,
        SNT_Language,
        SNT_Extern,
        SNT_Object,
        SNT_Property,
        SNT_RefValue,
        SNT_Value
    };

    /** 脚本节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptNode : public ScriptAlloc
    {
    public:
        ScriptNode(ScriptNode * parent, ScriptNodeType type);
        ~ScriptNode();

        /** 获取值
        @version NIIEngine 3.0.0
        */
        virtual String getValue() const = 0;

        /** 副本
        @version NIIEngine 3.0.0
        */
        virtual ScriptNode * clone() const = 0;
    public:
        ScriptNode * mParent;
        ScriptNodeType mType;
        mutable void * mData;
        String mFile;
        Nui32 mLine;
    };
    typedef list<ScriptNode *>::type ScriptNodeList;

    /** 语言控制节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LanguageScriptNode : public ScriptNode
    {
    public:
        LanguageScriptNode();

        /// @copydetails ScriptNode::getValue
        String getValue() const;

        /// @copydetails ScriptNode::clone
        ScriptNode * clone() const;
    public:
        String mLanguage;
    };

    /** 导入文件脚本节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ExternScriptNode : public ScriptNode
    {
    public:
        ExternScriptNode();

        /// @copydetails ScriptNode::getValue
        String getValue() const;

        /// @copydetails ScriptNode::clone
        ScriptNode * clone() const;
    public:
        String mFile;
        String mTarget;
    };

    /** 对象脚本节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ObjectScriptNode : public ScriptNode
    {
    public:
        ObjectScriptNode(ScriptNode * parent);
        ~ObjectScriptNode();

        /** 设置变量
        @version NIIEngine 3.0.0
        */
        void setVariable(const String & name, const String & in);

        /** 获取变量
        @version NIIEngine 3.0.0
        */
        bool getVariable(const String & name, String & out) const;

        /// @copydetails ScriptNode::getValue
        String getValue() const;

        /// @copydetails ScriptNode::clone
        ScriptNode * clone() const;
    public:
        typedef map<String, String>::type MapValue;
    public:
        String mName;
        String mClass;
        StringList BaseObjList;
        ScriptNodeList mChildren;
        ScriptNodeList mParamValue;
        ScriptNodeList mInheritValue;
        MapValue mMapValue;
        bool mBaseObj;
    };

    /** 属性脚本节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyScriptNode : public ScriptNode
    {
    public:
        PropertyScriptNode(ScriptNode * parent);
        ~PropertyScriptNode();

        inline NCount getCount() const { return mValueDataList.size(); }

        inline bool isEmpty() const { return mValueDataList.empty(); }

        /// @copydetails ScriptNode::getValue
        String getValue() const;

        /// @copydetails ScriptNode::clone
        ScriptNode * clone() const;
    public:
        String mPropertyName;
        ScriptNodeList mValueDataList;
    };

    /** 变量引用节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RefValueScriptNode : public ScriptNode
    {
    public:
        RefValueScriptNode(ScriptNode * parent);

        /// @copydetails ScriptNode::getValue
        String getValue() const;

        /// @copydetails ScriptNode::clone
        ScriptNode * clone() const;
    public:
        String mRefData;
    };

    /** 变量赋值节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ValueScriptNode : public ScriptNode
    {
    public:
        ValueScriptNode(ScriptNode * parent);

        /// @copydetails ScriptNode::getValue
        String getValue() const;

        /// @copydetails ScriptNode::clone
        ScriptNode * clone() const;
    public:
        String mValueData;
    };

}
#endif