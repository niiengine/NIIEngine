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

#include "NiiDDSCodecPlugin.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

namespace NII
{
    //--------------------------------------------------------------------
    DDSCodec * DDSCodecPlugin::mInstance = 0;
    //--------------------------------------------------------------------
    DDSCodecPlugin::DDSCodecPlugin()
    {
    }
    //--------------------------------------------------------------------
    const String & DDSCodecPlugin::getLisence() const
    {
        static const String lis = "NII INTEL";
        return lis;
    }
    //--------------------------------------------------------------------
    const String & DDSCodecPlugin::getOwnVersion() const
    {   
        static const String version = "3.0.0";
        return version;
    }
    //--------------------------------------------------------------------
    const String & DDSCodecPlugin::getVerbose()const
    {
        static const String des = "support dds image format";
        return des;
    }
    //--------------------------------------------------------------------
    const String & DDSCodecPlugin::getName() const
    {
        static const String name = "NII-Plugin-DDS";
        return name;
    }
    //--------------------------------------------------------------------
    void DDSCodecPlugin::install()
    {
        if (!mInstance)
        {
            N_Only(Log).log(LML_NORMAL, _T("DDS codec registering"));

            mInstance = N_new DDSCodec();
            Codec::add(mInstance);
        }    
    }
    //--------------------------------------------------------------------
    void DDSCodecPlugin::init()
    {
    
    }
    //--------------------------------------------------------------------
    void DDSCodecPlugin::shutdown()
    {
    }
    //--------------------------------------------------------------------
    void DDSCodecPlugin::uninstall()
    {
        if(mInstance)
        {
            Codec::remove(mInstance);
            N_delete mInstance;
            mInstance = 0;
        }    
    }
    //--------------------------------------------------------------------
    NIIb DDSCodecPlugin::getComplex() const
    {
        return 1;
    }
    //--------------------------------------------------------------------
    NIIb DDSCodecPlugin::getExtention() const
    {
        return 1;
    }
    //--------------------------------------------------------------------
}