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
#include "NiiGroupLoadScheme.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GroupLoadScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GroupLoadScheme::GroupLoadScheme(SchemeID sid, GroupID gid, NCount wl) :
        LoadScheme(sid)
    {
        mWorkloadFactor = gid / wl;
    }
    //------------------------------------------------------------------------
    GroupLoadScheme::~GroupLoadScheme()
    {

    }
    //------------------------------------------------------------------------
    bool GroupLoadScheme::request(LoadingState ls)
    {
        return false;
    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onPreScriptParse(GroupID gid, NCount count)
    {

    }
    //------------------------------------------------------------------------
    bool GroupLoadScheme::onPreScriptParse(const String & scriptName)
    {
        return false;
    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onScriptParseEnd(GroupID gid)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onScriptParseEnd(const String & scriptName)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onPrePlan(GroupID gid, NCount count)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onPrePlan(const Resource * r)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onPlanEnd(GroupID gid)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onPlanEnd(const Resource * r)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onPreLoad(GroupID gid, NCount count)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onPreLoad(const Resource * r)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onLoadEnd(const Resource * r)
    {

    }
    //------------------------------------------------------------------------
    void GroupLoadScheme::onLoadEnd(GroupID gid)
    {

    }
    //------------------------------------------------------------------------
}