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
    //------------------------------------------------------------------------
    OggCodec * OggCodecPlugin::mInstance = 0;
    //------------------------------------------------------------------------
    OggCodecPlugin::OggCodecPlugin(){}
    //------------------------------------------------------------------------
    const String & OggCodecPlugin::getLisence() const
    {
        static const String lis = "BSD";
        return lis;
    }
    //------------------------------------------------------------------------
    const String & OggCodecPlugin::getOwnVersion() const
    {
        static const String version = vorbis_version_string();
        return version;
    }
    //------------------------------------------------------------------------
    const String & OggCodecPlugin::getVerbose()const
    {
        static const String des = "support ogg voice format";
        return des;
    }
    //------------------------------------------------------------------------
    const String & OggCodecPlugin::getName() const
    {
        static const String name = "NII-Plugin-OGG";
        return name;
    }
    //------------------------------------------------------------------------
    void OggCodecPlugin::install()
    {
        if (!mInstance)
        {
            N_Only(Log).log(LML_NORMAL, _T("OGG codec registering"));

            mInstance = N_new OggCodec();
            register_protocol();
            Codec::add(mInstance);
        }
    }
    //------------------------------------------------------------------------
    void OggCodecPlugin::init()
    {

    }
    //------------------------------------------------------------------------
    void OggCodecPlugin::shutdown()
    {
    }
    //------------------------------------------------------------------------
    void OggCodecPlugin::uninstall()
    {
        if(mInstance)
        {
            Codec::remove(mInstance);
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //------------------------------------------------------------------------
    NIIb OggCodecPlugin::getComplex() const
    {
        return 1;
    }
    //------------------------------------------------------------------------
    NIIb OggCodecPlugin::getExtention() const
    {
        return 1;
    }
    //-----------------------------------------------------------------------
}