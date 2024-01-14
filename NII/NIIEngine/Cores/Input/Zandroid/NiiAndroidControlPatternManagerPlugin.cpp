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

#include "NiiAndroidControlPatternManagerPlugin.h"

namespace NII
{
namespace NII_MEIDA
{
    //------------------------------------------------------------------------
    AndroidControlPatternManagerPlugin::AndroidControlPatternManagerPlugin() :
        mInstance(0){}
    //------------------------------------------------------------------------
    const String & AndroidControlPatternManagerPlugin::getLisence(void) const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & AndroidControlPatternManagerPlugin::getOwnVersion(void) const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & AndroidControlPatternManagerPlugin::getVerbose(void)const
    {
        static const String re = "";
        return re;
    }
    //------------------------------------------------------------------------
    const String & AndroidControlPatternManagerPlugin::getName() const
    {
        static const String re = "NII-Plugin-AndroidControlPatternManager";
        return re;
    }
    //------------------------------------------------------------------------
    void AndroidControlPatternManagerPlugin::install()
    {
        mInstance = N_new AndroidControlPatternManager();
        N_Engine().set(mInstance);
    }
    //------------------------------------------------------------------------
    void AndroidControlPatternManagerPlugin::init()
    {

    }
    //------------------------------------------------------------------------
    void AndroidControlPatternManagerPlugin::shutdown()
    {
        // do nothing
    }
    //------------------------------------------------------------------------
    void AndroidControlPatternManagerPlugin::uninstall()
    {
        N_Engine().setInput(InputManager::DummyControlPatternManager);
        if(mInstance)
        {
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //------------------------------------------------------------------------
    NIIb AndroidControlPatternManagerPlugin::getComplex(void)const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    NIIb AndroidControlPatternManagerPlugin::getExtention(void)const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}
}
