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
    ExpatPlugin::ExpatPlugin(PluginID pid) :
        Plugin(pid, T_Xml_Process),
        mInstance(0)
    {
    }
    //------------------------------------------------------------------------
    ExpatPlugin::~ExpatPlugin()
    {
    }
    //------------------------------------------------------------------------
    String ExpatPlugin::getName()
    {
    }
    //------------------------------------------------------------------------
    String ExpatPlugin::Version() const
    {
    
    }
    //------------------------------------------------------------------------
    String ExpatPlugin::getLisence() const
    {
    }
    //------------------------------------------------------------------------
    String ExpatPlugin::getVerbose() const
    {
    }
    //------------------------------------------------------------------------
    void ExpatPlugin::install()
    {
        mInstance = N_new ExpatParser();
    }
    //------------------------------------------------------------------------
    void ExpatPlugin::init()
    {
        mInstance->startup();
    }
    //------------------------------------------------------------------------
    void ExpatPlugin::shutdown()
    {
        mInstance->shutdown();
    }
    //------------------------------------------------------------------------
    void ExpatPlugin::uninstall()
    {
        N_delete mInstance;
        mInstance = 0;
    }
    //------------------------------------------------------------------------
    String ExpatPlugin::getOwnLisence() const
    {
    }
    //------------------------------------------------------------------------
    String ExpatPlugin::getOwnVersion() const
    {
    }
    //------------------------------------------------------------------------
    NIIb ExpatPlugin::getPerformance() const
    {
    }
    //------------------------------------------------------------------------
    NIIb ExpatPlugin::getEffectiveness() const
    {
    }
    //------------------------------------------------------------------------
    NIIb ExpatPlugin::getComplex() const
    {
    }
    //------------------------------------------------------------------------
    NIIb ExpatPlugin::getExtention() const
    {
    }
    //------------------------------------------------------------------------
}