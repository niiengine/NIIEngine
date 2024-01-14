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

#include "NiiWin32ControlPatternPlugin.h"

namespace NII
{
namespace NII_MEIDA
{
    //------------------------------------------------------------------------
    Win32ControlPatternManagerPlugin::Win32ControlPatternPlugin() :
        mInstance(0){}
    //------------------------------------------------------------------------
    void Win32ControlPatternManagerPlugin::install()
    {
        mInstance = N_new Win32ControlPatternManager();
    }
    //------------------------------------------------------------------------
    void Win32ControlPatternManagerPlugin::init()
    {
        N_Engine().set(mInstance);
    }
    //------------------------------------------------------------------------
    void Win32ControlPatternManagerPlugin::shutdown()
    {
        N_Engine().setInput(InputManager::DummyControlPatternManager);
    }
    //------------------------------------------------------------------------
    void Win32ControlPatternManagerPlugin::uninstall()
    {
        N_Engine().setInput(InputManager::DummyControlPatternManager);
        if(mInstance)
        {
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //------------------------------------------------------------------------
    const String & Win32ControlPatternManagerPlugin::getLisence() const
    {
        static const String re = "www.niiengine.com/license";
        return re;
    }
    //------------------------------------------------------------------------
    const String & Win32ControlPatternManagerPlugin::getOwnVersion() const
    {
        static const String re = "www.niiengine.com/license";
        return re;
    }
    //------------------------------------------------------------------------
    const String & Win32ControlPatternManagerPlugin::getVerbose()const
    {
        static const String re = "www.niiengine.com/license";
        return re;
    }
    //------------------------------------------------------------------------
    const String & Win32ControlPatternManagerPlugin::getName() const
    {
        static const String re = "NII-Plugin-Win32-ControlPattern-Manager";
        return re;
    }
    //------------------------------------------------------------------------
    NIIb Win32ControlPatternManagerPlugin::getComplex()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb Win32ControlPatternManagerPlugin::getExtention()const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}
}
