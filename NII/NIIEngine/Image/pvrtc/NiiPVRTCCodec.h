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

#ifndef _NII_NiiPVRTCCodec_H_
#define _NII_NiiPVRTCCodec_H_

#include "NiiPreInclude.h"
#include "NiiCodec.h"

namespace NII
{
    /** 专们为载入PVRTC(PowerVR的)图像的编解码器
    */
    class _EngineAPI PVRTCCodec : public ImageCodec
    {
    public:
        PVRTCCodec();

        virtual ~PVRTCCodec();

        /// @copydetails ImageCodec::decode
        CodecResult decode(DataStream * in, Image * out) const
        
        /// @copydetails Codec::match
        bool match(const String & hint) const;

        /// @copydetails Codec::match
        String match(const Nui8 * magic, NCount valid) const;
    };
}
#endif