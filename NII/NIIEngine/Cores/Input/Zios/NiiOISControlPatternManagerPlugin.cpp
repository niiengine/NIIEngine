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

#include "NiiOISControlPatternManagerPlugin.h"

namespace NII
{
namespace NII_MEIDA
{
    //------------------------------------------------------------------------
    OISControlPatternManagerPlugin::OISControlPatternManagerPlugin() :
        mInstance(0){}
    //------------------------------------------------------------------------
    void OISControlPatternManagerPlugin::install()
    {
        mInstance = N_new OISControlPatternManager();
        N_Engine().set(mInstance);
    }
    //------------------------------------------------------------------------
    void OISControlPatternManagerPlugin::init()
    {

    }
    //------------------------------------------------------------------------
    void OISControlPatternManagerPlugin::shutdown()
    {
        // do nothing
    }
    //------------------------------------------------------------------------
    void OISControlPatternManagerPlugin::uninstall()
    {
        N_Engine().setInput(InputManager::DummyControlPatternManager);
        if(mInstance)
        {
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //------------------------------------------------------------------------
    const String & OISControlPatternManagerPlugin::getLisence() const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & OISControlPatternManagerPlugin::getOwnVersion() const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & OISControlPatternManagerPlugin::getVerbose()const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & OISControlPatternManagerPlugin::getName() const
    {
        static const String re = "NII-Plugin-OISControlPatternManager";
        return re;
    }
    //------------------------------------------------------------------------
    NIIb OISControlPatternManagerPlugin::getComplex()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb OISControlPatternManagerPlugin::getExtention()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}
}
