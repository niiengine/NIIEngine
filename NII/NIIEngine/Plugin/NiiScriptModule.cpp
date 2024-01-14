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
#include "NiiScriptModule.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"

namespace NII
{
    //------------------------------------------------------------------------
    GroupID ScriptModule::mResourceGroup;
    //------------------------------------------------------------------------
    ScriptModule::ScriptModule() :
        mID(0)
    {
    }
    //------------------------------------------------------------------------
    ScriptModule::~ScriptModule()
    {

    }
    //------------------------------------------------------------------------
    void ScriptModule::load() 
    {

    }
    //------------------------------------------------------------------------
    void ScriptModule::unload() 
    {

    }
    //------------------------------------------------------------------------
    ScriptModuleID ScriptModule::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    void ScriptModule::setGroup(GroupID gid)
    {
        mResourceGroup = gid;
    }
    //------------------------------------------------------------------------
    GroupID ScriptModule::getGroup()
    {
        return mResourceGroup;
    }
    //------------------------------------------------------------------------
    bool ScriptFunctor::operator()(const EventArgs & e) const
    {
        ScriptModule * scriptModule = N_Engine().getScriptModule(mModuleID);

        if (scriptModule)
        {
            return scriptModule->executeFunc(mFuncName, e);
        }
        else
        {
            N_Only(Log).log(_I18n("Scripted event handler '") + mFuncName +
                _I18n("' could not be called as no ScriptModule is available."));

            return false;
        }
    }
    //------------------------------------------------------------------------
}