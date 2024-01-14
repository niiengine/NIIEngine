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
#ifndef _NII_TGA_IMAGE_CODEC_H_
#define _NII_TGA_IMAGE_CODEC_H_

#include "NiiImageCodec.h"

namespace NII
{
    /** TGA
    @version NIIEngine 3.0.0
    */
    class _EngineInner TGAImageCodec : public ImageCodec
    {
    public:
        TGAImageCodec();

        /// @copydetails ImageCodec::decode
        CodecResult decode(DataStream * in, Image * out) const;
    protected:
        /// vertically flip data for tImageTGA 'img'
        void flipVertImageTGA(ImageTGA * img);

        /// horizontally flip data for tImageTGA 'img'
        void flipHorzImageTGA(ImageTGA * img);

        ImageTGA * loadTGA(const unsigned char* buffer, NCount buffer_size);
    };
}
#endif