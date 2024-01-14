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
#include "NiiScriptCompiler.h"
#include "NiiScriptLexerParser.h"
#include "NiiScriptAnalyze.h"

namespace NII
{
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // ScriptNode
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    ScriptNode::ScriptNode(ScriptNode * parent, ScriptNodeType type) :
        mParent(parent),
        mType(type),
        mLine(0)
    {
    }
    //---------------------------------------------------------------------
    ScriptNode::~ScriptNode()
    {
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // ValueScriptNode
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    ValueScriptNode::ValueScriptNode(ScriptNode * parent) :
        ScriptNode(parent, SNT_Value)
    {
    }
    //---------------------------------------------------------------------
    String ValueScriptNode::getValue() const
    {
        return mValueData;
    }
    //---------------------------------------------------------------------
    ScriptNode * ValueScriptNode::clone() const
    {
        ValueScriptNode * node = N_new ValueScriptNode(mParent);
        node->mFile = mFile;
        node->mLine = mLine;
        node->mValueData = mValueData;
        return node;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // ObjectScriptNode
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    ObjectScriptNode::ObjectScriptNode(ScriptNode * parent) :
        ScriptNode(parent, SNT_Object),
        mBaseObj(false)
    {
    }
    //---------------------------------------------------------------------
    ObjectScriptNode::~ObjectScriptNode()
    {
        ScriptNodeList::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mChildren.clear();

        ScriptNodeList::iterator j, jend = mParamValue.end();
        for(j = mParamValue.begin(); j != jend; ++j)
        {
            N_delete *j;
        }
        mParamValue.clear();
    }
    //---------------------------------------------------------------------
    void ObjectScriptNode::setVariable(const String & name, const String & value)
    {
        mMapValue[name] = value;
    }
    //---------------------------------------------------------------------
    bool ObjectScriptNode::getVariable(const String & name, String & out) const
    {
        map<String, String>::type::const_iterator i = mMapValue.find(name);
        if(i != mMapValue.end())
        {
            out = i->second;
            return true;
        }
        ObjectScriptNode * parent = static_cast<ObjectScriptNode *>(mParent);
        while(parent)
        {
            i = parent->mMapValue.find(name);
            if(i != parent->mMapValue.end())
            {
                out = i->second;
                return true;
            }
            parent = static_cast<ObjectScriptNode *>(parent->mParent);
        }
        out = _T("");
        return false;
    }
    //---------------------------------------------------------------------
    String ObjectScriptNode::getValue() const
    {
        return mClass;
    }
    //---------------------------------------------------------------------
    ScriptNode * ObjectScriptNode::clone() const
    {
        ObjectScriptNode * node = N_new ObjectScriptNode(mParent);
        node->mFile = mFile;
        node->mLine = mLine;
        node->mName = mName;
        node->mClass = mClass;
        node->mBaseObj = mBaseObj;

        ScriptNodeList::const_iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            ScriptNode * newNode = (*i)->clone();
            newNode->mParent = node;
            node->mChildren.push_back(newNode);
        }
        ScriptNodeList::const_iterator j, jend = mParamValue.end();
        for(j = mParamValue.begin(); j != jend; ++j)
        {
            ScriptNode * newNode = (*j)->clone();
            newNode->mParent = node;
            node->mParamValue.push_back(newNode);
        }

        node->mMapValue = mMapValue;
        return node;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // PropertyScriptNode
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    PropertyScriptNode::PropertyScriptNode(ScriptNode * parent) :
        ScriptNode(parent, SNT_Property)
    {
    }
    //---------------------------------------------------------------------
    PropertyScriptNode::~PropertyScriptNode()
    {
        ScriptNodeList::iterator i, iend = mValueDataList.end();
        for(i = mValueDataList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mValueDataList.clear();
    }
    //---------------------------------------------------------------------
    String PropertyScriptNode::getValue() const
    {
        return mPropertyName;
    }
    //---------------------------------------------------------------------
    ScriptNode * PropertyScriptNode::clone() const
    {
        PropertyScriptNode * node = N_new PropertyScriptNode(mParent);
        node->mFile = mFile;
        node->mLine = mLine;
        node->mPropertyName = mPropertyName;

        ScriptNodeList::const_iterator i, iend = mValueDataList.end();
        for(i = mValueDataList.begin(); i != iend; ++i)
        {
            ScriptNode * newNode = (*i)->clone();
            newNode->mParent = node;
            node->mValueDataList.push_back(newNode);
        }
        return node;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // ExternScriptNode
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    ExternScriptNode::ExternScriptNode() :
        ScriptNode(0, SNT_Extern)
    {
    }
    //---------------------------------------------------------------------
    String ExternScriptNode::getValue() const
    {
        return mTarget;
    }
    //---------------------------------------------------------------------
    ScriptNode * ExternScriptNode::clone() const
    {
        ExternScriptNode * node = N_new ExternScriptNode();
        node->mFile = mFile;
        node->mLine = mLine;
        node->mTarget = mTarget;
        node->mFile = mFile;
        return node;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // LanguageScriptNode
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    LanguageScriptNode::LanguageScriptNode() :
        ScriptNode(0, SNT_Language)
    {
    }
    //---------------------------------------------------------------------
    String LanguageScriptNode::getValue() const
    {
        return mLanguage;
    }
    //---------------------------------------------------------------------
    ScriptNode * LanguageScriptNode::clone() const
    {
        LanguageScriptNode * node = N_new LanguageScriptNode();
        node->mFile = mFile;
        node->mLine = mLine;
        node->mLanguage = mLanguage;
        return node;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // RefValueScriptNode
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    RefValueScriptNode::RefValueScriptNode(ScriptNode * parent) :
        ScriptNode(parent, SNT_RefValue)
    {
    }
    //---------------------------------------------------------------------
    String RefValueScriptNode::getValue() const
    {
        return mRefData;
    }
    //---------------------------------------------------------------------
    ScriptNode * RefValueScriptNode::clone() const
    {
        RefValueScriptNode * node = N_new RefValueScriptNode(mParent);
        node->mFile = mFile;
        node->mLine = mLine;
        node->mRefData = mRefData;

        return node;
    }
    //---------------------------------------------------------------------
}