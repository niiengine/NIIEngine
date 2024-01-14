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
#include "NiiAudioCodec.h"

namespace NII
{
    //------------------------------------------------------------------------
    AudioCodec::AudioCodec(CodecID cid, const String & hint):
        Codec(cid, hint)
    {       
    }
    //------------------------------------------------------------------------
    AudioCodec::~AudioCodec()
    {
    }
    //------------------------------------------------------------------------
    bool AudioCodec::isSeekable() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    CodecResult AudioCodec::encode(const Audio * in, const String & out, const String & hint) const
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult AudioCodec::encode(const Audio * in, DataStream *& out, const String & hint) const
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult AudioCodec::open(const String & in)
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult AudioCodec::open(DataStream * in)
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult AudioCodec::decode(Audio * out, TimeDurMS time) const
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult AudioCodec::seek(TimeTotalMS time)
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult AudioCodec::close()
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
}