/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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