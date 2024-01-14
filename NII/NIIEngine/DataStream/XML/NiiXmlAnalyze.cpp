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
#include "NiiXmlAnalyze.h"
#include "NiiXmlPattern.h"
#include "NiiXmlAnalyzeManager.h"
#include "NiiScriptHelperManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    XmlAnalyze::XmlAnalyze(VersionID version, ScriptTypeID stid, LangID lid) :
        ScriptProperty(stid, lid),
        mTag(0),
        mVersion(version),
        mAutoTag(true),
        mSkipOther(false)
    {
    }
    //-----------------------------------------------------------------------
    XmlAnalyze::~XmlAnalyze()
    {
        if(mAutoTag && mTag)
        {
            N_Only(XmlAnalyze).destroy(mTag);
            mTag = 0;
        }
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::setSkipOther(bool b)
    {
        mSkipOther = b;
    }
    //-----------------------------------------------------------------------
    bool XmlAnalyze::isSkipOther() const
    {
        return mSkipOther;
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::setBeginPrc(PropertyID pid, PrcCB prc)
    {
        PrcCBMap::iterator i = mBeginMap.find(pid);
        if(i != mBeginMap.end())
        {
            i->second = prc;
        }
        else
        {
            mBeginMap.insert(PrcCBMap::value_type(pid, prc));
        }
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::unBeginPrc(PropertyID pid)
    {
        PrcCBMap::iterator i = mBeginMap.find(pid);
        if(i != mBeginMap.end())
        {
            mBeginMap.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::setEndPrc(PropertyID pid, PrcCB prc)
    {
        PrcCBMap::iterator i = mEndMap.find(pid);
        if(i != mEndMap.end())
        {
            i->second = prc;
        }
        else
        {
            mEndMap.insert(PrcCBMap::value_type(pid, prc));
        }
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::unEndPrc(PropertyID pid)
    {
        PrcCBMap::iterator i = mEndMap.find(pid);
        if(i != mEndMap.end())
        {
            mEndMap.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc)
    {
        if(mTag && !mSkipOther)
        {
            mTag->onBegin(this, pid, pdc);
        }
        else if(pid < ScriptHelperManager::UnitPropertyStart)
        {
            PrcCBMap::iterator i = mBeginMap.find(pid);
            if(i != mBeginMap.end())
            {
                (this->*i->second)(pdc);
            }
        }
        else if(pid == getUnit())
        {
            PrcCBMap::iterator i = mBeginMap.find(pid);
            if(i != mBeginMap.end())
            {
                (this->*i->second)(pdc);
            }
            else
            {
                PrcTagUnitCB(pdc);
            }
        }
        else
        {
            mTag = N_Only(XmlAnalyze).create((FactoryID)pid, mLangMap->getLangID());
            if(mTag && !mSkipOther)
            {
                mTag->onBegin(this, pid, pdc);
            }
        }
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc)
    {
        if(mTag)
        {
            if(!mSkipOther)
                mTag->onEnd(this, pid, pdc);
            if(mTag->getUnit() == pid)
            {
                if(mAutoTag)
                    N_Only(XmlAnalyze).destroy(mTag);
                mTag = 0;
            }
        }
        else if(pid == getUnit())
        {
            PrcCBMap::iterator i = mEndMap.find(pid);
            if(i != mEndMap.end())
            {
                (this->*i->second)(pdc);
            }
            else
            {
                PrcTagUnitECB(pdc);
            }
        }
        else
        {
            PrcCBMap::iterator i = mEndMap.find(pid);
            if(i != mEndMap.end())
            {
                (this->*i->second)(pdc);
            }
        }
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::onData(const String & data)
    {
        (void)data;
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::onData(const Nutf8 * data)
    {
        String str;
        StrConv::conv(data, str);
        onData(str);
    }
    //-----------------------------------------------------------------------
    const String & XmlAnalyze::getXSD() const
    {
        return N_StrBlank;
    }
    //-----------------------------------------------------------------------
    void XmlAnalyze::PrcTagUnitECB(const PropertyData *)
    {
    }
    //-----------------------------------------------------------------------
}