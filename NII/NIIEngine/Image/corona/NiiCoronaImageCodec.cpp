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
#include "NiiCoronaImageCodec.h"

#include <corona.h>

namespace NII
{
    //------------------------------------------------------------------------
    CoronaImageCodec::CoronaImageCodec()
    {
        mCID = N_IMAGE_NONE;
        corona::FileFormatDesc** formats = corona::GetSupportedReadFormats();
        for (NCount i = 0 ; formats[i] ; ++i)
        {
            for (NCount j = 0 ; j < formats[i]->getExtensionCount() ; ++j)
            {
                mType += formats[i]->getExtension(j);
                mType += "|";
            }
        }
    }
    //------------------------------------------------------------------------
    CodecResult CoronaImageCodec::decode(DataStream * in, Image * out) const
    {
        corona::File * texFile = corona::CreateMemoryFile(in.getData(), (int)in.size());
        if(texFile == 0)
        {
            return CodecResult_Error;
        }
        corona::Image * texImg = corona::OpenImage(texFile);
        delete texFile;
        if(texImg == 0)
        {
            return CodecResult_Error;
        }
        PixelFormat pf;
        switch(texImg->getFormat())
        {
        case corona::PF_R8G8B8:
            texImg = corona::ConvertImage(texImg, corona::PF_R8G8B8);
            pf = PF_B8G8R8UN;
            break;
        default:
            texImg = corona::ConvertImage(texImg, corona::PF_R8G8B8A8ui);
            pf = PF_A8B8G8R8;
            break;
        }
        texImg = corona::ConvertImage(texImg, corona::PF_R8G8B8A8ui);
        if (texImg == 0)
        {
            return CodecResult_Error;
        }
        NCount size = PixelUtil::getSize(texImg->getWidth(), texImg->getHeight(), 1, 1, pf)
        Nui8 * dest = N_alloc_t(Nui8, size);
        memcpy(dest, texImg->getPixels(), size);
        out->load(dest, texImg->getWidth(), texImg->getHeight(), pf, true);

        delete texImg;

        return CodecResult_Success;
    }
    //------------------------------------------------------------------------
}