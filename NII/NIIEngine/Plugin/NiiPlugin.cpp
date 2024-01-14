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
#include "NiiPlugin.h"
#include "NiiLogManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    Plugin::Plugin(PluginID pid, Type type) :
        mID(pid),
        mType(type)
    {
    }
    //------------------------------------------------------------------------
    Plugin::~Plugin()
    {
    }
    //------------------------------------------------------------------------
    PluginID Plugin::getID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb Plugin::getPerformance() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb Plugin::getEffectiveness() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb Plugin::getComplex() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb Plugin::getExtention() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void Plugin::log() const
    {
        N_Only(Log).log(LL_Info, this->getName());

        N_Only(Log).log(LL_Info, this->getOwnVersion());
        N_Only(Log).log(LL_Info, this->getLisence());

        N_Only(Log).log(LL_Info, _I18n("Complex Level") + this->getComplex());

        N_Only(Log).log(LL_Info, _I18n("Extention Level") + this->getExtention());

        N_Only(Log).log(LL_Info, this->getVerbose());
    }
    //------------------------------------------------------------------------
}