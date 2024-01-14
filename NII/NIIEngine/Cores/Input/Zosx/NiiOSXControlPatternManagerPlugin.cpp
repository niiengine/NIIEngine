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

#include "NiiOSXControlPatternManagerPlugin.h"

namespace NII
{
namespace NII_MEIDA
{
    //------------------------------------------------------------------------
    OSXControlPatternManagerPlugin::OSXControlPatternManagerPlugin() :
        mInstance(0){}
    //------------------------------------------------------------------------
    void OSXControlPatternManagerPlugin::install()
    {
        mInstance = N_new OSXControlPatternManager();
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManagerPlugin::init()
    {
        N_Engine().set(mInstance);
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManagerPlugin::shutdown()
    {
        N_Engine().setInput(InputManager::DummyControlPatternManager);
    }
    //------------------------------------------------------------------------
    void OSXControlPatternManagerPlugin::uninstall()
    {
        N_Engine().setInput(InputManager::DummyControlPatternManager);
        if(mInstance)
        {
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //------------------------------------------------------------------------
    const String & OSXControlPatternManagerPlugin::getLisence() const
    {
        static const String re = "www.niiengine.com/license";
        return re;
    }
    //------------------------------------------------------------------------
    const String & OSXControlPatternManagerPlugin::getOwnVersion() const
    {
        static const String re = "www.niiengine.com/license";
        return re;
    }
    //------------------------------------------------------------------------
    const String & OSXControlPatternManagerPlugin::getVerbose()const
    {
        static const String re = "www.niiengine.com/license";
        return re;
    }
    //------------------------------------------------------------------------
    const String & OSXControlPatternManagerPlugin::getName() const
    {
        static const String re = "NII-Plugin-OSX-ControlPattern-Manager";
        return re;
    }
    //------------------------------------------------------------------------
    NIIb OSXControlPatternManagerPlugin::getComplex()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb OSXControlPatternManagerPlugin::getExtention()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}
}
