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
#include "NiiILCodecs.h"
#include "NiiILImageCodec.h"
#include "NiiImage.h"
#include "NiiCodec.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include <IL/il.h>
#include <IL/ilu.h>

namespace NII
{
    //---------------------------------------------------
    std::list<ILImageCodec*> ILCodecs::codeclist;
    //---------------------------------------------------
    // Return IL type for a certain extension
    ILenum ILCodecs::getType(const String & ext, CodecID & cid)
    {
#ifdef IL_TGA
        if(ext=="tga" || ext=="vda" || ext=="icb" || ext=="vst")
        {
            cid = N_IMAGE_TGA;
            return IL_TGA;
        }
#endif
#ifdef IL_JPG
        if(ext=="jpg" || ext=="jpe" || ext=="jpeg")
        {
            cid = N_IMAGE_JPEG;
            return IL_JPG;
        }
#endif
/*#ifdef IL_DDS
        if (ext=="dds")
            return IL_DDS;
#endif*/
#ifdef IL_PNG
        if(ext=="png")
        {
            cid = N_IMAGE_PNG;
            return IL_PNG;
        }
#endif
#ifdef IL_BMP
        if(ext=="bmp" || ext=="dib")
        {
            cid = N_IMAGE_BMP;
            return IL_BMP;
        }
#endif
#ifdef IL_GIF
        if(ext=="gif")
        {
            cid = N_IMAGE_GIF;
            return IL_GIF;
        }
#endif
#ifdef IL_CUT
        if(ext=="cut")
        {
            cid = N_IMAGE_CUT;
            return IL_CUT;
        }
#endif
#ifdef IL_HDR
        if (ext=="hdr")
        {
            cid = N_IMAGE_HDR;
            return IL_HDR;
        }
#endif
#ifdef IL_ICO
        if(ext=="ico" || ext=="cur")
        {
            cid = N_IMAGE_ICO;
            return IL_ICO;
        }
#endif
#ifdef IL_JNG
        if(ext=="jng")
        {
            cid = N_IMAGE_JNG;
            return IL_JNG;
        }
#endif
#ifdef IL_LIF
        if(ext=="lif")
        {
            cid = N_IMAGE_LIF;
            return IL_LIF;
        }
#endif
#ifdef IL_MDL
        if(ext=="mdl")
        {
            cid = N_IMAGE_MDL;
            return IL_MDL;
        }
#endif
#ifdef IL_MNG
        if(ext=="mng" || ext=="jng")
        {
            cid = N_IMAGE_MNG;
            return IL_MNG;
        }
#endif
#ifdef IL_PCD
        if (ext=="pcd")
        {
            cid = N_IMAGE_PCD;
            return IL_PCD;
        }
#endif
#ifdef IL_PCX
        if (ext=="pcx")
        {
            cid = N_IMAGE_PCX;
            return IL_PCX;
        }
#endif
#ifdef IL_PIC
        if (ext=="pic")
        {
            cid = N_IMAGE_PIC;
            return IL_PIC;
        }
#endif
#ifdef IL_PIX
        if (ext=="pix")
        {
            cid = N_IMAGE_PIX;
            return IL_PIX;
        }
#endif
#ifdef IL_PNM
        if (ext=="pbm" || ext=="pgm" ||
            ext=="pnm" || ext=="ppm")
        {
            cid = N_IMAGE_PNM;
            return IL_PNM;
        }
#endif
#ifdef IL_PSD
        if (ext=="psd" || ext=="pdd")
        {
            cid = N_IMAGE_PSD;
            return IL_PSD;
        }
#endif
#ifdef IL_PSP
        if (ext=="psp")
        {
            cid = N_IMAGE_PSP;
            return IL_PSP;
        }
#endif
#ifdef IL_PXR
        if (ext=="pxr")
        {
            cid = N_IMAGE_PXR;
            return IL_PXR;
        }
#endif
#ifdef IL_SGI
        if (ext=="sgi" || ext=="bw" ||
            ext=="rgb" || ext=="rgba")
        {
            cid = N_IMAGE_SGI;
            return IL_SGI;
        }
#endif
#ifdef IL_TIF
        if (ext=="tif" || ext=="tiff")
        {
            cid = N_IMAGE_TIFF;
            return IL_TIF;
        }
#endif
#ifdef IL_WAL
        if (ext=="wal")
        {
            cid = N_IMAGE_WAL;
            return IL_WAL;
        }
#endif
#ifdef IL_XPM
        if (ext=="xpm")
        {
            cid = N_IMAGE_XPM;
            return IL_XPM;
        }
#endif
        cid = N_IMAGE_NONE;
        return IL_TYPE_UNKNOWN;
    }
    //--------------------------------------------------------------
    void ILCodecs::registerCodecs()
    {
        const NIIb * il_version = ilGetString(IL_VERSION_NUM);
        if(ilGetError() != IL_NO_ERROR)
        {
            // IL defined the version number as IL_VERSION in older versions, so we have to scan for it
            for(NIIi ver = 150; ver < 170; ++ver)
            {
                il_version = ilGetString(ver);
                if(ilGetError() == IL_NO_ERROR)
                    break;
                else
                    il_version = "Unknown";
            }
        }
        N_Only(Log).log(LML_NORMAL, _T("第三方库 DevIL 版本: ") + String(il_version));
        const NIIb * il_extensions = ilGetString(IL_LOAD_EXT);
        if(ilGetError() != IL_NO_ERROR)
            il_extensions = "";

        std::stringstream ext;
        String str, all;
        ext << il_extensions;
        while(ext >> str)
        {
#if 0
            String fileName = "dummy." + str;
            // DevIL doesn't export this under windows -- how weird
            NIIi ilType = ilTypeFromext(const_cast<NIIb *>(fileName.c_str()));
#endif
            CodecID cid;
            ILenum ilType = getType(str, cid);
            if(ilType != IL_TYPE_UNKNOWN)
            {
                ILImageCodec * codec = new ILImageCodec(cid, str, ilType);
                Codec::add(codec);
                codeclist.push_back(codec);
                //all += str+String("(")+StrConv::conv(ilType)+String(") ");
                all += str + String(" ");
            }
        }
        // Raw format, missing in image formats string
        ILImageCodec * cod = new ILImageCodec(N_IMAGE_RAW, "raw", IL_RAW);
        Codec::add(cod);
        codeclist.push_back(cod);
        //all += String("raw")+"("+StrConv::conv(IL_RAW)+String(") ");
        all += String("raw ");
        N_Only(Log).log( LML_NORMAL, _T("DevIL image formats: ") + all);
    }
    //--------------------------------------------------------------
    void ILCodecs::deleteCodecs()
    {
        std::list<ILImageCodec*>::const_iterator i, iend = codeclist.end();
        for(i = codeclist.begin(); i != iend; ++i)
        {
            Codec::remove(*i);
            delete *i;
        }
        codeclist.clear();
    }
    //--------------------------------------------------------------
}