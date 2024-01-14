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

#ifndef _NII_ILIMAGECODEC_H_
#define _NII_ILIMAGECODEC_H_

#include "NiiPreInclude.h"
#include "NiiImageCodec.h"

#include <IL/il.h>
#include <IL/ilu.h>

namespace NII
{
    /** 一个使用DevIL专门为图片解码的解码器.
    @version NIIEngine 3.0.0 DevIL第三方扩展
    */
    class _EngineAPI ILImageCodec : public Codec
    {
    public:
        ILImageCodec(CodecID cid, const String & hint, ILenum iltype);

        virtual ~ILImageCodec();

        /// 初始化 DevIL
        void initialiseIL(void);

        /// @copydetails Codec::encode
        CodecResult encode(const Image * in, const String & out, const String & hint) const;

        /// @copydetails Codec::decode
        CodecResult decode(DataStream * in, Image * out) const;

        /// @copydetails Codec::match
        bool match(const String & hint) const;

        /// @copydetails Codec::match
        String match(const Nui8 * magic, NCount valid) const;
    private:
        static bool _is_initialised;
        ILenum mIlType;
    };
}
#endif