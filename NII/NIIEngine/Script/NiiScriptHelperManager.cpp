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
#include "NiiScriptHelperManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ScriptHelperManager
    //-----------------------------------------------------------------------
    //------------------------------------------------------------------------
    const PropertyID ScriptHelperManager::UnitPropertyStart = 65536;
    //-----------------------------------------------------------------------
    template<> ScriptHelperManager * Singleton<ScriptHelperManager>::mOnly = 0;
    //------------------------------------------------------------------------
    ScriptHelperManager::ScriptHelperManager()
    {
    }
    //------------------------------------------------------------------------
    ScriptHelperManager::~ScriptHelperManager()
    {
        ScriptLangs::iterator i, iend = mScriptLangs.end();
        for(i = mScriptLangs.begin(); i != iend; ++i)
        {
            LangList::iterator z, zend = i->second.end();
            for(z = i->second.begin(); z != zend; ++z)
            {
                N_delete z->second;
            }
            i->second.clear();
        }
        mScriptLangs.clear();
    }
    //------------------------------------------------------------------------
    void ScriptHelperManager::add(ScriptTypeID sid, ScriptLangMap * sl)
    {
        ScriptLangs::iterator i = mScriptLangs.find(sid);
        if(i == mScriptLangs.end())
        {
            i = mScriptLangs.insert(Npair(sid, LangList())).first;
        }

        LangList::iterator j = i->second.find(sl->getLangID());
        if(j == i->second.end())
        {
            i->second.insert(Npair(sl->getLangID(), sl));
        }
    }
    //------------------------------------------------------------------------
    void ScriptHelperManager::remove(ScriptTypeID sid, LangID lid)
    {
        ScriptLangs::iterator i = mScriptLangs.find(sid);
        if(i != mScriptLangs.end())
        {
            LangList::iterator z = i->second.find(lid);
            if(z != i->second.end())
            {
                N_delete z->second;
                i->second.erase(z);
                if (i->second.size() == 0)
                {
                    mScriptLangs.erase(i);
                }
            }

        }
    }
    //------------------------------------------------------------------------
    void ScriptHelperManager::remove(ScriptTypeID sid)
    {
        ScriptLangs::iterator i = mScriptLangs.find(sid);
        if (i != mScriptLangs.end())
        {
            LangList::iterator z, zend = i->second.end();
            for (z = i->second.begin(); z != zend; ++z)
            {
                N_delete z->second;
            }
            mScriptLangs.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void ScriptHelperManager::removeAll()
    {
        ScriptLangs::iterator i, iend = mScriptLangs.end();
        for (i = mScriptLangs.begin(); i != iend; ++i)
        {
            LangList::iterator z, zend = i->second.end();
            for (z = i->second.begin(); z != zend; ++z)
            {
                N_delete z->second;
            }
        }
        mScriptLangs.clear();
    }
    //------------------------------------------------------------------------
    ScriptLangMap * ScriptHelperManager::get(ScriptTypeID sid, LangID lid) const
    {
        ScriptLangs::const_iterator i = mScriptLangs.find(sid);
        if(i != mScriptLangs.end())
        {
            LangList::const_iterator j = i->second.find(lid);
            if(j != i->second.end())
            {
                return j->second;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ScriptHelperManager::addUnit(LangID lid, ScriptTypeID sid, const String & m)
    {
        TokenLangs::iterator i = mTokenLangs.find(lid);
        if(i == mTokenLangs.end())
        {
            i = mTokenLangs.insert(Npair(lid, TokenList())).first;
        }
        i->second.insert(Npair(m, sid));
    }
    //------------------------------------------------------------------------
    void ScriptHelperManager::removeUnit(LangID lid, ScriptTypeID sid)
    {
        TokenLangs::iterator i = mTokenLangs.find(lid);
        if(i != mTokenLangs.end())
        {
            TokenList::iterator j, jend = i->second.end();
            for(j = i->second.begin(); j != jend; ++j)
            {
                if(sid == j->second)
                {
                    i->second.erase(j);
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    ScriptTypeID ScriptHelperManager::getUnit(LangID lid, const String & m) const
    {
        TokenLangs::const_iterator i = mTokenLangs.find(lid);
        if(i != mTokenLangs.end())
        {
            TokenList::const_iterator j = i->second.find(m);
            if(j != i->second.end())
            {
                return j->second;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    const String & ScriptHelperManager::getUnit(LangID lid, ScriptTypeID m) const
    {
        TokenLangs::const_iterator i = mTokenLangs.find(lid);
        if (i != mTokenLangs.end())
        {
            TokenList::const_iterator j, jend = i->second.end();
            for (j = i->second.begin(); j != jend; ++j)
            {
                if (j->second == m)
                    return j->first;
            }
        }
        return N_StrBlank;
    }
    //------------------------------------------------------------------------
    void ScriptHelperManager::removeAllUnit()
    {
        mTokenLangs.clear();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ScriptLangMap
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ScriptLangMap::ScriptLangMap()
    {
    }
    //------------------------------------------------------------------------
    ScriptLangMap::ScriptLangMap(LangID lid):
        mLangID(lid)
    {
    }
    //------------------------------------------------------------------------
    ScriptLangMap::~ScriptLangMap()
    {
        mPropertyMap.clear();
    }
    //------------------------------------------------------------------------
    void ScriptLangMap::add(PropertyID pid, const String & m)
    {
        mPropertyMap.insert(PropertyMap::value_type(m, pid));
    }
    //------------------------------------------------------------------------
    void ScriptLangMap::remove(PropertyID pid)
    {
        PropertyMap::iterator i, iend = mPropertyMap.end();
        for(i = mPropertyMap.begin(); i != iend; ++i)
        {
            if(i->second == pid)
            {
                mPropertyMap.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    PropertyID ScriptLangMap::get(const String & name) const
    {
        PropertyMap::const_iterator i = mPropertyMap.find(name);
        if(i != mPropertyMap.end())
        {
            return i->second;
        }
        return N_Only(ScriptHelper).getUnit(mLangID, name);
    }
    //------------------------------------------------------------------------
    const String & ScriptLangMap::get(PropertyID pid) const
    {
        PropertyMap::const_iterator i, iend = mPropertyMap.end();
        for(i = mPropertyMap.begin(); i != iend; ++i)
        {
            if(i->second == pid)
            {
                return i->first;
            }
        }
        return N_StrBlank;
    }
    //------------------------------------------------------------------------
}