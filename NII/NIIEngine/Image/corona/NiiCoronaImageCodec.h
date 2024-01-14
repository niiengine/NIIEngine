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

#ifndef _NII_CORONA_IMAGECODEC_H_
#define _NII_CORONA_IMAGECODEC_H_

#include "NiiImageCodec.h"

namespace NII
{
    /** 第三方库Corona专用
    @version NIIEngine 3.0.0
    */
    class _EngineInner CoronaImageCodec : public ImageCodec
    {
    public:
        CoronaImageCodec();

        /// @copydetails ImageCodec::decode
        CodecResult decode(DataStream * in, Image * out) const;

    };

}
#endif