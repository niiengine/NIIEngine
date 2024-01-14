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

#ifndef _NII_STB_IMAGECODEC_H_
#define _NII_STB_IMAGECODEC_H_

#include "NiiImageCodec.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineInner STBImageCodec : public ImageCodec
    {
    public:
        STBImageCodec();

        /// @copydetails ImageCodec::decode
        void STBImageCodec::decode(DataStream * in, Image * out);
    };
}
#endif