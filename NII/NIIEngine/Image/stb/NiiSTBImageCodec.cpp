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
#include "NiiSTBImageCodec.h"
#define STBI_HEADER_FILE_ONLY
#include "stb_image.cpp"

namespace NII
{
    //------------------------------------------------------------------------
    STBImageCodec::STBImageCodec() :
        ImageCodec(N_IMAGE_NONE, "tga|jpg|png|psd|bmp|hdr")
    {
    }
    //------------------------------------------------------------------------
    STBImageCodec::~STBImageCodec()
    {
    }
    //------------------------------------------------------------------------
    void STBImageCodec::decode(DataStream * in, Image * out)
    {
        int width, height, comp;
        NCount size;
        // load image
        unsigned char * data = stbi_load_from_memory(in.getData(),
            in.size(), &width, &height, &comp, 0);

        if(!data)
        {
            return CodecResult_Error;
        }

        PixelFormat format;
        switch(comp)
        {
        case 4:
            format = PF_A8B8G8R8;
            break;
        case 3:
            format = PF_B8G8R8UN;
            break;
        default:
            stbi_image_free(data);
            return CodecResult_Error;
        }
        size = PixelUtil::getSize(width, height, 1, 1, format)
        Nui8 * dest = N_alloc_t(Nui8, size);
        memcpy(dest, data, size);
        out->load(dest, width, height, format, true);

        stbi_image_free(data);
        return CodecResult_Success;
    }
    //------------------------------------------------------------------------
}