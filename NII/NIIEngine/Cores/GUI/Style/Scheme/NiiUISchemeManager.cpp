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
#include "NiiUISchemeManager.h"
#include "NiiUISchemeXmlAnalyze.h"
#include "NiiLogManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiEngine.h"
#include "NiiXmlPattern.h"

namespace NII
{
    template<> UI::SchemeManager * Singleton<UI::SchemeManager>::mOnly = 0;
namespace UI
{
    //------------------------------------------------------------------------
    GroupID SchemeManager::ResourceGroup = 0;
    //------------------------------------------------------------------------
    SchemeManager::SchemeManager() :
        mAutoLoad(true)
    {
    }
    //------------------------------------------------------------------------
    SchemeManager::~SchemeManager()
    {
        N_Only(Log).log(_I18n("---- Begining cleanup of GUI Scheme system ----"));

        destroyAll();
    }
    //------------------------------------------------------------------------
    void SchemeManager::createDefine(const String & pattern, GroupID gid)
    {
        if (gid == 0)
            gid = ResourceGroup;
        StringList names;
        N_Only(ResourceScheme).find(gid, names, pattern);
        StringList::iterator i, iend = names.end();
        for(i = names.begin(); i != iend; ++i)
            create(*i, gid);
    }
    //------------------------------------------------------------------------
    Scheme * SchemeManager::create(const String & file, GroupID gid)
    {
        if (gid == 0 || gid == GroupUnknow)
            gid = ResourceGroup;

        SchemeXmlAnalyze sxa;

        XmlPattern * xprc = N_EnginePtr()->getXmlPattern();

        DataStream * data = N_Only(ResourceScheme).open(file, gid);
        
        xprc->parse(&sxa, data);
        
        if(isExist(sxa.getObjID()))
            return

        mObjectList[sxa.getObjID()] = sxa.getObject();

        if(mAutoLoad)
        {
            sxa.getObject()->loadResources();
        }

        return sxa.getObject();
    }
    //------------------------------------------------------------------------
    void SchemeManager::destroy(Nid id)
    {
        ObjectList::iterator i = mObjectList.find(id);
        if(i != mObjectList.end())
        {
            N_delete i->second;
            mObjectList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    Scheme * SchemeManager::get(SchemeID id) const
    {
        ObjectList::const_iterator i = mObjectList.find(id);
        if(i != mObjectList.end())
            return i->second;

        return 0;
    }
    //------------------------------------------------------------------------
    void SchemeManager::destroyAll()
    {
        ObjectList::const_iterator i, iend = mObjectList.end();
        for(i = mObjectList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mObjectList.clear();
    }
    //------------------------------------------------------------------------
    bool SchemeManager::isExist(Nid id) const
    {
        return mObjectList.find(id) != mObjectList.end();
    }
    //------------------------------------------------------------------------
}
}