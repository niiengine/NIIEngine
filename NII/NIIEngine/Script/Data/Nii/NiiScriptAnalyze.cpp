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
#include "NiiScriptAnalyze.h"
#include "NiiScriptCompilerManager.h"
#include "NiiScriptHelperManager.h"

namespace NII
{
    //--------------------------------------------------------------------
    ScriptAnalyze::ScriptAnalyze(ScriptCompiler * cpl, ScriptTypeID stid, LangID lid) :
        ScriptProperty(stid, lid),
        mCompiler(cpl)
    {
    }
    //--------------------------------------------------------------------
    ScriptAnalyze::~ScriptAnalyze()
    {
    }
    //--------------------------------------------------------------------
    void ScriptAnalyze::analyze(const ScriptNode * in)
    {
        if(in->mType != SNT_Object || reinterpret_cast<const ObjectScriptNode *>(in)->mBaseObj)
            return;

        if(analyzeCom(in))
            return;

        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(in);
        ScriptAnalyze * analy = N_Only(ScriptCompiler).create(mCompiler, ScriptProperty::getProperty(obj->mClass), mLangMap->getLangID());

        if(analy)
        {
            analy->analyze(in);
            N_Only(ScriptCompiler).destroy(analy);
        }
        else
            mCompiler->error(*in, ScriptError::ET_ScriptError,
                _T("token \"") + reinterpret_cast<const ObjectScriptNode *>(in)->mClass + _T("\" is not recognized"));
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::analyzeCom(const ScriptNode *)
    {
        return false;
    }
    //-------------------------------------------------------------------------
    ScriptNodeList::const_iterator ScriptAnalyze::getIterator(const ScriptNodeList & nodes, Nidx index)
    {
        if(nodes.size() < index)
        {
            return nodes.end();
        }
        else
        {
            ScriptNodeList::const_iterator temp = nodes.begin();
            std::advance(temp, index);
            return temp;
        }
    }
    //-------------------------------------------------------------------------
    const ScriptNode * ScriptAnalyze::getNode(const ScriptNodeList & nodes, Nidx index)
    {
        if(nodes.size() < index)
        {
            return 0;
        }
        else
        {
            ScriptNodeList::const_iterator temp = nodes.begin();
            std::advance(temp, index);
            return *temp;
        }
    }
    //-------------------------------------------------------------------------
    GroupID ScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNode * node, String & out)
    {
        if(node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        out = atom->mValueData;
        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNode * node, bool & out)
    {
        if(node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);

        if (atom->mValueData == _T("true") || atom->mValueData == _T("TRUE"))
            out = true;
        else if (atom->mValueData == _T("false") || atom->mValueData == _T("FALSE"))
            out = false;
        else
            return false;
        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNode * node, Ni32 & out)
    {
        if(node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        NIIi n = Nsscanf(atom->mValueData.c_str(), _T("%d"), &out);
        if(n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNode * node, Nui32 & out)
    {
        if(node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        NIIi n = Nsscanf(atom->mValueData.c_str(), _T("%u"), &out);
        if(n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNode * node, NIIf & out)
    {
        if(node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        NIIi n = Nsscanf(atom->mValueData.c_str(), _T("%f"), &out);
        if(n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNode * node, NIId & out)
    {
        if(node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        NIIi n = Nsscanf(atom->mValueData.c_str(), _T("%lf"), &out);
        if(n == 0 || n == EOF)
            return false; // Conversion failed

        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, Colour & out)
    {
        if(list.size() < oft + 3)
            return false;

        bool result = true;
        ScriptNodeList::const_iterator b = list.begin();
        std::advance(b, oft);
        result &= getValue(*b, out.r);
        std::advance(b, 1);
        result &= getValue(*b, out.g);
        std::advance(b, 1);
        result &= getValue(*b, out.b);
        if (list.size() < oft + 3)
        {
            std::advance(b, 1);
            result &= getValue(*b, out.a);
        }
        return result;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, Matrix4f & out)
    {
        if(list.size() < oft + 16)
            return false;

        bool result = true;
        ScriptNodeList::const_iterator b = list.begin();
        std::advance(b, oft);
        result &= getValue(*b, out._m[0]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[1]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[2]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[3]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[4]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[5]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[6]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[7]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[8]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[9]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[10]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[11]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[12]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[13]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[14]);
        std::advance(b, 1);
        result &= getValue(*b, out._m[15]);

        return result;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, NCount cnt, StringList & out)
    {
        if (list.size() < oft + cnt)
            return false;

        NCount c = 0;
        Ni32 temp = 0;
        ScriptNodeList::const_iterator i, iend = list.end();
        for (i = list.begin(), std::advance(i, oft); i != iend && c < cnt; ++i, ++c)
        {
            if ((*i)->mType != SNT_Value)
                return false;

            out[c] = static_cast<const ValueScriptNode *>(*i)->mValueData;
        }
        if (c != cnt)
            return false;

        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, NCount cnt, Ni32 * out)
    {
        if(list.size() < oft + cnt)
            return false;

        NCount c = 0;
        Ni32 temp = 0;
        ScriptNodeList::const_iterator i, iend = list.end();
        for(i = list.begin(), std::advance(i, oft); i != iend && c < cnt; ++i, ++c)
        {
            if(getValue(*i, temp))
                out[c] = temp;
            else
                return false;
        }
        if(c != cnt)
            return false;

        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, NCount cnt, Nui32 * out)
    {
        if(list.size() < oft + cnt)
            return false;

        NCount c = 0;
        Nui32 temp = 0;
        ScriptNodeList::const_iterator i, iend = list.end();
        for(i = list.begin(), std::advance(i, oft); i != iend && c < cnt; ++i, ++c)
        {
            if(getValue(*i, temp))
                out[c] = temp;
            else
                return false;
        }
        if(c != cnt)
            return false;
        return true;
    }
    //----------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, NCount cnt, NIIf * out)
    {
        if(list.size() < oft + cnt)
            return false;

        NCount c = 0;
        NIIf temp = 0;
        ScriptNodeList::const_iterator i, iend = list.end();
        for(i = list.begin(), std::advance(i, oft); i != iend && c < cnt; ++i, ++c)
        {
            if(getValue(*i, temp))
                out[c] = temp;
            else
                return false;
        }
        if(c != cnt)
            return false;

        return true;
    }
    //----------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, NCount cnt, NIId * out)
    {
        if(list.size() < oft + cnt)
            return false;

        NCount c = 0;
        NIId temp = 0;
        ScriptNodeList::const_iterator i, iend = list.end();
        for(i = list.begin(), std::advance(i, oft); i != iend && c < cnt; ++i, ++c)
        {
            if(getValue(*i, temp))
                out[c] = temp;
            else
                return false;
        }
        if(c != cnt)
            return false;

        return true;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, String & out)
    {
        if(list.size() < oft + 1)
            return false;

        ScriptNodeList::const_iterator i = list.begin();
        std::advance(i, oft);
        if(getValue(*i, out))
            return true;

        return false;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, bool & out)
    {
        if(list.size() < oft + 1)
            return false;

        ScriptNodeList::const_iterator i = list.begin();
        std::advance(i, oft);
        if(getValue(*i, out))
            return true;

        return false;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, Ni32 & out)
    {
        if(list.size() < oft + 1)
            return false;

        ScriptNodeList::const_iterator i = list.begin();
        std::advance(i, oft);
        if(getValue(*i, out))
            return true;

        return false;
    }
    //-------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, Nui32 & out)
    {
        if(list.size() < oft + 1)
            return false;

        ScriptNodeList::const_iterator i = list.begin();
        std::advance(i, oft);
        if(getValue(*i, out))
            return true;

        return false;
    }
    //----------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, NIIf & out)
    {
        if(list.size() < oft + 1)
            return false;

        ScriptNodeList::const_iterator i = list.begin();
        std::advance(i, oft);
        if(getValue(*i, out))
            return true;

        return false;
    }
    //----------------------------------------------------------------------------
    bool ScriptAnalyze::getValue(const ScriptNodeList & list, NCount oft, NIId & out)
    {
        if(list.size() < oft + 1)
            return false;

        ScriptNodeList::const_iterator i = list.begin();
        std::advance(i, oft);
        if(getValue(*i, out))
            return true;

        return false;
    }
    //-------------------------------------------------------------------------
}