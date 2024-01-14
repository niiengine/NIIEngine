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
#include "NiiImageCodec.h"

namespace NII
{
    //------------------------------------------------------------------------
    ImageCodec::ImageCodec(CodecID cid, const String & hint) :
        Codec(cid, hint)
    {
    }
    //------------------------------------------------------------------------
    ImageCodec::~ImageCodec()
    {
    }
    //------------------------------------------------------------------------
    CodecResult ImageCodec::encode(const Image * in, const String & out, const String & hint, NCount mipBegin, NCount mipCount) const
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult ImageCodec::encode(const Image * in, DataStream *& out, const String & hint, NCount mipBegin, NCount mipCount) const
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult ImageCodec::decode(const String & in, Image * out) const
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
    CodecResult ImageCodec::decode(DataStream * in, Image * out) const
    {
        return CodecResult_NoImpl;
    }
    //------------------------------------------------------------------------
}