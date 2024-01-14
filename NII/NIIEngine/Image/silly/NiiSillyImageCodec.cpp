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
#include "NiiException.h"
#include "NiiSillyImageCodec.h"
#include <SILLY.h>

namespace NII
{
    //------------------------------------------------------------------------
    SILLYImageCodec::SILLYImageCodec():
        ImageCodec(N_IMAGE_NONE, "tga|jpg|png")
    {
        if(! SILLY::SILLYInit())
            N_EXCEPT(Generic,_T("Unable to initialize SILLY library"));

    }
    //------------------------------------------------------------------------
    SILLYImageCodec::~SILLYImageCodec()
    {
        SILLY::SILLYCleanup();
    }
    //------------------------------------------------------------------------
    CodecResult SILLYImageCodec::decode(DataStream * in, Image * out) const
    {
        SILLY::MemoryDataSource md(static_cast<const SILLY::byte*>(in.getData()), in.size());
        SILLY::Image img(md);
        if(!img.loadImageHeader())
        {
            return CodecResult_Error;
        }

        SILLY::PixelFormat dstfmt;
        PixelFormat cefmt;
        switch (img.getSourcePixelFormat())
        {
        case SILLY::PF_RGB:
            dstfmt = SILLY::PF_RGB;
            cefmt = PF_B8G8R8UN;
            break;
        case SILLY::PF_A8B8G8R8:
        case SILLY::PF_A1B5G5R5:
            dstfmt = SILLY::PF_A8B8G8R8;
            cefmt = PF_A8B8G8R8;
            break;
        default:
            return CodecResult_NotMatch;
        }

        if (!img.loadImageData(dstfmt, SILLY::PO_TOP_LEFT))
        {
            return CodecResult_Error;
        }
        NCount size = PixelUtil::getSize(img.getWidth(), img.getHeight(), 1, 1, cefmt)
        Nui8 * dest = N_alloc_t(Nui8, size);
        memcpy(dest, img.getPixelsDataPtr(), size);
        out->load(dest, img.getWidth(), img.getHeight(), cefmt, true);
        return CodecResult_Success;
    }
    //------------------------------------------------------------------------
}