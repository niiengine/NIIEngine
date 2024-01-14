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

#include "NiiPVRTCCodecPlugin.h"
#include "NiiPVRTCCodec.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

namespace NII
{
    //------------------------------------------------------------------------
    PVRTCCodecPlugin::PVRTCCodecPlugin(){}
    //------------------------------------------------------------------------
    const String & PVRTCCodecPlugin::getLisence() const
    {
        static const String lis = "NII INTEL";
        return lis;
    }
    //--------------------------------------------------------------------
    const String & PVRTCCodecPlugin::getOwnVersion() const
    {   // 和NII版本一样
        static const String version = "3.0.0";
        return version;
    }
    //--------------------------------------------------------------------
    const String & PVRTCCodecPlugin::getVerbose()const
    {
        static const String des = "support opengl-pvrt image format";
        return des;
    }
    //--------------------------------------------------------------------
    const String & PVRTCCodecPlugin::getName() const
    {
        static const String name = "NII-Plugin-PVRT";
        return name;
    }
    //--------------------------------------------------------------------
    void PVRTCCodecPlugin::install()
    {
        if (!mInstance)
        {
            N_Only(Log).log(LML_NORMAL,_T("PVRTC codec registering"));

            mInstance = N_new PVRTCCodec();
            Codec::add(mInstance);
        }
    }
    //--------------------------------------------------------------------
    void PVRTCCodecPlugin::init()
    {

    }
    //--------------------------------------------------------------------
    void PVRTCCodecPlugin::shutdown()
    {
    }
    //--------------------------------------------------------------------
    void PVRTCCodecPlugin::uninstall()
    {
        if(mInstance)
        {
            Codec::remove(mInstance);
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //--------------------------------------------------------------------
    NIIb PVRTCCodecPlugin::getComplex() const
    {
        return 0;
    }
    //--------------------------------------------------------------------
    NIIb PVRTCCodecPlugin::getExtention() const
    {
        return 1;
    }
    //--------------------------------------------------------------------
}