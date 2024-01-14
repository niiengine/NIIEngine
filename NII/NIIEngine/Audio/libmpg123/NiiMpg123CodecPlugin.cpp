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

#include "NiiMpg123CodecPlugin.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

namespace NII
{
    //--------------------------------------------------------------------
    Mpg123Codec * Mpg123CodecPlugin::mInstance = 0;
    //--------------------------------------------------------------------
    Mpg123CodecPlugin::Mpg123CodecPlugin(){}
    //--------------------------------------------------------------------
    const String & Mpg123CodecPlugin::getLisence() const
    {
        static const String lis = "LGPL 2.1";
        return lis;
    }
    //--------------------------------------------------------------------
    const String & Mpg123CodecPlugin::getOwnVersion() const
    {   
        static const String version = "1.13.4";
        return version;
    }
    //--------------------------------------------------------------------
    const String & Mpg123CodecPlugin::getVerbose()const
    {
        static const String des = "support mp3 voice format";
        return des;
    }
    //--------------------------------------------------------------------
    const String & Mpg123CodecPlugin::getName() const
    {
        static const String name = "NII-Plugin-MPG123";
        return name;
    }
    //--------------------------------------------------------------------
    void Mpg123CodecPlugin::install()
    {
        if (!mInstance)
        {
            NIIi result = mpg123_init();
            if (result != MPG123_OK)
            {
                String ErrorStr = mpg123_plain_strerror(result);

                N_EXCEPT(NoExist,
                    _T("MPG123: Couldn't initialize mpg123 ") + String(ErrorStr));
            }
            else
            {
                N_Only(Log).log(LML_NORMAL, _T("MP3 codec registering"));
                mInstance = N_new Mpg123Codec();
                Codec::add(mInstance);
            }            
        }    
    }
    //--------------------------------------------------------------------
    void Mpg123CodecPlugin::init()
    {
    
    }
    //--------------------------------------------------------------------
    void Mpg123CodecPlugin::shutdown()
    {
    }
    //--------------------------------------------------------------------
    void Mpg123CodecPlugin::uninstall()
    {
        if(mInstance)
        {
            Codec::remove(mInstance);
            N_delete mInstance;
            mInstance = 0;
            mpg123_exit();
        }    
    }
    //--------------------------------------------------------------------
    NIIb Mpg123CodecPlugin::getComplex() const
    {
        return 1;
    }
    //--------------------------------------------------------------------
    NIIb Mpg123CodecPlugin::getExtention() const
    {
        return 1;
    }
    //--------------------------------------------------------------------
}