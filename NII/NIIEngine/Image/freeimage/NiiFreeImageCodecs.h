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

#ifndef _NII_FreeImageCodec_H_
#define _NII_FreeImageCodec_H_

#include "NiiPreInclude.h"
#include "NiiCodec.h"

#include <FreeImage.h>

namespace NII
{
    */
    /** 一个使用FreeImage专门为图片解码的解码器.
    @remark
        使用ImageCodec子类的实现为ImageData类返回一个有效的解码函数(...)的指针
    */
    class _EngineAPI FreeImageCodec : public Codec
    {
    public:
        FreeImageCodec(CodecID cid, const String & hint, FREE_IMAGE_FORMAT fiType);

        virtual ~FreeImageCodec() {}

        /// @copydetails Codec::encode
        CodecResult encode(const Image * in, const String & out, const String & hint) const;

        /// @copydetails Codec::encode
        CodecResult encode(const Image * in, DataStream *& out, const String & hint) const;
        
        /// @copydetails Codec::decode
        CodecResult decode(const String & in, Image * out) const;
        
        /// @copydetails Codec::decode
        CodecResult decode(DataStream * in, Image * out) const;

        /// @copydetails Codec::match
        bool match(const String & hint) const;

        /// @copydetails Codec::match
        String match(const Nui8 * magic, NCount valid) const;
    private:
        FIBITMAP * _encode(const Image * in) const;
        CodecResult _decode(FIBITMAP * in, Image * out) const;
    private:
        FREE_IMAGE_FORMAT mOwnType;   ///< freeimage 内部标识
    };
}
#endif