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
#include "NiiExtData.h"

namespace NII
{
    //--------------------------------------------------------------------------
    ExtData::ExtData()
    {
        mDataList.insert(Npair((Nid)0, Npair((void *)0, false)));
    }
    //--------------------------------------------------------------------------
    ExtData::~ExtData()
    {
        mDataList.clear();
    }
    //-----------------------------------------------------------------------
    void ExtData::setExtData(void * data, bool autodestroy)
    {
        DataList::iterator i = mDataList.find(0);
        if(i != mDataList.end())
        {
            if(i->second.second)
            {
                N_free(i->second.first);
            }
            i->second.first = data;
            i->second.second = autodestroy;
            return;
        }
        mDataList.insert(Npair((Nid)0, Npair(data, autodestroy)));
    }
    //-----------------------------------------------------------------------
    void * ExtData::getExtData() const
    {
        DataList::const_iterator i = mDataList.find(0);
        if(i != mDataList.end())
        {
            return i->second.first;
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void * ExtData::getIndexData(Nidx idx)
    {
        if(mDataList.size() > idx)
            return 0;
        DataList::iterator temp = mDataList.begin();
        std::advance(temp, idx);
        return temp->second.first;
    }
    //-----------------------------------------------------------------------
    void ExtData::setExtData(Nid key, void * data, bool autodestroy)
    {
        DataList::iterator i = mDataList.find(key);
        if(i != mDataList.end())
        {
            if(i->second.second)
            {
                N_free(i->second.first);
            }
            i->second.first = data;
            i->second.second = autodestroy;
            return;
        }
        mDataList.insert(Npair(key, Npair(data, autodestroy)));
    }
    //-----------------------------------------------------------------------
    void * ExtData::getExtData(Nid key) const
    {
        DataList::const_iterator i = mDataList.find(key);
        if(i != mDataList.end())
        {
            return i->second.first;
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void ExtData::setExtData(Nid key, const String & value)
    {
        DataStrList::iterator i = mStringList.find(key);
        if(i != mStringList.end())
        {
            i->second = value;
        }
        mStringList.insert(std::pair<Nid, String>(key, value));
    }
    //-----------------------------------------------------------------------
    void ExtData::getExtData(Nid key, String & value) const
    {
        DataStrList::const_iterator i = mStringList.find(key);
        if(i != mStringList.end())
        {
            value = i->second;
        }
        value = _T("");
    }
    //-----------------------------------------------------------------------
    void ExtData::clear()
    {
        DataList::iterator i, iend = mDataList.end();
        for(i = mDataList.begin(); i != iend; ++i)
        {
            if(i->second.second)
            {
                N_free(i->second.first);
            }
        }
        mDataList.clear();
        mStringList.clear();
    }
    //-----------------------------------------------------------------------
}
