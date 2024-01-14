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

#include "NiiLinuxControlPatternManagerPlugin.h"

namespace NII
{
namespace NII_MEIDA
{
    //------------------------------------------------------------------------
    LinuxControlPatternManagerPlugin::LinuxControlPatternManagerPlugin() :
        mInstance(0){}
    //------------------------------------------------------------------------
    void LinuxControlPatternManagerPlugin::install()
    {
        mInstance = N_new LinuxControlPatternManager();
        N_Engine().set(mInstance);
    }
    //------------------------------------------------------------------------
    void LinuxControlPatternManagerPlugin::init()
    {

    }
    //------------------------------------------------------------------------
    void LinuxControlPatternManagerPlugin::shutdown()
    {
        // do nothing
    }
    //------------------------------------------------------------------------
    void LinuxControlPatternManagerPlugin::uninstall()
    {
        N_Engine().setInput(InputManager::DummyControlPatternManager);
        if(mInstance)
        {
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //------------------------------------------------------------------------
    const String & LinuxControlPatternManagerPlugin::getLisence() const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & LinuxControlPatternManagerPlugin::getOwnVersion() const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & LinuxControlPatternManagerPlugin::getVerbose()const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & LinuxControlPatternManagerPlugin::getName() const
    {
        static const String re = "NII-Plugin-LinuxControlPatternManager";
        return re;
    }
    //------------------------------------------------------------------------
    NIIb LinuxControlPatternManagerPlugin::getComplex()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb LinuxControlPatternManagerPlugin::getExtention()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}
}
