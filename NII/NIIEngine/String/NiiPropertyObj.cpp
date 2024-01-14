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
#include "NiiXMLSerializer.h"
#include "NiiPropertyObj.h"
#include "NiiIDHelper.h"
#include "NiiException.h"
#include "NiiScriptHelperManager.h"
#include "NiiStrConv.h"

namespace NII
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// PropertyValueData
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PropertyValueData::add(PropertyID key, const String & value)
	{
		PropertyValue p(key, value);
		PropertyValueList::iterator it = std::lower_bound(mData.begin(), mData.end(), p, PropertyValueLess);
		if(it == mData.end() || it->mKey != p.mKey)
			mData.insert(it, p);
		else
			*it = p;
	}
	//------------------------------------------------------------------------
	const String & PropertyValueData::get(PropertyID key, const String & init)
	{
		PropertyValue p(key, 0);
		PropertyValueList::const_iterator it = std::lower_bound(mData.begin(), mData.end(), p, PropertyValueLess);
		if(it != mData.end() && it->mKey == p.mKey)
            return it->mValue;

		return init;
	}
	//------------------------------------------------------------------------
	bool StringIdData::isExist(Nid key, String & defauleOut)
	{
		StringIdMap::const_iterator it = std::lower_bound(mData.begin(), mData.end(),
			std::pair<Nid, String>(key, String()), StringIdLess);

		if(it != mData.end() && it->first == key)
		{
			defauleOut = it->second;
			return true;
		}

		return false;
	}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PropertyData
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PropertyData::PropertyData()
    {
    }
    //------------------------------------------------------------------------
    PropertyData::PropertyData(const PropertyData & o)
    {
        mPropertys = o.mPropertys;
        mExtPropertys = o.mExtPropertys;
    }
    //------------------------------------------------------------------------
    PropertyData::~PropertyData()
    {
        mPropertys.clear();
        mExtPropertys.clear();
    }
    //------------------------------------------------------------------------
    void PropertyData::add(PropertyID pid, const String & value)
    {
        mPropertys[pid] = value;
    }
    //------------------------------------------------------------------------
    void PropertyData::add(PropertyID pid, const Nutf8 * value)
    {
        String temp;
        DataStream::s2ws(value, temp);
        mPropertys[pid] = temp;
    }
    //------------------------------------------------------------------------
    void PropertyData::addData(PropertyID pid, const Any & value)
    {
        mExtPropertys[pid] = value;
    }
    //------------------------------------------------------------------------
    void PropertyData::addSub(PropertyID pid, const PropertyData & value)
    {
        mSubPropertys[pid] = value;
    }
    //------------------------------------------------------------------------
    void PropertyData::add(const PropertyData & o)
    {
        PropertyList::const_iterator i, iend = o.mPropertys.end();
        for (i = o.mPropertys.begin(); i != iend; ++i)
        {
            mPropertys[i->first] = i->second;
        }
        ExtPropertyList::const_iterator j, jend = o.mExtPropertys.end();
        for (j = o.mExtPropertys.begin(); j != jend; ++j)
        {
            mExtPropertys[j->first] = j->second;
        }
    }
    //------------------------------------------------------------------------
    void PropertyData::remove(PropertyID pid)
    {
        PropertyList::iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            mPropertys.erase(pos);
        }
        else
        {
            ExtPropertyList::iterator pos2 = mExtPropertys.find(pid);
            if (pos2 != mExtPropertys.end())
            {
                mExtPropertys.erase(pos2);
            }
        }
    }
    //------------------------------------------------------------------------
    void PropertyData::removeAll()
    {
        mPropertys.clear();
        mExtPropertys.clear();
        mSubPropertys.clear();
    }
    //------------------------------------------------------------------------
    bool PropertyData::isExist(PropertyID pid) const
    {
        if (mPropertys.find(pid) == mPropertys.end())
            if (mExtPropertys.find(pid) == mExtPropertys.end())
                return mSubPropertys.find(pid) != mSubPropertys.end();

        return true;
    }
    //------------------------------------------------------------------------
    PropertyID PropertyData::getID(Nidx idx) const
    {
        if (idx < mPropertys.size())
        {
            PropertyList::const_iterator iter = mPropertys.begin();
            std::advance(iter, idx);
            return iter->first;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    const String & PropertyData::getStr(Nidx idx) const
    {
        if (idx < mPropertys.size())
        {
            PropertyList::const_iterator iter = mPropertys.begin();
            std::advance(iter, idx);
            return iter->second;
        }
        return N_StrBlank;
    }
    //------------------------------------------------------------------------
    const Any & PropertyData::getData(Nidx idx) const
    {
        if (idx < mExtPropertys.size())
        {
            ExtPropertyList::const_iterator iter = mExtPropertys.begin();
            std::advance(iter, idx);
            return iter->second;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    const PropertyData * PropertyData::getSub(Nidx idx) const
    {
        if (idx < mSubPropertys.size())
        {
            SubPropertyList::const_iterator iter = mSubPropertys.begin();
            std::advance(iter, idx);
            return &iter->second;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    const String & PropertyData::get(PropertyID pid) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            return pos->second;
        }
        return N_StrBlank;
    }
    //------------------------------------------------------------------------
    String PropertyData::getDefault(PropertyID pid, const String & init) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            return pos->second;
        }
        return init;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, PropertyData & out) const
    {
        SubPropertyList::const_iterator pos = mSubPropertys.find(pid);
        if (pos != mSubPropertys.end())
        {
            out = pos->second;
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, String & out, const String & init) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            out = pos->second;
            return true;
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, StringList & out) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, out);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, bool & out, bool init) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            const String & val = pos->second;
            if (val == _T("false") || val == _T("False") || val == _T("0"))
            {
                out = false;
                return true;
            }
            else if (val == _T("true") || val == _T("True") || val == _T("1"))
            {
                out = true;
                return true;
            }
            else
            {
                N_assert(0, "error convert");
                out = init;
                return false;
            }
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, Ni32 & out, Ni32 init) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            Nistringstream strm(pos->second.c_str());
            strm >> out;

            if (strm.fail() || !strm.eof())
            {
                N_assert(0, "error convert");
                out = init;
                return false;
            }
            else
            {
                return true;
            }
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, Ni32 * out, NCount cnt) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < cnt && i < vec.size(); ++i)
                StrConv::conv(vec[i], out[i]);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, IntArray & out) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < vec.size(); ++i)
            {
                Ni32 temp;
                StrConv::conv(vec[i], temp);
                out.push_back(temp);
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, Nui32 & out, Nui32 init) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            Nistringstream strm(pos->second.c_str());
            strm >> out;

            if (strm.fail() || !strm.eof())
            {
                N_assert(0, "error convert");
                out = init;
                return false;
            }
            else
            {
                return true;
            }
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, Nui32 * out, NCount cnt) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < cnt && i < vec.size(); ++i)
                StrConv::conv(vec[i], out[i]);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, UIntArray & out) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < vec.size(); ++i)
            {
                Nui32 temp;
                StrConv::conv(vec[i], temp);
                out.push_back(temp);
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, NIIf & out, NIIf init) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            Nistringstream strm(pos->second.c_str());
            strm >> out;

            if (strm.fail() || !strm.eof())
            {
                N_assert(0, "error convert");
                out = init;
                return false;
            }
            else
            {
                return true;
            }
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, NIIf * out, NCount cnt) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < cnt && i < vec.size(); ++i)
                StrConv::conv(vec[i], out[i]);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, FloatArray & out) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < vec.size(); ++i)
            {
                NIIf temp;
                StrConv::conv(vec[i], temp);
                out.push_back(temp);
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, NIId & out, NIId init) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            Nistringstream strm(pos->second.c_str());
            strm >> out;

            if (strm.fail() || !strm.eof())
            {
                N_assert(0, "error convert");
                out = init;
                return false;
            }
            else
            {
                return true;
            }
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, NIId * out, NCount cnt) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < cnt && i < vec.size(); ++i)
                StrConv::conv(vec[i], out[i]);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(PropertyID pid, DoubleArray & out) const
    {
        PropertyList::const_iterator pos = mPropertys.find(pid);
        if (pos != mPropertys.end())
        {
            StringList vec;
            StrUtil::split(pos->second, vec);
            for (NCount i = 0; i < vec.size(); ++i)
            {
                NIId temp;
                StrConv::conv(vec[i], temp);
                out.push_back(temp);
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::getData(PropertyID pid, Any & out, const Any & init) const
    {
        ExtPropertyList::const_iterator pos = mExtPropertys.find(pid);
        if (pos != mExtPropertys.end())
        {
            out = pos->second;
            return true;
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    bool PropertyData::get(LangID lid, ScriptTypeID sid, PropertyID pid, PropertyID & out, PropertyID init) const
    {
        ScriptLangMap * langmap = N_Only(ScriptHelper).get(sid, lid);
        if (langmap)
        {
            PropertyList::const_iterator pos = mPropertys.find(pid);
            if (pos != mPropertys.end())
            {
                out = langmap->get(pos->second);
                return true;
            }
        }
        out = init;
        return false;
    }
    //------------------------------------------------------------------------
    PropertyData * PropertyData::clone() const
    {
        PropertyData * re = N_new PropertyData();
        re->mPropertys = mPropertys;
        re->mExtPropertys = mExtPropertys;
        return re;
    }
    //------------------------------------------------------------------------
    const String PropertyCmd::PropertyAttribute(_T("Property"));
    const String PropertyCmd::NameAttribute(_T("name"));
    const String PropertyCmd::ValueAttribute(_T("value"));
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PropertyCmd
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PropertyCmd::PropertyCmd(PropertyID id, const String & verbose) :
        mID(id),
        mVerbose(verbose)
    {
    }
    //------------------------------------------------------------------------
    PropertyCmd::PropertyCmd(PropertyID id, const String & name, const String & verbose) :
        mID(id),
        mName(name),
        mVerbose(verbose)
    {
    }
    //------------------------------------------------------------------------
    PropertyCmd::~PropertyCmd()
    {
    }
    //------------------------------------------------------------------------
    void PropertyCmd::init(PropertyCmdObj *) const
    {
    }
    //------------------------------------------------------------------------
    void PropertyCmd::get(const PropertyCmdObj *, String &) const
    {
    }
    //------------------------------------------------------------------------
    void PropertyCmd::get(const PropertyCmdObj *, Any &) const
    {
    }
    //------------------------------------------------------------------------
    void PropertyCmd::set(PropertyCmdObj *, const String &)
    {
    }
    //------------------------------------------------------------------------
    void PropertyCmd::set(PropertyCmdObj *, const Any &)
    {
    }
    //------------------------------------------------------------------------
    void PropertyCmd::create(PropertyCmdObj *, FusionNum *& out) const
    {
        out = 0;
    }
    //------------------------------------------------------------------------
    void PropertyCmd::getDefault(String & out) const
    {
        out = _T("0");
    }
    //------------------------------------------------------------------------
    bool PropertyCmd::isDefault(const PropertyCmdObj * own) const
    {
        String _default, cmp;
        getDefault(_default);
        get(own, cmp);
        return (cmp == _default);
    }
    //------------------------------------------------------------------------
    bool PropertyCmd::isReadable() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool PropertyCmd::isWritable() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool PropertyCmd::isSerialize() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    void PropertyCmd::write(const PropertyCmdObj * obj, XmlSerializer * dst) const
    {
        if (isSerialize())
        {
            dst->begin(PropertyAttribute).attribute(NameAttribute, N_conv(mID));
            String out;
            get(obj, out);
            if (out.find((String::value_type)'\n') != String::npos)
            {
                dst->text(out);
            }
            else
            {
                dst->attribute(ValueAttribute, out);
            }
            dst->end();
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_mutex(PropertyCmdObj::mCmdCollectMapMutex)
    CmdCollectMap PropertyCmdObj::mCmdCollectMap;
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PropertyCmdSet
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PropertyCmdSet::PropertyCmdSet() :
        mOwn(0)
    {
    }
    //------------------------------------------------------------------------
    PropertyCmdSet::PropertyCmdSet(PropertyCmdObj * own):
        mOwn(own)
    {
    }
    //------------------------------------------------------------------------
    PropertyCmdSet::~PropertyCmdSet()
    {
        removeAll();
    }
    //------------------------------------------------------------------------
    void PropertyCmdSet::add(PropertyCmd * cmd)
    {
        PropertyCmdMap::iterator i = mCmdMap.find(cmd->getID());
        if(i != mCmdMap.end())
        {
            mCmdMap.insert(Npair(cmd->getID(), cmd));
        }
        else
        {
            N_delete i->second;
            i->second = cmd;
        }
        cmd->init(mOwn);
    }
    //-----------------------------------------------------------------------
    void PropertyCmdSet::remove(PropertyID pid)
    {
        PropertyCmdMap::iterator i = mCmdMap.find(pid);
        if(i != mCmdMap.end())
        {
            N_delete i->second;
            mCmdMap.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    PropertyCmd * PropertyCmdSet::get(PropertyID pid) const
    {
        PropertyCmdMap::const_iterator i = mCmdMap.find(pid);
        if(i != mCmdMap.end())
        {
            return i->second;
        }
        else
        {
            return 0;
        }
    }
    //-----------------------------------------------------------------------
    PropertyCmd * PropertyCmdSet::get(const String & pname) const
    {
        PropertyCmdMap::const_iterator i, iend = mCmdMap.end();
        for (i = mCmdMap.begin(); i != iend; ++i)
        {
            if(i->second->mName == pname)
                return i->second;
        }

        return 0;
    }
    //-----------------------------------------------------------------------
    void PropertyCmdSet::removeAll()
    {
        PropertyCmdMap::iterator i, iend = mCmdMap.end();
        for(i = mCmdMap.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mCmdMap.clear();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PropertyCmdObj
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PropertyCmdObj::PropertyCmdObj() :
        ScriptProperty(0, N_PrimaryLang),
        mProperty(NULL),
        mCmdObjID(0)
    {
    }
    //-----------------------------------------------------------------------
    PropertyCmdObj::PropertyCmdObj(ScriptTypeID stid, LangID lid) :
        ScriptProperty(stid, lid),
        mProperty(NULL),
        mCmdObjID(0)
    {
    }
    //-----------------------------------------------------------------------
    PropertyCmdObj::~PropertyCmdObj()
    {
    }
    //-----------------------------------------------------------------------
    void PropertyCmdObj::set(const PropertyData & params)
    {
        PropertyData::PropertyList::const_iterator i, iend = params.getList().end();
        for(i = params.getList().begin(); i != iend; ++i)
        {
            set(i->first, i->second);
        }
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::set(const String & pid, const String & value)
    {
        N_assert(mProperty, "error");
        PropertyCmd * cmd = mProperty->get(pid);
        if (cmd)
        {
            cmd->set(this, value);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::set(const String & pid, const Any & out)
    {
        N_assert(mProperty, "error");
        PropertyCmd * cmd = mProperty->get(pid);
        if (cmd)
        {
            cmd->set(this, out);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::set(PropertyID pid, const String & value)
    {
        N_assert(mProperty, "error");
        PropertyCmd * cmd = mProperty->get(pid);
        if(cmd)
        {
            cmd->set(this, value);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::set(PropertyID pid, const Any & out)
    {
        N_assert(mProperty, "error");
        PropertyCmd * cmd = mProperty->get(pid);
        if(cmd)
        {
            cmd->set(this, out);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::get(PropertyID pid, String & value) const
    {
        N_assert(mProperty, "error");
        const PropertyCmd * cmd = mProperty->get(pid);
        if(cmd)
        {
            cmd->get(this, value);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::get(PropertyID pid, Any & out) const
    {
        N_assert(mProperty, "error");
        const PropertyCmd * cmd = mProperty->get(pid);
        if(cmd)
        {
            cmd->get(this, out);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    FusionNum * PropertyCmdObj::createNum(PropertyID pid)
    {
        const PropertyCmd * cmd = mProperty->get(pid);
        if (cmd)
        {
            FusionNum * temp = 0;
            cmd->create(this, temp);
            return temp;
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void PropertyCmdObj::copyProperty(PropertyCmdObj * src)
    {
        N_assert(mProperty, "error");
        String temp;
        PropertyCmdMap::iterator i, iend = mProperty->mCmdMap.end();
        for(i = mProperty->mCmdMap.begin(); i != iend; ++i)
        {
            src->get(i->first, temp);
            set(i->first, temp);
        }
    }
    //-----------------------------------------------------------------------
    void PropertyCmdObj::login()
    {
        N_assert(mCmdObjID, "error");
        N_lock_mutex(mCmdCollectMapMutex)
        CmdCollectMap::iterator i = mCmdCollectMap.find(mCmdObjID);
        if(i == mCmdCollectMap.end())
        {
            mProperty = &mCmdCollectMap.insert(Npair(mCmdObjID, PropertyCmdSet(this))).first->second;
            initCmd(mProperty);
        }
        else
        {
            mProperty = &i->second;
        }
    }
    //-----------------------------------------------------------------------
    void PropertyCmdObj::logout()
    {
        N_assert(mCmdObjID, "error");
        N_lock_mutex(mCmdCollectMapMutex)
        CmdCollectMap::iterator i = mCmdCollectMap.find(mCmdObjID);
        if(i != mCmdCollectMap.end())
        {
            mCmdCollectMap.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void PropertyCmdObj::enterApp()
    {

    }
    //-----------------------------------------------------------------------
    void PropertyCmdObj::enterDesign()
    {

    }
    //-----------------------------------------------------------------------
    void PropertyCmdObj::cleanAll()
    {
        N_lock_mutex(mCmdCollectMapMutex)
        mCmdCollectMap.clear();
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::setupCmd(ClassID cid)
    {
        N_lock_mutex(mCmdCollectMapMutex)
        CmdCollectMap::iterator i = mCmdCollectMap.find(cid);
        if(i == mCmdCollectMap.end())
        {
            mProperty = &mCmdCollectMap.insert(Npair(cid, PropertyCmdSet(this))).first->second;
            mCmdObjID = cid;
            initCmd(mProperty);
            return true;
        }
        else
        {
            mProperty = &i->second;
            mCmdObjID = cid;
            return false;
        }
    }
    //-----------------------------------------------------------------------
    bool PropertyCmdObj::initCmd(PropertyCmdSet *)
    {
        return false;
    }
    //-----------------------------------------------------------------------
}
