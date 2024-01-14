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
#include "NiiScriptProperty.h"
#include "NiiScriptHelperManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    N_PROPERTY(NII, ScriptProperty, PropertyDefine,     0);
    N_PROPERTY(NII, ScriptProperty, IDProperty,         1);
    N_PROPERTY(NII, ScriptProperty, NameProperty,       2);
    N_PROPERTY(NII, ScriptProperty, GroupProperty,      3);
    N_PROPERTY(NII, ScriptProperty, TypeProperty,       4);
    N_PROPERTY(NII, ScriptProperty, SourceProperty,     5);
    N_PROPERTY(NII, ScriptProperty, ValueProperty,      6);
    N_PROPERTY(NII, ScriptProperty, VersionProperty,    7);
    N_PROPERTY(NII, ScriptProperty, TrueProperty,       8);
    N_PROPERTY(NII, ScriptProperty, FalseProperty,      9);
    N_PROPERTY(NII, ScriptProperty, PropertyCount,      10);
    //------------------------------------------------------------------------
    ScriptProperty::ScriptProperty(ScriptTypeID stid, LangID lid) :
        mScriptTypeID(stid)
    {
        mLangMap = N_Only(ScriptHelper).get(stid, lid);
    }
    //-----------------------------------------------------------------------
    ScriptProperty::~ScriptProperty()
    {
    }
    //------------------------------------------------------------------------
    PropertyID ScriptProperty::getProperty(const String & name) const
    {
        return mLangMap->get(name);
    }
    //-----------------------------------------------------------------------
    const String & ScriptProperty::getLang(PropertyID pid) const
    {
        return mLangMap->get(pid);
    }
    //-----------------------------------------------------------------------
    LangID ScriptProperty::getLangID() const
    {
        return mLangMap->getLangID();
    }
    //-----------------------------------------------------------------------
}