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

#include "NiiFFmpegACodecPlugin.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

namespace NII
{
    //--------------------------------------------------------------------
    FFmpegACodec * FFmpegACodecPlugin::mInstance = 0;
    //--------------------------------------------------------------------
    FFmpegACodecPlugin::FFmpegACodecPlugin(){}
    //--------------------------------------------------------------------
    const String & FFmpegACodecPlugin::getLisence() const
    {
        static const String lis = String("Format Lisence") + avformat_license() +
            String("Codec Lisence") + avfomat_avcodec_license();
        return lis;
    }
    //--------------------------------------------------------------------
    const String & FFmpegACodecPlugin::getOwnVersion() const
    {
        static const String version = String("Format Version") + avformat_version() +
            String("Codec Version") + avcodec_version();
        return version;
    }
    //--------------------------------------------------------------------
    const String & FFmpegACodecPlugin::getVerbose()const
    {
        static const String des = "support dds image format";
        return des;
    }
    //--------------------------------------------------------------------
    const String & FFmpegACodecPlugin::getName() const
    {
        static const String name = "NII-Plugin-DDS";
        return name;
    }
    //--------------------------------------------------------------------
    void FFmpegACodecPlugin::install()
    {
        if (!mInstance)
        {
            N_Only(Log).log(LML_NORMAL, _T("FFmpegA codec registering"));

            //注册所有可解码类型
            av_register_all();

            mInstance = N_new FFmpegACodec();
            Codec::add(mInstance);
        }
    }
    //--------------------------------------------------------------------
    void FFmpegACodecPlugin::init()
    {

    }
    //--------------------------------------------------------------------
    void FFmpegACodecPlugin::shutdown()
    {
    }
    //--------------------------------------------------------------------
    void FFmpegACodecPlugin::uninstall()
    {
        if(mInstance)
        {
            Codec::remove(mInstance);
            N_delete mInstance;
            mInstance = 0;
        }
    }
    //--------------------------------------------------------------------
    NIIb FFmpegACodecPlugin::getComplex() const
    {
        return 9;
    }
    //--------------------------------------------------------------------
    NIIb FFmpegACodecPlugin::getExtention() const
    {
        return 9;
    }
    //--------------------------------------------------------------------
}