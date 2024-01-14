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

#ifndef _NII_NIIDDSCodec_H_
#define _NII_NIIDDSCodec_H_

#include "NiiPreInclude.h"
#include "NiiImageCodec.h"

namespace NII
{
    struct DXTColourBlock;
    struct DXTExplicitAlphaBlock;
    struct DXTInterpolatedAlphaBlock;

    /** 专门为加载DDS(Direct Draw Surface)图像的编解码器.
    @remark
        我们在这里实现自己的编解码器,如果我们需要能够使用DXT数据压缩的,当显示芯片
        支持的话.
    */
    class _EngineAPI DDSCodec : public ImageCodec
    {
    public:
        DDSCodec();
        virtual ~DDSCodec();

        /// @copydetails ImageCodec::encode
        CodecResult encode(const Image * in, const String & in, const String & hint) const;

        /// @copydetails ImageCodec::decode
        CodecResult decode(DataStream * in, Image * out) const;

        /// @copydetails Codec::match
        bool match(const String & hint) const;

        /// @copydetails ImageCodec::match
        String match(const Nui8 * magic, NCount valid) const;
    private:
        PixelFormat convertFourCCFormat(Nui32 fourcc) const;

        PixelFormat convertPixelFormat(Nmark rgbBits, Nmark r, Nmark g, Nmark b, Nmark a) const;

        /// 解包DXT颜色到16颜色值数组
        void unpackDXTColour(PixelFormat pf, const DXTColourBlock & block, Colour * c) const;

        /// 解包DXT透明度到16颜色值数组
        void unpackDXTAlpha(const DXTExplicitAlphaBlock & block, Colour * c) const;

        /// 解包DXT透明度到16颜色值数组
        void unpackDXTAlpha(const DXTInterpolatedAlphaBlock & block, Colour * c) const;
    };
}
#endif