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
#include "NiiPVRTCCodec.h"
#include "NiiImage.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiException.h"
#include "NiiStrConv.h"

#define PVR_TEXTURE_FLAG_TYPE_MASK 0xff

namespace NII
{
#if N_COMPILER == N_CPP_MSVC
#pragma pack (push, 1)
#else
#pragma pack (1)
#endif
    //-----------------------------------------------------------------
    const Nui32 PVR_MAGIC = NII_MAGIC4('P', 'V', 'R', '!');
    //-----------------------------------------------------------------
    enum
    {
        kPVRTextureFlagTypePVRTC_2 = 24,
        kPVRTextureFlagTypePVRTC_4
    };
    //-----------------------------------------------------------------
    typedef struct _PVRTCTexHeader
    {
        Nui32 headerLength;
        Nui32 height;
        Nui32 width;
        Nui32 numMipmaps;
        Nui32 flags;
        Nui32 dataLength;
        Nui32 bpp;
        Nui32 bitmaskRed;
        Nui32 bitmaskGreen;
        Nui32 bitmaskBlue;
        Nui32 bitmaskAlpha;
        Nui32 pvrTag;
        Nui32 numSurfs;
    } PVRTCTexHeader;

#if N_COMPILER == N_CPP_MSVC
    #pragma pack(pop)
#else
    #pragma pack()
#endif
    //---------------------------------------------------------------------
    PVRTCCodec::PVRTCCodec() :
        ImageCodec(N_IMAGE_PVRT, "pvr")
    {
    }
    //---------------------------------------------------------------------
    PVRTCCodec::~PVRTCCodec()
    {
    }
    //---------------------------------------------------------------------
    CodecResult PVRTCCodec::decode(DataStream * in, Image * out) const
    {
        PVRTCTexHeader header;
        Nui32 flags = 0;
        Nui32 pvrTag = 0;
        Nui32 formatFlags = 0;
        NCount numFaces = 1; // Assume one face until we know otherwise

        // Read the PVRTC header
        in->read(&header, sizeof(PVRTCTexHeader));

        // Get the file type identifier
        pvrTag = header.pvrTag;

        if(PVR_MAGIC != pvrTag)
        {
            return CodecResult_NotMatch;
        }

        // Get format flags
        flags = header.flags;
        flipL((void *)flags, sizeof(Nui32), 1);
        formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;

        Nui32 bitmaskAlpha = header.bitmaskAlpha;
        flipL((void *)bitmaskAlpha, sizeof(Nui32), 1);

        if(formatFlags == kPVRTextureFlagTypePVRTC_4 ||
            formatFlags == kPVRTextureFlagTypePVRTC_2)
        {
            if(formatFlags == kPVRTextureFlagTypePVRTC_4)
            {
                out->mFormat = bitmaskAlpha ? PF_PVRTC_RGBA_4BPP : PF_PVRTC_RGB_4BPP;
            }
            else if(formatFlags == kPVRTextureFlagTypePVRTC_2)
            {
                out->mFormat = bitmaskAlpha ? PF_PVRTC_RGBA_2BPP : PF_PVRTC_RGB_2BPP;
            }

            out->mDepth = 1;
            out->mWidth = header.width;
            out->mHeight = header.height;
            out->mMipmapCount = header.numMipmaps;

            // PVRTC is a compressed format
            out->mFlags |= M_Compress;
        }

        // Calculate total size from number of mipmaps, faces and size
        NCount size = PixelUtil::getSize(out->mWidth, out->mHeight, out->mDepth, numFaces, out->mFormat, out->mMipmapCount);

        out->mSize = size;
        out->mBuffer = N_alloc_t(Nui8, size);
        in->read(out->mBuffer, size);
        return CodecResult_Success;
    }
    //---------------------------------------------------------------------
    bool PVRTCCodec::match(const String & hint) const
    {
        return false;
    }
    //---------------------------------------------------------------------
    String PVRTCCodec::match(const Nui8 * magic, NCount valid) const
    {
        if(valid >= sizeof(Nui32))
        {
            Nui32 fileType;
            memcpy(&fileType, magic, sizeof(Nui32));
            flipL(&fileType, sizeof(Nui32), 1);

            if (PVR_MAGIC == fileType)
            {
                return String("pvr");
            }
        }
        return StrUtil::BLANK;
    }
    //----------------------------------------------------------------------
}